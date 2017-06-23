// BMDX library 0.7 EXPERIMENTAL for desktop & mobile platforms
//  (binary modules data exchange)
// NOTE This is experimental version, quality/safety of code is not guaranteed. Be careful.
//
// Copyright 2004-2017 Yevgueny V. Kondratyev (Dnipro (Dnepropetrovsk), Ukraine)
// Contacts: ceqq [at] mail [dot] ru, sprdwn [at] gmail [dot] com
// Project website: hashx.dp.ua
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// https://opensource.org/licenses/MIT

// bmdx_cpiomt.h: cross-platform tools for i/o and multithreading (standalone header).

// Features:
//
//    Automatic definition for 32, 64-bit signed integers.    (bmdx_meta::  s_long, u_long, s_ll, u_ll)
//    Read/write data words between integer variable and memory.    (bmdx_str::words::  le2 le4 le8 .. be8, set_le2 .. set_be8)
//    Conversions between strings and numbers. Fixed-length string.    (bmdx_str::conv::  str_from_*, ntocs, ntows, str2i, str2f; bmdx_str::  flstr_t)
//
//    Timer with near-microseconds resolution.    (bmdx::  clock_ms)
//    Sleep with near-microseconds resolution.    (bmdx::  sleep_mcs)
//    Recursive mutex.    (bmdx::  struct critsec_t)
//    Unified thread control. Priorities, high-level data passing to thread etc.    (bmdx::  struct threadctl)
//    Simple initForCallback launcher. Execute, wait, check running state.    (bmdx::  struct processctl)
//    Const. reference object with threadsafe setting and copying, + unsafe non-const access.    (bmdx::  struct cref_t)
//
//    Basic unbuffered console i/o.    (bmdx::  struct console_io)
//    Bytewise file i/o + save/load whole file as string.    (bmdx::  struct file_io)

#ifndef bmdx_cpiomt_H
#define bmdx_cpiomt_H

  // Rough platform selector
#ifdef _WIN32
  #define _bmdxpl_Wnds
#else
  #define _bmdxpl_Psx
#endif

#undef _yk_reg
#if __cplusplus > 199711L
  #define _yk_reg
#else
  #define _yk_reg register
#endif





namespace bmdx_meta
{
  struct nothing {};
  template<bool cond, class T1, class T2> struct if_t { typedef T1 result; }; template<class T1, class T2> struct if_t<false, T1, T2> { typedef T2 result; };
  template <int n, class t1=nothing, class t2=nothing, class t3=nothing, class t4=nothing, class t5=nothing> struct find_size_n { typedef typename if_t<sizeof(t1) == n, t1, typename find_size_n<n, t2, t3, t4, nothing>::result>::result result; };
  template <int n> struct find_size_n<n> { typedef nothing result; };

    // 32-bit, 64-bit integers.
  typedef bmdx_meta::find_size_n<4, signed long int, signed int, signed short>::result s_long;
  typedef bmdx_meta::find_size_n<4, unsigned long int, unsigned int, unsigned short>::result u_long;
  typedef bmdx_meta::find_size_n<8, signed long long int, signed long int, signed int>::result s_ll;
  typedef bmdx_meta::find_size_n<8, unsigned long long int, unsigned long int, unsigned int>::result u_ll;
}

#undef _s_long
#undef _s_ll
#undef _u_ll
#ifdef yk_c_vecm_hashx_H
  #define _s_long ::yk_c::s_long
  #define _s_ll ::yk_c::meta::s_ll
#else
  #define _s_long ::bmdx_meta::s_long
  #define _s_ll ::bmdx_meta::s_ll
#endif
#define _u_ll ::bmdx_meta::u_ll




