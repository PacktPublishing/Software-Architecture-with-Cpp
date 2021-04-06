
// Copyright (c) 2015 Noah Lopez
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef MSEMSEARRAY_H
#define MSEMSEARRAY_H

/*compiler specific defines*/
#ifdef _MSC_VER
#if (1700 > _MSC_VER)
#define MSVC2010_COMPATIBLE 1
#endif /*(1700 > _MSC_VER)*/
#if (1900 > _MSC_VER)
#define MSVC2013_COMPATIBLE 1
#endif /*(1900 > _MSC_VER)*/
#else /*_MSC_VER*/
#if (defined(__GNUC__) || defined(__GNUG__))
#define GPP_COMPATIBLE 1
#if ((5 > __GNUC__) && (!defined(__clang__)))
#define GPP4P8_COMPATIBLE 1
#endif /*((5 > __GNUC__) && (!defined(__clang__)))*/
#endif
#endif /*_MSC_VER*/

//define MSE_MSEARRAY_USE_MSE_PRIMITIVES 1
#ifdef MSE_MSEARRAY_USE_MSE_PRIMITIVES
#include "mseprimitives.h"
#endif // MSE_MSEARRAY_USE_MSE_PRIMITIVES

#include "msescope.h"
#include "mseoptional.h"
//include "msealgorithm.h"
#include <array>
#include <cassert>
#include <memory>
#include <unordered_map>
#include <tuple>
#include <limits>
#include <climits>       // ULONG_MAX
#include <stdexcept>
#include <type_traits>
//include <shared_mutex>
//include <mutex>
#include <algorithm>
#include <iostream>
#ifdef MSE_HAS_CXX17
//include <variant>
#include <string_view>
#endif // MSE_HAS_CXX17

#ifdef MSE_TRASECTIONSPLITTERXWP_NDEBUG
#else // MSE_TRASECTIONSPLITTERXWP_NDEBUG
#include "msenorad.h"
#endif // MSE_TRASECTIONSPLITTERXWP_NDEBUG

#ifdef MSE_SELF_TESTS
#include <string>
#include <iterator>
#endif // MSE_SELF_TESTS

/* Conditional definition of MSE_MSTDARRAY_DISABLED and MSE_MSTDVECTOR_DISABLED was moved to msepointerbasics.h. */
///* This is done here rather than in msemstdarray.h because some elements in this file have to know whether or not
//MSE_MSTDARRAY_DISABLED and MSE_MSTDVECTOR_DISABLED will ultimately be defined. */
//#ifdef MSE_SAFER_SUBSTITUTES_DISABLED
//#define MSE_MSTDARRAY_DISABLED
//#endif /*MSE_SAFER_SUBSTITUTES_DISABLED*/
//#ifdef MSE_MSTDVECTOR_DISABLED
///* At the moment, the implementation of "disabled" mstd::vector<> is dependent on the implementation of disabled mstd::array<>,
//so you can't disable mstd::vector<> without also disabling mstd::array<>. */
//#define MSE_MSTDARRAY_DISABLED
//#endif /*MSE_MSTDVECTOR_DISABLED*/

#ifdef _MSC_VER
#pragma warning( push )  
#pragma warning( disable : 4100 4456 4189 4505 5046 )
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
#define _CONST_FUN constexpr
#endif /*_CONST_FUN*/

namespace mse {

#ifdef MSE_MSEARRAY_USE_MSE_PRIMITIVES
	typedef mse::CSize_t msear_size_t;
	typedef mse::CInt msear_int;
	typedef bool msear_bool; // no added safety benefit to using mse::CBool in this case
#define msear_as_a_size_t as_a_size_t
#else // MSE_MSEARRAY_USE_MSE_PRIMITIVES

#ifndef MSE_MSEARRAY_BASE_INTEGER_TYPE
#if SIZE_MAX <= UINT_MAX
#define MSE_MSEARRAY_BASE_INTEGER_TYPE int
#else // SIZE_MAX <= INT_MAX
#if SIZE_MAX <= ULONG_MAX
#define MSE_MSEARRAY_BASE_INTEGER_TYPE long int
#else // SIZE_MAX <= ULONG_MAX
#define MSE_MSEARRAY_BASE_INTEGER_TYPE long long int
#endif // SIZE_MAX <= ULONG_MAX
#endif // SIZE_MAX <= INT_MAX
#endif // !MSE_MSEARRAY_BASE_INTEGER_TYPE

	typedef size_t msear_size_t;
	typedef MSE_MSEARRAY_BASE_INTEGER_TYPE msear_int;
	typedef bool msear_bool;
	typedef size_t msear_as_a_size_t;
#endif // MSE_MSEARRAY_USE_MSE_PRIMITIVES

	class nii_array_range_error : public std::range_error {
	public:
		using std::range_error::range_error;
	};
	class nii_array_null_dereference_error : public std::logic_error {
	public:
		using std::logic_error::logic_error;
	};
	class msearray_range_error : public std::range_error {
	public:
		using std::range_error::range_error;
	};
	class msearray_null_dereference_error : public std::logic_error {
	public:
		using std::logic_error::logic_error;
	};

	/* msear_pointer behaves similar to native pointers. It's a bit safer in that it initializes to
	nullptr by default and checks for attempted dereference of null pointers. */
	template<typename _Ty>
	class msear_pointer {
	public:
		msear_pointer() : m_ptr(nullptr) {}
		msear_pointer(_Ty* ptr) : m_ptr(ptr) {}
		//msear_pointer(const msear_pointer<_Ty>& src) : m_ptr(src.m_ptr) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<
			std::is_same<_Ty2, _Ty>::value || ((!std::is_const<_Ty2>::value) && std::is_same<const _Ty2, _Ty>::value)
			> MSE_IMPL_EIS >
			msear_pointer(const msear_pointer<_Ty2>& src) : m_ptr(src.m_ptr) {}

		_Ty& operator*() const {
#ifndef MSE_DISABLE_MSEAR_POINTER_CHECKS
			if (nullptr == m_ptr) { MSE_THROW(msearray_null_dereference_error("attempt to dereference null pointer - mse::msear_pointer")); }
#endif /*MSE_DISABLE_MSEAR_POINTER_CHECKS*/
			return (*m_ptr);
		}
		_Ty* operator->() const {
#ifndef MSE_DISABLE_MSEAR_POINTER_CHECKS
			if (nullptr == m_ptr) { MSE_THROW(msearray_null_dereference_error("attempt to dereference null pointer - mse::msear_pointer")); }
#endif /*MSE_DISABLE_MSEAR_POINTER_CHECKS*/
			return m_ptr;
		}

		msear_pointer<_Ty>& operator=(_Ty* ptr) {
			m_ptr = ptr;
			return (*this);
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<
			std::is_same<_Ty2, _Ty>::value
			|| ((!std::is_const<_Ty2>::value) && std::is_same<const _Ty2, _Ty>::value)
			> MSE_IMPL_EIS >
			msear_pointer<_Ty>& operator=(const msear_pointer<_Ty2>& src) {
			m_ptr = src.m_ptr;
			return (*this);
		}

		bool operator==(const msear_pointer _Right_cref) const { return (_Right_cref.m_ptr == m_ptr); }
		bool operator!=(const msear_pointer _Right_cref) const { return (!((*this) == _Right_cref)); }
		//bool operator==(const _Ty* _Right_cref) const { return (_Right_cref == m_ptr); }
		//bool operator!=(const _Ty* _Right_cref) const { return (!((*this) == _Right_cref)); }

		bool operator!() const { return (!m_ptr); }
		operator bool() const { return (m_ptr != nullptr); }

		explicit operator _Ty*() const { return m_ptr; }

		void async_not_shareable_and_not_passable_tag() const {}

	private:
		_Ty* m_ptr;

		template<typename _Ty2> friend class msear_pointer;
	};


	/* Some iterators are prone to having their target container prematurely deallocated out from under them. If you have a safe pointer
	to the target container, you can use TSyncWeakFixedIterator<> as a safe iterator that welds a copy of the safe pointer (aka "lease")
	to the iterator. */
	template <class _TIterator, class _TLeasePointer>
	class TSyncWeakFixedIterator : public _TIterator {
	public:
		typedef _TIterator base_class;
		TSyncWeakFixedIterator(const TSyncWeakFixedIterator&) = default;
		template<class _TLeasePointer2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_TLeasePointer2, _TLeasePointer>::value> MSE_IMPL_EIS >
		TSyncWeakFixedIterator(const TSyncWeakFixedIterator<_TIterator, _TLeasePointer2>&src) : base_class(src), m_lease_pointer(src.lease_pointer()) {}

		auto operator*() const -> typename std::add_lvalue_reference<decltype(*((*this).m_target_pointer))>::type {
			/*const auto &test_cref =*/ *m_lease_pointer; // this should throw if m_lease_pointer is no longer valid
			return (*((*this).m_target_pointer));
		}
		auto operator->() const -> decltype(std::addressof(*((*this).m_target_pointer))) {
			/*const auto &test_cref =*/ *m_lease_pointer; // this should throw if m_lease_pointer is no longer valid
			return std::addressof(*((*this).m_target_pointer));
		}

		_TLeasePointer lease_pointer() const { return (*this).m_lease_pointer; }

		template <class _TIterator2, class _TLeasePointer2>
		static TSyncWeakFixedIterator make(const _TIterator2& src_iterator, const _TLeasePointer2& lease_pointer) {
			return TSyncWeakFixedIterator(src_iterator, lease_pointer);
		}
		void async_not_shareable_and_not_passable_tag() const {}

	protected:
		TSyncWeakFixedIterator(const _TIterator& src_iterator, const _TLeasePointer& lease_pointer/* often a registered pointer */)
			: base_class(src_iterator), m_lease_pointer(lease_pointer) {}
	private:
		TSyncWeakFixedIterator & operator=(const TSyncWeakFixedIterator& _Right_cref) = delete;

		_TLeasePointer m_lease_pointer;

		//friend class TSyncWeakFixedConstIterator<_TIterator, _TLeasePointer>;
	};

	template <class _Ty, class _TLeasePointer>
	class TSyncWeakFixedIterator<_Ty*, _TLeasePointer> : public mse::us::TSaferPtrForLegacy<_Ty> {
	public:
		typedef mse::us::TSaferPtrForLegacy<_Ty> _TIterator;
		typedef _TIterator base_class;
		TSyncWeakFixedIterator(const TSyncWeakFixedIterator&) = default;
		template<class _TLeasePointer2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_TLeasePointer2, _TLeasePointer>::value> MSE_IMPL_EIS >
		TSyncWeakFixedIterator(const TSyncWeakFixedIterator<_TIterator, _TLeasePointer2>&src) : base_class(src), m_lease_pointer(src.lease_pointer()) {}

		auto operator*() const -> typename std::add_lvalue_reference<decltype(*((*this).m_target_pointer))>::type {
			/*const auto &test_cref =*/ *m_lease_pointer; // this should throw if m_lease_pointer is no longer valid
			return (*((*this).m_target_pointer));
		}
		auto operator->() const -> decltype(std::addressof(*((*this).m_target_pointer))) {
			/*const auto &test_cref =*/ *m_lease_pointer; // this should throw if m_lease_pointer is no longer valid
			return std::addressof(*((*this).m_target_pointer));
		}

		_TLeasePointer lease_pointer() const { return (*this).m_lease_pointer; }

		template <class _TIterator2, class _TLeasePointer2>
		static TSyncWeakFixedIterator make(const _TIterator2& src_iterator, const _TLeasePointer2& lease_pointer) {
			return TSyncWeakFixedIterator(src_iterator, lease_pointer);
		}
		void async_not_shareable_and_not_passable_tag() const {}

	protected:
		TSyncWeakFixedIterator(const _TIterator& src_iterator, const _TLeasePointer& lease_pointer/* often a registered pointer */)
			: base_class(src_iterator), m_lease_pointer(lease_pointer) {}
		TSyncWeakFixedIterator(const _Ty* & src_iterator, const _TLeasePointer& lease_pointer/* often a registered pointer */)
			: base_class(mse::us::TSaferPtrForLegacy<_Ty>(src_iterator)), m_lease_pointer(lease_pointer) {}
	private:
		TSyncWeakFixedIterator & operator=(const TSyncWeakFixedIterator& _Right_cref) = delete;

		_TLeasePointer m_lease_pointer;

		//friend class TSyncWeakFixedConstIterator<_TIterator, _TLeasePointer>;
	};

	template <class _TIterator, class _TLeasePointer>
	TSyncWeakFixedIterator<_TIterator, _TLeasePointer> make_syncweak_iterator(const _TIterator& src_iterator, const _TLeasePointer& lease_pointer) {
		return TSyncWeakFixedIterator<_TIterator, _TLeasePointer>::make(src_iterator, lease_pointer);
	}


	namespace impl {
		/* Some data structures to determine, at compile time, if a given type has certain features. */

		template<class T> using HasOrInheritsAssignmentOperator_msemsearray = mse::impl::HasOrInheritsAssignmentOperator_msepointerbasics<T>;

		template<class T, class EqualTo>
		struct HasOrInheritsEqualityOperator_msemsearray_impl
		{
			template<class U, class V>
			static auto test(U*) -> decltype(std::declval<U>() == std::declval<V>(), bool(true));
			template<typename, typename>
			static auto test(...)->std::false_type;

			static const bool value = std::is_same<bool, decltype(test<T, EqualTo>(0))>::value;
			using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
		};
		template<class T, class EqualTo = T>
		struct HasOrInheritsEqualityOperator_msemsearray : HasOrInheritsEqualityOperator_msemsearray_impl<
			mse::impl::remove_reference_t<T>, mse::impl::remove_reference_t<EqualTo> >::type {};

		template<class T, class EqualTo>
		struct HasOrInheritsTargetContainerPtrMethod_msemsearray_impl
		{
			template<class U, class V>
			static auto test(U*) -> decltype(std::declval<U>().target_container_ptr(), std::declval<V>().target_container_ptr(), bool(true));
			template<typename, typename>
			static auto test(...)->std::false_type;

			static const bool value = std::is_same<bool, decltype(test<T, EqualTo>(0))>::value;
			using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
		};
		template<class T, class EqualTo = T>
		struct HasOrInheritsTargetContainerPtrMethod_msemsearray : HasOrInheritsTargetContainerPtrMethod_msemsearray_impl<
			mse::impl::remove_reference_t<T>, mse::impl::remove_reference_t<EqualTo> >::type {};
	}


	namespace us {
		namespace impl {
			namespace array_helper {

				/* some classes used to help emulate array aggregate initialization */
				template<class _Ty, size_t _Size>
				class default_constructible_array_helper_type {
				public:
					template <typename TBeginIter, typename TEndIter>
					static auto value(const TBeginIter& begin_iter, const TEndIter& end_iter, size_t index) {
						if (end_iter > (begin_iter + index)) {
							return *(begin_iter + index);
						}
						else {
							typedef mse::impl::remove_reference_t<decltype(*begin_iter)> T;
							return T();
						}
					}
					template <typename TBeginIter, typename TEndIter, size_t... IDXs>
					static auto _range_to_tuple(const TBeginIter& begin_iter, const TEndIter& end_iter, std::index_sequence<IDXs...>) {
						//return std::tuple{ value(begin_iter, end_iter, IDXs)... };
						return std::make_tuple(value(begin_iter, end_iter, IDXs)...);
					}
					template <size_t N, typename TBeginIter, typename TEndIter>
					static auto range_to_tuple(const TBeginIter& begin_iter, const TEndIter& end_iter) {
						return _range_to_tuple(begin_iter, end_iter, std::make_index_sequence<N>{});
					}
					template<class T, std::size_t... Is, typename... Ts>
					static std::array<T, sizeof...(Is)>
						range_to_array_impl(std::index_sequence<Is...>, std::tuple<Ts...>&& parts)
					{
						return { std::get<Is>(std::move(parts))... };
					}
					template <typename T, size_t N, typename TBeginIter, typename TEndIter>
					static auto range_to_array(const TBeginIter& begin_iter, const TEndIter& end_iter)
					{
						auto aggregate_tuple = range_to_tuple<N>(begin_iter, end_iter);
						constexpr auto size = std::tuple_size<decltype(aggregate_tuple)>::value;
						std::make_index_sequence<size> seq;
						return (range_to_array_impl<T>)(seq, std::move(aggregate_tuple));
					}

					template<typename _TBeginIter, typename _TEndIter>
					static constexpr std::array<_Ty, _Size> std_array_initialized_with_range(const _TBeginIter& begin_iter, const _TEndIter& end_iter)
					{
						return range_to_array<_Ty, _Size>(begin_iter, end_iter);
					}
				};
				/*
				template<class _Ty>
				class default_constructible_array_helper_type<_Ty, 0> {
				public:
					template<typename _TBeginIter, typename _TEndIter>
					static constexpr auto std_array_initialized_with_range(const _TBeginIter& begin_iter, const _TEndIter& end_iter)
					{
						return _XSTD array<_Ty, 0>{};
					}
				};
				*/

				template<class _Ty, size_t _Size>
				class not_default_constructible_array_helper_type {
				public:
					template <typename TBeginIter, typename TEndIter>
					static auto value(const TBeginIter& begin_iter, const TEndIter& end_iter, size_t index) {
						if (end_iter <= (begin_iter + index)) {
							MSE_THROW(msearray_range_error("not enough elements in the (non-default constructible) array initializer list"));
						}
						return *(begin_iter + index);
					}
					template <typename TBeginIter, typename TEndIter, size_t... IDXs>
					static auto _range_to_tuple(TBeginIter iter, const TEndIter& end_iter, std::index_sequence<IDXs...>) {
						//return std::tuple{ value(iter, end_iter, IDXs)... };
						return std::make_tuple(value(iter, end_iter, IDXs)...);
					}
					template <size_t N, typename TBeginIter, typename TEndIter>
					static auto range_to_tuple(const TBeginIter& begin_iter, const TEndIter& end_iter) {
						return _range_to_tuple(begin_iter, end_iter, std::make_index_sequence<N>{});
					}
					template<class T, std::size_t... Is, typename... Ts>
					static std::array<T, sizeof...(Is)>
						range_to_array_impl(std::index_sequence<Is...>, std::tuple<Ts...>&& parts)
					{
						return { std::get<Is>(std::move(parts))... };
					}
					template <typename T, size_t N, typename TBeginIter, typename TEndIter>
					static auto range_to_array(const TBeginIter& begin_iter, const TEndIter& end_iter)
					{
						auto aggregate_tuple = range_to_tuple<N>(begin_iter, end_iter);
						constexpr auto size = std::tuple_size<decltype(aggregate_tuple)>::value;
						std::make_index_sequence<size> seq;
						return (range_to_array_impl<T>)(seq, std::move(aggregate_tuple));
					}

					template<typename _TBeginIter, typename _TEndIter>
					static constexpr std::array<_Ty, _Size> std_array_initialized_with_range(const _TBeginIter& begin_iter, const _TEndIter& end_iter)
					{
						return range_to_array<_Ty, _Size>(begin_iter, end_iter);
					}
				};
				template<class _Ty>
				class not_default_constructible_array_helper_type<_Ty, 0> {};
			}
		}
	}

	namespace impl {
		template <typename _TRAIterator>
		class random_access_const_iterator_base_from_ra_iterator {
		public:
			using iterator_category = std::random_access_iterator_tag;
			using value_type = mse::impl::remove_reference_t<decltype(*std::declval<_TRAIterator>())>;
			using difference_type = decltype(std::declval<_TRAIterator>() - std::declval<_TRAIterator>());
			using size_type = typename std::array<int, 0>::size_type;
			using const_pointer = typename std::add_pointer<typename std::add_const<decltype(*std::declval<_TRAIterator>())>::type>::type;
			using const_reference = typename std::add_const<decltype(*std::declval<_TRAIterator>())>::type;
			using pointer = const_pointer;
			using reference = const_reference;
		};
		template <typename _TRAIterator>
		class random_access_iterator_base_from_ra_iterator {
		public:
			using iterator_category = std::random_access_iterator_tag;
			using value_type = mse::impl::remove_reference_t<decltype(*std::declval<_TRAIterator>())>;
			using difference_type = decltype(std::declval<_TRAIterator>() - std::declval<_TRAIterator>());
			using size_type = typename std::array<int, 0>::size_type;
			using pointer = typename std::add_pointer<decltype(*std::declval<_TRAIterator>())>::type;
			using reference = decltype(*std::declval<_TRAIterator>());
			using const_pointer = const pointer;
			using const_reference = const reference;
		};

		template <typename _TRAContainer>
		class random_access_const_iterator_base_from_ra_container {
		public:
			using iterator_category = std::random_access_iterator_tag;
			using value_type = mse::impl::remove_reference_t<decltype(std::declval<_TRAContainer>()[0])>;
			using difference_type = typename std::array<int, 0>::difference_type;
			using size_type = typename std::array<int, 0>::size_type;
			using const_pointer = typename std::add_pointer<typename std::add_const<decltype(std::declval<_TRAContainer>()[0])>::type>::type;
			using const_reference = typename std::add_const<decltype(std::declval<_TRAContainer>()[0])>::type;
			using pointer = const_pointer;
			using reference = const_reference;
		};
		template <typename _TRAContainer>
		class random_access_iterator_base_from_ra_container {
		public:
			using iterator_category = std::random_access_iterator_tag;
			using value_type = mse::impl::remove_reference_t<decltype(std::declval<_TRAContainer>()[0])>;
			using difference_type = typename std::array<int, 0>::difference_type;
			using size_type = typename std::array<int, 0>::size_type;
			using pointer = typename std::add_pointer<decltype(std::declval<_TRAContainer>()[0])>::type;
			using reference = decltype(std::declval<_TRAContainer>()[0]);
			using const_pointer = const pointer;
			using const_reference = const reference;
		};
	}

	/* forward declaration of class nii_array */
	template<class _Ty, size_t _Size>
	class nii_array;
	template<class _Ty, size_t _Size>
	class xscope_nii_array;

	namespace us {
		template<class _Ty, size_t _Size, class _TStateMutex = container_adjusted_default_state_mutex<_Ty> >
		class msearray;
	}
}

namespace std {

	template<size_t _Idx, class _Tz, size_t _Size2>
	_CONST_FUN _Tz& get(mse::nii_array<_Tz, _Size2>& _Arr) _NOEXCEPT;
	template<size_t _Idx, class _Tz, size_t _Size2>
	_CONST_FUN const _Tz& get(const mse::nii_array<_Tz, _Size2>& _Arr) _NOEXCEPT;
	template<size_t _Idx, class _Tz, size_t _Size2>
	_CONST_FUN _Tz&& get(mse::nii_array<_Tz, _Size2>&& _Arr) _NOEXCEPT;

	template<size_t _Idx, class _Tz, size_t _Size2>
	_CONST_FUN _Tz& get(mse::xscope_nii_array<_Tz, _Size2>& _Arr) _NOEXCEPT;
	template<size_t _Idx, class _Tz, size_t _Size2>
	_CONST_FUN const _Tz& get(const mse::xscope_nii_array<_Tz, _Size2>& _Arr) _NOEXCEPT;
	template<size_t _Idx, class _Tz, size_t _Size2>
	_CONST_FUN _Tz&& get(mse::xscope_nii_array<_Tz, _Size2>&& _Arr) _NOEXCEPT;

	template<size_t _Idx, class _Tz, size_t _Size2, class _TStateMutex2>
	_CONST_FUN _Tz& get(mse::us::msearray<_Tz, _Size2, _TStateMutex2>& _Arr) _NOEXCEPT;
	template<size_t _Idx, class _Tz, size_t _Size2, class _TStateMutex2>
	_CONST_FUN const _Tz& get(const mse::us::msearray<_Tz, _Size2, _TStateMutex2>& _Arr) _NOEXCEPT;
	template<size_t _Idx, class _Tz, size_t _Size2, class _TStateMutex2>
	_CONST_FUN _Tz&& get(mse::us::msearray<_Tz, _Size2, _TStateMutex2>&& _Arr) _NOEXCEPT;
}

namespace mse {

	namespace us {
		namespace impl {
			class StructureLockTagBase {};

			class StaticStructureContainerTagBase {};
			class LockableStructureContainerTagBase {};
			class ContiguousSequenceContainerTagBase {};
			class ContiguousSequenceStaticStructureContainerTagBase : public ContiguousSequenceContainerTagBase, public StaticStructureContainerTagBase {};
			class ContiguousSequenceLockableStructureContainerTagBase : public ContiguousSequenceContainerTagBase, public LockableStructureContainerTagBase {};

			class StrongContainerReferenceHoldingIteratorTagBase {};
			class StaticStructureIteratorTagBase {};
			class StructureLockingIteratorTagBase : public StructureLockingObjectTagBase {};
			class ContiguousSequenceIteratorTagBase {};
			class ContiguousSequenceStaticStructureIteratorTagBase : public ContiguousSequenceIteratorTagBase, public StaticStructureIteratorTagBase {};
		}
	}

	namespace impl {

		template<class T, class EqualTo>
		struct IsDereferenceable_msemsearray_impl
		{
			template<class U, class V>
			static auto test(U*) -> decltype((*std::declval<U>()), (*std::declval<V>()), bool(true));
			template<typename, typename>
			static auto test(...)->std::false_type;

			using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
		};
		template<class T, class EqualTo = T>
		struct IsDereferenceable_msemsearray : IsDereferenceable_msemsearray_impl<
			mse::impl::remove_reference_t<T>, mse::impl::remove_reference_t<EqualTo> >::type {};

		template<typename _Ty, MSE_IMPL_EIP mse::impl::enable_if_t<(IsDereferenceable_msemsearray<_Ty>::value)> MSE_IMPL_EIS >
		void T_valid_if_is_dereferenceable() {}

		template<typename T, size_t n>
		size_t native_array_size_msemsearray(const T(&)[n]) {
			return n;
		}
		template<class T, class EqualTo>
		struct IsNativeArray_msemsearray_impl
		{
			template<class U, class V>
			static auto test(U*) -> decltype(native_array_size_msemsearray(std::declval<U>()), native_array_size_msemsearray(std::declval<V>()), bool(true));
			template<typename, typename>
			static auto test(...)->std::false_type;

			using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
			static const bool value = type();
		};
		template<class T, class EqualTo = T>
		struct IsNativeArray_msemsearray : IsNativeArray_msemsearray_impl<
			mse::impl::remove_reference_t<T>, mse::impl::remove_reference_t<EqualTo> >::type {};

		template<typename T>
		struct is_std_array_strict : std::integral_constant<bool, false> {};
		template<class _Ty, size_t _Size>
		struct is_std_array_strict<std::array<_Ty, _Size> > : std::integral_constant<bool, true> {};
		template<typename _Ty>
		struct is_std_array : is_std_array_strict<mse::impl::remove_reference_t<_Ty> > {};

		template <typename _Ty> struct is_structure_lock_msemsearray : std::integral_constant<bool,
			(std::is_base_of<mse::us::impl::StructureLockTagBase, mse::impl::remove_reference_t<_Ty> >::value)> {};

		template <typename _Ty> struct is_static_structure_container : std::integral_constant<bool,
			(std::is_base_of<mse::us::impl::StaticStructureContainerTagBase, mse::impl::remove_reference_t<_Ty> >::value) || (is_std_array<_Ty>::value) || (IsNativeArray_msemsearray<_Ty>::value)> {};

		template<class _Ty, MSE_IMPL_EIP mse::impl::enable_if_t<(is_static_structure_container<_Ty>::value)> MSE_IMPL_EIS >
		void T_valid_if_is_static_structure_container() {}

		template <typename _Ty> struct is_lockable_structure_container : std::integral_constant<bool,
			(std::is_base_of<mse::us::impl::LockableStructureContainerTagBase, mse::impl::remove_reference_t<_Ty> >::value) || (is_std_array<_Ty>::value) || (IsNativeArray_msemsearray<_Ty>::value)> {};

		template<class _Ty, MSE_IMPL_EIP mse::impl::enable_if_t<(is_lockable_structure_container<_Ty>::value)> MSE_IMPL_EIS >
		void T_valid_if_is_lockable_structure_container() {}

		template <typename _Tx, typename _Ty> struct is_pointer_to_lockable_structure_container_helper1 : is_lockable_structure_container<mse::impl::remove_reference_t<decltype(*std::declval<_Ty>())> > {};
		template <typename _Ty> struct is_pointer_to_lockable_structure_container_helper1<std::false_type, _Ty> : std::false_type {};
		template <typename _Ty> struct is_pointer_to_lockable_structure_container : is_pointer_to_lockable_structure_container_helper1<typename mse::impl::IsDereferenceable_msemsearray<_Ty>::type, _Ty> {};

		template<class T, class EqualTo>
		struct IsSupportedByStdBegin_impl
		{
			template<class U, class V>
			static auto test(U* u) -> decltype(std::begin(*u), std::declval<V>(), bool(true));
			template<typename, typename>
			static auto test(...)->std::false_type;

			using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
		};
		template<class T, class EqualTo = T>
		struct IsSupportedByStdBegin : IsSupportedByStdBegin_impl<
			mse::impl::remove_reference_t<T>, mse::impl::remove_reference_t<EqualTo> >::type {};

		template <typename _Tx, typename _Ty> struct has_random_access_implicit_iterators_helper2 : std::false_type {};
		template <typename _Ty> struct has_random_access_implicit_iterators_helper2<std::true_type, _Ty> : std::integral_constant<bool,
			(std::is_same<std::random_access_iterator_tag, typename std::iterator_traits<decltype(std::begin(std::declval<_Ty>()))>::iterator_category>::value)> {};
		template <typename _Ty> struct has_random_access_implicit_iterators : has_random_access_implicit_iterators_helper2<typename IsSupportedByStdBegin<_Ty>::type, _Ty> {};

		template<class T, class EqualTo>
		struct HasOrInheritsDataMemberFunction_impl
		{
			template<class U, class V>
			static auto test(U* u) -> decltype((*u).data(), std::declval<V>(), bool(true));
			template<typename, typename>
			static auto test(...)->std::false_type;

			using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
		};
		template<class T, class EqualTo = T>
		struct HasOrInheritsDataMemberFunction : HasOrInheritsDataMemberFunction_impl<
			mse::impl::remove_reference_t<T>, mse::impl::remove_reference_t<EqualTo> >::type {};

		template <typename _Ty> struct is_contiguous_sequence_container : std::integral_constant<bool,
			(std::is_base_of<mse::us::impl::ContiguousSequenceContainerTagBase, mse::impl::remove_reference_t<_Ty> >::value) || (is_std_array<_Ty>::value)
			|| (IsNativeArray_msemsearray<_Ty>::value) || (has_random_access_implicit_iterators<_Ty>::value && HasOrInheritsDataMemberFunction<_Ty>::value)> {};

		template <typename _Ty> struct is_contiguous_sequence_static_structure_container_msemsearray : std::integral_constant<bool,
			(std::is_base_of<mse::us::impl::ContiguousSequenceContainerTagBase, mse::impl::remove_reference_t<_Ty> >::value && std::is_base_of<mse::us::impl::StaticStructureContainerTagBase, mse::impl::remove_reference_t<_Ty> >::value)
			|| (is_std_array<_Ty>::value) || (IsNativeArray_msemsearray<_Ty>::value)> {};

		template<class _Ty, MSE_IMPL_EIP mse::impl::enable_if_t<(is_contiguous_sequence_static_structure_container_msemsearray<_Ty>::value)> MSE_IMPL_EIS >
		void T_valid_if_is_contiguous_sequence_static_structure_container_msemsearray() {}

		template <typename _Ty> struct is_random_access_container : std::integral_constant<bool,
			(is_contiguous_sequence_container<_Ty>::value) || (has_random_access_implicit_iterators<_Ty>::value)> {};


#if defined(MSE_MSTDARRAY_DISABLED) || defined(MSE_MSTDVECTOR_DISABLED)
		typedef std::true_type some_safe_iterator_type_is_disabled;
#else // defined(MSE_MSTDARRAY_DISABLED) || defined(MSE_MSTDVECTOR_DISABLED)
		typedef std::false_type some_safe_iterator_type_is_disabled;
#endif // defined(MSE_MSTDARRAY_DISABLED) || defined(MSE_MSTDVECTOR_DISABLED)

		template <typename _TIterator> struct is_strong_container_reference_holding_iterator : std::integral_constant<bool,
			(std::is_base_of<mse::us::impl::StrongContainerReferenceHoldingIteratorTagBase, mse::impl::remove_reference_t<_TIterator> >::value)> {};

		template <typename _TIterator> struct is_static_structure_iterator : std::integral_constant<bool, some_safe_iterator_type_is_disabled::value ||
			(std::is_base_of<mse::us::impl::StaticStructureIteratorTagBase, mse::impl::remove_reference_t<_TIterator> >::value)> {};

		template <typename _TIterator> struct is_structure_locking_iterator : std::integral_constant<bool, some_safe_iterator_type_is_disabled::value ||
			(std::is_base_of<mse::us::impl::StructureLockingIteratorTagBase, mse::impl::remove_reference_t<_TIterator> >::value)> {};

		template <typename _TIterator> struct is_contiguous_sequence_iterator : std::integral_constant<bool, some_safe_iterator_type_is_disabled::value ||
			(std::is_base_of<mse::us::impl::ContiguousSequenceIteratorTagBase, mse::impl::remove_reference_t<_TIterator> >::value)> {};

		template <typename _TIterator> struct is_static_structure_strong_iterator_msemsearray : std::integral_constant<bool,
			is_static_structure_iterator<_TIterator>::value && is_strong_container_reference_holding_iterator<_TIterator>::value> {};

		template<class _TIterator, MSE_IMPL_EIP mse::impl::enable_if_t<(is_static_structure_strong_iterator_msemsearray<_TIterator>::value)> MSE_IMPL_EIS >
		void T_valid_if_is_static_structure_strong_iterator_msemsearray() {}

		template <typename _TIterator> struct is_static_structure_or_locking_strong_iterator_msemsearray : std::integral_constant<bool,
			is_strong_container_reference_holding_iterator<_TIterator>::value
			&& (is_static_structure_iterator<_TIterator>::value || is_structure_locking_iterator<_TIterator>::value)> {};

		template<class _TIterator, MSE_IMPL_EIP mse::impl::enable_if_t<(is_static_structure_or_locking_strong_iterator_msemsearray<_TIterator>::value)> MSE_IMPL_EIS >
		void T_valid_if_is_static_structure_or_locking_strong_iterator_msemsearray() {}

		template <typename _TIterator> struct is_contiguous_sequence_static_structure_strong_iterator_msemsearray : std::integral_constant<bool,
			is_contiguous_sequence_iterator<_TIterator>::value && is_static_structure_iterator<_TIterator>::value
			&& is_strong_container_reference_holding_iterator<_TIterator>::value> {};

		template<class _TIterator, MSE_IMPL_EIP mse::impl::enable_if_t<(is_contiguous_sequence_static_structure_strong_iterator_msemsearray<_TIterator>::value)> MSE_IMPL_EIS >
		void T_valid_if_is_contiguous_sequence_static_structure_strong_iterator_msemsearray() {}

		template <typename _TIterator> struct is_contiguous_sequence_static_structure_or_locking_strong_iterator_msemsearray : std::integral_constant<bool,
			is_contiguous_sequence_iterator<_TIterator>::value && is_strong_container_reference_holding_iterator<_TIterator>::value
			&& (is_static_structure_iterator<_TIterator>::value || is_structure_locking_iterator<_TIterator>::value)> {};

		template<class _TIterator, MSE_IMPL_EIP mse::impl::enable_if_t<(is_contiguous_sequence_static_structure_or_locking_strong_iterator_msemsearray<_TIterator>::value)> MSE_IMPL_EIS >
		void T_valid_if_is_contiguous_sequence_static_structure_or_locking_strong_iterator_msemsearray() {}
	}

	namespace impl {
		template<class _Ty>
		class random_access_iterator_base {
			typedef std::array<_Ty, 0> standard_t;
		public:
			using iterator_category = std::random_access_iterator_tag;
			using value_type = typename standard_t::value_type;
			//using difference_type = typename standard_t::difference_type;
			typedef msear_int difference_type;
			using pointer = typename standard_t::pointer;
			using reference = typename standard_t::reference;
			using const_pointer = typename standard_t::const_pointer;
			using const_reference = typename standard_t::const_reference;
			//using size_type = typename standard_t::size_type;
			typedef msear_size_t size_type;
		};
		template<class _Ty>
		class random_access_const_iterator_base {
		public:
			typedef random_access_iterator_base<_Ty> _Myt;
			using iterator_category = std::random_access_iterator_tag;
			using value_type = typename _Myt::value_type;
			using difference_type = typename _Myt::difference_type;
			using pointer = typename _Myt::const_pointer;
			using reference = typename _Myt::const_reference;
			using const_pointer = typename _Myt::const_pointer;
			using const_reference = typename _Myt::const_reference;
			using size_type = typename _Myt::size_type;
		};

#define MSE_INHERITED_RANDOM_ACCESS_MEMBER_TYPE_DECLARATIONS(base_class) \
	typedef typename base_class::value_type value_type; \
	typedef typename base_class::reference reference; \
	typedef typename base_class::const_reference const_reference; \
	typedef typename base_class::pointer pointer; \
	typedef typename base_class::const_pointer const_pointer; \
	typedef typename base_class::size_type size_type; \
	typedef typename base_class::difference_type difference_type; \
	typedef mse::impl::remove_const_t<value_type> nonconst_value_type;

#define MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class) \
	MSE_INHERITED_RANDOM_ACCESS_MEMBER_TYPE_DECLARATIONS(base_class); \
	typedef typename base_class::iterator_category iterator_category;


#define MSE_INHERIT_COMMON_ITERATOR_TAG_BASE_SET_FROM(class2, class3) \
	MSE_INHERIT_ASYNC_TAG_BASE_SET_FROM(class2, class3) \
	, public mse::impl::first_or_placeholder_if_not_base_of_second<mse::us::impl::StrongPointerTagBase, class2, class3> \
	, public mse::impl::first_or_placeholder_if_not_base_of_second<mse::us::impl::NeverNullTagBase, class2, class3> \
	, public mse::impl::first_or_placeholder_if_not_base_of_second<mse::us::impl::ExclusivePointerTagBase, class2, class3> \
	, public mse::impl::first_or_placeholder_if_not_base_of_second<mse::us::impl::StrongContainerReferenceHoldingIteratorTagBase, class2, class3> \
	, public mse::impl::first_or_placeholder_if_not_base_of_second<mse::us::impl::StaticStructureIteratorTagBase, class2, class3> \
	, public mse::impl::first_or_placeholder_if_not_base_of_second<mse::us::impl::StructureLockingIteratorTagBase, class2, class3> \
	, public mse::impl::first_or_placeholder_if_not_base_of_second<mse::us::impl::ContiguousSequenceIteratorTagBase, class2, class3>

#define MSE_INHERIT_COMMON_XSCOPE_ITERATOR_TAG_BASE_SET_FROM(class2, class3) \
	MSE_INHERIT_XSCOPE_TAG_BASE_SET_FROM(class2, class3) \
	, MSE_INHERIT_COMMON_ITERATOR_TAG_BASE_SET_FROM(class2, class3)

#define MSE_INHERIT_ITERATOR_ARITHMETIC_OPERATORS_FROM(base_class, this_class) \
	this_class& operator +=(difference_type x) { \
		base_class::operator +=(x); \
		return (*this); \
	} \
	this_class& operator -=(difference_type x) { operator +=(-x); return (*this); } \
	this_class& operator ++() { operator +=(1); return (*this); } \
	this_class operator ++(int) { auto _Tmp = *this; operator +=(1); return (_Tmp); } \
	this_class& operator --() { operator -=(1); return (*this); } \
	this_class operator --(int) { auto _Tmp = *this; operator -=(1); return (_Tmp); } \
	this_class operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; } \
	this_class operator-(difference_type n) const { return ((*this) + (-n)); } \
	difference_type operator-(const base_class& _Right_cref) const { \
		return base_class::operator-(_Right_cref); \
	}

	}

	namespace us {
		namespace impl {
			template <typename _TRAContainerPointer> class TRAConstIteratorBase;

			template <typename _TRAContainerPointer>
			class TRAIteratorBase : public mse::impl::random_access_iterator_base_from_ra_container<mse::impl::remove_reference_t<decltype(*std::declval<_TRAContainerPointer>())> >
				, std::conditional<mse::impl::is_strong_ptr<mse::impl::remove_reference_t<_TRAContainerPointer> >::value, mse::us::impl::StrongContainerReferenceHoldingIteratorTagBase, mse::impl::TPlaceHolder<mse::us::impl::StrongContainerReferenceHoldingIteratorTagBase, TRAIteratorBase<_TRAContainerPointer> > >::type
				, std::conditional<mse::impl::is_static_structure_container<mse::impl::remove_reference_t<decltype(*std::declval<_TRAContainerPointer>())> >::value, mse::us::impl::StaticStructureIteratorTagBase, mse::impl::TPlaceHolder<mse::us::impl::StaticStructureIteratorTagBase, TRAIteratorBase<_TRAContainerPointer> > >::type
				, std::conditional<mse::impl::is_contiguous_sequence_container<mse::impl::remove_reference_t<decltype(*std::declval<_TRAContainerPointer>())> >::value, mse::us::impl::ContiguousSequenceIteratorTagBase, mse::impl::TPlaceHolder<mse::us::impl::ContiguousSequenceIteratorTagBase, TRAIteratorBase<_TRAContainerPointer> > >::type
				, std::conditional<mse::impl::is_structure_lock_msemsearray<mse::impl::remove_reference_t<_TRAContainerPointer> >::value, mse::us::impl::StructureLockingIteratorTagBase, mse::impl::TPlaceHolder<mse::us::impl::StructureLockingIteratorTagBase, TRAIteratorBase<_TRAContainerPointer> > > ::type
				/* pointer tags (as iterators can be used as pointers )*/
				, std::conditional<mse::impl::is_strong_ptr<mse::impl::remove_reference_t<_TRAContainerPointer> >::value
					&& ((mse::impl::is_static_structure_container<mse::impl::remove_reference_t<decltype(*std::declval<_TRAContainerPointer>())> >::value)
						|| (mse::impl::is_structure_lock_msemsearray<mse::impl::remove_reference_t<_TRAContainerPointer> >::value))
					, mse::us::impl::StrongPointerTagBase, mse::impl::TPlaceHolder<mse::us::impl::StrongPointerTagBase, TRAIteratorBase<_TRAContainerPointer> > >::type
				, public mse::us::impl::NeverNullTagBase
				, public mse::impl::first_or_placeholder_if_not_base_of_second<mse::us::impl::ExclusivePointerTagBase, _TRAContainerPointer, TRAIteratorBase<_TRAContainerPointer> >
			{
			public:
				typedef mse::impl::random_access_iterator_base_from_ra_container<mse::impl::remove_reference_t<decltype(*std::declval<_TRAContainerPointer>())> > base_class;
				MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);
				typedef mse::impl::remove_reference_t<_TRAContainerPointer> _TRAContainerPointerRR;

			private:
				difference_type m_index = 0;
				/*const */_TRAContainerPointerRR m_ra_container_pointer;

				/* some construction helper functions and types */
				template<class _TRAIterator>
				static _TRAContainerPointerRR ra_container_pointer_from_lone_param(std::true_type, const _TRAIterator& src) { return _TRAContainerPointerRR(src.target_container_ptr()); }
				template<class _TRAIterator>
				static _TRAContainerPointerRR ra_container_pointer_from_lone_param(std::true_type, _TRAIterator&& src) { return _TRAContainerPointerRR(std::forward< _TRAIterator>(src).target_container_ptr()); }
				template<class _Ty2>
				static _TRAContainerPointerRR ra_container_pointer_from_lone_param(std::false_type, const _Ty2& param) { return _TRAContainerPointerRR(param); }
				template<class _Ty2>
				static _TRAContainerPointerRR ra_container_pointer_from_lone_param(std::false_type, _Ty2&& param) { return _TRAContainerPointerRR(std::forward<_Ty2>(param)); }
				template<class _TRAIterator>
				static auto index_from_lone_param(std::true_type, const _TRAIterator& src) { return src.position(); }
				template<class _Ty2>
				static difference_type index_from_lone_param(std::false_type, const _Ty2& param) { return 0; }

				template<class T, class EqualTo>
				struct lone_param_seems_valid_impl
				{
					template<class U, class V>
					static auto test(U* u) -> decltype(
						_TRAContainerPointerRR(ra_container_pointer_from_lone_param(typename mse::impl::HasOrInheritsTargetContainerPtrMethod_msemsearray<U>::type(), *u))
						, std::declval<V>(), bool(true));
					template<typename, typename>
					static auto test(...)->std::false_type;

					static const bool value = std::is_same<bool, decltype(test<T, EqualTo>(0))>::value;
					using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
				};
				template<class T, class EqualTo = T>
				struct lone_param_seems_valid : lone_param_seems_valid_impl<
					mse::impl::remove_reference_t<T>, mse::impl::remove_reference_t<EqualTo> >::type {};

			public:
				template<class _Ty2 = _TRAContainerPointerRR, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _TRAContainerPointerRR>::value) && (std::is_default_constructible<_Ty2>::value)> MSE_IMPL_EIS >
				TRAIteratorBase() {}

				TRAIteratorBase(const TRAIteratorBase& src) = default;
				TRAIteratorBase(TRAIteratorBase&& src) = default;
				//TRAIteratorBase(const _TRAContainerPointerRR& ra_container_pointer, size_type index = 0) : m_index(difference_type(mse::msear_as_a_size_t(index))), m_ra_container_pointer(ra_container_pointer) {}
				//TRAIteratorBase(_TRAContainerPointerRR&& ra_container_pointer, size_type index = 0) : m_index(difference_type(mse::msear_as_a_size_t(index))), m_ra_container_pointer(MSE_FWD(ra_container_pointer)) {}

				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2, _TRAContainerPointerRR>::value> MSE_IMPL_EIS >
				TRAIteratorBase(us::impl::TRAIteratorBase<_Ty2>&& src) : m_index(src.position()), m_ra_container_pointer(MSE_FWD(src).target_container_ptr()) {}
				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2, _TRAContainerPointerRR>::value> MSE_IMPL_EIS >
				TRAIteratorBase(const us::impl::TRAIteratorBase<_Ty2>& src) : m_index(src.position()), m_ra_container_pointer(src.target_container_ptr()) {}

				template<class _TRAContainerPointerRR2>
				TRAIteratorBase(const _TRAContainerPointerRR2& param, size_type index) : m_index(difference_type(mse::msear_as_a_size_t(index))), m_ra_container_pointer(param) {}
				template<class _TRAContainerPointerRR2>
				TRAIteratorBase(_TRAContainerPointerRR2&& param, size_type index) : m_index(difference_type(mse::msear_as_a_size_t(index))), m_ra_container_pointer(std::forward<_TRAContainerPointerRR2>(param)) {}

				template<class _TLoneParam, MSE_IMPL_EIP mse::impl::enable_if_t<lone_param_seems_valid<_TLoneParam>::value> MSE_IMPL_EIS >
				TRAIteratorBase(const _TLoneParam& param) : m_index(index_from_lone_param(typename mse::impl::HasOrInheritsTargetContainerPtrMethod_msemsearray<_TLoneParam>::type(), param))
					, m_ra_container_pointer(ra_container_pointer_from_lone_param(
					typename mse::impl::HasOrInheritsTargetContainerPtrMethod_msemsearray<_TLoneParam>::type(), param)) {}
				template<class _TLoneParam, MSE_IMPL_EIP mse::impl::enable_if_t<lone_param_seems_valid<_TLoneParam>::value
					&& (!std::is_base_of<TRAIteratorBase, _TLoneParam>::value)> MSE_IMPL_EIS >
				TRAIteratorBase(_TLoneParam&& param) : m_index(index_from_lone_param(typename mse::impl::HasOrInheritsTargetContainerPtrMethod_msemsearray<_TLoneParam>::type(), param))
					, m_ra_container_pointer(ra_container_pointer_from_lone_param(
					typename mse::impl::HasOrInheritsTargetContainerPtrMethod_msemsearray<_TLoneParam>::type(), std::forward<_TLoneParam>(param))) {}

				auto operator*() const -> reference {
					return (*m_ra_container_pointer)[m_index];
				}
				auto operator->() const -> typename std::add_pointer<value_type>::type {
					return std::addressof((*m_ra_container_pointer)[m_index]);
				}
				reference operator[](difference_type _Off) const { return (*m_ra_container_pointer)[m_index + _Off]; }
				TRAIteratorBase& operator +=(difference_type x) {
					m_index += (x);
					return (*this);
				}
				TRAIteratorBase& operator -=(difference_type x) { operator +=(-x); return (*this); }
				TRAIteratorBase& operator ++() { operator +=(1); return (*this); }
				TRAIteratorBase operator ++(int) { auto _Tmp = *this; operator +=(1); return (_Tmp); }
				TRAIteratorBase& operator --() { operator -=(1); return (*this); }
				TRAIteratorBase operator --(int) { auto _Tmp = *this; operator -=(1); return (_Tmp); }

				TRAIteratorBase operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
				TRAIteratorBase operator-(difference_type n) const { return ((*this) + (-n)); }
				difference_type operator-(const TRAIteratorBase& _Right_cref) const {
					if (!(_Right_cref.m_ra_container_pointer == m_ra_container_pointer)) { MSE_THROW(msearray_range_error("invalid argument - difference_type operator-() - TRAIteratorBase")); }
					return m_index - _Right_cref.m_index;
				}
				bool operator ==(const TRAIteratorBase& _Right_cref) const {
					if (!(_Right_cref.m_ra_container_pointer == m_ra_container_pointer)) { MSE_THROW(msearray_range_error("invalid argument - difference_type operator==() - TRAIteratorBase")); }
					return (_Right_cref.m_index == m_index);
				}
				bool operator !=(const TRAIteratorBase& _Right_cref) const { return !((*this) == _Right_cref); }
				bool operator<(const TRAIteratorBase& _Right_cref) const { return (0 > operator-(_Right_cref)); }
				bool operator>(const TRAIteratorBase& _Right_cref) const { return (0 > operator-(_Right_cref)); }
				bool operator<=(const TRAIteratorBase& _Right_cref) const { return (0 >= operator-(_Right_cref)); }
				bool operator>=(const TRAIteratorBase& _Right_cref) const { return (0 >= operator-(_Right_cref)); }

				TRAIteratorBase& operator=(const TRAIteratorBase& _Right_cref) {
					assignment_helper1(typename mse::impl::HasOrInheritsAssignmentOperator_msemsearray<_TRAContainerPointerRR>::type(), _Right_cref);
					return (*this);
				}
				TRAIteratorBase& operator=(TRAIteratorBase&& _Right_cref) {
					assignment_helper1(typename mse::impl::HasOrInheritsAssignmentOperator_msemsearray<_TRAContainerPointerRR>::type(), MSE_FWD(_Right_cref));
					return (*this);
				}
				/* This assignment operator accepts iterators of different types (but pointing to the same container) and
				attempts to obtain (and adopt) the given iterator's index position. */
				template<typename _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<(!std::is_convertible<_Ty2, TRAIteratorBase>::value) && (mse::impl::HasOrInheritsAssignmentOperator_msemsearray<_Ty2>::value)> MSE_IMPL_EIS >
				TRAIteratorBase& operator=(const _Ty2& _Right_cref) {
					assignment_helper1(std::false_type(), _Right_cref);
					return (*this);
				}

				void set_to_beginning() {
					m_index = 0;
				}
				difference_type position() const {
					return m_index;
				}
				_TRAContainerPointerRR target_container_ptr() const {
					return m_ra_container_pointer;
				}

				MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(_TRAContainerPointerRR);

			private:
				template<class _Ty2 = _TRAContainerPointerRR, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _TRAContainerPointerRR>::value)
					&& (mse::impl::HasOrInheritsAssignmentOperator_msemsearray<_Ty2>::value)> MSE_IMPL_EIS >
				void assignment_helper1(std::true_type, const TRAIteratorBase& _Right_cref) {
					((*this).m_ra_container_pointer) = _Right_cref.target_container_ptr();
					(*this).m_index = _Right_cref.position();
				}
				template<class _Ty2 = _TRAContainerPointerRR, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _TRAContainerPointerRR>::value)
					&& (mse::impl::HasOrInheritsAssignmentOperator_msemsearray<_Ty2>::value)> MSE_IMPL_EIS >
				void assignment_helper1(std::true_type, TRAIteratorBase&& _Right_cref) {
					(*this).m_index = _Right_cref.m_index;
					((*this).m_ra_container_pointer) = MSE_FWD(_Right_cref).m_ra_container_pointer;
				}
				template<typename _Ty2>
				void assignment_helper1(std::false_type, const _Ty2& _Right_cref) {
					if (((*this).m_ra_container_pointer) && (_Right_cref.target_container_ptr())) {
						const auto& this_m_ra_container_pointer_lvaluecref = *((*this).m_ra_container_pointer);
						const auto& Right_m_ra_container_pointer_lvaluecref = *(_Right_cref.target_container_ptr());
						if (std::addressof(this_m_ra_container_pointer_lvaluecref) != std::addressof(Right_m_ra_container_pointer_lvaluecref)
							/*|| (!std::is_same<mse::impl::remove_const_t<decltype(*((*this).m_ra_container_pointer))>, mse::impl::remove_const_t<decltype(*(_Right_cref.target_container_ptr()))> >::value)*/) {
							/* In cases where the container pointer type stored by this iterator doesn't support assignment (as with, for
							example, mse::TRegisteredFixedPointer<>), this iterator may only be assigned the value of another iterator
							pointing to the same container. */
							MSE_THROW(nii_array_range_error("invalid argument - TRAIteratorBase& operator=(const TRAIteratorBase& _Right) - TRAIteratorBase"));
						}
					}
					(*this).m_index = _Right_cref.position();
				}

				friend class TRAConstIteratorBase<_TRAContainerPointerRR>;
			};
		}
	}

	template <typename _TRAContainerPointer>
	class TXScopeRAIterator : public us::impl::TRAIteratorBase<mse::impl::remove_reference_t<_TRAContainerPointer> >, public mse::us::impl::XScopeTagBase {
	public:
		typedef us::impl::TRAIteratorBase<mse::impl::remove_reference_t<_TRAContainerPointer> > base_class;
		typedef mse::impl::remove_reference_t<_TRAContainerPointer> _TRAContainerPointerRR;
		MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class)

		MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TXScopeRAIterator, base_class);

		TXScopeRAIterator& operator +=(difference_type x) {
			base_class::operator +=(x);
			return (*this);
		}
		TXScopeRAIterator& operator -=(difference_type x) { operator +=(-x); return (*this); }
		TXScopeRAIterator& operator ++() { operator +=(1); return (*this); }
		TXScopeRAIterator operator ++(int) { auto _Tmp = *this; operator +=(1); return (_Tmp); }
		TXScopeRAIterator& operator --() { operator -=(1); return (*this); }
		TXScopeRAIterator operator --(int) { auto _Tmp = *this; operator -=(1); return (_Tmp); }

		TXScopeRAIterator operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
		TXScopeRAIterator operator-(difference_type n) const { return ((*this) + (-n)); }
		difference_type operator-(const base_class& _Right_cref) const {
			return base_class::operator-(_Right_cref);
		}

		TXScopeRAIterator& operator=(const TXScopeRAIterator& _Right_cref) {
			base_class::operator=(_Right_cref);
			return (*this);
		}
		TXScopeRAIterator& operator=(TXScopeRAIterator&& _Right_cref) {
			base_class::operator=(MSE_FWD(_Right_cref));
			return (*this);
		}
		MSE_USING_ASSIGNMENT_OPERATOR(base_class);

		MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(_TRAContainerPointerRR);
		void xscope_iterator_tag() const {}
		void xscope_tag() const {}

	private:
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
	};

	template <typename _TRAContainerPointer>
	class TRAIterator : public us::impl::TRAIteratorBase<mse::impl::remove_reference_t<_TRAContainerPointer> > {
	public:
		typedef us::impl::TRAIteratorBase<mse::impl::remove_reference_t<_TRAContainerPointer> > base_class;
		typedef mse::impl::remove_reference_t<_TRAContainerPointer> _TRAContainerPointerRR;
		MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class)

		template<class _Ty2 = base_class, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, base_class>::value) && (std::is_default_constructible<_Ty2>::value)> MSE_IMPL_EIS >
		TRAIterator() : base_class() {}

		TRAIterator(const TRAIterator& src) = default;
		TRAIterator(TRAIterator&& src) = default;

		template <typename _TRAContainerPointer1>
		TRAIterator(const _TRAContainerPointer1& ra_container_pointer, size_type index) : base_class(ra_container_pointer, index) {}
		template <typename _TRAContainerPointer1>
		TRAIterator(_TRAContainerPointer1&& ra_container_pointer, size_type index) : base_class(std::forward<_TRAContainerPointer1>(ra_container_pointer), index) {}
		template <typename _TLoneParam>
		TRAIterator(const _TLoneParam& lone_param) : base_class(lone_param) {}
		template <typename _TLoneParam, MSE_IMPL_EIP mse::impl::enable_if_t<(!std::is_base_of<TRAIterator, _TLoneParam>::value)> MSE_IMPL_EIS >
		TRAIterator(_TLoneParam&& lone_param) : base_class(std::forward<_TLoneParam>(lone_param)) {}

		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TRAIterator() {
			mse::impl::T_valid_if_not_an_xscope_type<_TRAContainerPointerRR>();
		}

		TRAIterator& operator +=(difference_type x) {
			base_class::operator +=(x);
			return (*this);
		}
		TRAIterator& operator -=(difference_type x) { operator +=(-x); return (*this); }
		TRAIterator& operator ++() { operator +=(1); return (*this); }
		TRAIterator& operator ++(int) { auto _Tmp = *this; operator +=(1); return (_Tmp); }
		TRAIterator& operator --() { operator -=(1); return (*this); }
		TRAIterator& operator --(int) { auto _Tmp = *this; operator -=(1); return (_Tmp); }

		TRAIterator operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
		TRAIterator operator-(difference_type n) const { return ((*this) + (-n)); }
		difference_type operator-(const base_class& _Right_cref) const {
			return base_class::operator-(_Right_cref);
		}

		TRAIterator& operator=(const TRAIterator& _Right_cref) {
			base_class::operator=(_Right_cref);
			return (*this);
		}
		TRAIterator& operator=(TRAIterator&& _Right_cref) {
			base_class::operator=(MSE_FWD(_Right_cref));
			return (*this);
		}
		MSE_USING_ASSIGNMENT_OPERATOR(base_class);

		MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(_TRAContainerPointerRR);
		void iterator_tag() const {}

	private:
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;
	};

	namespace us {
		namespace impl {
			template <typename _TRAContainerPointer>
			class TRAConstIteratorBase : public mse::impl::random_access_const_iterator_base_from_ra_container<mse::impl::remove_reference_t<decltype(*std::declval<_TRAContainerPointer>())> >
				, std::conditional<mse::impl::is_strong_ptr<mse::impl::remove_reference_t<_TRAContainerPointer> >::value, mse::us::impl::StrongContainerReferenceHoldingIteratorTagBase, mse::impl::TPlaceHolder<mse::us::impl::StrongContainerReferenceHoldingIteratorTagBase, TRAIteratorBase<_TRAContainerPointer> > >::type
				, std::conditional<mse::impl::is_static_structure_container<mse::impl::remove_reference_t<decltype(*std::declval<_TRAContainerPointer>())> >::value, mse::us::impl::StaticStructureIteratorTagBase, mse::impl::TPlaceHolder<mse::us::impl::StaticStructureIteratorTagBase, TRAIteratorBase<_TRAContainerPointer> > >::type
				, std::conditional<mse::impl::is_contiguous_sequence_container<mse::impl::remove_reference_t<decltype(*std::declval<_TRAContainerPointer>())> >::value, mse::us::impl::ContiguousSequenceIteratorTagBase, mse::impl::TPlaceHolder<mse::us::impl::ContiguousSequenceIteratorTagBase, TRAIteratorBase<_TRAContainerPointer> > >::type
				, std::conditional<mse::impl::is_structure_lock_msemsearray<mse::impl::remove_reference_t<_TRAContainerPointer> >::value, mse::us::impl::StructureLockingIteratorTagBase, mse::impl::TPlaceHolder<mse::us::impl::StructureLockingIteratorTagBase, TRAIteratorBase<_TRAContainerPointer> > > ::type
				/* pointer tags (as iterators can be used as pointers )*/
				, std::conditional<mse::impl::is_strong_ptr<mse::impl::remove_reference_t<_TRAContainerPointer> >::value
					&& ((mse::impl::is_static_structure_container<mse::impl::remove_reference_t<decltype(*std::declval<_TRAContainerPointer>())> >::value)
						|| (mse::impl::is_structure_lock_msemsearray<mse::impl::remove_reference_t<_TRAContainerPointer> >::value))
					, mse::us::impl::StrongPointerTagBase, mse::impl::TPlaceHolder<mse::us::impl::StrongPointerTagBase, TRAIteratorBase<_TRAContainerPointer> > >::type
				, public mse::us::impl::NeverNullTagBase
				, public mse::impl::first_or_placeholder_if_not_base_of_second<mse::us::impl::ExclusivePointerTagBase, _TRAContainerPointer, TRAIteratorBase<_TRAContainerPointer> >
			{
			public:
				typedef mse::impl::random_access_const_iterator_base_from_ra_container<mse::impl::remove_reference_t<decltype(*std::declval<_TRAContainerPointer>())> > base_class;
				MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);
				typedef mse::impl::remove_reference_t<_TRAContainerPointer> _TRAContainerPointerRR;

			private:
				difference_type m_index = 0;
				/*const */_TRAContainerPointerRR m_ra_container_pointer;

				/* some construction helper functions and types */
				template<class _TRAIterator>
				static _TRAContainerPointerRR ra_container_pointer_from_lone_param(std::true_type, const _TRAIterator& src) { return _TRAContainerPointerRR(src.target_container_ptr()); }
				template<class _TRAIterator>
				static _TRAContainerPointerRR ra_container_pointer_from_lone_param(std::true_type, _TRAIterator&& src) { return _TRAContainerPointerRR(std::forward< _TRAIterator>(src).target_container_ptr()); }
				template<class _Ty2>
				static _TRAContainerPointerRR ra_container_pointer_from_lone_param(std::false_type, const _Ty2& param) { return _TRAContainerPointerRR(param); }
				template<class _Ty2>
				static _TRAContainerPointerRR ra_container_pointer_from_lone_param(std::false_type, _Ty2&& param) { return _TRAContainerPointerRR(std::forward<_Ty2>(param)); }
				template<class _TRAIterator>
				static auto index_from_lone_param(std::true_type, const _TRAIterator& src) { return src.position(); }
				template<class _Ty2>
				static difference_type index_from_lone_param(std::false_type, const _Ty2& param) { return 0; }

				template<class T, class EqualTo>
				struct lone_param_seems_valid_impl
				{
					template<class U, class V>
					static auto test(U* u) -> decltype(
						_TRAContainerPointerRR(ra_container_pointer_from_lone_param(typename mse::impl::HasOrInheritsTargetContainerPtrMethod_msemsearray<U>::type(), *u))
						, std::declval<V>(), bool(true));
					template<typename, typename>
					static auto test(...)->std::false_type;

					static const bool value = std::is_same<bool, decltype(test<T, EqualTo>(0))>::value;
					using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
				};
				template<class T, class EqualTo = T>
				struct lone_param_seems_valid : lone_param_seems_valid_impl<
					mse::impl::remove_reference_t<T>, mse::impl::remove_reference_t<EqualTo> >::type {};

			public:
				template<class _Ty2 = _TRAContainerPointerRR, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _TRAContainerPointerRR>::value) && (std::is_default_constructible<_Ty2>::value)> MSE_IMPL_EIS >
				TRAConstIteratorBase() {}

				TRAConstIteratorBase(const TRAConstIteratorBase& src) = default;
				TRAConstIteratorBase(TRAConstIteratorBase&& src) = default;
				TRAConstIteratorBase(const TRAIteratorBase<_TRAContainerPointerRR>& src) : m_index(src.m_index), m_ra_container_pointer(src.m_ra_container_pointer) {}
				TRAConstIteratorBase(const TRAIteratorBase<_TRAContainerPointerRR>&& src) : m_index(src.m_index), m_ra_container_pointer(MSE_FWD(src).m_ra_container_pointer) {}
				//TRAConstIteratorBase(const _TRAContainerPointerRR& ra_container_pointer, size_type index = 0) : m_index(difference_type(mse::msear_as_a_size_t(index))), m_ra_container_pointer(ra_container_pointer) {}
				//TRAConstIteratorBase(_TRAContainerPointerRR&& ra_container_pointer, size_type index = 0) : m_index(difference_type(mse::msear_as_a_size_t(index))), m_ra_container_pointer(MSE_FWD(ra_container_pointer)) {}

				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2, _TRAContainerPointerRR>::value> MSE_IMPL_EIS >
				TRAConstIteratorBase(us::impl::TRAConstIteratorBase<_Ty2>&& src) : m_index(src.position()), m_ra_container_pointer(MSE_FWD(src).target_container_ptr()) {}
				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2, _TRAContainerPointerRR>::value> MSE_IMPL_EIS >
				TRAConstIteratorBase(const us::impl::TRAConstIteratorBase<_Ty2>& src) : m_index(src.position()), m_ra_container_pointer(src.target_container_ptr()) {}

				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2, _TRAContainerPointerRR>::value> MSE_IMPL_EIS >
				TRAConstIteratorBase(us::impl::TRAIteratorBase<_Ty2>&& src) : m_index(src.position()), m_ra_container_pointer(MSE_FWD(src).target_container_ptr()) {}
				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2, _TRAContainerPointerRR>::value> MSE_IMPL_EIS >
				TRAConstIteratorBase(const us::impl::TRAIteratorBase<_Ty2>& src) : m_index(src.position()), m_ra_container_pointer(src.target_container_ptr()) {}

				template<class _TRAContainerPointerRR2>
				TRAConstIteratorBase(const _TRAContainerPointerRR2& param, size_type index) : m_index(difference_type(mse::msear_as_a_size_t(index))), m_ra_container_pointer(param) {}
				template<class _TRAContainerPointerRR2>
				TRAConstIteratorBase(_TRAContainerPointerRR2&& param, size_type index) : m_index(difference_type(mse::msear_as_a_size_t(index))), m_ra_container_pointer(std::forward<_TRAContainerPointerRR2>(param)) {}

				template<class _TLoneParam, MSE_IMPL_EIP mse::impl::enable_if_t<lone_param_seems_valid<_TLoneParam>::value> MSE_IMPL_EIS >
				TRAConstIteratorBase(const _TLoneParam& param) : m_index(index_from_lone_param(typename mse::impl::HasOrInheritsTargetContainerPtrMethod_msemsearray<_TLoneParam>::type(), param))
					, m_ra_container_pointer(ra_container_pointer_from_lone_param(
					typename mse::impl::HasOrInheritsTargetContainerPtrMethod_msemsearray<_TLoneParam>::type(), param)) {}
				template<class _TLoneParam, MSE_IMPL_EIP mse::impl::enable_if_t<lone_param_seems_valid<_TLoneParam>::value
					&& (!std::is_base_of<TRAConstIteratorBase, _TLoneParam>::value)> MSE_IMPL_EIS >
				TRAConstIteratorBase(_TLoneParam&& param) : m_index(index_from_lone_param(typename mse::impl::HasOrInheritsTargetContainerPtrMethod_msemsearray<_TLoneParam>::type(), param))
					, m_ra_container_pointer(ra_container_pointer_from_lone_param(
					typename mse::impl::HasOrInheritsTargetContainerPtrMethod_msemsearray<_TLoneParam>::type(), std::forward<_TLoneParam>(param))) {}

				auto operator*() const -> const_reference {
					return (*m_ra_container_pointer)[m_index];
				}
				auto operator->() const -> typename std::add_pointer<typename std::add_const<value_type>::type>::type {
					return std::addressof((*m_ra_container_pointer)[m_index]);
				}
				const_reference operator[](difference_type _Off) const { return (*m_ra_container_pointer)[(m_index + _Off)]; }
				TRAConstIteratorBase& operator +=(difference_type x) {
					m_index += (x);
					return (*this);
				}
				TRAConstIteratorBase& operator -=(difference_type x) { operator +=(-x); return (*this); }
				TRAConstIteratorBase& operator ++() { operator +=(1); return (*this); }
				TRAConstIteratorBase operator ++(int) { auto _Tmp = *this; operator +=(1); return (_Tmp); }
				TRAConstIteratorBase& operator --() { operator -=(1); return (*this); }
				TRAConstIteratorBase operator --(int) { auto _Tmp = *this; operator -=(1); return (_Tmp); }

				TRAConstIteratorBase operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
				TRAConstIteratorBase operator-(difference_type n) const { return ((*this) + (-n)); }
				difference_type operator-(const TRAConstIteratorBase& _Right_cref) const {
					if (!(_Right_cref.m_ra_container_pointer == m_ra_container_pointer)) { MSE_THROW(msearray_range_error("invalid argument - difference_type operator-() - TRAConstIteratorBase")); }
					return m_index - _Right_cref.m_index;
				}
				bool operator ==(const TRAConstIteratorBase& _Right_cref) const {
					if (!(_Right_cref.m_ra_container_pointer == m_ra_container_pointer)) { MSE_THROW(msearray_range_error("invalid argument - difference_type operator==() - TRAConstIteratorBase")); }
					return (_Right_cref.m_index == m_index);
				}
				bool operator !=(const TRAConstIteratorBase& _Right_cref) const { return !((*this) == _Right_cref); }
				bool operator<(const TRAConstIteratorBase& _Right_cref) const { return (0 > operator-(_Right_cref)); }
				bool operator>(const TRAConstIteratorBase& _Right_cref) const { return (0 > operator-(_Right_cref)); }
				bool operator<=(const TRAConstIteratorBase& _Right_cref) const { return (0 >= operator-(_Right_cref)); }
				bool operator>=(const TRAConstIteratorBase& _Right_cref) const { return (0 >= operator-(_Right_cref)); }

				TRAConstIteratorBase& operator=(const TRAConstIteratorBase& _Right_cref) {
					assignment_helper1(typename mse::impl::HasOrInheritsAssignmentOperator_msemsearray<_TRAContainerPointerRR>::type(), _Right_cref);
					return (*this);
				}
				TRAConstIteratorBase& operator=(TRAConstIteratorBase&& _Right_cref) {
					assignment_helper1(typename mse::impl::HasOrInheritsAssignmentOperator_msemsearray<_TRAContainerPointerRR>::type(), MSE_FWD(_Right_cref));
					return (*this);
				}
				/* This assignment operator accepts iterators of different types (but pointing to the same container) and
				attempts to obtain (and adopt) the given iterator's index position. */
				template<typename _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<(!std::is_convertible<_Ty2, TRAConstIteratorBase>::value) && (mse::impl::HasOrInheritsAssignmentOperator_msemsearray<_Ty2>::value)> MSE_IMPL_EIS >
				TRAConstIteratorBase& operator=(const _Ty2& _Right_cref) {
					assignment_helper1(std::false_type(), _Right_cref);
					return (*this);
				}

				void set_to_beginning() {
					m_index = 0;
				}
				difference_type position() const {
					return m_index;
				}
				_TRAContainerPointerRR target_container_ptr() const {
					return m_ra_container_pointer;
				}

			private:
				template<class _Ty2 = _TRAContainerPointerRR, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _TRAContainerPointerRR>::value)
					&& (mse::impl::HasOrInheritsAssignmentOperator_msemsearray<_Ty2>::value)> MSE_IMPL_EIS >
				void assignment_helper1(std::true_type, const TRAConstIteratorBase& _Right_cref) {
					((*this).m_ra_container_pointer) = _Right_cref.target_container_ptr();
					(*this).m_index = _Right_cref.position();
				}
				template<class _Ty2 = _TRAContainerPointerRR, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _TRAContainerPointerRR>::value)
					&& (mse::impl::HasOrInheritsAssignmentOperator_msemsearray<_Ty2>::value)> MSE_IMPL_EIS >
				void assignment_helper1(std::true_type, TRAConstIteratorBase&& _Right_cref) {
					(*this).m_index = _Right_cref.m_index;
					((*this).m_ra_container_pointer) = MSE_FWD(_Right_cref).m_ra_container_pointer;
				}
				template<typename _Ty2>
				void assignment_helper1(std::false_type, const _Ty2& _Right_cref) {
					if (((*this).m_ra_container_pointer) && (_Right_cref.target_container_ptr())) {
						const auto& this_m_ra_container_pointer_lvaluecref = *((*this).m_ra_container_pointer);
						const auto& Right_m_ra_container_pointer_lvaluecref = *(_Right_cref.target_container_ptr());
						if (std::addressof(this_m_ra_container_pointer_lvaluecref) != std::addressof(Right_m_ra_container_pointer_lvaluecref)
							/*|| (!std::is_same<mse::impl::remove_const_t<decltype(*((*this).m_ra_container_pointer))>, mse::impl::remove_const_t<decltype(*(_Right_cref.target_container_ptr()))> >::value)*/) {
							/* In cases where the container pointer type stored by this iterator doesn't support assignment (as with, for
							example, mse::TRegisteredFixedPointer<>), this iterator may only be assigned the value of another iterator
							pointing to the same container. */
							MSE_THROW(nii_array_range_error("invalid argument - TRAConstIteratorBase& operator=(const TRAConstIteratorBase& _Right) - TRAConstIteratorBase"));
						}
					}
					(*this).m_index = _Right_cref.position();
				}
			};
		}
	}

	template <typename _TRAContainerPointer>
	class TXScopeRAConstIterator : public us::impl::TRAConstIteratorBase<mse::impl::remove_reference_t<_TRAContainerPointer> >, public mse::us::impl::XScopeTagBase {
	public:
		typedef us::impl::TRAConstIteratorBase<mse::impl::remove_reference_t<_TRAContainerPointer> > base_class;
		typedef mse::impl::remove_reference_t<_TRAContainerPointer> _TRAContainerPointerRR;
		MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class)

		MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TXScopeRAConstIterator, base_class);

		TXScopeRAConstIterator& operator +=(difference_type x) {
			base_class::operator +=(x);
			return (*this);
		}
		TXScopeRAConstIterator& operator -=(difference_type x) { operator +=(-x); return (*this); }
		TXScopeRAConstIterator& operator ++() { operator +=(1); return (*this); }
		TXScopeRAConstIterator operator ++(int) { auto _Tmp = *this; operator +=(1); return (_Tmp); }
		TXScopeRAConstIterator& operator --() { operator -=(1); return (*this); }
		TXScopeRAConstIterator operator --(int) { auto _Tmp = *this; operator -=(1); return (_Tmp); }

		TXScopeRAConstIterator operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
		TXScopeRAConstIterator operator-(difference_type n) const { return ((*this) + (-n)); }
		difference_type operator-(const base_class& _Right_cref) const {
			return base_class::operator-(_Right_cref);
		}

		TXScopeRAConstIterator& operator=(const TXScopeRAConstIterator& _Right_cref) {
			base_class::operator=(_Right_cref);
			return (*this);
		}
		TXScopeRAConstIterator& operator=(TXScopeRAConstIterator&& _Right_cref) {
			base_class::operator=(MSE_FWD(_Right_cref));
			return (*this);
		}
		MSE_USING_ASSIGNMENT_OPERATOR(base_class);

		MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(_TRAContainerPointerRR);
		void xscope_iterator_tag() const {}
		void xscope_tag() const {}

	private:
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
	};

	template <typename _TRAContainerPointer>
	class TRAConstIterator : public us::impl::TRAConstIteratorBase<mse::impl::remove_reference_t<_TRAContainerPointer> > {
	public:
		typedef us::impl::TRAConstIteratorBase<mse::impl::remove_reference_t<_TRAContainerPointer> > base_class;
		typedef mse::impl::remove_reference_t<_TRAContainerPointer> _TRAContainerPointerRR;
		MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class)

		template<class _Ty2 = base_class, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, base_class>::value) && (std::is_default_constructible<_Ty2>::value)> MSE_IMPL_EIS >
		TRAConstIterator() : base_class() {}

		TRAConstIterator(const TRAConstIterator& src) = default;
		TRAConstIterator(TRAConstIterator&& src) = default;
		TRAConstIterator(const TRAIterator<_TRAContainerPointer>& src) : base_class(src) {}
		TRAConstIterator(TRAIterator<_TRAContainerPointer>&& src) : base_class(MSE_FWD(src)) {}

		template <typename _TRAContainerPointer1>
		TRAConstIterator(const _TRAContainerPointer1& ra_container_pointer, size_type index) : base_class(ra_container_pointer, index) {}
		template <typename _TRAContainerPointer1>
		TRAConstIterator(_TRAContainerPointer1&& ra_container_pointer, size_type index) : base_class(std::forward<_TRAContainerPointer1>(ra_container_pointer), index) {}
		template <typename _TLoneParam>
		TRAConstIterator(const _TLoneParam& lone_param) : base_class(lone_param) {}
		template <typename _TLoneParam, MSE_IMPL_EIP mse::impl::enable_if_t<(!std::is_base_of<TRAConstIterator, _TLoneParam>::value)> MSE_IMPL_EIS >
		TRAConstIterator(_TLoneParam&& lone_param) : base_class(std::forward<_TLoneParam>(lone_param)) {}

		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TRAConstIterator() {
			mse::impl::T_valid_if_not_an_xscope_type<_TRAContainerPointerRR>();
		}

		TRAConstIterator& operator +=(difference_type x) {
			base_class::operator +=(x);
			return (*this);
		}
		TRAConstIterator& operator -=(difference_type x) { operator +=(-x); return (*this); }
		TRAConstIterator& operator ++() { operator +=(1); return (*this); }
		TRAConstIterator operator ++(int) { auto _Tmp = *this; operator +=(1); return (_Tmp); }
		TRAConstIterator& operator --() { operator -=(1); return (*this); }
		TRAConstIterator operator --(int) { auto _Tmp = *this; operator -=(1); return (_Tmp); }

		TRAConstIterator operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
		TRAConstIterator operator-(difference_type n) const { return ((*this) + (-n)); }
		difference_type operator-(const base_class& _Right_cref) const {
			return base_class::operator-(_Right_cref);
		}

		TRAConstIterator& operator=(const TRAConstIterator& _Right_cref) {
			base_class::operator=(_Right_cref);
			return (*this);
		}
		TRAConstIterator& operator=(TRAConstIterator&& _Right_cref) {
			base_class::operator=(MSE_FWD(_Right_cref));
			return (*this);
		}
		MSE_USING_ASSIGNMENT_OPERATOR(base_class);

		MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(_TRAContainerPointerRR);
		void iterator_tag() const {}

	private:
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;
	};

	template <typename _TRAContainerPointer>
	auto xscope_pointer(const TXScopeRAIterator<_TRAContainerPointer>& iter_cref) {
		mse::impl::T_valid_if_is_static_structure_or_locking_strong_iterator_msemsearray<TXScopeRAIterator<_TRAContainerPointer> >();
		return mse::us::unsafe_make_xscope_pointer_to(*iter_cref);
	}
	template <typename _TRAContainerPointer>
	auto xscope_pointer(const TXScopeRAConstIterator<_TRAContainerPointer>& iter_cref) {
		mse::impl::T_valid_if_is_static_structure_or_locking_strong_iterator_msemsearray<TXScopeRAConstIterator<_TRAContainerPointer> >();
		return mse::us::unsafe_make_xscope_const_pointer_to(*iter_cref);
	}
	template <typename _TRAContainerPointer>
	auto xscope_const_pointer(const TXScopeRAIterator<_TRAContainerPointer>& iter_cref) {
		mse::impl::T_valid_if_is_static_structure_or_locking_strong_iterator_msemsearray<TXScopeRAIterator<_TRAContainerPointer> >();
		return mse::us::unsafe_make_xscope_const_pointer_to(*iter_cref);
	}
	template <typename _TRAContainerPointer>
	auto xscope_const_pointer(const TXScopeRAConstIterator<_TRAContainerPointer>& iter_cref) {
		mse::impl::T_valid_if_is_static_structure_or_locking_strong_iterator_msemsearray<TXScopeRAConstIterator<_TRAContainerPointer> >();
		return mse::us::unsafe_make_xscope_const_pointer_to(*iter_cref);
	}

	template <typename _TRAContainerPointer>
	auto xscope_pointer(TXScopeRAIterator<_TRAContainerPointer>&& iter) {
		mse::impl::T_valid_if_is_static_structure_strong_iterator_msemsearray<TXScopeRAIterator<_TRAContainerPointer> >();
		return mse::us::unsafe_make_xscope_pointer_to(*iter);
	}
	template <typename _TRAContainerPointer>
	auto xscope_pointer(TXScopeRAConstIterator<_TRAContainerPointer>&& iter) {
		mse::impl::T_valid_if_is_static_structure_strong_iterator_msemsearray<TXScopeRAConstIterator<_TRAContainerPointer> >();
		return mse::us::unsafe_make_xscope_const_pointer_to(*iter);
	}
	template <typename _TRAContainerPointer>
	auto xscope_const_pointer(TXScopeRAIterator<_TRAContainerPointer>&& iter) {
		mse::impl::T_valid_if_is_static_structure_strong_iterator_msemsearray<TXScopeRAIterator<_TRAContainerPointer> >();
		return mse::us::unsafe_make_xscope_const_pointer_to(*iter);
	}
	template <typename _TRAContainerPointer>
	auto xscope_const_pointer(TXScopeRAConstIterator<_TRAContainerPointer>&& iter) {
		mse::impl::T_valid_if_is_static_structure_strong_iterator_msemsearray<TXScopeRAConstIterator<_TRAContainerPointer> >();
		return mse::us::unsafe_make_xscope_const_pointer_to(*iter);
	}

	template <typename _TRAIterator>
	class TFriendlyAugmentedRAIterator : public _TRAIterator {
	public:
		typedef _TRAIterator base_class;
		MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

		template<class _Ty2 = base_class, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, base_class>::value) && (std::is_default_constructible<_Ty2>::value)> MSE_IMPL_EIS >
		TFriendlyAugmentedRAIterator() : base_class() {}

		MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TFriendlyAugmentedRAIterator, base_class);

		auto& operator=(const TFriendlyAugmentedRAIterator& _Right_cref) { base_class::operator=(_Right_cref); return (*this); }
		auto& operator=(TFriendlyAugmentedRAIterator&& _Right_cref) { base_class::operator=(MSE_FWD(_Right_cref)); return (*this); }
		MSE_USING_ASSIGNMENT_OPERATOR(base_class);

		void assert_valid_index() const {
			if (difference_type((*this).target_container_ptr()->size() - 0) < (*this).position()) { MSE_THROW(nii_array_range_error("invalid index - void assert_valid_index() const - TFriendlyAugmentedRAIterator")); }
		}
		void reset() { set_to_end_marker(); }
		bool points_to_an_item() const {
			if (difference_type((*this).target_container_ptr()->size() - 0) > (*this).position()) { return true; }
			else {
				assert((*this).position() == difference_type((*this).target_container_ptr()->size() - 0));
				return false;
			}
		}
		bool points_to_end_marker() const {
			if (false == points_to_an_item()) {
				assert((*this).position() == difference_type((*this).target_container_ptr()->size() - 0));
				return true;
			}
			else { return false; }
		}
		bool points_to_beginning() const {
			if (0 == (*this).position()) { return true; }
			else { return false; }
		}
		/* has_next_item_or_end_marker() is just an alias for points_to_an_item(). */
		bool has_next_item_or_end_marker() const { return points_to_an_item(); }
		/* has_next() is just an alias for points_to_an_item() that's familiar to java programmers. */
		bool has_next() const { return has_next_item_or_end_marker(); }
		bool has_previous() const {
			return ((1 <= (*this).target_container_ptr()->size()) && (!points_to_beginning()));
		}
		void set_to_end_marker() {
			(*this).set_to_beginning();
			advance(difference_type((*this).target_container_ptr()->size() - 0));
		}
		void set_to_next() {
			if (points_to_an_item()) {
				advance(1);
				assert(difference_type((*this).target_container_ptr()->size() - 0) >= (*this).position());
			}
			else {
				MSE_THROW(nii_array_range_error("attempt to use invalid item_pointer - void set_to_next() - TFriendlyAugmentedRAIterator"));
			}
		}
		void set_to_previous() {
			if (has_previous()) {
				regress(1);
			}
			else {
				MSE_THROW(nii_array_range_error("attempt to use invalid item_pointer - void set_to_previous() - TFriendlyAugmentedRAIterator"));
			}
		}
		TFriendlyAugmentedRAIterator& operator ++() { (*this).set_to_next(); return (*this); }
		TFriendlyAugmentedRAIterator operator++(int) { TFriendlyAugmentedRAIterator _Tmp = *this; (*this).set_to_next(); return (_Tmp); }
		TFriendlyAugmentedRAIterator& operator --() { (*this).set_to_previous(); return (*this); }
		TFriendlyAugmentedRAIterator operator--(int) { TFriendlyAugmentedRAIterator _Tmp = *this; (*this).set_to_previous(); return (_Tmp); }
		void advance(difference_type n) {
			auto new_index = msear_int((*this).position()) + n;
			if ((0 > new_index) || ((*this).target_container_ptr()->size() < msear_size_t(new_index))) {
				MSE_THROW(nii_array_range_error("index out of range - void advance(difference_type n) - TFriendlyAugmentedRAIterator"));
			}
			else {
				base_class::operator+=(n);
			}
		}
		void regress(difference_type n) { advance(-n); }
		TFriendlyAugmentedRAIterator& operator +=(difference_type n) { (*this).advance(n); return (*this); }
		TFriendlyAugmentedRAIterator& operator -=(difference_type n) { (*this).regress(n); return (*this); }
		TFriendlyAugmentedRAIterator operator+(difference_type n) const {
			TFriendlyAugmentedRAIterator retval(*this);
			retval.advance(n);
			return retval;
		}
		TFriendlyAugmentedRAIterator operator-(difference_type n) const { return ((*this) + (-n)); }
		difference_type operator-(const base_class& _Right_cref) const {
			return base_class::operator-(_Right_cref);
		}
		reference item() const { return (*this).operator*(); }
		reference previous_item() const {
			if ((*this).has_previous()) {
				return (*(*this).target_container_ptr())[(*this).position() - 1];
			}
			else {
				MSE_THROW(nii_array_range_error("attempt to use invalid item_pointer - reference previous_item() - TFriendlyAugmentedRAIterator"));
			}
		}

		void set_to_item_pointer(const TFriendlyAugmentedRAIterator& _Right_cref) {
			(*this) = _Right_cref;
		}

		MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(base_class);
		void iterator_tag() const {}
		template<class base_class2 = base_class, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<base_class2, base_class>::value)
			&& (mse::impl::is_potentially_xscope<base_class2>::value)> MSE_IMPL_EIS >
		void xscope_tag() const {}

	private:
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;
	};

	template <typename _TRAIterator>
	class TFriendlyAugmentedRAConstIterator : public _TRAIterator {
	public:
		typedef _TRAIterator base_class;
		MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

		template<class _Ty2 = base_class, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, base_class>::value) && (std::is_default_constructible<_Ty2>::value)> MSE_IMPL_EIS >
		TFriendlyAugmentedRAConstIterator() : base_class() {}

		MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TFriendlyAugmentedRAConstIterator, base_class);

		auto& operator=(const TFriendlyAugmentedRAConstIterator& _Right_cref) { base_class::operator=(_Right_cref); return (*this); }
		auto& operator=(TFriendlyAugmentedRAConstIterator&& _Right_cref) { base_class::operator=(MSE_FWD(_Right_cref)); return (*this); }
		MSE_USING_ASSIGNMENT_OPERATOR(base_class);

		void assert_valid_index() const {
			if (difference_type((*this).target_container_ptr()->size() - 0) < (*this).position()) { MSE_THROW(nii_array_range_error("invalid index - void assert_valid_index() const - TFriendlyAugmentedRAConstIterator")); }
		}
		void reset() { set_to_end_marker(); }
		bool points_to_an_item() const {
			if (difference_type((*this).target_container_ptr()->size() - 0) > (*this).position()) { return true; }
			else {
				assert((*this).position() == difference_type((*this).target_container_ptr()->size() - 0));
				return false;
			}
		}
		bool points_to_end_marker() const {
			if (false == points_to_an_item()) {
				assert((*this).position() == difference_type((*this).target_container_ptr()->size() - 0));
				return true;
			}
			else { return false; }
		}
		bool points_to_beginning() const {
			if (0 == (*this).position()) { return true; }
			else { return false; }
		}
		/* has_next_item_or_end_marker() is just an alias for points_to_an_item(). */
		bool has_next_item_or_end_marker() const { return points_to_an_item(); }
		/* has_next() is just an alias for points_to_an_item() that's familiar to java programmers. */
		bool has_next() const { return has_next_item_or_end_marker(); }
		bool has_previous() const {
			return ((1 <= (*this).target_container_ptr()->size()) && (!points_to_beginning()));
		}
		void set_to_end_marker() {
			(*this).set_to_beginning();
			advance((*this).target_container_ptr()->size());
		}
		void set_to_next() {
			if (points_to_an_item()) {
				advance(1);
				assert(difference_type((*this).target_container_ptr()->size() - 0) >= (*this).position());
			}
			else {
				MSE_THROW(nii_array_range_error("attempt to use invalid const_item_pointer - void set_to_next() - TFriendlyAugmentedRAConstIterator"));
			}
		}
		void set_to_previous() {
			if (has_previous()) {
				regress(1);
			}
			else {
				MSE_THROW(nii_array_range_error("attempt to use invalid const_item_pointer - void set_to_previous() - TFriendlyAugmentedRAConstIterator"));
			}
		}
		TFriendlyAugmentedRAConstIterator& operator ++() { (*this).set_to_next(); return (*this); }
		TFriendlyAugmentedRAConstIterator operator++(int) { TFriendlyAugmentedRAConstIterator _Tmp = *this; (*this).set_to_next(); return (_Tmp); }
		TFriendlyAugmentedRAConstIterator& operator --() { (*this).set_to_previous(); return (*this); }
		TFriendlyAugmentedRAConstIterator operator--(int) { TFriendlyAugmentedRAConstIterator _Tmp = *this; (*this).set_to_previous(); return (_Tmp); }
		void advance(difference_type n) {
			auto new_index = msear_int((*this).position()) + n;
			if ((0 > new_index) || ((*this).target_container_ptr()->size() < msear_size_t(new_index))) {
				MSE_THROW(nii_array_range_error("index out of range - void advance(difference_type n) - TFriendlyAugmentedRAConstIterator"));
			}
			else {
				base_class::operator+=(n);
			}
		}
		void regress(difference_type n) { advance(-n); }
		TFriendlyAugmentedRAConstIterator& operator +=(difference_type n) { (*this).advance(n); return (*this); }
		TFriendlyAugmentedRAConstIterator& operator -=(difference_type n) { (*this).regress(n); return (*this); }
		TFriendlyAugmentedRAConstIterator operator+(difference_type n) const {
			TFriendlyAugmentedRAConstIterator retval(*this);
			retval.advance(n);
			return retval;
		}
		TFriendlyAugmentedRAConstIterator operator-(difference_type n) const { return ((*this) + (-n)); }
		difference_type operator-(const base_class& _Right_cref) const {
			return base_class::operator-(_Right_cref);
		}
		const_reference item() const { return (*this).operator*(); }
		const_reference previous_item() const {
			if ((*this).has_previous()) {
				return (*(*this).target_container_ptr())[(*this).position() - 1];
			}
			else {
				MSE_THROW(nii_array_range_error("attempt to use invalid item_pointer - const_reference previous_item() - TFriendlyAugmentedRAConstIterator"));
			}
		}

		void set_to_const_item_pointer(const TFriendlyAugmentedRAConstIterator& _Right_cref) {
			(*this) = _Right_cref;
		}

		MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(base_class);
		void iterator_tag() const {}
		template<class base_class2 = base_class, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<base_class2, base_class>::value)
			&& (mse::impl::is_potentially_xscope<base_class2>::value)> MSE_IMPL_EIS >
		void xscope_tag() const {}

	private:
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;
	};

	namespace impl {
		template<class _TArray>
		auto container_size_helper2(std::true_type, const _TArray& native_array) {
			return native_array_size_msemsearray(native_array);
		}
		template<class _TArray>
		auto container_size_helper2(std::false_type, const _TArray& container) {
			return container.size();
		}

		template<class _TArrayPointer>
		auto container_size_helper(std::true_type, const _TArrayPointer& owner_ptr) {
			typedef mse::impl::remove_reference_t<decltype(*owner_ptr)> _TArray;
			return container_size_helper2(typename mse::impl::IsNativeArray_msemsearray<_TArray>::type(), *owner_ptr);
		}
		template<class _TArray>
		auto container_size_helper(std::false_type, const _TArray& container) {
			/* The parameter doesn't seem to be a pointer. */
			return container_size_helper2(typename mse::impl::IsNativeArray_msemsearray<_TArray>::type(), container);
		}
	}
	template<class _TArrayPointer>
	auto container_size(const _TArrayPointer& owner_ptr) {
		return impl::container_size_helper(typename mse::impl::IsDereferenceable_msemsearray<_TArrayPointer>::type(), owner_ptr);
	}

	/* value_at() is a free function that returns a copy of the value at the given index rather than a reference to it. Note that the
	returned value inherits the "returnability" of the given container pointer. */
	template<class _TArrayPointer>
	auto value_at(const _TArrayPointer& owner_ptr, size_t index) {
		return (*owner_ptr).at(index);
	}
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(value_at)

	namespace impl {
		template<class T, class EqualTo>
		struct HasDifferenceTypeMember_msemsearray_impl
		{
			template<class U, class V>
			static auto test(U*) -> decltype(std::declval<typename U::difference_type>() == std::declval<typename V::difference_type>(), bool(true));
			template<typename, typename>
			static auto test(...)->std::false_type;

			static const bool value = std::is_same<bool, decltype(test<T, EqualTo>(0))>::value;
			using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
		};
		template<class T, class EqualTo = T>
		struct HasDifferenceTypeMember_msemsearray : HasDifferenceTypeMember_msemsearray_impl<
			mse::impl::remove_reference_t<T>, mse::impl::remove_reference_t<EqualTo> >::type {};

		template<bool, class TIter>
		struct difference_type_of_iterator_helper;
		template<class TIter>
		struct difference_type_of_iterator_helper<false, TIter> {
			typedef msear_int type;
		};
		template<class TIter>
		struct difference_type_of_iterator_helper<true, TIter> {
			typedef typename TIter::difference_type type;
		};
	}
	template<class TIter>
	struct difference_type_of_iterator {
		typedef typename impl::difference_type_of_iterator_helper<impl::HasDifferenceTypeMember_msemsearray<TIter>::value, TIter>::type type;
	};

	template <typename _TRAContainerPointer> class TXScopeCSSSStrongRAIterator;
	template <typename _TRAContainerPointer> class TXScopeCSSSStrongRAConstIterator;

	namespace us {
		/* A couple of unsafe functions for internal use. */
		template<class _TRAContainerPointer>
		TXScopeCSSSStrongRAIterator<_TRAContainerPointer> unsafe_make_xscope_csss_strong_ra_iterator(const _TRAContainerPointer& ra_container_pointer, typename TXScopeCSSSStrongRAIterator<_TRAContainerPointer>::size_type index = 0);
		template<class _TRAContainerPointer>
		TXScopeCSSSStrongRAConstIterator<_TRAContainerPointer> unsafe_make_xscope_csss_strong_ra_const_iterator(const _TRAContainerPointer& ra_container_pointer, typename TXScopeCSSSStrongRAConstIterator<_TRAContainerPointer>::size_type index = 0);
	}

	template <typename _TRAContainerPointer> class TXScopeCSSSStrongRAConstIterator;

	template <typename _TRAContainerPointer>
	class TXScopeCSSSStrongRAIterator : public TXScopeRAIterator<_TRAContainerPointer> {
	public:
		typedef TXScopeRAIterator<_TRAContainerPointer> base_class;
		MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

		TXScopeCSSSStrongRAIterator(const TXScopeCSSSStrongRAIterator& src) : base_class(src) {}
		TXScopeCSSSStrongRAIterator(TXScopeCSSSStrongRAIterator&& src) : base_class(MSE_FWD(src)) {}
		TXScopeCSSSStrongRAIterator(const _TRAContainerPointer& ra_container_pointer, size_type index) : base_class(ra_container_pointer, index) {
			mse::impl::is_valid_if_strong_pointer<_TRAContainerPointer>::no_op();
			typedef mse::impl::remove_reference_t<decltype(*ra_container_pointer)> TRAContainer;
			mse::impl::T_valid_if_is_contiguous_sequence_static_structure_container_msemsearray<TRAContainer>();
		}
		TXScopeCSSSStrongRAIterator(_TRAContainerPointer&& ra_container_pointer, size_type index) : base_class(MSE_FWD(ra_container_pointer), index) {
			mse::impl::is_valid_if_strong_pointer<_TRAContainerPointer>::no_op();
			typedef mse::impl::remove_reference_t<decltype(*ra_container_pointer)> TRAContainer;
			mse::impl::T_valid_if_is_contiguous_sequence_static_structure_container_msemsearray<TRAContainer>();
		}
		TXScopeCSSSStrongRAIterator(const TXScopeRAIterator<_TRAContainerPointer>& xs_ra_iter) : base_class(xs_ra_iter) {
			mse::impl::T_valid_if_is_contiguous_sequence_static_structure_or_locking_strong_iterator_msemsearray<TXScopeRAIterator<_TRAContainerPointer> >();
		}
		TXScopeCSSSStrongRAIterator(TXScopeRAIterator<_TRAContainerPointer>&& xs_ra_iter) : base_class(MSE_FWD(xs_ra_iter)) {
			mse::impl::T_valid_if_is_contiguous_sequence_static_structure_strong_iterator_msemsearray<TXScopeRAIterator<_TRAContainerPointer> >();
		}

		MSE_INHERIT_ITERATOR_ARITHMETIC_OPERATORS_FROM(base_class, TXScopeCSSSStrongRAIterator);
		MSE_INHERIT_ASSIGNMENT_OPERATOR_FROM(base_class, TXScopeCSSSStrongRAIterator);
		MSE_USING_ASSIGNMENT_OPERATOR(base_class);

		bool is_valid() const {
			return ((0 <= (*this).position()) && (difference_type(mse::container_size(*(*this).target_container_ptr())) >= (*this).position()));
		}

		MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(base_class);
		void xscope_iterator_tag() const {}
		void xscope_tag() const {}

	private:
		class unsafe_t {};
		TXScopeCSSSStrongRAIterator(unsafe_t, const _TRAContainerPointer& ra_container_pointer, size_type index = 0) : base_class(ra_container_pointer, index) {}
		TXScopeCSSSStrongRAIterator(unsafe_t, _TRAContainerPointer&& ra_container_pointer, size_type index = 0) : base_class(MSE_FWD(ra_container_pointer), index) {}

		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		friend class TXScopeCSSSStrongRAConstIterator<_TRAContainerPointer>;
		template<class _TRAContainerPointer2> friend TXScopeCSSSStrongRAIterator<_TRAContainerPointer2> us::unsafe_make_xscope_csss_strong_ra_iterator(const _TRAContainerPointer2& ra_container_pointer, typename TXScopeCSSSStrongRAIterator<_TRAContainerPointer2>::size_type index);
	};

	template <typename _TRAContainerPointer>
	class TXScopeCSSSStrongRAConstIterator : public TXScopeRAConstIterator<_TRAContainerPointer> {
	public:
		typedef TXScopeRAConstIterator<_TRAContainerPointer> base_class;
		MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

		TXScopeCSSSStrongRAConstIterator(const TXScopeCSSSStrongRAConstIterator& src) : base_class(src) {}
		TXScopeCSSSStrongRAConstIterator(TXScopeCSSSStrongRAConstIterator&& src) : base_class(MSE_FWD(src)) {}

		TXScopeCSSSStrongRAConstIterator(const TXScopeCSSSStrongRAIterator<_TRAContainerPointer>& src) : base_class(src) {}
		TXScopeCSSSStrongRAConstIterator(TXScopeCSSSStrongRAIterator<_TRAContainerPointer>&& src) : base_class(MSE_FWD(src)) {}

		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2, _TRAContainerPointer>::value> MSE_IMPL_EIS >
		TXScopeCSSSStrongRAConstIterator(TXScopeCSSSStrongRAConstIterator<_Ty2>&& src) : base_class(MSE_FWD(src)) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2, _TRAContainerPointer>::value> MSE_IMPL_EIS >
		TXScopeCSSSStrongRAConstIterator(const TXScopeCSSSStrongRAConstIterator<_Ty2>& src) : base_class(src) {}

		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2, _TRAContainerPointer>::value> MSE_IMPL_EIS >
		TXScopeCSSSStrongRAConstIterator(TXScopeCSSSStrongRAIterator<_Ty2>&& src) : base_class(MSE_FWD(src)) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2, _TRAContainerPointer>::value> MSE_IMPL_EIS >
		TXScopeCSSSStrongRAConstIterator(const TXScopeCSSSStrongRAIterator<_Ty2>& src) : base_class(src) {}

		TXScopeCSSSStrongRAConstIterator(const _TRAContainerPointer& ra_container_pointer, size_type index = 0) : base_class(ra_container_pointer, index) {
			mse::impl::is_valid_if_strong_pointer<_TRAContainerPointer>::no_op();
			typedef mse::impl::remove_reference_t<decltype(*ra_container_pointer)> TRAContainer;
			mse::impl::T_valid_if_is_contiguous_sequence_static_structure_container_msemsearray<TRAContainer>();
		}
		TXScopeCSSSStrongRAConstIterator(_TRAContainerPointer&& ra_container_pointer, size_type index = 0) : base_class(MSE_FWD(ra_container_pointer), index) {
			mse::impl::is_valid_if_strong_pointer<_TRAContainerPointer>::no_op();
			typedef mse::impl::remove_reference_t<decltype(*ra_container_pointer)> TRAContainer;
			mse::impl::T_valid_if_is_contiguous_sequence_static_structure_container_msemsearray<TRAContainer>();
		}
		TXScopeCSSSStrongRAConstIterator(const TXScopeRAConstIterator<_TRAContainerPointer>& xs_ra_iter) : base_class(xs_ra_iter) {
			mse::impl::T_valid_if_is_contiguous_sequence_static_structure_or_locking_strong_iterator_msemsearray<TXScopeRAConstIterator<_TRAContainerPointer> >();
		}
		TXScopeCSSSStrongRAConstIterator(TXScopeRAConstIterator<_TRAContainerPointer>&& xs_ra_iter) : base_class(MSE_FWD(xs_ra_iter)) {
			mse::impl::T_valid_if_is_contiguous_sequence_static_structure_strong_iterator_msemsearray<TXScopeRAConstIterator<_TRAContainerPointer> >();
		}
		/*
		TXScopeCSSSStrongRAConstIterator(const TXScopeRAIterator<_TRAContainerPointer>& xs_ra_iter) : base_class(xs_ra_iter) {
			mse::impl::T_valid_if_is_contiguous_sequence_static_structure_or_locking_strong_iterator_msemsearray<TXScopeRAIterator<_TRAContainerPointer> >();
		}
		TXScopeCSSSStrongRAConstIterator(TXScopeRAIterator<_TRAContainerPointer>&& xs_ra_iter) : base_class(MSE_FWD(xs_ra_iter)) {
			mse::impl::T_valid_if_is_contiguous_sequence_static_structure_strong_iterator_msemsearray<TXScopeRAIterator<_TRAContainerPointer> >();
		}
		*/

		MSE_INHERIT_ITERATOR_ARITHMETIC_OPERATORS_FROM(base_class, TXScopeCSSSStrongRAConstIterator);
		MSE_INHERIT_ASSIGNMENT_OPERATOR_FROM(base_class, TXScopeCSSSStrongRAConstIterator);
		MSE_USING_ASSIGNMENT_OPERATOR(base_class);

		bool is_valid() const {
			return ((0 <= (*this).position()) && (difference_type(mse::container_size(*(*this).target_container_ptr())) >= (*this).position()));
		}

		MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(base_class);
		void xscope_iterator_tag() const {}
		void xscope_tag() const {}

	private:
		class unsafe_t {};
		TXScopeCSSSStrongRAConstIterator(unsafe_t, const _TRAContainerPointer& ra_container_pointer, size_type index = 0) : base_class(ra_container_pointer, index) {}
		TXScopeCSSSStrongRAConstIterator(unsafe_t, _TRAContainerPointer&& ra_container_pointer, size_type index = 0) : base_class(MSE_FWD(ra_container_pointer), index) {}

		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		template<class _TRAContainerPointer2> friend TXScopeCSSSStrongRAConstIterator<_TRAContainerPointer2> us::unsafe_make_xscope_csss_strong_ra_const_iterator(const _TRAContainerPointer2& ra_container_pointer, typename TXScopeCSSSStrongRAConstIterator<_TRAContainerPointer2>::size_type index);
	};

	/* These specializations are here only because there seems to be a bug(?) in g++7's (but not g++5's) std::is_constructible<>
	such that if its argument is convertible to one of these specializations (which are invalid instantiations and would
	otherwise not compile) it may(/does?) try to instantiate it. */
	template <typename _TRAContainer>
	class TXScopeCSSSStrongRAConstIterator<mse::TXScopeFixedConstPointer<mse::TXScopeFixedConstPointer<_TRAContainer> > > {};
	template <typename _TRAContainer>
	class TXScopeCSSSStrongRAConstIterator<mse::TXScopeFixedConstPointer<mse::TXScopeFixedPointer<_TRAContainer> > > {};
	template <typename _TRAContainer>
	class TXScopeCSSSStrongRAConstIterator<mse::TXScopeFixedPointer<mse::TXScopeFixedPointer<_TRAContainer> > > {};
	template <typename _TRAContainer>
	class TXScopeCSSSStrongRAConstIterator<mse::TXScopeFixedPointer<mse::TXScopeFixedConstPointer<_TRAContainer> > > {};

	namespace us {
		/* A couple of unsafe functions for internal use. */
		template<class _TRAContainerPointer>
		TXScopeCSSSStrongRAIterator<_TRAContainerPointer> unsafe_make_xscope_csss_strong_ra_iterator(const _TRAContainerPointer& ra_container_pointer, typename TXScopeCSSSStrongRAIterator<_TRAContainerPointer>::size_type index/* = 0*/) {
			return TXScopeCSSSStrongRAIterator<_TRAContainerPointer>(typename TXScopeCSSSStrongRAIterator<_TRAContainerPointer>::unsafe_t(), ra_container_pointer, index);
		}
		template<class _TRAContainerPointer>
		TXScopeCSSSStrongRAConstIterator<_TRAContainerPointer> unsafe_make_xscope_csss_strong_ra_const_iterator(const _TRAContainerPointer& ra_container_pointer, typename TXScopeCSSSStrongRAConstIterator<_TRAContainerPointer>::size_type index/* = 0*/) {
			return TXScopeCSSSStrongRAConstIterator<_TRAContainerPointer>(typename TXScopeCSSSStrongRAConstIterator<_TRAContainerPointer>::unsafe_t(), ra_container_pointer, index);
		}
	}

	template <typename _TRAContainerPointer>
	auto xscope_pointer(const TXScopeCSSSStrongRAIterator<_TRAContainerPointer>& iter_cref) {
		return mse::us::unsafe_make_xscope_pointer_to(*iter_cref);
	}
	template <typename _TRAContainerPointer>
	auto xscope_pointer(const TXScopeCSSSStrongRAConstIterator<_TRAContainerPointer>& iter_cref) {
		return mse::us::unsafe_make_xscope_const_pointer_to(*iter_cref);
	}
	template <typename _TRAContainerPointer>
	auto xscope_const_pointer(const TXScopeCSSSStrongRAIterator<_TRAContainerPointer>& iter_cref) {
		return mse::us::unsafe_make_xscope_const_pointer_to(*iter_cref);
	}
	template <typename _TRAContainerPointer>
	auto xscope_const_pointer(const TXScopeCSSSStrongRAConstIterator<_TRAContainerPointer>& iter_cref) {
		return mse::us::unsafe_make_xscope_const_pointer_to(*iter_cref);
	}


	/* Forward declaration of some algorithm elements we're going to specialize/overload. We can't just include
	"msealgorithm.h" (where they are declared), because msealgorithm.h includes this file ("msemsearray.h"). */
	namespace impl {

		namespace us {
			template<class _InIt> auto iterator_pair_to_raw_pointers_checked(const _InIt& first, const _InIt& last);
		}
		template<class _InIt> class TXScopeSpecializedFirstAndLast;
		template<class _ContainerPointer> class TXScopeRangeIterProvider;
		template<class _ContainerPointer> class TRangeIterProvider;
		template<class _ContainerPointer> auto make_xscope_range_iter_provider(const _ContainerPointer& ptr);
	}
	template<class _InIt, class _Pr, class... Args> inline _InIt find_if_ptr(const _InIt& _First, const _InIt& _Last, _Pr _Pred, const Args&... args);
	template<class _InIt, class _Fn, class... Args> inline auto for_each_ptr(const _InIt& _First, const _InIt& _Last, _Fn _Func, const Args&... args);
	template<class _RanIt> inline void sort(const _RanIt& _First, const _RanIt& _Last);

	namespace impl {

		/* Some algorithm implementation specializations for TXScopeCSSSStrongRA(Const)Iterator<>s.  */

		/* Provides raw pointer iterators from the given iterators of a "random access" container. */
		template<class _InIt>
		class TXScopeRawPointerRAFirstAndLast {
		public:
			typedef decltype(us::iterator_pair_to_raw_pointers_checked(std::declval<_InIt>(), std::declval<_InIt>())) raw_pair_t;
			TXScopeRawPointerRAFirstAndLast(const _InIt& _First, const _InIt& _Last)
				: m_raw_pair(us::iterator_pair_to_raw_pointers_checked(_First, _Last)) {}
			const auto& first() const {
				return m_raw_pair.first;
			}
			const auto& last() const {
				return m_raw_pair.second;
			}

		private:
			raw_pair_t m_raw_pair;
		};

		/* Provides raw pointer iterators for the given "random access" container. */
		template<class _ContainerPointer>
		class TXScopeRARangeRawPointerIterProvider {
		public:
			typedef decltype(std::addressof((*std::declval<_ContainerPointer>())[0])) iter_t;
			TXScopeRARangeRawPointerIterProvider(const _ContainerPointer& _XscpPtr) : m_begin(std::addressof((*_XscpPtr)[0]))
				, m_end(std::addressof((*_XscpPtr)[0]) + mse::as_a_size_t((*_XscpPtr).size())) {}
			const auto& begin() const { return m_begin; }
			const auto& end() const { return m_end; }

		private:
			iter_t m_begin;
			iter_t m_end;
		};

		/* Specializations of TXScopeSpecializedFirstAndLast<> that replace certain iterators with fast (raw pointer) iterators
		when it's safe to do so. In this case TXScopeCSSSStrongRA(Const)Iterator<>s. */
		template <typename _TRAContainerPointer>
		class TXScopeSpecializedFirstAndLast<TXScopeCSSSStrongRAConstIterator<_TRAContainerPointer> >
			: public TXScopeRawPointerRAFirstAndLast<TXScopeCSSSStrongRAConstIterator<_TRAContainerPointer> > {
		public:
			typedef TXScopeRawPointerRAFirstAndLast<TXScopeCSSSStrongRAConstIterator<_TRAContainerPointer> > base_class;
			MSE_USING(TXScopeSpecializedFirstAndLast, base_class);
		};

		template <typename _TRAContainerPointer>
		auto make_xscope_specialized_first_and_last_overloaded(const TXScopeCSSSStrongRAConstIterator<_TRAContainerPointer>& _First, const TXScopeCSSSStrongRAConstIterator<_TRAContainerPointer>& _Last) {
			return TXScopeSpecializedFirstAndLast<TXScopeCSSSStrongRAConstIterator<_TRAContainerPointer> >(_First, _Last);
		}

		template <typename _TRAContainerPointer>
		class TXScopeSpecializedFirstAndLast<TXScopeCSSSStrongRAIterator<_TRAContainerPointer> >
			: public TXScopeRawPointerRAFirstAndLast<TXScopeCSSSStrongRAIterator<_TRAContainerPointer> > {
		public:
			typedef TXScopeRawPointerRAFirstAndLast<TXScopeCSSSStrongRAIterator<_TRAContainerPointer> > base_class;
			MSE_USING(TXScopeSpecializedFirstAndLast, base_class);
		};

		template <typename _TRAContainerPointer>
		auto make_xscope_specialized_first_and_last_overloaded(const TXScopeCSSSStrongRAIterator<_TRAContainerPointer>& _First, const TXScopeCSSSStrongRAIterator<_TRAContainerPointer>& _Last) {
			return TXScopeSpecializedFirstAndLast<TXScopeCSSSStrongRAIterator<_TRAContainerPointer> >(_First, _Last);
		}

		/* Provides raw pointer iterators for the given "random access" container while holding on to one of the iterators (or a copy of it). */
		template <typename iter_t>
		class TXScopeStrongRawPointerRAFirstAndLast : public TXScopeRawPointerRAFirstAndLast<iter_t> {
		public:
			typedef TXScopeRawPointerRAFirstAndLast<iter_t> base_class;
			TXScopeStrongRawPointerRAFirstAndLast(iter_t first, const iter_t& last) : base_class(first, last), m_first(std::move(first)) {}

		private:
			/* We need to store one of the given iterators (or a copy of it) as it holds, while it exists, (safe) access rights to the range. */
			iter_t m_first;
		};

		/* Overloads that replace certain iterators with fast (raw pointer) iterators when it's safe to do so. In this case
		lvalue (but not rvalue) mse::TXScopeRA(Const)Iterator<mse::TXScopeAccessControlledConstPointer<> >s of "exclusive writer" objects. */
		template <typename _TRAContainer, class _TAccessMutex, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::is_exclusive_writer_enforcing_mutex_msemsearray<_TAccessMutex>::value> MSE_IMPL_EIS >
		auto make_xscope_specialized_first_and_last_overloaded(const mse::TXScopeRAConstIterator<mse::TXScopeAccessControlledConstPointer<_TRAContainer, _TAccessMutex> >& _First, const mse::TXScopeRAConstIterator<mse::TXScopeAccessControlledConstPointer<_TRAContainer, _TAccessMutex> >& _Last) {
			return TXScopeStrongRawPointerRAFirstAndLast<mse::TXScopeRAConstIterator<mse::TXScopeAccessControlledConstPointer<_TRAContainer, _TAccessMutex> > >(_First, _Last);
		}
		template <typename _TRAContainer, class _TAccessMutex, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::is_exclusive_writer_enforcing_mutex_msemsearray<_TAccessMutex>::value> MSE_IMPL_EIS >
		auto make_xscope_specialized_first_and_last_overloaded(mse::TXScopeRAConstIterator<mse::TXScopeAccessControlledConstPointer<_TRAContainer, _TAccessMutex> >& _First, const mse::TXScopeRAConstIterator<mse::TXScopeAccessControlledConstPointer<_TRAContainer, _TAccessMutex> >& _Last) {
			return TXScopeStrongRawPointerRAFirstAndLast<mse::TXScopeRAConstIterator<mse::TXScopeAccessControlledConstPointer<_TRAContainer, _TAccessMutex> > >(_First, _Last);
		}
		template <typename _TRAContainer, class _TAccessMutex, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::is_exclusive_writer_enforcing_mutex_msemsearray<_TAccessMutex>::value> MSE_IMPL_EIS >
		auto make_xscope_specialized_first_and_last_overloaded(mse::TXScopeRAConstIterator<mse::TXScopeAccessControlledConstPointer<_TRAContainer, _TAccessMutex> >&& _First, const mse::TXScopeRAConstIterator<mse::TXScopeAccessControlledConstPointer<_TRAContainer, _TAccessMutex> >& _Last) = delete;

		template <typename _TRAContainer, class _TAccessMutex, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::is_exclusive_writer_enforcing_mutex_msemsearray<_TAccessMutex>::value> MSE_IMPL_EIS >
		auto make_xscope_specialized_first_and_last_overloaded(const mse::TXScopeRAIterator<mse::TXScopeAccessControlledConstPointer<_TRAContainer, _TAccessMutex> >& _First, const mse::TXScopeRAIterator<mse::TXScopeAccessControlledConstPointer<_TRAContainer, _TAccessMutex> >& _Last) {
			return TXScopeStrongRawPointerRAFirstAndLast<mse::TXScopeRAIterator<mse::TXScopeAccessControlledConstPointer<_TRAContainer, _TAccessMutex> > >(_First, _Last);
		}
		template <typename _TRAContainer, class _TAccessMutex, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::is_exclusive_writer_enforcing_mutex_msemsearray<_TAccessMutex>::value> MSE_IMPL_EIS >
		auto make_xscope_specialized_first_and_last_overloaded(mse::TXScopeRAIterator<mse::TXScopeAccessControlledConstPointer<_TRAContainer, _TAccessMutex> >& _First, const mse::TXScopeRAIterator<mse::TXScopeAccessControlledConstPointer<_TRAContainer, _TAccessMutex> >& _Last) {
			return TXScopeStrongRawPointerRAFirstAndLast<mse::TXScopeRAIterator<mse::TXScopeAccessControlledConstPointer<_TRAContainer, _TAccessMutex> > >(_First, _Last);
		}
		template <typename _TRAContainer, class _TAccessMutex, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::is_exclusive_writer_enforcing_mutex_msemsearray<_TAccessMutex>::value> MSE_IMPL_EIS >
		auto make_xscope_specialized_first_and_last_overloaded(mse::TXScopeRAIterator<mse::TXScopeAccessControlledConstPointer<_TRAContainer, _TAccessMutex> >&& _First, const mse::TXScopeRAIterator<mse::TXScopeAccessControlledConstPointer<_TRAContainer, _TAccessMutex> >& _Last) = delete;
	}
}

namespace std {

	/* Overloads of standard algorithm functions for TXScopeCSSSStrongRA(Const)Iterator<>s. */

	template<class _Pr, typename _TRAContainerPointer>
	inline auto find_if(const mse::TXScopeCSSSStrongRAConstIterator<_TRAContainerPointer>& _First, const mse::TXScopeCSSSStrongRAConstIterator<_TRAContainerPointer>& _Last, _Pr _Pred) -> mse::TXScopeCSSSStrongRAConstIterator<_TRAContainerPointer> {
		auto pred2 = [&_Pred](auto ptr) { return _Pred(*ptr); };
		return mse::find_if_ptr(_First, _Last, pred2);
	}
	template<class _Pr, typename _TRAContainerPointer>
	inline auto find_if(const mse::TXScopeCSSSStrongRAIterator<_TRAContainerPointer>& _First, const mse::TXScopeCSSSStrongRAIterator<_TRAContainerPointer>& _Last, _Pr _Pred) -> mse::TXScopeCSSSStrongRAIterator<_TRAContainerPointer> {
		auto pred2 = [&_Pred](auto ptr) { return _Pred(*ptr); };
		return mse::find_if_ptr(_First, _Last, pred2);
	}

	template<class _Fn, typename _TRAContainerPointer>
	inline _Fn for_each(const mse::TXScopeCSSSStrongRAConstIterator<_TRAContainerPointer>& _First, const mse::TXScopeCSSSStrongRAConstIterator<_TRAContainerPointer>& _Last, _Fn _Func) {
		auto func2 = [&_Func](auto ptr) { _Func(*ptr); };
		mse::for_each_ptr(_First, _Last, func2);
		return (_Func);
	}
	template<class _Fn, typename _TRAContainerPointer>
	inline _Fn for_each(const mse::TXScopeCSSSStrongRAIterator<_TRAContainerPointer>& _First, const mse::TXScopeCSSSStrongRAIterator<_TRAContainerPointer>& _Last, _Fn _Func) {
		auto func2 = [&_Func](auto ptr) { _Func(*ptr); };
		mse::for_each_ptr(_First, _Last, func2);
		return (_Func);
	}

	template <typename _TRAContainerPointer>
	inline void sort(const mse::TXScopeCSSSStrongRAIterator<_TRAContainerPointer>& _First, const mse::TXScopeCSSSStrongRAIterator<_TRAContainerPointer>& _Last) {
		mse::sort(_First, _Last);
	}
}

namespace mse {

	template <typename _TRAContainer>
	using TXScopeCSSSXSRAConstIterator = TXScopeCSSSStrongRAConstIterator<mse::TXScopeFixedConstPointer<_TRAContainer> >;
	template <typename _TRAContainer>
	using TXScopeCSSSXSRAIterator = TXScopeCSSSStrongRAIterator<mse::TXScopeFixedPointer<_TRAContainer> >;

	namespace rsv {

		/* Template specializations of TFParam<>. */

#define MSE_FPARAM_RVALUE_TO_LVALUE_LONE_PARAM_USING(Derived, Base) \
    MSE_USING_SANS_INITIALIZER_LISTS(Derived, Base) \
	/* Template parameter type deduction doesn't work for initializer_lists so we add a constructor overload to handle them. */ \
	/* But this constructor overload might sometimes match when you don't want. */ \
    template<typename _Ty_using1, MSE_IMPL_EIP mse::impl::enable_if_t< \
			std::is_constructible<Base, std::initializer_list<_Ty_using1> >::value \
		> MSE_IMPL_EIS > \
    Derived(const std::initializer_list<_Ty_using1>& il) : Base(il) {} \
	template<typename Arg, MSE_IMPL_EIP mse::impl::enable_if_t< \
		std::is_constructible<Base, Arg>::value \
		&& !mse::impl::is_a_pair_with_the_first_a_base_of_the_second<Derived, Arg>::value \
	> MSE_IMPL_EIS > \
	/* Here we're "forwarding" (lone) rvalue arguments as lvalues. This enables certain conversions which are */ \
	/* not, in general, safe on temporaries, but are safe in this case because rsv::TFParam<> may only be used as */ \
	/* (non returnable) function parameters (so we know that the rvalue/temporary argument will outlive the */ \
	/* rsv::TFParam<> object). */ \
	Derived(Arg&& arg) : Base(arg) {} \
	template<typename Arg1, typename Arg2, MSE_IMPL_EIP mse::impl::enable_if_t< \
		std::is_constructible<Base, Arg1, Arg2>::value \
		&& !mse::impl::is_a_pair_with_the_first_a_base_of_the_second<Derived, Arg1, Arg2>::value \
	> MSE_IMPL_EIS > \
	Derived(Arg1&& arg1, Arg2&& arg2) : Base(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2)) {}

#define MSE_FPARAM_RVALUE_TO_LVALUE_LONE_PARAM_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(Derived, Base) MSE_FPARAM_RVALUE_TO_LVALUE_LONE_PARAM_USING(Derived, Base) MSE_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(Derived)


		template<typename _Ty>
		class TFParam<mse::TXScopeCSSSXSRAConstIterator<_Ty> > : public TXScopeCSSSXSRAConstIterator<_Ty> {
		public:
			typedef TXScopeCSSSXSRAConstIterator<_Ty> base_class;
			MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);
			MSE_FPARAM_RVALUE_TO_LVALUE_LONE_PARAM_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TFParam, base_class);

			MSE_INHERIT_ITERATOR_ARITHMETIC_OPERATORS_FROM(base_class, TFParam);

			void xscope_not_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};

		template<typename _Ty>
		class TFParam<const mse::TXScopeCSSSXSRAConstIterator<_Ty> > : public TXScopeCSSSXSRAConstIterator<_Ty> {
		public:
			typedef TXScopeCSSSXSRAConstIterator<_Ty> base_class;
			MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);
			MSE_FPARAM_RVALUE_TO_LVALUE_LONE_PARAM_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TFParam, base_class);

			MSE_INHERIT_ITERATOR_ARITHMETIC_OPERATORS_FROM(base_class, TFParam);

			void xscope_not_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};

		template<typename _Ty>
		class TFParam<mse::TXScopeCSSSXSRAIterator<_Ty> > : public TXScopeCSSSXSRAIterator<_Ty> {
		public:
			typedef TXScopeCSSSXSRAIterator<_Ty> base_class;
			MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);
			MSE_FPARAM_RVALUE_TO_LVALUE_LONE_PARAM_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TFParam, base_class);

			MSE_INHERIT_ITERATOR_ARITHMETIC_OPERATORS_FROM(base_class, TFParam);

			void xscope_not_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};

		template<typename _Ty>
		class TFParam<const mse::TXScopeCSSSXSRAIterator<_Ty> > : public TXScopeCSSSXSRAIterator<_Ty> {
		public:
			typedef TXScopeCSSSXSRAIterator<_Ty> base_class;
			MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);
			MSE_FPARAM_RVALUE_TO_LVALUE_LONE_PARAM_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TFParam, base_class);

			MSE_INHERIT_ITERATOR_ARITHMETIC_OPERATORS_FROM(base_class, TFParam);

			void xscope_not_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};
	}

	namespace us {
		namespace impl {
			/* This template class acts like an array container, but is actually used as a (type erased) proxy to another
			suitable container. It also acts as a pointer to itself. */
			template <typename _TElement>
			class TXScopeRuntimeRawArrayProxyAndSelfPointer : public mse::us::impl::XScopeContainsNonOwningScopeReferenceTagBase, public mse::us::impl::StrongPointerAsyncNotShareableAndNotPassableTagBase
				, public mse::us::impl::ContiguousSequenceContainerTagBase, public mse::us::impl::StaticStructureContainerTagBase {
			public:
				typedef typename std::array<int, 0>::size_type size_type;
				typedef typename std::array<int, 0>::difference_type difference_type;

				MSE_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TXScopeRuntimeRawArrayProxyAndSelfPointer);
				TXScopeRuntimeRawArrayProxyAndSelfPointer(_TElement* first_element_ptr, size_type size) : m_first_element_ptr(first_element_ptr), m_size(size) {}

				_TElement& at(size_type _Off) const {
					if (m_size <= _Off) {
						MSE_THROW(msearray_range_error("out of bounds index - operator[](size_type _Off) - TXScopeRuntimeRawArrayProxyAndSelfPointer"));
					}
					return m_first_element_ptr[_Off];
				}
				_TElement& operator[](size_type _Off) const {
					return at(_Off);
				}

				size_t size() const { return m_size; }

				auto operator*() const {
					return (*this);
				}
				auto operator->() const {
					return this;
				}
				bool operator==(const TXScopeRuntimeRawArrayProxyAndSelfPointer& rhs) const {
					return ((m_first_element_ptr == rhs.m_first_element_ptr) && (m_size == rhs.m_size));
				}
				TXScopeRuntimeRawArrayProxyAndSelfPointer& operator=(const TXScopeRuntimeRawArrayProxyAndSelfPointer& rhs) = default;

			private:
				MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

				_TElement* m_first_element_ptr;
				size_type m_size = 0;

				template <typename _TElement2>
				friend class TXScopeRuntimeRawArrayConstProxyAndSelfPointer;
			};

			template <typename _TElement>
			class TXScopeRuntimeRawArrayConstProxyAndSelfPointer : public TXScopeRuntimeRawArrayProxyAndSelfPointer<const _TElement> {
			public:
				typedef TXScopeRuntimeRawArrayProxyAndSelfPointer<const _TElement> base_class;
				typedef typename base_class::size_type size_type;
				typedef typename base_class::difference_type difference_type;
				using base_class::base_class;
				TXScopeRuntimeRawArrayConstProxyAndSelfPointer(const TXScopeRuntimeRawArrayProxyAndSelfPointer<_TElement>& src) : base_class(src.m_first_element_ptr, src.m_size) {}

				auto operator*() const {
					return (*this);
				}
				auto operator->() const {
					return this;
				}
			private:
				MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
			};
		}
	}

	template <typename _TElement> class TXScopeCSSSXSTERAConstIterator;

	template <typename _TElement>
	class TXScopeCSSSXSTERAIterator : public TXScopeCSSSStrongRAIterator<mse::us::impl::TXScopeRuntimeRawArrayProxyAndSelfPointer<_TElement> > {
	public:
		typedef TXScopeCSSSStrongRAIterator<mse::us::impl::TXScopeRuntimeRawArrayProxyAndSelfPointer<_TElement> > base_class;
		MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

	private:
		struct CDummy {};
		template<typename _Ty2, typename _TRAIterator2>
		struct container_pointer_type_from_iterator_type {
			typedef decltype(std::declval<_TRAIterator2>().target_container_ptr()) type;
		};
		template<typename _TRAIterator2>
		struct container_pointer_type_from_iterator_type<std::false_type, _TRAIterator2> {
			typedef CDummy* const type;
		};
		template<typename _TRAIterator2>
		struct container_type_from_iterator_type {
			typedef typename container_pointer_type_from_iterator_type<typename mse::impl::HasOrInheritsTargetContainerPtrMethod_msemsearray<_TRAIterator2>::type, _TRAIterator2>::type TRAContainerPointer;
			typedef mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(*std::declval<TRAContainerPointer>())> > TRAContainer;
			typedef TRAContainer type;
		};

		template<typename _Ty2, typename _TRAIterator2, typename _TRAContainer2 = typename container_type_from_iterator_type<_TRAIterator2>::type>
		struct is_convertible_to_TXScopeCSSSXSRAIterator_helper1 : std::is_convertible<_TRAIterator2, TXScopeCSSSXSRAIterator<_TRAContainer2> > {};
		template<typename _TRAIterator2>
		struct is_convertible_to_TXScopeCSSSXSRAIterator_helper1<std::false_type, _TRAIterator2> : std::integral_constant<bool, false> {};
		template<typename _TRAIterator2>
		struct is_convertible_to_TXScopeCSSSXSRAIterator : std::integral_constant<bool
			, is_convertible_to_TXScopeCSSSXSRAIterator_helper1<typename mse::impl::HasOrInheritsTargetContainerPtrMethod_msemsearray<_TRAIterator2>::type, _TRAIterator2>::value> {};

	public:
		TXScopeCSSSXSTERAIterator(const TXScopeCSSSXSTERAIterator& src) : base_class(src) {}
		TXScopeCSSSXSTERAIterator(TXScopeCSSSXSTERAIterator&& src) : base_class(MSE_FWD(src)) {}

		template <typename _TRAContainer2>
		TXScopeCSSSXSTERAIterator(const TXScopeCSSSXSRAIterator<_TRAContainer2>& src) : base_class(construction_helper1(src.target_container_ptr()), src.position()) {}

		template<typename _TRAIterator2>
		TXScopeCSSSXSTERAIterator(const _TRAIterator2& ra_iter, size_type index = 0)
			: base_class(construction_helper1(typename std::is_base_of<TXScopeCSSSXSTERAIterator, _TRAIterator2>::type(), ra_iter, index)) {}
		template<typename _TRAIterator2>
		TXScopeCSSSXSTERAIterator(_TRAIterator2&& ra_iter, size_type index = 0)
			: base_class(construction_helper1(typename std::is_base_of<TXScopeCSSSXSTERAIterator, mse::impl::remove_reference_t<decltype(ra_iter)> >::type(), MSE_FWD(ra_iter), index)) {}

		auto& operator=(const TXScopeCSSSXSTERAIterator& _Right_cref) { base_class::operator=(_Right_cref); return (*this); }
		auto& operator=(TXScopeCSSSXSTERAIterator&& _Right_cref) { base_class::operator=(MSE_FWD(_Right_cref)); return (*this); }
		MSE_USING_ASSIGNMENT_OPERATOR(base_class);

		MSE_INHERIT_ITERATOR_ARITHMETIC_OPERATORS_FROM(base_class, TXScopeCSSSXSTERAIterator);

		MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(base_class);
		void xscope_iterator_tag() const {}
		void xscope_tag() const {}

	private:

		template<typename _TRAIterator2>
		static auto construction_helper9(std::true_type, const _TRAIterator2& ra_iter, const size_type index = 0) {
			_TElement * const ptr = std::addressof(*ra_iter);
			return base_class(mse::us::impl::TXScopeRuntimeRawArrayProxyAndSelfPointer<_TElement>(ptr, std::numeric_limits<size_type>::max()), index);
		}
		template<typename _TParam1>
		static auto construction_helper9(std::false_type, const _TParam1& param1, const size_type index = 0) {
			return construction_helper6(param1, index);
		}
#if defined(MSE_MSTDARRAY_DISABLED) || defined(MSE_MSTDVECTOR_DISABLED)
		template <typename _TRAContainer2>
		static auto construction_helper8(const mse::TXScopeFixedPointer<_TRAContainer2>& ptr, const size_type index = 0) {
			return base_class(construction_helper3(ptr), index);
		}
#else // defined(MSE_MSTDARRAY_DISABLED) || defined(MSE_MSTDVECTOR_DISABLED)
		template<size_t _Size>
		static auto construction_helper8(const mse::TXScopeFixedPointer<std::array<_TElement, _Size> >& ptr, const size_type index = 0) {
			return base_class(construction_helper3(ptr), index);
		}
#endif // defined(MSE_MSTDARRAY_DISABLED) || defined(MSE_MSTDVECTOR_DISABLED)
		template <typename _TRAContainer2>
		static auto construction_helper7(const mse::TXScopeFixedPointer<_TRAContainer2>& ptr, const size_type index = 0) {
			return construction_helper8(ptr, index);
		}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		template <typename _TRAContainer2>
		static auto construction_helper7(const mse::TXScopeObjFixedPointer<_TRAContainer2>& ptr, const size_type index = 0) {
			return construction_helper8(mse::TXScopeFixedPointer<_TRAContainer2>(ptr), index);
		}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
		template <typename _TRAContainerPointer2>
		static auto construction_helper6(const mse::TXScopeRAIterator<_TRAContainerPointer2>& ra_iter, const size_type index = 0) {
			/* If you get a compile error here, it means that the given iterator (or arguments) is not convertible to
			the specified TXScopeCSSSXSTERAIterator<>. A common case is when trying to convert an rvalue (temporary) scope
			iterator of a dynamic (structure lockable) container (like a vector), where only lvalues are supported. */
			assert(0 == index); return construction_helper7(ra_iter.target_container_ptr(), ra_iter.position());
		}
		template <typename _TRAContainer2>
		static auto construction_helper6(const TXScopeCSSSXSRAIterator<_TRAContainer2>& ra_iter, const size_type index = 0) {
			assert(0 == index); return base_class(construction_helper3(ra_iter.target_container_ptr()), ra_iter.position());
		}
		static auto construction_helper5(std::true_type, _TElement * const ptr, const size_type index = 0) {
			return base_class(mse::us::impl::TXScopeRuntimeRawArrayProxyAndSelfPointer<_TElement>(ptr, std::numeric_limits<size_type>::max()), index);
		}
		template<typename _TParam1>
		static auto construction_helper5(std::false_type, const _TParam1& param1, const size_type index = 0) {
#if defined(MSE_MSTDARRAY_DISABLED) || defined(MSE_MSTDVECTOR_DISABLED)
			return construction_helper9(typename mse::impl::is_non_pointer_iterator<_TParam1>::type(), param1, index);
#else // defined(MSE_MSTDARRAY_DISABLED) || defined(MSE_MSTDVECTOR_DISABLED)
			return construction_helper6(param1, index);
#endif // defined(MSE_MSTDARRAY_DISABLED) || defined(MSE_MSTDVECTOR_DISABLED)
		}
		template<typename _TParam1>
		static auto construction_helper10(std::true_type, const _TParam1& native_array, const size_type index = 0) {
			assert(0 == index); return base_class(mse::us::impl::TXScopeRuntimeRawArrayProxyAndSelfPointer<_TElement>(native_array, mse::impl::native_array_size_msemsearray(native_array)), index);
		}
		template<typename _TParam1>
		static auto construction_helper10(std::false_type, const _TParam1& param1, const size_type index = 0) {
			typedef _TElement * element_ptr_t;
			return construction_helper5(typename std::is_convertible<decltype(param1), element_ptr_t>::type(), param1, index);
		}
		template<typename _TParam1>
		static auto construction_helper4(std::false_type, const _TParam1& param1, const size_type index = 0) {
			return construction_helper10(typename mse::impl::IsNativeArray_msemsearray<decltype(param1)>::type(), param1, index);
		}
		template<typename _TRAContainer2>
		static mse::us::impl::TXScopeRuntimeRawArrayProxyAndSelfPointer<_TElement> construction_helper3(const mse::TXScopeFixedPointer<_TRAContainer2>& ra_container_pointer) {
			auto size1 = mse::container_size(*ra_container_pointer);
			if (0 >= size1) {
				return mse::us::impl::TXScopeRuntimeRawArrayProxyAndSelfPointer<_TElement>(nullptr, size1);
			}
			else {
				return mse::us::impl::TXScopeRuntimeRawArrayProxyAndSelfPointer<_TElement>(std::addressof((*ra_container_pointer)[0]), size1);
			}
		}
		template<typename _TRAIterator2>
		static auto construction_helper2(std::true_type, const _TRAIterator2& ra_iter, const size_type index = 0) {
			assert(0 == index); return base_class(construction_helper3(static_cast<const TXScopeCSSSXSRAIterator<typename container_type_from_iterator_type<_TRAIterator2>::type>&>(ra_iter).target_container_ptr()), ra_iter.position());
		}
		template<typename _TParam1>
		static auto construction_helper2(std::false_type, const _TParam1& param1, const size_type index = 0) {
			return construction_helper4(std::false_type(), param1, index);
		}
		template<typename _TParam1>
		static auto construction_helper2(std::false_type, _TParam1&& param1, const size_type index = 0) {
			return construction_helper4(std::false_type(), param1, index);
		}

		static auto construction_helper1(std::true_type, const TXScopeCSSSXSTERAIterator& ra_iter, const size_type index = 0) { assert(0 == index); return base_class(ra_iter); }
		static auto construction_helper1(std::true_type, TXScopeCSSSXSTERAIterator&& ra_iter, const size_type index = 0) { assert(0 == index); return base_class(MSE_FWD(ra_iter)); }
		template<typename _TParam1>
		static auto construction_helper1(std::false_type, const _TParam1& param1, const size_type index = 0) {
			return construction_helper2(typename is_convertible_to_TXScopeCSSSXSRAIterator<const _TParam1&>::type(), param1, index);
		}
		template<typename _TParam1>
		static auto construction_helper1(std::false_type, _TParam1&& param1, const size_type index = 0) {
			return construction_helper2(typename is_convertible_to_TXScopeCSSSXSRAIterator<decltype(param1)>::type(), MSE_FWD(param1), index);
		}

		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		friend class TXScopeCSSSXSTERAConstIterator<_TElement>;
	};

	template <typename _TElement>
	class TXScopeCSSSXSTERAConstIterator : public TXScopeCSSSStrongRAConstIterator<mse::us::impl::TXScopeRuntimeRawArrayConstProxyAndSelfPointer<_TElement> > {
	public:
		typedef  TXScopeCSSSStrongRAConstIterator<mse::us::impl::TXScopeRuntimeRawArrayConstProxyAndSelfPointer<_TElement> > base_class;
		typedef mse::us::impl::TXScopeRuntimeRawArrayConstProxyAndSelfPointer<_TElement> _TRAContainerPointer;
		MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

	private:

		struct CDummy {};
		template<typename _Ty2, typename _TRAIterator2>
		struct container_pointer_type_from_iterator_type {
			typedef decltype(std::declval<_TRAIterator2>().target_container_ptr()) type;
		};
		template<typename _TRAIterator2>
		struct container_pointer_type_from_iterator_type<std::false_type, _TRAIterator2> {
			typedef CDummy* const type;
		};
		template<typename _TRAIterator2>
		struct container_type_from_iterator_type {
			typedef typename container_pointer_type_from_iterator_type<typename mse::impl::HasOrInheritsTargetContainerPtrMethod_msemsearray<_TRAIterator2>::type, _TRAIterator2>::type TRAContainerPointer;
			typedef mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(*std::declval<TRAContainerPointer>())> > TRAContainer;
			typedef TRAContainer type;
		};

		template<typename _Ty2, typename _TRAIterator2, typename _TRAContainer2 = typename container_type_from_iterator_type<_TRAIterator2>::type>
		struct is_convertible_to_TXScopeCSSSXSRAConstIterator_helper1 : std::is_convertible<_TRAIterator2, TXScopeCSSSXSRAConstIterator<_TRAContainer2> > {};
		template<typename _TRAIterator2>
		struct is_convertible_to_TXScopeCSSSXSRAConstIterator_helper1<std::false_type, _TRAIterator2> : std::integral_constant<bool, false> {};
		template<typename _TRAIterator2>
		struct is_convertible_to_TXScopeCSSSXSRAConstIterator : std::integral_constant<bool
			, is_convertible_to_TXScopeCSSSXSRAConstIterator_helper1<typename mse::impl::HasOrInheritsTargetContainerPtrMethod_msemsearray<_TRAIterator2>::type, _TRAIterator2>::value> {};

	public:
		TXScopeCSSSXSTERAConstIterator(const TXScopeCSSSXSTERAConstIterator& src) : base_class(src) {}
		TXScopeCSSSXSTERAConstIterator(TXScopeCSSSXSTERAConstIterator&& src) : base_class(MSE_FWD(src)) {}

		TXScopeCSSSXSTERAConstIterator(const TXScopeCSSSXSTERAIterator<_TElement>& src) : base_class(src) {}
		TXScopeCSSSXSTERAConstIterator(TXScopeCSSSXSTERAIterator<_TElement>&& src) : base_class(MSE_FWD(src)) {}

		template <typename _TRAContainer2>
		TXScopeCSSSXSTERAConstIterator(const TXScopeCSSSXSRAConstIterator<_TRAContainer2>& src) : base_class(construction_helper3(src.target_container_ptr()), src.position()) {}
		template <typename _TRAContainer2>
		TXScopeCSSSXSTERAConstIterator(const TXScopeCSSSXSRAIterator<_TRAContainer2>& src) : base_class(construction_helper3(src.target_container_ptr()), src.position()) {}

		template<typename _TRAIterator2>
		TXScopeCSSSXSTERAConstIterator(const _TRAIterator2& ra_iter, size_type index = 0)
			: base_class(construction_helper1(typename mse::impl::disjunction<std::is_base_of<TXScopeCSSSXSTERAConstIterator, _TRAIterator2>
				, std::is_base_of<TXScopeCSSSXSTERAIterator<_TElement>, _TRAIterator2> >::type(), ra_iter, index)) {}
		template<typename _TRAIterator2>
		TXScopeCSSSXSTERAConstIterator(_TRAIterator2&& ra_iter, size_type index = 0)
			: base_class(construction_helper1(typename mse::impl::disjunction<std::is_base_of<TXScopeCSSSXSTERAConstIterator, mse::impl::remove_reference_t<decltype(ra_iter)> >
				, std::is_base_of<TXScopeCSSSXSTERAIterator<_TElement>, mse::impl::remove_reference_t<decltype(ra_iter)> > >::type(), MSE_FWD(ra_iter), index)) {}

		auto& operator=(const TXScopeCSSSXSTERAConstIterator& _Right_cref) { base_class::operator=(_Right_cref); return (*this); }
		auto& operator=(TXScopeCSSSXSTERAConstIterator&& _Right_cref) { base_class::operator=(MSE_FWD(_Right_cref)); return (*this); }
		MSE_USING_ASSIGNMENT_OPERATOR(base_class);

		MSE_INHERIT_ITERATOR_ARITHMETIC_OPERATORS_FROM(base_class, TXScopeCSSSXSTERAConstIterator);

		MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(base_class);
		void xscope_iterator_tag() const {}
		void xscope_tag() const {}

	private:

		template<typename _TRAIterator2>
		static auto construction_helper9(std::true_type, const _TRAIterator2& ra_iter, const size_type index = 0) {
			_TElement const * const ptr = std::addressof(*ra_iter);
			return base_class(mse::us::impl::TXScopeRuntimeRawArrayConstProxyAndSelfPointer<_TElement>(ptr, std::numeric_limits<size_type>::max()), index);
		}
		template<typename _TParam1>
		static auto construction_helper9(std::false_type, const _TParam1& param1, const size_type index = 0) {
			return construction_helper6(param1, index);
		}
#if defined(MSE_MSTDARRAY_DISABLED) || defined(MSE_MSTDVECTOR_DISABLED)
		template <typename _TRAContainer2>
		static auto construction_helper8(const mse::TXScopeFixedConstPointer<_TRAContainer2>& ptr, const size_type index = 0) {
			return base_class(construction_helper3(ptr), index);
		}
#else // defined(MSE_MSTDARRAY_DISABLED) || defined(MSE_MSTDVECTOR_DISABLED)
		template<size_t _Size>
		static auto construction_helper8(const mse::TXScopeFixedConstPointer<std::array<_TElement, _Size> >& ptr, const size_type index = 0) {
			return base_class(construction_helper3(ptr), index);
		}
#endif // defined(MSE_MSTDARRAY_DISABLED) || defined(MSE_MSTDVECTOR_DISABLED)
		template <typename _TRAContainer2>
		static auto construction_helper7(const mse::TXScopeFixedConstPointer<_TRAContainer2>& ptr, const size_type index = 0) {
			return construction_helper8(ptr, index);
		}
		template <typename _TRAContainer2>
		static auto construction_helper7(const mse::TXScopeFixedPointer<_TRAContainer2>& ptr, const size_type index = 0) {
			return construction_helper8(mse::TXScopeFixedConstPointer<_TRAContainer2>(ptr), index);
		}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		template <typename _TRAContainer2>
		static auto construction_helper7(const mse::TXScopeObjFixedConstPointer<_TRAContainer2>& ptr, const size_type index = 0) {
			return construction_helper8(mse::TXScopeFixedConstPointer<_TRAContainer2>(ptr), index);
		}
		template <typename _TRAContainer2>
		static auto construction_helper7(const mse::TXScopeObjFixedPointer<_TRAContainer2>& ptr, const size_type index = 0) {
			return construction_helper8(mse::TXScopeFixedConstPointer<_TRAContainer2>(ptr), index);
		}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
		template <typename _TRAContainerPointer2>
		static auto construction_helper6(const mse::TXScopeRAConstIterator<_TRAContainerPointer2>& ra_iter, const size_type index = 0) {
			/* If you get a compile error here, it means that the given iterator (or arguments) is not convertible to
			the specified TXScopeCSSSXSTERAIterator<>. A common case is when trying to convert an rvalue (temporary) scope
			iterator of a dynamic (structure lockable) container (like a vector), where only lvalues are supported. */
			assert(0 == index); return construction_helper7(ra_iter.target_container_ptr(), ra_iter.position());
		}
		template <typename _TRAContainerPointer2>
		static auto construction_helper6(const mse::TXScopeRAIterator<_TRAContainerPointer2>& ra_iter, const size_type index = 0) {
			/* If you get a compile error here, it means that the given iterator (or arguments) is not convertible to
			the specified TXScopeCSSSXSTERAIterator<>. A common case is when trying to convert an rvalue (temporary) scope
			iterator of a dynamic (structure lockable) container (like a vector), where only lvalues are supported. */
			assert(0 == index); return construction_helper7(ra_iter.target_container_ptr(), ra_iter.position());
		}
		template <typename _TRAContainer2>
		static auto construction_helper6(const TXScopeCSSSXSRAIterator<_TRAContainer2>& ra_iter, const size_type index = 0) {
			assert(0 == index); return base_class(construction_helper3(ra_iter.target_container_ptr()), ra_iter.position());
		}
		template <typename _TRAContainer2>
		static auto construction_helper6(const TXScopeCSSSXSRAConstIterator<_TRAContainer2>& ra_iter, const size_type index = 0) {
			assert(0 == index); return base_class(construction_helper3(ra_iter.target_container_ptr()), ra_iter.position());
		}
		static auto construction_helper5(std::true_type, _TElement const * const ptr, const size_type index = 0) {
			return base_class(mse::us::impl::TXScopeRuntimeRawArrayConstProxyAndSelfPointer<_TElement>(ptr, std::numeric_limits<size_type>::max()), index);
		}
		template<typename _TParam1>
		static auto construction_helper5(std::false_type, const _TParam1& param1, const size_type index = 0) {
#if defined(MSE_MSTDARRAY_DISABLED) || defined(MSE_MSTDVECTOR_DISABLED)
			return construction_helper9(typename mse::impl::is_non_pointer_iterator<_TParam1>::type(), param1, index);
#else // defined(MSE_MSTDARRAY_DISABLED) || defined(MSE_MSTDVECTOR_DISABLED)
			return construction_helper6(param1, index);
#endif // defined(MSE_MSTDARRAY_DISABLED) || defined(MSE_MSTDVECTOR_DISABLED)
		}
		template<typename _TParam1>
		static auto construction_helper10(std::true_type, const _TParam1& native_array, const size_type index = 0) {
			assert(0 == index); return base_class(mse::us::impl::TXScopeRuntimeRawArrayConstProxyAndSelfPointer<_TElement>(native_array, mse::impl::native_array_size_msemsearray(native_array)), index);
		}
		template<typename _TParam1>
		static auto construction_helper10(std::false_type, const _TParam1& param1, const size_type index = 0) {
			typedef _TElement const * element_cptr_t;
			return construction_helper5(typename std::is_convertible<decltype(param1), element_cptr_t>::type(), param1, index);
		}
		template<typename _TRAIterator2>
		static auto construction_helper4(std::true_type, const _TRAIterator2& ra_iter, const size_type index = 0) {
			const TXScopeCSSSXSRAIterator<typename container_type_from_iterator_type<_TRAIterator2>::type>& ra_iter2 = ra_iter;
			TXScopeCSSSXSRAConstIterator<typename container_type_from_iterator_type<_TRAIterator2>::type> ra_iter3 = ra_iter2;
			assert(0 == index);
			return construction_helper2(std::true_type(), ra_iter3, index);
		}
		template<typename _TParam1>
		static auto construction_helper4(std::false_type, const _TParam1& param1, const size_type index = 0) {
			return construction_helper10(typename mse::impl::IsNativeArray_msemsearray<decltype(param1)>::type(), param1, index);
		}
		template<typename _TRAContainer2>
		static mse::us::impl::TXScopeRuntimeRawArrayConstProxyAndSelfPointer<_TElement> construction_helper3(const mse::TXScopeFixedConstPointer<_TRAContainer2>& ra_container_pointer) {
			auto size1 = mse::container_size(*ra_container_pointer);
			if (0 >= size1) {
				return mse::us::impl::TXScopeRuntimeRawArrayConstProxyAndSelfPointer<_TElement>(nullptr, size1);
			}
			else {
				return mse::us::impl::TXScopeRuntimeRawArrayConstProxyAndSelfPointer<_TElement>(std::addressof((*ra_container_pointer)[0]), size1);
			}
		}
		template<typename _TRAIterator2>
		static auto construction_helper2(std::true_type, const _TRAIterator2& ra_iter, const size_type index = 0) {
			assert(0 == index); return base_class(construction_helper3(static_cast<const TXScopeCSSSXSRAConstIterator<typename container_type_from_iterator_type<_TRAIterator2>::type>&>(ra_iter).target_container_ptr()), ra_iter.position());
		}
		template<typename _TParam1>
		static auto construction_helper2(std::false_type, const _TParam1& param1, const size_type index = 0) {
			return construction_helper4(typename TXScopeCSSSXSTERAIterator<_TElement>::template is_convertible_to_TXScopeCSSSXSRAIterator<const _TParam1&>::type(), param1, index);
		}
		template<typename _TParam1>
		static auto construction_helper2(std::false_type, _TParam1&& param1, const size_type index = 0) {
			return construction_helper4(typename TXScopeCSSSXSTERAIterator<_TElement>::template is_convertible_to_TXScopeCSSSXSRAIterator<decltype(param1)>::type(), param1, index);
		}

		static auto construction_helper1(std::true_type, const TXScopeCSSSXSTERAConstIterator& ra_iter, const size_type index = 0) { assert(0 == index); return base_class(ra_iter); }
		static auto construction_helper1(std::true_type, TXScopeCSSSXSTERAConstIterator&& ra_iter, const size_type index = 0) { assert(0 == index); return base_class(MSE_FWD(ra_iter)); }
		static auto construction_helper1(std::true_type, const TXScopeCSSSXSTERAIterator<_TElement>& ra_iter, const size_type index = 0) { assert(0 == index); return base_class(ra_iter); }
		static auto construction_helper1(std::true_type, TXScopeCSSSXSTERAIterator<_TElement>&& ra_iter, const size_type index = 0) { assert(0 == index); return base_class(MSE_FWD(ra_iter)); }
		template<typename _TParam1>
		static auto construction_helper1(std::false_type, const _TParam1& param1, const size_type index = 0) {
			return construction_helper2(typename is_convertible_to_TXScopeCSSSXSRAConstIterator<const _TParam1&>::type(), param1, index);
		}
		template<typename _TParam1>
		static auto construction_helper1(std::false_type, _TParam1&& param1, const size_type index = 0) {
			return construction_helper2(typename is_convertible_to_TXScopeCSSSXSRAConstIterator<decltype(param1)>::type(), MSE_FWD(param1), index);
		}

		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
	};

	namespace rsv {

		/* Template specializations of TFParam<>. */

		template<typename _Ty>
		class TFParam<mse::TXScopeCSSSXSTERAConstIterator<_Ty> > : public TXScopeCSSSXSTERAConstIterator<_Ty> {
		public:
			typedef TXScopeCSSSXSTERAConstIterator<_Ty> base_class;
			MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);
			MSE_FPARAM_RVALUE_TO_LVALUE_LONE_PARAM_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TFParam, base_class);

			MSE_INHERIT_ITERATOR_ARITHMETIC_OPERATORS_FROM(base_class, TFParam);

			void xscope_not_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};

		template<typename _Ty>
		class TFParam<const mse::TXScopeCSSSXSTERAConstIterator<_Ty> > : public TXScopeCSSSXSTERAConstIterator<_Ty> {
		public:
			typedef TXScopeCSSSXSTERAConstIterator<_Ty> base_class;
			MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);
			MSE_FPARAM_RVALUE_TO_LVALUE_LONE_PARAM_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TFParam, base_class);

			MSE_INHERIT_ITERATOR_ARITHMETIC_OPERATORS_FROM(base_class, TFParam);

			void xscope_not_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};

		template<typename _Ty>
		class TFParam<mse::TXScopeCSSSXSTERAIterator<_Ty> > : public TXScopeCSSSXSTERAIterator<_Ty> {
		public:
			typedef TXScopeCSSSXSTERAIterator<_Ty> base_class;
			MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);
			MSE_FPARAM_RVALUE_TO_LVALUE_LONE_PARAM_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TFParam, base_class);

			MSE_INHERIT_ITERATOR_ARITHMETIC_OPERATORS_FROM(base_class, TFParam);

			void xscope_not_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};

		template<typename _Ty>
		class TFParam<const mse::TXScopeCSSSXSTERAIterator<_Ty> > : public TXScopeCSSSXSTERAIterator<_Ty> {
		public:
			typedef TXScopeCSSSXSTERAIterator<_Ty> base_class;
			MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);
			MSE_FPARAM_RVALUE_TO_LVALUE_LONE_PARAM_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TFParam, base_class);

			MSE_INHERIT_ITERATOR_ARITHMETIC_OPERATORS_FROM(base_class, TFParam);

			void xscope_not_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};
	}

	template <typename _TRAContainerPointer>
	using TXScopeRandomAccessIterator = TXScopeRAIterator<_TRAContainerPointer>;
	template <typename _TRAContainerPointer>
	using TRandomAccessIterator = TRAIterator<_TRAContainerPointer>;
	template <typename _TRAContainerPointer>
	using TXScopeRandomAccessConstIterator = TXScopeRAConstIterator<_TRAContainerPointer>;
	template <typename _TRAContainerPointer>
	using TRandomAccessConstIterator = TRAConstIterator<_TRAContainerPointer>;
	template <typename _TElement>
	using TXScopeCSSSXSTERandomAccessConstIterator = TXScopeCSSSXSTERAConstIterator<_TElement>;
	template <typename _TElement>
	using TXScopeCSSSXSTERandomAccessIterator = TXScopeCSSSXSTERAIterator<_TElement>;

	template <typename _TRAContainerPointer>
	auto make_xscope_random_access_iterator(_TRAContainerPointer&& ra_container_pointer, typename TXScopeRandomAccessIterator<mse::impl::remove_reference_t<_TRAContainerPointer> >::size_type index = 0) {
		typedef mse::impl::remove_reference_t<_TRAContainerPointer> _TRAContainerPointerRR;
		return TXScopeRandomAccessIterator<_TRAContainerPointerRR>(std::forward<_TRAContainerPointer>(ra_container_pointer), index);
	}
	template <typename _TRAContainerPointer>
	auto make_xscope_random_access_iterator(const _TRAContainerPointer& ra_container_pointer, typename TXScopeRandomAccessIterator<mse::impl::remove_reference_t<_TRAContainerPointer> >::size_type index = 0) {
		typedef mse::impl::remove_reference_t<_TRAContainerPointer> _TRAContainerPointerRR;
		return TXScopeRandomAccessIterator<_TRAContainerPointerRR>(ra_container_pointer, index);
	}

	template <typename _TRAContainerPointer, class = MSE_IMPL_ENABLE_IF_NOT_RETURNABLE_FPARAM(_TRAContainerPointer)>
	auto make_random_access_iterator(_TRAContainerPointer&& ra_container_pointer, typename TRandomAccessIterator<mse::impl::remove_reference_t<_TRAContainerPointer> >::size_type index = 0) {
		typedef mse::impl::remove_reference_t<_TRAContainerPointer> _TRAContainerPointerRR;
		return TRandomAccessIterator<_TRAContainerPointerRR>(std::forward<_TRAContainerPointer>(ra_container_pointer), index);
	}
	template <typename _TRAContainerPointer>
	auto make_random_access_iterator(const _TRAContainerPointer& ra_container_pointer, typename TRandomAccessIterator<mse::impl::remove_reference_t<_TRAContainerPointer> >::size_type index = 0) {
		typedef mse::impl::remove_reference_t<_TRAContainerPointer> _TRAContainerPointerRR;
		return TRandomAccessIterator<_TRAContainerPointerRR>(ra_container_pointer, index);
	}

	template <typename _TRAContainerPointer, class = MSE_IMPL_ENABLE_IF_NOT_RETURNABLE_FPARAM(_TRAContainerPointer)>
	auto make_xscope_random_access_const_iterator(_TRAContainerPointer&& ra_container_pointer, typename TXScopeRandomAccessConstIterator<mse::impl::remove_reference_t<_TRAContainerPointer> >::size_type index = 0) {
		typedef mse::impl::remove_reference_t<_TRAContainerPointer> _TRAContainerPointerRR;
		return TXScopeRandomAccessConstIterator<_TRAContainerPointerRR>(std::forward<_TRAContainerPointer>(ra_container_pointer), index);
	}
	template <typename _TRAContainerPointer>
	auto make_xscope_random_access_const_iterator(const _TRAContainerPointer& ra_container_pointer, typename TXScopeRandomAccessConstIterator<mse::impl::remove_reference_t<_TRAContainerPointer> >::size_type index = 0) {
		typedef mse::impl::remove_reference_t<_TRAContainerPointer> _TRAContainerPointerRR;
		return TXScopeRandomAccessConstIterator<_TRAContainerPointerRR>(ra_container_pointer, index);
	}
	/* Overloads for rsv::TReturnableFParam<>. */
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_random_access_const_iterator)
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_random_access_iterator)

	template <typename _TRAContainerPointer>
	auto make_random_access_const_iterator(_TRAContainerPointer&& ra_container_pointer, typename TRandomAccessConstIterator<mse::impl::remove_reference_t<_TRAContainerPointer> >::size_type index = 0) {
		typedef mse::impl::remove_reference_t<_TRAContainerPointer> _TRAContainerPointerRR;
		return TRandomAccessConstIterator<_TRAContainerPointerRR>(std::forward<_TRAContainerPointer>(ra_container_pointer), index);
	}
	template <typename _TRAContainerPointer>
	auto make_random_access_const_iterator(const _TRAContainerPointer& ra_container_pointer, typename TRandomAccessConstIterator<mse::impl::remove_reference_t<_TRAContainerPointer> >::size_type index = 0) {
		typedef mse::impl::remove_reference_t<_TRAContainerPointer> _TRAContainerPointerRR;
		return TRandomAccessConstIterator<_TRAContainerPointerRR>(ra_container_pointer, index);
	}

	/* Overloads for rsv::TReturnableFParam<>. */
	template <typename _Ty, typename _TSize = size_t>
	auto make_random_access_const_iterator(const rsv::TReturnableFParam<_Ty>& ra_container_pointer, _TSize count = 0) {
		typedef typename rsv::TReturnableFParam<_Ty>::base_class ra_container_pointer_base_class;
		ra_container_pointer_base_class const * ra_container_pointer_base_pointer = std::addressof(ra_container_pointer);
		typedef decltype(make_random_access_const_iterator(std::declval<ra_container_pointer_base_class>(), count)) base_return_type;
		return rsv::TReturnableFParam<base_return_type>(make_random_access_const_iterator(*ra_container_pointer_base_pointer, count));
	}
	template <typename _Ty, typename _TSize = size_t>
	auto make_random_access_iterator(const rsv::TReturnableFParam<_Ty>& ra_container_pointer, _TSize count = 0) {
		typedef typename rsv::TReturnableFParam<_Ty>::base_class ra_container_pointer_base_class;
		ra_container_pointer_base_class const * ra_container_pointer_base_pointer = std::addressof(ra_container_pointer);
		typedef decltype(make_random_access_iterator(std::declval<ra_container_pointer_base_class>(), count)) base_return_type;
		return rsv::TReturnableFParam<base_return_type>(make_random_access_iterator(*ra_container_pointer_base_pointer, count));
	}


	namespace impl {
		namespace ns_nii_array {
			/* Following are a bunch of template (iterator) classes that, organizationally, should be members of nii_array<>. (And they
			used to be.) However, being a member of nii_array<> makes them "dependent types", and dependent types do not participate
			in automatic template parameter type deduction. So we had to haul them here outside of nii_array<>. */

			/* The reason we specify the default parameter in the definition instead of this forward declaration is that there seems to be a
			bug in clang (3.8.0) such that if we don't specify the default parameter in the definition it seems to subsequently behave as if
			one were never specified. g++ and msvc don't seem to have the same issue. */
			template<typename _TArrayPointer, class _Ty, size_t _Size, class _TStateMutex>
			class Tarray_ss_iterator_type;

			/* Tarray_ss_const_iterator_type is a bounds checked const_iterator. */
			template<typename _TArrayConstPointer, class _Ty, size_t _Size, class _TStateMutex>
			class Tarray_ss_const_iterator_type : public mse::TFriendlyAugmentedRAConstIterator<mse::TRAConstIterator<_TArrayConstPointer> > {
			public:
				typedef mse::TFriendlyAugmentedRAConstIterator<mse::TRAConstIterator<_TArrayConstPointer> > base_class;
				MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

				template<class _TArrayConstPointer2 = _TArrayConstPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_TArrayConstPointer2, _TArrayConstPointer>::value) && (std::is_default_constructible<_TArrayConstPointer>::value)> MSE_IMPL_EIS >
				Tarray_ss_const_iterator_type() {}

				Tarray_ss_const_iterator_type(const _TArrayConstPointer& owner_cptr) : base_class(owner_cptr) {}
				Tarray_ss_const_iterator_type(_TArrayConstPointer&& owner_cptr) : base_class(MSE_FWD(owner_cptr)) {}

				Tarray_ss_const_iterator_type(Tarray_ss_const_iterator_type&& src) = default;
				Tarray_ss_const_iterator_type(const Tarray_ss_const_iterator_type& src) = default;
				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2, _TArrayConstPointer>::value> MSE_IMPL_EIS >
				Tarray_ss_const_iterator_type(const Tarray_ss_const_iterator_type<_Ty2, _Ty, _Size, _TStateMutex>& src) : base_class(src.target_container_ptr(), src.position()) {}
				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2, _TArrayConstPointer>::value> MSE_IMPL_EIS >
				Tarray_ss_const_iterator_type(const Tarray_ss_iterator_type<_Ty2, _Ty, _Size, _TStateMutex>& src) : base_class(src.target_container_ptr(), src.position()) {}

				MSE_USING_ASSIGNMENT_OPERATOR(base_class);
				auto& operator=(Tarray_ss_const_iterator_type&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); }
				auto& operator=(const Tarray_ss_const_iterator_type& _X) { base_class::operator=(_X); return (*this); }

				Tarray_ss_const_iterator_type& operator ++() { (*this).set_to_next(); return (*this); }
				Tarray_ss_const_iterator_type operator++(int) { Tarray_ss_const_iterator_type _Tmp = *this; (*this).set_to_next(); return (_Tmp); }
				Tarray_ss_const_iterator_type& operator --() { (*this).set_to_previous(); return (*this); }
				Tarray_ss_const_iterator_type operator--(int) { Tarray_ss_const_iterator_type _Tmp = *this; (*this).set_to_previous(); return (_Tmp); }

				Tarray_ss_const_iterator_type& operator +=(difference_type n) { (*this).advance(n); return (*this); }
				Tarray_ss_const_iterator_type& operator -=(difference_type n) { (*this).regress(n); return (*this); }
				Tarray_ss_const_iterator_type operator+(difference_type n) const {
					Tarray_ss_const_iterator_type retval(*this);
					retval.advance(n);
					return retval;
				}
				Tarray_ss_const_iterator_type operator-(difference_type n) const { return ((*this) + (-n)); }
				difference_type operator-(const base_class& _Right_cref) const {
					return base_class::operator-(_Right_cref);
				}

				void set_to_const_item_pointer(const Tarray_ss_const_iterator_type& _Right_cref) {
					(*this) = _Right_cref;
				}

				MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(_TArrayConstPointer);

			private:

				//friend class /*_Myt*/nii_array<_Ty, _Size>;
			};
			/* Tarray_ss_iterator_type is a bounds checked iterator. */
			template<typename _TArrayPointer, class _Ty, size_t _Size, class _TStateMutex>
			class Tarray_ss_iterator_type : public mse::TFriendlyAugmentedRAIterator<mse::TRAIterator<_TArrayPointer> > {
			public:
				typedef mse::TFriendlyAugmentedRAIterator<mse::TRAIterator<_TArrayPointer> > base_class;
				MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

				template<class _TArrayPointer2 = _TArrayPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_TArrayPointer2, _TArrayPointer>::value) && (std::is_default_constructible<_TArrayPointer>::value)> MSE_IMPL_EIS >
				Tarray_ss_iterator_type() {}

				Tarray_ss_iterator_type(const _TArrayPointer& owner_ptr) : base_class(owner_ptr) {}
				Tarray_ss_iterator_type(_TArrayPointer&& owner_ptr) : base_class(MSE_FWD(owner_ptr)) {}

				Tarray_ss_iterator_type(Tarray_ss_iterator_type&& src) = default;
				Tarray_ss_iterator_type(const Tarray_ss_iterator_type& src) = default;
				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2, _TArrayPointer>::value> MSE_IMPL_EIS >
				Tarray_ss_iterator_type(const Tarray_ss_iterator_type<_Ty2, _Ty, _Size, _TStateMutex>& src) : base_class(src.target_container_ptr(), src.position()) {}

				MSE_USING_ASSIGNMENT_OPERATOR(base_class);
				auto& operator=(Tarray_ss_iterator_type&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); }
				auto& operator=(const Tarray_ss_iterator_type& _X) { base_class::operator=(_X); return (*this); }

				Tarray_ss_iterator_type& operator ++() { (*this).set_to_next(); return (*this); }
				Tarray_ss_iterator_type operator++(int) { Tarray_ss_iterator_type _Tmp = *this; (*this).set_to_next(); return (_Tmp); }
				Tarray_ss_iterator_type& operator --() { (*this).set_to_previous(); return (*this); }
				Tarray_ss_iterator_type operator--(int) { Tarray_ss_iterator_type _Tmp = *this; (*this).set_to_previous(); return (_Tmp); }

				Tarray_ss_iterator_type& operator +=(difference_type n) { (*this).advance(n); return (*this); }
				Tarray_ss_iterator_type& operator -=(difference_type n) { (*this).regress(n); return (*this); }
				Tarray_ss_iterator_type operator+(difference_type n) const {
					Tarray_ss_iterator_type retval(*this);
					retval.advance(n);
					return retval;
				}
				Tarray_ss_iterator_type operator-(difference_type n) const { return ((*this) + (-n)); }
				difference_type operator-(const base_class& _Right_cref) const {
					return base_class::operator-(_Right_cref);
				}

				void set_to_item_pointer(const Tarray_ss_iterator_type& _Right_cref) {
					(*this) = _Right_cref;
				}

				MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(_TArrayPointer);

			private:

				//friend class /*_Myt*/nii_array<_Ty, _Size>;
				template<typename _TArrayConstPointer, class _Ty2, size_t _Size2, class _TStateMutex2>
				friend class Tarray_ss_const_iterator_type;
			};

			template<typename _TArrayPointer, class _Ty, size_t _Size, class _TStateMutex/*, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_potentially_not_xscope<_TArrayPointer>::value)> MSE_IMPL_EIS*/>
			using Tarray_ss_reverse_iterator_type = std::reverse_iterator<Tarray_ss_iterator_type<_TArrayPointer, _Ty, _Size, _TStateMutex> >;
			template<typename _TArrayConstPointer, class _Ty, size_t _Size, class _TStateMutex/*, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_potentially_not_xscope<_TArrayConstPointer>::value)> MSE_IMPL_EIS*/>
			using Tarray_ss_const_reverse_iterator_type = std::reverse_iterator<Tarray_ss_const_iterator_type<_TArrayConstPointer, _Ty, _Size, _TStateMutex> >;

			template<class _Ty, size_t _Size, class _TStateMutex>
			class Tnii_array_xscope_ss_iterator_type;

			template<class _Ty, size_t _Size, class _TStateMutex>
			class Tnii_array_xscope_ss_const_iterator_type : public mse::TFriendlyAugmentedRAConstIterator<mse::TXScopeCSSSStrongRAConstIterator<mse::TXScopeFixedConstPointer<const mse::nii_array<_Ty, _Size> > > >
				/*, public mse::us::impl::StrongPointerAsyncNotShareableAndNotPassableTagBase*/ {
			public:
				typedef mse::TFriendlyAugmentedRAConstIterator<mse::TXScopeCSSSStrongRAConstIterator<mse::TXScopeFixedConstPointer<const mse::nii_array<_Ty, _Size> > > > base_class;
				MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

				MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(Tnii_array_xscope_ss_const_iterator_type, base_class);

				MSE_USING_ASSIGNMENT_OPERATOR(base_class);
				auto& operator=(Tnii_array_xscope_ss_const_iterator_type&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); }
				auto& operator=(const Tnii_array_xscope_ss_const_iterator_type& _X) { base_class::operator=(_X); return (*this); }

				Tnii_array_xscope_ss_const_iterator_type& operator ++() { base_class::operator ++(); return (*this); }
				Tnii_array_xscope_ss_const_iterator_type operator++(int) { Tnii_array_xscope_ss_const_iterator_type _Tmp = *this; base_class::operator++(); return (_Tmp); }
				Tnii_array_xscope_ss_const_iterator_type& operator --() { base_class::operator --(); return (*this); }
				Tnii_array_xscope_ss_const_iterator_type operator--(int) { Tnii_array_xscope_ss_const_iterator_type _Tmp = *this; base_class::operator--(); return (_Tmp); }

				Tnii_array_xscope_ss_const_iterator_type& operator +=(difference_type n) { base_class::operator +=(n); return (*this); }
				Tnii_array_xscope_ss_const_iterator_type& operator -=(difference_type n) { base_class::operator -=(n); return (*this); }
				Tnii_array_xscope_ss_const_iterator_type operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
				Tnii_array_xscope_ss_const_iterator_type operator-(difference_type n) const { return ((*this) + (-n)); }
				difference_type operator-(const Tnii_array_xscope_ss_const_iterator_type& _Right_cref) const { return base_class::operator-(_Right_cref); }

				void set_to_const_item_pointer(const Tnii_array_xscope_ss_const_iterator_type& _Right_cref) { base_class::set_to_const_item_pointer(_Right_cref); }

				MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(base_class);
				void xscope_ss_iterator_type_tag() const {}

			private:
				MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

				friend class /*_Myt*/nii_array<_Ty, _Size>;
				template<class _Ty2, size_t _Size2, class _TStateMutex2>
				friend class Tnii_array_xscope_ss_iterator_type;
			};
			template<class _Ty, size_t _Size, class _TStateMutex>
			class Tnii_array_xscope_ss_iterator_type : public mse::TFriendlyAugmentedRAIterator<mse::TXScopeCSSSStrongRAIterator<mse::TXScopeFixedPointer<mse::nii_array<_Ty, _Size> > > >
				/*, public mse::us::impl::XScopeContainsNonOwningScopeReferenceTagBase, public mse::us::impl::StrongPointerAsyncNotShareableAndNotPassableTagBase*/ {
			public:
				typedef mse::TFriendlyAugmentedRAIterator<mse::TXScopeCSSSStrongRAIterator<mse::TXScopeFixedPointer<mse::nii_array<_Ty, _Size> > > > base_class;
				MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

				MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(Tnii_array_xscope_ss_iterator_type, base_class);

				MSE_USING_ASSIGNMENT_OPERATOR(base_class);
				auto& operator=(Tnii_array_xscope_ss_iterator_type&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); }
				auto& operator=(const Tnii_array_xscope_ss_iterator_type& _X) { base_class::operator=(_X); return (*this); }

				Tnii_array_xscope_ss_iterator_type& operator ++() { base_class::operator ++(); return (*this); }
				Tnii_array_xscope_ss_iterator_type operator++(int) { Tnii_array_xscope_ss_iterator_type _Tmp = *this; base_class::operator++(); return (_Tmp); }
				Tnii_array_xscope_ss_iterator_type& operator --() { base_class::operator --(); return (*this); }
				Tnii_array_xscope_ss_iterator_type operator--(int) { Tnii_array_xscope_ss_iterator_type _Tmp = *this; base_class::operator--(); return (_Tmp); }

				Tnii_array_xscope_ss_iterator_type& operator +=(difference_type n) { base_class::operator +=(n); return (*this); }
				Tnii_array_xscope_ss_iterator_type& operator -=(difference_type n) { base_class::operator -=(n); return (*this); }
				Tnii_array_xscope_ss_iterator_type operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
				Tnii_array_xscope_ss_iterator_type operator-(difference_type n) const { return ((*this) + (-n)); }
				difference_type operator-(const Tnii_array_xscope_ss_iterator_type& _Right_cref) const { return base_class::operator-(_Right_cref); }

				void set_to_item_pointer(const Tnii_array_xscope_ss_iterator_type& _Right_cref) { base_class::set_to_item_pointer(_Right_cref); }

				MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(base_class);
				void xscope_ss_iterator_type_tag() const {}

			private:
				MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

				friend class /*_Myt*/nii_array<_Ty, _Size>;
			};
		}
	}

	template<class _Ty, class _TStateMutex>
	using adjusted_state_mutex = mse::impl::conditional_t<std::is_same<_TStateMutex, default_state_mutex>::value
		, container_adjusted_default_state_mutex<_Ty>, _TStateMutex>;

	namespace us {
		namespace impl {
			template<class _Ty, size_t _Size, class _TStateMutex/* = container_adjusted_default_state_mutex<_Ty>*/>
			class nii_array_base : private mse::impl::TOpaqueWrapper<std::array<_Ty, _Size> >, private adjusted_state_mutex<_Ty, _TStateMutex>, public mse::us::impl::ContiguousSequenceStaticStructureContainerTagBase {
			public:
				/* A (non-thread safe) mutex is used to enforce safety against potentially mischievous element constructors/destructors.
				There are no such concerns with trivial element types, so in those cases we'll just use a "no-op dummy" mutex. */
				typedef adjusted_state_mutex<_Ty, _TStateMutex> state_mutex_t;

				/* We (privately) inherit the underlying data type rather than make it a data member to ensure it's the "first" component in the structure.*/
				typedef mse::impl::TOpaqueWrapper<std::array<_Ty, _Size> > base_class;
				typedef std::array<_Ty, _Size> std_array;
				typedef std_array _MA;
				typedef nii_array_base _Myt;

				typedef mse::impl::random_access_iterator_base<_Ty> ra_it_base;
				typedef typename ra_it_base::value_type value_type;
				typedef typename ra_it_base::difference_type difference_type;
				typedef typename ra_it_base::pointer pointer;
				typedef typename ra_it_base::const_pointer const_pointer;
				typedef typename ra_it_base::reference reference;
				typedef typename ra_it_base::const_reference const_reference;
				typedef typename ra_it_base::size_type size_type;

			private:
				const _MA& contained_array() const& { return (*this).value(); }
				//const _MA& contained_array() const&& { return (*this).value(); }
				_MA& contained_array()& { return (*this).value(); }
				_MA&& contained_array()&& { return std::move(*this).value(); }

				state_mutex_t& state_mutex1()& { return (*this); }

			public:
				nii_array_base() {}
				nii_array_base(_MA&& _X) : base_class(MSE_FWD(_X)) {}
				nii_array_base(const _MA& _X) : base_class(_X) {}
				nii_array_base(_Myt&& _X) : base_class(MSE_FWD(_X).contained_array()) {}
				nii_array_base(const _Myt& _X) : base_class(_X.contained_array()) {}
				//nii_array_base(_XSTD initializer_list<typename _MA::base_class::value_type> _Ilist) : base_class(_Ilist) {}
			private:
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
			public:
				constexpr nii_array_base(_XSTD initializer_list<_Ty> _Ilist) : base_class(std_array_initial_value(std::is_default_constructible<_Ty>(), _Ilist)) {
					/* std::array<> is an "aggregate type" (basically a POD struct with no base class, constructors or private
					data members (details here: http://en.cppreference.com/w/cpp/language/aggregate_initialization)). As such,
					support for construction from initializer list is automatically generated by the compiler. Specifically,
					aggregate types support "aggregate initialization". But since mstd::array has a member with an explicitly
					defined constructor (or at least I think that's why), it is not an aggregate type and therefore doesn't
					qualify to have support for "aggregate initialization" automatically generated by the compiler. So we try
					to emulate it. */
				}

				~nii_array_base() {
					mse::impl::destructor_lock_guard1<state_mutex_t> lock1(state_mutex1());
				}

				operator _MA() const & { return contained_array(); }
				operator _MA() && { return std::forward<_MA>(contained_array()); }

				typename std_array::const_reference operator[](msear_size_t _P) const {
					return (*this).at(msear_as_a_size_t(_P));
				}
				typename std_array::reference operator[](msear_size_t _P) {
					return (*this).at(msear_as_a_size_t(_P));
				}
				typename std_array::reference front() {	// return first element of mutable sequence
					if (0 == (*this).size()) { MSE_THROW(nii_array_range_error("front() on empty - typename std_array::reference front() - nii_array_base")); }
					return contained_array().front();
				}
				_CONST_FUN typename std_array::const_reference front() const {	// return first element of nonmutable sequence
					if (0 == (*this).size()) { MSE_THROW(nii_array_range_error("front() on empty - typename std_array::const_reference front() - nii_array_base")); }
					return contained_array().front();
				}
				typename std_array::reference back() {	// return last element of mutable sequence
					if (0 == (*this).size()) { MSE_THROW(nii_array_range_error("back() on empty - typename std_array::reference back() - nii_array_base")); }
					return contained_array().back();
				}
				_CONST_FUN typename std_array::const_reference back() const {	// return last element of nonmutable sequence
					if (0 == (*this).size()) { MSE_THROW(nii_array_range_error("back() on empty - typename std_array::const_reference back() - nii_array_base")); }
					return contained_array().back();
				}

				void fill(const _Ty& _Value)
				{	// assign value to all elements
					std::lock_guard<state_mutex_t> lock1(state_mutex1());
					contained_array().fill(_Value);
				}

				void swap(_Myt& _Other) {	// swap contents with _Other
					std::lock_guard<state_mutex_t> lock1(state_mutex1());
					contained_array().swap(_Other.contained_array());
				}

				void swap(_MA& _Other) {	// swap contents with _Other
					std::lock_guard<state_mutex_t> lock1(state_mutex1());
					contained_array().swap(_Other);
				}

				size_type size() const _NOEXCEPT
				{	// return length of sequence
					return contained_array().size();
				}

				size_type max_size() const _NOEXCEPT
				{	// return maximum possible length of sequence
					return contained_array().max_size();
				}

				_CONST_FUN bool empty() const _NOEXCEPT
				{	// test if sequence is empty
					return contained_array().empty();
				}

				reference at(msear_size_t _Pos)
				{	// subscript mutable sequence with checking
					return contained_array().at(msear_as_a_size_t(_Pos));
				}

				const_reference at(msear_size_t _Pos) const
				{	// subscript nonmutable sequence with checking
					return contained_array().at(msear_as_a_size_t(_Pos));
				}

				value_type* data() _NOEXCEPT
				{	// return pointer to mutable data array
					return contained_array().data();
				}

				const value_type* data() const _NOEXCEPT
				{	// return pointer to nonmutable data array
					return contained_array().data();
				}

				nii_array_base& operator=(const nii_array_base& _Right_cref) {
					std::lock_guard<state_mutex_t> lock1(state_mutex1());
					contained_array() = _Right_cref.contained_array();
					return (*this);
				}

				//typedef xscope_ss_const_iterator_type xscope_const_iterator;
				//typedef xscope_ss_iterator_type xscope_iterator;
				typedef TXScopeCSSSXSRAConstIterator<_Myt> xscope_const_iterator;
				typedef TXScopeCSSSXSRAIterator<_Myt> xscope_iterator;

				template<typename _TArrayConstPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_potentially_not_xscope<_TArrayConstPointer>::value)> MSE_IMPL_EIS >
				using Tss_const_iterator_type = mse::impl::ns_nii_array::Tarray_ss_const_iterator_type<_TArrayConstPointer, _Ty, _Size, _TStateMutex>;
				template<typename _TArrayPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_potentially_not_xscope<_TArrayPointer>::value)> MSE_IMPL_EIS >
				using Tss_iterator_type = mse::impl::ns_nii_array::Tarray_ss_iterator_type<_TArrayPointer, _Ty, _Size, _TStateMutex>;

				template<typename _TArrayPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_potentially_not_xscope<_TArrayPointer>::value)> MSE_IMPL_EIS >
				using Tss_reverse_iterator_type = mse::impl::ns_nii_array::Tarray_ss_reverse_iterator_type<_TArrayPointer, _Ty, _Size, _TStateMutex>;
				template<typename _TArrayConstPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_potentially_not_xscope<_TArrayConstPointer>::value)> MSE_IMPL_EIS >
				using Tss_const_reverse_iterator_type = mse::impl::ns_nii_array::Tarray_ss_const_reverse_iterator_type<_TArrayConstPointer, _Ty, _Size, _TStateMutex>;

				template<typename _TArrayPointer>
				static auto ss_begin(const _TArrayPointer& owner_ptr) {
					mse::impl::T_valid_if_not_an_xscope_type<_TArrayPointer>();
					typedef mse::impl::conditional_t<std::is_const<mse::impl::remove_reference_t<decltype(*owner_ptr)> >::value
						, Tss_const_iterator_type<_TArrayPointer>, Tss_iterator_type<_TArrayPointer> > return_type;
					return_type retval(owner_ptr);
					retval.set_to_beginning();
					return retval;
				}

				template<typename _TArrayPointer>
				static auto ss_end(const _TArrayPointer& owner_ptr) {
					mse::impl::T_valid_if_not_an_xscope_type<_TArrayPointer>();
					typedef mse::impl::conditional_t<std::is_const<mse::impl::remove_reference_t<decltype(*owner_ptr)> >::value
						, Tss_const_iterator_type<_TArrayPointer>, Tss_iterator_type<_TArrayPointer> > return_type;
					return_type retval(owner_ptr);
					retval.set_to_end_marker();
					return retval;
				}

				template<typename _TArrayPointer>
				static auto ss_cbegin(const _TArrayPointer& owner_ptr) {
					mse::impl::T_valid_if_not_an_xscope_type<_TArrayPointer>();
					Tss_const_iterator_type<_TArrayPointer> retval(owner_ptr);
					retval.set_to_beginning();
					return retval;
				}

				template<typename _TArrayPointer>
				static auto ss_cend(const _TArrayPointer& owner_ptr) {
					mse::impl::T_valid_if_not_an_xscope_type<_TArrayPointer>();
					Tss_const_iterator_type<_TArrayPointer> retval(owner_ptr);
					retval.set_to_end_marker();
					return retval;
				}

				template<typename _TArrayPointer>
				static auto ss_rbegin(const _TArrayPointer& owner_ptr) {
					mse::impl::T_valid_if_not_an_xscope_type<_TArrayPointer>();
					typedef mse::impl::conditional_t<std::is_const<mse::impl::remove_reference_t<decltype(*owner_ptr)> >::value
						, Tss_const_reverse_iterator_type<_TArrayPointer>, Tss_reverse_iterator_type<_TArrayPointer> > return_type;
					return return_type(ss_end<_TArrayPointer>(owner_ptr));
				}

				template<typename _TArrayPointer>
				static auto ss_rend(const _TArrayPointer& owner_ptr) {
					mse::impl::T_valid_if_not_an_xscope_type<_TArrayPointer>();
					typedef mse::impl::conditional_t<std::is_const<mse::impl::remove_reference_t<decltype(*owner_ptr)> >::value
						, Tss_const_reverse_iterator_type<_TArrayPointer>, Tss_reverse_iterator_type<_TArrayPointer> > return_type;
					return return_type(ss_begin<_TArrayPointer>(owner_ptr));
				}

				template<typename _TArrayPointer>
				static auto ss_crbegin(const _TArrayPointer& owner_ptr) {
					mse::impl::T_valid_if_not_an_xscope_type<_TArrayPointer>();
					return (Tss_const_reverse_iterator_type<_TArrayPointer>(ss_cend<_TArrayPointer>(owner_ptr)));
				}

				template<typename _TArrayPointer>
				static auto ss_crend(const _TArrayPointer& owner_ptr) {
					mse::impl::T_valid_if_not_an_xscope_type<_TArrayPointer>();
					return (Tss_const_reverse_iterator_type<_TArrayPointer>(ss_crbegin<_TArrayPointer>(owner_ptr)));
				}

			private:
				template<typename _TArrayPointer>
				static auto xscope_ss_begin_helper1(std::true_type, const _TArrayPointer& owner_ptr) {
					xscope_iterator retval(owner_ptr);
					retval.set_to_beginning();
					return retval;
				}
				template<typename _TArrayPointer>
				static auto xscope_ss_begin_helper1(std::false_type, const _TArrayPointer& owner_ptr) {
					return xscope_ss_cbegin(owner_ptr);
				}
				template<typename _TArrayPointer>
				static auto xscope_ss_end_helper1(std::true_type, const _TArrayPointer& owner_ptr) {
					xscope_iterator retval(owner_ptr);
					retval.set_to_end_marker();
					return retval;
				}
				template<typename _TArrayPointer>
				static auto xscope_ss_end_helper1(std::false_type, const _TArrayPointer& owner_ptr) {
					return xscope_ss_cend(owner_ptr);
				}
			public:
				template<typename _TArrayPointer, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_TArrayPointer, mse::TXScopeFixedPointer<_Myt> >::value || std::is_convertible<_TArrayPointer, mse::TXScopeFixedConstPointer<_Myt> >::value> MSE_IMPL_EIS >
				static auto xscope_ss_begin(const _TArrayPointer& owner_ptr) {
					return xscope_ss_begin_helper1(typename std::is_convertible<_TArrayPointer, mse::TXScopeFixedPointer<_Myt> >::type(), owner_ptr);
				}
				template<typename _TArrayPointer, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_TArrayPointer, mse::TXScopeFixedPointer<_Myt> >::value || std::is_convertible<_TArrayPointer, mse::TXScopeFixedConstPointer<_Myt> >::value> MSE_IMPL_EIS >
				static auto xscope_ss_end(const _TArrayPointer& owner_ptr) {
					return xscope_ss_end_helper1(typename std::is_convertible<_TArrayPointer, mse::TXScopeFixedPointer<_Myt> >::type(), owner_ptr);
				}
				static auto xscope_ss_cbegin(const mse::TXScopeFixedConstPointer<_Myt>& owner_ptr) {
					xscope_const_iterator retval(owner_ptr);
					retval.set_to_beginning();
					return retval;
				}
				static auto xscope_ss_cend(const mse::TXScopeFixedConstPointer<_Myt>& owner_ptr) {
					xscope_const_iterator retval(owner_ptr);
					retval.set_to_end_marker();
					return retval;
				}

				static auto xscope_ss_rbegin(const mse::TXScopeFixedPointer<_Myt>& owner_ptr) {
					return xscope_ss_reverse_iterator_type(xscope_ss_end(owner_ptr));
				}
				static auto xscope_ss_rend(const mse::TXScopeFixedPointer<_Myt>& owner_ptr) {
					return xscope_ss_reverse_iterator_type(xscope_ss_begin(owner_ptr));
				}
				static auto xscope_ss_crbegin(const mse::TXScopeFixedConstPointer<_Myt>& owner_ptr) {
					return (xscope_ss_const_reverse_iterator_type(xscope_ss_cend(owner_ptr)));
				}
				static auto xscope_ss_crend(const mse::TXScopeFixedConstPointer<_Myt>& owner_ptr) {
					return (xscope_ss_const_reverse_iterator_type(xscope_ss_crbegin(owner_ptr)));
				}
				static auto xscope_ss_rbegin(const mse::TXScopeFixedConstPointer<_Myt>& owner_ptr) { return xscope_ss_crbegin(owner_ptr); }
				static auto xscope_ss_rend(const mse::TXScopeFixedConstPointer<_Myt>& owner_ptr) { return xscope_ss_crend(owner_ptr); }


				bool operator==(const _Myt& _Right) const {	// test for array equality
					return (_Right.contained_array() == contained_array());
				}
				bool operator<(const _Myt& _Right) const {	// test if _Left < _Right for arrays
					return (contained_array() < _Right.contained_array());
				}

				template<class _Ty2, class _Traits2>
				std::basic_ostream<_Ty2, _Traits2>& write_bytes(std::basic_ostream<_Ty2, _Traits2>& _Ostr, size_type byte_count, const size_type byte_start_offset = 0) const {
					const auto array_size_in_bytes = mse::msear_as_a_size_t(sizeof(_Ty) * (*this).size());
					auto byte_ptr = reinterpret_cast<const char*>((*this).contained_array().data());
					if ((array_size_in_bytes <= byte_start_offset) || (0 >= byte_count)) {
						return _Ostr;
					}
					else {
						byte_ptr += mse::msear_as_a_size_t(byte_start_offset);
						return _Ostr.write(byte_ptr, std::min(mse::msear_as_a_size_t(array_size_in_bytes - byte_start_offset), mse::msear_as_a_size_t(byte_count)));
					}
				}
				template<class _Ty2, class _Traits2>
				std::basic_ostream<_Ty2, _Traits2>& write_bytes(std::basic_ostream<_Ty2, _Traits2>& _Ostr) const {
					return write_bytes(_Ostr, mse::msear_as_a_size_t(sizeof(_Ty) * (*this).size()));
				}

			private:
				friend class mse::nii_array<_Ty, _Size>;
				friend class mse::xscope_nii_array<_Ty, _Size>;
				friend class mse::us::msearray<_Ty, _Size, _TStateMutex>;
			};
		}
	}

	/* nii_array<> is essentially a memory-safe array that does not expose (unprotected) non-static member functions
	like begin() or end() which return (memory) unsafe iterators. It does provide static member function templates
	like ss_begin<>(...) and ss_end<>(...) which take a pointer parameter and return a (bounds-checked) iterator that
	inherits the safety of the given pointer. nii_array<> also supports "scope" iterators which are safe without any
	run-time overhead. nii_array<> is a data type that is eligible to be shared between asynchronous threads. */
	template<class _Ty, size_t _Size>
	class nii_array : public mse::us::impl::nii_array_base<_Ty, _Size, container_adjusted_default_state_mutex<_Ty> > {
	public:
		typedef mse::us::impl::nii_array_base<_Ty, _Size, container_adjusted_default_state_mutex<_Ty> > base_class;
		typedef container_adjusted_default_state_mutex<_Ty> _TStateMutex;
		typedef std::array<_Ty, _Size> std_array;
		typedef std_array _MA;
		typedef nii_array _Myt;

		typedef mse::impl::random_access_iterator_base<_Ty> ra_it_base;
		typedef typename ra_it_base::value_type value_type;
		typedef typename ra_it_base::difference_type difference_type;
		typedef typename ra_it_base::pointer pointer;
		typedef typename ra_it_base::const_pointer const_pointer;
		typedef typename ra_it_base::reference reference;
		typedef typename ra_it_base::const_reference const_reference;
		typedef typename ra_it_base::size_type size_type;

	private:
		const _MA& contained_array() const& { return base_class::contained_array(); }
		//const _MA& contained_array() const&& { return base_class::contained_array(); }
		_MA& contained_array()& { return base_class::contained_array(); }
		_MA&& contained_array()&& { return std::forward<_MA>(base_class::contained_array()); }

	public:
		nii_array() {}
		nii_array(_MA&& _X) : base_class(MSE_FWD(_X)) {}
		nii_array(const _MA& _X) : base_class(_X) {}
		nii_array(_Myt&& _X) : base_class(MSE_FWD(_X).contained_array()) {}
		nii_array(const _Myt& _X) : base_class(_X.contained_array()) {}
		constexpr nii_array(_XSTD initializer_list<_Ty> _Ilist) : base_class(_Ilist) {}

		~nii_array() {
			/* This is just a no-op function that will cause a compile error when _Ty is not an eligible type. */
			valid_if_Ty_is_not_an_xscope_type();
		}
		operator _MA() const & { return contained_array(); }
		operator _MA() && { return std::forward<_MA>(contained_array()); }


		nii_array& operator=(const nii_array& _Right_cref) {
			base_class::operator=(_Right_cref);
			return (*this);
		}

		//class na_const_iterator_base : public std::iterator<std::random_access_iterator_tag, value_type, difference_type, const_pointer, const_reference> {};
		//class na_iterator_base : public std::iterator<std::random_access_iterator_tag, value_type, difference_type, pointer, reference> {};
		typedef mse::impl::random_access_const_iterator_base<_Ty> na_const_iterator_base;
		typedef mse::impl::random_access_iterator_base<_Ty> na_iterator_base;

		typedef mse::impl::ns_nii_array::Tnii_array_xscope_ss_const_iterator_type<_Ty, _Size, _TStateMutex> xscope_ss_const_iterator_type;
		typedef mse::impl::ns_nii_array::Tnii_array_xscope_ss_iterator_type<_Ty, _Size, _TStateMutex> xscope_ss_iterator_type;

		//typedef xscope_ss_const_iterator_type xscope_const_iterator;
		//typedef xscope_ss_iterator_type xscope_iterator;
		typedef TXScopeCSSSXSRAConstIterator<_Myt> xscope_const_iterator;
		typedef TXScopeCSSSXSRAIterator<_Myt> xscope_iterator;

		template<typename _TArrayPointer>
		static auto ss_begin(const _TArrayPointer& owner_ptr) {
			return base_class::template ss_begin(owner_ptr);
		}
		template<typename _TArrayPointer>
		static auto ss_end(const _TArrayPointer& owner_ptr) {
			return base_class::template ss_end(owner_ptr);
		}
		template<typename _TArrayPointer>
		static auto ss_cbegin(const _TArrayPointer& owner_ptr) {
			return base_class::template ss_cbegin(owner_ptr);
		}
		template<typename _TArrayPointer>
		static auto ss_cend(const _TArrayPointer& owner_ptr) {
			return base_class::template ss_cend(owner_ptr);
		}
		template<typename _TArrayPointer>
		static auto ss_rbegin(const _TArrayPointer& owner_ptr) {
			return base_class::template ss_rbegin(owner_ptr);
		}
		template<typename _TArrayPointer>
		static auto ss_rend(const _TArrayPointer& owner_ptr) {
			return base_class::template ss_rend(owner_ptr);
		}
		template<typename _TArrayPointer>
		static auto ss_crbegin(const _TArrayPointer& owner_ptr) {
			return base_class::template ss_crbegin(owner_ptr);
		}
		template<typename _TArrayPointer>
		static auto ss_crend(const _TArrayPointer& owner_ptr) {
			return base_class::template ss_crend(owner_ptr);
		}

		template<typename _TArrayPointer, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_TArrayPointer, mse::TXScopeFixedPointer<_Myt> >::value || std::is_convertible<_TArrayPointer, mse::TXScopeFixedConstPointer<_Myt> >::value> MSE_IMPL_EIS >
		static auto xscope_ss_begin(const _TArrayPointer& owner_ptr) {
			return base_class::template xscope_ss_begin(owner_ptr);
		}
		template<typename _TArrayPointer, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_TArrayPointer, mse::TXScopeFixedPointer<_Myt> >::value || std::is_convertible<_TArrayPointer, mse::TXScopeFixedConstPointer<_Myt> >::value> MSE_IMPL_EIS >
		static auto xscope_ss_end(const _TArrayPointer& owner_ptr) {
			return base_class::template xscope_ss_end(owner_ptr);
		}

		bool operator==(const _Myt& _Right) const {	// test for array equality
			return base_class::operator==(_Right);
		}
		bool operator<(const _Myt& _Right) const {	// test if _Left < _Right for arrays
			return base_class::operator<(_Right);
		}

		MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(_Ty);

	private:
		/* If _Ty is an xscope type, then the following member function will not instantiate, causing an
		(intended) compile error. */
		template<class _Ty2 = _Ty, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _Ty>::value) && (mse::impl::is_potentially_not_xscope<_Ty2>::value)> MSE_IMPL_EIS >
		void valid_if_Ty_is_not_an_xscope_type() const {}

		friend /*class */xscope_ss_const_iterator_type;
		friend /*class */xscope_ss_iterator_type;
		friend class us::msearray<_Ty, _Size, _TStateMutex>;

		template<size_t _Idx, class _Tz, size_t _Size2>
		friend _CONST_FUN _Tz& std::get(mse::nii_array<_Tz, _Size2>& _Arr) _NOEXCEPT;
		template<size_t _Idx, class _Tz, size_t _Size2>
		friend _CONST_FUN const _Tz& std::get(const mse::nii_array<_Tz, _Size2>& _Arr) _NOEXCEPT;
		template<size_t _Idx, class _Tz, size_t _Size2>
		friend _CONST_FUN _Tz&& std::get(mse::nii_array<_Tz, _Size2>&& _Arr) _NOEXCEPT;

		friend void swap(_Myt& a, _Myt& b) { a.swap(b); }
		friend void swap(_Myt& a, _MA& b) { a.swap(b); }
		friend void swap(_MA& a, _Myt& b) { b.swap(a); }
	};

#ifdef MSE_HAS_CXX17
	/* deduction guides */
	namespace impl {
		template<class _First,
			class... _Rest>
			struct _mse_Enforce_same
		{
			static_assert(std::conjunction<std::is_same<_First, _Rest>...>::value,
				"N4687 26.3.7.2 [array.cons]/2: "
				"Requires: (is_same_v<T, U> && ...) is true. Otherwise the program is ill-formed.");
			using type = _First;
		};
	}

	template<class _First,
		class... _Rest>
		nii_array(_First, _Rest...)
		->nii_array<typename impl::_mse_Enforce_same<_First, _Rest...>::type, 1 + sizeof...(_Rest)>;
#endif /* MSE_HAS_CXX17 */

	template<class _Ty, size_t _Size> inline bool operator!=(const nii_array<_Ty, _Size>& _Left,
		const nii_array<_Ty, _Size>& _Right) {	// test for array inequality
		return (!(_Left == _Right));
	}

	template<class _Ty, size_t _Size> inline bool operator>(const nii_array<_Ty, _Size>& _Left,
		const nii_array<_Ty, _Size>& _Right) {	// test if _Left > _Right for arrays
		return (_Right < _Left);
	}

	template<class _Ty, size_t _Size> inline bool operator<=(const nii_array<_Ty, _Size>& _Left,
		const nii_array<_Ty, _Size>& _Right) {	// test if _Left <= _Right for arrays
		return (!(_Right < _Left));
	}

	template<class _Ty, size_t _Size> inline bool operator>=(const nii_array<_Ty, _Size>& _Left,
		const nii_array<_Ty, _Size>& _Right) {	// test if _Left >= _Right for arrays
		return (!(_Left < _Right));
	}

	template<class _Ty, size_t _Size>
	TXScopeFixedPointer<_Ty> xscope_pointer_to_array_element(const typename nii_array<_Ty, _Size>::xscope_ss_iterator_type& iter_cref) {
		return mse::us::unsafe_make_xscope_pointer_to(*iter_cref);
	}
	template<class _Ty, size_t _Size>
	TXScopeFixedPointer<_Ty> xscope_pointer_to_array_element(const mse::TXScopeFixedPointer<nii_array<_Ty, _Size> >& ptr, typename nii_array<_Ty, _Size>::size_type _P) {
		return mse::us::unsafe_make_xscope_pointer_to((*ptr)[_P]);
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, size_t _Size>
	TXScopeFixedPointer<_Ty> xscope_pointer_to_array_element(const mse::TXScopeObjFixedPointer<nii_array<_Ty, _Size> >& ptr, typename nii_array<_Ty, _Size>::size_type _P) {
		return mse::us::unsafe_make_xscope_pointer_to((*ptr)[_P]);
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	template<class _Ty, size_t _Size>
	TXScopeFixedConstPointer<_Ty> xscope_const_pointer_to_array_element(const typename nii_array<_Ty, _Size>::xscope_ss_const_iterator_type& iter_cref) {
		return mse::us::unsafe_make_xscope_const_pointer_to(*iter_cref);
	}
	template<class _Ty, size_t _Size>
	TXScopeFixedConstPointer<_Ty> xscope_const_pointer_to_array_element(const mse::TXScopeFixedConstPointer<nii_array<_Ty, _Size> >& ptr, typename nii_array<_Ty, _Size>::size_type _P) {
		return mse::us::unsafe_make_xscope_const_pointer_to((*ptr)[_P]);
	}
	template<class _Ty, size_t _Size>
	TXScopeFixedConstPointer<_Ty> xscope_const_pointer_to_array_element(const mse::TXScopeFixedPointer<nii_array<_Ty, _Size> >& ptr, typename nii_array<_Ty, _Size>::size_type _P) {
		return mse::us::unsafe_make_xscope_const_pointer_to((*ptr)[_P]);
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, size_t _Size>
	TXScopeFixedConstPointer<_Ty> xscope_const_pointer_to_array_element(const mse::TXScopeObjFixedConstPointer<nii_array<_Ty, _Size> >& ptr, typename nii_array<_Ty, _Size>::size_type _P) {
		return mse::us::unsafe_make_xscope_const_pointer_to((*ptr)[_P]);
	}
	template<class _Ty, size_t _Size>
	TXScopeFixedConstPointer<_Ty> xscope_const_pointer_to_array_element(const mse::TXScopeObjFixedPointer<nii_array<_Ty, _Size> >& ptr, typename nii_array<_Ty, _Size>::size_type _P) {
		return mse::us::unsafe_make_xscope_const_pointer_to((*ptr)[_P]);
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	namespace impl {

		/* Some algorithm implementation specializations for nii_array<>.  */

		/* Provides raw pointer iterators from the given "random access" container. */
		template<class _Container>
		class TXScopeRangeRawPointerIterProvider {
		public:
			typedef decltype(std::addressof(std::declval<_Container>()[0])) raw_pointer_iter_t;
			typedef decltype(std::declval<raw_pointer_iter_t>() - std::declval<raw_pointer_iter_t>()) difference_type;

			TXScopeRangeRawPointerIterProvider(_Container& container) {
				const auto size = mse::container_size(container);
				if (1 <= size) {
					m_begin = std::addressof(container[0]);
					m_end = m_begin + difference_type(size);
				}
			}
			const auto& begin() const {
				return m_begin;
			}
			const auto& end() const {
				return m_end;
			}

		private:
			raw_pointer_iter_t m_begin = nullptr;
			raw_pointer_iter_t m_end = nullptr;
		};

		template<class _Ty, size_t _Size>
		auto make_xscope_specialized_range_iter_provider_overloaded(mse::nii_array< _Ty, _Size>& array_ref) {
			return TXScopeRangeRawPointerIterProvider<mse::impl::remove_reference_t<decltype(array_ref)> >(array_ref);
		}
		template<class _Ty, size_t _Size>
		auto make_xscope_specialized_range_iter_provider_overloaded(const mse::nii_array< _Ty, _Size>& array_ref) {
			return TXScopeRangeRawPointerIterProvider<mse::impl::remove_reference_t<decltype(array_ref)> >(array_ref);
		}
	}
}

namespace std {

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmismatched-tags"
#endif /*__clang__*/

	template<class _Ty, size_t _Size>
	struct tuple_size<mse::nii_array<_Ty, _Size> >
		: integral_constant<size_t, _Size>
	{	// struct to determine number of elements in array
	};

	template<size_t _Idx, class _Ty, size_t _Size>
	struct tuple_element<_Idx, mse::nii_array<_Ty, _Size> >
	{	// struct to determine type of element _Idx in array
		static_assert(_Idx < _Size, "array index out of bounds");

		typedef _Ty type;
	};

#ifdef __clang__
#pragma clang diagnostic pop
#endif /*__clang__*/

	// TUPLE INTERFACE TO array
	template<size_t _Idx, class _Ty, size_t _Size>
	_CONST_FUN _Ty& get(mse::nii_array<_Ty, _Size>& _Arr) _NOEXCEPT
	{	// return element at _Idx in array _Arr
		static_assert(_Idx < _Size, "array index out of bounds");
		return (std::get<_Idx>(_Arr.contained_array()));
	}

	template<size_t _Idx, class _Ty, size_t _Size>
	_CONST_FUN const _Ty& get(const mse::nii_array<_Ty, _Size>& _Arr) _NOEXCEPT
	{	// return element at _Idx in array _Arr
		static_assert(_Idx < _Size, "array index out of bounds");
		return (std::get<_Idx>(_Arr.contained_array()));
	}

	template<size_t _Idx, class _Ty, size_t _Size>
	_CONST_FUN _Ty&& get(mse::nii_array<_Ty, _Size>&& _Arr) _NOEXCEPT
	{	// return element at _Idx in array _Arr
		static_assert(_Idx < _Size, "array index out of bounds");
		return (_STD move(std::get<_Idx>(_Arr.contained_array())));
	}

	template<class _Ty, size_t _Size, class _TStateMutex = mse::default_state_mutex/*, class = enable_if_t<_Size == 0 || _Is_swappable<_Ty>::value>*/>
	void swap(mse::nii_array<_Ty, _Size>& _Left, mse::nii_array<_Ty, _Size>& _Right) _NOEXCEPT
	{
		_Left.swap(_Right);
	}
	template<class _Ty, size_t _Size, class _TStateMutex = mse::default_state_mutex/*, class = enable_if_t<_Size == 0 || _Is_swappable<_Ty>::value>*/>
	void swap(array<_Ty, _Size>& _Left, mse::nii_array<_Ty, _Size>& _Right) _NOEXCEPT_OP(_NOEXCEPT_OP(_Right.swap(_Left)))
	{	// swap arrays
		return (_Right.swap(_Left));
	}
	template<class _Ty, size_t _Size, class _TStateMutex = mse::default_state_mutex/*, class = enable_if_t<_Size == 0 || _Is_swappable<_Ty>::value>*/>
	void swap(mse::nii_array<_Ty, _Size>& _Left, array<_Ty, _Size>& _Right) _NOEXCEPT
	{
		_Left.swap(_Right);
	}
}

namespace mse {

	template<typename _TContainerPointer>
	auto ss_begin(const _TContainerPointer& owner_ptr) {
		return owner_ptr->ss_begin(owner_ptr);
	}
	template<typename _TContainerPointer>
	static auto ss_end(const _TContainerPointer& owner_ptr) {
		return owner_ptr->ss_end(owner_ptr);
	}
	template<typename _TContainerPointer>
	static auto ss_cbegin(const _TContainerPointer& owner_ptr) {
		return owner_ptr->ss_cbegin(owner_ptr);
	}
	template<typename _TContainerPointer>
	static auto ss_cend(const _TContainerPointer& owner_ptr) {
		return owner_ptr->ss_cend(owner_ptr);
	}
	template<typename _TContainerPointer>
	static auto ss_rbegin(const _TContainerPointer& owner_ptr) {
		return owner_ptr->ss_rbegin(owner_ptr);
	}
	template<typename _TContainerPointer>
	static auto ss_rend(const _TContainerPointer& owner_ptr) {
		return owner_ptr->ss_rend(owner_ptr);
	}
	template<typename _TContainerPointer>
	static auto ss_crbegin(const _TContainerPointer& owner_ptr) {
		return owner_ptr->ss_crbegin(owner_ptr);
	}
	template<typename _TContainerPointer>
	static auto ss_crend(const _TContainerPointer& owner_ptr) {
		return owner_ptr->ss_crend(owner_ptr);
	}

	template<typename _TContainerPointer1>
	auto& front(const _TContainerPointer1& this_ptr) {	// return first element of mutable sequence
		return this_ptr->front();
	}
	template<typename _TContainerPointer1>
	auto& back(const _TContainerPointer1& this_ptr) {	// return last element of mutable sequence
		return this_ptr->back();
	}
	template<typename _TContainerPointer1, typename size_type>
	auto& at(const _TContainerPointer1& this_ptr, size_type _Pos)
	{	// subscript mutable sequence with checking
		return this_ptr->at(_Pos);
	}
	template<typename _TContainerPointer1, typename _Ty>
	void fill(const _TContainerPointer1& this_ptr, const _Ty& _Value)
	{	// assign value to all elements
		this_ptr->fill(_Value);
	}
	template<typename _TContainerPointer1>
	auto size(const _TContainerPointer1& this_ptr) _NOEXCEPT
	{	// return length of sequence
		return this_ptr->size();
	}
	template<typename _TContainerPointer1>
	auto max_size(const _TContainerPointer1& this_ptr) _NOEXCEPT
	{	// return maximum possible length of sequence
		return this_ptr->max_size();
	}
	template<typename _TContainerPointer1>
	auto empty(const _TContainerPointer1& this_ptr) _NOEXCEPT
	{	// test if sequence is empty
		return this_ptr->empty();
	}


	/* xscope_nii_array<> is the scope version of nii_array<> (which unlike nii_array<>, can hold scope objects). */
	template<class _Ty, size_t _Size>
	class xscope_nii_array : public mse::us::impl::nii_array_base<_Ty, _Size, container_adjusted_default_state_mutex<_Ty> >, public mse::us::impl::XScopeTagBase
		, public mse::impl::first_or_placeholder_if_not_base_of_second<mse::us::impl::ReferenceableByScopePointerTagBase, _Ty, xscope_nii_array<_Ty, _Size> >
		, public mse::impl::first_or_placeholder_if_not_base_of_second<mse::us::impl::ContainsNonOwningScopeReferenceTagBase, _Ty, xscope_nii_array<_Ty, _Size> >
	{
	public:
		typedef mse::us::impl::nii_array_base<_Ty, _Size, container_adjusted_default_state_mutex<_Ty> > base_class;
		typedef container_adjusted_default_state_mutex<_Ty> _TStateMutex;
		typedef std::array<_Ty, _Size> std_array;
		typedef std_array _MA;
		typedef xscope_nii_array _Myt;

		typedef mse::impl::random_access_iterator_base<_Ty> ra_it_base;
		typedef typename ra_it_base::value_type value_type;
		typedef typename ra_it_base::difference_type difference_type;
		typedef typename ra_it_base::pointer pointer;
		typedef typename ra_it_base::const_pointer const_pointer;
		typedef typename ra_it_base::reference reference;
		typedef typename ra_it_base::const_reference const_reference;
		typedef typename ra_it_base::size_type size_type;

	private:
		const _MA& contained_array() const& { return base_class::contained_array(); }
		//const _MA& contained_array() const&& { return base_class::contained_array(); }
		_MA& contained_array()& { return base_class::contained_array(); }
		_MA&& contained_array()&& { return std::forward<_MA>(base_class::contained_array()); }

	public:
		xscope_nii_array() {}
		xscope_nii_array(_MA&& _X) : base_class(MSE_FWD(_X)) {}
		xscope_nii_array(const _MA& _X) : base_class(_X) {}
		xscope_nii_array(_Myt&& _X) : base_class(MSE_FWD(_X).contained_array()) {}
		xscope_nii_array(const _Myt& _X) : base_class(_X.contained_array()) {}
		constexpr xscope_nii_array(_XSTD initializer_list<_Ty> _Ilist) : base_class(_Ilist) {}

		operator _MA() const& { return contained_array(); }
		operator _MA() && { return std::forward<_MA>(contained_array()); }

		void assign(const _Ty& _Value)
		{	// assign value to all elements
			valid_if_Ty_is_not_marked_as_containing_a_scope_reference<_Ty>();
			valid_if_Ty_is_not_marked_as_containing_an_accessible_scope_address_of_operator<_Ty>();
			base_class::assign(_Value);
		}

		void fill(const _Ty& _Value)
		{	// assign value to all elements
			valid_if_Ty_is_not_marked_as_containing_a_scope_reference<_Ty>();
			valid_if_Ty_is_not_marked_as_containing_an_accessible_scope_address_of_operator<_Ty>();
			base_class::fill(_Value);
		}

		void swap(_Myt& _Other) {	// swap contents with _Other
			valid_if_Ty_is_not_marked_as_containing_a_scope_reference<_Ty>();
			valid_if_Ty_is_not_marked_as_containing_an_accessible_scope_address_of_operator<_Ty>();
			base_class::swap(_Other);
		}

		void swap(_MA& _Other) {	// swap contents with _Other
			valid_if_Ty_is_not_marked_as_containing_a_scope_reference<_Ty>();
			valid_if_Ty_is_not_marked_as_containing_an_accessible_scope_address_of_operator<_Ty>();
			base_class::swap(_Other);
		}

		xscope_nii_array& operator=(const xscope_nii_array& _Right_cref) {
			valid_if_Ty_is_not_marked_as_containing_a_scope_reference<_Ty>();
			valid_if_Ty_is_not_marked_as_containing_an_accessible_scope_address_of_operator<_Ty>();
			base_class::operator=(_Right_cref);
			return (*this);
		}

		//class na_const_iterator_base : public std::iterator<std::random_access_iterator_tag, value_type, difference_type, const_pointer, const_reference> {};
		//class na_iterator_base : public std::iterator<std::random_access_iterator_tag, value_type, difference_type, pointer, reference> {};
		typedef mse::impl::random_access_const_iterator_base<_Ty> na_const_iterator_base;
		typedef mse::impl::random_access_iterator_base<_Ty> na_iterator_base;

		typedef TXScopeCSSSXSRAConstIterator<_Myt> xscope_const_iterator;
		typedef TXScopeCSSSXSRAIterator<_Myt> xscope_iterator;

		template<typename _TArrayPointer>
		static auto ss_begin(const _TArrayPointer& owner_ptr) {
			return base_class::template ss_begin(owner_ptr);
		}
		template<typename _TArrayPointer>
		static auto ss_end(const _TArrayPointer& owner_ptr) {
			return base_class::template ss_end(owner_ptr);
		}
		template<typename _TArrayPointer>
		static auto ss_cbegin(const _TArrayPointer& owner_ptr) {
			return base_class::template ss_cbegin(owner_ptr);
		}
		template<typename _TArrayPointer>
		static auto ss_cend(const _TArrayPointer& owner_ptr) {
			return base_class::template ss_cend(owner_ptr);
		}
		template<typename _TArrayPointer>
		static auto ss_rbegin(const _TArrayPointer& owner_ptr) {
			return base_class::template ss_rbegin(owner_ptr);
		}
		template<typename _TArrayPointer>
		static auto ss_rend(const _TArrayPointer& owner_ptr) {
			return base_class::template ss_rend(owner_ptr);
		}
		template<typename _TArrayPointer>
		static auto ss_crbegin(const _TArrayPointer& owner_ptr) {
			return base_class::template ss_crbegin(owner_ptr);
		}
		template<typename _TArrayPointer>
		static auto ss_crend(const _TArrayPointer& owner_ptr) {
			return base_class::template ss_crend(owner_ptr);
		}

		template<typename _TArrayPointer, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_TArrayPointer, mse::TXScopeFixedPointer<_Myt> >::value || std::is_convertible<_TArrayPointer, mse::TXScopeFixedConstPointer<_Myt> >::value> MSE_IMPL_EIS >
		static auto xscope_ss_begin(const _TArrayPointer& owner_ptr) {
			return base_class::template xscope_ss_begin(owner_ptr);
		}
		template<typename _TArrayPointer, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_TArrayPointer, mse::TXScopeFixedPointer<_Myt> >::value || std::is_convertible<_TArrayPointer, mse::TXScopeFixedConstPointer<_Myt> >::value> MSE_IMPL_EIS >
		static auto xscope_ss_end(const _TArrayPointer& owner_ptr) {
			return base_class::template xscope_ss_end(owner_ptr);
		}

		static auto xscope_ss_cbegin(const mse::TXScopeFixedConstPointer<_Myt>& owner_ptr) {
			xscope_const_iterator retval(owner_ptr);
			retval.set_to_beginning();
			return retval;
		}
		static auto xscope_ss_cend(const mse::TXScopeFixedConstPointer<_Myt>& owner_ptr) {
			xscope_const_iterator retval(owner_ptr);
			retval.set_to_end_marker();
			return retval;
		}

		static auto xscope_ss_rbegin(const mse::TXScopeFixedPointer<_Myt>& owner_ptr) {
			return xscope_ss_reverse_iterator_type(xscope_ss_end(owner_ptr));
		}
		static auto xscope_ss_rend(const mse::TXScopeFixedPointer<_Myt>& owner_ptr) {
			return xscope_ss_reverse_iterator_type(xscope_ss_begin(owner_ptr));
		}
		static auto xscope_ss_crbegin(const mse::TXScopeFixedConstPointer<_Myt>& owner_ptr) {
			return (xscope_ss_const_reverse_iterator_type(xscope_ss_cend(owner_ptr)));
		}
		static auto xscope_ss_crend(const mse::TXScopeFixedConstPointer<_Myt>& owner_ptr) {
			return (xscope_ss_const_reverse_iterator_type(xscope_ss_crbegin(owner_ptr)));
		}
		static auto xscope_ss_rbegin(const mse::TXScopeFixedConstPointer<_Myt>& owner_ptr) { return xscope_ss_crbegin(owner_ptr); }
		static auto xscope_ss_rend(const mse::TXScopeFixedConstPointer<_Myt>& owner_ptr) { return xscope_ss_crend(owner_ptr); }


		bool operator==(const _Myt& _Right) const {	// test for array equality
			return base_class::operator==(_Right);
		}
		bool operator<(const _Myt& _Right) const {	// test if _Left < _Right for arrays
			return base_class::operator<(_Right);
		}

		MSE_INHERIT_XSCOPE_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(_Ty);

	private:
		/* If _Ty is "marked" as containing a scope reference, then the following member function
		will not instantiate, causing an (intended) compile error. */
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _Ty>::value)
			&& (mse::impl::potentially_does_not_contain_non_owning_scope_reference<_Ty2>::value)> MSE_IMPL_EIS >
		void valid_if_Ty_is_not_marked_as_containing_a_scope_reference() const {}

		/* If _Ty is "marked" as containing an accessible "scope address of" operator, then the following member function
		will not instantiate, causing an (intended) compile error. */
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _Ty>::value)
			&& (mse::impl::is_potentially_not_referenceable_by_scope_pointer<_Ty2>::value)
			> MSE_IMPL_EIS >
		void valid_if_Ty_is_not_marked_as_containing_an_accessible_scope_address_of_operator() const {}

		//std_array m_array;
		//state_mutex_t m_mutex1;

		template<size_t _Idx, class _Tz, size_t _Size2>
		friend _CONST_FUN _Tz& std::get(mse::xscope_nii_array<_Tz, _Size2>& _Arr) _NOEXCEPT;
		template<size_t _Idx, class _Tz, size_t _Size2>
		friend _CONST_FUN const _Tz& std::get(const mse::xscope_nii_array<_Tz, _Size2>& _Arr) _NOEXCEPT;
		template<size_t _Idx, class _Tz, size_t _Size2>
		friend _CONST_FUN _Tz&& std::get(mse::xscope_nii_array<_Tz, _Size2>&& _Arr) _NOEXCEPT;

		friend void swap(_Myt& a, _Myt& b) { a.swap(b); }
		friend void swap(_Myt& a, _MA& b) { a.swap(b); }
		friend void swap(_MA& a, _Myt& b) { b.swap(a); }
	};

#ifdef MSE_HAS_CXX17
	/* deduction guides */

	template<class _First,
		class... _Rest>
		xscope_nii_array(_First, _Rest...)
		->xscope_nii_array<typename impl::_mse_Enforce_same<_First, _Rest...>::type, 1 + sizeof...(_Rest)>;
#endif /* MSE_HAS_CXX17 */

	template<class _Ty, size_t _Size> inline bool operator!=(const xscope_nii_array<_Ty, _Size>& _Left,
		const xscope_nii_array<_Ty, _Size>& _Right) {	// test for array inequality
		return (!(_Left == _Right));
	}

	template<class _Ty, size_t _Size> inline bool operator>(const xscope_nii_array<_Ty, _Size>& _Left,
		const xscope_nii_array<_Ty, _Size>& _Right) {	// test if _Left > _Right for arrays
		return (_Right < _Left);
	}

	template<class _Ty, size_t _Size> inline bool operator<=(const xscope_nii_array<_Ty, _Size>& _Left,
		const xscope_nii_array<_Ty, _Size>& _Right) {	// test if _Left <= _Right for arrays
		return (!(_Right < _Left));
	}

	template<class _Ty, size_t _Size> inline bool operator>=(const xscope_nii_array<_Ty, _Size>& _Left,
		const xscope_nii_array<_Ty, _Size>& _Right) {	// test if _Left >= _Right for arrays
		return (!(_Left < _Right));
	}

	template<class _Ty, size_t _Size>
	TXScopeFixedPointer<_Ty> xscope_pointer_to_array_element(const typename xscope_nii_array<_Ty, _Size>::xscope_ss_iterator_type& iter_cref) {
		return mse::us::unsafe_make_xscope_pointer_to(*iter_cref);
	}
	template<class _Ty, size_t _Size>
	TXScopeFixedPointer<_Ty> xscope_pointer_to_array_element(const mse::TXScopeFixedPointer<xscope_nii_array<_Ty, _Size> >& ptr, typename xscope_nii_array<_Ty, _Size>::size_type _P) {
		return mse::us::unsafe_make_xscope_pointer_to((*ptr)[_P]);
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, size_t _Size>
	TXScopeFixedPointer<_Ty> xscope_pointer_to_array_element(const mse::TXScopeObjFixedPointer<xscope_nii_array<_Ty, _Size> >& ptr, typename xscope_nii_array<_Ty, _Size>::size_type _P) {
		return mse::us::unsafe_make_xscope_pointer_to((*ptr)[_P]);
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	template<class _Ty, size_t _Size>
	TXScopeFixedConstPointer<_Ty> xscope_const_pointer_to_array_element(const typename xscope_nii_array<_Ty, _Size>::xscope_ss_const_iterator_type& iter_cref) {
		return mse::us::unsafe_make_xscope_const_pointer_to(*iter_cref);
	}
	template<class _Ty, size_t _Size>
	TXScopeFixedConstPointer<_Ty> xscope_const_pointer_to_array_element(const mse::TXScopeFixedConstPointer<xscope_nii_array<_Ty, _Size> >& ptr, typename xscope_nii_array<_Ty, _Size>::size_type _P) {
		return mse::us::unsafe_make_xscope_const_pointer_to((*ptr)[_P]);
	}
	template<class _Ty, size_t _Size>
	TXScopeFixedConstPointer<_Ty> xscope_const_pointer_to_array_element(const mse::TXScopeFixedPointer<xscope_nii_array<_Ty, _Size> >& ptr, typename xscope_nii_array<_Ty, _Size>::size_type _P) {
		return mse::us::unsafe_make_xscope_const_pointer_to((*ptr)[_P]);
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, size_t _Size>
	TXScopeFixedConstPointer<_Ty> xscope_const_pointer_to_array_element(const mse::TXScopeObjFixedConstPointer<xscope_nii_array<_Ty, _Size> >& ptr, typename xscope_nii_array<_Ty, _Size>::size_type _P) {
		return mse::us::unsafe_make_xscope_const_pointer_to((*ptr)[_P]);
	}
	template<class _Ty, size_t _Size>
	TXScopeFixedConstPointer<_Ty> xscope_const_pointer_to_array_element(const mse::TXScopeObjFixedPointer<xscope_nii_array<_Ty, _Size> >& ptr, typename xscope_nii_array<_Ty, _Size>::size_type _P) {
		return mse::us::unsafe_make_xscope_const_pointer_to((*ptr)[_P]);
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	namespace impl {

		/* Some algorithm implementation specializations for xscope_nii_array<>.  */

		template<class _Ty, size_t _Size>
		auto make_xscope_specialized_range_iter_provider_overloaded(mse::xscope_nii_array< _Ty, _Size>& array_ref) {
			return TXScopeRangeRawPointerIterProvider<mse::impl::remove_reference_t<decltype(array_ref)> >(array_ref);
		}
		template<class _Ty, size_t _Size>
		auto make_xscope_specialized_range_iter_provider_overloaded(const mse::xscope_nii_array< _Ty, _Size>& array_ref) {
			return TXScopeRangeRawPointerIterProvider<mse::impl::remove_reference_t<decltype(array_ref)> >(array_ref);
		}
	}
}

namespace std {

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmismatched-tags"
#endif /*__clang__*/

	template<class _Ty, size_t _Size>
	struct tuple_size<mse::xscope_nii_array<_Ty, _Size> >
		: integral_constant<size_t, _Size>
	{	// struct to determine number of elements in array
	};

	template<size_t _Idx, class _Ty, size_t _Size>
	struct tuple_element<_Idx, mse::xscope_nii_array<_Ty, _Size> >
	{	// struct to determine type of element _Idx in array
		static_assert(_Idx < _Size, "array index out of bounds");

		typedef _Ty type;
	};

#ifdef __clang__
#pragma clang diagnostic pop
#endif /*__clang__*/

	// TUPLE INTERFACE TO array
	template<size_t _Idx, class _Ty, size_t _Size>
	_CONST_FUN _Ty& get(mse::xscope_nii_array<_Ty, _Size>& _Arr) _NOEXCEPT
	{	// return element at _Idx in array _Arr
		static_assert(_Idx < _Size, "array index out of bounds");
		return (std::get<_Idx>(_Arr.contained_array()));
	}

	template<size_t _Idx, class _Ty, size_t _Size>
	_CONST_FUN const _Ty& get(const mse::xscope_nii_array<_Ty, _Size>& _Arr) _NOEXCEPT
	{	// return element at _Idx in array _Arr
		static_assert(_Idx < _Size, "array index out of bounds");
		return (std::get<_Idx>(_Arr.contained_array()));
	}

	template<size_t _Idx, class _Ty, size_t _Size>
	_CONST_FUN _Ty&& get(mse::xscope_nii_array<_Ty, _Size>&& _Arr) _NOEXCEPT
	{	// return element at _Idx in array _Arr
		static_assert(_Idx < _Size, "array index out of bounds");
		return (_STD move(std::get<_Idx>(_Arr.contained_array())));
	}

	template<class _Ty, size_t _Size, class _TStateMutex = mse::default_state_mutex/*, class = enable_if_t<_Size == 0 || _Is_swappable<_Ty>::value>*/>
	void swap(mse::xscope_nii_array<_Ty, _Size>& _Left, mse::xscope_nii_array<_Ty, _Size>& _Right) _NOEXCEPT
	{
		_Left.swap(_Right);
	}
	template<class _Ty, size_t _Size, class _TStateMutex = mse::default_state_mutex/*, class = enable_if_t<_Size == 0 || _Is_swappable<_Ty>::value>*/>
	void swap(array<_Ty, _Size>& _Left, mse::xscope_nii_array<_Ty, _Size>& _Right) _NOEXCEPT_OP(_NOEXCEPT_OP(_Right.swap(_Left)))
	{	// swap arrays
		return (_Right.swap(_Left));
	}
	template<class _Ty, size_t _Size, class _TStateMutex = mse::default_state_mutex/*, class = enable_if_t<_Size == 0 || _Is_swappable<_Ty>::value>*/>
	void swap(mse::xscope_nii_array<_Ty, _Size>& _Left, array<_Ty, _Size>& _Right) _NOEXCEPT
	{
		_Left.swap(_Right);
	}
}

namespace mse {

	namespace us {
		namespace impl {
			namespace ns_msearray {
				template<class _Ty, size_t _Size, class _TStateMutex>
				using Tmsearray_rp_ss_iterator_type = mse::impl::ns_nii_array::Tarray_ss_iterator_type<msear_pointer<mse::us::msearray<_Ty, _Size, _TStateMutex> >, _Ty, _Size, _TStateMutex>;
				template<class _Ty, size_t _Size, class _TStateMutex>
				using Tmsearray_rp_ss_const_iterator_type = mse::impl::ns_nii_array::Tarray_ss_const_iterator_type<msear_pointer<const mse::us::msearray<_Ty, _Size, _TStateMutex> >, _Ty, _Size, _TStateMutex>;
				template<class _Ty, size_t _Size, class _TStateMutex>
				using Tmsearray_rp_ss_reverse_iterator_type = mse::impl::ns_nii_array::Tarray_ss_iterator_type<msear_pointer<mse::us::msearray<_Ty, _Size, _TStateMutex> >, _Ty, _Size, _TStateMutex>;
				template<class _Ty, size_t _Size, class _TStateMutex>
				using Tmsearray_rp_ss_const_reverse_iterator_type = mse::impl::ns_nii_array::Tarray_ss_const_reverse_iterator_type<msear_pointer<const mse::us::msearray<_Ty, _Size, _TStateMutex> >, _Ty, _Size, _TStateMutex>;
			}
		}

		/* msearray<> is an unsafe array that provides the traditional begin() and end() (non-static)
		member functions that return unsafe iterators. It also provides ss_begin() and ss_end() (non-static) member
		functions which return bounds-checked, but still technically unsafe iterators. */
		template<class _Ty, size_t _Size, class _TStateMutex/* = container_adjusted_default_state_mutex<_Ty> */>
		class msearray : public mse::us::impl::nii_array_base<_Ty, _Size, _TStateMutex>, public mse::us::impl::AsyncNotShareableTagBase {
		public:
			typedef mse::us::impl::nii_array_base<_Ty, _Size, _TStateMutex> base_class;
			typedef std::array<_Ty, _Size> std_array;
			typedef msearray _Myt;

			MSE_USING(msearray, base_class);
			msearray(_XSTD initializer_list<typename base_class::_MA::value_type> _Ilist) : base_class(_Ilist) {}

			MSE_INHERITED_RANDOM_ACCESS_MEMBER_TYPE_DECLARATIONS(base_class);

			typedef typename base_class::_MA::iterator iterator;
			typedef typename base_class::_MA::const_iterator const_iterator;
			typedef typename base_class::_MA::reverse_iterator reverse_iterator;
			typedef typename base_class::_MA::const_reverse_iterator const_reverse_iterator;

			iterator begin() _NOEXCEPT {	// return iterator for beginning of mutable sequence
				return base_class::contained_array().begin();
			}
			const_iterator begin() const _NOEXCEPT {	// return iterator for beginning of nonmutable sequence
				return base_class::contained_array().begin();
			}
			iterator end() _NOEXCEPT {	// return iterator for end of mutable sequence
				return base_class::contained_array().end();
			}
			const_iterator end() const _NOEXCEPT {	// return iterator for beginning of nonmutable sequence
				return base_class::contained_array().end();
			}
			reverse_iterator rbegin() _NOEXCEPT {	// return iterator for beginning of reversed mutable sequence
				return base_class::contained_array().rbegin();
			}
			const_reverse_iterator rbegin() const _NOEXCEPT {	// return iterator for beginning of reversed nonmutable sequence
				return base_class::contained_array().rbegin();
			}
			reverse_iterator rend() _NOEXCEPT {	// return iterator for end of reversed mutable sequence
				return base_class::contained_array().rend();
			}
			const_reverse_iterator rend() const _NOEXCEPT {	// return iterator for end of reversed nonmutable sequence
				return base_class::contained_array().rend();
			}
			const_iterator cbegin() const _NOEXCEPT {	// return iterator for beginning of nonmutable sequence
				return base_class::contained_array().cbegin();
			}
			const_iterator cend() const _NOEXCEPT {	// return iterator for end of nonmutable sequence
				return base_class::contained_array().cend();
			}
			const_reverse_iterator crbegin() const _NOEXCEPT {	// return iterator for beginning of reversed nonmutable sequence
				return base_class::contained_array().crbegin();
			}
			const_reverse_iterator crend() const _NOEXCEPT {	// return iterator for end of reversed nonmutable sequence
				return base_class::contained_array().crend();
			}

			typedef impl::ns_msearray::Tmsearray_rp_ss_iterator_type<_Ty, _Size, _TStateMutex> ss_iterator_type;
			typedef impl::ns_msearray::Tmsearray_rp_ss_const_iterator_type<_Ty, _Size, _TStateMutex> ss_const_iterator_type;
			typedef impl::ns_msearray::Tmsearray_rp_ss_reverse_iterator_type<_Ty, _Size, _TStateMutex> ss_reverse_iterator_type;
			typedef impl::ns_msearray::Tmsearray_rp_ss_const_reverse_iterator_type<_Ty, _Size, _TStateMutex> ss_const_reverse_iterator_type;

			ss_iterator_type ss_begin() {	// return std_array::iterator for beginning of mutable sequence
				ss_iterator_type retval(this);
				retval.set_to_beginning();
				return retval;
			}
			ss_const_iterator_type ss_begin() const {	// return std_array::iterator for beginning of nonmutable sequence
				ss_const_iterator_type retval(this);
				retval.set_to_beginning();
				return retval;
			}
			ss_iterator_type ss_end() {	// return std_array::iterator for end of mutable sequence
				ss_iterator_type retval(this);
				retval.set_to_end_marker();
				return retval;
			}
			ss_const_iterator_type ss_end() const {	// return std_array::iterator for end of nonmutable sequence
				ss_const_iterator_type retval(this);
				retval.set_to_end_marker();
				return retval;
			}
			ss_const_iterator_type ss_cbegin() const {	// return std_array::iterator for beginning of nonmutable sequence
				ss_const_iterator_type retval(this);
				retval.set_to_beginning();
				return retval;
			}
			ss_const_iterator_type ss_cend() const {	// return std_array::iterator for end of nonmutable sequence
				ss_const_iterator_type retval(this);
				retval.set_to_end_marker();
				return retval;
			}

			ss_const_reverse_iterator_type ss_crbegin() const {	// return std_array::iterator for beginning of reversed nonmutable sequence
				return (ss_rbegin());
			}
			ss_const_reverse_iterator_type ss_crend() const {	// return std_array::iterator for end of reversed nonmutable sequence
				return (ss_rend());
			}
			ss_reverse_iterator_type ss_rbegin() {	// return std_array::iterator for beginning of reversed mutable sequence
				return (reverse_iterator(ss_end()));
			}
			ss_const_reverse_iterator_type ss_rbegin() const {	// return std_array::iterator for beginning of reversed nonmutable sequence
				return (const_reverse_iterator(ss_end()));
			}
			ss_reverse_iterator_type ss_rend() {	// return std_array::iterator for end of reversed mutable sequence
				return (reverse_iterator(ss_begin()));
			}
			ss_const_reverse_iterator_type ss_rend() const {	// return std_array::iterator for end of reversed nonmutable sequence
				return (const_reverse_iterator(ss_begin()));
			}

			template<typename _TMseArrayPointer> using Tss_iterator_type = typename base_class::template Tss_iterator_type<_TMseArrayPointer>;
			template<typename _TMseArrayPointer> using Tss_const_iterator_type = typename base_class::template Tss_const_iterator_type<_TMseArrayPointer>;
			template<typename _TMseArrayPointer> using Tss_reverse_iterator_type = typename base_class::template Tss_reverse_iterator_type<_TMseArrayPointer>;
			template<typename _TMseArrayPointer> using Tss_const_reverse_iterator_type = typename base_class::template Tss_const_reverse_iterator_type<_TMseArrayPointer>;

			//template <class X> using ss_begin = typename base_class::template ss_begin<X>;
			template<typename _TMseArrayPointer>
			static Tss_iterator_type<_TMseArrayPointer> ss_begin(const _TMseArrayPointer& owner_ptr) {	// return iterator for beginning of mutable sequence
				return base_class::template ss_begin<_TMseArrayPointer>(owner_ptr);
			}
			template<typename _TMseArrayPointer>
			static Tss_iterator_type<_TMseArrayPointer> ss_end(_TMseArrayPointer owner_ptr) {	// return iterator for end of mutable sequence
				return base_class::template ss_end<_TMseArrayPointer>(owner_ptr);
			}
			template<typename _TMseArrayPointer>
			static Tss_const_iterator_type<_TMseArrayPointer> ss_cbegin(_TMseArrayPointer owner_ptr) {	// return iterator for beginning of nonmutable sequence
				return base_class::template ss_cbegin<_TMseArrayPointer>(owner_ptr);
			}
			template<typename _TMseArrayPointer>
			static Tss_const_iterator_type<_TMseArrayPointer> ss_cend(_TMseArrayPointer owner_ptr) {	// return iterator for end of nonmutable sequence
				return base_class::template ss_cend<_TMseArrayPointer>(owner_ptr);
			}
			template<typename _TMseArrayPointer>
			static Tss_reverse_iterator_type<_TMseArrayPointer> ss_rbegin(_TMseArrayPointer owner_ptr) {	// return iterator for beginning of reversed mutable sequence
				return (Tss_reverse_iterator_type<_TMseArrayPointer>(ss_end<_TMseArrayPointer>(owner_ptr)));
			}
			template<typename _TMseArrayPointer>
			static Tss_reverse_iterator_type<_TMseArrayPointer> ss_rend(_TMseArrayPointer owner_ptr) {	// return iterator for end of reversed mutable sequence
				return (Tss_reverse_iterator_type<_TMseArrayPointer>(ss_cbegin<_TMseArrayPointer>(owner_ptr)));
			}
			template<typename _TMseArrayPointer>
			static Tss_const_reverse_iterator_type<_TMseArrayPointer> ss_crbegin(_TMseArrayPointer owner_ptr) {	// return iterator for beginning of reversed nonmutable sequence
				return (Tss_const_reverse_iterator_type<_TMseArrayPointer>(ss_end<_TMseArrayPointer>(owner_ptr)));
			}

			//typedef typename base_class::xscope_ss_const_iterator_type xscope_ss_const_iterator_type;
			//typedef typename base_class::xscope_ss_iterator_type xscope_ss_iterator_type;

			typedef typename base_class::xscope_const_iterator xscope_const_iterator;
			typedef typename base_class::xscope_iterator xscope_iterator;

			void async_not_shareable_and_not_passable_tag() const {}

		private:

			auto contained_array() const -> decltype(base_class::contained_array()) { return base_class::contained_array(); }
			auto contained_array() -> decltype(base_class::contained_array()) { return base_class::contained_array(); }

			template<size_t _Idx, class _Tz, size_t _Size2, class _TStateMutex2>
			friend _CONST_FUN _Tz& std::get(mse::us::msearray<_Tz, _Size2, _TStateMutex2>& _Arr) _NOEXCEPT;
			template<size_t _Idx, class _Tz, size_t _Size2, class _TStateMutex2>
			friend _CONST_FUN const _Tz& std::get(const mse::us::msearray<_Tz, _Size2, _TStateMutex2>& _Arr) _NOEXCEPT;
			template<size_t _Idx, class _Tz, size_t _Size2, class _TStateMutex2>
			friend _CONST_FUN _Tz&& std::get(mse::us::msearray<_Tz, _Size2, _TStateMutex2>&& _Arr) _NOEXCEPT;
		};

#ifdef MSE_HAS_CXX17
		/* deduction guides */
		template<class _First,
			class... _Rest>
			msearray(_First, _Rest...)
			->msearray<typename mse::impl::_mse_Enforce_same<_First, _Rest...>::type, 1 + sizeof...(_Rest)>;
#endif /* MSE_HAS_CXX17 */

		template<class _Ty, size_t _Size, class _TStateMutex> inline bool operator!=(const msearray<_Ty, _Size, _TStateMutex>& _Left,
			const msearray<_Ty, _Size, _TStateMutex>& _Right) {	// test for array inequality
			return (!(_Left == _Right));
		}

		template<class _Ty, size_t _Size, class _TStateMutex> inline bool operator>(const msearray<_Ty, _Size, _TStateMutex>& _Left,
			const msearray<_Ty, _Size, _TStateMutex>& _Right) {	// test if _Left > _Right for arrays
			return (_Right < _Left);
		}

		template<class _Ty, size_t _Size, class _TStateMutex> inline bool operator<=(const msearray<_Ty, _Size, _TStateMutex>& _Left,
			const msearray<_Ty, _Size, _TStateMutex>& _Right) {	// test if _Left <= _Right for arrays
			return (!(_Right < _Left));
		}

		template<class _Ty, size_t _Size, class _TStateMutex> inline bool operator>=(const msearray<_Ty, _Size, _TStateMutex>& _Left,
			const msearray<_Ty, _Size, _TStateMutex>& _Right) {	// test if _Left >= _Right for arrays
			return (!(_Left < _Right));
		}

		template<class _Ty, size_t _Size, class _TStateMutex>
		TXScopeFixedPointer<_Ty> xscope_pointer_to_array_element(const typename msearray<_Ty, _Size, _TStateMutex>::xscope_ss_iterator_type& iter_cref) {
			return mse::us::unsafe_make_xscope_pointer_to(*iter_cref);
		}
		template<class _Ty, size_t _Size, class _TStateMutex>
		TXScopeFixedPointer<_Ty> xscope_pointer_to_array_element(const mse::TXScopeFixedPointer<msearray<_Ty, _Size, _TStateMutex> >& ptr, typename msearray<_Ty, _Size, _TStateMutex>::size_type _P) {
			return mse::us::unsafe_make_xscope_pointer_to((*ptr)[_P]);
		}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _Ty, size_t _Size, class _TStateMutex>
		TXScopeFixedPointer<_Ty> xscope_pointer_to_array_element(const mse::TXScopeObjFixedPointer<msearray<_Ty, _Size, _TStateMutex> >& ptr, typename msearray<_Ty, _Size, _TStateMutex>::size_type _P) {
			return mse::us::unsafe_make_xscope_pointer_to((*ptr)[_P]);
		}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

		template<class _Ty, size_t _Size, class _TStateMutex>
		TXScopeFixedConstPointer<_Ty> xscope_const_pointer_to_array_element(const typename msearray<_Ty, _Size, _TStateMutex>::xscope_ss_const_iterator_type& iter_cref) {
			return mse::us::unsafe_make_xscope_const_pointer_to(*iter_cref);
		}
		template<class _Ty, size_t _Size, class _TStateMutex>
		TXScopeFixedConstPointer<_Ty> xscope_const_pointer_to_array_element(const mse::TXScopeFixedConstPointer<msearray<_Ty, _Size, _TStateMutex> >& ptr, typename msearray<_Ty, _Size, _TStateMutex>::size_type _P) {
			return mse::us::unsafe_make_xscope_const_pointer_to((*ptr)[_P]);
		}
		template<class _Ty, size_t _Size, class _TStateMutex>
		TXScopeFixedConstPointer<_Ty> xscope_const_pointer_to_array_element(const mse::TXScopeFixedPointer<msearray<_Ty, _Size, _TStateMutex> >& ptr, typename msearray<_Ty, _Size, _TStateMutex>::size_type _P) {
			return mse::us::unsafe_make_xscope_const_pointer_to((*ptr)[_P]);
		}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _Ty, size_t _Size, class _TStateMutex>
		TXScopeFixedConstPointer<_Ty> xscope_const_pointer_to_array_element(const mse::TXScopeObjFixedConstPointer<msearray<_Ty, _Size, _TStateMutex> >& ptr, typename msearray<_Ty, _Size, _TStateMutex>::size_type _P) {
			return mse::us::unsafe_make_xscope_const_pointer_to((*ptr)[_P]);
		}
		template<class _Ty, size_t _Size, class _TStateMutex>
		TXScopeFixedConstPointer<_Ty> xscope_const_pointer_to_array_element(const mse::TXScopeObjFixedPointer<msearray<_Ty, _Size, _TStateMutex> >& ptr, typename msearray<_Ty, _Size, _TStateMutex>::size_type _P) {
			return mse::us::unsafe_make_xscope_const_pointer_to((*ptr)[_P]);
		}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
	}

	/* Using the mse::msearray<> alias of mse::us::msearray<> is deprecated. */
	template<class _Ty, size_t _Size, class _TStateMutex>
	using msearray MSE_DEPRECATED = us::msearray< _Ty, _Size, _TStateMutex>;


	/*
	template<class _Ty, size_t _Size, class _TStateMutex>
	TXScopeFixedPointer<_Ty> xscope_pointer_to_array_element(const typename us::msearray<_Ty, _Size, _TStateMutex>::xscope_ss_iterator_type& iter_cref) {
		return mse::us::unsafe_make_xscope_pointer_to(*iter_cref);
	}
	*/
	template<class _Ty, size_t _Size, class _TStateMutex>
	TXScopeFixedPointer<_Ty> xscope_pointer_to_array_element(const mse::TXScopeFixedPointer<us::msearray<_Ty, _Size, _TStateMutex> >& ptr, typename us::msearray<_Ty, _Size, _TStateMutex>::size_type _P) {
		return mse::us::unsafe_make_xscope_pointer_to((*ptr)[_P]);
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, size_t _Size, class _TStateMutex>
	TXScopeFixedPointer<_Ty> xscope_pointer_to_array_element(const mse::TXScopeObjFixedPointer<us::msearray<_Ty, _Size, _TStateMutex> >& ptr, typename us::msearray<_Ty, _Size, _TStateMutex>::size_type _P) {
		return mse::us::unsafe_make_xscope_pointer_to((*ptr)[_P]);
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	/*
	template<class _Ty, size_t _Size, class _TStateMutex>
	TXScopeFixedConstPointer<_Ty> xscope_const_pointer_to_array_element(const typename us::msearray<_Ty, _Size, _TStateMutex>::xscope_ss_const_iterator_type& iter_cref) {
		return mse::us::unsafe_make_xscope_const_pointer_to(*iter_cref);
	}
	*/
	template<class _Ty, size_t _Size, class _TStateMutex>
	TXScopeFixedConstPointer<_Ty> xscope_const_pointer_to_array_element(const mse::TXScopeFixedConstPointer<us::msearray<_Ty, _Size, _TStateMutex> >& ptr, typename us::msearray<_Ty, _Size, _TStateMutex>::size_type _P) {
		return mse::us::unsafe_make_xscope_const_pointer_to((*ptr)[_P]);
	}
	template<class _Ty, size_t _Size, class _TStateMutex>
	TXScopeFixedConstPointer<_Ty> xscope_const_pointer_to_array_element(const mse::TXScopeFixedPointer<us::msearray<_Ty, _Size, _TStateMutex> >& ptr, typename us::msearray<_Ty, _Size, _TStateMutex>::size_type _P) {
		return mse::us::unsafe_make_xscope_const_pointer_to((*ptr)[_P]);
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, size_t _Size, class _TStateMutex>
	TXScopeFixedConstPointer<_Ty> xscope_const_pointer_to_array_element(const mse::TXScopeObjFixedConstPointer<us::msearray<_Ty, _Size, _TStateMutex> >& ptr, typename us::msearray<_Ty, _Size, _TStateMutex>::size_type _P) {
		return mse::us::unsafe_make_xscope_const_pointer_to((*ptr)[_P]);
	}
	template<class _Ty, size_t _Size, class _TStateMutex>
	TXScopeFixedConstPointer<_Ty> xscope_const_pointer_to_array_element(const mse::TXScopeObjFixedPointer<us::msearray<_Ty, _Size, _TStateMutex> >& ptr, typename us::msearray<_Ty, _Size, _TStateMutex>::size_type _P) {
		return mse::us::unsafe_make_xscope_const_pointer_to((*ptr)[_P]);
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)


	/* deprecated */
	template<class _TArray>
	MSE_DEPRECATED typename _TArray::xscope_ss_const_iterator_type make_xscope_ss_const_iterator_type(const mse::TXScopeObjFixedConstPointer<_TArray>& owner_ptr) {
		return typename _TArray::xscope_ss_const_iterator_type(owner_ptr);
	}
	template<class _TArray>
	MSE_DEPRECATED typename _TArray::xscope_ss_const_iterator_type make_xscope_ss_const_iterator_type(const mse::TXScopeObjFixedPointer<_TArray>& owner_ptr) {
		return typename _TArray::xscope_ss_const_iterator_type(owner_ptr);
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _TArray>
	MSE_DEPRECATED typename _TArray::xscope_ss_const_iterator_type make_xscope_ss_const_iterator_type(const mse::TXScopeFixedConstPointer<_TArray>& owner_ptr) {
		return typename _TArray::xscope_ss_const_iterator_type(owner_ptr);
	}
	template<class _TArray>
	MSE_DEPRECATED typename _TArray::xscope_ss_const_iterator_type make_xscope_ss_const_iterator_type(const mse::TXScopeFixedPointer<_TArray>& owner_ptr) {
		return typename _TArray::xscope_ss_const_iterator_type(owner_ptr);
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	template<class _TArray>
	MSE_DEPRECATED typename _TArray::xscope_ss_iterator_type make_xscope_ss_iterator_type(const mse::TXScopeObjFixedPointer<_TArray>& owner_ptr) {
		return typename _TArray::xscope_ss_iterator_type(owner_ptr);
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _TArray>
	MSE_DEPRECATED typename _TArray::xscope_ss_iterator_type make_xscope_ss_iterator_type(const mse::TXScopeFixedPointer<_TArray>& owner_ptr) {
		return typename _TArray::xscope_ss_iterator_type(owner_ptr);
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

}

namespace std {

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmismatched-tags"
#endif /*__clang__*/

	template<class _Ty, size_t _Size>
	struct tuple_size<mse::us::msearray<_Ty, _Size> >
		: integral_constant<size_t, _Size>
	{	// struct to determine number of elements in array
	};

	template<size_t _Idx, class _Ty, size_t _Size>
	struct tuple_element<_Idx, mse::us::msearray<_Ty, _Size> >
	{	// struct to determine type of element _Idx in array
		static_assert(_Idx < _Size, "array index out of bounds");

		typedef _Ty type;
	};

#ifdef __clang__
#pragma clang diagnostic pop
#endif /*__clang__*/

	// TUPLE INTERFACE TO array
	template<size_t _Idx, class _Ty, size_t _Size, class _TStateMutex>
	_CONST_FUN _Ty& get(mse::us::msearray<_Ty, _Size, _TStateMutex>& _Arr) _NOEXCEPT
	{	// return element at _Idx in array _Arr
		static_assert(_Idx < _Size, "array index out of bounds");
		return (std::get<_Idx>(_Arr.contained_array()));
	}

	template<size_t _Idx, class _Ty, size_t _Size, class _TStateMutex>
	_CONST_FUN const _Ty& get(const mse::us::msearray<_Ty, _Size, _TStateMutex>& _Arr) _NOEXCEPT
	{	// return element at _Idx in array _Arr
		static_assert(_Idx < _Size, "array index out of bounds");
		return (std::get<_Idx>(_Arr.contained_array()));
	}

	template<size_t _Idx, class _Ty, size_t _Size, class _TStateMutex>
	_CONST_FUN _Ty&& get(mse::us::msearray<_Ty, _Size, _TStateMutex>&& _Arr) _NOEXCEPT
	{	// return element at _Idx in array _Arr
		static_assert(_Idx < _Size, "array index out of bounds");
		return (_STD move(std::get<_Idx>(_Arr.contained_array())));
	}

	template<class _Ty, size_t _Size, class _TStateMutex = mse::default_state_mutex/*, class = enable_if_t<_Size == 0 || _Is_swappable<_Ty>::value>*/>
	void swap(mse::us::msearray<_Ty, _Size, _TStateMutex>& _Left, mse::us::msearray<_Ty, _Size, _TStateMutex>& _Right) _NOEXCEPT
	{
		_Left.swap(_Right);
	}
	template<class _Ty, size_t _Size, class _TStateMutex = mse::default_state_mutex/*, class = enable_if_t<_Size == 0 || _Is_swappable<_Ty>::value>*/>
	void swap(array<_Ty, _Size>& _Left, mse::us::msearray<_Ty, _Size, _TStateMutex>& _Right) _NOEXCEPT_OP(_NOEXCEPT_OP(_Right.swap(_Left)))
	{	// swap arrays
		return (_Right.swap(_Left));
	}
	template<class _Ty, size_t _Size, class _TStateMutex = mse::default_state_mutex/*, class = enable_if_t<_Size == 0 || _Is_swappable<_Ty>::value>*/>
	void swap(mse::us::msearray<_Ty, _Size, _TStateMutex>& _Left, array<_Ty, _Size>& _Right) _NOEXCEPT
	{
		_Left.swap(_Right);
	}
}

namespace mse {

	namespace impl {
		template<class T, class EqualTo>
		struct SupportsStdBegin_msemsearray_impl
		{
			template<class U, class V>
			static auto test(U* u) -> decltype(std::begin(*u), std::declval<V>(), bool(true));
			template<typename, typename>
			static auto test(...)->std::false_type;

			using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
		};
		template<class T, class EqualTo = T>
		struct SupportsStdBegin_msemsearray : SupportsStdBegin_msemsearray_impl<
			mse::impl::remove_reference_t<T>, mse::impl::remove_reference_t<EqualTo> >::type {};

		template<class T, class EqualTo>
		struct HasOrInheritsSizeMethod_msemsearray_impl
		{
			template<class U, class V>
			static auto test(U*) -> decltype(std::declval<U>().size(), std::declval<V>().size(), bool(true));
			template<typename, typename>
			static auto test(...)->std::false_type;

			using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
		};
		template<class T, class EqualTo = T>
		struct HasOrInheritsSizeMethod_msemsearray : HasOrInheritsSizeMethod_msemsearray_impl<
			mse::impl::remove_reference_t<T>, mse::impl::remove_reference_t<EqualTo> >::type {};

		template<class T, class EqualTo>
		struct HasOrInheritsStaticSSBeginMethod_msemsearray_impl
		{
			template<class U, class V>
			static auto test(U* u) -> decltype(U::ss_begin(std::declval<mse::us::impl::TPointer<U> >()), std::declval<V*>(), bool(true));
			template<typename, typename>
			static auto test(...)->std::false_type;

			using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
		};
		template<class T, class EqualTo = T>
		struct HasOrInheritsStaticSSBeginMethod_msemsearray : HasOrInheritsStaticSSBeginMethod_msemsearray_impl<
			mse::impl::remove_reference_t<T>, mse::impl::remove_reference_t<EqualTo> >::type {};

		template<class T, class EqualTo>
		struct HasOrInheritsStaticXScopeSSBeginMethod_msemsearray_impl
		{
			template<class U, class V>
			static auto test(U* u) -> decltype(U::xscope_ss_begin(std::declval<mse::TXScopeFixedPointer<U> >()), std::declval<V>(), bool(true));
			template<typename, typename>
			static auto test(...)->std::false_type;

			using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
		};
		template<class T, class EqualTo = T>
		struct HasOrInheritsStaticXScopeSSBeginMethod_msemsearray : HasOrInheritsStaticXScopeSSBeginMethod_msemsearray_impl<
			mse::impl::remove_reference_t<T>, mse::impl::remove_reference_t<EqualTo> >::type {};

		template<class T, class EqualTo>
		struct HasOrInheritsXScopeIteratorMemberType_msemsearray_impl
		{
			template<class U, class V>
			static auto test(U*) -> decltype(std::declval<typename U::xscope_const_iterator>(), std::declval<typename V::xscope_const_iterator>(), bool(true));
			template<typename, typename>
			static auto test(...)->std::false_type;

			using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
		};
		template<class T, class EqualTo = T>
		struct HasOrInheritsXScopeIteratorMemberType_msemsearray : HasOrInheritsXScopeIteratorMemberType_msemsearray_impl<
			mse::impl::remove_reference_t<T>, mse::impl::remove_reference_t<EqualTo> >::type {};
	}

	namespace impl {
		namespace iterator {

			template <typename _TXSRAPointer>
			auto begin_iter_from_xsptr_helper(std::true_type, const _TXSRAPointer& xsptr) {
				typedef mse::impl::remove_reference_t<decltype(*xsptr)> container_t;
				return typename container_t::xscope_iterator(xsptr, 0);
			}
			template <typename _TXSRAPointer>
			auto begin_iter_from_xsptr_helper(std::false_type, const _TXSRAPointer& xsptr) {
				return mse::TXScopeRAIterator<_TXSRAPointer>(xsptr, 0);
			}

			template <typename _TRAPointer>
			auto begin_iter_from_ptr_helper4(std::true_type, const _TRAPointer& ptr) {
				return mse::TRAIterator<_TRAPointer>(ptr, 0);
				//return mse::make_random_access_iterator(ptr, 0);
			}
			template <typename _TRAPointer>
			auto begin_iter_from_ptr_helper4(std::false_type, const _TRAPointer& ptr) {
				return std::begin(*ptr);
			}
			template <typename _TRAPointer>
			auto begin_iter_from_ptr_helper3(std::true_type, const _TRAPointer& ptr) {
				typedef mse::impl::remove_reference_t<decltype(*ptr)> container_t;
				return container_t::ss_begin(ptr);
			}
			template <typename _TRAPointer>
			auto begin_iter_from_ptr_helper3(std::false_type, const _TRAPointer& ptr) {
				typedef mse::impl::remove_reference_t<decltype(*ptr)> container_t;
				return begin_iter_from_ptr_helper4(typename mse::impl::is_random_access_container<container_t>::type(), ptr);
				//return mse::TRAIterator<_TRAPointer>(ptr, 0);
				//return mse::make_random_access_iterator(ptr, 0);
			}
			template <typename _TXSRAPointer>
			auto begin_iter_from_ptr_helper2(std::true_type, const _TXSRAPointer& xsptr) {
				/* xsptr seems to be an xscope pointer.*/
				typedef mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(*xsptr)> > container_t;
				return begin_iter_from_xsptr_helper(typename mse::impl::HasOrInheritsXScopeIteratorMemberType_msemsearray<container_t>::type(), xsptr);
			}
			template <typename _TRAPointer>
			auto begin_iter_from_ptr_helper2(std::false_type, const _TRAPointer& ptr) {
				typedef mse::impl::remove_reference_t<decltype(*ptr)> container_t;
				return begin_iter_from_ptr_helper3(typename mse::impl::HasOrInheritsStaticSSBeginMethod_msemsearray<container_t>::type(), ptr);
			}

			template <typename _TRALoneParam>
			auto begin_iter_from_lone_param2(std::false_type, const _TRALoneParam& ra_container) {
				/* The parameter doesn't seem to be a pointer. */
#ifdef MSE_ENABLE_LEGACY_MAKE_ITER_FROM_NONPOINTER
				/* So we'll use std::begin() to obtain an iterator. If you get a compile error here, then construction from the
				given parameter type isn't supported. */
				return std::begin(ra_container);
#else // MSE_ENABLE_LEGACY_MAKE_ITER_FROM_NONPOINTER

				/* The "make_iterator" functions expect for their argument a (safe) pointer to a container. To obtain an iterator
				directly from a container (that supports it), rather than a pointer to the conatiner, use std::begin() instead. */
				return std::begin(*ra_container); /* Intentional compile error. */

#endif // MSE_ENABLE_LEGACY_MAKE_ITER_FROM_NONPOINTER
			}
			template <typename _TRAPointer>
			auto begin_iter_from_lone_param2(std::true_type, const _TRAPointer& ptr) {
				return begin_iter_from_ptr_helper2(typename mse::impl::is_nonowning_scope_pointer<_TRAPointer>::type(), ptr);
			}
			template <typename _TRALoneParam>
			auto begin_iter_from_lone_param1(std::false_type, const _TRALoneParam&param) {
				return begin_iter_from_lone_param2(typename mse::impl::IsDereferenceable_msemsearray<_TRALoneParam>::type(), param);
			}
			template<class _TRALoneParam>
			auto begin_iter_from_lone_param(const _TRALoneParam&param) {
				typedef mse::impl::remove_reference_t<_TRALoneParam> _TRALoneParamRR;
				return mse::impl::iterator::begin_iter_from_lone_param2(
					typename mse::impl::IsDereferenceable_msemsearray<_TRALoneParamRR>::type(), param);
			}
			template<class _TRALoneParam>
			auto begin_iter_from_lone_param(_TRALoneParam&& param) {
				typedef mse::impl::remove_reference_t<_TRALoneParam> _TRALoneParamRR;
				return mse::impl::iterator::begin_iter_from_lone_param2(
					typename mse::impl::IsDereferenceable_msemsearray<_TRALoneParamRR>::type(), MSE_FWD(param));
			}
		}

		namespace iterator {

			template <typename _TRAPointer>
			auto xscope_begin_iter_from_ptr_helper3(std::true_type, const _TRAPointer& ptr) {
				return (*ptr).xscope_ss_begin(ptr);
			}
			template <typename _TRAPointer>
			auto xscope_begin_iter_from_ptr_helper3(std::false_type, const _TRAPointer& ptr) {
				return mse::TXScopeRAIterator<_TRAPointer>(ptr, 0);
			}
			template <typename _TXSRAPointer>
			auto xscope_begin_iter_from_ptr_helper2(std::true_type, const _TXSRAPointer& xsptr) {
				/* xsptr seems to be an xscope pointer or convertible to one.*/
				typedef mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(*xsptr)> > container_t;
				return begin_iter_from_xsptr_helper(typename mse::impl::HasOrInheritsXScopeIteratorMemberType_msemsearray<container_t>::type(), xsptr);
			}
			template <typename _TRAPointer>
			auto xscope_begin_iter_from_ptr_helper2(std::false_type, const _TRAPointer& ptr) {
				typedef mse::impl::remove_reference_t<decltype(*ptr)> container_t;
				return xscope_begin_iter_from_ptr_helper3(typename mse::impl::HasOrInheritsStaticXScopeSSBeginMethod_msemsearray<container_t>::type(), ptr);
			}

			template <typename _TRALoneParam>
			auto xscope_begin_iter_from_lone_param2(std::false_type, const _TRALoneParam& ra_container) {
				/* The parameter doesn't seem to be a pointer. So we'll use std::begin() to obtain an iterator. If you get
				a compile error here, then construction from the given parameter type isn't supported. */
				return mse::make_xscope(std::begin(ra_container));
			}
			template <typename _TRAPointer>
			auto xscope_begin_iter_from_lone_param2(std::true_type, const _TRAPointer& ptr) {
				return xscope_begin_iter_from_ptr_helper2(typename mse::impl::is_convertible_to_nonowning_scope_or_indeterminate_pointer<_TRAPointer>::type(), ptr);
			}
			template<class _TRALoneParam>
			auto xscope_begin_iter_from_lone_param(const _TRALoneParam& param) {
				typedef mse::impl::remove_reference_t<_TRALoneParam> _TRALoneParamRR;
				return mse::impl::iterator::xscope_begin_iter_from_lone_param2(
					typename mse::impl::IsDereferenceable_msemsearray<_TRALoneParamRR>::type(), param);
			}
		}

		namespace iterator {

			template <typename _TXSRAPointer>
			auto begin_const_iter_from_xsptr_helper(std::true_type, const _TXSRAPointer& xsptr) {
				typedef mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(*xsptr)> > container_t;
				return typename container_t::xscope_const_iterator(xsptr, 0);
			}
			template <typename _TXSRAPointer>
			auto begin_const_iter_from_xsptr_helper(std::false_type, const _TXSRAPointer& xsptr) {
				return mse::TXScopeRAConstIterator<_TXSRAPointer>(xsptr, 0);
			}

			template <typename _TRAPointer>
			auto begin_const_iter_from_ptr_helper4(std::true_type, const _TRAPointer& ptr) {
				return mse::TRAConstIterator<_TRAPointer>(ptr, 0);
				//return mse::make_random_access_const_iterator(ptr, 0);
			}
			template <typename _TRAPointer>
			auto begin_const_iter_from_ptr_helper4(std::false_type, const _TRAPointer& ptr) {
				return std::cbegin(*ptr);
			}
			template <typename _TRAPointer>
			auto begin_const_iter_from_ptr_helper3(std::true_type, const _TRAPointer& ptr) {
				typedef mse::impl::remove_reference_t<decltype(*ptr)> container_t;
				return container_t::ss_cbegin(ptr);
				//return (*ptr).ss_cbegin(ptr);
			}
			template <typename _TRAPointer>
			auto begin_const_iter_from_ptr_helper3(std::false_type, const _TRAPointer& ptr) {
				typedef mse::impl::remove_reference_t<decltype(*ptr)> container_t;
				return begin_const_iter_from_ptr_helper4(typename mse::impl::is_random_access_container<container_t>::type(), ptr);
				//return mse::TRAConstIterator<_TRAPointer>(ptr, 0);
				//return mse::make_random_access_const_iterator(ptr, 0);
			}
			template <typename _TXSRAPointer>
			auto begin_const_iter_from_ptr_helper2(std::true_type, const _TXSRAPointer& xsptr) {
				/* xsptr seems to be an xscope pointer.*/
				typedef mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(*xsptr)> > container_t;
				return begin_const_iter_from_xsptr_helper(typename mse::impl::HasOrInheritsXScopeIteratorMemberType_msemsearray<container_t>::type(), xsptr);
			}
			template <typename _TRAPointer>
			auto begin_const_iter_from_ptr_helper2(std::false_type, const _TRAPointer& ptr) {
				typedef mse::impl::remove_reference_t<decltype(*ptr)> container_t;
				return begin_const_iter_from_ptr_helper3(typename mse::impl::HasOrInheritsStaticSSBeginMethod_msemsearray<container_t>::type(), ptr);
			}

			template <typename _TRALoneParam>
			auto begin_const_iter_from_lone_param2(std::false_type, const _TRALoneParam& ra_container) {
				/* The parameter doesn't seem to be a pointer. So we'll use std::cbegin() to obtain an iterator. If you get
				a compile error here, then construction from the given parameter type isn't supported. */
				return std::cbegin(ra_container);
			}
			template <typename _TRAPointer>
			auto begin_const_iter_from_lone_param2(std::true_type, const _TRAPointer& ptr) {
				return begin_const_iter_from_ptr_helper2(typename mse::impl::is_nonowning_scope_pointer<_TRAPointer>::type(), ptr);
			}
			template<class _TRALoneParam>
			auto begin_const_iter_from_lone_param(const _TRALoneParam& param) {
				typedef mse::impl::remove_reference_t<_TRALoneParam> _TRALoneParamRR;
				return mse::impl::iterator::begin_const_iter_from_lone_param2(typename mse::impl::IsDereferenceable_msemsearray<_TRALoneParamRR>::type(), param);
			}
		}

		namespace iterator {

			template <typename _TRAPointer>
			auto xscope_begin_const_iter_from_ptr_helper3(std::true_type, const _TRAPointer& ptr) {
				return (*ptr).xscope_ss_cbegin(ptr);
			}
			template <typename _TRAPointer>
			auto xscope_begin_const_iter_from_ptr_helper3(std::false_type, const _TRAPointer& ptr) {
				return mse::TXScopeRAConstIterator<_TRAPointer>(ptr, 0);
			}
			template <typename _TXSRAPointer>
			auto xscope_begin_const_iter_from_ptr_helper2(std::true_type, const _TXSRAPointer& xsptr) {
				/* xsptr seems to be an xscope pointer.*/
				typedef mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(*xsptr)> > container_t;
				return begin_const_iter_from_xsptr_helper(typename mse::impl::HasOrInheritsXScopeIteratorMemberType_msemsearray<container_t>::type(), xsptr);
			}
			template <typename _TRAPointer>
			auto xscope_begin_const_iter_from_ptr_helper2(std::false_type, const _TRAPointer& ptr) {
				typedef mse::impl::remove_reference_t<decltype(*ptr)> container_t;
				return xscope_begin_const_iter_from_ptr_helper3(typename mse::impl::HasOrInheritsStaticXScopeSSBeginMethod_msemsearray<container_t>::type(), ptr);
			}

			template <typename _TRALoneParam>
			auto xscope_begin_const_iter_from_lone_param2(std::false_type, const _TRALoneParam& ra_container) {
				/* The parameter doesn't seem to be a pointer. So we'll use std::cbegin() to obtain an iterator. If you get
				a compile error here, then construction from the given parameter type isn't supported. */
				return mse::make_xscope(std::cbegin(ra_container));
			}
			template <typename _TRAPointer>
			auto xscope_begin_const_iter_from_lone_param2(std::true_type, const _TRAPointer& ptr) {
				return xscope_begin_const_iter_from_ptr_helper2(typename mse::impl::is_convertible_to_nonowning_scope_or_indeterminate_pointer<_TRAPointer>::type(), ptr);
			}
			template<class _TRALoneParam>
			auto xscope_begin_const_iter_from_lone_param(const _TRALoneParam& param) {
				typedef mse::impl::remove_reference_t<_TRALoneParam> _TRALoneParamRR;
				return mse::impl::iterator::xscope_begin_const_iter_from_lone_param2(
					typename mse::impl::IsDereferenceable_msemsearray<_TRALoneParamRR>::type(), param);
			}
			template<class _TRALoneParam>
			auto xscope_begin_const_iter_from_lone_param(_TRALoneParam&& param) {
				typedef mse::impl::remove_reference_t<_TRALoneParam> _TRALoneParamRR;
				return mse::impl::iterator::xscope_begin_const_iter_from_lone_param2(
					typename mse::impl::IsDereferenceable_msemsearray<_TRALoneParamRR>::type(), MSE_FWD(param));
			}
		}

	}

	MSE_IMPL_FUNCTION_ALIAS_DECLARATION(make_xscope_const_iterator, impl::iterator::xscope_begin_const_iter_from_lone_param);
	MSE_IMPL_FUNCTION_ALIAS_DECLARATION(make_xscope_iterator, impl::iterator::xscope_begin_iter_from_lone_param);

	/* Overloads for rsv::TReturnableFParam<>. */
	//MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_const_iterator)
	//MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_iterator)
	template <typename _Ty, class... _Args, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::is_potentially_xscope<_Ty>::value && (!mse::impl::is_pointer_to_lockable_structure_container<_Ty>::value)> MSE_IMPL_EIS >
	auto make_xscope_iterator(const mse::rsv::TReturnableFParam<_Ty>& param, _Args&&... _Ax)
		-> decltype(mse::rsv::as_a_returnable_fparam(make_xscope_iterator(mse::us::impl::raw_reference_to<_Ty>(param), std::forward<_Args>(_Ax)...))) {
		const auto& param_base_ref = mse::us::impl::raw_reference_to<_Ty>(param);
		return mse::rsv::as_a_returnable_fparam(make_xscope_iterator(param_base_ref, std::forward<_Args>(_Ax)...));
	}
	template <typename _Ty, class... _Args, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::is_potentially_xscope<_Ty>::value && (!mse::impl::is_pointer_to_lockable_structure_container<_Ty>::value)> MSE_IMPL_EIS >
	auto make_xscope_iterator(mse::rsv::TReturnableFParam<_Ty>& param, _Args&&... _Ax)
		-> decltype(mse::rsv::as_a_returnable_fparam(make_xscope_iterator(mse::us::impl::raw_reference_to<_Ty>(param), std::forward<_Args>(_Ax)...))) {
		auto& param_base_ref = mse::us::impl::raw_reference_to<_Ty>(param);
		return mse::rsv::as_a_returnable_fparam(make_xscope_iterator(param_base_ref, std::forward<_Args>(_Ax)...));
	}
	template <typename _Ty, class... _Args, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::is_potentially_xscope<_Ty>::value && (!mse::impl::is_pointer_to_lockable_structure_container<_Ty>::value)> MSE_IMPL_EIS >
	auto make_xscope_iterator(mse::rsv::TReturnableFParam<_Ty>&& param, _Args&&... _Ax) {
		return mse::rsv::as_a_returnable_fparam(make_xscope_iterator(std::forward<_Ty>(param), std::forward<_Args>(_Ax)...));
	}

	template <typename _Ty, class... _Args, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::is_potentially_xscope<_Ty>::value && (!mse::impl::is_pointer_to_lockable_structure_container<_Ty>::value)> MSE_IMPL_EIS >
	auto make_xscope_const_iterator(const mse::rsv::TReturnableFParam<_Ty>& param, _Args&&... _Ax)
		-> decltype(mse::rsv::as_a_returnable_fparam(make_xscope_const_iterator(mse::us::impl::raw_reference_to<_Ty>(param), std::forward<_Args>(_Ax)...))) {
		const auto& param_base_ref = mse::us::impl::raw_reference_to<_Ty>(param);
		return mse::rsv::as_a_returnable_fparam(make_xscope_const_iterator(param_base_ref, std::forward<_Args>(_Ax)...));
	}
	template <typename _Ty, class... _Args, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::is_potentially_xscope<_Ty>::value && (!mse::impl::is_pointer_to_lockable_structure_container<_Ty>::value)> MSE_IMPL_EIS >
	auto make_xscope_const_iterator(mse::rsv::TReturnableFParam<_Ty>& param, _Args&&... _Ax)
		-> decltype(mse::rsv::as_a_returnable_fparam(make_xscope_const_iterator(mse::us::impl::raw_reference_to<_Ty>(param), std::forward<_Args>(_Ax)...))) {
		auto& param_base_ref = mse::us::impl::raw_reference_to<_Ty>(param);
		return mse::rsv::as_a_returnable_fparam(make_xscope_const_iterator(param_base_ref, std::forward<_Args>(_Ax)...));
	}
	template <typename _Ty, class... _Args, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::is_potentially_xscope<_Ty>::value && (!mse::impl::is_pointer_to_lockable_structure_container<_Ty>::value)> MSE_IMPL_EIS >
	auto make_xscope_const_iterator(mse::rsv::TReturnableFParam<_Ty>&& param, _Args&&... _Ax) {
		return mse::rsv::as_a_returnable_fparam(make_xscope_const_iterator(std::forward<_Ty>(param), std::forward<_Args>(_Ax)...));
	}


	/* We had iterators in mind with these overloads of operator+ and operator-. */
	template <typename _Ty, typename _Tz>
	auto operator+(const rsv::TReturnableFParam<_Ty>& y, const _Tz& z) {
		const _Ty& param_base_ref = y;
		return rsv::as_a_returnable_fparam(param_base_ref + z);
	}
	template <typename _Ty, typename _Tz>
	auto operator-(const rsv::TReturnableFParam<_Ty>& y, const _Tz& z) {
		const _Ty& param_base_ref = y;
		return rsv::as_a_returnable_fparam(param_base_ref - z);
	}

	MSE_IMPL_FUNCTION_ALIAS_DECLARATION(make_xscope_begin_const_iterator, make_xscope_const_iterator);
	MSE_IMPL_FUNCTION_ALIAS_DECLARATION(make_xscope_begin_iterator, make_xscope_iterator);

#define MSE_IMPL_MAKE_END_ITERATOR_FUNCTION_DECLARATION1(make_end_const_iterator_function, make_begin_const_iterator_function) \
	template<class _TArrayPointer> \
	auto make_end_const_iterator_function(const _TArrayPointer& owner_ptr) { \
		typedef typename mse::difference_type_of_iterator<decltype(mse::make_begin_const_iterator_function(owner_ptr))>::type difference_type; \
		return mse::make_begin_const_iterator_function(owner_ptr) + difference_type(mse::container_size(owner_ptr) - 0); \
	} \
	template<class _TArrayPointer> \
	auto make_end_const_iterator_function(_TArrayPointer&& owner_ptr) { \
		typedef typename mse::difference_type_of_iterator<decltype(mse::make_xscope_begin_const_iterator(MSE_FWD(owner_ptr)))>::type difference_type; \
		return mse::make_begin_const_iterator_function(MSE_FWD(owner_ptr)) + difference_type(mse::container_size(owner_ptr) - 0); \
	}

	MSE_IMPL_MAKE_END_ITERATOR_FUNCTION_DECLARATION1(make_xscope_end_const_iterator, make_xscope_begin_const_iterator);
	MSE_IMPL_MAKE_END_ITERATOR_FUNCTION_DECLARATION1(make_xscope_end_iterator, make_xscope_begin_iterator);

	template<class _TArrayPointer>
	auto make_const_iterator(const _TArrayPointer& param) {
		return mse::impl::iterator::begin_const_iter_from_lone_param(param);
	}

	template<class _TRALoneParam>
	auto make_iterator(const _TRALoneParam& param) {
		return mse::impl::iterator::begin_iter_from_lone_param(param);
	}
	template<class _TRALoneParam>
	auto make_iterator(_TRALoneParam&& param) {
		return mse::impl::iterator::begin_iter_from_lone_param(MSE_FWD(param));
	}

	MSE_IMPL_FUNCTION_ALIAS_DECLARATION(make_begin_const_iterator, make_const_iterator);
	MSE_IMPL_FUNCTION_ALIAS_DECLARATION(make_begin_iterator, make_iterator);

	namespace impl {
		template<class _TArrayPointer>
		auto make_end_const_iterator_helper1(std::true_type, const _TArrayPointer& owner_ptr) {
			typedef typename mse::difference_type_of_iterator<decltype(mse::make_begin_const_iterator(owner_ptr))>::type difference_type;
			return mse::make_begin_const_iterator(owner_ptr) + difference_type(mse::container_size(owner_ptr) - 0);
		}
		template<class _TArrayPointer>
		auto make_end_const_iterator_helper1(std::false_type, const _TArrayPointer& owner_ptr) {
			return std::cend(*owner_ptr);
		}

		template<class _TArrayPointer>
		auto make_end_iterator_helper1(std::true_type, _TArrayPointer& owner_ptr) {
			typedef typename mse::difference_type_of_iterator<decltype(mse::make_begin_iterator(owner_ptr))>::type difference_type;
			return mse::make_begin_iterator(owner_ptr) + difference_type(mse::container_size(owner_ptr) - 0);
		}
		template<class _TArrayPointer>
		auto make_end_iterator_helper1(std::true_type, _TArrayPointer&& owner_ptr) {
			typedef typename mse::difference_type_of_iterator<decltype(mse::make_begin_iterator(MSE_FWD(owner_ptr)))>::type difference_type;
			return mse::make_begin_iterator(MSE_FWD(owner_ptr)) + difference_type(mse::container_size(owner_ptr) - 0);
		}
		template<class _TArrayPointer>
		auto make_end_iterator_helper1(std::false_type, _TArrayPointer& owner_ptr) {
			return std::end(*owner_ptr);
		}
	}
	template<class _TArrayPointer>
	auto make_end_const_iterator(const _TArrayPointer& owner_ptr) {
		typedef mse::impl::remove_reference_t<decltype(*owner_ptr)> container_t;
		return mse::impl::make_end_const_iterator_helper1(typename mse::impl::is_random_access_container<container_t>::type(), owner_ptr);
	}
	template<class _TArrayPointer>
	auto make_end_iterator(_TArrayPointer& owner_ptr) {
		typedef mse::impl::remove_reference_t<decltype(*owner_ptr)> container_t;
		return mse::impl::make_end_iterator_helper1(typename mse::impl::is_random_access_container<container_t>::type(), owner_ptr);
	}
	template<class _TArrayPointer>
	auto make_end_iterator(_TArrayPointer&& owner_ptr) {
		typedef mse::impl::remove_reference_t<decltype(*owner_ptr)> container_t;
		return mse::impl::make_end_iterator_helper1(typename mse::impl::is_random_access_container<container_t>::type(), MSE_FWD(owner_ptr));
	}


	template <typename _TRAIterator> class TXScopeRASectionIterator;
	template <typename _TRAIterator> class TXScopeRASectionConstIterator;

	namespace us {
		namespace impl {
			template <typename _TRAIterator> class TRASectionIteratorBaseFriend1;

			template <typename _TRAIterator> class TRASectionConstIteratorBase;

			template <typename _TRAIterator>
			class TRASectionIteratorBase : public mse::impl::random_access_iterator_base_from_ra_iterator<_TRAIterator>
				, MSE_INHERIT_COMMON_XSCOPE_ITERATOR_TAG_BASE_SET_FROM(_TRAIterator, TRASectionIteratorBase<_TRAIterator>)
			{
			public:
				typedef mse::impl::random_access_iterator_base_from_ra_iterator<_TRAIterator> base_class;
				MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);
				typedef _TRAIterator iterator_type;

			private:
				const _TRAIterator m_ra_iterator;
				const size_type m_count = 0;
				difference_type m_index = 0;

			public:
				TRASectionIteratorBase(const TRASectionIteratorBase& src)
					: m_ra_iterator(src.m_ra_iterator), m_count(src.m_count), m_index(src.m_index) {}
				TRASectionIteratorBase(_TRAIterator ra_iterator, size_type count, size_type index = 0)
					: m_ra_iterator(ra_iterator), m_count(count), m_index(difference_type(mse::msear_as_a_size_t(index))) {}

				void bounds_check(difference_type index) const {
					if ((0 > index) || (difference_type(mse::msear_as_a_size_t(m_count)) <= index)) {
						MSE_THROW(msearray_range_error("out of bounds index - void bounds_check() - TRASectionIteratorBase"));
					}
				}
				void dereference_bounds_check() const {
					bounds_check(m_index);
				}
				auto operator*() const -> reference {
					dereference_bounds_check();
					return m_ra_iterator[m_index];
				}
				auto operator->() const -> typename std::add_pointer<value_type>::type {
					dereference_bounds_check();
					return std::addressof(m_ra_iterator[m_index]);
				}
				reference operator[](difference_type _Off) const {
					bounds_check(_Off);
					return m_ra_iterator[_Off];
				}
				TRASectionIteratorBase& operator +=(difference_type x) {
					m_index += (x);
					return (*this);
				}
				TRASectionIteratorBase& operator -=(difference_type x) { operator +=(-x); return (*this); }
				TRASectionIteratorBase& operator ++() { operator +=(1); return (*this); }
				TRASectionIteratorBase operator ++(int) { auto _Tmp = *this; operator +=(1); return (_Tmp); }
				TRASectionIteratorBase& operator --() { operator -=(1); return (*this); }
				TRASectionIteratorBase operator --(int) { auto _Tmp = *this; operator -=(1); return (_Tmp); }

				TRASectionIteratorBase operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
				TRASectionIteratorBase operator-(difference_type n) const { return ((*this) + (-n)); }
				difference_type operator-(const TRASectionIteratorBase& _Right_cref) const {
					if (!(_Right_cref.m_ra_iterator == m_ra_iterator)) { MSE_THROW(msearray_range_error("invalid argument - difference_type operator-() - TRASectionIteratorBase")); }
					return m_index - _Right_cref.m_index;
				}
				bool operator ==(const TRASectionIteratorBase& _Right_cref) const {
					return ((_Right_cref.m_index == m_index) && (_Right_cref.m_count == m_count) && (_Right_cref.m_ra_iterator == m_ra_iterator));
				}
				bool operator !=(const TRASectionIteratorBase& _Right_cref) const { return !((*this) == _Right_cref); }
				bool operator<(const TRASectionIteratorBase& _Right_cref) const { return (0 > operator-(_Right_cref)); }
				bool operator>(const TRASectionIteratorBase& _Right_cref) const { return (0 > operator-(_Right_cref)); }
				bool operator<=(const TRASectionIteratorBase& _Right_cref) const { return (0 >= operator-(_Right_cref)); }
				bool operator>=(const TRASectionIteratorBase& _Right_cref) const { return (0 >= operator-(_Right_cref)); }
				TRASectionIteratorBase& operator=(const TRASectionIteratorBase& _Right_cref) {
					if (!(_Right_cref.m_ra_iterator == m_ra_iterator)) { MSE_THROW(msearray_range_error("invalid argument - TRASectionIteratorBase& operator=() - TRASectionIteratorBase")); }
					m_index = _Right_cref.m_index;
					return (*this);
				}

				friend class TRASectionConstIteratorBase<_TRAIterator>;
				template <typename _TRAIterator2>
				friend class TXScopeRASectionIterator;
				template <typename _TRAIterator2>
				friend class TRASectionIteratorBaseFriend1;
			};

			template <typename _TRAIterator>
			class TRASectionIteratorBaseFriend1 {
			public:
				static auto xscope_pointer(const TRASectionIteratorBase<_TRAIterator>& ra_section_iter) {
					ra_section_iter.dereference_bounds_check();
					auto ra_iter = ra_section_iter.m_ra_iterator + ra_section_iter.m_index;
					return mse::xscope_pointer(ra_iter);
				}
				static auto xscope_pointer(TRASectionIteratorBase<_TRAIterator>&& ra_section_iter) {
					ra_section_iter.dereference_bounds_check();
					return mse::xscope_pointer(MSE_FWD(ra_section_iter).m_ra_iterator + ra_section_iter.m_index);
				}
				static auto xscope_const_pointer(const TRASectionIteratorBase<_TRAIterator>& ra_section_iter) {
					ra_section_iter.dereference_bounds_check();
					auto ra_iter = ra_section_iter.m_ra_iterator + ra_section_iter.m_index;
					return mse::xscope_const_pointer(ra_iter);
				}
				static auto xscope_const_pointer(TRASectionIteratorBase<_TRAIterator>&& ra_section_iter) {
					ra_section_iter.dereference_bounds_check();
					return mse::xscope_const_pointer(MSE_FWD(ra_section_iter).m_ra_iterator + ra_section_iter.m_index);
				}
			};
		}
	}

	template <typename _TRAIterator>
	class TXScopeRASectionIterator : public us::impl::TRASectionIteratorBase<_TRAIterator>, public mse::us::impl::XScopeTagBase {
	public:
		typedef us::impl::TRASectionIteratorBase<_TRAIterator> base_class;
		MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);
		typedef typename base_class::iterator_type iterator_type;

		TXScopeRASectionIterator(const us::impl::TRASectionIteratorBase<_TRAIterator>& src)
			: base_class(src) {}
		TXScopeRASectionIterator(_TRAIterator ra_iterator, size_type count, size_type index = 0)
			: base_class(ra_iterator, count, index) {}

		TXScopeRASectionIterator& operator +=(difference_type x) {
			base_class::operator +=(x);
			return (*this);
		}
		TXScopeRASectionIterator& operator -=(difference_type x) { operator +=(-x); return (*this); }
		TXScopeRASectionIterator& operator ++() { operator +=(1); return (*this); }
		TXScopeRASectionIterator operator ++(int) { auto _Tmp = *this; operator +=(1); return (_Tmp); }
		TXScopeRASectionIterator& operator --() { operator -=(1); return (*this); }
		TXScopeRASectionIterator operator --(int) { auto _Tmp = *this; operator -=(1); return (_Tmp); }

		TXScopeRASectionIterator operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
		TXScopeRASectionIterator operator-(difference_type n) const { return ((*this) + (-n)); }
		difference_type operator-(const us::impl::TRASectionIteratorBase<_TRAIterator>& _Right_cref) const {
			return base_class::operator-(_Right_cref);
		}

		TXScopeRASectionIterator& operator=(const us::impl::TRASectionIteratorBase<_TRAIterator>& _Right_cref) {
			base_class::operator=(_Right_cref);
			return (*this);
		}

		void xscope_tag() const {}
		void xscope_iterator_tag() const {}
		void async_not_shareable_and_not_passable_tag() const {}

	private:
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
	};

	template <typename _TRAIterator>
	class TRASectionIterator : public us::impl::TRASectionIteratorBase<_TRAIterator> {
	public:
		typedef us::impl::TRASectionIteratorBase<_TRAIterator> base_class;
		MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);
		typedef typename base_class::iterator_type iterator_type;

		TRASectionIterator(const TRASectionIterator& src)
			: base_class(src) {}
		template <typename _TRAIterator1>
		TRASectionIterator(_TRAIterator1 ra_iterator, size_type count, size_type index = 0) : base_class(ra_iterator, count, index) {}
		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TRASectionIterator() {
			mse::impl::T_valid_if_not_an_xscope_type<_TRAIterator>();
		}

		TRASectionIterator& operator +=(difference_type x) {
			base_class::operator +=(x);
			return (*this);
		}
		TRASectionIterator& operator -=(difference_type x) { operator +=(-x); return (*this); }
		TRASectionIterator& operator ++() { operator +=(1); return (*this); }
		TRASectionIterator operator ++(int) { auto _Tmp = *this; operator +=(1); return (_Tmp); }
		TRASectionIterator& operator --() { operator -=(1); return (*this); }
		TRASectionIterator operator --(int) { auto _Tmp = *this; operator -=(1); return (_Tmp); }

		TRASectionIterator operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
		TRASectionIterator operator-(difference_type n) const { return ((*this) + (-n)); }
		difference_type operator-(const us::impl::TRASectionIteratorBase<_TRAIterator>& _Right_cref) const {
			return base_class::operator-(_Right_cref);
		}

		TRASectionIterator& operator=(const us::impl::TRASectionIteratorBase<_TRAIterator>& _Right_cref) {
			base_class::operator=(_Right_cref);
			return (*this);
		}
	};

	namespace us {
		namespace impl {
			template <typename _TRAIterator> class TRASectionConstIteratorBaseFriend1;

			template <typename _TRAIterator>
			class TRASectionConstIteratorBase : public mse::impl::random_access_const_iterator_base_from_ra_iterator<_TRAIterator>
				, MSE_INHERIT_COMMON_XSCOPE_ITERATOR_TAG_BASE_SET_FROM(_TRAIterator, TRASectionConstIteratorBase<_TRAIterator>)
			{
			public:
				typedef mse::impl::random_access_const_iterator_base_from_ra_iterator<_TRAIterator> base_class;
				MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);
				typedef _TRAIterator iterator_type;

			private:
				const _TRAIterator m_ra_iterator;
				const size_type m_count = 0;
				difference_type m_index = 0;

			public:
				TRASectionConstIteratorBase(const TRASectionConstIteratorBase& src)
					: m_ra_iterator(src.m_ra_iterator), m_count(src.m_count), m_index(src.m_index) {}
				TRASectionConstIteratorBase(const TRASectionIteratorBase<_TRAIterator>& src)
					: m_ra_iterator(src.m_ra_iterator), m_count(src.m_count), m_index(src.m_index) {}
				TRASectionConstIteratorBase(_TRAIterator ra_iterator, size_type count, size_type index = 0)
					: m_ra_iterator(ra_iterator), m_count(count), m_index(difference_type(mse::msear_as_a_size_t(index))) {}

				void bounds_check(difference_type index) const {
					if ((0 > index) || (difference_type(mse::msear_as_a_size_t(m_count)) <= index)) {
						MSE_THROW(msearray_range_error("out of bounds index - void bounds_check() - TRASectionConstIteratorBase"));
					}
				}
				void dereference_bounds_check() const {
					bounds_check(m_index);
				}
				auto operator*() const -> const_reference {
					dereference_bounds_check();
					return m_ra_iterator[m_index];
				}
				auto operator->() const -> typename std::add_pointer<typename std::add_const<value_type>::type>::type {
					dereference_bounds_check();
					return std::addressof(m_ra_iterator[m_index]);
				}
				const_reference operator[](difference_type _Off) const {
					bounds_check(_Off);
					return m_ra_iterator[_Off];
				}
				TRASectionConstIteratorBase& operator +=(difference_type x) {
					m_index += (x);
					return (*this);
				}
				TRASectionConstIteratorBase& operator -=(difference_type x) { operator +=(-x); return (*this); }
				TRASectionConstIteratorBase& operator ++() { operator +=(1); return (*this); }
				TRASectionConstIteratorBase operator ++(int) { auto _Tmp = *this; operator +=(1); return (_Tmp); }
				TRASectionConstIteratorBase& operator --() { operator -=(1); return (*this); }
				TRASectionConstIteratorBase operator --(int) { auto _Tmp = *this; operator -=(1); return (_Tmp); }

				TRASectionConstIteratorBase operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
				TRASectionConstIteratorBase operator-(difference_type n) const { return ((*this) + (-n)); }
				difference_type operator-(const TRASectionConstIteratorBase& _Right_cref) const {
					if (!(_Right_cref.m_ra_iterator == m_ra_iterator)) { MSE_THROW(msearray_range_error("invalid argument - difference_type operator-() - TRASectionConstIteratorBase")); }
					return m_index - _Right_cref.m_index;
				}
				bool operator ==(const TRASectionConstIteratorBase& _Right_cref) const {
					return ((_Right_cref.m_index == m_index) && (_Right_cref.m_count == m_count) && (_Right_cref.m_ra_iterator == m_ra_iterator));
				}
				bool operator !=(const TRASectionConstIteratorBase& _Right_cref) const { return !((*this) == _Right_cref); }
				bool operator<(const TRASectionConstIteratorBase& _Right_cref) const { return (0 > operator-(_Right_cref)); }
				bool operator>(const TRASectionConstIteratorBase& _Right_cref) const { return (0 > operator-(_Right_cref)); }
				bool operator<=(const TRASectionConstIteratorBase& _Right_cref) const { return (0 >= operator-(_Right_cref)); }
				bool operator>=(const TRASectionConstIteratorBase& _Right_cref) const { return (0 >= operator-(_Right_cref)); }
				TRASectionConstIteratorBase& operator=(const TRASectionConstIteratorBase& _Right_cref) {
					if (!(_Right_cref.m_ra_iterator == m_ra_iterator)) { MSE_THROW(msearray_range_error("invalid argument - TRASectionConstIteratorBase& operator=() - TRASectionConstIteratorBase")); }
					m_index = _Right_cref.m_index;
					return (*this);
				}

				template <typename _TRAIterator2>
				friend class TXScopeRASectionConstIterator;
				template <typename _TRAIterator2>
				friend class TRASectionConstIteratorBaseFriend1;
			};

			template <typename _TRAIterator>
			class TRASectionConstIteratorBaseFriend1 {
			public:
				static auto xscope_const_pointer(const TRASectionConstIteratorBase<_TRAIterator>& ra_section_iter) {
					ra_section_iter.dereference_bounds_check();
					auto ra_iter = ra_section_iter.m_ra_iterator + ra_section_iter.m_index;
					return mse::xscope_const_pointer(ra_iter);
				}
				static auto xscope_const_pointer(TRASectionConstIteratorBase<_TRAIterator>&& ra_section_iter) {
					ra_section_iter.dereference_bounds_check();
					return mse::xscope_const_pointer(MSE_FWD(ra_section_iter).m_ra_iterator + ra_section_iter.m_index);
				}
			};
		}
	}

	template <typename _TRAIterator>
	class TXScopeRASectionConstIterator : public us::impl::TRASectionConstIteratorBase<_TRAIterator>, public mse::us::impl::XScopeTagBase {
	public:
		typedef us::impl::TRASectionConstIteratorBase<_TRAIterator> base_class;
		MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);
		typedef typename base_class::iterator_type iterator_type;

		TXScopeRASectionConstIterator(const us::impl::TRASectionConstIteratorBase<_TRAIterator>& src)
			: base_class(src) {}
		TXScopeRASectionConstIterator(_TRAIterator ra_iterator, size_type count, size_type index = 0)
			: base_class(ra_iterator, count, index) {}

		TXScopeRASectionConstIterator& operator +=(difference_type x) {
			base_class::operator +=(x);
			return (*this);
		}
		TXScopeRASectionConstIterator& operator -=(difference_type x) { operator +=(-x); return (*this); }
		TXScopeRASectionConstIterator& operator ++() { operator +=(1); return (*this); }
		TXScopeRASectionConstIterator operator ++(int) { auto _Tmp = *this; operator +=(1); return (_Tmp); }
		TXScopeRASectionConstIterator& operator --() { operator -=(1); return (*this); }
		TXScopeRASectionConstIterator operator --(int) { auto _Tmp = *this; operator -=(1); return (_Tmp); }

		TXScopeRASectionConstIterator operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
		TXScopeRASectionConstIterator operator-(difference_type n) const { return ((*this) + (-n)); }
		difference_type operator-(const us::impl::TRASectionConstIteratorBase<_TRAIterator>& _Right_cref) const {
			return base_class::operator-(_Right_cref);
		}

		TXScopeRASectionConstIterator& operator=(const us::impl::TRASectionConstIteratorBase<_TRAIterator>& _Right_cref) {
			base_class::operator=(_Right_cref);
			return (*this);
		}

		void xscope_tag() const {}
		void xscope_iterator_tag() const {}
		void async_not_shareable_and_not_passable_tag() const {}

	private:
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
	};

	template <typename _TRAIterator>
	class TRASectionConstIterator : public us::impl::TRASectionConstIteratorBase<_TRAIterator> {
	public:
		typedef us::impl::TRASectionConstIteratorBase<_TRAIterator> base_class;
		MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);
		typedef typename base_class::iterator_type iterator_type;

		TRASectionConstIterator(const TRASectionConstIterator& src) : base_class(src) {}
		template <typename _TRAIterator1>
		TRASectionConstIterator(_TRAIterator1 ra_iterator, size_type count, size_type index = 0)
			: base_class(ra_iterator, count, index) {}
		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TRASectionConstIterator() {
			mse::impl::T_valid_if_not_an_xscope_type<_TRAIterator>();
		}

		TRASectionConstIterator& operator +=(difference_type x) {
			base_class::operator +=(x);
			return (*this);
		}
		TRASectionConstIterator& operator -=(difference_type x) { operator +=(-x); return (*this); }
		TRASectionConstIterator& operator ++() { operator +=(1); return (*this); }
		TRASectionConstIterator operator ++(int) { auto _Tmp = *this; operator +=(1); return (_Tmp); }
		TRASectionConstIterator& operator --() { operator -=(1); return (*this); }
		TRASectionConstIterator operator --(int) { auto _Tmp = *this; operator -=(1); return (_Tmp); }

		TRASectionConstIterator operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
		TRASectionConstIterator operator-(difference_type n) const { return ((*this) + (-n)); }
		difference_type operator-(const us::impl::TRASectionConstIteratorBase<_TRAIterator>& _Right_cref) const {
			return base_class::operator-(_Right_cref);
		}

		TRASectionConstIterator& operator=(const us::impl::TRASectionConstIteratorBase<_TRAIterator>& _Right_cref) {
			base_class::operator=(_Right_cref);
			return (*this);
		}
	};

	template <typename _TRAIterator>
	auto xscope_pointer(const TXScopeRASectionIterator<_TRAIterator>& iter_cref) {
		return mse::us::impl::TRASectionIteratorBaseFriend1<_TRAIterator>::xscope_pointer(iter_cref);
	}
	template <typename _TRAIterator>
	auto xscope_pointer(const TXScopeRASectionConstIterator<_TRAIterator>& iter_cref) {
		return mse::us::impl::TRASectionConstIteratorBaseFriend1<_TRAIterator>::xscope_const_pointer(iter_cref);
	}
	template <typename _TRAIterator>
	auto xscope_const_pointer(const TXScopeRASectionIterator<_TRAIterator>& iter_cref) {
		return mse::us::impl::TRASectionIteratorBaseFriend1<_TRAIterator>::xscope_const_pointer(iter_cref);
	}
	template <typename _TRAIterator>
	auto xscope_const_pointer(const TXScopeRASectionConstIterator<_TRAIterator>& iter_cref) {
		return mse::us::impl::TRASectionConstIteratorBaseFriend1<_TRAIterator>::xscope_const_pointer(iter_cref);
	}

	template <typename _TRAIterator>
	auto xscope_pointer(TXScopeRASectionIterator<_TRAIterator>&& iter_cref) {
		return mse::us::impl::TRASectionIteratorBaseFriend1<_TRAIterator>::xscope_pointer(MSE_FWD(iter_cref));
	}
	template <typename _TRAIterator>
	auto xscope_pointer(TXScopeRASectionConstIterator<_TRAIterator>&& iter_cref) {
		return mse::us::impl::TRASectionConstIteratorBaseFriend1<_TRAIterator>::xscope_const_pointer(MSE_FWD(iter_cref));
	}
	template <typename _TRAIterator>
	auto xscope_const_pointer(TXScopeRASectionIterator<_TRAIterator>&& iter_cref) {
		return mse::us::impl::TRASectionIteratorBaseFriend1<_TRAIterator>::xscope_const_pointer(MSE_FWD(iter_cref));
	}
	template <typename _TRAIterator>
	auto xscope_const_pointer(TXScopeRASectionConstIterator<_TRAIterator>&& iter_cref) {
		return mse::us::impl::TRASectionConstIteratorBaseFriend1<_TRAIterator>::xscope_const_pointer(MSE_FWD(iter_cref));
	}

	namespace us {
		namespace impl {
			template <typename _TRAIterator> class TRandomAccessSectionBase;
			template <typename _TRAIterator> class TRandomAccessConstSectionBase;
		}
	}

	template <typename _TRAIterator> class TXScopeCSSSStrongRASectionIterator;
	template <typename _TRAIterator> class TXScopeCSSSStrongRASectionConstIterator;

	template <typename _TRAIterator>
	class TXScopeCSSSStrongRASectionIterator : public TXScopeRASectionIterator<_TRAIterator> {
	public:
		typedef TXScopeRASectionIterator<_TRAIterator> base_class;
		MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

		TXScopeCSSSStrongRASectionIterator(const TXScopeCSSSStrongRASectionIterator& src) : base_class(src) {}
		TXScopeCSSSStrongRASectionIterator(TXScopeCSSSStrongRASectionIterator&& src) : base_class(MSE_FWD(src)) {}

		auto& operator=(const TXScopeCSSSStrongRASectionIterator& _Right_cref) { base_class::operator=(_Right_cref); return (*this); }
		auto& operator=(TXScopeCSSSStrongRASectionIterator&& _Right_cref) { base_class::operator=(MSE_FWD(_Right_cref)); return (*this); }
		MSE_USING_ASSIGNMENT_OPERATOR(base_class);
		MSE_INHERIT_ITERATOR_ARITHMETIC_OPERATORS_FROM(base_class, TXScopeCSSSStrongRASectionIterator);

		MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(base_class);
		void xscope_iterator_tag() const {}
		void xscope_tag() const {}

	private:
		class unsafe_t {};
		TXScopeCSSSStrongRASectionIterator(unsafe_t, const _TRAIterator& ra_iterator, size_type count, size_type index = 0) : base_class(ra_iterator, count, index) {}

		TXScopeCSSSStrongRASectionIterator(const _TRAIterator& ra_iterator, size_type count, size_type index = 0) : base_class(ra_iterator, count, index) {
			static_assert(mse::impl::is_instantiation_of<_TRAIterator, mse::TXScopeCSSSStrongRAIterator>::value
				|| mse::impl::is_instantiation_of<_TRAIterator, mse::TXScopeCSSSStrongRASectionIterator>::value
				, "");
		}

		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		friend class TXScopeCSSSStrongRASectionConstIterator<_TRAIterator>;
		template<class _TRAIterator2> friend TXScopeCSSSStrongRASectionIterator<_TRAIterator2> us::unsafe_make_xscope_csss_strong_ra_iterator(const _TRAIterator2& ra_iterator, typename TXScopeCSSSStrongRASectionIterator<_TRAIterator2>::size_type count, typename TXScopeCSSSStrongRASectionIterator<_TRAIterator2>::size_type index);
		template <typename _TRAIterator2> friend class mse::us::impl::TRandomAccessSectionBase;
		template <typename _TRAIterator2> friend class mse::us::impl::TRandomAccessConstSectionBase;
	};

	template <typename _TRAIterator>
	class TXScopeCSSSStrongRASectionConstIterator : public TXScopeRASectionConstIterator<_TRAIterator> {
	public:
		typedef TXScopeRASectionConstIterator<_TRAIterator> base_class;
		MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

		TXScopeCSSSStrongRASectionConstIterator(const TXScopeCSSSStrongRASectionConstIterator& src) : base_class(src) {}
		TXScopeCSSSStrongRASectionConstIterator(TXScopeCSSSStrongRASectionConstIterator&& src) : base_class(MSE_FWD(src)) {}

		TXScopeCSSSStrongRASectionConstIterator(const TXScopeCSSSStrongRASectionIterator<_TRAIterator>& src) : base_class(src) {}
		TXScopeCSSSStrongRASectionConstIterator(TXScopeCSSSStrongRASectionIterator<_TRAIterator>&& src) : base_class(MSE_FWD(src)) {}

		auto& operator=(const TXScopeCSSSStrongRASectionConstIterator& _Right_cref) { base_class::operator=(_Right_cref); return (*this); }
		auto& operator=(TXScopeCSSSStrongRASectionConstIterator&& _Right_cref) { base_class::operator=(MSE_FWD(_Right_cref)); return (*this); }
		MSE_USING_ASSIGNMENT_OPERATOR(base_class);
		MSE_INHERIT_ITERATOR_ARITHMETIC_OPERATORS_FROM(base_class, TXScopeCSSSStrongRASectionConstIterator);

		MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(base_class);
		void xscope_iterator_tag() const {}
		void xscope_tag() const {}

	private:
		class unsafe_t {};
		TXScopeCSSSStrongRASectionConstIterator(unsafe_t, const _TRAIterator& ra_iterator, size_type count, size_type index = 0) : base_class(ra_iterator, count, index) {}

		TXScopeCSSSStrongRASectionConstIterator(const _TRAIterator& ra_iterator, size_type count, size_type index = 0) : base_class(ra_iterator, count, index) {
			static_assert(mse::impl::is_instantiation_of<_TRAIterator, mse::TXScopeCSSSStrongRAConstIterator>::value
				|| mse::impl::is_instantiation_of<_TRAIterator, mse::TXScopeCSSSStrongRAIterator>::value
				|| mse::impl::is_instantiation_of<_TRAIterator, mse::TXScopeCSSSStrongRASectionConstIterator>::value
				|| mse::impl::is_instantiation_of<_TRAIterator, mse::TXScopeCSSSStrongRASectionIterator>::value
				, "");
		}

		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		template<class _TRAIterator2> friend TXScopeCSSSStrongRASectionConstIterator<_TRAIterator2> us::unsafe_make_xscope_csss_strong_ra_const_iterator(const _TRAIterator2& ra_iterator, typename TXScopeCSSSStrongRASectionConstIterator<_TRAIterator2>::size_type count, typename TXScopeCSSSStrongRASectionConstIterator<_TRAIterator2>::size_type index);
		template <typename _TRAIterator2> friend class mse::us::impl::TRandomAccessSectionBase;
		template <typename _TRAIterator2> friend class mse::us::impl::TRandomAccessConstSectionBase;
	};

	namespace us {
		/* A couple of unsafe functions for internal use. */
		template<class _TRAIterator>
		TXScopeCSSSStrongRASectionIterator<_TRAIterator> unsafe_make_xscope_csss_strong_ra_section_iterator(const _TRAIterator& ra_iterator, typename TXScopeCSSSStrongRASectionIterator<_TRAIterator>::size_type count, typename TXScopeCSSSStrongRASectionIterator<_TRAIterator>::size_type index/* = 0*/) {
			return TXScopeCSSSStrongRASectionIterator<_TRAIterator>(typename TXScopeCSSSStrongRASectionIterator<_TRAIterator>::unsafe_t(), ra_iterator, count, index);
		}
		template<class _TRAIterator>
		TXScopeCSSSStrongRASectionConstIterator<_TRAIterator> unsafe_make_xscope_csss_strong_ra_section_const_iterator(const _TRAIterator& ra_iterator, typename TXScopeCSSSStrongRASectionConstIterator<_TRAIterator>::size_type count, typename TXScopeCSSSStrongRASectionConstIterator<_TRAIterator>::size_type index/* = 0*/) {
			return TXScopeCSSSStrongRASectionConstIterator<_TRAIterator>(typename TXScopeCSSSStrongRASectionConstIterator<_TRAIterator>::unsafe_t(), ra_iterator, count, index);
		}
	}

	template <typename _TRAIterator>
	auto xscope_pointer(const TXScopeCSSSStrongRASectionIterator<_TRAIterator>& iter_cref) {
		return mse::us::unsafe_make_xscope_pointer_to(*iter_cref);
	}
	template <typename _TRAIterator>
	auto xscope_pointer(const TXScopeCSSSStrongRASectionConstIterator<_TRAIterator>& iter_cref) {
		return mse::us::unsafe_make_xscope_const_pointer_to(*iter_cref);
	}
	template <typename _TRAIterator>
	auto xscope_const_pointer(const TXScopeCSSSStrongRASectionIterator<_TRAIterator>& iter_cref) {
		return mse::us::unsafe_make_xscope_const_pointer_to(*iter_cref);
	}
	template <typename _TRAIterator>
	auto xscope_const_pointer(const TXScopeCSSSStrongRASectionConstIterator<_TRAIterator>& iter_cref) {
		return mse::us::unsafe_make_xscope_const_pointer_to(*iter_cref);
	}


	/* "Random access sections" are basically the library's safe version of std::span<>. */

	/* These are some free functions to obtain a subsection of a given section. */
	template <typename _TSection>
	auto make_xscope_subsection(const _TSection& xs_section, typename _TSection::size_type pos = 0, typename _TSection::size_type n = _TSection::npos)
		-> decltype(xs_section.xscope_subsection_pv(pos, n)) {
		return xs_section.xscope_subsection_pv(pos, n);
	}
	template <typename _TSection>
	auto make_subsection(const _TSection& section, typename _TSection::size_type pos = 0, typename _TSection::size_type n = _TSection::npos)
		-> decltype(section.subsection_pv(pos, n)) {
		return section.subsection_pv(pos, n);
	}
	/* Overloads for rsv::TReturnableFParam<>. */
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_subsection)

	template <typename _TSection, class... Args>
	auto make_subsection(const rsv::TReturnableFParam<_TSection>& section, Args... args) {
		const _TSection& section_base_ref = section;
		typedef decltype(make_subsection(section_base_ref, args...)) base_return_type;
		return rsv::TReturnableFParam<base_return_type>(make_subsection(section_base_ref, args...));
	}

	template <typename _TRAIterator> class TXScopeRandomAccessSection;
	template <typename _TRAIterator> class TXScopeRandomAccessConstSection;
	//template <typename _TRAIterator> class TXScopeCagedRandomAccessSectionToRValue;
	template <typename _TRAIterator> class TXScopeCagedRandomAccessConstSectionToRValue;
	template <typename _TRAIterator> class TRandomAccessSection;
	template <typename _TRAIterator> class TRandomAccessConstSection;
	namespace rsv {
		//template <typename _TRAIterator> class TXScopeRandomAccessSectionFParam;
		template <typename _TRAIterator> class TXScopeRandomAccessConstSectionFParam;
	}

	namespace us {
		namespace impl {
			class RandomAccessSectionTagBase {};
			class RandomAccessConstSectionTagBase {};
		}
	}

#define MSE_INHERITED_RANDOM_ACCESS_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class) \
	MSE_INHERITED_RANDOM_ACCESS_MEMBER_TYPE_DECLARATIONS(base_class); \
	static const size_t npos = size_t(-1);

	namespace impl {
		/* This struct contains a bunch of construction helper functions for us::impl::TRandomAccessConstSectionBase<>. They used to
		be member functions of us::impl::TRandomAccessConstSectionBase<>, but we had to pull them out because they are needed to
		forward declare (the single parameter overload of) the make_xscope_random_access_const_section() function.
		Specifically, they are needed to determine the return type of that function. The forward declaration needs to be
		available before the definition of us::impl::TRandomAccessConstSectionBase<> as its implementation uses the function. */
		struct ra_const_section_helpers {
			template <typename _TRALoneParam>
			/* "const auto&" doesn't work as a return type here with msvc2017. Compiler bug presumably. */
			static const decltype(std::declval<_TRALoneParam>().m_start_iter)& s_xscope_iter_from_lone_param3(std::false_type, const _TRALoneParam& ra_section) {
				/* The "random access section" is an lvalue, so we should return its iterator as an lvalue reference (in
				this case it can be a const reference) as some iterators have different behavior depending on whether they
				are lvalues or rvalues. */
				return ra_section.m_start_iter;
			}
			template <typename _TRALoneParam>
			static auto s_xscope_iter_from_lone_param3(std::true_type, _TRALoneParam&& ra_section) -> decltype(MSE_FWD(ra_section).m_start_iter) {
				/* The parameter is another "random access section". */
				return MSE_FWD(ra_section).m_start_iter;
			}
			template <typename _TRALoneParam>
			static auto s_xscope_iter_from_lone_param4(std::false_type, const _TRALoneParam& param) {
				return mse::make_xscope_const_iterator(param);
			}
			template <typename _TRALoneParam>
			static auto s_xscope_iter_from_lone_param4(std::true_type, const _TRALoneParam& string_view_param) {
				/* Apparently the lone parameter is a basic_string_view. */
				return string_view_param.cbegin();
			}
			template <typename _TRALoneParam>
			static auto s_xscope_iter_from_lone_param2(std::false_type, const _TRALoneParam& param) {
#ifdef MSE_HAS_CXX17
				return s_xscope_iter_from_lone_param4(typename mse::impl::is_instantiation_of<_TRALoneParam, std::basic_string_view>::type(), param);
#else /* MSE_HAS_CXX17 */
				return s_xscope_iter_from_lone_param4(std::false_type(), param);
#endif /* MSE_HAS_CXX17 */
			}
			template <typename _TRALoneParam>
			static auto s_xscope_iter_from_lone_param2(std::true_type, const _TRALoneParam& native_array) {
				/* Apparently the lone parameter is a native array. */
				return native_array;
			}
			template <typename _TRALoneParam>
			static auto s_xscope_iter_from_lone_param1(std::false_type, const _TRALoneParam& ptr) {
				return s_xscope_iter_from_lone_param2(typename mse::impl::IsNativeArray_msemsearray<_TRALoneParam>::type(), ptr);
			}
			template <typename _TRALoneParam>
			static const auto& s_xscope_iter_from_lone_param1(std::true_type, const _TRALoneParam& ra_section) {
				/* The parameter is another "random access section". */
				return ra_section.m_start_iter;
			}
			template <typename _TRALoneParam>
			static auto s_xscope_iter_from_lone_param1(std::true_type, _TRALoneParam&& ra_section)
				-> decltype(s_xscope_iter_from_lone_param3(typename std::is_rvalue_reference<decltype(ra_section)>::type(), MSE_FWD(ra_section))) {
				/* The parameter is another "random access section". */
				return s_xscope_iter_from_lone_param3(typename std::is_rvalue_reference<decltype(ra_section)>::type(), MSE_FWD(ra_section));
			}

			template <typename _TRALoneParam>
			static auto s_iter_from_lone_param2(std::false_type, const _TRALoneParam& param) {
				return mse::make_const_iterator(param);
			}
			template <typename _TRALoneParam>
			static auto s_iter_from_lone_param2(std::true_type, const _TRALoneParam& native_array) {
				/* Apparently the lone parameter is a native array. */
				return native_array;
			}
			template <typename _TRALoneParam>
			static auto s_iter_from_lone_param1(std::false_type, const _TRALoneParam& ptr) {
				return s_iter_from_lone_param2(typename mse::impl::IsNativeArray_msemsearray<_TRALoneParam>::type(), ptr);
			}
			template <typename _TRALoneParam>
			static const decltype(std::declval<_TRALoneParam>().m_start_iter)& s_iter_from_lone_param1(std::true_type, const _TRALoneParam& ra_section) {
				/* The parameter is another "random access section". */
				return ra_section.m_start_iter;
			}

			template <typename _TRALoneParam>
			static auto s_count_from_lone_param3(std::false_type, const _TRALoneParam& ptr) {
				/* The parameter doesn't seem to be a container with a "begin()" member function or a native array. Here we'll
				assume that it is a pointer to a container with a size() member function. If you get a compile error here, then
				construction from the given parameter type isn't supported. In particular, "char *" pointers to null terminated
				strings are not supported as a lone parameter. */
				return (*ptr).size();
			}
			template <typename _TRALoneParam>
			static auto s_count_from_lone_param3(std::true_type, const _TRALoneParam& ra_container) {
				/* The parameter seems to be a container with a "begin()" member function. We'll assume it has a "size()" member function too. */
				return ra_container.size();
			}
			template <typename _TRALoneParam>
			static auto s_count_from_lone_param2(std::false_type, const _TRALoneParam& param) {
				/* The parameter is not a "random access section". */
				return s_count_from_lone_param3(typename mse::impl::SupportsStdBegin_msemsearray<_TRALoneParam>::type(), param);
			}
			template <typename _TRALoneParam>
			static auto s_count_from_lone_param2(std::true_type, const _TRALoneParam& native_array) {
				/* The parameter seems to be a native array. */
				return mse::impl::native_array_size_msemsearray(native_array);
			}
			template <typename _TRALoneParam>
			static auto s_count_from_lone_param1(std::false_type, const _TRALoneParam& param) {
				/* The parameter doesn't seem to be a container with a "begin()" member function. */
				return s_count_from_lone_param2(typename mse::impl::IsNativeArray_msemsearray<_TRALoneParam>::type(), param);
			}
			template <typename _TRALoneParam>
			static auto s_count_from_lone_param1(std::true_type, const _TRALoneParam& ra_section) {
				/* The parameter is another "random access section". */
				return ra_section.m_count;
			}

			template <typename _TRALoneParam>
			static auto s_xscope_iter_from_lone_param(const _TRALoneParam& param) -> decltype(s_xscope_iter_from_lone_param1(mse::impl::conditional_t<
				std::is_base_of<mse::us::impl::RandomAccessConstSectionTagBase, mse::impl::remove_reference_t<_TRALoneParam> >::value || std::is_base_of<mse::us::impl::RandomAccessSectionTagBase, mse::impl::remove_reference_t<_TRALoneParam> >::value
				, std::true_type, std::false_type>(), param)) {

				typedef mse::impl::remove_reference_t<_TRALoneParam> TRALoneParamNR;
				/* TRALoneParamNR being either another TRandomAccess(Const)SectionBase<> or a pointer to "random access" container is
				supported. Different initialization implementations are required for each of the two cases. */
				return s_xscope_iter_from_lone_param1(mse::impl::conditional_t<
					std::is_base_of<mse::us::impl::RandomAccessConstSectionTagBase, TRALoneParamNR>::value || std::is_base_of<mse::us::impl::RandomAccessSectionTagBase, TRALoneParamNR>::value
					, std::true_type, std::false_type>(), param);
			}
			template <typename _TRALoneParam>
			static auto s_xscope_iter_from_lone_param(_TRALoneParam&& param) -> decltype(s_xscope_iter_from_lone_param1(mse::impl::conditional_t<
				std::is_base_of<mse::us::impl::RandomAccessConstSectionTagBase, mse::impl::remove_reference_t<_TRALoneParam> >::value || std::is_base_of<mse::us::impl::RandomAccessSectionTagBase, mse::impl::remove_reference_t<_TRALoneParam> >::value
				, std::true_type, std::false_type>(), MSE_FWD(param))) {

				typedef mse::impl::remove_reference_t<_TRALoneParam> TRALoneParamNR;
				/* TRALoneParamNR being either another TRandomAccess(Const)SectionBase<> or a pointer to "random access" container is
				supported. Different initialization implementations are required for each of the two cases. */
				return s_xscope_iter_from_lone_param1(mse::impl::conditional_t<
					std::is_base_of<mse::us::impl::RandomAccessConstSectionTagBase, TRALoneParamNR>::value || std::is_base_of<mse::us::impl::RandomAccessSectionTagBase, TRALoneParamNR>::value
					, std::true_type, std::false_type>(), MSE_FWD(param));
			}
			template <typename _TRALoneParam>
			static auto s_iter_from_lone_param(const _TRALoneParam& param) {
				typedef mse::impl::remove_reference_t<_TRALoneParam> TRALoneParamNR;
				/* TRALoneParamNR being either another TRandomAccess(Const)SectionBase<> or a pointer to "random access" container is
				supported. Different initialization implementations are required for each of the two cases. */
				return s_iter_from_lone_param1(mse::impl::conditional_t<
					std::is_base_of<mse::us::impl::RandomAccessConstSectionTagBase, TRALoneParamNR>::value || std::is_base_of<mse::us::impl::RandomAccessSectionTagBase, TRALoneParamNR>::value
					, std::true_type, std::false_type>(), param);
			}
			template <typename _TRALoneParam>
			static auto s_count_from_lone_param(const _TRALoneParam& param) {
				typedef mse::impl::remove_reference_t<_TRALoneParam> TRALoneParamNR;
				return /*us::impl::TRandomAccessSectionBase<_TRAIterator>::*/s_count_from_lone_param1(mse::impl::conditional_t<
					std::is_base_of<mse::us::impl::RandomAccessConstSectionTagBase, TRALoneParamNR>::value || std::is_base_of<mse::us::impl::RandomAccessSectionTagBase, TRALoneParamNR>::value
					, std::true_type, std::false_type>(), param);
			}
		};

		struct ra_section_helpers {
			/* construction helper functions */
			template <typename _TRALoneParam>
			/* "const auto&" doesn't work as a return type here with msvc2017. Compiler bug presumably. */
			static const decltype(std::declval<_TRALoneParam>().m_start_iter)& s_xscope_iter_from_lone_param3(std::false_type, const _TRALoneParam& ra_section) {
				/* The "random access section" is an lvalue, so we should return its iterator as an lvalue reference (in
				this case it can be a const reference) as some iterators have different behavior depending on whether they
				are lvalues or rvalues. */
				return ra_section.m_start_iter;
			}
			template <typename _TRALoneParam>
			static auto s_xscope_iter_from_lone_param3(std::true_type, _TRALoneParam&& ra_section) -> decltype(MSE_FWD(ra_section).m_start_iter) {
				/* The parameter is another "random access section". */
				return MSE_FWD(ra_section).m_start_iter;
			}
			template <typename _TRALoneParam>
			static auto s_xscope_iter_from_lone_param2(std::false_type, const _TRALoneParam& param) {
				return mse::make_xscope_iterator(param);
			}
			template <typename _TRALoneParam>
			static auto s_xscope_iter_from_lone_param2(std::true_type, const _TRALoneParam& native_array) {
				/* Apparently the lone parameter is a native array. */
				return native_array;
			}
			template <typename _TRALoneParam>
			static auto s_xscope_iter_from_lone_param1(std::false_type, const _TRALoneParam& ptr) {
				return s_xscope_iter_from_lone_param2(typename mse::impl::IsNativeArray_msemsearray<_TRALoneParam>::type(), ptr);
			}
			template <typename _TRALoneParam>
			static const decltype(std::declval<_TRALoneParam>().m_start_iter)& s_xscope_iter_from_lone_param1(std::true_type, const _TRALoneParam& ra_section) {
				/* The parameter is another "random access section". */
				return ra_section.m_start_iter;
			}
			template <typename _TRALoneParam>
			static auto s_xscope_iter_from_lone_param1(std::true_type, _TRALoneParam&& ra_section)
				-> decltype(s_xscope_iter_from_lone_param3(typename std::is_rvalue_reference<decltype(ra_section)>::type(), MSE_FWD(ra_section))) {
				/* The parameter is another "random access section". */
				return s_xscope_iter_from_lone_param3(typename std::is_rvalue_reference<decltype(ra_section)>::type(), MSE_FWD(ra_section));
			}

			template <typename _TRALoneParam>
			static auto s_iter_from_lone_param2(std::false_type, const _TRALoneParam& param) {
				return mse::make_iterator(param);
			}
			template <typename _TRALoneParam>
			static auto s_iter_from_lone_param2(std::true_type, const _TRALoneParam& native_array) {
				/* Apparently the lone parameter is a native array. */
				return native_array;
			}
			template <typename _TRALoneParam>
			static auto s_iter_from_lone_param1(std::false_type, const _TRALoneParam& ptr) {
				return s_iter_from_lone_param2(typename mse::impl::IsNativeArray_msemsearray<_TRALoneParam>::type(), ptr);
			}
			template <typename _TRALoneParam>
			static auto s_iter_from_lone_param1(std::true_type, const _TRALoneParam& ra_section) {
				/* The parameter is another "random access section". */
				return ra_section.m_start_iter;
			}

			template <typename _TRALoneParam>
			static auto s_xscope_iter_from_lone_param(const _TRALoneParam& param)
				-> decltype(s_xscope_iter_from_lone_param1(typename std::is_base_of<mse::us::impl::RandomAccessSectionTagBase, mse::impl::remove_reference_t<_TRALoneParam> >::type(), param)) {

				typedef mse::impl::remove_reference_t<_TRALoneParam> TRALoneParamNR;
				/* TRALoneParamNR being either another TRandomAccessSectionBase<> or a pointer to "random access" container is
				supported. Different initialization implementations are required for each of the two cases. */
				return s_xscope_iter_from_lone_param1(typename std::is_base_of<mse::us::impl::RandomAccessSectionTagBase, TRALoneParamNR>::type(), param);
			}
			template <typename _TRALoneParam>
			static auto s_xscope_iter_from_lone_param(_TRALoneParam&& param)
				-> decltype(s_xscope_iter_from_lone_param1(typename std::is_base_of<mse::us::impl::RandomAccessSectionTagBase, mse::impl::remove_reference_t<_TRALoneParam> >::type(), MSE_FWD(param))) {

				typedef mse::impl::remove_reference_t<_TRALoneParam> TRALoneParamNR;
				/* TRALoneParamNR being either another TRandomAccessSectionBase<> or a pointer to "random access" container is
				supported. Different initialization implementations are required for each of the two cases. */
				return s_xscope_iter_from_lone_param1(typename std::is_base_of<mse::us::impl::RandomAccessSectionTagBase, TRALoneParamNR>::type(), MSE_FWD(param));
			}
			template <typename _TRALoneParam>
			static auto s_iter_from_lone_param(const _TRALoneParam& param) {
				typedef mse::impl::remove_reference_t<_TRALoneParam> TRALoneParamNR;
				/* TRALoneParamNR being either another us::impl::TRandomAccessSectionBase<>, a "random access" container, or a pointer to "random
				access" container is supported. Different initialization implementations are required for each case. */
				return s_iter_from_lone_param1(typename std::is_base_of<mse::us::impl::RandomAccessSectionTagBase, TRALoneParamNR>::type(), param);
			}
			template <typename _TRALoneParam>
			static auto s_count_from_lone_param(const _TRALoneParam& param) {
				typedef mse::impl::remove_reference_t<_TRALoneParam> TRALoneParamNR;
				return mse::impl::ra_const_section_helpers::s_count_from_lone_param1(typename std::is_base_of<mse::us::impl::RandomAccessSectionTagBase, TRALoneParamNR>::type(), param);
			}
		};

		namespace ra_section {
			template <typename _Ty> using mkxsracsh1_TRAIterator = mse::impl::remove_reference_t<decltype(mse::impl::ra_const_section_helpers::s_xscope_iter_from_lone_param(std::declval<mse::TXScopeFixedConstPointer<_Ty> >()))>;
			template <typename _Ty> using mkxsracsh1_ReturnType = mse::TXScopeCagedRandomAccessConstSectionToRValue<mkxsracsh1_TRAIterator<_Ty> >;

			template <typename _Ty>
			static auto make_xscope_random_access_const_section_helper1(std::true_type, const TXScopeCagedItemFixedConstPointerToRValue<_Ty>& param)
				-> mkxsracsh1_ReturnType<_Ty> {
				mse::TXScopeFixedConstPointer<_Ty> adj_param = mse::rsv::TXScopeFixedConstPointerFParam<_Ty>(param);
				typedef mse::impl::remove_reference_t<decltype(mse::impl::ra_const_section_helpers::s_xscope_iter_from_lone_param(adj_param))> _TRAIterator;
				mse::TXScopeRandomAccessConstSection<_TRAIterator> ra_section(adj_param);
				return mse::TXScopeCagedRandomAccessConstSectionToRValue<_TRAIterator>(ra_section);
			}
			template <typename _Ty>
			static auto make_xscope_random_access_const_section_helper1(std::true_type, TXScopeCagedItemFixedConstPointerToRValue<_Ty>&& param)
				-> mkxsracsh1_ReturnType<_Ty> {
				mse::TXScopeFixedConstPointer<_Ty> adj_param = mse::rsv::TXScopeFixedConstPointerFParam<_Ty>(MSE_FWD(param));
				typedef mse::impl::remove_reference_t<decltype(mse::impl::ra_const_section_helpers::s_xscope_iter_from_lone_param(adj_param))> _TRAIterator;
				mse::TXScopeRandomAccessConstSection<_TRAIterator> ra_section(adj_param);
				return mse::TXScopeCagedRandomAccessConstSectionToRValue<_TRAIterator>(ra_section);
			}
			template <typename _TRALoneParam>
			static auto make_xscope_random_access_const_section_helper1(std::false_type, const _TRALoneParam& param) {
				typedef mse::impl::remove_reference_t<decltype(mse::impl::ra_const_section_helpers::s_xscope_iter_from_lone_param(param))> _TRAIterator;
				return TXScopeRandomAccessConstSection<_TRAIterator>(param);
			}
		}
	}
	/* We're forward declaring this function here because it is used by the us::impl::TRandomAccessConstSectionBase<> class that follows.
	Note that this function has other overloads and bretheren that do not need to be forward declared. */
	template <typename _TRALoneParam> auto make_xscope_random_access_const_section(const _TRALoneParam& param) -> decltype(mse::impl::ra_section::make_xscope_random_access_const_section_helper1(
		typename mse::impl::is_instantiation_of<_TRALoneParam, mse::TXScopeCagedItemFixedConstPointerToRValue>::type(), param));

	namespace us {
		namespace impl {

			template <typename _TRAIterator> class TRandomAccessSectionBase;

			template <typename _TRAIterator>
			class TRandomAccessConstSectionBase : public mse::us::impl::RandomAccessConstSectionTagBase
				, MSE_INHERIT_COMMON_XSCOPE_ITERATOR_TAG_BASE_SET_FROM(_TRAIterator, TRandomAccessConstSectionBase<_TRAIterator>)
			{
			public:
				typedef _TRAIterator iterator_type;
				typedef _TRAIterator ra_iterator_type;
				MSE_INHERITED_RANDOM_ACCESS_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(
					mse::impl::random_access_const_iterator_base<mse::impl::remove_reference_t<decltype(std::declval<_TRAIterator>()[0])> >);

				//TRandomAccessConstSectionBase(const TRandomAccessConstSectionBase& src) = default;
				TRandomAccessConstSectionBase(const TRandomAccessConstSectionBase& src) : m_count(src.m_count), m_start_iter(src.m_start_iter) {}
				TRandomAccessConstSectionBase(const TRandomAccessSectionBase<_TRAIterator>& src) : m_count(src.m_count), m_start_iter(src.m_start_iter) {}
				TRandomAccessConstSectionBase(const _TRAIterator& start_iter, size_type count) : m_count(count), m_start_iter(start_iter) {}

				template <typename _TRALoneParam>
				TRandomAccessConstSectionBase(const _TRALoneParam& param)
					/* _TRALoneParam being either another TRandomAccess(Const)SectionBase<> or a pointer to "random access" container is
					supported. Different initialization implementations are required for each of the two cases. */
					: m_count(s_count_from_lone_param(param))
					, m_start_iter(s_xscope_iter_from_lone_param(param)) {}
				template <typename _TRALoneParam>
				TRandomAccessConstSectionBase(_TRALoneParam&& param)
					: m_count(s_count_from_lone_param(param))
					, m_start_iter(s_xscope_iter_from_lone_param(MSE_FWD(param))) {}

				const_reference operator[](size_type _P) const {
					if (m_count <= _P) { MSE_THROW(msearray_range_error("out of bounds index - reference operator[](size_type _P) - TRandomAccessConstSectionBase")); }
					return m_start_iter[difference_type(mse::msear_as_a_size_t(_P))];
				}
				const_reference at(size_type _P) const {
					return (*this)[_P];
				}
				const_reference front() const {
					if (0 == (*this).size()) { MSE_THROW(msearray_range_error("front() on empty - const_reference front() const - TRandomAccessConstSectionBase")); }
					return (*this)[0];
				}
				const_reference back() const {
					if (0 == (*this).size()) { MSE_THROW(msearray_range_error("back() on empty - const_reference back() const - TRandomAccessConstSectionBase")); }
					return (*this)[(*this).size() - 1];
				}
				size_type size() const _NOEXCEPT {
					return m_count;
				}
				size_type length() const _NOEXCEPT {
					return (*this).size();
				}
				size_type max_size() const _NOEXCEPT {	// return maximum possible length of sequence
					return static_cast<size_type>((std::numeric_limits<difference_type>::max)());
				}
				bool empty() const _NOEXCEPT {
					return (0 == (*this).size());
				}

				template<typename _TRAParam>
				bool equal(const _TRAParam& ra_param) const {
					auto sv = mse::make_xscope_random_access_const_section(mse::rsv::as_an_fparam(ra_param));
					if (size() != sv.size()) {
						return false;
					}
					//return std::equal(xscope_cbegin(), xscope_cend(), sv.xscope_cbegin());
					auto first1 = xscope_cbegin();
					auto last1 = xscope_cend();
					auto first2 = sv.xscope_cbegin();
					while (first1 != last1) {
						if (!(*first1 == *first2)) {
							return false;
						}
						++first1; ++first2;
					}
					return true;
				}
				template<typename _TRAParam>
				bool equal(size_type pos1, size_type n1, const _TRAParam& ra_param) const {
					auto sv = mse::make_xscope_random_access_const_section(mse::rsv::as_an_fparam(ra_param));
					return subsection(pos1, n1).equal(sv);
				}
				template<typename _TRAParam>
				bool equal(size_type pos1, size_type n1, const _TRAParam& ra_param, size_type pos2, size_type n2) const {
					auto sv = mse::make_xscope_random_access_const_section(mse::rsv::as_an_fparam(ra_param));
					return subsection(pos1, n1).equal(sv.subsection(pos2, n2));
				}
				template <typename _TRAIterator2>
				bool equal(size_type pos1, size_type n1, const _TRAIterator2& s, size_type n2) const {
					auto sv = us::impl::TRandomAccessConstSectionBase<_TRAIterator2>(mse::rsv::as_an_fparam(s), n2);
					return subsection(pos1, n1).equal(sv);
				}
				template<typename _TRAParam>
				bool operator==(const _TRAParam& ra_param) const {
					return equal(ra_param);
				}
				template<typename _TRAParam>
				bool operator!=(const _TRAParam& ra_param) const {
					return !((*this) == ra_param);
				}

				template<typename _TRAParam>
				bool lexicographical_compare(const _TRAParam& ra_param) const {
					auto sv = mse::make_xscope_random_access_const_section(mse::rsv::as_an_fparam(ra_param));
					return std::lexicographical_compare(xscope_cbegin(), xscope_cend(), sv.xscope_cbegin(), sv.xscope_cend());
				}
				template<typename _TRAParam>
				bool lexicographical_compare(size_type pos1, size_type n1, const _TRAParam& ra_param) const {
					auto sv = mse::make_xscope_random_access_const_section(mse::rsv::as_an_fparam(ra_param));
					return subsection(pos1, n1).lexicographical_compare(sv);
				}
				template<typename _TRAParam>
				bool lexicographical_compare(size_type pos1, size_type n1, const _TRAParam& ra_param, size_type pos2, size_type n2) const {
					auto sv = mse::make_xscope_random_access_const_section(mse::rsv::as_an_fparam(ra_param));
					return subsection(pos1, n1).lexicographical_compare(sv.subsection(pos2, n2));
				}
				template <typename _TRAIterator2>
				bool lexicographical_compare(size_type pos1, size_type n1, const _TRAIterator2& s, size_type n2) const {
					auto sv = mse::make_xscope_random_access_const_section(mse::rsv::as_an_fparam(s), n2);
					return subsection(pos1, n1).lexicographical_compare(sv);
				}
				template<typename _TRAParam>
				bool operator<(const _TRAParam& ra_param) const {
					auto sv = mse::make_xscope_random_access_const_section(mse::rsv::as_an_fparam(ra_param));
					return lexicographical_compare(sv);
				}
				template<typename _TRAParam>
				bool operator>(const _TRAParam& ra_param) const {
					auto sv = mse::make_xscope_random_access_const_section(mse::rsv::as_an_fparam(ra_param));
					return sv.lexicographical_compare(*this);
				}
				template<typename _TRAParam>
				bool operator<=(const _TRAParam& ra_param) const { return !((*this) > ra_param); }
				template<typename _TRAParam>
				bool operator>=(const _TRAParam& ra_param) const { return !((*this) < ra_param); }

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
						(*target_iter) = (*this)[i];
						++target_iter;
					}
					return n;
				}

				void remove_prefix(size_type n) /*_NOEXCEPT*/ {
					if (n > (*this).size()) { MSE_THROW(msearray_range_error("out of bounds index - void remove_prefix() - TRandomAccessConstSectionBase")); }
					m_count -= n;
					m_start_iter += n;
				}
				void remove_suffix(size_type n) /*_NOEXCEPT*/ {
					if (n > (*this).size()) { MSE_THROW(msearray_range_error("out of bounds index - void remove_suffix() - TRandomAccessConstSectionBase")); }
					m_count -= n;
				}

				template<typename _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_base_of<TRandomAccessConstSectionBase, _Ty2>::value && mse::impl::HasOrInheritsAssignmentOperator_msemsearray<_TRAIterator>::value> MSE_IMPL_EIS >
				void swap(_Ty2& _Other) _NOEXCEPT_OP(_NOEXCEPT_OP(TRandomAccessConstSectionBase(_Other)) && _NOEXCEPT_OP(std::declval<_TRAIterator>().operator=(std::declval<_TRAIterator>()))) {
					TRandomAccessConstSectionBase& _Other2 = _Other;
					std::swap((*this), _Other2);
				}

				size_type find(const us::impl::TRandomAccessConstSectionBase<_TRAIterator>& s, size_type pos = 0) const _NOEXCEPT {
					if ((1 > s.size()) || (1 > (*this).size())) {
						return npos;
					}
					auto cit = std::search((*this).xscope_cbegin(), (*this).xscope_cend(), s.xscope_cbegin(), s.xscope_cend());
					if ((*this).xscope_cend() == cit) {
						return npos;
					}
					return (cit - (*this).xscope_cbegin());
				}
				size_type find(const value_type& c, size_type pos = 0) const _NOEXCEPT {
					if ((*this).size() <= 1) {
						return npos;
					}
					auto cit1 = std::find(xscope_cbegin(), xscope_cend(), c);
					return (xscope_cend() == cit1) ? npos : (cit1 - xscope_cbegin());
				}
				size_type rfind(const us::impl::TRandomAccessConstSectionBase<_TRAIterator>& s, size_type pos = npos) const _NOEXCEPT {
					if ((1 > s.size()) || (1 > (*this).size())) {
						return npos;
					}
					auto cit = std::find_end((*this).xscope_cbegin(), (*this).xscope_cend(), s.xscope_cbegin(), s.xscope_cend());
					if ((*this).xscope_cend() == cit) {
						return npos;
					}
					return (cit - (*this).xscope_cbegin());
				}
				size_type rfind(const value_type& c, size_type pos = npos) const _NOEXCEPT {
					if ((*this).size() <= 1) {
						return npos;
					}
					if (pos < (*this).size()) {
						++pos;
					}
					else {
						pos = (*this).size();
					}
					for (size_type i = pos; 0 != i;) {
						--i;
						if ((*this)[i] == c) {
							return i;
						}
					}
					return npos;
				}
				size_type find_first_of(const us::impl::TRandomAccessConstSectionBase<_TRAIterator>& s, size_type pos = 0) const _NOEXCEPT {
					if ((1 > s.size()) || (1 > (*this).size())) {
						return npos;
					}
					auto cit = std::find_first_of((*this).xscope_cbegin(), (*this).xscope_cend(), s.xscope_cbegin(), s.xscope_cend());
					if ((*this).xscope_cend() == cit) {
						return npos;
					}
					return (cit - (*this).xscope_cbegin());
				}
				size_type find_first_of(const value_type& c, size_type pos = 0) const _NOEXCEPT {
					return find(c, pos);
				}
				size_type find_last_of(const us::impl::TRandomAccessConstSectionBase<_TRAIterator>& s, size_type pos = npos) const _NOEXCEPT {
					if ((1 > s.size()) || (1 > (*this).size())) {
						return npos;
					}
					if (pos < (*this).size()) {
						++pos;
					}
					else {
						pos = (*this).size();
					}
					for (size_type i = pos; 0 != i;) {
						--i;
						const auto& domain_element_cref((*this)[i]);
						for (const auto& search_element_cref : s) {
							if (domain_element_cref == search_element_cref) {
								return i;
							}
						}
					}
					return npos;
				}
				size_type find_last_of(const value_type& c, size_type pos = npos) const _NOEXCEPT {
					return rfind(c, pos);
				}
				size_type find_first_not_of(const us::impl::TRandomAccessConstSectionBase<_TRAIterator>& s, size_type pos = 0) const _NOEXCEPT {
					if ((1 > s.size()) || (1 > (*this).size())) {
						return npos;
					}
					auto cit = std::find_if_not((*this).xscope_cbegin(), (*this).xscope_cend(), [&s](const value_type& domain_element_cref) {
						for (const auto& search_element_cref : s) {
							if (domain_element_cref == search_element_cref) {
								return true;
							}
						}
						return false;
					}
					);
					if ((*this).xscope_cend() == cit) {
						return npos;
					}
					return (cit - (*this).xscope_cbegin());
				}
				size_type find_first_not_of(const value_type& c, size_type pos = 0) const _NOEXCEPT {
					if (1 > (*this).size()) {
						return npos;
					}
					auto cit = std::find_if_not((*this).xscope_cbegin(), (*this).xscope_cend(), [&c](const value_type& domain_element_cref) {
						if (domain_element_cref == c) {
							return true;
						}
						return false;
					}
					);
					if ((*this).xscope_cend() == cit) {
						return npos;
					}
					return (cit - (*this).xscope_cbegin());
				}
				size_type find_last_not_of(const us::impl::TRandomAccessConstSectionBase<_TRAIterator>& s, size_type pos = npos) const _NOEXCEPT {
					if ((1 > s.size()) || (1 > (*this).size())) {
						return npos;
					}
					if (pos < (*this).size()) {
						++pos;
					}
					else {
						pos = (*this).size();
					}
					for (size_type i = pos; 0 != i;) {
						--i;
						const auto& domain_element_cref((*this)[i]);
						for (const auto& search_element_cref : s) {
							if (domain_element_cref != search_element_cref) {
								return i;
							}
						}
					}
					return npos;
				}
				size_type find_last_not_of(const value_type& c, size_type pos = npos) const _NOEXCEPT {
					if (1 > (*this).size()) {
						return npos;
					}
					if (pos < (*this).size()) {
						++pos;
					}
					else {
						pos = (*this).size();
					}
					for (size_type i = pos; 0 != i;) {
						--i;
						const auto& domain_element_cref((*this)[i]);
						if (domain_element_cref != c) {
							return i;
						}
					}
					return npos;
				}

				bool starts_with(const us::impl::TRandomAccessConstSectionBase<_TRAIterator>& s) const _NOEXCEPT {
					return (size() >= s.size()) && equal(0, s.size(), s);
				}
				bool starts_with(const value_type& c) const _NOEXCEPT {
					return (!empty()) && (front() == c);
				}
				bool ends_with(const us::impl::TRandomAccessConstSectionBase<_TRAIterator>& s) const _NOEXCEPT {
					return (size() >= s.size()) && equal(size() - s.size(), npos, s);
				}
				bool ends_with(const value_type& c) const _NOEXCEPT {
					return (!empty()) && (back() == c);
				}

				typedef mse::impl::conditional_t<mse::impl::is_instantiation_of<_TRAIterator, mse::TXScopeCSSSStrongRAConstIterator>::value
					|| mse::impl::is_instantiation_of<_TRAIterator, mse::TXScopeCSSSStrongRAIterator>::value
					|| mse::impl::is_instantiation_of<_TRAIterator, mse::TXScopeCSSSStrongRASectionConstIterator>::value
					|| mse::impl::is_instantiation_of<_TRAIterator, mse::TXScopeCSSSStrongRASectionIterator>::value
					, TXScopeCSSSStrongRASectionConstIterator<_TRAIterator>, TXScopeRASectionConstIterator<_TRAIterator> > xscope_const_iterator_base;
				class xscope_const_iterator : public xscope_const_iterator_base {
				public:
					typedef xscope_const_iterator_base base_class;
					MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);
					xscope_const_iterator(const xscope_const_iterator&) = default;
					xscope_const_iterator(xscope_const_iterator&&) = default;

					//MSE_USING(xscope_const_iterator, base_class);
					template<class _TRASectionPointer, MSE_IMPL_EIP mse::impl::enable_if_t<!std::is_base_of<base_class, _TRASectionPointer>::value> MSE_IMPL_EIS >
					xscope_const_iterator(const _TRASectionPointer& ptr, size_type index = 0) : base_class((*ptr).m_start_iter, (*ptr).m_count, index) {}
					MSE_INHERIT_ITERATOR_ARITHMETIC_OPERATORS_FROM(base_class, xscope_const_iterator);
				private:
					xscope_const_iterator(const _TRAIterator& iter, size_type count, size_type index) : base_class(iter, count, index) {}
					friend class TRandomAccessConstSectionBase;
				};
				typedef xscope_const_iterator xscope_iterator;
				xscope_const_iterator xscope_begin() const { return (*this).xscope_cbegin(); }
				xscope_const_iterator xscope_cbegin() const { return xscope_const_iterator((*this).m_start_iter, (*this).m_count, 0); }
				xscope_const_iterator xscope_end() const { return (*this).xscope_cend(); }
				xscope_const_iterator xscope_cend() const {
					auto retval(xscope_const_iterator((*this).m_start_iter, (*this).m_count, 0));
					retval += mse::msear_as_a_size_t((*this).m_count);
					return retval;
				}

				template <typename _TRALoneParam>
				static auto s_xscope_iter_from_lone_param(const _TRALoneParam& param) -> decltype(mse::impl::ra_const_section_helpers::s_xscope_iter_from_lone_param(param)) {
					return mse::impl::ra_const_section_helpers::s_xscope_iter_from_lone_param(param);
				}
				template <typename _TRALoneParam>
				static auto s_xscope_iter_from_lone_param(_TRALoneParam&& param) -> decltype(mse::impl::ra_const_section_helpers::s_xscope_iter_from_lone_param(MSE_FWD(param))) {
					return mse::impl::ra_const_section_helpers::s_xscope_iter_from_lone_param(MSE_FWD(param));
				}
				template <typename _TRALoneParam>
				static auto s_iter_from_lone_param(const _TRALoneParam& param) {
					return mse::impl::ra_const_section_helpers::s_iter_from_lone_param(param);
				}
				template <typename _TRALoneParam>
				static auto s_count_from_lone_param(const _TRALoneParam& param) {
					return mse::impl::ra_const_section_helpers::s_count_from_lone_param(param);
				}

			protected:
				TRandomAccessConstSectionBase subsection(size_type pos = 0, size_type n = npos) const {
					if (pos > (*this).size()) { MSE_THROW(msearray_range_error("out of bounds index - TRandomAccessConstSectionBase subsection() const - TRandomAccessConstSectionBase")); }
					return TRandomAccessConstSectionBase((*this).m_start_iter + mse::msear_as_a_size_t(pos), std::min(mse::msear_as_a_size_t(n), mse::msear_as_a_size_t((*this).size()) - mse::msear_as_a_size_t(pos)));
				}

			private:
				MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

				size_type m_count = 0;
				_TRAIterator m_start_iter;

				friend class TXScopeRandomAccessConstSection<_TRAIterator>;
				friend class TRandomAccessConstSection<_TRAIterator>;
				template<typename _TRAIterator1> friend class TRandomAccessConstSectionBase;
				/* We're friending us::impl::TRandomAccessSectionBase<> because at the moment we're using its "constructor
				helper" (static) member functions, instead of duplicating them here, and those functions will need access to
				the private data members of this class. */
				template<typename _TRAIterator1> friend class TRandomAccessSectionBase;
				friend struct mse::impl::ra_const_section_helpers;
				friend struct mse::impl::ra_section_helpers;
			};
		}
	}

	template <typename _TRAIterator>
	class TXScopeRandomAccessConstSection : public us::impl::TRandomAccessConstSectionBase<_TRAIterator>, public mse::us::impl::XScopeContainsNonOwningScopeReferenceTagBase, public mse::us::impl::StrongPointerAsyncNotShareableAndNotPassableTagBase {
	public:
		typedef us::impl::TRandomAccessConstSectionBase<_TRAIterator> base_class;
		typedef _TRAIterator iterator_type;
		MSE_INHERITED_RANDOM_ACCESS_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);

		//TXScopeRandomAccessConstSection(const _TRAIterator& start_iter, size_type count) : base_class(start_iter, count) {}
		//TXScopeRandomAccessConstSection(const TXScopeRandomAccessConstSection& src) = default;
		//template<class _Ty2 = _TRAIterator, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _TRAIterator>::value) && (mse::impl::is_potentially_not_xscope<_TRAIterator>::value)> MSE_IMPL_EIS >
		//TXScopeRandomAccessConstSection(const us::impl::TRandomAccessConstSectionBase<_TRAIterator>& src) : base_class(src) {}
		//template<class _Ty2 = _TRAIterator, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _TRAIterator>::value) && (mse::impl::is_potentially_not_xscope<_TRAIterator>::value)> MSE_IMPL_EIS >
		//TXScopeRandomAccessConstSection(const us::impl::TRandomAccessSectionBase<_TRAIterator>& src) : base_class(src) {}

		MSE_USING(TXScopeRandomAccessConstSection, base_class);

		/* use the make_xscope_subsection() free function instead */
		MSE_DEPRECATED TXScopeRandomAccessConstSection xscope_subsection(size_type pos = 0, size_type n = npos) const {
			return xscope_subsection_pv(pos, n);
		}
		/* prefer the make_subsection() free function instead */
		auto subsection(size_type pos = 0, size_type n = npos) const {
			return subsection_pv(pos, n);
		}
		auto first(size_type count) const { return subsection_pv(0, count); }
		auto last(size_type count) const { return subsection_pv(std::max(difference_type(mse::msear_as_a_size_t((*this).size())) - difference_type(mse::msear_as_a_size_t(count)), 0), count); }

		//typedef typename base_class::xscope_iterator xscope_iterator;
		typedef typename base_class::xscope_const_iterator xscope_const_iterator;

		/* These are here because some standard algorithms require them. Prefer the "xscope_" prefixed versions to
		acknowledge that scope iterators are returned. */
		typedef xscope_const_iterator const_iterator;
		auto begin() const { return (*this).xscope_cbegin(); }
		auto cbegin() const { return (*this).xscope_cbegin(); }
		auto end() const { return (*this).xscope_cend(); }
		auto cend() const { return (*this).xscope_cend(); }

	private:

		TXScopeRandomAccessConstSection xscope_subsection_pv(size_type pos = 0, size_type n = npos) const {
			if (pos > (*this).size()) { MSE_THROW(msearray_range_error("out of bounds index - TXScopeRandomAccessConstSection xscope_subsection() const - TXScopeRandomAccessConstSection")); }
			return TXScopeRandomAccessConstSection((*this).m_start_iter + mse::msear_as_a_size_t(pos), std::min(mse::msear_as_a_size_t(n), mse::msear_as_a_size_t((*this).size()) - mse::msear_as_a_size_t(pos)));
		}
		typedef mse::impl::conditional_t<mse::impl::is_xscope<_TRAIterator>::value, TXScopeRandomAccessConstSection, TRandomAccessConstSection<_TRAIterator> > subsection_t;
		subsection_t subsection_pv(size_type pos = 0, size_type n = npos) const {
			if (pos > (*this).size()) { MSE_THROW(msearray_range_error("out of bounds index - TRandomAccessConstSection<_TRAIterator> subsection() const - TXScopeRandomAccessConstSection")); }
			return subsection_t((*this).m_start_iter + mse::msear_as_a_size_t(pos), std::min(mse::msear_as_a_size_t(n), mse::msear_as_a_size_t((*this).size()) - mse::msear_as_a_size_t(pos)));
		}

		TXScopeRandomAccessConstSection<_TRAIterator>& operator=(const TXScopeRandomAccessConstSection<_TRAIterator>& _Right_cref) = delete;
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		template <typename _TSection>
		friend auto make_xscope_subsection(const _TSection& xs_section, typename _TSection::size_type pos/* = 0*/, typename _TSection::size_type n/* = _TSection::npos*/) -> decltype(xs_section.xscope_subsection_pv(pos, n));
		template <typename _TSection>
		friend auto make_subsection(const _TSection& section, typename _TSection::size_type pos/* = 0*/, typename _TSection::size_type n/* = _TSection::npos*/)
			-> decltype(section.subsection_pv(pos, n));
	};

	template <typename _TRAIterator>
	class TRandomAccessConstSection : public us::impl::TRandomAccessConstSectionBase<_TRAIterator> {
	public:
		typedef us::impl::TRandomAccessConstSectionBase<_TRAIterator> base_class;
		typedef _TRAIterator iterator_type;
		MSE_INHERITED_RANDOM_ACCESS_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);

		TRandomAccessConstSection(const TRandomAccessConstSection& src) : base_class(static_cast<const base_class&>(src)) {}
		TRandomAccessConstSection(const TRandomAccessSection<_TRAIterator>& src) : base_class(static_cast<const base_class&>(src)) {}
		TRandomAccessConstSection(const _TRAIterator& start_iter, size_type count) : base_class(start_iter, count) {}
		template <typename _TRALoneParam>
		TRandomAccessConstSection(const _TRALoneParam& param) : base_class(base_class::s_iter_from_lone_param(param), base_class::s_count_from_lone_param(param)) {}
		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TRandomAccessConstSection() {
			mse::impl::T_valid_if_not_an_xscope_type<_TRAIterator>();
		}

		/* use the make_xscope_subsection() free function instead */
		MSE_DEPRECATED TXScopeRandomAccessConstSection<_TRAIterator> xscope_subsection(size_type pos = 0, size_type n = npos) const {
			return xscope_subsection_pv(pos, n);
		}
		/* prefer the make_subsection() free function instead */
		auto subsection(size_type pos = 0, size_type n = npos) const {
			return subsection_pv(pos, n);
		}
		auto first(size_type count) const { return subsection_pv(0, count); }
		auto last(size_type count) const { return subsection_pv(std::max(difference_type(mse::msear_as_a_size_t((*this).size())) - difference_type(mse::msear_as_a_size_t(count)), 0), count); }

		typedef TRASectionConstIterator<_TRAIterator> const_iterator_base;
		class const_iterator : public const_iterator_base {
		public:
			typedef const_iterator_base base_class;
			MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);
			const_iterator(const const_iterator&) = default;
			const_iterator(const_iterator&&) = default;

			//MSE_USING(const_iterator, base_class);
			template<class _TRASectionPointer, MSE_IMPL_EIP mse::impl::enable_if_t<!std::is_base_of<base_class, _TRASectionPointer>::value> MSE_IMPL_EIS >
			const_iterator(const _TRASectionPointer& ptr, size_type index = 0) : base_class((*ptr).m_start_iter, (*ptr).m_count, index) {}
			MSE_INHERIT_ITERATOR_ARITHMETIC_OPERATORS_FROM(base_class, const_iterator);
		private:
			const_iterator(const _TRAIterator& iter, size_type count, size_type index) : base_class(iter, count, index) {}
			friend class TRandomAccessConstSection;
		};
		typedef const_iterator iterator;

		const_iterator cbegin() const { return const_iterator((*this).m_start_iter, (*this).m_count, 0); }
		const_iterator begin() const { return cbegin(); }
		const_iterator cend() const {
			auto retval(const_iterator((*this).m_start_iter, (*this).m_count, 0));
			retval += mse::msear_as_a_size_t((*this).m_count);
			return retval;
		}
		const_iterator end() const { return cend(); }
		typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
		const_reverse_iterator rbegin() const {	// return iterator for beginning of reversed nonmutable sequence
			return (const_reverse_iterator(end()));
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

	private:

		TXScopeRandomAccessConstSection<_TRAIterator> xscope_subsection_pv(size_type pos = 0, size_type n = npos) const {
			if (pos > (*this).size()) { MSE_THROW(msearray_range_error("out of bounds index - TXScopeRandomAccessConstSection xscope_subsection() const - TRandomAccessConstSection")); }
			return TXScopeRandomAccessConstSection<_TRAIterator>((*this).m_start_iter + mse::msear_as_a_size_t(pos), std::min(mse::msear_as_a_size_t(n), mse::msear_as_a_size_t((*this).size()) - mse::msear_as_a_size_t(pos)));
		}
		typedef mse::impl::conditional_t<mse::impl::is_xscope<_TRAIterator>::value, TXScopeRandomAccessConstSection<_TRAIterator>, TRandomAccessConstSection<_TRAIterator> > subsection_t;
		subsection_t subsection_pv(size_type pos = 0, size_type n = npos) const {
			if (pos > (*this).size()) { MSE_THROW(msearray_range_error("out of bounds index - TRandomAccessConstSection<_TRAIterator> subsection() const - TRandomAccessConstSection")); }
			return subsection_t((*this).m_start_iter + mse::msear_as_a_size_t(pos), std::min(mse::msear_as_a_size_t(n), mse::msear_as_a_size_t((*this).size()) - mse::msear_as_a_size_t(pos)));
		}

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		friend class TXScopeRandomAccessConstSection<_TRAIterator>;

		template <typename _TSection>
		friend auto make_xscope_subsection(const _TSection& xs_section, typename _TSection::size_type pos/* = 0*/, typename _TSection::size_type n/* = _TSection::npos*/) -> decltype(xs_section.xscope_subsection_pv(pos, n));
		template <typename _TSection>
		friend auto make_subsection(const _TSection& section, typename _TSection::size_type pos/* = 0*/, typename _TSection::size_type n/* = _TSection::npos*/)
			-> decltype(section.subsection_pv(pos, n));
	};

	namespace us {
		namespace impl {
			template <typename _TRAIterator>
			class TRandomAccessSectionBase : public mse::us::impl::RandomAccessSectionTagBase
				, MSE_INHERIT_COMMON_XSCOPE_ITERATOR_TAG_BASE_SET_FROM(_TRAIterator, TRandomAccessSectionBase<_TRAIterator>)
				, std::conditional<mse::impl::is_static_structure_iterator<mse::impl::remove_reference_t<_TRAIterator> >::value || mse::impl::is_structure_locking_iterator<mse::impl::remove_reference_t<_TRAIterator> >::value, mse::us::impl::StaticStructureContainerTagBase, mse::impl::TPlaceHolder<mse::us::impl::StaticStructureContainerTagBase, TRandomAccessSectionBase<_TRAIterator> > >::type
				, std::conditional<mse::impl::is_contiguous_sequence_iterator<mse::impl::remove_reference_t<_TRAIterator> >::value, mse::us::impl::ContiguousSequenceContainerTagBase, mse::impl::TPlaceHolder<mse::us::impl::ContiguousSequenceContainerTagBase, TRandomAccessSectionBase<_TRAIterator> > >::type
			{
			public:
				typedef _TRAIterator iterator_type;
				typedef _TRAIterator ra_iterator_type;
				MSE_INHERITED_RANDOM_ACCESS_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(
					mse::impl::random_access_iterator_base<mse::impl::remove_reference_t<decltype(std::declval<_TRAIterator>()[0])> >);

				//TRandomAccessSectionBase(const TRandomAccessSectionBase& src) = default;
				TRandomAccessSectionBase(const TRandomAccessSectionBase& src) : m_count(src.m_count), m_start_iter(src.m_start_iter) {}
				TRandomAccessSectionBase(const _TRAIterator& start_iter, size_type count) : m_count(count), m_start_iter(start_iter) {}
				template <typename _TRALoneParam>
				TRandomAccessSectionBase(const _TRALoneParam& param)
					: m_count(s_count_from_lone_param(param)), m_start_iter(s_xscope_iter_from_lone_param(param)) {}
				template <typename _TRALoneParam>
				TRandomAccessSectionBase(_TRALoneParam&& param)
					: m_count(s_count_from_lone_param(param)), m_start_iter(s_xscope_iter_from_lone_param(MSE_FWD(param))) {}
				/* The presence of this constructor for native arrays should not be construed as condoning the use of native arrays. */
				template<size_t Tn>
				TRandomAccessSectionBase(value_type(&native_array)[Tn]) : m_count(Tn), m_start_iter(native_array) {}

				reference operator[](size_type _P) const {
					if (m_count <= _P) { MSE_THROW(msearray_range_error("out of bounds index - reference operator[](size_type _P) - TRandomAccessSectionBase")); }
					return m_start_iter[difference_type(mse::msear_as_a_size_t(_P))];
				}
				reference at(size_type _P) const {
					return (*this)[_P];
				}
				reference front() const {
					if (0 == (*this).size()) { MSE_THROW(msearray_range_error("front() on empty - reference front() const - TRandomAccessSectionBase")); }
					return (*this)[0];
				}
				reference back() const {
					if (0 == (*this).size()) { MSE_THROW(msearray_range_error("back() on empty - reference back() const - TRandomAccessSectionBase")); }
					return (*this)[(*this).size() - 1];
				}
				size_type size() const _NOEXCEPT {
					return m_count;
				}
				size_type length() const _NOEXCEPT {
					return (*this).size();
				}
				size_type max_size() const _NOEXCEPT {	// return maximum possible length of sequence
					return static_cast<size_type>((std::numeric_limits<difference_type>::max)());
				}
				bool empty() const _NOEXCEPT {
					return (0 == (*this).size());
				}

				template<typename _TRAParam>
				bool equal(const _TRAParam& ra_param) const {
					auto sv = mse::make_xscope_random_access_const_section(mse::rsv::as_an_fparam(ra_param));
					if (size() != sv.size()) {
						return false;
					}
					//return std::equal(xscope_cbegin(), xscope_cend(), sv.xscope_cbegin());
					auto first1 = xscope_cbegin();
					auto last1 = xscope_cend();
					auto first2 = sv.xscope_cbegin();
					while (first1 != last1) {
						if (!(*first1 == *first2)) {
							return false;
						}
						++first1; ++first2;
					}
					return true;
				}
				template<typename _TRAParam>
				bool equal(size_type pos1, size_type n1, const _TRAParam& ra_param) const {
					auto sv = mse::make_xscope_random_access_const_section(mse::rsv::as_an_fparam(ra_param));
					return subsection(pos1, n1).equal(sv);
				}
				template<typename _TRAParam>
				bool equal(size_type pos1, size_type n1, const _TRAParam& ra_param, size_type pos2, size_type n2) const {
					auto sv = mse::make_xscope_random_access_const_section(mse::rsv::as_an_fparam(ra_param));
					return subsection(pos1, n1).equal(sv.subsection(pos2, n2));
				}
				template <typename _TRAIterator2>
				bool equal(size_type pos1, size_type n1, const _TRAIterator2& s, size_type n2) const {
					auto sv = us::impl::TRandomAccessConstSectionBase<_TRAIterator2>(mse::rsv::as_an_fparam(s), n2);
					return subsection(pos1, n1).equal(sv);
				}
				template<typename _TRAParam>
				bool operator==(const _TRAParam& ra_param) const {
					return equal(ra_param);
				}
				template<typename _TRAParam>
				bool operator!=(const _TRAParam& ra_param) const {
					return !((*this) == ra_param);
				}

				template<typename _TRAParam>
				bool lexicographical_compare(const _TRAParam& ra_param) const {
					auto sv = mse::make_xscope_random_access_const_section(mse::rsv::as_an_fparam(ra_param));
					return std::lexicographical_compare(xscope_cbegin(), xscope_cend(), sv.xscope_cbegin(), sv.xscope_cend());
				}
				template<typename _TRAParam>
				bool lexicographical_compare(size_type pos1, size_type n1, const _TRAParam& ra_param) const {
					auto sv = mse::make_xscope_random_access_const_section(mse::rsv::as_an_fparam(ra_param));
					return subsection(pos1, n1).lexicographical_compare(sv);
				}
				template<typename _TRAParam>
				bool lexicographical_compare(size_type pos1, size_type n1, const _TRAParam& ra_param, size_type pos2, size_type n2) const {
					auto sv = mse::make_xscope_random_access_const_section(mse::rsv::as_an_fparam(ra_param));
					return subsection(pos1, n1).lexicographical_compare(sv.subsection(pos2, n2));
				}
				template <typename _TRAIterator2>
				bool lexicographical_compare(size_type pos1, size_type n1, const _TRAIterator2& s, size_type n2) const {
					auto sv = mse::make_xscope_random_access_const_section(mse::rsv::as_an_fparam(s), n2);
					return subsection(pos1, n1).lexicographical_compare(sv);
				}
				template<typename _TRAParam>
				bool operator<(const _TRAParam& ra_param) const {
					auto sv = mse::make_xscope_random_access_const_section(mse::rsv::as_an_fparam(ra_param));
					return lexicographical_compare(sv);
				}
				template<typename _TRAParam>
				bool operator>(const _TRAParam& ra_param) const {
					auto sv = mse::make_xscope_random_access_const_section(mse::rsv::as_an_fparam(ra_param));
					return sv.lexicographical_compare(*this);
				}
				template<typename _TRAParam>
				bool operator<=(const _TRAParam& ra_param) const { return !((*this) > ra_param); }
				template<typename _TRAParam>
				bool operator>=(const _TRAParam& ra_param) const { return !((*this) < ra_param); }

				int compare(const us::impl::TRandomAccessConstSectionBase<_TRAIterator>& sv) const _NOEXCEPT {
					size_type rlen = std::min(size(), sv.size());

					int retval = 0;
					auto _First1 = (*this).xscope_cbegin();
					auto _First2 = sv.xscope_cbegin();
					for (; 0 < rlen; --rlen, ++_First1, ++_First2)
						if (!((*_First1) == (*_First2)))
							return (((*_First1) < (*_First2)) ? -1 : +1);

					if (retval == 0) // first rlen chars matched
						retval = size() == sv.size() ? 0 : (size() < sv.size() ? -1 : 1);
					return retval;
				}
				int compare(size_type pos1, size_type n1, us::impl::TRandomAccessConstSectionBase<_TRAIterator> sv) const {
					return subsection(pos1, n1).compare(sv);
				}
				int compare(size_type pos1, size_type n1, us::impl::TRandomAccessConstSectionBase<_TRAIterator> sv, size_type pos2, size_type n2) const {
					return subsection(pos1, n1).compare(sv.subsection(pos2, n2));
				}
				template <typename _TRAIterator2>
				int compare(size_type pos1, size_type n1, const _TRAIterator2& s, size_type n2) const {
					return subsection(pos1, n1).compare(us::impl::TRandomAccessConstSectionBase<_TRAIterator>(s, n2));
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
						(*target_iter) = (*this)[i];
						++target_iter;
					}
					return n;
				}

				void remove_prefix(size_type n) /*_NOEXCEPT*/ {
					if (n > (*this).size()) { MSE_THROW(msearray_range_error("out of bounds index - void remove_prefix() - TRandomAccessSectionBase")); }
					m_count -= n;
					m_start_iter += n;
				}
				void remove_suffix(size_type n) /*_NOEXCEPT*/ {
					if (n > (*this).size()) { MSE_THROW(msearray_range_error("out of bounds index - void remove_suffix() - TRandomAccessSectionBase")); }
					m_count -= n;
				}

				template<typename _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_base_of<TRandomAccessSectionBase, _Ty2>::value && mse::impl::HasOrInheritsAssignmentOperator_msemsearray<_TRAIterator>::value> MSE_IMPL_EIS >
				void swap(_Ty2& _Other) _NOEXCEPT_OP(_NOEXCEPT_OP(TRandomAccessSectionBase(_Other)) && _NOEXCEPT_OP(std::declval<_TRAIterator>().operator=(std::declval<_TRAIterator>()))) {
					TRandomAccessSectionBase& _Other2 = _Other;
					std::swap((*this), _Other2);
				}

				size_type find(const us::impl::TRandomAccessConstSectionBase<_TRAIterator>& s, size_type pos = 0) const _NOEXCEPT {
					if ((1 > s.size()) || (1 > (*this).size())) {
						return npos;
					}
					auto cit = std::search((*this).xscope_cbegin(), (*this).xscope_cend(), s.xscope_cbegin(), s.xscope_cend());
					if ((*this).xscope_cend() == cit) {
						return npos;
					}
					return (cit - (*this).xscope_cbegin());
				}
				size_type find(const value_type& c, size_type pos = 0) const _NOEXCEPT {
					if ((*this).size() <= 1) {
						return npos;
					}
					auto cit1 = std::find(xscope_cbegin(), xscope_cend(), c);
					return (xscope_cend() == cit1) ? npos : (cit1 - xscope_cbegin());
				}
				size_type rfind(const us::impl::TRandomAccessConstSectionBase<_TRAIterator>& s, size_type pos = npos) const _NOEXCEPT {
					if ((1 > s.size()) || (1 > (*this).size())) {
						return npos;
					}
					auto cit = std::find_end((*this).xscope_cbegin(), (*this).xscope_cend(), s.xscope_cbegin(), s.xscope_cend());
					if ((*this).xscope_cend() == cit) {
						return npos;
					}
					return (cit - (*this).xscope_cbegin());
				}
				size_type rfind(const value_type& c, size_type pos = npos) const _NOEXCEPT {
					if ((*this).size() <= 1) {
						return npos;
					}
					if (pos < (*this).size()) {
						++pos;
					}
					else {
						pos = (*this).size();
					}
					for (size_type i = pos; 0 != i;) {
						--i;
						if ((*this)[i] == c) {
							return i;
						}
					}
					return npos;
				}
				size_type find_first_of(const us::impl::TRandomAccessConstSectionBase<_TRAIterator>& s, size_type pos = 0) const _NOEXCEPT {
					if ((1 > s.size()) || (1 > (*this).size())) {
						return npos;
					}
					auto cit = std::find_first_of((*this).xscope_cbegin(), (*this).xscope_cend(), s.xscope_cbegin(), s.xscope_cend());
					if ((*this).xscope_cend() == cit) {
						return npos;
					}
					return (cit - (*this).xscope_cbegin());
				}
				size_type find_first_of(const value_type& c, size_type pos = 0) const _NOEXCEPT {
					return find(c, pos);
				}
				size_type find_last_of(const us::impl::TRandomAccessConstSectionBase<_TRAIterator>& s, size_type pos = npos) const _NOEXCEPT {
					if ((1 > s.size()) || (1 > (*this).size())) {
						return npos;
					}
					if (pos < (*this).size()) {
						++pos;
					}
					else {
						pos = (*this).size();
					}
					for (size_type i = pos; 0 != i;) {
						--i;
						const auto& domain_element_cref((*this)[i]);
						for (const auto& search_element_cref : s) {
							if (domain_element_cref == search_element_cref) {
								return i;
							}
						}
					}
					return npos;
				}
				size_type find_last_of(const value_type& c, size_type pos = npos) const _NOEXCEPT {
					return rfind(c, pos);
				}
				size_type find_first_not_of(const us::impl::TRandomAccessConstSectionBase<_TRAIterator>& s, size_type pos = 0) const _NOEXCEPT {
					if ((1 > s.size()) || (1 > (*this).size())) {
						return npos;
					}
					auto cit = std::find_if_not((*this).xscope_cbegin(), (*this).xscope_cend(), [&s](const value_type& domain_element_cref) {
						for (const auto& search_element_cref : s) {
							if (domain_element_cref == search_element_cref) {
								return true;
							}
						}
						return false;
					}
					);
					if ((*this).xscope_cend() == cit) {
						return npos;
					}
					return (cit - (*this).xscope_cbegin());
				}
				size_type find_first_not_of(const value_type& c, size_type pos = 0) const _NOEXCEPT {
					if (1 > (*this).size()) {
						return npos;
					}
					auto cit = std::find_if_not((*this).xscope_cbegin(), (*this).xscope_cend(), [&c](const value_type& domain_element_cref) {
						if (domain_element_cref == c) {
							return true;
						}
						return false;
					}
					);
					if ((*this).xscope_cend() == cit) {
						return npos;
					}
					return (cit - (*this).xscope_cbegin());
				}
				size_type find_last_not_of(const us::impl::TRandomAccessConstSectionBase<_TRAIterator>& s, size_type pos = npos) const _NOEXCEPT {
					if ((1 > s.size()) || (1 > (*this).size())) {
						return npos;
					}
					if (pos < (*this).size()) {
						++pos;
					}
					else {
						pos = (*this).size();
					}
					for (size_type i = pos; 0 != i;) {
						--i;
						const auto& domain_element_cref((*this)[i]);
						for (const auto& search_element_cref : s) {
							if (domain_element_cref != search_element_cref) {
								return i;
							}
						}
					}
					return npos;
				}
				size_type find_last_not_of(const value_type& c, size_type pos = npos) const _NOEXCEPT {
					if (1 > (*this).size()) {
						return npos;
					}
					if (pos < (*this).size()) {
						++pos;
					}
					else {
						pos = (*this).size();
					}
					for (size_type i = pos; 0 != i;) {
						--i;
						const auto& domain_element_cref((*this)[i]);
						if (domain_element_cref != c) {
							return i;
						}
					}
					return npos;
				}

				bool starts_with(const us::impl::TRandomAccessConstSectionBase<_TRAIterator>& s) const _NOEXCEPT {
					return (size() >= s.size()) && equal(0, s.size(), s);
				}
				bool starts_with(const value_type& c) const _NOEXCEPT {
					return (!empty()) && (front() == c);
				}
				bool ends_with(const us::impl::TRandomAccessConstSectionBase<_TRAIterator>& s) const _NOEXCEPT {
					return (size() >= s.size()) && equal(size() - s.size(), npos, s);
				}
				bool ends_with(const value_type& c) const _NOEXCEPT {
					return (!empty()) && (back() == c);
				}

				typedef mse::impl::conditional_t<mse::impl::is_instantiation_of<_TRAIterator, mse::TXScopeCSSSStrongRAIterator>::value
					|| mse::impl::is_instantiation_of<_TRAIterator, mse::TXScopeCSSSStrongRASectionIterator>::value
					, TXScopeCSSSStrongRASectionIterator<_TRAIterator>, TXScopeRASectionIterator<_TRAIterator> > xscope_iterator_base;
				class xscope_iterator : public xscope_iterator_base {
				public:
					typedef xscope_iterator_base base_class;
					MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);
					xscope_iterator(const xscope_iterator&) = default;
					xscope_iterator(xscope_iterator&&) = default;

					//MSE_USING(xscope_iterator, base_class);
					template<class _TRASectionPointer, MSE_IMPL_EIP mse::impl::enable_if_t<!std::is_base_of<base_class, _TRASectionPointer>::value> MSE_IMPL_EIS >
					xscope_iterator(const _TRASectionPointer& ptr, size_type index = 0) : base_class((*ptr).m_start_iter, (*ptr).m_count, index) {}
					MSE_INHERIT_ITERATOR_ARITHMETIC_OPERATORS_FROM(base_class, xscope_iterator);
				private:
					xscope_iterator(const _TRAIterator& iter, size_type count, size_type index) : base_class(iter, count, index) {}
					friend class TRandomAccessSectionBase;
				};

				typedef mse::impl::conditional_t<mse::impl::is_instantiation_of<_TRAIterator, mse::TXScopeCSSSStrongRAConstIterator>::value
					|| mse::impl::is_instantiation_of<_TRAIterator, mse::TXScopeCSSSStrongRAIterator>::value
					|| mse::impl::is_instantiation_of<_TRAIterator, mse::TXScopeCSSSStrongRASectionConstIterator>::value
					|| mse::impl::is_instantiation_of<_TRAIterator, mse::TXScopeCSSSStrongRASectionIterator>::value
					, TXScopeCSSSStrongRASectionConstIterator<_TRAIterator>, TXScopeRASectionConstIterator<_TRAIterator> > xscope_const_iterator_base;
				class xscope_const_iterator : public xscope_const_iterator_base {
				public:
					typedef xscope_const_iterator_base base_class;
					xscope_const_iterator(const xscope_const_iterator&) = default;
					xscope_const_iterator(xscope_const_iterator&&) = default;

					//MSE_USING(xscope_const_iterator, base_class);
					template<class _TRASectionPointer, MSE_IMPL_EIP mse::impl::enable_if_t<!std::is_base_of<base_class, _TRASectionPointer>::value> MSE_IMPL_EIS >
					xscope_const_iterator(const _TRASectionPointer& ptr, size_type index = 0) : base_class((*ptr).m_start_iter, (*ptr).m_count, index) {}
				private:
					xscope_const_iterator(const _TRAIterator& iter, size_type count, size_type index) : base_class(iter, count, index) {}
					friend class TRandomAccessSectionBase;
				};
				xscope_iterator xscope_begin() const { return xscope_iterator((*this).m_start_iter, (*this).m_count, 0); }
				xscope_const_iterator xscope_cbegin() const { return xscope_const_iterator((*this).m_start_iter, (*this).m_count, 0); }
				xscope_iterator xscope_end() const {
					auto retval(xscope_iterator((*this).m_start_iter, (*this).m_count, 0));
					retval += mse::msear_as_a_size_t((*this).m_count);
					return retval;
				}
				xscope_const_iterator xscope_cend() const {
					auto retval(xscope_const_iterator((*this).m_start_iter, (*this).m_count, 0));
					retval += mse::msear_as_a_size_t((*this).m_count);
					return retval;
				}

				template <typename _TRALoneParam>
				static auto s_xscope_iter_from_lone_param(const _TRALoneParam& param) 
					-> decltype(mse::impl::ra_section_helpers::s_xscope_iter_from_lone_param(param)) {
					return mse::impl::ra_section_helpers::s_xscope_iter_from_lone_param(param);
				}
				template <typename _TRALoneParam>
				static auto s_xscope_iter_from_lone_param(_TRALoneParam&& param)
					-> decltype(mse::impl::ra_section_helpers::s_xscope_iter_from_lone_param(MSE_FWD(param))) {
					return mse::impl::ra_section_helpers::s_xscope_iter_from_lone_param(MSE_FWD(param));
				}
				template <typename _TRALoneParam>
				static auto s_iter_from_lone_param(const _TRALoneParam& param) {
					return mse::impl::ra_section_helpers::s_iter_from_lone_param(param);
				}
				template <typename _TRALoneParam>
				static auto s_count_from_lone_param(const _TRALoneParam& param) {
					return mse::impl::ra_section_helpers::s_count_from_lone_param(param);
				}

			protected:
				TRandomAccessSectionBase subsection(size_type pos = 0, size_type n = npos) const {
					if (pos > (*this).size()) { MSE_THROW(msearray_range_error("out of bounds index - TRandomAccessSectionBase subsection() const - TRandomAccessSectionBase")); }
					return TRandomAccessSectionBase((*this).m_start_iter + mse::msear_as_a_size_t(pos), std::min(mse::msear_as_a_size_t(n), mse::msear_as_a_size_t((*this).size()) - mse::msear_as_a_size_t(pos)));
				}

			private:

				MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

				size_type m_count = 0;
				_TRAIterator m_start_iter;

				friend class mse::TXScopeRandomAccessSection<_TRAIterator>;
				friend class mse::TRandomAccessSection<_TRAIterator>;
				template<typename _TRAIterator1> friend class TXScopeTRandomAccessConstSectionBase;
				template<typename _TRAIterator1> friend class TXScopeTRandomAccessSectionBase;
				template<typename _TRAIterator1> friend class TRandomAccessConstSectionBase;
				template<typename _TRAIterator1> friend class TRandomAccessSectionBase;
				friend struct mse::impl::ra_const_section_helpers;
				friend struct mse::impl::ra_section_helpers;
			};
		}
	}

	template <typename _TRAIterator>
	class TXScopeRandomAccessSection : public us::impl::TRandomAccessSectionBase<_TRAIterator>, public mse::us::impl::XScopeContainsNonOwningScopeReferenceTagBase, public mse::us::impl::StrongPointerAsyncNotShareableAndNotPassableTagBase {
	public:
		typedef us::impl::TRandomAccessSectionBase<_TRAIterator> base_class;
		typedef _TRAIterator iterator_type;
		MSE_INHERITED_RANDOM_ACCESS_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);

		//TXScopeRandomAccessSection(const _TRAIterator& start_iter, size_type count) : base_class(start_iter,count) {}
		//TXScopeRandomAccessSection(const TXScopeRandomAccessSection& src) = default;
		//template<class _Ty2 = _TRAIterator, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _TRAIterator>::value) && (mse::impl::is_potentially_not_xscope<_TRAIterator>::value)> MSE_IMPL_EIS >
		//TXScopeRandomAccessSection(const TRandomAccessSection<_TRAIterator>& src) : base_class(src) {}
		//TXScopeRandomAccessSection(const base_class& src) : base_class(src) {}

		MSE_USING(TXScopeRandomAccessSection, base_class);

		/* use the make_xscope_subsection() free function instead */
		MSE_DEPRECATED TXScopeRandomAccessSection xscope_subsection(size_type pos = 0, size_type n = npos) const {
			return xscope_subsection_pv(pos, n);
		}
		/* prefer the make_subsection() free function instead */
		auto subsection(size_type pos = 0, size_type n = npos) const {
			return subsection_pv(pos, n);
		}
		auto first(size_type count) const { return subsection_pv(0, count); }
		auto last(size_type count) const { return subsection_pv(std::max(difference_type(mse::msear_as_a_size_t((*this).size())) - difference_type(mse::msear_as_a_size_t(count)), 0), count); }

		typedef typename base_class::xscope_iterator xscope_iterator;
		typedef typename base_class::xscope_const_iterator xscope_const_iterator;

		/* These are here because some standard algorithms require them. Prefer the "xscope_" prefixed versions to
		acknowledge that scope iterators are returned. */
		typedef xscope_iterator iterator;
		typedef xscope_const_iterator const_iterator;
		auto begin() const { return (*this).xscope_begin(); }
		auto cbegin() const { return (*this).xscope_cbegin(); }
		auto end() const { return (*this).xscope_end(); }
		auto cend() const { return (*this).xscope_cend(); }

	private:

		TXScopeRandomAccessSection xscope_subsection_pv(size_type pos = 0, size_type n = npos) const {
			if (pos > (*this).size()) { MSE_THROW(msearray_range_error("out of bounds index - TXScopeRandomAccessSection xscope_subsection() const - TXScopeRandomAccessSection")); }
			return TXScopeRandomAccessSection((*this).m_start_iter + mse::msear_as_a_size_t(pos), std::min(mse::msear_as_a_size_t(n), mse::msear_as_a_size_t((*this).size()) - mse::msear_as_a_size_t(pos)));
		}
		typedef mse::impl::conditional_t<mse::impl::is_xscope<_TRAIterator>::value, TXScopeRandomAccessSection, TRandomAccessSection<_TRAIterator> > subsection_t;
		subsection_t subsection_pv(size_type pos = 0, size_type n = npos) const {
			if (pos > (*this).size()) { MSE_THROW(msearray_range_error("out of bounds index - TRandomAccessSection<_TRAIterator> subsection() const - TXScopeRandomAccessSection")); }
			return subsection_t((*this).m_start_iter + mse::msear_as_a_size_t(pos), std::min(mse::msear_as_a_size_t(n), mse::msear_as_a_size_t((*this).size()) - mse::msear_as_a_size_t(pos)));
		}

		TXScopeRandomAccessSection<_TRAIterator>& operator=(const TXScopeRandomAccessSection<_TRAIterator>& _Right_cref) = delete;
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		template <typename _TSection>
		friend auto make_xscope_subsection(const _TSection& xs_section, typename _TSection::size_type pos/* = 0*/, typename _TSection::size_type n/* = _TSection::npos*/) -> decltype(xs_section.xscope_subsection_pv(pos, n));
		template <typename _TSection>
		friend auto make_subsection(const _TSection& section, typename _TSection::size_type pos/* = 0*/, typename _TSection::size_type n/* = _TSection::npos*/)
			-> decltype(section.subsection_pv(pos, n));
	};

	template <typename _TRAIterator>
	class TRandomAccessSection : public us::impl::TRandomAccessSectionBase<_TRAIterator> {
	public:
		typedef us::impl::TRandomAccessSectionBase<_TRAIterator> base_class;
		typedef _TRAIterator iterator_type;
		MSE_INHERITED_RANDOM_ACCESS_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);

		TRandomAccessSection(const TRandomAccessSection& src) : base_class(static_cast<const base_class&>(src)) {}
		TRandomAccessSection(const _TRAIterator& start_iter, size_type count) : base_class(start_iter, count) {}
		template <typename _TRALoneParam>
		TRandomAccessSection(const _TRALoneParam& param) : base_class(base_class::s_iter_from_lone_param(param), base_class::s_count_from_lone_param(param)) {}
		/* The presence of this constructor for native arrays should not be construed as condoning the use of native arrays. */
		template<size_t Tn>
		TRandomAccessSection(value_type(&native_array)[Tn]) : base_class(native_array) {}
		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TRandomAccessSection() {
			mse::impl::T_valid_if_not_an_xscope_type<_TRAIterator>();
		}

		/* use the make_xscope_subsection() free function instead */
		MSE_DEPRECATED TXScopeRandomAccessSection<_TRAIterator> xscope_subsection(size_type pos = 0, size_type n = npos) const {
			return xscope_subsection_pv(pos, n);
		}
		/* prefer the make_subsection() free function instead */
		auto subsection(size_type pos = 0, size_type n = npos) const {
			return subsection_pv(pos, n);
		}
		auto first(size_type count) const { return subsection_pv(0, count); }
		auto last(size_type count) const { return subsection_pv(std::max(difference_type(mse::msear_as_a_size_t((*this).size())) - difference_type(mse::msear_as_a_size_t(count)), 0), count); }

		typedef TRASectionIterator<_TRAIterator> iterator_base;
		class iterator : public iterator_base {
		public:
			typedef iterator_base base_class;
			MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);
			iterator(const iterator&) = default;
			iterator(iterator&&) = default;

			//MSE_USING(iterator, base_class);
			template<class _TRASectionPointer, MSE_IMPL_EIP mse::impl::enable_if_t<!std::is_base_of<base_class, _TRASectionPointer>::value> MSE_IMPL_EIS >
			iterator(const _TRASectionPointer& ptr, size_type index = 0) : base_class((*ptr).m_start_iter, (*ptr).m_count, index) {}
			MSE_INHERIT_ITERATOR_ARITHMETIC_OPERATORS_FROM(base_class, iterator);
		private:
			iterator(const _TRAIterator& iter, size_type count, size_type index) : base_class(iter, count, index) {}
			friend class TRandomAccessSection;
		};
		typedef TRASectionConstIterator<_TRAIterator> const_iterator_base;
		class const_iterator : public const_iterator_base {
		public:
			typedef const_iterator_base base_class;
			MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);
			const_iterator(const const_iterator&) = default;
			const_iterator(const_iterator&&) = default;

			//MSE_USING(const_iterator, base_class);
			template<class _TRASectionPointer, MSE_IMPL_EIP mse::impl::enable_if_t<!std::is_base_of<base_class, _TRASectionPointer>::value> MSE_IMPL_EIS >
			const_iterator(const _TRASectionPointer& ptr, size_type index = 0) : base_class((*ptr).m_start_iter, (*ptr).m_count, index) {}
			MSE_INHERIT_ITERATOR_ARITHMETIC_OPERATORS_FROM(base_class, const_iterator);
		private:
			const_iterator(const _TRAIterator& iter, size_type count, size_type index) : base_class(iter, count, index) {}
			friend class TRandomAccessSection;
		};

		iterator begin() { return iterator((*this).m_start_iter, (*this).m_count, 0); }
		const_iterator begin() const { return cbegin(); }
		const_iterator cbegin() const { return const_iterator((*this).m_start_iter, (*this).m_count, 0); }
		iterator end() {
			auto retval(iterator((*this).m_start_iter, (*this).m_count, 0));
			retval += mse::msear_as_a_size_t((*this).m_count);
			return retval;
		}
		const_iterator end() const { return cend(); }
		const_iterator cend() const {
			auto retval(const_iterator((*this).m_start_iter, (*this).m_count, 0));
			retval += mse::msear_as_a_size_t((*this).m_count);
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

	private:

		TXScopeRandomAccessSection<_TRAIterator> xscope_subsection_pv(size_type pos = 0, size_type n = npos) const {
			if (pos > (*this).size()) { MSE_THROW(msearray_range_error("out of bounds index - TXScopeRandomAccessSection xscope_subsection() const - TRandomAccessSection")); }
			return TXScopeRandomAccessSection<_TRAIterator>((*this).m_start_iter + mse::msear_as_a_size_t(pos), std::min(mse::msear_as_a_size_t(n), mse::msear_as_a_size_t((*this).size()) - mse::msear_as_a_size_t(pos)));
		}
		typedef mse::impl::conditional_t<mse::impl::is_xscope<_TRAIterator>::value, TXScopeRandomAccessSection<_TRAIterator>, TRandomAccessSection<_TRAIterator> > subsection_t;
		subsection_t subsection_pv(size_type pos = 0, size_type n = npos) const {
			if (pos > (*this).size()) { MSE_THROW(msearray_range_error("out of bounds index - TRandomAccessSection<_TRAIterator> subsection() const - TRandomAccessSection")); }
			return subsection_t((*this).m_start_iter + mse::msear_as_a_size_t(pos), std::min(mse::msear_as_a_size_t(n), mse::msear_as_a_size_t((*this).size()) - mse::msear_as_a_size_t(pos)));
		}

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		friend class TXScopeRandomAccessSection<_TRAIterator>;
		friend class TXScopeRandomAccessConstSection<_TRAIterator>;
		friend class TRandomAccessConstSection<_TRAIterator>;

		template <typename _TSection>
		friend auto make_xscope_subsection(const _TSection& xs_section, typename _TSection::size_type pos/* = 0*/, typename _TSection::size_type n/* = _TSection::npos*/) -> decltype(xs_section.xscope_subsection_pv(pos, n));
		template <typename _TSection>
		friend auto make_subsection(const _TSection& section, typename _TSection::size_type pos/* = 0*/, typename _TSection::size_type n/* = _TSection::npos*/)
			-> decltype(section.subsection_pv(pos, n));
	};

	template <typename _TRAIterator>
	auto make_xscope_random_access_const_section(const _TRAIterator& start_iter, typename TXScopeRandomAccessConstSection<_TRAIterator>::size_type count) {
		return TXScopeRandomAccessConstSection<_TRAIterator>(start_iter, count);
	}

	template <typename _TRALoneParam>
	auto make_xscope_random_access_const_section(const _TRALoneParam& param) -> decltype(mse::impl::ra_section::make_xscope_random_access_const_section_helper1(
		typename mse::impl::is_instantiation_of<_TRALoneParam, mse::TXScopeCagedItemFixedConstPointerToRValue>::type(), param)) {

		return mse::impl::ra_section::make_xscope_random_access_const_section_helper1(
			typename mse::impl::is_instantiation_of<_TRALoneParam, mse::TXScopeCagedItemFixedConstPointerToRValue>::type(), param);
	}

	template <typename _TRALoneParam, class = MSE_IMPL_ENABLE_IF_NOT_RETURNABLE_FPARAM(_TRALoneParam)>
	auto make_xscope_random_access_const_section(_TRALoneParam&& param) -> decltype(mse::impl::ra_section::make_xscope_random_access_const_section_helper1(
		typename mse::impl::is_instantiation_of<_TRALoneParam, mse::TXScopeCagedItemFixedConstPointerToRValue>::type(), MSE_FWD(param))) {

		return mse::impl::ra_section::make_xscope_random_access_const_section_helper1(
			typename mse::impl::is_instantiation_of<_TRALoneParam, mse::TXScopeCagedItemFixedConstPointerToRValue>::type(), MSE_FWD(param));
	}

	/* Overloads for rsv::TReturnableFParam<>. */
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_random_access_const_section)

	template <typename _TRAIterator>
	auto make_random_access_const_section(const _TRAIterator& start_iter, typename TRandomAccessConstSection<_TRAIterator>::size_type count) {
		return TRandomAccessConstSection<_TRAIterator>(start_iter, count);
	}
	template <typename _TRALoneParam>
	auto make_random_access_const_section(const _TRALoneParam& param) {
		typedef mse::impl::remove_reference_t<decltype(mse::us::impl::TRandomAccessConstSectionBase<char *>::s_iter_from_lone_param(param))> _TRAIterator;
		return TRandomAccessConstSection<_TRAIterator>(param);
	}

	template <typename _TRAIterator>
	auto make_xscope_random_access_section(const _TRAIterator& start_iter, typename TXScopeRandomAccessSection<_TRAIterator>::size_type count) {
		return TXScopeRandomAccessSection<_TRAIterator>(start_iter, count);
	}
	namespace impl {
		namespace ra_section {
			template <typename _Ty>
			auto make_xscope_random_access_section_helper1(std::true_type, const mse::TXScopeCagedItemFixedPointerToRValue<_Ty>& param) {
				return mse::make_xscope_random_access_const_section(param);
			}
			template <typename _Ty>
			auto make_xscope_random_access_section_helper1(std::true_type, mse::TXScopeCagedItemFixedPointerToRValue<_Ty>&& param) {
				return mse::make_xscope_random_access_const_section(MSE_FWD(param));
			}
			template <typename _TRALoneParam>
			auto make_xscope_random_access_section_helper1(std::false_type, const _TRALoneParam& param) {
				typedef mse::impl::remove_reference_t<decltype(mse::us::impl::TRandomAccessSectionBase<char *>::s_xscope_iter_from_lone_param(param))> _TRAIterator;
				return TXScopeRandomAccessSection<_TRAIterator>(param);
			}
		}
	}
	template <typename _TRALoneParam>
	auto make_xscope_random_access_section(const _TRALoneParam& param) {
		return mse::impl::ra_section::make_xscope_random_access_section_helper1(
			typename mse::impl::is_instantiation_of<_TRALoneParam, mse::TXScopeCagedItemFixedConstPointerToRValue>::type(), param);
	}
	template <typename _TRALoneParam, class = MSE_IMPL_ENABLE_IF_NOT_RETURNABLE_FPARAM(_TRALoneParam)>
	auto make_xscope_random_access_section(_TRALoneParam&& param) {
		return mse::impl::ra_section::make_xscope_random_access_section_helper1(
			typename mse::impl::is_instantiation_of<_TRALoneParam, mse::TXScopeCagedItemFixedConstPointerToRValue>::type(), MSE_FWD(param));
	}

	/* Overloads for rsv::TReturnableFParam<>. */
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_random_access_section)

	/* This function basically just calls the give section's subsection() member function and returns the value.  */
	template<typename _Ty>
	auto random_access_subsection(const _Ty& ra_section, std::tuple<typename _Ty::size_type, typename _Ty::size_type> start_and_length = { 0U, _Ty::npos }) {
		return make_subsection(ra_section, std::get<0>(start_and_length), std::get<1>(start_and_length));
	}
	template<typename _Ty>
	auto xscope_random_access_subsection(const _Ty& ra_section, std::tuple<typename _Ty::size_type, typename _Ty::size_type> start_and_length = { 0U, _Ty::npos }) {
		return make_xscope_subsection(ra_section, std::get<0>(start_and_length), std::get<1>(start_and_length));
	}

	template <typename _TRAIterator>
	auto make_random_access_section(const _TRAIterator& start_iter, typename TRandomAccessSection<_TRAIterator>::size_type count) {
		return TRandomAccessSection<_TRAIterator>(start_iter, count);
	}
	template <typename _TRALoneParam>
	auto make_random_access_section(const _TRALoneParam& param) {
		typedef mse::impl::remove_reference_t<decltype(mse::us::impl::TRandomAccessSectionBase<char *>::s_iter_from_lone_param(param))> _TRAIterator;
		return TRandomAccessSection<_TRAIterator>(param);
	}

#ifdef MSE_HAS_CXX17
	/* deduction guides */
	template<class _TRAIterator>
	TRandomAccessSection(_TRAIterator, typename TRandomAccessSection<_TRAIterator>::size_type)
		->TRandomAccessSection<_TRAIterator>;
	template<class _TRALoneParam>
	TRandomAccessSection(_TRALoneParam)
		->TRandomAccessSection<typename decltype(make_random_access_section(std::declval<_TRALoneParam>()))::iterator_type>;

	template<class _TRAIterator>
	TRandomAccessConstSection(_TRAIterator, typename TRandomAccessConstSection<_TRAIterator>::size_type)
		->TRandomAccessConstSection<_TRAIterator>;
	template<class _TRALoneParam>
	TRandomAccessConstSection(_TRALoneParam)
		->TRandomAccessConstSection<typename decltype(make_random_access_const_section(std::declval<_TRALoneParam>()))::iterator_type>;

	template<class _TRAIterator>
	TXScopeRandomAccessSection(_TRAIterator, typename TXScopeRandomAccessSection<_TRAIterator>::size_type)
		->TXScopeRandomAccessSection<_TRAIterator>;
	template<class _TRALoneParam>
	TXScopeRandomAccessSection(_TRALoneParam)
		->TXScopeRandomAccessSection<typename decltype(make_xscope_random_access_section(std::declval<_TRALoneParam>()))::iterator_type>;

	template<class _TRAIterator>
	TXScopeRandomAccessConstSection(_TRAIterator, typename TXScopeRandomAccessConstSection<_TRAIterator>::size_type)
		->TXScopeRandomAccessConstSection<_TRAIterator>;
	template<class _TRALoneParam>
	TXScopeRandomAccessConstSection(_TRALoneParam)
		->TXScopeRandomAccessConstSection<typename decltype(make_xscope_random_access_const_section(std::declval<_TRALoneParam>()))::iterator_type>;
#endif /* MSE_HAS_CXX17 */

	/* TXScopeCagedRandomAccessConstSectionToRValue<> represents a "random access const section" that refers to a temporary
	object. The "random access const section" is inaccessible ("caged") by default because it is, in general, unsafe. Its
	copyability and movability are also restricted. The "random access const section" can only be accessed by certain types
	and functions (declared as friends) that will ensure that it will be handled safely. */
	template<typename _TRAIterator>
	class TXScopeCagedRandomAccessConstSectionToRValue : public mse::us::impl::XScopeContainsNonOwningScopeReferenceTagBase, public mse::us::impl::StrongPointerAsyncNotShareableAndNotPassableTagBase {
	public:
		void xscope_tag() const {}

	protected:
		TXScopeCagedRandomAccessConstSectionToRValue(TXScopeCagedRandomAccessConstSectionToRValue&&) = default;
		TXScopeCagedRandomAccessConstSectionToRValue(const TXScopeCagedRandomAccessConstSectionToRValue&) = delete;
		TXScopeCagedRandomAccessConstSectionToRValue(const TXScopeRandomAccessConstSection<_TRAIterator>& ptr) : m_xscope_ra_section(ptr) {}

		auto uncaged_ra_section() const { return m_xscope_ra_section; }

	private:
		TXScopeCagedRandomAccessConstSectionToRValue<_TRAIterator>& operator=(const TXScopeCagedRandomAccessConstSectionToRValue<_TRAIterator>& _Right_cref) = delete;
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		TXScopeRandomAccessConstSection<_TRAIterator> m_xscope_ra_section;

		friend class rsv::TXScopeRandomAccessConstSectionFParam<_TRAIterator>;
		template <typename _Ty>
		friend auto impl::ra_section::make_xscope_random_access_const_section_helper1(std::true_type, const TXScopeCagedItemFixedConstPointerToRValue<_Ty>& param)
			-> impl::ra_section::mkxsracsh1_ReturnType<_Ty>;
		template <typename _Ty>
		friend auto impl::ra_section::make_xscope_random_access_const_section_helper1(std::true_type, TXScopeCagedItemFixedConstPointerToRValue<_Ty>&& param)
			->impl::ra_section::mkxsracsh1_ReturnType<_Ty>;
		template <typename _TRALoneParam>
		friend auto make_xscope_random_access_const_section(const _TRALoneParam& param) -> decltype(mse::impl::ra_section::make_xscope_random_access_const_section_helper1(
			typename mse::impl::is_instantiation_of<_TRALoneParam, mse::TXScopeCagedItemFixedConstPointerToRValue>::type(), param));
		template <typename _TRALoneParam, class>
		friend auto make_xscope_random_access_const_section(_TRALoneParam&& param) -> decltype(mse::impl::ra_section::make_xscope_random_access_const_section_helper1(
			typename mse::impl::is_instantiation_of<_TRALoneParam, mse::TXScopeCagedItemFixedConstPointerToRValue>::type(), MSE_FWD(param)));
	};

	namespace rsv {

		template <typename _TRAIterator>
		class TXScopeRandomAccessConstSectionFParam : public TXScopeRandomAccessConstSection<_TRAIterator> {
		public:
			typedef TXScopeRandomAccessConstSection<_TRAIterator> base_class;
			typedef _TRAIterator iterator_type;
			MSE_INHERITED_RANDOM_ACCESS_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);

			//MSE_USING(TXScopeRandomAccessConstSectionFParam, base_class);
			TXScopeRandomAccessConstSectionFParam(const TXScopeRandomAccessConstSectionFParam& src) = default;
			TXScopeRandomAccessConstSectionFParam(const _TRAIterator& start_iter, size_type count) : base_class(start_iter, count) {}
			template <typename _TRALoneParam>
			TXScopeRandomAccessConstSectionFParam(const _TRALoneParam& param) : base_class(construction_helper1(typename
				std::conditional<mse::impl::is_instantiation_of<_TRALoneParam, mse::TXScopeCagedItemFixedConstPointerToRValue>::value
				|| mse::impl::is_instantiation_of<_TRALoneParam, mse::TXScopeCagedRandomAccessConstSectionToRValue>::value
				, std::true_type, std::false_type>::type(), param)) {
			}

			/* use the make_xscope_subsection() free function instead */
			MSE_DEPRECATED TXScopeRandomAccessConstSectionFParam xscope_subsection(size_type pos = 0, size_type n = npos) const {
				return xscope_subsection_pv(pos, n);
			}
			/* prefer the make_subsection() free function instead */
			auto subsection(size_type pos = 0, size_type n = npos) const {
				return subsection_pv(pos, n);
			}
			auto first(size_type count) const { return subsection_pv(0, count); }
			auto last(size_type count) const { return subsection_pv(std::max(difference_type(mse::msear_as_a_size_t((*this).size())) - difference_type(mse::msear_as_a_size_t(count)), 0), count); }

			//typedef typename base_class::xscope_iterator xscope_iterator;
			typedef typename base_class::xscope_const_iterator xscope_const_iterator;

			void xscope_not_returnable_tag() const {}
			void xscope_tag() const {}

		private:
			template <typename _TRAContainer>
			static mse::TXScopeFixedConstPointer<_TRAContainer> construction_helper1(std::true_type, const mse::TXScopeCagedItemFixedConstPointerToRValue<_TRAContainer>& caged_xscpptr) {
				return mse::rsv::TXScopeFixedConstPointerFParam<_TRAContainer>(caged_xscpptr);
			}
			static mse::TXScopeRandomAccessConstSection<_TRAIterator> construction_helper1(std::true_type, const mse::TXScopeCagedRandomAccessConstSectionToRValue<_TRAIterator>& caged_xscpsection) {
				return caged_xscpsection.uncaged_ra_section();
			}
			template <typename _TRALoneParam>
			static auto construction_helper1(std::false_type, const _TRALoneParam& param) {
				return param;
			}

			TXScopeRandomAccessConstSectionFParam xscope_subsection_pv(size_type pos = 0, size_type n = npos) const {
				return mse::make_xscope_subsection(*(static_cast<const base_class*>(this)), pos, n);
			}
			typedef mse::impl::conditional_t<mse::impl::is_xscope<_TRAIterator>::value, TXScopeRandomAccessConstSectionFParam, TRandomAccessConstSection<_TRAIterator> > subsection_t;
			subsection_t subsection_pv(size_type pos = 0, size_type n = npos) const {
				return mse::make_subsection(*(static_cast<const base_class*>(this)), pos, n);
			}

			MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

			template <typename _TSection>
			friend auto mse::make_xscope_subsection(const _TSection& xs_section, typename _TSection::size_type pos/* = 0*/, typename _TSection::size_type n/* = _TSection::npos*/) -> decltype(xs_section.xscope_subsection_pv(pos, n));
			template <typename _TSection>
			friend auto mse::make_subsection(const _TSection& section, typename _TSection::size_type pos/* = 0*/, typename _TSection::size_type n/* = _TSection::npos*/)
				-> decltype(section.subsection_pv(pos, n));
		};
	}

	namespace rsv {
		/* Template specializations of TFParam<>. */

		template<typename _Ty>
		class TFParam<mse::TXScopeRandomAccessConstSection<_Ty> > : public TXScopeRandomAccessConstSectionFParam<_Ty> {
		public:
			typedef TXScopeRandomAccessConstSectionFParam<_Ty> base_class;
			MSE_INHERITED_RANDOM_ACCESS_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);
			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TFParam, base_class);
			void xscope_not_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};

		template<typename _Ty>
		class TFParam<const mse::TXScopeRandomAccessConstSection<_Ty> > : public TXScopeRandomAccessConstSectionFParam<_Ty> {
		public:
			typedef TXScopeRandomAccessConstSectionFParam<_Ty> base_class;
			MSE_INHERITED_RANDOM_ACCESS_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);
			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TFParam, base_class);
			void xscope_not_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};

		template<typename _Ty>
		class TFParam<mse::TXScopeCagedRandomAccessConstSectionToRValue<_Ty> > : public TXScopeRandomAccessConstSectionFParam<_Ty> {
		public:
			typedef TXScopeRandomAccessConstSectionFParam<_Ty> base_class;
			MSE_INHERITED_RANDOM_ACCESS_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);
			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TFParam, base_class);
			void xscope_not_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};

		/* Template specializations of TReturnableFParam<>. */

		template<typename _Ty>
		class TReturnableFParam<mse::TXScopeRandomAccessConstSection<_Ty> > : public TXScopeRandomAccessConstSection<_Ty> {
		public:
			typedef TXScopeRandomAccessConstSection<_Ty> base_class;
			typedef typename base_class::iterator_type iterator_type;
			MSE_INHERITED_RANDOM_ACCESS_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);
			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TReturnableFParam, base_class);

			/* use the make_xscope_subsection() free function instead */
			MSE_DEPRECATED auto xscope_subsection(size_type pos = 0, size_type n = npos) const {
				typedef decltype(mse::make_xscope_subsection(*(static_cast<const base_class*>(this)), pos, n)) base_return_type;
				return TReturnableFParam<base_return_type>(mse::make_xscope_subsection(*(static_cast<const base_class*>(this)), pos, n));
			}
			/* prefer the make_subsection() free function instead */
			auto subsection(size_type pos = 0, size_type n = npos) const {
				typedef decltype(mse::make_subsection(*(static_cast<const base_class*>(this)), pos, n)) base_return_type;
				return TReturnableFParam<base_return_type>(mse::make_subsection(*(static_cast<const base_class*>(this)), pos, n));
			}

			void returnable_once_tag() const {}
			void xscope_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};

		template<typename _Ty>
		class TReturnableFParam<const mse::TXScopeRandomAccessConstSection<_Ty> > : public TXScopeRandomAccessConstSection<_Ty> {
		public:
			typedef TXScopeRandomAccessConstSection<_Ty> base_class;
			typedef typename base_class::iterator_type iterator_type;
			MSE_INHERITED_RANDOM_ACCESS_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);
			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TReturnableFParam, base_class);

			/* use the make_xscope_subsection() free function instead */
			MSE_DEPRECATED auto xscope_subsection(size_type pos = 0, size_type n = npos) const {
				typedef decltype(mse::make_xscope_subsection(*(static_cast<const base_class*>(this)), pos, n)) base_return_type;
				return TReturnableFParam<base_return_type>(mse::make_xscope_subsection(*(static_cast<const base_class*>(this)), pos, n));
			}
			/* prefer the make_subsection() free function instead */
			auto subsection(size_type pos = 0, size_type n = npos) const {
				typedef decltype(mse::make_subsection(*(static_cast<const base_class*>(this)), pos, n)) base_return_type;
				return TReturnableFParam<base_return_type>(mse::make_subsection(*(static_cast<const base_class*>(this)), pos, n));
			}

			void returnable_once_tag() const {}
			void xscope_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};

		template<typename _Ty>
		class TReturnableFParam<mse::TXScopeRandomAccessSection<_Ty> > : public TXScopeRandomAccessSection<_Ty> {
		public:
			typedef TXScopeRandomAccessSection<_Ty> base_class;
			typedef typename base_class::iterator_type iterator_type;
			MSE_INHERITED_RANDOM_ACCESS_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);
			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TReturnableFParam, base_class);

			/* use the make_xscope_subsection() free function instead */
			MSE_DEPRECATED auto xscope_subsection(size_type pos = 0, size_type n = npos) const {
				typedef decltype(mse::make_xscope_subsection(*(static_cast<const base_class*>(this)), pos, n)) base_return_type;
				return TReturnableFParam<base_return_type>(mse::make_xscope_subsection(*(static_cast<const base_class*>(this)), pos, n));
			}
			/* prefer the make_subsection() free function instead */
			auto subsection(size_type pos = 0, size_type n = npos) const {
				typedef decltype(mse::make_subsection(*(static_cast<const base_class*>(this)), pos, n)) base_return_type;
				return TReturnableFParam<base_return_type>(mse::make_subsection(*(static_cast<const base_class*>(this)), pos, n));
			}

			void returnable_once_tag() const {}
			void xscope_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};

		template<typename _Ty>
		class TReturnableFParam<const mse::TXScopeRandomAccessSection<_Ty> > : public TXScopeRandomAccessSection<_Ty> {
		public:
			typedef TXScopeRandomAccessSection<_Ty> base_class;
			typedef typename base_class::iterator_type iterator_type;
			MSE_INHERITED_RANDOM_ACCESS_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);
			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TReturnableFParam, base_class);

			/* use the make_xscope_subsection() free function instead */
			MSE_DEPRECATED auto xscope_subsection(size_type pos = 0, size_type n = npos) const {
				typedef decltype(mse::make_xscope_subsection(*(static_cast<const base_class*>(this)), pos, n)) base_return_type;
				return TReturnableFParam<base_return_type>(mse::make_xscope_subsection(*(static_cast<const base_class*>(this)), pos, n));
			}
			/* prefer the make_subsection() free function instead */
			auto subsection(size_type pos = 0, size_type n = npos) const {
				typedef decltype(mse::make_subsection(*(static_cast<const base_class*>(this)), pos, n)) base_return_type;
				return TReturnableFParam<base_return_type>(mse::make_subsection(*(static_cast<const base_class*>(this)), pos, n));
			}

			void returnable_once_tag() const {}
			void xscope_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};
	}

	template <typename _TElement>
	using TXScopeCSSSXSTERandomAccessConstSection = TXScopeRandomAccessConstSection<TXScopeCSSSXSTERAConstIterator<_TElement> >;
	template <typename _TElement>
	using TXScopeCSSSXSTERandomAccessSection = TXScopeRandomAccessSection<TXScopeCSSSXSTERAIterator<_TElement> >;

	template <typename _TRAIterator>
	auto make_xscope_csssxste_random_access_const_section(const _TRAIterator& start_iter, typename TXScopeRandomAccessConstSection<_TRAIterator>::size_type count) {
		typedef mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(*start_iter)> > _TElement;
		return TXScopeCSSSXSTERandomAccessConstSection<_TElement>(TXScopeCSSSXSTERAConstIterator<_TElement>(start_iter), count);
	}
	template <typename _TRALoneParam>
	auto make_xscope_csssxste_random_access_const_section(const _TRALoneParam& param) {
		typedef mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(make_xscope_random_access_const_section(param)[0])> > _TElement;
		return TXScopeCSSSXSTERandomAccessConstSection<_TElement>(make_xscope_random_access_const_section(param));
	}
	/* Overloads for rsv::TReturnableFParam<>. */
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_csssxste_random_access_const_section)

	template <typename _TRAIterator>
	auto make_xscope_csssxste_random_access_section(const _TRAIterator& start_iter, typename TXScopeRandomAccessSection<_TRAIterator>::size_type count) {
		typedef mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(*start_iter)> > _TElement;
		return TXScopeCSSSXSTERandomAccessSection<_TElement>(TXScopeCSSSXSTERAIterator<_TElement>(start_iter), count);
	}
	template <typename _TRALoneParam>
	auto make_xscope_csssxste_random_access_section(const _TRALoneParam& param) {
		typedef mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(make_xscope_random_access_section(param)[0])> > _TElement;
		return TXScopeCSSSXSTERandomAccessSection<_TElement>(make_xscope_random_access_section(param));
	}
	/* Overloads for rsv::TReturnableFParam<>. */
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_csssxste_random_access_section)


	namespace rsv {
		/* TAsyncShareableObj is intended as a transparent wrapper for other classes/objects. */
		template<typename _TROy>
		class TAsyncShareableObj : public _TROy {
		public:
			MSE_USING_WITH_ADDED_INIT(TAsyncShareableObj, _TROy, valid_if_TROy_is_not_marked_as_unshareable());

			MSE_USING_ASSIGNMENT_OPERATOR(_TROy);

			void async_shareable_tag() const {}

			/* inherit the async passability of _TROy */
			template<class _TROy2 = _TROy, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_TROy2, _TROy>::value) \
				&& (mse::impl::is_marked_as_passable_msemsearray<_TROy2>::value)> MSE_IMPL_EIS > \
			void async_passable_tag() const {}

		private:

			/* If _TROy is "marked" as not safe to share among threads, then the following member function will not
			instantiate, causing an (intended) compile error. */
			template<MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::is_marked_as_shareable_and_passable_msemsearray<_TROy>::value
				|| ((!std::is_convertible<_TROy*, mse::us::impl::AsyncNotShareableTagBase*>::value))> MSE_IMPL_EIS >
			void valid_if_TROy_is_not_marked_as_unshareable() const {}

			/* There's a bug in the g++ atomic<> implementation (prior to C++17) that requires (public) access to the '&' operator. */
#if defined(MSE_HAS_CXX17) || ((!defined(__GNUC__)) && (!defined(__GNUG__)))
			MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;
#endif // defined(MSE_HAS_CXX17) || ((!defined(__GNUC__)) && (!defined(__GNUG__)))
		};

		/* TAsyncPassableObj is intended as a transparent wrapper for other classes/objects. */
		template<typename _TROy>
		class TAsyncPassableObj : public _TROy {
		public:
			MSE_USING_WITH_ADDED_INIT(TAsyncPassableObj, _TROy, valid_if_TROy_is_not_marked_as_unpassable());

			MSE_USING_ASSIGNMENT_OPERATOR(_TROy);

			void async_passable_tag() const {} /* Indication that this type is eligible to be passed between threads. */

			/* inherit the async shareability of _TROy */
			template<class _TROy2 = _TROy, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_TROy2, _TROy>::value) \
				&& (mse::impl::is_marked_as_shareable_msemsearray<_TROy2>::value)> MSE_IMPL_EIS > \
			void async_shareable_tag() const {}

		private:

			/* If _TROy is "marked" as not safe to pass between threads, then the following member function will not
			instantiate, causing an (intended) compile error. */
			template<MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::is_marked_as_passable_msemsearray<_TROy>::value
				|| ((!std::is_convertible<_TROy*, mse::us::impl::AsyncNotPassableTagBase*>::value))> MSE_IMPL_EIS >
			void valid_if_TROy_is_not_marked_as_unpassable() const {}

			MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;
		};

		/* TAsyncShareableAndPassableObj is intended as a transparent wrapper for other classes/objects. */
		template<typename _TROy>
		class TAsyncShareableAndPassableObj : public _TROy {
		public:
			MSE_USING_WITH_ADDED_INIT(TAsyncShareableAndPassableObj, _TROy, valid_if_TROy_is_not_marked_as_unshareable_or_unpassable());

			MSE_USING_ASSIGNMENT_OPERATOR(_TROy);

			void async_shareable_and_passable_tag() const {}

		private:

			/* If _TROy is "marked" as not safe to share among threads, then the following member function will not
			instantiate, causing an (intended) compile error. */
			template<MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::is_marked_as_shareable_and_passable_msemsearray<_TROy>::value || (
				(!std::is_convertible<_TROy*, mse::us::impl::AsyncNotShareableTagBase*>::value)
				&& (!std::is_convertible<_TROy*, mse::us::impl::AsyncNotPassableTagBase*>::value)
				)> MSE_IMPL_EIS >
			void valid_if_TROy_is_not_marked_as_unshareable_or_unpassable() const {}

			/* There's a bug in the g++ atomic<> implementation (prior to C++17) that requires (public) access to the '&' operator. */
#if defined(MSE_HAS_CXX17) || ((!defined(__GNUC__)) && (!defined(__GNUG__)))
			MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;
#endif // defined(MSE_HAS_CXX17) || ((!defined(__GNUC__)) && (!defined(__GNUG__)))
		};


		/* template specializations */

#define MSE_ASYNC_SHAREABLE_IMPL_OBJ_INHERIT_ASSIGNMENT_OPERATOR(class_name) \
		auto& operator=(class_name&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); } \
		auto& operator=(const class_name& _X) { base_class::operator=(_X); return (*this); } \
		template<class _Ty2> auto& operator=(_Ty2&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); } \
		template<class _Ty2> auto& operator=(const _Ty2& _X) { base_class::operator=(_X); return (*this); }

#define MSE_ASYNC_SHAREABLE_IMPL_OBJ_SPECIALIZATION_DEFINITIONS1(class_name) \
		class_name(const class_name&) = default; \
		class_name(class_name&&) = default; \
		MSE_ASYNC_SHAREABLE_IMPL_OBJ_INHERIT_ASSIGNMENT_OPERATOR(class_name);

#if !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)
#define MSE_ASYNC_SHAREABLE_IMPL_OBJ_NATIVE_POINTER_PRIVATE_CONSTRUCTORS1(class_name) \
			class_name(std::nullptr_t) {} \
			class_name() {}
#else // !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)
#define MSE_ASYNC_SHAREABLE_IMPL_OBJ_NATIVE_POINTER_PRIVATE_CONSTRUCTORS1(class_name)
#endif // !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)

	/* Note that because we explicitly define some (private) constructors, default copy and move constructors
	and assignment operators won't be generated, so we have to define those as well. */
#define MSE_ASYNC_SHAREABLE_IMPL_OBJ_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type) \
		template<typename _Ty> \
		class TAsyncShareableObj<specified_type> : public TAsyncShareableObj<mapped_type> { \
		public: \
			typedef TAsyncShareableObj<mapped_type> base_class; \
			MSE_USING(TAsyncShareableObj, base_class); \
			MSE_ASYNC_SHAREABLE_IMPL_OBJ_SPECIALIZATION_DEFINITIONS1(TAsyncShareableObj); \
		private: \
			MSE_ASYNC_SHAREABLE_IMPL_OBJ_NATIVE_POINTER_PRIVATE_CONSTRUCTORS1(TAsyncShareableObj); \
		};

#define MSE_ASYNC_SHAREABLE_IMPL_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type) \
		MSE_ASYNC_SHAREABLE_IMPL_OBJ_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type);

		MSE_ASYNC_SHAREABLE_IMPL_NATIVE_POINTER_SPECIALIZATION(_Ty*, mse::us::impl::TPointerForLegacy<_Ty>);
		MSE_ASYNC_SHAREABLE_IMPL_NATIVE_POINTER_SPECIALIZATION(_Ty* const, const mse::us::impl::TPointerForLegacy<_Ty>);

#ifdef MSEPRIMITIVES_H

#define MSE_ASYNC_SHAREABLE_IMPL_OBJ_INTEGRAL_SPECIALIZATION(integral_type) \
		template<> \
		class TAsyncShareableObj<integral_type> : public TAsyncShareableObj<mse::TInt<integral_type>> { \
		public: \
			typedef TAsyncShareableObj<mse::TInt<integral_type>> base_class; \
			MSE_USING(TAsyncShareableObj, base_class); \
		};

#define MSE_ASYNC_SHAREABLE_IMPL_INTEGRAL_SPECIALIZATION(integral_type) \
		MSE_ASYNC_SHAREABLE_IMPL_OBJ_INTEGRAL_SPECIALIZATION(integral_type); \
		MSE_ASYNC_SHAREABLE_IMPL_OBJ_INTEGRAL_SPECIALIZATION(typename std::add_const<integral_type>::type);

		MSE_IMPL_APPLY_MACRO_FUNCTION_TO_EACH_OF_THE_INTEGER_TYPES(MSE_ASYNC_SHAREABLE_IMPL_INTEGRAL_SPECIALIZATION)

#endif /*MSEPRIMITIVES_H*/


#define MSE_ASYNC_PASSABLE_IMPL_OBJ_INHERIT_ASSIGNMENT_OPERATOR(class_name) \
		auto& operator=(class_name&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); } \
		auto& operator=(const class_name& _X) { base_class::operator=(_X); return (*this); } \
		template<class _Ty2> auto& operator=(_Ty2&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); } \
		template<class _Ty2> auto& operator=(const _Ty2& _X) { base_class::operator=(_X); return (*this); }

#define MSE_ASYNC_PASSABLE_IMPL_OBJ_SPECIALIZATION_DEFINITIONS1(class_name) \
		class_name(const class_name&) = default; \
		class_name(class_name&&) = default; \
		MSE_ASYNC_PASSABLE_IMPL_OBJ_INHERIT_ASSIGNMENT_OPERATOR(class_name);

#if !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)
#define MSE_ASYNC_PASSABLE_IMPL_OBJ_NATIVE_POINTER_PRIVATE_CONSTRUCTORS1(class_name) \
			class_name(std::nullptr_t) {} \
			class_name() {}
#else // !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)
#define MSE_ASYNC_PASSABLE_IMPL_OBJ_NATIVE_POINTER_PRIVATE_CONSTRUCTORS1(class_name)
#endif // !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)

		/* Note that because we explicitly define some (private) constructors, default copy and move constructors
		and assignment operators won't be generated, so we have to define those as well. */
#define MSE_ASYNC_PASSABLE_IMPL_OBJ_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type) \
		template<typename _Ty> \
		class TAsyncPassableObj<specified_type> : public TAsyncPassableObj<mapped_type> { \
		public: \
			typedef TAsyncPassableObj<mapped_type> base_class; \
			MSE_USING(TAsyncPassableObj, base_class); \
			MSE_ASYNC_PASSABLE_IMPL_OBJ_SPECIALIZATION_DEFINITIONS1(TAsyncPassableObj); \
		private: \
			MSE_ASYNC_PASSABLE_IMPL_OBJ_NATIVE_POINTER_PRIVATE_CONSTRUCTORS1(TAsyncPassableObj); \
		};

#define MSE_ASYNC_PASSABLE_IMPL_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type) \
		MSE_ASYNC_PASSABLE_IMPL_OBJ_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type);

		MSE_ASYNC_PASSABLE_IMPL_NATIVE_POINTER_SPECIALIZATION(_Ty*, mse::us::impl::TPointerForLegacy<_Ty>);
		MSE_ASYNC_PASSABLE_IMPL_NATIVE_POINTER_SPECIALIZATION(_Ty* const, const mse::us::impl::TPointerForLegacy<_Ty>);

#ifdef MSEPRIMITIVES_H

#define MSE_ASYNC_PASSABLE_IMPL_OBJ_INTEGRAL_SPECIALIZATION(integral_type) \
		template<> \
		class TAsyncPassableObj<integral_type> : public TAsyncPassableObj<mse::TInt<integral_type>> { \
		public: \
			typedef TAsyncPassableObj<mse::TInt<integral_type>> base_class; \
			MSE_USING(TAsyncPassableObj, base_class); \
		};

#define MSE_ASYNC_PASSABLE_IMPL_INTEGRAL_SPECIALIZATION(integral_type) \
		MSE_ASYNC_PASSABLE_IMPL_OBJ_INTEGRAL_SPECIALIZATION(integral_type); \
		MSE_ASYNC_PASSABLE_IMPL_OBJ_INTEGRAL_SPECIALIZATION(typename std::add_const<integral_type>::type);

		MSE_IMPL_APPLY_MACRO_FUNCTION_TO_EACH_OF_THE_INTEGER_TYPES(MSE_ASYNC_PASSABLE_IMPL_INTEGRAL_SPECIALIZATION)

#endif /*MSEPRIMITIVES_H*/


#define MSE_ASYNC_SHAREABLE_AND_PASSABLE_IMPL_OBJ_INHERIT_ASSIGNMENT_OPERATOR(class_name) \
		auto& operator=(class_name&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); } \
		auto& operator=(const class_name& _X) { base_class::operator=(_X); return (*this); } \
		template<class _Ty2> auto& operator=(_Ty2&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); } \
		template<class _Ty2> auto& operator=(const _Ty2& _X) { base_class::operator=(_X); return (*this); }

#define MSE_ASYNC_SHAREABLE_AND_PASSABLE_IMPL_OBJ_SPECIALIZATION_DEFINITIONS1(class_name) \
		class_name(const class_name&) = default; \
		class_name(class_name&&) = default; \
		MSE_ASYNC_SHAREABLE_AND_PASSABLE_IMPL_OBJ_INHERIT_ASSIGNMENT_OPERATOR(class_name);

#if !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)
#define MSE_ASYNC_SHAREABLE_AND_PASSABLE_IMPL_OBJ_NATIVE_POINTER_PRIVATE_CONSTRUCTORS1(class_name) \
			class_name(std::nullptr_t) {} \
			class_name() {}
#else // !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)
#define MSE_ASYNC_SHAREABLE_AND_PASSABLE_IMPL_OBJ_NATIVE_POINTER_PRIVATE_CONSTRUCTORS1(class_name)
#endif // !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)

		/* Note that because we explicitly define some (private) constructors, default copy and move constructors
		and assignment operators won't be generated, so we have to define those as well. */
#define MSE_ASYNC_SHAREABLE_AND_PASSABLE_IMPL_OBJ_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type) \
		template<typename _Ty> \
		class TAsyncShareableAndPassableObj<specified_type> : public TAsyncShareableAndPassableObj<mapped_type> { \
		public: \
			typedef TAsyncShareableAndPassableObj<mapped_type> base_class; \
			MSE_USING(TAsyncShareableAndPassableObj, base_class); \
			MSE_ASYNC_SHAREABLE_AND_PASSABLE_IMPL_OBJ_SPECIALIZATION_DEFINITIONS1(TAsyncShareableAndPassableObj); \
		private: \
			MSE_ASYNC_SHAREABLE_AND_PASSABLE_IMPL_OBJ_NATIVE_POINTER_PRIVATE_CONSTRUCTORS1(TAsyncShareableAndPassableObj); \
		};

#define MSE_ASYNC_SHAREABLE_AND_PASSABLE_IMPL_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type) \
		MSE_ASYNC_SHAREABLE_AND_PASSABLE_IMPL_OBJ_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type);

		MSE_ASYNC_SHAREABLE_AND_PASSABLE_IMPL_NATIVE_POINTER_SPECIALIZATION(_Ty*, mse::us::impl::TPointerForLegacy<_Ty>);
		MSE_ASYNC_SHAREABLE_AND_PASSABLE_IMPL_NATIVE_POINTER_SPECIALIZATION(_Ty* const, const mse::us::impl::TPointerForLegacy<_Ty>);

#ifdef MSEPRIMITIVES_H

#define MSE_ASYNC_SHAREABLE_AND_PASSABLE_IMPL_OBJ_INTEGRAL_SPECIALIZATION(integral_type) \
		template<> \
		class TAsyncShareableAndPassableObj<integral_type> : public TAsyncShareableAndPassableObj<mse::TInt<integral_type>> { \
		public: \
			typedef TAsyncShareableAndPassableObj<mse::TInt<integral_type>> base_class; \
			MSE_USING(TAsyncShareableAndPassableObj, base_class); \
		};

#define MSE_ASYNC_SHAREABLE_AND_PASSABLE_IMPL_INTEGRAL_SPECIALIZATION(integral_type) \
		MSE_ASYNC_SHAREABLE_AND_PASSABLE_IMPL_OBJ_INTEGRAL_SPECIALIZATION(integral_type); \
		MSE_ASYNC_SHAREABLE_AND_PASSABLE_IMPL_OBJ_INTEGRAL_SPECIALIZATION(typename std::add_const<integral_type>::type);

		MSE_IMPL_APPLY_MACRO_FUNCTION_TO_EACH_OF_THE_INTEGER_TYPES(MSE_ASYNC_SHAREABLE_AND_PASSABLE_IMPL_INTEGRAL_SPECIALIZATION)

#endif /*MSEPRIMITIVES_H*/

		/* end of template specializations */


		namespace impl {
			template<typename _TROy>
			auto make_async_shareable_helper1(std::true_type, const _TROy& src) {
				return src;
			}
			template<typename _TROy>
			auto make_async_shareable_helper1(std::false_type, const _TROy& src) -> TAsyncShareableObj<_TROy> {
				return src;
			}
			template<typename _TROy>
			auto make_async_shareable_helper1(std::true_type, _TROy&& src) {
				return src;
			}
			template<typename _TROy>
			auto make_async_shareable_helper1(std::false_type, _TROy&& src) -> TAsyncShareableObj<_TROy> {
				return MSE_FWD(src);
			}
		}
		template<typename _TROy>
		auto make_async_shareable(const _TROy& src) {
			return impl::make_async_shareable_helper1(typename mse::impl::is_marked_as_shareable_msemsearray<_TROy>::type(), src);
		}
		template<typename _TROy>
		auto make_async_shareable(_TROy&& src) {
			return impl::make_async_shareable_helper1(typename mse::impl::is_marked_as_shareable_msemsearray<_TROy>::type(), MSE_FWD(src));
		}

		namespace impl {
			template<typename _TROy>
			auto make_async_passable_helper1(std::true_type, const _TROy& src) {
				return src;
			}
			template<typename _TROy>
			auto make_async_passable_helper1(std::false_type, const _TROy& src) -> TAsyncPassableObj<_TROy> {
				return src;
			}
			template<typename _TROy>
			auto make_async_passable_helper1(std::true_type, _TROy&& src) {
				return src;
			}
			template<typename _TROy>
			auto make_async_passable_helper1(std::false_type, _TROy&& src) -> TAsyncPassableObj<_TROy> {
				return MSE_FWD(src);
			}
		}
		template<typename _TROy>
		auto make_async_passable(const _TROy& src) {
			return impl::make_async_passable_helper1(typename mse::impl::is_marked_as_passable_msemsearray<_TROy>::type(), src);
		}
		template<typename _TROy>
		auto make_async_passable(_TROy&& src) {
			return impl::make_async_passable_helper1(typename mse::impl::is_marked_as_passable_msemsearray<_TROy>::type(), MSE_FWD(src));
		}

		namespace impl {
			template<typename _TROy>
			auto make_async_shareable_and_passable_helper1(std::true_type, const _TROy& src) {
				return src;
			}
			template<typename _TROy>
			auto make_async_shareable_and_passable_helper1(std::false_type, const _TROy& src) -> TAsyncShareableAndPassableObj<_TROy> {
				return src;
			}
			template<typename _TROy>
			auto make_async_shareable_and_passable_helper1(std::true_type, _TROy&& src) {
				return src;
			}
			template<typename _TROy>
			auto make_async_shareable_and_passable_helper1(std::false_type, _TROy&& src) -> TAsyncShareableAndPassableObj<_TROy> {
				return MSE_FWD(src);
			}
		}
		template<typename _TROy>
		auto make_async_shareable_and_passable(const _TROy& src) {
			return impl::make_async_shareable_and_passable_helper1(typename mse::impl::is_marked_as_shareable_and_passable_msemsearray<_TROy>::type(), src);
		}
		template<typename _TROy>
		auto make_async_shareable_and_passable(_TROy&& src) {
			return impl::make_async_shareable_and_passable_helper1(typename mse::impl::is_marked_as_shareable_and_passable_msemsearray<_TROy>::type(), MSE_FWD(src));
		}
	}

	namespace us {
		template<typename _TROy> using TUserDeclaredAsyncShareableObj = mse::rsv::TAsyncShareableObj<_TROy>;
		template<typename _TROy> using TUserDeclaredAsyncPassableObj = mse::rsv::TAsyncPassableObj<_TROy>;
		template<typename _TROy> using TUserDeclaredAsyncShareableAndPassableObj = mse::rsv::TAsyncShareableAndPassableObj<_TROy>;
	}


	/* TXScopeExclusiveStrongPointerStoreForAccessControl<> is a data type that stores an exclusive strong pointer. From this data type you
	can obtain const, non-const and exclusive pointers. */
	template<typename _TExclusiveStrongPointer, class _TAccessMutex/* = non_thread_safe_recursive_shared_timed_mutex*/>
	class TXScopeExclusiveStrongPointerStoreForAccessControl : public mse::us::impl::XScopeTagBase
		, MSE_INHERIT_COMMON_XSCOPE_POINTER_TAG_BASE_SET_FROM(_TExclusiveStrongPointer, TXScopeExclusiveStrongPointerStoreForAccessControl<_TExclusiveStrongPointer>)
	{
	public:
		TXScopeExclusiveStrongPointerStoreForAccessControl(const TXScopeExclusiveStrongPointerStoreForAccessControl&) = delete;
#ifdef MSE_HAS_CXX17
		TXScopeExclusiveStrongPointerStoreForAccessControl(TXScopeExclusiveStrongPointerStoreForAccessControl&&) = delete;
#else // MSE_HAS_CXX17
		TXScopeExclusiveStrongPointerStoreForAccessControl(TXScopeExclusiveStrongPointerStoreForAccessControl&& src)
			: m_stored_ptr(MSE_FWD(src).m_stored_ptr) {}
#endif // MSE_HAS_CXX17

		typedef mse::impl::remove_reference_t<decltype(*std::declval<_TExclusiveStrongPointer>())> target_type;
		TXScopeExclusiveStrongPointerStoreForAccessControl(_TExclusiveStrongPointer&& stored_ptr) : m_stored_ptr(MSE_FWD(stored_ptr)) {
			*m_stored_ptr; /* Just verifying that stored_ptr points to a valid target. */
		}
		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TXScopeExclusiveStrongPointerStoreForAccessControl() {
			mse::impl::is_valid_if_strong_pointer<_TExclusiveStrongPointer>::no_op();
			mse::impl::is_valid_if_exclusive_pointer<_TExclusiveStrongPointer>::no_op();

			MSE_TRY {
				m_mutex1.nonrecursive_lock();
				//m_mutex1.nonrecursive_unlock();
			}
			MSE_CATCH_ANY {
				/* It would be unsafe to allow this object to be destroyed as there are outstanding references to this object (in
				this thread). */
#ifdef MSE_CUSTOM_FATAL_ERROR_MESSAGE_HANDLER
				MSE_CUSTOM_FATAL_ERROR_MESSAGE_HANDLER("Fatal Error: mse::us::impl::TAccessControlledObjBase<> destructed with outstanding references in the same thread \n");
#endif // MSE_CUSTOM_FATAL_ERROR_MESSAGE_HANDLER
				std::cerr << "\n\nFatal Error: mse::us::impl::TAccessControlledObjBase<> destructed with outstanding references in the same thread \n\n";
				assert(false); std::terminate();
			}
		}

		TXScopeAccessControlledPointer<target_type, _TAccessMutex> xscope_pointer() const {
			return TXScopeAccessControlledPointer<target_type, _TAccessMutex>(*m_stored_ptr, m_mutex1);
		}
		mse::xscope_optional<TXScopeAccessControlledPointer<target_type, _TAccessMutex>> xscope_try_pointer() const {
			mse::xscope_optional<TXScopeAccessControlledPointer<target_type, _TAccessMutex>> retval(TXScopeAccessControlledPointer<target_type, _TAccessMutex>(*m_stored_ptr, m_mutex1, std::try_to_lock));
			if (!((*retval).is_valid())) {
				return{};
			}
			return retval;
		}
		template<class _Rep, class _Period>
		mse::xscope_optional<TXScopeAccessControlledPointer<target_type, _TAccessMutex>> xscope_try_pointer_for(const std::chrono::duration<_Rep, _Period>& _Rel_time) const {
			mse::xscope_optional<TXScopeAccessControlledPointer<target_type, _TAccessMutex>> retval(TXScopeAccessControlledPointer<target_type, _TAccessMutex>(*m_stored_ptr, m_mutex1, std::try_to_lock, _Rel_time));
			if (!((*retval).is_valid())) {
				return{};
			}
			return retval;
		}
		template<class _Clock, class _Duration>
		mse::xscope_optional<TXScopeAccessControlledPointer<target_type, _TAccessMutex>> xscope_try_pointer_until(const std::chrono::time_point<_Clock, _Duration>& _Abs_time) const {
			mse::xscope_optional<TXScopeAccessControlledPointer<target_type, _TAccessMutex>> retval(TXScopeAccessControlledPointer<target_type, _TAccessMutex>(*m_stored_ptr, m_mutex1, std::try_to_lock, _Abs_time));
			if (!((*retval).is_valid())) {
				return{};
			}
			return retval;
		}
		TXScopeAccessControlledConstPointer<target_type, _TAccessMutex> xscope_const_pointer() const {
			return TXScopeAccessControlledConstPointer<target_type, _TAccessMutex>(*m_stored_ptr, m_mutex1);
		}
		mse::xscope_optional<TXScopeAccessControlledConstPointer<target_type, _TAccessMutex>> xscope_try_const_pointer() const {
			mse::xscope_optional<TXScopeAccessControlledConstPointer<target_type, _TAccessMutex>> retval(TXScopeAccessControlledConstPointer<target_type, _TAccessMutex>(*m_stored_ptr, m_mutex1, std::try_to_lock));
			if (!((*retval).is_valid())) {
				return{};
			}
			return retval;
		}
		template<class _Rep, class _Period>
		mse::xscope_optional<TXScopeAccessControlledConstPointer<target_type, _TAccessMutex>> xscope_try_const_pointer_for(const std::chrono::duration<_Rep, _Period>& _Rel_time) const {
			mse::xscope_optional<TXScopeAccessControlledConstPointer<target_type, _TAccessMutex>> retval(TXScopeAccessControlledConstPointer<target_type, _TAccessMutex>(*m_stored_ptr, m_mutex1, std::try_to_lock, _Rel_time));
			if (!((*retval).is_valid())) {
				return{};
			}
			return retval;
		}
		template<class _Clock, class _Duration>
		mse::xscope_optional<TXScopeAccessControlledConstPointer<target_type, _TAccessMutex>> xscope_try_const_pointer_until(const std::chrono::time_point<_Clock, _Duration>& _Abs_time) const {
			mse::xscope_optional<TXScopeAccessControlledConstPointer<target_type, _TAccessMutex>> retval(TXScopeAccessControlledConstPointer<target_type, _TAccessMutex>(*m_stored_ptr, m_mutex1, std::try_to_lock, _Abs_time));
			if (!((*retval).is_valid())) {
				return{};
			}
			return retval;
		}
		/* Note that an exclusive_pointer cannot coexist with any other lock_ptrs (targeting the same object), including ones in
		the same thread. Thus, using exclusive_pointers without sufficient care introduces the potential for exceptions (in a way
		that sticking to (regular) pointers doesn't). */
		TXScopeAccessControlledExclusivePointer<target_type, _TAccessMutex> xscope_exclusive_pointer() const {
			return TXScopeAccessControlledExclusivePointer<target_type, _TAccessMutex>(*m_stored_ptr, m_mutex1);
		}

		/* This type can be safely used as a function return value if the element it contains is also safely returnable. */
		template<class _Ty2 = _TExclusiveStrongPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _TExclusiveStrongPointer>::value) && (
			(std::integral_constant<bool, mse::impl::HasXScopeReturnableTagMethod<_Ty2>::Has>()) || (mse::impl::is_potentially_not_xscope<_Ty2>::value)
			)> MSE_IMPL_EIS >
			void xscope_returnable_tag() const {} /* Indication that this type is can be used as a function return value. */

	private:
		typedef recursive_shared_mutex_wrapped<_TAccessMutex> _TWrappedAccessMutex;
		mutable _TWrappedAccessMutex m_mutex1;

		_TExclusiveStrongPointer m_stored_ptr;
	};

#ifdef MSE_HAS_CXX17
	/* deduction guides */
	template<typename _TExclusiveStrongPointer>
	TXScopeExclusiveStrongPointerStoreForAccessControl(_TExclusiveStrongPointer&&)
		->TXScopeExclusiveStrongPointerStoreForAccessControl<_TExclusiveStrongPointer>;
#endif /* MSE_HAS_CXX17 */

	template<typename _TExclusiveStrongPointer, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	TXScopeExclusiveStrongPointerStoreForAccessControl<_TExclusiveStrongPointer, _TAccessMutex> make_xscope_exclusive_strong_pointer_store_for_access_control(_TExclusiveStrongPointer&& stored_ptr) {
		return TXScopeExclusiveStrongPointerStoreForAccessControl<_TExclusiveStrongPointer, _TAccessMutex>(MSE_FWD(stored_ptr));
	}

	/* This is just an alias of the TXScopeExclusiveStrongPointerStoreForAccessControl<> class for use as a function parameter type. */
	template<typename _TExclusiveStrongPointer, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	using TXScopeExclusiveStrongPointerStoreForAccessControlFParam = TXScopeExclusiveStrongPointerStoreForAccessControl<_TExclusiveStrongPointer, _TAccessMutex>;

	template<typename _TExclusiveStrongPointer> class TXScopeExclusiveStrongPointerStoreForSharing;

	template<typename _Ty, class _TAccessMutex/* = non_thread_safe_shared_mutex*/> class TXScopeExclusiveWriterObjPointerStoreForSharing;

	template<typename _Ty>
	class TXScopePassablePointer : public mse::us::impl::XScopeTagBase, public mse::us::impl::ContainsNonOwningScopeReferenceTagBase, public mse::us::impl::StrongExclusivePointerTagBase {
	public:
		TXScopePassablePointer(const TXScopePassablePointer& src) = delete;
		TXScopePassablePointer(TXScopePassablePointer&& src) = default;

		operator bool() const {
			return (nullptr != m_obj_ptr);
		}
		auto& operator*() const {
			return *m_obj_ptr;
		}
		auto operator->() const {
			return std::addressof(*m_obj_ptr);
		}

		/* This pointer is safely "async passable" if its target type is "async shareable". */
		template<class _Ty2 = _Ty, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _Ty>::value)
			&& (mse::impl::is_marked_as_xscope_shareable_msemsearray<_Ty2>::value)> MSE_IMPL_EIS >
		void xscope_async_passable_tag() const {} /* Indication that this type is eligible to be passed between threads. */

	private:
		TXScopePassablePointer(_Ty& obj_ref) : m_obj_ptr(std::addressof(obj_ref)) {}

		TXScopePassablePointer& operator=(const TXScopePassablePointer& _Right_cref) = delete;
		TXScopePassablePointer& operator=(TXScopePassablePointer&& _Right) = delete;

		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		_Ty* m_obj_ptr = nullptr;

		template<typename _Ty2, class _TAccessMutex2> friend class TXScopeExclusiveWriterObjPointerStoreForSharing;
		template<typename _Ty2> friend class TXScopeExclusiveStrongPointerStoreForSharing;
	};

	template<typename _Ty, class _TAccessMutex> class TXScopeExclusiveWriterObjConstPointerStoreForSharing;

	template<typename _Ty>
	class TXScopePassableConstPointer : public mse::us::impl::XScopeTagBase, public mse::us::impl::ContainsNonOwningScopeReferenceTagBase {
	public:
		TXScopePassableConstPointer(const TXScopePassableConstPointer& src) = default;
		TXScopePassableConstPointer(TXScopePassableConstPointer&& src) = default;

		operator bool() const {
			return (nullptr != m_obj_ptr);
		}
		const _Ty& operator*() const {
			return *m_obj_ptr;
		}
		const _Ty* operator->() const {
			return std::addressof(*m_obj_ptr);
		}

		/* This pointer is safely "async passable" if its target type is "async shareable". */
		template<class _Ty2 = _Ty, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _Ty>::value)
			&& (mse::impl::is_marked_as_xscope_shareable_msemsearray<_Ty2>::value)> MSE_IMPL_EIS >
		void xscope_async_passable_tag() const {} /* Indication that this type is eligible to be passed between threads. */

	private:
		TXScopePassableConstPointer(const _Ty& obj_ref) : m_obj_ptr(std::addressof(obj_ref)) {}

		TXScopePassableConstPointer& operator=(const TXScopePassableConstPointer& _Right_cref) = delete;
		TXScopePassableConstPointer& operator=(TXScopePassableConstPointer&& _Right) = delete;

		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		const _Ty* m_obj_ptr = nullptr;

		template<typename _Ty2, class _TAccessMutex2> friend class TXScopeExclusiveWriterObjConstPointerStoreForSharing;
		template<typename _Ty2> friend class TXScopeExclusiveStrongPointerStoreForSharing;
	};

	/* TXScopeExclusiveStrongPointerStoreForSharing<> is a data type that stores an exclusive strong pointer. From this data type you
	can obtain a "scope shareable pointer" which can be safely passed to a scope thread. */
	template<typename _TExclusiveStrongPointer>
	class TXScopeExclusiveStrongPointerStoreForSharing : public mse::us::impl::XScopeTagBase
		, MSE_INHERIT_COMMON_XSCOPE_POINTER_TAG_BASE_SET_FROM(_TExclusiveStrongPointer, TXScopeExclusiveStrongPointerStoreForSharing<_TExclusiveStrongPointer>)
	{
	public:
		TXScopeExclusiveStrongPointerStoreForSharing(const TXScopeExclusiveStrongPointerStoreForSharing&) = delete;
		TXScopeExclusiveStrongPointerStoreForSharing(TXScopeExclusiveStrongPointerStoreForSharing&& src)
			: m_stored_ptr(MSE_FWD(src).m_stored_ptr) {}

		typedef mse::impl::remove_reference_t<decltype(*std::declval<_TExclusiveStrongPointer>())> target_type;
		TXScopeExclusiveStrongPointerStoreForSharing(_TExclusiveStrongPointer&& stored_ptr) : m_stored_ptr(MSE_FWD(stored_ptr)) {
			*m_stored_ptr; /* Just verifying that stored_ptr points to a valid target. */
		}
		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TXScopeExclusiveStrongPointerStoreForSharing() {
			mse::impl::is_valid_if_strong_pointer<_TExclusiveStrongPointer>::no_op();
		}

		auto xscope_passable_pointer() const & {
			m_non_thread_safe_shared_mutex.lock();
			return TXScopePassablePointer<target_type>(*m_stored_ptr);
		}
		void xscope_passable_pointer() const && = delete;

		auto xscope_passable_const_pointer() const & {
			m_non_thread_safe_shared_mutex.lock_shared();
			return TXScopePassableConstPointer<target_type>(*m_stored_ptr);
		}
		void xscope_passable_const_pointer() const && = delete;

		/* This type can be safely used as a function return value if the element it contains is also safely returnable. */
		template<class _Ty2 = _TExclusiveStrongPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _TExclusiveStrongPointer>::value) && (
			(std::integral_constant<bool, mse::impl::HasXScopeReturnableTagMethod<_Ty2>::Has>()) || (mse::impl::is_potentially_not_xscope<_Ty2>::value)
			)> MSE_IMPL_EIS >
			void xscope_returnable_tag() const {} /* Indication that this type is can be used as a function return value. */

	private:
		mutable non_thread_safe_shared_mutex m_non_thread_safe_shared_mutex;
		_TExclusiveStrongPointer m_stored_ptr;
	};

	template<typename _TExclusiveStrongPointer>
	TXScopeExclusiveStrongPointerStoreForSharing<_TExclusiveStrongPointer> make_xscope_exclusive_strong_pointer_store_for_sharing(_TExclusiveStrongPointer&& stored_ptr) {
		return TXScopeExclusiveStrongPointerStoreForSharing<_TExclusiveStrongPointer>(MSE_FWD(stored_ptr));
	}

	/* This class is just a version of the TXScopeExclusiveStrongPointerStoreForSharing<> class for use as a function parameter type. */
	template<typename _TExclusiveStrongPointer>
	class TXScopeExclusiveStrongPointerStoreForSharingFParam : public TXScopeExclusiveStrongPointerStoreForSharing<_TExclusiveStrongPointer> {
		typedef TXScopeExclusiveStrongPointerStoreForSharing<_TExclusiveStrongPointer> base_class;

		TXScopeExclusiveStrongPointerStoreForSharingFParam(const TXScopeExclusiveStrongPointerStoreForSharingFParam&) = delete;
		TXScopeExclusiveStrongPointerStoreForSharingFParam(TXScopeExclusiveStrongPointerStoreForSharingFParam&&) = delete;
		MSE_USING(TXScopeExclusiveStrongPointerStoreForSharingFParam, base_class);
	};

	template <typename _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	class TXScopeACOLockerForSharing
		: public TXScopeExclusiveStrongPointerStoreForSharing<decltype(std::declval<mse::TXScopeAccessControlledObj<_Ty, _TAccessMutex> >().exclusive_pointer())> {
	public:
		typedef TXScopeExclusiveStrongPointerStoreForSharing<decltype(std::declval<mse::TXScopeAccessControlledObj<_Ty, _TAccessMutex> >().exclusive_pointer())> base_class;
		typedef decltype(std::declval<mse::TXScopeAccessControlledObj<_Ty, _TAccessMutex> >().exclusive_pointer()) _TExclusiveWritePointer;
		typedef mse::TXScopeFixedPointer<mse::TXScopeAccessControlledObj<_Ty, _TAccessMutex> > xsac_obj_xscpptr_t;
		typedef mse::TXScopeFixedPointer<mse::TAccessControlledObj<_Ty, _TAccessMutex> > ac_obj_xscpptr_t;

		TXScopeACOLockerForSharing(const TXScopeACOLockerForSharing&) = delete;
		TXScopeACOLockerForSharing(TXScopeACOLockerForSharing&&) = default;

		~TXScopeACOLockerForSharing() {
			/* This is just a no-op function that will cause a compile error when _Ty is not an eligible type. */
			//valid_if_Ty_is_marked_as_xscope_shareable();
		}

		static auto make(const xsac_obj_xscpptr_t& xscpptr) {
			return TXScopeACOLockerForSharing((*xscpptr).exclusive_pointer());
		}
		static auto make(const ac_obj_xscpptr_t& xscpptr) {
			return TXScopeACOLockerForSharing((*xscpptr).exclusive_pointer());
		}

	private:
		/* If _Ty is not "marked" as safe to share among threads (via the presence of the "async_shareable_tag()" member
		function), then the following member function will not instantiate, causing an (intended) compile error. User-defined
		objects can be marked safe to share by wrapping them with rsv::TUserDeclaredAsyncShareableObj<>. */
		template<class _Ty2 = _Ty, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _Ty>::value)
			&& (mse::impl::is_marked_as_xscope_shareable_and_passable_msemsearray<_Ty2>::value)> MSE_IMPL_EIS >
		void valid_if_Ty_is_marked_as_xscope_shareable() const {}

		TXScopeACOLockerForSharing(_TExclusiveWritePointer&& xwptr)
			: base_class(make_xscope_exclusive_strong_pointer_store_for_sharing(MSE_FWD(xwptr))) {}

		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
	};

	template <typename TXScopeAccessControlledObj1>
	auto make_xscope_aco_locker_for_sharing(const mse::TXScopeFixedPointer<TXScopeAccessControlledObj1>& xscpptr)
		-> TXScopeACOLockerForSharing<typename TXScopeAccessControlledObj1::object_type, typename TXScopeAccessControlledObj1::access_mutex_type> {
		return TXScopeACOLockerForSharing<typename TXScopeAccessControlledObj1::object_type, typename TXScopeAccessControlledObj1::access_mutex_type>::make(xscpptr);
	}

#ifndef MSE_SCOPEPOINTER_DISABLED
	template <typename TXScopeAccessControlledObj1>
	auto make_xscope_aco_locker_for_sharing(const mse::TXScopeObjFixedPointer<TXScopeAccessControlledObj1>& xscpptr)
		-> TXScopeACOLockerForSharing<typename TXScopeAccessControlledObj1::object_type, typename TXScopeAccessControlledObj1::access_mutex_type> {
		return TXScopeACOLockerForSharing<typename TXScopeAccessControlledObj1::object_type, typename TXScopeAccessControlledObj1::access_mutex_type>::make(xscpptr);
	}
#endif //!MSE_SCOPEPOINTER_DISABLED
	/* Overloads for rsv::TReturnableFParam<>. */
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_aco_locker_for_sharing)

	/* This class is just a version of the TXScopeACOLockerForSharing<> class for use as a function parameter type. */
	template <typename _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	class TXScopeACOLockerForSharingFParam : public TXScopeACOLockerForSharing<_Ty, _TAccessMutex> {
		typedef TXScopeACOLockerForSharing<_Ty, _TAccessMutex> base_class;

		TXScopeACOLockerForSharingFParam(const TXScopeACOLockerForSharingFParam&) = delete;
		TXScopeACOLockerForSharingFParam(TXScopeACOLockerForSharingFParam&&) = delete;
		MSE_USING(TXScopeACOLockerForSharingFParam, base_class);
	};

	//template<class _Ty> using TExclusiveWriterObj = TAccessControlledObj<_Ty, non_thread_safe_shared_mutex>;

	/* TXScopeExclusiveWriterObj<> is a specialization of TXScopeAccessControlledObj<> for which all non-const pointers are
	exclusive. That is, when a non-const pointer exists, no other pointer may exist. */
	template<class _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	class TXScopeExclusiveWriterObj : public TXScopeAccessControlledObj<_Ty, _TAccessMutex> {
	public:
		typedef TXScopeAccessControlledObj<_Ty, _TAccessMutex> base_class;
		MSE_USING(TXScopeExclusiveWriterObj, base_class);

		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TXScopeExclusiveWriterObj() {
			mse::impl::T_valid_if_is_exclusive_writer_enforcing_mutex_msemsearray<_TAccessMutex>();
		}

		auto xscope_pointer() {
			return base_class::xscope_exclusive_pointer();
		}
		auto xscope_try_pointer() {
			return base_class::xscope_try_exclusive_pointer();
		}
		template<class _Rep, class _Period>
		auto xscope_try_pointer_for(const std::chrono::duration<_Rep, _Period>& _Rel_time) {
			return base_class::xscope_try_exclusive_pointer_for(_Rel_time);
		}
		template<class _Clock, class _Duration>
		auto xscope_try_pointer_until(const std::chrono::time_point<_Clock, _Duration>& _Abs_time) {
			return base_class::xscope_try_exclusive_pointer_until(_Abs_time);
		}

		auto pointer() {
			return base_class::exclusive_pointer();
		}
		auto try_pointer() {
			return base_class::try_exclusive_pointer();
		}
		template<class _Rep, class _Period>
		auto try_pointer_for(const std::chrono::duration<_Rep, _Period>& _Rel_time) {
			return base_class::try_exclusive_pointer_for(_Rel_time);
		}
		template<class _Clock, class _Duration>
		auto try_pointer_until(const std::chrono::time_point<_Clock, _Duration>& _Abs_time) {
			return base_class::try_exclusive_pointer_until(_Abs_time);
		}
	};

	template<class _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	auto make_xscope_exclusive_writer(const _Ty& src) {
		return TXScopeExclusiveWriterObj<_Ty, _TAccessMutex>(src);
	}
	template<class _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	auto make_xscope_exclusive_writer(_Ty&& src) {
		return TXScopeExclusiveWriterObj<_Ty, _TAccessMutex>(MSE_FWD(src));
	}

	template<class _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	auto make_xscope_access_controlled_pointer(const mse::TXScopeFixedPointer<TXScopeExclusiveWriterObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_pointer(*xs_ptr); }
	template<class _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	auto make_xscope_access_controlled_pointer(const mse::TXScopeFixedConstPointer<TXScopeExclusiveWriterObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_pointer(*xs_ptr); }

	template<class _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	auto make_xscope_access_controlled_const_pointer(const mse::TXScopeFixedPointer<TXScopeExclusiveWriterObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_const_pointer(*xs_ptr); }
	template<class _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	auto make_xscope_access_controlled_const_pointer(const mse::TXScopeFixedConstPointer<TXScopeExclusiveWriterObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_const_pointer(*xs_ptr); }

	template<class _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	auto make_xscope_access_controlled_exclusive_pointer(const mse::TXScopeFixedPointer<TXScopeExclusiveWriterObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_exclusive_pointer(*xs_ptr); }
	template<class _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	auto make_xscope_access_controlled_exclusive_pointer(const mse::TXScopeFixedConstPointer<TXScopeExclusiveWriterObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_exclusive_pointer(*xs_ptr); }

	/* TExclusiveWriterObj<> is a specialization of TAccessControlledObj<> for which all non-const pointers are
	exclusive. That is, when a non-const pointer exists, no other pointer may exist. */
	template<class _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	class TExclusiveWriterObj : public TAccessControlledObj<_Ty, _TAccessMutex> {
	public:
		typedef TAccessControlledObj<_Ty, _TAccessMutex> base_class;
		MSE_USING(TExclusiveWriterObj, base_class);

		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TExclusiveWriterObj() {
			mse::impl::T_valid_if_is_exclusive_writer_enforcing_mutex_msemsearray<_TAccessMutex>();
		}

		auto pointer() {
			return base_class::exclusive_pointer();
		}
		auto try_pointer() {
			return base_class::try_exclusive_pointer();
		}
		template<class _Rep, class _Period>
		auto try_pointer_for(const std::chrono::duration<_Rep, _Period>& _Rel_time) {
			return base_class::try_exclusive_pointer_for(_Rel_time);
		}
		template<class _Clock, class _Duration>
		auto try_pointer_until(const std::chrono::time_point<_Clock, _Duration>& _Abs_time) {
			return base_class::try_exclusive_pointer_until(_Abs_time);
		}
	};

	template<class _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	auto make_exclusive_writer(const _Ty& src) {
		return TExclusiveWriterObj<_Ty, _TAccessMutex>(src);
	}
	template<class _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	auto make_exclusive_writer(_Ty&& src) {
		return TExclusiveWriterObj<_Ty, _TAccessMutex>(MSE_FWD(src));
	}

	template<class _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	auto make_xscope_access_controlled_pointer(const mse::TXScopeFixedPointer<TExclusiveWriterObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_pointer(*xs_ptr); }
	template<class _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	auto make_xscope_access_controlled_pointer(const mse::TXScopeFixedConstPointer<TExclusiveWriterObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_pointer(*xs_ptr); }

	template<class _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	auto make_xscope_access_controlled_const_pointer(const mse::TXScopeFixedPointer<TExclusiveWriterObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_const_pointer(*xs_ptr); }
	template<class _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	auto make_xscope_access_controlled_const_pointer(const mse::TXScopeFixedConstPointer<TExclusiveWriterObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_const_pointer(*xs_ptr); }

	template<class _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	auto make_xscope_access_controlled_exclusive_pointer(const mse::TXScopeFixedPointer<TExclusiveWriterObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_exclusive_pointer(*xs_ptr); }
	template<class _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	auto make_xscope_access_controlled_exclusive_pointer(const mse::TXScopeFixedConstPointer<TExclusiveWriterObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_exclusive_pointer(*xs_ptr); }

	template<class _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	auto make_access_controlled_pointer(const mse::TXScopeFixedPointer<TExclusiveWriterObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_access_controlled_pointer(*xs_ptr); }
	template<class _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	auto make_access_controlled_pointer(const mse::TXScopeFixedConstPointer<TExclusiveWriterObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_access_controlled_pointer(*xs_ptr); }

	template<class _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	auto make_access_controlled_const_pointer(const mse::TXScopeFixedPointer<TExclusiveWriterObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_access_controlled_const_pointer(*xs_ptr); }
	template<class _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	auto make_access_controlled_const_pointer(const mse::TXScopeFixedConstPointer<TExclusiveWriterObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_access_controlled_const_pointer(*xs_ptr); }

	template<class _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	auto make_access_controlled_exclusive_pointer(const mse::TXScopeFixedPointer<TExclusiveWriterObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_access_controlled_exclusive_pointer(*xs_ptr); }
	template<class _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	auto make_access_controlled_exclusive_pointer(const mse::TXScopeFixedConstPointer<TExclusiveWriterObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_access_controlled_exclusive_pointer(*xs_ptr); }

	/* It should be safe to obtain a scope pointer to target of an lvalue (but not rvalue) scope iterator based on
	a TXScopeAccessControlledConstPointer<>. While the TXScopeAccessControlledConstPointer<> exists (inside the scope iterator)
	it ensures that the container (exists and) is not modified (in a away that might destroy the target element). */
	template <typename _TRAContainer, class _TAccessMutex>
	auto xscope_const_pointer(const mse::TXScopeRAConstIterator<mse::TXScopeAccessControlledConstPointer<_TRAContainer, _TAccessMutex> >& iter) {
		mse::impl::T_valid_if_is_exclusive_writer_enforcing_mutex_msemsearray<_TAccessMutex>();
		return mse::us::unsafe_make_xscope_const_pointer_to(*iter);
	}
	template <typename _TRAContainer, class _TAccessMutex>
	auto xscope_const_pointer(mse::TXScopeRAConstIterator<mse::TXScopeAccessControlledConstPointer<_TRAContainer, _TAccessMutex> >&& iter) = delete;
	template <typename _TRAContainer, class _TAccessMutex>
	auto xscope_const_pointer(const mse::TXScopeFixedConstPointer<mse::TXScopeRAConstIterator<mse::TXScopeAccessControlledConstPointer<_TRAContainer, _TAccessMutex> > >& iter_xscptr) {
		mse::impl::T_valid_if_is_exclusive_writer_enforcing_mutex_msemsearray<_TAccessMutex>();
		return mse::us::unsafe_make_xscope_const_pointer_to(*(*iter_xscptr));
	}
	template <typename _TRAContainer, class _TAccessMutex>
	auto xscope_const_pointer(const mse::TXScopeFixedPointer<mse::TXScopeRAConstIterator<mse::TXScopeAccessControlledConstPointer<_TRAContainer, _TAccessMutex> > >& iter_xscptr) {
		mse::impl::T_valid_if_is_exclusive_writer_enforcing_mutex_msemsearray<_TAccessMutex>();
		return mse::us::unsafe_make_xscope_const_pointer_to(*(*iter_xscptr));
	}

	template <typename _TRAContainer, class _TAccessMutex>
	auto xscope_pointer(const mse::TXScopeRAConstIterator<mse::TXScopeAccessControlledConstPointer<_TRAContainer, _TAccessMutex> >& iter) {
		mse::impl::T_valid_if_is_exclusive_writer_enforcing_mutex_msemsearray<_TAccessMutex>();
		return mse::us::unsafe_make_xscope_const_pointer_to(*iter);
	}
	template <typename _TRAContainer, class _TAccessMutex>
	auto xscope_pointer(mse::TXScopeRAConstIterator<mse::TXScopeAccessControlledConstPointer<_TRAContainer, _TAccessMutex> >&& iter) = delete;


	template<typename _Ty, class _TAccessMutex = non_thread_safe_shared_mutex, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_exclusive_writer_enforcing_mutex_msemsearray<_TAccessMutex>::value)> MSE_IMPL_EIS >
	using TXScopeExclusiveWriterObjPointer = TXScopeAccessControlledExclusivePointer<_Ty, _TAccessMutex>;
	template<typename _Ty, class _TAccessMutex = non_thread_safe_shared_mutex, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_exclusive_writer_enforcing_mutex_msemsearray<_TAccessMutex>::value)> MSE_IMPL_EIS >
	using TExclusiveWriterObjPointer = TAccessControlledExclusivePointer<_Ty, _TAccessMutex>;

	template<typename _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	class TXScopeExclusiveWriterObjPointerStoreForSharing : public TXScopeExclusiveStrongPointerStoreForSharing<decltype(std::declval<TExclusiveWriterObj<_Ty, _TAccessMutex> >().pointer())> {
	public:
		MSE_USING(TXScopeExclusiveWriterObjPointerStoreForSharing, TXScopeExclusiveStrongPointerStoreForSharing<decltype(std::declval<TExclusiveWriterObj<_Ty, _TAccessMutex> >().pointer())>);
	};

	/* TXScopeExclusiveWriterObjPointerStoreForSharing<> is a data type that stores a (non-const, exclusive) pointer
	of a TExclusiveWriterObj<>. From this data type you can obtain a "scope shareable pointer" which can be
	safely passed to a scope thread. */
	template<typename _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	TXScopeExclusiveWriterObjPointerStoreForSharing<_Ty, _TAccessMutex> make_xscope_exclusive_writer_obj_pointer_store_for_sharing(TExclusiveWriterObjPointer<_Ty, _TAccessMutex>&& stored_ptr) {
		return TXScopeExclusiveWriterObjPointerStoreForSharing<_Ty, _TAccessMutex>(MSE_FWD(stored_ptr));
	}
	/* deprecated mis-spelling */
	template<typename _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	MSE_DEPRECATED auto make_xscope_exclusive_write_obj_pointer_store(TExclusiveWriterObjPointer<_Ty, _TAccessMutex>&& stored_ptr) {
		return make_xscope_exclusive_writer_obj_pointer_store_for_sharing(MSE_FWD(stored_ptr));
	}

	/* This class is just a version of the TXScopeExclusiveWriterObjPointerStoreForSharing<> class for use as a function parameter type. */
	template<typename _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	class TXScopeExclusiveWriterObjPointerStoreForSharingFParam : public TXScopeExclusiveWriterObjPointerStoreForSharing<_Ty, _TAccessMutex> {
		typedef TXScopeExclusiveWriterObjPointerStoreForSharing<_Ty, _TAccessMutex> base_class;

		TXScopeExclusiveWriterObjPointerStoreForSharingFParam(const TXScopeExclusiveWriterObjPointerStoreForSharingFParam&) = delete;
		TXScopeExclusiveWriterObjPointerStoreForSharingFParam(TXScopeExclusiveWriterObjPointerStoreForSharingFParam&&) = delete;
		MSE_USING(TXScopeExclusiveWriterObjPointerStoreForSharingFParam, base_class);
	};

	template<typename _Ty, class _TAccessMutex = non_thread_safe_shared_mutex, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_exclusive_writer_enforcing_mutex_msemsearray<_TAccessMutex>::value)> MSE_IMPL_EIS >
	using TExclusiveWriterObjConstPointer = TAccessControlledConstPointer<_Ty, _TAccessMutex>;

	/* TXScopeExclusiveWriterObjConstPointerStoreForSharing<> is a data type that stores a const pointer
	of a TExclusiveWriterObj<>. From this data type you can obtain a "scope shareable const pointer" which can be
	safely passed to a scope thread. */
	template<typename _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	class TXScopeExclusiveWriterObjConstPointerStoreForSharing : public mse::us::impl::XScopeTagBase, public mse::us::impl::ContainsNonOwningScopeReferenceTagBase {
	public:
		TXScopeExclusiveWriterObjConstPointerStoreForSharing(const TXScopeExclusiveWriterObjConstPointerStoreForSharing&) = delete;
		TXScopeExclusiveWriterObjConstPointerStoreForSharing(TXScopeExclusiveWriterObjConstPointerStoreForSharing&&) = default;

		TXScopeExclusiveWriterObjConstPointerStoreForSharing(const TExclusiveWriterObjConstPointer<_Ty, _TAccessMutex>& pointer) : m_xwo_pointer(MSE_FWD(pointer)) {}

		auto xscope_passable_const_pointer() const & {
			return TXScopePassableConstPointer<_Ty>(*m_xwo_pointer);
		}
		void xscope_passable_const_pointer() const && = delete;

		/* prefer xscope_passable_const_pointer() */
		auto xscope_passable_pointer() const & {
			return xscope_passable_const_pointer();
		}
		void xscope_passable_pointer() const && = delete;

	private:
		TExclusiveWriterObjConstPointer<_Ty, _TAccessMutex> m_xwo_pointer;
	};

	template<typename _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	TXScopeExclusiveWriterObjConstPointerStoreForSharing<_Ty, _TAccessMutex> make_xscope_exclusive_write_obj_const_pointer_store_for_sharing(const TExclusiveWriterObjConstPointer<_Ty, _TAccessMutex>& stored_ptr) {
		return TXScopeExclusiveWriterObjConstPointerStoreForSharing<_Ty, _TAccessMutex>(stored_ptr);
	}

	/* This class is just a version of the TXScopeExclusiveWriterObjConstPointerStoreForSharing<> class for use as a function parameter type. */
	template<typename _Ty, class _TAccessMutex = non_thread_safe_shared_mutex>
	class TXScopeExclusiveWriterObjConstPointerStoreForSharingFParam : public TXScopeExclusiveWriterObjConstPointerStoreForSharing<_Ty, _TAccessMutex> {
		typedef TXScopeExclusiveWriterObjConstPointerStoreForSharing<_Ty, _TAccessMutex> base_class;

		TXScopeExclusiveWriterObjConstPointerStoreForSharingFParam(const TXScopeExclusiveWriterObjConstPointerStoreForSharingFParam&) = delete;
		TXScopeExclusiveWriterObjConstPointerStoreForSharingFParam(TXScopeExclusiveWriterObjConstPointerStoreForSharingFParam&&) = delete;
		MSE_USING(TXScopeExclusiveWriterObjConstPointerStoreForSharingFParam, base_class);
	};

	/* TXScopeExclusiveStrongPointerStoreForExclusiveWriterAccess<> is a data type that stores an exclusive strong pointer. From this data type you
	can obtain const and non-const pointers that ensure conformance to the "exclusive writer" policy. */
	template<typename _TExclusiveStrongPointer, class _TAccessMutex = non_thread_safe_shared_mutex, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_exclusive_writer_enforcing_mutex_msemsearray<_TAccessMutex>::value)> MSE_IMPL_EIS >
	using TXScopeExclusiveStrongPointerStoreForExclusiveWriterAccess = TXScopeExclusiveStrongPointerStoreForAccessControl<_TExclusiveStrongPointer, _TAccessMutex>;

	template<typename _TExclusiveStrongPointer, class _TAccessMutex = non_thread_safe_shared_mutex, class = MSE_IMPL_ENABLE_IF_NOT_RETURNABLE_FPARAM(_TExclusiveStrongPointer)>
	TXScopeExclusiveStrongPointerStoreForExclusiveWriterAccess<_TExclusiveStrongPointer> make_xscope_exclusive_strong_pointer_store_for_exclusive_writer_access(_TExclusiveStrongPointer&& stored_ptr) {
		return TXScopeExclusiveStrongPointerStoreForExclusiveWriterAccess<_TExclusiveStrongPointer, _TAccessMutex>(MSE_FWD(stored_ptr));
	}
	/* Overloads for rsv::TReturnableFParam<>. */
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_exclusive_strong_pointer_store_for_exclusive_writer_access)

	/* This is just an alias of the TXScopeExclusiveStrongPointerStoreForExclusiveWriterAccess<> class for use as a function parameter type. */
	template<typename _TExclusiveStrongPointer>
	using TXScopeExclusiveStrongPointerStoreForExclusiveWriterAccessFParam = TXScopeExclusiveStrongPointerStoreForExclusiveWriterAccess<_TExclusiveStrongPointer>;


	namespace us {
		namespace impl {
			/* Some iterators are prone to having their target container prematurely deallocated out from under them. In cases where the
			target container is owned by reference counting pointers or "lock" pointers, you can use TStrongFixedIterator<> to weld an
			owning pointer (aka "lease") to the iterator to prevent the target container from being deallocated prematurely. */
			template <class _TIterator, class _TLeaseType>
			class TStrongFixedIterator : public _TIterator
				, public mse::impl::first_or_placeholder_if_base_of_second<mse::us::impl::StrongContainerReferenceHoldingIteratorTagBase, _TIterator, _TIterator/*TStrongFixedIterator<_TIterator, _TLeaseType>*/>
				, std::conditional<(!std::is_base_of<mse::us::impl::StrongPointerTagBase, _TIterator>::value)
					&& ((mse::impl::is_static_structure_iterator<_TIterator>::value) || (mse::impl::is_structure_locking_iterator<_TIterator>::value))
					, mse::us::impl::StrongPointerTagBase, mse::impl::TPlaceHolder<mse::us::impl::StrongPointerTagBase, TStrongFixedIterator<_TIterator, _TLeaseType> > >::type
			{
			public:
				typedef _TIterator base_class;
				TStrongFixedIterator(const TStrongFixedIterator&) = default;
				template<class _TLeaseType2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_TLeaseType2, _TLeaseType>::value> MSE_IMPL_EIS >
				TStrongFixedIterator(const TStrongFixedIterator<_TIterator, _TLeaseType2>&src) : base_class(src), m_lease(src.lease()) {}
				_TLeaseType lease() const { return (*this).m_lease; }

				template <class _TIterator2, class _TLeaseType2>
				static TStrongFixedIterator make(const _TIterator2& src_iterator, const _TLeaseType2& lease) {
					return TStrongFixedIterator(src_iterator, lease);
				}

				//void async_not_shareable_and_not_passable_tag() const {}

			protected:
				TStrongFixedIterator(const _TIterator& src_iterator, const _TLeaseType& lease/* often a reference counting pointer */)
					: base_class(src_iterator), m_lease(lease) {}
			private:
				TStrongFixedIterator& operator=(const TStrongFixedIterator& _Right_cref) = delete;

				_TLeaseType m_lease;

				//friend class TStrongFixedConstIterator<_TIterator, _TLeaseType>;
			};

			template <class _Ty, class _TLeaseType>
			class TStrongFixedIterator<_Ty*, _TLeaseType> : public TStrongFixedIterator<mse::TRAIterator<_Ty*>, _TLeaseType> {
			public:
				typedef TStrongFixedIterator<mse::TRAIterator<_Ty*>, _TLeaseType>base_class;
				MSE_USING(TStrongFixedIterator, base_class);

				//void async_not_shareable_and_not_passable_tag() const {}
			};

			template <class _TIterator, class _TLeaseType>
			TStrongFixedIterator<_TIterator, _TLeaseType> make_strong_iterator(const _TIterator& src_iterator, const _TLeaseType& lease) {
				return TStrongFixedIterator<_TIterator, _TLeaseType>::make(src_iterator, lease);
			}
		}
	}

	template <typename _TExclusiveWritelockPtr> class TXScopeRASectionSplitterXWP;
	template <typename _TExclusiveWritelockPtr> class TRASectionSplitterXWP;

	namespace impl {
		template <typename _TRAIterator>
		class TXScopeSplitterRandomAccessSection : public TXScopeRandomAccessSection<_TRAIterator>
			, public mse::impl::first_or_placeholder_if_base_of_second<mse::us::impl::AsyncNotPassableTagBase, TXScopeRandomAccessSection<_TRAIterator>, TXScopeSplitterRandomAccessSection<_TRAIterator> >
		{
		public:
			typedef TXScopeRandomAccessSection<_TRAIterator> base_class;
			MSE_INHERITED_RANDOM_ACCESS_MEMBER_TYPE_DECLARATIONS(base_class);

			TXScopeSplitterRandomAccessSection(TXScopeSplitterRandomAccessSection&& src) = default;

			/* We will mark this type as safely "async shareable" if the elements it contains are also "async shareable"
			and _TRAIterator is marked as "strong". This is technically unsafe as those criteria may not be sufficient
			to ensure safe "async shareability". */
			template<class value_type2 = value_type, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<value_type2, value_type>::value)
				&& (mse::impl::is_marked_as_shareable_msemsearray<value_type2>::value)
				&& ((std::is_base_of<mse::us::impl::StrongPointerTagBase, _TRAIterator>::value) || (mse::impl::is_potentially_xscope<_TRAIterator>::value))
				> MSE_IMPL_EIS >
			void xscope_async_shareable_tag() const {}

		private:
			TXScopeSplitterRandomAccessSection(const TXScopeSplitterRandomAccessSection& src) = default;
			template <typename _TRAIterator1>
			TXScopeSplitterRandomAccessSection(const TXScopeSplitterRandomAccessSection<_TRAIterator1>& src) : base_class(src) {}

			TXScopeSplitterRandomAccessSection(const _TRAIterator& start_iter, size_type count) : base_class(start_iter, count) {}
			//MSE_USING(TXScopeSplitterRandomAccessSection, base_class);

			TXScopeSplitterRandomAccessSection& operator=(const TXScopeSplitterRandomAccessSection& _Right_cref) = delete;
			MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

			template <typename _TExclusiveWritelockPtr>
			friend class TXScopeRASectionSplitterXWP;
			template<class _Ty, class _TAccessMutex/* = non_thread_safe_recursive_shared_timed_mutex*/>
			friend class mse::us::impl::TAccessControlledObjBase;
		};

		template <typename _TRAIterator>
		class TSplitterRandomAccessSection : public TRandomAccessSection<_TRAIterator>
			, public mse::impl::first_or_placeholder_if_base_of_second<mse::us::impl::AsyncNotPassableTagBase, TRandomAccessSection<_TRAIterator>, TSplitterRandomAccessSection<_TRAIterator> >
		{
		public:
			typedef TRandomAccessSection<_TRAIterator> base_class;
			MSE_INHERITED_RANDOM_ACCESS_MEMBER_TYPE_DECLARATIONS(base_class);

			TSplitterRandomAccessSection(TSplitterRandomAccessSection&& src) = default;
			MSE_IMPL_DESTRUCTOR_PREFIX1 ~TSplitterRandomAccessSection() {
				mse::impl::T_valid_if_not_an_xscope_type<_TRAIterator>();
			}

			/* We will mark this type as safely "async shareable" if the elements it contains are also "async shareable"
			and _TRAIterator is marked as "strong". This is technically unsafe as those criteria may not be sufficient
			to ensure safe "async shareability". */
			template<class value_type2 = value_type, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<value_type2, value_type>::value)
				&& (mse::impl::is_marked_as_shareable_msemsearray<value_type2>::value)
				&& (std::is_base_of<mse::us::impl::StrongPointerTagBase, _TRAIterator>::value)
				> MSE_IMPL_EIS >
				void async_shareable_tag() const {}

		private:
			TSplitterRandomAccessSection(const TSplitterRandomAccessSection& src) = default;
			template <typename _TRAIterator1>
			TSplitterRandomAccessSection(const TSplitterRandomAccessSection<_TRAIterator1>& src) : base_class(src) {}

			TSplitterRandomAccessSection(const _TRAIterator& start_iter, size_type count) : base_class(start_iter, count) {}

			MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

			template <typename _TExclusiveWritelockPtr>
			friend class TRASectionSplitterXWP;
		};
	}

	template <typename _TAccessLease>
	class TSplitterAccessLeaseObj {
	public:
		TSplitterAccessLeaseObj(_TAccessLease&& access_lease)
			: m_access_lease(std::forward<_TAccessLease>(access_lease)) {}
		const _TAccessLease& cref() const {
			return m_access_lease;
		}
	private:
		_TAccessLease m_access_lease;
	};

	template <typename _TExclusiveWritelockPtr>
	class TXScopeRASectionSplitterXWP : public mse::us::impl::XScopeTagBase {
	public:
		typedef _TExclusiveWritelockPtr exclusive_writelock_ptr_t;
		typedef mse::impl::remove_reference_t<decltype(*(std::declval<exclusive_writelock_ptr_t>()))> _TContainer;
		typedef mse::impl::remove_reference_t<decltype(std::declval<_TContainer>()[0])> element_t;
		typedef mse::TXScopeStrongPointerStore<exclusive_writelock_ptr_t> xscope_exclusive_writelock_ptr_store_t;
		typedef mse::TXScopeFixedPointer<_TContainer> container_xsptr_t;
		typedef mse::TXScopeRAIterator<container_xsptr_t> ra_iterator_t;
		typedef mse::impl::TXScopeSplitterRandomAccessSection<ra_iterator_t> xscope_splitter_ra_section_t;
		typedef decltype(std::declval<xscope_splitter_ra_section_t>().size()) size_type;
		typedef mse::TXScopeAccessControlledObj<xscope_splitter_ra_section_t> xscope_aco_splitter_ra_section_t;

		template<typename _TList>
		TXScopeRASectionSplitterXWP(exclusive_writelock_ptr_t&& exclusive_writelock_ptr, const _TList& section_sizes)
			: m_xscope_exclusive_writelock_ptr_store(std::forward<exclusive_writelock_ptr_t>(exclusive_writelock_ptr)) {
			size_t cummulative_size = 0;
			size_t count = 0;
			auto section_begin_it = ra_iterator_t(m_xscope_exclusive_writelock_ptr_store.xscope_ptr());

			for (const auto& section_size : section_sizes) {
				if (0 > section_size) { MSE_THROW(std::range_error("invalid section size - TXScopeRASectionSplitterXWP() - TXScopeRASectionSplitterXWP")); }
				auto section_size_szt = mse::msear_as_a_size_t(section_size);

				m_splitter_aco_ra_section_map.emplace(count, xscope_aco_splitter_ra_section_t(section_begin_it, section_size_szt));

				cummulative_size += section_size_szt;
				section_begin_it += section_size_szt;
				count += 1;
			}
			if (m_xscope_exclusive_writelock_ptr_store.xscope_ptr()->size() > cummulative_size) {
				auto section_size = m_xscope_exclusive_writelock_ptr_store.xscope_ptr()->size() - cummulative_size;
				auto section_size_szt = mse::msear_as_a_size_t(section_size);
				m_splitter_aco_ra_section_map.emplace(count, xscope_aco_splitter_ra_section_t(section_begin_it, section_size_szt));
			}
		}
		TXScopeRASectionSplitterXWP(exclusive_writelock_ptr_t&& exclusive_writelock_ptr, size_t split_index)
			: TXScopeRASectionSplitterXWP(std::forward<exclusive_writelock_ptr_t>(exclusive_writelock_ptr), std::array<size_t, 1>{ {split_index}}) {}
		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TXScopeRASectionSplitterXWP() {
			mse::impl::is_valid_if_exclusive_pointer<exclusive_writelock_ptr_t>::no_op();
		}

		typedef mse::TXScopeFixedPointer<xscope_aco_splitter_ra_section_t> aco_splitter_ra_section_xsptr_t;
		aco_splitter_ra_section_xsptr_t xscope_ptr_to_ra_section_aco(size_t index) {
			return mse::us::unsafe_make_xscope_pointer_to(m_splitter_aco_ra_section_map.at(index));
		}
		aco_splitter_ra_section_xsptr_t xscope_ptr_to_first_ra_section_aco() {
			return mse::us::unsafe_make_xscope_pointer_to(m_splitter_aco_ra_section_map.at(0));
		}
		aco_splitter_ra_section_xsptr_t xscope_ptr_to_second_ra_section_aco() {
			return mse::us::unsafe_make_xscope_pointer_to(m_splitter_aco_ra_section_map.at(1));
		}
	private:
		TXScopeRASectionSplitterXWP(const TXScopeRASectionSplitterXWP& src) = delete;
		TXScopeRASectionSplitterXWP(TXScopeRASectionSplitterXWP&& src) = delete;
		TXScopeRASectionSplitterXWP & operator=(const TXScopeRASectionSplitterXWP& _Right_cref) = delete;
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		xscope_exclusive_writelock_ptr_store_t m_xscope_exclusive_writelock_ptr_store;
		std::unordered_map<size_t, xscope_aco_splitter_ra_section_t> m_splitter_aco_ra_section_map;
	};

	template <typename _TExclusiveWritelockPtr>
	class TRASectionSplitterXWP {
	public:
		typedef _TExclusiveWritelockPtr exclusive_writelock_ptr_t;
		typedef mse::impl::remove_reference_t<decltype(*(std::declval<exclusive_writelock_ptr_t>()))> _TContainer;
		typedef mse::impl::remove_reference_t<decltype(std::declval<_TContainer>()[0])> element_t;
		typedef mse::TRAIterator<_TContainer*> ra_iterator_t;

#ifdef NDEBUG
		/* If the assumptions and implementation are correct, then the run-time checks in the debug branch should be
		totally unnecessary/redundant. Once we're confident in the implmentation, the debug branch can/should be removed. */
#define MSE_TRASECTIONSPLITTERXWP_NDEBUG
#endif // NDEBUG
#ifdef MSE_TRASECTIONSPLITTERXWP_NDEBUG
		typedef TSplitterAccessLeaseObj<exclusive_writelock_ptr_t> access_lease_t;
#else // MSE_TRASECTIONSPLITTERXWP_NDEBUG
		typedef mse::TNDNoradObj<TSplitterAccessLeaseObj<exclusive_writelock_ptr_t> > access_lease_t;
#endif // MSE_TRASECTIONSPLITTERXWP_NDEBUG
		typedef mse::impl::remove_reference_t<decltype(&std::declval<access_lease_t>())> access_lease_ptr_t;

		typedef decltype(adjusted_ra_iterator(std::declval<ra_iterator_t>(), std::declval<access_lease_ptr_t>())) adjusted_ra_iterator_t;
		typedef mse::impl::TSplitterRandomAccessSection<adjusted_ra_iterator_t> splitter_ra_section_t;
		typedef decltype(std::declval<splitter_ra_section_t>().size()) size_type;
		typedef mse::TAccessControlledObj<splitter_ra_section_t> aco_splitter_ra_section_t;

		template<typename _TList>
		TRASectionSplitterXWP(exclusive_writelock_ptr_t&& exclusive_writelock_ptr, const _TList& section_sizes)
			: m_access_lease_obj(std::forward<exclusive_writelock_ptr_t>(exclusive_writelock_ptr)) {
			size_t cummulative_size = 0;
			size_t count = 0;
			//auto section_begin_it = m_access_lease_obj.cref()->begin();
			auto section_begin_it = ra_iterator_t(std::addressof(*(m_access_lease_obj.cref())));

			for (const auto& section_size : section_sizes) {
				if (0 > section_size) { MSE_THROW(std::range_error("invalid section size - TRASectionSplitterXWP() - TRASectionSplitterXWP")); }
				auto section_size_szt = mse::msear_as_a_size_t(section_size);

				auto it1 = adjusted_ra_iterator(section_begin_it, &m_access_lease_obj);
				auto res1 = m_splitter_aco_ra_section_map.emplace(count, aco_splitter_ra_section_t(it1, section_size_szt));

				cummulative_size += section_size_szt;
				section_begin_it += section_size_szt;
				count += 1;
			}
			if (m_access_lease_obj.cref()->size() > cummulative_size) {
				auto section_size = m_access_lease_obj.cref()->size() - cummulative_size;
				auto section_size_szt = mse::msear_as_a_size_t(section_size);
				auto it1 = adjusted_ra_iterator(section_begin_it, &m_access_lease_obj);
				auto res1 = m_splitter_aco_ra_section_map.emplace(count, aco_splitter_ra_section_t(it1, section_size_szt));
			}
		}
		TRASectionSplitterXWP(exclusive_writelock_ptr_t&& exclusive_writelock_ptr, size_t split_index)
			: TRASectionSplitterXWP(std::forward<exclusive_writelock_ptr_t>(exclusive_writelock_ptr), std::array<size_t, 1>{ {split_index}}) {}
		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TRASectionSplitterXWP() {
			mse::impl::T_valid_if_not_an_xscope_type<exclusive_writelock_ptr_t>();
			mse::impl::is_valid_if_exclusive_pointer<exclusive_writelock_ptr_t>::no_op();
		}

		const aco_splitter_ra_section_t& ra_section_aco(size_t index) const {
			return m_splitter_aco_ra_section_map.at(index);
		}
		const aco_splitter_ra_section_t& first_ra_section_aco() const {
			return m_splitter_aco_ra_section_map.at(0);
		}
		const aco_splitter_ra_section_t& second_ra_section_aco() const {
			return m_splitter_aco_ra_section_map.at(1);
		}
	private:
		auto adjusted_ra_iterator(const ra_iterator_t& src_it, const access_lease_ptr_t& access_lease_ptr) const {
#ifdef MSE_TRASECTIONSPLITTERXWP_NDEBUG
			return src_it;
#else // MSE_TRASECTIONSPLITTERXWP_NDEBUG
			return mse::us::impl::make_strong_iterator(src_it, access_lease_ptr);
#endif // MSE_TRASECTIONSPLITTERXWP_NDEBUG
		}

		TRASectionSplitterXWP(const TRASectionSplitterXWP& src) = delete;
		TRASectionSplitterXWP(TRASectionSplitterXWP&& src) = delete;
		TRASectionSplitterXWP & operator=(const TRASectionSplitterXWP& _Right_cref) = delete;
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		access_lease_t m_access_lease_obj;
		std::unordered_map<size_t, aco_splitter_ra_section_t> m_splitter_aco_ra_section_map;
	};

	template <typename _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	class TXScopeACORASectionSplitter : public TXScopeRASectionSplitterXWP<decltype(std::declval<mse::TXScopeAccessControlledObj<_Ty, _TAccessMutex> >().exclusive_pointer())> {
	public:
		typedef TXScopeRASectionSplitterXWP<decltype(std::declval<mse::TXScopeAccessControlledObj<_Ty, _TAccessMutex> >().exclusive_pointer())> base_class;
		typedef mse::TXScopeFixedPointer<mse::TXScopeAccessControlledObj<_Ty, _TAccessMutex> > xsac_obj_xscpptr_t;
		typedef mse::TXScopeFixedPointer<mse::TAccessControlledObj<_Ty, _TAccessMutex> > ac_obj_xscpptr_t;

		TXScopeACORASectionSplitter(const xsac_obj_xscpptr_t& xsptr, size_t split_index) : base_class(xsptr->exclusive_pointer(), split_index) {}
		template<typename _TList>
		TXScopeACORASectionSplitter(const xsac_obj_xscpptr_t& xsptr, const _TList& section_sizes) : base_class(xsptr->exclusive_pointer(), section_sizes) {}

		TXScopeACORASectionSplitter(const ac_obj_xscpptr_t& xsptr, size_t split_index) : base_class(xsptr->exclusive_pointer(), split_index) {}
		template<typename _TList>
		TXScopeACORASectionSplitter(const ac_obj_xscpptr_t& xsptr, const _TList& section_sizes) : base_class(xsptr->exclusive_pointer(), section_sizes) {}

	private:
		TXScopeACORASectionSplitter & operator=(const TXScopeACORASectionSplitter& _Right_cref) = delete;
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
	};


#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif /*__clang__*/

#ifdef __clang__
#pragma clang diagnostic pop
#endif /*__clang__*/


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

	class msearray_test {
	public:
		void test1() {
#ifdef MSE_SELF_TESTS
			// construction uses aggregate initialization
			mse::us::msearray<int, 3> a1{ { 1, 2, 3 } }; // double-braces required in C++11 (not in C++14)
			mse::us::msearray<int, 3> a2 = { 11, 12, 13 };  // never required after =
			mse::us::msearray<std::string, 2> a3 = { std::string("a"), "b" };

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

			auto l_tuple_size = std::tuple_size<mse::us::msearray<int, 3>>::value;
			std::tuple_element<1, mse::us::msearray<int, 3>>::type b1 = 5;

			a1 = a2;

			{
				mse::us::msearray<int, 5> a = { 10, 20, 30, 40, 50 };
				mse::us::msearray<int, 5> b = { 10, 20, 30, 40, 50 };
				mse::us::msearray<int, 5> c = { 50, 40, 30, 20, 10 };

				if (a == b) std::cout << "a and b are equal\n";
				if (b != c) std::cout << "b and c are not equal\n";
				if (b<c) std::cout << "b is less than c\n";
				if (c>b) std::cout << "c is greater than b\n";
				if (a <= b) std::cout << "a is less than or equal to b\n";
				if (a >= b) std::cout << "a is greater than or equal to b\n";
			}
			{
				mse::us::msearray<int, 6> myarray;

				myarray.fill(5);

				std::cout << "myarray contains:";
				for (int& x : myarray) { std::cout << ' ' << x; }

				std::cout << '\n';
			}
#endif // MSE_SELF_TESTS
		}
	};

#ifdef __clang__
#pragma clang diagnostic pop
#else /*__clang__*/
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif /*__GNUC__*/
#endif /*__clang__*/

}

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

#endif /*ndef MSEMSEARRAY_H*/
