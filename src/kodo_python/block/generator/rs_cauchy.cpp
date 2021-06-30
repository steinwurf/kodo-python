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

#include "rs_cauchy.hpp"

#include "../../version.hpp"

#include <pybind11/pybind11.h>

#include <kodo/block/generator/rs_cauchy.hpp>
#include <kodo/version.hpp>

#include <cassert>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace kodo_python
{
inline namespace STEINWURF_KODO_PYTHON_VERSION
{
namespace block
{
namespace generator
{
struct rs_cauchy_wrapper : kodo::block::generator::rs_cauchy
{
    rs_cauchy_wrapper(kodo::finite_field field) :
        kodo::block::generator::rs_cauchy(field)
    {
    }
    std::function<void(const std::string&)> m_log_callback;
};

using rs_cauchy_type = rs_cauchy_wrapper;

void generator_rs_cauchy_enable_log(
    rs_cauchy_type& generator, std::function<void(const std::string&)> callback)
{
    generator.m_log_callback = callback;
    generator.enable_log(
        [](const std::string& message, void* data) {
            rs_cauchy_type* generator = static_cast<rs_cauchy_type*>(data);
            assert(generator->m_log_callback);
            generator->m_log_callback(message);
        },
        &generator);
}
auto block_generator_rs_cauchy_generate(rs_cauchy_type& generator,
                                        pybind11::handle coefficients_handle)
{
    PyObject* coefficients_obj = coefficients_handle.ptr();

    if (!PyByteArray_Check(coefficients_obj))
    {
        throw pybind11::type_error("coefficients: expected type bytearray");
    }

    return generator.generate((uint8_t*)PyByteArray_AsString(coefficients_obj));
}

void block_generator_rs_cauchy_generate_specific(
    rs_cauchy_type& generator, pybind11::handle coefficients_handle,
    std::size_t index)

{
    PyObject* coefficients_obj = coefficients_handle.ptr();

    if (!PyByteArray_Check(coefficients_obj))
    {
        throw pybind11::type_error("coefficients: expected type bytearray");
    }

    if (index >= generator.repair_symbols())
    {
        throw pybind11::value_error("index: must be less than repair_symbols");
    }

    generator.generate_specific(
        (uint8_t*)PyByteArray_AsString(coefficients_obj), index);
}

void rs_cauchy(pybind11::module& m)
{
    using namespace pybind11;
    class_<rs_cauchy_type>(m, "RSCauchy",
                           "The Kodo Reed-Solomon-Cauchy generator")
        .def(init<kodo::finite_field>(), arg("field"),
             "The Reed-Solomon-Cauchy generator constructor\n\n"
             "\t:param field: the chosen finite field.\n")
        .def("configure", &rs_cauchy_type::configure, arg("symbols"),
             "Configure the generator with the given parameters. This is "
             "useful for reusing an existing generator. Note that the "
             "reconfiguration always implies a reset, so the generator will be "
             "in a clean state after this operation.\n\n"
             "\t:param symbols: The number of symbols in a coding block.\n")
        .def(
            "reset", &rs_cauchy_type::reset,
            "Resets the generator to the state when it was first configured.\n")
        .def_property_readonly("field", &rs_cauchy_type::field,
                               "Return the configured finite field.\n")
        .def_property_readonly(
            "symbols", &rs_cauchy_type::symbols,
            "Return the number of symbols supported by this generator.\n")
        .def_property_readonly("repair_symbols",
                               &rs_cauchy_type::repair_symbols,
                               "Return the number of repair symbols supported "
                               "by this generator.\n")
        .def_property_readonly(
            "remaining_repair_symbols",
            &rs_cauchy_type::remaining_repair_symbols,
            "Return the number of remaining repair symbols supported by this "
            "generator. This number decreases with each call to "
            "RSCauchy.generate().\n")
        .def("generate", &block_generator_rs_cauchy_generate,
             arg("coefficients"),
             "Generates the coefficients.\n\n"
             "\t:param coefficients: The data buffer where the coefficients "
             "will be stored."
             "\t:return: the index of the generated coefficients. This can be "
             "used with RSCauchy.generate_specific() to recreate the "
             "coefficients at a later time.")
        .def("generate_specific", &block_generator_rs_cauchy_generate_specific,
             "Generate a specific set of coefficients.\n\n"
             "\t:param coefficients: The data buffer where the coefficients "
             "will be stored.\n"
             "\t:param index: The index of the coefficients to generate. The "
             "index must be less than or equal to RSCauchy.repair_symbols().\n")
        .def_property_readonly("max_coefficients_bytes",
                               &rs_cauchy_type::max_coefficients_bytes,
                               "Return the maximum number of bytes to be "
                               "generated when calling generate.\n")
        .def(
            "enable_log", &generator_rs_cauchy_enable_log, arg("callback"),
            "Enable logging for this generator.\n\n"
            "\t:param callback: The callback used for handling log messages.\n")
        .def("disable_log", &rs_cauchy_type::disable_log, "Disables the log.\n")
        .def("is_log_enabled", &rs_cauchy_type::is_log_enabled,
             "Return True if log is enabled, otherwise False.\n");
}
}
}
}
}
