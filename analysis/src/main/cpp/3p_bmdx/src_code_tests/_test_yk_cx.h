// In this module:
//  tests for vec2_t, link2_t, ordhs_t.

#ifndef _test_yk_cx_H
#define _test_yk_cx_H

#include "../src_code/yk_cx.h"
#include "../src_code/bmdx_cpiomt.h"

#ifndef _test_yk_c_common_H
  #include "_test_yk_c_common.h"
#endif
#ifndef yk_c_experimental_v1_1_H
  #include "yk_cx.h"
#endif

#if defined(__clang__)
#pragma clang diagnostic push
#endif
#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wundefined-bool-conversion"
#endif

namespace yk_tests
{
  using namespace yk_c;
  using namespace experimental;

  static const s_long hc__ordhs = 40;
  template<class K, class V> struct hmeta_t<hc__ordhs, K, V> : _hmeta_base_t<ordhs_t<K, V>, K, V> { };

  template<class T1, class T2, class T3, class T4> struct hsize<ordhs_t<T1, T2, T3, T4> > { static s_long F(const ordhs_t<T1, T2, T3, T4>& x) { return x.n(); } };
  template<class T1, class T2, class T3, class T4> struct hclear<ordhs_t<T1, T2, T3, T4> > { static void F(ordhs_t<T1, T2, T3, T4>& x) { x.ordhs_clear(); } };
  template<class T1, class T2, class T3, class T4, class K> struct hremove<ordhs_t<T1, T2, T3, T4>, K> { static s_long F(ordhs_t<T1, T2, T3, T4>& x, const K& k) { return x.remove(k); } };
  template<class T1, class T2, class T3, class T4, class K, class V> struct hinsert<ordhs_t<T1, T2, T3, T4>, K, V>
  {
    static s_long F(ordhs_t<T1, T2, T3, T4>& x, const K& k, const V& v)
    {
      static s_long cnt(0); ++cnt;
      s_long res(-1);
      if (cnt & 1) { s_long i(-1); res = x.insert(k, 0, &i); if (!x(i)) { return -10; } try { x(i)->v = v; } catch (...) { return -11; } }
      else { s_long i2(-1); res = x.insert(k, 0, 0, &i2); if (!x.h(i2)) { return -12; } try { x.h(i2)->v = v; } catch (...) { return -13; } }
      return res;
    }
  };

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

  enum hash_cat_exp { hc_ordhs = 32 };

