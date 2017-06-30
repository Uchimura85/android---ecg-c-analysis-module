/*
In this module:
  rnd values, comparisons, conversions,
  common perf and validity tests for assoc. arrays,
  hash table chains profiling for hashx.

USAGE NOTE

This header must be included using the following sequence:

[1]. Base headers (if having different paths):

  #include "common/yk_tests_base.h"
  #include "bmdx_cpiomt.h"

2. In any client module:

  #define _test_yk_c_common_PREINCL
    #include "_test_yk_c_common.h"

3. In the first .cpp:

  #define yk_c_cm_vecm_STATIC
  #define yk_c_cm_std_string_STATIC

[4]. Tested headers (if having different paths):

  #include "vecm_hashx.h"
  #include "yk_cx.h"

[5]. For specific test sets or different path:
  #include "_test_yk_c_common.h"

6. Test sets of interest, e.g.:

  #include "_test_yk_c_vecm_hashx.h"
  #include "_test_yk_cx.h"

[7]. For specific test sets in any additional .cpp:

  #include "_test_yk_c_common.h"
  #include "_test_yk_c_vecm_hashx.h"
  #include "_test_yk_cx.h"

*/




#ifndef yk_tests_base_H
  #include "common/yk_tests_base.h"
#endif

#ifndef bmdx_cpiomt_H
  #include "bmdx_cpiomt.h"
#endif

#if !defined(_test_yk_c_common_LOCK) && !defined(bmdx_config_H)
  namespace yk_c
  {
    template<class T, class Aux> struct lock_impl_t : bmdx::critsec_t<T>
    {
      typedef T t_target; typedef bmdx::critsec_t<T> t_impl; struct exc_lock {};
      lock_impl_t() : t_impl(10, -1) { if (!this->t_impl::is_locked()) { throw exc_lock(); } }
    };
  }
#endif
#if !defined(_test_yk_c_common_LOCK)
  #define _test_yk_c_common_LOCK
#endif





#ifdef _test_yk_c_common_PREINCL
  #undef _test_yk_c_common_PREINCL
#else




#ifndef _test_yk_c_common_H
#define _test_yk_c_common_H

#ifdef _MSC_VER
#pragma warning(disable:4800)
#pragma warning(disable:4290)
#pragma warning(disable:4345)
#pragma warning(disable:4355)
#pragma warning(disable:4996)
#pragma warning(disable:4244)
#undef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef yk_c_vecm_hashx_H
  #include "vecm_hashx.h"
#endif

namespace __test_common_aux1
{
  using namespace yk_c;

  // NOTE Copy of yk_c::experimental::  storage_t, iterator_t (decoupling from yk_cx.h).

  template<class T, class Aux = meta::nothing>
  struct storage_t
  {
    template<int n1 = sizeof(T)  / sizeof(meta::s_ll), int n2 = sizeof(T)  % sizeof(meta::s_ll)> struct place { meta::s_ll x1[n1]; char x2[n2]; };
    template<int n> struct place<n, 0> { meta::s_ll x1[n]; };
    template<int n> struct place<0, n> { char x2[n]; };
    mutable place<> pl;
    char inited;
    const signed char mode;
    inline storage_t(s_long mode_) throw() : inited(false), mode((signed char)(mode_)) { if (mode >= 1) { try_init(); } }
    inline ~storage_t() throw() { if (mode >= 0) { try_deinit(); } }
      // 1 - on_calm_result, 0 - already initialized; -1 - failed to initialize, nothing changed.
    inline s_long try_init() throw() { if (inited) { return 0; } try { meta::construct_f<T, Aux>().f(ptr()); inited = true; return 1; } catch (...) {} return -1; }
    inline s_long try_init(const T& x) throw() { if (inited) { return 0; } try { new (ptr()) T(x); inited = true; return 1; } catch (...) {} return -1; }
      // 1 - on_calm_result, 0 - was not initialized; -1 - destructor failed, so just set inited to false.
    inline s_long try_deinit() throw() { if (inited) { try { T* p = ptr(); p->~T(); inited = false; return 1; } catch (...) {} inited = false; return -1; } return 0; }
    inline operator T*() const throw() { return reinterpret_cast<T*>(&pl); }
    inline T* ptr() const throw() { return reinterpret_cast<T*>(&pl); }
    inline operator T&() const throw() { return reinterpret_cast<T&>(*ptr()); }
  };

  template<class T, bool is_const> class iterator_t : public yk_c::vecm::link1_t<T, is_const>, public std::iterator<std::random_access_iterator_tag, T>
  {
  public:
    typedef yk_c::s_long s_long; typedef yk_c::vecm vecm;
    typedef T t_value; typedef t_value value_type;
    typedef typename yk_c::meta::if_t<is_const, const t_value*, t_value*>::result  pointer;
    typedef typename yk_c::meta::if_t<is_const, const t_value&, t_value&>::result reference;
    typedef yk_c::vecm::link1_t<t_value, is_const> t_link;
    typedef typename t_link::t_ctnr t_ctnr;
    typedef std::random_access_iterator_tag iterator_category; typedef iterator_t iterator_type; typedef meta::t_pdiff difference_type;

    inline iterator_t() throw() {}
    inline iterator_t(t_ctnr& v) throw() : t_link(v) {} // end pos.
    inline iterator_t(const t_link& l) throw() : t_link(l) {}
    inline iterator_t(t_ctnr& v, s_long ind) throw() : t_link(v, ind) {}
    inline iterator_t(const iterator_t<T, false>& x) throw() : t_link(x) {}
    inline iterator_t(const iterator_t<T, true>& x) throw() : t_link(x) {}

    inline reference operator*() const throw() { return *this->_px; }
    inline pointer operator->() const throw() { return this->_px; }
    inline reference operator[](difference_type delta) const throw() { return *this->_pv->template pval_0u<t_value>(this->_i + delta); }
    inline iterator_type& operator++() throw() { this->incr(); return *this; }
    inline iterator_type& operator--() throw() { this->decr(); return *this; }
    inline iterator_type& operator+=(difference_type delta) throw() { this->move_by(delta); return *this; }
    inline iterator_type& operator-=(difference_type delta) throw() { this->move_by(-delta); return *this; }
    inline iterator_type operator++(int) throw() { iterator_type i = *this; this->incr(); return i; }
    inline iterator_type operator--(int) throw() { iterator_type i = *this; this->decr(); return i; }
    inline iterator_type operator+(difference_type delta) const throw() { iterator_type i = *this; i.move_by(delta); return i; }
    inline iterator_type operator-(difference_type delta) const throw() { iterator_type i = *this; i.move_by(-delta); return i; }
    inline difference_type operator-(const iterator_type& x) const throw() { return this->ind0() - x.ind0(); }
    inline bool operator==(const iterator_type& x) const throw() { return this->is_eq(x); }
    inline bool operator!=(const iterator_type& x) const throw() { return !this->is_eq(x); }
    inline bool operator>(const iterator_type& x) const throw() { return this->ind0() > x.ind0(); }
    inline bool operator<(const iterator_type& x) const throw() { return this->ind0() < x.ind0(); }
    inline bool operator>=(const iterator_type& x) const throw() { return this->ind0() >= x.ind0(); }
    inline bool operator<=(const iterator_type& x) const throw() { return this->ind0() <= x.ind0(); }
  };
  template<class T, bool b> iterator_t<T, b> operator+(typename iterator_t<T, b>::difference_type delta, const iterator_t<T, b>& x) throw() { iterator_t<T, b> i(x); i.move_by(delta); return i; }

}

namespace yk_c
{
  template<class T, bool b> struct meta::type_equi<__test_common_aux1::iterator_t<T, b>, meta::tag_construct> { typedef meta::tag_construct t_3; };
  template<class T, bool b> struct meta::type_equi<__test_common_aux1::iterator_t<T, b>, meta::tag_functor> { typedef meta::tag_functor t_3; };
  #if defined(yk_c_vecm_hashx_H) && !defined(_bmdx_cpiomt_threadctl_spec)
    #define _bmdx_cpiomt_threadctl_spec
    template<> struct vecm::spec<bmdx::threadctl> { struct aux : vecm::config_aux<bmdx::threadctl> { };  typedef config_t<bmdx::threadctl, 1, 4, 0, aux> config; };
  #endif
}


