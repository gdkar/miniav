_Pragma("once")
#include "tinycommon.hpp"

template<typename T, typename U>
struct avopt_ptr {
    inline void *get_voidp() const
    {
        return static_cast<void*>(static_cast<T*>(*static_cast<U*>(this)));
    }
    void opt_set_defaults()                        { av_opt_set_defaults(get_voidp()); }
    void opt_set_defaults(int mask, int flags = 0) { av_opt_set_defaults2(get_voidp(),mask,flags); }
    bool opt_flag_is_set(const char *field_name, const char *flag_name)
    {
        return av_opt_flag_is_set(get_voidp(), field_name, flag_name);
    }
    int opt_set(struct AVDictionary **opts)
    {
        return av_opt_set_dict(get_voidp(), opts);
    }
    int opt_set(struct AVDictionary **opts, int flags)
    {
        return av_opt_et_dict2(get_voidp(), opts, flags);
    }
    int opt_set(const char *name, const char *val, int flags = 0)
    {
        return av_opt_set(get_voidp(), name, val, search_flags);
    }
    int opt_set(const char *name, int64_t val, int flags = 0)
    {
        return av_opt_set_int(get_voidp(), name, val, search_flags);
    }
    int opt_set(const char *name, const AVRational &val, int search_flags)
    {
        return av_opt_set_q(get_voidp(), name, val, search_flags = 0);
    }
    int opt_set(const char *name, const uint8_t *val, int size, int search_flags)
    {
        return av_opt_set_bin(get_voidp(), name, val, size, search_flags);
    }
    int opt_set(const char *name, AVPixelFormat fmt, int search_flags = 0)
    {
        return av_opt_set_pixel_fmt(get_voidp(),name,fmt,search_flags);
    }
    int opt_set(const char *name, AVSampleFormat fmt, int search_flags = 0)
    {
        return av_opt_set_sample_fmt(get_voidp(), name, fmt, search_flags);
    }
    int opt_set(const char *name, int w, int h, int search_flags)
    {
        return av_opt_set_image_fmt(get_voidp(), name, w, h, search_flags);
    }
    int opt_set(const char *name, AVDictionary *val, int search_flags = 0)
    {
        return av_opt_set_dict_val(get_voidp(), name,val,search_flags);
    }
}
