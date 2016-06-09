_Pragma("once")

#include "tinycommon.hpp"

namespace tinyav{
struct defer_frame_t { };
constexpr const defer_frame_t defer_frame{};
class avframe {
public:
    struct ItemRef {
        constexpr ItemRef() = default;
        constexpr ItemRef(ItemRef&&) = default;
        constexpr ItemRef(const ItemRef&) = default;
        inline ItemRef(const avframe&frm, int sample = 0)
            : _data(frm.data())
            , _sample(sample)
            , _stride(frm.word_stride())
            , _planes(frm.planes()){ }

        float **      _data{nullptr};
              int     _sample{0};
              int16_t _stride{1};
              int16_t _planes{1};
        inline ItemRef& operator=(ItemRef&&) = default;
        inline ItemRef& operator=(const ItemRef&) = default;
        constexpr float &operator[](int ch)
        {
            return (_planes==1) ? _data[ch][_sample * _stride     ]
                               : _data[0 ][_sample * _stride + ch];
        }
        constexpr float operator[](int ch) const
        {
            return (_planes==1) ? _data[ch][_sample * _stride    ]
                                : _data[0][_sample * _stride + ch];
        }
    };
    AVFrame     *m_d{nullptr};
    avframe(const defer_frame_t &)
        : m_d{nullptr} {}
    avframe()
        : m_d(av_frame_alloc()) { };
    avframe(AVFrame *f)
        : m_d(f) { }
    avframe(avframe &&o)
        : m_d(o.release()) { }
    avframe(const avframe &o)
        : m_d(av_frame_clone(o)) { }
    avframe &operator = (AVFrame *f)
    {
        reset(f);
        return *this;
    }
    avframe &operator = (const AVFrame *f)
    {
        if(f != m_d)
            reset(av_frame_clone(f));
        return *this;
    }
    avframe &operator =(avframe &&o)
    {
        if(m_d) {
            ref(o);
        }else{
            reset(o.release());
        }
        return *this;
    }
    avframe &operator = (const avframe &o)
    {
        reset(av_frame_clone(o.m_d));
        return *this;
    }
    void swap(avframe &o) noexcept { std::swap(m_d,o.m_d);}
    void reset(AVFrame *f = nullptr) { av_frame_free(&m_d); m_d = f;}
    AVFrame *release() { auto ret = static_cast<AVFrame*>(nullptr);std::swap(m_d,ret);return ret;}
    AVFrame *get() const { return m_d;}
   ~avframe() { reset();}
    AVFrame *operator ->() { return m_d; }
    AVFrame *operator ->() const { return m_d;}
    AVFrame &operator *() { return *m_d;}
    const AVFrame &operator *() const { return *m_d;}
    operator bool() const { return !!m_d;}
    operator AVFrame *() const { return m_d;}
    inline void alloc()
    {
        if(!m_d)
            m_d = av_frame_alloc();
        else
            unref();
    }
    inline void unref()
    {
        av_frame_unref(m_d);
    }
    inline void ref(avframe &o)
    {
        if(o.m_d != m_d && o.m_d) {
            alloc();
            av_frame_ref(m_d,o.m_d);
        }
    }
    inline bool make_writable()
    {
        return !av_frame_make_writable(m_d);
    }
    inline bool writable()
    {
        return av_frame_is_writable(m_d);
    }
    bool get_buffer()
    { 
        if(!m_d)
            return false;
        unref();
        return av_frame_get_buffer(m_d,0);
    }
    inline int64_t get_best_effort_timestamp() const
    {
        return av_frame_get_best_effort_timestamp(m_d);
    }
    inline int64_t pkt_duration() const { return av_frame_get_pkt_duration(m_d);}
    inline int64_t pkt_pos() const { return av_frame_get_pkt_pos(m_d);}
    inline int64_t pkt_pts() const { return m_d ? m_d->pkt_pts : AV_NOPTS_VALUE;}
    inline int64_t pkt_dts() const { return m_d ? m_d->pkt_dts : AV_NOPTS_VALUE;}
    inline int64_t channel_layout() const { return av_frame_get_channel_layout(m_d);}
    inline int     channels() const { return av_frame_get_channels(m_d);}
    inline int     sample_rate() const { return av_frame_get_sample_rate(m_d);}
    inline int     pkt_size() const { return av_frame_get_pkt_size(m_d);}
    inline AVSampleFormat format() const { return m_d ? static_cast<AVSampleFormat>(m_d->format) : AV_SAMPLE_FMT_NONE;}
    inline int     samples() const { return m_d->nb_samples;}
    inline bool    planar() const { return av_sample_fmt_is_planar(format());}
    inline int64_t pts() const { return m_d ? m_d->pts : AV_NOPTS_VALUE;}
    int     copy(AVFrame *o) { return av_frame_copy(m_d,o);}
    int     copy_props(AVFrame *o) { return av_frame_copy_props(m_d,o);}
    inline float **data() const { return reinterpret_cast<float**>(m_d->extended_data);}
    inline float  *data(int i) const
    {
        return planar() ? (data()[i]) : (data()[0] + i);
    }
    inline float  &data(int c,int s)
    {
        return planar() ? data()[0][ c + s * channels()] : data()[c][s];
    }
    inline const float  &data(int c,int s) const
    {
        return planar() ? data()[0][ c + s * channels()] : data()[c][s];
    }
    inline ItemRef operator[](int s) { return ItemRef(*this, s);}
    inline const ItemRef operator[](int s) const { return ItemRef(*this, s);}
    inline const ItemRef at(int s) const { return ItemRef(*this, s);}
    int     copy_to(AVFrame *dst, int dst_offset, int src_offset, int nb_samples)
    {
        if(!dst || dst->format != format())
            return -EINVAL;
        if(dst->nb_samples < dst_offset + nb_samples
        || m_d->nb_samples < src_offset + nb_samples)
            return -EINVAL;
        return av_samples_copy(dst->extended_data, m_d->extended_data, dst_offset, src_offset, nb_samples, channels(), format());
    }
    int     copy_from(AVFrame *src, int dst_offset, int src_offset, int nb_samples)
    {
        if(!src|| src->format != format())
            return -EINVAL;
        if(m_d->nb_samples < dst_offset + nb_samples
        || src->nb_samples < src_offset + nb_samples)
            return -EINVAL;
        return av_samples_copy(m_d->extended_data, src->extended_data, dst_offset, src_offset, nb_samples, channels(), format());
    }
    int     copy_to(float **dst, int dst_offset, int src_offset, int nb_samples)
    {
        if(!dst)
            return -EINVAL;
        if(m_d->nb_samples < src_offset + nb_samples)
            return -EINVAL;
        return av_samples_copy(reinterpret_cast<uint8_t**>(dst), m_d->extended_data, dst_offset, src_offset, nb_samples, channels(), format());
    }
    int     shift_samples(int nb_samples)
    {
        if(!writable())
            make_writable();
        nb_samples = std::min(nb_samples, samples());
        if( av_samples_copy(m_d->extended_data, m_d->extended_data, 0, nb_samples, samples() - nb_samples,  channels(),format()))
            return -1;
        m_d->nb_samples -= nb_samples;
        return nb_samples;
    }
    int     shift_samples(int nb_samples, const AVRational &stream_tb)
    {
        if(!writable())
            make_writable();
        nb_samples = std::min(nb_samples, samples());
        if( av_samples_copy(m_d->extended_data, m_d->extended_data, 0, nb_samples, samples() - nb_samples,  channels(),format()))
            return -1;
        m_d->nb_samples -= nb_samples;
        if(stream_tb.den) {
            auto diff = av_rescale_q(nb_samples, (AVRational){ 1, sample_rate()}, stream_tb);
            m_d->pts += diff;
            m_d->pkt_pts += diff;
            m_d->pkt_duration -= diff;
        }
        return nb_samples;
    }
    inline int bytes_per_sample() const
    {
        return av_get_bytes_per_sample(format());
    }
    inline int planes() const
    {
        return planar() ? 1 : channels();
    }
    inline int word_stride() const
    {
        return planar() ? channels() : 1;
    }
    inline int stride() const
    {
        return word_stride() * bytes_per_sample();
    }
    int skip_samples(int nb_samples)
    {
        nb_samples = std::min(nb_samples,samples());
        auto _planes = planar() ? channels() : 1;
        auto _shift  = stride() * nb_samples;
        for(auto i = 0; i < _planes; i++)
            m_d->extended_data[i] += _shift;
        if(m_d->data != m_d->extended_data) {
            for(auto i = 0; i < FFMIN(_planes, AV_NUM_DATA_POINTERS); i++) {
                m_d->data[i] = m_d->extended_data[i];
            }
        }
        m_d->nb_samples -= nb_samples;
        return nb_samples;
    }
    int skip_samples(int nb_samples, const AVRational &stream_tb)
    {
        nb_samples = std::min(nb_samples,samples());
        auto _planes = planar() ? channels() : 1;
        auto _shift  = stride() * nb_samples;
        for(auto i = 0; i < _planes; i++)
            m_d->extended_data[i] += _shift;
        if(m_d->data != m_d->extended_data) {
            for(auto i = 0; i < FFMIN(_planes, AV_NUM_DATA_POINTERS); i++) {
                m_d->data[i] = m_d->extended_data[i];
            }
        }
        m_d->nb_samples -= nb_samples;
        if(stream_tb.den) {
            auto diff = av_rescale_q(nb_samples, (AVRational){ 1, sample_rate()}, stream_tb);
            m_d->pts += diff;
            m_d->pkt_pts += diff;
            m_d->pkt_duration -= diff;
        }
        return nb_samples;
    }
    inline int64_t duration(const AVRational &tb)
    {
        return av_rescale_q(samples(), (AVRational){ 1, sample_rate()}, tb);
    }
    void realloc(int nb_samples)
    {
        auto new_size = std::max(nb_samples * stride(), 0);
        for(auto i = 0, p = planes(); i < p; i++ ) {
            auto &bufref = (i<AV_NUM_DATA_POINTERS)
                ? m_d->buf[i] 
                : m_d->extended_buf[i];
            if(auto err = av_buffer_realloc(&bufref, new_size))
                throw std::system_error(err,std::system_category(),
                        "Failure reallocating AVBuffer.");
            m_d->extended_data[i] = bufref->data;
        }
        m_d->nb_samples = nb_samples;
    }
    int capacity() const
    {
        auto cap = 0;
        for(auto i = 0, p = planes(); i < p; i++) {
            auto bufref = av_frame_get_plane_buffer(m_d, i);
            if(bufref) {
                cap = std::min<int>(bufref->data + bufref->size - m_d->extended_data[i], cap);
            }
        }
        return cap / stride();
    }
};
};
inline void swap(tinyav::avframe &lhs, tinyav::avframe &rhs){ lhs.swap(rhs);}
