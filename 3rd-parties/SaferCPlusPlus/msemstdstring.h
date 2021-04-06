
// Copyright (c) 2015 Noah Lopez
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef MSEMSTDSTRING_H
#define MSEMSTDSTRING_H

#include "msemsestring.h"

/* Conditional definition of MSE_MSTDSTRING_DISABLED was moved to msemsestring.h. */
//#ifdef MSE_SAFER_SUBSTITUTES_DISABLED
//#define MSE_MSTDSTRING_DISABLED
//#endif /*MSE_SAFER_SUBSTITUTES_DISABLED*/

#ifdef _MSC_VER
#pragma warning( push )  
#pragma warning( disable : 4522 )
#endif /*_MSC_VER*/

#ifndef MSE_PUSH_MACRO_NOT_SUPPORTED
#pragma push_macro("MSE_THROW")
#pragma push_macro("_STD")
#pragma push_macro("_NOEXCEPT")
#pragma push_macro("_NOEXCEPT_OP")
#endif // !MSE_PUSH_MACRO_NOT_SUPPORTED

#ifdef MSE_CUSTOM_THROW_DEFINITION
#define MSE_THROW(x) MSE_CUSTOM_THROW_DEFINITION(x)
#else // MSE_CUSTOM_THROW_DEFINITION
#define MSE_THROW(x) throw(x)
#endif // MSE_CUSTOM_THROW_DEFINITION

#ifndef _STD
#define _STD std::
#endif /*_STD*/

#ifndef _NOEXCEPT
#define _NOEXCEPT
#endif /*_NOEXCEPT*/

#ifndef _NOEXCEPT_OP
#define _NOEXCEPT_OP(x)	noexcept(x)
#endif /*_NOEXCEPT_OP*/

namespace mse {

	namespace mstd {

#ifdef MSE_MSTDSTRING_DISABLED
		template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty> > using basic_string = std::basic_string<_Ty, _Traits, _A>;

		namespace ns_basic_string {

			struct dummy_xscope_structure_lock_guard {};

			template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty> >
			class xscope_structure_lock_guard : public mse::us::impl::Txscope_structure_lock_guard_of_wrapper<basic_string<_Ty, _Traits, _A>, dummy_xscope_structure_lock_guard> {
			public:
				typedef mse::us::impl::Txscope_structure_lock_guard_of_wrapper<basic_string<_Ty, _Traits, _A>, dummy_xscope_structure_lock_guard> base_class;

				xscope_structure_lock_guard(const xscope_structure_lock_guard&) = default;
				xscope_structure_lock_guard(xscope_structure_lock_guard&&) = default;

				xscope_structure_lock_guard(const mse::TXScopeObjFixedPointer<basic_string<_Ty, _Traits, _A> >& owner_ptr)
					: base_class(owner_ptr, dummy_xscope_structure_lock_guard()) {}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
				xscope_structure_lock_guard(const mse::TXScopeFixedPointer<basic_string<_Ty, _Traits, _A> >& owner_ptr)
					: base_class(owner_ptr, dummy_xscope_structure_lock_guard()) {}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
			};
			template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty> >
			class xscope_const_structure_lock_guard : public mse::us::impl::Txscope_const_structure_lock_guard_of_wrapper<basic_string<_Ty, _Traits, _A>, dummy_xscope_structure_lock_guard> {
			public:
				typedef mse::us::impl::Txscope_structure_lock_guard_of_wrapper<basic_string<_Ty, _Traits, _A>, dummy_xscope_structure_lock_guard> base_class;

				xscope_const_structure_lock_guard(const xscope_const_structure_lock_guard&) = default;
				xscope_const_structure_lock_guard(xscope_const_structure_lock_guard&&) = default;

				xscope_const_structure_lock_guard(const mse::TXScopeObjFixedConstPointer<basic_string<_Ty, _Traits, _A> >& owner_ptr)
					: base_class(owner_ptr, dummy_xscope_structure_lock_guard()) {}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
				xscope_const_structure_lock_guard(const mse::TXScopeFixedConstPointer<basic_string<_Ty, _Traits, _A> >& owner_ptr)
					: base_class(owner_ptr, dummy_xscope_structure_lock_guard()) {}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
			};
		}

		template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty> >
		auto make_xscope_structure_lock_guard(const mse::TXScopeObjFixedPointer<basic_string<_Ty, _Traits, _A> >& owner_ptr) {
			return ns_basic_string::xscope_structure_lock_guard<_Ty, _Traits, _A>(owner_ptr);
		}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty> >
		auto make_xscope_structure_lock_guard(const mse::TXScopeFixedPointer<basic_string<_Ty, _Traits, _A> >& owner_ptr) {
			return ns_basic_string::xscope_structure_lock_guard<_Ty, _Traits, _A>(owner_ptr);
		}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty> >
		auto make_xscope_structure_lock_guard(const mse::TXScopeObjFixedConstPointer<basic_string<_Ty, _Traits, _A> >& owner_ptr) {
			return ns_basic_string::xscope_const_structure_lock_guard<_Ty, _Traits, _A>(owner_ptr);
		}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty> >
		auto make_xscope_structure_lock_guard(const mse::TXScopeFixedConstPointer<basic_string<_Ty, _Traits, _A> >& owner_ptr) {
			return ns_basic_string::xscope_const_structure_lock_guard<_Ty, _Traits, _A>(owner_ptr);
		}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

		template<class _Ty, class _A = std::allocator<_Ty> >
		using Tbasic_string_xscope_const_iterator = typename std::basic_string<_Ty, _A>::const_iterator;
		template<class _Ty, class _A = std::allocator<_Ty> >
		using Tbasic_string_xscope_iterator = typename std::basic_string<_Ty, _A>::iterator;

#else /*MSE_MSTDSTRING_DISABLED*/

		class mstdbasic_string_range_error : public std::range_error {
		public:
			using std::range_error::range_error;
		};

		template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty> >
		class basic_string;

		namespace impl {
			namespace bs {
				template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty>>
				std::basic_istream<_Ty, _Traits>& in_from_stream(std::basic_istream<_Ty, _Traits>&& _Istr, basic_string<_Ty, _Traits, _A>& _Str);
				template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty>>
				std::basic_istream<_Ty, _Traits>& in_from_stream(std::basic_istream<_Ty, _Traits>& _Istr, basic_string<_Ty, _Traits, _A>& _Str);
				template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty>>
				std::basic_ostream<_Ty, _Traits>& out_to_stream(std::basic_ostream<_Ty, _Traits>& _Ostr, const basic_string<_Ty, _Traits, _A>& _Str);
			}
		}

		namespace ns_basic_string {
			/* For each (scope) basic_string instance, only one instance of xscope_structure_lock_guard may exist at any one
			time. While an instance of xscope_structure_lock_guard exists it ensures that direct (scope) pointers to
			individual elements in the basic_string do not become invalid by preventing any operation that might resize the basic_string
			or increase its capacity. Any attempt to execute such an operation would result in an exception. */
			template<class _Ty, class _Traits, class _A/* = std::allocator<_Ty> */> class xscope_structure_lock_guard;
			template<class _Ty, class _Traits, class _A/* = std::allocator<_Ty> */> class xscope_const_structure_lock_guard;
		}
		/* For each (scope) basic_string instance, only one instance of xscope_structure_lock_guard may exist at any one
		time. While an instance of xscope_structure_lock_guard exists it ensures that direct (scope) pointers to
		individual elements in the basic_string do not become invalid by preventing any operation that might resize the basic_string
		or increase its capacity. Any attempt to execute such an operation would result in an exception. */
		template<class _Ty, class _Traits, class _A/* = std::allocator<_Ty> */>
		auto make_xscope_structure_lock_guard(const mse::TXScopeObjFixedPointer<basic_string<_Ty, _Traits, _A> >& owner_ptr) -> decltype(ns_basic_string::xscope_structure_lock_guard<_Ty, _Traits, _A>(owner_ptr));
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _Ty, class _Traits, class _A/* = std::allocator<_Ty> */>
		auto make_xscope_structure_lock_guard(const mse::TXScopeFixedPointer<basic_string<_Ty, _Traits, _A> >& owner_ptr) -> decltype(ns_basic_string::xscope_structure_lock_guard<_Ty, _Traits, _A>(owner_ptr));
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _Ty, class _Traits, class _A/* = std::allocator<_Ty> */>
		auto make_xscope_structure_lock_guard(const mse::TXScopeObjFixedConstPointer<basic_string<_Ty, _Traits, _A> >& owner_ptr) -> decltype(ns_basic_string::xscope_const_structure_lock_guard<_Ty, _Traits, _A>(owner_ptr));
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _Ty, class _Traits, class _A/* = std::allocator<_Ty> */>
		auto make_xscope_structure_lock_guard(const mse::TXScopeFixedConstPointer<basic_string<_Ty, _Traits, _A> >& owner_ptr) -> decltype(ns_basic_string::xscope_const_structure_lock_guard<_Ty, _Traits, _A>(owner_ptr));
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
	}

