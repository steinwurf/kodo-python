// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include <boost/python.hpp>

#include <storage/storage.hpp>

#include "coder.hpp"

namespace kodo_python
{
template<class Decoder>
void set_mutable_symbols(Decoder& decoder, PyObject* obj)
{
    assert(PyByteArray_Check(obj) && "The symbol storage should be a "
           "Python bytearray object");

    auto symbol_storage = storage::mutable_storage(
        (uint8_t*)PyByteArray_AsString(obj),
        (uint32_t)PyByteArray_Size(obj));

    decoder.set_mutable_symbols(symbol_storage);
}

template<class Decoder>
void set_mutable_symbol(Decoder& decoder, uint32_t index, PyObject* obj)
{
    assert(PyByteArray_Check(obj) && "The symbol storage should be a "
           "Python bytearray object");

    auto symbol = storage::mutable_storage(
        (uint8_t*)PyByteArray_AsString(obj),
        (uint32_t)PyByteArray_Size(obj));

    decoder.set_mutable_symbol(index, symbol);
}

template<class Decoder>
PyObject* decoder_write_payload(Decoder& decoder)
{
    std::vector<uint8_t> payload(decoder.payload_size());
    auto length = decoder.write_payload(payload.data());

#if PY_MAJOR_VERSION >= 3
    return PyBytes_FromStringAndSize((char*)payload.data(), length);
#else
    return PyString_FromStringAndSize((char*)payload.data(), length);
#endif
}

template<class Decoder>
void read_payload(Decoder& decoder, const std::string& data)
{
    std::vector<uint8_t> payload(data.length());
    std::copy(data.c_str(), data.c_str() + data.length(), payload.data());
    decoder.read_payload(payload.data());
}

template<bool HasPartialDecodingTracker>
struct is_partially_complete_method
{
    template<class DecoderClass>
    is_partially_complete_method(DecoderClass& decoder_class)
    {
        (void) decoder_class;
    }
};

template<>
struct is_partially_complete_method<true>
{
    template<class DecoderClass>
    is_partially_complete_method(DecoderClass& decoder_class)
    {
        decoder_class
        .def("is_partially_complete",
             &DecoderClass::wrapped_type::is_partially_complete,
             "Check whether the decoding matrix is partially decoded.\n\n"
             "\t:returns: True if the decoding matrix is partially decoded.\n");
    }
};

template<bool HasWritePayload>
struct write_payload_method
{
    template<class DecoderClass>
    write_payload_method(DecoderClass& decoder_class)
    {
        (void) decoder_class;
    }
};

template<>
struct write_payload_method<true>
{
    template<class DecoderClass>
    write_payload_method(DecoderClass& decoder_class)
    {
        decoder_class
        .def("write_payload",
             &decoder_write_payload<typename DecoderClass::wrapped_type>,
             "Recode symbol.\n\n"
             "\t:returns: The recoded symbol.\n"
            );
    }
};

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
void decoder(const std::string& name)
{
    using namespace boost::python;
    using decoder_type = Coder;

    auto decoder_class =
        coder<Coder>(name)
        .def("read_payload", &read_payload<decoder_type>, arg("symbol_data"),
             "Decode the provided encoded symbol.\n\n"
             "\t:param symbol_data: The encoded symbol.\n"
            )
        .def("is_complete", &decoder_type::is_complete,
             "Check whether decoding is complete.\n\n"
             "\t:returns: True if the decoding is complete.\n"
            )
        .def("symbols_uncoded", &decoder_type::symbols_uncoded,
             "Returns the number of uncoded symbols currently known.\n\n"
             "Depending on the algorithm used the true number of uncoded\n"
             "symbols may be higher.\n"
             "The reason for this uncertainty is the some algorithms, for\n"
             "performance reasons, choose to not keep track of the exact\n"
             "status of the decoding matrix.\n"
             "It is however guaranteed that at least this amount of uncoded\n"
             "symbols exist.\n\n"
             "\t:returns: The number of symbols which have been uncoded.\n"
            )
        .def("symbols_missing", &decoder_type::symbols_missing,
             "Return the number of missing symbols at the decoder.\n\n"
             "\t:returns: The number of missing symbols.\n"
            )
        .def("symbols_partially_decoded",
             &decoder_type::symbols_partially_decoded,
             "Return the number of partially decoded symbols at the decoder.\n\n"
             "\t:returns: The number of partially decoded symbols.\n"
            )
        .def("is_symbol_uncoded", &decoder_type::is_symbol_uncoded,
             arg("index"),
             "Check if the symbol at given index is uncoded.\n\n"
             "This may return false for symbols that are actually uncoded,\n"
             "but never true for symbols that are not uncoded.\n"
             "As with the symbols_uncoded() function the reason for this is\n"
             "that some algorithms do not, for performance reasons, keep track\n"
             "of the exact status of the decoding matrix.\n\n"
             "\t:param index: Index of the symbol to check.\n"
             "\t:return: True if the symbol is uncoded, and otherwise false.\n"
            )
        .def("is_symbol_missing", &decoder_type::is_symbol_missing,
             arg("index"),
             "Check if the symbol at given index is missing.\n\n"
             "\t:param index: Index of the symbol to check.\n"
             "\t:return: True if the symbol is missing otherwise false.\n"
            )
        .def("is_symbol_partially_decoded",
             &decoder_type::is_symbol_partially_decoded,
             arg("index"),
             "Check if the symbol at given index is partially decoded.\n\n"
             "\t:param index: Index of the symbol to check.\n"
             "\t:return: True if the symbol is partially decoded otherwise\n"
             "\t         false.\n"
            )
        .def("set_mutable_symbols", &set_mutable_symbols<decoder_type>,
             arg("symbols"),
             "Set the buffer where the decoded symbols should be stored.\n\n"
             "\t:param symbols: The bytearray to store the symbols.\n"
            )
        .def("set_mutable_symbol", &set_mutable_symbol<decoder_type>,
             args("index", "symbol"),
             "Set the storage for a single symbol.\n\n"
             "\t:param index: The index of the symbol in the coding block.\n"
             "\t:param symbol: The bytearray to store the symbol.\n");

    //(write_payload_method<
    // kodo_core::has_write_payload<decoder_type>::value>(decoder_class));

    //(is_partially_complete_method<
    // kodo_core::has_partial_decoding_tracker<decoder_type>::value>(
    //     decoder_class));

    (extra_decoder_methods<Coder>(decoder_class));
}
}
