// vecm_hashx.h 1.2
// High-performance transactional vector and hash map with access by key and ordinal number.
// Author: Yevgueny V. Kondratyev (Dnipro (Dnepropetrovsk), Ukraine/ex-USSR, 2014-2016)
// Project website: hashx.dp.ua
// Contacts: ceqq [at] mail [dot] ru, sprdwn [at] gmail [dot] com
// See also "Guidelines for using this file" at the bottom.

#ifndef yk_c_vecm_hashx_H
#define yk_c_vecm_hashx_H
#include <new>

#undef _yk_reg
#if __cplusplus > 199711L
  #define _yk_reg
#else
  #define _yk_reg register
#endif
#if defined(__clang__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wunknown-pragmas"
#endif
#if defined(__GNUC__)
  #pragma GCC diagnostic ignored "-Wpragmas"
  #pragma GCC diagnostic ignored "-Wundefined-bool-conversion"
#endif

namespace yk_c
{
struct meta
{
  struct nothing {};

    // Static assertion.
  template<bool cond, class _ = nothing> struct assert { typedef bool t_false; };  template<class _> struct assert<true, _> { enum { result = true }; typedef bool t_true; };

    // Static check for equality of two values of type V.
  template<class V, V x1, V x2, class _ = nothing, bool __b = x1 == x2> struct assert_eq { }; template<class V, V x1, V x2, class _> struct assert_eq<V, x1, x2, _, true> { enum { result = true }; typedef bool EQ; };

    // if_t:: result == T1 if cond is true, or T2 if cond is false.
  template<bool cond, class T1, class T2> struct if_t { typedef T1 result; }; template<class T1, class T2> struct if_t<false, T1, T2> { typedef T2 result; };

    // ifv_t:: result == a if cond is true, or b if cond is false.
  template<bool cond, class T, T a, T b> struct ifv_t { static const T result = a; }; template<class T, T a, T b> struct ifv_t<false, T, a, b> { static const T result = b; };

    // same_t::result is T1 == T2.
    // If T1 == T2, same_t::t is T_eq. If T1 != T2, same_t::t is T_neq.
  template<class T1, class T2, class T_eq = T1, class T_neq = nothing> struct same_t { typedef T_neq t; enum { result = false }; }; template<class T, class T_eq, class T_neq> struct same_t<T, T, T_eq, T_neq> { typedef T_eq t; enum { result = true }; };

    // Getting non-const type + converting an array to pointer.
  template<class T, int __s = 0> struct nonc_t { typedef T t; }; template<class T> struct nonc_t<const T, 0> { typedef T t; }; template<class T> struct nonc_t<const T*, 0> { typedef T* t; }; template<class T> struct nonc_t<const T[], 0> { typedef T* t; }; template<class T, int __s> struct nonc_t<const T[__s], 0> { typedef T* t; }; template<class T, int __s> struct nonc_t<T[__s], 0> { typedef T* t; }; template<class T> struct nonc_t<const T&, 0> { typedef T& t; };

    // Remove reference (&) from the type.
  template<class T, int _ = 0> struct nonr_t { typedef T t; }; template<class T> struct nonr_t<T&, 0> { typedef T t; };

    // find_size_n::result contains a first of the listed types, whose sizeof is n, or else struct nothing.
  template <int n, class t1=nothing, class t2=nothing, class t3=nothing, class t4=nothing, class t5=nothing>
    struct find_size_n { typedef typename if_t<sizeof(t1) == n, t1, typename find_size_n<n, t2, t3, t4, nothing>::result>::result result; };
  template <int n> struct find_size_n<n> { typedef nothing result; };

    // 32-bit integers.
  typedef find_size_n<4, signed long int, signed int, signed short>::result s_long;
  typedef find_size_n<4, unsigned long int, unsigned int, unsigned short>::result u_long;

    // Max. positive value of this type. NOTE It may not be considered as binary mask.
  static const s_long s_long_mp = 0x7fffffffl;

    // 64-bit integers.
  typedef find_size_n<8, signed long long int, signed long int, signed int>::result s_ll;
  typedef find_size_n<8, unsigned long long int, unsigned long int, unsigned int>::result u_ll;

    // Type to hold signed diffirence of pointers.
  typedef if_t<(sizeof(void*) > 4), s_ll, s_long>::result t_pdiff;

    // is_ptr::result is true if T1 is a pointer, otherwise false
  template<class T> struct is_ptr { enum { result = false }; }; template<class T> struct is_ptr<T*> { typedef T t; enum { result = true }; };

    // type_equi: resolves 0..* types T1 into single T2, plus additional information.
    //  a) When t_3 is not defined for T1, T2, this means that T1, T2 are unrelated.
    //  b) When t_3 is defined in any way, T1 is equivalent of T2, i.e. resolving unambiguously in one direction (T1-->T2).
    //  NOTE t_3 most often should be set to T2, but may be of another (T1 or some third) type, depending on particular application.
  template<class T1, class T2> struct type_equi {}; template<class T> struct type_equi<T, T> { typedef T t_3; };

  template <class C, class Tag1, class Tag2> struct _resolve2_t  { enum { result = false }; }; template <class C, class Tag1> struct _resolve2_t<C, Tag1, typename type_equi<C, Tag1>::t_3> { enum { result = true }; };

    // resolve_t:: result is true when class C is tagged by (is equivalent to) Tag.
    //  Namely, type_equi<C, Tag> specialization contains typedef Tag t_3. See type_equi.
  template <class C, class Tag> struct resolve_t  { enum { result = _resolve2_t<C, Tag, Tag>::result }; };

  struct tag_functor {};

    // T() replacement for vecm and other structures.
    //  Aux can serve as additional discriminator for particular T at the time of construction.
    // NOTE Custom constructors.
    //  Any custom structure my_ctor may be defined/used same as construct_f.
    //    If must 1) supply t, f, (but no need to supply aux or be template),
    //    2) specialize meta::type_equi<my_ctor, meta::tag_construct> { typedef meta::tag_construct t_3; };
    //    3) specialize meta::type_equi<my_ctor, meta::tag_functor> { typedef meta::tag_functor t_3; };
    //    4) (optional, recommended) in my_ctor, define
    //      typedef typename meta::template type_equi<my_ctor, meta::tag_construct>::t_3 __check1;
    //      typedef typename meta::template type_equi<my_ctor, meta::tag_functor>::t_3 __check2;
    //      This statically ensures that type_equi declaration made on step 2 is correct.
    //  After steps 1-3, my_ctor will be accepted by instance by vecm el_append* and el_insert*,
    //    and by type by hashx<K, my_ctor, Kf>. In the latter case, hashx assumes my_ctor::t as value type.
    // NOTE f(t* p) must be const since functors are passed via "const T&" same as element values.
    //    When functor contains non-const data, the data should be declared mutable.
    // NOTE (!) Custom constructors are expected to throw an exception when failed to initialize the destination.
    // NOTE In some applications, construct_f template argument may be T equivalent (T_alt), not T itself.
    //    In this case, specialization may look like
    //    a) template <class Aux> struct construct_f<T_alt, Aux> { typedef typename vecm::specf<T_alt>::t_value t; typedef Aux aux; inline void f(t* p) const { new (p) t(); } };
    //    b) template <class Aux> struct construct_f<T_alt, Aux> { typedef T t; typedef Aux aux; inline void f(t* p) const { new (p) t(); } };
    //    See nt_allowDfltConstruct and ta_meta_construct in vecm::config_aux, and also vecm(ptyper p, s_long base).
  struct tag_construct {};
  template <class T, class Aux = nothing> struct construct_f { typedef T t; typedef Aux aux; inline void f(t* p) const { new (p) t(); } };
    template<class T, class Aux> struct type_equi<construct_f<T, Aux>, tag_construct> { typedef tag_construct t_3; };
    template<class T, class Aux> struct type_equi<construct_f<T, Aux>, tag_functor> { typedef tag_functor t_3; };

    // Distinguishing between an element type and a functor. Tag must be valid functor, i.e. have type_equi<Tag, tag_functor>.
    // NOTE Empty specs. are for invalid cases.
  template<class TF, class Tag, int __cat = 4 * resolve_t<TF, tag_functor>::result + 2 * resolve_t<TF, Tag>::result + 1 * resolve_t<Tag, tag_functor>::result> struct resolve_TF;
    template<class TF, class Tag> struct resolve_TF<TF, Tag, 0> {};
    template<class TF, class Tag> struct resolve_TF<TF, Tag, 2> {};
    template<class TF, class Tag> struct resolve_TF<TF, Tag, 4> {};
    template<class TF, class Tag> struct resolve_TF<TF, Tag, 6> {};
    template<class TF, class Tag> struct resolve_TF<TF, Tag, 1> { enum { is_functor = false, use_functor = false, use_value = true }; typedef TF t; };
    template<class TF, class Tag> struct resolve_TF<TF, Tag, 3> {};
    template<class TF, class Tag> struct resolve_TF<TF, Tag, 5> { enum { is_functor = true }; };
    template<class TF, class Tag> struct resolve_TF<TF, Tag, 7> { enum { is_functor = true, use_functor = true, use_value = false }; typedef typename TF::t t; };

// meta copy_t
//==section 1==============================================

    // Custom copy constructor.
    //    By default, undefined. See also copierMode 2 in config_t.
    // Behavior (required):
    //    On entry: *psrc contains a valid object, *pdest is not initialized.
    //    On exit / on_calm_result: *pdest contains a valid object, which is a copy of *psrc. *psrc is unchanged.
    //    On failure: F() throws an exception. *pdest is left totally uninitialized. *psrc is unchanged.
    // NOTE struct may be specialized with T equivalent as template argument, but T as F argument. See ta_meta_copy and others in vecm::config_aux.
  template<class T> struct copy_t { typedef typename assert_eq<int, sizeof(T), -1, T>::EQ __check; static inline void F(T* pdest, const T* psrc) { } };

    // Custom safe copy constructor.
    //    By default, undefined. See also copierMode 3 in config_t.
    // Behavior (required):
    //    F() must always succeed and throw no exceptions.
    //    On entry: *psrc contains a valid object, *pdest is not initialized.
    //    On exit / on_calm_result: *pdest contains a valid object, which is a copy of *psrc. *psrc is unchanged.
    // NOTE struct may be specialized with T equivalent as template argument, but T as F argument. See ta_meta_copy and others in vecm::config_aux.
  template<class T> struct safecopy_t { typedef typename assert_eq<int, sizeof(T), -1, T>::EQ __check; static inline void F(T* pdest, const T* psrc) throw() { } };

    // Optimized moving from *psrc to *pdest.
    //    By default, undefined. See also moverMode 2 in config_t.
    // NOTE As its best, F() may take into account if only one part of source object is unsafe to move, and try moving it first.
    //    If such is not possible, normal copy/destroy src. sequence may be tried.
    // NOTE If trymove_t::F() fails to move the object, its client should not retry moving in any way.
    // Behavior (required):
    //    F() must throw no exceptions.
    //    On entry: *psrc contains a valid object, *pdest is not initialized.
    //    On exit / on_calm_result: F() returns 1. *pdest contains a valid object, logically identical to former *psrc. *psrc is uninitialized.
    //    On exit / on_calm_result + src. destructor failure:  F() returns 0. *pdest contains a valid object, *psrc is considered uninitialized.
    //    On exit / failure: F() returns -1. *pdest is left totally uninitialized. *psrc is unchanged.
    // NOTE struct may be specialized with T equivalent as template argument, but T as F argument. See ta_meta_copy and others in vecm::config_aux.
  template<class T> struct trymove_t { typedef typename assert_eq<int, sizeof(T), -1, T>::EQ __check; static inline s_long F(T* pdest, T* psrc) throw() { return 0; } };

    // Safe moving from *psrc to *pdest.
    //    By default, undefined. See also moverMode 3 in config_t.
    // Behavior (required):
    //    F() must always succeed and throw no exceptions.
    //    On entry: *psrc contains a valid object, *pdest is not initialized.
    //    On exit / on_calm_result: *pdest contains a valid object, logically identical to former *psrc. *psrc is uninitialized.
    // NOTE struct may be specialized with T equivalent as template argument, but T as F argument. See ta_meta_copy and others in vecm::config_aux.
  template<class T> struct safemove_t { typedef typename assert_eq<int, sizeof(T), -1, T>::EQ __check; static inline void F(T* pdest, T* psrc) throw() { } };

    // Custom destructor.
    //    By default, undefined. See also destructorMode 2 in config_t.
    // Behavior (required):
    //    On entry: p points to a valid object.
    //    On exit / on_calm_result: the object is totally uninitialized.
    //    On failure: F() throws an exception. *p is undefined, though considered uninitialized.
    //      NOTE If an error occurs during destruction, F() must complete as much as possible
    //      of the destruction sequence, and only then generate the exception.
    // NOTE struct may be specialized with T equivalent as template argument, but T as F argument. See ta_meta_copy and others in vecm::config_aux.
  template<class T> struct destroy_t { typedef typename assert_eq<int, sizeof(T), -1, T>::EQ __check; static inline void F(T* p) { } };

private:
    typedef assert_eq<int, (unsigned char)(3 << 7), 128, char>::EQ __check1; typedef assert_eq<int, (signed char)(3 << 7), -128, char>::EQ __check2;
    static const s_long __1 = 1; static const s_long __2 = 2; static const s_long __4 = 4; static const s_long __0 = 0; static const s_long __30 = 30;
      typedef assert_eq<s_long, ((__1 << __30) - __1) << __2, -__4, s_long>::EQ __check3; typedef assert_eq<s_long, ((((__1 << __30) - __1) << __2) >> __2) + __1, +__0, s_long>::EQ __check4;
      // The following check limits possible ways of structure alignment for the correct byte-level access
      //    to 3 leading data members of vecm and 8 leading data members of vecm::type_descriptor.
    struct __s1 { s_long a; s_long b; }; struct __s2 { s_long a; s_long b; __s1* p; __s2() {} ~__s2() {} }; struct __s3 { s_long* p1; void* p2; void* (*p3)(s_long); void (*p4)(const __s2*); s_long (*p5)(__s2*, const __s2* psrc); s_long (*p6)(__s2*, const __s2* psrc, s_long); s_long (*p7)(__s2*); void* p8; };
      typedef assert<(sizeof(__s1) == 8 && sizeof(__s2) <= 8 + sizeof(void*) + 4 && sizeof(__s3) == 8 * sizeof(void*))>::t_true __check5;
    typedef assert<(sizeof(std::size_t) >= sizeof(s_long))>::t_true __check6;
};

template<> struct meta::type_equi<meta::tag_construct, meta::tag_functor> { typedef meta::tag_functor t_3; };


typedef meta::s_long s_long;

struct vecm;
template<class T, class Aux> struct lock_impl_t;
struct bytes
{
    // Whole number log2(). Returns -1 for 0; 31 for negative values
  static inline s_long ilog2(s_long x) throw() { if (x > 0) { _yk_reg s_long n = 0; while (x >>= 1) { ++n; } return n; } else { return x < 0 ? 31 : -1; } }
  template<s_long x, bool b = (x < 0)> struct ilog2_t { enum { f = ilog2_t<(x>>1)>::f + 1 }; }; template<s_long x> struct ilog2_t<x, true> { enum { f = 31 }; }; template<bool b> struct ilog2_t<0, b> { enum { f = -1 }; };

    // Max. positive number of an integer type.
  template<class I, I n = 0, bool __next = (I(I(n << I(1)) | I(1)) > I(0) && (I(I(n << I(1)) | I(1)) != n))> struct maxpn_t { static const I f = n; }; template<class I, I n> struct maxpn_t<I, n, true> { static const I f = maxpn_t<I, I(I(n << I(1)) | I(1))>::f; };

  static inline s_long min_sl(s_long x1, s_long x2) throw() { return x1 <= x2 ? x1 : x2; }
  static inline s_long max_sl(s_long x1, s_long x2) throw() { return x1 >= x2 ? x1 : x2; }

    // mem_is_null, mem_is_null_t return:
    //  1, if the object is whole filled with zero bytes, or nbytes <= 0
    //  0 if it has at least one non-0 byte,
    //  -1 if ptr == 0.
    // NOTE In 32-bit system, max. size for checked block is 2^31-1 bytes.
  static inline s_long mem_is_null(void* ptr, meta::t_pdiff nbytes) throw() { if (!ptr) { return -1; } _yk_reg meta::t_pdiff n1 = nbytes;  if (n1 <= 0) { return 1; } _yk_reg char* p = reinterpret_cast<char*>(ptr); do { if (*p++) { return 0; } } while(--n1); return 1; }
  template<class T> static inline s_long mem_is_null_t(T* ptr) throw() { if (!ptr) { return -1; } _yk_reg s_long n1 = sizeof(T);  if (n1 <= 0) { return 1; } _yk_reg char* p = reinterpret_cast<char*>(ptr); do { if (*p++) { return 0; } } while(--n1); return 1; }

    // mem_set_null, mem_set_null_t:
    //  a) set nbytes bytes to 0 starting from memory location ptr,
    //  b) do nothing if ptr == 0 or nbytes <= 0.
    // NOTE In 32-bit system, max. memory block size is 2^31-1 bytes.
  static inline void mem_set_null(void* ptr, meta::t_pdiff nbytes) throw() { if (!ptr) { return; } _yk_reg meta::t_pdiff n1 = nbytes;  if (n1 <= 0) { return; } _yk_reg char* p = reinterpret_cast<char*>(ptr); do { *p++ = 0; } while(--n1); }
  template<class T> static inline void mem_set_null_t(T* ptr) throw() { if (!ptr) { return; } _yk_reg s_long n1 = sizeof(T);  if (n1 <= 0) { return; } _yk_reg char* p = reinterpret_cast<char*>(ptr); do { *p++ = 0; } while(--n1); }

    // Exact equality of strings, given number of characters in each.
    //    Any n == -1 means that corresp. string is 0-terminated.
    //  Returns:
    //    a) true if strings are equal,
    //    b) false if strings are not equal,
    //    c) false if any (or both) of ptr1, ptr2 is 0.
    // NOTE In 32-bit system, max. string length is 2^31-1.
  template<class T> static inline bool is_eq_str(const T* ptr1, const T* ptr2, meta::t_pdiff n1, meta::t_pdiff n2) throw()
  {
    _yk_reg meta::t_pdiff n = n1;
    if (n == n2) { _yk_reg const T* p1 = ptr1; if (!p1) { return false; } _yk_reg const T* p2 = ptr2; if (!p2) { return false; } if (p1 == p2 || n == 0) { return true; } if (n > 0) { do { if (*p1++ != *p2++) { return false; } } while (--n); } else { _yk_reg T x; do { x = *p1++; if (x != *p2++) { return false; } } while (x); } return true; }
    else
    {
      if (n >= 0) { if (n2 >= 0) { return false; } _yk_reg const T* p1 = ptr2; if (!p1) { return false; } _yk_reg const T* p2 = ptr1; if (!p2) { return false; } _yk_reg T x; do { x = *p1++; if (n == 0) { return !x; } if (!x || x != *p2) { return false; } p2++; --n; } while (true); }
        else { n = n2; if (n < 0) { _yk_reg const T* p1 = ptr1; if (!p1) { return false; } _yk_reg const T* p2 = ptr2; if (!p2) { return false; } if (p1 == p2) { return true; } _yk_reg T x; do { x = *p1++; if (x != *p2++) { return false; } } while (x); return true; } else { _yk_reg const T* p1 = ptr1; if (!p1) { return false; } _yk_reg const T* p2 = ptr2; if (!p2) { return false; } _yk_reg T x; do { x = *p1++; if (n == 0) { return !x; } if (!x || x != *p2) { return false; } p2++; --n; } while (true); } }
    }
  }

    // Default "less" operation for keys of same or different type. Both less12(a, b), less21(a, b) return true if a < b.
  template<class K, class K2 = K, class _ = meta::nothing> struct less_t { bool less12(const K& a, const K2& b) const { return a < b; } bool less21 (const K2& a, const K& b) const { return a < b; } };

// bytes config0_t
//==section 2==============================================

    // Fixed properties of data type T (not changeable by vecm::spec).
    //    May be specialized only for user types.
    //
    // useAssignToCopy == true : vecm will 1) copy-construct, 2) move
    //    an element with single assignment instruction.
    //    Assignment speeds up append / insert / remove functions
    //    on small types, because it performs less checks and nested calls per value.
    //  NOTE For a type, useAssignToCopy == true effects only if certain conditions are met:
    //    1) Moving by assignment is enabled only when vecm spec has trivial moving:
    //        a) destructorMode 0, moverMode 4.
    //        b) destructorMode 0, moverMode 1 with copierMode 1 or 4.
    //    2) Copying by assignment is enabled only when vecm spec has trivial copying:
    //        a) copierMode 4.
    //        b) destructorMode 0, copierMode 1
    //          + operator=() to uninitialized destination works correctly.
    //    If the container is initialized with type equivalent having parameters different
    //    from the above, assignment will be disabled for this container.
    //
    // isKnown1 - internal flag to discern certain built-in types. Must be false in any
    //    user-side config0_t specializations.
    //    See also: usage of isKnown1 in this module.
  struct config0_dflt { enum { useAssignToCopy = false, isKnown1 = false }; };
  template<class T> struct config0_t : config0_dflt { };

    // User-side enabling the default constructor in type descriptor.
    //  enable == true: for the chosen type or equivalent, init vecm type_descriptor pcd_o
    //    with appropriate fn. (vecm config_ff construct_f F).
    //    Factually, F calls meta::construct_f for ta_meta_construct specified in type config.
    //  Setting enable true overrides vecm config_t nt_allowDefaultConstruct,
    //    even if the target type configuration is already specialized somehow.
    //    See also nt_allowDefaultConstruct, ta_meta_construct.
    // enable == false (default): only types with (nt_allowDefaultConstruct && !transactional)
    //    will have pcd_o non-0 (for storage recovery).
  template<class TA, class Aux = meta::nothing> struct config_cd_t { enum { enable = false }; };

    // Type-ignorant moving memory blocks.
    //    n specifies the number of bytes.
    //    May be specialized for trivial types like char, short etc., but this is not recommended.
    //  NOTE (!) Even if memmove_t is specialized, it must throw no exceptions.
    //  NOTE Standard memmove is NOT faster than memmove_t in most cases.
    //  NOTE In 32-bit system, max. size of moved memory block is 2^31-1.
  template<class T, class _ = meta::nothing> struct memmove_t
  {
    static inline void F(void* dest, const void* src, meta::t_pdiff n) throw() // 1) dest, src are valid, 2) 32-bit arch. n == [0..2^31 - 1]
    {
      if (n < 8)
      {
        if (n == 4)
        {
          _yk_reg s_long h = s_long((reinterpret_cast<char*>(dest)-static_cast<char*>(0)) | (reinterpret_cast<const char*>(src)-static_cast<const char*>(0)));
          h &= 3; if (h == 0 && dest != src) { *reinterpret_cast<s_long*>(dest) = *reinterpret_cast<const s_long*>(src); return; }
        }
        if (n < 1) { return; }
        if (dest < src) { _yk_reg meta::t_pdiff n1 = n; _yk_reg const char* q = reinterpret_cast<const char*>(src); _yk_reg char* q2 = reinterpret_cast<char*>(dest); do { *q2++ = *q++; } while (--n1); }
        else if (dest > src) { _yk_reg meta::t_pdiff n1 = n; _yk_reg const char* q = reinterpret_cast<const char*>(src); q += n1; --q; _yk_reg char* q2 = reinterpret_cast<char*>(dest); q2 += n1; --q2; do { *q2-- = *q--; } while (--n1); }
      }
      else
      {
        _yk_reg s_long h = s_long((reinterpret_cast<char*>(dest)-static_cast<char*>(0)) | (reinterpret_cast<const char*>(src)-static_cast<const char*>(0)));
        h &= 7;
        if (dest < src)
        {
          if (h == 0) { _yk_reg const meta::s_ll* p = reinterpret_cast<const meta::s_ll*>(src); _yk_reg meta::s_ll* p2 = reinterpret_cast<meta::s_ll*>(dest); _yk_reg meta::t_pdiff n1 = n; n1 >>= 3; do { *p2++ = *p++; } while (--n1); n1 = n; n1 &= 7; if (n1) { _yk_reg const char* q = reinterpret_cast<const char*>(p); _yk_reg char* q2 = reinterpret_cast<char*>(p2); do { *q2++ = *q++; } while (--n1); } return; }
          h &= 3; if (h == 0) { _yk_reg const s_long* p = reinterpret_cast<const s_long*>(src); _yk_reg s_long* p2 = reinterpret_cast<s_long*>(dest); _yk_reg meta::t_pdiff n1 = n; n1 >>= 2; do { *p2++ = *p++; } while (--n1); n1 = n; n1 &= 3; if (n1) { _yk_reg const char* q = reinterpret_cast<const char*>(p); _yk_reg char* q2 = reinterpret_cast<char*>(p2); do { *q2++ = *q++; } while (--n1); } return; }
          _yk_reg meta::t_pdiff n1 = n; if (n1) { _yk_reg const char* q = reinterpret_cast<const char*>(src); _yk_reg char* q2 = reinterpret_cast<char*>(dest); do { *q2++ = *q++; } while (--n1); }
        }
        else if (dest > src)
        {
          if (h == 0) { _yk_reg meta::t_pdiff n1 = n; n1 &= 7; if (n1) { _yk_reg const char* q = reinterpret_cast<const char*>(src); q+=n; --q; _yk_reg char* q2 = reinterpret_cast<char*>(dest); q2+=n; --q2; do { *q2-- = *q--; } while (--n1); } n1 = n;  n1 >>= 3; _yk_reg const meta::s_ll* p = reinterpret_cast<const meta::s_ll*>(src); p += n1; --p; _yk_reg meta::s_ll* p2 = reinterpret_cast<meta::s_ll*>(dest); p2 += n1; --p2; do { *p2-- = *p--; } while (--n1); return; }
          h &= 3; if (h == 0) { _yk_reg meta::t_pdiff n1 = n; n1 &= 3; if (n1) { _yk_reg const char* q = reinterpret_cast<const char*>(src); q+=n; --q; _yk_reg char* q2 = reinterpret_cast<char*>(dest); q2+=n; --q2; do { *q2-- = *q--; } while (--n1); } n1 = n;  n1 >>= 2; _yk_reg const s_long* p = reinterpret_cast<const s_long*>(src); p += n1; --p; _yk_reg s_long* p2 = reinterpret_cast<s_long*>(dest); p2 += n1; --p2; do { *p2-- = *p--; } while (--n1); return; }
          _yk_reg meta::t_pdiff n1 = n; if (n1) { _yk_reg const char* q = reinterpret_cast<const char*>(src); q+=n1; --q; _yk_reg char* q2 = reinterpret_cast<char*>(dest); q2+=n1; --q2; do { *q2-- = *q--; } while (--n1); }
        }
      }
    }
  };
    // This structure may be specialized for particular type if necessary. See t_alloc2 in vecm::config_aux.
    //  NOTE For customized functions, throw() spec. is optional.
  template<class T = meta::nothing> struct allocdef_t
  {
    static inline void* Falloc(meta::t_pdiff n) throw() { try { return operator new (n); } catch (...) { return 0; } }
    static inline void Ffree(void* p) throw() { try { operator delete(p); } catch (...) {} }
  };

private:
  template<class G> struct _uid_gt1_t { static s_long gen_id() throw() { s_long n2 = _n;  if (n2 == 0) { n2 = s_long(&_n - static_cast<s_long*>(0)); if (n2 < 0) { n2 += meta::s_long_mp; } } else { n2 += 1; }  if (n2 <= 1) { n2 = 2; } _n = n2; return n2; } private: static s_long _n; };
public:

    // This structure should only be instantiated, not specialized. See t_alloc2 in vecm::config_aux.
    //  Size of allocation must be <= 2^31 - 2 * sizeof(T*).
  template<class T = meta::nothing> struct xalloc2_t : allocdef_t<T>
  {
  private: struct _tmp; public:
    typedef allocdef_t<T> t_alloc1;
    static inline void* xalloc_r(s_long n) throw() { if (n >= 0) { try { char* p = reinterpret_cast<char*>(t_alloc1::Falloc(n + _dr)); *reinterpret_cast<s_long*>(p) = n; return p + _dr; } catch (...) { return 0; } } else return 0; }
    static inline void xfree_r(void* p) throw() { try { if (p) { t_alloc1::Ffree(reinterpret_cast<char*>(p) - _dr); } } catch (...) {} }
    static inline void* xrealloc_r(void* p, s_long n) throw() // NOTE if p == 0, returns 0 (no attempt to allocate)
    {
      if (!p || n < 0) { return 0; }
      s_long* p0 = reinterpret_cast<s_long*>(reinterpret_cast<char*>(p) - _dr); s_long n0 = *p0; s_long n2 = n;
      if (n2 & ~s_long(255)) { s_long m = ilog2(n2) - 2; if (m >= 0) { n2 |= (s_long(1) << m) - 1; } } if (n2 == n0) { return p; }
      void* p2 = xalloc_r(n2); if (!p2) { return 0; }
      if (n > n0) { n = n0; } memmove_t<_tmp>::F(p2, p, n); xfree_r(p); return p2;
    }
  };

    // Synchronization object. By default, does nothing.
    //    To implement locking, the client must define
    //  namespace yk_c { template<class T, class Aux> struct lock_impl_t { typedef T t_target; ... }; }
    //    before first including this header.
    //    Alternatively, lock_impl_t may be defined empty, and specialized
    //    for certain types. NOTE Aux arg. should not be bound.
    //  Required functioning of lock_impl_t:
    //    1. lock_impl_t<T> construction enters critical section, related to T.
    //    2. If the lock has failed (e.g. resource n/a, large timeout etc.), its constructor generates an exception.
    //      NOTE This is the exceptional situation and should be avoided as much as possible.
    //    3. lock_impl_t<T> destruction leaves the critical section.
    //    4. Lock impl. must correctly function during static init., main execution, and cleanup.
    //    5. Lock impl. must not block if the thread inside critical section reenters the section.
    //  The most important locks:
    //    1. lock_t<vecm, meta::nothing>. Created by vecm to protect global structures.
    //      By default (undefined lock_impl_t<vecm, meta::nothing>) does nothing.
    //    2. lock_t<meta::nothing, any type>.
    //      This specialization intentionally does no locking.
    template<class L, class __target> struct _lock_base_t
    {
      enum { can_lock = false };
      inline bool is_lckinit() const throw() { return true; }
      inline bool is_locked() const throw() { return false; }
    };
    template<class L> struct _lock_base_t<L, typename L::t_target>
    {
      enum { can_lock = true, _nq = sizeof(L) }; union { typename meta::if_t<(_nq < 4), s_long, meta::s_ll>::result _st; char _stc[_nq + 1]; };
      inline bool is_lckinit() const throw() { return bool(_stc[_nq]); }
      inline bool is_locked() const throw() { return bool(_stc[_nq]); }
      inline _lock_base_t() throw() { _stc[_nq] = 0; try { new (reinterpret_cast<L*>(_stc)) L(); _stc[_nq] = 1; } catch (...) { return; } }
      inline ~_lock_base_t() throw() { if (_stc[_nq]) { try { L* p = reinterpret_cast<L*>(_stc); p->~L(); } catch (...) {} _stc[_nq] = 0; } }
    };
    template<class T, class Aux = yk_c::meta::nothing> struct lock_t : _lock_base_t<lock_impl_t<T, Aux>, T> {};
    template<class Aux> struct lock_t<meta::nothing, Aux>
    {
      enum { can_lock = false };
      inline bool is_lckinit() const throw() { return true; }
      inline bool is_locked() const throw() { return false; }
    };

    // Type enumerator. T are types that are enumerated. Aux is reserved, must be meta::nothing.
    //    ind() performs synchronized unique index generation and setting xind once to that value.
    //    (Initially, xind == 0.)
    //  Ways of run-time type checking:
    //      if (_t_ind != bytes::type_index_t<T>::ind()) { ... }
    //      if (_t_ind != bytes::type_index_t<T>::xind) { if (_t_ind != bytes::type_index_t<T>::ind()) { ... } }
    //    The second way is generally faster.
    //  NOTE Automatically generated indices Test from 1. 0 stands for invalid value.
    //    Negative values have special meaning.
    //    (Types with fixed binary structure may be assigned neg. indices.
    //    This allows for their high-level crossing module boundary as elements of vecm object.
    //    For example, see bytes::type_index_t<char>.)
  template<class T, class Aux = meta::nothing> struct type_index_t
    { static s_long xind; static inline s_long ind() throw() { if (xind > 1) { return xind; } lock_t<vecm> __lock; if (__lock.is_lckinit()) { if (xind > 1) { return xind; } xind = _uid_gt1_t<vecm>::gen_id(); return xind; } if (xind > 1) { return xind; } s_long i1 = _uid_gt1_t<vecm>::gen_id(); s_long i2 = _uid_gt1_t<vecm>::gen_id(); xind = i2 - i1 > 1 ? i1 : i2; return xind; } };

    // Pseudo-unique type index. uind_t::x.
    //    Based on date and type size. Always negative.
    //    (y, m, d, h) is a date/time value; size may be
    //    a) 1..1024.
    //    b) -1 -- will be replaced by sizeof(T).
    //    For use example, see bytes::type_index_t<char>.
    //  NOTE This type should not be specialized, only instantiated.
  template<class TA, s_long y, s_long m, s_long d, s_long h, s_long size> struct uind_t;

    // Base class for cross-module type index.
    //    size == -1 will be calculated into sizeof(T);
  template<class TA, s_long y, s_long m, s_long d, s_long h, s_long size = -1> struct cmti_base_t
  {
    static const s_long xind;
    static inline s_long ind() throw() { if (_init) { return xind; } cmti_base_t(); return xind; }
    cmti_base_t() throw();
    private: static short _init;
  };

