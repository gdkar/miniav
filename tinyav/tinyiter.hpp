
_Pragma("once")

#include "tinycommon.hpp"
#include "tinytraits.hpp"
namespace tinyav {
    namespace iter {
        using namespace tiny_traits;
        
        template<class Iter>
        class stride_iterator : public Iter {
            pointer     _p{nullptr};
            size_type   _n{1};
            constexpr Iter &base() { return static_cast<Iter>(*this);}
        public:
            constexpr stride_iterator() noexcept = default;
            constexpr stride_iterator(Iter o)
            : Iter(o) { }
            constexpr stride_iterator(Iter o, size_type n )
            : Iter(o), _n(n) { }
            constexpr stride_iterator(const stride_iterator &) = default;
            stride_iterator &operator =(const stride_poitner &) = default;
            constexpr stride_iterator operator + (difference_type _d)
            {
                return stride_iterator(base() + (_n * _d), _n);
            }
            constexpr stride_iterator operator - (difference_type _d)
            {
                return stride_iterator(base() - (_n * _d), _n);
            }
            constexpr difference_type operator - ( const stride_iterator &o)
            {
                return std::distance(base(),o.base())/_n;
            }
            using Iter::operator *;
            using Iter::operator ->;
            constexpr stride_iterator& operator += ( difference_type _d)
            {
                std::advance(base(), _d * _n);return *this;
            }
            constexpr stride_iterator& operator -= ( difference_type _d)
            {
                std::advance(base(),-_d * _n);return *this;
            }
            constexpr stride_iterator &operator++()
            {
                std::advance(base(), _n);return *this;
            }
            constexpr stride_iterator &operator--()
            {
                std::advance(base(), -_n);return *this;
            }
            constexpr reference operator[](difference_type _d)
            {
                return base()[_n*_d];
            }
            constexpr const_reference operator[](difference_type _d) const
            {
                return base()[_n*_d];
            }
            constexpr stride_iterator operator++(int)
            {
                auto r = *this;
                ++(*this);
                return r;
            }
            constexpr stride_iterator operator--(int)
            {
                auto r = *this;
                --(*this);
                return r;
            }
            constexpr bool operator ==(const stride_iterator &o) const
            {
                return _n == o._n && base() == o.base();
            }
            constexpr bool operator != (const stride_iterator &o) const
            {
                return !(*this == o);
            }
            constexpr bool operator < (const stride_iterator &o) const
            {
                return (_n == o._n) && (base() < o.base());
            }
            constexpr bool operator > (const stride_iterator &o) const
            {
                return (_n == o._n) && (base() > o.base());
            }
            constexpr bool operator <= (const stride_iterator &o) const
            {
                return (_n == o._n) && (base() <= o.base());
            }
            constexpr bool operator >= (const stride_iterator &o) const
            {
                return (_n == o._n) && (base() <= o.base());
            }
            constexpr bool operator !() const { return (!base() || !_n);}
            constexpr operator bool() const { return !!(*this);}
        };

        template<class Iter>
        class iter_range {
            Iter    _start{};
            Iter    _stop {};
        public:
            using value_type = Iter::value_type;
            using distance_type = Iter::distance_type;
            using size_type     = Iter::size_type;
            using reference     = Iter::reference;
            using const_reference = Iter::const_reference;
            constexpr iter_range() = default;
            constexpr iter_range(const iter_range &) = default;
            constexpr iter_range(iter_range &&) noexcept = default;

            constexpr iter_range(Iter start, Iter stop)
            : _start(start), _stop(stop) { }
            constexpr iter_range(Iter start, size_type _size)
            : _start(start), _stop(std::next(_start,_size)) { }
            iter_range &operator =(const iter_range &) = default;
            iter_range &operator =(iter_range &&) noexcept = default;
            constexpr Iter begin() { return _start; }
            constexpr Iter begin() const { return _start; }
            constexpr Iter cbegin() const { return _start; }
            constexpr Iter end() { return _stop; }
            constexpr Iter end() const { return _stop; }
            constexpr Iter cend() const { return _stop; }
            constexpr size_type size() const { return std::distance(_start,_stop);}
        };
        template<class Iter>
        constexpr iter_range<Iter> make_range(Iter iter, Iter::size_type length)
        {
            return iter_range<Iter>(iter,length);
        }
        template<class Iter>
        constexpr iter_range<Iter> make_range(Iter iter, Iter iend)
        {
            return iter_range<Iter>(iter,iend);
        }
        template<class Iter>
        constexpr stride_iterator<Iter> make_strided(Iter it, Iter::size_type stride)
        {
            return stride_iterator<Iter>(it, stride);
        }
    }
}
