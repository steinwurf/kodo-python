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
void set_log_callback(Coder& coder, const log_callback& callback)
{
    coder.set_log_callback(callback);
}

template<class Coder>
pybind11::class_<Coder> coder(pybind11::module& m, const std::string& name)
{
    using namespace pybind11;

    using coder_type = Coder;

    // Declare std::shared_ptr as a holder type, because the factories
    // return a shared_ptr when building a coder instance
    return class_<coder_type, std::shared_ptr<coder_type>>(
        m, name.c_str(), "A coder object")
        .def(init<fifi::finite_field, uint32_t, uint32_t>(),
             arg("field"), arg("symbols"), arg("symbol_size"),
             "Coder constructor.\n\n"
             "\t:param field: The finite field to use.\n"
             "\t:param symbols: The number of symbols in a block.\n"
             "\t:param symbol_size: The size of a symbol in bytes.\n")
        .def("reset", &coder_type::reset,
             "Reset the coder to a clean state.\n")
        .def("max_payload_size", &coder_type::max_payload_size,
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
        .def("set_log_callback", &set_log_callback<coder_type>,
             arg("callback"),
             "Write the log information to a callback.\n\n"
             "\t:param callback: The callback which is called with the zone "
             "and message.")
        .def("set_log_stdout", &coder_type::set_log_stdout,
             "Log debug info to stdout.\n")
        .def("set_log_off", &coder_type::set_log_off,
             "Disable tracing.\n")
        .def("set_zone_prefix", &coder_type::set_zone_prefix,
             arg("zone_prefix"),
             "Set a zone prefix for the tracing output.\n\n"
             "\t:param zone_prefix: The zone prefix to append to all "
             "tracing zones.");
}
}