#include <typeinfo>
#include <exception>
#include <map>
#ifndef _WIN32
  #include <stdio.h>
  #include <time.h>
  #ifdef _STRUCT_TIMEVAL
    #include <sys/time.h>
    #define __use_sys_time_h
  #elif (defined(_TYPES_H_) || defined(_SYS__TYPES_H_))
    #include <sys/time.h>
    #define __use_sys_time_h
  #endif
#endif

#ifndef yk_c_cm_vecm_H
#define yk_c_cm_vecm_H

  // Cross-module vecm as vecm element (test for 4 variants of structure).

namespace yk_c
{
  struct __column { mutable char* pd; s_long kbase; template<class _ = meta::nothing> struct __x; struct nothing{}; private: __column(const __column&); __column& operator=(const __column&); __column(); ~__column(); };
  // In particular compilers, where vecm has different structure,
  //   ksize may compile as 0. Still, __x may be specialized
  //   for _ == meta::nothing, with some unique ksize value,
  //   to keep vecm cross-module at least under that compiler.
  template<class _> struct __column::__x
  {
    enum
    {
      _v36 = sizeof(vecm) == 36, _v44 = sizeof(vecm) == 44, _v48 = sizeof(vecm) == 48, _c8 = sizeof(__column) == 8, _c12 = sizeof(__column) == 12, _c16 = sizeof(__column) == 16,
      _sa =
          + 1 * (_v36 && _c8)
          + 2 * (_v44 && _c12)
          + 3 * (_v44 && _c16)
          + 4 * (_v48 && _c16)
      ,
      _sb = sizeof(vecm::type_descriptor),
      k_size = _sa != 0 && (_sb & 0x3) == 0 ? (_sb << 1) + _sa : 0,
      k_tisize = k_size == 0 ? 1 : k_size
    };
    typedef typename meta::if_t<k_size == 0, nothing, vecm>::result t;
  };
  typedef __column::__x<>::t __cm_vecm;
  template<> struct bytes::type_index_t<__cm_vecm> : cmti_base_t<__cm_vecm, 2015, 5, 2, 18, __column::__x<>::k_tisize> {};
}

#endif

#ifdef yk_c_cm_vecm_STATIC
#undef yk_c_cm_vecm_STATIC
namespace yk_c { bytes::type_index_t<__cm_vecm> __inst_cmti_vecm; }
#endif



  // Cross-module std string as vecm element (test for MINGW and MSVC).
  // NOTE Passing strings and other objects, internally using
  // dynamic memory allocation, between binary modules is, in general, unsafe.
  // If modules are created with same compiler type / version,
  // read-only access will be safe in most cases.
  // Creation/modification/destruction may be safe only if modules iternally call
  // same memory allocation/freeing routines from some third module.

#ifndef yk_c_cm_std_string_H
#define yk_c_cm_std_string_H

#include <string>

#ifndef yk_c_cmti_string_DEFINED
namespace yk_c
{
  #if defined(__MINGW32_MAJOR_VERSION)
    template<> struct bytes::type_index_t<std::string> : cmti_base_t<std::string, 2015, 5, 2, 18, (sizeof(std::string) % 4 == 0) * (sizeof(std::string) / 3 ^ __MINGW32_MAJOR_VERSION * 37 ^ __MINGW32_MINOR_VERSION * 4)> {};
    #define yk_c_cmti_string_DEFINED
  #elif defined(_MSC_VER)
    template<> struct bytes::type_index_t<std::string> : cmti_base_t<std::string, 2015, 5, 2, 19, (sizeof(std::string) % 4 == 0) * (sizeof(std::string) / 3 ^ _MSC_VER / 11 ^ _MSC_VER % 16)> {};
    #define yk_c_cmti_string_DEFINED
  #endif
}
#endif

#ifndef yk_c_cmti_wstring_DEFINED
namespace yk_c
{
  #if defined(__MINGW32_MAJOR_VERSION)
    template<> struct bytes::type_index_t<std::wstring> : cmti_base_t<std::wstring, 2015, 6, 1, 12, (sizeof(std::wstring) % 4 == 0) * (sizeof(std::wstring) / 3 ^ __MINGW32_MAJOR_VERSION * 37 ^ __MINGW32_MINOR_VERSION * 4)> {};
    #define yk_c_cmti_wstring_DEFINED
  #elif defined(_MSC_VER)
    template<> struct bytes::type_index_t<std::wstring> : cmti_base_t<std::wstring, 2015, 6, 1, 13, (sizeof(std::wstring) % 4 == 0) * (sizeof(std::wstring) / 3 ^ _MSC_VER / 11 ^ _MSC_VER % 16)> {};
    #define yk_c_cmti_wstring_DEFINED
  #endif
}
#endif

#endif

#ifdef yk_c_cm_std_string_STATIC
#undef yk_c_cm_std_string_STATIC
namespace yk_c { bytes::type_index_t<std::string> __inst_cmti_std_string; }
#endif




namespace yk_c
{
    // (!) Compiler bug workaround. For: Solaris v11.1 / CC v5.12.
    //  Explicit pre-instantiation of _select_* avoids compiler crash,
    //  otherwise occuring on autoinst. inside nested test fn. calls.
  void __f_fix1()
  {
    hashx_common::_select_kf<std::string>();
    hashx_common::_select_kf<char*>();

    hashx_common::_select_kf<std::wstring>();
    hashx_common::_select_kf<wchar_t*>();

    hashx_common::_select_skf<std::string, std::string, 1, 1>();
    hashx_common::_select_skf<std::string, char*, 1, 0>();
    hashx_common::_select_skf<char*, std::string, 0, 1>();
    hashx_common::_select_skf<char*, char*, 0, 0>();

    hashx_common::_select_skf<std::wstring, std::wstring, 2, 2>();
    hashx_common::_select_skf<std::wstring, wchar_t*, 2, 0>();
    hashx_common::_select_skf<wchar_t*, std::wstring, 0, 2>();
    hashx_common::_select_skf<wchar_t*, wchar_t*, 0, 0>();
  }
}




namespace yk_tests
{
  struct smptr
  {
    void* p1;
    void* p2;
    void* pz;
    signed long n1;
    signed long n2;
    struct __x;
  };

  struct smptr::__x
  {
    enum
    {
      __s = sizeof(smptr),
      __l4 = sizeof(signed long) == 4, __p4 = sizeof(void*) == 4,
      __l8 = sizeof(signed long) == 8, __p8 = sizeof(void*) == 8,

      // NOTE ksize == 0 yields compile error in  _cm_base_t.
      ksize =
        + 1 * (__p4 && __l4 && __s == 20)
        + 2 * (__p4 && __l8 && __s == 28)
        + 3 * (__p4 && __l8 && __s == 32)
        + 4 * (__p8 && __l4 && __s == 32)
        + 5 * (__p8 && __l8 && __s == 40)
    };
  };
}

  // Several test types for which cross-moduleness is statically resolved.
namespace yk_c
{
  using yk_tests::smptr;

  template<class T, int size>
  struct _resolve_cmt
  {
    typedef T t_target; struct nothing {};

    typedef t_target __temp0;
    typedef typename meta::same_t<__temp0, s_long, nothing, __temp0>::t __temp1;
    typedef typename meta::same_t<__temp1, meta::s_ll, nothing, __temp1>::t __temp2;

    typedef typename meta::if_t<sizeof(t_target) == size, __temp2, nothing>::result t;
  };

  template<> struct bytes::type_index_t<typename _resolve_cmt<int, 4>::t> : cmti_base_t<typename _resolve_cmt<int, 4>::t, 2015, 4, 19, 18> {};
  template<> struct bytes::type_index_t<typename _resolve_cmt<char*, 4>::t> : cmti_base_t<typename _resolve_cmt<char*, 4>::t, 2015, 4, 24, 18> {};
  template<> struct bytes::type_index_t<smptr> : cmti_base_t<smptr, 2015, 5, 3, 12, smptr::__x::ksize> {};
}

namespace yk_tests
{
  using namespace yk_c;

  template<class T, class Conv>
  struct ctf_sl_t
  {
    typedef typename meta::template type_equi<ctf_sl_t, meta::tag_construct>::t_3 __check1;
    typedef typename meta::template type_equi<ctf_sl_t, meta::tag_functor>::t_3 __check2;
    mutable s_long i; ctf_sl_t() : i(-1) {}
    typedef T t; inline void f(t* p) const { new (p) t(Conv::F(i)); }
  };

