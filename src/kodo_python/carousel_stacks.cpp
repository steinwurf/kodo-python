// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#if !defined(KODO_PYTHON_DISABLE_NOCODE)

#include <pybind11/pybind11.h>

#include <kodo_core/nocode/carousel_decoder.hpp>
#include <kodo_core/nocode/carousel_encoder.hpp>

#include "create_helpers.hpp"

namespace kodo_python
{
void create_carousel_stacks(pybind11::module& m)
{
    using namespace kodo_core::nocode;

    create_factory_and_encoder<carousel_encoder>(m, "NoCodeEncoder");
    create_factory_and_decoder<carousel_decoder>(m, "NoCodeDecoder");
}
}

#endif
