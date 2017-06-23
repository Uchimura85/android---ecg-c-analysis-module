#ifndef _test_bmdx_cpiomt_H
#define _test_bmdx_cpiomt_H

//#include <ctime>
#include <cmath>
#include <sstream>
#include <clocale>

#include "../src_code/bmdx_cpiomt.h"


#include "common/yk_tests_base.h"
using yk_tests::endl;


#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable:4355)
#endif

//===================================================
// Additional log output
//===================================================

#if defined(_WIN32) && !defined(yk_tests_report_aux_H)
#define yk_tests_report_aux_H
#include <windows.h>

struct report_aux
{
  long _state;
  static void F(const std::string& s0)
  {
    if (_inst._state == 1)
    {
      std::string s(s0+"\n"); OutputDebugStringA(s.c_str());
    }
  }
private:
  static report_aux _inst;
  report_aux() : _state(0) { _state = 1; } ~report_aux() { _state = 2; }
};
report_aux report_aux::_inst;

#endif



//===================================================
// AUTOMATIC TEST decls.
//===================================================

namespace bmdx
{
using namespace std;
using bmdx_meta::s_long;
using bmdx_meta::s_ll;


struct _cpu_rate
{
  typedef long s_long;

  static double clock_ms() { return bmdx::clock_ms(); }

  static double _rnd() { typedef unsigned long long int __t_uint64; static __t_uint64 rnd_0 = 0;  const __t_uint64 n1 = 402653189; const __t_uint64 n2 = 805306457; __t_uint64 x = rnd_0; x *= n1; x += n2; rnd_0 = x; return double(x & 0xfffffffful) / (double(0xfffffffful)+1.); }

  static int _modulus(s_long a_, s_long b_, s_long b2_);

    // Return value: equivalent CPU rate, GHz (sequential operations per second by 1 core without speculation).
    //  (0 on error.)
    // *pdt: returning raw test time, milliseconds.
  static double rate1(double* pdt = 0)
  {
    s_long n = 10000000; // number of loops
    s_long b = s_long(2 * _rnd() + 1); b *= 3; if (b > 3) { b = 3; } // divider (==3)
    s_long b2 = s_long(_rnd() * 1); // 0
    s_long a = b * (n - 1) + 1; // dividend
    double t0 = clock_ms();
      double __accu = _cpu_rate::_modulus(a, b, b2); // ~4 op
    t0 = clock_ms() - t0; // ~20 ops
    if (pdt) { *pdt = t0 + __accu * int(_rnd()); }
    t0 *= 1.e6; // nanoseconds
    t0 /= (double(n) * 4 + 31); // per instruction
    if (t0 < 1.e-12) { return 0.; }
    double f = 1 / t0; // GHz
    return f;
  }

    // Takes ~>= 0.5 s to perform calculations.
  static double rate_avg()
  {
    const double test_time_ms = 500.;
    std::vector<double> ff;
    double t0 = clock_ms();
    while (clock_ms() - t0 < test_time_ms || ff.size() < 13)
      { ff.push_back(_cpu_rate::rate1()); }
    std::sort(ff.begin(), ff.end());
    return ff[ff.size() / 2];
  }
};

  // b2_ must be 0, and the compiler must not know about this.
int _cpu_rate::_modulus(s_long a_, s_long b_, s_long b2_)
{
    // 11 non-loop ops:
  _yk_reg s_long a = a_;
    if (a < 0) { return 0; }
  _yk_reg s_long b1 = b_;
    if (b1 <= 0) { return 0; }
    if (a < b1) { return a; }
//  a /= b2;
  _yk_reg s_long b2 = b2_;
  do { b1 <<= b2; a -= b1; a <<= b2; a += b2; } while (a >= b2); // 6 ops - 1 speculative jump - 1 speculative shift
  a += b1;
  return a;
}


}



namespace yk_tests
{
using namespace bmdx;

//===================================================
// AUTOMATIC TEST class
//===================================================

