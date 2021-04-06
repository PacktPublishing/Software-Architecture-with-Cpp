
// Copyright (c) 2015 Noah Lopez
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef MSEMSTDARRAY_H
#define MSEMSTDARRAY_H

#include <memory>
#include <type_traits>
#include <stdexcept>
#include "msemsearray.h"
#ifndef MSE_MSTDARRAY_DISABLED
#include "mseregistered.h"
#endif /*MSE_MSTDARRAY_DISABLED*/

#ifdef _MSC_VER
#pragma warning( push )  
#pragma warning( disable : 4100 4456 4189 )
#endif /*_MSC_VER*/

#ifndef MSE_PUSH_MACRO_NOT_SUPPORTED
#pragma push_macro("MSE_THROW")
#pragma push_macro("_STD")
#pragma push_macro("_XSTD")
#pragma push_macro("_NOEXCEPT")
#pragma push_macro("_NOEXCEPT_OP")
#pragma push_macro("_CONST_FUN")
#endif // !MSE_PUSH_MACRO_NOT_SUPPORTED

#ifdef MSE_CUSTOM_THROW_DEFINITION
#define MSE_THROW(x) MSE_CUSTOM_THROW_DEFINITION(x)
#else // MSE_CUSTOM_THROW_DEFINITION
#define MSE_THROW(x) throw(x)
#endif // MSE_CUSTOM_THROW_DEFINITION

#ifndef _STD
#define _STD std::
#endif /*_STD*/

#ifndef _XSTD
#define _XSTD ::std::
#endif /*_XSTD*/

#ifndef _NOEXCEPT
#define _NOEXCEPT
#endif /*_NOEXCEPT*/

#ifndef _NOEXCEPT_OP
#define _NOEXCEPT_OP(x)	noexcept(x)
#endif /*_NOEXCEPT_OP*/

#ifndef _CONST_FUN
#define _CONST_FUN  constexpr
#endif /*_CONST_FUN*/

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-function"
#else /*__clang__*/
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#endif /*__GNUC__*/
#endif /*__clang__*/

namespace mse {

	namespace mstd {

#ifdef MSE_MSTDARRAY_DISABLED
		template<class _Ty, size_t _Size > using array = std::array<_Ty, _Size>;

		template<class _TArray> using xscope_array_const_iterator = typename _TArray::const_iterator;
		template<class _TArray> using xscope_array_iterator = typename _TArray::iterator;

		template<class _TArray>
		xscope_array_const_iterator<_TArray> make_xscope_const_iterator(const mse::TXScopeObjFixedConstPointer<_TArray>& owner_ptr) {
			return (*owner_ptr).cbegin();
		}
		template<class _TArray>
		xscope_array_const_iterator<_TArray> make_xscope_const_iterator(const mse::TXScopeObjFixedPointer<_TArray>& owner_ptr) {
			return (*owner_ptr).cbegin();
		}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _TArray>
		xscope_array_const_iterator<_TArray> make_xscope_const_iterator(const mse::TXScopeFixedConstPointer<_TArray>& owner_ptr) {
			return (*owner_ptr).cbegin();
		}
		template<class _TArray>
		xscope_array_const_iterator<_TArray> make_xscope_const_iterator(const mse::TXScopeFixedPointer<_TArray>& owner_ptr) {
			return (*owner_ptr).cbegin();
		}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

		template<class _TArray>
		xscope_array_iterator<_TArray> make_xscope_iterator(const mse::TXScopeObjFixedPointer<_TArray>& owner_ptr) {
			return (*owner_ptr).begin();
		}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _TArray>
		xscope_array_iterator<_TArray> make_xscope_iterator(const mse::TXScopeFixedPointer<_TArray>& owner_ptr) {
			return (*owner_ptr).begin();
		}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
	}

	template<class _Ty, size_t _Size>
	auto xscope_pointer(const typename std::array<_Ty, _Size>::iterator& iter_cref) {
		return mse::us::unsafe_make_xscope_pointer_to(*iter_cref);
	}
	template<class _Ty, size_t _Size>
	auto xscope_pointer(const typename std::array<_Ty, _Size>::const_iterator& iter_cref) {
		return mse::us::unsafe_make_xscope_const_pointer_to(*iter_cref);
	}
	template<class _Ty, size_t _Size>
	auto xscope_const_pointer(const typename std::array<_Ty, _Size>::iterator& iter_cref) {
		return mse::us::unsafe_make_xscope_const_pointer_to(*iter_cref);
	}
	template<class _Ty, size_t _Size>
	auto xscope_const_pointer(const typename std::array<_Ty, _Size>::const_iterator& iter_cref) {
		return mse::us::unsafe_make_xscope_const_pointer_to(*iter_cref);
	}

	namespace mstd {

#else /*MSE_MSTDARRAY_DISABLED*/

		class mstdarray_range_error : public std::range_error { public:
			using std::range_error::range_error;
		};

		template<class _Ty, size_t _Size >
		class array;
	}
}

namespace std {
	template<size_t _Idx, class _Tz, size_t _Size2>
	_CONST_FUN _Tz& get(mse::mstd::array<_Tz, _Size2>& _Arr) _NOEXCEPT;
	template<size_t _Idx, class _Tz, size_t _Size2>
	_CONST_FUN const _Tz& get(const mse::mstd::array<_Tz, _Size2>& _Arr) _NOEXCEPT;
	template<size_t _Idx, class _Tz, size_t _Size2>
	_CONST_FUN _Tz&& get(mse::mstd::array<_Tz, _Size2>&& _Arr) _NOEXCEPT;
}

namespace mse {
	namespace mstd {

		/* Following are some template (iterator) classes that, organizationally, should be members of mstd::array<>. (And they
		used to be.) However, being a member of mstd::array<> makes them "dependent types", and dependent types do not participate
		in automatic template parameter type deduction. So we had to haul them here outside of mstd::array<>. */

		template<class _Ty, size_t _Size>
		class Tarray_xscope_iterator;

