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

#include "encoder.hpp"

#include "../version.hpp"

#include <pybind11/pybind11.h>

#include <kodo/slide/encoder.hpp>
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
struct encoder_wrapper : kodo::slide::encoder
{
    encoder_wrapper(kodo::finite_field field) : kodo::slide::encoder(field)
    {
    }
    std::function<void(const std::string&)> m_log_callback;
    kodo::slide::stream<pybind11::object> m_stream;
};
using encoder_type = encoder_wrapper;

void slide_encoder_enable_log(encoder_type& encoder,
                              std::function<void(const std::string&)> callback)
{
    encoder.m_log_callback = callback;
    encoder.enable_log(
        [](const std::string& message, void* data) {
            encoder_type* encoder = static_cast<encoder_type*>(data);
            assert(encoder->m_log_callback);
            encoder->m_log_callback(message);
        },
        &encoder);
}

void slide_encoder_configure(encoder_type& encoder, std::size_t symbol_bytes)
{
    encoder.m_stream.reset();
    encoder.configure(symbol_bytes);
}

void slide_encoder_reset(encoder_type& encoder)
{
    encoder.m_stream.reset();
    encoder.reset();
}

auto slide_encoder_push_front_symbol(encoder_type& encoder,
                                     pybind11::object symbol_handle)
    -> std::size_t
{
    PyObject* symbol_obj = symbol_handle.ptr();

    if (!PyByteArray_Check(symbol_obj))
    {
        throw pybind11::type_error("symbol: expected type bytearray");
    }

    if ((std::size_t)PyByteArray_Size(symbol_obj) > encoder.symbol_bytes())
    {
        throw pybind11::value_error(
            "symbol: greater than encoder.symbol_bytes(). Must be less than or "
            "equal");
    }
    auto index = encoder.m_stream.push_front(symbol_handle);
    assert(encoder.m_stream.in_stream(index));
    (void)index;
    return encoder.push_front_symbol((uint8_t*)PyByteArray_AsString(symbol_obj),
                                     PyByteArray_Size(symbol_obj));
}

auto slide_encoder_symbol_at(encoder_type& encoder, std::size_t index)
{
    if (!encoder.m_stream.in_stream(index))
        throw pybind11::value_error("index not in stream");

    return encoder.m_stream.at(index);
}

auto slide_encoder_in_stream(encoder_type& encoder, std::size_t index)
{
    return encoder.m_stream.in_stream(index);
}

auto slide_encoder_pop_back_symbol(encoder_type& encoder)
{
    if (encoder.m_stream.is_empty())
        throw pybind11::value_error("Stream was empty");
    encoder.m_stream.pop_back();
    return encoder.pop_back_symbol();
}

auto slide_encoder_encode_symbol(encoder_type& encoder,
                                 pybind11::handle symbol_handle,
                                 pybind11::handle coefficients_handle)
    -> std::size_t
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

    if ((std::size_t)PyByteArray_Size(symbol_obj) > encoder.symbol_bytes())
    {
        throw pybind11::value_error("symbol: bytearray too large. Must be less "
                                    "than or equal to Encoder.symbol_bytes");
    }

    if ((std::size_t)PyByteArray_Size(coefficients_obj) == 0)
    {
        throw pybind11::value_error("coefficients: length is 0.");
    }

    return encoder.encode_symbol(
        (uint8_t*)PyByteArray_AsString(symbol_obj),
        (uint8_t*)PyByteArray_AsString(coefficients_obj));
}

auto slide_encoder_encode_systematic_symbol(encoder_type& encoder,
                                            pybind11::handle symbol_handle,
                                            std::size_t index) -> std::size_t
{
    PyObject* symbol_obj = symbol_handle.ptr();

    if (!PyByteArray_Check(symbol_obj))
    {
        throw pybind11::type_error("symbol: expected type bytearray");
    }

    if ((std::size_t)PyByteArray_Size(symbol_obj) > encoder.symbol_bytes())
    {
        throw pybind11::value_error("symbol: bytearray too large. Must be less "
                                    "than or equal to Encoder.symbol_bytes");
    }

    if (!encoder.m_stream.in_stream(index))
    {
        throw pybind11::value_error("index: out of range, must be in stream.");
    }

    return encoder.encode_systematic_symbol(
        (uint8_t*)PyByteArray_AsString(symbol_obj), index);
}
}