  enum { _ps = sizeof(char*), _dr = meta::ifv_t<_ps == 4, int, 4, ((_ps >> 3) << 3) + (_ps & 7 ? 8 : 0)>::result };
  typedef bool _t_is_defined; // marking struct bytes defined

private:
  struct __altest1 { struct S1 { meta::s_ll* p; s_long n; }; struct S2 { S1 ar[3]; };  };
  typedef meta::assert_eq<int, sizeof(double), 8, double>::EQ __check1;
  typedef meta::assert_eq<int, sizeof(char[256]), 256, char[256]>::EQ __check2;
  typedef meta::assert_eq<int, sizeof(s_long[256]), 1024, s_long[256]>::EQ __check3;
  typedef meta::assert_eq<int, sizeof(meta::s_ll[256]), 2048, meta::s_ll[256]>::EQ __check4;
  typedef meta::assert_eq<int, 3 * sizeof(__altest1::S1), sizeof(__altest1::S2)>::EQ __check5;
};

template<class G> s_long bytes::_uid_gt1_t<G>::_n(0);
template<class T, class Aux> s_long bytes::type_index_t<T, Aux>::xind(1);
template<class TA, s_long y, s_long m, s_long d, s_long h, s_long size> short bytes::cmti_base_t<TA, y, m, d, h, size>::_init(0);

template<> struct bytes::config0_t<bool> { enum { useAssignToCopy = true, isKnown1 = true }; };
template<> struct bytes::config0_t<char> { enum { useAssignToCopy = true, isKnown1 = true }; };
template<> struct bytes::config0_t<wchar_t> { enum { useAssignToCopy = true, isKnown1 = true }; };
template<> struct bytes::config0_t<signed char> { enum { useAssignToCopy = true, isKnown1 = true }; };
template<> struct bytes::config0_t<unsigned char> { enum { useAssignToCopy = true, isKnown1 = true }; };
template<> struct bytes::config0_t<signed short int> { enum { useAssignToCopy = true, isKnown1 = true }; };
template<> struct bytes::config0_t<unsigned short int> { enum { useAssignToCopy = true, isKnown1 = true }; };
template<> struct bytes::config0_t<signed int> { enum { useAssignToCopy = true, isKnown1 = true }; };
template<> struct bytes::config0_t<unsigned int> { enum { useAssignToCopy = true, isKnown1 = true }; };
template<> struct bytes::config0_t<signed long int> { enum { useAssignToCopy = true, isKnown1 = true }; };
template<> struct bytes::config0_t<unsigned long int> { enum { useAssignToCopy = true, isKnown1 = true }; };
template<> struct bytes::config0_t<signed long long int> { enum { useAssignToCopy = true, isKnown1 = true }; };
template<> struct bytes::config0_t<unsigned long long int> { enum { useAssignToCopy = true, isKnown1 = true }; };
template<> struct bytes::config0_t<float> { enum { useAssignToCopy = true, isKnown1 = true }; };
template<> struct bytes::config0_t<double> { enum { useAssignToCopy = true, isKnown1 = true }; };
template<> struct bytes::config0_t<long double> { enum { useAssignToCopy = true, isKnown1 = true }; };
template<class T> struct bytes::config0_t<T*> { enum { useAssignToCopy = true, isKnown1 = true }; };

  // Several cross-module types.
  //  NOTE In user program, in addition to specializing any user type as cross-module,
  //    an object bytes::type_index_t<user type> must be created at least once,
  //    either statically, or in a function. This registers its index in the global set,
  //    and thus enables mapping types from containers from another binary module
  //    to types in the current module.
template<> struct bytes::type_index_t<char> : cmti_base_t<char, 2015, 4, 17, 8> {};
template<> struct bytes::type_index_t<s_long> : cmti_base_t<s_long, 2015, 4, 17, 8> {};
template<> struct bytes::type_index_t<double> : cmti_base_t<double, 2015, 4, 17, 8> {};
template<> struct bytes::type_index_t<meta::s_ll> : cmti_base_t<meta::s_ll, 2015, 5, 6, 10> {};






struct vecm
{
protected: struct _column;
public:
  template<class TA> struct spec;

    // Dynamic object corresponding to config_t + config_aux.
    //    Non-private fields and function pointers may be useful on the client side.
    //    to perform specific operations without knowing the container elem. type.
    //  See also struct _td_inst.
  struct type_descriptor
  {
    s_long* psig; // object structure digest
    void* ptd2; // optional user-defined information (may be initialized by ptd2_t specialization)
    void* (*_pstatic)(s_long id); // (private) static information
    void (*pvecm_destroy)(const vecm* p); // local vecm destructor
    s_long (*pvecm_delete)(const vecm* p); // local vecm op. delete
    s_long (*pvecm_copy)(vecm* pdest, const vecm* psrc, s_long is_tr); // local vecm_copy impl.
    s_long (*pvecm_clear)(vecm* p); // local vecm_clear impl.
    void* _pr1; // reserved

    s_long (*p_copy_1)(void* pdest, const void* psrc); // copy single elem.
    s_long (*p_move_1)(void* pdest, void* psrc); // move single elem.
    s_long (*p_destroy_1)(void* p); // call destructor on single elem. if necessary
        typedef void* (*fpmem_alloc)(meta::t_pdiff); typedef void (*fpmem_free)(void*); typedef void* (*fpmem_realloc)(void*, meta::t_pdiff);
        typedef void* (*fpmem_alloc32)(s_long); typedef void* (*fpmem_realloc32)(void*, s_long);
    fpmem_alloc pma1;
    fpmem_free pmf1; // pma1, pmf1 -- allocator for elements
    fpmem_alloc pma2;
    fpmem_free pmf2; // pma2, pmf2 -- allocator for internal data
    fpmem_alloc32 _pxmalloc_r;
    fpmem_free _pxmfree_r;
    fpmem_realloc32 _pxmrealloc_r; // _pxm* (private) -- special allocator NOTE internal code expects non-throwing fns.

    s_long t_ind; // run-time element type index
    s_long ta_ind; // run-time equivalent type index
    s_long t_size; // element sizeof()
    s_long dmode;
    s_long mmode;
    s_long cmode; // *mode -- destructor, mover, copier modes
    char gc; // is copying fautless
    char gm; // is moving fautless
    char op_flags; // 0x1: value of config_aux:: transactional; 0x2 and higher: see near "op_flags = "
    char nt_allowDfltConstruct;
    char nt_allowNullify;
    char u_ignoreNullsOnCleanup;
    char u_ignoreDestructorsOnCleanup; // from config_aux
    char is_ptr; // != 0 if elem is a pointer, 0 if not

    s_long (*_pelscp)(_column* pcdest, _column* pcsrc, s_long k0, s_long k2, s_long cap); // (private) copy elems. between columns
    s_long (*_pelsdc)(_column* pc, s_long is_obj_unsafe, s_long k0, s_long k2, s_long cap); // (private) destroy column elements
    s_long (*pcd_o)(void* p); // optional dflt. construction (may be 0)
    s_long (*_pntrecov)(void* p); // (private) initialize single value using configured recovery methods
  };

private:
  vecm(); vecm& operator=(const vecm&);
protected:

  struct _column
  {
    mutable char* pd; s_long kbase;

    void init_move_from(const _column& x) throw() { pd = x.pd; kbase = x.kbase; x.pd = 0; }
    void* init_alloc(const type_descriptor* _ptd, s_long nbytes) throw() { kbase = 0; try { pd = reinterpret_cast<char*>(_ptd->pma1(nbytes)); } catch (...) { pd = 0; } return pd; }
    inline void clear(const type_descriptor* _ptd) throw() { if (pd) { try { _ptd->pmf1(pd); } catch (...) {} pd = 0; } }
    template<class T> inline void get_pd_kbase(T*& pd_, s_long& kbase_) const throw() { pd_ = reinterpret_cast<T*>(pd); kbase_ = kbase;  }
    template<class T> inline T* at_pd0u() throw() { return reinterpret_cast<T*>(pd); } // first element in the array (not kbase-shifted)
    template<class T> inline T* at_ku(s_long k, s_long cap) throw() { _yk_reg s_long r = k; r += kbase; if (r & cap) { r ^= cap; } return reinterpret_cast<T*>(pd) + r; }
    template<class T> inline T* at_kmu(s_long k, s_long mask) throw() { _yk_reg s_long r = k; r += kbase; r &= mask; return reinterpret_cast<T*>(pd) + r; }
    template<class T> inline T* at_0u() throw() { return reinterpret_cast<T*>(pd) + kbase; } // first element
    template<class T> inline T* at_9u(s_long cap) throw() { _yk_reg s_long r = kbase; if (r) { --r; } else { r = cap; --r; } return reinterpret_cast<T*>(pd) + r; } // the last element of full column
    inline char* atc_ku(s_long k, s_long cap, s_long t_size) throw() { _yk_reg s_long r = k; r += kbase; if (r & cap) { r ^= cap; } r *= t_size; return pd + r; }
    inline char* atc_0u(s_long t_size) throw() { _yk_reg s_long r = kbase; r *= t_size; return pd + r; }
    inline char* atc_9u(s_long cap, s_long t_size) throw() { _yk_reg s_long r = kbase; if (r) { --r; } else { r = cap; --r; } r *= t_size; return pd + r; } // the last element of full column
    inline void kbase_incr(s_long cap) throw() { ++kbase; if (kbase & cap) { kbase = 0; } }
    inline void kbase_decr(s_long cap) throw() { if (kbase) { --kbase; } else { kbase = cap - 1; } }

    private: _column(const _column&); _column& operator=(const _column&); _column(); ~_column();
  };

    // Mathematics.
  struct m
  {
      // 3 sequential parts.
      //  Each part i has m[i] containers ("columns") by c[i] elements.
      //  m[3] is not limited.
      //
      // (primary!) shift for getting column capacity: p1 = 3, p2 = 8, p3 = 13
      //    capacity: c1=2^p1, c2=2^p2, c3=2^p3 (8, 256, 8192)
      // (primary!) number of columns m1 = 128 (NOTE (m1*c1) / c2  must be whole number) ,
      // (primary!) imaginary number of columns m2 (as if m1 == 0): m2 = 1024 (NOTE (m2*c2) / c3  must be whole number),
      //    max. number of elements in a part: n1 = m1 * c1, n2 = m2 * c2
      //    compensation delta for column index: (j), d2 = m1 - n1/c2, d3 = m2 - n2/c3 + d2 (124, 1116)
      //    starting column index of the third part: ind3 = m2 + d2 (1148)
      //    mask for element pos. in column: mask1 = c1-1, mask2 = c2-1, mask3 = c3-1
      //    mask for determining the part of the given index: part 2: ~(n1 - 1), part 3: ~(n2 - 1)
    enum { p1 = 3, p2 = 8, p3 = 13, m1 = 32, m2 = 512, c1 = 1<<p1, c2 = 1<<p2, c3 = 1<<p3, n1 = m1 * c1, n2 = m2 * c2, d2 = m1 - n1/c2, d3 = m2 - n2/c3 + d2, ind3 = m2 + d2, mask1 = c1-1, mask2 = c2-1, mask3 = c3-1 };

    static inline void _ind_jk(s_long i, s_long& rj, s_long& rk, s_long& rcap) throw()
    {
      _yk_reg s_long ra = i; _yk_reg s_long rb = ra;
      if (ra & ~s_long(n2-1)) { ra >>= p3; ra += d3; rj = ra; rb &= mask3; rk = rb; rcap = c3; } else { if (ra & ~s_long(n1-1)) { ra >>= p2; ra += d2; rj = ra; rb &= mask2; rk = rb; rcap = c2; } else { ra >>= p1; rj = ra; rb &= mask1; rk = rb; rcap = c1; } }
    }
    static inline void _ind_jk(s_long i, s_long& rj, s_long& rk) throw()
    {
      _yk_reg s_long ra = i; _yk_reg s_long rb = ra;
      if (ra & ~s_long(n2-1)) { ra >>= p3; ra += d3; rj = ra; rb &= mask3; rk = rb; } else { if (ra & ~s_long(n1-1)) { ra >>= p2; ra += d2; rj = ra; rb &= mask2; rk = rb; } else { ra >>= p1; rj = ra; rb &= mask1; rk = rb; } }
    }
    static inline s_long _cap_j(s_long j) throw()
    {
      _yk_reg s_long r = j; if (r & ~s_long(m1-1)) { if (r < ind3) { return c2; } else { return c3; } } else  { return c1; }
    }
    static inline s_long _nrsv(s_long nj) throw()
    {
      _yk_reg s_long r = nj;
      if (r & ~s_long(m1-1)) { if (r < 0) { return 0; } if (r < ind3) { r -= d2; r <<= p2; return r; } else { r -= d3; if (r > ((meta::s_long_mp >> p3) + 1)) { return meta::s_long_mp; } r <<= p3; return r >= 0 ? r : meta::s_long_mp; } } else  { r <<= p1; return r; }
    }
  };

  s_long _t_ind; // const , fixed pos (this + 0 bytes)
  mutable s_long _nxf; // bits 0.._xsh-1 are flags area; bits _xsh..31 are exceptions count (0 - none, >0 - some, any <0 - too many)
  const type_descriptor* _ptd; // const, fixed pos (this + 8 bytes)
  s_long _n; // the number of existing elements
  s_long _last_j; // _column index of the last existing element; _last_j < _nj except case of _pj == 0, _nj == 0, _last_j == 0
  s_long _size_k; // the number of elements in _last_j column; [1..m::_cap_j(_last_j)]; except case of _n == 0, _last_j == 0, _size_k == 0
  _column* _pj; // columns
  s_long _nj; // number of elements (allocated columns) in _pj; 0 on _pj == 0; >= 1 on _pj != 0
  s_long _nbase; // client-side index of the first element (for pval, insert, remove)

    // _nxf structure. (_xsh lower bits are flags, all higher are nexc.)
  static const s_long _xsh = 8, _xd = 1 << _xsh, _fm = _xd - 1, _xm = ~_fm;

  inline s_long _nxf_default() const throw() { return 0x1; }
  inline bool _f_can_shrink() const throw() { return bool(_nxf & 0x1); }
  inline bool _f_unsafe() const throw() { return bool(_nxf & 0x2); }

  inline void _setf_can_shrink(bool x) throw() { x ? _nxf |= 0x1 : _nxf &= ~s_long(0x1); }
  inline void _setf_unsafe(bool x) const throw() { x ? _nxf |= 0x2 : _nxf &= ~s_long(0x2); }

    // Area in _nxf for user flags. _uf_n flags are available, starting from bit _uf_sh in _nxf.
    //    _uf_m is the mask extracting all bits.
    //  NOTE vecm_clear and all vecm constructors set all user flags to 0.
  static const s_long _uf_n = 4; // number of user flags
  static const s_long _uf_sh = _xsh - _uf_n; static const s_long _uf_1 = 1 << _uf_sh; static const s_long _uf_m = ((1 << _uf_n) - 1) << _uf_sh;

    // Access to user flags. i = [0.._uf_n-1].
    //  NOTE vecm_clear and all vecm constructors set all flags to 0.
  inline bool _uf(s_long i) const throw() { return bool(_nxf & _uf_m & (_uf_1 << i)); }
  inline void _set_uf(s_long i, bool x) const throw() { x ? _nxf |= (_uf_m & (_uf_1 << i)) : _nxf &= ~(_uf_m & (_uf_1 << i)); }

    // Adds a delta to bc, xc or nxf, according to _nxf structure.
    //  (bc is treated as l-shifted by _xsh, delta as normal integer. Least _xsh bits of bc are kept.)
    //  If delta is too large or negative, sets bc to negative value.
  static inline void _nx_add(s_long& bc, s_long delta) throw() { if (bc < 0 || delta == 0) { return; } if (delta >= 0 && delta <= (meta::s_long_mp >> _xsh) - 10) { bc += (delta << _xsh); if (bc >= 0) { return; } } bc = (bc & _fm) | ((1 << (1 + bytes::ilog2_t<meta::s_long_mp>::f)) & _xm); }

    // Assigns a number (ec) to bc, xc or nxf, according to _nxf structure.
    //  (bc is treated as l-shifted by _xsh, delta as normal integer. Least _xsh bits of bc are kept.)
  static inline void _nx_set(s_long& bc, s_long ec) throw() { if (ec == 0) { bc &= _fm; return; } if (ec >= 0 && ec <= (meta::s_long_mp >> _xsh) - 10) { bc = (bc & _fm) | (ec << _xsh); return; } bc = (bc & _fm) | ((1 << (1 + bytes::ilog2_t<meta::s_long_mp>::f)) & _xm); }

  static inline s_long __minmb() throw() { return 769; }

    // Allocates the first column. May be called only on _pj == 0. Sets _pj and _nj according to the results of allocation (!0 and 1, or 0 and 0). Returns the new _pj.
  _column* _alloc_j0() throw() { _column* pj = reinterpret_cast<_column*>(_ptd->_pxmalloc_r(sizeof(_column))); if (pj) { if (pj->init_alloc(_ptd, _ptd->t_size * m::c1)) { _pj = pj; _nj = 1; return pj; } _ptd->_pxmfree_r(pj); } _pj = 0; _nj = 0; return 0; }

    // Ensures allocation of the column _last_j+1. Returns a pointer to its first (kbase+0) element. May increase _nj. Returns 0 if any allocation failed.
  template<class T> T* _alloc_lj1() throw() { s_long j2 = _last_j + 1; if (j2 >= _nj) { if (j2 > _nj) { return 0; } _column* pj2 = reinterpret_cast<_column*>(_ptd->_pxmrealloc_r(_pj, (_nj + 1) * sizeof(_column))); if (!pj2) { return 0; } _pj = pj2; T* p = reinterpret_cast<T*>(_pj[_nj].init_alloc(_ptd, m::_cap_j(_nj) * sizeof(T))); if (!p) { return 0; } _nj += 1; return p; } else { return _pj[j2].at_0u<T>(); } }

    // Local copy constructor. Assumes that *this is not initialized.
    //  Returns:
    //    1 copying fully succeeded. vecm(const vecm&) case (a).
    //    0 element copying is disallowed. nbase() only copied. vecm(const vecm&) case (b).
    //    -1 copying has been completed unsuccessfully. vecm(const vecm&) case (c).
    //    -2 copying failed. x is invalid, unsafe or incompatible. Elem. type set to s_long. vecm(const vecm&) case (d).
    //    -3 this == 0. The call is ignored. vecm(const vecm&) case (e).
  s_long _l_cc(const vecm& x) throw()
  {
    if (!(this)) { return -3; }
    _n = 0; _last_j = 0; _size_k = 0; _pj = 0; _nj = 0; _nxf = _nxf_default();
    _ptd = 0; if (x.integrity() >= 0) { _ptd = _cmd_reg::get_compat(x._ptd, true, true); } if (!_ptd) { _ptd = &_config_ff::_td_inst<s_long>::ref(); _t_ind = _ptd->t_ind; _nbase = 0; _nxf = (_nxf & _fm) | _xd; return -2; }
    _t_ind = _ptd->t_ind; _nbase = x._nbase; _alloc_j0();

    if (x._n <= 0) { return 1; } if (_ptd->cmode == 0) { return 0; } if (!_pj) { _nxf = (_nxf & _fm) | _xd; return -1; }
    if ( ! _reserve_incr_u(x._n)) { _nxf = (_nxf & _fm) | _xd; return -1; }
    s_long j; for (j = 0; j <= x._last_j; ++j) { s_long cap = m::_cap_j(j); s_long ec = _ptd->_pelscp(_pj + j, x._pj + j, 0, j < x._last_j ? cap : x._size_k, cap); if (ec) { _nx_add(_nxf, ec); break; } }
    if (_nxf & _xm)
    {
      s_long j2 = j;
      if (_ptd->dmode)
      {
        for (j = 0; j < j2; ++j)
        {
          _column& c = _pj[j]; s_long cap = m::_cap_j(j); s_long k2 = j < x._last_j ? cap : x._size_k;
          s_long k1 = cap - c.kbase; s_long k = 0; char* p = c.atc_0u(_ptd->t_size);
          while (k < k2) { if (_ptd->p_destroy_1(p) < 0) { if (_nxf >= 0) { _nxf += _xd; } } p += _ptd->t_size; ++k; if (k == k1) { p = c.pd; } }
        }
      }
      for (j = 1; j < _nj; ++j) { _pj[j].clear(_ptd); }
      _column* pj2 = reinterpret_cast<_column*>(_ptd->_pxmrealloc_r(_pj, sizeof(_column))); if (pj2) { _pj = pj2; } // pj2 == 0 on shrinking is not critical
      _nj = 1; return -1;
    }
    _n = x._n; _last_j = x._last_j; _size_k = x._size_k; return 1;
  }

    // Local destructor.
    //    Clears the container and marks it invalid by settting _t_ind = 0.
  inline void _l_destroy() const throw()
  {
    if (!(this && _t_ind)) { return; }
    _column* pj = _pj;
    if (pj)
    {
      const_cast<vecm*>(this)->_pj = 0;
      for (s_long j = 0; j < _nj; ++j) { if (j <= _last_j) { s_long cap = m::_cap_j(j); s_long ec = _ptd->_pelsdc(pj + j, _f_unsafe(), 0, j == _last_j ? _size_k : cap, cap); _nx_add(_nxf, ec); } pj[j].clear(_ptd); }
      _ptd->_pxmfree_r(pj);
    }
    const_cast<vecm*>(this)->_t_ind = 0;
  }

    // Local vecm_copy impl.
  inline s_long _l_copy(const vecm* px, bool is_tr) throw()
  {
    if (!(this && _t_ind)) { return -3; } if (this == px) { return 1; } typedef vecm Q; enum { _nq = sizeof(Q) };
    if (is_tr) { union { meta::s_ll _st; char _stc[_nq]; }; Q* p = reinterpret_cast<Q*>(&_st); p->_l_cc(*px); if (p->_nxf >> _xsh) { p->_l_destroy(); _nx_add(_nxf, p->_nxf >> _xsh); return 0; } _nxf &= _fm; _l_destroy(); s_long ec = _nxf >> _xsh; bytes::memmove_t<Q>::F(this, p, _nq); _nx_add(_nxf, ec); return 1; }
      else { _nxf &= _fm; _l_destroy(); s_long ec = _nxf >> _xsh; s_long res = _l_cc(*px); _nx_add(_nxf, ec); return res >= 0 ? 1 : (res == -1 ? -1 : -2); }
  }

    // Local vecm_clear impl.
  inline s_long _l_clear() throw()
  {
    if (!(this && _t_ind)) { return -1; }
    if (_pj)
    {
      if (_ptd->dmode) { for (s_long j = 0; j < _nj; ++j) { if (j <= _last_j) { s_long cap = m::_cap_j(j); s_long ec = _ptd->_pelsdc(_pj + j, _f_unsafe(), 0, j == _last_j ? _size_k : cap, cap); _nx_add(_nxf, ec); } if (j >= 1) { _pj[j].clear(_ptd); } } }
        else { for (s_long j = 1; j < _nj; ++j) { _pj[j].clear(_ptd); } }
      _column* pj2 = reinterpret_cast<_column*>(_ptd->_pxmrealloc_r(_pj, sizeof(_column))); if (pj2) { _pj = pj2; }
    }
    _n = 0; _last_j = 0; _size_k = 0; _nj = _pj ? 1 : 0; s_long ec1 = _nxf >> _xsh; _nxf = _nxf_default(); return ec1;
  }

    // Static info + initialization.
  static inline s_long* _ls_psig() throw()
  {
    static s_long x[11] = { 0 };
    if (x[0]) { return x; }

    struct { s_long x(void* po1, void* pm1, void* po2, void* pm2) { s_long n1 = s_long(reinterpret_cast<char*>(pm1) - reinterpret_cast<char*>(po1)); s_long n2 = s_long(reinterpret_cast<char*>(pm2) - reinterpret_cast<char*>(po2)); enum { m = 255 * 255 - 1 }; if (n1 > m) { n1 = m; } if (n2 > m) { n2 = m; } return (n1 % 255 + 1) << 24 | (n1 / 255 + 1) << 16 | (n2 % 255 + 1) << 8 | (n2 / 255 + 1); } } _sigv;
    vecm* pv(0); _column* pc(0); type_descriptor* pd(0); char* p0(0);
    s_long i = 0;
    x[++i] = _sigv.x(pd, pd + 1, pv, pv + 1);
    x[++i] = _sigv.x(pd, &pd->is_ptr, pv, &pv->_n);
    x[++i] = _sigv.x(pv, &pv->_pj, pv, &pv->_nj);
    x[++i] = _sigv.x(pc, pc + 1, pc, &pc->kbase);
    x[++i] = _sigv.x(p0, p0+bytes::_dr, p0, p0+m::p1);
    x[++i] = _sigv.x(p0, p0+m::p2, p0, p0+m::p3);
    x[++i] = _sigv.x(p0, p0+m::m1, p0, p0+m::m2);
    x[++i] = _sigv.x(p0, p0+_xsh, p0, p0 + _uf_n * 10 + 2); // 2 is the number of vecm internal flags
    x[++i] = 101; // psig[9]: header version, may be used to allow partial compatibility
    x[++i] = 0;
    x[0] = _sigv.x(pd, &pd->pvecm_clear, pv, &pv->_ptd); // for non-local objects with same first dword of digest as locals, pvecm_* calls will be correct

    bytes::type_index_t<char>();
    bytes::type_index_t<s_long>();
    bytes::type_index_t<double>();
    bytes::type_index_t<meta::s_ll>();

    return x;
  }

    // Local static functions for whole container.
  static inline void _ls_destroy(const vecm* p) throw() { p->_l_destroy(); }
  static inline s_long _ls_delete(const vecm* p) throw() { try { delete p; return 1; } catch (...) { return 0; } }
  static inline s_long _ls_copy(vecm* pdest, const vecm* psrc, s_long is_tr) throw() { return pdest->_l_copy(psrc, is_tr); }
  static inline s_long _ls_clear(vecm* p) throw() { return p->_l_clear(); }

    // Cross-module descriptors registry.
  struct _cmd_reg
  {
      // Looks for ind_t (type index), then for ind_ta (type equiv. index).
      //  Returns:
      //    2 - found ind_t. *ret_p == valid static descr. for type index.
      //    1 - not found ind_t, but found ind_ta. *ret_p == valid static descr. for type equiv. index.
      //  NOTE If is_ta_first == true, ind_ta search occurs before ind_t search.
      //      This is suitable for copy construction only, because finds
      //      more specialized / less compatible configuration first.
      //    0 - found nothing, or both inds. are not cross-module (>= 0). *ret_p == 0.
    static s_long find_cm(s_long ind_t, s_long ind_ta, bool is_ta_first, const type_descriptor** ret_p = 0) throw()
    {
      bool b0 = ind_t < 0; bool b1 = ind_ta < 0;
      if (b0 || b1)
      {
        lock_t_reg __lock; if (!__lock.is_lckinit()) { return 0; }
        if (b1 && is_ta_first && __lock.pv[1]) { _cmd_k* p2(0); __lock.pv[1]->_ord_find<_cmd_k, s_long, _less_td_ind<true> >(ind_ta, 0, &p2, _less_td_ind<true>()); if (p2) { if (ret_p) { *ret_p = *p2; } return 1; } }
        if (b0 && __lock.pv[0]) { _cmd_k* p2(0); __lock.pv[0]->_ord_find<_cmd_k, s_long, _less_td_ind<false> >(ind_t, 0, &p2, _less_td_ind<false>()); if (p2) { if (ret_p) { *ret_p = *p2; } return 2; } }
        if (b1 && !is_ta_first && __lock.pv[1]) { _cmd_k* p2(0); __lock.pv[1]->_ord_find<_cmd_k, s_long, _less_td_ind<true> >(ind_ta, 0, &p2, _less_td_ind<true>()); if (p2) { if (ret_p) { *ret_p = *p2; } return 1; } }
      }
      if (ret_p) { *ret_p = 0; } return 0;
    }

      // Search for type descriptor able to copy data from vecm with p.
      //  Returns (in order of preference):
      //      a) p if p is from this module.
      //      b) on allow_ta == true, if found: cross-module descriptor compatible with p->ta_ind.
      //      c) on allow_t == true, if found: cross-module descriptor compatible with p->t_ind.
      //      d) 0 if no match.
    static inline const type_descriptor* get_compat(const type_descriptor* p, bool allow_t, bool allow_ta) throw()
    {
      if (!p) { return 0; } if (p->psig == _ls_psig()) { return p; }
      if (!bytes::is_eq_str<s_long>(p->psig, _ls_psig(), -1, -1)) { return 0; }
      const type_descriptor* p2(0); find_cm(allow_t ? p->t_ind : 0, allow_ta ? p->ta_ind : 0, true, &p2); return p2;
    }

      // Registers the type descriptor of TA, if it is declared cross-module (has negative t_ind and/or ta_ind).
    template<class TA> static void reg_cm() throw()
    {
      _cmd_k p = static_cast<_cmd_k>(&_config_ff::_td_inst<TA>::ref());
      bool b0 = p->t_ind < 0; bool b1 = p->ta_ind < 0;
      if (b0 || b1)
      {
        lock_t_reg __lock; if (!__lock.is_lckinit()) { return; }
        if (b0 && __lock.pv[0]) { __lock.pv[0]->_ord_insert<_cmd_k, s_long, _less_td_ind<false> >(p->t_ind, p, 0, 0, _less_td_ind<false>()); }
        if (b1 && __lock.pv[1]) { __lock.pv[1]->_ord_insert<_cmd_k, s_long, _less_td_ind<true> >(p->ta_ind, p, 0, 0, _less_td_ind<true>()); }
      }
    }

  private:
    struct _dx : type_descriptor {}; typedef const _dx* _cmd_k;
    struct _data { short* init; vecm** pv; meta::s_ll* d; _data(short* p1, vecm** p2, meta::s_ll* p3) throw() : init(p1), pv(p2), d(p3) {} };
    static inline _data _reg() throw() { enum { _nst = 1 + sizeof(vecm) / sizeof(meta::s_ll) }; static vecm* p[4] = { 0, 0, 0, 0 }; static meta::s_ll v[2 * _nst]; static short _init_state(0); return _data(&_init_state, p, v); }
    struct lock_t_reg : _data
    {
      const bool __b; const bytes::lock_t<vecm> __lock;
      inline bool is_lckinit() const throw() { return __lock.is_lckinit(); }
      lock_t_reg() throw() : _data(_reg()), __b(*init == 1 || _init0()), __lock()
      {
        enum { _nst = 1 + sizeof(vecm) / sizeof(meta::s_ll) };
        if (*init == 0)
        {
          *init = 1;
          vecm* a = reinterpret_cast<vecm*>(d); new (a) vecm(_config_ff::_td_inst<_cmd_k>::ref, 0);
          vecm* b = reinterpret_cast<vecm*>(d + _nst); new (b) vecm(_config_ff::_td_inst<_cmd_k>::ref, 0);
          pv[2] = a; pv[3] = b; pv[0] = a; pv[1] = b;
        }
      }
    };
    struct _detach { inline ~_detach() throw() { _data r = _reg(); if (*r.init == 1) { bytes::lock_t<vecm> __lock; if (!__lock.is_lckinit()) {} r.pv[0] = 0; r.pv[1] = 0; } } };
    struct _deinit { inline ~_deinit() throw() { _data r = _reg(); if (r.pv[2]) { r.pv[2]->~vecm(); } if (r.pv[3]) { r.pv[3]->~vecm(); } } };
    static inline s_long _init0() throw() { static _deinit x1; static _detach x2; return s_long(reinterpret_cast<char*>(&x2) - reinterpret_cast<char*>(&x1)); }
  };

    // Comparisons for ordering type descriptors by their t_ind and ta_ind.
  template<bool cmp_ta, class _ = meta::nothing> struct _less_td_ind { bool less21 (s_long t_ind1, const type_descriptor* p2) const throw() { return t_ind1 < p2->t_ind; } bool less12 (const type_descriptor* p1, s_long t_ind2) const throw() { return p1->t_ind < t_ind2; } };
  template<class _> struct _less_td_ind<true, _> { bool less21 (s_long ta_ind1, const type_descriptor* p2) const throw() { return ta_ind1 < p2->ta_ind; } bool less12 (const type_descriptor* p1, s_long ta_ind2) const throw() { return p1->ta_ind < ta_ind2; } };

    // Part of el_reserve_n, unchecked op.
    //    Increase the storage reserve to at least n2 elements.
    //    Must have _pj != 0 and n2 >= 0.
    //    If the new reserve is not larger than the current, nothing is done.
    //  Returns true on on_calm_result, false of memory allocation failure.
  inline bool _reserve_incr_u(s_long n2) throw()
  {
    // NOTE the function body is also present in el_append_m
    s_long nj2; s_long nk2; m::_ind_jk(n2, nj2, nk2); if (nk2 > 0) { ++nj2; } if (nj2 <= _nj) { return true; }

    _column* px = reinterpret_cast<_column*>(_ptd->_pxmalloc_r((nj2 - _nj) * sizeof(_column))); if (!px) { return false; }
    s_long jf = -1;
    for (s_long j = _nj; j < nj2; ++j)
    {
      s_long size = _ptd->t_size * m::_cap_j(j); _column& cj = px[j - _nj]; if (!cj.init_alloc(_ptd, size)) { jf = j; break; }
      for (s_long i = 0; i < size; i += __minmb()) { cj.pd[i] = 0; }
    }
    if (jf >= 0) { for (s_long j = _nj; j < jf; ++j) { px[j - _nj].clear(_ptd); } _ptd->_pxmfree_r(px); return false; }

    _column* pj2 = reinterpret_cast<_column*>(_ptd->_pxmrealloc_r(_pj, nj2 * sizeof(_column))); if (!pj2) { for (s_long j = _nj; j < nj2; ++j) { px[j - _nj].clear(_ptd); } _ptd->_pxmfree_r(px); return false; }
    _pj = pj2; for (s_long j = _nj; j < nj2; ++j) { _pj[j].init_move_from(px[j - _nj]); } _ptd->_pxmfree_r(px);
    _nj = nj2; return true;
  }

    // Part of el_reserve_n, unchecked op.
    //    Decrease the storage reserve to nj2 columns.
    //    Must have _pj != 0 and nj2 >= 1.
    //    Free any extra memory on reserve shrinking.
    //    If nj2 >= _nj, nothing is done.
  inline void _reserve_decr_nj_u(s_long nj2) throw()
  {
    if (nj2 >= _nj) { return; }
    if (nj2 <= 0) { nj2 = 1; if (nj2 == _nj) { return; } } for (s_long j = nj2; j < _nj; ++j) { _pj[j].clear(_ptd); }
    _column* pj2 = reinterpret_cast<_column*>(_ptd->_pxmrealloc_r(_pj, nj2 * sizeof(_column))); if (pj2) { _pj = pj2; } // pj2 == 0 on shrinking is not critical
    _nj = nj2;
  }

  inline void _try_shrink() throw() // req. _pj != 0
  {
    if (!_f_can_shrink()) { return; }
    s_long nj2 = _last_j + 1; if (nj2 >= _nj) { return; } for (s_long j = nj2; j < _nj; ++j) { _pj[j].clear(_ptd); }
    _column* pj2 = reinterpret_cast<_column*>(_ptd->_pxmrealloc_r(_pj, nj2 * sizeof(_column))); if (pj2) { _pj = pj2; } // pj2 == 0 on shrinking is not critical
    _nj = nj2;
  }

    // Unconditionally sets new _n, _last_j, _size_k. Structures must be correctly adjusted.
    //  NOTE n < 0 is treated as n == 0.
  inline void _set_size(s_long n) { if (n <= 0) { _last_j = 0; _size_k = 0; _n = 0; }  else { s_long j, k; m::_ind_jk(n - 1, j, k); _last_j = j; _size_k = k + 1; _n = n; } }



