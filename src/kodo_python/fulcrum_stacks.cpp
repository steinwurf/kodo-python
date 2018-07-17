// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#if !defined(KODO_PYTHON_DISABLE_FULCRUM)

#include <kodo_fulcrum/coders.hpp>

#include "create_helpers.hpp"

namespace kodo_python
{
struct fulcrum_coder_methods
{
    template<class CoderClass>
    fulcrum_coder_methods(CoderClass& coder_class)
    {
        coder_class
        .def("max_expansion",
             &CoderClass::wrapped_type::max_expansion,
             "Get the maximum expansion supported.\n\n"
             "\t:returns: The maximum expansion supported.\n"
            )
        .def("expansion",
             &CoderClass::wrapped_type::expansion,
             "Get the expansion factor used. The expansion factor denotes "
             "the number of additional symbols created by the outer "
             "code.\n\n"
             "\t:returns: The expansion factor used.\n"
            )
        .def("inner_symbols",
             &CoderClass::wrapped_type::inner_symbols,
             "Get the number of symbols in the inner code.\n\n"
             "\t:returns: The number of symbols in the inner code.\n"
            );
    }
};

struct fulcrum_factory_methods
{
    template<class FactoryClass>
    fulcrum_factory_methods(FactoryClass& factory_class)
    {
        using boost::python::arg;
        factory_class
        .def("max_expansion",
             &FactoryClass::wrapped_type::max_expansion,
             "Get the maximum expansion supported.\n\n"
             "\t:returns: The maximum expansion supported.\n"
            )
        .def("expansion",
             &FactoryClass::wrapped_type::expansion,
             "Get the expansion factor used. The expansion factor denotes "
             "the number of additional symbols created by the outer "
             "code.\n\n"
             "\t:returns: The expansion factor used.\n"
            )
        .def("set_expansion",
             &FactoryClass::wrapped_type::set_expansion,
             arg("expansion"),
             "Sets the number of expansion symbols.\n\n"
             "\t:param expansion: The number of expansion symbols to use.\n"
            )
        .def("max_inner_symbols",
             &FactoryClass::wrapped_type::max_inner_symbols,
             "Get the maximum number of symbols in the inner code.\n\n"
             "\t:returns: The maximum number of symbols in the inner code.\n"
            );
    }
};

template<>
struct extra_encoder_methods<kodo_fulcrum::encoder>
{
    template<class EncoderClass>
    extra_encoder_methods(EncoderClass& encoder_class)
    {
        (fulcrum_coder_methods(encoder_class));
    }
};

template<>
struct extra_decoder_methods<kodo_fulcrum::decoder>
{
    template<class DecoderClass>
    extra_decoder_methods(DecoderClass& decoder_class)
    {
        (fulcrum_coder_methods(decoder_class));
    }
};

template<>
struct extra_factory_methods<kodo_fulcrum::encoder>
{
    template<class FactoryClass>
    extra_factory_methods(FactoryClass& factory_class)
    {
        (fulcrum_factory_methods(factory_class));
    }
};

template<>
struct extra_factory_methods<kodo_fulcrum::decoder>
{
    template<class FactoryClass>
    extra_factory_methods(FactoryClass& factory_class)
    {
        (fulcrum_factory_methods(factory_class));
    }
};

void create_fulcrum_stacks()
{
    using namespace kodo_fulcrum;

    create_encoder<encoder>("Fulcrum");
    create_decoder<decoder>("Fulcrum");
}
}

#endif
