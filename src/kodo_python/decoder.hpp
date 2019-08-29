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

#include "coder.hpp"

namespace kodo_python
{
template<class Decoder>
void decoder_set_symbols_storage(Decoder& decoder, pybind11::handle handle)
{
    PyObject* obj = handle.ptr();
    assert(PyByteArray_Check(obj) && "The symbol storage should be a "
           "Python bytearray object");

    decoder.set_symbols_storage((uint8_t*)PyByteArray_AsString(obj));
}

template<class Decoder>
void decoder_set_symbol_storage(
    Decoder& decoder, pybind11::handle handle, uint32_t index)
{
    PyObject* obj = handle.ptr();
    assert(PyByteArray_Check(obj) && "The symbol storage should be a "
           "Python bytearray object");

    decoder.set_symbol_storage((uint8_t*)PyByteArray_AsString(obj), index);
}

template<class Decoder>
pybind11::handle decoder_produce_payload(Decoder& decoder)
{
    std::vector<uint8_t> payload(decoder.max_payload_size());
    auto length = decoder.produce_payload(payload.data());

    return PyByteArray_FromStringAndSize((char*)payload.data(), length);
}

template<class Decoder>
void consume_payload(Decoder& decoder, pybind11::handle handle)

{
    PyObject* obj = handle.ptr();
    assert(PyByteArray_Check(obj) && "The payload buffer should be a "
           "Python bytearray object");

    decoder.consume_payload((uint8_t*)PyByteArray_AsString(obj));
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
        .def("consume_payload", &consume_payload<decoder_type>,
             arg("symbol_data"),
             "Decode the provided encoded payload.\n\n"
             "\t:param symbol_data: The encoded payload.\n")
        .def("is_complete", &decoder_type::is_complete,
             "Check whether decoding is complete.\n\n"
             "\t:returns: True if the decoding is complete.\n")
        .def("symbols_decoded", &decoder_type::symbols_decoded,
             "Returns the number of decoded symbols currently known.\n\n"
             "Depending on the algorithm used the true number of decoded\n"
             "symbols may be higher.\n"
             "The reason for this uncertainty is the some algorithms, for\n"
             "performance reasons, choose to not keep track of the exact\n"
             "status of the decoding matrix.\n"
             "It is however guaranteed that at least this amount of decoded\n"
             "symbols exist.\n\n"
             "\t:returns: The number of symbols which have been decoded.\n")
        .def("symbols_missing", &decoder_type::symbols_missing,
             "Return the number of missing symbols at the decoder.\n\n"
             "\t:returns: The number of missing symbols.\n")
        .def("symbols_partially_decoded",
             &decoder_type::symbols_partially_decoded,
             "Return the number of partially decoded symbols at the decoder.\n\n"
             "\t:returns: The number of partially decoded symbols.\n")
        .def("is_symbol_decoded", &decoder_type::is_symbol_decoded,
             arg("index"),
             "Check if the symbol at given index is decoded.\n\n"
             "This may return false for symbols that are actually decoded,\n"
             "but never true for symbols that are not decoded.\n"
             "As with the symbols_decoded() function the reason for this is\n"
             "that some algorithms do not, for performance reasons, keep track\n"
             "of the exact status of the decoding matrix.\n\n"
             "\t:param index: Index of the symbol to check.\n"
             "\t:return: True if the symbol is decoded, and otherwise false.\n")
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
        .def("set_symbols_storage", &decoder_set_symbols_storage<decoder_type>,
             arg("symbols"),
             "Set the buffer where the decoded symbols should be stored.\n\n"
             "\t:param symbols: The bytearray to store the symbols.\n")
        .def("set_symbol_storage", &decoder_set_symbol_storage<decoder_type>,
             arg("symbol"), arg("index"),
             "Set the storage for a single symbol.\n\n"
             "\t:param symbol: The bytearray to store the symbol.\n"
             "\t:param index: The index of the symbol in the coding block.\n");

    (extra_decoder_methods<Coder>(decoder_class));
}
}
