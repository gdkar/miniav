_Pragma("once")

#include "tinycommon.hpp"
namespace tinyav {
struct avcodec_ctx {
    AVCodecContext     *m_d{nullptr};
    avcodec_ctx() = default;
    avcodec_ctx(AVCodecContext *f)
        : m_d(f) { }
    avcodec_ctx(avcodec_ctx &&o)
        : m_d(o.release()) { }
    avcodec_ctx(const avcodec_ctx &o)
    : m_d(avcodec_alloc_context3(nullptr))
    {
        avcodec_copy_context(m_d, o.m_d);
    }
    avcodec_ctx(AVCodecParameters *params)
    {
        alloc(params);
    }
    avcodec_ctx &operator = (AVCodecContext *f)
    {
        reset(f);
        return *this;
    }
    avcodec_ctx &operator = (const AVCodecContext *f)
    {
        reset();
        alloc(f->codec);
        avcodec_copy_context(m_d, f);
        return *this;
    }
    avcodec_ctx &operator = (const avcodec_ctx &f)
    {
        reset();
        alloc(f->codec);
        avcodec_copy_context(m_d, f.m_d);
        return *this;
    }
        avcodec_ctx &operator =(avcodec_ctx &&o)
    {
        reset(o.release());
        return *this;
    }
    void swap(avcodec_ctx &o) { std::swap(m_d,o.m_d);}
    void reset(AVCodecContext *f = nullptr)
    {
        if(f != m_d) {
            if(is_open())
                close();
            avcodec_free_context(&m_d);
        }
        m_d = f;
    }
    AVCodecContext *release() { auto ret = m_d; m_d = nullptr; return ret;}
    AVCodecContext *get() const { return m_d;}
   ~avcodec_ctx() { reset();}
    AVCodecContext *operator ->() { return m_d; }
    AVCodecContext *operator ->() const { return m_d;}
    AVCodecContext &operator *() { return *m_d;}
    const AVCodecContext &operator *() const { return *m_d;}
    int open(const AVCodec *codec = nullptr, AVDictionary **opts = nullptr)
    {
        if(!m_d)
            alloc(codec);
        return avcodec_open2(m_d,codec ? codec : m_d->codec,opts);
    }
    void close()
    {
        avcodec_close(m_d);
    }
    operator bool() const
    {
        return !!m_d;
    }
    operator AVCodecContext *() const { return m_d;}
    void alloc(const AVCodec *dec = nullptr)
    {
        if(!m_d)
            m_d = avcodec_alloc_context3(dec);
    }
    void alloc(AVCodecParameters *par, bool input = true)
    {
        if(m_d)
            close();
        else
            alloc();
        if(input)
            m_d->codec = avcodec_find_decoder(par->codec_id);
        else
            m_d->codec = avcodec_find_encoder(par->codec_id);
        avcodec_parameters_to_context(m_d, par);
    }
    void opt_set_int(const char *name, int val){ av_opt_set_int(m_d, name, val,0);}
    void opt_set_sample_fmt(const char *name, AVSampleFormat val){ av_opt_set_sample_fmt(m_d, name, val,0);}
    void opt_set_channel_layout(const char *name, int64_t val){ av_opt_set_channel_layout(m_d, name, val,0);}
    void opt_set(const char *name, int val) { opt_set_int(name,val);}
    void opt_set(const char *name, AVSampleFormat val) { opt_set_sample_fmt(name,val);}
    void opt_set(const char *name, int64_t val) { opt_set_channel_layout(name, val);}
    bool is_open() const { return avcodec_is_open(m_d);}
    bool is_input() const { return m_d && av_codec_is_decoder(m_d->codec);}
    bool is_output() const { return m_d && av_codec_is_encoder(m_d->codec);}
    void flush()
    {
        avcodec_flush_buffers(m_d);
    }
    int send_packet(AVPacket *pkt)
    {
        return avcodec_send_packet(m_d, pkt);
    }
    int receive_frame(AVFrame *frame)
    {
        return avcodec_receive_frame(m_d,frame);
    }
    int send_frame(AVFrame *frame)
    {
        return avcodec_send_frame(m_d,frame);
    }
    int receive_packet(AVPacket *pkt)
    {
        return avcodec_receive_packet(m_d,pkt);
    }
};
}
inline void swap(tinyav::avcodec_ctx &lhs, tinyav::avcodec_ctx &rhs){ lhs.swap(rhs);}