    // Configuration-dependent copying single object.
    //    May be faultless or not (see config_t enum value gc).
    //  On on_calm_result, *pdest is a copy of src.
    //  On failure, xc is increased, *pdest is initialized according to the configured recovery g_jmethod.
    // When construction functor is passed as src, it is used to create the object.
    //    In this case F() behaves as in non-guaranteed copying mode.
  template<class TF, bool initDestOnFailure, bool _usefunctor = meta::resolve_TF<TF, meta::tag_construct>::use_functor, bool _useassign = bytes::config0_t<typename meta::resolve_TF<TF, meta::tag_construct>::t>::useAssignToCopy>
  struct _copy_1u_t // normal copy of the source object
  {
    typedef TF T;
    static inline void F(const type_descriptor* _ptd, T* pdest, const T& src, s_long& xc) throw()
    {
      if (_ptd->p_copy_1(pdest, &src) >= 0) { return; }
      if (xc >= 0) { xc += _xd; } if (initDestOnFailure) { _ptd->_pntrecov(pdest); }
    }
  };
  template<class TF, bool initDestOnFailure> struct _copy_1u_t<TF, initDestOnFailure, false, true> // copying by assignment
  {
    typedef TF T;
    static inline void F(const type_descriptor* _ptd, T* pdest, const T& src, s_long& xc) throw()
    {
      try { if (_ptd->op_flags & 0x20) { *pdest = src; return; } } catch (...) {}
      if (_ptd->p_copy_1(pdest, &src) >= 0) { return; }
      if (xc >= 0) { xc += _xd; } if (initDestOnFailure) { _ptd->_pntrecov(pdest); }
    }
  };
  template<class Fc, bool initDestOnFailure, bool b> struct _copy_1u_t<Fc, initDestOnFailure, true, b> // creating the destination by functor src
  {
    typedef typename Fc::t T;
    static inline void F(const type_descriptor* _ptd, T* pdest, const Fc& src, s_long& xc) throw()
    {
      try { src.f(pdest); } catch (...) { if (xc >= 0) { xc += _xd; } if (initDestOnFailure) { _ptd->_pntrecov(pdest); } }
    }
  };

    // Configuration-dependent moving single object.
    //    May be guaranteed (see config_t enum value gm) or non-guaranteed.
    //    Assumes that the destination is not initialized.
    //    If moving was by copying and it's succeeded, psrc is destroyed as configured.
    //    On destructor failure, bc is increased.
    // If non-guaranteed moving fails, xc is increased. Also,
    //    forceMoveOnFailure false: *pdest is left uninitialized, *psrc is not changed.
    //    forceMoveOnFailure true: *pdest is initialized according to the configured recovery g_jmethod,
    //    *psrc is destroyed (if destruction is enabled).
    // See also: _remove_1u_t for bc, xc.
  template<class T, bool forceMoveOnFailure, bool _useassign = bytes::config0_t<T>::useAssignToCopy>
  struct _move_1u_t
  {
    static inline void F(const type_descriptor* _ptd, T* pdest, T* psrc, s_long& bc, s_long& xc) throw()
    {
      s_long res = _ptd->p_move_1(pdest, psrc);
      if (res >= 1) { return; } if (res == 0) { if (bc >= 0) { bc += _xd; } return; }
      if (xc >= 0) { xc += _xd; } if (forceMoveOnFailure) { _ptd->_pntrecov(pdest); if (_ptd->p_destroy_1(psrc) < 0) { if (bc >= 0) { bc += _xd; } } }
    }
  };
  template<class T, bool forceMoveOnFailure>
  struct _move_1u_t<T, forceMoveOnFailure, true>
  {
    static inline void F(const type_descriptor* _ptd, T* pdest, T* psrc, s_long& bc, s_long& xc) throw()
    {
      try { if (_ptd->op_flags & 0x10) { *pdest = *psrc; return; } } catch (...) {}
      s_long res = _ptd->p_move_1(pdest, psrc);
      if (res >= 1) { return; } if (res == 0) { if (bc >= 0) { bc += _xd; } return; }
      if (xc >= 0) { xc += _xd; } if (forceMoveOnFailure) { _ptd->_pntrecov(pdest); if (_ptd->p_destroy_1(psrc) < 0) { if (bc >= 0) { bc += _xd; } } }
    }
  };

    // Non-transactional (+ dest. recovery on copy failure).
    // Move n contiguous elements, starting at psrc, to pdest.
    //  pdest must point to uninitialized space for n elements (or less if src. and dest. ranges are overlapping).
    //  Can move forward, backward, with overlapping.
    //  In case of moving by copying, after copying, source element destructors are called as necessary.
    //  n <= 0 does nothing.
    // bc, xc - see _remove_1u_t.
    // NOTE In 32-bit system, max. size of moved memory block n*sizeof(T) is 2^31-1.
  template<class T>
  void _move_nu_t(T* pdest, T* psrc, s_long n, s_long& bc, s_long& xc) throw()
  {
    _yk_reg meta::t_pdiff n1 = n;
    if (_ptd->mmode == 4) { n1 *= sizeof(T); bytes::memmove_t<T>::F(pdest, psrc, n1); return; }
    _yk_reg T* p2 = pdest; _yk_reg T* p1 = psrc; if (n1 <= 1) { if (n1 == 1) { if (p2 != p1) { _move_1u_t<T, true>::F(_ptd, p2, p1, bc, xc); } } return; }
    if (p2 < p1) { do { _move_1u_t<T, true>::F(_ptd, p2, p1, bc, xc); ++p2; ++p1; } while (--n1); }
     else if (p2 > p1) { p2 += n1; --p2; p1 += n1; --p1; do { _move_1u_t<T, true>::F(_ptd, p2, p1, bc, xc); --p2; --p1; } while (--n1); }
  }

    // Non-transactional (+ dest. recovery on copy failure).
    // Inserts 1 element into the column pc at the given position (k).
    //  pc may be empty, partially or fully filled with elements. In the latter case,
    //  the element is inserted into pc_next at pos. 0 (on k == cap),
    //  or (on k < cap) to the pos. of last elem., which itself is moved to pc_next pos. 0.
    // Parameters:
    //  pc - target column.
    //  pc_next - next column. Must be specified only if size == cap (otherwise the value is ignored).
    //    pc_next must not contain an element at pos. 0.
    //  k - target pos., must be [0..size].
    //  size is the number of existing elements in pc. size may be [0..cap].
    //  cap is the capacity of pc.
    //  bc, xc - see _remove_1u_t.
  template<class TF>
  void _insert_1u_t(_column* pc, _column* pc_next, const TF& src, s_long k, s_long size, s_long cap, s_long& bc, s_long& xc) throw()
  {
    typedef typename meta::resolve_TF<TF, meta::tag_construct>::t T;
    if (size >= cap)
    {
      if (k >= cap) { _copy_1u_t<TF, true>::F(_ptd, pc_next->at_0u<T>(), src, xc); return; }
      _move_1u_t<T, true>::F(_ptd, pc_next->at_0u<T>(), pc->at_9u<T>(cap), bc, xc);
      size = cap-1;
      if (k >= size) { k = size; }
    }
    if (k <= 0) { _copy_1u_t<TF, true>::F(_ptd, pc->at_9u<T>(cap), src, xc); pc->kbase_decr(cap); return; }
    if (k >= size) { _copy_1u_t<TF, true>::F(_ptd, pc->at_ku<T>(size, cap), src, xc); return; }
    s_long nR = size-k; s_long nL = k;
    T* p0; T* p1; s_long kbase; pc->get_pd_kbase(p0, kbase);
    if (nL <= nR)
    {
      if (kbase > 0)
      {
        p1 = p0+kbase; s_long n = cap-kbase; if (n > nL) { n = nL; } _move_nu_t<T>(p1-1, p1, n, bc, xc); nL -= n;
        if (nL > 0) { _move_1u_t<T, true>::F(_ptd, p0+cap-1, p0, bc, xc); nL -= 1; }
        if (nL > 0) { _move_nu_t<T>(p0, p0+1, nL, bc, xc); }
      }
      else
      {
        _move_1u_t<T, true>::F(_ptd, p0+cap-1, p0, bc, xc); nL -= 1;
        if (nL > 0) { _move_nu_t<T>(p0, p0+1, nL, bc, xc); }
      }
      --k; if (k < 0) { k = cap - 1; }
      _copy_1u_t<TF, true>::F(_ptd, pc->at_ku<T>(k, cap), src, xc);
      pc->kbase_decr(cap);
    }
    else // nR < nL, nR may be 0
    {
      T* px = pc->at_ku<T>(k, cap);
      if (nR > 0)
      {
        s_long n = kbase+size-cap;
        if (n >= 0)
        {
          if (n > 0) { p1 = p0+n; if (n > nR) { n = nR; } p1 -= n; _move_nu_t<T>(p1+1, p1, n, bc, xc); nR -= n; }
          if (nR > 0) { _move_1u_t<T, true>::F(_ptd, p0, p0+cap-1, bc, xc); nR -= 1; }
        }
        if (nR > 0) { _move_nu_t<T>(px+1, px, nR, bc, xc); }
      }
      _copy_1u_t<TF, true>::F(_ptd, px, src, xc);
    }
  }

    // Non-transactional (+ dest. recovery on copy failure).
    // Removes 1 element from a column. Moves others as little as possible + sets kbase of the column
    //  such that the last previously occupied position (namely, size-1) becomes empty.
    // Result: the column contains size-1 elements.
    // Parameters:
    //  NOTE _remove_1u_t assumes that pos. k is not initialized.
    //  pc - target column.
    //  k - target pos., must be [0..size].
    //  size is the number of existing elements in pc. size must be [0..cap].
    //  cap is the capacity of pc.
    //  bc - ("bad") non-critical failure count. It is increased if a destructor of old
    //    or temp. object fails. The container does not suffer in this case.
    //  xc - ("exceptional") critical failure count. It is increased, for example, on exception in T(const T&).
    //    Element copy fails, but operation continues till the end
    //    regardless of such faults. _f_unsafe() is set. vecm integrity() becomes < 0.
    //    Some elements may be left uninitialized, 0-filled, dflt.-initialized according to the configured recovery g_jmethod.
    //    NOTE In transactional mode, _remove_1u_t and other like functions for copying / moving elements,
    //    are not called at all (_tr_* family is called instead) or called only for types
    //    with faultless copying / moving.
  template<class T>
  void _remove_1u_t(_column* pc, s_long k, s_long size, s_long cap, s_long& bc, s_long& xc) throw()
  {
    s_long nL = size-k-1; s_long nR = k;
    s_long kbase; T* p0; pc->get_pd_kbase(p0, kbase);
    s_long pos = kbase+k; s_long n;
    if (nL <= nR)
    {
      if (nL > 0)
      {
        if (pos >= cap) { pos -= cap; _move_nu_t<T>(p0+pos, p0+pos+1, nL, bc, xc); }
        else
        {
          n = cap - 1 - pos; if (n > nL) { n = nL; } _move_nu_t<T>(p0+pos, p0+pos+1, n, bc, xc); nL -= n;
          if (nL > 0) { _move_1u_t<T, true>::F(_ptd, p0+cap-1, p0, bc, xc); --nL; }
          if (nL > 0) { _move_nu_t<T>(p0, p0+1, nL, bc, xc); }
        }
      }
    }
    else // nR < nL
    {
      if (nR > 0)
      {
        if (pos < cap) { _move_nu_t<T>(p0+kbase+1, p0+kbase, nR, bc, xc); }
        else
        {
          pos -= cap; s_long n = pos; if (n > nR) { n = nR; } _move_nu_t<T>(p0+1, p0, n, bc, xc); nR -= n;
          if (nR > 0) { _move_1u_t<T, true>::F(_ptd, p0, p0+cap-1, bc, xc); --nR; }
          if (nR > 0) { _move_nu_t<T>(p0+kbase+1, p0+kbase, nR, bc, xc); }
        }
      }
      pc->kbase_incr(cap);
    }
  }

    // BU, BU0 must be sizeof(T) chars.
  template<class T>
  inline bool _tr_move_1only_t(T* pdest, T* psrc, char* BU) throw()
  {
    bytes::memmove_t<T>::F(BU, pdest, sizeof(T));
    s_long res = _ptd->p_move_1(pdest, psrc); if (res >= 1) { return true; }
    if (res < 0) { bytes::memmove_t<T>::F(pdest, BU, sizeof(T)); return false; }
    if (_nxf >= 0) { _nxf += _xd; } return true;
  }
  template<class T>
  inline bool _tr_destroy_prev_1only_t(T* pdest, char* BU, char* BU0) throw()
  {
    if (!_ptd->dmode) { return true; }
    bytes::memmove_t<T>::F(BU0, pdest, sizeof(T));
    bytes::memmove_t<T>::F(pdest, BU, sizeof(T));
    bool be = _ptd->p_destroy_1(pdest) < 0;
    bytes::memmove_t<T>::F(pdest, BU0, sizeof(T));
    if (be) { if (_nxf >= 0) { _nxf += _xd; } } return !be;
  }

    // Transactional, for this obj. only.
    // Copying *psrc to *pdest on transactional moving.
    //  Both *psrc and *pdest are considered as valid objects.
    // ibu is the number of elements currenly occupying BU. BU must be at least sizeof(T)*(ibu+1) long.
    // 1. *pdest is backed up. 2. T(src) is invoked on pdest.
    //  No destructor call for pdest and psrc is made.
    // Returns:
    //    true on on_calm_result. ibu is increased by 1.
    //    false if T(const T&) failed. BU, ibu and vecm data remain unchanged.
  template<class TF>
  inline bool _tr_copy_1_t(typename meta::resolve_TF<TF, meta::tag_construct>::t* pdest, const TF* psrc, char* BU, s_long& ibu) throw()
  {
    typedef typename meta::resolve_TF<TF, meta::tag_construct>::t T;
    bytes::memmove_t<T>::F(BU + meta::t_pdiff(ibu) * sizeof(T), pdest, sizeof(T));
    s_long xc(0); _copy_1u_t<TF, false>::F(_ptd, pdest, *psrc, xc);
    if (xc) { bytes::memmove_t<T>::F(pdest, BU + meta::t_pdiff(ibu) * sizeof(T), sizeof(T)); return false; }
      else { ++ibu; return true; }
  }

    // Transactional, for this obj. only.
    // Copying *psrc to *pdest on transactional moving.
    //  *psrc is considered as valid object, *pdest is considered uninitialized.
    // ibu is the number of elements currenly occupying BU. BU must be at least sizeof(T)*(ibu+1) long.
    // 1. T(src) is invoked on pdest. 2. If step 1 succeeds, src is backed up.
    //  No destructor call for pdest and psrc is made.
    // Returns:
    //    true on on_calm_result. ibu is increased by 1.
    //    false if T(const T&) failed. BU, ibu and vecm data remain unchanged.
  template<class T>
  inline bool _tr_copy_1_busrc_t(T* pdest, const T* psrc, char* BU, s_long& ibu) throw()
  {
    s_long xc(0); _copy_1u_t<T, false>::F(_ptd, pdest, *psrc, xc);
    if (xc) { return false; }
      else { bytes::memmove_t<T>::F(BU + meta::t_pdiff(ibu) * sizeof(T), psrc, sizeof(T)); ++ibu; return true; }
  }

    // Transactional, for this obj. only.
  template<class T>
  inline char* _tr_bu_alloc_t(s_long n_elems) throw() { try { return reinterpret_cast<char*>(_ptd->pma2(meta::t_pdiff(n_elems) * sizeof(T))); } catch (...) { return 0; }  }

    // Transactional, for this obj. only.
  inline void _tr_bu_free(char* p) throw() { try { _ptd->pmf2(p); } catch (...) {} }

    // Transactional, for this obj. only.
    //    1. Current dest contents are temporarily moved to BU0. BU0 must contain at least sizeof(T) chars.
    //    2. ibu'th element moved to pdest and its destructor is executed.
    //    3. dest contents are restored from BU0.
    //  Returns:
    //    a) true if destructor succeeded.
    //    b) false if destructor failed. nexc() is increased by 1.
    //  NOTE in contrary to _tr_copy_1_t, ibu is not modified.
  template<class T>
  inline bool _tr_destroy_prev_t(T* pdest, char* BU, s_long ibu, char* BU0) throw()
  {
    if (!_ptd->dmode) { return true; }
    bytes::memmove_t<T>::F(BU0, pdest, sizeof(T));
    bytes::memmove_t<T>::F(pdest, BU + meta::t_pdiff(ibu) * sizeof(T), sizeof(T));
    bool be = _ptd->p_destroy_1(pdest) < 0;
    bytes::memmove_t<T>::F(pdest, BU0, sizeof(T));
    if (be) { if (_nxf >= 0) { _nxf += _xd; } } return !be;
  }

    // Transactional, for this obj. only.
    //    1. Assuming *pdest is valid object, calls T's destructor on pdest.
    //    2. Increments nexc() if destructor has failed (the function anyway continues).
    //    3. ibu'th element bytes are moved from backup to pdest.
    //  NOTE in contrary to _tr_copy_1_t, ibu is not modified.
  template<class T>
  inline void _tr_revert_t(T* pdest, char* BU, s_long ibu) throw()
  {
    if (_ptd->dmode) {  if (_ptd->p_destroy_1(pdest) < 0) { if (_nxf >= 0) { _nxf += _xd; } }  }
    bytes::memmove_t<T>::F(pdest, BU + meta::t_pdiff(ibu) * sizeof(T), sizeof(T));
  }



    // 1: the element has been found (i == [0..n()-1], pk != 0).
    // 0: the element is not found (i == [0..n()] -- place of insertion, pk == 0).
    // -2: an exception occured (i == -1, pk == 0).
    //  NOTE Default less may be:
    //    bytes::less_t<K, K2>()
  template<class K, class K2, class C>
  inline s_long _ord_find(const K2& k, s_long* r_pi, K** r_ppk, const C& less) const throw()
  {
    try
    {
      _column* pj = _pj; if (_n <= 0 || !_pj) { if (r_pi) { *r_pi = 0; } if (r_ppk) { *r_ppk = 0; } return 0; }
      s_long r_j = 0; s_long r_k = 0; K* r_p = 0;
      if (true)
      {
        s_long xmid = 0; s_long xl = 0; s_long xr = _last_j + 1;
        while(xr > xl)
        {
          xmid = (xl + xr) / 2; K& kx = *pj[xmid].at_0u<K>();
          if (less.less21(k, kx)) { xr = xmid; }
            else { if (less.less12(kx, k)) { if (xr - xmid <= 1) { r_j = xmid; pj += r_j; goto lFoundX1; } xl = xmid; } else {  r_j = xmid; pj += r_j; r_p = &kx; goto lFoundEnd; } }
        }
        r_j = xmid; pj += r_j;
        goto lFoundEnd;
      lFoundX1:
        if (true)
        {
          xl = 1; s_long cap = m::_cap_j(r_j); xr = r_j == _last_j ? _size_k : cap;
          while(xr > xl)
          {
            xmid = (xl + xr) / 2; K& kx = *pj->at_ku<K>(xmid, cap);
            if (less.less21(k, kx)) { xr = xmid; }
              else { if (less.less12(kx, k)) { if (xr - xmid <= 1) { xl = xr; break; } xl = xmid; } else { r_k = xmid; r_p = &kx; goto lFoundEnd; } }
          }
          r_k = xl;
        }
      }
  lFoundEnd:
      if (r_pi) { *r_pi = r_k + m::_nrsv(r_j); } if (r_ppk) { *r_ppk = r_p; }
      return r_p ? 1 : 0;
    }
    catch (...) { if (r_pi) { *r_pi = -1; } if (r_ppk) { *r_ppk = 0; } return -2; }
  }

    // 1: the element has been inserted (i == [0..n()-1], pk != 0).
    // 0: the element already exists (i == [0..n()-1], pk != 0).
    // -1: failed to insert (i == -1, pk == 0).
    // -2: error during search (i == -1, pk == 0). The container is not changed.
    //  NOTE Default less may be:
    //    bytes::less_t<typename meta::resolve_TF<KF, meta::tag_construct>::t, K2>()
    //    bytes::less_t<K, K2>()
  template<class KF, class K2, class C>
  inline s_long _ord_insert(const K2& k, const KF& k_ins, s_long* r_pi, typename meta::resolve_TF<KF, meta::tag_construct>::t** r_ppk, const C& less) throw()
  {
    typedef typename meta::resolve_TF<KF, meta::tag_construct>::t K; s_long i; K* pk;
    s_long res = _ord_find<K, K2, C>(k, &i, &pk, less);
    switch (res)
    {
      case 1: { res = 0; break; }
      case 0: { if (el_insert_1(i + _nbase, k_ins) > 0) { res = 1; pk =  pval_0u<K>(i); } else { res = -1; } break; }
      default: { break; }
    }
    if (r_pi) { *r_pi = i; } if (r_ppk) { *r_ppk = pk; }
    return res;
  }

    // 1: the element has been removed.
    // 0: the element did not exist.
    // -1: failed to remove.
    // -2: error during search. The container is not changed.
    //  NOTE Default less may be:
    //    bytes::less_t<K, K2>()
  template<class K, class K2, class C>
  inline s_long _ord_remove(const K2& k, const C& less) throw()
  {
    s_long i;
    s_long res = _ord_find<K, K2, C>(k, &i, 0, less);
    if (res == 1) { res = el_remove_1<K>(i + _nbase, false) > 0 ? 1 : -1; }
    return res;
  }


  struct _config_ff
  {
    template<class cfg, class Aux = meta::nothing> struct _static { static void* F(s_long id) throw() { return 0; } };

      // Returning user-defined static type information (arbitrary). May be specialized.
      //    On entry, parent is already initialized, and will be accessible to the external client via typer<TA>
      //      when ptd2_t::F returns.
      //    is_locked is true if lock_t<vecm> is implemented and successfully set.
      //  The returned value is assigned to parent.ptd2.
      //  NOTE typer<TA>() may not be called from ptd2_t::F, to avoid reentering.
    template<class TA, class Aux = meta::nothing> struct ptd2_t { static void* F(const type_descriptor& parent, bool is_locked) { return 0; } };

    template<class TA> struct _td_inst
    {
      typedef typename spec<TA>::config t_config; typedef typename t_config::t_value t_value; typedef bytes::type_index_t<t_value> t_si;
      static const type_descriptor& ref() throw()
      {
        static type_descriptor* __ptd(0);
        if (__ptd) { return *__ptd; }
        static type_descriptor __td;

        __td.psig = _ls_psig();
        __td.ptd2 = 0;
        __td._pstatic = _static<t_config>::F;
        __td.pvecm_destroy = _ls_destroy;
        __td.pvecm_delete = _ls_delete;
        __td.pvecm_copy = _ls_copy;
        __td.pvecm_clear = _ls_clear;
        __td._pr1 = 0;

        __td.p_copy_1 = copier_t<t_config>::Fcopy_1;
        __td.p_move_1 = mover_t<t_config>::Fmove_1;
        __td.p_destroy_1 = destroyer_t<t_config>::Fdestroy_1;
        __td.pma1 =  t_config::t_alloc1::Falloc;
        __td.pmf1 = t_config::t_alloc1::Ffree;
        __td.pma2 =  t_config::t_alloc2::Falloc;
        __td.pmf2 = t_config::t_alloc2::Ffree;
        __td._pxmalloc_r =  t_config::t_alloc2::xalloc_r;
        __td._pxmfree_r =  t_config::t_alloc2::xfree_r;
        __td._pxmrealloc_r =  t_config::t_alloc2::xrealloc_r;

        __td.t_ind = t_si::ind();
        __td.ta_ind = bytes::type_index_t<TA>::ind();
        __td.t_size = sizeof(t_value);
        __td.dmode = t_config::dmode;
        __td.mmode = t_config::mmode;
        __td.cmode = t_config::cmode;
        __td.gc = t_config::gc;
        __td.gm = t_config::gm;
        __td.op_flags = 0;
          if (t_config::transactional)  { __td.op_flags |= 0x1; }
          if (t_config::transactional && t_config::gm)  { __td.op_flags |= 0x2; }
          if (t_config::transactional && !t_config::gm)  { __td.op_flags |= 0x4; }
          if (t_config::transactional && t_config::gm && !t_config::gc)  { __td.op_flags |= 0x8; }
          if (t_config::uam) { __td.op_flags |= 0x10; }
          if (t_config::uac) { __td.op_flags |= 0x20; }
        __td.nt_allowDfltConstruct = t_config::nt_allowDfltConstruct;
        __td.nt_allowNullify = t_config::nt_allowNullify;
        __td.u_ignoreNullsOnCleanup = t_config::u_ignoreNullsOnCleanup;
        __td.u_ignoreDestructorsOnCleanup = t_config::u_ignoreDestructorsOnCleanup;
        __td.is_ptr = meta::is_ptr<t_value>::result;

        __td._pelscp = _elems_copy<t_config>::F;
        __td._pelsdc = _elems_destroy_clr<t_config>::F;
        __td.pcd_o = constructor_t<t_config, (bytes::config_cd_t<TA>::enable || (t_config::nt_allowDfltConstruct && !t_config::transactional))>::Fconstruct_1;
        __td._pntrecov = _ntrecover_t<t_config>::F;

        if (true)
        {
          bytes::lock_t<vecm> __lock;
          try { __td.ptd2 = ptd2_t<TA>::F(__td, __lock.is_locked()); } catch (...) {}
        }

        __ptd = &__td;
        return __td;
      }
    };


      // Destroying the selected range of elements in a column. ONLY for calling from vecm_clear and ~vecm().
      // Returns 0 if all done successfully. >=1 indicates that one or more destructors have failed.
    template<class cfg, bool useDestructor = (cfg::dmode > 0)> struct _elems_destroy_clr { static inline s_long F(_column* pc, s_long is_unsafe, s_long k0, s_long k2, s_long cap) throw() { return 0; } };
    template<class cfg> struct _elems_destroy_clr<cfg, true>
    {
      static s_long F(_column* pc, s_long is_obj_unsafe, s_long k0, s_long k2, s_long cap) throw()
      {
        typedef typename cfg::t_value __t_v;
        if (is_obj_unsafe && cfg::u_ignoreDestructorsOnCleanup) { return 0; }
        s_long ec(0); s_long k1 = cap - pc->kbase; s_long k = k0; __t_v* p = pc->at_ku<__t_v>(k, cap);
        if (is_obj_unsafe && cfg::u_ignoreNullsOnCleanup)
          { while (k < k2) { if (bytes::mem_is_null_t<__t_v>(p) != 1) { if (destroyer_t<cfg>::Fdestroy_1(p) < 0) { ec += 1; } } ++p; ++k; if (k == k1) { p = pc->at_pd0u<__t_v>(); } } }
          else { while (k < k2) { if (destroyer_t<cfg>::Fdestroy_1(p) < 0) { ec += 1; } ++p; ++k; if (k == k1) { p = pc->at_pd0u<__t_v>(); } } }
          return ec;
      }
    };

      // Transactinal copying elements to the same positions in the new column.
      //    Destination column req.: 1) same capacity as source, 2) destination storages are not initialized.
      //  Returns 0 if all done successfully. -1 if copying is disallowed (cmode == 0).
      //    In case of error, all new elements in cdest are destroyed, and the number of errors is returned
      //    (>=1, namely 1 copy constructor and >= 0 destructors).
    template<class cfg, int cmode = cfg::cmode> struct _elems_copy
    {
      static s_long F(_column* pcdest, _column* pcsrc, s_long k0, s_long k2, s_long cap) throw()
      {
        typedef typename cfg::t_value __t_v; s_long ec(0);
        s_long k12 = cap - pcdest->kbase; s_long k11 = cap - pcsrc->kbase; s_long k = k0; __t_v* p2 = pcdest->at_ku<__t_v>(k, cap); __t_v* p1 = pcsrc->at_ku<__t_v>(k, cap);
        if (k < k2) { do { if (copier_t<cfg>::Fcopy_1(p2, p1) < 0) { ec += 1; break; } ++k; if (k >= k2) { break; } ++p2; ++p1; if (k == k12) { p2 = pcdest->at_pd0u<__t_v>(); } if (k == k11) { p1 = pcsrc->at_pd0u<__t_v>(); } } while (true); }
        if (ec) { k2 = k; k = k0; p2 = pcdest->at_ku<__t_v>(k, cap); while (k < k2) { if (destroyer_t<cfg>::Fdestroy_1(p2) < 0) { ec += 1; } ++p2; ++k; if (k == k12) { p2 = pcdest->at_pd0u<__t_v>(); } } }
        return ec;
      }
    };
    template<class cfg> struct _elems_copy<cfg, 0>
    {
      static s_long F(_column* pcdest, _column* pcsrc, s_long k0, s_long k2, s_long cap) throw() { return -1; }
    };
    template<class cfg> struct _elems_copy<cfg, 4>
    {
      static s_long F(_column* pcdest, _column* pcsrc, s_long k0, s_long k2, s_long cap) throw()
      {
        typedef typename cfg::t_value __t_v;
        s_long k12 = cap - pcdest->kbase; s_long k11 = cap - pcsrc->kbase; s_long k = k0; __t_v* p2 = pcdest->at_ku<__t_v>(k, cap); __t_v* p1 = pcsrc->at_ku<__t_v>(k, cap);
        s_long q1, q2; if (k11 <= k12) { q1 = k11; q2 = k12; } else { q1 = k12; q2 = k11; } if (q1 > k2) { q1 = k2; } if (q2 > k2) { q2 = k2; }
        if (k < q1) { s_long n = q1 - k; bytes::memmove_t<__t_v>::F(p2, p1, n * sizeof(__t_v)); p2 += n; p1 += n; k += n; if (k == k12) { p2 = pcdest->at_pd0u<__t_v>(); } if (k == k11) { p1 = pcsrc->at_pd0u<__t_v>(); } }
        if (k < q2) { s_long n = q2 - k; bytes::memmove_t<__t_v>::F(p2, p1, n * sizeof(__t_v)); p2 += n; p1 += n; k += n; if (k == k12) { p2 = pcdest->at_pd0u<__t_v>(); } if (k == k11) { p1 = pcsrc->at_pd0u<__t_v>(); } }
        if (k < k2) { s_long n = k2 - k; bytes::memmove_t<__t_v>::F(p2, p1, n * sizeof(__t_v)); }
        return 0;
      }
    };

      // Recovery from unsuccessful construction in non-transactional mode.
      // Returns:
      //  0: one of the allowed methods of recovery succeeded.
      //  -1: no recovery allowed by configuration or all methods failed.
      //  other >0: treat same as 0.
      //  other <-1: treat same as -1.
    template<class cfg, bool __allow = !cfg::transactional && (cfg::nt_allowDfltConstruct || cfg::nt_allowNullify)> struct _ntrecover_t
    {
      typedef typename cfg::t_value t_value;
      static inline s_long F(void* p) throw() { return -1; }
    };
    template<class cfg> struct _ntrecover_t<cfg, true>
    {
      typedef typename cfg::t_value t_value;
      static inline s_long F(void* p) throw()
      {
        if (constructor_t<cfg, (cfg::nt_allowDfltConstruct && !cfg::transactional)>::Fconstruct_1(reinterpret_cast<t_value*>(p)) == 0) { return 0; }
        if (cfg::nt_allowNullify) { bytes::mem_set_null_t<t_value>(reinterpret_cast<t_value*>(p)); return 0; }
        return -1;
      }
    };

    template<class cfg> struct _tr_t
    {
      typedef typename cfg::t_value t_value;
      static inline bool Fmove_1only(t_value* pdest, t_value* psrc, char* BU, s_long& bc) throw() { bytes::memmove_t<t_value>::F(BU, pdest, sizeof(t_value)); s_long res = mover_t<cfg>::Fmove_1(pdest, psrc); if (res >= 1) { return true; } if (res < 0) { bytes::memmove_t<t_value>::F(pdest, BU, sizeof(t_value)); return false; } if (bc >= 0) { bc += _xd; } return true; }
      static inline bool Fdestroy_prev_1only(t_value* pdest, char* BU, char* BU0, s_long& bc) throw() { if (!cfg::dmode) { return true; } bytes::memmove_t<t_value>::F(BU0, pdest, sizeof(t_value)); bytes::memmove_t<t_value>::F(pdest, BU, sizeof(t_value)); bool be = destroyer_t<cfg>::Fdestroy_1(pdest) < 0; bytes::memmove_t<t_value>::F(pdest, BU0, sizeof(t_value)); if (be) { if (bc >= 0) { bc += _xd; } } return !be; }
      static inline bool Fcopy_1only(t_value* pdest, const t_value* psrc, char* BU) throw() { bytes::memmove_t<t_value>::F(BU, pdest, sizeof(t_value)); if (copier_t<cfg>::Fcopy_1(pdest, psrc) >= 0) { return true; } bytes::memmove_t<t_value>::F(pdest, BU, sizeof(t_value)); return false; }
    };




      // CONSTRUCTOR. Returns:
      //  0: value construction succeeded. *p is initialized.
      //  -1: value construction failed. *p remains uninitialized.
      //  other >0: treat same as 0.
      //  other <-1: treat same as -1.
    template<class cfg, bool enable> struct constructor_t { static inline s_long Fconstruct_1(void* p) throw() { return -1; } };
    template<class cfg> struct constructor_t<cfg, true> { typedef typename cfg::ta_meta_construct __ta; static inline s_long Fconstruct_1(void* p) throw() { try { typedef typename cfg::t_value __t_v; meta::construct_f<__ta>().f(reinterpret_cast<__t_v*>(p)); } catch (...) { return 0; } return -1; } };


      // DESTROYER. Returns:
      //  0: destructor succeeded or was not needed. *p must be considered uninitialized.
      //  -1: destructor is executed, but failed. *p must be considered uninitialized.
      //  other >0: treat same as 0.
      //  other <-1: treat same as -1.
    template<class cfg, int dmode = cfg::dmode> struct destroyer_t { static inline s_long Fdestroy_1(void* p) throw() { return 0; } };
    template<class cfg> struct destroyer_t<cfg, 1> { static inline s_long Fdestroy_1(void* p) throw() { typedef typename cfg::t_value __t_v;                                                                                    try { reinterpret_cast<__t_v*>(p)->~__t_v(); return 0; } catch (...) { return -1; } } };
    template<class cfg> struct destroyer_t<cfg, 2> { static inline s_long Fdestroy_1(void* p) throw() { typedef typename cfg::t_value __t_v;                                                                                    try { delete *reinterpret_cast<__t_v*>(p); return 0; } catch (...) { return -1; } } };
    template<class cfg> struct destroyer_t<cfg, 3> { static inline s_long Fdestroy_1(void* p) throw() { typedef typename cfg::t_value __t_v; typedef typename cfg::ta_meta_destroy __ta; try { meta::destroy_t<__ta>::F(reinterpret_cast<__t_v*>(p)); return 0; } catch (...) { return -1; } } };