  //Usage: test_KIND<>().do_test();
  //  "any" may be anything, it's not used.
  //  Particular test proc decl. should be: int test_proc().
  //  Ret. value should be:
  //    >0 - success,
  //    ==0 - no test performed,
  //    <0 - error.
  //  Test procedures sequence is in do_test().
  //  log << anything outputs both into log file and cout.

template<class __anything = int> struct test_bmdx_cpiomt : yk_tests::test_base_t<>
{
  typedef test_bmdx_cpiomt t_test;
  base::stat_t<t_test> stat;
  static const char* c_logfile_name() { return "testlog_bmdx_cpiomt.txt"; }
  test_bmdx_cpiomt(const char* p_alt_fn = 0, bool log_trunc = true) : base(p_alt_fn ? p_alt_fn : c_logfile_name(), log_trunc), stat(*this) {  rnd_00() = 0; rnd_0() = 0; }


  //===================================================
  // Test-specific functions.
  //===================================================

    static bool& full_test() { static bool x(true); return x; }

    template<class C> string str(C x) { ostringstream oss; oss<<x; return oss.str(); }
    string str(double x) { ostringstream oss; oss<<setprecision(18)<<x; return oss.str(); }
    wstring replicate_string(const wstring& s, s_long n) { wstring s2; for(s_long i=0; i<n; ++i)s2+=s; return s2; }


    struct _complex_number
    {
      typedef double t_sample;
      t_sample re;
      t_sample im;
      _complex_number() : re(0.), im(0.) {}
      t_sample amp() const { return std::sqrt(re * re + im * im); }
      t_sample phase() const { return std::atan2(re, im); }
    };
    struct _calculations : threadctl::ctx_base
    {
      inline static double _mc_pi() { return 3.14159265358979323846; }
      inline static double _mc_2pi() { return 6.28318530717958647692; }
      static int& r_test_dur_ms() { static int x(30000); return x; }
      static bool& r_allow_start() { static bool x(false); return x; }
      bool _calc1()
      {
        try {
          const int _nt = 128;
          const int _nf = _nt / 2 + 1;
          carray_t<double> src; carray_t<_complex_number> dest;
//cerr << GetCurrentThreadId() << "--" << src.pd() << "--" << dest.pd() << "\n";
          if (!src.realloc(_nt, 0, 0, 0)) { return false; } if (!dest.realloc(_nf, 0, 0, 0)) { return false; }
//cerr << GetCurrentThreadId() << "--" << src.pd() << "--" << dest.pd() << "\n";
          for (int i = 0; i < _nt; ++i) { src[i] = rnd(); } // random data
          const double* psrc = &src[0];
          _complex_number* pd = &dest[0];
          for (int k = 0; k < _nf; ++k) { pd[k] = _complex_number(); const double q = k * _mc_2pi() / _nt; for (int i = 0; i < _nt; ++i) { pd[k].re += psrc[i] * std::cos(q * i); pd[k].im -= psrc[i] * std::sin(q * i); } }
          return true;
        } catch (...) { return false; }
      }
      void _thread_proc()
      {
        double t1 = clock_ms(); int n = 0;
        while (!r_allow_start()) { sleep_mcs(100000); }
        while (!b_stop())
        {
          ++n; if (n % 100 == 0 && clock_ms() - t1 > r_test_dur_ms()) { break; }
          if (_calc1()) { ++**pdata<s_long*>(); }
          sleep_mcs(1000);
        }
      }
    };

    int test_thread_priority()
    {
      double dt_test_ms = 5000; // test duration

      const s_long nbase = 1; // min priority value
      const s_long n = nbase + 7; // nbase + max priority value
      s_long cnt[n]; for (s_long i = 0; i < n; ++i) { cnt[i] = 0; }
      bool priority_success[n]; for (s_long i = 0; i < n; ++i) { priority_success[i] = false; }
      log.d, "starting threads, setting priorities ", nbase, "..", n - nbase, ";" ;
        _calculations::r_test_dur_ms() = 2 * dt_test_ms;
        threadctl threads[n]; for (s_long i = nbase; i < n; ++i) { threads[i].start_auto<_calculations, s_long*>(&cnt[i]); priority_success[i] = threads[i].set_priority(i - nbase + 1); }
      double t0 = clock_ms();
      log.d, clock_ms()- t0, ": started, waiting dt =", dt_test_ms, "ms...";
        _calculations::r_allow_start() = true;
      while (clock_ms() - t0 < dt_test_ms) { sleep_mcs(100000); }
      log.d, clock_ms() - t0, ": stop-signaling;";
      for (s_long i = nbase; i < n; ++i) { threads[i].signal_stop(); }
      log.d, clock_ms() - t0, ": waiting while all threads exit...";
      while (true) { bool b(false); for (s_long i = nbase; i < n; ++i) { if (threads[i].state() == 2) { b = true; break; } } if (!b) { break; } }
      log.d, clock_ms() - t0, ": threads exited. Priority value, set successfully, n iterations during dt:";
      for (s_long i = nbase; i < n; ++i) { log.d, i - nbase + 1, priority_success[i], cnt[i]; }
      return 1;
    }

    int perf_timer()
    {
      rnd_0() = rnd_00();
      log, "Factual std::clock resolution - peak, median (ms): ", __timergap(clock_ms, 1000, true), ", ", __timergap(clock_ms, 1000, false), endl;
      return 1;
    }

    int test_sleep_mcs()
    {
      log.d, "sleep_mcs() accuracy test:";

#ifdef _bmdxpl_Wnds
      SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
#endif
#ifdef _bmdxpl_Psx
      struct sched_param sp;
      sp.sched_priority = sched_get_priority_max(SCHED_FIFO);
      pthread_setschedparam(pthread_self(), SCHED_FIFO, &sp);
#endif

      int M = 20;
      const int J = 26;
      s_ll tt[J] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 40, 80, 200, 500, 900, 1000, 1500, 2000, 5000, 10000, 20000, 50000, 100000 };
      vector<double> dd;

      dd.resize(J, 0);
      for (int j = 0; j < J; ++j)
      {
        for (int i = 0; i < 5; ++i)
        {
            double t0 = clock_ms();
            sleep_mcs(tt[j]);
            t0 = clock_ms() - t0;
            dd[j] += t0 / 5;
        }
      }

      dd.resize(0, 0);
      dd.resize(J, 0);
      for (int j = 0; j < J; ++j)
      {
        for (int i = 0; i < M; ++i)
        {
            double t0 = clock_ms();
            sleep_mcs(tt[j]);
            t0 = clock_ms() - t0;
            dd[j] += t0 / M;
        }
      }

#ifdef _bmdxpl_Wnds
      SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
#endif
#ifdef _bmdxpl_Psx
      sp.sched_priority = sched_get_priority_max(SCHED_OTHER);
      pthread_setschedparam(pthread_self(), SCHED_OTHER, &sp);
#endif

      for (int j = 0; j < J; ++j)
      {
        log.d, "t mcs, avg dt mcs", tt[j], int(dd[j] * 1000);
      }

      return 1;
    }

