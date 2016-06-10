_Pragma("once")

#include "tinycommon.hpp"
#include "avobject.hpp"
namespace tinyav {
    class swr_ctx : public avobject<swr_ctx, SwrContext>{

    public:
        SwrContext     *m_d{nullptr};
        swr_ctx() = default;
        swr_ctx(SwrContext *f)
            : m_d(f) { }
        swr_ctx(swr_ctx &&o)
            : m_d(o.release()) { }
        swr_ctx(const swr_ctx &o) = delete;
        swr_ctx &operator = (SwrContext *f)
        {
            reset(f);
            return *this;
        }
        swr_ctx &operator = (const SwrContext *f) = delete;
            swr_ctx &operator =(swr_ctx &&o)
        {
            reset(o.release());
            return *this;
        }
        swr_ctx &operator = (const swr_ctx &o) = delete;
        void swap(swr_ctx &o) { std::swap(m_d,o.m_d);}
        void reset(SwrContext *f = nullptr) { swr_free(&m_d); m_d = f;}
        SwrContext *release() { auto ret = m_d; m_d = nullptr; return ret;}
        SwrContext *get() const { return m_d;}
    ~swr_ctx() { reset();}
        SwrContext *operator ->() { return m_d; }
        SwrContext *operator ->() const { return m_d;}
        SwrContext &operator *() { return *m_d;}
        const SwrContext &operator *() const { return *m_d;}
        bool initialized() const { return swr_is_initialized(m_d);}
        bool init() { return m_d && !swr_init(m_d);}
        void close() { swr_close(m_d);}
        bool config(AVFrame *dst, AVFrame *src) { return !swr_config_frame(m_d,dst,src);}
        bool convert(AVFrame *dst, AVFrame *src) { return !swr_convert_frame(m_d,dst,src);}
        int64_t delay(int64_t base) const { return swr_get_delay(m_d,base);}
        operator bool() const { return !!m_d;}
        operator SwrContext *() const { return m_d;}
        void alloc() { if(!m_d) m_d = swr_alloc();}
        int  drop_output(int samples) { return swr_drop_output(m_d, samples);}
        int inject_silence(int samples) { return swr_inject_silence(m_d,samples);};
        int64_t next_pts(int64_t pts) { return swr_next_pts(m_d,pts);}
    };
};
inline void swap(tinyav::swr_ctx &lhs, tinyav::swr_ctx &rhs){ lhs.swap(rhs);}
