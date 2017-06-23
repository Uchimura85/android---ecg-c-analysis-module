// In this module:
//  logger_t, test_base_t.

#ifndef yk_tests_base_H
#define yk_tests_base_H

#include <cstdlib>
#include <clocale>
#include <ctime>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <limits.h>
#include <exception>

#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable:4355)
#endif

namespace yk_tests
{
  // ===================================================
  // Text output for debug purposes.
  // ===================================================

  struct t_endl {}; inline static t_endl endl() { return t_endl(); }

  template<class _ = int> //  "_" is ignored
  struct logger_t
  {
    typedef logger_t logger;
    typedef std::ios_base& (*t_stdmanip)(std::ios_base&);
  private:
    struct debugger
    {
      logger* const parent; debugger(logger& r, long n, long m) : parent(&r), _n(n), _m(m) {} ~debugger() { if (_n || _m) { (*parent) << endl; } }
      template<class T> debugger& operator<<(const T& x) { if (_n) { (*parent) << ' '; } (*parent) << x; ++_n; return *this; }
      debugger& operator<<(t_stdmanip x) { (*parent) << x; return *this; }
      debugger& operator<<(t_endl (*x)()) { (void)x; (*parent) << endl; _n = 0; ++_m; return *this; }
      debugger& operator<<(const std::string& x) { if (x.empty()) { return *this; } char c1 = x[0]; char c2 = x.size() > 1 ? x[1] : 'z'; char c3 = x[x.size()-1]; if (parent->_puncts.find(c1) != std::string::npos && !(c1 == '.' && c2 >= '0' && c2 <= '9')) { _n = 0; ++_m; } bool is_endl(c3 == '\n' || c3 == '\r'); if (is_endl) { try { operator<< <std::string>(x.substr(0, x.size()-1)); } catch (...) {}; *this << endl; } else { operator<< <std::string>(x); } return *this; }
      debugger& operator<<(const char* x) { return operator<<(std::string(x)); } debugger& operator<<(const wchar_t* x) { return *this << std::wstring(x); }
      typedef debugger& t_ret; template<class T> t_ret operator , (const T& x) { return *this << x; } t_ret operator , (t_stdmanip x) { return *this << x; } t_ret operator , (t_endl (*x)()) { return *this << x; } t_ret operator , (const std::string& x) { return *this << x; } t_ret operator , (const char* x) { return *this << x; } t_ret operator , (const wchar_t* x) { return *this << x; }
      private: long _n; long _m;
    };
    struct debugger0
    {
      logger* const parent; debugger0(logger& r) : parent(&r) {}
      template<class T> debugger operator<<(const T& x) const { (*parent) << x; return debugger(*parent, 1, 0); }
      debugger operator<<(t_stdmanip x) const { (*parent) << x; return debugger(*parent, 0, 0); }
      debugger operator<<(t_endl (*x)()) const { (*parent) << endl; return debugger(*parent, 0, 1); }
      debugger operator<<(const std::string& x) { if (x.empty()) { return debugger(*parent, 0, 1); } char c3 = x[x.size()-1]; bool is_endl(c3 == '\n' || c3 == '\r'); if (is_endl) { try { operator<< <std::string>(x.substr(0, x.size()-1)); } catch (...) {}; *this << endl; return debugger(*parent, 0, 0); } else { operator<< <std::string>(x); return debugger(*parent, 1, 0); } }
      debugger operator<<(const char* x) { return operator<<(std::string(x)); } debugger operator<<(const wchar_t* x) { return *this << std::wstring(x); }
      typedef debugger t_ret; template<class T> t_ret operator , (const T& x) const { return *this << x; } t_ret operator , (t_stdmanip x) const { return *this << x; } t_ret operator , (t_endl (*x)()) const { return *this << x; } t_ret operator , (const std::string& x) const { return *this << x; } t_ret operator , (const char* x) const { return *this << x; } t_ret operator , (const wchar_t* x) const { return *this << x; }
      private: debugger0(const debugger0&); debugger0& operator=(const debugger0&);
    };

