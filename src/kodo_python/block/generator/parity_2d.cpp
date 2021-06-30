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

#include "parity_2d.hpp"

#include "../../version.hpp"

#include <pybind11/pybind11.h>

#include <kodo/block/generator/parity_2d.hpp>
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
struct parity_2d_wrapper : kodo::block::generator::parity_2d
{
    std::function<void(const std::string&)> m_log_callback;
};

using parity_2d_type = parity_2d_wrapper;

void generator_parity_2d_enable_log(
    parity_2d_type& generator, std::function<void(const std::string&)> callback)
{
    generator.m_log_callback = callback;
    generator.enable_log(
        [](const std::string& message, void* data) {
            parity_2d_type* generator = static_cast<parity_2d_type*>(data);
            assert(generator->m_log_callback);
            generator->m_log_callback(message);
        },
        &generator);
}

auto block_generator_parity_2d_generate(parity_2d_type& generator,
                                        pybind11::handle coefficients_handle)
{
    PyObject* coefficients_obj = coefficients_handle.ptr();

    if (!PyByteArray_Check(coefficients_obj))
    {
        throw pybind11::type_error("coefficients: expected type bytearray");
    }

    return generator.generate((uint8_t*)PyByteArray_AsString(coefficients_obj));
}

void block_generator_parity_2d_generate_specific(
    parity_2d_type& generator, pybind11::handle coefficients_handle,
    std::size_t position)
{
    PyObject* coefficients_obj = coefficients_handle.ptr();

    if (!PyByteArray_Check(coefficients_obj))
    {
        throw pybind11::type_error("coefficients: expected type bytearray");
    }

    generator.generate_specific(
        (uint8_t*)PyByteArray_AsString(coefficients_obj), position);
}

void parity_2d(pybind11::module& m)
{
    using namespace pybind11;
    class_<parity_2d_type>(m, "Parity2D", "The Kodo Parity2D FEC generator")
        .def(init<>(), "The Parity2D generator constructor.\n")
        .def(
            "configure", &parity_2d_type::configure, arg("rows"),
            arg("columns"),
            "Configure the generator with the given parameters. This is "
            "useful for reusing an existing generator.\n\n"
            "\t:param rows: The number of rows, must be larger than 0.\n"
            "\t:param columns: The number of columns, must be larger than 0.\n")
        .def("reset", &parity_2d_type::reset,
             "Resets the generator to when it was first configured.\n")
        .def_property_readonly("symbols", &parity_2d_type::symbols,
                               "Return the number of symbols. This is equal to "
                               "SMPTE2022.rows * SMPTE2022.columns.\n")
        .def_property_readonly("rows", &parity_2d_type::rows,
                               "Return the number of rows.\n")
        .def_property_readonly("columns", &parity_2d_type::columns,
                               "Return the number of columns.\n")
        .def("can_advance", &parity_2d_type::can_advance,
             "Returns False if the generator reaches beyond the chosen amount "
             "of\n"
             "and repair packets, otherwise True")
        .def("advance", &parity_2d_type::advance,
             "Advances the generator to the next step of the repair process.\n")
        .def("can_generate", &parity_2d_type::can_generate,
             "Returns True if the generator can generate the coefficients at "
             "this point.\n\n")
        .def("generate", &block_generator_parity_2d_generate,
             arg("coefficients"),
             "Generate the coefficients for the current position. Use "
             "SMPTE2022.can_generate() to check if calling this function is "
             "allowed.\n\n"
             "Returns the current position of the generator."
             "\t:param coefficients: The data buffer where the coefficients "
             "will be written.\n")
        .def("generate_specific", &block_generator_parity_2d_generate_specific,
             arg("coefficients"), arg("position"),
             "Generate the coefficients for a specified position.\n\n"
             "\t:param coefficients: The data buffer where the coefficients "
             "will be written.\n"
             "\t:param position: The specific position to generate "
             "coefficients for.\n")
        .def_property_readonly("max_coefficients_bytes",
                               &parity_2d_type::max_coefficients_bytes,
                               "Return the maximum number of bytes to be "
                               "generated when calling generate.\n")
        .def("set_column_redundancy_enabled",
             &parity_2d_type::set_column_redundancy_enabled,
             "Specifies if the generator should generate column redundancy or "
             "not.\n\n"
             "\t:param enabled: A bool. True is enabled, False is disabled.\n")
        .def("column_redundancy_enabled",
             &parity_2d_type::column_redundancy_enabled,
             "Returns True if column redundancy is enabled, otherwise False.\n"
             "Column redundancy is enabled by default.\n")
        .def("set_row_redundancy_enabled",
             &parity_2d_type::set_row_redundancy_enabled,
             "Specifies if the generator should generate row redundancy or "
             "not.\n\n"
             "\t:param enabled: A bool. True is enabled, False is disabled.\n")
        .def("row_redundancy_enabled", &parity_2d_type::row_redundancy_enabled,
             "Returns True if row redundancy is enabled, otherwise False.\n"
             "Row redundancy is enabled by default.\n")
        .def("enable_log", &generator_parity_2d_enable_log, arg("callback"),
             "Enables the log.\n\n"
             "\t:param callback: The callback which handles the log message.")
        .def("disable_log", &parity_2d_type::disable_log, "Disables the log.\n")
        .def("is_log_enabled", &parity_2d_type::is_log_enabled,
             "Return True if log is enabled, otherwise False.\n");
}
}
}
}
}