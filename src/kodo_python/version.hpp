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

#include <string>

namespace kodo_python
{
/// Here we define the STEINWURF_KODO_PYTHON_VERSION this should be updated
/// on each release
#define STEINWURF_KODO_PYTHON_VERSION v1_0_0

inline namespace STEINWURF_KODO_PYTHON_VERSION
{
/// @return The version of the library as string
std::string version();
}
}