		template<class _Ty, size_t _Size>
		class Tarray_xscope_const_iterator : public mse::TFriendlyAugmentedRAConstIterator<mse::TXScopeCSSSStrongRAConstIterator<mse::TXScopeFixedConstPointer<array<_Ty, _Size> > > >
			/*, public mse::us::impl::StrongPointerAsyncNotShareableAndNotPassableTagBase*/ {
		public:
			typedef mse::TFriendlyAugmentedRAConstIterator<mse::TXScopeCSSSStrongRAConstIterator<mse::TXScopeFixedConstPointer<array<_Ty, _Size> > > > base_class;
			MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(Tarray_xscope_const_iterator, base_class);

			MSE_USING_ASSIGNMENT_OPERATOR(base_class);
			auto& operator=(Tarray_xscope_const_iterator&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); }
			auto& operator=(const Tarray_xscope_const_iterator& _X) { base_class::operator=(_X); return (*this); }

			Tarray_xscope_const_iterator& operator ++() { base_class::operator ++(); return (*this); }
			Tarray_xscope_const_iterator operator++(int) { Tarray_xscope_const_iterator _Tmp = *this; base_class::operator++(); return (_Tmp); }
			Tarray_xscope_const_iterator& operator --() { base_class::operator --(); return (*this); }
			Tarray_xscope_const_iterator operator--(int) { Tarray_xscope_const_iterator _Tmp = *this; base_class::operator--(); return (_Tmp); }

			Tarray_xscope_const_iterator& operator +=(difference_type n) { base_class::operator +=(n); return (*this); }
			Tarray_xscope_const_iterator& operator -=(difference_type n) { base_class::operator -=(n); return (*this); }
			Tarray_xscope_const_iterator operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
			Tarray_xscope_const_iterator operator-(difference_type n) const { return ((*this) + (-n)); }
			difference_type operator-(const Tarray_xscope_const_iterator& _Right_cref) const { return base_class::operator-(_Right_cref); }

			void set_to_const_item_pointer(const Tarray_xscope_const_iterator& _Right_cref) { base_class::set_to_const_item_pointer(_Right_cref); }

			MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(base_class);
			void xscope_iterator_type_tag() const {}

		private:
			MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

			friend class array<_Ty, _Size>;
			template<class _Ty2, size_t _Size2>
			friend class Tarray_xscope_iterator;
		};
		template<class _Ty, size_t _Size>
		class Tarray_xscope_iterator : public mse::TFriendlyAugmentedRAIterator<mse::TXScopeCSSSStrongRAIterator<mse::TXScopeFixedPointer<array<_Ty, _Size> > > >
			/*, public mse::us::impl::XScopeContainsNonOwningScopeReferenceTagBase, public mse::us::impl::StrongPointerAsyncNotShareableAndNotPassableTagBase*/ {
		public:
			typedef mse::TFriendlyAugmentedRAIterator<mse::TXScopeCSSSStrongRAIterator<mse::TXScopeFixedPointer<array<_Ty, _Size> > > > base_class;
			MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(Tarray_xscope_iterator, base_class);

			MSE_USING_ASSIGNMENT_OPERATOR(base_class);
			auto& operator=(Tarray_xscope_iterator&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); }
			auto& operator=(const Tarray_xscope_iterator& _X) { base_class::operator=(_X); return (*this); }

			Tarray_xscope_iterator& operator ++() { base_class::operator ++(); return (*this); }
			Tarray_xscope_iterator operator++(int) { Tarray_xscope_iterator _Tmp = *this; base_class::operator++(); return (_Tmp); }
			Tarray_xscope_iterator& operator --() { base_class::operator --(); return (*this); }
			Tarray_xscope_iterator operator--(int) { Tarray_xscope_iterator _Tmp = *this; base_class::operator--(); return (_Tmp); }

			Tarray_xscope_iterator& operator +=(difference_type n) { base_class::operator +=(n); return (*this); }
			Tarray_xscope_iterator& operator -=(difference_type n) { base_class::operator -=(n); return (*this); }
			Tarray_xscope_iterator operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
			Tarray_xscope_iterator operator-(difference_type n) const { return ((*this) + (-n)); }
			difference_type operator-(const Tarray_xscope_iterator& _Right_cref) const { return base_class::operator-(_Right_cref); }

			void set_to_item_pointer(const Tarray_xscope_iterator& _Right_cref) { base_class::set_to_item_pointer(_Right_cref); }

			MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(base_class);
			void xscope_iterator_type_tag() const {}

		private:
			MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

