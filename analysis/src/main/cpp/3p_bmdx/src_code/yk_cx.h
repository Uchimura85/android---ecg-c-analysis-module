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

// yk_cx.h 1.1:
//    vec2_t - alternative vector with O(N^0.5) insert/remove and memory-efficient appending.
//    ordhs_t - ordered hash map with access by key and ordinal number.
//    + storage_t, iterator_t, link2_t, container copy utilities.

// USAGE NOTE
//
// The following headers must be pre-included
// if they are not in the same directory as this file:
//
// vecm_hashx.h

#ifndef yk_c_experimental_v1_1_H
#define yk_c_experimental_v1_1_H


#ifndef yk_c_vecm_hashx_H
  #include "vecm_hashx.h"
#endif




#if defined(__clang__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wunknown-pragmas"
#endif
#if defined(__GNUC__)
  #pragma GCC diagnostic ignored "-Wpragmas"
  #pragma GCC diagnostic ignored "-Wundefined-bool-conversion"
#endif

#include <iterator>

namespace yk_c
{
  namespace experimental
  {
      // storage_t(-1) == storage only, no automatic initialization and destruction.
      //    The client may do it manually or with try_init(), try_deinit().
      //  storage_t(0) == storage with automatic T destruction on bool(inited) == true. Initialization is done, if necessary,
      //    by the client, either manually (+ setting inited true on on_calm_result), or with try_init().
      //  storage_t(1) == storage with automatic T construction and destruction.
      //  In all modes, try_init(), try_deinit() work correctly.
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
//    namespace yk_c { template<class T, bool b> struct meta::type_equi<iterator_t<T, b>, meta::tag_construct> { typedef meta::tag_construct t_3; }; }
//    namespace yk_c { template<class T, bool b> struct meta::type_equi<iterator_t<T, b>, meta::tag_functor> { typedef meta::tag_functor t_3; }; }
  }
  template<class T, bool b> struct meta::type_equi<experimental::iterator_t<T, b>, meta::tag_construct> { typedef meta::tag_construct t_3; };
  template<class T, bool b> struct meta::type_equi<experimental::iterator_t<T, b>, meta::tag_functor> { typedef meta::tag_functor t_3; };

  namespace experimental
  {
    struct _link2_desc
    {
      meta::s_ll id;
      meta::s_ll idc;
      meta::s_ll rcnt;
      void* px; // 0 on ind0 == -2 and -3
      s_long ind0x; // -1 -- needs updating from px / not in container, -2 -- after-end pos., -3 -- value deleted

      inline _link2_desc() throw() : id(-1), idc(-1), rcnt(0), px(0), ind0x(-2) { }

      private: _link2_desc(const _link2_desc&); _link2_desc& operator=(const _link2_desc&);
    };

    struct _link2_opinfo
    {
      void* pdest;
      char flags; // 0x1 destructor, 0x2 moving, 0x4 copying -- see also bool tr_notify()
      inline _link2_opinfo() throw() : pdest(0), flags(0) {}
    };

    template<class TA> struct _v2ta_a {};
    template<class TA, class Ctnr, bool bsync> struct _v2ta_b {};
    template<class T, class Itr> struct _v2insert_arg {};

    struct link2_lks {};
      // - lock selector for _link2_reg ops.
      //  For multithreaded use of perm. links, yk_c::lock_impl_t<link2_lks> must be implemented.
  }
  using namespace experimental;

  template<> struct vecm::spec<_link2_desc> { typedef config_t<_link2_desc, 0, 4, 0> config; };
  template<> struct vecm::spec<_link2_opinfo> { typedef config_t<_link2_opinfo, 0, 4, 0> config; };

  namespace experimental
  {
    struct _link2_reg
    {
      typedef const hashx<void*, meta::s_ll>::entry Ep;
      typedef const hashx<meta::s_ll, _link2_desc>::entry E;
      typedef const hashx<void*, _link2_opinfo>::entry Etr;
      typedef const hashx<void*, hashx<void*, _link2_opinfo> >::entry Etrct;

        // Lock for single operation.
      struct lock_t_reg
      {
        storage_t<bytes::lock_t<link2_lks> > s; lock_t_reg(bool bsync) : s(bsync ? 1 : -1) {}
        bool is_lckinit() const throw() { return s.mode == -1 || (bool(s.inited) && s.ptr()->is_lckinit()); }
      };

        // When rpself_ == 0, registry ignores any calls, returning reasonable error codes.
      _link2_reg(bool bsync, _link2_reg*& rpself_) throw() : _idgen(-1), _sync(bsync), _px(&rpself_) { __hp.hashx_setf_can_shrink(false); }
      ~_link2_reg() throw() { lock_t_reg __lock(_sync); __htr.hashx_clear(); }

      static _link2_reg* ref(bool bsync) throw() { _initializer r; return bsync ? r.pv[0] : r.pv[1]; }

        // Removes all registered links data. The existing links will return 0 ptrs. New links will function normally.
      static void reg_clear()
      {
        _link2_reg* pl2 = ref(false); if (pl2 && *pl2->_px) { pl2->__hp.hashx_clear(); pl2->__hp.hashx_setf_can_shrink(false); pl2->h.hashx_clear(); pl2->__htr.hashx_clear(); }
        lock_t_reg __lock(true);  pl2 = ref(true); if (pl2 && *pl2->_px) { pl2->__hp.hashx_clear(); pl2->__hp.hashx_setf_can_shrink(false); pl2->h.hashx_clear(); pl2->__htr.hashx_clear(); }
      }

        // NOTE h, __hp:
        //  1. Deleting a perm. link target object sets its ind0 to -3 and decreases rcnt of its container.
        //      This way, the container may non-forcedly change its mode when all link targets were deleted,
        //      even if links still persist.
        //  2. Deleting a perm. link target object removes its pointer from __hp.
        //      Creating a new object at the same address inserts again an entry into __hp,
        //      but its value already represents another id. Older links ignore this entry.

        // Get pointer for element or its container.
        //  If the element and/or its container is deleted, returns 0 for any get_ct.
      inline void* get_p(meta::s_ll id, bool get_ct) const throw()
      {
        if (!this) { return 0; } if (id == -1) { return 0; } lock_t_reg __lock(_sync); if (!__lock.is_lckinit() || !*_px) { return 0; }
        E* e = h.find(id);
          if (!e || e->v.ind0x == -3) { return 0; }
          if (e->v.idc == -1) { return get_ct ? 0 : e->v.px; }
        E* e2 = h.find(e->v.idc);
          if (!e2 || e2->v.ind0x == -3) { return 0; }
        return get_ct ? e2->v.px : e->v.px;
      }

        // Get / refresh element position for the given id.
        //    Returns iterator poinitng to the element or after-end pos.
      template<class ctnr, class ct_access, bool is_const>
      inline typename meta::if_t<is_const, typename ct_access::const_iterator, typename ct_access::iterator>::result
        gr_iter(meta::s_ll id) throw()
      {
        typedef typename meta::if_t<is_const, typename ct_access::const_iterator, typename ct_access::iterator>::result t_iter;
        if (!(this && id != -1)) { return t_iter(); }
        lock_t_reg __lock(_sync); if (!__lock.is_lckinit() || !*_px) { return t_iter(); }
        ctnr* pct(0); s_long ind0(-1);
        do {
          E* e = h.find(id);
            if (!e || e->v.ind0x == -3 || e->v.idc == -1) { break; }
          E* e2 = h.find(e->v.idc);
            if (!e2 || e2->v.ind0x == -3) { break; }
          pct = reinterpret_cast<ctnr*>(e2->v.px);
          if (e->v.ind0x == -1 || (e->v.ind0x >= 0 && ct_access::Fpval0(*pct, e->v.ind0x) != e->v.px))
            { e->v.ind0x = ct_access::Find0x(pct, reinterpret_cast<typename ct_access::t_value*>(e->v.px)); }
          ind0 = e->v.ind0x >= 0 ? e->v.ind0x : pct->n();
        } while (false);
        return pct ? ct_access::template get_iter<is_const>::F(*pct, ind0) : t_iter();
      }

        // Get / create reg. entry for pv/ind0x_v and pct.
        //    If pct is not 0:
        //      1) pv may be 0 if ind0x_v == -1, this means ref. to after-end pos in *pct.
        //      2) otherwise ind0x_v / pv ref. the same existing value in pct.
        //    If pct is 0:
        //      3) pv != 0, ind0x_v == -1. pv is the value without container, or container itself.
        //    All other combinations are invalid.
        // gc_id increments links count for the value and its container (if specified).
        //    Returns value id or -1 on no on_calm_result.
      meta::s_ll gc_id(void* pct, void* pv, s_long ind0x_v) throw()
      {
        if (!this) { return -1; }
        s_long mode = pct ? 1 * (!pv && ind0x_v == -1) + 2 * (pv && ind0x_v >= 0) : pv && ind0x_v == -1 ? 3 : 0;
          if (mode <= 0) { return -1; }
        lock_t_reg __lock(_sync); if (!__lock.is_lckinit() || !*_px) { return -1; }

        Ep* ep_c(0); Ep* ep_v(0); E* e_c(0); E* e_v(0);

        s_long res1 = -1; if (pct) { res1 = hp_insert(pct, &ep_c); if (res1 < 0) { return 0; } }
        s_long res2 = -1; if (pv) { res2 = hp_insert(pv, &ep_v); if (res2 < 0) { if (res1 == 1) { hp_remove_e(ep_c); } return 0; } }

        meta::s_ll id1(-1); if (res1 == 0) { id1 = ep_c->v; } else { if (res1 == 1) { id1 = ++_idgen; } }
        meta::s_ll id2(-1); if (res2 == 0) { id2 = ep_v->v; } else { if (res2 == 1 || mode == 1) { id2 = ++_idgen; } }

        s_long res3 = -1;
          if (res1 == 0) { e_c = h.find(ep_c->v); if (!e_c) { if (res2 == 1) { hp_remove_e(ep_v); } if (res1 == 1) { hp_remove_e(ep_c); } return 0; } res3 = 0; }
            else if (res1 == 1) { res3 = h.insert2(id1, *h.pkf(), 0, &e_c); if (res3 < 0) { if (res2 == 1) { hp_remove_e(ep_v); } if (res1 == 1) { hp_remove_e(ep_c); } return 0; } }
        s_long res4 = -1;
          if (res2 == 0) { e_v = h.find(ep_v->v); if (!e_v) { if (res3 == 1) { h.remove_e(e_c); } if (res2 == 1) { hp_remove_e(ep_v); } if (res1 == 1) { hp_remove_e(ep_c); } return 0; } res4 = 0; }
            else if (res2 == 1 || mode == 1) { res4 = h.insert2(id2, *h.pkf(), 0, &e_v); if (res4 < 0) { if (res3 == 1) { h.remove_e(e_c); } if (res2 == 1) { hp_remove_e(ep_v); } if (res1 == 1) { hp_remove_e(ep_c); } return 0; } }

        bool b3 = res4 > 0 || (res4 == 0 && e_v->v.idc != id1);
        if (res1 == 1) { ep_c->v = id1; }
        if (res2 == 1) { ep_v->v = id2; }
        if (res3 > 0) { e_c->v.id = id1; e_c->v.px = pct; } if (b3) { e_c->v.rcnt += 1 + e_v->v.rcnt; }
        if (b3) { e_v->v.idc = id1; }
        if (res4 > 0) { e_v->v.id = id2; e_v->v.px = pv; } if (res4 >= 0) { ++e_v->v.rcnt; e_v->v.ind0x = ind0x_v; }

        return id2;
      }

        // Decrease the ref. cnt for the given id.
        //    If it becomes 0, the link may be removed. Then Its parents are also recursively unlinked.
      void link_remove(meta::s_ll id) throw() { if (!this) { return; } if (id == -1) { return; } lock_t_reg __lock(_sync); if (!__lock.is_lckinit() || !*_px) { return; }_link_remove(id); }

