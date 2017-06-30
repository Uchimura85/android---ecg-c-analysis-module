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

// bmdx_main.h, bmdx_main.cpp: polytype value, message dispatcher, utilities.

// Features:
//
//    Polytype value, for passing high-level data structures across binary modules.    (bmdx::  struct unity)
//
//    Simple date class.    (bmdx::  struct _unitydate)
//    Configuration file format with support of trees, + encoding/decoding utility.    (bmdx::  struct paramline)
//    Utilities for files and directories.    (bmdx::  struct file_utils)
//    Access to command line arguments array, executable name/path etc.    (bmdx::  cmd_myexe, cmd_string, cmd_array)
//    String character set conversions.    (bmdx::  wsToBs, bsToWs etc.)
//    String manipulation and other utility functions.    (bmdx::  split, trim, replace, array etc.)
//
//    Asynchronous message dispatcher.    (bmdx::  dispatcher_mt)


#ifndef bmdx_main_H
#define bmdx_main_H

#if defined(__clang__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wunknown-pragmas"
#endif
#if defined(__GNUC__)
  #pragma GCC diagnostic ignored "-Wpragmas"
  #pragma GCC diagnostic ignored "-Wundefined-bool-conversion"
#endif

#include "bmdx_config.h"
#include "yk_cx.h"


//**************************************************************
// BRIEF INTRO to struct unity
//**************************************************************

// CONSTRUCTION

  // unity() - constructs an empty object (utype() == utEmpty)
  // unity(<scalar value>) - constructs a scalar value; input value may be converted to one of the supported types
  // unity(ref std::vector<scalar value>) - constructs an array of values
  // unity(<object type>&, bool isStrongRef) - constructs an object reference; see also set_obj below

  // clear() - sets utEmpty, all internally handled objects are freed, storage cleared

// FUNCTION FAMILIES

  // pval - pointer to value or array elem.
  // ref - ref. to value or array elem.
  // rx - pointer to value or array elem. with autoconversion to the specified type
  // val - value copy to the specified type
  // cref - ref. to value if it is of the specified type, othrwise to temp. dflt. value
  // conv - in-place type conv.
  // arr - array info and modification
  // obj*, set_obj, cpvoid*, pinterface - objects and interfaces
  //
  // u_* - various fns. for multiple types, object naming
  // sc - autoconv. to scalar
  // ua - extended set of array fns. with automatic conversion and expansion
  // map - map fns + automatic conversion to map
  // hash - hashlist fns + automatic conversion to hashlist

#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <exception>
#include <typeinfo>
#include <iostream>

namespace bmdx
{
  using namespace yk_c::experimental;
  using yk_c::meta;
  using yk_c::hashx;
  using yk_c::hashx_common;
  using yk_c::vecm;
  using yk_c::typer;
  using yk_c::bytes;
  using yk_c::s_long;
  using yk_c::s_ll;

  struct unity;
  struct unity_common
  {
    typedef meta::assert<meta::same_t<meta::s_long, bmdx_meta::s_long>::result>::t_true __check1;
    typedef meta::assert<meta::same_t<meta::s_ll, bmdx_meta::s_ll>::result>::t_true __check2;
      // private:
    typedef void* (*__Psm)(s_long ind_itf_method);
    static void _ls_psm_set(const char* piname, __Psm p) throw();
    enum Edef_itf_method { _defsm_pi = 0, sizeof_defsm = 10 };

    template<class T, class TList> struct find_type { typedef typename meta::same_t<typename TList::t_cur, T, TList, find_type<T, typename TList::tt_next> >::t __t_res; enum { ind = __t_res::ind }; };
      template<class T> struct find_type<T, meta::nothing> { enum { ind = -1 }; };

    template < class F1=int, class F2=int, class F3=int, class F4=int, class F5=int, class F6=int, class F7=int, class F8=int, class F9=int, class F10=int, class F11=int, class F12=int, class F13=int, class F14=int, class F15=int, class F16=int, class F17=int, class F18=int, class F19=int, class F20=int, class F21=int, class F22=int, class F23=int, class F24=int, class F25=int, class F26=int, class F27=int, class F28=int, class F29=int, class F30=int, class F31=int, class F32=int, class F33=int, class F34=int, class F35=int, class F36=int, class F37=int, class F38=int, class F39=int, class F40=int, class F41=int, class F42=int, class F43=int, class F44=int, class F45=int, class F46=int, class F47=int, class F48=int, class F49=int,
        class F50=int, class F51=int, class F52=int, class F53=int, class F54=int, class F55=int, class F56=int, class F57=int, class F58=int, class F59=int, class F60=int, class F61=int, class F62=int, class F63=int, class F64=int, class F65=int, class F66=int, class F67=int, class F68=int, class F69=int, class F70=int, class F71=int, class F72=int, class F73=int, class F74=int, class F75=int, class F76=int, class F77=int, class F78=int, class F79=int, class F80=int, class F81=int, class F82=int, class F83=int, class F84=int, class F85=int, class F86=int, class F87=int, class F88=int, class F89=int, class F90=int, class F91=int, class F92=int, class F93=int, class F94=int, class F95=int, class F96=int, class F97=int, class F98=int, class F99=int,
        class F100=int, int __ind = sizeof_defsm >
    struct fn_list
      : public fn_list < F2,F3,F4,F5,F6,F7,F8,F9, F10,F11,F12,F13,F14,F15,F16,F17,F18,F19, F20,F21,F22,F23,F24,F25,F26,F27,F28,F29, F30,F31,F32,F33,F34,F35,F36,F37,F38,F39, F40,F41,F42,F43,F44,F45,F46,F47,F48,F49, F50,F51,F52,F53,F54,F55,F56,F57,F58,F59, F60,F61,F62,F63,F64,F65,F66,F67,F68,F69, F70,F71,F72,F73,F74,F75,F76,F77,F78,F79, F80,F81,F82,F83,F84,F85,F86,F87,F88,F89, F90,F91,F92,F93,F94,F95,F96,F97,F98,F99,
          F100, int, __ind + 1 >
    {
      typedef fn_list < F2,F3,F4,F5,F6,F7,F8,F9, F10,F11,F12,F13,F14,F15,F16,F17,F18,F19, F20,F21,F22,F23,F24,F25,F26,F27,F28,F29, F30,F31,F32,F33,F34,F35,F36,F37,F38,F39, F40,F41,F42,F43,F44,F45,F46,F47,F48,F49, F50,F51,F52,F53,F54,F55,F56,F57,F58,F59, F60,F61,F62,F63,F64,F65,F66,F67,F68,F69, F70,F71,F72,F73,F74,F75,F76,F77,F78,F79, F80,F81,F82,F83,F84,F85,F86,F87,F88,F89, F90,F91,F92,F93,F94,F95,F96,F97,F98,F99,
          F100, int, __ind + 1 >
          tt_next;
      typedef F1 t_cur; enum { ind = __ind };
      static void Freg(void** psmt) { psmt[ind] = (void*)t_cur::F; tt_next::Freg(psmt); }
    };

    template < class F2, class F3, class F4, class F5, class F6, class F7, class F8, class F9, class F10, class F11, class F12, class F13, class F14, class F15, class F16, class F17, class F18, class F19, class F20, class F21, class F22, class F23, class F24, class F25, class F26, class F27, class F28, class F29, class F30, class F31, class F32, class F33, class F34, class F35, class F36, class F37, class F38, class F39, class F40, class F41, class F42, class F43, class F44, class F45, class F46, class F47, class F48, class F49,
        class F50, class F51, class F52, class F53, class F54, class F55, class F56, class F57, class F58, class F59, class F60, class F61, class F62, class F63, class F64, class F65, class F66, class F67, class F68, class F69, class F70, class F71, class F72, class F73, class F74, class F75, class F76, class F77, class F78, class F79, class F80, class F81, class F82, class F83, class F84, class F85, class F86, class F87, class F88, class F89, class F90, class F91, class F92, class F93, class F94, class F95, class F96, class F97, class F98, class F99,
        class F100, int __ind >
    struct fn_list
    <
      int, F2,F3,F4,F5,F6,F7,F8,F9, F10,F11,F12,F13,F14,F15,F16,F17,F18,F19, F20,F21,F22,F23,F24,F25,F26,F27,F28,F29, F30,F31,F32,F33,F34,F35,F36,F37,F38,F39, F40,F41,F42,F43,F44,F45,F46,F47,F48,F49, F50,F51,F52,F53,F54,F55,F56,F57,F58,F59, F60,F61,F62,F63,F64,F65,F66,F67,F68,F69, F70,F71,F72,F73,F74,F75,F76,F77,F78,F79, F80,F81,F82,F83,F84,F85,F86,F87,F88,F89, F90,F91,F92,F93,F94,F95,F96,F97,F98,F99,
      F100, __ind
    > { typedef meta::nothing tt_next; typedef meta::nothing t_cur; enum { ind = -1, fn_cnt = __ind }; static void Freg(void** psmt) {} };

      // Static methods of the module.
    static void* ls_modsm(s_long ind);
    typedef void* (*__Pmodsm)(s_long ind_mod_method);
    typedef __Psm (*__Pipsm)(const char* interface_name);
    typedef void* (*__Ppi)(void* phandler, const char *__iname);
    typedef void* (*__Pcpvoid)(void* phandler, s_long is_checked);
    typedef unity* (*__Punity)(void* phandler);
    typedef void* (*__Pprefo)(void* phandler, const char* tname);
    enum Emod_method
    {
      msm_obj_ipsm,
        //  1. unity_common::ls_modsm(msm_obj_ipsm) returns __Pipsm -- a pointer to static _get_sm function of the called module.
        //  2. _get_sm(name) returns __Psm -- a pointer to static __sm function of an interface, given its name.
        //  3. __sm(ind_itf_method), for the given index, returns a pointer to static helper function performing
        //      the interface method call from within the module that hash created the object.
        //      Methods of the interface are enumerated statically in o_proxy struct specialization for the interface.
        //  4. The helper function takes interface pointer + same arguments as the appropriate interface function.
      msm_obj_pi,
      msm_obj_cpvoid,
      msm_obj_punity,
      msm_cv_reg,
      msm_cv_clear,
      msm_cv_create,
      msm_cv_destroy,
      msm_cv_unity_new,
      msm_cv_delete,
      msm_cv_convert,
      msm_cv_assign,
      msm_wstring_new,
      msm_wstring_n,
      msm_wstring_p0,
      msm_wstring_wcs,
      msm_wstring_ti,
      msm_arr_el_get,
      msm_arr_el_set,
      msm_arr_el_append,
      msm_arr_el_insrem,
      msm_arr_sz_set,
      msm_arr_lb_u,
      msm_arr_ub_u,
      msm_arr_sz_u,
      msm_u_set_name,
      msm_u_clear,
      msm_ua_fill,
      msm_ua_ensure_ind,
      msm_obj_prefo,
      sizeof_msm
    };
  };

    // Supported types of values contained in unity objects.
  enum EUnityType
  {
    // Scalar types.
    // (utype() & utMaskScalar) makes one of these values.
    utEmpty,
    utInt,      // integer number, represented by long
    utFloat,    // f.p. number, represented by double
    utDate,     // date value, represented by double
    utChar,     // char
    utString,   // std::wstring
    utObject,   // object + descriptor
    utUnity,     // only in combination with utArray
    utMap, // ordered map { k, v }
    utHash, // hashlist { k, v }
    _ut_end,

      // Array flag.
    utArray = 0x20,

    _ut_0 = utEmpty | utArray, // no arrays of Empty
    utIntArray,
    utFloatArray,
    utDateArray,
    utCharArray,
    utStringArray,
    _ut_1, // no arrays of Object
    utUnityArray,


    utMaskNoArray=0x1f, // (utype() & utMaskScalar) yields the scalar type, without utArray flag

      // "return std vector" flag for unity::val.
    fvalVector = 0x100
  };

    // Exceptions, generated by the library functions.
  typedef bmdx_str::flstr_t<75> _fls75;

  struct _XUBase : std::exception
  {
    _fls75 s;
    const char* what() const throw() { return s.c_str(); }
    _XUBase(const _fls75& exc_type, const _fls75& src, const _fls75& x1, const _fls75& x2, const _fls75& x3, const _fls75& x4)
    {
      s = exc_type; s += '/'; s += src; s_long n = 0;
      if (x1.length()) { s += (n ? ", " : ": "); s += x1; ++n; }
      if (x2.length()) { s += (n ? ", " : ": "); s += x2; ++n; }
      if (x3.length()) { s += (n ? ", " : ": "); s += x3; ++n; }
      if (x4.length()) { s += (n ? ", " : ": "); s += x4; ++n; }
    }
  };
  struct XUTypeMismatch : _XUBase { XUTypeMismatch(const _fls75& src, const _fls75& x1 = "", const _fls75& x2 = "", const _fls75& x3 = "", const _fls75& x4 = "") : _XUBase("XUTM", src, x1.length() ? "T1=" + x1 : x1, x2.length() ? "T2=" + x2 : x2, x3, x4) {} };
  struct XUConvFailed: _XUBase { XUConvFailed(const _fls75& src, const _fls75& x1 = "", const _fls75& x2 = "", const _fls75& x3 = "", const _fls75& x4 = "") : _XUBase("XUCF", src, x1.length() ? "T1=" + x1 : x1, x2.length() ? "T2=" + x2 : x2, x3, x4) {} };
    // NOTE XUExec may be used by client code to generate custom exceptions.
  struct XUExec: _XUBase { XUExec(const _fls75& src, const _fls75& x1 = "", const _fls75& x2 = "", const _fls75& x3 = "", const _fls75& x4 = "") : _XUBase("XUE", src, x1, x2, x3, x4) {} };

    // Value conversion modes.
  enum EConvStrength
  {
    csLazy,
    csNormal,
    csStrict,
  };


  typedef char _unitychar;

  struct _unitydate
  {
    inline double f() const { return _jds; } // Julian date in seconds
    meta::s_ll f_sec() const; // Julian date in whole seconds (rounding takes into account floating point calc. precision)
    s_long jdn() const; // Julian day number

    inline void set(double x) { const double max = (meta::s_long_mp - 1) * 86400.; _jds = x < -max ? -max : (x > max ? max : x); }
    inline _unitydate() : _jds(0.) {}
    inline explicit _unitydate(double x) { set(x); }
      // y is not limited, d is limited to 1..31, others - to their standard range.
    _unitydate(s_long y, s_long m, s_long d, s_long h = 0, s_long min = 0, s_long s = 0);

    _unitydate& add_datetime(s_long dy, s_long dm, s_long dd, s_long dh = 0, s_long dmin = 0, s_long ds = 0);
    inline _unitydate& add_time(s_long dh, s_long dmin, s_long ds)        { return add_datetime(0, 0, 0, dh, dmin, ds); }
    inline _unitydate& add_seconds(double ss)        { set(_jds + ss); return *this; }
    inline _unitydate& round_s()        { set(double(f_sec())); return *this; } // rounds the date to whole number of seconds

    s_long d_year() const;
    s_long d_month() const;
    s_long d_day() const;
    s_long d_hour() const;
    s_long d_minute() const;
    s_long d_second() const;
    double d_ms() const; // milliseconds NOTE precision is about 0.05
    s_long d_day_of_week() const; // 1..7 (Mon..Sun)
    void d_get(s_long* py, s_long* pm, s_long* pd, s_long* ph, s_long* pmin, s_long* ps) const; // any ptr. may be 0

    bool operator==(const _unitydate& x) const { return _jds == x._jds; }
    bool operator!=(const _unitydate& x) const { return _jds != x._jds; }
    bool operator>=(const _unitydate& x) const { return _jds >= x._jds; }
    bool operator<=(const _unitydate& x) const { return _jds <= x._jds; }
    bool operator>(const _unitydate& x) const { return _jds > x._jds; }
    bool operator<(const _unitydate& x) const { return _jds < x._jds; }

    private: double _jds;
  };


    // String character set conversions.

    // NOTE When ws/bs functions are used, even indirectly, in multithreaded program,
    //  client-side setlocale and locale-dependent standard function calls
    //  must be protected with a lock:
    //    t_critsec_locale __lock(wsbs_lkp(), -1);
    //  (The above object blocks only if it's necessary, depending on the platform.)
  struct wsbs_lks {};
  typedef critsec_t<wsbs_lks> t_critsec_locale;
  s_long wsbs_lkp(); // lock sleep period: 1 if lock is needed on this platform, -1 if not

  std::string wsToBs(const std::wstring& x); // UTF-16 string to OS local encoding char string
  std::string wsToBs(const wchar_t* x, meta::s_ll n = -1); // n < 0: autodetect length based on null char.
  std::string wsToBsOem(const std::wstring& x); // UTF-16 string to OEM char string - Windows-specific (can be used for text output to console)
  std::string wsToBsOem(const wchar_t* x, meta::s_ll n = -1);
  std::string wsToBsUtf8(const std::wstring& x); // UTF-16 string to UTF-8 string
  std::string wsToBsUtf8(const wchar_t* x, meta::s_ll n = -1);
  std::string wsToBsLsb(const std::wstring& x); // low byte of each wide char value to char
  std::string wsToBsLsb(const wchar_t* x, meta::s_ll n = -1);

  std::wstring bsToWs(const std::string& x); // OS local encoding char string to UTF-16 string
  std::wstring bsToWs(const char* x, meta::s_ll n = -1);
  std::wstring bsUtf8ToWs(const std::string& x); // UTF-8 string to UTF-16 string
  std::wstring bsUtf8ToWs(const char* x, meta::s_ll n = -1);
  std::wstring bsLsbToWs(const std::string& x); // char. values to wide char values 0..255
  std::wstring bsLsbToWs(const char* x, meta::s_ll n = -1);


// Custom interafcing to a class (o_*).

  // Purpose:
  //  a) accessing interfaces, declared in the class,
  //  b) replacing their implementation without modifying the class,
  //  c) adding interfaces that are not supported by the class at all,
  //  d) adding interfaces to non-class objects (e.g. char*).

  // Usage:
  //  (1) declare a o_iimpl specialization as shown below.
  //  (2) declare a o_interfaces_top specialization, mentioning the required interface,
  //    along with any other interfaces, as shown below.
  //  (3) create unity(object), and use its pinterface() method to get the specific interface reference.

  // All steps are illustrated by the example below (MyClass1, MyInterface1, MyInterface2, MyInterface3).

  // (1) o_iimpl specialization: a custom interface implementation for a class.

    // template<>
    // struct o_iimpl<MyClass1, MyInterface1>
    // :
    //   public MyInterface1,
    //   public o_iref<MyInterface1>,
    //   public virtual o_interfaces<MyClass1>
    // {
    //   virtual MyInterface1* __pMyI() { return this; }
    //
    //   ... (custom interface implementation)
    // };
    //
    // template<>
    // struct o_iimpl<MyClass1, MyInterface1>
    // ...

  // (2) o_interfaces_top specialization: a structure comprising all interfaces supported for/by a class.

    //template<>
    // struct o_interfaces_top<MyClass1>
    // : public o_interfaces<MyClass1, MyInterface1, MyInterface2 ... up to 30 interfaces>{};

  // (3) usage

    // unity obj(*new MyClass1, true);
    // obj.pinterface<MyInterface1>()->...;

  // (4) Another way of use. Dynamic attaching interfaces.

    // template<>
    // struct o_iimpl<MyClass2, MyInterface1>
    // ...
    // template<>
    // struct o_iimpl<MyClass2, MyInterface2>
    // ...
    // unity obj(*new MyClass2, true);
    // obj.objItfsAttach<o_interfaces<MyClass2, MyInterface1, MyInterface2> >();
    // obj.pinterface<MyInterface1>()->...; // invokes o_iimpl<MyClass2, MyInterface1>

  // (5) Notes.

    //  1. o_iimpl accesses its target object by __pobj() (defined in the base o_interfaces<T>).
    //  2. Any interface name entries listed in o_interfaces<> arguments can be
    //    set to int instead of interface name. This way, interface list is statically configurable.
    //  3. It is poissible to attach interfaces without knowing the real object class.
    //    o_iimpl classes will lose access to __pobj(), but still have access to __pinterface().
    //    This allows for working with object purely through attached interfaces.
    //  4. For the client, the interface attached hides all previous interfaces of the same type.
    //    Still, o_iimpl can shortly access the nearest previous interface by __pinterface_prev().
    //    (The client can do the same with some sequence of unity::objItf... calls.)

      // class Nothing{};
      // unity obj(*new MyClass1, true);
      //
      // obj.objItfsAttach<o_interfaces<Nothing, MyInterface1, int, MyInterface3> >();
      // obj.pinterface<MyInterface1>()->...;
      //      //-- invokes o_iimpl<Nothing, MyInterface1>,
      //      //  which can still access anyhing in o_interfaces_top<MyClass1> using __pinterface_prev(this).
      // obj.pinterface<MyInterface2>()->...; // same with o_iimpl<Nothing, MyInterface3>
      //
      // obj.objItfRemoveLast<MyInterface1>(); // removes whole o_interfaces<Nothing, MyInterface1, MyInterface2>
      // obj.pinterface<MyInterface1>()->...; // invokes o_iimpl<MyClass1, MyInterface1>

  // Interface proxy class allows for calling from module B
  //    methods of object, created in binary module A.
  //    Main contents of a proxy is its list of function specifications.
  // To use proxy on an interface, the client declares a specialization of o_proxy,
  //    or includes one, declared together with interface.
  //  Example:

  //  struct ITest
  //  {
  //    virtual int test_sum(double x, double y) = 0;
  //    virtual int test_product(double x, double y) = 0;
  //  };

  //  namespace bmdx
  //  {

  //    template<> struct o_proxy<ITest> : o_proxy_base<ITest>
  //    {
  //      static inline const char* __iname() { return "Developer name / Type library name / ITest"; }

  //      struct __test_sum
  //      {
  //        typedef int (*PF)(__Interface* __pi, double x, double y);
  //        static int F(__Interface* __pi, double x, double y) { return __pi->test_sum(x, y); }
  //      };
  //      virtual int test_sum(double x, double y) { return __call<__test_sum>()(__psi(), x, y); }

  //      struct __test_product
  //      {
  //        typedef int (*PF)(__Interface* __pi, double x, double y);
  //        static int F(__Interface* __pi, double x, double y) { return __pi->test_product(x, y); }
  //      };
  //      virtual int test_product(double x, double y) { return __call<__test_product>()(__psi(), x, y); }

  //      typedef unity_common::fn_list<__test_sum, __test_product> __Methods;

  //    };

  //    // In one compilation unit of the project:
  //    template<> o_proxy<ITest> o_proxy_base<ITest>::__server;

  //  }

  // An implementation for ITest (header or cpp file):

  //  struct TestClass: public ITest
  //  {
  //    virtual int test_sum(double x, double y)
  //    {
  //      return x + y;
  //    }
  //    virtual int test_product(double x, double y)
  //    {
  //      return x * y;
  //    }
  //  };

  // A. Invocation on the client side in the same module with the target object:

//  unity v;
//  v.set_obj<o_interfaces<TestClass, ITest> >(*new TestClass, true);

//  unity::interface_ptr<ITest> ptr = v.pinterface<ITest>(); // ptr contains the direct pointer to TestClass instance interface
//  if (ptr)
//  {
//    ptr->test_sum(1.1, 2);
//    ptr->test_product(1.1, 2);
//  }

  // B. Invocation on the client side in any module (the container object is passed by pointer):

//  void f(unity* pv)
//  {
//    unity::interface_ptr<ITest> ptr = pv->pinterface<ITest>(); // ptr contains a proxy object, calling the target interface on the side of the target object
//    if (ptr)
//    {
//      ptr->test_sum(1.1, 2);
//      ptr->test_product(1.1, 2);
//    }
//  }

  struct o_handler_base;
  template<class Interface> struct o_proxy;
  template<class Interface> struct o_proxy_base : Interface
  {
    typedef Interface __Interface;
    typedef o_proxy<__Interface> __Proxy;

    o_proxy_base() : __psm(0), __pi(0) { __smt(); }
    virtual ~o_proxy_base() {}
    template<class T> typename T::PF __call() const { return (typename T::PF)__psm(unity_common::find_type<T, typename __Proxy::__Methods>::ind);  }
    static void* __sm(s_long ind) { return ind >= 0 && ind < __Proxy::__Methods::fn_cnt ? __smt()[ind] : 0; }
    Interface* __pci() const { return const_cast<o_proxy_base*>(this); }
    Interface* __psi() const { return __pi; }
    void __set_pp(unity_common::__Psm psm, Interface* pi) throw () { __psm = psm; __pi = pi; }

  private:
    unity_common::__Psm __psm; // not used by static __server instance
    __Interface* __pi; // -"-
    static __Proxy __server;
    static void** __smt()
    {
      static void* x[__Proxy::__Methods::fn_cnt] = { 0 };
        // The following may be safely called several times in MT env.
      if (x[0] == 0)
      {
        for (s_long i = 1; i < unity_common::sizeof_defsm; ++i) { x[i] = 0; }
        __Proxy::__Methods::Freg(x);
        x[0] = (void*)_ls_pi;
        unity_common::_ls_psm_set(__Proxy::__iname(), &__sm);
        if (!__server.__pi) {}
      }
      return x;
    }
    static void* _ls_pi(o_handler_base* ph);
  };

  template<class Interface> typename o_proxy_base<Interface>::__Proxy o_proxy_base<Interface>::__server;

  template<class Interface> struct o_proxy
  {
    static inline const char* __iname() { return typeid(Interface).name(); }
    unity_common::__Psm __psm; Interface* __pi; // not used in default impl.
    o_proxy() : __psm(0), __pi(0) {}
    Interface* __pci() const { return 0; }
    Interface* __psi() const { return 0; }
    void __set_pp(unity_common::__Psm psm, Interface* pi) { __psm = psm; __pi = pi; }
  };

  struct o_itfset_base;

  template <class I>
  struct o_iref
  {
    virtual I* __pMyI() = 0;

    // Occurs exactly when the set of interfaces has finished its creation.
    // See also o_itfset_base.
    virtual void __on_after_init() {}

    // Occurs exactly when the set of interfaces has fully finished its copy constructor.
    // See also o_itfset_base.
    virtual void __on_after_copy(o_itfset_base* src) {}

    // Occurs right before the set of interfaces is destroyed.
    // See also o_itfset_base.
    virtual void __on_before_destroy() {}

    o_iref(){}
    virtual ~o_iref(){}
  };

  struct o_itfset_base
  {
    // For the following 3 functions, default (itfset_base) versions are called until whole
    //  the interface set is created.
    //  After, __x_on_after_init() or __x_on_after_copy() is called from the topmost class,
    //  according to what's done to the set. At this time, all functions work in full,
    //  and the interface has chance to init itself properly.
    virtual const std::type_info& __interfaces_type() const { return typeid(void*); }
    virtual o_itfset_base* __clone_interfaces(o_handler_base* dest_phandler) const { return 0; }
    virtual o_itfset_base* __new_instance(o_handler_base* phandler) const { return 0; }
    virtual o_handler_base* __phandler() const { return 0; }

    // Occurs exactly when the set of interfaces has finished its creation.
    // This calls o_iimpl<T,I>::__on_after_init for all interfaces in the set,
    //  from leftmost to rightmost.
    virtual void __x_on_after_init() {}

    // Occurs exactly when the set of interfaces has fully finished its copy constructor.
    // This calls o_iimpl<T,I>::__on_after_copy for all interfaces in the set,
    //  from leftmost to rightmost.
    virtual void __x_on_after_copy(o_itfset_base* src) {}

