_Pragma("once")

#include "tinycommon.hpp"

constexpr bool sample_fmt_is_packed(AVSampleFormat format)
{
    return static_cast<int>(format) > static_cast<int>(AV_SAMPLE_FMT_NONE) && static_cast<int>(format) <= static_cast<int>(AV_SAMPLE_FMT_DBL);
}
constexpr bool sample_fmt_is_planar(AVSampleFormat format)
{
    return static_cast<int>(format) > static_cast<int>(AV_SAMPLE_FMT_DBL) && static_cast<int>(format) < static_cast<int>(AV_SAMPLE_FMT_NB);
}
template<AVSampleFormat fmt>
struct from_sample_fmt {
};
template<>
struct from_sample_fmt<AV_SAMPLE_FMT_FLT> { using type = float ; using planar = std::false_type;};
template<>
struct from_sample_fmt<AV_SAMPLE_FMT_FLTP>{ using type = float; using planar = std::true_type ;};
template<>
struct from_sample_fmt<AV_SAMPLE_FMT_DBL> { using type = double;using planar = std::false_type;};
template<>
struct from_sample_fmt<AV_SAMPLE_FMT_DBLP>{ using type = double;using planar = std::true_type ;};
template<>
struct from_sample_fmt<AV_SAMPLE_FMT_U8> { using type = uint8_t;using planar = std::false_type ;};
template<>
struct from_sample_fmt<AV_SAMPLE_FMT_U8P>{ using type = uint8_t;using planar = std::true_type;};
template<>
struct from_sample_fmt<AV_SAMPLE_FMT_S16> { using type = int16_t;using planar = std::false_type;};
template<>
struct from_sample_fmt<AV_SAMPLE_FMT_S16P>{ using type = int16_t;using planar = std::true_type;};
template<>
struct from_sample_fmt<AV_SAMPLE_FMT_S32> { using type = int32_t ;using planar = std::false_type;};
template<>
struct from_sample_fmt<AV_SAMPLE_FMT_S32P>{ using type = int32_t ;using planar = std::true_type;};
template<>
struct from_sample_fmt<AV_SAMPLE_FMT_NONE>{ typedef void type; };


template<typename T, bool planar>
struct to_sample_fmt { using type = typename std::integral_constant<AVSampleFormat,AV_SAMPLE_FMT_NONE>;};
template<>
struct  to_sample_fmt<uint8_t,false>{ using type = typename std::integral_constant<AVSampleFormat,AV_SAMPLE_FMT_U8>;};
template<>
struct  to_sample_fmt<uint8_t,true>{ using type = typename std::integral_constant<AVSampleFormat,AV_SAMPLE_FMT_U8P>;};
template<>
struct  to_sample_fmt<int16_t,false>{ using type = typename std::integral_constant<AVSampleFormat,AV_SAMPLE_FMT_S16>;};
template<>
struct  to_sample_fmt<int16_t,true>{ using type = typename std::integral_constant<AVSampleFormat,AV_SAMPLE_FMT_S16P>;};
template<>
struct  to_sample_fmt<int32_t,false>{ using type = typename std::integral_constant<AVSampleFormat,AV_SAMPLE_FMT_S32>;};
template<>
struct  to_sample_fmt<int32_t,true>{ using type = typename std::integral_constant<AVSampleFormat,AV_SAMPLE_FMT_S32P>;};
template<>
struct  to_sample_fmt<float,false>{ using type = typename std::integral_constant<AVSampleFormat,AV_SAMPLE_FMT_FLT>;};
template<>
struct  to_sample_fmt<float,true>{ using type = typename std::integral_constant<AVSampleFormat,AV_SAMPLE_FMT_FLTP>;};
template<>
struct  to_sample_fmt<double,false>{ using type = typename std::integral_constant<AVSampleFormat,AV_SAMPLE_FMT_DBL>;};
template<>
struct  to_sample_fmt<double,true>{ using type = typename std::integral_constant<AVSampleFormat,AV_SAMPLE_FMT_DBLP>;};
template<typename T, bool planar>
struct to_alternate_sample_fmt { using type = typename to_sample_fmt<T, !planar>::type::type; };
