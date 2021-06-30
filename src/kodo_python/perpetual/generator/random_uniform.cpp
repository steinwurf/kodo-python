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

#include <kodo/perpetual/generator/random_uniform.hpp>

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
namespace generator
{
struct random_uniform_wrapper : kodo::perpetual::generator::random_uniform
{
    random_uniform_wrapper(kodo::finite_field field) :
        kodo::perpetual::generator::random_uniform(field)
    {
    }
    std::function<void(const std::string&)> m_log_callback;
};

using random_uniform_type = random_uniform_wrapper;

void generator_random_uniform_enable_log(
    random_uniform_type& generator,
    std::function<void(const std::string&)> callback)
{
    generator.m_log_callback = callback;
    generator.enable_log(
        [](const std::string& message, void* data) {
            random_uniform_type* generator =
                static_cast<random_uniform_type*>(data);
            assert(generator->m_log_callback);
            generator->m_log_callback(message);
        },
        &generator);
}

void perpetual_generator_random_uniform_generate(
    random_uniform_type& generator, pybind11::handle coefficients_handle)
{
    PyObject* coefficients_obj = coefficients_handle.ptr();

    if (!PyByteArray_Check(coefficients_obj))
    {
        throw pybind11::type_error("coefficients: expected type bytearray");
    }

    if ((std::size_t)PyByteArray_Size(coefficients_obj) <
        generator.coefficients_bytes())
    {
        throw pybind11::value_error(
            "coefficients: not large enough to contain coefficients");
    }

    generator.generate((uint8_t*)PyByteArray_AsString(coefficients_obj));
}

void random_uniform(pybind11::module m)
{
    using namespace pybind11;
    class_<random_uniform_type>(m, "RandomUniform",
                                "The Kodo uniform random perpetual generator")
        .def(init<kodo::finite_field>(), arg("field"),
             "The random_uniform constructor\n\n"
             "\t:param field: the chosen finite field.\n")
        .def("configure", &random_uniform_type::configure, arg("symbols"),
             arg("width"),
             "Configure the generator with the given parameters. This is "
             "useful for reusing an existing coder. Note that the "
             "reconfiguration always implies a reset, so the coder will be in "
             "a clean state after this operation.\n\n"
             "\t:param symbols: The number of symbols in a coding block.\n"
             "\t:param width: The width is the number of randomly generated\n"
             "\t\tcoefficients.\n")
        .def(
            "reset", &random_uniform_type::reset,
            "Resets the generator to the state when it was first configured.\n")
        .def_property_readonly("field", &random_uniform_type::field,
                               "Return the configured finite field.\n")
        .def_property_readonly(
            "symbols", &random_uniform_type::symbols,
            "Return the number of symbols supported by this generator.\n")
        .def_property_readonly(
            "width", &random_uniform_type::width,
            "Return the width of the generator in symbols.\n")
        .def("generate", &perpetual_generator_random_uniform_generate,
             "Generates the coefficients.\n\n"
             "\t:param coefficients: The buffer where the coefficients will be "
             "written.\n")
        .def("set_seed", &random_uniform_type::set_seed, arg("seed"),
             "Sets the state of the coefficient generator. The coefficient "
             "generator will always produce the same set of coefficients for a "
             "given seed and offset.\n\n"
             "\t:param seed: The seed that is used to set the state of the "
             "generator.\n")
        .def("set_offset", &random_uniform_type::set_offset, arg("offset"),
             "Sets the offset of the coefficient generator. The offset is the "
             "index of the first symbol in the encoding. In total width "
             "symbols are combined in each encoded symbol.\n\n"
             "\t:param offset: The offset to be set.\n")
        .def_property_readonly("coefficients_bytes",
                               &random_uniform_type::coefficients_bytes,
                               "Return the number of bytes to be generated "
                               "when calling generate next time.\n")
        .def_property_readonly("max_coefficients_bytes",
                               &random_uniform_type::max_coefficients_bytes,
                               "Return the maximum number of bytes to be "
                               "generated when calling generate next time.\n")
        .def_property_readonly(
            "offset", &random_uniform_type::offset,
            "Return the symbol offset of the perpetual generator. The offset "
            "is the index of the first non-zero coefficient in the width of "
            "coefficients.\n")
        .def(
            "enable_log", &generator_random_uniform_enable_log, arg("callback"),
            "Enable logging for this generator.\n\n"
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
