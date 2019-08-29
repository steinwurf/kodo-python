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

template<class Decoder>
void consume_symbol(
    Decoder& decoder, pybind11::handle handle1, pybind11::handle handle2)
{
    PyObject* symbol_data = handle1.ptr();
    PyObject* coefficients = handle2.ptr();
    assert(PyByteArray_Check(symbol_data) && "The symbol buffer should be a "
           "Python bytearray object");
    assert(PyByteArray_Check(coefficients) && "The coefficients buffer should "
           "be a Python bytearray object");

    decoder.consume_symbol(
        (uint8_t*)PyByteArray_AsString(symbol_data),
        (uint8_t*)PyByteArray_AsString(coefficients));
}

template<class Decoder>
void consume_systematic_symbol(
    Decoder& decoder, pybind11::handle handle, uint32_t index)
{
    PyObject* obj = handle.ptr();
    assert(PyByteArray_Check(obj) && "The symbol buffer should be a "
           "Python bytearray object");

    decoder.consume_systematic_symbol(
        (uint8_t*)PyByteArray_AsString(obj), index);
}

}
