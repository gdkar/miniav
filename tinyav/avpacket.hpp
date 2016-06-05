_Pragma("once")

#include "tinycommon.h"

struct avpacket {
    AVPacket     *m_d{nullptr};
    avpacket() = default;
    avpacket(AVPacket *f)
        : m_d(f) { }
    avpacket(avpacket &&o)
        : m_d(o.release()) { }
    avpacket(const avpacket &o)
        : m_d(av_packet_clone(o)) { }
    avpacket &operator = (AVPacket *f)
    {
        reset(f);
        return *this;
    }
    avpacket &operator =(avpacket &&o)
    {
        if(m_d) {
            ref(o);
        }else{
            reset(o.release());
        }
        return *this;
    }
    avpacket &operator = (const avpacket &o)
    {
        reset(av_packet_clone(o.m_d));
        return *this;
    }
    void swap(avpacket &o) { std::swap(m_d,o.m_d);}
    void reset(AVPacket *f = nullptr) { av_packet_free(&m_d); m_d = f;}
    AVPacket *release() { auto ret = m_d; m_d = nullptr; return ret;}
    AVPacket *get() const { return m_d;}
   ~avpacket() { reset();}
    AVPacket *operator ->() { return m_d; }
    AVPacket *operator ->() const { return m_d;}
    AVPacket &operator *() { return *m_d;}
    const AVPacket &operator *() const { return *m_d;}
    operator bool() const { return !!m_d;}
    operator AVPacket *() const { return m_d;}
    void alloc() { if(!m_d) m_d = av_packet_alloc(); else unref();}
    void unref() { av_packet_unref(m_d); }
    void ref(avpacket &o) { if(o.m_d != m_d) { unref(); av_packet_ref(m_d,o.m_d); } }
};

inline void swap(avpacket &lhs, avpacket &rhs){ lhs.swap(rhs);}
