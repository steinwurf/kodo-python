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

#include "rate_controller.hpp"

#include "../version.hpp"

#include <pybind11/pybind11.h>

#include <kodo/slide/rate_controller.hpp>

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

void rate_controller(pybind11::module& m)
{
    using namespace pybind11;
    class_<kodo::slide::rate_controller>(
        m, "RateController", "The Kodo sliding window rate-controller")
        .def(init<std::size_t, std::size_t>(), arg("n"), arg("k"),
             "The RateController constructor.\n\n"
             ":param n: The number of symbols to send.\n"
             ":param k: The number of source symbols to send.\n"
             "example: n=4 and k=2 means you will send 2 source symbols and "
             "4-2 = 2 repair symbols.\n")
        .def("advance", &kodo::slide::rate_controller::advance,
             "Increment position to be a number from 0 to n.\n")
        .def("send_repair", &kodo::slide::rate_controller::send_repair,
             "If True, we should generate a repair symbol. Otherwise send a "
             "source"
             "symbol.\n");
}
}
}
}
