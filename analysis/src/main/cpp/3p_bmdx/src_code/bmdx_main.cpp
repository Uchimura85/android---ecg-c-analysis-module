// BMDX library 0.7 EXPERIMENTAL for desktop & mobile platforms
//  (binary modules data exchange)
// See bmdx_main.h for description.
#if defined(__clang__)
  #pragma clang diagnostic ignored "-Wunknown-pragmas"
#endif
#if defined(__GNUC__)
  #pragma GCC diagnostic ignored "-Wpragmas"
  #pragma GCC diagnostic ignored "-Wundefined-bool-conversion"
  #pragma GCC diagnostic ignored "-Wtautological-undefined-compare"
#endif

#include "bmdx_main.h"
using namespace bmdx_str::words;
using namespace bmdx_str::conv;

#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <clocale>
#include <ctype.h>
#include <ctime>
#include <cmath>
#include <cwctype>
#include <limits>
#include <limits.h>
#include <sys/stat.h>

namespace
{
  enum { pslen = 1, ps2len = 2 };
  typedef std::wstring::size_type _t_wz;
  typedef std::string::size_type _t_sz;
  const _t_wz nposw = std::wstring::npos;
  const _t_sz nposc = std::string::npos;
}

namespace bmdx
{
  // Internal utilities.

    // storage with lifetime extended into static init. (init. on first access)
    //    and deinit. (return null ptr).
    //  For declaring as module or class static variable,
    //    together with static int state variable, initialized to 0.
    // NOTE multhreaded access must be protected by a lock.
  template<class T>
  struct storage_es_t
  {
    int* const _pst;
    s_ll _stg[1 + sizeof(T) / 8];
    void _autoinit() { if (*_pst == 0) { try { new (&_stg[0]) T(); *_pst = 1; } catch (...) { *_pst = -1; } } }
    void _autodes() { if (*_pst == 1) { try { ((T*)(&_stg[0]))->~T(); *_pst = 2; } catch (...) { *_pst = -2; } } }
    T* operator()() { _autoinit(); return *_pst == 1 ? (T*)&_stg[0] : 0; }
    storage_es_t(int& state_) : _pst(&state_) { _autoinit(); }
    ~storage_es_t() { _autodes(); }
  };

    // converting numbers to strings
  template<class Char>
  std::basic_string<Char> _trim(const std::basic_string<Char>& s, const Char* pwhat, bool b_left = true, bool b_right = true)
  {
    if (!pwhat) { return s; }
    meta::s_ll lw = 0; const Char* p = pwhat; while (*p++) { ++lw; }
    meta::s_ll ls = s.length();
    if (ls == 0 || ls < lw) { return s; }
    meta::s_ll pos1(0); meta::s_ll pos2(ls);
    if (b_left) { while (pos1 <= ls - lw) { if (s.substr(_t_sz(pos1), _t_sz(lw)) == pwhat) { pos1 += lw; } else { break; } } }
    if (b_right) { while (pos2 > pos1 + lw) { if (s.substr(_t_sz(pos2 - lw), _t_sz(lw)) == pwhat) { pos2 -= lw; } else { break; } } }
    if (pos2 - pos1 == meta::s_ll(s.length())) { return s; }
    if (pos2 <= pos1) { return std::basic_string<Char>(); }
    return s.substr(_t_sz(pos1), _t_sz(pos2 - pos1));
  }
  template<class Char>
  std::basic_string<Char> _trim(const std::basic_string<Char>& s, const std::basic_string<Char>& swhat, bool b_left = true, bool b_right = true)
  { return _trim(s, swhat.c_str(), b_left, b_right); }

  std::wstring trim(const std::wstring& s, const std::wstring& swhat, bool b_left, bool b_right) { return _trim(s, swhat, b_left, b_right); }
  std::string trim(const std::string& s, const std::string& swhat, bool b_left, bool b_right) { return _trim(s, swhat, b_left, b_right); }

  // ceil(x1/x2)
  meta::s_ll ceil_div_long(meta::s_ll x1, meta::s_ll x2);
  std::wstring replicate_string(const std::wstring& s, s_long n);
  std::string replicate_string(const std::string& s, s_long n);
  std::string rtrim1(const std::string& s, const std::string& swhat);
  std::string _lcase(const std::string& s);
  std::string _ucase(const std::string& s);
  std::wstring _lcase(const std::wstring& s);
  std::wstring _ucase(const std::wstring& s);

    // Returns <=n chars starting from s[pos], like any of the given chars.
    //    n == -1 means get all matching chars.
  std::string trim_n(const std::string& s, _t_sz pos, const std::string& chars, s_long n)
  {
    struct __local { static int _0() { return 0; } };
    if (meta::s_ll(pos) < __local::_0()) { pos = 0; } if (pos >= s.size() || n < -1) { return std::string(); }
    _t_sz pos2 = s.find_first_not_of(chars, pos); if (pos2 == nposc) { pos2 = s.size(); }
    if (n != -1) { if (pos2 > pos + n) { pos2 = pos + n; } }
    return s.substr(pos, pos2 - pos);
  }

  _fls75 dtofls(const _unitydate& x)
  {
    s_long y, m, d, h, min, s; x.d_get(&y, &m, &d, &h, &min, &s); double fs = x.d_ms(); bool is_fs = fs >= 0.001 && fs <= 999.999;

    _fls75 q;
    if (y >= 0 && y < 1000) { q += '+'; } q += y;
    q += '-'; if (m < 10) { q += '0'; } q += m;
    q += '-'; if (d < 10) { q += '0'; } q += d;
    if (h || min || s || is_fs)
    {
      q += ' '; if (h < 10) { q += '0'; } q += h;
      q += ':'; if (min < 10) { q += '0'; } q += min;
      q += ':'; if (s < 10) { q += '0'; } q += s;
      if (is_fs)
      {
        _fls75 xf(int(fs * 1000) + 1000000);
        xf[0] = '.';
        int n = xf.length();
        while (n > 2 && xf[n - 1] == '0') { --n; }
        q += xf.substr(0, n);
      }
    }
    return q;
  }

namespace
{
  typedef meta::assert<meta::same_t<unity::t_hash::entry, hashx<unity, unity>::entry>::result>::t_true __check1;
  typedef meta::assert<meta::same_t<unity::t_map::entry, hashx<unity, unity>::entry>::result>::t_true __check2;
  bytes::type_index_t<unity> __cmti_inst_unity;
  bytes::type_index_t<unity::t_hash::entry> __cmti_inst_unity_h_entry;
  bytes::type_index_t<unity::t_map::entry> __cmti_inst_unity_m_entry;
  bytes::type_index_t<std::wstring> __cmti_inst_wstring;

  typedef meta::assert<(sizeof(unity) == 4 * (sizeof(void*) >= sizeof(s_long) ? sizeof(void*) : sizeof(s_long)))>::t_true __check4;
  typedef meta::assert<(sizeof(unsigned long long) >= 8)>::t_true __check5;
}
void* __bmdx_use1() { return &__cmti_inst_wstring; }

//enum { _trace_cnv_n = 256 };
//int _trace_cnv_ind(0);
//s_long _trace_cnv_data[4 * _trace_cnv_n] = {};
//extern void _trace_cnv_f(s_long line, bool b_enter)
//{
//  int i = _trace_cnv_ind;
//  int i0 = i % _trace_cnv_n;
//  set_be4(_trace_cnv_data, 16 * i0 + 0, 0x0f0f0f0f);
//  set_be4(_trace_cnv_data, 16 * i0 + 4, i);
//  set_be4(_trace_cnv_data, 16 * i0 + 8, line + (b_enter ? 0 : 0x10000000));
//  set_be4(_trace_cnv_data, 16 * i0 + 12, GetCurrentThreadId());
//  i += 1;
//  int i2 = _trace_cnv_ind; i2 += 1;
//  if (i != i2) { i += 1; }
//  _trace_cnv_ind = i;
//}
//struct _trace_cnv_s
//{
//  s_long line;
//  _trace_cnv_s(s_long line_) { line = line_; _trace_cnv_f(line, true); }
//  ~_trace_cnv_s() { _trace_cnv_f(line, false); }
//};

s_long unity::_def_size(s_long utt) throw()
{
  enum { sp = sizeof(void*) };
  static s_long size_table[2 * _ut_end] =
  {
    0, sizeof(meta::s_ll), sizeof(double), sizeof(_unitydate), 1, sp, sp, -1, sp, sp
    -1, sp, sp, sp, sp, sp, -1, sp, -1, -1,
  };
  return size_table[(utt & utMaskNoArray) + (utt & utArray ? _ut_end : 0)];
}

s_long unity::_compatibility() const throw()
{
  if (this == 0) { return -3; }
  if (pmsm == unity_common::ls_modsm) { return 2; }
  if (isMap()) { return _m()->compatibility(); }
  if (isHash()) { return _h()->compatibility(); }
  if (isScalar())
  {
    if (utype() == utString)
    {
    std::wstring* p = reference_t<std::wstring>::deref(_data, true);
      return p ? (typer<std::wstring>().t_ind < 0 ? 1 : 0) : -1;
    }
    return 1;
  }
  if (isArray()) { return _drf_q()->compatibility(); }
  return -1;
}

struct cv_ff
{
  struct _pdyn { typedef unity T; _pdyn(T* p_) : _p(p_) {} ~_pdyn() { if (_p) { try { delete _p;  } catch (...) {} _p = 0; } } operator bool() const { return bool(_p); } T* ptr() const { return _p; } T* take() { T* p2 = _p; _p = 0; return p2; } private: T* _p; void operator=(const _pdyn&); _pdyn(const _pdyn&); };

    // Returns std::string representation of the dimensions.
    //  "" on error.
  static std::string vecm_strdim(const vecm& x) { try { std::string s; s += "["; s += ntocs(x.nbase()); s += ".."; s += ntocs(x.nbase() + x.n() - 1); s += "]"; return s; } catch (...) {} return std::string(); }

  template<class Td, class Ts, bool b = meta::same_t<Td, Ts>::result, class _ = meta::nothing> struct _kfx_conv
    { static inline Td F(const Ts* x) { Td z[1] = { Td(*x) }; return z[0]; } };
  template<class Td, class _> struct _kfx_conv<Td, Td, true, _> { static inline const Td& F(const Td* x) { return *x; } };
  template<class _> struct _kfx_conv<double, _unitydate, false, _> { static inline double F(const _unitydate* x) { return x->f(); } };

  template<class Td, class Aux = meta::nothing> struct _kfx
  {
    template<class Ts> static inline Td Fconv(const Ts* x) { Td z[1] = { *x }; return z[0]; }
    static inline const Td& Fconv(const Td* x) { return *x; }
    static inline Td Fconv(const _unitydate* x) { Td z[1] = { x->f() }; return z[0]; }

    template<class A, class B> static inline std::string _pmsg(const A* p1, const B* p2) { std::string s = p1 ? "1" : "0"; s += p2 ? "1" : "0"; s += " "; s += typeid(A).name(); s += " "; s += typeid(B).name(); return s; }
    template<class B> static inline std::string _pmsg(const B* p) { std::string s = p ? "1" : "0"; s += " "; s += typeid(B).name(); return s; }



    // In Feq, Fless*, Fhash:
    //  unity::fkcmpSNocase - case insensitive comparison, lowercase chars in hash fn.
    //  unity::fkcmpNFloat - the flag is ignored, A and B (or C1, C2) are always converted to Td, except if any of them is std::wstring (then, must be A=B=Td=std::wstring).



    template<class B> static inline s_long Fhash(const B* p, s_long flags)
    {
      typedef typename meta::assert<meta::same_t<B, std::wstring>::result>::t_false __check2; enum { __z2 = sizeof(__check2) };
      if (!p) { throw XUExec("_kfx::Fhash.1", _pmsg(p)); }
      return hashx_common::kf_basic<Td>().hash(_kfx_conv<Td, B>::F(p));
    }
    static inline s_long Fhash(const std::wstring* p_, s_long flags)
    {
      if (!p_) { throw XUExec("_kfx::Fhash.2", _pmsg(p_)); }
      const std::wstring& key = *p_;
      if (flags & unity::fkcmpSNocase) { _yk_reg s_long h = 0; _yk_reg const wchar_t* p = key.c_str(); _yk_reg _t_wz n = key.size(); while (n > 0) { h *= 31; h += std::towlower(*p); ++p; --n; } return h; }
        else { _yk_reg s_long h = 0; _yk_reg const wchar_t* p = key.c_str(); _yk_reg _t_wz n = key.size(); while (n > 0) { h *= 31; h += *p++; --n; } return h; }
    }
    static inline s_long Fhash(const unity* p_, s_long flags) { return p_->k_hashf(flags); }
    template<class B> static inline s_long Fhash(const vec2_t<B>* p, s_long flags)
    {
      if (!(p && (p->compatibility() > 0))) { throw XUExec("_kfx::Fhash.3"); }
      iterator_t<B, true> l1 = p->begin();
      s_long n = p->n(); s_long h = _kfx<s_long>::Fhash(&n, 0);
      while (!l1.is_aend()) { h ^= Fhash(l1.pval(), flags); l1.incr(); }
      return h;
    }
    template<class M> static inline s_long _Fhash_assoc(const M* p_, s_long flags)
    {
      if (!p_) { throw XUExec("_kfx::_Fhash_assoc.1",  _pmsg(p_)); }
      s_long n = p_->n(); s_long h = _kfx<s_long>::Fhash(&n, 0);
      for (s_long i = 0; i < n; ++i) { const typename M::entry* e = (*p_)(i); h += _kfx<unity>::Fhash(&e->k, flags); h ^= h * 17; h += _kfx<unity>::Fhash(&e->v, flags); h ^= h / 15; }
      return h;
    }
    static inline s_long Fhash(const unity::t_map* p_, s_long flags) { return _Fhash_assoc(p_, flags); }
    static inline s_long Fhash(const unity::t_hash* p_, s_long flags) { return _Fhash_assoc(p_, flags); }


    template<class A, class B> static inline bool Feq(const A* p1, const B* p2, s_long flags)
    {
      typedef typename meta::assert<meta::same_t<A, std::wstring>::result>::t_false __check1; enum { __z1 = sizeof(__check1) };
      typedef typename meta::assert<meta::same_t<B, std::wstring>::result>::t_false __check2; enum { __z2 = sizeof(__check2) };
      typedef typename meta::assert<meta::same_t<A, unity>::result>::t_false __check3; enum { __z3 = sizeof(__check3) };
      typedef typename meta::assert<meta::same_t<B, unity>::result>::t_false __check4; enum { __z4 = sizeof(__check4) };
      if (!(p1 && p2)) { throw XUExec("_kfx::Feq.1", _pmsg(p1, p2)); }
      Td a = _kfx_conv<Td, A>::F(p1);
      Td b = _kfx_conv<Td, B>::F(p2);
      if (meta::same_t<Td, double>::result && *reinterpret_cast<meta::s_ll*>(&a) == *reinterpret_cast<meta::s_ll*>(&b)) { return true; }
      return a == b;
    }
    static inline bool Feq(const std::wstring* p1, const std::wstring* p2, s_long flags)
    {
      typedef typename meta::assert<meta::same_t<Td, std::wstring>::result>::t_true __check1; enum { __z1 = sizeof(__check1) };
      if (!(p1 && p2)) { throw XUExec("_kfx::Feq.2", _pmsg(p1, p2)); }
      if (flags & unity::fkcmpSNocase)
      {
        if (p1->size() != p2->size()) { return false; }
        _t_wz n = p1->size(); for (_t_wz pos = 0; pos < n; ++pos) { if (towlower((*p1)[pos]) != towlower((*p2)[pos])) { return false; } }
        return true;
      }
      else { return *p1 == *p2; }
    }
    static inline bool Feq(const unity* p1, const unity* p2, s_long flags)
    {
      typedef typename meta::assert<meta::same_t<Td, unity>::result>::t_true __check1; enum { __z1 = sizeof(__check1) };
      if (!(p1 && p2)) { throw XUExec("_kfx::Feq.5", _pmsg(p1, p2)); }
      return p1->k_eq(*p2, flags);
    }
    template<class C1, class C2> static inline bool Feq(const vec2_t<C1>* p1, const vec2_t<C2>* p2, s_long flags)
    {
      if (!(p1 && (p1->compatibility() > 0))) { throw XUExec("_kfx::Feq.3"); }
      if (!(p2 && (p2->compatibility() > 0))) { throw XUExec("_kfx::Feq.4"); }
      if (p1->nbase() != p2->nbase()) { return false; }
      if (p1->n() != p2->n()) { return false; }
      iterator_t<C1, true> l1 = p1->begin(); iterator_t<C2, true> l2 = p2->begin();
      while (!l1.is_aend()) { if (!Feq(l1.pval(), l2.pval(), flags)) { return false; } l1.incr(); l2.incr(); }
      return true;
    }
    template<class M> static inline s_long _Feq_assoc(const M* p1, const M* p2, s_long flags)
    {
      if (!(p1 && p2)) { throw XUExec("_kfx::_Feq_assoc.1", _pmsg(p1, p2)); }
      if (p1->n() != p2->n()) { return false; }
      for (s_long i = 0; i < p1->n(); ++i) { const typename M::entry *e1 = (*p1)(i), *e2 = (*p2)(i); if (!_kfx<unity>::Feq(&e1->k, &e2->k, flags)) { return false; } if (!_kfx<unity>::Feq(&e1->v, &e2->v, flags)) { return false; } }
      return true;
    }
    static inline s_long Feq(const unity::t_map* p1, const unity::t_map* p2, s_long flags) { return _Feq_assoc(p1, p2, flags); }
    static inline s_long Feq(const unity::t_hash* p1, const unity::t_hash* p2, s_long flags) { return _Feq_assoc(p1, p2, flags); }



      // NOTE Fless12 ignores fkcmpRevOrder.
    template<class A, class B> static inline bool Fless12(const B* p1, const A* p2, s_long flags)
    {
      return _Fless12(p1, p2, flags);
//      if (flags & unity::fkcmpRevOrder) { return _Fless12(p2, p1, flags); }
//        else { return _Fless12(p1, p2, flags); }
    }

    template<class A, class B> static inline bool _Fless12(const A* p1, const B* p2, s_long flags)
    {
      typedef typename meta::assert<meta::same_t<A, std::wstring>::result>::t_false __check1; enum { __z1 = sizeof(__check1) };
      typedef typename meta::assert<meta::same_t<B, std::wstring>::result>::t_false __check2; enum { __z2 = sizeof(__check2) };
      typedef typename meta::assert<meta::same_t<A, unity>::result>::t_false __check3; enum { __z3 = sizeof(__check3) };
      typedef typename meta::assert<meta::same_t<B, unity>::result>::t_false __check4; enum { __z4 = sizeof(__check4) };
      if (!(p1 && p2)) { throw XUExec("_kfx::Fless.1", _pmsg(p1, p2)); }
      Td a = _kfx_conv<Td, A>::F(p1);
      Td b = _kfx_conv<Td, B>::F(p2);
      if (meta::same_t<Td, double>::result && *reinterpret_cast<meta::s_ll*>(&a) == *reinterpret_cast<meta::s_ll*>(&b)) { return false; }
      return a < b;
    }
    static inline bool _Fless12(const std::wstring* p1, const std::wstring* p2, s_long flags)
    {
      typedef typename meta::assert<meta::same_t<Td, std::wstring>::result>::t_true __check1; enum { __z1 = sizeof(__check1) };
      if (!(p1 && p2)) { throw XUExec("_kfx::Fless.2", _pmsg(p1, p2)); }
      const std::wstring& a = *p1; const std::wstring& b = *p2;
      typedef wchar_t T;
      if (a.size() == 0) { return b.size() > 0; } if (b.size() == 0) { return false; }
      typedef typename std::wstring::const_iterator L;
      L la(a.begin()); L lb(b.begin()); L lae(a.end()); L lbe(b.end());
      do {
        if (la == lae) { return lb != lbe; } if (lb == lbe) { return false; }
        // ~!!! add flag for locale-dependent comparing c1, c2
        if (flags & unity::fkcmpSNocase) { T c1 = towlower(*la); T c2 = towlower(*lb); if (c1 != c2) { return c1 < c2; } }
          else { if (*la < *lb) { return true; } if (*lb < *la) { return false; } }
        ++la; ++lb;
      } while (true);
    }
    static inline bool _Fless12(const unity* p1, const unity* p2, s_long flags)
    {
      typedef typename meta::assert<meta::same_t<Td, unity>::result>::t_true __check1; enum { __z1 = sizeof(__check1) };
      if (!(p1 && p2)) { throw XUExec("_kfx::Fless.3", _pmsg(p1, p2)); }
      return p1->k_less(*p2, flags);
    }
    template<class C1, class C2> static inline bool _Fless12(const vec2_t<C1>* p1, const vec2_t<C2>* p2, s_long flags)
    {
      if (!(p1 && (p1->compatibility() > 0))) { throw XUExec("_kfx::Fless.3"); }
      if (!(p2 && (p2->compatibility() > 0))) { throw XUExec("_kfx::Fless.4"); }
      const vec2_t<C1>& a = *p1; const vec2_t<C2>& b = *p2;
      if (a.n() == 0) { return b.n() > 0; } if (b.n() == 0) { return false; }
      vecm::link1_t<C1, true> la(a.link1_cbegin()); vecm::link1_t<C2, true> lb(b.link1_cbegin());
      do {
        _yk_reg const C1* p1 = la.pval(); _yk_reg const C2* p2 = lb.pval(); if (!p1) { return bool(p2); } if (!p2) { return false; }
        if (_Fless12(p1, p2, flags)) { return true; } if (_Fless12(p2, p1, flags)) { return false; }
        la.incr(); lb.incr();
      } while (true);
    }
    template<class M> static inline s_long _Fless12_assoc(const M* p1, const M* p2, s_long flags)
    {
      if (!(p1 && p2)) { throw XUExec("_kfx::_Fless12_assoc.1", _pmsg(p1, p2)); }
      const M& a = *p1; const M& b = *p2;
      if (a.n() == 0) { return b.n() > 0; } if (b.n() == 0) { return false; }
      s_long m = a.n(); if (m < b.n()) { m = b.n(); }
      for (s_long i = 0; i < m; ++i)
      {
        _yk_reg const typename M::entry *e1 = a(i), *e2 = b(i);
        if (!e1) { return bool(e2); } if (!e2) { return false; }
        if (_kfx<unity>::_Fless12(&e1->k, &e2->k, flags)) { return true; } if (_kfx<unity>::_Fless12(&e2->k, &e1->k, flags)) { return false; }
        if (_kfx<unity>::_Fless12(&e1->v, &e2->v, flags)) { return true; } if (_kfx<unity>::_Fless12(&e2->v, &e1->v, flags)) { return false; }
      }
      return true;
    }
    static inline s_long _Fless12(const unity::t_map* p1, const unity::t_map* p2, s_long flags) { return _Fless12_assoc(p1, p2, flags); }
    static inline s_long _Fless12(const unity::t_hash* p1, const unity::t_hash* p2, s_long flags) { return _Fless12_assoc(p1, p2, flags); }
  };


  struct cv_reg
  {
      // Only for cross-module calls.
      //  flags: 0x1 set == x constructor completed, 0x2 set == x destructor has completed.
    typedef void (*PF)(unity* px, s_long flags);
    static inline void L(unity* px, s_long flags) { ((PF)px->pmsm(unity_common::msm_cv_reg))(px, flags); }
    static inline void _F(unity* px, s_long flags) {}
  };

  struct cv_clear
  {
    typedef void (*PF)(unity* x);
    static inline void L(unity* x) { ((PF)x->pmsm(unity_common::msm_cv_clear))(x); }
    static inline void F(unity* x) { x->clear(); }
  };

  struct cv_array
  {
    typedef s_long (*PFlb_u)(const unity* pv);
    static inline s_long Llb_u(const unity* pv) { if (pv->pmsm == unity_common::ls_modsm) { return Flb_u(pv); } return ((PFlb_u)pv->pmsm(unity_common::msm_arr_lb_u))(pv); }
    static s_long Flb_u(const unity* pv) { return pv->_drf_q()->nbase(); }

    typedef s_long (*PFub_u)(const unity* pv);
    static inline s_long Lub_u(const unity* pv) { if (pv->pmsm == unity_common::ls_modsm) { return Fub_u(pv); } return ((PFub_u)pv->pmsm(unity_common::msm_arr_ub_u))(pv); }
    static s_long Fub_u(const unity* pv) { return pv->_drf_q()->nbase() + pv->_drf_q()->n() - 1; }

    typedef s_long (*PFsz_u)(const unity* pv);
    static inline s_long Lsz_u(const unity* pv) { if (pv->pmsm == unity_common::ls_modsm) { return Fsz_u(pv); } return ((PFsz_u)pv->pmsm(unity_common::msm_arr_sz_u))(pv); }
    static s_long Fsz_u(const unity* pv) { return pv->_drf_q()->n(); }

    typedef s_long (*PFua_ensure_ind)(unity* pv, s_long ind);
    static inline s_long Lua_ensure_ind(unity* pv, s_long ind) { if (pv->pmsm == unity_common::ls_modsm) { return Fua_ensure_ind(pv, ind); } return ((PFua_ensure_ind)pv->pmsm(unity_common::msm_ua_ensure_ind))(pv, ind); }
      // 1 - on_calm_result.
      // 0 - ind already exists.
      // < 0 - error.
    static s_long Fua_ensure_ind(unity* pv, s_long ind)
    {
      s_long n = pv->_drf_q()->n(); s_long nb = pv->_drf_q()->nbase();
      ind -= nb;
      if (ind < 0)
      {
          // Insert diff., nbase -= diff.
        if (cv_ff::cv_array::Fel_insrem(pv, 0, nb, -ind) <= 0) { return -1; }
        cv_ff::cv_array::Fsz_set(pv, nb + ind, 0, 0x1); // succeeds
        return 1;
      }
      else if (ind >= n)
      {
          // Append diff.
        if (cv_ff::cv_array::Lsz_set(pv, 0, ind + 1, 0x2) <= 0) { return -2; }
        return 1;
      }
      return 0;
    }

      // 1 - on_calm_result.
      // -1 - op. failed or ind is out of range.
      // -2 - pv is not an array, or any ptr is 0.
    typedef s_long (*PFel_get)(const unity* pv, unity* pst, s_long ind);
    static inline s_long Lel_get(const unity* pv, unity* pst, s_long ind) { if (pv->pmsm == unity_common::ls_modsm) { return Fel_get(pv, pst, ind); } return ((PFel_get)pv->pmsm(unity_common::msm_arr_el_get))(pv, pst, ind); }
    static s_long Fel_get(const unity* pv, unity* pst, s_long ind)
    {
      if (!(pv && pst)) { return -2; }
      try {
        switch (pv->utype())
        {
          case utIntArray: { enum { utt = utInt }; new (pst) unity(pv->ref<utt>(ind)); break; }
          case utFloatArray: { enum { utt = utFloat }; new (pst) unity(pv->ref<utt>(ind)); break; }
          case utDateArray: { enum { utt = utDate }; new (pst) unity(pv->ref<utt>(ind)); break; }
          case utCharArray: { enum { utt = utChar }; new (pst) unity(pv->ref<utt>(ind)); break; }
          case utStringArray: { enum { utt = utString }; new (pst) unity(pv->ref<utt>(ind)); break; }
          case utUnityArray: { enum { utt = utUnity }; new (pst) unity(pv->ref<utt>(ind)); break; }
          default: return -2;
        }
      } catch (...) { return -1; }
      return 1;
    }

      // 1 - on_calm_result.
      // -1 - op. failed or ind is out of range.
      // -2 - pv is not an array, or any ptr is 0.
    typedef s_long (*PFel_set)(unity* pv, const unity* px, s_long ind);
    static inline s_long Lel_set(unity* pv, const unity* px, s_long ind) { if (pv->pmsm == unity_common::ls_modsm) { return Fel_set(pv, px, ind); } return ((PFel_set)pv->pmsm(unity_common::msm_arr_el_set))(pv, px, ind); }
    static s_long Fel_set(unity* pv, const unity* px, s_long ind)
    {
      if (!(pv && px)) { return -2; }
      try {
        switch (pv->utype())
        {
          case utIntArray: { enum { utt = utInt }; typedef unity::valtype_t<utt>::t T; const T* p = px->pval<utt>(); pv->ref<utt>(ind) = p ? *p : px->val<utt>(); break; }
          case utFloatArray: { enum { utt = utFloat }; typedef unity::valtype_t<utt>::t T; const T* p = px->pval<utt>(); pv->ref<utt>(ind) = p ? *p : px->val<utt>(); break; }
          case utDateArray: { enum { utt = utDate }; typedef unity::valtype_t<utt>::t T; const T* p = px->pval<utt>(); pv->ref<utt>(ind) = p ? *p : px->val<utt>(); break; }
          case utCharArray: { enum { utt = utChar }; typedef unity::valtype_t<utt>::t T; const T* p = px->pval<utt>(); pv->ref<utt>(ind) = p ? *p : px->val<utt>(); break; }
          case utStringArray: { enum { utt = utString }; typedef unity::valtype_t<utt>::t T; const T* p = px->pval<utt>(); pv->ref<utt>(ind) = p ? *p : px->val<utt>(); break; }
          case utUnityArray: { enum { utt = utUnity }; pv->ref<utt>(ind) = *px; break; }
          default: return -2;
        }
      } catch (...) { return -1; }
      return 1;
    }

      // 1 - on_calm_result.
      // -1 - op. failed.
      // -2 - pv is not an array, or any ptr is 0.
    typedef s_long (*PFel_append)(unity* pv, const unity* px);
    static inline s_long Lel_append(unity* pv, const unity* px) { if (pv->pmsm == unity_common::ls_modsm) { return Fel_append(pv, px); } return ((PFel_append)pv->pmsm(unity_common::msm_arr_el_append))(pv, px); }
    static s_long Fel_append(unity* pv, const unity* px)
    {
      if (!(pv && px)) { return -2; }
      try {
        switch (pv->utype())
        {
          case utIntArray: { enum { utt = utInt }; typedef unity::valtype_t<utt>::t T; const T* p = px->pval<utt>(); pv->ref<utt | utArray>().push_back(p ? *p : px->val<utt>()); break; }
          case utFloatArray: { enum { utt = utFloat }; typedef unity::valtype_t<utt>::t T; const T* p = px->pval<utt>(); pv->ref<utt | utArray>().push_back(p ? *p : px->val<utt>()); break; }
          case utDateArray: { enum { utt = utDate }; typedef unity::valtype_t<utt>::t T; const T* p = px->pval<utt>(); pv->ref<utt | utArray>().push_back(p ? *p : px->val<utt>()); break; }
          case utCharArray: { enum { utt = utChar }; typedef unity::valtype_t<utt>::t T; const T* p = px->pval<utt>(); pv->ref<utt | utArray>().push_back(p ? *p : px->val<utt>()); break; }
          case utStringArray: { enum { utt = utString }; typedef unity::valtype_t<utt>::t T; const T* p = px->pval<utt>(); pv->ref<utt | utArray>().push_back(p ? *p : px->val<utt>()); break; }
          case utUnityArray: { enum { utt = utUnity }; pv->ref<utt | utArray>().push_back(*px); break; }
          default: return -2;
        }
      } catch (...) { return -1; }
      return 1;
    }

      // flags:
      //    0x1 - set lbnd (otherwise lbnd arg. is ignored).
      //    0x2 - set size or ubnd (otherwise sx arg. is ignored).
      //    0x4 (not ignored only on 0x2) - sx is ubnd, otherwise sx is size.
      //  Resizing is done at the end of the array.
      // 1 - on_calm_result. If pv is an array and only lbnd is set, the function always succeeds.
      // -1 - op. failed.
      // -2 - pv is not an array.
      // -3 - bad args.
    typedef s_long (*PFsz_set)(unity* pv, s_long lbnd, s_long size, s_long flags);
    static inline s_long Lsz_set(unity* pv, s_long lbnd, s_long size, s_long flags) { if (pv->pmsm == unity_common::ls_modsm) { return Fsz_set(pv, lbnd, size, flags); } return ((PFsz_set)pv->pmsm(unity_common::msm_arr_sz_set))(pv, lbnd, size, flags); }
    static s_long Fsz_set(unity* pv, s_long lbnd, s_long sx, s_long flags)
    {
      if (!pv) { return -2; }
      try {
        switch (pv->utype())
        {
          case utIntArray: { enum { utt = utInt }; unity::valtype_t<utt | utArray>::t& r = pv->ref<utt | utArray>(); if (flags & 0x1) { r.vec2_set_nbase(lbnd); } if (flags & 0x2) { if (flags & 0x4) { sx = sx + 1 - r.nbase(); } if (sx < 0) { return -3; } r.resize(sx); } break; }
          case utFloatArray: { enum { utt = utFloat }; unity::valtype_t<utt | utArray>::t& r = pv->ref<utt | utArray>(); if (flags & 0x1) { r.vec2_set_nbase(lbnd); } if (flags & 0x2) { if (flags & 0x4) { sx = sx + 1 - r.nbase(); } if (sx < 0) { return -3; } r.resize(sx); } break; }
          case utDateArray: { enum { utt = utDate }; unity::valtype_t<utt | utArray>::t& r = pv->ref<utt | utArray>(); if (flags & 0x1) { r.vec2_set_nbase(lbnd); } if (flags & 0x2) { if (flags & 0x4) { sx = sx + 1 - r.nbase(); } if (sx < 0) { return -3; } r.resize(sx); } break; }
          case utCharArray: { enum { utt = utChar }; unity::valtype_t<utt | utArray>::t& r = pv->ref<utt | utArray>(); if (flags & 0x1) { r.vec2_set_nbase(lbnd); } if (flags & 0x2) { if (flags & 0x4) { sx = sx + 1 - r.nbase(); } if (sx < 0) { return -3; } r.resize(sx); } break; }
          case utStringArray: { enum { utt = utString }; unity::valtype_t<utt | utArray>::t& r = pv->ref<utt | utArray>(); if (flags & 0x1) { r.vec2_set_nbase(lbnd); } if (flags & 0x2) { if (flags & 0x4) { sx = sx + 1 - r.nbase(); } if (sx < 0) { return -3; } r.resize(sx); } break; }
          case utUnityArray: { enum { utt = utUnity }; unity::valtype_t<utt | utArray>::t& r = pv->ref<utt | utArray>(); if (flags & 0x1) { r.vec2_set_nbase(lbnd); } if (flags & 0x2) { if (flags & 0x4) { sx = sx + 1 - r.nbase(); } if (sx < 0) { return -3; } r.resize(sx); } break; }
          default: return -2;
        }
      } catch (...) { return -1; }
      return 1;
    }

    static inline s_long Lua_fill(unity* pv, const unity* px, s_long utt) { if (pv->pmsm == unity_common::ls_modsm) { return Fua_fill(pv, px, utt); } return ((PFua_fill)pv->pmsm(unity_common::msm_ua_fill))(pv, px, utt); }
      // Returns:
      //  1 - on_calm_result,
      //  0 - nothing done because the array is empty and its type is not changed,
      //  < 0 - failure (the value indicates the place of error occurence).
    typedef s_long (*PFua_fill)(unity* pv, const unity* px, s_long utt);
    static s_long Fua_fill(unity* pv, const unity* px, s_long utt)
    {
      if (pv->pmsm != unity_common::ls_modsm) { return -1; }
      utt |= utArray;

      if (!pv->isArray()) { try { pv->u_clear(utt); return 1; } catch (...) {} return -2; }

      s_long n = pv->_drf_q()->n(); s_long nb = pv->_drf_q()->nbase();
      if (pv->utype() == utt &&  n == 0) { return 0; }
      unity z0; z0.u_clear(utt);
      if (n && cv_ff::cv_array::Fel_insrem(&z0, px, 1, n) <= 0) { return -3; }
      if (nb != 1) { cv_ff::cv_array::Fsz_set(&z0, nb, 0, 0x1); } // succeeds
        void* _pn = pv->_data.p2; if (_pn) { pv->_data.p2 = 0; }
      cv_ff::cv_create::Fdestroy(&pv->_data, pv->utype(), 0x1 | 0x4);
      pv->_data.p1 = z0._data.p1; pv->ut = z0.ut;
        if (_pn) { pv->_data.p2 = _pn; }
      z0._data.p1 = 0; z0.ut = utEmpty;
      return 1;
    }

      // px may be 0 -- a) on removal, b) T() will be used on insertion.
      //  m > 0 -- insert.
      //  m < 0 -- remove.
      //  m == 0 -- do nothing.
      // 1 - on_calm_result.
      // -1 - op. failed.
      // -2 - pv is not an array, or pv is 0.
    typedef s_long (*PFel_insrem)(unity* pv, const unity* px, s_long ind, s_long m);
    static inline s_long Lel_insrem(unity* pv, const unity* px, s_long ind, s_long m) { if (pv->pmsm == unity_common::ls_modsm) { return Fel_insrem(pv, px, ind, m); } return ((PFel_insrem)pv->pmsm(unity_common::msm_arr_el_insrem))(pv, px, ind, m); }
    static s_long Fel_insrem(unity* pv, const unity* px, s_long ind, s_long m)
    {
      if (!pv) { return -2; }
      if (m == 0) { return 1; }
      s_long res;
      try {
        switch (pv->utype())
        {
          case utIntArray: { enum { utt = utInt }; typedef unity::valtype_t<utt>::t T; const T* p = px ? px->pval<utt>().p : 0; _yk_reg unity::valtype_t<utt | utArray>::t& rv = pv->ref<utt | utArray>(); if (m > 0) { if (m == 1) { res = rv.el_insert_1(ind, p ? *p : (px ? px->val<utt>() : unity().val<utt>())); } else { res = rv.el_insert_ml(ind, m, p ? *p : (px ? px->val<utt>() : unity().val<utt>())); } } else { if (m == -1) { res = rv.el_remove_1(ind, false); } else { res = rv.el_remove_ml(ind, -m); } } break; }
          case utFloatArray: { enum { utt = utFloat }; typedef unity::valtype_t<utt>::t T; const T* p = px ? px->pval<utt>().p : 0; _yk_reg unity::valtype_t<utt | utArray>::t& rv = pv->ref<utt | utArray>(); if (m > 0) { if (m == 1) { res = rv.el_insert_1(ind, p ? *p : (px ? px->val<utt>() : unity().val<utt>())); } else { res = rv.el_insert_ml(ind, m, p ? *p : (px ? px->val<utt>() : unity().val<utt>())); } } else { if (m == -1) { res = rv.el_remove_1(ind, false); } else { res = rv.el_remove_ml(ind, -m); } } break; }
          case utDateArray: { enum { utt = utDate }; typedef unity::valtype_t<utt>::t T; const T* p = px ? px->pval<utt>().p : 0; _yk_reg unity::valtype_t<utt | utArray>::t& rv = pv->ref<utt | utArray>(); if (m > 0) { if (m == 1) { res = rv.el_insert_1(ind, p ? *p : (px ? px->val<utt>() : unity().val<utt>())); } else { res = rv.el_insert_ml(ind, m, p ? *p : (px ? px->val<utt>() : unity().val<utt>())); } } else { if (m == -1) { res = rv.el_remove_1(ind, false); } else { res = rv.el_remove_ml(ind, -m); } } break; }
          case utCharArray: { enum { utt = utChar }; typedef unity::valtype_t<utt>::t T; const T* p = px ? px->pval<utt>().p : 0; _yk_reg unity::valtype_t<utt | utArray>::t& rv = pv->ref<utt | utArray>(); if (m > 0) { if (m == 1) { res = rv.el_insert_1(ind, p ? *p : (px ? px->val<utt>() : unity().val<utt>())); } else { res = rv.el_insert_ml(ind, m, p ? *p : (px ? px->val<utt>() : unity().val<utt>())); } } else { if (m == -1) { res = rv.el_remove_1(ind, false); } else { res = rv.el_remove_ml(ind, -m); } } break; }
          case utStringArray: { enum { utt = utString }; typedef unity::valtype_t<utt>::t T; const T* p = px ? px->pval<utt>().p : 0; _yk_reg unity::valtype_t<utt | utArray>::t& rv = pv->ref<utt | utArray>(); if (m > 0) { if (m == 1) { res = rv.el_insert_1(ind, p ? *p : (px ? px->val<utt>() : unity().val<utt>())); } else { res = rv.el_insert_ml(ind, m, p ? *p : (px ? px->val<utt>() : unity().val<utt>())); } } else { if (m == -1) { res = rv.el_remove_1(ind, false); } else { res = rv.el_remove_ml(ind, -m); } } break; }
          case utUnityArray: { enum { utt = utUnity }; _yk_reg unity::valtype_t<utt | utArray>::t& rv = pv->ref<utt | utArray>(); if (m > 0) { if (m == 1) { res = rv.el_insert_1(ind, px ? *px : unity()); } else { res = rv.el_insert_ml(ind, m, px ? *px : unity()); } } else { if (m == -1) { res = rv.el_remove_1(ind, false); } else { res = rv.el_remove_ml(ind, -m); } } break; }
          default: return -2;
        }
      } catch (...) { return -1; }
      return res >= 0 ? 1 : -1;
    }
  };

  struct cv_wstring
  {
    // L / F accepts 1, 2, 4-byte character sequences. Resulting wchar_t is formed by mere assignment, without any conversion.
    // flags are reserved, must be 0.
    typedef std::wstring* (*PFnew)(meta::s_ll, const void* p, s_long, s_long);
    static inline std::wstring* Lnew(void* (*pmsm)(s_long), meta::s_ll n, const void* p, s_long wcsx, s_long flags) { if (pmsm == unity_common::ls_modsm) { return Fnew(n, p, wcsx, flags); } return ((PFnew)pmsm(unity_common::msm_wstring_new))(n, p, wcsx, flags); }
    static inline std::wstring* Fnew(meta::s_ll n, const void* p, s_long wcsx, s_long flags)
    {
      if (wcsx == Fwcs()) { return new (std::nothrow) std::wstring(reinterpret_cast<const wchar_t*>(p), _t_wz(n)); }
      std::wstring* ps = new (std::nothrow) std::wstring;
      if (Fcopy(ps, n, p, wcsx, flags)) { return ps; }
      try { delete ps; } catch (...) {} return 0;
    }
      // *ps - dest. std::string, must exist.
      //  *p - first source character, n - number of characters, wcsx - sizeof(character type) + 0x100 if memory layout is BE
    static inline bool Fcopy(std::wstring* ps, meta::s_ll n, const void* p, s_long wcsx, s_long flags)
    {
      if (!ps) { return false; }
      bool b = false;
      try
      {
        const char* pc = reinterpret_cast<const char*>(p);
        ps->reserve(_t_wz(n));
        if (wcsx == 1) { for (meta::s_ll i = 0; i < n; ++i) { *ps += wchar_t(*pc); pc += 1; } b = true; }
          else if (wcsx == 2) { for (meta::s_ll i = 0; i < n; ++i) { *ps += wchar_t(s_long(pc[0]) + (s_long(pc[1]) << 8)); pc += 2; } b = true; }
          else if (wcsx == 0x102) { for (meta::s_ll i = 0; i < n; ++i) { *ps += wchar_t(s_long(pc[1]) + (s_long(pc[0]) << 8)); pc += 2; } b = true; }
          else if (wcsx == 4) { for (meta::s_ll i = 0; i < n; ++i) { *ps += wchar_t(s_long(pc[0]) + (s_long(pc[1]) << 8)); pc += 4; } b = true; }
          else if (wcsx == 0x104) { for (meta::s_ll i = 0; i < n; ++i) { *ps += wchar_t(s_long(pc[3]) + (s_long(pc[2]) << 8)); pc += 4; } b = true; }
      } catch (...) {}
      return b;
    }

    typedef meta::s_ll (*PFn)(const void* ps);
    typedef const void* (*PFp0)(const void* ps);
    typedef s_long (*PFwcs)();
    typedef unity::_PFti PFti;

    static inline meta::s_ll Ln(void* (*pmsm)(s_long), const void* ps) { if (pmsm == unity_common::ls_modsm) { return Fn(ps); } return ((PFn)pmsm(unity_common::msm_wstring_n))(ps); }
    static inline const void* Lp0(void* (*pmsm)(s_long), const void* ps) { if (pmsm == unity_common::ls_modsm) { return Fp0(ps); } return ((PFp0)pmsm(unity_common::msm_wstring_p0))(ps); }
    static inline s_long Lwcs(void* (*pmsm)(s_long)) { if (pmsm == unity_common::ls_modsm) { return Fwcs(); } return ((PFwcs)pmsm(unity_common::msm_wstring_wcs))(); }
    static inline s_long Lti(void* (*pmsm)(s_long)) { if (pmsm == unity_common::ls_modsm) { return Fti(); } return ((PFti)pmsm(unity_common::msm_wstring_ti))(); }

    static meta::s_ll Fn(const void* ps) { return reinterpret_cast<const std::wstring*>(ps)->size(); }
    static const void* Fp0(const void* ps) { static const wchar_t c0(L' '); const std::wstring& r = *reinterpret_cast<const std::wstring*>(ps); return r.size() ? &r[0] : &c0; }
    static s_long Fwcs() { s_long x1 = 0x201; s_long x2 = s_long(((char*)&x1)[0]) + (s_long(((char*)&x1)[1]) << 8); return sizeof(wchar_t) | (x1 == x2 || sizeof(wchar_t) == 1 ? 0 : 1) << 8; }
    static s_long Fti() { return typer<std::wstring>().t_ind; }
  };

  struct cv_delete
  {
    typedef s_long (*PF)(void* p, s_long utt);
    typedef s_long (*PFdel)(void* p);
    template<int utt_, class _ = meta::nothing> struct _del_t { static s_long F(void * p) { if (!p) { return 1; } try { delete reinterpret_cast<typename unity::valtype_t<utt_>::t*>(p); return 2; } catch (...) { return 0; } } };
    template<class _> struct _del_t<utEmpty, _> { static s_long F(void * p) { return 1; } };
    template<class _> struct _del_t<utObject, _> { static s_long F(void * p) { if (!p) { return 1; } try { delete reinterpret_cast<o_handler_base*>(p); return 2; } catch (...) { return 0; } } };
    template<class _> struct _del_t<utMap, _> { static s_long F(void * p) { if (!p) { return 1; } try { delete reinterpret_cast<unity::t_map*>(p); return 2; } catch (...) { return 0; } } };
    template<class _> struct _del_t<utHash, _> { static s_long F(void * p) { if (!p) { return 1; } try { delete reinterpret_cast<unity::t_hash*>(p); return 2; } catch (...) { return 0; } } };

      // 2 - deleted.
      // 1 - p == 0.
      // 0 - delete generated an exception.
      // -1 - wrong utt.
    static inline s_long L(void* (*pmsm)(s_long), void* p, s_long utt) { if (pmsm == unity_common::ls_modsm) { return F(p, utt); } return ((PF)pmsm(unity_common::msm_cv_delete))(p, utt); }
    static s_long F(void* p, s_long utt)
    {
      s_long i1 = (utt & utMaskNoArray) + (utt & utArray ? _ut_end : 0) ;
      static PFdel del_table[2 * _ut_end] = {
        _del_t<utEmpty>::F, _del_t<utInt>::F, _del_t<utFloat>::F, _del_t<utDate>::F, _del_t<utChar>::F, _del_t<utString>::F, _del_t<utObject>::F, _del_t<utUnity>::F, _del_t<utMap>::F, _del_t<utHash>::F,
        0, _del_t<utIntArray>::F, _del_t<utFloatArray>::F, _del_t<utDateArray>::F, _del_t<utCharArray>::F, _del_t<utStringArray>::F, 0, _del_t<utUnityArray>::F, 0, 0,
      };
      if (i1 >= 2 * _ut_end) { return -1; }
      PFdel pf = reinterpret_cast<PFdel>(del_table[i1]); if (!pf) { return -1; }
      return pf(p);
    }
  };

  struct cv_create
  {
    typedef s_long (*PFcreate)(unity::_storage* pdest, s_long utt, s_long flags);

      // pdest must be non-0.
      // flags:
      //  0x1: creates dynamic object, and writes it to pdest->p1.  pdest->p2 is set to 0.
      //      Otherwise, the object is initialized directly in the storage. Unused bytes are set to 0.
      // Returns:
      //  1 - on_calm_result.
      //  0 - dynamic allocation failed. pdest p1 and p2 are set to 0.
      //  -1 - invalid utt/flags. pdest p1 and p2 are set to 0.
    static inline s_long Lcreate(void* (*pmsm)(s_long), unity::_storage* pdest, s_long utt, s_long flags)
      { if (pmsm == unity_common::ls_modsm) { return Fcreate(pdest, utt, flags); } return ((PFcreate)pmsm(unity_common::msm_cv_create))(pdest, utt, flags); }
    static s_long Fcreate(unity::_storage* pdest, s_long utt, s_long flags)
    {
      if (utt & utArray)
      {
        pdest->p2 = 0;
        if ((flags & 0x1) == 0) { pdest->p1 = 0; return -1; }
        switch (utt)
        {
          case utIntArray: pdest->p1 = new (std::nothrow) vec2_t<meta::s_ll>(1, 0, iterator_t<meta::s_ll, true>()); return pdest->p1 != 0;
          case utFloatArray: pdest->p1 = new (std::nothrow) vec2_t<double>(1, 0, iterator_t<double, true>()); return pdest->p1 != 0;
          case utDateArray: pdest->p1 = new (std::nothrow) vec2_t<_unitydate>(1, 0, iterator_t<_unitydate, true>()); return pdest->p1 != 0;
          case utCharArray: pdest->p1 = new (std::nothrow) vec2_t<_unitychar>(1, 0, iterator_t<_unitychar, true>()); return pdest->p1 != 0;
          case utStringArray: pdest->p1 = new (std::nothrow) vec2_t<std::wstring>(1, 0, iterator_t<std::wstring, true>()); return pdest->p1 != 0;
          case utUnityArray: pdest->p1 = new (std::nothrow) vec2_t<unity>(1, 0, iterator_t<unity, true>()); return pdest->p1 != 0;
          default: pdest->p1 = 0; return -1;
        }
      }
      else
      {
        if (flags & 0x1)
        {
          pdest->p2 = 0;
          switch (utt)
          {
            case utEmpty: pdest->p1 = 0; pdest->p2 = 0; return 1;
            case utInt: pdest->p1 = new (std::nothrow) meta::s_ll(); return pdest->p1 != 0;
            case utChar: pdest->p1 = new (std::nothrow) _unitychar(); return pdest->p1 != 0;
            case utFloat: pdest->p1 = new (std::nothrow) double(); return pdest->p1 != 0;
            case utDate: pdest->p1 = new (std::nothrow) _unitydate(); return pdest->p1 != 0;
            case utString: pdest->p1 = new (std::nothrow) std::wstring; return pdest->p1 != 0;
            case utMap: pdest->p1 = new (std::nothrow) unity::t_map; return pdest->p1 != 0;
            case utHash: pdest->p1 = new (std::nothrow) unity::t_hash; return pdest->p1 != 0;
            default: pdest->p1 = 0; return -1;
          }
        }
        else
        {
          switch (utt)
          {
            case utEmpty: pdest->p1 = 0; pdest->p2 = 0; return 1;
            case utInt: new ((meta::s_ll*)pdest) meta::s_ll(); return 1;
            case utChar: pdest->p1 = 0; pdest->p2 = 0; new ((char*)pdest) _unitychar(); return 1;
            case utFloat: new ((double*)pdest) double(); return 1;
            case utDate: new ((_unitydate*)pdest) _unitydate(); return 1;
            default: pdest->p1 = 0; pdest->p2 = 0; return -1;
          }
        }
      }
    }

      // pdest must be non-0.
      // flags:
      //  0x1 - pdest->p1 is a pointer. The object was dynamically allocated. (Same meaning as for Fcreate.).
      //    Calls type's op. delete. Sets pdest p1 and p2 to 0.
      //  0x4 - set p1, p2 to 0 even if utt is invalid.
      // NOTE when pdest->p1 is a pointer, p2 is expected to be unity* or 0. p2 is automatically deleted.
      // Returns flags:
      //  positive value:
      //    0x1 - p1 del. successfully, or scalar in *pdest was trivially erased.
      //      0x2 - p1 destructor failed,
      //    0x4 - p2 del. successfully,
      //      0x8 - p2 destructor failed.
      //  0 - p1 and p2 are 0.
      //  -1 - invalid utt/flags, nothing is deleted. p1 and p2 are set to 0 if 0x4 flag was set.
    typedef s_long (*PFdestroy)(unity::_storage* pdest, s_long utt, s_long flags);
    static inline s_long Ldestroy(void* (*pmsm)(s_long), unity::_storage* pdest, s_long utt, s_long flags) { if (pmsm == unity_common::ls_modsm) { return Fdestroy(pdest, utt, flags); } return ((PFdestroy)pmsm(unity_common::msm_cv_destroy))(pdest, utt, flags); }
    static s_long Fdestroy(unity::_storage* pdest, s_long utt, s_long flags)
    {
      if ((flags & 0x1) == 0)
      {
        if (utt >= utEmpty && utt < utString) { pdest->p1 = 0; pdest->p2 = 0; return 1; }
      }
      else
      {
        s_long res = 0;
        if (pdest->p1) { s_long res2 = cv_delete::F(pdest->p1, utt); if (res2 >= 0) { res |= res2 > 0 ? 0x1 : 0x2; } else { res = -1; } }
        if (res >= 0 && pdest->p2) { s_long res2 = cv_delete::F(pdest->p2, utUnity); res |= res2 > 0 ? 0x4 : 0x8; }
        if (res >= 0) { pdest->p1 = 0; pdest->p2 = 0; return res; }
      }
      if ((flags & 0x4)) { pdest->p1 = 0; pdest->p2 = 0; }
      return -1;
    }

    typedef unity* (*PFunity_new)();
    static inline unity* Lunity_new(void* (*pmsm)(s_long)) { if (pmsm == unity_common::ls_modsm) { return Funity_new(); } return ((PFunity_new)pmsm(unity_common::msm_cv_unity_new))(); }
    static inline unity* Funity_new() { return new (std::nothrow) unity; }
  };

  struct cv_convert
  {
      //  keep_name
      //    true -  keep object name, if it exists,
      //    false - the name will be reset if utt_dest != utype() and conversion succeeds.
      //  NOTE Fcnv on error:
      //    a) on csNormal, csStrict: generates exceptions. The object is left unchanged.
      //    b) on csLazy: sets the object to utEmpty. The object name is kept if keep_name is true.
    static inline void Lcnv(unity* p, EConvStrength cs, s_long utt_dest, s_long keep_name)
    {
      s_long res(0);
      if (p->pmsm == unity_common::ls_modsm) { res = Fcnv(p, cs, utt_dest, keep_name); } else { res = ((PFcnv)p->pmsm(unity_common::msm_cv_convert))(p, cs, utt_dest, keep_name); }
      if (res >= 0 || cs == csLazy) { return; }
      std::string e("Lcnv.A");
      e[e.size() - 1] = char(s_long('A') + (-res % 26));
      throw XUExec(e);
    }
    typedef s_long (*PFcnv)(unity* p, s_long cs, s_long utt_dest, s_long keep_name);
      // Fcnv returns:
      //  1 - on_calm_result.
      //   0 - utt_dest is same as the current type.
      //  < 0 - failure (the number shows where it occured.)
    static s_long Fcnv(unity* p, s_long cs, s_long utt_dest, s_long keep_name) throw();

      // flags:
      //    0x1 - keep object name, if it exists.
      //    0x2 - for any type, create new object dynamically, even if 0x1 is not set or the type is trivial.
      //      (Nonetheless, if px->isEmpty() and the name is not copied from it,
      //        p->_data.p1, p2 will be 0, and p->ut == utEmpty, without xfPtr.)
      //  NOTE Fasg generates exceptions on error (the object is left unchanged).
    static inline void Lasg(unity* p, const unity* px, s_long flags)
    {
      s_long res(0);
      if (p->pmsm == unity_common::ls_modsm) { res = Fasg(p, px, flags); } else { res = ((PFasg)p->pmsm(unity_common::msm_cv_assign))(p, px, flags); }
      if (res >= 0) { return; }
      std::string e("Lasg.A");
      e[e.size() - 1] = char(s_long('A') + (-res % 26));
      throw XUExec(e);
    }
      // Fasg returns:
      //  1 - on_calm_result.
      //   0 - p and px are the same.
      //  < 0 - failure (the number shows where it occured.)
    typedef s_long (*PFasg)(unity* p, const unity* px, s_long flags);
    static s_long Fasg(unity* p, const unity* px, s_long flags) throw();
  };

  struct u_set_name
  {
      // Returns:
      //    2 - name has been set successfully.
      //    1 (on pn->isEmpty()) - name has been removed successfully or not existed.
      //    0 - failed to set or remove name, nothing changed.
    typedef s_long (*PF)(unity* p, const unity* pn);
    static inline s_long L(unity* p, const unity* pn) { return ((PF)p->pmsm(unity_common::msm_u_set_name))(p, pn); }
    static s_long F(unity* p, const unity* pn)
    {
      if (pn->isEmpty()) // remove the current name
      {
        if (!p->isNamed()) { return 1; }
        unity*& pn2 = p->_drf_name(); try { delete pn2; } catch (...) {} pn2 = 0;
        return 1;
      }
      try
      {
        _pdyn pn2(new unity(pn->isScalar() ? *pn : pn->u_key()));
        if (p->isNamed()) { try { delete p->_drf_name(); } catch (...) {} p->_data.p2 = pn2.take(); return 2; }
        else
        {
          if (p->isByPtr()) { p->_data.p2 = pn2.take(); return 2; }
          unity temp; if (cv_ff::cv_convert::Fasg(&temp, p, 0x2) < 0) { return 0; }
          p->_data.p1 = temp._data.p1;
            temp._data.p1 = 0; temp.ut = utEmpty;
            p->_data.p2 = pn2.take(); p->ut |= unity::xfPtr;
          return 2;
        }
      }
      catch (...) { return 0; }
    }
  };

  struct u_clear
  {
      // flags:
      //    0x1 (for arrays, map, hashlist) - full clear, set base inds and ky fns to dflt.; otherwise only removes elements.
      //  NOTE The object name is not changed by this function.
      // Returns:
      //    2, 1 - cleared successfully.
      //    0 - failed to create the dflt. value (theoretically for strings), nothing done.
      //    -1 - unknown type, nothing done.
    typedef s_long (*PF)(unity* p, s_long flags);
    static inline s_long L(unity* p, s_long flags) { return ((PF)p->pmsm(unity_common::msm_u_clear))(p, flags); }
    static s_long F(unity* p, s_long flags);
  };
};


typedef hashx<std::string, unity_common::__Psm> t_hipsm;
static t_hipsm* _sy_i2psm() throw()
  { static int state(0); static storage_es_t<t_hipsm> x(state); return x(); }
unity_common::__Psm _ls_psm_find(const char* piname) throw()
{
  if (!piname) { return 0; }
  critsec_t<t_hipsm> __lock(100, -1); if (sizeof(__lock)) {}
  t_hipsm* ph = _sy_i2psm(); if (!ph) { return 0; }
  const hashx<std::string, unity_common::__Psm>::entry* e = ph->find(piname);
  unity_common::__Psm pf = e ? e->v : 0;
  return pf;
}
void unity_common::_ls_psm_set(const char* piname, unity_common::__Psm pf) throw()
{
  if (!piname) { return; }
  critsec_t<t_hipsm> __lock(100, -1); if (sizeof(__lock)) {}
  t_hipsm* ph = _sy_i2psm(); if (!ph) { return; }
  try { (*ph)[piname] = pf; } catch (...) {}
}
void* _ls_obj_pi(o_handler_base* ph, const char *piname) throw()
{
  typedef void* (*__Psm)(s_long);
  typedef void* (*__Ppi)(o_handler_base* ph);
  __Psm psm = _ls_psm_find(piname); if (!psm) { return 0; }
  __Ppi pi = (__Ppi)psm(unity_common::_defsm_pi); if (!pi) { return 0; }
  return pi(ph);
}
void* _ls_obj_cpvoid(o_handler_base* ph, s_long is_checked) throw ()
  { return ph->pvoid(is_checked); }
unity* _ls_obj_punity(o_handler_base* ph) throw ()
{
  void* p = ph->pvoid(true);
  if (p && typeid(unity) == ph->objStatTypeInfo()) { return reinterpret_cast<unity*>(p); }
  return 0;
}
void* _ls_obj_prefo(o_handler_base* ph, const char* tname) throw ()
  { return ph->prefo(tname); }


void* unity_common::ls_modsm(s_long ind)
{
  enum { size = unity_common::sizeof_msm };
  static void* smt[size] =
  {
    (void*)_ls_psm_find,
    (void*)_ls_obj_pi,
    (void*)_ls_obj_cpvoid,
    (void*)_ls_obj_punity,
    (void*)cv_ff::cv_reg::_F,
    (void*)cv_ff::cv_clear::F,
    (void*)cv_ff::cv_create::Fcreate,
    (void*)cv_ff::cv_create::Fdestroy,
    (void*)cv_ff::cv_create::Funity_new,
    (void*)cv_ff::cv_delete::F,
    (void*)cv_ff::cv_convert::Fcnv,
    (void*)cv_ff::cv_convert::Fasg,
    (void*)cv_ff::cv_wstring::Fnew,
    (void*)cv_ff::cv_wstring::Fn,
    (void*)cv_ff::cv_wstring::Fp0,
    (void*)cv_ff::cv_wstring::Fwcs,
    (void*)cv_ff::cv_wstring::Fti,
    (void*)cv_ff::cv_array::Fel_get,
    (void*)cv_ff::cv_array::Fel_set,
    (void*)cv_ff::cv_array::Fel_append,
    (void*)cv_ff::cv_array::Fel_insrem,
    (void*)cv_ff::cv_array::Fsz_set,
    (void*)cv_ff::cv_array::Flb_u,
    (void*)cv_ff::cv_array::Fub_u,
    (void*)cv_ff::cv_array::Fsz_u,
    (void*)cv_ff::u_set_name::F,
    (void*)cv_ff::u_clear::F,
    (void*)cv_ff::cv_array::Fua_fill,
    (void*)cv_ff::cv_array::Fua_ensure_ind,
    (void*)_ls_obj_prefo,
  };
  return ind >= 0 && ind < size ? smt[ind] : 0;
}

struct StaticConv
{

  // psrc: points to the source value.
  //  pdest: points to the destination value.
  //      For T with constructors, the destination value must be already initialized (e.g. by default).
  //  utt_src = any valid utEmpty..utUnityArray.
  //  utt_dest = any valid utInt..utUnityArray.
  //  flags:
  //    0x1 (not ignored only if utt_dest is an array): pdest points to std::vector, otherwise to vec2_t
  // NOTE conv_X_X calls assignment (operator=) for any X and cs, including csStrict.
  // NOTE conv_*_Unity calls assignment (operator=) for any input type and cs, including csStrict.
  // NOTE conv_Empty_* creates a default value for any output type and cs, including csStrict.
  //    psrc is ignored in this case.
  // NOTE conv_Object_Unity creates a new object reference.
  // NOTE for map and hash, only 6 conversions are defined:
  //  Empty --> Map, Map --> Map, Map --> Hash, Map --> unity.
  //  Empty --> Hash, Hash --> Map, Hash --> Hash, Hash --> unity.
  // NOTE no conversion is defined for:
  //    conv_*_Empty (== clear),
  //    conv_*_Object (== set_obj),
  //    conv_Object_UnityArray,
  //    conv_Object_UnityVector.
  // NOTE On error, conversion generates exceptions even with cs == csLazy.
  //    *pdest is left valid, but with undefined value.
inline static void cnv_all(EConvStrength cs, s_long utt_src, s_long utt_dest, s_long flags, const void* psrc, void* pdest)
{
  if (cs == csStrict && utt_src != utt_dest && utt_src != utEmpty && utt_dest != utUnity) { throw XUTypeMismatch("cnv_all.6", unity::_tname0(utt_src), unity::_tname0(utt_dest)); }

  s_long i1 = (utt_src & utMaskNoArray) + (utt_src & utArray ? _ut_end : 0);
  s_long i2 = (utt_dest & utMaskNoArray) + (utt_dest & utArray ? _ut_end : 0);

  static void* conv_table[2][2 * _ut_end][2 * _ut_end]
  = {
    {
      { 0, (void*)&conv_Empty_Int, (void*)&conv_Empty_Float, (void*)&conv_Empty_Date, (void*)&conv_Empty_Char, (void*)&conv_Empty_String, 0, (void*)&conv_Empty_Unity, (void*)&conv_Empty_Map, (void*)&conv_Empty_Hash, 0, (void*)&conv_Empty_IntArray, (void*)&conv_Empty_FloatArray, (void*)&conv_Empty_DateArray, (void*)&conv_Empty_CharArray, (void*)&conv_Empty_StringArray, 0, (void*)&conv_Empty_UnityArray, 0, 0 },
      { 0, (void*)&conv_Int_Int, (void*)&conv_Int_Float, (void*)&conv_Int_Date, (void*)&conv_Int_Char, (void*)&conv_Int_String, 0, (void*)&conv_Int_Unity, 0, 0, 0, (void*)&conv_Int_IntArray, (void*)&conv_Int_FloatArray, (void*)&conv_Int_DateArray, (void*)&conv_Int_CharArray, (void*)&conv_Int_StringArray, 0, (void*)&conv_Int_UnityArray, 0, 0 },
      { 0, (void*)&conv_Float_Int, (void*)&conv_Float_Float, (void*)&conv_Float_Date, (void*)&conv_Float_Char, (void*)&conv_Float_String, 0, (void*)&conv_Float_Unity, 0, 0, 0, (void*)&conv_Float_IntArray, (void*)&conv_Float_FloatArray, (void*)&conv_Float_DateArray, (void*)&conv_Float_CharArray, (void*)&conv_Float_StringArray, 0, (void*)&conv_Float_UnityArray, 0, 0 },
      { 0, (void*)&conv_Date_Int, (void*)&conv_Date_Float, (void*)&conv_Date_Date, (void*)&conv_Date_Char, (void*)&conv_Date_String, 0, (void*)&conv_Date_Unity, 0, 0, 0, (void*)&conv_Date_IntArray, (void*)&conv_Date_FloatArray, (void*)&conv_Date_DateArray, (void*)&conv_Date_CharArray, (void*)&conv_Date_StringArray, 0, (void*)&conv_Date_UnityArray, 0, 0 },
      { 0, (void*)&conv_Char_Int, (void*)&conv_Char_Float, (void*)&conv_Char_Date, (void*)&conv_Char_Char, (void*)&conv_Char_String, 0, (void*)&conv_Char_Unity, 0, 0, 0, (void*)&conv_Char_IntArray, (void*)&conv_Char_FloatArray, (void*)&conv_Char_DateArray, (void*)&conv_Char_CharArray, (void*)&conv_Char_StringArray, 0, (void*)&conv_Char_UnityArray, 0, 0 },
      { 0, (void*)&conv_String_Int, (void*)&conv_String_Float, (void*)&conv_String_Date, (void*)&conv_String_Char, (void*)&conv_String_String, 0, (void*)&conv_String_Unity, 0, 0, 0, (void*)&conv_String_IntArray, (void*)&conv_String_FloatArray, (void*)&conv_String_DateArray, (void*)&conv_String_CharArray, (void*)&conv_String_StringArray, 0, (void*)&conv_String_UnityArray, 0, 0 },
      { 0, (void*)&conv_Object_Int, (void*)&conv_Object_Float, (void*)&conv_Object_Date, (void*)&conv_Object_Char, (void*)&conv_Object_String, 0, (void*)&conv_Object_Unity, 0, 0, 0, (void*)&conv_Object_IntArray, (void*)&conv_Object_FloatArray, (void*)&conv_Object_DateArray, (void*)&conv_Object_CharArray, (void*)&conv_Object_StringArray, 0, 0, 0, 0 },
      { 0, (void*)&conv_Unity_Int, (void*)&conv_Unity_Float, (void*)&conv_Unity_Date, (void*)&conv_Unity_Char, (void*)&conv_Unity_String, 0, (void*)&conv_Unity_Unity, 0, 0, 0, (void*)&conv_Unity_IntArray, (void*)&conv_Unity_FloatArray, (void*)&conv_Unity_DateArray, (void*)&conv_Unity_CharArray, (void*)&conv_Unity_StringArray, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 0, (void*)&conv_Map_Unity, (void*)&conv_Map_Map, (void*)&conv_Map_Hash, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 0, (void*)&conv_Hash_Unity, (void*)&conv_Hash_Map, (void*)&conv_Hash_Hash, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
      { 0, (void*)&conv_IntArray_Int, (void*)&conv_IntArray_Float, (void*)&conv_IntArray_Date, (void*)&conv_IntArray_Char, (void*)&conv_IntArray_String, 0, (void*)&conv_IntArray_Unity, 0, 0, 0, (void*)&conv_IntArray_IntArray, (void*)&conv_IntArray_FloatArray, (void*)&conv_IntArray_DateArray, (void*)&conv_IntArray_CharArray, (void*)&conv_IntArray_StringArray, 0, (void*)&conv_IntArray_UnityArray, 0, 0 },
      { 0, (void*)&conv_FloatArray_Int, (void*)&conv_FloatArray_Float, (void*)&conv_FloatArray_Date, (void*)&conv_FloatArray_Char, (void*)&conv_FloatArray_String, 0, (void*)&conv_FloatArray_Unity, 0, 0, 0, (void*)&conv_FloatArray_IntArray, (void*)&conv_FloatArray_FloatArray, (void*)&conv_FloatArray_DateArray, (void*)&conv_FloatArray_CharArray, (void*)&conv_FloatArray_StringArray, 0, (void*)&conv_FloatArray_UnityArray, 0, 0 },
      { 0, (void*)&conv_DateArray_Int, (void*)&conv_DateArray_Float, (void*)&conv_DateArray_Date, (void*)&conv_DateArray_Char, (void*)&conv_DateArray_String, 0, (void*)&conv_DateArray_Unity, 0, 0, 0, (void*)&conv_DateArray_IntArray, (void*)&conv_DateArray_FloatArray, (void*)&conv_DateArray_DateArray, (void*)&conv_DateArray_CharArray, (void*)&conv_DateArray_StringArray, 0, (void*)&conv_DateArray_UnityArray, 0, 0 },
      { 0, (void*)&conv_CharArray_Int, (void*)&conv_CharArray_Float, (void*)&conv_CharArray_Date, (void*)&conv_CharArray_Char, (void*)&conv_CharArray_String, 0, (void*)&conv_CharArray_Unity, 0, 0, 0, (void*)&conv_CharArray_IntArray, (void*)&conv_CharArray_FloatArray, (void*)&conv_CharArray_DateArray, (void*)&conv_CharArray_CharArray, (void*)&conv_CharArray_StringArray, 0, (void*)&conv_CharArray_UnityArray, 0, 0 },
      { 0, (void*)&conv_StringArray_Int, (void*)&conv_StringArray_Float, (void*)&conv_StringArray_Date, (void*)&conv_StringArray_Char, (void*)&conv_StringArray_String, 0, (void*)&conv_StringArray_Unity, 0, 0, 0, (void*)&conv_StringArray_IntArray, (void*)&conv_StringArray_FloatArray, (void*)&conv_StringArray_DateArray, (void*)&conv_StringArray_CharArray, (void*)&conv_StringArray_StringArray, 0, (void*)&conv_StringArray_UnityArray, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
      { 0, (void*)&conv_UnityArray_Int, (void*)&conv_UnityArray_Float, (void*)&conv_UnityArray_Date, (void*)&conv_UnityArray_Char, (void*)&conv_UnityArray_String, 0, (void*)&conv_UnityArray_Unity, 0, 0, 0, (void*)&conv_UnityArray_IntArray, (void*)&conv_UnityArray_FloatArray, (void*)&conv_UnityArray_DateArray, (void*)&conv_UnityArray_CharArray, (void*)&conv_UnityArray_StringArray, 0, (void*)&conv_UnityArray_UnityArray, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    },
    {
      { 0, (void*)&conv_Empty_Int, (void*)&conv_Empty_Float, (void*)&conv_Empty_Date, (void*)&conv_Empty_Char, (void*)&conv_Empty_String, 0, (void*)&conv_Empty_Unity, (void*)&conv_Empty_Map, (void*)&conv_Empty_Hash, 0, (void*)&conv_Empty_IntArray, (void*)&conv_Empty_FloatArray, (void*)&conv_Empty_DateArray, (void*)&conv_Empty_CharArray, (void*)&conv_Empty_StringArray, 0, (void*)&conv_Empty_UnityArray, 0, 0 },
      { 0, (void*)&conv_Int_Int, (void*)&conv_Int_Float, (void*)&conv_Int_Date, (void*)&conv_Int_Char, (void*)&conv_Int_String, 0, (void*)&conv_Int_Unity, 0, 0, 0, (void*)&conv_Int_IntArray, (void*)&conv_Int_FloatArray, (void*)&conv_Int_DateArray, (void*)&conv_Int_CharArray, (void*)&conv_Int_StringArray, 0, (void*)&conv_Int_UnityArray, 0, 0 },
      { 0, (void*)&conv_Float_Int, (void*)&conv_Float_Float, (void*)&conv_Float_Date, (void*)&conv_Float_Char, (void*)&conv_Float_String, 0, (void*)&conv_Float_Unity, 0, 0, 0, (void*)&conv_Float_IntArray, (void*)&conv_Float_FloatArray, (void*)&conv_Float_DateArray, (void*)&conv_Float_CharArray, (void*)&conv_Float_StringArray, 0, (void*)&conv_Float_UnityArray, 0, 0 },
      { 0, (void*)&conv_Date_Int, (void*)&conv_Date_Float, (void*)&conv_Date_Date, (void*)&conv_Date_Char, (void*)&conv_Date_String, 0, (void*)&conv_Date_Unity, 0, 0, 0, (void*)&conv_Date_IntArray, (void*)&conv_Date_FloatArray, (void*)&conv_Date_DateArray, (void*)&conv_Date_CharArray, (void*)&conv_Date_StringArray, 0, (void*)&conv_Date_UnityArray, 0, 0 },
      { 0, (void*)&conv_Char_Int, (void*)&conv_Char_Float, (void*)&conv_Char_Date, (void*)&conv_Char_Char, (void*)&conv_Char_String, 0, (void*)&conv_Char_Unity, 0, 0, 0, (void*)&conv_Char_IntArray, (void*)&conv_Char_FloatArray, (void*)&conv_Char_DateArray, (void*)&conv_Char_CharArray, (void*)&conv_Char_StringArray, 0, (void*)&conv_Char_UnityArray, 0, 0 },
      { 0, (void*)&conv_String_Int, (void*)&conv_String_Float, (void*)&conv_String_Date, (void*)&conv_String_Char, (void*)&conv_String_String, 0, (void*)&conv_String_Unity, 0, 0, 0, (void*)&conv_String_IntArray, (void*)&conv_String_FloatArray, (void*)&conv_String_DateArray, (void*)&conv_String_CharArray, (void*)&conv_String_StringArray, 0, (void*)&conv_String_UnityArray, 0, 0 },
      { 0, (void*)&conv_Object_Int, (void*)&conv_Object_Float, (void*)&conv_Object_Date, (void*)&conv_Object_Char, (void*)&conv_Object_String, 0, (void*)&conv_Object_Unity, 0, 0, 0, (void*)&conv_Object_IntArray, (void*)&conv_Object_FloatArray, (void*)&conv_Object_DateArray, (void*)&conv_Object_CharArray, (void*)&conv_Object_StringArray, 0, 0, 0, 0 },
      { 0, (void*)&conv_Unity_Int, (void*)&conv_Unity_Float, (void*)&conv_Unity_Date, (void*)&conv_Unity_Char, (void*)&conv_Unity_String, 0, (void*)&conv_Unity_Unity, 0, 0, 0, (void*)&conv_Unity_IntArray, (void*)&conv_Unity_FloatArray, (void*)&conv_Unity_DateArray, (void*)&conv_Unity_CharArray, (void*)&conv_Unity_StringArray, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 0, (void*)&conv_Map_Unity, (void*)&conv_Map_Map, (void*)&conv_Map_Hash, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 0, (void*)&conv_Hash_Unity, (void*)&conv_Hash_Map, (void*)&conv_Hash_Hash, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },

      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
      { 0, (void*)&conv_IntArray_Int, (void*)&conv_IntArray_Float, (void*)&conv_IntArray_Date, (void*)&conv_IntArray_Char, (void*)&conv_IntArray_String, 0, (void*)&conv_IntArray_Unity, 0, 0, 0, (void*)&conv_IntArray_IntVector, (void*)&conv_IntArray_FloatVector, (void*)&conv_IntArray_DateVector, (void*)&conv_IntArray_CharVector, (void*)&conv_IntArray_StringVector, 0, (void*)&conv_IntArray_UnityVector, 0, 0 },
      { 0, (void*)&conv_FloatArray_Int, (void*)&conv_FloatArray_Float, (void*)&conv_FloatArray_Date, (void*)&conv_FloatArray_Char, (void*)&conv_FloatArray_String, 0, (void*)&conv_FloatArray_Unity, 0, 0, 0, (void*)&conv_FloatArray_IntVector, (void*)&conv_FloatArray_FloatVector, (void*)&conv_FloatArray_DateVector, (void*)&conv_FloatArray_CharVector, (void*)&conv_FloatArray_StringVector, 0, (void*)&conv_FloatArray_UnityVector, 0, 0 },
      { 0, (void*)&conv_DateArray_Int, (void*)&conv_DateArray_Float, (void*)&conv_DateArray_Date, (void*)&conv_DateArray_Char, (void*)&conv_DateArray_String, 0, (void*)&conv_DateArray_Unity, 0, 0, 0, (void*)&conv_DateArray_IntVector, (void*)&conv_DateArray_FloatVector, (void*)&conv_DateArray_DateVector, (void*)&conv_DateArray_CharVector, (void*)&conv_DateArray_StringVector, 0, (void*)&conv_DateArray_UnityVector, 0, 0 },
      { 0, (void*)&conv_CharArray_Int, (void*)&conv_CharArray_Float, (void*)&conv_CharArray_Date, (void*)&conv_CharArray_Char, (void*)&conv_CharArray_String, 0, (void*)&conv_CharArray_Unity, 0, 0, 0, (void*)&conv_CharArray_IntVector, (void*)&conv_CharArray_FloatVector, (void*)&conv_CharArray_DateVector, (void*)&conv_CharArray_CharVector, (void*)&conv_CharArray_StringVector, 0, (void*)&conv_CharArray_UnityVector, 0, 0 },
      { 0, (void*)&conv_StringArray_Int, (void*)&conv_StringArray_Float, (void*)&conv_StringArray_Date, (void*)&conv_StringArray_Char, (void*)&conv_StringArray_String, 0, (void*)&conv_StringArray_Unity, 0, 0, 0, (void*)&conv_StringArray_IntVector, (void*)&conv_StringArray_FloatVector, (void*)&conv_StringArray_DateVector, (void*)&conv_StringArray_CharVector, (void*)&conv_StringArray_StringVector, 0, (void*)&conv_StringArray_UnityVector, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
      { 0, (void*)&conv_UnityArray_Int, (void*)&conv_UnityArray_Float, (void*)&conv_UnityArray_Date, (void*)&conv_UnityArray_Char, (void*)&conv_UnityArray_String, 0, (void*)&conv_UnityArray_Unity, 0, 0, 0, (void*)&conv_UnityArray_IntVector, (void*)&conv_UnityArray_FloatVector, (void*)&conv_UnityArray_DateVector, (void*)&conv_UnityArray_CharVector, (void*)&conv_UnityArray_StringVector, 0, (void*)&conv_UnityArray_UnityVector, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    },
  };

  typedef void(*Conv1)(EConvStrength cs, char& pdest);
  typedef void(*Conv2)(EConvStrength cs, const char& psrc, char& pdest);

  if (i1 >= 2 * _ut_end || i2 >=  2 * _ut_end) { throw XUConvFailed("cnv_all.1", i1, i2, flags); }
  try
  {
    if (i1 == utEmpty)
    {
      Conv1 p = (Conv1)conv_table[flags & 0x1 ? 1 : 0][i1][i2];
      if (p && pdest) { p(cs, *(char*)pdest); }
        else { throw XUConvFailed("cnv_all.2", i1, i2, flags); }
    }
    else
    {
      Conv2 p = (Conv2)conv_table[flags & 0x1 ? 1 : 0][i1][i2];
      if (p && psrc && pdest) { p(cs, *(char*)psrc, *(char*)pdest); }
        else { throw XUConvFailed("cnv_all.3", i1, i2, flags); }
    }
  }
  catch (_XUBase&) { throw; }
  catch (std::exception& e) { throw XUConvFailed("cnv_all.4", e.what(), i1, i2, flags); }
  catch (...) { throw XUConvFailed("cnv_all.5", i1, i2, flags); }
}

//========================================================================

static void conv_Empty_Int(EConvStrength cs, meta::s_ll& retval) { retval=0L; }
static void conv_Empty_Float(EConvStrength cs, double& retval) { retval=0.; }
static void conv_Empty_Date(EConvStrength cs, _unitydate& retval) { retval=_unitydate(0.); }
static void conv_Empty_String(EConvStrength cs, std::wstring& retval) { retval=L""; }
static void conv_Empty_Char(EConvStrength cs, _unitychar& retval) { retval=0; }
static void conv_Empty_Unity(EConvStrength cs, unity& retval) { retval.clear(); }
static void conv_Empty_Map(EConvStrength cs, unity::t_map& retval) { retval.ordhs_clear(); }
static void conv_Empty_Hash(EConvStrength cs, unity::t_hash& retval) { retval.hl_clear(true); }

static void conv_Empty_UnityArray(EConvStrength cs, vec2_t<unity>& retval) { retval.clear(); }
static void conv_Empty_IntArray(EConvStrength cs, vec2_t<meta::s_ll>& retval) { retval.clear(); }
static void conv_Empty_FloatArray(EConvStrength cs, vec2_t<double>& retval) { retval.clear(); }
static void conv_Empty_DateArray(EConvStrength cs, vec2_t<_unitydate>& retval) { retval.clear(); }
static void conv_Empty_StringArray(EConvStrength cs, vec2_t<std::wstring>& retval) { retval.clear(); }
static void conv_Empty_CharArray(EConvStrength cs, vec2_t<_unitychar>& retval) { retval.clear(); }

static void conv_Empty_UnityVector(EConvStrength cs, std::vector<unity>& retval) { retval.clear(); }
static void conv_Empty_IntVector(EConvStrength cs, std::vector<meta::s_ll>& retval) { retval.clear(); }
static void conv_Empty_FloatVector(EConvStrength cs, std::vector<double>& retval) { retval.clear(); }
static void conv_Empty_DateVector(EConvStrength cs, std::vector<_unitydate>& retval) { retval.clear(); }
static void conv_Empty_StringVector(EConvStrength cs, std::vector<std::wstring>& retval) { retval.clear(); }
static void conv_Empty_CharVector(EConvStrength cs, std::vector<_unitychar>& retval) { retval.clear(); }

//========================================================================

static void conv_Int_Int(EConvStrength cs, const meta::s_ll& x, meta::s_ll& retval) { retval = x; }
static void conv_Int_Float(EConvStrength cs, const meta::s_ll& x, double& retval) {   retval = double(x); }
static void conv_Int_Date(EConvStrength cs, const meta::s_ll& x, _unitydate& retval) {   retval.set(double(x)); }
static void conv_Int_String(EConvStrength cs, const meta::s_ll& x, std::wstring& retval) {   retval = ntows(x); }
static void conv_Int_Char(EConvStrength cs, const meta::s_ll& x, _unitychar& retval) {   retval = _unitychar(x); }
static void conv_Int_Unity(EConvStrength cs, const meta::s_ll& x, unity& retval) { retval = x; }

static void conv_Int_UnityArray(EConvStrength cs, const meta::s_ll& x, vec2_t<unity>& retval) {  retval.clear(); retval.push_back(x); }
static void conv_Int_IntArray(EConvStrength cs, const meta::s_ll& x, vec2_t<meta::s_ll>& retval) {  retval.clear(); retval.push_back(x); }
static void conv_Int_FloatArray(EConvStrength cs, const meta::s_ll& x, vec2_t<double>& retval) {  retval.clear(); retval.push_back(double(x)); }
static void conv_Int_DateArray(EConvStrength cs, const meta::s_ll& x, vec2_t<_unitydate>& retval) {  retval.clear(); retval.push_back(_unitydate(double(x))); }
static void conv_Int_StringArray(EConvStrength cs, const meta::s_ll& x, vec2_t<std::wstring>& retval) {  retval.clear(); std::wstring s; conv_Int_String(cs, x, s); retval.push_back(s); }
static void conv_Int_CharArray(EConvStrength cs, const meta::s_ll& x, vec2_t<_unitychar>& retval) {  retval.clear(); _unitychar b; conv_Int_Char(cs, x, b); retval.push_back(b); }

static void conv_Int_UnityVector(EConvStrength cs, const meta::s_ll& x, std::vector<unity>& retval) {  retval.clear(); retval.push_back(x); }
static void conv_Int_IntVector(EConvStrength cs, const meta::s_ll& x, std::vector<meta::s_ll>& retval) {  retval.clear(); retval.push_back(x); }
static void conv_Int_FloatVector(EConvStrength cs, const meta::s_ll& x, std::vector<double>& retval) {  retval.clear(); retval.push_back(double(x)); }
static void conv_Int_DateVector(EConvStrength cs, const meta::s_ll& x, std::vector<_unitydate>& retval) {  retval.clear(); retval.push_back(_unitydate(double(x))); }
static void conv_Int_StringVector(EConvStrength cs, const meta::s_ll& x, std::vector<std::wstring>& retval) {  retval.clear(); std::wstring s; conv_Int_String(cs, x, s); retval.push_back(s); }
static void conv_Int_CharVector(EConvStrength cs, const meta::s_ll& x, std::vector<_unitychar>& retval) {  retval.clear(); _unitychar b; conv_Int_Char(cs, x, b); retval.push_back(b); }

//========================================================================

static void conv_Char_Int(EConvStrength cs, const _unitychar& x, meta::s_ll& retval) {   retval=x?1L:0L; }
static void conv_Char_Float(EConvStrength cs, const _unitychar& x, double& retval) {   retval=x?1.:0.; }
static void conv_Char_Date(EConvStrength cs, const _unitychar& x, _unitydate& retval) { retval = d_time(0, (s_long((unsigned char)x)) / 60, (s_long((unsigned char)x)) % 60); }
static void conv_Char_String(EConvStrength cs, const _unitychar& x, std::wstring& retval) {   retval=ntows(s_long(x)); }
static void conv_Char_Char(EConvStrength cs, const _unitychar& x, _unitychar& retval) { retval = x; }
static void conv_Char_Unity(EConvStrength cs, const _unitychar& x, unity& retval) { retval = x; }

static void conv_Char_UnityArray(EConvStrength cs, const _unitychar& x, vec2_t<unity>& retval) {  retval.clear(); retval.push_back(x); }
static void conv_Char_IntArray(EConvStrength cs, const _unitychar& x, vec2_t<meta::s_ll>& retval) {  retval.clear(); meta::s_ll temp; conv_Char_Int(cs, x, temp); retval.push_back(temp); }
static void conv_Char_FloatArray(EConvStrength cs, const _unitychar& x, vec2_t<double>& retval) {  retval.clear(); double temp; conv_Char_Float(cs, x, temp); retval.push_back(temp); }
static void conv_Char_DateArray(EConvStrength cs, const _unitychar& x, vec2_t<_unitydate>& retval) {  retval.clear(); _unitydate temp; conv_Char_Date(cs, x, temp); retval.push_back(temp); }
static void conv_Char_StringArray(EConvStrength cs, const _unitychar& x, vec2_t<std::wstring>& retval) {  retval.clear(); std::wstring s; conv_Char_String(cs, x, s); retval.push_back(s); }
static void conv_Char_CharArray(EConvStrength cs, const _unitychar& x, vec2_t<_unitychar>& retval) {  retval.clear(); retval.push_back(x); }

static void conv_Char_UnityVector(EConvStrength cs, const _unitychar& x, std::vector<unity>& retval) {  retval.clear(); retval.push_back(x); }
static void conv_Char_IntVector(EConvStrength cs, const _unitychar& x, std::vector<meta::s_ll>& retval) {  retval.clear(); meta::s_ll temp; conv_Char_Int(cs, x, temp); retval.push_back(temp); }
static void conv_Char_FloatVector(EConvStrength cs, const _unitychar& x, std::vector<double>& retval) {  retval.clear(); double x2; conv_Char_Float(cs, x, x2); retval.push_back(x2); }
static void conv_Char_DateVector(EConvStrength cs, const _unitychar& x, std::vector<_unitydate>& retval) {  retval.clear(); _unitydate x2; conv_Char_Date(cs, x, x2); retval.push_back(x2); }
static void conv_Char_StringVector(EConvStrength cs, const _unitychar& x, std::vector<std::wstring>& retval) {  retval.clear(); std::wstring s; conv_Char_String(cs, x, s); retval.push_back(s); }
static void conv_Char_CharVector(EConvStrength cs, const _unitychar& x, std::vector<_unitychar>& retval) {  retval.clear(); retval.push_back(x); }

//========================================================================

static void conv_Float_Int(EConvStrength cs, const double& x, meta::s_ll& retval) { retval=meta::s_ll(floor(x)); }
static void conv_Float_Float(EConvStrength cs, const double& x, double& retval) { retval = x; }
static void conv_Float_Date(EConvStrength cs, const double& x, _unitydate& retval) {   retval.set(x); }
static void conv_Float_String(EConvStrength cs, const double& x, std::wstring& retval) {   retval = _fls75(x, 15, 15).wstr(); }
static void conv_Float_Char(EConvStrength cs, const double& x, _unitychar& retval) {   retval=_unitychar(meta::s_ll(floor(x))); }
static void conv_Float_Unity(EConvStrength cs, const double& x, unity& retval) { retval = x; }

static void conv_Float_UnityArray(EConvStrength cs, const double& x, vec2_t<unity>& retval) {  retval.clear(); retval.push_back(x); }
static void conv_Float_IntArray(EConvStrength cs, const double& x, vec2_t<meta::s_ll>& retval) { retval.clear(); retval.push_back(meta::s_ll(floor(x))); }
static void conv_Float_FloatArray(EConvStrength cs, const double& x, vec2_t<double>& retval) {  retval.clear(); retval.push_back(x); }
static void conv_Float_DateArray(EConvStrength cs, const double& x, vec2_t<_unitydate>& retval) {  retval.clear(); retval.push_back(_unitydate(x)); }
static void conv_Float_StringArray(EConvStrength cs, const double& x, vec2_t<std::wstring>& retval) {  retval.clear(); std::wstring s; conv_Float_String(cs, x, s); retval.push_back(s); }
static void conv_Float_CharArray(EConvStrength cs, const double& x, vec2_t<_unitychar>& retval) {  retval.clear(); _unitychar b; conv_Float_Char(cs, x, b); retval.push_back(b); }

static void conv_Float_UnityVector(EConvStrength cs, const double& x, std::vector<unity>& retval) {  retval.clear(); retval.push_back(x); }
static void conv_Float_IntVector(EConvStrength cs, const double& x, std::vector<meta::s_ll>& retval) { retval.clear(); retval.push_back(meta::s_ll(floor(x))); }
static void conv_Float_FloatVector(EConvStrength cs, const double& x, std::vector<double>& retval) {  retval.clear(); retval.push_back(x); }
static void conv_Float_DateVector(EConvStrength cs, const double& x, std::vector<_unitydate>& retval) {  retval.clear(); retval.push_back(_unitydate(x)); }
static void conv_Float_StringVector(EConvStrength cs, const double& x, std::vector<std::wstring>& retval) {  retval.clear(); std::wstring s; conv_Float_String(cs, x, s); retval.push_back(s); }
static void conv_Float_CharVector(EConvStrength cs, const double& x, std::vector<_unitychar>& retval) {  retval.clear(); _unitychar b; conv_Float_Char(cs, x, b); retval.push_back(b); }

//========================================================================

static void conv_Date_Int(EConvStrength cs, const _unitydate& x, meta::s_ll& retval) { retval=meta::s_ll(floor(x.f())); return; }
static void conv_Date_Float(EConvStrength cs, const _unitydate& x, double& retval) {   retval = x.f(); }
static void conv_Date_Date(EConvStrength cs, const _unitydate& x, _unitydate& retval) { retval = x; }
static void conv_Date_String(EConvStrength cs, const _unitydate& x, std::wstring& retval) { retval = dtofls(x).wstr(); }
static void conv_Date_Char(EConvStrength cs, const _unitydate& x, _unitychar& retval) {   retval = _unitychar(meta::s_ll(floor(x.f()))); }
static void conv_Date_Unity(EConvStrength cs, const _unitydate& x, unity& retval) { retval = x; }

static void conv_Date_UnityArray(EConvStrength cs, const _unitydate& x, vec2_t<unity>& retval) {  retval.clear(); retval.push_back(x); }
static void conv_Date_IntArray(EConvStrength cs, const _unitydate& x, vec2_t<meta::s_ll>& retval) { retval.clear(); retval.push_back(meta::s_ll(floor(x.f()))); }
static void conv_Date_FloatArray(EConvStrength cs, const _unitydate& x, vec2_t<double>& retval) {  retval.push_back(x.f()); }
static void conv_Date_DateArray(EConvStrength cs, const _unitydate& x, vec2_t<_unitydate>& retval) {  retval.push_back(x); }
static void conv_Date_StringArray(EConvStrength cs, const _unitydate& x, vec2_t<std::wstring>& retval) {  retval.clear(); std::wstring s; conv_Date_String(cs, x, s); retval.push_back(s); }
static void conv_Date_CharArray(EConvStrength cs, const _unitydate& x, vec2_t<_unitychar>& retval) {  retval.clear(); _unitychar b; conv_Date_Char(cs, x, b); retval.push_back(b); }

static void conv_Date_UnityVector(EConvStrength cs, const _unitydate& x, std::vector<unity>& retval) {  retval.clear(); retval.push_back(x); }
static void conv_Date_IntVector(EConvStrength cs, const _unitydate& x, std::vector<meta::s_ll>& retval) {  retval.clear(); retval.push_back(meta::s_ll(floor(x.f()))); }
static void conv_Date_FloatVector(EConvStrength cs, const _unitydate& x, std::vector<double>& retval) {  retval.push_back(x.f()); }
static void conv_Date_DateVector(EConvStrength cs, const _unitydate& x, std::vector<_unitydate>& retval) {  retval.push_back(x); }
static void conv_Date_StringVector(EConvStrength cs, const _unitydate& x, std::vector<std::wstring>& retval) {  retval.clear(); std::wstring s; conv_Date_String(cs, x, s); retval.push_back(s); }
static void conv_Date_CharVector(EConvStrength cs, const _unitydate& x, std::vector<_unitychar>& retval) {  retval.clear(); _unitychar b; conv_Date_Char(cs, x, b); retval.push_back(b); }

//========================================================================

static void conv_String_Int(EConvStrength cs, const std::wstring& x, meta::s_ll& retval)
{
  try { retval = str2i(x, 0, false); return; } catch (...) {}
  if (cs == csLazy) { retval = 0; return; }
  throw XUConvFailed("conv_String_Int.1", ".String", x);
}

static void conv_String_Float(EConvStrength cs, const std::wstring& x, double& retval)
{
  try { retval = str2f(x, 0, false); return; } catch (...) {}
  if (cs == csLazy) { retval = 0.; return; }
  throw XUConvFailed("conv_String_Float.1", ".String", ".Float", x);
}

static bool conv_String_Date0(const std::wstring& x, _unitydate& retval, bool no_exc)
{
    try
    {
      std::string s0 = wsToBs(x); _t_sz pos = 0;
      // \s*[+-]?y{>=1 w/sign | >=4 w/o sign}[-/.]mm[-/.]dd(\s+h{1,2}:mm(:ss([.][0-9]+)?)?)?
      const std::string space(" \t");
      const std::string digit("0123456789");
      const std::string pm("+-");
      const std::string sep1("-/.");
      const std::string sep2(":-");
      const std::string sep3(".");
      std::string s;
      bool is_signed(false); bool b(false); s_long sign(1), y(0), m(0), d(0), h(0), mt(0), sec(0); bool is_frac(false); double frac(0.);
      do // once
      {
          // leading spaces
        s = trim_n(s0, pos, space, -1); pos += s.size();

          // year sign, year, month, day
        s = trim_n(s0, pos, pm, 1); pos += s.size();
          if (s.size() > 0) { is_signed = true; sign = s[0] == '+' ? 1 : -1; }
        s = trim_n(s0, pos, digit, 9); pos += s.size();
          if (!(s.size() >= 4 || (is_signed && s.size() >= 1))) { break; }
            y = s_long(atol(s.c_str())) * sign; if (s.size() > 4 && y == 0) { break; }
        s = trim_n(s0, pos, sep1, 1); pos += s.size(); if (s.size() != 1) { break; }
        s = trim_n(s0, pos, digit, 2); pos += s.size();
          if (s.size() != 2) { break; }
            m = s_long(atol(s.c_str())); if (m < 1 || m > 12) { break; }
        s = trim_n(s0, pos, sep1, 1); pos += s.size(); if (s.size() != 1) { break; }
        s = trim_n(s0, pos, digit, 2); pos += s.size();
          if (s.size() != 2) { break; }
            d = s_long(atol(s.c_str())); if (d < 1 || d > 31) { break; }

          // spaces between date and time (or possibly trailing spaces)
        s = trim_n(s0, pos, space, -1); pos += s.size();
          if (pos == s0.size()) { b = true; break; } if (s.size() == 0) { break; }

          // hours, minutes
        s = trim_n(s0, pos, digit, 2); pos += s.size();
          if (!(s.size() == 1 || s.size() == 2)) { break; }
            h = s_long(atol(s.c_str())); if (h > 23) { break; }
        s = trim_n(s0, pos, sep2, 1); pos += s.size(); if (s.size() != 1) { break; }
        s = trim_n(s0, pos, digit, 2); pos += s.size();
          if (s.size() != 2) { break; }
            mt = s_long(atol(s.c_str())); if (mt > 59) { break; }

          // possibly trailing spaces
        s = trim_n(s0, pos, space, -1); pos += s.size(); if (pos == s0.size() || s.size() > 0) { b = true; break; }

          // seconds
        s = trim_n(s0, pos, sep2, 1); pos += s.size(); if (s.size() != 1) { break; }
        s = trim_n(s0, pos, digit, 2); pos += s.size();
          if (s.size() != 2) { break; }
            sec = s_long(atol(s.c_str())); if (sec > 59) { break; }

          // possibly trailing spaces
        s = trim_n(s0, pos, space, -1); pos += s.size(); if (pos == s0.size() || s.size() > 0) { b = true; break; }

          // frac. part of seconds
        s = trim_n(s0, pos, sep3, 1); pos += s.size(); if (s.size() != 1) { break; }
        s = trim_n(s0, pos, digit, 9); pos += s.size();
          if (s.size() == 0) { break; }
            is_frac = true; s = "." + s; frac = atof(s.c_str());

          // end of std::string or trailing spaces
        s = trim_n(s0, pos, space, -1); pos += s.size(); if (pos == s0.size() || s.size() > 0) { b = true; break; }
      }
      while (false);
      if (b) { retval = _unitydate(y, m, d, h, mt, sec); if (is_frac) { retval.add_seconds(frac); } return true; }
      return false;
    } catch (...) { if (no_exc) { return false; } throw; }
}
static void conv_String_Date(EConvStrength cs, const std::wstring& x, _unitydate& retval)
{
  try
  {
    if (conv_String_Date0(x, retval, false)) { return; }
    throw XUConvFailed("conv_String_Date.1", ".String", ".Date", x);
  }
  catch(_XUBase&) { if (cs!=csLazy) throw; }
  catch(std::exception& e) { if (cs!=csLazy) { throw XUConvFailed("conv_String_Date.2", ".String", ".Date", e.what(), x); } }
  catch(...) { if (cs!=csLazy) { throw XUConvFailed("conv_String_Date.3", ".String", ".Date", x); } }
  retval.set(0.);
}
static void conv_String_String(EConvStrength cs, const std::wstring& x, std::wstring& retval) { retval = x; }
static void conv_String_Char(EConvStrength cs, const std::wstring& x, _unitychar& retval) { meta::s_ll temp; conv_String_Int(cs, x, temp); retval = _unitychar(temp); }
static void conv_String_Unity(EConvStrength cs, const std::wstring& x, unity& retval) { retval = x; }

static void conv_String_UnityArray(EConvStrength cs, const std::wstring& x, vec2_t<unity>& retval) {  retval.clear(); retval.push_back(x); }
static void conv_String_IntArray(EConvStrength cs, const std::wstring& x, vec2_t<meta::s_ll>& retval) {  retval.clear(); meta::s_ll temp; conv_String_Int(cs, x, temp); retval.push_back(temp); }
static void conv_String_FloatArray(EConvStrength cs, const std::wstring& x, vec2_t<double>& retval) {  retval.clear(); double y; conv_String_Float(cs, x, y); retval.push_back(y); }
static void conv_String_DateArray(EConvStrength cs, const std::wstring& x, vec2_t<_unitydate>& retval) {  retval.clear(); _unitydate y; conv_String_Date(cs, x, y); retval.push_back(y); }
static void conv_String_StringArray(EConvStrength cs, const std::wstring& x, vec2_t<std::wstring>& retval) {  retval.clear(); retval.push_back(x); }
static void conv_String_CharArray(EConvStrength cs, const std::wstring& x, vec2_t<_unitychar>& retval) {  retval.clear(); _unitychar b; conv_String_Char(cs, x, b); retval.push_back(b); }

static void conv_String_UnityVector(EConvStrength cs, const std::wstring& x, std::vector<unity>& retval) {  retval.clear(); retval.push_back(x); }
static void conv_String_IntVector(EConvStrength cs, const std::wstring& x, std::vector<meta::s_ll>& retval) {  retval.clear(); meta::s_ll temp; conv_String_Int(cs, x, temp); retval.push_back(temp); }
static void conv_String_FloatVector(EConvStrength cs, const std::wstring& x, std::vector<double>& retval) {  retval.clear(); double y; conv_String_Float(cs, x, y); retval.push_back(y); }
static void conv_String_DateVector(EConvStrength cs, const std::wstring& x, std::vector<_unitydate>& retval) {  retval.clear(); _unitydate y; conv_String_Date(cs, x, y); retval.push_back(y); }
static void conv_String_StringVector(EConvStrength cs, const std::wstring& x, std::vector<std::wstring>& retval) {  retval.clear(); retval.push_back(x); }
static void conv_String_CharVector(EConvStrength cs, const std::wstring& x, std::vector<_unitychar>& retval) {  retval.clear(); _unitychar b; conv_String_Char(cs, x, b); retval.push_back(b); }

//========================================================================

static void conv_Object_Int(EConvStrength cs, const o_handler_base& x, meta::s_ll& retval) {  char* p0=0; retval=(char*)x.pvoid(false)-p0; }
static void conv_Object_Float(EConvStrength cs, const o_handler_base& x, double& retval) {  char* p0=0; retval=double(meta::s_ll((char*)x.pvoid(false)-p0)); }
static void conv_Object_Date(EConvStrength cs, const o_handler_base& x, _unitydate& retval) {  retval.set(0); }
static void conv_Object_String(EConvStrength cs, const o_handler_base& x, std::wstring& retval)
{
    std::wostringstream oss;
    oss << x.objStatTypeInfo().name() << L':' << ( x.objStrongRefsCount() == 0 ? L'D' : (x.objIsStrongRef() ? L'S' : L'W') ) << L'-' << x.pvoid(false) ;
    retval = oss.str();
}
static void conv_Object_Char(EConvStrength cs, const o_handler_base& x, _unitychar& retval) {  retval=bool(x.pvoid(true)); }
static void conv_Object_Unity(EConvStrength cs, const o_handler_base& x, unity& retval) { retval.clear(); retval._data.p1 = x.clone(); retval.ut = utObject | unity::xfPtr; }

static void conv_Object_IntArray(EConvStrength cs, const o_handler_base& x, vec2_t<meta::s_ll>& retval) { meta::s_ll temp; conv_Object_Int(cs,x,temp); retval.clear(); retval.push_back(temp); }

static void conv_Object_FloatArray(EConvStrength cs, const o_handler_base& x, vec2_t<double>& retval) {  double temp; conv_Object_Float(cs,x,temp); retval.clear(); retval.push_back(temp); }
static void conv_Object_DateArray(EConvStrength cs, const o_handler_base& x, vec2_t<_unitydate>& retval) {  retval.clear(); retval.push_back(_unitydate(0.)); }
static void conv_Object_StringArray(EConvStrength cs, const o_handler_base& x, vec2_t<std::wstring>& retval) { std::wstring temp; conv_Object_String(cs,x,temp); retval.clear(); retval.push_back(temp); }
static void conv_Object_CharArray(EConvStrength cs, const o_handler_base& x, vec2_t<_unitychar>& retval) { _unitychar temp; conv_Object_Char(cs,x,temp); retval.clear(); retval.push_back(temp); }

static void conv_Object_IntVector(EConvStrength cs, const o_handler_base& x, std::vector<meta::s_ll>& retval) {  meta::s_ll temp; conv_Object_Int(cs,x,temp); retval.clear(); retval.push_back(temp); }
static void conv_Object_FloatVector(EConvStrength cs, const o_handler_base& x, std::vector<double>& retval) { double temp; conv_Object_Float(cs,x,temp); retval.clear(); retval.push_back(temp); }
static void conv_Object_DateVector(EConvStrength cs, const o_handler_base& x, std::vector<_unitydate>& retval) { _unitydate temp; conv_Object_Date(cs,x,temp); retval.clear(); retval.push_back(temp); }
static void conv_Object_StringVector(EConvStrength cs, const o_handler_base& x, std::vector<std::wstring>& retval) {   std::wstring temp; conv_Object_String(cs,x,temp); retval.clear(); retval.push_back(temp); }
static void conv_Object_CharVector(EConvStrength cs, const o_handler_base& x, std::vector<_unitychar>& retval) {   _unitychar temp; conv_Object_Char(cs,x,temp); retval.clear(); retval.push_back(temp); }

static void conv_Unity_Int(EConvStrength cs, const unity& x, meta::s_ll& retval) { cnv_all(cs, x.utype(), utInt, 0, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
static void conv_Unity_Float(EConvStrength cs, const unity& x, double& retval) { cnv_all(cs, x.utype(), utFloat, 0, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
static void conv_Unity_Date(EConvStrength cs, const unity& x, _unitydate& retval) { cnv_all(cs, x.utype(), utDate, 0, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
static void conv_Unity_String(EConvStrength cs, const unity& x, std::wstring& retval)
{
  if (x.isAssoc()) { std::wostringstream st; st << x; retval = st.str(); return; }
  cnv_all(cs, x.utype(), utString, 0, x.isByPtr() ? x._data.p1 : &x._data, &retval);
}
static void conv_Unity_Char(EConvStrength cs, const unity& x, _unitychar& retval) { cnv_all(cs, x.utype(), utChar, 0, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
static void conv_Unity_Unity(EConvStrength cs, const unity& x, unity& retval) { retval = x; }

static void conv_Unity_UnityArray(EConvStrength cs, const unity& x, vec2_t<unity>& retval) { cnv_all(cs, x.utype(), utUnityArray, 0, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
static void conv_Unity_IntArray(EConvStrength cs, const unity& x, vec2_t<meta::s_ll>& retval) { cnv_all(cs, x.utype(), utIntArray, 0, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
static void conv_Unity_FloatArray(EConvStrength cs, const unity& x, vec2_t<double>& retval) { cnv_all(cs, x.utype(), utFloatArray, 0, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
static void conv_Unity_DateArray(EConvStrength cs, const unity& x, vec2_t<_unitydate>& retval) { cnv_all(cs, x.utype(), utDateArray, 0, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
static void conv_Unity_StringArray(EConvStrength cs, const unity& x, vec2_t<std::wstring>& retval) { cnv_all(cs, x.utype(), utStringArray, 0, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
static void conv_Unity_CharArray(EConvStrength cs, const unity& x, vec2_t<_unitychar>& retval) { cnv_all(cs, x.utype(), utCharArray, 0, x.isByPtr() ? x._data.p1 : &x._data, &retval); }

static void conv_Unity_UnityVector(EConvStrength cs, const unity& x, std::vector<unity>& retval) { cnv_all(cs, x.utype(), utUnityArray, 0x1, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
static void conv_Unity_IntVector(EConvStrength cs, const unity& x, std::vector<meta::s_ll>& retval) { cnv_all(cs, x.utype(), utIntArray, 0x1, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
static void conv_Unity_FloatVector(EConvStrength cs, const unity& x, std::vector<double>& retval) { cnv_all(cs, x.utype(), utFloatArray, 0x1, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
static void conv_Unity_DateVector(EConvStrength cs, const unity& x, std::vector<_unitydate>& retval) { cnv_all(cs, x.utype(), utDateArray, 0x1, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
static void conv_Unity_StringVector(EConvStrength cs, const unity& x, std::vector<std::wstring>& retval) { cnv_all(cs, x.utype(), utStringArray, 0x1, x.isByPtr() ? x._data.p1 : &x._data, &retval); }
static void conv_Unity_CharVector(EConvStrength cs, const unity& x, std::vector<_unitychar>& retval) { cnv_all(cs, x.utype(), utCharArray, 0x1, x.isByPtr() ? x._data.p1 : &x._data, &retval); }

//========================================================================

static void conv_Map_Map(EConvStrength cs, const unity::t_map& x, unity::t_map& retval)
  { retval.ordhs_set0_nexc(); retval = x; if (retval.nexc() == 0) { return; } if (cs != csLazy) { throw XUConvFailed("conv_Map_Map", "", ""); } retval.ordhs_clear(); }
static void conv_Map_Hash(EConvStrength cs, const unity::t_map& x, unity::t_hash& retval)
  { retval.hl_clear(true); bool b = retval.hl_set_kf(*x.pkf()); if (b) { try { for (s_long i = 0; i < x.n(); ++i) { const unity::t_map::entry* e = x(i); retval[e->k] = e->v; } } catch (...) { b = false; } } if (b) { return; } if (cs != csLazy) { throw XUConvFailed("conv_Map_Hash", "", ""); } retval.hl_clear(true); }
static void conv_Map_Unity(EConvStrength cs, const unity::t_map& x, unity& retval)
  {
    retval.clear(); unity::_storage st;
    if (cv_ff::cv_create::Lcreate(retval.pmsm, &st, utMap, 0x1) > 0)
      { retval._data = st; retval.ut = utMap | unity::xfPtr; conv_Map_Map(cs, x, *retval._m()); return; }
    if (cs != csLazy) { throw XUConvFailed("conv_Map_Unity", "", ""); }
    retval.clear();
  }

static void conv_Hash_Map(EConvStrength cs, const unity::t_hash& x, unity::t_map& retval)
  { retval.ordhs_clear(); bool b = retval.ordhs_set_kf(*x.pkf()); if (b) { try { for (s_long i = 0; i < x.n(); ++i) { const unity::t_hash::entry* e = x(i); retval[e->k] = e->v; } } catch (...) { b = false; } } if (b) { return; } if (cs != csLazy) { throw XUConvFailed("conv_Hash_Map", "", ""); } retval.ordhs_clear(); }
static void conv_Hash_Hash(EConvStrength cs, const unity::t_hash& x, unity::t_hash& retval)
  { try { retval = x; return; } catch (...) {} if (cs != csLazy) { throw XUConvFailed("conv_Hash_Hash", "", ""); } retval.hl_clear(true); }
static void conv_Hash_Unity(EConvStrength cs, const unity::t_hash& x, unity& retval)
  {
    retval.clear(); unity::_storage st;
    if (cv_ff::cv_create::Lcreate(retval.pmsm, &st, utHash, 0x1) > 0)
      { retval._data = st; retval.ut = utHash | unity::xfPtr; conv_Hash_Hash(cs, x, *retval._h()); return; }
    if (cs != csLazy) { throw XUConvFailed("conv_Hash_Unity", "", ""); }
    retval.clear();
  }

//========================================================================

static void conv_UnityArray_Int(EConvStrength cs, const vec2_t<unity>& x, meta::s_ll& retval) {  if (x.n() > 0) { retval = x.pval_0u(0)->val<utInt>(cs); } else { retval = 0; } }
static void conv_UnityArray_Float(EConvStrength cs, const vec2_t<unity>& x, double& retval) {  if (x.n() > 0) { retval = x.pval_0u(0)->val<utFloat>(cs); } else { retval = 0; } }
static void conv_UnityArray_Date(EConvStrength cs, const vec2_t<unity>& x, _unitydate& retval) {  if (x.n() > 0) { retval = x.pval_0u(0)->val<utDate>(cs); } else { retval.set(0.); } }
static void conv_UnityArray_String(EConvStrength cs, const vec2_t<unity>& x, std::wstring& retval)
{
    std::wstring s = L".Unity" + bsToWs(cv_ff::vecm_strdim(x.rvecm()));
    if (x.n() > 0) { s += L":"; s += wLF; }
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { s += ntows(ind); s += L' '; s += x.pc(ind)->val<utString>(cs); s += wLF; }
    retval=s;
}
static void conv_UnityArray_Char(EConvStrength cs, const vec2_t<unity>& x, _unitychar& retval) {  if (x.n() > 0) { retval = x.pval_0u(0)->val<utChar>(cs); } else { retval = 0; } }
static void conv_UnityArray_Unity(EConvStrength cs, const vec2_t<unity>& x, unity& retval) { retval = x; }

static void conv_UnityArray_UnityArray(EConvStrength cs, const vec2_t<unity>& x, vec2_t<unity>& retval) { retval = x; }
static void conv_UnityArray_IntArray(EConvStrength cs, const vec2_t<unity>& x, vec2_t<meta::s_ll>& retval)
{
    retval.clear();
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(x.pc(ind)->val<utInt>(cs)); }
    retval.vec2_set_nbase(x.nbase());
}
static void conv_UnityArray_FloatArray(EConvStrength cs, const vec2_t<unity>& x, vec2_t<double>& retval)
{
    retval.clear();
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(x.pc(ind)->val<utFloat>(cs)); }
    retval.vec2_set_nbase(x.nbase());
}
static void conv_UnityArray_DateArray(EConvStrength cs, const vec2_t<unity>& x, vec2_t<_unitydate>& retval)
{
    retval.clear();
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(x.pc(ind)->val<utDate>(cs)); }
    retval.vec2_set_nbase(x.nbase());
}
static void conv_UnityArray_StringArray(EConvStrength cs, const vec2_t<unity>& x, vec2_t<std::wstring>& retval)
{
    retval.clear();
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(x.pc(ind)->val<utString>(cs)); }
    retval.vec2_set_nbase(x.nbase());
}
static void conv_UnityArray_CharArray(EConvStrength cs, const vec2_t<unity>& x, vec2_t<_unitychar>& retval)
{
    retval.clear();
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(x.pc(ind)->val<utChar>(cs)); }
    retval.vec2_set_nbase(x.nbase());
}
static void conv_UnityArray_UnityVector(EConvStrength cs, const vec2_t<unity>& x, std::vector<unity>& retval)
{
  retval.clear();
  retval.reserve(x.n());
  for (s_long i = 0; i < x.n(); ++i) { retval.push_back(*x.pval(i)); }
}
static void conv_UnityArray_IntVector(EConvStrength cs, const vec2_t<unity>& x, std::vector<meta::s_ll>& retval)
{
    retval.clear();
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(x.pc(ind)->val<utInt>(cs)); }
}
static void conv_UnityArray_FloatVector(EConvStrength cs, const vec2_t<unity>& x, std::vector<double>& retval)
{
    retval.clear();
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(x.pc(ind)->val<utFloat>(cs)); }
}
static void conv_UnityArray_DateVector(EConvStrength cs, const vec2_t<unity>& x, std::vector<_unitydate>& retval)
{
    retval.clear();
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(x.pc(ind)->val<utDate>(cs)); }
}
static void conv_UnityArray_StringVector(EConvStrength cs, const vec2_t<unity>& x, std::vector<std::wstring>& retval)
{
    retval.clear();
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(x.pc(ind)->val<utString>(cs)); }
}
static void conv_UnityArray_CharVector(EConvStrength cs, const vec2_t<unity>& x, std::vector<_unitychar>& retval)
{
    retval.clear();
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(x.pc(ind)->val<utChar>(cs)); }
}

//========================================================================

static void conv_IntArray_Int(EConvStrength cs, const vec2_t<meta::s_ll>& x, meta::s_ll& retval) {  if (x.n() > 0) { retval = *x.pval_0u(0); } else { retval = 0; } }
static void conv_IntArray_Float(EConvStrength cs, const vec2_t<meta::s_ll>& x, double& retval) {  if (x.n() > 0) { conv_Int_Float(cs, *x.pval_0u(0), retval); } else { retval = 0; } }
static void conv_IntArray_Date(EConvStrength cs, const vec2_t<meta::s_ll>& x, _unitydate& retval) {  if (x.n() > 0) { conv_Int_Date(cs, *x.pval_0u(0), retval); } else { retval.set(0.); } }
static void conv_IntArray_String(EConvStrength cs, const vec2_t<meta::s_ll>& x, std::wstring& retval)
{
    std::wstring s = L".Int" + bsToWs(cv_ff::vecm_strdim(x.rvecm()));
    if (x.n() > 0) { s += wLF; }
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { s += ntows(ind); s += L' '; s += ntows(*x.pc(ind)); s += wLF; }
    retval = s;
}
static void conv_IntArray_Char(EConvStrength cs, const vec2_t<meta::s_ll>& x, _unitychar& retval) {  if (x.n() > 0) { conv_Int_Char(cs, *x.pval_0u(0), retval); } else { retval = 0; } }
static void conv_IntArray_Unity(EConvStrength cs, const vec2_t<meta::s_ll>& x, unity& retval) { retval = x; }

static void conv_IntArray_UnityArray(EConvStrength cs, const vec2_t<meta::s_ll>& x, vec2_t<unity>& retval)
{
    retval.clear();
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(*x.pc(ind)); }
    retval.vec2_set_nbase(x.nbase());
}
static void conv_IntArray_IntArray(EConvStrength cs, const vec2_t<meta::s_ll>& x, vec2_t<meta::s_ll>& retval) { retval = x; }
static void conv_IntArray_FloatArray(EConvStrength cs, const vec2_t<meta::s_ll>& x, vec2_t<double>& retval)
{
    retval.clear();
    double temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Int_Float(cs, *x.pc(ind),temp); retval.push_back(temp); }
    retval.vec2_set_nbase(x.nbase());
}
static void conv_IntArray_DateArray(EConvStrength cs, const vec2_t<meta::s_ll>& x, vec2_t<_unitydate>& retval)
{
    retval.clear();
    _unitydate temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Int_Date(cs, *x.pc(ind), temp); retval.push_back(temp); }
    retval.vec2_set_nbase(x.nbase());
}
static void conv_IntArray_StringArray(EConvStrength cs, const vec2_t<meta::s_ll>& x, vec2_t<std::wstring>& retval)
{
    retval.clear();
    std::wstring temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Int_String(cs, *x.pc(ind), temp); retval.push_back(temp); }
    retval.vec2_set_nbase(x.nbase());
}
static void conv_IntArray_CharArray(EConvStrength cs, const vec2_t<meta::s_ll>& x, vec2_t<_unitychar>& retval)
{
    retval.clear(); _unitychar temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Int_Char(cs, *x.pc(ind), temp); retval.push_back(temp); }
    retval.vec2_set_nbase(x.nbase());
}

static void conv_IntArray_UnityVector(EConvStrength cs, const vec2_t<meta::s_ll>& x, std::vector<unity>& retval)
{
    retval.clear();
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(*x.pc(ind)); }
}
static void conv_IntArray_IntVector(EConvStrength cs, const vec2_t<meta::s_ll>& x, std::vector<meta::s_ll>& retval)
{
  retval.clear(); retval.reserve(x.n());
  for (s_long i = 0; i < x.n(); ++i) { retval.push_back(*x.pval(i)); }
}
static void conv_IntArray_FloatVector(EConvStrength cs, const vec2_t<meta::s_ll>& x, std::vector<double>& retval)
{
    retval.clear(); double temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Int_Float(cs, *x.pc(ind),temp); retval.push_back(temp); }
}
static void conv_IntArray_DateVector(EConvStrength cs, const vec2_t<meta::s_ll>& x, std::vector<_unitydate>& retval)
{
    retval.clear(); _unitydate temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Int_Date(cs, *x.pc(ind),temp); retval.push_back(temp); }
}
static void conv_IntArray_StringVector(EConvStrength cs, const vec2_t<meta::s_ll>& x, std::vector<std::wstring>& retval)
{

    retval.clear(); std::wstring temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Int_String(cs, *x.pc(ind),temp); retval.push_back(temp); }
}
static void conv_IntArray_CharVector(EConvStrength cs, const vec2_t<meta::s_ll>& x, std::vector<_unitychar>& retval)
{
    retval.clear(); _unitychar temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Int_Char(cs, *x.pc(ind),temp); retval.push_back(temp); }
}

//========================================================================

static void conv_FloatArray_Int(EConvStrength cs, const vec2_t<double>& x, meta::s_ll& retval) { if (x.n() > 0) { conv_Float_Int(cs, *x.pval_0u(0), retval); } else { retval = 0; } }
static void conv_FloatArray_Float(EConvStrength cs, const vec2_t<double>& x, double& retval) { if (x.n() > 0) { retval = *x.pval_0u(0); } else { retval = 0; } }
static void conv_FloatArray_Date(EConvStrength cs, const vec2_t<double>& x, _unitydate& retval) {  if (x.n() > 0) { conv_Float_Date(cs, *x.pval_0u(0),retval); } else { retval.set(0.); } }
static void conv_FloatArray_String(EConvStrength cs, const vec2_t<double>& x, std::wstring& retval)
{
    std::wstring s = L".Float" + bsToWs(cv_ff::vecm_strdim(x.rvecm()));
    if (x.n() > 0) { s += wLF; }
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { s += ntows(ind); s += L' '; s += ntows(*x.pc(ind)); s += wLF; }
    retval = s;
}
static void conv_FloatArray_Char(EConvStrength cs, const vec2_t<double>& x, _unitychar& retval) {  if (x.n() > 0) { conv_Float_Char(cs, *x.pval_0u(0), retval); } else { retval = 0; } }
static void conv_FloatArray_Unity(EConvStrength cs, const vec2_t<double>& x, unity& retval) { retval = x; }

static void conv_FloatArray_UnityArray(EConvStrength cs, const vec2_t<double>& x, vec2_t<unity>& retval)
{
    retval.clear();
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(*x.pc(ind)); }
    retval.vec2_set_nbase(x.nbase());
}
static void conv_FloatArray_IntArray(EConvStrength cs, const vec2_t<double>& x, vec2_t<meta::s_ll>& retval)
{
    retval.clear();
    meta::s_ll temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Float_Int(cs, *x.pc(ind), temp); retval.push_back(temp); }
    retval.vec2_set_nbase(x.nbase());
}
static void conv_FloatArray_FloatArray(EConvStrength cs, const vec2_t<double>& x, vec2_t<double>& retval) { retval = x; }
static void conv_FloatArray_DateArray(EConvStrength cs, const vec2_t<double>& x, vec2_t<_unitydate>& retval)
{
    retval.clear();
    _unitydate temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Float_Date(cs, *x.pc(ind), temp); retval.push_back(temp); }
    retval.vec2_set_nbase(x.nbase());
}
static void conv_FloatArray_StringArray(EConvStrength cs, const vec2_t<double>& x, vec2_t<std::wstring>& retval)
{
    retval.clear();
    std::wstring temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Float_String(cs, *x.pc(ind), temp); retval.push_back(temp); }
    retval.vec2_set_nbase(x.nbase());
}
static void conv_FloatArray_CharArray(EConvStrength cs, const vec2_t<double>& x, vec2_t<_unitychar>& retval)
{
    retval.clear();
    _unitychar temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Float_Char(cs, *x.pc(ind), temp); retval.push_back(temp); }
    retval.vec2_set_nbase(x.nbase());
}

static void conv_FloatArray_UnityVector(EConvStrength cs, const vec2_t<double>& x, std::vector<unity>& retval)
{
    retval.clear();
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(*x.pc(ind)); }
}
static void conv_FloatArray_IntVector(EConvStrength cs, const vec2_t<double>& x, std::vector<meta::s_ll>& retval)
{
    retval.clear();
    meta::s_ll temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Float_Int(cs, *x.pc(ind),temp); retval.push_back(temp); }
}
static void conv_FloatArray_FloatVector(EConvStrength cs, const vec2_t<double>& x, std::vector<double>& retval)
{
    retval.clear(); retval.reserve(x.n());
    for (s_long i = 0; i < x.n(); ++i) { retval.push_back(*x.pval(i)); }
}
static void conv_FloatArray_DateVector(EConvStrength cs, const vec2_t<double>& x, std::vector<_unitydate>& retval)
{
    retval.clear(); _unitydate temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Float_Date(cs, *x.pc(ind), temp); retval.push_back(temp); }
}
static void conv_FloatArray_StringVector(EConvStrength cs, const vec2_t<double>& x, std::vector<std::wstring>& retval)
{
    retval.clear(); std::wstring temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Float_String(cs, *x.pc(ind),temp); retval.push_back(temp); }
}
static void conv_FloatArray_CharVector(EConvStrength cs, const vec2_t<double>& x, std::vector<_unitychar>& retval)
{
    retval.clear(); _unitychar temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Float_Char(cs, *x.pc(ind),temp); retval.push_back(temp); }
}

//========================================================================

static void conv_CharArray_Int(EConvStrength cs, const vec2_t<_unitychar>& x, meta::s_ll& retval) {  if (x.n() > 0) { conv_Char_Int(cs, *x.pval_0u(0), retval); } else { retval = 0; } }
static void conv_CharArray_Float(EConvStrength cs, const vec2_t<_unitychar>& x, double& retval) {  if (x.n() > 0) { conv_Char_Float(cs, *x.pval_0u(0),retval); } else { retval = 0; } }
static void conv_CharArray_Date(EConvStrength cs, const vec2_t<_unitychar>& x, _unitydate& retval) {  if (x.n() > 0) { conv_Char_Date(cs, *x.pval_0u(0), retval); } else { retval.set(0.); } }
static void conv_CharArray_String(EConvStrength cs, const vec2_t<_unitychar>& x, std::wstring& retval)
{
    std::wstring s = L".Char" + bsToWs(cv_ff::vecm_strdim(x.rvecm()));
    if (x.n() > 0) { s += wLF; }
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { s += ntows(ind); s += L' '; s += *x.pc(ind) ? L'1': L'0'; s += wLF; }
    retval = s;
}
static void conv_CharArray_Char(EConvStrength cs, const vec2_t<_unitychar>& x, _unitychar& retval) {  if (x.n() > 0) { retval = *x.pval_0u(0); } else { retval = 0; } }
static void conv_CharArray_Unity(EConvStrength cs, const vec2_t<_unitychar>& x, unity& retval) { retval = x; }

static void conv_CharArray_UnityArray(EConvStrength cs, const vec2_t<_unitychar>& x, vec2_t<unity>& retval)
{
    retval.clear();
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(*x.pc(ind)); }
    retval.vec2_set_nbase(x.nbase());
}
static void conv_CharArray_IntArray(EConvStrength cs, const vec2_t<_unitychar>& x, vec2_t<meta::s_ll>& retval)
{
    retval.clear(); meta::s_ll temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Char_Int(cs,*x.pc(ind),temp); retval.push_back(temp); }
    retval.vec2_set_nbase(x.nbase());
}
static void conv_CharArray_DateArray(EConvStrength cs, const vec2_t<_unitychar>& x, vec2_t<_unitydate>& retval)
{
    retval.clear(); _unitydate temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Char_Date(cs,*x.pc(ind), temp); retval.push_back(temp); }
    retval.vec2_set_nbase(x.nbase());
}
static void conv_CharArray_FloatArray(EConvStrength cs, const vec2_t<_unitychar>& x, vec2_t<double>& retval)
{
    retval.clear(); double temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Char_Float(cs,*x.pc(ind),temp); retval.push_back(temp); }
    retval.vec2_set_nbase(x.nbase());
}
static void conv_CharArray_StringArray(EConvStrength cs, const vec2_t<_unitychar>& x, vec2_t<std::wstring>& retval)
{
    retval.clear(); std::wstring temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Char_String(cs,*x.pc(ind),temp); retval.push_back(temp); }
    retval.vec2_set_nbase(x.nbase());
}
static void conv_CharArray_CharArray(EConvStrength cs, const vec2_t<_unitychar>& x, vec2_t<_unitychar>& retval) { retval = x; }

static void conv_CharArray_UnityVector(EConvStrength cs, const vec2_t<_unitychar>& x, std::vector<unity>& retval)
{
    retval.clear();
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(*x.pc(ind)); }
}
static void conv_CharArray_IntVector(EConvStrength cs, const vec2_t<_unitychar>& x, std::vector<meta::s_ll>& retval)
{
    retval.clear(); meta::s_ll temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Char_Int(cs,*x.pc(ind),temp); retval.push_back(temp); }
}
static void conv_CharArray_FloatVector(EConvStrength cs, const vec2_t<_unitychar>& x, std::vector<double>& retval)
{
    retval.clear(); double temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Char_Float(cs, *x.pc(ind),temp); retval.push_back(temp); }
}
static void conv_CharArray_DateVector(EConvStrength cs, const vec2_t<_unitychar>& x, std::vector<_unitydate>& retval)
{
    retval.clear(); _unitydate temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Char_Date(cs,*x.pc(ind),temp); retval.push_back(temp); }
}
static void conv_CharArray_StringVector(EConvStrength cs, const vec2_t<_unitychar>& x, std::vector<std::wstring>& retval)
{
    retval.clear(); retval.clear(); std::wstring temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Char_String(cs,*x.pc(ind),temp); retval.push_back(temp); }
}
static void conv_CharArray_CharVector(EConvStrength cs, const vec2_t<_unitychar>& x, std::vector<_unitychar>& retval)
{
    retval.clear(); retval.reserve(x.n());
    for (s_long i = 0; i < x.n(); ++i) { retval.push_back(*x.pval(i)); }
}

//========================================================================

static void conv_DateArray_Int(EConvStrength cs, const vec2_t<_unitydate>& x, meta::s_ll& retval) {  if (x.n() > 0) { conv_Date_Int(cs, *x.pval_0u(0), retval); } else { retval = 0; } }
static void conv_DateArray_Float(EConvStrength cs, const vec2_t<_unitydate>& x, double& retval) {  if (x.n() > 0) { conv_Date_Float(cs, *x.pval_0u(0), retval); } else { retval = 0; } }
static void conv_DateArray_Date(EConvStrength cs, const vec2_t<_unitydate>& x, _unitydate& retval) {  if (x.n() > 0) { retval = *x.pval_0u(0); } else { retval = _unitydate(0); } }
static void conv_DateArray_String(EConvStrength cs, const vec2_t<_unitydate>& x, std::wstring& retval)
{
    std::wstring s = L".Date" + bsToWs(cv_ff::vecm_strdim(x.rvecm())); std::wstring s2;
    if (x.n() > 0) { s += L":"; s += wLF; }
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { s += ntows(ind); s += L' '; conv_Date_String(cs, *x.pc(ind), s2); s += s2; s += wLF; }
    retval=s;
}
static void conv_DateArray_Char(EConvStrength cs, const vec2_t<_unitydate>& x, _unitychar& retval) { if (x.n() > 0) { conv_Date_Char(cs, *x.pval_0u(0), retval); } else { retval = 0; } }
static void conv_DateArray_Unity(EConvStrength cs, const vec2_t<_unitydate>& x, unity& retval) { retval = x; }

static void conv_DateArray_UnityArray(EConvStrength cs, const vec2_t<_unitydate>& x, vec2_t<unity>& retval)
{
    retval.clear();
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(*x.pc(ind)); }
    retval.vec2_set_nbase(x.nbase());
}
static void conv_DateArray_IntArray(EConvStrength cs, const vec2_t<_unitydate>& x, vec2_t<meta::s_ll>& retval)
{
    retval.clear(); meta::s_ll temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Date_Int(cs, *x.pc(ind), temp); retval.push_back(temp); }
    retval.vec2_set_nbase(x.nbase());
}
static void conv_DateArray_FloatArray(EConvStrength cs, const vec2_t<_unitydate>& x, vec2_t<double>& retval)
{
    retval.clear(); retval.clear(); double temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Date_Float(cs, *x.pc(ind), temp); retval.push_back(temp); }
    retval.vec2_set_nbase(x.nbase());
}
static void conv_DateArray_DateArray(EConvStrength cs, const vec2_t<_unitydate>& x, vec2_t<_unitydate>& retval) { retval = x; }
static void conv_DateArray_StringArray(EConvStrength cs, const vec2_t<_unitydate>& x, vec2_t<std::wstring>& retval)
{
    retval.clear(); std::wstring temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Date_String(cs, *x.pc(ind), temp); retval.push_back(temp); }
    retval.vec2_set_nbase(x.nbase());
}
static void conv_DateArray_CharArray(EConvStrength cs, const vec2_t<_unitydate>& x, vec2_t<_unitychar>& retval)
{
    retval.clear(); _unitychar temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Date_Char(cs, *x.pc(ind),temp); retval.push_back(temp); }
    retval.vec2_set_nbase(x.nbase());
}

static void conv_DateArray_UnityVector(EConvStrength cs, const vec2_t<_unitydate>& x, std::vector<unity>& retval)
{
    retval.clear();
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(*x.pc(ind)); }
}
static void conv_DateArray_IntVector(EConvStrength cs, const vec2_t<_unitydate>& x, std::vector<meta::s_ll>& retval)
{
    retval.clear(); meta::s_ll temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Date_Int(cs, *x.pc(ind),temp); retval.push_back(temp); }
}
static void conv_DateArray_FloatVector(EConvStrength cs, const vec2_t<_unitydate>& x, std::vector<double>& retval)
{
    retval.clear(); double temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Date_Float(cs, *x.pc(ind),temp); retval.push_back(temp); }
}
static void conv_DateArray_DateVector(EConvStrength cs, const vec2_t<_unitydate>& x, std::vector<_unitydate>& retval)
{
    retval.clear(); retval.reserve(x.n());
    for (s_long i = 0; i < x.n(); ++i) { retval.push_back(*x.pval(i)); }
}
static void conv_DateArray_StringVector(EConvStrength cs, const vec2_t<_unitydate>& x, std::vector<std::wstring>& retval)
{
    retval.clear(); std::wstring temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Date_String(cs, *x.pc(ind),temp); retval.push_back(temp); }
}
static void conv_DateArray_CharVector(EConvStrength cs, const vec2_t<_unitydate>& x, std::vector<_unitychar>& retval)
{
    retval.clear(); _unitychar temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_Date_Char(cs, *x.pc(ind),temp); retval.push_back(temp); }
}

//========================================================================

static void conv_StringArray_Int(EConvStrength cs, const vec2_t<std::wstring>& x, meta::s_ll& retval) {  if (x.n() > 0) { conv_String_Int(cs, *x.pval_0u(0),retval); } else { retval = 0; } }
static void conv_StringArray_Float(EConvStrength cs, const vec2_t<std::wstring>& x, double& retval) {  if (x.n() > 0) { conv_String_Float(cs, *x.pval_0u(0),retval); } else { retval = 0; } }
static void conv_StringArray_Date(EConvStrength cs, const vec2_t<std::wstring>& x, _unitydate& retval) {  if (x.n() > 0) { conv_String_Date(cs, *x.pval_0u(0),retval); } else { retval.set(0.); } }
static void conv_StringArray_String(EConvStrength cs, const vec2_t<std::wstring>& x, std::wstring& retval)
{
    std::wstring s = L".String" + bsToWs(cv_ff::vecm_strdim(x.rvecm()));
    if (x.n() > 0) { s += L":"; s += wLF; }
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { s += ntows(ind); s += L' '; s += *x.pc(ind); s += wLF; }
    retval=s;
}
static void conv_StringArray_Char(EConvStrength cs, const vec2_t<std::wstring>& x, _unitychar& retval) {  if (x.n() > 0) { conv_String_Char(cs, *x.pval_0u(0), retval); } else { retval = 0; } }
static void conv_StringArray_Unity(EConvStrength cs, const vec2_t<std::wstring>& x, unity& retval) { retval = x; }

static void conv_StringArray_UnityArray(EConvStrength cs, const vec2_t<std::wstring>& x, vec2_t<unity>& retval)
{
    retval.clear();
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(*x.pc(ind)); }
    retval.vec2_set_nbase(x.nbase());
}
static void conv_StringArray_IntArray(EConvStrength cs, const vec2_t<std::wstring>& x, vec2_t<meta::s_ll>& retval)
{
    retval.clear(); meta::s_ll temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_String_Int(cs, *x.pc(ind),temp); retval.push_back(temp); }
    retval.vec2_set_nbase(x.nbase());
}
static void conv_StringArray_FloatArray(EConvStrength cs, const vec2_t<std::wstring>& x, vec2_t<double>& retval)
{
    retval.clear(); double temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_String_Float(cs, *x.pc(ind),temp); retval.push_back(temp); }
    retval.vec2_set_nbase(x.nbase());
}
static void conv_StringArray_DateArray(EConvStrength cs, const vec2_t<std::wstring>& x, vec2_t<_unitydate>& retval)
{
    retval.clear(); _unitydate temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_String_Date(cs, *x.pc(ind), temp); retval.push_back(temp); }
    retval.vec2_set_nbase(x.nbase());
}
static void conv_StringArray_StringArray(EConvStrength cs, const vec2_t<std::wstring>& x, vec2_t<std::wstring>& retval) { retval = x; }
static void conv_StringArray_CharArray(EConvStrength cs, const vec2_t<std::wstring>& x, vec2_t<_unitychar>& retval)
{
    retval.clear(); _unitychar temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_String_Char(cs, *x.pc(ind),temp); retval.push_back(temp); }
    retval.vec2_set_nbase(x.nbase());
}

static void conv_StringArray_UnityVector(EConvStrength cs, const vec2_t<std::wstring>& x, std::vector<unity>& retval)
{
    retval.clear();
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { retval.push_back(*x.pc(ind)); }
}
static void conv_StringArray_IntVector(EConvStrength cs, const vec2_t<std::wstring>& x, std::vector<meta::s_ll>& retval)
{

    retval.clear(); meta::s_ll temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_String_Int(cs, *x.pc(ind),temp); retval.push_back(temp); }
}
static void conv_StringArray_FloatVector(EConvStrength cs, const vec2_t<std::wstring>& x, std::vector<double>& retval)
{

    retval.clear(); double temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_String_Float(cs, *x.pc(ind),temp); retval.push_back(temp); }
}
static void conv_StringArray_DateVector(EConvStrength cs, const vec2_t<std::wstring>& x, std::vector<_unitydate>& retval)
{

    retval.clear(); _unitydate temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_String_Date(cs, *x.pc(ind),temp); retval.push_back(temp); }
}
static void conv_StringArray_StringVector(EConvStrength cs, const vec2_t<std::wstring>& x, std::vector<std::wstring>& retval)
{
    retval.clear(); retval.reserve(x.n());
    for (s_long i = 0; i < x.n(); ++i) { retval.push_back(*x.pval(i)); }
}
static void conv_StringArray_CharVector(EConvStrength cs, const vec2_t<std::wstring>& x, std::vector<_unitychar>& retval)
{

    retval.clear(); _unitychar temp;
    const s_long _i2 = x.nbase() + x.n();  for (s_long ind = x.nbase(); ind < _i2; ++ind) { conv_String_Char(cs, *x.pc(ind),temp); retval.push_back(temp); }
}

};





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// unity impl.




void unity::_x_cnv_this(EConvStrength cs, s_long utt_dest, bool keep_name)  { if (utt_dest == utype()) { return; } cv_ff::cv_convert::Lcnv(this, cs, utt_dest, keep_name);  }
s_long cv_ff::cv_convert::Fcnv(unity* p, s_long cs, s_long utt_dest, s_long keep_name) throw()
{
  if (utt_dest == p->utype()) { return 0; }
  void* _pn = keep_name && p->isNamed() ? p->_data.p2 : 0;
  if (utt_dest == utEmpty)
  {
      if (_pn) { p->_data.p2 = 0; }
    p->clear();
      if (_pn) { p->_data.p2 = _pn; p->ut |= unity::xfPtr; }
    return 1;
  }
  if (cs == csStrict) { return -1; }
  unity::_storage st; s_long f_cr = _pn || utt_dest >= utString ? 0x1 : 0;
  s_long _step(0);
  try
  {
    if (cv_ff::cv_create::Fcreate(&st, utt_dest, f_cr) <= 0) { return -2; }
    _step = 1;
    StaticConv::cnv_all((EConvStrength)cs, p->utype(), utt_dest, 0, p->isByPtr() ? p->_data.p1 : &p->_data, (f_cr & 0x1) ? st.p1 : &st);
    _step = 2;
  }
  catch (_XUBase&) { if (cs != csLazy) { if (_step >= 1) { cv_ff::cv_create::Fdestroy(&st, utt_dest, f_cr); } return -3; } }
  catch (std::exception&) { if (cs != csLazy) { if (_step >= 1) { cv_ff::cv_create::Fdestroy(&st, utt_dest, f_cr); } return -4; } }
  catch (...) { if (cs != csLazy) { if (_step >= 1) { cv_ff::cv_create::Fdestroy(&st, utt_dest, f_cr); } return -5; } }

  if (_step == 2)
  {
    if (_pn) { st.p2 = _pn; p->_data.p2 = 0; }
    cv_ff::cv_create::Fdestroy(&p->_data, p->utype(), (p->isByPtr() ? 0x1 : 0) | 0x4);
    p->_data = st;
    p->ut = utt_dest; if (f_cr & 0x1) { p->ut |= unity::xfPtr; }
    return 1;
  }

  if (_step >= 1) { cv_ff::cv_create::Fdestroy(&st, utt_dest, f_cr); }
  if (_pn) { p->_data.p2 = 0; }
  cv_ff::cv_create::Fdestroy(&p->_data, p->utype(), (p->isByPtr() ? 0x1 : 0) | 0x4);
  p->ut = utEmpty; if (_pn) { p->ut |= unity::xfPtr; p->_data.p2 = _pn; }
  return -6;
}
void unity::_x_asg(const unity& x, s_long flags) { cv_ff::cv_convert::Lasg(this, &x, flags); }
s_long cv_ff::cv_convert::Fasg(unity* p, const unity* px, s_long flags) throw()
{
  if (px == p) { return 0; }
  if ((flags & 0x1) && p->isNamed()) { flags |= 0x100; }
  if (!(flags & 0x1) && px->isNamed()) { flags |= 0x200; }
  if (px->utype() < utString && !(flags & 0x302) && p->isScalar())
  {
    // Assign -- 1
    p->clear(); bytes::memmove_t<char>::F(&p->_data, px->isByPtr() ? px->_data.p1 : &px->_data, unity::_def_size(px->utype())); p->ut = px->utype();
    return 1;
  }
  try
  {
    void* _pn1 ((flags & 0x100) ? p->_data.p2 : 0);
    cv_ff::_pdyn _pn2 ((flags & 0x200) ? new unity(*px->_drf_name()) : 0);
    switch (px->utype())
    {
      case utEmpty:
        {
            if (_pn1) { p->_data.p2 = 0; }
          p->clear(); p->ut = utEmpty;
            if (_pn1) { p->_data.p2 = _pn1; p->ut |= unity::xfPtr; } else if (_pn2) { p->_data.p2 = _pn2.take(); p->ut |= unity::xfPtr; }
          return 1;
        }
      case utString:
        {
            // NOTE pss may be incompatible with current module.
            // It's not called directly, only through cv_std::wstring::L* wrappers.
          std::wstring* pss = unity::reference_t<std::wstring>::deref(px->_data, true);
          std::wstring* next = cv_ff::cv_wstring::Fnew(cv_ff::cv_wstring::Ln(px->pmsm, pss), cv_ff::cv_wstring::Lp0(px->pmsm, pss), cv_ff::cv_wstring::Lwcs(px->pmsm), 0);
          if (!next) { return -2; }
          // Assign -- 2
            if (_pn1) { p->_data.p2 = 0; }
          p->clear(); p->_data.p1 = next; p->ut = utString | unity::xfPtr;
            if (_pn1) { p->_data.p2 = _pn1; } else if (_pn2) { p->_data.p2 = _pn2.take(); }
          return 1;
        }
      case utObject:
        {
            // Handled object of type unity is copied by value.
            //  Other types of object are copied by reference (currently, only local object ref. copying is supported).
          unity* pu = ((unity_common::__Punity)px->pmsm(unity_common::msm_obj_punity))(px->_drf_o());
          if (pu)
          {
            unity* pu2(0); o_handler_base* ph(0);
            try { pu2 = new unity(*pu); if (pu2) { ph = new unity::objhandler<unity, o_interfaces_top<unity> >(*pu2, true); } } catch (...) {}
            if (!pu2) { return -12; }
            if (!ph) { if (pu2) { try { delete pu2; } catch (...) {} } return -13; }
            // Assign -- 3
              if (_pn1) { p->_data.p2 = 0; }
            p->clear(); p->_data.p1 = ph; p->ut = utObject | unity::xfPtr;
              if (_pn1) { p->_data.p2 = _pn1; } else if (_pn2) { p->_data.p2 = _pn2.take(); }
            return 1;
          }
          else
          {
            o_handler_base* next = 0;
            if (px->pmsm != unity_common::ls_modsm) { return -9; }
            try { next = px->_drf_o()->clone(); } catch (...) { return -10; }
            if (!next) { return -4; }
            // Assign -- 4
              if (_pn1) { p->_data.p2 = 0; }
            p->clear(); p->_data.p1 = next; p->ut = utObject | unity::xfPtr;
              if (_pn1) { p->_data.p2 = _pn1; } else if (_pn2) { p->_data.p2 = _pn2.take(); }
            return 1;
          }
        }
      default:
        {
              // NOTE Array compatibility == 0 is acceptable as well as > 0, because the array is just copied to the same type.
          if (px->isArray()) { if (px->pmsm != unity_common::ls_modsm) { if (px->_drf_q()->compatibility() < 0) { return -5; } } }
            else { if (!px->_compat_chk()) { return -6; } }
          unity::_storage st; const volatile int f_cr = 0x1;
          if (cv_ff::cv_create::Fcreate(&st, px->utype(), f_cr) <= 0) { return -7; }
          bool b(false); try { StaticConv::cnv_all(csStrict, px->utype(), px->utype(), 0, px->isByPtr() ? px->_data.p1 : &px->_data, f_cr & 0x1 ? st.p1 : &st); b = true; } catch (...) {}
          if (!b) { cv_ff::cv_create::Fdestroy(&st, px->utype(), f_cr); return -8; }
          // Assign -- 5
            if (_pn1) { p->_data.p2 = 0; }
          p->clear(); p->_data = st; p->ut = px->utype(); if (f_cr & 0x1) { p->ut |= unity::xfPtr; }
            if (_pn1) { p->_data.p2 = _pn1; } else if (_pn2) { p->_data.p2 = _pn2.take(); }
          return 1;
        } // end default
    } // end switch
  } catch (...) {}
  return -1;
}
  // _cnv_val:
  // Returns false if, in csLazy, failed to copy, and left *pdest valid, but with undefined value.
bool unity::_x_cnv_val(EConvStrength cs, s_long utt_dest, s_long flags, void* pdest) const
{
  const s_long utt_src = utype();
  if (cs == csStrict && utt_dest != utt_src) { throw XUTypeMismatch("_cnv_val.1", _tname0(utt_src), _tname0(utt_dest)); }
  if (isScalar())
  {
    if (utt_src == utString)
    {
      if (pmsm != unity_common::ls_modsm)
      {
          // NOTE pss may be incompatible with current module.
          // It's not called directly, only through cv_std::wstring::L* wrappers.
        std::wstring* pss = reference_t<std::wstring>::deref(_data, true);
        if (utt_dest == utString)
        {
          std::wstring* psd = reinterpret_cast<std::wstring*>(pdest);
          if (cv_ff::cv_wstring::Fcopy(psd, cv_ff::cv_wstring::Ln(pmsm, pss), cv_ff::cv_wstring::Lp0(pmsm, pss), cv_ff::cv_wstring::Lwcs(pmsm), 0))
            { return true; }
        }
        else
        {
          std::wstring temp;
          if (cv_ff::cv_wstring::Fcopy(&temp, cv_ff::cv_wstring::Ln(pmsm, pss), cv_ff::cv_wstring::Lp0(pmsm, pss), cv_ff::cv_wstring::Lwcs(pmsm), 0))
            { try { StaticConv::cnv_all(cs, utString, utt_dest, flags, &temp, pdest); return true; } catch (...) {} }
        }
        if (cs == csLazy) { return false; }
        throw XUConvFailed("_cnv_val.2", _tname0(utt_src), _tname0(utt_dest));
      }
    }
  }
  else if (isArray())
  {
    if (pmsm != unity_common::ls_modsm)
    {
        // NOTE compatibility == 0 may be acceptable if the array is just copied to the same type.
        //    In other cases, compatibility > 0 is acceptable only.
      const s_long c = _drf_q()->compatibility();
      if (!(_data.p1 && (c > 0 || (c == 0 && utt_dest == utt_src && !(flags & 0x1))))) { if (cs == csLazy) { return false; } throw XUConvFailed("_cnv_val.3", _tname0(utt_src), _tname0(utt_dest)); }
    }
  }
  else
  {
    if (!_compat_chk()) { if (cs == csLazy) { return false; } throw XUConvFailed("_cnv_val.4", _tname0(utt_src), _tname0(utt_dest)); }
  }
  try { StaticConv::cnv_all(cs, utt_src, utt_dest, flags, isByPtr() ? _data.p1 : &_data, pdest); return true; }
    catch (...) { if (cs != csLazy) { throw; } }
  return false;
}
void unity::_xu_move_p1(unity& src)
{
  if (this == &src) { return; }
  if (pmsm == src.pmsm && src.isByPtr())
  {
    if (!isEmpty()) { u_clear(utEmpty); }
    this->_data.p1 = src._data.p1; this->ut = src.utype() | xfPtr;
    src._data.p1 = 0; src.ut = utEmpty; if (src._data.p2) { src.ut |= xfPtr; }
  }
  else { _x_asg(src, 0x1); }
}

s_long unity::_Lcreate(_storage* pdest, s_long utt, s_long flags) { return cv_ff::cv_create::Lcreate(pmsm, pdest, utt, flags); }
s_long unity::_Ldestroy(_storage* pdest, s_long utt, s_long flags) { return cv_ff::cv_create::Ldestroy(pmsm, pdest, utt, flags); }
s_long unity::_Lel_get(s_long ind, unity* pst) const { return cv_ff::cv_array::Lel_get(this, pst, ind); }
s_long unity::_Lel_set(s_long ind, const unity& x) { return cv_ff::cv_array::Lel_set(this, &x, ind); }
s_long unity::_Lel_append(const unity& x) { return cv_ff::cv_array::Lel_append(this, &x); }
s_long unity::_Lel_insrem(s_long ind, s_long m) { return cv_ff::cv_array::Lel_insrem(this, 0, ind, m); }
s_long unity::_Lel_insrem(s_long ind, s_long m, const unity& x) { return cv_ff::cv_array::Lel_insrem(this, &x, ind, m); }
s_long unity::_Lu_set_name(const unity* pn) { return cv_ff::u_set_name::L(this, pn); }
s_long unity::_Ldelete_this() { return cv_ff::cv_delete::L(pmsm, this, utUnity); }

  // flags: see unity::u_clear() for description.
s_long unity::_Lu_clear(s_long flags) { return cv_ff::u_clear::L(this, flags); }
s_long cv_ff::u_clear::F(unity* p, s_long flags)
{
  if (p->isEmpty()) { return 2; }
  else if (p->isScalar())
  {
    try { StaticConv::cnv_all(csLazy, utEmpty, p->utype(), 0, 0, p->isByPtr() ? p->_data.p1 : &p->_data); } catch (...) { return 0; }
    return 2;
  }
  else
  {
    switch (p->utype())
    {
      case utObject:
      {
          void* _pn = p->_data.p2; if (_pn) { p->_data.p2 = 0; }
        cv_ff::cv_create::Fdestroy(&p->_data, p->utype(), 0x1 | 0x4); p->ut = utEmpty;
          if (_pn) { p->_data.p2 = _pn; p->ut |= unity::xfPtr; }
        return 2;
      }
      case utIntArray: { enum { utt = utInt }; unity::valtype_t<utt | utArray>::t& r = p->ref<utt | utArray>(); if (flags & 0x1) { r.clear(); r.vec2_set_nbase(1); return 2; } return r.el_remove_all() >= 0 ? 1 : -1; }
      case utFloatArray: { enum { utt = utFloat }; unity::valtype_t<utt | utArray>::t& r = p->ref<utt | utArray>(); if (flags & 0x1) { r.clear(); r.vec2_set_nbase(1); return 2; } return r.el_remove_all() >= 0 ? 1 : -1; }
      case utDateArray: { enum { utt = utDate }; unity::valtype_t<utt | utArray>::t& r = p->ref<utt | utArray>(); if (flags & 0x1) { r.clear(); r.vec2_set_nbase(1); return 2; } return r.el_remove_all() >= 0 ? 1 : -1; }
      case utStringArray: { enum { utt = utString }; unity::valtype_t<utt | utArray>::t& r = p->ref<utt | utArray>(); if (flags & 0x1) { r.clear(); r.vec2_set_nbase(1); return 2; } return r.el_remove_all() >= 0 ? 1 : -1; }
      case utCharArray: { enum { utt = utChar }; unity::valtype_t<utt | utArray>::t& r = p->ref<utt | utArray>(); if (flags & 0x1) { r.clear(); r.vec2_set_nbase(1); return 2; } return r.el_remove_all() >= 0 ? 1 : -1; }
      case utUnityArray: { enum { utt = utUnity }; unity::valtype_t<utt | utArray>::t& r = p->ref<utt | utArray>(); if (flags & 0x1) { r.clear(); r.vec2_set_nbase(1); return 2; } return r.el_remove_all() >= 0 ? 1 : -1; }
      case utMap: { if (flags & 0x1) { p->_m()->ordhs_clear(); return 2; } return p->_m()->remove_all() >= 0 ? 1 : -1; }
      case utHash: { return p->_h()->hl_clear(bool(flags & 0x1)) >= 0 ? 1 : -1; }
      default: { return -1; }
    }
  }
}



unity* unity::_path_u(const std::wstring& keylist, bool forced) throw()
{
  try {
    std::wstring s2 = _trim(keylist, L" ", true, false); if (s2.length() == 0) { s2 = L"=|" + keylist; } else if (s2[0] == L'=') {} else if (s2[0] == L'|') { s2 = L"=" + keylist; } else { s2 = L"=|" + keylist; }
    unity k; k = paramline().decode(s2, false).hash(L"");
    if (!k.isArray()) { return 0; }
    unity* node = this;
    for (int j = k.arrlb(); j <= k.arrub(); ++j)
    {
      if (node->isHash())
      {
        if (node->hash_locate(k.ref<utUnity>(j)) || forced) { node = &node->hash(k.ref<utUnity>(j)); } else { return 0; }
        continue;
      }
      else if (node->isMap())
      {
        if (node->map_locate(k.ref<utUnity>(j)) || forced) { node = &node->map(k.ref<utUnity>(j)); } else { return 0; }
        continue;
      }
      else if (node->utype() == utUnityArray)
      {
        meta::s_ll ind(0); try { if (k.ua(j).utype() == utEmpty || k.ua(j).utype() == utInt || k.ua(j).utype() == utFloat) { ind = k.ua(j).vint(); } else { return 0; } } catch (...) { return 0; }
        if (node->ua_has(s_long(ind)) || forced) { node = &node->ua(s_long(ind)); }
        continue;
      }
      else if (!forced) { return 0; }
      bool b(false);
      meta::s_ll ind(0); try { b = k.ua(j).utype() == utEmpty || k.ua(j).utype() == utInt || k.ua(j).utype() == utFloat; if (b) { ind = k.ua(j).vint(); } } catch (...) { return 0; }
      if (b) { node = &node->ua(s_long(ind)); } else { node = &node->hash(k.ref<utUnity>(j)); }
      continue;
    }
      return node;
  } catch (...) {}
  return 0;
}

unity::~unity() throw() { clear(); if (pmsm != unity_common::ls_modsm) { cv_ff::cv_reg::L(this, 0x2); } }

void unity::clear() throw() { if (ut == utEmpty) { return; } cv_ff::cv_create::Ldestroy(pmsm, &_data, utype(), (isByPtr() ? 0x1 : 0) | 0x4); ut = utEmpty; }

unity& unity::operator=(const std::wstring& src)
{
  std::wstring* next = cv_ff::cv_wstring::Lnew(pmsm, src.length(), cv_ff::cv_wstring::Fp0(&src), cv_ff::cv_wstring::Fwcs(), 0);
  if (next) { clear(); _data.p1 = next; ut = utString | xfPtr; return *this; }
  throw XUExec("operator=(const std::wstring&)");
}

const std::type_info& unity::objStatTypeInfo() const { if (isObject() && pmsm == unity_common::ls_modsm) { return _drf_o()->objStatTypeInfo(); } else { return typeid(void*); } }
const std::type_info& unity::objDynTypeInfo() const { if (isObject() && pmsm == unity_common::ls_modsm) { return _drf_o()->objDynTypeInfo(); } else { return typeid(void*); } }
int unity::objStrongRefsCount() const { if (isObject() && pmsm == unity_common::ls_modsm) { return _drf_o()->objStrongRefsCount(); } else { return -2; } }
int unity::objWeakRefsCount() const { if (isObject() && pmsm == unity_common::ls_modsm) { return _drf_o()->objWeakRefsCount(); } else { return -2; } }
bool unity::objIsStrongRef() const { if (cpvoid() && pmsm == unity_common::ls_modsm) { return _drf_o()->objIsStrongRef(); } else { return false; } }
bool unity::objIsWeakRef() const { if (cpvoid() && pmsm == unity_common::ls_modsm) { return !_drf_o()->objIsStrongRef(); } else { return false; } }
bool unity::objSetStrongRef(bool isStrongRef) { if (isObject() && pmsm == unity_common::ls_modsm) { return _drf_o()->objSetStrongRef(isStrongRef); } else { return false; } }


s_long unity::arrlb() const { if (isArray()) { return cv_ff::cv_array::Llb_u(this); } throw(XUTypeMismatch("arrlb", _tname0(utype()), "array")); }
s_long unity::arrub() const { if (isArray()) { return cv_ff::cv_array::Lub_u(this); } throw(XUTypeMismatch("arrub", _tname0(utype()), "array")); }
s_long unity::arrsz()const { if (isArray()) { return cv_ff::cv_array::Lsz_u(this); } throw(XUTypeMismatch("arrsz", _tname0(utype()), "array")); }
unity& unity::arrlb_(s_long lbnd)
{
  s_long res = cv_ff::cv_array::Lsz_set(this, lbnd, 0, 0x1);
  if (res == 1) { return *this; } if (res == -2) { throw XUTypeMismatch("arrlb_.1", _tname0(utype()), "array"); }
  throw XUExec("arrlb_.2", _tname0(utype()));
}
unity& unity::arrsz_(s_long n)
{
  s_long res = cv_ff::cv_array::Lsz_set(this, 0, n, 0x2);
  if (res == 1) { return *this; } if (res == -2) { throw XUTypeMismatch("arrsz_.1", _tname0(utype()), "array"); }
  throw XUExec("arrsz_.2", _tname0(utype()));
}
unity& unity::arrub_(s_long ubnd)
{
  s_long res = cv_ff::cv_array::Lsz_set(this, 0, ubnd, 0x6);
  if (res == 1) { return *this; } if (res == -2) { throw XUTypeMismatch("arrub_.1", _tname0(utype()), "array"); }
  throw XUExec("arrub_.2", _tname0(utype()));
}


std::wstring unity::vstr() const
{
  if (isAssoc() || isArray()) { std::wostringstream st; st << *this; return st.str(); }
  return val<utString>(csNormal);
}
std::wstring unity::vstr(s_long ind) const
{
  if (utype() == utUnityArray) { return ref<utUnity>(ind).vstr(); }
  return _arr_el_get<utString>::F(this, ind, csNormal);
}
std::string unity::vcstr() const
{
  if (isAssoc() || isArray()) { std::ostringstream st; st << *this; return st.str(); }
  return wsToBs(val<utString>(csNormal));
}
std::string unity::vcstr(s_long ind) const
{
  if (utype() == utUnityArray) { return ref<utUnity>(ind).vcstr(); }
  return wsToBs(_arr_el_get<utString>::F(this, ind, csNormal));
}
_fls75 unity::vflstr() const throw()
{
  switch (utype())
  {
    case utEmpty: return "";
    case utInt: return *_drf<utInt>();
    case utFloat: return *_drf<utFloat>();
    case utDate: { return dtofls(*_drf<utDate>()); }
    case utChar: { return "+" + _fls75(s_long((unsigned char)(*_drf<utChar>()))); }
    case utString:
    {
      std::wstring* p = _drf<utString>(); if (p) { return *p; }
      p = reference_t<std::wstring>::deref(this->_data, true);
      _fls75 s;
      try
      {
        meta::s_ll n = cv_ff::cv_wstring::Ln(pmsm, p);
        const void* p0 = cv_ff::cv_wstring::Lp0(pmsm, p);
        s_long wcsx = cv_ff::cv_wstring::Lwcs(pmsm);
        s.resize(s_long(n)); n = s.length();
        const char* pc = reinterpret_cast<const char*>(p0);
        if (wcsx == 1) { for (meta::s_ll i = 0; i < n; ++i) { s += wchar_t(*pc); pc += 1; } }
          else if (wcsx == 2) { for (meta::s_ll i = 0; i < n; ++i) { s += wchar_t(s_long(pc[0]) + (s_long(pc[1]) << 8)); pc += 2; } }
          else if (wcsx == 0x102) { for (meta::s_ll i = 0; i < n; ++i) { s += wchar_t(s_long(pc[1]) + (s_long(pc[0]) << 8)); pc += 2; } }
          else if (wcsx == 4) { for (meta::s_ll i = 0; i < n; ++i) { s += wchar_t(s_long(pc[0]) + (s_long(pc[1]) << 8)); pc += 4; } }
          else if (wcsx == 0x104) { for (meta::s_ll i = 0; i < n; ++i) { s += wchar_t(s_long(pc[3]) + (s_long(pc[2]) << 8)); pc += 4; } }
      } catch (...) {}
      return s;
    }
    case utObject: return _fls75("obj@") + meta::s_ll(reinterpret_cast<char*>(_drf_o()->pvoid(0)) - reinterpret_cast<char*>(0));
    case utMap: { if (! _compat_chk()) { break; } _fls75 s; t_map& rm = *_m(); s += "map"; s += rm.n(); if (rm.pkf()->flags() != 0) { s += " f"; s += rm.pkf()->flags(); } s += ":"; for (s_long i = 0; i < rm.n(); ++i) { s += rm(i)->k.vflstr(); s += "="; s += rm(i)->v.vflstr(); if (i + 1 < rm.n()) { s += ";"; } if (s.is_full()) { break; } } return s; }
    case utHash: { if (! _compat_chk()) { break; } _fls75 s; t_hash& rh = *_h(); s += "hash"; s += rh.n(); if (rh.pkf()->flags() != 0) { s += " f"; s += rh.pkf()->flags(); } s += ":"; for (s_long i = 0; i < rh.n(); ++i) { s += rh(i)->k.vflstr(); s += "="; s += rh(i)->v.vflstr(); if (i + 1 < rh.n()) { s += ";"; } if (s.is_full()) { break; } } return s; }
    case utIntArray:
    case utFloatArray:
    case utDateArray:
    case utCharArray:
    case utStringArray:
    case utUnityArray:
    {
      if (! _compat_chk()) { break; }
      _fls75 s;
      try
      {
        const char* al = "IFDCS?V";
        s_long n = cv_ff::cv_array::Lsz_u(this); s_long nb = cv_ff::cv_array::Llb_u(this);
        s += "ar"; s += al[utype() - utIntArray]; s += n;
        if (nb != 0) { s += " b"; s += nb; }
        switch (utype())
        {
          case utUnityArray: { if (n > 0) { s += ": "; for (s_long i = nb; i < nb + n; ++ i) { s += ref<utUnity>(i).vflstr(); if (i + 1 < nb + n) { s += ","; } } } break; }
          case utStringArray: { if (n > 0) { s += ": "; for (s_long i = nb; i < nb + n; ++ i) { s += crefx<utString>(i).ref(); if (i + 1 < nb + n) { s += ","; } } } break; }
          default: { if (n > 0) { s += ": "; for (s_long i = nb; i < nb + n; ++ i) { s += val<utUnity>(i).vflstr(); if (i + 1 < nb + n) { s += ","; } } } break; }
        }
      }
      catch (...) {}
      return s;
    }

    default: break;
  }
  return _fls75("?") + _fls75(utype());
}

std::wstring unity::tname(bool allowArrDim) const
{
  if (allowArrDim && isArray())
  {
    switch (utype())
    {
      case utIntArray: return L".Int" + bsToWs(cv_ff::vecm_strdim(_drf<utIntArray>()->rvecm()));
      case utFloatArray: return L".Float" + bsToWs(cv_ff::vecm_strdim(_drf<utFloatArray>()->rvecm()));
      case utDateArray: return L".Date" + bsToWs(cv_ff::vecm_strdim(_drf<utDateArray>()->rvecm()));
      case utStringArray: return L".String" + bsToWs(cv_ff::vecm_strdim(_drf<utStringArray>()->rvecm()));
      case utCharArray: return L".Char" + bsToWs(cv_ff::vecm_strdim(_drf<utCharArray>()->rvecm()));
      case utUnityArray: return L".Unity" + bsToWs(cv_ff::vecm_strdim(_drf<utUnityArray>()->rvecm()));
      default:
          return L".T" + ntows(utype());
    }
  }
  else if (utype() == utObject) { return bsToWs(_drf_o()->objStatTypeInfo().name()); }
  else { return bsToWs(_tname0(utype())); }
}

_fls75 unity::_tname0(s_long utt) throw ()
{
    switch (utt)
    {
        case utEmpty: return ".Empty";
        case utInt: return ".Int";
        case utFloat: return ".Float";
        case utDate: return ".Date";
        case utString: return ".String";
        case utObject: return ".Object";
        case utChar: return ".Char";
        case utMap: return ".Map";
        case utHash: return ".Hash";
        case utIntArray: return ".Int[]";
        case utFloatArray: return ".Float[]";
        case utDateArray: return ".Date[]";
        case utStringArray: return ".String[]";
        case utCharArray: return ".Char[]";
        case utUnityArray: return ".Unity[]";
        default: return _fls75(".T") + _fls75(utt);
    }
}

s_long unity::k_hashf(s_long flags) const
{
  if (!isScalar() && (flags & fkcmpScalarize)) { return u_key().k_hashf(flags); }
  switch (utype())
  {
      case utEmpty: return -1;
      case utInt: { return flags & fkcmpNFloat ? cv_ff::_kfx<double>::Fhash(_drf<utInt>(), flags) : cv_ff::_kfx<meta::s_ll>::Fhash(_drf<utInt>(), flags); }
      case utFloat: return cv_ff::_kfx<double>::Fhash(_drf<utFloat>(), flags);
      case utDate: return cv_ff::_kfx<double>::Fhash(_drf<utDate>(), flags) ^ 0x999;
      case utChar: { return flags & fkcmpNFloat ? cv_ff::_kfx<double>::Fhash(_drf<utChar>(), flags) : cv_ff::_kfx<_unitychar>::Fhash(_drf<utChar>(), flags); }
      case utString:
      {
        const std::wstring* ps = _drf<utString>();
        if (ps) { return cv_ff::_kfx<std::wstring>::Fhash(ps, flags); }
        std::wstring s = this->vstr(); return cv_ff::_kfx<std::wstring>::Fhash(&s, flags);
      }
      case utObject: { const void* p = cpvoidkey(); return cv_ff::_kfx<const void*>::Fhash(&p, flags); }
      case utMap: { return cv_ff::_kfx<t_map>::Fhash(_m(), flags); }
      case utHash: { return cv_ff::_kfx<t_hash>::Fhash(_h(), flags); }

      case utIntArray: { return flags & fkcmpNFloat ? cv_ff::_kfx<double>::Fhash(_drf<utIntArray>(), flags) : cv_ff::_kfx<meta::s_ll>::Fhash(_drf<utIntArray>(), flags); }
      case utFloatArray: return cv_ff::_kfx<double>::Fhash(_drf<utFloatArray>(), flags);
      case utDateArray: return cv_ff::_kfx<double>::Fhash(_drf<utDateArray>(), flags);
      case utStringArray: return cv_ff::_kfx<std::wstring>::Fhash(_drf<utStringArray>(), flags);
      case utCharArray: { return flags & fkcmpNFloat ? cv_ff::_kfx<double>::Fhash(_drf<utCharArray>(), flags) : cv_ff::_kfx<_unitychar>::Fhash(_drf<utCharArray>(), flags); }
      case utUnityArray: return cv_ff::_kfx<unity>::Fhash(_drf<utUnityArray>(), flags);
      default: return -2 - utype();
  }
}

bool unity::k_eq(const unity& x, s_long flags) const
{
    if (this == &x) { return true; }
    if (flags & fkcmpScalarize)
    {
      if (!isScalar()) { return u_key().k_eq(x, flags); }
      if (!x.isScalar()) { return k_eq(x.u_key(), flags); }
    }
    switch (utype())
    {
        case utEmpty:
          { return x.isEmpty(); }
        case utInt:
            switch (x.utype())
            {
              case utInt: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf<utInt>(), x._drf<utInt>(), flags); } return cv_ff::_kfx<meta::s_ll>::Feq(_drf<utInt>(), x._drf<utInt>(), flags);  }
              case utFloat: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf<utInt>(), x._drf<utFloat>(), flags); } return false; }
              case utChar: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf<utInt>(), x._drf<utChar>(), flags); } return false; }
              default: return false;
            }
        case utFloat:
            switch (x.utype())
            {
              case utFloat: { return cv_ff::_kfx<double>::Feq(_drf<utFloat>(), x._drf<utFloat>(), flags); }
              case utInt: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf<utFloat>(), x._drf<utInt>(), flags); } return false;  }
              case utChar: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf<utFloat>(), x._drf<utChar>(), flags); } return false;  }
              default: return false;
            }
        case utChar:
            switch (x.utype())
            {
              case utChar: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf<utChar>(), x._drf<utChar>(), flags); } return cv_ff::_kfx<_unitychar>::Feq(_drf<utChar>(), x._drf<utChar>(), flags);  }
              case utFloat: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf<utChar>(), x._drf<utFloat>(), flags); } return false; }
              case utInt: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf<utChar>(), x._drf<utInt>(), flags); } return false; }
              default: return false;
            }
        case utString:
          {
            if (x.utype() != utString) { return false; }
            const std::wstring* ps1 = _drf<utString>();
            const std::wstring* ps2 = x._drf<utString>();
            if (ps1)
            {
              if (ps2) { return cv_ff::_kfx<std::wstring>::Feq(ps1, ps2, flags); }
              std::wstring x2 = x.vstr(); return cv_ff::_kfx<std::wstring>::Feq(ps1, &x2, flags);
            }
            else
            {
              std::wstring x1 = this->vstr();
              if (ps2) { return cv_ff::_kfx<std::wstring>::Feq(&x1, ps2, flags); }
              std::wstring x2 = x.vstr(); return cv_ff::_kfx<std::wstring>::Feq(&x1, &x2, flags);
            }
          }
        case utDate:
          { if (x.utype() == utDate) { return cv_ff::_kfx<double>::Feq(_drf<utDate>(), x._drf<utDate>(), flags); } return false; }

        case utObject:
          { if (x.utype() == utObject) { return cpvoidkey() == x.cpvoidkey(); } return false; }

        case utMap: { if (x.utype() == utMap) { return cv_ff::_kfx<t_map>::Feq(_m(), x._m(), flags); } return false; }
        case utHash: { if (x.utype() == utHash) { return cv_ff::_kfx<t_hash>::Feq(_h(), x._h(), flags); } return false; }

        case utIntArray:
            switch (x.utype())
            {
              case utIntArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf<utIntArray>(), x._drf<utIntArray>(), flags); } return cv_ff::_kfx<meta::s_ll>::Feq(_drf<utIntArray>(), x._drf<utIntArray>(), flags);  }
              case utFloatArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf<utIntArray>(), x._drf<utFloatArray>(), flags); } return false; }
              case utCharArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf<utIntArray>(), x._drf<utCharArray>(), flags); } return false; }
              default: return false;
            }
        case utFloatArray:
            switch (x.utype())
            {
              case utFloatArray: { return cv_ff::_kfx<double>::Feq(_drf<utFloatArray>(), x._drf<utFloatArray>(), flags); }
              case utIntArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf<utFloatArray>(), x._drf<utIntArray>(), flags); } return false;  }
              case utCharArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf<utFloatArray>(), x._drf<utCharArray>(), flags); } return false;  }
              default: return false;
            }
        case utCharArray:
            switch (x.utype())
            {
              case utCharArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf<utCharArray>(), x._drf<utCharArray>(), flags); } return cv_ff::_kfx<_unitychar>::Feq(_drf<utCharArray>(), x._drf<utCharArray>(), flags);  }
              case utFloatArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf<utCharArray>(), x._drf<utFloatArray>(), flags); } return false; }
              case utIntArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Feq(_drf<utCharArray>(), x._drf<utIntArray>(), flags); } return false; }
              default: return false;
            }
        case utDateArray:
          { if (x.utype() == utDateArray) { return cv_ff::_kfx<double>::Feq(_drf<utDateArray>(), x._drf<utDateArray>(), flags); } return false; }
        case utStringArray:
          { if (x.utype() == utStringArray) { return cv_ff::_kfx<std::wstring>::Feq(_drf<utStringArray>(), x._drf<utStringArray>(), flags); } return false; }
        case utUnityArray:
          { if (x.utype() == utUnityArray) { return cv_ff::_kfx<unity>::Feq(_drf<utUnityArray>(), x._drf<utUnityArray>(), flags); } return false; }
        default: throw XUExec("k_eq", utype());
    }
}

bool unity::k_less(const unity& x, s_long flags) const
{
    if (this == &x) { return false; }
    if (flags & fkcmpScalarize)
    {
      if (!isScalar()) { return u_key().k_less(x, flags); }
      if (!x.isScalar()) { return k_less(x.u_key(), flags); }
    }
    if (flags & fkcmpRevOrder) { return x.k_less(*this, flags & ~fkcmpRevOrder); }
    switch (utype())
    {
        case utEmpty:
          { return !x.isEmpty(); }
        case utInt:
            switch (x.utype())
            {
              case utInt: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf<utInt>(), x._drf<utInt>(), flags); } return cv_ff::_kfx<meta::s_ll>::Fless12(_drf<utInt>(), x._drf<utInt>(), flags);  }
              case utFloat: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf<utInt>(), x._drf<utFloat>(), flags); } return utInt < utFloat; }
              case utChar: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf<utInt>(), x._drf<utChar>(), flags); } return utInt < utChar; }
              default: return utInt < x.utype();
            }
        case utFloat:
            switch (x.utype())
            {
              case utFloat: { return cv_ff::_kfx<double>::Fless12(_drf<utFloat>(), x._drf<utFloat>(), flags); }
              case utInt: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf<utFloat>(), x._drf<utInt>(), flags); } return utFloat < utInt; }
              case utChar: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf<utFloat>(), x._drf<utChar>(), flags); } return utFloat < utChar; }
              default: return utFloat < x.utype();
            }
        case utChar:
            switch (x.utype())
            {
              case utChar: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf<utChar>(), x._drf<utChar>(), flags); } return cv_ff::_kfx<_unitychar>::Fless12(_drf<utChar>(), x._drf<utChar>(), flags);  }
              case utFloat: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf<utChar>(), x._drf<utFloat>(), flags); } return utChar < utFloat; }
              case utInt: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf<utChar>(), x._drf<utInt>(), flags); } return utChar < utInt; }
              default: return utChar < x.utype();
            }
        case utString:
          {
            if (x.utype() != utString) { return utString < x.utype(); }
            const std::wstring* ps1 = _drf<utString>();
            const std::wstring* ps2 = x._drf<utString>();
            if (ps1)
            {
              if (ps2) { return cv_ff::_kfx<std::wstring>::Fless12(ps1, ps2, flags); }
              std::wstring x2 = x.vstr(); return cv_ff::_kfx<std::wstring>::Fless12(ps1, &x2, flags);
            }
            else
            {
              std::wstring x1 = this->vstr();
              if (ps2) { return cv_ff::_kfx<std::wstring>::Fless12(&x1, ps2, flags); }
              std::wstring x2 = x.vstr(); return cv_ff::_kfx<std::wstring>::Fless12(&x1, &x2, flags);
            }
          }
        case utDate:
          { if (x.utype() == utDate) { return cv_ff::_kfx<double>::Fless12(_drf<utDate>(), x._drf<utDate>(), flags); } return utDate < x.utype(); }

        case utObject:
          { if (x.utype() == utObject) { return cpvoidkey() < x.cpvoidkey(); } return utObject < x.utype(); }

        case utMap: { if (x.utype() == utMap) { return cv_ff::_kfx<t_map>::Fless12(_m(), x._m(), flags); } return utMap < x.utype(); }
        case utHash: { if (x.utype() == utHash) { return cv_ff::_kfx<t_hash>::Fless12(_h(), x._h(), flags); } return utHash < x.utype(); }

        case utIntArray:
            switch (x.utype())
            {
              case utIntArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf<utIntArray>(), x._drf<utIntArray>(), flags); } return cv_ff::_kfx<meta::s_ll>::Fless12(_drf<utIntArray>(), x._drf<utIntArray>(), flags);  }
              case utFloatArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf<utIntArray>(), x._drf<utFloatArray>(), flags); } return utIntArray < utFloatArray; }
              case utCharArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf<utIntArray>(), x._drf<utCharArray>(), flags); } return utIntArray < utCharArray; }
              default: return utIntArray < x.utype();
            }
        case utFloatArray:
            switch (x.utype())
            {
              case utFloatArray: { return cv_ff::_kfx<double>::Fless12(_drf<utFloatArray>(), x._drf<utFloatArray>(), flags); }
              case utIntArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf<utFloatArray>(), x._drf<utIntArray>(), flags); } return utFloatArray < utIntArray;  }
              case utCharArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf<utFloatArray>(), x._drf<utCharArray>(), flags); } return utFloatArray < utCharArray;  }
              default: return utFloatArray < x.utype();
            }
        case utCharArray:
            switch (x.utype())
            {
              case utCharArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf<utCharArray>(), x._drf<utCharArray>(), flags); } return cv_ff::_kfx<_unitychar>::Fless12(_drf<utCharArray>(), x._drf<utCharArray>(), flags);  }
              case utFloatArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf<utCharArray>(), x._drf<utFloatArray>(), flags); } return utCharArray < utFloatArray; }
              case utIntArray: { if (flags & fkcmpNFloat) { return cv_ff::_kfx<double>::Fless12(_drf<utCharArray>(), x._drf<utIntArray>(), flags); } return utCharArray < utIntArray; }
              default: return utCharArray < x.utype();
            }
        case utDateArray:
          { if (x.utype() == utDateArray) { return cv_ff::_kfx<double>::Fless12(_drf<utDateArray>(), x._drf<utDateArray>(), flags); } return utDateArray < x.utype(); }
        case utStringArray:
          { if (x.utype() == utStringArray) { return cv_ff::_kfx<std::wstring>::Fless12(_drf<utStringArray>(), x._drf<utStringArray>(), flags); } return utStringArray < x.utype(); }
        case utUnityArray:
          { if (x.utype() == utUnityArray) { return cv_ff::_kfx<unity>::Fless12(_drf<utUnityArray>(), x._drf<utUnityArray>(), flags); } return utUnityArray < x.utype(); }
        default: throw XUExec("k_less", utype());
    }
}

const unity::arg& unity::_noarg() { static const arg x; return x; }

unity& unity::array( const arg& x1, const arg& x2, const arg& x3, const arg& x4, const arg& x5, const arg& x6, const arg& x7, const arg& x8, const arg& x9, const arg& x10, const arg& x11, const arg& x12, const arg& x13, const arg& x14, const arg& x15, const arg& x16, const arg& x17, const arg& x18, const arg& x19, const arg& x20, const arg& x21, const arg& x22, const arg& x23, const arg& x24, const arg& x25, const arg& x26, const arg& x27, const arg& x28, const arg& x29, const arg& x30 )
{
    arr_init<utUnity>(1);
    if (x1.parg()) { arr_append(*x1.parg()); } else { return *this; }
    if (x2.parg()) { arr_append(*x2.parg()); } else { return *this; }
    if (x3.parg()) { arr_append(*x3.parg()); } else { return *this; }
    if (x4.parg()) { arr_append(*x4.parg()); } else { return *this; }
    if (x5.parg()) { arr_append(*x5.parg()); } else { return *this; }
    if (x6.parg()) { arr_append(*x6.parg()); } else { return *this; }
    if (x7.parg()) { arr_append(*x7.parg()); } else { return *this; }
    if (x8.parg()) { arr_append(*x8.parg()); } else { return *this; }
    if (x9.parg()) { arr_append(*x9.parg()); } else { return *this; }
    if (x10.parg()) { arr_append(*x10.parg()); } else { return *this; }
    if (x11.parg()) { arr_append(*x11.parg()); } else { return *this; }
    if (x12.parg()) { arr_append(*x12.parg()); } else { return *this; }
    if (x13.parg()) { arr_append(*x13.parg()); } else { return *this; }
    if (x14.parg()) { arr_append(*x14.parg()); } else { return *this; }
    if (x15.parg()) { arr_append(*x15.parg()); } else { return *this; }
    if (x16.parg()) { arr_append(*x16.parg()); } else { return *this; }
    if (x17.parg()) { arr_append(*x17.parg()); } else { return *this; }
    if (x18.parg()) { arr_append(*x18.parg()); } else { return *this; }
    if (x19.parg()) { arr_append(*x19.parg()); } else { return *this; }
    if (x20.parg()) { arr_append(*x20.parg()); } else { return *this; }
    if (x21.parg()) { arr_append(*x21.parg()); } else { return *this; }
    if (x22.parg()) { arr_append(*x22.parg()); } else { return *this; }
    if (x23.parg()) { arr_append(*x23.parg()); } else { return *this; }
    if (x24.parg()) { arr_append(*x24.parg()); } else { return *this; }
    if (x25.parg()) { arr_append(*x25.parg()); } else { return *this; }
    if (x26.parg()) { arr_append(*x26.parg()); } else { return *this; }
    if (x27.parg()) { arr_append(*x27.parg()); } else { return *this; }
    if (x28.parg()) { arr_append(*x28.parg()); } else { return *this; }
    if (x29.parg()) { arr_append(*x29.parg()); } else { return *this; }
    if (x30.parg()) { arr_append(*x30.parg()); } else { return *this; }
    return *this;
}




















































const unity& unity::u_key_get(unity& dest, EConvStrength cs) const
{
  try {
    if (utype() <= utString) { dest._x_asg(*this, 0); return *this; }
    if (isArray()) { dest = L"?array/" + tname(); return *this; }
    if (isObject()) { const unity* p = objPtr_c<unity>(true); dest = p ? L"?unity/" + p->u_name().vstr() : bsToWs(std::string("?object/") + objStatTypeInfo().name()); return *this; }
    dest = L"?value/" + tname();
    return *this;
  }
  catch (_XUBase&) { if (cs == csLazy) { dest.clear(); return *this; } throw; }
  catch (...) { if (cs == csLazy) { dest.clear(); return *this; } throw XUExec("u_key_get.1", dest.vflstr()); }
}

unity& unity::u_clear(s_long utt, s_long flags, EConvStrength cs)
{
  // utt changed: create st, clear this, assign st.
  // utt changed for named: create ptr st, backup name, clear this, assign st, assign name.
  // utt unchanged: call _Lu_clear.
  if (utt == -1 || utt == utype()) { if (_Lu_clear(flags) > 0) { return *this; } else { throw XUExec("u_clear.1"); } }
  else if (utt >= 0) // create new object
  {
    if (cs == csStrict) { throw XUTypeMismatch("u_clear.4", _tname0(utt), _tname0(utype())); }
    _storage st;
    void* _pn = (ut & xfPtr) ? _data.p2 : 0;
    s_long f_cr = _pn || utt >= utString ? 0x1 : 0;
    if (_Lcreate(&st, utt, f_cr) > 0)
    {
        if (_pn) { _data.p2 = 0; }
      clear(); _data = st; ut = utt; if (f_cr & 0x1) { ut |= xfPtr; }
        if (_pn) { _data.p2 = _pn; }
      return *this;
    }
    else { throw XUExec("u_clear.2"); }
  } else { throw XUExec("u_clear.3"); }
}





bool unity::ua_has(s_long ind) const { if (!isArray()) { return false; } s_long n = cv_ff::cv_array::Lsz_u(this); s_long nb = cv_ff::cv_array::Llb_u(this); ind -= nb; return ind >= 0 && ind < n; }
bool unity::ua_canref(s_long ind) const { if (!isArray()) { return pmsm == unity_common::ls_modsm; } return utype() == utUnityArray && _compat_chk(); }
bool unity::ua_empty(s_long ind) const { if (!isArray()) { return true; } if (utype() != utUnityArray) { return false; } unity* p = _drf<utUnityArray>()->pval(ind); return !p || p->isEmpty(); }
unity& unity::ua(s_long ind) { if (!isArray()) { u_clear(utUnityArray); } if (utype() == utUnityArray && cv_ff::cv_array::Lua_ensure_ind(this, ind) >= 0) { return *_drf<utUnityArray>()->pc(ind); }  throw XUExec("ua"); }
unity& unity::ua_first() { if (!isArray()) { u_clear(utUnityArray); } unity* px = ref<utUnityArray>().pval_first(); if (px) { return *px; } throw XUExec("ua_first.1"); }
unity& unity::ua_last() { if (!isArray()) { u_clear(utUnityArray); } unity* px = ref<utUnityArray>().pval_last(); if (px) { return *px; } throw XUExec("ua_last.1"); }
unity unity::ua_val(s_long ind) { if (!isArray()) { u_clear(utUnityArray); } cv_ff::cv_array::Lua_ensure_ind(this, ind); return val<utUnity>(ind); }
unity unity::ua_val_first() { if (!isArray()) { u_clear(utUnityArray); } return val<utUnity>(cv_ff::cv_array::Llb_u(this)); }
unity unity::ua_val_last() { if (!isArray()) { u_clear(utUnityArray); } return val<utUnity>(cv_ff::cv_array::Lub_u(this)); }
unity& unity::ua_set(s_long ind, const unity& v) { if (!isArray()) { u_clear(utUnityArray); } cv_ff::cv_array::Lua_ensure_ind(this, ind); arr_set(ind, v); return *this; }
unity& unity::ua_append(const unity& v) { if (!isArray()) { u_clear(utUnityArray); } arr_append(v); return *this; }
s_long unity::uaS() { if (!isArray()) { u_clear(utUnityArray); } return cv_ff::cv_array::Lsz_u(this); }
s_long unity::uaLb() { if (!isArray()) { u_clear(utUnityArray); } return cv_ff::cv_array::Llb_u(this); }
s_long unity::uaUb() { if (!isArray()) { u_clear(utUnityArray); } return cv_ff::cv_array::Lub_u(this); }
unity& unity::uaS_set(s_long new_S) { if (!isArray()) { u_clear(utUnityArray); } if (cv_ff::cv_array::Lsz_set(this, 0, new_S, 0x2) > 0) { return *this; } throw XUExec("uaS_set.1"); }
unity& unity::uaS_set(s_long new_S, const unity& v) { if (!isArray()) { u_clear(utUnityArray); } s_long n = cv_ff::cv_array::Lsz_u(this); s_long delta = new_S - n; if (!delta) { return *this; } s_long nb = cv_ff::cv_array::Llb_u(this); if (cv_ff::cv_array::Lel_insrem(this, &v, nb + n + (delta > 0 ? 0 : delta), delta) > 0) { return *this; } throw XUExec("uaS_set.2"); }
unity& unity::ua_resize(s_long ind, s_long m) { if (!isArray()) { u_clear(utUnityArray); } if (m == 0) { return *this; } s_long q1 = cv_ff::cv_array::Llb_u(this); s_long q2 = q1 + cv_ff::cv_array::Lsz_u(this); if (m > 0) { if (ind < q1) { ind = q1; } else if (ind > q2) { ind = q2; } } else { s_long ind2 = ind - m; if (ind < q1) { ind = q1; } else if (ind > q2) { ind = q2; } if (ind2 < q1) { ind2 = q1; } else if (ind2 > q2) { ind2 = q2; } m = ind - ind2; if (m == 0) { return *this; } } if (cv_ff::cv_array::Lel_insrem(this, 0, ind, m) > 0) { return *this; } throw XUExec("ua_resize.1"); }
unity& unity::ua_resize(s_long ind, s_long m, const unity& v) { if (!isArray()) { u_clear(utUnityArray); } if (m == 0) { return *this; } s_long q1 = cv_ff::cv_array::Llb_u(this); s_long q2 = q1 + cv_ff::cv_array::Lsz_u(this); if (m > 0) { if (ind < q1) { ind = q1; } else if (ind > q2) { ind = q2; } } else { s_long ind2 = ind - m; if (ind < q1) { ind = q1; } else if (ind > q2) { ind = q2; } if (ind2 < q1) { ind2 = q1; } else if (ind2 > q2) { ind2 = q2; } m = ind - ind2; if (m == 0) { return *this; } } if (cv_ff::cv_array::Lel_insrem(this, &v, ind, m) > 0) { return *this; } throw XUExec("ua_resize.2"); }
unity& unity::uaLb_set(s_long new_L) { if (!isArray()) { u_clear(utUnityArray); } if (cv_ff::cv_array::Lsz_set(this, new_L, 0, 0x1) > 0) { return *this; } throw XUExec("uaLb_set.1"); }
unity& unity::ua_fill(s_long utt, const unity& v) { s_long res = cv_ff::cv_array::Lua_fill(this, &v, utt); if (res >= 0) { return *this; } std::string e("ua_fill.A"); e[e.size() - 1] = char(s_long('A') + (-res % 26)); throw XUExec(e); }






namespace {
struct _mx : unity::t_map
{
  typedef meta::assert<(no_elem == -1)>::t_true __check1;

  void __rev_inds() { s_long m = n() / 2; for (s_long i = 0; i < m; ++i) { s_long* p1 = _inds.pval_0u<s_long>(i); s_long* p2 = _inds.pval_0u<s_long>(n() - i - 1); s_long temp = *p1; *p1 = *p2; *p2 = temp; } }
    // 1 - k was inserted, v assigned.
    // 0 - k existed, v assigned on keep_first false, or ignored on keep_first true.
    // -1 - an error occured, the map did not change.
    // NOTE if appending breaks the order, normal insertion occurs.
  s_long __append(bool at_front, const unity& k, const unity& v, bool keep_first)
  {
    const entry* e(0); s_long i1(-1); bool r2(false);
    do {
      if (n() > 0)
      {
        if (at_front)
        {
          e = _d(*_inds.pval_first<s_long>()); if (!e) { return -1; }
          if (pkf()->less12(k, e->k))
          {
            if (_d.insert(k, &e) != 1) { e = 0; break; } r2 = true;
            if (_inds.el_insert_1<s_long>(0, _inds.n()) != 1) { break; }
            i1 = 0;
          }
          else if (!pkf()->less12(e->k, k)) { i1 = 0; }
          else { e = 0; }
        }
        else
        {
          e = _d(*_inds.pval_last<s_long>()); if (!e) { return -1; }
          if (pkf()->less12(e->k, k))
          {
            if (_d.insert(k, &e) != 1) { e = 0; break; } r2 = true;
            if (_inds.el_insert_1<s_long>(_inds.n(), _inds.n()) != 1) { break; }
            i1 = _inds.n() - 1;
          }
          else if (!pkf()->less12(k, e->k)) { i1 = n() - 1; }
          else { e = 0; }
        }
      }
    } while (false);
    if (e)
    {
      if (i1 < 0) { _d.remove_e(e); return -1; }
      if (keep_first && !r2) { return 0; }
      try { e->v = v; } catch (...) { if (r2) { _d.remove_e(e); _inds.el_remove_1<s_long>(i1, false); } return -1; }
      return 1;
    }
    s_long res = insert(k, &e, &i1); if (res < 0) { return -1; }
    if (keep_first && res == 0) { return 0; }
    try { e->v = v; } catch (...) { if (res > 0) { _d.remove_e(e); _inds.el_remove_1<s_long>(i1, false); } return -1; }
    return res;
  }
};
}

unity& unity::map_keys_get(unity& dest, s_long lb) { _ensure_m(); unity z0; vec2_t<unity>& z = z0.rx<utUnityArray>(); z.vec2_set_nbase(lb); t_map& rm = *_m(); for (s_long i = 0; i < rm.n(); ++i) { z.push_back(rm(i)->k); }  dest._xu_move_p1(z0); return *this; }
unity& unity::map_values_get(unity& dest, s_long lb) { _ensure_m(); unity z0; vec2_t<unity>& z = z0.rx<utUnityArray>(); z.vec2_set_nbase(lb); t_map& rm = *_m(); for (s_long i = 0; i < rm.n(); ++i) { z.push_back(rm(i)->v); }  dest._xu_move_p1(z0); return *this; }
s_long unity::mapFlags()        { _ensure_m(); return _m()->pkf()->flags(); }
void unity::mapFlags_set(s_long fk_reset, s_long fk_set)
{
  _ensure_m();
  s_long fk0 = _m()->pkf()->flags();
  s_long fk = ((fk0 & ~fk_reset) | fk_set) & _fkcmp_mask;
  if (fk0 == fk || _m()->n() == 0) { _m()->pkf()->_set_flags(fk); return; }
  _mx& rm = *static_cast<_mx*>(_m());
  if ((fk0 ^ fk) == fkcmpRevOrder)
  {
    rm.__rev_inds();
    rm.pkf()->_set_flags(fk); return;
  }
  bool b_ro = bool((fk0 ^ fk) & fkcmpRevOrder);
  unity z0; if (pmsm != unity_common::ls_modsm) { z0.pmsm = pmsm; cv_ff::cv_reg::L(&z0, 0x1); } z0._ensure_m(); _mx& rm2 = *static_cast<_mx*>(z0._m()); rm2.pkf()->_set_flags(fk);
  for (s_long i = 0; i < rm.n(); ++i) { const t_map::entry* e = rm(i); if (rm2.__append(b_ro, e->k, e->v, true) < 0) { throw XUExec("mapFlags_set.1"); } }
  u_clear(); _data.p1 = z0._data.p1; z0._data.p1 = 0; z0.ut = utEmpty;
}

bool unity::map_empty(const unity& k) const
{
  if (!(isMap() && _compat_chk())) { return true; }
  if (_m()->n() == 0) { return true; }
  if (k.isEmpty()) { t_map& rm = *_m(); const t_map::entry* e = (rm.pkf()->flags() & fkcmpRevOrder) ? rm(rm.n() - 1) : rm(0); return e->k.isEmpty() ? e->v.isEmpty() : true; }
  const t_map::entry* e = _m()->find(k); return !e || e->v.isEmpty();
}
bool unity::map_locate(const unity& k, bool insert)
{
  _ensure_m();
  if (insert) { s_long res = _m()->insert(k); if (res >= 0) { return res > 0; } throw XUExec("map_locate.1"); }
  return _m()->find(k);
}
bool unity::map_locate(const unity& k, bool insert, s_long& ind)
{
  _ensure_m();
  if (k.isEmpty())
  {
    _mx& rm = *static_cast<_mx*>(_m());
    bool b_ro = bool(rm.pkf()->flags() & fkcmpRevOrder);
    if (rm.find(k)) { ind = b_ro ? rm.n() : 1; return !insert; }
    if (!insert) { ind = b_ro ? rm.n() + 1 : 1; return false; }
    s_long res = rm.__append(!b_ro, k, unity(), false);
    if (res == 1) { ind = b_ro ? rm.n() : 1; return true; }
    throw XUExec("map_locate.3");
  }
  s_long ind2; s_long res;
  if (insert) { res = _m()->insert(k, 0, &ind2); if (res >= 0) { ind = ind2 + 1; return res > 0; } throw XUExec("map_locate.2"); }
  res = _m()->find(k, &ind2) ? 1 : 0; ind = res ? ind2 + 1 : map_noel(); return bool(res);
}

unity& unity::map(const unity& k)        { _ensure_m(); return (*_m())[k]; }
bool unity::map_append(const unity& k, const unity& v, bool keep_first)        { _ensure_m(); _mx& rm = *static_cast<_mx*>(_m()); s_long res = rm.__append(false, k, v, keep_first); if (res >= 0) { return res > 0; } throw XUExec("map_append.1"); }
bool unity::map_del(const unity& k)        { _ensure_m(); s_long res = _m()->remove(k); if (res >= 0) { return res > 0; } throw XUExec("map_del.1"); }

s_long unity::mapS()        { _ensure_m(); return _m()->n(); }
unity& unity::mapS_set(s_long new_S)
{
  _ensure_m(); t_map& rm = *_m();
  if (new_S >= rm.n()) { return *this; }
  if (new_S <= 0) { if (rm.remove_all() >= 0) { return *this; } throw XUExec("mapS_set.1"); }
  if (new_S == rm.n() - 1) { if (rm.remove(rm(new_S)->k) >= 0) { return *this; } throw XUExec("mapS_set.2"); }
  unity z0; if (pmsm != unity_common::ls_modsm) { z0.pmsm = pmsm; cv_ff::cv_reg::L(&z0, 0x1); } z0._ensure_m(); _mx& rm2 = *static_cast<_mx*>(z0._m()); rm2.pkf()->_set_flags(rm.pkf()->flags());
  for (s_long i = 0; i < new_S; ++i) { const t_map::entry* e = rm(i); if (rm2.__append(false, e->k, e->v, true) < 0) { throw XUExec("mapS_set.3"); } }
  u_clear(); _data.p1 = z0._data.p1; z0._data.p1 = 0; z0.ut = utEmpty;
  return *this;
}

bool unity::mapi_has(s_long ind) const        { return isMap() && _compat_chk() && ind >= 1 && ind <= _m()->n(); }
bool unity::mapi_empty(s_long ind) const        { if (!(isMap() && _compat_chk())) { return false; } const t_map::entry* e = (*_m())(ind - 1); return e && e->v.isEmpty(); }
const unity& unity::mapi_key(s_long ind)        { _ensure_m(); const t_map::entry* e = (*_m())(ind - 1); if (e) { return e->k; } throw XUExec("mapi_key.1"); }
unity& unity::mapi(s_long ind)        { _ensure_m(); const t_map::entry* e = (*_m())(ind - 1); if (e) { return e->v; } throw XUExec("mapi.1"); }
unity& unity::mapi_del(s_long ind)        { _ensure_m(); const t_map::entry* e = (*_m())(ind - 1); if (e) { if (_m()->remove(e->k) > 0) { return *this; } throw XUExec("mapi_del.1"); } throw XUExec("mapi_del.2"); }






unity& unity::_hl_impl::operator[] (const unity& k) throw (exc_subscript)
{
  const t_hash::entry* e(0);
  s_long res = this->t_hash::insert(k, &e);
  if (res > 0) { if (!_a_appended()) { this->t_hash::remove_e(e); res = -1; } }
  if (res >= 0) { return e->v; } throw exc_subscript();
}
  // Returns same as hashx remove_all (>= 0 - on_calm_result, < 0 - failure, no changes.)
s_long unity::_hl_impl::hl_clear(bool full) throw ()
{
  if (full) { s_long n_prev = _n; hashx_clear(); _a_cleared(); return n_prev; }
  s_long res = this->t_hash::remove_all(); if (res >= 0) { _a_cleared(); } return res;
}
bool unity::_hl_impl::_a_rsv_1() throw()
  { return _list.el_reserve_n(n() + 2, false) && _indk.el_reserve_n(n() + 1, false); }
bool unity::_hl_impl::_a_appended() throw()
{
  s_long i = n() - 1; if (i < 0 || i != _list.n() - 1) { return false; }
  if (!_list.el_append<meta::s_ll>(-1)) { return false; }
  __insert(i, _prev_of(-1));
  if (i == _indk.n() && _indk.el_append<s_long>(i)) { return true; }
  if (_indk.n()) { _indk.vecm_clear(); } return true;
}
void unity::_hl_impl::_a_removed(s_long ind) throw()
{
  if (!(ind >= 0 && ind <= n() && _list.n() == n() + 2)) { return; }
  if (_indk.n())
  {
    if (_indk.n() - 1 == n() && *_indk.pval_last<s_long>() == n() && ind == n()) { _indk.el_expunge_last<s_long>(); }
      else { _indk.vecm_clear(); }
  }
  if (_list.n() != n() + 2) { return; }
  __eject(ind);
  if (ind == n()) { _list.el_expunge_last<meta::s_ll>(); return; }
  s_long ip = _prev_of(n()); __eject(n()); _list.el_expunge_last<meta::s_ll>(); __insert(ind, ip);
}
void unity::_hl_impl::__eject(s_long ind) throw()
{
  s_long ipr = _prev_of(ind); s_long inx = _next_of(ind);
  if (ipr == inx) { _setv(ipr, _v(ipr, ipr)); }
    else if (_list.n() > 2) { meta::s_ll xp = _v(_prev_of(ipr), inx); meta::s_ll xn =  _v(ipr, _next_of(inx)); _setv(ipr, xp); _setv(inx, xn); }
}
void unity::_hl_impl::__insert(s_long ind, s_long after) throw()
{
  if (_list.n() > 2)
  {
    meta::s_ll xp = _v(_prev_of(after), ind); s_long inx = _next_of(after); meta::s_ll xn = _v(ind, _next_of(inx));
    _setv(ind, _v(after, inx)); _setv(after, xp); _setv(inx, xn);
  }
  else
  {
    if (ind == after) { return; }
    _setv(ind, _v(after, after)); _setv(after, _v(ind, ind));
  }
}
void unity::_hl_impl::_a_cleared() throw()
  {  _list.el_expand_n(1); *_list.pval_first<meta::s_ll>() = _v(-1, -1);  _indk.vecm_clear(); }
bool unity::_hl_impl::_a_indk() const throw()
{
  if (_indk.n() == n()) { return true; }
  if (!_indk.el_expand_n(n())) { _indk.vecm_clear(); return false; }
  s_long pos = -1; s_long i = 0; while (i < n()) { pos = _next_of(pos); if (!(pos >= 0 && pos < n())) { break; } *_indk.pval_0u<s_long>(i) = pos; ++i; }
  if (i != n()) { _indk.vecm_clear(); return false; }
  return true;
}


unity& unity::hash_keys_get(unity& dest, s_long lb) { _ensure_h(); unity z0; vec2_t<unity>& z = z0.rx<utUnityArray>(); z.vec2_set_nbase(lb); t_hash& rh = *_h(); s_long pos = rh.qi_next(rh.qi_noel()); while (pos != rh.qi_noel()) { z.push_back(rh.h(pos)->k); pos = rh.qi_next(pos); } dest._xu_move_p1(z0); return *this; }
unity& unity::hash_values_get(unity& dest, s_long lb) { _ensure_h(); unity z0; vec2_t<unity>& z = z0.rx<utUnityArray>(); z.vec2_set_nbase(lb); t_hash& rh = *_h(); s_long pos = rh.qi_next(rh.qi_noel()); while (pos != rh.qi_noel()) { z.push_back(rh.h(pos)->v); pos = rh.qi_next(pos); } dest._xu_move_p1(z0); return *this; }
s_long unity::hashFlags()        { _ensure_h(); return _h()->pkf()->flags(); }
void unity::hashFlags_set(s_long fk_reset, s_long fk_set)
{
  _ensure_h();
  s_long fk0 = _h()->pkf()->flags();
  s_long fk = ((fk0 & ~fk_reset) | fk_set) & _fkcmp_mask;
  if (((fk0 ^ fk) & ~fkcmpRevOrder) == 0 || _h()->n() == 0) { _h()->pkf()->_set_flags(fk); return; }
  unity z0; if (pmsm != unity_common::ls_modsm) { z0.pmsm = pmsm; cv_ff::cv_reg::L(&z0, 0x1); } z0._ensure_h(); t_hash& rh2 = *z0._h(); rh2.pkf()->_set_flags(fk);
  t_hash& rh = *_h(); s_long pos = rh.qi_next(rh.qi_noel());
  while (pos != rh.qi_noel()) { const t_hash::entry* e = rh.h(pos); rh2[e->k] = e->v; pos = rh.qi_next(pos); }
  u_clear(); _data.p1 = z0._data.p1; z0._data.p1 = 0; z0.ut = utEmpty;
}

bool unity::hash_empty(const unity& k) const        { if (!(isHash() && _compat_chk())) { return true; } const t_hash::entry* e = _h()->find(k); return !e || e->v.isEmpty(); }
bool unity::hash_locate(const unity& k, bool insert)
{
  _ensure_h();
  if (insert) { s_long res = _h()->insert(k); if (res >= 0) { return res > 0; } throw XUExec("hash_locate.1"); }
  return _h()->find(k);
}
unity& unity::hash(const unity& k)        { _ensure_h(); return (*_h())[k]; }
bool unity::hash_set(const unity& k, const unity& v, bool keep_first)
{
  _ensure_h();
  const t_hash::entry* e(0);
  s_long res = _h()->insert(k, &e);
  if (res > 0) { e->v = v; return true; }
    else if (res == 0) { if (!keep_first) { e->v = v; } return false; }
  throw XUExec("hash_set.1");
}
bool unity::hash_del(const unity& k)
{
  _ensure_h();
  s_long res = _h()->remove(k);
  if (res >= 0) { return res > 0; }
  throw XUExec("hash_del.1");
}

s_long unity::hashS()        { _ensure_h(); return _h()->n(); }
s_long unity::hashS_c() const    { if (isHash() && _compat_chk()) { return _h()->n(); } throw XUExec("hashS_c.1"); }

bool unity::hashi_has(s_long ind) const        { return isHash() && _compat_chk() && ind >= 1 && ind <= _h()->n(); }
bool unity::hashi_empty(s_long ind) const        { if (!(isHash() && _compat_chk())) { return false; } const t_hash::entry* e = (*_h())(ind - 1); return e && e->v.isEmpty(); }
const unity& unity::hashi_key(s_long ind)        { _ensure_h(); const t_hash::entry* e = (*_h())(ind - 1); if (e) { return e->k; } throw XUExec("hashi_key.1"); }
const unity& unity::hashi_key_c(s_long ind) const    { if (isHash() && _compat_chk()) { const t_hash::entry* e = (*_h())(ind - 1); if (e) { return e->k; } } throw XUExec("hashi_key_c.1"); }
unity& unity::hashi(s_long ind)        { _ensure_h(); const t_hash::entry* e = (*_h())(ind - 1); if (e) { return e->v; } throw XUExec("hashi.1"); }
const unity& unity::hashi_c(s_long ind) const        { if (isHash() && _compat_chk()) { const t_hash::entry* e = (*_h())(ind - 1); if (e) { return e->v; } } throw XUExec("hashi_c.1"); }

s_long unity::hashl_next(s_long pos)        { _ensure_h(); return _h()->qi_next(pos); }
s_long unity::hashl_prev(s_long pos)        { _ensure_h(); return _h()->qi_prev(pos); }
const unity& unity::hashl_key(s_long pos)        { _ensure_h(); const t_hash::entry* e = _h()->h(pos); if (e) { return e->k; } throw XUExec("hashl_key.1"); }
unity& unity::hashl(s_long pos)        { _ensure_h(); const t_hash::entry* e = _h()->h(pos); if (e) { return e->v; } throw XUExec("hashl.1"); }

bool unity::u_has(const unity& ki, s_long tt) const
{
  if (!_compat_chk()) { return false; }
  if ((tt & 1) != 0 && utype() == utUnityArray)
  {
    meta::s_ll ind;
    try { ind = ki.vint(); } catch (...) { return false; }
    if (!(ind >= cv_ff::cv_array::Llb_u(this) && ind <= cv_ff::cv_array::Lub_u(this))) { return false; }
    return true;
  }
  if ((tt & 2) != 0 && utype() == utMap) { return _m()->find(ki); }
  if ((tt & 4) != 0 && utype() == utHash) { return _h()->find(ki); }
  if ((tt & 8) != 0 && utype() == utObject) { const unity* p = objPtr_c<unity>(); if (p) { return p->u_has(ki, tt); } }
  return false;
}

unity& unity::operator[] (const unity& ki)
{
  if (!_compat_chk()) { throw XUExec("operator[].6"); }
  if (utype() == utUnityArray)
  {
    meta::s_ll ind;
    try { ind = ki.vint(); } catch (...) { throw XUExec("operator[].1"); }
    if (!(ind >= cv_ff::cv_array::Llb_u(this) && ind <= cv_ff::cv_array::Lub_u(this))) { throw XUExec("operator[].2"); }
    return *_drf<utUnityArray>()->pval(s_long(ind));
  }
  else if (utype() == utMap) { const t_map::entry* e = _m()->find(ki); if (e) { return e->v; } throw XUExec("operator[].3"); }
  else if (utype() == utHash) { const t_hash::entry* e = _h()->find(ki); if (e) { return e->v; } throw XUExec("operator[].4"); }
  else if (utype() == utObject) { unity* p = objPtr<unity>(); if (p) { return (*p)[ki]; } }
  throw XUExec("operator[].5");
}


bool unity::assoc_set(const unity& k, const unity& v, bool keep_first)
{
  if (isHash()) { if (_compat_chk()) { const t_hash::entry* e(0); s_long res = _h()->insert(k, &e); if (res > 0) { e->v = v; return true; } else if (res == 0) { if (!keep_first) { e->v = v; } return false; } } throw XUExec("assoc_set.2"); }
    else if (isMap()) { if (_compat_chk()) { _mx& rm = *static_cast<_mx*>(_m()); s_long res = rm.__append(false, k, v, keep_first); if (res >= 0) { return res > 0; } } throw XUExec("assoc_set.1"); }
    else { throw XUExec("assoc_set.3"); }
}
bool unity::assoc_del(const unity& k)
{
  if (isHash()) { if (_compat_chk()) { s_long res = _h()->remove(k); if (res >= 0) { return res > 0; } } throw XUExec("assoc_del.2"); }
    else if (isMap()) { if (_compat_chk()) { s_long res = _m()->remove(k); if (res >= 0) { return res > 0; } } throw XUExec("assoc_del.1"); }
    else { throw XUExec("assoc_del.3"); }
}
s_long unity::assocl_next(s_long pos) const
{
  if (isHash()) { if (_compat_chk()) { return _h()->qi_next(pos); } }
    else if (isMap()) { if (_compat_chk()) { pos = pos > 0 ? pos + 1 : 1; if (pos > _m()->n()) { pos = hashx_common::no_elem; } return pos; } }
  return hashx_common::no_elem;
}
s_long unity::assocl_prev(s_long pos) const
{
  if (isHash()) { if (_compat_chk()) { return _h()->qi_prev(pos); } }
    else if (isMap()) { if (_compat_chk()) { pos = pos < 1 ? _m()->n() : pos - 1; if (pos <= 0) { pos = hashx_common::no_elem; } return pos; } }
  return hashx_common::no_elem;
}
const unity& unity::assocl_key(s_long pos) const
{
  if (isHash()) { if (_compat_chk()) { const t_hash::entry* e = _h()->h(pos); if (e) { return e->k; } } throw XUExec("assocl_key.2"); }
    else if (isMap()) { if (_compat_chk()) { const t_map::entry* e = (*_m())(pos - 1); if (e) { return e->k; } } throw XUExec("assocl_key.1"); }
    else { throw XUExec("assocl_key.3"); }
}
unity& unity::assocl(s_long pos)
{
  if (isHash()) { if (_compat_chk()) { const t_hash::entry* e = _h()->h(pos); if (e) { return e->v; } } throw XUExec("assocl.2"); }
    else if (isMap()) { if (_compat_chk()) { const t_map::entry* e = (*_m())(pos - 1); if (e) { return e->v; } } throw XUExec("assocl.1"); }
    else { throw XUExec("assocl.3"); }
}
const unity& unity::assocl_c(s_long pos) const
{
  if (isHash()) { if (_compat_chk()) { const t_hash::entry* e = _h()->h(pos); if (e) { return e->v; } } throw XUExec("assocl_c.2"); }
    else if (isMap()) { if (_compat_chk()) { const t_map::entry* e = (*_m())(pos - 1); if (e) { return e->v; } } throw XUExec("assocl_c.1"); }
    else { throw XUExec("assocl_c.3"); }
}
s_long unity::assocS_c() const
{
  if (isHash()) { if (_compat_chk()) { return _h()->n(); } throw XUExec("assocS_c.1"); }
    else if (isMap()) { if (_compat_chk()) { return _m()->n(); } throw XUExec("assocS_c.2"); }
    else { throw XUExec("assocS_c.3"); }
}




  // bsp true once appends a space character before outputting the value.
  // nsp > 0 appends nsp space characters when starting each new line.
void unity::write_fmt(std::ostream& s, const unity& x, const iofmt& f, bool bsp, s_long nsp)
{
  const int _nsptab = 2;
  if (bsp) { s << ' '; } bool bsp2(x.isNamed());
  if (bsp2) { s << '`'; write_fmt(s, x.u_name(), x.format(f.fmt), false, 0); s << '`'; }

  switch (x.utype())
  {
    case utEmpty: { s << "Empty"; return; }
    case utInt: { if (bsp2) { s << ' '; }  s << *x._drf<utInt>(); return; }
    case utFloat:
    {
      if (bsp2) { s << ' '; }
      std::ostringstream s2; s2 << std::setprecision(15) << *x._drf<utFloat>(); std::string s3 = s2.str();
      _t_sz i = s3.find_first_of("."); if (i != nposc) { s << s3; return; }
      i = s3.find_first_of("eE"); if (i == nposc) { i = s3.length(); } s3.insert(i, 1, '.'); s << s3;
      return;
    }
    case utDate: { if (bsp2) { s << ' '; } s << dtofls(*x._drf<utDate>()); return; }
    case utChar: { if (bsp2) { s << ' '; } s << '+' << s_long((unsigned char)(*x._drf<utChar>())); return; }
    case utString: { cref_t<std::wstring> cr = x.cref<utString>(); if (bsp2) { s << ' '; } else { s << 's' << cr.ref().length() << ' '; }  s << wsToBs(cr.ref()); return; }
    case utObject:
    {
      const void* p0 = x.cpvoidkey(); s_long ind = f._rho().n() + 1; bool bnew = false; const iofmt::t_h::entry* e = f._rho().find(p0); if (e) { ind = e->v; } else { bnew = true; f._rho()[p0] = ind; }
      meta::s_ll adr = (char*)p0 - (char*)(0);
      s << "obj#" << ind << " @" << adr;
      if (bnew) { const unity* p = x.objPtr_c<unity>(true); if (p) { s << ":\n"; for (s_long i = 0; i < nsp; ++i) { s << ' '; } write_fmt(s, *p, f, false, nsp + _nsptab); s << "\n"; } else { s << '.'; } }
        else { s << '>'; }
      return;
    }
    case utMap:
    {
      t_map& rm = *x._m();
      s << "Map n=" << rm.n() << " f=" << rm.pkf()->flags() << (rm.n() ? ":\n" : "\n");
      for (s_long i = 0; i < rm.n(); ++ i)
      {
        for (s_long j = 0; j < nsp; ++j) { s << ' '; }
        s << (i + 1);
        write_fmt(s, rm(i)->k, f, true, nsp + _nsptab);
        write_fmt(s, rm(i)->v, f, true, nsp + _nsptab);
        s << "\n";
      }
      return;
    }
    case utHash:
    {
      t_hash& rh = *x._h();
      s << "Hash n=" << rh.n() << " f=" << rh.pkf()->flags() << (rh.n() ? ":\n" : "\n");
      for (s_long i = 0; i < rh.n(); ++ i)
      {
        for (s_long j = 0; j < nsp; ++j) { s << ' '; }
        s << (i + 1);
        write_fmt(s, rh(i)->k, f, true, nsp + _nsptab);
        write_fmt(s, rh(i)->v, f, true, nsp + _nsptab);
        s << "\n";
      }
      return;
    }
    case utIntArray:
    case utFloatArray:
    case utDateArray:
    case utStringArray:
    case utCharArray:
    case utUnityArray:
    {
      static const char* al[7] = { "Int", "Float", "Date", "Char", "String", "?" , "Unity" };
      s_long n = cv_ff::cv_array::Lsz_u(&x); s_long nb = cv_ff::cv_array::Llb_u(&x); s_long nlast = nb + n - 1;
      s << al[x.utype() - utIntArray] << "Array n=" << n << " b=" << nb;
      switch (x.utype())
      {
        case utUnityArray: { if (n > 0) { s << ":\n"; for (s_long i = nb; i <= nlast; ++ i) { for (s_long j = 0; j < nsp; ++j) { s << ' '; } s << (i); const unity& u = x.ref<utUnity>(i); write_fmt(s, u, f, true, nsp + _nsptab); if (u.isScalar() || u.isObject()) { s << "\n"; } } } else { s << "\n"; } break; }
        case utStringArray: { if (n > 0) { s << ":\n"; for (s_long i = nb; i <= nlast; ++ i) { for (s_long j = 0; j < nsp; ++j) { s << ' '; } s << (i); write_fmt(s, x.crefx<utString>(i).ref(), f, true, nsp + _nsptab); s << "\n"; } } else { s << "\n"; } break; }
        default: { if (n > 0) { s << ":\n"; for (s_long i = nb; i <= nlast; ++ i) { for (s_long j = 0; j < nsp; ++j) { s << ' '; } s << (i); write_fmt(s, x.val<utUnity>(i), x.format(f.fmt), true, nsp + _nsptab); s << "\n"; } } else { s << "\n"; } break; }
      }
      return;
    }
    default: { s << "Unknown t=" << x.utype(); return; }
  }
}

void unity::write_fmt(std::wostream& s, const unity& x, const iofmt& f, bool bsp, s_long nsp)
{
  const int _nsptab = 2;
  if (bsp) { s << L' '; } bool bsp2(x.isNamed());
  if (bsp2) { s << L'`'; write_fmt(s, x.u_name(), x.format(f.fmt), false, 0); s << L'`'; }

  switch (x.utype())
  {
    case utEmpty: { s << L"Empty"; return; }
    case utInt: { if (bsp2) { s << L' '; }  s << *x._drf<utInt>(); return; }
    case utFloat:
    {
      if (bsp2) { s << L' '; }
      std::wostringstream s2; s2 << std::setprecision(15) << *x._drf<utFloat>(); std::wstring s3 = s2.str();
      _t_wz i = s3.find_first_of(L"."); if (i != nposc) { s << s3; return; }
      i = s3.find_first_of(L"eE"); if (i == nposc) { i = s3.length(); } s3.insert(i, 1, '.'); s << s3;
      return;
    }
    case utDate: { if (bsp2) { s << L' '; } s << dtofls(*x._drf<utDate>()).wstr(); return; }
    case utChar: { if (bsp2) { s << L' '; } s << L'+' << s_long((unsigned char)(*x._drf<utChar>())); return; }
    case utString: { cref_t<std::wstring> cr = x.cref<utString>(); if (bsp2) { s << L' '; } else { s << L's' << cr.ref().length() << L' '; }  s << cr.ref(); return; }
    case utObject:
    {
      const void* p0 = x.cpvoidkey(); s_long ind = f._rho().n() + 1; bool bnew = false; const iofmt::t_h::entry* e = f._rho().find(p0); if (e) { ind = e->v; } else { bnew = true; f._rho()[p0] = ind; }
      meta::s_ll adr = (char*)p0 - (char*)(0);
      s << L"obj#" << ind << L" @" << adr;
      if (bnew) { const unity* p = x.objPtr_c<unity>(true); if (p) { s << L":\n"; for (s_long i = 0; i < nsp; ++i) { s << L' '; } write_fmt(s, *p, f, false, nsp + _nsptab); s << L"\n"; } else { s << L'.'; } }
        else { s << L'>'; }
      return;
    }
    case utMap:
    {
      t_map& rm = *x._m();
      s << L"Map n=" << rm.n() << L" f=" << rm.pkf()->flags() << (rm.n() ? L":\n" : L"\n");
      for (s_long i = 0; i < rm.n(); ++ i)
      {
        for (s_long j = 0; j < nsp; ++j) { s << L' '; }
        s << (i + 1);
        write_fmt(s, rm(i)->k, f, true, nsp + _nsptab);
        write_fmt(s, rm(i)->v, f, true, nsp + _nsptab);
        s << L"\n";
      }
      return;
    }
    case utHash:
    {
      t_hash& rh = *x._h();
      s << L"Hash n=" << rh.n() << L" f=" << rh.pkf()->flags() << (rh.n() ? ":\n" : "\n");
      for (s_long i = 0; i < rh.n(); ++ i)
      {
        for (s_long j = 0; j < nsp; ++j) { s << L' '; }
        s << (i + 1);
        write_fmt(s, rh(i)->k, f, true, nsp + _nsptab);
        write_fmt(s, rh(i)->v, f, true, nsp + _nsptab);
        s << L"\n";
      }
      return;
    }
    case utIntArray:
    case utFloatArray:
    case utDateArray:
    case utStringArray:
    case utCharArray:
    case utUnityArray:
    {
      static const char* al[7] = { "Int", "Float", "Date", "Char", "String", "?" , "Unity" };
      s_long n = cv_ff::cv_array::Lsz_u(&x); s_long nb = cv_ff::cv_array::Llb_u(&x); s_long nlast = nb + n - 1;
      s << al[x.utype() - utIntArray] << L"Array n=" << n << L" b=" << nb;
      switch (x.utype())
      {
        case utUnityArray: { if (n > 0) { s << L":\n"; for (s_long i = nb; i <= nlast; ++ i) { for (s_long j = 0; j < nsp; ++j) { s << L' '; } s << (i); const unity& u = x.ref<utUnity>(i); write_fmt(s, u, f, true, nsp + _nsptab); if (u.isScalar() || u.isObject()) { s << L"\n"; } } } else { s << L"\n"; } break; }
        case utStringArray: { if (n > 0) { s << L":\n"; for (s_long i = nb; i <= nlast; ++ i) { for (s_long j = 0; j < nsp; ++j) { s << L' '; } s << (i); write_fmt(s, x.crefx<utString>(i).ref(), f, true, nsp + _nsptab); s << L"\n"; } } else { s << L"\n"; } break; }
        default: { if (n > 0) { s << L":\n"; for (s_long i = nb; i <= nlast; ++ i) { for (s_long j = 0; j < nsp; ++j) { s << L' '; } s << (i); write_fmt(s, x.val<utUnity>(i), x.format(f.fmt), true, nsp + _nsptab); s << L"\n"; } } else { s << L"\n"; } break; }
      }
      return;
    }
    default: { s << L"Unknown t=" << x.utype(); return; }
  }
}











////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Utilities

std::string _wsToBs(const wchar_t* ps, meta::s_ll n, bool is_oem); // platform-dependent
std::string _wsToBsUtf8(const wchar_t* ps, meta::s_ll n);
std::string _wsToBsLsb(const wchar_t* ps, meta::s_ll n);

std::string wsToBs(const wchar_t* x, meta::s_ll n){ return _wsToBs(x, n, false); }
std::string wsToBs(const std::wstring& x) { return _wsToBs(x.c_str(), x.length(), false); }
std::string wsToBsOem(const wchar_t* x, meta::s_ll n){ return _wsToBs(x, n, true); }
std::string wsToBsOem(const std::wstring& x) { return _wsToBs(x.c_str(), x.length(), true); }
std::string wsToBsUtf8(const wchar_t* x, meta::s_ll n) { return _wsToBsUtf8(x, n); }
std::string wsToBsUtf8(const std::wstring& x) { return _wsToBsUtf8(x.c_str(), x.length()); }
std::string wsToBsLsb(const wchar_t* x, meta::s_ll n) { return _wsToBsLsb(x, n); }
std::string wsToBsLsb(const std::wstring& x) { return _wsToBsLsb(x.c_str(), x.length()); }

std::wstring _bsToWs(const char* ps, meta::s_ll n); // platform-dependent
std::wstring _bsUtf8ToWs(const char* ps, meta::s_ll n);
std::wstring _bsLsbToWs(const char* ps, meta::s_ll n);

std::wstring bsToWs(const char* x, meta::s_ll n){ return _bsToWs(x, n); }
std::wstring bsToWs(const std::string& x){ return _bsToWs(x.c_str(), x.length()); }
std::wstring bsUtf8ToWs(const char* x, meta::s_ll n) { return _bsUtf8ToWs(x, n); }
std::wstring bsUtf8ToWs(const std::string& x) { return _bsUtf8ToWs(x.c_str(), x.length()); }
std::wstring bsLsbToWs(const char* x, meta::s_ll n) { return _bsLsbToWs(x, n); }
std::wstring bsLsbToWs(const std::string& x) { return _bsLsbToWs(x.c_str(), x.length()); }


std::string _wsToBsUtf8(const wchar_t* ps, meta::s_ll n)
{
  try {
    if (!ps) { throw XUExec("_wsToBsUtf8.3"); }
    if (n < 0) { n = 0; const wchar_t* p = ps; while (*p++) { ++n; } }
    std::string s;
    if (n <= 0) { return s; }
    for (meta::s_ll i = 0; i < n; ++i)
    {
      s_long q = s_long(ps[i]) & 0xffff;
      if (q >= 0xd800 && q < 0xe000)
      {
        if (q >= 0xdc00 || i + 1 >= n) { q = L'?'; }
        else
        {
          s_long q2 = s_long(ps[i + 1]) & 0xffff;
          if (q2 >= 0xdc00 && q2 < 0xe000) { q = (((q - 0xd800) << 10) | (q2 - 0xdc00)) + 0x10000; ++i; }
            else { q = L'?'; }
        }
      }
      if (q <= 0x7f) { s += char(q); }
        else if (q <= 0x7ff) { s += char((q >> 6) | 0xc0); s += char((q & 0x3f) | 0x80); }
        else if (q <= 0xffff) { s += char((q >> 12) | 0xe0); s += char(((q >> 6) & 0x3f) | 0x80); s += char((q & 0x3f) | 0x80); }
        else { s += char((q >> 18) | 0xf0); s += char(((q >> 12) & 0x3f) | 0x80); s += char(((q >> 6) & 0x3f) | 0x80); s += char((q & 0x3f) | 0x80); }
    }
    return s;
  }
  catch (_XUBase&) { throw; }
  catch (const std::exception& e) { throw XUExec("_wsToBsUtf8.1", e.what()); }
  catch (...) { throw XUExec("_wsToBsUtf8.2"); }
}
std::string _wsToBsLsb(const wchar_t* ps, meta::s_ll n)
{
  try {
    if (!ps) { throw XUExec("_wsToBsLsb.3"); }
    if (n < 0) { n = 0; const wchar_t* p = ps; while (*p++) { ++n; } }
    std::string s;
    _t_sz nz1 = s.max_size(); _t_sz nz2 = (1ul << 31) - 1; if (nz1 < nz2) { nz2 = nz1; } if (n > s_ll(nz2)) { n = nz2; }
    s.resize(_t_sz(n));
    for (meta::s_ll i = 0; i < n; ++i) { s[_t_sz(i)] = char(ps[i]); }
    return s;
  }
  catch (_XUBase&) { throw; }
  catch (const std::exception& e) { throw XUExec("_wsToBsLsb.1", e.what()); }
  catch (...) { throw XUExec("_wsToBsLsb.2"); }
}

std::wstring _bsUtf8ToWs(const char* ps, meta::s_ll n)
{
  try {
    if (!ps) { throw XUExec("_bsUtf8ToWs.3"); }
    if (n < 0) { n = 0; const char* p = ps; while (*p++) { ++n; } }
    std::wstring s; meta::s_ll i = 0;
    while (i < n)
    {
      unsigned char c = ps[i];
      if ((c & 0x80) == 0) { s += wchar_t(c); ++i; }
      else
      {
        s_long nbchr = 0;
        while (((c << nbchr) & 0xc0) == 0xc0) { ++nbchr; }
        if (nbchr >= 1 && nbchr <= 5 && i + nbchr < n)
        {
          s_long q(c & (0x3f >> nbchr)); ++i;
          do { c = ps[i]; if ((c & 0xc0) != 0x80) { q = L'?'; break; } q = (q << 6) | (c & 0x3f); ++i; } while (--nbchr);
          if (q > 0x10ffff) { s += L'?'; }
            else if (q >= 0x10000) { q -= 0x10000; s += wchar_t(0xd800 | (q >> 10)); s += wchar_t(0xdc00 | (q & 0x3ff)); }
            else { s += wchar_t(q); }
        }
        else { s += L'?'; ++i; }
      }
    }
    return s;
  }
  catch (_XUBase&) { throw; }
  catch (const std::exception& e) { throw XUExec("_bsUtf8ToWs.1", e.what()); }
  catch (...) { throw XUExec("_bsUtf8ToWs.2"); }
}
std::wstring _bsLsbToWs(const char* ps, meta::s_ll n)
{
  try {
    if (!ps) { throw XUExec("_bsLsbToWs.3"); }
    if (n < 0) { n = 0; const char* p = ps; while (*p++) { ++n; } }
    std::wstring s;
    _t_wz nz1 = s.max_size(); _t_wz nz2 = (1ul << 31) - 1; if (nz1 < nz2) { nz2 = nz1; } if (n > s_ll(nz2)) { n = nz2; }
    s.resize(_t_wz(n));
    for (meta::s_ll i = 0; i < n; ++i) { s[_t_wz(i)] = wchar_t(unsigned(ps[i]) & 0xff); }
    return s;
  }
  catch (_XUBase&) { throw; }
  catch (const std::exception& e) { throw XUExec("_bsLsbToWs.1", e.what()); }
  catch (...) { throw XUExec("_bsLsbToWs.2"); }
}


std::string replace(const std::string& s, const std::string& from, const std::string& to, bool ignoreCase)
{
    if (from.length() == 0) { return s; }
    else if (s.length() > 0)
    {
        std::string dest;
        _t_sz pos, pos2;
        pos=0;
        if (ignoreCase)
        {
          t_critsec_locale __lock(wsbs_lkp(), -1); if (sizeof(__lock)) {}
          const char* prevlocn = std::setlocale(LC_CTYPE, "");
            std::string s_l = _lcase(s);
            std::string from_l = _lcase(from);
            do
            {
              pos2 = s_l.find(from_l, pos);
              if (pos2 == nposc) { pos2 = s.length(); dest += s.substr(pos, pos2 - pos); }
                else { dest += s.substr(pos, pos2 - pos); dest += to; pos2 += from.length(); }
              pos = pos2;
            }
            while(pos < s.length());
          std::setlocale(LC_CTYPE, prevlocn);
        }
        else
        {
          do
          {
              pos2 = s.find(from, pos);
              if (pos2 == nposc) { pos2 = s.length(); dest += s.substr(pos, pos2 - pos); }
                else { dest += s.substr(pos, pos2 - pos); dest += to; pos2 += from.length(); }
              pos = pos2;
          }
          while(pos < s.length());
        }
        return dest;
    }
    else return s;
}

std::wstring replace(const std::wstring& s, const std::wstring& from, const std::wstring& to, bool ignoreCase)
{
    if (from.length() == 0) { return s; }
    else if (s.length() > 0)
    {
        std::wstring dest;
        _t_wz pos, pos2;
        pos=0;
        if (ignoreCase)
        {
          t_critsec_locale __lock(wsbs_lkp(), -1); if (sizeof(__lock)) {}
          const char* prevlocn = std::setlocale(LC_CTYPE, "");
            std::wstring s_l = _lcase(s);
            std::wstring from_l = _lcase(from);
            do
            {
              pos2 = s_l.find(from_l, pos);
              if (pos2 == nposw) { pos2 = s.length(); dest += s.substr(pos, pos2 - pos); }
                else { dest += s.substr(pos, pos2 - pos); dest += to; pos2 += from.length(); }
              pos = pos2;
            }
            while(pos < s.length());
          std::setlocale(LC_CTYPE, prevlocn);
        }
        else
        {
          do
          {
              pos2 = s.find(from, pos);
              if (pos2 == nposw) { pos2 = s.length(); dest += s.substr(pos, pos2 - pos); }
                else { dest += s.substr(pos, pos2 - pos); dest += to; pos2 += from.length(); }
              pos = pos2;
          }
          while(pos < s.length());
        }
        return dest;
    }
    else return s;
}


bool wstring_like(const std::wstring& str, const std::wstring& pattern)
{
    std::vector<std::wstring> ptnParts;
      //- ptnParts[i] having appropriate isPtnRange[i] == true is a wildcard (ptnParts[i].size() == 1 i.e. single character)
      //  or one or more ranges of characters (ptnParts[i].size() is a multiple of 2, each odd-even characters specify
      //  one range of characters, inclusive).
    std::vector<bool> isPtnRange;
    std::wstring sFixed;
    bool isUnclosedBracket = false;
    _t_wz ptnPos = 0;
    while (ptnPos < pattern.length())
    {
      switch (pattern[ptnPos])
      {
      case L'*':
        if (sFixed.size() > 0)
        {
          ptnParts.push_back(sFixed);
          isPtnRange.push_back(false);
          sFixed.clear();
        }
        if (ptnParts.size() == 0 || ptnParts.back() != L"*")
        {
          ptnParts.push_back(L"*");
          isPtnRange.push_back(true);
          ++ptnPos;
        }
        break;
      case L'?':
      case L'#':
        if (sFixed.size() > 0)
        {
          ptnParts.push_back(sFixed);
          isPtnRange.push_back(false);
          sFixed.clear();
        }
        ptnParts.push_back(pattern.substr(ptnPos, 1)); // either ? or #
        isPtnRange.push_back(true);
        ++ptnPos;
        break;
      case L'[':
        if (isUnclosedBracket)
        {
          sFixed += pattern[ptnPos]; // added '[' a fixed character
          ++ptnPos;
        }
        else
        {
          _t_wz pos2 = pattern.find(L']', ptnPos+1);
          if (pos2 == nposw) // means: there is the first unclosed bracket found at pos
          {
            isUnclosedBracket = true;
            sFixed += pattern[ptnPos]; // added '[' a fixed character
            ++ptnPos;
          }
          else
          {
            // extract character ranges specified between closed brackets
            if (pos2 - ptnPos > 1)
            {
              std::wstring sRanges;
              _t_wz pos3 = ptnPos + 1;

              if (pattern[pos3] == L'!')
              {
                if (pos2 > pos3 + 1) { sRanges += L'!'; } else { sRanges += L" !!"; }
                ++pos3;
              }
              else
              {
                sRanges += L' ';
              }
              while (pos3 < pos2)
              {
                if (pos3 + 2 < pos2 && pattern[pos3+1] == L'-')
                {
                  // a range of characters specified by 3-character sequence like "a-z"
                  sRanges += pattern[pos3];
                  sRanges += pattern[pos3+2];
                  pos3 += 3;
                }
                else
                {
                  // range containing only one character
                  sRanges += pattern[pos3];
                  sRanges += pattern[pos3];
                  ++pos3;
                }
              }

              if (sRanges.size() == 3 && sRanges[0] == L' ' && sRanges[1] == sRanges[2])
              {
                sFixed += sRanges[1];
              }
              else
              {
                if (sFixed.size() > 0)
                {
                  ptnParts.push_back(sFixed);
                  isPtnRange.push_back(false);
                  sFixed.clear();
                }

                ptnParts.push_back(sRanges);
                isPtnRange.push_back(true);
              }

              ptnPos = pos2 + 1; // go to position next after the closing bracket
            }
            else
            {
              ptnPos = pos2 + 1; // skip empty brackets
            }
          }
        }
        break;
      default:
        sFixed += pattern[ptnPos]; // a fixed character
        ++ptnPos;
        break;
      }
    }
    if (sFixed.size() > 0) // care for tail of pattern possibly left in sFixed
    {
      ptnParts.push_back(sFixed);
      isPtnRange.push_back(false);
      sFixed.clear();
    }

    std::vector<_t_wz> strPosStack; // std::string positions tried to match against *
    std::vector<_t_wz> indPtnStack; // the index of *-s that are matched

    _t_wz strPos = 0;
    _t_wz indPtnPart = 0;

    while (true)
    {
      if (strPos >= str.size()) // case: str is gone, i.e. nothing is left to compare, should return already
      {
        if (indPtnPart >= ptnParts.size())
        {
          // cases:
          // a) both str and pattern are empty, i.e. they match
          // b) both str and pattern are gone at the same time, i.e. they match
          goto lProcessMatch;
        }
        else
        {
          if (indPtnPart == ptnParts.size() - 1 && isPtnRange[indPtnPart] && ptnParts[indPtnPart] == L"*")
          {
            // case: in pattern, only * is left, here this matches with the end of std::string, i.e. with ""
            goto lProcessMatch;
          }
        }
        goto lProcessMismatch;
      }
      else // case: comparing the current pos in the std::string with the current pattern
      {
        if (indPtnPart >= ptnParts.size())
        {
          // case: pattern has gone, but std::string had not gone yet, i.e. mismatch
          goto lProcessMismatch;
        }
        if (isPtnRange[indPtnPart])
        {
          if (ptnParts[indPtnPart].size() == 1) // a wildcard character should be processed
          {
            if (ptnParts[indPtnPart] == L"?")
            {
              // matched successfully (any character matches),
              // move to next position in the std::string and initForCallback the next pattern
              strPos += 1;
              indPtnPart += 1;
            }
            else if (ptnParts[indPtnPart] == L"#")
            {
              if (isdigit(str[strPos]))
              {
                // matched a digit,
                // move to next position in the std::string and initForCallback the next pattern
                strPos += 1;
                indPtnPart += 1;
              }
              else // don't match
              {
                goto lProcessMismatch;
              }
            }
            else if (ptnParts[indPtnPart] == L"*")
            {
              goto lProcessAsterisk;
            }
            else // bad branch, assuming don't match
            {
              goto lProcessMismatch;
            }
          }
          else // the current str character should be matched against one or more character ranges
          {
            bool isInRangeTest = ptnParts[indPtnPart][0] != L'!';
            bool isMatch = false;
            for (unsigned int i = 1; i < ptnParts[indPtnPart].size(); i += 2)
            {
              if (str[strPos] >= ptnParts[indPtnPart][i] && str[strPos] <= ptnParts[indPtnPart][i + 1])
              {
                // found a range that matches to the current str character
                isMatch = true;
                break;
              }
            }
            if (isMatch == isInRangeTest)
            {
              // matched successfully,
              // move to next position in the std::string and initForCallback the next pattern
              strPos += 1;
              indPtnPart += 1;
            }
            else // don't match
            {
              goto lProcessMismatch;
            }
          }
        }
        else // the str characters at the current position should be matched against some fixed string
        {
          if (str.substr(strPos, ptnParts[indPtnPart].size()) == ptnParts[indPtnPart])
          {
            // matched successfully,
            // move to next position in the std::string and initForCallback the next pattern
            strPos += ptnParts[indPtnPart].size();
            indPtnPart += 1;
          }
          else // don't match
          {
            goto lProcessMismatch;
          }
        }
      }
      continue;
lProcessAsterisk: // position at some * wildcard somewhere in str
      // Needs to calculate next guess for strPos, if any exists at all, store it into the stack,
      // and then match the current strPos against the rest of patterns list.
      // cases:
      // a) * is the last pattern, i.e. total match
      // b) * is followed by a fixed string
      // c) * is followed by a range (the worst case, i.e. need processing str char by char)
      if (indPtnPart == ptnParts.size() - 1) // a)
      {
        goto lProcessMatch;
      }
      else if (!isPtnRange[indPtnPart+1]) // b)
      {
        // next guess
        _t_wz pos4 = str.find(ptnParts[indPtnPart+1], strPos);
        if (pos4 == nposw)
        {
          // Failed to find a fixed part of the pattern.
          //  Our matching is not greedy, and processes shorter guesses first,
          //  so here the whole pattern appears longer than str.
          //  This means final mismatch.
          return false;
        }
        else // the fixed part of the pattern matches with the current str position or greater position
        {
          // prepare possibly one more guess, and store it to stack, then initForCallback the current match
          _t_wz pos5 = pos4 + 1;
          if (pos5 < str.size())
          {
            strPosStack.push_back(pos5);
            indPtnStack.push_back(indPtnPart);
          }

          // continue matching in the current guess, having skipped the current fixed part of the pattern
          strPos += pos4 + ptnParts[indPtnPart+1].size();
          indPtnPart += 2;
        }
      }
      else // c)
      {
        // next guess
        _t_wz pos4 = strPos + 1;

        // store next guess
        strPosStack.push_back(pos4);
        indPtnStack.push_back(indPtnPart);

        indPtnPart += 1; // continue matching in the current guess, from the next pattern, which is guaranteed to be a range, anything except *
      }
      continue;
lProcessMismatch:
      if (strPosStack.size() == 0)
      {
        return false;
      }
      strPos = strPosStack.back();
        strPosStack.pop_back();
      indPtnPart = indPtnStack.back();
        indPtnStack.pop_back();
      goto lProcessAsterisk;
lProcessMatch:
      return true;
    }
}

unity choose ( s_long ind, const unity::arg& x1, const unity::arg& x2, const unity::arg& x3, const unity::arg& x4, const unity::arg& x5, const unity::arg& x6, const unity::arg& x7, const unity::arg& x8, const unity::arg& x9, const unity::arg& x10, const unity::arg& x11, const unity::arg& x12, const unity::arg& x13, const unity::arg& x14, const unity::arg& x15, const unity::arg& x16, const unity::arg& x17, const unity::arg& x18, const unity::arg& x19, const unity::arg& x20, const unity::arg& x21, const unity::arg& x22, const unity::arg& x23, const unity::arg& x24, const unity::arg& x25, const unity::arg& x26, const unity::arg& x27, const unity::arg& x28, const unity::arg& x29, const unity::arg& x30 )
{
  switch (ind)
  {
    case 1: return x1.parg()?*x1.parg():unity(); case 2: return x2.parg()?*x2.parg():unity(); case 3: return x3.parg()?*x3.parg():unity(); case 4: return x4.parg()?*x4.parg():unity(); case 5: return x5.parg()?*x5.parg():unity(); case 6: return x6.parg()?*x6.parg():unity(); case 7: return x7.parg()?*x7.parg():unity(); case 8: return x8.parg()?*x8.parg():unity(); case 9: return x9.parg()?*x9.parg():unity(); case 10: return x10.parg()?*x10.parg():unity(); case 11: return x11.parg()?*x11.parg():unity(); case 12: return x12.parg()?*x12.parg():unity(); case 13: return x13.parg()?*x13.parg():unity(); case 14: return x14.parg()?*x14.parg():unity(); case 15: return x15.parg()?*x15.parg():unity(); case 16: return x16.parg()?*x16.parg():unity(); case 17: return x17.parg()?*x17.parg():unity(); case 18: return x18.parg()?*x18.parg():unity(); case 19: return x19.parg()?*x19.parg():unity(); case 20: return x20.parg()?*x20.parg():unity(); case 21: return x21.parg()?*x21.parg():unity(); case 22: return x22.parg()?*x22.parg():unity(); case 23: return x23.parg()?*x23.parg():unity(); case 24: return x24.parg()?*x24.parg():unity(); case 25: return x25.parg()?*x25.parg():unity(); case 26: return x26.parg()?*x26.parg():unity(); case 27: return x27.parg()?*x27.parg():unity(); case 28: return x28.parg()?*x28.parg():unity(); case 29: return x29.parg()?*x29.parg():unity(); case 30: return x30.parg()?*x30.parg():unity();
    default: return unity();
  }
}

unity split(const std::wstring& s, const std::wstring& delim, meta::s_ll nmax)
{
    unity x; x.arr_init<utString>(0); if (nmax == 0) { return x; }
    if (delim.length()==0) { x.arr_append(s); }
    else if (s.length() > 0)
    {
        _t_wz pos, pos2; pos = 0;
        do
        {
            pos2=s.find(delim,pos);
            if (pos2 == nposw  || (nmax > 0 && x.arrsz() + 1 >= nmax)) { pos2=s.length(); x.arr_append(s.substr(pos, pos2 - pos)); }
              else { x.arr_append(s.substr(pos, pos2 - pos)); pos2+=delim.length(); if (pos2 >= s.length()) x.arr_append(L""); }
            pos=pos2;
        } while (pos < s.length());
    }
    return x;
}

unity split(const std::string& s, const std::string& delim, meta::s_ll nmax)
{
    unity x;  x.arr_init<utString>(0); if (nmax == 0) { return x; }
    if (delim.length()==0) { x.arr_append(s); }
    else if (s.length() > 0)
    {
        _t_wz pos, pos2;  pos = 0;
        do
        {
            pos2=s.find(delim,pos);
            if (pos2==nposc || (nmax > 0 && x.arrsz() + 1 >= nmax)) { pos2=s.length(); x.arr_append(s.substr(pos, pos2 - pos)); }
              else { x.arr_append(s.substr(pos, pos2 - pos)); pos2+=delim.length(); if (pos2 >= s.length()) x.arr_append(""); }
            pos=pos2;
        } while (pos < s.length());
    }
    return x;
}

std::vector<std::string> splitToVector(const std::string& s, const std::string& delim, meta::s_ll nmax)
{
  std::vector<std::string> x; if (nmax == 0) { return x; }
  if (delim.length()==0) { x.push_back(s); }
  else if (s.length()>0)
  {
      _t_sz pos, pos2; pos=0;
      do
      {
          pos2=s.find(delim,pos);
          if (pos2==nposc || (nmax > 0 && x.size() + 1 >= std::size_t(nmax))) { pos2=s.length(); x.push_back(s.substr(pos, pos2 - pos)); }
          else { x.push_back(s.substr(pos, pos2 - pos)); pos2+=delim.length(); if (pos2>=s.length()) { x.push_back(""); } }
          pos=pos2;
      } while(pos<s.length());
  }
  return x;
}

std::vector<std::wstring> splitToVector(const std::wstring& s, const std::wstring& delim, meta::s_ll nmax)
{
  std::vector<std::wstring> x; if (nmax == 0) { return x; }
  if (delim.length()==0) { x.push_back(s); }
  else if (s.length()>0)
  {
      _t_wz pos, pos2; pos=0;
      do
      {
          pos2=s.find(delim, pos);
          if (pos2 == nposw || (nmax > 0 && x.size() + 1 >= std::size_t(nmax))) { pos2=s.length(); x.push_back(s.substr(pos, pos2 - pos)); }
          else { x.push_back(s.substr(pos, pos2 - pos)); pos2+=delim.length(); if (pos2>=s.length()) { x.push_back(L""); } }
          pos=pos2;
      } while(pos<s.length());
  }
  return x;
}

std::string join(const unity& asrc, const std::string& delim)
{
    std::string s;
    if (asrc.isArray())
    {
        for(s_long ind=asrc.arrlb(); ind<=asrc.arrub(); ++ind)
        {
            if (ind==asrc.arrlb()) s=wsToBs(asrc.vstr(ind));
            else (s+=delim)+=wsToBs(asrc.vstr(ind));
        }
    }
    else s=wsToBs(asrc.vstr());
    return s;
}
std::wstring join(const unity& asrc, const std::wstring& delim)
{
    std::wstring s;
    if (asrc.isArray())
    {
        for(s_long ind=asrc.arrlb(); ind<=asrc.arrub(); ++ind)
        {
            if (ind==asrc.arrlb()) s=asrc.vstr(ind);
            else (s+=delim)+=asrc.vstr(ind);
        }
    }
    else s=asrc.vstr();
    return s;
}



std::wstring CStr3(const unity& x)
{
    if (x.isObject()) { return L"?object " + x.tname(); }
    if (x.isArray()) { return L"?array " + x.tname(true); }

    switch (x.utype())
    {
      case utEmpty: return std::wstring();
      case utInt: return x.vstr();
      case utFloat: return x.vstr();
      case utChar: { return x.ref<utChar>() ? L"True" : L"False"; }
      case utString: { return x.vstr(); }
      case utDate:
        {
          // Note: this loses fractional part of seconds,
          //  unlike conv_Date_String.
          _unitydate d = x.val<utDate>();
          std::ostringstream oss;
          oss << std::setw(4) << std::setfill('0') << std::right << d.d_year() << "-" << std::setw(2) <<d.d_month() << "-" << std::setw(2) <<d.d_day();
          if (d.f() != floor(d.f())) { oss << " " << std::setw(2) << d.d_hour() << ":" << std::setw(2) << d.d_minute() << ":" << std::setw(2) << d.d_second(); }
          return bsToWs(oss.str());
        }
      default: { return L"?value/" + x.tname(); }
    }
}

namespace
{
namespace date_calc
{
  const double _jds_eps = 5.e-5;
  inline meta::s_ll _n_sec(double jds) { return meta::s_ll(jds + (43200 + _jds_eps)); }
  inline s_long _n(double jds) { return s_long(_n_sec(jds) / 86400); }

  s_long _is_g(s_long n) { return n >= 2299161; }
  s_long _is_g(s_long y, s_long m, s_long d)
  {
    if (y > 1582) { return true; }
    if (y == 1582) { return m > 10 || (m == 10 && d >= 15); }
    return false;
  }
  meta::s_ll _n_any(s_long y_, s_long m_, s_long d_) // retval may be  > 32 bit
  {
    meta::s_ll year(y_), month(m_), day(d_);
    if (_is_g(y_, m_, d_)) { meta::s_ll a((14 - month) / 12), y(year + 4800 - a), m(month + 12 * a - 3); return day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045; }
      else { meta::s_ll a((14 - month) / 12), y(year + 4800 - a + (year < 0 ? 1 : 0)), m(month + 12 * a - 3); return day + (153 * m + 2) / 5 + 365 * y + y / 4 - 32083; }
  }
  meta::s_ll _jds_nhms(s_long n, s_long h, s_long mt, s_long s) { return meta::s_ll(n) * 86400 + (h - 12) * 3600 + mt * 60 + s; }

  void _ymd(s_long n, s_long* py, s_long* pm, s_long* pd)
  {
    if (_is_g(n))
    {
      meta::s_ll a(n + 32044), b((4 * a + 3) / 146097), c(a - (146097 * b) / 4), d((4 * c + 3) / 1461), e(c - (1461 * d) / 4), m((5 * e + 2) / 153);
      if (py) { *py = s_long(100 * b + d - 4800 + m / 10); } if (pm) { *pm = s_long(m + 3 - 12 * (m / 10)); } if (pd) { *pd = s_long(e - (153 * m + 2) / 5 + 1); }
    }
    else
    {
      meta::s_ll c(n + 32082), d((4 * c + 3) / 1461), e(c - (1461 * d) / 4), m((5 * e + 2) / 153);
      if (py) { s_long y = s_long(d - 4800 + m / 10); if (y < 1) { y -= 1; } *py = y; } if (pm) { *pm = s_long(m + 3 - 12 * (m / 10)); } if (pd) { *pd = s_long(e - (153 * m + 2) / 5 + 1); }
    }
  }
  void _hms(double jds, s_long* ph, s_long* pmt, s_long* ps) { meta::s_ll ss = _n_sec(jds); ss %= 86400; if (ph) { *ph = s_long(ss / 3600); } ss %= 3600; if (pmt) { *pmt = s_long(ss / 60); } if (ps) { *ps = s_long(ss % 60); } }
}
}
using namespace date_calc;

s_long _unitydate::jdn() const { return _n(_jds); }
meta::s_ll _unitydate::f_sec() const { return meta::s_ll(_jds + _jds_eps); }

_unitydate::_unitydate(s_long y, s_long m, s_long d, s_long h, s_long min, s_long s)
{
  if (m < 1) { m = 1; } else if (m > 12) { m = 12; }
  if (d < 1) { d = 1; } else if (d > 31) { d = 31; }
  if (h < 0) { h = 0; } else if (h > 23) { h = 23; }
  if (min < 0) { min = 0; } else if (min > 59) { min = 59; }
  if (s < 0) { s = 0; } else if (s > 59) { s = 59; }
  meta::s_ll n2 = _n_any(y, m, d);
  const meta::s_ll max = meta::s_long_mp - 1;
  if (n2 > max) { n2 = max; } else if (n2 < -max) { n2 = -max; }
  _jds = double(_jds_nhms(s_long(n2), h, min, s));
}

_unitydate& _unitydate::add_datetime(s_long dy, s_long dm, s_long dd, s_long dh, s_long dmin, s_long ds)
{
  const meta::s_ll max = meta::s_long_mp - 1;
  meta::s_ll __nsec = _n_sec(_jds);
  meta::s_ll n0 = __nsec / 86400; meta::s_ll ns0 = __nsec % 86400;
  s_long y_, m_, d_; _ymd(s_long(n0), &y_, &m_, &d_);
  meta::s_ll n2 = n0; meta::s_ll ns2 = ns0;
  if (dy || dm)
  {
    meta::s_ll y(y_), m(m_); if (y < 0) { y += 1; }
    y += dy; y += dm / 12; m += dm % 12; if (m < 1) { m += 12; y -= 1; } else if (m > 12) { m -= 12; y += 1; }
    if (y < 1) { y -= 1; }
    if (y > max) { y = max; } else if (y < -max) { y = -max; }
    n2 = _n_any(s_long(y), s_long(m), d_);
  }
  n2 += dd;
  if (dh || dmin || ds)
  {
    if (dh) { ns2 += meta::s_ll(dh) * 3600; }
    if (dmin) { ns2 += meta::s_ll(dmin) * 60; }
    if (ds) { ns2 += ds; }
    n2 += ns2 / 86400;
    ns2 %= 86400;
    if (ns2 < 0) { ns2 += 86400; n2 -= 1; }
  }
  if (n2 > max) { n2 = max; ns2 = 0; } else if (n2 < -max) { n2 = -max; ns2 = 0; }
  if (n2 != n0 || ns2 != ns0)
  {
    double f = _jds - std::floor(_jds + _jds_eps); if (f < 0.) { f = 0.; }
    set(double(n2 * 86400 + ns2 - 43200) + f);
  }
  return *this;
}

s_long _unitydate::d_year() const { s_long x; _ymd(_n(_jds), &x, 0, 0); return x; }
s_long _unitydate::d_month() const { s_long x; _ymd(_n(_jds), 0, &x, 0); return x; }
s_long _unitydate::d_day() const { s_long x; _ymd(_n(_jds), 0, 0, &x); return x; }
s_long _unitydate::d_day_of_week() const { s_long n = _n(_jds); n %= 7; if (n < 0) { n += 7; } n += 1; return n; }
s_long _unitydate::d_hour() const { meta::s_ll ss = _n_sec(_jds); ss %= 86400;  return s_long(ss / 3600); }
s_long _unitydate::d_minute() const { meta::s_ll ss = _n_sec(_jds); ss %= 3600;  return s_long(ss / 60); }
s_long _unitydate::d_second() const { meta::s_ll ss = _n_sec(_jds); return ss % 60; }
double _unitydate::d_ms() const { double f = _jds - std::floor(_jds + _jds_eps); if (f < 0.) { f = 0.; } return f * 1000.; }
void _unitydate::d_get(s_long* py, s_long* pm, s_long* pd, s_long* ph, s_long* pmin, s_long* ps) const
{
  if (py || pm || pd) { _ymd(_n(_jds), py, pm, pd); }
  if (ph || pmin || ps) { _hms(_jds, ph, pmin, ps); }
}

_unitydate d_datetime(s_long y, s_long m, s_long d, s_long h, s_long mt, s_long s) { return _unitydate(y, m, d, h, mt, s); }
_unitydate d_time(s_long h, s_long mt, s_long s) { return _unitydate(0, 0, 0, h, mt, s); }





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Private utilities

    // ceil(x1/x2)
meta::s_ll ceil_div_long(meta::s_ll x1, meta::s_ll x2) { return (x1%x2 ? 1 : 0) + x1/x2; }
std::string replicate_string(const std::string& s, s_long n) { std::string s2; for(s_long i=0; i<n; ++i) { s2+=s; } return s2; }
std::wstring replicate_string(const std::wstring& s, s_long n) { std::wstring s2; for(s_long i=0; i<n; ++i) { s2+=s; } return s2; }

std::string rtrim1(const std::string& s, const std::string& swhat)
{
    if (swhat.length()<=s.length())
      { if (s.substr(s.length()-swhat.length(),swhat.length()) == swhat) { return s.substr(0, s.length() - swhat.length()); } else { return s; } }
      else { return s; }
}

  // NOTE l/u case functions work with current locale set by the client.
std::string _lcase(const std::string& s) { std::string s2; for(_t_sz pos=0; pos < s.length(); ++pos) { s2 += char(tolower(s[pos])); } return s2; }
std::string _ucase(const std::string& s) { std::string s2; for(_t_sz pos=0; pos < s.length(); ++pos) { s2 += char(toupper(s[pos])); } return s2; }
std::wstring _lcase(const std::wstring& s) { std::wstring s2; for(_t_wz pos=0; pos < s.length(); ++pos) { s2 += wchar_t(std::towlower(s[pos])); } return s2; }
std::wstring _ucase(const std::wstring& s) { std::wstring s2; for(_t_wz pos=0; pos < s.length(); ++pos) { s2 += wchar_t(std::towupper(s[pos])); } return s2; }


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// paramline
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const std::string paramline::cpterm = "; ";
const std::string paramline::ceterm = " = ";
const std::string paramline::cvterm = "|";

const std::wstring paramline::wpterm = L"; ";
const std::wstring paramline::weterm = L" = ";
const std::wstring paramline::wvterm = L"|";

unity& paramline::decode1v(const std::wstring& ssrc0, unity& dest)
{
  dest.clear();
  try
  {
    std::wstring s; s_long flags(0);
    x_replace4(ssrc0, s, flags);
    _t_wz pos0 = s.find(L";"); if (pos0 != nposw) { s.resize(pos0); }
    if (dest.isLocal())
    {
      dest = _trim(s, L" ");
      x_decode1v(dest, false, flags);
    }
    else
    {
      unity temp = _trim(s, L" ");
      x_decode1v(temp, false, flags);
      dest = temp;
    }
  }
  catch(_XUBase&) { throw; }
  catch(std::exception& e) { throw XUExec("decode1v.1", e.what(), ssrc0); }
  catch(...) { throw XUExec("decode1v.2", ssrc0); }
  return dest;
}

unity& paramline::decode(const std::wstring& ssrc0, unity& mh, bool useMap, const std::wstring& pterm_0)
{
  std::wstring src;
  try
  {
      std::wstring pterm = pterm_0.length() == 0 ? wCRLF : pterm_0;
      if (useMap) { mh.map_clear(false); } else { mh.hash_clear(false); }

      std::wstring s, n; unity v;
      s_long flags(0);
      x_replace4(ssrc0, src, flags);
      src = replace(src, L";", pterm);
      _t_wz pos0(0), pos2(0), pos_eq(0);
      while(pos0 < src.length())
      {
          pos2 = src.find(pterm,pos0);
          if (pos2 == nposw) { pos2 = src.length(); }
          s = src.substr(pos0, pos2 - pos0);

          pos_eq = s.find(L"=");
          if (pos_eq != nposw)
          {
              n = _trim(s.substr(0, pos_eq), L" ");
              v = _trim(s.substr(pos_eq + 1), L" ");
              x_replace2a(n, flags);
              x_decode1v(v, false, flags);
              if (v.objPtr<unity>()) v.objPtr<unity>()->u_name_set(n);
              mh.assoc_set(n, v, true);
          }
          else
          {
              n = _trim(s, L" ");
              if (n.length() > 0)
              {
                x_replace2a(n, flags);
                mh.assoc_set(n, unity(), true);
              }
          }
          pos0 = pos2 + pterm.length();
      }
  }
  catch(_XUBase&) { throw; }
  catch(std::exception& e) { throw XUExec("decode.1", e.what(), src); }
  catch(...) { throw XUExec("decode.2", src); }
  return mh;
}

unity& paramline::decode_tree(const std::wstring& ssrc0, unity& mh, s_long flags, const std::wstring& pterm_0)
{
  try {
    std::wstring pterm2 = pterm_0.length() == 0 ? wCRLF : pterm_0;
    bool b_map = bool(flags & 0x1); bool b_clear = (flags & 0x4) == 0; bool b_keep = (flags & 0x2) == 0; bool b_skipslc = bool(flags & 0x8); bool b_convcr(flags & 0x10);
    if (b_clear || !mh.isAssoc()) { if (b_map) { mh.map_clear(false); } else { mh.hash_clear(false); } }
    const unity k_empty(L"");

    std::wstring s2;
    if (b_convcr)
    {
      s2.reserve(ssrc0.size());
      for (_t_wz pos = 0; pos < ssrc0.size(); ++pos)
      {
        wchar_t c = ssrc0[pos];
        if (c == L'\r')
        {
          _t_wz pos2 = pos + 1;
          if (pos2 < ssrc0.size() && ssrc0[pos2] == L'\n') { pos = pos2; }
          s2 += L"\r\n";
        }
        else if (c == L'\n')
        {
          _t_wz pos2 = pos + 1;
          if (pos2 < ssrc0.size() && ssrc0[pos2] == L'\r') { pos = pos2; }
          s2 += L"\r\n";
        }
        else { s2 += c; }
      }
    }

    _t_wz pos(0); const std::wstring& ssrc = b_convcr ? s2 : ssrc0;
    while (pos < ssrc.length())
    {
      _t_wz pos2 = ssrc.find(pterm2, pos); if (pos2 == nposw) { pos2 = ssrc.length(); }
      if (pos2 > pos)
      {
        bool b_skip = false;
        if (b_skipslc) { _t_wz pos3 = ssrc.find_first_not_of(L"\t ", pos); if (pos3 != nposw && pos3 + 2 <= pos2 && ssrc[pos3] == '/' && ssrc[pos3 + 1] == '/') { b_skip = true; } }
        if (!b_skip)
        {
          unity h; paramline().decode(ssrc.substr(pos, pos2 - pos), h, false, pterm2);
          unity* node = &mh;
          const unity& k = h.u_has(k_empty, 6) ? h[k_empty] : k_empty;
          if (k.utype() == utUnityArray)
          {
            for (int j = k.arrlb(); j <= k.arrub(); ++j)
            {
              const unity& k2 = k.ref<utUnity>(j);
              node->assoc_set(k2, unity(), true);
              node = &(*node)[k2];
              if (node->isAssoc()) {}
                else if (node->isEmpty() || !b_keep) { node->u_clear(b_map ? utMap : utHash); }
                else { node = 0; break; }
            }
          }
          if (node) { paramline().merge(*node, h, b_keep); }
        }
      }
      pos = pos2 + pterm2.length();
    }
  }
  catch(_XUBase&) { throw; }
  catch(std::exception& e) { throw XUExec("decode_tree.1", e.what(), ssrc0); }
  catch(...) { throw XUExec("decode_tree.2", ssrc0); }
  return mh;
}

std::wstring& paramline::encode(const unity& mh, std::wstring& sdest, const std::wstring& pterm_0)
{
  try
  {
    sdest.clear(); if (!mh.isAssoc() || mh.assocS_c() == 0) { return sdest; }
    std::wstring s1, s2; std::wstring pterm = pterm_0.length() == 0 ? wpterm : pterm_0;
    s_long pos = mh.assocl_first();
    while (pos != mh.assocl_noel())
    {
      x_encode1(mh.assocl_key(pos), s1, true, false); x_encode1(mh.assocl_c(pos), s2, false, false);
      s1 += weterm; s1 += s2; x_replace2a(s1, 1);
      pos = mh.assocl_next(pos);
      if (pos != mh.assocl_noel()) { s1 += pterm; }
      sdest += s1;
    }
    return sdest;
  }
  catch(_XUBase&) { throw; }
  catch(std::exception& e) { throw XUExec("encode.1", e.what(), mh.vflstr()); }
  catch(...) { throw XUExec("encode.2", mh.vflstr()); }
}

std::wstring paramline::encode1n(const unity& name)
{
  try { std::wstring s; x_encode1(name, s, true, false); x_replace2a(s, 1); return s; }
  catch(_XUBase&) { throw; }
  catch(std::exception& e) { throw XUExec("encode1n.1", e.what(), name.vflstr()); }
  catch(...) { throw XUExec("encode1n.2", name.vflstr()); }
}

std::wstring& paramline::encode1v(const unity& value, std::wstring& s)
{
  try { x_encode1(value, s, false, false); x_replace2a(s, 1); return s; }
  catch(_XUBase&) { throw; }
  catch(std::exception& e) { throw XUExec("encode1v.1", e.what(), "Failed to encode val.", value.utype()); }
  catch(...) { throw XUExec("encode1v.2", "Failed to encode val.", value.utype()); }
}

void paramline::x_encode_branch(const unity& mh, const std::wstring& path, std::wstring& sdest, hashx<const unity*, int>& hstop, const std::wstring& pterm, const std::wstring& pterm2, const std::wstring& pathpfx)
{
    // return if not assoc or empty assoc
    // append path if not empty
    // append non-branch pairs, collect branch pairs, skip stopped
    // append term2
    // for each branch, call x_encode_branch recursively with path + string(branch key)
  if (!(mh.isAssoc() && mh.assocS_c())) { return; }
  bool bpath = false;
  vec2_t<s_long> ibr;
  for (s_long ind = mh.assocl_first(); ind != mh.assocl_noel(); ind = mh.assocl_next(ind))
  {
    const unity& k = mh.assocl_key(ind);
      s_long res = hstop.insert(&k);
      if (res == 0) { continue; }
      if (res != 1) { throw XUExec("x_encode_branch.1"); }
    const unity& v = mh.assocl_c(ind);
    if (v.isAssoc()) { ibr.push_back(ind); }
    else
    {
      if (!bpath) { bpath = true; if (path.length()) { sdest += path; } }
      std::wstring s1, s2;
      x_encode1(k, s1, true, false); x_encode1(k, s2, false, false);
      s1 += weterm; s1 += s2; x_replace2a(s1, 1);
      sdest += pterm; sdest += s1;
    }
  }
  if (bpath) { sdest += pterm2; }
  for (s_long i = 0; i < ibr.n(); ++i)
  {
    const unity& k = mh.assocl_key(ibr[i]);
    const unity& v = mh.assocl_c(ibr[i]);
    std::wstring s1, sp;
      x_encode1(k, s1, true, false); x_replace2a(s1, 1);
      sp = path; if (sp.empty()) { sp += pathpfx; } sp += wvterm; sp += s1; s1.clear();
    x_encode_branch(v, sp, sdest, hstop, pterm, pterm2, pathpfx);
  }
}

std::wstring& paramline::encode_tree(const unity& mh, std::wstring& sdest, const std::wstring& pterm_0, const std::wstring& pterm2_0)
{
  try
  {
    sdest.clear(); if (!mh.isAssoc() || mh.assocS_c() == 0) { return sdest; }
    hashx<const unity*, int> hstop;
    std::wstring pterm = pterm_0.length() == 0 ? wpterm : pterm_0;
    std::wstring pterm2 = pterm2_0.length() == 0 ? wCRLF : pterm2_0;
    std::wstring pathpfx = _trim(weterm, L" ", true, true);
    x_encode_branch(mh, L"", sdest, hstop, pterm, pterm2, pathpfx);
    return sdest;
  }
  catch(_XUBase&) { throw; }
  catch(std::exception& e) { throw XUExec("encode_tree.1", e.what(), mh.vflstr()); }
  catch(...) { throw XUExec("encode_tree.2", mh.vflstr()); }
}

unity& paramline::merge(unity& set1, const unity& set2, bool keep_first)
{
  if (!(set1.compatibility() > 0 && set2.compatibility() > 0)) { throw XUExec("merge.4", set1.utype(), set1.compatibility(), set2.utype(), set2.compatibility()); }
  if (set1.isMap())
  {
    _mx& rm1 = *static_cast<_mx*>(set1._pmap());
    if (set2.isMap())
      { unity::t_map& rm2 = *set2._pmap(); for(s_long ia = 0; ia < rm2.n(); ++ia) { rm1.__append(false, rm2(ia)->k, rm2(ia)->v, keep_first); } }
    else if (set2.isHash())
      { unity::t_hash& rh2 = *set2._phash(); for(s_long ia = 0; ia < rh2.n(); ++ia) { rm1.__append(false, rh2(ia)->k, rh2(ia)->v, keep_first); } }
    else { throw XUTypeMismatch("merge.1", "set2", set2.utype(), "utMap or utHash"); }
  }
  else if (set1.isHash())
  {
    unity& rh1 = set1;
    if (set2.isMap())
      { unity::t_map& rm2 = *set2._pmap(); for(s_long ia = 0; ia < rm2.n(); ++ia) { rh1.hash_set(rm2(ia)->k, rm2(ia)->v, keep_first); } }
    else if (set2.isHash())
      { unity::t_hash& rh2 = *set2._phash(); for(s_long ia = 0; ia < rh2.n(); ++ia) { rh1.hash_set(rh2(ia)->k, rh2(ia)->v, keep_first); } }
    else { throw XUTypeMismatch("merge.2", "set2", set2.utype(), "utMap or utHash"); }
  }
  else { throw XUTypeMismatch("merge.3", "set1", set1.utype(), "utMap or utHash"); }
  return set1;
}

unity& paramline::merge(unity& set1, const std::wstring& set2_pl, bool keep_first)
  { return merge(set1, decode(set2_pl, true), keep_first); }

unity paramline::list_m ( rA x1, rA x2, rA x3, rA x4, rA x5, rA x6, rA x7, rA x8, rA x9, rA x10, rA x11, rA x12, rA x13, rA x14, rA x15, rA x16, rA x17, rA x18, rA x19, rA x20, rA x21, rA x22, rA x23, rA x24, rA x25, rA x26, rA x27, rA x28, rA x29, rA x30, rA x31, rA x32, rA x33, rA x34, rA x35, rA x36, rA x37, rA x38, rA x39, rA x40, rA x41, rA x42, rA x43, rA x44, rA x45, rA x46, rA x47, rA x48, rA x49, rA x50, rA x51, rA x52, rA x53, rA x54, rA x55, rA x56, rA x57, rA x58, rA x59, rA x60 )
{
  unity mdest; vec2_t<const unity*> a; mdest.map_clear(true);
  do { if (x1.parg()) a.el_append(x1.parg()); else break; if (x2.parg()) a.el_append(x2.parg()); else break; if (x3.parg()) a.el_append(x3.parg()); else break; if (x4.parg()) a.el_append(x4.parg()); else break; if (x5.parg()) a.el_append(x5.parg()); else break; if (x6.parg()) a.el_append(x6.parg()); else break; if (x7.parg()) a.el_append(x7.parg()); else break; if (x8.parg()) a.el_append(x8.parg()); else break; if (x9.parg()) a.el_append(x9.parg()); else break; if (x10.parg()) a.el_append(x10.parg()); else break; if (x11.parg()) a.el_append(x11.parg()); else break; if (x12.parg()) a.el_append(x12.parg()); else break; if (x13.parg()) a.el_append(x13.parg()); else break; if (x14.parg()) a.el_append(x14.parg()); else break; if (x15.parg()) a.el_append(x15.parg()); else break; if (x16.parg()) a.el_append(x16.parg()); else break; if (x17.parg()) a.el_append(x17.parg()); else break; if (x18.parg()) a.el_append(x18.parg()); else break; if (x19.parg()) a.el_append(x19.parg()); else break; if (x20.parg()) a.el_append(x20.parg()); else break; if (x21.parg()) a.el_append(x21.parg()); else break; if (x22.parg()) a.el_append(x22.parg()); else break; if (x23.parg()) a.el_append(x23.parg()); else break; if (x24.parg()) a.el_append(x24.parg()); else break; if (x25.parg()) a.el_append(x25.parg()); else break; if (x26.parg()) a.el_append(x26.parg()); else break; if (x27.parg()) a.el_append(x27.parg()); else break; if (x28.parg()) a.el_append(x28.parg()); else break; if (x29.parg()) a.el_append(x29.parg()); else break; if (x30.parg()) a.el_append(x30.parg()); else break; if (x31.parg()) a.el_append(x31.parg()); else break; if (x32.parg()) a.el_append(x32.parg()); else break; if (x33.parg()) a.el_append(x33.parg()); else break; if (x34.parg()) a.el_append(x34.parg()); else break; if (x35.parg()) a.el_append(x35.parg()); else break; if (x36.parg()) a.el_append(x36.parg()); else break; if (x37.parg()) a.el_append(x37.parg()); else break; if (x38.parg()) a.el_append(x38.parg()); else break; if (x39.parg()) a.el_append(x39.parg()); else break; if (x40.parg()) a.el_append(x40.parg()); else break; if (x41.parg()) a.el_append(x41.parg()); else break; if (x42.parg()) a.el_append(x42.parg()); else break; if (x43.parg()) a.el_append(x43.parg()); else break; if (x44.parg()) a.el_append(x44.parg()); else break; if (x45.parg()) a.el_append(x45.parg()); else break; if (x46.parg()) a.el_append(x46.parg()); else break; if (x47.parg()) a.el_append(x47.parg()); else break; if (x48.parg()) a.el_append(x48.parg()); else break; if (x49.parg()) a.el_append(x49.parg()); else break; if (x50.parg()) a.el_append(x50.parg()); else break; if (x51.parg()) a.el_append(x51.parg()); else break; if (x52.parg()) a.el_append(x52.parg()); else break; if (x53.parg()) a.el_append(x53.parg()); else break; if (x54.parg()) a.el_append(x54.parg()); else break; if (x55.parg()) a.el_append(x55.parg()); else break; if (x56.parg()) a.el_append(x56.parg()); else break; if (x57.parg()) a.el_append(x57.parg()); else break; if (x58.parg()) a.el_append(x58.parg()); else break; if (x59.parg()) a.el_append(x59.parg()); else break; if (x60.parg()) a.el_append(x60.parg()); else break; } while (false);
  for(s_long ind = 0; ind < a.n(); ind += 2) { mdest.map_append(*a[ind], ind + 1 < a.n() ? *a[ind+1] : unity(), true); }
  return mdest;
}
unity paramline::list_mx ( s_long fk, rA x1, rA x2, rA x3, rA x4, rA x5, rA x6, rA x7, rA x8, rA x9, rA x10, rA x11, rA x12, rA x13, rA x14, rA x15, rA x16, rA x17, rA x18, rA x19, rA x20, rA x21, rA x22, rA x23, rA x24, rA x25, rA x26, rA x27, rA x28, rA x29, rA x30, rA x31, rA x32, rA x33, rA x34, rA x35, rA x36, rA x37, rA x38, rA x39, rA x40, rA x41, rA x42, rA x43, rA x44, rA x45, rA x46, rA x47, rA x48, rA x49, rA x50, rA x51, rA x52, rA x53, rA x54, rA x55, rA x56, rA x57, rA x58, rA x59, rA x60 )
{
  unity mdest; vec2_t<const unity*> a; mdest.map_clear(true); mdest.mapFlags_set(-1, fk);
  do { if (x1.parg()) a.el_append(x1.parg()); else break; if (x2.parg()) a.el_append(x2.parg()); else break; if (x3.parg()) a.el_append(x3.parg()); else break; if (x4.parg()) a.el_append(x4.parg()); else break; if (x5.parg()) a.el_append(x5.parg()); else break; if (x6.parg()) a.el_append(x6.parg()); else break; if (x7.parg()) a.el_append(x7.parg()); else break; if (x8.parg()) a.el_append(x8.parg()); else break; if (x9.parg()) a.el_append(x9.parg()); else break; if (x10.parg()) a.el_append(x10.parg()); else break; if (x11.parg()) a.el_append(x11.parg()); else break; if (x12.parg()) a.el_append(x12.parg()); else break; if (x13.parg()) a.el_append(x13.parg()); else break; if (x14.parg()) a.el_append(x14.parg()); else break; if (x15.parg()) a.el_append(x15.parg()); else break; if (x16.parg()) a.el_append(x16.parg()); else break; if (x17.parg()) a.el_append(x17.parg()); else break; if (x18.parg()) a.el_append(x18.parg()); else break; if (x19.parg()) a.el_append(x19.parg()); else break; if (x20.parg()) a.el_append(x20.parg()); else break; if (x21.parg()) a.el_append(x21.parg()); else break; if (x22.parg()) a.el_append(x22.parg()); else break; if (x23.parg()) a.el_append(x23.parg()); else break; if (x24.parg()) a.el_append(x24.parg()); else break; if (x25.parg()) a.el_append(x25.parg()); else break; if (x26.parg()) a.el_append(x26.parg()); else break; if (x27.parg()) a.el_append(x27.parg()); else break; if (x28.parg()) a.el_append(x28.parg()); else break; if (x29.parg()) a.el_append(x29.parg()); else break; if (x30.parg()) a.el_append(x30.parg()); else break; if (x31.parg()) a.el_append(x31.parg()); else break; if (x32.parg()) a.el_append(x32.parg()); else break; if (x33.parg()) a.el_append(x33.parg()); else break; if (x34.parg()) a.el_append(x34.parg()); else break; if (x35.parg()) a.el_append(x35.parg()); else break; if (x36.parg()) a.el_append(x36.parg()); else break; if (x37.parg()) a.el_append(x37.parg()); else break; if (x38.parg()) a.el_append(x38.parg()); else break; if (x39.parg()) a.el_append(x39.parg()); else break; if (x40.parg()) a.el_append(x40.parg()); else break; if (x41.parg()) a.el_append(x41.parg()); else break; if (x42.parg()) a.el_append(x42.parg()); else break; if (x43.parg()) a.el_append(x43.parg()); else break; if (x44.parg()) a.el_append(x44.parg()); else break; if (x45.parg()) a.el_append(x45.parg()); else break; if (x46.parg()) a.el_append(x46.parg()); else break; if (x47.parg()) a.el_append(x47.parg()); else break; if (x48.parg()) a.el_append(x48.parg()); else break; if (x49.parg()) a.el_append(x49.parg()); else break; if (x50.parg()) a.el_append(x50.parg()); else break; if (x51.parg()) a.el_append(x51.parg()); else break; if (x52.parg()) a.el_append(x52.parg()); else break; if (x53.parg()) a.el_append(x53.parg()); else break; if (x54.parg()) a.el_append(x54.parg()); else break; if (x55.parg()) a.el_append(x55.parg()); else break; if (x56.parg()) a.el_append(x56.parg()); else break; if (x57.parg()) a.el_append(x57.parg()); else break; if (x58.parg()) a.el_append(x58.parg()); else break; if (x59.parg()) a.el_append(x59.parg()); else break; if (x60.parg()) a.el_append(x60.parg()); else break; } while (false);
  for(s_long ind = 0; ind < a.n(); ind += 2) { mdest.map_append(*a[ind], ind + 1 < a.n() ? *a[ind+1] : unity(), true); }
  return mdest;
}
unity paramline::list_h ( rA x1, rA x2, rA x3, rA x4, rA x5, rA x6, rA x7, rA x8, rA x9, rA x10, rA x11, rA x12, rA x13, rA x14, rA x15, rA x16, rA x17, rA x18, rA x19, rA x20, rA x21, rA x22, rA x23, rA x24, rA x25, rA x26, rA x27, rA x28, rA x29, rA x30, rA x31, rA x32, rA x33, rA x34, rA x35, rA x36, rA x37, rA x38, rA x39, rA x40, rA x41, rA x42, rA x43, rA x44, rA x45, rA x46, rA x47, rA x48, rA x49, rA x50, rA x51, rA x52, rA x53, rA x54, rA x55, rA x56, rA x57, rA x58, rA x59, rA x60 )
{
  unity hdest; vec2_t<const unity*> a; hdest.hash_clear(true);
  do { if (x1.parg()) a.el_append(x1.parg()); else break; if (x2.parg()) a.el_append(x2.parg()); else break; if (x3.parg()) a.el_append(x3.parg()); else break; if (x4.parg()) a.el_append(x4.parg()); else break; if (x5.parg()) a.el_append(x5.parg()); else break; if (x6.parg()) a.el_append(x6.parg()); else break; if (x7.parg()) a.el_append(x7.parg()); else break; if (x8.parg()) a.el_append(x8.parg()); else break; if (x9.parg()) a.el_append(x9.parg()); else break; if (x10.parg()) a.el_append(x10.parg()); else break; if (x11.parg()) a.el_append(x11.parg()); else break; if (x12.parg()) a.el_append(x12.parg()); else break; if (x13.parg()) a.el_append(x13.parg()); else break; if (x14.parg()) a.el_append(x14.parg()); else break; if (x15.parg()) a.el_append(x15.parg()); else break; if (x16.parg()) a.el_append(x16.parg()); else break; if (x17.parg()) a.el_append(x17.parg()); else break; if (x18.parg()) a.el_append(x18.parg()); else break; if (x19.parg()) a.el_append(x19.parg()); else break; if (x20.parg()) a.el_append(x20.parg()); else break; if (x21.parg()) a.el_append(x21.parg()); else break; if (x22.parg()) a.el_append(x22.parg()); else break; if (x23.parg()) a.el_append(x23.parg()); else break; if (x24.parg()) a.el_append(x24.parg()); else break; if (x25.parg()) a.el_append(x25.parg()); else break; if (x26.parg()) a.el_append(x26.parg()); else break; if (x27.parg()) a.el_append(x27.parg()); else break; if (x28.parg()) a.el_append(x28.parg()); else break; if (x29.parg()) a.el_append(x29.parg()); else break; if (x30.parg()) a.el_append(x30.parg()); else break; if (x31.parg()) a.el_append(x31.parg()); else break; if (x32.parg()) a.el_append(x32.parg()); else break; if (x33.parg()) a.el_append(x33.parg()); else break; if (x34.parg()) a.el_append(x34.parg()); else break; if (x35.parg()) a.el_append(x35.parg()); else break; if (x36.parg()) a.el_append(x36.parg()); else break; if (x37.parg()) a.el_append(x37.parg()); else break; if (x38.parg()) a.el_append(x38.parg()); else break; if (x39.parg()) a.el_append(x39.parg()); else break; if (x40.parg()) a.el_append(x40.parg()); else break; if (x41.parg()) a.el_append(x41.parg()); else break; if (x42.parg()) a.el_append(x42.parg()); else break; if (x43.parg()) a.el_append(x43.parg()); else break; if (x44.parg()) a.el_append(x44.parg()); else break; if (x45.parg()) a.el_append(x45.parg()); else break; if (x46.parg()) a.el_append(x46.parg()); else break; if (x47.parg()) a.el_append(x47.parg()); else break; if (x48.parg()) a.el_append(x48.parg()); else break; if (x49.parg()) a.el_append(x49.parg()); else break; if (x50.parg()) a.el_append(x50.parg()); else break; if (x51.parg()) a.el_append(x51.parg()); else break; if (x52.parg()) a.el_append(x52.parg()); else break; if (x53.parg()) a.el_append(x53.parg()); else break; if (x54.parg()) a.el_append(x54.parg()); else break; if (x55.parg()) a.el_append(x55.parg()); else break; if (x56.parg()) a.el_append(x56.parg()); else break; if (x57.parg()) a.el_append(x57.parg()); else break; if (x58.parg()) a.el_append(x58.parg()); else break; if (x59.parg()) a.el_append(x59.parg()); else break; if (x60.parg()) a.el_append(x60.parg()); else break; } while (false);
  for(s_long ind = 0; ind < a.n(); ind += 2) { hdest.hash_set(*a[ind], ind + 1 < a.n() ? *a[ind+1] : unity(), true); }
  return hdest;
}
unity paramline::list_hx ( s_long fk, rA x1, rA x2, rA x3, rA x4, rA x5, rA x6, rA x7, rA x8, rA x9, rA x10, rA x11, rA x12, rA x13, rA x14, rA x15, rA x16, rA x17, rA x18, rA x19, rA x20, rA x21, rA x22, rA x23, rA x24, rA x25, rA x26, rA x27, rA x28, rA x29, rA x30, rA x31, rA x32, rA x33, rA x34, rA x35, rA x36, rA x37, rA x38, rA x39, rA x40, rA x41, rA x42, rA x43, rA x44, rA x45, rA x46, rA x47, rA x48, rA x49, rA x50, rA x51, rA x52, rA x53, rA x54, rA x55, rA x56, rA x57, rA x58, rA x59, rA x60 )
{
  unity hdest; vec2_t<const unity*> a; hdest.hash_clear(true); hdest.hashFlags_set(-1, fk);
  do { if (x1.parg()) a.el_append(x1.parg()); else break; if (x2.parg()) a.el_append(x2.parg()); else break; if (x3.parg()) a.el_append(x3.parg()); else break; if (x4.parg()) a.el_append(x4.parg()); else break; if (x5.parg()) a.el_append(x5.parg()); else break; if (x6.parg()) a.el_append(x6.parg()); else break; if (x7.parg()) a.el_append(x7.parg()); else break; if (x8.parg()) a.el_append(x8.parg()); else break; if (x9.parg()) a.el_append(x9.parg()); else break; if (x10.parg()) a.el_append(x10.parg()); else break; if (x11.parg()) a.el_append(x11.parg()); else break; if (x12.parg()) a.el_append(x12.parg()); else break; if (x13.parg()) a.el_append(x13.parg()); else break; if (x14.parg()) a.el_append(x14.parg()); else break; if (x15.parg()) a.el_append(x15.parg()); else break; if (x16.parg()) a.el_append(x16.parg()); else break; if (x17.parg()) a.el_append(x17.parg()); else break; if (x18.parg()) a.el_append(x18.parg()); else break; if (x19.parg()) a.el_append(x19.parg()); else break; if (x20.parg()) a.el_append(x20.parg()); else break; if (x21.parg()) a.el_append(x21.parg()); else break; if (x22.parg()) a.el_append(x22.parg()); else break; if (x23.parg()) a.el_append(x23.parg()); else break; if (x24.parg()) a.el_append(x24.parg()); else break; if (x25.parg()) a.el_append(x25.parg()); else break; if (x26.parg()) a.el_append(x26.parg()); else break; if (x27.parg()) a.el_append(x27.parg()); else break; if (x28.parg()) a.el_append(x28.parg()); else break; if (x29.parg()) a.el_append(x29.parg()); else break; if (x30.parg()) a.el_append(x30.parg()); else break; if (x31.parg()) a.el_append(x31.parg()); else break; if (x32.parg()) a.el_append(x32.parg()); else break; if (x33.parg()) a.el_append(x33.parg()); else break; if (x34.parg()) a.el_append(x34.parg()); else break; if (x35.parg()) a.el_append(x35.parg()); else break; if (x36.parg()) a.el_append(x36.parg()); else break; if (x37.parg()) a.el_append(x37.parg()); else break; if (x38.parg()) a.el_append(x38.parg()); else break; if (x39.parg()) a.el_append(x39.parg()); else break; if (x40.parg()) a.el_append(x40.parg()); else break; if (x41.parg()) a.el_append(x41.parg()); else break; if (x42.parg()) a.el_append(x42.parg()); else break; if (x43.parg()) a.el_append(x43.parg()); else break; if (x44.parg()) a.el_append(x44.parg()); else break; if (x45.parg()) a.el_append(x45.parg()); else break; if (x46.parg()) a.el_append(x46.parg()); else break; if (x47.parg()) a.el_append(x47.parg()); else break; if (x48.parg()) a.el_append(x48.parg()); else break; if (x49.parg()) a.el_append(x49.parg()); else break; if (x50.parg()) a.el_append(x50.parg()); else break; if (x51.parg()) a.el_append(x51.parg()); else break; if (x52.parg()) a.el_append(x52.parg()); else break; if (x53.parg()) a.el_append(x53.parg()); else break; if (x54.parg()) a.el_append(x54.parg()); else break; if (x55.parg()) a.el_append(x55.parg()); else break; if (x56.parg()) a.el_append(x56.parg()); else break; if (x57.parg()) a.el_append(x57.parg()); else break; if (x58.parg()) a.el_append(x58.parg()); else break; if (x59.parg()) a.el_append(x59.parg()); else break; if (x60.parg()) a.el_append(x60.parg()); else break; } while (false);
  for(s_long ind = 0; ind < a.n(); ind += 2) { hdest.hash_set(*a[ind], ind + 1 < a.n() ? *a[ind+1] : unity(), true); }
  return hdest;
}

void paramline::_list_m_set_u ( unity& mdest, rA x1, rA x2, rA x3, rA x4, rA x5, rA x6, rA x7, rA x8, rA x9, rA x10, rA x11, rA x12, rA x13, rA x14, rA x15, rA x16, rA x17, rA x18, rA x19, rA x20, rA x21, rA x22, rA x23, rA x24, rA x25, rA x26, rA x27, rA x28, rA x29, rA x30, rA x31, rA x32, rA x33, rA x34, rA x35, rA x36, rA x37, rA x38, rA x39, rA x40, rA x41, rA x42, rA x43, rA x44, rA x45, rA x46, rA x47, rA x48, rA x49, rA x50, rA x51, rA x52, rA x53, rA x54, rA x55, rA x56, rA x57, rA x58, rA x59, rA x60 )
{
  vec2_t<const unity*> a; mdest.map_clear(true);
  do { if (x1.parg()) a.el_append(x1.parg()); else break; if (x2.parg()) a.el_append(x2.parg()); else break; if (x3.parg()) a.el_append(x3.parg()); else break; if (x4.parg()) a.el_append(x4.parg()); else break; if (x5.parg()) a.el_append(x5.parg()); else break; if (x6.parg()) a.el_append(x6.parg()); else break; if (x7.parg()) a.el_append(x7.parg()); else break; if (x8.parg()) a.el_append(x8.parg()); else break; if (x9.parg()) a.el_append(x9.parg()); else break; if (x10.parg()) a.el_append(x10.parg()); else break; if (x11.parg()) a.el_append(x11.parg()); else break; if (x12.parg()) a.el_append(x12.parg()); else break; if (x13.parg()) a.el_append(x13.parg()); else break; if (x14.parg()) a.el_append(x14.parg()); else break; if (x15.parg()) a.el_append(x15.parg()); else break; if (x16.parg()) a.el_append(x16.parg()); else break; if (x17.parg()) a.el_append(x17.parg()); else break; if (x18.parg()) a.el_append(x18.parg()); else break; if (x19.parg()) a.el_append(x19.parg()); else break; if (x20.parg()) a.el_append(x20.parg()); else break; if (x21.parg()) a.el_append(x21.parg()); else break; if (x22.parg()) a.el_append(x22.parg()); else break; if (x23.parg()) a.el_append(x23.parg()); else break; if (x24.parg()) a.el_append(x24.parg()); else break; if (x25.parg()) a.el_append(x25.parg()); else break; if (x26.parg()) a.el_append(x26.parg()); else break; if (x27.parg()) a.el_append(x27.parg()); else break; if (x28.parg()) a.el_append(x28.parg()); else break; if (x29.parg()) a.el_append(x29.parg()); else break; if (x30.parg()) a.el_append(x30.parg()); else break; if (x31.parg()) a.el_append(x31.parg()); else break; if (x32.parg()) a.el_append(x32.parg()); else break; if (x33.parg()) a.el_append(x33.parg()); else break; if (x34.parg()) a.el_append(x34.parg()); else break; if (x35.parg()) a.el_append(x35.parg()); else break; if (x36.parg()) a.el_append(x36.parg()); else break; if (x37.parg()) a.el_append(x37.parg()); else break; if (x38.parg()) a.el_append(x38.parg()); else break; if (x39.parg()) a.el_append(x39.parg()); else break; if (x40.parg()) a.el_append(x40.parg()); else break; if (x41.parg()) a.el_append(x41.parg()); else break; if (x42.parg()) a.el_append(x42.parg()); else break; if (x43.parg()) a.el_append(x43.parg()); else break; if (x44.parg()) a.el_append(x44.parg()); else break; if (x45.parg()) a.el_append(x45.parg()); else break; if (x46.parg()) a.el_append(x46.parg()); else break; if (x47.parg()) a.el_append(x47.parg()); else break; if (x48.parg()) a.el_append(x48.parg()); else break; if (x49.parg()) a.el_append(x49.parg()); else break; if (x50.parg()) a.el_append(x50.parg()); else break; if (x51.parg()) a.el_append(x51.parg()); else break; if (x52.parg()) a.el_append(x52.parg()); else break; if (x53.parg()) a.el_append(x53.parg()); else break; if (x54.parg()) a.el_append(x54.parg()); else break; if (x55.parg()) a.el_append(x55.parg()); else break; if (x56.parg()) a.el_append(x56.parg()); else break; if (x57.parg()) a.el_append(x57.parg()); else break; if (x58.parg()) a.el_append(x58.parg()); else break; if (x59.parg()) a.el_append(x59.parg()); else break; if (x60.parg()) a.el_append(x60.parg()); else break; } while (false);
  for(s_long ind = 0; ind < a.n(); ind += 2) { mdest.map_append(*a[ind], ind + 1 < a.n() ? *a[ind+1] : unity(), true); }
}
void paramline::_list_mx_set_u ( unity& mdest, s_long fk, rA x1, rA x2, rA x3, rA x4, rA x5, rA x6, rA x7, rA x8, rA x9, rA x10, rA x11, rA x12, rA x13, rA x14, rA x15, rA x16, rA x17, rA x18, rA x19, rA x20, rA x21, rA x22, rA x23, rA x24, rA x25, rA x26, rA x27, rA x28, rA x29, rA x30, rA x31, rA x32, rA x33, rA x34, rA x35, rA x36, rA x37, rA x38, rA x39, rA x40, rA x41, rA x42, rA x43, rA x44, rA x45, rA x46, rA x47, rA x48, rA x49, rA x50, rA x51, rA x52, rA x53, rA x54, rA x55, rA x56, rA x57, rA x58, rA x59, rA x60 )
{
  vec2_t<const unity*> a; mdest.map_clear(true); mdest.mapFlags_set(-1, fk);
  do { if (x1.parg()) a.el_append(x1.parg()); else break; if (x2.parg()) a.el_append(x2.parg()); else break; if (x3.parg()) a.el_append(x3.parg()); else break; if (x4.parg()) a.el_append(x4.parg()); else break; if (x5.parg()) a.el_append(x5.parg()); else break; if (x6.parg()) a.el_append(x6.parg()); else break; if (x7.parg()) a.el_append(x7.parg()); else break; if (x8.parg()) a.el_append(x8.parg()); else break; if (x9.parg()) a.el_append(x9.parg()); else break; if (x10.parg()) a.el_append(x10.parg()); else break; if (x11.parg()) a.el_append(x11.parg()); else break; if (x12.parg()) a.el_append(x12.parg()); else break; if (x13.parg()) a.el_append(x13.parg()); else break; if (x14.parg()) a.el_append(x14.parg()); else break; if (x15.parg()) a.el_append(x15.parg()); else break; if (x16.parg()) a.el_append(x16.parg()); else break; if (x17.parg()) a.el_append(x17.parg()); else break; if (x18.parg()) a.el_append(x18.parg()); else break; if (x19.parg()) a.el_append(x19.parg()); else break; if (x20.parg()) a.el_append(x20.parg()); else break; if (x21.parg()) a.el_append(x21.parg()); else break; if (x22.parg()) a.el_append(x22.parg()); else break; if (x23.parg()) a.el_append(x23.parg()); else break; if (x24.parg()) a.el_append(x24.parg()); else break; if (x25.parg()) a.el_append(x25.parg()); else break; if (x26.parg()) a.el_append(x26.parg()); else break; if (x27.parg()) a.el_append(x27.parg()); else break; if (x28.parg()) a.el_append(x28.parg()); else break; if (x29.parg()) a.el_append(x29.parg()); else break; if (x30.parg()) a.el_append(x30.parg()); else break; if (x31.parg()) a.el_append(x31.parg()); else break; if (x32.parg()) a.el_append(x32.parg()); else break; if (x33.parg()) a.el_append(x33.parg()); else break; if (x34.parg()) a.el_append(x34.parg()); else break; if (x35.parg()) a.el_append(x35.parg()); else break; if (x36.parg()) a.el_append(x36.parg()); else break; if (x37.parg()) a.el_append(x37.parg()); else break; if (x38.parg()) a.el_append(x38.parg()); else break; if (x39.parg()) a.el_append(x39.parg()); else break; if (x40.parg()) a.el_append(x40.parg()); else break; if (x41.parg()) a.el_append(x41.parg()); else break; if (x42.parg()) a.el_append(x42.parg()); else break; if (x43.parg()) a.el_append(x43.parg()); else break; if (x44.parg()) a.el_append(x44.parg()); else break; if (x45.parg()) a.el_append(x45.parg()); else break; if (x46.parg()) a.el_append(x46.parg()); else break; if (x47.parg()) a.el_append(x47.parg()); else break; if (x48.parg()) a.el_append(x48.parg()); else break; if (x49.parg()) a.el_append(x49.parg()); else break; if (x50.parg()) a.el_append(x50.parg()); else break; if (x51.parg()) a.el_append(x51.parg()); else break; if (x52.parg()) a.el_append(x52.parg()); else break; if (x53.parg()) a.el_append(x53.parg()); else break; if (x54.parg()) a.el_append(x54.parg()); else break; if (x55.parg()) a.el_append(x55.parg()); else break; if (x56.parg()) a.el_append(x56.parg()); else break; if (x57.parg()) a.el_append(x57.parg()); else break; if (x58.parg()) a.el_append(x58.parg()); else break; if (x59.parg()) a.el_append(x59.parg()); else break; if (x60.parg()) a.el_append(x60.parg()); else break; } while (false);
  for(s_long ind = 0; ind < a.n(); ind += 2) { mdest.map_append(*a[ind], ind + 1 < a.n() ? *a[ind+1] : unity(), true); }
}
void paramline::_list_h_set_u ( unity& hdest, rA x1, rA x2, rA x3, rA x4, rA x5, rA x6, rA x7, rA x8, rA x9, rA x10, rA x11, rA x12, rA x13, rA x14, rA x15, rA x16, rA x17, rA x18, rA x19, rA x20, rA x21, rA x22, rA x23, rA x24, rA x25, rA x26, rA x27, rA x28, rA x29, rA x30, rA x31, rA x32, rA x33, rA x34, rA x35, rA x36, rA x37, rA x38, rA x39, rA x40, rA x41, rA x42, rA x43, rA x44, rA x45, rA x46, rA x47, rA x48, rA x49, rA x50, rA x51, rA x52, rA x53, rA x54, rA x55, rA x56, rA x57, rA x58, rA x59, rA x60 )
{
  vec2_t<const unity*> a; hdest.hash_clear(true);
  do { if (x1.parg()) a.el_append(x1.parg()); else break; if (x2.parg()) a.el_append(x2.parg()); else break; if (x3.parg()) a.el_append(x3.parg()); else break; if (x4.parg()) a.el_append(x4.parg()); else break; if (x5.parg()) a.el_append(x5.parg()); else break; if (x6.parg()) a.el_append(x6.parg()); else break; if (x7.parg()) a.el_append(x7.parg()); else break; if (x8.parg()) a.el_append(x8.parg()); else break; if (x9.parg()) a.el_append(x9.parg()); else break; if (x10.parg()) a.el_append(x10.parg()); else break; if (x11.parg()) a.el_append(x11.parg()); else break; if (x12.parg()) a.el_append(x12.parg()); else break; if (x13.parg()) a.el_append(x13.parg()); else break; if (x14.parg()) a.el_append(x14.parg()); else break; if (x15.parg()) a.el_append(x15.parg()); else break; if (x16.parg()) a.el_append(x16.parg()); else break; if (x17.parg()) a.el_append(x17.parg()); else break; if (x18.parg()) a.el_append(x18.parg()); else break; if (x19.parg()) a.el_append(x19.parg()); else break; if (x20.parg()) a.el_append(x20.parg()); else break; if (x21.parg()) a.el_append(x21.parg()); else break; if (x22.parg()) a.el_append(x22.parg()); else break; if (x23.parg()) a.el_append(x23.parg()); else break; if (x24.parg()) a.el_append(x24.parg()); else break; if (x25.parg()) a.el_append(x25.parg()); else break; if (x26.parg()) a.el_append(x26.parg()); else break; if (x27.parg()) a.el_append(x27.parg()); else break; if (x28.parg()) a.el_append(x28.parg()); else break; if (x29.parg()) a.el_append(x29.parg()); else break; if (x30.parg()) a.el_append(x30.parg()); else break; if (x31.parg()) a.el_append(x31.parg()); else break; if (x32.parg()) a.el_append(x32.parg()); else break; if (x33.parg()) a.el_append(x33.parg()); else break; if (x34.parg()) a.el_append(x34.parg()); else break; if (x35.parg()) a.el_append(x35.parg()); else break; if (x36.parg()) a.el_append(x36.parg()); else break; if (x37.parg()) a.el_append(x37.parg()); else break; if (x38.parg()) a.el_append(x38.parg()); else break; if (x39.parg()) a.el_append(x39.parg()); else break; if (x40.parg()) a.el_append(x40.parg()); else break; if (x41.parg()) a.el_append(x41.parg()); else break; if (x42.parg()) a.el_append(x42.parg()); else break; if (x43.parg()) a.el_append(x43.parg()); else break; if (x44.parg()) a.el_append(x44.parg()); else break; if (x45.parg()) a.el_append(x45.parg()); else break; if (x46.parg()) a.el_append(x46.parg()); else break; if (x47.parg()) a.el_append(x47.parg()); else break; if (x48.parg()) a.el_append(x48.parg()); else break; if (x49.parg()) a.el_append(x49.parg()); else break; if (x50.parg()) a.el_append(x50.parg()); else break; if (x51.parg()) a.el_append(x51.parg()); else break; if (x52.parg()) a.el_append(x52.parg()); else break; if (x53.parg()) a.el_append(x53.parg()); else break; if (x54.parg()) a.el_append(x54.parg()); else break; if (x55.parg()) a.el_append(x55.parg()); else break; if (x56.parg()) a.el_append(x56.parg()); else break; if (x57.parg()) a.el_append(x57.parg()); else break; if (x58.parg()) a.el_append(x58.parg()); else break; if (x59.parg()) a.el_append(x59.parg()); else break; if (x60.parg()) a.el_append(x60.parg()); else break; } while (false);
  for(s_long ind = 0; ind < a.n(); ind += 2) { hdest.hash_set(*a[ind], ind + 1 < a.n() ? *a[ind+1] : unity(), true); }
}
void paramline::_list_hx_set_u ( unity& hdest, s_long fk, rA x1, rA x2, rA x3, rA x4, rA x5, rA x6, rA x7, rA x8, rA x9, rA x10, rA x11, rA x12, rA x13, rA x14, rA x15, rA x16, rA x17, rA x18, rA x19, rA x20, rA x21, rA x22, rA x23, rA x24, rA x25, rA x26, rA x27, rA x28, rA x29, rA x30, rA x31, rA x32, rA x33, rA x34, rA x35, rA x36, rA x37, rA x38, rA x39, rA x40, rA x41, rA x42, rA x43, rA x44, rA x45, rA x46, rA x47, rA x48, rA x49, rA x50, rA x51, rA x52, rA x53, rA x54, rA x55, rA x56, rA x57, rA x58, rA x59, rA x60 )
{
  vec2_t<const unity*> a; hdest.hash_clear(true); hdest.hashFlags_set(-1, fk);
  do { if (x1.parg()) a.el_append(x1.parg()); else break; if (x2.parg()) a.el_append(x2.parg()); else break; if (x3.parg()) a.el_append(x3.parg()); else break; if (x4.parg()) a.el_append(x4.parg()); else break; if (x5.parg()) a.el_append(x5.parg()); else break; if (x6.parg()) a.el_append(x6.parg()); else break; if (x7.parg()) a.el_append(x7.parg()); else break; if (x8.parg()) a.el_append(x8.parg()); else break; if (x9.parg()) a.el_append(x9.parg()); else break; if (x10.parg()) a.el_append(x10.parg()); else break; if (x11.parg()) a.el_append(x11.parg()); else break; if (x12.parg()) a.el_append(x12.parg()); else break; if (x13.parg()) a.el_append(x13.parg()); else break; if (x14.parg()) a.el_append(x14.parg()); else break; if (x15.parg()) a.el_append(x15.parg()); else break; if (x16.parg()) a.el_append(x16.parg()); else break; if (x17.parg()) a.el_append(x17.parg()); else break; if (x18.parg()) a.el_append(x18.parg()); else break; if (x19.parg()) a.el_append(x19.parg()); else break; if (x20.parg()) a.el_append(x20.parg()); else break; if (x21.parg()) a.el_append(x21.parg()); else break; if (x22.parg()) a.el_append(x22.parg()); else break; if (x23.parg()) a.el_append(x23.parg()); else break; if (x24.parg()) a.el_append(x24.parg()); else break; if (x25.parg()) a.el_append(x25.parg()); else break; if (x26.parg()) a.el_append(x26.parg()); else break; if (x27.parg()) a.el_append(x27.parg()); else break; if (x28.parg()) a.el_append(x28.parg()); else break; if (x29.parg()) a.el_append(x29.parg()); else break; if (x30.parg()) a.el_append(x30.parg()); else break; if (x31.parg()) a.el_append(x31.parg()); else break; if (x32.parg()) a.el_append(x32.parg()); else break; if (x33.parg()) a.el_append(x33.parg()); else break; if (x34.parg()) a.el_append(x34.parg()); else break; if (x35.parg()) a.el_append(x35.parg()); else break; if (x36.parg()) a.el_append(x36.parg()); else break; if (x37.parg()) a.el_append(x37.parg()); else break; if (x38.parg()) a.el_append(x38.parg()); else break; if (x39.parg()) a.el_append(x39.parg()); else break; if (x40.parg()) a.el_append(x40.parg()); else break; if (x41.parg()) a.el_append(x41.parg()); else break; if (x42.parg()) a.el_append(x42.parg()); else break; if (x43.parg()) a.el_append(x43.parg()); else break; if (x44.parg()) a.el_append(x44.parg()); else break; if (x45.parg()) a.el_append(x45.parg()); else break; if (x46.parg()) a.el_append(x46.parg()); else break; if (x47.parg()) a.el_append(x47.parg()); else break; if (x48.parg()) a.el_append(x48.parg()); else break; if (x49.parg()) a.el_append(x49.parg()); else break; if (x50.parg()) a.el_append(x50.parg()); else break; if (x51.parg()) a.el_append(x51.parg()); else break; if (x52.parg()) a.el_append(x52.parg()); else break; if (x53.parg()) a.el_append(x53.parg()); else break; if (x54.parg()) a.el_append(x54.parg()); else break; if (x55.parg()) a.el_append(x55.parg()); else break; if (x56.parg()) a.el_append(x56.parg()); else break; if (x57.parg()) a.el_append(x57.parg()); else break; if (x58.parg()) a.el_append(x58.parg()); else break; if (x59.parg()) a.el_append(x59.parg()); else break; if (x60.parg()) a.el_append(x60.parg()); else break; } while (false);
  for(s_long ind = 0; ind < a.n(); ind += 2) { hdest.hash_set(*a[ind], ind + 1 < a.n() ? *a[ind+1] : unity(), true); }
}

void paramline::x_encode1(const unity& x, std::wstring& retval, bool b_name, bool b_ar_elem)
{
    if (b_name && x.utype()!=utString) { x_encode1(CStr3(x), retval, true, false); }

    if (x.isArray())
    {
      if (b_ar_elem) { retval = L"\\z"; }
      else
      {
          if (x.arrsz()==0) { retval = L"\\z"; }
          else
          {
            unity ar; ar.arr_init<utString>(x.arrlb() - 1); ar.arrub_(x.arrub());
            if (x.utype() == utUnityArray)
              { for(s_long ind = x.arrlb(); ind <= x.arrub(); ++ind) { x_encode1(x.ref<utUnity>(ind), ar.ref<utString>(ind), false, true); } }
            else { for(s_long ind = x.arrlb(); ind <= x.arrub(); ++ind) { x_encode1(x.val<utUnity>(ind), ar.ref<utString>(ind), false, true); } }
            retval = join(ar, L"|");
          }
      }
    }
    else
    {
        switch(x.utype())
        {
            case utEmpty: if (b_ar_elem) retval = L""; else retval = L"\\e"; break;
            case utChar: retval = x.val<utChar>() ? L"\\1" : L"\\0"; break;
            case utInt: retval = x.vstr(); break;
            case utDate: retval = x.vstr(); break;
            case utFloat:
            {
              double x1 = x.vfp();
              static const int prec_mimi = 11;
              int prec = 13;
              while (prec >= prec_mimi)
              {
                _fls75 z1(x1, prec + 1, prec);
                if (prec == prec_mimi) { retval = z1.wstr(); break; }
                double x2 = str2f(z1.c_str()); _fls75 z2(x2, prec + 1, prec);
                if (z2 == z1) { retval = z1.wstr(); break; }
                --prec;
              }

              if (retval.find(L".") == nposw) { retval.insert(0,L"\\f"); }
              break;
            }
            case utString:
                {
                  cref_t<std::wstring> r_s = x.cref<utString>();
                  x_repl_e1(r_s.ref(), retval, b_name, b_ar_elem);
                  if (b_name) return;
                  std::wstring s = _trim(retval, L" ");
                  if (s.length() == 0) { goto lInsertStringPrefix; }
                  if (wstring_like(s.substr(0,1),L"[0123456789.+-]")) { goto lInsertStringPrefix; }
                  if (_lcase(s) == L"true") { goto lInsertStringPrefix; }
                  if (_lcase(s) == L"false") { goto lInsertStringPrefix; }
                  break;
                }
lInsertStringPrefix:
                    retval.insert(0,L"\\s");
                break;
            case utObject:
                {
                    const unity* pv = x.objPtr_c<unity>(); if (!pv || !pv->isArray()) { goto lObjNoAr; }
                    if (b_ar_elem) { retval = L"\\z"; }
                    else
                    {
                        const unity& x = *pv;
                        if (x.arrsz()==0) { retval = L"\\z"; }
                        else
                        {
                          unity ar; ar.arr_init<utString>(x.arrlb() - 1); ar.arrub_(x.arrub());
                          if (x.utype() == utUnityArray)
                            { for(s_long ind = x.arrlb(); ind <= x.arrub(); ++ind) { x_encode1(x.ref<utUnity>(ind), ar.ref<utString>(ind), false, true); } }
                          else { for(s_long ind = x.arrlb(); ind <= x.arrub(); ++ind) { x_encode1(x.val<utUnity>(ind), ar.ref<utString>(ind), false, true); } }
                          retval = join(ar, L"|");
                        }
                    }
                    break;
                }
lObjNoAr: // non-array object
                x_encode1(CStr3(x), retval, false, b_ar_elem);
                break;
            default: // unknown type
                x_encode1(CStr3(x), retval, false, b_ar_elem);
                break;
        }
    }
}

void paramline::x_repl_e1(const std::wstring& s1, std::wstring& s2, bool b_name, bool b_ar_elem)
{
    switch(s1.length())
    {
        case 0: { break; }
        case 1: { switch (s1[0]) { case L';': case L' ': case L'=': case L'|': case L'\\': s2.clear(); s2 += L'\\'; s2 += s1[0]; break; default: s2 = s1; break; } break; }
        default:
        {
          _t_wz ps = 0, n = s1.size();
          s2.clear(); s2.reserve(s1.size() + 16);
          wchar_t c0 = s1[ps];
          switch (c0)
          {
            case L'\\':
            {
              ++ps; wchar_t c = s1[ps];
              switch (c)
              {
                case L'\\': s2 += L"`b`b`b`b"; ++ps; break;
                case L'\r': ++ps; if (ps < n && s1[ps] == L'\n') { s2 += L"`b`b`b~"; ++ps; } else { s2 += L"`b`b\r"; } break;
                default: s2 += L"`b`b"; break;
              }
              break;
            }
            case L' ': s2 += L"`b "; ++ps; break;
            case L';': s2 += L"`b`,"; ++ps; break;
            case L'\r': ++ps; if (ps < n && s1[ps] == L'\n') { s2 += L"`b~"; ++ps; } else { s2 += c0; } break;
            case L'=': if (b_name) { s2 += L"`b`e"; } else { s2 += c0; } ++ps; break;
            case L'|': s2 += L"`b`v"; ++ps; break;
            case L'`': s2 += L"`a"; ++ps; break;
            default: s2 += c0; ++ps; break;
          }
          while (ps < n)
          {
            c0 = s1[ps];
            switch (c0)
            {
              case L'\\':
              {
                ++ps;
                if (ps < n)
                {
                  wchar_t c = s1[ps];
                  switch (c)
                  {
                    case L'\\': s2 += L"`b`b`b`b"; ++ps; continue;
                    case L'\r': ++ps; if (ps < n && s1[ps] == L'\n') { s2 += L"`b`b`b~"; ++ps; } else { s2 += L"`b`b\r"; } continue;
                    case L';': case L'=': case L'|': case L'~': case L' ': s2 += L"`b`b"; continue;
                    default: s2 += c0; continue;
                  }
                }
                else { s2 += c0; continue; }
              }
              case L';': s2 += L"`b`,"; ++ps; continue;
              case L'\r': ++ps; if (ps < n && s1[ps] == L'\n') { s2 += L"`b~"; ++ps; } else { s2 += c0; } continue;
              case L'=': if (b_name) { s2 += L"`b`e"; } else { s2 += c0; } ++ps; continue;
              case L'|': if (b_ar_elem) { s2 += L"`b`v"; } else { s2 += c0; } ++ps; continue;
              case L'`': s2 += L"`a"; ++ps; continue;
              default: s2 += c0; ++ps; continue;
            }
          }
          ps = s2.size() - 1; c0 = s2[ps]; if (c0 == L' ' || c0 == L'\\') { s2[ps] = L'\\'; s2 += c0; }
          break;


//          s2 = s1;
//            if (s2.find(L"`") != nposw) s2 = replace(s2, L"`", L"`a");
//            if (s2.find(L"\\\\") != nposw) s2 = replace(s2, L"\\\\", L"`b`b`b`b");
//            if (s2.find(L"\\;") != nposw) s2 = replace(s2, L"\\;", L"`b`b;");
//            if (s2.find(L"\\~") != nposw) s2 = replace(s2, L"\\~", L"`b`b~");
//            if (s2.find(__wBSCRLF) != nposw) s2 = replace(s2, __wBSCRLF, L"`b`b`b~");
//            if (s2.find(L"\\=") != nposw) s2 = replace(s2, L"\\=", L"`b`b=");
//            if (s2.find(L"\\ ") != nposw) s2 = replace(s2, L"\\ ", L"`b`b ");
//            if (s2.find(L"\\|") != nposw) s2 = replace(s2, L"\\|", L"`b`b|");

//            if (s2.find(L";") != nposw) s2 = replace(s2, L";", L"`b`,");
//            if (s2.find(wCRLF) != nposw) s2 = replace(s2, wCRLF, L"`b~");

//            if (s2.substr(0,1).find_first_of(L" \\") != nposw) s2.insert(0,L"\\");
//            if (s2.substr(s2.length() - 1,1).find_first_of(L" \\") != nposw) s2.insert(s2.length() - 1,L"\\");

//            if (b_name) { if (s2.find(L"=") != nposw) s2 = replace(s2, L"=", L"`b`e"); }
//            else
//            {
//                if (b_ar_elem) { if (s2.find(L"|") != nposw) s2 = replace(s2, L"|", L"`b`v"); }
//                  else { if (s2.substr(0,1) == L"|") s2.insert(0, L"\\"); }
//            }
        }
    }
}

void paramline::x_replace2a(std::wstring& s, s_long flags)
{
  if (flags & 1)
  {
    _t_wz ps = 0, pd = 0, n = s.size();
    while (ps < n)
    {
      wchar_t c = s[ps++];
      if (c == L'`' && ps < n)
      {
        switch (s[ps])
        {
          case L'b': s[pd++] = L'\\'; ++ps; continue;
          case L'v': s[pd++] = L'|'; ++ps; continue;
          case L'n': s[pd++] = L'\r'; s[pd++] = L'\n'; ++ps; continue;
          case L',': s[pd++] = L';'; ++ps; continue;
          case L's': s[pd++] = L' '; ++ps; continue;
          case L'e': s[pd++] = L'='; ++ps; continue;
          case L'a': s[pd++] = L'`'; ++ps; continue;
          default: break;
        }
      }
      s[pd++] = c;
    }
    s.resize(pd);
  }
}

void paramline::x_replace4(const std::wstring& s1, std::wstring& s2, s_long& flags)
{
  _t_wz n = s1.size(), n2 = 0, nmax = s2.max_size() - 2, ps = 0, pd = 0;
  while (ps < n)
  {
    wchar_t c = s1[ps++];
    if (c == L'`') { if (n2 > nmax) { throw XUExec("x_replace4.1"); } n2 += 2; flags |= 1; continue; }
    if (c == L'\\')
    {
      switch (s1[ps])
      {
        case L'\\':  case L';':  case L'~':  case L' ':  case L'=':  case L'|':
          if (n2 > nmax) { throw XUExec("x_replace4.2"); } n2 += 2; ++ps; flags |= 1; continue;
        default: break;
      }
    }
    n2 += 1; if (n2 > nmax) { throw XUExec("x_replace4.3"); }
  }
  if ((flags & 1) == 0) { s2 = s1; return; }
  ps = 0; s2.resize(n2);
  while (ps < n)
  {
    wchar_t c = s1[ps++];
    if (c == L'`') { s2[pd++] = L'`'; s2[pd++] = L'a'; continue; }
    if (c == L'\\')
    {
      switch (s1[ps])
      {
        case L'\\': s2[pd++] = L'`'; s2[pd++] = L'b'; ++ps; continue;
        case L';': s2[pd++] = L'`'; s2[pd++] = L','; ++ps; continue;
        case L'~': s2[pd++] = L'`'; s2[pd++] = L'n'; ++ps; continue;
        case L' ': s2[pd++] = L'`'; s2[pd++] = L's'; ++ps; continue;
        case L'=': s2[pd++] = L'`'; s2[pd++] = L'e'; ++ps; continue;
        case L'|': s2[pd++] = L'`'; s2[pd++] = L'v'; ++ps; continue;
        default: break;
      }
    }
    s2[pd++] = c;
  }
}

  // NOTE v must contain string, already passed
  //   through x_replace4 and _trim(, L" ")
void paramline::x_decode1v(unity& v, bool v_ar_elem, s_long flags)
{
    std::wstring s;

    try
    {
        std::wstring pfx = v.ref<utString>().substr(0,1);
        if (pfx == L"") { v.clear(); }
        else if (pfx == L"\\")
        {
            pfx = v.ref<utString>().substr(0,2);
            if (pfx == L"\\e") { if (_trim(v.ref<utString>().substr(2), L" ").length()>0) { goto lLogicalDecodeError; } v.clear(); }
            else if (pfx == L"\\s") { v = v.ref<utString>().substr(2); x_replace2a(v.rx<utString>(), flags); }
            else if (pfx == L"\\i") { s = v.ref<utString>().substr(2); if (x_incorrect_integer_value_str(s, false)) { goto lLogicalDecodeError; } v = str2i(s); }
            else if (pfx == L"\\f") { s = v.ref<utString>().substr(2); if (x_incorrect_numeric_value_str(s)) { goto lLogicalDecodeError; } v = str2f(s); }
            else if (pfx == L"\\d") { s = v.ref<utString>().substr(2); try { v = unity(s).val<utDate>(); } catch (...) { goto lLogicalDecodeError; } }
            else if (pfx == L"\\0") { if (_trim(v.ref<utString>().substr(2), L" ").length()>0) { goto lLogicalDecodeError; } v = false; }
            else if (pfx == L"\\1") { if (_trim(v.ref<utString>().substr(2), L" ").length()>0) { goto lLogicalDecodeError; } v = true; }
            else if (pfx == L"\\z") { if (v_ar_elem) { v.clear(); } else { v.arr_init<utUnity>(1); } }
            else
            {
                if (v.ref<utString>().length()<=2) { v.clear(); return; }
                v = v.ref<utString>().substr(2);
                s = _trim(v.rx<utString>(), L" ");
                goto lAutoDetectType;
            }
        }
        else if (pfx == L"|")
        {
            if (v_ar_elem) goto lLogicalDecodeError;
            unity ar2 = split(v.ref<utString>(),L"|");
            v.arr_init<utUnity>(1); v.arrub_(ar2.arrub());
            for(s_long ind = 1; ind <= ar2.arrub(); ++ind)
            {
                v.arr_set(ind, _trim(ar2.rx<utString>(ind), L" "));
                x_decode1v(v.rx<utUnity>(ind), true, flags);
            }
        }
        else { s = v.ref<utString>(); goto lAutoDetectType; }
        return;

lAutoDetectType:

        if (x_decode1v_auto_date(s, v)) return;
        if (!x_incorrect_integer_value_str(s, false)) { v = str2i(s); return; }
        if (!x_incorrect_numeric_value_str(s)) { v = str2f(s); return; }
        if (s == L"true") { v = true; return; }
        if (s == L"false") { v = false; return; }
        x_replace2a(v.ref<utString>(), flags);
        return;

    }
  catch(std::exception& e) { throw XUExec("x_decode1v.1", e.what(), v.vflstr()); }
    catch(...) { throw XUExec("x_decode1v.2", v.vflstr()); }
lLogicalDecodeError:
    throw XUExec("x_decode1v.3", v.vflstr());
    return;
}

bool paramline::x_decode1v_auto_date(const std::wstring& s, unity& retval) throw ()
{
  try { _unitydate d; if (!StaticConv::conv_String_Date0(s, d, true)) { return false; } retval = d; return true; }
    catch(...) { return false; }
}

bool paramline::x_incorrect_numeric_value(const unity& x, bool allow_empty) const
{
  switch(x.utype())
  {
    case utInt: case utFloat: return false;
    case utEmpty: return !allow_empty;
    case utString: return x_incorrect_numeric_value_str(x.ref<utString>());
    default: return true;
  }
}
bool paramline::x_incorrect_numeric_value_str(const std::wstring& s) const
{
    if (s.find_first_not_of(L"0123456789.Ee + -") != nposw) return true;
    try { double x = str2f(s, 0., false); return !(x == x); } catch(...) { return true; }
}
bool paramline::x_incorrect_integer_value(const unity& x, bool allow_empty, bool allow_float_str) const
{
  switch(x.utype())
  {
    case utInt: return false;
    case utFloat: { try { double y = x.ref<utFloat>(); return floor(y) != y; } catch(...) { return true; } return false; }
    case utEmpty: return !allow_empty;
    case utString: return x_incorrect_integer_value_str(x.ref<utString>(), allow_float_str);
    default: return true;
  }
}
bool paramline::x_incorrect_integer_value_str(const std::wstring& s, bool allow_float_str) const
{
  if (allow_float_str)
  {
    if (s.find_first_not_of(L"0123456789.Ee + -") != nposw) { return true; }
    try { (void)str2i(s, 0, false); double y = str2f(s, 0., false); return floor(y) != y; } catch(...) { return true; }
  }
  else
  {
    if (s.find_first_not_of(L"0123456789 + -") != nposw) { return true; }
    try { (void)str2i(s, 0, false); return false; } catch(...) { return true; }
  }
}

}













////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Common library OS-dependent code

#ifdef _bmdxpl_Wnds

#include <windows.h>
#ifdef h_addr
  #undef h_addr
#endif
#ifdef min
  #undef min
#endif
#ifdef max
  #undef max
#endif

namespace bmdx
{
  s_long wsbs_lkp() { return -1; } // ws/bs functions do not change locale, locking is not needed
  std::string _wsToBs(const wchar_t* ps, meta::s_ll n, bool is_oem) // n < 0: autodetect length based on null char.
  {
    try {
      #if bmdx_wsbs_force_utf8
        return _wsToBsUtf8(ps, n);
      #else
        if (!ps) { throw XUExec("_wsToBs.3"); }
        if (n < 0) { n = 0; const wchar_t* p = ps; while (*p++) { ++n; } }
        if (n > std::numeric_limits<int>::max()) { n = std::numeric_limits<int>::max(); }
        std::string s;
        if (n <= 0) { return s; }
        s_long n2 = WideCharToMultiByte(is_oem ? CP_OEMCP : CP_ACP, 0, ps, int(n), 0, 0, 0, 0);
        if (n2 <= 0) { throw XUExec("_wsToBs.4"); }
        s.resize(_t_sz(n2), ' ');
        s_long n3 = WideCharToMultiByte(is_oem ? CP_OEMCP : CP_ACP, 0, ps, int(n), &s[0], n2, 0, 0);
        if (n3 != n2) { throw XUExec("_wsToBs.5"); }
        return s;
      #endif
    }
    catch (_XUBase&) { throw; }
    catch (const std::exception& e) { throw XUExec("_wsToBs.1", e.what()); }
    catch (...) { throw XUExec("_wsToBs.2"); }
  }

  std::wstring _bsToWs(const char* ps, meta::s_ll n) // n < 0: autodetect length based on null char.
  {
    try {
      #if bmdx_bsws_force_utf8
        return _bsUtf8ToWs(ps, n);
      #else
        if (!ps) { throw XUExec("_bsToWs.3"); }
        if (n < 0) { n = 0; const char* p = ps; while (*p++) { ++n; } }
        if (n > std::numeric_limits<int>::max()) { n = std::numeric_limits<int>::max(); }
        std::wstring s;
        if (n <= 0) { return s; }
        s_long n2 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, ps, int(n), 0, 0);
        if (n2 <= 0) { throw XUExec("_bsToWs.4"); }
        s.resize(_t_wz(n2), L' ');
        s_long n3 = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, ps, int(n), &s[0], n2);
        if (n3 != n2) { throw XUExec("_bsToWs.5"); }
        return s;
      #endif
    }
    catch (_XUBase&) { throw; }
    catch (const std::exception& e) { throw XUExec("_bsToWs.1", e.what()); }
    catch (...) { throw XUExec("_bsToWs.2"); }
  }


    // 1: s1 > s2, 0: s1 == s2, -1: s1 < s2.
  s_long wscompare(const std::wstring& s1, const std::wstring& s2, bool ignore_case)
  {
    if (ignore_case)
    {
      s_long n = CompareStringW(LOCALE_USER_DEFAULT, NORM_IGNORECASE|SORT_STRINGSORT, s1.c_str(), int(std::min(_t_wz(std::numeric_limits<int>::max()), s1.length())), s2.c_str(), int(std::min(_t_wz(std::numeric_limits<int>::max()), s2.length())));
      if (n == 0) { throw XUExec("wscompare.1"); }
      return n - 2;
    }
    else { int cmp1 = s1.compare(s2); return cmp1 == 0 ? 0 : (cmp1 > 0 ? 1 : -1); }
  }

    _unitydate d_now(bool allow_fracsec) { SYSTEMTIME t; GetLocalTime(&t); _unitydate x(t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond); if (allow_fracsec) { x.add_seconds(t.wMilliseconds / 1000.); } return x; }
    _unitydate d_nowutc(bool allow_fracsec) { SYSTEMTIME t; GetSystemTime(&t); _unitydate x(t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond); if (allow_fracsec) { x.add_seconds(t.wMilliseconds / 1000.); } return x; }

      // single path separator
    std::string cpathsep() {  return "\\"; }
      // single path separator
    std::wstring wpathsep() {  return L"\\"; }
      // two path separators
    std::string cpathsep2() {  return "\\\\"; }
      // two path separators
    std::wstring wpathsep2() {  return L"\\\\"; }
      // full path and filename of this exe (calculated once)
    std::wstring cmd_myexe()
    {
      static bool initDone(false); static wchar_t buf[500] = { L'\0' };
      if (!initDone) { GetModuleFileNameW(0,buf,500); initDone = true;  }
      return buf;
    }
      // command line, without executable path/filename
    std::wstring cmd_string()
    {
      std::wstring s;
      std::wstring scl = GetCommandLineW();
      if (scl.size() > 0)
      {
        _t_wz pos2;
        if (scl[0] == L'\"')
        {
          pos2 = scl.find(L"\"", 1);
          if (pos2 == nposw) { pos2 = scl.size(); }
          else
          {
            pos2 = scl.find_first_not_of(L" \t", pos2 + 1);
            if (pos2 == nposw) { pos2 = scl.size(); }
          }
          s = scl.substr(pos2);
        }
        else
        {
          pos2 = scl.find_first_of(L" \t");
          if (pos2 == nposw) { pos2 = scl.size(); }
          else
          {
            pos2 = scl.find_first_not_of(L" \t", pos2);
            if (pos2 == nposw) { pos2 = scl.size(); }
          }
        }
        s = scl.substr(pos2);
      }
      return s;
    }

      // argv (0-based string array of command line arguments, size >= 1; 0 - program name (not exactly executable name/path), 1..* - dequoted arguments)
    unity cmd_array()
    {
      unity a; a.arr_init<utString>(0);
      int n; LPWSTR* ppc = CommandLineToArgvW(GetCommandLineW(), &n);
      if (ppc) { for (int i = 0; i < n; ++i) { a.arr_append(ppc[i]); } LocalFree(ppc); }
        else { a.arr_append(L""); return a; }
      return a;
    }
}

#endif

#ifdef _bmdxpl_Psx

  #include <wchar.h>
  #include <wctype.h>
  #include <sys/time.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <errno.h>
  #include <unistd.h>

  #ifdef __FreeBSD__
    #include <sys/sysctl.h>
  #endif

  #if __APPLE__ && __MACH__
    #include <libproc.h>
  #endif

#define AUTO___buf_argv
static int __buf_argc = 0;
static char __buf_argv[65000] = "_\0\0";
static void __read_argv(int argc, char** argv, char** envp)
{
  __buf_argc = 1; if (!argv) { return; }
  const int n = sizeof(__buf_argv);
  int pos = 0;
  for (int i = 0; i < argc; ++i)
  {
    const char* p = argv[i]; if (!p) { break; }
    while (*p && pos < n - 1) { __buf_argv[pos++] = *p++; } __buf_argv[pos++] = '\0';
    if (i > 0) { __buf_argc += 1; }
    if (pos >= n) { break; }
  }
}

#ifdef __ANDROID__
  inline int __wcsncasecmp(const wchar_t* s1, const wchar_t* s2, size_t n)
  {
    wchar_t c1, c2;
    if (n == 0) { return 0; }
    for (; *s1; s1++, s2++)
    {
      c1 = towlower(*s1); c2 = towlower(*s2);
      if (c1 != c2) { return (int)c1 - c2; }
      if (--n == 0) { return 0; }
    }
    return -*s2;
  }
  inline int __wctomb(char* ps, wchar_t c) { return wcrtomb(ps, c, 0); }
  inline int __mbtowc(wchar_t* pc, const char* ps, size_t n) { return mbrtowc(pc, ps, n, 0); }
#else
  inline int __wcsncasecmp(const wchar_t* s1, const wchar_t* s2, size_t n) { return wcsncasecmp(s1, s2, n); }
  inline int __wctomb(char* ps, wchar_t c) { return wctomb(ps, c); }
  inline int __mbtowc(wchar_t* pc, const char* ps, size_t n) { return mbtowc(pc, ps, n); }
#endif

#if __APPLE__ && __MACH__
  __attribute__((section("__DATA,__mod_init_func"))) void (* p___read_argv)(int, char*[], char*[]) = &__read_argv;
#else
  __attribute__((section(".init_array"))) void (* p___read_argv)(int, char*[], char*[]) = &__read_argv;
#endif

namespace bmdx
{
    struct _tcnvchk { static bool _test_mbrtowc() { char cc[MB_LEN_MAX+1]; size_t res; wchar_t c; cc[0] = 'A'; res = mbrtowc(&c, cc, 1, 0); if (res != 1 || c != L'A') { return false; } cc[0] = '1'; res = mbrtowc(&c, cc, 1, 0); if (res != 1 || c != L'1') { return false; } cc[0] = ';'; res = mbrtowc(&c, cc, 1, 0); if (res != 1 || c != L';') { return false; } cc[0] = ' '; res = mbrtowc(&c, cc, 1, 0); if (res != 1 || c != L' ') { return false; } return true; } static bool _test_wcrtomb() { char cc[MB_LEN_MAX+1]; size_t res; res = wcrtomb(cc, L'A', 0); if (res != 1 || cc[0] != 'A') { return false; } res = wcrtomb(cc, L'1', 0); if (res != 1 || cc[0] != '1') { return false; } res = wcrtomb(cc, L';', 0); if (res != 1 || cc[0] != ';') { return false; } res = wcrtomb(cc, L' ', 0); if (res != 1 || cc[0] != ' ') { return false; } return true; } };
    s_long wsbs_lkp() { return 1; } // ws/bs functions change locale, locking is needed
    std::string _wsToBs(const wchar_t* ps, meta::s_ll n, bool is_oem) // n < 0: autodetect length based on null char.
    {
      try {
        #if bmdx_wsbs_force_utf8
          static int _mb(2);
        #else
          static int _mb(0);
        #endif
        if (_mb == 2) { return _wsToBsUtf8(ps, n); }

        if (!ps) { throw XUExec("_wsToBs.3"); }
        if (n < 0) { n = 0; const wchar_t* p = ps; while (*p++) { ++n; } }

        std::string s;
        char cc[MB_LEN_MAX+1];
        t_critsec_locale __lock(10, -1); if (sizeof(__lock)) {}
        const char* prevlocn = std::setlocale(LC_CTYPE, "");
        if (__wctomb(0, 0)) {}

        if (!_mb) { _mb = 2 + (int)_tcnvchk::_test_wcrtomb(); }
        if (_mb == 2) { std::setlocale(LC_CTYPE, prevlocn); return _wsToBsUtf8(ps, n); }

        for(meta::s_ll pos = 0; pos < n; ++pos)
        {
          if (ps[pos] == L'\0') { s += '\0'; }
            else { int cnt = __wctomb(cc, ps[pos]); if (cnt == -1) { s += '?'; } else { cc[cnt] = 0; s += cc; } }
        }
        std::setlocale(LC_CTYPE, prevlocn);
        return s;
      }
      catch (_XUBase&) { throw; }
      catch (const std::exception& e) { throw XUExec("_wsToBs.1", e.what()); }
      catch (...) { throw XUExec("_wsToBs.2"); }
    }

    std::wstring _bsToWs(const char* ps, meta::s_ll n) // n < 0: autodetect length based on null char.
    {
      try {
        #if bmdx_bsws_force_utf8
          static int _mb(2);
        #else
          static int _mb(0);
        #endif
        if (_mb == 2) { return _bsUtf8ToWs(ps, n); }

        if (!ps) { throw XUExec("_bsToWs.3"); }
        if (n < 0) { n = 0; const char* p = ps; while (*p++) { ++n; } }
        std::wstring s;
        if (n <= 0) { return s; }
        t_critsec_locale __lock(10, -1); if (sizeof(__lock)) {}
        const char* prevlocn = std::setlocale(LC_CTYPE, "");

        if (!_mb) { _mb = 2 + (int)_tcnvchk::_test_mbrtowc(); }
        if (_mb == 2) { std::setlocale(LC_CTYPE, prevlocn); return _bsUtf8ToWs(ps, n); }

        meta::s_ll pos(0);
        while (pos < n)
        {
          if (ps[pos] == '\0') { s += L'\0'; pos += 1; }
          else
          {
            wchar_t c; int cnt = __mbtowc(&c, &ps[pos], n - pos);
            if (cnt <= -1) { s += L'?'; pos += 1; }
              else if (cnt > 0) { s += c; pos += cnt; }
              else { s += L'\0'; pos += 1; }
          }
        }
        std::setlocale(LC_CTYPE, prevlocn);
        return s;
      }
      catch (_XUBase&) { throw; }
      catch (const std::exception& e) { throw XUExec("_bsToWs.1", e.what()); }
      catch (...) { throw XUExec("_bsToWs.2"); }
    }

      // wide character std::string comparison
    s_long wscompare(const std::wstring& s1, const std::wstring& s2, bool ignore_case)
    {
        if (ignore_case)
        {
            _t_wz minlen= s1.length(); if (s2.length() < minlen) { minlen = s2.length(); }
            int cmp1 = __wcsncasecmp(s1.c_str(), s2.c_str(), minlen);
//            int cmp1 = _wcsnicmp(s1.c_str(),s2.c_str(),minlen); // just to test compiling in Windows
            if (cmp1 != 0) { return (cmp1 > 0 ? 1 : -1); }
            if (s1.length() == s2.length()) { return 0; }
            return s1.length() < s2.length() ? -1 : 1;
        }
        else { int cmp1 = s1.compare(s2); return cmp1 == 0 ? 0 : (cmp1 > 0 ? 1 : -1); }
    }

    _unitydate d_now(bool allow_fracsec)
    {
      static s_long init(0); static s_long hdelta(0);
      if (init == 0)
      {
        std::time_t t0 = std::time(0);
        std::tm t1; std::tm* __t1 = std::gmtime(&t0); if (__t1) { t1 = *__t1; }
        std::tm t2; std::tm* __t2 = std::localtime(&t0); if (__t2) { t2 = *__t2; }
        if (__t1 && __t2) { s_long d = t2.tm_hour - t1.tm_hour; if (d > 12) { d -= 24; } hdelta = d; init = 1; }
        else { init = -1; }
      }
      _unitydate x = d_nowutc(allow_fracsec); x.add_seconds(hdelta * 3600.); return x;
    }
    _unitydate d_nowutc(bool allow_fracsec)
    {
      static s_long init(0); static std::time_t sec0(0); static std::clock_t ms0(0);
      meta::s_ll ms1 = meta::s_ll(std::clock()) * 1000 / CLOCKS_PER_SEC;
      meta::s_ll sec1 = std::time(0);
      if (init != 0) { if ((ms1 - ms0) - (sec1 - sec0) * 1000 < -1000) { init = 0; } }
      if (init == 0) { sec0 = sec1; ms0 = ms1; init = 1; }
      sec1 = sec0 + (ms1 - ms0) / 1000;
      s_long n = s_long((((sec1 + 43200) / (86400 >> 1)) + (2440587 << 1) + 1) >> 1); s_long h = (sec1 / 3600) % 24; s_long m = (sec1 / 60) % 60; s_long s = sec1 % 60;
      _unitydate x(double(_jds_nhms(n, h, m, s)));
      if (allow_fracsec) { x.add_seconds(double((ms1 - ms0) % 1000) / 1000.); }
      return x;
    }

      // single path separator
    std::string cpathsep() {  return "/" ; }
      // single path separator
    std::wstring wpathsep() {  return L"/"; }
      // two path separators
    std::string cpathsep2() {  return "//"; }
      // two path separators
    std::wstring wpathsep2() {  return L"//"; }
      // full path and filename of this exe (calculated once)
    std::wstring cmd_myexe()
    {
        static bool initDone(false); static std::wstring spath;
        if (!initDone)
        {
          char buf[2048];
          #if defined(__FreeBSD__)
            int mib[4]; mib[0] = CTL_KERN; mib[1] = KERN_PROC; mib[2] = KERN_PROC_PATHNAME; mib[3] = -1;
            size_t n = sizeof(buf);
            sysctl(mib, 4, buf, &n, 0, 0);
            spath = bsToWs(std::string(buf, n - 1));
          #elif __APPLE__ && __MACH__
            if (proc_pidpath(getpid(), buf, sizeof(buf)) > 0) { spath = bsToWs(buf); }
          #elif defined(__SUNPRO_CC)
            spath = bsToWs(getexecname());
          #else
            int n = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
            if (n <= 0) { n = readlink("/proc/curproc/exe", buf, sizeof(buf) - 1); }
            if (n <= 0) { n = readlink("/proc/curproc/file", buf, sizeof(buf) - 1); }
            if (n > 0) { buf[n] = '\0'; spath = bsToWs(buf); }
          #endif
          initDone=true;
        }
        return spath;
    }

    static void __buf_argv_ensure()
    {
      static bool initDone(false);
      if (!initDone)
      {
        #if defined(AUTO___buf_argv)
            bool b_read(__buf_argc <= 0);
        #else
            bool b_read(true);
        #endif
        if (b_read)
        {
          __buf_argc = 0;
          while (true) // once
          {
            const int n0 = sizeof(__buf_argv) - 1;
            int fd;
            fd = open("/proc/self/cmdline", O_RDONLY);
            if (fd == 0) { fd = open("/proc/curproc/cmdline", O_RDONLY); }
            if (fd == 0) { break; }
            int n = (int)read(fd, __buf_argv, n0);
            close(fd);
            if (n <= 0) { break; }
            if (__buf_argv[n - 1] != '\0') { __buf_argv[n] = '\0'; ++n; }
            int pos = 0; while (pos < n) { if (__buf_argv[pos++] == '\0') { __buf_argc += 1; } }
            break;
          }
        }
        if (__buf_argc == 0) { __buf_argc = 1; __buf_argv[0] = '\0'; }
        initDone = true;
      }
    }

      // command line, without executable path/filename
    std::wstring cmd_string()
    {
      static bool initDone(false); static std::wstring s;
      if (!initDone)
      {
        __buf_argv_ensure();
        s.clear(); std::string s2; int pos = 0; int j = 0;
        while (j < __buf_argc)
        {
          char c = __buf_argv[pos++];
          if (c == '\0')
          {
            if (j > 1) { s += L' '; }
            if (j > 0)
            {
              if (s2.find_first_of(" \t'\"\\$[](){}^?*") != nposc)
              {
                if (s2.find("'") != nposc) { s2 = replace(s2, "'", "'\\''"); }
                s2 = "'" + s2 + "'";
              }
              s += bsToWs(s2);
            }
            s2.clear();
            j += 1;
          }
          else { s2 += c;  }
        }
        initDone=true;
      }
      return s;
    }

      // argv (0-based string array of command line arguments, size >= 1; 0 - program name (not exactly executable name/path), 1..* - dequoted arguments)
    unity cmd_array()
    {
      unity a; a.arr_init<utString>(0);
      __buf_argv_ensure();
      std::string s2; int pos = 0; int j = 0;
      while (j < __buf_argc)
      {
        char c = __buf_argv[pos++];
        if (c == '\0') { a.arr_append(bsToWs(s2)); s2.clear(); j += 1; }
        else { s2 += c;  }
      }
      return a;
    }
}

#endif


//==============================
// OS or library-dependent code template
//==============================

#ifdef _bmdxpl_OS_NAME_HERE
namespace bmdx
{
    s_long wsbs_lkp() { }
    std::string _wsToBs(const wchar_t* ps, meta::s_ll n, bool is_oem) { } // n < 0: autodetect length based on null char.
    std::wstring _bsToWs(const char* ps, meta::s_ll n) { } // n < 0: autodetect length based on null char.

      // wide character std::string comparison
    s_long wscompare(const std::wstring& s1, const std::wstring& s2, bool ignore_case) { }

      //Returns the current value of high-resolution timer available on the system.
      //  The value is expressed in milliseconds.
    _unitydate d_now(bool allow_fracsec)
    {
      static s_long init(0); static s_long hdelta(0);
      if (init == 0)
      {
        std::time_t t0 = std::time(0);
        std::tm t1; std::tm* __t1 = std::gmtime(&t0); if (__t1) { t1 = *__t1; }
        std::tm t2; std::tm* __t2 = std::localtime(&t0); if (__t2) { t2 = *__t2; }
        if (__t1 && __t2) { s_long d = t2.tm_hour - t1.tm_hour; if (d > 12) { d -= 24; } hdelta = d; init = 1; }
        else { init = -1; }
      }
      _unitydate x = d_nowutc(allow_fracsec); x.add_seconds(hdelta * 3600.); return x;
    }
    _unitydate d_nowutc(bool allow_fracsec)
    {
      static s_long init(0); static std::time_t sec0(0); static std::clock_t ms0(0);
      meta::s_ll ms1 = meta::s_ll(std::clock()) * 1000 / CLOCKS_PER_SEC;
      meta::s_ll sec1 = std::time(0);
      if (init != 0) { if ((ms1 - ms0) - (sec1 - sec0) * 1000 < -1000) { init = 0; } }
      if (init == 0) { sec0 = sec1; ms0 = ms1; init = 1; }
      sec1 = sec0 + (ms1 - ms0) / 1000;
      s_long n = s_long((((sec1 + 43200) / (86400 >> 1)) + (2440587 << 1) + 1) >> 1); s_long h = (sec1 / 3600) % 24; s_long m = (sec1 / 60) % 60; s_long s = sec1 % 60;
      _unitydate x(double(_jds_nhms(n, h, m, s)));
      if (allow_fracsec) { x.add_seconds(double((ms1 - ms0) % 1000) / 1000.); }
      return x;
    }
      // single path separator
    std::string cpathsep() { }
      // single path separator
    std::wstring wpathsep() { }
      // two path separators
    std::string cpathsep2() { }
      // two path sparators
    std::wstring wpathsep2() { }
      // full path and filename of this exe (calculated once)
    std::wstring cmd_myexe() { }
      // command line, without executable path/filename
    std::wstring cmd_string() { }
      // argv (0-based string array of command line arguments, size >= 1; 0 - program name (not exactly executable name/path), 1..* - dequoted arguments)
    unity cmd_array() { }
}
#endif














////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// File utils OS-dependent code


#ifdef _bmdxpl_Wnds

#include <windows.h>
#include <direct.h>

#ifndef F_OK
#define F_OK  0 /* Check for file existence */
#endif

#define getcwd _getcwd
#define mkdir _mkdir
#define access _access

namespace bmdx
{
  bool file_utils::is_full_path(const std::wstring& pathstr) const { return pathstr.length() >= 2 && (wstring_like(pathstr.substr(0, 2), L"[a-zA-Z]:") || pathstr.substr(0, 2) == wpathsep2()); } // <Wnds> </Wnds>
  bool file_utils::is_full_path(const std::string& pathstr) const { return pathstr.length() >= 2 && (wstring_like(bsToWs(pathstr.substr(0, 2)), L"[a-zA-Z]:") || pathstr.substr(0, 2) == cpathsep2()); } // <Wnds> </Wnds>

  bool file_utils::is_valid_path(const std::wstring& s) const
  {
    // <Wnds>
    if (s.find_first_of(L"*/?\"<>|\0\1\2\3\4\5\6\7\10\11\12\13\14\15\16\17\20\21\22\23\24\25\26\27\30\31\32\33\34\35\36\37") != nposw) { return false; }
    if (s.length() >= 2 && s[1] == L':') { if (s.find(L":", 2) != nposw) { return false; } }
      else { if (s.find(L":") != nposw) { return false; } }
    return true;
    // </Wnds>
  }

  bool file_utils::is_valid_path(const std::string& s) const
  {
    // <Wnds>
    if (s.find_first_of("*/?\"<>|\0\1\2\3\4\5\6\7\10\11\12\13\14\15\16\17\20\21\22\23\24\25\26\27\30\31\32\33\34\35\36\37") != nposc) { return false; }
    if (s.length() >= 2 && s[1]==':') { if (s.find(":", 2) != nposc) { return false; } }
      else { if (s.find(":") != nposc) { return false; } }
    return true;
    // </Wnds>
  }

  bool file_utils::xmk_subdir(const std::string& sPath, int level) const
  {
    std::string wp;

    if (level == 0)
    {
      wp = trim(join_path(complete_path(sPath, pdCurDir)), cpathsep(), false, true);
      if (wp.length() == 0) { return false; }
    }
    else
    {
      wp = trim(sPath, cpathsep(), false, true);
      if (wp.length() == 0) { return true; }
    }
    if (is_ex_dir(wp)) { return true; }
    if (!xmk_subdir(strip_path(wp), level+1)) { return false; }

    // <Wnds>
    return 0 == mkdir(wp.c_str());
    // </Wnds>
  }

  std::wstring file_utils::complete_path(const std::wstring& sPath, EFileUtilsPredefinedDir pd, const std::wstring& sUserDefDir) const
    { return bsToWs(complete_path(wsToBs(sPath), pd, wsToBs(sUserDefDir))); }
  std::string file_utils::complete_path(const std::string& sPath, EFileUtilsPredefinedDir pd, const std::string& sUserDefDir) const
  {
      if (is_full_path(sPath)) { return sPath; }
      if (xHasCurDirShortCut(sPath)) { char buf[2048]; return join_path(getcwd(buf, sizeof(buf)), sPath.substr(1)); }
      std::string s;
      switch(pd)
      {
          case pdCurDir: { char buf[2048]; s = join_path(getcwd(buf,sizeof(buf)), sPath); break; }
          case pdThisAppDir: { s = join_path(wsToBs(strip_path(cmd_myexe())), sPath); break; }
          case pdTempDir:
              // <Wnds>
              s = join_path(getenv("TEMP"), sPath);
              // </Wnds>
              if (!is_full_path(s)) { s = join_path(wsToBs(strip_path(cmd_myexe())), s); }
              break;
          default: //pdUserDefinedDir
              s = join_path(sUserDefDir,sPath);
              if (!is_full_path(s)) { s = join_path(wsToBs(strip_path(cmd_myexe())), s); }
      }
      return s;
  }

}
#endif


#ifdef _bmdxpl_Psx

    #include <unistd.h>
    #include <dlfcn.h>

namespace bmdx
{
  bool file_utils::is_full_path(const std::wstring& pathstr) const {  return pathstr.length() && pathstr.substr(0, 1) == wpathsep(); } // <Psx> </Psx>
  bool file_utils::is_full_path(const std::string& pathstr) const { return pathstr.length() && pathstr.substr(0, 1) == cpathsep(); } // <Psx> </Psx>

  bool file_utils::is_valid_path(const std::wstring& s) const { if (s.find_first_of(L"\0") != nposw) return false; return true; } // <Psx> </Psx>
  bool file_utils::is_valid_path(const std::string& s) const { if (s.find_first_of("\0") != nposc) return false; return true; } // <Psx> </Psx>

  bool file_utils::xmk_subdir(const std::string& sPath, int level) const
  {
    std::string wp;

    if (level == 0)
    {
      wp = trim(join_path(complete_path(sPath, pdCurDir)), cpathsep(), false, true);
      if (wp.length() == 0) { return false; }
    }
    else
    {
      wp = trim(sPath, cpathsep(), false, true);
      if (wp.length() == 0) { return true; }
    }
    if (is_ex_dir(wp)) { return true; }
    if (!xmk_subdir(strip_path(wp), level+1)) { return false; }

    // <Psx>
    return 0==mkdir(wp.c_str(), 0777);
    // </Psx>
  }

  std::wstring file_utils::complete_path(const std::wstring& sPath, EFileUtilsPredefinedDir pd, const std::wstring& sUserDefDir) const
  { return bsToWs(complete_path(wsToBs(sPath), pd, wsToBs(sUserDefDir))); }
  std::string file_utils::complete_path(const std::string& sPath, EFileUtilsPredefinedDir pd, const std::string& sUserDefDir) const
  {
    if (is_full_path(sPath)) { return sPath; }
    if (xHasCurDirShortCut(sPath)) { char buf[2048]; return join_path(getcwd(buf, sizeof(buf)), sPath.substr(1)); }
    std::string s;
    switch(pd)
    {
        case pdCurDir: { char buf[2048]; s = join_path(getcwd(buf,sizeof(buf)), sPath); break; }
          case pdThisAppDir: { s = join_path(wsToBs(strip_path(cmd_myexe())), sPath); break; }
          case pdTempDir:
              // <Psx>
              s = join_path(getenv("TMPDIR"), sPath);
              // </Psx>
              if (!is_full_path(s)) { s = join_path(wsToBs(strip_path(cmd_myexe())), s); }
              break;
          default: //pdUserDefinedDir
              s = join_path(sUserDefDir,sPath);
              if (!is_full_path(s)) { s = join_path(wsToBs(strip_path(cmd_myexe())), s); }
      }
      return s;
  }
}
#endif



//=============================================================================================================================
// OS-independent part
//=============================================================================================================================

namespace bmdx
{
std::wstring file_utils::strip_path(const std::wstring& pathstr) const { _t_wz pos = pathstr.rfind(wpathsep()); return pos == nposw ? pathstr : pathstr.substr(0, pos); }
std::string file_utils::strip_path(const std::string& pathstr) const { _t_sz pos = pathstr.rfind(cpathsep()); return pos == nposc ? pathstr : pathstr.substr(0, pos); }

std::wstring file_utils::strip_filename(const std::wstring& pathstr) const { _t_wz pos = pathstr.rfind(wpathsep()); return pos == nposw ? pathstr : pathstr.substr(pos+1); }
std::string file_utils::strip_filename(const std::string& pathstr) const { _t_sz pos = pathstr.rfind(cpathsep()); return pos == nposc ? pathstr : pathstr.substr(pos+1); }

std::wstring file_utils::remove_ext(const std::wstring& pathstr, const std::wstring& extsep) const
{
  _t_wz end = nposw, pos1 = pathstr.rfind(extsep), pos2 = pathstr.rfind(wpathsep());
  if (pos1 != end && (pos2 == end || pos2 < pos1)) { return pathstr.substr(0, pos1); }
  return pathstr;
}
std::string file_utils::remove_ext(const std::string& pathstr, const std::string& extsep) const
{
  _t_sz end = nposc, pos1 = pathstr.rfind(extsep), pos2 = pathstr.rfind(cpathsep());
  if (pos1 != end && (pos2 == end || pos2 < pos1)) { return pathstr.substr(0, pos1); }
  return pathstr;
}

std::wstring file_utils::add_to_name(const std::wstring& pathstr, const std::wstring& s_add, const std::wstring& extsep) const
{
  _t_wz end = nposw, pos1 = pathstr.rfind(extsep), pos2 = pathstr.rfind(wpathsep());
  if (pos1 != end && (pos2 == end || pos2 < pos1)) { return pathstr.substr(0, pos1) + s_add + pathstr.substr(pos1); }
  return pathstr + s_add;
}
std::string file_utils::add_to_name(const std::string& pathstr, const std::string& s_add, const std::string& extsep) const
{
  _t_sz end = nposc, pos1 = pathstr.rfind(extsep), pos2 = pathstr.rfind(cpathsep());
  if (pos1 != end && (pos2 == end || pos2 < pos1)) { return pathstr.substr(0, pos1) + s_add + pathstr.substr(pos1); }
  return pathstr + s_add;
}

std::wstring file_utils::replace_filename(const std::wstring& fnp, const std::wstring& fn) const { return join_path(strip_path(fnp), fn); }
std::string file_utils::replace_filename(const std::string& fnp, const std::string& fn) const { return join_path(strip_path(fnp), fn); }

std::wstring file_utils::join_path(const std::wstring& pathstr, const std::wstring& pathstr2) const
{
    std::wstring s0, s; s = pathstr;
    if (s.substr(0, ps2len) == wpathsep2()) { s0 = wpathsep2(); s = s.substr(ps2len); }
    s += wpathsep(); s += pathstr2;
    while(s.find(wpathsep2()) != nposw) { s = replace(s, wpathsep2(), wpathsep()); }
    if (s_long(s.length()) >= pslen && s.substr(s.length() - pslen) == wpathsep()) { s.erase(s.length() - pslen); }
    #ifdef _bmdxpl_Wnds
      if (s.substr(0, pslen) == wpathsep()) { s.erase(0, pslen); }
    #endif
    s0 += s;
    return s0;
}
std::string file_utils::join_path(const std::string& pathstr, const std::string& pathstr2) const
{
    std::string s0, s; s = pathstr;
    if (s.substr(0, ps2len) == cpathsep2()) { s0 = cpathsep2(); s = s.substr(ps2len); }
    s += cpathsep(); s += pathstr2;
    while(s.find(cpathsep2()) != nposc) { s = replace(s, cpathsep2(), cpathsep()); }
    if (s_long(s.length()) >= pslen && s.substr(s.length() - pslen) == cpathsep()) { s.erase(s.length() - pslen); }
    #ifdef _bmdxpl_Wnds
      if (s.substr(0, pslen) == cpathsep()) { s.erase(0, pslen); }
    #endif
    s0 += s;
    return s0;
}

bool file_utils::has_rightmost_patshep(const std::wstring& s) const { return s_long(s.length())>=pslen && s.substr(s.length()-pslen)==wpathsep(); }
bool file_utils::has_rightmost_patshep(const std::string& s) const { return s_long(s.length())>=pslen && s.substr(s.length()-pslen)==cpathsep(); }

bool file_utils::is_ex_file(const std::wstring& sPath) const { return is_ex_file(wsToBs(sPath)); }
bool file_utils::is_ex_file(const std::string& sPath) const
{
    if (has_rightmost_patshep(sPath))return false;
    std::string p = sPath;
    if ( 0 == access(p.c_str(),F_OK) ) // file exists...
    {
        p += cpathsep();
        return 0 != access(p.c_str(),F_OK); // ...and the directory does not
    }
    else { return false; }
}

bool file_utils::is_ex_dir(const std::wstring& sPath) const { return is_ex_dir(wsToBs(sPath)); }
bool file_utils::is_ex_dir(const std::string& sPath) const
{
    std::string p = sPath;
    if (!has_rightmost_patshep(sPath)) { p += cpathsep(); }
    return 0 == access(p.c_str(), F_OK); // directory exists
}

std::wstring file_utils::expand_env_nr(const std::wstring& s) const
{
  std::wstring s2; _t_wz pos0(0), pos2;
  do {
    pos2 = s.find('%', pos0);
    if (pos2 != nposw)
    {
      s2 += s.substr(pos0, pos2 - pos0);
      pos0 = pos2 + 1;
      pos2 = s.find('%', pos0);
      if (pos2 == nposw) { pos0 -= 1; s2 += s.substr(pos0, s.length() - pos0); break; }
        else if (pos2 > pos0) { const char* p = std::getenv(wsToBs(s.substr(pos0, pos2 - pos0)).c_str()); if (p) { s2 += bsToWs(p); } pos0 = pos2 + 1; }
        else { pos0 = pos2 + 1; }
    }
    else { s2 += s.substr(pos0, s.length() - pos0); break; }
  } while (pos0 < s.length());
  return s2;
}

std::string file_utils::expand_env_nr(const std::string& s) const
{
  std::string s2; _t_wz pos0(0), pos2;
  do {
    pos2 = s.find('%', pos0);
    if (pos2 != nposc)
    {
      s2 += s.substr(pos0, pos2 - pos0);
      pos0 = pos2 + 1;
      pos2 = s.find('%', pos0);
      if (pos2 == nposc) { pos0 -= 1; s2 += s.substr(pos0, s.length() - pos0); break; }
        else if (pos2 > pos0) { const char* p = std::getenv(s.substr(pos0, pos2 - pos0).c_str()); if (p) { s2 += p; } pos0 = pos2 + 1; }
        else { pos0 = pos2 + 1; }
    }
    else { s2 += s.substr(pos0, s.length() - pos0); break; }
  } while (pos0 < s.length());
  return s2;
}

bool file_utils::mk_subdir(const std::wstring& sPath) const { return xmk_subdir(wsToBs(sPath),0); }
bool file_utils::mk_subdir(const std::string& sPath) const { return xmk_subdir(sPath,0); }
unity file_utils::load_string(const std::string& format_string, const std::string& sPath, EFileUtilsPredefinedDir pd, unity::arg ret_s) const
    { return load_string(format_string, bsToWs(sPath), pd, ret_s); }
unity file_utils::load_string(const std::string& format_string, const std::wstring& sPath0, EFileUtilsPredefinedDir pd, unity::arg ret_s) const
{
  typedef unsigned int uint;
  typedef unsigned char uchar;

  const int ReadBufSize = 2048;
  enum EEncodings { local8bit, lsb8bit, utf16le, utf16be };
  bool is_binary(false); bool is_text(false); bool is_local8bit(false); bool is_utf16le(false); bool is_utf16be(false); bool is_lsb8bit(false);
  std::vector<int> encs;
  std::vector<std::string> args = splitToVector(format_string, " ");
  for (unsigned int i = 0; i < args.size(); ++i)
  {
    if (!is_binary && args[i] == "binary") is_binary = true;
    if (!is_text && args[i] == "text") is_text = true;
    if (!is_local8bit && args[i] == "local8bit") { is_local8bit = true; encs.push_back(local8bit); }
    if (!is_utf16le && args[i] == "utf16le") { is_utf16le = true; encs.push_back(utf16le); }
    if (!is_utf16be && args[i] == "utf16be") { is_utf16le = true; encs.push_back(utf16be); }
    if (!is_lsb8bit && args[i] == "lsb8bit") { is_lsb8bit = true; encs.push_back(lsb8bit); }
  }
  bool is_successful = false; std::wstring s0;

  while(true) // once
  {
    if (is_binary == is_text) break;
    if (encs.size() == 0) break;
    std::wstring sPath = complete_path(sPath0,pd);
    if (!is_ex_file(sPath)) { break; }
    std::fstream f(wsToBs(sPath).c_str(), std::ios_base::openmode(std::ios_base::binary|std::ios_base::in));
    if (!f.is_open()) { break; }
    f >> std::noskipws;
    char fileBuf[ReadBufSize];
    f.rdbuf()->pubsetbuf(fileBuf, ReadBufSize);
      // load whole file
    std::string s(static_cast<std::stringstream const&>(std::stringstream() << f.rdbuf()).str());
      // Test for allowed encodings.
    unsigned int pos = 0;
    int ind_enc = -1;
    int ind_enc_local8bit = -1;
    for (unsigned int i = 0; i < encs.size(); ++i)
    {
      switch (encs[i])
      {
          // local8bit, if allowed and all others do not match, is used as the default encoding
        case local8bit: { ind_enc_local8bit = i;  continue; }
        case utf16le: { if (s.size() >= 2 && uchar(s[0]) == 0xff && uchar(s[1]) == 0xfe) { if (is_text) { pos = 2; } ind_enc = i; goto lExitFor1; } break; }
        case utf16be: { if (s.size() >= 2 && uchar(s[0]) == 0xfe && uchar(s[1]) == 0xff) { if (is_text) { pos = 2; } ind_enc = i; goto lExitFor1; } break; }
        case lsb8bit: { ind_enc = i; goto lExitFor1; break; }
        default: { break; }
      }
    }
lExitFor1:
    if (ind_enc < 0) { if (ind_enc_local8bit >= 0) { ind_enc = ind_enc_local8bit; } else { ind_enc = 0; } }

    // Recode file data into the required encoding.
    switch (encs[ind_enc])
    {
    case local8bit:
      if (is_binary) { s0 = bsToWs(s); }
      else // is_text
      {
        std::string s2;
        while (pos < s.size())
        {
          char c = s[pos];
          if (c == '\n') { s2 += "\r\n"; if (pos < s.size() - 1 && s[pos + 1] == '\r') { ++pos; } }
            else if (c == '\r') { s2 += "\r\n"; if (pos < s.size() - 1 && s[pos + 1] == '\n') { ++pos; } }
            else { s2 += c; }
          ++pos;
        }
        s0 = bsToWs(s2);
      }
      is_successful = true;
      break;

    case utf16le:
      while (pos < s.size() - 1)
      {
        wchar_t c = wchar_t(uint(s[pos])) | (wchar_t(s[pos + 1]) << 8);
        if (is_text && c == L'\n') { s0 += L"\r\n"; if (pos < s.size() - 3 && s[pos + 2] == '\r' && s[pos + 3] == '\0') { pos += 2; } }
          else if (is_text && c == L'\r') { s0 += L"\r\n"; if (pos < s.size() - 3 && s[pos + 2] == '\n' && s[pos + 3] == '\0') { pos += 2; } }
          else { s0 += c; }
        pos += 2;
      }
      is_successful = true;
      break;

    case utf16be:
      while (pos < s.size() - 1)
      {
        wchar_t c = (wchar_t(s[pos]) << 8) | wchar_t(uint(s[pos + 1]));
        if (is_text && c == L'\n') { s0 += L"\r\n"; if (pos < s.size() - 3 && s[pos + 2] == '\0' && s[pos + 3] == '\r') { pos += 2; } }
          else if (is_text && c == L'\r') { s0 += L"\r\n"; if (pos < s.size() - 3 && s[pos + 2] == '\0' && s[pos + 3] == '\r') { pos += 2; } }
          else { s0 += c; }
        pos += 2;
      }
      is_successful = true;
      break;

    case lsb8bit:
      if (is_binary) { while (pos < s.size()) { s0 += wchar_t(uint(s[pos])); ++pos; } }
      else // is_text
      {
        while (pos < s.size())
        {
          wchar_t c = wchar_t(uint(s[pos]));
          if (c == L'\n') { s0 += L"\r\n"; if (pos < s.size() - 1 && s.at(pos + 1) == '\r') { ++pos; } }
            else if (c == L'\r') { s0 += L"\r\n"; if (pos < s.size() - 1 && s.at(pos + 1) == '\n') { ++pos; } }
            else { s0 += c; }
          ++pos;
        }
      }
      is_successful = true;
      break;

    default:
      break;
    }

    break;
  } // while (true)

  // Create the resulting std::string.

  if (is_successful) { if (ret_s.parg()) { *ret_s.parg()=s0; return true; } else { return s0; } } else { if (ret_s.parg()) { ret_s.parg()->clear(); return false; } else { return unity(); } }
}

bool file_utils::save_string(const std::string& format_string, const std::wstring& str, const std::string& sTargetFilePath, EFileUtilsPredefinedDir pd, const std::wstring& sUserDefDir) const throw()
  { return save_string(format_string, str, bsToWs(sTargetFilePath), pd, sUserDefDir); }
bool file_utils::save_string(const std::string& format_string, const std::wstring& s0, const std::wstring& sPath0, EFileUtilsPredefinedDir pd, const std::wstring& sUserDefDir) const throw()
{
  try {
    typedef unsigned char uchar;

    bool is_binary(false);
    bool is_text(false);
    bool is_truncate(false);
    bool is_append(false);
    bool is_local8bit(false);
    bool is_lsb8bit(false);
    bool is_utf16le(false);
    bool is_utf16be(false);

    std::vector<std::string> args = splitToVector(format_string, " ");
    for (unsigned int i = 0; i < args.size(); ++i)
    {
      if (!is_binary && args[i] == "binary") is_binary = true;
      if (!is_text && args[i] == "text") is_text = true;
      if (!is_truncate && args[i] == "truncate") is_truncate = true;
      if (!is_append && args[i] == "append") is_append = true;
      if (!is_local8bit && args[i] == "local8bit") is_local8bit = true;
      if (!is_utf16le && args[i] == "utf16le") is_utf16le = true;
      if (!is_utf16be && args[i] == "utf16be") is_utf16be = true;
      if (!is_lsb8bit && args[i] == "lsb8bit") is_lsb8bit = true;
    }

    if (is_binary == is_text) return false;
    if (is_truncate == is_append) return false;
    if (int(is_local8bit) + int(is_lsb8bit) + int(is_utf16le) + int(is_utf16be) != 1) return false;

    const std::wstring* ps2 = &s0;

    std::fstream f(wsToBs(complete_path(sPath0, pd, sUserDefDir)).c_str(), std::ios_base::openmode(std::ios_base::binary|(is_append?std::ios_base::app|std::ios_base::ate:std::ios_base::trunc)|std::ios_base::out));
    if (!f.is_open()) { return false; }
    f << std::noskipws;

    if (is_utf16le)
    {
      if (is_text && (is_truncate || f.tellp() == std::ostream::pos_type(0))) { f << '\xff' << '\xfe'; }
      for (_t_wz i = 0; i < ps2->size(); ++i)
      {
        wchar_t c = ps2->at(i);
        if (is_text && c == L'\n') { f << '\r' << '\0' << '\n' << '\0'; if (i < ps2->size() - 1 && ps2->at(i + 1) == L'\r') { ++i; } }
          else if (is_text && c == L'\r') { f << '\r' << '\0' << '\n' << '\0'; if (i < ps2->size() - 1 && ps2->at(i + 1) == L'\n') { ++i; } }
          else { f << uchar(c & 0xff) << uchar(uchar(c >> 8 & 0xff)); }
      }
    }
    else if (is_utf16be)
    {
      if (is_text && (is_truncate || f.tellp() == std::ostream::pos_type(0))) { f << '\xfe' << '\xff'; }
      for (_t_wz i = 0; i < ps2->size(); ++i)
      {
        wchar_t c = ps2->at(i);
        if (is_text && c == L'\n') { f << '\0' << '\r' << '\0' << '\n'; if (i < ps2->size() - 1 && ps2->at(i + 1) == L'\r') { ++i; } }
          else if (is_text && c == L'\r') { f << '\0' << '\r' << '\0' << '\n'; if (i < ps2->size() - 1 && ps2->at(i + 1) == L'\n') { ++i; } }
          else { f << uchar(c & 0xff) << uchar(uchar(c >> 8 & 0xff)); }
      }
    }
    else if (is_lsb8bit)
    {
      for (_t_wz i = 0; i < ps2->size(); ++i)
      {
        char c = char(ps2->at(i));
        if (is_text && c == '\n') { f << '\r' << '\n'; if (i < ps2->size() - 1 && ps2->at(i + 1) == L'\r') { ++i; } }
          else if (is_text && c == '\r') { f << '\r' << '\n'; if (i < ps2->size() - 1 && ps2->at(i + 1) == L'\n') { ++i; } }
          else { f << c; }
      }
    }
    else // local8bit
    {
      const std::string s2(wsToBs(*ps2));
      for (_t_sz i = 0; i < s2.size(); ++i)
      {
        char c = s2[i];
        if (is_text && c == '\n') { f << '\r' << '\n'; if (i < s2.size() - 1 && s2[i + 1] == '\r') { ++i; } }
          else if (is_text && c == '\r') { f << '\r' << '\n'; if (i < s2.size() - 1 && s2[i + 1] == '\n') { ++i; } }
          else { f << c; }
      }
    }

    bool good=f.good(); f.close(); return good;
  } catch (...) { return false; }
}

bool file_utils::xHasCurDirShortCut(const std::wstring& sPath) const  { return sPath==L"." || sPath.substr(0,1+pslen)==L"."+wpathsep(); }
bool file_utils::xHasCurDirShortCut(const std::string& sPath) const  { return sPath=="." || sPath.substr(0,1+pslen)=="."+cpathsep(); }


s_long unity::sig_struct() throw ()
{
  unity x;
  return ((sizeof(&x) - 1) << 28)
   ^ ((sizeof(unity) - 4 * sizeof(&x)) << 22)
   ^ s_long(((const char*)&x.pmsm - (const char*)&x) << 18)
   ^ s_long(((const char*)&x.ut - (const char*)&x.pmsm) << 12)
   ^ s_long(((const char*)&x._data.p1 - (const char*)&x.ut) << 6)
   ^ s_long(((const char*)&x._data.p2 - (const char*)&x._data.p1));
}

s_long unity::compatibility() const throw () { return _compatibility(); }

}

#if bmdx_part_dllmgmt

namespace bmdx
{
  #ifdef _bmdxpl_Wnds
    struct unity::_mod_exhandle
    {
      typedef s_long (*prequest)(s_long, s_long, const unity*, unity*);
      HINSTANCE handle;
      prequest prq;
      bool b_au; // considered in ~mod_handle()
      _mod_exhandle() throw(): prq(0), b_au(true) {}
      ~_mod_exhandle() throw() { _mod_unload(); }

      bool b_h() const throw() { return handle ? true : false; }
      bool b_rq() const throw() { return prq ? true : false; }

        // 1 - on_calm_result, 0 - already loaded, -1 - failure.
      s_long _mod_load(const char* name) throw()
      {
        if (handle) { return 0; } if (!name || !*name) { return -1; }
        HINSTANCE h = LoadLibraryA(name); if (!h) { return -1; }
        handle = h; prq = (prequest)GetProcAddress(h, "bmdx_mod_request");
        return 1;
      }
      void _mod_unload() throw() { if (handle) { FreeLibrary(handle); handle = 0; } prq = 0; }
    };
  #endif
  #ifdef _bmdxpl_Psx
    struct unity::_mod_exhandle
    {
      typedef s_long (*prequest)(s_long, s_long, const unity*, unity*);
      void* handle;
      prequest prq;
      bool b_au; // considered in ~mod_handle()
      _mod_exhandle() throw(): prq(0), b_au(true) {}
      ~_mod_exhandle() throw() { _mod_unload(); }

      bool b_h() const throw() { return handle ? true : false; }
      bool b_rq() const throw() { return prq ? true : false; }

        // 1 - on_calm_result, 0 - already loaded, -1 - failure.
      s_long _mod_load(const char* name) throw()
      {
        if (handle) { return 0; } if (!name || !*name) { return -1; }
        void* h = dlopen(name, RTLD_NOW); if (!h) { return -1; }
        handle = h; prq = (prequest)dlsym(h, "bmdx_mod_request");
        return 1;
      }
      void _mod_unload() throw() { if (handle) { dlclose(handle); handle = 0; } prq = 0; }
    };
  #endif
}

namespace yk_c
{
  template<> struct vecm::spec<unity::_mod_exhandle> { typedef config_t<unity::_mod_exhandle, 0, 4, 4> config; };
}

namespace bmdx
{

typedef hashx<unity::mod_handle::t_nativehandle, cref_t<unity::_mod_exhandle> > t_hmods;
static t_hmods* _sy_hmods()
  { static int state(0); static storage_es_t<t_hmods> x(state); return x(); }

bool unity::mod_handle::is_valid() const throw()
  { return _rex && _rex.ref().handle; }

unity::mod_handle::t_nativehandle unity::mod_handle::native() const throw()
{
  if (!_rex) { return 0; }
  return _rex.ref().handle;
}

bool unity::mod_handle::operator==(const mod_handle& x) const throw ()
{
  if (!_rex) { return !x._rex; }
  if (!x._rex) { return false; }
  return _rex.ref().handle == x._rex.ref().handle;
}

#ifdef _bmdxpl_Wnds
  void* unity::mod_handle::sym(const char* name) const throw()
  {
    if (!name || !*name) { return 0; }
    if (!_rex) { return 0; }
    return (void*)GetProcAddress(_rex.ref().handle, name);
  }
#endif
#ifdef _bmdxpl_Psx
  void* unity::mod_handle::sym(const char* name) const throw()
  {
    if (!name || !*name) { return 0; }
    if (!_rex) { return 0; }
    return (void*)dlsym(_rex.ref().handle, name);
  }
#endif

unity::mod_handle unity::mod(const char* name, bool b_au) throw ()
{
  unity::mod_handle mh;
  cref_t<_mod_exhandle> rd;
  if (!rd.create0(true)) { return mh; }
  if (rd._pnonc_u()->_mod_load(name) < 0) { return mh; } // +1 load

  critsec_t<t_hmods> __lock(100, 60000); if (!__lock.is_locked()) { return mh; }
  t_hmods* ph = _sy_hmods(); if (!ph) { return mh; }
  const t_hmods::entry* e(0);
  int res = ph->insert(rd.ref().handle, &e);
  if (res == 1) { e->v = rd; }
    else if (res == 0) { rd = e->v; } // +1 unload (overwritten object is destroyed)
    else { return mh; }
  if (!b_au) { e->v._pnonc_u()->b_au = false; }
  mh._rex = rd;
  mh._b_au = b_au;
  return mh;
}

s_long unity::mod_handle::request(const unity& para, unity& retval) const throw()
{
  if (!(&para && &retval)) { return -2; }
  if (!(_rex && _rex.ref().handle)) { return -2; }
  critsec_t<t_hmods> __lock(100, 60000); if (!__lock.is_locked()) { return -2; }
  t_hmods* ph = _sy_hmods(); if (!ph) { return -2; }
  const t_hmods::entry* e = ph->find(_rex.ref().handle); if (!e) { return -2; }
  return e->v.ref().prq(sig_struct(), 1, &para, &retval);
}

unity::mod_handle::mod_handle() throw() : _rex(), _b_au(true) {}
unity::mod_handle::~mod_handle() throw()
{

  if (!(_rex && _rex.ref().handle && _b_au)) { return; }
  critsec_t<t_hmods> __lock(100, 60000); if (!__lock.is_locked()) { return; }
  t_hmods* ph = _sy_hmods(); if (!ph) { return; }
  const t_hmods::entry* e = ph->find(_rex.ref().handle); if (!e) { return; }
  if (e->v.ref().b_au && e->v.n_refs() <= 2) { ph->remove_e(e); }
}

}
#endif

#if bmdx_part_dispatcher_mt

namespace bmdx
{

template<> o_proxy<i_dispatcher_mt> o_proxy_base<i_dispatcher_mt>::__server;

struct cch_slot
{
  struct qbi_value { cref_t<std::string, meta::nothing> msg; cref_t<std::wstring, meta::nothing> src; };
  struct qs_value { unity hmsg; cref_t<carray_t<char> > bin; }; // bin may contain no object

  critsec_t<dispatcher_mt>::csdata lkd; // for locking pins and queues against multithreaded read/write
  cref_t<std::string, meta::nothing> r_pin; // input pin data
  cref_t<fifo_m11_t<std::string, meta::nothing>, meta::nothing> r_q; // input queue
  cref_t<fifo_m11_t<qbi_value, meta::nothing>, meta::nothing> r_qbi; // input command queue
  cref_t<fifo_m11_t<qs_value, meta::nothing>, meta::nothing> r_qs; // subscription queue
  cref_t<std::wstring, meta::nothing> r_lsrc; // last source address, for input command pins and queues
  cref_t<hashx<unity, s_long>, meta::nothing> r_hsubs;
    //--
    //  a) for qs slot: "output list" { target address (string), 0 }
    //  b) for subscriber slot (pi, qi): "input list" { source address (string), flags }
    //    flags: remote op. in progress: 0x1 - subscribing, 0x2 - unsubscribing, 0x4 - checking.
    //  c) for any slot (incl. pi, qi), except qs: no reference (no inputs, or this slot type cannot subscribe)
  s_long phase;
    //--
    //  0 - no msg.
    //  1 - input msg arrived.
    //      2 - input msg popped.
    //      3 - command response pushed by the receiver and forwarded to sender.
    //  4 - command sent.
    //      5 - command response received.
    //      6 - command response popped.
    //  pbo: 0, 6 --> 4 --> 5 --> 6
    //  pbi: 0, 3 --> 1 --> 2 --> 3
    //  qbi: 0, 3 --> 2 --> 3
    //  pi: 0, 2 --> 1 --> 2
  s_long cqsflags;
    //-- mask 0x3 -- delivery type:
    //    1 - immediate, 2 - thread, 3 - disp.
    //    0 - slot is not a qs.
    //  0x4 -- accept input from all threads (if 0, accepts input from its own thread only).
    //  0x8 -- accept global input from disp. thread.
    //  0x10 -- fixed recipients list (no subscribing during run-time)
    inline s_long qs_delivery_type() const { return cqsflags & 0x3; }
    inline bool qs_deliv_imm() const { return (cqsflags & 0x3) == 1; }
    inline bool qs_deliv_thread() const { return (cqsflags & 0x3) == 2; }
    inline bool qs_deliv_disp() const { return (cqsflags & 0x3) == 3; }
    inline bool qs_input_any_th() const { return (cqsflags & 0x4) != 0; }
    inline bool qs_input_lpa() const { return (cqsflags & 0x8) != 0; }
    inline bool qs_output_fixed() const { return (cqsflags & 0x10) != 0; }

  cch_slot() : phase(0), cqsflags(0) {}
};
struct cch_thread
{
  hashx<std::wstring, cref_t<cch_slot, dispatcher_mt> > h_sl;
  hashx<unity, int> h_qs_imm; // { qs slot name (string), 0 } thread's qs slots with immediate delivery
  hashx<unity, int> h_qs_thread; // { qs slot name (string), 0 } thread's qs slots with delivery by thread periodic()
  bool b_disp;
  cch_thread() : b_disp(false) {}
};
} // namespace bmdx
namespace yk_c
{
  using namespace bmdx;
  template<> struct vecm::spec<cch_slot> { struct aux : vecm::config_aux<cch_slot> { }; typedef config_t<cch_slot, 1, 4, 1, aux> config; };
  template<> struct vecm::spec<cch_thread> { struct aux : vecm::config_aux<cch_thread> { }; typedef config_t<cch_thread, 1, 4, 1, aux> config; };
}
namespace bmdx
{
struct dispatcher_mt::cch_session
{
  cch_session() : exitmode(1), ses_state(-1), nprx(0), qsdt(-1), qsd_prio(4) {}

  int exitmode;
  volatile int ses_state; // true until the curent session is closed (when dispatcher object is destroyed)

  critsec_t<dispatcher_mt>::csdata lkm_ths;
    //-- locks main cch_session members modification:
    //    1) h_ths during a thread navigating from h_ths to any of its leaves.
    //    2) h_qs_disp during modification on qs slot creation/removal,
    //      and during copying on messages delivery.
  hashx<std::wstring, cch_thread> h_ths;
    //-- NOTE h_ths destruction is that of enclosing object,
    //  which is held reference-counted by multiple objects.
  hashx<unity, unity> h_qs_disp; // { qs slot name (string), owning thread name (string) } -- initForCallback-global qs slots with from different threads; delivery by disp. thread periodic()

  critsec_t<dispatcher_mt>::csdata lkm_nprx; // nprx incr/decr lock
  volatile s_long nprx; // nprx counts the dispatcher_mt single object + all thread_proxy objects

  s_ll qsdt; // (const) >= 0: global qs delivery period, mcs; == -1: no automatic delivery

  std::wstring name_pr; // initForCallback name (const)
  std::wstring name_th_disp; // disp thread name (const)

  threadctl tc_qsd; // qs messages automatic delivery
  s_long qsd_prio; // last set tc_qsd priority
};
struct dispatcher_mt::thread_proxy : i_dispatcher_mt
{
  typedef fifo_m11_t<std::string, meta::nothing> t_q;
  typedef fifo_m11_t<cch_slot::qbi_value, meta::nothing> t_qbi;
  typedef fifo_m11_t<std::string, meta::nothing>::t_elem t_qel;
  typedef fifo_m11_t<cch_slot::qbi_value, meta::nothing>::t_elem t_qbiel;

   // A queue in a slot. Accessed only under slot lock, so does not need its own element-wise lock.
  template<class T>
  struct aux_fifo : fifo_m11_t<T, meta::nothing>
  {
    typedef typename fifo_m11_t<T>::t_elem t_elem;
    s_long _push_nonblk(const t_elem& x) throw()
    {
      if (!this->is_valid()) { return -5; }
      s_long s(this->_isup), s2(s + 1), r(this->_irec); if (s2 >= this->_elems.n()) { s2 = 0; }
      if (s2 == r) { return -2; } // near-full (2 elems. free) - considered as overflow
      t_elem* p = this->_elems.template pval_0u<t_elem>(s); *p = x;
      if (p->has_ref()) { this->_isup = s2; return 1; }
      return -4;
    }

    t_elem _pop_nonblk(bool b_pop) throw()
    {
      if (!this->is_valid()) { return t_elem(); }
      s_long r(this->_irec), s(this->_isup);
      if (r == s) { return t_elem(); }
      s_long r2 = r + 1; if (r2 >= this->_elems.n()) { r2 = 0; }
      t_elem* p = this->_elems.template pval_0u<t_elem>(r);
      t_elem x = *p;
      if (b_pop) { p->clear(); this->_irec = r2; }
      return x;
    }
  };

  struct tc_autodelivery : threadctl::ctx_base
  {
    virtual void _thread_proc()
    {
      cref_t<cch_session>& _r_ths = *this->pdata<cref_t<cch_session> >();
      try {
        cch_session& ths = *_r_ths._pnonc_u();
        while (true)
        {
          if (ths.ses_state == 1) { _s_qs_deliver(_r_ths, ths.name_th_disp, 0); }
          const s_ll delta = 100000;
          for (s_ll dt = ths.qsdt; dt > 0; dt -= delta)
          {
            sleep_mcs(dt < delta ? dt : delta);
            if (ths.ses_state == 0 || ths.nprx <= 0) { goto lExit; } // during dispatcher_mt(), ses_state == -1, but exit does not occur since nprx == 1
          }
        }
      lExit:;
      } catch (...) {}
      _r_ths.clear();
    }
  };

    // Append values from array src to array dest.
    //  If dest is not utUnityArray or utStringArray, it's cleared and initialized as 1-based empty utUnityArray.
    // NOTE If operation fails, it generates exception, leaving dest partially modified.
  static void __append_vals(unity& dest, const unity& src, s_long n_skip = 0)
  {
    if (n_skip < 0) { n_skip = 0; }
    if (dest.utype() == utStringArray)
    {
      if (src.isArray()) { for (s_long i = src.arrlb() + n_skip; i <= src.arrub(); ++i) { dest.arr_append(src.vstr(i)); } }
        else { dest.arr_append(src.vstr()); }
    }
    else
    {
      if (dest.utype() != utUnityArray) { dest.clear(); dest.arr_init<utUnityArray>(1); }
      if (src.utype() == utUnityArray) { for (s_long i = src.arrlb() + n_skip; i <= src.arrub(); ++i) { dest.arr_append(src.ref<unity>(i)); } }
        else if (src.isArray()) { for (s_long i = src.arrlb() + n_skip; i <= src.arrub(); ++i) { dest.arr_append(src.val<unity>(i)); } }
        else { dest.arr_append(src); }
    }
  }

  static s_long _s_add_slots_nl(cch_session& ths, const std::wstring& k_th, const unity& slotscfg, hashx<std::wstring, hashx<std::wstring, s_long> >& hsubs_acc) throw();
  static s_long _s_update_subs_inputs(cch_session& ths, const hashx<std::wstring, hashx<std::wstring, s_long> >& hsubs) throw();

    // sn may be string or already decoded array.
    //  pn1 may be specified to receive 1st part of slot name (in decoded form).
    //  pntail may be specified to receive [2nd..end) parts of slot name.
    // Returns:
    //    1 and recoded slot name, if sn is valid.
    //    -1 - if the name is not valid. retval is not changed.
    //    -2 - failure. retval is not changed.
  static s_long __recode_slotname(const unity& sn, std::wstring& retval, std::wstring* pn1 = 0, std::wstring* pntail = 0) throw()
  {
    try {
      unity _a; const unity* pa(0);
      if (sn.isString()) { paramline().decode1v(sn.vstr(), _a); pa = &_a; }
        else { pa = &sn; }
      if (!(pa->isString() || (pa->isArray() && pa->arrsz() >= 2))) { return -1; }
      std::wstring sn1 = pa->isArray() ? pa->vstr(pa->arrlb()) : pa->vstr();
      if (!sltchk_main(sn1)) { return -1; }
      if (pn1) { *pn1 = sn1; }
      if (pntail) { if (pa->isArray()) { unity x; __append_vals(x, *pa, 1); *pntail = paramline().encode1v(x); } else { pntail->clear(); } }
      retval = paramline().encode1v(*pa);
      return 1;
    } catch (...) { return -2; }
  }

  struct address
  {
      // Empty, or valid address array (1-based, utUnityArray, containing only strings).
    const unity& addr() const throw() { return _addr; }

    bool is_empty() const throw() { return _addr.isEmpty(); }
    s_long n() const throw() { return _addr.isEmpty() ? 0 : _addr.arrsz(); }

      // ind is 1..n().
    const unity& addr(s_long ind) const { return _addr.ref<utUnity>(ind); }

      // Returns string representation of the address,
      //    or empty string if addr() is empty.
    std::wstring wstr_addr() const { return paramline().encode1v(_addr); }

      // Returns string representation of slot name part of the address,
      //    or empty string if addr() is empty.
    std::wstring wstr_sln() const
    {
      std::wstring s; if (_addr.isEmpty()) { return s; } s_long ind = sln_ind(); if (ind < 1) { return s; }
      if (ind >= _addr.arrub()) { s = paramline().encode1v(_addr.vstr(ind)); }
        else { unity _a; __append_vals(_a, _addr, ind - _addr.arrlb()); s = paramline().encode1v(_a); }
      return s;
    }

      // Returns slot name part of the address in the form of scalar (if slot name has 1 part)
      //    or array (if slot name has >= 2 parts).
      // Returns empty value if addr() is empty.
    unity sln_v() const
    {
      unity x; if (_addr.isEmpty()) { return x; } s_long ind = sln_ind(); if (ind < 1) { return x; }
      if (ind >= _addr.arrub()) { x = _addr.ref<utUnity>(ind); }
        else { __append_vals(x, _addr, ind - _addr.arrlb()); }
      return x;
    }

      // Returns string representation of the first element of slot name
      //  (including slot type and name root),
      //    or empty string if addr() is empty.
    std::wstring wstr_sln_1() const
    {
      std::wstring s; if (_addr.isEmpty()) { return s; } s_long ind = sln_ind(); if (ind < 1) { return s; }
      s = _addr.vstr(ind);
      return s;
    }

      // Returns string representation of slot name part of the address,
      //    without the first part (type/name root).
      // Returns empty string if addr() is empty or slot name has only one part.
    std::wstring wstr_sln_tail() const
    {
      std::wstring s; if (_addr.isEmpty()) { return s; } s_long ind = sln_ind(); if (ind < 1 || ind + 1 > _addr.arrub()) { return s; }
      unity _a; __append_vals(_a, _addr, ind + 1 - _addr.arrlb());
      paramline().encode1v(_a, s);
      return s;
    }

      // Returns string representation of thread name part of the address,
      //    or empty string if addr() does not contain thread name.
    std::wstring wstr_thn() const
    {
      std::wstring s; if (_addr.isEmpty()) { return s; }
      const _fls75 k1 = _addr.ref<utUnity>(1).vflstr();
      s_long ind = -1;
      if (k1 == L"LP") { ind = 2; }
        else if (k1 == L"LM") { ind = 3; }
        else if (k1 == L"R") { ind = 4; }
      if (ind >= 0) { s = _addr.vstr(ind); }
      return s;
    }

      // Returns string representation of initForCallback name part of the address,
      //    or empty string if addr() does not contain initForCallback name.
    std::wstring wstr_pn() const
    {
      std::wstring s; if (_addr.isEmpty()) { return s; }
      const _fls75 k1 = _addr.ref<utUnity>(1).vflstr();
      s_long ind = -1;
      if (k1 == L"LM") { ind = 2; }
        else if (k1 == L"R") { ind = 3; }
        else if (k1 == L"RPA") { ind = 3; }
      if (ind >= 0) { s = _addr.vstr(ind); }
      return s;
    }

      // Returns string representation of host name part of the (remote) address,
      //    or empty string if addr() does not contain host name.
    std::wstring wstr_hostn() const
    {
      std::wstring s; if (_addr.isEmpty()) { return s; }
      const _fls75 k1 = _addr.ref<utUnity>(1).vflstr();
      if (k1.length() > 0 && k1[0] == L'R') { s = _addr.vstr(2); }
      return s;
    }

      // x may be string or already decoded array.
      // Returns:
      //    1 if set decoded, corrected address.
      //    -1 if x is not a valid address. addr() is cleared.
      //    -2 if setting failed. addr() is cleared.
    s_long set_addr(const unity& x) throw()
    {
      _addr.clear();
      try {
        unity _a; const unity* pa(0); if (x.isString()) { paramline().decode1v(x.vstr(), _a); pa = &_a; } else { pa = &x; }
        if (!(pa->isArray() && pa->arrsz() >= 2)) { return -1; }
        s_long nb = pa->arrlb();
        const std::wstring k1 = pa->vstr(nb + 0);
        if (!is_addr_cat(k1)) { return -1; }
        if (k1 == L"LP") { if (pa->arrsz() < 3 || !thnchk(pa->vstr(nb + 1)) || !sltchk_main(pa->vstr(nb + 2))) { return -1; } }
          else if (k1 == L"LPA") { if (!sltchk_main_qs(pa->vstr(nb + 1))) { return -1; } }
          else if (k1 == L"LM") { if (pa->arrsz() < 4 || !pnchk(pa->vstr(nb + 1)) || !thnchk(pa->vstr(nb + 2)) || !sltchk_main(pa->vstr(nb + 3))) { return -1; } }
          else if (k1 == L"LMA") { if (!sltchk_main_qs(pa->vstr(nb + 1))) { return -1; } }
          else if (k1 == L"R") { if (pa->arrsz() < 5 || !hostportchk(pa->vstr(nb + 1)) || !pnchk(pa->vstr(nb + 2)) || !thnchk(pa->vstr(nb + 3)) || !sltchk_main(pa->vstr(nb + 4))) { return -1; } }
          else if (k1 == L"RMA") { if (pa->arrsz() < 3 || !hostportchk(pa->vstr(nb + 1)) || !sltchk_main_qs(pa->vstr(nb + 2))) { return -1; } }
          else if (k1 == L"RPA") { if (pa->arrsz() < 4 || !hostportchk(pa->vstr(nb + 1)) || !pnchk(pa->vstr(nb + 2)) || !sltchk_main_qs(pa->vstr(nb + 3))) { return -1; } }
          else { return -1; }
        _addr.arr_init<utUnity>(1); _addr.arrsz_(pa->arrsz());
        for (s_long i = 0; i < pa->arrsz(); ++i) { _addr.arr_set(1 + i, pa->vstr(nb + i)); }
        return 1;
      } catch (...) { _addr.clear(); return -2; }
    }

      // thn - thread name, sl - slot name (array, or string, representing paramline-encoded scalar or array).
      // Returns:
      //    1 set successfully.
      //    -1 thn or sl is not valid. addr() is cleared.
      //    -2 if setting failed. addr() is cleared.
    s_long set_addr_LP(const std::wstring& thn, const unity& sl)
    {
      _addr.clear();
      try {
        if (!thnchk(thn)) { return -1; }

        unity _a; const unity* pa(0); if (sl.isString()) { paramline().decode1v(sl.vstr(), _a); pa = &_a; } else { pa = &sl; }
        if (pa->isArray())
        {
          s_long nb = pa->arrlb(); if (!sltchk_main(pa->vstr(nb + 0))) { return -1; }
          _addr.arr_init<utUnity>(1); _addr.arr_append(L"LP"); _addr.arr_append(thn);
          for (s_long i = 0; i < pa->arrsz(); ++i) { _addr.arr_append(pa->vstr(nb + i)); }
        }
        else
        {
          if (!sltchk_main(pa->vstr())) { return -1; }
          _addr.arr_init<utUnity>(1); _addr.arr_append(L"LP"); _addr.arr_append(thn); _addr.arr_append(pa->vstr());
        }
        return 1;
      } catch (...) { _addr.clear(); return -2; }
    }

    bool isLP_any() const throw() { return isLP() || isLPA(); }
    bool isLP() const throw() { if (_addr.isEmpty()) { return false; } try { return _addr.ref<utUnity>(1).vflstr() == L"LP"; } catch (...) {} return false; }
    bool isLPA() const throw() { if (_addr.isEmpty()) { return false; } try { return _addr.ref<utUnity>(1).vflstr() == L"LPA"; } catch (...) {} return false; }
    bool isLM() const throw() { if (_addr.isEmpty()) { return false; } try { return _addr.ref<utUnity>(1).vflstr() == L"LM"; } catch (...) {} return false; }
    bool isLMA() const throw() { if (_addr.isEmpty()) { return false; } try { return _addr.ref<utUnity>(1).vflstr() == L"LMA"; } catch (...) {} return false; }
    bool isR() const throw() { if (_addr.isEmpty()) { return false; } try { return _addr.ref<utUnity>(1).vflstr() == L"R"; } catch (...) {} return false; }
    bool isRMA() const throw() { if (_addr.isEmpty()) { return false; } try { return _addr.ref<utUnity>(1).vflstr() == L"RMA"; } catch (...) {} return false; }
    bool isRPA() const throw() { if (_addr.isEmpty()) { return false; } try { return _addr.ref<utUnity>(1).vflstr() == L"RPA"; } catch (...) {} return false; }

      // Returns base index of slot name in addr():
      //  >= 2 on on_calm_result.
      //  -1 if addr() is empty.
      //  -2 on failure.
    s_long sln_ind() const throw ()
    {
      try {
        if (_addr.isEmpty()) { return -1; }
        const _fls75 k1 = _addr.ref<utUnity>(1).vflstr();
        s_long ind = -2;
        if (k1 == L"LP") { ind = 3; }
          else if (k1 == L"LPA") { ind = 2; }
          else if (k1 == L"LM") { ind = 4; }
          else if (k1 == L"LMA") { ind = 2; }
          else if (k1 == L"R") { ind = 5; }
          else if (k1 == L"RMA") { ind = 3; }
          else if (k1 == L"RPA") { ind = 4; }
        return ind;
      } catch (...) { return -2; }
    }

      // Returns slot type part,
      //  or empty string if addr() is empty or on failure.
    _fls75 slt() const throw ()
    {
      try {
        s_long ind = sln_ind(); if (ind < 1) { return _fls75(); }
        _fls75 sln1 = _addr.ref<utUnity>(ind).vflstr();
        return sln1.substr(0, sln1.find('_'));
      } catch (...) {}
      return _fls75();
    }

  private:
    unity _addr;
  };

    // Encodes message hash or map (hm), converts the result to char std::string,
    //    adds header length prefix. If buf is given, buf length and data are appended.
    // retmsg: the resulting std::string.
    // Limits:
    //    max. hm size when encoded into std::string - 2^30 16-bit characters.
    //    max. buf size - 2^31-1 chars.
    //    sum of sizes must be < std::string::max_size().
    // Returns:
    //  1 - on_calm_result.
    //  -1 - hm is not a hash or map.
    //  -2 - failure (mem. alloc. or smth. else).
  static s_long __aux_msg_encode(const unity& hm, const arrayref_t<char>* buf, std::string& retmsg) throw()
  {
    try {
        if (!hm.isAssoc()) { return -1; }
      std::wstring sm; std::string pr1(11, ':'), pr2(11, ':');
        paramline().encode(hm, sm);
        meta::s_ll l1 = sm.length(); l1 *= 2; if (!(l1 >= 0 && l1 <= meta::s_long_mp)) { return -2; }
          str_from_s_ll(l1, &pr1[0], 10); if (buf) { str_from_s_ll(buf->n(), &pr2[0], 10); }
        meta::s_ll lpr1 = pr1.find(':') + 1; meta::s_ll lpr2 = buf ? pr2.find(':') + 1 : 0; meta::s_ll n = l1 + lpr1 + (buf ? 2 + lpr2 + buf->n() : 0);
        typedef unsigned long long u_ll; if (u_ll(n) > u_ll(retmsg.max_size())) { return -2; }
      retmsg.clear(); retmsg.resize(_t_sz(n), ' ');
        _yk_reg char* pdest = &retmsg[0];
        if (true) { _yk_reg char* pc1 = &pr1[0]; _yk_reg char* p_end = pdest + lpr1; while (pdest != p_end) { *pdest++ = *pc1++; } }
        if (l1) { _yk_reg wchar_t* pw1 = &sm[0]; _yk_reg wchar_t* pw_end = pw1 + sm.length(); while (pw1 != pw_end) { _yk_reg wchar_t c = *pw1++; *pdest++ = char(c); c >>= 8; *pdest++ = char(c); } }
        if (buf)
        {
          *pdest++ = '\r'; *pdest++ = '\n';
          if (true) { _yk_reg char* pc2 = &pr2[0]; _yk_reg char* p_end = pdest + lpr2; while (pdest != p_end) { *pdest++ = *pc2++; } }
          yk_c::bytes::memmove_t<char>::F(pdest, buf->pd(), buf->n());
        }
      return 1;
    } catch (...) { return -2; }
  }

    // Decodes message and binary attachment (if exists).
    //    Message must have message part, and optional binary data part.
    //    Part format:
    //    [ \t\r\n]*([0-9]{1,10})([^:]*):(.*)
    //    \1 is decimal length of data (\3). \2 is optional metadata.
    //    Length must not exceed 2^31 - 1.
    // b_rmstr:
    //    true - retmsg must be std::wstring (unity / utString).
    //    false - retmsg must be hash, containing decoded data (unity / utHash).
    // b_discard_bin: ignore message binary part if it is received but no retbuf given.
    // Returns:
    //  2 - on_calm_result, got 2 parts.
    //  1 - on_calm_result, got 1 part.
    //  0 - got empty message, no parts.
    //  -1 - msg has wrong format.
    //  -2 - failure (mem. alloc. or smth else).
  static s_long __aux_msg_decode(const std::string& msg, bool b_rmstr, bool b_discard_bin, unity& retmsg, _carray_base_t<char>* retbuf) throw()
  {
    try {
      retmsg.u_clear(b_rmstr ? utString : utHash);

      typedef std::string::size_type _t_sz; typedef unsigned long long u_ll; typedef unsigned char u_char;
      _t_sz n(msg.length()); const char* ptr1(0); const char* ptr2(0);
      if (n == 0) { if (retbuf) { retbuf->realloc(0, 0, 0, 0); } return 0; }
      meta::s_ll l1(0), l2(0);
      const char* p = &msg[0]; const char* p_end = p + n;

      bool b = false;
      while (p != p_end) { char c = *p; if (!(c == ' ' || c == '\t' || c == '\r' || c == '\n')) { break; } ++p; }
      if (p == p_end) { if (retbuf) { retbuf->realloc(0, 0, 0, 0); } return 0; }
      if (!(*p >= '0' && *p <= '9')) { return -1; }
      while (p != p_end) { char c = *p; if (!(c >= '0' && c <= '9')) { break; } ++p; l1 = l1 * 10 + meta::s_ll(c - '0'); if (l1 > meta::s_long_mp) { return -1; } }
      while (p != p_end) { if (*p++ == ':') { b = true; break; } }
      if (!b || p_end - p < l1) { return -1; } ptr1 = p; p += l1;
      if (l1 & 1) { return -1; }
      if (u_ll(l1 >> 1) > u_ll(std::wstring().max_size())) { return -2; }

      b = false;
      while (p != p_end) { char c = *p; if (!(c == ' ' || c == '\t' || c == '\r' || c == '\n')) { break; } ++p; }
      bool bp2 = p != p_end; bool bp2ov = bp2 && retbuf == 0 && !b_discard_bin;
//        bool bp2ov_done = false;
      if (bp2)
      {
        if (!(*p >= '0' && *p <= '9')) { return -1; }
        while (p != p_end) { char c = *p; if (!(c >= '0' && c <= '9')) { break; } ++p; l2 = l2 * 10 + meta::s_ll(c - '0'); if (l2 > meta::s_long_mp) { return -1; } }
        while (p != p_end) { if (*p++ == ':') { b = true; break; } }
        if (!b || p_end - p < l2) { return -1; } ptr2 = p;
        if (bp2ov && u_ll(l2) > u_ll(std::wstring().max_size())) { return -2; }
      }


      if (b_rmstr)
      {
        std::wstring* pws_r = retmsg.pval<utString>();
        std::wstring s2_r; if (!pws_r) { pws_r = &s2_r; }
        pws_r->resize(_t_wz(l1 >> 1), L' ');
        if (l1) { _yk_reg wchar_t* pw = &(*pws_r)[0]; _yk_reg const char* p = ptr1; _yk_reg const char* p_end = p + l1; while (p != p_end) { ++p; _yk_reg wchar_t c = *p; c <<= 8; --p; c |= *p; p += 2; *pw++ = c; } }
        if (bp2ov)
        {
          unity h; unity k_bin = L"bin";
          paramline().decode(*pws_r, h, false);
//            bp2ov_done = h.hash_locate(k_bin);
          unity& x = h.hash(k_bin); x.u_clear(utString);
          if (l2)
          {
            std::wstring* pws = x.pval<utString>(); // 0 if another module strings are incompatible
            std::wstring s2; if (!pws) { pws = &s2; }
            pws->resize(_t_wz(l2));
            if (true) { _yk_reg wchar_t* pw = &(*pws)[0]; _yk_reg const char* p = ptr2; _yk_reg const char* p_end = p + l2; while (p != p_end) { *pw++ = wchar_t(u_char(*p++)); } }
            if (pws == &s2) { x = s2; } // copy to incompat. string
          }
          paramline().encode(h, *pws_r);
        }
        else if (retbuf)
        {
          if (bp2) { if (!retbuf->realloc(l2, 0, 0, 0)) { return -2; } bytes::memmove_t<char>::F(retbuf->pd(), ptr2, l2); }
            else { retbuf->realloc(0, 0, 0, 0); }
        }
        if (pws_r == &s2_r) { retmsg = s2_r; }
      }
      else
      {
        std::wstring wmsg1(_t_wz(l1 >> 1), L' ');
        if (l1) { _yk_reg wchar_t* pw = &wmsg1[0]; _yk_reg const char* p = ptr1; _yk_reg const char* p_end = p + l1; while (p != p_end) { ++p; _yk_reg wchar_t c = *p; c <<= 8; --p; c |= *p; p += 2; *pw++ = c; } }
        paramline().decode(wmsg1, retmsg, false);
        if (bp2ov)
        {
          unity k_bin = L"bin";
//            bp2ov_done = retmsg.hash_locate(k_bin);
          unity& x = retmsg.hash(k_bin); x.u_clear(utString);
          if (l2)
          {
            std::wstring* pws = x.pval<utString>(); // 0 if another module strings are incompatible
            std::wstring s2; if (!pws) { pws = &s2; }
            pws->resize(_t_wz(l2));
            if (true) { _yk_reg wchar_t* pw = &(*pws)[0]; _yk_reg const char* p = ptr2; _yk_reg const char* p_end = p + l2; while (p != p_end) { *pw++ = wchar_t(u_char(*p++)); } }
            if (pws == &s2) { x = s2; } // copy to incompat. string
          }
        }
        else if (retbuf)
        {
          if (bp2) { if (!retbuf->realloc(l2, 0, 0, 0)) { return -2; } bytes::memmove_t<char>::F(retbuf->pd(), ptr2, l2); }
            else { retbuf->realloc(0, 0, 0, 0); }
        }
      }

      return bp2 ? 2 : 1;
    } catch (...) { return -2; }
  }


private:
  std::wstring _name_th;
  std::wstring _name_pr;
  cref_t<dispatcher_mt::cch_session> _r_ths;

  thread_proxy(const thread_proxy&); thread_proxy& operator=(const thread_proxy& x);

public:
  thread_proxy() {}
  virtual ~thread_proxy()
  {
    if (_r_ths)
    {
      critsec_t<dispatcher_mt> __lock(10, -1, &_r_ths._pnonc_u()->lkm_nprx); if (sizeof(__lock)) {}
      if ((_r_ths._pnonc_u()->nprx -= 1) <= 0) { _r_ths._pnonc_u()->ses_state = 0; }
    }
    _r_ths.clear();
  }

    // 1 - on_calm_result.
    // -1 - invalid thread_name or r_ths.
    // -2 - mem. alloc. error.
    // -3 - disp. session is closed.
    // -4 - specified thread does not exist in dispatcher cache.
  static s_long _s_init(thread_proxy& x, const cref_t<dispatcher_mt::cch_session>& _r_ths, const std::wstring& thread_name) throw()
  {
    if (!(thread_name.length() > 0 && _r_ths)) { return -1; }
    if (_r_ths.ref().ses_state != 1) { return -3; }
    try {
      std::wstring process_name;
      if (true)
      {
        critsec_t<dispatcher_mt> __lock(10, -1, &_r_ths._pnonc_u()->lkm_ths); if (sizeof(__lock)) {}
        if (_r_ths.ref().ses_state != 1) { return -3; }
        process_name = _r_ths.ref().name_pr;
        if (!_r_ths.ref().h_ths.find(thread_name)) { return -4; }
      }
      x._name_th = thread_name;
      x._name_pr = process_name;

      if (true)
      {
        critsec_t<dispatcher_mt> __lock(10, -1, &_r_ths._pnonc_u()->lkm_nprx); if (sizeof(__lock)) {}
        x._r_ths = _r_ths;
        x._r_ths._pnonc_u()->nprx += 1;
      }
      return 1;
    } catch (...) { return -2; }
  }

  static inline bool thnchk(const std::wstring& s) throw () { return !(s.length() == 0 || (s.length() >= 2 && s[0] == L'_' && s[1] == L'_')); }
  static inline bool pnchk(const std::wstring& s) throw () { return s.length() != 0 && !is_addr_cat(s); }
  static inline bool hostportchk(const std::wstring& s) throw () { return s.length() != 0 && !is_addr_cat(s); }

  static inline bool is_addr_cat(const std::wstring& s) throw ()
  {
    if (s.length() == 0) { return false; }
    if (s[0] == L'L')
    {
      switch (s.length())
      {
        case 2: return s[1] == L'P' || s[1] == L'M';
        case 3: return (s[1] == L'P' || s[1] == L'M') && s[2] == L'A';
        default: return false;
      }
    }
    else if (s[0] == L'R')
    {
      if (s.length() == 1) { return true; }
      if (s.length() != 3) { return false; }
      return (s[1] == L'P' || s[1] == L'M') && s[2] == L'A';
    }
    return false;
  }

  static inline bool sltchk_main(const std::wstring& st)
    {
      if (st.length() < 4) { return false; }
      if (st[1] == L'b')
      {
        if (st.length() < 5 || st[3] != L'_') { return false; }
        if (st[0] == 'p' && (st[2] == 'o' || st[2] == 'i')) { return true; }
          else if (st[0] == L'q' && st[2] == 'i') { return true; }
      }
      else
      {
        if (st[2] != L'_') { return false; }
        if (st[0] == L'p' && (st[1] == 'o' || st[1] == 'i')) { return true; }
          else if (st[0] == L'q' && (st[1] == 'i' || st[1] == 's')) { return true; }
      }
      return false;
    }
  static inline bool sltchk_main_qs(const std::wstring& st) { return sltchk_main(st) && st[0] == L'q' && st[1] == L's'; }
  static inline bool sltchk_qs(const _fls75& st) { return st.length() >= 2 && st[0] == L'q' && st[1] == L's'; }
  static inline bool sltchk_subscriber(const _fls75& st) throw () { return st.length() >= 2 && (st[0] == 'p' || st[0] == 'q') && st[1] == 'i'; }
  static inline bool sltchk_main_o(const std::wstring& st)
    {
      if (st.length() < 4) { return false; }
      if (st[1] == L'b')
      {
        if (st.length() < 5 || st[3] != L'_') { return false; }
        if (st[0] == 'p' && (st[2] == 'o' || st[2] == 'i')) { return true; }
          else if (st[0] == L'q') { if (st[2] == 'i') { return true; } }
      }
      else
      {
        if (st[2] != L'_') { return false; }
        if (st[0] == L'p' && st[1] == 'o') { return true; }
          else if (st[0] == L'q' && st[1] == 's') { return true; }
      }
      return false;
    }
  static inline bool sltchk_main_i(const std::wstring& st)
    {
      if (st.length() < 4) { return false; }
      if (st[1] == L'b')
      {
        if (st.length() < 5 || st[3] != L'_') { return false; }
        if (st[0] == 'p' && (st[2] == 'o' || st[2] == 'i')) { return true; }
          else if (st[0] == L'q' && st[2] == 'i') { return true; }
      }
      else
      {
        if (st[2] != L'_') { return false; }
        if (st[0] == L'p' && st[1] == 'i') { return true; }
          else if (st[0] == L'q' && (st[1] == 'i' || st[1] == 's')) { return true; }
      }
      return false;
    }
  static inline std::wstring slt_root(const std::wstring& st)
    {
      _t_wz i = st.find(L'_');
      return i == nposw ? std::wstring() : st.substr(i+1);
    }
  static inline bool sltchk_iomatch(const std::wstring& st, const std::wstring& dt)
    {
      if (st.length() < 4 || dt.length() < 4) { return false; }
      if (st[1] == L'b')
      {
        if (st.length() < 5 || dt.length() < 5 || st[3] != L'_' || dt[3] != L'_') { return false; }
        if (st[0] == 'p' && st[2] == 'o' && (dt[0] == 'p' || dt[0] == 'q') && dt[2] == 'i') { return true; }
          else if (st[0] == 'p' && st[2] == 'i' && dt[0] == 'p' && dt[2] == 'o') { return true; }
          else if (st[0] == 'q' && st[2] == 'i' && dt[0] == 'p' && dt[2] == 'o') { return true; }
      }
      else
      {
        if (st[2] != L'_') { return false; }
        if (st[0] == 'p' && st[1] == 'o' && dt[0] == 'p' && dt[1] == 'i') { return true; }
          else if (st[0] == 'p' && st[1] == 'o' && dt[0] == 'q' && (dt[1] == 'i' || dt[1] == 's')) { return true; }
          else if (st[0] == 'q' && st[1] == 's' && (dt[0] == 'p' || dt[0] == 'q') && dt[1] == 'i') { return true; }
      }
      return false;
    }

  static s_long _s_pop(cref_t<dispatcher_mt::cch_session>& _r_ths, const std::wstring& _name_th, const unity& slotname, unity& retmsg, _carray_base_t<char>* retbuf, s_long flags) throw()
  {
    if (!_r_ths.has_ref()) { return -2; }
    if (_r_ths.ref().ses_state != 1) { return -3; }
    try {

        //  Det. exact slot name.
      std::wstring ssln1, ssln_r;
      s_long res = __recode_slotname(slotname, ssln_r, &ssln1); if (res != 1) { return res == -1 ? -1 : -2; }

        // Check if slot exists / get ref.
      cref_t<cch_slot, dispatcher_mt> ri_sl;
      if (true)
      {
        critsec_t<dispatcher_mt> __lock(10, -1, &_r_ths._pnonc_u()->lkm_ths); if (sizeof(__lock)) {}
        const hashx<std::wstring, cch_thread>::entry* e_th;
        const hashx<std::wstring, cref_t<cch_slot, dispatcher_mt> >::entry* e_sl;
        e_th = _r_ths.ref().h_ths.find(_name_th); if (!e_th) { return -2; }
        e_sl = e_th->v.h_sl.find(ssln_r); if (!e_sl) { return -2; }
          ri_sl = e_sl->v; if (!ri_sl.has_ref()) { return -2; }
      } // end threads hash lock
      cch_slot& i_sl = *ri_sl._pnonc_u();

        // Check if message exists, get message, set slot phase as necessary for slot type
      if (ssln1[1] == L'i') // pi, qi
      {
        if (ssln1[0] == L'p')
        {
          critsec_t<dispatcher_mt> __lock(10, -1, &i_sl.lkd); if (sizeof(__lock)) {}
          if (i_sl.phase != 1) { return 0; }

          s_long res = 3;
          if ((flags & 2) == 0) { res = __aux_msg_decode(i_sl.r_pin.ref(), (flags & 1) == 0, (flags & 4) != 0, retmsg, retbuf); }
          if (res <= 0) { return -2; }

          if ((flags & 8) != 0) { } // peek only, do not pop
          else
          {
            i_sl.r_pin.clear();
            i_sl.phase = 2;
          }
          return res;
        }
        else if (ssln1[0] == L'q')
        {
          critsec_t<dispatcher_mt> __lock(10, -1, &i_sl.lkd); if (sizeof(__lock)) {}

          t_qel r_msg = i_sl.r_q._pnonc_u()->pop(-1, false);
            if (!r_msg) { return 0; }

          s_long res;
          if ((flags & 2) != 0) { res = 3; } // discard (do not read) the message
          else
          {
            res = __aux_msg_decode(r_msg.ref(), (flags & 1) == 0, (flags & 4) != 0, retmsg, retbuf);
              if (res <= 0) { return -2; }
          }

          if ((flags & 8) != 0) { } // peek only, do not pop
          else
          {
            i_sl.r_q._pnonc_u()->pop(-1, true);
          }
          return res;
        }
        else { return -2; }
      }
      else if (ssln1[1] == L'b') // pbo, pbi, qbi
      {
      //  1 - input msg arrived.
      //      2 - input msg popped.
      //      3 - command response pushed by the receiver and forwarded to sender.
      //  4 - command sent.
      //      5 - command response received.
      //      6 - command response popped.
      //  pbo: 0, 6 --> 4 --> 5 --> 6
      //  pbi: 0, 3 --> 1 --> 2 --> 3
      //  qbi: 0, 3 --> 2 --> 3
      //  pi: 0, 2 --> 1 --> 2
        if (ssln1[2] == L'o') // pbo (popping response)
        {
          critsec_t<dispatcher_mt> __lock(10, -1, &i_sl.lkd); if (sizeof(__lock)) {}
          if (i_sl.phase == 4) { return -20; }
          if (i_sl.phase != 5) { return 0; }

          s_long res;
          if ((flags & 2) != 0) { res = 3; } // discard (do not read) the message
          else
          {
            res = __aux_msg_decode(i_sl.r_pin.ref(), (flags & 1) == 0, (flags & 4) != 0, retmsg, retbuf);
              if (res <= 0) { return -2; }
          }

          if ((flags & 8) == 0)
          {
            i_sl.r_pin.clear();
            i_sl.phase = 6;
          }
          return res;
        }
        else if (ssln1[0] == 'p' && ssln1[2] == L'i') // pbi (popping command)
        {
          critsec_t<dispatcher_mt> __lock(10, -1, &i_sl.lkd); if (sizeof(__lock)) {}
          if (i_sl.phase == 2) { return -21; }
          if (i_sl.phase != 1) { return 0; }

          s_long res;
          if ((flags & 2) != 0) { res = 3; } // discard (do not read) the message
          else
          {
            res = __aux_msg_decode(i_sl.r_pin.ref(), (flags & 1) == 0, (flags & 4) != 0, retmsg, retbuf);
              if (res <= 0) { return -2; }
          }

          if ((flags & 8) != 0) { } // peek only, do not pop
          else
          {
            i_sl.r_pin.clear();
            i_sl.phase = 2;
          }
          return res;
        }
        else if (ssln1[0] == 'q' && ssln1[2] == L'i') // qbi (popping command from queue)
        {
          critsec_t<dispatcher_mt> __lock(10, -1, &i_sl.lkd); if (sizeof(__lock)) {}
          if (i_sl.phase == 2) { return -21; }
          if (i_sl.r_qbi.ref().is_empty()) { return 0; }

          t_qbiel r_msg = i_sl.r_qbi._pnonc_u()->pop(-1, false);
            if (!r_msg.has_ref()) { return -2; }

          s_long res;
          if ((flags & 2) != 0) { res = 3; } // discard (do not read) the message
          else
          {
            res = __aux_msg_decode(r_msg.ref().msg.ref(), (flags & 1) == 0, (flags & 4) != 0, retmsg, retbuf);
              if (res <= 0) { return -2; }
          }

          if ((flags & 8) != 0) { } // peek only, do not pop
          else
          {
            i_sl.r_qbi._pnonc_u()->pop(-1, true);
            i_sl.r_lsrc = r_msg.ref().src;
            i_sl.phase = 2;
          }
          r_msg.clear();

          return res;
        }
        return -2;
      }
      return 0; // po, qo
    } catch (...) { return -2; }
  }

    // _name_th: name of the thread, calling _s_write (should be same as thread name in source address (src = ...), if it's specified)
    // hm: must be already decoded mesage (hash or map).
    //  Must have "src", "trg", "text" fields.
    //  During processing, "src" and "trg" fields are modified, to reflect the receiving side context.
    // NOTE _s_write is called multiple times for one message from _s_subs_deliver,
    //  each time with different "trg" and re-set "src".
  static s_long _s_write(cref_t<dispatcher_mt::cch_session>& _r_ths, const std::wstring& _name_th, unity& hm, const arrayref_t<char>* buf, bool b_delivery) throw()
  {
    if (!_r_ths.has_ref()) { return -2; }
    dispatcher_mt::cch_session& rses = *_r_ths._pnonc_u();
    if (rses.ses_state != 1) { return -3; }
    try {
      const unity k_src(L"src"), k_trg(L"trg"), k_text(L"text");
      if (!(hm.isAssoc() && hm.u_has(k_src, 4) && hm.u_has(k_trg, 4) && hm.u_has(k_text, 4))) { return -1; }

      s_long res;

        // Source address:
        // a) On b_delivery == false:
        //    <slot name> - thread sending a message from itself.
        //  Before encoding, the address is completed: |LP|<thread name>|<slot name>.
        // b) On b_delivery == true:
        //    |LP|<thread name>|<qs slot name>
        //  -- thread's periodic() / _s_qs_deliver() sends a message to a subscriber.
        //  Thread name must be same as _name_th.
        //  Source qs slot existence is checked, but the slot itself is not accessed, because this is done already by _s_qs_deliver.
        // c) On b_delivery == true:
        //    |LPA|<qs slot name>
        //  -- dispatcher periodic() / _s_qs_deliver() sends a message to a subscriber from a slot belonging to thread _name_th.
        //  Source qs slot existence is checked, but the slot itself is not accessed, because this is done already by _s_qs_deliver.
      address sa; std::wstring ssln_r, ssln1, sslntail;
      if (b_delivery)
      {
        sa.set_addr(hm[k_src]);
          if (sa.is_empty()) { return -1; }
          if (sa.isLP()) { if (sa.wstr_thn() != _name_th) { return -1; } }
            else if (sa.isLPA()) { }
            else { return -1; }
        res = __recode_slotname(sa.sln_v(), ssln_r, &ssln1, &sslntail);
          if (res != 1) { return -1; }
          if (!sltchk_main_o(ssln1)) { return -1; }
      }
      else
      {
        unity a;
        __append_vals(a, L"LP");
        __append_vals(a, _name_th);
        __append_vals(a, hm[k_src]);
        sa.set_addr(a);
          if (sa.is_empty()) { return -1; }
        res = __recode_slotname(sa.sln_v(), ssln_r, &ssln1, &sslntail);
          if (res != 1) { return -1; }
          if (!sltchk_main_o(ssln1)) { return -1; }
        hm[k_src] = sa.addr();
      }

      address da;
      res = da.set_addr(hm[k_trg]); if (res != 1) { return res == -1 ? -1 : -2; }

      if (da.isLP() || da.isLPA())
      {
        std::wstring dthn, dsln_r, dsln1;
          // Det. dest. slot type and exact name.
          // Check source / dest. type match.
        dsln_r = da.wstr_sln();
        dsln1 = da.wstr_sln_1();
          if (!sltchk_main_i(dsln1)) { return -1; }
          if (slt_root(ssln1) != slt_root(dsln1)) { return -1; }
          if (sslntail.length() > 0 && da.wstr_sln_tail() != sslntail) { return -1; }
          if (!sltchk_iomatch(ssln1, dsln1)) { return -1; }

          // Get slot cache refs.
        cref_t<cch_slot, dispatcher_mt> rs_sl, rd_sl;
        if (true)
        {
          const hashx<std::wstring, cch_thread>::entry* e_th;
          const hashx<std::wstring, cref_t<cch_slot, dispatcher_mt> >::entry* e_sl;

          critsec_t<dispatcher_mt> __lock(10, -1, &rses.lkm_ths); if (sizeof(__lock)) {}
          if (da.isLP()) { dthn = da.wstr_thn(); }
          else // LPA
          {
            const hashx<unity, unity>::entry* e_dispqs;
            e_dispqs = rses.h_qs_disp.find(dsln_r); if (!e_dispqs) { return -8; }
            dthn = e_dispqs->v.vstr();
          }

          e_th = rses.h_ths.find(dthn); if (!e_th) { return -8; }
          e_sl = e_th->v.h_sl.find(dsln_r); if (!e_sl) { return -8; }
            rd_sl = e_sl->v; if (!rd_sl.has_ref()) { return -2; }

          e_th = rses.h_ths.find(_name_th); if (!e_th) { return -2; }
          e_sl = e_th->v.h_sl.find(ssln_r); if (!e_sl) { return -2; }
            rs_sl = e_sl->v; if (!rs_sl.has_ref()) { return -2; }
        } // end threads hash lock
        cch_slot& d_sl = *rd_sl._pnonc_u();
        cch_slot& s_sl = *rs_sl._pnonc_u();

          // Check phases, recode message, write dest. slot, update phases.
        if (dsln1[1] == L'i') // pi, qi
        {
          if (dsln1[0] == L'p')
          {
            cref_t<std::string, meta::nothing> msg2; if (!msg2.create0(true)) { return -2; }
            s_long res = __aux_msg_encode(hm, buf, *msg2._pnonc_u()); if (res < 0) { return -2; }

            critsec_t<dispatcher_mt> __lock(10, -1, &d_sl.lkd); if (sizeof(__lock)) {}
            d_sl.r_pin = msg2; msg2.clear();
            d_sl.phase = 1;
            return 1;
          }
          else if (dsln1[1] == L'i')
          {
            cref_t<std::string, meta::nothing> msg2; if (!msg2.create0(true)) { return -2; }
            s_long res = __aux_msg_encode(hm, buf, *msg2._pnonc_u()); if (res < 0) { return -2; }

            critsec_t<dispatcher_mt> __lock(10, -1, &d_sl.lkd); if (sizeof(__lock)) {}
            if (static_cast<aux_fifo<std::string>*>(d_sl.r_q._pnonc_u())->_push_nonblk(msg2) < 0) { return -4; }
            msg2.clear();
            return 1;
          }
          else { return -2; }
        }
        else if (dsln1[1] == L'b') // pbo, pbi, qbi
        {
          cref_t<std::string, meta::nothing> msg2; if (!msg2.create0(true)) { return -2; }
          s_long res = __aux_msg_encode(hm, buf, *msg2._pnonc_u()); if (res < 0) { return -2; }

          cref_t<std::wstring, meta::nothing> cmdsrc;
          if (dsln1[2] == L'o')
          {
            cmdsrc.copy(da.wstr_addr(), true);
              if (!cmdsrc.has_ref()) { return -2; }
          }
          else if (dsln1[2] == L'i')
          {
            cmdsrc.copy(sa.wstr_addr(), true);
              if (!cmdsrc.has_ref()) { return -2; }
          }

          critsec_t<dispatcher_mt> __lock(10, -1, &d_sl.lkd); if (sizeof(__lock)) {}
          critsec_t<dispatcher_mt> __lock2(10, -1, &s_sl.lkd); if (sizeof(__lock2)) {}

          s_long& d_ph = d_sl.phase;
          s_long& s_ph = s_sl.phase;
//  1 - input msg arrived, 2 - input msg popped.
//    3 - command response pushed by the receiver and forwarded to sender.
//  4 - command sent, 5 - command response received.
//    6 - command response popped.
//  pbo: 0, 6 --> 4 --> 5 --> 6
//  pbi: 0, 3 --> 1 --> 2 --> 3
//  qbi: 0, 3 --> 2 --> 3
//  pi: 0, 2 --> 1 --> 2
          if (dsln1[2] == L'o' && (dsln1[0] == 'p' || dsln1[0] == 'q')) // pbo (writing response to sender pin)
          {
            if (!(d_ph == 4 && s_ph == 2)) { return -5; }
            if (cmdsrc.ref() != s_sl.r_lsrc.ref()) { return -9; }
            d_sl.r_pin = msg2; msg2.clear();
            s_sl.r_lsrc.clear();
            d_ph = 5;
            s_ph = 3;
            return 1;
          }
          else if (dsln1[0] == 'p' && dsln1[2] == L'i') // pbi (writing command to receiver pin)
          {
            if (!((d_ph == 0 || d_ph == 3) && (s_ph == 0 || s_ph == 6))) { return -5; }
            d_sl.r_pin = msg2; msg2.clear();
            d_sl.r_lsrc = cmdsrc; cmdsrc.clear();
            d_ph = 1;
            s_ph = 4;
            return 1;
          }
          else if (dsln1[0] == 'q' && dsln1[2] == L'i') // qbi (writing command to receiver queue)
          {
            if (!(s_ph == 0 || s_ph == 6)) { return -5; }
            cref_t<cch_slot::qbi_value, meta::nothing> rz;
              if (!rz.create0(true)) { return -2; }
              rz._pnonc_u()->msg = msg2; msg2.clear();
              rz._pnonc_u()->src = cmdsrc; cmdsrc.clear();
            if (static_cast<aux_fifo<cch_slot::qbi_value>*>(d_sl.r_qbi._pnonc_u())->_push_nonblk(rz) < 0) { return -4; }
            rz.clear();
            s_ph = 4;
            return 1;
          }
        }
        else if (dsln1[1] == L's') // qs
        {
          if (dsln1[0] == L'q')
          {
            if (da.isLPA()) { if (!d_sl.qs_input_lpa()) { return -10; } }
              else if (!d_sl.qs_input_any_th()) { if (_name_th != dthn) { return -10; } }

              // Creating subscription message.
            cref_t<cch_slot::qs_value, meta::nothing> r_x; if (!r_x.create0(true)) { return -2; }
            cch_slot::qs_value& x = *r_x._pnonc_u();
            try { x.hmsg = hm; } catch (...) { return -2; }
            if (buf)
            {
              if (!x.bin.create0(true)) { return -2; }
              if (!x.bin._pnonc_u()->resize(buf->n())) { return -2; }
              bytes::memmove_t<char>::F(x.bin._pnonc_u()->pd(), buf->pd(), buf->n());
            }

              // Writing to qs slot.
            if (true)
            {
              critsec_t<dispatcher_mt> __lock(10, -1, &d_sl.lkd); if (sizeof(__lock)) {}
              if (static_cast<aux_fifo<cch_slot::qs_value>*>(d_sl.r_qs._pnonc_u())->_push_nonblk(r_x) < 0) { return -4; }
            }

              // Immediate delivery if necessary.
            if (d_sl.qs_deliv_imm()) { _s_subs_deliver(_r_ths, rd_sl, false, dthn, dsln_r); }
            return 1;
          }
        }
        return -2;
      }
      else
      {
        // ~!!! later: LM, R, LMA, RMA
        return -6;
      }
    } catch (...) { return -2; }
  }

    // is_disp true: delivers messages
    //  from disp thread's own qs slots (ths.h_ths[_name_th].h_qs_thread, source address is |LP|<_name_th>|<qs_...>)
    //  + from different threads' global qs slots, listed in ths.h_qs_disp (source address is |LPA|<qs_...>).
    //  _name_th - disp thread name.
    //
    // is_disp false: delivers messages
    //  from the calling thread's own qs slots
    //  (ths.h_ths[_name_th].h_qs_thread, source address is |LP|<_name_th>|<qs_...>).
    //  _name_th - the calling (sending) thread name.
    //
    // Returns:
    //  >=0 - number of subscription queues processed.
    //  -1 - failure, no messages sent.
    //  -2 - failure, some part of messages has been sent.
    //  -3 - session is closed, no messages sent.
    //  -4 - invalid flags.
  static s_long _s_qs_deliver(cref_t<dispatcher_mt::cch_session>& _r_ths, const std::wstring& _name_th, s_long flags) throw()
  {
    if (!_r_ths.has_ref()) { return -2; }
    if (_r_ths.ref().ses_state != 1) { return -3; }
    if (flags != 0) { return -5; }
    s_long res = 0;
    cch_session& ths = *_r_ths._pnonc_u();
    try {
      bool b_disp = false;
      vec2_t<cref_t<cch_slot, dispatcher_mt> > sls_disp;
      vec2_t<std::wstring> slsn_disp;
      vec2_t<cref_t<cch_slot, dispatcher_mt> > sls_th;
      vec2_t<std::wstring> slsn_th;
      if (true)
      {
        critsec_t<dispatcher_mt> __lock(10, -1, &ths.lkm_ths); if (sizeof(__lock)) {}

        const hashx<std::wstring, cch_thread>::entry* e_th = ths.h_ths.find(_name_th); if (!e_th) { return -2; }
        cch_thread& th_my = e_th->v;

        if (th_my.b_disp)
        {
          b_disp = true;
          hashx<unity, unity>& h1 = ths.h_qs_disp;
          for (s_long i = 0; i < h1.n(); ++i)
          {
            const hashx<std::wstring, cch_thread>::entry* e_th = ths.h_ths.find(h1(i)->v.vstr()); if (!e_th) { continue; }
            const hashx<std::wstring, cref_t<cch_slot, dispatcher_mt> >::entry* e_sl = e_th->v.h_sl.find(h1(i)->k.vstr()); if (!(e_sl && e_sl->v.has_ref())) { continue; }
            sls_disp.push_back(e_sl->v);
            slsn_disp.push_back(h1(i)->k.vstr());
          }
        }

        if (true)
        {
          hashx<unity, int>& h1 = th_my.h_qs_thread;
          for (s_long i = 0; i < h1.n(); ++i)
          {
            const hashx<std::wstring, cref_t<cch_slot, dispatcher_mt> >::entry* e_sl = th_my.h_sl.find(h1(i)->k.vstr()); if (!(e_sl && e_sl->v.has_ref())) { continue; }
            sls_th.push_back(e_sl->v);
            slsn_th.push_back(h1(i)->k.vstr());
          }
        }
      } // end lock

      for (s_long i = 0; i < sls_th.n(); ++i)
      {
        s_long res2 = _s_subs_deliver(_r_ths, sls_th[i], false, _name_th, slsn_th[i]);
        if (res2 < res) { res = res2; if (res == -3) { return res; } }
        if (res >= 0 && res2 >= 0) { ++res; }
      }
      if (b_disp)
      {
        for (s_long i = 0; i < sls_disp.n(); ++i)
        {
          s_long res2 = _s_subs_deliver(_r_ths, sls_disp[i], true, _name_th, slsn_disp[i]);
          if (res2 < res) { res = res2; if (res == -3) { return res; } }
          if (res >= 0 && res2 >= 0) { ++res; }
        }
      }
      return res;
    } catch (...) { return -2; }
  }

    // is_disp false, _name_th and _name_qs: r_qs owner thread and name.
    // is_disp true, _name_th: disp thread, _name_qs: r_qs name.
    // Returns:
    //  >=0 - number of messages processed (sent + failed).
    //  -1 - failure, no messages sent.
    //  -2 - failure, some part of messages may have been sent.
    //  -3 - session is closed, no messages sent.
  static s_long _s_subs_deliver(cref_t<dispatcher_mt::cch_session>& _r_ths, cref_t<cch_slot, dispatcher_mt>& r_qs, bool is_disp, const std::wstring& _name_th, const std::wstring& _name_qs) throw()
  {
    if (!_r_ths.has_ref()) { return -2; }
    if (_r_ths.ref().ses_state != 1) { return -3; }
    if (!r_qs.has_ref()) { return -1; }

    s_long res0 = -1;
    try {
      const unity k_src(L"src"), k_trg(L"trg"), k_src0(L"src0"), k_trg0(L"trg0");
      address sa;
        if (true)
        {
          if (!sltchk_main_qs(_name_qs)) { return -1; }
          unity a;
          __append_vals(a, is_disp ? L"LPA" : L"LP");
          if (!is_disp) { __append_vals(a, _name_th); }
          __append_vals(a, paramline().decode1v(_name_qs));
          sa.set_addr(a); if (!sltchk_qs(sa.slt())) { return -1; }
        }
      cch_slot& qs_sl = *r_qs._pnonc_u();

      typedef aux_fifo<cch_slot::qs_value>::t_elem t_elem;
      vec2_t<t_elem> msgs;
      hashx<unity, s_long> hsubs;
      try {
        critsec_t<dispatcher_mt> __lock(10, -1, &qs_sl.lkd); if (sizeof(__lock)) {}
        if (!(qs_sl.r_qs.has_ref() && qs_sl.r_hsubs.has_ref())) { return -2; }
        aux_fifo<cch_slot::qs_value>* pqs = static_cast<aux_fifo<cch_slot::qs_value>*>(qs_sl.r_qs._pnonc_u());
        while (true) { t_elem x = pqs->_pop_nonblk(true); if (!x) { break; } msgs.push_back(x); }
        if (hsubs.hashx_copy(qs_sl.r_hsubs.ref(), false) < 1) { return -2; }
      } catch (...) {}

      res0 = -2; s_long n = 0;
      for (s_long imsg = 0; imsg < msgs.n(); ++imsg)
      {
        unity& hmsg = msgs[imsg]._pnonc_u()->hmsg; if (!hmsg.isAssoc()) { continue; }
        const arrayref_t<char> bin = msgs[imsg].ref().bin.ref();
        for (s_long isubs = 0; isubs < hsubs.n(); ++isubs)
        {
          try
          {
            address da;
            da.set_addr(hsubs(isubs)->k); if (da.is_empty()) { continue; }
            hmsg.assoc_set(k_src0, hmsg[k_src], true);
            hmsg.assoc_set(k_trg0, hmsg[k_trg], true);
            hmsg.assoc_set(k_src, sa.addr(), false);
            hmsg.assoc_set(k_trg, da.addr(), false);
          } catch (...) { continue; }
          _s_write(_r_ths, _name_th, hmsg, &bin, true);
        }
        ++n;
      }

      return n;
    } catch (...) { return res0; }
  }

  static s_long _s_subscribe(cref_t<dispatcher_mt::cch_session>& _r_ths, const unity& add_qs0, const unity& recv_sl0, s_long rt, const std::wstring& _name_th_recv)
  {
    if (!_r_ths.has_ref()) { return -2; }
    if (_r_ths.ref().ses_state != 1) { return -3; }
    if (!(rt >= 1 && rt <= 5)) { return -1; }
    cch_session& ths = *_r_ths._pnonc_u();
    try {
      address sa, da;
        sa.set_addr(unity(add_qs0)); if (!sltchk_qs(sa.slt())) { return -1; }
        if (sa.isLP_any()) // local subscription
        {
          if (rt == 4 || rt == 5) { return 0; }
          if (da.set_addr_LP(_name_th_recv, unity(recv_sl0)) != 1) { return -2; }
          if (!sltchk_subscriber(da.slt())) { return -1; }

          const hashx<std::wstring, cch_thread>::entry* _e_th = 0; const hashx<std::wstring, cref_t<cch_slot, dispatcher_mt> >::entry* _e_sl = 0;

          cref_t<cch_slot, dispatcher_mt> rqs_sl;
          if (sa.isLPA())
          {
            critsec_t<dispatcher_mt> __lock(10, -1, &ths.lkm_ths); if (sizeof(__lock)) {}
            const hashx<unity, unity>::entry* e1 = ths.h_qs_disp.find(sa.wstr_sln()); if (!e1) { return -10; }

            _e_th = ths.h_ths.find(e1->v.vstr()); if (!_e_th) { return -2; }
            _e_sl = _e_th->v.h_sl.find(e1->k.vstr()); if (!(_e_sl && _e_sl->v.has_ref())) { return -2; }
            rqs_sl = _e_sl->v;
          }
          else
          {
            critsec_t<dispatcher_mt> __lock(10, -1, &ths.lkm_ths); if (sizeof(__lock)) {}
            _e_th = ths.h_ths.find(sa.wstr_thn()); if (!_e_th) { return -2; }
            _e_sl = _e_th->v.h_sl.find(sa.wstr_sln()); if (!(_e_sl && _e_sl->v.has_ref())) { return -2; }
            rqs_sl = _e_sl->v;
          }

          cref_t<cch_slot, dispatcher_mt> rsub_sl;
          if (true)
          {
            critsec_t<dispatcher_mt> __lock(10, -1, &ths.lkm_ths); if (sizeof(__lock)) {}
            _e_th = ths.h_ths.find(_name_th_recv); if (!_e_th) { return -2; }
            _e_sl = _e_th->v.h_sl.find(da.wstr_sln()); if (!(_e_sl && _e_sl->v.has_ref())) { return -5; }
            rsub_sl = _e_sl->v;
          }

          cch_slot& qs_sl = *rqs_sl._pnonc_u();
          cch_slot& sub_sl = *rsub_sl._pnonc_u();
          hashx<unity, s_long> *ph1(0), *ph2(0);
          bool b_sub1(false), b_sub2(false);
          if (true) { critsec_t<dispatcher_mt> __lock(10, -1, &qs_sl.lkd); if (sizeof(__lock)) {} ph1 = qs_sl.r_hsubs._pnonc_u(); if (!ph1) { return -2; } b_sub1 = !! ph1->find(da.wstr_addr()); }
          if (true) { critsec_t<dispatcher_mt> __lock(10, -1, &sub_sl.lkd); if (sizeof(__lock)) {} ph2 = sub_sl.r_hsubs._pnonc_u(); if (!ph2) { return -2; } b_sub2 = !! ph2->find(sa.wstr_addr()); }

          if (rt == 1) // sub
          {
            if (b_sub1 && b_sub2) { return 2; }
            if (qs_sl.qs_output_fixed()) { return 12; }
            if (!b_sub1) { critsec_t<dispatcher_mt> __lock(10, -1, &qs_sl.lkd); if (sizeof(__lock)) {} if (ph1->insert(da.wstr_addr()) < 0) { return -2; } }
            if (!b_sub2) { critsec_t<dispatcher_mt> __lock(10, -1, &sub_sl.lkd); if (sizeof(__lock)) {} if (ph2->insert(sa.wstr_addr()) < 0) { return -2; } }
            return 2;
          }
          else if (rt == 2) // uns
          {
            if (!b_sub1 || !b_sub2) { return 1; }
            if (qs_sl.qs_output_fixed()) { return 12; }
            if (b_sub1) { critsec_t<dispatcher_mt> __lock(10, -1, &qs_sl.lkd); if (sizeof(__lock)) {} if (ph1->remove(da.wstr_addr()) >= 0) { b_sub1 = false; } }
            if (b_sub2) { critsec_t<dispatcher_mt> __lock(10, -1, &sub_sl.lkd); if (sizeof(__lock)) {} if (ph2->remove(sa.wstr_addr()) >= 0) { b_sub2 = false; } }
            if (b_sub1 && b_sub2) { return -2; }
            return 1;
          }
          else // rt == 3, chk sub
            { return b_sub1 && b_sub2 ? 2 : 1; }
        }
        else
        {
          //~!!!
          // remote request:
          //  recode and verify qs and recv, also verify their matching
          //  the current op. is pending:
          //    match rt
          //    check for timeout
          //    return the result as for now
          //    remove the current op. state if it's completed
          //  no current op.:
          //    create op. state
          //    add op. state into states hash, check memory limit
          //    send ipc or remote request
          // ----------------------------
          // ipc or network thread:
          //  receive remote response
          //  update the current op. state
          return -2;
        }
    } catch (...) { return -2; }
  }

    // empty _name_th means the call directly from dispatcher_mt (not associated with any thread).
  static s_long _s_request(cref_t<dispatcher_mt::cch_session>& _r_ths, s_long rt, unity& retval, const unity& args, const std::wstring& _name_th) throw()
  {
    if (!_r_ths.has_ref()) { return -2; }
    if (_r_ths.ref().ses_state != 1) { return -3; }
    cch_session& ths = *_r_ths._pnonc_u();
    switch (rt)
    {
    case 1: // get list of local threads
      try {
        retval.u_clear(utHash);
        hashx<std::wstring, int> h;
        if (true)
        {
          critsec_t<dispatcher_mt> __lock(10, -1, &ths.lkm_ths); if (sizeof(__lock)) {}
          hashx<std::wstring, cch_thread>& ht = ths.h_ths;
          for (s_long i = 0; i < ht.n(); ++i) { if (h.insert(ht(i)->k) < 0) { return -2; } }
        }
        for (s_long i = 0; i < h.n(); ++i) { retval.hash(h(i)->k); }
        return 1;
      } catch (...) { return -2; }
    case 2: // get all slots of local thread
      try {
        retval.u_clear(utHash);
        hashx<std::wstring, int> h;
        if (true)
        {
          critsec_t<dispatcher_mt> __lock(10, -1, &ths.lkm_ths); if (sizeof(__lock)) {}
          const hashx<std::wstring, cch_thread>::entry* e_th = ths.h_ths.find(args.vstr()); if (!e_th) { return -4; }
          hashx<std::wstring, cref_t<cch_slot, dispatcher_mt> >& hsl = e_th->v.h_sl;
          for (s_long i = 0; i < hsl.n(); ++i) { if (h.insert(hsl(i)->k) < 0) { return -2; } }
        }
        for (s_long i = 0; i < h.n(); ++i) { retval.hash(h(i)->k); }
        return 1;
      } catch (...) { return -2; }
    case 3: // set priority and timing for internal subscription delivery
      try {
        if (_r_ths.ref().qsdt < 0) { return -4; }
        unity x = args; x.arrlb_(1); if (!(x.arrsz() == 2)) { return -1; }
        s_ll pr = x.ref<utInt>(1); s_ll dt = x.ref<utInt>(2); if (!(pr >= 1 && pr <= 7 && dt >= 0)) { return -1; }
        if (!_r_ths._pnonc_u()->tc_qsd.set_priority(s_long(pr))) { return -2; }
        _r_ths._pnonc_u()->qsd_prio = s_long(pr);
        _r_ths._pnonc_u()->qsdt = dt;
        return 1;
      } catch (...) { return -2; }
    case 4: // get priority and timing of internal subscription delivery
      try {
        if (_r_ths.ref().qsdt < 0) { return -4; }
        unity x; x.arr_init<utString>(1);
        x.arr_append(_r_ths.ref().qsd_prio);
        x.arr_append(_r_ths.ref().qsdt);
        retval = x;
        return 1;
      } catch (...) { return -2; }
    default: { return -1; }
    }
  }

  virtual s_long pop(const unity& slotname, unity& retmsg, _carray_base_t<char>* retbuf, s_long flags) throw()
    { return _s_pop(_r_ths, _name_th, slotname, retmsg, retbuf, flags); }
  virtual s_long write(const unity& msg, const arrayref_t<char>* buf) throw()
  {
    if (!_r_ths.has_ref()) { return -2; }
    if (_r_ths.ref().ses_state != 1) { return -3; }
    unity hm; try { paramline().decode(msg.cref<utString>().ref(), hm, false); } catch (...) { return -2; }
    return _s_write(_r_ths, _name_th, hm, buf, false);
  }

  virtual s_long request(s_long rt, unity& retval, const unity& args = unity()) throw()
    { return _s_request(_r_ths, rt, retval, args, _name_th); }

  virtual s_long slots_create(const unity& slotscfg) throw()
  {
    if (!_r_ths.has_ref()) { return -2; }
    if (_r_ths.ref().ses_state != 1) { return -3; }
    if (!(slotscfg.isString() || (slotscfg.isAssoc() && slotscfg.compatibility() > 0))) { return -1; }
    s_long res = -2;
    cch_session& ths = *_r_ths._pnonc_u(); hashx<std::wstring, hashx<std::wstring, s_long> > hsubs;
    if (true)
    {
      critsec_t<dispatcher_mt> __lock(10, -1, &ths.lkm_ths); if (sizeof(__lock)) {}
      try { res = _s_add_slots_nl(ths, _name_th, slotscfg.isString() ? paramline().decode_tree(slotscfg.vstr(), 8) : slotscfg, hsubs); } catch (...) {}
    } // end threads hash lock
    if (res == 1) { _s_update_subs_inputs(ths, hsubs); }
    return res;
  }

  virtual s_long slot_remove(const unity& slotname0) throw()
  {
    if (!_r_ths.has_ref()) { return -2; }
    if (_r_ths.ref().ses_state != 1) { return -3; }
    unity sln = slotname0;
    if (!(sln.isString() || sln.isArray())) { return -1; }
    s_long res = -2;
    cch_session& ths = *_r_ths._pnonc_u();
    try {
        //  Det. exact slot name.
      unity _a;
      if (sln.isString())
      {
        paramline().decode1v(sln.vstr(), _a);
        if (!sltchk_main((_a.isArray() ? _a[1] : _a).vstr())) { return -1; }
      }
      else { if (!sltchk_main(sln.vstr(sln.arrlb()))) { return -1; } }
      std::wstring k_sl; s_long res = __recode_slotname(sln, k_sl); if (res != 1) { return res == -1 ? -1 : -2; }
      const unity ku_sl = k_sl;

      if (true)
      {
        critsec_t<dispatcher_mt> __lock(10, -1, &ths.lkm_ths); if (sizeof(__lock)) {}
        const hashx<std::wstring, cch_thread>::entry* e_th = ths.h_ths.find(_name_th); if (!e_th) { return -2; }
        const hashx<std::wstring, cref_t<cch_slot, dispatcher_mt> >::entry* e_sl = e_th->v.h_sl.find(k_sl); if (!(e_sl && e_sl->v.has_ref())) { return -2; }
        const s_long qsd = e_sl->v.ref().cqsflags & 0x3;
        res = e_th->v.h_sl.remove(k_sl); if (res < 0) { res = -2; }
          // The following removals do not fail (both key and value types are bytewise movable).
        switch (qsd)
        {
          case 1: e_th->v.h_qs_imm.remove(ku_sl); break;
          case 2: e_th->v.h_qs_thread.remove(ku_sl); break;
          case 3: ths.h_qs_disp.remove(ku_sl); break;
          default: break;
        }
      }
    }
    catch (...) { return -2; }
    return res;
  }

  virtual s_long thread_name(unity& retname) throw()
    { if (!_r_ths.has_ref()) { return -2; } try { retname = _name_th; return 1; } catch (...) { return -2; } }
  virtual s_long process_name(unity& retname) throw()
    { if (!_r_ths.has_ref()) { return -2; } try { retname = _name_pr; return 1; } catch (...) { return -2; } }
  virtual s_long subscribe(const unity& addr_qs, const unity& recv_sl, s_long rt) throw()
    { return _s_subscribe(_r_ths, addr_qs, recv_sl, rt, _name_th); }
  virtual s_long periodic(s_long flags) throw()
  {
    s_long res = _s_qs_deliver(_r_ths, _name_th, flags);
    if (res > 0) { return 1; } if (res == 0 || res == -3) { return res; } if (res == -4) { return -1; } return -2;
  }
};

struct _aux_qsh
{
  hashx<unity, unity> hd;
  hashx<unity, int> ht, hi;
};

  // Adds slots to existing thread.
  //    No locking is done for any structures.
  //    The client must set a lock with ths.lkm_ths everywhere except dispatcher_mt constructor.
  // Returns:
  //  1 - on_calm_result.
  //    hsubs_acc - the list, accumulating inputs for pins, subscribed to newly created qs slots. Format:
  //      { <ADDR slot-subscriber (local or remote)>, { <ADDR qs slot-supplier>, 0 } }
  //    NOTE on error, hsubs_acc may be left modified.
  //  -1 - bad arg. (k_th empty, slotscfg invalid).
  //  -2 - failure (mem. alloc.).
  //  -3 - thread does not exist.
  //  -4 - one of new slot names is not unique.
s_long dispatcher_mt::thread_proxy::_s_add_slots_nl(cch_session& ths, const std::wstring& k_th, const unity& slotscfg, hashx<std::wstring, hashx<std::wstring, s_long> >& hsubs_acc) throw()
{
  try {
    unity k_slots(L"slots");
    if (!(k_th.length() > 0 && slotscfg.compatibility() > 0 && slotscfg.isAssoc() && slotscfg.u_has(k_slots, 6) && slotscfg[k_slots].isArray())) { return -1; }
    const hashx<std::wstring, cch_thread>::entry* e_th = ths.h_ths.find(k_th); if (!e_th) { return -3; }

    const unity& sls = slotscfg[k_slots];
    cref_t<_aux_qsh> qsh;
    hashx<std::wstring, cref_t<cch_slot, dispatcher_mt> > h_sl2;
    hashx<unity, unity>& h_d = ths.h_qs_disp; hashx<unity, int>& h_t = e_th->v.h_qs_thread; hashx<unity, int>& h_i = e_th->v.h_qs_imm;
    for (s_long i = sls.arrlb(); i <= sls.arrub(); ++i)
    {
      std::wstring k_sl, sln1; s_long res = __recode_slotname(sls.utype() == utUnityArray ? sls.ref<utUnity>(i) : sls.vstr(i), k_sl, &sln1); if (res != 1) { return res == -1 ? -1 : -2; }
      const unity ku_sl = k_sl;

        // Create uninitialized slot.
      cref_t<cch_slot, dispatcher_mt>& rr_sl = h_sl2[k_sl];
        if (rr_sl.has_ref()) { return -4; }
        if (!rr_sl.create0(true)) { return -2; }
        cch_slot& new_sl = *rr_sl._pnonc_u();

        // Initialize slot data according with slot type.
      const unity* psc = slotscfg.u_has(k_sl, 6) ? &slotscfg[k_sl] : 0; if (!psc->isAssoc()) { psc = 0; }
      if (sln1[0] == L'q') // qi, qbi, qs
      {
        unity k_size(L"size");
        s_long n = -1;
           if (psc && psc->u_has(k_size, 6) && (*psc)[k_size].isInt()) { n = s_long((*psc)[k_size].vint()); if (n < 3) { return -1; } }

        if (sln1[1] == L's')
        {
          unity k_delivery(L"delivery");
          s_long qsd = 1;
          if (psc && psc->u_has(k_delivery, 6) && (*psc)[k_delivery].isString())
          {
            std::wstring sd = (*psc)[k_delivery].vstr();
            if (sd == L"immediate") { }
              else if (sd == L"thread") { qsd = 2; }
              else if (sd == L"disp") { qsd = 3; }
              else { return -1; }
          }

          if (!new_sl.r_hsubs.has_ref()) { if (!new_sl.r_hsubs.create0(true)) { return -2; } }

          unity k_input_all(L"input all"), k_input_disp(L"input disp");
          s_long qsinp = 0x8;
          if (psc && psc->u_has(k_input_all, 6)) { const unity& x = (*psc)[k_input_all]; if (!x.isBool()) { return -1; } if (x.isBoolTrue()) { qsinp |=  0x4; } }
          if (psc && psc->u_has(k_input_disp, 6)) { const unity& x = (*psc)[k_input_disp]; if (!x.isBool()) { return -1; } if (x.isBoolFalse()) { qsinp &=  ~s_long(0x8); } }


          unity k_output(L"output"), k_output_fixed(L"output fixed");
          s_long qsoutp = 0x0;
          if (psc && psc->u_has(k_output, 6))
          {
            const unity& x = (*psc)[k_output]; if (!x.isArray()) { return -1; }
            std::wstring s_sa;
            if (true)
            {
              unity a_sa; a_sa.arr_init<utUnityArray>(1);
                a_sa.arr_append<unity>(qsd == 3 ? L"LPA" : L"LP");
                if (qsd != 3) { a_sa.arr_append<unity>(k_th); }
                if (true) { unity a; paramline().decode1v(k_sl, a); if (a.isArray()) { for (s_long i = a.arrlb(); i <= a.arrub(); ++i) { a_sa.arr_append<unity>(a.ref<utUnity>(i)); } } else { a_sa.arr_append<unity>(a); } }
              s_sa = paramline().encode1v(a_sa);
            }
            for (s_long i = x.arrlb(); i <= x.arrub(); ++i)
            {
              address da; da.set_addr(x.utype() == utUnityArray ? x.ref<utUnity>(i) : x.vstr(i)); if (da.is_empty()) { return -1; }
              std::wstring s_da = da.wstr_addr();
              if (new_sl.r_hsubs._pnonc_u()->insert(s_da) != 1) { return -1; }
              hsubs_acc[s_da][s_sa] = 0;
            }
          }
          if (psc && psc->u_has(k_output_fixed, 6)) { const unity& x = (*psc)[k_output_fixed]; if (!x.isBool()) { return -1; } if (x.isBoolTrue()) { qsoutp |=  0x10; } }

          new_sl.cqsflags = qsd | qsinp | qsoutp;

          if (!qsh.has_ref()) { qsh.create0(false); }
          _aux_qsh& q = *qsh._pnonc_u();
          if (qsd == 3) { if (h_d.find(ku_sl) || q.hd.find(ku_sl)) { return -4; } q.hd[ku_sl] = k_th; }
            else if (qsd == 2) { if (h_t.find(ku_sl) || q.ht.find(ku_sl)) { return -4; } q.ht[ku_sl] = 0; }
            else { if (h_i.find(ku_sl) || q.hi.find(ku_sl)) { return -4; } q.hi[ku_sl] = 0; }
        }

        if (sln1[1] == L'i')
        {
          new_sl.r_q.create1(false, n >= 0 ? n : (sln1 == L"qi_char" ? 200 : 10));
          if (!new_sl.r_q.ref().is_valid()) { return -2; }
        }
        else if (sln1[1] == L's')
        {
          new_sl.r_qs.create1(false, n >= 0 ? n : 10);
          if (!new_sl.r_qs.ref().is_valid()) { return -2; }
        }
        else if (sln1[1] == L'b')
        {
          new_sl.r_qbi.create1(false, n >= 0 ? n : 10);
          if (!new_sl.r_qbi.ref().is_valid()) { return -2; }
        }
      }
      else // pi, po, pbi, pbo
      {
        // Initially, pins contain no messages (strings).
      }
    }

    hashx<std::wstring, cref_t<cch_slot, dispatcher_mt> >& h_sl = e_th->v.h_sl;
    for (s_long i = 0; i < h_sl2.n(); ++i) { if (h_sl.find(h_sl2(i)->k)) { return -4; } }

    const s_long ns = h_sl.n();
    const s_long nd = h_d.n();
    const s_long nt = h_t.n();
    const s_long ni = h_i.n();

    bool b(false);
    for (s_long i = 0; i < h_sl2.n(); ++i) { if (h_sl.insert(h_sl2(i)->k) != 1) { goto lSkip1; } h_sl(h_sl.n() - 1)->v = h_sl2(i)->v; }
    if (qsh.has_ref())
    {
      _aux_qsh& q = *qsh._pnonc_u();
      for (s_long i = 0; i < q.hd.n(); ++i) { if (h_d.insert(q.hd(i)->k) != 1) { goto lSkip1; } h_d(h_d.n() - 1)->v = q.hd(i)->v; }
      for (s_long i = 0; i < q.ht.n(); ++i) { if (h_t.insert(q.ht(i)->k) != 1) { goto lSkip1; } h_t(h_t.n() - 1)->v = q.ht(i)->v; }
      for (s_long i = 0; i < q.hi.n(); ++i) { if (h_i.insert(q.hi(i)->k) != 1) { goto lSkip1; } h_i(h_i.n() - 1)->v = q.hi(i)->v; }
    }
    b = true;
lSkip1:;

    if (!b)
    {
      while (h_sl.n() > ns) { h_sl.remove_i(h_sl.n() - 1); }
      while (h_d.n() > nd) { h_d.remove_i(h_d.n() - 1); }
      while (h_t.n() > nt) { h_t.remove_i(h_t.n() - 1); }
      while (h_i.n() > ni) { h_i.remove_i(h_i.n() - 1); }
      return -2;
    }

    h_sl2.hashx_clear();

    return 1;
  } catch (...) {}
  return -2;
}

  // Update input list for each of the given subscribers.
  //  This function processes subscriber slots in a loop, all necessary locking is done in each iteration.
  //  subs: see add_slots_nl.
  //  2 - on_calm_result.
  //  1 - some part of subscriptions was not updated due to non-local or invalid address.
  //  0 - some part of subscriptions was not updated due to failure during update.
s_long dispatcher_mt::thread_proxy::_s_update_subs_inputs(cch_session& ths, const hashx<std::wstring, hashx<std::wstring, s_long> >& hsubs) throw()
{
  s_long n1 = 0, n2 = 0;
  for (s_long i1 = 0; i1 < hsubs.n(); ++i1)
  {
    address da; da.set_addr(hsubs(i1)->k);
      if (!(da.isLP() && sltchk_subscriber(da.slt()))) { ++n1; continue; }

      // Get subscriber slot cache ref.
    cref_t<cch_slot, dispatcher_mt> rsub_sl;
    if (true)
    {
      critsec_t<dispatcher_mt> __lock(10, -1, &ths.lkm_ths); if (sizeof(__lock)) {}
      const hashx<std::wstring, cch_thread>::entry* e_th;
      const hashx<std::wstring, cref_t<cch_slot, dispatcher_mt> >::entry* e_sl;
      try {
        e_th = ths.h_ths.find(da.wstr_thn()); if (!e_th) { ++n1; continue; }
        e_sl = e_th->v.h_sl.find(da.wstr_sln()); if (!e_sl) { ++n1; continue; }
          rsub_sl = e_sl->v; if (!rsub_sl.has_ref()) { continue; }
      } catch (...) { continue; }
    } // end threads hash lock
    cch_slot& sub_sl = *rsub_sl._pnonc_u();

    if (true)
    {
      critsec_t<dispatcher_mt> __lock(10, -1, &sub_sl.lkd); if (sizeof(__lock)) {}
      const hashx<std::wstring, s_long>& h2 = hsubs(i1)->v;
      try {
        for (s_long i2 = 0; i2 < h2.n(); ++i2)
        {
          address a_qs; a_qs.set_addr(h2(i2)->v); if (a_qs.is_empty()) { continue; }
          if (!sub_sl.r_hsubs.has_ref()) { sub_sl.r_hsubs.create0(false); }
          sub_sl.r_hsubs._pnonc_u()->insert(h2(i2)->k);
        }
      } catch (...) { continue; }
    }
    ++n1; ++n2;
  }
  return n2 == hsubs.n() ? 2 : (n1 == hsubs.n() ? 1 : 0);
}

dispatcher_mt::dispatcher_mt(const std::wstring& process_name, const unity& cfg) throw()
{
  if (!cfg.isAssoc()) { return; }
  if (!_r_ths.create0(true)) { return; }
    _r_ths._pnonc_u()->nprx += 1;

  cch_session& ths = *_r_ths._pnonc_u();

  bool b = true;
  try { while (true) { // once
    const unity k_disp(L"disp");
    const unity kg_qsdt(L"__qsdt");
    const unity kg_exitmode(L"__exitmode");

    if (!thread_proxy::pnchk(process_name)) { b = false; break; }
    ths.name_pr = process_name;

    if (true) { int e = +cfg / kg_exitmode / 1; if (!(e >= 0 && e <=2)) { b = false; break; } ths.exitmode = e; }
    if (true) { s_ll dt = +cfg / kg_qsdt / -1; if (!(dt >= -1)) { b = false; break; } ths.qsdt = dt; }

    hashx<std::wstring, hashx<std::wstring, s_long> > hsubs;
    for (s_long i = cfg.assocl_first(); i != cfg.assocl_noel(); i = cfg.assocl_next(i))
    {
      std::wstring k_th = cfg.assocl_key(i).vstr();
      if (!thread_proxy::thnchk(k_th)) { continue; }

      const hashx<std::wstring, cch_thread>::entry* e_th(0);
        if (ths.h_ths.insert(k_th, &e_th) != 1) { b = false; break; }

      const unity& th_cfg = cfg.assocl_c(i);
      if (th_cfg.u_has(k_disp, 6))
      {
        if (!(th_cfg[k_disp].isBool() && ths.name_th_disp.length() == 0)) { b = false; break; }
        ths.name_th_disp = k_th;
        e_th->v.b_disp = true;
      }

      if (thread_proxy::_s_add_slots_nl(ths, k_th, th_cfg, hsubs) != 1) { b = false; break; }
    }
    if (!b) { break; }

    thread_proxy::_s_update_subs_inputs(ths, hsubs);

    if (ths.qsdt >= 0) { if (!ths.tc_qsd.start_auto<thread_proxy::tc_autodelivery, cref_t<cch_session> >(_r_ths)) { b = false; break; } }

    break;
  } } catch (...) { b = false; }
  if (!b)
  {
    critsec_t<dispatcher_mt> __lock(10, -1, &_r_ths._pnonc_u()->lkm_nprx); if (sizeof(__lock)) {}
    _r_ths._pnonc_u()->nprx -= 1;
  }
  if (!b) { _r_ths.clear(); return; }

  _r_ths._pnonc_u()->ses_state = 1;
}
dispatcher_mt::~dispatcher_mt() throw()
{
  if (_r_ths)
  {
    int e = _r_ths._pnonc_u()->exitmode;
    critsec_t<dispatcher_mt> __lock(10, -1, &_r_ths._pnonc_u()->lkm_nprx); if (sizeof(__lock)) {}
    _r_ths._pnonc_u()->nprx -= 1;
    if (e > 0 || _r_ths._pnonc_u()->nprx <= 0) { _r_ths._pnonc_u()->ses_state = 0; }
  }
  if (_r_ths)
  {
    int e = _r_ths._pnonc_u()->exitmode;
    if (e == 2) { while (_r_ths._pnonc_u()->nprx > 0) { sleep_mcs(10); } }
    _r_ths.clear();
  }
}
bool dispatcher_mt::is_valid() const throw() { return _r_ths; }
bool dispatcher_mt::has_session() const throw() { return _r_ths && _r_ths.ref().ses_state == 1; }
void dispatcher_mt::end_session() throw() { if (_r_ths) { _r_ths._pnonc_u()->ses_state = 0; } }
s_long dispatcher_mt::new_proxy(unity& dest, const std::wstring& thread_name) const throw()
{
  dest.clear();
  if (!(_r_ths && _r_ths.ref().ses_state == 1)) { return -3; }

  thread_proxy* p(0);
  try { p = new thread_proxy(); } catch (...) {}
  if (!p) { return -2; }
  s_long res = thread_proxy::_s_init(*p, _r_ths, thread_name);
  if (res < 0) { try { delete p; } catch (...) {} return res >= -2 ? res : -2; }
  try { dest.set_obj<thread_proxy, o_interfaces<thread_proxy, i_dispatcher_mt> >(*p, true); } catch (...) { try { delete p; } catch (...) {} return -2; }
  return 1;
}

s_long dispatcher_mt::request(s_long rt, unity& retval, const unity& args) throw()
  { try { return thread_proxy::_s_request(_r_ths, rt, retval, args, std::wstring()); } catch (...) { return -2; } }


}

#endif // bmdx_part_dispatcher_mt