#include <new>
#include <string>
#include <ctime>
#include <cmath>
#include <ostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace bmdx_str
{
  namespace words
  {
    inline static _s_ll le8(const void* p, _s_long pos) throw() { _yk_reg unsigned char* pc = ((unsigned char*)p) + pos; _s_ll x = _s_ll(*pc++); x |= _s_ll(*pc++) << 8; x |= _s_ll(*pc++) << 16; x |= _s_ll(*pc++) << 24; x |= _s_ll(*pc++) << 32; x |= _s_ll(*pc++) << 40; x |= _s_ll(*pc++) << 48; x |= _s_ll(*pc) << 56; return x; }
    inline static _s_long le4(const void* p, _s_long pos) throw() { _yk_reg unsigned char* pc = ((unsigned char*)p) + pos; _s_long x = _s_long(*pc++); x |= _s_long(*pc++) << 8; x |= _s_long(*pc++) << 16; x |= _s_long(*pc) << 24; return x; }
    inline static _s_long le2(const void* p, _s_long pos) throw() { _yk_reg unsigned char* pc = ((unsigned char*)p) + pos; _s_long x = _s_long(*pc++); x |= _s_long(*pc) << 8; return x; }
    inline static void set_le8(void* p, _s_long pos, _s_ll x) throw() { _yk_reg char* pc = ((char*)p) + pos; *pc++ = char(x); x >>= 8; *pc++ = char(x); x >>= 8; *pc++ = char(x); x >>= 8; *pc++ = char(x); x >>= 8; *pc++ = char(x); x >>= 8; *pc++ = char(x); x >>= 8; *pc++ = char(x); x >>= 8; *pc = char(x);  }
    inline static void set_le4(void* p, _s_long pos, _s_long x) throw() { _yk_reg char* pc = ((char*)p) + pos; *pc++ = char(x); x >>= 8; *pc++ = char(x); x >>= 8; *pc++ = char(x); x >>= 8; *pc = char(x);  }
    inline static void set_le2(void* p, _s_long pos, _s_long x) throw() { _yk_reg char* pc = ((char*)p) + pos; *pc++ = char(x); x >>= 8; *pc = char(x);  }

    inline static _s_ll be8(const void* p, _s_long pos) throw() { _yk_reg unsigned char* pc = ((unsigned char*)p) + pos + 7; _s_ll x = _s_ll(*pc--); x |= _s_ll(*pc--) << 8; x |= _s_ll(*pc--) << 16; x |= _s_ll(*pc--) << 24; x |= _s_ll(*pc--) << 32; x |= _s_ll(*pc--) << 40; x |= _s_ll(*pc--) << 48; x |= _s_ll(*pc) << 56; return x; }
    inline static _s_long be4(const void* p, _s_long pos) throw() { _yk_reg unsigned char* pc = ((unsigned char*)p) + pos + 3; _s_long x = _s_long(*pc--); x |= _s_long(*pc--) << 8; x |= _s_long(*pc--) << 16; x |= _s_long(*pc) << 24; return x; }
    inline static _s_long be2(const void* p, _s_long pos) throw() { _yk_reg unsigned char* pc = ((unsigned char*)p) + pos + 1; _s_long x = _s_long(*pc--); x |= _s_long(*pc) << 8; return x; }
    inline static void set_be8(void* p, _s_long pos, _s_ll x) throw() { _yk_reg char* pc = ((char*)p) + pos + 7; *pc-- = char(x); x >>= 8; *pc-- = char(x); x >>= 8; *pc-- = char(x); x >>= 8; *pc-- = char(x); x >>= 8; *pc-- = char(x); x >>= 8; *pc-- = char(x); x >>= 8; *pc-- = char(x); x >>= 8; *pc = char(x);  }
    inline static void set_be4(void* p, _s_long pos, _s_long x) throw() { _yk_reg char* pc = ((char*)p) + pos + 3; *pc-- = char(x); x >>= 8; *pc-- = char(x); x >>= 8; *pc-- = char(x); x >>= 8; *pc = char(x);  }
    inline static void set_be2(void* p, _s_long pos, _s_long x) throw() { _yk_reg char* pc = ((char*)p) + pos + 1; *pc-- = char(x); x >>= 8; *pc = char(x);  }
  }

  namespace conv
  {
    struct exc_str2i : std::exception { const char* what() const throw () { return "bmdx_str::str2i"; } };
    struct exc_str2f : std::exception { const char* what() const throw () { return "bmdx_str::str2f"; } };

    template<class _ = int>
    struct _bmdx_str_impl
    {
      static _s_long str_from_s_ll(_s_ll x, char* buf, _s_long nchars, bool b_signed) throw();
      static _s_long str_from_double(double x, char* buf, _s_long nchars, _s_long ndmmax, _s_long nfracmax) throw();
      static _s_ll str2i(const wchar_t* x, _s_ll xlen, _s_ll dflt, bool no_exc);
      static double str2f(const wchar_t* x, _s_ll xlen, double dflt, bool no_exc);
      static _s_ll str2i(const char* x, _s_ll xlen, _s_ll dflt, bool no_exc);
      static double str2f(const char* x, _s_ll xlen, double dflt, bool no_exc);
    };

    template<class _>
    _s_long _bmdx_str_impl<_>::str_from_s_ll(_s_ll x, char* buf, _s_long nchars, bool b_signed) throw()
    {
      if (!(buf && nchars >= 0)) { return -1; }
      if (nchars == 0) { return 0; }
      bool b_neg = b_signed && x < 0;
      _s_long n = 0;
      if (b_signed) { _s_ll x2 = x; do { n += 1; x2 /= 10; } while (x2); if (b_neg) { n += 1; } }
        else { _u_ll x2 = x; do { n += 1; x2 /= 10; } while (x2);  }
      if (n > nchars) { if (x > 0) { buf[0] = '+'; } else if (x < 0) { buf[0] = b_signed ? '-' : '+'; } else { buf[0] = '0'; } return 1; }
      if (b_neg) { buf[0] = '-'; }
      _s_long pos = n;
      if (b_signed) { _s_ll x2 = x; do { _s_long d = x2 % 10; if (d < 0) { d = -d; } d += '0'; buf[--pos] = char(d); x2 /= 10; } while (x2); }
        else { _u_ll x2 = x; do { _s_long d = x2 % 10; d += '0'; buf[--pos] = char(d); x2 /= 10; } while (x2); }
      return n;
    }

    template<class _>
    _s_long _bmdx_str_impl<_>::str_from_double(double x, char* buf, _s_long nchars, _s_long ndmmax, _s_long nfracmax) throw()
    {
      if (!(x == x)) { x = 0.; }
      if (!(buf && nchars >= 0)) { return -1; }
      if (nchars == 0) { return 0; }
      if (nchars == 1) { buf[0] = x > 0. ? '+' : (x < 0. ? '-' : '0'); return 1; }
      static const int ndm_mm = 14;
      static const int nfrac_mm = 13;
      static const _s_ll _pow10[ndm_mm + 1] =
        { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000, 10000000000,
          100000000000, 1000000000000, 10000000000000, 100000000000000 };
      if (ndmmax < 1) { ndmmax = 1; } else if (ndmmax > ndm_mm) { ndmmax = ndm_mm; }
      if (nfracmax < 1) { nfracmax = 1; } else if (nfracmax > nfrac_mm) { nfracmax = nfrac_mm; }
      _s_long n_neg = _s_long(x < 0.);
      double x2 = n_neg ? -x : x;
      _s_long order = 0;
      static const double cmp1[310] = { 1e0,1e1,1e2,1e3,1e4,1e5,1e6,1e7,1e8,1e9,1e10,1e11,1e12,1e13,1e14,1e15,1e16,1e17,1e18,1e19,1e20,1e21,1e22,1e23,1e24,1e25,1e26,1e27,1e28,1e29,1e30,1e31,1e32,1e33,1e34,1e35,1e36,1e37,1e38,1e39,1e40,1e41,1e42,1e43,1e44,1e45,1e46,1e47,1e48,1e49,1e50,1e51,1e52,1e53,1e54,1e55,1e56,1e57,1e58,1e59,1e60,1e61,1e62,1e63,1e64,1e65,1e66,1e67,1e68,1e69,1e70,1e71,1e72,1e73,1e74,1e75,1e76,1e77,1e78,1e79,1e80,1e81,1e82,1e83,1e84,1e85,1e86,1e87,1e88,1e89,1e90,1e91,1e92,1e93,1e94,1e95,1e96,1e97,1e98,1e99,1e100,1e101,1e102,1e103,1e104,1e105,1e106,1e107,1e108,1e109,1e110,1e111,1e112,1e113,1e114,1e115,1e116,1e117,1e118,1e119,1e120,1e121,1e122,1e123,1e124,1e125,1e126,1e127,1e128,1e129,1e130,1e131,1e132,1e133,1e134,1e135,1e136,1e137,1e138,1e139,1e140,1e141,1e142,1e143,1e144,1e145,1e146,1e147,1e148,1e149,1e150,1e151,1e152,1e153,1e154,1e155,1e156,1e157,1e158,1e159,1e160,1e161,1e162,1e163,1e164,1e165,1e166,1e167,1e168,1e169,1e170,1e171,1e172,1e173,1e174,1e175,1e176,1e177,1e178,1e179,1e180,1e181,1e182,1e183,1e184,1e185,1e186,1e187,1e188,1e189,1e190,1e191,1e192,1e193,1e194,1e195,1e196,1e197,1e198,1e199,1e200,1e201,1e202,1e203,1e204,1e205,1e206,1e207,1e208,1e209,1e210,1e211,1e212,1e213,1e214,1e215,1e216,1e217,1e218,1e219,1e220,1e221,1e222,1e223,1e224,1e225,1e226,1e227,1e228,1e229,1e230,1e231,1e232,1e233,1e234,1e235,1e236,1e237,1e238,1e239,1e240,1e241,1e242,1e243,1e244,1e245,1e246,1e247,1e248,1e249,1e250,1e251,1e252,1e253,1e254,1e255,1e256,1e257,1e258,1e259,1e260,1e261,1e262,1e263,1e264,1e265,1e266,1e267,1e268,1e269,1e270,1e271,1e272,1e273,1e274,1e275,1e276,1e277,1e278,1e279,1e280,1e281,1e282,1e283,1e284,1e285,1e286,1e287,1e288,1e289,1e290,1e291,1e292,1e293,1e294,1e295,1e296,1e297,1e298,1e299,1e300,1e301,1e302,1e303,1e304,1e305,1e306,1e307,1e308,1.7976e308 };
      static const double cmp2[309] = { 2.225e-308,1e-307,1e-306,1e-305,1e-304,1e-303,1e-302,1e-301,1e-300,1e-299,1e-298,1e-297,1e-296,1e-295,1e-294,1e-293,1e-292,1e-291,1e-290,1e-289,1e-288,1e-287,1e-286,1e-285,1e-284,1e-283,1e-282,1e-281,1e-280,1e-279,1e-278,1e-277,1e-276,1e-275,1e-274,1e-273,1e-272,1e-271,1e-270,1e-269,1e-268,1e-267,1e-266,1e-265,1e-264,1e-263,1e-262,1e-261,1e-260,1e-259,1e-258,1e-257,1e-256,1e-255,1e-254,1e-253,1e-252,1e-251,1e-250,1e-249,1e-248,1e-247,1e-246,1e-245,1e-244,1e-243,1e-242,1e-241,1e-240,1e-239,1e-238,1e-237,1e-236,1e-235,1e-234,1e-233,1e-232,1e-231,1e-230,1e-229,1e-228,1e-227,1e-226,1e-225,1e-224,1e-223,1e-222,1e-221,1e-220,1e-219,1e-218,1e-217,1e-216,1e-215,1e-214,1e-213,1e-212,1e-211,1e-210,1e-209,1e-208,1e-207,1e-206,1e-205,1e-204,1e-203,1e-202,1e-201,1e-200,1e-199,1e-198,1e-197,1e-196,1e-195,1e-194,1e-193,1e-192,1e-191,1e-190,1e-189,1e-188,1e-187,1e-186,1e-185,1e-184,1e-183,1e-182,1e-181,1e-180,1e-179,1e-178,1e-177,1e-176,1e-175,1e-174,1e-173,1e-172,1e-171,1e-170,1e-169,1e-168,1e-167,1e-166,1e-165,1e-164,1e-163,1e-162,1e-161,1e-160,1e-159,1e-158,1e-157,1e-156,1e-155,1e-154,1e-153,1e-152,1e-151,1e-150,1e-149,1e-148,1e-147,1e-146,1e-145,1e-144,1e-143,1e-142,1e-141,1e-140,1e-139,1e-138,1e-137,1e-136,1e-135,1e-134,1e-133,1e-132,1e-131,1e-130,1e-129,1e-128,1e-127,1e-126,1e-125,1e-124,1e-123,1e-122,1e-121,1e-120,1e-119,1e-118,1e-117,1e-116,1e-115,1e-114,1e-113,1e-112,1e-111,1e-110,1e-109,1e-108,1e-107,1e-106,1e-105,1e-104,1e-103,1e-102,1e-101,1e-100,1e-99,1e-98,1e-97,1e-96,1e-95,1e-94,1e-93,1e-92,1e-91,1e-90,1e-89,1e-88,1e-87,1e-86,1e-85,1e-84,1e-83,1e-82,1e-81,1e-80,1e-79,1e-78,1e-77,1e-76,1e-75,1e-74,1e-73,1e-72,1e-71,1e-70,1e-69,1e-68,1e-67,1e-66,1e-65,1e-64,1e-63,1e-62,1e-61,1e-60,1e-59,1e-58,1e-57,1e-56,1e-55,1e-54,1e-53,1e-52,1e-51,1e-50,1e-49,1e-48,1e-47,1e-46,1e-45,1e-44,1e-43,1e-42,1e-41,1e-40,1e-39,1e-38,1e-37,1e-36,1e-35,1e-34,1e-33,1e-32,1e-31,1e-30,1e-29,1e-28,1e-27,1e-26,1e-25,1e-24,1e-23,1e-22,1e-21,1e-20,1e-19,1e-18,1e-17,1e-16,1e-15,1e-14,1e-13,1e-12,1e-11,1e-10,1e-9,1e-8,1e-7,1e-6,1e-5,1e-4,1e-3,1e-2,1e-1,1 };
      if (x2 >= 1.e6)
      {
        if (x2 >= 1.79e308) { x2 = 1.79e308; }
        _s_long n0 = 0; _s_long n2 = 310; while (n2 - n0 > 1) { _s_long n = (n2 + n0) / 2; if (x2 < cmp1[n]) { n2 = n; } else if (x2 >= cmp1[n]) { n0 = n; } }
        order = n0; x2 /= cmp1[n0];
      }
      else if (x2 < 0.001)
      {
        if (x2 < 2.23e-308) { x2 = 0.; }
        else
        {
          _s_long n0 = 0; _s_long n2 = 309; while (n2 - n0 > 1) { _s_long n = (n2 + n0) / 2; if (x2 < cmp2[n]) { n2 = n; } else if (x2 >= cmp2[n]) { n0 = n; } }
          if (n0 == 0) { n0 = 1; } order = n0 - 308; x2 /= cmp2[n0];
        }
      }
      _s_long i = _s_long(x2);
      _s_long n_int = 0; if (true) { _s_long i2 = i; do { ++n_int; i2 /= 10; } while (i2); }
      if (n_int > ndmmax)
      {
        _s_long n0 = 0; _s_long n2 = 7; while (n2 - n0 > 1) { _s_long n = (n2 + n0) / 2; if (x2 < cmp1[n]) { n2 = n; } else if (x2 >= cmp1[n]) { n0 = n; } }
        order += n0; n_int -= n0; x2 /= cmp1[n0];
        i = _s_long(x2);
      }
      _s_long n_order = 0; if (order) { if (order <= -100) { n_order = 5; } else if (order <= -10 || order >= 100) { n_order = 4; } else if (order <= -1 || order >= 10) { n_order = 3; } else { n_order = 2; } }
      _s_long n = n_neg + n_int + 1 + n_order;

      if (n > nchars) { buf[0] = x > 0. ? '+' : (x < 0. ? '-' : '0'); return 1; }

      _s_long pos = 0; if (n_neg) { buf[pos++] = '-'; }
      _s_long q = str_from_s_ll(i, buf + pos, n_int, false); pos += q;
      n_int = q;
      n = n_neg + n_int + 1 + n_order;
      buf[pos++] = '.';
      q = ndmmax - n_int; _s_long n_frac = nchars - n; if (q < n_frac) { n_frac = q; } if (nfracmax < n_frac) { n_frac = nfracmax; }
      q = 0;
      if (n_frac > 0)
      {
        if (i) { x2 -= double(i); } if (x2 < 0.) { x2 = 0.; } else if (x2 >= 0.9999999999999) { x2 = 0.9999999999999; }
        _s_ll fr = _s_ll(x2 * 1.e13 + 0.5), mult = 1000000000000ll;
        _s_ll fr_min = _pow10[nfrac_mm - n_frac];
        while (n_frac && fr >= fr_min) { buf[pos++] = char((fr / mult) + '0'); fr %= mult; mult /= 10; --n_frac; ++q; }
      }
      if (n_order) { buf[pos++] = 'e'; str_from_s_ll(order, buf + pos, n_order - 1, true); }
      return n + q;
    }

    template<class _>
    _s_ll _bmdx_str_impl<_>::str2i(const wchar_t* x, _s_ll xlen, _s_ll dflt, bool no_exc)
    {
        if (!x) { if (no_exc) { return dflt; } throw exc_str2i(); }
        if (xlen == -1) { xlen = _s_ll(wcslen(x)); }
        if (xlen <= 0) { if (no_exc) { return dflt; } throw exc_str2i(); }
        const _u_ll mn = _u_ll(1) << (8 * sizeof(_u_ll) - 1);
        const _u_ll mp = ~mn;
        _u_ll q(0); _s_long nq(0);
        bool bf = false;
        _s_ll pos = 0; wchar_t c = L' ';
        bool b_neg(false);
        do // once
        {
            while (pos < xlen) { c = x[pos]; if (c == L' ' || c == L'\t') { ++pos; } else { break; } }
              if (pos >= xlen) { bf = true; break; }
          c = x[pos]; if (c == L'+') { ++pos; } else if (c == L'-') { b_neg = true; ++pos; }
              if (pos >= xlen) { bf = true; break; }
            while (pos < xlen) { c = x[pos]; if (c == L' ' || c == L'\t') { ++pos; } else { break; } }
              if (pos >= xlen) { bf = true; break; }
            while (pos < xlen) { c = x[pos]; if (c >= L'0' && c <= L'9') { q = q * 10 + int(c - L'0'); ++nq; ++pos; if (nq > 19) { bf = true; break; } } else { break; } }
              if (bf) { break; } if (pos >= xlen) { bf = nq == 0; break; }
            while (pos < xlen) { c = x[pos]; if (c == L' ' || c == L'\t') { ++pos; } else { break; } }
              if (pos < xlen) { bf = true; break; }
        } while (false);
        _s_ll z(0);
        do // once
        {
          if (bf) { break; }
          if (b_neg) { if (q < mn) { z = -_s_ll(q); } else if (q == mn) { z = -_s_ll(mp) - 1; } else { bf = true; break; } }
            else { if (q <= mp) { z = _s_ll(q); } else { bf = true; break; } }
        } while (false);
        if (!bf) { return z; }
        if (no_exc) { return dflt; }
        throw exc_str2i();
    }

    template<class _>
    double _bmdx_str_impl<_>::str2f(const wchar_t* x, _s_ll xlen, double dflt, bool no_exc)
    {
        if (!x) { if (no_exc) { return dflt; } throw exc_str2f(); }
        if (xlen == -1) { xlen = _s_ll(wcslen(x)); }
        if (xlen <= 0) { if (no_exc) { return dflt; } throw exc_str2f(); }
        bool bf = false;
        _s_ll pos = 0; wchar_t c = L' ';
        _s_ll q1(0), q2(0), q3(0); _s_long nq1(0), nq2(0), nq3(0); bool b_neg1(false), b_neg3(false); _s_ll nsd(0);
        do // once
        {
            while (pos < xlen) { c = x[pos]; if (c == L' ' || c == L'\t') { ++pos; } else { break; } }
              if (pos >= xlen) { bf = true; break; }
          c = x[pos]; if (c == L'+') { ++pos; } else if (c == L'-') { b_neg1 = true; ++pos; }
              if (pos >= xlen) { bf = true; break; }
            while (pos < xlen) { c = x[pos]; if (c == L' ' || c == L'\t') { ++pos; } else { break; } }
              if (pos >= xlen) { bf = true; break; }
            while (pos < xlen) { c = x[pos]; if (c >= L'0' && c <= L'9') { q1 = q1 * 10 + int(c - L'0'); ++nq1; if (q1) { ++nsd; } ++pos; if (nq1 > 19) { bf = true; break; } } else { break; } }
              if (bf) { break; } if (pos >= xlen) { bf = nq1 == 0; break; }
          c = x[pos];
            if (c == L'.')
            {
              ++pos;
              while (pos < xlen) { c = x[pos]; if (c >= L'0' && c <= L'9') { q2 = q2 * 10 + int(c - L'0'); ++nq2; if (q1 || (!q1 && q2)) { ++nsd; } ++pos; if (nq2 == 18) { break; } } else { break; } }
            }
            if (pos >= xlen) { bf = nq1 == 0 && nq2 == 0; break; }
          while (pos < xlen) { c = x[pos]; if (c == L' ' || c == L'\t') { ++pos; } else { break; } }
            if (pos >= xlen) { break; }
          c = x[pos];
            if (c == L'e' || c == L'E')
            {
              ++pos;
              c = x[pos]; if (c == L'+') { ++pos; } else if (c == L'-') { b_neg3 = true; ++pos; }
                  if (pos >= xlen) { bf = true; break; }
              while (pos < xlen) { c = x[pos]; if (c >= L'0' && c <= L'9') { q3 = q3 * 10 + int(c - L'0'); ++nq3; ++pos; if (nq3 > 3) { bf = true; break; } } else { break; } }
                if (bf) { break; }
                if (nq3 == 0) { bf = true; break; }
            }
            else { bf = true; break; }
            while (pos < xlen) { c = x[pos]; if (c == L' ' || c == L'\t') { ++pos; } else { break; } }
              if (pos < xlen) { bf = true; break; }
        } while (false);
        double z(0.);
        do // once
        {
          if (bf) { break; }
          if (nq2)
          {
            const _s_ll nprec = 15;
            _s_ll m(1); for (_s_ll i = 0; i < nq2; ++i) { m *= 10; }
            if (nsd >= nprec)
            {
              z = (q1 * m + q2 + 0.0) / m;
            }
            else
            {
              _s_ll m2 = 1; for (_s_ll i = nsd; i < nprec; ++i) { m2 *= 10; }
              z = ((q1 * m + q2) * m2 + 0.1) / (m * m2);
            }
          }
          else { z = double(q1); }
          if (b_neg1) { z = -z; }
          if (q3)
          {
            if (b_neg3) { q3 = -q3; }
            if (q3 >= 280) { z *= 1.e280; q3 -= 280; if (q3 > 50) { bf = true; break; } }
              else if (q3 >= -280) { }
              else { z *= 1.e-280; q3 += 280; if (q3 < -50) { z = 0.; break; } }
            z *= std::pow(10., int(q3));
          }
        } while (false);
        if (!bf) { return z; }
        if (no_exc) { return dflt; }
        throw exc_str2f();
    }

      // Trivial analog of str2i(wchar_t*).
    template<class _>
    _s_ll _bmdx_str_impl<_>::str2i(const char* x, _s_ll xlen, _s_ll dflt, bool no_exc)
    {
if (!x) { if (no_exc) { return dflt; } throw exc_str2i(); } if (xlen == -1) { xlen = _s_ll(strlen(x)); } if (xlen <= 0) { if (no_exc) { return dflt; } throw exc_str2i(); } const _u_ll mn = _u_ll(1) << (8 * sizeof(_u_ll) - 1); const _u_ll mp = ~mn; _u_ll q(0); _s_long nq(0); bool bf = false; _s_ll pos = 0; char c = ' '; bool b_neg(false);
do { while (pos < xlen) { c = x[pos]; if (c == ' ' || c == '\t') { ++pos; } else { break; } } if (pos >= xlen) { bf = true; break; } c = x[pos]; if (c == '+') { ++pos; } else if (c == '-') { b_neg = true; ++pos; } if (pos >= xlen) { bf = true; break; } while (pos < xlen) { c = x[pos]; if (c == ' ' || c == '\t') { ++pos; } else { break; } } if (pos >= xlen) { bf = true; break; } while (pos < xlen) { c = x[pos]; if (c >= '0' && c <= '9') { q = q * 10 + int(c - '0'); ++nq; ++pos; if (nq > 19) { bf = true; break; } } else { break; } } if (bf) { break; } if (pos >= xlen) { bf = nq == 0; break; } while (pos < xlen) { c = x[pos]; if (c == ' ' || c == '\t') { ++pos; } else { break; } } if (pos < xlen) { bf = true; break; } } while (false);
_s_ll z(0); do { if (bf) { break; } if (b_neg) { if (q < mn) { z = -_s_ll(q); } else if (q == mn) { z = -_s_ll(mp) - 1; } else { bf = true; break; } } else { if (q <= mp) { z = _s_ll(q); } else { bf = true; break; } } } while (false);
if (!bf) { return z; } if (no_exc) { return dflt; } throw exc_str2i();
    }

      // Trivial analog of str2f(wchar_t*).
    template<class _>
    double _bmdx_str_impl<_>::str2f(const char* x, _s_ll xlen, double dflt, bool no_exc)
    {
if (!x) { if (no_exc) { return dflt; } throw exc_str2f(); } if (xlen == -1) { xlen = _s_ll(strlen(x)); } if (xlen <= 0) { if (no_exc) { return dflt; } throw exc_str2f(); } bool bf = false; _s_ll pos = 0; char c = ' '; _s_ll q1(0), q2(0), q3(0); _s_long nq1(0), nq2(0), nq3(0); bool b_neg1(false), b_neg3(false);
do { while (pos < xlen) { c = x[pos]; if (c == ' ' || c == '\t') { ++pos; } else { break; } } if (pos >= xlen) { bf = true; break; } c = x[pos]; if (c == '+') { ++pos; } else if (c == '-') { b_neg1 = true; ++pos; } if (pos >= xlen) { bf = true; break; } while (pos < xlen) { c = x[pos]; if (c == ' ' || c == '\t') { ++pos; } else { break; } } if (pos >= xlen) { bf = true; break; } while (pos < xlen) { c = x[pos]; if (c >= '0' && c <= '9') { q1 = q1 * 10 + int(c - '0'); ++nq1; ++pos; if (nq1 > 19) { bf = true; break; } } else { break; } } if (bf) { break; } if (pos >= xlen) { bf = nq1 == 0; break; }
c = x[pos]; if (c == '.') { ++pos; while (pos < xlen) { c = x[pos]; if (c >= '0' && c <= '9') { q2 = q2 * 10 + int(c - '0'); ++nq2; ++pos; if (nq2 == 18) { break; } } else { break; } } } if (pos >= xlen) { bf = nq1 == 0 && nq2 == 0; break; } while (pos < xlen) { c = x[pos]; if (c == ' ' || c == '\t') { ++pos; } else { break; } } if (pos >= xlen) { break; } c = x[pos]; if (c == 'e' || c == 'E') { ++pos; c = x[pos]; if (c == '+') { ++pos; } else if (c == '-') { b_neg3 = true; ++pos; } if (pos >= xlen) { bf = true; break; } while (pos < xlen) { c = x[pos]; if (c >= '0' && c <= '9') { q3 = q3 * 10 + int(c - '0'); ++nq3; ++pos; if (nq3 > 3) { bf = true; break; } } else { break; } } if (bf) { break; } if (nq3 == 0) { bf = true; break; } } else { bf = true; break; } while (pos < xlen) { c = x[pos]; if (c == ' ' || c == '\t') { ++pos; } else { break; } } if (pos < xlen) { bf = true; break; } } while (false);
double z(0.); do { if (bf) { break; } if (nq1 > 0) { if (nq2 > 0) { const _s_ll nprec = 15; _s_ll m(1); for (_s_ll i = 0; i < nq2; ++i) { m *= 10; } _s_ll nd = nq1 + nq2; if (nd >= nprec) { z = (q1 * m + q2 + 0.0) / m; } else { _s_ll m2 = 1; for (_s_ll i = nd; i < nprec; ++i) { m2 *= 10; } z = ((q1 * m + q2) * m2 + 0.1) / (m * m2); } } else { z = double(q1); } } if (b_neg1) { z = -z; } if (q3) { if (b_neg3) { q3 = -q3; } if (q3 >= 280) { z *= 1.e280; q3 -= 280; if (q3 > 50) { bf = true; break; } } else if (q3 >= -280) { } else { z *= 1.e-280; q3 += 280; if (q3 < -50) { z = 0.; break; } } z *= std::pow(10., int(q3)); } } while (false);
if (!bf) { return z; } if (no_exc) { return dflt; } throw exc_str2f();
    }
  }



    // Fixed-length string of char.
    //    Automatic conversion from std string, wstring, integer types, floating-point types.
    //    Min., max. length: 4, 2^20 - 1.
  template<_s_long nmax_>
  struct flstr_t
  {
  private: enum { _m_size = 0xfffff, _sh_res = 20 };
  public:
    typedef flstr_t t_string; enum { nmin_c = 4, nmax_c = nmax_ >= nmin_c ? (nmax_ <= _m_size ? nmax_ : _s_long(_m_size)) : nmin_c, res_max = _m_size, res_min = -res_max };

    flstr_t() { _nr = 1 << _sh_res; _x[0] = 0; }

      // res(): 1: success; 0: only part is added; -1: length() == nmax() already.
    flstr_t(const char* ps, _s_ll n = -1) throw () { _nr = 0; _x[0] = 0; _set_res_u(_append_s(ps, n >= 0 ? n : -1)); }
    flstr_t(const std::string& s) throw () { _nr = 0; _x[0] = 0; _set_res_u(_append_s(s.c_str(), s.length())); }
    flstr_t(const wchar_t* ps, _s_ll n = -1) throw () { _nr = 0; _x[0] = 0; _set_res_u(_append_wcs(ps, n >= 0 ? n : -1)); }
    flstr_t(const std::wstring& s) throw () { _nr = 0; _x[0] = 0; _set_res_u(_append_wcs(s.c_str(), s.length())); }

    template<_s_long nmax2> flstr_t(const flstr_t<nmax2>& s) throw () { _nr = 0; _x[0] = 0; _set_res_u(_append_s(s.c_str(), s.length())); }

    flstr_t(double x, _s_long ndmmax = 6, _s_long nfracmax = 12) throw () { _s_long n = conv::_bmdx_str_impl<>::str_from_double(x, _x, nmax(), ndmmax, nfracmax); _set_end_u(n >= 0 ? n : 0); _set_res_u(n >= 2 ? 1 : (n >= 0 ? 0 : -1)); }
    flstr_t(_s_ll x, bool b_signed) throw () { _s_long n = conv::_bmdx_str_impl<>::str_from_s_ll(x, _x, nmax(), b_signed); _set_end_u(n >= 0 ? n : 0); _set_res_u(n >= 2 || (n == 1 && _x[0] != '-' && _x[0] != '+') ? 1 : (n >= 0 ? 0 : -1)); }

    flstr_t(signed short x) throw () { new (this) flstr_t(_s_ll(x), true); }
    flstr_t(signed int x) throw () { new (this) flstr_t(_s_ll(x), true); }
    flstr_t(signed long x) throw () { new (this) flstr_t(_s_ll(x), true); }
    flstr_t(signed long long x) throw () { new (this) flstr_t(_s_ll(x), true); }
    flstr_t(unsigned short x) throw () { new (this) flstr_t(_s_ll(x), false); }
    flstr_t(unsigned int x) throw () { new (this) flstr_t(_s_ll(x), false); }
    flstr_t(unsigned long x) throw () { new (this) flstr_t(_s_ll(x), false); }
    flstr_t(unsigned long long x) throw () { new (this) flstr_t(_s_ll(x), false); }

    flstr_t(const void* x) throw () { new (this) flstr_t((_s_ll)x, false); }

    _s_long nmax() const  throw () { return nmax_c; }
    _s_long length() const throw () { return _nr & _m_size; }
    _s_long is_full() const throw () { return length() == nmax(); }
    _s_long res() const throw () { return _nr >> _sh_res; }
    const char* c_str() const throw () { return _x; }
    _s_long type() const throw () { return 0; }

      // If n >= 0, chars [pos0..min(pos0+n, length())) are returned.
      //  If n < 0, chars [pos0..length()) are returned.
    t_string substr(_s_long pos0, _s_long n) const throw() { _s_long n0 = length(); if (n <= 0) { if (n == 0) { return t_string(); } n = n0; } if (n > n0) { n = n0; } if (pos0 < 0) { pos0 = 0; } else if (pos0 > n0) { pos0 = n0; } if (pos0 + n > n0) { n = n0 - pos0; } return t_string(_x + pos0, n); }
      // Starts searching from pos0.
      //  (find_any) n_pcc >= 0 specifies num of chars. in pcc.
      // Returns [0..length()), or -1 if not found.
    _s_long find(char c, _s_long pos0 = 0) const throw() { _s_long pos2 = length(); if (pos0 < 0) { pos0 = 0; } while (pos0 < pos2) { if (_x[pos0] == c) { return pos0; } ++pos0; } return -1; }
    _s_long find_any(const char* pcc, _s_long pos0 = 0, _s_long n_pcc = -1) const throw() { if (!pcc) { return -1; } _s_long pos2 = length(); if (pos0 < 0) { pos0 = 0; } if (pos0 >= pos2) { return -1; } if (n_pcc < 0) { n_pcc = 0; const char* p = pcc; while (*p++) { ++n_pcc; } } while (pos0 < pos2) { char c = _x[pos0]; for (_s_long i = 0; i < n_pcc; ++i) { if (c == pcc[i]) { return pos0; } } ++pos0; } return -1; }

    inline operator std::string () const { return _x; }
    inline operator std::wstring () const { return wstr(); }
    inline std::string str() const { return _x; }
    std::wstring wstr() const // UTF-8 --> UTF-16
    {
      std::wstring s; _s_ll i = 0, n = length();
      while (i < n)
      {
        unsigned char c = _x[i];
        if ((c & 0x80) == 0) { s += wchar_t(c); ++i; }
        else
        {
          _s_long nbchr = 0;
          while (((c << nbchr) & 0xc0) == 0xc0) { ++nbchr; }
          if (nbchr >= 1 && nbchr <= 5 && i + nbchr < n)
          {
            _s_long q(c & (0x3f >> nbchr)); ++i;
            do { c = _x[i]; if ((c & 0xc0) != 0x80) { q = L'?'; break; } q = (q << 6) | (c & 0x3f); ++i; } while (--nbchr);
            if (q > 0x10ffff) { s += L'?'; }
              else if (q >= 0x10000) { q -= 0x10000; s += wchar_t(0xd800 | (q >> 10)); s += wchar_t(0xdc00 | (q & 0x3ff)); }
              else { s += wchar_t(q); }
          }
          else { s += L'?'; ++i; }
        }
      }
      return s;
    }

      // Before setting, x is limited by res_min..res_max.
    void set_res(_s_long x) throw () { if (x < res_min) { x = res_min; } else if (x > res_max) { x = res_max; } _set_res_u(x); }

      // res(): 1: success; 0: n > nmax(), so nmax() is set; -1: n is negative, nothing changed.
    void resize(_s_long n, char c = ' ') throw () { if (n < 0) { _set_res_u(-1); return; } bool b = n > nmax(); if (b) { n = nmax(); } _s_long delta = n - length(); if (delta > 0) { _set_u(c, _x + length(), delta); } if (delta != 0) { _set_end_u(n); } _set_res_u(b ? 0 : 1); }

      // res(): 1: success; -1: length() == nmax() already.
    void operator += (char c) throw () { _s_long n = length(); if (n >= nmax()) { _set_res_u(-1); return; } _x[n] = c; ++n; _x[n] = 0; _nr = (1 << _sh_res) | n; }

    void operator += (double x) throw () { *this += t_string(x); }
    void operator += (wchar_t x) throw () { append(&x, 1); }

    void operator += (signed short x) throw () { *this += t_string(x); }
    void operator += (signed int x) throw () { *this += t_string(x); }
    void operator += (signed long x) throw () { *this += t_string(x); }
    void operator += (signed long long x) throw () { *this += t_string(x); }
    void operator += (unsigned short x) throw () { *this += t_string(x); }
    void operator += (unsigned int x) throw () { *this += t_string(x); }
    void operator += (unsigned long x) throw () { *this += t_string(x); }
    void operator += (unsigned long long x) throw () { *this += t_string(x); }

    t_string operator + (char c) throw () { t_string s2(*this); s2 += c; return s2; }

    t_string operator + (double x) throw () { t_string s2(*this); s2 += x; return s2; }
    t_string operator + (wchar_t x) throw () { t_string s2(*this); s2 += x; return s2; }

    t_string operator + (signed short x) throw () { t_string s2(*this); s2 += x; return s2; }
    t_string operator + (signed int x) throw () { t_string s2(*this); s2 += x; return s2; }
    t_string operator + (signed long x) throw () { t_string s2(*this); s2 += x; return s2; }
    t_string operator + (signed long long x) throw () { t_string s2(*this); s2 += x; return s2; }
    t_string operator + (unsigned short x) throw () { t_string s2(*this); s2 += x; return s2; }
    t_string operator + (unsigned int x) throw () { t_string s2(*this); s2 += x; return s2; }
    t_string operator + (unsigned long x) throw () { t_string s2(*this); s2 += x; return s2; }
    t_string operator + (unsigned long long x) throw () { t_string s2(*this); s2 += x; return s2; }

      // res(): 1: success; 0: only part is added; -1: length() == nmax() already; -2: ps == 0.
    void append (const char* ps, _s_ll n) throw () { _set_res_u(_append_s(ps, n >= 0 ? n : -1)); }
    void operator += (const char* ps) throw () { _set_res_u(_append_s(ps, -1)); }
    void operator += (const std::string& s) throw () { _set_res_u(_append_s(s.c_str(), s.length())); }
    t_string& operator = (const char* ps) throw () { resize(0); _set_res_u(_append_s(ps, -1)); return *this; }
    t_string& operator = (const std::string& s) throw () { resize(0); _set_res_u(_append_s(s.c_str(), s.length())); return *this; }
    t_string operator + (const char* ps) throw () { t_string s2(*this); s2 += ps; return s2; }
    t_string operator + (const std::string& s) throw () { t_string s2(*this); s2 += s; return s2; }

    bool operator == (const t_string& s) const throw () { const _s_long n = length(); if (n != s.length()) { return false; } for (_s_long i = 0; i < n; ++i) { if (_x[i] != s._x[i]) { return false; } } return true; }
    bool operator != (const t_string& s) const throw () { return !operator==(s); }

    void append (const wchar_t* ps, _s_ll n) throw () { _set_res_u(_append_wcs(ps, n >= 0 ? n : -1)); }
    void operator += (const wchar_t* ps) throw () {  _set_res_u(_append_wcs(ps, -1)); }
    void operator += (const std::wstring& s) throw () { _set_res_u(_append_wcs(s.c_str(), s.length())); }
    t_string& operator = (const wchar_t* ps) throw () { resize(0); _set_res_u(_append_wcs(ps, -1)); return *this; }
    t_string& operator = (const std::wstring& s) throw () { resize(0); _set_res_u(_append_wcs(s.c_str(), s.length())); return *this; }
    t_string operator + (const wchar_t* ps) throw () { t_string s2(*this); s2 += ps; return s2; }
    t_string operator + (const std::wstring& s) throw () { t_string s2(*this); s2 += s; return s2; }

    template<_s_long nmax2> void operator += (const flstr_t<nmax2>& s) throw () { _set_res_u(_append_s(s.c_str(), s.length())); }
    template<_s_long nmax2> t_string operator + (const flstr_t<nmax2>& s) throw () { t_string s2(*this); s2 += s; return s2; }
    template<_s_long nmax2> t_string& operator = (const flstr_t<nmax2>& s) throw () { resize(0); _set_res_u(append(s.c_str(), s.length())); return *this; }

    char& operator[] (_s_long i) { return _x[i]; }
    const char& operator[] (_s_long i) const { return _x[i]; }

    private:
      _s_long _nr; char _x[nmax_c + 1];
      inline void _copy_u(const char* buf, char* dest, _s_long n) { while (n > 0) { *dest++ = *buf++; --n; } }
        // Copies <= nc chars., stops on null character (it's not copied).
        //  Returns num. of characters copied. Num. < nmax means "null character is reached".
      inline _s_long _copy_le_u(const char* buf, char* dest, _s_long nc) { _s_long n = 0; while (nc > 0) { char c = *buf++; if (!c) { break; } *dest++ = c; ++n; --nc; } return n; }
      inline void _set_u(char c, char* dest, _s_long n) { while (n > 0) { *dest++ = c; --n; } }
      inline void _set_end_u(_s_long n) { _x[n] = 0; _nr &= ~_s_long(_m_size); _nr |= n; }
      inline void _set_res_u(_s_long x) throw () { _nr &= _m_size; _nr |= (x << _sh_res); }
        // 1: success, 0: partially copied, -1: string is full, cannot append, -2: invalid ps, nsrc.
      _s_long _append_s(const char* ps, _s_ll nsrc = -1) throw ()
      {
        if (!(ps && nsrc >= -1)) { return -2; }
        _s_long ncmax = nmax() - length(); if (nsrc > 0 && ncmax <= 0) { return -1; }
        if (nsrc >= 0)
        {
          bool b_part = nsrc > ncmax; if (b_part) { nsrc = ncmax; }
          _copy_u(ps, &_x[length()], _s_long(nsrc)); _set_end_u(length() + _s_long(nsrc));
          return b_part ? 0 : 1;
        }
        else
        {
          _s_long n = _copy_le_u(ps, &_x[length()], ncmax + 1);
          if (n == ncmax + 1) { _set_end_u(nmax()); return 0; }
            else { _set_end_u(length() + n); return 1; }
        }
      }
        // 1: success, 0: partially copied, -1: string is full, cannot append, -2: invalid ps, nsrc.
      inline _s_long _append_wcs(const wchar_t* ps, _s_ll nsrc = -1)
      {
        _s_long res; _s_ll pos = 0;
        do { res = _append_wc(ps, pos, nsrc); } while (res == 1);
        if (res == 0) { return 1; }
        if (res == -1 && pos > 0) { return 0; }
        return res;
      }
        // nsrc: >=0 - num. of characters available in the input, starting from ps. -1 - input is 0-terminated.
        // 1: success, 0: end of input, -1: string is full, cannot append, -2: invalid ps, pos, nsrc.
      inline _s_long _append_wc(const wchar_t* ps, _s_ll& pos, _s_ll nsrc)
      {
        if (!(ps && nsrc >= -1 && pos >= 0)) { return -2; } const wchar_t* p2 = ps + pos;
        if (nsrc >= 0 && pos >= nsrc) { return 0; }
        _s_long q = p2[0] & 0xffff; if (nsrc == -1 && q == 0) { return 0; }
        _s_ll dpos = 1;
        if (q >= 0xd800 && q < 0xe000)
        {
          if (q >= 0xdc00 || (nsrc == -1 && p2[1] == 0) || (nsrc >= 0 && pos + 1 >= nsrc)) { q = L'?'; }
          else
          {
            _s_long q2 = _s_long(p2[1]);
            if (q2 >= 0xdc00 && q2 < 0xe000) { q = (((q - 0xd800) << 10) | (q2 - 0xdc00)) + 0x10000; dpos += 1; }
              else { q = L'?'; }
          }
        }
        _s_long n = length();
        _s_long nmdest = nmax() - n;
        if (q <= 0x7f) { if (nmdest < 1) { return -1; } }
          else if (q <= 0x7ff) { if (nmdest < 2) { return -1; } }
          else if (q <= 0xffff) { if (nmdest < 3) { return -1; } }
          else { if (nmdest < 4) { return -1; } }
        if (q <= 0x7f) { _x[n] = char(q); _set_end_u(n + 1); }
          else if (q <= 0x7ff) { _x[n] = char((q >> 6) | 0xc0); _x[n + 1] = char((q & 0x3f) | 0x80); _set_end_u(n + 2); }
          else if (q <= 0xffff) { _x[n] = char((q >> 12) | 0xe0); _x[n + 1] = char(((q >> 6) & 0x3f) | 0x80); _x[n + 2] = char((q & 0x3f) | 0x80); _set_end_u(n + 3); }
          else { _x[n] = char((q >> 18) | 0xf0); _x[n + 1] = char(((q >> 12) & 0x3f) | 0x80); _x[n + 2] = char(((q >> 6) & 0x3f) | 0x80); _x[n + 3] = char((q & 0x3f) | 0x80); _set_end_u(n + 4); }
        pos += dpos;
        return 1;
      }
  };

  template<_s_long nmax_> inline flstr_t<nmax_> operator + (const char* ps, const flstr_t<nmax_>& s2) throw () { flstr_t<nmax_> s3(ps); s3 += s2; return s3; }
  template<_s_long nmax_> inline flstr_t<nmax_> operator + (const std::string& s, const flstr_t<nmax_>& s2) throw () { flstr_t<nmax_> s3(s); s3 += s2; return s3; }

  template<_s_long nmax_> inline flstr_t<nmax_> operator + (const wchar_t* ps, const flstr_t<nmax_>& s2) throw () { flstr_t<nmax_> s3(ps); s3 += s2; return s3; }
  template<_s_long nmax_> inline flstr_t<nmax_> operator + (const std::wstring& s, const flstr_t<nmax_>& s2) throw () { flstr_t<nmax_> s3(s); s3 += s2; return s3; }

  template<_s_long nmax_> inline std::ostream& operator << (std::ostream& stm, const flstr_t<nmax_>& s2) throw () { stm << s2.str(); return stm; }

  namespace conv
  {
      // Converting numbers and pointer values to strings, using default format.
    template<class T> inline std::string ntocs(const T& x) { typedef flstr_t<25> S; return S(x).str(); }
    template<class T> inline std::wstring ntows(const T& x) { typedef flstr_t<25> S; return S(x).wstr(); }
    inline std::string ftocs(double x, _s_long ndmmax = 6, _s_long nfracmax = 12) { typedef flstr_t<25> S; return S(x, ndmmax, nfracmax).str(); }
    inline std::wstring ftows(double x, _s_long ndmmax = 6, _s_long nfracmax = 12) { typedef flstr_t<25> S; return S(x, ndmmax, nfracmax).wstr(); }
    inline std::string itocs(_s_ll x, bool b_signed) { typedef flstr_t<25> S; return S(x, b_signed).str(); }
    inline std::wstring itows(_s_ll x, bool b_signed) { typedef flstr_t<25> S; return S(x, b_signed).wstr(); }

      // Converts x to decimal string representation.
      //    nchars >= 0.
      // NOTE Max. number of characters in string repr. of x is 20.
      // Returns:
      //    >=2 - success.
      //    1:
      //      a) x >= 10 : "+",
      //      b) x == 0..9: "0".."9".
      //      c) x < 0: "-".
      //    0 - nchars == 0.
      //    -1 - invlaid input parameter (buf == 0, nchars < 0).
    inline static _s_long str_from_s_ll(_s_ll x, char* buf, _s_long nchars, bool b_signed = true) throw() { return _bmdx_str_impl<>::str_from_s_ll(x, buf, nchars, b_signed); }

      // ndmmax (max. num. of digits in mantissa) is limited to 1..14.
      //  Max number of output characters == limited ndmmax + 7 (0..1 sign, 1 point, 0..1 "e", 0..4 order).
      // nfracmax (max. num. of digits in fractional part) is limited to 0..ndmmax-1.
      // abs(x) < 1.e-3 or  >= 1.e6 is shown in exponential format.
      //  if x cannot fit in ndmmax, it is also shown in exponential format.
      // Returns:
      //    >=2 - success.
      //    1 - string representation is longer than nchars, so only sign is extracted: "+", "0", "-".
      //    0 - nchars == 0.
      //    -1 - invlaid input parameter (buf == 0, nchars < 0).
    inline static _s_long str_from_double(double x, char* buf, _s_long nchars, _s_long ndmmax = 6, _s_long nfracmax = 12) throw() { return _bmdx_str_impl<>::str_from_double(x, buf, nchars, ndmmax, nfracmax); }

      // Converts string to number. On failure, returns dflt, or (if no_exc == false) generates exc_str2i.
      //  For char* and wchar_t* versions, xlen == -1 means that x is 0-terminated.
    inline static _s_ll str2i(const std::wstring& x, _s_ll dflt = 0, bool no_exc = true) { return _bmdx_str_impl<>::str2i(x.c_str(), _s_ll(x.length()), dflt, no_exc); }
    inline static _s_ll str2i(const wchar_t* x, _s_ll xlen, _s_ll dflt = 0, bool no_exc = true) { return _bmdx_str_impl<>::str2i(x, xlen, dflt, no_exc); }
    inline static _s_ll str2i(const std::string& x, _s_ll dflt = 0, bool no_exc = true) { return _bmdx_str_impl<>::str2i(x.c_str(), _s_ll(x.length()), dflt, no_exc); }
    inline static _s_ll str2i(const char* x, _s_ll xlen, _s_ll dflt = 0, bool no_exc = true) { return _bmdx_str_impl<>::str2i(x, xlen, dflt, no_exc); }

      // Converts string to number. On failure, returns dflt, or (if no_exc == false) generates exc_str2f.
      //  For char* and wchar_t* versions, xlen == -1 means that x is 0-terminated.
    inline static double str2f(const std::wstring& x, double dflt = 0., bool no_exc = true) { return _bmdx_str_impl<>::str2f(x.c_str(), _s_ll(x.length()), dflt, no_exc); }
    inline static double strn2f(const wchar_t* x, _s_ll xlen, double dflt, bool no_exc) { return _bmdx_str_impl<>::str2f(x, xlen, dflt, no_exc); }
    inline static double str2f(const std::string& x, double dflt = 0., bool no_exc = true) { return _bmdx_str_impl<>::str2f(x.c_str(), _s_ll(x.length()), dflt, no_exc); }
    inline static double strn2f(const char* x, _s_ll xlen, double dflt, bool no_exc) { return _bmdx_str_impl<>::str2f(x, xlen, dflt, no_exc); }
  }
}

