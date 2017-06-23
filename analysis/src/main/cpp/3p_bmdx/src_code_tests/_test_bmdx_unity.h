#ifndef _test_bmdx_unity_H
#define _test_bmdx_unity_H

//#include <ctime>
#include <cmath>
#include <sstream>
#include <clocale>
#include <limits.h>

#include "../src_code/bmdx_main.h"


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
// Manual pinterface test decls.
//===================================================

struct ITest
{
  virtual int test_sum(double x, double y) = 0;
  virtual int test_product(double x, double y) = 0;
};
struct ITest2
{
  virtual int test_sum(double x, double y) = 0;
  virtual int test_product(double x, double y) = 0;
};

struct TestClass: public ITest
{
  virtual int test_sum(double x, double y)
  {
    return int(x+y);
  }
  virtual int test_product(double x, double y)
  {
    return int(x*y);
  }
};

namespace bmdx
{
  template<> struct o_interfaces_top<TestClass> : public o_interfaces<TestClass, ITest> {};

  template<> struct o_proxy<ITest> : o_proxy_base<ITest>
  {
    static inline const char* __iname() { return "ITest"; }


    struct __test_sum
    {
      typedef int (*PF)(__Interface* __pi, double x, double y);
      static int F(__Interface* __pi, double x, double y) { return __pi->test_sum(x, y); }
    };
    virtual int test_sum(double x, double y) { return __call<__test_sum>()(__psi(), x, y); }

    struct __test_product
    {
      typedef int (*PF)(__Interface* __pi, double x, double y);
      static int F(__Interface* __pi, double x, double y) { return __pi->test_product(x, y); }
    };
    virtual int test_product(double x, double y) { return __call<__test_product>()(__psi(), x, y); }


    typedef unity_common::fn_list<__test_sum, __test_product> __Methods;
  };
  template<> o_proxy<ITest> o_proxy_base<ITest>::__server;
}


//===================================================
// AUTOMATIC TEST decls.
//===================================================

namespace bmdx
{
using namespace std;

//===================================================
// For vu_test_obj()
//===================================================

struct INTA
{
  virtual string msg1() const = 0;
  virtual int& ref_n() = 0;
  virtual string& ref_s() = 0;
  virtual ~INTA() {}
};

struct INTB
{
  virtual string msg2() const = 0;
  virtual void set_data(double x) const = 0;
  virtual double get_data() const = 0;
  virtual ~INTB() {}
};

struct INTC
{
  virtual string msg3b_prev() const = 0;
  virtual string msg3a_this() const = 0;
  virtual int& ref_sx() = 0;
  virtual void __on_before_destroy() = 0;
  virtual ~INTC() {}
};

struct INTA_alt {};
struct INTB_alt {};

struct CLSA
:
  public INTA,
  public INTB
{
  int n;
  string s;
  CLSA() : n(0), s() {}
  string msg1() const { return "CLSA INTA msg1"; }
  string msg2() const { return "CLSA INTB msg2"; }
  void set_data(double x) const {}
  double get_data() const { return -3.; }
  int& ref_n() { return n; }
  string& ref_s() { return s; }
};


template<>
struct o_iimpl<int, INTA_alt>
:
  public INTA,
  public o_iref<INTA>,
  public virtual o_interfaces<int>
{
  virtual INTA* __pMyI() { return this; }

  string msg1() const { return "o_iimpl<int, INTA_alt> msg1"; }
  int& ref_n()
  {
    INTA* pa = o_itfset_base::__pinterface_prev<INTA>(false);
    if (pa)
    {
      return pa->ref_n();
    }
    else
    {
      static int n(0);
      return n;
    }
  }
  string& ref_s()
  {
    INTA* pa = o_itfset_base::__pinterface_prev<INTA>(false);
    if (pa)
    {
      return pa->ref_s();
    }
    else
    {
      static string s(0);
      return s;
    }
  }
};

template<>
struct o_iimpl<CLSA, INTB_alt>
:
  public INTB,
  public o_iref<INTB>,
  public virtual o_interfaces<CLSA>
{
  virtual INTB* __pMyI() { return this; }

  string msg2() const { return "o_iimpl<CLSA, INTB_alt> msg2"; }
  virtual void set_data(double x) const { if (itf_data) *itf_data = x; }
  virtual double get_data() const { if (itf_data) return *itf_data; else return -1.; }

  double* itf_data;

  o_iimpl<CLSA, INTB_alt>() : itf_data(0) {}
  virtual void __on_after_init() { itf_data = new double(0); }
  virtual void __on_after_copy(o_itfset_base* src) { o_iimpl<CLSA, INTB_alt>* psrcb = dynamic_cast<o_iimpl<CLSA, INTB_alt>*>(src->__pinterface<INTB>()); itf_data = psrcb ? new double(*psrcb->itf_data): new double(-2.); }
  virtual void __on_before_destroy() { delete itf_data; itf_data = 0; }
};

template<>
struct o_iimpl<int, INTB_alt>
:
  public INTB,
  public o_iref<INTB>,
  public virtual o_interfaces<int>
{
  virtual INTB* __pMyI() { return this; }

  virtual string msg2() const { return "o_iimpl<int, INTB_alt> msg2"; }
  virtual void set_data(double x) const {}
  virtual double get_data() const { return -4.; }
};

template<class T>
struct o_iimpl<T, INTC>
:
  public INTC,
  public o_iref<INTC>,
  public virtual o_interfaces<T>
{
  virtual INTC* __pMyI() { return this; }

  static int& sx() { static int _sx(0); return _sx; }

  string msg3b_prev() const
  {
    INTB* pb = o_itfset_base::__pinterface_prev<INTB>(false);
    return pb ? "o_iimpl<T, INTC> msg3 + " + pb->msg2() : "o_iimpl<T, INTC> msg3";
  }
  string msg3a_this() const
  {
    INTA* pa = o_itfset_base::__pinterface_prev<INTA>(true);
    return pa ? "o_iimpl<T, INTC> msg3 + " + pa->msg1() : "o_iimpl<T, INTC> msg3";
  }
  int& ref_sx() { return sx(); }
  void __on_before_destroy() { ++sx(); }
};

template<> struct o_interfaces_top<CLSA> : public o_interfaces<CLSA, INTA, INTB> {};
struct o_interfaces_top_CLSA_alt1 : public o_interfaces<CLSA, INTA, INTB_alt, INTC> {};
struct o_interfaces_top_CLSA_alt2 : public o_interfaces<int, INTA_alt, INTB_alt, INTC> {};



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

template<class __anything = int> struct test_bmdx_unity : yk_tests::test_base_t<>
{
  typedef test_bmdx_unity t_test;
  base::stat_t<t_test> stat;
  static const char* c_logfile_name() { return "testlog_bmdx_unity.txt"; }
  test_bmdx_unity(const char* p_alt_fn = 0, bool log_trunc = true) : base(p_alt_fn ? p_alt_fn : c_logfile_name(), log_trunc), stat(*this) {  rnd_00() = 0; rnd_0() = 0; }


  //===================================================
  // Test-specific functions.
  //===================================================

    static bool& full_test() { static bool x(true); return x; }

        // Avg. number of descending links (object references or arrays)
        //    per 1 object-container or array (>=0, may be real number).
    double NDescLinks() { return 15; }
        //  <<1. if the random value is an array or objects, it may contain
        //    about NDescLinks descinding links, but this
        //    number decreases in geometrical progression: *NDescCoef.
    double NDescCoef() { return 0.8; }
        // Random double value precision (digits). 0 means not limited.
    int& NRndDoublePrec() { static int x = 0; return x; }
        // Min./max. array size
    s_long MinArS() { return 10; } s_long MaxArS() { return 25; }
        // Max. number of elements within an object-container
    s_long MaxColS() { return 25; }
        // Probability of creating a random sub-object as test array element.
    double PSubObject(double NDL, s_long colsize) { if (NDL >= colsize) return NDL / MaxColS(); return NDL / colsize; }
        // String utilities.
    template<class C> string str(C x) { ostringstream oss; oss<<x; return oss.str(); }
    string str(double x) { ostringstream oss; oss<<setprecision(18)<<x; return oss.str(); }
    template<class C> wstring wstr(C x) { ostringstream oss; oss<<x; return bsToWs(oss.str()); }
    wstring wstr(double x) { ostringstream oss; oss<<setprecision(18)<<x; return bsToWs(oss.str()); }
    wstring replicate_string(const wstring& s, s_long n) { wstring s2; for(s_long i=0; i<n; ++i)s2+=s; return s2; }

      // Ref. to test target object.
    static unity*& __pu0() { static unity* px(0); return px; }

    int vu_test_common() { return vu_test_common_2(*__pu0()); }
//      int vu_test_obj() // non-local testing is not designed in cur. ver.
    int vu_test_scalar_validity() { return vu_test_scalar_validity_2(*__pu0()); }
    int vu_test_array_validity() { return vu_test_array_validity_2(*__pu0()); }
    int vu_test_hash_validity() { return vu_test_hash_validity_2(*__pu0()); }
    int vu_test_map_validity() { return vu_test_map_validity_2(*__pu0()); }
//      int vu_ostream_test() // non-local testing is not designed in cur. ver.
    int vu_pl_test1() { return vu_pl_test1_2(*__pu0()); }
    int vu_pl_test2() { return vu_pl_test2_2(*__pu0()); }
    int vu_test_perf_map_and_hash() { return vu_test_perf_map_and_hash_2(*__pu0()); }

    int vu_test_common_2(unity& k)
    {
        unity k0; k0.arr_init<utInt>(10); k0.arrub_(20);
        k = k0.u_key();
        if (k.isArray()) { return -1; }
        if (k.val<utString>().find(L"array")==wstring::npos) { return -2; }
        k.arr_init<utFloat>(1); k.arrsz_(2);
        if (!k.isArray()) { return -3; }
        k.u_name_set(k);
        if (!k.isArray()) { return -4; }
        if (k.u_name().isArray()) { return -5; }
        k.u_name().clear();
        if (k.u_name().ref<utString>().find(L"array") == wstring::npos) { return -6; }
        if (k.u_copy().arrub() != 2) { return -20; }
        if (k.u_copy().pval<utFloat>(2) == k.pval<utFloat>(2)) { return -21; }
        if (k.u_copy().ref<utFloat>(2) != k.ref<utFloat>(2)) { return -22; }

        k.u_assign(k0);
          k0.arr_set(19, 5);
          unity k2; k2.u_assign(k0);
        if (k.u_copy().arrub() != 20) { return -23; }
        if (k2.u_copy().arrub() != 20) { return -24; }
        if (k2.u_copy().ref<utInt>(19) == k.ref<utInt>(19)) { return -25; }
        if (k2.val<utChar>(19) - k.val<utChar>(19) != 5) { return -26; }
        if (k.u_name().ref<utString>().find(L"array") == wstring::npos) { return -27; }

        if (k.u_clear(utDateArray).utype() != utDateArray) { return -16; }
          k.arrlb_(2);
          if (k.u_clear(utDateArray, 0).utype() != utDateArray) { return -17; }
          if (k.u_clear(-1, 0).arrlb() != 2) { return -15; }

        if (!k.ua_clear(true).isArray())return -7;
        if (!k.map_clear(true).isMap())return -8;
        if (!k.hash_clear(true).isHash())return -9;
        if (!k.sc_clear().isEmpty())return -13;

        if (!k.ua_ensure().isArray())return -10;
        if (!k.map_ensure().isMap())return -11;
        if (!k.hash_ensure().isHash())return -12;
        if (!k.sc_ensure().isEmpty())return -14;

        return 1;
    }

    int vu_test_scalar_validity_2(unity & u)
    {
        u.clear();
          if (!u.sc_empty()) { return -1; }
        u.ua_clear(true);
        u.ua_append(123);
          if (!u.sc_empty()) { return -2; }
          if (!u.isArray()) { return -3; }
        u.map_clear(true);
        u.map("k1") = "v1";
          if (!u.sc_empty()) { return -4; }
          if (!u.isMap()) { return -5; }
        u.sc_ensure();
          if (!u.sc_empty()) { return -6; }
          if (!u.isEmpty()) { return -7; }
        u.ua_ensure(); u = 25;
          if (u.sc_empty()) { return -8; }
          if (u.utype() != utInt) { return -9; }
          if (u.val<utString>() != L"25") { return -10; }
        u.sc_clear();
          if (!u.sc_empty()) { return -11; }
          if (!u.isEmpty()) { return -12; }
        unity temp(10); temp = u; temp.rx<utInt>() += temp.cref<utInt>().ref() + 25;
          if (temp.ref<utInt>() != 25) { return -13; }
        return 1;
    }

    int vu_test_array_validity_2(unity& a)
    {
        a.clear(); unity x1;

        a.ua_set(1, a.ua_has(1));
        a.ua_set(2, a.ua_has(1));
        a.ua_set(3, a.ua_empty(2));
        a.ua_set(-1, a.uaS());
        a.ua_set(-2, a.uaS());
        a.ua_resize(0, -2);
        a.ua_resize(0, 3, string("inserted"));

        if (a.uaLb() != -2)return -1;
        if (a.uaUb() != 4)return -2;

        if (a.ua_val(-2).utype() != utInt)return -3;
        if (a.ua_val(-1).utype() != utInt)return -15;
        if (a.ua_val(0).utype() != utString)return -4;
        if (a.ua_val(1).utype() != utString)return -5;
        if (a.ua_val(2).utype() != utString)return -6;
        if (a.ua_val(3).utype() != utChar)return -7;
        if (a.ua_val(4).utype() != utChar)return -8;

        try { if (a.ua_val(-2).ref<utInt>() != 5)return -9; } catch (...) { return -209; }
        try { if (a.ua_val(-1).ref<utInt>() != 3)return -10; } catch (...) { return -210; }
        try { if (a.ua_val(0).cref<utString>().ref() != L"inserted")return -11; } catch (...) { return -211; }
        try { if (a.ua_val(1).cref<utString>().ref() != L"inserted")return -12; } catch (...) { return -212; }
        try { if (a.ua_val(2).cref<utString>().ref() != L"inserted")return -13; } catch (...) { return -213; }
        try { if (bool(a.ua_val(3).ref<_unitychar>()) != true)return -14; } catch (...) { return -214; }
        try { if (bool(a.ua_val(4).ref<_unitychar>()) != false)return -16; } catch (...) { return -216; }

        if (a.ua_val(-2) != a.ua_val_first()) { return -79; }
        if (a.ua_val(4) != a.ua_val_last()) { return -78; }

        x1 = a;
        x1.rx<utUnity>(-2) = x1.rx<utInt>(-2) + 1.5;
        x1.rx<utUnity>(1).set_obj(*new unity, true);
        x1.rx<utUnity>(1).u_name_set("object1");
        a.u_assign(x1);
        x1.clear();
        a.ua_resize(4, -1).ua_resize(4, 1, "false");

        if (a.uaLb() != -2)return -31;
        if (a.uaUb() != 4)return -32;

        if (a.ua_val(-2).utype() != utFloat)return -17;
        if (a.ua_val(-1).utype() != utInt)return -18;
        if (a.ua_val(0).utype() != utString)return -19;
        if (a.ua_val(1).utype() != utObject)return -20;
        if (a.ua_val(2).utype() != utString)return -21;
        if (a.ua_val(3).utype() != utChar)return -22;
        if (a.ua_val(4).utype() != utString)return -23;

        try { if (a.ua_val(-2).ref<utFloat>() != 6.5) return -24; } catch (...) { return -224; }
        try { if (a.ua_val(-1).ref<utInt>()!=3) return -25; } catch (...) { return -225; }
        try { if (a.ua_val(0).cref<utString>().ref() != L"inserted") return -26; } catch (...) { return -226; }
        try { if (!(a.ua(1).objPtr<unity>() || (a.ua(1).isObject() && !a.ua(1).isLocal()))) return -27; } catch (...) { return -227; }
        try { if (a.ua_val(2).cref<utString>().ref() != L"inserted") return -28; } catch (...) { return -228; }
        try { if (bool(a.ua_val(3).ref<_unitychar>()) != true) return -29; } catch (...) { return -229; }
        try { if (a.ua_val(4).cref<utString>().ref() != L"false") return -33; } catch (...) { return -233; }

        x1 = a;
        x1.arrlb_(-2);
        x1.arrub_(11);
        a.ua_fill(utInt).uaS_set(11, 126).ua_set(5,120);
        x1.rx<utUnity>(2) = a.uaLb_set(-5).ua_val(2);
        x1.rx<utUnity>(6) = a.ua_val(1);
        x1.rx<utUnity>(7) = a.ua_val(3);
        a.ua_fill(utString); bool is_astr(true);
        if (!a.isLocal() && a.ref<utStringArray>().compatibility() <= 0)
        {
          a.ua_fill(utUnity); is_astr = false;
        }
        a.ua_set(-3,"vbString");
        x1.rx<utUnity>(8) = a.ua_val(8 - a.uaS());
        x1.rx<utUnity>(9) = a.ua_val(4);

        if (a.uaLb() != -5) { return -37; }
        if (a.uaUb() != 5) { return -38; }

        if (is_astr)
        {
          if (a.ua_val(-5).utype() != utString)return -34;
          if (a.ua_val(-3).utype() != utString)return -83;
          if (a.ua_val(0).utype() != utString)return -35;
          if (a.ua_val(5).utype() != utString)return -36;
        }
        else
        {
          if (a.ua_val(-3).utype() != utString)return -84;
          if (a.ua_val(-5).utype() != utEmpty)return -85;
          if (a.ua_val(5).utype() != utEmpty)return -86;
        }

        try { if (a.ua_val(-5).cref<utString>().ref() != L"") return -39; } catch (...) { return -239; }
        try { if (a.ua_val(-4).cref<utString>().ref() != L"") return -40; } catch (...) { return -240; }
        try { if (a.ua_val(-3).cref<utString>().ref() != L"vbString") return -41; } catch (...) { return -241; }
        try { if (a.ua_val(-2).cref<utString>().ref() != L"") return -42; } catch (...) { return -242; }
        try { if (a.ua_val(-1).cref<utString>().ref() != L"") return -43; } catch (...) { return -243; }
        try { if (a.ua_val(0).cref<utString>().ref() != L"") return -44; } catch (...) { return -244; }
        try { if (a.ua_val(1).cref<utString>().ref() != L"") return -45; } catch (...) { return -245; }
        try { if (a.ua_val(2).cref<utString>().ref() != L"") return -46; } catch (...) { return -246; }
        try { if (a.ua_val(3).cref<utString>().ref() != L"") return -47; } catch (...) { return -247; }
        try { if (a.ua_val(4).cref<utString>().ref() != L"") return -48; } catch (...) { return -248; }
        try { if (a.ua_val(5).cref<utString>().ref() != L"") return -49; } catch (...) { return -249; }

        x1.arr_set(10, a.ua_val(11));
        x1.arr_set(10, x1.val<utString>(10) + unity(a.uaUb()).val<utString>());
        a.ua_fill(utString, L"fill_str");


        if (a.uaLb() != -5) return -50;
        if (a.uaUb() != 11) return -51;

        for(int i = -5; i <= 11; ++i)
        {
            try { if (a.ua_val(i).cref<utString>().ref() != L"fill_str") return -52; } catch (...) { return -252; }
        }

        x1.rx<utUnity>(11)
          = a.ua_val(11).val<utString>()
            +unity(a.ua_val(12).val<utString>().length()).val<utString>();
        a.u_assign(x1);
        a.ua_resize(-5, -4);
        a.ua_resize(-2, -1);
        a.ua_set(3, a.ua_val(0).utype()).uaLb_set(0);
        a.ua_resize(a.uaUb(), 1, 1.2345);
        a.ua_resize(a.uaUb(), 1);
        a.ua_resize(a.uaUb() + 1, 1, 2.2345);
        a.ua_append(a.ua_first());
        a.uaLb_set(2).ua_resize(2, -1).uaLb_set(-1);
        a.ua(10) = a.ua_empty(10);
        a.ua(14) = a.ua_empty(10);

        if (a.uaLb() != -1) return -53;
        if (a.uaUb() != 14) return -54;

        unity& temp3 = a.ua(-1);
        try { if (!(temp3.objPtr<unity>() || (temp3.isObject() && !temp3.isLocal()))) { return -55; } } catch (...) { return -255; }
        try { if (a.ua_val(2).cref<utString>().ref() != L"false") return -56; } catch (...) { return -256; }
        try { if (a.ua_val(6).cref<utString>().ref() != L"vbString") return -57; } catch (...) { return -257; }
        try { if (a.ua_val(7).cref<utString>().ref() != L"") return -58; } catch (...) { return -258; }
        try { if (a.ua_val(8).cref<utString>().ref() != L"11") return -59; } catch (...) { return -259; }
        try { if (a.ua_val(11).cref<utString>().ref() != L"fill_str0") return -60; } catch (...) { return -260; }
        try { if (a.ua_val(13).cref<utString>().ref() != L"inserted") return -61; } catch (...) { return -261; }

        try { if (a.ua_val(0).ref<utInt>() != 120) return -62; } catch (...) { return -262; }
        try { if (a.ua_val(3).ref<utInt>() != 1) return -64; } catch (...) { return -264; }
        try { if (a.ua_val(4).ref<utInt>() != 0) return -65; } catch (...) { return -265; }
        try { if (a.ua_val(5).ref<utInt>() != 126) return -68; } catch (...) { return -268; }
        try { if (bool(a.ua_val(1).ref<_unitychar>()) != true) return -63; } catch (...) { return -263; }
        try { if (bool(a.ua_val(10).ref<_unitychar>()) != true) return -69; } catch (...) { return -269; }
        try { if (bool(a.ua_val(14).ref<_unitychar>()) != false) return -70; } catch (...) { return -270; }
        try { if (fabs(a.ua_val(9).ref<utFloat>()-1.2345) > 1.e-12) return -66; } catch (...) { return -266; }
        try { if (fabs(a.ua_val(12).ref<utFloat>()-2.2345) > 1.e-12) return -71; } catch (...) { return -271; }


        a.ua(-10).u_name_set("qq");
        if (!(a.ua_canref(-10) && a.ua_has(-10) && a.ua_empty(-10))) return -72;
        if (!(a.ua_canref(20) && !a.ua_has(20) && a.ua_empty(20))) return -73;
        a.ua(20) = "ww";
        if (!(a.ua_canref(20) && !a.ua_empty(20))) return -74;
        unity& temp4 = a.ua(20);
        unity& temp5 = a.ua(-10);
        if (temp4.val<utString>() != L"ww") return -81;
        if (temp5.u_name().val<utString>() != L"qq") return -82;
        a.ua_resize(-10, -10);
        a.uaLb_set(0);
        a.ua_resize(3, -100);
        try { if (a.ua_first().ref<utInt>() != 120) return -75; } catch (...) { return -275; }
        try { if (a.ua_last().cref<utString>().ref() != L"false") return -76; } catch (...) { return -276; }
        if (!a.ua_has(a.uaLb())) return -77;
        if (!a.ua_has(a.uaUb())) return -80;

        return 1;
    }

    template<class T, class _ = meta::nothing> struct _test_hash_validity_gen_rnd { static inline T F(t_test& t, s_long N1, s_long i, s_long flags) { return unity(s_long(d_nowutc(true).f() / 2e8 + rnd() * N1) % (N1 / 10)).val<T>(); } };
    template<class _> struct _test_hash_validity_gen_rnd<unity, _> { typedef unity T; static inline T F(t_test& t, s_long N1, s_long i, s_long flags) { return t.random_v(utUnity); }  };

    template<class T> int _test_hash_validity(unity& h, s_long N1, s_long flags)
    {
      // hash_locate, hash_set, hash, hashS, hashi, hashi_key, hash_del,
      // hashl_next, hashl_prev, hashl_key, hashl
      // hash_ensure, hash_clear, hash_empty, hashi_has, hashi_empty
      // hash_keys, hash_values, hashFlags, hashFlags_set, fk*

      h.clear();
      if (true)
      {
        vector<T> kk;
        vector<T> kk2;
        typedef typename vector<T>::const_iterator Iter;

        h.hashFlags_set(-1, flags);

        s_long cnt(0);
        for (int i = 0; i < N1; ++i)
        {
          kk.push_back(_test_hash_validity_gen_rnd<T>::F(*this, N1, i, flags));
          double r = rnd();
          if (r < 0.2)
          {
            bool b = h.hash_set(kk[i], double(i+1), true);
            if (b)
            {
              cnt += 1; kk2.push_back(kk[i]);
              if (h.hashS() != cnt) { return -100; }
              unity& x3 = h.hash(kk[i]); if (h.hashS() != cnt) { return -101; }
              meta::s_ll j = x3.val<utInt>(); if (j != i + 1) { return -102; }
            }
            else
            {
              Iter ia = std::find(kk.begin(), kk.end() - 1, kk[i]); if (ia == kk.end() - 1) { return -110; }
              if (h.hashS() != cnt) { return -111; }
              unity& x3 = h.hash(kk[i]); if (h.hashS() != cnt) { return -112; }
              meta::s_ll j = x3.val<utInt>(); if (j < 1 || j >= i + 1) { return -113; }
              x3 = (ia - kk.begin()) + 1;
            }
          }
          else if (r < 0.4)
          {
            bool b = h.hash_set(kk[i], double(i+1), false);
            if (b)
            {
              cnt += 1; kk2.push_back(kk[i]);
              if (h.hashS() != cnt) { return -120; }
              unity& x3 = h.hash(kk[i]); if (h.hashS() != cnt) { return -121; }
              meta::s_ll j = x3.val<utInt>(); if (j != i + 1) { return -122; }
            }
            else
            {
              Iter ia = std::find(kk.begin(), kk.end() - 1, kk[i]); if (ia == kk.end() - 1) { return -130; }
              if (h.hashS() != cnt) { return -131; }
              unity& x3 = h.hash(kk[i]); if (h.hashS() != cnt) { return -132; }
              meta::s_ll j = x3.val<utInt>(); if (j != i + 1) { return -133; }
              x3 = (ia - kk.begin()) + 1;
            }
          }
          else if (r < 0.7)
          {
            bool b0 = !h.hash_locate(kk[i]);
            unity& x3 = h.hash(kk[i]); bool b = x3.isEmpty();
            if (b != b0) { return -149; }
            if (b)
            {
              cnt += 1; kk2.push_back(kk[i]);
              x3 = double(i + 1);
              if (h.hashS() != cnt) { return -140; }
              unity& x4 = h.hash(kk[i]); if (h.hashS() != cnt) { return -141; }
              if (&x4 != &x3) { return -142; }
            }
            else
            {
              Iter ia = std::find(kk.begin(), kk.end() - 1, kk[i]); if (ia == kk.end() - 1) { return -145; }
              if (h.hashS() != cnt) { return -146; }
              unity& x4 = h.hash(kk[i]); if (h.hashS() != cnt) { return -147; }
              if (&x4 != &x3) { return -148; }
              meta::s_ll j = x4.val<utInt>(); if (j < 1 || j >= i + 1) { return -149; }
              x3 = (ia - kk.begin()) + 1;
            }
          }
          else
          {
            bool b = h.hash_locate(kk[i], true);
            if (b)
            {
              cnt += 1; kk2.push_back(kk[i]);
              if (h.hashS() != cnt) { return -150; }
              unity& x3 = h.hashi(cnt); if (!x3.isEmpty()) { return -151; }
              x3 = double(i + 1);
            }
            else
            {
              Iter ia = std::find(kk.begin(), kk.end() - 1, kk[i]); if (ia == kk.end() - 1) { return -155; }
              unity& x3 = h.hash(kk[i]); if (h.hashS() != cnt) { return -156; }
              meta::s_ll j = x3.val<utInt>(); if (j < 1 || j >= i + 1) { return -157; }
              x3 = (ia - kk.begin()) + 1;
            }
          }
          unity& x1 = h.hashi(cnt); unity& x2 = h.hashl(cnt - 1); const unity& k1 = h.hashi_key(cnt); const unity& k2 = h.hashl_key(cnt - 1);
            if (&x1 != &x2) { return -165; } if (&k1 != &k2) { return -166; } if (s_long(kk2.size()) != cnt || k1 != kk2[cnt-1]) { return -167; }
        }

        if (h.hashS() < 2) { return -308; } // hint: bad rnd

        for (int i = 0; i < N1; ++i)
        {
          if (!h.hash_locate(kk[i]))
          {
            log << "Hash is missing a key/value pair (key type only double). i, kk[i]:" << " " << i << " " << kk[i] << endl;
            return -3;
          }
        }

        s_long i, i2;

//        log.d, h._phash()->_list.n(), h._phash()->_indk.n();
//        for (i = h._phash()->_list.nbase(); i < h._phash()->_list.nbase() + h._phash()->_list.n(); ++i)
//        {
//          log.d, i, h._phash()->qi_prev(i), h._phash()->qi_next(i);
//          if (i > 10) { break; }
//        }

        i = 0; i2 = 0;
        for (s_long pos = h.hashl_first(); pos != h.hashl_noel(); pos = h.hashl_next(pos))
        {
          try{
            if (h.hashl(pos).val<utInt>() <= i) { return -217; }
            i = s_long(h.hashl(pos).val<utInt>());
          } catch (...) { return -218; }
          ++i2;
        }
        if (i2 != h.hashS()) { return -219; }

        i = kk.size() + 1; i2 = 0;
        for (s_long pos = h.hashl_last(); pos != h.hashl_noel(); pos = h.hashl_prev(pos))
        {
          try{
            if (h.hashl(pos).val<utInt>() >= i) { return -220; }
            i = s_long(h.hashl(pos).val<utInt>());
          } catch (...) { return -221; }
          ++i2;
        }
        if (i2 != h.hashS()) { return -222; }

        h.hash_del(kk[0]); cnt -= 1;
          if (h.hashS() != cnt) { return -200; }
          if (h.hash_locate(kk[0])) { return -210; }
        i = 1; i2 = 0;
        for (s_long pos = h.hashl_first(); pos != h.hashl_noel(); pos = h.hashl_next(pos))
        {
          try{
            if (i >= s_long(kk2.size())) { return -201; }
            if (h.hashl_key(pos) != kk2[i]) { return -203; }
            if (&h.hashi_key(i) != &h.hashl_key(pos)) { return -202; }
            if (kk2[i] == kk[i] && !(h.hashl(pos).val<utInt>() == i + 1)) { return -204; }
            if (kk2[i] != kk[i] && !(h.hashl(pos).val<utInt>() > i + 1)) { return -205; }
          } catch (...) { return -250; }
          ++i2; ++i;
        }
        if (i2 != h.hashS()) { return -251; }

        for (i = 0; i < N1 / 3; ++i)
        {
          cnt -= s_long(h.hash_del(kk[typename vector<T>::size_type(rnd() * kk.size())]));
        }
        if (h.hashS() != cnt) { return -211; }

        i = 1; i2 = 0;
        for (s_long pos = h.hashl_first(); pos != h.hashl_noel(); pos = h.hashl_next(pos))
        {
          try{
            if (h.hashl(pos).val<utInt>() <= i) { return -212; }
            i = s_long(h.hashl(pos).val<utInt>());
          } catch (...) { return -213; }
          ++i2;
        }
        if (i2 != h.hashS()) { return -214; }

        i = kk.size() + 1; i2 = 0;
        for (s_long pos = h.hashl_last(); pos != h.hashl_noel(); pos = h.hashl_prev(pos))
        {
          try{
            if (h.hashl(pos).val<utInt>() >= i) { return -223; }
            i = s_long(h.hashl(pos).val<utInt>());
          } catch (...) { return -224; }
          ++i2;
        }
        if (i2 != h.hashS()) { return -225; }

        for (i = 1; i <= h.hashS(); ++i)
        {
          try{
            if (h.hashi(i).val<utInt>() <= i) { return -215; }
            i = s_long(h.hashi(i).val<utInt>());
          } catch (...) { return -216; }
        }

        unity h2 = h;
        if (!h2.isHash()) { return -300; }
        if (h2 != h) { return -301; }
        if (true)
        {
          unity _v1, _v2;
          _v1 = h2.hash_keys(0);
          _v2 = h2.hash_values(0);
          for (s_long i = 1; i <= h2.hashS(); ++i)
          {
            if (h2.hashi_key(i) != h.hashi_key(i)) { return -302; }
            if (h2.hashi(i) != h.hashi(i)) { return -303; }
            if (_v1.val<utUnity>(i - 1) != h.hashi_key(i)) { return -310; }
            if (_v2.val<utUnity>(i - 1) != h.hashi(i)) { return -311; }
          }
          _v1 = h.hash_keys(0);
          _v2 = h.hash_values(0);
          for (s_long i = 1; i <= h2.hashS(); ++i)
          {
            if (_v1.val<utUnity>(i - 1) != h.hashi_key(i)) { return -312; }
            if (_v2.val<utUnity>(i - 1) != h.hashi(i)) { return -313; }
          }
        }

        h2.u_name_set("abc");
        h2.hashFlags_set(-1, 1);
        h2.hash_clear(true);
          if (h2.u_name() != "abc") { return -340; }
          if (h2.hashFlags() != unity::kf_unity().flags()) { return -341; }
        h2.hashFlags_set(-1, 4+1);
        h2.hash_clear(false);
          if (h2.u_name() != "abc") { return -342; }
          if (h2.hashFlags() != 4+1) { return -343; }
        h2.hash(unity()) = 1;
        h2.hash(2);
          if (h2.hash_empty(unity())) { return -348; }
          if (!h2.hash_empty(1)) { return -349; }
          if (!h2.hash_empty(2)) { return -350; }
          if (h2.hashi_empty(1)) { return -351; }
          if (!h2.hashi_empty(2)) { return -352; }
          if (h2.hashi_empty(3)) { return -353; }
          if (h2.hashi_has(0) || h2.hashi_has(-1) || h2.hashi_has(3)) { return -354; }
          if (!h2.hashi_has(1) || !h2.hashi_has(2)) { return -355; }
        h2.clear();
          if (h2.u_name() != unity()) { return -344; }
          if (!h2.isEmpty()) { return -345; }
          if (h2.hashFlags() != unity::kf_unity().flags()) { return -346; }
          if (!h2.isHash()) { return -347; }

        for (s_long i = 0; i < s_long(kk2.size()); ++i) { h2.hash(kk2[i]) = i + 1; if (h2.hashS() != i + 1) { return -304; } }
        for (s_long i = 1; i <= h.hashS(); ++i) { if (!h2.hash_locate(h.hashi_key(i))) { return -305; } }

        if (unity::reference_t<T>::utt == utUnity)
        {
          if (rnd() < 0.3)
          {
            kk2[0] = unity("__ABC199").val<T>();
            kk2[1] = unity("__aBc199").val<T>();
          }
          else if (rnd() < 0.3)
          {
            kk2[0] = unity((1ll << 60) + 1).val<T>();
            kk2[1] = unity((1ll << 60) + 2).val<T>();
          }
          else
          {
            kk2[0] = unity().u_clear(utUnityArray).val<T>();
            kk2[1] = unity().ua_clear(true).arrlb_(-1).val<T>();
          }
        }
        else if (unity::reference_t<T>::utt == utString)
        {
          kk2[0] = unity("__ABC199").val<T>();
          kk2[1] = unity("__aBc199").val<T>();
        }
        else if (unity::reference_t<T>::utt == utInt)
        {
          kk2[0] = unity((1ll << 60) + 1).val<T>();
          kk2[1] = unity((1ll << 60) + 2).val<T>();
        }

        s_long delta = 0;
        s_long fwcnt = 0;
        s_long fwcnt_exact = 0;
        for (s_long j = 0; j < unity::_fkcmp_n; ++j)
        {
          h2.clear();
          for (s_long i = 0; i < s_long(kk2.size()); ++i) { h2.hash(kk2[i]) = i + 1; if (h2.hashS() != i + 1) { return -30600 - j; } }
          delta += s_long(kk2.size()) - h2.hashS();
          h2.hashFlags_set(-1, j);
          for (s_long i = 0; i < s_long(kk2.size()); ++i) { if (!h2.hash_locate(kk2[i])) { return -30700 - j; } }
            bool b1 = unity::kf_unity(unity::fkcmpNFloat | unity::fkcmpScalarize | unity::fkcmpSNocase).is_eq(h2.hashl_key(h2.hashl_first()), h2.hashl_key(h2.hashl_next(h2.hashl_first())));
            bool b2 = unity::kf_unity(h2.hashFlags()).is_eq(kk2[0], kk2[1]);
            fwcnt += b1;
            fwcnt_exact += b2;
//if (unity::reference_t<T>::utt == utInt)
//{
//  log.d, b1, b2,
//  h2.hashl_key(h2.hashl_first()), h2.hashl_key(h2.hashl_next(h2.hashl_first())),
//  h2.hashl_key(h2.hashl_first()).val<utFloat>(csLazy), h2.hashl_key(h2.hashl_next(h2.hashl_first())).val<utFloat>(csLazy),
//  h2.hashl_key(h2.hashl_first()).val<utFloat>(csLazy) == h2.hashl_key(h2.hashl_next(h2.hashl_first())).val<utFloat>(csLazy),
//  unity::kf_unity(unity::fkcmpNFloat | unity::fkcmpScalarize | unity::fkcmpSNocase).is_eq(h2.hashl_key(h2.hashl_first()), h2.hashl_key(h2.hashl_next(h2.hashl_first()))),
//  kk2[0], kk2[1]
//  ;
//}
        }

        if (unity::reference_t<T>::utt == utUnity || unity::reference_t<T>::utt == utString || (unity::reference_t<T>::utt == utInt && (h2.hashFlags() & unity::fkcmpNFloat)))
        {
          if (!(fwcnt == unity::_fkcmp_n / 2)) { return -360; }
          if (!(fwcnt_exact == unity::_fkcmp_n / 2)) { return -361; }
        }
        else
        {
          if (!(fwcnt == 0)) { return -362; }
          if (!(fwcnt_exact == 0)) { return -363; }
        }
      }

      return 1;
    }

    int vu_test_hash_validity_2(unity& h)
    {
      s_long res; s_long ecnt = 0; enum { N1 = 20000, N2 = 5000 }; s_long i = 0;
      res = _test_hash_validity<double>(h, N1, 0); ++i; ecnt += s_long(res < 0); if (res < 0) { log.d, "vu_test_hash_validity_2[", i, "]:", res; }
      res = _test_hash_validity<unity>(h, N1, 0); ++i; ecnt += s_long(res < 0); if (res < 0) { log.d, "vu_test_hash_validity_2[", i, "]:", res; }
      res = _test_hash_validity<wstring>(h, N1, 0); ++i; ecnt += s_long(res < 0); if (res < 0) { log.d, "vu_test_hash_validity_2[", i, "]:", res; }
      res = _test_hash_validity<meta::s_ll>(h, N1, 0); ++i; ecnt += s_long(res < 0); if (res < 0) { log.d, "vu_test_hash_validity_2[", i, "]:", res; }
      res = _test_hash_validity<_unitychar>(h, N1, 0); ++i; ecnt += s_long(res < 0); if (res < 0) { log.d, "vu_test_hash_validity_2[", i, "]:", res; }
      res = _test_hash_validity<_unitydate>(h, N1, 0); ++i; ecnt += s_long(res < 0); if (res < 0) { log.d, "vu_test_hash_validity_2[", i, "]:", res; }

      return ecnt > 0 ? - ecnt : 1;
    }

    int vu_test_map_validity_2(unity& h)
    {
        s_long result(0); string errm;

        double t1=clock_ms();

        try { xvu_test_map_insdel(h, result, errm); }
          catch (exception& e) { log.d, "xvu_test_map_insdel exception: ", e.what(); return -1101; }
          catch (...) { log.d, "xvu_test_map_insdel unknown exception"; return -1102; }

        double t2=clock_ms();
        log<<"xvu_test_map_insdel: time="<<t2-t1<<" ms (norm 600)"<<endl;

        if (result<0)
        {
            log<<" error="<<errm<<endl;
            return result;
        }

        return 1;
    }

    void xvu_test_map_insdel(unity& ptA, s_long& result, string& errm)
    {
        s_long my_res(0); string my_errm;

        const s_long turns(800), minsize(0), maxsize(20);
        const double pdel(0.5); //probability of deleting element from whole (i.e. placing into the list A)

        s_long __n(0); bool del_success(false);

        ptA.clear();
        unity ptB, whole; //parts A, B, and the whole
        //1. Into the whole, a random unique key/value are inserted,
        //  they are also inserted into whether A or B.
        //2. From the whole, one by one, all elements are deleted, which exist in A.
        //  Sometimes, a single call deletes several elements at the end of whole,
        //  before this, they are deleted from A and B. After, the rest is moved
        //  from A to B.
        //3. As the result, whole must be equal to B.

        s_long flags(0);

        s_long ia(0), ib(0), ic(0);
        unity k, v;
//part 1
        for(ia=1; ia<=turns; ++ia)
        {
            ptA=unity();
            ptB=unity();
            whole=unity();
            if (rnd()<0.5) flags ^= unity::fkcmpSNocase;
            if (rnd()<0.5) flags ^= unity::fkcmpRevOrder;
            if (rnd()<0.5) flags ^= unity::fkcmpScalarize;

            ptA.mapFlags_set(-1, flags);
            ptB.mapFlags_set(-1, flags);
            whole.mapFlags_set(-1, flags);

            bool wholeOrdReversed = false;

            __n = s_long(minsize + floor(rnd() * (maxsize - minsize + 1)));

            for(ib=1; ib<=__n; ++ib)
            {
                random_kv(k, v, 0, 0, 0, 0);
                if (whole.map_locate(k, true, ic)) { //new element
                    //Checking mapi_key:
                    whole.u_name_set(k); k.clear();
                    k = whole.mapi_key(ic);
                    if (whole.u_name() != k) { my_res = -1; my_errm = "mapi_key error"; goto lError; }
                    if (rnd() < 0.5) { whole.mapi(ic) = v; }
                      else { whole.map_append(k, v); }
                    if (rnd() < pdel) { ptA.map(k) = v; }
                      else { ptB.map(k) = v; }
                }
            }
            if (whole.mapS() != ptB.mapS() + ptA.mapS()) { my_res=-2; my_errm="the number of elements after the insertion is different"; goto lError; }

//part 2
            if (rnd()<0.5)
            {
                if (rnd()<0.5)
                {
                    wholeOrdReversed = true;
                    whole.mapFlags_set(unity::fkcmpRevOrder, (~ptB.mapFlags()) & unity::fkcmpRevOrder);
                }

                ic = s_long(floor((whole.mapS() * (1 - pdel)))); //new size (after resize)
                for(ib = ic + 1; ib <= whole.mapS(); ++ib)
                {
                  del_success = ptA.map_del(whole.mapi_key(ib)) != ptB.map_del(whole.mapi_key(ib));
                  if (!del_success) { my_res = -3; my_errm = "parity error while deleting"; goto lError; }
                }
                whole.mapS_set(ic);

                if ( whole.mapS() != ptB.mapS() + ptA.mapS() ) { my_res = -4; my_errm = "the number of elements does not match"; goto lError; }

                for(ib=1; ib<=ptA.mapS(); ++ib)
                {
                    ptB.map(ptA.mapi_key(ib)) = ptA.map(ptA.mapi_key(ib));
                }
                ptA.map_clear(false);
                if (whole.mapS()!=ptB.mapS() + ptA.mapS()) { my_res = -5; my_errm = "the number of elements does not match (loc. 2)"; goto lError; }
            }
            else
            {
                ib=1;
                while(ib<=ptA.mapS())
                {
                    if (rnd()<0.5)
                    {
                        del_success=whole.map_del(ptA.mapi_key(ib));
                        if (!del_success) { my_res = -6; my_errm = "the element has not been deleted"; goto lError; }
                        ib = ib + 1;
                    }
                    else
                    {
                        del_success=whole.map_del(ptA.mapi_key(ib));
                        if (!del_success) { my_res = -7; my_errm = "the element has not been deleted (loc. 2)"; goto lError; }
                        ptA.mapi_del(ib);
                    }
                }
            }

//part 3
            if (whole.mapS() != ptB.mapS()) { my_res = -8; my_errm = "the number of elements does not match (loc. 3)"; goto lError; }
            for(ic=1; ic<=whole.mapS(); ++ic)
            {
                if (wholeOrdReversed) { ib=whole.mapS() + 1 - ic; } else { ib = ic; }
                  //Checking map_locate/find, mapi_has, mapi_empty, map_empty and mapi*.
                if (whole.mapi_empty(ib) != ptB.map_empty(ptB.mapi_key(ic))) { my_res = -9; my_errm = "map*_empty error"; goto lError; }
                if (whole.map_empty(ptB.mapi_key(ic)) != ptB.map_empty(ptB.mapi_key(ic))) { my_res = -23; my_errm = "map*_empty error [2]"; goto lError; }
                if (!ptB.map_locate(ptB.mapi_key(ic))) { my_res = -10; my_errm = "map_locate/find error"; goto lError; }
                s_long ic2 = -1;
                if (!ptB.map_locate(ptB.mapi_key(ic), false, ic2)) { my_res = -22; my_errm = "map_locate error [2]"; goto lError; }
                if (ic2 != ic) { my_res = -21; my_errm = "map_locate error [3]"; goto lError; }
                if (!ptB.mapi_has(ic)) { my_res = -24; my_errm = "mapi_has error"; goto lError; }
                if (rnd() < 0.1)
                {
                    v = whole.mapi(ib);
                    if (rnd() < 0.5)
                    {
                      whole.mapi(ib).u_name_set(ib);
                      ptB.mapi(ic) = whole.mapi(ib);
                      ptB.mapi(ic) = unity(whole.mapi(ib));
                    }
                    else
                    {
                      whole.map(whole.mapi_key(ib)).u_name_set(ib);
                      ptB.map(ptB.mapi_key(ic)) = whole.map(whole.mapi_key(ib));
                      ptB.map(ptB.mapi_key(ic)) = unity(whole.map(whole.mapi_key(ib)));
                    }
                }

                if (whole.mapi_key(ib) != ptB.mapi_key(ic)) { my_res = -11; my_errm = "entry keys do not match"; goto lError; }
                if (whole.mapi(ib) != ptB.mapi(ic)) { my_res = -12; my_errm = "entry values do not match"; goto lError; }
            }

            if (true)
            {
              unity m2 = whole;
              if (!m2.isMap()) { my_res = -30; my_errm = "assertion failed"; goto lError; }
              if (m2 != whole) { my_res = -31; my_errm = "assertion failed"; goto lError; }
              if (true)
              {
                unity _v1, _v2;
                _v1 = m2.map_keys(0);
                _v2 = m2.map_values(0);
                for (s_long i = 1; i <= m2.mapS(); ++i)
                {
                  if (m2.mapi_key(i) != whole.mapi_key(i)) { my_res = -32; my_errm = "assertion failed"; goto lError; }
                  if (m2.mapi(i) != whole.mapi(i)) { my_res = -33; my_errm = "assertion failed"; goto lError; }
                  if (_v1.val<utUnity>(i - 1) != m2.mapi_key(i)) { my_res = -34; my_errm = "assertion failed"; goto lError; }
                  if (_v2.val<utUnity>(i - 1) != m2.mapi(i)) { my_res = -35; my_errm = "assertion failed"; goto lError; }
                }
                _v1 = whole.map_keys(0);
                _v2 = whole.map_values(0);
                for (s_long i = 1; i <= whole.mapS(); ++i)
                {
                  if (_v1.val<utUnity>(i - 1) != whole.mapi_key(i)) { my_res = -36; my_errm = "assertion failed"; goto lError; }
                  if (_v2.val<utUnity>(i - 1) != whole.mapi(i)) { my_res = -37; my_errm = "assertion failed"; goto lError; }
                }
              }

              m2.u_name_set("abc");
              m2.mapFlags_set(-1, 1);
              m2.map_clear(true);
                if (m2.u_name() != "abc") { my_res = -40; my_errm = "assertion failed"; goto lError; }
                if (m2.mapFlags() != unity::kf_unity().flags()) { my_res = -41; my_errm = "assertion failed"; goto lError; }
              m2.mapFlags_set(-1, unity::fkcmpRevOrder | unity::fkcmpSNocase);
              m2.map_clear(false);
                if (m2.u_name() != "abc") { my_res = -42; my_errm = "assertion failed"; goto lError; }
                if (m2.mapFlags() != (unity::fkcmpRevOrder | unity::fkcmpSNocase)) { my_res = -43; my_errm = "assertion failed"; goto lError; }
              m2.map(unity()) = 1;
              m2.map(2);
                if (m2.map_empty(unity())) { my_res = -48; my_errm = "assertion failed"; goto lError; }
                if (!m2.map_empty(1)) { my_res = -49; my_errm = "assertion failed"; goto lError; }
                if (!m2.map_empty(2)) { my_res = -50; my_errm = "assertion failed"; goto lError; }
                if (m2.mapi_empty(3)) { my_res = -51; my_errm = "assertion failed"; goto lError; }
                if (m2.mapi_empty(2)) { my_res = -52; my_errm = "assertion failed"; goto lError; }
                if (!m2.mapi_empty(1)) { my_res = -53; my_errm = "assertion failed"; goto lError; }
                if (m2.mapi_has(0) || m2.mapi_has(-1) || m2.mapi_has(3)) { my_res = -54; my_errm = "assertion failed"; goto lError; }
                if (!m2.mapi_has(1) || !m2.mapi_has(2)) { my_res = -55; my_errm = "assertion failed"; goto lError; }
              m2.clear();
                if (m2.u_name() != unity()) { my_res = -44; my_errm = "assertion failed"; goto lError; }
                if (!m2.isEmpty()) { my_res = -45; my_errm = "assertion failed"; goto lError; }
                if (m2.mapFlags() != unity::kf_unity().flags()) { my_res = -46; my_errm = "assertion failed"; goto lError; }
                if (!m2.isMap()) { my_res = -47; my_errm = "assertion failed"; goto lError; }
            }

        }
        my_res=1;
        goto lSuccess;

lError:
        errm=my_errm;
        result=my_res;
        return;

lSuccess:
        result=my_res;
        return;
    }

    int vu_test_perf_map_and_hash_2(unity& h)
    {
      if (true)
      {
        long N1 = 200000;
        long N2 = 100000;
        long kLookup = 100;

        double t1(0), t2(0);

        h.clear();

        t1 = clock_ms();
          for (int i = 0; i < N1; ++i) { h.hash_set(d_nowutc(true).f() / 2e8 + rnd(), 0.); }
        t2 = clock_ms();

        log << "Random double insert: N, h.hashS(), sec, ns/1" << " " << N1 << " " << h.hashS() << " " << (t2-t1) / 1000. << " " << (t2-t1) * 1.e6 / N1  << endl;

        t1 = clock_ms();
          for (int j = 0; j < kLookup / 2; ++j) { for (int i = 1; i <= h.hashS(); ++i) { __consume(h.hash_locate(h.hashi_key(i))); __consume(h.hash_locate(rnd())); } }
        t2 = clock_ms();

        long N3 = h.hashS() * kLookup;
        log << "Keys lookup in hash: N, sec, ns/1" << " " << N3 << " " << (t2-t1) / 1000. << " " << (t2-t1) * 1.e6 / N3 << endl;


        unity& m = h; m.clear();

        t1 = clock_ms();
          for (int i = 0; i < N2; ++i) { m.map(d_nowutc(true).f() / 2e8 + rnd()) = 0.; }
        t2 = clock_ms();

        log << "Random double insert: N, m.mapS(), sec, ns/1" << " " << N2 << " " << m.mapS() << " " << (t2-t1) / 1000. << " " << (t2-t1) * 1.e6 / N2  << endl;

        t1 = clock_ms();
          for (int j = 0; j < kLookup / 2; ++j) { for (int i = 1; i <= m.mapS(); ++i) { __consume(m.map_locate(m.mapi_key(i))); __consume(m.map_locate(rnd())); } }
        t2 = clock_ms();

        long N4 = m.mapS() * kLookup;
        log << "Keys lookup in map: N, sec, ns/1" << " " << N4 << " " << (t2-t1) / 1000. << " " << (t2-t1) * 1.e6 / N4 << endl;
      }

      if (true)
      {
        long N1 = 200000;
        long N2 = 100000;
        long kLookup = 100;

        double t1(0), t2(0);
        vector<unity> vv; for (int i = 0; i < max(N1, N2); ++i) { vv.push_back(random_v(utUnity)); }

        h.clear();

        t1 = clock_ms();
          for (int i = 0; i < N1; ++i) { h.hash(vv[i]) = vv[i]; }
        t2 = clock_ms();

        log << "Random keys/values insert: N, h.hashS(), sec, ns/1" << " " << N1 << " " << h.hashS() << " " << (t2-t1) / 1000. << " " << (t2-t1) * 1.e6 / N1 << endl;

        t1 = clock_ms();
          for (int j = 0; j < kLookup / 2; ++j) { for (int i = 1; i <= h.hashS(); ++i) { __consume(h.hash_locate(h.hashi_key(i))); __consume(h.hash_locate(rnd())); } }
        t2 = clock_ms();

        long N3 = h.hashS() * kLookup;
        log << "Keys lookup in hash: N, sec, ns/1" << " " << N3 << " " << (t2-t1) / 1000. << " " << (t2-t1) * 1.e6 / N3 << endl;



        unity& m = h; m.clear();

        t1 = clock_ms();
          for (int i = 0; i < N2; ++i) { m.map(vv[i]) = vv[i]; }
        t2 = clock_ms();

        log << "Random keys/values insert: N, m.mapS(), sec, ns/1" << " " << N2 << " " << m.mapS() << " " << (t2-t1) / 1000. << " " << (t2-t1) * 1.e6 / N2 << endl;

        t1 = clock_ms();
          for (int j = 0; j < kLookup / 2; ++j) { for (int i = 1; i <= m.mapS(); ++i) { __consume(m.map_locate(m.mapi_key(i))); __consume(m.map_locate(rnd())); } }
        t2 = clock_ms();

        long N4 = m.mapS() * kLookup;
        log << "Keys lookup in map: N, sec, ns/1" << " " << N4 << " " << (t2-t1) / 1000. << " " << (t2-t1) * 1.e6 / N4 << endl;
      }

      return 4;
    }

    int vu_pl_test1_2(unity& set1)
    {
        paramline pl;

        double t1=clock_ms();

        const wstring syms = L" _`_`bs_`ba_`v_`eq_`cl_\r_\n_\r\n_~_~_`sc_`sp_a_b_c_z_0;_1;_e;_|_|_\\_\\_\\_\\_\\_\\_\\_\\_;_;_;_=_=_=_=_2_3_true_2006-06-25";
        unity syms2=split(syms,L"_");

//log<<syms2<<endl;

        s_long n_test=8000;

        set1.clear();
        unity set2; wstring s, s2, s3; double r(0);
        s_long ia(0), ib(0), size1(0);
//wstring sxbuf;

        for(ia=1; ia<=n_test; ++ia)
        {
//log.d,            ia, n_test;
            size1=s_long(rnd()*20);
            s=L"";
            for(ib=1; ib<=size1; ++ib)
            {
                r=rnd();
//log.d,      "      ",      ib, size1, r;
                if (r<0.5)
                {
                    s+=syms2.rx<utString>(s_long(floor(1 + rnd() * (syms2.arrub()))));
                }
                else if (r<1.)
                {
                    s+=
                        L";;pname"
                        + wstr(s_long(floor(rnd() * 10000.)))
                        + replicate_string(L" ", s_long(floor(rnd() * 3)))
                        + L"="
                        + replicate_string(L" ", s_long(floor(rnd() * 3)))
                        + choose(
                            1+s_long(floor(rnd() * 9)),
                            L"\\e",
                            L"\\f" + unity(rnd() * 5.E+99 - 1.E+100).val<utString>(),
                            L"\\d" + unity(_unitydate(rnd() * 4e11)).val<utString>(),
                            L"\\i" + unity(meta::s_ll(floor(rnd() * 1.e13 - 0.5e13))).val<utString>(),
                            L"\\s"
                                + syms2.rx<utString>(s_long(floor(1 + rnd() * (syms2.arrub()))))
                                + syms2.rx<utString>(s_long(floor(1 + rnd() * (syms2.arrub()))))
                                + syms2.rx<utString>(s_long(floor(1 + rnd() * (syms2.arrub()))))
                                + syms2.rx<utString>(s_long(floor(1 + rnd() * (syms2.arrub()))))
                                + syms2.rx<utString>(s_long(floor(1 + rnd() * (syms2.arrub()))))
                                + syms2.rx<utString>(s_long(floor(1 + rnd() * (syms2.arrub()))))
                                + syms2.rx<utString>(s_long(floor(1 + rnd() * (syms2.arrub()))))
                                + syms2.rx<utString>(s_long(floor(1 + rnd() * (syms2.arrub()))))
                                ,
                            L"\\0",
                            L"\\1",
                            L"false",
                            L"\\z"
                        ).rx<utString>()
                        + L";";
                }
            }
            pl.encode(set1, s2);
            pl.decode(s2, set2, true);
            pl.encode(set2, s3);

            if (s3 != s2)
            {
                log<<"ia="<<ia<<":"<<endl<<s2<<endl<<endl<<s3<<endl<<endl;
                log<<s<<endl<<endl;
                log<<set1<<endl<<endl;
                log<<set2<<endl<<endl;
                return -1;
            }

//if (sxbuf.length() < 500000) sxbuf += s2 + wLF;
        }
//log<<sxbuf<<endl;

        double t2=clock_ms();
        log<<"pl_test1:"<<endl;
        log<<" time="<<t2-t1<<" ms (norm 20000)"<<endl;
        return 1;
    }


    struct _gen_rnd_set
    {
      static void gen1(t_test& parent, unity& set0, const unity& syms2)
      {
        wstring sk;
        unity v; double r(0);
        s_long ib(0), ic(0), id(0), size1(0), size2(0);
        set0=unity();
        size2=1+s_long(floor(rnd()*20));
        for(ic=1; ic<=size2; ++ic)
        {
            size1=s_long(floor(rnd()*200));
            sk=L"";
            for(ib=1; ib<=size1; ++ib)
              { sk+=syms2.ref<utString>(s_long(floor(1 + rnd() * (syms2.arrub())))); }

            r=rnd();
            if (r < 0.5)
              { v = parent.random_v(utUnity); }
            else if (r < 0.7)
            {
                v=parent.random_v(utArray);
                for(id=v.arrlb(); id<=v.arrub(); ++id)
                {
                    if (rnd()<0.5 && (v.utype()==(utString|utArray) || v.utype()==(utUnity|utArray)) )
                    {
                        size1=s_long(floor(rnd()*200));
                        v.arr_set(id,unity());
                        for(ib=1; ib<=size1; ++ib)
                        {
                            v.arr_set(id, v.val<utString>(id) + syms2.ref<utString>(s_long(floor(1 + rnd() * (syms2.arrub())))) );
                        }
                    }
                    else
                    {
                        if (v.utype()==(utUnity|utArray) && (v.rx<utUnity>(id).isArray() || v.rx<utUnity>(id).isObject()))
                        {
                            v.rx<utUnity>(id)=parent.random_v(utUnity);
                        }
                    }
                    if (v.utype()==(utUnity|utArray) && v.rx<utUnity>(id).utype()==utString && v.rx<utString>(id).substr(0,2)==L"\\z")
                    {
                        v.rx<utString>(id).insert(0, L"\\");
                    }
                    if (v.utype()==(utString|utArray) && v.rx<utString>(id).substr(0,2)==L"\\z")
                    {
                        v.rx<utString>(id).insert(0, L"\\");
                    }
                }
                if (rnd()<0.5)
                {
                    v.set_obj(*new unity(v), true);
                }
            }
            else if (r<1.)
            {
                size1=s_long(floor(rnd()*200));
                v="";
                for(ib=1; ib<=size1; ++ib)
                {
                    v = v.val<utString>() + syms2.ref<utString>(s_long(floor(1 + rnd() * (syms2.arrub()))));
                }
            }
            set0.map(sk) = v;
        }
      }
    };

    int vu_pl_test2_2(unity& set1)
    {
        const wstring syms = L" _`_`bs_`ba_`v_`eq_`cl_\r_\n_\r\n_~_~_`sc_`sp_a_b_c_z_f_0_1_e_d_i_s_|_|_\\_\\_\\_\\_\\_\\_\\_\\_;_;_;_=_=_=_=_2_3_true_false_2006-06-25";
        unity syms2 = split(syms,L"_");

        paramline pl;

        double t1=clock_ms();

        s_long n_test=83;

        set1.clear();
        unity set2, set0; wstring str0, str1;
        s_long ia(0);

        rnd_0() = 0;
//        int prec_prev = NRndDoublePrec();

        for(ia=1; ia<=n_test; ++ia)
        {
//            __t_uint64 rnd0_prev = rnd_0();

            _gen_rnd_set::gen1(*this, set0, syms2);
            pl.encode(set0, str0);
            pl.decode(str0, set1, true);
            pl.encode(set1, str1);

//            if (str1 != str0)
//            {
//              NRndDoublePrec() = 6;

//              log<< "WARNING For testing paramline enc/dec, setting rnd. float prec. = " << NRndDoublePrec() <<  ". ia, n_test" << ia << " " << n_test << endl;

//              rnd_0() = rnd0_prev;

//              _gen_rnd_set::gen1(*this, set0, syms2);
//              pl.encode(set0, str0);
//              pl.decode(str0, set1, true);
//              pl.encode(set1, str1);
//            }

            if (str1 != str0)
            {
                log<<"ia="<<ia<<":"<<endl;
                log<<"==== ==== str0 (=encode(set0)) ==== ===="<<endl;
                log<<str0<<endl;
                log<<"==== ==== str1 (=encode(decode(str0))) ==== ===="<<endl;
                log<<str1<<endl;
                log<<"==== ==== set0 ==== ===="<<endl;
                  for (s_long j = 1; j <= set0.mapS(); ++j)
                  {
                    log << "set0.mapi_key(" << j << ")=" << set0.mapi_key(j) << endl;
                    log << "set0.mapi(" << j << ")=" << set0.mapi(j) << endl;
                  }
                log<<"==== ==== set1 (=decode(encode(set0))) ==== ===="<<endl;
                  for (s_long j = 1; j <= set1.mapS(); ++j)
                  {
                    log << "set1.mapi_key(" << j << ")=" << set1.mapi_key(j) << endl;
                    log << "set1.mapi(" << j << ")=" << set1.mapi(j) << endl;
                  }
                log<<"==== ==== END ==== ===="<<endl;
                return -1;
            }

            pl.decode(str1, set2, true);
        }
//      NRndDoublePrec() = prec_prev;

        double t2=clock_ms();
        log<<"pl_test2:"<<endl;
        log<<" time="<<t2-t1<<" ms (norm 5000)"<<endl;
        return 1;
    }















    unity random_v(EUnityType utt)
    {
        unity x;
        xrandom_v(x, utt, NDescLinks(), false);
        return x;
    }

    unity& _s_random_kv_tk() { static unity x; return x; }
    unity& _s_random_kv_tv() { static unity x; return x; }

    //p* may be >=0.
    void random_kv(unity& retk, unity& retv,
        double pkArray=0.1, double pkObject=0.1,
        double pvArray=10, double pvObject=4)
    {
        unity& tk = _s_random_kv_tk();
        unity& tv = _s_random_kv_tv();
        static double pka(0), pko(0), pva(0), pvo(0);
        const s_long tsize=4000;

        if (tk.isEmpty() || pkArray!=pka || pkObject!=pko)
        {
            pka = pkArray; pko = pkObject;
            xget_random_typelist(
                unity().array(
                    utEmpty, 0.1,
                    utInt, 1.,
                    utFloat, 1.,
                    utDate, 1.,
                    utString, 1.,
                    utChar, 0.1,
                    utObject, pko,
                    utArray, pka
                ),
                tk, tsize
            );
        }

        if (tv.isEmpty() || pvArray!=pva || pvObject!=pvo)
        {
            pva = pvArray; pvo = pvObject;
            xget_random_typelist(
                unity().array(
                    utEmpty, 1.,
                    utInt, 1.,
                    utFloat, 1.,
                    utDate, 1.,
                    utString, 1.,
                    utChar, 1.,
                    utObject, pvo,
                    utArray, pva
                ),
                tv, tsize
            );
        }

        xrandom_v(retk, s_long(tk.val<utInt>(s_long(floor(rnd() * tsize)))), 0, false);
        xrandom_v(retv, s_long(tv.val<utInt>(s_long(floor(rnd() * tsize)))), NDescLinks(), false);
    }

    void xget_random_typelist(const unity& weights, unity& dest, s_long tsize)
    {
        s_long ia(0), ib(0); double sk(0), ssum(0);
        sk=0;
        for(ia=weights.arrlb() + 1; ia<=weights.arrub(); ia+=2)sk+=weights.val<utFloat>(ia);
        dest.arr_init<utInt>(0);
        dest.arrub_(tsize-1);
        ssum=0;
        for(ia=weights.arrlb() + 1; ia<=weights.arrub(); ia+=2)
        {
            for(ib=s_long(floor(ssum/sk*tsize)); ib<=s_long(floor((ssum+weights.val<utFloat>(ia))/sk*tsize)-1); ++ib)
            {
                dest.rx<utInt>(ib)=weights.val<utInt>(ia-1);
            }
            ssum+=weights.val<utFloat>(ia);
        }
    }

    void xrandom_v(unity& retv, s_long utt, double NDL, bool continue_tree=false)
    {
        static s_long xrandom_v_cnt(0);
        if (continue_tree)++xrandom_v_cnt;
        else xrandom_v_cnt=0;

        double r(0);
        switch(utt)
        {
            case utEmpty:
                retv.clear();
                break;
            case utInt:
            {
                meta::s_ll m1 = meta::s_ll(rnd()*0x10000000);
                meta::s_ll m2 = meta::s_ll(rnd()*0x10000000);
                meta::s_ll m3 = m1 * m2; if (rnd() < 0.5) { m3 = -m3; }
                retv = m3;
                break;
            }
            case utFloat:
            {
                int prec = NRndDoublePrec();
                if (prec <= 0)
                {
                  if (rnd()<0.3) { retv=rnd(); }
                    else { retv=(rnd()*2-1)*pow(10,rnd()*600-300); }
                }
                else
                {
                  double q = pow(10., prec);
                  if (rnd() < 0.3) { double rnd1 = s_ll(rnd() * q) / q; retv = rnd1; }
                    else { double rnd1 = s_ll((rnd() * 2 - 1) * q) / q; double rnd2 = pow(10., int(rnd()*600-300)); retv = rnd1 * rnd2; }
                }
                break;
            }
            case utDate:
                retv = _unitydate(floor(2e11+2e11*rnd())).round_s();
                break;
            case utString:
                r=rnd();
                retv=choose(1+s_long(floor(r*5)),"alpha","beta","gamma","delta","epsilon").val<utString>() + wstr(s_long(floor(rnd()*100)));
                break;
            case utChar:
                retv=rnd()<0.5;
                break;
            case utUnity: //recursive call
                r=rnd();
                xrandom_v(
                    retv,
                    s_long(choose(1+s_long(floor(r*6)),utEmpty,utInt,utFloat,utDate,utString,utChar).val<utInt>()),
                    NDL,false
                );
                break;
            case utArray:
                xrandom_v_array(retv,NDL);
                break;
            case utObject:
                xrandom_v_object(retv,NDL);
                break;
            default:
                throw XUExec("random_v", "invalid utt", utt);
        }
    }

    void xrandom_v_array(unity& x, double NDL)
    {
        s_long elt = s_long(choose(s_long(rnd() * 5 + 1), utUnity, utInt, utFloat, utString, utDate).val<utInt>());
        s_long fullS=s_long(floor(MinArS()+(MaxArS()-MinArS())*rnd()));

        x.clear();
        unity temp;
        switch(elt)
        {
            case utUnity:
                x.arr_init<utUnity>(s_long(floor(rnd()*20))-10); x.arrsz_(fullS);
                for(int ia=0; ia<=fullS-1; ++ia)
                    if (rnd()<PSubObject(NDL, fullS))
                        xrandom_v(x.rx<utUnity>(ia+x.arrlb()), s_long(choose(s_long(rnd() * 2) + 1, utArray, utObject).val<utInt>()), NDL * NDescCoef());
                    else
                        xrandom_v(x.rx<utUnity>(ia+x.arrlb()), elt, 0);
                break;
            case utInt:
                x.arr_init<utInt>(s_long(floor(rnd()*20))-10); x.arrsz_(fullS);
                for(int ia=0; ia<=fullS-1; ++ia) { xrandom_v(temp, elt, 0); x.arr_set(ia+x.arrlb(), temp); }
                break;
            case utFloat:
                x.arr_init<utFloat>(s_long(floor(rnd()*20))-10); x.arrsz_(fullS);
                for(int ia=0; ia<=fullS-1; ++ia) { xrandom_v(temp, elt, 0); x.arr_set(ia+x.arrlb(), temp); }
                break;
            case utString:
                x.arr_init<utString>(s_long(floor(rnd()*20))-10); x.arrsz_(fullS);
                for(int ia=0; ia<=fullS-1; ++ia) { xrandom_v(temp, elt, 0); x.arr_set(ia+x.arrlb(), temp); }
                break;
            case utDate:
                x.arr_init<utDate>(s_long(floor(rnd()*20))-10); x.arrsz_(fullS);
                for(int ia=0; ia<=fullS-1; ++ia) { xrandom_v(temp, elt, 0); x.arr_set(ia+x.arrlb(), temp); }
                break;
        }


    }

    void xrandom_v_object(unity& x, double NDL)
    {
        x.clear();
        unity& u=*new unity();
        x.set_obj(u,true);
        xrandom_v(u, utUnity, 0);
        u.u_name_set(u);
        u.u_clear();
        unity temp, k, v; s_long n(0);
        switch(s_long(rnd()*3))
        {
            case 0:
                u.u_clear(s_long(choose(s_long(rnd() * 5) + 1, utIntArray, utFloatArray, utStringArray, utDateArray, utUnityArray).val<utInt>()));
                u.uaLb_set( s_long(floor(rnd()*20))-10 );
                u.uaS_set( s_long(floor(MaxColS()*rnd())) );
                for(s_long ia=u.uaLb(); ia<=u.uaUb(); ++ia)
                {
                    if (u.utype()==utUnityArray)
                    {
                        if (rnd() < PSubObject(NDL, u.uaS()))
                        {
                            xrandom_v(u.ua(ia), s_long(choose(s_long(rnd() * 2) + 1, utArray, utObject).val<utInt>()), NDL * NDescCoef());
                        }
                        else
                        {
                            xrandom_v(temp, u.utype_noarray(), 0);
                            u.ua_set(ia, temp);
                        }
                    }
                    else
                    {
                        xrandom_v(temp, u.utype_noarray(), 0);
                        u.ua_set(ia, temp);
                    }
                }
                break;
            case 1:
                if (rnd() < 0.5) u.mapFlags_set(unity::fkcmpRevOrder, unity::fkcmpRevOrder); else u.mapFlags_set(unity::fkcmpRevOrder, 0);
                if (rnd() < 0.5) u.mapFlags_set(unity::fkcmpSNocase, unity::fkcmpSNocase); else u.mapFlags_set(unity::fkcmpSNocase, 0);
                if (rnd() < 0.5) u.mapFlags_set(unity::fkcmpScalarize, unity::fkcmpScalarize); else u.mapFlags_set(unity::fkcmpScalarize, 0);
                if (rnd() < 0.5) u.mapFlags_set(unity::fkcmpNFloat, unity::fkcmpNFloat); else u.mapFlags_set(unity::fkcmpNFloat, 0);
                n = s_long(floor(MaxColS()*rnd()));
                for(s_long ia=1; ia<=n; ++ia)
                {
                    xrandom_v(k, utUnity, 0);
                    if (rnd() < PSubObject(NDL, u.uaS()))
                    {
                        xrandom_v(v, s_long(choose(s_long(rnd() * 2) + 1, utArray, utObject).val<utInt>()), NDL * NDescCoef());
                    }
                    else
                    {
                        xrandom_v(v, utUnity, 0);
                    }
                    u.map(k) = v;
                }
                break;
            default:
                if (rnd()<PSubObject(NDL, 1))
                    xrandom_v(u, s_long(choose(s_long(rnd() * 2) + 1, utArray, utObject).val<utInt>()), NDL * NDescCoef());
                else
                    xrandom_v(u, utUnity, 0);
        }
    }

    int test_vflstr()
    {
      const char* pvals[] =
      {
        "Empty: ",
        "Int: 1",
        "Date: 2008-12-30 23:59:00",
        "Char: +38",
        "Float: 1.",
        "String: abcde",
        "Object: obj@",
        "Map: map2:key 1=value 1;key 2=",
        "Hash: hash2:hash k1=hash v1;20=2",
        "UnityArray: arV3 b1: val1,2,3.",
        "IntArray: arI3 b1: 10,20,30",
        "FloatArray: arF3 b1: 1.1,1.2,1.3",
        "CharArray: arC3 b1: +81,+87,+82",
        "DateArray: arD2 b1: 2010-01-02 03:04:05,2010-01-02 03:04:06",
        "StringArray: arS3 b1: abc,def,ghi"
      };

      if ("Empty: " + unity().vflstr() != pvals[0]) { return -40; }
      if ("Int: " + unity(1).vflstr() != pvals[1]) { return -41; }
      if ("Date: " + unity(_unitydate(2008, 12, 30, 23, 59)).vflstr() != pvals[2]) { return -42; }
      if ("Char: " + unity('&').vflstr() != pvals[3]) { return -43; }
      if ("Float: " + unity(1.).vflstr() != pvals[4]) { return -44; }
      if ("String: " + unity("abcde").vflstr() != pvals[5]) { return -45; }
      int n;
      if ("Object: " + unity().set_obj<int>(n, false).vflstr() != pvals[6] + _fls75(((const char*)&n) - ((const char*)0))) { return -46; }
      if ("Map: " + paramline().list_m("key 1", "value 1", "key 2").vflstr() != pvals[7]) { return -47; }
      if ("Hash: " + paramline().list_h("hash k1", "hash v1", 20, 2).vflstr() != pvals[8]) { return -48; }
      if ("UnityArray: " + unity().array("val1", 2, "3.").vflstr() != pvals[9]) { return -49; }
      unity a;
      a.arr_init<utInt>(1); a.arr_append(10); a.arr_append(20); a.arr_append(30);
        if ("IntArray: " + a.vflstr() != pvals[10]) { return -50; }
      a.arr_init<utFloat>(1); a.arr_append(1.1); a.arr_append(1.2); a.arr_append(1.3);
        if ("FloatArray: " + a.vflstr() != pvals[11]) { return -51; }
      a.arr_init<utChar>(1); a.arr_append('Q'); a.arr_append('W'); a.arr_append('R');
        if ("CharArray: " + a.vflstr() != pvals[12]) { return -52; }
      a.arr_init<utDate>(1); a.arr_append(_unitydate(2010, 1, 2, 3, 4, 5)); a.arr_append(_unitydate(2010, 1, 2, 3, 4, 6));
        if ("DateArray: " + a.vflstr() != pvals[13]) { return -53; }
      a.arr_init<utString>(1); a.arr_append("abc"); a.arr_append("def"); a.arr_append("ghi");
        if ("StringArray: " + a.vflstr() != pvals[14]) { return -54; }

      log.d, a.vflstr(); // print to stream test (arg. is automatically converted to unity, then goes to ostream)

      return 1;
    }

    int test_wsbs_perf()
    {
      struct wsbs_test
      {
        struct _tcnvchk { static bool _test_mbrtowc() { char cc[MB_LEN_MAX+1]; int res; wchar_t c; cc[0] = 'A'; res = mbrtowc(&c, cc, 1, 0); if (res != 1 || c != L'A') { return false; } cc[0] = '1'; res = mbrtowc(&c, cc, 1, 0); if (res != 1 || c != L'1') { return false; } cc[0] = ';'; res = mbrtowc(&c, cc, 1, 0); if (res != 1 || c != L';') { return false; } cc[0] = ' '; res = mbrtowc(&c, cc, 1, 0); if (res != 1 || c != L' ') { return false; } return true; } static bool _test_wcrtomb() { char cc[MB_LEN_MAX+1]; int res; res = wcrtomb(cc, L'A', 0); if (res != 1 || cc[0] != 'A') { return false; } res = wcrtomb(cc, L'1', 0); if (res != 1 || cc[0] != '1') { return false; } res = wcrtomb(cc, L';', 0); if (res != 1 || cc[0] != ';') { return false; } res = wcrtomb(cc, L' ', 0); if (res != 1 || cc[0] != ' ') { return false; } return true; } };
        static std::string wsbs1(const wchar_t* ps, meta::s_ll n)
        {
          try {
            static int _mb(0);
            if (_mb == 2) { return wsToBsUtf8(ps, n); }

            if (!ps) { throw XUExec("_wsToBs.3"); }
            if (n < 0) { n = 0; const wchar_t* p = ps; while (*p++) { ++n; } }
            std::string s;
            char cc[MB_LEN_MAX+1];
            const char* prevlocn = std::setlocale(LC_CTYPE, "");
            if (wcrtomb(0, 0, 0)) {}

            if (!_mb) { _mb = 2 + (int)_tcnvchk::_test_wcrtomb(); }
            if (_mb == 2) { std::setlocale(LC_CTYPE, prevlocn); return wsToBsUtf8(ps, n); }

            for(meta::s_ll pos = 0; pos < n; ++pos)
            {
              if (ps[pos] == L'\0') { s += '\0'; }
                else { int cnt = wcrtomb(cc, ps[pos], 0); if (cnt == -1) { s += '?'; } else { cc[cnt] = 0; s += cc; } }
            }
            std::setlocale(LC_CTYPE, prevlocn);
            return s;
          }
          catch (_XUBase&) { throw; }
          catch (const std::exception& e) { throw XUExec("_wsToBs.1", e.what()); }
          catch (...) { throw XUExec("_wsToBs.2"); }
        }
        static std::string wsbs2(const wchar_t* ps, meta::s_ll n)
        {
          static int _mb(0);
          if (_mb == 2) { return wsToBsUtf8(ps, n); }

          critsec_t<std::wstring> __lock(1, -1); if (sizeof(__lock)) {}
          try {
            if (!ps) { throw XUExec("_wsToBs.3"); }
            if (n < 0) { n = 0; const wchar_t* p = ps; while (*p++) { ++n; } }
            std::string s;
            char cc[MB_LEN_MAX+1];
            const char* prevlocn = std::setlocale(LC_CTYPE, "");
            if (wcrtomb(0, 0, 0)) {}

            if (!_mb) { _mb = 2 + (int)_tcnvchk::_test_wcrtomb(); }
            if (_mb == 2) { std::setlocale(LC_CTYPE, prevlocn); return wsToBsUtf8(ps, n); }

            for(meta::s_ll pos = 0; pos < n; ++pos)
            {
              if (ps[pos] == L'\0') { s += '\0'; }
                else { int cnt = wcrtomb(cc, ps[pos], 0); if (cnt == -1) { s += '?'; } else { cc[cnt] = 0; s += cc; } }
            }
            std::setlocale(LC_CTYPE, prevlocn);
            return s;
          }
          catch (_XUBase&) { throw; }
          catch (const std::exception& e) { throw XUExec("_wsToBs.1", e.what()); }
          catch (...) { throw XUExec("_wsToBs.2"); }
        }
        static std::string wsbs3(const wchar_t* ps, meta::s_ll n)
        {
          static int _mb(0);
          if (_mb == 2) { return wsToBsUtf8(ps, n); }

          critsec_t<std::wstring> __lock(1, -1); if (sizeof(__lock)) {}
          try {
            if (!ps) { throw XUExec("_wsToBs.3"); }
            if (n < 0) { n = 0; const wchar_t* p = ps; while (*p++) { ++n; } }
            std::string s;
            char cc[MB_LEN_MAX+1];
            if (wcrtomb(0, 0, 0)) {}

            if (!_mb) { _mb = 2 + (int)_tcnvchk::_test_wcrtomb(); }
            if (_mb == 2) { return wsToBsUtf8(ps, n); }

            for(meta::s_ll pos = 0; pos < n; ++pos)
            {
              if (ps[pos] == L'\0') { s += '\0'; }
                else { int cnt = wcrtomb(cc, ps[pos], 0); if (cnt == -1) { s += '?'; } else { cc[cnt] = 0; s += cc; } }
            }
            return s;
          }
          catch (_XUBase&) { throw; }
          catch (const std::exception& e) { throw XUExec("_wsToBs.1", e.what()); }
          catch (...) { throw XUExec("_wsToBs.2"); }
        }
      };

      double t1, t2;
      std::wstring s; std::string s2;
      const s_long n = 1000;
      const s_long ntests = 40000;
      s_long res(0);
      for(s_long i = 0; i < n; ++i) { s += s_long(rnd() * 0xd800); }

      std::string locname0; if (1) { const char* p = std::setlocale(LC_CTYPE, ""); if (p) { locname0 = p; } }
      std::setlocale(LC_CTYPE, "C"); // intentionally, to determine if the tested functions are using the correct locale

      log.d, "WCS to char string perf. test: n ch., n tests;", n, ntests;

      t1 = clock_ms();
        for(s_long i = 0; i < ntests; ++i) { s2 = wsbs_test::wsbs1(s.c_str(), s.length()); s[0] = s_long(rnd() * 0xd800); res += s_long(s2[0]) & 0xff; }
      t2 = clock_ms();
      log.d, "wsbs1 (setlocale+wcrtomb): t, s; t, ns/char;", (t2 - t1) / 1000., (t2 - t1) * 1e6 / ntests / n;

      t1 = clock_ms();
        for(s_long i = 0; i < ntests; ++i) { s2 = wsbs_test::wsbs2(s.c_str(), s.length()); s[0] = s_long(rnd() * 0xd800); res += s_long(s2[0]) & 0xff; }
      t2 = clock_ms();
      log.d, "wsbs2 (setlocale+wcrtomb+critsec): t, s; t, ns/char;", (t2 - t1) / 1000., (t2 - t1) * 1e6 / ntests / n;

      std::setlocale(LC_CTYPE, "");

      t1 = clock_ms();
        for(s_long i = 0; i < ntests; ++i) { s2 = wsbs_test::wsbs3(s.c_str(), s.length()); s[0] = s_long(rnd() * 0xd800); res += s_long(s2[0]) & 0xff; }
      t2 = clock_ms();
      log.d, "wsbs3 (wcrtomb only): t, s; t, ns/char;", (t2 - t1) / 1000., (t2 - t1) * 1e6 / ntests / n;

      std::setlocale(LC_CTYPE, "C"); // intentionally, to determine if the tested functions are using the correct locale

      t1 = clock_ms();
        for(s_long i = 0; i < ntests; ++i) { s2 = wsToBs(s.c_str(), s.length()); s[0] = s_long(rnd() * 0xd800); res += s_long(s2[0]) & 0xff; }
      t2 = clock_ms();
      log.d, "wsToBs (OS-dependent): t, s; t, ns/char;", (t2 - t1) / 1000., (t2 - t1) * 1e6 / ntests / n;

      log.d, res ? ' ' : '\t';

      std::setlocale(LC_CTYPE, locname0.c_str());

      return 1;
    }

    int test_string_conversion()
    {
      if (true)
      {
        // UTF-8 <--> UTF-16 conv. test.
        std::wstring s; // 0..d7ff, e000..ffff, 10000..10ffff (as pairs)
          for (s_long i = 0; i < 0xd800; ++ i) { s += wchar_t(i); }
          for (s_long i = 0xe000; i < 0x10000; ++ i) { s += wchar_t(i); }
          for (s_long i = 0x10000; i < 0x110000; ++ i) { s_long x = i - 0x10000; s += wchar_t(0xd800 | x >> 10); s += wchar_t(0xdc00 | (x & 0x3ff)); }
        std::string s2 = wsToBsUtf8(s);
//        file_utils().save_string("binary truncate utf16le", s, L"d:\\str_utf16.txt");
//        file_utils().save_string("binary truncate lsb8bit", bsLsbToWs(s2), L"d:\\str_utf8.txt");
        std::wstring s3 = bsUtf8ToWs(s2);
        bool b_eq = s3 == s;
//        log.d, "UTF-8 <--> UTF-16 s3 == s:", b_eq;
        if (!b_eq) { return -100; }
      }
      std::string locname0; if (1) { const char* p = std::setlocale(LC_CTYPE, ""); if (p) { locname0 = p; } }
      std::string locname; if (1) { const char* p = std::setlocale(LC_CTYPE, NULL); if (p) { locname = p; } }
      std::setlocale(LC_CTYPE, "C"); // intentionally, to determine if the tested functions are using the correct locale

      const size_t none = std::string::npos;
      size_t i1 = locname.find("UTF");
      size_t i2 = locname.find("utf");
      size_t i3 = locname.find("8");
      bool is_utf8 =  locname.empty() || (i3 != none && ((i1 != none && i1 < i3) || (i2 != none && i2 < i3)));

      std::string s0(is_utf8 ? "\xD0\x90\xD0\xAF\xD0\xB0\xD1\x8F" : "\xC0\xDF\xE0\xFF"); // cyr. A, Ya, a, ya in UTF-8 / ANSI-1251
      std::wstring s_w = bsToWs(s0);
      std::string s1 = wsToBs(s_w);
      std::string s2 = wsToBsOem(s_w);

      typedef unsigned int uint;
      typedef unsigned char uchar;

      log << endl << "Source string encoding: " << (is_utf8 ? "UTF-8" : "ANSI-1251") << endl;
      log << "Locale used by bsToWs, wsToBs: " << locname << endl;

      if (locname == "C") { log << "This platform uses C locale. Skipping non-ASCII chars. conv. test." << endl; return 0; }

      log << endl << "Source string:" << endl << "  s0 == ";
      for (unsigned int i = 0; i < s0.length(); ++i) { log << hex << "0x" << uint(uchar(s0[i])) << " " << dec; }
      log << endl << "  Literally: " << s0;
      log << endl;

      log << endl << "String conversion 1: bsToWs -- from local 8-bit to UTF-16:" << endl << "  s_w == ";
      for (unsigned int i = 0; i < s_w.length(); ++i) { log << hex << "0x" << (uint(s_w[i]) & 0xFFFF) << " " << dec; }
      log << endl;

      log << endl << "String conversion 2: wsToBs -- UTF-16 back to local 8-bit:" << endl << "  s1 == ";
      for (unsigned int i = 0; i < s1.length(); ++i) { log << hex << "0x" << uint(uchar(s1[i])) << " " << dec; }
      log << endl << "  Literally: " << s1;
      log << endl;

      log << endl << "String conversion 3: wsToBsOem -- UTF-16 to 8-bit console (Windows-specific):" << endl << "  s2 == ";
      for (unsigned int i = 0; i < s2.length(); ++i) { log << hex << "0x" << uint(uchar(s2[i])) << " " << dec; }
      log << endl << "  Literally: " << s2;
      log << endl;

      std::setlocale(LC_CTYPE, locname0.c_str());

#ifdef _bmdxpl_Wnds
      if (!is_utf8 && s_w.length() != s0.length()) { return -1; }
      if (s1.length() != s0.length()) { return -2; }
      if (s2.length() != s0.length()) { return -3; }
      if (s1 != s0) { return -4; }
      if (s2 == s1) { return -5; }
#else
      if (!is_utf8 && s_w.length() != s0.length()) { return -11; }
      if (s1.length() != s0.length()) { return -12; }
      if (s2.length() != s0.length()) { return -13; }
      if (s1 != s0) { return -14; }
      if (s2 != s1) { return -15; }
#endif
      return 1;
    }

    int test_file_utils()
    {
//~!!!
#ifdef _bmdxpl_Psx
      return 0;
#endif

      log << "Several russian \"abv\"-prefixed filenames and directories will be created in the current working directory." << endl;

      file_utils fu;

      int result = 0;
      int good_result = 0;

      wstring rootdir = L"_file_utils_tmp";

      ++good_result;
        result += fu.mk_subdir(rootdir);
        if (result != good_result)
        {
          return -1000*result - good_result;
        }

      ++good_result;
        // !mk_subdir-Windowsabvgd\ezh\ziy in Russian
        // !mk_subdir-Windows2abvgd/ezh/ziy in Russian
        result += fu.mk_subdir(rootdir + L"\\!mk_subdir-Windows\x0430\x0431\x0432\x0433\x0434\\\x0435\x0436\\\x0437\x0438\x0439")
                  // - works only for pure Windows-based build
          || fu.mk_subdir(rootdir + L"/!mk_subdir-Posix\x0430\x0431\x0432\x0433\x0434/\x0435\x0436/\x0437\x0438\x0439");
                  // - works only for Unix systems

      vector<wstring> msg;
      vector<wstring> fn;

#define RU_TEXT1L L"\x0420\x0443\x0441\x0441\x043a\x043e\x044f\x0437\x044b\x0447\x043d\x044b\x0439\x0020\x0442\x0435\x043a\x0441\x0442\x0020\x0432"
  // -- "Russkoyazychny tekst v"
#define RU_TEXT2L L"\x0430\x0431\x0432"
  // -- "abv"

      msg.push_back((L"Binary_ stream mode, truncate_: " RU_TEXT1L L" local Windows encoding\n")); fn.push_back(fu.join_path(rootdir, RU_TEXT2L + bsToWs("_bin.txt")));
      msg.push_back((L"Binary_ stream mode, append___: " RU_TEXT1L L" local Windows encoding 164 bytes\n")); fn.push_back(fu.join_path(rootdir, RU_TEXT2L + bsToWs("_bin.txt")));

      msg.push_back((L"Text___ stream mode, truncate_: " RU_TEXT1L L" local Windows encoding\n")); fn.push_back(fu.join_path(rootdir, RU_TEXT2L + bsToWs("_text.txt")));
      msg.push_back((L"Text___ stream mode, append___: " RU_TEXT1L L" local Windows encoding 166 bytes\n")); fn.push_back(fu.join_path(rootdir, RU_TEXT2L + bsToWs("_text.txt")));

      msg.push_back((L"Binary_ stream mode, truncate_: " RU_TEXT1L L" UTF16LE\n")); fn.push_back(fu.join_path(rootdir, RU_TEXT2L + bsToWs("16le_bin.txt")));
      msg.push_back((L"Binary_ stream mode, append___: " RU_TEXT1L L" UTF16LE 268 bytes\n")); fn.push_back(fu.join_path(rootdir, RU_TEXT2L + bsToWs("16le_bin.txt")));

      msg.push_back((L"Text___ stream mode, truncate_: " RU_TEXT1L L" UTF16LE\n")); fn.push_back(fu.join_path(rootdir, RU_TEXT2L + bsToWs("16le_text.txt")));
      msg.push_back((L"Text___ stream mode, append___: " RU_TEXT1L L" UTF16LE 274 bytes\n")); fn.push_back(fu.join_path(rootdir, RU_TEXT2L + bsToWs("16le_text.txt")));

      msg.push_back((L"Binary_ stream mode, truncate_: " RU_TEXT1L L" UTF16BE\n")); fn.push_back(fu.join_path(rootdir, RU_TEXT2L + bsToWs("16be_bin.txt")));
      msg.push_back((L"Binary_ stream mode, append___: " RU_TEXT1L L" UTF16BE 268 bytes\n")); fn.push_back(fu.join_path(rootdir, RU_TEXT2L + bsToWs("16be_bin.txt")));

      msg.push_back((L"Text___ stream mode, truncate_: " RU_TEXT1L L" UTF16BE\n")); fn.push_back(fu.join_path(rootdir, RU_TEXT2L + bsToWs("16be_text.txt")));
      msg.push_back((L"Text___ stream mode, append___: " RU_TEXT1L L" UTF16BE 274 bytes\n")); fn.push_back(fu.join_path(rootdir, RU_TEXT2L + bsToWs("16be_text.txt")));

      msg.push_back(wstring(L"Binary_ stream mode, truncate_: " RU_TEXT1L L" ANSI-1251\n")); fn.push_back(fu.join_path(rootdir, RU_TEXT2L + bsToWs("8lsb_bin.txt")));
      msg.push_back(wstring(L"Binary_ stream mode, append___: " RU_TEXT1L L" ANSI-1251 138 bytes\n")); fn.push_back(fu.join_path(rootdir, RU_TEXT2L + bsToWs("8lsb_bin.txt")));

      msg.push_back(wstring(L"Text___ stream mode, truncate_: " RU_TEXT1L L" ANSI-1251\n")); fn.push_back(fu.join_path(rootdir, RU_TEXT2L + bsToWs("8lsb_text.txt")));
      msg.push_back(wstring(L"Text___ stream mode, append___: " RU_TEXT1L L" ANSI-1251 140 bytes\n")); fn.push_back(fu.join_path(rootdir, RU_TEXT2L + bsToWs("8lsb_text.txt")));

      ++good_result; result += fu.save_string("binary truncate local8bit", msg[0], fn[0]);
      ++good_result; result += fu.save_string("binary append local8bit", msg[1], fn[1]);

      ++good_result; result += fu.save_string("text truncate local8bit", msg[2], fn[2]);
      ++good_result; result += fu.save_string("text append local8bit", msg[3], fn[3]);

      ++good_result; result += fu.save_string("binary truncate utf16le", msg[4], fn[4]);
      ++good_result; result += fu.save_string("binary append utf16le", msg[5], fn[5]);

      ++good_result; result += fu.save_string("text truncate utf16le", msg[6], fn[6]);
      ++good_result; result += fu.save_string("text append utf16le", msg[7], fn[7]);

      ++good_result; result += fu.save_string("binary truncate utf16be", msg[8], fn[8]);
      ++good_result; result += fu.save_string("binary append utf16be", msg[9], fn[9]);

      ++good_result; result += fu.save_string("text truncate utf16be", msg[10], fn[10]);
      ++good_result; result += fu.save_string("text append utf16be", msg[11], fn[11]);

      ++good_result; result += fu.save_string("binary truncate lsb8bit", msg[12], fn[12]);
      ++good_result; result += fu.save_string("binary append lsb8bit", msg[13], fn[13]);

      ++good_result; result += fu.save_string("text truncate lsb8bit", msg[14], fn[14]);
      ++good_result; result += fu.save_string("text append lsb8bit", msg[15], fn[15]);

      if (result == good_result)
      {
        vector<wstring> fn2;
        vector<wstring> msg2;
        vector<wstring> msg2loaded;

        for (unsigned int i = 0; i < fn.size() - 1; i += 2)
        {
          fn2.push_back(fn[i] + fn[i+1]);
          msg2.push_back(msg[i] + msg[i+1]);
        }

        wstring* ps;
        ps = &msg2[3]; *ps = L"\xff\xfe" + *ps;
        ps = &msg2[5]; *ps = L"\xfe\xff" + *ps;
        //~!!!
      }
      else
      {
        log << "Warning: load_string is not tested because some save_string tests are falied." << endl;
      }

      if (result == good_result)
      {
        return result;
      }
      else
      {
        return -1000*result - good_result;
      }
    }

    int test_utils1()
    {
      wstring scl = cmd_string();
      log.d, "cmd_string()";
      log.d, "  length:", scl.length();
      log.d, "  value:", "'" + wsToBs(scl) + "'";

      wstring sp1 = cmd_myexe();
      wstring sp2 = cmd_myexe();

      log.d, "cmd_myexe()";
      log.d, "  length:", sp1.length();
      log.d, "  value:", "'" + wsToBs(sp1) + "'";

      if
      (
        true !=
        (
          wstring_like(sp1, L"[a-zA-Z]:[\\/]?*")
          || wstring_like(sp1, L"/?*")
        )
      )
      {
        log << "cmd_myexe()  value is not a correct path." << endl;
        return -1;
      }
      else if (sp2 != sp1)
      {
        log << "cmd_myexe() called twice returns diffrent results. |" << sp1 << "|" << sp2 << endl;
        return -2;
      }
      return 1;
    }

    int vu_test_obj() { return ns_vu_test_obj::vu_test_obj(*this); }

    struct ns_vu_test_obj
    {
      struct C1 { static int& sx() { static int _sx(0); return _sx; } ~C1() { x = 0; ++sx(); } double x; C1() : x(17) {} }; struct C2 { long x; C2() : x(27) {} };
      struct C3 : public C1, public C2 {};

      struct V1 { static int& sx() { static int _sx(0); return _sx; } int dyn_sx() { return V1::sx(); } virtual ~V1() { ++sx(); } virtual double x() { return 33; } };
      struct V2 { long x; V2() : x(43) {} virtual ~V2() {} };
      struct V3 : public C1, public V1, public V2, public C2 {};

      static int vu_test_obj(test_bmdx_unity& parent)
      {
        C1::sx() = 0;
        V1::sx() = 0;

        C3 c3;

        //C1& rc1 = c3;
        C2& rc2 = c3;
        C3& rc3 = c3;

        unity c_2, c_3;
        unity temp;

        c_2.set_obj(rc2, false);
        if (c_2.cpvoid() != &rc2) return -1;
        if (c_2.cpvoidkey() != &rc2) return -2;

        c_3.set_obj(rc3, false);
        if (c_3.cpvoid() != &rc3) return -3;
        if (c_3.cpvoidkey() != &rc3) return -4;

        if (c_2.objStatTypeInfo() != typeid(C2)) return -5;
        if (c_2.objDynTypeInfo() != typeid(C2)) return -6;
        if (c_3.objStatTypeInfo() != typeid(C3)) return -7;
        if (c_3.objDynTypeInfo() != typeid(C3)) return -8;

        if ((char*)c_2.cpvoid() - (char*)c_3.cpvoid() != sizeof(C1)) return -9;

        if (c_2.objPtr<C2>(true) != &rc2) return -10;
        if (c_3.objPtr<C3>(true) != &rc3) return -11;
        if (c_2.objPtr<C2>(false) != &rc2) return -12;
        if (c_3.objPtr<C3>(false) != &rc3) return -13;
        if (c_2.objPtr<C3>(true) != 0) return -14;
        if (c_3.objPtr<C2>(true) != 0) return -15;
        if (c_2.objPtr<C3>(false) != (void*)&rc2) return -16;
        if (c_3.objPtr<C2>(false) != (void*)&rc3) return -17;

        if (c_2.objPtr<C2>(true)->x != 27) return -18;
        if (c_3.objPtr<C3>(true)->C1::x != 17) return -19;

        unity dyn_c_3;
        C3* p_dyn_c3;

      //===================================================

        p_dyn_c3 = new C3;
        p_dyn_c3->C1::x = 18;
        p_dyn_c3->C2::x = 28;
        dyn_c_3.set_obj(*p_dyn_c3, true);

        if (dyn_c_3.cpvoid() != p_dyn_c3) return -23;
        if (dyn_c_3.cpvoidkey() != p_dyn_c3) return -24;
        if (dyn_c_3.objStatTypeInfo() != typeid(C3)) return -25;
        if (dyn_c_3.objDynTypeInfo() != typeid(C3)) return -26;
        if (dyn_c_3.objPtr<C3>(false) != p_dyn_c3) return -27;
        if (dyn_c_3.objPtr<C2>(true) != 0) return -28;
        if (dyn_c_3.objPtr<C2>(false) != (void*)p_dyn_c3) return -29;

        if (dyn_c_3.objPtr<C3>(true)->C1::x != 18) return -30;

        temp = dyn_c_3;
        if (temp.cpvoid() != p_dyn_c3) return -31;
        if (temp.cpvoidkey() != p_dyn_c3) return -32;

        temp.clear();
        if (temp.cpvoid() != 0) return -33;
        if (temp.cpvoidkey() != 0) return -34;

        if (p_dyn_c3->C1::x != 18) return -35;

        dyn_c_3 = dyn_c_3;
        if (dyn_c_3.cpvoid() != p_dyn_c3) return -36;
        if (dyn_c_3.cpvoidkey() != p_dyn_c3) return -37;

        dyn_c_3 = temp;

        if (C1::sx() != 1) return -38;
        if (dyn_c_3.cpvoid() != 0) return -39;
        if (dyn_c_3.cpvoidkey() != 0) return -40;

      //===================================================

        p_dyn_c3 = new C3;

        dyn_c_3.set_obj(*p_dyn_c3, true);

        if (dyn_c_3.cpvoid() != p_dyn_c3) return -41;
        if (dyn_c_3.cpvoidkey() != p_dyn_c3) return -42;

        temp = dyn_c_3;

        if (!temp.objIsStrongRef()) return -52;
        if (temp.objIsWeakRef()) return -53;
        if (dyn_c_3.objStrongRefsCount() != 2) return -56;
        if (dyn_c_3.objWeakRefsCount() != 0) return -57;
        if (temp.objStrongRefsCount() != 2) return -58;
        if (temp.objWeakRefsCount() != 0) return -59;

        temp.objSetStrongRef(false);

        if (temp.objIsStrongRef()) return -54;
        if (!temp.objIsWeakRef()) return -55;
        if (dyn_c_3.objStrongRefsCount() != 1) return -75;
        if (dyn_c_3.objWeakRefsCount() != 1) return -76;
        if (temp.objStrongRefsCount() != 1) return -77;
        if (temp.objWeakRefsCount() != 1) return -78;

        if (temp.cpvoid() != p_dyn_c3) return -43;
        if (temp.cpvoidkey() != p_dyn_c3) return -44;

        dyn_c_3.clear();

        if (dyn_c_3.objStrongRefsCount() != -2) return -79;
        if (dyn_c_3.objWeakRefsCount() != -2) return -80;
        if (temp.objStrongRefsCount() != 0) return -81;
        if (temp.objWeakRefsCount() != 1) return -82;

        if (C1::sx() != 2) return -45;

        if (dyn_c_3.cpvoid() != 0) return -46;
        if (dyn_c_3.cpvoidkey() != 0) return -47;

        if (temp.cpvoid() != 0) return -48;
        if (temp.cpvoidkey() != p_dyn_c3) return -49;

        dyn_c_3 = temp;

        if (dyn_c_3.cpvoid() != 0) return -50;
        if (dyn_c_3.cpvoidkey() != p_dyn_c3) return -51;

      //===================================================

        p_dyn_c3 = new C3;

        dyn_c_3.set_obj(*p_dyn_c3, true);

        if (dyn_c_3.cpvoid() != p_dyn_c3) return -61;
        if (dyn_c_3.cpvoidkey() != p_dyn_c3) return -62;

        temp = dyn_c_3;

        dyn_c_3.objSetStrongRef(false);

        if (temp.cpvoid() != p_dyn_c3) return -63;
        if (temp.cpvoidkey() != p_dyn_c3) return -64;

        temp.objSetStrongRef(false);

        if (C1::sx() != 3) return -65;

        if (temp.cpvoid() != 0) return -68;
        if (temp.cpvoidkey() != p_dyn_c3) return -69;
        if (dyn_c_3.cpvoid() != 0) return -70;
        if (dyn_c_3.cpvoidkey() != p_dyn_c3) return -71;

      //===================================================

        V3 v3;

        C1& rc1v3 = v3;
        C2& rc2v3 = v3;

        unity v_3;

        v_3.set_obj(rc1v3, false);

        if (v_3.objStatTypeInfo() != typeid(C1)) return -100;
        if (v_3.objDynTypeInfo() != typeid(C1)) return -101;

        if (v_3.objPtr<C1>() != &v3) return -102;

        v_3.set_obj(rc2v3, false);

        if (v_3.objStatTypeInfo() != typeid(C2)) return -103;
        if (v_3.objDynTypeInfo() != typeid(C2)) return -104;

        if (v_3.objPtr<C2>() != &rc2v3) return -105;

        if (v_3.objStrongRefsCount() != -1) return -106;
        if (v_3.objWeakRefsCount() != 1) return -107;

        v_3.clear();

        if (v3.V1::sx() != 0) return -108;

        v3 = V3();

        if (v3.V1::sx() != 1) return -109;

        if (v_3.cpvoid() != 0) return -110;
        if (v_3.cpvoidkey() != 0) return -111;


      //===================================================

        V3* p_dyn_v3;
        V1* pv1;
        V2* pv2;

        p_dyn_v3 = new V3;
        pv1 = p_dyn_v3;
        pv2 = p_dyn_v3;

        v_3.set_obj(*pv1, true);

        if (v_3.objStatTypeInfo() != typeid(V1)) return -112;
        if (v_3.objDynTypeInfo() != typeid(V3)) return -113;

        if (v_3.objSetStrongRef(false) != true) return -114;

        if (v_3.cpvoid() != 0) return -115;
        if (v_3.cpvoidkey() != p_dyn_v3) return -116;

        if (v_3.objPtr<V1>()->dyn_sx() != 2) return -117;

        if (v_3.objSetStrongRef(true) != false) return -118;

      //===================================================

        p_dyn_v3 = new V3;
        pv1 = p_dyn_v3;
        pv2 = p_dyn_v3;

        v_3.set_obj(*pv2, true);

        if (v_3.objSetStrongRef(true) != true) return -119;

        if (v_3.objStatTypeInfo() != typeid(V2)) return -120;
        if (v_3.objDynTypeInfo() != typeid(V3)) return -121;

        C1* pc1_v3 = static_cast<C1*>(dynamic_cast<V3*>(v_3.objPtr<V2>()));
        if (pc1_v3 != ((C1*)p_dyn_v3)) return -122;
        if (pc1_v3->C1::sx() != C1::sx()) return -123;

        temp = v_3;
        if (v_3.objStrongRefsCount() != 2) return -139;
        if (temp.objStrongRefsCount() != 2) return -140;
        if (v_3.objSetStrongRef(false) != true) return -124;

        if (v_3.objStrongRefsCount() != 1) return -125;
        if (v_3.objWeakRefsCount() != 1) return -126;
        if (temp.objStrongRefsCount() != 1) return -127;
        if (temp.objWeakRefsCount() != 1) return -128;

        temp.clear();
        if (temp.objStrongRefsCount() != -2) return -129;
        if (temp.objWeakRefsCount() != -2) return -130;
        if (v_3.objStrongRefsCount() != 0) return -131;
        if (v_3.objWeakRefsCount() != 1) return -132;

        if (v_3.cpvoid() != 0) return -133;
        if (v_3.cpvoidkey() != pv2) return -134;
        if (v_3.objStatTypeInfo() != typeid(V2)) return -135;
        if (v_3.objDynTypeInfo() != typeid(void*)) return -136;
        if (temp.objStatTypeInfo() != typeid(void*)) return -137;
        if (temp.objDynTypeInfo() != typeid(void*)) return -140;

      //===================================================

        unity a1, a2;
        CLSA* pclsa(0);

        a1.set_obj(*new CLSA, true);

        pclsa = a1.objPtr<CLSA>();
        if (!pclsa) return -219;

        if (a1.pinterface<INTA>() == 0) return -200;
        if (a1.pinterface<INTA>()->msg1() != "CLSA INTA msg1") return -201;
        if (a1.pinterface<INTB>() == 0) return -202;
        if (a1.pinterface<INTB>()->msg2() != "CLSA INTB msg2") return -203;
        if (a1.pinterface<INTC>() != 0) return -204;

        if (a1.objItfsAttach<o_interfaces_top_CLSA_alt1>() != 2) return -205;

        if (a1.pinterface<INTA>() == 0) return -220;
        if (a1.pinterface<INTA>()->msg1() != "CLSA INTA msg1") return -221;
        if (a1.pinterface<INTB>() == 0) return -222;
        if (a1.pinterface<INTB>()->msg2() != "o_iimpl<CLSA, INTB_alt> msg2") return -223;
        if (a1.pinterface<INTC>() == 0) return -224;
        if (a1.pinterface<INTC>()->msg3b_prev() != "o_iimpl<T, INTC> msg3 + CLSA INTB msg2") return -225;
        if (a1.pinterface<INTC>()->msg3a_this() != "o_iimpl<T, INTC> msg3 + CLSA INTA msg1") return -226;

        a1.pinterface<INTC>()->ref_sx() = 10;
        a1.pinterface<INTA>()->ref_n() = 20;
        a1.pinterface<INTA>()->ref_s() = "30";
        a1.pinterface<INTB>()->set_data(0.1);

        if (o_iimpl<CLSA, INTC>::sx() != 10) return -240;
        if (pclsa->n != 20) return -241;
        if (pclsa->s != "30") return -242;

        // a2 vs a1 test begin

        a2 = a1;

        if (a2.pinterface<INTA>() == 0) return -260;
        if (a2.pinterface<INTA>()->msg1() != "CLSA INTA msg1") return -261;
        if (a2.pinterface<INTB>() == 0) return -262;
        if (a2.pinterface<INTB>()->msg2() != "o_iimpl<CLSA, INTB_alt> msg2") return -263;
        if (a2.pinterface<INTC>() == 0) return -264;

        if (a2.pinterface<INTC>()->msg3b_prev() != "o_iimpl<T, INTC> msg3 + CLSA INTB msg2") return -265;
        if (a2.pinterface<INTC>()->msg3a_this() != "o_iimpl<T, INTC> msg3 + CLSA INTA msg1") return -275;

        if (a2.pinterface<INTB>()->get_data() != 0.1) return -266;

        if (a1.objItfRemoveLast<INTB>() != 1) return -267;
        if (a1.objItfRemoveLast<INTB>() != -1) return -268;
        if (a1.objItfRemoveLast<INTA>() != -1) return -269;
        if (a1.objItfRemoveLast<INTC>() != 0) return -270;
        if (a1.pinterface<INTC>()) return -273;
        if (o_iimpl<CLSA, INTC>::sx() != 11) return -274;

        if (a1.objItfRemoveLast<INTA>() != -1) return -271;
        if (!a1.pinterface<INTB>()) return -275;

        if (!a2.pinterface<INTB>()) return -276;
        if (a2.pinterface<INTB>()->get_data() != 0.1) return -277;

        if (a2.objItfsAttach<o_interfaces_top_CLSA_alt1>() != 0) return -300;

        if (a1.objItfsAttach<o_interfaces_top_CLSA_alt1>() != 2) return -301;

        if (a1.pinterface<INTA>() == 0) return -310;
        if (a1.pinterface<INTA>()->msg1() != "CLSA INTA msg1") return -311;
        if (a1.pinterface<INTB>() == 0) return -312;
        if (a1.pinterface<INTB>()->msg2() != "o_iimpl<CLSA, INTB_alt> msg2") return -313;
        if (a1.pinterface<INTC>() == 0) return -314;
        if (a1.pinterface<INTC>()->msg3b_prev() != "o_iimpl<T, INTC> msg3 + CLSA INTB msg2") return -315;

        if (a2.pinterface<INTA>() == 0) return -320;
        if (a2.pinterface<INTA>()->msg1() != "CLSA INTA msg1") return -321;
        if (a2.pinterface<INTB>() == 0) return -322;
        if (a2.pinterface<INTB>()->msg2() != "o_iimpl<CLSA, INTB_alt> msg2") return -323;
        if (a2.pinterface<INTC>() == 0) return -324;
        if (a2.pinterface<INTC>()->msg3b_prev() != "o_iimpl<T, INTC> msg3 + CLSA INTB msg2") return -325;

        a1.pinterface<INTB>()->set_data(0.2);

        if (a1.pinterface<INTB>()->get_data() != 0.2) return -330;
        if (a2.pinterface<INTB>()->get_data() != 0.1) return -331;
        if (pclsa->get_data() != -3) return -332;

        if (a2.objItfsRemove<o_interfaces_top_CLSA_alt1>() != 1) return -340;

        if (o_iimpl<CLSA, INTC>::sx() != 12) return -341;

        if (a2.objItfsRemove<o_interfaces_top_CLSA_alt1>() != 0) return -342;
        if (a2.objItfsRemove<o_interfaces_top<CLSA> >() != -1) return -343;

        if (a2.pinterface<INTA>() == 0) return -344;
        if (a2.pinterface<INTA>()->msg1() != "CLSA INTA msg1") return -345;
        if (a2.pinterface<INTB>() == 0) return -346;
        if (a2.pinterface<INTB>()->msg2() != "CLSA INTB msg2") return -347;
        if (a2.pinterface<INTC>() != 0) return -348;

        // a2 vs a1 test end

        if (a1.objItfsFind<o_interfaces_top_CLSA_alt1>() != 0) return -360;
        if (a1.objItfsFind<o_interfaces_top<CLSA> >() != -1) return -361;

        a1.set_obj<CLSA, o_interfaces_top_CLSA_alt1>(*new CLSA, true);

        if (a1.objItfsFind<o_interfaces_top_CLSA_alt1>() != -1) return -362;
        if (a1.objItfsFind<o_interfaces_top<CLSA> >() != -2) return -363;

        if (a1.pinterface<INTA>() == 0) return -440;
        if (a1.pinterface<INTA>()->msg1() != "CLSA INTA msg1") return -441;
        if (a1.pinterface<INTB>() == 0) return -442;
        if (a1.pinterface<INTB>()->msg2() != "o_iimpl<CLSA, INTB_alt> msg2") return -443;
        if (a1.pinterface<INTC>() == 0) return -444;
        if (a1.pinterface<INTC>()->msg3b_prev() != "o_iimpl<T, INTC> msg3") return -445;
        if (a1.pinterface<INTC>()->msg3a_this() != "o_iimpl<T, INTC> msg3 + CLSA INTA msg1") return -446;

        a1.pinterface<INTA>()->ref_n() = 27;
        a1.pinterface<INTA>()->ref_s() = "37";

        if (a1.objItfsAttach<o_interfaces_top<CLSA> >() != 2) return -450;

        if (a1.pinterface<INTA>() == 0) return -460;
        if (a1.pinterface<INTA>()->msg1() != "CLSA INTA msg1") return -461;
        if (a1.pinterface<INTB>() == 0) return -462;
        if (a1.pinterface<INTB>()->msg2() != "CLSA INTB msg2") return -463;
        if (a1.pinterface<INTC>() == 0) return -464;
        if (a1.pinterface<INTC>()->msg3b_prev() != "o_iimpl<T, INTC> msg3") return -465;
        if (a1.pinterface<INTC>()->msg3a_this() != "o_iimpl<T, INTC> msg3 + CLSA INTA msg1") return -466;

        if (a1.pinterface<INTB>()->get_data() != -3) return -467;

        if (a1.objItfsAttach<o_interfaces_top_CLSA_alt2>() != 1) return -468;
        if (a1.objItfsAttach<o_interfaces_top_CLSA_alt1>() != 0) return -469;
        if (a1.objItfsAttach<o_interfaces_top_CLSA_alt2>() != 0) return -470;

        vector<int> inds_itf = a1.objItfFindAll<INTA>();

        if (a1.objItfsFind<o_interfaces_top_CLSA_alt1>() != -1) return -471;
        if (a1.objItfsFind<o_interfaces_top<CLSA> >() != 0) return -472;
        if (a1.objItfsFind<o_interfaces_top_CLSA_alt2>() != 1) return -473;

        if (inds_itf.size() != 3) return -481;
        if (inds_itf[0] != -1) return -482;
        if (inds_itf[1] != 0) return -483;
        if (inds_itf[2] != 1) return -484;
        if (a1.objItfsFindPtr(a1.objItfsByInd(-1)) != -1) return -485;
        if (a1.objItfsFindPtr(a1.objItfsByInd(0)) != 0) return -486;
        if (a1.objItfsFindPtr(a1.objItfsByInd(1)) != 1) return -487;

        inds_itf = a1.objItfFindAll<INTC>();

        if (inds_itf.size() != 2) return -488;
        if (inds_itf[0] != -1) return -489;
        if (inds_itf[1] != 1) return -490;

        if (a1.objItfsMaxInd() != 1) return -500;

        if (a1.objItfFindLast<INTA>() != 1) return -510;
        if (a1.objItfFindLast<INTB>() != 1) return -511;
        if (a1.objItfFindLast<INTC>() != 1) return -512;

        if (!a1.pinterface<INTA>()) return -520;
        if (!a1.pinterface<INTB>()) return -521;
        if (!a1.pinterface<INTC>()) return -522;

        if (a1.pinterface<INTA>()->ref_n() != 27) return -530;
        if (a1.pinterface<INTA>()->ref_s() != "37") return -531;
        if (a1.pinterface<INTA>()->msg1() != "o_iimpl<int, INTA_alt> msg1") return -532;
        if (a1.pinterface<INTB>()->msg2() != "o_iimpl<int, INTB_alt> msg2") return -533;
        if (a1.pinterface<INTB>()->get_data() != -4.) return -534;
        if (a1.pinterface<INTC>()->msg3b_prev() != "o_iimpl<T, INTC> msg3 + CLSA INTB msg2") return -535;
        if (a1.pinterface<INTC>()->msg3a_this() != "o_iimpl<T, INTC> msg3 + o_iimpl<int, INTA_alt> msg1") return -536;

        if (a1.objItfsRemoveInd(0) != 1) return -540;

        if (o_iimpl<CLSA, INTC>::sx() != 13) return -550;
        if (a1.objItfsMaxInd() != 0) return -551;

        if (a1.objItfFindLast<INTA>() != 0) return -560;
        if (a1.objItfFindLast<INTB>() != 0) return -560;
        if (a1.objItfFindLast<INTC>() != 0) return -561;

        if (!a1.pinterface<INTA>()) return -570;
        if (!a1.pinterface<INTB>()) return -571;
        if (!a1.pinterface<INTC>()) return -572;

        if (a1.pinterface<INTA>()->ref_n() != 27) return -580;
        if (a1.pinterface<INTA>()->ref_s() != "37") return -581;
        if (a1.pinterface<INTA>()->msg1() != "o_iimpl<int, INTA_alt> msg1") return -582;
        if (a1.pinterface<INTB>()->msg2() != "o_iimpl<int, INTB_alt> msg2") return -583;
        if (a1.pinterface<INTB>()->get_data() != -4.) return -584;
        if (a1.pinterface<INTC>()->msg3b_prev() != "o_iimpl<T, INTC> msg3 + o_iimpl<CLSA, INTB_alt> msg2") return -585;
        if (a1.pinterface<INTC>()->msg3a_this() != "o_iimpl<T, INTC> msg3 + o_iimpl<int, INTA_alt> msg1") return -586;

        if (a1.objItfRemoveLast<INTC>() != 1) return -590;

        if (o_iimpl<CLSA, INTC>::sx() != 13) return -591;
        if (o_iimpl<int, INTC>::sx() != 1) return -592;
        if (a1.objItfsMaxInd() != -1) return -593;

        if (a1.objItfFindLast<INTA>() != -1) return -600;
        if (a1.objItfFindLast<INTB>() != -1) return -601;
        if (a1.objItfFindLast<INTC>() != -1) return -602;
        if (!a1.pinterface<INTA>()) return -603;
        if (!a1.pinterface<INTB>()) return -604;
        if (!a1.pinterface<INTC>()) return -605;

        if (a1.pinterface<INTA>()->ref_n() != 27) return -610;
        if (a1.pinterface<INTA>()->ref_s() != "37") return -611;
        if (a1.pinterface<INTA>()->msg1() != "CLSA INTA msg1") return -612;
        if (a1.pinterface<INTB>()->msg2() != "o_iimpl<CLSA, INTB_alt> msg2") return -613;
        if (a1.pinterface<INTB>()->get_data() != 0.) return -614;
        if (a1.pinterface<INTC>()->msg3b_prev() != "o_iimpl<T, INTC> msg3") return -615;
        if (a1.pinterface<INTC>()->msg3a_this() != "o_iimpl<T, INTC> msg3 + CLSA INTA msg1") return -616;

        a2.clear();

        if (o_iimpl<CLSA, INTC>::sx() != 13) return -620;
        if (o_iimpl<int, INTC>::sx() != 1) return -621;

        if (!a1.pinterface<INTA>()) return -630;
        if (!a1.pinterface<INTB>()) return -631;
        if (!a1.pinterface<INTC>()) return -632;

        a1.clear();

        if (o_iimpl<CLSA, INTC>::sx() != 14) return -640;

        if (a1.pinterface<INTA>()) return -650;
        if (a1.pinterface<INTB>()) return -651;
        if (a1.pinterface<INTC>()) return -652;
        if (a2.pinterface<INTA>()) return -653;
        if (a2.pinterface<INTB>()) return -654;
        if (a2.pinterface<INTC>()) return -655;

        o_iimpl<CLSA, INTC>::sx() = 0;
        o_iimpl<int, INTC>::sx() = 0;

      //===================================================

        a1.set_obj(*new CLSA, true);
          a1.objItfsAttach<o_interfaces_top_CLSA_alt1>();
        a2.set_obj(*new CLSA, true);

        double t1 = clock_ms();
        int N = 100000;
        for (int i = 0; i < N / 6; ++i)
        {
          a1.pinterface<INTA>();
          a1.pinterface<INTB>();
          a1.pinterface<INTC>();
          a2.pinterface<INTA>();
          a2.pinterface<INTB>();
          a2.pinterface<INTC>();
        }
        double t2 = clock_ms();

        parent.log << "unity::pinterface() avg. time, mcs" << endl << (t2-t1) * 1000. / N << endl;

        return 1;
      }
    };

    int vu_ostream_test()
    {
      bool b = true; s_long _n1 = log.nsp; log.nsp = -1000;
      try
      {
        log.d, "=== Serialization test ===";
        log << unity() << endl;
        log << unity(1) << endl;
        log << unity(1.) << endl;
        log << unity(_unitydate(2300000  * 86400ll)) << endl;
        log << unity(char(1)) << ' ' << unity(char(-3)) << endl;
        log << unity("abcd\xF7\xFF\xEC\xF1\xE8>") << endl; // rus. ya ch s m i
        log.d, "------------";
        log << unity(*new unity(12), true) << endl;
        log << unity(*new int(13), true) << endl;
        log.d, "------------";
        log << paramline().list_m("cold", 1, "hot", 2, "warm", 3) << endl;
        log << paramline().list_mx(unity::fkcmpRevOrder, "cold", 1, "hot", 2, "warm", 3) << endl;
        log << paramline().list_mx(unity::fkcmpSNocase, "\xE6", 1, "\xC6", 2, "\xB8", 3) << endl; // rus. zh, Zh, yo
        log.d, "------------";
        log << paramline().list_h("cold", 1, "hot", 2, "warm", 3) << endl;
        log << paramline().list_hx(unity::fkcmpSNocase, "\xE6", 1, "\xC6", 2, "\xB8", 3) << endl;
        log.d, "------------";
        log << unity().array("", 2, 2., _unitydate(2300001  * 86400ll), true);
        log << unity().u_clear(utIntArray).ua_append(12).ua_append(13);
        log << unity().u_clear(utFloatArray).ua_append(14.).ua_append(15.);
        log << unity().u_clear(utDateArray).ua_append(_unitydate().add_datetime(2000, 12, 1, 12, 01, 02));
        log << unity().u_clear(utCharArray).ua_append(16).ua_append(17).ua_set(1, 18);
        log << unity().u_clear(utStringArray).ua_append("alpha").ua_append("beta").ua_append("gamma").uaLb_set(13);
        log.d, "------------";
        log << unity().array
        (
          "Q", 100, 101., _unitydate(2300002  * 86400ll), char(102),
          unity().u_clear(utIntArray).ua_append(67).ua_append(68),
          paramline().list_m("cold", 1, "hot", 2, "warm", 3),
          paramline().list_h("cold", 1, "hot", 2, "warm", 3),
          paramline().list_m
          (
            unity().u_clear(utStringArray).ua_append("k level3-1").ua_append("k level3-2"),
            unity().u_clear(utStringArray).ua_append("v level3-1").ua_append("v level3-2"),
            unity("hot").u_name_set(char(8)), unity(*new unity("v level2-2 obj."), true),
            unity("warm").u_name_set("Sleeps"), 3
          )
        );
        log.d, "------------", endl;
      }
      catch(...) { b = false; }

      log.nsp = _n1;

      return b ? 1 : -1;
    }

    int vu_test_date_validity()
    {
      if (_unitydate(1582, 10, 15).jdn() != 2299161) { return -1; } // 1-st day of Gregorian calendar
      if (_unitydate(1582, 10, 4).jdn() != 2299160) { return -2; } // last day of Julian calendar
      if (_unitydate(-4713, 1, 1).jdn() != 0) { return -3; } // Julian day number 0
        // Arbitrary dates.
      if (_unitydate(1944, 6, 13).jdn() != 2431255) { return -4; }
      if (_unitydate(2000, 1, 1).jdn() != 2451545) { return -5; }
      if (_unitydate(2015, 6, 11).jdn() != 2457185) { return -6; }
      if (_unitydate(1, 1, 1).jdn() != 1721424) { return -7; }
        // Interpretation of incorrect dates.
      for (s_long i = 5; i < 15; ++i) { if (_unitydate(1582, 10, i).jdn() != 2299161 + i - 5) { return -8; } }
      if (_unitydate(0, 0, 0).jdn() != 1721058) { return -9; } // interpreted as -1, 1, 1
      if (_unitydate(2000, 2, 30).jdn() != _unitydate(2000, 3, 1).jdn()) { return -10; }
      if (_unitydate(1900, 2, 30).jdn() != _unitydate(1900, 3, 2).jdn()) { return -11; }

        // Adding / subtracting.
      if (_unitydate(1900, 2, 28).add_datetime(100, 0, 0).jdn() != _unitydate(2000, 2, 28).jdn()) { return -11; }
      if (_unitydate(1900, 3, 2).add_datetime(100, 0, 0).jdn() != _unitydate(2000, 3, 2).jdn()) { return -12; }
      if (_unitydate(1900, 2, 30).add_datetime(100, 0, 0).jdn() != _unitydate(2000, 3, 2).jdn()) { return -13; }
      if (_unitydate(1000, 1, 1).add_datetime(1000, 1, 30).jdn() != _unitydate(2000, 3, 2).jdn()) { return -14; }
      if (_unitydate(2000, 2, 28).add_datetime(-100, 0, 0).jdn() != _unitydate(1900, 2, 28).jdn()) { return -15; }
      if (_unitydate(2000, 2, 28).add_datetime(0, -1, 0).jdn() != _unitydate(2000, 1, 28).jdn()) { return -16; }

        // Parts.
      if (true) { _unitydate u(2000, 9, 1); if (u.d_year() != 2000 || u.d_month() != 9 || u.d_day() != 1) { return -20; } }
      if (true) { _unitydate u(2000, 9, 1, 23, 59, 59); if (u.d_year() != 2000 || u.d_month() != 9 || u.d_day() != 1) { return -21; } }
      if (true) { _unitydate u(2000, 9, 1, 23, 59, 59); u.add_seconds(0.999); if (u.d_year() != 2000 || u.d_month() != 9 || u.d_day() != 1) { return -22; } }
      if (true) { _unitydate u(2000, 9, 1, 23, 59, 57); u.add_seconds(0.999); if (u.d_hour() != 23 || u.d_minute() != 59 || u.d_second() != 57) { return -23; } }
      if (true) { _unitydate u(2000, 9, 1, 23, 59, 57); u.add_seconds(1.001); if (u.d_hour() != 23 || u.d_minute() != 59 || u.d_second() != 58) { return -24; } }

        // Adding / subtracting + comparison.
      if (_unitydate(1900, 2, 28).add_datetime(100, 0, 0) != _unitydate(2000, 2, 28)) { return -41; }
      if (_unitydate(1900, 3, 2).add_datetime(100, 0, 0) != _unitydate(2000, 3, 2)) { return -42; }
      if (_unitydate(1900, 2, 30).add_datetime(100, 0, 0) != _unitydate(2000, 3, 2)) { return -43; }
      if (_unitydate(1000, 1, 1).add_datetime(1000, 1, 30) != _unitydate(2000, 3, 2)) { return -44; }
      if (_unitydate(2000, 2, 28).add_datetime(-100, 0, 0) != _unitydate(1900, 2, 28)) { return -45; }
      if (_unitydate(2000, 2, 28).add_datetime(0, -1, 0) != _unitydate(2000, 1, 28)) { return -46; }

      if (_unitydate(2000, 2, 28).add_datetime(0, -1, 0, -1) != _unitydate(2000, 1, 27, 23, 0, 0)) { return -47; }
      if (_unitydate(2000, 2, 28).add_datetime(0, -1, 0, 0, -1) != _unitydate(2000, 1, 27, 23, 59, 0)) { return -48; }
      if (_unitydate(2000, 2, 28).add_datetime(0, -1, 0, 0, 0, -1) != _unitydate(2000, 1, 27, 23, 59, 59)) { return -49; }
      if (_unitydate(2000, 2, 28).add_datetime(-100, -1, 0, 0, 0, -1) != _unitydate(1900, 1, 27, 23, 59, 59)) { return -50; }
      if (_unitydate(2000, 2, 28).add_datetime(100, -1, 0, 0, 0, -1) != _unitydate(2100, 1, 27, 23, 59, 59)) { return -51; }

      if (_unitydate(2015, 6, 12).d_day_of_week() != 5) { return -52; }

      if (true)
      {
        _unitydate u (2015, 6, 12);
        for (s_long i = 0; i < 86400; ++i) { u.add_time(0, 0, 1); }
        if (u != _unitydate(2015, 6, 13)) { return -53; }
      }
      if (true)
      {
        _unitydate u (2015, 6, 12);
        s_long i = 0; while (i < 86400) { s_long d = s_long(rnd() * 100); if (i + d > 86400) { d = 86400 - i; } u.add_time(0, 0, d); i += d; }
        if (u != _unitydate(2015, 6, 13)) { return -54; }
      }

      return 1;
    }

  //===================================================
  // Entry point for full test sequence.
  //===================================================

    template<class T> struct ptr_deref_t { typedef T t; };
    template<class T> struct ptr_deref_t<T*> { typedef T t; };
    template<class T> struct ptr_deref_t<const T*> { typedef T t; };

  int do_test(bool is_full, const char* prnd0, unity* pu0)
  {
    full_test() = is_full;
    rnd_00() = prnd0 ? atol(prnd0) : s_long(std::time(0)); rnd_0() = rnd_00();
    std::string prevLocaleName; if (1) { const char* p = std::setlocale(LC_CTYPE, ""); if (p) { prevLocaleName = p; } }

#ifdef _WIN32
    log.aux = report_aux::F;
#endif

    unity u0_local; __pu0() = pu0 ? pu0 : &u0_local;



    try
    {

  //===================================================
  // Automatic tests.
  //===================================================
      int res = 0;
      if (full_test())
      {
        log << nowstr() << " -- " << this->fnp_log << endl;
        log << "do_test(). NOTE Testing may take more than one minute." << endl;
        if (pu0) { log << "NOTE Test is run for external object. " << (void*)__pu0() << endl; }
        log << endl; log.flush();

        log.d, "cmd_myexe()", endl, L"  [" + cmd_myexe() + L"]";
        log.d, "cmd_string()", endl, L"  [" + cmd_string() + L"]";
        log.d, "cmd_array()", endl, cmd_array();


//system("pause");
//goto lExit;


        if (!pu0) {
          res = stat.exec_test(&t_test::test_vflstr, "test_flstr2()") < 0; if (res < 0) { goto lExit; }
          res = stat.exec_test(&t_test::test_string_conversion, "test_string_conversion()") < 0; if (res < 0) { goto lExit; }
          res = stat.exec_test(&t_test::test_wsbs_perf, "test_wsbs_perf()") < 0; if (res < 0) { goto lExit; }
          res = stat.exec_test(&t_test::test_utils1, "test_utils1()") < 0; if (res < 0) { goto lExit; }
          res = stat.exec_test(&t_test::test_file_utils, "test_file_utils()");

          res = stat.exec_test(&t_test::vu_test_date_validity, "vu_test_date_validity()");
          res = stat.exec_test(&t_test::vu_test_obj, "vu_test_obj()");
        }

        res = stat.exec_test(&t_test::vu_test_common, "vu_test_common()");
        res = stat.exec_test(&t_test::vu_test_scalar_validity, "vu_test_scalar_validity()");
        res = stat.exec_test(&t_test::vu_test_array_validity, "vu_test_array_validity()");
        res = stat.exec_test(&t_test::vu_test_hash_validity, "vu_test_hash_validity()");
        res = stat.exec_test(&t_test::vu_test_map_validity, "vu_test_map_validity()");
        res = stat.exec_test(&t_test::vu_pl_test1, "vu_pl_test1()");
        res = stat.exec_test(&t_test::vu_pl_test2, "vu_pl_test2()");
        res = stat.exec_test(&t_test::vu_test_perf_map_and_hash, "vu_test_perf_map_and_hash()");

        if (!pu0) {
          res = stat.exec_test(&t_test::vu_ostream_test, "vu_ostream_test()");
        }
      }

  //===================================================
  // Manual tests.
  //===================================================

      if (true)
      {
  // PINTERFACE TEST

  //    if (true)
  //    {
  //      TestClass c0;

  //      unity::interface_ptr<ITest> ptr(&c0, unity_common::ls_modsm);
  //      log.d, ptr->test_sum(1., 2.);
  //      log.d, ptr->test_product(3., 4.);

  //      unity object(c0, false);
  //      log.d, object.pinterface<ITest>()->test_sum(5., 6.);
  //      log.d, object.pinterface<ITest>()->test_product(7., 8.);
  //    }

  //    if (false)
  //    {
  //      unity::interface_ptr<ITest2> ptr2(0);
  //      log.d, ptr2->test_sum(1., 2.);
  //    }

      }

      // Cleanup for leaks detection.
      __pu0()->clear();
      _s_random_kv_tk().clear();
      _s_random_kv_tv().clear();
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

struct test_bmdx_unity_aux1
{
    // argv[1] == "launchN", N - name, selecting the initForCallback-launch test.
    // argv[2] etc. - launched initForCallback arguments, depending on test.
  int test_launch(int argc, char** argv)
  {
    return 0;
  }
};

}

#ifdef _MSC_VER
  #pragma warning(pop)
#endif


#endif
