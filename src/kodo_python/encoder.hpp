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
template<class Encoder>
void set_const_symbols(Encoder& encoder, pybind11::handle handle)
{
    PyObject* obj = handle.ptr();
    assert(PyByteArray_Check(obj) && "The symbol storage should be a "
           "Python bytearray object");

    auto symbol_storage = storage::const_storage(
        (uint8_t*)PyByteArray_AsString(obj),
        (uint32_t)PyByteArray_Size(obj));

    encoder.set_const_symbols(symbol_storage);
}

template<class Encoder>
void set_const_symbol(
    Encoder& encoder, uint32_t index, pybind11::handle handle)
{
    PyObject* obj = handle.ptr();
    assert(PyByteArray_Check(obj) && "The symbol storage should be a "
           "Python bytearray object");

    auto symbol = storage::const_storage(
        (uint8_t*)PyByteArray_AsString(obj),
        (uint32_t)PyByteArray_Size(obj));

    encoder.set_const_symbol(index, symbol);
}

template<class Encoder>
pybind11::handle encoder_write_payload(Encoder& encoder)
{
    std::vector<uint8_t> payload(encoder.payload_size());
    uint32_t length = encoder.write_payload(payload.data());

    return PyByteArray_FromStringAndSize((char*)payload.data(), length);
}

template<class Encoder>
pybind11::handle encoder_write_symbol(Encoder& encoder, pybind11::handle handle)
{
    PyObject* coefficients = handle.ptr();
    assert(PyByteArray_Check(coefficients) && "The coefficients should be a "
           "Python bytearray object");

    std::vector<uint8_t> symbol(encoder.symbol_size());
    uint32_t length = encoder.write_symbol(
        symbol.data(), (uint8_t*)PyByteArray_AsString(coefficients));

    return PyByteArray_FromStringAndSize((char*)symbol.data(), length);
}


template<class Encoder>
pybind11::handle encoder_write_uncoded_symbol(Encoder& encoder, uint32_t index)
{
    std::vector<uint8_t> symbol(encoder.symbol_size());
    uint32_t length = encoder.write_uncoded_symbol(symbol.data(), index);

    return PyByteArray_FromStringAndSize((char*)symbol.data(), length);
}

template<class Encoder>
pybind11::handle encoder_generate(Encoder& encoder)
{
    std::vector<uint8_t> coefficients(encoder.coefficient_vector_size());
    encoder.generate(coefficients.data());

    return PyByteArray_FromStringAndSize(
        (char*)coefficients.data(), coefficients.size());
}

template<class Coder>
struct extra_encoder_methods
{
    template<class EncoderClass>
    extra_encoder_methods(EncoderClass& encoder_class)
    {
        (void) encoder_class;
    }
};

template<class Coder>
void encoder(pybind11::module& m, const std::string& name)
{
    using namespace pybind11;

    using encoder_type = Coder;

    auto encoder_class =
        coder<Coder>(m, name)
        .def("rank", &encoder_type::rank,
             "Return the current rank of the encoder.\n\n"
             "The rank of an encoder indicates how many symbols are\n"
             "available for encoding.\n\n"
             "\t:returns: The rank.\n")
        .def("write_payload", &encoder_write_payload<encoder_type>,
             "Generate an encoded payload.\n\n"
             "\t:returns: The bytearray containing the encoded payload.\n")
        .def("write_symbol", &encoder_write_symbol<encoder_type>,
             arg("coefficients"),
             "Generate an encoded symbol using the given coefficients.\n\n"
             "\t:param coefficients: The coding coefficients.\n"
             "\t:returns: The bytearray containing the encoded symbol.\n")
        .def("write_uncoded_symbol",
             &encoder_write_uncoded_symbol<encoder_type>,
             arg("index"),
             "Return a systematic symbol for the given symbol index.\n\n"
             "\t:param index: The symbol index with the coding block.\n"
             "\t:returns: The bytearray containing the systematic symbol.\n")
        .def("generate", &encoder_generate<encoder_type>,
             "Generate some coding coefficients.\n\n"
             "\t:returns: The bytearray containing the coding coefficients.\n")
        .def("set_const_symbols", &set_const_symbols<encoder_type>,
             arg("symbols"),
             "Set the symbols to be encoded.\n\n"
             "\t:param symbols: The symbols to be encoded.\n")
        .def("set_const_symbol", &set_const_symbol<encoder_type>,
             arg("index"), arg("symbol"),
             "Set a symbol to be encoded.\n\n"
             "\t:param index: The index of the symbol in the coding block.\n"
             "\t:param symbol: The actual data of that symbol.\n");

    (extra_encoder_methods<Coder>(encoder_class));
}
}
