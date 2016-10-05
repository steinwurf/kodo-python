// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <string>
#include <vector>

#include <Python.h>
#include <bytesobject.h>
#include <boost/python.hpp>
#include <boost/python/args.hpp>

#include <kodo_core/has_set_systematic_off.hpp>

#include "coder.hpp"
#include "resolve_field_name.hpp"

namespace kodo_python
{
template<class Encoder>
void set_const_symbols(Encoder& encoder, const std::string& data)
{
    auto storage = storage::const_storage(
        (uint8_t*)data.c_str(), (uint32_t)data.length());
    encoder.set_const_symbols(storage);
}

template<class Encoder>
void set_const_symbol(Encoder& encoder, uint32_t index, const std::string& data)
{
    auto storage = storage::const_storage(
        (uint8_t*)data.c_str(), (uint32_t)data.length());
    encoder.set_const_symbol(index, storage);
}

template<class Encoder>
PyObject* encoder_write_payload(Encoder& encoder)
{
    std::vector<uint8_t> payload(encoder.payload_size());
    uint32_t length = encoder.write_payload(payload.data());
#if PY_MAJOR_VERSION >= 3
    return PyBytes_FromStringAndSize((char*)payload.data(), length);
#else
    return PyString_FromStringAndSize((char*)payload.data(), length);
#endif
}

template<bool IsSystematicEncoder>
struct systematic_encoder_methods
{
    template<class EncoderClass>
    systematic_encoder_methods(EncoderClass& encoder_class)
    {
        (void) encoder_class;
    }
};

template<>
struct systematic_encoder_methods<true>
{
    template<class EncoderClass>
    systematic_encoder_methods(EncoderClass& encoder_class)
    {
        encoder_class
        .def("is_systematic_on",
             &EncoderClass::wrapped_type::is_systematic_on,
             "Check if the encoder systematic mode.\n\n"
             "\t:returns: True if the encoder is in systematic mode.\n"
            )
        .def("in_systematic_phase",
             &EncoderClass::wrapped_type::in_systematic_phase,
             "Check if the encoder has systematic packets available.\n\n"
             "\t:returns: True if the encoder is in systematic phase.\n"
            )
        .def("set_systematic_on",
             &EncoderClass::wrapped_type::set_systematic_on,
             "Set the encoder in systematic mode.\n"
            )
        .def("set_systematic_off",
             &EncoderClass::wrapped_type::set_systematic_off,
             "Turn off systematic mode.\n");
    }
};

template<template<class, class> class Coder>
struct extra_encoder_methods
{
    template<class EncoderClass>
    extra_encoder_methods(EncoderClass& encoder_class)
    {
        (void) encoder_class;
    }
};

template<
    template<class, class> class Coder,
    class Field, class TraceTag
>
void encoder(const std::string& stack)
{
    using boost::python::arg;
    using boost::python::args;
    using encoder_type = Coder<Field, TraceTag>;

    std::string field = resolve_field_name<Field>();
    std::string kind = "Encoder";
    std::string name = stack + kind + field;

    auto encoder_class =
        coder<Coder, Field, TraceTag>(name)
        .def("write_payload", &encoder_write_payload<encoder_type>,
             "Encode a symbol.\n\n"
             "\t:returns: The encoded symbol.\n"
            )
        .def("set_const_symbols", &set_const_symbols<encoder_type>,
             arg("symbols"),
             "Set the symbols to be encoded.\n\n"
             "\t:param symbols: The symbols to be encoded.\n"
            )
        .def("set_const_symbol", &set_const_symbol<encoder_type>,
             args("index", "symbol"),
             "Set a symbol to be encoded.\n\n"
             "\t:param index: The index of the symbol in the coding block.\n"
             "\t:param symbol: The actual data of that symbol.\n");

    (systematic_encoder_methods<
     kodo_core::has_set_systematic_off<encoder_type>::value>(encoder_class));

    (extra_encoder_methods<Coder>(encoder_class));
}
}