        // Increase the ref. cnt for the given id.
      void link_add(meta::s_ll id) throw() { if (!this) { return; } if (id == -1) { return; } lock_t_reg __lock(_sync); if (!__lock.is_lckinit() || !*_px) { return; } _link_add(id); }

        // Add and remove two links at once under the same lock.
      void link_a_r(meta::s_ll id_add, meta::s_ll id_remove) throw() { if (!this) { return; } if (id_add == id_remove) { return; } lock_t_reg __lock(_sync); if (!__lock.is_lckinit() || !*_px) { return; } _link_add(id_add); _link_remove(id_remove); }

      struct exc_tr_v0 : std::exception { const char* _p; exc_tr_v0(const char* pmsg) throw() : _p(pmsg) {} const char* what() const throw() { return _p; } };
      static s_long _ls_tr_start1() { throw exc_tr_v0("_ls_tr_start1: incompatible fn."); }
      static s_long  _ls_tr_start2() { throw exc_tr_v0("_ls_tr_start2: incompatible fn."); }
      static void _ls_tr_end1(s_long commit) { throw exc_tr_v0("_ls_tr_end1: incompatible fn."); }
      static void _ls_tr_end2(s_long commit) { throw exc_tr_v0("_ls_tr_end2: incompatible fn."); }

      static s_long _ls_tr_start_21(void* pct, s_long nrsv) throw() { _link2_reg* pl2 = ref(true); return pl2 ? s_long(pl2->tr_start(pct, nrsv)) : 0; }
      static s_long  _ls_tr_start_22(void* pct, s_long nrsv) throw() { _link2_reg* pl2 = ref(false); return pl2 ? s_long(pl2->tr_start(pct, nrsv)) : 0; }
      static void _ls_tr_end_21(void* pct, s_long commit) throw() { _link2_reg* pl2 = ref(true); if (pl2) { pl2->tr_end(pct, commit); } }
      static void _ls_tr_end_22(void* pct, s_long commit) throw() { _link2_reg* pl2 = ref(false); if (pl2) { pl2->tr_end(pct, commit); } }
      static s_long _ls_tr_notify1(void* psrc, void* pdest, s_long opflag) throw() { _link2_reg* pl2 = ref(true); return pl2 ? s_long(pl2->tr_notify(psrc, pdest, char(opflag))) : 1; }
      static s_long _ls_tr_notify2(void* psrc, void* pdest, s_long opflag) throw() { _link2_reg* pl2 = ref(false); return pl2 ? s_long(pl2->tr_notify(psrc, pdest, char(opflag))) : 1; }

      bool tr_start(void* pct, s_long nrsv) throw()
      {
        if (!this) { return false; } lock_t_reg __lock(_sync); if (!__lock.is_lckinit() || !*_px) { return false; }

        Ep* ecp = __hp.find(pct); if (!ecp) { return true; } // no links to elements of this container --> no transaction needed
        E* ec = h.find(ecp->v); if (!ec || ec->v.ind0x == -3) { return true; }
          if (ec->v.px != pct) { return false; }

        hashx<void*, _link2_opinfo>* phl2 = 0;
        try { phl2 = &__htr[ec->v.px];  } catch (...) { return false; }

        if (nrsv > ec->v.rcnt) { nrsv = (s_long)ec->v.rcnt; } if (nrsv < 0) { nrsv = 0; }
        if (!phl2->hashx_set_reserve(nrsv, true)) { return false; }

        return true;
      }

      void tr_end(void* pct, bool commit) throw()
      {
        if (!this) { return; }
        lock_t_reg __lock(_sync); if (!__lock.is_lckinit() || !*_px) { return; }

        if (!commit) { __htr.remove(pct); return; }

        Etrct* etrc = __htr.find(pct); if (!etrc) { return; }
        hashx<void*, _link2_opinfo>& hl2 = etrc->v;
          // Deleted-only element positions are all processed first, moved positions next
          //  -- to avoid de-registering destination addresses.
        for (s_long i = 0; i < hl2.n(); ++i)
        {
          _link2_opinfo& op = hl2(i)->v;
          if ((op.flags & 0x7) == 0x1) // deleted
              { _notify_deleted(hl2(i)->k, true); continue; }
        }
        for (s_long i = 0; i < hl2.n(); ++i)
        {
          _link2_opinfo& op = hl2(i)->v;
          if ((op.flags & 0x2) || (op.flags & 0x5) == 0x5) // moved
          {
            Ep* ep = __hp.find(hl2(i)->k); if (!ep) { continue; }
              E* e = h.find(ep->v); if (e) { e->v.ind0x = -1; e->v.px = op.pdest; }
              meta::s_ll id = ep->v; hp_remove_e(ep);
              Ep* ep2 = 0; if (hp_insert(op.pdest, &ep2) >= 0) { ep2->v = id; }
            continue;
          }
        }
        __htr.remove(pct);
      }

        // opflag (OR):
        //  0x1 - psrc is destoryed (pdest will be ignored)
        //  0x2 - object is moved from psrc to pdest.
        //  0x4 - object is copied from psrc to pdest.
        // Returns:
        //    true if reg. data updated successfully or updating is not necessary.
        //    false -- failure (the client should consider it as its op. failure).
        //        This is possible only when transaction is active.
      bool tr_notify(void* psrc, void* pdest, char opflag) throw()
      {
        if (!this) { return true; }
        lock_t_reg __lock(_sync); if (!__lock.is_lckinit() || !*_px) { return true; }

        Ep* ep = __hp.find(psrc); if (!ep) { return true; }
        E* e = h.find(ep->v); if (!e || e->v.idc < 0) { return true; }
        E* ec = h.find(e->v.idc); if (!ec || ec->v.ind0x == -3) { return true; }
        Etrct* etrc = __htr.find(ec->v.px);
          if (!etrc) // tr_notify called out of transaction (i.e. some simple modification)
          {
            if (opflag & 0x2) { e->v.ind0x = -1; meta::s_ll id = ep->v; hp_remove_e(ep); Ep* ep2 = 0; if (hp_insert(pdest, &ep2) >= 0) { ep2->v = id; } return true; }
            if (opflag & 0x1) { _notify_deleted(psrc, true); return true; }
            return true;
          }
        hashx<void*, _link2_opinfo>& hl2 = etrc->v;
        _link2_opinfo* pop = 0; try { pop = &hl2[psrc]; } catch (...) { return false; }

        if (opflag & 0x6) { pop->pdest = pdest; } // set dest. for copy or move op.
        pop->flags |= opflag;

        return true;
      }

        // Marks p as deleted. See _notify_deleted.
      s_long detach_p(void* p, bool forced) throw() { if (!this) { return 0; } if (!p) { return 0; } lock_t_reg __lock(_sync); if (!__lock.is_lckinit() || !*_px) { return 0; } return _notify_deleted(p, forced); }

    private:
      struct _data { short* init; _link2_reg** pv; meta::s_ll* d; inline _data(short* p1, _link2_reg** p2, meta::s_ll* p3) throw() : init(p1), pv(p2), d(p3) {} };
      static inline _data _reg() throw() { enum { _nst = 1 + sizeof(_link2_reg) / sizeof(meta::s_ll) }; static _link2_reg* p[4] = { 0, 0, 0, 0 }; static meta::s_ll v[2 * _nst]; static short _init_state(0); return _data(&_init_state, p, v); }
      struct _initializer : _data
      {
        const bool __b;
        _initializer() throw() : _data(_reg()), __b(*init == 1 || _init0())
        {
          enum { _nst = 1 + sizeof(_link2_reg) / sizeof(meta::s_ll) };
          if (*init == 0)
          {
            lock_t_reg __lock(true);
            _link2_reg* a = reinterpret_cast<_link2_reg*>(d); new (a) _link2_reg(true, pv[0]);
            _link2_reg* b = reinterpret_cast<_link2_reg*>(d + _nst); new (b) _link2_reg(false, pv[1]);
            pv[2] = a; pv[3] = b; pv[0] = a; pv[1] = b;
            *init = 1;
          }
        }
      };
      struct _detach { inline ~_detach() throw() { _data r = _reg(); if (*r.init == 1) { lock_t_reg __lock(true); if (!__lock.is_lckinit()) {} if (r.pv[0]) { r.pv[0]->_px = 0; r.pv[0] = 0; } if (r.pv[1]) { r.pv[1]->_px = 0; r.pv[1] = 0; } } } };
      struct _deinit { inline ~_deinit() throw() { _data r = _reg(); if (r.pv[2]) { r.pv[2]->~_link2_reg(); } if (r.pv[3]) { r.pv[3]->~_link2_reg(); } } };
      static inline s_long _init0() throw() { static _deinit x1; static _detach x2; return s_long(reinterpret_cast<char*>(&x2) - reinterpret_cast<char*>(&x1)); }

      _link2_reg(const _link2_reg&); _link2_reg& operator=(const _link2_reg&);

      hashx<void*, meta::s_ll> __hp; // kept with non-zero place reserve
        s_long hp_insert(void* pv, Ep** epp) throw() { if (__hp.nrsv() - __hp.n() < 8) { if (!__hp.hashx_set_reserve(__hp.n() + 8, false)) { return -1; } } return __hp.insert2(pv, *__hp.pkf(), 0, epp); }
        s_long hp_remove_e(Ep* ep) throw() { s_long res = __hp.remove_e(ep); if (__hp.nrsv() > 2 * __hp.n() && __hp.n() > 1000) { __hp.hashx_set_reserve(2 * __hp.n(), true); } return res; }
      hashx<meta::s_ll, _link2_desc> h;
      meta::s_ll _idgen;
      const bool _sync; // NOTE this value is accessible even if the object is already destroyed (static storage)
      _link2_reg** _px;
      hashx<void*, hashx<void*, _link2_opinfo> > __htr; // { container, { object, copy/move/destroy flags } }


        // Marks p as deleted.
        //  Returns:
        //    2 - p has been unregistered.
        //    1 - p has been unregistered, ignoring existing links (only on forced == true).
        //    0 - p was not registered.
        //    -1 - p has existing links and continues to be registered (only on forced == false).
      inline s_long _notify_deleted(void* p, bool forced) throw()
      {
        if (!p) { return 0; }
        Ep* ep = __hp.find(p); if (!ep) { return 0; }
        E* e = h.find(ep->v);
        bool cnz(false);
        if (e)
        {
          cnz = e->v.rcnt > 0;
          if (cnz && !forced) { return -1; }
          if (e->v.ind0x != -3) { e->v.ind0x = -3; hp_remove_e(ep); _link_remove(e->v.idc); }
          return cnz ? 1 : 2;
        }
        else
        {
          hp_remove_e(ep);
          return cnz ? 1 : 2;
        }
      }
      inline void _link_add(meta::s_ll id) throw() { if (id == -1) { return; } E* e = h.find(id); if (!e) { return; } ++e->v.rcnt; }
      inline void _link_remove(meta::s_ll id) throw()
      {
        if (id == -1) { return; }
        E* e = h.find(id); if (!e) { return; } if (--e->v.rcnt > 0) { return; }
        meta::s_ll idc = e->v.ind0x == -3 ? -1 : e->v.idc; void* px = e->v.px;
        h.remove_e(e);
        Ep* ep = __hp.find(px); if (ep && ep->v == id) { hp_remove_e(ep); }
        _link_remove(idc);
      }
    };

    struct _vec2_td
    {
      s_long version;
      s_long link2_flags; // 0x1 f_perm, 0x2 f_sync
      s_long* psig;
      s_long (*pvec2_copy)(void* pdest, const void* psrc, const vecm* pvecm, s_long mode);
      s_long (*pvec2_delete)(const void* pdest);
      s_long (*_p_tr_start)();
      void (*_p_tr_end)(s_long commit);
      s_long (*_p_tr_notify)(void* psrc, void* pdest, s_long opflag);
      s_long (*_pexpand_n)(void* p, s_long n2);
      s_long (*_pswap)(void* p1, void* p2);
      s_long (*_p_tr_start_2)(void* pct, s_long nrsv);
      void (*_p_tr_end_2)(void* pct, s_long commit);
    };

