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
pybind11::handle recoder_write_payload(Recoder& recoder)
{
    std::vector<uint8_t> payload(recoder.payload_size());
    uint32_t length = recoder.write_payload(payload.data());

    return PyByteArray_FromStringAndSize((char*)payload.data(), length);
}

template<class Recoder>
void recoder_read_payload(Recoder& recoder, pybind11::handle handle)
{
    PyObject* obj = handle.ptr();
    assert(PyByteArray_Check(obj) && "The payload buffer should be a "
           "Python bytearray object");

    recoder.read_payload((uint8_t*)PyByteArray_AsString(obj));
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
        .def("read_payload", &recoder_read_payload<recoder_type>,
             arg("symbol_data"),
             "Decode the provided encoded payload.\n\n"
             "\t:param symbol_data: The encoded payload.\n")
        .def("write_payload", &recoder_write_payload<recoder_type>,
             "Generate an recoded payload.\n\n"
             "\t:returns: The bytearray containing the recoded payload.\n");

    (extra_recoder_methods<Coder>(recoder_class));
}
}