    // Occurs right before the set of interfaces is destroyed.
    // This calls o_iimpl<T,I>::__on_before_destroy for all interfaces in the set,
    //  from rightmost to leftmost.
    virtual void __x_on_before_destroy() {}

    // Convenience function.
    // Note: do not call it from constructor/destructor. See comments at the beginning of o_itfset_base decl.
    //
    // Returns a valid pointer, if the object supports Interface, or 0 otherwise.
    // If several instances of Interface attached (in different sets), the last is returned.
    template<class Interface> inline Interface* __pinterface() const;

    // Convenience function
    // Note: do not call it from constructor/destructor. See comments at the beginning of o_itfset_base decl.
    //
    // For intefaces overriding other interfaces of the same type.
    // Looks for the specified interface starting from the current (on includeThis == true)
    //  or the previous (includeThis == false) set back to the default set.
    // Returns a valid Interface* in the closest set. Or 0 if nothing found.
    template<class Interface> inline Interface* __pinterface_prev(bool includeThis) const;

    virtual ~o_itfset_base(){}
  };

  struct o_handler_base
  {

    virtual void* pvoid(bool isValidOnly) const = 0; // object pointer value; isValidOnly true: 0 if the object has already been deleted, false: anyway non-null reference
    virtual const std::type_info& objDynTypeInfo() const = 0; // factual object type info (typeid(*zpObj)), note: if the object is not valid, typeid(void*) is returned instead
    virtual const std::type_info& objStatTypeInfo() const = 0; // static object type info (typeid of template argument), a valid structure even if the object has already been deleted
    virtual int objStrongRefsCount() const = 0; // -1: object deletion is not managed at all, 0: the object has been deleted, >0: the object is valid
    virtual int objWeakRefsCount() const = 0; // 0: no weak references left, together with objStrongRefsCount() <= 0 means that objhandler_refcount object is not needed already
    virtual bool objIsStrongRef() const = 0; // true means strong reference; false means weak reference -- even if object is not valid (objStrongRefsCount() <= 0)

      //Sets strong reference flag, changes reference counts accordingly.
      //  Deletes the object if the last strong ref has gone (transformed into weak).
      //  Normally, returns true (on_calm_result).
      //  Returns false in the following case: cannot create strong reference
      //  if the object has been deleted already (i.e. objStrongRefsCount() == 0),
    virtual bool objSetStrongRef(bool isStrongRef) = 0;
    virtual o_handler_base* clone() const = 0; // clones the current handler, i.e. correctly duplicates the object reference
    virtual ~o_handler_base() {}

    virtual void* prefo(const char* tname) const = 0; // if handler contains T or cref<T, L> such that string(typeid(T).name()) == string(tname), returns T*, otherwise 0

      // The entry point for accessing all interfaces supported by the object referenced from *this, at the time of call.
      //  First, tries each set of interfaces added to the object at runtime, from last to first.
      //  Finally, tries the inital set, created along with setting the object reference to the target unity object.
    template<class Interface> inline Interface* pinterface()
    {
      if (pvoid(true))
      {
        if (zAttached.nptrs() > 0) { bool is_found; Interface* pi0 = zAttached.pinterface<Interface>(is_found); if (is_found) return pi0; }
        o_iref<Interface>* iref = dynamic_cast<o_iref<Interface>*>(this); return iref ? iref->__pMyI() : 0;
      }
      else { return 0; }
    };

    // See unity::objItfsAttach for description.
    template<class Interfaces>
    int objItfsAttach()
    {
      o_itfset_base* pd = itfset_default();
      if (!pd) { return -1; } if (typeid(Interfaces) == pd->__interfaces_type()) { return 0; }
      o_itfset_base* p;
      int res = zAttached.append_new<Interfaces>(this, p);
      if (res > 0) { p->__x_on_after_init(); return res; } else if (res == -2) { return 0; } else { return -3; }
    }

    // See unity::objItfsRemove for description.
    template<class Interfaces>
    int objItfsRemove()
    {
      int ind = zAttached.find_by_set<Interfaces>();
      if (ind < 0)
      {
        o_itfset_base* pd = itfset_default(); if (!pd) { return -3; }
        if (typeid(Interfaces) == pd->__interfaces_type()) { return -1; }
        return 0;
      }
      o_itfset_base* p = zAttached.ptr(ind);
      p->__x_on_before_destroy();
      zAttached.remove_one(ind);
      delete p;
      return 1;
    }

    // See unity::objItfsRemoveInd for description.
    int objItfsRemoveInd(int ind_of_set)
    {
      if (ind_of_set == -1) { return -1; } else if (ind_of_set < 0 || ind_of_set >= zAttached.nptrs()) { return -3; }
      o_itfset_base* p = zAttached.ptr(ind_of_set);
      p->__x_on_before_destroy();
      zAttached.remove_one(ind_of_set);
      delete p;
      return 1;
    }

    // See unity::objItfRemoveLast for description.
    template<class Interface>
    int objItfRemoveLast()
    {
      int ind = zAttached.find_by_interface_prev_from<Interface>();
      if (ind < 0) { o_itfset_base* pd = itfset_default(); if (!pd) { return -3; } if (dynamic_cast<o_iref<Interface>*>(pd)) { return -1; } return 0; }
      o_itfset_base* p = zAttached.ptr(ind);
      p->__x_on_before_destroy(); zAttached.remove_one(ind); delete p;
      return 1;
    }

    // See unity::objItfsFind for description.
    template<class Interfaces>
    int objItfsFind() const
    {
      int ind = zAttached.find_by_set<Interfaces>();
      if (ind < 0) { o_itfset_base* pd = itfset_default(); if (!pd) { return -3; } if (typeid(Interfaces) == pd->__interfaces_type()) { return -1; } return -2; }
      return ind;
    }

    // See unity::objItfsFindPtr for description.
    int objItfsFindPtr(o_itfset_base* x) const
    {
      if (x == 0) { return -2; }
      int ind = zAttached.find_ptr(x); if (ind >= 0) { return ind; }
      o_itfset_base* pd = itfset_default();
      if (!pd) { return -3; } if (x == pd) { return -1; }
      return -2;
    }

    // See unity::objItfFindLast for description.
    template<class Interface>
    int objItfFindLast() const
    {
      int ind = zAttached.find_by_interface_prev_from<Interface>();
      if (ind < 0) { o_itfset_base* pd = itfset_default(); if (!pd) { return -3; } if (dynamic_cast<o_iref<Interface>*>(pd)) { return -1; } return -2; }
      return ind;
    }

      // See unity::objItfFindAll for description.
    template<class Interface>
      std::vector<int> objItfFindAll() const        { std::vector<int> retval;  o_itfset_base* pd = itfset_default(); if (dynamic_cast<o_iref<Interface>*>(pd)) { retval.push_back(-1); }  zAttached.find_by_interface_all<Interface>(retval); return retval; }

    // See unity::objItfsByInd for description.
      o_itfset_base* objItfsByInd(int ind_of_set) const        { if (ind_of_set == -1) { return itfset_default(); } else if (ind_of_set >= 0 && ind_of_set < zAttached.nptrs()) { return zAttached.ptr(ind_of_set); }  return 0; }

    // See unity::objItfsMaxInd for description.
    int objItfsMaxInd() const { return zAttached.nptrs() - 1; }

    //Looks for the specified interface starting from currentSet (on includeCurrent == true)
    //  or the previous (includeCurrent == false) set back to the default set.
    // Returns a valid Interface* in the closest set. Or 0 if nothing found.
    // Note that if the closest set hides the interface (i.e. its __MyI() returns 0),
    //  pinterface_prev returns 0. Even if some of yet deeper underlying sets contain
    //  a non-null reference.
    template<class Interface>
    Interface* pinterface_prev(const o_itfset_base* currentSet, bool includeCurrent)
    {
      int ind = zAttached.find_ptr(currentSet);
      if (ind < 0) { if (currentSet != itfset_default()) { return 0; } ind = -1; }
      if (!includeCurrent) { --ind; } if (ind < -1) { return 0; }
      if (ind >= 0)
      {
        int n = zAttached.nptrs();
        if (n >= 0)
        {
          if (ind > n - 1) { ind = n - 1; } o_itfset_base** pp = &zAttached.ptr(ind);
          for (int i = ind; i >= 0; --i) { o_iref<Interface>* iref = dynamic_cast<o_iref<Interface>*>(*pp); if (iref) { return iref->__pMyI(); } --pp; }
        }
      }
      o_itfset_base* pd = itfset_default();
      o_iref<Interface>* iref = dynamic_cast<o_iref<Interface>*>(pd);
      if (iref) { return iref->__pMyI(); }
      return 0;
    }

    // dest is cleared, then the function sets dest = &x,
    //  like unity::set_obj. The difference is that the list of interfaces
    //  attached to dest is the same as in this.
    //  This allows for implementing canonic constructors
    //  (new object, copy of object)
    //  interface for particular classes. See i_construction template.
    // Returns:
    //  1 - on_calm_result, dest is cleared and then initialized with x.
    //  -1 - type of x does not match this->objStatTypeInfo. dest is not changed.
    //  -2 - dest == * this. dest is not changed.
    //  -4 - dest == 0 dest is not changed.
    //  -3 - any other error. dest is not changed.
    template<class Obj>
    int replicate_interfaces(Obj* x, bool isStrongRef, unity& dest);

  protected:

    virtual o_itfset_base* itfset_default() const { return 0; }
    virtual void __replicate_interfaces_unchecked(void* x, bool isStrongRef, unity& dest) const = 0;

    class pi_array
    {
      void* d;
    public:
      typedef o_itfset_base* PB;

      int nptrs() const { return d ? *((short*)d) : 0; }
      PB& ptr(int ind) const { return *(ind + (PB*)(sizeof(short) + (char*)d)); }

      // NOTE 1: on append on_calm_result, pi_array takes ownership on the given object (*x).
      // NOTE 2: insertion succeds only if x has both unique value and type among other pi_array elements.
      // Returns:
      //  1 - appending successful.
      //  -1 - (possible only on doTypeCheck == true) attempt to store duplicate pointer (i.e. x already exists in the array).
      //  -2 - (possible only on doTypeCheck == true) attempt to store a pointer to the object of the same type as of some object that is already in the array.
      //  -3 - x==0.
      //  -4 - failure by any other cause.
      int append(PB x, bool doTypeCheck)
      {
        if (!x) return -3;
        int n = nptrs();
        if (n > 0)
        {
          if (doTypeCheck)
          {
            const std::type_info& ti_itfs = x->__interfaces_type();
            short n2 = n + 1;
            if (n2 <= 0) return -4;
            PB* pp = &ptr(0);
            for (int i = 0; i < n; ++i) { if (x == *pp) { return -1; } if (ti_itfs == (*pp)->__interfaces_type()) { return -2; } ++pp; }
          }
          void* d2 = realloc(d, sizeof(short) + sizeof(PB) * (n + 1));
          if (!d2) { return -4; }
          d = d2; *((short*)d) = n + 1; ptr(n) = x;
        }
        else
        {
          d = malloc(sizeof(short) + sizeof(PB));
          if (!d) { return -4; }
          *((short*)d) = 1; ptr(0) = x;
        }
        return 1;
      }

      template<class Obj, class Interfaces>
      struct itfset_top_tneq // for C != Obj
        : public Interfaces
      {
        typedef itfset_top_tneq TSelf;
        itfset_top_tneq(o_handler_base* phandler) : Interfaces(), zpHandler(phandler) {}
        itfset_top_tneq(const itfset_top_tneq& src, o_handler_base* phandler) : Interfaces(src), zpHandler(phandler) {}

      private:
        o_handler_base* const zpHandler;

        virtual const std::type_info& __interfaces_type() const { return typeid(Interfaces); }
        virtual o_itfset_base* __clone_interfaces(o_handler_base* dest_phandler) const { return new TSelf(*this, dest_phandler); }
        virtual o_itfset_base* __new_instance(o_handler_base* phandler) const { return new TSelf(phandler); }
        virtual o_handler_base* __phandler() const { return zpHandler; }
        virtual Obj* __pobj() const { return 0; }
      };

      template<class Obj, class Interfaces>
      struct itfset_top_teq // for C == Obj
      :
        public Interfaces
      {
        typedef itfset_top_teq TSelf;
        itfset_top_teq(o_handler_base* phandler) : Interfaces(), zpHandler(phandler) {}
        itfset_top_teq(const itfset_top_teq& src, o_handler_base* phandler) : Interfaces(src), zpHandler(phandler) {}

      private:
        o_handler_base* const zpHandler;

        virtual const std::type_info& __interfaces_type() const { return typeid(Interfaces); }
        virtual o_itfset_base* __clone_interfaces(o_handler_base* dest_phandler) const { return new TSelf(*this, dest_phandler); }
        virtual o_handler_base* __phandler() const { return zpHandler; }
        virtual o_itfset_base* __new_instance(o_handler_base* phandler) const { return new TSelf(phandler); }
        virtual Obj* __pobj() const { return reinterpret_cast<Obj*>(zpHandler->pvoid(true)); }
      };

        // Creates an interfaces set and appends it to the collection if no set of this type exists.
        // The set must inherit from o_interfaces<C>,
        //  where either C == Obj (type of object handled by unity),
        //  or C != Obj.
        // Note: pi_array owns any object it refers to unless the pointer is removed
        //  (see remove_one, remove_last).
        // Returns:
        //  2 - appending successful, C == Obj.
        //  1 - appending successful, C != Obj.
        //  -2 - attempt to attach to the set of the same type that is already in the collection.
        //  -4 - failure by any other cause.
      template<class Interfaces>
      int append_new(o_handler_base* phandler, o_itfset_base*& ret_pset)
      {
        int n = nptrs();
        if (n > 0)
        {
          const std::type_info& ti_itfs = typeid(Interfaces);
          PB* pp = &ptr(0);
          for (int i = 0; i < n; ++i) { if (ti_itfs == (*pp)->__interfaces_type()) { return -2; } ++pp; }
          void* d2 = realloc(d, sizeof(short) + sizeof(PB) * (n + 1));
          if (!d2) { return -4; } d = d2;
        }
        else
        {
          d = malloc(sizeof(short) + sizeof(PB)); if (!d) { return -4; }
        }

        const std::type_info& ti_itfs_obj = typeid(typename Interfaces::__Obj);
        bool isCEqObj = ti_itfs_obj == phandler->objStatTypeInfo();

        o_itfset_base* x;
        if (isCEqObj) { x = new itfset_top_teq<typename Interfaces::__Obj, Interfaces>(phandler); }
          else { x = new itfset_top_tneq<typename Interfaces::__Obj, Interfaces>(phandler); }
        *((short*)d) = n + 1; ptr(n) = x;
        ret_pset = x; if (isCEqObj) { return 2; } else { return 1; }
      }

      // If ind is correct (0..nptrs()-1), removes the specified pointer (guaranteed).
      // The client takes ownership on the pointer.
      PB remove_one(int ind)
      {
        int n = nptrs(); if (ind < 0 || ind >= n) return 0;
        PB& rp = ptr(ind); PB p = rp;
        if (n == 1) { free(d); d = 0; }
        else
        {
          PB* pp = &rp; for (int i = ind + 1; i < n; ++i) { *pp = *(pp+1); ++pp; }
          *((short*)d) = n - 1;
          void* d2 = realloc(d, sizeof(short) + sizeof(PB) * (n - 1)); if (d2) { d = d2; }
        }

        return p;
      }

      // Removes the last (rightmost) pointer to a set of interfaces,
      //  returns that pointer. The client takes ownership on the pointer.
      // If the set was empty (nothing removed) - returns 0.
      PB remove_last()
      {
        return remove_one(nptrs() - 1);
      }

      //Returns the index of the specified interface to the internally contained object.
      //  If interface is not found, returns -1.
      //  If the object has >1 instances of the interface, the index of the last attached is returned.
      //If ind >= 0 is specified, the search starts from ind back to 0th element.
      //  If ind is omitted or incorrect value, the search starts from the last element back to 0th element.
      template<class Interface>
      int find_by_interface_prev_from(int ind = -1) const
      {
        int n = nptrs();
        if (n == 0) { return -1; }
        if (ind < 0 || ind > n - 1) { ind = n - 1; }
        PB* pp = &ptr(ind);
        for (int i = ind; i >= 0; --i) { if (dynamic_cast<o_iref<Interface>*>(*pp)) { return i; } --pp; }
        return -1;
      }

      //Returns indices of all sets supporting the specified interface.
      //  Testing is done first-to-last, i.e. the most actual set will be at the end of the collection.
      template<class Interface>
      void find_by_interface_all(std::vector<int>& io_retval) const
      {
        int n = nptrs();
        if (n > 0)
        {
          PB* pp = &ptr(0);
          for (int i = 0; i < n; ++i) { if (dynamic_cast<o_iref<Interface>*>(*pp)) { io_retval.push_back(i); } ++pp; }
        }
      }

      //Returns the index of a set of interfaces of the specified type.
      //  If not found, returns -1.
      template<class Interfaces>
      int find_by_set() const
      {
        int n = nptrs();
        if (n > 0)
        {
          const std::type_info& ti_itfs = typeid(Interfaces);
          PB* pp = &ptr(n-1);
          for (int i = n - 1; i >= 0; --i) { if (ti_itfs == (*pp)->__interfaces_type()) { return i; } --pp; }
        }
        return -1;
      }

      //Returns the index of the given pointer.
      //  If not found, returns -1.
      int find_ptr(const o_itfset_base* x) const
      {
        if (!x) return -1;

        int n = nptrs();
        if (n > 0)
        {
          PB* pp = &ptr(n-1);
          for (int i = n - 1; i >= 0; --i) { if (x == *pp) { return i; } --pp; }
        }
        return -1;
      }

      //Returns the specified interface to the internally contained object.
      //  If not found, returns false and 0.
      //  If found, returns true and the pointer from the last attached set containing o_iref<Interface>.
      //  (The pointer can be 0 in cases, when the set hides it by defining __MyI() { return 0; }.)
      //  If the object has >1 instances of the interface, the last attached is returned.
      template<class Interface>
      Interface* pinterface(bool& is_found) const
      {
        int n = nptrs();
        if (n > 0)
        {
          PB* pp = &ptr(n-1);
          for (int i = 0; i < n; ++i)
          {
            o_iref<Interface>* iref = dynamic_cast<o_iref<Interface>*>(*pp);
            if (iref)
            {
              is_found = true;
              return iref->__pMyI();
            }
            --pp;
          }
        }

        is_found = false;
        return 0;
      }

      pi_array() : d(0) {}
      ~pi_array() { if (d) { int n = nptrs(); for (int i = 0; i < n; ++i) { delete ptr(i); } free(d); } }
    private:
      pi_array(const pi_array& src); // disabled
      pi_array& operator=(const pi_array& src); // disabled
    };

    pi_array zAttached; // the set of interfaces, dynamically attached to the object (may be empty)
    inline static pi_array& ref_zAttached(o_handler_base& h) { return h.zAttached; }
  };

  template<class Interface> inline Interface* o_itfset_base::__pinterface() const
    { o_handler_base* p = __phandler(); return p ? p->pinterface<Interface>() : 0; }
  template<class Interface> inline Interface* o_itfset_base::__pinterface_prev(bool includeThis) const
    { o_handler_base* p = __phandler(); return p ? p->pinterface_prev<Interface>(this, includeThis) : 0; }

  template<class Interface>
    void* o_proxy_base<Interface>::
    _ls_pi(o_handler_base* ph) { return ph->pinterface<__Interface>(); }


  template <class MyT, class MyI>
  struct o_iimpl;

  template
  <
      class T,
      class I1=int, class I2=int, class I3=int, class I4=int, class I5=int, class I6=int, class I7=int, class I8=int, class I9=int,
      class I10=int, class I11=int, class I12=int, class I13=int, class I14=int, class I15=int, class I16=int, class I17=int, class I18=int, class I19=int,
      class I20=int, class I21=int, class I22=int, class I23=int, class I24=int, class I25=int, class I26=int, class I27=int, class I28=int, class I29=int,
      class I30=int
  >
  struct o_interfaces
  :
    public virtual o_iimpl<T, I1>,
    public virtual o_interfaces<T, I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,I15,I16,I17,I18,I19,I20,I21,I22,I23,I24,I25,I26,I27,I28,I29,I30, int>
  {
    typedef o_interfaces<T, I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,I15,I16,I17,I18,I19,I20,I21,I22,I23,I24,I25,I26,I27,I28,I29,I30, int> __NextIImpl;
    virtual void __x_on_after_init() { o_iimpl<T, I1>* p = this; p->__on_after_init(); __NextIImpl::__x_on_after_init(); }
    virtual void __x_on_after_copy(o_itfset_base* src) { o_iimpl<T, I1>* p = this; p->__on_after_copy(src); __NextIImpl::__x_on_after_copy(src); }
    virtual void __x_on_before_destroy() { __NextIImpl::__x_on_before_destroy(); o_iimpl<T, I1>* p = this; p->__on_before_destroy(); }
  };

  template
  <
      class T,
      class I2, class I3, class I4, class I5, class I6, class I7, class I8, class I9,
      class I10, class I11, class I12, class I13, class I14, class I15, class I16, class I17, class I18, class I19,
      class I20, class I21, class I22, class I23, class I24, class I25, class I26, class I27, class I28, class I29,
      class I30
  >
  struct o_interfaces<T, int,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,I15,I16,I17,I18,I19,I20,I21,I22,I23,I24,I25,I26,I27,I28,I29,I30>
  :
    public virtual o_interfaces<T, I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,I15,I16,I17,I18,I19,I20,I21,I22,I23,I24,I25,I26,I27,I28,I29,I30, int>
  {
    typedef o_interfaces<T, I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,I14,I15,I16,I17,I18,I19,I20,I21,I22,I23,I24,I25,I26,I27,I28,I29,I30, int> __NextIImpl;
    virtual void __x_on_after_init() { __NextIImpl::__x_on_after_init(); }
    virtual void __x_on_after_copy(o_itfset_base* src) { __NextIImpl::__x_on_after_copy(src); }
    virtual void __x_on_before_destroy() { __NextIImpl::__x_on_before_destroy(); }
  };

  template<class T>
  struct o_interfaces<T>
  :
    public virtual o_itfset_base
  {
    typedef T __Obj;
    virtual T* __pobj() const { return 0; } // may return 0 if T is not the type of object to which this interface is attached to
  };

  template <class MyT, class MyI>
  struct o_iimpl
  :
    public o_iref<MyI>,
    public virtual o_interfaces<MyT>
  {
    virtual MyI* __pMyI() { o_interfaces<MyT>* p = this; return p->__pobj(); }
  };

  // This is the default argument for unity::set_obj.
  //  If not specialized for a class, the object of that class, referenced by the unity container,
  //  will not support any interfaces, at least until some are attached.
  template<class T>
  struct o_interfaces_top
  : public o_interfaces<T>{};

  // i_construction is an interface allowing unity object
  //  to create/copy (without special arguments) objects of the same type
  //  as referenced by the unity object.
  // Default implementation struct o_iimpl<T, i_construction>
  //  is in this file below (see). It uses T() and T(const T&).
  // Usage:

    //  unity v;
    //  v.set_obj<o_interfaces<Obj, i_construction> >(*new Obj, true);
    //  ...
    //  (in another place where const unity& v is accessible:)
    //  unity v2 = v.pinterface<i_construction>()->new_instance();
    //  unity v3 = v.pinterface<i_construction>()->new_copy();

  struct i_construction
  {
    // Returns a new T().
    virtual unity new_instance() const = 0;

    // Returns a new T(*__pobj()).
    // Note if the source object is not accessible (__pobj() == 0),
    //  the function returns unity().
    virtual unity new_copy() const = 0;

    virtual ~i_construction(){}
  };
}

namespace yk_c
{
  using namespace bmdx;
  template<> struct vecm::spec<unity> { struct aux : vecm::config_aux<unity> { }; typedef config_t<unity, 1, 4, 1, aux> config; };
  template<class T, class L> struct vecm::spec<cref_t<T, L> > { typedef cref_t<T, L> t; struct aux : vecm::config_aux<t> { }; typedef config_t<t, 1, 4, 1, aux> config; };
}

namespace bmdx
{
  struct unity
  {
  private:
    struct _storage { void* p1; void* p2; };
    typedef s_long (*_PFti)();
    template<class T, class _ = meta::nothing> struct _reference_base { static inline T* _drf_c(const unity* p) throw() { return 0; } };
    template<class T> struct _reference_base<T, short> { static const s_long utt = -1; };
    template<class T> struct _reference_base<T, int>
    {
      enum { rb_tag = 1 }; typedef T t;
      static inline t* deref(_storage& x, bool byptr) throw() { return reinterpret_cast<t*>(byptr ? x.p1 : &x); }
      static inline t* _drf_c(const unity* p) throw() { return deref(p->_data, p->isByPtr()); }
    };
  public:

    template<class T> struct checked_ptr
    {
      T* p;
      inline checked_ptr(T* p_) : p(p_) {}
      inline T& operator*() const { check(); return *p; }
      inline operator T*() const { return p; }
      inline operator bool() const { return bool(p); }
      inline T* operator->() const { check(); return p; }
      inline bool operator==(const checked_ptr& p2) { return p == p2.p; }
      inline bool operator==(const T* p2) { return p == p2; }
      inline bool operator!=(const checked_ptr& p2) { return p == p2.p; }
      inline bool operator!=(const T* p2) { return p != p2; }

      inline void check() const throw(XUExec) { if (!p) { throw XUExec("checked_ptr.p == 0: ", typeid(T*).name()); } }
    };

    template<class Interface> struct interface_ptr
    {
      typedef Interface __Interface; typedef o_proxy<__Interface> __Proxy;

      struct exc_null_ptr {};

      __Interface* operator->() const throw (exc_null_ptr) { return _p_checked(); }
      __Interface& operator*() const throw (exc_null_ptr) { return *_p_checked(); }

      interface_ptr() throw () : __p(0), __isProxy(false) {}
      ~interface_ptr() throw () { if (__isProxy) { try { delete (__Proxy*)__p; } catch (...) {} __p = 0; } else { __p = 0; } }

      interface_ptr(__Interface* pi, unity_common::__Pmodsm pmsm) throw ()
        : __p(0), __isProxy(false)
      {
        if (pmsm == unity_common::ls_modsm) { __p = pi; return; }
        if (!pi || !pmsm) { return; }
        unity_common::__Pipsm _get_sm = (unity_common::__Pipsm)pmsm(unity_common::msm_obj_ipsm);
        unity_common::__Psm psm = _get_sm(__Proxy::__iname()); if (!psm) { return; }
        try { __p = new __Proxy(); __isProxy = true; ((__Proxy*)__p)->__set_pp(psm, pi); } catch (...) {}
      }

      interface_ptr(o_handler_base* ph, unity_common::__Pmodsm pmsm) throw ()
        : __p(0), __isProxy(false)
      {
        if (pmsm == unity_common::ls_modsm) { __p = ph->pinterface<__Interface>(); return; }
        if (!ph || !pmsm) { return; }
        unity_common::__Ppi _get_pi = (unity_common::__Ppi)pmsm(unity_common::msm_obj_pi);
        __Interface* pi = (__Interface*)(_get_pi(ph, __Proxy::__iname())); if (!pi) { return; }
        unity_common::__Pipsm _get_sm = (unity_common::__Pipsm)pmsm(unity_common::msm_obj_ipsm);
        unity_common::__Psm psm = _get_sm(__Proxy::__iname()); if (!psm) { return; }
        try { __p = new __Proxy(); __isProxy = true; ((__Proxy*)__p)->__set_pp(psm, pi); } catch (...) {}
      }

        // NOTE Copying does not generate exceptions.
        //  If copying fails, ptr_u() == 0.
        //  If copying succeeds, bool(ptr_u()) == bool(x.ptr_u()).
      interface_ptr(const interface_ptr& x)  throw () : __p(0), __isProxy(false) { if (x.__isProxy) { try { __p = new __Proxy(*(__Proxy*)x.__p); __isProxy = true; } catch (...) {} } else { __p = x.__p; } }

        // NOTE Assignment does not generate exceptions.
        //  If assignment fails, ptr_u() == 0.
        //  If assignment succeeds, bool(ptr_u()) == bool(x.ptr_u()).
      interface_ptr& operator=(const interface_ptr& x) throw () { this->~interface_ptr(); new (this) interface_ptr(x); return *this; }

        // NOTE In case of proxy, client-side interface pointers are compared.
      bool operator == (const interface_ptr& x) const { return _p_u() == x._p_u(); }
      bool operator != (const interface_ptr& x) const { return _p_u() != x._p_u(); }
      bool operator == (const __Interface* x) const { return _p_u() == x; }
      bool operator != (const __Interface* x) const { return _p_u() != x; }

      __Interface* ptr() throw (exc_null_ptr) { return _p_checked(); }
      const __Interface* ptr() const throw (exc_null_ptr) { return _p_checked(); }
      __Interface* ptr_u() throw () { return _p_u(); }
      const __Interface* ptr_u() const throw () { return _p_u(); }

        // In case of local interface, returns local pointer.
        // In case of proxy, returns server-side interface pointer (same as source unity::objPtr()).
      __Interface* psi_u() throw () { return _psi_u(); }
      const __Interface* psi_u() const throw () { return _psi_u(); }

      operator const void*() const { return _p_u(); }

    private: void* __p; bool __isProxy;
      inline __Interface* _p_checked() const { if (__isProxy) { if (__p) { __Interface* p = ((__Proxy*)__p)->__pci(); if (p) { return p; } } } else if (__p) { return (__Interface*)__p; } throw exc_null_ptr(); }
      inline __Interface* _p_u() const { if (__isProxy) { if (!__p) { return 0; } return ((__Proxy*)__p)->__pci(); } else { return (__Interface*)__p; } }
      inline __Interface* _psi_u() const { if (__isProxy) { if (!__p) { return 0; } return ((__Proxy*)__p)->__psi(); } else { return (__Interface*)__p; } }
    };

    struct kf_unity
    {
        // By default, no flags are set.
        //  See also: static const s_long fkcmp*
      kf_unity(s_long flags_ = 0) : _flags(char(flags_)) {}

        // See: static const s_long fkcmp*
      s_long flags() const { return _flags; }
      void _set_flags(s_long x) const { _flags = x & unity::_fkcmp_mask; }

      template<class K2> inline void cnew(unity* p, const K2& x) const { new (p) unity(x); if ((_flags & fkcmpScalarize) && !p->isScalar()) { try { *p = p->u_key(); } catch (...) { p->clear(); throw; } } }
      inline s_long hash(const unity& x) const { return x.k_hashf(_flags); }
      inline bool is_eq(const unity& x1, const unity& x2) const { return x1.k_eq(x2, _flags); }
      template<class K2> inline s_long hash(const K2& x) const { return this->hash(unity(x)); }
      template<class K2> inline bool is_eq(const unity& x1, const K2& x2) const { return x1.k_eq(unity(x2), _flags); }

      inline bool less12(const unity& x1, const unity& x2) const { return x1.k_less(x2, _flags); }
      inline bool less21(const unity& x1, const unity& x2) const { return x1.k_less(x2, _flags); }
      template<class K2> inline bool less12(const unity& x1, const K2& x2) const { return x1.k_less(unity(x2), _flags); }
      template<class K2> inline bool less21(const K2& x1, const unity& x2) const { return unity(x2).k_less(x1, _flags); }

      private: mutable s_long _flags;
    };

    struct _hl_impl;
    typedef yk_c::experimental::ordhs_t<unity, unity, kf_unity, kf_unity> t_map;
    typedef _hl_impl t_hash;

    template<int utt, class _ = meta::nothing> struct valtype_t {};

      template<class _> struct valtype_t<utInt, _> { typedef meta::s_ll t; };
      template<class _> struct valtype_t<utFloat, _> { typedef double t; };
      template<class _> struct valtype_t<utDate, _> { typedef _unitydate t; };
      template<class _> struct valtype_t<utChar, _> { typedef _unitychar t; };
      template<class _> struct valtype_t<utString, _> { typedef std::wstring t; };
      template<class _> struct valtype_t<utUnity, _> { typedef unity t; };
      template<class _> struct valtype_t<utIntArray, _> { typedef vec2_t<meta::s_ll> t; typedef typename t::t_value t_elem; };
      template<class _> struct valtype_t<utFloatArray, _> { typedef vec2_t<double> t; typedef typename t::t_value t_elem; };
      template<class _> struct valtype_t<utDateArray, _> { typedef vec2_t<_unitydate> t; typedef typename t::t_value t_elem; };
      template<class _> struct valtype_t<utCharArray, _> { typedef vec2_t<_unitychar> t; typedef typename t::t_value t_elem; };
      template<class _> struct valtype_t<utStringArray, _> { typedef vec2_t<std::wstring> t; typedef typename t::t_value t_elem; };
      template<class _> struct valtype_t<utUnityArray, _> { typedef vec2_t<unity> t; typedef typename t::t_value t_elem; };
      template<class _> struct valtype_t<-1, _> { typedef void t; };

    template<class T, class _ = meta::nothing> struct reference_t : _reference_base<T, _> {};

      template<class _> struct reference_t<meta::s_ll, _>  : _reference_base<meta::s_ll, int> { static const s_long utt = utInt; };
      template<class _> struct reference_t<double, _>  : _reference_base<double, int> { static const s_long utt = utFloat; };
      template<class _> struct reference_t<_unitydate, _>  : _reference_base<_unitydate, int> { static const s_long utt = utDate; };
      template<class _> struct reference_t<_unitychar, _>  : _reference_base<_unitychar, int> { static const s_long utt = utChar; };
        // NOTE reference_t<std::wstring>::_drf_c includes additional check for string compatibility.
        //    Returns 0 when string is incompatible.
        //    Still, val<utString>, cref<utString> check it and get a valid value from cv_ff::cv_wstring::Fcopy.
      template<class _> struct reference_t<std::wstring, _>  : _reference_base<std::wstring, int> { static const s_long utt = utString; static inline typename reference_t::t* _drf_c(const unity* p) throw() { if (p->pmsm == unity_common::ls_modsm) { return reference_t::deref(p->_data, true); } s_long ti1 = typer<std::wstring>().t_ind; s_long ti2 = ((_PFti)p->pmsm(unity_common::msm_wstring_ti))(); return  ti1 < 0 && ti1 == ti2 ? reference_t::deref(p->_data, true) : 0; } };
      template<class _> struct reference_t<unity, _>  { static const s_long utt = utUnity; typedef unity t; };
      template<class _> struct reference_t<vec2_t<meta::s_ll>, _>  : _reference_base<vec2_t<meta::s_ll>, int> { static const s_long utt = utIntArray; typedef typename reference_t::t t; typedef typename t::t_value t_elem; };
      template<class _> struct reference_t<vec2_t<double>, _>  : _reference_base<vec2_t<double>, int> { static const s_long utt = utFloatArray; typedef typename reference_t::t t; typedef typename t::t_value t_elem; };
      template<class _> struct reference_t<vec2_t<_unitydate>, _>  : _reference_base<vec2_t<_unitydate>, int> { static const s_long utt = utDateArray; typedef typename reference_t::t t; typedef typename t::t_value t_elem; };
      template<class _> struct reference_t<vec2_t<_unitychar>, _>  : _reference_base<vec2_t<_unitychar>, int> { static const s_long utt = utCharArray; typedef typename reference_t::t t; typedef typename t::t_value t_elem;  };
      template<class _> struct reference_t<vec2_t<std::wstring>, _>  : _reference_base<vec2_t<std::wstring>, int> { static const s_long utt = utStringArray; typedef typename reference_t::t t; typedef typename t::t_value t_elem; };
      template<class _> struct reference_t<vec2_t<unity>, _>  : _reference_base<vec2_t<unity>, int> { static const s_long utt = utUnityArray; typedef typename reference_t::t t; typedef typename t::t_value t_elem; };
      template<class _> struct reference_t<void, _>  : _reference_base<void, int> {};

    template<class T, class _ = meta::nothing> struct trivconv_t { typedef meta::s_ll t_target; static inline t_target F(const T& x) { return x; } static inline T Fback(const t_target& x) { return x; } };

      template<class _> struct trivconv_t<double, _> { typedef double t_target; static inline const t_target& F(const t_target& x) { return x; } static inline t_target Fback(const t_target& x) { return x; } };
      template<class _> struct trivconv_t<_unitydate, _> { typedef _unitydate t_target; static inline const t_target& F(const t_target& x) { return x; } static inline t_target Fback(const t_target& x) { return x; } };
      template<class _> struct trivconv_t<_unitychar, _> { typedef _unitychar t_target; static inline const t_target& F(const t_target& x) { return x; } static inline t_target Fback(const t_target& x) { return x; } };
      template<class _> struct trivconv_t<std::wstring, _> { typedef std::wstring t_target; static inline const t_target& F(const t_target& x) { return x; } static inline t_target Fback(const t_target& x) { return x; } };
      template<class _> struct trivconv_t<unity, _> { typedef unity t_target; static inline const t_target& F(const t_target& x) { return x; } static inline t_target Fback(const t_target& x) { return x; } };

      template<class _> struct trivconv_t<bool, _> { typedef _unitychar t_target; static inline t_target F(bool x) { return x; } static inline bool Fback(const t_target& x) { return x; } };
      template<class _> struct trivconv_t<signed char, _> { typedef _unitychar t_target; static inline t_target F(signed char x) { return x; } static inline signed char Fback(const t_target& x) { return x; } };
      template<class _> struct trivconv_t<unsigned char, _> { typedef _unitychar t_target; static inline t_target F(unsigned char x) { return x; } static inline unsigned char Fback(const t_target& x) { return x; } };
      template<class _> struct trivconv_t<wchar_t, _> { typedef meta::s_ll t_target; static inline t_target F(wchar_t x) { return x; } static inline wchar_t Fback(const t_target& x) { return x; } };
      template<class _> struct trivconv_t<signed short int, _> { typedef meta::s_ll t_target; static inline t_target F(signed short int x) { return x; } static inline signed short int Fback(const t_target& x) { return x; } };
      template<class _> struct trivconv_t<unsigned short int, _> { typedef meta::s_ll t_target; static inline t_target F(unsigned short int x) { return x; } static inline unsigned short int Fback(const t_target& x) { return x; } };
      template<class _> struct trivconv_t<signed int, _> { typedef meta::s_ll t_target; static inline t_target F(signed int x) { return x; } static inline signed int Fback(const t_target& x) { return (signed int)x; } };
      template<class _> struct trivconv_t<unsigned int, _> { typedef meta::s_ll t_target; static inline t_target F(unsigned int x) { return x; } static inline unsigned int Fback(const t_target& x) { return (unsigned int)x; } };
      template<class _> struct trivconv_t<signed long int, _> { typedef meta::s_ll t_target; static inline t_target F(signed long int x) { return x; } static inline signed long int Fback(const t_target& x) { return (signed long int)(x); } };
      template<class _> struct trivconv_t<unsigned long int, _> { typedef meta::s_ll t_target; static inline t_target F(unsigned long int x) { return x; } static inline unsigned long int Fback(const t_target& x) { return (unsigned long int)x; } };
      template<class _> struct trivconv_t<signed long long int, _> { typedef meta::s_ll t_target; static inline t_target F(signed long long int x) { return x; } static inline signed long long int Fback(const t_target& x) { return x; } };
      template<class _> struct trivconv_t<unsigned long long int, _> { typedef meta::s_ll t_target; static inline t_target F(unsigned long long int x) { return x; } static inline unsigned long long int Fback(const t_target& x) { return x; } };

      template<class _> struct trivconv_t<float, _> { typedef double t_target; static inline t_target F(float x) { return x; } static inline float Fback(const t_target& x) { return x; } };
      template<class _> struct trivconv_t<long double, _> { typedef double t_target; static inline t_target F(long double x) { return x; } static inline long double Fback(const t_target& x) { return x; } };

      struct _wr_cstring :  std::string { typedef std::string t_base; _wr_cstring() {} _wr_cstring(const std::string& x) : t_base(x) {} const t_base& cstr() const { return *this; } std::wstring wstr() const { return bsToWs(*this); } };
      struct _wr_wstring :  std::wstring { typedef std::wstring t_base; _wr_wstring() {} _wr_wstring(const std::wstring& x) : t_base(x) {} std::string cstr() const { return wsToBs(*this); } const t_base& wstr() const { return *this; } };

      template<class _> struct trivconv_t<std::string, _> { typedef std::wstring t_target; static inline t_target F(const std::string& x) { return bsToWs(x); } static inline std::string Fback(const t_target& x) { return wsToBs(x); } };
      template<class _> struct trivconv_t<char*, _> { typedef std::wstring t_target; static inline t_target F(const char* x) { return bsToWs(x); } static inline _wr_cstring Fback(const t_target& x) { return wsToBs(x); } };
      template<class _> struct trivconv_t<char[], _> { typedef std::wstring t_target; static inline t_target F(const char x[]) { return bsToWs(x); } static inline _wr_cstring Fback(const t_target& x) { return wsToBs(x); } };

      template<class _> struct trivconv_t<wchar_t*, _> { typedef std::wstring t_target; static inline t_target F(const wchar_t* x) { return x; } static inline const _wr_wstring Fback(const t_target& x) { return x; } };
      template<class _> struct trivconv_t<wchar_t[], _> { typedef std::wstring t_target; static inline t_target F(const wchar_t x[]) { return x; } static inline const _wr_wstring Fback(const t_target& x) { return x; } };

      template<class _> struct trivconv_t<_wr_cstring, _> { typedef std::wstring t_target; static inline t_target F(const _wr_cstring& x) { return x.wstr(); } static inline _wr_cstring Fback(const t_target& x) { return wsToBs(x); } };
      template<class _> struct trivconv_t<_wr_wstring, _> { typedef std::wstring t_target; static inline const t_target& F(const _wr_wstring& x) { return x.wstr(); } static inline _wr_wstring Fback(const t_target& x) { return x; } };

      template<class _, s_long n> struct trivconv_t<bmdx_str::flstr_t<n>, _> { typedef std::string t_target; static inline t_target F(const bmdx_str::flstr_t<n> x) { return x; } static inline bmdx_str::flstr_t<n> Fback(const t_target& x) { return x.c_str(); } };

  private:
    template<int utt, bool __bv = (utt & fvalVector)> struct _valtype2_t : valtype_t<utt> {};
    template<int utt> struct _valtype2_t<utt, true> { typedef typename valtype_t<((utt & ~fvalVector) | utArray)>::t_elem t_elem; typedef std::vector<t_elem> t; };
    template <int utt, class _  = meta::nothing> struct _rx_el { static inline typename valtype_t<utt>::t& F(unity& x) { return x.rx<utt>(); } };
      template <class _> struct _rx_el<utUnity, _> { static inline typename valtype_t<utUnity>::t& F(unity& x) { return x; } };
    template <int utt, class _  = meta::nothing> struct _refc_el { static inline const typename valtype_t<utt>::t& F(const unity& x) { return x.ref<utt>(); } };
      template <class _> struct _refc_el<utUnity, _> { static inline const typename valtype_t<utUnity>::t& F(const unity& x) { return x; } };
    template<int utt, class _ = meta::nothing> struct _arr_el_get
    {
      static inline typename _valtype2_t<utt>::t F(const unity* pv, s_long ind, EConvStrength cs)
      {
        typedef typename _valtype2_t<utt>::t T;
        try {
          const T* p = pv->pval<utt>(ind); if (p) { return *p; }
          unity temp; s_long res = pv->_Lel_get(ind, &temp);
          if (res == 1) { return temp.val<utt>(cs); }
          if (res == -2) { throw XUTypeMismatch("_arr_el_get.1", _tname0(pv->utype_noarray()), _tname0(utt)); }
          throw XUExec("_arr_el_get.2", _tname0(pv->utype()), ind);
        } catch (...) { if (cs == csLazy) { return T(); } throw; }
      }
    };
    template<class _> struct _arr_el_get<utUnity, _>
    {
      static inline unity F(const unity* pv, s_long ind, EConvStrength cs)
      {
        typedef unity T;
        try {
          const T* p = pv->pval<utUnity>(ind); if (p) { return *p; }
          unity temp; s_long res = pv->_Lel_get(ind, &temp);
          if (res == 1) { return temp; }
          if (res == -2) { throw XUTypeMismatch("_arr_el_get.3", _tname0(pv->utype_noarray()), _tname0(utUnity)); }
          throw XUExec("_arr_el_get.4", _tname0(pv->utype()), ind);
        } catch (...) { if (cs == csLazy) { return T(); } throw; }
      }
    };
    template<class Ts> void _x_asgvec(const std::vector<Ts>& src, bool keep_name)        { typedef typename trivconv_t<typename meta::nonc_t<Ts>::t>::t_target Td; enum { utt = reference_t<Td>::utt | utArray }; _storage st; if (_Lcreate(&st, utt, 0x1) > 0) { vec2_t<Td>* pv = reference_t<vec2_t<Td> >::deref(st, true); if ((pv->compatibility() > 0) && _utils::copy(*pv, src)) { void* _pn(0); if (keep_name && isNamed()) { _pn = _data.p2; _data.p2 = 0; } clear(); _data.p1 = st.p1; _data.p2 = _pn; ut = utt | xfPtr; return; } _Ldestroy(&st, utt, 0x1); } throw XUExec("_x_asgvec", "src: " + _tname0(utt)); }
    template<class Ts> void _x_asgvec(const vec2_t<Ts>& src, bool keep_name)        { typedef typename trivconv_t<typename meta::nonc_t<Ts>::t>::t_target Td; enum { utt = reference_t<Td>::utt | utArray }; _storage st; if (_Lcreate(&st, utt, 0x1) > 0) { vec2_t<Td>* pv = reference_t<vec2_t<Td> >::deref(st, true); if (pv->vec2_copy(src, false) == 1) { void* _pn(0); if (keep_name && isNamed()) { _pn = _data.p2; _data.p2 = 0; } clear(); _data.p1 = st.p1; _data.p2 = _pn; ut = utt | xfPtr; return; } _Ldestroy(&st, utt, 0x1); } throw XUExec("_x_asgvec", "src: " + _tname0(utt)); }
    template <class T> inline T& _rnonc()
    {
      enum { utt = reference_t<T, short>::utt };
      if (isObject())
      {
        if (pmsm == unity_common::ls_modsm) { return *checked_ptr<T>(reinterpret_cast<T*>(_drf_o()->prefo(typeid(T).name()))); }
          else { unity_common::__Pprefo pf = (unity_common::__Pprefo)pmsm(unity_common::msm_obj_prefo); if (!pf) { throw XUExec("_rnonc.1"); } return *checked_ptr<T>(reinterpret_cast<T*>(pf(_drf_o(), typeid(T).name()))); }
      }
      if (utt == utype()) { return *checked_ptr<T>(reference_t<T>::_drf_c(this)); }
      throw XUTypeMismatch("_rnonc", _tname0(utype()), _tname0(utt));
    }
  public:




    // Informational functions.

      // EUnityType
    inline s_long utype() const throw () { return ut & xutFull; }

      // EUnityType without utArray flag
    inline s_long utype_noarray() const throw () { return ut & utMaskNoArray; }

      // Returns type name of the contained object or one of predefined types,
      //  like
      // .Empty, .Int, .String, .Map,
      // X (typeid.name() of the contained object),
      // .Float[], .Unity[].
      //
      // If allowArrDim == true, arrays are with bound. indices, e. g.:
      // .Int[L..U], .String[L..U], .Unity[L..U].
    std::wstring tname(bool allowArrDim = false) const;

      // The set
      //    scalar, array, object, assoc. array (map or hash)
      //  covers all available types in a valid object.
      //  Empty is a scalar. Bool is represented by 0/non-0 char.
      // NOTE Tests are able to return false on this == 0.
    inline bool isScalar() const throw () { return this && utype() <= utString; }
    inline bool isArray() const throw () { return this && bool(ut & utArray); }
    inline bool isObject() const throw () { return this && utype() == utObject; }
    inline bool isAssoc() const throw () { if (!this) { return false; } s_long t = utype(); return t == utHash || t == utMap; }

      // Tests for particular types and values.
      // NOTE Tests are able to return false on this == 0.
    inline bool isEmpty() const throw () { return this && utype() == utEmpty; }
    inline bool isInt() const throw () { return this && utype() == utInt; }
    inline bool isFloat() const throw () { return this && utype() == utFloat; }
    inline bool isDate() const throw () { return this && utype() == utDate; }
    inline bool isString() const throw () { return this && utype() == utString; }
    inline bool isChar() const throw () { return this && utype() == utChar; }

    inline bool isMap() const throw () { return this && utype() == utMap; }
    inline bool isHash() const throw () { return this && utype() == utHash; }

    inline bool isBool() const throw () { return this && utype() == utChar; }
    inline bool isBoolTrue() const throw () { return this && utype() == utChar && *_drf<utChar>() != 0; } // true if *this contains char != 0
    inline bool isBoolFalse() const throw () { return this && utype() == utChar && *_drf<utChar>() == 0; } // true if *this contains char == 0

    inline bool isIntOrFloat() const throw () { return this && utype() == utInt && utype() == utFloat; }

    // Other tests.

      // true if the value is kept by pointer (dynamically allocated).
      // false if the value is kept as bytes within the object storage.
    inline bool isByPtr() const { return ut & xfPtr; }

      // true if the object has a name assigned.
    inline bool isNamed() const { return (ut & xfPtr) && _data.p2; }

      // true if the object is created in the current binary module.
    inline bool isLocal() const { return pmsm == unity_common::ls_modsm; }

      // Exact equality (values, arrays, strings, assoc. arrays, object pointers)
    inline bool operator==(const unity& x) const { return k_eq(x, 0); }
    inline bool operator!=(const unity& x) const { return !k_eq(x, 0); }

      // Flags for uniqueness and relation of map and hash keys
    static const s_long fkcmpNFloat = 0x2; // convert integer keys (utInt, utChar) to double
    static const s_long fkcmpSNocase = 0x1; // strings comparison and hashing are case insensitive (0 == binary comp.)
    static const s_long fkcmpRevOrder = 0x4; // reverse order in "<" comparison
    static const s_long fkcmpScalarize = 0x8; // do not use non-scalar keys, convert a copy to scalar and use it as key
    static const s_long _fkcmp_n = 16;
    static const s_long _fkcmp_mask = _fkcmp_n - 1;

      // Hash value.
      //  flags:
      //    fkcmpNFloat - for utChar, utInt, calc. hash value from val<utFloat>
      //    fkcmpSNocase - for utString calc. hash value from string in lowercase
    s_long k_hashf(s_long flags) const;

      // Equality and op. <.
      //  flags:
      //    fkcmpNFloat - for utChar, utInt, convert values to utFloat, then compare.
      //    fkcmpSNocase - for utString, case-insensitive comparison.
    bool k_eq(const unity& x, s_long flags) const;
    bool k_less(const unity& x, s_long flags) const;

      // Formatting + stream i/o.

    enum Eiofmt { iofmt_default = 0 };
    struct iofmt
    {
        // px (always != 0) represents the linked object during formatted output.
      const Eiofmt fmt; const unity* const px; iofmt(Eiofmt fmt_, const unity& x) : fmt(fmt_), px(&x) {}
      typedef hashx<const void*, s_long> t_h; t_h& _rho() const { if (!_ho) { _ho.create0(false); } return *_ho._pnonc_u(); } private: mutable cref_t<t_h> _ho;
    };

      // Create specific format type, linked with the current object.
    iofmt format(Eiofmt fmt = iofmt_default) const { return iofmt(fmt, *this); }

      // Write text representation of x to stream. See also operator<<(ostream...)
    static void write_fmt(std::ostream& s, const unity& x, const unity::iofmt& f, bool bsp, s_long nsp = 2);
    static void write_fmt(std::wostream& s, const unity& x, const unity::iofmt& f, bool bsp, s_long nsp = 2);

      // Underlying map, hash access. Objects are implementation-dependent!
    t_map* _pmap() const { return isMap() ? _m() : 0; }
    t_hash* _phash() const { return isHash() ? _h() : 0; }


    // Construction.

    unity()        { ut = utEmpty; pmsm = unity_common::ls_modsm; _data.p1 = 0; _data.p2 = 0; }

    unity(const meta::s_ll& x)        { ut = utInt; pmsm = unity_common::ls_modsm; _data.p2 = 0; *_drf<utInt>() = x; }
    unity(const double& x)        { ut = utFloat; pmsm = unity_common::ls_modsm; _data.p2 = 0; *_drf<utFloat>() = x; }
    unity(const _unitychar& x)        { ut = utChar; pmsm = unity_common::ls_modsm; _data.p1 = 0; _data.p2 = 0; *_drf<utChar>() = x; }
    unity(const _unitydate& x)        { ut = utEmpty; pmsm = unity_common::ls_modsm; _data.p1 = 0; _data.p2 = 0; *this = x; }

    unity(const unity& x)        { ut = utEmpty; pmsm = unity_common::ls_modsm; _data.p1 = 0; _data.p2 = 0; *this = x; }
    unity(const std::wstring& x)        { ut = utEmpty; pmsm = unity_common::ls_modsm; _data.p1 = 0; _data.p2 = 0; *this = x; }

    template<class T> unity(const T& x)        { ut = utEmpty; pmsm = unity_common::ls_modsm; _data.p1 = 0; _data.p2 = 0; *this = x; }

      // NOTE unity().set_obj<Obj, Interfaces> is more functional (allows setting specific interfaces).
    template<class Obj> unity(Obj& x, bool isStrongRef)        { ut = utEmpty; pmsm = unity_common::ls_modsm; _data.p1 = 0; _data.p2 = 0; set_obj<Obj, o_interfaces_top<Obj> >(x, isStrongRef); }

      // Create a copy of vector with specific base index.
      //    a) on on_calm_result: *this contains a copy of x.
      //    b) on failure: XUExec is generated. *this is set to utEmpty.
      //  NOTE vector and vec2_t with base index 0 can also be created or assigned with 1-argument call.
    template<class T> unity(const std::vector<T>& x, s_long arrlb)        { ut = utEmpty; pmsm = unity_common::ls_modsm; _data.p1 = 0; _data.p2 = 0; *this = x; arrlb_(arrlb); }

      // Create and own a copy of reference object.
    template<class T, class L> unity(const cref_t<T, L>& x)        { typedef cref_t<T, L> Obj; ut = utEmpty; pmsm = unity_common::ls_modsm; _data.p1 = 0; _data.p2 = 0; Obj* p = new Obj(x); try { set_obj<Obj, o_interfaces_top<Obj> >(*p, true); } catch (...) { try { delete p; } catch (...) {} throw XUExec("unity(const cref_t<T, L>&)", "set_obj failed"); } }

    ~unity() throw();



    // Clear / assign.
    //  NOTE Ops. in this group do not preserve the current object name.
    //    Assigning dest = src will copy both the name and value of src.
    //    Others just remove the current name.
    //    See also u_assign, u_clear.

      // Clears and sets to utEmpty.
    void clear() throw();

      // Destroys previous contents and initializes the storage
      //    as an empty value created by the current binary module.
    void recreate() throw() { this->~unity(); new (this) unity(); }

      // Deleting the current object (if it was dynamically allocated).
      //    true - the object is deleted successfully.
      //    false - the object deleted, but generated an exception during op. delete.
    bool unity_delete() throw() { return _Ldelete_this() > 0; }

    unity& operator=(const unity& src) { _x_asg(src, 0); return *this; }
    unity& operator=(const std::wstring&);

    unity& operator=(const meta::s_ll& src) { clear(); ut = utInt; *_drf<utInt>() = src; return *this; }
    unity& operator=(const double& src) { clear(); ut = utFloat; *_drf<utFloat>() = src; return *this; }
    unity& operator=(const _unitydate& src) { clear(); ut = utDate; *_drf<utDate>() = src; return *this; }
    unity& operator=(const _unitychar& src) { clear(); ut = utChar; *_drf<utChar>() = src; return *this; }

    template<class T> unity& operator=(const T& x)        { return (*this = trivconv_t<typename meta::nonc_t<T>::t>::F(x)); }

      // Array assignment.
      //  a) on on_calm_result, the object contains a copy of x.
      //  b) on failure, XUExec is generated. *this is not changed.
    template<class Ts> unity& operator=(const std::vector<Ts>& src) { _x_asgvec(src, false); return *this; }
    template<class Ts> unity& operator=(const vec2_t<Ts>& src) { _x_asgvec(src, false); return *this; }




      // Get pointer to scalar, array, or array element.
      //  MODE This function does not generate exceptions or modify the object.
      //    To get non-0 pointer,
      //    1) utype() must exactly match the given type.
      //    2) In pval(ind), ind must be valid arrlb-based array index.
      //    In all other cases, pval returns 0.
      //  For pval<utt>(ind), the given utt may be either array type or its element type.
      //    ind is arrlb-based.
      //  NOTE The returned pointer may be either checked or safely dereferenced (throws XUExec on 0).
    template <int utt> inline checked_ptr<typename valtype_t<utt>::t>
      pval()                { return utt == utype() ? _drf<utt>() : 0; }
    template <class T> inline checked_ptr<T>
      pval()                { enum { utt = reference_t<T>::utt }; return utt == utype() ? _drf<utt>() : 0; }
    template <int utt> inline checked_ptr<typename valtype_t<utt & utMaskNoArray>::t>
      pval(s_long ind)                { typename valtype_t<utt | utArray>::t* p = pval<utt | utArray>(); return p ? p->pval(ind) : 0; }
    template <class T> inline checked_ptr<T>
      pval(s_long ind)                { enum { utt = reference_t<T>::utt }; typename valtype_t<utt | utArray>::t* p = pval<utt | utArray>(); return p ? p->pval(ind) : 0; }
    template <int utt> inline checked_ptr<const typename valtype_t<utt>::t>
      pval() const                { return utt == utype() ? _drf<utt>() : 0; }
    template <class T> inline checked_ptr<const T>
      pval() const                { enum { utt = reference_t<T>::utt }; return utt == utype() ? _drf<utt>() : 0; }
    template <int utt> inline checked_ptr<const typename valtype_t<utt & utMaskNoArray>::t>
      pval(s_long ind) const                { const typename valtype_t<utt | utArray>::t* p = pval<utt | utArray>(); return p ? p->pval(ind) : 0; }
    template <class T> inline checked_ptr<const T>
      pval(s_long ind) const                { enum { utt = reference_t<T>::utt }; const typename valtype_t<utt | utArray>::t* p = pval<utt | utArray>(); return p ? p->pval(ind) : 0; }



      // Get reference to scalar, array, or array element.
      //  MODE This function does not modify the object.
      //  Arguments are same as in pval.
      // Feature of ref<T>(): if unity object references a) valid object b) cref_t to such object,
      //    a reference to that object is returned. Object type is checked by name.
      //    NOTE The reference to valid object returned for both local objects and objects from other modules
      //      (same as cpvoid()).
      //      The client is responsible for compatibility checks.
      //  Possible exceptions:
      //    XUTypeMismatch if utype() is not the same as given type.
      //    XUExec a) ind is out of range, b) failed to return refrence to contained object.
    template <class T> inline T&        ref()                { return _rnonc<T>(); }
    template <class T> inline const T&        ref() const                { return _rnonc<T>(); }
    template <int utt> inline typename valtype_t<utt>::t&        ref()                { typedef typename valtype_t<utt>::t T; if (utt != utype()) { throw XUTypeMismatch("ref", _tname0(utype()), _tname0(utt)); } return *checked_ptr<T>(_drf<utt>()); }
    template <int utt> inline const typename valtype_t<utt>::t&        ref() const                { typedef typename valtype_t<utt>::t T; if (utt != utype()) { throw XUTypeMismatch("ref", _tname0(utype()), _tname0(utt)); } return *checked_ptr<const T>(_drf<utt>()); }

    template <class T> inline T&        ref(s_long ind)                { enum { utt = reference_t<T>::utt }; return *ref<utt | utArray>().pc(ind); }
    template <class T> inline const T&        ref(s_long ind) const                { enum { utt = reference_t<T>::utt }; return *ref<utt | utArray>().pc(ind); }
    template <int utt> inline typename valtype_t<utt & utMaskNoArray>::t&        ref(s_long ind)                { return *ref<utt | utArray>().pc(ind); }
    template <int utt> inline const typename valtype_t<utt & utMaskNoArray>::t&        ref(s_long ind) const                { return *ref<utt | utArray>().pc(ind); }



      // Get const reference to scalar, array, array element, or the default value.
      //    On type mismatch or array range error,
      //    a default value is created:
      //      a) T(),
      //      b) T(x_dflt)
      //      c) crefx<utUnity>(ind)) only: unity(ref(ind)) for any scalar type
      //    (The value is created dynamically and owned by the reference object and its copies.)
      //  MODE This function does not modify the object.
      //  Possible exceptions: (rare) XUExec on alloc. error for the default value creation / copy.
    template <int utt> inline cref_t<typename valtype_t<utt>::t>
      cref() const                {
        typedef typename valtype_t<utt>::t T;
        const T* p = pval<utt>(); if (p) { return cref_t<T>(*p, false, false); }
        if (utt == utString) { if (utype() == utString) { try { cref_t<T> r; r.create1(false, val<utt>()); return r; } catch (...) {} } }
        try { cref_t<T> r; r.create0(false); return r; } catch (...) {}
        throw XUExec("cref"); }
    template <int utt> inline cref_t<typename valtype_t<utt>::t>
      cref(const typename valtype_t<utt>::t& x_dflt) const                {
        typedef typename valtype_t<utt>::t T;
        const T* p = pval<utt>(); if (p) { return cref_t<T>(*p, false, false); }
        if (utt == utString) { if (utype() == utString) { try { cref_t<T> r; r.create1(false, val<utt>()); return r; } catch (...) {} } }
        try { cref_t<T> r; r.create1(false, x_dflt); return r; } catch (...) {}
        throw XUExec("cref(x_dflt)"); }
    template <int utt> inline cref_t<typename valtype_t<utt>::t>
      cref(s_long ind, const typename valtype_t<utt>::t& x_dflt) const                {
        typedef typename valtype_t<utt>::t T;
        const T* p = pval<utt>(ind); if (p) { return cref_t<T>(*p, false, false); }
        if (utt == utString) { if (utype() == utString) { try { cref_t<T> r; r.create1(false, val<utt>(ind)); return r; } catch (...) {} } }
        try { cref_t<T> r; r.create1(false, x_dflt); return r; } catch (...) {}
        throw XUExec("cref(ind, x_dflt)"); }
    template <int utt> inline cref_t<typename valtype_t<utt>::t>
      crefx(s_long ind) const        {
          typedef typename valtype_t<utt>::t T;
          const T* p = pval<utt>(ind); if (p) { return cref_t<T>(*p, false, false); }
          if (utt == utString) { if (utype() == utString) { try { cref_t<T> r; r.create1(false, val<utt>(ind)); return r; } catch (...) {} } }
          if (utt == utUnity && utype() != utUnityArray) { try { cref_t<T> r; reinterpret_cast<cref_t<unity>*>(&r)->create1(false, _arr_el_get<utUnity>::F(this, ind, csLazy)); return r; } catch (...) {} }
          try { cref_t<T> r; r.create0(false); return r; } catch (...) {}
          throw XUExec("crefx"); }



      // Get reference to scalar or array with automatic type conversion.
      //  MODE This function may 1) generate exceptions, 2) modify the object. Modification is transactional.
      //    a) if the internal value matches the given type, returns a reference to the value.
      //      If the internal value is of another type, tries to convert it.
      //      On accessing an array element, the array must already exist,
      //      and be of either target type or utUnity. In case of utUnity,
      //      the contained value type converted to the required type automatically.
      //    b) on successful conversion, returns a reference to the value.
      //    c) if conversion has failed, XUConvFailed is generated. *this is not changed.
      //    d) if ind is out of range, XUExec is generated.
      // NOTE Conversion removes the object name.
      // NOTE Array, created to replace non-array,  is 1-based by default.
    template <int utt> inline typename valtype_t<utt>::t&
      rx()                { if (utt == utype()) { return *pval<utt>(); } _x_cnv_this(csNormal, utt, false); if (utt == utype()) { return *pval<utt>(); } throw XUConvFailed("rx", _tname0(utype()), _tname0(utt)); }
    template <class T> inline T&
      rx()                { enum { utt = reference_t<T>::utt }; if (utt == utype()) { return *pval<utt>(); } _x_cnv_this(csNormal, utt, false); if (utt == utype()) { return *pval<utt>(); } throw XUConvFailed("rx", _tname0(utype()), _tname0(utt)); }
    template <int utt> inline typename valtype_t<utt & utMaskNoArray>::t&
      rx(s_long ind)
        {
          enum { utta = utt | utArray, utte = utt & utMaskNoArray }; typedef typename valtype_t<utte>::t T; enum { __z1 = sizeof(T) };
          const s_long uttx = utype(); if (uttx == utta) { return ref<utta>(ind); } else if (uttx == utUnityArray) { return _rx_el<utte>::F(ref<utUnity>(ind)); }
          throw XUConvFailed("rx(ind)", _tname0(utype_noarray()), _tname0(utt));
        }
    template <class T> inline T&
      rx(s_long ind)        { return rx<reference_t<T>::utt>(ind); }



      // Ensure that the value in *this is of the given type.
      //  MODES
      //    1) csStrict -- does not modify the object.
      //      If the given type != current object type, XUTypeMismatch is generated.
      //    2) csNormal -- transactional.
      //      a) The object is/converted to the given type with all data.
      //      b) The object is not changed, XUConvFailed is generated.
      //    3) csLazy -- non-transactional. No exceptions. The object
      //      a) is/converted to the given type with all data,
      //      b) on allocation error, initialized by default of the given type (the data is lost),
      //      c) cleared (utEmpty).
      // NOTE Conversion removes the object name.
      // NOTE Array, created to replace non-array,  is 1-based by default.
    template <int utt> inline bool
      conv(EConvStrength cs = csNormal)                { if (utt == utype()) { return true; } _x_cnv_this(cs, utt, false); return utt == utype(); }
    template <class T> inline bool
      conv(EConvStrength cs = csNormal)                { enum { utt = reference_t<T>::utt }; if (utt == utype()) { return true; } _x_cnv_this(cs, utt, false); return utt == utype(); }


      // Copy the internal value, possibly with conversion to the target type.
      //    On accessing an array element, the array must already exist.
      //    utt may be
      //      a) scalar type -- approp. scalar is returned.
      //      b) array type -- vec2_t is returned.
      //      c) (array type | fvalVector) -- std vector is returned.
      //    b and c are useful when the current object is utUnityArray, whose elements
      //    are also arrays.
      //  MODES
      //    1) csStrict
      //      a) returns a copy of the internal value.
      //      b) if the given type != current object type, XUTypeMismatch is generated.
      //    2) csNormal
      //      a) returns a copy of the internal value.
      //      b) if the given type != current object type, performs copy + conversion.
      //      c) if copying or conversion fails, generates XUConvFailed.
      //    3) csLazy
      //      a) returns a copy of the internal value.
      //      b) on allocation error, returns the default value of the given type,
      //      c) on alloc. error even on the default value, generates XUConvFailed.
    template<int utt> inline typename _valtype2_t<utt>::t
      val(EConvStrength cs = csNormal) const
        {
          typedef typename _valtype2_t<utt>::t T;
          if (utt == utype())
          {
            if (utt < utString) { return _refc_el<utt>::F(*this); }
              // compatibility may be 0, because the array will be copied.
            if (isArray()) { if (!(_data.p1 && (_drf_q()->compatibility() >= 0))) { goto lCnvF; } }
            if (!(utt == utString && pmsm != unity_common::ls_modsm)) { try { return _refc_el<utt>::F(*this); } catch (...) {} goto lCnvF; }
          }
          if (true) { T retval; if (_x_cnv_val(cs, utt, (utt & fvalVector) ? 0x1 : 0, &retval)) { return retval; } }
        lCnvF:
          if (cs == csLazy) { try { return T(); } catch (...) {} }
          throw XUConvFailed("val", _tname0(utype()), _tname0(utt));
        }
    template <class T> inline T
      val(EConvStrength cs = csNormal) const                { return val<reference_t<T>::utt>(cs); }
    template<int utt> inline typename _valtype2_t<utt>::t
      val(s_long ind, EConvStrength cs = csNormal) const        { return _arr_el_get<utt>::F(this, ind, cs); }
    template <class T> inline T
      val(s_long ind, EConvStrength cs = csNormal) const        { return _arr_el_get<reference_t<T>::utt>::F(this, ind, cs); }

      // NOTE vstr() values returned may be formatted differently than val<utString>().
    std::wstring vstr() const;
      std::wstring vstr(s_long ind) const;

    std::string vcstr() const;
      std::string vcstr(s_long ind) const;

    inline meta::s_ll vint() const { return val<utInt>(csNormal); }
      inline meta::s_ll vint(s_long ind) const { return _arr_el_get<utInt>::F(this, ind, csNormal); }

      // High doubleword.
    inline s_long vint_h() const { return s_long(val<utInt>(csNormal) >> 32); }
      inline s_long vint_h(s_long ind) const { return s_long(_arr_el_get<utInt>::F(this, ind, csNormal) >> 32); }

      // Low doubleword.
    inline s_long vint_l() const { return s_long(val<utInt>(csNormal)); }
      inline s_long vint_l(s_long ind) const { return s_long(_arr_el_get<utInt>::F(this, ind, csNormal)); }

    inline double vfp() const { return val<utFloat>(csNormal); }
      inline double vfp(s_long ind) const { return _arr_el_get<utFloat>::F(this, ind, csNormal); }

    _fls75 vflstr() const throw(); // returns short representation of a value





      // Array initialization. T is the type of element. utt is the type of element or array.
      //  If the array is already of the required type, it is simply cleared.
      //  MODES
      //    1) csStrict -- crears the current array, or generates XUTypeMismatch.
      //    2) csNormal -- transactional. Clears or initializes / the object to the target type. Otherwise generates an XUConvFailed.
      //    3) csLazy -- non-transactional. Clears or initializes / the object to the target type. On alloc. error, sets utEmpty and generates XUConvFailed.
      // arr_init<utt>:
      //  Both scalar and array type is accepted. Array element type is resolved automatically.
    template<int utt> vec2_t<typename valtype_t<utt & utMaskNoArray>::t>&
      arr_init(s_long arrlb, EConvStrength cs = csNormal)        { return arr_init<typename valtype_t<utt & utMaskNoArray>::t>(arrlb, cs); }
    template<class T> vec2_t<T>&
      arr_init(s_long arrlb, EConvStrength cs = csNormal)
      {
        enum { utt = reference_t<T>::utt | utArray };
        if (utt == utype())
        {
          vec2_t<T>* pv = _drf<utt>();
          if ((pv->compatibility() >= 0)) { pv->clear(); pv->vec2_set_nbase(arrlb); return *pv; }
          throw XUConvFailed("arr_init.1", "-", _tname0(utt), "incompat.");
        }
        else
        {
          if (cs == csStrict) { throw XUTypeMismatch("arr_init.2", _tname0(utype()), _tname0(utt)); }
          _storage st;
          if (_Lcreate(&st, utt, 0x1) > 0)
          {
            vec2_t<T>* pv = reference_t<vec2_t<T> >::deref(st, true);
            if ((pv->compatibility() >= 0)) { pv->vec2_set_nbase(arrlb); clear(); _data = st; ut = utt | xfPtr; return *pv; }
            _Ldestroy(&st, utt, 0x1);
          }
          throw XUConvFailed("arr_init.2", _tname0(utype()), _tname0(utt));
        }
      }


      // Array size and boundaries.
      //  If *this does not contain an array, XUTypeMismatch is genereated.
    s_long arrlb() const; // lower bound (first existing element index).
    s_long arrub() const; // upper bound (largest existing element index).
    s_long arrsz() const; // the number of elements
    unity& arrlb_(s_long); // sets the new lower boundary (upper boundary shifts as well, because the array is not resized)
    unity& arrsz_(s_long); // sets the new (>=0) array size; lower bnd. is preserved
    unity& arrub_(s_long); // setting the new upper bnd., which may resize the array (if ==arrlb(), the array is cleared)


      // Constructs 0-based utUnityArray from copies of arbitrary values (up to 30 values).
      //  Returns *this.
    struct arg; static const arg& _noarg();
    unity& array(const arg& x1 = _noarg(), const arg& x2 = _noarg(), const arg& x3 = _noarg(), const arg& x4 = _noarg(), const arg& x5 = _noarg(), const arg& x6 = _noarg(), const arg& x7 = _noarg(), const arg& x8 = _noarg(), const arg& x9 = _noarg(), const arg& x10 = _noarg(), const arg& x11 = _noarg(), const arg& x12 = _noarg(), const arg& x13 = _noarg(), const arg& x14 = _noarg(), const arg& x15 = _noarg(), const arg& x16 = _noarg(), const arg& x17 = _noarg(), const arg& x18 = _noarg(), const arg& x19 = _noarg(), const arg& x20 = _noarg(), const arg& x21 = _noarg(), const arg& x22 = _noarg(), const arg& x23 = _noarg(), const arg& x24 = _noarg(), const arg& x25 = _noarg(), const arg& x26 = _noarg(), const arg& x27 = _noarg(), const arg& x28 = _noarg(), const arg& x29 = _noarg(), const arg& x30 = _noarg() );

      // Assigns ind-th elem. of the internal array = x, with conversion as necessary.
      //  NOTE Trivial conversions are allowed even on cs == csStrict.
      //  Returns: true on on_calm_result, exception on failure, false on failure in csLazy.
    template<class Ts> bool arr_set(s_long ind, const Ts& x, EConvStrength cs = csNormal)
    {
      typedef typename trivconv_t<typename meta::nonc_t<Ts>::t>::t_target Td; enum { uttd = reference_t<Td>::utt };
      try
      {
        if (pmsm == unity_common::ls_modsm && uttd == utype_noarray()) { *pval<uttd>(ind) = trivconv_t<typename meta::nonc_t<Ts>::t>::F(x); return true; }
        if (cs == csStrict) { if (uttd != utype_noarray()) { throw XUTypeMismatch("arr_set.4", _tname0(uttd), _tname0(utype_noarray())); } }
        s_long res = _Lel_set(ind, x); if (res == 1) { return true; } if (res == -2) { throw XUTypeMismatch("arr_set.1", typeid(x).name(), _tname0(utype())); }
        throw XUExec("arr_set.2", _tname0(utype()), ind);
      }
      catch (_XUBase&) { if (cs == csLazy) { return false; } throw; }
      catch (...) { if (cs == csLazy) { return false; } throw XUExec("arr_set.3", _tname0(utype()), ind); }
    }

      // Appends x to the internal array, with conversion as necessary.
      //  NOTE Trivial conversions are allowed even on cs == csStrict.
      //  Returns: true on on_calm_result, exception on failure, false on failure in csLazy.
    template<class Ts> bool arr_append(const Ts& x, EConvStrength cs = csNormal)
    {
      typedef typename trivconv_t<typename meta::nonc_t<Ts>::t>::t_target Td; enum { uttd = reference_t<Td>::utt };
      try
      {
        if (pmsm == unity_common::ls_modsm && uttd == utype_noarray()) { if (pval<uttd | utArray>()->el_append(trivconv_t<typename meta::nonc_t<Ts>::t>::F(x))) { return true; } throw XUExec("arr_append.5", _tname0(utype())); }
        if (cs == csStrict) { if (uttd != utype_noarray()) { throw XUTypeMismatch("arr_append.4", _tname0(uttd), _tname0(utype_noarray())); } }
        s_long res = _Lel_append(x); if (res == 1) { return true; } if (res == -2) { throw XUTypeMismatch("arr_append.1", typeid(x).name(), _tname0(utype())); }
        throw XUExec("arr_append.2", _tname0(utype()));
      }
      catch (_XUBase&) { if (cs == csLazy) { return false; } throw; }
      catch (...) { if (cs == csLazy) { return false; } throw XUExec("arr_append.3", _tname0(utype())); }
    }

      // Inserts m or removes -m (when m is negative) elements.
      //    On insertion, ind is the place of insertion, [lb..ub+1]. If x is not specified, a default value will be inserted.
      //    On removal, elements to be removed are [ind..ind-m-1]. All in this range must be valid element indices.
      //  Returns: true on on_calm_result, exception on failure, false on failure in csLazy.
    bool arr_insrem(s_long ind, s_long m, EConvStrength cs = csNormal)
    {
      try
      {
        s_long res = _Lel_insrem(ind, m); if (res == 1) { return true; } if (res == -2) { throw XUExec("arr_insrem.6", _tname0(utype())); }
        throw XUExec("arr_insrem.7", _tname0(utype()));
      }
      catch (_XUBase&) { if (cs == csLazy) { return false; } throw; }
      catch (...) { if (cs == csLazy) { return false; } throw XUExec("arr_insrem.3", _tname0(utype())); }
    }
    template<class Ts> bool arr_insrem(s_long ind, s_long m, const Ts& x, EConvStrength cs = csNormal)
    {
      typedef typename trivconv_t<typename meta::nonc_t<Ts>::t>::t_target Td; enum { uttd = reference_t<Td>::utt };
      if (cs == csStrict) { if (uttd != utype_noarray()) { throw XUTypeMismatch("arr_insrem.4", _tname0(utype()), _tname0(utype_noarray())); } }
      try
      {
        if (pmsm == unity_common::ls_modsm && uttd == utype_noarray())
        {
          s_long res;
          _yk_reg typename unity::valtype_t<uttd | utArray>::t& rv = ref<uttd | utArray>();
          if (m > 0) { if (m == 1) { res = rv.el_insert_1(ind, trivconv_t<typename meta::nonc_t<Ts>::t>::F(x)); } else { res = rv.el_insert_ml(ind, m, trivconv_t<typename meta::nonc_t<Ts>::t>::F(x)); } }
            else { if (m == -1) { res = rv.el_remove_1(ind, false); } else { res = rv.el_remove_ml(ind, -m); } }
          if (res >= 0) { return true; }
          throw XUExec("arr_insrem.5", _tname0(utype()));
        }
        s_long res = _Lel_insrem(ind, m, x); if (res == 1) { return true; } if (res == -2) { throw XUTypeMismatch("arr_insrem.1", typeid(x).name(), _tname0(utype())); }
        throw XUExec("arr_insrem.2", _tname0(utype()));
      }
      catch (_XUBase&) { if (cs == csLazy) { return false; } throw; }
      catch (...) { if (cs == csLazy) { return false; } throw XUExec("arr_insrem.3", _tname0(utype())); }
    }



    // SPECIFIC FUNCTIONS
    // u_*, sc*, ua*, map*, hash*, [ ]

    //==Common==

      // Returns a scalar value, copied from the current object.
      //    utype of the copy is <= stString. Special cases:
      //    a) array - "?array/" + array type is returned.
      //    b) object - "?object/" + typeid name is returned.
      //    c) object of type unity - "?unity/" + its u_name is returned.
      //    d) any other value "?value/" + its type name is returned.
      //  NOTE On cs == csLazy, the function does not throw exceptions
      //    even on error (just returns an empty value).
      //  NOTE result / destination object has no name.
    inline unity u_key(EConvStrength cs = csNormal) const { unity u; u_key_get(u, cs); return u; }
      const unity& u_key_get(unity& dest, EConvStrength cs = csNormal) const; // returns *this

      // If this object has a name (scalar), its copy is returned.
      //  Otherwise, an empty object is returned.
      //  On copy error, XUExec is generated.
    unity u_name() const { if (isNamed()) { return unity(*_drf_name()); } return unity(); }

      // Sets the new object name. Before assignment, the name is scalarized by u_key.
    unity& u_name_set(const unity& name)       { s_long res = _Lu_set_name(&name); if (res > 0) { return *this; } throw XUExec("u_name_set"); }

      // Similar to clear() + conv<utt>(cs), but keeps object name.
      //    If utt == -1, the object type does not change, except utype() == utObject.
      //    utt or utype() utObject removes the object reference, and sets *this to utEmpty.
      // flags:
      //    0x1 - clear array/map/hashlist in full (array base index = 1, assoc. array key fn. flags = 0, etc.).
      //      If flag is not set, only elements are removed, no container parameters changed.
      //      For assoc. array flags meaning, see static const s_long fkcmp*.
      // NOTE Unlike clear(), u_clear() may generate exceptions.
    unity& u_clear(s_long utt = -1, s_long flags = 0x1, EConvStrength cs = csNormal);

      // Same as copy constructor, but keeps object name.
      //  NOTE Returning value may cause double copying. To avoid that, call u_assign.
    unity u_copy() const        { unity x; x._x_asg(*this, 0x1); return x; }

      // Same as op.=, but keeps object name.
    unity& u_assign(const unity& x)        { _x_asg(x, 0x1); return *this; }
    template<class T> unity&
      u_assign(const T& x)
      {
        typedef typename meta::assert<meta::same_t<T, unity>::result>::t_false __check; enum { __z1 = sizeof(__check) };
        if (isNamed())
        {
          if (pmsm != unity_common::ls_modsm) { _x_asg(x, 0x1); return *this; }
          unity x2(x); if (!x2.isByPtr()) { _x_asg(x2, 0x1); return *this; }
            void* _pn = (ut & xfPtr) ? _data.p2 : 0; if (_pn) { _data.p2 = 0; }
          clear(); _data.p1 = x2._data.p1; ut = x2.utype() | xfPtr; x2._data.p1 = 0; x2.ut = utEmpty; return *this;
            if (_pn) { _data.p2 = _pn; }
        }
        else { return (*this = x); }
        return *this;
      }

      // Same as rx(), but keeps object name.
    template <int utt> inline typename valtype_t<utt>::t&
      u_rx()                { if (utt == utype()) { return *pval<utt>(); } _x_cnv_this(csNormal, utt, true); if (utt == utype()) { return *pval<utt>(); } throw XUConvFailed("rx", _tname0(utype()), _tname0(utt)); }
    template <class T> inline T&
      u_rx()                { enum { utt = reference_t<T>::utt }; if (utt == utype()) { return *pval<utt>(); } _x_cnv_this(csNormal, utt, true); if (utt == utype()) { return *pval<utt>(); } throw XUConvFailed("rx", _tname0(utype()), _tname0(utt)); }

