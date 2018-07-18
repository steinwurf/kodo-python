// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#if !defined(KODO_PYTHON_DISABLE_RLNC)

#include <kodo_rlnc/coders.hpp>

#include "create_helpers.hpp"
#include "symbol_decoding_status_updater_methods.hpp"

namespace kodo_python
{
template<>
struct extra_encoder_methods<kodo_rlnc::encoder>
{
    template<class EncoderClass>
    extra_encoder_methods(EncoderClass& encoder_class)
    {
        using boost::python::arg;
        encoder_class
        .def("set_density",
             &EncoderClass::wrapped_type::set_density, arg("density"),
             "Set the density of the coefficients generated.\n\n"
             "\t:param density: The coefficients density.\n"
            )
        .def("density",
             &EncoderClass::wrapped_type::density,
             "Get the density of the coefficients generated.\n\n"
             "\t:returns: The density of the generator.\n"
            );
    }
};

template<>
struct extra_decoder_methods<kodo_rlnc::decoder>
{
    template<class DecoderClass>
    extra_decoder_methods(DecoderClass& decoder_class)
    {
        symbol_decoding_status_updater_methods<DecoderClass>(decoder_class);
    }
};

void create_rlnc_stacks()
{
    create_factory_and_encoder<kodo_rlnc::encoder>("RLNCEncoder");
    create_factory_and_decoder<kodo_rlnc::decoder>("RLNCDecoder");
}
}

#endif