			friend class array<_Ty, _Size>;
		};


		template<class _Ty, size_t _Size >
		class array : private mse::impl::TOpaqueWrapper<mse::TRegisteredObj<mse::nii_array<_Ty, _Size> > >, public mse::us::impl::AsyncNotShareableTagBase, public mse::us::impl::ContiguousSequenceStaticStructureContainerTagBase {
		public:
			/* We (privately) inherit the underlying data type rather than make it a data member to ensure it's the "first" component in the structure.*/
			typedef mse::impl::TOpaqueWrapper<mse::TRegisteredObj<mse::nii_array<_Ty, _Size> > > base_class;
			typedef mse::TRegisteredObj<mse::nii_array<_Ty, _Size> > _RMA;
			typedef array _Myt;
			typedef mse::nii_array<_Ty, _Size> _MA;

			MSE_INHERITED_RANDOM_ACCESS_MEMBER_TYPE_DECLARATIONS(_MA);

		private:
			const _RMA& contained_array() const& { return (*this).value(); }
			//const _RMA& contained_array() const&& { return (*this).value(); }
			_RMA& contained_array()& { return (*this).value(); }
			_RMA&& contained_array()&& { return std::move(*this).value(); }

		public:
			operator _MA() const& { return as_nii_array(); }
			operator _MA()&& { return std::forward<_MA>(as_nii_array()); }
			operator std::array<_Ty, _Size>() const& { return as_nii_array(); }
			operator std::array<_Ty, _Size>()&& { return std::forward<std::array<_Ty, _Size> >(as_nii_array()); }

			array() {}
			array(_MA&& _X) : base_class(MSE_FWD(_X)) {}
			array(const _MA& _X) : base_class(_X) {}
			array(_Myt&& _X) : base_class(MSE_FWD(_X).as_nii_array()) {}
			array(const _Myt& _X) : base_class(_X.as_nii_array()) {}
			array(std::array<_Ty, _Size>&& _X) : base_class(MSE_FWD(_X)) {}
			array(const std::array<_Ty, _Size>& _X) : base_class(_X) {}
			//array(_XSTD initializer_list<typename _MA::base_class::value_type> _Ilist) : base_class(_Ilist) {}
			static constexpr std::array<_Ty, _Size> std_array_initial_value(std::true_type, _XSTD initializer_list<_Ty> _Ilist) {
				/* _Ty is default constructible. */
				assert(_Ilist.size() <= _Size);
				return mse::us::impl::array_helper::default_constructible_array_helper_type<_Ty, _Size>::std_array_initialized_with_range(_Ilist.begin(), _Ilist.end());
			}
			static constexpr std::array<_Ty, _Size> std_array_initial_value(std::false_type, _XSTD initializer_list<_Ty> _Ilist) {
				/* _Ty is not default constructible. */
				assert(_Ilist.size() == _Size);
				return mse::us::impl::array_helper::not_default_constructible_array_helper_type<_Ty, _Size>::std_array_initialized_with_range(_Ilist.begin(), _Ilist.end());
			}
			constexpr array(_XSTD initializer_list<_Ty> _Ilist) : base_class(_MA{ std_array_initial_value(std::is_default_constructible<_Ty>(), _Ilist) }) {
				/* std::array<> is an "aggregate type" (basically a POD struct with no base class, constructors or private
				data members (details here: http://en.cppreference.com/w/cpp/language/aggregate_initialization)). As such,
				support for construction from initializer list is automatically generated by the compiler. Specifically,
				aggregate types support "aggregate initialization". But since mstd::array has a member with an explicitly
				defined constructor (or at least I think that's why), it is not an aggregate type and therefore doesn't
				qualify to have support for "aggregate initialization" automatically generated by the compiler. So we try
				to emulate it. */
			}

			//_Myt& operator=(_MA&& _X) { contained_array().operator=(MSE_FWD(_X)); return (*this); }
			//_Myt& operator=(const _MA& _X) { contained_array().operator=(_X); return (*this); }
			_Myt& operator=(_Myt&& _X) { contained_array().operator=(MSE_FWD(_X).as_nii_array()); return (*this); }
			_Myt& operator=(const _Myt& _X) { contained_array().operator=(_X.as_nii_array()); return (*this); }
			typename _MA::const_reference operator[](size_type _P) const { return contained_array().operator[](_P); }
			typename _MA::reference operator[](size_type _P) { return contained_array().operator[](_P); }
			void fill(const _Ty& _Value) { contained_array().fill(_Value); }
			void swap(_MA& _X) { contained_array().swap(_X); }
			void swap(_Myt& _X) { contained_array().swap(_X.as_nii_array()); }

			size_type size() const _NOEXCEPT { return contained_array().size(); }
			size_type max_size() const _NOEXCEPT { return contained_array().max_size(); }
			_CONST_FUN bool empty() const _NOEXCEPT { return contained_array().empty(); }
			typename _MA::const_reference at(size_type _Pos) const { return contained_array().at(_Pos); }
			typename _MA::reference at(size_type _Pos) { return contained_array().at(_Pos); }
			typename _MA::reference front() { return contained_array().front(); }
			_CONST_FUN typename _MA::const_reference front() const { return contained_array().front(); }
			typename _MA::reference back() { return contained_array().back(); }
			_CONST_FUN typename _MA::const_reference back() const { return contained_array().back(); }

			/* Try to avoid using these whenever possible. */
			value_type *data() _NOEXCEPT {	// return pointer to mutable data array
				return contained_array().data();
			}
			const value_type *data() const _NOEXCEPT {	// return pointer to nonmutable data array
				return contained_array().data();
			}


			class reg_ss_iterator_type : public _MA::template Tss_iterator_type<mse::TRegisteredPointer<_MA>> {
			public:
				typedef typename _MA::template Tss_iterator_type<mse::TRegisteredPointer<_MA>> base_class;
				MSE_USING(reg_ss_iterator_type, base_class);
				reg_ss_iterator_type(const base_class& src) : base_class(src) {}

				friend class reg_ss_const_iterator_type;
			};
			class reg_ss_const_iterator_type : public _MA::template Tss_const_iterator_type<mse::TRegisteredConstPointer<_MA>> {
			public:
				typedef typename _MA::template Tss_const_iterator_type<mse::TRegisteredConstPointer<_MA>> base_class;
				MSE_USING(reg_ss_const_iterator_type, base_class);
				reg_ss_const_iterator_type(const base_class& src) : base_class(src) {}
				reg_ss_const_iterator_type(const reg_ss_iterator_type& src) {
					(*this).m_owner_cptr = src.m_owner_ptr;
					(*this).m_index = src.m_index;
				}
			};

			typedef Tarray_xscope_const_iterator<_Ty, _Size> xscope_const_iterator;
			typedef Tarray_xscope_iterator<_Ty, _Size> xscope_iterator;

			class const_iterator : public _MA::na_const_iterator_base {
			public:
				typedef typename _MA::na_const_iterator_base base_class;
				MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

				const_iterator() {}
				const_iterator(const const_iterator& src_cref) = default;
				~const_iterator() {}
				const reg_ss_const_iterator_type& nii_array_reg_ss_const_iterator_type() const {
					//if (!contained_array()_regcptr) { MSE_THROW(mstdarray_range_error("attempt to use an invalid iterator - mse::mstd::array<>::const_iterator")); }
					return m_reg_ss_const_iterator;
				}
				reg_ss_const_iterator_type& nii_array_reg_ss_const_iterator_type() {
					//if (!contained_array()_regcptr) { MSE_THROW(mstdarray_range_error("attempt to use an invalid iterator - mse::mstd::array<>::const_iterator")); }
					return m_reg_ss_const_iterator;
				}
				const reg_ss_const_iterator_type& mvssci() const { return nii_array_reg_ss_const_iterator_type(); }
				reg_ss_const_iterator_type& mvssci() { return nii_array_reg_ss_const_iterator_type(); }

				void reset() { nii_array_reg_ss_const_iterator_type().reset(); }
				bool points_to_an_item() const { return nii_array_reg_ss_const_iterator_type().points_to_an_item(); }
				bool points_to_end_marker() const { return nii_array_reg_ss_const_iterator_type().points_to_end_marker(); }
				bool points_to_beginning() const { return nii_array_reg_ss_const_iterator_type().points_to_beginning(); }
				/* has_next_item_or_end_marker() is just an alias for points_to_an_item(). */
				bool has_next_item_or_end_marker() const { return nii_array_reg_ss_const_iterator_type().has_next_item_or_end_marker(); }
				/* has_next() is just an alias for points_to_an_item() that's familiar to java programmers. */
				bool has_next() const { return nii_array_reg_ss_const_iterator_type().has_next(); }
				bool has_previous() const { return nii_array_reg_ss_const_iterator_type().has_previous(); }
				void set_to_beginning() { nii_array_reg_ss_const_iterator_type().set_to_beginning(); }
				void set_to_end_marker() { nii_array_reg_ss_const_iterator_type().set_to_end_marker(); }
				void set_to_next() { nii_array_reg_ss_const_iterator_type().set_to_next(); }
				void set_to_previous() { nii_array_reg_ss_const_iterator_type().set_to_previous(); }
				const_iterator& operator ++() { nii_array_reg_ss_const_iterator_type().operator ++(); return (*this); }
				const_iterator operator++(int) { const_iterator _Tmp = *this; ++*this; return (_Tmp); }
				const_iterator& operator --() { nii_array_reg_ss_const_iterator_type().operator --(); return (*this); }
				const_iterator operator--(int) { const_iterator _Tmp = *this; --*this; return (_Tmp); }
				void advance(typename _MA::difference_type n) { nii_array_reg_ss_const_iterator_type().advance(n); }
				void regress(typename _MA::difference_type n) { nii_array_reg_ss_const_iterator_type().regress(n); }
				const_iterator& operator +=(difference_type n) { nii_array_reg_ss_const_iterator_type().operator +=(n); return (*this); }
				const_iterator& operator -=(difference_type n) { nii_array_reg_ss_const_iterator_type().operator -=(n); return (*this); }
				const_iterator operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
				const_iterator operator-(difference_type n) const { return ((*this) + (-n)); }
				typename _MA::difference_type operator-(const const_iterator& _Right_cref) const { return nii_array_reg_ss_const_iterator_type() - (_Right_cref.nii_array_reg_ss_const_iterator_type()); }
				typename _MA::const_reference operator*() const { return nii_array_reg_ss_const_iterator_type().operator*(); }
				typename _MA::const_reference item() const { return operator*(); }
				typename _MA::const_reference previous_item() const { return nii_array_reg_ss_const_iterator_type().previous_item(); }
				typename _MA::const_pointer operator->() const { return nii_array_reg_ss_const_iterator_type().operator->(); }
				typename _MA::const_reference operator[](typename _MA::difference_type _Off) const { return nii_array_reg_ss_const_iterator_type()[_Off]; }
				bool operator==(const const_iterator& _Right_cref) const { return nii_array_reg_ss_const_iterator_type().operator==(_Right_cref.nii_array_reg_ss_const_iterator_type()); }
				bool operator!=(const const_iterator& _Right_cref) const { return (!(_Right_cref == (*this))); }
				bool operator<(const const_iterator& _Right) const { return (nii_array_reg_ss_const_iterator_type() < _Right.nii_array_reg_ss_const_iterator_type()); }
				bool operator<=(const const_iterator& _Right) const { return (nii_array_reg_ss_const_iterator_type() <= _Right.nii_array_reg_ss_const_iterator_type()); }
				bool operator>(const const_iterator& _Right) const { return (nii_array_reg_ss_const_iterator_type() > _Right.nii_array_reg_ss_const_iterator_type()); }
				bool operator>=(const const_iterator& _Right) const { return (nii_array_reg_ss_const_iterator_type() >= _Right.nii_array_reg_ss_const_iterator_type()); }
				void set_to_const_item_pointer(const const_iterator& _Right_cref) { nii_array_reg_ss_const_iterator_type().set_to_const_item_pointer(_Right_cref.nii_array_reg_ss_const_iterator_type()); }
				msear_size_t position() const { return nii_array_reg_ss_const_iterator_type().position(); }
				auto target_container_ptr() const -> decltype(nii_array_reg_ss_const_iterator_type().target_container_ptr()) {
					return nii_array_reg_ss_const_iterator_type().target_container_ptr();
				}
				void async_not_shareable_and_not_passable_tag() const {}
			private:
				const_iterator(mse::TRegisteredConstPointer<_MA> nii_array_regcptr) {
					if (nii_array_regcptr) {
						m_reg_ss_const_iterator = _MA::template ss_cbegin<mse::TRegisteredConstPointer<_MA>>(nii_array_regcptr);
					}
				}
				reg_ss_const_iterator_type m_reg_ss_const_iterator;

				friend class /*_Myt*/array<_Ty, _Size>;
				friend class iterator;
				friend /*class*/ xscope_const_iterator;
			};
			class iterator : public _MA::na_iterator_base {
			public:
				typedef typename _MA::na_iterator_base base_class;
				MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

				iterator() {}
				iterator(const iterator& src_cref) = default;
				~iterator() {}
				const reg_ss_iterator_type& nii_array_reg_ss_iterator_type() const {
					//if (!contained_array()_regptr) { MSE_THROW(mstdarray_range_error("attempt to use an invalid iterator - mse::mstd::array<>::iterator")); }
					return m_reg_ss_iterator;
				}
				reg_ss_iterator_type& nii_array_reg_ss_iterator_type() {
					//if (!contained_array()_regptr) { MSE_THROW(mstdarray_range_error("attempt to use an invalid iterator - mse::mstd::array<>::iterator")); }
					return m_reg_ss_iterator;
				}
				const reg_ss_iterator_type& mvssi() const { return nii_array_reg_ss_iterator_type(); }
				reg_ss_iterator_type& mvssi() { return nii_array_reg_ss_iterator_type(); }
				operator const_iterator() const {
					auto nii_array_regptr = m_reg_ss_iterator.target_container_ptr();
					const_iterator retval(nii_array_regptr);
					if (nii_array_regptr) {
						retval.nii_array_reg_ss_const_iterator_type().set_to_beginning();
						retval.nii_array_reg_ss_const_iterator_type().advance(msear_int(nii_array_reg_ss_iterator_type().position()));
					}
					return retval;
				}

				void reset() { nii_array_reg_ss_iterator_type().reset(); }
				bool points_to_an_item() const { return nii_array_reg_ss_iterator_type().points_to_an_item(); }
				bool points_to_end_marker() const { return nii_array_reg_ss_iterator_type().points_to_end_marker(); }
				bool points_to_beginning() const { return nii_array_reg_ss_iterator_type().points_to_beginning(); }
				/* has_next_item_or_end_marker() is just an alias for points_to_an_item(). */
				bool has_next_item_or_end_marker() const { return nii_array_reg_ss_iterator_type().has_next_item_or_end_marker(); }
				/* has_next() is just an alias for points_to_an_item() that's familiar to java programmers. */
				bool has_next() const { return nii_array_reg_ss_iterator_type().has_next(); }
				bool has_previous() const { return nii_array_reg_ss_iterator_type().has_previous(); }
				void set_to_beginning() { nii_array_reg_ss_iterator_type().set_to_beginning(); }
				void set_to_end_marker() { nii_array_reg_ss_iterator_type().set_to_end_marker(); }
				void set_to_next() { nii_array_reg_ss_iterator_type().set_to_next(); }
				void set_to_previous() { nii_array_reg_ss_iterator_type().set_to_previous(); }
				iterator& operator ++() { nii_array_reg_ss_iterator_type().operator ++(); return (*this); }
				iterator operator++(int) { iterator _Tmp = *this; ++*this; return (_Tmp); }
				iterator& operator --() { nii_array_reg_ss_iterator_type().operator --(); return (*this); }
				iterator operator--(int) { iterator _Tmp = *this; --*this; return (_Tmp); }
				void advance(typename _MA::difference_type n) { nii_array_reg_ss_iterator_type().advance(n); }
				void regress(typename _MA::difference_type n) { nii_array_reg_ss_iterator_type().regress(n); }
				iterator& operator +=(difference_type n) { nii_array_reg_ss_iterator_type().operator +=(n); return (*this); }
				iterator& operator -=(difference_type n) { nii_array_reg_ss_iterator_type().operator -=(n); return (*this); }
				iterator operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
				iterator operator-(difference_type n) const { return ((*this) + (-n)); }
				typename _MA::difference_type operator-(const iterator& _Right_cref) const { return nii_array_reg_ss_iterator_type() - (_Right_cref.nii_array_reg_ss_iterator_type()); }
				typename _MA::reference operator*() const { return nii_array_reg_ss_iterator_type().operator*(); }
				typename _MA::reference item() const { return operator*(); }
				typename _MA::reference previous_item() const { return nii_array_reg_ss_iterator_type().previous_item(); }
				typename _MA::pointer operator->() const { return nii_array_reg_ss_iterator_type().operator->(); }
				typename _MA::reference operator[](typename _MA::difference_type _Off) const { return nii_array_reg_ss_iterator_type()[_Off]; }
				bool operator==(const iterator& _Right_cref) const { return nii_array_reg_ss_iterator_type().operator==(_Right_cref.nii_array_reg_ss_iterator_type()); }
				bool operator!=(const iterator& _Right_cref) const { return (!(_Right_cref == (*this))); }
				bool operator<(const iterator& _Right) const { return (nii_array_reg_ss_iterator_type() < _Right.nii_array_reg_ss_iterator_type()); }
				bool operator<=(const iterator& _Right) const { return (nii_array_reg_ss_iterator_type() <= _Right.nii_array_reg_ss_iterator_type()); }
				bool operator>(const iterator& _Right) const { return (nii_array_reg_ss_iterator_type() > _Right.nii_array_reg_ss_iterator_type()); }
				bool operator>=(const iterator& _Right) const { return (nii_array_reg_ss_iterator_type() >= _Right.nii_array_reg_ss_iterator_type()); }
				void set_to_item_pointer(const iterator& _Right_cref) { nii_array_reg_ss_iterator_type().set_to_item_pointer(_Right_cref.nii_array_reg_ss_iterator_type()); }
				msear_size_t position() const { return nii_array_reg_ss_iterator_type().position(); }
				auto target_container_ptr() const -> decltype(nii_array_reg_ss_iterator_type().target_container_ptr()) {
					return nii_array_reg_ss_iterator_type().target_container_ptr();
				}
				void async_not_shareable_and_not_passable_tag() const {}
			private:
				reg_ss_iterator_type m_reg_ss_iterator;

				friend class /*_Myt*/array<_Ty, _Size>;
				friend /*class*/ xscope_const_iterator;
				friend /*class*/ xscope_iterator;
			};

			iterator begin()
			{	// return iterator for beginning of mutable sequence
				iterator retval; //retval.contained_array()_regptr = &(this->contained_array());
				(retval.m_reg_ss_iterator) = contained_array().template ss_begin<mse::TRegisteredPointer<_MA>>(&(this->contained_array()));
				return retval;
			}
			const_iterator begin() const
			{	// return iterator for beginning of nonmutable sequence
				const_iterator retval; //retval.contained_array()_regcptr = &(this->contained_array());
				(retval.m_reg_ss_const_iterator) = contained_array().template ss_cbegin<mse::TRegisteredConstPointer<_MA>>(&(this->contained_array()));
				return retval;
			}
			iterator end() {	// return iterator for end of mutable sequence
				iterator retval; //retval.contained_array()_regptr = &(this->contained_array());
				(retval.m_reg_ss_iterator) = contained_array().template ss_end<mse::TRegisteredPointer<_MA>>(&(this->contained_array()));
				return retval;
			}
			const_iterator end() const {	// return iterator for end of nonmutable sequence
				const_iterator retval; //retval.contained_array()_regcptr = &(this->contained_array());
				(retval.m_reg_ss_const_iterator) = contained_array().template ss_cend<mse::TRegisteredConstPointer<_MA>>(&(this->contained_array()));
				return retval;
			}
			const_iterator cbegin() const {	// return iterator for beginning of nonmutable sequence
				const_iterator retval; //retval.contained_array()_regcptr = &(this->contained_array());
				(retval.m_reg_ss_const_iterator) = contained_array().template ss_cbegin<mse::TRegisteredConstPointer<_MA>>(&(this->contained_array()));
				return retval;
			}
			const_iterator cend() const {	// return iterator for end of nonmutable sequence
				const_iterator retval; //retval.contained_array()_regcptr = &(this->contained_array());
				(retval.m_reg_ss_const_iterator) = contained_array().template ss_cend<mse::TRegisteredConstPointer<_MA>>(&(this->contained_array()));
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


			bool operator==(const _Myt& _Right) const {	// test for array equality
				return (_Right.contained_array() == contained_array());
			}
			bool operator<(const _Myt& _Right) const {	// test if _Left < _Right for arrays
				return (contained_array() < _Right.contained_array());
			}

			void async_not_shareable_tag() const {}
			/* this array should be safely passable iff the element type is safely passable */
			template<class _Ty2 = _Ty, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _Ty>::value)
				&& (mse::impl::is_marked_as_passable_msemsearray<_Ty2>::value)> MSE_IMPL_EIS >
			void async_passable_tag() const {}

		private:
			const _MA& as_nii_array() const & { return contained_array(); }
			//const _MA& as_nii_array() const&& { return contained_array(); }
			_MA& as_nii_array()& { return contained_array(); }
			_MA&& as_nii_array()&& { return std::forward<_RMA>(contained_array()); }

			//mse::TRegisteredObj<_MA> m_nii_array;

			friend xscope_const_iterator;
			friend xscope_iterator;

			template<size_t _Idx, class _Tz, size_t _Size2>
			friend _CONST_FUN _Tz& std::get(mse::mstd::array<_Tz, _Size2>& _Arr) _NOEXCEPT;
			template<size_t _Idx, class _Tz, size_t _Size2>
			friend _CONST_FUN const _Tz& std::get(const mse::mstd::array<_Tz, _Size2>& _Arr) _NOEXCEPT;
			template<size_t _Idx, class _Tz, size_t _Size2>
			friend _CONST_FUN _Tz&& std::get(mse::mstd::array<_Tz, _Size2>&& _Arr) _NOEXCEPT;

			friend void swap(_Myt& a, _Myt& b) { a.swap(b); }
			friend void swap(_Myt& a, _MA& b) { a.swap(b); }
			friend void swap(_Myt& a, mse::us::msearray<_Ty, _Size>& b) { a.swap(b); }
			friend void swap(_Myt& a, std::array<_Ty, _Size>& b) { a.swap(b); }
			friend void swap(_MA& a, _Myt& b) { b.swap(a); }
			friend void swap(mse::us::msearray<_Ty, _Size>& a, _Myt& b) { b.swap(a); }
			friend void swap(std::array<_Ty, _Size>& a, _Myt& b) { b.swap(a); }
		};