	template<class _Ty, class _Traits, class _A/* = std::allocator<_Ty> */>
	auto make_xscope_structure_lock_guard(const mse::TXScopeObjFixedPointer<mstd::basic_string<_Ty, _Traits, _A> >& owner_ptr) -> decltype(mstd::make_xscope_structure_lock_guard(owner_ptr)) {
		mstd::make_xscope_structure_lock_guard(owner_ptr);
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, class _Traits, class _A/* = std::allocator<_Ty> */>
	auto make_xscope_structure_lock_guard(const mse::TXScopeFixedPointer<mstd::basic_string<_Ty, _Traits, _A> >& owner_ptr) -> decltype(mstd::make_xscope_structure_lock_guard(owner_ptr)) {
		mstd::make_xscope_structure_lock_guard(owner_ptr);
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, class _Traits, class _A/* = std::allocator<_Ty> */>
	auto make_xscope_structure_lock_guard(const mse::TXScopeObjFixedConstPointer<mstd::basic_string<_Ty, _Traits, _A> >& owner_ptr) -> decltype(mstd::make_xscope_structure_lock_guard(owner_ptr)) {
		mstd::make_xscope_structure_lock_guard(owner_ptr);
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, class _Traits, class _A/* = std::allocator<_Ty> */>
	auto make_xscope_structure_lock_guard(const mse::TXScopeFixedConstPointer<mstd::basic_string<_Ty, _Traits, _A> >& owner_ptr) -> decltype(mstd::make_xscope_structure_lock_guard(owner_ptr)) {
		mstd::make_xscope_structure_lock_guard(owner_ptr);
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	namespace mstd {

		/* Following are some template (iterator) classes that, organizationally, should be members of mstd::basic_string<>. (And they
		used to be.) However, being a member of mstd::basic_string<> makes them "dependent types", and dependent types do not participate
		in automatic template parameter type deduction. So we had to haul them here outside of mstd::basic_string<>. */

		template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty> >
		using TXScopeBasicStringPointer = mse::TXScopeFixedPointer<basic_string<_Ty, _Traits, _A> >;
		template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty> >
		using TXScopeBasicStringConstPointer = mse::TXScopeFixedConstPointer<basic_string<_Ty, _Traits, _A> >;

		template<class _Ty, class _Traits, class _A>
		class Tbasic_string_xscope_iterator;

		template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty> >
		class Tbasic_string_xscope_const_iterator : public mse::TFriendlyAugmentedRAConstIterator<mse::us::impl::TXScopeCSLSStrongRAConstIterator<TXScopeBasicStringConstPointer<_Ty, _Traits, _A>, mse::mstd::ns_basic_string::xscope_const_structure_lock_guard<_Ty, _Traits, _A> > >
			/*, public mse::us::impl::XScopeContainsNonOwningScopeReferenceTagBase, public mse::us::impl::AsyncNotShareableAndNotPassableTagBase*/ {
		public:
			typedef mse::TFriendlyAugmentedRAConstIterator<mse::us::impl::TXScopeCSLSStrongRAConstIterator<TXScopeBasicStringConstPointer<_Ty, _Traits, _A>, mse::mstd::ns_basic_string::xscope_const_structure_lock_guard<_Ty, _Traits, _A> > > base_class;
			MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(Tbasic_string_xscope_const_iterator, base_class);

			MSE_USING_ASSIGNMENT_OPERATOR(base_class);
			auto& operator=(Tbasic_string_xscope_const_iterator&& _X) & { base_class::operator=(MSE_FWD(_X)); return (*this); }
			auto& operator=(const Tbasic_string_xscope_const_iterator& _X) & { base_class::operator=(_X); return (*this); }

			Tbasic_string_xscope_const_iterator& operator ++() & { base_class::operator ++(); return (*this); }
			Tbasic_string_xscope_const_iterator operator++(int) { Tbasic_string_xscope_const_iterator _Tmp = *this; base_class::operator++(); return (_Tmp); }
			Tbasic_string_xscope_const_iterator& operator --() & { base_class::operator --(); return (*this); }
			Tbasic_string_xscope_const_iterator operator--(int) { Tbasic_string_xscope_const_iterator _Tmp = *this; base_class::operator--(); return (_Tmp); }

			Tbasic_string_xscope_const_iterator& operator +=(difference_type n) & { base_class::operator +=(n); return (*this); }
			Tbasic_string_xscope_const_iterator& operator -=(difference_type n) & { base_class::operator -=(n); return (*this); }
			Tbasic_string_xscope_const_iterator operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
			Tbasic_string_xscope_const_iterator operator-(difference_type n) const { return ((*this) + (-n)); }
			difference_type operator-(const Tbasic_string_xscope_const_iterator& _Right_cref) const { return base_class::operator-(_Right_cref); }
			const_reference operator*() const { return base_class::operator*(); }

			Tbasic_string_xscope_const_iterator operator=(Tbasic_string_xscope_const_iterator&& _X) && { base_class::operator=(MSE_FWD(_X)); return std::move(*this); }
			Tbasic_string_xscope_const_iterator operator=(const Tbasic_string_xscope_const_iterator& _X) && { base_class::operator=(_X); return std::move(*this); }
			Tbasic_string_xscope_const_iterator operator ++() && { base_class::operator ++(); return std::move(*this); }
			Tbasic_string_xscope_const_iterator operator --() && { base_class::operator --(); return std::move(*this); }
			Tbasic_string_xscope_const_iterator operator +=(difference_type n) && { base_class::operator +=(n); return std::move(*this); }
			Tbasic_string_xscope_const_iterator operator -=(difference_type n) && { base_class::operator -=(n); return std::move(*this); }

			void set_to_const_item_pointer(const Tbasic_string_xscope_const_iterator& _Right_cref) { base_class::set_to_item_pointer(_Right_cref); }

			MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(base_class);
			void xscope_cslsstrong_iterator_type_tag() const {}

		private:
			MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

			friend class /*_Myt*/basic_string<_Ty, _Traits, _A>;
			template<class _Ty2, class _Traits2, class _A2>
			friend class Tbasic_string_xscope_iterator;
		};

		template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty> >
		class Tbasic_string_xscope_iterator : public mse::TFriendlyAugmentedRAIterator<mse::us::impl::TXScopeCSLSStrongRAIterator<TXScopeBasicStringPointer<_Ty, _Traits, _A>, mse::mstd::ns_basic_string::xscope_structure_lock_guard<_Ty, _Traits, _A>/*decltype(mse::make_xscope_structure_lock_guard(std::declval<TXScopeBasicStringPointer<_Ty, _Traits, _A> >()))*/> >
			/*, public mse::us::impl::XScopeContainsNonOwningScopeReferenceTagBase, public mse::us::impl::AsyncNotShareableAndNotPassableTagBase*/ {
		public:
			typedef mse::TFriendlyAugmentedRAIterator<mse::us::impl::TXScopeCSLSStrongRAIterator<TXScopeBasicStringPointer<_Ty, _Traits, _A>, mse::mstd::ns_basic_string::xscope_structure_lock_guard<_Ty, _Traits, _A>/*decltype(mse::make_xscope_structure_lock_guard(std::declval<TXScopeBasicStringPointer<_Ty, _Traits, _A> >()))*/> > base_class;
			MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(Tbasic_string_xscope_iterator, base_class);

			MSE_USING_ASSIGNMENT_OPERATOR(base_class);
			auto& operator=(Tbasic_string_xscope_iterator&& _X) & { base_class::operator=(MSE_FWD(_X)); return (*this); }
			auto& operator=(const Tbasic_string_xscope_iterator& _X) & { base_class::operator=(_X); return (*this); }

			Tbasic_string_xscope_iterator& operator ++() & { base_class::operator ++(); return (*this); }
			Tbasic_string_xscope_iterator operator++(int) { Tbasic_string_xscope_iterator _Tmp = *this; base_class::operator++(); return (_Tmp); }
			Tbasic_string_xscope_iterator& operator --() & { base_class::operator --(); return (*this); }
			Tbasic_string_xscope_iterator operator--(int) { Tbasic_string_xscope_iterator _Tmp = *this; base_class::operator--(); return (_Tmp); }

			Tbasic_string_xscope_iterator& operator +=(difference_type n) & { base_class::operator +=(n); return (*this); }
			Tbasic_string_xscope_iterator& operator -=(difference_type n) & { base_class::operator -=(n); return (*this); }
			Tbasic_string_xscope_iterator operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
			Tbasic_string_xscope_iterator operator-(difference_type n) const { return ((*this) + (-n)); }
			difference_type operator-(const Tbasic_string_xscope_iterator& _Right_cref) const { return base_class::operator-(_Right_cref); }

			Tbasic_string_xscope_iterator operator=(Tbasic_string_xscope_iterator&& _X) && { base_class::operator=(MSE_FWD(_X)); return std::move(*this); }
			Tbasic_string_xscope_iterator operator=(const Tbasic_string_xscope_iterator _X) && { base_class::operator=(_X); return std::move(*this); }
			Tbasic_string_xscope_iterator operator ++() && { base_class::operator ++(); return std::move(*this); }
			Tbasic_string_xscope_iterator operator --() && { base_class::operator --(); return std::move(*this); }
			Tbasic_string_xscope_iterator operator +=(difference_type n) && { base_class::operator +=(n); return std::move(*this); }
			Tbasic_string_xscope_iterator operator -=(difference_type n) && { base_class::operator -=(n); return std::move(*this); }

			void set_to_item_pointer(const Tbasic_string_xscope_iterator& _Right_cref) { base_class::set_to_item_pointer(_Right_cref); }

			MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(base_class);
			void xscope_cslsstrong_iterator_type_tag() const {}

		private:
			MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

			friend class /*_Myt*/basic_string<_Ty, _Traits, _A>;
		};

		template<class _Ty, class _Traits/* = std::char_traits<_Ty>*/, class _A/* = std::allocator<_Ty> */>
		class basic_string : public mse::us::impl::AsyncNotShareableTagBase, public us::impl::ContiguousSequenceContainerTagBase, public us::impl::LockableStructureContainerTagBase {
		public:
			typedef basic_string _Myt;
			typedef mse::us::msebasic_string<_Ty, _Traits, _A> _MBS;

			typedef typename _MBS::allocator_type allocator_type;
			MSE_INHERITED_RANDOM_ACCESS_MEMBER_TYPE_DECLARATIONS(_MBS);
			static const size_t npos = size_t(-1);

			operator mse::nii_basic_string<_Ty, _Traits, _A>() const& { return msebasic_string(); }
			operator mse::nii_basic_string<_Ty, _Traits, _A>()&& { return std::forward<_MBS>(msebasic_string()); }
			operator std::basic_string<_Ty, _Traits, _A>() const& { return msebasic_string(); }
			operator std::basic_string<_Ty, _Traits, _A>()&& { return std::forward<std::basic_string<_Ty, _Traits, _A> >(msebasic_string()); }

			explicit basic_string(const _A& _Al = _A()) : m_shptr(std::make_shared<_MBS>(_Al)) {}
			explicit basic_string(size_type _N) : m_shptr(std::make_shared<_MBS>(_N)) {}
#ifdef MSE_HAS_CXX17
			template<class _Alloc2 = _A, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc2>::value, int> = 0>
#endif /* MSE_HAS_CXX17 */
			explicit basic_string(size_type _N, const _Ty& _V, const _A& _Al = _A()) : m_shptr(std::make_shared<_MBS>(_N, _V, _Al)) {}
			basic_string(_Myt&& _X) : m_shptr(std::make_shared<_MBS>(MSE_FWD(_X).msebasic_string())) {}
			basic_string(const _Myt& _X) : basic_string(mse::us::unsafe_make_xscope_const_pointer_to(_X)) {}
			basic_string(_MBS&& _X) : m_shptr(std::make_shared<_MBS>(MSE_FWD(_X))) {}
			basic_string(const _MBS& _X) : basic_string(mse::us::unsafe_make_xscope_const_pointer_to(_X)) {}
			basic_string(mse::nii_basic_string<_Ty, _Traits>&& _X) : m_shptr(std::make_shared<_MBS>(MSE_FWD(_X))) {}
			basic_string(const mse::nii_basic_string<_Ty, _Traits>& _X) : basic_string(mse::us::unsafe_make_xscope_const_pointer_to(_X)) {}
			basic_string(std::basic_string<_Ty, _Traits>&& _X) : m_shptr(std::make_shared<_MBS>(MSE_FWD(_X))) {}
			basic_string(const std::basic_string<_Ty, _Traits>& _X) : basic_string(mse::us::unsafe_make_xscope_const_pointer_to(_X)) {}
			typedef typename _MBS::const_iterator _It;
			basic_string(_It _F, _It _L, const _A& _Al = _A()) : m_shptr(std::make_shared<_MBS>(_F, _L, _Al)) {}
			basic_string(const _Ty* _F, const _Ty* _L, const _A& _Al = _A()) : m_shptr(std::make_shared<_MBS>(_F, _L, _Al)) {}
			template<class _Iter, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value> MSE_IMPL_EIS >
			basic_string(_Iter _First, _Iter _Last) : m_shptr(std::make_shared<_MBS>(_First, _Last)) {}
			template<class _Iter, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value> MSE_IMPL_EIS >
			basic_string(_Iter _First, _Iter _Last, const _A& _Al) : m_shptr(std::make_shared<_MBS>(_First, _Last, _Al)) {}
			basic_string(const _Ty* const _Ptr) : m_shptr(std::make_shared<_MBS>(_Ptr)) {}
			basic_string(const _Ty* const _Ptr, const size_t _Count) : m_shptr(std::make_shared<_MBS>(_Ptr, _Count)) {}
			basic_string(const _Myt& _X, const size_type _Roff, const _A& _Al = _A()) : m_shptr(std::make_shared<_MBS>(_X.msebasic_string(), _Roff, npos, _Al)) {}
			basic_string(const _Myt& _X, const size_type _Roff, const size_type _Count, const _A& _Al = _A()) : m_shptr(std::make_shared<_MBS>(_X.msebasic_string(), _Roff, _Count, _Al)) {}

			basic_string(const mse::TXScopeFixedConstPointer<_Myt>& xs_ptr) : m_shptr(std::make_shared<_MBS>(xs_ptr->msebasic_string())) {}
			basic_string(const mse::TXScopeFixedConstPointer<_MBS>& xs_ptr) : m_shptr(std::make_shared<_MBS>(*xs_ptr)) {}
			basic_string(const mse::TXScopeFixedConstPointer<mse::nii_basic_string<_Ty, _Traits> >& xs_ptr) : m_shptr(std::make_shared<_MBS>(*xs_ptr)) {}
			basic_string(const mse::TXScopeFixedConstPointer<std::basic_string<_Ty, _Traits> >& xs_ptr) : m_shptr(std::make_shared<_MBS>(*xs_ptr)) {}
			basic_string(const mse::TXScopeFixedConstPointer<_Myt>& xs_ptr, const size_type _Roff, const _A& _Al = _A()) : m_shptr(std::make_shared<_MBS>(xs_ptr->msebasic_string(), _Roff, npos, _Al)) {}
			basic_string(const mse::TXScopeFixedConstPointer<_Myt>& xs_ptr, const size_type _Roff, const size_type _Count, const _A& _Al = _A()) : m_shptr(std::make_shared<_MBS>(xs_ptr->msebasic_string(), _Roff, _Count, _Al)) {}

#ifdef MSE_HAS_CXX17
			template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1>*/>
			basic_string(const _TParam1& _Right) : m_shptr(std::make_shared<_MBS>()) { assign(_Right); }

			template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1>*/>
			basic_string(const _TParam1& _Right, const size_type _Roff, const size_type _Count, const _A& _Al = _A())
				: m_shptr(std::make_shared<_MBS>(_Al)) {
				assign(_Right, _Roff, _Count);
			}
#else /* MSE_HAS_CXX17 */
			/* construct from mse::string_view and "string sections". */
			template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
			explicit basic_string(const _TStringSection& _X) : m_shptr(std::make_shared<_MBS>(_X)) {}
#endif /* MSE_HAS_CXX17 */

			MSE_IMPL_DESTRUCTOR_PREFIX1 ~basic_string() {
				msebasic_string().note_parent_destruction();
			}

			_Myt& operator=(_Myt&& _X) {
				static_assert(typename std::is_rvalue_reference<decltype(_X)>::type(), "");
				/* It would be more efficient to just move _X.m_shptr into m_shptr, but that would leave _X in what we
				would consider an invalid state. */
				msebasic_string() = std::move(_X.msebasic_string());
				return (*this);
			}
			_Myt& operator=(const _Myt& _X) { msebasic_string() = (_X.msebasic_string()); return (*this); }
			_Myt& operator=(const _Ty* const _Ptr) { msebasic_string() = (_Myt(_Ptr).msebasic_string()); return (*this); }
			void reserve(size_type _Count) { m_shptr->reserve(_Count); }
			void resize(size_type _N, const _Ty& _X = _Ty()) { m_shptr->resize(_N, _X); }
			typename _MBS::const_reference operator[](size_type _P) const { return m_shptr->operator[](_P); }
			typename _MBS::reference operator[](size_type _P) { return m_shptr->operator[](_P); }
			void push_back(_Ty&& _X) { m_shptr->push_back(MSE_FWD(_X)); }
			void push_back(const _Ty& _X) { m_shptr->push_back(_X); }
			void pop_back() { m_shptr->pop_back(); }

			basic_string& assign(mse::TXScopeFixedConstPointer<basic_string> xs_ptr) {
				m_shptr->assign(xs_ptr->msebasic_string());
				return (*this);
			}
			basic_string& assign(const basic_string& _Right) {
				auto xs_ptr = mse::us::unsafe_make_xscope_const_pointer_to(_Right);
				return assign(xs_ptr);
			}
			basic_string& assign(mse::TXScopeFixedConstPointer<basic_string> xs_ptr, const size_type _Roff, size_type _Count = npos) {
				m_shptr->assign(xs_ptr->msebasic_string(), _Roff, _Count);
				return (*this);
			}
			basic_string& assign(const basic_string& _Right, const size_type _Roff, size_type _Count = npos) {
				auto xs_ptr = mse::us::unsafe_make_xscope_const_pointer_to(_Right);
				return assign(xs_ptr, _Roff, _Count);
			}

			basic_string& assign(const _Ty * const _Ptr, const size_type _Count) {
				m_shptr->assign(_Ptr, _Count);
				return (*this);
			}
			basic_string& assign(const _Ty * const _Ptr) {
				m_shptr->assign(_Ptr);
				return (*this);
			}
			basic_string& assign(const size_type _Count, const _Ty& _Ch) {
				m_shptr->assign(_Count, _Ch);
				return (*this);
			}
			template<class _Iter, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::_mse_Is_iterator_v<_Iter> > MSE_IMPL_EIS >
			basic_string& assign(const _Iter _First, const _Iter _Last) {
				m_shptr->assign(_First, _Last);
				return (*this);
			}

#ifdef MSE_HAS_CXX17
		private:
			template<class _TParam1>
			basic_string& assign_helper1(std::true_type, const _TParam1& _Right) {
				return (assign(static_cast<const basic_string&>(_Right)));
			}
			template<class _TParam1>
			basic_string& assign_helper1(std::false_type, const _TParam1& _Right) {
				m_shptr->assign(_Right);
				return (*this);
			}
		public:
			template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
			basic_string& assign(const _TParam1& _Right) {
				return assign_helper1(typename std::is_base_of<basic_string, _TParam1>::type(), _Right);
			}

		private:
			template<class _TParam1>
			basic_string& assign_helper1(std::true_type, const _TParam1& _Right, const size_type _Roff, const size_type _Count) {
				return (assign(static_cast<const basic_string&>(_Right, _Roff, _Count)));
			}
			template<class _TParam1>
			basic_string& assign_helper1(std::false_type, const _TParam1& _Right, const size_type _Roff, const size_type _Count) {
				m_shptr->assign(_Right, _Roff, _Count);
				return (*this);
			}
		public:
			template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
			basic_string& assign(const _TParam1& _Right, const size_type _Roff, const size_type _Count = npos) {
				return assign_helper1(typename std::is_base_of<basic_string, _TParam1>::type(), _Right, _Roff, _Count);
			}
#else /* MSE_HAS_CXX17 */
			template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
			basic_string& assign(const _TStringSection& _X) {
				m_shptr->assign(_X);
				return (*this);
			}
#endif /* MSE_HAS_CXX17 */

			template<class ..._Valty>
			void emplace_back(_Valty&& ..._Val) { m_shptr->emplace_back(std::forward<_Valty>(_Val)...); }
			void clear() { m_shptr->clear(); }
			void swap(_MBS& _X) { m_shptr->swap(_X); }
			void swap(_Myt& _X) { m_shptr->swap(_X.msebasic_string()); }
			void swap(std::basic_string<_Ty, _Traits, _A>& _X) { m_shptr->swap(_X); }
			template<typename _TStateMutex2, template<typename> class _TTXScopeConstIterator2>
			void swap(mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex2, _TTXScopeConstIterator2>& _X) { m_shptr->swap(_X); }

			basic_string(_XSTD initializer_list<typename _MBS::value_type> _Ilist, const _A& _Al = _A()) : m_shptr(std::make_shared<_MBS>(_Ilist, _Al)) {}
			_Myt& operator=(_XSTD initializer_list<typename _MBS::value_type> _Ilist) { msebasic_string() = (_Ilist); return (*this); }
			void assign(_XSTD initializer_list<typename _MBS::value_type> _Ilist) { m_shptr->assign(_Ilist); }

			size_type capacity() const _NOEXCEPT { return m_shptr->capacity(); }
			void shrink_to_fit() { m_shptr->shrink_to_fit(); }
			size_type length() const _NOEXCEPT { return m_shptr->length(); }
			size_type size() const _NOEXCEPT { return m_shptr->size(); }
			size_type max_size() const _NOEXCEPT { return m_shptr->max_size(); }
			bool empty() const _NOEXCEPT { return m_shptr->empty(); }
			_A get_allocator() const _NOEXCEPT { return m_shptr->get_allocator(); }
			typename _MBS::const_reference at(size_type _Pos) const { return m_shptr->at(_Pos); }
			typename _MBS::reference at(size_type _Pos) { return m_shptr->at(_Pos); }
			typename _MBS::reference front() { return m_shptr->front(); }
			typename _MBS::const_reference front() const { return m_shptr->front(); }
			typename _MBS::reference back() { return m_shptr->back(); }
			typename _MBS::const_reference back() const { return m_shptr->back(); }

			/* Try to avoid using these whenever possible. */
			value_type *data() _NOEXCEPT {
				return m_shptr->data();
			}
			const value_type *data() const _NOEXCEPT {
				return m_shptr->data();
			}

			typedef Tbasic_string_xscope_const_iterator<_Ty, _Traits, _A> xscope_const_iterator;
			typedef Tbasic_string_xscope_iterator<_Ty, _Traits, _A> xscope_iterator;

			class const_iterator : public _MBS::random_access_const_iterator_base {
			public:
				typedef typename _MBS::random_access_const_iterator_base base_class;
				MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

				const_iterator() {}
				const_iterator(const const_iterator& src_cref) : m_msebasic_string_cshptr(src_cref.m_msebasic_string_cshptr) {
					(*this) = src_cref;
				}
				~const_iterator() {}
				const typename _MBS::ss_const_iterator_type& msebasic_string_ss_const_iterator_type() const { return m_ss_const_iterator; }
				typename _MBS::ss_const_iterator_type& msebasic_string_ss_const_iterator_type() { return m_ss_const_iterator; }
				const typename _MBS::ss_const_iterator_type& mvssci() const { return msebasic_string_ss_const_iterator_type(); }
				typename _MBS::ss_const_iterator_type& mvssci() { return msebasic_string_ss_const_iterator_type(); }

				void reset() { msebasic_string_ss_const_iterator_type().reset(); }
				bool points_to_an_item() const { return msebasic_string_ss_const_iterator_type().points_to_an_item(); }
				bool points_to_end_marker() const { return msebasic_string_ss_const_iterator_type().points_to_end_marker(); }
				bool points_to_beginning() const { return msebasic_string_ss_const_iterator_type().points_to_beginning(); }
				/* has_next_item_or_end_marker() is just an alias for points_to_an_item(). */
				bool has_next_item_or_end_marker() const { return msebasic_string_ss_const_iterator_type().has_next_item_or_end_marker(); }
				/* has_next() is just an alias for points_to_an_item() that's familiar to java programmers. */
				bool has_next() const { return msebasic_string_ss_const_iterator_type().has_next(); }
				bool has_previous() const { return msebasic_string_ss_const_iterator_type().has_previous(); }
				void set_to_beginning() { msebasic_string_ss_const_iterator_type().set_to_beginning(); }
				void set_to_end_marker() { msebasic_string_ss_const_iterator_type().set_to_end_marker(); }
				void set_to_next() { msebasic_string_ss_const_iterator_type().set_to_next(); }
				void set_to_previous() { msebasic_string_ss_const_iterator_type().set_to_previous(); }
				const_iterator& operator ++() { msebasic_string_ss_const_iterator_type().operator ++(); return (*this); }
				const_iterator operator++(int) { const_iterator _Tmp = *this; ++*this; return (_Tmp); }
				const_iterator& operator --() { msebasic_string_ss_const_iterator_type().operator --(); return (*this); }
				const_iterator operator--(int) { const_iterator _Tmp = *this; --*this; return (_Tmp); }
				void advance(typename _MBS::difference_type n) { msebasic_string_ss_const_iterator_type().advance(n); }
				void regress(typename _MBS::difference_type n) { msebasic_string_ss_const_iterator_type().regress(n); }
				const_iterator& operator +=(difference_type n) { msebasic_string_ss_const_iterator_type().operator +=(n); return (*this); }
				const_iterator& operator -=(difference_type n) { msebasic_string_ss_const_iterator_type().operator -=(n); return (*this); }
				const_iterator operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
				const_iterator operator-(difference_type n) const { return ((*this) + (-n)); }
				typename _MBS::difference_type operator-(const const_iterator& _Right_cref) const { return msebasic_string_ss_const_iterator_type() - (_Right_cref.msebasic_string_ss_const_iterator_type()); }
				typename _MBS::const_reference operator*() const {
					(*m_msebasic_string_cshptr).assert_parent_not_destroyed();
					return msebasic_string_ss_const_iterator_type().operator*();
				}
				typename _MBS::const_reference item() const { return operator*(); }
				typename _MBS::const_reference previous_item() const { return msebasic_string_ss_const_iterator_type().previous_item(); }
				typename _MBS::const_pointer operator->() const {
					(*m_msebasic_string_cshptr).assert_parent_not_destroyed();
					return msebasic_string_ss_const_iterator_type().operator->();
				}
				typename _MBS::const_reference operator[](typename _MBS::difference_type _Off) const { return (*(*this + _Off)); }
				bool operator==(const const_iterator& _Right_cref) const { return msebasic_string_ss_const_iterator_type().operator==(_Right_cref.msebasic_string_ss_const_iterator_type()); }
				bool operator!=(const const_iterator& _Right_cref) const { return (!(_Right_cref == (*this))); }
				bool operator<(const const_iterator& _Right) const { return (msebasic_string_ss_const_iterator_type() < _Right.msebasic_string_ss_const_iterator_type()); }
				bool operator<=(const const_iterator& _Right) const { return (msebasic_string_ss_const_iterator_type() <= _Right.msebasic_string_ss_const_iterator_type()); }
				bool operator>(const const_iterator& _Right) const { return (msebasic_string_ss_const_iterator_type() > _Right.msebasic_string_ss_const_iterator_type()); }
				bool operator>=(const const_iterator& _Right) const { return (msebasic_string_ss_const_iterator_type() >= _Right.msebasic_string_ss_const_iterator_type()); }
				void set_to_const_item_pointer(const const_iterator& _Right_cref) { msebasic_string_ss_const_iterator_type().set_to_const_item_pointer(_Right_cref.msebasic_string_ss_const_iterator_type()); }
				msev_size_t position() const { return msebasic_string_ss_const_iterator_type().position(); }
				auto target_container_ptr() const -> decltype(msebasic_string_ss_const_iterator_type().target_container_ptr()) {
					return msebasic_string_ss_const_iterator_type().target_container_ptr();
				}
				void async_not_shareable_and_not_passable_tag() const {}
			private:
				const_iterator(std::shared_ptr<_MBS> msebasic_string_shptr) : m_msebasic_string_cshptr(msebasic_string_shptr) {
					m_ss_const_iterator = msebasic_string_shptr->ss_cbegin();
				}
				std::shared_ptr<const _MBS> m_msebasic_string_cshptr;
				/* m_ss_const_iterator needs to be declared after m_msebasic_string_cshptr so that its destructor will be called first. */
				typename _MBS::ss_const_iterator_type m_ss_const_iterator;
				friend class /*_Myt*/basic_string<_Ty, _Traits, _A>;
				friend class iterator;
			};
			class iterator : public _MBS::random_access_iterator_base {
			public:
				typedef typename _MBS::random_access_iterator_base base_class;
				MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

				iterator() {}
				iterator(const iterator& src_cref) : m_msebasic_string_shptr(src_cref.m_msebasic_string_shptr) {
					(*this) = src_cref;
				}
				~iterator() {}
				const typename _MBS::ss_iterator_type& msebasic_string_ss_iterator_type() const { return m_ss_iterator; }
				typename _MBS::ss_iterator_type& msebasic_string_ss_iterator_type() { return m_ss_iterator; }
				const typename _MBS::ss_iterator_type& mvssi() const { return msebasic_string_ss_iterator_type(); }
				typename _MBS::ss_iterator_type& mvssi() { return msebasic_string_ss_iterator_type(); }
				operator const_iterator() const {
					const_iterator retval(m_msebasic_string_shptr);
					if (m_msebasic_string_shptr) {
						retval.msebasic_string_ss_const_iterator_type().set_to_beginning();
						retval.msebasic_string_ss_const_iterator_type().advance(msev_int(msebasic_string_ss_iterator_type().position()));
					}
					return retval;
				}

				void reset() { msebasic_string_ss_iterator_type().reset(); }
				bool points_to_an_item() const { return msebasic_string_ss_iterator_type().points_to_an_item(); }
				bool points_to_end_marker() const { return msebasic_string_ss_iterator_type().points_to_end_marker(); }
				bool points_to_beginning() const { return msebasic_string_ss_iterator_type().points_to_beginning(); }
				/* has_next_item_or_end_marker() is just an alias for points_to_an_item(). */
				bool has_next_item_or_end_marker() const { return msebasic_string_ss_iterator_type().has_next_item_or_end_marker(); }
				/* has_next() is just an alias for points_to_an_item() that's familiar to java programmers. */
				bool has_next() const { return msebasic_string_ss_iterator_type().has_next(); }
				bool has_previous() const { return msebasic_string_ss_iterator_type().has_previous(); }
				void set_to_beginning() { msebasic_string_ss_iterator_type().set_to_beginning(); }
				void set_to_end_marker() { msebasic_string_ss_iterator_type().set_to_end_marker(); }
				void set_to_next() { msebasic_string_ss_iterator_type().set_to_next(); }
				void set_to_previous() { msebasic_string_ss_iterator_type().set_to_previous(); }
				iterator& operator ++() { msebasic_string_ss_iterator_type().operator ++(); return (*this); }
				iterator operator++(int) { iterator _Tmp = *this; ++*this; return (_Tmp); }
				iterator& operator --() { msebasic_string_ss_iterator_type().operator --(); return (*this); }
				iterator operator--(int) { iterator _Tmp = *this; --*this; return (_Tmp); }
				void advance(typename _MBS::difference_type n) { msebasic_string_ss_iterator_type().advance(n); }
				void regress(typename _MBS::difference_type n) { msebasic_string_ss_iterator_type().regress(n); }
				iterator& operator +=(difference_type n) { msebasic_string_ss_iterator_type().operator +=(n); return (*this); }
				iterator& operator -=(difference_type n) { msebasic_string_ss_iterator_type().operator -=(n); return (*this); }
				iterator operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
				iterator operator-(difference_type n) const { return ((*this) + (-n)); }
				typename _MBS::difference_type operator-(const iterator& _Right_cref) const { return msebasic_string_ss_iterator_type() - (_Right_cref.msebasic_string_ss_iterator_type()); }
				typename _MBS::reference operator*() const { return msebasic_string_ss_iterator_type().operator*(); }
				typename _MBS::reference item() const { return operator*(); }
				typename _MBS::reference previous_item() const { return msebasic_string_ss_iterator_type().previous_item(); }
				typename _MBS::pointer operator->() const { return msebasic_string_ss_iterator_type().operator->(); }
				typename _MBS::reference operator[](typename _MBS::difference_type _Off) const { return (*(*this + _Off)); }
				bool operator==(const iterator& _Right_cref) const { return msebasic_string_ss_iterator_type().operator==(_Right_cref.msebasic_string_ss_iterator_type()); }
				bool operator!=(const iterator& _Right_cref) const { return (!(_Right_cref == (*this))); }
				bool operator<(const iterator& _Right) const { return (msebasic_string_ss_iterator_type() < _Right.msebasic_string_ss_iterator_type()); }
				bool operator<=(const iterator& _Right) const { return (msebasic_string_ss_iterator_type() <= _Right.msebasic_string_ss_iterator_type()); }
				bool operator>(const iterator& _Right) const { return (msebasic_string_ss_iterator_type() > _Right.msebasic_string_ss_iterator_type()); }
				bool operator>=(const iterator& _Right) const { return (msebasic_string_ss_iterator_type() >= _Right.msebasic_string_ss_iterator_type()); }
				void set_to_item_pointer(const iterator& _Right_cref) { msebasic_string_ss_iterator_type().set_to_item_pointer(_Right_cref.msebasic_string_ss_iterator_type()); }
				msev_size_t position() const { return msebasic_string_ss_iterator_type().position(); }
				auto target_container_ptr() const -> decltype(msebasic_string_ss_iterator_type().target_container_ptr()) {
					return msebasic_string_ss_iterator_type().target_container_ptr();
				}
				void async_not_shareable_and_not_passable_tag() const {}
			private:
				std::shared_ptr<_MBS> m_msebasic_string_shptr;
				/* m_ss_iterator needs to be declared after m_msebasic_string_shptr so that its destructor will be called first. */
				typename _MBS::ss_iterator_type m_ss_iterator;
				friend class /*_Myt*/basic_string<_Ty, _Traits, _A>;
			};

			iterator begin() {	// return iterator for beginning of mutable sequence
				iterator retval; retval.m_msebasic_string_shptr = this->m_shptr;
				(retval.m_ss_iterator) = m_shptr->ss_begin();
				return retval;
			}
			const_iterator begin() const {	// return iterator for beginning of nonmutable sequence
				const_iterator retval; retval.m_msebasic_string_cshptr = this->m_shptr;
				(retval.m_ss_const_iterator) = m_shptr->ss_begin();
				return retval;
			}
			iterator end() {	// return iterator for end of mutable sequence
				iterator retval; retval.m_msebasic_string_shptr = this->m_shptr;
				(retval.m_ss_iterator) = m_shptr->ss_end();
				return retval;
			}
			const_iterator end() const {	// return iterator for end of nonmutable sequence
				const_iterator retval; retval.m_msebasic_string_cshptr = this->m_shptr;
				(retval.m_ss_const_iterator) = m_shptr->ss_end();
				return retval;
			}
			const_iterator cbegin() const {	// return iterator for beginning of nonmutable sequence
				const_iterator retval; retval.m_msebasic_string_cshptr = this->m_shptr;
				(retval.m_ss_const_iterator) = m_shptr->ss_cbegin();
				return retval;
			}
			const_iterator cend() const {	// return iterator for end of nonmutable sequence
				const_iterator retval; retval.m_msebasic_string_cshptr = this->m_shptr;
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


			basic_string(const const_iterator &start, const const_iterator &end, const _A& _Al = _A())
				: m_shptr(std::make_shared<_MBS>(start.msebasic_string_ss_const_iterator_type(), end.msebasic_string_ss_const_iterator_type(), _Al)) {}
			void assign(const const_iterator &start, const const_iterator &end) {
				m_shptr->assign(start.msebasic_string_ss_const_iterator_type(), end.msebasic_string_ss_const_iterator_type());
			}
			void assign_inclusive(const const_iterator &first, const const_iterator &last) {
				m_shptr->assign_inclusive(first.msebasic_string_ss_const_iterator_type(), last.msebasic_string_ss_const_iterator_type());
			}
			iterator insert_before(const const_iterator &pos, size_type _M, const _Ty& _X) {
				auto res = m_shptr->insert_before(pos.msebasic_string_ss_const_iterator_type(), _M, _X);
				iterator retval = begin(); retval.msebasic_string_ss_iterator_type() = res;
				return retval;
			}
			iterator insert_before(const const_iterator &pos, _Ty&& _X) {
				auto res = m_shptr->insert_before(pos.msebasic_string_ss_const_iterator_type(), MSE_FWD(_X));
				iterator retval = begin(); retval.msebasic_string_ss_iterator_type() = res;
				return retval;
			}
			iterator insert_before(const const_iterator &pos, const _Ty& _X = _Ty()) { return insert_before(pos, 1, _X); }
			template<class _Iter
				//>mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value, typename base_class::iterator>
				, class = mse::impl::_mse_RequireInputIter<_Iter> >
			iterator insert_before(const const_iterator &pos, const _Iter &start, const _Iter &end) {
				auto res = m_shptr->insert_before(pos.msebasic_string_ss_const_iterator_type(), start, end);
				iterator retval = begin(); retval.msebasic_string_ss_iterator_type() = res;
				return retval;
			}
			template<class _Iter
				//>mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value, typename base_class::iterator>
				, class = mse::impl::_mse_RequireInputIter<_Iter> >
			iterator insert_before_inclusive(const const_iterator &pos, const _Iter &first, const _Iter &last) {
				auto end = last; end++;
				return insert_before(pos, first, end);
			}
			iterator insert_before(const const_iterator &pos, _XSTD initializer_list<typename _MBS::value_type> _Ilist) {	// insert initializer_list
				auto res = m_shptr->insert_before(pos.msebasic_string_ss_const_iterator_type(), _Ilist);
				iterator retval = begin(); retval.msebasic_string_ss_iterator_type() = res;
				return retval;
			}
			template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
			iterator insert_before(const const_iterator &pos, const _TStringSection& _X) {	// insert initializer_list
				auto res = m_shptr->insert_before(pos.msebasic_string_ss_const_iterator_type(), _X);
				iterator retval = begin(); retval.msebasic_string_ss_iterator_type() = res;
				return retval;
			}
			basic_string& insert_before(msev_size_t pos, const _Ty& _X = _Ty()) {
				m_shptr->insert_before(pos, _X);
				return *this;
			}
			basic_string& insert_before(msev_size_t pos, size_type _M, const _Ty& _X) {
				m_shptr->insert_before(pos, _M, _X);
				return *this;
			}
			basic_string& insert_before(msev_size_t pos, _XSTD initializer_list<typename _MBS::value_type> _Ilist) {	// insert initializer_list
				m_shptr->insert_before(pos, _Ilist);
				return *this;
			}
			template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
			basic_string& insert_before(msev_size_t pos, const _TStringSection& _X) {
				m_shptr->insert_before(pos, _X);
				return *this;
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
			iterator insert(const const_iterator &pos, _XSTD initializer_list<typename _MBS::value_type> _Ilist) { return insert_before(pos, _Ilist); }
			template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
			iterator insert(const const_iterator &pos, const _TStringSection& _X) { return insert_before(pos, _X); }
			template<class ..._Valty>
			iterator emplace(const const_iterator &pos, _Valty&& ..._Val) {
				auto res = m_shptr->emplace(pos.msebasic_string_ss_const_iterator_type(), std::forward<_Valty>(_Val)...);
				iterator retval = begin(); retval.msebasic_string_ss_iterator_type() = res;
				return retval;
			}
			iterator erase(const const_iterator &pos) {
				auto res = m_shptr->erase(pos.msebasic_string_ss_const_iterator_type());
				iterator retval = begin(); retval.msebasic_string_ss_iterator_type() = res;
				return retval;
			}
			iterator erase(const const_iterator &start, const const_iterator &end) {
				auto res = m_shptr->erase(start.msebasic_string_ss_const_iterator_type(), end.msebasic_string_ss_const_iterator_type());
				iterator retval = begin(); retval.msebasic_string_ss_iterator_type() = res;
				return retval;
			}
			iterator erase_inclusive(const const_iterator &first, const const_iterator &last) {
				auto end = last; end.set_to_next();
				return erase_inclusive(first, end);
			}
			bool operator==(const _Myt& _Right) const {	// test for basic_string equality
				return ((*(_Right.m_shptr)) == (*m_shptr));
			}
			bool operator<(const _Myt& _Right) const {	// test if _Left < _Right for basic_strings
				return ((*m_shptr) < (*(_Right.m_shptr)));
			}


			basic_string& append(mse::TXScopeFixedConstPointer<basic_string> xs_ptr) {
				msebasic_string().append(xs_ptr->msebasic_string());
				return (*this);
			}
			basic_string& append(const basic_string& _Right) {	// append _Right
				auto xs_ptr = mse::us::unsafe_make_xscope_const_pointer_to(_Right);
				return append(xs_ptr);
			}
			basic_string& append(mse::TXScopeFixedConstPointer<basic_string> xs_ptr, const size_type _Roff, size_type _Count = npos) {
				msebasic_string().append(xs_ptr->msebasic_string(), mse::as_a_size_t(_Roff), mse::as_a_size_t(_Count));
				return (*this);
			}
			basic_string& append(const basic_string& _Right, const size_type _Roff, size_type _Count = npos) {
				auto xs_ptr = mse::us::unsafe_make_xscope_const_pointer_to(_Right);
				return append(xs_ptr, _Roff, _Count);
			}

			basic_string& append(const _Ty * const _Ptr, const size_type _Count) {
				msebasic_string().append(_Ptr, mse::as_a_size_t(_Count));
				return (*this);
			}
			basic_string& append(const _Ty * const _Ptr) {
				msebasic_string().append(_Ptr);
				return (*this);
			}
			basic_string& append(const size_type _Count, const _Ty& _Ch) {
				msebasic_string().append(mse::as_a_size_t(_Count), _Ch);
				return (*this);
			}
			template<class _Iter, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::_mse_Is_iterator_v<_Iter> > MSE_IMPL_EIS >
			basic_string& append(const _Iter _First, const _Iter _Last) {
				msebasic_string().append(_First, _Last);
				return (*this);
			}
			basic_string& append(std::initializer_list<_Ty> _Ilist) {
				msebasic_string().append(_Ilist);
				return (*this);
			}

#ifdef MSE_HAS_CXX17
		private:
			basic_string& append_helper1(std::true_type, const basic_string& _Right) {
				return (append(static_cast<const basic_string&>(_Right)));
			}
			template<class _TParam1>
			basic_string& append_helper1(std::false_type, const _TParam1& _Right) {
				m_shptr->append(_Right);
				return (*this);
			}
		public:
			template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
			basic_string& append(const _TParam1& _Right) {
				return append_helper1(typename std::is_base_of<basic_string, _TParam1>::type(), _Right);
			}

		private:
			basic_string& append_helper1(std::true_type, const basic_string& _Right, const size_type _Roff, const size_type _Count) {
				return (append(static_cast<const basic_string&>(_Right, _Roff, _Count)));
			}
			template<class _TParam1>
			basic_string& append_helper1(std::false_type, const _TParam1& _Right, const size_type _Roff, const size_type _Count) {
				m_shptr->append(_Right, _Roff, _Count);
				return (*this);
			}
		public:
			template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
			basic_string& append(const _TParam1& _Right, const size_type _Roff, const size_type _Count = npos) {
				return append_helper1(typename std::is_base_of<basic_string, _TParam1>::type(), _Right, _Roff, _Count);
			}
#else /* MSE_HAS_CXX17 */
			template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
			basic_string& append(const _TStringSection& _X) {
				m_shptr->append(_X);
				return (*this);
			}
#endif /* MSE_HAS_CXX17 */

			basic_string& operator+=(mse::TXScopeFixedConstPointer<basic_string> xs_ptr) {
				return (append(xs_ptr));
			}
			basic_string& operator+=(const basic_string& _Right) {
				return (append(_Right));
			}
			basic_string& operator+=(const _Ty * const _Ptr) {
				return (append(_Ptr));
			}
			basic_string& operator+=(_Ty _Ch) {
				push_back(_Ch);
				return (*this);
			}
			basic_string& operator+=(std::initializer_list<_Ty> _Ilist) {
				return (append(_Ilist));
			}

#ifdef MSE_HAS_CXX17
			template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1>*/>
			basic_string & operator+=(const _TParam1& _Right) {
				return (append(_Right));
			}
#else /* MSE_HAS_CXX17 */
			template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
			basic_string& operator+=(const _TStringSection& _X) {
				return (append(_X));
			}
#endif /* MSE_HAS_CXX17 */


			basic_string& replace(const size_type _Off, const size_type _N0, mse::TXScopeFixedConstPointer<basic_string> xs_ptr) {
				msebasic_string().replace(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), (*xs_ptr).msebasic_string());
				return (*this);
			}
			basic_string& replace(const size_type _Off, const size_type _N0, const basic_string& _Right) {
				msebasic_string().replace(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _Right.msebasic_string());
				return (*this);
			}

			basic_string& replace(const size_type _Off, size_type _N0,
				mse::TXScopeFixedConstPointer<basic_string> xs_ptr, const size_type _Roff, size_type _Count = npos) {
				msebasic_string().replace(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), (*xs_ptr).msebasic_string(), mse::msev_as_a_size_t(_Roff), mse::msev_as_a_size_t(_Count));
				return (*this);
			}
			basic_string& replace(const size_type _Off, size_type _N0,
				const basic_string& _Right, const size_type _Roff, size_type _Count = npos) {
				msebasic_string().replace(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _Right.msebasic_string(), mse::msev_as_a_size_t(_Roff), mse::msev_as_a_size_t(_Count));
				return (*this);
			}

#ifdef MSE_HAS_CXX17
		private:
			basic_string& replace_helper1(std::true_type, const size_type _Off, const size_type _N0, const basic_string& _Right) {
				return (replace(_Off, _N0, _Right));
			}
			template<class _TParam1>
			basic_string& replace_helper1(std::false_type, const size_type _Off, const size_type _N0, const _TParam1& _Right) {
				return m_shptr->replace(_Off, _N0, _Right);
			}
		public:
			template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
			basic_string& replace(const size_type _Off, const size_type _N0, const _TParam1& _Right) {
				return replace_helper1(typename std::is_base_of<basic_string, _TParam1>::type(), _Off, _N0, _Right);
			}

		private:
			basic_string& replace_helper1(std::true_type, const size_type _Off, const size_type _N0, const basic_string& _Right, const size_type _Roff, const size_type _Count) {
				return (replace(_Off, _N0, _Right, _Roff, _Count));
			}
			template<class _TParam1>
			basic_string& replace_helper1(std::false_type, const size_type _Off, const size_type _N0, const _TParam1& _Right, const size_type _Roff, const size_type _Count) {
				return m_shptr->replace(_Off, _N0, _Right, _Roff, _Count);
			}
		public:
			template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
			basic_string& replace(const size_type _Off, const size_type _N0, const _TParam1& _Right, const size_type _Roff, const size_type _Count = npos) {
				return replace_helper1(typename std::is_base_of<basic_string, _TParam1>::type(), _Off, _N0, _Right, _Roff, _Count);
			}
#else /* MSE_HAS_CXX17 */
			template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
			basic_string& replace(const size_type _Off, const size_type _N0, const _TStringSection& _X) {
				msebasic_string().replace(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _X.cbegin(), _X.cend());
				return (*this);
			}
#endif /* MSE_HAS_CXX17 */

			basic_string& replace(const size_type _Off, size_type _N0, const _Ty * const _Ptr, const size_type _Count) {
				msebasic_string().replace(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _Ptr, mse::msev_as_a_size_t(_Count));
				return (*this);
			}

			basic_string& replace(const size_type _Off, const size_type _N0, const _Ty * const _Ptr) {
				return (replace(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _Ptr, size_type(_Traits::length(_Ptr))));
			}

			basic_string& replace(const size_type _Off, size_type _N0, const size_type _Count, const _Ty _Ch) {
				msebasic_string().replace(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), mse::msev_as_a_size_t(_Count), _Ch);
				return (*this);
			}

			basic_string& replace(const const_iterator _First, const const_iterator _Last, mse::TXScopeFixedConstPointer<basic_string> xs_ptr) {
				return (replace(_First - begin(), _Last - _First, xs_ptr));
			}
			basic_string& replace(const const_iterator _First, const const_iterator _Last, const basic_string& _Right) {
				return (replace(_First - begin(), _Last - _First, _Right));
			}

#if 0//_HAS_CXX17
			basic_string& replace(const const_iterator _First, const const_iterator _Last,
				const mstd::basic_string_view<_Ty, _Traits> _Right) {
				return (replace(_First - begin(), _Last - _First, _Right));
			}
#endif /* _HAS_CXX17 */

			basic_string& replace(const const_iterator _First, const const_iterator _Last,
				const _Ty * const _Ptr, const size_type _Count) {
				return (replace(_First - begin(), _Last - _First, _Ptr, _Count));
			}

			basic_string& replace(const const_iterator _First, const const_iterator _Last, const _Ty * const _Ptr) {
				return (replace(_First - begin(), _Last - _First, _Ptr));
			}

			basic_string& replace(const const_iterator _First, const const_iterator _Last, const size_type _Count, const _Ty _Ch) {
				return (replace(_First - begin(), _Last - _First, _Count, _Ch));
			}

			template<class _Iter, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value> MSE_IMPL_EIS >
			basic_string& replace(const const_iterator _First, const const_iterator _Last,
				const _Iter _First2, const _Iter _Last2) {
				replace(_First, _Last, _First2, _Last2);
				return (*this);
			}

			template<class _Iter, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value> MSE_IMPL_EIS >
			basic_string& replace(const size_type _Off, const size_type _N0, const _Iter _First2, const _Iter _Last2) {
				msebasic_string().replace(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _First2, _Last2);
				return (*this);
			}

			int compare(mse::TXScopeFixedConstPointer<basic_string> xs_ptr) const _NOEXCEPT {
				return msebasic_string().compare((*xs_ptr).msebasic_string());
			}
			int compare(const basic_string& _Right) const _NOEXCEPT {
				return msebasic_string().compare(_Right.msebasic_string());
			}
			int compare(size_type _Off, size_type _N0, mse::TXScopeFixedConstPointer<basic_string> xs_ptr) const {
				return msebasic_string().compare(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), (*xs_ptr).msebasic_string());
			}
			int compare(size_type _Off, size_type _N0, const basic_string& _Right) const {
				return msebasic_string().compare(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _Right.msebasic_string());
			}
			int compare(const size_type _Off, const size_type _N0, mse::TXScopeFixedConstPointer<basic_string> xs_ptr,
				const size_type _Roff, const size_type _Count = npos) const {
				return msebasic_string().compare(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), (*xs_ptr).msebasic_string(), _Roff, _Count);
			}
			int compare(const size_type _Off, const size_type _N0, const basic_string& _Right,
				const size_type _Roff, const size_type _Count = npos) const {
				return msebasic_string().compare(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _Right.msebasic_string(), _Roff, _Count);
			}

			int compare(const _Ty * const _Ptr) const _NOEXCEPT {
				return msebasic_string().compare(_Ptr);
			}

			int compare(const size_type _Off, const size_type _N0, const _Ty * const _Ptr) const {
				return msebasic_string().compare(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _Ptr);
			}

			int compare(const size_type _Off, const size_type _N0, const _Ty * const _Ptr,
				const size_type _Count) const {
				return msebasic_string().compare(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _Ptr, mse::msev_as_a_size_t(_Count));
			}

#ifdef MSE_HAS_CXX17
		private:
			int compare_helper1(std::true_type, const basic_string& _Right) const {
				return compare(_Right);
			}
			template<class _TParam1>
			int compare_helper1(std::false_type, const _TParam1& _Right) const {
				return msebasic_string().compare(_Right);
			}
		public:
			template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
			int compare(const _TParam1& _Right) const {
				return compare_helper1(typename std::is_base_of<basic_string, _TParam1>::type(), _Right);
			}

		private:
			int compare_helper1(std::true_type, const size_type _Off, const size_type _N0, const basic_string& _Right) const {
				return compare(_Off, _N0, _Right);
			}
			template<class _TParam1>
			int compare_helper1(std::false_type, const size_type _Off, const size_type _N0, const _TParam1& _Right) const {
				return msebasic_string().compare(_Off, _N0, _Right);
			}
		public:
			template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
			int compare(const size_type _Off, const size_type _N0, const _TParam1& _Right) const {
				return compare_helper1(typename std::is_base_of<basic_string, _TParam1>::type(), _Off, _N0, _Right);
			}

		private:
			int compare_helper1(std::true_type, const size_type _Off, const size_type _N0, const basic_string& _Right, const size_type _Roff, const size_type _Count) const {
				return compare(_Off, _N0, _Right, _Roff, _Count);
			}
			template<class _TParam1>
			int compare_helper1(std::false_type, const size_type _Off, const size_type _N0, const _TParam1& _Right, const size_type _Roff, const size_type _Count) const {
				return msebasic_string().compare(_Off, _N0, _Right, _Roff, _Count);
			}
		public:
			template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
			int compare(const size_type _Off, const size_type _N0, const _TParam1& _Right, const size_type _Roff, const size_type _Count = npos) const {
				return compare_helper1(typename std::is_base_of<basic_string, _TParam1>::type(), _Off, _N0, _Right, _Roff, _Count);
			}
#else /* MSE_HAS_CXX17 */
			template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
			int compare(const size_type _Off, const size_type _N0, const _TStringSection& _X) const {
				msebasic_string().compare(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), basic_string(_X.cbegin(), _X.cend()));
				return (*this);
			}
#endif /* MSE_HAS_CXX17 */

#ifdef MSE_HAS_CXX17
		private:
			size_type find_helper1(std::true_type, const basic_string& _Right, const size_type _Off = npos) const {
				return find(_Right, _Off);
			}
			template<class _TParam1>
			size_type find_helper1(std::false_type, const _TParam1& _Right, const size_type _Off = npos) const {
				return m_shptr->find(_Right, mse::as_a_size_t(_Off));
			}
		public:
			template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
			size_type find(const _TParam1& _Right, const size_type _Off = npos) const {
				return find_helper1(typename std::is_base_of<basic_string, _TParam1>::type(), _Right, _Off);
			}
#else /* MSE_HAS_CXX17 */
			template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
			size_type find(const _TStringSection& _X, const size_type _Off = npos) const {
				return m_shptr->find(basic_string(_X.cbegin(), _X.cend()), mse::as_a_size_t(_Off));
			}
#endif /* MSE_HAS_CXX17 */

			size_type find(const basic_string& _Right, const size_type _Off = 0) const _NOEXCEPT {
				return msebasic_string().find(_Right.msebasic_string(), _Off);
			}

			size_type find(const _Ty * const _Ptr, const size_type _Off, const size_type _Count) const _NOEXCEPT {
				return msebasic_string().find(_Ptr, _Off, _Count);
			}

			size_type find(const _Ty * const _Ptr, const size_type _Off = 0) const _NOEXCEPT {
				return msebasic_string().find(_Ptr, _Off);
			}

			size_type find(const _Ty _Ch, const size_type _Off = 0) const _NOEXCEPT {
				return msebasic_string().find(_Ch, _Off);
			}

#ifdef MSE_HAS_CXX17
		private:
			size_type rfind_helper1(std::true_type, const basic_string& _Right, const size_type _Off = npos) const {
				return rfind(_Right, _Off);
			}
			template<class _TParam1>
			size_type rfind_helper1(std::false_type, const _TParam1& _Right, const size_type _Off = npos) const {
				return m_shptr->rfind(_Right, mse::as_a_size_t(_Off));
			}
		public:
			template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
			size_type rfind(const _TParam1& _Right, const size_type _Off = npos) const {
				return rfind_helper1(typename std::is_base_of<basic_string, _TParam1>::type(), _Right, _Off);
			}
#else /* MSE_HAS_CXX17 */
			template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
			size_type rfind(const _TStringSection& _X, const size_type _Off = npos) const {
				return m_shptr->rfind(basic_string(_X.cbegin(), _X.cend()), mse::as_a_size_t(_Off));
			}
#endif /* MSE_HAS_CXX17 */

			size_type rfind(const basic_string& _Right, const size_type _Off = npos) const _NOEXCEPT {
				return msebasic_string().rfind(_Right.msebasic_string(), _Off);
			}

			size_type rfind(const _Ty * const _Ptr, const size_type _Off, const size_type _Count) const _NOEXCEPT {
				return msebasic_string().rfind(_Ptr, _Off, _Count);
			}

			size_type rfind(const _Ty * const _Ptr, const size_type _Off = npos) const _NOEXCEPT {
				return msebasic_string().rfind(_Ptr, _Off);
			}

			size_type rfind(const _Ty _Ch, const size_type _Off = npos) const _NOEXCEPT {
				return msebasic_string().rfind(_Ch, _Off);
			}

#if 0//_HAS_CXX17
			size_type find_first_of(const basic_string_view<_Ty, _Traits> _Right, const size_type _Off = 0) const _NOEXCEPT
			{	// look for one of _Right at or after _Off
				auto& _My_data = this->_Get_data();
				return (static_cast<size_type>(_Traits_find_first_of<_Traits>(_My_data._Myptr(), _My_data._Mysize, _Off,
					_Right.data(), _Right.size())));
			}
#endif /* _HAS_CXX17 */

			size_type find_first_of(const basic_string& _Right, const size_type _Off = 0) const _NOEXCEPT {
				return msebasic_string().find_first_of(_Right.msebasic_string(), _Off);
			}

			size_type find_first_of(const _Ty * const _Ptr, const size_type _Off,
				const size_type _Count) const _NOEXCEPT {
				return msebasic_string().find_first_of(_Ptr, _Off, _Count);
			}

			size_type find_first_of(const _Ty * const _Ptr, const size_type _Off = 0) const _NOEXCEPT {
				return msebasic_string().find_first_of(_Ptr, _Off);
			}

			size_type find_first_of(const _Ty _Ch, const size_type _Off = 0) const _NOEXCEPT {
				return msebasic_string().find_first_of(_Ch, _Off);
			}

#if 0//_HAS_CXX17
			size_type find_last_of(const basic_string_view<_Ty, _Traits> _Right, const size_type _Off = npos) const _NOEXCEPT
			{	// look for one of _Right before _Off
				auto& _My_data = this->_Get_data();
				return (static_cast<size_type>(_Traits_find_last_of<_Traits>(_My_data._Myptr(), _My_data._Mysize, _Off,
					_Right.data(), _Right.size())));
			}
#endif /* _HAS_CXX17 */

			size_type find_last_of(const basic_string& _Right, size_type _Off = npos) const _NOEXCEPT {
				return msebasic_string().find_last_of(_Right.msebasic_string(), _Off);
			}

			size_type find_last_of(const _Ty * const _Ptr, const size_type _Off,
				const size_type _Count) const _NOEXCEPT {
				return msebasic_string().find_last_of(_Ptr, _Off, _Count);
			}

			size_type find_last_of(const _Ty * const _Ptr, const size_type _Off = npos) const _NOEXCEPT {
				return msebasic_string().find_last_of(_Ptr, _Off);
			}

			size_type find_last_of(const _Ty _Ch, const size_type _Off = npos) const _NOEXCEPT {
				return msebasic_string().find_last_of(_Ch, _Off);
			}

#if 0//_HAS_CXX17
			size_type find_first_not_of(const basic_string_view<_Ty, _Traits> _Right, const size_type _Off = 0) const
				_NOEXCEPT
			{	// look for none of _Right at or after _Off
				auto& _My_data = this->_Get_data();
				return (static_cast<size_type>(_Traits_find_first_not_of<_Traits>(_My_data._Myptr(), _My_data._Mysize, _Off,
					_Right.data(), _Right.size())));
			}
#endif /* _HAS_CXX17 */

			size_type find_first_not_of(const basic_string& _Right, const size_type _Off = 0) const _NOEXCEPT {
				return msebasic_string().find_first_not_of(_Right.msebasic_string(), _Off);
			}

			size_type find_first_not_of(const _Ty * const _Ptr, const size_type _Off,
				const size_type _Count) const _NOEXCEPT {
				return msebasic_string().find_first_not_of(_Ptr, _Off, _Count);
			}

			size_type find_first_not_of(const _Ty * const _Ptr, size_type _Off = 0) const _NOEXCEPT {
				return msebasic_string().find_first_not_of(_Ptr, _Off);
			}

			size_type find_first_not_of(const _Ty _Ch, const size_type _Off = 0) const _NOEXCEPT {
				return msebasic_string().find_first_not_of(_Ch, _Off);
			}

#if 0//_HAS_CXX17
			size_type find_last_not_of(const basic_string_view<_Ty, _Traits> _Right, const size_type _Off = npos) const
				_NOEXCEPT
			{	// look for none of _Right before _Off
				auto& _My_data = this->_Get_data();
				return (static_cast<size_type>(_Traits_find_last_not_of<_Traits>(_My_data._Myptr(), _My_data._Mysize, _Off,
					_Right.data(), _Right.size())));
			}
#endif /* _HAS_CXX17 */

			size_type find_last_not_of(const basic_string& _Right, const size_type _Off = npos) const _NOEXCEPT {
				return msebasic_string().find_last_not_of(_Right.msebasic_string(), _Off);
			}

			size_type find_last_not_of(const _Ty * const _Ptr, const size_type _Off,
				const size_type _Count) const _NOEXCEPT {
				return msebasic_string().find_last_not_of(_Ptr, _Off, _Count);
			}

			size_type find_last_not_of(const _Ty * const _Ptr, const size_type _Off = npos) const _NOEXCEPT {
				return msebasic_string().find_last_not_of(_Ptr, _Off);
			}

			size_type find_last_not_of(const _Ty _Ch, const size_type _Off = npos) const _NOEXCEPT {
				return msebasic_string().find_last_not_of(_Ch, _Off);
			}

			template <typename _TRAIterator2>
			size_type copy(_TRAIterator2 target_iter, size_type n, size_type pos = 0) const {
				if (pos + n > (*this).size()) {
					if (pos >= (*this).size()) {
						return 0;
					}
					else {
						n = (*this).size() - pos;
					}
				}
				for (size_type i = 0; i < n; i += 1) {
					(*target_iter) = msebasic_string()[i];
					++target_iter;
				}
				return n;
			}

			basic_string substr(const size_type _Off = 0, const size_type _Count = npos) const {
				//return (basic_string(*this, _Off, _Count, get_allocator()));
				return (basic_string(msebasic_string().substr(_Off, _Count)));
			}

			static basic_string& _Myt_ref(basic_string& bs_ref) {
				return bs_ref;
			}

			template<typename _TThisPointer>
			static std::basic_istream<_Ty, _Traits>& getline(std::basic_istream<_Ty, _Traits>&& _Istr, _TThisPointer this_ptr, const _Ty _Delim) {
				return std::getline(MSE_FWD(_Istr), _Myt_ref(*this_ptr).msebasic_string(), _Delim);
			}
			template<typename _TThisPointer>
			static std::basic_istream<_Ty, _Traits>& getline(std::basic_istream<_Ty, _Traits>&& _Istr, _TThisPointer this_ptr) {
				return std::getline(MSE_FWD(_Istr), _Myt_ref(*this_ptr).msebasic_string());
			}
			template<typename _TThisPointer>
			static std::basic_istream<_Ty, _Traits>& getline(std::basic_istream<_Ty, _Traits>& _Istr, _TThisPointer this_ptr, const _Ty _Delim) {
				return std::getline(_Istr, _Myt_ref(*this_ptr).msebasic_string(), _Delim);
			}
			template<typename _TThisPointer>
			static std::basic_istream<_Ty, _Traits>& getline(std::basic_istream<_Ty, _Traits>& _Istr, _TThisPointer this_ptr) {
				return std::getline(_Istr, _Myt_ref(*this_ptr).msebasic_string());
			}

			void async_not_shareable_tag() const {}
			/* this array should be safely passable iff the element type is safely passable */
			template<class _Ty2 = _Ty, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _Ty>::value)
				&& (mse::impl::is_marked_as_passable_msemsearray<_Ty2>::value)> MSE_IMPL_EIS >
			void async_passable_tag() const {}

		private:
			static std::basic_istream<_Ty, _Traits>& in_from_stream(std::basic_istream<_Ty, _Traits>&& _Istr, basic_string& _Str) {
				return MSE_FWD(_Istr) >> _Str.msebasic_string();
			}
			static std::basic_istream<_Ty, _Traits>& in_from_stream(std::basic_istream<_Ty, _Traits>& _Istr, basic_string& _Str) {
				return _Istr >> _Str.msebasic_string();
			}
			static std::basic_ostream<_Ty, _Traits>& out_to_stream(std::basic_ostream<_Ty, _Traits>& _Ostr, const basic_string& _Str) {
				return _Ostr << _Str.msebasic_string();
			}

			const _MBS& msebasic_string() const& { return (*m_shptr); }
			//const _MBS& msebasic_string() const&& { return (*m_shptr); }
			_MBS& msebasic_string()& { return (*m_shptr); }
			_MBS&& msebasic_string()&& { return std::move(*m_shptr); }

			std::shared_ptr<_MBS> m_shptr;

			friend xscope_const_iterator;
			friend xscope_iterator;

			friend struct std::hash<basic_string>;
			template<class _Ty2, class _Traits2/* = std::char_traits<_Ty2>*/, class _A2/* = std::allocator<_Ty2> */>
			friend std::basic_istream<_Ty2, _Traits2>& impl::bs::in_from_stream(std::basic_istream<_Ty2, _Traits2>&& _Istr, basic_string<_Ty2, _Traits2, _A2>& _Str);
			template<class _Ty2, class _Traits2/* = std::char_traits<_Ty2>*/, class _A2/* = std::allocator<_Ty2> */>
			friend std::basic_istream<_Ty2, _Traits2>& impl::bs::in_from_stream(std::basic_istream<_Ty2, _Traits2>& _Istr, basic_string<_Ty2, _Traits2, _A2>& _Str);
			template<class _Ty2, class _Traits2/* = std::char_traits<_Ty2>*/, class _A2/* = std::allocator<_Ty2> */>
			friend std::basic_ostream<_Ty2, _Traits2>& impl::bs::out_to_stream(std::basic_ostream<_Ty2, _Traits2>& _Ostr, const basic_string<_Ty2, _Traits2, _A2>& _Str);

			/*
			friend void swap(_Myt& a, _Myt& b) _NOEXCEPT_OP(_NOEXCEPT_OP(a.swap(b))) { a.swap(b); }
			friend void swap(_Myt& a, _MBS& b) _NOEXCEPT_OP(_NOEXCEPT_OP(a.swap(b))) { a.swap(b); }
			friend void swap(_Myt& a, mse::nii_basic_string<_Ty, _A>& b) _NOEXCEPT_OP(_NOEXCEPT_OP(a.swap(b))) { a.swap(b); }
			friend void swap(_Myt& a, std::basic_string<_Ty, _A>& b) _NOEXCEPT_OP(_NOEXCEPT_OP(a.swap(b))) { a.swap(b); }
			friend void swap(_MBS& a, _Myt& b) _NOEXCEPT_OP(_NOEXCEPT_OP(b.swap(a))) { b.swap(a); }
			friend void swap(mse::nii_basic_string<_Ty, _A>& a, _Myt& b) _NOEXCEPT_OP(_NOEXCEPT_OP(b.swap(a))) { b.swap(a); }
			friend void swap(std::basic_string<_Ty, _A>& a, _Myt& b) _NOEXCEPT_OP(_NOEXCEPT_OP(b.swap(a))) { b.swap(a); }
			*/

			friend class mse::mstd::ns_basic_string::xscope_structure_lock_guard<_Ty, _Traits, _A>;
			friend class mse::mstd::ns_basic_string::xscope_const_structure_lock_guard<_Ty, _Traits, _A>;
		};

#ifdef MSE_HAS_CXX17
		/* deduction guides */
		template<class _Iter, class _Alloc = std::allocator<typename std::iterator_traits<_Iter>::value_type>
			, std::enable_if_t<std::conjunction_v< mse::impl::_mse_Is_iterator<_Iter>, mse::impl::_mse_Is_allocator<_Alloc> >, int> = 0>
			basic_string(_Iter, _Iter, _Alloc = _Alloc())
			->basic_string<typename std::iterator_traits<_Iter>::value_type, std::char_traits<typename std::iterator_traits<_Iter>::value_type>, _Alloc>;

