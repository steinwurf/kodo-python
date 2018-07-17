// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <string>

#include "encoder.hpp"
#include "decoder.hpp"
#include "factory.hpp"
#include "resolve_field_name.hpp"

namespace kodo_python
{
template<class Coder>
void create_factory_and_encoder(const std::string& stack)
{
    // First create the factory type
    factory<Coder>(stack);
    // Then create the corresponding encoder type
    encoder<Coder>(stack);
}

template<class Coder>
void create_factory_and_decoder(const std::string& stack)
{
    // First create the factory type
    factory<Coder>(stack);
    // Then create the corresponding decoder type
    decoder<Coder>(stack);
}

template<class Coder>
void create_encoder(const std::string& stack)
{
    create_factory_and_encoder<Coder>(stack);
}

template<template<class, class> class Coder>
void create_decoder(const std::string& stack)
{
    create_factory_and_decoder<Coder>(stack);
}
}