#ifdef MSE_HAS_CXX17
		/* deduction guides */
		template<class _First,
			class... _Rest>
			array(_First, _Rest...)
			->array<typename mse::impl::_mse_Enforce_same<_First, _Rest...>::type, 1 + sizeof...(_Rest)>;
#endif /* MSE_HAS_CXX17 */

		template<class _Ty, size_t _Size> inline bool operator!=(const array<_Ty, _Size>& _Left,
			const array<_Ty, _Size>& _Right) {	// test for array inequality
			return (!(_Left == _Right));
		}

		template<class _Ty, size_t _Size> inline bool operator>(const array<_Ty, _Size>& _Left,
			const array<_Ty, _Size>& _Right) {	// test if _Left > _Right for arrays
			return (_Right < _Left);
		}

		template<class _Ty, size_t _Size> inline bool operator<=(const array<_Ty, _Size>& _Left,
			const array<_Ty, _Size>& _Right) {	// test if _Left <= _Right for arrays
			return (!(_Right < _Left));
		}

		template<class _Ty, size_t _Size> inline bool operator>=(const array<_Ty, _Size>& _Left,
			const array<_Ty, _Size>& _Right) {	// test if _Left >= _Right for arrays
			return (!(_Left < _Right));
		}

		template<class _TArray> using xscope_array_const_iterator = typename _TArray::xscope_const_iterator;
		template<class _TArray> using xscope_array_iterator = typename _TArray::xscope_iterator;

		template<class _TArray>
		auto make_xscope_const_iterator(const mse::TXScopeObjFixedConstPointer<_TArray>& owner_ptr) {
			return mse::make_xscope_const_iterator(owner_ptr);
		}
		template<class _TArray>
		auto make_xscope_const_iterator(const mse::TXScopeObjFixedPointer<_TArray>& owner_ptr) {
			return mse::make_xscope_const_iterator(owner_ptr);
		}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _TArray>
		auto make_xscope_const_iterator(const mse::TXScopeFixedConstPointer<_TArray>& owner_ptr) {
			return mse::make_xscope_const_iterator(owner_ptr);
		}
		template<class _TArray>
		auto make_xscope_const_iterator(const mse::TXScopeFixedPointer<_TArray>& owner_ptr) {
			return mse::make_xscope_const_iterator(owner_ptr);
		}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

		template<class _TArray>
		auto make_xscope_iterator(const mse::TXScopeObjFixedPointer<_TArray>& owner_ptr) {
			return mse::make_xscope_iterator(owner_ptr);
		}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _TArray>
		auto make_xscope_iterator(const mse::TXScopeFixedPointer<_TArray>& owner_ptr) {
			return mse::make_xscope_iterator(owner_ptr);
		}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
	}
}