		template<class _Elem, class _Traits, class _Alloc = std::allocator<_Elem>
			, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
			explicit basic_string(std::basic_string_view<_Elem, _Traits>, const _Alloc& = _Alloc())
			->basic_string<_Elem, _Traits, _Alloc>;

		template<class _Elem, class _Traits, class _Alloc = std::allocator<_Elem>
			, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
			basic_string(std::basic_string_view<_Elem, _Traits>, mse::impl::_mse_Guide_size_type_t<_Alloc>
				, mse::impl::_mse_Guide_size_type_t<_Alloc>, const _Alloc& = _Alloc())
			->basic_string<_Elem, _Traits, _Alloc>;

		template<class _TRAIterator, class _Traits, class _Alloc = std::allocator<typename std::iterator_traits<_TRAIterator>::value_type>
			, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
			explicit basic_string(mse::TStringConstSection<_TRAIterator, _Traits>, const _Alloc& = _Alloc())
			->basic_string<typename std::iterator_traits<_TRAIterator>::value_type, _Traits, _Alloc>;
		template<class _TRAIterator, class _Traits, class _Alloc = std::allocator<typename std::iterator_traits<_TRAIterator>::value_type>
			, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
			explicit basic_string(mse::TStringSection<_TRAIterator, _Traits>, const _Alloc& = _Alloc())
			->basic_string<typename std::iterator_traits<_TRAIterator>::value_type, _Traits, _Alloc>;
		template<class _TRAIterator, class _Traits, class _Alloc = std::allocator<typename std::iterator_traits<_TRAIterator>::value_type>
			, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
			explicit basic_string(mse::TXScopeStringConstSection<_TRAIterator, _Traits>, const _Alloc& = _Alloc())
			->basic_string<typename std::iterator_traits<_TRAIterator>::value_type, _Traits, _Alloc>;
		template<class _TRAIterator, class _Traits, class _Alloc = std::allocator<typename std::iterator_traits<_TRAIterator>::value_type>
			, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
			explicit basic_string(mse::TXScopeStringSection<_TRAIterator, _Traits>, const _Alloc& = _Alloc())
			->basic_string<typename std::iterator_traits<_TRAIterator>::value_type, _Traits, _Alloc>;
#endif /* MSE_HAS_CXX17 */