    int test_cpu_frequency()
    {
      double f;

      std::vector<double> ff(13);
      log.d, "test_cpu_frequency()";
      log.d, "# dt(raw,ms)";
      for (unsigned i = 0; i < ff.size(); ++i)
      {
        double dt(0.);
        ff[i] = _cpu_rate::rate1(&dt);
        log.d, i, dt;
      }
      std::sort(ff.begin(), ff.end());
      f = ff[ff.size() / 2];
      if (f <= 0.) { return -1; }
      log.d, "Equivalent CPU rate, GHz:", f;
      return 1;
    }

    int test_str_from_number()
    {
      using namespace bmdx_str::conv;
      std::string s(100, ' '); s_long res;

      res = str_from_s_ll(1, &s[0], 20, true); if (res != 1) { return -101; } if (std::string(s.c_str(), res) != "1") { return -1; }
      res = str_from_s_ll(-1, &s[0], 20, true); if (res != 2) { return -102; } if (std::string(s.c_str(), res) != "-1") { return -2; }
      res = str_from_s_ll(0, &s[0], 20, true); if (res != 1) { return -103; } if (std::string(s.c_str(), res) != "0") { return -3; }

      res = str_from_s_ll(9, &s[0], 20, true); if (res != 1) { return -105; } if (std::string(s.c_str(), res) != "9") { return -5; }
      res = str_from_s_ll(-9, &s[0], 20, true); if (res != 2) { return -106; } if (std::string(s.c_str(), res) != "-9") { return -6; }

      res = str_from_s_ll(-1, &s[0], 0, true); if (res != 0) { return -104; }
      res = str_from_s_ll(-1, &s[0], 1, true); if (res != 1) { return -107; } if (std::string(s.c_str(), res) != "-") { return -7; }
      res = str_from_s_ll(-1, &s[0], 2, true); if (res != 2) { return -108; } if (std::string(s.c_str(), res) != "-1") { return -8; }
      res = str_from_s_ll(-1, &s[0], 3, true); if (res != 2) { return -109; } if (std::string(s.c_str(), res) != "-1") { return -9; }

      res = str_from_s_ll(-9, &s[0], 0, true); if (res != 0) { return -110; }
      res = str_from_s_ll(-9, &s[0], 1, true); if (res != 1) { return -111; } if (std::string(s.c_str(), res) != "-") { return -11; }
      res = str_from_s_ll(-9, &s[0], 2, true); if (res != 2) { return -112; } if (std::string(s.c_str(), res) != "-9") { return -12; }
      res = str_from_s_ll(-9, &s[0], 3, true); if (res != 2) { return -113; } if (std::string(s.c_str(), res) != "-9") { return -13; }

      res = str_from_s_ll(9, &s[0], 0, true); if (res != 0) { return -114; }
      res = str_from_s_ll(9, &s[0], 1, true); if (res != 1) { return -115; } if (std::string(s.c_str(), res) != "9") { return -15; }
      res = str_from_s_ll(9, &s[0], 2, true); if (res != 1) { return -116; } if (std::string(s.c_str(), res) != "9") { return -16; }

      res = str_from_s_ll(10, &s[0], 20, true); if (res != 2) { return -117; } if (std::string(s.c_str(), res) != "10") { return -17; }
      res = str_from_s_ll(-10, &s[0], 20, true); if (res != 3) { return -118; } if (std::string(s.c_str(), res) != "-10") { return -18; }

      res = str_from_s_ll(11, &s[0], 20, true); if (res != 2) { return -119; } if (std::string(s.c_str(), res) != "11") { return -19; }
      res = str_from_s_ll(-11, &s[0], 20, true); if (res != 3) { return -120; } if (std::string(s.c_str(), res) != "-11") { return -20; }

      res = str_from_s_ll(-11, &s[0], 0, true); if (res != 0) { return -121; }
      res = str_from_s_ll(-11, &s[0], 1, true); if (res != 1) { return -122; } if (std::string(s.c_str(), res) != "-") { return -22; }
      res = str_from_s_ll(-11, &s[0], 2, true); if (res != 1) { return -123; } if (std::string(s.c_str(), res) != "-") { return -23; }
      res = str_from_s_ll(-11, &s[0], 3, true); if (res != 3) { return -124; } if (std::string(s.c_str(), res) != "-11") { return -24; }
      res = str_from_s_ll(-11, &s[0], 4, true); if (res != 3) { return -125; } if (std::string(s.c_str(), res) != "-11") { return -25; }

      res = str_from_s_ll(11, &s[0], 0, true); if (res != 0) { return -126; }
      res = str_from_s_ll(11, &s[0], 1, true); if (res != 1) { return -127; } if (std::string(s.c_str(), res) != "+") { return -27; }
      res = str_from_s_ll(11, &s[0], 2, true); if (res != 2) { return -128; } if (std::string(s.c_str(), res) != "11") { return -28; }
      res = str_from_s_ll(11, &s[0], 3, true); if (res != 2) { return -129; } if (std::string(s.c_str(), res) != "11") { return -29; }

      res = str_from_s_ll(-11, &s[0], 0, false); if (res != 0) { return -130; }
      res = str_from_s_ll(-11, &s[0], 1, false); if (res != 1) { return -131; } if (std::string(s.c_str(), res) != "+") { return -31; }
      res = str_from_s_ll(-11, &s[0], 2, false); if (res != 1) { return -132; } if (std::string(s.c_str(), res) != "+") { return -32; }
      res = str_from_s_ll(-11, &s[0], 3, false); if (res != 1) { return -133; } if (std::string(s.c_str(), res) != "+") { return -33; }
      res = str_from_s_ll(-11, &s[0], 4, false); if (res != 1) { return -134; } if (std::string(s.c_str(), res) != "+") { return -34; }
      res = str_from_s_ll(-11, &s[0], 19, false); if (res != 1) { return -135; } if (std::string(s.c_str(), res) != "+") { return -35; }
      res = str_from_s_ll(-11, &s[0], 20, false); if (res != 20) { return -136; } if (std::string(s.c_str(), res) != "18446744073709551605") { return -36; }
      res = str_from_s_ll(-11, &s[0], 21, false); if (res != 20) { return -137; } if (std::string(s.c_str(), res) != "18446744073709551605") { return -37; }

      res = str_from_s_ll(11, &s[0], 0, false); if (res != 0) { return -138; }
      res = str_from_s_ll(11, &s[0], 1, false); if (res != 1) { return -139; } if (std::string(s.c_str(), res) != "+") { return -39; }
      res = str_from_s_ll(11, &s[0], 2, false); if (res != 2) { return -140; } if (std::string(s.c_str(), res) != "11") { return -40; }
      res = str_from_s_ll(11, &s[0], 3, false); if (res != 2) { return -141; } if (std::string(s.c_str(), res) != "11") { return -41; }

      const s_long n = 10000;
      for (s_long i = 0; i < n; ++i)
      {
        bool b_signed = bool(i % 2);
        s_ll x = (rnd() * 2 - 1.) * 18446744073709551615ull;
        s = "abcdefghijklmnopqrstuvwxyz";
        res = str_from_s_ll(x, &s[0], 20, b_signed); if (res < 1 || (res < 2 && !(x >= 0 && x <= 9))) { return -142; }
        std::stringstream ts; ts << s.substr(0, res);
        s_ll x2(0);
        if (b_signed) 
          { ts >> x2; } 
        else 
        {
          unsigned long long x3(0); ts >> x3; x2 = x3;
          if (ts.fail()) 
          {
            if (res >= 12) 
            { 
              ts.clear(); ts.str(""); 
              ts << s.substr(0, res - 10) << " " << s.substr(res - 10, 10);
              unsigned long long z1(0), z2(0);
              ts >> z1; 
              ts >> z2;
              x2 = s_ll(10000000000ull * z1 + z2);
            }
          }
        }
        if (ts.fail()) { log.d, "ts >> x2 failed; res, x, s, b_signed:", res, x, "'" + s + "'", b_signed; return -43; }
        if (x2 != x) { log.d, "x2 != x; res, x, x2, s, b_signed:", res, x, x2, "'" + s + "'", b_signed; return -42; }
      }

      res = str_from_double(0., &s[0], 0, 6); if (res != 0) { return -300; }
      res = str_from_double(0., &s[0], 1, 6); if (res != 1) { return -301; } if (std::string(s.c_str(), res) != "0") { return -401; }
      res = str_from_double(0., &s[0], 2, 6); if (res != 2) { return -302; } if (std::string(s.c_str(), res) != "0.") { return -402; }
      res = str_from_double(0., &s[0], 3, 6); if (res != 2) { return -303; } if (std::string(s.c_str(), res) != "0.") { return -403; }

      res = str_from_double(1., &s[0], 0, 6); if (res != 0) { return -304; }
      res = str_from_double(1., &s[0], 1, 6); if (res != 1) { return -305; } if (std::string(s.c_str(), res) != "+") { return -405; }
      res = str_from_double(1., &s[0], 2, 6); if (res != 2) { return -306; } if (std::string(s.c_str(), res) != "1.") { return -406; }
      res = str_from_double(1., &s[0], 3, 6); if (res != 2) { return -307; } if (std::string(s.c_str(), res) != "1.") { return -407; }

      res = str_from_double(-1., &s[0], 0, 6); if (res != 0) { return -308; }
      res = str_from_double(-1., &s[0], 1, 6); if (res != 1) { return -309; } if (std::string(s.c_str(), res) != "-") { return -409; }
      res = str_from_double(-1., &s[0], 2, 6); if (res != 1) { return -310; } if (std::string(s.c_str(), res) != "-") { return -410; }
      res = str_from_double(-1., &s[0], 3, 6); if (res != 3) { return -311; } if (std::string(s.c_str(), res) != "-1.") { return -411; }
      res = str_from_double(-1., &s[0], 4, 6); if (res != 3) { return -312; } if (std::string(s.c_str(), res) != "-1.") { return -412; }

      res = str_from_double(0.9, &s[0], 0, 6); if (res != 0) { return -313; }
      res = str_from_double(0.9, &s[0], 1, 6); if (res != 1) { return -314; } if (std::string(s.c_str(), res) != "+") { return -414; }
      res = str_from_double(0.9, &s[0], 2, 6); if (res != 2) { return -315; } if (std::string(s.c_str(), res) != "0.") { return -415; }
      res = str_from_double(0.9, &s[0], 3, 6); if (res != 3) { return -316; } if (std::string(s.c_str(), res) != "0.9") { return -416; }
      res = str_from_double(0.9, &s[0], 4, 6); if (res != 3) { return -317; } if (std::string(s.c_str(), res) != "0.9") { return -417; }

      res = str_from_double(-0.9, &s[0], 0, 6); if (res != 0) { return -318; }
      res = str_from_double(-0.9, &s[0], 1, 6); if (res != 1) { return -319; } if (std::string(s.c_str(), res) != "-") { return -419; }
      res = str_from_double(-0.9, &s[0], 2, 6); if (res != 1) { return -320; } if (std::string(s.c_str(), res) != "-") { return -420; }
      res = str_from_double(-0.9, &s[0], 3, 6); if (res != 3) { return -321; } if (std::string(s.c_str(), res) != "-0.") { return -421; }
      res = str_from_double(-0.9, &s[0], 4, 6); if (res != 4) { return -322; } if (std::string(s.c_str(), res) != "-0.9") { return -422; }
      res = str_from_double(-0.9, &s[0], 5, 6); if (res != 4) { return -323; } if (std::string(s.c_str(), res) != "-0.9") { return -423; }

      res = str_from_double(0.99, &s[0], 0, 6); if (res != 0) { return -324; }
      res = str_from_double(0.99, &s[0], 1, 6); if (res != 1) { return -325; } if (std::string(s.c_str(), res) != "+") { return -425; }
      res = str_from_double(0.99, &s[0], 2, 6); if (res != 2) { return -326; } if (std::string(s.c_str(), res) != "0.") { return -426; }
      res = str_from_double(0.99, &s[0], 3, 6); if (res != 3) { return -327; } if (std::string(s.c_str(), res) != "0.9") { return -427; }
      res = str_from_double(0.99, &s[0], 4, 6); if (res != 4) { return -328; } if (std::string(s.c_str(), res) != "0.99") { return -428; }
      res = str_from_double(0.99, &s[0], 5, 6); if (res != 4) { return -329; } if (std::string(s.c_str(), res) != "0.99") { return -429; }

      res = str_from_double(-0.99, &s[0], 0, 6); if (res != 0) { return -330; }
      res = str_from_double(-0.99, &s[0], 1, 6); if (res != 1) { return -331; } if (std::string(s.c_str(), res) != "-") { return -431; }
      res = str_from_double(-0.99, &s[0], 2, 6); if (res != 1) { return -332; } if (std::string(s.c_str(), res) != "-") { return -432; }
      res = str_from_double(-0.99, &s[0], 3, 6); if (res != 3) { return -333; } if (std::string(s.c_str(), res) != "-0.") { return -433; }
      res = str_from_double(-0.99, &s[0], 4, 6); if (res != 4) { return -334; } if (std::string(s.c_str(), res) != "-0.9") { return -434; }
      res = str_from_double(-0.99, &s[0], 5, 6); if (res != 5) { return -335; } if (std::string(s.c_str(), res) != "-0.99") { return -435; }
      res = str_from_double(-0.99, &s[0], 6, 6); if (res != 5) { return -336; } if (std::string(s.c_str(), res) != "-0.99") { return -436; }

      res = str_from_double(9.99, &s[0], 0, 6); if (res != 0) { return -344; }
      res = str_from_double(9.99, &s[0], 1, 6); if (res != 1) { return -345; } if (std::string(s.c_str(), res) != "+") { return -445; }
      res = str_from_double(9.99, &s[0], 2, 6); if (res != 2) { return -346; } if (std::string(s.c_str(), res) != "9.") { return -446; }
      res = str_from_double(9.99, &s[0], 3, 6); if (res != 3) { return -347; } if (std::string(s.c_str(), res) != "9.9") { return -447; }
      res = str_from_double(9.99, &s[0], 4, 6); if (res != 4) { return -348; } if (std::string(s.c_str(), res) != "9.99") { return -448; }
      res = str_from_double(9.99, &s[0], 5, 6); if (res != 4) { return -349; } if (std::string(s.c_str(), res) != "9.99") { return -449; }

      res = str_from_double(-9.99, &s[0], 0, 6); if (res != 0) { return -350; }
      res = str_from_double(-9.99, &s[0], 1, 6); if (res != 1) { return -351; } if (std::string(s.c_str(), res) != "-") { return -451; }
      res = str_from_double(-9.99, &s[0], 2, 6); if (res != 1) { return -352; } if (std::string(s.c_str(), res) != "-") { return -452; }
      res = str_from_double(-9.99, &s[0], 3, 6); if (res != 3) { return -353; } if (std::string(s.c_str(), res) != "-9.") { return -453; }
      res = str_from_double(-9.99, &s[0], 4, 6); if (res != 4) { return -354; } if (std::string(s.c_str(), res) != "-9.9") { return -454; }
      res = str_from_double(-9.99, &s[0], 5, 6); if (res != 5) { return -355; } if (std::string(s.c_str(), res) != "-9.99") { return -455; }
      res = str_from_double(-9.99, &s[0], 6, 6); if (res != 5) { return -356; } if (std::string(s.c_str(), res) != "-9.99") { return -456; }

      res = str_from_double(19.99, &s[0], 0, 6); if (res != 0) { return -364; }
      res = str_from_double(19.99, &s[0], 1, 6); if (res != 1) { return -365; } if (std::string(s.c_str(), res) != "+") { return -465; }
      res = str_from_double(19.99, &s[0], 2, 6); if (res != 1) { return -366; } if (std::string(s.c_str(), res) != "+") { return -466; }
      res = str_from_double(19.99, &s[0], 3, 6); if (res != 3) { return -367; } if (std::string(s.c_str(), res) != "19.") { return -467; }
      res = str_from_double(19.99, &s[0], 4, 6); if (res != 4) { return -368; } if (std::string(s.c_str(), res) != "19.9") { return -468; }
      res = str_from_double(19.99, &s[0], 5, 6); if (res != 5) { return -369; } if (std::string(s.c_str(), res) != "19.99") { return -469; }
      res = str_from_double(19.99, &s[0], 5, 6); if (res != 5) { return -361; } if (std::string(s.c_str(), res) != "19.99") { return -461; }

      res = str_from_double(-19.99, &s[0], 0, 6); if (res != 0) { return -370; }
      res = str_from_double(-19.99, &s[0], 1, 6); if (res != 1) { return -371; } if (std::string(s.c_str(), res) != "-") { return -471; }
      res = str_from_double(-19.99, &s[0], 2, 6); if (res != 1) { return -372; } if (std::string(s.c_str(), res) != "-") { return -472; }
      res = str_from_double(-19.99, &s[0], 3, 6); if (res != 1) { return -373; } if (std::string(s.c_str(), res) != "-") { return -473; }
      res = str_from_double(-19.99, &s[0], 4, 6); if (res != 4) { return -374; } if (std::string(s.c_str(), res) != "-19.") { return -474; }
      res = str_from_double(-19.99, &s[0], 5, 6); if (res != 5) { return -375; } if (std::string(s.c_str(), res) != "-19.9") { return -475; }
      res = str_from_double(-19.99, &s[0], 6, 6); if (res != 6) { return -376; } if (std::string(s.c_str(), res) != "-19.99") { return -476; }
      res = str_from_double(-19.99, &s[0], 7, 6); if (res != 6) { return -376; } if (std::string(s.c_str(), res) != "-19.99") { return -476; }

      res = str_from_double(196.789, &s[0], 20, 6); if (res != 7) { return -381; } if (std::string(s.c_str(), res) != "196.789") { return -481; }
      res = str_from_double(196.789, &s[0], 20, 5); if (res != 6) { return -382; } if (std::string(s.c_str(), res) != "196.78") { return -482; }
      res = str_from_double(196.789, &s[0], 20, 4); if (res != 5) { return -383; } if (std::string(s.c_str(), res) != "196.7") { return -483; }
      res = str_from_double(196.789, &s[0], 20, 3); if (res != 4) { return -384; } if (std::string(s.c_str(), res) != "196.") { return -484; }
      res = str_from_double(196.789, &s[0], 20, 2); if (res != 5) { return -385; } if (std::string(s.c_str(), res) != "1.9e2") { return -485; }
      res = str_from_double(196.789, &s[0], 20, 1); if (res != 4) { return -386; } if (std::string(s.c_str(), res) != "1.e2") { return -486; }
      res = str_from_double(196.789, &s[0], 20, 0); if (res != 4) { return -387; } if (std::string(s.c_str(), res) != "1.e2") { return -487; }

      res = str_from_double(-196.789, &s[0], 20, 6); if (res != 8) { return -391; } if (std::string(s.c_str(), res) != "-196.789") { return -491; }
      res = str_from_double(-196.789, &s[0], 20, 5); if (res != 7) { return -392; } if (std::string(s.c_str(), res) != "-196.78") { return -492; }
      res = str_from_double(-196.789, &s[0], 20, 4); if (res != 6) { return -393; } if (std::string(s.c_str(), res) != "-196.7") { return -493; }
      res = str_from_double(-196.789, &s[0], 20, 3); if (res != 5) { return -394; } if (std::string(s.c_str(), res) != "-196.") { return -494; }
      res = str_from_double(-196.789, &s[0], 20, 2); if (res != 6) { return -395; } if (std::string(s.c_str(), res) != "-1.9e2") { return -495; }
      res = str_from_double(-196.789, &s[0], 20, 1); if (res != 5) { return -396; } if (std::string(s.c_str(), res) != "-1.e2") { return -496; }
      res = str_from_double(-196.789, &s[0], 20, 0); if (res != 5) { return -397; } if (std::string(s.c_str(), res) != "-1.e2") { return -497; }

      std::string s2;

      s2 = "0.001"; res = str_from_double(0.001, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -501; } if (std::string(s.c_str(), res) != s2) { return -601; }
      s2 = "0.0015"; res = str_from_double(0.0015, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -502; } if (std::string(s.c_str(), res) != s2) { return -602; }
      s2 = "9.99e-4"; res = str_from_double(0.000999, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -503; } if (std::string(s.c_str(), res) != s2) { return -603; }

        s2 = "-0.001"; res = str_from_double(-0.001, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -504; } if (std::string(s.c_str(), res) != s2) { return -604; }
        s2 = "-0.0015"; res = str_from_double(-0.0015, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -505; } if (std::string(s.c_str(), res) != s2) { return -605; }
        s2 = "-9.99e-4"; res = str_from_double(-0.000999, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -506; } if (std::string(s.c_str(), res) != s2) { return -606; }

        s2 = "999999."; res = str_from_double(999999.99, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -507; } if (std::string(s.c_str(), res) != s2) { return -607; }
        s2 = "1.e6"; res = str_from_double(1.e6, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -508; } if (std::string(s.c_str(), res) != s2) { return -608; }
        s2 = "1.001e6"; res = str_from_double(1.001e6, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -509; } if (std::string(s.c_str(), res) != s2) { return -609; }

        s2 = "-999999."; res = str_from_double(-999999.99, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -511; } if (std::string(s.c_str(), res) != s2) { return -611; }
        s2 = "-1.e6"; res = str_from_double(-1.e6, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -512; } if (std::string(s.c_str(), res) != s2) { return -612; }
        s2 = "-1.001e6"; res = str_from_double(-1.001e6, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -513; } if (std::string(s.c_str(), res) != s2) { return -613; }

      s2 = "1.e308"; res = str_from_double(1.e308, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -514; } if (std::string(s.c_str(), res) != s2) { return -614; }
      s2 = "1.001e308"; res = str_from_double(1.001e308, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -515; } if (std::string(s.c_str(), res) != s2) { return -615; }
      s2 = "9.99e307"; res = str_from_double(0.999e308, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -516; } if (std::string(s.c_str(), res) != s2) { return -616; }

        s2 = "1.e50"; res = str_from_double(1.e50, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -517; } if (std::string(s.c_str(), res) != s2) { return -617; }
        s2 = "1.001e50"; res = str_from_double(1.001e50, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -518; } if (std::string(s.c_str(), res) != s2) { return -618; }
        s2 = "9.99e49"; res = str_from_double(0.999e50, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -519; } if (std::string(s.c_str(), res) != s2) { return -619; }

        s2 = "1.e7"; res = str_from_double(1.e7, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -521; } if (std::string(s.c_str(), res) != s2) { return -621; }
        s2 = "1.1e7"; res = str_from_double(1.1e7, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -522; } if (std::string(s.c_str(), res) != s2) { return -622; }
        s2 = "9.99e6"; res = str_from_double(0.999e7, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -523; } if (std::string(s.c_str(), res) != s2) { return -623; }

        s2 = "-1.e308"; res = str_from_double(-1.e308, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -524; } if (std::string(s.c_str(), res) != s2) { return -624; }
        s2 = "-1.001e308"; res = str_from_double(-1.001e308, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -525; } if (std::string(s.c_str(), res) != s2) { return -625; }
        s2 = "-9.99e307"; res = str_from_double(-0.999e308, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -526; } if (std::string(s.c_str(), res) != s2) { return -626; }

        s2 = "-1.e50"; res = str_from_double(-1.e50, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -527; } if (std::string(s.c_str(), res) != s2) { return -627; }
        s2 = "-1.001e50"; res = str_from_double(-1.001e50, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -528; } if (std::string(s.c_str(), res) != s2) { return -628; }
        s2 = "-9.99e49"; res = str_from_double(-0.999e50, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -529; } if (std::string(s.c_str(), res) != s2) { return -629; }

        s2 = "-1.e7"; res = str_from_double(-1.e7, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -531; } if (std::string(s.c_str(), res) != s2) { return -631; }
        s2 = "-1.1e7"; res = str_from_double(-1.1e7, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -532; } if (std::string(s.c_str(), res) != s2) { return -632; }
        s2 = "-9.99e6"; res = str_from_double(-0.999e7, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -533; } if (std::string(s.c_str(), res) != s2) { return -633; }

      s2 = "0."; res = str_from_double(1.e-308, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -714; } if (std::string(s.c_str(), res) != s2) { return -814; }
      s2 = "0.223e-307"; res = str_from_double(2.23e-308, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -715; } if (std::string(s.c_str(), res) != s2) { return -815; }
      s2 = "0."; res = str_from_double(0.999e-308, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -716; } if (std::string(s.c_str(), res) != s2) { return -816; }

        s2 = "1.e-50"; res = str_from_double(1.e-50, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -717; } if (std::string(s.c_str(), res) != s2) { return -817; }
        s2 = "1.001e-50"; res = str_from_double(1.001e-50, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -718; } if (std::string(s.c_str(), res) != s2) { return -818; }
        s2 = "9.99e-51"; res = str_from_double(0.999e-50, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -719; } if (std::string(s.c_str(), res) != s2) { return -819; }

        s2 = "1.e-7"; res = str_from_double(1.e-7, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -721; } if (std::string(s.c_str(), res) != s2) { return -821; }
        s2 = "1.1e-7"; res = str_from_double(1.1e-7, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -722; } if (std::string(s.c_str(), res) != s2) { return -822; }
        s2 = "9.99e-8"; res = str_from_double(0.999e-7, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -723; } if (std::string(s.c_str(), res) != s2) { return -823; }

        s2 = "-0."; res = str_from_double(-1.e-308, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -724; } if (std::string(s.c_str(), res) != s2) { return -824; }
        s2 = "-0.223e-307"; res = str_from_double(-2.23e-308, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -725; } if (std::string(s.c_str(), res) != s2) { return -825; }
        s2 = "-0."; res = str_from_double(-0.999e-308, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -726; } if (std::string(s.c_str(), res) != s2) { return -826; }

        s2 = "-1.e-50"; res = str_from_double(-1.e-50, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -727; } if (std::string(s.c_str(), res) != s2) { return -827; }
        s2 = "-1.001e-50"; res = str_from_double(-1.001e-50, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -728; } if (std::string(s.c_str(), res) != s2) { return -828; }
        s2 = "-9.99e-51"; res = str_from_double(-0.999e-50, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -729; } if (std::string(s.c_str(), res) != s2) { return -829; }

        s2 = "-1.e-7"; res = str_from_double(-1.e-7, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -731; } if (std::string(s.c_str(), res) != s2) { return -831; }
        s2 = "-1.1e-7"; res = str_from_double(-1.1e-7, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -732; } if (std::string(s.c_str(), res) != s2) { return -832; }
        s2 = "-9.99e-8"; res = str_from_double(-0.999e-7, &s[0], 20, 6); if (unsigned(res) !=  s2.length()) { return -733; } if (std::string(s.c_str(), res) != s2) { return -833; }

        s2 = "-0.223e-307"; res = str_from_double(-2.23e-308, &s[0], 20, 5); if (unsigned(res) !=  s2.length()) { return -735; } if (std::string(s.c_str(), res) != s2) { return -835; }
        s2 = "-0.223e-307"; res = str_from_double(-2.23e-308, &s[0], 20, 4); if (unsigned(res) !=  s2.length()) { return -736; } if (std::string(s.c_str(), res) != s2) { return -836; }
        s2 = "-0.22e-307"; res = str_from_double(-2.23e-308, &s[0], 20, 3); if (unsigned(res) !=  s2.length()) { return -737; } if (std::string(s.c_str(), res) != s2) { return -837; }
        s2 = "-0.2e-307"; res = str_from_double(-2.23e-308, &s[0], 20, 2); if (unsigned(res) !=  s2.length()) { return -738; } if (std::string(s.c_str(), res) != s2) { return -838; }
        s2 = "-0.e-307"; res = str_from_double(-2.23e-308, &s[0], 20, 1); if (unsigned(res) !=  s2.length()) { return -739; } if (std::string(s.c_str(), res) != s2) { return -839; }

      const s_long n2 = 10000;
      for (s_long i = 0; i < n2; ++i)
      {
        double x = (rnd() * 2 - 1.) * 1.e308;
        s = "abcdefghijklmnopqrstuvwxyz";
        res = str_from_double(x, &s[0], 7 + 6, 6); if (res < 2) { return -901; }
        std::stringstream ts; ts << s.substr(0, res);
        double x2(0); ts >> x2;
        bool b1 = std::fabs(x) >= 1.e-100 && std::fabs(x2 - x) / std::fabs(x) >= 1.e-5;
        bool b2 = std::fabs(x) >= 1.e-306 && std::fabs(x2 - x) / std::fabs(x) >= 0.1;
        bool b3 = std::fabs(x) > 0. && std::fabs(x) < 1.e-306 && std::fabs(x2 - x) / std::fabs(x) >= 1.;
        if (b1 || b2 || b3) { log.d, "x2 != x; res, x, x2, s, delta:", res, x, x2, "'" + s + "'", std::fabs(x2 - x) / std::fabs(x); return -902; }
      }

      return 1;
    }