namespace std {

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmismatched-tags"
#endif /*__clang__*/

	template<class _Ty, size_t _Size>
	struct tuple_size<mse::mstd::array<_Ty, _Size> >
		: integral_constant<size_t, _Size>
	{	// struct to determine number of elements in array
	};

	template<size_t _Idx, class _Ty, size_t _Size>
	struct tuple_element<_Idx, mse::mstd::array<_Ty, _Size> >
	{	// struct to determine type of element _Idx in array
		static_assert(_Idx < _Size, "array index out of bounds");

		typedef _Ty type;
	};

#ifdef __clang__
#pragma clang diagnostic pop
#endif /*__clang__*/

	// TUPLE INTERFACE TO array
	template<size_t _Idx, class _Ty, size_t _Size>
	_CONST_FUN _Ty& get(mse::mstd::array<_Ty, _Size>& _Arr) _NOEXCEPT
	{	// return element at _Idx in array _Arr
		static_assert(_Idx < _Size, "array index out of bounds");
		return (std::get<_Idx>(_Arr.contained_array()));
	}
	template<size_t _Idx, class _Ty, size_t _Size>
	_CONST_FUN const _Ty& get(const mse::mstd::array<_Ty, _Size>& _Arr) _NOEXCEPT
	{	// return element at _Idx in array _Arr
		static_assert(_Idx < _Size, "array index out of bounds");
		return (std::get<_Idx>(_Arr.contained_array()));
	}
	template<size_t _Idx, class _Ty, size_t _Size>
	_CONST_FUN _Ty&& get(mse::mstd::array<_Ty, _Size>&& _Arr) _NOEXCEPT
	{	// return element at _Idx in array _Arr
		static_assert(_Idx < _Size, "array index out of bounds");
		return (_STD move(std::get<_Idx>(_Arr.contained_array())));
	}