    struct _vec2_td_ver
    {
      struct s1 { s_long a; s_long b; void* c; };
      enum { nsig = 1, ver = nsig }; // ver == sig. length + 0x100 * incompat. version
      static inline s_long* psig() throw() { static s_long x[nsig + 1] = { sizeof(_vec2_td) * 0x100 + sizeof(s1), 0 }; return x; }
    };

    template<class TA> struct vec2_t;

    template<class TA, class Ctnr> struct _ct_access : Ctnr
    {
      typedef TA t_ta; typedef Ctnr t_ctnr; typedef vecm::specf<t_ta> cfgt; typedef typename cfgt::t_value t_value;
      typedef iterator_t<t_value, false> iterator; typedef iterator_t<t_value, true> const_iterator;

      static inline s_long Fsize(t_ctnr& c) throw() { return c.n(); }
      static inline bool Flocal(t_ctnr& c) throw() { return c.locality() == 1; }
      static inline bool Fperm(t_ctnr& c) throw() { return c.f_perm(); }
      static inline bool Fsync(t_ctnr& c) throw() { return c.f_sync(); }

      static inline t_value* Fpval0(t_ctnr& c, s_long ind0) throw() { if (ind0 >= 0 && ind0 < c.n()) { return &c[ind0]; } return 0; }
      template<bool is_const> struct get_iter
      {
        typedef typename meta::if_t<is_const, const_iterator, iterator>::result t_iter;
          // ind0 < 0 creates an iterator pointing to the after-end pos.
        static inline t_iter F(t_ctnr& c, s_long ind0) throw() { return t_iter(c, c.nbase() + (ind0 < 0 ? c.n() : ind0)); }
      };

        // Returns:
        //    a) index of px in ct: [0..n()).
        //    b) -1 if px is not found in ct.
        //    Search takes O(N^0.5).
      static s_long Find0x(t_ctnr* pct_, t_value* px) throw()
      {
        if (!(px && pct_)) { return -1; }
        _ct_access* pct = static_cast<_ct_access*>(pct_);
        typedef bytes::type_index_t<t_value> t_si;
        s_long r = pct->_t_ind; if (r != t_si::xind) { if (r != t_si::ind()) { return -1; } }
        if (pct->_n <= 0) { return -1; }
        vecm::_column* pc = pct->_pj;
        s_long n0 = 0;
        for (s_long j = 0; j <= pct->_last_j; ++j)
        {
          s_long cap = vecm::m::_cap_j(j);
          meta::s_ll d = meta::s_ll(px - (t_value*)(0)) - meta::s_ll(pc->at_pd0u<t_value>() - (t_value*)(0));
          if (d >= 0 && d < cap)
          {
              d -= pc->kbase; if (d < 0) { d += cap; }
              d += n0;
              if (d >= pct->_n) { return -1; }
              return s_long(d);
          }
          ++pc; n0 += cap;
        }
        return -1;
      }
    };








    template<class TA, class Ctnr = vec2_t<TA>, class Aux = meta::nothing> struct link2_t;

      // Integer result code, convertible to bool on_calm_result / failure.
    template<s_long eq_i> struct _result_eq { const s_long res; inline _result_eq(s_long res_) throw() : res(res_) {} inline operator bool() throw() { return res == eq_i; } };
    template<s_long ge_i> struct _result_ge { const s_long res; inline _result_ge(s_long res_) throw() : res(res_) {} inline operator bool() throw() { return res >= ge_i; } };