		namespace impl {
			namespace bs {
				template<class _Ty, class _Traits/* = std::char_traits<_Ty>*/, class _A/* = std::allocator<_Ty> */>
				std::basic_istream<_Ty, _Traits>& in_from_stream(std::basic_istream<_Ty, _Traits>&& _Istr, basic_string<_Ty, _Traits, _A>& _Str) {
					return _Str.in_from_stream(MSE_FWD(_Istr), _Str);
				}
				template<class _Ty, class _Traits/* = std::char_traits<_Ty>*/, class _A/* = std::allocator<_Ty> */>
				std::basic_istream<_Ty, _Traits>& in_from_stream(std::basic_istream<_Ty, _Traits>& _Istr, basic_string<_Ty, _Traits, _A>& _Str) {
					return _Str.in_from_stream(_Istr, _Str);
				}
				template<class _Ty, class _Traits/* = std::char_traits<_Ty>*/, class _A/* = std::allocator<_Ty> */>
				std::basic_ostream<_Ty, _Traits>& out_to_stream(std::basic_ostream<_Ty, _Traits>& _Ostr, const basic_string<_Ty, _Traits, _A>& _Str) {
					return _Str.out_to_stream(_Ostr, _Str);
				}
			}
		}

		template<class _Ty, class _Traits>
		std::basic_istream<_Ty, _Traits>& operator>>(std::basic_istream<_Ty, _Traits>&& _Istr, basic_string<_Ty, _Traits>& _Str) {
			return impl::bs::in_from_stream(MSE_FWD(_Istr), _Str);
		}
		template<class _Ty, class _Traits>
		std::basic_istream<_Ty, _Traits>& operator>>(std::basic_istream<_Ty, _Traits>& _Istr, basic_string<_Ty, _Traits>& _Str) {
			return impl::bs::in_from_stream(_Istr, _Str);
		}
		template<class _Ty, class _Traits>
		std::basic_ostream<_Ty, _Traits>& operator<<(std::basic_ostream<_Ty, _Traits>& _Ostr, const basic_string<_Ty, _Traits>& _Str) {
			return impl::bs::out_to_stream(_Ostr, _Str);
		}