	template<class _Ty, size_t _Size>
	void swap(mse::mstd::array<_Ty, _Size>& _Left, mse::mstd::array<_Ty, _Size>& _Right) _NOEXCEPT_OP(_NOEXCEPT_OP(_Left.swap(_Right)))
	{	// swap arrays
		return (_Left.swap(_Right));
	}
	template<class _Ty, size_t _Size/*, class = enable_if_t<_Size == 0 || _Is_swappable<_Ty>::value>*/>
	void swap(mse::mstd::array<_Ty, _Size>& _Left, mse::nii_array<_Ty, _Size>& _Right) _NOEXCEPT_OP(_NOEXCEPT_OP(_Left.swap(_Right)))
	{	// swap arrays
		return (_Left.swap(_Right));
	}
	template<class _Ty, size_t _Size, class _TStateMutex = mse::default_state_mutex/*, class = enable_if_t<_Size == 0 || _Is_swappable<_Ty>::value>*/>
	void swap(mse::mstd::array<_Ty, _Size>& _Left, mse::us::msearray<_Ty, _Size, _TStateMutex>& _Right) _NOEXCEPT_OP(_NOEXCEPT_OP(_Left.swap(_Right)))
	{	// swap arrays
		return (_Left.swap(_Right));
	}

	template<class _Ty, size_t _Size/*, class = enable_if_t<_Size == 0 || _Is_swappable<_Ty>::value>*/>
	void swap(mse::nii_array<_Ty, _Size>& _Left, mse::mstd::array<_Ty, _Size>& _Right) _NOEXCEPT_OP(_NOEXCEPT_OP(_Right.swap(_Left)))
	{	// swap arrays
		return (_Right.swap(_Left));
	}
	template<class _Ty, size_t _Size, class _TStateMutex = mse::default_state_mutex/*, class = enable_if_t<_Size == 0 || _Is_swappable<_Ty>::value>*/>
	void swap(mse::us::msearray<_Ty, _Size, _TStateMutex>& _Left, mse::mstd::array<_Ty, _Size>& _Right) _NOEXCEPT_OP(_NOEXCEPT_OP(_Right.swap(_Left)))
	{	// swap arrays
		return (_Right.swap(_Left));
	}

}

