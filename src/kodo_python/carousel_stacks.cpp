// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#if !defined(KODO_PYTHON_DISABLE_NOCODE)

#include <kodo_core/nocode/carousel_decoder.hpp>
#include <kodo_core/nocode/carousel_encoder.hpp>

#include <kodo_core/pool_factory.hpp>
#include <kodo_core/rebind_factory.hpp>

#include "create_helpers.hpp"

namespace kodo_python
{
template<class Field, class TraceTag>
class carousel_encoder_wrapper : public
    kodo_core::nocode::carousel_encoder<TraceTag>
{
public:
    using factory = kodo_core::rebind_factory<
                    kodo_core::nocode::carousel_encoder<TraceTag>,
                    carousel_encoder_wrapper<Field, TraceTag>>;
};

template<class Field, class TraceTag>
class carousel_decoder_wrapper : public
    kodo_core::nocode::carousel_decoder<TraceTag>
{
public:
    using factory = kodo_core::rebind_factory<
                    kodo_core::nocode::carousel_encoder<TraceTag>,
                    carousel_decoder_wrapper<Field, TraceTag>>;

public:
    uint32_t symbols_uncoded()
    {
        return kodo_core::nocode::carousel_decoder<TraceTag>::rank();
    }
};

void create_carousel_stacks()
{
    create_factory_and_encoder<carousel_encoder_wrapper, no_field>("NoCode");
    create_factory_and_decoder<carousel_decoder_wrapper, no_field>("NoCode");
}
}

#endif
