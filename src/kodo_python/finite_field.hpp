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

#pragma once

#include <pybind11/pybind11.h>

#include <kodo/finite_field.hpp>

#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

namespace kodo_python
{
inline namespace STEINWURF_KODO_PYTHON_VERSION
{
namespace
{
kodo::finite_field from_value(uint8_t value)
{
    return static_cast<kodo::finite_field>(value);
}
}

void finite_field(pybind11::module& m)
{
    pybind11::enum_<kodo::finite_field>(m, "FiniteField")
        .value("binary", kodo::finite_field::binary,
               "The binary field, containing the two elements {0,1}.")
        .value("binary4", kodo::finite_field::binary4,
               "A binary extension field with 2⁴ elements")
        .value("binary8", kodo::finite_field::binary8,
               "A binary extension field with 2⁸ elements")
        .value("binary16", kodo::finite_field::binary16,
               "A binary extension field with 2¹⁶ elements")
        .def("from_value", &from_value);
}
}
}
