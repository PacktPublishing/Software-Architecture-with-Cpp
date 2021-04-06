
// Copyright (c) 2015 Noah Lopez
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef MSEMSTDVECTOR_H
#define MSEMSTDVECTOR_H

#include "msemsevector.h"
#include "msemstdarray.h"

/* Conditional definition of MSE_MSTDVECTOR_DISABLED was moved to msemsevector.h. */
//#ifdef MSE_SAFER_SUBSTITUTES_DISABLED
//#define MSE_MSTDVECTOR_DISABLED
//#endif /*MSE_SAFER_SUBSTITUTES_DISABLED*/

#ifdef _MSC_VER
#pragma warning( push )  
#pragma warning( disable : 4522 )
#endif /*_MSC_VER*/

#ifndef MSE_PUSH_MACRO_NOT_SUPPORTED
#pragma push_macro("MSE_THROW")
#pragma push_macro("_NOEXCEPT_OP")
#endif // !MSE_PUSH_MACRO_NOT_SUPPORTED

#ifdef MSE_CUSTOM_THROW_DEFINITION
#define MSE_THROW(x) MSE_CUSTOM_THROW_DEFINITION(x)
#else // MSE_CUSTOM_THROW_DEFINITION
#define MSE_THROW(x) throw(x)
#endif // MSE_CUSTOM_THROW_DEFINITION

#ifndef _NOEXCEPT_OP
#define _NOEXCEPT_OP(x)	noexcept(x)
#endif /*_NOEXCEPT_OP*/

namespace mse {

	namespace mstd {

#ifdef MSE_MSTDVECTOR_DISABLED
		template<class _Ty, class _A = std::allocator<_Ty> > using vector = std::vector<_Ty, _A>;

		namespace ns_vector {

			struct dummy_xscope_structure_lock_guard {};

			template<class _Ty, class _A = std::allocator<_Ty> >
			class xscope_structure_lock_guard : public mse::us::impl::Txscope_structure_lock_guard_of_wrapper<vector<_Ty, _A>, dummy_xscope_structure_lock_guard> {
			public:
				typedef mse::us::impl::Txscope_structure_lock_guard_of_wrapper<vector<_Ty, _A>, dummy_xscope_structure_lock_guard> base_class;

				xscope_structure_lock_guard(const xscope_structure_lock_guard&) = default;
				xscope_structure_lock_guard(xscope_structure_lock_guard&&) = default;

				xscope_structure_lock_guard(const mse::TXScopeObjFixedPointer<vector<_Ty, _A> >& owner_ptr)
					: base_class(owner_ptr, dummy_xscope_structure_lock_guard()) {}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
				xscope_structure_lock_guard(const mse::TXScopeFixedPointer<vector<_Ty, _A> >& owner_ptr)
					: base_class(owner_ptr, dummy_xscope_structure_lock_guard()) {}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
			};
			template<class _Ty, class _A = std::allocator<_Ty> >
			class xscope_const_structure_lock_guard : public mse::us::impl::Txscope_const_structure_lock_guard_of_wrapper<vector<_Ty, _A>, dummy_xscope_structure_lock_guard> {
			public:
				typedef mse::us::impl::Txscope_structure_lock_guard_of_wrapper<vector<_Ty, _A>, dummy_xscope_structure_lock_guard> base_class;

				xscope_const_structure_lock_guard(const xscope_const_structure_lock_guard&) = default;
				xscope_const_structure_lock_guard(xscope_const_structure_lock_guard&&) = default;

				xscope_const_structure_lock_guard(const mse::TXScopeObjFixedConstPointer<vector<_Ty, _A> >& owner_ptr)
					: base_class(owner_ptr, dummy_xscope_structure_lock_guard()) {}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
				xscope_const_structure_lock_guard(const mse::TXScopeFixedConstPointer<vector<_Ty, _A> >& owner_ptr)
					: base_class(owner_ptr, dummy_xscope_structure_lock_guard()) {}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
			};
		}

		template<class _Ty, class _A = std::allocator<_Ty> >
		auto make_xscope_structure_lock_guard(const mse::TXScopeObjFixedPointer<vector<_Ty, _A> >& owner_ptr) {
			return ns_vector::xscope_structure_lock_guard<_Ty, _A>(owner_ptr);
		}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _Ty, class _A = std::allocator<_Ty> >
		auto make_xscope_structure_lock_guard(const mse::TXScopeFixedPointer<vector<_Ty, _A> >& owner_ptr) {
			return ns_vector::xscope_structure_lock_guard<_Ty, _A>(owner_ptr);
		}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _Ty, class _A = std::allocator<_Ty> >
		auto make_xscope_structure_lock_guard(const mse::TXScopeObjFixedConstPointer<vector<_Ty, _A> >& owner_ptr) {
			return ns_vector::xscope_const_structure_lock_guard<_Ty, _A>(owner_ptr);
		}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _Ty, class _A = std::allocator<_Ty> >
		auto make_xscope_structure_lock_guard(const mse::TXScopeFixedConstPointer<vector<_Ty, _A> >& owner_ptr) {
			return ns_vector::xscope_const_structure_lock_guard<_Ty, _A>(owner_ptr);
		}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

		template<class _Ty, class _A = std::allocator<_Ty> >
		using Tvector_xscope_const_iterator = typename std::vector<_Ty, _A>::const_iterator;
		template<class _Ty, class _A = std::allocator<_Ty> >
		using Tvector_xscope_iterator = typename std::vector<_Ty, _A>::iterator;

#else /*MSE_MSTDVECTOR_DISABLED*/

#ifndef _NOEXCEPT
#define _NOEXCEPT
#endif /*_NOEXCEPT*/

		class mstdvector_range_error : public std::range_error {
		public:
			using std::range_error::range_error;
		};

		template<class _Ty, class _A>
		class vector;

