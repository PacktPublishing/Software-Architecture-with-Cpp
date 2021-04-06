
// Copyright (c) 2015 Noah Lopez
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef MSELEGACYHELPERS_H_
#define MSELEGACYHELPERS_H_

#if defined(MSE_SAFER_SUBSTITUTES_DISABLED) || defined(MSE_MSTDARRAY_DISABLED)
#define MSE_LEGACYHELPERS_DISABLED
#endif /*defined(MSE_SAFER_SUBSTITUTES_DISABLED) || defined(MSE_MSTDARRAY_DISABLED)*/

#ifndef MSE_LEGACYHELPERS_DISABLED
#include "msepoly.h"
#include "msefunctional.h"
#include <cstring>
#else // !MSE_LEGACYHELPERS_DISABLED
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#endif // !MSE_LEGACYHELPERS_DISABLED

#ifdef _MSC_VER
#pragma warning( push )  
#pragma warning( disable : 4100 4456 4189 4127 )
#endif /*_MSC_VER*/

#ifdef MSE_LEGACYHELPERS_DISABLED

#define MSE_LH_FIXED_ARRAY_TYPE_PREFIX(size) 
#define MSE_LH_FIXED_ARRAY_TYPE_SUFFIX(size) 
#define MSE_LH_FIXED_ARRAY_TYPE_POST_NAME_SUFFIX(size) [size]
#define MSE_LH_FIXED_ARRAY_DECLARATION(element_type, size, name) MSE_LH_FIXED_ARRAY_TYPE_PREFIX(size) element_type MSE_LH_FIXED_ARRAY_TYPE_SUFFIX(size) name MSE_LH_FIXED_ARRAY_TYPE_POST_NAME_SUFFIX(size)
#define MSE_LH_DYNAMIC_ARRAY_ITERATOR_TYPE(element_type) element_type *

#define MSE_LH_ALLOC_POINTER1(element_type) (element_type *)malloc(sizeof(element_type))
#define MSE_LH_ALLOC_DYN_ARRAY1(iterator_type, num_bytes) (iterator_type)malloc(num_bytes)
#define MSE_LH_REALLOC(element_type, ptr, num_bytes) (element_type *)realloc(ptr, num_bytes)
#define MSE_LH_FREE(ptr) free(ptr)

/* generally prefer MSE_LH_ALLOC_DYN_ARRAY1() or MSE_LH_ALLOC_POINTER1() over MSE_LH_ALLOC() */
#define MSE_LH_ALLOC(element_type, ptr, num_bytes) ptr = (element_type *)malloc(num_bytes)

#define MSE_LH_ARRAY_ITERATOR_TYPE(element_type) element_type *
#define MSE_LH_PARAM_ONLY_ARRAY_ITERATOR_TYPE(element_type) element_type *

#define MSE_LH_FREAD(ptr, size, count, stream) fread(ptr, size, count, stream)
#define MSE_LH_FWRITE(ptr, size, count, stream) fwrite(ptr, size, count, stream)

#define MSE_LH_TYPED_MEMCPY(element_type, destination, source, num_bytes) memcpy(destination, source, num_bytes)
#define MSE_LH_TYPED_MEMCMP(element_type, destination, source, num_bytes) memcmp(destination, source, num_bytes)
#define MSE_LH_TYPED_MEMSET(element_type, ptr, value, num_bytes) memset(ptr, value, num_bytes)
#define MSE_LH_MEMCPY(destination, source, num_bytes) memcpy(destination, source, num_bytes)
#define MSE_LH_MEMCMP(destination, source, num_bytes) memcmp(destination, source, num_bytes)
#define MSE_LH_MEMSET(ptr, value, num_bytes) memset(ptr, value, num_bytes)

#define MSE_LH_ADDRESSABLE_TYPE(object_type) object_type
#define MSE_LH_POINTER_TYPE(element_type) element_type *
#define MSE_LH_ALLOC_POINTER_TYPE(element_type) element_type *
#define MSE_LH_PARAM_ONLY_POINTER_TYPE(element_type) element_type *
#define MSE_LH_NULL_POINTER NULL

#define MSE_LH_CAST(type, value) ((type)value)
#define MSE_LH_UNSAFE_CAST(type, value) ((type)value)
#define MSE_LH_UNSAFE_MAKE_POINTER_TO(target) &(target)
#define MSE_LH_UNSAFE_MAKE_RAW_POINTER_TO(target) &(target)

#define MSE_LH_SUPPRESS_CHECK_IN_XSCOPE
#define MSE_LH_SUPPRESS_CHECK_IN_DECLSCOPE

#define MSE_LH_IF_ENABLED(x)
#define MSE_LH_IF_DISABLED(x) x

#else /*MSE_LEGACYHELPERS_DISABLED*/

#define MSE_LH_FIXED_ARRAY_TYPE_PREFIX(size) mse::lh::TNativeArrayReplacement< 
#define MSE_LH_FIXED_ARRAY_TYPE_SUFFIX(size) , size >
#define MSE_LH_FIXED_ARRAY_TYPE_POST_NAME_SUFFIX(size) 
#define MSE_LH_FIXED_ARRAY_DECLARATION(element_type, size, name) MSE_LH_FIXED_ARRAY_TYPE_PREFIX(size) element_type MSE_LH_FIXED_ARRAY_TYPE_SUFFIX(size) name MSE_LH_FIXED_ARRAY_TYPE_POST_NAME_SUFFIX(size)
#define MSE_LH_DYNAMIC_ARRAY_ITERATOR_TYPE(element_type) mse::lh::TStrongVectorIterator< element_type >

#define MSE_LH_ALLOC_POINTER1(element_type) mse::lh::allocate<mse::TNullableAnyPointer<element_type> >()
#define MSE_LH_ALLOC_DYN_ARRAY1(iterator_type, num_bytes) mse::lh::allocate_dyn_array1<iterator_type>(num_bytes)
#define MSE_LH_REALLOC(element_type, ptr, num_bytes) mse::lh::reallocate(ptr, num_bytes)
#define MSE_LH_FREE(ptr) mse::lh::free(ptr)

/* generally prefer MSE_LH_ALLOC_DYN_ARRAY1() or MSE_LH_ALLOC_POINTER1() over MSE_LH_ALLOC() */
#define MSE_LH_ALLOC(element_type, ptr, num_bytes) mse::lh::allocate(ptr, num_bytes)

