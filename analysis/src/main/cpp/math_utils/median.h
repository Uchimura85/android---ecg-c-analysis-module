// A set of stat. utils.
// Reviewed: 2017-04-17

// struct median_avg_t:
//  - median average    median()
//  - 1st and i-th min and max    min1() max1() min_i() max_i()
//  NOTE implemented as matrix container, value feed takes O(N^0.5), calculations O(1)
//
// struct sliding_sum_t:
//  - sum of elements in queue of the specified size (calc. takes O(1))    sum()
//  - average (calc. takes O(1))    avg_k() avg_f()
//  - std. deviation    stddev_f()
//
// struct stat_mode:
//  - mode value and frequency for elements in queue of the specified size    mode_f() ret_mo ret_n
//  NOTE implemented for double precision values only



#ifndef yk_algs_median_H
#define yk_algs_median_H

#include <queue>
#include <deque>
#include <cmath>

#include "../3p_bmdx/src_code/bmdx_config.h"
#include "../3p_bmdx/src_code/yk_cx.h"

namespace algs_median
{
namespace _algs_median
{
  using namespace bmdx;
  using namespace yk_c;
  using namespace yk_c::experimental;
  namespace _impl
  {
    template<class K>
    struct median_avg_t
    {
        // key and cmp are for multihashing of K (different key instances for same K value are not equal to each other).
      struct key
      {
        K x; s_long ind;
        key(median_avg_t& parent, K x_) : x(x_), ind(parent._inew++) {}
      };
      struct cmp
      {
        bool less12(const key& a, const key& b) const { return a.x < b.x || (a.x == b.x && a.ind < b.ind); }
        bool less21(const key& a, const key& b) const { return less12(a, b); }
        void cnew(key* p, const key& x) const { new (p) key(x); }
        s_long hash(const key& x) const { return yk_c::hashx_common::kf_basic<K>().hash(x.x) ^ yk_c::hashx_common::kf_basic<K>().hash(x.ind); }
        bool is_eq(const key& x1, const key& x2) const { return x1.x == x2.x && x1.ind == x2.ind; }
      };

      median_avg_t(s_long nmax_, K x0_ = K()) : _x0(x0_), _nmax(nmax_), _inew(0) { if (_nmax < 1) { _nmax = 1; } }

      void push(K x) { key k(*this, x); _q.push(k); _v[k] = 0; while (_q.size() > size_t(_nmax)) { _v.remove(_q.front()); _q.pop(); } }
      K median() const { s_long n = _v.n(); if (n) { return _v(n / 2)->k.x; } return _x0; }
      K max1() const { s_long n = _v.n(); if (n) { return _v(n - 1)->k.x; } return _x0; }
      K min1() const { s_long n = _v.n(); if (n) { return _v(0)->k.x; } return _x0; }
        // i==1..n, selects i-th value equal or less than maximum (max1()).
      K max_i(s_long i) const { if (i < 1) { return _x0; } s_long n = _v.n();  if (n - i >= 0) { return _v(n - i)->k.x; } return min1(); }
        // i==1..n, selects i-th value equal or greater than minimum (min1()).
      K min_i(s_long i) const { if (i < 1) { return _x0; } s_long n = _v.n(); if (i - 1 < n) { return _v(i - 1)->k.x; } return max1(); }

      void clear() { _v.ordhs_clear(); _q = std::queue<key>(); _inew = 0; }

      s_long n() const { return _v.n(); } // >= 0
      s_long nmax() const { return _nmax; } // >= 1
      K x0() const { return x0; } // returned if no values pushed
      void set_x0(K x0_) { _x0 = x0_; }

      private:
        K _x0;
        s_long _nmax;
        s_long _inew;
        std::queue<key> _q;
        ordhs_t<key, int, cmp, cmp> _v;
    };

    template<class K>
    struct sliding_sum_t
    {
      sliding_sum_t(s_long nmax_, K x0_ = K()) : _x0(x0_), _sum1(0), _sum2(0), _nmax(nmax_), _isplit(0) { if (_nmax < 1) { _nmax = 1; } }

      void push(K x)
      {
        if (s_long(_q.size()) < _nmax) { _q.push_back(x); _sum2 += x; }
          else if (_isplit > 0) { K z = _q.front(); _q.push_back(x); _q.pop_front(); _sum1 -= z; _sum2 += x; _isplit -= 1; if (_isplit == 0) { _sum1 = 0; } }
          else { K z = _q.front(); _q.push_back(x); _q.pop_front(); _sum1 = _sum2; _sum1 -= z; _sum2 = x; _isplit = s_long(_q.size()) - 1; if (_isplit == 0) { _sum1 = 0; } }
      }
      K sum() const { return _sum1 + _sum2; }
      K avg_k() const { s_long N = n(); return N ? (_sum1 + _sum2) / N : _x0; }
      double avg_f() const { s_long N = n(); return N ? double(_sum1 + _sum2) / N : double(_x0); }
      double stddev_f() const
      {
        s_long N = n(); if (N <= 0) { return _x0; } if (N == 1) { return 0; }
        double sd = 0; double a = avg_f();
        for (size_t i = 0; i < _q.size(); ++i) { double x = _q[i] - a; sd += x * x; }
        sd /= N; sd = std::sqrt(sd);
        return sd;
      }

      void clear() { _q.clear(); _sum1 = 0; _sum2 = 0; _isplit = 0; }

      s_long n() const { return s_long(_q.size()); } // >= 0
      s_long nmax() const { return _nmax; } // >= 1
      K x0() const { return x0; } // returned if no values pushed
      void set_x0(K x0_) { _x0 = x0_; }

      private:
        K _x0;
        K _sum1;
        K _sum2;
        s_long _nmax;
        s_long _isplit;
        std::deque<K> _q;
    };

    struct stat_mode
    {
      s_long ret_n;
      double ret_mo;
      double _m0;
      stat_mode(double m0_ = 0): ret_n(0), ret_mo(0), _m0(m0_) {}

        // Find mode frequency in the current dataset, and (on flags & 0x2) the modal average.
        //  Internally, an interval with largest possible number of values is calculated.
        //  flags (OR):
        //    0x1 - calculate and return mode frequency,
        //    0x2 - calculate and return mode value (also calculates mode frequency),
        //    0x4 - use only values in range [xr0..xr2) as input.
      void mode_f(const double* px, s_long nx, double interval, int flags = 0x3, double xr0 = 0, double xr2 = 0)
      {
        ret_n = 0; ret_mo = _m0;
        bool b_freq = !!(flags & 0x3), b_mode = !!(flags & 0x2), b_range = !!(flags & 0x4);
        if (!b_freq || !px || nx <= 0 || interval < 0) { return; }

        ordhs_t<double, long> m;
        for (s_long i = 0; i < nx; ++i) { double x = px[i]; if (!b_range || (x >= xr0 && x < xr2)) { m[x] += 1; } }
        if (m.n() == 0) { return; }

        s_long ibest0 = -1, ibest2 = -1, nbest = 0, ia = 0, ib = 0, n = 0;
        double x0 = m(0)->k;
        while (ib < m.n())
        {
          double x = m(ib)->k;
          while (ia < ib && x - x0 >= interval) { n = n - m(ia)->v; ia = ia + 1; x0 = m(ia)->k; }
          n += m(ib)->v;
          if (n > nbest) { ibest0 = ia; ibest2 = ib + 1; nbest = n; }
          ib += 1;
        }
        x0 = m(ibest0)->k;
        if (interval == 0) { ret_n = nbest; if (b_mode) { ret_mo = x0; } return; }

        ret_n = nbest; if (!b_mode) { return; }

        s_long nnext(0), nprev(0);
        ia = ibest0 - 1; while (ia >= 0) { if (x0 - m(ia)->k > interval) { break; } nprev += m(ia)->v; ia -= 1; }
        ia = ibest2; while (ia < m.n()) { if (m(ia)->k - (x0 + interval) >= interval) { break; } nnext += m(ia)->v; ia += 1; }

        ret_mo = x0 + interval * double(nbest - nprev) / (2 * nbest - nprev - nnext);
      }
    };
  }
}
using namespace _algs_median::_impl;
}

#endif
