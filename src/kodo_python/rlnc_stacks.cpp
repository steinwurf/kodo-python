// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#if !defined(KODO_PYTHON_DISABLE_RLNC)

#include <pybind11/pybind11.h>

#include <kodo_rlnc/coders.hpp>

#include "create_helpers.hpp"
#include "symbol_decoding_status_updater_methods.hpp"
#include "systematic_encoder_methods.hpp"

namespace kodo_python
{
template<>
struct extra_encoder_methods<kodo_rlnc::encoder>
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
        .def("set_density",
             &EncoderClass::type::set_density, arg("density"),
             "Set the density of the coefficients generated.\n\n"
             "\t:param density: The coefficients density.\n")
        .def("density",
             &EncoderClass::type::density,
             "Get the density of the coefficients generated.\n\n"
             "\t:returns: The density of the generator.\n");

        systematic_encoder_methods(encoder_class);
    }
};

template<>
struct extra_decoder_methods<kodo_rlnc::decoder>
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
             "Generate a recoded symbol.\n\n"
             "\t:returns: The recoded symbol.\n")
        .def("is_partially_complete",
             &DecoderClass::type::is_partially_complete,
             "Check if some symbols in the decoder are fully decoded even\n"
             "though the full data block has not been sent.\n\n"
             "\t:returns: True if the decoding matrix is partially complete.\n");

        symbol_decoding_status_updater_methods(decoder_class);
    }
};

template<>
struct extra_factory_methods<kodo_rlnc::encoder>
{
    template<class FactoryClass>
    extra_factory_methods(FactoryClass& factory_class)
    {
        using namespace pybind11;

        factory_class
        .def("set_coding_vector_format",
             &FactoryClass::type::set_coding_vector_format,
             arg("format"),
             "Set the coding vector format for encoded payloads.\n\n"
             "\t:param format: The selected coding vector format,\n");
    }
};

template<>
struct extra_factory_methods<kodo_rlnc::pure_recoder>
{
    template<class FactoryClass>
    extra_factory_methods(FactoryClass& factory_class)
    {
        using namespace pybind11;

        factory_class
        .def("recoder_symbols",
             &FactoryClass::type::recoder_symbols,
             "Return the the number of internal symbols that should be\n"
             "stored in the recoder.\n\n"
             "\t:returns: The number of recoder symbols.\n")
        .def("set_recoder_symbols",
             &FactoryClass::type::set_recoder_symbols,
             arg("symbols"),
             "Set the number of recoder symbols for coders to be created\n"
             "with this factory.\n\n"
             "\t:param symbols: The number of recoder symbols to use.\n");
    }
};

void create_rlnc_stacks(pybind11::module& m)
{
    using namespace pybind11;

    enum_<kodo_rlnc::coding_vector_format>(m, "coding_vector_format")
    .value("full_vector", kodo_rlnc::coding_vector_format::full_vector)
    .value("seed", kodo_rlnc::coding_vector_format::seed)
    .value("sparse_seed", kodo_rlnc::coding_vector_format::sparse_seed);

    create_factory_and_encoder<kodo_rlnc::encoder>(m, "RLNCEncoder");
    create_factory_and_decoder<kodo_rlnc::decoder>(m, "RLNCDecoder");
    create_factory_and_recoder<kodo_rlnc::pure_recoder>(m, "RLNCPureRecoder");
}
}

#endif