#ifdef _bmdxpl_Wnds
  #include <windows.h>
#else
#endif



namespace bmdx
{
  struct exc_carr_alloc_asg : std::exception { const char* what() const throw () { return "c[pp]array_t/op=|cc/alloc"; } };
  struct _exc_carr_asg : std::exception { const char* what() const throw () { return "_carr_asgx_t::try_asg"; } };
  struct _exc_carr_new : std::exception { const char* what() const throw () { return "_carr_asgx_t:try_cc"; } };
  template<class T, bool no_exc_asg> struct carray_t;
  template<class T, bool no_exc_asg> struct cpparray_t;
  template<class A, class Ax> struct _carr_aux1_t { inline static void try_asg(A& dest, const A& src) { (Ax&)dest = (const Ax&)src; if (dest.n() != src.n()) { throw _exc_carr_asg(); } } inline static void try_cc(void* pdest, const A& src) { new (pdest) Ax((const Ax&)src); if (((A*)pdest)->n() != src.n()) { throw _exc_carr_new(); } } };
    // _carr_asgx_t provides assignment and placement copy construction (try_asg, try_cc)
    //    with forced exceptions on error for all carray_t and cpparray_t variants.
    //    (Easier error processing for deeply nested structures.)
  template<class A, class _ = bmdx_meta::nothing> struct _carr_asgx_t { enum { is_carr_any = 0, is_cpparray = 0 }; typedef A t_ax; inline static void try_asg(A& dest, const A& src) { dest = src; } inline static void try_cc(void* pdest, const A& src) { new (pdest) A(src); } };
  template<class T> struct _carr_asgx_t<carray_t<T, true>, bmdx_meta::nothing> : _carr_aux1_t<carray_t<T, true>, carray_t<T, false> > { enum { is_carr_any = 1, is_cpparray = 0 }; typedef carray_t<T, false> t_ax; inline static void check_exc_alloc() { } };
  template<class T> struct _carr_asgx_t<cpparray_t<T, true>, bmdx_meta::nothing> : _carr_aux1_t<cpparray_t<T, true>, cpparray_t<T, false> > { enum { is_carr_any = 1, is_cpparray = 1 }; typedef cpparray_t<T, false> t_ax; inline static void check_exc_alloc() { } };
  template<class T> struct _carr_asgx_t<carray_t<T, false>, bmdx_meta::nothing> : _carr_asgx_t<carray_t<T, false>, int> { enum { is_carr_any = 1, is_cpparray = 0 }; inline static void check_exc_alloc() { throw exc_carr_alloc_asg(); } };
  template<class T> struct _carr_asgx_t<cpparray_t<T, false>, bmdx_meta::nothing> : _carr_aux1_t<cpparray_t<T, false>, int> { enum { is_carr_any = 1, is_cpparray = 1 }; inline static void check_exc_alloc() { throw exc_carr_alloc_asg(); } };

