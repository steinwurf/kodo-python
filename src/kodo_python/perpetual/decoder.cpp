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

#include <kodo/perpetual/decoder.hpp>

#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

namespace kodo_python
{
inline namespace STEINWURF_KODO_PYTHON_VERSION
{
namespace perpetual
{

struct decoder_wrapper : kodo::perpetual::decoder
{
    decoder_wrapper(kodo::finite_field field) : kodo::perpetual::decoder(field)
    {
    }
    decoder_wrapper() : kodo::perpetual::decoder()
    {
    }

    std::function<void(const std::string&)> m_log_callback;
};

using decoder_type = decoder_wrapper;

void perpetual_decoder_enable_log(
    decoder_type& decoder, std::function<void(const std::string&)> callback)
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

void perpetual_decoder_set_symbols_storage(
    decoder_type& decoder, pybind11::handle symbols_storage_handle)
{
    PyObject* symbols_storage_obj = symbols_storage_handle.ptr();

    if (!PyByteArray_Check(symbols_storage_obj))
    {
        throw pybind11::type_error("symbols_storage: expected type bytearray");
    }

    decoder.set_symbols_storage(
        (uint8_t*)PyByteArray_AsString(symbols_storage_obj));
}

void perpetual_decoder_decode_symbol(decoder_type& decoder,
                                     pybind11::handle symbol_handle,
                                     pybind11::handle coefficients_handle,
                                     std::size_t offset)
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

    if ((std::size_t)PyByteArray_Size(symbol_obj) < decoder.symbol_bytes())
    {
        throw pybind11::value_error(
            "symbol: not large enough to contain symbol");
    }

    if (offset >= decoder.symbols())
    {
        throw pybind11::value_error("offset: must be less than symbols");
    }

    decoder.decode_symbol((uint8_t*)PyByteArray_AsString(symbol_obj),
                          (uint8_t*)PyByteArray_AsString(coefficients_obj),
                          offset);
}

void perpetual_decoder_save_state(decoder_type& decoder,
                                  pybind11::handle handle)
{
    PyObject* obj = handle.ptr();
    assert(PyByteArray_Check(obj) &&
           "The data should be a Python bytearray object");
    assert(((std::size_t)PyByteArray_Size(obj)) >= decoder.state_bytes());

    decoder.save_state((uint8_t*)PyByteArray_AsString(obj));
}

pybind11::object perpetual_decoder_restore_state(pybind11::handle data_handle)
{
    PyObject* data_obj = data_handle.ptr();
    assert(PyByteArray_Check(data_obj) &&
           "The data should be a Python bytearray object");
    decoder_type decoder;

    bool result = decoder.restore_state(
        (uint8_t*)PyByteArray_AsString(data_obj), PyByteArray_Size(data_obj));
    if (result)
    {
        return pybind11::cast(std::move(decoder));
    }
    else
    {
        return pybind11::none();
    }
}

void decoder(pybind11::module& m)
{
    using namespace pybind11;
    class_<decoder_type>(m, "Decoder", "The Kodo perpetual decoder.")
        .def(init<kodo::finite_field>(), arg("field"),
             "The perpetual decoder constructor\n\n"
             "\t:param field: the chosen finite field.\n")
        .def("configure", &decoder_type::configure, arg("block_bytes"),
             arg("symbol_bytes"), arg("width"),
             "configure the decoder with the given parameters. This is also "
             "useful for reusing an existing coder. Note that the "
             "reconfiguration always implies a reset, so the decoder will be "
             "in a clean state after this operation.\n\n"
             "\t:param block_bytes: The size of the block in bytes.\n"
             "\t:param symbol_bytes: The size of a symbol in bytes.\n"
             "\t:param width: The width of the encoding i.e. how many symbols "
             "are combined for each encoded symbol.\n")
        .def("reset", &decoder_type::reset, "Reset the state of the decoder.\n")
        .def_property_readonly("field", &decoder_type::field,
                               "Return the configured finite field.\n")
        .def_property_readonly(
            "symbols", &decoder_type::symbols,
            "Return the number of symbols supported by this decoder.\n")
        .def_property_readonly("symbol_bytes", &decoder_type::symbol_bytes,
                               "Return the size in bytes per the symbol "
                               "supported by this decoder.\n")
        .def_property_readonly(
            "width", &decoder_type::width,
            "Return the width of the encoding i.e. how many symbols are "
            "combined for each encoded symbol.\n")
        .def_property_readonly("block_bytes", &decoder_type::block_bytes,
                               "Return the maximum number of bytes that can be "
                               "decoded with this decoder.\n")
        .def("set_symbols_storage", &perpetual_decoder_set_symbols_storage,
             arg("symbol_storage"),
             "Initialize all the symbols in the block.\n\n"
             "\t:param symbol_storage: The buffer containing all the data for "
             "the block.\n")
        .def("symbols_storage", &decoder_type::symbols_storage,
             "Return the memory of the block.\n")
        .def("decode_symbol", &perpetual_decoder_decode_symbol, arg("symbol"),
             arg("coefficients"), arg("offset"),
             "Feed a coded symbol to the decoder.\n\n"
             "\t:param symbol: The data of the symbol assumed to be "
             "Decoder.symbol_bytes bytes in size.\n"
             "\t:param coefficients: The coding coefficients that describe the "
             "encoding performed on the symbol.\n"
             "\t:param offset: The offset of the coding coefficients.\n")
        .def("pivot_found", &decoder_type::pivot_found,
             "Return True if a new pivot was found during "
             "Decoder.decode_symbol().\n")
        .def_property_readonly("pivot", &decoder_type::pivot,
                               "Return index of the newest pivot found.\n")
        .def("is_pivot", &decoder_type::is_pivot, arg("index"),
             "Return True if the decoder contains a pivot at the specific "
             "index.\n\n"
             "\t:param index: The specific index to check.\n")
        .def_property_readonly("rank", &decoder_type::rank,
                               "Return the rank of the decoder.\n")
        .def("can_complete_decoding", &decoder_type::can_complete_decoding,
             "Return True if the decoder can complete the decoding, i.e, that "
             "the rank is full.\n")
        .def("complete_decoding", &decoder_type::complete_decoding,
             "Instruct the decoder to complete the decoding, this is only "
             "possible when Decoder.can_complete_decoding() returns true.\n")
        .def("is_complete", &decoder_type::is_complete,
             "Return True if the decoder is complete, when this is true the "
             "content stored in symbols_storage() is decoded.\n")
        .def("enable_log", &perpetual_decoder_enable_log, arg("callback"),
             "Enable logging for the decoder.\n\n"
             "\t:param callback: The callback used for handling messages.\n")
        .def("disable_log", &decoder_type::disable_log, "Disables the log.\n")
        .def("is_log_enabled", &decoder_type::is_log_enabled,
             "Return True if log is enabled, otherwise False.\n")
        .def("state_bytes", &decoder_type::state_bytes,
             "The number of bytes needed for storing the state.\n")
        .def("save_state", &perpetual_decoder_save_state,
             "Save the state of this decoder so that it can be restored later "
             "using restore_state.\n\n"
             "\t:param data: The data buffer writing the state, please make "
             "sure it is large enough to contain the number of bytes returned "
             "by Decoder.state_bytes().\n")
        .def_static(
            "restore_state", &perpetual_decoder_restore_state, arg("data"),
            "Return a decoder restored by the state of a past decoder."
            "\t:param data: The data buffer containing the state bytes.\n");
}
}
}
}