#define MSE_LH_ARRAY_ITERATOR_TYPE(element_type) mse::lh::TLHNullableAnyRandomAccessIterator< element_type >
/* MSE_LH_PARAM_ONLY_ARRAY_ITERATOR_TYPE is a significantly restricted version of MSE_LH_ARRAY_ITERATOR_TYPE. You might choose to
use it, despite its restrictions, as a function parameter type because it accepts some (high performance) iterators that (the 
otherwise more flexible) MSE_LH_ARRAY_ITERATOR_TYPE doesn't. */
#define MSE_LH_PARAM_ONLY_ARRAY_ITERATOR_TYPE(element_type) mse::lh::TXScopeLHNullableAnyRandomAccessIterator< element_type >

#define MSE_LH_FREAD(ptr, size, count, stream) mse::lh::fread(ptr, size, count, stream)
#define MSE_LH_FWRITE(ptr, size, count, stream) mse::lh::fwrite(ptr, size, count, stream)

#define MSE_LH_TYPED_MEMCPY(element_type, destination, source, num_bytes) mse::lh::memcpy<mse::lh::TLHNullableAnyRandomAccessIterator<element_type>, mse::lh::TLHNullableAnyRandomAccessIterator<element_type> >(destination, source, num_bytes)
#define MSE_LH_TYPED_MEMCMP(element_type, destination, source, num_bytes) mse::lh::memcmp< mse::lh::TLHNullableAnyRandomAccessIterator<element_type>, mse::lh::TLHNullableAnyRandomAccessIterator<element_type> >(destination, source, num_bytes)
#define MSE_LH_TYPED_MEMSET(element_type, ptr, value, num_bytes) mse::lh::memset< mse::lh::TLHNullableAnyRandomAccessIterator<element_type> >(ptr, value, num_bytes)
#define MSE_LH_MEMCPY(destination, source, num_bytes) mse::lh::memcpy(destination, source, num_bytes)
#define MSE_LH_MEMCMP(destination, source, num_bytes) mse::lh::memcmp(destination, source, num_bytes)
#define MSE_LH_MEMSET(ptr, value, num_bytes) mse::lh::memset(ptr, value, num_bytes)

/* MSE_LH_ADDRESSABLE_TYPE() is a type annotation used to indicate that the '&' operator may/will be used to obtain the address of
the associated declared object(s). */
#define MSE_LH_ADDRESSABLE_TYPE(object_type) mse::TRegisteredObj< object_type >
#define MSE_LH_POINTER_TYPE(element_type) mse::lh::TLHNullableAnyPointer< element_type >
#define MSE_LH_ALLOC_POINTER_TYPE(element_type) mse::TRefCountingPointer< element_type >
/* MSE_LH_PARAM_ONLY_POINTER_TYPE is a significantly restricted version of MSE_LH_POINTER_TYPE. You might choose to use it, despite its
restrictions, as a function parameter type because it accepts some (high performance) pointers that (the otherwise more flexible)
MSE_LH_POINTER_TYPE doesn't. (Including raw pointers.) */
#define MSE_LH_PARAM_ONLY_POINTER_TYPE(element_type) mse::lh::TXScopeLHNullableAnyPointer< element_type >
#define MSE_LH_NULL_POINTER nullptr

#define MSE_LH_CAST(type, value) type(value)
#define MSE_LH_UNSAFE_CAST(type, value) mse::us::lh::unsafe_cast<type>(value)
#define MSE_LH_UNSAFE_MAKE_POINTER_TO(target) MSE_LH_POINTER_TYPE(mse::us::unsafe_make_any_pointer_to(target))
#define MSE_LH_UNSAFE_MAKE_RAW_POINTER_TO(target) std::addressof(target)

#define MSE_LH_SUPPRESS_CHECK_IN_XSCOPE MSE_SUPPRESS_CHECK_IN_XSCOPE
#define MSE_LH_SUPPRESS_CHECK_IN_DECLSCOPE MSE_SUPPRESS_CHECK_IN_DECLSCOPE

#define MSE_LH_IF_ENABLED(x) x
#define MSE_LH_IF_DISABLED(x)

namespace mse {
	namespace lh {
		typedef decltype(NULL) NULL_t;

		template <typename _Ty>
		class TLHNullableAnyPointer : public mse::TNullableAnyPointer<_Ty> {
		public:
			typedef mse::TNullableAnyPointer<_Ty> base_class;
			MSE_USING(TLHNullableAnyPointer, base_class);
			TLHNullableAnyPointer(const TLHNullableAnyPointer& src) = default;
			TLHNullableAnyPointer(const NULL_t val) : base_class(std::nullptr_t()) {
				/* This constructor is just to support zero being used as a null pointer value. */
				assert(0 == val);
			}

			friend void swap(TLHNullableAnyPointer& first, TLHNullableAnyPointer& second) {
				swap(static_cast<base_class&>(first), static_cast<base_class&>(second));
			}

			bool operator==(const std::nullptr_t& _Right_cref) const { return base_class::operator==(_Right_cref); }
			bool operator!=(const std::nullptr_t& _Right_cref) const { return !((*this) == _Right_cref); }
			bool operator==(const NULL_t val) const {
				/* This operator is just to support zero being used as a null pointer value. */
				assert(0 == val);
				return (*this == nullptr);
			}
			bool operator!=(const NULL_t val) const { return !((*this) == val); }

			TLHNullableAnyPointer& operator=(const std::nullptr_t& _Right_cref) {
				base_class::operator=(_Right_cref);
				return *this;
			}
			TLHNullableAnyPointer& operator=(const TLHNullableAnyPointer& _Right_cref) {
				base_class::operator=(_Right_cref);
				return (*this);
			}

			operator bool() const {
				return base_class::operator bool();
			}

			void async_not_shareable_and_not_passable_tag() const {}

		private:
			MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;
		};

		template <typename _Ty>
		bool operator==(const NULL_t lhs, const TLHNullableAnyPointer<_Ty>& rhs) { return rhs == lhs; }
		template <typename _Ty>
		bool operator!=(const NULL_t lhs, const TLHNullableAnyPointer<_Ty>& rhs) { return rhs != lhs; }

		template <typename _Ty>
		class TXScopeLHNullableAnyPointer : public mse::TXScopeNullableAnyPointer<_Ty> {
		public:
			typedef mse::TXScopeNullableAnyPointer<_Ty> base_class;
			MSE_USING(TXScopeLHNullableAnyPointer, base_class);
			TXScopeLHNullableAnyPointer(const TXScopeLHNullableAnyPointer& src) = default;
			TXScopeLHNullableAnyPointer(const NULL_t val) : base_class(std::nullptr_t()) {
				/* This constructor is just to support zero being used as a null pointer value. */
				assert(0 == val);
			}

			friend void swap(TXScopeLHNullableAnyPointer& first, TXScopeLHNullableAnyPointer& second) {
				swap(static_cast<base_class&>(first), static_cast<base_class&>(second));
			}