  template<class T>
  struct _carray_base_t
  {
    typedef T t_value; typedef size_t t_size;
    _carray_base_t() throw() : _n(0), _data(0), _psf1(_s_psf1()), _pff(_spff) {}

    inline _s_ll n() const throw()  { return _n; }

    inline const t_value* pd() const throw() { return _data; }
    inline t_value* pd() throw() { return _data; }

    inline const t_value& operator[] (t_size i) const throw() { return _data[i]; }
    inline t_value& operator[] (t_size i) throw() { return _data[i]; }

      // Set all existing (or [i0..i2) cut to [0..n()) elements to x, using operator=.
      //  Returns: 1 - all set successfully, 0 - all assignments failed, -1 - part of assignments failed.
      //    (If operator= always succeeds, the function will also succeed and return 1.)
    inline int set(const t_value& x) throw()
    {
      t_size i0 = 0; t_size nf(0); while (i0 < t_size(_n)) { try { for (; i0 < t_size(_n); ++i0) { _carr_asgx_t<T>::try_asg(_data[i0], x); } } catch (...) { ++i0; ++nf; } }
      return nf ? (nf == t_size(_n) ? 0 : -1) : 1;
    }
    inline int set(const t_value& x, _s_ll i0, _s_ll i2) throw()
    {
      if (i0 < 0) { i0 = 0; } if (i2 > _n) { i2 = _n; }
      t_size _i0 = t_size(i0); const t_size _i2 = t_size(i2); if (!(_s_ll(_i0) == i0 && _s_ll(_i2) == i2)) { return 0; }
      if (_i0 >= _i2) { return 1; }
      const t_size nset = _i2 - _i0;
      t_size nf(0); while (_i0 < _i2) { try { for (; _i0 < _i2; ++_i0) { _carr_asgx_t<T>::try_asg(_data[_i0], x); } } catch (...) { ++_i0; ++nf; } }
      return nf ? (nf == nset ? 0 : -1) : 1;
    }

      // Reallocate storage. Copy, destroy, initialize elements as specified.
      //  n >= 0: the new number of elements of type T.
      //  dmode: 1 - use local destructor, 0 - do not call destructor.
      //  imode: 4 - copy existing elems. with memcpy, 1 - copy with local T(const T&), 0 - no copying (new allocation only).
      //  px:
      //    != 0 - if elements are added, initialize them to *px (depending on imode).
      //    == 0 with imode == 0, 4 - do not initialize.
      //    == 0 with imode == 1 - initialize as T().
      // Returns: true - success, false - failure, no changes.
      // NOTE realloc() proceeds with realloc/copy/init even if n == n().
    bool realloc(_s_ll n, int dmode, int imode, const t_value* px) throw()
    {
      if (!(n >= 0 && dmode == (dmode & 1) && (imode == 4 || imode == (imode & 1)))) { return false; }
      T* pd2 = 0;
      if (n > 0)
      {
        pd2 = (T*)_alloc(n * sizeof(T)); if (!pd2) { return false; }
        t_size n0 = t_size(n < _n ? n : _n); t_size i0 = 0;
        if (imode == 1)
        {
          try { for (; i0 < n0; ++i0) { _carr_asgx_t<T>::try_cc(pd2 + i0, _data[i0]); } } catch (...) {}
            if (i0 < n0) { if (dmode == 1) { _destroy(pd2, 0, i0); } _free(pd2); return false; }
          if (n > _n)
          {
            if (px) { try { for (; i0 < t_size(n); ++i0) { _carr_asgx_t<T>::try_cc(pd2 + i0, *px); } } catch (...) {} }
              else { try { for (; i0 < t_size(n); ++i0) { new (pd2 + i0) T(); } } catch (...) {} }
            if (i0 < t_size(n)) { if (dmode == 1) { _destroy(pd2, 0, i0); } _free(pd2); return false; }
          }
        }
        else if (imode == 4)
          { if (n0) { memcpy(pd2, _data, n0 * sizeof(T)); } if (px && n > _n) { for (i0 = n0; i0 < t_size(n); ++i0) { memcpy(pd2 + i0, px, sizeof(T)); } } }
      }
      if (_n > 0) { if (dmode == 1) { _destroy(_data, 0, _n); } _free(_data); }
      _data = pd2; _n = n;
      return true;
    }

      // Element-by-element equality check.
    bool is_eq(const _carray_base_t<T>& x) const { if (this->_n != x._n) { return false; } for (_s_ll i = 0; i < this->_n; ++i) { if (!(this->_data[i] == x[i])) { return false; } } return true; }

  protected:
    _s_ll _n; T* _data; void* _psf1; void** _pff; static void* _spff[3];
    ~_carray_base_t() throw() { if (_data) { _free(_data); _data = 0; } _n = 0; }

    typedef void* (*F_alloc)(_s_ll nbytes); typedef void (*F_free)(void* p);
    inline void* _alloc(_s_ll nbytes) throw() { if (_psf1 && _psf1 == _s_psf1()) { return _sf_alloc(nbytes); } return ((F_alloc)_pff[1])(nbytes); }
    inline void _free(void* p) throw() { if (!p) { return; } if (_psf1 && _psf1 == _s_psf1()) { _sf_free(p); return; } ((F_free)_pff[2])(p); }

    inline void _destroy(T* pd, t_size i0, t_size n) throw() { while (i0 < t_size(n)) { try { for (; i0 < t_size(n); ++i0) { (pd + i0)->~T(); } } catch (...) { ++i0; } } }

    #ifdef _bmdxpl_Wnds
      inline static void* _s_psf1() throw() { return (void*)&HeapAlloc; }
      static void* _sf_alloc(_s_ll nb) throw() { if (nb < 0) { return 0; } SIZE_T n2 = SIZE_T(size_t(nb)); if (_s_ll(n2) != nb) { return 0; } return HeapAlloc(GetProcessHeap(), 0, n2); }
      static void _sf_free(void* p) throw() { HeapFree(GetProcessHeap(), 0, p); }
    #else
      #ifdef malloc
        inline static void* _s_psf1() throw() { return 0; }
      #else
        inline static void* _s_psf1() throw() { return (void*)&malloc; }
      #endif
      static void* _sf_alloc(_s_ll nb) throw() { if (nb < 0) { return 0; } size_t n2 = size_t(nb); if (_s_ll(n2) != nb) { return 0; } try { return malloc(n2); } catch (...) { return 0; } }
      static void _sf_free(void* p) throw() { try { free(p); } catch (...) {} }
    #endif
  private:
    _carray_base_t(const _carray_base_t&); _carray_base_t& operator=(const _carray_base_t&);
  };
  template<class T> void* _carray_base_t<T>::_spff[3] = { (void*)(2 + (const char*)0), (void*)&_sf_alloc, (void*)&_sf_free };

  template<class T, bool no_exc_asg>
  struct cpparray_t;

    // C-style array wrapper. For T allowing bytewise copying and not needing destructor calls.
    // Behavior:
    //    1. Any copying copies just bytes. If storage allocation fails, no exceptions (dflt., no_exc_asg == true):
    //      a) assignment returns false, no changes to target; b) copy ctor. creates an empty object.
    //    2. resize(n) preserves elements. If size grows, new elements are initialized as T().
    //      (For just storage reallocation, call realloc(n, 0, 0, 0).)
    //    3. carray_t, created in binary module A, may be passed as bytes or as container element,
    //        manipulated and destroyed locally in binary module B.
    //      a) WinAPI: any A, B / POSIX: A, B using standard malloc() family:
    //        carray_t destruction in B is safe even after A being unloaded.
    //      b) POSIX: A or B using custom malloc() family: carray_t destruction in B is safe only before unloading A.
  template<class T, bool no_exc_asg = true>
  struct carray_t : _carray_base_t<T>
  {
    typedef carray_t t_a; typedef T t_value; typedef typename _carray_base_t<T>::t_size t_size;

      // If this != &src: clears this, moves src to this, sets src to be empty.
    inline void move(carray_t& src) throw() { if (this == &src) { return; } this->~carray_t(); memcpy(this, &src, sizeof(carray_t)); src._data = 0; src._n = 0; }
    inline void swap(carray_t& src) throw() { if (this == &src) { return; } _s_ll x[sizeof(t_a) / 8 + 1]; memcpy(x, &src, sizeof(t_a)); memcpy(&src, this, sizeof(t_a)); memcpy(this, x, sizeof(t_a)); }
    inline bool resize(_s_ll n) throw() { if (n == this->_n) { return true; } return this->realloc(n, 0, 4, 0); }
    inline bool clear() throw() { return this->realloc(0, 0, 4, 0); }
    inline bool operator=(const carray_t& x)
      { if (this == &x) { return true; } carray_t x2; if (!x2.realloc(x.n(), 0, 0, 0)) { _carr_asgx_t<t_a>::check_exc_alloc(); return false; } if (x.n()) { memcpy(x2._data, x._data, x.n() * sizeof(T)); } this->move(x2); return true; }
    carray_t() throw() {}
    carray_t(const carray_t& x) : _carray_base_t<T>() { if (!this->realloc(x.n(), 0, 0, 0)) { _carr_asgx_t<t_a>::check_exc_alloc(); return; } if (x.n()) { memcpy(this->_data, x._data, x.n() * sizeof(T)); } }
    ~carray_t() throw() {}
    template<bool _ne> inline bool operator==(const carray_t<T, _ne>& x) const { return this->is_eq(x); }
    template<bool _ne> inline bool operator!=(const carray_t<T, _ne>& x) const { return !this->is_eq(x); }
    template<bool _ne> inline bool operator==(const cpparray_t<T, _ne>& x) const { return this->is_eq(x); }
    template<bool _ne> inline bool operator!=(const cpparray_t<T, _ne>& x) const { return !this->is_eq(x); }
  };

    // C++-style array wrapper. For T with non-trivial construction, copying and destructor calls.
    // Behavior:
    //    1. Any copying is via T(const T&). If operation fails, no exceptions (dflt., no_exc_asg == true):
    //      a) assignment returns false, no changes to target; b) copy ctor. creates an empty object.
    //    2. resize(n) preserves elements. If size grows, new elements are initialized as T().
    //    3. cpparray_t, created in binary module A, may be passed as bytes or as container element,
    //        manipulated and destroyed locally in binary module B if all the following conditions are true:
    //      1) T(), T(const T&), ~T() in B behave exactly as in A (i.e. has module-independent data, like flstr_t)
    //        or are aware of the current binary module (like carray_t, cpparray_t, vec2_t, unity, also vecm, hashx).
    //        NOTE STL and other objects with dynamic data, like string, vector etc. can safely be elements of cpparray_t
    //        only within one binary module.
    //      2) cpparray_t destruction in B is safe only before unloading A.
  template<class T, bool no_exc_asg = true>
  struct cpparray_t : _carray_base_t<T>
  {
    typedef cpparray_t t_a; typedef T t_value; typedef typename _carray_base_t<T>::t_size t_size;

      // If this != &src: clears this, moves src to this, sets src to be empty.
    inline void move(cpparray_t& src) throw() { if (this == &src) { return; } this->~cpparray_t(); memcpy(this, &src, sizeof(t_a)); src._data = 0; src._n = 0; }
    inline void swap(cpparray_t& src) throw() { if (this == &src) { return; } _s_ll x[sizeof(t_a) / 8 + 1]; memcpy(x, &src, sizeof(t_a)); memcpy(&src, this, sizeof(t_a)); memcpy(this, x, sizeof(t_a)); }
    inline bool resize(_s_ll n) throw() { if (n == this->_n) { return true; } return this->realloc(n, 1, 1, 0); }
    inline bool clear() throw() { return this->realloc(0, 1, 1, 0); }
    inline bool operator=(const cpparray_t& x)
    {
      if (this == &x) { return true; } cpparray_t x2; if (!x2.realloc(x.n(), 0, 0, 0)) { _carr_asgx_t<t_a>::check_exc_alloc(); return false; }
      T* pd2 = x2.pd(); t_size i0 = 0; t_size n0 = x2.n();
      try { for (; i0 < n0; ++i0) { new (pd2 + i0) T(x[i0]); } } catch (...) {}
      if (i0 < n0) { this->_destroy(pd2, 0, i0); x2.realloc(0, 0, 0, 0); _carr_asgx_t<t_a>::check_exc_alloc(); return false; }
      this->move(x2);
      return true;
    }
    cpparray_t() throw() {}
    cpparray_t(const cpparray_t& x) : _carray_base_t<T>() { *this = x; }
    ~cpparray_t() throw() { this->realloc(0, 1, 0, 0); }
    template<bool _ne> inline bool operator==(const carray_t<T, _ne>& x) const { return this->is_eq(x); }
    template<bool _ne> inline bool operator!=(const carray_t<T, _ne>& x) const { return !this->is_eq(x); }
    template<bool _ne> inline bool operator==(const cpparray_t<T, _ne>& x) const { return this->is_eq(x); }
    template<bool _ne> inline bool operator!=(const cpparray_t<T, _ne>& x) const { return !this->is_eq(x); }
  };

    // Reference to an array of elements in memory.
    //  The number of referenced elements is specified when the reference object is created.
  template<class T>
  struct arrayref_t
  {
    typedef T t_value; typedef size_t t_size; typedef arrayref_t t_a;

    inline _s_ll n() const throw()  { return _n; }

    inline const t_value* pd() const throw() { return _data; }
    inline t_value* pd() throw() { return _data; }

    inline const t_value& operator[] (t_size i) const throw() { return _data[i]; }
    inline t_value& operator[] (t_size i) throw() { return _data[i]; }

    arrayref_t() throw() { unlink(); }
    ~arrayref_t() throw() { unlink(); }

    bool link(const T* px, _s_ll n) throw() { _n = n; if (_n < 0 || (_n > 0 && !px)) { _n = 0; _data = 0; return false; } _data = px; return true; }
    void unlink() throw() { _n = 0;_data = 0; }

    arrayref_t(const T* px, _s_ll n) throw() { link(px, n); }
    arrayref_t(const arrayref_t& x) throw() : _n(x._n), _data(x._data) {}
    template<bool _ne> arrayref_t(const carray_t<T, _ne>& x) throw() : _n(x.n()), _data(const_cast<char*>(x.pd())) {}
    template<bool _ne> arrayref_t(const cpparray_t<T, _ne>& x) throw() : _n(x.n()), _data(const_cast<char*>(x.pd())) {}
    arrayref_t(const std::basic_string<T>& x) throw() { *this = x; }

    arrayref_t& operator=(const arrayref_t& x) throw() { _data = x._data; _n = x._n; return *this; }
    template<bool _ne> arrayref_t& operator=(const carray_t<T, _ne>& x) throw() { _data = const_cast<char*>(x.pd()); _n = x.n(); return *this; }
    template<bool _ne> arrayref_t& operator=(const cpparray_t<T, _ne>& x) throw() { _data = const_cast<char*>(x.pd()); _n = x.n(); return *this; }
    arrayref_t& operator=(const std::basic_string<T>& x) throw() { _n = _s_ll(x.size()); if (_n <= 0) { _n = 0; _data = 0; return *this; } _data = &x[0]; return *this; }

      // Set all existing (or [i0..i2) cut to [0..n()) elements to x, using operator=.
      //  Returns: 1 - all set successfully, 0 - all assignments failed, -1 - part of assignments failed.
      //    (If operator= always succeeds, the function will also succeed and return 1.)
    inline int set(const t_value& x) throw()
    {
      t_size i0 = 0; t_size nf(0); while (i0 < t_size(_n)) { try { for (; i0 < t_size(_n); ++i0) { _carr_asgx_t<T>::try_asg(_data[i0], x); } } catch (...) { ++i0; ++nf; } }
      return nf ? (nf == t_size(_n) ? 0 : -1) : 1;
    }
    inline int set(const t_value& x, _s_ll i0, _s_ll i2) throw()
    {
      if (i0 < 0) { i0 = 0; } if (i2 > _n) { i2 = _n; }
      t_size _i0 = t_size(i0); const t_size _i2 = t_size(i2); if (!(_s_ll(_i0) == i0 && _s_ll(_i2) == i2)) { return 0; }
      if (_i0 >= _i2) { return 1; }
      const t_size nset = _i2 - _i0;
      t_size nf(0); while (_i0 < _i2) { try { for (; _i0 < _i2; ++_i0) { _carr_asgx_t<T>::try_asg(_data[_i0], x); } } catch (...) { ++_i0; ++nf; } }
      return nf ? (nf == nset ? 0 : -1) : 1;
    }