      // Strongly-typed vector with most of vecm features.
      //    Possible application:
      //    1) Replacement for std vector in algorithms with frequent 1 el. insertion/removal.
      //    2) As element in multilevel containers with efficient moving (vecm config_t gm true).
      //    3) Move/destroy-aware target container for permanent links (link2_t).
      //    4) High-level passing between binary modules.
    template<class TA> struct vec2_t : protected yk_c::vecm
    {
    public:
      typedef yk_c::s_long s_long; typedef yk_c::vecm vecm;
      typedef TA ta_value; typedef typename specf<ta_value>::t_value t_value; struct exc_vec2_t : std::exception { const char* _p; exc_vec2_t(const char* pmsg) throw() : _p(pmsg) {} const char* what() const throw() { return _p; } };
      typedef s_long size_type; typedef t_value value_type; typedef t_value& reference; typedef const t_value& const_reference; typedef s_long difference_type;
      typedef iterator_t<t_value, false> iterator; typedef iterator_t<t_value, true> const_iterator;
#ifndef _RWSTD_NO_CLASS_PARTIAL_SPEC
      typedef std::reverse_iterator<iterator> reverse_iterator; typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
#else
      typedef std::reverse_iterator<iterator, std::random_access_iterator_tag, t_value> reverse_iterator; typedef std::reverse_iterator<const_iterator, std::random_access_iterator_tag, const t_value> const_reverse_iterator;
#endif
      typedef vec2_t<ta_value> t_ctnr;
      template<class F> struct _fcheck { enum { result = meta::assert<meta::resolve_TF<F, meta::tag_construct>::use_functor>::result }; };

      // partial replication of std vector
      //==section 1==============================================

      inline vec2_t() throw() : vecm(yk_c::typer<_v2ta_a<ta_value> >, 0) {}

        // NOTE vec2_t(const vec2_t&) and operator= set all destination flags to dflt., not that of x.
        // NOTE If operator= fails (gen. exception), the container is left with 0 size and default flags.
      inline vec2_t(const t_ctnr& x) : vecm(yk_c::typer<_v2ta_a<ta_value> >, 0) { if (_l_copy(&x, x.rvecm(), 0) == 1) { return; } throw exc_vec2_t("vec2_t(c vec2_t&)"); }
      inline vec2_t& operator=(const t_ctnr& x) { if (vec2_copy(x, true) == 1) { return *this; } throw exc_vec2_t("vec2_t.operator="); }

        // Copying between vectors with same elem. type but different TA arg.
      template<class TA2> inline vec2_t(const vec2_t<TA2>& x) : vecm(yk_c::typer<_v2ta_a<ta_value> >, 0)
      {
        enum { __check = meta::assert<meta::same_t<typename vecm::specf<TA2>::t_value, t_value>::result>::result };
        if (_l_copy(&x, x.rvecm(), 0) == 1) { return; } throw exc_vec2_t("vec2_t(c vec2_t<TA>&)");
      }
      template<class TA2> inline vec2_t& operator=(const vec2_t<TA2>& x) { if (vec2_copy(x, true) == 1) { return *this; } throw exc_vec2_t("vec2_t.operator="); }

      inline explicit vec2_t(size_type m, const value_type& x) : vecm(yk_c::typer<_v2ta_a<ta_value> >, 0) { if (this->vecm::el_append_m(m, x) >= 0) { return; } throw exc_vec2_t("vec2_t(m, c T& x)"); }
      inline explicit vec2_t(s_long base, size_type m, const value_type& x) : vecm(yk_c::typer<_v2ta_a<ta_value> >, base) { if (this->vecm::el_append_m(m, x) >= 0) { return; } throw exc_vec2_t("vec2_t(base, m, c T& x)"); }
      template<class F> inline explicit vec2_t(size_type m, const F& x) : vecm(yk_c::typer<_v2ta_a<ta_value> >, 0) { enum { __check = _fcheck<F>::result }; if (this->vecm::el_append_m(m, x) >= 0) { return; } throw exc_vec2_t("vec2_t(m, c F& x)"); }
      template<class F> inline explicit vec2_t(s_long base, size_type m, const F& x) : vecm(yk_c::typer<_v2ta_a<ta_value> >, base) { enum { __check = _fcheck<F>::result }; if (this->vecm::el_append_m(m, x) >= 0) { return; } throw exc_vec2_t("vec2_t(base, m, c F& x)"); }

      ~vec2_t() throw() { if (!(this && _t_ind && _ptd && *_ptd->psig == *_ls_psig())) { return; } if (f_perm()) { _ptd2()->_p_tr_notify(this, 0, 0x1); } }

      inline size_type size() const throw() { return _n; }
      inline reference operator[] (size_type i) { return *this->vecm::pval_0u<t_value>(i); } // ignores nbase()
          inline const_reference operator[] (size_type i) const { return *this->vecm::pval_0u<t_value>(i); } // -"-
      inline reference at (size_type i) { t_value* p = this->vecm::pval<t_value>(i + _nbase); if (p) { return *p; } throw exc_vec2_t("vec2_t.at"); } // -"-
          inline const_reference at (size_type i) const { t_value* p = this->vecm::pval<t_value>(i + _nbase); if (p) { return *p; } throw exc_vec2_t("vec2_t.at c"); } // -"-
      inline reference front() { t_value* p = this->vecm::pval_first<t_value>(); if (p) { return *p; } throw exc_vec2_t("vec2_t.front"); }
          inline const_reference front() const { t_value* p = this->vecm::pval_first<t_value>(); if (p) { return *p; } throw exc_vec2_t("vec2_t.front c"); }
      inline reference back() { t_value* p = this->vecm::pval_last<t_value>(); if (p) { return *p; } throw exc_vec2_t("vec2_t.back"); }
          inline const_reference back() const { t_value* p = this->vecm::pval_last<t_value>(); if (p) { return *p; } throw exc_vec2_t("vec2_t.back c"); }
      inline void push_back(const value_type& x) { if (this->vecm::el_append(x)) { return; }  throw exc_vec2_t("vec2_t.push_back"); }
      inline void pop_back() { if (this->vecm::el_remove_last<t_value>() > 0) { return; } throw exc_vec2_t("vec2_t.pop_back"); }
        // NOTE clear(), vec2_clear() set all to default, except:
        //    1. nbase() is kept.
        //    2. Permanent links mode (f_perm(), f_sync()) does not change.
        //      If it is enabled, the container may stay registered as perm. link target.
      inline void clear() throw() { vecm_clear(); }
        inline s_long vec2_clear() throw() { s_long nx = vecm_clear(); return nx; }

      inline bool empty() const throw() { return _n == 0; }
      inline size_type capacity() const throw() { return this->vecm::nrsv(); }
      inline void reserve(size_type n2) { if (n2 >= 0 && this->vecm::el_reserve_n(n2, _f_can_shrink())) { return; } throw exc_vec2_t("vec2_t.reserve"); }
      inline void resize(size_type n2) { if (n2 > _n) { s_long m = n2 - _n; if (this->vecm::el_append_m(m, meta::construct_f<t_value>()) == m) { return; } } else if (n2 >= 0) { s_long m = _n - n2; if (this->vec2_t::el_remove_ml(_nbase + n2, m) == m) { return; } } throw exc_vec2_t("vec2_t.resize(n2)"); }
      inline void resize(size_type n2, const value_type& x) { if (n2 > _n) { s_long m = n2 - _n; if (this->vecm::el_append_m(m, x) == m) { return; } } else if (n2 >= 0) { s_long m = _n - n2; if (this->vec2_t::el_remove_ml(_nbase + n2, m) == m) { return; } } throw exc_vec2_t("vec2_t.resize(n2, x)"); }
      inline void swap(vec2_t& x) { if (_ptd2()->_pswap(this, &x)) { return; } throw exc_vec2_t("vec2_t.swap"); }

      iterator insert(const iterator& pos, const t_value& x) { if (pos.pcontainer() == this) { s_long res = this->el_insert_1(pos.ind(), x); if (res == 1) { return iterator(*this, pos.ind()); } } throw exc_vec2_t("vec2_t.insert(pos, x)"); }
      void insert(const iterator& pos, size_type m, const t_value& x) { if (pos.pcontainer() == this) { if (m == 0) { return; } if (m > 0) { s_long res = this->el_insert_ml(pos.ind(), m, x); if (res > 0) { return; } } } throw exc_vec2_t("vec2_t.insert(pos, m, x)"); }
      template<class Itr> void insert(const iterator& pos, const Itr& s0, const Itr& s2) { if (pos.pcontainer() == this && s2 >= s0) { s_long m = 0; Itr x1 = s0; while (x1 != s2 && ++m > 0) { ++x1; } if (m == 0) { return; } if (m > 0) { s_long res = this->el_insert_ml(pos.ind(), m, meta::construct_f<_v2insert_arg<t_value, typename meta::nonc_t<Itr>::t > >(s0)); if (res > 0) { return; } } } throw exc_vec2_t("vec2_t.insert(pos, s0, s2)"); }
      iterator erase(const iterator& dest) { if (dest.pcontainer() == this) { s_long res = this->el_remove_1(dest.ind(), false); if (res == 1) { return iterator(*this, dest.ind()); } } throw exc_vec2_t("vec2_t.erase(dest)"); }
      iterator erase(const iterator& dest0, const iterator& dest2) { if (dest0.pcontainer() == this && dest2.pcontainer() == this && dest2 >= dest0) { s_long m = dest2.ind0() - dest0.ind0(); if (m == 0) { return iterator(*this, dest0.ind()); } if (m > 0) { s_long res = this->el_remove_ml(dest0.ind(), m); if (res == m) { return iterator(*this, dest0.ind()); } } } throw exc_vec2_t("vec2_t.erase(dest0, dest2)"); }

      inline iterator begin() throw() { return iterator(*this, _nbase); }
      inline iterator end() throw() { return iterator(*this); }
        inline const_iterator begin() const throw() { return const_iterator(*this, _nbase); }
        inline const_iterator end() const throw() { return const_iterator(*this); }
      inline reverse_iterator rbegin() throw() { return reverse_iterator(iterator(*this)); }
      inline reverse_iterator rend() throw() { return reverse_iterator(iterator(*this, _nbase)); }
        inline const_reverse_iterator rbegin() const throw() { return const_reverse_iterator(const_iterator(*this)); }
        inline const_reverse_iterator rend() const throw() { return const_reverse_iterator(const_iterator(*this, _nbase)); }

      // replication of vecm (all funtions are exceptionless)
      //==section 2==============================================

        // NOTE Casting vec2_t  to vecm may be convenient in a number of applications.
        //    NOTE The following vecm operations are unsafe in vec2_t:
        //    1) vecm_copy, vecm_delete.
        //    2) el_insert*, el_remove* on f_perm() == true.
        //    For them, calling analogs in vec2_t should be preferred.
      inline yk_c::vecm& rvecm() throw() { return *this; }
      inline const yk_c::vecm& rvecm() const throw() { return *this; }

      inline s_long n() const throw() { return _n; }
      inline s_long nbase() const throw() { return _nbase; }
      inline s_long nrsv() const throw() { return m::_nrsv(_nj); }
      inline bool can_shrink() const throw() { return _f_can_shrink(); }
      inline bool is_transactional() const throw() { return this->vecm::is_transactional(); }
      inline s_long nexc() const throw() { return _nxf >> _xsh; }
      inline s_long integrity() const throw() { return this->vecm::integrity(); }
      inline s_long locality() const throw() { return this->vecm::locality(); }
      inline s_long compatibility() const throw()
      {
          s_long c = this->vecm::compatibility(); if (c <= -2) { return c; }
          s_long nx = _ptd2()->version; s_long n = _vec2_td_ver::ver; if ((nx & 0xffff00) != (n & 0xffff00)) { return -3; }
          if (_cv0() >= 0) { return c; }
          n &= 0xff; nx &= 0xff; if (n > nx) { n = nx; }
          return bytes::is_eq_str<s_long>(_ptd2()->psig, _vec2_td_ver::psig(), n, n) ? c : -3;
      }
      inline const type_descriptor* ptd() const throw() { return this->vecm::ptd(); }
      inline s_long null_state(s_long ind) const throw() { return this->vecm::null_state(ind); }
      inline void vec2_set_nbase(s_long nbase) throw() { _nbase = nbase; }
      inline void vec2_setf_can_shrink(bool x) throw() { _setf_can_shrink(x); }
      inline void vec2_set0_nexc() const throw() { _nxf &= _fm; }
      inline void vec2_setf_unsafe(bool x) const throw() { _setf_unsafe(x); }

        // Container copy. Behaves same as vecm_copy.
        //  NOTE vec2_copy() does not reset perm. links mode or unregister the container.
        //  NOTE (!) is_tr false may not be used for copying between container and its own element,
        //    even indirectly. (This is possible with polymorphic types.)
        //  Returns: 1, 0, -1, -3 -- same as vecm_copy. No other values are returned.
      template<class TA2> inline _result_eq<1> vec2_copy(const vec2_t<TA2>& x, bool is_tr) throw()
      {
        enum { __check = meta::assert<meta::same_t<typename vecm::specf<TA2>::t_value, t_value>::result>::result };
        if (!(this && _t_ind && _ptd && *_ptd->psig == *_ls_psig())) { return -3; } return _ptd2()->pvec2_copy(this, &x, &x.rvecm(), s_long(is_tr) | 0x2 );
      }
        // Returns: 1, 0, -1 -- same as vecm_delete.
      inline _result_ge<0> vec2_delete() throw() { if (!(this && _t_ind && _ptd && *_ptd->psig == *_ls_psig())) { return -1; } return _ptd2()->pvec2_delete(this); }

      inline s_long el_remove_all() throw() { return this->vecm::el_remove_all(); }
      inline bool el_reserve_n(s_long n, bool allow_shrink) throw() { return this->vecm::el_reserve_n(n, allow_shrink); }
      inline t_value* el_expand_1() throw() { return this->vecm::el_expand_1<t_value>(); }
      inline bool el_expand_n(s_long n2) throw() { return _ptd2()->_pexpand_n(this, n2); }
      inline bool el_expunge_last() throw() { if (f_perm()) { _ptd2()->_pexpand_n(this, n() - 1); } return this->vecm::el_expunge_last<t_value>(); }
      inline t_value* el_append(const t_value& x) throw() { return this->vecm::el_append(x); }
        template<class F> inline t_value* el_append(const F& x) throw() { enum { __check = _fcheck<F>::result }; return this->vecm::el_append(x); }
      inline s_long el_insert_1(s_long ind, const t_value& x) throw() { if (!f_perm()) { return this->vecm::el_insert_1(ind, x); } else { _transaction t(this, _nrsv_tr_1(ind)); if (!t) { return -2; } s_long res = this->vecm::el_insert_1(ind, x); t.end(res == 1 || res == -4); return res; } }
        template<class F> inline s_long el_insert_1(s_long ind, const F& x) throw() { enum { __check = _fcheck<F>::result }; if (!f_perm()) { return this->vecm::el_insert_1(ind, x); } else { _transaction t(this, _nrsv_tr_1(ind)); if (!t) { return -2; } s_long res = this->vecm::el_insert_1(ind, x); t.end(res == 1 || res == -4); return res; } }
      inline s_long el_remove_last() throw() { return this->vecm::el_remove_last<t_value>(); }
      inline s_long el_remove_1(s_long ind, bool move_last) throw() { if (!f_perm()) { return this->vecm::el_remove_1<t_value>(ind, move_last); } else { _transaction t(this, move_last ? 2 : _nrsv_tr_1(ind)); if (!t) { return -2; } s_long res = this->vecm::el_remove_1<t_value>(ind, move_last); t.end(res == 1 || res == -4); return res; } }
      inline s_long el_append_m(s_long m, const t_value& x) throw() { return this->vecm::el_append_m(m, x); }
        template<class F> inline s_long el_append_m(s_long m, const F& x) throw() { enum { __check = _fcheck<F>::result }; return this->vecm::el_append_m(m, x); }
      inline s_long el_insert_ml(s_long ind, s_long m, const t_value& x) throw() { if (!f_perm()) { return this->vecm::el_insert_ml(ind, m, x); } else { _transaction t(this, _nrsv_tr_m(ind, 0)); if (!t) { return -2; } s_long res = this->vecm::el_insert_ml(ind, m, x); t.end(res > 0 || res == -4); return res; } }
        template<class F> inline s_long el_insert_ml(s_long ind, s_long m, const F& x) throw() { enum { __check = _fcheck<F>::result }; if (!f_perm()) { return this->vecm::el_insert_ml(ind, m, x); } else { _transaction t(this, _nrsv_tr_m(ind, 0)); if (!t) { return -2; } s_long res = this->vecm::el_insert_ml(ind, m, x); t.end(res > 0 || res == -4); return res; } }
      inline s_long el_remove_ml(s_long ind, s_long m) throw() { if (!f_perm()) { return this->vecm::el_remove_ml<t_value>(ind, m); } else { _transaction t(this, _nrsv_tr_m(ind, m)); if (!t) { return -2; } s_long res = this->vecm::el_remove_ml<t_value>(ind, m); t.end(res > 0 || res == -4); return res; } }

      template<class T> struct checked_ptr
      {
        T* p;
        inline checked_ptr(T* p_) throw() : p(p_) {}
        inline T& operator*() const throw(exc_vec2_t) { check(); return *p; }
        inline operator T*() const throw() { return p; }
        inline operator bool() const throw() { return bool(p); }
        inline T* operator->() const throw(exc_vec2_t) { check(); return p; }
        inline bool operator==(const checked_ptr& p2) throw() { return p == p2.p; }
        inline bool operator==(const T* p2) throw() { return p == p2; }
        inline bool operator!=(const checked_ptr& p2) throw() { return p == p2.p; }
        inline bool operator!=(const T* p2) throw() { return p != p2; }

        inline void check() const throw(exc_vec2_t) { if (!p) { throw exc_vec2_t("vec2_t.checked_ptr"); } }
      };

        // pval: vecm pval analog. ind is nbase-based.
        // pc: vecm pval analog, returning checked pointer. ind is nbase-based.
        // pval_0u: vecm pval_0u and vec2 operator [ ] analog. i is 0-based.
        // pval_first: vecm pval_first and vec2 front analog.
        // pval_last: vecm pval_last and vec2 back analog.
      inline const t_value* pval(s_long ind) const throw() { return this->vecm::pval<t_value>(ind); }
      inline checked_ptr<const t_value> pc(s_long ind) const throw() { return this->vecm::pval<t_value>(ind); }
      inline const t_value* pval_0u(s_long i) const throw() { return this->vecm::pval_0u<t_value>(i); }
      inline const t_value* pval_first() const throw() { return this->vecm::pval_first<t_value>(); }
      inline const t_value* pval_last() const throw() { return this->vecm::pval_last<t_value>(); }

      inline t_value* pval(s_long ind) throw() { return this->vecm::pval<t_value>(ind); }
      inline checked_ptr<t_value> pc(s_long ind) throw() { return this->vecm::pval<t_value>(ind); }
      inline t_value* pval_0u(s_long i) throw() { return this->vecm::pval_0u<t_value>(i); }
      inline t_value* pval_first() throw() { return this->vecm::pval_first<t_value>(); }
      inline t_value* pval_last() throw() { return this->vecm::pval_last<t_value>(); }

      inline link1_t<t_value, false> link1_begin() throw() { return this->vecm::link1_begin<t_value>(); }
      inline link1_t<t_value, false> link1_aend() throw() { return this->vecm::link1_aend<t_value>(); }
      inline link1_t<t_value, true> link1_cbegin() const throw() { return this->vecm::link1_cbegin<t_value>(); }
      inline link1_t<t_value, true> link1_caend() const throw() { return this->vecm::link1_caend<t_value>(); }

      // permanent links in vec2_t
      //==section 3==============================================

        // Switching permanent links mode (enabled / disabled, synchronized / non-synchronized access).
        // forced_change true: unconditionally sets the new mode.
        //    All existing links will be lost (return invalid iterators)
        //      if sync. flag is inverted or perm. flag changes to false.
        //    If the current f_perm() is false, forced_change flag does nothing.
        //  forced_change false:
        //    a) successfully enables perm. links in this container, if it was not enabled,
        //    b) does nothing if the new mode is same as the current,
        //    c) disables perm. links in this container if no valid links exist.
        //  sync is ignored on perm == false.
        //  Returns:
        //    2 - perm. links are enabled / disabled / mode changed successfully.
        //    1 - links mode has been changed successfully (sync <--> non-sync.), forcedly. Previous links were lost.
        //    0 - new mode is same as the current. Nothing done.
        //    -1 - failed to set new mode. The existing mode and links are kept.
      s_long link2_setf(bool perm, bool sync, bool forced_change) throw()
      {
        if (locality() != 1) { return -1; }
        bool sync0 = f_sync();
        s_long res = 2;
        if (f_perm())
        {
          if (perm && sync == sync0) { return 0; }
          _link2_reg* pl2 = _link2_reg::ref(f_sync()); res = pl2 ? pl2->detach_p(this, forced_change) : 0; if (res < 0) { return -1; }
          res = res == 1 ? 1 : 2;
        }
        else { if (!perm) { return 0; } }
        if (perm) { _ptd = sync ? &yk_c::typer<_v2ta_b<ta_value, t_ctnr, true> >() : &yk_c::typer<_v2ta_b<ta_value, t_ctnr, false> >(); }
          else { _ptd = &yk_c::typer<_v2ta_a<ta_value> >(); }
        return res;
      }

        // true: the container is enabled for permanent links.
        // false (default): the container does not allow permanent links.
      inline bool f_perm() const throw() { return _ptd2()->link2_flags & 0x1; }

        // true: permanent links are processed inside critical sections.
        // false (default): permanent links are processed without thread synchronization,
        //    assuming that all vec2_t containers with f_sync() == false
        //    are called only by one thread in application.
        //  NOTE This flag is false when f_perm() == false.
      inline bool f_sync() const throw() { return _ptd2()->link2_flags & 0x2; }

        // Returns:
        //  a) valid link to ind0-th element or after-end pos (n()).
        //    NOTE ind0 is 0-based.
        //  b) an invalid link on
        //    - incorrect i,
        //    - non-local container,
        //    - non-local target element (vecm or vec2_t),
        //    - container with f_perm() == false. See also link2_setf().
      link2_t<ta_value, t_ctnr> link2(s_long ind0) throw() { return link2_t<ta_value, t_ctnr>(*this, ind0); }

    private:
      struct _transaction
      {
          _transaction(t_ctnr* pct_, s_long nrsv) throw() : _pct(pct_), _pd(pct_->_ptd2()), _b(false) { start(nrsv); }
          ~_transaction() throw() { end(false); }
          operator bool() const throw() { return _b; }
          void start(s_long nrsv) throw() {  if (!_b && _pct->_cv0() >= 0 && _pd->_p_tr_start_2) { _b =  bool(_pd->_p_tr_start_2(_pct, nrsv)); } }
          void end(bool commit) throw() { if (_b) { _b = false; _pd->_p_tr_end_2(_pct, commit); } }
      private: t_ctnr* _pct; _vec2_td* _pd; bool _b;
      };
      inline _vec2_td* _ptd2() const throw() { return reinterpret_cast<_vec2_td*>(_ptd->ptd2); }
      inline int _cv0() const throw() // if ver. 0 && sig. length 1, return ptd length (cur. mod.) - ptd length (this); else -1
        { s_long nx = _ptd2()->version; s_long n = _vec2_td_ver::ver; if (nx == 1 && n == 1)  { s_long ndx = _ptd2()->psig[0] >> 8; s_long nd = sizeof(_vec2_td); if (nd >= ndx) { return nd - ndx; } } return -1; }
      inline  s_long _nrsv_tr_1(s_long ind) { ind -= _nbase; if (! (ind >= 0 && ind <= _n)) { return 0; }  s_long j = 0, k = 0; vecm::m::_ind_jk(ind, j, k); return _last_j + 1 - j; }
      inline  s_long _nrsv_tr_m(s_long ind, s_long mdel) { ind -= _nbase; if (! (ind >= 0 && ind <= _n)) { return 0; } if (mdel < 0) { mdel = 0; }  s_long q = _n - ind - mdel; if (q < 0) { q = 0; } return q; }
        // mode flags:
        //    0x1 -- same as is_tr in vecm_copy.
        //    0x2 -- keep perm. links mode and registration for this container.
        //        (Otherwise perm. flags are switched off).
      inline s_long _l_copy(const void* pctnr, const vecm& x, s_long mode) throw()
      {
        if (this == pctnr) { return 1; } typedef t_ctnr Q; enum { _nq = sizeof(Q), _nst = 1 + _nq / sizeof(meta::s_ll) };
        meta::s_ll _st[_nst]; Q* p = reinterpret_cast<Q*>(_st); s_long res = p->vecm::_l_cc(x);
        if ((res >= 0 || (res == -1 && (mode & 0x1) == 0)) && p->_t_ind == _t_ind)
        {
          if (mode & 0x2) { const type_descriptor* ptd1 = _ptd; this->vecm::~vecm(); bytes::memmove_t<Q>::F(this, p, _nq); _ptd = ptd1; }
            else { this->~Q(); bytes::memmove_t<Q>::F(this, p, _nq); _ptd = &yk_c::typer<_v2ta_a<ta_value> >(); }
          return res >= 0 ? 1 : -1;
        }
        p->vecm::~vecm(); if (mode & 0x1) { return 0; }
        clear(); if ((mode & 0x2) == 0) { link2_setf(false, false, true); } return -1;
      }

      friend struct vecm::_config_ff::ptd2_t<_v2ta_a<ta_value> >;
      friend struct vecm::_config_ff::ptd2_t<_v2ta_b<ta_value, t_ctnr, true> >;
      friend struct vecm::_config_ff::ptd2_t<_v2ta_b<ta_value, t_ctnr, false> >;
      friend struct _ct_access<ta_value, t_ctnr>;

      static inline s_long _ls_copy(void* pdest, const void* psrc, const vecm* pvecm, s_long mode) throw() { if (!(psrc && pdest)) { return -3; } return reinterpret_cast<t_ctnr*>(pdest)->_l_copy(psrc, *pvecm, mode); }
      static inline s_long _ls_delete(const void* p) throw() { try { delete reinterpret_cast<const t_ctnr*>(p); return 1; } catch (...) { return 0; } }
      static inline s_long _ls_expand_n(void* p, s_long n2) throw()
      {
        t_ctnr* pct = reinterpret_cast<t_ctnr*>(p); if (!(pct->_t_ind && n2 >= 0)) { return false; }
        if (n2 < pct->_n && pct->f_perm()) { link1_t<t_value, false> l(*pct, pct->_nbase + n2); while (!l.is_aend()) { _link2_reg* pl2 = _link2_reg::ref(pct->f_sync()); if (pl2) { pl2->detach_p(l.pval(), true); } l.incr(); } }
        return pct->vecm::el_expand_n(n2);
      }
      static inline s_long _ls_swap(void* p1, void* p2) throw()
      {
        typedef t_ctnr Q; enum { _nq = sizeof(Q), _nst = 1 + _nq / sizeof(meta::s_ll) };
        Q* pct1 = reinterpret_cast<Q*>(p1); Q* pct2 = reinterpret_cast<Q*>(p2);
        if (pct1->locality() != 1 || pct2->locality() != 1) { return false; }
        meta::s_ll _st[_nst]; Q* p = reinterpret_cast<Q*>(_st); meta::safemove_t<t_ctnr>::F(p, pct1); meta::safemove_t<t_ctnr>::F(pct1, pct2); meta::safemove_t<t_ctnr>::F(pct2, p);
        return true;
      }
    };

