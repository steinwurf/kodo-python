// Copyright Steinwurf ApS 2018.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
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
template<class Recoder>
pybind11::handle recoder_produce_payload(Recoder& recoder)
{
    std::vector<uint8_t> payload(recoder.max_payload_size());
    uint32_t length = recoder.produce_payload(payload.data());

    return PyByteArray_FromStringAndSize((char*)payload.data(), length);
}

template<class Recoder>
void recoder_consume_payload(Recoder& recoder, pybind11::handle handle)
{
    PyObject* obj = handle.ptr();
    assert(PyByteArray_Check(obj) && "The payload buffer should be a "
           "Python bytearray object");

    recoder.consume_payload((uint8_t*)PyByteArray_AsString(obj));
}

template<class Recoder>
void recoder_consume_symbol(
    Recoder& recoder, pybind11::handle handle1, pybind11::handle handle2)
{
    PyObject* payload = handle1.ptr();
    PyObject* coefficients = handle2.ptr();
    assert(PyByteArray_Check(payload) && "The payload buffer should be a "
           "Python bytearray object");
    assert(PyByteArray_Check(coefficients) && "The coefficients buffer should "
           "be a Python bytearray object");

    recoder.consume_symbol(
        (uint8_t*)PyByteArray_AsString(payload),
        (uint8_t*)PyByteArray_AsString(coefficients));
}

template<class Recoder>
pybind11::tuple recoder_produce_symbol(
    Recoder& recoder, pybind11::handle handle)
{
    PyObject* recoding_coefficients = handle.ptr();

    assert(PyByteArray_Check(recoding_coefficients) && "The recoding "
           "coefficients buffer should be a Python bytearray object");

    std::vector<uint8_t> symbol(recoder.symbol_size());
    std::vector<uint8_t> coefficients(recoder.coefficient_vector_size());

    recoder.recoder_produce_symbol(
        symbol.data(), coefficients.data(),
        (uint8_t*)PyByteArray_AsString(recoding_coefficients));

    pybind11::handle symbol_bytearray = PyByteArray_FromStringAndSize(
        (char*)symbol.data(), symbol.size());

    pybind11::handle coefficients_bytearray = PyByteArray_FromStringAndSize(
        (char*)coefficients.data(), coefficients.size());

    return pybind11::make_tuple(symbol_bytearray, coefficients_bytearray);
}

template<class Recoder>
pybind11::handle recoder_generate(Recoder& recoder)
{
    std::vector<uint8_t> coefficients(recoder.coefficient_vector_size());
    recoder.recoder_generate(coefficients.data());

    return PyByteArray_FromStringAndSize(
        (char*)coefficients.data(), coefficients.size());
}

template<class Coder>
struct extra_recoder_methods
{
    template<class RecoderClass>
    extra_recoder_methods(RecoderClass& recoder_class)
    {
        (void) recoder_class;
    }
};

template<class Coder>
void recoder(pybind11::module& m, const std::string& name)
{
    using namespace pybind11;

    using recoder_type = Coder;

    auto recoder_class =
        coder<Coder>(m, name)
        .def("recoder_symbols", &recoder_type::recoder_symbols,
             "Return the number of internal symbols that can be stored in "
             "the pure recoder.\n\n"
             "\t:returns: The number of internal symbols.\n")
        .def("consume_payload", &recoder_consume_payload<recoder_type>,
             arg("symbol_data"),
             "Decode the provided encoded payload.\n\n"
             "\t:param symbol_data: The encoded payload.\n")
        .def("consume_symbol", &recoder_consume_symbol<recoder_type>,
             arg("symbol_data"), arg("coefficients"),
             "Decode the provided encoded symbol with the provided coding "
             "coefficients.\n\n"
             "\t:param symbol_data: The encoded payload.\n"
             "\t:param coefficients: The coding coefficients.\n")
        .def("recoder_generate", &recoder_generate<recoder_type>,
             "Generate some coding coefficients suitable for recoding.\n\n"
             "\t:returns: The bytearray containing the coding coefficients.\n")
        .def("recoder_produce_symbol", &recoder_produce_symbol<recoder_type>,
             arg("coefficients"),
             "Generate a recoded symbol using the given coefficients.\n\n"
             "\t:param coefficients: The coding coefficients.\n"
             "\t:returns: A tuple of two bytearrays containing the recoded "
             "symbol and the resulting symbol coefficients.\n")
        .def("produce_payload", &recoder_produce_payload<recoder_type>,
             "Generate an recoded payload.\n\n"
             "\t:returns: The bytearray containing the recoded payload.\n");

    (extra_recoder_methods<Coder>(recoder_class));
}
}