      // Element-by-element equality check.
    inline bool is_eq(const arrayref_t<T>& x) const { if (this->_n != x._n) { return false; } if (this->_data == x._data) { return true; } for (_s_ll i = 0; i < this->_n; ++i) { if (!(this->_data[i] == x[i])) { return false; } } return true; }

    inline void swap(arrayref_t& src) throw() { if (this == &src) { return; } _s_ll x[sizeof(t_a) / 8 + 1]; memcpy(x, &src, sizeof(t_a)); memcpy(&src, this, sizeof(t_a)); memcpy(this, x, sizeof(t_a)); }

    inline bool operator==(const arrayref_t<T>& x) const { return this->is_eq(x); }
    inline bool operator!=(const arrayref_t<T>& x) const { return !this->is_eq(x); }
  private:
    _s_ll _n; T* _data;
  };
}








#ifdef _bmdxpl_Wnds

  #include <io.h>
  #include <conio.h>

  namespace bmdx
  {
    #ifndef __bmdx__clock_ms
      #define __bmdx__clock_ms
      static double clock_ms()
      {
        static double _msclk_freq(1.); static bool _msclk_is_init(false);
        if (!_msclk_is_init) { LARGE_INTEGER f; if (QueryPerformanceFrequency(&f)) { double f2 = ((ULONG*)&f)[1]; f2 *= 4294967296.; f2 += ((ULONG*)&f)[0]; f2 /= 1e3; _msclk_freq = f2; _msclk_is_init = true; } }
        LARGE_INTEGER cnt;
        if (_msclk_is_init && QueryPerformanceCounter(&cnt)) { double x = ((ULONG*)&cnt)[1]; x *= 4294967296.; x += ((ULONG*)&cnt)[0]; return x / _msclk_freq; }
          else { return GetTickCount(); }
      }
    #endif

      // NOTE avg. max. sleep duration error depends on thread priority.
      //  On overloaded CPU, order of 160..40 ms for normal..time-critical priority.
      //  In normal conditions, order of 1 mcs.
    static void sleep_mcs(_s_ll t) throw()
    {
      if (t < 0) { return; }
      if (t == 0) { SleepEx(0, FALSE); return; }
      static _s_ll _tmin(1000); // max. detected lag of SleepEx function (real sleep time minus required)
      _s_ll tmin = _tmin;
      if (t - tmin < 1000) { double t0 = clock_ms(); do { SleepEx(0, FALSE); } while (clock_ms() - t0 < t / 1000.); return; }
      _s_ll t1 =  (t - tmin) / 1000;
      double t0 = clock_ms();
      SleepEx(DWORD(t1), 0);
      t0 = clock_ms() - t0;
      _s_ll t2 = _s_ll(t0 * 1000);
      _s_ll t3 = t2 - t1 * 1000;
      if (t3 > _tmin) { _tmin = t3; }
      t3 = t - t2;
      if (t3 > 0) { t0 = clock_ms(); do { SleepEx(0, FALSE); } while (clock_ms() - t0 < t3 / 1000.); }
    }



      // NOTE critsec_t is working correctly both in main program and static init./deinit.
    template<class T> struct _critsec_data0_t { LONG cnt; LONG cnt2; DWORD tid; };
    template<class T> struct critsec_t
    {
      struct csdata : _critsec_data0_t<T> { inline csdata() throw() { this->cnt = 0; this->cnt2 = 0; this->tid = 0; } };

        // check_period_mcs: thread sleep duration between lock tries.
        //  NOTE on negative check_period_mcs, constructor exits immediately w/o locking.
        // timeout_ms: max. total time of sleep, after which constructor exits with is_locked() == false.
        //    Negative value means "never", i.e. the constructor will wait until it is able to set the lock.
      critsec_t(_s_ll check_period_mcs, _s_long timeout_ms, csdata* pdata = 0) throw () : _p(pdata ? pdata : &_tdata), _bl(false)
      {
        if (check_period_mcs < 0) { return; }
        double t0(0.); bool b(false);
        while (InterlockedIncrement(&_p->cnt) != 1)
        {
          if (_p->tid == GetCurrentThreadId()) { ++_p->cnt2; _bl = true; return; }
          if (timeout_ms >= 0) { if (!b) { t0 = clock_ms(); b = true; } }
          InterlockedDecrement(&_p->cnt);
          if (timeout_ms >= 0) { double dt = clock_ms() - t0; if (dt < 0 || dt >= double(timeout_ms)) { return; } }
          sleep_mcs(check_period_mcs);
        }
        _p->tid = GetCurrentThreadId(); ++_p->cnt2; _bl = true;
      }
        // Return value: a) true, b) false - lock tries were timed out (ONLY on constructor timeout_ms >= 0).
      inline bool is_locked() const throw () { return _bl; }
      inline operator bool() const throw() { return _bl; }
      ~critsec_t() throw () { if (_bl) { if (--_p->cnt2 == 0) { _p->tid = 0; } InterlockedDecrement(&_p->cnt); _bl = false; } }
      private: static _critsec_data0_t<T> _tdata; _critsec_data0_t<T>* _p; bool _bl;
    };
    template<class T> _critsec_data0_t<T> critsec_t<T>::_tdata = { 0, 0, 0 };



    struct threadctl
    {
      struct ctx_base
      {
          // Thread proc. must check this flag and return when it's true.
        const volatile _s_long& b_stop() const { return __bs; }
        template<class T> T* pdata() const { return reinterpret_cast<T*>(__pdata); }

        ctx_base() : __in_thread(false), __in_ctl(false), __bs(0), __csdata(), __pdata(0) {} virtual ~ctx_base() {}

      protected:
        virtual void _thread_proc() = 0;

      private:
        friend struct threadctl;
        void _unset_in_thread() throw() { if (!__in_thread) { return; } bool b = false; if (true) { critsec_t<ctx_base> __lock(10, -1, &__csdata); if (sizeof(__lock)) {} __in_thread = false; if (!__in_ctl) { b = true; } } if (b) { try { delete this; } catch (...) {} } }
        void _unset_in_ctl() throw() { if (!__in_ctl) { return; } bool b = false; if (true) { critsec_t<ctx_base> __lock(10, -1, &__csdata); if (sizeof(__lock)) {} __in_ctl = false; if (!__in_thread) { b = true; } } if (b) { try { delete this; } catch (...) {} } }
        volatile bool __in_thread, __in_ctl; volatile _s_long __bs; critsec_t<ctx_base>::csdata __csdata; void* __pdata;
      };

      template <class Data> struct _ctx_data { Data _x; _ctx_data(const Data& x) : _x(x) {} };
      template <class Ctx, class Data> struct _ctx_wrapper : _ctx_data<Data>, Ctx  { typedef _ctx_data<Data> t_d; _ctx_wrapper(const Data& x) : t_d(x) {} };

      struct tid
      {
        typedef DWORD t_native_tid; enum { n = 2 * sizeof(t_native_tid) + 1 };
        bool is_null() const { return x[0] == 0; }
        void clear() { memset(this, 0, sizeof(tid)); }
        tid() { clear(); }
        tid(const t_native_tid& x_) { *this = x_; }
        tid& operator=(t_native_tid x_) { _orig = x_; x[n - 1] = 0; for (int i = n - 2; i >= 0; --i) { char c = x_ & 0xf; c += c < 10 ? '0' : 'A' - char(10); x[i] = c; x_ >>= 4; } return *this; }
        bool operator==(const tid& x_) const { return memcmp(x, x_.x, n) == 0; }
        bool operator!=(const tid& x_) const { return !this->operator==(x_); }
          // NOTE Thread IDs and their string representations may become not unique when threads are recreated.
        operator const char*() const { return x; }
        const char* c_str() const { return x; }

        const t_native_tid& native() const { return _orig; }

      private: char x[n]; t_native_tid _orig;
      };



      threadctl() : _pctx(0), _th(0), _tid() {}
      ~threadctl() { stop(-1); }

        // !is_null() when thread is running.
      const tid& id() const throw() { return _tid; }

        // 2 - context is referenced, thread is running (volatile state!),
        // 1 - context is referenced, thread has already exited,
        // 0 - no context, prev. thread state is unknown.
      int state() const { if (!_pctx) { return 0; } DWORD c(0); GetExitCodeThread(_th, &c); return c == STILL_ACTIVE ? 2 : 1; }

      ctx_base* pctx() const throw() { return _pctx; }

        // Succeeds only in state() == 0, if the new thread was created and started.
        //  state() becomes > 0.
        // NOTE *p must be created dynamically and initialized prior to Test().
        //    threadctl takes ownership on *p.
      bool Test(ctx_base* p) throw()
      {
        if (!p || _pctx || p->__in_ctl || p->__in_thread) { return false; }
        DWORD tid1;
        p->__in_ctl = true; p->__in_thread = true;
        try { _th = CreateThread(0, 0, &_thproc, p, 0, &tid1); } catch (...) {}
        if (!_th) { p->__in_ctl = false; p->__in_thread = false; return false; }
        _pctx = p; _tid = tid1;
        return true;
      }

        // Creates a thread context object of the specified type,
        //    starts thread with it's _thread_proc().
        //    Passes a copy of d as additional data member,
        //    accessible via ctx_base::pdata<Data>().
        //  Ctx must:
        //    1. Inherit from ctx_base.
        //    2. Override ctx_base::_thread_proc().
        //    3. Have dflt Ctx().
        //    4. Know Data type to correctly use ctx_base::pdata(),
        //      or ignore pdata at all.
        //  NOTE Additional data member is destroyed later than Ctx.
        //  Returns:
        //    true on successful thread Test.
        //    false on any failure. All temp. objects are deleted.
      template<class Ctx, class Data>
      bool start_auto(const Data& d = Data()) throw()
      {
        _ctx_wrapper<Ctx, Data>* p(0);
        try { p = new _ctx_wrapper<Ctx, Data>(d); } catch (...) {} if (!p) { return false; }
        p->Ctx::__pdata = &p->_ctx_data<Data>::_x;
        bool b = Test(p); if (!b) { try { delete p; } catch (...) {} }
        return b;
      }

        // Sets thread priority in realtion to process priority class.
        // Allowed values:
        //  4 - normal priority (same as assigned by default),
        //  5 - above normal,
        //  6 - high,
        //  7 - time-critical,
        //  3 - below normal,
        //  2 - low,
        //  1 - background.
      bool set_priority(_s_long p)
      {
        if (!_pctx) { return false; }
        switch (p)
        {
          case 4: { bool b = bool(SetThreadPriority(_th, THREAD_PRIORITY_NORMAL)); SetThreadPriorityBoost(_th, FALSE); return b; }
          case 5: { bool b = bool(SetThreadPriority(_th, THREAD_PRIORITY_ABOVE_NORMAL)); SetThreadPriorityBoost(_th, FALSE); return b; }
          case 6: { bool b = bool(SetThreadPriority(_th, THREAD_PRIORITY_HIGHEST)); SetThreadPriorityBoost(_th, FALSE); return b; }
          case 7: { bool b = bool(SetThreadPriority(_th, THREAD_PRIORITY_TIME_CRITICAL)); SetThreadPriorityBoost(_th, FALSE); return b; }
          case 3: { bool b = bool(SetThreadPriority(_th, THREAD_PRIORITY_BELOW_NORMAL)); SetThreadPriorityBoost(_th, FALSE); return b; }
          case 2: { bool b = bool(SetThreadPriority(_th, THREAD_PRIORITY_LOWEST)); SetThreadPriorityBoost(_th, FALSE); return b; }
          case 1: { bool b = bool(SetThreadPriority(_th, THREAD_PRIORITY_IDLE)); SetThreadPriorityBoost(_th, FALSE); return b; }
          default: return false;
        }
      }

        // Sets b_stop() of the target thread.
        // Returns:
        //    true - successful, the client may wait until state() becomes != 2.
        //    false - could not set the flag, threadctl has no context. state() will return 0.
      bool signal_stop() { if (!_pctx) { return false; } if (!_pctx->__bs) { _pctx->__bs = 1; } return true; }

        // Sets pctx()->b_stop() flag, and possibly waits for thread exiting.
        //    If thread had exited, detaches from its context (which is automatically destroyed).
        // timeout_ms:
        //    >0 - sleeps approx.  (timeout_ms / 10) times * 10 ms,
        //      checking thread state and returns on timeout or thread having exited.
        //    0 - yields once (sleep(0)), then checks thread state and returns.
        //    -1 immediately detaches from thread context.
        //      Thread context will be destroyed later by thread itself when it exits.
        //  Returns:
        //    2 - thread has exited normally.
        //    1 - thread already exited.
        //    0 - thread was not running (no context).
        //    -1 - detached.
        //    -2 - still running (timeout).
      int stop(_s_long timeout_ms)
      {
        if (!_pctx) { return 0; }
        if (!_pctx->__bs) { _pctx->__bs = 1; }
        if (!_pctx->__in_thread) { _pctx->_unset_in_ctl(); _pctx = 0; CloseHandle(_th); _tid.clear(); return 1; }
        _s_long t = timeout_ms;
        if (t == 0) { sleep_mcs(0); }
        while (t > 0) { sleep_mcs(t > 10 ? 10000 : t * 1000); t -= 10; }
        if (!_pctx->__in_thread) { _pctx->_unset_in_ctl(); _pctx = 0; CloseHandle(_th); _tid.clear(); return 2; }
        if (timeout_ms < 0) { _pctx->_unset_in_ctl(); _pctx = 0; CloseHandle(_th); _tid.clear(); return -1; }
        return -2;
      }

        // Immediately terminates the thread.
        //  This is unsafe + platform-dependent. Use it only to terminate pure computational tasks.
        //  Returns:
        //    2 - terminated, context destroyed.
        //    1 - thread already exited.
        //    0 - thread was not running (no context).
      int terminate()
      {
        if (!_pctx) { return 0; }
        if (!_pctx->__bs) { _pctx->__bs = 1; }
        if (!_pctx->__in_thread) { _pctx->_unset_in_ctl(); _pctx = 0; CloseHandle(_th); _tid.clear(); return 1; }
        TerminateThread(_th, 1); CloseHandle(_th);
        _pctx->_unset_in_thread(); _pctx->_unset_in_ctl();
        _pctx = 0; _tid.clear();
        return 2;
      }

      private: ctx_base* _pctx; HANDLE _th; tid _tid;
      static DWORD WINAPI _thproc(LPVOID _p) { try { ((ctx_base*)_p)->_thread_proc(); } catch (...) {} ((ctx_base*)_p)->_unset_in_thread(); return 1; }
        // IMPORTANT:
        //  threadctl may not be copied (1 threadctl per 1 thread).
        //  Use cref_t to handle threadctl instances when multiple references needed.
        //  Also, threadctl functions are not thread-safe (may be called only by one thread at a time).
      threadctl(const threadctl&); threadctl& operator=(const threadctl&);
    };

    struct processctl
    {
      bool has_ref() const throw () { return bool(_hp); }
      operator bool() const throw () { return bool(_hp); }

        // On has_ref() == true, pid() has meaningful value.
      typedef DWORD t_pid;
      t_pid pid() const throw() { return _pid; }

        // OS-dependent handles (not cross-platform).
      HANDLE _phandle() const throw() { return _hp; }
      HANDLE _thandle() const throw() { return _ht; }

      processctl() throw() : _hp(0), _ht(0), _pid(0), _exited(false) {}
      ~processctl() throw() { clear(); }

      processctl(const processctl& x) throw() : _hp(0), _ht(0), _pid(0), _exited(false)
      {
        if (_hp) { DuplicateHandle(GetCurrentProcess(), x._hp, GetCurrentProcess(), &_hp, 0, TRUE, DUPLICATE_SAME_ACCESS); }
        if (_ht) { DuplicateHandle(GetCurrentProcess(), x._ht, GetCurrentProcess(), &_ht, 0, TRUE, DUPLICATE_SAME_ACCESS); }
        _pid = x._pid; _exited = x._exited;
      }
      processctl& operator=(const processctl& x) { if (this == &x) { return *this; } clear(); new (this) processctl(x); return *this; }

      void clear() throw() { _pid = 0; _exited = false; if (_hp) { CloseHandle(_hp); _hp = 0; } if (_ht) { CloseHandle(_ht); _ht = 0; } }

        // Creates new process and returns immediately.
        // fnp_process with b_shell == false: must be full path and executable name (for CreateProcessA()).
        //   fnp_process with b_shell == true: must be executable path and name in any form (for system()).
        // args: all program arguments, separated with null character ('\0').
        //   Each argument will be pre-processed with arg1(),
        //   so the client must not do any special processing (escaping etc.).
        // b_shell true: use CreateProcessA() to launch process.
        //   On success, launch() returns true. Also, has_ref() and pid() reflect the result.
        //   On failure, launch() returns false. Also, has_ref() == false, and pid() value in not valid.
        // b_shell false (dflt.): use system("Test ...") to launch process (asynchronously).
        //   In this case, has_ref() == false and pid() value is not valid.
        //   On success, launch() returns true.
        //   On failure, launch() returns false.
      bool launch(const std::string& fnp_process, const std::string& args, bool b_shell = false)
      {
        clear();
        if (fnp_process.empty()) { return false; }

        std::string a2; size_t pos = 0, pos2 = 0, end = std::string::npos;
        while (pos < args.length())
        {
          pos2 = args.find('\0', pos);
          if (pos2 == end) { pos2 = args.length(); }
          if (pos2 - pos > 1) { if (a2.length()) { a2 += ' '; } a2 += arg1(args.substr(pos, pos2 - pos), b_shell); }
          pos = pos2 + 1;
        }
        if (b_shell)
        {
          std::string s = "Test \"\" ";
          if (fnp_process[0] != '\"') { s += '\"'; }
          s += fnp_process;
          if (fnp_process[fnp_process.length() - 1] != '\"') { s += '\"'; }
          if (a2.length()) { s += ' '; s += a2; }
          int res = system(s.c_str());
          return res == 0;
        }
        else
        {
          STARTUPINFOA si; PROCESS_INFORMATION pi; ZeroMemory(&si, sizeof(si)); ZeroMemory(&pi, sizeof(pi));
          BOOL res = CreateProcessA(fnp_process.c_str(), a2.empty() ? 0 : &a2[0], 0, 0, FALSE, CREATE_NEW_CONSOLE, 0, 0, &si, &pi);
          if (!res) { return false; }
          _hp = pi.hProcess;
          _ht = pi.hThread;
          _pid = pi.dwProcessId;
          _exited = false;
        }
      }

        // Waits for the launched process completion, return its exit code or termination code.
        //  Returned value:
        //    2 - the process has exited during waiting. *ret_pec = the process exit code.
        //    (Platform-dependent: 1 - the process has been terminated during waiting. *ret_pec = the initForCallback termination code.)
        //    0 - the process had been completed before wait_exit() was called. *ret_pec = 0.
        //    -1 - the process is not started, nothing to wait. *ret_pec not changed.
        //    -2 - failure, process may exist, but its state is undefined. *ret_pec not changed.
      int wait_exit(long* ret_pec = 0) throw()
      {
        if (!_hp) { return -1; }
        if (_exited) { if (ret_pec) { *ret_pec = 0; } return 0; }

        WaitForSingleObject(_hp, INFINITE);
        DWORD ec = 0;
        if (!GetExitCodeProcess(_hp, &ec)) { return -2; }
        if (ret_pec) { *ret_pec = long(ec); }
        _exited = true;
        return 2;
      }

      bool is_running() const throw()
      {
        if (!has_ref()) { return false; }
        DWORD res = WaitForSingleObject(_hp, 0);
        return res == WAIT_TIMEOUT;
      }

        // Represent s as single argument for passing into program command line.
        //  (Appropriate quoting and escaping is made as necessary).
        //  b_shell false (dflt.) preserves CR and LF characters - intended for passing into CreateProcess().
        //  b_shell true replaces CR and LF characters with ' ', intended for passing single line into system().
        //  In all cases, horz./vert. tab. and formfeed characters are kept.
        //  Other ASCII control characters (0..31) are replaced with '?'.
      static std::string arg1(const std::string& s, bool b_shell = false)
      {
        bool b = false;
        for (size_t i = 0; i < s.length(); ++i)
          { if ( ! ((s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z') || (s[i] >= '0' && s[i] <= '9') || s[i] == '-' || s[i] == '_' || s[i] == '.') )
            { b = true; break; } }
        if (!b) { return s; }

    std::string z = s; b = b_shell;
        for (size_t i = 0; i < z.length(); ++i)
          { if (z[i] >= 0 && z[i] < 32) { switch(z[i]) { case '\r': case '\n': if (b) { z[i] = ' '; } break; case '\t': case '\v': case '\f': break; default: z[i] = '?'; break; } } }

        std::string z2 = "\"";
          size_t pos = 0, pos2 = 0, end = std::string::npos;
          while (pos < z.length())
          {
            pos2 = z.find_first_of("\\\"", pos);
              if (pos2 == end) { z2 += z.substr(pos, z.length() - pos); break; }
            z2 += z.substr(pos, pos2 - pos); pos = pos2;
              if (z[pos2] == '\"') { z2 += "\\\""; pos = pos2 + 1; continue; }
            pos2 = z.find_first_not_of("\\", pos);
              if (pos2 == end) { z2 += std::string(2 * (z.length() - pos) + 1, '\\'); break; }
                else if (z[pos2] == '\"') { z2 += std::string(2 * (pos2 - pos) + 1, '\\'); z2 += '\"'; pos = pos2 + 1; continue; }
            z2 += z.substr(pos, pos2 - pos); pos = pos2; // slashes are not followed by ", so no escaping
          }
        z2 += '\"';
        return z2;
      }

    private:
      HANDLE _hp, _ht; DWORD _pid; bool _exited;
    };


    struct console_io { unsigned int ugetch(unsigned int no_char = 0) throw() { return _kbhit() ? _getch() : no_char; } };
  }

#endif



#ifdef _bmdxpl_Psx

  #include <unistd.h>
  #include <sys/time.h>
  #include <fcntl.h>
  #include <termios.h>
  #include <pthread.h>
  #include <signal.h>
  #include <sys/wait.h>

  #ifndef __ANDROID__
    #include <spawn.h>
  #endif

  #if __APPLE__ && __MACH__
    #include <mach/mach_time.h>
  #endif
  extern char **environ;

  namespace bmdx
  {
    #ifndef __bmdx__clock_ms
      #define __bmdx__clock_ms

      static double clock_ms()
      {
        #if defined(__SUNPRO_CC)
          return gethrtime() * 1.e-6;
        #elif __APPLE__ && __MACH__
          static mach_timebase_info_data_t rate = { 0, 1 }; if (!rate.numer) { mach_timebase_info(&rate); } // mach_timebase_info always succeeds
          return mach_absolute_time() * 1.e-6 * rate.numer / rate.denom;
        #else
          #ifdef CLOCK_MONOTONIC_RAW
            static const clockid_t t_timer = CLOCK_MONOTONIC_RAW;
          #elif defined(CLOCK_MONOTONIC_FAST)
            static const clockid_t t_timer = CLOCK_MONOTONIC_FAST;
          #else
            static const clockid_t t_timer = CLOCK_MONOTONIC;
          #endif
          static bool bfall(false); if (bfall) { return std::clock() * (1000. / CLOCKS_PER_SEC); }
          timespec t; if (clock_gettime(t_timer, &t) !=0) { bfall = true; return clock_ms(); }
          return double(t.tv_sec) * 1000. + double(t.tv_nsec) * 1.e-6;
        #endif
      }
    #endif

    static void sleep_mcs(_s_ll t) throw () { if (t < 0) { return; } timespec t1; t1.tv_sec = t / 1000000; t1.tv_nsec = 1000 * (t % 1000000); nanosleep(&t1, 0); }



      // NOTE critsec_t is working correctly both in main program and static init./deinit.
    template<class T> struct _critsec_data0_t { int _kind; pthread_mutex_t _m; bool _b_lk1; _s_ll _tid[(sizeof(pthread_t) + 7) / 8]; };
    template<class T> struct critsec_t
    {
      struct csdata : _critsec_data0_t<T>
      {
        csdata() throw ()
        {
          this->_kind = 1; pthread_mutex_t _m_temp = PTHREAD_MUTEX_INITIALIZER; this->_m = _m_temp; this->_b_lk1 = false;
          bool b(true);
          pthread_mutexattr_t a;
            if (b && pthread_mutexattr_init(&a) != 0) { b = false; }
            if (b && pthread_mutexattr_settype(&a, PTHREAD_MUTEX_RECURSIVE) != 0) { pthread_mutexattr_destroy(&a); b = false; }
            if (b && pthread_mutex_init(&this->_m, &a) != 0) { pthread_mutexattr_destroy(&a); b = false; }
          if (b) { this->_kind = 2; } else { pthread_mutex_t _m_temp = PTHREAD_MUTEX_INITIALIZER; this->_m = _m_temp; }
        }
        ~csdata() throw() { if (this->_kind == 2) { pthread_mutex_destroy(&this->_m); } }
      };
        // check_period_mcs: thread sleep duration between lock tries.
        //  NOTE on negative check_period_mcs, constructor exits immediately w/o locking.
        // timeout_ms: max. total time of sleep, after which constructor exits with is_locked() == false.
        //    Negative value means "never", i.e. the constructor will wait until it is able to set the lock.
      critsec_t(_s_ll check_period_mcs, _s_long timeout_ms, csdata* pdata = 0) throw () : _p(pdata ? pdata : &_trecur), _bignore(false), _bl(false), _brec(false)
      {
        if (check_period_mcs < 0) { _bignore = true; return; }
        if (_p == &_trecur) { pthread_mutex_lock(&_tglobal._m); if (++_cnt == 1) { new (static_cast<csdata*>(&_trecur)) csdata(); } pthread_mutex_unlock(&_tglobal._m); }
        if (_p->_kind == 1)  { pthread_mutex_lock(&_tglobal._m); if (_p->_b_lk1 && pthread_equal((*(pthread_t*)&_p->_tid), pthread_self()) == 0) { _bl = true; _brec = true; } pthread_mutex_unlock(&_tglobal._m); if (_brec) { return; } }
        double t0(0.); bool b(false);
        while (pthread_mutex_trylock(&_p->_m) != 0)
        {
          if (timeout_ms >= 0) { if (!b) { t0 = clock_ms(); b = true; } double dt = clock_ms() - t0; if (dt < 0 || dt >= double(timeout_ms)) { return; } }
          sleep_mcs(check_period_mcs);
        }
        _bl = true; if (_p->_kind == 1) { new (&_p->_tid) pthread_t(pthread_self()); _p->_b_lk1 = true; }
      }
      ~critsec_t() throw ()
      {
        if (_bignore) { return; }
        if (_p->_kind == 1) { if (_brec) { return; } pthread_mutex_lock(&_tglobal._m); ((pthread_t*)&_p->_tid)->~pthread_t(); _p->_b_lk1 = false; pthread_mutex_unlock(&_tglobal._m); }
        if (_bl) { pthread_mutex_unlock(&_p->_m); _bl = false; }
        if (_p == &_trecur) { pthread_mutex_lock(&_tglobal._m); if (--_cnt == 0) { static_cast<csdata*>(&_trecur)->~csdata(); } pthread_mutex_unlock(&_tglobal._m); }
      }
        // Return value: a) true, b) false - lock tries were timed out (ONLY on constructor timeout_ms >= 0).
      inline bool is_locked() const throw () { return _bl; }
      inline operator bool() const throw() { return _bl; }
      private: static _critsec_data0_t<T> _trecur; static _critsec_data0_t<T> _tglobal; static _s_long _cnt; static critsec_t _cs0; _critsec_data0_t<T>* _p; bool _bignore; bool _bl; bool _brec;
    };
    template<class T> _critsec_data0_t<T> critsec_t<T>::_trecur = { 1, PTHREAD_MUTEX_INITIALIZER, false, {} };
    template<class T> _critsec_data0_t<T> critsec_t<T>::_tglobal = { 1, PTHREAD_MUTEX_INITIALIZER, false, {} };
    template<class T> _s_long critsec_t<T>::_cnt(0);
    template<class T> critsec_t<T> critsec_t<T>::_cs0;



    struct threadctl
    {
      struct ctx_base
      {
          // Thread proc. must check this flag and return when it's true.
        const volatile _s_long& b_stop() const { return __bs; }
        template<class T> T* pdata() const { return reinterpret_cast<T*>(__pdata); }

        ctx_base() : __in_thread(false), __in_ctl(false), __bs(0), __csdata(), __pdata(0) {} virtual ~ctx_base() {}

      protected:
        virtual void _thread_proc() = 0;

      private:
        friend struct threadctl;
        void _unset_in_thread() throw() { if (!__in_thread) { return; } bool b = false; if (true) { critsec_t<ctx_base> __lock(10, -1, &__csdata); if (sizeof(__lock)) {} __in_thread = false; if (!__in_ctl) { b = true; } } if (b) { try { delete this; } catch (...) {} } }
        void _unset_in_ctl() throw() { if (!__in_ctl) { return; } bool b = false; if (true) { critsec_t<ctx_base> __lock(10, -1, &__csdata); if (sizeof(__lock)) {} __in_ctl = false; if (!__in_thread) { b = true; } } if (b) { try { delete this; } catch (...) {} } }
        volatile bool __in_thread, __in_ctl; volatile _s_long __bs; critsec_t<ctx_base>::csdata __csdata; void* __pdata;
      };

      template <class Data> struct _ctx_data { Data _x; _ctx_data(const Data& x) : _x(x) {} };
      template <class Ctx, class Data> struct _ctx_wrapper : _ctx_data<Data>, Ctx  { typedef _ctx_data<Data> t_d; _ctx_wrapper(const Data& x) : t_d(x) {} };

      struct tid
      {
        typedef pthread_t t_native_tid; enum { n = 2 * sizeof(t_native_tid) + 1, nch = n - 1 };

        bool is_null() const { return x[0] == 0; }
        void clear() { memset(this, 0, sizeof(tid)); }
        tid() { clear(); }
        tid(const t_native_tid& x_) { *this = x_; }
        tid& operator=(const t_native_tid& x_) { _orig = x_; x[n - 1] = 0; const char* p = (const char*)&_orig; for (int i = 0; i < nch; ++i) { char c = p[i >> 1]; if (i & 1) { c >>= 4; } c &= 0xf; c += c < 10 ? '0' : 'A' - char(10); x[nch - 1 - i] = c; } return *this; }
        bool operator==(const tid& x_) const { if (is_null() != x_.is_null()) { return false; } if (is_null()) { return true; } return pthread_equal(_orig, x_._orig) == 0; }
        bool operator!=(const tid& x_) const { return !this->operator==(x_); }

          // NOTE Thread IDs and their string representations may become not unique when threads are recreated.
        operator const char*() const { return x; }
        const char* c_str() const { return x; }

        const t_native_tid& native() const { return _orig; }

      private: char x[n]; t_native_tid _orig;
      };



      threadctl() : _pctx(0), _tid() {}
      ~threadctl() { stop(-1); }

        // !is_null() when thread is running.
      const tid& id() const throw() { return _tid; }

        // 2 - context is referenced, thread is running (volatile state!),
        // 1 - context is referenced, thread has already exited,
        // 0 - no context, prev. thread state is unknown.
      int state() const { if (!_pctx) { return 0; } return _pctx->__in_thread ? 2 : 1; }

        // If a thread is run successfully, pctx() remains valid until
        //    threadctl destruction or explicit calling stop(-1) or terminate().
      ctx_base* pctx() const throw() { return _pctx; }

        // Succeeds only in state() == 0, if the new thread was created and started.
        //  state() becomes > 0.
        // NOTE *p must be created dynamically and initialized prior to Test().
        //    In case of successful thread Test, threadctl takes ownership on *p.
      bool start(ctx_base* p) throw()
      {
        if (!p || _pctx || p->__in_ctl || p->__in_thread) { return false; }
        pthread_attr_t a; pthread_attr_init(&a); pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED);
        memset(&_th_native, 0, sizeof(_th_native));
        int res(0);
        p->__in_ctl = true; p->__in_thread = true;
        try { res = pthread_create(&_th_native, &a, &_thproc, p); } catch (...) {}
        if (res != 0) { p->__in_ctl = false; p->__in_thread = false; return false; }
        _pctx = p; _tid = _th_native;
        return true;
      }

        // Creates a thread context object of the specified type,
        //    starts thread with it's _thread_proc().
        //    Passes a copy of d as additional data member,
        //    accessible via ctx_base::pdata<Data>().
        //  Ctx must:
        //    1. Inherit from ctx_base.
        //    2. Override ctx_base::_thread_proc().
        //    3. Have dflt Ctx().
        //    4. Know Data type to correctly use ctx_base::pdata(),
        //      or ignore pdata at all.
        //  NOTE Additional data member is destroyed later than Ctx.
        //  Returns:
        //    true on successful thread Test.
        //    false on any failure. All temp. objects are deleted.
      template<class Ctx, class Data>
      bool start_auto(const Data& d = Data()) throw()
      {
        _ctx_wrapper<Ctx, Data>* p(0);
        try { p = new _ctx_wrapper<Ctx, Data>(d); } catch (...) {} if (!p) { return false; }
        p->Ctx::__pdata = &p->_ctx_data<Data>::_x;
        bool b = start(p); if (!b) { try { delete p; } catch (...) {} }
        return b;
      }

        // Sets thread priority in realtion to initForCallback priority class.
        // Allowed values:
        //  4 - normal priority (same as assigned by default),
        //  5 - above normal,
        //  6 - high,
        //  7 - time-critical,
        //  3 - below normal,
        //  2 - low,
        //  1 - background.
        // NOTE If the program is run from a non-root user, setting priorities 5..7 fails. Possible workarounds:
        //  a) Start program from root user (sudo).
        //  b) Add to /etc/security/limits.conf:
        //    * - rtprio 100
        //    Meaning: <user> <"hard" (kernel)|"soft" (program)|"-" (both)> <parameter kind> <limit value>.
      bool set_priority(_s_long p)
      {
        if (!_pctx) { return false; }
        struct __local
        {
            // level: 0 - min, 1 - avg, 2 - high, 3 - highest.
          static bool set_sp(int pl, sched_param& sp, int level)
          {
            int sp1 = sched_get_priority_min(pl); int sp2 = sched_get_priority_max(pl); if (sp1 < 0 || sp2 < 0) { return false; }
            int spx;
            switch (level)
            {
              case 3: sp.sched_priority = sp2; break;
              case 2: spx = sp2 - 1; if (spx < sp1) { spx = sp1; } sp.sched_priority = spx; break;
              case 1: spx = (sp2 + sp1 + 1) / 2; if (spx > sp2 - 1) { spx = sp2 - 1; } if (spx < sp1) { spx = sp1; } sp.sched_priority = spx; break;
              default: sp.sched_priority = sp1; break;
            }
            return true;
          }
        };

        int pl; struct sched_param sp; int lev;
        if (pthread_getschedparam(_th_native, &pl, &sp) != 0) { return false; }
        int pl_low = SCHED_OTHER;
        #ifdef SCHED_IDLE
          pl_low = SCHED_IDLE;
        #endif

        switch (p)
        {
          case 4: { pl = SCHED_OTHER; lev = 3; bool b = __local::set_sp(pl, sp, lev) && 0 == pthread_setschedparam(_th_native, pl, &sp); return b; }
          case 5: { pl = SCHED_RR; lev = 2; bool b = __local::set_sp(pl, sp, lev) && 0 == pthread_setschedparam(_th_native, pl, &sp); return b; }
          case 6: { pl = SCHED_RR; lev = 3; bool b = __local::set_sp(pl, sp, lev) && 0 == pthread_setschedparam(_th_native, pl, &sp); return b; }
          case 7: { pl = SCHED_FIFO; lev = 3; bool b = __local::set_sp(pl, sp, lev) && 0 == pthread_setschedparam(_th_native, pl, &sp); return b; }
          case 1: { pl = pl_low; lev = 0; bool b = __local::set_sp(pl, sp, lev) && 0 == pthread_setschedparam(_th_native, pl, &sp); return b; }
          case 2: { pl = SCHED_OTHER; lev = 1; bool b = __local::set_sp(pl, sp, lev) && 0 == pthread_setschedparam(_th_native, pl, &sp); return b; }
          case 3: { pl = SCHED_OTHER; lev = 2; bool b = __local::set_sp(pl, sp, lev) && 0 == pthread_setschedparam(_th_native, pl, &sp); return b; }
          default: return false;
        }

        return false;
      }

        // Sets b_stop() of the target thread.
        // Returns:
        //    true - successful, the client may wait until state() becomes != 2.
        //    false - could not set the flag, threadctl has no context. state() will return 0.
      bool signal_stop() { if (!_pctx) { return false; } if (!_pctx->__bs) { _pctx->__bs = 1; } return true; }

        // Sets pctx()->b_stop() flag, and possibly waits for thread exiting.
        //    If thread had exited, detaches from its context (which is automatically destroyed).
        // timeout_ms:
        //    >0 - sleeps approx.  (timeout_ms / 10) times * 10 ms,
        //      checking thread state and returns on timeout or thread having exited.
        //    0 - yields once (sleep(0)), then checks thread state and returns.
        //    -1 immediately detaches from thread context.
        //      Thread context will be destroyed later by thread itself when it exits.
        //  Returns:
        //    2 - thread has exited normally.
        //    1 - thread already exited.
        //    0 - thread was not running (no context).
        //    -1 - detached.
        //    -2 - still running (timeout).
      int stop(_s_long timeout_ms)
      {
        if (!_pctx) { return 0; }
        if (!_pctx->__bs) { _pctx->__bs = 1; }
        if (!_pctx->__in_thread) { _pctx->_unset_in_ctl(); _pctx = 0; _tid.clear(); return 1; }
        _s_long t = timeout_ms;
        if (t == 0) { sleep_mcs(0); }
        while (t > 0) { sleep_mcs(t > 10 ? 10000 : t * 1000); t -= 10; }
        if (!_pctx->__in_thread) { _pctx->_unset_in_ctl(); _pctx = 0; _tid.clear(); return 2; }
        if (timeout_ms < 0) { _pctx->_unset_in_ctl(); _pctx = 0; _tid.clear(); return -1; }
        return -2;
      }

        //  Sets exit pctx()->b_stop() flag and sends cancellation request.
        //    The thread will exit as soon as possible without breaking whole program state.
        //  Returns:
        //    2 - cancellation request sent.
        //      Automatic cancellation points:
        //          accept, open, sendmsg, close, pause, sendto,
        //          connect, read, system, fcntl, recv, tcdrain
        //          fsync, recvfrom, wait, lseek, recvmsg, waitpid
        //          msync, send, write, nanosleep
        //    1 - thread already exited.
        //    0 - thread was not running (no context).
      int terminate()
      {
        if (!_pctx) { return 0; }
        if (!_pctx->__bs) { _pctx->__bs = 1; }
        if (!_pctx->__in_thread) { _pctx->_unset_in_ctl(); _pctx = 0; _tid.clear(); return 1; }
        #ifdef __ANDROID__
          pthread_kill(_th_native, SIGUSR1);
        #else
          pthread_cancel(_th_native);
        #endif
        _pctx->_unset_in_thread(); _pctx->_unset_in_ctl();
        _pctx = 0; _tid.clear();
        return 2;
      }

      private: ctx_base* _pctx; tid _tid; pthread_t _th_native;
      static void* _thproc(void* _p)
      {
        #ifdef __ANDROID__
          struct _local1 { static void hsig(int sig) { pthread_exit(0); } };
          struct sigaction act; memset(&act, 0, sizeof(act)); sigemptyset(&act.sa_mask); act.sa_flags = 0; act.sa_handler = _local1::hsig;
          sigaction(SIGUSR1, &act, NULL);
        #endif
        try { ((ctx_base*)_p)->_thread_proc(); } catch (...) {}
        ((ctx_base*)_p)->_unset_in_thread();
        return 0;
      }
        // IMPORTANT:
        //  threadctl may not be copied (1 threadctl per 1 thread).
        //  Use cref_t to handle threadctl instances when multiple references needed.
        //  Also, threadctl functions are not thread-safe (may be called only by one thread at a time).
      threadctl(const threadctl&); threadctl& operator=(const threadctl&);
    };


    struct processctl
    {
      bool has_ref() const throw () { return _pid > 0; }
      operator bool() const throw () { return _pid > 0; }

        // On has_ref() == true, pid() has meaningful value.
      typedef pid_t t_pid;
      t_pid pid() const throw() { return t_pid(_pid); }

      processctl() throw() : _pid(-1) {}
      ~processctl() throw() { clear(); }

      void clear() throw() { _pid = -1; if (_has_sigchld()) { _handler_sigchld(0, 0, 0); } }

        // Creates new initForCallback and returns immediately.
        // fnp_process with b_shell == false: must be full path and executable name (for posix_spawn()).
        //   fnp_process with b_shell == true: must be executable path and name in any form (for system()).
        // args: all program arguments, separated with null character ('\0').
        //   Each argument will be pre-processed with arg1() as necessary,
        //   so the client must not do any special processing (escaping etc.).
        // b_shell true: use CreateProcessA() to launch initForCallback.
        //   On success, launch() returns true. Also, has_ref() and pid() reflect the result.
        //   On failure, launch() returns false. Also, has_ref() == false, and pid() value in not valid.
        // b_shell false (dflt.): use system("... &") to launch initForCallback (asynchronously).
        //   NOTE This case sets custom SIGCHLD handler.
        //   In this case, has_ref() == false and pid() value is not valid.
        //   On success, launch() returns true.
        //   On failure, launch() returns false.
      bool launch(const std::string& fnp_process, const std::string& args, bool b_shell = false)
      {
        clear();
        if (fnp_process.empty()) { return false; }

        if (b_shell)
        {
          std::string a2; size_t pos = 0, pos2 = 0, end = std::string::npos;
          while (pos < args.length())
          {
            pos2 = args.find('\0', pos);
            if (pos2 == end) { pos2 = args.length(); }
            if (pos2 - pos > 1) { if (a2.length()) { a2 += ' '; } a2 += arg1(args.substr(pos, pos2 - pos), b_shell); }
            pos = pos2 + 1;
          }
          std::string s;
          pos = 0, pos2 = 0, end = std::string::npos;
          while (pos < fnp_process.length())
          {
            pos2 = fnp_process.find_first_of("' ", pos);
              if (pos2 == end) { s += fnp_process.substr(pos, fnp_process.length() - pos); break; }
            s += fnp_process.substr(pos, pos2 - pos);
            s += '\\';
            s += fnp_process[pos2];
            pos = pos2 + 1;
          }
          if (a2.length()) { s += ' '; s += a2; }
          s += " &";
          int res = system(s.c_str());
          return res == 0;
        }
        else
        {
          std::string z = args;
              for (size_t i = 0; i < z.length(); ++i)
                { if (z[i] >= 0 && z[i] < 32) { switch(z[i]) { case '\r': case '\n': case '\t': case '\v': case '\f': break; default: z[i] = '?'; break; } } }
          std::basic_string<char*> a2;
          std::string sp = fnp_process;
          a2 += &sp[0];
          size_t pos = 0, pos2 = 0, end = std::string::npos;
          while (pos < z.length())
          {
            pos2 = z.find('\0', pos);
            if (pos2 == end) { pos2 = z.length(); }
            if (pos2 - pos > 1) { a2 += &z[pos]; }
            pos = pos2 + 1;
          }
          a2 += (char*)0;

          struct sigaction act; memset(&act, 0, sizeof(act)); act.sa_sigaction = &_handler_sigchld; sigemptyset(&act.sa_mask); act.sa_flags = SA_NOCLDSTOP|SA_RESTART|SA_SIGINFO;
          sigaction(SIGCHLD, &act, NULL);
          _has_sigchld() = true;

          struct _local1
          {
            inline static int spawn(pid_t* pid, const char* path, char* const argv[], char* const envp[])
            #ifdef __ANDROID__
              {
                volatile int error = 0; pid_t p = vfork();
                switch (p)
                {
                  case -1: return errno;
                  case 0: execve(path, argv, envp); error = errno; _exit(127);
                  default:
                    if (error != 0) { waitpid(p, NULL, WNOHANG); } else { if (pid != NULL) { *pid = p; } }
                    return error;
                }
              }
            #else
              { return posix_spawn(pid, path, 0, 0, argv, envp); }
            #endif
          };
          pid_t __pid;
          int res = _local1::spawn(&__pid, &sp[0], &a2[0], environ);
          if (res != 0) { return false; }
          _pid = __pid;
          return true;
        }
      }

        // Waits for the launched initForCallback completion, return its exit code or termination code.
        //  Returned value:
        //    2 - the initForCallback has exited during waiting. *ret_pec = the process exit code.
        //    1 - the initForCallback has been terminated during waiting. *ret_pec = the process termination code.
        //    0 - the initForCallback had been completed before wait_exit() was called. *ret_pec = 0.
        //    -1 - the initForCallback is not started, nothing to wait. *ret_pec not changed.
        //    -2 - failure, initForCallback may exist, but its state is undefined. *ret_pec not changed.
      int wait_exit(long* ret_pec = 0) throw()
      {
        if (_pid == -2) { if (ret_pec) { *ret_pec = 0; } return 0; }
        if (_pid < 0) { return -1; }
        int st;
        long res = waitpid(pid_t(_pid), &st, 0);
        if (res != _pid) { return -2; }
        _pid = -2;
        if (WIFEXITED(st)) { if (ret_pec) { *ret_pec = long(WEXITSTATUS(st)); } return 2; }
        if (WIFSIGNALED(st)) { if (ret_pec) { *ret_pec = long(WTERMSIG(st)); } return 1; }
        return -2;
      }

      bool is_running() const throw()
      {
        if (!has_ref()) { return false; }
        int res = kill(pid_t(_pid), 0);
        return res == 0;
      }

        // Represent s as single argument for passing into program command line.
        //  (Appropriate quoting and escaping is made as necessary).
        //  b_shell false (dflt.) preserves CR and LF characters, intended for passing into posix_spawn() as is.
        //  b_shell true replaces CR and LF characters with ' ', intended for passing single line into system().
        //  In all cases, horz./vert. tab. and formfeed characters are kept.
        //  Other ASCII control characters (0..31) are replaced with '?'.
      static std::string arg1(const std::string& s, bool b_shell = false)
      {
        bool b = false;
        for (size_t i = 0; i < s.length(); ++i)
          { if ( ! ((s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z') || (s[i] >= '0' && s[i] <= '9') || s[i] == '-' || s[i] == '_' || s[i] == '.') )
            { b = true; break; } }
        if (!b) { return s; }

        std::string z = s; b = b_shell;
            for (size_t i = 0; i < z.length(); ++i)
              { if (z[i] >= 0 && z[i] < 32) { switch(z[i]) { case '\r': case '\n': if (b) { z[i] = ' '; } break; case '\t': case '\v': case '\f': break; default: z[i] = '?'; break; } } }

        if (!b_shell) { return z; }

        std::string z2 = "'";
          size_t pos = 0, pos2 = 0, end = std::string::npos;
          while (pos < z.length())
          {
            pos2 = z.find("'", pos);
              if (pos2 == end) { z2 += z.substr(pos, z.length() - pos); break; }
            z2 += z.substr(pos, pos2 - pos);
            z2 += "'\\''";
            pos = pos2 + 1;
          }
        z2 += '\'';
        return z2;
      }

    private:
      long _pid;
      struct _invoke1 { ~_invoke1() { _handler_sigchld(0, 0, 0); }  };
      static bool& _has_sigchld() { static bool b(false); static _invoke1 x1; (void)x1; return b; }
      static void _handler_sigchld(int, siginfo_t *, void *)
        { pid_t p; do { p = waitpid(-1, NULL, WNOHANG); } while (p != (pid_t)0 && p != (pid_t)-1); }
    };



    struct console_io
    {
      template<class _ = bmdx_meta::nothing>
      struct _lock_t { static int n; static termios t0; };
      typedef _lock_t<> t_lock;

      void _set_unbuf(bool on) throw()
      {
        t_lock __lock; if (sizeof(__lock)) {}
        t_lock::n += on ? 1 : -1;
        if (t_lock::n == 1)
        {
          tcgetattr(STDIN_FILENO, &t_lock::t0);
            termios t1 = t_lock::t0;
            t1.c_lflag &= ~(ICANON | ECHO);
          tcsetattr(STDIN_FILENO, TCSANOW, &t1);
        }
        else if (t_lock::n == 0) { tcsetattr(STDIN_FILENO, TCSANOW, &t_lock::t0); }
        else if (t_lock::n < 0) { t_lock::n = 0; }
      }

      console_io() throw() { _set_unbuf(true); }
      ~console_io() throw() { _set_unbuf(false); }

      unsigned int ugetch(unsigned int no_char = 0) throw()
      {
        t_lock __lock; if (sizeof(__lock)) {}
        int f0 = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, f0 | O_NONBLOCK);
          int ch = getchar();
        fcntl(STDIN_FILENO, F_SETFL, f0);
        if (ch != EOF) { ungetc(ch, stdin); return getchar(); } else { return no_char; }
      }
    };
    template<class _> int console_io::_lock_t<_>::n = 0;
    template<class _> termios console_io::_lock_t<_>::t0;
  }
#endif



#if defined(yk_c_vecm_hashx_H) && !defined(_bmdx_cpiomt_threadctl_spec)
  #define _bmdx_cpiomt_threadctl_spec
  namespace yk_c { template<> struct vecm::spec<bmdx::threadctl> { struct aux : vecm::config_aux<bmdx::threadctl> { };  typedef config_t<bmdx::threadctl, 1, 4, 0, aux> config; }; }
#endif



namespace bmdx
{
  #ifndef __bmdx__clock_ms
    #define __bmdx__clock_ms
    static double clock_ms() { return std::clock() * (1000. / CLOCKS_PER_SEC); }
  #endif



  struct file_io
  {
      // Factual type of the offset in file.
    #if defined(_MSC_VER)
      typedef __int64 t_offset;
    #elif defined(__FreeBSD__)
      typedef __off_t t_offset;
    #elif __APPLE__ && __MACH__
      typedef off_t t_offset;
    #else
       typedef off64_t t_offset;
    #endif

    //  is_open, result.
    //  open, close, seek, seek_end, tell, read, write, flush.
    //  static: is_ex_file, is_ex_dir, load_bytes, save_bytes

    file_io() throw() : _desc(0), _mode(0), _res(0), _nwr(0), _last_op_wr(false) {}
    ~file_io() throw() { if (is_open()) { close(); } }

      // NOTE Copy constructor does not actually copy, its equivalent to wav_fs().
      //    Defined for basic compatibility with library containers.
    file_io(const file_io&) throw() { new (this) file_io(); }

    inline bool is_open() const throw() { return bool(_desc); }
      // mode 0: file is not open, mode 1: file is open read-only, mode 2: file is open read-write.
    inline _s_long mode() const throw() { return _desc ? _mode : 0; }
    inline _s_long result() const throw() { return _res; }

      // Opens or reopens a file with the specified name and parameters.
      // result():
      //    1 - success,
      //    -1 opening existing file for reading failed,
      //    -2 opening existing file for r/w failed,
      //    -3 file does not exist, cannot open for reading.
      //    -4 file does not exist, file creation (for r/w) failed.
      //    -5 pfilename is null.
    inline void open(const char* pfilename, bool can_wrcr, bool wr_trunc = false) throw()
    {
      if (!pfilename || *pfilename == '\0') { _res = -5; }
      if (is_open()) { close(); }
      if (is_ex_file(pfilename)) { if (can_wrcr) { _desc = ::fopen(pfilename, wr_trunc ? "w+b" : "r+b"); _res = _desc ? 1 : -2; } else { _desc = ::fopen(pfilename, "rb"); _res = _desc ? 1 : -1; } }
        else { if (can_wrcr) { _desc = ::fopen(pfilename, "w+b"); _res = _desc ? 1 : -4; } else { _res = -3; } }
      _mode = can_wrcr ? 2 : 1;
      if (_res == 1) { setvbuf(_desc, 0, _IOFBF, _nwrchunk); }
    }

      // result():
      //    1 - success,
      //    0 - the file was not open,
      //    -1 - an error, the file is regarded closed.
    inline void close() throw()
    {
      if (!is_open()) { _res = 0; return; }
      _res = fclose(_desc) == 0 ? 1 : -1;
      _desc = 0;
    }

      // result():
      //    1 - success,
      //    -1 - the file is not open,
      //    -2 - seek error.
    inline void seek(_s_ll pos) throw()
    {
      if (!is_open()) { _res = -1; return; }
      if (pos < 0) { _res = -2; return; }
      if (_last_op_wr && _nwr) { fflush(_desc); _nwr = 0; }
      _res = _seek_u(pos) == 0 ? 1 : -2;
    }

      // result():
      //    1 - success,
      //    -1 - the file is not open,
      //    -2 - seek error.
    inline void seek_end() throw()
    {
      if (!is_open()) { _res = -1; return; }
      if (_last_op_wr && _nwr) { fflush(_desc); _nwr = 0; }
      #if defined(_MSC_VER)
        _res = _fseeki64(_desc, 0, SEEK_END) == 0 ? 1 : -2;
      #elif defined(__FreeBSD__) || (__APPLE__ && __MACH__) || defined(__ANDROID__)
        _res = fseeko(_desc, 0, SEEK_END) == 0 ? 1 : -2;
      #else
        _res = fseeko64(_desc, 0, SEEK_END) == 0 ? 1 : -2;
      #endif
    }

      // Returns valid pos. >=0, or -1 on error.
      // result():
      //    1 - success,
      //    -1 - the file is not open,
      //    -2 - error.
    inline _s_ll tell() const throw()
    {
      if (!is_open()) { _res = -1; return -1; }
      _s_ll pos = _tell_u();
      _res = pos >= 0 ? 1 : -2;
      return pos;
    }

      // Returns: number of bytes factually read.
      // result():
      //    1 - success, size bytes have been read
      //    -1 - the file is not open,
      //    -2 - read error, pos. is not changed,
      //    -3 - read error, pos. is changed.
    inline size_t read(void* dest, size_t size) throw()
    {
      if (!is_open()) { _res = -1; return 0; }
      if (_last_op_wr)
      {
        if (_nwr) { fflush(_desc); _nwr = 0; }
        _s_ll pos = _tell_u();
        if (pos < 0) { _res = -2; return 0; }
        rewind(_desc);
        if (_seek_u(pos) != 0) { _res = -3; return 0; }
      }
      size_t s2 = fread(dest, 1, size, _desc);
      _last_op_wr = false;
      _res = s2 == size ? 1 : (s2 == 0 ? -2 : -3);
      return s2;
    }

      // Returns: number of bytes factually written.
      // result():
      //    1 - success, size bytes have been written
      //    -1 - the file is not open,
      //    -2 - write error, file/pos is not changed,
      //    -3 - write error, file/pos is changed.
    inline size_t write(const void* src, size_t size) throw()
    {
      if (!is_open()) { _res = -1; return 0; }
      if (!_last_op_wr)
      {
        _s_ll pos = _tell_u();
        if (pos < 0) { _res = -2; return 0; }
        rewind(_desc);
        if (_seek_u(pos) != 0) { _res = -3; return 0; }
      }
      if (_nwr + size >= _nwrchunk) { fflush(_desc); _nwr = 0; }
      size_t s2 = fwrite(src, 1, size, _desc);
      _last_op_wr = true;
      _res = s2 == size ? 1 : (s2 == 0 ? -2 : -3);
      _nwr += s2;
      return s2;
    }

      // result():
      //    1 - success,
      //    -1 - the file is not open,
      //    -2 - operation error.
      // NOTE Due to system caching, no guarantee that flush() writes to disk immediately.
    inline void flush() throw()
    {
      if (!is_open()) { _res = -1; return; }
      _res = fflush(_desc) == 0 ? 1 : -2;
    }

      // Returns true if the specified path points to the existing file, false in all other cases.
    inline static bool is_ex_file(const char* ppath) throw()
    {
      if ( ppath && *ppath != '\0' && 0 == access(ppath, __F_OK) )
        { struct stat st; stat(ppath, &st); return bool(st.st_mode & S_IFREG); }
      else { return false; }
    }
    inline static bool is_ex_file(const std::string& path) throw() { return is_ex_file(path.c_str()); }

      // Returns true if the specified path points to the existing directory, false in all other cases.
    inline static bool is_ex_dir(const char* ppath) throw()
    {
      if ( ppath && *ppath != '\0' && 0 == access(ppath, __F_OK) )
        { struct stat st; stat(ppath, &st); return bool(st.st_mode & S_IFDIR); }
      else { return false; }
    }
    inline static bool is_ex_dir(const std::string& path) throw() { return is_ex_dir(path.c_str()); }

  private: struct _stra_base; struct _stra_ca;  struct _stra_str;
  public:

        // Loads bytes from the given file into the dest. container. Resizes the container as necessary.
        // 1 - success.
        // 0 - file does not exist.
        // -1 - memory alloc. error, or wrong arguments.
        // -2 - file i/o error. NOTE On i/o error, dest may be left modified.
    inline static int load_bytes(const std::string& fnp, std::string& dest) throw() { return _load_bytes(fnp.c_str(), _stra_str(), &dest); }
    inline static int load_bytes(const char* fnp, std::string& dest) throw() { return _load_bytes(fnp, _stra_str(), &dest); }

    inline static int load_bytes(const std::string& fnp, _carray_base_t<char>& dest) throw() { return _load_bytes(fnp.c_str(), _stra_ca(), &dest); }
    inline static int load_bytes(const char* fnp, _carray_base_t<char>& dest) throw() { return _load_bytes(fnp, _stra_ca(), &dest); }
    inline static int load_bytes(const std::string& fnp, _carray_base_t<unsigned char>& dest) throw() { return _load_bytes(fnp.c_str(), _stra_ca(), &dest); }
    inline static int load_bytes(const char* fnp, _carray_base_t<unsigned char>& dest) throw() { return _load_bytes(fnp, _stra_ca(), &dest); }
    inline static int load_bytes(const std::string& fnp, _carray_base_t<signed char>& dest) throw() { return _load_bytes(fnp.c_str(), _stra_ca(), &dest); }
    inline static int load_bytes(const char* fnp, _carray_base_t<signed char>& dest) throw() { return _load_bytes(fnp, _stra_ca(), &dest); }

        // Saves bytes from src to the given file.
        //    b_append == false truncates the file before writing, if it exists.
        //    if n == 0, pdata may be 0.
        // 1 - success.
        // 0 - failed to create file (or open the existing file for writing).
        // -1 - data size too large, or memory alloc. error, or wrong arguments.
        // -2 - file i/o error. NOTE On i/o error, the file may be left modified.
    inline static int save_bytes(const char* fnp, const std::string& src, bool b_append) throw() { return save_bytes(fnp, src.c_str(), src.length(), b_append); }
    inline static int save_bytes(const std::string& fnp, const std::string& src, bool b_append) throw() { return save_bytes(fnp.c_str(), src, b_append); }

    inline static int save_bytes(const char* fnp, const arrayref_t<char>& src, bool b_append) throw() { return save_bytes(fnp, src.pd(), src.n(), b_append); }
    inline static int save_bytes(const std::string& fnp, const arrayref_t<char>& src, bool b_append) throw() { return save_bytes(fnp.c_str(), src.pd(), b_append); }
    inline static int save_bytes(const char* fnp, const arrayref_t<unsigned char>& src, bool b_append) throw() { return save_bytes(fnp, (const char*)src.pd(), src.n(), b_append); }
    inline static int save_bytes(const std::string& fnp, const arrayref_t<unsigned char>& src, bool b_append) throw() { return save_bytes(fnp.c_str(), (const char*)src.pd(), b_append); }
    inline static int save_bytes(const char* fnp, const arrayref_t<signed char>& src, bool b_append) throw() { return save_bytes(fnp, (const char*)src.pd(), src.n(), b_append); }
    inline static int save_bytes(const std::string& fnp, const arrayref_t<signed char>&  src, bool b_append) throw() { return save_bytes(fnp.c_str(), (const char*)src.pd(), b_append); }

    static int save_bytes(const char* fnp, const char* pdata, _s_ll n0, bool b_append) throw()
    {
      if (!(fnp && n0 >= 0 && !(n0 && !pdata))) { return -1; }
      size_t n = size_t(n0); if (_s_ll(n) != n0) { return -1; }
      file_io f; f.open(fnp, true, !b_append); if (!f.is_open()) { return 0; }
      if (!n) { return 1; }
      if (b_append) { f.seek_end(); if (f.result() < 1) { return -2; } }
      f.write(pdata, n); if (f.result() < 1) { return -2; }
      return 1;
    }

  private:
    file_io& operator=(const file_io&);

    static const int __F_OK = 0;

    FILE* _desc;
    _s_long _mode;
    mutable _s_long _res;
    mutable size_t _nwr;
    mutable bool _last_op_wr;
    static const size_t _nwrchunk = 2048;

    inline _s_ll _tell_u() const throw()
    {
      #if defined(_MSC_VER)
        return _s_ll(_ftelli64(_desc));
      #elif defined(__FreeBSD__) || (__APPLE__ && __MACH__) || defined(__ANDROID__)
        return _s_ll(ftello(_desc));
      #else
        return _s_ll(ftello64(_desc));
      #endif
    }
    inline int _seek_u(_s_ll pos) throw()
    {
      int res;
      #if defined(_MSC_VER)
        res = _fseeki64(_desc, t_offset(pos), SEEK_SET);
      #elif defined(__FreeBSD__) || (__APPLE__ && __MACH__) || defined(__ANDROID__)
        res = fseeko(_desc, t_offset(pos), SEEK_SET);
      #else
        res = fseeko64(_desc, t_offset(pos), SEEK_SET);
      #endif
      return res;
    }

    struct _stra_base { typedef std::string::size_type _t_sz; virtual bool _resize(void* ps, _s_ll n0) const throw() = 0; virtual char* _pd(void* ps) const throw() = 0; };
    struct _stra_str : _stra_base { virtual bool _resize(void* ps, _s_ll n0) const throw() { typedef std::string T; _t_sz n = _t_sz(n0); if (_s_ll(n) != n0) { return false; } try { if (n) { ((T*)ps)->resize(n); } else { ((T*)ps)->clear(); } return true; } catch (...) { return false; } } virtual char* _pd(void* ps) const throw() { typedef std::string T; return &(*(T*)ps)[0]; } };
    struct _stra_ca : _stra_base { virtual bool _resize(void* ps, _s_ll n0) const throw() { typedef _carray_base_t<char> T; return ((T*)ps)->realloc(n0, 0, 0, 0); } virtual char* _pd(void* ps) const throw() { typedef _carray_base_t<char> T; return ((T*)ps)->pd(); } };

    static int _load_bytes(const char* fnp, const _stra_base& a, void* ps) throw()
    {
      if (!fnp) { return -1; }
      file_io f; f.open(fnp, false); if (!f.is_open()) { return f.result() == -3 ? 0 : -2; }
      f.seek_end(); if (f.result() < 1) { return -2; }
      _s_ll n0 = f.tell(); if (n0 < 0) { return -2; }
      f.seek(0); if (f.result() < 1) { return -2; }
      if (_s_ll(size_t(n0)) != n0) { return -1; }
      if (!a._resize(ps, n0)) { return -1; }
      if (!n0) { return 1; }
      f.read(a._pd(ps), size_t(n0)); if (f.result() < 1) { a._resize(ps, 0); return -2; }
      return 1;
    }
  };






    // Thread-safe const reference type.

  struct cref_nonlock {};
  template<class T> struct _cref_lock_t : critsec_t<T> { typedef critsec_t<T> t_impl; _cref_lock_t() : t_impl(10, -1) {} };
  template<> struct _cref_lock_t<cref_nonlock> {};

  template<class T, class LockSelector = T>
  struct cref_t
  {
    typedef T t_value; typedef _cref_lock_t<LockSelector> t_lock;

    struct exc_ref : std::exception { const char* what() const throw () { return "bmdx::cref_t::ref"; } };
    struct exc_ref_ts : std::exception { const char* what() const throw () { return "bmdx::cref_t::ref_ts"; } };
    struct exc_create0 : std::exception { const char* what() const throw () { return "bmdx::cref_t::create0"; } };
    struct exc_create1 : std::exception { const char* what() const throw () { return "bmdx::cref_t::create1"; } };
    struct exc_create2 : std::exception { const char* what() const throw () { return "bmdx::cref_t::create2"; } };
    struct exc_create3 : std::exception { const char* what() const throw () { return "bmdx::cref_t::create3"; } };
    struct exc_create4 : std::exception { const char* what() const throw () { return "bmdx::cref_t::create4"; } };
    struct exc_assign : std::exception { const char* what() const throw () { return "bmdx::cref_t::assign"; } };
    struct exc_copy : std::exception { const char* what() const throw () { return "bmdx::cref_t::copy"; } };
    struct exc_cc3 : std::exception { const char* what() const throw () { return "bmdx::cref_t(x,is_own,false)"; } };

      // ref():
      //  a) on is_empty() == true, returns a ref. to valid object.
      //  b) on is_empty() == false, generates an exception.
      // NOTE ref() is unsafe if the current object may be overwritten
      //  from another thread. Use ref_ts() to overcome this.
    const t_value& ref() const { if (!_p) { throw exc_ref(); } return *_p; }
    const t_value& ref_ts() const { t_lock __lock; if (sizeof(__lock)) {} if (!_p) { throw exc_ref_ts(); } return *_p; }

    bool is_own() const throw() { return bool(_pcnt); } // true if this object contains a strong reference
    bool has_ref() const throw () { return bool(_p); } // false only if a) no-exception construction failed b) after cref_t()
    operator bool() const throw () { return bool(_p); }
    const t_value* ptr() const throw () { return _p; }

      // Non-const pointer to object.
      // NOTE Treating constant as variable is unsafe unless you control all side effects,
      //  esp. in multithreaded program.
    t_value* _pnonc_u() const throw () { return const_cast<t_value*>(_p); }

      // n_refs():
      //  0 - unknown (weak ref., is_own() == false).
      //  >=1 - number of strong references (on is_own() == true).
      //  -1 - failed to lock the object.
    _s_long n_refs() const throw() { t_lock __lock; if (sizeof(__lock)) {} return _pcnt ? * _pcnt : 0; }

      // Copy object, and own the new copy.
      //  The copy will be deleted when, later, its last owning cref object is deleted.
    cref_t(const t_value& x, bool no_exc) : _pcnt(0), _p(0) { copy(x, no_exc); }

      // Set a strong (is_own_ == true), or a weak reference to an object (is_own_ == false).
      //  NOTE: on is_own_ == true,
      //    1) x must have been created dynamically.
      //    2) the current cref object must be the first existing owner.
    cref_t(const t_value& x, bool is_own_, bool no_exc) : _pcnt(0), _p(0) { if (is_own_) { try { _pcnt = new _s_long(1); } catch (...) { if (!no_exc) { throw exc_cc3(); } return; } } _p = &x; }

      // Object creation with 0..3 arguments.
      //    On success, the previous object reference is correctly removed, and the new is set on its place.
      //    On failure, the previous object reference remains unchanged.
      //  NOTE create1 with Arg1 == t_value behaves same as copy().
    bool create0(bool no_exc) { t_lock __lock; if (sizeof(__lock)) {} _s_long* pcnt2(0); const t_value* p2(0); try { p2 = new t_value(); } catch (...) {} try { pcnt2 = new _s_long(1); } catch (...) {} if (p2 && pcnt2) { _reset(); _p = p2; _pcnt = pcnt2; return true; } try { delete p2; } catch (...) {} try { delete pcnt2; } catch (...) {} if (!no_exc) { throw exc_create0(); } return false; }
    template<class Arg1> bool create1(bool no_exc, const Arg1& x1) { t_lock __lock; if (sizeof(__lock)) {} _s_long* pcnt2(0); const t_value* p2(0); try { p2 = new t_value(x1); } catch (...) {} try { pcnt2 = new _s_long(1); } catch (...) {} if (p2 && pcnt2) { _reset(); _p = p2; _pcnt = pcnt2; return true; } try { delete p2; } catch (...) {} try { delete pcnt2; } catch (...) {} if (!no_exc) { throw exc_create1(); } return false; }
    template<class Arg1, class Arg2> bool create2(bool no_exc, const Arg1& x1, const Arg2& x2) { t_lock __lock; if (sizeof(__lock)) {} _s_long* pcnt2(0); const t_value* p2(0); try { p2 = new t_value(x1, x2); } catch (...) {} try { pcnt2 = new _s_long(1); } catch (...) {} if (p2 && pcnt2) { _reset(); _p = p2; _pcnt = pcnt2; return true; } try { delete p2; } catch (...) {} try { delete pcnt2; } catch (...) {} if (!no_exc) { throw exc_create2(); } return false; }
    template<class Arg1, class Arg2, class Arg3> bool create3(bool no_exc, const Arg1& x1, const Arg2& x2, const Arg3& x3) { t_lock __lock; if (sizeof(__lock)) {} _s_long* pcnt2(0); const t_value* p2(0); try { p2 = new t_value(x1, x2, x3); } catch (...) {} try { pcnt2 = new _s_long(1); } catch (...) {} if (p2 && pcnt2) { _reset(); _p = p2; _pcnt = pcnt2; return true; } try { delete p2; } catch (...) {} try { delete pcnt2; } catch (...) {} if (!no_exc) { throw exc_create3(); } return false; }
    template<class Arg1, class Arg2, class Arg3, class Arg4> bool create4(bool no_exc, const Arg1& x1, const Arg2& x2, const Arg3& x3, const Arg4& x4) { t_lock __lock; if (sizeof(__lock)) {} _s_long* pcnt2(0); const t_value* p2(0); try { p2 = new t_value(x1, x2, x3, x4); } catch (...) {} try { pcnt2 = new _s_long(1); } catch (...) {} if (p2 && pcnt2) { _reset(); _p = p2; _pcnt = pcnt2; return true; } try { delete p2; } catch (...) {} try { delete pcnt2; } catch (...) {} if (!no_exc) { throw exc_create4(); } return false; }

      // NOTE The following 5 functions do not throw exceptions.
    cref_t()         throw(): _pcnt(0), _p(0) {}
    cref_t(const cref_t& x)        throw(): _pcnt(0), _p(0) { t_lock __lock; if (sizeof(__lock)) {} if (x._pcnt) { _pcnt = x._pcnt; ++*_pcnt; } _p = x._p; }
    ~cref_t()        throw () { if (!_pcnt) { _p = 0; return; } t_lock __lock; if (sizeof(__lock)) {} _reset(); }
    cref_t& operator=(const cref_t& x)        throw() { t_lock __lock; if (sizeof(__lock)) {} if (x._p == _p) { if (x.is_own() && !is_own()) { _pcnt = x._pcnt; ++*_pcnt; } } else { _reset(); if (x.is_own()) { _pcnt = x._pcnt; ++*_pcnt; } _p = x._p; } return *this; }
    void clear()        throw () { t_lock __lock; if (sizeof(__lock)) {} _reset(); }


      // Returns true on success, false on failure with no_exc == true,
      //    or generates exception on failure with no_exc == false.
      // NOTE assign() may be used to change the kind of ownership on its referenced object.
      //    If the last strong ref. was changed to weak, the object may be deleted,
      //    but its pointer is still kept.
      // NOTE assign() does not copy x in any way. Do not assign references to temporary objects.
    bool assign(const t_value& x, bool is_own_, bool no_exc)
    {
      t_lock __lock; if (sizeof(__lock)) {}
      if (&x == _p) { if (is_own() == is_own_) { return true; } _reset(); if (!is_own_) { _p = &x; return true; } }
        else { _reset(); }
      if (is_own_) { try { _pcnt = new _s_long(1); } catch (...) { if (!no_exc) {  throw exc_assign(); } return false; } }
      _p = &x;
      return true;
    }

      // Copies the object and the copy becomes cref's own object.
      // Returns true on success, false on failure with no_exc == true,
      //    or generates exception (exc_copy) on failure with no_exc == false.
      //  On failure, the current reference is kept.
    bool copy(const t_value& x, bool no_exc)
    {
      t_lock __lock; if (sizeof(__lock)) {}
      _s_long* pcnt2(0); const t_value* p2(0);
      try { p2 = new t_value(x); } catch (...) {} try { pcnt2 = new _s_long(1); } catch (...) {}
      if (p2 && pcnt2) { _reset(); _p = p2; _pcnt = pcnt2; return true; }
      try { delete p2; } catch (...) {} try { delete pcnt2; } catch (...) {}
      if (!no_exc) { throw exc_copy(); }
      return false;
    }

  private:
    _s_long* _pcnt; const t_value* _p;
    void _reset() throw () { if (_pcnt) { bool b = (--*_pcnt) == 0; if (b) { try { delete _p; } catch (...) {} try { delete _pcnt; } catch (...) {} } _pcnt = 0; } _p = 0; }
  };

}

#undef _s_long
#undef _s_ll
#undef _u_ll

#endif

