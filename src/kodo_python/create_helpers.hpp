// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <string>

#include "encoder.hpp"
#include "decoder.hpp"
#include "recoder.hpp"
#include "factory.hpp"

namespace kodo_python
{
template<class Coder>
void create_factory_and_encoder(pybind11::module& m, const std::string& stack)
{
    // First create the factory type
    factory<Coder>(m, stack);
    // Then create the corresponding encoder type
    encoder<Coder>(m, stack);
}

template<class Coder>
void create_factory_and_decoder(pybind11::module& m, const std::string& stack)
{
    // First create the factory type
    factory<Coder>(m, stack);
    // Then create the corresponding decoder type
    decoder<Coder>(m, stack);
}

template<class Coder>
void create_factory_and_recoder(pybind11::module& m, const std::string& stack)
{
    // First create the factory type
    factory<Coder>(m, stack);
    // Then create the corresponding recoder type
    recoder<Coder>(m, stack);
}
}
