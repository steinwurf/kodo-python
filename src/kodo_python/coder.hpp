// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>

#include <string>
#include <functional>

namespace kodo_python
{

/// The callback used to output the logged information
using log_callback =
    std::function<void(const std::string& zone, const std::string& data)>;

template<class Coder>
void set_trace_callback(Coder& coder, const log_callback& callback)
{
    coder.set_trace_callback(callback);
}

template<class Coder>
pybind11::class_<Coder> coder(pybind11::module& m, const std::string& name)
{
    using namespace pybind11;

    using coder_type = Coder;

    // Declare std::shared_ptr as a holder type, because the factories
    // return a shared_ptr when building a coder instance
    auto coder_class = class_<coder_type, std::shared_ptr<coder_type>>(
        m, name.c_str(), "A coder object")
        .def("payload_size", &coder_type::payload_size,
             "Return the required payload buffer size in bytes.\n\n"
             "\t:returns: The required payload buffer size in bytes.\n")
        .def("block_size", &coder_type::block_size,
             "Return the block size.\n\n"
             "\t:returns: The block size i.e. the total size in bytes that "
             "this coder operates on.\n")
        .def("symbol_size", &coder_type::symbol_size,
             "Return the symbol size of a symbol in bytes.\n\n"
             "\t:returns: The symbol size of a symbol in bytes.\n")
        .def("symbols", &coder_type::symbols,
             "Return the number of symbols in this block coder.\n\n"
             "\t:returns: The number of symbols in this block coder.\n")
        .def("set_trace_callback", &set_trace_callback<coder_type>,
             arg("callback"),
             "Write the trace information to a callback.\n\n"
             "\t:param callback: The callback which is called with the zone "
             "and message.")
        .def("set_trace_stdout", &coder_type::set_trace_stdout,
             "Trace debug info to stdout.\n")
        .def("set_trace_off", &coder_type::set_trace_off,
             "Disable tracing.\n")
        .def("set_zone_prefix", &coder_type::set_zone_prefix,
             arg("zone_prefix"),
             "Set a zone prefix for the tracing output.\n\n"
             "\t:param zone_prefix: The zone prefix to append to all "
             "tracing zones.");

    return std::move(coder_class);
}
}
