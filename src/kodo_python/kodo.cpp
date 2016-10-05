// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <boost/python.hpp>
#include <boost/python/docstring_options.hpp>

#include <string>

namespace kodo_python
{
// Forward declarations of "create" functions implemented in other cpp files
void create_full_vector_stacks();
void create_on_the_fly_stacks();
void create_sliding_window_stacks();
void create_perpertual_stacks();
void create_carousel_stacks();
void create_fulcrum_stacks();

void create_stacks()
{
#if !defined(KODO_PYTHON_DISABLE_NOCODE)
    create_carousel_stacks();
#endif

#if !defined(KODO_PYTHON_DISABLE_RLNC)

#if !defined(KODO_PYTHON_DISABLE_FULL_VECTOR)
    create_full_vector_stacks();
#endif
#if !defined(KODO_PYTHON_DISABLE_ON_THE_FLY)
    create_on_the_fly_stacks();
#endif
#if !defined(KODO_PYTHON_DISABLE_SLIDING_WINDOW)
    create_sliding_window_stacks();
#endif
#if !defined(KODO_PYTHON_DISABLE_PERPETUAL)
    create_perpertual_stacks();
#endif

#endif // !defined(KODO_PYTHON_DISABLE_RLNC)

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
    version += std::string("\n\tkodo-fulcrum: ");
#ifdef STEINWURF_KODO_FULCRUM_VERSION
    version += std::string(STEINWURF_KODO_FULCRUM_VERSION);
#endif
    version += std::string("\n\tkodo-core: ");
#ifdef STEINWURF_KODO_CORE_VERSION
    version += std::string(STEINWURF_KODO_CORE_VERSION);
#endif
    version += std::string("\n\tkodo-rlnc: ");
#ifdef STEINWURF_KODO_RLNC_VERSION
    version += std::string(STEINWURF_KODO_RLNC_VERSION);
#endif
    version += std::string("\n\tmeta: ");
#ifdef STEINWURF_META_VERSION
    version += std::string(STEINWURF_META_VERSION);
#endif
    version += std::string("\n\tplatform: ");
#ifdef STEINWURF_PLATFORM_VERSION
    version += std::string(STEINWURF_PLATFORM_VERSION);
#endif
    version += std::string("\n\trecycle: ");
#ifdef STEINWURF_RECYCLE_VERSION
    version += std::string(STEINWURF_RECYCLE_VERSION);
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
