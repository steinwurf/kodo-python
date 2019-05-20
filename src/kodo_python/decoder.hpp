// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <pybind11/pybind11.h>

#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

#include <storage/storage.hpp>

#include "coder.hpp"

namespace kodo_python
{
template<class Decoder>
void set_mutable_symbols(Decoder& decoder, pybind11::handle handle)
{
    PyObject* obj = handle.ptr();
    assert(PyByteArray_Check(obj) && "The symbol storage should be a "
           "Python bytearray object");

    auto symbol_storage = storage::mutable_storage(
        (uint8_t*)PyByteArray_AsString(obj),
        (uint32_t)PyByteArray_Size(obj));

    decoder.set_mutable_symbols(symbol_storage);
}

template<class Decoder>
void set_mutable_symbol(
    Decoder& decoder, uint32_t index, pybind11::handle handle)
{
    PyObject* obj = handle.ptr();
    assert(PyByteArray_Check(obj) && "The symbol storage should be a "
           "Python bytearray object");

    auto symbol = storage::mutable_storage(
        (uint8_t*)PyByteArray_AsString(obj),
        (uint32_t)PyByteArray_Size(obj));

    decoder.set_mutable_symbol(index, symbol);
}

template<class Decoder>
pybind11::handle decoder_write_payload(Decoder& decoder)
{
    std::vector<uint8_t> payload(decoder.payload_size());
    auto length = decoder.write_payload(payload.data());

    return PyByteArray_FromStringAndSize((char*)payload.data(), length);
}

template<class Decoder>
void read_payload(Decoder& decoder, pybind11::handle handle)

{
    PyObject* obj = handle.ptr();
    assert(PyByteArray_Check(obj) && "The payload buffer should be a "
           "Python bytearray object");

    decoder.read_payload((uint8_t*)PyByteArray_AsString(obj));
}

template<class Decoder>
void read_symbol(
    Decoder& decoder, pybind11::handle handle1, pybind11::handle handle2)
{
    PyObject* symbol_data = handle1.ptr();
    PyObject* coefficients = handle2.ptr();
    assert(PyByteArray_Check(symbol_data) && "The symbol buffer should be a "
           "Python bytearray object");
    assert(PyByteArray_Check(coefficients) && "The coefficients buffer should "
           "be a Python bytearray object");

    decoder.read_symbol(
        (uint8_t*)PyByteArray_AsString(symbol_data),
        (uint8_t*)PyByteArray_AsString(coefficients));
}

template<class Decoder>
void read_uncoded_symbol(
    Decoder& decoder, pybind11::handle handle, uint32_t index)
{
    PyObject* obj = handle.ptr();
    assert(PyByteArray_Check(obj) && "The symbol buffer should be a "
           "Python bytearray object");

    decoder.read_uncoded_symbol((uint8_t*)PyByteArray_AsString(obj), index);
}

template<class Coder>
struct extra_decoder_methods
{
    template<class DecoderClass>
    extra_decoder_methods(DecoderClass& decoder_class)
    {
        (void) decoder_class;
    }
};

template<class Coder>
void decoder(pybind11::module& m, const std::string& name)
{
    using namespace pybind11;

    using decoder_type = Coder;

    auto decoder_class =
        coder<Coder>(m, name)
        .def("rank", &decoder_type::rank,
             "Return the current rank of the decoder.\n\n"
             "The rank of a decoder indicates how many symbols have been "
             "decoded or partially decoded.\n\n"
             "\t:returns: The rank.\n")
        .def("read_payload", &read_payload<decoder_type>,
             arg("symbol_data"),
             "Decode the provided encoded payload.\n\n"
             "\t:param symbol_data: The encoded payload.\n")
        .def("read_symbol", &read_symbol<decoder_type>,
             arg("symbol_data"), arg("coefficients"),
             "Decode the provided encoded symbol with the provided coding "
             "coefficients.\n\n"
             "\t:param symbol_data: The encoded payload.\n"
             "\t:param coefficients: The coding coefficients.\n")
        .def("read_uncoded_symbol", &read_uncoded_symbol<decoder_type>,
             arg("symbol_data"), arg("index"),
             "Decode the provided systematic symbol.\n\n"
             "\t:param symbol_data: The systematic symbol.\n"
             "\t:param index: The index of this systematic symbol.\n")
        .def("is_complete", &decoder_type::is_complete,
             "Check whether decoding is complete.\n\n"
             "\t:returns: True if the decoding is complete.\n")
        .def("symbols_uncoded", &decoder_type::symbols_uncoded,
             "Returns the number of uncoded symbols currently known.\n\n"
             "Depending on the algorithm used the true number of uncoded\n"
             "symbols may be higher.\n"
             "The reason for this uncertainty is the some algorithms, for\n"
             "performance reasons, choose to not keep track of the exact\n"
             "status of the decoding matrix.\n"
             "It is however guaranteed that at least this amount of uncoded\n"
             "symbols exist.\n\n"
             "\t:returns: The number of symbols which have been uncoded.\n")
        .def("symbols_missing", &decoder_type::symbols_missing,
             "Return the number of missing symbols at the decoder.\n\n"
             "\t:returns: The number of missing symbols.\n")
        .def("symbols_partially_decoded",
             &decoder_type::symbols_partially_decoded,
             "Return the number of partially decoded symbols at the decoder.\n\n"
             "\t:returns: The number of partially decoded symbols.\n")
        .def("is_symbol_uncoded", &decoder_type::is_symbol_uncoded,
             arg("index"),
             "Check if the symbol at given index is uncoded.\n\n"
             "This may return false for symbols that are actually uncoded,\n"
             "but never true for symbols that are not uncoded.\n"
             "As with the symbols_uncoded() function the reason for this is\n"
             "that some algorithms do not, for performance reasons, keep track\n"
             "of the exact status of the decoding matrix.\n\n"
             "\t:param index: Index of the symbol to check.\n"
             "\t:return: True if the symbol is uncoded, and otherwise false.\n")
        .def("is_symbol_missing", &decoder_type::is_symbol_missing,
             arg("index"),
             "Check if the symbol at given index is missing.\n\n"
             "\t:param index: Index of the symbol to check.\n"
             "\t:return: True if the symbol is missing otherwise false.\n")
        .def("is_symbol_partially_decoded",
             &decoder_type::is_symbol_partially_decoded,
             arg("index"),
             "Check if the symbol at given index is partially decoded.\n\n"
             "\t:param index: Index of the symbol to check.\n"
             "\t:return: True if the symbol is partially decoded otherwise\n"
             "\t         false.\n")
        .def("is_symbol_pivot", &decoder_type::is_symbol_pivot,
             arg("index"),
             "The symbol pivot indicates whether a symbol is present in\n"
             "the decoding matrix (it could be partially or fully decoded).\n\n"
             "\t:param index: The index of the symbol.\n"
             "\t:returns: True if the symbol is available.\n")
        .def("set_mutable_symbols", &set_mutable_symbols<decoder_type>,
             arg("symbols"),
             "Set the buffer where the decoded symbols should be stored.\n\n"
             "\t:param symbols: The bytearray to store the symbols.\n")
        .def("set_mutable_symbol", &set_mutable_symbol<decoder_type>,
             arg("index"), arg("symbol"),
             "Set the storage for a single symbol.\n\n"
             "\t:param index: The index of the symbol in the coding block.\n"
             "\t:param symbol: The bytearray to store the symbol.\n");

    (extra_decoder_methods<Coder>(decoder_class));
}
}