		template<class _Ty, class _Traits, class _Alloc> inline bool operator!=(const basic_string<_Ty, _Traits, _Alloc>& _Left, const basic_string<_Ty, _Traits, _Alloc>& _Right) {	// test for basic_string inequality
			return (!(_Left == _Right));
		}
		template<class _Ty, class _Traits, class _Alloc> inline bool operator>(const basic_string<_Ty, _Traits, _Alloc>& _Left, const basic_string<_Ty, _Traits, _Alloc>& _Right) {	// test if _Left > _Right for basic_strings
			return (_Right < _Left);
		}
		template<class _Ty, class _Traits, class _Alloc> inline bool operator<=(const basic_string<_Ty, _Traits, _Alloc>& _Left, const basic_string<_Ty, _Traits, _Alloc>& _Right) {	// test if _Left <= _Right for basic_strings
			return (!(_Right < _Left));
		}
		template<class _Ty, class _Traits, class _Alloc> inline bool operator>=(const basic_string<_Ty, _Traits, _Alloc>& _Left, const basic_string<_Ty, _Traits, _Alloc>& _Right) {	// test if _Left >= _Right for basic_strings
			return (!(_Left < _Right));
		}

		template<class _Elem, class _Traits, class _Alloc>
		inline basic_string<_Elem, _Traits, _Alloc> operator+(const basic_string<_Elem, _Traits, _Alloc>& _Left,
			const basic_string<_Elem, _Traits, _Alloc>& _Right) {	// return string + string
			basic_string<_Elem, _Traits, _Alloc> _Ans;
			_Ans.reserve(_Left.size() + _Right.size());
			_Ans += _Left;
			_Ans += _Right;
			return (_Ans);
		}
		template<class _Elem, class _Traits, class _Alloc>
		inline basic_string<_Elem, _Traits, _Alloc> operator+(const _Elem * const _Left,
			const basic_string<_Elem, _Traits, _Alloc>& _Right) {	// return NTCTS + string
			using _String_type = basic_string<_Elem, _Traits, _Alloc>;
			using _Size_type = typename _String_type::size_type;
			_String_type _Ans;
			_Ans.reserve(_Size_type(_Traits::length(_Left) + _Right.size()));
			_Ans += _Left;
			_Ans += _Right;
			return (_Ans);
		}
		template<class _Elem, class _Traits, class _Alloc>
		inline basic_string<_Elem, _Traits, _Alloc> operator+(const _Elem _Left,
			const basic_string<_Elem, _Traits, _Alloc>& _Right) {	// return character + string
			basic_string<_Elem, _Traits, _Alloc> _Ans;
			_Ans.reserve(1 + _Right.size());
			_Ans += _Left;
			_Ans += _Right;
			return (_Ans);
		}
		template<class _Elem, class _Traits, class _Alloc>
		inline basic_string<_Elem, _Traits, _Alloc> operator+(const basic_string<_Elem, _Traits, _Alloc>& _Left,
			const _Elem * const _Right) {	// return string + NTCTS
			using _String_type = basic_string<_Elem, _Traits, _Alloc>;
			using _Size_type = typename _String_type::size_type;
			_String_type _Ans;
			_Ans.reserve(_Size_type(_Left.size() + _Traits::length(_Right)));
			_Ans += _Left;
			_Ans += _Right;
			return (_Ans);
		}
		template<class _Elem, class _Traits, class _Alloc>
		inline basic_string<_Elem, _Traits, _Alloc> operator+(const basic_string<_Elem, _Traits, _Alloc>& _Left,
			const _Elem _Right) {	// return string + character
			basic_string<_Elem, _Traits, _Alloc> _Ans;
			_Ans.reserve(_Left.size() + 1);
			_Ans += _Left;
			_Ans += _Right;
			return (_Ans);
		}
		template<class _Elem, class _Traits, class _Alloc>
		inline basic_string<_Elem, _Traits, _Alloc> operator+(const basic_string<_Elem, _Traits, _Alloc>& _Left,
			basic_string<_Elem, _Traits, _Alloc>&& _Right) {	// return string + string
			return (_STD move(_Right.insert(0, _Left)));
		}
		template<class _Elem, class _Traits, class _Alloc>
		inline basic_string<_Elem, _Traits, _Alloc> operator+(basic_string<_Elem, _Traits, _Alloc>&& _Left,
			const basic_string<_Elem, _Traits, _Alloc>& _Right) {	// return string + string
			return (_STD move(_Left.append(_Right)));
		}
		template<class _Elem, class _Traits, class _Alloc>
		inline basic_string<_Elem, _Traits, _Alloc> operator+(basic_string<_Elem, _Traits, _Alloc>&& _Left,
			basic_string<_Elem, _Traits, _Alloc>&& _Right) {	// return string + string
			if (_Right.size() <= _Left.capacity() - _Left.size()
				|| _Right.capacity() - _Right.size() < _Left.size())
				return (_STD move(_Left.append(_Right)));
			else
				return (_STD move(_Right.insert(0, _Left)));
		}
		template<class _Elem, class _Traits, class _Alloc>
		inline basic_string<_Elem, _Traits, _Alloc> operator+(const _Elem * const _Left,
			basic_string<_Elem, _Traits, _Alloc>&& _Right) {
			return (_STD move(_Right.insert(0, _Left)));
		}
		template<class _Elem, class _Traits, class _Alloc>
		inline basic_string<_Elem, _Traits, _Alloc> operator+(const _Elem _Left,
			basic_string<_Elem, _Traits, _Alloc>&& _Right) {
			using size_type = typename std::allocator_traits<_Alloc>::size_type;
			return (_STD move(_Right.insert(static_cast<size_type>(0), static_cast<size_type>(1), _Left)));
		}
		template<class _Elem, class _Traits, class _Alloc>
		inline basic_string<_Elem, _Traits, _Alloc> operator+(basic_string<_Elem, _Traits, _Alloc>&& _Left,
			const _Elem * const _Right) {
			return (_STD move(_Left.append(_Right)));
		}
		template<class _Elem, class _Traits, class _Alloc>
		inline basic_string<_Elem, _Traits, _Alloc> operator+(basic_string<_Elem, _Traits, _Alloc>&& _Left,
			const _Elem _Right) {	// return string + character
			_Left.push_back(_Right);
			return (_STD move(_Left));
		}