namespace mse {
	namespace mstd {

#endif /*MSE_MSTDARRAY_DISABLED*/

		template<class _TArrayPointer>
		auto make_xscope_begin_const_iterator(const _TArrayPointer& owner_ptr) {
			return mse::mstd::make_xscope_const_iterator(owner_ptr);
		}
		template<class _TArrayPointer>
		auto make_xscope_begin_iterator(const _TArrayPointer& owner_ptr) {
			return mse::mstd::make_xscope_iterator(owner_ptr);
		}

		template<class _TArrayPointer>
		auto make_xscope_end_const_iterator(const _TArrayPointer& owner_ptr) {
			return mse::mstd::make_xscope_begin_const_iterator(owner_ptr) + (*owner_ptr).size();
		}
		template<class _TArrayPointer>
		auto make_xscope_end_iterator(const _TArrayPointer& owner_ptr) {
			auto retval = mse::mstd::make_xscope_begin_iterator(owner_ptr);
			retval += (*owner_ptr).size();
			return retval;
		}
		/* Overloads for rsv::TReturnableFParam<>. */
		MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_begin_const_iterator)
		MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_begin_iterator)
		MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_end_const_iterator)
		MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_end_iterator)
		MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_const_iterator)
		MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_iterator)

		template<class _Ty, size_t _Size>
		TXScopeFixedPointer<_Ty> xscope_pointer_to_array_element(const xscope_array_iterator<array<_Ty, _Size> >& iter_cref) {
			return mse::us::unsafe_make_xscope_pointer_to(*iter_cref);
		}
		template<class _Ty, size_t _Size>
		TXScopeFixedPointer<_Ty> xscope_pointer_to_array_element(const mse::TXScopeFixedPointer<array<_Ty, _Size> >& ptr, typename array<_Ty, _Size>::size_type _P) {
			return mse::us::unsafe_make_xscope_pointer_to((*ptr)[_P]);
		}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _Ty, size_t _Size>
		TXScopeFixedPointer<_Ty> xscope_pointer_to_array_element(const mse::TXScopeObjFixedPointer<array<_Ty, _Size> >& ptr, typename array<_Ty, _Size>::size_type _P) {
			return mse::us::unsafe_make_xscope_pointer_to((*ptr)[_P]);
		}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

		template<class _Ty, size_t _Size>
		TXScopeFixedConstPointer<_Ty> xscope_const_pointer_to_array_element(const xscope_array_const_iterator<array<_Ty, _Size> >& iter_cref) {
			return mse::us::unsafe_make_xscope_const_pointer_to(*iter_cref);
		}
		template<class _Ty, size_t _Size>
		TXScopeFixedConstPointer<_Ty> xscope_const_pointer_to_array_element(const mse::TXScopeFixedConstPointer<array<_Ty, _Size> >& ptr, typename array<_Ty, _Size>::size_type _P) {
			return mse::us::unsafe_make_xscope_const_pointer_to((*ptr)[_P]);
		}
		template<class _Ty, size_t _Size>
		TXScopeFixedConstPointer<_Ty> xscope_const_pointer_to_array_element(const mse::TXScopeFixedPointer<array<_Ty, _Size> >& ptr, typename array<_Ty, _Size>::size_type _P) {
			return mse::us::unsafe_make_xscope_const_pointer_to((*ptr)[_P]);
		}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _Ty, size_t _Size>
		TXScopeFixedConstPointer<_Ty> xscope_const_pointer_to_array_element(const mse::TXScopeObjFixedConstPointer<array<_Ty, _Size> >& ptr, typename array<_Ty, _Size>::size_type _P) {
			return mse::us::unsafe_make_xscope_const_pointer_to((*ptr)[_P]);
		}
		template<class _Ty, size_t _Size>
		TXScopeFixedConstPointer<_Ty> xscope_const_pointer_to_array_element(const mse::TXScopeObjFixedPointer<array<_Ty, _Size> >& ptr, typename array<_Ty, _Size>::size_type _P) {
			return mse::us::unsafe_make_xscope_const_pointer_to((*ptr)[_P]);
		}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
		/* Overloads for rsv::TReturnableFParam<>. */
		MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(xscope_pointer_to_array_element)
		MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(xscope_const_pointer_to_array_element)

		class array_test {
		public:
			void test1() {
#ifdef MSE_SELF_TESTS
				// construction uses aggregate initialization
				mse::mstd::array<int, 3> a1{ { 1, 2, 3 } }; // double-braces required in C++11 (not in C++14)
				mse::mstd::array<int, 3> a2 = { 11, 12, 13 };  // never required after =
				mse::mstd::array<std::string, 2> a3 = { std::string("a"), "b" };

				// container operations are supported
				std::sort(a1.begin(), a1.end());
				std::reverse_copy(a2.begin(), a2.end(),
					std::ostream_iterator<int>(std::cout, " "));

				std::cout << '\n';

				// ranged for loop is supported
				for (const auto& s : a3)
					std::cout << s << ' ';

				a1.swap(a2);
				assert(13 == a1[2]);
				assert(3 == a2[2]);

				std::swap(a1, a2);
				assert(3 == a1[2]);
				assert(13 == a2[2]);

				std::get<0>(a1) = 21;
				std::get<1>(a1) = 22;
				std::get<2>(a1) = 23;

				auto l_tuple_size = std::tuple_size<mse::mstd::array<int, 3>>::value;
				std::tuple_element<1, mse::mstd::array<int, 3>>::type b1 = 5;

				a1 = a2;

				{
					mse::mstd::array<int, 5> a = { 10, 20, 30, 40, 50 };
					mse::mstd::array<int, 5> b = { 10, 20, 30, 40, 50 };
					mse::mstd::array<int, 5> c = { 50, 40, 30, 20, 10 };

					if (a == b) std::cout << "a and b are equal\n";
					if (b != c) std::cout << "b and c are not equal\n";
					if (b<c) std::cout << "b is less than c\n";
					if (c>b) std::cout << "c is greater than b\n";
					if (a <= b) std::cout << "a is less than or equal to b\n";
					if (a >= b) std::cout << "a is greater than or equal to b\n";
				}
				{
					mse::mstd::array<int, 6> myarray;

					myarray.fill(5);

					std::cout << "myarray contains:";
					for (int& x : myarray) { std::cout << ' ' << x; }

					std::cout << '\n';
				}
				{
					/* arrays of non-default constructible elements */
					class A {
					public:
						A(int i) : m_i(i) {}
						int m_i;
					};
					int z1 = 7;
					std::array<A, 3> sa1{z1, A(8), 9};
					mse::mstd::array<A, 3> mstda1{ z1, A(8), 9 };
				}
#endif // MSE_SELF_TESTS
			}
		};
	}
}

#ifdef __clang__
#pragma clang diagnostic pop
#else /*__clang__*/
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif /*__GNUC__*/
#endif /*__clang__*/

#ifndef MSE_PUSH_MACRO_NOT_SUPPORTED
#pragma pop_macro("MSE_THROW")
#pragma pop_macro("_STD")
#pragma pop_macro("_XSTD")
#pragma pop_macro("_NOEXCEPT")
#pragma pop_macro("_NOEXCEPT_OP")
#pragma pop_macro("_CONST_FUN")
#endif // !MSE_PUSH_MACRO_NOT_SUPPORTED

#ifdef _MSC_VER
#pragma warning( pop )  
#endif /*_MSC_VER*/

#endif /*ndef MSEMSTDARRAY_H*/
