// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#if !defined(KODO_PYTHON_DISABLE_NOCODE)

#include <pybind11/pybind11.h>

#include <kodo_core/nocode/carousel_decoder.hpp>
#include <kodo_core/nocode/carousel_encoder.hpp>

#include "encoder.hpp"
#include "decoder.hpp"


namespace kodo_python
{
void create_carousel_stacks(pybind11::module& m)
{
    using namespace kodo_core::nocode;

    encoder<carousel_encoder>(m, "NoCodeEncoder");
    decoder<carousel_decoder>(m, "NoCodeDecoder");
}
}

#endif
