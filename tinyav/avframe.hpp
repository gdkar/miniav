_Pragma("once")

#include "tinycommon.hpp"

namespace tinyav{
    class avframe {
        AVFrame     *m_d{nullptr};
    public:
        using value_type      = iter_range<stride_iterator<float *> >;
        using size_type       = uint64_t;
        using difference_type = int64_t;
        using time_type       = int64_t;
        using reference       = iter_range<stride_iterator<float *> >;
        using const_reference = iter_range<stride_iterator<const float *> >;
        using pointer         = AVFrame *;
        using const_pointer   = const AVFrame *;
        avframe()
        : m_d(av_frame_alloc()) { };
        explicit avframe(pointer f)
        : m_d(f) { }
        avframe(avframe &&o) noexcept
            : m_d(o.release()) { }
        avframe(const avframe &o)
        : m_d(av_frame_clone(o)) { }
        avframe &operator =(avframe &&o) noexcept
        {
            swap(*this,o);
            return *this;
        }
        avframe &operator = (const avframe &o)
        {
            if(o.m_d != m_d) {
                av_frame_unref(m_d);
                av_frame_ref  (m_d, o.m_d);
            }
            return *this;
        }
        void swap(avframe &o) noexcept { std::swap(m_d,o.m_d);}
        void reset(pointer f = nullptr)
        {
            if(m_d != f)
                av_frame_free(&m_d);
            m_d = f;
        }
        pointer release()
        {
            return std::exchange(m_d, av_frame_alloc());
        }
        pointer get() const { return m_d;}
       ~avframe() { av_frame_free(&m_d);}
        pointer operator ->() { return m_d; }
        const_pointer operator ->() const { return m_d;}
        AVFrame &operator *() { return *m_d;}
        const AVFrame &operator *() const { return *m_d;}
        operator bool() const { return !!m_d;}
        explicit operator pointer() { return m_d;}
        explicit operator const_pointer() const { return m_d;}
        void unref() { av_frame_unref(m_d); }
        void ref(avframe &o)
        {
            if(o.m_d != m_d) {
                av_frame_unref(m_d);
                av_frame_ref(m_d,o.m_d);
            }
        }
        bool make_writable() { return !av_frame_make_writable(m_d); }
        bool writable() { return av_frame_is_writable(m_d); }
        bool get_buffer() {  return !av_frame_get_buffer(m_d,0); }
        time_type get_best_effort_timestamp() const
        {
            return av_frame_get_best_effort_timestamp(m_d);
        }
        time_type pkt_duration() const { return av_frame_get_pkt_duration(m_d);}
        time_type pkt_pos() const { return av_frame_get_pkt_pos(m_d);}
        time_type pkt_pts() const { return m_d ? m_d->pkt_pts : AV_NOPTS_VALUE;}
        time_type pkt_dts() const { return m_d ? m_d->pkt_dts : AV_NOPTS_VALUE;}
        int64_t channel_layout() const { return av_frame_get_channel_layout(m_d);}
        int     channels() const { return av_frame_get_channels(m_d);}
        int     sample_rate() const { return av_frame_get_sample_rate(m_d);}
        int     pkt_size() const { return av_frame_get_pkt_size(m_d);}
        AVSampleFormat format() const { return  static_cast<AVSampleFormat>(m_d->format);}
        int     samples() const { return m_d->nb_samples;}
        bool    planar() const { return av_sample_fmt_is_planar(format());}
        time_type pts() const { return m_d->pts ;}
        int     copy(AVFrame *o) { return av_frame_copy(m_d,o);}
        int     copy_props(AVFrame *o) { return av_frame_copy_props(m_d,o);}
        float **data() const { return reinterpret_cast<float**>(m_d->extended_data);}
        reference data(int i)
        {
            return make_range(make_strided((planar() ? data()[i] : (data()[0] + i)), word_stride()),
                    channels());
        }
        const_reference data(int i) const
        {
            return make_range(make_strided((planar() ? data()[i] : (data()[0] + i)), word_stride()),
                    channels());
        }
        float  &data(int c,int s)
        {
            return planar() ? data()[0][ c + s * channels()] : data()[c][s];
        }
        const float  &data(int c,int s) const
        {
            return planar() ? data()[0][ c + s * channels()] : data()[c][s];
        }
        reference operator[](std::pair<int64_t,int>) { 
            return make_range(make_strided((planar() ? data()[i] : (data()[0] + i)), word_stride()),channels());
        }
        const_reference operator[](std::pair<int64_t,int>) const { 
            return make_range(make_strided((planar() ? data()[i] : (data()[0] + i)), word_stride()),channels());
        }
        const_reference at(int s) const { return return (*this)[s];}

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
        int bytes_per_sample() const
        {
            return av_get_bytes_per_sample(format());
        }
        int planes() const
        {
            return planar() ? 1 : channels();
        }
        int word_stride() const
        {
            return planar() ? channels() : 1;
        }
        int stride() const
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
        int64_t duration(const AVRational &tb)
        {
            return av_rescale_q(samples(), (AVRational){ 1, sample_rate()}, tb);
        }
        void realloc(int nb_samples)
        {
            make_writable();
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
    inline void swap(tinyav::avframe &lhs, tinyav::avframe &rhs){ lhs.swap(rhs);}
};
