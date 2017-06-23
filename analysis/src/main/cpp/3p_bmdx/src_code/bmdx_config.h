// BMDX library 0.7 EXPERIMENTAL for desktop & mobile platforms
//  (binary modules data exchange)
// See bmdx_main.h for description.

// LIBRARY CONFIGURATION HEADER
// FOR MANUAL EDITING

#ifndef bmdx_config_H
#define bmdx_config_H



  // This part requires linking with -ldl in Linux.
#ifndef bmdx_part_dllmgmt
  #define bmdx_part_dllmgmt 1
#endif

#ifndef bmdx_part_dispatcher_mt
  #define bmdx_part_dispatcher_mt 1
#endif



  // Disabling excessive compiler warnings
#if defined(__GNUC__) && !defined(__clang__)
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
  #pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#ifdef _MSC_VER
//  #pragma warning(disable:4800)
//  #pragma warning(disable:4290)
//  #pragma warning(disable:4355)
//  #pragma warning(disable:4345)
//  #pragma warning(disable:4996)
//  #undef _CRT_SECURE_NO_WARNINGS
//  #define _CRT_SECURE_NO_WARNINGS
#endif



  // UTF-8 may be forced if libc does not provide symmetric wctomb / mbtowc.
#ifdef __ANDROID__
  #define bmdx_wsbs_force_utf8 1
  #define bmdx_bsws_force_utf8 1
#endif



namespace yk_c { struct bytes; }
namespace
{
  struct nothing {}; template<bool cond, class _ = nothing> struct assert { typedef bool t_false; };  template<class _> struct assert<true, _> { enum { result = true }; typedef bool t_true; };
  template<class L, class _> struct _check_isdef { enum { b = false }; }; template<class L> struct _check_isdef<L, typename L::_t_is_defined> { enum { b = true }; };

    // ERROR: vecm_hashx.h must not be included before this header.
    //  Include bmdx_config.h before or instead of any inclusion of vecm_hashx.h.
  typedef assert<_check_isdef<yk_c::bytes, bool>::b>::t_false __check1;
}



  // Required
#include "bmdx_cpiomt.h"
namespace yk_c
{
  template<class T, class Aux> struct lock_impl_t : bmdx::critsec_t<T>
  {
    typedef T t_target; typedef bmdx::critsec_t<T> t_impl; struct exc_lock {};
    lock_impl_t() : t_impl(10, -1) { if (!this->t_impl::is_locked()) { throw exc_lock(); } }
  };
}
#undef yk_c_vecm_hashx_H
#include "vecm_hashx.h" // must be included after bmdx_cpiomt.h to use the above lock
namespace yk_c
{
  typedef meta::s_ll s_ll;
  #if !defined(_bmdx_cpiomt_threadctl_spec)
    #define _bmdx_cpiomt_threadctl_spec
    template<> struct vecm::spec<bmdx::threadctl> { struct aux : vecm::config_aux<bmdx::threadctl> { };  typedef config_t<bmdx::threadctl, 1, 4, 0, aux> config; };
  #endif
}



  // Cross-module compatibility for strings (optimization only).

#include <string>

#ifndef yk_c_cmti_wstring_DEFINED
  namespace yk_c
  {
  #define yk_c_cmti_wstring_DEFINED

    #if defined(__MINGW32_MAJOR_VERSION)
        // In GCC family, empty strings may not be resized outside their native binary module if the compiler itself is not built using special configuration.
      #if _GLIBCXX_FULLY_DYNAMIC_STRING != 0
        template<> struct bytes::type_index_t<std::wstring> : cmti_base_t<std::wstring, 2015, 6, 1, 12, (sizeof(std::wstring) / (sizeof(std::wstring) % 4 == 0 ? 3 : 1) ^ __MINGW32_MAJOR_VERSION * 37 ^ __MINGW32_MINOR_VERSION * 4)> {};
      #endif
    #elif defined(_MSC_VER)
	    // MSVC can share strings between modules only when CRT is linked dynamically
	    // (code generation /MD or /MDd, which automatically defines _DLL).
      #if defined(_DLL)
        template<> struct bytes::type_index_t<std::wstring> : cmti_base_t<std::wstring, 2015, 6, 1, 13, (sizeof(std::wstring) / (sizeof(std::wstring) % 4 == 0 ? 3 : 1) ^ _MSC_VER / 11 ^ _MSC_VER % 16)> {};
      #endif
    #elif defined(__GNUC__) && !defined(__MINGW32_MAJOR_VERSION)
        // In GCC family, empty strings may not be resized outside their native binary module if the compiler itself is not built using special configuration.
      #if _GLIBCXX_FULLY_DYNAMIC_STRING != 0
        template<> struct bytes::type_index_t<std::wstring> : cmti_base_t<std::wstring, 2015, 6, 1, 14, (sizeof(std::wstring) / (sizeof(std::wstring) % 4 == 0 ? 3 : 1) ^ __GNUC__ * 37 ^ __GNUC_MINOR__ * 4)> {};
      #endif
    #elif defined(__clang__)
      template<> struct bytes::type_index_t<std::wstring> : cmti_base_t<std::wstring, 2015, 6, 1, 15, (sizeof(std::wstring) / (sizeof(std::wstring) % 4 == 0 ? 3 : 1) ^ __clang_major__ * 37 ^ __clang_minor__ * 4)> {};
    #elif defined(__SUNPRO_CC)
      template<> struct bytes::type_index_t<std::wstring> : cmti_base_t<std::wstring, 2015, 6, 1, 16, (sizeof(std::wstring) / (sizeof(std::wstring) % 4 == 0 ? 3 : 1) ^ __SUNPRO_CC / 11 ^ __SUNPRO_CC % 16)> {};
    #else
      #undef yk_c_cmti_wstring_DEFINED
    #endif
  }
#endif


#endif