    template<class TA> inline bool operator==(const vec2_t<TA>& a, const vec2_t<TA>& b)
    {
      typedef typename vec2_t<TA>::t_value T;
      if (a.n() != b.n()) { return false; }
      if (a.n() == 0) { return true; }
      vecm::link1_t<T, true> la(a.link1_cbegin()); vecm::link1_t<T, true> lb(b.link1_cbegin());
      do { if (!(*la.pval() == *lb.pval())) { return false; } la.incr(); } while (lb.incr());
    }
    template<class TA> inline bool operator!=(const vec2_t<TA>& a, const vec2_t<TA>& b) { return !(a == b); }

    template<class TA> inline bool operator<(const vec2_t<TA>& a, const vec2_t<TA>& b)
    {
      typedef typename vec2_t<TA>::t_value T;
      if (a.n() == 0) { return b.n() > 0; } if (b.n() == 0) { return false; }
      vecm::link1_t<T, true> la(a.link1_cbegin()); vecm::link1_t<T, true> lb(b.link1_cbegin());
      do { _yk_reg const T* p1 = la.pval(); _yk_reg const T* p2 = lb.pval(); if (!p1) { return bool(p2); } if (!p2) { return false; } if (*p1 < *p2) { return true; } if (*p2 < *p1) { return false; } la.incr(); lb.incr(); } while (true);
    }
    template<class TA> inline bool operator>(const vec2_t<TA>& a, const vec2_t<TA>& b) { return b < a; }
    template<class TA> inline bool operator<=(const vec2_t<TA>& a, const vec2_t<TA>& b) { return !(b < a); }
    template<class TA> inline bool operator>=(const vec2_t<TA>& a, const vec2_t<TA>& b) { return !(a < b); }

    template<class TA> inline void swap(vec2_t<TA>& a, vec2_t<TA>& b) { a.swap(b); }

    namespace _utils
    {
        // each_f is a construction functor, iterating once through all elements in a source container.
        //    It is created from a container object, and points at once to its first element.
        //    .size() is the number of elements in the container.
        //    .m1() is the number of elements already iterated.
        //    .m2() is the number of elements left. m1() + m2() == size().
        // each_f may be passed to: el_append, el_append_m, el_insert_1, el_insert_ml.
      template<class TA, class Ctnr, class Aux = meta::nothing> struct each_f
      {
        typedef typename Ctnr::size_type t_size; typedef typename vecm::specf<TA>::t_value t; struct exc_each_range {};
        inline each_f(const Ctnr& ct_) : ct(ct_), _m(0), i(ct.begin()) {} inline void f(t* p) const { if (is_end()) { throw exc_each_range(); } new (p) t(*i); ++_m; ++i; }
        inline t_size size() const { return ct.size(); } inline t_size m1() const { return _m; } inline t_size m2() const { return ct.size() - _m; } inline bool is_end() const { return _m >= ct.size(); }
        private: const Ctnr& ct; mutable t_size _m; mutable typename Ctnr::const_iterator i;
        typedef typename meta::template type_equi<each_f, meta::tag_construct>::t_3 __check1; typedef typename meta::template type_equi<each_f, meta::tag_functor>::t_3 __check2;
      };
      template<class TA, class Aux> struct each_f<TA, vecm, Aux>
      {
        typedef s_long t_size; typedef typename vecm::specf<TA>::t_value t; struct exc_each_range {};
        inline each_f(const vecm& ct_) : ct(ct_), _m(0), l(ct.link1_cbegin<t>()) {} inline void f(t* p) const { if (l.is_out()) { throw exc_each_range(); } new (p) t(*l.pval()); ++_m; l.incr(); }
        inline t_size size() const { return ct.n(); } inline t_size m1() const { return _m; } inline t_size m2() const { return ct.n() - _m; } inline bool is_end() const { return _m >= ct.n(); }
        private: const vecm& ct; mutable s_long _m; mutable vecm::link1_t<t, true> l;
        typedef typename meta::template type_equi<each_f, meta::tag_construct>::t_3 __check1; typedef typename meta::template type_equi<each_f, meta::tag_functor>::t_3 __check2;
      };

        // Appending an element to a container. Returns true on on_calm_result, false on failure.
      template<class T, class Ctnr, class Aux = meta::nothing> struct append_t { static inline bool F(Ctnr& ct, const T& x) throw() { try { ct.push_back(x); } catch (...) { return false; } return true; } };
      template<class TF, class Aux> struct append_t<TF, vecm, Aux> { static inline bool F(vecm& ct, const TF& x) throw() { return bool(ct.el_append(x)); } };

        // Removing all elements from a container. Returns true on on_calm_result, false on failure.
      template<class Ctnr, class Aux = meta::nothing> struct clear_t { static inline bool F(Ctnr& ct) throw() { try { ct.clear(); } catch (...) { return false; } return true; } };
      template<class Aux> struct clear_t<vecm, Aux> { static inline bool F(vecm& ct) throw() { ct.vecm_clear(); return true; } };