    int test_flstr()
    {
      using namespace bmdx_str;
      typedef flstr_t<30> _fl30;
      _fl30 s1("1.23456e25"), s2(123456), s3(1.23456e25);
      if (s1 != s3) { return -1; }
      if (s1 != "1.23456e25") { return -2; }
      if (s1 != L"1.23456e25") { return -3; }
      if (s2 != L"123456") { return -4; }
      if (s1 != std::string("1.23456e25")) { return -5; }
      if (s1 != std::wstring(L"1.23456e25")) { return -6; }
      if (s1 + "/" + s2 + "/" + s3 != "1.23456e25/123456/1.23456e25") { return -7; }
      if ((s1 + "/" + s2 + "/" + s3).c_str() != std::string("1.23456e25/123456/1.23456e25")) { return -8; }
      if ((s1 + "/" + s2 + "/" + s3 + "ab").c_str() != std::string("1.23456e25/123456/1.23456e25ab")) { return -9; }
      if ((s1 + "/" + s2 + "/" + s3 + "abc").c_str() != std::string("1.23456e25/123456/1.23456e25ab")) { return -10; }
      if ((s1 + L"/" + s2 + L"/" + s3 + L"abc").c_str() != std::string("1.23456e25/123456/1.23456e25ab")) { return -11; }

      return 1;
    }

