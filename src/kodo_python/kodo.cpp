// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <boost/python.hpp>

#include <string>

#include <fifi/api/field.hpp>

namespace kodo_python
{
// Forward declarations of "create" functions implemented in other cpp files
void create_carousel_stacks();
void create_rlnc_stacks();
void create_perpetual_stacks();
void create_fulcrum_stacks();

void create_stacks()
{
    using namespace boost::python;

    enum_<fifi::api::field>("field")
    .value("binary", fifi::api::field::binary)
    .value("binary4", fifi::api::field::binary4)
    .value("binary8", fifi::api::field::binary8)
    .value("binary16", fifi::api::field::binary16);

#if !defined(KODO_PYTHON_DISABLE_NOCODE)
    create_carousel_stacks();
#endif

#if !defined(KODO_PYTHON_DISABLE_RLNC)
    create_rlnc_stacks();
#endif

#if !defined(KODO_PYTHON_DISABLE_PERPETUAL)
    create_perpetual_stacks();
#endif

#if !defined(KODO_PYTHON_DISABLE_FULCRUM)
    create_fulcrum_stacks();
#endif
}

std::string version()
{
    std::string version = std::string("kodo-python: ");
    version += STEINWURF_KODO_PYTHON_VERSION;

    // Add dependency versions:

    version += std::string("\n\tboost: ");
#ifdef STEINWURF_BOOST_VERSION
    version += std::string(STEINWURF_BOOST_VERSION);
#endif

    version += std::string("\n\tcpuid: ");
#ifdef STEINWURF_CPUID_VERSION
    version += std::string(STEINWURF_CPUID_VERSION);
#endif

    version += std::string("\n\tfifi: ");
#ifdef STEINWURF_FIFI_VERSION
    version += std::string(STEINWURF_FIFI_VERSION);
#endif

    version += std::string("\n\tkodo-core: ");
#ifdef STEINWURF_KODO_CORE_VERSION
    version += std::string(STEINWURF_KODO_CORE_VERSION);
#endif

#if !defined(KODO_PYTHON_DISABLE_RLNC)
    version += std::string("\n\tkodo-rlnc: ");
#ifdef STEINWURF_KODO_RLNC_VERSION
    version += std::string(STEINWURF_KODO_RLNC_VERSION);
#endif
#endif

#if !defined(KODO_PYTHON_DISABLE_PERPETUAL)
    version += std::string("\n\tkodo-perpetual: ");
#ifdef STEINWURF_KODO_PERPETUAL_VERSION
    version += std::string(STEINWURF_KODO_PERPETUAL_VERSION);
#endif
#endif

#if !defined(KODO_PYTHON_DISABLE_FULCRUM)
    version += std::string("\n\tkodo-fulcrum: ");
#ifdef STEINWURF_KODO_FULCRUM_VERSION
    version += std::string(STEINWURF_KODO_FULCRUM_VERSION);
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

void create_version_function()
{
    using namespace boost::python;
    scope().attr("__version__") = version();
}

BOOST_PYTHON_MODULE(kodo)
{
    boost::python::docstring_options doc_options;
    doc_options.disable_signatures();
    create_version_function();
    create_stacks();
}
}