		namespace ns_basic_string {

			/* For each (scope) basic_string instance, only one instance of xscope_structure_lock_guard may exist at any one
			time. While an instance of xscope_structure_lock_guard exists it ensures that direct (scope) pointers to
			individual elements in the basic_string do not become invalid by preventing any operation that might resize the basic_string
			or increase its capacity. Any attempt to execute such an operation would result in an exception. */
			template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty> >
			class xscope_structure_lock_guard : public mse::us::impl::Txscope_structure_lock_guard_of_wrapper<basic_string<_Ty, _Traits, _A>, typename mse::us::ns_msebasic_string::xscope_structure_lock_guard<mse::us::msebasic_string<_Ty, _Traits, _A> > > {
			public:
				typedef mse::us::impl::Txscope_structure_lock_guard_of_wrapper<basic_string<_Ty, _Traits, _A>, typename mse::us::ns_msebasic_string::xscope_structure_lock_guard<mse::us::msebasic_string<_Ty, _Traits, _A> > > base_class;
				using base_class::base_class;

				xscope_structure_lock_guard(const mse::TXScopeObjFixedPointer<basic_string<_Ty, _Traits, _A> >& owner_ptr)
					: base_class(owner_ptr, mse::us::unsafe_make_xscope_pointer_to(*((*owner_ptr).m_shptr))) {}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
				xscope_structure_lock_guard(const mse::TXScopeFixedPointer<basic_string<_Ty, _Traits, _A> >& owner_ptr)
					: base_class(owner_ptr, mse::us::unsafe_make_xscope_pointer_to(*((*owner_ptr).m_shptr))) {}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

