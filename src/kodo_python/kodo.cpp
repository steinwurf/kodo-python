// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <pybind11/pybind11.h>

#include <string>

#include <fifi/finite_field.hpp>

#include <cpuid/version.hpp>
#include <fifi/version.hpp>
#include <kodo_core/version.hpp>

#if !defined(KODO_PYTHON_DISABLE_RLNC)
    #include <kodo_rlnc/version.hpp>
#endif

#if !defined(KODO_PYTHON_DISABLE_PERPETUAL)
    #include <kodo_perpetual/version.hpp>
#endif

#if !defined(KODO_PYTHON_DISABLE_FULCRUM)
    #include <kodo_fulcrum/version.hpp>
#endif

namespace kodo_python
{
// Forward declarations of "create" functions implemented in other cpp files
void create_carousel_stacks(pybind11::module& m);
void create_rlnc_stacks(pybind11::module& m);
void create_perpetual_stacks(pybind11::module& m);
void create_fulcrum_stacks(pybind11::module& m);

void create_stacks(pybind11::module& m)
{
    pybind11::enum_<fifi::finite_field>(m, "field")
    .value("binary", fifi::finite_field::binary)
    .value("binary4", fifi::finite_field::binary4)
    .value("binary8", fifi::finite_field::binary8)
    .value("binary16", fifi::finite_field::binary16);

#if !defined(KODO_PYTHON_DISABLE_NOCODE)
    create_carousel_stacks(m);
#endif

#if !defined(KODO_PYTHON_DISABLE_RLNC)
    create_rlnc_stacks(m);
#endif

#if !defined(KODO_PYTHON_DISABLE_PERPETUAL)
    create_perpetual_stacks(m);
#endif

#if !defined(KODO_PYTHON_DISABLE_FULCRUM)
    create_fulcrum_stacks(m);
#endif
}

std::string version()
{
    std::string version = std::string("kodo-python: ");
    version += STEINWURF_KODO_PYTHON_VERSION;

    // Add dependency versions:

    version += std::string("\n\tpybind11: ");
#ifdef STEINWURF_PYBIND11_VERSION
    version += std::string(STEINWURF_PYBIND11_VERSION);
#endif

    version += std::string("\n\tboost: ");
#ifdef STEINWURF_BOOST_VERSION
    version += std::string(STEINWURF_BOOST_VERSION);
#endif

    version += std::string("\n\tcpuid: ");
#ifdef STEINWURF_CPUID_VERSION
    version += cpuid::version();
#endif

    version += std::string("\n\tfifi: ");
#ifdef STEINWURF_FIFI_VERSION
    version += fifi::version();
#endif

    version += std::string("\n\tkodo-core: ");
#ifdef STEINWURF_KODO_CORE_VERSION
    version += kodo_core::version();
#endif

#if !defined(KODO_PYTHON_DISABLE_RLNC)
    version += std::string("\n\tkodo-rlnc: ");
#ifdef STEINWURF_KODO_RLNC_VERSION
    version += kodo_rlnc::version();
#endif
#endif

#if !defined(KODO_PYTHON_DISABLE_PERPETUAL)
    version += std::string("\n\tkodo-perpetual: ");
#ifdef STEINWURF_KODO_PERPETUAL_VERSION
    version += kodo_perpetual::version();
#endif
#endif

#if !defined(KODO_PYTHON_DISABLE_FULCRUM)
    version += std::string("\n\tkodo-fulcrum: ");
#ifdef STEINWURF_KODO_FULCRUM_VERSION
    version += kodo_fulcrum::version();
#endif
#endif

    version += std::string("\n\tplatform: ");
#ifdef STEINWURF_PLATFORM_VERSION
    version += std::string(STEINWURF_PLATFORM_VERSION);
#endif

    version += std::string("\n\tstorage: ");
#ifdef STEINWURF_STORAGE_VERSION
    version += std::string(STEINWURF_STORAGE_VERSION);
#endif

    return version;
}

PYBIND11_MODULE(kodo, m)
{
    pybind11::options options;
    options.disable_function_signatures();

    m.attr("__version__") = version();

    create_stacks(m);
}
}