		namespace ns_vector {
			/* For each (scope) vector instance, only one instance of xscope_structure_lock_guard may exist at any one
			time. While an instance of xscope_structure_lock_guard exists it ensures that direct (scope) pointers to
			individual elements in the vector do not become invalid by preventing any operation that might resize the vector
			or increase its capacity. Any attempt to execute such an operation would result in an exception. */
			template<class _Ty, class _A/* = std::allocator<_Ty> */> class xscope_structure_lock_guard;
			template<class _Ty, class _A/* = std::allocator<_Ty> */> class xscope_const_structure_lock_guard;
		}
		/* For each (scope) vector instance, only one instance of xscope_structure_lock_guard may exist at any one
		time. While an instance of xscope_structure_lock_guard exists it ensures that direct (scope) pointers to
		individual elements in the vector do not become invalid by preventing any operation that might resize the vector
		or increase its capacity. Any attempt to execute such an operation would result in an exception. */
		template<class _Ty, class _A/* = std::allocator<_Ty> */>
		auto make_xscope_structure_lock_guard(const mse::TXScopeObjFixedPointer<vector<_Ty, _A> >& owner_ptr) -> decltype(ns_vector::xscope_structure_lock_guard<_Ty, _A>(owner_ptr));
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _Ty, class _A/* = std::allocator<_Ty> */>
		auto make_xscope_structure_lock_guard(const mse::TXScopeFixedPointer<vector<_Ty, _A> >& owner_ptr) -> decltype(ns_vector::xscope_structure_lock_guard<_Ty, _A>(owner_ptr));
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _Ty, class _A/* = std::allocator<_Ty> */>
		auto make_xscope_structure_lock_guard(const mse::TXScopeObjFixedConstPointer<vector<_Ty, _A> >& owner_ptr) -> decltype(ns_vector::xscope_const_structure_lock_guard<_Ty, _A>(owner_ptr));
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _Ty, class _A/* = std::allocator<_Ty> */>
		auto make_xscope_structure_lock_guard(const mse::TXScopeFixedConstPointer<vector<_Ty, _A> >& owner_ptr) -> decltype(ns_vector::xscope_const_structure_lock_guard<_Ty, _A>(owner_ptr));
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
	}
	