      // COPIER. Returns:
      //  0: copying succeeded. *pdest is initialized.
      //  -1: copying failed, *pdest left uninitialized.
      //  other >0: treat same as 0.
      //  other <-1: treat same as -1.
    template<class cfg, int cmode = cfg::cmode, bool useAssignToCopy = cfg::uac> struct copier_t { };
    template<class cfg> struct copier_t<cfg, 0, false> { static inline s_long Fcopy_1(void* pdest, const void* psrc) throw() { return -1; } };
    template<class cfg> struct copier_t<cfg, 0, true> { static inline s_long Fcopy_1(void* pdest, const void* psrc) throw() { return -1; } };
    template<class cfg> struct copier_t<cfg, 1, false> { typedef typename cfg::t_value __t_v;                                                                         static inline s_long Fcopy_1(void* pdest, const void* psrc) throw() { try { new (reinterpret_cast<__t_v*>(pdest)) __t_v(*reinterpret_cast<const __t_v*>(psrc)); return 0; } catch (...) { return -1; } } };
    template<class cfg> struct copier_t<cfg, 2, false> { typedef typename cfg::t_value __t_v; typedef typename cfg::ta_meta_copy __ta;     static inline s_long Fcopy_1(void* pdest, const void* psrc) throw() { try { meta::copy_t<__ta>::F(reinterpret_cast<__t_v*>(pdest), reinterpret_cast<const __t_v*>(psrc)); return 0; } catch (...) { return -1; } } };
    template<class cfg> struct copier_t<cfg, 3, false> { typedef typename cfg::t_value __t_v; typedef typename cfg::ta_meta_safecopy __ta;   static inline s_long Fcopy_1(void* pdest, const void* psrc) throw() { meta::safecopy_t<__ta>::F(reinterpret_cast<__t_v*>(pdest), reinterpret_cast<const __t_v*>(psrc)); return 0; } };
    template<class cfg> struct copier_t<cfg, 4, false> { typedef typename cfg::t_value __t_v;                                                                                     static inline s_long Fcopy_1(void* pdest, const void* psrc) throw() { bytes::memmove_t<__t_v>::F(pdest, psrc, sizeof(__t_v)); return 0; } };
    template<class cfg, int cmode> struct copier_t<cfg, cmode, true> { typedef typename cfg::t_value __t_v;                                                                           static inline s_long Fcopy_1(void* pdest, const void* psrc) throw() { try { *reinterpret_cast<__t_v*>(pdest) = *reinterpret_cast<const __t_v*>(psrc); return 0; } catch (...) { return copier_t<cfg, cmode, false>::Fcopy_1(pdest, psrc); } } };

      // MOVER. Returns:
      //  1: moving succeeded. *pdest is valid, *psrc is destroyed as configured.
      //  0: moving succeeded. *pdest is valid, *psrc destrustor called, but failed (the client must regard *psrc destroyed).
      //  -1: moving failed. *pdest left uninitialized, *psrc is not changed.
      //  other >1: treat same as 1.
      //  other <-1: treat same as -1.
    template<class cfg, int mmode = cfg::mmode, bool useAssignToMove = cfg::uam, int dmode = cfg::dmode> struct mover_t { };
    template<class cfg, int dmode> struct mover_t<cfg, 0, false, dmode> { static inline s_long Fmove_1(void* pdest, const void* psrc) throw() { return -1; } };
    template<class cfg, int dmode> struct mover_t<cfg, 0, true, dmode> { static inline s_long Fmove_1(void* pdest, const void* psrc) throw() { return -1; } };
    template<class cfg> struct mover_t<cfg, 1, false, 0> { typedef typename cfg::t_value __t_v;                                                                                                               static inline s_long Fmove_1(void* pdest, void* psrc) throw() { if (copier_t<cfg>::Fcopy_1(pdest, psrc) >= 0) { return 1; } return -1; } };
    template<class cfg> struct mover_t<cfg, 1, false, 1> { typedef typename cfg::t_value __t_v;                                                                                                                 static inline s_long Fmove_1(void* pdest, void* psrc) throw() { if (copier_t<cfg>::Fcopy_1(pdest, psrc) >= 0) { if (destroyer_t<cfg>::Fdestroy_1(psrc) >= 0) { return 1; } return 0; } return -1; } };
    template<class cfg> struct mover_t<cfg, 1, false, 2> { typedef typename cfg::t_value __t_v;                                                                                                                 static inline s_long Fmove_1(void* pdest, void* psrc) throw() { if (copier_t<cfg>::Fcopy_1(pdest, psrc) >= 0) { if (destroyer_t<cfg>::Fdestroy_1(psrc) >= 0) { return 1; } return 0; } return -1; } };
    template<class cfg, int dmode> struct mover_t<cfg, 2, false, dmode> { typedef typename cfg::t_value __t_v; typedef typename cfg::ta_meta_trymove __ta;  static inline s_long Fmove_1(void* pdest, void* psrc) throw() { return meta::trymove_t<__ta>::F(reinterpret_cast<__t_v*>(pdest), reinterpret_cast<__t_v*>(psrc)); } };
    template<class cfg, int dmode> struct mover_t<cfg, 3, false, dmode> { typedef typename cfg::t_value __t_v; typedef typename cfg::ta_meta_safemove __ta;  static inline s_long Fmove_1(void* pdest, void* psrc) throw() { meta::safemove_t<__ta>::F(reinterpret_cast<__t_v*>(pdest), reinterpret_cast<__t_v*>(psrc)); return 1; } };
    template<class cfg, int dmode> struct mover_t<cfg, 4, false, dmode> { typedef typename cfg::t_value __t_v;                                                                                static inline s_long Fmove_1(void* pdest, void* psrc) throw() { bytes::memmove_t<__t_v>::F(pdest, psrc, sizeof(__t_v)); return 1; } };
    template<class cfg, int mmode, int dmode> struct mover_t<cfg, mmode, true, dmode> { typedef typename cfg::t_value __t_v;                                                                                 static inline s_long Fmove_1(void* pdest, void* psrc) throw() { try { *reinterpret_cast<__t_v*>(pdest) = *reinterpret_cast<const __t_v*>(psrc); return 1; } catch (...) { return mover_t<cfg, mmode, false, dmode>::Fmove_1(pdest, psrc); } } };
  };

public:

// vecm config_t
//==section 3==============================================

    // NOTE config_t must not be specialized, only template instantiated,
    //    possibly with customized Aux.
    //
    // destructorMode 0 destructorMode 1 destructorMode 2 destructorMode 3
    // moverMode 0 moverMode 1 moverMode 2 moverMode 3 moverMode 4
    // copierMode 0 copierMode 1 copierMode 2 copierMode 3 copierMode 4
    //
    // destructorMode:
    //    0 - object does not need destruction.
    //    1 - execute destructor to make an object uninitialized.
    //    2 - execute operator delete on the referenced object. T must be pointer type. Using moverMode 4 is recommended.
    //    3 - use meta::destroy_t (must be specialized for this case).
    //
    // moverMode:
    //    4 - use memmove_t. Exceptioless moving by byte-level copying. Only for objects that do not depend on memory address change.
    //    3 - use meta::safemove_t specialization, supplied by the client. It performs exceptioless moving + manipulations
    //          with object memory address, if necessary. In this mode, it's guaranteed that, as vecm element,
    //          only one object occupies the same memory location at a time. (Container modifications do not back up objects.)
    //    2 - use meta::trymove_t (must be specialized for this case). Moving is not guaranteed, even if trymove_t is internally faultless.
    //        NOTE moverMode 2 may be useful in non-transactional mode as a replacement for pair of calls to copier and destructor.
    //          Nonetheless, in transactional mode, copier of copierMode + destructor of destructorMode will be called everywhere
    //          where >= 2 elements may be moved. (The transaction must first successfully copy all objects that should be moved,
    //          and then only destroy all unnecessary objects.)
    //          The only remarkable exception is transactionsl el_remove_1 with move_last == true,
    //          which in moverMode 2 really uses meta::trymove_t for moving the last element.
    //        In transactional mode with moverMode 1 and 2, 2 client objects may temporarily occupy the same address
    //          during single modification operation (due to backing up). The order of destruction may be different in each particular case.
    //    1 - try copying (copier_t) + source destructor (if needed).
    //        In transactional mode with moverMode 1 and 2, 2 client objects may temporarily occupy the same address
    //          during single modification operation (due to backing up). The order of destruction may be different in each particular case.
    //    0 - moving is disabled. Some operations on elements in vecm and possibly in dependent classes will return an error code at run time.
    //
    // copierMode:
    //    4 - use memmove_t. Guaranteed copying. Only for objects that do not depend on memory address and can be correctly copied byte-by-byte.
    //    3 - use meta::safecopy_t (must be specialized for this case). Guaranteed copying. May be useful if some processing must be done before/after copying.
    //    2 - use meta::copy_t (must be specialized for this case). This is a custom constructor. May be useful if T(const T&) is unavailable or undesirable. Copying is not guaranteed.
    //    1 - use T(const T&). Copying is not guaranteed.
    //    0 - copying is disabled. Some operations on elements in vecm and possibly in dependent classes will return an error code at run time.
    //
    //  NOTE the only setting requiring the accessible copy constructor for T, is copierMode == 1.
    //    In all other cases, it may be undefined or inaccessible.
    //    Still, copierMode == 1 is the default for all objects except built-in trivial types,
    //    unless vecm::spec is somehow specialized.
    //
  template<class T> struct config_aux;
  template<class T, int destructorMode, int moverMode, int copierMode, class Aux = config_aux<T> >
  struct config_t
  {
    typedef T t_value;
    typedef config_t t_config;
    enum
    {
      dmode = destructorMode, mmode = moverMode, cmode = copierMode,
      gc = cmode >= 3, gm = mmode >= 3 || (mmode == 1 && gc),
      transactional = Aux::transactional,
      nt_allowDfltConstruct = Aux::nt_allowDfltConstruct, nt_allowNullify = Aux::nt_allowNullify, u_ignoreNullsOnCleanup = Aux::u_ignoreNullsOnCleanup, u_ignoreDestructorsOnCleanup = Aux::u_ignoreDestructorsOnCleanup,
      uam = bool(bytes::config0_t<t_value>::useAssignToCopy) && dmode == 0 && (mmode == 4 || (mmode == 1 && (cmode == 1 || cmode == 4))),
      uac = bool(bytes::config0_t<t_value>::useAssignToCopy) && (cmode == 4 || (dmode == 0 && cmode == 1))
    };
    typedef  typename Aux::t_alloc1 t_alloc1;
    typedef  typename Aux::t_alloc2 t_alloc2;
    typedef  typename Aux::ta_meta_copy ta_meta_copy;
    typedef  typename Aux::ta_meta_safecopy ta_meta_safecopy;
    typedef  typename Aux::ta_meta_trymove ta_meta_trymove;
    typedef  typename Aux::ta_meta_safemove ta_meta_safemove;
    typedef  typename Aux::ta_meta_destroy ta_meta_destroy;
    typedef  typename Aux::ta_meta_construct ta_meta_construct;

    enum { __check2 = meta::assert<(dmode >= 0 && dmode <= 3)>::result };
    enum { __check3 = meta::assert<(mmode >= 0 && mmode <= 4)>::result };
    enum { __check4 = meta::assert<(cmode >= 0 && cmode <= 4)>::result };
    enum { __check5 = meta::assert<!(dmode == 2 && !meta::is_ptr<T>::result)>::result }; // for destructor by op. delete, T must be pointer
    typedef typename meta::type_equi<ta_meta_copy, T>::t_3 __check10;
    typedef typename meta::type_equi<ta_meta_safecopy, T>::t_3 __check11;
    typedef typename meta::type_equi<ta_meta_trymove, T>::t_3 __check12;
    typedef typename meta::type_equi<ta_meta_safemove, T>::t_3 __check13;
    typedef typename meta::type_equi<ta_meta_destroy, T>::t_3 __check14;
    typedef typename meta::type_equi<ta_meta_construct, T>::t_3 __check15;
  };

    // Recommended use of config_aux is inheriting from it + passing the subclass
    //  as parameter to config_t.
  template<class T>
  struct config_aux
  {
      // config_aux transactional
      // transactional == true.
      //    A modifying operation on elements sequence normally results in only one of the two cases:
      //      a) operation has been completed in full.
      //      b) operation has failed on copying/moving an object.
      //          All partial modifications of the elements sequence are cancelled.
      //          (To ensure this for types whose objects cannot be safely moved,
      //          the modification functions temporarily allocate some memory
      //          and back up objects that will be overwritten.)
      //    NOTE In the conditions beyond normal, the above results are not guaranteed. For example:
      //        - the container object is damaged (memory contains the incorrect data),
      //        - a client-side or automatic copy or move proc., called by the modifying operation,
      //            writes incorrect data or generates system-level exceptions.
      //        - the modifying operation is interrupted externally (system-level exception,
      //            hardware or programmatic interrupt w/o return, thread or initForCallback termination etc.).
      //        - copying or destroying composite object generates overlapping exceptions, e.g.
      //            copy constructor generates an exception, and an already existing part generates
      //            another exception in its destructor.
      //            (Currently, one of similar cases is handled: hashx is resistant
      //            to pair of exceptions in both entry key and value destructors.)
      //    NOTE (!) When elements are referred from outside or from each other by pointer,
      //      transactional == true requires additional tuning.
      //      Memory addresses of the elements change when some other elements are inserted or removed
      //      from the sequence.
      //      So, the client must specify config_t moverMode 3 and supply the client-side specialization
      //      for meta::safemove_t<T>. It should faultlessly move the object data + update any references.
      //      Otherwise, sequence modification behavior may be not as expected:
      //        a) moverMode 0 disables sequence modification at all. Only when elements
      //            are appended or removed from the end of sequence, a modification function will succeed.
      //        b) moverMode 1 and 2 creates backups of the existing elements
      //            for possible transaction rollback, because it does not guarantee moving.
      //            So, two objects may temporarily (logically) reside at the same memory address.
      //            This is not acceptable when each T instance globally registers itself somewhere,
      //            or when pointer value is used by outside objects to distinguish the referred objects.
      //            (For example, vecm may keep list-like structure with nodes, referring each other by pointer,
      //            only in moverMode 3, because it allows for faultless moving and relinking each particular element.)
      //        c) moverMode 4 moves memory and does nothing with any outward objects,
      //            so the outward pointers into the elements sequence become generally invalid.
      //      The above may be taken into account to achieve max. efficiency in special cases.
      //  transactional == false.
      //    All modifying operations on vecm are performed in forced manner,
      //        with no extra time and memory expenses on backup.
      //    a) For types with faultless moving and construction the result is same as transactional == true.
      //    b) For types with moving/construction that may fail, successful operation completion yields same
      //        result as in transactional == true.
      //    c) For types with moving/construction that may fail, unsuccessful operation
      //        may leave one or more objects in undefined state, or initialized to some known value.
      //        This is controlled by nt_allowDfltConstruct and nt_allowNullify flags (see below).
      //        Case (c) is acceptable if and only if the client is aware that the object storage
      //        may be partially initialized after T(const T&) failure, or initialized by one of nt_* methods.
      //  The results of modifications in all modes are reflected by vecm integrity() property.
    enum { transactional = true };

      // (Used only in non-transactional mode.)
      // nt_allowDfltConstruct = true:
      //    Allow T() to initialize the destination element after unsuccessful moving or copying.
      //       Factually, meta::construct_f<ta_meta_construct>().f() is called.
      //        (By dflt., ta_meta_construct == T, and construct_f<T>::f(T* p) executes T() at p.)
      //        construct_f::f() may fail as well. In this case, recovery may try another g_jmethod.
      // nt_allowDfltConstruct = false:
      //    construct_f::f() is not called. Recovery proceeds to the next g_jmethod.
      //  NOTE nt_allowDfltConstruct false disables possible indirect call of T()
      //      from the dflt. construct_f<ta_meta_construct>.
      //      This is one of the ways to successfully compile the program when T() is not accessible.
    enum { nt_allowDfltConstruct = true };

      // (Used only in non-transactional mode.)
      // nt_allowNullify = true:
      //  After unsuccessful moving or copying, allow filling with zero bytes the destination element.
      //    Along with u_ignoreNullsOnCleanup == true, this allows vecm_clear(), ~vecm()
      //      to succeed without risk of generating both C++ and system-level exceptions.
      //  NOTE the client may check for integrity() < 0 after each operation, and if this occurs,
      //    initialize null objects in some way, or call vecm_clear() or destroy vecm object.
      //    The client may use null_state(ind) or bytes::mem_is_null_t<T>(T*)
      //    to check if the object is 0-filled.
      //  NOTE On nt_allowDfltConstruct == true, 0-filling g_jmethod is tried after T() has failed.
    enum { nt_allowNullify = true };

      // (Used only by ~vecm and vecm_clear() on integrity() < 0.)
      //  If the flag is true, vecm_clear() and ~vecm() won't execute destructors
      //    on elements whose bytes are all 0. See also nt_allowNullify.
      //  NOTE This flag/operation is ignored on normal cleanup (when integrity() >= 0).
      //  NOTE This flag/operation is ignored if u_ignoreDestructorsOnCleanup == true.
    enum { u_ignoreNullsOnCleanup = true };

      // (Used only by ~vecm and vecm_clear() on integrity() < 0.)
      // If the flag is true, vecm_clear() and ~vecm() won't execute destructors on any elements at all.
      //  NOTE This flag/operation is ignored on normal cleanup (when integrity() >= 0).
      //  NOTE The client only is responsible for deinitialization of both normal and damaged elements.
      //  NOTE This flag suppresses u_ignoreNullsOnCleanup.
    enum { u_ignoreDestructorsOnCleanup = false };

      // Procedures for allocating/freeing dynamic memory for this object.
      //    The memory from t_alloc1 is expected to be correctly aligned to keep arrays of data of type T.
      //    The memory from t_alloc2 is expected to be min. 8-byte aligned to keep C++ built-in types and structures.
    typedef bytes::allocdef_t<meta::nothing> t_alloc1;
    typedef bytes::xalloc2_t<meta::nothing> t_alloc2;

      // Type argument for custom constructors, movers, destructors.
      //    Any ta_* definition is in use if and only if one of the relevant settings
      //    in config_t or config_aux chooses to reference it.
      //  (For example, config_t moverMode 3 will always call meta::safemove_t<ta_meta_safemove>::F
      //    to move an object, moverMode 2 will always call meta::trymove_t<ta_meta_trymove>::F etc.
      //    See also config_t.)
      //  NOTE The type assigned must be T or any of its equivalents (see vecm(ptyper p, s_long base) for descr.).
      //    It may differ from typer argument passed to vecm().
    typedef T ta_meta_copy;
    typedef T ta_meta_safecopy;
    typedef T ta_meta_trymove;
    typedef T ta_meta_safemove;
    typedef T ta_meta_destroy;
    typedef T ta_meta_construct;
  };


    // Type specification.
    //    May be specialized to set up behavior (construction, moving, critical error handling, memory allocation etc.),
    //    of values of the type, both as sequence elements and as independent objects.
    //  NOTE To access the type configuration anywhere in the program,
    //    using shorter vecm::specf<t> instead of vecm::spec<t>::config is recommended.
  template<class T> struct spec { enum { __k = bytes::config0_t<T>::isKnown1 }; typedef config_t<T, (__k ? 0 : 1), (__k ? 4 : 1), (__k ? 4 : 1)> config; };

    // Actual configuration and specialized routines (moving, copying, destruction)
    //    for values of type T.
    //    TA may specify either T or its equivalent (for alt. configuration).
    // NOTE specf must not be specialized, only instantiated.
  template<class TA>  struct specf :
    spec<TA>::config,
    _config_ff::destroyer_t<typename spec<TA>::config>,
    _config_ff::mover_t<typename spec<TA>::config>,
    _config_ff::copier_t<typename spec<TA>::config>
  {
      typedef TA ta;
      //    static inline s_long Fdestroy_1(const void* p) throw(); // see destroyer_t
      //    static inline s_long Fmove_1(void* pdest, void* psrc) throw(); // see mover_t
      //    static inline s_long Fcopy_1(void* pdest, const void* psrc) throw(); // see copier_t
  private:
      specf(); typedef typename meta::type_equi<TA, typename vecm::specf<TA>::t_value>::t_3 __check;
  };

// vecm n()
//==section 4==============================================

    // The number of elements. [0..meta::s_long_mp].
  inline s_long n() const throw() { return _n; }

    // Index of the first element (no default).
    //  NOTE Any indices supplied by the client are internally made 0-based before any further interpretation:
    //    ind0 = ind - _nbase.
    //    So, integer wrap-arounds do not influence max. capacity of the container
    //    and functions behavior, even if larger indices become negative.
  inline s_long nbase() const throw() { return _nbase; }

    // The number of elements to which the container can grow
    //  without additional dynamic memory allocation for the storage.
    //  nrsv() >= n().
  inline s_long nrsv() const throw() { return m::_nrsv(_nj); }

    // true (default): storage place reserve shrinks to minimal possible value
    //    when elements are removed from the sequence by el_remove*, el_expand*, el_expunge_last.
    // false: the elements storage does not shrink from the existing value in any way,
    //    except explicit calling el_reserve_n or vecm_clear.
  inline bool can_shrink() const throw() { return _f_can_shrink(); }

    // Static information. Transactionality of modifying the elements sequence.
    //    true if all vecm modification (member) operations result in either full completion,
    //      or in cancelling any changes. This is the default configuration for all types.
    //    false if at least one of them may leave vecm element(s) in undefined or corrupted state.
    //    false also if the container is invalid.
    //  NOTE is_transactional reports only the worst case of the container behavior.
    //      If is_transactional() == false, the client code must always check
    //      for all documented operation return values. (Modifiers choose tr./non-tr. algorithm
    //      for target type dynamically, to minimize memory and CPU use.)
    //  NOTE Some operations (el_append*, el_remove_last, el_expand*, el_expunge_last, copy construction)
    //    are always transactional, regardless of type configuration.
  inline bool is_transactional() const throw() { return this && _t_ind ? (_ptd->op_flags & 0x1) || (_ptd->gm && _ptd->gc) : false; }

    // The number of exceptions in vecm element constructors, destructors or their custom versions.
    //      May be increased after any modifying function, even if it returns a on_calm_result code.
    //      If a function accidentally finds an integrity assertion break, nexc() is also increased.
    //  nexc() is internally an integer value that is shorter than s_long.
    //    It may become negative and not change any further. This means "too many errors occured".
    //    To reset nexc() to 0, call vecm_set0_nexc().
    //  NOTE nexc() also becomes 0 after vecm_clear().
  inline s_long nexc() const throw() { return _nxf >> _xsh; }

    // vecm integrity()
    // Safety state of the container.
    //  NOTE integrity(), locality(), compatibility() work correctly independently on each other.
    //  Returns:
    //  1 - normal. nexc() == 0.
    //  0 - normal. All elements are correct. nexc() != 0.
    //      This state indicates that during prev. operations, exceptions occured in element constructors and destructors
    //        on temporary and already removed elements.
    //      Positive nexc() is the exact number of exceptions.
    //      Negative nexc() means too many exceptions.
    //  NOTE Normally, all operations are transactional, i.e. complete in full or cancel any changes.
    //      The container keeps invariant (integrity() >= 0).
    //      See config_aux transactional for details.
    //  -1 - unsafe state. nexc() == anything.
    //      The container modification may be unsafe. Some elements are in undefined state,
    //        or initialized to the value different from expected after successful modification.
    //        This depends on type configuration.
    //      Access to elements (n(), nbase(), pval* etc.) and informational functions are safe,
    //        because the container itself is not damaged.
    //      NOTE In this mode, container destruction follows special rules
    //        (see u_* flags in config_aux).
    //      NOTE The client may set or reset the unsafe flag by calling vecm_setf_unsafe.
    //  -2 - invalid state. this->_t_ind is set to 0:
    //        a) by destructor (~vecm()), because it is already executed;
    //        b) externally to block any operations on the object.
    //      In this state, most of public functions are safe. They ignore the call and return an error code.
    //        The only function is unsafe and must not be called: pval_0u.
    //  -3 - invalid state:
    //      a) this == 0.
    //      b) the container is 100% incompatible.
    //      c) possibly, the container is heavily damaged.
    //      The only functions that may be called:
    //        copy construction, destructor, vecm_copy, vecm_clear, vecm_delete, ptd, null_state -- the call is ignored.
    //        integrity, locality, compatibility, -- the return value is adequate.
    //
    //  NOTE integrity() doesn't reflect compatibility of the container with the current binary module.
    //    See compatibility().
    //  NOTE integrity() doesn't reflect any possible damages of the container itself,
    //    other than filling its storage with zero bytes.
    //  NOTE integrity() doesn't reflect execution of el_expand_* and el_expunge_last functions.
    //    They may create uninitialized positions or free objects storage without calling destructors.
    //    Only the client is responsible for correct processing in this case.
  inline s_long integrity() const throw() { if (!this) { return -3; } if (!_t_ind) { return -2; } if (!_ptd) { return -3; } if (_ptd->psig != _ls_psig() &&  *_ptd->psig != *_ls_psig()) { return -3; } if (_f_unsafe()) { return -1; } return _nxf & _xm ? 0 : 1; }

    // Fast check if this container is created in this module or another module.
    //  NOTE integrity(), locality(), compatibility() work correctly independently on each other.
    //  Returns:
    //    1 - the container is created in this module.
    //    0 - the container is created in another module.
    //    -1 - the container is invalid (integrity() is -2 or -3).
    //  NOTE If non-local object is dynamically created (new), it may be correctly deleted
    //    only by vecm_delete(). vecm_delete() should be preferred to op. delete
    //    when working with both local and non-local objects.
  inline s_long locality() const throw() { if (!(this && _t_ind && _ptd)) { return -1; } return _ptd->psig == _ls_psig() ? 1 : 0; }

    // Check if this container may be called from the current binary module.
    //  NOTE integrity(), locality(), compatibility() work correctly independently on each other.
    //  NOTE This check may be slower than locality() if the object is from another module.
    //    The client may rely on:
    //      locality() == 1 --> compatibility() == 2.
    //      locality() == -1 --> compatibility() == -3.
    //      locality() == 0 --> compatibility() may be [-2..1].
    //  NOTE This check doesn't distinguish integrity() -1, 0, 1 (when returns -1, 0, 1, 2).
    //  Returns:
    //    2 - usable, the container is created in this module.
    //    1 - usable, the container is created in another module. All operations are working.
    //    0 - partially usable. The container (x) is from another module.
    //        Informational and flag functions (n, nbase ... vecm_set* ... is_transactional ... null_state) succeed.
    //        Modifiers that succeed:
    //          vecm(x) -- copying to a new local container. Only on x.inegrity() >= 0. The copy is fully usable in the current module.
    //          c.vecm_copy(x) -- copying to a local container c. Only on x.inegrity() >= 0. The copy is fully usable in the current module.
    //          x.vecm_copy(z) -- copying to the non-local x. If z.compatibiltiy() >= 0 in the binary module of x,
    //              the function will succeed. x remains usable only in its module.
    //          x.vecm_clear -- always.
    //          x.vecm_delete (for x that is dynamically created) -- always.
    //          destructor -- always.
    //        Other functions are safe to call, but return a value type error.
    //    -1 - not usable. The container is from another module.
    //        Most calls are unsafe.
    //        The only functions that execute correctly and succeed:
    //          vecm_clear,
    //          vecm_delete (if the object was dynamically created),
    //          destructor,
    //          integrity, locality, compatibility,
    //          ptd, nexc, can_shrink, vecm_setf_*, vecm_set0_nexc.
    //    -2, -3 - invalid. These return values are same as integrity() -2, -3.
    //        The container may be from this or another module.
    //        The only functions that execute correctly:
    //          integrity, locality, compatibility.
  inline s_long compatibility() const throw()
  {
    if (!this) { return -3; } if (!_t_ind) { return -2; } if (!_ptd) { return -3; }
    if (_ptd->psig == _ls_psig()) { return 2; }
    if (*_ptd->psig != *_ls_psig()) { return -3; }
    if (!bytes::is_eq_str<s_long>(_ptd->psig, _ls_psig(), 9, 9)) { return -1; }
    if (bytes::is_eq_str<s_long>(_ptd->psig + 9, _ls_psig() + 9, -1, -1)) { s_long z = _cmd_reg::find_cm(_t_ind, _ptd->ta_ind, false); return z - 1; }
    s_long ver2 = _ptd->psig[9];
    if (ver2 == 0) { s_long z = _cmd_reg::find_cm(_t_ind, _ptd->ta_ind, false); if (sizeof(meta::t_pdiff) == 4) { return z - 1; } return z >= 1 ? 0 : -1; }
    if (ver2 >= 101 && ver2 <= 109) { s_long z = _cmd_reg::find_cm(_t_ind, _ptd->ta_ind, false); return z >= 1 ? 0 : -1; }
    return -1;
  }

    // On integrity() >= -1, returns non-0 ptr. to type descriptor.
    //  Otherwise, returns 0.
    //  See also struct type_descriptor.
  inline const type_descriptor* ptd() const throw() { if (!(this && _t_ind && _ptd && (_ptd->psig == _ls_psig() || *_ptd->psig == *_ls_psig()))) { return 0; } return _ptd; }

    // Informational. Checks the byte level state of an element.
    //  May be useful if the container is unsafe (integrity() == -1).
    //  Returns:
    //    1, if the object at ind is whole filled with zero bytes,
    //    0 if it has at least one non-0 byte,
    //    -1 a) ind is incorrect b) the container is invalid (integrity() <= -2).
    //  NOTE bytes::mem_is_null_t<T>(p) performs the same check for an object given by pointer.
  inline s_long null_state(s_long ind) const throw() { if (!(this && _t_ind && _ptd && (_ptd->psig == _ls_psig() || *_ptd->psig == *_ls_psig()))) { return -1; } if (!_pj) { return -1; } ind -= _nbase; if (ind < 0|| ind >= _n)  { return -1; } s_long j, k, cap; m::_ind_jk(ind, j, k, cap); return bytes::mem_is_null(_pj[j].atc_ku(k, cap, _ptd->t_size), _ptd->t_size); }

    // Changing nbase().
  inline void vecm_set_nbase(s_long nbase) throw() { _nbase = nbase; }

    // Set the value of can_shrink() flag.
  inline void vecm_setf_can_shrink(bool x) throw() { _setf_can_shrink(x); }

    // Setting nexc() == 0. If integrity() was 0, it becomes 1.
  inline void vecm_set0_nexc() const throw() { _nxf &= _fm; }

     // x true: mark the object as unsafe. integrity() 1 or 0 becomes -1.
     //   x false: integrity() -1 becomes 0 or 1 again.
     //   See integrity() return values for details.
  inline void vecm_setf_unsafe(bool x) const throw() { _setf_unsafe(x); }

// vecm(ptyper p, s_long base)
//==section 5==============================================

    // Constructing a valid container for sequence of elements
    //    of the given type (ptyper = typer<T>).
    //
    //  Requirements for T:
    //
    //    1. T must have an accessible desturctor.
    //    2. By dflt., T must have an accessible copy constructor.
    //      This is for normal operations on elements:
    //      insert, remove, append, copy etc..
    //    3. By dflt., T may have no default constructor.
    //      See also type configuration flag nt_allowDfltConstruct.
    //
    //  NOTE There are several options for overriding destructors, constructors etc.
    //      See config_t, esp. moverMode, copierMode.
    //
    //  Parameters:
    //
    //    ptyper - by default it is typer<T> (function pointer).
    //
    //      Optionally, vecm may use an alternative type configuration:
    //
    //      struct T_alt {}; // a formal equivalent of T, may be itself unusable
    //        template<> spec<T_alt> { typedef config_t<T, ...> config; };
    //        template<> struct meta::type_equi<T_alt, T> { typedef T t_3; };
    //
    //      Here, T_alt is made equivalent to T. With ptyper == typer<T_alt>,
    //        vecm will have T as its element, but use spec<T_alt> instead of spec<T> as configuration.
    //        So, different vecm objects with same T may have different type configurations.
    //      This allows fine-tuning their behavior (time/memory tradeoffs, grade of exception safety,
    //        memory management etc.).
    //
    //    base - base index of elements (numeric value of the index of the very first element of non-empty sequence).
    //
  typedef const type_descriptor& (*ptyper)();
  inline vecm(ptyper p, s_long base) throw()
    { if(!this) { return; } _ptd = &p(); _t_ind = _ptd ? _ptd->t_ind : 0; _n = 0; _last_j = 0; _size_k = 0; _pj = 0; _nj = 0; _nbase = base; _nxf = _nxf_default(); if (!_t_ind) { _ptd = &_config_ff::_td_inst<s_long>::ref(); _t_ind = _ptd->t_ind; _nxf = (_nxf & _fm) | _xd; return; } _alloc_j0(); }

    // Copying nbase and n elements.
    //  NOTE this->can_shrink() is set to dflt. true, not copied.
    //    this->nrsv() is set to min. value. not that of x.
    //  Results:
    //    a) copying succeeded.
    //          The container is a copy of x.
    //          nexc() == 0. integrity() == 1.
    //    b) element copying is disallowed (see copierMode 0 in vecm config_t),
    //        The container is empty. From x, only the element type and nbase() are copied.
    //          nexc() == 0. integrity() == 1.
    //    c) copying has been completed unsuccessfully.
    //        The container is empty. From x, only the element type and nbase() are copied.
    //          nexc():
    //              == 1 (copying failed),
    //              >= 2 or negative (copying and some temp. elem. desructors are failed).
    //          integrity() == 0.
    //    d) copying failed.
    //        x is invalid, unsafe or incompatible.
    //        The container is empty, initialized by default for elements of type s_long. nbase() == 0.
    //        nexc() == 1. integrity() == 0.
    //    e) this object is invalid (integrity() <= -2). The call is ignored.
  inline vecm(const vecm& x) throw() { if (!this) { return; } _l_cc(x); }

    // The object is cleared. _t_ind is set to 0 to mark the object unusable.
    // NOTE nexc() is not cleared. It may be increased by the number of errors in element destructors.
    // NOTE elements are destroyed in the order first-to-last.
  inline ~vecm() throw() { if (!(this && _t_ind && _ptd && (_ptd->psig == _ls_psig() || *_ptd->psig == *_ls_psig()))) { return; } _ptd->pvecm_destroy(this); }

    // Copying x to *this.
    //  is_tr true: transactional copying. If it fails, all changes are cancelled.
    //      NOTE The old object is destroyed only after the new has been created.
    //  is_tr false: the operation behaves like ~vecm() + vecm(const vecm&).
    //      NOTE (!) is_tr false may not be used for copying between container and its own element,
    //        even indirectly.
    //  NOTE this->can_shrink() is set to dflt. true, not copied or preserved.
    //    this->nrsv() is set to min. value. not that of x or preserved.
    //  Returns:
    //    1 on on_calm_result. (Also if this == &x.)
    //        nexc() may be > 0 if a number of failures occured in the old object during destruction.
    //    0 (only on is_tr == true) the operation failed, changes are cancelled.
    //        nexc() is increased by 1 + the number of exceptions in destructors
    //        of the temporary objects.
    //    -1 (only on is_tr == false) copying has been completed unsuccessfully.
    //        The container is empty. From x, only the element type and nbase() are copied.
    //        nexc() != 0. It is increased by the number of errors during operation.
    //    -2 (only on is_tr == false) copying failed.
    //        x is invalid, unsafe or incompatible.
    //        The container is empty, initialized by default for elements of type s_long. nbase() == 0.
    //        nexc() != 0 (increased by 1 from prev. value). integrity() == 0.
    //    -3 - this object is invalid (integrity() <= -2) or incompatible. The call is ignored.
  inline s_long vecm_copy(const vecm& x, bool is_tr) throw() { if (!(this && _t_ind && _ptd && (_ptd->psig == _ls_psig() || *_ptd->psig == *_ls_psig()))) { return -3; } return _ptd->pvecm_copy(this, &x, is_tr); }