  template<class _ = int> struct test_experimental : test_common_t<>
  {
    typedef test_common_t<> base;
    base::stat_t<test_experimental> stat;
    static const char* c_logfile_name() { return "testlog_yk_c_experimental.txt"; } // 1
    test_experimental() : base(c_logfile_name(), true), stat(*this) { rnd_00() = 0; rnd_0() = 0; }
    test_experimental(const char* p_alt_fn = 0, bool log_truncate = true) : base(p_alt_fn ? p_alt_fn : c_logfile_name(), log_truncate), stat(*this) { rnd_00() = 0; rnd_0() = 0; }

    // ===================================================
    // Test-specific functions.
    // ===================================================

    static bool& full_test() { static bool x(true); return x; }
    static s_long& rnd_00() { static s_long x(0); return x; }

      // res == 1: on_calm_result.
      //  0: filename == 0.
      //  -1: failed to open file for input.
      //  -2: any other error (read err., convert err. etc.).
    static std::string load_filestr(const char* filename, bool is_textmode, s_long* res = 0)
    {
      std::string s; if (!filename) { if (res) { *res = 0; } return s; }
      try
      {
        std::fstream fs(filename, std::ios_base::openmode((is_textmode ? 0 : std::ios_base::binary) | std::ios_base::in));
        if (!fs.is_open()) { if (res) { *res = -1; } return s; }
        char fbuf[2048]; fs.rdbuf()->pubsetbuf(fbuf, 2048); std::stringstream ss; ss << fs.rdbuf(); s = ss.str();
        if (res) { *res = -1; } return s;
      }
      catch (...) {}
      if (res) { *res = -2; } return s;
    }

    static std::string& txtsample(bool reset = false) { static std::string x; static bool init(false); if (!init && !reset) { x = load_filestr(c_txtsample_filename(), true); init = true; } if (reset) { x.clear(); init = false; } return x; }

    int test_ordhs()
    {
      rnd_0() = rnd_00();
      log.d, "ordhs_t validity.";
      enum { hc = hc__ordhs };
      const char* hname = "ordhs_t";
      s_long res(0);

      do {
        res = htest_wrapper::htest_any_t<hc, bool, double>::F(log, (std::string(hname) + "<bool, double>").c_str(), 1000, 1000); if (res < 0) { break; }
        res = htest_wrapper::htest_any_t<hc, char, double>::F(log, (std::string(hname) + "<char, double>").c_str(), 1000, 1000); if (res < 0) { break; }
        res = htest_wrapper::htest_any_t<hc, wchar_t, double>::F(log, (std::string(hname) + "<wchar_t, double>").c_str(), 1000, 1000); if (res < 0) { break; }
        res = htest_wrapper::htest_any_t<hc, signed char, double>::F(log, (std::string(hname) + "<signed char, double>").c_str(), 1000, 1000); if (res < 0) { break; }
        res = htest_wrapper::htest_any_t<hc, unsigned char, double>::F(log, (std::string(hname) + "<unsigned char, double>").c_str(), 1000, 1000); if (res < 0) { break; }
        res = htest_wrapper::htest_any_t<hc, signed short int, double>::F(log, (std::string(hname) + "<signed short int, double>").c_str(), 1000, 1000); if (res < 0) { break; }
        res = htest_wrapper::htest_any_t<hc, unsigned short int, double>::F(log, (std::string(hname) + "<unsigned short int, double>").c_str(), 1000, 1000); if (res < 0) { break; }
        res = htest_wrapper::htest_any_t<hc, signed int, double>::F(log, (std::string(hname) + "<signed int, double>").c_str(), 1000, 1000); if (res < 0) { break; }
        res = htest_wrapper::htest_any_t<hc, unsigned int, double>::F(log, (std::string(hname) + "<unsigned int, double>").c_str(), 1000, 1000); if (res < 0) { break; }
        res = htest_wrapper::htest_any_t<hc, signed long int, double>::F(log, (std::string(hname) + "<signed long int, double>").c_str(), 1000, 1000); if (res < 0) { break; }
        res = htest_wrapper::htest_any_t<hc, unsigned long int, double>::F(log, (std::string(hname) + "<unsigned long int, double>").c_str(), 1000, 1000); if (res < 0) { break; }
        res = htest_wrapper::htest_any_t<hc, signed long long int, double>::F(log, (std::string(hname) + "<signed long long int, double>").c_str(), 1000, 1000); if (res < 0) { break; }
        res = htest_wrapper::htest_any_t<hc, unsigned long long int, double>::F(log, (std::string(hname) + "<unsigned long long int, double>").c_str(), 1000, 1000); if (res < 0) { break; }
        res = htest_wrapper::htest_any_t<hc, float, double>::F(log, (std::string(hname) + "<float, double>").c_str(), 1000, 1000); if (res < 0) { break; }
        res = htest_wrapper::htest_any_t<hc, double, double>::F(log, (std::string(hname) + "<double, double>").c_str(), 1000, 1000); if (res < 0) { break; }
        res = htest_wrapper::htest_any_t<hc, long double, double>::F(log, (std::string(hname) + "<long double, double>").c_str(), 1000, 1000); if (res < 0) { break; }
        res = htest_wrapper::htest_any_t<hc, void*, double>::F(log, (std::string(hname) + "<void*, double>").c_str(), 1000, 1000); if (res < 0) { break; }
        res = htest_wrapper::htest_any_t<hc, std::string, double>::F(log, (std::string(hname) + "<string, double>").c_str(), 1000, 1000); if (res < 0) { break; }
      } while (false); if (res < 0) { return res; }
      return 1;
    }

    int perf_ordhs()
    {
      rnd_0() = rnd_00();
      log.d, "ordhs_t insert/find/remove.";
      const char* hname = "ordhs_t";
      s_long res(0);

      do {
        res = htest_wrapper::template hperf_t<hc__ordhs, long, long, 10000>::F(log,  false,  false, (std::string(hname) += "<long, long>").c_str()); if (res < 0) { break; }
        res = htest_wrapper::template hperf_t<hc__ordhs, long, long, 90000>::F(log,  false,  false, (std::string(hname) += "<long, long>").c_str()); if (res < 0) { break; }

        log, endl;
        res = htest_wrapper::template hperf_t<hc__ordhs, long, long, 50000>::F(log,  false,  true, (std::string(hname) += "<long, long>").c_str()); if (res < 0) { break; }

        log, endl;
        res = htest_wrapper::template hperf_t<hc__ordhs, double, double, 10000>::F(log,  false,  false, (std::string(hname) += "<double, double>").c_str()); if (res < 0) { break; }
        res = htest_wrapper::template hperf_t<hc__ordhs, double, double, 90000>::F(log,  false,  false, (std::string(hname) += "<double, double>").c_str()); if (res < 0) { break; }

        log, endl;
        res = htest_wrapper::template hperf_t<hc__ordhs, std::string, double, 10000>::F(log,  false,  false, (std::string(hname) += "<string, double>").c_str()); if (res < 0) { break; }
        res = htest_wrapper::template hperf_t<hc__ordhs, std::string, double, 90000>::F(log,  false,  false, (std::string(hname) += "<string, double>").c_str()); if (res < 0) { break; }

        log, endl;
        res = htest_wrapper::template hperf_t<hc__ordhs, bool, double, 100>::F(log,  false,  false, (std::string(hname) += "<bool, double>").c_str()); if (res < 0) { break; }
        res = htest_wrapper::template hperf_t<hc__ordhs, char, double, 5000>::F(log,  false,  false, (std::string(hname) += "<char, double>").c_str()); if (res < 0) { break; }
        res = htest_wrapper::template hperf_t<hc__ordhs, int, double, 5000>::F(log,  false,  false, (std::string(hname) += "<int, double>").c_str()); if (res < 0) { break; }
        res = htest_wrapper::template hperf_t<hc__ordhs, long long, double, 5000>::F(log,  false,  false, (std::string(hname) += "<long long, double>").c_str()); if (res < 0) { break; }
        res = htest_wrapper::template hperf_t<hc__ordhs, long double, double, 5000>::F(log,  false,  false, (std::string(hname) += "<long double, double>").c_str()); if (res < 0) { break; }
        res = htest_wrapper::template hperf_t<hc__ordhs, void*, double, 5000>::F(log,  false,  false, (std::string(hname) += "<void*, double>").c_str()); if (res < 0) { break; }
      } while (false); if (res < 0) { return res; }
      return 1;
    }



      // vec2_t and link2_t validity test.
      //    integrity, compatibility, locality, ptd, null_state.
      //    el*, link*, clear, null_state.
      //    std vector interface.
      //  nmax > 0 is exact max. number of elements for pv growing.
      //    nmax > 0 but < pv->n() is adjusted to pv->n().
      //    nmax == 0 performs only a small part of tests (section 1).
      //    nmax < 0 is not valid.
      //  ntests must be >= 1.
      //  TA2 specifies elem. type if TA is vecm. Otherwise must be meta::nothing.
      //  NOTE TA may be hashx only if both its key and value are not vecm.
      //  NOTE pv is modified during the test.
      //  NOTE pv == 0 is valid. Some of fns. will be tested.
      //  NOTE vec2_t(any excgen) is treated in special in comp. to types.
      //    (Exception generation is taken into account and additionally tested.)
    template<class TA, class TA2> struct test_vec2_t
    {
      typedef typename vecm::specf<TA>::t_value T;
      typedef typename vecm::specf<TA2>::t_value T2;


      struct linktest_ctx
      {
        enum { duration_ms = 500, timeout_ms = 4000, mmax = 100, n_threads1 = 30 };
        vec2_t<TA>* pv;
        s_long n_linked_sync;
        s_long n_finished_sync;
        s_long ind_stage_sync; // 0 - not started, 1 - test 1 started, 2 - test 2 started, 3 - finished
        s_long cnt_err1; // link creation failure
        s_long cnt_err2; // link is lost
        s_long cnt_err3; // linked value has changed
        s_long cnt_err4; // lock failure
        s_long cnt_err5; // container modification failure
        s_long cnt_err6; // unknown error
        s_long cnt_err7; // thread sync. failure or timeout
        s_long cntx_0; // total number of pv modifications (ins. + rem.)
        s_long cntx_1; // total number of link checks
        s_long cntx_2; // number of mismatches between link deref. under lock / without lock (this is not an error)
        s_long err_bits() const { return ((bool(cnt_err7) << 7) | (bool(cnt_err6) << 6) | (bool(cnt_err5) << 5) | (bool(cnt_err4) << 4) | (bool(cnt_err3) << 3) | (bool(cnt_err2) << 2) | (bool(cnt_err1) << 1)) >> 1; }
        logger& log;
        linktest_ctx(vec2_t<TA>* pv_, logger& log_)
          : pv(pv_), n_linked_sync(0), n_finished_sync(0), ind_stage_sync(0),
          cnt_err1(0), cnt_err2(0), cnt_err3(0), cnt_err4(0), cnt_err5(0), cnt_err6(0), cnt_err7(0),
          cntx_0(0), cntx_1(0), cntx_2(0),
          log(log_)
        {}
      };

      struct linktest_thread1 : bmdx::threadctl::ctx_base
      {
        void _thread_proc()
        {
          storage_t<vec2_t<T> > x9(0);

          linktest_ctx* p = *pdata<linktest_ctx*>(); logger& log = p->log; if (&log) {} vec2_t<TA>* pv = p->pv;
            if (p->ind_stage_sync != 1) { ++p->cnt_err7; goto lExit; }

          try {
            link2_t<TA> q;
            bool is_aendlnk(false);
            if (true)
            {
              _link2_reg::lock_t_reg __lock(true); if (!__lock.s.ptr()->is_locked()) { _link2_reg::lock_t_reg __lock(true); ++p->cnt_err4; goto lExit; }
              if (!x9.try_init()) { _link2_reg::lock_t_reg __lock(true); ++p->cnt_err1; goto lExit; }
              s_long ind0 = (1 + pv->n()) * rnd() - 1;
              q = pv->link2(ind0);
              if (q.is_empty()) { _link2_reg::lock_t_reg __lock(true); ++p->cnt_err1; goto lExit; }
              is_aendlnk = ind0 < 0;
              T* px = q.pv();
              if (!is_aendlnk && !px) { _link2_reg::lock_t_reg __lock(true); ++p->cnt_err1; goto lExit; }
              if (!is_aendlnk) { if (!x9.ptr()->el_append(*px)) { _link2_reg::lock_t_reg __lock(true); ++p->cnt_err1; goto lExit; } }
            }
            if (true)
            {
              _link2_reg::lock_t_reg __lock(true);
              ++p->n_linked_sync;
            }

            double t1;

            t1 = bmdx::clock_ms();
            while (bmdx::clock_ms() - t1 < 2 * linktest_ctx::duration_ms)
            {
              if (b_stop()) { goto lExit; }
              if (p->ind_stage_sync > 2) { goto lExit; }
              if (true)
              {
                if (1) { _link2_reg::lock_t_reg __lock(true); ++p->cntx_1; }
                T* px = q.pv(); // get ptr. without link locking
                if (!px) { ++p->cnt_err2; goto lExit; }

                _link2_reg::lock_t_reg __lock(true);
                  if (!__lock.s.ptr()->is_locked()) { _link2_reg::lock_t_reg __lock(true); ++p->cnt_err4; goto lExit; }
                  T* px2 = q.pv(); // get ptr. with link locking
                  if (!px2) { ++p->cnt_err2; goto lExit; }

                if (px2 != px) { _link2_reg::lock_t_reg __lock(true); ++p->cntx_2; }
                if (!_eq<T, T2>::F(*px2, *x9.ptr()->pval_first())) { _link2_reg::lock_t_reg __lock(true); ++p->cnt_err3; goto lExit; }
              }
              bmdx::sleep_mcs(1000);
            }

          } catch (...) { ++p->cnt_err6; }
        lExit:
          if (true)
          {
            _link2_reg::lock_t_reg __lock(true);
            x9.try_deinit();
            ++p->n_finished_sync;
          }
        }
      };

      struct linktest_thread2 : bmdx::threadctl::ctx_base
      {
        void _thread_proc()
        {

          linktest_ctx* p = *pdata<linktest_ctx*>(); logger& log = p->log; if (&log) {} vec2_t<TA>* pv = p->pv;
            if (p->ind_stage_sync != 2) { goto lExit; }

          try {

            double t1 = bmdx::clock_ms();
            while (bmdx::clock_ms() - t1 < linktest_ctx::duration_ms)
            {
              if (b_stop()) { goto lExit; }
              if (true)
              {
                ++p->cntx_0;
                s_long op = s_long(3 * rnd());
                switch (op)
                {
                  case 0:
                  {
                    s_long m = 1 + rnd() * linktest_ctx::mmax;
                    if (true)
                    {
                      _link2_reg::lock_t_reg __lock(true); if (!__lock.s.ptr()->is_locked()) { ++p->cnt_err4; goto lContinue; }
                      if (pv->el_append_m(m, _rnd_val<T, T2>::F()) != m) { ++p->cnt_err5; goto lContinue; }
                    }
                    bmdx::sleep_mcs(1000);
                    if (true)
                    {
                      _link2_reg::lock_t_reg __lock(true); if (!__lock.s.ptr()->is_locked()) { ++p->cnt_err4; goto lExit; }
                      if (pv->el_remove_ml(pv->nbase() + pv->n() - m, m) != m) { ++p->cnt_err5; goto lExit; }
                    }
                    break;
                  }
                  case 1:
                  {
                    s_long ind0 = rnd() * (1 + pv->n());
                    s_long m = 1 + rnd() * linktest_ctx::mmax;
                    if (true)
                    {
                      _link2_reg::lock_t_reg __lock(true); if (!__lock.s.ptr()->is_locked()) { ++p->cnt_err4; goto lContinue; }
                      if (pv->el_insert_ml(pv->nbase() + ind0, m, _rnd_val<T, T2>::F()) != m) { ++p->cnt_err5; goto lContinue; }
                    }
                    bmdx::sleep_mcs(1000);
                    if (true)
                    {
                      _link2_reg::lock_t_reg __lock(true); if (!__lock.s.ptr()->is_locked()) { ++p->cnt_err4; goto lExit; }
                      if (pv->el_remove_ml(pv->nbase() + ind0, m) != m) { ++p->cnt_err5; goto lExit; }
                    }
                    break;
                  }
                  default:
                  {
                    s_long ind0 = rnd() * (1 + pv->n());
                    s_long m = 1;
                    bool b = rnd() < 0.5;
                    if (true)
                    {
                      _link2_reg::lock_t_reg __lock(true); if (!__lock.s.ptr()->is_locked()) { ++p->cnt_err4; goto lContinue; }
                      if (pv->el_insert_1(pv->nbase() + ind0, _rnd_val<T, T2>::F()) != m) { ++p->cnt_err5; goto lContinue; }
                    }
                    bmdx::sleep_mcs(1000);
                    if (true)
                    {
                      _link2_reg::lock_t_reg __lock(true); if (!__lock.s.ptr()->is_locked()) { ++p->cnt_err4; goto lExit; }
                      if (pv->el_remove_1(pv->nbase() + ind0, b) != m) { ++p->cnt_err5; goto lExit; }
                    }
                    break;
                  }
                }
              }
        lContinue:;
            }

          } catch (...) { ++p->cnt_err6; }

        lExit:
          p->ind_stage_sync = 3;
        }
      };


      static int F(logger& log, vec2_t<TA>* pv, s_long nmax, s_long ntests)
      {
        if (pv) { log.d, "test_vec2_t::F: n", nmax, "nt", ntests, typeid(TA).name(), (meta::same_t<TA2, meta::nothing>::result ? "" : typeid(TA2).name()); log.flush(); }
          else { log.d, "test_vec2_t::F: pv == 0"; log.flush(); }

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
        vec2_t<TA> v2;
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
            s_long res = v2.vec2_copy(*pv, false); if (res != 1) { return -10 * npass - 6; }
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
            if (!__vecm_x::nr_ok(&pv->rvecm())) { return _retval(f.i, -36); }
          }
          else { if (pv->nrsv() != nr) { return _retval(f.i, -37); } }
        }

        vecm::link1_t<T, false> l1, l2;
        vecm::link1_t<T, true> l3, l4;
        if (l1.is_valid() || l2.is_valid() || l3.is_valid() || l4.is_valid()) { return -38; }
        l1 = pv->link1_begin(); l2 = pv->link1_aend(); l3 = pv->link1_cbegin(); l4 = pv->link1_caend();
          if (!(l1.is_valid() && l2.is_valid() && l3.is_valid() && l4.is_valid())) { return -39; }
          if (l1.ind() != pv->nbase() || l1.is_out()) { return -39; }
          if (l2.ind() != pv->nbase() + pv->n() || !l2.is_out() || !l2.is_aend()) { return -40; }
          if (l3.ind() != pv->nbase() || l3.is_out()) { return -41; }
          if (l4.ind() != pv->nbase() + pv->n() || !l4.is_out() || !l4.is_aend()) { return -42; }

        for (f.i = 0; f.i < n1 + m; ++f.i)
        {
          px = pv->pval(f.i + pv->nbase());
          if (!px) { return _retval(f.i, -50); }
          if (f.i >= n1) { if (_eq<T, T2>::F(*px, t_conv::F(f.i)) != 1) { return _retval(f.i, -51); } }

          T* px2 = pv->pval_0u(f.i);
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

          if (pv->pval_first() != l2.pval()) { return -75; }
          if (pv->pval_first() != l4.pval()) { return -76; }

        l2.move_by(n1 + m - 1);
        l4.move_by(n1 + m - 1);
          if (const_cast<const vec2_t<TA>*>(pv)->pval_last() != l2.pval()) { return -110; }
          if (const_cast<const vec2_t<TA>*>(pv)->pval_last() != l4.pval()) { return -111; }
        l1.move_by(-(n1 + m));
        l3.move_by(-(n1 + m));
        l2.move_by(-(n1 + m - 1));
        l4.move_by(-(n1 + m - 1));
          if (pv->pval_first() != l1.pval()) { return -79; }
          if (pv->pval_first() != l2.pval()) { return -80; }
          if (pv->pval_first() != l3.pval()) { return -81; }
          if (pv->pval_first() != l4.pval()) { return -82; }
          if (const_cast<const vec2_t<TA>*>(pv)->pval_first() != l3.pval()) { return -108; }
          if (const_cast<const vec2_t<TA>*>(pv)->pval_first() != l4.pval()) { return -109; }


        l2.set_aend();
        l4.set_aend();
        for (f.i = 0; f.i < n1 + m; ++f.i)
        {
          l1.move_by(f.i);
            if (l1.ind() != pv->nbase() + f.i) { return -83; }
            if (l1.pval() != pv->pval_0u(f.i)) { return -84; }
            if (l1.pval() != pv->pval(l1.ind())) { return -106; }
            if (l1.pval() != pv->pc(l1.ind())) { return -107; }
            if (l1.pval() != const_cast<const vec2_t<TA>*>(pv)->pval(l1.ind())) { return -110; }
            if (l1.pval() != const_cast<const vec2_t<TA>*>(pv)->pc(l1.ind())) { return -111; }
            if (l1.pval() != const_cast<const vec2_t<TA>*>(pv)->pval_0u(f.i)) { return -112; }
            l1.move_by(-f.i);
              if (l1.ind() != pv->nbase()) { return -85; }
              if (l1.pval() != pv->pval_first()) { return -86; }
          l3.move_by(f.i);
            if (l3.ind() != pv->nbase() + f.i) { return -87; }
            if (l3.pval() != pv->pval_0u(f.i)) { return -88; }
            l3.move_by(-f.i);
              if (l3.ind() != pv->nbase()) { return -89; }
              if (l3.pval() != pv->pval_first()) { return -90; }
          l2.move_by(-f.i-1);
            if (l2.ind() != pv->nbase() + n1+ m - f.i - 1) { return -91; }
            if (l2.pval() != pv->pval_0u(n1+ m - f.i - 1)) { return -92; }
            l2.move_by(f.i);
              if (l2.ind() != pv->nbase() + n1+ m - 1) { return -93; }
              if (l2.pval() != pv->pval_last()) { return -94; }
              l2.incr();
                if (!l2.is_aend()) { return -95; }
          l4.move_by(-f.i-1);
            if (l4.ind() != pv->nbase() + n1+ m - f.i - 1) { return -101; }
            if (l4.pval() != pv->pval_0u(n1+ m - f.i - 1)) { return -102; }
            l4.move_by(f.i);
              if (l4.ind() != pv->nbase() + n1+ m - 1) { return -103; }
              if (l4.pval() != pv->pval_last()) { return -104; }
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
            case 0: // clear
            {
              if (rnd() >= p_shrink) { __skip = true; break; }
              if (pv->nexc() != 0) { return _retval(q, -250); }
              s_long nx = 0; if (excgen_setf_vecm<T>::exists && pv->n() >= 2) { nx = 2; excgen_setf_vecm<T>::F(&pv->rvecm(), 0, false, true); excgen_setf_vecm<T>::F(&pv->rvecm(), 1 + (pv->n() - 1) * rnd(), false, true); }
                m = pv->vec2_clear();
                if (m != nx) { return _retval(q, -251); }
                __nr = __vecm_x::_cap(0);
              if (pv->nexc() != 0) { return _retval(q, -252); }
              if (rnd() < 0.2) { pv->vec2_setf_can_shrink(!pv->can_shrink()); }
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
                    excgen* px = pv->rvecm().template el_append<excgen>(x2);
                    if (px) { return _retval(q, -272); }
                    if (pv->n() != __n + m2) { return _retval(q, -273); }
                }
                else
                {
                  T x9(t_conv::F(f.i)); if (pv->n() >= 1) { _assign<T>::F(x9, *pv->pval_last()); }
                  px = pv->el_append(f);
                    if (!px) { return _retval(q, -270); }
                    if (_eq<T, T2>::F(*px, t_conv::F(f.i)) != 1) { return _retval(q, -271); }
                    m2 += 1;
                    if (pv->n() != __n + m2) { return _retval(q, -274); }
                    if (pv->n() >= 2 && _eq<T, T2>::F(x9, *pv->pval_0u(pv->n() - 2)) != 1) { return _retval(q, -276); }
                }
              }
              if (m2 != m) { __cancelled = true; }
              break;
            }
            case 2: // el_remove_all
            {
              if (rnd() >= p_shrink) { __skip = true; break; }
              if (pv->nexc() != 0) { return _retval(q, -290); }
              s_long nx = 0; if (excgen_setf_vecm<T>::exists && pv->n() >= 2) { nx = 2; excgen_setf_vecm<T>::F(&pv->rvecm(), pv->n() - 1, false, true); excgen_setf_vecm<T>::F(&pv->rvecm(), (pv->n() - 1) * rnd(), false, true); }
              m = pv->el_remove_all();
                if (m != __n) { return _retval(q, -291); }
                if (pv->nexc() != nx) { return _retval(q, -292); }
                pv->vec2_set0_nexc();
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
              px = pv->el_expand_1();
                if (!px) { return _retval(q, -310); }
                if (pv->n() != __n + 1) { return _retval(q, -311); }
              T x(t_conv::F(__n));
                if (pv->ptd()->p_copy_1(pv->pval_last(), &x) < 0) { static_cast<__vecm_x*>(&pv->rvecm())->template forced_clear<T>(__n); return _retval(q, -312); }
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
                for (s_long i = __n; i < n; ++i) { T x(t_conv::F(i)); if (pv->ptd()->p_copy_1(pv->pval_0u(i), &x) < 0) { break; } ++m; }
                  if (m < n - __n) { static_cast<__vecm_x*>(&pv->rvecm())->template forced_clear<T>(__n + m); return _retval(q, -231); }
              }
              else // shrink
              {
                if (rnd() >= p_shrink) { __skip = true; break; }
                n = __n * rnd();
                m = 0;
                for (s_long i = __n - 1; i >= n; --i) { if (pv->ptd()->p_destroy_1(pv->pval_0u(i)) < 0) { break; } ++m; }
                  if (m < __n - n) { static_cast<__vecm_x*>(&pv->rvecm())->template forced_clear<T>(__n - m); return _retval(q, -232); }
                if (!pv->el_expand_n(n)) { return _retval(q, -234); }
                  if (pv->n() != n) { return _retval(q, -235); }
              }
              break;
            }
            case 6: // el_expunge_last
            {
              if (rnd() >= p_shrink) { __skip = true; break; }
              if (pv->ptd()->p_destroy_1(pv->pval_last()) < 0) { return _retval(q, -331); }
              if (!pv->el_expunge_last()) { static_cast<__vecm_x*>(&pv->rvecm())->template forced_clear<T>(__n - 1); return _retval(q, -332); }
                if (pv->n() != __n - 1) { return _retval(q, -333); }
              break;
            }
            case 7: // el_remove_last
            {
              if (rnd() >= p_shrink) { __skip = true; break; }
              if (pv->nexc() != 0) { return _retval(q, -350); }
              s_long nx = 0; if (excgen_setf_vecm<T>::exists && rnd() < 0.3) { nx = 1; excgen_setf_vecm<T>::F(&pv->rvecm(), pv->n() - 1, false, true); }
              if (pv->el_remove_last() != 1) { return _retval(q, -351); }
                if (pv->nexc() != nx) { return _retval(q, -352); }
                pv->vec2_set0_nexc();
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
                vec2_t<T> x9; s_long i9[n_cmp]; if (__n >= 1) { for (s_long i = 0; i < n_cmp; ++i) { i9[i] = __n * rnd(); if (!x9.el_append(*pv->pval_0u(i9[i]))) { return _retval(q, -445); } } }
                  if (!(pv->ptd()->op_flags & 0xc)) // non-tr. ins. alg.
                  {
                    if (ind0 < __n && !(__n < __vecm_x::_nrsv_n(__n - 1) && ind0 == __vecm_x::_nrsv_n(__n - 1) - __vecm_x::_cap(__n - 1))) { nx += 1; } // exc. is counted for x2 copy only if insertion moves anything
                    if (ind0 < __n - __vecm_x::_cap(__n - 1)) { pv->rvecm().template pval_0u<excgen>(ind0)->setf_xc(true); pv->rvecm().template pval_0u<excgen>(ind0)->setf_xd(true); nx += 2; p0 = pv->rvecm().template pval_0u<excgen>(ind0); }
                  }
                m2 = pv->rvecm().template el_insert_1<excgen>(ind, x2);
                  if (m2 == -3)
                  {
                    if (pv->n() != __n) { return _retval(q, -422); }
                    if (pv->nexc() != nx)
                    {
                      if (!p0 || p0 != pv->rvecm().template pval_0u<excgen>(ind0)) { return _retval(q, -423); }
                      if (nx - pv->nexc() != 2) { return _retval(q, -496); }
                      if (!(p0->f_xc() && p0->f_xd())) { return _retval(q, -497); }
                      p0->setf_xc(false); p0->setf_xd(false);
                    }
                    if (__n >= 1) { for (s_long i = 0; i < n_cmp; ++i) { if (_eq<T, T2>::F(*x9.pval_0u(i), *pv->pval_0u(i9[i])) != 1) { return _retval(q, -425); } } }
                  }
                  else if (m2 == -4)
                  {
                    m2 = m;
                    if (pv->integrity() != -1) { return _retval(q, -571); }
                      pv->vec2_setf_unsafe(false);
                    if (pv->is_transactional()) { return _retval(q, -426); }
                    if (pv->n() != __n + m) { return _retval(q, -427); }
                    if (pv->nexc() != nx)
                    {
                      if (!p0 || p0 != pv->rvecm().template pval_0u<excgen>(ind0 + 1)) { return _retval(q, -428); }
                      if (nx - pv->nexc() != 2) { return _retval(q, -494); }
                      if (!(p0->f_xc() && p0->f_xd())) { return _retval(q, -495); }
                      p0->setf_xc(false); p0->setf_xd(false);
                    }
                    if (__n >= 1) { for (s_long i = 0; i < n_cmp; ++i) { if (i9[i] == ind0) { continue; } if (_eq<T, T2>::F(*x9.pval_0u(i), *pv->pval_0u(i9[i] < ind0 ? i9[i] : i9[i] + 1)) != 1) { return _retval(q, -429); } } }
                    if (pv->rvecm().template pval_0u<excgen>(ind0)->a != f.i) { return _retval(q, -430); }
                    if (pv->rvecm().template pval_0u<excgen>(ind0)->b != -1) { return _retval(q, -431); }
                    if (pv->rvecm().template pval_0u<excgen>(ind0 + m - 1)->a != f.i) { return _retval(q, -432); }
                    if (pv->rvecm().template pval_0u<excgen>(ind0 + m - 1)->b != -1) { return _retval(q, -433); }
                  }
                  else
                  {
                    return _retval(q, -434);
                  }
              }
              else
              {
                s_long nx = 0;
                vec2_t<T> x9; s_long i9[n_cmp]; if (__n >= 1) { for (s_long i = 0; i < n_cmp; ++i) { i9[i] = __n * rnd(); if (!x9.el_append(*pv->pval_0u(i9[i]))) { return _retval(q, -444); } } }
                bool _b_excgen_mmode3 = excgen_setf_vecm<T>::exists && (pv->ptd()->op_flags & 0x8);
                bool _b_xc(false);
                if (excgen_setf_vecm<T>::exists && rnd() < 0.3) { if (ind0 < __n - __vecm_x::_cap(__n - 1)) { pv->rvecm().template pval_last<excgen>()->setf_xd(true); nx += 1; } }
                if (_b_excgen_mmode3 && rnd() < 0.3 && ind0 < __n) { _b_xc = true; pv->rvecm().template pval_0u<excgen>(ind0)->setf_xc(true); }
                m2 = pv->el_insert_1(ind, f);
                  if (m2 != m) { return _retval(q, -435); }
                  if (pv->n() != __n + m) { return _retval(q, -436); }
                  if (pv->nexc() != nx)
                  {
                    if (!excgen_setf_vecm<T>::exists) { return _retval(q, -490); }
                    if (!pv->rvecm().template pval_last<excgen>()->f_xd()) { return _retval(q, -437); }
                    pv->rvecm().template pval_last<excgen>()->setf_xd(false);
                  }
                  else
                  {
                    if (excgen_setf_vecm<T>::exists && pv->rvecm().template pval_last<excgen>()->flags && (!_b_excgen_mmode3 || ind0 != __n - 1)) { return _retval(q, -489); }
                  }
                  if (_b_excgen_mmode3)
                  {
                    if (_b_xc && !pv->rvecm().template pval_0u<excgen>(ind0 + m)->f_xc()) { return _retval(q, -486); }
                    if (_b_xc) { pv->rvecm().template pval_0u<excgen>(ind0 + m)->setf_xc(false); }
                  }
                  if (__n >= 1) { for (s_long i = 0; i < n_cmp; ++i) { if (_eq<T, T2>::F(*x9.pval_0u(i), *pv->pval_0u(i9[i] < ind0 ? i9[i] : i9[i] + 1)) != 1) { return _retval(q, -438); } } }
                  if (excgen_setf_vecm<T>::exists)
                  {
                    if (pv->rvecm().template pval_0u<excgen>(ind0)->a != f.i) { return _retval(q, -439); }
                    if (pv->rvecm().template pval_0u<excgen>(ind0)->b != f.i) { return _retval(q, -440); }
                    if (pv->rvecm().template pval_0u<excgen>(ind0 + m - 1)->a != f.i) { return _retval(q, -441); }
                    if (pv->rvecm().template pval_0u<excgen>(ind0 + m - 1)->b != f.i) { return _retval(q, -442); }
                  }
                  else
                  {
                    if (_eq<T, T2>::F(*pv->pval_0u(ind0), t_conv::F(f.i)) != 1) { return _retval(q, -443); }
                    if (_eq<T, T2>::F(*pv->pval_0u(ind0 + m - 1), t_conv::F(f.i)) != 1) { return _retval(q, -446); }
                  }
                m2 = m;
              }
              pv->vec2_set0_nexc();
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
                vec2_t<T> x9; if (!x9.el_append(*pv->pval_last())) { return _retval(q, -462); }
                if (excgen_setf_vecm<T>::exists && rnd() < 0.3)
                {
                  pv->rvecm().template pval_0u<excgen>(ind0)->setf_xd(true);
                  pv->rvecm().template pval_0u<excgen>(__n - 1)->setf_xc(true);
                  m2 = pv->el_remove_1(ind, true);
                    if (m2 == -3)
                    {
                      if (ind0 == __n - 1) { return _retval(q, -463); }
                      if (!(pv->ptd()->op_flags & 0x1)) { return _retval(q, -464); }
                      if (pv->n() != __n) { return _retval(q, -465); }

                      pv->rvecm().template pval_0u<excgen>(ind0)->setf_xd(false);
                      pv->rvecm().template pval_0u<excgen>(__n - 1)->setf_xc(false);

                      if (ind0 != __n - 1 && !_eq<T, T2>::F(*pv->pval_last(), *x9.pval_first())) { return _retval(q, -466); }
                    }
                    else if (m2 == -4)
                    {
                      if (pv->integrity() != -1) { return _retval(q, -572); }
                        pv->vec2_setf_unsafe(false);
                      if (ind0 == __n - 1) { return _retval(q, -467); }
                      if (pv->n() != __n - 1) { return _retval(q, -468); }
                      if (pv->nexc() != 2) { return _retval(q, -468); }
                      if (pv->rvecm().template pval_0u<excgen>(ind0)->a != x9.rvecm().template pval_first<excgen>()->a) { return _retval(q, -470); }
                      if (pv->rvecm().template pval_0u<excgen>(ind0)->b != -1) { return _retval(q, -471); }
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
                          pv->rvecm().template pval_0u<excgen>(ind0)->setf_xc(false);
                        if (pv->n() != __n - 1) { return _retval(q, -482); }
                        if (pv->nexc() != 1) { return _retval(q, -483); }
                        if (ind0 != __n - 1 && !_eq<T, T2>::F(*pv->pval_0u(ind0), *x9.pval_first())) { return _retval(q, -484); }
                      }
                    }
                }
                else
                {
                  nx = 0;
                  if (excgen_setf_vecm<T>::exists && rnd() < 0.3)
                  {
                    pv->rvecm().template pval_0u<excgen>(ind0)->setf_xd(true);
                    nx += 1;
                  }
                  m2 = pv->el_remove_1(ind, true);
                    if (m2 != 1) { return _retval(q, -476); }
                    if (pv->n() != __n - 1) { return _retval(q, -477); }
                    if (pv->nexc() != nx)
                    {
                      if (pv->ptd()->op_flags & 0x8) // excgen_mmode3 only
                      {
                        if (!pv->rvecm().template pval_last<excgen>()->f_xd()) { return _retval(q, -485); }
                        pv->rvecm().template pval_last<excgen>()->setf_xd(false);
                        if (pv->rvecm().template pval_last<excgen>()->flags) { return _retval(q, -491); }
                      }
                      else { return _retval(q, -478); }
                    }
                    if (ind0 != __n - 1 && !_eq<T, T2>::F(*pv->pval_0u(ind0), *x9.pval_first())) { return _retval(q, -479); }
                }
              }
              else // test move_last == false
              {
                if (excgen_setf_vecm<T>::exists && rnd() < 0.3)
                {
                  s_long nx = 0;
                  const s_long n_cmp2 = n_cmp < m ? s_long(n_cmp) : m;
                  vec2_t<T> x9; s_long i9[n_cmp];
                  vec2_t<T> x9b; std::vector<s_long> i9b; try { i9b.resize(n_cmp2); } catch (...) { return _retval(q, -566); }
                    bool _tr_alg = (__n - ind0 > m) && (pv->ptd()->op_flags & 0x1); // most prob. that tr. alg. will be called (may be non-tr. in rare cases)
                    if (__n > m) { for (s_long i = 0; i < n_cmp; ++i) { i9[i] = (__n - m) * rnd(); if (!x9.el_append(*pv->pval_0u(i9[i] < ind0 ? i9[i] : i9[i] + m))) { return _retval(q, -500); } } }
                    for (s_long i = 0; i < n_cmp2; ++i) { i9b[i] = m * rnd(); if (!x9b.el_append(*pv->pval_0u(ind0 + i9b[i]))) { return _retval(q, -501); } }
                    s_long __j1, __j2, __j0, __k, __cap; __vecm_x::_ind_jk(ind0, __j0, __k, __cap); __vecm_x::_ind_jk(ind0 + m - 1, __j1, __k, __cap); __vecm_x::_ind_jk(__n - 1, __j2, __k, __cap);
                    pv->rvecm().template pval_0u<excgen>(ind0)->setf_xd(true);
                      nx += 1;
                    for (s_long j = __j1 + 1; j <= __j2;  ++j) { pv->rvecm().template pval_0u<excgen>(__vecm_x::_nrsv(j))->setf_xc(true); }
                      s_long nx2 = __j2 - __j1;
                  m2 = pv->el_remove_1(ind, false);
                    if (m2 == -3)
                    {
                      if (!_tr_alg) { return _retval(q, -502); }
                      if (pv->n() != __n) { return _retval(q, -503); }
                      if (pv->nexc() != 0) { return _retval(q, -504); }

                      pv->rvecm().template pval_0u<excgen>(ind0)->setf_xd(false);
                      for (s_long j = __j1 + 1; j <= __j2;  ++j) { pv->rvecm().template pval_0u<excgen>(__vecm_x::_nrsv(j))->setf_xc(false); }

                      if (__n > m) { for (s_long i = 0; i < n_cmp; ++i) { if (_eq<T, T2>::F(*x9.pval_0u(i), *pv->pval_0u(i9[i] < ind0 ? i9[i] : i9[i] + m)) != 1) { return _retval(q, -505); } } }
                      for (s_long i = 0; i < n_cmp2; ++i) { if (_eq<T, T2>::F(*x9b.pval_0u(i), *pv->pval_0u(ind0 + i9b[i])) != 1) { return _retval(q, -506); } }
                    }
                    else if (m2 == -4)
                    {
                      if (pv->integrity() != -1) { return _retval(q, -573); }
                        pv->vec2_setf_unsafe(false);
                      if (pv->n() != __n - m) { return _retval(q, -507); }
                      if (pv->nexc() != nx + nx2) { return _retval(q, -508); }
                      if (__n > m) { for (s_long i = 0; i < n_cmp; ++i)
                      {
                        if (x9.rvecm().template pval_0u<excgen>(i)->a != pv->rvecm().template pval_0u<excgen>(i9[i])->a) { return _retval(q, -509); }
                        if (x9.rvecm().template pval_0u<excgen>(i)->b != pv->rvecm().template pval_0u<excgen>(i9[i])->b && -1 != pv->rvecm().template pval_0u<excgen>(i9[i])->b) { return _retval(q, -510); }
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
                          for (s_long i = __vecm_x::_nrsv(__j0); i < pv->n();  ++i) { pv->rvecm().template pval_0u<excgen>(i)->setf_xc(false); }
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
                  vec2_t<T> x9; s_long i9[n_cmp];
                    if (__n > m) { for (s_long i = 0; i < n_cmp; ++i) { i9[i] = (__n - m) * rnd(); if (!x9.el_append(*pv->pval_0u(i9[i] < ind0 ? i9[i] : i9[i] + m))) { return _retval(q, -516); } } }
                    if (excgen_setf_vecm<T>::exists && rnd() < 0.3)
                    {
                      pv->rvecm().template pval_0u<excgen>(ind0)->setf_xd(true);
                        nx += 1;
                    }
                  m2 = pv->el_remove_1(ind, false);
                    if (m2 != m) { return _retval(q, -517); }
                    if (pv->n() != __n - m) { return _retval(q, -518); }
                    if (pv->nexc() != nx) { return _retval(q, -519); }
                    if (__n > m) { for (s_long i = 0; i < n_cmp; ++i) { if (_eq<T, T2>::F(*x9.pval_0u(i), *pv->pval_0u(i9[i])) != 1) { return _retval(q, -520); } } }
                }
              }
              pv->vec2_set0_nexc();
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
                m2 = pv->rvecm().template el_append_m<excgen>(m, x2);
                  if (m2 != -3) { return _retval(q, -370); }
                  if (pv->n() != __n) { return _retval(q, -371); }
                  if (!(m == 1) && pv->nrsv() != __nr) { return _retval(q, -372); } // if not using el_append, must revert the reserve
              }
              else
              {
                T x9(t_conv::F(f.i)); if (__n >= 1) { _assign<T>::F(x9, *pv->pval_last()); }
                m2 = pv->el_append_m(m, f);
                  if (m2 != m) { return _retval(q, -373); }
                  if (pv->n() != __n + m) { return _retval(q, -374); }
                  if (_eq<T, T2>::F(*pv->pval_last(), t_conv::F(f.i)) != 1) { return _retval(q, -375); }
                  if (__n >= 1 && _eq<T, T2>::F(x9, *pv->pval_0u(__n - 1)) != 1) { return _retval(q, -376); }
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
                T x9(t_conv::F(f.i)); if (__n >= 1) { _assign<T>::F(x9, *pv->pval_last()); }
                excgen x2(f.i); x2.setf_xc(true);
                s_long nx = 0;
                  if (pv->ptd()->op_flags & 0xc) // tr. ins. alg.
                  {
                    // nx += 1; // exc. on the first x2 copy won't be counted in tr. mode
                    // This exc. will not occur, since exc. on x2 copy occurs first and the transaction is cancelled.
                    if (ind0 < __n - __vecm_x::_cap(__n - 1)) { pv->rvecm().template pval_last<excgen>()->setf_xd(true); }
                  }
                  else
                  {
                    if (ind0 < __n) { nx += m; } // if not appending, exc. is counted for each x2 copy
                    // Both exc. will occur.
                    if (ind0 < __n - __vecm_x::_cap(__n - 1)) { pv->rvecm().template pval_0u<excgen>(__n - 2)->setf_xc(true); pv->rvecm().template pval_0u<excgen>(__n - 2)->setf_xd(true); nx += 2; }
                  }
                m2 = pv->rvecm().template el_insert_ml<excgen>(ind, m, x2);
                  if (m2 == -3)
                  {
                    if (pv->n() != __n) { return _retval(q, -392); }
                    if (pv->nexc() != nx) { return _retval(q, -393); }
                    if (!(m == 1 && ind0 == __n) && pv->nrsv() != __nr) { return _retval(q, -394); } // if not using el_append, must revert the reserve
                    if (__n >= 1 && _eq<T, T2>::F(x9, *pv->pval_last()) != 1) { return _retval(q, -395); }
                    // Disable exc.
                    if (ind0 < __n - __vecm_x::_cap(__n - 1)) { pv->rvecm().template pval_last<excgen>()->setf_xd(false); }
                  }
                  else if (m2 == -4)
                  {
                    m2 = m;
                    if (pv->integrity() != -1) { return _retval(q, -574); }
                      pv->vec2_setf_unsafe(false);
                    if (pv->is_transactional()) { return _retval(q, -396); }
                    if (pv->n() != __n + m) { return _retval(q, -397); }
                    if (pv->nexc() != nx)
                    {
                      if (pv->ptd()->op_flags & 0x8) // excgen_mmode3 only
                      {
                        if (!pv->rvecm().template pval_last<excgen>()->f_xd()) { return _retval(q, -417); }
                        pv->rvecm().template pval_last<excgen>()->setf_xd(false);
                        if (pv->rvecm().template pval_last<excgen>()->flags) { return _retval(q, -492); }
                      }
                      else { return _retval(q, -398); }
                    }
                    if (__n >= 1 && _eq<T, T2>::F(x9, *pv->pval_0u(ind0 == __n ? __n - 1 : __n - 1 + m)) != 1) { return _retval(q, -399); }
                    if (pv->rvecm().template pval_0u<excgen>(ind0)->a != f.i) { return _retval(q, -400); }
                    if (pv->rvecm().template pval_0u<excgen>(ind0)->b != -1) { return _retval(q, -401); }
                    if (pv->rvecm().template pval_0u<excgen>(ind0 + m - 1)->a != f.i) { return _retval(q, -402); }
                    if (pv->rvecm().template pval_0u<excgen>(ind0 + m - 1)->b != -1) { return _retval(q, -403); }
                  }
                  else
                  {
                    return _retval(q, -404);
                  }
              }
              else
              {
                T x9(t_conv::F(f.i));
                if (__n >= 1) { _assign<T>::F(x9, *pv->pval_last()); }
                s_long nx = 0;
                bool _b_excgen_mmode3 = excgen_setf_vecm<T>::exists && (pv->ptd()->op_flags & 0x8);
                bool _b_xc(false);
                if (excgen_setf_vecm<T>::exists && rnd() < 0.3) { if (ind0 < __n - __vecm_x::_cap(__n - 1)) { pv->rvecm().template pval_last<excgen>()->setf_xd(true); nx += 1; } }
                if (_b_excgen_mmode3 && rnd() < 0.3 && ind0 < __n) { _b_xc = true; pv->rvecm().template pval_0u<excgen>(ind0)->setf_xc(true); }
                m2 = pv->el_insert_ml(ind, m, f);
                  if (m2 != m) { return _retval(q, -405); }
                  if (pv->n() != __n + m) { return _retval(q, -406); }
                  if (pv->nexc() != nx)
                  {
                    if (!pv->rvecm().template pval_last<excgen>()->f_xd()) { return _retval(q, -416); }
                    pv->rvecm().template pval_last<excgen>()->setf_xd(false);
                    if (!excgen_setf_vecm<T>::exists) { return _retval(q, -493); }
                  }
                  else
                  {
                    if (excgen_setf_vecm<T>::exists && pv->rvecm().template pval_last<excgen>()->flags && (!_b_excgen_mmode3 || ind0 != __n - 1)) { return _retval(q, -407); }
                  }
                  if (_b_excgen_mmode3)
                  {
                    if (_b_xc && !pv->rvecm().template pval_0u<excgen>(ind0 + m)->f_xc()) { return _retval(q, -487); }
                    if (_b_xc) { pv->rvecm().template pval_0u<excgen>(ind0 + m)->setf_xc(false); }
                  }
                  if (__n >= 1 && _eq<T, T2>::F(x9, *pv->pval_0u(ind0 == __n ? __n - 1 : __n - 1 + m)) != 1) { return _retval(q, -408); }
                  if (excgen_setf_vecm<T>::exists)
                  {
                    if (pv->rvecm().template pval_0u<excgen>(ind0)->a != f.i) { return _retval(q, -409); }
                    if (pv->rvecm().template pval_0u<excgen>(ind0)->b != f.i) { return _retval(q, -410); }
                    if (pv->rvecm().template pval_0u<excgen>(ind0 + m - 1)->a != f.i) { return _retval(q, -411); }
                    if (pv->rvecm().template pval_0u<excgen>(ind0 + m - 1)->b != f.i) { return _retval(q, -412); }
                  }
                  else
                  {
                    if (_eq<T, T2>::F(*pv->pval_0u(ind0), t_conv::F(f.i)) != 1) { return _retval(q, -413); }
                    if (_eq<T, T2>::F(*pv->pval_0u(ind0 + m - 1), t_conv::F(f.i)) != 1) { return _retval(q, -415); }
                  }
              }
              pv->vec2_set0_nexc();
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
                vec2_t<T> x9; s_long i9[n_cmp];
                vec2_t<T> x9b; std::vector<s_long> i9b; try { i9b.resize(n_cmp2); } catch (...) { return _retval(q, -565); }
                  bool _tr_alg = (__n - ind0 > m) && (pv->ptd()->op_flags & 0x1); // most prob. that tr. alg. will be called (may be non-tr. in rare cases)
                  if (__n > m) { for (s_long i = 0; i < n_cmp; ++i) { i9[i] = (__n - m) * rnd(); if (!x9.el_append(*pv->pval_0u(i9[i] < ind0 ? i9[i] : i9[i] + m))) { return _retval(q, -542); } } }
                  for (s_long i = 0; i < n_cmp2; ++i) { i9b[i] = m * rnd(); if (!x9b.el_append(*pv->pval_0u(ind0 + i9b[i]))) { return _retval(q, -543); } }
                  pv->rvecm().template pval_0u<excgen>(ind0)->setf_xd(true);
                  pv->rvecm().template pval_0u<excgen>(ind0 + m - 1)->setf_xd(true);
                    nx += m == 1 ? 1 : 2;
                  const s_long x2_step = 17;
                  for (s_long i = ind0 + m; i < __n; i += x2_step) { pv->rvecm().template pval_0u<excgen>(i)->setf_xc(true); }
                    s_long nx2 = (__n - (ind0 + m) + x2_step - 1) / x2_step;
                m2 = pv->el_remove_ml(ind, m);
                  if (m2 == -3)
                  {
                    if (!_tr_alg) { return _retval(q, -544); }
                    if (pv->n() != __n) { return _retval(q, -545); }
                    if (pv->nexc() != 0) { return _retval(q, -546); }

                    pv->rvecm().template pval_0u<excgen>(ind0)->setf_xd(false);
                    pv->rvecm().template pval_0u<excgen>(ind0 + m - 1)->setf_xd(false);
                    for (s_long i = ind0 + m; i < __n; i += x2_step) { pv->rvecm().template pval_0u<excgen>(i)->setf_xc(false); }

                    if (__n > m) { for (s_long i = 0; i < n_cmp; ++i) { if (_eq<T, T2>::F(*x9.pval_0u(i), *pv->pval_0u(i9[i] < ind0 ? i9[i] : i9[i] + m)) != 1) { return _retval(q, -547); } } }
                    for (s_long i = 0; i < n_cmp2; ++i) { if (_eq<T, T2>::F(*x9b.pval_0u(i), *pv->pval_0u(ind0 + i9b[i])) != 1) { return _retval(q, -548); } }
                  }
                  else if (m2 == -4)
                  {
                    if (pv->integrity() != -1) { return _retval(q, -575); }
                      pv->vec2_setf_unsafe(false);
                    if (pv->n() != __n - m) { return _retval(q, -549); }
                    if (pv->nexc() != nx + nx2) { return _retval(q, -550); }
                    if (__n > m) { for (s_long i = 0; i < n_cmp; ++i)
                    {
                      if (x9.rvecm().template pval_0u<excgen>(i)->a != pv->rvecm().template pval_0u<excgen>(i9[i])->a) { return _retval(q, -551); }
                      if (x9.rvecm().template pval_0u<excgen>(i)->b != pv->rvecm().template pval_0u<excgen>(i9[i])->b && -1 != pv->rvecm().template pval_0u<excgen>(i9[i])->b) { return _retval(q, -552); }
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
                      for (s_long i = ind0; i < pv->n(); i += x2_step) { pv->rvecm().template pval_0u<excgen>(i)->setf_xc(false); }
                      if (__n > m) { for (s_long i = 0; i < n_cmp; ++i) { if (*x9.rvecm().template pval_0u<excgen>(i) != *pv->rvecm().template pval_0u<excgen>(i9[i])) { return _retval(q, -564); } } }
                    }
                    m2 = m;
                  }
              }
              else
              {
                s_long nx = 0;
                vec2_t<T> x9; s_long i9[n_cmp];
                  if (__n > m) { for (s_long i = 0; i < n_cmp; ++i) { i9[i] = (__n - m) * rnd(); if (!x9.el_append(*pv->pval_0u(i9[i] < ind0 ? i9[i] : i9[i] + m))) { return _retval(q, -557); } } }
                  if (excgen_setf_vecm<T>::exists && rnd() < 0.3)
                  {
                    pv->rvecm().template pval_0u<excgen>(ind0)->setf_xd(true);
                    pv->rvecm().template pval_0u<excgen>(ind0 + m - 1)->setf_xd(true);
                      nx += m == 1 ? 1 : 2;
                  }
                m2 = pv->el_remove_ml(ind, m);
                  if (m2 != m) { return _retval(q, -558); }
                  if (pv->n() != __n - m) { return _retval(q, -559); }
                  if (pv->nexc() != nx) { return _retval(q, -560); }
                  if (__n > m) { for (s_long i = 0; i < n_cmp; ++i) { if (_eq<T, T2>::F(*x9.pval_0u(i), *pv->pval_0u(i9[i])) != 1) { return _retval(q, -570); } } }
              }
              pv->vec2_set0_nexc();
              if (m2 != m) { __cancelled = true; }
              break;
            }
            case 13: // copy constructor
            {
              vec2_t<T> v2(*pv);
              if (v2.integrity() != 1) { return _retval(q, -621); }
              if (v2.n() != __n) { return _retval(q, -622); }
              if (v2.nbase() != pv->nbase()) { return _retval(q, -625); }
              if (v2.nexc() != 0) { return _retval(q, -623); }
              if (v2.can_shrink() != true) { return _retval(q, -624); }
              for (s_long i = 0; i < __n; ++i) { if (_eq<T, T2>::F(*v2.pval_0u(i), *pv->pval_0u(i)) != 1) { return _retval(q, -626); } }
              __cancelled = true; // pv is not changed in this test
              break;
            }
            case 14: // vec2_copy
            {
              vec2_t<T> v2;
              if (v2.vec2_copy(*pv, q & 1) != 1) { return _retval(q, -600); }
              if (v2.integrity() != 1) { return _retval(q, -601); }
              if (v2.n() != __n) { return _retval(q, -602); }
              if (v2.nbase() != pv->nbase()) { return _retval(q, -605); }
              if (v2.nexc() != 0) { return _retval(q, -603); }
              if (v2.can_shrink() != true) { return _retval(q, -604); }
              for (s_long i = 0; i < __n; ++i) { if (_eq<T, T2>::F(*v2.pval_0u(i), *pv->pval_0u(i)) != 1) { return _retval(q, -606); } }
              __cancelled = true; // pv is not changed in this test
              break;
            }
            default: { return -8; }
          }

          if (__skip) { continue; }

          if (pv->integrity() < 0) { return _retval(q * 10 + 1, -880 - ind_mf); }
          if (pv->nrsv() < pv->n()) { return _retval(q * 10 + 2, -880 - ind_mf); }
          if (!__cancelled)
          {
            if (pv->n() < __n)
            {
              if (pv->can_shrink()) { if (!__vecm_x::nr_ok(&pv->rvecm())) { return _retval(q * 10 + 3, -880 - ind_mf); } }
                else { if (pv->nrsv() != __nr) { _retval(q * 10 + 4, -880 - ind_mf); } }
            }
            else if (pv->n() > __n)
            {
              if (pv->nrsv() != __nr && !__vecm_x::nr_ok(&pv->rvecm())) { return _retval(q * 10 + 5, -880 - ind_mf); }
            }
            else { return _retval(q * 10 + 6, -880 - ind_mf); }
          }

          ++q;
        }

        // Section 4. Additional functions.
        if (true)
        {
          typedef vec2_t<TA> Q;
          typedef vec2_t<T> Q2;
          bool b(false);

          m = rnd() * nmax; if (m < 10) { m = 10; }
            while (pv->n() < m)
            {
              if (!pv->el_append(_rnd_val<T, T2>::F())) { return -700; }
              if (pv->capacity() != pv->nrsv()) { return -767; }
            }
            if (pv->n() > m) { m = pv->n(); }



            // size, empty, clear, op.=, op[], iterators,
            //  at, front, back, push, pop
            //  vec2_delete
          if (true)
          {
            Q2* pv2(0);
            try { pv2 = new Q2(); } catch (...) {} if (!pv2) { return -701; }

            try { *pv2 = *pv; b = true; } catch (...) {}
              if (!b) { return pv2->vec2_delete() == 1 ? -702 : -722; }

            b = false;
            try {
              if (pv2->size() != m || pv2->n() != m || pv->size() != m) { return -703; }

              typename Q2::iterator b1 = pv2->begin();
              typename Q2::const_iterator b2 = pv2->begin();
              typename Q2::reverse_iterator b3 = pv2->rbegin();
              typename Q2::const_reverse_iterator b4 = const_cast<const Q2*>(pv2)->rbegin();

              typename Q::iterator a1 = pv->begin();
              typename Q::const_iterator a2 = pv->begin();
              typename Q::reverse_iterator a3 = pv->rbegin();
              typename Q::const_reverse_iterator a4 = const_cast<const Q*>(pv)->rbegin();

              for (s_long i = 0; i < pv->size(); ++i)
              {
                if (&*b1 != &*b2) { return _retval(i, -704); }
                if (&*b3 != &*b4) { return _retval(i, -705); }
                if (&*a1 != &*a2) { return _retval(i, -706); }
                if (&*a3 != &*a4) { return _retval(i, -707); }

                if (_eq<T, T2>::F(*a1, *b1) != 1) { return _retval(i, -708); }
                if (_eq<T, T2>::F(*a3, *b3) != 1) { return _retval(i, -709); }

                if (m % 2 == 0) { ++b1; ++b2; ++a1; ++a2; }

                if (i == (m - 1) / 2)
                {
                  if (&*a1 != &*a3) {
                  return _retval(i, -710);
                  }
                  if (&*a1 != &*a4) { return _retval(i, -711); }
                  if (&*b1 != &*b3) { return _retval(i, -712); }
                  if (&*b1 != &*b4) { return _retval(i, -713); }
                }

                ++b3; ++b4; ++a3; ++a4;
                if (m % 2 == 1) { ++b1; ++b2; ++a1; ++a2; }
              }

              if (a1 != pv->end()) { return -714; }
              if (a2 != pv->end()) { return -715; }
              if (a3 != pv->rend()) { return -716; }
              if (a4 != ((const vec2_t<TA>*)pv)->rend()) { return -717; }
              if (b1 != pv2->end()) { return -718; }
              if (b2 != pv2->end()) { return -719; }
              if (b3 != pv2->rend()) { return -720; }
              if (b4 != ((const vec2_t<TA>*)pv2)->rend()) { return -721; }

              if (&*(a1 - m) != &pv->front()) { return -734; }
              if (&*(a3 - m) != &pv->back()) { return -735; }
              if (&*(b1 - m) != &pv2->front()) { return -736; }
              if (&*(b3 - m) != &pv2->back()) { return -737; }

              if (&*(a1 - m) != &(*const_cast<const Q*>(pv)).front()) { return -738; }
              if (&*(a3 - m) != &(*const_cast<const Q*>(pv)).back()) { return -739; }
              if (&*(b1 - m) != &(*const_cast<const Q2*>(pv2)).front()) { return -740; }
              if (&*(b3 - m) != &(*const_cast<const Q2*>(pv2)).back()) { return -741; }

              s_long nb_prev = pv->nbase();
                pv->vec2_set_nbase(nb_prev + 10);

              if (&pv->front() != &(*pv)[0]) { return -742; }
              if (&pv->back() != &(*pv)[m-1]) { return -743; }
              if (&pv2->front() != &(*pv2)[0]) { return -744; }
              if (&pv2->back() != &(*pv2)[m-1]) { return -745; }

              if (&*(a3 - m / 3 - 1) != &(*pv)[m / 3]) { return -746; }
              if (&*(a4 - m / 3 - 1) != &(*pv)[m / 3]) { return -747; }
              if (&*(b3 - m / 3 - 1) != &(*pv2)[m / 3]) { return -748; }
              if (&*(b4 - m / 3 - 1) != &(*pv2)[m / 3]) { return -749; }

              if (&*(a3 - m / 3 - 1) != &(*const_cast<const Q*>(pv))[m / 3]) { return -750; }
              if (&*(a4 - m / 3 - 1) != &(*const_cast<const Q*>(pv))[m / 3]) { return -751; }
              if (&*(b3 - m / 3 - 1) != &(*const_cast<const Q2*>(pv2))[m / 3]) { return -752; }
              if (&*(b4 - m / 3 - 1) != &(*const_cast<const Q2*>(pv2))[m / 3]) { return -753; }

              if (&*(a3 - m / 3 - 1) != &pv->at(m / 3)) { return -754; }
              if (&*(a4 - m / 3 - 1) != &pv->at(m / 3)) { return -755; }
              if (&*(b3 - m / 3 - 1) != &pv2->at(m / 3)) { return -756; }
              if (&*(b4 - m / 3 - 1) != &pv2->at(m / 3)) { return -757; }

              if (&*(a3 - m / 3 - 1) != &(*const_cast<const Q*>(pv)).at(m / 3)) { return -758; }
              if (&*(a4 - m / 3 - 1) != &(*const_cast<const Q*>(pv)).at(m / 3)) { return -759; }
              if (&*(b3 - m / 3 - 1) != &(*const_cast<const Q2*>(pv2)).at(m / 3)) { return -760; }
              if (&*(b4 - m / 3 - 1) != &(*const_cast<const Q2*>(pv2)).at(m / 3)) { return -761; }

              a3 -= m / 3 + 1; a4 -= m / 3 + 1; b3 -= m / 3 + 1; b4 -= m /3 + 1;

              if (&*(a3 - m / 3) != &(*pv)[m / 3 + m / 3]) { return -762; }
              if (&*(a4 - m / 3) != &(*pv)[m / 3 + m / 3]) { return -763; }
              if (&*(b3 - m / 3) != &(*pv2)[m / 3 + m / 3]) { return -764; }
              if (&*(b4 - m / 3) != &(*pv2)[m / 3 + m / 3]) { return -765; }

              pv->vec2_set_nbase(nb_prev);

              b = true;
            } catch (...) {}
            if (!b) { return pv2->vec2_delete() == 1 ? -723 : -724; }

            if (pv2->vec2_delete() != 1) { return -725; }
            pv2 = 0;
            b = false; try { pv->at(-1); }  catch (...) { b = true; } if (!b) { return -727; }
            b = false; try { pv->at(m+1); }  catch (...) { b = true; } if (!b) { return -728; }
            pv->clear(); if (pv->n() != 0) { return -726; }
            b = false; try { pv->front(); }  catch (...) { b = true; } if (!b) { return -729; }
            b = false; try { pv->back(); }  catch (...) { b = true; } if (!b) { return -730; }
            b = false; try { pv->push_back(_rnd_val<T, T2>::F()); b = true; }  catch (...) {} if (!b) { return -731; }
            b = false; try { pv->pop_back(); b = true; }  catch (...) {} if (!b) { return -732; }
            b = false; try { pv->pop_back(); }  catch (...) { b = true; } if (!b) { return -733; }
            if (!pv->empty()) { return -766; }
          }



            // resize, reserve, capacity, swap
          if (true)
          {
            for (s_long i = 0; i < 10; ++i)
            {
              pv->vec2_setf_can_shrink(bool(i % 2));
              nr = rnd() * m;
              b = false; try { pv->reserve(nr); b = true; } catch (...) {} if (!b) { return _retval(nr, -768); }
              b = false; try { pv->reserve(-1); } catch (...) { b = true; } if (!b) { return -769; }
                if (pv->capacity() != pv->nrsv() || pv->capacity() < pv->size()) { return -770; }
              b = false; try { pv->reserve(0); b = true; } catch (...) {} if (!b) { return -771; }
                if (pv->capacity() != pv->nrsv() || pv->capacity() < pv->size()) { return -772; }
            }

            pv->clear();

            for (s_long i = 0; i < 5; ++i)
            {
              typedef typename meta::same_t<T, vecm, vec2_t<int>, vec2_t<TA> >::t Qx; Qx* pvx = (Qx*)pv;
              storage_t<T, T2> x91(1); if (!x91.inited) { return -773; }
              if (!meta::same_t<T, vecm>::result)
              {
                b = false; try { pvx->resize(nr); b = true; } catch (...) {} if (!b) { return _retval(nr, -774); }
                  for (s_long j = 0; j < pv->size(); ++j)
                  {
                    if (!meta::same_t<T, excgen>::result) { if (_eq<T, T2>::F(pv->at(j), x91) != 1) { return -775; } }
                      else { if (_echo<T, excgen>::F(pv->at(j)).a != _echo<T, excgen>::F(x91).a + j + 1) { return -842; } }
                  }
                b = false; try { pvx->resize(-1); } catch (...) { b = true; } if (!b) { return -776; }
                b = false; try { pvx->resize(0); b = true; } catch (...) {} if (!b || pv->size() != 0) { return -777; }
              }
              else { pv->clear(); }

              storage_t<T, T2> x92(0); if (x92.try_init(_rnd_val<T, T2>::F()) != 1) { return -778; }
              b = false; try { pv->resize(nr, x92); b = true; } catch (...) {} if (!b) { return _retval(nr, -779); }
                for (s_long j = 0; j < pv->size(); ++j) { if (_eq<T, T2>::F(pv->at(j), x92) != 1) { return -780; } }
              if (!meta::same_t<T, vecm>::result)
              {
                b = false; try { pvx->resize(0); b = true; } catch (...) {} if (!b || pv->size() != 0) { return -781; }
              }
              else { pv->clear(); }
            }

            m = rnd() * nmax; if (m < 10) { m = 10; }
              storage_t<T, T2> x91(1); if (!x91.inited) { return -800; }
              while (pv->n() < m) { if (!pv->el_append(*x91)) { return -782; } }
              if (pv->n() > m) { m = pv->n(); }

            if (pv->locality() == 1 && !meta::same_t<T, excgen>::result)
            {
              storage_t<T, T2> x92(0); if (x92.try_init(_rnd_val<T, T2>::F()) != 1) { return -783; }
              vec2_t<TA> v2;
                if (!v2.el_append(*x92)) { return -785; }
              b = false; try { v2.swap(*pv); b = true; } catch (...) {} if (!b) { return -786; }
                if (v2.size() != m || pv->size() != 1) { return -787; }
                if (_eq<T, T2>::F(v2.front(), x91) != 1) { return -788; }
                if (_eq<T, T2>::F(v2.back(), x91) != 1) { return -789; }
                if (_eq<T, T2>::F(pv->front(), x92) != 1) { return -790; }
              if (!v2.el_append(*x92) || v2.size() != m + 1) { return -791; }
              if (!pv->el_append(*x91) || pv->size() != 2) { return -792; }
              b = false; try { v2.swap(*pv); b = true; } catch (...) {} if (!b) { return -793; }
                if (_eq<T, T2>::F(v2.front(), x92) != 1) { return -794; }
                if (_eq<T, T2>::F(v2[v2.size()-2], x92) != 1) { return -795; }
                if (_eq<T, T2>::F(v2.back(), x91) != 1) { return -796; }
                if (_eq<T, T2>::F(pv->front(), x91) != 1) { return -797; }
                if (_eq<T, T2>::F(pv->at(pv->size()-2), x91) != 1) { return -798; }
                if (_eq<T, T2>::F(pv->back(), x92) != 1) { return -799; }
            }
          }

            // insert, erase, constructors with param.
          if (true)
          {
            storage_t<T, T2> x91(1); if (!x91.inited) { return -814; }
            storage_t<T, T2> x92(0); f.i = 12345;
              b = false; try { f.f(x92); x92.inited = true; b = true; } catch (...) {} if (!b) { return -801; }
            try
            {
              vec2_t<TA> v1(2, *x91);
              vec2_t<TA> v2(-5, 2, *x91);
              vec2_t<TA> v3(2, f);
              vec2_t<TA> v4(-5, 2, f);

              if (v1.size() != 2 || v1.nbase() != 0) { return -802; }
                if (!_eq<T, T2>::F(v1.front(), x91)) { return -803; }
                if (!_eq<T, T2>::F(v1.back(), x91)) { return -804; }
              if (v2.size() != 2 || v2.nbase() != -5) { return -805; }
                if (!_eq<T, T2>::F(v2.front(), x91)) { return -806; }
                if (!_eq<T, T2>::F(v2.back(), x91)) { return -807; }
              if (v3.size() != 2 || v3.nbase() != 0) { return -808; }
                if (!_eq<T, T2>::F(v3.front(), x92)) { return -809; }
                if (!_eq<T, T2>::F(v3.back(), x92)) { return -810; }
              if (v4.size() != 2 || v4.nbase() != -5) { return -811; }
                if (!_eq<T, T2>::F(v4.front(), x92)) { return -812; }
                if (!_eq<T, T2>::F(v4.back(), x92)) { return -813; }

              T *p;
                p = &*v1.insert(v1.begin(), x92);
                  if (v1.size() != 3) { return -816; }
                  if (p != &v1.front()) { return -817; }
                  if (!_eq<T, T2>::F(*p, x92)) { return -818; }
                v1.insert(v1.end(), 2, x92);
                  if (v1.size() != 5) { return -819; }
                  if (!_eq<T, T2>::F(v1.back(), x92)) { return -820; }
                v2.insert(++v2.begin(), v1.begin(), v1.end());
                  if (v2.size() != 7) { return -821; }
                  if (!_eq<T, T2>::F(v2.at(0), x91)) { return -840; }
                  if (!_eq<T, T2>::F(v2.at(1), x92)) { return -822; }
                  if (!_eq<T, T2>::F(v2.at(2), x91)) { return -823; }
                  if (!_eq<T, T2>::F(v2.at(3), x91)) { return -824; }
                  if (!_eq<T, T2>::F(v2.at(4), x92)) { return -825; }
                  if (!_eq<T, T2>::F(v2.at(5), x92)) { return -826; }
                  if (!_eq<T, T2>::F(v2.at(6), x91)) { return -827; }
                p = &*v2.erase(v2.begin() + 3);
                  if (v2.size() != 6) { return -828; }
                  if (p != &v2.at(3)) { return -829; }
                  if (!_eq<T, T2>::F(*p, x92)) { return -830; }
                v2.erase(v2.begin() + 1, v2.begin() + 4);
                  if (v2.size() != 3) { return -831; }
                  if (!_eq<T, T2>::F(v2.at(0), x91)) { return -832; }
                  if (!_eq<T, T2>::F(v2.at(1), x92)) { return -833; }
                  if (!_eq<T, T2>::F(v2.at(2), x91)) { return -834; }
                n = v1.size();
                b = false; try { v1.insert(v2.begin(), x91); } catch (...) { b = true; } if (!b || v1.size() != n) { return -835; }
                b = false; try { v1.insert(v2.begin(), 12, x91); } catch (...) { b = true; } if (!b || v1.size() != n) { return -836; }
                b = false; try { v1.insert(v1.begin(), v2.rend(), v2.rbegin()); } catch (...) { b = true; } if (!b || v1.size() != n) { return -837; }
                b = false; try { v1.erase(v2.begin()); } catch (...) { b = true; } if (!b || v1.size() != n) { return -838; }
                b = false; try { v1.erase(v2.begin(), v1.end()); } catch (...) { b = true; } if (!b || v1.size() != n) { return -839; }
                b = false; try { v1.erase(v1.begin(), v2.end()); } catch (...) { b = true; } if (!b || v1.size() != n) { return -841; }
            }
            catch (...) { return -815; }
          }
        }

        // Section 5. Permanent links.
        if (pv->locality() == 1)
        {
          if (true)
          {
            typedef vec2_t<TA> Q;

            m = pv->link2_setf(false, false, true);
              if (m < 0) { return -859; }
            for (s_long imode = 0; imode < 2; ++imode)
            {
              m = rnd() * nmax; if (m < 10) { m = 10; }
                while (pv->n() < m) { if (!pv->el_append(_rnd_val<T, T2>::F())) { return _retval(imode, -850); } }
                if (pv->n() > m) { m = pv->n(); }

              m = pv->link2_setf(true, imode % 2 == 1, false);
                if (m != 2) { return _retval(imode, -851); }
                if (!pv->f_perm()) { return _retval(imode, -852); }
                if (pv->f_sync() != (imode % 2 == 1)) { return _retval(imode, -853); }
              link2_t<TA> q1 = pv->link2(1);
              link2_t<TA> q2 = pv->link2(pv->n());
              link2_t<TA> q3 = pv->link2(-1);
                if (q1.is_empty() || q2.is_empty() || !q3.is_empty()) { return _retval(imode, -854); }
                if (!q1.pct() || !q2.pct() || q3.pct()) { return _retval(imode, -855); }
                  if (!q1.pv()) { return _retval(imode, -856); }
                  if (q2.pv()) { return _retval(imode, -857); }
                  if (q3.pv()) { return _retval(imode, -858); }
              if (!(q1.iter().is_valid() && !q1.iter().is_out() && &*q1.iter() == q1.pv() && &*q1.iter() == pv->pval_0u(1))) { return _retval(imode, -860); }
              if (!(q2.iter().is_valid() && q2.iter().is_aend())) { return _retval(imode, -861); }
              if (q3.iter().is_valid()) { return _retval(imode, -862); }

              if (!(q1.iter_c().is_valid() && !q1.iter_c().is_out() && &*q1.iter_c() == q1.pv() && &*q1.iter_c() == pv->pval_0u(1))) { return _retval(imode, -872); }
              if (!(q2.iter_c().is_valid() && q2.iter_c().is_aend())) { return _retval(imode, -873); }
              if (q3.iter_c().is_valid()) { return _retval(imode, -874); }

              m = pv->link2_setf(false, false, false); // try disable links (must fail, because not forced)
                if (m != -1) { return _retval(imode, -863); }

              q1 = link2_t<TA>(); q2 = link2_t<TA>(); q3 = link2_t<TA>(); // remove all existing links
              m = pv->link2_setf(false, false, false); // disable links (must succeed)
                if (m != 2) { return _retval(imode, -864); }

              m = pv->link2_setf(true, imode % 2 == 1, false); // reenable links
                if (m != 2) { return _retval(imode, -865); }
              q1 = pv->link2(2);
              q2 = pv->link2(0);
                  if (!(q1.pv() == &(*pv)[2] && q1.iter().pval() == q1.pv())) { return _retval(imode, -866); }
                  if (!(q2.pv() == &(*pv)[0] && q2.iter().pval() == q2.pv())) { return _retval(imode, -900); }
                if (pv->el_insert_1(pv->nbase(), _rnd_val<T, T2>::F()) != 1) { return _retval(imode, -919); }
                  if (!(q1.pv() == &(*pv)[3] && q1.iter().pval() == q1.pv())) { return _retval(imode, -920); }
                  if (!(q2.pv() == &(*pv)[1] && q2.iter().pval() == q2.pv())) { return _retval(imode, -921); }
                if (pv->el_remove_1(pv->nbase(), false) != 1) { return _retval(imode, -922); }
                  if (!(q1.pv() == &(*pv)[2] && q1.iter().pval() == q1.pv())) { return _retval(imode, -923); }
                  if (!(q2.pv() == &(*pv)[0] && q2.iter().pval() == q2.pv())) { return _retval(imode, -918); }
                if (pv->el_remove_1(pv->nbase(), false) != 1) { return _retval(imode, -867); }
                  if (!(q1.pv() == &(*pv)[1] && q1.iter().pval() == q1.pv())) { return _retval(imode, -868); }
                  if (q2.pv() || q2.iter().is_valid()) { return _retval(imode, -901); }
                if (pv->el_insert_ml(pv->nbase(), 15, _rnd_val<T, T2>::F()) != 15) { return _retval(imode, -876); }
                  if (!(q1.pv() == &(*pv)[16] && q1.iter().pval() == q1.pv())) { return _retval(imode, -877); }
                if (pv->el_remove_ml(pv->nbase() + 5, 10) != 10) { return _retval(imode, -878); }
                  if (!(q1.pv() == &(*pv)[6] && q1.iter().pval() == q1.pv())) { return _retval(imode, -879); }

              m = pv->link2_setf(false, false, false); // try disable links (must fail, because not forced)
                if (m != -1) { return _retval(imode, -869); }
              m = pv->link2_setf(false, false, true); // forced disabling (must succeed)
                if (m != 1) { return _retval(imode, -870); }
              if (q1.pv() || q1.iter().is_valid()) { return _retval(imode, -871); }

              q1 = pv->link2(1);
                if (q1.pv() || q1.iter().is_valid()) { return _retval(imode, -875); }

              m = pv->link2_setf(true, imode % 2 == 1, false); // reenable links
                if (m != 2) { return _retval(imode, -902); }

                // Create 1 link, clear / overwrite the container.
              q1 = pv->link2(1);
                if (!(q1.pv() == &(*pv)[1] && q1.iter().pval() == q1.pv())) { return _retval(imode, -903); }
              pv->clear();
                if (q1.pv() || q1.iter().is_valid()) { return _retval(imode, -904); }

              if (pv->el_append_m(10, _rnd_val<T, T2>::F()) != 10) { return _retval(imode, -905); }
                if (q1.pv() || q1.iter().is_valid()) { return _retval(imode, -906); }
              q1 = pv->link2(1);
                if (!(q1.pv() == &(*pv)[1] && q1.iter().pval() == q1.pv()))
                { return _retval(imode, -907); }
              *pv = Q();
                if (q1.pv() || q1.iter().is_valid()) { return _retval(imode, -908); }

                // Create 2 links, clear / overwrite the container.
              if (pv->el_append_m(10, _rnd_val<T, T2>::F()) != 10) { return _retval(imode, -909); }
              q1 = pv->link2(0);
              q2 = pv->link2(9);
                  if (!(q1.pv() == &(*pv)[0] && q1.iter().pval() == q1.pv())) { return _retval(imode, -910); }
                  if (!(q2.pv() == &(*pv)[9] && q2.iter().pval() == q2.pv())) { return _retval(imode, -911); }
              pv->clear();
                if (q1.pv() || q1.iter().is_valid()) { return _retval(imode, -912); }
                if (q2.pv() || q2.iter().is_valid()) { return _retval(imode, -913); }

              if (pv->el_append_m(10, _rnd_val<T, T2>::F()) != 10) { return _retval(imode, -914); }
              q1 = pv->link2(0);
              q2 = pv->link2(9);
                  if (!(q1.pv() == &(*pv)[0] && q1.iter().pval() == q1.pv())) { return _retval(imode, -915); }
                  if (!(q2.pv() == &(*pv)[9] && q2.iter().pval() == q2.pv())) { return _retval(imode, -916); }
              *pv = Q();
                if (q1.pv() || q1.iter().is_valid()) { return _retval(imode, -917); }
                if (q2.pv() || q2.iter().is_valid()) { return _retval(imode, -924); }

              m = 10;
                while (pv->n() < m) { if (!pv->el_append(_rnd_val<T, T2>::F())) { return _retval(imode, -925); } }

              link2_t<vec2_t<TA> > q4;
              vec2_t<vec2_t<TA> > vv;
                m = vv.link2_setf(true, imode % 2 == 1, false);
                if (m != 2) { return _retval(imode, -926); }
                if (vv.el_append_m(2, *pv) != 2) { return _retval(imode, -926); }
              q1 = vv.front().link2(1);
                if (!q1.is_empty()) { return _retval(imode, -927); }
              m = vv.front().link2_setf(true, !(imode % 2 == 1), false);
                if (m != 2) { return _retval(imode, -928); }
              q4 = vv.link2(0);
                if (!q4.is_empty()) { return _retval(imode, -929); }
              m = vv.front().link2_setf(true, imode % 2 == 1, false);
                if (m != 2) { return _retval(imode, -930); }
              m = vv.back().link2_setf(true, imode % 2 == 1, false);
                if (m != 2) { return _retval(imode, -931); }
              q4 = vv.link2(0);
                if (!(q4.pv() == &vv[0] && q4.iter().pval() == q4.pv())) { return _retval(imode, -944); }
              q1 = vv.front().link2(1);
              q2 = vv.front().link2(9);
              q3 = vv.back().link2(3);
                if (!(q1.pv() == &vv.front()[1] && q1.iter().pval() == q1.pv())) { return _retval(imode, -932); }
                if (!(q2.pv() == &vv.front()[9] && q2.iter().pval() == q2.pv())) { return _retval(imode, -933); }
                if (!(q3.pv() == &vv.back()[3] && q3.iter().pval() == q3.pv())) { return _retval(imode, -934); }
              if (vv.front().el_remove_1(vv.front().nbase() + 1, true) != 1) { return _retval(imode, -935); }
                if (q1.pv() || q1.iter().is_valid()) { return _retval(imode, -936); }
                if (!(q2.pv() == &vv.front()[1] && q2.iter().pval() == q2.pv())) { return _retval(imode, -936); }
                if (!(q3.pv() == &vv.back()[3] && q3.iter().pval() == q3.pv())) { return _retval(imode, -937); }
                if (!(q4.pv() == &vv[0] && q4.iter().pval() == q4.pv())) { return _retval(imode, -945); }
              if (vv.el_remove_1(0, true) != 1) { return _retval(imode, -938); }
                if (q1.pv() || q1.iter().is_valid()) { return _retval(imode, -939); }
                if (q2.pv() || q2.iter().is_valid()) { return _retval(imode, -940); }
                if (!(q3.pv() == &vv.back()[3] && q3.iter().pval() == q3.pv())) { return _retval(imode, -941); }
                if (q4.pv() || q4.iter().pval()) { return _retval(imode, -942); }
              if (vv.el_remove_1(0, true) != 1) { return _retval(imode, -943); }
                if (q3.pv() || q3.iter().is_valid()) { return _retval(imode, -946); }

              pv->clear();
            }
          }

          if (true)
          {
            pv->clear();
            m = 20; while (pv->n() < m) { if (!pv->el_append(_rnd_val<T, T2>::F())) { return -950; } }

            m = pv->link2_setf(false, false, false);
              if (m != 2) { return -947; }
            m = pv->link2_setf(true, true, false);
              if (m != 2) { return -948; }

  // Permanent links test sequence:
  //  1. Test starts multiple linktest_thread1.
  //        Each at once creates a link to randomly chosen element,
  //        and performs its validity checks in a loop during 2 * duration_ms.
  //        Some checks must give errors, and these errors are counted.
  //  2. Test waits while all linktest_thread1 create a link (max. till timeout_ms).
  //  3. Test starts single linktest_thread2.
  //  4. During duration_ms, linktest_thread2 makes random modifications to the target container,
  //    keeping the initially linked values unchanged.
  //  5. linktest_thread2 exits.
  //  6. Test signals to linktest_thread1 that test has ended.
  //  7. Test wait all linktest_thread1 exit (max. till timeout_ms).
  //  8. Test checks errors, accumulated during threads working.

            double t1(0);
            s_long res(-953);

            linktest_ctx ctx(pv, log);
            bmdx::threadctl* pthread2(0);

            ctx.ind_stage_sync = 1;

            vec2_t<bmdx::threadctl> threads;
              if (threads.el_append_m(linktest_ctx::n_threads1, meta::construct_f<bmdx::threadctl>()) <= 0) { res = -951; goto lThErr1; }

            for (s_long i = 0; i < linktest_ctx::n_threads1; ++i)
            {
              if (!threads[i].start_auto<linktest_thread1>(&ctx)) { res = -956; goto lThErr1; }
            }


            t1 = bmdx::clock_ms();
            while (bmdx::clock_ms() - t1 < linktest_ctx::timeout_ms)
            {
              if (ctx.n_linked_sync == linktest_ctx::n_threads1) { goto lThStage2; }
              if (ctx.n_finished_sync > 0) { res = -954; goto lThErr1; } // one or more threads got error during validity test
              bmdx::sleep_mcs(10000);
            }
            res = -959;
            goto lThErr1;

lThStage2:
            ctx.ind_stage_sync = 2;

            if (!threads.el_append(meta::construct_f<bmdx::threadctl>())) { res = -957; goto lThErr1; }
            pthread2 = &threads[threads.n() -1];
            if (!pthread2->start_auto<linktest_thread2>(&ctx)) { res = -962; goto lThErr1; }
            pthread2->set_priority(5);

            res = -958;

            t1 = bmdx::clock_ms();
            while (bmdx::clock_ms() - t1 < linktest_ctx::timeout_ms)
            {
              if (ctx.n_finished_sync == linktest_ctx::n_threads1) { pthread2->stop(-1); }
              if (ctx.n_finished_sync == linktest_ctx::n_threads1 && pthread2->state() != 2) { goto lThExitTest; }
              bmdx::sleep_mcs(10000);
            }
            res = -961;
            goto lThErr1;

lThErr1:
            ctx.ind_stage_sync = 3;
            for (s_long i = 0; i < threads.n(); ++i) { threads[i].stop(-1); }
            return res;

lThExitTest:
            ctx.ind_stage_sync = 3;
              for (s_long i = 0; i < threads.n(); ++i) { threads[i].stop(-1); }
              if (ctx.err_bits() != 0)
              {
                log.d, "  Perm. link test errors:", ctx.cnt_err1, ctx.cnt_err2, ctx.cnt_err3, ctx.cnt_err4, ctx.cnt_err5, ctx.cnt_err6, ctx.cnt_err7;
                return _retval(ctx.err_bits(), -970);
              }
            m = pv->link2_setf(false, false, false);
              if (m != 2) { return -949; }
            log.d, "  Perm. link test stats: modifications, checks, sync. errors (<checks*0.1).", ctx.cntx_0, ctx.cntx_1, ctx.cntx_2;
          }

        }
        return 1;
      }
    };

    int test_vec2()
    {
      rnd_0() = rnd_00();
      s_long res = 0; s_long i_t = 0;
      enum { N = 1000, Q = 10000, N2 = 300000, Q2 = 1000, N3 = 1000, Q3 = 1000 };
      do
      {
          // passing a null pointer
        if (true) { ++i_t; res = test_vec2_t<s_long, meta::nothing>::F(log, 0, 1, 1); if (res < 0) { break; } }

          // char
        log, endl;
        if (true) { ++i_t; typedef char T; vec2_t<T> v; res = test_vec2_t<T, meta::nothing>::F(log, &v, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef char T; vec2_t<T> v(-1, 0, char(0)); res = test_vec2_t<T, meta::nothing>::F(log, &v, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef char T; vec2_t<T> v(meta::s_long_mp, 0, char(0)); res = test_vec2_t<T, meta::nothing>::F(log, &v, N2, Q2); if (res < 0) { break; } }

          // other types
        log, endl;
        if (true) { ++i_t; typedef s_long T; vec2_t<T> v; res = test_vec2_t<T, meta::nothing>::F(log, &v, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef double T; vec2_t<T> v; res = test_vec2_t<T, meta::nothing>::F(log, &v, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef signed long long int T; vec2_t<T> v; res = test_vec2_t<T, meta::nothing>::F(log, &v, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef long double T; vec2_t<T> v; res = test_vec2_t<T, meta::nothing>::F(log, &v, N, Q); if (res < 0) { break; } }
        if (true) { ++i_t; typedef std::string T; vec2_t<T> v; res = test_vec2_t<T, meta::nothing>::F(log, &v, N, Q / 2); if (res < 0) { break; } }
        if (true) { ++i_t; typedef S3 T; vec2_t<T> v; res = test_vec2_t<T, meta::nothing>::F(log, &v, N, Q); if (res < 0) { break; } }

          // exception generator as elem.
        log, endl;
        if (true) { ++i_t; typedef excgen T; vec2_t<T> v; res = test_vec2_t<T, meta::nothing>::F(log, &v, N3, Q3); if (res < 0) { break; } }
        if (true) { ++i_t; typedef excgen_nt T; vec2_t<T> v; res = test_vec2_t<T, meta::nothing>::F(log, &v, N3, Q3); if (res < 0) { break; } }
        if (true) { ++i_t; typedef excgen_mmode3 T; vec2_t<T> v; res = test_vec2_t<T, meta::nothing>::F(log, &v, N3, Q3); if (res < 0) { break; } }

          // vec2_t<various types> as elem.
        log, endl;
        if (true) { ++i_t; typedef char T2; vec2_t<vecm> v; res = test_vec2_t<vecm, T2>::F(log, &v, N3, Q3); if (res < 0) { break; } }
        if (true) { ++i_t; typedef s_long T2; vec2_t<vecm> v; res = test_vec2_t<vecm, T2>::F(log, &v, N3, Q3); if (res < 0) { break; } }
        if (true) { ++i_t; typedef std::string T2; vec2_t<vecm> v; res = test_vec2_t<vecm, T2>::F(log, &v, N3, Q3); if (res < 0) { break; } }
        if (true) { ++i_t; typedef hashx<char, s_long> T2; vec2_t<vecm> v; res = test_vec2_t<vecm, T2>::F(log, &v, N3, Q3); if (res < 0) { break; } }
        if (true) { ++i_t; typedef hashx<s_long, s_long> T2; vec2_t<vecm> v; res = test_vec2_t<vecm, T2>::F(log, &v, N3, Q3); if (res < 0) { break; } }
        if (true) { ++i_t; typedef hashx<std::string, s_long> T2; vec2_t<vecm> v; res = test_vec2_t<vecm, T2>::F(log, &v, N3, Q3); if (res < 0) { break; } }
        if (true) { ++i_t; typedef hashx<std::string*, s_long> T2; vec2_t<vecm> v; res = test_vec2_t<vecm, T2>::F(log, &v, N3, Q3); if (res < 0) { break; } }
      } while(false);
      if (res < 0) { log, "err. test_vec2_t[", i_t, "]", endl; log.flush(); return res; }

      return 1;
    }

    // ===================================================
    // Entry point for full test sequence.
    // ===================================================

    int do_test(bool is_full, const char* prnd0)
    {
      full_test() = is_full;
      rnd_00() = prnd0 ? atol(prnd0) : std::time(0);
      rnd_0() = rnd_00();

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

#if !defined(bmdx_test_sections) || (bmdx_test_sections & 4)
      if (stat.exec_test(&test_experimental::test_ordhs, "test_ordhs()") < 0) { goto lExit; }
      stat.exec_test(&test_experimental::perf_ordhs, "perf_ordhs()");

#endif

#if !defined(bmdx_test_sections) || (bmdx_test_sections & 8)
#if !(defined(__mips) && defined(__ANDROID__))
      if (stat.exec_test(&test_experimental::test_vec2, "test_vec2()") < 0) { goto lExit; }
#endif
      _link2_reg::reg_clear(); // removes extra reserves from perm. links registry
      txtsample(true); // removes test text sample
#endif

lExit:
      double dt = double(s_long((bmdx::clock_ms() - t0) / 100.)) / 10.; if (dt >= 50.) { dt = double(s_long(dt)); }
      log, "TESTING COMPLETED. Total: on_calm_result ", stat.__ns, ", failure ", stat.__nf, ", skipped ", stat.__ni, ". Taken ", dt, " s.", endl, endl; log.flush();
      if (prevLocaleName.length()) { std::setlocale(LC_CTYPE, prevLocaleName.c_str()); }

      return stat.__nf;
    }
  };
}

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif
