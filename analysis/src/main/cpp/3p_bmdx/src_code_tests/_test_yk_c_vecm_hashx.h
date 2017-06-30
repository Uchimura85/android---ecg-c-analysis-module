// In this module:
//  tests for vecm, hashx, vector_t, iterator_t.

#ifndef _test_yk_c_vecm_hashx_H
#define _test_yk_c_vecm_hashx_H

#ifndef _test_yk_c_common_H
  #include "_test_yk_c_common.h"
#endif

#include <string.h>

namespace yk_c
{
  namespace experimental
  {
    template<class TA> struct vector_t : yk_c::vecm
    {
    public:
      typedef yk_c::s_long s_long; typedef yk_c::vecm vecm;
      typedef TA ta_value; typedef typename specf<ta_value>::t_value t_value; struct exc_vector_t : std::exception { const char* _p; exc_vector_t(const char* pmsg) : _p(pmsg) {} const char* what() const throw() { return _p; } };
      typedef s_long size_type; typedef t_value value_type; typedef t_value& reference; typedef const t_value& const_reference; typedef s_long difference_type;
      typedef __test_common_aux1::iterator_t<t_value, false> iterator; typedef __test_common_aux1::iterator_t<t_value, true> const_iterator;

      inline vector_t() throw() : vecm(yk_c::typer<ta_value>, 0) {} inline ~vector_t() throw() {}
        // NOTE can_shrink is not copied by vector_t(const vector_t&) and operator=.
      inline vector_t(const vector_t& x) : vecm(x) { if (_nxf >> _xsh) { throw exc_vector_t("vector_t(const vector_t&)"); } }
      inline vector_t& operator=(const vector_t& x) { if (vecm_copy(x, false) == 1) { return *this; } if (!(_t_ind == x._t_ind && _ptd->ta_ind == x._ptd->ta_ind)) { this->~vector_t(); new (this) vector_t(); _nxf += _xd; } throw exc_vector_t("vector_t.operator="); }

      inline size_type size() const { return _n; }
      inline reference operator[] (size_type i) { return *pval_0u<t_value>(i); } // op. [ ] ignores nbase()
          inline const_reference operator[] (size_type i) const { return *pval_0u<t_value>(i); } // -"-
      inline reference front() { t_value* p = pval_first<t_value>(); if (p) { return *p; } throw exc_vector_t("vector_t.front"); }
          inline const_reference front() const { t_value* p = pval_first<t_value>(); if (p) { return *p; } throw exc_vector_t("vector_t.front const"); }
      inline reference back() { t_value* p = pval_last<t_value>(); if (p) { return *p; } throw exc_vector_t("vector_t.back"); }
          inline const_reference back() const { t_value* p = pval_last<t_value>(); if (p) { return *p; } throw exc_vector_t("vector_t.back const"); }
      inline void push_back(const value_type& x) { if (!el_append(x)) { throw exc_vector_t("vector_t.push_back"); } }
      inline void pop_back() { if (el_remove_last<t_value>() <= 0) { throw exc_vector_t("vector_t.pop_back"); }  }

      inline iterator begin() throw() { return iterator(*this, nbase()); }
        inline const_iterator begin() const throw() { return const_iterator(*this, nbase()); }
      inline iterator end() throw() { return iterator(*this); }
        inline const_iterator end() const throw() { return const_iterator(*this); }
    };
//    namespace yk_c { template<class TA> struct vecm::spec<vector_t<TA> > { typedef config_t<vector_t<TA>, 1, 4, 1> config; }; }
  }
  template<class TA> struct vecm::spec<experimental::vector_t<TA> > { typedef config_t<experimental::vector_t<TA>, 1, 4, 1> config; };
}

namespace yk_tests
{
  //Usage: test_KIND<>().do_test(args.);
  //  Particular test proc decl. should be: int test_proc().
  //  Ret. value should be:
  //    >0 - on_calm_result,
  //    ==0 - no test performed,
  //    <0 - error.
  //  Test procedures sequence is in do_test().
  //  "log.d, x1, x2..." outputs values into log file and cout.

  using namespace yk_c;
  using namespace experimental;

  // ===================================================
  // Tunable code.
  // ===================================================