      struct exc_conv { const s_long res; exc_conv(s_long res_) : res (res_) {} };

      template<class Ctnr2, class Ctnr1> struct _copy_t {};
      template<class TA2, class Ctnr1> struct _copy_t<vec2_t<TA2>, Ctnr1>
      {
        static inline _result_eq<1> F(vec2_t<TA2>& dest, const Ctnr1& src, s_long mode)
        {
          if (dest.locality() != 1 && dest.compatibility() < 1) { if (mode >= 10) { throw exc_conv(-20000 - 10 * mode - 1); } return -1; }
          if ((mode & 1) == 0) { dest.clear(); }
          each_f<TA2, Ctnr1> e(src);
          while(!e.is_end()) { if (!dest.el_append(e)) { if (mode >= 10) { throw exc_conv(-20000 - 10 * mode - 0); } return 0; } }
          return 1;
        }
      };
      template<class TA2, class TA1> struct _copy_t<vec2_t<TA2>, vec2_t<TA1> >
      {
        static inline _result_eq<1> F(vec2_t<TA2>& dest, const vec2_t<TA1>& src, s_long mode = 0)
            {
              if (src.locality() != 1 && src.compatibility() < 1) { if (mode >= 10) { throw exc_conv(-30000 - 10 * mode - 2); } return -1; }
              if (dest.locality() != 1 && dest.compatibility() < 1) { if (mode >= 10) { throw exc_conv(-30000 - 10 * mode - 1); } return -1; }
              if ((mode & 1) == 0) { dest.clear(); }
              each_f<TA2, vec2_t<TA2> > e(src);
              while(!e.is_end()) { if (!dest.el_append(e)) { if (mode >= 10) { throw exc_conv(-30000 - 10 * mode - 0); } return 0; } }
              return 1;
            }
      };

        // Non-transactional copy and appending vec2_t <-- C or C <-- vec2_t, where C is arbitrary sequential container
        //  (std string, vector, list etc.; also vecm(t_value)).
        //  Requires existing conversion from Ctnr element type to t_value of this container or vice versa.
        //  Returns:
        //    mode 0 (exceptionless copy):
        //      1 - on_calm_result: dest is cleared, then all src elements copied to dest.
        //      0 - error during copying: dest is partially filled by copies of src elements.
        //      -1 - nothing done: dest is invalid or incompatible.
        //    mode 10 (copy):
        //      1 - on_calm_result: dest is cleared, then all src elements copied to dest.
        //      exception - on any error. dest may be untouched or contain a part of src elements.
        //    mode 1 (exceptionless append):
        //      1 - on_calm_result: all src elements appended to dest.
        //      0 - error during copying: part of src elements appended to dest.
        //      -1 - nothing done: dest is invalid or incompatible.
        //    mode 11 (append):
        //      1 - on_calm_result: all src elements appended to dest.
        //      exception - on any error. dest may be untouched or added a part of src elements.
      template<class TA, class Ctnr> inline _result_eq<1>
        copy (vec2_t<TA>& dest, const Ctnr& src, s_long mode = 0)
          { return _copy_t<vec2_t<TA>, Ctnr>::F(dest, src, mode); }
      template<class Ctnr, class Iter> inline _result_eq<1>
        copy (Ctnr& dest, Iter i0, const Iter& i2, s_long mode = 0)
          {
            if ((mode & 1) == 0) { if (!clear_t<Ctnr>::F(dest)) { if (mode >= 10) { throw exc_conv(-40000 - 10 * mode - 1); } return 0; } }
            typedef typename Ctnr::value_type T2; typedef typename Iter::value_type T1;
            if (meta::same_t<T2, T1>::result) { while (i0 != i2) { if (!append_t<T2, Ctnr>::F(dest, *i0)) { if (mode >= 10) { throw exc_conv(-40000 - 10 * mode - 2); } return 0; } ++i0; } }
              else { while (i0 != i2) { if (!append_t<T2, Ctnr>::F(dest, T2(*i0))) { if (mode >= 10) { throw exc_conv(-40000 - 10 * mode - 3); } return 0; } ++i0; } }
            return 1;
          }
      template<class Ctnr, class T1, bool bconst> inline _result_eq<1>
        copy (Ctnr& dest, vecm::link1_t<T1, bconst> l0, s_long mode = 0, bool reverse = false)
          {
            if ((mode & 1) == 0) { if (!clear_t<Ctnr>::F(dest)) { if (mode >= 10) { throw exc_conv(-40000 - 10 * mode - 4); } return 0; } }
            typedef typename Ctnr::value_type T2; const s_long dir = reverse ? -1 : 1;
            if (meta::same_t<T2, T1>::result) { while (!l0.is_out()) { if (!append_t<T2, Ctnr>::F(dest, *l0.pval())) { if (mode >= 10) { throw exc_conv(-40000 - 10 * mode - 5); } return 0; } l0.move_by(dir); } }
              else { while (!l0.is_out()) { if (!append_t<T2, Ctnr>::F(dest, T2(*l0.pval()))) { if (mode >= 10) { throw exc_conv(-40000 - 10 * mode - 6); } return 0; } l0.move_by(dir); } }
            return 1;
          }
      template<class Ctnr, class TA> inline _result_eq<1>
        copy (Ctnr& dest, const vec2_t<TA>& src, s_long mode = 0, bool reverse = false)
        {
          if (!(src.locality() == 1 || src.compatibility() > 0)) { if (mode >= 10) { throw exc_conv(-40000 - 10 * mode - 7); } return 0; }
          return copy(dest, src.link1_cbegin(), mode, reverse);
        }

        // Returns vec2_t(C) or C(vec2_t), where C is arbitrary sequential container
        //  (std string, vector, list etc.; also vecm(t_value)).
        //  Requires existing conversion from Ctnr element type to t_value of this container or vice versa.
        //  On conversion failure, throws an exception.
      template<class Ctnr> struct convert
      {
        template<class Ctnr2> static inline Ctnr
          F(const Ctnr2& x)
            { Ctnr dest; copy(dest, x, 10); return dest; }
      };
    }


      // Permanent link to vec2_t object or its element.
      //    Produces updated pointers and iterators to the target object
      //    even if it was moved, and null pointers / default iterators (is_valid() == false) if it is deleted.
      //  NOTE Permanent link cannot be created for container or element from another binary module.
      //  NOTE After container modification, link recalculation may take O(N^0.5).
      //    During container modification, any link state update takes O(const).
    template<class TA, class Ctnr, class Aux> struct link2_t
    {
      typedef TA t_ta; typedef Ctnr t_ctnr; typedef vecm::specf<t_ta> cfgt; typedef typename cfgt::t_value t_value;
      typedef _ct_access<t_ta, t_ctnr> t_ct_access;
      typedef typename t_ct_access::iterator iterator; typedef typename t_ct_access::const_iterator const_iterator;
      typedef _link2_reg t_reg; typedef link2_t<t_ta, t_ctnr, Aux> t_link;

      template<class Ctsub> struct _ctsub_t { static s_long Fmode(void* pv) throw() { return 0; } };
        template<class T> struct _ctsub_t<vec2_t<T> > { static s_long Fmode(vec2_t<T>* pv) throw() { return pv ? 1 * pv->f_perm() + 2 * pv->f_sync() : 0; } };

      // permanent link (link2_t)
      //==section 4==============================================

        // ind0 == 0..ct size -1 specifies a valid 0-based element index.
        //  ind0 == ct size creates a link to the after-end pos in c.
        //  ct must have locality 1 and permanent links enabled (ct.f_perm() == true).
      link2_t(t_ctnr& ct, s_long ind0) throw() : _id(-1), _sync(false)
      {
        if (!(t_ct_access::Fperm(ct) && t_ct_access::Flocal(ct))) { return; }
        const s_long n = t_ct_access::Fsize(ct);
        if (ind0 < 0 || ind0 > n) { return; }
        t_value* pv = t_ct_access::Fpval0(ct, ind0); if (pv && !_pvc_local<t_value>::F(pv)) { return; }
        s_long mode = _ctsub_t<t_value>::Fmode(pv); if (bool(mode & 0x1) && bool(mode & 0x2) != t_ct_access::Fsync(ct)) { return; }
        _sync = t_ct_access::Fsync(ct); _link2_reg* pl2 = t_reg::ref(_sync); _id = pl2 ? pl2->gc_id(&ct, pv, ind0 < n ? ind0 : -1) : -1;
      }

        // Creates an empty link.
      link2_t() throw() : _id(-1), _sync(false) {}

        // true if the link contains the default object id (-1).
        //  false -- the link contains some real object id.
        //    The referenced object may either valid or deleted, invalid etc.
        //    pct(), pv(), iter(), iter_c() should be checked to determine that.
      bool is_empty() const throw() { return _id == -1; }

        // pct() == 0 means that the link is not valid:
        //    a) The container is already deleted.
        //    b) The link has failed to initialize.
        //    c) The link is created empty.
        //    d) Other reasons.
        // pv() == 0 means that the referenced object is not avaliable.
        //    If pct() != 0 on pv() == 0, the referenced position is the after-end pos.
      t_ctnr* pct() const throw() { _link2_reg* pl2 = t_reg::ref(_sync); if (!pl2) { return 0; } return reinterpret_cast<t_ctnr*>(pl2->get_p(_id, true)); }
      t_value* pv() const throw() { _link2_reg* pl2 = t_reg::ref(_sync); if (!pl2) { return 0; } return reinterpret_cast<t_value*>(pl2->get_p(_id, false)); }

        // NOTE (when the iterator is factually link1_t.)
        //    In syncronous mode, iterator is created under link registry lock.
        //    Even if target container asynchronously changes, 0-based value
        //    of iterator position (link1_t ind0()) for an existing element remains constant.
        //    End pos., and also link1_t ind() and px() may be invalid under such conditions.
      iterator iter() const throw()        { _link2_reg* pl2 = t_reg::ref(_sync); if (!pl2) { return iterator(); } return pl2->template gr_iter<t_ctnr, t_ct_access, false>(_id); }
      const_iterator iter_c() const throw()        { _link2_reg* pl2 = t_reg::ref(_sync); if (!pl2) { return const_iterator(); } return pl2->template gr_iter<t_ctnr, t_ct_access, true>(_id); }

      link2_t(const link2_t& x) throw() : _id(x._id), _sync(x._sync) { _link2_reg* pl2 = t_reg::ref(_sync); if (pl2) { pl2->link_add(_id); } }
      ~link2_t() throw() { _link2_reg* pl2 = t_reg::ref(_sync); if (pl2) { pl2->link_remove(_id); } }
      link2_t& operator=(const link2_t& x) throw()
      {
        if (&x == this) { return *this; }
        if (_sync != x._sync) { _link2_reg* pl21 = t_reg::ref(x._sync); if (pl21) { pl21->link_add(x._id); } _link2_reg* pl22 = t_reg::ref(_sync); if (pl22) { pl22->link_remove(_id); } _id = x._id; _sync = x._sync; }
          else { _link2_reg* pl2 = t_reg::ref(_sync); if (pl2) { pl2->link_a_r(x._id, _id); } _id = x._id; }
        return *this;
      }

    private:
      meta::s_ll _id; bool _sync;

      template<class T, class _ = meta::nothing> struct _pvc_local { static inline bool F(T* p) throw() { return true; } };
      template<class T, class _> struct _pvc_local<vec2_t<T>, _> { static inline bool F(vec2_t<T>* p) throw() { return p ? p->locality() == 1 : true; } };
      template<class _> struct _pvc_local<vecm, _> { static inline bool F(vecm* p) throw() { return p ? p->locality() == 1 : true; } };
    };
  }
  using namespace experimental;


  // Additional type descriptors.