			bool operator==(const std::nullptr_t& _Right_cref) const { return base_class::operator==(_Right_cref); }
			bool operator!=(const std::nullptr_t& _Right_cref) const { return !((*this) == _Right_cref); }
			bool operator==(const NULL_t val) const {
				/* This operator is just to support zero being used as a null pointer value. */
				assert(0 == val);
				return (*this == nullptr);
			}
			bool operator!=(const NULL_t val) const { return !((*this) == val); }

			operator bool() const {
				return base_class::operator bool();
			}

			void async_not_shareable_and_not_passable_tag() const {}

		private:
			MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;
		};

		template <typename _Ty>
		bool operator==(const NULL_t lhs, const TXScopeLHNullableAnyPointer<_Ty>& rhs) { return rhs == lhs; }
		template <typename _Ty>
		bool operator!=(const NULL_t lhs, const TXScopeLHNullableAnyPointer<_Ty>& rhs) { return rhs != lhs; }

		template <typename _Ty>
		class TLHNullableAnyRandomAccessIterator : public mse::TNullableAnyRandomAccessIterator<_Ty> {
		public:
			typedef mse::TNullableAnyRandomAccessIterator<_Ty> base_class;
			MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

			MSE_USING(TLHNullableAnyRandomAccessIterator, base_class);
			TLHNullableAnyRandomAccessIterator(const TLHNullableAnyRandomAccessIterator& src) = default;

			TLHNullableAnyRandomAccessIterator(const NULL_t val) : base_class(std::nullptr_t()) {
				/* This constructor is just to support zero being used as a null pointer/iterator value. */
				assert(0 == val);
			}

			friend void swap(TLHNullableAnyRandomAccessIterator& first, TLHNullableAnyRandomAccessIterator& second) {
				swap(static_cast<base_class&>(first), static_cast<base_class&>(second));
			}

			bool operator==(const std::nullptr_t& _Right_cref) const { return base_class::operator==(_Right_cref); }
			TLHNullableAnyRandomAccessIterator& operator=(const std::nullptr_t& _Right_cref) {
				base_class::operator=(_Right_cref);
				return *this;
			}
			TLHNullableAnyRandomAccessIterator& operator=(const TLHNullableAnyRandomAccessIterator& _Right_cref) {
				base_class::operator=(_Right_cref);
				return (*this);
			}

			explicit operator bool() const {
				return base_class::operator bool();
			}

			MSE_INHERIT_ITERATOR_ARITHMETIC_OPERATORS_FROM(base_class, TLHNullableAnyRandomAccessIterator);

			void async_not_shareable_and_not_passable_tag() const {}

		private:
			MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;
		};

		template <typename _Ty>
		class TXScopeLHNullableAnyRandomAccessIterator : public mse::TXScopeNullableAnyRandomAccessIterator<_Ty> {
		public:
			typedef mse::TXScopeNullableAnyRandomAccessIterator<_Ty> base_class;
			MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

			MSE_USING(TXScopeLHNullableAnyRandomAccessIterator, base_class);
			TXScopeLHNullableAnyRandomAccessIterator(const TXScopeLHNullableAnyRandomAccessIterator& src) = default;

			TXScopeLHNullableAnyRandomAccessIterator(const NULL_t val) : base_class(std::nullptr_t()) {
				/* This constructor is just to support zero being used as a null pointer/iterator value. */
				assert(0 == val);
			}

			friend void swap(TXScopeLHNullableAnyRandomAccessIterator& first, TXScopeLHNullableAnyRandomAccessIterator& second) {
				swap(static_cast<base_class&>(first), static_cast<base_class&>(second));
			}

			bool operator==(const std::nullptr_t& _Right_cref) const { return base_class::operator==(_Right_cref); }

			explicit operator bool() const {
				return base_class::operator bool();
			}

			MSE_INHERIT_ITERATOR_ARITHMETIC_OPERATORS_FROM(base_class, TXScopeLHNullableAnyRandomAccessIterator);

			void async_not_shareable_and_not_passable_tag() const {}

		private:
			MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;
		};

		/* This data type was motivated by the need for a direct substitute for native pointers targeting dynamically
		allocated (native) arrays, which can kind of play a dual role as a reference to the array object and/or as an
		iterator. */
		template <typename _Ty>
		class TStrongVectorIterator : public mse::TRAIterator<mse::TRefCountingPointer<mse::stnii_vector<_Ty>>> {
		public:
			typedef mse::TRAIterator<mse::TRefCountingPointer<mse::stnii_vector<_Ty>>> base_class;
			MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

			TStrongVectorIterator() = default;
			TStrongVectorIterator(const std::nullptr_t& src) : TStrongVectorIterator() {}
			TStrongVectorIterator(const TStrongVectorIterator& src) = default;
			TStrongVectorIterator(TStrongVectorIterator&& src) = default;
			TStrongVectorIterator(_XSTD initializer_list<_Ty> _Ilist) : base_class(mse::make_refcounting<mse::stnii_vector<_Ty>>(_Ilist), 0) {}
			TStrongVectorIterator(const base_class& src) : base_class(src) {}
			TStrongVectorIterator(const mse::TXScopeRAIterator<mse::TRefCountingPointer<mse::stnii_vector<_Ty>>>& src) : base_class(src) {}
			explicit TStrongVectorIterator(size_type _N) : base_class(mse::make_refcounting<mse::stnii_vector<_Ty>>(_N), 0) {}
			explicit TStrongVectorIterator(size_type _N, const _Ty& _V) : base_class(mse::make_refcounting<mse::stnii_vector<_Ty>>(_N, _V), 0) {}
			/*
			template <class... Args>
			TStrongVectorIterator(Args&&... args) : base_class(mse::make_refcounting<mse::stnii_vector<_Ty>>(std::forward<Args>(args)...), 0) {}
			*/

			size_type size() const {
				if (vector_refcptr()) {
					return (*vector_refcptr()).size();
				}
				else {
					return 0;
				}
			}
			void resize(size_type _N, const _Ty& _X = _Ty()) {
				if (!vector_refcptr()) {
					base_class::operator=(base_class(mse::make_refcounting<mse::stnii_vector<_Ty>>(), 0));
				}

				//auto old_size = size();

				(*vector_refcptr()).resize(_N, _X);
				(*vector_refcptr()).shrink_to_fit();

				/*
				if (true || (0 == old_size)) {
					(*this).set_to_beginning();
				}
				*/
			}