  public:

    const debugger0 d; // use this for debug output with automatic space and new line insertions, args. may be comma-separated

    std::ofstream fs; // this is opened if given logfn_ is not a null ptr. and points to a valid file

    bool use_cout; // true by dflt
    bool use_cerr; // false by dflt

      // Additional output, optionally set by the client.
    typedef void (*F_log_string)(const std::string&);
    F_log_string aux;

    long nsp; // number of spaces at the beginning of each line

      // Logger puts its arguments into 0 or more of: given file, stdout (dflt. enabled), stderr (dflt.disabled), auxiliary output (dflt.disabled).

    logger_t(const char* logfn_, bool fs_trunc_) : d(*this), fs(), use_cout(true), use_cerr(false), aux(0), nsp(0), _logfn(logfn_), _fs_trunc(fs_trunc_), _has_chars(false), _puncts(".,;:)]}") {}
    template<class T> logger& operator<<(const T& x) { try { _try_open_fs(); _try_log_spaces(); if (use_cerr) { std::cerr << x; } if (fs.is_open()) { fs << x; } if (use_cout) { std::cout << x; } if (aux) { oss << x; } _has_chars = true; } catch (...) {} return *this; }
    logger& operator<<(t_stdmanip x) { try { _try_open_fs(); if (use_cerr) { std::cerr << x; } if (fs.is_open()) { fs << x; } if (use_cout) { std::cout << x; } if (aux) { oss << x; } } catch (...) {} return *this; }
    logger& operator<<(t_endl (*x)()) { try { (void)x; _try_open_fs(); if (use_cerr) { std::cerr << std::endl; } if (fs.is_open()) { fs << std::endl; } if (use_cout) { std::cout << std::endl; } _has_chars = false; if (aux) { oss << '\0'; std::string s = oss.str(); oss.str(""); aux(s.c_str()); } } catch (...) {} return *this; }
    logger& operator<<(const std::wstring& x) { return *this << wsbs(x); } logger& operator<<(const char* x) { return operator<<(std::string(x)); } debugger operator<<(const wchar_t* x) { return *this << std::wstring(x); }
    typedef logger& t_ret; template<class T> t_ret operator , (const T& x) { return *this << x; } t_ret operator , (t_stdmanip x) { return *this << x; } t_ret operator , (t_endl (*x)()) { return *this << x; } t_ret operator , (const std::string& x) { return *this << x; } t_ret operator , (const char* x) { return *this << x; } t_ret operator , (const wchar_t* x) { return *this << x; }
    void flush() { try { if (use_cerr) {} if (fs.is_open()) { fs.flush(); } if (use_cout) { std::cout.flush(); } } catch (...) {} }

