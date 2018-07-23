// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

namespace kodo_python
{
template<class DecoderClass>
static void symbol_decoding_status_updater_methods(DecoderClass& decoder_class)
{
    decoder_class
    .def("set_status_updater_on",
         &DecoderClass::wrapped_type::set_status_updater_on,
         "Sets the status updater on.\n\n")
    .def("set_status_updater_off",
         &DecoderClass::wrapped_type::set_status_updater_off,
         "Sets the status updater off.\n\n")
    .def("update_symbol_status",
         &DecoderClass::wrapped_type::update_symbol_status,
         "Updates the symbol status so that all uncoded symbols, "
         "label partially complete.\n\n")
    .def("is_status_updater_enabled",
         &DecoderClass::wrapped_type::is_status_updater_enabled,
         "Check if the symbol status updater is enabled or not.\n\n"
         "\t:returns: True if enabled, otherwise false.\n");
}
}
