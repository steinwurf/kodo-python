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
        .def("expansion",
             &CoderClass::wrapped_type::expansion,
             "Get the expansion which denotes the number of additional "
             "symbols created by the outer code.\n\n"
             "\t:returns: The expansion used.\n"
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
        .def("expansion",
             &FactoryClass::wrapped_type::expansion,
             "Get the expansion which denotes the number of additional "
             "symbols created by the outer code.\n\n"
             "\t:returns: The expansion used.\n"
            )
        .def("set_expansion",
             &FactoryClass::wrapped_type::set_expansion,
             arg("expansion"),
             "Set the number of expansion symbols.\n\n"
             "\t:param expansion: The number of expansion symbols to use.\n"
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
    create_factory_and_encoder<kodo_fulcrum::encoder>("FulcrumEncoder");
    create_factory_and_decoder<kodo_fulcrum::decoder>("FulcrumDecoder");
}
}

#endif