			TStrongVectorIterator& operator=(const std::nullptr_t& _Right_cref) {
				return operator=(TStrongVectorIterator());
			}
			TStrongVectorIterator& operator=(const TStrongVectorIterator& _Right_cref) {
				base_class::operator=(_Right_cref);
				return(*this);
			}

			MSE_INHERIT_ITERATOR_ARITHMETIC_OPERATORS_FROM(base_class, TStrongVectorIterator);

			explicit operator bool() const {
				return ((*this).size() != 0);
			}

			template <class... Args>
			static TStrongVectorIterator make(Args&&... args) {
				return TStrongVectorIterator(std::forward<Args>(args)...);
			}

		private:
			auto vector_refcptr() { return (*this).target_container_ptr(); }
			auto vector_refcptr() const { return (*this).target_container_ptr(); }
		};

		template <class X, class... Args>
		TStrongVectorIterator<X> make_strong_vector_iterator(Args&&... args) {
			return TStrongVectorIterator<X>::make(std::forward<Args>(args)...);
		}

		template <typename _Ty>
		class TXScopeStrongVectorIterator : public mse::TXScopeRAIterator<mse::TRefCountingPointer<mse::stnii_vector<_Ty>>> {
		public:
			typedef mse::TXScopeRAIterator<mse::TRefCountingPointer<mse::stnii_vector<_Ty>>> base_class;
			MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

			TXScopeStrongVectorIterator() = default;
			TXScopeStrongVectorIterator(const std::nullptr_t& src) : TXScopeStrongVectorIterator() {}
			TXScopeStrongVectorIterator(const TXScopeStrongVectorIterator& src) = default;
			TXScopeStrongVectorIterator(TXScopeStrongVectorIterator&& src) = default;
			TXScopeStrongVectorIterator(_XSTD initializer_list<_Ty> _Ilist) : base_class(mse::make_refcounting<mse::stnii_vector<_Ty>>(_Ilist), 0) {}
			TXScopeStrongVectorIterator(const base_class& src) : base_class(src) {}
			TXScopeStrongVectorIterator(const mse::TRAIterator<mse::TRefCountingPointer<mse::stnii_vector<_Ty>>>& src) : base_class(src) {}
			explicit TXScopeStrongVectorIterator(size_type _N) : base_class(mse::make_refcounting<mse::stnii_vector<_Ty>>(_N), 0) {}
			explicit TXScopeStrongVectorIterator(size_type _N, const _Ty& _V) : base_class(mse::make_refcounting<mse::stnii_vector<_Ty>>(_N, _V), 0) {}
			/*
			template <class... Args>
			TXScopeStrongVectorIterator(Args&&... args) : base_class(mse::make_refcounting<mse::stnii_vector<_Ty>>(std::forward<Args>(args)...), 0) {}
			*/

			size_type size() const {
				if (vector_refcptr()) {
					return (*vector_refcptr()).size();
				}
				else {
					return 0;
				}
			}
			void resize(size_type _N, const _Ty& _X = _Ty()) {
				if (!vector_refcptr()) {
					base_class::operator=(base_class(mse::make_refcounting<mse::stnii_vector<_Ty>>(), 0));
				}

				//auto old_size = size();

				(*vector_refcptr()).resize(_N, _X);
				(*vector_refcptr()).shrink_to_fit();

				/*
				if (true || (0 == old_size)) {
					(*this).set_to_beginning();
				}
				*/
			}

			TXScopeStrongVectorIterator& operator=(const std::nullptr_t& _Right_cref) {
				return operator=(TXScopeStrongVectorIterator());
			}
			TXScopeStrongVectorIterator& operator=(const TXScopeStrongVectorIterator& _Right_cref) {
				base_class::operator=(_Right_cref);
				return(*this);
			}

			MSE_INHERIT_ITERATOR_ARITHMETIC_OPERATORS_FROM(base_class, TXScopeStrongVectorIterator);

			explicit operator bool() const {
				return ((*this).size() != 0);
			}

			template <class... Args>
			static TXScopeStrongVectorIterator make(Args&&... args) {
				return TXScopeStrongVectorIterator(std::forward<Args>(args)...);
			}

		private:
			auto vector_refcptr() { return (*this).target_container_ptr(); }
			auto vector_refcptr() const { return (*this).target_container_ptr(); }
		};

		template <class X, class... Args>
		TXScopeStrongVectorIterator<X> make_xscope_strong_vector_iterator(Args&&... args) {
			return TXScopeStrongVectorIterator<X>::make(std::forward<Args>(args)...);
		}

		/* TXScopeStrongVectorIterator<> does not directly convert to mse::rsv::TFParam<mse::TXScopeCSSSXSTERandomAccessIterator<> >.
		But the following function can be used to obtain a "locking" scope iterator that does. */
		template <class X>
		auto make_xscope_locking_vector_iterator(const TXScopeStrongVectorIterator<X>& xs_strong_iter) {
			auto retval = mse::make_xscope_begin_iterator(mse::us::unsafe_make_xscope_pointer_to(*xs_strong_iter.target_container_ptr()));
			retval += xs_strong_iter.position();
			return retval;
		}
		template <class X>
		auto make_xscope_locking_vector_iterator(TXScopeStrongVectorIterator<X>&& xs_strong_iter) = delete;

		/* deprecated aliases */
		template <typename _Ty>
		using TIPointerWithBundledVector = TStrongVectorIterator<_Ty>;
		template <class X, class... Args>
		TIPointerWithBundledVector<X> make_ipointer_with_bundled_vector(Args&&... args) {
			return make_string_vector_iterator(std::forward<Args>(args)...);
		}


		template <typename _Ty, size_t _Size>
		class TNativeArrayReplacement : public mse::mstd::array<_Ty, _Size> {
		public:
			typedef mse::mstd::array<_Ty, _Size> base_class;
			using base_class::base_class;

