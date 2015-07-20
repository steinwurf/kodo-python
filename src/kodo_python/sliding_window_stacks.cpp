// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <kodo/rlnc/sliding_window_encoder.hpp>
#include <kodo/rlnc/sliding_window_decoder.hpp>

#include "create_helpers.hpp"

namespace kodo_python
{

    template<class Encoder>
    void read_feedback(Encoder& encoder, const std::string& feedback)
    {
        std::vector<uint8_t> _feedback(feedback.length());
        std::copy(
            feedback.c_str(),
            feedback.c_str() + feedback.length(),
            _feedback.data());
        encoder.read_feedback(_feedback.data());
    }

    template<>
    struct extra_encoder_methods<kodo::rlnc::sliding_window_encoder>
    {
        template<class EncoderClass>
        extra_encoder_methods(EncoderClass& encoder_class)
        {
            encoder_class
            .def("feedback_size", &EncoderClass::wrapped_type::feedback_size,
                "Return the required feedback buffer size in bytes.\n\n"
                "\t:returns: The required feedback buffer size in bytes.\n"
                )
            .def("read_feedback", &read_feedback<typename EncoderClass::wrapped_type>,
                "Return the feedback information.\n\n"
                "\t:returns: The feedback information.\n");
        }
    };

    template<class Decoder>
    PyObject* write_feedback(Decoder& decoder)
    {
        std::vector<uint8_t> payload(decoder.feedback_size());
        uint32_t length = decoder.write_feedback(payload.data());
        #if PY_MAJOR_VERSION >= 3
        return PyBytes_FromStringAndSize((char*)payload.data(), length);
        #else
        return PyString_FromStringAndSize((char*)payload.data(), length);
        #endif
    }

    template<>
    struct extra_decoder_methods<kodo::rlnc::sliding_window_decoder>
    {
        template<class DecoderClass>
        extra_decoder_methods(DecoderClass& decoder_class)
        {
            decoder_class
            .def("feedback_size", &DecoderClass::wrapped_type::feedback_size,
                "Return the required feedback buffer size in bytes.\n\n"
                "\t:returns: The required feedback buffer size in bytes.\n"
                )
            .def("write_feedback",
                &write_feedback<typename DecoderClass::wrapped_type>,
                "Return a buffer containing the feedback.\n\n"
                "\t:returns: A buffer containing the feedback.\n");
        }
    };

    void create_sliding_window_stacks()
    {
        using namespace kodo::rlnc;

        create_encoder<sliding_window_encoder>("SlidingWindow");
        create_decoder<sliding_window_decoder>("SlidingWindow");
    }
}