  template<class _ = int> struct test_vecm_hashx : test_common_t<>
  {
    typedef test_common_t<> base;
    base::stat_t<test_vecm_hashx> stat;
    static const char* c_logfile_name() { return "testlog_vecm_hashx.txt"; }
    test_vecm_hashx() : base(c_logfile_name(), true), stat(*this) { rnd_00() = 0; rnd_0() = 0; }
    test_vecm_hashx(const char* p_alt_fn = 0, bool log_truncate = true) : base(p_alt_fn ? p_alt_fn : c_logfile_name(), log_truncate), stat(*this) { rnd_00() = 0; rnd_0() = 0; }

    // ===================================================
    // Test-specific functions.
    // ===================================================

    struct fill_vecm_wrapper
    {
      template<int __i, class T>
      struct __fill_vecm_base
      {
        static void __fill(logger& log, s_long& res, std::vector<T>& x, s_long base, s_long n)
        {
            if (res < 0) { res -= 1000; return; }
            long ind = base - 1;
            try { x.clear(); } catch (...) { res = -1; } if (res < 0) { goto lError; }
            try { for (ind = 0; ind < n; ++ind) { x.push_back(_conv_sl<T>::F(base+ind)); } } catch (...) { res = -2; }  if (res < 0) { goto lError; }
            try { if (s_long(x.size()) != n) { res = -3; } } catch (...) { res = -4; } if (res < 0) { goto lError; }
            try { for (ind = 0; ind < n; ++ind) { if (x[ind] != _conv_sl<T>::F(base+ind)) { res = -5; } } } catch (...) { res = -6; }  if (res < 0) { goto lError; }
            return;
          lError:
            log.d, "err. __fill_vecm_base::__fill/std::vector:", typeid(T).name(), base, n, ind;
        }
        static void __fill(logger& log, s_long& res, yk_c::vecm& x, s_long base, s_long n)
        {
            if (res < 0) { res -= 1000; return; }
            long ind = base - 1;
            if (x.vecm_clear() > 0) { res = -21; } if (res < 0) { goto lError; }
            try { for (ind = 0; ind < n; ++ind) { if (!x.el_append<T>(_conv_sl<T>::F(base+ind))) { res = -22; break; } }  if (res < 0) { goto lError; } } catch (...) { res = -31; goto lError; }
            if (x.n() != n) { res = -23; } if (res < 0) { goto lError; }
            try { for (ind = 0; ind < n; ++ind) { if (!x.pval<T>(base+ind)) { res = -25; break; } if (*x.pval<T>(base+ind) != _conv_sl<T>::F(base+ind)) { res = -26; break; } } if (res < 0) { goto lError; } } catch (...) { res = -31; goto lError; }
            return;
          lError:
            log.d, "err. __fill_vecm_base::__fill/vecm:", typeid(T).name(), base, n, ind;
        }
      };
      template<class T>
      struct impl : __fill_vecm_base<1, T>
      {
        // pv* may be 0.
        // Returns
        //    a) 1 if successfully filled structures as necessary,
        //    b) <0 and log record on failure.
        //    c) 0 if nothing to fill.
        static int do_fill(logger& log, std::vector<T>* pv1, yk_c::vecm* pv2, s_long base, s_long n)
        {
          s_long res = 0;
          if (pv1) { impl::__fill(log, res, *pv1, base, n); } if (res < 0) { return res; }
          if (pv2) { impl::__fill(log, res, *pv2, base, n); } if (res < 0) { return res; }
          return 1;
        }
      };
    };

    struct eq_wrapper
    {
      template<class T>
      struct impl
      {
          // 1: v1 == v2;
          // 0: v1 != v2 (size or contents);
          // -1: error during comparison.
          // NOTE v2.nbase() is ignored, both vectors are compared as 0-based.
        static int __is_eq(logger& log, std::vector<T>& v1, yk_c::vecm& v2)
        {
          s_long ind = -1;
          try
          {
            if (s_long(v1.size()) != v2.n()) { log.d, "err. eq_wrapper::__is_eq: v1.size() != v2.n()", v1.size(), v2.n(); return 0; }
            for (ind = 0; ind < v2.n(); ++ind) { if (v1[ind] != *v2.pval_0u<T>(ind)) { log.d, "err. eq_wrapper::__is_eq: v1[ind] != *v2.pval_0u<T>(ind) at", ind, v1[ind], *v2.pval_0u<T>(ind); return 0; } }
            return 1;
          }
          catch (...) { log.d, "err. eq_wrapper::__is_eq: catch(...)", ind; }
          return -1;
        }
      };
    };

    struct el_ins_wrapper
    {
      struct test_plan { s_long nbase; s_long n_append; s_long n_ind0; s_long n_ins; bool is_random; s_long x0; bool verif_end_state_only; }; // ind0 is 0-based; is_random true inserts n_ins at rand. pos in [n_ind0..n()]

      template<class T>
      struct impl
      {
        static void log_insert_title(logger& log, std::vector<T>& v1, yk_c::vecm& v2, test_plan* plan)
        {
          log.d, "el_ins_wrapper::do_insert: T nbase n_append n_ind0 n_ins is_random x0:", typeid(T).name(), plan->nbase, plan->n_append, plan->n_ind0, plan->n_ins, plan->is_random, plan->x0;
          log.flush();
        }
          // v1, v2 are arbitrary, only el. type is T.
          // Returns
          //    a) 1 if removal passed correctly.
          //    b) <0 and log record on any failure.
        static int do_insert(logger& log, std::vector<T>& v1, yk_c::vecm& v2, test_plan* plan, bool log_title)
        {
          if (log_title) { log_insert_title(log, v1, v2, plan); }
          try { v1.clear(); } catch (...) { return -11; }
          if (v2.vecm_clear() > 0) { return -12; }

          v2.vecm_set_nbase(plan->nbase);

          typedef typename eq_wrapper::template impl<T> t_eq;

          for (s_long i = 0; i < plan->n_append; ++i)
          {
            try { v1.push_back(_conv_sl<T>::F(i)); } catch (...) { return -21; }
            try { if (v2.el_append(_conv_sl<T>::F(i)) == 0) { return -22; } } catch (...) { return -23; }
          }
          if (t_eq::__is_eq(log, v1, v2) != 1) { return -24; }

          s_long x = plan->x0;
          for (s_long i = 0; i < plan->n_ins; ++i)
          {
            s_long ind = plan->is_random ? plan->n_ind0 + s_long(rnd() * (v2.n()+1-plan->n_ind0)) : plan->n_ind0;
            s_long res2 = -10;
            try
            {
              T x2 = _conv_sl<T>::F(x);
              res2 = -11;
              v1.insert(v1.begin()+ind, x2);
              res2 = v2.el_insert_1<T>(v2.nbase()+ind, x2);
//              res2 = v2.el_insert_ml<T>(v2.nbase()+ind, 1, x2);
            }
            catch (...) {}
            if (res2 <= 0) { if (!log_title) { log, endl; log_insert_title(log, v1, v2, plan); } log.d, "err. el_ins_wrapper::do_insert v1.size v2.n ind res2 x:", v1.size(), v2.n(), ind, res2, x; return -31; }
            if (!plan->verif_end_state_only || i == plan->n_ins - 1) { if (t_eq::__is_eq(log, v1, v2) != 1) { return -32; } }
            ++x;
          }
          return 1;
        }

        static int do_test_plans(logger& log, test_plan* plans, s_long n_plans)
        {
          s_long res = 0;
          std::vector<T> v1; vecm v2(typer<T>, 0);
          log.d, "el_ins_wrapper::do_test_plans:",  typeid(T).name(); log.flush();
          log, "  ", n_plans, ":";
          for (int j = 0; j < n_plans; ++ j)
          {
            log, " ", j;
            res = do_insert(log, v1, v2, plans+j, false); if (res < 0) { log.flush(); return res; }
          }
          log, endl; log.flush();
          return 1;
        }
      };
    };

    struct el_rmv_wrapper
    {
      struct test_plan { s_long n0; s_long n1; bool is_random; s_long at_ind; s_long n_rmv; }; // at_ind is 0-based, n_rmv <= 0 == del. max. poss. number of els.

      template<class T>
      struct impl
      {
        static void log_remove_title(logger& log, std::vector<T>& v1, yk_c::vecm& v2, s_long n_rmv, bool is_random, s_long at_ind)
        {
          log.d, "el_rmv_wrapper::do_remove:", typeid(T).name(), "base", v2.nbase(), "n rmv.", n_rmv, "at", (is_random ? std::string("rand. ind.") : str(at_ind)), "vec. sizes:", v1.size(), v2.n();
          log.flush();
        }
          // v1, v2 must contain identical data.
          //  n_rmv must be < vec. size.
          //  at_ind must be specified only if is_random == false. at_ind is 0-based. n_rmv must be such that at_ind will always address a valid element.
          // Returns
          //    a) 1 if removal passed correctly.
          //    b) <0 and log record on any failure.
        static int do_remove(logger& log, std::vector<T>& v1, yk_c::vecm& v2, s_long n_rmv, bool is_random, s_long at_ind, bool log_title)
        {
          if (log_title) { impl::log_remove_title(log, v1, v2, n_rmv, is_random, at_ind); }
          if ((is_random && n_rmv > v2.n()) || (!is_random && at_ind + n_rmv > v2.n())) { return -1; }
          typedef typename eq_wrapper::template impl<T> t_eq;
          if (t_eq::__is_eq(log, v1, v2) != 1) { return -2; }
          for (s_long i = 0; i < n_rmv; ++i)
          {
            s_long ind = is_random ? s_long(rnd() * v2.n()) : at_ind;
            s_long res2 = -10;
            try
            {
              v1.erase(v1.begin()+ind);
              res2 = v2.el_remove_1<T>(v2.nbase()+ind, false);
//              res2 = v2.el_remove_ml<T>(v2.nbase()+ind, 1);
            }
            catch (...) {}
            if (res2 <= 0) { if (!log_title) { log, endl; log_remove_title(log, v1, v2, n_rmv, is_random, at_ind); } log.d, "err. el_rmv_wrapper::do_remove/ind,res2:", ind, res2; return -3; }
            if (t_eq::__is_eq(log, v1, v2) != 1) { return -4; }
          }
          return 1;
        }

        static int do_test_plans(logger& log, s_long* bases, test_plan* plans, s_long n_bases, s_long n_plans)
        {
          s_long res = 0;
          std::vector<T> v1; vecm v2(typer<T>, 0);
          typedef typename fill_vecm_wrapper::template impl<T> t_fill;
          log.d, "el_rmv_wrapper::do_test_plans:",  typeid(T).name(); log.flush();
          log, "  ", n_bases * n_plans, ":";
          for (int ib = 0; ib < n_bases; ++ ib)
          {
            for (int j = 0; j < n_plans; ++j)
            {
              log, " ", ib * n_plans + j;
              v2.vecm_set_nbase(bases[ib]);
              s_long nx = plans[j].n0 + s_long(plans[j].n1 * rnd());
              s_long nr = plans[j].n_rmv; if (nr <= 0) { nr = plans[j].is_random ? nx : nx - plans[j].at_ind; }
              res = t_fill::do_fill(log, &v1, &v2, bases[ib], nx); if (res < 0) { log, endl; log.d, "err. t_fill::do_fill base n", bases[ib], nx ; log.flush(); return res; }
              res = do_remove(log, v1, v2, nr, plans[j].is_random, plans[j].at_ind, false); if (res < 0) { log.flush(); return res; }
            }
          }
          log, endl; log.flush();
          return 1;
        }
      };
    };

    struct perf_vector_wrapper
    {
      template<class T>
      struct impl
      {
          // q_acc == 1: normal number of tests.
          // q_acc > 1: increased number of tests to measure time correctly.
          // do_vector: perform test for std::vector.
          // do_vecm: perform test for vecm el_append(T&).
          // do_vecm_f: perform test for vecm el_append(functor).
          // n_vecm_m (1..3):  test vecm el_append_m by 1..3 elements at once. 0 - do not perform the test.
        static int Fperf_append_1(logger& log, s_long n, bool do_vector, bool do_vecm, bool do_vecm_f, int n_vecm_m, s_long q_acc)
        {
          if (q_acc <= 0 || n < 3 || !(do_vector || do_vecm || do_vecm_f || (n_vecm_m > 0 && n_vecm_m <= 3))) { return 0; }
          try
          {
            typedef _conv_sl<T> t_conv;

            double t0(0.); double dt1(0.); double dt2(0.); double dt3(0.); double dt4(0.); double dt5(0.); double dt6(0.);

            if (do_vector)
            {
              typedef std::vector<T> V;
              V v1;
              for (s_long q = 0; q < q_acc; ++q)
              {
                t0 = bmdx::clock_ms();
                  for (s_long i = 0; i < n; ++i) { v1.push_back(t_conv::F(i)); }
                dt1 += (bmdx::clock_ms() - t0);
                s_long ind = s_long(v1.size() * rnd()); if (v1[ind] != t_conv::F(ind)) { log.d, "err. Fperf_append_1 [1]: T, ind, v1[ind]:", typeid(T).name(), ind, v1[ind]; return -1; }
                v1.clear();
              }
              dt1 /= q_acc;
            }

            if (do_vecm)
            {
              vecm v2(typer<T>, 0);
              for (s_long q = 0; q < q_acc; ++q)
              {
                t0 = bmdx::clock_ms();
                  for (s_long i = 0; i < n; ++i) { v2.el_append<T>(t_conv::F(i)); }
                dt2 += (bmdx::clock_ms() - t0);
                s_long ind = s_long(v2.n() * rnd()); if (*v2.pval_0u<T>(ind) != t_conv::F(ind)) { log.d, "err. Fperf_append_1 [2]: T, ind, pval_0u(ind):", typeid(T).name(), ind, *v2.pval_0u<T>(ind); return -2; }
                v2.el_remove_all();
              }
              dt2 /= q_acc;
            }

            if (do_vecm_f)
            {
              vecm v2(typer<T>, 0);
              ctf_sl_t<T, t_conv> f;
              for (s_long q = 0; q < q_acc; ++q)
              {
                t0 = bmdx::clock_ms();
                  for (f.i = 0; f.i < n; ++f.i) { v2.el_append(f); }
                dt3 += (bmdx::clock_ms() - t0);
                s_long ind = s_long(v2.n() * rnd()); if (*v2.pval_0u<T>(ind) != t_conv::F(ind)) { log.d, "err. Fperf_append_1 [3]: T, ind, pval_0u(ind):", typeid(T).name(), ind, *v2.pval_0u<T>(ind); return -3; }
                v2.el_remove_all();
              }
              dt3 /= q_acc;
            }

            s_long n4 = n;
            s_long n5 = (n / 2) * 2;
            s_long n6 = (n / 3) * 3;

            if (n_vecm_m == 1)
            {
              s_long k = n_vecm_m; s_long n = n4 / k;
              vecm v2(typer<T>, 0);
              for (s_long q = 0; q < q_acc; ++q)
              {
                t0 = bmdx::clock_ms();
                  for (s_long i = 0; i < n; ++i) { v2.el_append_m<T>(1, t_conv::F(i)); }
                dt4 += (bmdx::clock_ms() - t0);
                s_long ind = s_long(n * rnd()); if (*v2.pval_0u<T>(k * ind) != t_conv::F(ind)) { log.d, "err. Fperf_append_1 [4]: T, ind, pval_0u(k * ind):", typeid(T).name(), k * ind, *v2.pval_0u<T>(k * ind); return -4; }
                v2.el_remove_all();
              }
              dt4 /= q_acc;
            }

            if (n_vecm_m == 2)
            {
              s_long k = n_vecm_m; s_long n = n5 / k;
              vecm v2(typer<T>, 0);
              for (s_long q = 0; q < q_acc; ++q)
              {
                t0 = bmdx::clock_ms();
                  for (s_long i = 0; i < n; ++i) { v2.el_append_m<T>(2, t_conv::F(i)); }
                dt5 += (bmdx::clock_ms() - t0);
                s_long ind = s_long(n * rnd()); if (*v2.pval_0u<T>(k * ind) != t_conv::F(ind)) { log.d, "err. Fperf_append_1 [5]: T, ind, pval_0u(k * ind):", typeid(T).name(), k * ind, *v2.pval_0u<T>(k * ind); return -5; }
                v2.el_remove_all();
              }
              dt5 /= q_acc;
            }

            if (n_vecm_m == 3)
            {
              s_long k = n_vecm_m; s_long n = n6 / k;
              vecm v2(typer<T>, 0);
              for (s_long q = 0; q < q_acc; ++q)
              {
                t0 = bmdx::clock_ms();
                  for (s_long i = 0; i < n; ++i) { v2.el_append_m<T>(3, t_conv::F(i)); }
                dt6 += (bmdx::clock_ms() - t0);
                s_long ind = s_long(n * rnd()); if (*v2.pval_0u<T>(k * ind) != t_conv::F(ind)) { log.d, "err. Fperf_append_1 [6]: T, ind, pval_0u(k * ind):", typeid(T).name(), k * ind, *v2.pval_0u<T>(k * ind); return -6; }
                v2.el_remove_all();
              }
              dt6 /= q_acc;
            }

            if (do_vector || do_vecm || do_vecm_f || n_vecm_m)
            {
              if (do_vector) log.d, "Perf. std::vector::push_back(), T:", typeid(T).name(), "N:", n, ":", "t:", dt1 * 1.e6 / n, "ns/1" ;
              if (do_vecm) log.d, "Perf. vecm::el_append(), T:", typeid(T).name(), "N:", n, ":", "t:", dt2 * 1.e6 / n, "ns/1" ;
              if (do_vecm_f) log.d, "Perf. vecm::el_append(functor), T:", typeid(T).name(), "N:", n, ":", "t:", dt3 * 1.e6 / n, "ns/1" ;
              if (n_vecm_m == 1) log.d, "Perf. vecm::el_append_m() m=1, T:", typeid(T).name(), "N:", n4, ":", "t:", dt4 * 1.e6 / n4, "ns/1" ;
              if (n_vecm_m == 2) log.d, "Perf. vecm::el_append_m() m=2, T:", typeid(T).name(), "N:", n5, ":", "t:", dt5 * 1.e6 / n5, "ns/1" ;
              if (n_vecm_m == 3) log.d, "Perf. vecm::el_append_m() m=3, T:", typeid(T).name(), "N:", n6, ":", "t:", dt6 * 1.e6 / n6, "ns/1" ;
              log.flush();
            }
          }
          catch (...) { return -1; }

          return 1;
        }

          // q_acc == 1: normal number of tests.
          // q_acc > 1: increased number of tests to measure time correctly.
        static int Fperf_elem_copy(logger& log, s_long n, s_long q_acc)
        {
          try
          {
            bool can_memcopy = vecm::specf<T>::cmode == 4 && vecm::specf<T>::dmode == 0;
            typedef _conv_sl<T> t_conv;

            double t1b(0.), dt1(0.);
            double t2b(0.), dt2(0.);
            double t3b(0.), dt3(0.);
            double t4b(0.), dt4(0.);
            double t5b(0.), dt5(0.);
            double t6b(0.), dt6(0.);
            double t7b(0.), dt7(0.);
            double t8b(0.), dt8(0.);

            log.d, "Perf. copying on vecm. access, T:", typeid(T).name(), "N:", n, ":";
            log.flush();

            if (true)
            {
              typedef std::vector<T> V;
              V v(n, t_conv::F(1)); V v2(n, t_conv::F(0));
              t1b = bmdx::clock_ms();
                for (s_long q = 0; q < q_acc; ++q)
                {
                  T* p = &v[0]; T* p2 = &v2[0]; T* p3 = p2+n;
                  while (p2 < p3) { *p2++ = *p++; }
                  __consume(v2[(unsigned int)(n * rnd())]);
                }
              dt1 = bmdx::clock_ms(); dt1 -= t1b; dt1 /= q_acc;
            }

            if (true)
            {
              typedef std::vector<T> V;
              V v(n, t_conv::F(1)); V v2(n, t_conv::F(0));
              t6b = bmdx::clock_ms();
                for (s_long q = 0; q < q_acc; ++q)
                {
                  for (unsigned int i = 0; i < v.size(); ++i) { v2[i] = v[i]; }
                  __consume(v2[(unsigned int)(n * rnd())]);
                }
              dt6 = bmdx::clock_ms(); dt6 -= t6b; dt6 /= q_acc;
            }

            if (can_memcopy)
            {
              if (true)
              {
                typedef std::vector<T> V;
                V v(n, t_conv::F(1)); V v2(n, t_conv::F(0));
                t2b = bmdx::clock_ms();
                  for (s_long q = 0; q < q_acc; ++q)
                  {
                    memcpy(&v2[0], &v[0], n * sizeof(T));
                    __consume(v2[(unsigned int)(n * rnd())]);
                  }
                dt2 = bmdx::clock_ms(); dt2 -= t2b; dt2 /= q_acc;
              }
            }

            if (true)
            {
              typedef std::vector<T> V;
              V v(n, t_conv::F(1)); V v2(n, t_conv::F(0));
              typedef typename V::iterator L;
              t3b = bmdx::clock_ms();
                for (s_long q = 0; q < q_acc; ++q)
                {
                  L p = v.begin(); L p2 = v2.begin(); L p3 = v2.end();
                  while (p2 != p3) { *p2++ = *p++; }
                  __consume(v2[(unsigned int)(n * rnd())]);
                }
              dt3 = bmdx::clock_ms(); dt3 -= t3b; dt3 /= q_acc;
            }

            if (true)
            {
              vecm v(typer<T>, 0); vecm v2(typer<T>, 0);
              v.el_append_m<T>(n, t_conv::F(1)); v2.el_append_m<T>(n, t_conv::F(0));
              t7b = bmdx::clock_ms();
                for (s_long q = 0; q < q_acc; ++q)
                {
                  for (s_long i = 0; i < v.n(); ++i) { *v2.pval_0u<T>(i) = *v.pval_0u<T>(i); }
                  __consume(*v2.pval_0u<T>(s_long(n * rnd())));
                }
              dt7 = bmdx::clock_ms(); dt7 -= t7b; dt7 /= q_acc;
            }

            if (true)
            {
              vecm v(typer<T>, 0); vecm v2(typer<T>, 0);
              v.el_append_m<T>(n, t_conv::F(1)); v2.el_append_m<T>(n, t_conv::F(0));
              t8b = bmdx::clock_ms();
                for (s_long q = 0; q < q_acc; ++q)
                {
                  for (s_long i = 0; i < v.n(); ++i) { *v2.pval<T>(i) = *v.pval<T>(i); }
                  __consume(*v2.pval<T>(s_long(n * rnd())));
                }
              dt8 = bmdx::clock_ms(); dt8 -= t8b; dt8 /= q_acc;
            }

            if (true)
            {
              if (true)
              {
                vecm v(typer<T>, 0); vecm v2(typer<T>, 0);
                v.el_append_m<T>(n, t_conv::F(1)); v2.el_append_m<T>(n, t_conv::F(0));
                typedef vecm::link1_t<T, false> L;
                t4b = bmdx::clock_ms();
                  for (s_long q = 0; q < q_acc; ++q)
                  {
                    L p = v.link1_begin<T>(); L p2 = v2.link1_begin<T>();
                    while (p.pval()) { *p2.pval() = *p.pval(); p2.incr(); p.incr(); }
                    __consume(*v2.pval_0u<T>(s_long(n * rnd())));
                  }
                dt4 = bmdx::clock_ms(); dt4 -= t4b; dt4 /= q_acc;
              }

              if (true)
              {
                vecm v(typer<T>, 0); vecm v2(typer<T>, 0);
                v.el_append_m<T>(n, t_conv::F(1)); v2.el_append_m<T>(n, t_conv::F(0));
                  // The following line slightly shifts kbase in all columns.
                v.el_remove_1<T>(0, false); v.el_remove_1<T>(0, false); v.el_remove_1<T>(0, false); v2.el_remove_1<T>(0, false); v2.el_remove_1<T>(0, false); v2.el_remove_1<T>(0, false);
                v.el_append_m<T>(3, t_conv::F(1)); v2.el_append_m<T>(3, t_conv::F(0));
                typedef vecm::link1_t<T, false> L;
                t5b = bmdx::clock_ms();
                  for (s_long q = 0; q < q_acc; ++q)
                  {
                    L p = v.link1_begin<T>(); L p2 = v2.link1_begin<T>();
                    while (p.pval()) { *p2.pval() = *p.pval(); p2.incr(); p.incr(); }
                    __consume(*v2.pval_0u<T>(s_long(n * rnd())));
                  }
                dt5 = bmdx::clock_ms(); dt5 -= t5b; dt5 /= q_acc;
              }

            }

            log.d, "  - by pointer:", dt1 * 1.e6 / n, "ns/1" ;
            log.d, "  - by std::vector::operator[](i):", dt6 * 1.e6 / n, "ns/1" ;
            if (can_memcopy) log.d, "  - memcpy:", dt2 * 1.e6 / n, "ns/1";
            log.d, "  - by std::vector::iterator:", dt3 * 1.e6 / n, "ns/1" ;
            log.d, "  - by vecm::pval_0u(i):", dt7 * 1.e6 / n, "ns/1" ;
            log.d, "  - by vecm::pval(i):", dt8 * 1.e6 / n, "ns/1" ;
            log.d, "  - by vecm::link1_t:", dt4 * 1.e6 / n, "...", dt5 * 1.e6 / n, "ns/1" ;
            log.flush();
          }
          catch (...) { return -1; }

          return 1;
        }

          // q_acc == 1: normal number of tests.
          // q_acc > 1: increased number of tests to measure time correctly.
        static int Fperf_rand_copy(logger& log, s_long n, bool do_vector, bool do_vecm, s_long q_acc)
        {
          if (q_acc <= 0 || !(do_vector || do_vecm)) { return 0; }
          try
          {
            typedef _conv_sl<T> t_conv;

            double t0(0.), dt6(0.), dt7(0.);

            s_long* pinds = (s_long*)bytes::allocdef_t<>::Falloc(n * sizeof(s_long));
            if (pinds == 0) { return -1; }

            if (do_vector)
            {
              typedef std::vector<T> V;
              V v(n, t_conv::F(1)); V v2(n, t_conv::F(0));
                for (s_long q = 0; q < q_acc; ++q)
                {
                  for (s_long i = 0; i < n; ++i) { pinds[i] = s_long(n * rnd()); }
                  t0 = bmdx::clock_ms();
                    for (unsigned int i = 0; i < v.size(); ++i) { v2[pinds[i]] = v[pinds[n-i-1]]; }
                    __consume(v2[(unsigned int)(n * rnd())]);
                  dt6 += (bmdx::clock_ms()-t0);
                }
                dt6 /= q_acc;
            }

            if (do_vecm)
            {
              vecm v(typer<T>, 0); vecm v2(typer<T>, 0);
              v.el_append_m<T>(n, t_conv::F(1)); v2.el_append_m<T>(n, t_conv::F(0));
                for (s_long q = 0; q < q_acc; ++q)
                {
                  for (s_long i = 0; i < n; ++i) { pinds[i] = s_long(n * rnd()); }
                  t0 = bmdx::clock_ms();
                    for (s_long i = 0; i < v.n(); ++i) { *v2.pval_0u<T>(pinds[i]) = *v.pval_0u<T>(pinds[n-i-1]); }
                    __consume(*v2.pval_0u<T>(s_long(n * rnd())));
                  dt7 += (bmdx::clock_ms()-t0);
                }
                dt7 /= q_acc;
            }

            bytes::allocdef_t<>::Ffree(pinds);

            if (do_vector || do_vecm)
            {
              if (do_vector) log.d, "Perf. copying on random access by std::vector::operator[](i), T:", typeid(T).name(), "N:", n, "t:", dt6 * 1.e6 / n, "ns/1" ;
              if (do_vecm) log.d, "Perf. copying on random access by vecm::pval_0u(i), T:", typeid(T).name(), "N:", n, "t:", dt7 * 1.e6 / n, "ns/1" ;
              log.flush();
            }
          }
          catch (...) { return -1; }

          return 1;
        }

          // q_acc == 1: normal number of tests.
          // q_acc > 1: increased number of tests to measure time correctly.
        static int Fperf_rnd_insdel_1(logger& log, s_long n, s_long q_acc)
        {
          try
          {
            typedef typename eq_wrapper::template impl<T> t_eq;
            vecm inds(typer<s_long>, 0);
            vecm inds2(typer<s_long>, 0);
            vecm vals(typer<T>, 0);
              for (int i = 0; i < n; ++ i)
              {
                s_long i1 = s_long(rnd() * (i + 1));
                s_long i2 = s_long(rnd() * (n-i));
                inds.el_append<s_long>(i1);
                inds2.el_append<s_long>(i2);
                vals.el_append<T>(_conv_sl<T>::F(i));
              }

            s_long n_obj1 = 10000/n; if (n < 30) { n_obj1 *= 2; } if (n < 300) { n_obj1 *= 2; } if (n < 3000) { n_obj1 *= 4; } if (n_obj1 == 0) { n_obj1 = 1; } n_obj1 *= q_acc;
            s_long n_obj2 = 2 * n_obj1;

            typedef std::vector<T> V1;
            typedef vecm V2;
            std::vector<char> vv1_(n_obj1 * sizeof(std::vector<T>));
            std::vector<char> vv2_(n_obj2 * sizeof(vecm));
            std::vector<char> vv21_(n_obj2 * sizeof(vecm));
            V1* vv1 = (V1*)&vv1_[0];
            V2* vv2 = (V2*)&vv2_[0];
            V2* vv21 = (V2*)&vv21_[0];

            for (s_long j = 0; j < n_obj1; ++j) { new (vv1+j) V1(); }
            for (s_long j = 0; j < n_obj2; ++j) { new (vv2+j) V2(typer<T>, 0); }
            for (s_long j = 0; j < n_obj2; ++j) { new (vv21+j) V2(typer<T>, 0); (vv21+j)->vecm_setf_can_shrink(false); }

            double t0, dt1(0.), dt2(0.), dt3(0.), dt4(0.), dt6(0.);
            t0 = bmdx::clock_ms();
              for (s_long j = 0; j < n_obj1; ++j) for (int i = 0; i < n; ++i) { vv1[j].insert(vv1[j].begin()+*inds.pval<s_long>(i), *vals.pval<T>(i)); }
            dt1 = bmdx::clock_ms() - t0;
            t0 = bmdx::clock_ms();
              for (s_long j = 0; j < n_obj2; ++j) for (int i = 0; i < n; ++i) { vv2[j].el_insert_1(*inds.pval<s_long>(i), *vals.pval<T>(i)); }
//              for (s_long j = 0; j < n_obj2; ++j) for (int i = 0; i < n; ++i) { vv2[j].el_insert_ml(*inds.pval<s_long>(i), 1, *vals.pval<T>(i)); }
            dt2 = bmdx::clock_ms() - t0;
            for (s_long j = 0; j < n_obj2; ++j) for (int i = 0; i < n; ++i) { vv21[j].el_insert_1(*inds.pval<s_long>(i), *vals.pval<T>(i)); }
//            for (s_long j = 0; j < n_obj2; ++j) for (int i = 0; i < n; ++i) { vv21[j].el_insert_ml(*inds.pval<s_long>(i), 1, *vals.pval<T>(i)); }

              if (t_eq::__is_eq(log, vv1[0], vv2[0]) != 1) { return -2; }
              if (t_eq::__is_eq(log, vv1[0], vv21[0]) != 1) { return -2; }

            t0 = bmdx::clock_ms();
              for (s_long j = 0; j < n_obj1; ++j) for (int i = 0; i < n; ++i) { vv1[j].erase(vv1[j].begin()+*inds2.pval<s_long>(i)); }
            dt3 = bmdx::clock_ms() - t0;
            t0 = bmdx::clock_ms();
              for (s_long j = 0; j < n_obj2; ++j) for (int i = 0; i < n; ++i) { vv2[j].el_remove_1<T>(*inds2.pval<s_long>(i), false); }
//              for (s_long j = 0; j < n_obj2; ++j) for (int i = 0; i < n; ++i) { vv2[j].el_remove_ml<T>(*inds2.pval<s_long>(i), 1); }
            dt4 = bmdx::clock_ms() - t0;

            t0 = bmdx::clock_ms();
              for (s_long j = 0; j < n_obj2; ++j) for (int i = 0; i < n; ++i) { vv21[j].el_remove_1<T>(*inds2.pval<s_long>(i), false); }
//              for (s_long j = 0; j < n_obj2; ++j) for (int i = 0; i < n; ++i) { vv21[j].el_remove_ml<T>(*inds2.pval<s_long>(i), 1); }
            dt6 = bmdx::clock_ms() - t0;

            double k1 = 1.e6 / n / n_obj1; double k2 = 1.e6 / n / n_obj2;
            dt1 *= k1; dt2 *= k2; dt3 *= k1; dt4 *= k2; dt6 *= k2;

            for (s_long j = 0; j < n_obj1; ++j) { vv1[j].~V1(); }
            for (s_long j = 0; j < n_obj2; ++j) { vv2[j].~V2(); }
            for (s_long j = 0; j < n_obj2; ++j) { vv21[j].~V2(); }

            log.d
              , "Perf. (avg. ns/1): T n, vector ins./del., vecm ins./del.+unrsv./del. only: "
              , typeid(T).name(), n
              , "--", dt1, '/', dt3
              , "--", dt2, '/', dt4, '/', dt6
              ;
            log.flush();
          }
          catch (...) { return -1; }

          return 1;
        }
      };
    };

    struct __hashx_x : hashx<char, char>
    {
      enum { delta_ht = t_hashx::delta_ht, M_ins = t_hashx::M_ins, M_rem = t_hashx::M_rem, k_grow = t_hashx::k_grow, k_max = t_hashx::k_max };
        // Normal place reserve value in hash containing n elements.
      static inline s_long _nrsv_n(s_long n)
      {
        if (n < 0) { return 0; }
        s_long n1 = __vecm_x::_nrsv_n(n);
        s_long nbase =  _base_min;
        s_long n2 = 0;
        while (true)
        {
          if (nbase >= meta::s_long_mp / k_max || nbase >= meta::s_long_mp / k_grow) { n2 = meta::s_long_mp; break; }
          n2 = k_max * nbase - 1;
          if (n <= n2) { break; }
          nbase *= k_grow;
        }
        return bytes::min_sl(n1, n2);
      }      
    };
    template<class H> struct __hashx_x2 : H
    {
      s_long nrsv_elems() const { return this->vecm::nrsv(); }
      s_long nrsv_ht() const { return this->_ht.nrsv(); }
    };

      // Main vecm validity test.
      //    integrity, compatibility, locality, ptd, null_state.
      //    el*, link*, vecm_clear, null_state.
      //  nmax > 0 is exact max. number of elements for pv growing.
      //    nmax > 0 but < pv->n() is adjusted to pv->n().
      //    nmax == 0 performs only a small part of tests (section 1).
      //    nmax < 0 is not valid.
      //  ntests must be >= 1.
      //  TA2 specifies elem. type if TA is vecm. Otherwise must be meta::nothing.
      //  NOTE TA may be hashx only if both its key and value are not vecm.
      //  NOTE pv is modified during the test.
      //  NOTE pv == 0 is valid in almost all compilers. Some of fns. will be tested.
      //  NOTE vecm(any excgen) is treated in special in comp. to types.
      //    (Exception generation is taken into account and additionally tested.)
    template<class TA, class TA2> struct test_vecm_t
    {
    static int F(logger& log, vecm* pv, s_long nmax, s_long ntests)
    {
      if (pv) { log.d, "test_vecm_t::F: n", nmax, "nt", ntests, typeid(TA).name(), (meta::same_t<TA2, meta::nothing>::result ? "" : typeid(TA2).name()); log.flush(); }
        else { log.d, "test_vecm_t::F: pv == 0"; log.flush(); }

      typedef typename vecm::specf<TA>::t_value T;
      typedef typename vecm::specf<TA2>::t_value T2;

      // Section 1. pv validity / preparation.
      if (!pv)
      {
        #if !defined(__MINGW64__)
          if (pv->integrity() != -3) { return -1; }
          if (pv->locality() != -1) { return -2; }
          if (pv->compatibility() != -3) { return -3; }
          if (pv->ptd()) { return -4; }
          if (pv->null_state(0) != -1) { return -5; }
        #endif
        return 1;
      }
      if (nmax < 0) { return -6; }
      if (ntests <= 0) { return -7; }
      vecm v2(typer<meta::nothing>, 0);
      do
      {
        s_long npass = pv == &v2 ? 2 : 1;
        if (pv->ptd()->t_ind == bytes::type_index_t<meta::nothing>::ind()) { return -10 * npass - 1; }
        if (pv->integrity() < 0) { return -10 * npass - 2; }
        if (pv->locality() < 0) { return -10 * npass - 3; }
        if (pv->compatibility() < 0) { return -10 * npass - 4; }
        if (pv->compatibility() == 0)
        {
          if (pv == &v2) { return -10 * npass - 5; }
          s_long res = v2.vecm_copy(*pv, false); if (res != 1) { return -10 * npass - 6; }
          pv = &v2; continue;
        }
        if (pv == &v2 && pv->compatibility() != 2) { return -10 * npass - 7; }
      } while (false);

      if (nmax == 0) { return 1; }
      if (nmax < pv->n()) { nmax = pv->n() + 1; }
      T* px = 0;
      s_long n = 0; s_long nr = 0; s_long m = 0;
      const s_long n1 = pv->n(); if (n1 < 0) { return -30; }
      if (nmax > 0 && nmax <= n1) { nmax = n1 + 1; if (nmax < 0) { return -31; } }

      typedef _conv_sl<T, T2> t_conv;
      ctf_sl_t<T, t_conv> f;

      // Section 2. Appending and elem. access w/o error generation.
      m = rnd() * (nmax - n1); if (m == 0) { m = 1; }
      nr = pv->nrsv();
      for (f.i = n1; f.i < n1 + m; ++f.i)
      {
        if (rnd() < 0.5) { px = pv->el_append(t_conv::F(f.i)); if (!px) { return _retval(f.i, - 32); } }
          else { px = pv->el_append(f); if (!px) { return _retval(f.i, - 33); } }
        if (_eq<T, T2>::F(*px, t_conv::F(f.i)) != 1) { return _retval(f.i, -34); }
        if (pv->n() != f.i + 1) { return _retval(f.i, -35); }
        if (pv->n() > nr)
        {
          n = pv->nrsv() - pv->n();
          if (!__vecm_x::nr_ok(pv)) { return _retval(f.i, -36); }
        }
        else { if (pv->nrsv() != nr) { return _retval(f.i, -37); } }
      }

      vecm::link1_t<T, false> l1, l2;
      vecm::link1_t<T, true> l3, l4;
      if (l1.is_valid() || l2.is_valid() || l3.is_valid() || l4.is_valid()) { return -38; }
      l1 = pv->link1_begin<T>(); l2 = pv->link1_aend<T>(); l3 = pv->link1_cbegin<T>(); l4 = pv->link1_caend<T>();
        if (!(l1.is_valid() && l2.is_valid() && l3.is_valid() && l4.is_valid())) { return -39; }
        if (l1.ind() != pv->nbase() || l1.is_out()) { return -39; }
        if (l2.ind() != pv->nbase() + pv->n() || !l2.is_out() || !l2.is_aend()) { return -40; }
        if (l3.ind() != pv->nbase() || l3.is_out()) { return -41; }
        if (l4.ind() != pv->nbase() + pv->n() || !l4.is_out() || !l4.is_aend()) { return -42; }

      for (f.i = 0; f.i < n1 + m; ++f.i)
      {
        px = pv->pval<T>(f.i + pv->nbase());
        if (!px) { return _retval(f.i, -50); }
        if (f.i >= n1) { if (_eq<T, T2>::F(*px, t_conv::F(f.i)) != 1) { return _retval(f.i, -51); } }

        T* px2 = pv->pval_0u<T>(f.i);
        if (px2 != px) { return _retval(f.i, -52); }

        if (pv->n() % 2 == 1)
        {
          if (rnd() < 0.5) { l2.move_by(-1); } else { l2.decr(); }
          if (rnd() < 0.5) { l4.move_by(-1); } else { l4.decr(); }
        }

        if (f.i == pv->n() / 2)
        {
          if (l1.pval() != px) { return _retval(f.i, -60); }
          if (l2.pval() != px) { return _retval(f.i, -61); }
          if (l3.pval() != px) { return _retval(f.i, -62); }
          if (l4.pval() != px) { return _retval(f.i, -63); }

          if (l1.ind() != l2.ind()) { return _retval(f.i, -64); }
          if (l1.ind0() != l2.ind0()) { return _retval(f.i, -65); }
          if (l1.ind0() != f.i) { return _retval(f.i, -66); }

          if (l3.ind() != l4.ind()) { return _retval(f.i, -67); }
          if (l3.ind0() != l4.ind0()) { return _retval(f.i, -68); }
          if (l3.ind0() != f.i) { return _retval(f.i, -69); }
        }

        if (pv->n() % 2 == 0)
        {
          if (rnd() < 0.5) { l2.move_by(-1); } else { l2.decr(); }
          if (rnd() < 0.5) { l4.move_by(-1); } else { l4.decr(); }
        }

        if (rnd() < 0.5) { l1.move_by(1); } else { l1.incr(); }
        if (rnd() < 0.5) { l3.move_by(1); } else { l3.incr(); }

        if (l1.ind() != l3.ind() || l1.ind0() != l3.ind0() || l1.ind0() != f.i + 1 || l1.pval() != l3.pval()) { return _retval(f.i, -55); }
        if (l2.ind() != l4.ind() || l2.ind0() != l4.ind0() || l2.ind0() != n1 + m - 1 - f.i || l2.pval() != l4.pval()) { return _retval(f.i, -56); }
      }

        if (l2.ind() != pv->nbase() || l2.is_out() || !l2.pval()) { return -70; }
        if (l1.ind() != pv->nbase() + pv->n() || !l1.is_out() || !l1.is_aend()) { return -71; }
        if (l4.ind() != pv->nbase() || l4.is_out() || !l4.pval()) { return -72; }
        if (l3.ind() != pv->nbase() + pv->n() || !l3.is_out() || !l3.is_aend()) { return -73; }

        if (pv->pval_first<T>() != l2.pval()) { return -75; }
        if (pv->pval_first<T>() != l4.pval()) { return -76; }

      l2.move_by(n1 + m - 1);
      l4.move_by(n1 + m - 1);
        if (pv->pval_last<T>() != l2.pval()) { return -77; }
        if (pv->pval_last<T>() != l4.pval()) { return -78; }
      l1.move_by(-(n1 + m));
      l3.move_by(-(n1 + m));
      l2.move_by(-(n1 + m - 1));
      l4.move_by(-(n1 + m - 1));
        if (pv->pval_first<T>() != l1.pval()) { return -79; }
        if (pv->pval_first<T>() != l2.pval()) { return -80; }
        if (pv->pval_first<T>() != l3.pval()) { return -81; }
        if (pv->pval_first<T>() != l4.pval()) { return -82; }


      l2.set_aend();
      l4.set_aend();
      for (f.i = 0; f.i < n1 + m; ++f.i)
      {
        l1.move_by(f.i);
          if (l1.ind() != pv->nbase() + f.i) { return -83; }
          if (l1.pval() != pv->pval_0u<T>(f.i)) { return -84; }
          l1.move_by(-f.i);
            if (l1.ind() != pv->nbase()) { return -85; }
            if (l1.pval() != pv->pval_first<T>()) { return -86; }
        l3.move_by(f.i);
          if (l3.ind() != pv->nbase() + f.i) { return -87; }
          if (l3.pval() != pv->pval_0u<T>(f.i)) { return -88; }
          l3.move_by(-f.i);
            if (l3.ind() != pv->nbase()) { return -89; }
            if (l3.pval() != pv->pval_first<T>()) { return -90; }
        l2.move_by(-f.i-1);
          if (l2.ind() != pv->nbase() + n1+ m - f.i - 1) { return -91; }
          if (l2.pval() != pv->pval_0u<T>(n1+ m - f.i - 1)) { return -92; }
          l2.move_by(f.i);
            if (l2.ind() != pv->nbase() + n1+ m - 1) { return -93; }
            if (l2.pval() != pv->pval_last<T>()) { return -94; }
            l2.incr();
              if (!l2.is_aend()) { return -95; }
        l4.move_by(-f.i-1);
          if (l4.ind() != pv->nbase() + n1+ m - f.i - 1) { return -101; }
          if (l4.pval() != pv->pval_0u<T>(n1+ m - f.i - 1)) { return -102; }
          l4.move_by(f.i);
            if (l4.ind() != pv->nbase() + n1+ m - 1) { return -103; }
            if (l4.pval() != pv->pval_last<T>()) { return -104; }
            l4.incr();
              if (!l4.is_aend()) { return -105; }
      }


      // Section 3. Modifiers.
      enum { NF = 15, n_cmp = 5 };
      for (s_long q = 0; q < ntests;)
      {
        bool __skip = false;
        bool __cancelled = false;
        s_long __nr = pv->nrsv();
        s_long __n = pv->n();
        if (__n < 0) { return _retval(q, -200); }
        double p_grow = 1.; if (__n >= s_long(0.9 * nmax)) { p_grow = 0.5; } if (__n == nmax) { p_grow = 0.; }
        double p_shrink = 1.; if (__n < s_long(0.1 * nmax)) { p_shrink = 0.5; } if (__n == 0) { p_shrink = 0.; }
        s_long ind_mf = rnd() * NF;

        switch (ind_mf)
        {
          case 0: // vecm_clear
          {
            if (rnd() >= p_shrink) { __skip = true; break; }
            if (pv->nexc() != 0) { return _retval(q, -250); }
            s_long nx = 0; if (excgen_setf_vecm<T>::exists && pv->n() >= 2) { nx = 2; excgen_setf_vecm<T>::F(pv, 0, false, true); excgen_setf_vecm<T>::F(pv, 1 + (pv->n() - 1) * rnd(), false, true); }
            m = pv->vecm_clear();
              if (m != nx) { return _retval(q, -251); }
              __nr = __vecm_x::_cap(0);
            if (pv->nexc() != 0) { return _retval(q, -252); }
            if (rnd() < 0.2) { pv->vecm_setf_can_shrink(!pv->can_shrink()); }
            break;
          }
          case 1: // el_append
          {
            if (rnd() >= p_grow) { __skip = true; break; }
            m = (nmax - __n) * rnd() / 3. + 1;
            if (rnd() < 0.05) { m = nmax - __n; }
            s_long m2 = 0;
            for (f.i = 0; f.i < m; ++f.i)
            {
              if (excgen_setf_vecm<T>::exists && rnd() < 0.3)
              {
                excgen x2(f.i); x2.setf_xc(true);
                  excgen* px = pv->el_append(x2);
                  if (px) { return _retval(q, -272); }
                  if (pv->n() != __n + m2) { return _retval(q, -273); }
              }
              else
              {
                T x9(t_conv::F(f.i)); if (pv->n() >= 1) { _assign<T>::F(x9, *pv->pval_last<T>()); }
                px = pv->el_append(f);
                  if (!px) { return _retval(q, -270); }
                  if (_eq<T, T2>::F(*px, t_conv::F(f.i)) != 1) { return _retval(q, -271); }
                  m2 += 1;
                  if (pv->n() != __n + m2) { return _retval(q, -274); }
                  if (pv->n() >= 2 && _eq<T, T2>::F(x9, *pv->pval_0u<T>(pv->n() - 2)) != 1) { return _retval(q, -276); }
              }
            }
            if (m2 != m) { __cancelled = true; }
            break;
          }
          case 2: // el_remove_all
          {
            if (rnd() >= p_shrink) { __skip = true; break; }
            if (pv->nexc() != 0) { return _retval(q, -290); }
            s_long nx = 0; if (excgen_setf_vecm<T>::exists && pv->n() >= 2) { nx = 2; excgen_setf_vecm<T>::F(pv, pv->n() - 1, false, true); excgen_setf_vecm<T>::F(pv, (pv->n() - 1) * rnd(), false, true); }
            m = pv->el_remove_all();
              if (m != __n) { return _retval(q, -291); }
              if (pv->nexc() != nx) { return _retval(q, -292); }
              pv->vecm_set0_nexc();
            break;
          }
          case 3: // el_reserve_n
          {
            if (rnd() < 0.5) // grow
            {
              if (rnd() >= p_grow) { __skip = true; break; }
              nr = (nmax - __nr) * rnd() + __nr + 1;
              if (!pv->el_reserve_n(nr, false)) { return _retval(q, -211); }
                if (!__vecm_x::nr_ok(nr, pv->nrsv())) { return _retval(q, -210); }
            }
            else // shrink
            {
              if (rnd() >= p_shrink) { __skip = true; break; }
              nr = __nr * rnd();
              if (!pv->el_reserve_n(nr, false)) { return _retval(q, -212); }
                if (pv->nrsv() != __nr) { return _retval(q, -213); }
              if (!pv->el_reserve_n(nr, true)) { return _retval(q, -214); }
                if (nr < __n) { nr = __n; }
                if (!__vecm_x::nr_ok(nr, pv->nrsv())) { return _retval(q, -215); }
            }
            __cancelled = true; // cancel common rsv. checks based on n() change
            break;
          }
          case 4: // el_expand_1
          {
            if (rnd() >= p_grow) { __skip = true; break; }
            px = pv->el_expand_1<T>();
              if (!px) { return _retval(q, -310); }
              if (pv->n() != __n + 1) { return _retval(q, -311); }
            T x(t_conv::F(__n));
              if (pv->ptd()->p_copy_1(pv->pval_last<T>(), &x) < 0) { static_cast<__vecm_x*>(pv)->template forced_clear<T>(__n); return _retval(q, -312); }
            break;
          }
          case 5: // el_expand_n
          {
            if (rnd() < 0.5) // grow
            {
              if (rnd() >= p_grow) { __skip = true; break; }
              n = (nmax - __n) * rnd() + __n + 1;
                if (!pv->el_expand_n(n)) { return _retval(q, -230); }
                  if (pv->n() != n) { return _retval(q, -233); }
              m = 0;
              for (s_long i = __n; i < n; ++i) { T x(t_conv::F(i)); if (pv->ptd()->p_copy_1(pv->pval_0u<T>(i), &x) < 0) { break; } ++m; }
                if (m < n - __n) { static_cast<__vecm_x*>(pv)->template forced_clear<T>(__n + m); return _retval(q, -231); }
            }
            else // shrink
            {
              if (rnd() >= p_shrink) { __skip = true; break; }
              n = __n * rnd();
              m = 0;
              for (s_long i = __n - 1; i >= n; --i) { if (pv->ptd()->p_destroy_1(pv->pval_0u<T>(i)) < 0) { break; } ++m; }
                if (m < __n - n) { static_cast<__vecm_x*>(pv)->template forced_clear<T>(__n - m); return _retval(q, -232); }
              if (!pv->el_expand_n(n)) { return _retval(q, -234); }
                if (pv->n() != n) { return _retval(q, -235); }
            }
            break;
          }
          case 6: // el_expunge_last
          {
            if (rnd() >= p_shrink) { __skip = true; break; }
            if (pv->ptd()->p_destroy_1(pv->pval_last<T>()) < 0) { return _retval(q, -331); }
            if (!pv->el_expunge_last<T>()) { static_cast<__vecm_x*>(pv)->template forced_clear<T>(__n - 1); return _retval(q, -332); }
              if (pv->n() != __n - 1) { return _retval(q, -333); }
            break;
          }
          case 7: // el_remove_last
          {
            if (rnd() >= p_shrink) { __skip = true; break; }
            if (pv->nexc() != 0) { return _retval(q, -350); }
            s_long nx = 0; if (excgen_setf_vecm<T>::exists && rnd() < 0.3) { nx = 1; excgen_setf_vecm<T>::F(pv, pv->n() - 1, false, true); }
            if (pv->el_remove_last<T>() != 1) { return _retval(q, -351); }
              if (pv->nexc() != nx) { return _retval(q, -352); }
              pv->vecm_set0_nexc();
              if (pv->n() != __n - 1) { return _retval(q, -353); }
            break;
          }
          case 8: // el_insert_1
          {
            if (rnd() >= p_grow) { __skip = true; break; }
            m = 1;
            s_long m2 = 0;
            f.i = __n;
            s_long ind0 = (__n + 1) * rnd(); if (rnd() < 0.2) { ind0 = 0; } else if (rnd() < 0.2) { ind0 = __n; }
            s_long ind = ind0 + pv->nbase();
            if (pv->nexc() != 0) { return _retval(q, -420); }
            if (vecm::specf<TA>::transactional != (pv->ptd()->op_flags & 0x1)) { return _retval(q, -421); }
            if (excgen_setf_vecm<T>::exists && rnd() < 0.3)
            {
              excgen x2(f.i); x2.setf_xc(true);
              s_long nx = 0;
              excgen* p0 = 0;
              vecm x9(typer<T>, 0); s_long i9[n_cmp]; if (__n >= 1) { for (s_long i = 0; i < n_cmp; ++i) { i9[i] = __n * rnd(); if (!x9.el_append(*pv->pval_0u<T>(i9[i]))) { return _retval(q, -445); } } }
                if (!(pv->ptd()->op_flags & 0xc)) // non-tr. ins. alg.
                {
                  if (ind0 < __n && !(__n < __vecm_x::_nrsv_n(__n) && ind0 == __vecm_x::_nrsv_n(__n) - __vecm_x::_cap(__n - 1))) { nx += 1; } // exc. is counted for x2 copy only if insertion moves anything
                  if (ind0 < __n - __vecm_x::_cap(__n - 1)) { pv->pval_0u<excgen>(ind0)->setf_xc(true); pv->pval_0u<excgen>(ind0)->setf_xd(true); nx += 2; p0 = pv->pval_0u<excgen>(ind0); }
                }
              m2 = pv->el_insert_1(ind, x2);
                if (m2 == -3)
                {
                  if (pv->n() != __n) { return _retval(q, -422); }
                  if (pv->nexc() != nx)
                  {
                    if (!p0 || p0 != pv->pval_0u<excgen>(ind0)) { return _retval(q, -423); }
                    if (nx - pv->nexc() != 2) { return _retval(q, -496); }
                    if (!(p0->f_xc() && p0->f_xd())) { return _retval(q, -497); }
                    p0->setf_xc(false); p0->setf_xd(false);
                  }
                  if (__n >= 1) { for (s_long i = 0; i < n_cmp; ++i) { if (_eq<T, T2>::F(*x9.pval_0u<T>(i), *pv->pval_0u<T>(i9[i])) != 1) { return _retval(q, -425); } } }
                }
                else if (m2 == -4)
                {
                  m2 = m;
                  if (pv->integrity() != -1) { return _retval(q, -571); }
                    pv->vecm_setf_unsafe(false);
                  if (pv->is_transactional()) { return _retval(q, -426); }
                  if (pv->n() != __n + m) { return _retval(q, -427); }
                  if (pv->nexc() != nx)
                  {
                    if (!p0 || p0 != pv->pval_0u<excgen>(ind0 + 1)) { return _retval(q, -428); }
                    if (nx - pv->nexc() != 2) { return _retval(q, -494); }
                    if (!(p0->f_xc() && p0->f_xd())) { return _retval(q, -495); }
                    p0->setf_xc(false); p0->setf_xd(false);
                  }
                  if (__n >= 1) { for (s_long i = 0; i < n_cmp; ++i) { if (i9[i] == ind0) { continue; } if (_eq<T, T2>::F(*x9.pval_0u<T>(i), *pv->pval_0u<T>(i9[i] < ind0 ? i9[i] : i9[i] + 1)) != 1) { return _retval(q, -429); } } }
                  if (pv->pval_0u<excgen>(ind0)->a != f.i) { return _retval(q, -430); }
                  if (pv->pval_0u<excgen>(ind0)->b != -1) { return _retval(q, -431); }
                  if (pv->pval_0u<excgen>(ind0 + m - 1)->a != f.i) { return _retval(q, -432); }
                  if (pv->pval_0u<excgen>(ind0 + m - 1)->b != -1) { return _retval(q, -433); }
                }
                else
                {
                  return _retval(q, -434);
                }
            }
            else
            {
              s_long nx = 0;
              vecm x9(typer<T>, 0); s_long i9[n_cmp]; if (__n >= 1) { for (s_long i = 0; i < n_cmp; ++i) { i9[i] = __n * rnd(); if (!x9.el_append(*pv->pval_0u<T>(i9[i]))) { return _retval(q, -444); } } }
              bool _b_excgen_mmode3 = excgen_setf_vecm<T>::exists && (pv->ptd()->op_flags & 0x8);
              bool _b_xc(false);
              if (excgen_setf_vecm<T>::exists && rnd() < 0.3) { if (ind0 < __n - __vecm_x::_cap(__n - 1)) { pv->pval_last<excgen>()->setf_xd(true); nx += 1; } }
              if (_b_excgen_mmode3 && rnd() < 0.3 && ind0 < __n) { _b_xc = true; pv->pval_0u<excgen>(ind0)->setf_xc(true); }
              m2 = pv->el_insert_1(ind, f);
                if (m2 != m) { return _retval(q, -435); }
                if (pv->n() != __n + m) { return _retval(q, -436); }
                if (pv->nexc() != nx)
                {
                  if (!excgen_setf_vecm<T>::exists) { return _retval(q, -490); }
                  if (!pv->pval_last<excgen>()->f_xd()) { return _retval(q, -437); }
                  pv->pval_last<excgen>()->setf_xd(false);
                }
                else
                {
                  if (excgen_setf_vecm<T>::exists && pv->pval_last<excgen>()->flags && (!_b_excgen_mmode3 || ind0 != __n - 1)) { return _retval(q, -489); }
                }
                if (_b_excgen_mmode3)
                {
                  if (_b_xc && !pv->pval_0u<excgen>(ind0 + m)->f_xc()) { return _retval(q, -486); }
                  if (_b_xc) { pv->pval_0u<excgen>(ind0 + m)->setf_xc(false); }
                }
                if (__n >= 1) { for (s_long i = 0; i < n_cmp; ++i) { if (_eq<T, T2>::F(*x9.pval_0u<T>(i), *pv->pval_0u<T>(i9[i] < ind0 ? i9[i] : i9[i] + 1)) != 1) { return _retval(q, -438); } } }
                if (excgen_setf_vecm<T>::exists)
                {
                  if (pv->pval_0u<excgen>(ind0)->a != f.i) { return _retval(q, -439); }
                  if (pv->pval_0u<excgen>(ind0)->b != f.i) { return _retval(q, -440); }
                  if (pv->pval_0u<excgen>(ind0 + m - 1)->a != f.i) { return _retval(q, -441); }
                  if (pv->pval_0u<excgen>(ind0 + m - 1)->b != f.i) { return _retval(q, -442); }
                }
                else
                {
                  if (_eq<T, T2>::F(*pv->pval_0u<T>(ind0), t_conv::F(f.i)) != 1) { return _retval(q, -443); }
                  if (_eq<T, T2>::F(*pv->pval_0u<T>(ind0 + m - 1), t_conv::F(f.i)) != 1) { return _retval(q, -446); }
                }
              m2 = m;
            }
            pv->vecm_set0_nexc();
            if (m2 != m) { __cancelled = true; }
            break;
          }
          case 9: // el_remove_1
          {
            if (rnd() >= p_shrink) { __skip = true; break; }
            m = 1;
            s_long m2 = 0;
            s_long ind0 = __n * rnd(); if (rnd() < 0.1) { ind0 = 0; } else if (rnd() < 0.1) { ind0 = __n - 1; }
            s_long ind = ind0 + pv->nbase();
            if (pv->nexc() != 0) { return _retval(q, -460); }
            if (vecm::specf<TA>::transactional != (pv->ptd()->op_flags & 0x1)) { return _retval(q, -461); }

            if (rnd() < 0.3) // test move_last == true
            {
              s_long nx = 0;
              vecm x9(typer<T>, 0); if (!x9.el_append(*pv->pval_last<T>())) { return _retval(q, -462); }
              if (excgen_setf_vecm<T>::exists && rnd() < 0.3)
              {
                pv->pval_0u<excgen>(ind0)->setf_xd(true);
                pv->pval_0u<excgen>(__n - 1)->setf_xc(true);
                m2 = pv->el_remove_1<T>(ind, true);
                  if (m2 == -3)
                  {
                    if (ind0 == __n - 1) { return _retval(q, -463); }
                    if (!(pv->ptd()->op_flags & 0x1)) { return _retval(q, -464); }
                    if (pv->n() != __n) { return _retval(q, -465); }

                    pv->pval_0u<excgen>(ind0)->setf_xd(false);
                    pv->pval_0u<excgen>(__n - 1)->setf_xc(false);

                    if (ind0 != __n - 1 && !_eq<T, T2>::F(*pv->pval_last<T>(), *x9.pval_first<T>())) { return _retval(q, -466); }
                  }
                  else if (m2 == -4)
                  {
                    if (pv->integrity() != -1) { return _retval(q, -572); }
                      pv->vecm_setf_unsafe(false);
                    if (ind0 == __n - 1) { return _retval(q, -467); }
                    if (pv->n() != __n - 1) { return _retval(q, -468); }
                    if (pv->nexc() != 2) { return _retval(q, -468); }
                    if (pv->pval_0u<excgen>(ind0)->a != x9.pval_first<excgen>()->a) { return _retval(q, -470); }
                    if (pv->pval_0u<excgen>(ind0)->b != -1) { return _retval(q, -471); }
                    m2 = m;
                  }
                  else
                  {
                    if (__n - ind0 == 1) // removed at the end
                    {
                      if (m2 != 1) { return _retval(q, -472); }
                      if (pv->n() != __n - 1) { return _retval(q, -474); }
                      if (pv->nexc() != 1) { return _retval(q, -475); }
                    }
                    else // possibly removed using safemove
                    {
                      if (!((pv->ptd()->op_flags & 0x4) == 0)) { return -480; }
                      if (m2 != 1) { return _retval(q, -481); }
                      if (!(pv->n() > ind0)) { return _retval(q, -488); }
                        pv->pval_0u<excgen>(ind0)->setf_xc(false);
                      if (pv->n() != __n - 1) { return _retval(q, -482); }
                      if (pv->nexc() != 1) { return _retval(q, -483); }
                      if (ind0 != __n - 1 && !_eq<T, T2>::F(*pv->pval_0u<T>(ind0), *x9.pval_first<T>())) { return _retval(q, -484); }
                    }
                  }
              }
              else
              {
                nx = 0;
                if (excgen_setf_vecm<T>::exists && rnd() < 0.3)
                {
                  pv->pval_0u<excgen>(ind0)->setf_xd(true);
                  nx += 1;
                }
                m2 = pv->el_remove_1<T>(ind, true);
                  if (m2 != 1) { return _retval(q, -476); }
                  if (pv->n() != __n - 1) { return _retval(q, -477); }
                  if (pv->nexc() != nx)
                  {
                    if (pv->ptd()->op_flags & 0x8) // excgen_mmode3 only
                    {
                      if (!pv->pval_last<excgen>()->f_xd()) { return _retval(q, -485); }
                      pv->pval_last<excgen>()->setf_xd(false);
                      if (pv->pval_last<excgen>()->flags) { return _retval(q, -491); }
                    }
                    else { return _retval(q, -478); }
                  }
                  if (ind0 != __n - 1 && !_eq<T, T2>::F(*pv->pval_0u<T>(ind0), *x9.pval_first<T>())) { return _retval(q, -479); }
              }
            }
            else // test move_last == false
            {
              if (excgen_setf_vecm<T>::exists && rnd() < 0.3)
              {
                s_long nx = 0;
                const s_long n_cmp2 = n_cmp < m ? s_long(n_cmp) : m;
                vecm x9(typer<T>, 0); s_long i9[n_cmp];
                vecm x9b(typer<T>, 0); std::vector<s_long> i9b; try { i9b.resize(n_cmp2); } catch (...) { return _retval(q, -566); }
                  bool _tr_alg = (__n - ind0 > m) && (pv->ptd()->op_flags & 0x1); // most prob. that tr. alg. will be called (may be non-tr. in rare cases)
                  if (__n > m) { for (s_long i = 0; i < n_cmp; ++i) { i9[i] = (__n - m) * rnd(); if (!x9.el_append(*pv->pval_0u<T>(i9[i] < ind0 ? i9[i] : i9[i] + m))) { return _retval(q, -500); } } }
                  for (s_long i = 0; i < n_cmp2; ++i) { i9b[i] = m * rnd(); if (!x9b.el_append(*pv->pval_0u<T>(ind0 + i9b[i]))) { return _retval(q, -501); } }
                  s_long __j1, __j2, __j0, __k, __cap; __vecm_x::_ind_jk(ind0, __j0, __k, __cap); __vecm_x::_ind_jk(ind0 + m - 1, __j1, __k, __cap); __vecm_x::_ind_jk(__n - 1, __j2, __k, __cap);
                  pv->pval_0u<excgen>(ind0)->setf_xd(true);
                    nx += 1;
                  for (s_long j = __j1 + 1; j <= __j2;  ++j) { pv->pval_0u<excgen>(__vecm_x::_nrsv(j))->setf_xc(true); }
                    s_long nx2 = __j2 - __j1;
                m2 = pv->el_remove_1<T>(ind, false);
                  if (m2 == -3)
                  {
                    if (!_tr_alg) { return _retval(q, -502); }
                    if (pv->n() != __n) { return _retval(q, -503); }
                    if (pv->nexc() != 0) { return _retval(q, -504); }

                    pv->pval_0u<excgen>(ind0)->setf_xd(false);
                    for (s_long j = __j1 + 1; j <= __j2;  ++j) { pv->pval_0u<excgen>(__vecm_x::_nrsv(j))->setf_xc(false); }

                    if (__n > m) { for (s_long i = 0; i < n_cmp; ++i) { if (_eq<T, T2>::F(*x9.pval_0u<T>(i), *pv->pval_0u<T>(i9[i] < ind0 ? i9[i] : i9[i] + m)) != 1) { return _retval(q, -505); } } }
                    for (s_long i = 0; i < n_cmp2; ++i) { if (_eq<T, T2>::F(*x9b.pval_0u<T>(i), *pv->pval_0u<T>(ind0 + i9b[i])) != 1) { return _retval(q, -506); } }
                  }
                  else if (m2 == -4)
                  {
                    if (pv->integrity() != -1) { return _retval(q, -573); }
                      pv->vecm_setf_unsafe(false);
                    if (pv->n() != __n - m) { return _retval(q, -507); }
                    if (pv->nexc() != nx + nx2) { return _retval(q, -508); }
                    if (__n > m) { for (s_long i = 0; i < n_cmp; ++i)
                    {
                      if (x9.pval_0u<excgen>(i)->a != pv->pval_0u<excgen>(i9[i])->a) { return _retval(q, -509); }
                      if (x9.pval_0u<excgen>(i)->b != pv->pval_0u<excgen>(i9[i])->b && -1 != pv->pval_0u<excgen>(i9[i])->b) { return _retval(q, -510); }
                    } }
                    m2 = m;
                  }
                  else
                  {
                    if (m2 != m) { return _retval(q, -511); }
                    if (pv->n() != __n - m) { return _retval(q, -512); }
                    if (ind0 + m == __n) // removed at the end
                    {
                      if (pv->nexc() != nx + nx2) { return _retval(q, -513); }
                    }
                    else // removed using safemove and/or from the last column
                    {
                      if ((pv->ptd()->op_flags & 0x4) == 0) // non-tr. moving
                      {
                        if (pv->nexc() != nx) { return _retval(q, -515); }
                        for (s_long i = __vecm_x::_nrsv(__j0); i < pv->n();  ++i) { pv->pval_0u<excgen>(i)->setf_xc(false); }
                      }
                      else
                      {
                        if (pv->nexc() != nx + nx2) { return _retval(q, -521); }
                        if (!(m == 1 && __j2 == __j1)) { return _retval(q, -514); }
                      }
                    }
                    m2 = m;
                  }
              }
              else
              {
                s_long nx = 0;
                vecm x9(typer<T>, 0); s_long i9[n_cmp];
                  if (__n > m) { for (s_long i = 0; i < n_cmp; ++i) { i9[i] = (__n - m) * rnd(); if (!x9.el_append(*pv->pval_0u<T>(i9[i] < ind0 ? i9[i] : i9[i] + m))) { return _retval(q, -516); } } }
                  if (excgen_setf_vecm<T>::exists && rnd() < 0.3)
                  {
                    pv->pval_0u<excgen>(ind0)->setf_xd(true);
                      nx += 1;
                  }
                m2 = pv->el_remove_1<T>(ind, false);
                  if (m2 != m) { return _retval(q, -517); }
                  if (pv->n() != __n - m) { return _retval(q, -518); }
                  if (pv->nexc() != nx) { return _retval(q, -519); }
                  if (__n > m) { for (s_long i = 0; i < n_cmp; ++i) { if (_eq<T, T2>::F(*x9.pval_0u<T>(i), *pv->pval_0u<T>(i9[i])) != 1) { return _retval(q, -520); } } }
              }
            }
            pv->vecm_set0_nexc();
            if (m2 != m) { __cancelled = true; }
            break;
          }
          case 10: // el_append_m
          {
            if (rnd() >= p_grow) { __skip = true; break; }
            m = (nmax - __n) * rnd() + 1;
            s_long m2 = 0; f.i = __n;
            if (excgen_setf_vecm<T>::exists && rnd() < 0.3)
            {
              excgen x2(f.i); x2.setf_xc(true);
                m2 = pv->el_append_m(m, x2);
                if (m2 != -3) { return _retval(q, -370); }
                if (pv->n() != __n) { return _retval(q, -371); }
                if (!(m == 1) && pv->nrsv() != __nr) { return _retval(q, -372); } // if not using el_append, must revert the reserve
            }
            else
            {
              T x9(t_conv::F(f.i)); if (__n >= 1) { _assign<T>::F(x9, *pv->pval_last<T>()); }
              m2 = pv->el_append_m(m, f);
                if (m2 != m) { return _retval(q, -373); }
                if (pv->n() != __n + m) { return _retval(q, -374); }
                if (_eq<T, T2>::F(*pv->pval_last<T>(), t_conv::F(f.i)) != 1) { return _retval(q, -375); }
                if (__n >= 1 && _eq<T, T2>::F(x9, *pv->pval_0u<T>(__n - 1)) != 1) { return _retval(q, -376); }
            }
            if (m2 != m) { __cancelled = true; }
            break;
          }
          case 11: // el_insert_ml
          {
            if (rnd() >= p_grow) { __skip = true; break; }
            m = (nmax - __n) * rnd() + 1;
            s_long m2 = 0;
            f.i = __n;
            s_long ind0 = (__n + 1) * rnd(); if (rnd() < 0.2) { ind0 = 0; } else if (rnd() < 0.2) { ind0 = __n; }
            s_long ind = ind0 + pv->nbase();
            if (pv->nexc() != 0) { return _retval(q, -390); }
            if (vecm::specf<TA>::transactional != (pv->ptd()->op_flags & 0x1)) { return _retval(q, -414); }
            if (excgen_setf_vecm<T>::exists && rnd() < 0.3)
            {
              T x9(t_conv::F(f.i)); if (__n >= 1) { _assign<T>::F(x9, *pv->pval_last<T>()); }
              excgen x2(f.i); x2.setf_xc(true);
              s_long nx = 0;
                if (pv->ptd()->op_flags & 0xc) // tr. ins. alg.
                {
                  // nx += 1; // exc. on the first x2 copy won't be counted in tr. mode
                  // This exc. will not occur, since exc. on x2 copy occurs first and the transaction is cancelled.
                  if (ind0 < __n - __vecm_x::_cap(__n - 1)) { pv->pval_last<excgen>()->setf_xd(true); }
                }
                else
                {
                  if (ind0 < __n) { nx += m; } // if not appending, exc. is counted for each x2 copy
                  // Both exc. will occur.
                  if (ind0 < __n - __vecm_x::_cap(__n - 1)) { pv->pval_0u<excgen>(__n - 2)->setf_xc(true); pv->pval_0u<excgen>(__n - 2)->setf_xd(true); nx += 2; }
                }
              m2 = pv->el_insert_ml(ind, m, x2);
                if (m2 == -3)
                {
                  if (pv->n() != __n) { return _retval(q, -392); }
                  if (pv->nexc() != nx) { return _retval(q, -393); }
                  if (!(m == 1 && ind0 == __n) && pv->nrsv() != __nr) { return _retval(q, -394); } // if not using el_append, must revert the reserve
                  if (__n >= 1 && _eq<T, T2>::F(x9, *pv->pval_last<T>()) != 1) { return _retval(q, -395); }
                  // Disable exc.
                  if (ind0 < __n - __vecm_x::_cap(__n - 1)) { pv->pval_last<excgen>()->setf_xd(false); }
                }
                else if (m2 == -4)
                {
                  m2 = m;
                  if (pv->integrity() != -1) { return _retval(q, -574); }
                    pv->vecm_setf_unsafe(false);
                  if (pv->is_transactional()) { return _retval(q, -396); }
                  if (pv->n() != __n + m) { return _retval(q, -397); }
                  if (pv->nexc() != nx)
                  {
                    if (pv->ptd()->op_flags & 0x8) // excgen_mmode3 only
                    {
                      if (!pv->pval_last<excgen>()->f_xd()) { return _retval(q, -417); }
                      pv->pval_last<excgen>()->setf_xd(false);
                      if (pv->pval_last<excgen>()->flags) { return _retval(q, -492); }
                    }
                    else { return _retval(q, -398); }
                  }
                  if (__n >= 1 && _eq<T, T2>::F(x9, *pv->pval_0u<T>(ind0 == __n ? __n - 1 : __n - 1 + m)) != 1) { return _retval(q, -399); }
                  if (pv->pval_0u<excgen>(ind0)->a != f.i) { return _retval(q, -400); }
                  if (pv->pval_0u<excgen>(ind0)->b != -1) { return _retval(q, -401); }
                  if (pv->pval_0u<excgen>(ind0 + m - 1)->a != f.i) { return _retval(q, -402); }
                  if (pv->pval_0u<excgen>(ind0 + m - 1)->b != -1) { return _retval(q, -403); }
                }
                else
                {
                  return _retval(q, -404);
                }
            }
            else
            {
              T x9(t_conv::F(f.i));
              if (__n >= 1) { _assign<T>::F(x9, *pv->pval_last<T>()); }
              s_long nx = 0;
              bool _b_excgen_mmode3 = excgen_setf_vecm<T>::exists && (pv->ptd()->op_flags & 0x8);
              bool _b_xc(false);
              if (excgen_setf_vecm<T>::exists && rnd() < 0.3) { if (ind0 < __n - __vecm_x::_cap(__n - 1)) { pv->pval_last<excgen>()->setf_xd(true); nx += 1; } }
              if (_b_excgen_mmode3 && rnd() < 0.3 && ind0 < __n) { _b_xc = true; pv->pval_0u<excgen>(ind0)->setf_xc(true); }
              m2 = pv->el_insert_ml(ind, m, f);
                if (m2 != m) { return _retval(q, -405); }
                if (pv->n() != __n + m) { return _retval(q, -406); }
                if (pv->nexc() != nx)
                {
                  if (!pv->pval_last<excgen>()->f_xd()) { return _retval(q, -416); }
                  pv->pval_last<excgen>()->setf_xd(false);
                  if (!excgen_setf_vecm<T>::exists) { return _retval(q, -493); }
                }
                else
                {
                  if (excgen_setf_vecm<T>::exists && pv->pval_last<excgen>()->flags && (!_b_excgen_mmode3 || ind0 != __n - 1)) { return _retval(q, -407); }
                }
                if (_b_excgen_mmode3)
                {
                  if (_b_xc && !pv->pval_0u<excgen>(ind0 + m)->f_xc()) { return _retval(q, -487); }
                  if (_b_xc) { pv->pval_0u<excgen>(ind0 + m)->setf_xc(false); }
                }
                if (__n >= 1 && _eq<T, T2>::F(x9, *pv->pval_0u<T>(ind0 == __n ? __n - 1 : __n - 1 + m)) != 1) { return _retval(q, -408); }
                if (excgen_setf_vecm<T>::exists)
                {
                  if (pv->pval_0u<excgen>(ind0)->a != f.i) { return _retval(q, -409); }
                  if (pv->pval_0u<excgen>(ind0)->b != f.i) { return _retval(q, -410); }
                  if (pv->pval_0u<excgen>(ind0 + m - 1)->a != f.i) { return _retval(q, -411); }
                  if (pv->pval_0u<excgen>(ind0 + m - 1)->b != f.i) { return _retval(q, -412); }
                }
                else
                {
                  if (_eq<T, T2>::F(*pv->pval_0u<T>(ind0), t_conv::F(f.i)) != 1) { return _retval(q, -413); }
                  if (_eq<T, T2>::F(*pv->pval_0u<T>(ind0 + m - 1), t_conv::F(f.i)) != 1) { return _retval(q, -415); }
                }
            }
            pv->vecm_set0_nexc();
            if (m2 != m) { __cancelled = true; }
            break;
          }
          case 12: // el_remove_ml
          {
            if (rnd() >= p_shrink) { __skip = true; break; }
            s_long m2 = 0;
            s_long ind0 = __n * rnd(); if (rnd() < 0.1) { ind0 = 0; } else if (rnd() < 0.1) { ind0 = __n - 1; }
            s_long ind = ind0 + pv->nbase();
            m = 1 + (__n - ind0) * rnd();
            if (pv->nexc() != 0) { return _retval(q, -540); }
            if (vecm::specf<TA>::transactional != (pv->ptd()->op_flags & 0x1)) { return _retval(q, -541); }

            if (excgen_setf_vecm<T>::exists && rnd() < 0.3)
            {
              s_long nx = 0;
              const s_long n_cmp2 = n_cmp < m ? s_long(n_cmp) : m;
              vecm x9(typer<T>, 0); s_long i9[n_cmp];
              vecm x9b(typer<T>, 0); std::vector<s_long> i9b; try { i9b.resize(n_cmp2); } catch (...) { return _retval(q, -565); }
                bool _tr_alg = (__n - ind0 > m) && (pv->ptd()->op_flags & 0x1); // most prob. that tr. alg. will be called (may be non-tr. in rare cases)
                if (__n > m) { for (s_long i = 0; i < n_cmp; ++i) { i9[i] = (__n - m) * rnd(); if (!x9.el_append(*pv->pval_0u<T>(i9[i] < ind0 ? i9[i] : i9[i] + m))) { return _retval(q, -542); } } }
                for (s_long i = 0; i < n_cmp2; ++i) { i9b[i] = m * rnd(); if (!x9b.el_append(*pv->pval_0u<T>(ind0 + i9b[i]))) { return _retval(q, -543); } }
                pv->pval_0u<excgen>(ind0)->setf_xd(true);
                pv->pval_0u<excgen>(ind0 + m - 1)->setf_xd(true);
                  nx += m == 1 ? 1 : 2;
                const s_long x2_step = 17;
                for (s_long i = ind0 + m; i < __n; i += x2_step) { pv->pval_0u<excgen>(i)->setf_xc(true); }
                  s_long nx2 = (__n - (ind0 + m) + x2_step - 1) / x2_step;
              m2 = pv->el_remove_ml<T>(ind, m);
                if (m2 == -3)
                {
                  if (!_tr_alg) { return _retval(q, -544); }
                  if (pv->n() != __n) { return _retval(q, -545); }
                  if (pv->nexc() != 0) { return _retval(q, -546); }

                  pv->pval_0u<excgen>(ind0)->setf_xd(false);
                  pv->pval_0u<excgen>(ind0 + m - 1)->setf_xd(false);
                  for (s_long i = ind0 + m; i < __n; i += x2_step) { pv->pval_0u<excgen>(i)->setf_xc(false); }

                  if (__n > m) { for (s_long i = 0; i < n_cmp; ++i) { if (_eq<T, T2>::F(*x9.pval_0u<T>(i), *pv->pval_0u<T>(i9[i] < ind0 ? i9[i] : i9[i] + m)) != 1) { return _retval(q, -547); } } }
                  for (s_long i = 0; i < n_cmp2; ++i) { if (_eq<T, T2>::F(*x9b.pval_0u<T>(i), *pv->pval_0u<T>(ind0 + i9b[i])) != 1) { return _retval(q, -548); } }
                }
                else if (m2 == -4)
                {
                  if (pv->integrity() != -1) { return _retval(q, -575); }
                    pv->vecm_setf_unsafe(false);
                  if (pv->n() != __n - m) { return _retval(q, -549); }
                  if (pv->nexc() != nx + nx2) { return _retval(q, -550); }
                  if (__n > m) { for (s_long i = 0; i < n_cmp; ++i)
                  {
                    if (x9.pval_0u<excgen>(i)->a != pv->pval_0u<excgen>(i9[i])->a) { return _retval(q, -551); }
                    if (x9.pval_0u<excgen>(i)->b != pv->pval_0u<excgen>(i9[i])->b && -1 != pv->pval_0u<excgen>(i9[i])->b) { return _retval(q, -552); }
                  } }
                  m2 = m;
                }
                else
                {
                  if (__n - ind0 == m) // removed at the end
                  {
                    if (m2 != m) { return _retval(q, -553); }
                    if (pv->n() != __n - m) { return _retval(q, -555); }
                    if (pv->nexc() != nx + nx2) { return _retval(q, -556); }
                  }
                  else // possibly removed using safemove
                  {
                    if (!((pv->ptd()->op_flags & 0x4) == 0)) { return -554; }
                    if (m2 != m) { return _retval(q, -561); }
                    if (pv->n() != __n - m) { return _retval(q, -562); }
                    if (pv->nexc() != nx) { return _retval(q, -563); }
                    for (s_long i = ind0; i < pv->n(); i += x2_step) { pv->pval_0u<excgen>(i)->setf_xc(false); }
                    if (__n > m) { for (s_long i = 0; i < n_cmp; ++i) { if (*x9.pval_0u<excgen>(i) != *pv->pval_0u<excgen>(i9[i])) { return _retval(q, -564); } } }
                  }
                  m2 = m;
                }
            }
            else
            {
              s_long nx = 0;
              vecm x9(typer<T>, 0); s_long i9[n_cmp];
                if (__n > m) { for (s_long i = 0; i < n_cmp; ++i) { i9[i] = (__n - m) * rnd(); if (!x9.el_append(*pv->pval_0u<T>(i9[i] < ind0 ? i9[i] : i9[i] + m))) { return _retval(q, -557); } } }
                if (excgen_setf_vecm<T>::exists && rnd() < 0.3)
                {
                  pv->pval_0u<excgen>(ind0)->setf_xd(true);
                  pv->pval_0u<excgen>(ind0 + m - 1)->setf_xd(true);
                    nx += m == 1 ? 1 : 2;
                }
              m2 = pv->el_remove_ml<T>(ind, m);
                if (m2 != m) { return _retval(q, -558); }
                if (pv->n() != __n - m) { return _retval(q, -559); }
                if (pv->nexc() != nx) { return _retval(q, -560); }
                if (__n > m) { for (s_long i = 0; i < n_cmp; ++i) { if (_eq<T, T2>::F(*x9.pval_0u<T>(i), *pv->pval_0u<T>(i9[i])) != 1) { return _retval(q, -570); } } }
            }
            pv->vecm_set0_nexc();
            if (m2 != m) { __cancelled = true; }
            break;
          }
          case 13: // copy constructor
          {
            vecm v2(*pv);
            if (v2.integrity() != 1) { return _retval(q, -621); }
            if (v2.n() != __n) { return _retval(q, -622); }
            if (v2.nbase() != pv->nbase()) { return _retval(q, -625); }
            if (v2.nexc() != 0) { return _retval(q, -623); }
            if (v2.can_shrink() != true) { return _retval(q, -624); }
            for (s_long i = 0; i < __n; ++i) { if (_eq<T, T2>::F(*v2.pval_0u<T>(i), *pv->pval_0u<T>(i)) != 1) { return _retval(q, -626); } }
            __cancelled = true; // pv is not changed in this test
            break;
          }
          case 14: // vecm_copy
          {
            vecm v2(typer<T>, 0);
            if (v2.vecm_copy(*pv, q & 1) != 1) { return _retval(q, -600); }
            if (v2.integrity() != 1) { return _retval(q, -601); }
            if (v2.n() != __n) { return _retval(q, -602); }
            if (v2.nbase() != pv->nbase()) { return _retval(q, -605); }
            if (v2.nexc() != 0) { return _retval(q, -603); }
            if (v2.can_shrink() != true) { return _retval(q, -604); }
            for (s_long i = 0; i < __n; ++i) { if (_eq<T, T2>::F(*v2.pval_0u<T>(i), *pv->pval_0u<T>(i)) != 1) { return _retval(q, -606); } }
            __cancelled = true; // pv is not changed in this test
            break;
          }
          default: { return -8; }
        }
        if (__skip) { continue; }

        if (pv->integrity() < 0) { return _retval(q, -880 - ind_mf); }
        if (pv->nrsv() < pv->n()) { return _retval(q, -900 - ind_mf); }
        if (!__cancelled)
        {
          if (pv->n() < __n)
          {
            if (pv->can_shrink()) { if (!__vecm_x::nr_ok(pv)) { return _retval(q, -920 - ind_mf); } }
              else { if (pv->nrsv() != __nr) { return _retval(q, -940 - ind_mf); } }
          }
          else if (pv->n() > __n)
          {
            if (pv->nrsv() != __nr && !__vecm_x::nr_ok(pv)) { return _retval(q, -960 - ind_mf); }
          }
          else { return _retval(q, -980 - ind_mf); }
        }

        ++q;
      }
      return 1;
    }
    };

    struct container_copy_wrapper
    {
      template<class H>
      struct hx_copy_test
      {
        typedef typename H::t_v V;
        static int F(logger& log, int f, bool is_tr, bool exc_c, bool exc_d)
        {
          enum { m = 9, m2 = m+1, q = 3 }; // q == [nb..nb+m-1]
          excgen::set_cnt(1);
          H ha; for (s_long i = 0; i < m; ++i) if (ha.insert(i) != 1) { return -10; }
          if (!ha.find(q)) { return -11; }

          ha[q].setf_xc(exc_c);
          ha[q].setf_xd(exc_d);

          if (f == 4)
          {
            H hb(ha);
            if (hb.nexc() == 0) // copy ctor. succeeded
            {
              if (exc_c) { return -110; }
              if (hb.n() != m) { return -112; }
              if (!hb.can_shrink()) { return -113; }
              for (s_long i = 0; i < m; ++i) { if (!hb.find(i)) { return -114; } else { excgen& x = hb[i]; if (x.a != x.b || x.a != 1 + i) { return -115; } } }
            }
            else // copy ctor. failed
            {
              if (!exc_c) { return -210; }
              if (hb.n() != 0) { return -212; }
              if (!hb.can_shrink()) { return -213; }
            }
            return 1;
          }
          else if (f == 5)
          {
            H hb; for (s_long i = 0; i < m2; ++i) if (hb.insert(m+i) != 1) { return -10; }
            if (!hb.find(m+q)) { return -11; }
            if (hb.n() != m2) { return -12; }
            if (hb(m2-1)->v.a != m + m2) { return -13; }
            hb[m+q].setf_xd(exc_d);
            hb.hashx_setf_can_shrink(false);

            switch (hb.hashx_copy(ha, is_tr))
            {
              case 1:
                {
                  if (exc_c) { return -210; }
                  if (exc_d && hb.nexc() != 1) { return -211; }
                  if (hb.n() != m) { return -212; }
                  if (!hb.can_shrink()) { return -214; }
                  for (s_long i = 0; i < m; ++i) { if (!hb.find(i)) { return -216; } excgen& x = hb[i]; if (x.a != x.b || x.a != i + 1) { return -215; } }
                  break;
                }
              case 0:
                {
                  if (!exc_c) { return -220; }
                  if (hb.nexc() != 1) { return -221; }
                  if (hb.n() != m2) { return -222; }
                  if (hb.can_shrink()) { return -224; }
                  for (s_long i = m; i < m + m2; ++i) { if (!hb.find(i)) { return -226; } excgen& x = hb[i]; if (x.a != x.b || x.a != i + 1) { return -225; } }
                  break;
                }
              case -1:
                {
                  if (!exc_c) { return -230; }
                  if (exc_d && hb.nexc() != 2) { return -231; }
                  if (!exc_d && hb.nexc() != 1) { return -235; }
                  if (hb.n() != 0) { return -232; }
                  if (!hb.can_shrink()) { return -236; }
                  break;
                }
              default: return -240;
            }

            return 1;
          }
          return -1;
        }
      };
      template<class VA>
      struct vm_copy_test
      {
        static int F(logger& log, int f, bool is_tr, bool exc_c, bool exc_d)
        {
          enum { m = 9, m2 = m+1, q = 3, nb = 1 }; // q == [nb..nb+m-1]
          excgen::set_cnt(1);
          vecm va(typer<VA>, nb); if (va.el_append_m(m, meta::construct_f<excgen>()) != m) { return -10; }
          if (!(q >= nb && q <= nb + m - 1)) { return -11; }

          va.pval<excgen>(q)->setf_xc(exc_c);
          va.pval<excgen>(q)->setf_xd(exc_d);

          if (f == 1)
          {
            va.vecm_setf_can_shrink(false);
            vecm vb(va);
            if (vb.nexc() == 0) // copy ctor. succeeded
            {
              if (exc_c) { return -110; }
              if (vb.n() != m) { return -112; }
              if (vb.nbase() != nb) { return -113; }
              if (!vb.can_shrink()) { return -114; }
              for (s_long i = nb; i < nb + m; ++i) { excgen& x = *vb.pval<excgen>(i); if (x.a != x.b || x.a != 1 + i - nb) { return -115; } }
            }
            else // copy ctor. failed
            {
              if (!exc_c) { return -120; }
              if (vb.nexc() != 1) { return -121; }
              if (vb.n() != 0) { return -122; }
              if (vb.nbase() != nb) { return -123; }
              if (!vb.can_shrink()) { return -124; }
            }
            return 1;
          }
          else if (f == 2)
          {
            vecm vb(typer<VA>, 0); if (vb.el_append_m(m2, meta::construct_f<excgen>()) != m2) { return -200; }
            if (vb.pval_last<excgen>()->a != m + m2) { return -201; }
            vb.pval<excgen>(q-nb)->setf_xd(exc_d);
            vb.vecm_setf_can_shrink(false);

            switch (vb.vecm_copy(va, is_tr))
            {
              case 1:
                {
                  if (exc_c) { return -210; }
                  if (exc_d && vb.nexc() != 1) { return -211; }
                  if (vb.n() != m) { return -212; }
                  if (vb.nbase() != nb) { return -213; }
                  if (!vb.can_shrink()) { return -214; }
                  for (s_long i = nb; i < nb + m; ++i) { excgen& x = *vb.pval<excgen>(i); if (x.a != x.b || x.a != 1 + i - nb) { return -215; } }
                  break;
                }
              case 0:
                {
                  if (!exc_c) { return -220; }
                  if (vb.nexc() != 1) { return -221; }
                  if (vb.n() != m2) { return -222; }
                  if (vb.nbase() != 0) { return -223; }
                  if (vb.can_shrink()) { return -224; }
                  for (s_long i = 0; i < m2; ++i) { excgen& x = *vb.pval<excgen>(i); if (x.a != x.b || x.a != m + 1 + i) { return -225; } }
                  break;
                }
              case -1:
                {
                  if (!exc_c) { return -230; }
                  if (exc_d && vb.nexc() != 2) { return -231; }
                  if (!exc_d && vb.nexc() != 1) { return -235; }
                  if (vb.n() != 0) { return -232; }
                  if (vb.nbase() != nb) { return -233; }
                  if (!vb.can_shrink()) { return -236; }
                  break;
                }
              default: return -240;
            }
            return 1;
          }
          else if (f == 3)
          {
            s_long* pt = reinterpret_cast<s_long*>(&va); s_long t = *pt; *pt = 0;
            vecm vb(va);
            *pt = t;
            if (!vb.ptd()) { return -300; }
            if (vb.ptd()->t_ind != bytes::type_index_t<s_long>::ind()) { return -301; }
            if (vb.nexc() != 1) { return -304; }
            if (vb.n() != 0) { return -302; }
            if (vb.nbase() != 0) { return -303; }
            if (!vb.can_shrink()) { return -304; }
            return 1;
          }
          return -1;
        }
      };
        //  f
        //    1 vecm(const vecm&)
        //    2 vecm_copy
        //    3 vecm(const vecm& x)  when x._t_ind is erased by 0
        //    4 hashx(const hashx&)
        //    5 hashx_copy
        //  tr_mode 0 / 1 / 2 / 3: excgen (tr. mode) / excgen_mmode3 (tr. mode w/o backup) / excgen_nt (non-tr. mode) / skip test
        //  gen. exc. in copy ctor on / off
        //  gen. exc. in dtor on / off
      enum { NF1 = 5, NF2 = 4, NF3 = 2, NF4 = 2, NF_all = NF1 * NF2 * NF3 * NF4, F4m = 1, F3m = F4m * NF4, F2m = F3m * NF2, F1m = F2m * NF1 };
      static int F(logger& log, int f, s_long tr_mode, bool exc_c, bool exc_d)
      {
        if (f >= 1 && f <= 3)
        {
          switch(tr_mode)
          {
            case 0: return vm_copy_test<excgen>::F(log, f, true, exc_c, exc_d);
            case 1: return vm_copy_test<excgen_mmode3>::F(log, f, true, exc_c, exc_d);
            case 2: return vm_copy_test<excgen_nt>::F(log, f, false, exc_c, exc_d);
            case 3: return 0;
            default: return -1;
          }
        }
        if (f >= 4 && f <= 5)
        {
          switch(tr_mode)
          {
            case 0: return hx_copy_test<hashx<double, meta::construct_f<excgen> > >::F(log, f, true, exc_c, exc_d);
            case 1: return hx_copy_test<hashx<double, meta::construct_f<excgen_mmode3> > >::F(log, f, true, exc_c, exc_d);
            case 2: return hx_copy_test<hashx<double, meta::construct_f<excgen_nt> > >::F(log, f, false, exc_c, exc_d);
            case 3: return 0;
            default: return -1;
          }
        }
        return -1;
      }
    };

    struct __hfns
    {
      template<class Q> struct _alt_hashf_int { inline s_long hash(const Q& x) const { if (double(x) >= 0 && double(x) <= double(meta::s_long_mp)) { return hashx_common::kf_basic<s_long>().hash(s_long(x)); } return hashx_common::kf_basic<Q>().hash(x); } };
      template<class Q> struct _alt_hashf_fl { inline s_long hash(const Q& x) const { if (double(x) >= 0 && x <= meta::s_long_mp && Q(s_long(x)) == x) { return hashx_common::kf_basic<s_long>().hash(s_long(x)); } return hashx_common::kf_basic<Q>().hash(x); } };

      template<class K, class _2 = meta::nothing, class _3 = meta::nothing> struct _alt_hashf : hashx_common::kf_basic<K> { };

        template<class _2, class _3> struct _alt_hashf<bool, _2, _3> :  _alt_hashf_int<bool> {};
        template<class _2, class _3> struct _alt_hashf<char, _2, _3> :  _alt_hashf_int<char> {};
        template<class _2, class _3> struct _alt_hashf<wchar_t, _2, _3> :  _alt_hashf_int<wchar_t> {};
        template<class _2, class _3> struct _alt_hashf<signed char, _2, _3> :  _alt_hashf_int<signed char> {};
        template<class _2, class _3> struct _alt_hashf<unsigned char, _2, _3> :  _alt_hashf_int<unsigned char> {};
        template<class _2, class _3> struct _alt_hashf<signed short int, _2, _3> :  _alt_hashf_int<signed short int> {};
        template<class _2, class _3> struct _alt_hashf<unsigned short int, _2, _3> :  _alt_hashf_int<unsigned short int> {};
        template<class _2, class _3> struct _alt_hashf<signed int, _2, _3> :  _alt_hashf_int<signed int> {};
        template<class _2, class _3> struct _alt_hashf<unsigned int, _2, _3> :  _alt_hashf_int<unsigned int> {};
        template<class _2, class _3> struct _alt_hashf<signed long int, _2, _3> :  _alt_hashf_int<signed long int> {};
        template<class _2, class _3> struct _alt_hashf<unsigned long int, _2, _3> :  _alt_hashf_int<unsigned long int> {};
        template<class _2, class _3> struct _alt_hashf<signed long long int, _2, _3> :  _alt_hashf_int<signed long long int> {};
        template<class _2, class _3> struct _alt_hashf<unsigned long long int, _2, _3> :  _alt_hashf_int<unsigned long long int> {};

        template<class _2, class _3> struct _alt_hashf<float, _2, _3> :  _alt_hashf_fl<float> {};
        template<class _2, class _3> struct _alt_hashf<double, _2, _3> :  _alt_hashf_fl<double> {};
        template<class _2, class _3> struct _alt_hashf<long double, _2, _3> :  _alt_hashf_fl<long double> {};

        template<class _2, class _3> struct _alt_hashf<excgen, _2, _3> { inline s_long hash(const excgen& x) const { return hashx_common::kf_basic<s_long>().hash(s_long(x.a)) ^ hashx_common::kf_basic<s_long>().hash(s_long(x.b)) ; } };

        template<class _2, class _3> struct _alt_hashf<vecm, _2, _3>
        {
          inline s_long hash(const vecm& x) const
          {
            if (meta::same_t<_2, s_long>::result && x.n() == 1) { return hashx_common::kf_basic<s_long>().hash(x.pval_first<s_long>()); }
            s_long h(0); for (s_long ind = x.nbase(); ind < x.nbase() + x.n(); ++ind) { _2* p = x.pval<_2>(ind); if (p) { h ^= _alt_hashf<_2>().hash(*p); } }
            return h;
          }
        };
        template<class _3> struct _alt_hashf<vecm, meta::nothing, _3> {};

        template<class HT1, class HT2, class HT3, class _2, class _3> struct _alt_hashf<hashx<HT1, HT2, HT3>, _2, _3>
        {
          typedef hashx<HT1, HT2, HT3> t_k;
          typedef typename t_k::t_k t1;
          typedef typename t_k::t_v t2;
          inline s_long hash(const t_k& x) const
          {
            s_long h(0); for (s_long i = 0; i < x.n(); ++i) { h ^= _alt_hashf<t1, _2>().hash(x(i)->k); h ^= _alt_hashf<t2, _3>().hash(x(i)->v); }
            return h;
          }
        };
    };

      // K == vecm: _2 is elem. type.
      // K == hashx: _2 is elem. type when hash key is vecm, _3 is elem. type when hash value is vecm.
    template<class K, class _2 = meta::nothing, class _3 = meta::nothing>
    struct _alt_kf : __hfns::template _alt_hashf<K, _2, _3>
    {
      typedef _conv_sl<K, _2> t_conv;
      typedef typename __hfns::template _alt_hashf<K, _2, _3> base;

      inline void cnew(K* p, const K& x) const { new (p) K(x); }
      inline s_long hash(const K& x) const { return this->base::hash(x); }
      inline bool is_eq(const K& x1, const K& x2) const { return _eq<K, _2, _3>::F(x1, x2); }

      inline void cnew(K* p, s_long x) const { cnew(p, t_conv::F(x)); }
      inline s_long hash(s_long x) const { return hash(t_conv::F(x)); }
      inline bool is_eq(const K& x1, s_long x2) const { return is_eq(x1, t_conv::F(x2)); }
    };

    template<class _2, class _3> struct _alt_kf<s_long, _2, _3> : hashx_common::kf_basic<s_long> { };

    struct _alt_kf_excgen: __hfns::template _alt_hashf<excgen, meta::nothing, meta::nothing>
    {
      typedef excgen K; typedef meta::nothing _2; typedef meta::nothing _3;
      typedef _conv_sl<K, _2> t_conv;
      typedef typename __hfns::template _alt_hashf<K, _2, _3> base;

      inline void cnew(K* p, const K& x) const { new (p) K(x); }
      inline s_long hash(const K& x) const { return this->base::hash(x); }
      inline bool is_eq(const K& x1, const K& x2) const { return _eq<K, _2, _3>::F(x1, x2); }

        // x == -1 causes exception in cnew.
        // x == -2 -"- hashf.
        // x2 == -3 -"- is_eq.
      struct excgen_kf1 {}; struct excgen_kf2 {}; struct excgen_kf3 {};
      inline void cnew(K* p, s_long x) const { if (x == -1) { throw excgen_kf1(); } cnew(p, t_conv::F(x)); }
      inline s_long hash(s_long x) const { if (x == -2) { throw excgen_kf2(); } return hash(t_conv::F(x)); }
      inline bool is_eq(const K& x1, s_long x2) const { if (x2 == -3) { throw excgen_kf3(); } return is_eq(x1, t_conv::F(x2)); }

        // Only for compiling non-executing branches of tests.
      template<class _4, class _5> inline void cnew(_4* p, const _5& x) const { }
      template<class _4> inline s_long hash(const _4& x) const { return 0; }
      template<class _4, class _5> inline bool is_eq(const _4& x1, const _5& x2) const { return false; }
    };

      // Main hashx validity test.
      //    nrsv, integrity.
      //    insert*, remove*, hashx_clear.
      //  nmax >= 0 is exact max. number of entries on hash growing.
      //    nmax < 0 is not valid.
      //  ntests must be >= 1.
      //  Special type arg. values:
      //    KA == vecm: KA2 is elem. type.
      //    VA == vecm: VA2 is elem. type.
      //    KA == itself hashx of one or two vecm: KA2, KA3 are elem. type of the corresp. vecm.
      //  NOTE any excgen as key or value is treated in special in comp. to types.
      //    (Exception generation is taken into account and additionally tested.)
    template<class KA, class VA, class KA2 = meta::nothing, class VA2 = meta::nothing, class KA3 = meta::nothing> struct test_hashx_t
    {
      static int F(logger& log, s_long nmax, s_long ntests)
      {
        log.d, "test_hashx_t::F: n", nmax, "nt", ntests, typeid(KA).name(), typeid(VA).name()
          , (meta::same_t<KA2, meta::nothing>::result ? "-" : typeid(KA2).name())
          , (meta::same_t<VA2, meta::nothing>::result ? "-" : typeid(VA2).name())
          , (meta::same_t<KA3, meta::nothing>::result ? "-" : typeid(KA3).name())
        ; log.flush();

        typedef KA t_ka;
        typedef VA t_va;
        typedef typename vecm::specf<KA>::t_value t_k;
        typedef typename vecm::specf<VA>::t_value t_v;
        typedef typename vecm::specf<KA2>::t_value t_k2;
        typedef typename vecm::specf<VA2>::t_value t_v2;
        typedef typename vecm::specf<KA3>::t_value t_k3;
        typedef typename meta::same_t<t_v, vecm, meta::construct_f<vecm, t_v2>, t_va>::t t_vaf;
        typedef hashx<t_ka, t_vaf, _alt_kf<t_k, t_k2, t_k3> > t_h;
        typedef typename t_h::f_kf f_kf;
          enum { __check = meta::assert<meta::same_t<f_kf, _alt_kf<t_k, t_k2, t_k3> >::result>::result };
        typedef typename t_h::entry entry;
        typedef vecm::specf<entry> cfg_entry;

        t_h h;
        f_kf kf;

        if (h.integrity() < 0) { return -1; }
        if (nmax < 0) { return -2; }
        if (ntests <= 0) { return -3; }
        if (static_cast<t_h*>(0)->integrity() != -3) { return -4; }
        if (h.integrity() != 1) { return -5; }

        if (nmax == 0) { return 1; }

        enum { NF = 10 };
        for (s_long q = 0; q < ntests;)
        {
          bool __skip = false;
          bool __cancelled = false;
          bool __size_unchanged = false;
          s_long m = 0;
          s_long n = 0;
          s_long __nr = h.nrsv();
          s_long __n = h.n();
          if (__n < 0) { return _retval(q, -100); }
          double p_grow = 1.; if (__n >= s_long(0.9 * nmax)) { p_grow = 0.5; } if (__n == nmax) { p_grow = 0.; }
          double p_shrink = 1.; if (__n < s_long(0.1 * nmax)) { p_shrink = 0.5; } if (__n == 0) { p_shrink = 0.; }
          s_long ind_mf = rnd() * NF;
          switch (ind_mf)
          {
            case 0: // hashx_clear
            {
              if (rnd() >= p_shrink) { __skip = true; break; }
              if (h.nexc() != 0) { return _retval(q, -150); }
              s_long nx = 0;
              s_long _ei2 = -1;
              if (meta::same_t<t_k, excgen>::result && h.n() >= 2) { nx += 2; _echo<t_k, excgen>::F(h(0)->k).setf_xd(true); _ei2 = 1 + (h.n() - 1) * rnd(); _echo<t_k, excgen>::F(h(_ei2)->k).setf_xd(true); }
              if (meta::same_t<t_v, excgen>::result && h.n() >= 2) { if (nx == 0) { nx += 1; } _echo<t_v, excgen>::F(h(0)->v).setf_xd(true); s_long _ei3 = 1 + (h.n() - 1) * rnd(); if (_ei3 != _ei2) { nx += 1; } _echo<t_v, excgen>::F(h(_ei3)->v).setf_xd(true); }
              m = h.hashx_clear();
                if (m != nx) { return _retval(q, -151); }
                if (h.n() != 0) { return _retval(q, -153); }
                if (!h.can_shrink()) { return _retval(q, -140); }
                __nr = __hashx_x::_nrsv_n(0);
              if (h.nexc() != 0) { return _retval(q, -152); }
              if (rnd() < 0.5) { h.hashx_setf_can_shrink(!h.can_shrink()); }
              if (rnd() < 0.2) { h.hashx_set_reserve((2 * nmax) * rnd(), rnd() < 0.5); __skip = true; }
              break;
            }
            case 1: // insert
            {
              if (rnd() >= p_grow) { __skip = true; break; }
              m = (nmax - __n) * rnd() / 3. + 1; if (rnd() < 0.1) { m = nmax - __n; }
              n = __n + m;
              s_long m2 = 0; s_long m3 = 0;
              bool _allow_exc = rnd() < 0.5;
              while (h.n() < n && m2 < m)
              {
                if (_allow_exc && meta::same_t<t_k, excgen>::result && rnd() < 10 / (m + 20))
                {
                  const entry* e(0); s_long ind(-1);
                  excgen k0(_rnd_sl()); k0.setf_xc(true);
                  const t_k& k = _echo<excgen, t_k>::F(k0);
                  const entry* e0 = h.find(k);
                  s_long res = h.insert(k, &e,&ind);
                  if (res >= 1) { return _retval(q, -160 + res); }
                  if (res == 0)
                  {
                    ++m2;
                    if (!e0) { return _retval(q, -161); }
                    if (e != e0) { return _retval(q, -162); }
                    if (h.n() != __n + m3) { return _retval(q, -165); }
                    if (_eq<t_k, t_k2, t_k3>::F(e->k, k) != 1) { return _retval(q, -163); }
                    if (e->h != kf.hash(k)) { return _retval(q, -164); }
                  }
                  else if (res == -1)
                  {
                    if (h.n() != __n + m3) { return _retval(q, -166); }
                    __cancelled = true;
                  }
                  continue;
                }
                const entry* e(0); s_long ind(-1);
                t_k k(_rnd_val<t_k, t_k2>::F());

                const entry* e0 = h.find(k);
                s_long res = h.insert(k, &e,&ind);

                  if (res > 1) { return _retval(q, -170); }
                  if (res < 0) { return _retval(q, -171); }
                  if (res == 0)
                  {
                    ++m2;
                    if (!e0) { return _retval(q, -172); }
                    if (e != e0 || e != h(ind)) { return _retval(q, -173); }
                    if (h.n() != __n + m3) { return _retval(q, -174); }
                    if (_eq<t_k, t_k2, t_k3>::F(e->k, k) != 1) { return _retval(q, -175); }
                    if (e->h != h.pkf()->hash(k)) { return _retval(q, -176); }
                  }
                  else // res == 1
                  {
                    ++m3;
                    if (h.n() != __n + m3) { return _retval(q, -177); }
                    if (e0) { return _retval(q, -181); }
                    __test_common_aux1::storage_t<t_v, t_v2> v(1);
                    if (!e || e != h(ind)) { return _retval(q, -178); }
                    if (ind != h.n() - 1) { return _retval(q, -179); }
                    if (_eq<t_k, t_k2, t_k3>::F(e->k, k) != 1) { return _retval(q, -180); }
                    if (!meta::same_t<t_v, excgen>::result && _eq<t_v, t_v2>::F(e->v, v) != 1) { return _retval(q, -182); }
                    if (e->h != h.pkf()->hash(k)) { return _retval(q, -183); }
                  }
              }
              if (h.n() == __n) { __size_unchanged = true; }
              break;
            }
            case 2: // insert2
            {
              if (rnd() >= p_grow) { __skip = true; break; }
              m = (nmax - __n) * rnd() / 3. + 1; if (rnd() < 0.1) { m = nmax - __n; }
              n = __n + m;
              s_long m2 = 0; s_long m3 = 0;
              bool _allow_exc = rnd() < 0.5;
              while (h.n() < n && m2 < m)
              {
                if (_allow_exc && meta::same_t<t_k, excgen>::result && rnd() < 10 / (m + 20))
                {
                  const entry* e(0); s_long ind(-1);
                  _alt_kf_excgen kf_x;
                  s_long k2(_rnd_sl() * 3 - 3); // -3..-1
                  const entry* e0(0); s_long ind0(-10);

                  bool _b_ext_h = rnd() < 0.5 && k2 == -1;
                  s_long _ext_h = h.pkf()->hash(k2);
                  s_long res = h.find2(k2, kf_x, _b_ext_h ? &_ext_h : 0, &e0, &ind0);
                    if (_ext_h && !(res == 0 || res == 1)) { return _retval(q, -201); }
                      else { if (res != -2) { return _retval(q, -202); } }
                    if (res == 1 && (!e0 || ind0 < 0)) { return _retval(q, -203); }
                    if (res == 0 && (e0 || ind0 >= 0)) { return _retval(q, -204); }
                    e0 = 0; ind0 = -10;
                  res = h.find2(k2, *h.pkf(), 0, &e0, &ind0);
                    if (!(res == 1 || res == 0)) { return _retval(q, -205); }
                    if (res == 1 && (!e0 || ind0 < 0)) { return _retval(q, -206); }
                    if (res == 0 && (e0 || ind0 >= 0)) { return _retval(q, -207); }
                  res = h.insert2(k2, kf_x, _b_ext_h ? &_ext_h : 0, &e, &ind);
                    if (_ext_h && !(res == 0 || res == 1)) { return _retval(q, -208); }
                      else { if (res != -2) { return _retval(q, -209); } __cancelled = true; }
                    if (res == 1)
                    {
                      ++m3;
                      if (!_b_ext_h) { return _retval(q, -210); }
                      if (!e || ind < 0) { return _retval(q, -211); }
                      if (h.n() != __n + m3) { return _retval(q, -212); }
                    }
                    else if (res == 0)
                    {
                      ++m2;
                      if (!_b_ext_h) { return _retval(q, -213); }
                      if (!e || ind < 0) { return _retval(q, -214); }
                      if (h.n() != __n + m3) { return _retval(q, -215); }
                    }
                    else if (res == -2)
                    {
                      if (_b_ext_h) { return _retval(q, -216); }
                      if (h.n() != __n + m3) { return _retval(q, -217); }
                    }
                    else { return _retval(q, -218); }
                  continue;
                }
                const entry* e(0); s_long ind(-1);
                const entry* e0(0); s_long ind0(-10);
                s_long k2(_rnd_sl());

                s_long res = h.find2(k2, *h.pkf(), 0, &e0, &ind0);
                    if (res == 1 && (!e0 || ind0 < 0)) { return _retval(q, -232); }
                    if (res == 0 && (e0 || ind0 >= 0)) { return _retval(q, -233); }

                res = h.insert2(k2, *h.pkf(), 0, &e, &ind);
                  if (res > 1) { return _retval(q, -220); }
                  if (res < 0) { return _retval(q, -221); }
                  if (res == 0)
                  {
                    ++m2;
                    if (!e0) { return _retval(q, -220); }
                    if (e != e0 || e != h(ind)) { return _retval(q, -223); }
                    if (h.n() != __n + m3) { return _retval(q, -224); }
                    if (_eq<t_k, t_k2, t_k3>::F(e->k, _conv_sl<t_k, t_k2>::F(k2)) != 1) { return _retval(q, -225); }
                    if (e->h != h.pkf()->hash(k2)) { return _retval(q, -226); }
                  }
                  else // res == 1
                  {
                    ++m3;
                    if (h.n() != __n + m3) { return _retval(q, -227); }
                    if (e0) { return _retval(q, -228); }
                    __test_common_aux1::storage_t<t_v, t_v2> v(1);
                    if (!e || e != h(ind)) { return _retval(q, -229); }
                    if (ind != h.n() - 1) { return _retval(q, -230); }
                    if (_eq<t_k, t_k2, t_k3>::F(e->k, _conv_sl<t_k, t_k2>::F(k2)) != 1) { return _retval(q, -231); }
                    if (!meta::same_t<t_v, excgen>::result && _eq<t_v, t_v2>::F(e->v, v) != 1) { return _retval(q, -234); }
                    if (e->h != h.pkf()->hash(k2)) { return _retval(q, -235); }
                  }
              }
              if (h.n() == __n) { __size_unchanged = true; }
              break;
            }
            case 3: // operator [ ]
            {
              if (rnd() >= p_grow) { __skip = true; break; }
              m = (nmax - __n) * rnd() / 3. + 1; if (rnd() < 0.1) { m = nmax - __n; }
              n = __n + m;
              s_long m2 = 0; s_long m3 = 0;
              bool _allow_exc = rnd() < 0.5;
              while (h.n() < n && m2 < m)
              {
                if (_allow_exc && meta::same_t<t_k, excgen>::result && rnd() < 10 / (m + 20))
                {
                  const t_v* pv(0);
                  excgen k0(_rnd_sl()); k0.setf_xc(true);
                  const t_k& k = _echo<excgen, t_k>::F(k0);
                  const entry* e0 = h.find(k);
                  try { pv = &h[k]; }
                    catch (hashx_common::EHashOpExc)
                    {
                      if (h.n() != __n + m3) { return _retval(q, -252); }
                      __cancelled = true;
                      continue;
                    }
                    catch (...) { return _retval(q, -251); }
                    ++m2;
                    if (!e0) { return _retval(q, -253); }
                    if (pv != &e0->v) { return _retval(q, -254); }
                    if (h.n() != __n + m3) { return _retval(q, -255); }
                    if (_eq<t_k, t_k2, t_k3>::F(e0->k, k) != 1) { return _retval(q, -256); }
                    if (e0->h != kf.hash(k)) { return _retval(q, -257); }
                  continue;
                }
                t_v* pv(0);
                t_k k(_rnd_val<t_k, t_k2>::F());
                t_v v(_rnd_val<t_v, t_v2>::F());
                const entry* e0 = h.find(k);

                  try { pv = &h[k]; }
                    catch (...) { return _retval(q, -260); }
                  if (e0)
                  {
                    ++m2;
                    if (h.n() != __n + m3) { return _retval(q, -261); }
                    if (pv != &e0->v) { return _retval(q, -262); }
                    if (_eq<t_k, t_k2, t_k3>::F(e0->k, k) != 1) { return _retval(q, -264); }
                    if (e0->h != h.pkf()->hash(k)) { return _retval(q, -263); }
                  }
                  else
                  {
                    ++m3;
                    if (h.n() != __n + m3) { return _retval(q, -265); }
                    __test_common_aux1::storage_t<t_v, t_v2> v(1);
                    if (!meta::same_t<t_v, excgen>::result && _eq<t_v, t_v2>::F(*pv, v) != 1) { return _retval(q, -266); }
                  }
                  try { _assign<t_v, t_v2>::F(*pv, v); }
                    catch (...) { return _retval(q, -267); }
                  if (_eq<t_v, t_v2>::F(*pv, v) != 1) { return _retval(q, -268); }
              }
              if (h.n() == __n) { __size_unchanged = true; }
              break;
            }
            case 4: // remove
            {
              if (rnd() >= p_shrink) { __skip = true; break; }
              m = __n * rnd() / 3. + 1; if (rnd() < 0.1) { m = __n; }
              n = __n - m;
              s_long m2 = 0; s_long m3 = 0;
              bool _allow_exc = rnd() < 0.5;
              while (h.n() > n && m2 < m)
              {
                if (rnd() < 0.3) // remove random value
                {
                  t_k k(_rnd_val<t_k, t_k2>::F());
                  const entry* e0 = h.find(k);
                  s_long res = h.remove(k);
                  const entry* e1 = h.find(k);
                    if (!((res == 1 && e0) || (res == 0 && !e0))) { return _retval(q, -280); }
                    if (e1) { return _retval(q, -281); }
                    if (res == 1) { ++m3; } else { ++m2; }
                      if (h.n() != __n - m3) { return _retval(q, -282); }
                }
                else if (rnd() < 0.5 || h.n() == 1) // remove the last value
                {
                  const entry* e0 = h(h.n() - 1);
                    if (!e0) { return _retval(q, -300); }
                    t_k k(e0->k);
                  if (_allow_exc && meta::same_t<t_k, excgen>::result && rnd() < 10 / (m + 20))
                  {
                    if (h.nexc() != 0) { return _retval(q, -301); }
                    _echo<t_k, excgen>::F(e0->k).setf_xd(true);
                    s_long res = h.remove(k);
                    const entry* e1 = h.find(k);
                      if (res != 1) { return _retval(q, -302); }
                      if (e1) { return _retval(q, -303); }
                      if (h.nexc() != 1) { return _retval(q, -304); }
                      h.hashx_set0_nexc();
                      ++m3;
                      if (h.n() != __n - m3) { return _retval(q, -305); }
                  }
                  else
                  {
                    s_long res = h.remove(k);
                    const entry* e1 = h.find(k);
                      if (res != 1) { return _retval(q, -290); }
                      if (e1) { return _retval(q, -291); }
                      ++m3;
                        if (h.n() != __n - m3) { return _retval(q, -292); }
                  }
                }
                else // remove a non-last value
                {
                  s_long ix = (h.n() - 1) * rnd();
                  const entry* ex = h(ix);
                    if (!ex) { return _retval(q, -310); }
                    if (h.nexc() != 0) { return _retval(q, -311); }
                  const entry* e0 = h(h.n() - 1);
                    if (!e0) { return _retval(q, -312); }
                    t_k k(e0->k);
                    t_k kx(ex->k);

                  if (_allow_exc && meta::same_t<t_k, excgen>::result && rnd() < 10 / (m + 20))
                  {
                    bool b = rnd() < 0.5;
                    _echo<t_k, excgen>::F(ex->k).setf_xd(true);
                    if (b) { _echo<t_k, excgen>::F(e0->k).setf_xc(true); }
                    s_long res = h.remove(kx);
                      if (h(ix) != ex) { return _retval(q, -317); }
                    const entry* ex1 = h.find(kx);
                      if (b && cfg_entry::mmode != 3)
                      {
                        if (res != -1) { return _retval(q, -319); }
                        if (h(h.n()-1) != e0) { return _retval(q, -317); }
                        if (!ex1) { return _retval(q, -313); }
                        if (h.nexc() != 0) { return _retval(q, -320); }
                        if (ex1 != ex) { return _retval(q, -314); }
                        ++m2;
                        if (h.n() != __n - m3) { return _retval(q, -315); }
                        if (_eq<t_k, t_k2, t_k3>::F(ex1->k, kx) != 1) { return _retval(q, -316); }
                        if (_eq<t_k, t_k2, t_k3>::F(e0->k, k) != 1) { return _retval(q, -318); }
                        _echo<t_k, excgen>::F(ex->k).setf_xd(false);
                        _echo<t_k, excgen>::F(e0->k).setf_xc(false);
                      }
                      else
                      {
                        if (res != 1) { return _retval(q, -330); }
                        if (ex1) { return _retval(q, -332); }
                        if (h.nexc() != 1) { return _retval(q, -331); }
                        h.hashx_set0_nexc();
                        if (h.find(k) != ex) { return _retval(q, -334); }
                        if (cfg_entry::mmode == 3)
                        {
                          if (_echo<t_k, excgen>::F(ex->k).f_xc()) { return _retval(q, -345); }
                          _echo<t_k, excgen>::F(ex->k).setf_xc(false);
                        }
                        ++m3;
                        if (h.n() != __n - m3) { return _retval(q, -333); }
                      }
                  }
                  else
                  {
                    s_long res = h.remove(kx);
                    const entry* ex1 = h.find(kx);
                      if (res != 1) { return _retval(q, -340); }
                      if (ex1) { return _retval(q, -342); }
                      if (h.nexc() != 0) { return _retval(q, -341); }
                      if (h.find(k) != ex) { return _retval(q, -343); }
                      ++m3;
                      if (h.n() != __n - m3) { return _retval(q, -344); }
                  }
                }
              }
              if (h.n() == __n) { __size_unchanged = true; }
              break;
            }
            case 5: // remove2
            {
              if (rnd() >= p_shrink) { __skip = true; break; }
              if (rnd() >= p_shrink) { __skip = true; break; }
              m = __n * rnd() / 3. + 1; if (rnd() < 0.1) { m = __n; }
              n = __n - m;
              s_long m2 = 0; s_long m3 = 0;
              bool _allow_exc = rnd() < 0.5;
              while (h.n() > n && m2 < m)
              {
                if (_allow_exc && meta::same_t<t_k, excgen>::result && rnd() < 10 / (m + 20))
                {
                  s_long k0 = _rnd_sl();
                  const entry* e0(0); s_long ind0(hashx_common::no_elem - 1);
                  s_long res = h.find2(k0, *h.pkf(), 0, &e0, &ind0);
                    if (!((res == 1 && e0 && ind0 >= 0) || (res == 0 && !e0 && ind0 == hashx_common::no_elem))) { return _retval(q, -370); }

                  _echo<t_k, excgen>::F(e0->k).setf_xd(true);
                  if (rnd() < 0.5)
                  {
                    _alt_kf_excgen kf_x; s_long _ext_h = kf_x.hash(k0);
                    res = h.remove2(k0, kf_x, &_ext_h);
                  }
                  else
                  {
                    res = h.remove2(k0, *h.pkf());
                  }
                    if (res == 1) { ++m3; } else { ++m2; }
                    if (h.n() != __n - m3) { return _retval(q, -371); }

                    if (h.nexc() != 1) { return _retval(q, -372); }
                    h.hashx_set0_nexc();

                  const entry* e1(0); s_long ind1(hashx_common::no_elem - 1);
                    res = h.find2(k0, *h.pkf(), 0, &e1, &ind1);
                      if (!(res == 0 && !e1 && ind1 == hashx_common::no_elem)) { return _retval(q, -373); }
                }
                else // remove random value
                {
                  s_long k0 = _rnd_sl();
                  const entry* e0(0); s_long ind0(hashx_common::no_elem - 1);
                  s_long res = h.find2(k0, *h.pkf(), 0, &e0, &ind0);
                    if (!((res == 1 && e0 && ind0 >= 0) || (res == 0 && !e0 && ind0 == hashx_common::no_elem))) { return _retval(q, -360); }
                  if (rnd() < 0.5)
                  {
                    f_kf kf_x; s_long _ext_h = kf_x.hash(k0);
                    res = h.remove2(k0, kf_x, &_ext_h);
                  }
                  else
                  {
                    res = h.remove2(k0, *h.pkf());
                  }
                    if (res == 1) { ++m3; } else { ++m2; }
                    if (h.n() != __n - m3) { return _retval(q, -362); }
                  const entry* e1(0); s_long ind1(hashx_common::no_elem - 1);
                    res = h.find2(k0, *h.pkf(), 0, &e1, &ind1);
                      if (!(res == 0 && !e1 && ind1 == hashx_common::no_elem)) { return _retval(q, -363); }
                }
              }
              if (h.n() == __n) { __size_unchanged = true; }
              break;
            }
            case 6: // remove_i
            {
              if (rnd() >= p_shrink) { __skip = true; break; }
              m = __n * rnd() / 3. + 1; if (rnd() < 0.1) { m = __n; }
              n = __n - m;
              s_long m2 = 0; s_long m3 = 0;
              bool _allow_exc = rnd() < 0.5;
              while (h.n() > n && m2 < m)
              {
                if (rnd() < 0.1) // remove the non-existing value
                {
                  s_long ind0 = rnd() < 0.5 ? h.n() + (meta::s_long_mp - h.n()) * rnd() : -1 - meta::s_long_mp * rnd();
                  if (h.remove_i(ind0) != 0) { return _retval(q, -380); }
                  ++m2;
                }
                else if (rnd() < 0.2 || h.n() == 1) // remove the last value
                {
                  s_long ind0 = h.n() - 1;
                  const entry* e0 = h(ind0);
                    if (!e0) { return _retval(q, -381); }
                    t_k k(e0->k);
                  if (_allow_exc && meta::same_t<t_k, excgen>::result && rnd() < 10 / (m + 20))
                  {
                    if (h.nexc() != 0) { return _retval(q, -382); }
                    _echo<t_k, excgen>::F(e0->k).setf_xd(true);
                    s_long res = h.remove_i(ind0);
                    const entry* e1 = h.find(k);
                      if (res != 1) { return _retval(q, -383); }
                      if (e1) { return _retval(q, -384); }
                      if (h.nexc() != 1) { return _retval(q, -385); }
                      h.hashx_set0_nexc();
                      ++m3;
                      if (h.n() != __n - m3) { return _retval(q, -386); }
                  }
                  else
                  {
                    s_long res = h.remove_i(ind0);
                    const entry* e1 = h.find(k);
                      if (res != 1) { return _retval(q, -387); }
                      if (e1) { return _retval(q, -388); }
                      ++m3;
                        if (h.n() != __n - m3) { return _retval(q, -389); }
                  }
                }
                else // remove a non-last value
                {
                  s_long ix = (h.n() - 1) * rnd();
                  const entry* ex = h(ix);
                    if (!ex) { return _retval(q, -390); }
                    if (h.nexc() != 0) { return _retval(q, -391); }
                  const entry* e0 = h(h.n() - 1);
                    if (!e0) { return _retval(q, -392); }
                    t_k k(e0->k);
                    t_k kx(ex->k);

                  if (_allow_exc && meta::same_t<t_k, excgen>::result && rnd() < 10 / (m + 20))
                  {
                    bool b = rnd() < 0.5;
                    _echo<t_k, excgen>::F(ex->k).setf_xd(true);
                    if (b) { _echo<t_k, excgen>::F(e0->k).setf_xc(true); }
                    s_long res = h.remove_i(ix);
                      if (h(ix) != ex) { return _retval(q, -393); }
                    const entry* ex1 = h.find(kx);
                      if (b && cfg_entry::mmode != 3)
                      {
                        if (res != -1) { return _retval(q, -394); }
                        if (h(h.n()-1) != e0) { return _retval(q, -395); }
                        if (!ex1) { return _retval(q, -396); }
                        if (h.nexc() != 0) { return _retval(q, -397); }
                        if (ex1 != ex) { return _retval(q, -398); }
                        ++m2;
                        if (h.n() != __n - m3) { return _retval(q, -399); }
                        if (_eq<t_k, t_k2, t_k3>::F(ex1->k, kx) != 1) { return _retval(q, -400); }
                        if (_eq<t_k, t_k2, t_k3>::F(e0->k, k) != 1) { return _retval(q, -401); }
                        _echo<t_k, excgen>::F(ex->k).setf_xd(false);
                        _echo<t_k, excgen>::F(e0->k).setf_xc(false);
                      }
                      else
                      {
                        if (res != 1) { return _retval(q, -402); }
                        if (ex1) { return _retval(q, -403); }
                        if (h.nexc() != 1) { return _retval(q, -404); }
                        h.hashx_set0_nexc();
                        if (h.find(k) != ex) { return _retval(q, -405); }
                        if (cfg_entry::mmode == 3)
                        {
                          if (_echo<t_k, excgen>::F(ex->k).f_xc()) { return _retval(q, -412); }
                          _echo<t_k, excgen>::F(ex->k).setf_xc(false);
                        }
                        ++m3;
                        if (h.n() != __n - m3) { return _retval(q, -406); }
                      }
                  }
                  else
                  {
                    s_long res = h.remove_i(ix);
                    const entry* ex1 = h.find(kx);
                      if (res != 1) { return _retval(q, -407); }
                      if (ex1) { return _retval(q, -408); }
                      if (h.nexc() != 0) { return _retval(q, -409); }
                      if (h.find(k) != ex) { return _retval(q, -410); }
                      ++m3;
                      if (h.n() != __n - m3) { return _retval(q, -411); }
                  }
                }
              }
              if (h.n() == __n) { __size_unchanged = true; }
              break;
            }
            case 7: // hashx_copy
            {
              t_h h2;
              if (h2.hashx_copy(h, q & 1) != 1) { return _retval(q, -600); }
              if (h2.integrity() != 1) { return _retval(q, -601); }
              if (h2.n() != __n) { return _retval(q, -602); }
              if (h2.nexc() != 0) { return _retval(q, -603); }
              if (h2.can_shrink() != true) { return _retval(q, -604); }
              for (s_long i = 0; i < __n; ++i)
              {
                if (!h(i) || !h2(i)) { return _retval(q, -605); }
                if (_eq<t_k, t_k2, t_k3>::F(h2(i)->k, h(i)->k) != 1) { return _retval(q, -606); }
                if (_eq<t_v, t_v2>::F(h2(i)->v, h(i)->v) != 1) { return _retval(q, -607); }
              }
              __size_unchanged = true;
              break;
            }
            case 8: // hashx copy constructor
            {
              t_h h2(h);
              if (h2.n() != __n) { return _retval(q, -622); }
              if (h2.nexc() != 0) { return _retval(q, -623); }
              if (h2.integrity() != 1) { return _retval(q, -621); }
              if (h2.can_shrink() != true) { return _retval(q, -624); }
              for (s_long i = 0; i < __n; ++i)
              {
                if (!h(i) || !h2(i)) { return _retval(q, -625); }
                if (_eq<t_k, t_k2, t_k3>::F(h2(i)->k, h(i)->k) != 1) { return _retval(q, -626); }
                if (_eq<t_v, t_v2>::F(h2(i)->v, h(i)->v) != 1) { return _retval(q, -627); }
              }
              __size_unchanged = true;
              break;
            }
            case 9: // remove_all
            {
              if (h.nexc() != 0) { return _retval(q, -630); }
              s_long nx = 0;
              s_long _ei2 = -1;
              bool __can_shrink = h.can_shrink();
              const void* __pkf = h.pkf();
              void* __pctf = h.pctf();
              if (meta::same_t<t_k, excgen>::result && h.n() >= 2) { nx += 2; _echo<t_k, excgen>::F(h(0)->k).setf_xd(true); _ei2 = 1 + (h.n() - 1) * rnd(); _echo<t_k, excgen>::F(h(_ei2)->k).setf_xd(true); }
              if (meta::same_t<t_v, excgen>::result && h.n() >= 2) { if (nx == 0) { nx += 1; } _echo<t_v, excgen>::F(h(0)->v).setf_xd(true); s_long _ei3 = 1 + (h.n() - 1) * rnd(); if (_ei3 != _ei2) { nx += 1; } _echo<t_v, excgen>::F(h(_ei3)->v).setf_xd(true); }
              m = h.remove_all();
                if (m != __n) { return _retval(q, -636); }
                if (h.n() != 0) { return _retval(q, -632); }
                if (h.nexc() != nx) { return _retval(q, -631); }
                  h.hashx_set0_nexc();
                if (h.can_shrink() != __can_shrink) { return _retval(q, -633); }
                if (h.can_shrink() && h.nrsv() != __hashx_x::_nrsv_n(h.n())) { return _retval(q, -634); }
                if (!h.can_shrink() && h.nrsv() != __nr) { return _retval(q, -638); }
                if (m == 0) { __skip = true; }  // skip standard reserve check
                if (h.pkf() != __pkf) { return _retval(q, -635); }
                if (h.pctf() != __pctf) { return _retval(q, -637); }
              if (rnd() < 0.2) { h.hashx_setf_can_shrink(!h.can_shrink()); __skip = true; }
              if (rnd() < 0.2) { h.hashx_set_reserve((2 * nmax) * rnd(), rnd() < 0.5); __skip = true; }
              break;
            }
            default: { return -9; }
          }
          if (__skip) { continue; }

          if (h.integrity() < 0) { return _retval(q, -880 - ind_mf); }
          s_long nr_normal = __hashx_x::_nrsv_n(h.n());
          if (h.nrsv() < nr_normal)
          {
            __hashx_x2<t_h>& hx = *static_cast<__hashx_x2<t_h>*>(&h);
            s_long nrht2 = hx.nrsv_ht() * __hashx_x::k_max - 1;
//log.d, h.n(), h.nrsv(), nr_normal, hx.nrsv_ht(), hx.nrsv_elems(), nrht2;
            if (nrht2 < nr_normal)
            {
              s_long ndelay = nrht2 + 2 + (hx.nrsv_ht() * __hashx_x::k_grow - hx.nrsv_ht()) / __hashx_x::delta_ht;
              if (h.n() >= ndelay) { return _retval(q, -900 - ind_mf); }
            }
          }
          if (!__cancelled)
          {
            if (h.n() < __n)
            {
              if (h.can_shrink())
              {
                if (h.nrsv() > nr_normal)
                {
                  __hashx_x2<t_h>& hx = *static_cast<__hashx_x2<t_h>*>(&h);
                  s_long nrht2 = hx.nrsv_ht() * __hashx_x::k_max - 1;
                  if (nrht2 <= h.nrsv()) { return _retval(q, -910 - ind_mf); }
                }
              }
              else { if (h.nrsv() != __nr) { return _retval(q, -920 - ind_mf); } }
            }
            else if (h.n() > __n)
            {
              if (h.nrsv() != __nr && h.nrsv() > nr_normal)
              {
                __hashx_x2<t_h>& hx = *static_cast<__hashx_x2<t_h>*>(&h);
                s_long nrht2 = hx.nrsv_ht() * __hashx_x::k_max - 1;
                if (nrht2 <= h.nrsv()) { return _retval(q, -930 - ind_mf); }
              }
            }
            else
            {
              if (!__size_unchanged) { return _retval(q, -940 - ind_mf); }
              if (h.nrsv() != __nr) { return _retval(q, -950 - ind_mf); }
            }
          }
          if (h.integrity() < 0) { return _retval(q, -960 - ind_mf); }

          ++q;
        }
        return 1;
      }
    };

      // Very basic tests for vector_t functions and iterators.
      //  NOTE The test doesn't work for excgen_nt.
    template<class T>
    struct test_vector_t_t
    {
      static int F_ops(logger& log)
      {
        log.d, "test_vector_t_t::F_ops", typeid(T).name();

        vector_t<T> x;

        typedef typename vecm::specf<T>::t_value t_value;
        typedef typename vector_t<T>::exc_vector_t t_exc;
        typedef typename vector_t<T>::iterator iterator;
        typedef typename vector_t<T>::const_iterator citerator;
        enum { N = 5, B = -1 }; // N must be >= 5
        bool b(false);
        const vector_t<T>& cx(x);

        if (x.size() != 0) { return -1; }
        if (x.ptd()->t_ind != bytes::type_index_t<t_value>::ind()) { return -10; }
        if (x.ptd()->ta_ind != bytes::type_index_t<T>::ind()) { return -11; }
        x.vecm_set_nbase(B);
        if (x.nbase() != B) { return -2; }
        try { for (s_long i = 1; i <= N; ++i) { x.push_back(_conv_sl<t_value>::F(i)); } } catch (...) { return -7; }
        if (x.size() != N) { return -3; }
        try {
          if (_eq<t_value>::F(x[0], _conv_sl<t_value>::F(1)) != 1) { return -4; }
          if (_eq<t_value>::F(x[N-1], _conv_sl<t_value>::F(N)) != 1) { return -5; }
          if (_eq<t_value>::F(cx[0], _conv_sl<t_value>::F(1)) != 1) { return -28; }
          if (_eq<t_value>::F(cx[N-1], _conv_sl<t_value>::F(N)) != 1) { return -29; }
          if (_eq<t_value>::F(x[0], x.front()) != 1) { return -30; }
          if (_eq<t_value>::F(x[N-1], x.back()) != 1) { return -31; }
          if (_eq<t_value>::F(cx[0], cx.front()) != 1) { return -32; }
          if (_eq<t_value>::F(cx[N-1], cx.back()) != 1) { return -33; }
        } catch (...) { return -8; }

        typedef typename meta::if_t<meta::same_t<t_value, excgen>::result, _echo<vector_t<T>, vector_t<T> >, _echo<vector_t<T>, vector_t<excgen> > >::result VX;

        if (meta::same_t<t_value, excgen>::result)
        {
          excgen v; v.setf_xc(true);
          b = false;
          try { VX::F(x).push_back(v); } catch (t_exc) { b = true; } catch (...) {} if (!b) { return -12; }
          if (x.size() != N) { return -13; }
        }

        try
        {
          vector_t<T> x2(x);
          if (x2.size() != N) { return -15; }
          if (_eq<t_value>::F(x2[0], _conv_sl<t_value>::F(1)) != 1) { return -16; }
          if (_eq<t_value>::F(x2[N-1], _conv_sl<t_value>::F(N)) != 1) { return -17; }
          if (x2.nbase() != x.nbase()) { return -18; }
        } catch (...) { return -14; }

        try
        {
          vector_t<T> x2; x2 = x;
          if (x2.size() != N) { return -19; }
          if (_eq<t_value>::F(x2[0], _conv_sl<t_value>::F(1)) != 1) { return -20; }
          if (_eq<t_value>::F(x2[N-1], _conv_sl<t_value>::F(N)) != 1) { return -21; }
          if (x2.nbase() != x.nbase()) { return -22; }
        } catch (...) { return -23; }

        if (meta::same_t<t_value, excgen>::result)
        {
          VX::F(x)[N-1].setf_xc(true);

          b = false;
          if (true) { try { vector_t<T> x2(x); } catch (t_exc) { b = true; } catch (...) {} if (!b) { return -24; } }
          b = false;
          if (true) { vector_t<T> x2; try { x2 = x; } catch (t_exc) { b = true; if (x2.size() != 0) { return -27; } if (x2.nexc() != 1) { return -35; } } catch (...) {} if (!b) { return -25; } }

          VX::F(x)[N-1].setf_xc(false);
          VX::F(x)[N-1].setf_xd(true);

          try { x.pop_back(); } catch (...) { return -38; }
          if (x.size() != N - 1) { return -36; }
          if (x.nexc() != 1) { return -37; }
          x.vecm_set0_nexc();
          try
          {
            excgen v(N);
            VX::F(x).push_back(v);
            if (x.size() != N) { return -39; }
            if (_eq<t_value>::F(cx.back(), _conv_sl<t_value>::F(N)) != 1) { return -40; }
          } catch (...) { return -41; }
        }

        enum { M = N - 2 };

        try { x.pop_back(); } catch (...) { return -42; } if (x.size() != M + 1) { return -43; }
        try { x.pop_back(); } catch (...) { return -44; } if (x.size() != M) { return -45; }
        try
        {
          if (_eq<t_value>::F(x[0], _conv_sl<t_value>::F(1)) != 1) { return -46; }
          if (_eq<t_value>::F(x[M-1], _conv_sl<t_value>::F(M)) != 1) { return -47; }
        } catch (...) { return -48; }

        if (true)
        {
          vector_t<T> xe;
          vector_t<T>& cxe(xe);
          b = false;
          try { xe.front(); } catch (t_exc) { b = true; } catch (...) {} if (!b) { return -50; }
          b = false;
          try { xe.back(); } catch (t_exc) { b = true; } catch (...) {} if (!b) { return -51; }
          b = false;
          try { xe.pop_back(); } catch (t_exc) { b = true; } catch (...) {} if (!b) { return -52; }
          b = false;
          try { cxe.front(); } catch (t_exc) { b = true; } catch (...) {} if (!b) { return -54; }
          b = false;
          try { cxe.back(); } catch (t_exc) { b = true; } catch (...) {} if (!b) { return -55; }
        }

        iterator i1 = x.begin();
        iterator i2 = x.end();
        citerator i3 = cx.begin();
        citerator i4 = cx.end();
        if (!i1.is_valid() || !i2.is_valid() || !i3.is_valid() || !i4.is_valid()) { return -56; }
        if (i1.is_out() || !i2.is_out() || i3.is_out() || !i4.is_out()) { return -57; }
        if (_eq<t_value>::F(*i1, *i3) != 1) { return -58; }
        if (_eq<t_value>::F(*i1, _conv_sl<t_value>::F(1)) != 1) { return -59; }
        ++i1; ++i3; --i2; --i4;
        if (i1.is_out() || i2.is_out() || i3.is_out() || i4.is_out()) { return -60; }
          if (i1.is_bbeg() || i2.is_bbeg() || i3.is_bbeg() || i4.is_bbeg()) { return -65; }
          if (i1.is_aend() || i2.is_aend() || i3.is_aend() || i4.is_aend()) { return -66; }
        if (i1 >= i2) { return -69; }
        if (i3 >= i4) { return -70; }
        if (i1 > i2) { return -71; }
        if (i3 > i4) { return -72; }
        if (i1 == i2) { return -73; }
        if (i3 == i4) { return -74; }
        if (i2 <= i1) { return -75; }
        if (i4 <= i3) { return -76; }
        if (i2 < i1) { return -77; }
        if (i4 < i3) { return -78; }
        if (_eq<t_value>::F(*i1, _conv_sl<t_value>::F(2)) != 1) { return -61; }
        if (_eq<t_value>::F(*i2, _conv_sl<t_value>::F(3)) != 1) { return -62; }
        if (_eq<t_value>::F(*i3, _conv_sl<t_value>::F(2)) != 1) { return -63; }
        if (_eq<t_value>::F(*i4, _conv_sl<t_value>::F(3)) != 1) { return -64; }
        i1 += M - 1; i3 += M - 1; i2 -= M; i4 -= M;
        if (!i1.is_out() || !i2.is_out() || !i3.is_out() || !i4.is_out()) { return -67; }
          if (!i1.is_aend() || !i2.is_bbeg() || !i3.is_aend() || !i4.is_bbeg()) { return -68; }
        i1 = i1 - M;
        i3 = i3 - M;
        i2 = i2 + 1;
        i4 = i4 + 1;
        if (i1.ind0() != 0 || i2.ind0() != 0 || i3.ind0() != 0 || i4.ind0() != 0) { return -79; }
          if (i1.ind() != B || i2.ind() != B || i3.ind() != B || i4.ind() != B) { return -80; }
        if (i1 != i2) { return -81; }
        if (i3 != i4) { return -82; }
        if (_eq<t_value>::F(i3[1], _conv_sl<t_value>::F(2)) != 1) { return -91; }
        i1 = 1 + i1;
        i3 = 1 + i3 ;
        i2 = i1++;
        i4 = i3++;
        if (i1.ind0() != 2 || i2.ind0() != 1 || i3.ind0() != 2 || i4.ind0() != 1) { return -83; }
        i2 = i1--;
        i4 = i3--;
        if (i1.ind0() != 1 || i2.ind0() != 2 || i3.ind0() != 1 || i4.ind0() != 2) { return -84; }
        if (i1.ind() != 1 + B || i2.ind() != 2 + B || i3.ind() != 1 + B || i4.ind() != 2 + B) { return -87; }
        if (i2 - i1 != 1) { return -85; }
        if (i3 - i4 != -1) { return -86; }
        if (_eq<t_value>::F(*i1, _conv_sl<t_value>::F(2)) != 1) { return -88; }
        if (_eq<t_value>::F(*i2, _conv_sl<t_value>::F(3)) != 1) { return -89; }
        if (_eq<t_value>::F(*i3, _conv_sl<t_value>::F(2)) != 1) { return -90; }
        if (_eq<t_value>::F(*i4, _conv_sl<t_value>::F(3)) != 1) { return -92; }

        return 1;
      }

        // std::sort comparative test for arrays, std::vector and vector_t.
      static int F_sort(logger& log, const s_long N)
      {
        if (N <= 0) { return 0; }
        log.d, "test_vector_t_t::F_sort", N, typeid(T).name();
        double t0(0.); double dt1(0.); double dt2(0.); double dt3(0.);

        std::vector<s_long> v1; vector_t<s_long> v2; std::vector<s_long> v3;
        for (s_long i = 0; i < N; ++i) { v1.push_back(N * rnd()); v2.push_back(v1.back()); v3.push_back(v1.back()); }

        t0 = bmdx::clock_ms(); std::sort(&v3.front(), &v3.back()+1); dt3 = bmdx::clock_ms() - t0;
        t0 = bmdx::clock_ms(); std::sort(v1.begin(), v1.end()); dt1 = bmdx::clock_ms() - t0;
        t0 = bmdx::clock_ms(); std::sort(v2.begin(), v2.end()); dt2 = bmdx::clock_ms() - t0;

        for (s_long i = 0; i < N; ++i)
        {
          if (v2[i] != v1[i]) { log.d, "err. after std::sort: v2[i] != v1[i]; i", i; return -1; }
          if (v3[i] != v1[i]) { log.d, "err. after std::sort: v3[i] != v1[i]; i", i; return -2; }
          if (i > 0 && v1[i] < v1[i-1]) { log.d, "err. after std::sort: v1[i] < v1[i-1]; i", i; return -2; }
        }
        if (true) { double& x = dt3; x = x * 1.e6 / N; if (x > 10.) { x = s_long(x); } }
        if (true) { double& x = dt1; x = x * 1.e6 / N; if (x > 10.) { x = s_long(x); } }
        if (true) { double& x = dt2; x = x * 1.e6 / N; if (x > 10.) { x = s_long(x); } }
        log.d, "  std::sort T* / std::vector / vector_t:" , dt3, dt1, dt2, "ns/1";

        return 1;
      }
    };

    struct test_hashx_add_wrapper
    {
      struct _kf_str : hashx_common::_select_kf<std::string>
      {
        typedef std::string K;
        _kf_str() : _delta(0) {}
        _kf_str(s_long d) : _delta(d) {}
        template<class K2> inline void cnew(K* p, const K2& x) const { new (p) K(x); }
        template<class K2> inline s_long hash(const K2& x) const { return this->operator()(x) + _delta; }
        template<class K2> inline bool is_eq(const K& x1, const K2& x2) const { return this->operator()(x1, x2); }
        private: s_long _delta;
      };

      static int F(logger& log)
      {
        if (true)
        {
          hashx<std::string, meta::construct_f<vecm, double>, _kf_str> h;

          _kf_str kf1(1); _kf_str kf2(2);
          if (!h.hashx_set_kf(kf1)) { return -1; }
          if (h.insert("a") != 1) { return -2; }
          if (h.hashx_set_kf(kf2)) { return -3; }
          if (h.hashx_set_kf(kf1)) { return -4; }
          if (!h.find("a")) { return -5; }
          if (!h.find2("a", kf1)) { return -6; }
          if (h.find2("a", kf2)) { return -7; }

          s_long hf1 = h.find("a")->h;
          if (!h.find2("a", kf2, &hf1)) { return -8; }
          if (h.insert2("a", kf2, &hf1) != 0) { return -9; }

          if (h.insert2("a", kf2) != 1) { return -10; }
          if (h.n() != 2) { return -11; }
          if (h.remove("a") != 1) { return -12; }
          if (h.remove("a") != 0) { return -13; }
          if (h.n() != 1) { return -14; }
          s_long hf2 = kf2.hash("a");
          if (h.remove2("a", kf1, &hf2) != 1) { return -15; }
          if (h.n() != 0) { return -16; }

          if (!h.hashx_set_kf(kf1)) { return -17; }
          if (!h.hashx_set_kf(kf2)) { return -18; }
          if (h.insert("a") != 1) { return -19; }
          if (!h.find("a")) { return -20; }
          if (h.find("a")->h - hf1 != 1) { return -21; }

          h.hashx_clear();
          if (h.n() != 0) { return -22; }
          if (h.insert("a") != 1) { return -23; }
          if (!h.find("a")) { return -24; }
          if (h.find("a")->h - hf1 != -1) { return -25; }
        }

        if (true)
        {
          int n1, n2, n3, n4, n5;
          std::string s1("(S)");
          std::wstring s2(L"(S)");
          if (true)
          {
            hashx<std::string, s_long,  hashx_common::kf_str<std::string> > hs_s;
              n1 = hs_s.insert(s1); n2 = hs_s.insert2("(C*)", *hs_s.pkf()); n3 = hs_s.n(); n4 = int(bool((hs_s.find(s1)))); n5 = hs_s.find2("(C*)", *hs_s.pkf());
              if (!(n1 == 1 && n2 == 1 && n3 == 2 && n4 == 1 && n5 == 1)) { return -30; }
          }
          if (true)
          {
            hashx<const char*, s_long,  hashx_common::kf_str<const char*> > hs_pc;
              n1 = hs_pc.insert2(s1, *hs_pc.pkf()); n2 = hs_pc.insert("(C*)"); n3 = hs_pc.n(); n4 = hs_pc.find2(s1, *hs_pc.pkf()); n5 = int(bool(hs_pc.find("(C*)")));
              if (!(n1 == 1 && n2 == 1 && n3 == 2 && n4 == 1 && n5 == 1)) { return -40; }
          }
          if (true)
          {
            hashx<std::wstring, s_long,  hashx_common::kf_str<std::wstring> > hs_ws;
              n1 = hs_ws.insert(s2); n2 = hs_ws.insert2(L"(C*)", *hs_ws.pkf()); n3 = hs_ws.n(); n4 = int(bool((hs_ws.find(s2)))); n5 = hs_ws.find2(L"(C*)", *hs_ws.pkf());
              if (!(n1 == 1 && n2 == 1 && n3 == 2 && n4 == 1 && n5 == 1)) { return -50; }
          }
          if (true)
          {
            hashx<const wchar_t*, s_long,  hashx_common::kf_str<const wchar_t*> > hs_pwc;
              n1 = hs_pwc.insert2(s2, *hs_pwc.pkf()); n2 = hs_pwc.insert(L"(C*)"); n3 = hs_pwc.n(); n4 = hs_pwc.find2(s2, *hs_pwc.pkf()); n5 = int(bool(hs_pwc.find(L"(C*)")));
              if (!(n1 == 1 && n2 == 1 && n3 == 2 && n4 == 1 && n5 == 1)) { return -60; }
          }
        }

        if (true)
        {
          enum { N = 1000, Q = 1000 };
          vecm vr(typer<s_long>, 0);
          for (s_long i = 0; i < N; ++ i)
          {
            if (!vr.el_append(s_long(N * rnd()))) { return -103; }
          }
          for (s_long i = N; i <= 10000000; i *= 3)
          {
            if (!vr.el_append(i)) { return -104; }
          }
          hashx<std::string, double> h;
          for (s_long i = 0; i < vr.n(); ++ i)
          {
            s_long n = *vr.pval_0u<s_long>(i); bool b = rnd() < 0.5;
            s_long __nr = h.nrsv();
            if (!h.hashx_set_reserve(n, b)) { return _retval(i, -100); }
            if (h.nrsv() < n) { return _retval(i, -103); }
            if (n < __nr && !b)
            {
              if (h.nrsv() != __nr) { return _retval(i, -101); }
            }
            else
            {
              if (h.nrsv() != __hashx_x::_nrsv_n(n)) { return _retval(i, -102); }
            }
          }
        }
        return 1;
      }
    };

    struct test_is_eq_str_wrapper
    {
      static int F(logger& log)
      {
        std::string& stxt = txtsample(); s_long M = 900000; s_long Mn = 120; if ( ! (s_long(stxt.size()) > M + 2*Mn + 0x10000) ) { log.d, "err. test_is_eq_str_wrapper::F: invalid dictionary file (req.:", c_txtsample_filename(), ">= 1 MB)."; return -1; }

        enum { N = 100000, L = 25 };
        s_long x[L];
        s_long pos0 = M * rnd();
        s_long pos;
        s_long size = Mn * rnd();
        for (int i = 0; i < L; ++i) { x[i] = 0; }
        for (int i = 0; i < N; ++i)
        {
          if (i & 0xffff) { pos0 = M * rnd(); size = Mn * rnd(); }
          pos = pos0 + (i & 0xffff);
          std::string s = stxt.substr(pos, size);
          std::string s2 = stxt.substr(pos, size);
          std::string s3 = stxt.substr(pos, size+1);

          x[0] += bytes::is_eq_str(&s[0], &s2[0], -1, -2);
          x[1] += bytes::is_eq_str(&s[0], &s2[0], -2, -1);
          x[2] += bytes::is_eq_str(&s[0], &s2[0], -1, -1);

          x[3] += bytes::is_eq_str(&s[0], &s2[0], size, -1);
          x[4] += bytes::is_eq_str(&s[0], &s2[0], -1, size);
          x[5] += bytes::is_eq_str(&s[0], &s2[0], size, size);

          x[6] += bytes::is_eq_str(&s[0], &s3[0], -1, size);
          x[7] += bytes::is_eq_str(&s3[0], &s[0], size, -1);

          x[8] += bytes::is_eq_str(&s[0], &s3[0], size, size);

          x[9] += !bytes::is_eq_str<char>(&s[0], 0, -1, -1);
          x[10] += !bytes::is_eq_str<char>(&s[0], 0, -1, i);
          x[11] += !bytes::is_eq_str<char>(&s[0], 0, i, -1);
          x[12] += !bytes::is_eq_str<char>(&s[0], 0, i, i);

          x[13] += !bytes::is_eq_str<char>(0, &s[0], -1, -1);
          x[14] += !bytes::is_eq_str<char>(0, &s[0], -1, i);
          x[15] += !bytes::is_eq_str<char>(0, &s[0], i, -1);
          x[16] += !bytes::is_eq_str<char>(0, &s[0], i, i);

          x[17] += !bytes::is_eq_str<char>(0, 0, -1, -1);
          x[18] += !bytes::is_eq_str<char>(0, 0, -1, i);
          x[19] += !bytes::is_eq_str<char>(0, 0, i, -1);
          x[20] += !bytes::is_eq_str<char>(0, 0, i, i);

          x[21] += !bytes::is_eq_str(&s[0], &s3[0], -1, -1);
          x[22] += !bytes::is_eq_str(&s3[0], &s[0], -1, -1);
          x[23] += !bytes::is_eq_str(&s[0], &s3[0], size, -1);
          x[24] += !bytes::is_eq_str(&s3[0], &s[0], -1, size);
        }
        for (int i = 0; i < L; ++i) { if (x[i] != N) { return _retval(N - x[i], -i); } }
        return 1;
      }
    };

    int perf_timer()
    {
      rnd_0() = rnd_00();
      log, "Factual std::clock resolution - peak, median (ms): ", __timergap(msclk, 1000, true), ", ", __timergap(msclk, 1000, false), endl;
      return 1;
    }

    int test_is_eq_str()
    {
      rnd_0() = rnd_00();
      return test_is_eq_str_wrapper::F(log);
    }

    int test_cmti()
    {
      log.d, "int: type ind., size, is crossm.", bytes::type_index_t<int>::ind(), sizeof(int), bytes::type_index_t<int>::ind() < 0;
      log.d, "char*: type ind., size, is crossm.", bytes::type_index_t<char*>::ind(), sizeof (char*), bytes::type_index_t<char*>::ind() < 0;
      log.d, "smptr: type ind., size, is crossm.", bytes::type_index_t<smptr>::ind(), sizeof(smptr), bytes::type_index_t<smptr>::ind() < 0;
      log.d, "std::string: type ind., size, is crossm.", bytes::type_index_t<std::string>::ind(), sizeof(std::string), bytes::type_index_t<std::string>::ind() < 0;
        struct _column { mutable char* pd; s_long kbase; inline void f() { pd = 0; kbase = 0; } private: _column() {} ~_column() {} };
      log.d, "vecm: type ind., vecm / td. / col. size, is crossm.", bytes::type_index_t<vecm>::ind(), sizeof(vecm), "/", sizeof(vecm::type_descriptor), "/", sizeof(_column), bytes::type_index_t<vecm>::ind() < 0;
      return 1;
    }

      // vecm validity tests for simpler types / normal conditions.
    int test_vecm_a()
    {
      rnd_0() = rnd_00();
      s_long res = 0; s_long i_t = 0;
      enum { N = 1000, Q = 10000, N2 = 300000, Q2 = 1000, N3 = 1000, Q3 = 1000 };
      do
      {
          // passing a null pointer
        if (true) { ++i_t; res = test_vecm_t<s_long, meta::nothing>::F(log, 0, 1, 1); if (res < 0) { break; } }

          // char
        log, endl;
        if (true) { ++i_t; typedef char T; vecm v(typer<T>, 0); res = test_vecm_t<T, meta::nothing>::F(log, &v, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef char T; vecm v(typer<T>, -1); res = test_vecm_t<T, meta::nothing>::F(log, &v, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef char T; vecm v(typer<T>, meta::s_long_mp); res = test_vecm_t<T, meta::nothing>::F(log, &v, N2, Q2); if (res < 0) { break; } }

          // other types
        log, endl;
        if (true) { ++i_t; typedef s_long T; vecm v(typer<T>, 0); res = test_vecm_t<T, meta::nothing>::F(log, &v, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef double T; vecm v(typer<T>, 0); res = test_vecm_t<T, meta::nothing>::F(log, &v, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef signed long long int T; vecm v(typer<T>, 0); res = test_vecm_t<T, meta::nothing>::F(log, &v, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef long double T; vecm v(typer<T>, 0); res = test_vecm_t<T, meta::nothing>::F(log, &v, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef std::string T; vecm v(typer<T>, 0); res = test_vecm_t<T, meta::nothing>::F(log, &v, N, Q / 2); if (res < 0) { break; } }
        if (true) { ++i_t; typedef S3 T; vecm v(typer<T>, 0); res = test_vecm_t<T, meta::nothing>::F(log, &v, N, Q); if (res < 0) { break; } }
      } while(false);
      if (res < 0) { log, "err. test_vecm_t[", i_t, "]", endl; log.flush(); return res; }
      return 1;
    }

      // vecm validity tests for complex and exception-generating classes.
      //    NOTE This test disrupts system memory manager (allocation slows down).
      //    After it, any perf. measurements are inadequate.
    int test_vecm_b()
    {
      rnd_0() = rnd_00();
      s_long res = 0; s_long i_t = 0;
      enum { N3 = 600, Q3 = 1000 };
      do
      {
          // exception generator as elem.
        if (true) { ++i_t; typedef excgen T; vecm v(typer<T>, 0); res = test_vecm_t<T, meta::nothing>::F(log, &v, N3, Q3); if (res < 0) { break; } }
        if (true) { ++i_t; typedef excgen_nt T; vecm v(typer<T>, 0); res = test_vecm_t<T, meta::nothing>::F(log, &v, N3, Q3); if (res < 0) { break; } }
        if (true) { ++i_t; typedef excgen_mmode3 T; vecm v(typer<T>, 0); res = test_vecm_t<T, meta::nothing>::F(log, &v, N3, Q3); if (res < 0) { break; } }

          // vecm(various types) as elem.
        log, endl;
        if (true) { ++i_t; typedef char T2; vecm v(typer<vecm>, 0); res = test_vecm_t<vecm, T2>::F(log, &v, N3, Q3); if (res < 0) { break; } }
        if (true) { ++i_t; typedef s_long T2; vecm v(typer<vecm>, 0); res = test_vecm_t<vecm, T2>::F(log, &v, N3, Q3); if (res < 0) { break; } }
        if (true) { ++i_t; typedef std::string T2; vecm v(typer<vecm>, 0); res = test_vecm_t<vecm, T2>::F(log, &v, N3, Q3); if (res < 0) { break; } }
        if (true) { ++i_t; typedef hashx<char, s_long> T2; vecm v(typer<vecm>, 0); res = test_vecm_t<vecm, T2>::F(log, &v, N3, Q3); if (res < 0) { break; } }
        if (true) { ++i_t; typedef hashx<s_long, s_long> T2; vecm v(typer<vecm>, 0); res = test_vecm_t<vecm, T2>::F(log, &v, N3, Q3); if (res < 0) { break; } }
        if (true) { ++i_t; typedef hashx<std::string, s_long> T2; vecm v(typer<vecm>, 0); res = test_vecm_t<vecm, T2>::F(log, &v, N3, Q3); if (res < 0) { break; } }
        if (true) { ++i_t; typedef hashx<std::string*, s_long> T2; vecm v(typer<vecm>, 0); res = test_vecm_t<vecm, T2>::F(log, &v, N3, Q3); if (res < 0) { break; } }
      } while(false);
      if (res < 0) { log, "err. test_vecm_t[", i_t, "]", endl; log.flush(); return res; }
      return 1;
    }

      // hashx validity tests for simpler types / normal conditions.
    int test_hashx_a()
    {
      rnd_0() = rnd_00();
      s_long res = 0; s_long i_t = 0;
      enum { N = 1000, Q = 1000 };
      do
      {
          // Factual test.
        if (full_test())
        {
          if (true) { ++i_t; typedef char K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
          if (true) { ++i_t; typedef bool K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
          if (true) { ++i_t; typedef wchar_t K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
          if (true) { ++i_t; typedef signed char K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
          if (true) { ++i_t; typedef unsigned char K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
          if (true) { ++i_t; typedef signed short int K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
          if (true) { ++i_t; typedef unsigned short int K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
          if (true) { ++i_t; typedef signed int K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
          if (true) { ++i_t; typedef unsigned int K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
          if (true) { ++i_t; typedef signed long int K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
          if (true) { ++i_t; typedef unsigned long int K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
          if (true) { ++i_t; typedef signed long long int K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
          if (true) { ++i_t; typedef unsigned long long int K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
          if (true) { ++i_t; typedef float K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
          if (true) { ++i_t; typedef double K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
          if (true) { ++i_t; typedef long double K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
          if (true) { ++i_t; typedef void* K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
          if (true) { ++i_t; typedef std::string K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
        }
        else
        {
          if (true) { ++i_t; typedef char K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
          if (true) { ++i_t; typedef bool K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
          if (true) { ++i_t; typedef signed long int K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
          if (!meta::same_t<s_long, signed long int>::result) { ++i_t; typedef s_long K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
          if (true) { ++i_t; typedef signed long long int K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
          if (true) { ++i_t; typedef float K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
          if (true) { ++i_t; typedef double K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
          if (true) { ++i_t; typedef long double K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
          if (true) { ++i_t; typedef void* K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
          if (true) { ++i_t; typedef std::string K; typedef s_long V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
        }
      } while(false);
      if (res < 0) { log, "err. test_hashx_t[", i_t, "]", endl; log.flush(); return res; }
      return 1;
    }

      // hashx validity tests for complex and exception-generating classes.
    int test_hashx_b()
    {
      rnd_0() = rnd_00();
      s_long res = 0; s_long i_t = 0;
      enum { N = 600, Q = 500, Q2 = 250 };
      do
      {
        log, endl;
        if (true) { ++i_t; typedef char K; typedef std::string V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef s_long K; typedef std::string V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef double K; typedef std::string V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef signed long long int K; typedef std::string V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef long double K; typedef std::string V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef std::string K; typedef std::string V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }

        log, endl;
        if (true) { ++i_t; typedef signed int K; typedef vecm V; typedef char V2; res = test_hashx_t<K, V, meta::nothing, V2>::F(log, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef double K; typedef vecm V; typedef char V2; res = test_hashx_t<K, V, meta::nothing, V2>::F(log, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef void* K; typedef vecm V; typedef std::string V2; res = test_hashx_t<K, V, meta::nothing, V2>::F(log, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef std::string K; typedef vecm V; typedef std::string V2; res = test_hashx_t<K, V, meta::nothing, V2>::F(log, N, Q); if (res < 0) { break; } }

        log, endl;
        if (true) { ++i_t; typedef bool K; typedef vecm V; typedef char V2; res = test_hashx_t<K, V, meta::nothing, V2>::F(log, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef char K; typedef vecm V; typedef char V2; res = test_hashx_t<K, V, meta::nothing, V2>::F(log, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef wchar_t K; typedef vecm V; typedef char V2; res = test_hashx_t<K, V, meta::nothing, V2>::F(log, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef signed char K; typedef vecm V; typedef char V2; res = test_hashx_t<K, V, meta::nothing, V2>::F(log, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef unsigned char K; typedef vecm V; typedef char V2; res = test_hashx_t<K, V, meta::nothing, V2>::F(log, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef signed short int K; typedef vecm V; typedef char V2; res = test_hashx_t<K, V, meta::nothing, V2>::F(log, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef unsigned short int K; typedef vecm V; typedef char V2; res = test_hashx_t<K, V, meta::nothing, V2>::F(log, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef unsigned int K; typedef vecm V; typedef char V2; res = test_hashx_t<K, V, meta::nothing, V2>::F(log, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef signed long int K; typedef vecm V; typedef char V2; res = test_hashx_t<K, V, meta::nothing, V2>::F(log, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef unsigned long int K; typedef vecm V; typedef char V2; res = test_hashx_t<K, V, meta::nothing, V2>::F(log, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef signed long long int K; typedef vecm V; typedef char V2; res = test_hashx_t<K, V, meta::nothing, V2>::F(log, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef unsigned long long int K; typedef vecm V; typedef char V2; res = test_hashx_t<K, V, meta::nothing, V2>::F(log, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef float K; typedef vecm V; typedef char V2; res = test_hashx_t<K, V, meta::nothing, V2>::F(log, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef long double K; typedef vecm V; typedef char V2; res = test_hashx_t<K, V, meta::nothing, V2>::F(log, N, Q); if (res < 0) { break; } }

        log, endl;
        if (true) { ++i_t; typedef excgen K; typedef std::string V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef std::string K; typedef excgen V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef excgen K; typedef excgen V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef excgen_mmode3 K; typedef std::string V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef std::string K; typedef excgen_mmode3 V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef excgen_mmode3 K; typedef excgen_mmode3 V; res = test_hashx_t<K, V>::F(log, N, Q); if (res < 0) { break; } }

        log, endl;
        if (true) { ++i_t; typedef char K2; typedef hashx<vecm, s_long, _alt_kf<vecm, K2> > K; typedef std::string V; res = test_hashx_t<K, V, K2>::F(log, N, Q2); if (res < 0) { break; } }
        if (true) { ++i_t; typedef char K2; typedef hashx<vecm, s_long, _alt_kf<vecm, K2> > K; typedef char K2; typedef vecm V; typedef std::string V2; res = test_hashx_t<K, V, K2, V2>::F(log, N, Q2); if (res < 0) { break; } }
        if (true) { ++i_t; typedef char K2; typedef wchar_t K3; typedef hashx<vecm, meta::construct_f<vecm, K3>, _alt_kf<vecm, K2, K3> > K; typedef char K2; typedef vecm V; typedef std::string V2; res = test_hashx_t<K, V, K2, V2, K3>::F(log, N, Q2); if (res < 0) { break; } }
        if (true) { ++i_t; typedef hashx<std::string, s_long> K; typedef vecm V; typedef vecm V2; res = test_hashx_t<K, V, meta::nothing, V2>::F(log, N, Q2); if (res < 0) { break; } }
      } while(false);
      if (res < 0) { log, "err. test_hashx_t[", i_t, "]", endl; log.flush(); return res; }
      return 1;
    }

    int test_hashx_add()
    {
      rnd_0() = rnd_00();
      return test_hashx_add_wrapper::F(log);
    }
      // Tests most of branches in: vecm(const vecm&), vecm_copy, hashx(const hashx&), hashx_copy.
    int test_container_copy()
    {
      rnd_0() = rnd_00();
      typedef container_copy_wrapper W;
        // Example: ind == 57 --> tcode == 4201, means "test hashx(const hashx&) in non-transactional mode, with exceptions in value desturctor only".
      struct _2 { static s_long tcode(s_long ind) { return 1000 * (1 + ((ind / W::F1m) % W::NF1)) + 100 * s_long((ind / W::F2m) % W::NF2) + 10 * s_long((ind / W::F3m) % W::NF3) + 1 * s_long((ind / W::F4m) % W::NF4); } };
      for (s_long ind = 0; ind < W::NF_all; ++ind)
        { s_long res = W::F(log, 1 + ((ind / W::F1m) % W::NF1), (ind / W::F2m) % W::NF2, bool((ind / W::F3m) % W::NF3), bool((ind / W::F4m) % W::NF4)); if (res < 0) { return -1000 * _2::tcode(ind) + res; } }
      return 1;
    }

    int test_vecm_el_insert_1()
    {
      rnd_0() = rnd_00();
      s_long res = 0;

      // Multiple plans for longs, doubles, strings, chars.
      const s_long n_plans = 32;
      const s_long n_plans2 = full_test() ? n_plans : 3;
      // nbase, n_append, n_ind0, n_ins, is_random, x0, verif_end_state_only;
      typename el_ins_wrapper::test_plan plans[n_plans] =
      {
        { -1, 0, 0, 50, false, 10000, false },
        { 2, 1, 0, 50, false, 10000, false },
        { -1, 1, 1, 50, true, 10000, false },
        { 0, 0, 0, 50, false, 10000, false },
        { 0, 1, 0, 50, false, 10000, false },
        { -1, 1, 1, 1600, false, 10000, false },
        { 0, 1, 1, 1600, false, 10000, false },
        { -1, 1, 1, 1600, true, 10000, false },
        { 0, 1, 1, 1600, true, 10000, false },
        { 0, 7, 0, 50, false, 10000, false },
        { 0, 7, 0, 50, true, 10000, false },
        { 0, 7, 1, 50, false, 10000, false },
        { 0, 7, 1, 50, true, 10000, false },
        { 0, 7, 7, 50, false, 10000, false },
        { 0, 7, 7, 50, true, 10000, false },
        { 0, 8, 0, 50, false, 10000, false },
        { 0, 8, 0, 50, true, 10000, false },
        { 0, 8, 1, 50, false, 10000, false },
        { 0, 8, 1, 50, true, 10000, false },
        { 0, 8, 7, 50, false, 10000, false },
        { 0, 8, 7, 50, true, 10000, false },
        { 0, 8, 8, 50, false, 10000, false },
        { 0, 8, 8, 50, true, 10000, false },
        { 0, 9, 8, 50, false, 10000, false },
        { 0, 9, 8, 50, true, 10000, false },
        { -1, 140000, 0, 10, false, 1000000, false },
        { -1, 140000, 1, 10, false, 1000000, false },
        { 9, 140000, 3, 10, false, 1000000, false },
        { 9, 140000, 4, 10, false, 1000000, false },
        { 9, 140000, 7, 10, false, 1000000, false },
        { 9, 140000, 0, 10, true, 1000000, false },
        { -1, 0, 0, 9900, true, 1, true }
      };
      if (true) { typedef char T; typedef typename el_ins_wrapper::template impl<T> t_impl; res = t_impl::do_test_plans(log, plans, n_plans2); if (res < 0) { return res; } }
      if (true) { typedef int T; typedef typename el_ins_wrapper::template impl<T> t_impl; res = t_impl::do_test_plans(log, plans, n_plans2); if (res < 0) { return res; } }
      if (true) { typedef s_long T; typedef typename el_ins_wrapper::template impl<T> t_impl; res = t_impl::do_test_plans(log, plans, n_plans2); if (res < 0) { return res; } }
      if (true) { typedef double T; typedef typename el_ins_wrapper::template impl<T> t_impl; res = t_impl::do_test_plans(log, plans, n_plans2); if (res < 0) { return res; } }
      if (true) { typedef std::string T; typedef typename el_ins_wrapper::template impl<T> t_impl; res = t_impl::do_test_plans(log, plans, n_plans2); if (res < 0) { return res; } }
      return 1;
    }

    int test_vecm_el_remove_1()
    {
      rnd_0() = rnd_00();
      s_long res = 0;

      //  for longs, doubles, strings, chars:
      //    for base 0, -1, 1, 1000000:
      //      remove 1 at 0 until empty
      //      remove 1 at 1 until 1 left
      //      remove 1 at 7 until 7 left
      //      remove 1 at (first in first col. with cap. 256) 512 times
      //      remove 1 at (second in second col. with cap. 256) 300 times
      //      remove 1 at random pos until empty
      //      remove 1 at random pos 10 times from a container with such size that vecm contains at least 2 cols. with cap. 8192
      const s_long n_bases = 4;
      const s_long n_plans = 8;
      const s_long n_bases2 = full_test() ? n_bases : 1;
      const s_long n_plans2 = full_test() ? n_plans : 3;
      s_long bases[n_bases] = { 0, -1, 1, 1000000 };

      // n0 n1 is_random at_ind n_rmv
      typename el_rmv_wrapper::test_plan plans[n_plans] =
      {
        { 1600, 2000, false, 0, -1 },
        { 1600, 2000, false, 1, -1 },
        { 1600, 2000, true, 1, -1 },
        { 1600, 2000, false, 7, -1 },
        { 1600, 2000, false, 1024, 512 },
        { 1600, 2000, false, 1281, 300 },
        { 1600, 2000, true, -1, -1 },
        { 130000, 10000, true, -1, 10 },
      };
      if (true) { typedef char T; typedef typename el_rmv_wrapper::template impl<T> t_impl; res = t_impl::do_test_plans(log, bases, plans, n_bases2, n_plans2); if (res < 0) { return res; } }
      if (true) { typedef int T; typedef typename el_rmv_wrapper::template impl<T> t_impl; res = t_impl::do_test_plans(log, bases, plans, n_bases2, n_plans2); if (res < 0) { return res; } }
      if (true) { typedef s_long T; typedef typename el_rmv_wrapper::template impl<T> t_impl; res = t_impl::do_test_plans(log, bases, plans, n_bases2, n_plans2); if (res < 0) { return res; } }
      if (true) { typedef double T; typedef typename el_rmv_wrapper::template impl<T> t_impl; res = t_impl::do_test_plans(log, bases, plans, n_bases2, n_plans2); if (res < 0) { return res; } }
      if (true)
      {
        const s_long n_bases = 2;
        const s_long n_plans = 7;
        const s_long n_bases2 = full_test() ? n_bases : 1;
        const s_long n_plans2 = full_test() ? n_plans : 3;
        s_long bases[n_bases] = { 0, -1 };
        typename el_rmv_wrapper::test_plan plans[n_plans] =
        {
          { 100, 100, false, 0, -1 },
          { 100, 100, false, 1, -1 },
          { 100, 100, true, 1, -1 },
          { 100, 100, false, 7, -1 },
          { 1600, 2000, false, 1281, 300 },
          { 100, 100, true, -1, -1 },
          { 130000, 10000, true, -1, 10 }
        };
        typedef std::string T; typedef typename el_rmv_wrapper::template impl<T> t_impl; res = t_impl::do_test_plans(log, bases, plans, n_bases2, n_plans2); if (res < 0) { return res; }
      }
      return 1;
    }

      // Simple test for inserting/removing keys from the ordered sequence.
    int test_vecm_ord()
    {
      rnd_0() = rnd_00();

      enum { N = 100 };
      struct __ord : vecm
      {
        __ord() : vecm(typer<s_long>, -1) {}
        void ins(s_long k) { _ord_insert<s_long, s_long, bytes::less_t<s_long> >(k, k, 0, 0, bytes::less_t<s_long>()); }
        void rmv(s_long k) { _ord_remove<s_long, s_long, bytes::less_t<s_long> >(k, bytes::less_t<s_long>()); }
        bool find(const s_long& k) { return 1 == _ord_find<s_long, s_long, bytes::less_t<s_long> >(k, 0, 0, bytes::less_t<s_long>()); }
        bool find(const double& k) { return 1 == _ord_find<s_long, double, bytes::less_t<s_long, double> >(k, 0, 0, bytes::less_t<s_long, double>()); }
      };

      unsigned long delta = 50331653ul;
      unsigned long x;
      __ord v;
      for (s_long n = 0; n < N; ++n)
      {
        v.vecm_clear();
        x = n / 2; for (s_long i = 0; i < n; ++i) { if (v.find(s_long(x))) { return -(n * 10 + 1); }  v.ins(x); if (!v.find(s_long(x))) { return -(n * 10 + 2); } x = (x + delta) % n; }
        if (v.n() != n) { return -3; }
        for (s_long i = 0; i < n; ++i) { if (*v.vecm::template pval_0u<s_long>(i) != i) { return -(n * 10 + 4); } }
        x = n / 2; for (s_long i = 0; i < n; ++i) { if (!v.find(s_long(x))) { return -(n * 10 + 5); } if (!v.find(double(x))) { return -(n * 10 + 6); } v.rmv(x); if (v.find(s_long(x))) { return -(n * 10 + 7); } if (v.find(double(x))) { return -(n * 10 + 8); } x = (x + delta) % n; }
        if (v.n() != 0) { return -9; }
      }
      return 1;
    }

    int test_vector_t()
    {
      rnd_0() = rnd_00();

      s_long res = 0; s_long i_t = 0;
      do
      {
        if (true) { ++i_t; typedef s_long T; res = test_vector_t_t<T>::F_ops(log); if (res < 0) { break; } }
        if (true) { ++i_t; typedef std::string T; res = test_vector_t_t<T>::F_ops(log); if (res < 0) { break; } }
        if (true) { ++i_t; typedef excgen T; res = test_vector_t_t<T>::F_ops(log); if (res < 0) { break; } }
        if (true) { ++i_t; typedef excgen_mmode3 T; res = test_vector_t_t<T>::F_ops(log); if (res < 0) { break; } }
        if (true) { ++i_t; typedef s_long T; res = test_vector_t_t<T>::F_sort(log, 2000000); if (res < 0) { break; } }
      } while(false);
      if (res < 0) { log, "err. test_vector_t[", i_t, "]", endl; log.flush(); return res; }
      return 1;
    }

    int perf_vector_append_1()
    {
      rnd_0() = rnd_00();

      #ifdef __ANDROID__
        const s_long k = 5;
      #else
        const s_long k = full_test() ? 1 : 20;
      #endif

      int res = 0;
      do {
        if (true) { typedef char T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_append_1(log, 100000000 / k, true, false, false, 0, 5); if (res < 0) { break; } }
        if (true) { typedef s_long T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_append_1(log, 50000000 / k, true, false, false, 0, 5); if (res < 0) { break; } }
        if (true) { typedef double T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_append_1(log, 25000000 / k, true, false, false, 0, 5); if (res < 0) { break; } }
        if (true) { typedef std::string T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_append_1(log, 1000000 / k, true, false, false, 0, 2); if (res < 0) { break; } }

        log, endl; log.flush();
        if (true) { typedef char T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_append_1(log, 100000000 / k, false, true, false, 0, 5); if (res < 0) { break; } }
        if (true) { typedef s_long T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_append_1(log, 50000000 / k, false, true, false, 0, 5); if (res < 0) { break; } }
        if (true) { typedef double T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_append_1(log, 25000000 / k, false, true, false, 0, 5); if (res < 0) { break; } }
        if (true) { typedef std::string T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_append_1(log, 1000000 / k, false, true, false, 0, 2); if (res < 0) { break; } }

        log, endl; log.flush();
        if (true) { typedef char T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_append_1(log, 100000000 / k, false, false, true, 0, 5); if (res < 0) { break; } }
        if (true) { typedef s_long T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_append_1(log, 50000000 / k, false, false, true, 0, 5); if (res < 0) { break; } }
        if (true) { typedef double T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_append_1(log, 25000000 / k, false, false, true, 0, 5); if (res < 0) { break; } }
        if (true) { typedef std::string T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_append_1(log, 1000000 / k, false, false, true, 0, 2); if (res < 0) { break; } }

        log, endl; log.flush();
        if (true) { typedef char T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_append_1(log, 100000000 / k, false, false, false, 1, 3); if (res < 0) { break; } }
        if (true) { typedef s_long T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_append_1(log, 50000000 / k, false, false, false, 1, 3); if (res < 0) { break; } }
        if (true) { typedef double T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_append_1(log, 25000000 / k, false, false, false, 1, 3); if (res < 0) { break; } }
        if (true) { typedef std::string T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_append_1(log, 1000000 / k, false, false, false, 1, 1); if (res < 0) { break; } }

        log, endl; log.flush();
        if (true) { typedef char T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_append_1(log, 100000000 / k, false, false, false, 2, 3); if (res < 0) { break; } }
        if (true) { typedef s_long T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_append_1(log, 50000000 / k, false, false, false, 2, 3); if (res < 0) { break; } }
        if (true) { typedef double T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_append_1(log, 25000000 / k, false, false, false, 2, 3); if (res < 0) { break; } }
        if (true) { typedef std::string T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_append_1(log, 1000000 / k, false, false, false, 2, 1); if (res < 0) { break; } }

        log, endl; log.flush();
        if (true) { typedef char T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_append_1(log, 100000000 / k, false, false, false, 3, 3); if (res < 0) { break; } }
        if (true) { typedef s_long T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_append_1(log, 50000000 / k, false, false, false, 3, 3); if (res < 0) { break; } }
        if (true) { typedef double T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_append_1(log, 25000000 / k, false, false, false, 3, 3); if (res < 0) { break; } }
        if (true) { typedef std::string T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_append_1(log, 1000000 / k, false, false, false, 3, 1); if (res < 0) { break; } }
      } while (false); if (res < 0) { return res; }
      return 1;
    }

    int perf_vector_elem_copy()
    {
      rnd_0() = rnd_00();
      #ifdef __ANDROID__
        const s_long k = 5;
      #else
        const s_long k = full_test() ? 1 : 20;
      #endif

      int res = 0;
      do {
        if (true) { typedef char T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_elem_copy(log, 100000000 / k, 1); if (res < 0) { break; } }
        if (true) { typedef s_long T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_elem_copy(log, 50000000 / k, 1); if (res < 0) { break; } }
        if (true) { typedef double T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_elem_copy(log, 25000000 / k, 1); if (res < 0) { break; } }
        if (true) { typedef std::string T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_elem_copy(log, 1000000 / k,  2); if (res < 0) { break; } }
      } while (false); if (res < 0) { return res; }

      return 1;
    }

    int perf_vector_rand_copy()
    {
      rnd_0() = rnd_00();
      const s_long k = full_test() ? 1 : 20;

      int res = 0;
      do {
        if (true) { typedef char T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_rand_copy(log, 10000000 / k, true, false, 1); if (res < 0) { break; } }
        if (true) { typedef s_long T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_rand_copy(log, 5000000 / k, true, false, 1); if (res < 0) { break; } }
        if (true) { typedef double T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_rand_copy(log, 5000000 / k, true, false, 1); if (res < 0) { break; } }
        if (true) { typedef std::string T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_rand_copy(log, 1000000 / k,  true, false, 2); if (res < 0) { break; } }

        log, endl;
        if (true) { typedef char T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_rand_copy(log, 10000000 / k, false, true, 1); if (res < 0) { break; } }
        if (true) { typedef s_long T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_rand_copy(log, 5000000 / k, false, true, 1); if (res < 0) { break; } }
        if (true) { typedef double T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_rand_copy(log, 5000000 / k, false, true, 1); if (res < 0) { break; } }
        if (true) { typedef std::string T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_rand_copy(log, 1000000 / k, false, true, 2); if (res < 0) { break; } }
      } while (false); if (res < 0) { return res; }
      return 1;
    }

    int perf_vector_rnd_insdel_1()
    {
      rnd_0() = rnd_00();
      int res = 0;
      if (full_test())
      {
        const s_long n_ns = 4;
        s_long ns[n_ns] = { 10, 100, 1000, 10000 };
        for (int j = 0; j < n_ns; ++j) { if (true) { typedef char T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_rnd_insdel_1(log, ns[j], 10); if (res < 0) { return res; } } }
          if (true) { typedef char T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_rnd_insdel_1(log, 50000, 1); if (res < 0) { return res; } }
        for (int j = 0; j < n_ns; ++j) { if (true) { typedef s_long T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_rnd_insdel_1(log, ns[j], 10); if (res < 0) { return res; } } }
          if (true) { typedef s_long T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_rnd_insdel_1(log, 50000, 1); if (res < 0) { return res; } }
        for (int j = 0; j < n_ns; ++j) { if (true) { typedef double T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_rnd_insdel_1(log, ns[j], 10); if (res < 0) { return res; } } }
        for (int j = 0; j < n_ns; ++j) { if (true) { typedef std::string T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_rnd_insdel_1(log, ns[j], 2); if (res < 0) { return res; } } }
      }
      else
      {
        const s_long n_ns = 1;
        s_long ns[n_ns] = { 1000 };
        for (int j = 0; j < n_ns; ++j) { if (true) { typedef char T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_rnd_insdel_1(log, ns[j], 5); if (res < 0) { return res; } } }
        for (int j = 0; j < n_ns; ++j) { if (true) { typedef s_long T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_rnd_insdel_1(log, ns[j], 5); if (res < 0) { return res; } } }
        for (int j = 0; j < n_ns; ++j) { if (true) { typedef double T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_rnd_insdel_1(log, ns[j], 5); if (res < 0) { return res; } } }
        for (int j = 0; j < n_ns; ++j) { if (true) { typedef std::string T; typedef typename perf_vector_wrapper::template impl<T> t_impl; res = t_impl::Fperf_rnd_insdel_1(log, ns[j], 1); if (res < 0) { return res; } } }
      }
      return 1;
    }

    int perf_hashx()
    {
      rnd_0() = rnd_00();
      log.d, "Hash insert/find/remove.";
      const char* hname = "hashx";
      s_long res;

      //  NOTE In case of progressive test (is_prog == true)
      //    t per el. = avg( t(10^i objs. * N/10^i els.) ) / N, i: 0..3 but N/10^i >= 1000.
      do {
        if (full_test())
        {
          res = htest_wrapper::template hperf_t<hc_hashx, long, long, 10000>::F(log,  false,  false, (std::string(hname) += "<long, long>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, long, long, 50000>::F(log,  false,  false, (std::string(hname) += "<long, long>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, long, long, 100000>::F(log,  false,  false, (std::string(hname) += "<long, long>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, long, long, 300000>::F(log,  false,  false, (std::string(hname) += "<long, long>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, long, long, 2000000>::F(log,  false,  false, (std::string(hname) += "<long, long>").c_str()); if (res < 0) { break; }

          log, endl;
          res = htest_wrapper::template hperf_t<hc_hashx, long, long, 300000>::F(log,  true,  true, "hashx<long, long> prog+pk"); if (res < 0) { break; }

          log, endl;
          res = htest_wrapper::template hperf_t<hc_hashx, double, double, 10000>::F(log,  false,  false, (std::string(hname) += "<double, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, double, double, 50000>::F(log,  false,  false, (std::string(hname) += "<double, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, double, double, 100000>::F(log,  false,  false, (std::string(hname) += "<double, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, double, double, 300000>::F(log,  false,  false, (std::string(hname) += "<double, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, double, double, 2000000>::F(log,  false,  false, (std::string(hname) += "<double, double>").c_str()); if (res < 0) { break; }

          log, endl;
          res = htest_wrapper::template hperf_t<hc_hashx, double, double, 300000>::F(log,  true,  true, "hashx<double, double> prog+pk"); if (res < 0) { break; }

          log, endl;
          res = htest_wrapper::template hperf_t<hc_hashx, std::string, double, 10000>::F(log,  false,  false, (std::string(hname) += "<string, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, std::string, double, 50000>::F(log,  false,  false, (std::string(hname) += "<string, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, std::string, double, 100000>::F(log,  false,  false, (std::string(hname) += "<string, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, std::string, double, 300000>::F(log,  false,  false, (std::string(hname) += "<string, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, std::string, double, 1000000>::F(log,  false,  false, (std::string(hname) += "<string, double>").c_str()); if (res < 0) { break; }

          log, endl;
          res = htest_wrapper::template hperf_t<hc_hashx, std::string, double, 300000>::F(log,  true,  true, "hashx<string, double> prog+pk"); if (res < 0) { break; }

          log, endl;
          res = htest_wrapper::template hperf_t<hc_hashx, bool, double, 100>::F(log,  false,  false, (std::string(hname) += "<bool, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, char, double, 10000>::F(log,  false,  false, (std::string(hname) += "<char, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, wchar_t, double, 10000>::F(log,  false,  false, (std::string(hname) += "<wchar_t, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, signed char, double, 10000>::F(log,  false,  false, (std::string(hname) += "<signed char, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, unsigned char, double, 10000>::F(log,  false,  false, (std::string(hname) += "<unsigned char, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, signed short int, double, 10000>::F(log,  false,  false, (std::string(hname) += "<signed short int, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, unsigned short int, double, 10000>::F(log,  false,  false, (std::string(hname) += "<unsigned short int, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, signed int, double, 10000>::F(log,  false,  false, (std::string(hname) += "<signed int, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, unsigned int, double, 10000>::F(log,  false,  false, (std::string(hname) += "<unsigned int, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, signed long int, double, 10000>::F(log,  false,  false, (std::string(hname) += "<signed long int, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, unsigned long int, double, 10000>::F(log,  false,  false, (std::string(hname) += "<unsigned long int, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, signed long long int, double, 10000>::F(log,  false,  false, (std::string(hname) += "<signed long long int, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, unsigned long long int, double, 10000>::F(log,  false,  false, (std::string(hname) += "<unsigned long long int, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, float, double, 10000>::F(log,  false,  false, (std::string(hname) += "<float, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, double, double, 10000>::F(log,  false,  false, (std::string(hname) += "<double, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, long double, double, 10000>::F(log,  false,  false, (std::string(hname) += "<long double, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, void*, double, 10000>::F(log,  false,  false, (std::string(hname) += "<void*, double>").c_str()); if (res < 0) { break; }
        }
        else
        {
          res = htest_wrapper::template hperf_t<hc_hashx, long, long, 10000>::F(log,  false,  false, (std::string(hname) += "<long, long>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, long, long, 2000000>::F(log,  false,  false, (std::string(hname) += "<long, long>").c_str()); if (res < 0) { break; }

          log, endl;
          res = htest_wrapper::template hperf_t<hc_hashx, long, long, 300000>::F(log,  false,  true, (std::string(hname) += "<long, long>").c_str()); if (res < 0) { break; }

          log, endl;
          res = htest_wrapper::template hperf_t<hc_hashx, double, double, 10000>::F(log,  false,  false, (std::string(hname) += "<double, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, double, double, 2000000>::F(log,  false,  false, (std::string(hname) += "<double, double>").c_str()); if (res < 0) { break; }

          log, endl;
          res = htest_wrapper::template hperf_t<hc_hashx, std::string, double, 10000>::F(log,  false,  false, (std::string(hname) += "<string, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, std::string, double, 1000000>::F(log,  false,  false, (std::string(hname) += "<string, double>").c_str()); if (res < 0) { break; }

          log, endl;
          res = htest_wrapper::template hperf_t<hc_hashx, bool, double, 100>::F(log,  false,  false, (std::string(hname) += "<bool, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, char, double, 5000>::F(log,  false,  false, (std::string(hname) += "<char, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, int, double, 5000>::F(log,  false,  false, (std::string(hname) += "<int, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, long long, double, 5000>::F(log,  false,  false, (std::string(hname) += "<long long, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, long double, double, 5000>::F(log,  false,  false, (std::string(hname) += "<long double, double>").c_str()); if (res < 0) { break; }
          res = htest_wrapper::template hperf_t<hc_hashx, void*, double, 5000>::F(log,  false,  false, (std::string(hname) += "<void*, double>").c_str()); if (res < 0) { break; }
        }
      } while (false); if (res < 0) { return res; }
      return 1;
    }

    int profile_ht_chains()
    {
      rnd_0() = rnd_00();
      log.d, "Hash table chains histogram. x = len, f(x) = ilog2(count(ht(i)): len(ht(i)) == x).";
      s_long res(1);

      if (full_test())
      {
        res = bytes::min_sl(res, htest_wrapper::template hxprofch_t<long, 10000>::F(log, "long", false));
        res = bytes::min_sl(res, htest_wrapper::template hxprofch_t<long, 2000000>::F(log, "long", false));
        res = bytes::min_sl(res, htest_wrapper::template hxprofch_t<long, 2000000>::F(log, "long", true));

        log, endl;
        res = bytes::min_sl(res, htest_wrapper::template hxprofch_t<double, 10000>::F(log, "double", false));
        res = bytes::min_sl(res, htest_wrapper::template hxprofch_t<double, 2000000>::F(log, "double", false));
        res = bytes::min_sl(res, htest_wrapper::template hxprofch_t<double, 2000000>::F(log, "double", true));

        log, endl;
        res = bytes::min_sl(res, htest_wrapper::template hxprofch_t<std::string, 10000>::F(log, "std::string", false));
        res = bytes::min_sl(res, htest_wrapper::template hxprofch_t<std::string, 1000000>::F(log, "std::string", false));
        res = bytes::min_sl(res, htest_wrapper::template hxprofch_t<std::string, 1000000>::F(log, "std::string", true));
      }
      else
      {
        res = bytes::min_sl(res, htest_wrapper::template hxprofch_t<long, 100000>::F(log, "long", false));
        log, endl;
        res = bytes::min_sl(res, htest_wrapper::template hxprofch_t<double, 100000>::F(log, "double", false));
        log, endl;
        res = bytes::min_sl(res, htest_wrapper::template hxprofch_t<std::string, 100000>::F(log, "std::string", false));
      }
      return res;
    }

    // ===================================================
    // Entry point for full test sequence.
    // ===================================================

    int do_test(bool is_full, const char* prnd0)
    {
      full_test() = is_full;
      rnd_00() = prnd0 ? atol(prnd0) : std::time(0); rnd_0() = rnd_00();

      std::string prevLocaleName; if (1) { const char* p = std::setlocale(LC_CTYPE, ""); if (p) { prevLocaleName = p; } }
      log, nowstr(), " -- ", this->fnp_log, endl;
      log, "rnd0, 00 = ", rnd_00(), endl;
      log, "__cplusplus:",
#ifdef __cplusplus
      __cplusplus,
#else
      "not defined",
#endif
      endl;
      log, "do_test(). NOTE testing may take more than one minute.", endl, endl; log.flush();
      double t0 = bmdx::clock_ms();

#if !defined(bmdx_test_sections) || (bmdx_test_sections & 1)

      stat.exec_test(&test_vecm_hashx::perf_timer, "perf_timer()");

      if (stat.exec_test(&test_vecm_hashx::test_is_eq_str, "test_is_eq_str()") < 0) { goto lExit; }
      if (stat.exec_test(&test_vecm_hashx::test_cmti, "test_cmti()") < 0) { goto lExit; }
      if (stat.exec_test(&test_vecm_hashx::test_vecm_a, "test_vecm_a()") < 0) { goto lExit; }
      if (stat.exec_test(&test_vecm_hashx::test_hashx_a, "test_hashx_a()") < 0) { goto lExit; }

      log, endl, "[1] rnd0 == ", rnd_0(), endl, endl;

      stat.exec_test(&test_vecm_hashx::perf_hashx, "perf_hashx()");

      log, endl, "[2] rnd0 == ", rnd_0(), endl, endl;

      stat.exec_test(&test_vecm_hashx::profile_ht_chains, "profile_ht_chains()");

#endif

#if !defined(bmdx_test_sections) || (bmdx_test_sections & 2)
      if (stat.exec_test(&test_vecm_hashx::test_container_copy, "test_container_copy()") < 0) { goto lExit; }
      if (stat.exec_test(&test_vecm_hashx::test_vecm_el_remove_1, "test_vecm_el_remove_1()") < 0) { goto lExit; }
      if (stat.exec_test(&test_vecm_hashx::test_vecm_el_insert_1, "test_vecm_el_insert_1()") < 0) { goto lExit; }
      if (stat.exec_test(&test_vecm_hashx::test_vecm_ord, "test_vecm_ord()") < 0) { goto lExit; }
      if (stat.exec_test(&test_vecm_hashx::test_vector_t, "test_vector_t()") < 0) { goto lExit; }

      stat.exec_test(&test_vecm_hashx::perf_vector_append_1, "perf_vector_append_1()");
      stat.exec_test(&test_vecm_hashx::perf_vector_elem_copy, "perf_vector_elem_copy()");
      stat.exec_test(&test_vecm_hashx::perf_vector_rand_copy, "perf_vector_rand_copy()");
      stat.exec_test(&test_vecm_hashx::perf_vector_rnd_insdel_1, "perf_vector_rnd_insdel_1()");

#endif

#if !defined(bmdx_test_sections) || (bmdx_test_sections & 16)
      if (stat.exec_test(&test_vecm_hashx::test_vecm_b, "test_vecm_b()") < 0) { goto lExit; }
      if (stat.exec_test(&test_vecm_hashx::test_hashx_b, "test_hashx_b()") < 0) { goto lExit; }
      if (stat.exec_test(&test_vecm_hashx::test_hashx_add, "test_hashx_add()") < 0) { goto lExit; }
#endif

      log, endl, "[3] rnd0 == ", rnd_0(), endl, endl;

      txtsample(true); // clear static text sample

lExit:
      double dt = double(s_long((bmdx::clock_ms() - t0) / 100.)) / 10.; if (dt >= 50.) { dt = double(s_long(dt)); }
      log, "TESTING COMPLETED. Total: on_calm_result ", stat.__ns, ", failure ", stat.__nf, ", skipped ", stat.__ni, ". Taken ", dt, " s.", endl, endl; log.flush();
      if (prevLocaleName.length()) { std::setlocale(LC_CTYPE, prevLocaleName.c_str()); }

      return stat.__nf;
    }
  };
}

#endif