			operator mse::lh::TLHNullableAnyRandomAccessIterator<_Ty>() {
				return base_class::begin();
			}
			operator mse::TNullableAnyRandomAccessIterator<_Ty>() {
				return base_class::begin();
			}
			operator mse::TAnyRandomAccessIterator<_Ty>() {
				return base_class::begin();
			}
			operator mse::TAnyRandomAccessConstIterator<_Ty>() const {
				return base_class::cbegin();
			}
			operator typename mse::mstd::array<_Ty, _Size>::iterator() {
				return base_class::begin();
			}
			template<class _Ty2 = _Ty, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _Ty>::value)
				&& (!std::is_same<typename mse::mstd::array<_Ty2, _Size>::const_iterator, typename mse::mstd::array<_Ty2, _Size>::iterator>::value)> MSE_IMPL_EIS >
			operator typename mse::mstd::array<_Ty, _Size>::const_iterator() {
				return base_class::cbegin();
			}
			template<class _Ty2 = _Ty, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _Ty>::value)
				&& (!std::is_const<_Ty2>::value)> MSE_IMPL_EIS >
			operator mse::TNullableAnyRandomAccessIterator<const _Ty>() {
				return base_class::begin();
			}
			typename base_class::iterator operator+(typename base_class::difference_type n) { return base_class::begin() + n; }
			typename base_class::iterator operator-(typename base_class::difference_type n) { return base_class::begin() - n; }
			typename base_class::difference_type operator-(const typename base_class::iterator& _Right_cref) const { return base_class::begin() - _Right_cref; }
			typename base_class::const_iterator operator+(typename base_class::difference_type n) const { return base_class::cbegin() + n; }
			typename base_class::const_iterator operator-(typename base_class::difference_type n) const { return base_class::cbegin() - n; }
			template<class _Ty2 = _Ty, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _Ty>::value)
				&& (!std::is_same<typename mse::mstd::array<_Ty2, _Size>::const_iterator, typename mse::mstd::array<_Ty2, _Size>::iterator>::value)> MSE_IMPL_EIS >
			typename base_class::difference_type operator-(const typename base_class::const_iterator& _Right_cref) const { return base_class::cbegin() - _Right_cref; }

#ifdef MSE_LEGACYHELPERS_DISABLED
			TNativeArrayReplacement(_XSTD initializer_list<_Ty> _Ilist) : base_class(mse::nii_array<_Ty, _Size>(_Ilist)) {}