void encoder(pybind11::module& m)
{
    using namespace pybind11;
    class_<encoder_type>(m, "Encoder", "The Kodo sliding window encoder")
        .def(init<kodo::finite_field>(), arg("field"),
             "The sliding window encoder constructor\n\n"
             "\t:param field: the chosen finite field.\n")
        .def("configure", &slide_encoder_configure, arg("symbol_bytes"),
             "Configures the encoder with the given parameters. This must"
             "be called before anything else. If needed configure can be"
             "called again. This is useful for reusing an existing coder."
             "Note that the a reconfiguration always implies a reset,"
             "so the coder will be in a clean state after the operation\n\n"
             "\t:param symbol_bytes: The size of a symbol in bytes.\n")
        .def("reset", &slide_encoder_reset, "Reset the state of the encoder.\n")
        .def_property_readonly("field", &encoder_type::field,
                               "Return the finite field used.\n")
        .def_property_readonly(
            "symbol_bytes", &encoder_type::symbol_bytes,
            "Return the maximum size of a symbol in the stream in bytes.\n")
        .def_property_readonly(
            "stream_symbol_bytes", &encoder_type::stream_symbol_bytes,
            "Return the current maximum number of bytes needed"
            "for an encoded symbol.\n")
        .def_property_readonly(
            "stream_symbols", &encoder_type::stream_symbols,
            "Return the total number of symbols available in memory at the "
            "encoder."
            "The number of symbols in the coding window MUST be less than"
            "or equal to this number.\n")
        .def_property_readonly(
            "is_stream_empty", &encoder_type::is_stream_empty,
            "Return True if Encoder.stream_symbols == 0. Else False.\n")
        .def_property_readonly(
            "stream_lower_bound", &encoder_type::stream_lower_bound,
            "Return the index of the oldest symbol known by the encoder. This "
            "symbol"
            "may not be inside the window but can be included in the window"
            "if needed.\n")
        .def_property_readonly("stream_upper_bound",
                               &encoder_type::stream_upper_bound,
                               "Return the upper bound of the stream.\n")
        .def("push_front_symbol", &slide_encoder_push_front_symbol,
             arg("symbol"),
             "Adds a new symbol to the front of the encoder. Increments the "
             "number"
             "of symbols in the stream and increases the"
             "Encoder.stream_upper_bound\n\n"
             ":param symbol: The buffer containing all the data for the symbol"
             "Return the stream index of the symbol being added.\n")
        .def("pop_back_symbol", &slide_encoder_pop_back_symbol,
             "Remove the \"oldest\" symbol from the stream. Increments the"
             "Encoder.stream_lower_bound\n\n"
             "Return the index of the symbol being removed.\n")
        .def_property_readonly(
            "window_symbols", &encoder_type::window_symbols,
            "Return the number of symbols currently in the coding window. The"
            "window must be within the bounds of the stream.\n")
        .def_property_readonly(
            "window_lower_bound", &encoder_type::window_lower_bound,
            "Return the index of the \"oldest\" symbol in the coding window.\n")
        .def_property_readonly("window_upper_bound",
                               &encoder_type::window_upper_bound,
                               "Return the upper bound of the window. The "
                               "range of valid symbol indices"
                               "is range(Encoder.window_lower_bound, "
                               "Encoder.window_upper_bound).\n\n"
                               "Note that Encoder.window_upper_bound - 1 is "
                               "the largest number in this range.\n")
        .def("set_window", &encoder_type::set_window, arg("lower_bound"),
             arg("symbols"),
             "The window represents the symbols which will be included in the "
             "next"
             "encoding. The window cannot exceed the bound of the stream.\n\n"
             ":param lower_bound: Sets the index of the oldest symbol in the "
             "window\n"
             ":param symbols: Sets the number of symbols within the window.")
        .def(
            "encode_symbol", &slide_encoder_encode_symbol, arg("symbol"),
            arg("coefficients"),
            "Write an encoded symbol according to the coding coefficients.\n\n"
            ":param symbol: The buffer where the encoded symbol will be "
            "stored. The"
            "\tsymbol must be Encoder.symbol_bytes large."
            ":param coefficients: The coding coefficients. These must have the"
            "\tmemory layout required (see README.rst). A compatible format can"
            "\tbe created using Encoder.generate()."
            "Return The size of the output symbol in bytes i.e the number of"
            "\tbytes used from the symbol buffer.\n")
        .def("encode_systematic_symbol",
             &slide_encoder_encode_systematic_symbol, arg("symbol"),
             arg("index"),
             "Write a source symbol to the symbol buffer.\n\n"
             ":param symbol: The buffer where the source symbol will be "
             "stored. The"
             "\tsymbol buffer must be Encoder.symbol_bytes large"
             ":param index: The symbol index which should be written."
             "Return The size of the output symbol in bytes i.e the number of"
             "\tbytes used from the symbol buffer.")
        .def("symbol_at", &slide_encoder_symbol_at, arg("index"),
             "Get a symbol from the stream.\n\n"
             ":param index: The index of the symbol to get.")
        .def("in_stream", &slide_encoder_in_stream, arg("index"),
             "Check if a symbol is contained in the stream.\n\n"
             ":param index: The index of the symbol to check.")
        .def("enable_log", &slide_encoder_enable_log, arg("callback"),
             "Enable logging for this encoder.\n\n"
             ":param callback: The callback used for handling "
             "log messages.")
        .def("disable_log", &encoder_type::disable_log, "Disables the log.\n")
        .def("is_log_enabled", &encoder_type::is_log_enabled,
             "Return True if log is enabled, otherwise False.\n");
}
}
}
}