    // Removes all elements, frees the reserved place, sets all flags to default, sets nexc() to 0.
    //      nbase() is not changed.
    //  Returns:
    //    a) the previous value of nexc() (it may indicate errors during clearing).
    //    b) -1 - this object is invalid (integrity() <= -2) or incompatible. The call is ignored.
    //  NOTE The following:
    //        v.vecm_clear(); v.vecm_set_nbase(nb);
    //    makes v same as a new object constructed by
    //        vecm(typer<...>, nb);
    // NOTE elements are destroyed in the order first-to-last.
  inline s_long vecm_clear() throw() { if (!(this && _t_ind && _ptd && (_ptd->psig == _ls_psig() || *_ptd->psig == *_ls_psig()))) { return -1; } return _ptd->pvecm_clear(this); }

    // Replacement for op. delete vecm*, when the object may have been created in another module.
    //    Returns:
    //    1 - the object is deleted successfully.
    //    0 - the object deleted, but generated an exception during op. delete.
    //    -1 - the object is invalid or incompatible, nothing done.
    // NOTE vecm_delete() is integrity/compatibility-resistant.
    //    It should be preferred to op. delete when the client works
    //    with mixed local and non-local objects.
  inline s_long vecm_delete() throw() { if (!(this && _t_ind && _ptd && (_ptd->psig == _ls_psig() || *_ptd->psig == *_ls_psig()))) { return -1; } return _ptd->pvecm_delete(this); }

// vecm el_remove_all()
//==section 6==============================================

    // Removes all existing elements.
    //    The space left is freed if can_shrink() is true.
    // Returns:
    //  >0 on on_calm_result - the number of elements removed (previous n()).
    //      nexc() may indicate errors occured during elements destruction.
    //  0 if the container was empty (n() == 0). Nothing is done,
    //      except possible storage space reserve freeing.
    //  -1 if the container object is invalid (integrity() == -2). Nothing done.
    //  NOTE unlike ~vecm() and vecm_clear(), el_remove_all()
    //    ignores u_ignoreNullsOnCleanup and u_ignoreDestructorsOnCleanup flags,
    //    so, calling it for object with integrity() < 0 instead of vecm_clear() may be less safe.
    // NOTE elements are destroyed in the order first-to-last.
  s_long el_remove_all() throw()
  {
    if (!_t_ind) { return -1; }
    s_long n_prev(_n);
    if (_pj && _ptd->dmode)
    {
      for (s_long j = 0; j <= _last_j; ++j)
      {
        s_long ec(0);
        _column& c = _pj[j]; s_long cap = m::_cap_j(j); s_long k2 = j == _last_j ? _size_k : cap;
        s_long k1 = cap - c.kbase; s_long k = 0; char* p = c.atc_0u(_ptd->t_size);
        while (k < k2) { if (_ptd->p_destroy_1(p) < 0) { ++ec; } p += _ptd->t_size; ++k; if (k == k1) { p = c.pd; } }
        if (ec) { _nx_add(_nxf, ec); }
      }
    }
    _n = 0; _last_j = 0; _size_k = 0; if (_pj) { _try_shrink(); }
    return n_prev;
  }

    // Setting the place reserve in the container enough for at least n elements.
    //  Returns:
    //    a) true when ensured enough space for this vecm to contain at least n elements.
    //      If n < nrsv(), and allow_shrink = true, the existing place reserve will be shrunk to min. possible value.
    //    b) false when failed to allocate enough space (may occur only on n > n()).
    //  NOTES:
    //    The factually set reserve (nrsv()) may be slightly more than the given n.
    //    Reserving does not guarantee faultless subsequent adding elements if element constructor
    //      may itself fail for any inner reason (getting locks, handles, memory or anything else).
    //    The function does not change the number of elements (n()).
    //    The function does not relocate any existing elements.
    //    The function is transactional in relation to the reserved place.
    //    The function ignores can_shrink() flag.
    //    Under normal conditions, on n <= n(), the function always succeeds, since the place is already allocated.
  bool el_reserve_n(s_long n, bool allow_shrink) throw()
  {
    if (!_t_ind) { return false; } if (!_pj) { if (n <= 0) { return true; } if (!_alloc_j0()) { return false; } }
    s_long nj2; s_long nk2; m::_ind_jk(n > _n ? n : _n, nj2, nk2); if (nk2 > 0) { ++nj2; }
    if (nj2 == _nj) { return true; }

    if (nj2 > _nj) // grow
    {
      _column* px = reinterpret_cast<_column*>(_ptd->_pxmalloc_r((nj2 - _nj) * sizeof(_column))); if (!px) { return false; }
      s_long jf = -1;
      for (s_long j = _nj; j < nj2; ++j)
      {
        s_long size = _ptd->t_size * m::_cap_j(j); _column& cj = px[j - _nj]; if (!cj.init_alloc(_ptd, size)) { jf = j; break; }
        for (s_long i = 0; i < size; i += __minmb()) { cj.pd[i] = 0; }
      }
      if (jf >= 0) { for (s_long j = _nj; j < jf; ++j) { px[j - _nj].clear(_ptd); } _ptd->_pxmfree_r(px); return false; }

      _column* pj2 = reinterpret_cast<_column*>(_ptd->_pxmrealloc_r(_pj, nj2 * sizeof(_column))); if (!pj2) { for (s_long j = _nj; j < nj2; ++j) { px[j - _nj].clear(_ptd); } _ptd->_pxmfree_r(px); return false; }
      _pj = pj2; for (s_long j = _nj; j < nj2; ++j) { _pj[j].init_move_from(px[j - _nj]); } _ptd->_pxmfree_r(px);
      _nj = nj2; return true;
    }

    if (allow_shrink) // nj2 < _nj, shrink
    {
      if (nj2 == 0) { nj2 = 1; } for (s_long j = nj2; j < _nj; ++j) { _pj[j].clear(_ptd); }
      _column* pj2 = reinterpret_cast<_column*>(_ptd->_pxmrealloc_r(_pj, nj2 * sizeof(_column))); if (pj2) { _pj = pj2; } // pj2 == 0 on shrinking is not critical
      _nj = nj2; return true;
    }

    return true;
  }

    //  Ensures allocating place for 1 additional element at sequence end.
    //    Element's constructor is not called (client decides call/no call, before/after etc.).
    //  Returns:
    //    a) on on_calm_result, non-0 pointer to the allocated place. Also, n() is increased by 1.
    //      NOTE Under normal conditions, if the place was reserved before (e.g. el_reserve_n),
    //          the function always succeeds.
    //    b) 0 on type error or memory allocation error. Elem. sequence is not changed.
  template<class T>
  inline T* el_expand_1() throw()
  {
    typedef bytes::type_index_t<T> t_si; if (_t_ind != t_si::xind) { if (_t_ind != t_si::ind()) { return 0; } }
    if (++_n <= 1) { if (_n < 0) { --_n; return 0; } if (!_pj) { if (!_alloc_j0()) { return 0; } } }

    _yk_reg s_long r = _last_j; if (r & ~s_long(m::m1-1)) { if (r < m::ind3) { r = m::mask2; } else { r = m::mask3; } } else  { r = m::mask1; }
    if (r < _size_k)
    {
      T* ret_p = _alloc_lj1<T>(); if (!ret_p) { --_n; return 0; }
      _size_k = 1; ++_last_j; return ret_p;
    }
    else
    {
      T* ret_p = _pj[_last_j].at_kmu<T>(_size_k, r);
      ++_size_k; return ret_p;
    }
  }

    //  Expand. n2 > n(). The function ensures enough place for n2 - n() additional elements
    //    at the end of sequence. Elements constructors are not called
    //    (the client decides call/no call, before/after etc.).
    //    NOTE If the existing place reserve is large enough, no additional reserving or shrinking occurs.
    //  Expunge / unreserve. n2 == [0..n()]. regards n() - n2 last elements
    //    of the sequence as non-existing.
    //    Destructors are not called (the client decides call/no call, before/after etc.).
    //    n() becomes n2. Memory reserve is shrunk if can_shrink() == true.
    // Returns:
    //    true -- Expand or expunge succeeded. (In the valid container, expunge always succeeds.)
    //    false -- Expand failed allocate enough place for additional n2 - n() elements. Nothing changed.
    //    false -- n2 < 0 or the container is invalid (integrity() == -2). Nothing changed.
  inline bool el_expand_n(s_long n2) throw()
  {
    if (!_t_ind) { return false; }
    if (n2 > _n) { if (!_pj) { if (!_alloc_j0()) { return false; } } if ( ! _reserve_incr_u(n2) ) { return false; } _set_size(n2); }
      else { if (n2 < 0) { return false; } _set_size(n2); _try_shrink(); }
    return true;
  }

    // Regards the last element as non-existing. n() decreases by 1.
    //    Element's destructor is not called (the client decides call/no call, before/after etc.).
    //    Place reserve may shrink if can_shrink() == true.
    // Returns:
    //    true the last position existed, and has been removed,
    //    false on type error or on n() == 0.
    // NOTE. el_expunge_last is counterpart to el_expand_1, same as el_remove_last to el_append.
  template<class T>
  inline bool el_expunge_last() throw()
  {
    typedef bytes::type_index_t<T> t_si; if (_t_ind != t_si::xind) { if (_t_ind != t_si::ind()) { return false; } }
    if (_n <= 0) { return false; }
    --_n; --_size_k; if (_size_k == 0 && _last_j > 0) { --_last_j; _size_k = m::_cap_j(_last_j); }
    _try_shrink();
    return true;
  }

    // NOTE appending does not relocate any existing elements.
    // NOTE el_append is always transactional, regardless of T configuration.
    //    Only, on first failure, it can slightly increase the reserved place.
    // NOTE el_append also accepts as x an arbitrary functor like construct_f<T> (see).
  template<class TF> inline typename meta::resolve_TF<TF, meta::tag_construct>::t*
  el_append(const TF& x) throw()
  {
    typedef typename meta::resolve_TF<TF, meta::tag_construct>::t T;
    typedef bytes::type_index_t<T> t_si;
    _yk_reg s_long r = _t_ind; if (r != t_si::xind) { if (r != t_si::ind()) { return 0; } }
    r = _n; if (++r <= 1) { if (r < 0) { return 0; } if (!_pj) { if (!_alloc_j0()) { return 0; } } }

    _n = r; r = _last_j; if (r & ~s_long(m::m1-1)) { if (r < m::ind3) { r = m::mask2; } else { r = m::mask3; } } else  { r = m::mask1; }
    if (r < _size_k)
    {
      T* ret_p = _alloc_lj1<T>(); if (!ret_p) { --_n; return 0; }
      s_long xc(0); _copy_1u_t<TF, false>::F(_ptd, ret_p, x, xc); if (xc) { --_n; return 0; }
      _size_k = 1; ++_last_j; return ret_p;
    }
    else
    {
      T* ret_p = _pj[_last_j].at_kmu<T>(_size_k, r);
      s_long xc(0); _copy_1u_t<TF, false>::F(_ptd, ret_p, x, xc); if (xc) { --_n; return 0; }
      ++_size_k; return ret_p;
    }
  }

    // Removes the last element.
    // Returns:
    //    1: the last element existed and was removed.
    //    -1: wrong type or n() == 0.
  template<class T>
  inline s_long el_remove_last() throw()
  {
    typedef bytes::type_index_t<T> t_si; if (_t_ind != t_si::xind) { if (_t_ind != t_si::ind()) { return -1; } }
    if (_n <= 0) { return -1; }
    --_n; --_size_k;
    if (_ptd->dmode) { if (_ptd->p_destroy_1(_pj[_last_j].at_ku<T>(_size_k, m::_cap_j(_last_j))) < 0) { if (_nxf >= 0) { _nxf += _xd; } } }
    if (_size_k == 0 && _last_j > 0) { --_last_j; _size_k = m::_cap_j(_last_j); }
    _try_shrink();
    return 1;
  }


    // Inserts a copy of x into elements sequence at pos. ind = [nbase()..nbase()+n()].
    //    Elements starting from [ind] will have inds. +1 from prev. value. This takes O(N^0.5).
    //    Some elements are moved in memory, unless ind is after-last pos. (nbase()+n()).
    // NOTE el_insert_1 also accepts as x an arbitrary functor like construct_f<T> (see).
    // Returns:
    //    1 if the element was successfully inserted.
    //    -1 on parameter checks error (integrity, x type, ind range). Nothing done.
    //    -2: failed before insertion. Not enough memory or an assertion failed. Nothing done.
    //    -3: failed during transcational insertion (dflt. for all types).
    //        All changes are CANCELLED. The place may be once left a bit larger than it was (like in el_append).
    //    -4: failed during non-transactional insertion.
    //        Operation is forcibly COMPLETED. n() is increased by 1. Integrity state is set to -1.
    //        >=1 arbitrary element may be left with invalid value.
    //      NOTE It is not recommended to create type configurations allowing for unsafe non-transactional insertion.
    //      NOTE On appending (ind == nbase() + n()) the operation is always transactional,
    //        regardless of T configuration.
  template<class TF>
  s_long el_insert_1(s_long ind, const TF& x) throw()
  {
    typedef typename meta::resolve_TF<TF, meta::tag_construct>::t T;
    typedef bytes::type_index_t<T> t_si; if (_t_ind != t_si::xind) { if (_t_ind != t_si::ind()) { return -1; } }
    ind -= _nbase; if (ind < 0)  { return -1; }
    s_long xc(0);

    if (ind >= _n) // insert without any moving (append)
    {
      if (ind > _n) { return -1; }
      if (ind == meta::s_long_mp) { return -2; }
      if (_ptd->cmode == 0) { if (!meta::resolve_TF<TF, meta::tag_construct>::use_functor) { return -2; } }
      if ( ! _reserve_incr_u(_n + 1) ) { return -2; }
      s_long cap_j = m::_cap_j(_last_j);
      T* px = _size_k < cap_j ? _pj[_last_j].at_ku<T>(_size_k, cap_j) : _pj[_last_j+1].at_0u<T>();
      _copy_1u_t<TF, false>::F(_ptd, px, x, xc); if (xc) { return -3; }
      ++_n; ++_size_k; if (_size_k > cap_j) { ++_last_j; _size_k = 1; }
      return 1;
    }

    if (_n == meta::s_long_mp) { return -2; }
    s_long j, k0, cap_j; m::_ind_jk(ind, j, k0, cap_j);
    if (k0 == 0 && j == _last_j && _size_k < cap_j) // insert without any moving (before the 1st el. in _last_j)
    {
      if (_ptd->cmode == 0) { if (!meta::resolve_TF<TF, meta::tag_construct>::use_functor) { return -2; } }
      if ( ! _reserve_incr_u(_n + 1) ) { return -2; }
      _column* pc = _pj + _last_j;
      _copy_1u_t<TF, false>::F(_ptd, pc->at_9u<T>(cap_j), x, xc); if (xc) { return -3; }
      pc->kbase_decr(cap_j);
      ++_n; ++_size_k;
      return 1;
    }

    if (_ptd->op_flags & 0x4) // tr. alg. when moving may fail
    {
      if (_ptd->cmode == 0) { return -2; }
      if (j > _last_j) { if (_nxf >= 0) { _nxf += _xd; } return -2; }
      s_long cap_last = j == _last_j ? cap_j : m::_cap_j(_last_j);
      bool b1 = _size_k >= cap_last;
      s_long nR; if (j == _last_j) { nR = _size_k - k0; if (b1) { --nR; } } else { nR = cap_j-1-k0; } s_long nL = k0;
      s_long m1 = _last_j - j; if (b1) { ++m1; }
      s_long m2 = m1; s_long dir; if (nL <= nR) { m2 += nL; dir = 1; } else { m2 += nR; dir = -1; }
      s_long n(0); bool cancel(false); char* bu = _tr_bu_alloc_t<T>(m2+1); if (!bu) { return -2; } char* bu0 = bu + m2 * sizeof(T);
      if ( ! _reserve_incr_u(_n + 1) ) { _tr_bu_free(bu); return -2; }

      T* px1; T* px2; s_long j2, k2;
      if (m1 > 0)
      {
        if (b1) { j2 = _last_j+1; px1 = _pj[j2].at_0u<T>(); } else { j2 = _last_j; px1 = _pj[j2].at_9u<T>(cap_last); }
        do { --j2; px2 = _pj[j2].at_9u<T>(m::_cap_j(j2)); if (!_tr_copy_1_busrc_t<T>(px1, px2, bu, n)) { cancel = true; break; } px1 = px2; } while (n < m1);
      }
      if (!cancel && n < m2)
      {
        if (nL <= nR) { k2 = 0; px1 = _pj[j].at_9u<T>(cap_j); } else { k2 = k0 + nR; px1 = _pj[j].at_ku<T>(k2, cap_j); --k2; }
        do { px2 = _pj[j].at_ku<T>(k2, cap_j); if (!_tr_copy_1_busrc_t<T>(px1, px2, bu, n)) { cancel = true; break; } px1 = px2; k2 += dir; } while (n < m2);
      }
      s_long k01 = k0; if (nL <= nR) { --k01; if (k01 < 0) { k01 = cap_j-1; } }
      if (!cancel)
      {
        px1 = _pj[j].at_ku<T>(k01, cap_j);
        xc = 0; _copy_1u_t<TF, false>::F(_ptd, px1, x, xc); if (xc) { cancel = true; }
      }
      if (cancel)
      {
        if (n > 0)
        {
          if (m1 > 0) { if (b1) { j2 = _last_j+1; px1 = _pj[j2].at_0u<T>(); } else { j2 = _last_j; px1 = _pj[j2].at_9u<T>(cap_last); } }
            else { if (nL <= nR) { px1 = _pj[j].at_9u<T>(cap_j); } else { px1 = _pj[j].at_ku<T>(k0 + nR, cap_j); } }
          if (_ptd->dmode) { if (_ptd->p_destroy_1(px1) < 0) { if (_nxf >= 0) { _nxf += _xd; } } }

          s_long n2 = 0;
          if (m1 > 0 && n2 < n) { if (b1) { j2 = _last_j+1; } else { j2 = _last_j; } while (n2 < m1 && n2 < n) { --j2; px2 = _pj[j2].at_9u<T>(m::_cap_j(j2)); _tr_revert_t<T>(px2, bu, n2); ++n2; } }
          if (m2 > m1 && n2 < n) { if (nL <= nR) { k2 = 0; } else { k2 = k0 + nR - 1; } while (n2 < m2 && n2 < n) { px2 = _pj[j].at_ku<T>(k2, cap_j); _tr_revert_t<T>(px2, bu, n2); ++n2; k2 += dir; } }
        }
        _tr_bu_free(bu); return -3;
      }
      n = 0;
      if (m1 > 0)
      {
        // b1 true (the last col. was full): _tr_destroy_prev_t+kbase_decr Test from _last_j
        // b1 false: kbase_decr starts from _last_j, _tr_destroy_prev_t starts from _last_j-1
        if (b1) { _tr_destroy_prev_t<T>(_pj[_last_j].at_9u<T>(cap_last), bu, n, bu0); ++n; }
        if (n < m1) { _pj[_last_j].kbase_decr(cap_last); }
        j2 = _last_j; while (n < m1) { --j2; s_long cap2 = m::_cap_j(j2); px2 = _pj[j2].at_9u<T>(cap2); _tr_destroy_prev_t<T>(px2, bu, n, bu0); ++n; if (n < m1) { _pj[j2].kbase_decr(cap2); } };
      }
      if (m2 > m1)
      {
        if (nL <= nR) { k2 = 0; } else { k2 = k0 + nR; --k2; }
        do { px2 = _pj[j].at_ku<T>(k2, cap_j); _tr_destroy_prev_t<T>(px2, bu, n, bu0); ++n; k2 += dir; } while (n < m2);
      }
      if (nL <= nR) { _pj[j].kbase_decr(cap_j); }
      _tr_bu_free(bu);
      ++_n; ++_size_k; if (b1) { ++_last_j; _size_k = 1; }
      return 1;
    }
    else if ((_ptd->op_flags & 0x8) || ((_ptd->op_flags & 0x2) && meta::resolve_TF<TF, meta::tag_construct>::use_functor)) // tr. alg. when only copy may fail
    {
      if (_ptd->cmode == 0) { if (!meta::resolve_TF<TF, meta::tag_construct>::use_functor) { return -2; } }
      if (j > _last_j) { if (_nxf >= 0) { _nxf += _xd; } return -2; }
      s_long cap_last = j == _last_j ? cap_j : m::_cap_j(_last_j);
      bool b1 = _size_k >= cap_last;
      s_long nR; if (j == _last_j) { nR = _size_k - k0; if (b1) { --nR; } } else { nR = cap_j-1-k0; } s_long nL = k0;
      s_long m1 = _last_j - j; if (b1) { ++m1; }
      s_long m2 = m1; s_long dir; if (nL <= nR) { m2 += nL; dir = 1; } else { m2 += nR; dir = -1; }
      if ( ! _reserve_incr_u(_n + 1) ) { return -2; }

      T* px1; T* px2; s_long j2, k2; s_long n;
      n = 0;
      if (m1 > 0)
      {
        if (b1) { j2 = _last_j+1; px1 = _pj[j2].at_0u<T>(); } else { j2 = _last_j; px1 = _pj[j2].at_9u<T>(cap_last); }
        do { --j2; px2 = _pj[j2].at_9u<T>(m::_cap_j(j2)); _ptd->p_move_1(px1, px2); ++n; px1 = px2; } while (n < m1);
      }
      if (n < m2)
      {
        if (nL <= nR) { k2 = 0; px1 = _pj[j].at_9u<T>(cap_j); } else { k2 = k0 + nR; px1 = _pj[j].at_ku<T>(k2, cap_j); --k2; }
        do { px2 = _pj[j].at_ku<T>(k2, cap_j); _ptd->p_move_1(px1, px2); ++n; px1 = px2; k2 += dir; } while (n < m2);
      }
      s_long k01 = k0; if (nL <= nR) { --k01; if (k01 < 0) { k01 += cap_j; } }
      px1 = _pj[j].at_ku<T>(k01, cap_j);
      xc = 0; _copy_1u_t<TF, false>::F(_ptd, px1, x, xc);
      if (xc) // cancelling
      {
        n = m2;
        if (n > m1) { k2 = k01; px1 = _pj[j].at_ku<T>(k2, cap_j); do { k2 -= dir; if (k2 < 0) { k2 += cap_j; } px2 = _pj[j].at_ku<T>(k2, cap_j); _ptd->p_move_1(px1, px2); --n; px1 = px2; } while (n > m1); }
        if (n > 0) { px1 = _pj[j].at_9u<T>(cap_j); j2 = j; do { ++j2; if (j2 <= _last_j) { px2 = _pj[j2].at_9u<T>(m::_cap_j(j2)); } else { px2 = _pj[j2].at_0u<T>(); } _ptd->p_move_1(px1, px2); --n; px1 = px2; } while (n > 0); }
        return -3;
      }
      n = 0;
      if (m1 > 0)
      {
        if (b1) { ++n; } if (n < m1) { _pj[_last_j].kbase_decr(cap_last); }
        j2 = _last_j; while (n < m1) { --j2; ++n; if (n < m1) { _pj[j2].kbase_decr(m::_cap_j(j2)); } };
      }
      if (nL <= nR) { _pj[j].kbase_decr(cap_j); }
      ++_n; ++_size_k; if (b1) { ++_last_j; _size_k = 1; }
      return 1;
    }
    else // non-transcational op.
    {
      if (_ptd->cmode == 0) { if (!meta::resolve_TF<TF, meta::tag_construct>::use_functor) { return -2; } }
      if (_ptd->mmode == 0) { return -2; }
      if ( ! _reserve_incr_u(_n + 1) ) { return -2; }
      bool b1(false); // b1 true == new column is added
      if (j < _last_j) // shift elements in columns [j.._last_j] by 1 pos. fwd., insert T(x) into column j
      {
        s_long cap2 = m::_cap_j(_last_j); s_long j2; _column* pc; _column* pc_prev;
        if (_size_k == cap2) { b1 = true; j2 = _last_j+1; pc = _pj+j2; }
          else { j2 = _last_j; pc = _pj+j2; pc->kbase_decr(cap2); }
        do
        {
          pc_prev = pc; --pc; --j2; cap2 = m::_cap_j(j2);
          _move_1u_t<T, true>::F(_ptd, pc_prev->at_0u<T>(), pc->at_9u<T>(cap2), _nxf, xc);
          if (j2 == j) { break; } pc->kbase_decr(cap2);
        } while (true);
        _insert_1u_t<TF>(pc, 0, x, k0, cap2-1, cap2, _nxf, xc);
      }
      else if (j == _last_j) // insert T(x) into the last column
      {
        if (_size_k == cap_j) { b1 = true; }
        _column* pc = _pj+_last_j;
        _insert_1u_t<TF>(pc, b1 ? pc+1 : 0, x, k0, _size_k, cap_j, _nxf, xc);
      }
      else // insert T(x) into a new column
      {
        b1 = true;
        _column* pc = _pj+_last_j+1;
        _copy_1u_t<TF, true>::F(_ptd, pc->at_0u<T>(), x, xc);
      }
      if (xc) { _setf_unsafe(true); _nx_add(_nxf, xc >> _xsh); }
      ++_n; ++_size_k; if (b1) { ++_last_j; _size_k = 1; }
      return xc ? -4 : 1;
    }
  }

    // Removes 1 element from the sequence.
    //    ind must be [nbase()..nbase()+n()-1].
    // A) move_last false removes [ind], index of any existing element at >ind will be -1 from prev. value.
    //  This takes O(N^0.5). Some elements are moved in memory, unless [ind] was the last element.
    //  (Note that element pointers, obtained before the operation, in general, become invalid.
    //  Use indices or iterators instead.)
    // B) move_last true: if [ind] is the last element (L), L is removed (same as move_last false).
    //  If [ind] is not L, [ind] is replaced by L, so L will now reside at ind.
    //  Any other elements are not moved. All pointers to elements remain valid.
    //  Fixed time.
    // NOTE If can_shrink() == true, after an element has been removed, the free storage place,
    //    left from this and previous removals, is returned back to system.
    // Returns:
    //    1 if [ind] existed and was removed.
    //      NOTE Under normal conditions, on removing the last element (ind == nbase() + n() - 1)
    //        the operation always succeeds.
    //        (Additionally, nexc() > 0 may indicate that destructor of the last element has failed.)
    //    -1 on parameter checks error (integrity, x type, ind range). Nothing done.
    //    -2: failed before removal. Not enough memory or an assertion failed. Nothing done.
    //    -3: failed during transcational removal (dflt. for all types).
    //        All changes are CANCELLED.
    //    -4: failed during non-transactional removal.
    //        Operation is forcibly COMPLETED. n() is decreased by 1. Integrity state is set to -1.
    //        >=1 arbitrary element may be left with invalid value.
    //        (On move_last == true, only the element [ind] may be left with invalid value.)
    //      NOTE It is not recommended to create type configurations allowing for unsafe non-transactional removal.
  template<class T>
  s_long el_remove_1(s_long ind, bool move_last) throw()
  {
    typedef bytes::type_index_t<T> t_si; if (_t_ind != t_si::xind) { if (_t_ind != t_si::ind()) { return -1; } }
    ind -= _nbase; if (ind < 0)  { return -1; } if (ind >= _n)  { return -1; }
    if (ind == _n - 1)
    {
      --_n; --_size_k;
      if (_ptd->dmode) { if (_ptd->p_destroy_1(_pj[_last_j].at_ku<T>(_size_k, m::_cap_j(_last_j))) < 0) { if (_nxf >= 0) { _nxf += _xd; } } }
      if (_size_k == 0 && _last_j > 0) { --_last_j; _size_k = m::_cap_j(_last_j); }
      _try_shrink();
      return 1;
    }
    else if (move_last)
    {
      if (_ptd->mmode == 0) { return -2; }
      s_long xc(0);
      T* px; T* px_last;

      s_long j, k, cap; m::_ind_jk(ind, j, k, cap);
      px = _pj[j].at_ku<T>(k, cap); px_last = _pj[_last_j].at_ku<T>(_size_k-1, m::_cap_j(_last_j));

      if (_ptd->op_flags & 0x4)
      {
        if (sizeof(T) <= 1024)
        {
          char bu0[sizeof(T)]; char bu[sizeof(T)];
          if (!_tr_move_1only_t<T>(px, px_last, bu)) { return -3; }
          _tr_destroy_prev_1only_t<T>(px, bu, bu0);
        }
        else
        {
          char* bu = _tr_bu_alloc_t<T>(2); if (!bu) { return -2; }
          if (!_tr_move_1only_t<T>(px, px_last, bu)) { _tr_bu_free(bu); return -3; }
          _tr_destroy_prev_1only_t<T>(px, bu, bu+sizeof(T));
          _tr_bu_free(bu);
        }
      }
      else // non-transcational op.
      {
        if (_ptd->dmode) { if (_ptd->p_destroy_1(px) < 0) { if (_nxf >= 0) { _nxf += _xd; } } }
        _move_1u_t<T, true>::F(_ptd, px, px_last, _nxf, xc);
        if (xc) { _setf_unsafe(true); _nx_add(_nxf, xc >> _xsh); }
      }
      --_n; --_size_k; if (_size_k == 0 && _last_j > 0) { --_last_j; _size_k = m::_cap_j(_last_j); } _try_shrink();
      return xc ? -4 : 1;
    }
    else // move_last == false and the removed element is not the last in sequence
    {
      s_long j, k0, cap; m::_ind_jk(ind, j, k0, cap); if (j > _last_j) { if (_nxf >= 0) { _nxf += _xd; } return -2; }
      if (j == _last_j && k0 == 0) // at the beginning of the last column
      {
        --_n; --_size_k;
        _column& c = _pj[_last_j];
        if (_ptd->dmode) { if (_ptd->p_destroy_1(c.at_0u<T>()) < 0) { if (_nxf >= 0) { _nxf += _xd; } } }
        if (_size_k == 0) { if (_last_j > 0) { --_last_j; _size_k = m::_cap_j(_last_j); } } else { c.kbase_incr(cap); }
        _try_shrink();
        return 1;
      }
      if (_ptd->op_flags & 0x4)
      {
        s_long nL = (j == _last_j ? _size_k : cap)-k0-1; s_long nR = k0;
        if (j == _last_j && nL <= 0) { if (_nxf >= 0) { _nxf += _xd; } return -2; }
        s_long m1, dir; if (nL <= nR) { m1 = nL; dir = 1; } else { m1 = nR; dir = -1; }
        T* px;

        if (j == _last_j && m1 == 0) // may be only on nR == 0
        {
          px = _pj[j].at_0u<T>(); if (_ptd->dmode) { if (_ptd->p_destroy_1(px) < 0) { if (_nxf >= 0) { _nxf += _xd; } } }
          _pj[j].kbase_incr(cap);
          --_n; --_size_k; _try_shrink();
          return 1;
        }
        else
        {
          s_long m2 = m1 + _last_j - j;

          T* px_next;
          s_long n(0); bool cancel(false); char* bu = _tr_bu_alloc_t<T>(m2+1); if (!bu) { return -2; } char* bu0 = bu + m2 * sizeof(T);

          s_long k(k0); px = _pj[j].at_ku<T>(k, cap);
          while (n < m1) { k += dir; px_next = _pj[j].at_ku<T>(k, cap); if (!_tr_copy_1_t<T>(px, px_next, bu, n)) { cancel = true; break; } px = px_next; }
          if (!cancel) { s_long j2(j); while (n < m2) { ++j2; px_next = _pj[j2].at_0u<T>(); if (!_tr_copy_1_t<T>(px, px_next, bu, n)) { cancel = true; break; } px = px_next; } }
          if (cancel)
          {
            s_long n2 = 0; k = k0;
            while (n2 < m1 && n2 < n) { px = _pj[j].at_ku<T>(k, cap); k += dir; _tr_revert_t<T>(px, bu, n2); ++n2; }
            while (n2 < m2 && n2 < n) { s_long i = n2 - m1; px = i ? _pj[j+i].at_0u<T>() : _pj[j].at_ku<T>(k, cap); _tr_revert_t<T>(px, bu, n2); ++n2; }
            _tr_bu_free(bu); return -3;
          }
          k = k0; for (s_long i = 0; i < m1; ++i) { px = _pj[j].at_ku<T>(k, cap); k += dir;  _tr_destroy_prev_t<T>(px, bu, i, bu0); }
          if (j == _last_j)
          {
            px = _pj[j].at_ku<T>(k, cap);
          }
          else
          {
            px = _pj[j].at_ku<T>(k, cap);  _tr_destroy_prev_t<T>(px, bu, m1, bu0);
            for (s_long i = 1; i < m2 - m1; ++i) { px = _pj[j+i].at_0u<T>(); _tr_destroy_prev_t<T>(px, bu, m1+i, bu0); }
            px = _pj[_last_j].at_0u<T>();
          }
          if (_ptd->dmode) { if (_ptd->p_destroy_1(px) < 0) { if (_nxf >= 0) { _nxf += _xd; } } }
          if (dir < 0) { _pj[j].kbase_incr(cap); }
          for (s_long j2 = j+1; j2 <= _last_j; ++j2) { _pj[j2].kbase_incr(m::_cap_j(j2)); }
          _tr_bu_free(bu);
          --_n; --_size_k; if (_size_k == 0 && _last_j > 0) { --_last_j; _size_k = m::_cap_j(_last_j); } _try_shrink();
          return 1;
        }
      }
      else // non-transcational op.
      {
        s_long xc(0);
        if (j < _last_j) // remove el. from column j, shift el. pos. in all the next columns by 1 backw.
        {
          if (_ptd->mmode == 0) { return -2; }
          if (_ptd->dmode) { if (_ptd->p_destroy_1(_pj[j].at_ku<T>(k0, cap)) < 0) { if (_nxf >= 0) { _nxf += _xd; } } }
          _column* pc = _pj+j;
          _remove_1u_t<T>(pc, k0, cap, cap, _nxf, xc);
          _column* pc2; s_long cap2;
          do
          {
            ++j; pc2 = _pj+j; cap2 = m::_cap_j(j);
            _move_1u_t<T, true>::F(_ptd, pc->at_9u<T>(cap), pc2->at_0u<T>(), _nxf, xc);
            pc2->kbase_incr(cap2);
            pc = pc2; cap = cap2;
          }
          while (j < _last_j);
        }
        else // remove el. from the last column
        {
          _column* pc = _pj + j;
          if (_ptd->mmode == 0) { if (!(k0 == 0 || k0 == _size_k - 1)) { return -2; } }
          if (_ptd->dmode) { if (_ptd->p_destroy_1(pc->at_ku<T>(k0, cap)) < 0) { if (_nxf >= 0) { _nxf += _xd; } } }
          _remove_1u_t<T>(pc, k0, _size_k, cap, _nxf, xc);
        }
        if (xc) { _setf_unsafe(true); _nx_add(_nxf, xc >> _xsh); }
        --_n; --_size_k; if (_size_k == 0 && _last_j > 0) { --_last_j; _size_k = m::_cap_j(_last_j); } _try_shrink();
        return xc ? -4 : 1;
      }
    }
  }