#endif // MSE_LEGACYHELPERS_DISABLED

		};


		namespace impl {
			template<class _TPtr>
			class CAllocF {
			public:
				static void free(_TPtr& ptr) {
					ptr = nullptr;
				}
				static void allocate(_TPtr& ptr, size_t num_bytes) {
					typedef mse::impl::remove_reference_t<decltype(*ptr)> target_t;
					if (0 == num_bytes) {
						ptr = nullptr;
					}
					else if (sizeof(target_t) == num_bytes) {
						ptr = mse::make_refcounting<target_t>();
					}
					else {
						assert(false);
						ptr = mse::make_refcounting<target_t>();
						//MSE_THROW(std::bad_alloc("the given allocation size is not supported for this pointer type - CAllocF<_TPtr>::allocate()"));
					}
				}
				//static void reallocate(_TPtr& ptr, size_t num_bytes);
			};
			template<class _Ty>
			class CAllocF<_Ty*> {
			public:
				static void free(_Ty* ptr) {
					::free(ptr);
				}
				static void allocate(_Ty*& ptr, size_t num_bytes) {
					ptr = ::malloc(num_bytes);
				}
				static void reallocate(_Ty*& ptr, size_t num_bytes) {
					ptr = ::realloc(ptr, num_bytes);
				}
			};
			template<class _Ty>
			void free(_Ty* ptr) { CAllocF<_Ty*>::free(ptr); }
			template<class _Ty>
			void allocate(_Ty*& ptr, size_t num_bytes) { CAllocF<_Ty*>::allocate(ptr, num_bytes); }
			template<class _Ty>
			void reallocate(_Ty*& ptr, size_t num_bytes) { CAllocF<_Ty*>::reallocate(ptr, num_bytes); }

			template<class _Ty>
			class CAllocF<mse::lh::TStrongVectorIterator<_Ty>> {
			public:
				static void free(mse::lh::TStrongVectorIterator<_Ty>& ptr) {
					ptr = mse::lh::TStrongVectorIterator<_Ty>();
				}
				static void allocate(mse::lh::TStrongVectorIterator<_Ty>& ptr, size_t num_bytes) {
					mse::lh::TStrongVectorIterator<_Ty> tmp(num_bytes / sizeof(_Ty));
					ptr = tmp;
				}
				static void reallocate(mse::lh::TStrongVectorIterator<_Ty>& ptr, size_t num_bytes) {
					ptr.resize(num_bytes / sizeof(_Ty));
				}
			};
			template<class _Ty>
			void free_overloaded(mse::lh::TStrongVectorIterator<_Ty>& ptr) { CAllocF<mse::lh::TStrongVectorIterator<_Ty>>::free(ptr); }
			template<class _Ty>
			void allocate_overloaded(mse::lh::TStrongVectorIterator<_Ty>& ptr, size_t num_bytes) { CAllocF<mse::lh::TStrongVectorIterator<_Ty>>::allocate(ptr, num_bytes); }
			template<class _Ty>
			void reallocate_overloaded(mse::lh::TStrongVectorIterator<_Ty>& ptr, size_t num_bytes) { CAllocF<mse::lh::TStrongVectorIterator<_Ty>>::reallocate(ptr, num_bytes); }

			template<class _Ty>
			class CAllocF<mse::TNullableAnyRandomAccessIterator<_Ty>> {
			public:
				static void free(mse::TNullableAnyRandomAccessIterator<_Ty>& ptr) {
					ptr = mse::lh::TStrongVectorIterator<_Ty>();
				}
				static void allocate(mse::TNullableAnyRandomAccessIterator<_Ty>& ptr, size_t num_bytes) {
					mse::lh::TStrongVectorIterator<_Ty> tmp(num_bytes / sizeof(_Ty));
					ptr = tmp;
				}
				//static void reallocate(mse::TNullableAnyRandomAccessIterator<_Ty>& ptr, size_t num_bytes);
			};
			template<class _Ty>
			void free_overloaded(mse::TNullableAnyRandomAccessIterator<_Ty>& ptr) { CAllocF<mse::TNullableAnyRandomAccessIterator<_Ty>>::free(ptr); }
			template<class _Ty>
			void allocate_overloaded(mse::TNullableAnyRandomAccessIterator<_Ty>& ptr, size_t num_bytes) { CAllocF<mse::TNullableAnyRandomAccessIterator<_Ty>>::allocate(ptr, num_bytes); }
			template<class _Ty>
			void reallocate_overloaded(mse::TNullableAnyRandomAccessIterator<_Ty>& ptr, size_t num_bytes) { CAllocF<mse::TNullableAnyRandomAccessIterator<_Ty>>::reallocate(ptr, num_bytes); }

			template<class _Ty>
			class CAllocF<mse::lh::TLHNullableAnyRandomAccessIterator<_Ty>> {
			public:
				static void free(mse::lh::TLHNullableAnyRandomAccessIterator<_Ty>& ptr) {
					ptr = mse::lh::TStrongVectorIterator<_Ty>();
				}
				static void allocate(mse::lh::TLHNullableAnyRandomAccessIterator<_Ty>& ptr, size_t num_bytes) {
					mse::lh::TStrongVectorIterator<_Ty> tmp(num_bytes / sizeof(_Ty));
					ptr = tmp;
				}
				//static void reallocate(mse::lh::TLHNullableAnyRandomAccessIterator<_Ty>& ptr, size_t num_bytes);
			};
			template<class _Ty>
			void free_overloaded(mse::lh::TLHNullableAnyRandomAccessIterator<_Ty>& ptr) { CAllocF<mse::lh::TLHNullableAnyRandomAccessIterator<_Ty>>::free(ptr); }
			template<class _Ty>
			void allocate_overloaded(mse::lh::TLHNullableAnyRandomAccessIterator<_Ty>& ptr, size_t num_bytes) { CAllocF<mse::lh::TLHNullableAnyRandomAccessIterator<_Ty>>::allocate(ptr, num_bytes); }
			//template<class _Ty>
			//void reallocate_overloaded(mse::lh::TLHNullableAnyRandomAccessIterator<_Ty>& ptr, size_t num_bytes) { CAllocF<mse::lh::TLHNullableAnyRandomAccessIterator<_Ty>>::reallocate(ptr, num_bytes); }

			template<class _Ty>
			class CAllocF<mse::lh::TXScopeStrongVectorIterator<_Ty>> {
			public:
				static void free(mse::lh::TXScopeStrongVectorIterator<_Ty>& ptr) {
					ptr = mse::lh::TXScopeStrongVectorIterator<_Ty>();
				}
				static void allocate(mse::lh::TXScopeStrongVectorIterator<_Ty>& ptr, size_t num_bytes) {
					mse::lh::TXScopeStrongVectorIterator<_Ty> tmp(num_bytes / sizeof(_Ty));
					ptr = tmp;
				}
				static void reallocate(mse::lh::TXScopeStrongVectorIterator<_Ty>& ptr, size_t num_bytes) {
					ptr.resize(num_bytes / sizeof(_Ty));
				}
			};
			template<class _Ty>
			void free_overloaded(mse::lh::TXScopeStrongVectorIterator<_Ty>& ptr) { CAllocF<mse::lh::TXScopeStrongVectorIterator<_Ty>>::free(ptr); }
			template<class _Ty>
			void allocate_overloaded(mse::lh::TXScopeStrongVectorIterator<_Ty>& ptr, size_t num_bytes) { CAllocF<mse::lh::TXScopeStrongVectorIterator<_Ty>>::allocate(ptr, num_bytes); }
			template<class _Ty>
			void reallocate_overloaded(mse::lh::TXScopeStrongVectorIterator<_Ty>& ptr, size_t num_bytes) { CAllocF<mse::lh::TXScopeStrongVectorIterator<_Ty>>::reallocate(ptr, num_bytes); }

			template<class T, class EqualTo>
			struct IsSupportedByAllocateOverloaded_impl
			{
				template<class U, class V>
				static auto test(U* u) -> decltype(allocate_overloaded(*u, 5), std::declval<V>(), bool(true));
				template<typename, typename>
				static auto test(...)->std::false_type;

				using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
			};
			template<class T, class EqualTo = T>
			struct IsSupportedByAllocateOverloaded : IsSupportedByAllocateOverloaded_impl<
				mse::impl::remove_reference_t<T>, mse::impl::remove_reference_t<EqualTo> >::type {};

			template<class _Ty>
			auto free_helper1(std::true_type, _Ty& ptr) {
				return free_overloaded(ptr);
			}
			template<class _Ty>
			auto free_helper1(std::false_type, _Ty& ptr) {
				return CAllocF<_Ty>::free(ptr);
			}
			template<class _Ty>
			auto allocate_helper1(std::true_type, _Ty& ptr, size_t num_bytes) {
				return allocate_overloaded(ptr, num_bytes);
			}
			template<class _Ty>
			auto allocate_helper1(std::false_type, _Ty& ptr, size_t num_bytes) {
				return CAllocF<_Ty>::allocate(ptr, num_bytes);
			}
			template<class _Ty>
			auto reallocate_helper1(std::true_type, _Ty& ptr, size_t num_bytes) {
				return reallocate_overloaded(ptr, num_bytes);
			}
			template<class _Ty>
			auto reallocate_helper1(std::false_type, _Ty& ptr, size_t num_bytes) {
				return CAllocF<_Ty>::reallocate(ptr, num_bytes);
			}
		}

		template<class _TPointer>
		_TPointer allocate() {
			_TPointer ptr;
			auto num_bytes = sizeof(decltype(*ptr));
			MSE_TRY{
				impl::allocate_helper1(typename impl::IsSupportedByAllocateOverloaded<_TPointer>::type(), ptr, num_bytes);
			}
			MSE_CATCH_ANY{
				return _TPointer();
			}
			return ptr;
		}
		template<class _TDynArrayIter>
		_TDynArrayIter allocate_dyn_array1(size_t num_bytes) {
			_TDynArrayIter ptr;
			MSE_TRY{
				impl::allocate_helper1(typename impl::IsSupportedByAllocateOverloaded<_TDynArrayIter>::type(), ptr, num_bytes);
			}
			MSE_CATCH_ANY{
				return _TDynArrayIter();
			}
			return ptr;
		}
		template<class _TDynArrayIter>
		_TDynArrayIter reallocate(const _TDynArrayIter& ptr2, size_t num_bytes) {
			_TDynArrayIter ptr = ptr2;
			MSE_TRY{
				impl::reallocate_helper1(typename impl::IsSupportedByAllocateOverloaded<_TDynArrayIter>::type(), ptr, num_bytes);
			}
			MSE_CATCH_ANY{
				return _TDynArrayIter();
			}
			return ptr;
		}
		template<class _TDynArrayIter>
		_TDynArrayIter allocate(_TDynArrayIter& ptr, size_t num_bytes) {
			MSE_TRY{
				impl::allocate_helper1(typename impl::IsSupportedByAllocateOverloaded<_TDynArrayIter>::type(), ptr, num_bytes);
			}
			MSE_CATCH_ANY{
				return _TDynArrayIter();
			}
			return ptr;
		}
		template<class _TDynArrayIter>
		void free(_TDynArrayIter& ptr) {
			impl::free_helper1(typename impl::IsSupportedByAllocateOverloaded<_TDynArrayIter>::type(), ptr);
		}

		/* Memory safe approximation of fread(). */
		template<class _TIter>
		size_t fread(_TIter ptr, size_t size, size_t count, FILE* stream) {
			typedef mse::impl::remove_reference_t<decltype((ptr)[0])> element_t;
			thread_local std::vector<unsigned char> v;
			v.resize(size * count);
			auto num_bytes_read = ::fread(v.data(), size, count, stream);
			auto num_items_read = num_bytes_read / sizeof(element_t);
			size_t uc_index = 0;
			size_t element_index = 0;
			for (; element_index < num_items_read; uc_index += sizeof(element_t), element_index += 1) {
				unsigned char* uc_ptr = &(v[uc_index]);
				if (false) {
					element_t* element_ptr = reinterpret_cast<element_t*>(uc_ptr);
					ptr[element_index] = (*element_ptr);
				}
				else {
					long long int adjusted_value = 0;
					static_assert(sizeof(adjusted_value) >= sizeof(element_t), "The (memory safe) lh::fread() function does not support element types larger than `long long int`. ");
					for (size_t i = 0; i < sizeof(element_t); i += 1) {
						adjusted_value |= ((uc_ptr[i]) << (8 * ((sizeof(element_t) - 1) - i)));
					}
					/* Only element types that are (or can be constructed from) integral types are supported. */
					ptr[element_index] = element_t(adjusted_value);
				}
			}
			v.resize(0);
			v.shrink_to_fit();
			return num_bytes_read;
		}
		/* Memory safe approximation of fwrite(). */
		template<class _TIter>
		size_t fwrite(_TIter ptr, size_t size, size_t count, FILE* stream) {
			typedef mse::impl::remove_reference_t<decltype((ptr)[0])> element_t;
			auto num_items_to_write = size * count / sizeof(element_t);
			thread_local std::vector<unsigned char> v;
			v.resize(size * count);
			//assert(num_items_to_write * sizeof(element_t) == size * count);
			size_t uc_index = 0;
			size_t element_index = 0;
			for (; element_index < num_items_to_write; uc_index += sizeof(element_t), element_index += 1) {
				unsigned char* uc_ptr = &(v[uc_index]);
				if (false) {
					typedef mse::impl::remove_const_t<element_t> non_const_element_t;
					non_const_element_t* element_ptr = reinterpret_cast<non_const_element_t*>(uc_ptr);
					(*element_ptr) = ptr[element_index];
				}
				else {
					/* Only element types that are (or convert to) integral types are supported. */
					long long int value(ptr[element_index]);
					static_assert(sizeof(value) >= sizeof(element_t), "The (memory safe) lh::write() function does not support element types larger than `long long int`. ");
					for (size_t i = 0; i < sizeof(element_t); i += 1) {
						uc_ptr[i] = ((value >> (8 * ((sizeof(element_t) - 1) - i))) & 0xff);
					}
				}
			}
			auto res = ::fwrite(v.data(), size, count, stream);
			v.resize(0);
			v.shrink_to_fit();
			return res;
		}

		/* One of the principles of the safe library is that the underlying representation of objects/variable/data (as stored in
		memory) is not directly accessible. The low-level memory functions, like memcpy/memcmp/memset/etc., aren't really compatible
		with the spirit of this principle. But in practice, for many simple cases, these functions have an equivalent "safe"
		counterpart implementation. Though in some cases, the safe implementations will not produce the exact same result. */

		namespace impl {
			template<class T, class EqualTo>
			struct HasOrInheritsSubscriptOperator_impl
			{
				template<class U, class V>
				static auto test(U* u) -> decltype((*u)[0], std::declval<V>(), bool(true));
				template<typename, typename>
				static auto test(...)->std::false_type;

				static const bool value = std::is_same<bool, decltype(test<T, EqualTo>(0))>::value;
				using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
			};
			template<class T, class EqualTo = T>
			struct HasOrInheritsSubscriptOperator : HasOrInheritsSubscriptOperator_impl<
				mse::impl::remove_reference_t<T>, mse::impl::remove_reference_t<EqualTo> >::type {};

			namespace us {
				template<class _TIter, class _TIter2>
				_TIter memcpy_helper1(std::true_type, _TIter destination, _TIter2 source, size_t num_bytes) {
					typedef mse::impl::remove_reference_t<decltype((destination)[0])> element_t;
					auto num_items = num_bytes / sizeof(element_t);
					//assert(num_items * sizeof(element_t) == num_bytes);
					for (size_t i = 0; i < num_items; i += 1) {
						destination[i] = source[i];
					}
					return destination;
				}

				template<class _TPointer, class _TPointer2>
				_TPointer memcpy_helper1(std::false_type, _TPointer destination, _TPointer2 source, size_t num_bytes) {
					//typedef mse::impl::remove_reference_t<decltype(*destination)> element_t;
					//auto num_items = num_bytes / sizeof(element_t);
					//assert(1 == num_items);
					//assert(num_items * sizeof(element_t) == num_bytes);
					*destination = *source;
					return destination;
				}
			}
		}
		/* Memory safe approximation of memcpy(). */
		template<class _TIter, class _TIter2>
		_TIter memcpy(_TIter destination, _TIter2 source, size_t num_bytes) {
			return impl::us::memcpy_helper1(typename impl::HasOrInheritsSubscriptOperator<_TIter>::type(), destination, source, num_bytes);
		}

		namespace impl {
			namespace us {
				template<class _TIter, class _TIter2>
				int memcmp_helper1(std::true_type, _TIter destination, _TIter2 source, size_t num_bytes) {
					typedef mse::impl::remove_reference_t<decltype((destination)[0])> element_t;
					auto num_items = num_bytes / sizeof(element_t);
					//assert(num_items * sizeof(element_t) == num_bytes);
					for (size_t i = 0; i < num_items; i += 1) {
						auto diff = destination[i] - source[i];
						if (0 != diff) {
							return diff;
						}
					}
					return 0;
				}

				template<class _TPointer, class _TPointer2>
				int memcmp_helper1(std::false_type, _TPointer destination, _TPointer2 source, size_t num_bytes) {
					//typedef mse::impl::remove_reference_t<decltype(*destination)> element_t;
					//auto num_items = num_bytes / sizeof(element_t);
					//assert(1 == num_items);
					//assert(num_items * sizeof(element_t) == num_bytes);
					return (*destination) - (*source);
				}
			}
		}
		/* Memory safe approximation of memcmp(). */
		template<class _TIter, class _TIter2>
		int memcmp(_TIter destination, _TIter2 source, size_t num_bytes) {
			return impl::us::memcmp_helper1(typename impl::HasOrInheritsSubscriptOperator<_TIter>::type(), destination, source, num_bytes);
		}

		namespace impl {
			namespace us {
				template<class element_t>
				auto memset_adjusted_value1(std::true_type, int value) {
					value &= 0xff;
					long long int adjusted_value = value;
					if (sizeof(adjusted_value) >= sizeof(element_t)) {
						for (size_t i = 1; i < sizeof(element_t); i += 1) {
							adjusted_value |= (value << (8 * i));
						}
					}
					else {
						/* In this case, if the value being set is non-zero, then it's likely that this function won't
						faithfully emulate the standard memset() function. */
						assert(0 == value);
					}
					return element_t(adjusted_value);
				}
				template<class element_t>
				auto memset_adjusted_value1(std::false_type, int value) {
					/* The (integer) value is not assignable to the element. If the given value is zero, we'll assume that memset is
					just being used to "reset" the element to "the default" state. */
					assert(0 == value);
					return element_t{};
				}

				template<class _TIter>
				void memset_helper1(std::true_type, _TIter iter, int value, size_t num_bytes) {
					typedef mse::impl::remove_reference_t<decltype(iter[0])> element_t;
					const auto element_value = memset_adjusted_value1<element_t>(typename std::is_assignable<element_t, long long int>::type(), value);
					auto num_items = num_bytes / sizeof(element_t);
					//assert(num_items * sizeof(element_t) == num_bytes);
					for (size_t i = 0; i < num_items; i += 1) {
						iter[i] = element_value;
					}
				}

				template<class _TPointer>
				void memset_helper1(std::false_type, _TPointer ptr, int value, size_t num_bytes) {
					typedef mse::impl::remove_reference_t<decltype(*ptr)> element_t;
					//auto num_items = num_bytes / sizeof(element_t);
					//assert(1 == num_items);
					//assert(num_items * sizeof(element_t) == num_bytes);
					*ptr = memset_adjusted_value1<element_t>(typename std::is_assignable<element_t, long long int>::type(), value);
				}
			}
		}
		/* Memory safe approximation of memset(). */
		template<class _TIter>
		void memset(_TIter iter, int value, size_t num_bytes) {
			impl::us::memset_helper1(typename impl::HasOrInheritsSubscriptOperator<_TIter>::type(), iter, value, num_bytes);
		}
	}
	namespace us {
		namespace lh {
			namespace impl {
				namespace ns_unsafe_cast {

					/* "C-style" (unsafe) casts can convert a native pointer to a native pointer to an incompatible type. It cannot
					convert an object that is not a native pointer(/reference) to an object of incompatible type. The "safe"
					pointers in the library are (often) objects, not native pointers, but for compatibility with legacy code we
					provide a function that that can (unsafely) convert the library's safe pointers (and iterators) to corresponding
					pointers (or iterators) pointing to incompatible types. */

					/* The 'are_compatible_pointer_objects' struct is used to determine if two "safe" pointer (or iterator) objects
					correspond sufficiently to support (unsafe) conversion between them. Currently, the only supported objects are
					mse::lh::TLHNullableAnyPointer<> and mse::lh::TLHNullableAnyRandomAccessIterator<>. */
					template<typename _Ty, typename _Ty2, typename _TyPointee, typename _Ty2Pointee>
					struct are_compatible_pointer_objects_helper2 : mse::impl::disjunction<
						mse::impl::conjunction<std::is_convertible<_Ty*, const mse::lh::TLHNullableAnyRandomAccessIterator<_TyPointee>*>
							, std::is_convertible<_Ty2*, const mse::lh::TLHNullableAnyRandomAccessIterator<_Ty2Pointee>*> >
						, mse::impl::conjunction<std::is_convertible<_Ty*, const mse::lh::TLHNullableAnyPointer<_TyPointee>*>
							, std::is_convertible<_Ty2*, const mse::lh::TLHNullableAnyPointer<_Ty2Pointee>*> >
					>::type {};

					template<typename T1, typename _Ty, typename _Ty2>
					struct are_compatible_pointer_objects_helper1 : std::false_type {};
					template<typename _Ty, typename _Ty2>
					struct are_compatible_pointer_objects_helper1<std::true_type, _Ty, _Ty2> : are_compatible_pointer_objects_helper2<_Ty, _Ty2
						, mse::impl::remove_reference_t<decltype(*std::declval<_Ty>())>, mse::impl::remove_reference_t<decltype(*std::declval<_Ty2>())> > {};

					template<typename _Ty, typename _Ty2>
					struct are_compatible_pointer_objects : are_compatible_pointer_objects_helper1<typename mse::impl::conjunction<
							mse::impl::IsDereferenceable_msemsearray<_Ty>, mse::impl::IsDereferenceable_msemsearray<_Ty2> >::type, _Ty, _Ty2> {};

					template<typename _Ty, typename _Ty2>
					_Ty unsafe_cast_helper2(std::false_type, _Ty2& x) {
						return (_Ty)(x);
					}
					template<typename _Ty, typename _Ty2>
					_Ty unsafe_cast_helper2(std::true_type, _Ty2& x) {
						return std::forward<_Ty>(reinterpret_cast<_Ty&&>(x));
					}

					template<typename _Ty, typename _Ty2>
					_Ty unsafe_cast_helper1(std::false_type, _Ty2& x) {
						return unsafe_cast_helper2<_Ty>(typename ns_unsafe_cast::are_compatible_pointer_objects<_Ty, _Ty2>::type(), x);
					}
					template<typename _Ty, typename _Ty2>
					_Ty unsafe_cast_helper1(std::true_type, const _Ty2& x) {
						return unsafe_cast_helper2<_Ty>(typename ns_unsafe_cast::are_compatible_pointer_objects<_Ty, _Ty2>::type(), const_cast<_Ty2&>(x));
					}
				}
			}
			/* Unlike "C-style" casts, this 'unsafe_cast()' function can (unsafely) convert a library pointer or iterator object
			to a corresponding pointer or iterator object targeting an incompatible type. */
			template<typename _Ty, typename _Ty2>
			auto unsafe_cast(_Ty2&& x) -> decltype(impl::ns_unsafe_cast::unsafe_cast_helper1<_Ty>(typename std::is_rvalue_reference<decltype(x)>::type(), MSE_FWD(x))) {
				return impl::ns_unsafe_cast::unsafe_cast_helper1<_Ty>(typename std::is_rvalue_reference<decltype(x)>::type(), MSE_FWD(x));
			}
			template<typename _Ty, typename _Ty2>
			_Ty unsafe_cast(_Ty2& x) {
				return impl::ns_unsafe_cast::unsafe_cast_helper2<_Ty>(typename impl::ns_unsafe_cast::are_compatible_pointer_objects<_Ty, _Ty2>::type(), x);
			}
		}
	}
}

#endif /*MSE_LEGACYHELPERS_DISABLED*/

#ifdef _MSC_VER
#pragma warning( pop )  
#endif /*_MSC_VER*/

#endif // MSELEGACYHELPERS_H_
