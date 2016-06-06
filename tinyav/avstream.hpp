_Pragma("once")


#include "tinycommon.hpp"
#include "avframe.hpp"
#include "avpacket.hpp"
#include "avformat_ctx.hpp"
#include "avcodec_ctx.hpp"

namespace tinyav{
    class avstream {
        public:
            AVStream        *m_d{nullptr};
            avcodec_ctx      m_c{};
            AVFormatContext *m_f{nullptr};
            
            avstream() = default;
            avstream(const avstream &s) = delete;
            avstream &operator =(const avstream &s) = delete;
            avstream(avstream && o)
            : m_d(o.m_d)
            , m_c(o.m_c.release())
            , m_f(o.m_f)
            { }
            avstream &operator =(avstream && o)
            {
                close();
                m_d = o.m_d;
                m_c.reset(o.m_c.release());
                m_f = o.m_f;
                return *this;
            }
            avstream(AVFormatContext *f, AVStream *s)
                :m_d(s)
                ,m_c(s->codecpar)
                ,m_f(f) {
                if(s != f->streams[s->index])
                    throw std::system_error(EINVAL,std::system_category());
                m_c.open();
            }
            avstream(AVFormatContext *f, int i)
            : avstream(f,f->streams[i])
            {}
            avstream(AVFormatContext *f, AVCodec *c)
            : avstream(f, avformat_new_stream(f,c))
            { }
            virtual ~avstream()
            {
                close();
            }
            bool is_input() const
            {
                return m_c.is_input();
            }
            bool is_output() const
            {
                return m_c.is_output();
            }
            void close()
            {
                if(m_c.is_output())
                    flush();
                m_c.reset();
                m_f = nullptr;
                m_d = nullptr;
            }
            int flush(bool interleaved = true)
            {
                return write(nullptr,interleaved);
            }
            int write(AVFrame *frame = nullptr, bool interleaved = true)
            {
                auto err = m_c.send_frame(frame);
                auto pkt = avpacket();
                pkt.alloc();
                while(!(err = m_c.receive_packet(pkt))) {
                    pkt->stream_index = m_d->index;
                    if(m_f) {
                        if(interleaved)
                            av_interleaved_write_frame(m_f,pkt);
                        else
                            av_write_frame(m_f,pkt);
                    }
                    pkt.unref();
                }
                return (err == AVERROR_EOF || err == AVERROR(EAGAIN)) ? 0 : err;
            }
    };

};