    // Appending m elements.
    //    (Reserves the place as necessary.)
    // NOTE appending does not relocate any existing elements.
    // NOTE el_append_m is always transactional, regardless of T configuration.
    // NOTE el_append_m also accepts as x an arbitrary functor like meta:: construct_f<T> (see).
    // Returns:
    //    m (>=1): m elements have been inserted successfully.
    //    m (== 0): Nothing done.
    //    -1 on parameter checks error (integrity, x type or m<0). Nothing done.
    //    -2: failed before appending. Not enough memory or an assertion failed. Nothing done.
    //    -3: failed during elements appending.
    //        All changes to the elements sequence are cancelled. The place reserve is also reverted.
    //        (On the first failure on appending 1 element (m == 1),
    //        the place reserve may be left a bit larger than it was, due to using el_append.)
  template<class TF>
  s_long el_append_m(s_long m, const TF& x) throw()
  {
      if (m == 1) { if (el_append<TF>(x)) { return 1; } else { return -3; } }
    typedef typename meta::resolve_TF<TF, meta::tag_construct>::t T;
    typedef bytes::type_index_t<T> t_si;
      _yk_reg s_long r = _t_ind; if (r != t_si::xind) { if (r != t_si::ind()) { return -1; } }
      r = m; if (r <= 0) { if (r == 0) { return 0; } return -1; }
      r += _n; if (r < 0) { return -2; }

      // NOTE The remaining part is also present in el_insert_ml.
    s_long n2 = r; s_long nj_prev = _nj; s_long n_prev = _n;
    if (_ptd->cmode == 0) { if (!meta::resolve_TF<TF, meta::tag_construct>::use_functor) { return -2; } }
    if (true) // adapted _reserve_incr_u(n2)
    {
      s_long nj2; s_long nk2; m::_ind_jk(n2, nj2, nk2); if (nk2 > 0) { ++nj2; }
      if (nj2 > _nj)
      {
        _column* px = reinterpret_cast<_column*>(_ptd->_pxmalloc_r((nj2 - _nj) * sizeof(_column))); if (!px) { return -2; }
        s_long jf = -1;
        for (s_long j = _nj; j < nj2; ++j)
        {
          s_long size = _ptd->t_size * m::_cap_j(j); _column& cj = px[j - _nj]; if (!cj.init_alloc(_ptd, size)) { jf = j; break; }
          for (s_long i = 0; i < size; i += __minmb()) { cj.pd[i] = 0; }
        }
        if (jf >= 0) { for (s_long j = _nj; j < jf; ++j) { px[j - _nj].clear(_ptd); } _ptd->_pxmfree_r(px); return -2; }

        _column* pj2 = reinterpret_cast<_column*>(_ptd->_pxmrealloc_r(_pj, nj2 * sizeof(_column))); if (!pj2) { for (s_long j = _nj; j < nj2; ++j) { px[j - _nj].clear(_ptd); } _ptd->_pxmfree_r(px); return -2; }
        _pj = pj2; for (s_long j = _nj; j < nj2; ++j) { _pj[j].init_move_from(px[j - _nj]); } _ptd->_pxmfree_r(px);
        _nj = nj2;
      }
    }
    s_long xc(0); s_long cap = m::_cap_j(_last_j); s_long k1(0); T* px(0); if (_size_k < cap) { _column& c = _pj[_last_j]; px = c.at_ku<T>(_size_k, cap); k1 = cap - c.kbase; }

    do
    {
      if (_size_k < cap) { _copy_1u_t<TF, false>::F(_ptd, px, x, xc); if (xc) { break; } ++_size_k; }
        else { s_long j2 = _last_j + 1; cap = m::_cap_j(j2); _pj[j2].get_pd_kbase(px, k1); px += k1; k1 = cap - k1;
          _copy_1u_t<TF, false>::F(_ptd, px, x, xc); if (xc) { break; } ++_last_j; _size_k = 1; }
      ++px; if (_size_k == k1) { px -= cap; } ++_n;
    } while (_n < n2);

    if (xc)
    {
      if (_ptd->dmode) { for (s_long i = _n - 1; i >= n_prev; ++i) { if (_ptd->p_destroy_1(pval_0u<T>(i)) < 0) { if (_nxf >= 0) { _nxf += _xd; } } } }
      _set_size(n_prev); _reserve_decr_nj_u(nj_prev); return -3;
    }

    return m;
  }

    // Inserts m >= 0 copies of x at pos. ind = [nbase()..nbase()+n()].
    //    Unlike in el_insert_1, all elements starting from ind are linearly moved forward by m positions.
    //    The former i-th element will reside at the same memory address as the former (i+m)-th element,
    //    or at a new memory location if i+m >= former nbase()+n().
    //    Then, the m freed positions are filled with copies of x. Their memory address does not change.
    //    This takes O(N).
    // NOTE el_insert_ml also accepts as x an arbitrary functor like meta:: construct_f<T> (see).
    //    On invocation, positions are initialized in forward order (ind, ind+1...).
    // NOTE Using el_insert_ml should be preferred to el_insert_1 in the following cases:
    //    a) max(m, nbase() + n() - ind) >= n() ^ 0.5.
    //    b) Movement of the existing elements must be linear, as described above.
    //    c) Inserting m >= 2 elements must be single transaction.
    // NOTE The operation allocates (nbase() + n() - ind +1) * sizeof(T) bytes dyn. memory
    //    when transaction requires backing up the existing objects.
    // Returns:
    //    m (>=1): m elements have been inserted successfully.
    //    m (== 0): Nothing done.
    //    -1 on parameter checks error (integrity, x type, ind/m range). Nothing done.
    //    -2: failed before insertion. Not enough memory or an assertion failed. Nothing done.
    //    -3: failed during transcational insertion (dflt. for all types).
    //        All changes are CANCELLED. The place reserve is also reverted.
    //        (In case of appending 1 element (m == 1 and ind == nbase() + n()),
    //        the place reserve may be left a bit larger than it was, because of using el_append.)
    //    -4: failed during non-transactional insertion.
    //        Operation is forcibly COMPLETED. n() is increased by m. Integrity state is set to -1.
    //        >=1 arbitrary element may be left with invalid value.
    //      NOTE It is not recommended to create type configurations allowing for unsafe non-transactional insertion.
    //      NOTE On appending (ind == nbase() + n()) the operation is always transactional,
    //        regardless of T configuration.
  template<class TF>
  s_long el_insert_ml(s_long ind, s_long m, const TF& x) throw()
  {
    typedef typename meta::resolve_TF<TF, meta::tag_construct>::t T;
    typedef bytes::type_index_t<T> t_si;
    _yk_reg s_long r = _t_ind; if (r != t_si::xind) { if (r != t_si::ind()) { return -1; } }
    r = ind; r -= _nbase;
    if (r >= _n) // possibly appending
    {
      if (r != _n) { return -1; }
      r = m; if (r == 1) { if (el_append<TF>(x)) { return 1; } else { return -3; } }
      if (r <= 0) { if (r == 0) { return 0; } return -1; }
      r += _n; if (r < 0) { return -2; }

        // NOTE The following appending proc. is part of el_append_m.
      s_long n2 = r; s_long nj_prev = _nj; s_long n_prev = _n;
      if (_ptd->cmode == 0) { if (!meta::resolve_TF<TF, meta::tag_construct>::use_functor) { return -2; } } if ( ! _reserve_incr_u(n2) ) { return -2; }
      s_long xc(0); s_long cap = m::_cap_j(_last_j); s_long k1(0); T* px(0); if (_size_k < cap) { _column& c = _pj[_last_j]; px = c.at_ku<T>(_size_k, cap); k1 = cap - c.kbase; }

      do
      {
        if (_size_k < cap) { _copy_1u_t<TF, false>::F(_ptd, px, x, xc); if (xc) { break; } ++_size_k; }
          else { s_long j2 = _last_j + 1; cap = m::_cap_j(j2); _pj[j2].get_pd_kbase(px, k1); px += k1; k1 = cap - k1;
            _copy_1u_t<TF, false>::F(_ptd, px, x, xc); if (xc) { break; } ++_last_j; _size_k = 1; }
        ++px; if (_size_k == k1) { px -= cap; } ++_n;
      } while (_n < n2);

      if (xc)
      {
        if (_ptd->dmode) { for (s_long i = _n - 1; i >= n_prev; ++i) { if (_ptd->p_destroy_1(pval_0u<T>(i)) < 0) { if (_nxf >= 0) { _nxf += _xd; } } } }
        _set_size(n_prev); _reserve_decr_nj_u(nj_prev); return -3;
      }

      return m;
    }
    if (r < 0) { return -1; }
    ind = r;
    r = m; if (r <= 0) { if (r == 0) { return 0; } return -1; }
    r += _n; if (r < 0) { return -2; }

    s_long n2 = r; s_long nj_prev = _nj; s_long n_prev = _n;

    s_long xc(0);
    if (_ptd->op_flags & 0x4) // tr. alg. when moving may fail
    {
      if (_ptd->cmode == 0) { return -2; }
      bool cancel(false); s_long m1 = n_prev - ind;
      char* bu = _tr_bu_alloc_t<T>(m1 + 1); if (!bu) { return -2; } char* bu0 = bu + meta::t_pdiff(m1) * sizeof(T);
      if (!el_expand_n(n2)) { _tr_bu_free(bu); return -2; }

      s_long q(0); s_long q0(0); s_long q1(0); s_long m0(m1); if (m < m0) { m0 = m; }
      link1_t<T, true> ls(*this); ls.set_ind0(n_prev - 1); link1_t<T, false> ld(*this); ld.decr();
      for (s_long i = 0; i < m0; ++i) { _copy_1u_t<T, false>::F(_ptd, ld.pval(), *ls.pval(), xc); if (xc) { cancel = true; break; } ++q0; ls.decr(); ld.decr(); }
      if (!cancel) { for (s_long i = m0; i < m1; ++i) { if (!_tr_copy_1_t<T>(ld.pval(), ls.pval(), bu, q)) { cancel = true; break; } ls.decr(); ld.decr(); } }
      if (!cancel) { ld.set_ind0(ind); while (q < m1) { if (!_tr_copy_1_t<TF>(ld.pval(), &x, bu, q)) { cancel = true; break; } ld.incr(); } }
      if (!cancel) { for (s_long i = m1; i < m; ++i) { _copy_1u_t<TF, false>::F(_ptd, ld.pval(), x, xc); if (xc) { cancel = true; break; } ++q1; ld.incr(); } }

      if (cancel)
      {
        if (_ptd->dmode)
        {
          if (q1 > 0) { ld.set_ind0(n_prev); do { if (_ptd->p_destroy_1(ld.pval()) < 0) { if (_nxf >= 0) { _nxf += _xd; } } ld.incr(); --q1; } while (q1 > 0); }
          if (q0 > 0) { ld.set_ind0(n2 - 1); do { if (_ptd->p_destroy_1(ld.pval()) < 0) { if (_nxf >= 0) { _nxf += _xd; } } ld.decr(); --q0; } while (q0 > 0); }
        }
        q0 = 0; if (q0 < q) { ld.set_ind0(n_prev - 1); for (s_long i = m0; i < m1; ++i) { _tr_revert_t<T>(ld.pval(), bu, q0); ld.decr(); ++q0; if (q0 >= q) { break; } } }
        if (q0 < q) { ld.set_ind0(ind); do { _tr_revert_t<T>(ld.pval(), bu, q0); ld.incr(); ++q0; } while (q0 < q); }
        _tr_bu_free(bu); _set_size(n_prev); _reserve_decr_nj_u(nj_prev);
        return -3;
      }

      if (_ptd->dmode)
      {
        q = 0; ld.set_ind0(n_prev - 1); for (s_long i = m0; i < m1; ++i) { _tr_destroy_prev_t<T>(ld.pval(), bu, q, bu0); ++q; ld.decr(); }
        ld.set_ind0(ind); while (q < m1) { _tr_destroy_prev_t<T>(ld.pval(), bu, q, bu0); ++q; ld.incr(); }
      }
      _tr_bu_free(bu);
      return m;
    }
    else if ((_ptd->op_flags & 0x8) || ((_ptd->op_flags & 0x2) && meta::resolve_TF<TF, meta::tag_construct>::use_functor)) // tr. alg. when only copy may fail
    {
      if (_ptd->cmode == 0) { if (!meta::resolve_TF<TF, meta::tag_construct>::use_functor) { return -2; } }
      s_long m1 = n_prev - ind;
      if (!el_expand_n(n2)) { return -2; }

      s_long q(0); s_long q2(0);
      link1_t<T, false> ls(*this); ls.set_ind0(n_prev - 1); link1_t<T, false> ld(*this); ld.decr();
      while (q < m1) { _ptd->p_move_1(ld.pval(), ls.pval()); ++q; ls.decr(); ld.decr(); }
      ld.set_ind0(ind); s_long xc(0); while (q2 < m) { _copy_1u_t<TF, false>::F(_ptd, ld.pval(), x, xc); if (xc) { break; } ++q2; ld.incr(); }

      if (xc) // cancelling
      {
        if (q2 > 0 && _ptd->dmode) { ld.set_ind0(ind); do { if (_ptd->p_destroy_1(ld.pval()) < 0) { if (_nxf >= 0) { _nxf += _xd; } } --q2; ld.incr(); } while (q2 > 0); }
        if (q > 0) { ld.set_ind0(ind); ls.set_ind0(ind + m); do { _ptd->p_move_1(ld.pval(), ls.pval()); --q; ls.incr(); ld.incr(); } while (q > 0); }
        _set_size(n_prev); _reserve_decr_nj_u(nj_prev);
        return -3;
      }
      return m;
    }
    else // non-transactional op.
    {
      if (_ptd->cmode == 0) { if (!meta::resolve_TF<TF, meta::tag_construct>::use_functor) { return -2; } }
      if (!el_expand_n(n2)) { return -2; }
      link1_t<T, false> ls(*this); link1_t<T, false> ld(*this); ls.set_ind0(n_prev - 1); ld.decr();
      s_long m1 = n_prev - ind;
      while (m1)
      {
        s_long q = ls.ncontig_bw(); s_long q2 = ld.ncontig_bw(); if (q2 < q) { q = q2; } if (m1 < q) { q = m1; } q2 = q - 1;
        _move_nu_t<T>(ld.pval() - q2, ls.pval() - q2, q, _nxf, xc);
        ls.move_by(-q); ld.move_by(-q); m1 -= q;
      }
      ld.set_ind0(ind);
      for (s_long m2 = 0; m2 < m; ++m2) { _copy_1u_t<TF, true>::F(_ptd, ld.pval(), x, xc); ld.incr(); }
      if (xc) { _setf_unsafe(true); _nx_add(_nxf, xc >> _xsh); }
      return xc ? -4 : m;
    }
  }

    // Removes m >= 0 elements starting at pos. ind = [nbase()..nbase()+n()-m].
    //    Unlike in el_remove_1, elements at pos. >= ind + m (if any left) are linearly moved backward by m positions.
    //    The former i-th element will reside at the same memory address as the former (i-m)-th element.
    //    This takes O(N).
    //    If ind + m == nbase()+n(), no elements are moved. Only the last m elements are freed.
    // NOTE On can_shrink() == true, after elements removal, free place at the end of the container
    //    is returned back to system.
    // NOTE Using el_remove_ml should be preferred to el_remove_1 in the following cases:
    //    a) nbase() + n() - ind >= n() ^ 0.5.
    //    b) Movement of the existing elements must be linear, as described above.
    //    c) Removing m >= 2 elements must be single transaction.
    // NOTE The operation allocates (nbase() + n() - ind +1) * sizeof(T) bytes dyn. memory
    //    when transaction requires backing up the existing objects.
    // Returns:
    //    m (>=1): m elements have been removed successfully.
    //      NOTE Under normal conditions, on removing at the end (ind == nbase() + n() - m),
    //        the operation always succeeds.
    //        (Additionally, nexc() > 0 may indicate that one or more element destructors have failed.)
    //    m (== 0): Nothing done.
    //    -1 on parameter checks error (integrity, x type, ind/m range). Nothing done.
    //    -2: failed before removal. Not enough memory or an assertion failed. Nothing done.
    //    -3: failed during transcational removal (dflt. for all types).
    //        All changes are CANCELLED.
    //    -4: failed during non-transactional removal.
    //        Operation is forcibly COMPLETED. n() is decreased by m. Integrity state is set to -1.
    //        >=1 arbitrary element may be left with invalid value.
    //        (On move_last == true, only the element [ind] may be left with invalid value.)
    //      NOTE It is not recommended to create type configurations allowing for unsafe non-transactional removal.
  template<class T>
  s_long el_remove_ml(s_long ind, s_long m) throw()
  {
    typedef bytes::type_index_t<T> t_si;
    _yk_reg s_long r = _t_ind; if (r != t_si::xind) { if (r != t_si::ind()) { return -1; } }
    r = ind; r -= _nbase; if (r < 0) { return -1; } if (r > _n) { return -1; } ind = r;
    r = m; if (r <= 0) { if (r == 0) { return 0; } return -1; }
    r += ind; if (r < 0) { return -1; }
    if (r >= _n) // possibly removing at the end
    {
      if (r != _n) { return -1; }
      if (_ptd->dmode) { link1_t<T, false> ld(*this); for (s_long i = 0; i < m; ++i) { ld.decr(); if (_ptd->p_destroy_1(ld.pval()) < 0) { if (_nxf >= 0) { _nxf += _xd; } } } }
        _set_size(_n - m); _try_shrink(); return m;
    }

    s_long xc(0);
    if (_ptd->op_flags & 0x4)
    {
      if (_ptd->cmode == 0) { return -2; }
      bool cancel(false); s_long m1 = _n - ind - m;
      char* bu = _tr_bu_alloc_t<T>(m1 + 1); if (!bu) { return -2; } char* bu0 = bu + meta::t_pdiff(m1) * sizeof(T);

      s_long q(0); link1_t<T, true> ls(*this); link1_t<T, false> ld(*this); ls.set_ind0(ind + m); ld.set_ind0(ind);
      for (s_long i = 0; i < m1; ++i) { if (!_tr_copy_1_t<T>(ld.pval(), ls.pval(), bu, q)) { cancel = true; break; } ls.incr(); ld.incr(); }
      if (cancel)
      {
        if (q) { ld.set_ind0(ind); for (s_long i = 0; i < q; ++i) { _tr_revert_t<T>(ld.pval(), bu, i); ld.incr(); } }
        _tr_bu_free(bu); return -3;
      }

      if (_ptd->dmode)
      {
        ld.set_ind0(ind); for (s_long i = 0; i < m1; ++i) { _tr_destroy_prev_t<T>(ld.pval(), bu, i, bu0); ld.incr(); }
        s_long m2 = _n - ind; for (s_long i = m1; i < m2; ++i) { if (_ptd->p_destroy_1(ld.pval()) < 0) { if (_nxf >= 0) { _nxf += _xd; } } ld.incr(); }
      }
      _tr_bu_free(bu); _set_size(_n - m); _try_shrink();
      return m;
    }
    else // non-transactional op.
    {
      if (_ptd->mmode == 0) { return -2; }
      link1_t<T, false> ls(*this); link1_t<T, false> ld(*this); ls.set_ind0(ind + m);
      if (_ptd->dmode) { ld.set_ind0(ind); do { if (_ptd->p_destroy_1(ld.pval()) < 0) { if (_nxf >= 0) { _nxf += _xd; } } ld.incr(); } while (!ld.is_eq(ls)); }
      ld.set_ind0(ind);
      s_long m1 = _n - ind - m;
      while (m1)
      {
        s_long q = ls.ncontig_fw(); s_long q2 = ld.ncontig_fw(); if (q2 < q) { q = q2; } if (m1 < q) { q = m1; }
        _move_nu_t<T>(ld.pval(), ls.pval(), q, _nxf, xc);
        ls.move_by(q); ld.move_by(q); m1 -= q;
      }
      if (xc) { _setf_unsafe(true); _nx_add(_nxf, xc >> _xsh); }
      _set_size(_n - m); _try_shrink();
      return xc ? -4 : m;
    }
  }

// vecm pval()
//==section 7==============================================

    // Get element by ind = [nbase()..nbase()+n()-1].
    //    Returns 0 on type error or ind out of range.
  template<class T>
  inline T* pval(s_long ind) const throw()
  {
    enum { p1 = m::p1, p2 = m::p2, p3 = m::p3, m1 = m::m1, m2 = m::m2, c1 = m::c1, c2 = m::c2, c3 = m::c3, n1 = m::n1, n2 = m::n2, d2 = m::d2, d3 = m::d3, ind3 = m::ind3, mask1 = m::mask1, mask2 = m::mask2, mask3 = m::mask3 };
    typedef bytes::type_index_t<T> t_si;
    _yk_reg s_long ra = _t_ind; if (ra != t_si::xind) { if (ra != t_si::ind()) { return 0; } }
    ra = ind; ra -= _nbase; if (ra < 0)  { return 0; } if (ra >= _n)  { return 0; }
    _yk_reg s_long rb = ra; _yk_reg char* c = reinterpret_cast<char*>(_pj);
    if (ra & ~s_long(n2-1)) { ra >>= p3; ra += d3; ra *= sizeof(_column); c += ra; rb += reinterpret_cast<_column*>(c)->kbase; rb &= mask3; rb *= sizeof(T); c = reinterpret_cast<_column*>(c)->pd; c += rb; return reinterpret_cast<T*>(c); }
    else
    { if (ra & ~s_long(n1-1)) { ra >>= p2; ra += d2; ra *= sizeof(_column); c += ra; rb += reinterpret_cast<_column*>(c)->kbase; rb &= mask2; rb *= sizeof(T); c = reinterpret_cast<_column*>(c)->pd; c += rb; return reinterpret_cast<T*>(c); }
      else { ra >>= p1; ra *= sizeof(_column); c += ra; rb += reinterpret_cast<_column*>(c)->kbase; rb &= mask1; rb *= sizeof(T); c = reinterpret_cast<_column*>(c)->pd; c += rb; return reinterpret_cast<T*>(c); }
    }
  }

    // Get element by 0-based i, range unchecked.
    //    Fastest access to elements.
  template<class T>
  inline T* pval_0u(s_long i) const throw()
  {
    enum { p1 = m::p1, p2 = m::p2, p3 = m::p3, m1 = m::m1, m2 = m::m2, c1 = m::c1, c2 = m::c2, c3 = m::c3, n1 = m::n1, n2 = m::n2, d2 = m::d2, d3 = m::d3, ind3 = m::ind3, mask1 = m::mask1, mask2 = m::mask2, mask3 = m::mask3 };
    _yk_reg s_long ra = i;
    _yk_reg s_long rb = ra; _yk_reg char* c = reinterpret_cast<char*>(_pj);
    if (ra & ~s_long(n2-1)) { ra >>= p3; ra += d3; ra *= sizeof(_column); c += ra; rb += reinterpret_cast<_column*>(c)->kbase; rb &= mask3; rb *= sizeof(T); c = reinterpret_cast<_column*>(c)->pd; c += rb; return reinterpret_cast<T*>(c); }
    else
    { if (ra & ~s_long(n1-1)) { ra >>= p2; ra += d2; ra *= sizeof(_column); c += ra; rb += reinterpret_cast<_column*>(c)->kbase; rb &= mask2; rb *= sizeof(T); c = reinterpret_cast<_column*>(c)->pd; c += rb; return reinterpret_cast<T*>(c); }
      else { ra >>= p1; ra *= sizeof(_column); c += ra; rb += reinterpret_cast<_column*>(c)->kbase; rb &= mask1; rb *= sizeof(T); c = reinterpret_cast<_column*>(c)->pd; c += rb; return reinterpret_cast<T*>(c); }
    }
  }

    // Get the first element. Returns 0 on type/size errors.
  template<class T>
  inline T* pval_first() const throw()
  {
    typedef bytes::type_index_t<T> t_si; if (_t_ind != t_si::xind) { if (_t_ind != t_si::ind()) { return 0; } }
    if (_n <= 0) { return 0; } return _pj->at_0u<T>();
  }

    // Get the last element. Returns 0 on type/size errors.
  template<class T>
  inline T* pval_last() const throw()
  {
    typedef bytes::type_index_t<T> t_si; if (_t_ind != t_si::xind) { if (_t_ind != t_si::ind()) { return 0; } }
    if (_n <= 0) { return 0; } return _pj[_last_j].at_ku<T>(_size_k-1, m::_cap_j(_last_j));
  }


  template<class T, bool is_const>
  struct link1_t
  {
    typedef T t_value;
    typedef typename meta::if_t<is_const, const t_value, t_value>::result t_x;
    typedef typename meta::if_t<is_const, const vecm, vecm>::result t_ctnr;
  protected:
    friend struct link1_t<T, !is_const>;
    t_ctnr* _pv; t_x* _px; s_long _i; s_long _i0; s_long _i2;
    bool _set_ind_0u(s_long i) throw() // returns is_out() for the given i
    {
      if (i >= _pv->_n) { _px = 0; _i = -2; return false; } if (i < 0) { _px = 0; _i = -1; return false; }
      s_long j, k, cap, kbase; m::_ind_jk(i, j, k, cap); _pv->_pj[j].template get_pd_kbase<t_x>(_px, kbase);
      _i = i; s_long q = kbase + k;
      if (q < cap) { _px += q; _i0 = i - k; q = cap - kbase; if (j == _pv->_last_j && q > _pv->_size_k) { q = _pv->_size_k; } _i2 = _i0 + q; }
        else { q -= cap; _px += q; _i0 = i - q; _i2 = _i0 + kbase; if (j == _pv->_last_j) { q = cap - _pv->_size_k; if (q > 0) { _i2 -= q; } } }
      return true;
    }
  public:

      // EDGE POSITIONS "after-end", "aend" pos. means "next position after the last element".
      //    Also, "before-beg.", "bbeg" pos. means "the position before the first element".
      //    "beg." pos. means "the position of the first element".
      //    "end" pos. term is not used.

      // CREATION / ACCESS A new link is valid (is_valid()==true) only when T == type of v's element.
      //    On is_valid() == false:
      //        pcontainer() == 0,
      //        pval() == 0,
      //        is_out() == true,
      //        is_bbeg() == false,
      //        is_aend() == true,
      //        others: undefined behavior.

      // MOTION Allowed positions are nbase-1 (before beg.), [nbase..nbase+n-1], nbase+n (after end).
      //    (In 0-based functions, allowed positions will be [-1..n].)
      //    Moving from any pos. to any other pos is hard-limited by [nbase-1..nbase+n] range.
      //    Setting a position to the left or right of that range will also limit it.

      // Creates a link to the after-end pos.
    inline link1_t(t_ctnr& v) throw() : _i0(0), _i2(0) { typedef bytes::type_index_t<t_value> t_si; if (v._t_ind != t_si::xind)  { if (v._t_ind != t_si::ind()) { _pv = 0; _px = 0; _i = -2; return; } }  _pv = &v; _px = 0; _i = -2; }
      // Creates a link to arbitrary pos. The given ind will be limited by [nbase()-1..nbase()+n()].
    inline link1_t(t_ctnr& v, s_long ind) throw() : _i0(0), _i2(0) { typedef bytes::type_index_t<t_value> t_si; if (v._t_ind != t_si::xind)  { if (v._t_ind != t_si::ind()) { _pv = 0; _px = 0; _i = -2; return; } } _pv = &v; _set_ind_0u(ind - _pv->_nbase); }
      // Creates an invalid link.
    inline link1_t() throw() : _i0(0), _i2(0) { _pv = 0; _px = 0; _i = -2; }

    inline link1_t(const link1_t<T, !is_const>& l) throw() : _pv(l._pv), _px(l._px), _i(l._i), _i0(l._i0), _i2(l._i2) { } // NOTE const to non-const conv. does not compile
    inline link1_t(const link1_t& l) throw() : _pv(l._pv), _px(l._px), _i(l._i), _i0(l._i0), _i2(l._i2) { }

    inline bool is_valid() const throw() { return bool(_pv); }
    inline bool is_out() const throw() { return !_px; }
    inline t_ctnr* pcontainer() const throw() { return _pv; } // t_ctnr is either vecm or const vecm
    inline t_x* pval() const throw() { return _px; } // t_x is either T or const T
    inline bool is_bbeg() const throw() { return _i == -1; }
    inline bool is_aend() const throw() { return _i == -2; }

      // true if two valid links point to the same allowed position in the same container.
      // true also if both links are invalid.
      // false in other cases.
    template<bool is_const_>
    inline bool is_eq(const link1_t<T, is_const_>& x) const throw() { return _i == x._i && _pv == x._pv; }

      // The current position. [nbase-1..nbase+n].
    inline s_long ind() const throw() { return _i >= -1 ? _i + _pv->_nbase : _pv->_n + _pv->_nbase; }
      // The current 0-based position. [-1..n].
    inline s_long ind0() const throw() { return _i >= -1 ? _i : _pv->_n; }

      // For all functions changing the position:
      // Returning
      //    true: the new pos. points to the valid element,
      //    false: is_out() == true.

    inline bool incr() throw() { _yk_reg s_long pos = _i; if (pos < 0) { if (pos == -2) { return false; } return _set_ind_0u(0); } ++pos; if (pos < _i2) { _i = pos; ++_px; return true; } return _set_ind_0u(pos); }
    inline bool decr() throw() { _yk_reg s_long pos = _i; if (pos < 0) { if (pos == -1) { return false; } return _set_ind_0u(_pv->_n - 1); } if (pos > _i0) { --pos; _i = pos; --_px; return true; } return _set_ind_0u(pos - 1); }
    bool move_by(s_long delta) throw()
    {
      _yk_reg s_long pos = _i;
      if (pos < 0) { if (delta >= 0) { if (pos == -2) { return false; } else { return _set_ind_0u(-1 + delta); } } else { if (pos == -1) { return false; } else { return _set_ind_0u(_pv->_n + delta); } } }
      pos += delta; if (pos >= _i0 && pos < _i2) { _i = pos; _px += delta; return true; } if (pos >=  0) { return _set_ind_0u(pos); } _i = delta > 0 ? -2 : -1; _px = 0; return false;
    }
      // Set the current position.
    inline bool set_ind(s_long ind) throw() { s_long i = ind - _pv->_nbase; if (_i >= 0 && i >= _i0 && i < _i2) { _px += i - _i; _i = i; return true; } return _set_ind_0u(i); }
      // Set the current position by 0-based index.
    inline bool set_ind0(s_long i) throw() { if (_i >= 0 && i >= _i0 && i < _i2) { _px += i - _i; _i = i; return true; } return _set_ind_0u(i); }
      // Go to before-beg. and after-end pos.
    inline void set_aend() throw() { _px = 0; _i = -2; }
    inline void set_bbeg() throw() { _px = 0; _i = -1; }


      // Recalculates the cached position parameters, based on the numeric value of the current position.
      //    For making the link valid again when the target elements sequence was modified.
      //    (Before-begin and after-end positions remain as is. Pos. of the previously valid element
      //    is either updated, or, if the number of remaining elements <= pos.,
      //    becomes the after-end pos.)
    inline void update_cpos() throw() { if (_px) { _set_ind_0u(_i); } }

      // The number of contiguous elements starting from the current pos.
      //  >= 1 if is_out() == false.
      //  0 if is_out() == true.
    inline s_long ncontig_fw() const throw() { return _i >= 0 ? _i2 - _i : 0; }

      // The number of contiguous elements starting from the current pos. backwards.
      //  >= 1 if is_out() == false.
      //  0 if is_out() == true.
    inline s_long ncontig_bw() const throw() { return _i >= 0 ? _i + 1 - _i0 : 0; }

      // construct_f interface impl.
    typedef t_value t; struct exc_construct {}; inline void f(t* p) const { if (_px) { new (p) t(*_px); } else { throw exc_construct(); } }
  };

  template<class T> inline link1_t<T, false> link1_begin() throw() { return link1_t<T, false>(*this, _nbase); }
  template<class T> inline link1_t<T, false> link1_aend() throw() { return link1_t<T, false>(*this); }
  template<class T> inline link1_t<T, true> link1_cbegin() const throw() { return link1_t<T, true>(*this, _nbase); }
  template<class T> inline link1_t<T, true> link1_caend() const throw() { return link1_t<T, true>(*this); }
};

  // NOTE This function must not be specialized, only instantiated.
template<class TA> static inline const vecm::type_descriptor& typer() throw() { struct _vx : vecm { static inline const type_descriptor& ref() { return vecm::_config_ff::_td_inst<TA>::ref(); } }; return _vx::ref(); }

template<class TA, s_long y, s_long m, s_long d, s_long h, s_long size_> struct bytes::uind_t
{
  typedef typename vecm::specf<TA>::t_value t_value;
  static const s_long size = size_ == -1 ? sizeof(t_value) : size_; // dflt. size will be sizeof(T)
  static const s_long x = - (m << 27 | ((y - 2015) & 0x7f) << 20 | d << 15 | h << 10 | (size & 0x3ff));
  enum { __check = meta::assert<(size >= 1 && size <= 1024)>::result
    + meta::assert<(y >= 2015 && m >= 1 && m <= 12 && d >= 1 && d <= 31 && h >= 0 && h <= 23)>::result };
};

template<class TA, s_long y, s_long m, s_long d, s_long h, s_long size> const s_long bytes::cmti_base_t<TA, y, m, d, h, size>::xind(uind_t<TA, y, m, d, h, size>::x);

template<class TA, s_long y, s_long m, s_long d, s_long h, s_long size> bytes::cmti_base_t<TA, y, m, d, h, size>::cmti_base_t() throw()
{
  struct _vx : vecm { static void F() throw() { vecm::_cmd_reg::reg_cm<TA>(); } };
  if (_init) { return; } _init = 1; _vx::F();
}

  // Type configuration for vecm as whole object.
  // NOTE both hashx and vecm are movable, so multilevel structures
  //  (V(H), V(V), H(H), H(V), recursively) are very efficient.
template<> struct vecm::spec<vecm> { struct aux : vecm::config_aux<vecm> { enum { nt_allowDfltConstruct = false }; };  typedef config_t<vecm, 1, 4, 2, aux> config; };
template<> struct meta::copy_t<vecm> { struct exc_copy {}; static inline void F(vecm* pdest, const vecm* psrc) { new (pdest) vecm(*psrc); if (pdest->nexc() != 0) { pdest->~vecm(); throw exc_copy(); } } };
template <class Aux> struct meta::construct_f<vecm, Aux> { typedef vecm t; typedef Aux aux; void f(t* p) const { new (p) vecm(typer<aux>, 0); } };
template <> struct meta::construct_f<vecm, meta::nothing> { };

template<class T, bool b> struct meta::type_equi<vecm::link1_t<T, b>, meta::tag_construct> { typedef meta::tag_construct t_3; };
template<class T, bool b> struct meta::type_equi<vecm::link1_t<T, b>, meta::tag_functor> { typedef meta::tag_functor t_3; };




struct hashx_common
{
  enum { no_elem = -1 };
  enum EHashOpExc { hoxOpFailed = 1 };

