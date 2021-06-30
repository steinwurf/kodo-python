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

#include <kodo/slide/generator/random_uniform.hpp>
#include <kodo/version.hpp>

#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

namespace kodo_python
{
inline namespace STEINWURF_KODO_PYTHON_VERSION
{
namespace slide
{
namespace generator
{
struct random_uniform_wrapper : kodo::slide::generator::random_uniform
{
    random_uniform_wrapper(kodo::finite_field field) :
        kodo::slide::generator::random_uniform(field)
    {
    }
    std::function<void(const std::string&)> m_log_callback;
};

using random_uniform_type = random_uniform_wrapper;

void slide_generator_random_uniform_enable_log(
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

void slide_generator_random_uniform_generate(
    random_uniform_type& generator, pybind11::handle coefficients_handle,
    std::size_t lower_bound, std::size_t symbols)
{
    PyObject* coefficients_obj = coefficients_handle.ptr();

    if (!PyByteArray_Check(coefficients_obj))
    {
        throw pybind11::type_error("coefficients: expected type bytearray");
    }

    generator.generate((uint8_t*)PyByteArray_AsString(coefficients_obj),
                       lower_bound, symbols);
}

void random_uniform(pybind11::module& m)
{
    using namespace pybind11;
    class_<random_uniform_type>(
        m, "RandomUniform", "The Kodo uniform random sliding window generator")
        .def(init<kodo::finite_field>(), arg("field"),
             "The RandomUniform constructor\n\n"
             "\t:param field: the chosen finite field.\n")
        .def_property_readonly("field", &random_uniform_type::field,
                               "Return the configured finite field.\n")
        .def("generate", &slide_generator_random_uniform_generate,
             arg("coefficients"), arg("lower_bound"), arg("symbols"),
             "Generates the coefficients.\n\n"
             ":param coefficients: The data buffer where the coefficients will "
             "be written.\n"
             ":param lower_bound: The lower bound of the coder's window.\n"
             ":param symbols: The number of symbols in the coder's window.\n")
        .def("set_seed", &random_uniform_type::set_seed, arg("seed"),
             "Sets the state of the coefficient generator. The coefficient "
             "generator will always produce the same set of coefficients for a "
             "given seed.\n")
        .def("coefficients_bytes", &random_uniform_type::coefficients_bytes,
             arg("lower_bound"), arg("symbols"),
             "Return the number of bytes to be generated when calling generate "
             "next time.\n\n"
             ":param lower_bound: The lower bound of the coder's window.\n"
             ":param symbols: The number of symbols in the coder's window.\n")
        .def("enable_log", &slide_generator_random_uniform_enable_log,
             "Enables the log.\n\n"
             ":param callback: The callback which handles the log message.\n")
        .def("disable_log", &random_uniform_type::disable_log,
             "Disables the log.\n")
        .def("is_log_enabled", &random_uniform_type::is_log_enabled,
             "Return True if log is enabled, otherwise False");
}
}
}
}
}
