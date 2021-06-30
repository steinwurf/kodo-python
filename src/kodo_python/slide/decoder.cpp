// License for Commercial Usage
// Distributed under the "KODO EVALUATION LICENSE 1.3"
//
// Licensees holding a valid commercial license may use this project
// in accordance with the standard license agreement terms provided
// with the Software (see accompanying file LICENSE.rst or
// https://www.steinwurf.com/license), unless otherwise different
// terms and conditions are agreed in writing between Licensee and
// Steinwurf ApS in which case the license will be regulated by that
// separate written agreement.
//
// License for Non-Commercial Usage
// Distributed under the "KODO RESEARCH LICENSE 1.2"
//
// Licensees holding a valid research license may use this project
// in accordance with the license agreement terms provided with the
// Software
//
// See accompanying file LICENSE.rst or https://www.steinwurf.com/license

#include "decoder.hpp"

#include "../version.hpp"

#include <pybind11/pybind11.h>

#include <kodo/slide/decoder.hpp>
#include <kodo/slide/stream.hpp>

#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

namespace kodo_python
{
inline namespace STEINWURF_KODO_PYTHON_VERSION
{
namespace slide
{
namespace
{
struct decoder_wrapper : kodo::slide::decoder
{
    decoder_wrapper(kodo::finite_field field) : kodo::slide::decoder(field)
    {
    }
    std::function<void(const std::string&)> m_log_callback;
    kodo::slide::stream<pybind11::object> m_stream;
};

using decoder_type = decoder_wrapper;

void slide_decoder_enable_log(decoder_type& decoder,
                              std::function<void(const std::string&)> callback)
{
    decoder.m_log_callback = callback;
    decoder.enable_log(
        [](const std::string& message, void* data) {
            decoder_type* decoder = static_cast<decoder_type*>(data);
            assert(decoder->m_log_callback);
            decoder->m_log_callback(message);
        },
        &decoder);
}

void slide_decoder_configure(decoder_type& decoder, std::size_t symbol_bytes)
{
    decoder.m_stream.reset();
    decoder.configure(symbol_bytes);
}

void slide_decoder_reset(decoder_type& decoder)
{
    decoder.m_stream.reset();
    decoder.reset();
}

auto slide_decoder_push_front_symbol(decoder_type& decoder,
                                     pybind11::object symbol_handle)
    -> std::size_t
{
    PyObject* symbol_obj = symbol_handle.ptr();

    if (!PyByteArray_Check(symbol_obj))
    {
        throw pybind11::type_error("symbol: expected type bytearray");
    }
    if (decoder.symbol_bytes() > (std::size_t)PyByteArray_Size(symbol_obj))
    {
        throw pybind11::value_error("symbol not large enough");
    }
    decoder.m_stream.push_front(symbol_handle);
    return decoder.push_front_symbol(
        (uint8_t*)PyByteArray_AsString(symbol_obj));
}

auto slide_decoder_pop_back_symbol(decoder_type& decoder)
{
    if (decoder.m_stream.is_empty())
        throw pybind11::value_error("Stream was empty");
    decoder.m_stream.pop_back();
    return decoder.pop_back_symbol();
}

auto slide_decoder_symbol_at(decoder_type& decoder, std::size_t index)
{
    if (!decoder.m_stream.in_stream(index))
        throw pybind11::value_error("index not in stream");

    return decoder.m_stream.at(index);
}

auto slide_decoder_in_stream(decoder_type& decoder, std::size_t index)
{
    return decoder.m_stream.in_stream(index);
}

void slide_decoder_decode_symbol(decoder_type& decoder,
                                 pybind11::handle symbol_handle,
                                 pybind11::handle coefficients_handle)
{
    PyObject* symbol_obj = symbol_handle.ptr();
    PyObject* coefficients_obj = coefficients_handle.ptr();

    if (!PyByteArray_Check(symbol_obj))
    {
        throw pybind11::type_error("symbol: expected type bytearray");
    }

    if (!PyByteArray_Check(coefficients_obj))
    {
        throw pybind11::type_error("coefficients: expected type bytearray");
    }

    if ((std::size_t)PyByteArray_Size(coefficients_obj) == 0)
    {
        throw pybind11::value_error("coefficients: length is 0.");
    }

    decoder.decode_symbol((uint8_t*)PyByteArray_AsString(symbol_obj),
                          PyByteArray_Size(symbol_obj),
                          (uint8_t*)PyByteArray_AsString(coefficients_obj));
}

void slide_decoder_decode_systematic_symbol(decoder_type& decoder,
                                            pybind11::handle symbol_handle,
                                            std::size_t index)
{
    PyObject* symbol_obj = symbol_handle.ptr();

    if (!PyByteArray_Check(symbol_obj))
    {
        throw pybind11::type_error("symbol: expected type bytearray");
    }

    if ((std::size_t)PyByteArray_Size(symbol_obj) > decoder.symbol_bytes())
    {
        throw pybind11::value_error("symbol: bytearray too large. Must be less "
                                    "than or equal to Encoder.symbol_bytes");
    }

    if (!decoder.m_stream.in_stream(index))
    {
        throw pybind11::value_error("index: out of range, must be in stream.");
    }

    decoder.decode_systematic_symbol((uint8_t*)PyByteArray_AsString(symbol_obj),
                                     PyByteArray_Size(symbol_obj), index);
}
}

void decoder(pybind11::module& m)
{
    using namespace pybind11;
    class_<decoder_type>(m, "Decoder", "The Kodo sliding window decoder")
        .def(init<kodo::finite_field>(), arg("field"),
             "The sliding window decoder constructor\n\n"
             "\t:param field: the chosen finite field.\n")
        .def("configure", &slide_decoder_configure, arg("symbol_bytes"),
             "Configure the decoder with the given parameters. This is also "
             "useful for reusing an existing coder. Note that the "
             "reconfiguration always implies a reset, so the decoder will be "
             "in a clean state after this operation.\n\n"
             "\t:param symbol_bytes: The size of a symbol in bytes.\n")
        .def("reset", &slide_decoder_reset, "Reset the state of the decoder.\n")
        .def_property_readonly("field", &decoder_type::field,
                               "Return the finite field used.\n")
        .def_property_readonly(
            "symbol_bytes", &decoder_type::symbol_bytes,
            "Return the size of a symbol in the stream in bytes.\n")
        .def_property_readonly(
            "stream_symbols", &decoder_type::stream_symbols,
            "Return the total number of symbols known at the "
            "decoder."
            "The number of symbols in the coding window MUST be less than"
            "or equal to this number.\n")
        .def_property_readonly(
            "is_stream_empty", &decoder_type::is_stream_empty,
            "Return True if Decoder.stream_symbols == 0. Else False.\n")
        .def_property_readonly(
            "stream_lower_bound", &decoder_type::stream_lower_bound,
            "Return the index of the oldest symbol known by the decoder. This "
            "symbol"
            "may not be inside the window but can be included in the window"
            "if needed.\n")
        .def_property_readonly("stream_upper_bound",
                               &decoder_type::stream_upper_bound,
                               "Return the upper bound of the stream.\n")
        .def("set_stream_lower_bound", &decoder_type::set_stream_lower_bound,
             arg("stream_lower_bound"),
             "Set the index of the oldest symbol known by the decoder."
             "This must only be called on a decoder with an empty stream.\n\n"
             ":param stream_lower_bound: The new stream lower bound, the new "
             "lower"
             "\tbound must be larger than or equal to the prior lower bound.\n")
        .def("push_front_symbol", &slide_decoder_push_front_symbol,
             arg("symbol"),
             "Adds a new symbol to the front of the decoder. Increments the "
             "number"
             "of symbols in the stream and increases the"
             "Decoder.stream_upper_bound\n\n"
             ":param symbol: The buffer containing all the data for the symbol"
             "Return the stream index of the symbol being added.\n")
        .def("pop_back_symbol", &slide_decoder_pop_back_symbol,
             "Remove the \"oldest\" symbol from the stream. Increments the"
             "Decoder.stream_lower_bound\n\n"
             "Return the index of the symbol being removed.\n")
        .def_property_readonly(
            "window_symbols", &decoder_type::window_symbols,
            "Return the number of symbols currently in the coding window. The"
            "window must be within the bounds of the stream.\n")
        .def_property_readonly(
            "window_lower_bound", &decoder_type::window_lower_bound,
            "Return the index of the \"oldest\" symbol in the coding window.\n")
        .def_property_readonly("window_upper_bound",
                               &decoder_type::window_upper_bound,
                               "Return the upper bound of the window. The "
                               "range of valid symbol indices"
                               "is range(Decoder.window_lower_bound, "
                               "Decoder.window_upper_bound).\n\n"
                               "Note that Decoder.window_upper_bound - 1 is "
                               "the largest number in this range.\n")
        .def("set_window", &decoder_type::set_window, arg("lower_bound"),
             arg("symbols"),
             "The window represents the symbols which will be included in the "
             "next"
             "encoding. The window cannot exceed the bound of the stream.\n\n"
             ":param lower_bound: Sets the index of the oldest symbol in the "
             "window\n"
             ":param symbols: Sets the number of symbols within the window.")
        .def(
            "decode_symbol", &slide_decoder_decode_symbol, arg("symbol"),
            arg("coefficients"),
            "Write an decoded symbol according to the coding coefficients.\n\n"
            ":param symbol: The buffer where the decoded symbol will be "
            "stored. The"
            "\tsymbol must be Decoder.symbol_bytes large."
            ":param coefficients: The coding coefficients. These must have the"
            "\tmemory layout required (see README.rst). A compatible format can"
            "\tbe created using Decoder.generate()."
            "Return The size of the output symbol in bytes i.e the number of"
            "\tbytes used from the symbol buffer.\n")
        .def("decode_systematic_symbol",
             &slide_decoder_decode_systematic_symbol, arg("symbol"),
             arg("index"),
             "Write a source symbol to the symbol buffer.\n\n"
             ":param symbol: The buffer where the source symbol will be "
             "stored. The"
             "\tsymbol buffer must be Decoder.symbol_bytes large"
             ":param index: The symbol index which should be written."
             "Return The size of the output symbol in bytes i.e the number of"
             "\tbytes used from the symbol buffer.")
        .def_property_readonly(
            "rank", &decoder_type::rank,
            "The rank of a decoder indicates how many symbols have been"
            "partially or fully decoded. This number is equivalent to the"
            "number of pivot elements in the stream.\n\n"
            "Return the rank of the decoder.\n")
        .def_property_readonly(
            "symbols_missing", &decoder_type::symbols_missing,
            "Return the number of missing symbols in the stream.\n")
        .def_property_readonly(
            "symbols_partially_decoded",
            &decoder_type::symbols_partially_decoded,
            "Return the number of partially decoded symbols in the stream.\n")
        .def_property_readonly(
            "symbols_decoded", &decoder_type::symbols_decoded,
            "Return the number of decoded symbols in the stream.\n")
        .def("symbol_at", &slide_decoder_symbol_at, arg("index"),
             "Get a symbol from the stream.\n\n"
             ":param index: The index of the symbol to get.")
        .def("in_stream", &slide_decoder_in_stream, arg("index"),
             "Check if a symbol is contained in the stream.\n\n"
             ":param index: The index of the symbol to check.")
        .def("is_symbol_decoded", &decoder_type::is_symbol_decoded,
             arg("index"),
             ":param index: The index of the symbol to check.\n\n"
             "Return True if the symbol is decoded (i.e it correspond to a "
             "source"
             "\tsymbol), otherwise False.\n")
        .def("enable_log", &slide_decoder_enable_log, arg("callback"),
             "Enable logging for this decoder.\n\n"
             ":param callback: The callback used for handling "
             "log messages.")
        .def("disable_log", &decoder_type::disable_log, "Disables the log.\n")
        .def("is_log_enabled", &decoder_type::is_log_enabled,
             "Return True if log is enabled, otherwise False.\n");
}
}
}
}