      // Same as conv(), but keeps object name.
    template <int utt> inline bool
      u_conv(EConvStrength cs = csNormal)                { if (utt == utype()) { return true; } _x_cnv_this(cs, utt, true); return utype() == utt; }
    template <class T> inline bool
      u_conv(EConvStrength cs = csNormal)                { enum { utt = reference_t<T>::utt }; if (utt == utype()) { return true; } _x_cnv_this(cs, utt, true); return utt == utype(); }


    //==Scalar==

    inline bool sc_empty() const        { return isEmpty() || !isScalar(); }
    inline unity& sc_ensure()        { _ensure_sc(); return *this; }
    inline unity& sc_clear()        { if (!isEmpty()) { u_clear(utEmpty); } return *this; }


    //==Array==

      // By default, all ua_*, except const, automatically create 1-based utUnityArray instead of non-array.
      //    ua, ua_val, ua_set automatically add dflt.-constructed elements to front or back of the array
      //    to reach the specified index.

    inline unity& ua_ensure()        { if (!isArray()) { u_clear(utUnityArray); } return *this; }
    inline unity& ua_clear(bool full)        { return u_clear(isArray() ? -1 : utUnityArray, full ? 0x1 : 0); } // true resets all, false only removes elements
    template<class Ts> unity& ua_assign(const std::vector<Ts>& src) { _x_asgvec(src, true); return *this; }
    template<class Ts> unity& ua_assign(const vec2_t<Ts>& src) { _x_asgvec(src, true); return *this; }

    bool ua_has(s_long ind) const;
    bool ua_canref(s_long ind) const;
    bool ua_empty(s_long ind) const;

    unity& ua(s_long ind);
    unity& ua_first();
    unity& ua_last();
    unity ua_val(s_long ind);
    unity ua_val_first();
    unity ua_val_last();
    unity& ua_set(s_long ind, const unity& v); // returns *this
    unity& ua_append(const unity& v); // returns *this
    s_long uaS();
    s_long uaLb();
    s_long uaUb();
    unity& uaS_set(s_long new_S);
    unity& uaS_set(s_long new_S, const unity& v);
    unity& ua_resize(s_long ind, s_long m);
    unity& ua_resize(s_long ind, s_long m, const unity& v);
    unity& uaLb_set(s_long new_L);
    unity& ua_fill(s_long utt, const unity& v = unity()); // -"-; creates new array with same bounds as existing, + fills with v; on err. gen. exc., the object is not changed


    //==Map==

    inline unity& map_ensure() { _ensure_m(); return *this; }
    inline unity& map_clear(bool full) { return u_clear(utMap, full ? 0x1 : 0); } // true resets all, false only removes elements
    inline unity map_keys(s_long lb)        { unity u; map_keys_get(u, lb); return u; }
      unity& map_keys_get(unity& dest, s_long lb);  // returns *this
    inline unity map_values(s_long lb)        { unity u; map_values_get(u, lb); return u; }
      unity& map_values_get(unity& dest, s_long lb);  // returns *this
    s_long mapFlags();
    void mapFlags_set(s_long fk_reset, s_long fk_set);

    bool map_empty(const unity& k) const;
    bool map_locate(const unity& k, bool insert = false);
    inline s_long map_noel() const { return hashx_common::no_elem; }
    bool map_locate(const unity& k, bool insert, s_long& ind); // ind == map_noel() if not found on insert == false
    unity& map(const unity& k);
      // Returns: true if k was inserted, false if it existed.
    bool map_append(const unity& k, const unity& v, bool keep_first = false);
    bool map_del(const unity& k);

    s_long mapS();
    unity& mapS_set(s_long new_S);

      // In mapi*, ind is 1-based. Valid inds are [1..mapS()].
      //  NOTE Key, value functions genereate an exception if ind is out of range.
    bool mapi_has(s_long ind) const;
    bool mapi_empty(s_long ind) const;
    const unity& mapi_key(s_long ind);
    unity& mapi(s_long ind);
    unity& mapi_del(s_long ind);




    //==Hashlist==

    inline unity& hash_ensure()        { _ensure_h(); return *this; }
    inline unity& hash_clear(bool full)        { return u_clear(utHash, full ? 0x1 : 0); }  // true resets all, false only removes elements
    unity hash_keys(s_long lb)        { unity u; hash_keys_get(u, lb); return u; }
      unity& hash_keys_get(unity& dest, s_long lb);  // returns *this
    unity hash_values(s_long lb)        { unity u; hash_values_get(u, lb); return u; }
      unity& hash_values_get(unity& dest, s_long lb);  // returns *this
      // NOTE fkcmpRevOrder is ignored by hashlist object.
    s_long hashFlags();
    void hashFlags_set(s_long fk_reset, s_long fk_set);

    bool hash_empty(const unity& k) const;
    bool hash_locate(const unity& k, bool insert = false);
    unity& hash(const unity& k);
      // Returns: true if k was inserted, false if it existed.
    bool hash_set(const unity& k, const unity& v, bool keep_first = false);
    bool hash_del(const unity& k);

    s_long hashS();
    s_long hashS_c() const;

      // In hashi*, ind is 1-based. Valid inds are [1..hashS()].
      //  NOTE Key, value functions genereate an exception if ind is out of range.
    bool hashi_has(s_long ind) const;
    bool hashi_empty(s_long ind) const;
    const unity& hashi_key(s_long ind);
    const unity& hashi_key_c(s_long ind) const;
    unity& hashi(s_long ind);
    const unity& hashi_c(s_long ind) const;

      // In hashl, pos is 0-based.
      //  Next, prev.: valid pos is (hashl_noel(),  [0..hashS()-1]). On invalid pos, hashl_noel() will be returned.
      //  Key, value: valid pos. is [0..hashS()-1].On invalid pos, an exception is generated.
      // Negative hashl_noel() serves as before-beginning and after-end list pos.
      // next/prev fns. generate list order, starting from hashl_noel().
    inline s_long hashl_noel() const { return hashx_common::no_elem; }
    inline s_long hashl_first() { return hashl_next(hashl_noel()); }
    inline s_long hashl_last() { return hashl_prev(hashl_noel()); }
    s_long hashl_next(s_long pos);
    s_long hashl_prev(s_long pos);
    const unity& hashl_key(s_long pos);
    unity& hashl(s_long ind);



    //==Map, Hash, Array elem. access==

      // u_has returns true if the inner value is the container of one of the specified types
      //    (tt is a combination of flags: 1 <=> utUnityArray, 2 <=> utMap, 4 <=> utHash, 8: allow forwarding to the contained unity object.)
    bool u_has(const unity& ki, s_long tt) const;
      // operator[] returns, if found type/key/index match, one of the following:
      //    a) ref. to ki-th array element,
      //    b) map(ki),
      //    c) hash(ki),
      //    d) contained unity object [ki].
      //  Otherwise, XUExec is generated.
    unity& operator[] (const unity& ki);
    const unity& operator[] (const unity& ki) const { return const_cast<unity*>(this)->operator [](ki); }

      // Keys in the list should be in paramline array format (first "|" or "=|" is optional).
      //  mha is arbitrary tree of maps, hashes, arrays.
      //  NOTE forced == true: if key (keylist element) is a non-numeric key, but, in this position
      //  in the tree, array already exists, the function returns 0 (array is not converted to hash or map).
      //  In other cases, for numeric key, an array is created containing that index,
      //  for non-numeric key -  a hash with appropriate entry is autocreated
      //  (if a scalar exists in this place, it will be overwritten).
    inline const unity* path(const std::wstring& keylist) const throw() { return const_cast<unity*>(this)->_path_u(keylist, false); }
    inline const unity* path(const wchar_t* keylist) const throw() { try { return const_cast<unity*>(this)->_path_u(keylist, false); } catch (...) { return 0; } }
    inline const unity* path(const std::string& keylist) const throw() { try { return const_cast<unity*>(this)->_path_u(bsToWs(keylist), false); } catch (...) { return 0; } }
    inline const unity* path(const char* keylist) const throw() { try { return const_cast<unity*>(this)->_path_u(bsToWs(keylist), false); } catch (...) { return 0; } }
    inline const unity* path(const unity& keylist) const throw();

    inline const unity* path(const std::wstring& keylist, const unity& x_dflt) const throw() { const unity* p = path(keylist); return p ? p : &x_dflt; }
    inline const unity* path(const wchar_t* keylist, const unity& x_dflt) const throw() { const unity* p = path(keylist); return p ? p : &x_dflt; }
    inline const unity* path(const std::string& keylist, const unity& x_dflt) const throw() { const unity* p = path(keylist); return p ? p : &x_dflt; }
    inline const unity* path(const char* keylist, const unity& x_dflt) const throw() { const unity* p = path(keylist); return p ? p : &x_dflt; }
    inline const unity* path(const unity& keylist, const unity& x_dflt) const throw() { const unity* p = path(keylist); return p ? p : &x_dflt; }

    inline unity* path_w(const std::wstring& keylist) throw() { return _path_u(keylist, true); }
    inline unity* path_w(const wchar_t* keylist) throw() { try { return _path_u(keylist, true); } catch (...) { return 0; } }
    inline unity* path_w(const std::string& keylist) throw() { try { return _path_u(bsToWs(keylist), true); } catch (...) { return 0; } }
    inline unity* path_w(const char* keylist) throw() { try { return _path_u(bsToWs(keylist), true); } catch (...) { return 0; } }
    inline unity* path_w(const unity& keylist) throw();



    //==Map, Hash==

      // assoc* functions work both with maps and with hashes,
      //    without changing the container type. The container must be map or hash.
      //    For maps, list of key/value pairs is iterated in natural sort order,
      //    numeric positions in assocl_* and index values in mapi_* are the same.
    bool assoc_set(const unity& k, const unity& v, bool keep_first = false);
    bool assoc_del(const unity& k);

    s_long assocS_c() const;

    inline s_long assocl_noel() const { return hashx_common::no_elem; }
    inline s_long assocl_first() const { return assocl_next(assocl_noel()); }
    inline s_long assocl_last() const { return assocl_prev(assocl_noel()); }
    s_long assocl_next(s_long pos) const;
    s_long assocl_prev(s_long pos) const;
    const unity& assocl_key(s_long pos) const;
    unity& assocl(s_long pos);
    const unity& assocl_c(s_long pos) const;



    // CONTAINED OBJECTS (utObject) AND INTERFACES


    // The following 3 functions (pinterface, cpvoid, cpvoidkey)
    //    work properly with unity object created in any binary module.
    //  Other object functions work only with unity object created in the current module.

      // If *this references a VALID object, and supports the specified inteface
      //    for the object, the function returns a pointer to the interface.
      //    In all other cases, the function returns 0.
      // Note. The pointer obtained can be used for calls until one of two occurs:
      //  a) the object is destroyed,
      //  b) *this is rewritten by any other reference or value,
      //      even reference to the same object from other unity object.
      // Note. Interfaces can be implemented
      //  a) by the object itself (inherit/implement virtual functions),
      //      in this case pinterface() == objPtr(),
      //  b) as a o_iimpl specialization (see above).
    template<class Interface> interface_ptr<Interface> pinterface() const { typedef interface_ptr<Interface> t_pi; return (isObject()) ? t_pi(_drf_o(), pmsm) : t_pi(); }

      // Non-0 if both isObject() is true and the object reference is VALID.
      //  0 in all other cases.
    const void* cpvoid() const { if (isObject()) { if (pmsm == unity_common::ls_modsm) { return _drf_o()->pvoid(1); } return ((unity_common::__Pcpvoid)pmsm(unity_common::msm_obj_cpvoid))(_drf_o(), 1); } return 0; }

      // Non-0 if isObject() == true, even if the object is already deleted (just a weak refrence left).
      //  0 if isObject() == false.
    const void* cpvoidkey() const { if (isObject()) { if (pmsm == unity_common::ls_modsm) { return _drf_o()->pvoid(0); } return ((unity_common::__Pcpvoid)pmsm(unity_common::msm_obj_cpvoid))(_drf_o(), 0); } return 0; }



      // isStrongRef true makes *this handling object destruction, false leaves it to the client.
      // If x is already referenced by set_obj, isStrongRef can be used to change type of reference. See also objSetStrongRef().
      //  NOTE Currently, set_obj succeeds only in local binary module of *this,
      //    so that both x and *this always belong to the same module, even when passed/manipulated in another.
      // Result:
      //    a) Normally, the function creates/sets all necessary and returns *this. If isStrongRef == true, unity object takes ownership on it.
      //    b) In case of error, an exception is generated. If isStrongRef == true, the client remains responsible for deleting the object if it's just created.
    template<class Obj, class Interfaces>
    unity& set_obj(Obj& x, bool isStrongRef)
      {
        struct __check { typename Interfaces::__Obj* const temp; __check() : temp(Interfaces().o_interfaces<Obj>::__pobj()) {} }; enum Temp { __1 = sizeof(__check) };
        if (pmsm != unity_common::ls_modsm) { throw XUExec("set_obj", "non-local call"); }
        if (isObject()) { o_handler_base* ph = _drf_o(); if (&x == ph->pvoid(true)) { ph->objSetStrongRef(isStrongRef); return *this; } }
        o_handler_base* ph = new objhandler<Obj, Interfaces>(x, isStrongRef);
        clear(); _data.p1 = ph; _data.p2 = 0; ut = utObject | xfPtr; return *this;
      }
    template<class Obj>
    unity& set_obj(Obj& x, bool isStrongRef)        { return set_obj<Obj, o_interfaces_top<Obj> >(x, isStrongRef); }

      // Returns.
      //  a) (isChecked true) valid pointer to the referenced object,
      //    if Obj is the same type as type of the internally contained object reference.
      //    0 if the object is not valid, or type does not match.
      //    NOTE this case works only if the object is created in the current binary module,
      //    otherwies objPtr returns 0.
      //  b) (isChecked false) internal pointer to the referenced object (same value as cpvoidkey),
      //    directly cast into Obj. For polymorphic pointers, this pointer will be valid only
      //    if Obj exactly matches the internal pointer type.
    template<class Obj> Obj* objPtr(bool isChecked = true)
    {
      if (!isChecked) { return const_cast<Obj*>(reinterpret_cast<const Obj*>(cpvoidkey())); }
      if (!isObject() || pmsm != unity_common::ls_modsm) { return 0; }
      o_handler_base* h = _drf_o(); void* p = h->pvoid(true);
      if (p && typeid(Obj) == h->objStatTypeInfo()) { return reinterpret_cast<Obj*>(p); }
      return 0;
    }
    template<class Obj> const Obj* objPtr_c(bool isChecked = true) const
      { return const_cast<unity*>(this)->objPtr<Obj>(isChecked); }

      // If *this contains a pointer to object, even no valid (i.e. object is already deleted).
      //  returns its static type info (fixed by set_obj on object handler creation).
      // If *this does not contain an object, returns type_info(void*).
    const std::type_info& objStatTypeInfo() const;

      // If *this references a VALID object, returns its dynamic type info.
      //  Otherwise returns type_info(void*). See also objPtr().
    const std::type_info& objDynTypeInfo() const;

      // >0: *this references a VALID target object, using either weak or strong pointer. Other objects may reference it as well.
      // 0 if *this contains a pointer to object that has been deleted.
      // -1 *this and other objects reference the target object using weak pointer, and do not handle object deletion.
      //  Note that the client only is responsible for keeping the object alive while any *this references it.
      // -2 if isObject() == false.
    int objStrongRefsCount() const;

      // >0 if *this references an object, and there are weak references to that object from this and/or other unity objects.
      // 0 if *this references an object using strong pointer, and there are no weak references to that object from other objects.
      //  Also, in this case if objStrongRefsCount() == 0, the object has been deleted, only pointer value is still kept.
      // -2 if isObject() == false.
    int objWeakRefsCount() const;

      // true if this *this references a VALID object, and handles its deletion.
      // false in all other cases.
    bool objIsStrongRef() const;

      // true if this *this references a VALID object, but does not handle its deletion.
      // false in all other cases.
    bool objIsWeakRef() const;

      // Switches *this to reference its object by strong or weak reference.
      // true on on_calm_result.
      // false in the following cases:
      //  a) isObject() == false.
      //  b) isStrongRef == true, but the object has been deleted already (i.e. objStrongRefsCount() == 0).
      // Note (!) that objSetStrongRef(false) may lead to object deletion if it changed the last strong reference to weak.
    bool objSetStrongRef(bool isStrongRef);

      // Creates an interfaces set and attaches it to the collection
      //  if no set of this type exists.
      // The set must inherit from o_interfaces<C>,
      // where either C == Obj (type of object handled by unity container),
      // or C != Obj. In the latter case, the set will have no direct access
      // to the handled object, i.e. its __pobj() == 0. Still, its __pinterface()
      // and __pinterface_prev() give normal access to any interfaces
      // supported by *this for the handled object, i.e. the interface
      // implementation can operate on object through interfaces,
      // without referencing the factual object.
      //
      //Returns:
      // 2: the set of interfaces created and attached successfully, C == Obj.
      // 1: the set of interfaces created and attached successfully, C != Obj.
      // 0: (nothing changed) the set of interfaces of the specified type is already attached to the object.
      // -3: any failure (nothing changed).
    template<class Interfaces>
    int objItfsAttach()
    {
      struct __check { typename Interfaces::__Obj* const temp; __check() : temp(Interfaces().o_interfaces<typename Interfaces::__Obj>::__pobj()) {} };
      enum Temp { __1 = sizeof(__check) };
      if (pmsm != unity_common::ls_modsm) { return -3; }
      if (isObject()) { return _drf_o()->objItfsAttach<Interfaces>(); }
      return -3;
    }

      //Removes the set of interfaces of the given type to the handled object.
      //Returns:
      // 1: the set of interfaces removed successfully.
      // 0: nothing changed - the specified set of interfaces was not attached to the object.
      // -1: (nothing changed) failed to remove the existing set.
      // -3: (nothing changed) any other errors.
    template<class Interfaces>
      int objItfsRemove()        { if (pmsm != unity_common::ls_modsm) { return -3; } if (isObject()) { return _drf_o()->objItfsRemove<Interfaces>(); } return -3; }

      //Removes the set of interfaces with the given index.
      //Returns:
      // 1: the set of interfaces removed successfully.
      // -1: (nothing changed) failed to remove the existing set.
      // -3: (nothing changed) any other errors (for example, ind_of_set is not correct).
    int objItfsRemoveInd(int ind_of_set)        { if (pmsm != unity_common::ls_modsm) { return -3; } if (isObject()) { return _drf_o()->objItfsRemoveInd(ind_of_set); } return -3; }

      //Removes the last set of interfaces containing Interface.
      //Returns:
      // 1: the last set of interfaces containing Interface removed successfully.
      // 0: (nothing changed) any set containing Interface was not attached to the object.
      // -1: (nothing changed) failed to remove the existing set.
      // -3: (nothing changed) any other errors.
    template<class Interface>
      int objItfRemoveLast()        { if (pmsm != unity_common::ls_modsm) { return -3; } if (isObject()) { return _drf_o()->objItfRemoveLast<Interface>(); } return -3; }

      //Finds the position of the set of interfaces of the specified type.
      //Returns:
      // >=0: (on_calm_result) index of the set of interfaces of the specified type, in the object handler's collection.
      // -1: (on_calm_result) the specified set is the object's default set of intefaces (created by set_obj).
      // -2: (not found) the specified set is not found.
      // -3: (failure) any other error.
    template<class Interfaces>
      int objItfsFind() const        { if (pmsm != unity_common::ls_modsm) { return -3; } if (isObject()) { return _drf_o()->objItfsFind<Interfaces>(); } return -3; }

      //Finds the position of the given object (set of interfaces).
      //Returns:
      // >=0: (on_calm_result) index of the specified interfaces set in the target object handler's collection.
      //  Matching is by pointer value, not by type of set.
      // -1: (on_calm_result) the specified set is the object's default set of intefaces (created by set_obj).
      // -2: (not found) the specified set is not found (x==0 makes this ret. val. too).
      // -3: (failure) any other error.
    int objItfsFindPtr(o_itfset_base* x) const        { if (pmsm != unity_common::ls_modsm) { return -3; } if (isObject()) { return _drf_o()->objItfsFindPtr(x); } return -3; }

      //Finds the position of the latest set of interfaces containing Interface.
      //  Note: pinterface() always chooses the latest attached set for the given Interface.
      //  This way, several interfaces of the same can multiply override each other.
      //Returns:
      // >=0: (on_calm_result) index of the last set of interfaces containing Interface.
      // -1: (on_calm_result) the only set containing Interface is object's default set of intefaces (created by set_obj).
      // -2: (not found) no set containing Interface is found.
      // -3: (failure) any other error.
    template<class Interface>
      int objItfFindLast() const        { if (isObject()) { if (pmsm != unity_common::ls_modsm) { return -3; } return _drf_o()->objItfFindLast<Interface>(); } return -3; }

      //Returns indices of all sets supporting the specified interface.
      //  Testing is done first-to-last, i.e. the most actual set will be at the end of the collection.
      //If nothing found, or on error, an empty vector is returned.
    template<class Interface>
      std::vector<int> objItfFindAll() const        { if (pmsm != unity_common::ls_modsm) { return std::vector<int>(); } if (isObject()) { return _drf_o()->objItfFindAll<Interface>(); } return std::vector<int>(); }

      //Returns a pointer to the set of interfaces given an index.
      //  ind_of_set can be -1..objItfsMaxInd().
      //  -1 returns the default set of interfaces, created by set_obj.
      //  0..objItfsMaxInd() addresses interface sets that have been attached during unity object's lifetime.
      //  If ind_of_set is wrong, the function returns 0.
      //Note. Set pointers can be used for dynamic casting
      //  into particular interface reference (o_iref<Interface>)
      //  or other subclass of the returned object.
    o_itfset_base* objItfsByInd(int ind_of_set) const        { if (pmsm != unity_common::ls_modsm) { return 0; } if (isObject()) { return _drf_o()->objItfsByInd(ind_of_set); } return 0; }

      //Returns the maximum valid index for sets of interfaces.
      //  Note: the default set always exists, and its index is -1.
      //  Any attached interfaces have indices 0 to objItfsMaxInd().
      //  If nothing is attached (i.e. only default set exists), objItfsMaxInd() == -1.
      //  Total number of interface sets in the object handler is always 2+objItfsMaxInd().
      //If *this does not contain an object, objItfsMaxInd() returns -2.
    int objItfsMaxInd() const        { if (pmsm != unity_common::ls_modsm) { return -2; } if (isObject()) { return _drf_o()->objItfsMaxInd(); } return -2; }




    // ACCESS TO SHARED LIBRARIES
    #if bmdx_part_dllmgmt   
      struct _mod_exhandle;
      struct mod_handle
      {
        mod_handle() throw(); ~mod_handle() throw();

        #ifdef _bmdxpl_Wnds
          typedef HINSTANCE t_nativehandle;
        #endif
        #ifdef _bmdxpl_Psx
          typedef void* t_nativehandle;
        #endif

        bool is_valid() const throw();
        t_nativehandle native() const throw();
        bool operator==(const mod_handle& x) const throw();
        bool operator!=(const mod_handle& x) const throw() { return !operator==(x); }

          // Pointer to procedure or variable by name (GetProcAddress, dlsym).
        void* sym(const char* name) const throw();

          // Call into shared module (.dll, .so) predefined function:
          //    s_long bmdx_mod_request(s_long sig_struct, s_long action, const unity* ppara, unity* pretval);
          //    with action == 1,
          //    sig_struct == unity::sig_struct() of the current binary module,
          //    ppara == &para,
          //    pretval == &retval.
          // para: input parameters of the request.
          // retval: value, returned by the reuest.
          //    Input and output contents are determined by the client and requested module relationships.
          // The request is done under global lock, which may block up to 60 s.
          //  (In multithreaded program, requests should take small time, <= milliseconds.)
          // Returns:
          //  1 - on_calm_result.
          //  -1 - the requested module is not compatible.
          //  -2 - client-side failure or lock timeout.
          //  <= -10 - module-side error.
        s_long request(const unity& para, unity& retval) const throw();

// Specification for request function on the shared module side.
//
//  sig: must be equal to unity::sig_struct(), otherwise module must return -1 (compatibility error).
//  action: 1 - client request.
//      0 and >2 - reserved., the function must return -2.
//      < 0 - module-specified action.
//  ppara, pretval: nonzero pointers. Must be checked for compatibility before use.
//  Returns:
//    1 - on_calm_result.
//    -1 - compatibility error.
//    -2 - unrecognized action code.
//    >0, -3..-9 - reserved.
//    <= -10 - custom module-side error.
//
//extern "C" bmdx::s_long bmdx_mod_request(bmdx::s_long sig, bmdx::s_long action, const bmdx::unity* ppara, bmdx::unity* pretval)
//{
//  if (!(sig == unity::sig_struct() && ppara->compatibility() > 0 && pretval->compatibility() > 0)) { return -1; }
//  if (action != 1) { return -2; }
//  try {
//   ...
//   return 1;
//  } catch (...) { return -10; }
//}
        private: friend struct unity; cref_t<_mod_exhandle> _rex; bool _b_au;
      };

        // Returns binary module (.dll, .so) handle representation.
        //    If module with given name is not loaded yet, an attempt to load is done.
        // If name is invalid, or module loading has failed, the returned reference is empty.
        // If b_autounload is true, the module may be unloaded if no other handle remains.
        //  If any handle was created with b_autounload == false,
        //  the module won't be unloaded until the program exits.
        // NOTE mod(), mod_handle::request(), and other functions
        //    may be safely called from multiple threads.
        // NOTE The client must ensure that no high-level cross-module objects
        //    left in memory when the module is unloaded.
      static mod_handle mod(const char* name, bool b_autounload) throw ();

    #endif //bmdx_part_dllmgmt

        // unity object structure signature for binary comatibility check.
    static s_long sig_struct() throw ();

      // Returns:
      //  2 - the current object is created in the current module.
      //  1 - the current object is compatible with the current module.
      //  0 - the current object may be safely copied to an object, created in the current module.
      //    For strings, 0 means that direct reference to string may not be got by the client.
      //    Still, strings may be copied and assigned.
      //  <0 - the current object is not compatible with the current module.
      //  Only clear() and destructor may be called safely.
    s_long compatibility() const throw ();

    inline t_map* _m() const throw () { return reinterpret_cast<t_map*>(_data.p1); }
    inline t_hash* _h() const throw () { return reinterpret_cast<t_hash*>(_data.p1); }

  private:

    enum { xutFull = 0x3f, xfPtr = 0x40, _cptm = 0xffffff, _cptsh = 8, xutFullWithFlags = (1 << _cptsh) - 1 };

    unity_common::__Pmodsm pmsm;
    union { mutable s_long ut; void* __3; }; // ut bits 8..31 contain either 0 or cached compatibility id
    union { mutable _storage _data; meta::s_ll __1; double __2; };

    friend struct StaticConv; friend struct o_handler_base; friend struct cv_ff;

    static _fls75 _tname0(s_long utt) throw ();

    static s_long _def_size(s_long utt) throw();