			private:
				MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

				friend class xscope_const_structure_lock_guard<_Ty, _Traits, _A>;
			};
			template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty> >
			class xscope_const_structure_lock_guard : public mse::us::impl::Txscope_const_structure_lock_guard_of_wrapper<basic_string<_Ty, _Traits, _A>, typename mse::us::ns_msebasic_string::xscope_const_structure_lock_guard<mse::us::msebasic_string<_Ty, _Traits, _A> > > {
			public:
				typedef mse::us::impl::Txscope_const_structure_lock_guard_of_wrapper<basic_string<_Ty, _Traits, _A>, typename mse::us::ns_msebasic_string::xscope_const_structure_lock_guard<mse::us::msebasic_string<_Ty, _Traits, _A> > > base_class;
				using base_class::base_class;

				xscope_const_structure_lock_guard(const mse::TXScopeObjFixedConstPointer<basic_string<_Ty, _Traits, _A> >& owner_ptr)
					: base_class(owner_ptr, mse::us::unsafe_make_xscope_const_pointer_to(*((*owner_ptr).m_shptr))) {}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
				xscope_const_structure_lock_guard(const mse::TXScopeFixedConstPointer<basic_string<_Ty, _Traits, _A> >& owner_ptr)
					: base_class(owner_ptr, mse::us::unsafe_make_xscope_const_pointer_to(*((*owner_ptr).m_shptr))) {}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

