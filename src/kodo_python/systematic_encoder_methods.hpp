// Copyright Steinwurf ApS 2018.
// Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

namespace kodo_python
{
template<class EncoderClass>
static void systematic_encoder_methods(EncoderClass& encoder_class)
{
    encoder_class
    .def("is_systematic_on",
         &EncoderClass::wrapped_type::is_systematic_on,
         "Check if the encoder is in systematic mode.\n\n"
         "\t:returns: True if the encoder is in systematic mode.\n"
        )
    .def("in_systematic_phase",
         &EncoderClass::wrapped_type::in_systematic_phase,
         "Check if the encoder has systematic packets available.\n\n"
         "\t:returns: True if the encoder is in systematic phase.\n"
        )
    .def("set_systematic_on",
         &EncoderClass::wrapped_type::set_systematic_on,
         "Turn on systematic mode.\n"
        )
    .def("set_systematic_off",
         &EncoderClass::wrapped_type::set_systematic_off,
         "Turn off systematic mode.\n");
}
}
