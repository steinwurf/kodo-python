// Copyright Steinwurf ApS 2018.
// Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <pybind11/pybind11.h>

#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

namespace kodo_python
{

template<class Encoder>
pybind11::handle encoder_produce_symbol(
    Encoder& encoder, pybind11::handle handle)
{
    PyObject* coefficients = handle.ptr();
    assert(PyByteArray_Check(coefficients) && "The coefficients should be a "
           "Python bytearray object");

    std::vector<uint8_t> symbol(encoder.symbol_size());
    uint32_t length = encoder.produce_symbol(
        symbol.data(), (uint8_t*)PyByteArray_AsString(coefficients));

    return PyByteArray_FromStringAndSize((char*)symbol.data(), length);
}


template<class Encoder>
pybind11::handle encoder_produce_systematic_symbol(
    Encoder& encoder, uint32_t index)
{
    std::vector<uint8_t> symbol(encoder.symbol_size());
    uint32_t length = encoder.produce_systematic_symbol(symbol.data(), index);

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

}