    struct _tcnvchk { static bool _test_mbrtowc() { char cc[MB_LEN_MAX+1]; int res; wchar_t c; cc[0] = 'A'; res = mbrtowc(&c, cc, 1, 0); if (res != 1 || c != L'A') { return false; } cc[0] = '1'; res = mbrtowc(&c, cc, 1, 0); if (res != 1 || c != L'1') { return false; } cc[0] = ';'; res = mbrtowc(&c, cc, 1, 0); if (res != 1 || c != L';') { return false; } cc[0] = ' '; res = mbrtowc(&c, cc, 1, 0); if (res != 1 || c != L' ') { return false; } return true; } static bool _test_wcrtomb() { char cc[MB_LEN_MAX+1]; int res; res = wcrtomb(cc, L'A', 0); if (res != 1 || cc[0] != 'A') { return false; } res = wcrtomb(cc, L'1', 0); if (res != 1 || cc[0] != '1') { return false; } res = wcrtomb(cc, L';', 0); if (res != 1 || cc[0] != ';') { return false; } res = wcrtomb(cc, L' ', 0); if (res != 1 || cc[0] != ' ') { return false; } return true; } };
    static std::string wsbs(const std::wstring& x) { std::string s; int cnt(0); char cc[9]; if (wcrtomb(0, 0, 0)) {} static int _mb(0); if (!_mb) { _mb = 2 + (int)_tcnvchk::_test_wcrtomb(); } if (_mb == 2) { for(size_t pos = 0; pos < x.length(); ++pos) { unsigned int q = x[pos]; if (q >= 128) { q = '?'; } s += (char)q; } return s; } for(size_t pos = 0; pos < x.length(); ++pos) { cnt = wcrtomb(cc, x[pos], 0); if (cnt == -1) { s += '?'; } else { cc[cnt] = 0; s += cc; } } return s; }
    static std::wstring bsws(const std::string& x) { std::wstring ws; static int _mb(0); if (!_mb) { _mb = 2 + (int)_tcnvchk::_test_mbrtowc(); } if (_mb == 2) { for(size_t pos = 0; pos < x.length(); ++pos) { unsigned int q = x[pos]; if (q >= 128) { q = L'?'; } ws += (wchar_t)q; } return ws; } int cnt(0); size_t pos(0); wchar_t dest; while(pos < x.length()) { cnt = mbtowc(&dest, &x[pos], x.length()-pos); if (cnt == -1) { pos += 1; ws += L'?'; } else { pos += size_t(cnt); ws += dest; } } return ws; }
  private:
    const char* _logfn; const bool _fs_trunc; bool _has_chars; std::ostringstream oss; const std::string _puncts;
    void _try_open_fs() { if (!fs.is_open() && _logfn) { fs.open(_logfn, _fs_trunc ? std::ios_base::trunc | std::ios_base::ate : std::ios_base::app); if (!fs.is_open() && _logfn && _fs_trunc) { fs.open(_logfn, std::ios_base::app); } } }
    void _try_log_spaces() { if (!_has_chars && nsp > 0) { long nsp2 = nsp; if (nsp > 2048) { nsp2 = 2048; } for (long i = 0; i < nsp2; ++i) { if (use_cerr) { std::cerr << ' '; } if (fs.is_open()) { fs << ' '; } if (use_cout) { std::cout << ' '; } if (aux) { oss << ' '; } } } }
    logger_t(const logger&); logger_t& operator=(const logger&);
  };

  // ===================================================
  // Basic service functions for automated testing
  //  (log inst., rnd, millisecond clock, num/str conv. etc.).
  // ===================================================

  template<class _ = int> //  "_" is ignored
  struct test_base_t
  {
  public:
    std::string fnp_log;
    logger_t<> log;

      // Valid non-null log file name will output test results both into file and into stdout.
      // Otherwise will output test results only into stdout.
    test_base_t() : log("yk_tests-base-log.txt", true) { rnd_0() = 0; }
    test_base_t(const char* logfn, bool log_truncate) : fnp_log(logfn), log(logfn, log_truncate) { rnd_0() = 0; }

    static double msclk() { static const double k = 1000. / CLOCKS_PER_SEC; return double(std::clock()) * k; }

    static std::string nowstr() { std::time_t _t; std::time(&_t); return std::string(std::ctime(&_t), 24); }

    static long& rnd_00() { static long x(0); return x; } // optional common starting value for each test

    typedef unsigned long long int __t_uint64;
    static __t_uint64& rnd_0() { static __t_uint64 x = 0; return x; } // the current rnd. generator value
    static double rnd() { const __t_uint64 n1 = 402653189; const __t_uint64 n2 = 805306457; __t_uint64 x = rnd_0(); x *= n1; x += n2; rnd_0() = x; return double(x & 0xfffffffful) / (double(0xfffffffful)+1.); }