  struct excgen
  {
    s_long a; s_long b; mutable char flags;

    static s_long cnt() { return __cnt(); }
    static void set_cnt(s_long i) { __cnt() = i - s_long(1); }
    bool f_xc() const { return bool(flags & 0x1); }
    bool f_xd() const { return bool(flags & 0x2); }
    void setf_xc (bool x) const { x ? flags |= 0x1: flags &= ~0x1ul; }
    void setf_xd (bool x) const { x ? flags |= 0x2: flags &= ~0x2ul; }

    excgen() : a(cnt()), b(a), flags(0) {}
    excgen(s_long a_) : a(a_), b(a), flags(0) {}

    struct excgen_c {}; struct excgen_d {};

    excgen(const excgen& x) : a(x.a), b(-1), flags(0) { if (x.f_xc()) { throw excgen_c(); } b = x.b; }
    excgen& operator=(const excgen& x) { a = x.a; b = -1; flags = 0; if (x.f_xc()) { throw excgen_c(); } b = x.b; return *this; }
    ~excgen()
#if __cplusplus >= 201103
      noexcept(false)
#endif
      { a = -1; if (f_xd()) { throw excgen_d(); } b = -1;  }

    bool operator== (const excgen& x) const { return a == x.a && b == x.b; }
    bool operator!= (const excgen& x) const { return !(*this == x); }

    private: static s_long& __cnt() { static s_long x(0); ++x; return x; }
  };

  template<class T> struct excgen_setf_vecm { enum { exists = false }; static inline void F(vecm* pv, s_long ind0, bool xc, bool xd) {} };
  template<> struct excgen_setf_vecm<excgen>
  {
    enum { exists = true };
    static inline void F(vecm* pv, s_long ind0, bool xc, bool xd) { if (!(pv && ind0 >=0 && ind0 < pv->n())) { return; } pv->pval_0u<excgen>(ind0)->setf_xc(xc); pv->pval_0u<excgen>(ind0)->setf_xd(xd); }
  };

  struct excgen_nt {};
  struct excgen_mmode3 {};

  struct S3 { char x[3]; S3() { x[0] = 0; x[1] = 0; x[2] = 0; } S3(yk_c::s_long z) { x[0] = z & 0xff; x[1] = (z >> 8) & 0xff; x[2] = (z >> 16) & 0xff; } bool operator==(const S3& z) const { return x[0] == z.x[0] && x[1] == z.x[1] && x[2] == z.x[2]; } };

}

namespace yk_c
{
  using namespace yk_tests;

  template<class T, class Conv> struct meta::type_equi<ctf_sl_t<T, Conv>, meta::tag_construct> { typedef meta::tag_construct t_3; };
  template<class T, class Conv> struct meta::type_equi<ctf_sl_t<T, Conv>, meta::tag_functor> { typedef meta::tag_functor t_3; };

    // NOTE Under excgen_nt, excgen object is moved using copy constructor. Destination flags  for exception generation are set to 0, even if copying itself generates an exception.
  template<> struct vecm::spec<excgen_nt> { struct aux : vecm::config_aux<excgen> { enum { transactional = false, nt_allowDfltConstruct = false, nt_allowNullify = false }; }; typedef config_t<excgen, 1, 1, 1, aux> config; };
  template<> struct meta::type_equi<excgen_nt, excgen> { typedef excgen t_3; };
  template <class Aux> struct meta::construct_f<excgen_nt, Aux> { typedef excgen t; typedef Aux aux; inline void f(t* p) const { new (p) t(); } };

    // NOTE Under excgen_mmode3, excgen object is bytewise moved, so exception generation flags are moved as is. They should be checked/reset at the destination pos.
  template<> struct vecm::spec<excgen_mmode3> { struct aux : vecm::config_aux<excgen> { typedef excgen_mmode3 ta_meta_safemove; }; typedef config_t<excgen, 1, 3, 1, aux> config; };
  template<> struct meta::type_equi<excgen_mmode3, excgen> { typedef excgen t_3; };
  template<> struct meta::safemove_t<excgen_mmode3> { typedef excgen T; static inline void F(T* pdest, T* psrc) throw() { bytes::memmove_t<T>::F(pdest, psrc, sizeof(T)); } };
  template <class Aux> struct meta::construct_f<excgen_mmode3, Aux> { typedef excgen t; typedef Aux aux; inline void f(t* p) const { new (p) t(); } };
}

namespace yk_tests
{
  using namespace yk_c;

  enum hash_cat0 { hc_hashx = 1, hc_stdmap = 18 };

  template<class H_, class K_, class V_> struct _hmeta_base_t { typedef H_ H; typedef K_ K; typedef V_ V; };

  template<int cat, class K, class V> struct hmeta_t { };

  template<class K, class V> struct hmeta_t<hc_hashx, K, V> : _hmeta_base_t<hashx<K, V>, K, V> { };

  template<class H> struct hconfig { static void F(H& x) {} };
  template<class H> struct hsize { static s_long F(const H& x) { return x.size(); } };
  template<class H> struct hclear { static void F(H& x) { x.clear(); } };
  template<class H, class K> struct hfind { static s_long F(H& x, const K& k) { return s_long(bool(x.find(k))); } }; // !=0 - exists, 0 - not found or error
  template<class H, class K> struct hremove { static s_long F(H& x, const K& k) { x.erase(k); return 1; } };
  template<class H, class K, class V> struct hinsert
  {
    static s_long F(H& x, const K& k, const V& v) // 1 - inserted, 0 - already exists (v assigned only), < 0 - error
    {
      s_long res = hfind<H, K>::F(x, k) ? 0 : 1;
      V* pv(0);
      try { pv = &x[k]; } catch (...) { return -1; }
      try { *pv = v; } catch (...) { return -2; }
      return res;
    }
  };
  template<class H> struct hassign
  {
    static s_long Fassign(H& dest, const H& src) // 1 - on_calm_result (dest is changed), 0 - not supported, < 0 - failure
    {
      try { dest = src; } catch (...) { return -1; } return 1;
    }
    static s_long Fcc(H* pdest, const H& src) // 1 - on_calm_result (dest is initialized), 0 - not supported, < 0 - failure
    {
      if (!pdest) { return -1; }
      try { new (pdest) H(src); }  catch (...) { return -1; } return 1;
    }
  };

  template<class T1, class T2, class T3> struct hsize<hashx<T1, T2, T3> > { static s_long F(const hashx<T1, T2, T3>& x) { return x.n(); } };
  template<class T1, class T2, class T3> struct hclear<hashx<T1, T2, T3> > { static void F(hashx<T1, T2, T3>& x) { x.hashx_clear(); } };
    bool& hconfig_hashx_can_shrink() { static bool x(true); return x; }
  template<class T1, class T2, class T3> struct hconfig<hashx<T1, T2, T3> > { static void F(hashx<T1, T2, T3>& x) { x.hashx_setf_can_shrink(hconfig_hashx_can_shrink()); } };
  template<class T1, class T2, class T3, class K> struct hremove<hashx<T1, T2, T3>, K> { static s_long F(hashx<T1, T2, T3>& x, const K& k) { return x.remove(k); } };
  template<class T1, class T2, class T3, class K, class V> struct hinsert<hashx<T1, T2, T3>, K, V> { static s_long F(hashx<T1, T2, T3>& x, const K& k, const V& v) { s_long i(-1); s_long res = x.insert(k, 0, &i); if (!x(i)) { return -10; } try { x(i)->v = v; } catch (...) { return -11; } return res; } };
  template<class T1, class T2, class T3> struct hassign<hashx<T1, T2, T3> > { typedef hashx<T1, T2, T3> H; static s_long Fassign(H& dest, const H& src) { dest.hashx_copy(src, false); return 1; } static s_long Fcc(H* pdest, const H& src) { new (pdest) H(); return 1; } };


  template<class K, class V> struct hmeta_t<hc_stdmap, K, V> : _hmeta_base_t<std::map<K, V>, K, V> { };
  template<class T1, class T2, class T3, class K> struct hfind<std::map<T1, T2, T3>, K> { static s_long F(std::map<T1, T2, T3>& x, const K& k) { return x.count(k); } };


  // ===================================================
  // Tunable code.
  // ===================================================