  struct __vecm_x: vecm { typedef vecm::_config_ff _config_ff; };
  template<class TA> struct __vecm_x::_config_ff::ptd2_t<_v2ta_a<TA> >
  {
    static _vec2_td td; static bool init;
    static void* F(const type_descriptor& parent, bool is_locked)
    {
      typedef vec2_t<TA> t_ctnr;
      if (!init)
      {
        td.version = _vec2_td_ver::ver;
        td.link2_flags = 0;
        td.psig = _vec2_td_ver::psig();
        td.pvec2_copy = t_ctnr::_ls_copy;
        td.pvec2_delete = t_ctnr::_ls_delete;
        td._p_tr_start = 0;
        td._p_tr_end = 0;
        td._p_tr_notify = 0;
        td._pexpand_n = t_ctnr::_ls_expand_n;
        td._pswap = t_ctnr::_ls_swap;
        td._p_tr_start_2 = 0;
        td._p_tr_end_2 = 0;
        init = true;
      }
      return &td;
    }
  };
    template<class TA> _vec2_td __vecm_x::_config_ff::ptd2_t<_v2ta_a<TA> >::td;
    template<class TA> bool __vecm_x::_config_ff::ptd2_t<_v2ta_a<TA> >::init(false);

  template<class TA, class Ctnr, bool bsync> struct __vecm_x::_config_ff::ptd2_t<_v2ta_b<TA, Ctnr, bsync> >
  {
    static _vec2_td td; static bool init;
    static void* F(const type_descriptor& parent, bool is_locked)
    {
      typedef Ctnr t_ctnr;
      if (!init)
      {
        td.version = _vec2_td_ver::ver;
        td.link2_flags = 0x1 | (bsync ? 0x2 : 0);
        td.psig = _vec2_td_ver::psig();
        td.pvec2_copy = t_ctnr::_ls_copy;
        td.pvec2_delete = t_ctnr::_ls_delete;
        td._p_tr_start = bsync ? _link2_reg::_ls_tr_start1 : _link2_reg::_ls_tr_start2;
        td._p_tr_end = bsync ? _link2_reg::_ls_tr_end1 : _link2_reg::_ls_tr_end2;
        td._p_tr_notify = bsync ? _link2_reg::_ls_tr_notify1 : _link2_reg::_ls_tr_notify2;
        td._pexpand_n = t_ctnr::_ls_expand_n;
        td._pswap = t_ctnr::_ls_swap;
        td._p_tr_start_2 = bsync ? _link2_reg::_ls_tr_start_21 : _link2_reg::_ls_tr_start_22;
        td._p_tr_end_2 = bsync ? _link2_reg::_ls_tr_end_21 : _link2_reg::_ls_tr_end_22;
        init = true;
      }
      return &td;
    }
  };
    template<class TA, class Ctnr, bool bsync> _vec2_td __vecm_x::_config_ff::ptd2_t<_v2ta_b<TA, Ctnr, bsync> >::td;
    template<class TA, class Ctnr, bool bsync> bool __vecm_x::_config_ff::ptd2_t<_v2ta_b<TA, Ctnr, bsync> >::init(false);


  // Type configurations for vec2_t elements.

  template<class TA, class Ctnr, bool b> struct vecm::spec<_v2ta_b<TA, Ctnr, b> >
  {
    typedef _v2ta_b<TA, Ctnr, b> ta_link;
    typedef typename vecm::spec<TA>::config cfgt;
    typedef typename cfgt::t_value t_value;
    struct aux : cfgt
    {
      typedef ta_link ta_meta_copy;
      typedef ta_link ta_meta_safecopy;
      typedef ta_link ta_meta_trymove;
      typedef ta_link ta_meta_safemove;
      typedef ta_link ta_meta_destroy;
    };
    enum
    {
      dmode = 3,
      mmode = cfgt::gm ? 3 : (cfgt::mmode == 1 ? 2 : cfgt::mmode),
      cmode = cfgt::cmode == 1 ? 2 : (cfgt::cmode == 4 ? 3 : cfgt::cmode)
    };
    typedef config_t<t_value, dmode, mmode, cmode, aux> config;
  };
    template<class TA, class Ctnr, bool b> struct meta::type_equi<_v2ta_b<TA, Ctnr, b>, typename vecm::specf<TA>::t_value> { typedef typename vecm::specf<TA>::t_value t_3; };
    template<class TA, class Ctnr, bool b, class Aux> struct bytes::type_index_t<_v2ta_b<TA, Ctnr, b>, Aux> : type_index_t<TA, Aux> {};

  template<class TA> struct vecm::spec<_v2ta_a<TA> > { typedef typename vecm::spec<TA>::config config; };
    template<class TA> struct meta::type_equi<_v2ta_a<TA>, typename vecm::specf<TA>::t_value> { typedef typename vecm::specf<TA>::t_value t_3; };
    template<class TA, class Aux> struct bytes::type_index_t<_v2ta_a<TA>, Aux> : type_index_t<TA, Aux> {};


  // Special configuration for vec2_t as element (custom moving and destruction).

  template<class TA> struct vecm::spec<vec2_t<TA> > { typedef config_t<vec2_t<TA>, 3, 3, 1> config; };


  // Special configuration for link2_t (byte level moving).

  template<class TA, class Ctnr, class Aux> struct vecm::spec<link2_t<TA, Ctnr, Aux> > { typedef config_t<link2_t<TA, Ctnr, Aux>, 1, 4, 1> config; };


  // Custom functions for vec2_t elements manipulation.

  template<class TA> struct meta::safemove_t<vec2_t<TA> > { typedef vec2_t<TA> V; static inline void F(V* pdest, V* psrc) throw() { typedef _link2_reg t_reg; bytes::memmove_t<V>::F(pdest, psrc, sizeof(V)); if (pdest->f_perm()) { _link2_reg* pl2 = t_reg::ref(pdest->f_sync()); if (pl2) { pl2->tr_notify(psrc, pdest, 0x2); } } } };
  template<class TA> struct meta:: destroy_t<vec2_t<TA> > { typedef vec2_t<TA> V; typedef _link2_reg t_reg; static inline void F(V* p) { bool b1 = p->f_perm(); bool b2 = p->f_sync(); p->~V(); if (b1) { _link2_reg* pl2 = t_reg::ref(b2); if (pl2) { pl2->tr_notify(p, 0, 0x1); } } } };

  template<class TA, class Ctnr, bool b> struct meta:: copy_t<_v2ta_b<TA, Ctnr, b> > { typedef _link2_reg t_reg; typedef vecm::specf<TA> cfg; struct exc_copy {}; typedef typename cfg::t_value T; static inline void F(T* pdest, const T* psrc) { s_long res = cfg::Fcopy_1(pdest, psrc); if (res < 0) { throw exc_copy(); } _link2_reg* pl2 = t_reg::ref(b); if (pl2) { pl2->tr_notify(const_cast<T*>(psrc), pdest, 0x4); } } };
  template<class TA, class Ctnr, bool b> struct meta:: safecopy_t<_v2ta_b<TA, Ctnr, b> > { typedef _link2_reg t_reg; typedef vecm::specf<TA> cfg; typedef typename cfg::t_value T; static inline void F(T* pdest, const T* psrc) throw() { cfg::Fcopy_1(pdest, psrc); _link2_reg* pl2 = t_reg::ref(b); if (pl2) { pl2->tr_notify(const_cast<T*>(psrc), pdest, 0x4); } } };
  template<class TA, class Ctnr, bool b> struct meta:: trymove_t<_v2ta_b<TA, Ctnr, b> > { typedef _link2_reg t_reg; typedef vecm::specf<TA> cfg; typedef typename cfg::t_value T; static inline s_long F(T* pdest, T* psrc) throw() { s_long res = cfg::Fmove_1(pdest, psrc); if (res >= 0) { _link2_reg* pl2 = t_reg::ref(b); if (pl2) { pl2->tr_notify(psrc, pdest, 0x2); } } return res; } };
  template<class TA, class Ctnr, bool b> struct meta:: safemove_t<_v2ta_b<TA, Ctnr, b> > { typedef _link2_reg t_reg; typedef vecm::specf<TA> cfg; typedef typename cfg::t_value T; static inline void F(T* pdest, T* psrc) throw() { cfg::Fmove_1(pdest, psrc); _link2_reg* pl2 = t_reg::ref(b); if (pl2) { pl2->tr_notify(psrc, pdest, 0x2); } } };
  template<class TA, class Ctnr, bool b> struct meta:: destroy_t<_v2ta_b<TA, Ctnr, b> > { typedef _link2_reg t_reg; typedef vecm::specf<TA> cfg; struct exc_destroy {}; typedef typename cfg::t_value T; static inline void F(T* p) { s_long res = cfg::Fdestroy_1(p); _link2_reg* pl2 = t_reg::ref(b); if (pl2) { pl2->tr_notify(p, 0, 0x1); } if (res < 0) { throw exc_destroy(); } } };

  template<class TA, class Ctnr, bool b> struct meta:: safemove_t<_v2ta_b<vec2_t<TA>, Ctnr, b> > { typedef vec2_t<TA> V; static inline void F(V* pdest, V* psrc) throw() { meta::safemove_t<V>::F(pdest, psrc); } };
  template<class TA, class Ctnr, bool b> struct meta:: destroy_t<_v2ta_b<vec2_t<TA>, Ctnr, b> > { typedef vec2_t<TA> V; static inline void F(V* p) { meta:: destroy_t<V>::F(p); } };


  // each_f tags.

  template<class T1, class T2, class T3> struct meta::type_equi<_utils::each_f<T1, T2, T3>, meta::tag_construct> { typedef meta::tag_construct t_3; };
  template<class T1, class T2, class T3> struct meta::type_equi<_utils::each_f<T1, T2, T3>, meta::tag_functor> { typedef meta::tag_functor t_3; };


  // Special construction functor for vec2_t insert.
  //  After creating each value, increments its copy of source iterator.

  template <class T, class Itr> struct meta::construct_f<_v2insert_arg<T, Itr> > { typedef T t; mutable Itr z; construct_f(const Itr& begin) : z(begin) {} inline void f(t* p) const { new (p) t(*z); ++z; } };








  namespace experimental
  {
      // Container analogous to std::map + keys hashing.
      //    Feature: access to entry by numeric index in order (operator()), or numeric index in hash (h()).
      //  Possible application:
      //      1. Percentile and median values calculation.
      //      2. Live charts with numeric position recalculation when chart members are frequently added and removed.
      //        (The order does not require recalculation.)
      //  Performance:
      //    1. Worst insertion/removal is O(N^0.5). Factual time is comparable with std::map on n < 10000. 2-5 times slower on n 10k..1M.
      //    2. Finding an entry position in the order O(log(N)). Factual time is comparable with std::map.
      //    3. Finding an entry ptr. without order O(1) as in usual hash.
      //  Requirements:
      //    1. Consistent dual comparison k2 < k, k < k2, where k2 is the key sought, k is a key in the container.
      //    2. Hash function and keys equality comparison. Equality consistent with "<" comparison.
    template<class KA, class VAF, class Less = bytes::less_t<typename vecm::specf<KA>::t_value, typename vecm::specf<KA>::t_value>, class Kf = hashx_common::kf_basic<typename vecm::specf<KA>::t_value> >
    struct ordhs_t
    {
      typedef KA t_ka; // key or its type equivalent
      typedef typename meta::resolve_TF<VAF, meta::tag_construct>::t t_va; // value or its type equivalent
      typedef vecm::specf<t_ka> cfgk; typedef vecm::specf<t_va> cfgv; // type configurations for key and value
      typedef typename cfgk::t_value t_k; // key
      typedef typename cfgv::t_value t_v; // value
      typedef typename meta::if_t<meta::resolve_TF<VAF, meta::tag_construct>::use_functor, VAF, meta::construct_f<t_v> >::result f_ctf; // construction functor for value
      typedef hashx_common::entry<t_ka, t_va> entry; // hash map entry
      typedef vecm::specf<entry> cfge; // type configuration for entry
      enum { no_elem = hashx_common::no_elem };

        // Single class for hash functions, key comparisons, possibly key type conversions:
      struct __f0 {}; struct __f1 : Kf, meta::same_t<Less, Kf, __f0, Less>::t {};
      typedef typename meta::same_t<Less, Kf, Less, __f1>::t f_kf;

      typedef hashx<KA, VAF, f_kf> t_hash;

