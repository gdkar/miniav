_Pragma("once")
#include "tinycommon.hpp"
namespace tinyav {
template<class Derived, class T>
struct avobject {
    inline void *get_T() const
    {
        return static_cast<T*>(static_cast<const Derived*>(this)->get());
    }
    void opt_set_defaults()                        { av_opt_set_defaults(get_T()); }
    void opt_set_defaults(int mask, int flags = 0) { av_opt_set_defaults2(get_T(),mask,flags); }
    bool opt_flag_is_set(const char *field_name, const char *flag_name)
    {
        return av_opt_flag_is_set(get_T(), field_name, flag_name);
    }
    int opt_set(struct AVDictionary **opts)
    {
        return av_opt_set_dict(get_T(), opts);
    }
    int opt_set(struct AVDictionary **opts, int flags)
    {
        return av_opt_et_dict2(get_T(), opts, flags);
    }
    int opt_set(const char *name, const char *val, int flags = 0)
    {
        return av_opt_set(get_T(), name, val, flags);
    }
    int opt_set(const char *name, int64_t val, int flags = 0)
    {
        return av_opt_set_int(get_T(), name, val, flags);
    }
    int opt_set(const char *name, const AVRational &val, int flags)
    {
        return av_opt_set_q(get_T(), name, val, flags = 0);
    }
    int opt_set(const char *name, const uint8_t *val, int size, int flags)
    {
        return av_opt_set_bin(get_T(), name, val, size, flags);
    }
    int opt_set(const char *name, AVPixelFormat fmt, int flags = 0)
    {
        return av_opt_set_pixel_fmt(get_T(),name,fmt,flags);
    }
    int opt_set(const char *name, AVSampleFormat fmt, int flags = 0)
    {
        return av_opt_set_sample_fmt(get_T(), name, fmt, flags);
    }
    int opt_set(const char *name, int w, int h, int flags)
    {
        return av_opt_set_image_fmt(get_T(), name, w, h, flags);
    }
    int opt_set(const char *name, AVDictionary *val, int flags = 0)
    {
        return av_opt_set_dict_val(get_T(), name,val,flags);
    }
    int opt_get(const char *name, int&val, int flags = 0) const
    {
        auto tmp = int64_t{0};
        auto ret = opt_get(name, tmp, flags);
        val = tmp;
        return ret;
    }
    int opt_get(const char *name, int64_t &val, int flags = 0) const
    {
        return av_opt_get_int(get_T(),name,flags,&val);
    }
    int opt_get(const char *name, double &val, int flags = 0) const
    {
        return av_opt_get_double(get_T(),flags, &val);
    }
    int opt_get(const char *name, AVRational&val, int flags = 0) const
    {
        return av_opt_get_q(get_T(),flags, &val);
    }
    int opt_get(const char *name, AVPixelFormat &val, int flags = 0) const
    {
        return av_opt_get_pixel_fmt(get_T(),flags, &val);
    }
    int opt_get(const char *name, AVSampleFormat &val, int flags = 0) const
    {
        return av_opt_get_sample_fmt(get_T(), &val, flags);
    }
    int opt_get(const char *name, AVDictionary **val, int flags = 0) const
    {
        return av_opt_get_image_size(get_T(),flags, val);
    }
    std::string opt_get_str(const char *name, int flags = 0)
    {
            auto val = static_cast<uint8_t*>(nullptr);
        av_opt_get(get_T(), name, flags, &val);
        if(val) {
            auto res = std::string(val);
            av_freep(&val);
            return res;
        }else{
            return std::string{};
        }
    }
};

}