    template<int utt> inline typename valtype_t<utt>::t* _drf() const throw () { return reference_t<typename valtype_t<utt>::t>::_drf_c(this); }
    inline o_handler_base* _drf_o() const { return _reference_base<o_handler_base, int>::deref(_data, true); }
    inline s_long _compat_id(unity_common::__Pmodsm p, s_long utype) const throw () { return (((char*)p - (char*)0) + utype) & _cptm; }
    inline bool _compat_chk() const throw() { if (!this) { return false; } if (pmsm == unity_common::ls_modsm) { return true; } const s_long v1 = (ut >> _cptsh) & _cptm; const s_long v2 = _compat_id(&unity_common::ls_modsm, utype()); if (v1 == v2) { return true; } const s_long v3 = _compat_id(&unity_common::ls_modsm, -1); if (v1 == v3) { return false; } if (_compatibility() > 0) { ut = (ut & xutFullWithFlags) | (v2 << _cptsh); return true; } ut = (ut & xutFullWithFlags) | (v3 << _cptsh); return false; }
    s_long _compatibility() const throw();

    inline vec2_t<meta::s_ll>* _drf_q() const { return reinterpret_cast<vec2_t<meta::s_ll>*>(_data.p1); }
    inline unity*& _drf_name() const { return *reinterpret_cast<unity**>(&_data.p2); }
    struct _vx0 : vecm { void* pval_z(s_long ind) const { ind -= _nbase; if (!(ind >= 0 && ind < _n)) { return 0; } s_long j, k, cap; m::_ind_jk(ind, j, k, cap); return _pj[j].atc_ku(k, cap, _ptd->t_size); } };
    static inline void* _pval_z(const vecm& rv, s_long ind) { return static_cast<const _vx0&>(rv).pval_z(ind); }

      // _x_* may generate exceptions.
    bool _x_cnv_val(EConvStrength cs, s_long utt_dest, s_long flags, void* pdest) const;
    void _x_cnv_this(EConvStrength cs, s_long utt_dest, bool keep_name);
    void _x_asg(const unity& x, s_long flags);
    void _xu_move_p1(unity& src); // moves_data.p1 from src to this, if can; otherwise calls _x_asg with object name keeping

      // _L* return result codes (no exceptions).
    s_long _Lcreate(_storage* pdest, s_long utt, s_long flags);
    s_long _Ldestroy(_storage* pdest, s_long utt, s_long flags);
    s_long _Lel_get(s_long ind, unity* pst) const;
    s_long _Lel_set(s_long ind, const unity& x);
    s_long _Lel_append(const unity& x);
    s_long _Lel_insrem(s_long ind, s_long m);
    s_long _Lel_insrem(s_long ind, s_long m, const unity& x);
    s_long _Lu_set_name(const unity* pn);
    s_long _Lu_clear(s_long flags);
    s_long _Ldelete_this();

    inline void _ensure_h();
    inline void _ensure_m() { if (!isMap()) { u_clear(utMap); } if (!(_m()->pkf() && _compat_chk())) { throw XUExec("_ensure_m"); } }
    inline void _ensure_sc() { if (!isScalar()) { u_clear(utEmpty); } }

    unity* _path_u(const std::wstring& keylist, bool forced) throw();

    struct objhandler_refcount
    {
      int cntStrongRefs; int cntWeakRefs;
      objhandler_refcount() : cntStrongRefs(0), cntWeakRefs(0) {}

      objhandler_refcount moveRef(bool isFromWeakToStrong)
      {
        if (isFromWeakToStrong) { cntStrongRefs < 0 ? cntStrongRefs = 1 : ++cntStrongRefs; --cntWeakRefs; }
        else { ++cntWeakRefs; --cntStrongRefs; }
        objhandler_refcount retNewState = *this;
        return retNewState;
      }

      objhandler_refcount addToRef(bool isTargetRefStrong, int delta)
      {
        isTargetRefStrong ? cntStrongRefs += delta : cntWeakRefs += delta;
        objhandler_refcount retNewState = *this;
        return retNewState;
      }

      bool isNoRefs() const { return cntStrongRefs <= 0 && cntWeakRefs == 0; }
    };

    template<class Obj> class objhandler_d
    {
    protected:

      Obj* const zpObj; objhandler_refcount* zpCntRef; bool zIsStrongRef;

      objhandler_d(Obj& x, bool isMakeStrongRef)
        : zpObj(0), zpCntRef(new objhandler_refcount), zIsStrongRef(isMakeStrongRef)
      {
        Obj* const &temp_o = zpObj; const_cast<Obj*&>(temp_o) = &x;

        // The following is thread-safe, because this handler is the first for the given object, no concurrency.
        if (zIsStrongRef) { zpCntRef->cntStrongRefs = 1; zpCntRef->cntWeakRefs = 0; }
          else { zpCntRef->cntStrongRefs = -1; zpCntRef->cntWeakRefs = 1; }
      }

      objhandler_d(const objhandler_d& src)
        : zpObj(src.zpObj), zpCntRef(0), zIsStrongRef(false)
        { src.zpCntRef->addToRef(src.zIsStrongRef, 1); zpCntRef = src.zpCntRef; zIsStrongRef = src.zIsStrongRef; }

      objhandler_d& operator=(const objhandler_d& src); // not defined
      virtual ~objhandler_d(){}
    };

    template<class Obj, class Interfaces> class objhandler
      : public objhandler_d<Obj>, public o_handler_base, public Interfaces
    {
      typedef objhandler TSelf; objhandler& operator=(const objhandler& src); // not defined

      // used by clone() only
      objhandler(const objhandler& src)
        : objhandler_d<Obj>(src), o_handler_base(), Interfaces(src)
      {
        o_interfaces<Obj>* p = this;
        p->__x_on_after_copy(const_cast<objhandler*>(&src));
        for (int i = 0; i < src.zAttached.nptrs(); ++i)
        {
          o_itfset_base* p = src.zAttached.ptr(i);
          o_itfset_base* p2 = p->__clone_interfaces(this);
          int res = this->zAttached.append(p2, false);
          if (res >= 1) { p2->__x_on_after_copy(const_cast<objhandler*>(&src)); } else { delete p2; }
        }
      }

      friend struct unity; friend struct cv_ff;
      objhandler(Obj& x, bool isMakeStrongRef)
        : objhandler_d<Obj>(x, isMakeStrongRef), o_handler_base(), Interfaces()
        { o_itfset_base* p = this; p->__x_on_after_init(); }

      virtual void __replicate_interfaces_unchecked(void* x, bool isStrongRef, unity& dest) const
      {
        o_handler_base* ph = new objhandler(*reinterpret_cast<Obj*>(x), isStrongRef);
        dest.clear(); dest._data.p1 = ph; dest.ut = utObject | xfPtr; int n = zAttached.nptrs();
        if (n > 0)
        {
          o_itfset_base** pp = &zAttached.ptr(0);
          for (int i = 0; i < n; ++i)
          {
            o_itfset_base* p2 = (*pp)->__new_instance(ph); int res = ref_zAttached(*ph).append(p2, false);
            if (res >= 1) { p2->__x_on_after_init(); } else { delete p2; }
            ++pp;
          }
        }
      }

      virtual ~objhandler()
      {
        while (zAttached.nptrs() > 0)
        {
          o_itfset_base* p = zAttached.ptr(zAttached.nptrs() - 1);
          p->__x_on_before_destroy();
          zAttached.remove_last();
          delete p;
        }
        o_itfset_base* p = this; p->__x_on_before_destroy();

        objhandler_refcount res = objhandler_d<Obj>::zpCntRef->addToRef(objhandler_d<Obj>::zIsStrongRef, -1);
        if (objhandler_d<Obj>::zIsStrongRef && res.cntStrongRefs == 0) { delete objhandler_d<Obj>::zpObj; }
        if (res.isNoRefs()) { delete objhandler_d<Obj>::zpCntRef; }
      }

      virtual void* pvoid(bool isValidOnly) const { return !isValidOnly || objhandler_d<Obj>::zpCntRef->cntStrongRefs != 0 ? const_cast<typename meta::nonc_t<Obj>::t*>(objhandler_d<Obj>::zpObj) : 0; }
      virtual const std::type_info& objStatTypeInfo() const { return typeid(Obj); }
      virtual const std::type_info& objDynTypeInfo() const { return objhandler_d<Obj>::zpCntRef->cntStrongRefs == 0 ? typeid(void*) : typeid(*objhandler_d<Obj>::zpObj); }
      virtual int objStrongRefsCount() const { return objhandler_d<Obj>::zpCntRef->cntStrongRefs; }
      virtual int objWeakRefsCount() const { return objhandler_d<Obj>::zpCntRef->cntWeakRefs; }
      virtual bool objIsStrongRef() const { return objhandler_d<Obj>::zIsStrongRef; }

      virtual bool objSetStrongRef(bool isStrongRef)
      {
        if (isStrongRef != objhandler_d<Obj>::zIsStrongRef)
        {
          if (isStrongRef) { if (objhandler_d<Obj>::zpCntRef->cntStrongRefs == 0) { return false; } objhandler_d<Obj>::zpCntRef->moveRef(true); }
            else { objhandler_refcount res = objhandler_d<Obj>::zpCntRef->moveRef(false); if (res.cntStrongRefs == 0) { delete objhandler_d<Obj>::zpObj; } }
          objhandler_d<Obj>::zIsStrongRef = isStrongRef;
        }
        return true;
      }

      virtual o_handler_base* clone() const { return new objhandler(*this); }
      virtual o_itfset_base* itfset_default() const { return const_cast<TSelf*>(this); }

      virtual const std::type_info& __interfaces_type() const { return typeid(Interfaces); }
      virtual o_itfset_base* __clone_interfaces(o_handler_base* dest_phandler) const { return 0; }
      virtual o_handler_base* __phandler() const { const o_handler_base* p = this; return const_cast<o_handler_base*>(p); }
      virtual Obj* __pobj() const { return objhandler_d<Obj>::zpCntRef->cntStrongRefs == 0 ? 0 : objhandler_d<Obj>::zpObj; }

      template <class T> struct _deref_any { typedef T t_obj; static T* ptr(void* psrc) { return reinterpret_cast<T*>(psrc); } };
      template <class T, class L> struct _deref_any<cref_t<T, L> > { typedef T t_obj; static T* ptr(void* psrc) { return reinterpret_cast<cref_t<T, L>* >(psrc)->_pnonc_u(); } };
      virtual void* prefo(const char* tname) const
      {
        if (!tname) { return 0; }
        if (0 == strcmp(typeid(typename _deref_any<Obj>::t_obj).name(), tname)) { return _deref_any<Obj>::ptr(pvoid(true)); }
        return 0;
      }
    };
  };











  template<class Obj>
  int o_handler_base::replicate_interfaces(Obj* x, bool isStrongRef, unity& dest)
  {
    if (!x) { return -4; }
    if (dest.isObject() && dest._data.p1 == this) { return -2; }
    if (typeid(Obj) != this->objStatTypeInfo()) { return -1; }
    __replicate_interfaces_unchecked(x, isStrongRef, dest);
    return 1;
  }

  template<class T>
  struct o_iimpl<T, i_construction>
  :
   public i_construction,
   public o_iref<i_construction>,
   public virtual o_interfaces<T>
  {
    virtual i_construction* __pMyI() { return this; }

    virtual unity new_instance() const
    {
      unity v;
      o_interfaces<T>* pbase = this;
      o_handler_base* ph = pbase->__phandler();

      if (ph)
      {
        T* px = new T;
        int res = ph->replicate_interfaces<T>(px, true, v);
        if (res < 0) { delete px; }
      }
      return v;
     }

    virtual unity new_copy() const
    {
      unity v;

      o_interfaces<T>* pbase = this;
      T* px0 = pbase->__pobj();
      o_handler_base* ph = pbase->__phandler();

      if (px0 && ph)
      {
        T* px = new T(*px0);
        int res = ph->replicate_interfaces<T>(px, true, v);
        if (res < 0) { delete px; }
      }
      return v;
     }
  };




    //Argument for passing into functions.
    //Constructs:
    //  a) value copy for non-unity arguments.
    //  b) value reference for arguments of type unity.
    //  c) 0 pointer on default construction.
  struct unity::arg
  {
    arg(): _p(0) {}
    arg(const unity& x): _p(&x) {}
    arg(const arg& x) { if (x._p == &x._z) { _z = x._z; _p = &_z; } else { _p = x._p; } }
    template<class C> arg(const C& x): _z(x), _p(&_z) {}

    const unity* parg() const { return _p; }
    unity* parg() { return const_cast<unity*>(_p); }

    private: unity _z; const unity* _p;
  };




  struct unity::_hl_impl : protected yk_c::hashx<unity, unity, kf_unity>
  {
    typedef meta::assert<(no_elem == -1)>::t_true __check1;
    typedef yk_c::hashx<unity, unity, kf_unity> t_hash; struct exc_consistency {}; struct exc_subscript {};

    _hl_impl() : _list(typer<meta::s_ll>, -1), _indk(typer<s_long>, 0)        { _list.el_append<meta::s_ll>(_v(-1, -1)); if (!_check()) { throw exc_consistency(); } }
    _hl_impl(const _hl_impl& x) : t_hash(x), _list(x._list), _indk(x._indk)        { if (!_check()) { throw exc_consistency(); } }
    void operator=(const _hl_impl& x)        { if (this == &x) { return; } hl_clear(true); s_long res = 100 * hashx_copy(x, true) + 10 * _list.vecm_copy(x._list, true) + _indk.vecm_copy(x._indk, true); if (res != 111 || !_check()) { hl_clear(true);  throw exc_consistency(); } }

    typedef t_hash::entry entry;

    inline s_long n() const throw() { return _n; }
    unity& operator[] (const unity& k) throw (exc_subscript);
    inline const entry* operator() (s_long ind) const throw() { return this->t_hash::operator()(qi_indk(ind)); }
    inline const entry* find(const unity& k, s_long* ret_pind = 0) const throw() { const entry* e; s_long ind; this->t_hash::find2(k, *this->t_hash::pkf(), 0, &e, &ind); if (ret_pind) { *ret_pind = ind; } return e; }
    inline s_long insert(const unity& k, const entry** ret_pentry = 0, s_long* ret_pind = 0) throw() { const t_hash::entry* e(0); s_long ind; s_long res = this->t_hash::insert(k, &e, &ind); if (res > 0) { if (!_a_appended()) { this->t_hash::remove_e(e); e = 0; ind = no_elem; res = -1; } } if (ret_pentry) { *ret_pentry = e; } if (ret_pind) { *ret_pind = ind; } return res; }
    inline s_long remove(const unity& k) throw() { s_long ind; this->t_hash::find2(k, *this->t_hash::pkf(), 0, 0, &ind); return remove_i(ind); }
    inline s_long remove_i(s_long ind) throw() { s_long res = this->t_hash::remove_i(ind); if (res == 1) { _a_removed(ind); } return res; }
    inline const entry* h(s_long ind_h) const throw() { return this->t_hash::operator()(ind_h); }
    s_long hl_clear(bool full) throw();
    inline const f_kf* pkf() const throw() { return this->t_hash::pkf(); }
    inline bool hl_set_kf(const f_kf& kf) throw() { return this->t_hash::hashx_set_kf(kf); }

    inline s_long qi_noel() const throw() { return no_elem; } // starting pos for iterating the list in both directions
      // Next, prev. elem. ind. >= 0, or qi_noel() at list end
    inline s_long qi_next(s_long pos) const throw() { meta::s_ll* p = _list.pval<meta::s_ll>(pos); return p ? _inext(*p) : s_long(no_elem); }
    inline s_long qi_prev(s_long pos) const throw() { meta::s_ll* p = _list.pval<meta::s_ll>(pos); return p ? _iprev(*p) : s_long(no_elem); }

      // Index of ind-th element in hash, according to list, or qi_noel() for bad ind or indk.
    inline s_long qi_indk(s_long i) const { if (!_a_indk()) { return no_elem; } s_long* p = _indk.pval<s_long>(i); return p ? *p : s_long(no_elem); }

    inline s_long compatibility() const throw() { const s_long ti = bytes::type_index_t<s_long>::ind(); const s_long ti2 = bytes::type_index_t<meta::s_ll>::ind(); s_long c = __crvx(_list)._t_ind == ti2  && __crvx(_indk)._t_ind == ti && __crvx(_ht)._t_ind == ti && sizeof(*this) == ((char*)&_indk - (char*)this) + sizeof(vecm) ? this->vecm::compatibility() : -1; if (c == 0) { c = -1; } return c; }
  private:
    vecm _list; // n == 1 + hash n
    mutable vecm _indk; // n == hash n

      // Adjustment notifications for list and keys index, on each hash change.
    bool _a_rsv_1() throw(); // reserve place in list and indk before possible adding 1 elem. to hash
    bool _a_appended() throw(); // adjust list and indk after adding 1 elem. to hash, true on on_calm_result
    void _a_removed(s_long ind) throw(); // adjust list and indk after removing 1 elem. from hash
    void _a_cleared() throw(); // clear list and indk
    bool _a_indk() const throw(); // ensure indk containing full index of hash elems.

    bool _check() const { return nexc() == 0 && _list.n() == n() + 1 && (_indk.n() == 0 || _indk.n() == n()); }
    struct _vx : vecm { friend struct _hl_impl; };
    static const _vx& __crvx(const vecm& v) { return *static_cast<const _vx*>(&v); }
    static inline meta::s_ll _v(s_long prev, s_long next) throw() { return (meta::s_ll(prev) << 32) | (meta::s_ll(next) & 0xffffffffll); }
    static inline s_long _iprev(meta::s_ll x) throw() { return s_long(x >> 32); }
    static inline s_long _inext(meta::s_ll x) throw() { return s_long(x & 0xffffffffll); }
    inline meta::s_ll _v_of(s_long ind) const throw() { meta::s_ll* p = _list.pval<meta::s_ll>(ind); return p ? *p : -1; }
    inline s_long _prev_of(s_long ind) const throw() { meta::s_ll* p = _list.pval<meta::s_ll>(ind); return p ? _iprev(*p) : -1; }
    inline s_long _next_of(s_long ind) const throw() { meta::s_ll* p = _list.pval<meta::s_ll>(ind); return p ? _inext(*p) : -1; }
    inline void _setv(s_long ind, meta::s_ll x) const throw() { meta::s_ll* p = _list.pval<meta::s_ll>(ind); if (p) { *p = x; } }
    void __eject(s_long ind) throw();
    void __insert(s_long ind, s_long after) throw();
  };

  inline void unity::_ensure_h() { if (!isHash()) { u_clear(utHash); } if (!(_h()->pkf() && _compat_chk())) { throw XUExec("_ensure_h"); } }


    // Output unity object string representation to std ostream.
  inline std::ostream& operator<<(std::ostream& s, const unity::iofmt& f) { unity::write_fmt(s, *f.px, f, false); return s; }
  inline std::ostream& operator<<(std::ostream& s, const unity& x) { unity::write_fmt(s, x, x.format(), false); return s; }
  inline std::wostream& operator<<(std::wostream& s, const unity::iofmt& f) { unity::write_fmt(s, *f.px, f, false); return s; }
  inline std::wostream& operator<<(std::wostream& s, const unity& x) { unity::write_fmt(s, x, x.format(), false); return s; }

  inline std::ostream& operator<<(std::ostream& s, const unity::_wr_cstring& x) { s << x.cstr(); return s; }
  inline std::ostream& operator<<(std::ostream& s, const unity::_wr_wstring& x) { s << x.cstr(); return s; }
  inline std::wostream& operator<<(std::wostream& s, const unity::_wr_cstring& x) { s << x.wstr(); return s; }
  inline std::wostream& operator<<(std::wostream& s, const unity::_wr_wstring& x) { s << x.wstr(); return s; }


  struct _unitypc2;
  struct unitypc
  {
  private: const unity* _p;
  public:
    unitypc(const unity* p_) : _p(p_) {}
    unitypc(const unity& x) : _p(&x) {}
    unity* _pnonc() const { return const_cast<unity*>(_p); }

    bool b_valid() const { return bool(_p); }
    unity& operator[] (const unity& ki) { check(); return (*_pnonc())[ki]; }
    const unity& operator[] (const unity& ki) const { check(); return (*_pnonc())[ki]; }
    unity* operator->() { check(); return _pnonc(); }
    const unity* operator->() const { check(); return _pnonc(); }
    unity& operator*() { check(); return *_pnonc(); }
    const unity& operator*() const { check(); return *_pnonc(); }
    bool operator==(const unitypc& p2) { return _p == p2._p; }
    bool operator==(const unity* p2) { return _p == p2; }
    bool operator!=(const unitypc& p2) { return _p == p2._p; }
    bool operator!=(const unity* p2) { return _p != p2; }
    operator unity*() { return _pnonc(); }
    operator const unity*() const { return _pnonc(); }
    operator unity&() { return **this; }
    operator const unity&() const { return **this; }

    void check() const throw(XUExec) { if (!_p) { throw XUExec("unitypc.p == 0"); } }
    template<class T> inline _unitypc2 operator/ (const T& path) const;
  };

  struct _unitypc2 : unitypc
  {
    _unitypc2(const unity* p_) : unitypc(p_) {}
    _unitypc2(const unity& x) : unitypc(&x) {}
    _unitypc2(const unitypc& x_) : unitypc(x_) {}
    template<class T> inline T operator/ (T dflt) const;
    inline unity::_wr_cstring operator/ (const char* dflt) const;
    inline unity::_wr_wstring operator/ (const wchar_t* dflt) const;
  };

    // Smart pointer for advanced functionality.
  inline unitypc operator+(const unity& x) { return x; }

    // Returns this if it is an associative array containing the given path.
    //  Otherwise returns 0.
  template<class T>
  unitypc operator* (const unitypc& x, const T& path)
    { if (!x) { return 0; } return x->path(path) ? x : unitypc(0); }

    // Returns a pointer to the object, referenced by the given path, if it exists.
    //  Otherwise returns 0.
  template<class T>
  _unitypc2 unitypc::operator/ (const T& path) const
    { if (!*this) { return 0; } return (**this).path(path); }

    // Converts the value of the referenced object into T, and returns the result.
    //  If !*this, returns dflt.
  template<class T>
  T _unitypc2::operator/ (T dflt) const
    { if (!*this) { return dflt; } typedef unity::trivconv_t<typename meta::nonc_t<T>::t> t_conv; return t_conv::Fback((**this).template val<typename t_conv::t_target>()); }

  unity::_wr_cstring _unitypc2::operator/ (const char* dflt) const
    { if (!*this) { return unity::_wr_cstring(dflt); } return unity::_wr_cstring((**this).vcstr()); }

  unity::_wr_wstring _unitypc2::operator/ (const wchar_t* dflt) const
    { if (!*this) { return unity::_wr_wstring(dflt); } return unity::_wr_wstring((**this).vstr()); }



// Utilities.

  std::wstring cmd_myexe(); // full path and filename of this exe (calculated once)
  std::wstring cmd_string(); // command line, without executable path/filename
  unity cmd_array(); // argv (0-based string array of command line arguments, size >= 1; 0 - program name (not exactly executable name/path), 1..* - dequoted arguments)

    // Prepare single command line argument with properly escaped and replaced characters.
    //  See original processctl::arg1() for details.
  inline std::string cmd_arg1(const std::string& s, bool b_shell) { return processctl::arg1(s, b_shell); }
  inline std::string cmd_arg1(const std::wstring& s, bool b_shell) { return processctl::arg1(wsToBs(s), b_shell); }

  const std::string cCR="\r";
  const std::string cLF="\n";
  const std::string cCRLF="\r\n";
  const std::wstring wCR=L"\r";
  const std::wstring wLF=L"\n";
  const std::wstring wCRLF=L"\r\n";

  const std::wstring __wBSCRLF=L"\\\r\n";

  std::string cpathsep(); // single path separator character
  std::wstring wpathsep(); // single path separator character
  std::string cpathsep2(); // two path separator characters
  std::wstring wpathsep2(); // two path separator characters

  // wide character string comparison (s1>s2: 1, s1==s2: 0, s1<s2: -1)
  s_long wscompare(const std::wstring& s1, const std::wstring& s2, bool ignore_case);

  std::string replace(const std::string& s, const std::string& from, const std::string& to, bool ignoreCase = false); // replace 'from' string occurences in 's' by 'to', return the result
  std::wstring replace(const std::wstring& s, const std::wstring& from, const std::wstring& to, bool ignoreCase = false); // replace 'from' string occurences in 's' by 'to', return the result

  std::wstring trim(const std::wstring& s, const std::wstring& swhat = L" ", bool b_left = true, bool b_right = true); // trim all 'swat' string occurences in the beginning and end of the string 's'
  std::string trim(const std::string& s, const std::string& swhat = " ", bool b_left = true, bool b_right = true); // trim all 'swat' string occurences in the beginning and end of the string 's'

  // true if str matches the given pattern; not greedy, recognizes constructs similar to: [a-zA-Z], *, ?, # (i.e. digit)
  bool wstring_like(const std::wstring& str, const std::wstring& ptn0);

  unity split(const std::wstring&, const std::wstring& delim, meta::s_ll nmax = -1); // returns 0-based array
  unity split(const std::string&, const std::string& delim, meta::s_ll nmax = -1); // returns 0-based array
  std::vector<std::string> splitToVector(const std::string&, const std::string& delim, meta::s_ll nmax = -1);
  std::vector<std::wstring> splitToVector(const std::wstring&, const std::wstring& delim, meta::s_ll nmax = -1);
  std::string join(const unity& asrc, const std::string& delim);
  std::wstring join(const unity& asrc, const std::wstring& delim);

    // Returns a short string representation of x.
    // If x is a string, it is just copied.
    // If x is an array, object, or unknown value,
    //    only a short prefix and the object type name
    //    is included into output (but NO CONTENTS).
    // If x is a date, it is converted using fixed format: YYYY-MM-DD HH-MM-SS,
    //    where H, M, S are missing if all are 0.
    // If x is char, its boolean value is returned: "True" on 0 or "False" on non-0.
    // For other values (utEmpty, utInt, utFloat)
    //    x.s_copy() is returned.
  std::wstring CStr3(const unity& x);

    //Retuns a unity object, chosen by ind (starting from 1)
    //  from the given arguments.
    //  The object will contain a copy of the given value.
    //If ind is out of range, .Empty is returned.
  unity choose  (s_long ind, const unity::arg& x1 = unity::arg(), const unity::arg& x2 = unity::arg(), const unity::arg& x3 = unity::arg(), const unity::arg& x4 = unity::arg(), const unity::arg& x5 = unity::arg(), const unity::arg& x6 = unity::arg(), const unity::arg& x7 = unity::arg(), const unity::arg& x8 = unity::arg(), const unity::arg& x9 = unity::arg(), const unity::arg& x10 = unity::arg(), const unity::arg& x11 = unity::arg(), const unity::arg& x12 = unity::arg(), const unity::arg& x13 = unity::arg(), const unity::arg& x14 = unity::arg(), const unity::arg& x15 = unity::arg(), const unity::arg& x16 = unity::arg(), const unity::arg& x17 = unity::arg(), const unity::arg& x18 = unity::arg(), const unity::arg& x19 = unity::arg(), const unity::arg& x20 = unity::arg(), const unity::arg& x21 = unity::arg(), const unity::arg& x22 = unity::arg(), const unity::arg& x23 = unity::arg(), const unity::arg& x24 = unity::arg(), const unity::arg& x25 = unity::arg(), const unity::arg& x26 = unity::arg(), const unity::arg& x27 = unity::arg(), const unity::arg& x28 = unity::arg(), const unity::arg& x29 = unity::arg(), const unity::arg& x30 = unity::arg());