  template<class KA, class VA> struct _select_entry;

  template<class KA, class VA, int kind = _select_entry<KA, VA>::result> struct entry { };

    // 1-3: entries that are copied by automatically generated copy constructor.
    // 4-6: entries that are copied in special way (note: copy ctor. is not available).
    //    NOTE For all entries, custom destructors are defined. Autogenerated are inaccessible.
  template<class KA, class VA> struct entry<KA, VA, 1> { typedef typename vecm::specf<KA>::t_value t_k; typedef typename vecm::specf<VA>::t_value t_v; t_k k; mutable t_v v; s_long h; s_long q; private: entry(); ~entry(); };
  template<class KA, class VA> struct entry<KA, VA, 2> { typedef typename vecm::specf<KA>::t_value t_k; typedef typename vecm::specf<VA>::t_value t_v; t_k k; s_long h; mutable t_v v; s_long q; private: entry(); ~entry(); };
  template<class KA, class VA> struct entry<KA, VA, 3> { typedef typename vecm::specf<KA>::t_value t_k; typedef typename vecm::specf<VA>::t_value t_v; t_k k; s_long h; s_long q; mutable t_v v; private: entry(); ~entry(); };
  template<class KA, class VA> struct entry<KA, VA, 4> { typedef typename vecm::specf<KA>::t_value t_k; typedef typename vecm::specf<VA>::t_value t_v; t_k k; mutable t_v v; s_long h; s_long q; private: entry(); entry(const entry&); ~entry(); };
  template<class KA, class VA> struct entry<KA, VA, 5> { typedef typename vecm::specf<KA>::t_value t_k; typedef typename vecm::specf<VA>::t_value t_v; t_k k; s_long h; mutable t_v v; s_long q; private: entry(); entry(const entry&); ~entry(); };
  template<class KA, class VA> struct entry<KA, VA, 6> { typedef typename vecm::specf<KA>::t_value t_k; typedef typename vecm::specf<VA>::t_value t_v; t_k k; s_long h; s_long q; mutable t_v v; private: entry(); entry(const entry&); ~entry(); };

    // dkind: 0x1 == needs destructor for key, 0x2 == needs destructor for value.
  template<class KA, class VA, int kind, int dkind> struct _entry_da { };

  template<class KA, class VA> struct _select_entry
  {
    typedef vecm::specf<KA> cfgk; typedef vecm::specf<VA> cfgv;
    enum
    {
      _k = cfgk::cmode == 1 && cfgv::cmode == 1 ? 0 : 3,
      _s1 = sizeof(entry<KA, VA, _k + 1>), _s2 = sizeof(entry<KA, VA, _k + 2>), _s3 = sizeof(entry<KA, VA, _k + 3>),
      _r1 = meta::ifv_t<(_s1 <= _s2), int, 1, 2>::result, _sr1 = meta::ifv_t<(_s1 <= _s2), int, _s1, _s2>::result,
      result = _k + meta::ifv_t<(_sr1 <= _s3), int, _r1, 3>::result
    };
  };

  template<class K>
  struct _kf_0
  {
    enum { _kf_kind = 0 };

  private:
    template<class T> struct _valkind
    {
      enum
      {
            // 0 - kind still unknown
            // ========================
            // 1 - double
            // 2 - <= 4-byte known non-pointer type convertible to s_long
            // 3 - sequence of chars (default treatment for the known types that are out of other categories)
            // ========================
            // 4 - pointer <= sizeof s_long
            // 5 - 5-8-byte pointer
            // 6 - any other pointer
            // 7 - any known 8-byte integer type
            // 8 - float
            // 9 - long double

        _k8 = 8 * meta::same_t<T, float>::result,
        _k9 = _k8 ? _k8 : 9 * meta::same_t<T, long double>::result,

        _k4 = _k9 ? _k9 : 4 * (meta::is_ptr<T>::result && sizeof(T) <= sizeof(s_long)),
        _k5 = _k4 ? _k4 : 5 * (meta::is_ptr<T>::result && sizeof(T) > sizeof(s_long) && sizeof(T) <= 8),
        _k6 = _k5 ? _k5 : 6 * meta::is_ptr<T>::result,
        _k7 = _k6 ? _k6 : 7 * (sizeof(T) == 8 && bytes::config0_t<T>::isKnown1),

        _k1 = _k7 ? _k7 : 1 * (meta::same_t<T, double>::result),
        _k2 = _k1 ? _k1 : 2 * (meta::same_t<T, s_long>::result || (bytes::config0_t<T>::isKnown1 && sizeof(T) <= sizeof(s_long))),
        _k3 = _k2 ? _k2 : 3 * (bytes::config0_t<T>::isKnown1),

        result = _k3
      };
    };

    template<class T, int __vkind = _valkind<T>::result> struct _hashf_t { };

    template<class T> struct _hashf_t<T, 1> { static inline s_long F(double key) { double k[1] = { key }; _yk_reg const s_long* p = reinterpret_cast<const s_long*>(&k[0]);  return p[0] >> 17 ^ p[0] * 37 ^ p[1] >> 13 ^ p[1] * 169; } };
    template<class T> struct _hashf_t<T, 2> { static inline s_long F(s_long key) { return key >> 17 ^ key * 37; } };
    template<class T> struct _hashf_t<T, 3> { static inline s_long F(const T& key) { const T k[1] = { key }; _yk_reg s_long h = 0; _yk_reg const char* p = reinterpret_cast<const char*>(&k[0]); _yk_reg s_long n = sizeof(T); while (n > 0) { h *= 19; h -= *p++; --n; } h ^= 53970; return h; } };

    template<class T> struct _hashf_t<T, 4> { static inline s_long F(const void* p) { return _hashf_t<T, 2>::F(reinterpret_cast<const char*>(p)-static_cast<const char*>(0)); } };
    template<class T> struct _hashf_t<T, 6> { static inline s_long F(const void* p) { return _hashf_t<T, 3>::F(p); } };
    template<class T> struct _hashf_t<T, 7> { static inline s_long F(meta::s_ll key) { _yk_reg s_long h = s_long(key); h = h >> 17 ^ h * 37; _yk_reg s_long h2 = s_long(key >> 32); h2 = h2 >> 15 ^ h2 * 23; return h ^ h2; } };
    template<class T> struct _hashf_t<T, 5> { static inline s_long F(const void* p) { return _hashf_t<T, 7>::F(reinterpret_cast<const char*>(p)-static_cast<const char*>(0)); } };

    template<class T> struct _hashf_t<T, 8> { static inline s_long F(float key) { return _hashf_t<T, 1>::F(key); } };
    template<class T> struct _hashf_t<T, 9> { static inline s_long F(long double key) { if (key == 0.) { return 0; } const long double ep = 1.e-100; const long double en = -1.e-100; const long double d = 1.e40; s_long order = 0; while (key > en && key < ep) { key *= d; --order; } while (key >= d || key <= -d) { key /= d; ++order; } key *= 0.97; return _hashf_t<T, 1>::F(key) ^ _hashf_t<T, 2>::F(order); } };

  public:
    inline s_long operator () (s_long key) const { return key >> 17 ^ key * 37; }

      // NOTE (!) floating point conversions may give incorrect results,
      //    influencing key hashing and comparisons.
      //  Consider the following code:
      //      long n = 3, n2 = 30; do { double x = n, y = n2 * 0.1; std::cout << (x == y ? "EQ" : "NEQ") << ' '; --n; n2 -= 10; } while (n);
      //  It prints EQ NEQ NEQ on certain compilers.
    inline s_long operator () (double key) const { double k[1] = { key }; _yk_reg const s_long* p = reinterpret_cast<const s_long*>(&k[0]);  return p[0] >> 17 ^ p[0] * 37 ^ p[1] >> 13 ^ p[1] * 169; }

    template<class T> inline s_long operator() (const T& key) const { return _hashf_t<T>::F(key); }

    inline bool operator () (const K& x1, const K& x2) const { return x1 == x2; }
  };



  template<class S, int __kind, class T1, class T2> struct _kf_string { enum { _kf_kind = 0 }; };
  template<class S, int __kind> struct _kf_string<S, __kind, typename S::value_type, typename S::traits_type::char_type>
  {
    enum { _kf_kind = __kind }; typedef typename S::value_type t_char;
    inline s_long operator () (const S& key) const { _yk_reg s_long h = 0; _yk_reg const t_char* p = key.c_str(); _yk_reg meta::t_pdiff n = key.size(); while (n > 0) { h *= 19; h -= *p++; --n; } h ^= 53970; return h; }
    inline s_long operator () (const t_char* p0) const { _yk_reg s_long h = 0; _yk_reg const t_char* p = p0; _yk_reg t_char c; while ((c = *p++)) { h *= 19; h -= c; } h ^= 53970; return h; }
    inline bool operator () (const S& k1, const S& k2) const { return bytes::is_eq_str(k1.c_str(), k2.c_str(), k1.size(), k2.size()); }
    inline bool operator () (const S& k1, const t_char* k2) const { return bytes::is_eq_str(k1.c_str(), k2, k1.size(), -1); }
  };
  template<class K, int __kind = (_kf_string<K, 1, char, char>::_kf_kind + _kf_string<K, 2, wchar_t, wchar_t>::_kf_kind + _kf_0<K>::_kf_kind)> struct _select_kf {};
  template<class K> struct _select_kf<K, 0> : _kf_0<K> {};
  template<class K> struct _select_kf<K, 1> : _kf_string<K, 1, char, char> {};
  template<class K> struct _select_kf<K, 2> : _kf_string<K, 2, wchar_t, wchar_t> {};



  template<class S, class T1, class T2> struct _skf_string { enum { _is_pc = false, _is_string = false }; };
  template<class S> struct _skf_string<S, typename S::value_type, typename S::traits_type::char_type>
  {
    enum { _is_pc = false, _is_string = true }; typedef typename S::value_type t_char;
    static inline void Fcnew(S* p, const S& x) { new (p) S(x); }
    static inline void Fcnew(S* p, const t_char* x) { new (p) S(x); }
    static inline void Fcnew(const t_char** p, const t_char* x) { *p = x; }
    static inline void Fcnew(const t_char** p, const S& x) { *p = x.c_str(); }
    static inline s_long Fhash(const t_char* p0) { _yk_reg s_long h = 0; _yk_reg const t_char* p = p0; _yk_reg t_char c; while ((c = *p++)) { h *= 19; h -= c; } h ^= 53970; return h; }
    static inline s_long Fhash(const S& key) { _yk_reg s_long h = 0; _yk_reg const t_char* p = key.c_str(); _yk_reg s_long n = key.size(); while (n > 0) { h *= 31; h += *p++; --n; } return h; }
    static inline bool Fis_eq(const S& k1, const S& k2) { return bytes::is_eq_str(k1.c_str(), k2.c_str(), k1.size(), k2.size()); }
    static inline bool Fis_eq(const S& k1, const t_char* k2) { return bytes::is_eq_str(k1.c_str(), k2, k1.size(), -1); }
    static inline bool Fis_eq(const t_char* k1, const t_char* k2) { return bytes::is_eq_str(k1, k2, -1, -1); }
  };
  template<class C> struct _skf_pchars
  {
    enum { _is_pc = true, _is_string = false }; typedef C t_char;
    static inline void Fcnew(const t_char** p, const t_char* x) { *p = x; }
    static inline s_long Fhash(const t_char* p0) { _yk_reg s_long h = 0; _yk_reg const t_char* p = p0; _yk_reg t_char c; while ((c = *p++)) { h *= 19; h -= c; } h ^= 53970; return h; }
    static inline bool Fis_eq(const t_char* k1, const t_char* k2) { return bytes::is_eq_str(k1, k2, -1, -1); }
  };
  template<class T1, class T2, int __s1 = _select_kf<T1>::_kf_kind, int __s2 = _select_kf<T2>::_kf_kind> struct _select_skf {};
  template<class S> struct _select_skf<S, S, 1, 1> : _skf_string<S, char, char> {};
  template<class S> struct _select_skf<S, S, 2, 2> : _skf_string<S, wchar_t, wchar_t> {};
  template<class S, class C> struct _select_skf<S, C*, 1, 0> : _skf_string<S, char, char> {};
  template<class S, class C> struct _select_skf<S, C*, 2, 0> : _skf_string<S, wchar_t, wchar_t> {};
  template<class S, class C> struct _select_skf<C*, S, 0, 1> : _skf_string<S, char, char> {};
  template<class S, class C> struct _select_skf<C*, S, 0, 2> : _skf_string<S, wchar_t, wchar_t> {};
  template<int __s2> struct _select_skf<char*, meta::nothing, 0, __s2> : _skf_pchars<char> {};
  template<int __s2> struct _select_skf<wchar_t*, meta::nothing, 0, __s2> : _skf_pchars<wchar_t> {};
  template<class C> struct _select_skf<C*, C*, 0, 0> : _select_skf<C*, meta::nothing, 0> {};

// hashx_common kf_basic
//==section 8==============================================

    // kf_basic is a set of predefined hash functions. hashx uses it for default processing.
    // NOTE Directly, hashx calls exclusively cnew(), hash(), is_eq().
  template<class K>
  struct kf_basic : _select_kf<K>
  {
    template<class K2> inline void cnew(K* p, const K2& x) const { new (p) K(x); }
    template<class K2> inline s_long hash(const K2& x) const { return this->operator()(x); }
    template<class K2> inline bool is_eq(const K& x1, const K2& x2) const { return this->operator()(x1, x2); }
  };

    // kf_std is the single class, combining STL-style hash and equality functions for passing into hashx.
    // NOTE Directly, hashx calls exclusively cnew(), hash(), is_eq().
  template<class K, class HashF = kf_basic<K>, class Eq = kf_basic<K> >
  struct kf_std : HashF, Eq
  {
    template<class K2> inline void cnew(K* p, const K2& x) const { new (p) K(x); }
    template<class K2> inline s_long hash(const K2& x) const { return this->operator()(x); }
    template<class K2> inline bool is_eq(const K& x1, const K2& x2) const { return this->operator()(x1, x2); }
  };

    // kf_str allows hash keys to be std::string, std::wstring, const char* or const wchar_t*.
    //  If hash key is a pointer, hash function and comparison are applied to pointed strings, instead of numeric value of the pointer.
    //  If key argument type in find2, insert2, remove2 is not the same as hash key type,
    //    both hash function calculation and keys comparison work directly with the key argument,
    //    without conversion. Consider the following objects:
    //
    //    hashx<const char*, s_long,  hashx_common::kf_str<const char*> > h;
    //      const std::string s("string1");
    //      h.insert2(s, *h.pkf()); // NOTE after s going out of scope, h find/ins./rem. are unsafe, only h.hashx_clear() is safe
    //        h.find2(s, *h.pkf());
    //        h.find2(std::string("string2"), *h.pkf());
    //
    //    hashx<std::string, s_long,  hashx_common::kf_str<std::string> > h2;
    //      h2.insert2("string9", *h2.pkf());
    //        h2.find2("string9", *h2.pkf());
    //        h2.find2("string11", *h2.pkf());
    //
    //    Here, both insert2 and find2 use .c_str() and .size() for string keys
    //      to work directly with characters.
    //      "string9" in insert2 is not converted to std::string until insertion into h2.
    //      "string9" and "string11" in find2 are not converted to std::string at all, still "string9" will be found.
  template<class SP>
  struct kf_str
  {
    template<class SP2> inline void cnew(SP* p, const SP2& x) const { _select_skf<typename meta::nonc_t<SP>::t, typename meta::nonc_t<SP2>::t>::Fcnew(p, x); }
    template<class SP2> inline s_long hash(const SP2& x) const { return _select_skf<typename meta::nonc_t<SP>::t, typename meta::nonc_t<SP2>::t>::Fhash(x); }
    template<class SP2> inline bool is_eq(const SP& x1, const SP2& x2) const { return _select_skf<typename meta::nonc_t<SP>::t, typename meta::nonc_t<SP2>::t>::Fis_eq(x1, x2); }
  };

};

template<class KA, class VA, int kind, int dkind> struct meta::type_equi<hashx_common::_entry_da<KA, VA, kind, dkind>, hashx_common::entry<KA, VA, kind> > { typedef hashx_common::entry<KA, VA, kind> t_3; };


  // Mixed copy-construction of hash entries. Occurs when K and V require different ways of copying.
template<class KA, class VA, int kind>
struct meta::copy_t<hashx_common::entry<KA, VA, kind> >
{
  typedef hashx_common::entry<KA, VA, kind> E;
  struct exc_copy_entry {};
  static inline void F(E* pdest, const E* psrc)
  {
    typedef vecm::specf<VA> cfgv; typedef vecm::specf<KA> cfgk;
    if (cfgv::Fcopy_1(&pdest->v, &psrc->v) < 0) { throw exc_copy_entry(); }
    if (cfgk::Fcopy_1(&pdest->k, &psrc->k) < 0) { cfgv::Fdestroy_1(&pdest->v); throw exc_copy_entry(); }
    pdest->h = psrc->h;
    pdest->q = psrc->q;
  }
};

  // Optimized moving of hash entries. Occurs when K and V require different ways of moving.
template<class KA, class VA, int kind>
struct meta::trymove_t<hashx_common::entry<KA, VA, kind> >
{
  typedef hashx_common::entry<KA, VA, kind> E;
  static inline s_long F(E* pdest, E* psrc) throw()
  {
    typedef vecm::specf<KA> cfgk; typedef vecm::specf<VA> cfgv; typedef vecm::specf<E> cfge;
      // NOTE Processing depends on any (or both) of k and v having faultless moving.
    if (cfgk::gm)
    {
      if (cfgv::gm) { cfgk::Fmove_1(&pdest->k, &psrc->k); cfgv::Fmove_1(&pdest->v, &psrc->v); pdest->h = psrc->h; pdest->q = psrc->q; return 1; }
      else { s_long res = cfgv::Fmove_1(&pdest->v, &psrc->v); if (res < 0) { return res; } cfgk::Fmove_1(&pdest->k, &psrc->k); pdest->h = psrc->h; pdest->q = psrc->q; return res; }
    }
    else
    {
      if (cfgv::gm) { s_long res = cfgk::Fmove_1(&pdest->k, &psrc->k); if (res < 0) { return res; } cfgv::Fmove_1(&pdest->v, &psrc->v); pdest->h = psrc->h; pdest->q = psrc->q; return res; }
      else { s_long res = cfge::Fcopy_1(pdest, psrc); if (res < 0) { return -1; } res = cfge::Fdestroy_1(psrc); if (res < 0) { return 0; } return 1;  }
    }
  }
};

  // Custom destructor of hash entry. Occurs when K and/or V require non-standard destruction.
template<class KA, class VA, int kind, int dkind> struct meta::destroy_t<hashx_common::_entry_da<KA, VA, kind, dkind> > {};
  template<class KA, class VA, int kind>
  struct meta::destroy_t<hashx_common::_entry_da<KA, VA, kind, 1> >
  {
    typedef hashx_common::entry<KA, VA, kind> E; struct exc_destroy_entry {};
    static inline void F(E* p) { if (vecm::specf<KA>::Fdestroy_1(&p->k) < 0) { throw exc_destroy_entry(); } }
  };
  template<class KA, class VA, int kind>
  struct meta::destroy_t<hashx_common::_entry_da<KA, VA, kind, 2> >
  {
    typedef hashx_common::entry<KA, VA, kind> E; struct exc_destroy_entry {};
    static inline void F(E* p) { if (vecm::specf<VA>::Fdestroy_1(&p->v) < 0) { throw exc_destroy_entry(); } }
  };
  template<class KA, class VA, int kind>
  struct meta::destroy_t<hashx_common::_entry_da<KA, VA, kind, 3> >
  {
    typedef hashx_common::entry<KA, VA, kind> E; struct exc_destroy_entry {};
    static inline void F(E* p) { if (vecm::specf<VA>::Fdestroy_1(&p->v) < 0 || vecm::specf<KA>::Fdestroy_1(&p->k) < 0) { throw exc_destroy_entry(); } }
  };

  // Safe moving hash entries. Occurs when both K and V allow safe moving, but by different methods.
template<class KA, class VA, int kind>
struct meta::safemove_t<hashx_common::entry<KA, VA, kind> >
{
  typedef hashx_common::entry<KA, VA, kind> E;
  static inline void F(E* pdest, E* psrc) throw()
  {
    typedef vecm::specf<VA> cfgv; typedef vecm::specf<KA> cfgk;
    cfgk::Fmove_1(&pdest->k, &psrc->k); cfgv::Fmove_1(&pdest->v, &psrc->v); pdest->h = psrc->h; pdest->q = psrc->q;
  }
};





  // hashx expects factual data types or equivalents for KA, VAF.
  // Additionally, VAF may be a functor for creating a value.
  //    Functor instance is created per each hashx object (see pctf() in hashx).
  //    In this case, hash value type is VAF::t.
  //  NOTE If VAF is not a functor, the VAF is resolved to value type (t_v) first.
  //    For dflt. creating values, construct_f<t_v> will be called.
  //  NOTE If both functor and alt. config. for a value type are necessary,
  //    a compound functor may be defined and passed as VAF arg. For example:
  //
  //        struct newX_alt : meta::construct_f<X> { typedef X_alt t; };
  //          type_equi<newX_alt, tag_construct> { typedef tag_construct t_3; };
  //          type_equi<newX_alt, tag_functor> { typedef tag_functor t_3; };
  //        typedef hashx<some key, newX_alt, ...> H;
  //
  //    Here, H will have value of type X, meta::construct_f<X>::f() as X default constructor,
  //      and vecm::spec<X_alt>::config as X configuration (mainly, its [dmc]mode).
  //      (This implies that X_alt is correctly declared as equivalent for X.
  //      See vecm(ptyper p, s_long base) for descr.)
  //  NOTE If hash value is a vecm containing particular T, VAF must be construct_f<vecm, T> or analogous.
  //    This ensures uniform dflt. creating vecm objects with the same value type in each new entry.
  //    (If the client needs to keep unspecified vecm objects in the hash
  //    and manually control their element types,
  //    it should provide specialization of construct_f<vecm, C>,
  //    where C is any client-side class.
  //  NOTE The default construct_f<vecm, meta::nothing> cannot be compiled.
template<class KA, class VAF, class Kf = hashx_common::kf_basic<typename vecm::specf<KA>::t_value> >
struct hashx : protected vecm
{
  typedef hashx t_hashx;
  typedef KA t_ka; // key or its type equivalent
  typedef typename meta::resolve_TF<VAF, meta::tag_construct>::t t_va; // value or its type equivalent
  typedef vecm::specf<t_ka> cfgk; typedef vecm::specf<t_va> cfgv; // type configurations for key and value
  typedef typename cfgk::t_value t_k; // key
  typedef typename cfgv::t_value t_v; // value
  typedef typename meta::if_t<meta::resolve_TF<VAF, meta::tag_construct>::use_functor, VAF, meta::construct_f<t_v> >::result f_ctf; // construction functor for value
  typedef Kf f_kf; // hash functions, key comparisons, possibly key type conversions
  typedef hashx_common::entry<t_ka, t_va> entry; // hash map entry
  typedef vecm::specf<entry> cfge; // type configuration for entry
  enum { no_elem = hashx_common::no_elem };

// hashx()
//==section 9==============================================

    // Constructing an empty container.
  inline hashx() throw() : vecm(typer<entry>, 0), _ht(typer<s_long>, 0), _pz(0) { _d_reinit(); }

  inline ~hashx() throw() { _d_destroy(); }

    // Removes all entries, clears hash table, frees the reserved place, sets everything to default, sets nexc() == 0.
    //  Returns: The previous value of nexc() (it may indicate errors during clearing).
  inline s_long hashx_clear() throw() { _d_reinit(); s_long nexc0 = nexc(); _set0_nexc(); return nexc0; }

    // Copying the hash table and all entries.
    //  NOTE this->can_shrink() is set to dflt. true (not copied). this->nrsv() is set to min. value (not that of x).
    //  Results:
    //    a) If copying succeeds,
    //          *this contains a copy of x.
    //          nexc() == 0.
    //          integrity() == 1.
    //    b) If copying is disallowed for entry (e.g. for k or v) (see copierMode 0 in vecm config_t),
    //          *this is empty, with dflt. key fn. and space reserve.
    //          nexc() == 0.
    //          integrity() == 1.
    //    c) If copying fails,
    //          *this is empty, with dflt. key fn. and space reserve.
    //          nexc():
    //              == 1 (either x < 0, or copying failed),
    //              >= 2 up to negative (copying failed + >=1 dtors. failed on cancelling).
    //          integrity() == 0.
    //    d) If primary initialization failed,
    //          *this is empty. It will be reinitialized automatically on the next call of a public function.
    //          nexc() >= 1.
    //          integrity() == 0.
  hashx(const hashx& x) throw() : vecm(typer<entry>, 0), _ht(typer<s_long>, 0), _pz(0)
  {
    _d_reinit(); if (!_pz || x.integrity() < 0) { _nxf = (_nxf & _fm) | _xd; return; } if (cfge::cmode == 0) { return; }
    t_dyndata* _pz2 = Fz_new_copy(_ht.ptd(), x._pz); if (!_pz2) { _nxf = (_nxf & _fm) | _xd; return; }
    if (_ht.vecm_copy(x._ht, true) != 1) { Fz_del(_ht.ptd(), _pz2, _nxf); return; }
    if (vecm_copy(x, true) != 1) { _ht.el_expand_n(_base_min); for (vecm::link1_t<s_long, false> l = _ht.link1_begin<s_long>(); !l.is_aend(); l.incr()) { *l.pval() = no_elem; } Fz_del(_ht.ptd(), _pz2, _nxf); return; }
    _base = x._base; _mask = x._mask; _base2 = x._base2; _mask2 = x._mask2; _rhdir = x._rhdir; _rhind = x._rhind;
    s_long bc(0); Fz_del(_ht.ptd(), _pz, bc); _pz = _pz2;
  }

    // Copying x to *this.
    //  is_tr true: transactional copying. If it fails, all changes are cancelled.
    //      NOTE The old object is destroyed only after the new has been created.
    //  is_tr false: the operation behaves like ~hashx() + hashx(const hashx&).
    //      It does not cancel changes if anything fails during copying.
    //      NOTE (!) is_tr false may not be used for copying between container and its own element,
    //        even indirectly. (This is possible with polymorphic types.)
    //  NOTE this->can_shrink() is set to dflt. true (not copied or preserved). this->nrsv() is set to min. value (not that of x or preserved).
    //  Returns:
    //    1 on on_calm_result. (Also if this == &x.)
    //        nexc() may be > 0 if a number of failures occured in the old object during destruction.
    //    0 (only on is_tr == true) the operation failed, changes are cancelled.
    //        nexc() is increased by 1 + number of exceptions in destructors
    //        of the temporary objects.
    //    -1 (only on is_tr == false) the operation is completed. The old object is destroyed.
    //        The new object is initialized as hashx(x), but contains no entries.
    //        nexc() is increased by the errors during copying + possible errors during the old object destruction.
    //    -2 the function is called for a null pointer.
  s_long hashx_copy(const hashx& x, bool is_tr) throw()
  {
    if (!this) { return -2; } if (this == &x) { return 1; } typedef t_hashx Q; enum { _nq = sizeof(Q) };
    if (is_tr) { union { meta::s_ll _st; char _stc[_nq]; }; Q* p = reinterpret_cast<Q*>(&_st); new (p) Q(x); if (p->nexc()) { p->~Q(); _nx_add(_nxf, p->nexc()); return 0; } _set0_nexc(); this->~Q(); s_long ec = nexc(); bytes::memmove_t<Q>::F(this, p, _nq); _nx_add(_nxf, ec); return 1; }
      else { _set0_nexc(); this->~Q(); s_long ec = nexc(); new (this) Q(x); s_long res = nexc() ? -1 : 1; _nx_add(_nxf, ec); return res; }
  }

// hashx find()
//==section 10==============================================

    // Finding an entry with key equal to k.
    //  Returns:
    //    a) a non-0 pointer when found a valid entry with the given key.
    //    b) 0 in all other cases.
  inline const entry* find(const t_k& k) const throw() { if (!_pz) { return 0; }  entry* e; s_long ind; try { _find(k, _pz->_kf.hash(k), _pz->_kf, e, ind); } catch (...) { return 0; } return e; }

    // Inserting an entry with key equal to k.
    //  Returns:
    //    a) 1 - the element was inserted. ret_pentry, ret_pind point to it.
    //    b) 0 - the element already exists. ret_pentry, ret_pind point to it.
    //    c) -1 - failed to insert element (e.g. memory allocation error). No changes to hash.
    //    d) -2 - internal error. No changes.
    //  In cases c, d *ret_pentry is set to 0, *ret_pind to no_elem.
  inline s_long insert(const t_k& k, const entry** ret_pentry = 0, s_long* ret_pind = 0) throw() { if (!_pz) { if (!_d_reinit()) { if (ret_pentry) { *ret_pentry = 0; } if (ret_pind) { *ret_pind = no_elem; } return -2; } } entry* e; s_long ind, res; try { res = _insert(k, _pz->_kf.hash(k), _pz->_kf, e, ind); } catch (...) { e = 0; ind = no_elem; res = -2; } if (ret_pentry) { *ret_pentry = e; } if (ret_pind) { *ret_pind = ind; }  return res; }

    // Removing an entry with key equal to k.
    //  Returns:
    //    1 - the element existed and was removed.
    //    0 - the element did not exist.
    //    -1 - failed to remove the existing element (memory allocation error on non-trivial moving). No changes.
    //    -2 - internal error. No changes.
  inline s_long remove(const t_k& k) throw() { if (!_pz) { if (!_d_reinit()) { return -2; } } if (_n == 0) { return 0; } try { return _remove(k, _pz->_kf.hash(k), _pz->_kf); } catch (...) { return -2; } }

    // Accessing an entry with key equal to k, with automatic insertion.
    //  Returns:
    //      a) a reference to the value, corresponding to the newly inserted or existing key.
    //      b) throws hoxOpFailed when failed to insert (e.g. memory allocation / object construction error).
  inline t_v& operator[](const t_k& k) throw (hashx_common::EHashOpExc) { if (!_pz) { if (!_d_reinit()) { throw hashx_common::hoxOpFailed; } } entry* e; s_long ind; try { _insert(k, _pz->_kf.hash(k), _pz->_kf, e, ind); } catch (...) { e = 0; } if (e) { return e->v; } throw hashx_common::hoxOpFailed; }

    // The number of entries in the hash.
  inline s_long n() const throw() { return _n; } // >=0

    // Accessing the entry by sequential ind == [0..n()-1].
    //  Returns:
    //    a) a non-null pointer to a valid entry.
    //    b) 0 on ind out of range or any error.
  inline const entry* operator()(s_long ind) const throw() { return pval<entry>(ind); }

// hashx find2()
//==section 11==============================================

    // Finding an entry with key equal to k.
    //    K2 may differ from K.
    //    kf.hash(const K2&) must be defined.
    //    kf.is_eq(const K&, const K2&) must be defined.
    //  If ph != 0, *ph is taken as ready hash value (must be correct).
    //      If ph == 0, hash value is kf.hash(k).
    // Returns:
    //    a) 1 - the element exists. ret_pentry, ret_pind point to it.
    //    b) 0 - the element does not exist.
    //    c) -1 - failed during search.
    //    d) -2 - internal error. Could not perform search.
    //  In cases b, c, d *ret_pentry is set to 0, *ret_pind to no_elem.
  template<class K2, class Kf_> inline s_long find2(const K2& k, const Kf_& kf, const s_long* ph = 0, const entry** ret_pentry = 0, s_long* ret_pind = 0) const throw() { if (!_pz) { if (ret_pentry) { *ret_pentry = 0; } if (ret_pind) { *ret_pind = no_elem; } return -2; } entry* e; s_long ind, res; try { res = _find(k, ph ? *ph : kf.hash(k), kf, e, ind); } catch (...) { e = 0; ind = no_elem; res = -2; } if (ret_pentry) { *ret_pentry = e; } if (ret_pind) { *ret_pind = ind; } return res; }

    // Inserting an entry with key equal to k.
    //    K2 may differ from K.
    //    kf.hash(const K2&) must be defined.
    //    kf.is_eq(const K&, const K2&) must be defined.
    //    kf.cnew(K*, const K2&) must be defined, constructing K from K2.
    //  If ph != 0, *ph is taken as ready hash value (must be correct).
    //      If ph == 0, hash value is kf.hash(k).
    // Returns:
    //    a) 1 - the element was inserted. ret_pentry, ret_pind point to it.
    //    b) 0 - the element already exists. ret_pentry, ret_pind point to it.
    //    c) -1 - failed to insert element (e.g. memory allocation error). The container is not changed.
    //    d) -2 - internal error. The container is not changed.
    //  In cases c, d *ret_pentry is set to 0, *ret_pind to no_elem.
  template<class K2, class Kf_> inline s_long insert2(const K2& k, const Kf_& kf, const s_long* ph = 0, const entry** ret_pentry = 0, s_long* ret_pind = 0) throw() { if (!_pz) { if (!_d_reinit()) { if (ret_pentry) { *ret_pentry = 0; } if (ret_pind) { *ret_pind = no_elem; } return -2; } } entry* e; s_long ind, res; try { res = _insert(k, ph ? *ph : kf.hash(k), kf, e, ind); } catch (...) { e = 0; ind = no_elem; res = -2; } if (ret_pentry) { *ret_pentry = e; } if (ret_pind) { *ret_pind = ind; }  return res; }

    // Removing the entry with key equal to k.
    //    K2 may differ from K.
    //    kf.hash(const K2&) must be defined.
    //    kf.is_eq(const K&, const K2&) must be defined.
    //  If ph != 0, *ph is taken as ready hash value (must be correct).
    //      If ph == 0, hash value is kf.hash(k).
    // Returns:
    //    1 - the element existed and was removed.
    //    0 - the element did not exist.
    //    -1 - failed to remove the existing element (e.g. memory allocation error). The container is not changed.
    //    -2 - internal error. The container is not changed.
  template<class K2, class Kf_> inline s_long remove2(const K2& k, const Kf_& kf, const s_long* ph = 0) throw() { if (!_pz) { if (!_d_reinit()) { return -2; } } if (_n == 0) { return 0; } try { return _remove(k, ph ? *ph : kf.hash(k), kf); } catch (...) { return -2; } }

    // Removing the entry given by 0-based index or pointer.
    //    For on_calm_result, the entry must belong to this container.
    // Returns:
    //    1 - the entry was removed.
    //    0 - the entry does not exist (ind is out of range, e == 0 or not from this container).
    //    -1 - failed to remove the given entry. The container is not changed.
    //    -2 - internal error. The container is not changed.
  inline s_long remove_i(s_long ind) throw() { if (!_pz) { if (!_d_reinit()) { return -2; } } return _remove_e(pval<entry>(ind)); }
  inline s_long remove_e(const entry* e) throw() { if (!_pz) { if (!_d_reinit()) { return -2; } } return _remove_e(e); }

