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

#include <pybind11/pybind11.h>

#include <sstream>
#include <string>

#include <kodo/finite_field.hpp>
#include <kodo/version.hpp>

#include "block/decoder.hpp"
#include "block/encoder.hpp"
#include "block/generator/parity_2d.hpp"
#include "block/generator/random_uniform.hpp"
#include "block/generator/rs_cauchy.hpp"

#include "finite_field.hpp"
#include "version.hpp"

#include "perpetual/decoder.hpp"
#include "perpetual/encoder.hpp"
#include "perpetual/generator/random_uniform.hpp"
#include "perpetual/offset/random_sequence.hpp"
#include "perpetual/offset/random_uniform.hpp"
#include "perpetual/offset/sequential_sequence.hpp"

#include "slide/decoder.hpp"
#include "slide/encoder.hpp"
#include "slide/generator/random_uniform.hpp"
#include "slide/rate_controller.hpp"
namespace kodo_python
{
inline namespace STEINWURF_KODO_PYTHON_VERSION
{
PYBIND11_MODULE(kodo, m)
{
    pybind11::options options;
    options.disable_function_signatures();

    std::stringstream ss;
    ss << "Kodo version: " << kodo::version() << '\n';
    ss << "Kodo python version: " << kodo_python::version();

    m.attr("__version__") = ss.str();
    m.attr("__license__") = "Kodo Evaluation/Research License 1.2";
    m.attr("__copyright__") = "Steinwurf ApS";

    finite_field(m);
    auto block = m.def_submodule("block", "Block codec");
    block::encoder(block);
    block::decoder(block);

    auto block_generator =
        block.def_submodule("generator", "Block codec generators");
    block::generator::random_uniform(block_generator);
    block::generator::rs_cauchy(block_generator);
    block::generator::parity_2d(block_generator);

    auto perpetual = m.def_submodule("perpetual", "Perpetual codec");
    perpetual::encoder(perpetual);
    perpetual::decoder(perpetual);

    auto perpetual_generator =
        perpetual.def_submodule("generator", "Perpetual codec generator");
    perpetual::generator::random_uniform(perpetual_generator);

    auto perpetual_offset =
        perpetual.def_submodule("offset", "Perpetual codec offset");
    perpetual::offset::random_uniform(perpetual_offset);
    perpetual::offset::random_sequence(perpetual_offset);
    perpetual::offset::sequential_sequence(perpetual_offset);

    auto slide = m.def_submodule("slide", "Sliding window codec");
    slide::encoder(slide);
    slide::decoder(slide);
    slide::rate_controller(slide);

    auto slide_generator =
        slide.def_submodule("generator", "Sliding window codec generator");
    slide::generator::random_uniform(slide_generator);
}
}
}