  _unitydate d_datetime(s_long y, s_long m, s_long d, s_long h = 0, s_long min = 0, s_long s = 0);
  _unitydate d_time(s_long h, s_long m, s_long s);
  _unitydate d_now(bool allow_fracsec = false); //returns the current local time
  _unitydate d_nowutc(bool allow_fracsec = false); //returns the current UTC time




  struct paramline
  {
    static const std::string cpterm; //= "; ";
    static const std::string ceterm; //= " = ";
    static const std::string cvterm; //= "|";

    static const std::wstring wpterm; //= L"; ";
    static const std::wstring weterm; //= L" = ";
    static const std::wstring wvterm; //= L"|";

      //Decode a parameter line.
      //Recognizes ";" and pterm2 as element separator.
      //Puts keys and values into mh.
      //  mh is initialized as utMap (or existing map cleared) on useMap == true,
      //  or as utHash on useMap (or existing hash cleared) == false.
      // NOTE When existing container is cleared, its flags are not changed, only elements removed.
      //    This may influence results of decoding. If the client needs stable default result,
      //    it must call mh.clear() or mh.u_clear() before decode().
      //Example:
      //  key1 = value1; key2 = value2; key3 = | array elem. 1 | array elem. 2 | array elem. 3 | 1234 | 3.14159 | array elem. 6;
      //  key4 = \0; key4a = false; key5 = \1; key5a = true;
      //  key6 = 2014-01-15 23:59:00; key6a = \d2014-01-15 23:59:00;
      //  key6b = 2014-01-15; key6c = \d2014-01-15;
      //  key6d = \s2014-01-15 23:59:00;
      //  key7 = \e;
      //  key8 = 1.1; key8a = 1.; key8b = \f1;
      //  key9 = 2; key9a = \i2.0;
      //  key10 = \z
      //
      //  Here,
      //    key1, key2 have literal "value1", "value2" values,
      //    key3 is 1-based utArray of 6 utUnity elements,
      //    keys 4 to 5a have boolean values,
      //    keys 6 to 6c have utDate values,
      //    key6d has the literal string value,
      //    key7 has the utEmpty value,
      //    all key8* have the floating point values,
      //    both key9* have integer values,
      //    key10 has the utArray value with no elements.
      //
      //  The key itself is always a string value.
      //
      //Returns mh.
      //
      //Note 1. Literal occurences of special characters must be escaped
      //  in places or cases where they would be treated incorrectly.
      //  In any place, special characters that are escaped, are treated literally.
      //  All rules do not depend on the platform.
      //  More detailed:
      //    1.1. Literal semicolon (;) must be escaped in all cases (\;).
      //    1.2. Literal 2-byte (CR, LF) sequence should be stated as \~ by default (pterm2 == wCRLF),
      //      to avoid splitting the string literal, i.e. if you need to decode multiline strings as single values.
      //    1.3. Literal spaces must be escaped if occuring leftmost/rightmost of keys, values, and array values.
      //      Otherwise they will not go with the corresponding strings to further processing.
      //    1.4. Literal equality signs (=) must be escaped in keys, i.e. in a "<key>=<value>" construct
      //      the first unescaped "=" is treated as separating the key and the value.
      //      Literal "=" in values is treated correctly both with and without preceding "\".
      //    1.5. Literal "|" must be escaped if it is the first character of a string value.
      //      Alternatively, such string may be preceded by \s.
      //      Otherwise, the value will be treated as an array.
      //    1.6. Literal backslash must be escaped (\\)
      //      when it's necessary to avoid all the above cases,
      //      and also if \0, \1, \e, \d, \i, \f, \s or \z should occur literally at the beginning of a string value,
      //      instead of specifyng a type or a special value.
      //      Any other occurences of the backslash are not treated as escape sequnces,
      //      and both the backslash and the following character are normal part of a key or string value.
      //Note 2. If an exception occurs during the decoding,
      //  mh contains keys and values decoded up to the place in ssrc
      //  that caused the exception.
    unity& decode(const std::wstring& ssrc, unity& mh, bool useMap, const std::wstring& pterm2 = wCRLF);
    inline unity decode(const std::wstring& ssrc, bool useMap, const std::wstring& pterm2 = wCRLF) { unity x; decode(ssrc, x, useMap, pterm2); return x; }

    unity& decode1v(const std::wstring& ssrc, unity& dest);
    inline unity decode1v(const std::wstring& ssrc) { unity x; decode1v(ssrc, x); return x; }

      // Decoding multiline text that represents a tree of values. Default behavior:
      //  1. Clears mh (or removes exiting elements if it's already of proper type).
      //  2. Splits ssrc, using pterm2.
      //  3. Decodes each resulting substring and merges the result into mh,
      //      keeping existing end values if met a duplicate key. For duplicate subree keys,
      //      contents are merged.
      //    Normally, each substring specifies a path (an array under an empty key, in form "=|key1|key2..."),
      //      where the decoded branch will reside in the resulting hash.
      //    If path is not specified this way, the decoded branch (substring's key/value pairs)
      //      is merged into the root of mh.
      // flags:
      //  0x1 - for mh and branches, use map as target container type instead of hash.
      //  0x2 - overwrite value or branch  if met duplicate key.
      //    (Branch by value, value by branch, value by value. Branches having same path are merged).
      //  0x4 (decode_tree(mh) only) - do not clear mh if it's already associative (0x1 flag will be ignored for mh),
      //    merge ssrc tree into mh.
      //  0x8 - ignore substring starting with regexp "[\t ]*//". This allows skipping single-line C-like comments.
      //  0x10 - convert distinct CR, LF and LFCR sequences to CRLF before decoding.
      // Returns: mh.
      // NOTE With default pterm2, literal CRLF pairs in ssrc must be replaced with special sequence "\~".
      //  See also decode().
    unity& decode_tree(const std::wstring& ssrc, unity& mh, s_long flags = 0, const std::wstring& pterm2 = wCRLF);
    inline unity decode_tree(const std::wstring& ssrc, s_long flags = 0, const std::wstring& pterm2 = wCRLF) { unity x; decode_tree(ssrc, x, flags, pterm2); return x; }

      //Encodes the given map or hash (uses pterm as element separator).
      //  If mhsrc is of other subtype, returns "".
      //  If an exception occurs during encoding, sdest may contain partially encoded data.
    std::wstring& encode(const unity& mhsrc, std::wstring& sdest, const std::wstring& pterm = wpterm);
    inline std::wstring encode(const unity& mhsrc, const std::wstring& pterm = wpterm) { std::wstring s; encode(mhsrc, s, pterm); return s; }

      //Encode single part of line.
    std::wstring encode1n(const unity& name); // encode single name
    std::wstring& encode1v(const unity& value, std::wstring& sdest); // encode single value (scalar or array)
    inline std::wstring encode1v(const unity& value) { std::wstring s; encode1v(value, s); return s; }

      //Encodes the given map or hash into multiline text, producing separate text line
      //  for each branch (all key-value pairs of each associative array except pairs where value is associative array itself).
      //  At the beginning of each line, an empty key with value == full path of the branch is inserted: "=|key1|key2...".
      //  pterm - used to separate parameters in a line, representing a branch.
      //  pterm2 - used to separate lines (branches).
      // Result:
      //  a) multiline text.
      //  b) if mhsrc is not an associative array, returns "".
      //  c) if an exception occurs during encoding, sdest may contain partially encoded data.
    std::wstring& encode_tree(const unity& mhsrc, std::wstring& sdest, const std::wstring& pterm = wpterm, const std::wstring& pterm2 = wCRLF);
    inline std::wstring encode_tree(const unity& mhsrc, const std::wstring& pterm = wpterm, const std::wstring& pterm2 = wCRLF) { std::wstring s; encode_tree(mhsrc, s, pterm, pterm2); return s; }

      //set1 += set2, where sets can be utMap or utHash (or else XUTypeMismatch is generated),
      //  keep_first = true prevents overwriting the existing values in set1 by values from set2.
      //Returns set1.
    unity& merge(unity& set1, const unity& set2, bool keep_first = false);
    unity& merge(unity& set1, const std::wstring& set2_pl, bool keep_first = false);

    typedef unity::arg A;
    typedef const unity::arg& rA;

      // Returns utMap, containing pairs {x1, x2}, {x3, x4} and so one.
      //  On inserting keys into the output map, duplicate keys are ignored, i.e. the first unique key-values pairs are kept.
    unity list_m
    (
      rA x1 = A(), rA x2 = A(), rA x3 = A(), rA x4 = A(), rA x5 = A(), rA x6 = A(), rA x7 = A(), rA x8 = A(), rA x9 = A(), rA x10 = A(), rA x11 = A(), rA x12 = A(), rA x13 = A(), rA x14 = A(), rA x15 = A(), rA x16 = A(), rA x17 = A(), rA x18 = A(), rA x19 = A(), rA x20 = A(), rA x21 = A(), rA x22 = A(), rA x23 = A(), rA x24 = A(), rA x25 = A(), rA x26 = A(), rA x27 = A(), rA x28 = A(), rA x29 = A(), rA x30 = A(), rA x31 = A(), rA x32 = A(), rA x33 = A(), rA x34 = A(), rA x35 = A(), rA x36 = A(), rA x37 = A(), rA x38 = A(), rA x39 = A(), rA x40 = A(), rA x41 = A(), rA x42 = A(), rA x43 = A(), rA x44 = A(), rA x45 = A(), rA x46 = A(), rA x47 = A(), rA x48 = A(), rA x49 = A(), rA x50 = A(), rA x51 = A(), rA x52 = A(), rA x53 = A(), rA x54 = A(), rA x55 = A(), rA x56 = A(), rA x57 = A(), rA x58 = A(), rA x59 = A(),
      rA x60 = A()
    );

      // Returns utMap, containing pairs {x1, x2}, {x3, x4} and so one.
      //  On inserting keys into the output map, duplicate keys are ignored, i.e. the first unique key-values pairs are kept.
    unity list_mx
    (
      s_long fk, // unity::fkcmp* flags
      rA x1 = A(), rA x2 = A(), rA x3 = A(), rA x4 = A(), rA x5 = A(), rA x6 = A(), rA x7 = A(), rA x8 = A(), rA x9 = A(), rA x10 = A(), rA x11 = A(), rA x12 = A(), rA x13 = A(), rA x14 = A(), rA x15 = A(), rA x16 = A(), rA x17 = A(), rA x18 = A(), rA x19 = A(), rA x20 = A(), rA x21 = A(), rA x22 = A(), rA x23 = A(), rA x24 = A(), rA x25 = A(), rA x26 = A(), rA x27 = A(), rA x28 = A(), rA x29 = A(), rA x30 = A(), rA x31 = A(), rA x32 = A(), rA x33 = A(), rA x34 = A(), rA x35 = A(), rA x36 = A(), rA x37 = A(), rA x38 = A(), rA x39 = A(), rA x40 = A(), rA x41 = A(), rA x42 = A(), rA x43 = A(), rA x44 = A(), rA x45 = A(), rA x46 = A(), rA x47 = A(), rA x48 = A(), rA x49 = A(), rA x50 = A(), rA x51 = A(), rA x52 = A(), rA x53 = A(), rA x54 = A(), rA x55 = A(), rA x56 = A(), rA x57 = A(), rA x58 = A(), rA x59 = A(),
      rA x60 = A()
    );

      // Returns utHash, containing pairs {x1, x2}, {x3, x4} and so one.
      //  On inserting keys into the output hash, duplicate keys are ignored, i.e. the first unique key-values pairs are kept.
    unity list_h
    (
      rA x1 = A(), rA x2 = A(), rA x3 = A(), rA x4 = A(), rA x5 = A(), rA x6 = A(), rA x7 = A(), rA x8 = A(), rA x9 = A(), rA x10 = A(), rA x11 = A(), rA x12 = A(), rA x13 = A(), rA x14 = A(), rA x15 = A(), rA x16 = A(), rA x17 = A(), rA x18 = A(), rA x19 = A(), rA x20 = A(), rA x21 = A(), rA x22 = A(), rA x23 = A(), rA x24 = A(), rA x25 = A(), rA x26 = A(), rA x27 = A(), rA x28 = A(), rA x29 = A(), rA x30 = A(), rA x31 = A(), rA x32 = A(), rA x33 = A(), rA x34 = A(), rA x35 = A(), rA x36 = A(), rA x37 = A(), rA x38 = A(), rA x39 = A(), rA x40 = A(), rA x41 = A(), rA x42 = A(), rA x43 = A(), rA x44 = A(), rA x45 = A(), rA x46 = A(), rA x47 = A(), rA x48 = A(), rA x49 = A(), rA x50 = A(), rA x51 = A(), rA x52 = A(), rA x53 = A(), rA x54 = A(), rA x55 = A(), rA x56 = A(), rA x57 = A(), rA x58 = A(), rA x59 = A(),
      rA x60 = A()
    );

      // Returns utHash, containing pairs {x1, x2}, {x3, x4} and so one.
      //  On inserting keys into the output hash, duplicate keys are ignored, i.e. the first unique key-values pairs are kept.
    unity list_hx
    (
      s_long fk, // unity::fkcmp* flags
      rA x1 = A(), rA x2 = A(), rA x3 = A(), rA x4 = A(), rA x5 = A(), rA x6 = A(), rA x7 = A(), rA x8 = A(), rA x9 = A(), rA x10 = A(), rA x11 = A(), rA x12 = A(), rA x13 = A(), rA x14 = A(), rA x15 = A(), rA x16 = A(), rA x17 = A(), rA x18 = A(), rA x19 = A(), rA x20 = A(), rA x21 = A(), rA x22 = A(), rA x23 = A(), rA x24 = A(), rA x25 = A(), rA x26 = A(), rA x27 = A(), rA x28 = A(), rA x29 = A(), rA x30 = A(), rA x31 = A(), rA x32 = A(), rA x33 = A(), rA x34 = A(), rA x35 = A(), rA x36 = A(), rA x37 = A(), rA x38 = A(), rA x39 = A(), rA x40 = A(), rA x41 = A(), rA x42 = A(), rA x43 = A(), rA x44 = A(), rA x45 = A(), rA x46 = A(), rA x47 = A(), rA x48 = A(), rA x49 = A(), rA x50 = A(), rA x51 = A(), rA x52 = A(), rA x53 = A(), rA x54 = A(), rA x55 = A(), rA x56 = A(), rA x57 = A(), rA x58 = A(), rA x59 = A(),
      rA x60 = A()
    );

      // list_*_set_u are same as list_*, only assign the result directly to dest. arg.
      //  NOTE Assigning a value to its own container is unsafe and must be avoided.
    void _list_m_set_u
    (
      unity& mdest,
      rA x1 = A(), rA x2 = A(), rA x3 = A(), rA x4 = A(), rA x5 = A(), rA x6 = A(), rA x7 = A(), rA x8 = A(), rA x9 = A(), rA x10 = A(), rA x11 = A(), rA x12 = A(), rA x13 = A(), rA x14 = A(), rA x15 = A(), rA x16 = A(), rA x17 = A(), rA x18 = A(), rA x19 = A(), rA x20 = A(), rA x21 = A(), rA x22 = A(), rA x23 = A(), rA x24 = A(), rA x25 = A(), rA x26 = A(), rA x27 = A(), rA x28 = A(), rA x29 = A(), rA x30 = A(), rA x31 = A(), rA x32 = A(), rA x33 = A(), rA x34 = A(), rA x35 = A(), rA x36 = A(), rA x37 = A(), rA x38 = A(), rA x39 = A(), rA x40 = A(), rA x41 = A(), rA x42 = A(), rA x43 = A(), rA x44 = A(), rA x45 = A(), rA x46 = A(), rA x47 = A(), rA x48 = A(), rA x49 = A(), rA x50 = A(), rA x51 = A(), rA x52 = A(), rA x53 = A(), rA x54 = A(), rA x55 = A(), rA x56 = A(), rA x57 = A(), rA x58 = A(), rA x59 = A(),
      rA x60 = A()
    );
    void _list_mx_set_u
    (
      unity& mdest,
      s_long fk, // unity::fkcmp* flags
      rA x1 = A(), rA x2 = A(), rA x3 = A(), rA x4 = A(), rA x5 = A(), rA x6 = A(), rA x7 = A(), rA x8 = A(), rA x9 = A(), rA x10 = A(), rA x11 = A(), rA x12 = A(), rA x13 = A(), rA x14 = A(), rA x15 = A(), rA x16 = A(), rA x17 = A(), rA x18 = A(), rA x19 = A(), rA x20 = A(), rA x21 = A(), rA x22 = A(), rA x23 = A(), rA x24 = A(), rA x25 = A(), rA x26 = A(), rA x27 = A(), rA x28 = A(), rA x29 = A(), rA x30 = A(), rA x31 = A(), rA x32 = A(), rA x33 = A(), rA x34 = A(), rA x35 = A(), rA x36 = A(), rA x37 = A(), rA x38 = A(), rA x39 = A(), rA x40 = A(), rA x41 = A(), rA x42 = A(), rA x43 = A(), rA x44 = A(), rA x45 = A(), rA x46 = A(), rA x47 = A(), rA x48 = A(), rA x49 = A(), rA x50 = A(), rA x51 = A(), rA x52 = A(), rA x53 = A(), rA x54 = A(), rA x55 = A(), rA x56 = A(), rA x57 = A(), rA x58 = A(), rA x59 = A(),
      rA x60 = A()
    );
    void _list_h_set_u
    (
      unity& hdest,
      rA x1 = A(), rA x2 = A(), rA x3 = A(), rA x4 = A(), rA x5 = A(), rA x6 = A(), rA x7 = A(), rA x8 = A(), rA x9 = A(), rA x10 = A(), rA x11 = A(), rA x12 = A(), rA x13 = A(), rA x14 = A(), rA x15 = A(), rA x16 = A(), rA x17 = A(), rA x18 = A(), rA x19 = A(), rA x20 = A(), rA x21 = A(), rA x22 = A(), rA x23 = A(), rA x24 = A(), rA x25 = A(), rA x26 = A(), rA x27 = A(), rA x28 = A(), rA x29 = A(), rA x30 = A(), rA x31 = A(), rA x32 = A(), rA x33 = A(), rA x34 = A(), rA x35 = A(), rA x36 = A(), rA x37 = A(), rA x38 = A(), rA x39 = A(), rA x40 = A(), rA x41 = A(), rA x42 = A(), rA x43 = A(), rA x44 = A(), rA x45 = A(), rA x46 = A(), rA x47 = A(), rA x48 = A(), rA x49 = A(), rA x50 = A(), rA x51 = A(), rA x52 = A(), rA x53 = A(), rA x54 = A(), rA x55 = A(), rA x56 = A(), rA x57 = A(), rA x58 = A(), rA x59 = A(),
      rA x60 = A()
    );
    void _list_hx_set_u
    (
      unity& hdest,
      s_long fk, // unity::fkcmp* flags
      rA x1 = A(), rA x2 = A(), rA x3 = A(), rA x4 = A(), rA x5 = A(), rA x6 = A(), rA x7 = A(), rA x8 = A(), rA x9 = A(), rA x10 = A(), rA x11 = A(), rA x12 = A(), rA x13 = A(), rA x14 = A(), rA x15 = A(), rA x16 = A(), rA x17 = A(), rA x18 = A(), rA x19 = A(), rA x20 = A(), rA x21 = A(), rA x22 = A(), rA x23 = A(), rA x24 = A(), rA x25 = A(), rA x26 = A(), rA x27 = A(), rA x28 = A(), rA x29 = A(), rA x30 = A(), rA x31 = A(), rA x32 = A(), rA x33 = A(), rA x34 = A(), rA x35 = A(), rA x36 = A(), rA x37 = A(), rA x38 = A(), rA x39 = A(), rA x40 = A(), rA x41 = A(), rA x42 = A(), rA x43 = A(), rA x44 = A(), rA x45 = A(), rA x46 = A(), rA x47 = A(), rA x48 = A(), rA x49 = A(), rA x50 = A(), rA x51 = A(), rA x52 = A(), rA x53 = A(), rA x54 = A(), rA x55 = A(), rA x56 = A(), rA x57 = A(), rA x58 = A(), rA x59 = A(),
      rA x60 = A()
    );

  private:
    enum { fl_v = 1, fl_bs = 2, fl_crlf = 4, fl_sc = 8, fl_sp = 16, fl_eq = 32, fl_ba = 64 };
    void x_encode1(const unity& x, std::wstring& retval, bool x_name, bool x_ar_elem);
    void x_encode_branch(const unity& mh, const std::wstring& path, std::wstring& sdest, hashx<const unity*, int>& hstop, const std::wstring& pterm, const std::wstring& pterm2, const std::wstring& pathpfx);
    void x_repl_e1(const std::wstring& s1, std::wstring& s2, bool s_name, bool s_ar_elem);
    void x_replace2a(std::wstring& s, s_long flags);
    void x_replace4(const std::wstring& s1, std::wstring& s2, s_long& flags);
    void x_decode1v(unity& v, bool v_ar_elem, s_long flags);
    bool x_decode1v_auto_date(const std::wstring& s, unity& retval) throw ();
    bool x_incorrect_numeric_value(const unity& x, bool allow_empty = true) const;
    bool x_incorrect_numeric_value_str(const std::wstring& s) const;
    bool x_incorrect_integer_value(const unity& x, bool allow_empty = true, bool allow_float_str = false) const;
    bool x_incorrect_integer_value_str(const std::wstring& s, bool allow_float_str)  const;
  };

  const unity* unity::path(const unity& keylist) const throw() { try { unity* px = const_cast<unity*>(this); if (keylist.isString()) { return px->_path_u(keylist.ref<utString>(), false); } if (keylist.isArray()) { return px->_path_u(paramline().encode1v(keylist), false); } return px->_path_u(keylist.vstr(), false); } catch (...) { return 0; } }
  unity* unity::path_w(const unity& keylist) throw() { try { unity* px = const_cast<unity*>(this); if (keylist.isString()) { return px->_path_u(keylist.ref<utString>(), true); } if (keylist.isArray()) { return px->_path_u(paramline().encode1v(keylist), true); } return px->_path_u(keylist.vstr(), true); } catch (...) { return 0; } }




  enum EFileUtilsPredefinedDir
  {
      pdCurDir, //the current directory
      pdThisAppDir, //path to the current application
      pdTempDir, //Windows: %systemroot%\temp, Linux: %TMPDIR%
      pdUserDefinedDir
          //- user-defined directory, passed as second argument; if none, the path to the current application is used
  };

  struct file_utils
  {
          //Returns true is pathstr specifies the full path.
          //Returns false for relative paths.
          //The function analyzes both correct anf incorrect paths.
      bool is_full_path(const std::wstring& pathstr) const;
      bool is_full_path(const std::string& pathstr) const;

          //Get path without filename (just remove the last path element).
      std::wstring strip_path(const std::wstring& pathstr) const;
      std::string strip_path(const std::string& pathstr) const;

          //Get filename (the last path element).
      std::wstring strip_filename(const std::wstring& pathstr) const;
      std::string strip_filename(const std::string& pathstr) const;

          // Returns pathstr without the last extsep and the characters after,
          //  if this extsep occurs after the last path separator.
      std::wstring remove_ext(const std::wstring& pathstr, const std::wstring& extsep = L".") const;
      std::string remove_ext(const std::string& pathstr, const std::string& extsep = ".") const;

          // Inserts s_add into copy of pathstr before the last occurence of extsep,
          //  if this extsep occurs after the last path separator.
          //  Otherwise, returns pathstr + s_add.
      std::wstring add_to_name(const std::wstring& pathstr, const std::wstring& s_add, const std::wstring& extsep = L".") const;
      std::string add_to_name(const std::string& pathstr, const std::string& s_add, const std::string& extsep = ".") const;

          // Replace filename part in fnp with another filename fn.
      std::wstring replace_filename(const std::wstring& fnp, const std::wstring& fn) const;
      std::string replace_filename(const std::string& fnp, const std::string& fn) const;

          //Joins two path parts (or copies pathstr, if pathstr2 is omitted).
          //Then, deletes all duplicate slahses.
          //Finally, deletes the last slash, if the path has no characterss after it.
      std::wstring join_path(const std::wstring& pathstr, const std::wstring& pathstr2 = L"") const;
      std::string join_path(const std::string& pathstr, const std::string& pathstr2 = "") const;

          //Returns True if path contains slash at the end.
      bool has_rightmost_patshep(const std::wstring& pathstr) const;
      bool has_rightmost_patshep(const std::string& pathstr) const;

          //Returns false if path contains invalid symbols
          //   or incorrectly represents path in some other way.
          //Otherwise returns true.
      bool is_valid_path(const std::wstring& pathstr) const;
      bool is_valid_path(const std::string& pathstr) const;

          //If sPath is already a full path, it is returned as is.
          //If sPath starts with ".", the point is removed, and the result is prepended with the current directory, then the path is regarded as complete.
          //In other cases, a predefined path, selected by pd argument, is prepended to sPath.
          //  For pd = pdUserDefinedDir, the function uses sUserDefDir. If sUserDefDir is not specified, it is assumed to be this applicaition executable file path.
      std::wstring complete_path(const std::wstring& sPath, EFileUtilsPredefinedDir pd, const std::wstring& sUserDefDir = L"") const;
      std::string complete_path(const std::string& sPath, EFileUtilsPredefinedDir pd, const std::string& sUserDefDir = "") const;

          //Returns true if file exists (and is not a directory).
          //  If sPath contains rightmost path separator character,
          //  the function always returns false.
      bool is_ex_file(const std::wstring& sPath) const;
      bool is_ex_file(const std::string& sPath) const;

          //Returns true if directory exists (and is not a file).
          //  sPath may or may not contain rightmost path separator character,
          //  this does not influence the behavior.
      bool is_ex_dir(const std::wstring& sPath) const;
      bool is_ex_dir(const std::string& sPath) const;

          // Non-recursive expansion of environment variables in s.
          // NOTE std::wstring version of expand_env_nr
          //  converts system variable names between %-signs
          //  into local OS encoding (chars), gets variable value (again chars),
          //  then converts back to UTF-16.
      std::wstring expand_env_nr(const std::wstring& s) const;
      std::string expand_env_nr(const std::string& s) const;

          //Recursively create direcories to ensure that sPath exists.
          //  On on_calm_result (all was created or already existed) returns true.
      bool mk_subdir(const std::wstring& sPath) const;
      bool mk_subdir(const std::string& sPath) const;


//~!!! --- update the description:
          //Loads whole file into the string in binary or text mode.
          //  On error returns .Empty.
          //  On on_calm_result - .String with the characters.
          //
          //MODES:
          //
            //binary_mode true:
            //  The file is read as is. (In use_wchar true, each two sequential bytes are converted
            //  to a wide character and put to the returned string.
            //  If the last byte is at even pos, it is assumed that the input has one more byte 0x00.)
          //
            //binary_mode false, use_wchar false (smart mode):
            //  If the first two bytes in the file indicate UTF16 encoding (0xff, 0xfe), the following data is interpreted as UTF16.
            //  Otherwise the whole data is interpreted as single-byte characters in the local system encoding.
            //  The string is read as is and then converted to wide string.
            //  Note: during reading, (0xD, 0xA) pairs are automatically converted into 0xA characters.
          //
            //binary_mode false, use_wchar true:
            //  The data in the file is interpreted as UTF16.
            //  If the first two bytes in the file are (0xff, 0xfe), they are skipped.
            //  Note: during reading, (0x000D, 0x000A) pairs are automatically converted into 0xA characters.
          //
          //If ret_s is specified, the return value is written to ret_s,
          //  and function itself returns boolean true on on_calm_result, false on error.
          //See complete_path for explanation of pd.

