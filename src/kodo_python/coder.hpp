// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <string>

#include <boost/python.hpp>

namespace kodo_python
{
template<bool HasIsSymbolPivot>
struct is_symbol_pivot_method
{
    template<class CoderClass>
    is_symbol_pivot_method(CoderClass& coder_class)
    {
        (void) coder_class;
    }
};

template<>
struct is_symbol_pivot_method<true>
{
    template<class CoderClass>
    is_symbol_pivot_method(CoderClass& coder_class)
    {
        coder_class
        .def("is_symbol_pivot", &CoderClass::wrapped_type::is_symbol_pivot,
             boost::python::arg("symbol_index"),
             "Check if a certain symbol is a pivot.\n\n"
             "A symbol is pivot if it is available to either the encoder or "
             "decoder. A coefficient generator may use this information "
             "when generating coding coefficients.\n\n"
             "\t:param symbol_index: The index of the symbol.\n"
             "\t:returns: True if the symbol is available.\n"
            );
    }
};

template<class Coder>
void set_trace_callback(Coder& coder, PyObject* function)
{
    auto callback = [function](
        const std::string& zone, const std::string& message)
    {
        boost::python::call<void>(function, zone, message);
    };

    coder.set_trace_callback(callback);
}

template<class Coder>
auto coder(const std::string& name) ->
    boost::python::class_<Coder, boost::noncopyable>
{
    using namespace boost::python;

    using coder_type = Coder;
    auto coder_class = class_<coder_type, boost::noncopyable>(
        name.c_str(), "An (en/de)coder", no_init)
        .def("payload_size", &coder_type::payload_size,
             "Return the required payload buffer size in bytes.\n\n"
             "\t:returns: The required payload buffer size in bytes.\n"
            )
        .def("block_size", &coder_type::block_size,
             "Return the block size.\n\n"
             "\t:returns: The block size i.e. the total size in bytes that "
             "this coder operates on.\n"
            )
        .def("symbol_size", &coder_type::symbol_size,
             "Return the symbol size of a symbol in bytes.\n\n"
             "\t:returns: The symbol size of a symbol in bytes.\n"
            )
        .def("symbols", &coder_type::symbols,
             "Return the number of symbols in this block coder.\n\n"
             "\t:returns: The number of symbols in this block coder.\n"
            )
        .def("rank", &coder_type::rank,
             "Return the rank.\n\n"
             "The rank of a decoder states how many symbols have been "
             "decoded or partially decoded. The rank of an encoder states "
             "how many symbols are available for encoding.\n\n"
             "\t:returns: The rank.\n"
            )
        .def("set_trace_callback",
             &set_trace_callback<coder_type::wrapped_type>,
             boost::python::arg("callback"),
             "Write the trace information to a callback.\n\n"
             "\t:param callback: The callback which is called with the zone "
             "and message.")
        .def("set_trace_stdout", &coder_type::set_trace_stdout,
             "Trace debug info to stdout.\n")
        .def("set_zone_prefix", &coder_type::set_zone_prefix,
             boost::python::arg("zone_prefix"),
             "Sets a zone prefix for the tracing output.\n\n"
             "\t:param zone_prefix: The zone prefix to append to all "
             "tracing zones.");

    //(is_symbol_pivot_method<
    // kodo_core::has_is_symbol_pivot<coder_type>::value>(coder_class));

    return coder_class;
}
}
