// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <pybind11/pybind11.h>

#include <string>

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
void factory(pybind11::module& m, const std::string& stack)
{
    using namespace pybind11;

    using stack_type = Coder;
    using factory_type = typename stack_type::factory;

    std::string name = stack + std::string("Factory");

    auto factory_class = class_<factory_type>(
        m, name.c_str(), "Factory for creating encoders/decoders.")
        .def(init<fifi::api::field, uint32_t, uint32_t>(),
             arg("field"), arg("symbols"), arg("symbol_size"),
             "Factory constructor.\n\n"
             "\t:param field: The finite field to use.\n"
             "\t:param symbols: The number of symbols in a block.\n"
             "\t:param symbol_size: The size of a symbol in bytes.\n")
        .def("build", &factory_type::build,
             "Build the actual coder.\n\n"
             "\t:returns: A coder instance.\n")
        .def("set_symbols", &factory_type::set_symbols, arg("symbols"),
             "Set the number of symbols.\n\n"
             "\t:param symbols: The number of symbols.\n")
        .def("symbols", &factory_type::symbols,
             "Return the number of symbols in a block.\n\n"
             "\t:returns: The number of symbols in a block.\n")
        .def("set_symbol_size", &factory_type::set_symbol_size,
             arg("symbol_size"),
             "Set the symbol size.\n\n"
             "\t:param symbols_size: The symbol size.\n")
        .def("symbol_size", &factory_type::symbol_size,
             "Return the symbol size in bytes.\n\n"
             "\t:returns: The symbol size in bytes.\n");

    (extra_factory_methods<Coder>(factory_class));
}
}