          // format_string should contain
          // 1) one of "binary" or "text". "text" is different only in that inserts \r before \n if it was not.
          // 2) 1 or more "local8bit", "utf16le", "utf16be", "lsb8bit".
          //    local8bit, if allowed and all others do not match, is used as the default encoding.
          //    utf16*, if allowed, in text mode are detected by 0xfeff tag. In binary mode, all characters are loaded as is, including 0xfeff is it exists.
          //    lsb8bit (read raw bytes as lower byte of wchar_t) suppresses local8bit.
      unity load_string(const std::string& format_string, const std::wstring& sPath, EFileUtilsPredefinedDir pd = pdCurDir, unity::arg ret_s = unity::arg()) const;
      unity load_string(const std::string& format_string, const std::string& sPath, EFileUtilsPredefinedDir pd = pdCurDir, unity::arg ret_s = unity::arg()) const;

          //Saves characters of the string str to the specified file.
          //  Format string template:
          //  "MODE APP ENC". Here MODE = binary ^ text, APP = append ^ truncate, ENC = local8bit ^ utf16le ^ utf16be.
          //  For example: "text append utf16le".
          //Note 1. In text mode, all '\n' entries are automatically replaced with "\r\n" and also utf16 modes write 0xfffe character at the beginning of file.
          //Note 2. If str is not a string, a string representation of the value is created and written to file.
          //On on_calm_result, the function returns true.
          //See complete_path for explanation of pd and sUserDefDir.
      bool save_string(const std::string& format_string, const std::wstring& str, const std::wstring& sTargetFilePath, EFileUtilsPredefinedDir pd = pdCurDir, const std::wstring& sUserDefDir = L"") const throw();
      bool save_string(const std::string& format_string, const std::wstring& str, const std::string& sTargetFilePath, EFileUtilsPredefinedDir pd = pdCurDir, const std::wstring& sUserDefDir = L"") const throw();


        // Loads bytes from the given file into dest.
        // 1 - on_calm_result.
        // 0 - file does not exist.
        // -1 - memory alloc. error, or wrong arguments.
        // -2 - file i/o error. NOTE On i/o error, dest may be left modified.
      inline int load_cstr(const std::wstring& fnp, std::string& dest) throw() { return file_io::load_bytes(wsToBs(fnp), dest); }
      inline int load_cstr(const std::string& fnp, std::string& dest) throw() { return file_io::load_bytes(fnp, dest); }
      inline int load_cstr(const char* fnp, std::string& dest) throw() { return file_io::load_bytes(fnp, dest); }

        // Saves bytes from src to the given file.
        //    b_append == false truncates the file before writing, if it exists.
        //    if n == 0, pdata may be 0.
        // 1 - on_calm_result.
        // 0 - failed to create file (or open the existing file for writing).
        // -1 - data size too large, or memory alloc. error, or wrong arguments.
        // -2 - file i/o error. NOTE On i/o error, the file may be left modified.
      inline int save_cstr(const std::wstring& fnp, const std::string& src, bool b_append) throw() { return file_io::save_bytes(wsToBs(fnp), src, b_append); }
      inline int save_cstr(const std::string& fnp, const std::string& src, bool b_append) throw() { return file_io::save_bytes(fnp, src, b_append); }
      inline int save_cstr(const char* fnp, const std::string& src, bool b_append) throw() { return file_io::save_bytes(fnp, src, b_append); }
      inline int save_cstr(const char* fnp, const char* pdata, s_ll n, bool b_append) throw() { return file_io::save_bytes(fnp, pdata, n, b_append); }

  private:
      bool xHasCurDirShortCut(const std::wstring& sPath) const;
      bool xHasCurDirShortCut(const std::string& sPath) const;
      bool xmk_subdir(const std::string& sPath, int level) const;
  };



  template<class T>
  struct fifo_m11_lks_t {};
    //-- default lock selector for fifo_m11_t. Lock itself may be redefined. For example, to disable queue locking on a user type:
    //  namespace yk_c { template<> struct bytes::lock_t<fifo_m11_lks_t<user type> > : bytes::lock_t<meta::nothing> {}; }

      // Async. queue, where 1 reader and 1 writer may reside in different threads.
  template<class T, class LockSelector = fifo_m11_lks_t<T> >
  struct fifo_m11_t
  {
    typedef bytes::lock_t<LockSelector> t_lock; typedef T t_value; typedef cref_t<t_value, meta::nothing> t_elem;

      // If n < 3 it is adjusted to 3.
      //  After construction, is_valid() should be checked.
    fifo_m11_t(s_long n) throw () : _isup(0), _irec(0), _elems(typer<t_elem>, 0) { if (n < 3) { n = 3; } _elems.el_append_m(n, meta::construct_f<t_elem>()); }

    inline bool is_valid() const throw() { return _elems.n() >= 3; }

      // Pushing a copy of the given value to queue.
      // m:
      //    -2 - non-blocking push. On queue overflow, does nothing, returns -2.
      //    -1 - semi-blocking push. On queue overflow, attempts to overwrite the last pushed value,
      //        which may cause short blocking (rarely - if the receiver pops out all elements
      //        up to the overwritten one and tries to pop it as well within the same thread time quantum).
      //        NOTE In semi-blocking mode, x is copied before locking.
      //    0..1e7 - time, ms. On overflow, push will sleep during that time, and then retry once,
      //        in non-blocking mode.
      // Returns:
      //    1 - pushed successfully.
      //    0 - overflow, but the last pushed element was overwritten with a copy of x.
      //    -1 - incorrect m.
      //    -2 - queue overflow (only on m == -2).
      //    -3 - timeout (only on m >= 0).
      //    -4 - memory allocation error (on any m).
      //    -5 - the queue object is invalid.
    s_long push(const t_value& x, s_long m) throw()
    {
      if (!is_valid()) { return -5; } if (!(m >= -2 && m <= 10000000)) { return -1; }
      s_long s(_isup), s2(s + 1), r(_irec); if (s2 >= _elems.n()) { s2 = 0; }
      if (s2 == r) // near-full (2 elems. free) - considered as overflow
      {
        if (m == -2) { return -2; }
        if (m >= 0) { sleep_mcs(meta::s_ll(m) * 1000); return push(x, -2); }
        t_elem e(x, true); if (!e.has_ref()) { return -4; }
        if (true)
        {
          t_lock __lock; (void)__lock;
          if (_irec == r) { s -= 1; if (s < 0) { s = _elems.n(); } *_elems.pval_0u<t_elem>(s) = e; return 0; }
        }
      }
        // Enough space
      t_elem* p = _elems.pval_0u<t_elem>(s); p->clear(); p->copy(x, true);
      if (p->has_ref()) { _isup = s2; return 1; }
      return -4;
    }

    bool is_empty() const { return _irec == _isup; }

      // m:
      //    -2 - non-blocking pop. May return an empty ref., leaving 1 element unpopped.
      //      This is suitable for very frequent regular pushs/pops.
      //    -1 - semi-blocking pop. Rarely, may block to get the one available element
      //        (only if push uses m == -1).
      //    0..1e7 - time, ms. Sleep and then once recheck, in non-blocking mode, if an element is available.
      //      The check may block for short time as necessary.
      // b_pop:
      //    true: pop an element and return it (i.e. strong reference to the object).
      //    false: do not pop an element, return its copy (i.e. copy of strong reference to the object).
      // Returns:
      //    a) t_elem with has_ref() == true -- popped successfully.
      //    b) t_elem with has_ref() == false -- no elements available (m == -1: right now, m == -2: after timeout),
      //      or (only on m == -2) 0 or 1 available, but left unpopped; also on wrong m or invalid queue object.
      //    NOTE Since t_elem owns the dynamically allocated value, no mem. alloc. errors may occur in pop.
    t_elem pop(s_long m, bool b_pop) throw()
    {
      if (!is_valid()) { return t_elem(); } if (!(m >= -2 && m <= 10000000)) { return t_elem(); }
      s_long r(_irec), s(_isup);
      if (r == s) { return t_elem(); }
      s_long r2 = r + 1; if (r2 >= _elems.n()) { r2 = 0; }
      if (r2 == s) // getting seemingly last available element
      {
        if (m == -2) { return t_elem(); }
        if (m >= 0) { sleep_mcs(meta::s_ll(m) * 1000); return pop(-2, b_pop); }
        t_elem x;
        if (true)
        {
          t_lock __lock; (void)__lock;
          t_elem* p = _elems.pval_0u<t_elem>(r); x = *p;
          if (b_pop) { p->clear(); _irec = r2; }
        }
        return x;
      }
      else  // getting exactly non-last element
      {
        t_elem* p = _elems.pval_0u<t_elem>(r);
        t_elem x = *p;
        if (b_pop) { p->clear(); _irec = r2; }
        return x;
      }
    }

  protected:  volatile s_long _isup, _irec; vecm _elems;
  };


// MULTITHREADED MESSAGE DISPATCHER
#if bmdx_part_dispatcher_mt
    // All functions are intended to be called from a dispatcher client,
    //    possibly residing in separate thread.
    //    All operations are internally synchronized.
  struct i_dispatcher_mt
  {
      // Take a new message out of the slot.
      //  slotname: must be string (utString).
      //  retmsg: on on_calm_result, overwritten by message string.
      //  retbuf (optional, may be 0): receiving container for binary data,
      //    associated with the message.
      //    If retbuf is not specified, but the message contains binary part,
      //    characters are recoded into wide string (wide char. values 0..255)
      //    and put into "bin" key of the message.
      //    NOTE recoding large binary data blocks results in performance loss.
      //  flags (OR):
      //    1 - return hash in retmsg. If not set, return string in retmsg.
      //      NOTE If the client resides in another binary module than dispatcher,
      //        popping + locally decoding string message is recommended:
      //        1) It avoids creating heterogeneous hash object.
      //        2) pop() won't fail if unity hash of the client module is incompatible
      //          within dispatcher module.
      //    2 - discard message if it exists, do not write to retmsg, retbuf.
      //      NOTE (command input only) The client must respond anyway to correct address.
      //        Possible cases:
      //        a) peek (no pop); extract sender address; pop/discard; write response.
      //        b) when the client receives commands from single source:
      //          pop/discard; write response to the known address.
      //    4 - ignore message binary part if it is received but no retbuf given.
      //    8 - "peek" (return everything as specified, only do not pop the message).
      //        May be used with flag value 2 to test potential pop result w/o getting any data.
      // Returns:
      //    3 - on_calm_result, ret. message discarded (only on (flags & 2) != 0).
      //    2 - on_calm_result, got two-part message (msg. itself and binary part). Parts are returned as flags specify.
      //    1 - on_calm_result, got single-part message (no binary part).
      //    0 - no message, or no response because of no command sent.
      //    20 - (pbo only) remote or interprocess command failed.
      //      retmsg is set to auto-generated response, containing
      //      trg, src, err_code, err_msg. (It does not contain original command message.)
      //      NOTE (flags & 2) suppresses code 20, code 3 is returned instead.
      // NOTE After codes >= 0, the client may write another message or command.
      //    In case of -1..-19 codes, the given slot probably won't function normally.
      //    After codes -20, -21, the client should take specific action.
      //    -1 - invalid slotname.
      //    -2 - failed (mem. alloc. error, invalid proxy object, invalid message format,
      //      binary incompatibility etc.). The client may try re-popping later,
      //      or try to discard possibly existing message (re-pop with (flags & 2) != 0).
      //    -3 - session is closed, no more requests, please.
      //    -20 - (pbo only) waiting for command completion, still command target sent no response.
      //    -21 - (pbi, qbi only) waiting for the client responding to the current command (i.e. the client must call write() first).
    virtual s_long pop(const unity& slotname, unity& retmsg, _carray_base_t<char>* retbuf, s_long flags) throw() = 0;

      // Write a message into the slot.
      //    Depending on the slot type and condition,
      //    msg may be put into the queue, rejected,
      //    or overwrite prev. message.
      //  msg: must be string. Min. contents:
      //    src = <NAME>; trg = <ADDR>; text = <T>
      //      NAME - source slot name, same as NAME, described near slots_create.
      //      ADDR - destination slot, format is same as N2 described near slots_create.
      //      If NAME contains N2 part, ADDR must be the same.
      //      T contains arbitrary client message. "text" key is optional.
      //  buf: optional binary data, associated with the message.
      // Returns:
      //    1 - on_calm_result, written.
      //    0 - on_calm_result, prev. message overwritten (only for non-command pins).
      //    -1 - invalid message format (multiple cases).
      //    -2 - failed (mem. alloc. error, invalid object or smth. else).
      //    -3 - session is closed, no more requests, please.
      //    -4 - target queue overflow (only for queues).
      //    -5 - rejected, out of order (only for command messages and responses).
      //    -6 - remote or interprocess sending is disabled or not supported.
      //    -7 - remote or interprocess sending failed.
      //    -8 - local target thread or slot does not exist.
      //    -9 - target address mismatch with cmd. source (only for command responses).
      //    -10 - (qs slot only) writing to the specified slot is disabled for the current source thread.
    virtual s_long write(const unity& msg, const arrayref_t<char>* buf = 0) throw() = 0;

      // Call for information or extended operation.
      //  rt (request type):
      //    1 - get list of local threads. On on_calm_result, returns: utStringArray.
      //    2 - get all slots of local thread. args = thread name (scalar). On on_calm_result, returns: utStringArray.
      //    3 - set priority and timing for internal thread, delivering subscription messages.
      //      args = array(priority, delivery loop sleep time).
      //        priority: 1..7 in units of threadctl.
      //        sleep time: mcs, >= 0
      //      On on_calm_result, returns: empty retval.
      //      NOTE The request fails (-4) if the internal thread does not exist (not configured).
      //    4 - get priority and timing for internal thread, delivering subscription messages.
      //      retval: 1-based array with 2 elements same as args in rt == 3, see above.
      //      NOTE The request fails (-4) if the internal thread does not exist (not configured).
      // Return value:
      //  1 - on_calm_result.
      //  -1 - invalid argument.
      //  -2 - failure.
      //  -3 - session is closed.
      //  -4 - request failure (e.g. no object found, operation not possible or not permitted, etc.).
    virtual s_long request(s_long rt, unity& retval, const unity& args = unity()) throw() = 0;

      // Creates one or more slots during dispatcher session.
      // slotscfg: must be string (utString), representing configuration tree acceptably
      //    for paramline::decode_tree(flags=8), or already decoded tree.
      //  with the following structure:
      //  slots = |<slot name 1>[|<slot name 2>...]
      //    Slot name:
      //    a) <slot type>_<slot name root>
      //      Slot name root: non-empty string.
      //      Slot types:
      //        po -- output-only pin, message goes directly to receiver
      //        pi -- input-only pin, messages may be overwritten if not pushed in time
      //        pbo -- strong-bound call (command) pin, keeps sequence
      //          "sender pushes cmd, receiver pops cmd, receiver pushes response, sender pops response"
      //        pbi -- strong-bound call input pin on the receiver side,
      //          only one command at a time may be processed
      //        qi -- non-blocking input queue for values; on overflow, the new value cannot be pushed
      //          (local sender receives an error code, remote sender is not notified)
      //        qbi -- strong-bound call (command) input queue on the receiver side; on overflow,
      //          a new command will not be pushed, and for both local and remote sender
      //          a response with an error message is automatically generated
      //        qs -- queue for broadcasting (multiple senders, multiple receivers)
      //    b) |<NAME>|<N2>
      //      NAME - see (a),
      //      N2 - additional address parts, e.g.
      //        |qi_char|LP|UI
      //        |qi_char|R|192.168.1.2:3500|vmpcp|UI
      //  <slot name> = <slot configuration (tree)>
      //      This part is optional.
      //      See arch_notes.txt "dispatcher_mt threads configuration structure" for details.
      // Returns:
      //    1 - on_calm_result.
      //    -1 - invalid or incompatible slotscfg.
      //    -2 - failed (mem. alloc. error, invalid object or smth. else).
      //    -3 - session is closed, no more requests, please.
      //    -4 - one of new slot names is not unique.
    virtual s_long slots_create(const unity& slotscfg) throw() = 0;

      // slotname: must be
      //    a) (string) Scalar slot name in the format, described near slots_create.
      //    b) (string) Paramline-encoded (encode1v) vector slot name (string) in the format, described near slots_create.
      //    c) decoded vector slot name (array).
      // Returns:
      //    1 - on_calm_result.
      //    0 - slot does not exist.
      //    -1 - invalid slotname.
      //    -2 - failed (mem. alloc. error, invalid object or smth. else).
      //    -3 - session is closed, no more requests, please.
    virtual s_long slot_remove(const unity& slotname) throw() = 0;

      // Get this thread name as registered in dispatcher.
      //  Thread names are unique within the initForCallback.
      //  Thread name cannot be empty string.
      //  retname: on on_calm_result, overwritten by the returned string.
      // Returns:
      //    1 - on_calm_result.
      //      In all the following, retname remains unchanged:
      //    -2 - failed (mem. alloc. error, invalid object or smth. else).
    virtual s_long thread_name(unity& retname) throw() = 0;

      // Get this initForCallback name as registered in dispatcher.
      //  ProcessAnalysis names are unique within the local machine.
      //  ProcessAnalysis name cannot be empty string.
      //  retname: on on_calm_result, overwritten by the returned string.
      // Returns:
      //    1 - on_calm_result.
      //      In all the following, retname remains unchanged:
      //    -2 - failed (mem. alloc. error, invalid object or smth. else).
    virtual s_long process_name(unity& retname) throw() = 0;

      // Subscribe, unsubscribe for messages, or check subscription state.
      //  addr_qs - messages source (full address as string or array),
      //  recv_sl - messages receiver in the current thread (slot name as string or array),
      //  rt - request type:
      //    1 - begin subscribe,
      //    2 - begin unsubscribe,
      //    3 - begin checking for being subscribed,
      //    4 - return the current state of operation,
      //    5 - discard the current operation (do not wait for result).
      //  Ops. 1, 2, 3 complete immediately if request target (addr_qs) is in-initForCallback.
      //  Returns:
      //    2 - op. completed, result: subscribed, or already subscribed.
      //    1 - op. completed, result: not subscribed, or just unsubscribed.
      //    0 - op. discarded or no operation is in progress with this addr_qs, recv_sl pair.
      //    10 - op. completed: slot, defined by addr_qs, does not exist on the destination side.
      //    11 - op. completed: addr_qs is unreachable (network and cross-initForCallback only).
      //    12 - op. completed: addr_qs does not allow subscribing recv_sl.
      //    13 - op. timeout (network and cross-initForCallback only).
      //    20 - op. is in progress (network and cross-initForCallback only).
      //    -1 - invalid args (address, slot name, request type).
      //    -2 - failure (out of memory, out of number of parallel requests, no network access etc.).
      //    -3 - session is closed, no more requests, please.
      //    -4 - another operation with same addr_qs, recv_sl is currently pending.
      //    -5 - recv_sl does not exist.
      // NOTE Subscriptions can be created
      //  a) statically, with the receiver slot configuration + corresponding qs slot configuration.
      //  b) dynamically, with subscribe() call from receiver's owning thread.
      //  (a) is automatically recreated on any side's restart.
      //  (b) disappears if any side's restarts, so the receiving side may periodically check
      //    and resubscribe as necessary.
    virtual s_long subscribe(const unity& addr_qs, const unity& recv_sl, s_long rt) throw() = 0;

      // Performs periodic tasks:
      //    - pop/deliver messages from thread's qs slots configured with delivery = thread.
      //    - (for disp thread) pop/deliver messages from qs slots with delivery = disp, in all threads.
      // periodic() should be called in a loop
      //    with some sleep at each iteration, during whole session.
      // periodic() call is not necessary in the following cases:
      //    a) the current thread has no qs slots with delivery = thread,
      //    b) the current thread is disp thread, but configuration contains __qsdt >= 0
      //      (automatic delivery for disp thread's messages by internal thread).
      // flags: reserved, must be 0.
      // Returns:
      //    1 - on_calm_result.
      //    0 - nothing to do for this thread.
      //    -1 - invalid flags.
      //    -2 - failed (mem. alloc. error or smth. else).
      //    -3 - session is closed, no more requests, please.
    virtual s_long periodic(s_long flags = 0) throw() = 0;

    virtual ~i_dispatcher_mt(){}
  };

  template<> struct o_proxy<i_dispatcher_mt> : o_proxy_base<i_dispatcher_mt>
  {
    static inline const char* __iname() { return "Ye.K. / bmdx / i_dispatcher_mt"; }

    struct __pop { typedef s_long (*PF)(__Interface* __pi, const unity* slotname, unity* retmsg, _carray_base_t<char>* retbuf, s_long flags); static s_long F(__Interface* __pi, const unity* slotname, unity* retmsg, _carray_base_t<char>* retbuf, s_long flags) { return __pi->pop(*slotname, *retmsg, retbuf, flags); } };
    struct __write { typedef s_long (*PF)(__Interface* __pi, const unity* msg, const arrayref_t<char>* buf); static s_long F(__Interface* __pi, const unity* msg, const arrayref_t<char>* buf) { return __pi->write(*msg, buf); } };
    struct __request { typedef s_long (*PF)(__Interface* __pi, s_long rt, unity* retval, const unity* args); static s_long F(__Interface* __pi, s_long rt, unity* retval, const unity* args) { return __pi->request(rt, *retval, *args); } };
    struct __slots_create { typedef s_long (*PF)(__Interface* __pi, const unity* slotscfg); static s_long F(__Interface* __pi, const unity* slotscfg) { return __pi->slots_create(*slotscfg); } };
    struct __slot_remove { typedef s_long (*PF)(__Interface* __pi, const unity* slotname); static s_long F(__Interface* __pi, const unity* slotname) { return __pi->slot_remove(*slotname); } };
    struct __thread_name { typedef s_long (*PF)(__Interface* __pi, unity* retname); static s_long F(__Interface* __pi, unity* retname) { return __pi->thread_name(*retname); } };
    struct __process_name { typedef s_long (*PF)(__Interface* __pi, unity* retname); static s_long F(__Interface* __pi, unity* retname) { return __pi->process_name(*retname); } };
    struct __subscribe { typedef s_long (*PF)(__Interface* __pi, const unity* addr_qs, const unity* recv_sl, s_long rt); static s_long F(__Interface* __pi, const unity* addr_qs, const unity* recv_sl, s_long rt) { return __pi->subscribe(*addr_qs, *recv_sl, rt); } };
    struct __periodic { typedef s_long (*PF)(__Interface* __pi, s_long flags); static s_long F(__Interface* __pi, s_long flags) { return __pi->periodic(flags); } };

    virtual s_long pop(const unity& slotname, unity& retmsg, _carray_base_t<char>* retbuf, s_long flags) throw() { return __call<__pop>()(__psi(), &slotname, &retmsg, retbuf, flags); }
    virtual s_long write(const unity& msg, const arrayref_t<char>* buf) throw() { return __call<__write>()(__psi(), &msg, buf); }
    virtual s_long request(s_long rt, unity& retval, const unity& args) throw() { return __call<__request>()(__psi(), rt, &retval, &args); }
    virtual s_long slots_create(const unity& slotscfg) throw() { return __call<__slots_create>()(__psi(), &slotscfg); }
    virtual s_long slot_remove(const unity& slotname) throw() { return __call<__slot_remove>()(__psi(), &slotname); }
    virtual s_long thread_name(unity& retname) throw() { return __call<__thread_name>()(__psi(), &retname); }
    virtual s_long process_name(unity& retname) throw() { return __call<__process_name>()(__psi(), &retname); }
    virtual s_long subscribe(const unity& addr_qs, const unity& recv_sl, s_long rt) throw() { return __call<__subscribe>()(__psi(), &addr_qs, &recv_sl, rt); }
    virtual s_long periodic(s_long flags) throw() { return __call<__periodic>()(__psi(), flags); }

    typedef unity_common::fn_list<__pop, __write, __request, __slots_create, __slot_remove, __thread_name, __process_name, __subscribe, __periodic> __Methods;
  };

  struct dispatcher_mt
  {
      // process_name - non-empty string, should be unique in the local machine context.
      //    Following names are reserved: LP, LM, R, LPA, LMA, RPA, RMA.
      // cfg:
      //  hash or map { thread name : non-empty string, thread configuration tree }
      //    Empty key for thread name is skipped.
      //    Min. required key in thread configuration is "slots", containing slots list.
      //    See arch_notes.txt "dispatcher_mt threads configuration structure" for details.
      //    See also slots_create() for bried descr. of slot types.
      // Results:
      // On successful construction, an internal session object is created.
      //    is_valid() will return true.
      //    The client creates proxy objects (new_proxy()), referring to the session.
      //    The client puts proxy objects into threads, so that threads could write and pop
      //      messages.
      // Session ends according with __exitmode parameter in configuration.
      //  (By default, session is ended by ~dispatcher_mt, then it exits immediately.
      //    All internal objects are released only after all proxy objects are released by the client.)
    dispatcher_mt(const std::wstring& process_name, const unity& cfg = unity()) throw();
    ~dispatcher_mt() throw();

      // true - on successful construction.
      // false - on construction failure, and after destruction.
    bool is_valid() const throw();

      // true from successful construction till destruction or end_session().
    bool has_session() const throw();

      // Resets an internal flag to signal that the session is no longer valid.
      //  Any call via any proxy interface will return error code "session is closed".
      // NOTE until being destroyed, dispatcher_mt holds the session object
      //  with all associated data.
    void end_session() throw();

      // Proxies with different thread names may reside in same or different
      //    processor threads freely.
      // NOTE Multiple proxies for one thread_name are allowed,
      //    but calling them is safe if
      //    a) they are called from single thread.
      //    b) subsets of slots addressed from different threads
      //      do not intersect.
      //    c) calls from different threads are additionally synchronized.
      // NOTE Multiple references to same proxy object (unity object copies)
      //    must be created/deleted in single thread context or if synchronized.
      // NOTE Proxy object destruction must not occur concurrent with calling
      //    any of its methods.
      // 1 - on_calm_result. dest contains the new object.
      //  In all the below cases, dest is cleared (empty).
      //  -1 - thread_name is invalid or not registered.
      //  -2 - failure (mem. alloc. error or smth. else).
      //  -3 - dispatcher is not valid.
    s_long new_proxy(unity& dest, const std::wstring& thread_name) const throw();


      // Same as i_dispatcher_mt::request (see),
      //  but fails when rt requires to be related to particular thread.
      // From other side, some functions (e.g. create/remove thread)
      //  succeed only with dispatcher_mt::request.
    s_long request(s_long rt, unity& retval, const unity& args = unity()) throw();

  private:
    struct cch_session; cref_t<cch_session> _r_ths;
    struct thread_proxy;
    dispatcher_mt(const dispatcher_mt&); dispatcher_mt& operator=(const dispatcher_mt&);
  };
#endif // bmdx_part_dispatcher_mt

}

namespace yk_c
{
  using namespace bmdx;
  template<> struct hashx_common::kf_basic<unity> : unity::kf_unity {};
  template <class Aux> struct meta::construct_f<unity, Aux> { s_long __fill; typedef unity t; inline void f(t* p) const { new (p) t(); } };
  template<> struct bytes::type_index_t<unity> : cmti_base_t<unity, 2015, 6, 1, 12, -1> {};
  template<int i> struct bytes::type_index_t<hashx_common::entry<unity, unity, i> > : cmti_base_t<hashx_common::entry<unity, unity, i>, 2015, 6, 10, 17 + i, -1> {};
}

#if defined(__clang__)
  #pragma clang diagnostic pop
#endif

#endif