    // Removing all elements.
    //    The space left is freed if can_shrink is true.
    //    Unlike hashx_clear, the function keeps can_shrink, nexc, key functor etc. unchanged.
    // Returns:
    //  >0 on on_calm_result - the number of entries removed (previous n()).
    //      nexc() may be increased by the number of errors occured during elements destruction.
    //  0 if the container was empty (n() == 0). Nothing is done,
    //      except possible storage space reserve freeing.
    //  -2 internal error. The container is not changed.
  s_long remove_all() throw()
  {
    if (!_pz) { return _d_reinit() ? 0 : -2; } s_long n_prev = _n; if (n_prev == 0) { if (this->vecm::can_shrink()) { el_reserve_n(0, true); } if (_ht.can_shrink()) { _ht.el_reserve_n(0, true); } return 0; }
    s_long nh2 = _ht.can_shrink() ? _base_min : (_rhdir && _rhind >= 0 ? _base2 : _base);
    if (_ht.n() < nh2) { return -2; } if (this->vecm::el_remove_all() < 1) { return -2; }
    if (_ht.can_shrink()) { _ht.el_expand_n(nh2); }
    _base = nh2; _mask = nh2 - 1; _rhdir = 0;
    for (vecm::link1_t<s_long, false> l = _ht.link1_begin<s_long>(); !l.is_aend(); l.incr()) { *l.pval() = no_elem; }
    return n_prev;
  }

    // Normally, returns a valid ptr. to key hash/equality functor.
    //  On error, returns 0.
  inline const f_kf* pkf() const throw() { if (!_pz) { if (!_d_reinit()) { return 0; } } return &_pz->_kf; }

    // Normally, returns a valid ptr. to key hash/equality functor, used for creating default values (t_v).
    //    It may be modified or replaced if necessary.
    //  On error, returns 0.
  inline f_ctf* pctf() const throw() { if (!_pz) { if (!_d_reinit()) { return 0; } } return &_pz->_c; }

    // Sets the new key hash/equality functor. (Only if n() == 0.)
    //    On on_calm_result, returns true.
    //    On any error, returns false.
    //    On n() > 0, returns false (existing elements are already using the existing kf).
  inline bool hashx_set_kf(const f_kf& kf) throw() { if (!_pz) { if (!_d_reinit()) { return false; } } if (_n > 0) { return false; } if (&kf == &_pz->_kf) { return true; } char bu[2][sizeof(f_kf)]; typedef vecm::_config_ff::_tr_t<specf<f_kf> > tr; if (!tr::Fcopy_1only(&_pz->_kf, &kf, bu[1])) { return false; } tr::Fdestroy_prev_1only(&_pz->_kf, bu[1], bu[0], _nxf); return true; }

    // Safety state of the container.
    //   1 - normal. nexc() == 0.
    //   0 - normal. All elements are correct. nexc() != 0.
    //      Positive nexc() is the exact number of exceptions.
    //      Negative nexc() means too many exceptions.
    //   -1 - unsafe state. May NOT occur unless the object is incorrectly configured
    //          or client-side key/value functions are incorrectly working.
    //      Access by key and modification ops. may be unsafe, depending on situation.
    //      Access to entries by index and link is safe.
    //  -2 - unusable state. May NOT occur unless object memory is externally set to zero.
    //      The container functions, except the destructor, are unsafe.
    //  -3 - The function is called for a null pointer.
    //  See vecm integrity() for more details.
  inline s_long integrity() const throw() { if (!this) { return -3; } return bytes::min_sl(this->vecm::integrity(), _ht.integrity()); }

    // Getting a link to an entry. The link pval() returns const entry*,
    //    which has const k (key), h (hash fn. value), and non-const v (value).
    //    The fourth const member (q) should not be utilized by the client.
    //  For link1_cat, ind may be anything. It will be limited to the range [-1..n()].
    //    link1_cat(n()) is the same as link1_caend().
  inline link1_t<entry, true> link1_cbegin() const throw() { return link1_t<entry, true>(*this, 0); }
  inline link1_t<entry, true> link1_caend() const throw() { return link1_t<entry, true>(*this); }
  inline link1_t<entry, true> link1_cat(s_long ind) const throw() { return link1_t<entry, true>(*this, ind); }

    // Max. number of entries, to which the container can grow
    //    without additional dynamic memory allocation
    //    for entries storage, hash table and inner structures.
    //    This assumes that the reserve does not shrink as side effect of elements removal
    //    (either can_shrink() == false or the elements are only added).
    //  NOTE Normally, n() <= nrsv(), but in certain cases, n() may be slightly larger,
    //    because of delayed reserve adjustment in response to n() growth.
    //    To ensure n() <= nrsv() at the moment, use hashx_set_reserve(n(), false).
  inline s_long nrsv() const throw()
  {
    if (!_pz) { return 0; } s_long n1 = this->vecm::nrsv();
    s_long n2; if (true) { _yk_reg s_long zmax = _ht.nrsv(); _yk_reg s_long z = _base;
      while (true) { _yk_reg s_long z2 = z << bytes::ilog2_t<k_grow>::f; if (z2 <= z) { z2 = s_long(1) << bytes::ilog2_t<meta::s_long_mp>::f; } if (z2 > zmax) { break; } z = z2; }
      n2 = z <= meta::s_long_mp / k_max ? z * k_max - 1 : meta::s_long_mp; }
    return bytes::min_sl(n1, n2);
  }

    // true (default): entries and hash table storage reserve shrinks automatically on entries removal.
    //    NOTE For hash table, this occurs in jumps, when hash table size is decreased.
    // false: the reserve does not shrink from the current value in any way.
    //    Amount of allocated memory may only increase, when entries are added
    //    and their number becomes larger than any previous.
    // NOTE On can_shrink() false, hash table is not resized on elements removal.
  inline bool can_shrink() const throw() { return this->vecm::can_shrink(); }

    // The number of exceptions in key, value constructors, destructors, and internal object destructors.
    //    Negative value means "too many". See also vecm nexc().
  inline s_long nexc() const throw() { s_long n = _nxf; _nx_add(n, _ht.nexc()); return n >> _xsh; }

    // Setting the place reserve in the container enough for at least n entries.
    //    (This does not take into account possible allocations inside new k and v objects).
    //  allow_shrink false disables possible shrinking of any existing structure.
    //      It will be left as large as is.
    //  See also multiple notes for vecm el_reserve_n.
    //  Returns:
    //    true if succeeded. (If n == [0..n()], always true.)
    //      The actually created reserve (nrsv()) may be slightly larger than n.
    //    false on n < 0 or any creation/allocation failure.
    //  NOTE The function does not resize the hash table,
    //    this is done only by insert/remove/clear functions.
  inline bool hashx_set_reserve(s_long n, bool allow_shrink) throw()
  {
    if (n < 0) { return false; } if (!_pz) { if (n == 0) { return true; } if (!_d_reinit()) { return false; } }
    s_long n10 = this->vecm::nrsv(); s_long n20 = _ht.nrsv(); s_long n1 = n;
    s_long n2; if (true) { _yk_reg s_long zmin = n / k_max; if (zmin < meta::s_long_mp) { zmin += 1; } _yk_reg s_long z = _base;
      while (z < zmin) { _yk_reg s_long z2 = z << bytes::ilog2_t<k_grow>::f; if (z2 <= z) { z = s_long(1) << bytes::ilog2_t<meta::s_long_mp>::f; break; } z = z2; }
      n2 = z; }
    if (!allow_shrink) { n1 = bytes::max_sl(n1, n10); n2 = bytes::max_sl(n2, n20); }
    bool cancel(false); if (n1 > n10) { if (!el_reserve_n(n1, true)) { cancel = true; } } if (!cancel && n2 > n20) { if (!_ht.el_reserve_n(n2, true)) { cancel = true; } }
    if (!cancel) { if (n1 < n10) { el_reserve_n(n1, true); } if (n2 < n20) { _ht.el_reserve_n(n2, true); } return true; }
    if (n1 > n10) { el_reserve_n(n10, true); } if (n2 > n20) { _ht.el_reserve_n(n20, true); } return false;
  }

    // Setting the value of can_shrink() flag.
  inline void hashx_setf_can_shrink(bool x) throw() { _setf_can_shrink(x); _ht.vecm_setf_can_shrink(x); }

    // Setting nexc() == 0. If integrity() was 0, it becomes 1.
  inline void hashx_set0_nexc() const throw() { _set0_nexc(); }

private:
  hashx& operator=(const hashx&);
  enum
  {
    __check1 = meta::assert<vecm::template specf<entry>::transactional>::result,
    __check2 = meta::assert<vecm::template specf<t_ka>::transactional>::result,
    __check3 = meta::assert<vecm::template specf<t_va>::transactional>::result
  };
protected:

  static const s_long _base_min = 32; // >= 4
  static const s_long k_max = 2; // avg. no. of keys in one hash table entry to allow growing (>= 1)
  static const s_long k_grow = 4; // hash table size multiplier -- power of 2

  enum { delta_ht = 2560, M_ins = 15, M_rem = 10 }; struct t_dyndata { f_kf _kf; f_ctf _c; };

  vecm _ht; t_dyndata* _pz;
  s_long _base; s_long _mask; // _base is always _base_min * (k_grow ^ N)
  s_long _base2; s_long _mask2; // _base2 is always _base_min * (k_grow ^ (N + 1))
  s_long _rhdir; s_long _rhind;

  static bool Fd_reinit(t_hashx* p) throw();
  static void Fd_destroy(t_hashx* p) throw();
  static inline t_dyndata* Fz_new(const vecm::type_descriptor* _ptd) throw() { t_dyndata* p = 0; try { p = reinterpret_cast<t_dyndata*>(_ptd->pma2(sizeof(t_dyndata))); } catch (...) {} if (p) { try { new (p) t_dyndata(); } catch (...) { try { _ptd->pmf2(p); } catch (...) {} p = 0; } } return p; }
  static inline t_dyndata* Fz_new_copy(const vecm::type_descriptor* _ptd, const t_dyndata* psrc) throw() { if (!psrc) { return 0; } t_dyndata* p = 0; try { p = reinterpret_cast<t_dyndata*>(_ptd->pma2(sizeof(t_dyndata))); } catch (...) {} if (p) { try { new (p) t_dyndata(*psrc); } catch (...) { try { _ptd->pmf2(p); } catch (...) {} p = 0; } } return p; }
  static inline void Fz_del(const vecm::type_descriptor* _ptd, const t_dyndata* _pz, s_long& bc) throw() { if (!_pz) { return; } try { _pz->~t_dyndata(); } catch (...) { vecm::_nx_add(bc, 1); } try { _ptd->pmf2(const_cast<t_dyndata*>(_pz)); } catch (...) {} }

    // Dynamic data initialization (on _pz == 0) or reinitialization (on _pz != 0).
    //    Additionally, removes all entries and reduces hash table size to default.
    //  Returns:
    //    true on on_calm_result (_pz and other members are valid),
    //    false on failure (_pz == 0, other members are not initialized).
    //    NOTE nexc() keeps errs. counting.
  inline bool _d_reinit() const throw() { return Fd_reinit(const_cast<t_hashx*>(this)); }
    // Dynamic data cleanup. NOTE nexc() keep errs. counting.
  inline void _d_destroy() const throw() { Fd_destroy(const_cast<t_hashx*>(this)); }
  inline void _set0_nexc() const throw() { _nxf &= _fm; _ht.vecm_set0_nexc(); }

  inline s_long _hind(s_long h) const throw() { if (!_rhdir) { return h & _mask; } _yk_reg s_long r = h; r &= _mask; if (r < _rhind) { r = h; r &= _mask2; } return r; }

  template<class K2, class KeyFs>
  s_long _find(const K2& k, s_long h, const KeyFs& _kf, entry*& ret_e, s_long& ret_ind) const throw()
  {
    try {
      _yk_reg s_long q = *_ht.pval_0u<s_long>(_hind(h));
      if (q == no_elem) { ret_e = 0; ret_ind = no_elem; return 0; }
      while (true)
      {
        _yk_reg entry* e = pval_0u<entry>(q);
        s_long eh = e->h; if (h < eh) { ret_e = 0; ret_ind = no_elem; return 0; }
        if (h == eh && _kf.is_eq(e->k, k)) { ret_e = e; ret_ind = q; return 1; }
        q = e->q; if (q == no_elem) { ret_e = 0; ret_ind = no_elem; return 0; }
      }
    } catch (...) { ret_e = 0; ret_ind = no_elem; return -2; }
  }

  template<class K2, class KeyFs>
  s_long _insert(const K2& k, s_long h, const KeyFs& _kf, entry*& ret_e, s_long& ret_ind) throw()
  {
    try {
      _yk_reg s_long* pq = _ht.pval_0u<s_long>(_hind(h));
      _yk_reg s_long q = *pq;
        // Look for existing entry.
      if (q != no_elem)
      {
        while (true)
        {
          _yk_reg entry* e = pval_0u<entry>(q);
          s_long eh = e->h; if (h < eh) { break; }
          if (h == eh && _kf.is_eq(e->k, k)) { ret_e = e; ret_ind = q; return 0; }
          pq = &e->q; q = *pq; if (q == no_elem) break;
        }
      }

        // Add new entry.
      _yk_reg entry* e = el_expand_1<entry>();
      if (!e) { ret_e = 0; ret_ind = no_elem; return -1; }
      e->q = q;
      e->h = h;
      try { _pz->_c.f(&e->v); } catch (...) { el_expunge_last<entry>(); ret_e = 0; ret_ind = no_elem; return -1; }
      try { _kf.cnew(&e->k, k); } catch (...) { if (cfgv::dmode) { cfgv::Fdestroy_1(&e->v); } el_expunge_last<entry>(); ret_e = 0; ret_ind = no_elem; return -1; }

      ret_e = e; q = _n; --q; *pq = q; ret_ind = q;

        // Expand hash table as necessary.
      if (!_rhdir)
      {
        if (_n & _mask) { return 1; } // the below code will execute only on n % _base == 0
        if (_n / _base < k_max) { return 1; }
        q = s_long(1) << bytes::ilog2_t<meta::s_long_mp>::f; q /= k_grow; if (q < _base) { return 1; } // hash table has reached its max. size (power of k_grow)
        _base2 = _base * k_grow; _mask2 = _base2 - 1; _rhind = -1; _rhdir = 1; return 1;
      }
      else
      {
        if (_rhdir > 0)
        {
          if (_rhind < 0)
          {
            s_long n = _ht.n(); s_long m = _base2 - n;
            if (m > 0) { if (m > delta_ht) { m = delta_ht; } if (_ht.el_append_m<s_long>(m, no_elem) != m) { _rhdir = 0; } return 1; }
            _rhind = 0;
          }
            // Per one element inserted above, M_ins * k_max entries are moved (rehashed).
            // The more this value, the faster overall insertion, until internal CPU cache capacity is exceeded (practically, moving 10..50 entries is most efficient).  .
            // The less this value, the faster response to inserting each particular element.
          for (s_long j = 0; j < M_ins; ++j) { _rh_exp1(); ++_rhind; if (_rhind & _base) { _base = _base2; _mask = _mask2; _rhdir = 0; return 1; } }
          return 1;
        }
        else
        {
          if (_n & _mask) { return 1; } // the below code will execute only on n % _base == 0
          if (_n / _base < k_max) { return 1; } _rhdir = 1; return 1; // go to growing mode
        }
      }
    } catch (...) { ret_e = 0; ret_ind = no_elem; return -2; }
  }


    // returns 1 - the element existed and was removed, 0 - the element did not exist, -1 - failed to remove the existing element (no changes were made).
  template<class K2, class KeyFs>
  s_long _remove(const K2& k, s_long h, const KeyFs& _kf) throw()
  {
    try {
      _yk_reg s_long* pq = _ht.pval_0u<s_long>(_hind(h));
      _yk_reg s_long q = *pq;
      if (q == no_elem) { return 0; }
      while (true)
      {
        _yk_reg entry* e = pval_0u<entry>(q);
        s_long eh = e->h; if (h < eh) { return 0; }
        if (h == eh && _kf.is_eq(e->k, k))
        {
          s_long q_last = _n; --q_last;
          if (q == q_last) { *pq = e->q; el_remove_last<entry>(); } // A) the element being removed is the last element in the container
          else
          {
            s_long q_next = e->q;
            if (q_next == q_last)  // B) the element being removed is the previous of the last element. (The last simply goes to the place of the removed.)
              { if (el_remove_1<entry>(q, true) <= 0) { return -1; } }
            else
            {
              entry* e_last = pval_last<entry>();
              if (q == e_last->q) // C) the element being removed is the next of the last element.
              {
                s_long* pq2 = _ht.pval_0u<s_long>(_hind(h));
                while (true) { _yk_reg s_long q2 = *pq2; if (q2 == q_last) { break; } if (q2 == no_elem) { return -1; } pq2 = &pval_0u<entry>(q2)->q; }
                if (el_remove_1<entry>(q, true) <= 0) { return -1; }
                *pq2 = q; e->q = q_next; // here: e already points to the element moved from the last position to the position of the removed element.
              }
              else // D) the element being removed is unrelated to the last element.
              {
                s_long* pq2 = _ht.pval_0u<s_long>(_hind(e_last->h));
                while (true) { _yk_reg s_long q2 = *pq2; if (q2 == q_last) { break; } if (q2 == no_elem) { return -1; } pq2 = &pval_0u<entry>(q2)->q; }
                if (el_remove_1<entry>(q, true) <= 0) { return -1; }
                *pq = q_next; *pq2 = q;
              }
            }
          }
            // Shrink hash table as necessary.
          if (!_rhdir)
          {
            if (!_ht.can_shrink()) { return 1; }
            q = _base; q >>= bytes::ilog2_t<k_grow>::f; if (q < _base_min) { return 1; }
            q *= k_max; q -= q >> 2; if (_n >= q) { return 1; } // 25% of the new base size is the gap for switching into expansion mode, if insertions/removals are mixed
            _base2 = _base; _mask2 = _mask; _base >>= bytes::ilog2_t<k_grow>::f; _mask = _base - 1; _rhind = _base; _rhdir = -1; return 1;
          }
          else
          {
            if (_rhdir < 0)
            {
              if (_rhind <= 0)
              {
                s_long n = _ht.n(); s_long m = n - _base;
                if (m > 0) { if (m > delta_ht) { m = delta_ht; } n -= m; if (!_ht.el_expand_n(n) || n == _base) { _rhdir = 0; } return 1; }
                _rhdir = 0; return 1;
              }
              for (s_long j = 0; j < M_rem; ++j) { --_rhind; _rh_shr1(); if (_rhind == 0) { _rhind = -1; return 1; } }
              return 1;
            }
            else { q = _base; q *= k_max; q -= q >> 2; if (_n >= q) { return 1; } _rhdir = -1; return 1; }
          }
        }
        pq = &e->q; q = *pq; if (q == no_elem) { return 0; }
      }
    } catch (...) { return -2; }
  }

  s_long _remove_e(const entry* e_) throw()
  {
    if (!e_) { return 0; }
    s_long h = e_->h;
    _yk_reg s_long* pq = _ht.pval_0u<s_long>(_hind(h));
    _yk_reg s_long q = *pq;
    if (q == no_elem) { return 0; }
    while (true)
    {
      _yk_reg entry* e = pval_0u<entry>(q);
      s_long eh = e->h; if (h < eh) { return 0; }
      if (h == eh && e == e_)
      { // this block is copied from _remove()
s_long q_last = _n; --q_last; if (q == q_last) { *pq = e->q; el_remove_last<entry>(); } else { s_long q_next = e->q; if (q_next == q_last)   { if (el_remove_1<entry>(q, true) <= 0) { return -1; } }
else { entry* e_last = pval_last<entry>(); if (q == e_last->q) { s_long* pq2 = _ht.pval_0u<s_long>(_hind(h)); while (true) { _yk_reg s_long q2 = *pq2; if (q2 == q_last) { break; } if (q2 == no_elem) { return -1; } pq2 = &pval_0u<entry>(q2)->q; } if (el_remove_1<entry>(q, true) <= 0) { return -1; } *pq2 = q; e->q = q_next; }
else { s_long* pq2 = _ht.pval_0u<s_long>(_hind(e_last->h)); while (true) { _yk_reg s_long q2 = *pq2; if (q2 == q_last) { break; } if (q2 == no_elem) { return -1; } pq2 = &pval_0u<entry>(q2)->q; } if (el_remove_1<entry>(q, true) <= 0) { return -1; } *pq = q_next; *pq2 = q; } } }
if (!_rhdir) { if (!_ht.can_shrink()) { return 1; } q = _base; q >>= bytes::ilog2_t<k_grow>::f; if (q < _base_min) { return 1; } q *= k_max; q -= q >> 2; if (_n >= q) { return 1; } _base2 = _base; _mask2 = _mask; _base >>= bytes::ilog2_t<k_grow>::f; _mask = _base - 1; _rhind = _base; _rhdir = -1; return 1; }
else { if (_rhdir < 0) { if (_rhind <= 0) { s_long n = _ht.n(); s_long m = n - _base; if (m > 0) { if (m > delta_ht) { m = delta_ht; } n -= m; if (!_ht.el_expand_n(n) || n == _base) { _rhdir = 0; } return 1; } _rhdir = 0; return 1; } for (s_long j = 0; j < M_rem; ++j) { --_rhind; _rh_shr1(); if (_rhind == 0) { _rhind = -1; return 1; } } return 1; }
else { q = _base; q *= k_max; q -= q >> 2; if (_n >= q) { return 1; } _rhdir = -1; return 1; } }
      }
      pq = &e->q; q = *pq; if (q == no_elem) { return 0; }
    }
  }

  void _rh_exp1() throw()
  {
    s_long* p0 = _ht.pval_0u<s_long>(_rhind); _yk_reg s_long qx = *p0;
    if (qx != no_elem)
    {
      *p0 = no_elem;
      while (true)
      {
        _yk_reg entry* ex = pval_0u<entry>(qx); s_long* pq = _ht.pval_0u<s_long>(ex->h & _mask2); _yk_reg s_long q = *pq;
        if (q == no_elem) { *pq = qx; } else { _yk_reg entry* e = pval_0u<entry>(q); while ((q = e->q) != no_elem) { e = pval_0u<entry>(q);} e->q = qx; }
        qx = ex->q; if (qx == no_elem) { break; } ex->q = no_elem;
      }
    }
  }
  void _rh_exp1_k4() throw() // works better than _rh_exp1() for k_max >= 4
  {
    s_long* p0 = _ht.pval_0u<s_long>(_rhind); s_long q = *p0;
    if (q != no_elem)
    {
      s_long* pqsb[k_grow]; pqsb[0] = p0; for (s_long j = 1, k = _rhind + _base; j < k_grow; ++j, k += _base) { pqsb[j] = _ht.pval_0u<s_long>(k); }
      s_long shift = bytes::ilog2(_base); s_long j;
      while (true)
      {
        _yk_reg entry* e = pval_0u<entry>(q); j = e->h; j &= _mask2; j >>= shift; *pqsb[j] = q; q = e->q;
        if (q == no_elem) { break; } pqsb[j] = &e->q;
      }
      for (s_long j2 = 0; j2 < j; ++j2) { *pqsb[j2] = no_elem; } for (s_long j2 = j + 1; j2 < k_grow; ++j2) { *pqsb[j2] = no_elem; }
    }
  }
  void _rh_shr1() throw()
  {
    s_long* pqa0(_ht.pval_0u<s_long>(_rhind));
    for (s_long j = 1; j < k_grow; ++j)
    {
      _yk_reg s_long qb;
      if (true) { _yk_reg s_long* pqb = _ht.pval_0u<s_long>(_rhind + j * _base); qb = *pqb; if (qb == no_elem) { continue; } *pqb = no_elem; }
      _yk_reg s_long* pqa = pqa0; _yk_reg s_long qa = *pqa; if (qa == no_elem) { *pqa = qb; continue; }
      _yk_reg entry* ea = pval_0u<entry>(qa);
      do
      {
        _yk_reg entry* eb = pval_0u<entry>(qb);
        while (true)
        {
          if (eb->h <= ea->h) { break; }
          pqa = &ea->q; qa = *pqa; if (qa == no_elem) { *pqa = qb; goto lExitLevel2; } ea = pval_0u<entry>(qa);
        }
        *pqa = qb; pqa = &eb->q; qb = *pqa; *pqa = qa;
      } while (qb != no_elem);
      lExitLevel2:;
    }
  }
};

template<class T1, class T2, class T3> bool hashx<T1, T2, T3>::Fd_reinit(t_hashx* p) throw()
{
  if (p->_pz) { Fd_destroy(p); }
  p->_base = _base_min; p->_mask = _base_min - 1; p->_rhdir = 0; p->_pz = Fz_new(p->_ht.ptd());
  if (p->_pz) { if (p->_ht.template el_append_m<s_long>(_base_min, no_elem) != _base_min) { Fd_destroy(p); } }
  return bool(p->_pz);
}
template<class T1, class T2, class T3> void hashx<T1, T2, T3>::Fd_destroy(t_hashx* p) throw()
{
  if (p->_pz) { Fz_del(p->_ht.ptd(), p->_pz, p->_nxf); p->_pz = 0; }
  s_long ec1 = p->vecm_clear(); s_long ec2 = p->_ht.vecm_clear(); vecm::_nx_set(p->_nxf, ec1); vecm::_nx_add(p->_nxf, ec2);
}

  // Type configuration for hash dynamic parts.
  // 1. Hash table uses vecm::spec<s_long>.
  // 2. hashx uses hash table's t_alloc2 for its own dynamic allocations.
  // 3. Entries storage uses vecm::spec<hashx_common::entry<KA, VA> >.
  //  This takes into account movability and destructor use for both K and V,
  //  as specified in their main or alt. configurations.
  // 4. vecm of hashx itself is created in usual way: vecm(typer<hashx<K, V, Kf> >, base).
  //  hashx is movable, as well as vecm, so multilevel structures
  //  (V(H), V(V), H(H), H(V), recursively) are very efficient.
  // NOTE For special allocation or alignment of particular key/value combinations,
  //    vecm::spec<entry> may be specialized more, as necessary.
  // NOTE Any entry configuration must have transactional == true.

template<class KA, class VA, int kind> struct vecm::spec<hashx_common::entry<KA, VA, kind> >
{
  typedef hashx_common::entry<KA, VA, kind> E; typedef specf<KA> cfgk; typedef specf<VA> cfgv;
  enum
  {
    _ck = cfgk::cmode, _cv = cfgv::cmode,  _dk = cfgk::dmode, _dv = cfgv::dmode,  _mk = cfgk::mmode, _mv = cfgv::mmode, _gmk = cfgk::gm, _gmv = cfgv::gm,
    _dq = (_dk == 0 && _dv == 0 ? 0 : 3),
    _cq = (_ck == 1 && _cv ==1 ? 1 : (_ck == 4 && _cv == 4 ? 4 : (_ck >= 3 && _cv >= 3 ? 3 :  2 * (_ck && _cv)))),
    _mq = (_mk == 4 && _mv == 4 ? 4 : (_gmk && _gmv ? 3 : 2 * (_mk && _mv)))
  };
  typedef hashx_common::_entry_da<KA, VA, kind, (_dk == 0 ? 0 : 1) | (_dv == 0 ? 0 : 2)> D;
  struct aux : vecm::config_aux<E> { typedef D ta_meta_destroy; };
  typedef config_t<E, _dq,  _mq, _cq, aux> config;
};

template<class KA, class VAF, class Kf> struct vecm::spec<hashx<KA, VAF, Kf> > { typedef hashx<KA, VAF, Kf> H; struct aux : vecm::config_aux<H> { }; typedef config_t<H, 1, 4, 2, aux> config; };

template<class KA, class VAF, class Kf> struct meta::copy_t<hashx<KA, VAF, Kf> > { typedef hashx<KA, VAF, Kf> H; struct exc_copy {}; static inline void F(H* pdest, const H* psrc) { new (pdest) H(*psrc); if (pdest->nexc() != 0) { pdest->~H(); throw exc_copy(); } } };

}

#if defined(__clang__)
  #pragma clang diagnostic pop
#endif

#endif

/*
========================================
 Guidelines for using this file
========================================

INTRO

vecm_hashx.h clearly implements several well-known concepts to support
both end applications and software libraries.

It is a part of in-progress scientific research
on the conjoint computing and programming performance.

Author aims at rare, but highly elaborate updates for this module, to keep the following:

performance of the resulting executable code on multiple compilers and platforms;
predictible results of operations;
least possible module dependencies;
add-only public interface and most of internal elements;
low human time expenses on writing / compiling / testing high-quality client code.

Parts that may be added to this module, first pass through experimental modules.

REUSE

1. Including vecm_hashx.h as is is highly recommended. Client fine-tuning
of type properties and behavior should be via user template specializations
and externally defined structures.

2. For any partial copies and modifications, please always insert original
autorship information from the top of this file.

3. Use this module only for adequate purposes (programming, education,
research). Let the good will be with you.

THE COMPILER MUST HAVE SEVERAL THINGS

char is signed 8-bit value.
long long types are supported.
Some of short, int, long, long long (signed and unsigned) are 32- and 64-bit.
sizeof(double) == 8.
Pointers to data and functions have equal size
and aligned in structures without spaces.

TOP-LEVEL NAMESPACE OBJECTS

struct meta -- abstract type logics
typedef meta::s_long s_long -- 32-bit signed integer
struct lock_impl_t - template decl. for system-dependent critical section
struct bytes -- byte-level functions and utilities
struct vecm -- the vector
typer -- function template, returning ref. to type configuration
struct hashx_common -- hash's supporting structs and utilities
struct hashx -- the hash template

QUICK START EXAMPLES

Creating vecm(string):

    #include "vecm_hashx.h"
    using namespace yk_c;

    typedef std::string S;
    vecm v(typer<S>, -3); // base index is -3
    v.el_append<S>("A"); // el. 0
    v.el_append_m<S>(9, "");
    v.el_append<S>("C");
    v.el_insert_1<S>(2, "B"); // el. 5
    v.el_remove_1<S>(1, false);

    std::cout << v.n() << "\n"; // outputs 11 (the number of elements)
    std::cout << *v.pval_0u<S>(0) << ' ' << *v.pval_0u<S>(4) << "\n"; // outputs "A B"
    std::cout << *v.pval<S>(-3+0) << ' ' << *v.pval<S>(-3+4) << "\n"; // outputs "A B"
    std::cout << *v.pval_first<S>() << ' ' << *v.pval_last<S>() << "\n"; // outputs "A C"

Creating hash(integer, real number) with the default hash function:

    typedef hashx<int, double> H; H h;
    h.insert(25); h(0)->v = 1.3; h[26] = 1.4; h[-3] = 0.1; h[7] = -1; // the order of keys is kept: 25, 26, -3, 7
    const H::entry* pe = h.find(25);

    std::cout << h.remove(26) << ' ' << pe->v << "\n"; // outputs "1 1.3".
      // -- The first entry (pe) keeps its place.
      //   The only change is that the last entry jumps to pos. of the removed: 25, 7, -3.
    std::cout << h.remove(27) << "\n"; // outputs 0
    std::cout << h.n() << ' ' << h(1)->k << ' ' << h(1)->v << "\n"; // outputs "3 7 -1"

Creating hash(string, hash(integer, real number)):

    typedef hashx<std::string, hashx<int, double> > Hb;
    Hb hb;
    hb["a"][17] = 2.5;
    hb["a"][19] = 3.5;
    std::cout << hb["a"].n() << ' ' << hb["a"](0)->k << ' ' << hb["a"](1)->k << "\n"; // outputs "2 17 19"

Creating hash(string, vecm(hash(string, vecm of pointers))).

    typedef hashx<std::string, meta::construct_f<vecm, void*> > Hc;
    typedef hashx<std::string, meta::construct_f<vecm, Hc> > Hd;
      // -- second arg. of meta::construct_f specifies vecm element type.
    Hd hd;

    if (true)
    {
      Hc hc;
      vecm v(typer<Hc>, 0);
      int* p = 0;

      hc.insert("a");
      hc["b"].el_append<void*>(p+1);
      v.el_append(hc);
      hd["complex"].vecm_copy(v, true);
    }

    std::cout << hd["complex"].pval_first<Hc>()->remove("a") << "\n"; // outputs 1
    std::cout << *(*hd["complex"].pval_first<Hc>())["b"].pval_first<void*>() << "\n"; // outputs sizeof(int)

    //  NOTE hashx and vecm are movable in memory. Insertions/removals do not implicitly copy
    //    any unrelated elements.

STL-style iterator and very basic strongly-typed vector:

    template<class T, bool is_const> class iterator_t : public yk_c::vecm::link1_t<T, is_const>, public std::iterator<std::random_access_iterator_tag, T>
    {
    public:
      typedef yk_c::s_long s_long; typedef yk_c::vecm vecm;
      typedef T t_value; typedef t_value value_type;
      typedef typename yk_c::meta::if_t<is_const, const t_value*, t_value*>::result  pointer;
      typedef typename yk_c::meta::if_t<is_const, const t_value&, t_value&>::result reference;
      typedef yk_c::vecm::link1_t<t_value, is_const> t_link;
      typedef typename t_link::t_ctnr t_ctnr;
      typedef std::random_access_iterator_tag iterator_category; typedef iterator_t iterator_type; typedef s_long difference_type;

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
    namespace yk_c { template<class T, bool b> struct meta::type_equi<iterator_t<T, b>, meta::tag_construct> { typedef meta::tag_construct t_3; }; }
    namespace yk_c { template<class T, bool b> struct meta::type_equi<iterator_t<T, b>, meta::tag_functor> { typedef meta::tag_functor t_3; }; }

    template<class TA> struct vector_t : yk_c::vecm
    {
    public:
      typedef yk_c::s_long s_long; typedef yk_c::vecm vecm;
      typedef TA ta_value; typedef typename specf<ta_value>::t_value t_value; struct exc_vector_t : std::exception { const char* _p; exc_vector_t(const char* pmsg) : _p(pmsg) {} const char* what() const throw() { return _p; } };
      typedef s_long size_type; typedef t_value value_type; typedef t_value& reference; typedef const t_value& const_reference; typedef s_long difference_type;
      typedef iterator_t<t_value, false> iterator; typedef iterator_t<t_value, true> const_iterator;

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
    namespace yk_c { template<class TA> struct vecm::spec<vector_t<TA> > { typedef config_t<vector_t<TA>, 1, 4, 1> config; }; }

Structures for fine-tuning type properties and behavior:

    vecm:: spec, config_aux, config_t, bytes:: config0_t, config_cd_t - type configurations (movability, transactionality, categories etc.),
    meta:: type_equi - type aliasing for alt. configurations and tagging of classes,
    meta:: construct_f - functor prototype for automating container element construction,
    meta:: copy_t, safecopy_t, trymove_t, safemove_t, destroy_t - custom copy constructors, movers, destructor
    bytes:: allocdef_t - memory allocation functions (dflt.: operator new() + processing),
    lock_impl_t - template for system-dependent critical section,
    hashx_common:: kf_basic, kf_std, kf_str - default hash functions and adapters.

  NOTE Learning how these entities are used by vecm_hashx.h itself may help your understanding.
*/
