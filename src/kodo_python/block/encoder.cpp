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

#include <kodo/block/encoder.hpp>

#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

namespace kodo_python
{
inline namespace STEINWURF_KODO_PYTHON_VERSION
{
namespace block
{

struct encoder_wrapper : kodo::block::encoder
{
    encoder_wrapper(kodo::finite_field field) : kodo::block::encoder(field)
    {
    }
    std::function<void(const std::string&)> m_log_callback;
};

using encoder_type = encoder_wrapper;

void block_encoder_enable_log(encoder_type& encoder,
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

void block_encoder_encode_symbol(encoder_type& encoder,
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

    if ((std::size_t)PyByteArray_Size(symbol_obj) < encoder.symbol_bytes())
    {
        throw pybind11::value_error(
            "symbol: not large enough to contain symbol");
    }

    encoder.encode_symbol((uint8_t*)PyByteArray_AsString(symbol_obj),
                          (uint8_t*)PyByteArray_AsString(coefficients_obj));
}

void block_encoder_encode_systematic_symbol(encoder_type& encoder,
                                            pybind11::handle storage_handle,
                                            std::size_t index)
{
    PyObject* symbol_obj = storage_handle.ptr();

    if (!PyByteArray_Check(symbol_obj))
    {
        throw pybind11::type_error("symbol: expected type bytearray");
    }

    if (index >= encoder.symbols())
    {
        throw pybind11::value_error("index: must be less than symbols");
    }

    encoder.encode_systematic_symbol((uint8_t*)PyByteArray_AsString(symbol_obj),
                                     index);
}

void block_encoder_set_symbols_storage(encoder_type& encoder,
                                       pybind11::handle symbols_storage_handle)
{
    PyObject* symbols_storage_obj = symbols_storage_handle.ptr();

    if (!PyByteArray_Check(symbols_storage_obj))
    {
        throw pybind11::type_error("symbols_storage: expected type bytearray");
    }

    if (encoder.rank() != 0)
    {
        throw std::runtime_error("symbol storage must only be set once");
    }

    encoder.set_symbols_storage(
        (uint8_t*)PyByteArray_AsString(symbols_storage_obj));
}

void block_encoder_set_symbol_storage(encoder_type& encoder,
                                      pybind11::handle symbol_storage_handle,
                                      std::size_t index)
{
    PyObject* symbol_storage_obj = symbol_storage_handle.ptr();

    if (!PyByteArray_Check(symbol_storage_obj))
    {
        throw pybind11::type_error("symbol_storage: expected type bytearray");
    }

    if (index >= encoder.symbols())
    {
        throw pybind11::value_error("index: must be less than symbols");
    }

    if (encoder.is_symbol_set(index))
    {
        throw pybind11::value_error("index: symbols is already set");
    }

    encoder.set_symbol_storage(
        (uint8_t*)PyByteArray_AsString(symbol_storage_obj), index);
}

void encoder(pybind11::module& m)
{
    using namespace pybind11;
    class_<encoder_type>(m, "Encoder", "The Kodo block encoder")
        .def(init<kodo::finite_field>(), arg("field"),
             "The block encoder constructor\n\n"
             "\t:param field: the chosen finite field.\n")
        .def("configure", &encoder_type::configure, arg("symbols"),
             arg("symbol_bytes"),
             "Configure the encoder with the given parameters. This is also "
             "useful for reusing an existing coder. Note that the "
             "reconfiguration always implies a reset, so the encoder will be"
             "in a clean state after this operation.\n\n"
             "\t:param symbols: The number of symbols.\n"
             "\t:param symbol_bytes: The size of a symbol in bytes.")
        .def("reset", &encoder_type::reset, "Reset the state of the encoder.\n")
        .def_property_readonly(
            "symbols", &encoder_type::symbols,
            "Return the number of symbols supported by this encoder.\n")
        .def_property_readonly(
            "symbol_bytes", &encoder_type::symbol_bytes,
            "Return the size in bytes per symbol supported by this encoder.\n")
        .def_property_readonly("field", &encoder_type::field,
                               "Return the :class:`~kodo.FiniteField` used.\n")
        .def_property_readonly("block_bytes", &encoder_type::block_bytes,
                               "Return the total number of bytes that is "
                               "currently being encoded with this encoder.\n")
        .def_property_readonly("rank", &encoder_type::rank,
                               "Return the rank of the decoder.\n")
        .def("set_symbols_storage", &block_encoder_set_symbols_storage,
             arg("symbols_storage"),
             "Set the symbols to be encoded.\n\n"
             "\t:param symbols_storage: The buffer containing all the data for "
             "the block.\n")
        .def("set_symbol_storage", &block_encoder_set_symbol_storage,
             arg("symbol_storage"), arg("index"),
             "Set a symbol to be encoded.\n\n"
             "\t:param symbol_storage: The buffer containing all the data for "
             "the block.\n"
             "\t:param index: The index of the symbol.\n")
        .def("is_symbol_set", &encoder_type::is_symbol_set, arg("index"),
             "Checks if a symbol has been set.\n\n"
             "\t:param index: The index of the symbol to check.\n")
        .def("encode_symbol", &block_encoder_encode_symbol,
             arg("symbol_storage"), arg("coefficients"),
             "Create a new encoded symbol given the passed encoding "
             "coefficients.\n\n"
             "\t:param symbol_storage: The buffer containing all the data for "
             "the block.\n"
             "\t:param coefficients: The coding coefficients.\n")
        .def("encode_systematic_symbol",
             &block_encoder_encode_systematic_symbol, arg("symbol_storage"),
             arg("index"),
             "Creates a new systematic, i.e, un-coded symbol given the passed "
             "index.\n\n"
             "\t:param symbol_storage: The buffer containing all the data for "
             "the block.\n"
             "\t:param index: The index of the systematic symbol to produce.")
        .def(
            "enable_log", &block_encoder_enable_log, arg("callback"),
            "Enable logging for this encoder.\n\n"
            "\t:param callback: The callback used for handling log messages.\n")
        .def("disable_log", &encoder_type::disable_log, "Disables the log.\n")
        .def("is_log_enabled", &encoder_type::is_log_enabled,
             "Return True if log is enabled, otherwise False.\n");
}
}
}
}