			private:
				MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
			};

		}

		/* For each (scope) basic_string instance, only one instance of xscope_structure_lock_guard may exist at any one
		time. While an instance of xscope_structure_lock_guard exists it ensures that direct (scope) pointers to
		individual elements in the basic_string do not become invalid by preventing any operation that might resize the basic_string
		or increase its capacity. Any attempt to execute such an operation would result in an exception. */
		template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty> >
		auto make_xscope_structure_lock_guard(const mse::TXScopeObjFixedPointer<basic_string<_Ty, _Traits, _A> >& owner_ptr) -> decltype(ns_basic_string::xscope_structure_lock_guard<_Ty, _Traits, _A>(owner_ptr)) {
			return ns_basic_string::xscope_structure_lock_guard<_Ty, _Traits, _A>(owner_ptr);
		}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty> >
		auto make_xscope_structure_lock_guard(const mse::TXScopeFixedPointer<basic_string<_Ty, _Traits, _A> >& owner_ptr) -> decltype(ns_basic_string::xscope_structure_lock_guard<_Ty, _Traits, _A>(owner_ptr)) {
			return ns_basic_string::xscope_structure_lock_guard<_Ty, _Traits, _A>(owner_ptr);
		}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty> >
		auto make_xscope_structure_lock_guard(const mse::TXScopeObjFixedConstPointer<basic_string<_Ty, _Traits, _A> >& owner_ptr) -> decltype(ns_basic_string::xscope_const_structure_lock_guard<_Ty, _Traits, _A>(owner_ptr)) {
			return ns_basic_string::xscope_const_structure_lock_guard<_Ty, _Traits, _A>(owner_ptr);
		}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty> >
		auto make_xscope_structure_lock_guard(const mse::TXScopeFixedConstPointer<basic_string<_Ty, _Traits, _A> >& owner_ptr) -> decltype(ns_basic_string::xscope_const_structure_lock_guard<_Ty, _Traits, _A>(owner_ptr)) {
			return ns_basic_string::xscope_const_structure_lock_guard<_Ty, _Traits, _A>(owner_ptr);
		}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	}
}

namespace std {

	template<class _Elem, class _Traits, class _Alloc>
	struct hash<mse::mstd::basic_string<_Elem, _Traits, _Alloc> > {	// hash functor for mse::mstd::basic_string
		typedef typename mse::mstd::basic_string<_Elem, _Traits, _Alloc>::_MBS base_string_t;
		using argument_type = mse::mstd::basic_string<_Elem, _Traits, _Alloc>;
		using result_type = size_t;

		size_t operator()(const mse::mstd::basic_string<_Elem, _Traits, _Alloc>& _Keyval) const _NOEXCEPT {
			auto retval = m_bs_hash(_Keyval.msebasic_string());
			return retval;
		}

		hash<base_string_t> m_bs_hash;
	};

	template<class _Elem, class _Traits, class _Alloc>
	inline basic_istream<_Elem, _Traits>& getline(basic_istream<_Elem, _Traits>&& _Istr,
		mse::mstd::basic_string<_Elem, _Traits, _Alloc>& _Str, const _Elem _Delim) {	// get characters into string, discard delimiter
		return _Str.getline(MSE_FWD(_Istr), &_Str, _Delim);
	}
	template<class _Elem, class _Traits, class _Alloc>
	inline basic_istream<_Elem, _Traits>& getline(basic_istream<_Elem, _Traits>&& _Istr,
		mse::mstd::basic_string<_Elem, _Traits, _Alloc>& _Str) {	// get characters into string, discard newline
		return _Str.getline(MSE_FWD(_Istr), &_Str);
	}
	template<class _Elem, class _Traits, class _Alloc>
	inline basic_istream<_Elem, _Traits>& getline(basic_istream<_Elem, _Traits>& _Istr,
		mse::mstd::basic_string<_Elem, _Traits, _Alloc>& _Str, const _Elem _Delim) {	// get characters into string, discard delimiter
		return _Str.getline(_Istr, &_Str, _Delim);
	}
	template<class _Elem, class _Traits, class _Alloc>
	inline basic_istream<_Elem, _Traits>& getline(basic_istream<_Elem, _Traits>& _Istr,
		mse::mstd::basic_string<_Elem, _Traits, _Alloc>& _Str) {	// get characters into string, discard newline
		return _Str.getline(_Istr, &_Str);
	}


	template<class _Ty, class _Traits, class _A>
	void swap(mse::mstd::basic_string<_Ty, _Traits, _A>& _Left, mse::mstd::basic_string<_Ty, _Traits, _A>& _Right) _NOEXCEPT_OP(_NOEXCEPT_OP(_Left.swap(_Right)))
	{	// swap basic_strings
		return (_Left.swap(_Right));
	}
	template<class _Ty, class _Traits, class _A, class _TStateMutex, template<typename> class _TTXScopeConstIterator/*, class = enable_if_t<_Is_swappable<_Ty>::value>*/>
	void swap(mse::mstd::basic_string<_Ty, _Traits, _A>& _Left, mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>& _Right) _NOEXCEPT_OP(_NOEXCEPT_OP(_Left.swap(_Right)))
	{	// swap basic_strings
		return (_Left.swap(_Right));
	}
	template<class _Ty, class _Traits, class _A, class _TStateMutex/*, class = enable_if_t<_Is_swappable<_Ty>::value>*/>
	void swap(mse::mstd::basic_string<_Ty, _Traits, _A>& _Left, mse::us::msebasic_string<_Ty, _Traits, _A, _TStateMutex>& _Right) _NOEXCEPT_OP(_NOEXCEPT_OP(_Left.swap(_Right)))
	{	// swap basic_strings
		return (_Left.swap(_Right));
	}
	template<class _Ty, class _Traits, class _A/*, class = enable_if_t<_Is_swappable<_Ty>::value>*/>
	void swap(mse::mstd::basic_string<_Ty, _Traits, _A>& _Left, std::basic_string<_Ty, _Traits, _A>& _Right) _NOEXCEPT_OP(_NOEXCEPT_OP(_Left.swap(_Right)))
	{	// swap basic_strings
		return (_Left.swap(_Right));
	}

	template<class _Ty, class _Traits, class _A, class _TStateMutex, template<typename> class _TTXScopeConstIterator/*, class = enable_if_t<_Is_swappable<_Ty>::value>*/>
	void swap(mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>& _Left, mse::mstd::basic_string<_Ty, _Traits, _A>& _Right) _NOEXCEPT_OP(_NOEXCEPT_OP(_Right.swap(_Left)))
	{	// swap basic_strings
		return (_Right.swap(_Left));
	}
	template<class _Ty, class _Traits, class _A, class _TStateMutex/*, class = enable_if_t<_Is_swappable<_Ty>::value>*/>
	void swap(mse::us::msebasic_string<_Ty, _Traits, _A, _TStateMutex>& _Left, mse::mstd::basic_string<_Ty, _Traits, _A>& _Right) _NOEXCEPT_OP(_NOEXCEPT_OP(_Right.swap(_Left)))
	{	// swap basic_strings
		return (_Right.swap(_Left));
	}
	template<class _Ty, class _Traits, class _A/*, class = enable_if_t<_Is_swappable<_Ty>::value>*/>
	void swap(std::basic_string<_Ty, _Traits, _A>& _Left, mse::mstd::basic_string<_Ty, _Traits, _A>& _Right) _NOEXCEPT_OP(_NOEXCEPT_OP(_Right.swap(_Left)))
	{	// swap basic_strings
		return (_Right.swap(_Left));
	}
}

namespace mse {
	namespace mstd {

#endif /*MSE_MSTDSTRING_DISABLED*/

		using string = basic_string<char>;
		using wstring = basic_string<wchar_t>;
		using u16string = basic_string<char16_t>;
		using u32string = basic_string<char32_t>;

		inline namespace literals {
			inline namespace string_literals {
				inline mse::mstd::basic_string<char> operator "" _mstds(const char *_str, size_t _len) _NOEXCEPT {
					return mse::mstd::basic_string<char>(_str, _len);
				}
				inline mse::mstd::basic_string<wchar_t> operator "" _mstds(const wchar_t *_str, size_t _len) _NOEXCEPT {
					return mse::mstd::basic_string<wchar_t>(_str, _len);
				}
				inline mse::mstd::basic_string<char16_t> operator "" _mstds(const char16_t *_str, size_t _len) _NOEXCEPT {
					return mse::mstd::basic_string<char16_t>(_str, _len);
				}
				inline mse::mstd::basic_string<char32_t> operator "" _mstds(const char32_t *_str, size_t _len) _NOEXCEPT {
					return mse::mstd::basic_string<char32_t>(_str, _len);
				}
			}
		}
	}
}

#ifndef MSE_PUSH_MACRO_NOT_SUPPORTED
#pragma pop_macro("MSE_THROW")
#pragma pop_macro("_STD")
#pragma pop_macro("_NOEXCEPT")
#pragma pop_macro("_NOEXCEPT_OP")
#endif // !MSE_PUSH_MACRO_NOT_SUPPORTED

#ifdef _MSC_VER
#pragma warning( pop )  
#endif /*_MSC_VER*/

#endif /*ndef MSEMSTDSTRING_H*/