  template<class _ = int> struct test_common_t : test_base_t<>
  {
    test_common_t(const char* logfn, bool log_truncate) : base(logfn, log_truncate) { rnd_0() = 0; }
    static const char* c_txtsample_filename() { return "txtsample.txt"; } // 1

    // ===================================================
    // Test-specific functions.
    // ===================================================

    static bool& full_test() { static bool x(true); return x; }

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

    static s_long _rnd_sl() { double x = (rnd() * 2. - 1.) * (double(meta::s_long_mp) + 1.); return s_long(x); }

      // For the below templates:
      // T == vecm: _2 is elem. type.
      // T == hashx: _2 is elem. type when hash key is vecm. _3 is elem. type when hash value is vecm.
      //  NOTE In _conv_sl, _conv_rnd, _rnd_val, _3 is absent.
      //      hash value of type vecm is expected to be constructed automatically,
      //      being specified as hashx<key, meta::construct_f<vecm, elem. type>,... >.
    template<class T, class _2 = meta::nothing> struct _conv_sl { static T F(s_long ind) { return T(ind); } };
    template<class T, class _2> struct _conv_sl<T*, _2> { static T* F(s_long ind) { return reinterpret_cast<T*>(static_cast<char*>(0) + ind); } };
    template<class _2> struct _conv_sl<std::string, _2> { typedef std::string T; static T F(s_long ind) { std::ostringstream ss; ss << ind << '\0'; return ss.str(); } };
    template<class _2> struct _conv_sl<vecm, _2> { static vecm F(s_long ind) { vecm v(typer<_2>, 0); v.el_append(_conv_sl<_2>::F(ind)); return v; } };
    template<class _2> struct _conv_sl<float, _2> { typedef float T; static T F(s_long ind) { s_long abs = ind > 0 ? ind : -ind; s_long sign = ind >= 0 ? (ind > 0 ? 1 : 0) : -1; if (abs < (1 << 22)) { return T(ind); } abs >>= 16; return (sign * T(abs)) * 65536; } };
    template<class HT1, class HT2, class HT3, class _2> struct _conv_sl<hashx<HT1, HT2, HT3>, _2> { typedef hashx<HT1, HT2, HT3> H; static H F(s_long ind) { H h; h.insert(_conv_sl<HT1, _2>::F(ind)); return h; } };

    template<class T, class _2 = meta::nothing> struct _conv_rnd { static T F(double x) { return T(s_long((x * 2. - 1.) * (double(meta::s_long_mp) + 1.))); } };
    template<class _2> struct _conv_rnd<std::string, _2> { typedef std::string T; static T F(double x) { std::ostringstream ss; ss << x << '\0'; return ss.str(); } };
    template<class _2> struct _conv_rnd<bool, _2> { typedef bool T; static T F(double x) { return x < 0.5; } };
    template<class _2> struct _conv_rnd<float, _2> { typedef float T; static T F(double x) { return T(x); } };
    template<class _2> struct _conv_rnd<double, _2> { typedef double T; static T F(double x) { return x; } };
    template<class _2> struct _conv_rnd<long double, _2> { typedef long double T; static T F(double x) { return T(x); } };
    template<class Q, class _2> struct _conv_rnd<Q*, _2> { typedef Q* T; static T F(double x) { return reinterpret_cast<T>(static_cast<char*>(0) + s_long(x * meta::s_long_mp)); } };
    template<class _2> struct _conv_rnd<vecm, _2> { static vecm F(double x) { vecm v(typer<_2>, 0); v.el_append(_conv_rnd<_2>::F(x)); return v; } };
    template<class HT1, class HT2, class HT3, class _2> struct _conv_rnd<hashx<HT1, HT2, HT3>, _2> { typedef hashx<HT1, HT2, HT3> H; static H F(double x) { H h; typedef typename H::t_k t_k;  h.insert(_conv_rnd<t_k, _2>::F(x)); return h; } };

    template<class T, class _2 = meta::nothing> struct _rnd_val { };

    template<class _2> struct _rnd_val<bool, _2> { typedef bool T; static T F() { double x; double* p0 = &x; reinterpret_cast<s_long*>(p0)[0] = _rnd_sl(); reinterpret_cast<s_long*>(p0)[1] = _rnd_sl(); return T(*reinterpret_cast<T*>(p0)); } };
    template<class _2> struct _rnd_val<char, _2> { typedef char T; static T F() { double x; double* p0 = &x; reinterpret_cast<s_long*>(p0)[0] = _rnd_sl(); reinterpret_cast<s_long*>(p0)[1] = _rnd_sl(); return T(*reinterpret_cast<T*>(p0)); } };
    template<class _2> struct _rnd_val<wchar_t, _2> { typedef wchar_t T; static T F() { double x; double* p0 = &x; reinterpret_cast<s_long*>(p0)[0] = _rnd_sl(); reinterpret_cast<s_long*>(p0)[1] = _rnd_sl(); return T(*reinterpret_cast<T*>(p0)); } };
    template<class _2> struct _rnd_val<signed char, _2> { typedef signed char T; static T F() { double x; double* p0 = &x; reinterpret_cast<s_long*>(p0)[0] = _rnd_sl(); reinterpret_cast<s_long*>(p0)[1] = _rnd_sl(); return T(*reinterpret_cast<T*>(p0)); } };
    template<class _2> struct _rnd_val<unsigned char, _2> { typedef unsigned char T; static T F() { double x; double* p0 = &x; reinterpret_cast<s_long*>(p0)[0] = _rnd_sl(); reinterpret_cast<s_long*>(p0)[1] = _rnd_sl(); return T(*reinterpret_cast<T*>(p0)); } };
    template<class _2> struct _rnd_val<signed short int, _2> { typedef signed short int T; static T F() { double x; double* p0 = &x; reinterpret_cast<s_long*>(p0)[0] = _rnd_sl(); reinterpret_cast<s_long*>(p0)[1] = _rnd_sl(); return T(*reinterpret_cast<T*>(p0)); } };
    template<class _2> struct _rnd_val<unsigned short int, _2> { typedef unsigned short int T; static T F() { double x; double* p0 = &x; reinterpret_cast<s_long*>(p0)[0] = _rnd_sl(); reinterpret_cast<s_long*>(p0)[1] = _rnd_sl(); return T(*reinterpret_cast<T*>(p0)); } };
    template<class _2> struct _rnd_val<signed int, _2> { typedef signed int T; static T F() { double x; double* p0 = &x; reinterpret_cast<s_long*>(p0)[0] = _rnd_sl(); reinterpret_cast<s_long*>(p0)[1] = _rnd_sl(); return T(*reinterpret_cast<T*>(p0)); } };
    template<class _2> struct _rnd_val<unsigned int, _2> { typedef unsigned int T; static T F() { double x; double* p0 = &x; reinterpret_cast<s_long*>(p0)[0] = _rnd_sl(); reinterpret_cast<s_long*>(p0)[1] = _rnd_sl(); return T(*reinterpret_cast<T*>(p0)); } };
    template<class _2> struct _rnd_val<signed long int, _2> { typedef signed long int T; static T F() { double x; double* p0 = &x; reinterpret_cast<s_long*>(p0)[0] = _rnd_sl(); reinterpret_cast<s_long*>(p0)[1] = _rnd_sl(); return T(*reinterpret_cast<T*>(p0)); } };
    template<class _2> struct _rnd_val<unsigned long int, _2> { typedef unsigned long int T; static T F() { double x; double* p0 = &x; reinterpret_cast<s_long*>(p0)[0] = _rnd_sl(); reinterpret_cast<s_long*>(p0)[1] = _rnd_sl(); return T(*reinterpret_cast<T*>(p0)); } };
    template<class _2> struct _rnd_val<signed long long int, _2> { typedef signed long long int T; static T F() { double x; double* p0 = &x; reinterpret_cast<s_long*>(p0)[0] = _rnd_sl(); reinterpret_cast<s_long*>(p0)[1] = _rnd_sl(); return T(*reinterpret_cast<T*>(p0)); } };
    template<class _2> struct _rnd_val<unsigned long long int, _2> { typedef unsigned long long int T; static T F() { double x; double* p0 = &x; reinterpret_cast<s_long*>(p0)[0] = _rnd_sl(); reinterpret_cast<s_long*>(p0)[1] = _rnd_sl(); return T(*reinterpret_cast<T*>(p0)); } };

    template<class _2> struct _rnd_val<float, _2> { typedef float T; static T F() { return (rnd() - 0.5) * 1.e10; } };
    template<class _2> struct _rnd_val<double, _2> { typedef double T; static T F() { return (rnd() - 0.5) * 1.e12; } };
    template<class _2> struct _rnd_val<long double, _2> { typedef long double T; static T F() { return (rnd() - 0.5) * 1.e15; } };

    template<class _2> struct _rnd_val<excgen, _2> { typedef excgen T; static T F() { return _rnd_sl(); } };
    template<class _2> struct _rnd_val<S3, _2> { typedef S3 T; static T F() { return _rnd_sl(); } };

    template<class _2> struct _rnd_val<std::string, _2> { typedef std::string T; static T F() { std::ostringstream ss; ss << _rnd_sl() << ' ' << _rnd_sl() << '\0'; return ss.str(); } };
    template<class Q, class _2> struct _rnd_val<Q*, _2> { typedef Q* T; static T F() { struct S { char a[0x10000]; }; return reinterpret_cast<T>(static_cast<S*>(0) + _rnd_sl()); } };
    template<class _2> struct _rnd_val<vecm, _2> { typedef typename meta::same_t<_2, vecm, s_long, meta::nothing>::t T2; static vecm F() { vecm v(typer<_2>, 0); v.el_append(_conv_sl<_2, T2>::F(_rnd_sl())); return v; } };
    template<class HT1, class HT2, class HT3, class _2> struct _rnd_val<hashx<HT1, HT2, HT3>, _2> { typedef hashx<HT1, HT2, HT3> H; typedef typename H::t_k t_k; static H F() { H h; h.insert(_rnd_val<t_k, _2>::F()); return h; } };

      // T == vecm: _2 is elem. type.
      // T == hashx: _2 is elem. type when hash key is vecm, _3 is elem. type when hash value is vecm.
      //  NOTE hashx: if value type == vecm, _3 must be == element type. It's not extracted automatically from hash class.
    template<class T, class _2 = meta::nothing, class _3 = meta::nothing> struct _eq { static s_long F(const T& x1, const T& x2) { try { return s_long(x1 == x2); } catch (...) { return -1; } } };
    template<class _3> struct _eq<vecm, meta::nothing, _3>
    {
      static s_long F(const vecm& x1, const vecm& x2)
      {
        if (x1.ptd()->t_ind == bytes::type_index_t<bool>::xind) { return _eq<vecm, bool>::F(x1, x2); }
        if (x1.ptd()->t_ind == bytes::type_index_t<char>::xind) { return _eq<vecm, char>::F(x1, x2); }
        if (x1.ptd()->t_ind == bytes::type_index_t<wchar_t>::xind) { return _eq<vecm, wchar_t>::F(x1, x2); }
        if (x1.ptd()->t_ind == bytes::type_index_t<signed char>::xind) { return _eq<vecm, signed char>::F(x1, x2); }
        if (x1.ptd()->t_ind == bytes::type_index_t<unsigned char>::xind) { return _eq<vecm, unsigned char>::F(x1, x2); }
        if (x1.ptd()->t_ind == bytes::type_index_t<signed short int>::xind) { return _eq<vecm, signed short int>::F(x1, x2); }
        if (x1.ptd()->t_ind == bytes::type_index_t<unsigned short int>::xind) { return _eq<vecm, unsigned short int>::F(x1, x2); }
        if (x1.ptd()->t_ind == bytes::type_index_t<signed int>::xind) { return _eq<vecm, signed int>::F(x1, x2); }
        if (x1.ptd()->t_ind == bytes::type_index_t<unsigned int>::xind) { return _eq<vecm, unsigned int>::F(x1, x2); }
        if (x1.ptd()->t_ind == bytes::type_index_t<signed long int>::xind) { return _eq<vecm, signed long int>::F(x1, x2); }
        if (x1.ptd()->t_ind == bytes::type_index_t<unsigned long int>::xind) { return _eq<vecm, unsigned long int>::F(x1, x2); }
        if (x1.ptd()->t_ind == bytes::type_index_t<signed long long int>::xind) { return _eq<vecm, signed long long int>::F(x1, x2); }
        if (x1.ptd()->t_ind == bytes::type_index_t<unsigned long long int>::xind) { return _eq<vecm, unsigned long long int>::F(x1, x2); }
        if (x1.ptd()->t_ind == bytes::type_index_t<float>::xind) { return _eq<vecm, float>::F(x1, x2); }
        if (x1.ptd()->t_ind == bytes::type_index_t<double>::xind) { return _eq<vecm, double>::F(x1, x2); }
        if (x1.ptd()->t_ind == bytes::type_index_t<long double>::xind) { return _eq<vecm, long double>::F(x1, x2); }
        if (x1.ptd()->is_ptr) { return _eq<vecm, void*>::F(x1, x2); }
        if (x1.ptd()->t_ind == bytes::type_index_t<std::string>::xind) { return _eq<vecm, std::string>::F(x1, x2); }
        return -1;
      }
    };
    template<class _2, class _3> struct _eq<vecm, _2, _3>
    {
      static s_long F(const vecm& x1, const vecm& x2)
      {
        if (x1.ptd()->is_ptr) { if (!meta::is_ptr<_2>::result) { return -1; } if (x1.ptd()->t_ind != x2.ptd()->t_ind) { return -1; } }
          else { if (x1.ptd()->t_ind != bytes::type_index_t<_2>::xind || x2.ptd()->t_ind != bytes::type_index_t<_2>::xind) { return -1; } }
        if (x1.n() != x2.n()) { return 0; }
        if (x1.nbase() != x2.nbase()) { return 0; }
        for (s_long i = 0; i < x1.n(); ++i) { s_long res = _eq<_2>::F(*x1.pval_0u<_2>(i), *x2.pval_0u<_2>(i)); if (res != 1) { return res; } }
        return 1;
      }
    };
    template<class HT1, class HT2, class HT3, class _2, class _3> struct _eq<hashx<HT1, HT2, HT3>, _2, _3>
    {
      typedef hashx<HT1, HT2, HT3> H; typedef typename H::t_k t_k; typedef typename H::t_v t_v;
      static s_long F(const H& x1, const H& x2)
      {
        if (x1.integrity() < 0 || x2.integrity() < 0) { return -1; }
        if (x1.n() != x2.n()) { return 0; }
        for (s_long i = 0; i < x1.n(); ++i) { s_long res1 = _eq<t_k, _2>::F(x1(i)->k, x2(i)->k); s_long res2 = _eq<t_v, _3>::F(x1(i)->v, x2(i)->v); if (res2 < res1) { res1 = res2; } if (res1 != 1) { return res1; } }
        return 1;
      }
    };
    template<class T, class _2 = meta::nothing> struct _assign { static void F(T& dest, const T& src) { dest = src; } };
    template<class _2> struct _assign<vecm, _2> { static void F(vecm& dest, const vecm& src) { dest.vecm_copy(src, false); } };
    template<class HT1, class HT2, class HT3, class _2> struct _assign<hashx<HT1, HT2, HT3>, _2> { typedef hashx<HT1, HT2, HT3> H; static void F(H& dest, const H& src) { dest.hashx_copy(src, false); } };

      // retval(ind, err):
      //  The resulting value is negative or 0 (on ind == 0 and err == 0).
      //    Accessible via .res and operator long.
      //  ind will occupy decimal digits from 4th to max.
      //    ind is treated as unsigned 32-bit value.
      //    For ind > 2^16, 1e5*(log2(ind)-16) will be set.
      //  err will occupy 3 lower decimal digits. Positive err is negated.
    struct _retval { s_long res; _retval(s_long ind, s_long err) : res((err <= 0 ? err : -err) + (bytes::ilog2(ind) - 16 <= 0 ? -1000 * ind : -1000 * 100000 * (bytes::ilog2(ind) - 16))) {} operator long () const { return res; } };

        // _echo::F returns const ref its argument if Ts == Td. Otherwise, returns a default value of Td.
        //    Allows compiling type-dependent conditional branches with any type.
      template<class Ts, class Td> struct _echo { static const Td& F(const Ts&) { static __test_common_aux1::storage_t<Td> x(1); return x; } static Td& F(Ts&) { static __test_common_aux1::storage_t<Td> x(1); return x; } };
        template<class T> struct _echo<T, T> { static const T& F(const T& x) { return x; } static T& F(T& x) { return x; } };

    struct __vecm_x : vecm
    {
        // NOTE ind0 may be >= -1, i.e. _cap(n() - 1) always returns correct results.
      static s_long _cap(s_long ind0) { if (ind0 < 0) { return  m::c1; } s_long j, k, cap; m::_ind_jk(ind0, j, k, cap); return cap; }
      static inline void _ind_jk(s_long ind0, s_long& rj, s_long& rk, s_long& rcap) { m::_ind_jk(ind0, rj, rk, rcap); }
      static inline s_long _nrsv(s_long nj) { return m::_nrsv(nj); }
      static inline s_long _nrsv_n(s_long n) { if (n < 0) { return 0; } if (n == 0) { return m::c1; } s_long j, k; m::_ind_jk(n - 1, j, k); return m::_nrsv(j + 1); }
      static bool nr_ok(vecm* pv) { return nr_ok(pv->n(), pv->nrsv()); }
      static bool nr_ok(s_long nr_req, s_long nr_act) { s_long n = nr_act - nr_req; return n >= 0 && (n < _cap(nr_req - 1) || (nr_req == 0 && n == _cap(nr_req - 1))); }
      template<class T> void forced_clear(s_long n) { for (s_long i = 0; i < n; ++i) { ptd()->p_destroy_1(pval_0u<T>(i)); } _n = 0; _last_j = 0; _size_k = 0; }
    };

    struct htest_wrapper
    {
        // Op. []/remove/find/clear validity test for different kinds of maps.
        //  nmax >= 0 is exact max. number of entries on hash growing.
        //    nmax < 0 is not valid.
        //  ntests must be >= 1.
        //  NOTE any excgen as key is treated in special in comp. to types.
        //    (Exception generation is taken into account and additionally tested.)
      template<int cat, class KA, class VA> struct htest_any_t
      {
        static int F(logger& log, const char* which, s_long nmax, s_long ntests)
        {
          log.g_ecg_data, "Validity", which, "n nt", nmax, ntests, typeid(KA).name(), typeid(VA).name()
          ; log.flush();

          typedef typename vecm::specf<KA>::t_value t_k;
          typedef typename vecm::specf<VA>::t_value t_v;
          // typedef typename meta::if_t<cat == hc_hashx, KA, t_k>::result t_ka;
          // typedef typename meta::if_t<cat == hc_hashx, VA, t_v>::result t_va;
          typedef typename hmeta_t<cat, t_k, t_v>::H t_h;

          s_long q = 0;
          try
          {
            t_h h;

            if (nmax < 0) { return -1; }
            if (ntests <= 0) { return -2; }

            if (nmax == 0) { return 1; }

            try { hconfig<t_h>::F(h); } catch (...) { return -3; }

            enum { NF = 6, NFIX = 10 };
            while (q < ntests)
            {
              bool __skip = false;
              bool __cancelled = false;
              bool __size_unchanged = false;
              s_long m = 0;
              s_long n = 0;
              s_long __n = hsize<t_h>::F(h);
              if (__n < 0) { return _retval(q, -100); }
              double p_grow = 1.; if (__n >= 0.9 * nmax) { p_grow = 0.5; } if (__n == nmax) { p_grow = 0.; }
              double p_shrink = 1.; if (__n < 0.1 * nmax) { p_shrink = 0.5; } if (__n == 0) { p_shrink = 0.; }
              s_long ind_mf = rnd() * NF;
              switch (ind_mf)
              {
                case 0: // clear, find
                {
                  try
                  {
                    if (rnd() >= p_shrink) { __skip = true; break; }
                    if (meta::same_t<t_k, excgen>::result && hfind<t_h, t_k>::F(h, _conv_sl<t_k>::F(1)) && rnd() < 0.5) { _echo<t_v, excgen>::F(h[_conv_sl<t_k>::F(1)]).setf_xd(true); }
                    try { hclear<t_h>::F(h); } catch (...) { if (hsize<t_h>::F(h) == 0) { return _retval(q, -10); } else if (hsize<t_h>::F(h) == __n) { return _retval(q, -11); } else { return _retval(q, -12); } }
                    if (hsize<t_h>::F(h) != 0) { return _retval(q, -15); }
                  }
                  catch (...) { return _retval(q, -99); }
                  break;
                }
                case 1: // operator [ ], find
                {
                  try
                  {
                    if (rnd() >= p_grow) { __skip = true; break; }
                    m = (nmax - __n) * rnd() / 3. + 1; if (rnd() < 0.1) { m = nmax - __n; }
                    n = __n + m;
                    s_long m2 = 0; s_long m3 = 0;
                    while (hsize<t_h>::F(h) < n && m2 < m)
                    {
                      s_long z = rnd() < 0.95 ? -1 : rnd() * NFIX;
                        if (z >= 2 && meta::same_t<t_k, bool>::result) { z = 1; }
                        t_k k(z < 0 ? _rnd_val<t_k>::F() : _conv_sl<t_k>::F(z));
                        if (z < 0) { for(s_long f = 0; f < NFIX; ++f) { if (_eq<t_k>::F(k, _conv_sl<t_k>::F(f))) { z = f; break; } } }
                      const t_v v(_conv_sl<t_v>::F(z));
                      bool b = hfind<t_h, t_k>::F(h, k);
                      t_v* pv = 0;
                      try { pv = &h[k]; }
                        catch (...)
                        {
                          if (hsize<t_h>::F(h) != __n + m3) { return _retval(q, -100); }
                          return _retval(q, -101);
                        }
                        if (b && z >= 0)
                        {
                          if (!_eq<t_v>::F(*pv, v)) { return _retval(q, -103); }
                        }
                      try { *pv = v; } catch (...) { return _retval(q, -104); }
                        if (b)
                        {
                          ++m2;
                          if (hsize<t_h>::F(h) != __n + m3) { return _retval(q, -101); }
                        }
                        else
                        {
                          ++m3;
                          if (hsize<t_h>::F(h) != __n + m3) { return _retval(q, -102); }
                        }
                    }
                    if (hsize<t_h>::F(h) == __n) { __size_unchanged = true; }
                  }
                  catch (...) { return _retval(q, -199); }
                  break;
                }
                case 2: // remove, find
                {
                  if (rnd() >= p_shrink) { __skip = true; break; }
                  m = __n * rnd() / 3. + 1; if (rnd() < 0.1) { m = __n; }
                  n = __n - m;
                  s_long m2 = 0; s_long m3 = 0;
                  while (hsize<t_h>::F(h) > n && m2 < m)
                  {
                    s_long z = rnd() < 0.95 ? -1 : rnd() * NFIX;
                      if (z >= 2 && meta::same_t<t_k, bool>::result) { z = 1; }
                      t_k k(z < 0 ? _rnd_val<t_k>::F() : _conv_sl<t_k>::F(z));
                    bool b = hfind<t_h, t_k>::F(h, k);
                    try { hremove<t_h, t_k>::F(h, k); }
                    catch (...)
                    {
                      if (hsize<t_h>::F(h) != __n - m3) { return _retval(q, -200); }
                      return _retval(q, -201);
                    }
                    if (hfind<t_h, t_k>::F(h, k)) { return _retval(q, -202); }
                    if (b)
                    {
                      ++m3;
                      if (hsize<t_h>::F(h) != __n - m3) { return _retval(q, -204); }
                    }
                    else
                    {
                      ++m2;
                      if (hsize<t_h>::F(h) != __n - m3) { return _retval(q, -203); }
                    }
                  }
                  if (hsize<t_h>::F(h) == __n) { __size_unchanged = true; }
                  break;
                }
                case 3: // copy constructor
                {
                  try
                  {
                    __test_common_aux1::storage_t<t_h> h2_(0);
                    s_long res = hassign<t_h>::Fcc(h2_, h); if (res == 0) { __cancelled = true; break; }
                    if (res != 1) { return _retval(q, -302); }
                    h2_.inited = true; t_h& h2(h2_);
                    if (hsize<t_h>::F(h2) != __n) { return _retval(q, -300); }
                    for(s_long f = 0; f < NFIX; ++f)
                    {
                      t_k k(_conv_sl<t_k>::F(f));
                      if (hfind<t_h, t_k>::F(h, k) != hfind<t_h, t_k>::F(h2, k)) { return _retval(q, -301); }
                    }
                    __size_unchanged = true;
                  }
                  catch (...) { return _retval(q, -319); }
                  break;
                }
                case 4: // assignment
                {
                  try
                  {
                    t_h h2;
                    s_long res = hassign<t_h>::Fassign(h2, h); if (res == 0) { __cancelled = true; break; }
                    if (res != 1) { return _retval(q, -322); }
                    if (hsize<t_h>::F(h2) != __n) { return _retval(q, -320); }
                    for(s_long f = 0; f < NFIX; ++f)
                    {
                      t_k k(_conv_sl<t_k>::F(f));
                      if (hfind<t_h, t_k>::F(h, k) != hfind<t_h, t_k>::F(h2, k)) { return _retval(q, -321); }
                    }
                    __size_unchanged = true;
                  }
                  catch (...) { return _retval(q, -339); }
                  break;
                }
                case 5: // insert, find
                {
                  try
                  {
                    if (rnd() >= p_grow) { __skip = true; break; }
                    m = (nmax - __n) * rnd() / 3. + 1; if (rnd() < 0.1) { m = nmax - __n; }
                    n = __n + m;
                    s_long m2 = 0; s_long m3 = 0;
                    while (hsize<t_h>::F(h) < n && m2 < m)
                    {
                      s_long z = rnd() < 0.95 ? 0 : 1 + rnd() * NFIX;
                        if (z >= 2 && meta::same_t<t_k, bool>::result) { z = 1; }
                        t_k k(z <= 0 ? _rnd_val<t_k>::F() : _conv_sl<t_k>::F(z));
                        if (z <= 0) { for(s_long f = 0; f < NFIX; ++f) { if (_eq<t_k>::F(k, _conv_sl<t_k>::F(f))) { z = f; break; } } }
                      const t_v v(_conv_sl<t_v>::F(z));
                      bool b = hfind<t_h, t_k>::F(h, k);
                      try
                      {
                        s_long res = hinsert<t_h, t_k, t_v>::F(h, k, v);
                        if (!((!b && res == 1) || (b && res == 0))) { return _retval(q, -340); }
                        if (b)
                        {
                          ++m2;
                          if (hsize<t_h>::F(h) != __n + m3) { return _retval(q, -344); }
                        }
                        else
                        {
                          ++m3;
                          if (hsize<t_h>::F(h) != __n + m3) { return _retval(q, -345); }
                        }
                      }
                      catch (...)
                      {
                        if (hsize<t_h>::F(h) != __n + m3) { return _retval(q, -341); }
                        return _retval(q, -101);
                      }
                    }
                    if (hsize<t_h>::F(h) == __n) { __size_unchanged = true; }
                  }
                  catch (...) { return _retval(q, -359); }
                  break;
                }
                default: { return -299; }
              }
              if (__skip) { continue; }

              if (!__cancelled)
              {
                if (hsize<t_h>::F(h) == __n)
                {
                  if (!__size_unchanged) { return _retval(q, -900 - ind_mf); }
                }
              }

              ++q;
            }
          } catch (...) { return _retval(q, -8); }
          return 1;
        }
      };

      template<class H, class K, class V> struct _hperf_any_t
      {
        static int F(yk_tests::logger& log, bool is_prog, bool do_peak, const vecm& _ks, const vecm& _vs, const std::string& which)
        {
          s_long N = _ks.n();
          if (N == 0) { log.d, "_hperf_any_t::F: N == 0"; return -1; }

          double t0, t1b, t1e; double dt1 = 0.; double dt2 = 0.; double dt3 = 0.; double dt4 = 0.; double dt5 = 0.;
          long n1(0), n2(0), n3(0), ind4(0), ind5(0);
          const long n_findps = 6; const long n_median = 5;
          s_long ind; s_long size_full(0); s_long size_empty(0); s_long size_max_peak(0);
          double clk_res = __timergap(msclk, 500, false);
          bool can_measure_peak(clk_res <= 2.e-3);

          long nt = 0; long n = N; long i = 0;
          const long _n1 = full_test() ? 1000000 : 300000;
          const long _n2 = N < _n1 ? _n1 : N;
          try
          {
            do
            {
              long n_obj = _n2 / n;
              std::vector<char> hh0(n_obj * sizeof(H));
              H* hh = (H*)&hh0[0]; for (long q0 = 0; q0 < n_obj; ++q0) { new (&hh[q0]) H; hconfig<H>::F(hh[q0]); }

              t1b = bmdx::clock_ms();
                ind = 0;
                for (long q = 0; q < n_obj; ++q)
                {
                  H& h = hh[q];
                  for (i = 0; i < n; ++i)
                  {
                    h[*_ks.pval_0u<K>(ind)] = *_vs.pval_0u<V>(ind);
                    ++ind; ind %= N;
                  }
                  if (nt == 0 && q * n < N) { size_full += hsize<H>::F(h); }
                }
              t1e = bmdx::clock_ms(); dt1 += (t1e - t1b) * 1e6; n1 += n * n_obj;

              t1b = bmdx::clock_ms();
                for (long q2 = 0; q2 < n_findps; ++q2)
                {
                  ind = 0;
                  for (long q = 0; q < n_obj; ++q)
                  {
                    H& h = hh[q];
                    for (i = 0; i < n; ++i)
                    {
                      __consume(hfind<H, K>::F(h, *_ks.pval_0u<K>(ind)));
                      ++ind; ind %= N;
                    }
                  }
                }
              t1e = bmdx::clock_ms(); dt2 += (t1e - t1b) * 1e6; n2 += n * n_obj * n_findps;

              t1b = bmdx::clock_ms();
                ind = 0;
                for (long q = 0; q < n_obj; ++q)
                {
                  H& h = hh[q];
                  for (i = 0; i < n; ++i)
                  {
                    hremove<H, K>::F(h, *_ks.pval_0u<K>(ind));
                    ++ind; ind %= N;
                  }
                  if (nt == 0 && q * n < N) { size_empty += hsize<H>::F(h); }
                }
              t1e = bmdx::clock_ms(); dt3 += (t1e - t1b) * 1e6; n3 += n * n_obj;

              ++nt; n /= 10;

              for (long q0 = 0; q0 < n_obj; ++q0) { hh[q0].~H(); }

            } while (is_prog && n >= 1000 && N / n <= 1000);

            n = N; if (n > _n1) { n = _n1; }
            size_max_peak = n;
            if (can_measure_peak && do_peak)
            {
              std::vector<double> durs(n * n_median * 2);
              long n_obj = 1;

              std::vector<char> hh0(n_obj * sizeof(H));
              H* hh = (H*)&hh0[0];

              if (true)
              {
                const long q = 0; ind = 0;

                for (long j= 0; j < n_median; ++j)
                {
                  for (long q0 = 0; q0 < n_obj; ++q0) { new (&hh[q0]) H; hconfig<H>::F(hh[q0]); }

                  H& h = hh[q];

                  for (i = 0; i < n; ++i)
                  {
                    t0 = bmdx::clock_ms();
                    while (true) { t1b = bmdx::clock_ms(); if (t1b > t0) { break; } }
                    h[*_ks.pval_0u<K>(ind)] = *_vs.pval_0u<V>(ind);
                    t1e = bmdx::clock_ms();
                    durs[i * n_median + j] = t1e - t1b;
                    ++ind;

                  }
                  ind -= n;

                  for (i = 0; i < n; ++i)
                  {
                    t0 = bmdx::clock_ms();
                    while (true) { t1b = bmdx::clock_ms(); if (t1b > t0) { break; } }
                    hremove<H, K>::F(h, *_ks.pval_0u<K>(ind));
                    t1e = bmdx::clock_ms();
                    durs[n * n_median + i * n_median + j] = t1e - t1b;
                    ++ind;
                  }
                  if (ind + n > N) { ind = 0; }

                  for (long q0 = 0; q0 < n_obj; ++q0) { hh[q0].~H(); }

                }

                double pk1(0.), pk2(0.), pk;
                for (i = 0; i < n; ++i)
                {
                  long ind;
                  ind = i * n_median;
                    std::sort(durs.begin() + ind, durs.begin() + (ind + n_median));
                    pk = *(durs.begin() + ind + n_median / 2);
                    if (pk > pk1) { pk1 = pk; ind4 = i; }
                  ind = n * n_median + i * n_median;
                    std::sort(durs.begin() + ind, durs.begin() + (ind + n_median));
                    pk = *(durs.begin() + ind + n_median / 2);
                    if (pk > pk2) { pk2 = pk; ind5 = i; }
                }
                dt4 = pk1 * 1e6; dt5 = pk2 * 1e6;
              }
            }

            dt1 /= n1; dt2 /= n2; dt3 /= n3; dt4 /= 1; dt5 /= 1;
            if (dt1 > 10.) { dt1 = int(dt1); } if (dt2 > 10.) { dt2 = int(dt2); } if (dt3 > 10.) { dt3 = int(dt3); } if (dt4 > 10.) { dt4 = int(dt4); } if (dt5 > 10.) { dt5 = int(dt5); }

            if (can_measure_peak && do_peak)
            {
              log.d, which, "n ins, uniq, rmv left, ins for peak:"
                 , N , size_full, size_empty, size_max_peak
                 , "; avg ins, find, rmv:", dt1, dt2, dt3, "ns/1; peak ins, rmv:", dt4, "ns at", ind4, ",", dt5, "ns at", ind5;
            }
            else
            {
              if (!can_measure_peak && do_peak)
              {
                log.d, which, "n ins, uniq, rmv left:"
                   , N , size_full, size_empty
                   , "; avg ins, find, rmv:", dt1, dt2, dt3, "ns/1; (!) No peak meas., clk. res.:", clk_res * 1000., "mcs, req.: 2 mcs.";
              }
              else
              {
                log.d, which, "n ins, uniq, rmv left:"
                   , N , size_full, size_empty
                   , "; avg ins, find, rmv:", dt1, dt2, dt3, "ns/1";
              }
            }
            return size_empty == 0 ? 1 : -3;
          } catch (...)
          {
            log.d, which, "C++ exception at i of n:", i, n, "(during ins/rem).";
          }
          return -2;
        }
      };

      template<int cat, class K, class V, s_long N> struct hperf_t
      {
        typedef typename hmeta_t<cat, K, V>::H H;
        static int F(yk_tests::logger& log, bool is_prog, bool do_peak, const char* which)
        {
          rnd_0() = 0;
          vecm _ks(typer<K>, 0); vecm _vs(typer<V>, 0);
          s_long x = s_long(bmdx::clock_ms());
          for (int i = 0; i < N; ++i)
          {
            _ks.el_append<K>(_conv_rnd<K>::F(rnd()));
            _vs.el_append<V>(i + x);
          }
          return _hperf_any_t<H, K, V>::F(log, is_prog, do_peak, _ks, _vs, which);
        }
      };
      template<int cat, class V, s_long N> struct hperf_t<cat, std::string, V, N>
      {
        typedef std::string K; typedef typename hmeta_t<cat, K, V>::H H;
        static int F(yk_tests::logger& log, bool is_prog, bool do_peak, const char* which)
        {
          typedef std::string K;
          rnd_0() = 0;
          std::string& stxt = txtsample(); s_long M = 900000; s_long Mn = 120; if ( ! (s_long(stxt.size()) > M + 2*Mn + 0x10000) ) { log.d, "hperf_t<string>::F: invalid dictionary file (req.:", c_txtsample_filename(), ">= 1 MB)."; return -1; }

          vecm _ks(typer<K>, 0); vecm _vs(typer<V>, 0);
          for (s_long j = 0; j < 10; ++j)
          {
            s_long pos = s_long(M * rnd()); s_long size = (Mn/10) + s_long((Mn*9/10) * rnd());
            for (int i = 0; i < N / 10; ++i)
            {
              if ((i & 0xffff) == 0) { pos = s_long(M * rnd()); size = (Mn/10) + s_long((Mn*9/10) * rnd()); }
              std::string s = stxt.substr(pos + (i & 0xffff), size);
              _ks.el_append<K>(s);
              _vs.el_append<V>(_conv_sl<V>::F(i));
            }
          }
          return _hperf_any_t<H, K, V>::F(log, is_prog, do_peak, _ks, _vs, which);
        }
      };

      template<class H>
      struct _HP : H
      {
        typedef typename _HP::t_k t_k;
        bool _profile_chains(vecm& res)
        {
          res.vecm_clear(); res.vecm_set_nbase(0);
          for (s_long i = 0; i < this->_ht.n(); ++i)
          {
            if (this->_rhdir && i >= this->_base && (i & this->_mask) >= this->_rhind) { continue; }
            s_long nch(0);
            _yk_reg s_long q = *this->_ht.template pval_0u<s_long>(i);
            while (q != _HP::no_elem) { ++nch; q = this->template pval_0u<typename _HP::entry>(q)->q; }
            if (nch >= res.n()) { if (res.el_append_m<s_long>(nch + 1 - res.n(), 0) <= 0) { return false; } }
            ++*res.pval_0u<s_long>(nch);
          }
          return true;
        }
        static int Fprofile_chains(yk_tests::logger& log, const vecm& _ks, const std::string& which_k, const std::string& which_v, bool is_sequential)
        {
          _HP h;
          h.hashx_clear();
          bool cancel(false);
          for (s_long i = 0; i < _ks.n(); ++i) { if (h.insert(*_ks.pval_0u<t_k>(i)) < 0) { cancel = true; break; } }
          vecm res(typer<s_long>, 0);
          if (!cancel) { cancel = !h._profile_chains(res); }
          if (cancel) { log.d, "err. profiling chains for hashx<", which_k, ",", which_v, "; N:", _ks.n(); return -900; }
          log, "hashx<", which_k, ",", which_v, "> "; log.d, (is_sequential ? "seq." : "rnd."), "N", _ks.n();
          log.nsp += 2;
            log, "\t"; for (s_long x = 0; x < 10; ++x) { log, "  +"; log, x; } log, endl;
            for (s_long x = 0; x < res.n(); ++x) { if (x % 10 == 0) { log, s_long(x / 10) * 10, "\t"; } s_long f = bytes::ilog2(*res.pval_0u<s_long>(x)); if (f >= 0) { log, pad(f, 4), f; } else { log, "    "; } if (x % 10 == 9 || x == res.n() - 1) { log, endl; } }
          log.nsp -= 2;
          log.flush();
          return 1;
        }
      };

      template<class K, int N> struct hxprofch_t
      {
        typedef _HP<hashx<K, char> > t_hp; typedef typename t_hp::t_k t_k;
        static int F(yk_tests::logger& log, const std::string& which_k, bool is_sequential)
        {
          vecm _ks(typer<t_k>, 0);
          if (is_sequential) { for (int i = 0; i < N; ++i) { _ks.el_append<t_k>(_conv_sl<t_k>::F(i)); } }
            else { for (int i = 0; i < N; ++i) { _ks.el_append<t_k>(_conv_rnd<t_k>::F(rnd())); } }
          return t_hp::Fprofile_chains(log, _ks, which_k, "char", is_sequential);
        }
      };
      template<int N> struct hxprofch_t<std::string, N>
      {
        typedef _HP<hashx<std::string, char> > t_hp; typedef typename t_hp::t_k t_k;
        static int F(yk_tests::logger& log, const std::string& which_k, bool is_sequential)
        {
          std::string& stxt = txtsample(); s_long M = 900000; s_long Mn = 120; if ( ! (s_long(stxt.size()) > M + 2*Mn + 0x10000) ) { log.d, "hxprofch_t<string>::F: invalid dictionary file (req.:", c_txtsample_filename(), ">= 1 MB)."; return -1; }
          vecm _ks(typer<t_k>, 0);
          if (is_sequential) { for (int i = 0; i < N; ++i) { _ks.el_append<t_k>(_conv_sl<t_k>::F(i)); } }
          else
          {
            for (s_long j = 0; j < 10; ++j)
            {
              s_long pos = s_long(M * rnd()); s_long size = (Mn/10) + s_long((Mn*9/10) * rnd());
              for (int i = 0; i < N / 10; ++i)
              {
                if ((i & 0xffff) == 0) { pos = s_long(M * rnd()); size = (Mn/10) + s_long((Mn*9/10) * rnd()); }
                std::string s = stxt.substr(pos + (i & 0xffff), size);
                _ks.el_append<t_k>(s);
              }
            }
          }
          return t_hp::Fprofile_chains(log, _ks, which_k, "char", is_sequential);
        }
      };

    }; // end htest_wrapper

    int perf_timer()
    {
      log, "Factual std::clock resolution - peak, median (ms): ", __timergap(msclk, 1000, true), ", ", __timergap(msclk, 1000, false), endl;
      return 1;
    }

  }; // end test_common_t

}

#endif

#endif