	template<class _Ty, class _A/* = std::allocator<_Ty> */>
	auto make_xscope_structure_lock_guard(const mse::TXScopeObjFixedPointer<mstd::vector<_Ty, _A> >& owner_ptr) -> decltype(mstd::make_xscope_structure_lock_guard(owner_ptr)) {
		mstd::make_xscope_structure_lock_guard(owner_ptr);
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, class _A/* = std::allocator<_Ty> */>
	auto make_xscope_structure_lock_guard(const mse::TXScopeFixedPointer<mstd::vector<_Ty, _A> >& owner_ptr) -> decltype(mstd::make_xscope_structure_lock_guard(owner_ptr)) {
		mstd::make_xscope_structure_lock_guard(owner_ptr);
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, class _A/* = std::allocator<_Ty> */>
	auto make_xscope_structure_lock_guard(const mse::TXScopeObjFixedConstPointer<mstd::vector<_Ty, _A> >& owner_ptr) -> decltype(mstd::make_xscope_structure_lock_guard(owner_ptr)) {
		mstd::make_xscope_structure_lock_guard(owner_ptr);
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, class _A/* = std::allocator<_Ty> */>
	auto make_xscope_structure_lock_guard(const mse::TXScopeFixedConstPointer<mstd::vector<_Ty, _A> >& owner_ptr) -> decltype(mstd::make_xscope_structure_lock_guard(owner_ptr)) {
		mstd::make_xscope_structure_lock_guard(owner_ptr);
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	namespace mstd {

		/* Following are some template (iterator) classes that, organizationally, should be members of mstd::vector<>. (And they
		used to be.) However, being a member of mstd::vector<> makes them "dependent types", and dependent types do not participate
		in automatic template parameter type deduction. So we had to haul them here outside of mstd::vector<>. */

		template<class _Ty, class _A = std::allocator<_Ty> >
		using TXScopeVectorPointer = mse::TXScopeFixedPointer<vector<_Ty, _A> >;
		template<class _Ty, class _A = std::allocator<_Ty> >
		using TXScopeVectorConstPointer = mse::TXScopeFixedConstPointer<vector<_Ty, _A> >;

		template<class _Ty, class _A>
		class Tvector_xscope_iterator;

		template<class _Ty, class _A = std::allocator<_Ty> >
		class Tvector_xscope_const_iterator : public mse::TFriendlyAugmentedRAConstIterator<mse::us::impl::TXScopeCSLSStrongRAConstIterator<TXScopeVectorConstPointer<_Ty, _A>, mse::mstd::ns_vector::xscope_const_structure_lock_guard<_Ty, _A> > >
			/*, public mse::us::impl::XScopeContainsNonOwningScopeReferenceTagBase, public mse::us::impl::AsyncNotShareableAndNotPassableTagBase*/ {
		public:
			typedef mse::TFriendlyAugmentedRAConstIterator<mse::us::impl::TXScopeCSLSStrongRAConstIterator<TXScopeVectorConstPointer<_Ty, _A>, mse::mstd::ns_vector::xscope_const_structure_lock_guard<_Ty, _A> > > base_class;
			MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(Tvector_xscope_const_iterator, base_class);

			MSE_USING_ASSIGNMENT_OPERATOR(base_class);
			auto& operator=(Tvector_xscope_const_iterator&& _X) & { base_class::operator=(MSE_FWD(_X)); return (*this); }
			auto& operator=(const Tvector_xscope_const_iterator& _X) & { base_class::operator=(_X); return (*this); }

			Tvector_xscope_const_iterator& operator ++() & { base_class::operator ++(); return (*this); }
			Tvector_xscope_const_iterator operator++(int) { Tvector_xscope_const_iterator _Tmp = *this; base_class::operator++(); return (_Tmp); }
			Tvector_xscope_const_iterator& operator --() & { base_class::operator --(); return (*this); }
			Tvector_xscope_const_iterator operator--(int) { Tvector_xscope_const_iterator _Tmp = *this; base_class::operator--(); return (_Tmp); }

			Tvector_xscope_const_iterator& operator +=(difference_type n) & { base_class::operator +=(n); return (*this); }
			Tvector_xscope_const_iterator& operator -=(difference_type n) & { base_class::operator -=(n); return (*this); }
			Tvector_xscope_const_iterator operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
			Tvector_xscope_const_iterator operator-(difference_type n) const { return ((*this) + (-n)); }
			difference_type operator-(const Tvector_xscope_const_iterator& _Right_cref) const { return base_class::operator-(_Right_cref); }
			const_reference operator*() const { return base_class::operator*(); }

			Tvector_xscope_const_iterator operator=(Tvector_xscope_const_iterator&& _X) && { base_class::operator=(MSE_FWD(_X)); return std::move(*this); }
			Tvector_xscope_const_iterator operator=(const Tvector_xscope_const_iterator& _X) && { base_class::operator=(_X); return std::move(*this); }
			Tvector_xscope_const_iterator operator ++() && { base_class::operator ++(); return std::move(*this); }
			Tvector_xscope_const_iterator operator --() && { base_class::operator --(); return std::move(*this); }
			Tvector_xscope_const_iterator operator +=(difference_type n) && { base_class::operator +=(n); return std::move(*this); }
			Tvector_xscope_const_iterator operator -=(difference_type n) && { base_class::operator -=(n); return std::move(*this); }

			void set_to_const_item_pointer(const Tvector_xscope_const_iterator& _Right_cref) { base_class::set_to_item_pointer(_Right_cref); }

			MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(base_class);
			void xscope_cslsstrong_iterator_type_tag() const {}

		private:
			MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

			friend class /*_Myt*/vector<_Ty, _A>;
			template<class _Ty2, class _A2>
			friend class Tvector_xscope_iterator;
		};

		template<class _Ty, class _A = std::allocator<_Ty> >
		class Tvector_xscope_iterator : public mse::TFriendlyAugmentedRAIterator<mse::us::impl::TXScopeCSLSStrongRAIterator<TXScopeVectorPointer<_Ty, _A>, mse::mstd::ns_vector::xscope_structure_lock_guard<_Ty, _A>/*decltype(mse::make_xscope_structure_lock_guard(std::declval<TXScopeVectorPointer<_Ty, _A> >()))*/> >
			/*, public mse::us::impl::XScopeContainsNonOwningScopeReferenceTagBase, public mse::us::impl::AsyncNotShareableAndNotPassableTagBase*/ {
		public:
			typedef mse::TFriendlyAugmentedRAIterator<mse::us::impl::TXScopeCSLSStrongRAIterator<TXScopeVectorPointer<_Ty, _A>, mse::mstd::ns_vector::xscope_structure_lock_guard<_Ty, _A>/*decltype(mse::make_xscope_structure_lock_guard(std::declval<TXScopeVectorPointer<_Ty, _A> >()))*/> > base_class;
			MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(Tvector_xscope_iterator, base_class);

			MSE_USING_ASSIGNMENT_OPERATOR(base_class);
			auto& operator=(Tvector_xscope_iterator&& _X) & { base_class::operator=(MSE_FWD(_X)); return (*this); }
			auto& operator=(const Tvector_xscope_iterator& _X) & { base_class::operator=(_X); return (*this); }

			Tvector_xscope_iterator& operator ++() & { base_class::operator ++(); return (*this); }
			Tvector_xscope_iterator operator++(int) { Tvector_xscope_iterator _Tmp = *this; base_class::operator++(); return (_Tmp); }
			Tvector_xscope_iterator& operator --() & { base_class::operator --(); return (*this); }
			Tvector_xscope_iterator operator--(int) { Tvector_xscope_iterator _Tmp = *this; base_class::operator--(); return (_Tmp); }

			Tvector_xscope_iterator& operator +=(difference_type n) & { base_class::operator +=(n); return (*this); }
			Tvector_xscope_iterator& operator -=(difference_type n) & { base_class::operator -=(n); return (*this); }
			Tvector_xscope_iterator operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
			Tvector_xscope_iterator operator-(difference_type n) const { return ((*this) + (-n)); }
			difference_type operator-(const Tvector_xscope_iterator& _Right_cref) const { return base_class::operator-(_Right_cref); }

			Tvector_xscope_iterator operator=(Tvector_xscope_iterator&& _X) && { base_class::operator=(MSE_FWD(_X)); return std::move(*this); }
			Tvector_xscope_iterator operator=(const Tvector_xscope_iterator _X) && { base_class::operator=(_X); return std::move(*this); }
			Tvector_xscope_iterator operator ++() && { base_class::operator ++(); return std::move(*this); }
			Tvector_xscope_iterator operator --() && { base_class::operator --(); return std::move(*this); }
			Tvector_xscope_iterator operator +=(difference_type n) && { base_class::operator +=(n); return std::move(*this); }
			Tvector_xscope_iterator operator -=(difference_type n) && { base_class::operator -=(n); return std::move(*this); }

			void set_to_item_pointer(const Tvector_xscope_iterator& _Right_cref) { base_class::set_to_item_pointer(_Right_cref); }

			MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(base_class);
			void xscope_cslsstrong_iterator_type_tag() const {}

		private:
			MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

			friend class /*_Myt*/vector<_Ty, _A>;
		};

		template<class _Ty, class _A = std::allocator<_Ty> >
		class vector : public mse::us::impl::AsyncNotShareableTagBase, public us::impl::ContiguousSequenceContainerTagBase, public us::impl::LockableStructureContainerTagBase {
		public:
			typedef vector _Myt;
			typedef mse::us::msevector<_Ty, _A> _MV;

			MSE_INHERITED_RANDOM_ACCESS_MEMBER_TYPE_DECLARATIONS(_MV);

			operator mse::nii_vector<_Ty, _A>() const & { return msevector(); }
			operator mse::nii_vector<_Ty, _A>() && { return std::forward<_MV>(msevector()); }
			operator std::vector<_Ty, _A>() const & { return msevector(); }
			operator std::vector<_Ty, _A>() && { return std::forward<std::vector<_Ty, _A> >(msevector()); }

			explicit vector(const _A& _Al = _A()) : m_shptr(std::make_shared<_MV>(_Al)) {}
			explicit vector(size_type _N) : m_shptr(std::make_shared<_MV>(_N)) {}
			explicit vector(size_type _N, const _Ty& _V, const _A& _Al = _A()) : m_shptr(std::make_shared<_MV>(_N, _V, _Al)) {}
			vector(_Myt&& _X) : m_shptr(std::make_shared<_MV>()) {
				static_assert(typename std::is_rvalue_reference<decltype(_X)>::type(), "");
				/* It would be more efficient to just move _X.m_shptr into m_shptr, but that would leave _X in what we
				would consider an invalid state. */
				msevector() = std::move(_X.msevector());
			}
			vector(const _Myt& _X) : m_shptr(std::make_shared<_MV>(_X.msevector())) {}
			vector(_MV&& _X) : m_shptr(std::make_shared<_MV>(MSE_FWD(_X))) {}
			vector(const _MV& _X) : m_shptr(std::make_shared<_MV>(_X)) {}
			vector(std::vector<_Ty>&& _X) : m_shptr(std::make_shared<_MV>(MSE_FWD(_X))) {}
			vector(const std::vector<_Ty>& _X) : m_shptr(std::make_shared<_MV>(_X)) {}
			typedef typename _MV::const_iterator _It;
			vector(_It _F, _It _L, const _A& _Al = _A()) : m_shptr(std::make_shared<_MV>(_F, _L, _Al)) {}
			vector(const _Ty* _F, const _Ty* _L, const _A& _Al = _A()) : m_shptr(std::make_shared<_MV>(_F, _L, _Al)) {}
			template<class _Iter, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value> MSE_IMPL_EIS >
			vector(_Iter _First, _Iter _Last) : m_shptr(std::make_shared<_MV>(_First, _Last)) {}
			template<class _Iter, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value> MSE_IMPL_EIS >
			vector(_Iter _First, _Iter _Last, const _A& _Al) : m_shptr(std::make_shared<_MV>(_First, _Last, _Al)) {}

			MSE_IMPL_DESTRUCTOR_PREFIX1 ~vector() {
				msevector().note_parent_destruction();
			}

			//_Myt& operator=(_MV&& _X) { msevector() = (MSE_FWD(_X)); return (*this); }
			//_Myt& operator=(const _MV& _X) { msevector() = (_X); return (*this); }
			_Myt& operator=(_Myt&& _X) {
				static_assert(typename std::is_rvalue_reference<decltype(_X)>::type(), "");
				/* It would be more efficient to just move _X.m_shptr into m_shptr, but that would leave _X in what we
				would consider an invalid state. */
				msevector() = std::move(_X.msevector());
				return (*this);
			}
			_Myt& operator=(const _Myt& _X) { msevector() = (_X.msevector()); return (*this); }
			void reserve(size_type _Count) { m_shptr->reserve(_Count); }
			void resize(size_type _N, const _Ty& _X = _Ty()) { m_shptr->resize(_N, _X); }
			typename _MV::const_reference operator[](size_type _P) const { return m_shptr->operator[](_P); }
			typename _MV::reference operator[](size_type _P) { return m_shptr->operator[](_P); }
			void push_back(_Ty&& _X) { m_shptr->push_back(MSE_FWD(_X)); }
			void push_back(const _Ty& _X) { m_shptr->push_back(_X); }
			void pop_back() { m_shptr->pop_back(); }
			void assign(_It _F, _It _L) { m_shptr->assign(_F, _L); }
			void assign(size_type _N, const _Ty& _X = _Ty()) { m_shptr->assign(_N, _X); }
			template<class ..._Valty>
			void emplace_back(_Valty&& ..._Val) { m_shptr->emplace_back(std::forward<_Valty>(_Val)...); }
			void clear() { m_shptr->clear(); }
			void swap(_MV& _X) { m_shptr->swap(_X); }
			void swap(_Myt& _X) { m_shptr->swap(_X.msevector()); }
			void swap(std::vector<_Ty, _A>& _X) { m_shptr->swap(_X); }
			template<typename _TStateMutex2, template<typename> class _TTXScopeConstIterator, class TConstLockableIndicator2>
			void swap(mse::us::impl::gnii_vector<_Ty, _A, _TStateMutex2, _TTXScopeConstIterator, TConstLockableIndicator2>& _X) { m_shptr->swap(_X); }

			vector(_XSTD initializer_list<typename _MV::value_type> _Ilist, const _A& _Al = _A()) : m_shptr(std::make_shared<_MV>(_Ilist, _Al)) {}
			_Myt& operator=(_XSTD initializer_list<typename _MV::value_type> _Ilist) { msevector() = (_Ilist); return (*this); }
			void assign(_XSTD initializer_list<typename _MV::value_type> _Ilist) { m_shptr->assign(_Ilist); }

			size_type capacity() const _NOEXCEPT { return m_shptr->capacity(); }
			void shrink_to_fit() { m_shptr->shrink_to_fit(); }
			size_type size() const _NOEXCEPT { return m_shptr->size(); }
			size_type max_size() const _NOEXCEPT { return m_shptr->max_size(); }
			bool empty() const _NOEXCEPT { return m_shptr->empty(); }
			_A get_allocator() const _NOEXCEPT { return m_shptr->get_allocator(); }
			typename _MV::const_reference at(size_type _Pos) const { return m_shptr->at(_Pos); }
			typename _MV::reference at(size_type _Pos) { return m_shptr->at(_Pos); }
			typename _MV::reference front() { return m_shptr->front(); }
			typename _MV::const_reference front() const { return m_shptr->front(); }
			typename _MV::reference back() { return m_shptr->back(); }
			typename _MV::const_reference back() const { return m_shptr->back(); }

			/* Try to avoid using these whenever possible. */
			value_type *data() _NOEXCEPT {
				return m_shptr->data();
			}
			const value_type *data() const _NOEXCEPT {
				return m_shptr->data();
			}

			typedef Tvector_xscope_const_iterator<_Ty, _A> xscope_const_iterator;
			typedef Tvector_xscope_iterator<_Ty, _A> xscope_iterator;

			class const_iterator : public _MV::random_access_const_iterator_base {
			public:
				typedef typename _MV::random_access_const_iterator_base base_class;
				MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

				const_iterator() {}
				const_iterator(const const_iterator& src_cref) : m_msevector_cshptr(src_cref.m_msevector_cshptr) {
					(*this) = src_cref;
				}
				~const_iterator() {}
				const typename _MV::ss_const_iterator_type& msevector_ss_const_iterator_type() const { return m_ss_const_iterator; }
				typename _MV::ss_const_iterator_type& msevector_ss_const_iterator_type() { return m_ss_const_iterator; }
				const typename _MV::ss_const_iterator_type& mvssci() const { return msevector_ss_const_iterator_type(); }
				typename _MV::ss_const_iterator_type& mvssci() { return msevector_ss_const_iterator_type(); }

				void reset() { msevector_ss_const_iterator_type().reset(); }
				bool points_to_an_item() const { return msevector_ss_const_iterator_type().points_to_an_item(); }
				bool points_to_end_marker() const { return msevector_ss_const_iterator_type().points_to_end_marker(); }
				bool points_to_beginning() const { return msevector_ss_const_iterator_type().points_to_beginning(); }
				/* has_next_item_or_end_marker() is just an alias for points_to_an_item(). */
				bool has_next_item_or_end_marker() const { return msevector_ss_const_iterator_type().has_next_item_or_end_marker(); }
				/* has_next() is just an alias for points_to_an_item() that's familiar to java programmers. */
				bool has_next() const { return msevector_ss_const_iterator_type().has_next(); }
				bool has_previous() const { return msevector_ss_const_iterator_type().has_previous(); }
				void set_to_beginning() { msevector_ss_const_iterator_type().set_to_beginning(); }
				void set_to_end_marker() { msevector_ss_const_iterator_type().set_to_end_marker(); }
				void set_to_next() { msevector_ss_const_iterator_type().set_to_next(); }
				void set_to_previous() { msevector_ss_const_iterator_type().set_to_previous(); }
				const_iterator& operator ++() { msevector_ss_const_iterator_type().operator ++(); return (*this); }
				const_iterator operator++(int) { const_iterator _Tmp = *this; ++*this; return (_Tmp); }
				const_iterator& operator --() { msevector_ss_const_iterator_type().operator --(); return (*this); }
				const_iterator operator--(int) { const_iterator _Tmp = *this; --*this; return (_Tmp); }
				void advance(typename _MV::difference_type n) { msevector_ss_const_iterator_type().advance(n); }
				void regress(typename _MV::difference_type n) { msevector_ss_const_iterator_type().regress(n); }
				const_iterator& operator +=(difference_type n) { msevector_ss_const_iterator_type().operator +=(n); return (*this); }
				const_iterator& operator -=(difference_type n) { msevector_ss_const_iterator_type().operator -=(n); return (*this); }
				const_iterator operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
				const_iterator operator-(difference_type n) const { return ((*this) + (-n)); }
				typename _MV::difference_type operator-(const const_iterator& _Right_cref) const { return msevector_ss_const_iterator_type() - (_Right_cref.msevector_ss_const_iterator_type()); }
				typename _MV::const_reference operator*() const {
					(*m_msevector_cshptr).assert_parent_not_destroyed();
					return msevector_ss_const_iterator_type().operator*();
				}
				typename _MV::const_reference item() const { return operator*(); }
				typename _MV::const_reference previous_item() const { return msevector_ss_const_iterator_type().previous_item(); }
				typename _MV::const_pointer operator->() const {
					(*m_msevector_cshptr).assert_parent_not_destroyed();
					return msevector_ss_const_iterator_type().operator->();
				}
				typename _MV::const_reference operator[](typename _MV::difference_type _Off) const { return (*(*this + _Off)); }
				bool operator==(const const_iterator& _Right_cref) const { return msevector_ss_const_iterator_type().operator==(_Right_cref.msevector_ss_const_iterator_type()); }
				bool operator!=(const const_iterator& _Right_cref) const { return (!(_Right_cref == (*this))); }
				bool operator<(const const_iterator& _Right) const { return (msevector_ss_const_iterator_type() < _Right.msevector_ss_const_iterator_type()); }
				bool operator<=(const const_iterator& _Right) const { return (msevector_ss_const_iterator_type() <= _Right.msevector_ss_const_iterator_type()); }
				bool operator>(const const_iterator& _Right) const { return (msevector_ss_const_iterator_type() > _Right.msevector_ss_const_iterator_type()); }
				bool operator>=(const const_iterator& _Right) const { return (msevector_ss_const_iterator_type() >= _Right.msevector_ss_const_iterator_type()); }
				void set_to_const_item_pointer(const const_iterator& _Right_cref) { msevector_ss_const_iterator_type().set_to_const_item_pointer(_Right_cref.msevector_ss_const_iterator_type()); }
				msev_size_t position() const { return msevector_ss_const_iterator_type().position(); }
				auto target_container_ptr() const -> decltype(msevector_ss_const_iterator_type().target_container_ptr()) {
					return msevector_ss_const_iterator_type().target_container_ptr();
				}
				void async_not_shareable_and_not_passable_tag() const {}
			private:
				const_iterator(std::shared_ptr<_MV> msevector_shptr) : m_msevector_cshptr(msevector_shptr) {
					m_ss_const_iterator = msevector_shptr->ss_cbegin();
				}
				std::shared_ptr<const _MV> m_msevector_cshptr;
				/* m_ss_const_iterator needs to be declared after m_msevector_cshptr so that its destructor will be called first. */
				typename _MV::ss_const_iterator_type m_ss_const_iterator;

				friend class /*_Myt*/vector<_Ty, _A>;
				friend class iterator;
				//friend /*class*/ xscope_const_iterator;
			};
			class iterator : public _MV::random_access_iterator_base {
			public:
				typedef typename _MV::random_access_iterator_base base_class;
				MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

				iterator() {}
				iterator(const iterator& src_cref) : m_msevector_shptr(src_cref.m_msevector_shptr) {
					(*this) = src_cref;
				}
				~iterator() {}
				const typename _MV::ss_iterator_type& msevector_ss_iterator_type() const { return m_ss_iterator; }
				typename _MV::ss_iterator_type& msevector_ss_iterator_type() { return m_ss_iterator; }
				const typename _MV::ss_iterator_type& mvssi() const { return msevector_ss_iterator_type(); }
				typename _MV::ss_iterator_type& mvssi() { return msevector_ss_iterator_type(); }
				operator const_iterator() const {
					const_iterator retval(m_msevector_shptr);
					if (m_msevector_shptr) {
						retval.msevector_ss_const_iterator_type().set_to_beginning();
						retval.msevector_ss_const_iterator_type().advance(msev_int(msevector_ss_iterator_type().position()));
					}
					return retval;
				}

				void reset() { msevector_ss_iterator_type().reset(); }
				bool points_to_an_item() const { return msevector_ss_iterator_type().points_to_an_item(); }
				bool points_to_end_marker() const { return msevector_ss_iterator_type().points_to_end_marker(); }
				bool points_to_beginning() const { return msevector_ss_iterator_type().points_to_beginning(); }
				/* has_next_item_or_end_marker() is just an alias for points_to_an_item(). */
				bool has_next_item_or_end_marker() const { return msevector_ss_iterator_type().has_next_item_or_end_marker(); }
				/* has_next() is just an alias for points_to_an_item() that's familiar to java programmers. */
				bool has_next() const { return msevector_ss_iterator_type().has_next(); }
				bool has_previous() const { return msevector_ss_iterator_type().has_previous(); }
				void set_to_beginning() { msevector_ss_iterator_type().set_to_beginning(); }
				void set_to_end_marker() { msevector_ss_iterator_type().set_to_end_marker(); }
				void set_to_next() { msevector_ss_iterator_type().set_to_next(); }
				void set_to_previous() { msevector_ss_iterator_type().set_to_previous(); }
				iterator& operator ++() { msevector_ss_iterator_type().operator ++(); return (*this); }
				iterator operator++(int) { iterator _Tmp = *this; ++*this; return (_Tmp); }
				iterator& operator --() { msevector_ss_iterator_type().operator --(); return (*this); }
				iterator operator--(int) { iterator _Tmp = *this; --*this; return (_Tmp); }
				void advance(typename _MV::difference_type n) { msevector_ss_iterator_type().advance(n); }
				void regress(typename _MV::difference_type n) { msevector_ss_iterator_type().regress(n); }
				iterator& operator +=(difference_type n) { msevector_ss_iterator_type().operator +=(n); return (*this); }
				iterator& operator -=(difference_type n) { msevector_ss_iterator_type().operator -=(n); return (*this); }
				iterator operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
				iterator operator-(difference_type n) const { return ((*this) + (-n)); }
				typename _MV::difference_type operator-(const iterator& _Right_cref) const { return msevector_ss_iterator_type() - (_Right_cref.msevector_ss_iterator_type()); }
				typename _MV::reference operator*() const {
					(*m_msevector_shptr).assert_parent_not_destroyed();
					return msevector_ss_iterator_type().operator*();
				}
				typename _MV::reference item() const { return operator*(); }
				typename _MV::reference previous_item() const { return msevector_ss_iterator_type().previous_item(); }
				typename _MV::pointer operator->() const {
					(*m_msevector_shptr).assert_parent_not_destroyed();
					return msevector_ss_iterator_type().operator->();
				}
				typename _MV::reference operator[](typename _MV::difference_type _Off) const { return (*(*this + _Off)); }
				bool operator==(const iterator& _Right_cref) const { return msevector_ss_iterator_type().operator==(_Right_cref.msevector_ss_iterator_type()); }
				bool operator!=(const iterator& _Right_cref) const { return (!(_Right_cref == (*this))); }
				bool operator<(const iterator& _Right) const { return (msevector_ss_iterator_type() < _Right.msevector_ss_iterator_type()); }
				bool operator<=(const iterator& _Right) const { return (msevector_ss_iterator_type() <= _Right.msevector_ss_iterator_type()); }
				bool operator>(const iterator& _Right) const { return (msevector_ss_iterator_type() > _Right.msevector_ss_iterator_type()); }
				bool operator>=(const iterator& _Right) const { return (msevector_ss_iterator_type() >= _Right.msevector_ss_iterator_type()); }
				void set_to_item_pointer(const iterator& _Right_cref) { msevector_ss_iterator_type().set_to_item_pointer(_Right_cref.msevector_ss_iterator_type()); }
				msev_size_t position() const { return msevector_ss_iterator_type().position(); }
				auto target_container_ptr() const -> decltype(msevector_ss_iterator_type().target_container_ptr()) {
					return msevector_ss_iterator_type().target_container_ptr();
				}
				void async_not_shareable_and_not_passable_tag() const {}
			private:
				std::shared_ptr<_MV> m_msevector_shptr;
				/* m_ss_iterator needs to be declared after m_msevector_shptr so that its destructor will be called first. */
				typename _MV::ss_iterator_type m_ss_iterator;

				friend class /*_Myt*/vector<_Ty, _A>;
				//friend /*class*/ xscope_const_iterator;
				//friend /*class*/ xscope_iterator;
			};

			iterator begin() {	// return iterator for beginning of mutable sequence
				iterator retval; retval.m_msevector_shptr = this->m_shptr;
				(retval.m_ss_iterator) = m_shptr->ss_begin();
				return retval;
			}
			const_iterator begin() const {	// return iterator for beginning of nonmutable sequence
				const_iterator retval; retval.m_msevector_cshptr = this->m_shptr;
				(retval.m_ss_const_iterator) = m_shptr->ss_begin();
				return retval;
			}
			iterator end() {	// return iterator for end of mutable sequence
				iterator retval; retval.m_msevector_shptr = this->m_shptr;
				(retval.m_ss_iterator) = m_shptr->ss_end();
				return retval;
			}
			const_iterator end() const {	// return iterator for end of nonmutable sequence
				const_iterator retval; retval.m_msevector_cshptr = this->m_shptr;
				(retval.m_ss_const_iterator) = m_shptr->ss_end();
				return retval;
			}
			const_iterator cbegin() const {	// return iterator for beginning of nonmutable sequence
				const_iterator retval; retval.m_msevector_cshptr = this->m_shptr;
				(retval.m_ss_const_iterator) = m_shptr->ss_cbegin();
				return retval;
			}
			const_iterator cend() const {	// return iterator for end of nonmutable sequence
				const_iterator retval; retval.m_msevector_cshptr = this->m_shptr;
				(retval.m_ss_const_iterator) = m_shptr->ss_cend();
				return retval;
			}

			typedef std::reverse_iterator<iterator> reverse_iterator;
			typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

			reverse_iterator rbegin() {	// return iterator for beginning of reversed mutable sequence
				return (reverse_iterator(end()));
			}
			const_reverse_iterator rbegin() const {	// return iterator for beginning of reversed nonmutable sequence
				return (const_reverse_iterator(end()));
			}
			reverse_iterator rend() {	// return iterator for end of reversed mutable sequence
				return (reverse_iterator(begin()));
			}
			const_reverse_iterator rend() const {	// return iterator for end of reversed nonmutable sequence
				return (const_reverse_iterator(begin()));
			}
			const_reverse_iterator crbegin() const {	// return iterator for beginning of reversed nonmutable sequence
				return (rbegin());
			}
			const_reverse_iterator crend() const {	// return iterator for end of reversed nonmutable sequence
				return (rend());
			}


			vector(const const_iterator &start, const const_iterator &end, const _A& _Al = _A())
				: m_shptr(std::make_shared<_MV>(start.msevector_ss_const_iterator_type(), end.msevector_ss_const_iterator_type(), _Al)) {}
			void assign(const const_iterator &start, const const_iterator &end) {
				m_shptr->assign(start.msevector_ss_const_iterator_type(), end.msevector_ss_const_iterator_type());
			}
			void assign_inclusive(const const_iterator &first, const const_iterator &last) {
				m_shptr->assign_inclusive(first.msevector_ss_const_iterator_type(), last.msevector_ss_const_iterator_type());
			}
			iterator insert_before(const const_iterator &pos, size_type _M, const _Ty& _X) {
				auto res = m_shptr->insert_before(pos.msevector_ss_const_iterator_type(), _M, _X);
				iterator retval = begin(); retval.msevector_ss_iterator_type() = res;
				return retval;
			}
			iterator insert_before(const const_iterator &pos, _Ty&& _X) {
				auto res = m_shptr->insert_before(pos.msevector_ss_const_iterator_type(), MSE_FWD(_X));
				iterator retval = begin(); retval.msevector_ss_iterator_type() = res;
				return retval;
			}
			iterator insert_before(const const_iterator &pos, const _Ty& _X = _Ty()) { return insert_before(pos, 1, _X); }
			template<class _Iter
				//>mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value, typename base_class::iterator>
				, class = mse::impl::_mse_RequireInputIter<_Iter> >
				iterator insert_before(const const_iterator &pos, const _Iter &start, const _Iter &end) {
				auto res = m_shptr->insert_before(pos.msevector_ss_const_iterator_type(), start, end);
				iterator retval = begin(); retval.msevector_ss_iterator_type() = res;
				return retval;
			}
			template<class _Iter
				//>mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value, typename base_class::iterator>
				, class = mse::impl::_mse_RequireInputIter<_Iter> >
				iterator insert_before_inclusive(const const_iterator &pos, const _Iter &first, const _Iter &last) {
				auto end = last; end++;
				return insert_before(pos, first, end);
			}
			iterator insert_before(const const_iterator &pos, _XSTD initializer_list<typename _MV::value_type> _Ilist) {	// insert initializer_list
				auto res = m_shptr->insert_before(pos.msevector_ss_const_iterator_type(), _Ilist);
				iterator retval = begin(); retval.msevector_ss_iterator_type() = res;
				return retval;
			}
			void insert_before(msev_size_t pos, const _Ty& _X = _Ty()) {
				m_shptr->insert_before(pos, _X);
			}
			void insert_before(msev_size_t pos, size_type _M, const _Ty& _X) {
				m_shptr->insert_before(pos, _M, _X);
			}
			void insert_before(msev_size_t pos, _XSTD initializer_list<typename _MV::value_type> _Ilist) {	// insert initializer_list
				m_shptr->insert_before(pos, _Ilist);
			}
			/* These insert() functions are just aliases for their corresponding insert_before() functions. */
			iterator insert(const const_iterator &pos, size_type _M, const _Ty& _X) { return insert_before(pos, _M, _X); }
			iterator insert(const const_iterator &pos, _Ty&& _X) { return insert_before(pos, MSE_FWD(_X)); }
			iterator insert(const const_iterator &pos, const _Ty& _X = _Ty()) { return insert_before(pos, _X); }
			template<class _Iter
				//>mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value, typename base_class::iterator>
				, class = mse::impl::_mse_RequireInputIter<_Iter> >
				iterator insert(const const_iterator &pos, const _Iter &start, const _Iter &end) { return insert_before(pos, start, end); }
			iterator insert(const const_iterator &pos, const _Ty* start, const _Ty* end) { return insert_before(pos, start, end); }
			iterator insert(const const_iterator &pos, _XSTD initializer_list<typename _MV::value_type> _Ilist) { return insert_before(pos, _Ilist); }
			template<class ..._Valty>
			iterator emplace(const const_iterator &pos, _Valty&& ..._Val) {
				auto res = m_shptr->emplace(pos.msevector_ss_const_iterator_type(), std::forward<_Valty>(_Val)...);
				iterator retval = begin(); retval.msevector_ss_iterator_type() = res;
				return retval;
			}
			iterator erase(const const_iterator &pos) {
				auto res = m_shptr->erase(pos.msevector_ss_const_iterator_type());
				iterator retval = begin(); retval.msevector_ss_iterator_type() = res;
				return retval;
			}
			iterator erase(const const_iterator &start, const const_iterator &end) {
				auto res = m_shptr->erase(start.msevector_ss_const_iterator_type(), end.msevector_ss_const_iterator_type());
				iterator retval = begin(); retval.msevector_ss_iterator_type() = res;
				return retval;
			}
			iterator erase_inclusive(const const_iterator &first, const const_iterator &last) {
				auto end = last; end.set_to_next();
				return erase_inclusive(first, end);
			}
			bool operator==(const _Myt& _Right) const {	// test for vector equality
				return ((*(_Right.m_shptr)) == (*m_shptr));
			}
			bool operator<(const _Myt& _Right) const {	// test if _Left < _Right for vectors
				return ((*m_shptr) < (*(_Right.m_shptr)));
			}

			void async_not_shareable_tag() const {}
			/* this array should be safely passable iff the element type is safely passable */
			template<class _Ty2 = _Ty, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _Ty>::value)
				&& (mse::impl::is_marked_as_passable_msemsearray<_Ty2>::value)> MSE_IMPL_EIS >
			void async_passable_tag() const {}

		private:
			const _MV& msevector() const& { return (*m_shptr); }
			//const _MV& msevector() const&& { return (*m_shptr); }
			_MV& msevector()& { return (*m_shptr); }
			_MV&& msevector()&& { return std::move(*m_shptr); }

			std::shared_ptr<_MV> m_shptr;

			friend xscope_const_iterator;
			friend xscope_iterator;

			friend class mse::mstd::ns_vector::xscope_structure_lock_guard<_Ty, _A>;
			friend class mse::mstd::ns_vector::xscope_const_structure_lock_guard<_Ty, _A>;
		};

#ifdef MSE_HAS_CXX17
		/* deduction guides */
		template<class _Iter, class _Alloc = std::allocator<typename std::iterator_traits<_Iter>::value_type> >
		vector(_Iter, _Iter, _Alloc = _Alloc())
			->vector<typename std::iterator_traits<_Iter>::value_type, _Alloc>;
#endif /* MSE_HAS_CXX17 */

		template<class _Ty, class _Alloc> inline bool operator!=(const vector<_Ty, _Alloc>& _Left, const vector<_Ty, _Alloc>& _Right) {	// test for vector inequality
			return (!(_Left == _Right));
		}
		template<class _Ty, class _Alloc> inline bool operator>(const vector<_Ty, _Alloc>& _Left, const vector<_Ty, _Alloc>& _Right) {	// test if _Left > _Right for vectors
			return (_Right < _Left);
		}
		template<class _Ty, class _Alloc> inline bool operator<=(const vector<_Ty, _Alloc>& _Left, const vector<_Ty, _Alloc>& _Right) {	// test if _Left <= _Right for vectors
			return (!(_Right < _Left));
		}
		template<class _Ty, class _Alloc> inline bool operator>=(const vector<_Ty, _Alloc>& _Left, const vector<_Ty, _Alloc>& _Right) {	// test if _Left >= _Right for vectors
			return (!(_Left < _Right));
		}

		namespace ns_vector {

			/* For each (scope) vector instance, only one instance of xscope_structure_lock_guard may exist at any one
			time. While an instance of xscope_structure_lock_guard exists it ensures that direct (scope) pointers to
			individual elements in the vector do not become invalid by preventing any operation that might resize the vector
			or increase its capacity. Any attempt to execute such an operation would result in an exception. */
			template<class _Ty, class _A = std::allocator<_Ty> >
			class xscope_structure_lock_guard : public mse::us::impl::Txscope_structure_lock_guard_of_wrapper<vector<_Ty, _A>, typename mse::us::ns_msevector::xscope_structure_lock_guard<mse::us::msevector<_Ty, _A> > > {
			public:
				typedef mse::us::impl::Txscope_structure_lock_guard_of_wrapper<vector<_Ty, _A>, typename mse::us::ns_msevector::xscope_structure_lock_guard<mse::us::msevector<_Ty, _A> > > base_class;
				using base_class::base_class;

				xscope_structure_lock_guard(const mse::TXScopeObjFixedPointer<vector<_Ty, _A> >& owner_ptr)
					: base_class(owner_ptr, mse::us::unsafe_make_xscope_pointer_to(*((*owner_ptr).m_shptr))) {}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
				xscope_structure_lock_guard(const mse::TXScopeFixedPointer<vector<_Ty, _A> >& owner_ptr)
					: base_class(owner_ptr, mse::us::unsafe_make_xscope_pointer_to(*((*owner_ptr).m_shptr))) {}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

			private:
				MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

				friend class xscope_const_structure_lock_guard<_Ty, _A>;
			};
			template<class _Ty, class _A = std::allocator<_Ty> >
			class xscope_const_structure_lock_guard : public mse::us::impl::Txscope_const_structure_lock_guard_of_wrapper<vector<_Ty, _A>, typename mse::us::ns_msevector::xscope_const_structure_lock_guard<mse::us::msevector<_Ty, _A> > > {
			public:
				typedef mse::us::impl::Txscope_const_structure_lock_guard_of_wrapper<vector<_Ty, _A>, typename mse::us::ns_msevector::xscope_const_structure_lock_guard<mse::us::msevector<_Ty, _A> > > base_class;
				using base_class::base_class;

				xscope_const_structure_lock_guard(const mse::TXScopeObjFixedConstPointer<vector<_Ty, _A> >& owner_ptr)
					: base_class(owner_ptr, mse::us::unsafe_make_xscope_const_pointer_to(*((*owner_ptr).m_shptr))) {}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
				xscope_const_structure_lock_guard(const mse::TXScopeFixedConstPointer<vector<_Ty, _A> >& owner_ptr)
					: base_class(owner_ptr, mse::us::unsafe_make_xscope_const_pointer_to(*((*owner_ptr).m_shptr))) {}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

			private:
				MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
			};

		}

		/* For each (scope) vector instance, only one instance of xscope_structure_lock_guard may exist at any one
		time. While an instance of xscope_structure_lock_guard exists it ensures that direct (scope) pointers to
		individual elements in the vector do not become invalid by preventing any operation that might resize the vector
		or increase its capacity. Any attempt to execute such an operation would result in an exception. */
		template<class _Ty, class _A = std::allocator<_Ty> >
		auto make_xscope_structure_lock_guard(const mse::TXScopeObjFixedPointer<vector<_Ty, _A> >& owner_ptr) -> decltype(ns_vector::xscope_structure_lock_guard<_Ty, _A>(owner_ptr)) {
			return ns_vector::xscope_structure_lock_guard<_Ty, _A>(owner_ptr);
		}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _Ty, class _A = std::allocator<_Ty> >
		auto make_xscope_structure_lock_guard(const mse::TXScopeFixedPointer<vector<_Ty, _A> >& owner_ptr) -> decltype(ns_vector::xscope_structure_lock_guard<_Ty, _A>(owner_ptr)) {
			return ns_vector::xscope_structure_lock_guard<_Ty, _A>(owner_ptr);
		}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _Ty, class _A = std::allocator<_Ty> >
		auto make_xscope_structure_lock_guard(const mse::TXScopeObjFixedConstPointer<vector<_Ty, _A> >& owner_ptr) -> decltype(ns_vector::xscope_const_structure_lock_guard<_Ty, _A>(owner_ptr)) {
			return ns_vector::xscope_const_structure_lock_guard<_Ty, _A>(owner_ptr);
		}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _Ty, class _A = std::allocator<_Ty> >
		auto make_xscope_structure_lock_guard(const mse::TXScopeFixedConstPointer<vector<_Ty, _A> >& owner_ptr) -> decltype(ns_vector::xscope_const_structure_lock_guard<_Ty, _A>(owner_ptr)) {
			return ns_vector::xscope_const_structure_lock_guard<_Ty, _A>(owner_ptr);
		}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
	}
}

namespace std {

	template<class _Ty, class _A >
	void swap(mse::mstd::vector<_Ty, _A>& _Left, mse::mstd::vector<_Ty, _A>& _Right) _NOEXCEPT_OP(_NOEXCEPT_OP(_Left.swap(_Right)))
	{	// swap vectors
		return (_Left.swap(_Right));
	}
	template<class _Ty, class _A, class _TStateMutex, template<typename> class _TTXScopeConstIterator, class TConstLockableIndicator/*, class = enable_if_t<_Is_swappable<_Ty>::value>*/>
	void swap(mse::mstd::vector<_Ty, _A>& _Left, mse::us::impl::gnii_vector<_Ty, _A, _TStateMutex, _TTXScopeConstIterator, TConstLockableIndicator>& _Right) _NOEXCEPT_OP(_NOEXCEPT_OP(_Left.swap(_Right)))
	{	// swap vectors
		return (_Left.swap(_Right));
	}
	template<class _Ty, class _A, class _TStateMutex/*, class = enable_if_t<_Is_swappable<_Ty>::value>*/>
	void swap(mse::mstd::vector<_Ty, _A>& _Left, mse::us::msevector<_Ty, _A, _TStateMutex>& _Right) _NOEXCEPT_OP(_NOEXCEPT_OP(_Left.swap(_Right)))
	{	// swap vectors
		return (_Left.swap(_Right));
	}
	template<class _Ty, class _A/*, class = enable_if_t<_Is_swappable<_Ty>::value>*/>
	void swap(mse::mstd::vector<_Ty, _A>& _Left, std::vector<_Ty, _A>& _Right) _NOEXCEPT_OP(_NOEXCEPT_OP(_Left.swap(_Right)))
	{	// swap vectors
		return (_Left.swap(_Right));
	}

	template<class _Ty, class _A, class _TStateMutex, template<typename> class _TTXScopeConstIterator, class TConstLockableIndicator/*, class = enable_if_t<_Is_swappable<_Ty>::value>*/>
	void swap(mse::us::impl::gnii_vector<_Ty, _A, _TStateMutex, _TTXScopeConstIterator, TConstLockableIndicator>& _Left, mse::mstd::vector<_Ty, _A>& _Right) _NOEXCEPT_OP(_NOEXCEPT_OP(_Right.swap(_Left)))
	{	// swap vectors
		return (_Right.swap(_Left));
	}
	template<class _Ty, class _A, class _TStateMutex/*, class = enable_if_t<_Is_swappable<_Ty>::value>*/>
	void swap(mse::us::msevector<_Ty, _A, _TStateMutex>& _Left, mse::mstd::vector<_Ty, _A>& _Right) _NOEXCEPT_OP(_NOEXCEPT_OP(_Right.swap(_Left)))
	{	// swap vectors
		return (_Right.swap(_Left));
	}
	template<class _Ty, class _A/*, class = enable_if_t<_Is_swappable<_Ty>::value>*/>
	void swap(std::vector<_Ty, _A>& _Left, mse::mstd::vector<_Ty, _A>& _Right) _NOEXCEPT_OP(_NOEXCEPT_OP(_Right.swap(_Left)))
	{	// swap vectors
		return (_Right.swap(_Left));
	}
}

namespace mse {
	namespace mstd {

#endif /*MSE_MSTDVECTOR_DISABLED*/

		/* Overloads for rsv::TReturnableFParam<>. */
		MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_structure_lock_guard)
	}
}

#ifndef MSE_PUSH_MACRO_NOT_SUPPORTED
#pragma pop_macro("MSE_THROW")
#pragma pop_macro("_NOEXCEPT_OP")
#endif // !MSE_PUSH_MACRO_NOT_SUPPORTED

#ifdef _MSC_VER
#pragma warning( pop )  
#endif /*_MSC_VER*/

#endif /*ndef MSEMSTDVECTOR_H*/
