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
void encoder_set_symbols_storage(Encoder& encoder, pybind11::handle handle)
{
    PyObject* obj = handle.ptr();
    assert(PyByteArray_Check(obj) && "The symbol storage should be a "
           "Python bytearray object");

    encoder.set_symbols_storage((const uint8_t*)PyByteArray_AsString(obj));
}

template<class Encoder>
void encoder_set_symbol_storage(
    Encoder& encoder, pybind11::handle handle, uint32_t index)
{
    PyObject* obj = handle.ptr();
    assert(PyByteArray_Check(obj) && "The symbol storage should be a "
           "Python bytearray object");

    encoder.set_symbol_storage(
        (const uint8_t*)PyByteArray_AsString(obj), index);
}

template<class Encoder>
pybind11::handle encoder_produce_payload(Encoder& encoder)
{
    std::vector<uint8_t> payload(encoder.max_payload_size());
    uint32_t length = encoder.produce_payload(payload.data());

    return PyByteArray_FromStringAndSize((char*)payload.data(), length);
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
        .def("produce_payload", &encoder_produce_payload<encoder_type>,
             "Generate an encoded payload.\n\n"
             "\t:returns: The bytearray containing the encoded payload.\n")
        .def("set_symbols_storage", &encoder_set_symbols_storage<encoder_type>,
             arg("symbols"),
             "Set the symbols to be encoded.\n\n"
             "\t:param symbols: The symbols to be encoded.\n")
        .def("set_symbol_storage", &encoder_set_symbol_storage<encoder_type>,
             arg("symbol"), arg("index"),
             "Set a symbol to be encoded.\n\n"
             "\t:param symbol: The actual data of that symbol.\n"
             "\t:param index: The index of the symbol in the coding block.\n");

    (extra_encoder_methods<Coder>(encoder_class));
}
}
