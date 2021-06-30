// License for Commercial Usage
// Distributed under the "KODO EVALUATION LICENSE 1.3"
//
// Licensees holding a valid commercial license may use this project
// in accordance with the standard license agreement terms provided
// with the Software (see accompanying file LICENSE.rst or
// https://www.steinwurf.com/license), unless otherwise different
// terms and conditions are agreed in writing between Licensee and
// Steinwurf ApS in which case the license will be regulated by that
// separate written agreement.
//
// License for Non-Commercial Usage
// Distributed under the "KODO RESEARCH LICENSE 1.2"
//
// Licensees holding a valid research license may use this project
// in accordance with the license agreement terms provided with the
// Software
//
// See accompanying file LICENSE.rst or https://www.steinwurf.com/license

#include "random_uniform.hpp"

#include "../../version.hpp"

#include <pybind11/pybind11.h>

#include <kodo/perpetual/offset/random_uniform.hpp>

#include <cassert>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace kodo_python
{
inline namespace STEINWURF_KODO_PYTHON_VERSION
{
namespace perpetual
{
namespace offset
{
struct random_uniform_wrapper : kodo::perpetual::offset::random_uniform
{
    std::function<void(const std::string&)> m_log_callback;
};

using random_uniform_type = random_uniform_wrapper;

void offset_random_uniform_enable_log(
    random_uniform_type& offset_generator,
    std::function<void(const std::string&)> callback)
{
    offset_generator.m_log_callback = callback;
    offset_generator.enable_log(
        [](const std::string& message, void* data) {
            random_uniform_type* offset_generator =
                static_cast<random_uniform_type*>(data);
            assert(offset_generator->m_log_callback);
            offset_generator->m_log_callback(message);
        },
        &offset_generator);
}

void random_uniform(pybind11::module m)
{
    using namespace pybind11;
    class_<random_uniform_type>(
        m, "RandomUniform",
        "Generates a random uniform offset for the perpetual code.")
        .def(init<>(), "The RandomUniform constructor.\n")
        .def("configure", &random_uniform_type::configure, arg("symbols"),
             arg("width"),
             "Configure the offset generator with the given parameters. This "
             "is useful for reusing an existing coder. Note that the "
             "reconfiguration always implies a reset, so the coder will be in "
             "a clean state after this operation.\n\n"
             "\t:param symbols: The number of symbols in a coding block.\n"
             "\t:param width: The width is the number of randomly generated "
             "coefficients.")
        .def(
            "reset", &random_uniform_type::reset,
            "Resets the generator to the state when it was first configured.\n")
        .def_property_readonly(
            "symbols", &random_uniform_type::symbols,
            "Return the number of symbols supported by this generator.\n")
        .def_property_readonly(
            "width", &random_uniform_type::width,
            "Return the width of the generator in symbols.\n")
        .def("offset", &random_uniform_type::offset,
             "Return the next offset.\n")
        .def("set_seed", &random_uniform_type::set_seed, arg("seed"),
             "Set the seed for the offset generator.\n\n"
             "\t:param seed: The chosen seed.\n")
        .def(
            "enable_log", &offset_random_uniform_enable_log, arg("callback"),
            "Enable logging for the generator.\n\n"
            "\t:param callback: The callback used for handling log messages.\n")
        .def("disable_log", &random_uniform_type::disable_log,
             "Disables the log.\n")
        .def("is_log_enabled", &random_uniform_type::is_log_enabled,
             "Return True if log is enabled, otherwise False.\n");
}
}
}
}
}
