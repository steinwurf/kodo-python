// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#if !defined(KODO_PYTHON_DISABLE_RLNC) && \
    !defined(KODO_PYTHON_DISABLE_ON_THE_FLY)

#include <kodo_rlnc/on_the_fly_codes.hpp>

#include "create_helpers.hpp"
#include "symbol_decoding_status_updater_methods.hpp"

namespace kodo_python
{

template<>
struct extra_decoder_methods<kodo_rlnc::on_the_fly_decoder>
{
    template<class DecoderClass>
    extra_decoder_methods(DecoderClass& decoder_class)
    {
        symbol_decoding_status_updater_methods<DecoderClass>(decoder_class);
    }
};

void create_on_the_fly_stacks()
{
    create_encoder<kodo_rlnc::on_the_fly_encoder>("OnTheFly");
    create_decoder<kodo_rlnc::on_the_fly_decoder>("OnTheFly");
}
}

#endif