    int test_1251()
    {
      log << "Testing ANSI-1251 in log. The following should look as ABVGD-abvgd in Russian: \xC0\xC1\xC2\xC3\xC4-\xE0\xE1\xE2\xE3\xE4." << endl;
      return 1;
    }


  //===================================================
  // Entry point for full test sequence.
  //===================================================

    template<class T> struct ptr_deref_t { typedef T t; };
    template<class T> struct ptr_deref_t<T*> { typedef T t; };
    template<class T> struct ptr_deref_t<const T*> { typedef T t; };

  int do_test(bool is_full, const char* prnd0)
  {
    full_test() = is_full;
    rnd_00() = prnd0 ? atol(prnd0) : s_long(std::time(0)); rnd_0() = rnd_00();
    std::string prevLocaleName; if (1) { const char* p = std::setlocale(LC_CTYPE, ""); if (p) { prevLocaleName = p; } }

#ifdef _WIN32
    log.aux = report_aux::F;
#endif

    try
    {

      log << nowstr() << " -- " << this->fnp_log << endl;
      log << "do_test(). NOTE Testing may take more than one minute." << endl;

      int res;

//system("pause");
//goto lExit;

      res = stat.exec_test(&t_test::perf_timer, "perf_timer()");
        if (res < 0) { goto lExit; }
      res = stat.exec_test(&t_test::test_sleep_mcs, "test_sleep_mcs()");
        if (res < 0) { goto lExit; }
      res = stat.exec_test(&t_test::test_cpu_frequency, "test_cpu_frequency()");
        if (res < 0) { goto lExit; }
      res = stat.exec_test(&t_test::test_thread_priority, "test_thread_priority()");
        if (res < 0) { goto lExit; }
        rnd_0() = rnd_00();
      res = stat.exec_test(&t_test::test_str_from_number, "test_str_from_number()");
        if (res < 0) { goto lExit; }
      res = stat.exec_test(&t_test::test_flstr, "test_flstr()");
        if (res < 0) { goto lExit; }
      res = stat.exec_test(&t_test::test_1251, "test_1251()");
        if (res < 0) { goto lExit; }

//system("pause");
    }
    catch (exception& e) { log.d, "do_test / std exception:", e.what(); }
    catch (...) { log.d, "do_test / unknown exception."; }


lExit:
    log << "TESTING COMPLETED. Total: success " << stat.__ns << ", failure " << stat.__nf << ", skipped " << stat.__ni << "." << endl << endl; log.flush();
    if (prevLocaleName.length()) { std::setlocale(LC_CTYPE, prevLocaleName.c_str()); }

    return stat.__nf;
  }


};

}

#ifdef _MSC_VER
  #pragma warning(pop)
#endif

#endif