        // Finding k in the order.
        //    O(log(N)) if pind_ord != 0.
        //    O(1) if pind_ord == 0.
        //  Returns:
        //    On on_calm_result / found, ptr. != 0 and *pind_ord containing entry index [0..n()-1] in the order specified by Less. *pind_h is the index of entry in the hash.
        //    On on_calm_result / not found, ptr. == 0 and *pind_ord containing index [0..n()] of the place of insertion. *pind_h == no_elem.
        //    On failure, ptr. == 0 and *pind_ord containing no_elem. *pind_h == no_elem.
      inline const entry* find(const t_k& k, s_long* pind_ord = 0, s_long* pind_h = 0) const throw()
      {
        if (pind_ord)
        {
          s_long i; s_long res = __inds().template _ord_find<s_long, t_k, _tlsbind>(k, &i, 0, _tlsbind(_d, *_d.pkf()));
          if (res == 1) { s_long i2 = * _inds.pval_0u<s_long>(i); *pind_ord = i; if (pind_h) { *pind_h = i2; } return _d(i2); }
          *pind_ord = res == 0 ? i : s_long(no_elem); if (pind_h) { *pind_h = no_elem; } return 0;
        }
        else
        {
          const f_kf* p = _d.pkf(); if (!p) { if (pind_h) { *pind_h = no_elem; } return 0; } const entry* e; _d.find2(k, *p, 0, &e, pind_h); return e;
        }
      }

        // Finds/inserts an entry with key k.
        //    O(1) if the entry exists and pind_ord == 0.
        //    O(log(N)) if the entry exists and pind_ord != 0.
        //    O(N^0.5) if the entry is inserted.
        //  Returns:
        //    1 - the entry has been inserted.
        //    0 - the entry already exists.
        //    -1 - an error occured. The container is not changed.
        //    On 1, 0, *pentry != 0. On -1, *pentry == 0.
        //    On 1, 0, *pind_ord, *pind_h contain a valid index. On -1, both of them are no_elem.
      s_long insert(const t_k& k, const entry** pentry = 0, s_long* pind_ord = 0, s_long* pind_h = 0) throw()
      {
        if (pind_ord)
        {
          do
          {
            s_long i; s_long res = __inds().template _ord_find<s_long, t_k, _tlsbind>(k, &i, 0, _tlsbind(_d, *_d.pkf()));
            const entry* e(0);
            if (res == 1) { s_long* p = _inds.pval_0u<s_long>(i); if (pentry) { *pentry = _d(*p); } *pind_ord = i; if (pind_h) { *pind_h = *p; } return 0; }
            if (res != 0) { break; }
            s_long i2;
            res = _d.insert(k, &e, &i2); if (res != 1) { break; } if (_inds.el_insert_1(i, i2) != 1) { _d.remove_i(i2); break; }
            if (pentry) { *pentry = e; } *pind_ord = i; if (pind_h) { *pind_h = i2; }
            return 1;
          }
          while (false);
          if (pentry) { *pentry = 0; } *pind_ord = no_elem; if (pind_h) { *pind_h = no_elem; } return -1;
        }
        else
        {
          do
          {
            const entry* e(0); s_long i2;
            const f_kf* p = _d.pkf(); if (!p) { break; } if (_d.find2(k, *p, 0, &e, &i2) == 1) { if (pentry) { *pentry = e; } if (pind_h) { *pind_h = i2; } return 0; }
            s_long i; s_long res = __inds().template _ord_find<s_long, t_k, _tlsbind>(k, &i, 0, _tlsbind(_d, *_d.pkf()));
            if (res != 0) { break; }
            res = _d.insert(k, &e, &i2); if (res != 1) { break; } if (_inds.el_insert_1(i, i2) != 1) { _d.remove_i(i2); break; }
            if (pentry) { *pentry = e; } if (pind_h) { *pind_h = i2; }
            return 1;
          }
          while (false);
          if (pentry) { *pentry = 0; } if (pind_h) { *pind_h = no_elem; } return -1;
        }
      }

        // Removing an entry.
        //    O(N^0.5) if the entry existed.
        //    O(1) if the entry did not exist.
        //  Returns:
        //    1 - the entry with k was removed.
        //    0 - the entry with k did not exist.
        //    -1 - an error occured. The container is not changed.
      s_long remove(const t_k& k) throw()
      {
        const entry* e(0); s_long i2;
        const f_kf* p = _d.pkf(); if (!p) { return -1; } s_long res = _d.find2(k, *p, 0, &e, &i2); if (res == 0) { return 0; } if (res != 1) { return -1; }
        s_long n9 = _d.n() - 1;
        if (i2 == n9)
        {
          s_long i9; res = __inds().template _ord_find<s_long, t_k, _tlsbind>(_d(n9)->k, &i9, 0, _tlsbind(_d, *_d.pkf()));
          if (res == 0) { i9 = -1; } else if (res != 1) { return -1; }
          if (_d.remove_e(e) != 1) { return -1; }
          if (i9 >= 0) { _inds.el_remove_1<s_long>(i9, false); } // always succeeds
        }
        else
        {
          s_long i9; res = __inds().template _ord_find<s_long, t_k, _tlsbind>(_d(n9)->k, &i9, 0, _tlsbind(_d, *_d.pkf()));
          if (res == 0) { i9 = -1; } else if (res != 1) { return -1; }
          s_long i; res = __inds().template _ord_find<s_long, t_k, _tlsbind>(k, &i, 0, _tlsbind(_d, *_d.pkf()));
          if (res == 0) { i = -1; } else if (res != 1) { return -1; }
          if (_d.remove_e(e) != 1) { return -1; }
          if (i9 >= 0) { *_inds.pval_0u<s_long>(i9) = i2; }
          if (i >= 0) { _inds.el_remove_1<s_long>(i, false); } // always succeeds
        }
        return 1;
      }

        // Same as hashx::remove_all.
      inline s_long remove_all() throw() { s_long m = _d.remove_all(); if (m < 0) { return m; } _inds.vecm_clear(); return m; }

        // Finds/inserts an entry with key k. Returns a reference to value.
        //    O(1) if the entry exists. O(N^0.5) if it is inserted.
      inline t_v& operator[](const t_k& k) throw (hashx_common::EHashOpExc)
      {
        do
        {
          const entry* e(0);
          const f_kf* p = _d.pkf(); if (!p) { break; } if (_d.find2(k, *p, 0, &e) == 1) { return e->v; }
          s_long i; s_long res = __inds().template _ord_find<s_long, t_k, _tlsbind>(k, &i, 0, _tlsbind(_d, *_d.pkf()));
          if (res != 0) { break; }
          s_long i2;
          res = _d.insert(k, &e, &i2); if (res != 1) { break; } if (_inds.el_insert_1(i, i2) != 1) { _d.remove_e(e); break; }
          return e->v;
        }
        while (false);
        throw hashx_common::hoxOpFailed;
      }

        // The number of elements in the container.
      inline s_long n() const throw() { return _d.n(); } // >=0

        // The number of errors since construction or the last assignment, clearing, or value reset.
      inline s_long nexc() const throw() { return _d.nexc(); }

        // Returns entry by index [0..n()-1] in the order.
      inline const entry* operator()(s_long ind_ord) const throw() { s_long* p = _inds.pval<s_long>(ind_ord); if (!p) { return 0; } return _d(*p); }

        // Returns same as hashx::operator().
      inline const entry* h(s_long ind_h) const throw() { return _d(ind_h); }

      ordhs_t() throw() : _d(), _inds(typer<s_long>, 0) {}
      ~ordhs_t() throw() {}

        // If copying fails, n() == 0, nexc() != 0.
      ordhs_t(const ordhs_t& x) throw() : _d(x._d), _inds(x._inds) { if (_d.n() != _inds.n()) { _d.hashx_clear(); _inds.vecm_clear(); } }

        // Constructs a copy of x.
        //    On on_calm_result, nexc() is set to 0.
        //    On failure nexc() is set to non-0, *this is not changed, no exceptions generated.
      ordhs_t& operator=(const ordhs_t& x) throw()
      {
        if (this == &x) { return *this; } typedef ordhs_t Q; enum { _nq = sizeof(Q), _nst = 1 + _nq / sizeof(meta::s_ll) };
        meta::s_ll _st[_nst]; Q* p = reinterpret_cast<Q*>(_st); new (p) Q(x);
        if (p->nexc()) { _set_nexc(p->nexc()); p->~Q(); return *this; }
        this->~Q(); bytes::memmove_t<Q>::F(this, p, _nq); return *this;
      }

        // Removes all elements, sets everything to default value.
      void ordhs_clear() throw() { _d.hashx_clear(); _inds.vecm_clear(); }

      inline void ordhs_set0_nexc() const throw () { _d.hashx_set0_nexc(); }

        // Links for iterating entries in hash order, analogous to that of hashx.
      inline vecm::link1_t<entry, true> link1_cbegin() const throw() { return _d.link1_cbegin(); }
      inline vecm::link1_t<entry, true> link1_caend() const throw() { return _d.link1_caend(); }
      inline vecm::link1_t<entry, true> link1_cat(s_long ind) const throw() { return _d.link1_cat(ind); }

        // Reference to combined key function, value constructor, key function setter.
        //    Just an interface to hashx pkf, pctf, hashx_set_kf.
      inline const f_kf* pkf() const throw() { return _d.pkf(); }
      inline f_ctf* pctf() const throw() { return _d.pctf(); }
      inline bool ordhs_set_kf(const f_kf& kf) throw() { return _d.hashx_set_kf(kf); }

      inline s_long compatibility() const throw() { const s_long ti = bytes::type_index_t<s_long>::ind(); s_long c = __crvx(_inds)._t_ind == ti && __crvx(__d()._ht)._t_ind == ti && sizeof(*this) == ((char*)&_inds - (char*)this) + sizeof(vecm) ? __d().vecm::compatibility() : -1; if (c == 0) { c = -1; } return c; }

    protected:
      t_hash _d;
      vecm _inds;

      typedef Less _t_less;
      struct _tlsbind { inline _tlsbind(const t_hash& h_, const _t_less& l_) throw() : h(h_), l(l_) {} const t_hash& h; const _t_less& l; inline bool less21(const t_k& k1, s_long i2) const { return l.less21(k1, h(i2)->k); } inline bool less12(s_long i1, const t_k& k2) const { return l.less12(h(i1)->k, k2); } };

      struct _Hx : t_hash { friend struct ordhs_t; inline void _set_nexc(s_long n) throw() { this->_ht.vecm_set0_nexc(); vecm::_nx_set(this->vecm::_nxf, n); } };
      inline void _set_nexc(s_long n) throw() { __d()._set_nexc(n); _inds.vecm_set0_nexc(); }
      inline _Hx& __d() throw () { return *static_cast<_Hx*>(&_d); }
      inline const _Hx& __d() const throw () { return *static_cast<const _Hx*>(&_d); }

      struct _Vx : vecm { friend struct ordhs_t; };
      inline _Vx& __inds() throw()  { return *static_cast<_Vx*>(&_inds); }
      inline const _Vx& __inds() const throw()  { return *static_cast<const _Vx*>(&_inds); }
      static const _Vx& __crvx(const vecm& v) throw() { return *static_cast<const _Vx*>(&v); }
    };
  }
  using namespace experimental;

  template<class T1, class T2, class T3, class T4> struct vecm::spec<ordhs_t<T1, T2, T3, T4> > { typedef ordhs_t<T1, T2, T3, T4> H; struct aux : vecm::config_aux<H> { }; typedef config_t<H, 1, 4, 2, aux> config; };

  template<class T1, class T2, class T3, class T4> struct meta::copy_t<ordhs_t<T1, T2, T3, T4> > { typedef ordhs_t<T1, T2, T3, T4> H; static inline void F(H* pdest, const H* psrc) { struct exc_copy {}; new (pdest) H(*psrc); if (pdest->nexc() != 0) { pdest->~H(); throw exc_copy(); } } };
}

#if defined(__clang__)
  #pragma clang diagnostic pop
#endif

#endif