      // The function measuring delays between timer value changes.
      // Delay depends on both clock resolution and thread priority.
      //  F must return the current clock value in milliseconds, maybe with frac. part.
      // duration -- max duration of measurements, ms. (1000 is a good dflt.)
      // is_peak == true measures max. delay during duration.
      // is_peak == false calculates median average of delays measured during duration.
    static double __timergap(double(*F)(), double duration, bool is_peak)
    {
      double t0 = F();
      if (is_peak) { double dt(0.); while (true) { double t1 = F(); double t2, t3; while (true) { t2 = F(); if (t2 > t1) { break; } } while (true) { t3 = F(); if (t3 > t2) { break; } } if (t3 - t2 > dt) { dt = t3 - t2; } if (t3 >= t0 + duration) { break; } } return dt; }
       else { long nmax = 180000; long n = 0; std::vector<double> vals(nmax); for (long i = 0; i < nmax; ++i) { double t1 = F(); double t2, t3; while (true) { t2 = F(); if (t2 > t1) { break; } } while (true) { t3 = F(); if (t3 > t2) { break; } } vals[i] = t3 - t2; ++n; if (t3 >= t0 + duration) { break; } } if (n == 0) { return duration; } std::sort(vals.begin(), vals.begin()+n); return vals[n/2]; }
    }

      // Invocation of __consume(x) ensures that any previous client code using x will be compiled
      //  and executed even if the compiler performs optimizations.
    template<class T> static long __consume(const T& x) { static long x0(0); x0 += (const unsigned char&)x; return x0; }

    template<class T> static std::string str(const T& x) { std::ostringstream oss; oss << x << '\0'; return oss.str().c_str(); }
    static std::string str(float x) { std::ostringstream oss; oss << std::setprecision(9) <<  x << '\0'; return oss.str().c_str(); }
    static std::string str(double x) { std::ostringstream oss; oss << std::setprecision(16) <<  x << '\0'; return oss.str().c_str(); }
    static std::string str(long double x) { std::ostringstream oss; oss << std::setprecision(25) <<  x << '\0'; return oss.str().c_str(); }
    static double to_dbl(const std::string& x) { try { std::string x2(x); x2 += '\0'; return atof(x2.c_str()); } catch (...) { return 0.; } }
    static long to_lng(const std::string& x) { try { std::string x2(x); x2 += '\0'; return atol(x2.c_str()); } catch (...) { return 0.; } }
    static std::string wsbs(const std::wstring& x) { return logger_t<>::wsbs(x); }
    static std::wstring bsws(const std::string& x) { return logger_t<>::bsws(x); }
    static std::string pad(long n, long width) { long nd = 0; if (n <= 0) { ++nd; } while(n) { ++nd; n /= 10; } nd = width - nd; if (nd < 0) { nd = 0; } return std::string(nd, ' '); }

    template<class test_KIND>
    struct stat_t
    {
      typedef test_KIND t_test;
      int __ns, __nf, __ni; t_test& __r_parent; stat_t(t_test& parent) : __ns(0), __nf(0), __ni(0), __r_parent(parent) {}
      int exec_test(int (t_test::*F)(), const char * F_name) { int res; __r_parent.log.nsp += 2; try { res = (__r_parent.*F)(); } catch(const std::exception& e) { ++__nf; __r_parent.log, "FAILURE: C++ exception: ", F_name, endl, e.what(), endl, endl; return -1999000; } catch(...) { ++__nf; __r_parent.log, "FAILURE: C++ exception: ", F_name, endl, endl; return -1999000; } (res>0 ?++__ns:(res == 0 ? ++__ni : ++__nf)); __r_parent.log.nsp -= 2; __r_parent.log, (res>0 ? "SUCCESS: " : (res==0?"SKIPPED: " : "FAILURE: ")), res, ": ", F_name, endl, endl; return res; }
      void reset() { __ns = 0; __nf = 0; __ni = 0; }
    };
  };

  typedef logger_t<> logger;
  typedef test_base_t<> base;
}

#ifdef _MSC_VER
  #pragma warning(pop)
#endif

#endif
