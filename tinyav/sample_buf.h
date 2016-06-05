_Pragma("once")

#include "tinycommon.hpp"
#include "avframe.hpp"

namespace tinyav{
    struct sample_buf {
        std::deque<avframe> m_buf;
        using qiter = std::deque<avframe>::iterator;
        using cqiter= std::deque<avframe>::const_iterator;

        AVRational            m_stream_tb{0,1};
        AVRational            m_output_tb{0,1};
        int64_t               m_off{0};
        sample_buf() = default;
        sample_buf(sample_buf&&) = default;
        sample_buf& operator =(sample_buf&&) = default;
        sample_buf(const AVRational &stream_tb, const AVRational output_tb)
            :m_stream_tb(stream_tb)
            ,m_output_tb(output_tb)
        { }
        sample_buf(const AVRational &stream_tb, int rate)
            :m_stream_tb{stream_tb}
            ,m_output_tb{1, rate}
        { }
        avframe &front() { return m_buf.front();}
        avframe &back()  { return m_buf.back();}
        const avframe &front() const { return m_buf.front();}
        const avframe &back() const { return m_buf.back();}
        int64_t sample_to_pts(int64_t t) const { return av_rescale_q(t, m_output_tb,m_stream_tb); }
        int64_t pts_to_sample(int64_t t) const { return av_rescale_q(t,m_stream_tb, m_output_tb); }
        int64_t pts_begin() const { return front().pts();}
        int64_t pts_end()   const { return back().pts() + sample_to_pts(back().samples());}

        int64_t off_begin() const { return m_off; }
        int64_t off_end()   const { return m_off + back().samples() + pts_to_sample(back().pts()-front().pts());}
        qiter frame_for_pts(int64_t pts)
        {
            if(pts == AV_NOPTS_VALUE)
                return qiter{};
            return std::upper_bound(m_buf.begin(),m_buf.end(),pts,[](const auto &tgt,
                            const auto &frame){return tgt < frame.pts();});
        }
        qiter frame_for_off(int64_t off)
        {
            auto pts = pts_begin() + sample_to_pts(off - off_begin());
            return std::upper_bound(m_buf.begin(),m_buf.end(),pts, [](const auto &tgt,
                            const auto &frame){return tgt < frame.pts();});
        }
        inline bool empty() const { return m_buf.empty();}
        inline size_t size() const { return m_buf.size();}
        void push_back(avframe && ptr) {
            auto _end = empty() ? ptr.pts() : pts_end();
            m_buf.push_back(std::forward<avframe&&>(ptr));
            m_buf.back()->pts = _end;
        }
        void push_back(const avframe & ptr) {
            auto _end = empty() ? ptr.pts() :back().pts() + sample_to_pts(back().samples());;
            m_buf.push_back(ptr);
            back()->pts = _end;
        }
        void emplace_back(AVFrame *frm) {
            auto _end = empty() ? frm->pts :back().pts() + sample_to_pts(back().samples());;
            m_buf.emplace_back(frm);
            m_buf.back()->pts = _end;
        }
        void push_front(avframe && ptr) {
            auto _begin = empty() ? ptr.pts() : pts_begin() - sample_to_pts(ptr.samples());;
            m_buf.push_front(std::forward<avframe&&>(ptr));
            m_buf.front()->pts = _begin;
            m_off -= m_buf.front().samples();
        }
        void push_front(const avframe & ptr) {
            auto _begin = empty() ? ptr.pts() : pts_begin() - sample_to_pts(ptr.samples());;
            m_buf.push_front(ptr);
            m_buf.front()->pts = _begin;
            m_off -= m_buf.front().samples();
        }
        void emplace_front(AVFrame * frm) {
            auto _begin = empty() ? frm->pts : front().pts() - sample_to_pts(frm->nb_samples);
            m_buf.emplace_front(frm);
            front()->pts = _begin;
            m_off -= m_buf.front().samples();
        }
        void pop_front()
        {
            m_off += front().samples();
            m_buf.pop_front();
        }
        void pop_back()
        {
            m_buf.pop_back();
        }
    };
};
inline void swap(tinyav::sample_buf &lhs, tinyav::sample_buf &rhs) { lhs.swap(rhs);}
