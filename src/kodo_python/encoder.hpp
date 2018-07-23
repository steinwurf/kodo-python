// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

#include <boost/python.hpp>

#include "coder.hpp"

namespace kodo_python
{
template<class Encoder>
void set_const_symbols(Encoder& encoder, PyObject* obj)
{
    assert(PyByteArray_Check(obj) && "The symbol storage should be a "
           "Python bytearray object");

    auto symbol_storage = storage::const_storage(
        (uint8_t*)PyByteArray_AsString(obj),
        (uint32_t)PyByteArray_Size(obj));

    encoder.set_const_symbols(symbol_storage);
}

template<class Encoder>
void set_const_symbol(Encoder& encoder, uint32_t index, PyObject* obj)
{
    assert(PyByteArray_Check(obj) && "The symbol storage should be a "
           "Python bytearray object");

    auto symbol = storage::const_storage(
        (uint8_t*)PyByteArray_AsString(obj),
        (uint32_t)PyByteArray_Size(obj));

    encoder.set_const_symbol(index, symbol);
}

template<class Encoder>
PyObject* encoder_write_payload(Encoder& encoder)
{
    std::vector<uint8_t> payload(encoder.payload_size());
    uint32_t length = encoder.write_payload(payload.data());

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
void encoder(const std::string& name)
{
    using namespace boost::python;

    using encoder_type = Coder;

    auto encoder_class =
        coder<Coder>(name)
        .def("rank", &encoder_type::rank,
             "Return the current rank of the encoder.\n\n"
             "The rank of an encoder indicates how many symbols are\n"
             "available for encoding.\n\n"
             "\t:returns: The rank.\n")
        .def("write_payload", &encoder_write_payload<encoder_type>,
             "Generate an encoded payload.\n\n"
             "\t:returns: The bytearray containing the encoded payload.\n")
        .def("set_const_symbols", &set_const_symbols<encoder_type>,
             arg("symbols"),
             "Set the symbols to be encoded.\n\n"
             "\t:param symbols: The symbols to be encoded.\n")
        .def("set_const_symbol", &set_const_symbol<encoder_type>,
             args("index", "symbol"),
             "Set a symbol to be encoded.\n\n"
             "\t:param index: The index of the symbol in the coding block.\n"
             "\t:param symbol: The actual data of that symbol.\n");

    (extra_encoder_methods<Coder>(encoder_class));
}
}
