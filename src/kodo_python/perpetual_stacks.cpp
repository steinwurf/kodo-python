// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#if !defined(KODO_PYTHON_DISABLE_PERPETUAL)

#include <pybind11/pybind11.h>

#include <kodo_perpetual/coders.hpp>

#include "encoder.hpp"
#include "decoder.hpp"
#include "symbol_decoding_status_updater_methods.hpp"
#include "systematic_encoder_methods.hpp"

namespace kodo_python
{
template<>
struct extra_encoder_methods<kodo_perpetual::encoder>
{
    template<class EncoderClass>
    extra_encoder_methods(EncoderClass& encoder_class)
    {
        using pybind11::arg;

        encoder_class
        .def("set_seed",
             &EncoderClass::type::set_seed, arg("seed"),
             "Set the seed of the coefficient generator.\n\n"
             "\t:param seed: The seed value.\n")
        .def("pseudo_systematic",
             &EncoderClass::type::pseudo_systematic,
             "Get the pseudo-systematic property of the generator.\n\n"
             "\t:returns: The current setting for pseudo-systematic.\n")
        .def("set_pseudo_systematic",
             &EncoderClass::type::set_pseudo_systematic,
             arg("pseudo_systematic"),
             "Set the pseudo-systematic property of the generator.\n\n"
             "\t:param pseudo_systematic: The new setting for "
             "pseudo-systematic\n")
        .def("pre_charging",
             &EncoderClass::type::pre_charging,
             "Get the pre-charging property of the generator.\n\n"
             "\t:returns: The current setting for pseudo-systematic.\n")
        .def("set_pre_charging",
             &EncoderClass::type::set_pre_charging,
             arg("pre_charging"),
             "Set the pre-charging property of the generator.\n\n"
             "\t:param pre_charging: The current setting for pre-charging.\n")
        .def("width", &EncoderClass::type::width,
             "Get the width.\n\n"
             "\t:returns: The width used by the generator.\n")
        .def("set_width", &EncoderClass::type::set_width,
             arg("width"),
             "Set the number of non-zero coefficients after the pivot. "
             "Width ratio is recalculated from this value.\n\n"
             "\t:param width: The width.\n")
        .def("width_ratio", &EncoderClass::type::width_ratio,
             "Get the ratio that is used to calculate the width.\n\n"
             "\t:returns: The width ratio of the generator.\n")
        .def("set_width_ratio",
             &EncoderClass::type::set_width_ratio,
             arg("width_ratio"),
             "Set the ratio that is used to calculate the number of "
             "non-zero coefficients after the pivot (i.e. the width).\n\n"
             "\t:param width_ratio: The width ratio.\n");
    }
};

template<>
struct extra_decoder_methods<kodo_perpetual::decoder>
{
    template<class DecoderClass>
    extra_decoder_methods(DecoderClass& decoder_class)
    {
        using pybind11::arg;

        decoder_class
        .def("set_seed",
             &DecoderClass::type::set_seed, arg("seed"),
             "Set the seed of the coefficient generator.\n\n"
             "\t:param seed: The seed value.\n")
        .def("write_payload",
             &decoder_write_payload<typename DecoderClass::type>,
             "Recode symbol.\n\n"
             "\t:returns: The recoded symbol.\n");

        symbol_decoding_status_updater_methods<DecoderClass>(decoder_class);
    }
};

void create_perpetual_stacks(pybind11::module& m)
{
    encoder<kodo_perpetual::encoder>(m, "PerpetualEncoder");
    decoder<kodo_perpetual::decoder>(m, "PerpetualDecoder");
}
}

#endif
