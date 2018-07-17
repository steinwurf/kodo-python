// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <string>

#include <boost/python.hpp>

#include "resolve_field_name.hpp"

namespace kodo_python
{
template<class Coder>
struct extra_factory_methods
{
    template<class FactoryClass>
    extra_factory_methods(FactoryClass& factory_class)
    {
        (void) factory_class;
    }
};

template<class Coder>
void factory(const std::string& stack)
{
    using boost::python::arg;
    using boost::python::args;
    using boost::python::class_;
    using boost::python::init;
    using stack_type = Coder;
    using factory_type = typename stack_type::factory;

    //std::string field = resolve_field_name<Field>();
    std::string coder =
        kodo_core::has_is_complete<stack_type>::value ? "Decoder" : "Encoder";
    std::string kind = coder + std::string("Factory");
    std::string name = stack + kind + field;

    auto factory_class =
        class_<factory_type, boost::noncopyable>(
            name.c_str(),
            "Factory for creating encoders/decoders.",
            init<uint32_t, uint32_t>(
                args("symbols", "symbol_size"),
                "Factory constructor.\n\n"
                "\t:param symbols: "
                "The number of symbols in a block.\n"
                "\t:param symbol_size: "
                "The size of a symbol in bytes.\n"))
        .def("build", &factory_type::build,
             "Build the actual coder.\n\n"
             "\t:returns: An instantiation of a coder.\n")
        .def("set_symbols", &factory_type::set_symbols, arg("symbols"),
             "Set the number of symbols.\n\n"
             "\t:param symbols: The number of symbols.\n"
            )
        .def("symbols", &factory_type::symbols,
             "Return the number of symbols in a block.\n\n"
             "\t:returns: The number of symbols in a block.\n"
            )
        .def("set_symbol_size", &factory_type::set_symbol_size,
             arg("symbol_size"),
             "Set the symbol size.\n\n"
             "\t:param symbols_size: The symbol size.\n"
            )
        .def("symbol_size", &factory_type::symbol_size,
             "Return the symbol size in bytes.\n\n"
             "\t:returns: The symbol size in bytes.\n"
            )


    (extra_factory_methods<Coder>(factory_class));

    // Enable boost to map from the c++ pointer type to the python coder
    // type. E.g., from std::shared_ptr<Codec> to python [Codec]Encoder.
    boost::python::register_ptr_to_python<typename factory_type::pointer>();
}
}
