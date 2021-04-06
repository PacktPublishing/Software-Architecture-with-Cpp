
// Copyright (c) 2015 Noah Lopez
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef MSEMSESTRING_H
#define MSEMSESTRING_H

#include <string>
#include <iostream>
#include "msemsevector.h"
#include "msealgorithm.h"
#ifdef MSE_HAS_CXX17
#include <string_view>
#endif /* MSE_HAS_CXX17 */

#ifdef MSE_SAFER_SUBSTITUTES_DISABLED
#define MSE_MSTDSTRING_DISABLED
#endif /*MSE_SAFER_SUBSTITUTES_DISABLED*/

#ifndef NDEBUG
#ifndef MSE_SUPPRESS_MSTD_STRING_CHECK_USE_AFTER_FREE
#define MSE_MSTD_STRING_CHECK_USE_AFTER_FREE
#endif // !MSE_SUPPRESS_MSTD_STRING_CHECK_USE_AFTER_FREE
#endif // !NDEBUG

/* for string_view */
//include <__config>
//include <__string>
#include <algorithm>
#include <iterator>
#include <limits>
#include <stdexcept>
//include <__debug>

#ifdef _MSC_VER
#pragma warning( push )  
#pragma warning( disable : 4505 4522 )
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
#endif //_NOEXCEPT

#ifndef _NOEXCEPT_OP
#define _NOEXCEPT_OP(x)	noexcept(x)
#endif /*_NOEXCEPT_OP*/

namespace mse {

	/* This macro roughly simulates constructor inheritance.*/
#define MSE_MSESTRING_USING(Derived, Base)  MSE_USING(Derived, Base)

	class gnii_basic_string_range_error : public std::range_error {
	public:
		using std::range_error::range_error;
	};
	class gnii_basic_string_null_dereference_error : public std::logic_error {
	public:
		using std::logic_error::logic_error;
	};
	class msebasic_string_range_error : public std::range_error {
	public:
		using std::range_error::range_error;
	};
	class msebasic_string_null_dereference_error : public std::logic_error {
	public:
		using std::logic_error::logic_error;
	};


	/* "String sections" are essentially "random access sections" that support the string output stream operator ("<<").
	So a const string section is the functional equivalent of an std::string_view, with a very similar interface. */

	namespace us {
		namespace impl {
			class StringSectionTagBase {};
			class CagedStringSectionTagBase {};
		}
	}

	template <typename _TRASection, typename _TRAConstSection, class _Traits = std::char_traits<typename _TRASection::value_type> >
	class TStringConstSectionBase;
	template <typename _TRASection, typename _TRAConstSection, class _Traits = std::char_traits<typename _TRASection::value_type> >
	class TStringSectionBase;

	template <typename _TRAIterator, class _Traits = std::char_traits<mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(*(std::declval<_TRAIterator>()))> > > >
	class TXScopeStringSection;
	template <typename _TRAIterator, class _Traits = std::char_traits<mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(*(std::declval<_TRAIterator>()))> > > >
	class TXScopeStringConstSection;
	//template <typename _TRAIterator, class _Traits = std::char_traits<mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(*(std::declval<_TRAIterator>()))> > > >
	//class TXScopeCagedStringSectionToRValue;
	template <typename _TRAIterator, class _Traits = std::char_traits<mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(*(std::declval<_TRAIterator>()))> > > >
	class TXScopeCagedStringConstSectionToRValue;
	template <typename _TRAIterator, class _Traits = std::char_traits<mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(*(std::declval<_TRAIterator>()))> > > >
	class TStringSection;
	template <typename _TRAIterator, class _Traits = std::char_traits<mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(*(std::declval<_TRAIterator>()))> > > >
	class TStringConstSection;
	namespace rsv {
		//template <typename _TRAIterator, class _Traits = std::char_traits<mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(*(std::declval<_TRAIterator>()))> > > >
		//class TXScopeStringSectionFParam;
		template <typename _TRAIterator, class _Traits = std::char_traits<mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(*(std::declval<_TRAIterator>()))> > > >
		class TXScopeStringConstSectionFParam;
	}

	/* "NRP" string sections are just versions of the regular string sections that don't support raw pointers or
	std::basic_string_view<> iterators (for extra safety). */
	template <typename _TRAIterator, class _Traits = std::char_traits<mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(*(std::declval<_TRAIterator>()))> > > >
	class TXScopeNRPStringSection;
	template <typename _TRAIterator, class _Traits = std::char_traits<mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(*(std::declval<_TRAIterator>()))> > > >
	class TXScopeNRPStringConstSection;
	template <typename _TRAIterator, class _Traits = std::char_traits<mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(*(std::declval<_TRAIterator>()))> > > >
	class TXScopeCagedNRPStringConstSectionToRValue;
	template <typename _TRAIterator, class _Traits = std::char_traits<mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(*(std::declval<_TRAIterator>()))> > > >
	class TNRPStringSection;
	template <typename _TRAIterator, class _Traits = std::char_traits<mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(*(std::declval<_TRAIterator>()))> > > >
	class TNRPStringConstSection;
	namespace rsv {
		template <typename _TRAIterator, class _Traits = std::char_traits<mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(*(std::declval<_TRAIterator>()))> > > >
		class TXScopeNRPStringConstSectionFParam;
	}

	namespace impl {
		namespace ns_gnii_basic_string {
			template<typename _TVector>
			class Tgnii_basic_string_xscope_ss_const_iterator_type;
		}
	}

	namespace us {
		namespace impl {
			template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty>, class _TStateMutex = mse::non_thread_safe_shared_mutex, template<typename> class _TTXScopeConstIterator = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_ss_const_iterator_type>
			class gnii_basic_string;
		}
	}

	/* nii_basic_string<> is a string that is eligible to be shared among threads and does not support implicit iterators. */
	template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty> >
	using nii_basic_string = us::impl::gnii_basic_string<_Ty, _Traits, _A, mse::non_thread_safe_shared_mutex>;

	namespace us {
		template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty>, class _TStateMutex = mse::non_thread_safe_shared_mutex>
		class msebasic_string;
	}
	namespace mstd {
		template <typename _Ty, class _Traits>
		class basic_string_view;
	}

	namespace impl {
		template <typename _Ty, class _Traits, typename _A> struct is_contiguous_sequence_container<std::basic_string<_Ty, _Traits, _A> > : std::true_type {};

		namespace ns_gnii_basic_string {
			template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty>, class _TStateMutex = mse::non_thread_safe_shared_mutex>
			std::basic_istream<_Ty, _Traits>& in_from_stream(std::basic_istream<_Ty, _Traits>&& _Istr, mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex>& _Str);
			template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty>, class _TStateMutex = mse::non_thread_safe_shared_mutex>
			std::basic_istream<_Ty, _Traits>& in_from_stream(std::basic_istream<_Ty, _Traits>& _Istr, mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex>& _Str);
			template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty>, class _TStateMutex = mse::non_thread_safe_shared_mutex>
			std::basic_ostream<_Ty, _Traits>& out_to_stream(std::basic_ostream<_Ty, _Traits>& _Ostr, const mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex>& _Str);
		}
	}
}

namespace mse {
	namespace us {
		namespace impl {

			//===------------------------ string_view ---------------------------------===//
			//
			//                     The LLVM Compiler Infrastructure
			//
			// This file is distributed under the University of Illinois Open Source
			// License.
			//
			//===----------------------------------------------------------------------===//

#define _LIBCPP_TEMPLATE_VIS
#define _LIBCPP_INLINE_VISIBILITY
#define _LIBCPP_CONSTEXPR
#define _LIBCPP_CONSTEXPR_AFTER_CXX11
#define _LIBCPP_CONSTEXPR_AFTER_CXX14
#define _LIBCPP_CONSTEXPR_AFTER_CXX17
#define _LIBCPP_NODISCARD
#define _LIBCPP_NODISCARD_AFTER_CXX11
#define _LIBCPP_NODISCARD_AFTER_CXX14
#define _LIBCPP_NODISCARD_AFTER_CXX17
#define _LIBCPP_ASSERT(x, y) assert(x)
#define _VSTD std
#define z_throw_out_of_range(x) MSE_THROW(x)

			typedef decltype(std::declval<int*>() - std::declval<int*>()) ptrdiff_t;

			// z_str_find
			template<class _CharT, class _SizeT, class _Traits, _SizeT z_npos>
			inline _SizeT _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				z_str_find(const _CharT *z_p, _SizeT z_sz,
					_CharT z_c, _SizeT z_pos) _NOEXCEPT
			{
				if (z_pos >= z_sz)
					return z_npos;
				const _CharT* z_r = _Traits::find(z_p + z_pos, z_sz - z_pos, z_c);
				if (z_r == 0)
					return z_npos;
				return static_cast<_SizeT>(z_r - z_p);
			}
			template <class _CharT, class _Traits>
			inline _LIBCPP_CONSTEXPR_AFTER_CXX11 const _CharT *
				z_search_substring(const _CharT *z_first1, const _CharT *z_last1,
					const _CharT *z_first2, const _CharT *z_last2) {
				// Take advantage of knowing source and pattern lengths.
				// Stop short when source is smaller than pattern.
				const ptrdiff_t z_len2 = z_last2 - z_first2;
				if (z_len2 == 0)
					return z_first1;

				ptrdiff_t z_len1 = z_last1 - z_first1;
				if (z_len1 < z_len2)
					return z_last1;

				// First element of z_first2 is loop invariant.
				_CharT z_f2 = *z_first2;
				while (true) {
					z_len1 = z_last1 - z_first1;
					// Check whether z_first1 still has at least z_len2 bytes.
					if (z_len1 < z_len2)
						return z_last1;

					// Find z_f2 the first byte matching in z_first1.
					z_first1 = _Traits::find(z_first1, z_len1 - z_len2 + 1, z_f2);
					if (z_first1 == 0)
						return z_last1;

					// It is faster to compare from the first byte of z_first1 even if we
					// already know that it matches the first byte of z_first2: this is because
					// z_first2 is most likely aligned, as it is user's "pattern" string, and
					// z_first1 + 1 is most likely not aligned, as the match is in the middle of
					// the string.
					if (_Traits::compare(z_first1, z_first2, z_len2) == 0)
						return z_first1;

					++z_first1;
				}
			}

			template<class _CharT, class _SizeT, class _Traits, _SizeT z_npos>
			inline _SizeT _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				z_str_find(const _CharT *z_p, _SizeT z_sz,
					const _CharT* z_s, _SizeT z_pos, _SizeT z_n) _NOEXCEPT
			{
				if (z_pos > z_sz)
					return z_npos;

				if (z_n == 0) // There is nothing to search, just return z_pos.
					return z_pos;

				const _CharT *z_r = z_search_substring<_CharT, _Traits>(
					z_p + z_pos, z_p + z_sz, z_s, z_s + z_n);

				if (z_r == z_p + z_sz)
					return z_npos;
				return static_cast<_SizeT>(z_r - z_p);
			}


			// z_str_rfind

			template<class _CharT, class _SizeT, class _Traits, _SizeT z_npos>
			inline _SizeT _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				z_str_rfind(const _CharT *z_p, _SizeT z_sz,
					_CharT z_c, _SizeT z_pos) _NOEXCEPT
			{
				if (z_sz < 1)
					return z_npos;
				if (z_pos < z_sz)
					++z_pos;
				else
					z_pos = z_sz;
				for (const _CharT* z_ps = z_p + z_pos; z_ps != z_p;)
				{
					if (_Traits::eq(*--z_ps, z_c))
						return static_cast<_SizeT>(z_ps - z_p);
				}
				return z_npos;
			}

			template<class _CharT, class _SizeT, class _Traits, _SizeT z_npos>
			inline _SizeT _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				z_str_rfind(const _CharT *z_p, _SizeT z_sz,
					const _CharT* z_s, _SizeT z_pos, _SizeT z_n) _NOEXCEPT
			{
				z_pos = _VSTD::min(z_pos, z_sz);
				if (z_n < z_sz - z_pos)
					z_pos += z_n;
				else
					z_pos = z_sz;
				const _CharT* z_r = _VSTD::find_end(
					z_p, z_p + z_pos, z_s, z_s + z_n, _Traits::eq,
					std::random_access_iterator_tag(), std::random_access_iterator_tag());
				if (z_n > 0 && z_r == z_p + z_pos)
					return z_npos;
				return static_cast<_SizeT>(z_r - z_p);
			}

			template <class _ForwardIterator1, class _ForwardIterator2, class _BinaryPredicate>
			_LIBCPP_CONSTEXPR_AFTER_CXX11 _ForwardIterator1
				z_find_first_of_ce(_ForwardIterator1 z_first1, _ForwardIterator1 z_last1,
					_ForwardIterator2 z_first2, _ForwardIterator2 z_last2, _BinaryPredicate z_pred)
			{
				for (; z_first1 != z_last1; ++z_first1)
					for (_ForwardIterator2 z_j = z_first2; z_j != z_last2; ++z_j)
						if (z_pred(*z_first1, *z_j))
							return z_first1;
				return z_last1;
			}

			// z_str_find_first_of
			template<class _CharT, class _SizeT, class _Traits, _SizeT z_npos>
			inline _SizeT _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				z_str_find_first_of(const _CharT *z_p, _SizeT z_sz,
					const _CharT* z_s, _SizeT z_pos, _SizeT z_n) _NOEXCEPT
			{
				if (z_pos >= z_sz || z_n == 0)
					return z_npos;
				const _CharT* z_r = /*_VSTD::*/z_find_first_of_ce
				(z_p + z_pos, z_p + z_sz, z_s, z_s + z_n, _Traits::eq);
				if (z_r == z_p + z_sz)
					return z_npos;
				return static_cast<_SizeT>(z_r - z_p);
			}


			// z_str_find_last_of
			template<class _CharT, class _SizeT, class _Traits, _SizeT z_npos>
			inline _SizeT _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				z_str_find_last_of(const _CharT *z_p, _SizeT z_sz,
					const _CharT* z_s, _SizeT z_pos, _SizeT z_n) _NOEXCEPT
			{
				if (z_n != 0)
				{
					if (z_pos < z_sz)
						++z_pos;
					else
						z_pos = z_sz;
					for (const _CharT* z_ps = z_p + z_pos; z_ps != z_p;)
					{
						const _CharT* z_r = _Traits::find(z_s, z_n, *--z_ps);
						if (z_r)
							return static_cast<_SizeT>(z_ps - z_p);
					}
				}
				return z_npos;
			}


			// z_str_find_first_not_of
			template<class _CharT, class _SizeT, class _Traits, _SizeT z_npos>
			inline _SizeT _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				z_str_find_first_not_of(const _CharT *z_p, _SizeT z_sz,
					const _CharT* z_s, _SizeT z_pos, _SizeT z_n) _NOEXCEPT
			{
				if (z_pos < z_sz)
				{
					const _CharT* z_pe = z_p + z_sz;
					for (const _CharT* z_ps = z_p + z_pos; z_ps != z_pe; ++z_ps)
						if (_Traits::find(z_s, z_n, *z_ps) == 0)
							return static_cast<_SizeT>(z_ps - z_p);
				}
				return z_npos;
			}


			template<class _CharT, class _SizeT, class _Traits, _SizeT z_npos>
			inline _SizeT _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				z_str_find_first_not_of(const _CharT *z_p, _SizeT z_sz,
					_CharT z_c, _SizeT z_pos) _NOEXCEPT
			{
				if (z_pos < z_sz)
				{
					const _CharT* z_pe = z_p + z_sz;
					for (const _CharT* z_ps = z_p + z_pos; z_ps != z_pe; ++z_ps)
						if (!_Traits::eq(*z_ps, z_c))
							return static_cast<_SizeT>(z_ps - z_p);
				}
				return z_npos;
			}


			// z_str_find_last_not_of
			template<class _CharT, class _SizeT, class _Traits, _SizeT z_npos>
			inline _SizeT _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				z_str_find_last_not_of(const _CharT *z_p, _SizeT z_sz,
					const _CharT* z_s, _SizeT z_pos, _SizeT z_n) _NOEXCEPT
			{
				if (z_pos < z_sz)
					++z_pos;
				else
					z_pos = z_sz;
				for (const _CharT* z_ps = z_p + z_pos; z_ps != z_p;)
					if (_Traits::find(z_s, z_n, *--z_ps) == 0)
						return static_cast<_SizeT>(z_ps - z_p);
				return z_npos;
			}


			template<class _CharT, class _SizeT, class _Traits, _SizeT z_npos>
			inline _SizeT _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				z_str_find_last_not_of(const _CharT *z_p, _SizeT z_sz,
					_CharT z_c, _SizeT z_pos) _NOEXCEPT
			{
				if (z_pos < z_sz)
					++z_pos;
				else
					z_pos = z_sz;
				for (const _CharT* z_ps = z_p + z_pos; z_ps != z_p;)
					if (!_Traits::eq(*--z_ps, z_c))
						return static_cast<_SizeT>(z_ps - z_p);
				return z_npos;
			}


			template <typename _TRAIterator>
			inline size_t T_Hash_bytes(const _TRAIterator/*unsigned char **/_First, size_t _Count) /*_NOEXCEPT*/
			{	// FNV-1a hash function for bytes in [_First, _First + _Count)
#if defined(_WIN64) || defined(z_x86_64z_) || defined(z_LP64z_) || defined(_LP64) /* todo: make this portable */
				//static_assert(sizeof(size_t) == 8, "This code is for 64-bit size_t.");
				const size_t _FNV_offset_basis = 14695981039346656037ULL;
				const size_t _FNV_prime = 1099511628211ULL;

#else /* defined(_WIN64) */
				//static_assert(sizeof(size_t) == 4, "This code is for 32-bit size_t.");
				const size_t _FNV_offset_basis = 2166136261U;
				const size_t _FNV_prime = 16777619U;
#endif /* defined(_WIN64) */

				size_t _Val = _FNV_offset_basis;
				for (size_t _Next = 0; _Next < _Count; ++_Next)
				{	// fold in another byte
					_Val ^= (size_t)_First[_Next];
					_Val *= _FNV_prime;
				}
				return (_Val);
			}

			template<class _Ptr>
			inline _LIBCPP_INLINE_VISIBILITY
				size_t z_do_string_hash(_Ptr z_p, _Ptr z_e)
			{
				typedef typename std::iterator_traits<_Ptr>::value_type value_type;
				typedef decltype(sizeof(value_type)) l_size_type;
				l_size_type num_bytes = (z_e - z_p) * sizeof(value_type);

				if (true) {
					return T_Hash_bytes(z_p, num_bytes);
				}
				else {
					//return z_murmur2_or_cityhash<size_t>()(z_p, (z_e - z_p) * sizeof(value_type));

					/* Rather than include a hash implementation, we'll just use the existing one for std::string. */
					/* todo: check if C++17 is available, and if so, use std::basic_string_view's hash instead. */
					std::string string1;
					string1.resize(num_bytes);
					char * bytesz_p = reinterpret_cast<char *>(z_p);
					for (l_size_type i = 0; i < num_bytes; i += 1) {
						string1[i] = *bytesz_p;
						++bytesz_p;
					}
					std::hash<std::string> str_hash;
					return str_hash(string1);
				}
			}

			template<class _CharT, class _Traits = std::char_traits<_CharT> >
			class _LIBCPP_TEMPLATE_VIS basic_string_view {
			public:
				// types
				typedef _Traits                                    traits_type;
				typedef _CharT                                     value_type;
				typedef const _CharT*                              pointer;
				typedef const _CharT*                              const_pointer;
				typedef const _CharT&                              reference;
				typedef const _CharT&                              const_reference;
				typedef const_pointer                              const_iterator; // See [string.view.iterators]
				typedef const_iterator                             iterator;
				typedef _VSTD::reverse_iterator<const_iterator>    const_reverse_iterator;
				typedef const_reverse_iterator                     reverse_iterator;
				typedef size_t                                     size_type;
				typedef ptrdiff_t                                  difference_type;
				//static _LIBCPP_CONSTEXPR const size_type npos = -1; // size_type(-1);
				static _LIBCPP_CONSTEXPR const size_type npos = size_type(-1);

				static_assert(std::is_pod<value_type>::value, "Character type of basic_string_view must be a POD");
				static_assert((std::is_same<_CharT, typename traits_type::char_type>::value),
					"traits_type::char_type must be the same type as CharT");

				// [string.view.cons], construct/copy
				_LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
					basic_string_view() _NOEXCEPT : z_data(nullptr), z_size(0) {}

				_LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
					basic_string_view(const basic_string_view&) _NOEXCEPT = default;

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					basic_string_view& operator=(const basic_string_view&) _NOEXCEPT = default;

				_LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
					basic_string_view(const _CharT* z_s, size_type z_len) _NOEXCEPT
					: z_data(z_s), z_size(z_len)
				{
					// #if _LIBCPP_STD_VER > 11
					//         _LIBCPP_ASSERT(z_len == 0 || z_s != nullptr, "string_view::string_view(_CharT *, size_t): received nullptr");
					// #endif
				}

				_LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
					basic_string_view(const _CharT* z_s)
					: z_data(z_s), z_size(_Traits::length(z_s)) {}

				// [string.view.iterators], iterators
				_LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
					const_iterator begin()  const _NOEXCEPT { return cbegin(); }

				_LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
					const_iterator end()    const _NOEXCEPT { return cend(); }

				_LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
					const_iterator cbegin() const _NOEXCEPT { return z_data; }

				_LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
					const_iterator cend()   const _NOEXCEPT { return z_data + z_size; }

				_LIBCPP_CONSTEXPR_AFTER_CXX14 _LIBCPP_INLINE_VISIBILITY
					const_reverse_iterator rbegin()   const _NOEXCEPT { return const_reverse_iterator(cend()); }

				_LIBCPP_CONSTEXPR_AFTER_CXX14 _LIBCPP_INLINE_VISIBILITY
					const_reverse_iterator rend()     const _NOEXCEPT { return const_reverse_iterator(cbegin()); }

				_LIBCPP_CONSTEXPR_AFTER_CXX14 _LIBCPP_INLINE_VISIBILITY
					const_reverse_iterator crbegin()  const _NOEXCEPT { return const_reverse_iterator(cend()); }

				_LIBCPP_CONSTEXPR_AFTER_CXX14 _LIBCPP_INLINE_VISIBILITY
					const_reverse_iterator crend()    const _NOEXCEPT { return const_reverse_iterator(cbegin()); }

				// [string.view.capacity], capacity
				_LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
					size_type size()     const _NOEXCEPT { return z_size; }

				_LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
					size_type length()   const _NOEXCEPT { return z_size; }

				_LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
					size_type max_size() const _NOEXCEPT { return std::numeric_limits<size_type>::max(); }

				_LIBCPP_NODISCARD_AFTER_CXX17 _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR
					bool empty()         const _NOEXCEPT { return z_size == 0; }

				// [string.view.access], element access
				_LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
					const_reference operator[](size_type z_pos) const _NOEXCEPT { return z_data[z_pos]; }

				_LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
					const_reference at(size_type z_pos) const
				{
					return z_pos >= size()
						? (z_throw_out_of_range("string_view::at"), z_data[0])
						: z_data[z_pos];
				}

				_LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
					const_reference front() const
				{
					return _LIBCPP_ASSERT(!empty(), "string_view::front(): string is empty"), z_data[0];
				}

				_LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
					const_reference back() const
				{
					return _LIBCPP_ASSERT(!empty(), "string_view::back(): string is empty"), z_data[z_size - 1];
				}

				_LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
					const_pointer data() const _NOEXCEPT { return z_data; }

				// [string.view.modifiers], modifiers:
				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					void remove_prefix(size_type z_n) _NOEXCEPT
				{
					_LIBCPP_ASSERT(z_n <= size(), "remove_prefix() can't remove more than size()");
					z_data += z_n;
					z_size -= z_n;
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					void remove_suffix(size_type z_n) _NOEXCEPT
				{
					_LIBCPP_ASSERT(z_n <= size(), "remove_suffix() can't remove more than size()");
					z_size -= z_n;
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					void swap(basic_string_view& z_other) _NOEXCEPT
				{
					const value_type *z_p = z_data;
					z_data = z_other.z_data;
					z_other.z_data = z_p;

					size_type z_sz = z_size;
					z_size = z_other.z_size;
					z_other.z_size = z_sz;
				}

				_LIBCPP_INLINE_VISIBILITY
					size_type copy(_CharT* z_s, size_type z_n, size_type z_pos = 0) const
				{
					if (z_pos > size())
						z_throw_out_of_range("string_view::copy");
					size_type z_rlen = _VSTD::min(z_n, size() - z_pos);
					_Traits::copy(z_s, data() + z_pos, z_rlen);
					return z_rlen;
				}

				_LIBCPP_CONSTEXPR _LIBCPP_INLINE_VISIBILITY
					basic_string_view substr(size_type z_pos = 0, size_type z_n = npos) const
				{
					return z_pos > size()
						? (z_throw_out_of_range("string_view::substr"), basic_string_view())
						: basic_string_view(data() + z_pos, _VSTD::min(z_n, size() - z_pos));
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 int compare(basic_string_view z_sv) const _NOEXCEPT
				{
					size_type z_rlen = _VSTD::min(size(), z_sv.size());
					int z_retval = _Traits::compare(data(), z_sv.data(), z_rlen);
					if (z_retval == 0) // first z_rlen chars matched
						z_retval = size() == z_sv.size() ? 0 : (size() < z_sv.size() ? -1 : 1);
					return z_retval;
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					int compare(size_type z_pos1, size_type z_n1, basic_string_view z_sv) const
				{
					return substr(z_pos1, z_n1).compare(z_sv);
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					int compare(size_type z_pos1, size_type z_n1,
						basic_string_view z_sv, size_type z_pos2, size_type z_n2) const
				{
					return substr(z_pos1, z_n1).compare(z_sv.substr(z_pos2, z_n2));
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					int compare(const _CharT* z_s) const _NOEXCEPT
				{
					return compare(basic_string_view(z_s));
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					int compare(size_type z_pos1, size_type z_n1, const _CharT* z_s) const
				{
					return substr(z_pos1, z_n1).compare(basic_string_view(z_s));
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					int compare(size_type z_pos1, size_type z_n1, const _CharT* z_s, size_type z_n2) const
				{
					return substr(z_pos1, z_n1).compare(basic_string_view(z_s, z_n2));
				}

				// find
				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					size_type find(basic_string_view z_s, size_type z_pos = 0) const _NOEXCEPT
				{
					_LIBCPP_ASSERT(z_s.size() == 0 || z_s.data() != nullptr, "string_view::find(): received nullptr");
					return z_str_find<value_type, size_type, traits_type, npos>
						(data(), size(), z_s.data(), z_pos, z_s.size());
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					size_type find(_CharT z_c, size_type z_pos = 0) const _NOEXCEPT
				{
					return z_str_find<value_type, size_type, traits_type, npos>
						(data(), size(), z_c, z_pos);
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					size_type find(const _CharT* z_s, size_type z_pos, size_type z_n) const
				{
					_LIBCPP_ASSERT(z_n == 0 || z_s != nullptr, "string_view::find(): received nullptr");
					return z_str_find<value_type, size_type, traits_type, npos>
						(data(), size(), z_s, z_pos, z_n);
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					size_type find(const _CharT* z_s, size_type z_pos = 0) const
				{
					_LIBCPP_ASSERT(z_s != nullptr, "string_view::find(): received nullptr");
					return z_str_find<value_type, size_type, traits_type, npos>
						(data(), size(), z_s, z_pos, traits_type::length(z_s));
				}

				// rfind
				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					size_type rfind(basic_string_view z_s, size_type z_pos = npos) const _NOEXCEPT
				{
					_LIBCPP_ASSERT(z_s.size() == 0 || z_s.data() != nullptr, "string_view::find(): received nullptr");
					return z_str_rfind<value_type, size_type, traits_type, npos>
						(data(), size(), z_s.data(), z_pos, z_s.size());
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					size_type rfind(_CharT z_c, size_type z_pos = npos) const _NOEXCEPT
				{
					return z_str_rfind<value_type, size_type, traits_type, npos>
						(data(), size(), z_c, z_pos);
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					size_type rfind(const _CharT* z_s, size_type z_pos, size_type z_n) const
				{
					_LIBCPP_ASSERT(z_n == 0 || z_s != nullptr, "string_view::rfind(): received nullptr");
					return z_str_rfind<value_type, size_type, traits_type, npos>
						(data(), size(), z_s, z_pos, z_n);
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					size_type rfind(const _CharT* z_s, size_type z_pos = npos) const
				{
					_LIBCPP_ASSERT(z_s != nullptr, "string_view::rfind(): received nullptr");
					return z_str_rfind<value_type, size_type, traits_type, npos>
						(data(), size(), z_s, z_pos, traits_type::length(z_s));
				}

				// find_first_of
				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					size_type find_first_of(basic_string_view z_s, size_type z_pos = 0) const _NOEXCEPT
				{
					_LIBCPP_ASSERT(z_s.size() == 0 || z_s.data() != nullptr, "string_view::find_first_of(): received nullptr");
					return z_str_find_first_of<value_type, size_type, traits_type, npos>
						(data(), size(), z_s.data(), z_pos, z_s.size());
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					size_type find_first_of(_CharT z_c, size_type z_pos = 0) const _NOEXCEPT
				{
					return find(z_c, z_pos);
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					size_type find_first_of(const _CharT* z_s, size_type z_pos, size_type z_n) const
				{
					_LIBCPP_ASSERT(z_n == 0 || z_s != nullptr, "string_view::find_first_of(): received nullptr");
					return z_str_find_first_of<value_type, size_type, traits_type, npos>
						(data(), size(), z_s, z_pos, z_n);
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					size_type find_first_of(const _CharT* z_s, size_type z_pos = 0) const
				{
					_LIBCPP_ASSERT(z_s != nullptr, "string_view::find_first_of(): received nullptr");
					return z_str_find_first_of<value_type, size_type, traits_type, npos>
						(data(), size(), z_s, z_pos, traits_type::length(z_s));
				}

				// find_last_of
				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					size_type find_last_of(basic_string_view z_s, size_type z_pos = npos) const _NOEXCEPT
				{
					_LIBCPP_ASSERT(z_s.size() == 0 || z_s.data() != nullptr, "string_view::find_last_of(): received nullptr");
					return z_str_find_last_of<value_type, size_type, traits_type, npos>
						(data(), size(), z_s.data(), z_pos, z_s.size());
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					size_type find_last_of(_CharT z_c, size_type z_pos = npos) const _NOEXCEPT
				{
					return rfind(z_c, z_pos);
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					size_type find_last_of(const _CharT* z_s, size_type z_pos, size_type z_n) const
				{
					_LIBCPP_ASSERT(z_n == 0 || z_s != nullptr, "string_view::find_last_of(): received nullptr");
					return z_str_find_last_of<value_type, size_type, traits_type, npos>
						(data(), size(), z_s, z_pos, z_n);
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					size_type find_last_of(const _CharT* z_s, size_type z_pos = npos) const
				{
					_LIBCPP_ASSERT(z_s != nullptr, "string_view::find_last_of(): received nullptr");
					return z_str_find_last_of<value_type, size_type, traits_type, npos>
						(data(), size(), z_s, z_pos, traits_type::length(z_s));
				}

				// find_first_not_of
				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					size_type find_first_not_of(basic_string_view z_s, size_type z_pos = 0) const _NOEXCEPT
				{
					_LIBCPP_ASSERT(z_s.size() == 0 || z_s.data() != nullptr, "string_view::find_first_not_of(): received nullptr");
					return z_str_find_first_not_of<value_type, size_type, traits_type, npos>
						(data(), size(), z_s.data(), z_pos, z_s.size());
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					size_type find_first_not_of(_CharT z_c, size_type z_pos = 0) const _NOEXCEPT
				{
					return z_str_find_first_not_of<value_type, size_type, traits_type, npos>
						(data(), size(), z_c, z_pos);
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					size_type find_first_not_of(const _CharT* z_s, size_type z_pos, size_type z_n) const
				{
					_LIBCPP_ASSERT(z_n == 0 || z_s != nullptr, "string_view::find_first_not_of(): received nullptr");
					return z_str_find_first_not_of<value_type, size_type, traits_type, npos>
						(data(), size(), z_s, z_pos, z_n);
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					size_type find_first_not_of(const _CharT* z_s, size_type z_pos = 0) const
				{
					_LIBCPP_ASSERT(z_s != nullptr, "string_view::find_first_not_of(): received nullptr");
					return z_str_find_first_not_of<value_type, size_type, traits_type, npos>
						(data(), size(), z_s, z_pos, traits_type::length(z_s));
				}

				// find_last_not_of
				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					size_type find_last_not_of(basic_string_view z_s, size_type z_pos = npos) const _NOEXCEPT
				{
					_LIBCPP_ASSERT(z_s.size() == 0 || z_s.data() != nullptr, "string_view::find_last_not_of(): received nullptr");
					return z_str_find_last_not_of<value_type, size_type, traits_type, npos>
						(data(), size(), z_s.data(), z_pos, z_s.size());
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					size_type find_last_not_of(_CharT z_c, size_type z_pos = npos) const _NOEXCEPT
				{
					return z_str_find_last_not_of<value_type, size_type, traits_type, npos>
						(data(), size(), z_c, z_pos);
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					size_type find_last_not_of(const _CharT* z_s, size_type z_pos, size_type z_n) const
				{
					_LIBCPP_ASSERT(z_n == 0 || z_s != nullptr, "string_view::find_last_not_of(): received nullptr");
					return z_str_find_last_not_of<value_type, size_type, traits_type, npos>
						(data(), size(), z_s, z_pos, z_n);
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					size_type find_last_not_of(const _CharT* z_s, size_type z_pos = npos) const
				{
					_LIBCPP_ASSERT(z_s != nullptr, "string_view::find_last_not_of(): received nullptr");
					return z_str_find_last_not_of<value_type, size_type, traits_type, npos>
						(data(), size(), z_s, z_pos, traits_type::length(z_s));
				}

#if 1//_LIBCPP_STD_VER > 17
				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					bool starts_with(basic_string_view z_s) const _NOEXCEPT
				{
					return size() >= z_s.size() && compare(0, z_s.size(), z_s) == 0;
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					bool starts_with(value_type z_c) const _NOEXCEPT
				{
					return !empty() && _Traits::eq(front(), z_c);
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					bool starts_with(const value_type* z_s) const _NOEXCEPT
				{
					return starts_with(basic_string_view(z_s));
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					bool ends_with(basic_string_view z_s) const _NOEXCEPT
				{
					return size() >= z_s.size() && compare(size() - z_s.size(), npos, z_s) == 0;
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					bool ends_with(value_type z_c) const _NOEXCEPT
				{
					return !empty() && _Traits::eq(back(), z_c);
				}

				_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
					bool ends_with(const value_type* z_s) const _NOEXCEPT
				{
					return ends_with(basic_string_view(z_s));
				}
#endif

			private:
				const   value_type* z_data;
				size_type           z_size;
			};


			// [string.view.comparison]
			// operator ==
			template<class _CharT, class _Traits>
			_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				bool operator==(basic_string_view<_CharT, _Traits> z_lhs,
					basic_string_view<_CharT, _Traits> z_rhs) _NOEXCEPT
			{
				if (z_lhs.size() != z_rhs.size()) return false;
				return z_lhs.compare(z_rhs) == 0;
			}

			template<class _CharT, class _Traits>
			_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				bool operator==(basic_string_view<_CharT, _Traits> z_lhs,
					typename std::common_type<basic_string_view<_CharT, _Traits> >::type z_rhs) _NOEXCEPT
			{
				if (z_lhs.size() != z_rhs.size()) return false;
				return z_lhs.compare(z_rhs) == 0;
			}

			template<class _CharT, class _Traits>
			_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				bool operator==(typename std::common_type<basic_string_view<_CharT, _Traits> >::type z_lhs,
					basic_string_view<_CharT, _Traits> z_rhs) _NOEXCEPT
			{
				if (z_lhs.size() != z_rhs.size()) return false;
				return z_lhs.compare(z_rhs) == 0;
			}


			// operator !=
			template<class _CharT, class _Traits>
			_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				bool operator!=(basic_string_view<_CharT, _Traits> z_lhs, basic_string_view<_CharT, _Traits> z_rhs) _NOEXCEPT
			{
				if (z_lhs.size() != z_rhs.size())
					return true;
				return z_lhs.compare(z_rhs) != 0;
			}

			template<class _CharT, class _Traits>
			_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				bool operator!=(basic_string_view<_CharT, _Traits> z_lhs,
					typename std::common_type<basic_string_view<_CharT, _Traits> >::type z_rhs) _NOEXCEPT
			{
				if (z_lhs.size() != z_rhs.size())
					return true;
				return z_lhs.compare(z_rhs) != 0;
			}

			template<class _CharT, class _Traits>
			_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				bool operator!=(typename std::common_type<basic_string_view<_CharT, _Traits> >::type z_lhs,
					basic_string_view<_CharT, _Traits> z_rhs) _NOEXCEPT
			{
				if (z_lhs.size() != z_rhs.size())
					return true;
				return z_lhs.compare(z_rhs) != 0;
			}


			// operator <
			template<class _CharT, class _Traits>
			_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				bool operator<(basic_string_view<_CharT, _Traits> z_lhs, basic_string_view<_CharT, _Traits> z_rhs) _NOEXCEPT
			{
				return z_lhs.compare(z_rhs) < 0;
			}

			template<class _CharT, class _Traits>
			_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				bool operator<(basic_string_view<_CharT, _Traits> z_lhs,
					typename std::common_type<basic_string_view<_CharT, _Traits> >::type z_rhs) _NOEXCEPT
			{
				return z_lhs.compare(z_rhs) < 0;
			}

			template<class _CharT, class _Traits>
			_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				bool operator<(typename std::common_type<basic_string_view<_CharT, _Traits> >::type z_lhs,
					basic_string_view<_CharT, _Traits> z_rhs) _NOEXCEPT
			{
				return z_lhs.compare(z_rhs) < 0;
			}


			// operator >
			template<class _CharT, class _Traits>
			_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				bool operator> (basic_string_view<_CharT, _Traits> z_lhs, basic_string_view<_CharT, _Traits> z_rhs) _NOEXCEPT
			{
				return z_lhs.compare(z_rhs) > 0;
			}

			template<class _CharT, class _Traits>
			_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				bool operator>(basic_string_view<_CharT, _Traits> z_lhs,
					typename std::common_type<basic_string_view<_CharT, _Traits> >::type z_rhs) _NOEXCEPT
			{
				return z_lhs.compare(z_rhs) > 0;
			}

			template<class _CharT, class _Traits>
			_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				bool operator>(typename std::common_type<basic_string_view<_CharT, _Traits> >::type z_lhs,
					basic_string_view<_CharT, _Traits> z_rhs) _NOEXCEPT
			{
				return z_lhs.compare(z_rhs) > 0;
			}


			// operator <=
			template<class _CharT, class _Traits>
			_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				bool operator<=(basic_string_view<_CharT, _Traits> z_lhs, basic_string_view<_CharT, _Traits> z_rhs) _NOEXCEPT
			{
				return z_lhs.compare(z_rhs) <= 0;
			}

			template<class _CharT, class _Traits>
			_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				bool operator<=(basic_string_view<_CharT, _Traits> z_lhs,
					typename std::common_type<basic_string_view<_CharT, _Traits> >::type z_rhs) _NOEXCEPT
			{
				return z_lhs.compare(z_rhs) <= 0;
			}

			template<class _CharT, class _Traits>
			_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				bool operator<=(typename std::common_type<basic_string_view<_CharT, _Traits> >::type z_lhs,
					basic_string_view<_CharT, _Traits> z_rhs) _NOEXCEPT
			{
				return z_lhs.compare(z_rhs) <= 0;
			}


			// operator >=
			template<class _CharT, class _Traits>
			_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				bool operator>=(basic_string_view<_CharT, _Traits> z_lhs, basic_string_view<_CharT, _Traits> z_rhs) _NOEXCEPT
			{
				return z_lhs.compare(z_rhs) >= 0;
			}


			template<class _CharT, class _Traits>
			_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				bool operator>=(basic_string_view<_CharT, _Traits> z_lhs,
					typename std::common_type<basic_string_view<_CharT, _Traits> >::type z_rhs) _NOEXCEPT
			{
				return z_lhs.compare(z_rhs) >= 0;
			}

			template<class _CharT, class _Traits>
			_LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
				bool operator>=(typename std::common_type<basic_string_view<_CharT, _Traits> >::type z_lhs,
					basic_string_view<_CharT, _Traits> z_rhs) _NOEXCEPT
			{
				return z_lhs.compare(z_rhs) >= 0;
			}

			typedef basic_string_view<char>     string_view;
			typedef basic_string_view<char16_t> u16string_view;
			typedef basic_string_view<char32_t> u32string_view;
			typedef basic_string_view<wchar_t>  wstring_view;

#if 0//_LIBCPP_STD_VER > 11 
			inline namespace literals
			{
				inline namespace string_view_literals
				{
					inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR
						basic_string_view<char> operator "" sv(const char *z_str, size_t z_len) _NOEXCEPT
					{
						return basic_string_view<char>(z_str, z_len);
					}

					inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR
						basic_string_view<wchar_t> operator "" sv(const wchar_t *z_str, size_t z_len) _NOEXCEPT
					{
						return basic_string_view<wchar_t>(z_str, z_len);
					}

					inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR
						basic_string_view<char16_t> operator "" sv(const char16_t *z_str, size_t z_len) _NOEXCEPT
					{
						return basic_string_view<char16_t>(z_str, z_len);
					}

					inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR
						basic_string_view<char32_t> operator "" sv(const char32_t *z_str, size_t z_len) _NOEXCEPT
					{
						return basic_string_view<char32_t>(z_str, z_len);
					}
				}
			}
#endif
		}
	}
}

namespace std {
	// [string.view.hash]
	template<class _CharT, class _Traits>
	struct _LIBCPP_TEMPLATE_VIS hash<mse::us::impl::basic_string_view<_CharT, _Traits> >
		/*: public std::unary_function<mse::us::impl::basic_string_view<_CharT, _Traits>, size_t>*/
	{
		size_t operator()(const mse::us::impl::basic_string_view<_CharT, _Traits> z_val) const _NOEXCEPT;
	};

	template<class _CharT, class _Traits>
	size_t
		hash<mse::us::impl::basic_string_view<_CharT, _Traits> >::operator()(
			const mse::us::impl::basic_string_view<_CharT, _Traits> z_val) const _NOEXCEPT
	{
		return mse::us::impl::z_do_string_hash(z_val.data(), z_val.data() + z_val.size());
	}
}

namespace mse {

	/* "String sections" are essentially "random access sections" that support the string output stream operator ("<<").
	So a const string section is the functional equivalent of an std::string_view, with a very similar interface. */

#define MSE_INHERITED_STRING_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class) \
	MSE_INHERITED_RANDOM_ACCESS_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class); \
	typedef typename base_class::traits_type traits_type;

	/* These are some free functions to obtain a substring_section of a given string_section. */
	template <typename _TSection>
	auto make_xscope_substr(const _TSection& xs_section, typename _TSection::size_type pos = 0, typename _TSection::size_type n = _TSection::npos)
		-> decltype(make_xscope_subsection(xs_section, pos, n)) {
		return make_xscope_subsection(xs_section, pos, n);
	}
	template <typename _TSection>
	auto make_substr(const _TSection& section, typename _TSection::size_type pos = 0, typename _TSection::size_type n = _TSection::npos)
		-> decltype(make_subsection(section, pos, n)) {
		return make_subsection(section, pos, n);
	}

	namespace impl {
		namespace ra_section {
			template <typename _Ty> using mkxsscsh1_TRAIterator = mse::impl::remove_reference_t<decltype(mse::us::impl::TRandomAccessConstSectionBase<char *>::s_xscope_iter_from_lone_param(std::declval<mse::TXScopeFixedConstPointer<_Ty> >()))>;
			template <typename _Ty> using mkxsscsh1_ReturnType = mse::TXScopeCagedStringConstSectionToRValue<mkxsscsh1_TRAIterator<_Ty> >;

			template <typename _Ty> auto make_xscope_string_const_section_helper1(std::true_type, const TXScopeCagedItemFixedConstPointerToRValue<_Ty>& param)
				->impl::ra_section::mkxsscsh1_ReturnType<_Ty>;
			template <typename _Ty> auto make_xscope_string_const_section_helper1(std::true_type, TXScopeCagedItemFixedConstPointerToRValue<_Ty>&& param)
				->impl::ra_section::mkxsscsh1_ReturnType<_Ty>;
			template <typename _TRALoneParam> auto make_xscope_string_const_section_helper1(std::false_type, const _TRALoneParam& param);
		}
	}
	template <typename _TRALoneParam> auto make_xscope_string_const_section(const _TRALoneParam& param) -> decltype(mse::impl::ra_section::make_xscope_string_const_section_helper1(
		typename mse::impl::is_instantiation_of<_TRALoneParam, mse::TXScopeCagedItemFixedConstPointerToRValue>::type(), param));
	template <typename _TRALoneParam, class = MSE_IMPL_ENABLE_IF_NOT_RETURNABLE_FPARAM(_TRALoneParam)> auto make_xscope_string_const_section(_TRALoneParam&& param) -> decltype(mse::impl::ra_section::make_xscope_string_const_section_helper1(
		typename mse::impl::is_instantiation_of<_TRALoneParam, mse::TXScopeCagedItemFixedConstPointerToRValue>::type(), MSE_FWD(param)));

	template <typename _TRASection, typename _TRAConstSection, class _Traits>
	class TStringSectionBase : public _TRASection, public mse::us::impl::StringSectionTagBase {
	public:
		typedef _TRASection base_class;
		typedef _Traits traits_type;
		typedef typename base_class::iterator_type iterator_type;
		typedef iterator_type _TRAIterator;
		MSE_INHERITED_RANDOM_ACCESS_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);

		TStringSectionBase(const TStringSectionBase& src) : base_class(static_cast<const base_class&>(src)) {}
		TStringSectionBase(const base_class& src) : base_class(src) {}
		TStringSectionBase(const _TRAIterator& start_iter, size_type count) : base_class(start_iter, count) {}
		template <typename _TRALoneParam>
		TStringSectionBase(const _TRALoneParam& param) : base_class(param) {}

		/* The presence of this constructor for native arrays should not be construed as condoning the use of native arrays. */
		template<size_t Tn>
		TStringSectionBase(nonconst_value_type(&native_array)[Tn]) : base_class(native_array, Tn) {}

		template<size_t Tn, typename = typename std::enable_if<1 <= Tn>::type>
		TStringSectionBase(const value_type(&presumed_string_literal)[Tn]) : base_class(presumed_string_literal, Tn) {
			/* "Const" string sections are more appropriate for use with const native arrays, including string literals. */
			if ((1 <= (*this).size()) && (0 == (*this).back())) {
				/* We presume that the argument is a string literal, and remove the terminating null. */
				(*this).remove_suffix(1);
			}
			else {
				/* The last character does not seem to be a null terminator, so presumably the argument is not a string
				literal. */
			}
		}

		template<typename _TRAParam>
		bool equal(const _TRAParam& ra_param) const {
			auto sv = mse::make_xscope_string_const_section(mse::rsv::as_an_fparam(ra_param));
			return base_class::equal(sv);
		}
		template<typename _TRAParam>
		bool equal(size_type pos1, size_type n1, const _TRAParam& ra_param) const {
			auto sv = mse::make_xscope_string_const_section(mse::rsv::as_an_fparam(ra_param));
			return subsection(pos1, n1).equal(sv);
		}
		template<typename _TRAParam>
		bool equal(size_type pos1, size_type n1, const _TRAParam& ra_param, size_type pos2, size_type n2) const {
			auto sv = mse::make_xscope_string_const_section(mse::rsv::as_an_fparam(ra_param));
			return subsection(pos1, n1).equal(sv.subsection(pos2, n2));
		}
		template <typename _TRAIterator2>
		bool equal(size_type pos1, size_type n1, const _TRAIterator2& s, size_type n2) const {
			auto sv = TStringConstSectionBase<TXScopeRandomAccessSection<_TRAIterator2>
				, TXScopeRandomAccessConstSection<_TRAIterator2> >(mse::rsv::as_an_fparam(s), n2);
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
			auto sv = mse::make_xscope_string_const_section(mse::rsv::as_an_fparam(ra_param));
			return base_class::lexicographical_compare(sv);
		}
		template<typename _TRAParam>
		bool lexicographical_compare(size_type pos1, size_type n1, const _TRAParam& ra_param) const {
			auto sv = mse::make_xscope_string_const_section(mse::rsv::as_an_fparam(ra_param));
			return subsection(pos1, n1).lexicographical_compare(sv);
		}
		template<typename _TRAParam>
		bool lexicographical_compare(size_type pos1, size_type n1, const _TRAParam& ra_param, size_type pos2, size_type n2) const {
			auto sv = mse::make_xscope_string_const_section(mse::rsv::as_an_fparam(ra_param));
			return subsection(pos1, n1).lexicographical_compare(sv.subsection(pos2, n2));
		}
		template <typename _TRAIterator2>
		bool lexicographical_compare(size_type pos1, size_type n1, const _TRAIterator2& s, size_type n2) const {
			auto sv = mse::make_xscope_string_const_section(mse::rsv::as_an_fparam(s), n2);
			return subsection(pos1, n1).lexicographical_compare(sv);
		}
		template<typename _TRAParam>
		bool operator<(const _TRAParam& ra_param) const {
			auto sv = mse::make_xscope_string_const_section(mse::rsv::as_an_fparam(ra_param));
			return lexicographical_compare(sv);
		}
		template<typename _TRAParam>
		bool operator>(const _TRAParam& ra_param) const {
			auto sv = mse::make_xscope_string_const_section(mse::rsv::as_an_fparam(ra_param));
			return sv.lexicographical_compare(*this);
		}
		template<typename _TRAParam>
		bool operator<=(const _TRAParam& ra_param) const { return !((*this) > ra_param); }
		template<typename _TRAParam>
		bool operator>=(const _TRAParam& ra_param) const { return !((*this) < ra_param); }

		int compare(const TStringConstSectionBase<_TRASection, _TRAConstSection>& sv) const _NOEXCEPT {
			return base_class::compare(sv);
		}
		int compare(size_type pos1, size_type n1, TStringConstSectionBase<_TRASection, _TRAConstSection> sv) const {
			return subsection(pos1, n1).compare(sv);
		}
		int compare(size_type pos1, size_type n1, TStringConstSectionBase<_TRASection, _TRAConstSection> sv, size_type pos2, size_type n2) const {
			return subsection(pos1, n1).compare(sv.subsection(pos2, n2));
		}
		template <typename _TRAIterator2>
		int compare(size_type pos1, size_type n1, const _TRAIterator2& s, size_type n2) const {
			return subsection(pos1, n1).compare(TStringConstSectionBase<_TRASection, _TRAConstSection>(s, n2));
		}

		size_type find(const TStringConstSectionBase<_TRASection, _TRAConstSection>& s, size_type pos = 0) const _NOEXCEPT {
			return base_class::find(s, pos);
		}
		size_type rfind(const TStringConstSectionBase<_TRASection, _TRAConstSection>& s, size_type pos = npos) const _NOEXCEPT {
			return base_class::rfind(s, pos);
		}
		size_type find_first_of(const TStringConstSectionBase<_TRASection, _TRAConstSection>& s, size_type pos = 0) const _NOEXCEPT {
			return base_class::find_first_of(s, pos);
		}
		size_type find_last_of(const TStringConstSectionBase<_TRASection, _TRAConstSection>& s, size_type pos = npos) const _NOEXCEPT {
			return base_class::find_last_of(s, pos);
		}
		size_type find_first_not_of(const TStringConstSectionBase<_TRASection, _TRAConstSection>& s, size_type pos = 0) const _NOEXCEPT {
			return base_class::find_first_not_of(s, pos);
		}
		size_type find_last_not_of(const TStringConstSectionBase<_TRASection, _TRAConstSection>& s, size_type pos = npos) const _NOEXCEPT {
			return base_class::find_last_not_of(s, pos);
		}

		bool starts_with(const TStringConstSectionBase<_TRASection, _TRAConstSection>& s) const _NOEXCEPT {
			return base_class::starts_with(s);
		}
		bool ends_with(const TStringConstSectionBase<_TRASection, _TRAConstSection>& s) const _NOEXCEPT {
			return base_class::ends_with(s);
		}

		typedef typename base_class::xscope_iterator xscope_iterator;
		typedef typename base_class::xscope_const_iterator xscope_const_iterator;

	protected:
		TStringSectionBase subsection(size_type pos = 0, size_type n = npos) const {
			return base_class::subsection(pos, n);
		}

		typedef typename base_class::iterator iterator;
		typedef typename base_class::const_iterator const_iterator;
		iterator begin() { return base_class::begin(); }
		const_iterator begin() const { return cbegin(); }
		const_iterator cbegin() const { return base_class::cbegin(); }
		iterator end() { return base_class::end(); }
		const_iterator end() const { return cend(); }
		const_iterator cend() const { return base_class::cend(); }

		friend class TXScopeStringSection<_TRAIterator>;
		friend class TStringSection<_TRAIterator>;
		template<typename _TRASection1, typename _TRAConstSection1, class _Traits1> friend class TStringConstSectionBase;
		template<typename _TRASection1, typename _TRAConstSection1, class _Traits1> friend class TStringSectionBase;

		template<class _Ty2, class _Traits2>
		friend std::basic_ostream<_Ty2, _Traits2>& operator<<(std::basic_ostream<_Ty2, _Traits2>& _Ostr, const TStringSectionBase& _Str) {
			/* todo: consider optimizing */
			for (size_t i = 0; i < _Str.length(); i += 1) {
				_Ostr << _Str[i];
			}
			return _Ostr;
		}
	};

	template <typename _TRAIterator, class _Traits>
	class TXScopeStringSection : public TStringSectionBase<TXScopeRandomAccessSection<_TRAIterator>, TXScopeRandomAccessConstSection<_TRAIterator>, _Traits> {
	public:
		typedef TStringSectionBase<TXScopeRandomAccessSection<_TRAIterator>, TXScopeRandomAccessConstSection<_TRAIterator>, _Traits> base_class;
		MSE_INHERITED_STRING_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);

		//MSE_USING(TXScopeStringSection, base_class);
		TXScopeStringSection(const TXScopeStringSection& src) : base_class(static_cast<const base_class&>(src)) {}
		template<class _Ty2 = _TRAIterator, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _TRAIterator>::value) && (mse::impl::is_potentially_not_xscope<_TRAIterator>::value)> MSE_IMPL_EIS >
		TXScopeStringSection(const TStringSection<_TRAIterator, _Traits>& src) : base_class(static_cast<const typename TStringSection<_TRAIterator, _Traits>::base_class&>(src)) {}
		TXScopeStringSection(const base_class& src) : base_class(src) {}
		TXScopeStringSection(const _TRAIterator& start_iter, size_type count) : base_class(start_iter, count) {}
		template <typename _TRALoneParam>
		TXScopeStringSection(const _TRALoneParam& param) : base_class(param) {}

		/* The presence of this constructor for native arrays should not be construed as condoning the use of native arrays. */
		template<size_t Tn>
		TXScopeStringSection(nonconst_value_type(&native_array)[Tn]) : base_class(native_array, Tn) {}

		template<size_t Tn, typename = typename std::enable_if<1 <= Tn>::type>
		TXScopeStringSection(const value_type(&presumed_string_literal)[Tn]) : base_class(presumed_string_literal) {}

		/* use the make_xscope_subsection() free function instead */
		MSE_DEPRECATED TXScopeStringSection<_TRAIterator, _Traits> xscope_subsection(size_type pos = 0, size_type n = npos) const {
			return xscope_subsection_pv(pos, n);
		}
		MSE_DEPRECATED TXScopeStringSection xscope_substr(size_type pos = 0, size_type n = npos) const {
			return xscope_subsection(pos, n);
		}
		/* prefer the make_subsection() free function instead */
		auto subsection(size_type pos = 0, size_type n = npos) const {
			return subsection_pv(pos, n);
		}
		auto substr(size_type pos = 0, size_type n = npos) const {
			return subsection(pos, n);
		}

		typedef typename base_class::xscope_iterator xscope_iterator;
		typedef typename base_class::xscope_const_iterator xscope_const_iterator;

		/* These are here because some standard algorithms require them. Prefer the "xscope_" prefixed versions to
		acknowledge that scope iterators are returned. */
		auto begin() { return (*this).xscope_begin(); }
		auto begin() const { return cbegin(); }
		auto cbegin() const { return (*this).xscope_cbegin(); }
		auto end() { return (*this).xscope_end(); }
		auto end() const { return cend(); }
		auto cend() const { return (*this).xscope_cend(); }

	private:

		TXScopeStringSection xscope_subsection_pv(size_type pos = 0, size_type n = npos) const {
			return base_class::subsection(pos, n);
		}
		typedef mse::impl::conditional_t<mse::impl::is_potentially_xscope<_TRAIterator>::value, TXScopeStringSection, TStringSection<_TRAIterator, _Traits> > subsection_t;
		subsection_t subsection_pv(size_type pos = 0, size_type n = npos) const {
			return base_class::subsection(pos, n);
		}

		TXScopeStringSection<_TRAIterator, _Traits>& operator=(const TXScopeStringSection<_TRAIterator, _Traits>& _Right_cref) = delete;
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		template<class _Ty2, class _Traits2>
		friend std::basic_ostream<_Ty2, _Traits2>& operator<<(std::basic_ostream<_Ty2, _Traits2>& _Ostr, const TXScopeStringSection& _Str) {
			_Ostr << static_cast<const base_class&>(_Str);
			return _Ostr;
		}

		template <typename _TSection>
		friend auto make_xscope_subsection(const _TSection& xs_section, typename _TSection::size_type pos/* = 0*/, typename _TSection::size_type n/* = _TSection::npos*/) -> decltype(xs_section.xscope_subsection_pv(pos, n));
		template <typename _TSection>
		friend auto make_subsection(const _TSection& section, typename _TSection::size_type pos/* = 0*/, typename _TSection::size_type n/* = _TSection::npos*/)
			-> decltype(section.subsection_pv(pos, n));
	};

	template <typename _TRAIterator, class _Traits>
	class TStringSection : public TStringSectionBase<TRandomAccessSection<_TRAIterator>, TRandomAccessConstSection<_TRAIterator>, _Traits> {
	public:
		typedef TStringSectionBase<TRandomAccessSection<_TRAIterator>, TRandomAccessConstSection<_TRAIterator>, _Traits> base_class;
		MSE_INHERITED_STRING_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);

		TStringSection(const TStringSection& src) : base_class(static_cast<const base_class&>(src)) {}
		TStringSection(const base_class& src) : base_class(src) {}
		TStringSection(const _TRAIterator& start_iter, size_type count) : base_class(start_iter, count) {}
		template <typename _TRALoneParam>
		TStringSection(const _TRALoneParam& param) : base_class(param) {}

		/* The presence of this constructor for native arrays should not be construed as condoning the use of native arrays. */
		template<size_t Tn>
		TStringSection(nonconst_value_type(&native_array)[Tn]) : base_class(native_array, Tn) {}

		template<size_t Tn, typename = typename std::enable_if<1 <= Tn>::type>
		TStringSection(const value_type(&presumed_string_literal)[Tn]) : base_class(presumed_string_literal) {}

		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TStringSection() {
			mse::impl::T_valid_if_not_an_xscope_type<_TRAIterator>();
		}

		/* use the make_xscope_subsection() free function instead */
		MSE_DEPRECATED TXScopeStringSection<_TRAIterator, _Traits> xscope_subsection(size_type pos = 0, size_type n = npos) const {
			return xscope_subsection_pv(pos, n);
		}
		MSE_DEPRECATED TXScopeStringSection<_TRAIterator, _Traits> xscope_substr(size_type pos = 0, size_type n = npos) const {
			return xscope_subsection(pos, n);
		}
		/* prefer the make_subsection() free function instead */
		auto subsection(size_type pos = 0, size_type n = npos) const {
			return subsection_pv(pos, n);
		}
		auto substr(size_type pos = 0, size_type n = npos) const {
			return subsection(pos, n);
		}

		typedef typename base_class::iterator iterator;
		typedef typename base_class::const_iterator const_iterator;
		iterator begin() { return base_class::begin(); }
		const_iterator begin() const { return cbegin(); }
		const_iterator cbegin() const { return base_class::cbegin(); }
		iterator end() { return base_class::end(); }
		const_iterator end() const { return cend(); }
		const_iterator cend() const { return base_class::cend(); }
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

		TXScopeStringSection<_TRAIterator> xscope_subsection_pv(size_type pos = 0, size_type n = npos) const {
			return base_class::subsection(pos, n);
		}
		TStringSection subsection_pv(size_type pos = 0, size_type n = npos) const {
			return base_class::subsection(pos, n);
		}

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		friend class TXScopeStringSection<_TRAIterator, _Traits>;
		friend class TXScopeStringConstSection<_TRAIterator, _Traits>;
		friend class TStringConstSection<_TRAIterator, _Traits>;
		template<class _Ty2, class _Traits2>
		friend std::basic_ostream<_Ty2, _Traits2>& operator<<(std::basic_ostream<_Ty2, _Traits2>& _Ostr, const TStringSection& _Str) {
			_Ostr << static_cast<const base_class&>(_Str);
			return _Ostr;
		}

		template <typename _TSection>
		friend auto make_xscope_subsection(const _TSection& xs_section, typename _TSection::size_type pos/* = 0*/, typename _TSection::size_type n/* = _TSection::npos*/) -> decltype(xs_section.xscope_subsection_pv(pos, n));
		template <typename _TSection>
		friend auto make_subsection(const _TSection& section, typename _TSection::size_type pos/* = 0*/, typename _TSection::size_type n/* = _TSection::npos*/)
			-> decltype(section.subsection_pv(pos, n));
	};

	template <typename _TRASection, typename _TRAConstSection, class _Traits>
	class TStringConstSectionBase : public _TRAConstSection, public mse::us::impl::StringSectionTagBase {
	public:
		typedef _TRAConstSection base_class;
		typedef _Traits traits_type;
		typedef typename base_class::iterator_type iterator_type;
		typedef iterator_type _TRAIterator;
		MSE_INHERITED_RANDOM_ACCESS_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);

		TStringConstSectionBase(const TStringConstSectionBase& src) : base_class(static_cast<const base_class&>(src)) {}
		TStringConstSectionBase(const TStringSectionBase<_TRASection, _TRAConstSection>& src) : base_class(static_cast<const base_class&>(src)) {}
		TStringConstSectionBase(const base_class& src) : base_class(src) {}
		TStringConstSectionBase(const _TRAIterator& start_iter, size_type count) : base_class(start_iter, count) {}
		template <typename _TRALoneParam>
		TStringConstSectionBase(const _TRALoneParam& param) : base_class(param) {}

		/* The presence of this constructor for native arrays should not be construed as condoning the use of native arrays. */
		template<size_t Tn>
		TStringConstSectionBase(nonconst_value_type(&native_array)[Tn]) : base_class(native_array, Tn) {}

		template<size_t Tn, typename = typename std::enable_if<1 <= Tn>::type>
		TStringConstSectionBase(const value_type(&presumed_string_literal)[Tn]) : base_class(presumed_string_literal, Tn) {
			if ((1 <= (*this).size()) && (0 == (*this).back())) {
				/* We presume that the argument is a string literal, and remove the terminating null. */
				(*this).remove_suffix(1);
			}
			else {
				/* The last character does not seem to a null terminator, so presumably the argument is not a string
				literal. */
			}
		}

		template<typename _TRAParam>
		bool equal(const _TRAParam& ra_param) const {
			auto sv = mse::make_xscope_string_const_section(mse::rsv::as_an_fparam(ra_param));
			return base_class::equal(sv);
		}
		template<typename _TRAParam>
		bool equal(size_type pos1, size_type n1, const _TRAParam& ra_param) const {
			auto sv = mse::make_xscope_string_const_section(mse::rsv::as_an_fparam(ra_param));
			return subsection(pos1, n1).equal(sv);
		}
		template<typename _TRAParam>
		bool equal(size_type pos1, size_type n1, const _TRAParam& ra_param, size_type pos2, size_type n2) const {
			auto sv = mse::make_xscope_string_const_section(mse::rsv::as_an_fparam(ra_param));
			return subsection(pos1, n1).equal(sv.subsection(pos2, n2));
		}
		template <typename _TRAIterator2>
		bool equal(size_type pos1, size_type n1, const _TRAIterator2& s, size_type n2) const {
			auto sv = TStringConstSectionBase<TXScopeRandomAccessSection<_TRAIterator2>
				, TXScopeRandomAccessConstSection<_TRAIterator2> >(mse::rsv::as_an_fparam(s), n2);
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
			auto sv = mse::make_xscope_string_const_section(mse::rsv::as_an_fparam(ra_param));
			return base_class::lexicographical_compare(sv);
		}
		template<typename _TRAParam>
		bool lexicographical_compare(size_type pos1, size_type n1, const _TRAParam& ra_param) const {
			auto sv = mse::make_xscope_string_const_section(mse::rsv::as_an_fparam(ra_param));
			return subsection(pos1, n1).lexicographical_compare(sv);
		}
		template<typename _TRAParam>
		bool lexicographical_compare(size_type pos1, size_type n1, const _TRAParam& ra_param, size_type pos2, size_type n2) const {
			auto sv = mse::make_xscope_string_const_section(mse::rsv::as_an_fparam(ra_param));
			return subsection(pos1, n1).lexicographical_compare(sv.subsection(pos2, n2));
		}
		template <typename _TRAIterator2>
		bool lexicographical_compare(size_type pos1, size_type n1, const _TRAIterator2& s, size_type n2) const {
			auto sv = mse::make_xscope_string_const_section(mse::rsv::as_an_fparam(s), n2);
			return subsection(pos1, n1).lexicographical_compare(sv);
		}
		template<typename _TRAParam>
		bool operator<(const _TRAParam& ra_param) const {
			auto sv = mse::make_xscope_string_const_section(mse::rsv::as_an_fparam(ra_param));
			return lexicographical_compare(sv);
		}
		template<typename _TRAParam>
		bool operator>(const _TRAParam& ra_param) const {
			auto sv = mse::make_xscope_string_const_section(mse::rsv::as_an_fparam(ra_param));
			return sv.lexicographical_compare(*this);
		}
		template<typename _TRAParam>
		bool operator<=(const _TRAParam& ra_param) const { return !((*this) > ra_param); }
		template<typename _TRAParam>
		bool operator>=(const _TRAParam& ra_param) const { return !((*this) < ra_param); }

		int compare(const TStringConstSectionBase& sv) const _NOEXCEPT {
			return base_class::compare(sv);
		}
		int compare(size_type pos1, size_type n1, TStringConstSectionBase sv) const {
			return subsection(pos1, n1).compare(sv);
		}
		int compare(size_type pos1, size_type n1, TStringConstSectionBase sv, size_type pos2, size_type n2) const {
			return subsection(pos1, n1).compare(sv.subsection(pos2, n2));
		}
		template <typename _TRAIterator2>
		int compare(size_type pos1, size_type n1, const _TRAIterator2& s, size_type n2) const {
			return subsection(pos1, n1).compare(TStringConstSectionBase(s, n2));
		}

		size_type find(const TStringConstSectionBase& s, size_type pos = 0) const _NOEXCEPT {
			return base_class::find(s, pos);
		}
		size_type rfind(const TStringConstSectionBase& s, size_type pos = npos) const _NOEXCEPT {
			return base_class::rfind(s, pos);
		}
		size_type find_first_of(const TStringConstSectionBase& s, size_type pos = 0) const _NOEXCEPT {
			return base_class::find_first_of(s, pos);
		}
		size_type find_last_of(const TStringConstSectionBase& s, size_type pos = npos) const _NOEXCEPT {
			return base_class::find_last_of(s, pos);
		}
		size_type find_first_not_of(const TStringConstSectionBase& s, size_type pos = 0) const _NOEXCEPT {
			return base_class::find_first_not_of(s, pos);
		}
		size_type find_last_not_of(const TStringConstSectionBase& s, size_type pos = npos) const _NOEXCEPT {
			return base_class::find_last_not_of(s, pos);
		}

		bool starts_with(const TStringConstSectionBase& s) const _NOEXCEPT {
			return base_class::starts_with(s);
		}
		bool ends_with(const TStringConstSectionBase& s) const _NOEXCEPT {
			return base_class::ends_with(s);
		}

		//typedef typename base_class::xscope_iterator xscope_iterator;
		typedef typename base_class::xscope_const_iterator xscope_const_iterator;

	protected:
		TStringConstSectionBase subsection(size_type pos = 0, size_type n = npos) const {
			return base_class::subsection(pos, n);
		}

		typedef typename base_class::const_iterator const_iterator;
		const_iterator cbegin() const { return base_class::cbegin(); }
		const_iterator cend() const { return base_class::cend(); }
		const_iterator begin() const { return cbegin(); }
		const_iterator end() const { return cend(); }

		friend class TXScopeStringConstSection<_TRAIterator>;
		friend class TStringConstSection<_TRAIterator>;
		template<typename _TRASection1, typename _TRAConstSection1, class _Traits1> friend class TStringConstSectionBase;
		template<typename _TRASection1, typename _TRAConstSection1, class _Traits1> friend class TStringSectionBase;

		template<class _Ty2, class _Traits2>
		friend std::basic_ostream<_Ty2, _Traits2>& operator<<(std::basic_ostream<_Ty2, _Traits2>& _Ostr, const TStringConstSectionBase& _Str) {
			/* todo: consider optimizing */
			for (size_t i = 0; i < _Str.length(); i += 1) {
				_Ostr << _Str[i];
			}
			return _Ostr;
		}
	};


	template <typename _TRAIterator, class _Traits>
	class TXScopeStringConstSection : public TStringConstSectionBase<TXScopeRandomAccessSection<_TRAIterator>, TXScopeRandomAccessConstSection<_TRAIterator>, _Traits> {
	public:
		typedef TStringConstSectionBase<TXScopeRandomAccessSection<_TRAIterator>, TXScopeRandomAccessConstSection<_TRAIterator>, _Traits> base_class;
		MSE_INHERITED_STRING_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);

		//MSE_USING(TXScopeStringConstSection, base_class);

		TXScopeStringConstSection(const TXScopeStringConstSection& src) : base_class(static_cast<const base_class&>(src)) {}
		template<class _Ty2 = _TRAIterator, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _TRAIterator>::value) && (mse::impl::is_potentially_not_xscope<_TRAIterator>::value)> MSE_IMPL_EIS >
		TXScopeStringConstSection(const TStringConstSection<_TRAIterator, _Traits>& src) : base_class(static_cast<const typename TStringConstSection<_TRAIterator, _Traits>::base_class&>(src)) {}
		TXScopeStringConstSection(const TXScopeStringSection<_TRAIterator, _Traits>& src) : base_class(static_cast<const typename TXScopeStringSection<_TRAIterator, _Traits>::base_class&>(src)) {}
		TXScopeStringConstSection(const base_class& src) : base_class(src) {}
		template<class _Ty2 = _TRAIterator, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _TRAIterator>::value) && (mse::impl::is_potentially_not_xscope<_TRAIterator>::value)> MSE_IMPL_EIS >
		TXScopeStringConstSection(const TStringSection<_TRAIterator, _Traits>& src) : base_class(static_cast<const typename TStringSection<_TRAIterator, _Traits>::base_class&>(src)) {}
		TXScopeStringConstSection(const _TRAIterator& start_iter, size_type count) : base_class(start_iter, count) {}
		template <typename _TRALoneParam>
		TXScopeStringConstSection(const _TRALoneParam& param) : base_class(param) {}

		/* The presence of this constructor for native arrays should not be construed as condoning the use of native arrays. */
		template<size_t Tn>
		TXScopeStringConstSection(nonconst_value_type(&native_array)[Tn]) : base_class(native_array, Tn) {}

		template<size_t Tn, typename = typename std::enable_if<1 <= Tn>::type>
		TXScopeStringConstSection(const value_type(&presumed_string_literal)[Tn]) : base_class(presumed_string_literal) {}

		/* use the make_xscope_subsection() free function instead */
		MSE_DEPRECATED TXScopeStringConstSection xscope_subsection(size_type pos = 0, size_type n = npos) const {
			return xscope_subsection_pv(pos, n);
		}
		MSE_DEPRECATED TXScopeStringConstSection xscope_substr(size_type pos = 0, size_type n = npos) const {
			return xscope_subsection(pos, n);
		}
		/* prefer the make_subsection() free function instead */
		auto subsection(size_type pos = 0, size_type n = npos) const {
			return subsection_pv(pos, n);
		}
		auto substr(size_type pos = 0, size_type n = npos) const {
			return subsection(pos, n);
		}

		//typedef typename base_class::xscope_iterator xscope_iterator;
		typedef typename base_class::xscope_const_iterator xscope_const_iterator;

		/* These are here because some standard algorithms require them. Prefer the "xscope_" prefixed versions to
		acknowledge that scope iterators are returned. */
		auto begin() const { return (*this).xscope_cbegin(); }
		auto cbegin() const { return (*this).xscope_cbegin(); }
		auto end() const { return (*this).xscope_cend(); }
		auto cend() const { return (*this).xscope_cend(); }

	private:

		TXScopeStringConstSection xscope_subsection_pv(size_type pos = 0, size_type n = npos) const {
			return base_class::subsection(pos, n);
		}
		typedef mse::impl::conditional_t<mse::impl::is_potentially_xscope<_TRAIterator>::value, TXScopeStringConstSection, TStringConstSection<_TRAIterator, _Traits> > subsection_t;
		subsection_t subsection_pv(size_type pos = 0, size_type n = npos) const {
			return base_class::subsection(pos, n);
		}

		TXScopeStringConstSection<_TRAIterator, _Traits>& operator=(const TXScopeStringConstSection<_TRAIterator, _Traits>& _Right_cref) = delete;
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		template<class _Ty2, class _Traits2>
		friend std::basic_ostream<_Ty2, _Traits2>& operator<<(std::basic_ostream<_Ty2, _Traits2>& _Ostr, const TXScopeStringConstSection& _Str) {
			_Ostr << static_cast<const base_class&>(_Str);
			return _Ostr;
		}

		template <typename _TSection>
		friend auto make_xscope_subsection(const _TSection& xs_section, typename _TSection::size_type pos/* = 0*/, typename _TSection::size_type n/* = _TSection::npos*/) -> decltype(xs_section.xscope_subsection_pv(pos, n));
		template <typename _TSection>
		friend auto make_subsection(const _TSection& section, typename _TSection::size_type pos/* = 0*/, typename _TSection::size_type n/* = _TSection::npos*/)
			-> decltype(section.subsection_pv(pos, n));
	};

	template <typename _TRAIterator, class _Traits>
	class TStringConstSection : public TStringConstSectionBase<TRandomAccessSection<_TRAIterator>, TRandomAccessConstSection<_TRAIterator>, _Traits> {
	public:
		typedef TStringConstSectionBase<TRandomAccessSection<_TRAIterator>, TRandomAccessConstSection<_TRAIterator>, _Traits> base_class;
		MSE_INHERITED_STRING_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);

		TStringConstSection(const TStringConstSection& src) : base_class(static_cast<const base_class&>(src)) {}
		TStringConstSection(const TStringSection<_TRAIterator, _Traits>& src) : base_class(static_cast<const typename TStringSection<_TRAIterator, _Traits>::base_class&>(src)) {}
		TStringConstSection(const base_class& src) : base_class(src) {}
		TStringConstSection(const _TRAIterator& start_iter, size_type count) : base_class(start_iter, count) {}
		template <typename _TRALoneParam>
		TStringConstSection(const _TRALoneParam& param) : base_class(param) {}

		/* The presence of this constructor for native arrays should not be construed as condoning the use of native arrays. */
		template<size_t Tn>
		TStringConstSection(nonconst_value_type(&native_array)[Tn]) : base_class(native_array, Tn) {}

		template<size_t Tn, typename = typename std::enable_if<1 <= Tn>::type>
		TStringConstSection(const value_type(&presumed_string_literal)[Tn]) : base_class(presumed_string_literal) {}

		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TStringConstSection() {
			mse::impl::T_valid_if_not_an_xscope_type<_TRAIterator>();
		}

		/* use the make_xscope_subsection() free function instead */
		MSE_DEPRECATED TXScopeStringConstSection<_TRAIterator, _Traits> xscope_subsection(size_type pos = 0, size_type n = npos) const {
			return xscope_subsection_pv(pos, n);
		}
		MSE_DEPRECATED TXScopeStringConstSection<_TRAIterator, _Traits> xscope_substr(size_type pos = 0, size_type n = npos) const {
			return xscope_subsection(pos, n);
		}
		/* prefer the make_subsection() free function instead */
		TStringConstSection subsection(size_type pos = 0, size_type n = npos) const {
			return subsection_pv(pos, n);
		}
		TStringConstSection substr(size_type pos = 0, size_type n = npos) const {
			return subsection(pos, n);
		}

		typedef typename base_class::const_iterator const_iterator;
		const_iterator begin() const { return cbegin(); }
		const_iterator cbegin() const { return base_class::cbegin(); }
		const_iterator end() const { return cend(); }
		const_iterator cend() const { return base_class::cend(); }
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

		TXScopeStringConstSection<_TRAIterator, _Traits> xscope_subsection_pv(size_type pos = 0, size_type n = npos) const {
			return base_class::subsection(pos, n);
		}
		TStringConstSection subsection_pv(size_type pos = 0, size_type n = npos) const {
			return base_class::subsection(pos, n);
		}

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		friend class TXScopeStringConstSection<_TRAIterator, _Traits>;
		template<class _Ty2, class _Traits2>
		friend std::basic_ostream<_Ty2, _Traits2>& operator<<(std::basic_ostream<_Ty2, _Traits2>& _Ostr, const TStringConstSection& _Str) {
			_Ostr << static_cast<const base_class&>(_Str);
			return _Ostr;
		}

		template <typename _TSection>
		friend auto make_xscope_subsection(const _TSection& xs_section, typename _TSection::size_type pos/* = 0*/, typename _TSection::size_type n/* = _TSection::npos*/) -> decltype(xs_section.xscope_subsection_pv(pos, n));
		template <typename _TSection>
		friend auto make_subsection(const _TSection& section, typename _TSection::size_type pos/* = 0*/, typename _TSection::size_type n/* = _TSection::npos*/)
			-> decltype(section.subsection_pv(pos, n));
	};

	template <typename _TRAIterator>
	auto make_xscope_string_const_section(const _TRAIterator& start_iter, typename TXScopeStringConstSection<_TRAIterator>::size_type count) {
		return TXScopeStringConstSection<_TRAIterator>(start_iter, count);
	}
	namespace impl {
		namespace ra_section {
			template <typename _Ty>
			auto make_xscope_string_const_section_helper1(std::true_type, const TXScopeCagedItemFixedConstPointerToRValue<_Ty>& param)
				-> impl::ra_section::mkxsscsh1_ReturnType<_Ty> {
				mse::TXScopeFixedConstPointer<_Ty> adj_param = mse::rsv::TXScopeFixedConstPointerFParam<_Ty>(param);
				typedef mse::impl::remove_reference_t<decltype(mse::us::impl::TRandomAccessConstSectionBase<char *>::s_xscope_iter_from_lone_param(adj_param))> _TRAIterator;
				mse::TXScopeStringConstSection<_TRAIterator> ra_section(adj_param);
				return mse::TXScopeCagedStringConstSectionToRValue<_TRAIterator>(ra_section);
			}
			template <typename _Ty>
			auto make_xscope_string_const_section_helper1(std::true_type, TXScopeCagedItemFixedConstPointerToRValue<_Ty>&& param)
				-> impl::ra_section::mkxsscsh1_ReturnType<_Ty> {
				mse::TXScopeFixedConstPointer<_Ty> adj_param = mse::rsv::TXScopeFixedConstPointerFParam<_Ty>(MSE_FWD(param));
				typedef mse::impl::remove_reference_t<decltype(mse::us::impl::TRandomAccessConstSectionBase<char*>::s_xscope_iter_from_lone_param(adj_param))> _TRAIterator;
				mse::TXScopeStringConstSection<_TRAIterator> ra_section(adj_param);
				return mse::TXScopeCagedStringConstSectionToRValue<_TRAIterator>(ra_section);
			}
			template <typename _TRALoneParam>
			auto make_xscope_string_const_section_helper1(std::false_type, const _TRALoneParam& param) {
				typedef mse::impl::remove_reference_t<decltype(mse::us::impl::TRandomAccessConstSectionBase<char *>::s_xscope_iter_from_lone_param(param))> _TRAIterator;
				return TXScopeStringConstSection<_TRAIterator>(param);
			}
		}
	}
	template <typename _TRALoneParam>
	auto make_xscope_string_const_section(const _TRALoneParam& param) -> decltype(mse::impl::ra_section::make_xscope_string_const_section_helper1(
		typename mse::impl::is_instantiation_of<_TRALoneParam, mse::TXScopeCagedItemFixedConstPointerToRValue>::type(), param)) {
		return mse::impl::ra_section::make_xscope_string_const_section_helper1(
			typename mse::impl::is_instantiation_of<_TRALoneParam, mse::TXScopeCagedItemFixedConstPointerToRValue>::type(), param);
	}
	template <typename _TRALoneParam, class>
	auto make_xscope_string_const_section(_TRALoneParam&& param) -> decltype(mse::impl::ra_section::make_xscope_string_const_section_helper1(
		typename mse::impl::is_instantiation_of<_TRALoneParam, mse::TXScopeCagedItemFixedConstPointerToRValue>::type(), MSE_FWD(param))) {
		return mse::impl::ra_section::make_xscope_string_const_section_helper1(
			typename mse::impl::is_instantiation_of<_TRALoneParam, mse::TXScopeCagedItemFixedConstPointerToRValue>::type(), MSE_FWD(param));
	}

	/* Overloads for rsv::TReturnableFParam<>. */
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_string_const_section)

	template <typename _TRAIterator>
	auto make_string_const_section(const _TRAIterator& start_iter, typename TStringConstSection<_TRAIterator>::size_type count) {
		return TStringConstSection<_TRAIterator>(start_iter, count);
	}
	template <typename _TRALoneParam>
	auto make_string_const_section(const _TRALoneParam& param) {
		typedef mse::impl::remove_reference_t<decltype(mse::impl::ra_const_section_helpers::s_iter_from_lone_param(param))> _TRAIterator;
		return TStringConstSection<_TRAIterator>(param);
	}

	template <typename _TRAIterator>
	auto make_xscope_string_section(const _TRAIterator& start_iter, typename TXScopeStringSection<_TRAIterator>::size_type count) {
		return TXScopeStringSection<_TRAIterator>(start_iter, count);
	}
	namespace impl {
		namespace ra_section {
			template <typename _Ty>
			auto make_xscope_string_section_helper1(std::true_type, const mse::TXScopeCagedItemFixedPointerToRValue<_Ty>& param) {
				return mse::make_xscope_string_const_section(param);
			}
			template <typename _TRALoneParam>
			auto make_xscope_string_section_helper1(std::false_type, const _TRALoneParam& param) {
				typedef mse::impl::remove_reference_t<decltype(mse::us::impl::TRandomAccessSectionBase<char *>::s_xscope_iter_from_lone_param(param))> _TRAIterator;
				return TXScopeStringSection<_TRAIterator>(param);
			}
		}
	}
	template <typename _TRALoneParam>
	auto make_xscope_string_section(const _TRALoneParam& param) {
		return mse::impl::ra_section::make_xscope_string_section_helper1(
			typename mse::impl::is_instantiation_of<_TRALoneParam, mse::TXScopeCagedItemFixedConstPointerToRValue>::type(), param);
	}

	/* Overloads for rsv::TReturnableFParam<>. */
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_string_section)

	/* This function basically just calls the give section's subsection() member function and returns the value.  */
	template<typename _Ty>
	auto string_subsection(const _Ty& ra_section, std::tuple<typename _Ty::size_type, typename _Ty::size_type> start_and_length = { 0U, _Ty::npos }) {
		return ra_section.subsection(std::get<0>(start_and_length), std::get<1>(start_and_length));
	}
	template<typename _Ty>
	auto xscope_string_subsection(const _Ty& ra_section, std::tuple<typename _Ty::size_type, typename _Ty::size_type> start_and_length = { 0U, _Ty::npos }) {
		return ra_section.xscope_subsection(std::get<0>(start_and_length), std::get<1>(start_and_length));
	}
	template<typename _Ty>
	auto xscope_string_subsection(const rsv::TReturnableFParam<_Ty>& ra_section, std::tuple<typename _Ty::size_type, typename _Ty::size_type> start_and_length = { 0U, _Ty::npos }) {
		const _Ty& ra_section_base_ref = ra_section;
		typedef decltype(xscope_string_subsection(ra_section_base_ref, start_and_length)) base_return_type;
		return rsv::TReturnableFParam<base_return_type>(xscope_string_subsection(ra_section_base_ref, start_and_length));
	}

	template <typename _TRAIterator>
	auto make_string_section(const _TRAIterator& start_iter, typename TStringSection<_TRAIterator>::size_type count) {
		return TStringSection<_TRAIterator>(start_iter, count);
	}
	template <typename _TRALoneParam>
	auto make_string_section(const _TRALoneParam& param) {
		typedef mse::impl::remove_reference_t<decltype(mse::us::impl::TRandomAccessSectionBase<char *>::s_iter_from_lone_param(param))> _TRAIterator;
		return TStringSection<_TRAIterator>(param);
	}

#ifdef MSE_HAS_CXX17
	/* deduction guides */
	template<class _TRAIterator>
	TStringSection(_TRAIterator, typename TStringSection<_TRAIterator>::size_type)
		->TStringSection<_TRAIterator>;
	template<class _TRALoneParam>
	TStringSection(_TRALoneParam)
		->TStringSection<typename decltype(make_string_section(std::declval<_TRALoneParam>()))::iterator_type>;

	template<class _TRAIterator>
	TStringConstSection(_TRAIterator, typename TStringConstSection<_TRAIterator>::size_type)
		->TStringConstSection<_TRAIterator>;
	template<class _TRALoneParam>
	TStringConstSection(_TRALoneParam)
		->TStringConstSection<typename decltype(make_string_const_section(std::declval<_TRALoneParam>()))::iterator_type>;

	template<class _TRAIterator>
	TXScopeStringSection(_TRAIterator, typename TXScopeStringSection<_TRAIterator>::size_type)
		->TXScopeStringSection<_TRAIterator>;
	template<class _TRALoneParam>
	TXScopeStringSection(_TRALoneParam)
		->TXScopeStringSection<typename decltype(make_xscope_string_section(std::declval<_TRALoneParam>()))::iterator_type>;

	template<class _TRAIterator>
	TXScopeStringConstSection(_TRAIterator, typename TXScopeStringConstSection<_TRAIterator>::size_type)
		->TXScopeStringConstSection<_TRAIterator>;
	template<class _TRALoneParam>
	TXScopeStringConstSection(_TRALoneParam)
		->TXScopeStringConstSection<typename decltype(make_xscope_string_const_section(std::declval<_TRALoneParam>()))::iterator_type>;
#endif /* MSE_HAS_CXX17 */

	template <typename _TElement, class _Traits = std::char_traits<_TElement> >
	using TXScopeCSSSXSTEStringConstSection = TXScopeStringConstSection<TXScopeCSSSXSTERAConstIterator<_TElement>, _Traits>;
	template <typename _TElement, class _Traits = std::char_traits<_TElement> >
	using TXScopeCSSSXSTEStringSection = TXScopeStringSection<TXScopeCSSSXSTERAIterator<_TElement>, _Traits>;

	template <typename _TRAIterator>
	auto make_xscope_csssxste_string_const_section(const _TRAIterator& start_iter, typename TXScopeStringConstSection<_TRAIterator>::size_type count) {
		typedef mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(*start_iter)> > _TElement;
		return TXScopeCSSSXSTEStringConstSection<_TElement>(TXScopeCSSSXSTERAConstIterator<_TElement>(start_iter), count);
	}
	template <typename _TRALoneParam>
	auto make_xscope_csssxste_string_const_section(const _TRALoneParam& param) {
		typedef mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(make_xscope_string_const_section(param)[0])> > _TElement;
		return TXScopeCSSSXSTEStringConstSection<_TElement>(make_xscope_string_const_section(param));
	}
	/* Overloads for rsv::TReturnableFParam<>. */
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_csssxste_string_const_section)

		template <typename _TRAIterator>
	auto make_xscope_csssxste_string_section(const _TRAIterator& start_iter, typename TXScopeStringSection<_TRAIterator>::size_type count) {
		typedef mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(*start_iter)> > _TElement;
		return TXScopeCSSSXSTEStringSection<_TElement>(TXScopeCSSSXSTERAIterator<_TElement>(start_iter), count);
	}
	template <typename _TRALoneParam>
	auto make_xscope_csssxste_string_section(const _TRALoneParam& param) {
		typedef mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(make_xscope_string_section(param)[0])> > _TElement;
		return TXScopeCSSSXSTEStringSection<_TElement>(make_xscope_string_section(param));
	}
	/* Overloads for rsv::TReturnableFParam<>. */
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_csssxste_string_section)


	/* "nrp string sections" are derived from "string sections". Rather than define their own associtaed
	TXScopeCagedNRPStringConstSectionToRValue<>, for now they just use the TXScopeCagedStringConstSectionToRValue<>
	associated with their base class "string sections". But this requires TXScopeCagedStringConstSectionToRValue<>
	to friend the "nrp string sections" make functions. But in order to be declared as friends, they need to be
	forward declared here (because some of them are in a different namespace from
	TXScopeCagedStringConstSectionToRValue<>). */

	template <typename _TRAIterator>
	using make_xscope_nrp_string_const_section_helper_type1 = decltype(make_xscope_string_const_section(std::declval<_TRAIterator>(), std::declval<typename TXScopeNRPStringConstSection<_TRAIterator>::size_type>()));
	template <typename _TRALoneParam>
	using make_xscope_nrp_string_const_section_helper_type2 = decltype(make_xscope_string_const_section(std::declval<_TRALoneParam>()));

	template <typename _TRAIterator>
	auto make_xscope_nrp_string_const_section(const _TRAIterator& start_iter, typename TXScopeNRPStringConstSection<_TRAIterator>::size_type count)
		//-> decltype(TXScopeNRPStringConstSection<typename decltype(make_xscope_string_const_section(start_iter, count))::iterator_type>(make_xscope_string_const_section(start_iter, count)));
		/* the previous line seems to be (sometimes?) too complex for msvc2017 */
		-> decltype(TXScopeNRPStringConstSection<typename make_xscope_nrp_string_const_section_helper_type1<_TRAIterator>::iterator_type>(make_xscope_string_const_section(start_iter, count)));

	template<typename _Ty, size_t Tn, typename = typename std::enable_if<1 <= Tn>::type>
	auto make_xscope_nrp_string_const_section(const _Ty(&presumed_string_literal)[Tn]) -> decltype(TXScopeNRPStringConstSection<const _Ty*>(presumed_string_literal));
	namespace impl {
		namespace ra_section {
			template <typename _Ty>
			auto make_xscope_nrp_string_const_section_helper1(std::true_type, const _Ty& param) -> decltype(make_xscope_string_const_section(param));
			template <typename _Ty>
			auto make_xscope_nrp_string_const_section_helper1(std::true_type, _Ty&& param) -> decltype(make_xscope_string_const_section(MSE_FWD(param)));
			template <typename _TRALoneParam>
			auto make_xscope_nrp_string_const_section_helper1(std::false_type, const _TRALoneParam& param)
				//-> decltype(TXScopeNRPStringConstSection<typename decltype(make_xscope_string_const_section(param))::iterator_type>(make_xscope_string_const_section(param)));
				-> decltype(TXScopeNRPStringConstSection<typename make_xscope_nrp_string_const_section_helper_type2<_TRALoneParam>::iterator_type>(make_xscope_string_const_section(param)));
		}
	}
	template <typename _TRALoneParam>
	auto make_xscope_nrp_string_const_section(const _TRALoneParam& param) -> decltype(mse::impl::ra_section::make_xscope_nrp_string_const_section_helper1(
		typename std::is_base_of<mse::us::impl::CagedStringSectionTagBase, decltype(make_xscope_string_const_section(std::declval<_TRALoneParam>()))>::type(), param));
	template <typename _TRALoneParam>
	auto make_xscope_nrp_string_const_section(_TRALoneParam&& param) -> decltype(mse::impl::ra_section::make_xscope_nrp_string_const_section_helper1(
		typename std::is_base_of<mse::us::impl::CagedStringSectionTagBase, decltype(make_xscope_string_const_section(std::declval<_TRALoneParam>()))>::type(), MSE_FWD(param)));

	/* TXScopeCagedStringConstSectionToRValue<> represents a "string const section" that refers to a temporary string object.
	The "string const section" is inaccessible ("caged") by default because it is, in general, unsafe. Its copyability and 
	movability are also restricted. The "string const section" can only be accessed by certain types and functions (declared
	as friends) that will ensure that it will be handled safely. */
	template<typename _TRAIterator, class _Traits>
	class TXScopeCagedStringConstSectionToRValue : public TXScopeCagedRandomAccessConstSectionToRValue<_TRAIterator>, public mse::us::impl::CagedStringSectionTagBase {
	public:
		void xscope_tag() const {}

	protected:
		typedef TXScopeCagedRandomAccessConstSectionToRValue<_TRAIterator> base_class;
		TXScopeCagedStringConstSectionToRValue(TXScopeCagedStringConstSectionToRValue&&) = default;
		TXScopeCagedStringConstSectionToRValue(const TXScopeCagedStringConstSectionToRValue&) = delete;
		TXScopeCagedStringConstSectionToRValue(const TXScopeStringConstSection<_TRAIterator, _Traits>& ptr) : base_class(ptr) {}

		auto uncaged_string_section() const { return TXScopeStringConstSection<_TRAIterator, _Traits>(base_class::uncaged_ra_section()); }

		TXScopeCagedStringConstSectionToRValue<_TRAIterator>& operator=(const TXScopeCagedStringConstSectionToRValue<_TRAIterator>& _Right_cref) = delete;
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		friend class rsv::TXScopeStringConstSectionFParam<_TRAIterator>;
		template <typename _Ty>
		friend auto impl::ra_section::make_xscope_string_const_section_helper1(std::true_type, const TXScopeCagedItemFixedConstPointerToRValue<_Ty>& param)
			->impl::ra_section::mkxsscsh1_ReturnType<_Ty>;
		template <typename _Ty>
		friend auto impl::ra_section::make_xscope_string_const_section_helper1(std::true_type, TXScopeCagedItemFixedConstPointerToRValue<_Ty>&& param)
			->impl::ra_section::mkxsscsh1_ReturnType<_Ty>;
		template <typename _TRALoneParam>
		friend auto make_xscope_string_const_section(const _TRALoneParam& param) -> decltype(mse::impl::ra_section::make_xscope_string_const_section_helper1(
			typename mse::impl::is_instantiation_of<_TRALoneParam, mse::TXScopeCagedItemFixedConstPointerToRValue>::type(), param));
		template <typename _TRALoneParam, class>
		friend auto make_xscope_string_const_section(_TRALoneParam&& param) -> decltype(mse::impl::ra_section::make_xscope_string_const_section_helper1(
			typename mse::impl::is_instantiation_of<_TRALoneParam, mse::TXScopeCagedItemFixedConstPointerToRValue>::type(), MSE_FWD(param)));

		/* For now, "nrp string sections" are also using this class rather than creating an "nrp" specific one. So we need to
		friend their associated elements too. */
		friend class rsv::TXScopeNRPStringConstSectionFParam<_TRAIterator>;
		template <typename _Ty>
		friend auto impl::ra_section::make_xscope_nrp_string_const_section_helper1(std::true_type, const _Ty& param) -> decltype(make_xscope_string_const_section(param));
		template <typename _Ty>
		friend auto impl::ra_section::make_xscope_nrp_string_const_section_helper1(std::true_type, _Ty&& param) -> decltype(make_xscope_string_const_section(MSE_FWD(param)));
		template <typename _TRALoneParam>
		friend auto make_xscope_nrp_string_const_section(const _TRALoneParam& param) -> decltype(mse::impl::ra_section::make_xscope_nrp_string_const_section_helper1(
			typename std::is_base_of<mse::us::impl::CagedStringSectionTagBase, decltype(make_xscope_string_const_section(std::declval<_TRALoneParam>()))>::type(), param));
		template <typename _TRALoneParam>
		friend auto make_xscope_nrp_string_const_section(_TRALoneParam&& param) -> decltype(mse::impl::ra_section::make_xscope_nrp_string_const_section_helper1(
			typename std::is_base_of<mse::us::impl::CagedStringSectionTagBase, decltype(make_xscope_string_const_section(std::declval<_TRALoneParam>()))>::type(), MSE_FWD(param)));
	};

	namespace rsv {

		template <typename _TRAIterator, class _Traits>
		class TXScopeStringConstSectionFParam : public TXScopeStringConstSection<_TRAIterator> {
		public:
			typedef TXScopeStringConstSection<_TRAIterator> base_class;
			typedef _TRAIterator iterator_type;
			MSE_INHERITED_STRING_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);

			//MSE_USING(TXScopeStringConstSectionFParam, base_class);
			TXScopeStringConstSectionFParam(const TXScopeStringConstSectionFParam& src) = default;
			TXScopeStringConstSectionFParam(const _TRAIterator& start_iter, size_type count) : base_class(start_iter, count) {}
			template <typename _TRALoneParam>
			TXScopeStringConstSectionFParam(const _TRALoneParam& param) : base_class(construction_helper1(typename
				std::conditional<mse::impl::is_instantiation_of<_TRALoneParam, mse::TXScopeCagedItemFixedConstPointerToRValue>::value
				// || mse::impl::is_instantiation_of<_TRALoneParam, mse::TXScopeCagedStringConstSectionToRValue>::value
				|| std::is_base_of<mse::us::impl::CagedStringSectionTagBase, _TRALoneParam>::value
				, std::true_type, std::false_type>::type(), param)) {
			}

			TXScopeStringConstSectionFParam xscope_subsection(size_type pos = 0, size_type n = npos) const {
				return base_class::xscope_subsection(pos, n);
			}
			typedef mse::impl::conditional_t<mse::impl::is_potentially_xscope<_TRAIterator>::value, TXScopeStringConstSectionFParam, TStringConstSection<_TRAIterator> > subsection_t;
			subsection_t subsection(size_type pos = 0, size_type n = npos) const {
				return base_class::subsection(pos, n);
			}

			//typedef typename base_class::xscope_iterator xscope_iterator;
			typedef typename base_class::xscope_const_iterator xscope_const_iterator;

			void xscope_not_returnable_tag() const {}
			void xscope_tag() const {}

		private:
			template <typename _TRAContainer>
			static mse::TXScopeFixedConstPointer<_TRAContainer> construction_helper1(std::true_type, const mse::TXScopeCagedItemFixedConstPointerToRValue<_TRAContainer>& caged_xscpptr) {
				return mse::rsv::TXScopeFixedConstPointerFParam<_TRAContainer>(caged_xscpptr);
			}
			static mse::TXScopeStringConstSection<_TRAIterator> construction_helper1(std::true_type, const mse::TXScopeCagedStringConstSectionToRValue<_TRAIterator>& caged_xscpsection) {
				return caged_xscpsection.uncaged_string_section();
			}
			template <typename _TRALoneParam>
			static auto construction_helper1(std::false_type, const _TRALoneParam& param) {
				return param;
			}

			MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
		};

		/* Template specializations of TFParam<>. */

		template<typename _Ty>
		class TFParam<mse::TXScopeStringConstSection<_Ty> > : public TXScopeStringConstSectionFParam<_Ty> {
		public:
			typedef TXScopeStringConstSectionFParam<_Ty> base_class;
			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TFParam, base_class);
			void xscope_not_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};

		template<typename _Ty>
		class TFParam<const mse::TXScopeStringConstSection<_Ty> > : public TXScopeStringConstSectionFParam<_Ty> {
		public:
			typedef TXScopeStringConstSectionFParam<_Ty> base_class;
			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TFParam, base_class);
			void xscope_not_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};

		template<typename _Ty>
		class TFParam<mse::TXScopeCagedStringConstSectionToRValue<_Ty> > : public TXScopeStringConstSectionFParam<_Ty> {
		public:
			typedef TXScopeStringConstSectionFParam<_Ty> base_class;
			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TFParam, base_class);
			void xscope_not_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};

		/* It's hard to think of a scenario where these native char array specializations would actually be necessary. But
		just in case. */
		template<size_t _Size>
		class TFParam<const char[_Size]> : public TXScopeStringConstSection<const char*> {
		public:
			typedef TXScopeStringConstSection<const char*> base_class;
			MSE_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TFParam);
			TFParam(const char(&presumed_string_literal)[_Size]) : base_class(presumed_string_literal) {}
		private:
			MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
		};

		template<size_t _Size>
		class TFParam<char[_Size]> : public TXScopeStringSection<char*> {
		public:
			typedef TXScopeStringSection<char*> base_class;
			MSE_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TFParam);
			TFParam(char(&param)[_Size]) : base_class(param) {}
		private:
			MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
		};

		/* Template specializations of TReturnableFParam<>. */

		template<typename _Ty>
		class TReturnableFParam<mse::TXScopeStringConstSection<_Ty> > : public TXScopeStringConstSection<_Ty> {
		public:
			typedef TXScopeStringConstSection<_Ty> base_class;
			MSE_INHERITED_STRING_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);
			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TReturnableFParam, base_class);

			/* Subsections of TReturnableFParam<mse::TXScopeStringConstSection<_Ty> > can inherit the "returnability"
			of the original section. */
			auto xscope_subsection(size_type pos = 0, size_type n = npos) const {
				typedef decltype(base_class::xscope_subsection(pos, n)) base_return_type;
				return TReturnableFParam<base_return_type>(base_class::xscope_subsection(pos, n));
			}
			auto xscope_substr(size_type pos = 0, size_type n = npos) const {
				return xscope_subsection(pos, n);
			}
			auto subsection(size_type pos = 0, size_type n = npos) const {
				typedef decltype(base_class::subsection(pos, n)) base_return_type;
				return TReturnableFParam<base_return_type>(base_class::subsection(pos, n));
			}
			auto substr(size_type pos = 0, size_type n = npos) const {
				return subsection(pos, n);
			}

			void xscope_not_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};

		template<typename _Ty>
		class TReturnableFParam<const mse::TXScopeStringConstSection<_Ty> > : public TXScopeStringConstSection<_Ty> {
		public:
			typedef TXScopeStringConstSection<_Ty> base_class;
			MSE_INHERITED_STRING_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);
			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TReturnableFParam, base_class);

			/* Subsections of TReturnableFParam<const mse::TXScopeStringConstSection<_Ty> > can inherit the "returnability"
			of the original section. */
			auto xscope_subsection(size_type pos = 0, size_type n = npos) const {
				typedef decltype(base_class::xscope_subsection(pos, n)) base_return_type;
				return TReturnableFParam<base_return_type>(base_class::xscope_subsection(pos, n));
			}
			auto xscope_substr(size_type pos = 0, size_type n = npos) const {
				return xscope_subsection(pos, n);
			}
			auto subsection(size_type pos = 0, size_type n = npos) const {
				typedef decltype(base_class::subsection(pos, n)) base_return_type;
				return TReturnableFParam<base_return_type>(base_class::subsection(pos, n));
			}
			auto substr(size_type pos = 0, size_type n = npos) const {
				return subsection(pos, n);
			}

			void returnable_once_tag() const {}
			void xscope_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};

		template<typename _Ty>
		class TReturnableFParam<mse::TXScopeStringSection<_Ty> > : public TXScopeStringSection<_Ty> {
		public:
			typedef TXScopeStringSection<_Ty> base_class;
			MSE_INHERITED_STRING_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);
			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TReturnableFParam, base_class);

			/* Subsections of TReturnableFParam<mse::TXScopeStringSection<_Ty> > can inherit the "returnability"
			of the original section. */
			auto xscope_subsection(size_type pos = 0, size_type n = npos) const {
				typedef decltype(base_class::xscope_subsection(pos, n)) base_return_type;
				return TReturnableFParam<base_return_type>(base_class::xscope_subsection(pos, n));
			}
			auto xscope_substr(size_type pos = 0, size_type n = npos) const {
				return xscope_subsection(pos, n);
			}
			auto subsection(size_type pos = 0, size_type n = npos) const {
				typedef decltype(base_class::subsection(pos, n)) base_return_type;
				return TReturnableFParam<base_return_type>(base_class::subsection(pos, n));
			}
			auto substr(size_type pos = 0, size_type n = npos) const {
				return subsection(pos, n);
			}

			void returnable_once_tag() const {}
			void xscope_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};

		template<typename _Ty>
		class TReturnableFParam<const mse::TXScopeStringSection<_Ty> > : public TXScopeStringSection<_Ty> {
		public:
			typedef TXScopeStringSection<_Ty> base_class;
			MSE_INHERITED_STRING_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);
			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TReturnableFParam, base_class);

			/* Subsections of TReturnableFParam<const mse::TXScopeStringSection<_Ty> > can inherit the "returnability"
			of the original section. */
			auto xscope_subsection(size_type pos = 0, size_type n = npos) const {
				typedef decltype(base_class::xscope_subsection(pos, n)) base_return_type;
				return TReturnableFParam<base_return_type>(base_class::xscope_subsection(pos, n));
			}
			auto xscope_substr(size_type pos = 0, size_type n = npos) const {
				return xscope_subsection(pos, n);
			}
			auto subsection(size_type pos = 0, size_type n = npos) const {
				typedef decltype(base_class::subsection(pos, n)) base_return_type;
				return TReturnableFParam<base_return_type>(base_class::subsection(pos, n));
			}
			auto substr(size_type pos = 0, size_type n = npos) const {
				return subsection(pos, n);
			}

			void returnable_once_tag() const {}
			void xscope_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};
	}

}

namespace std {

	template<class _TRAIterator, class _Traits>
	struct hash<mse::TXScopeStringSection<_TRAIterator, _Traits> > {	// hash functor for mse::TXScopeStringSection
		using argument_type = mse::TXScopeStringSection<_TRAIterator, _Traits>;
		using result_type = size_t;

		size_t operator()(const mse::TXScopeStringSection<_TRAIterator, _Traits>& _Keyval) const /*_NOEXCEPT*/ {
			/* todo: optimize */
			auto retval = mse::us::impl::T_Hash_bytes(_Keyval.xscope_cbegin(), _Keyval.length());
			return retval;
		}
	};

	template<class _TRAIterator, class _Traits>
	struct hash<mse::TStringSection<_TRAIterator, _Traits> > {	// hash functor for mse::TStringSection
		using argument_type = mse::TStringSection<_TRAIterator, _Traits>;
		using result_type = size_t;

		size_t operator()(const mse::TStringSection<_TRAIterator, _Traits>& _Keyval) const /*_NOEXCEPT*/ {
			/* todo: optimize */
			auto retval = mse::us::impl::T_Hash_bytes(_Keyval.cbegin(), _Keyval.length());
			return retval;
		}
	};

	template<class _TRAIterator, class _Traits>
	struct hash<mse::TXScopeStringConstSection<_TRAIterator, _Traits> > {	// hash functor for mse::TXScopeStringConstSection
		using argument_type = mse::TXScopeStringConstSection<_TRAIterator, _Traits>;
		using result_type = size_t;

		size_t operator()(const mse::TXScopeStringConstSection<_TRAIterator, _Traits>& _Keyval) const /*_NOEXCEPT*/ {
			/* todo: optimize */
			auto retval = mse::us::impl::T_Hash_bytes(_Keyval.xscope_cbegin(), _Keyval.length());
			return retval;
		}
	};

	template<class _TRAIterator, class _Traits>
	struct hash<mse::TStringConstSection<_TRAIterator, _Traits> > {	// hash functor for mse::TStringConstSection
		using argument_type = mse::TStringConstSection<_TRAIterator, _Traits>;
		using result_type = size_t;

		size_t operator()(const mse::TStringConstSection<_TRAIterator, _Traits>& _Keyval) const /*_NOEXCEPT*/ {
			/* todo: optimize */
			auto retval = mse::us::impl::T_Hash_bytes(_Keyval.cbegin(), _Keyval.length());
			return retval;
		}
	};
}

namespace mse {

	/* "NRP" string sections are just versions of the regular string sections that don't support raw pointers or
	std::basic_string_view<> iterators (for extra safety). */

	namespace impl {
		template<class _TRAIterator, MSE_IMPL_EIP mse::impl::enable_if_t<(!std::is_pointer<_TRAIterator>::value)> MSE_IMPL_EIS >
		void T_valid_if_not_a_native_pointer_msemsestring() {}

		template<class _TRAIterator>
		struct std_basic_string_from_iterator {
			typedef std::basic_string<mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(std::declval<_TRAIterator>()[0])> > > type;
			typedef typename type::iterator iterator;
			typedef typename type::const_iterator const_iterator;
		};
		/* The following template function will instantiate iff the given iterator is not an std::basic_string<>::iterator
		(or const_iterator). */
		template<class _TRAIterator>
		struct is_not_an_std_basic_string_iterator_msemsestring : std::integral_constant<bool,
			(!std::is_same<_TRAIterator, typename std_basic_string_from_iterator<_TRAIterator>::iterator>::value)
			&& (!std::is_same<_TRAIterator, typename std_basic_string_from_iterator<_TRAIterator>::const_iterator>::value)
		> {};
		/* The following template function will instantiate iff the given iterator is not an std::basic_string<>::iterator
		(or const_iterator). */
		template<class _TRAIterator, MSE_IMPL_EIP mse::impl::enable_if_t<is_not_an_std_basic_string_iterator_msemsestring<_TRAIterator>::value> MSE_IMPL_EIS >
		void T_valid_if_not_an_std_basic_string_iterator_msemsestring() {}

		template<class _TRAIterator>
		struct is_not_an_std_basic_string_xscope_iterator_msemsestring : std::integral_constant<bool,
			(!std::is_same<_TRAIterator, mse::impl::remove_reference_t<decltype(mse::make_xscope_begin_iterator(std::declval<mse::TXScopeFixedPointer<typename std_basic_string_from_iterator<_TRAIterator>::type> >()))> >::value)
			&& (!std::is_same<_TRAIterator, mse::impl::remove_reference_t<decltype(mse::make_xscope_begin_const_iterator(std::declval<mse::TXScopeFixedPointer<typename std_basic_string_from_iterator<_TRAIterator>::type> >()))> >::value)
			&& (!std::is_same<_TRAIterator, mse::impl::remove_reference_t<decltype(mse::make_xscope_begin_const_iterator(std::declval<mse::TXScopeFixedConstPointer<typename std_basic_string_from_iterator<_TRAIterator>::type> >()))> >::value)
			&& (!std::is_same<_TRAIterator, mse::impl::remove_reference_t<decltype(mse::make_xscope_begin_iterator(std::declval<mse::TXScopeObjFixedPointer<typename std_basic_string_from_iterator<_TRAIterator>::type> >()))> >::value)
			&& (!std::is_same<_TRAIterator, mse::impl::remove_reference_t<decltype(mse::make_xscope_begin_const_iterator(std::declval<mse::TXScopeObjFixedPointer<typename std_basic_string_from_iterator<_TRAIterator>::type> >()))> >::value)
			&& (!std::is_same<_TRAIterator, mse::impl::remove_reference_t<decltype(mse::make_xscope_begin_const_iterator(std::declval<mse::TXScopeObjFixedConstPointer<typename std_basic_string_from_iterator<_TRAIterator>::type> >()))> >::value)
		> {};
		/* The following template function will instantiate iff the given iterator is not an xscope iterator of an
		std::basic_string<>. */
		template<class _TRAIterator, MSE_IMPL_EIP mse::impl::enable_if_t<is_not_an_std_basic_string_xscope_iterator_msemsestring<_TRAIterator>::value> MSE_IMPL_EIS >
		void T_valid_if_not_an_std_basic_string_xscope_iterator_msemsestring() {}

		template<class _TRAIterator>
		struct std_basic_string_view_from_iterator {
#ifdef MSE_HAS_CXX17
			typedef std::basic_string_view<mse::impl::remove_const_t<mse::impl::remove_reference_t<decltype(std::declval<_TRAIterator>()[0])> > > type;
			typedef typename type::iterator iterator;
			typedef typename type::const_iterator const_iterator;
#else /* MSE_HAS_CXX17 */
			struct CDummy {};
			typedef CDummy* const type;
			typedef CDummy* const iterator;
			typedef const CDummy* const const_iterator;
#endif /* MSE_HAS_CXX17 */
		};
		template<class _TRAIterator>
		struct is_not_an_std_basic_string_view_iterator_msemsestring : std::integral_constant<bool,
				(!std::is_same<_TRAIterator, typename std_basic_string_view_from_iterator<_TRAIterator>::iterator>::value)
				&& (!std::is_same<_TRAIterator, typename std_basic_string_view_from_iterator<_TRAIterator>::const_iterator>::value)
			> {};
		/* The following template function will instantiate iff the given iterator is not an std::basic_string_view<>::iterator
		(or const_iterator). */
		template<class _TRAIterator, MSE_IMPL_EIP mse::impl::enable_if_t<is_not_an_std_basic_string_view_iterator_msemsestring<_TRAIterator>::value> MSE_IMPL_EIS >
		void T_valid_if_not_an_std_basic_string_view_iterator_msemsestring() {}

		template<class _TRAIterator>
		struct is_not_an_std_basic_string_view_xscope_iterator_msemsestring : std::integral_constant<bool, 
				(!std::is_same<_TRAIterator, mse::impl::remove_reference_t<decltype(mse::make_xscope_begin_iterator(std::declval<mse::TXScopeFixedPointer<typename std_basic_string_view_from_iterator<_TRAIterator>::type> >()))> >::value)
				&& (!std::is_same<_TRAIterator, mse::impl::remove_reference_t<decltype(mse::make_xscope_begin_const_iterator(std::declval<mse::TXScopeFixedPointer<typename std_basic_string_view_from_iterator<_TRAIterator>::type> >()))> >::value)
				&& (!std::is_same<_TRAIterator, mse::impl::remove_reference_t<decltype(mse::make_xscope_begin_const_iterator(std::declval<mse::TXScopeFixedConstPointer<typename std_basic_string_view_from_iterator<_TRAIterator>::type> >()))> >::value)
				//&& (!std::is_same<_TRAIterator, mse::impl::remove_reference_t<decltype(mse::make_xscope_begin_iterator(std::declval<mse::TXScopeObjFixedPointer<typename std_basic_string_view_from_iterator<_TRAIterator>::type> >()))> >::value)
				//&& (!std::is_same<_TRAIterator, mse::impl::remove_reference_t<decltype(mse::make_xscope_begin_const_iterator(std::declval<mse::TXScopeObjFixedPointer<typename std_basic_string_view_from_iterator<_TRAIterator>::type> >()))> >::value)
				//&& (!std::is_same<_TRAIterator, mse::impl::remove_reference_t<decltype(mse::make_xscope_begin_const_iterator(std::declval<mse::TXScopeObjFixedConstPointer<typename std_basic_string_view_from_iterator<_TRAIterator>::type> >()))> >::value)
			> {};
		/* The following template function will instantiate iff the given iterator is not an xscope iterator of an
		std::basic_string_view<>. */
		template<class _TRAIterator, MSE_IMPL_EIP mse::impl::enable_if_t<is_not_an_std_basic_string_view_xscope_iterator_msemsestring<_TRAIterator>::value> MSE_IMPL_EIS >
		void T_valid_if_not_an_std_basic_string_view_xscope_iterator_msemsestring() {}

		template<class _TRAIterator, MSE_IMPL_EIP mse::impl::enable_if_t<
			is_not_an_std_basic_string_view_iterator_msemsestring<_TRAIterator>::value
			&& is_not_an_std_basic_string_view_xscope_iterator_msemsestring<_TRAIterator>::value
#ifndef MSE_MSTDSTRING_DISABLED
			&& is_not_an_std_basic_string_iterator_msemsestring<_TRAIterator>::value
			&& is_not_an_std_basic_string_xscope_iterator_msemsestring<_TRAIterator>::value
#endif /*!MSE_MSTDSTRING_DISABLED*/
		> MSE_IMPL_EIS >
		void T_valid_if_not_an_nrp_unsupported_iterator_msemsestring() {}
	}

	template <typename _TRAIterator, class _Traits>
	class TXScopeNRPStringSection : public TXScopeStringSection<_TRAIterator, _Traits> {
	public:
		typedef TXScopeStringSection<_TRAIterator, _Traits> base_class;
		MSE_INHERITED_STRING_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);

		//MSE_USING(TXScopeNRPStringSection, base_class);
		TXScopeNRPStringSection(const TXScopeNRPStringSection& src) : base_class(static_cast<const base_class&>(src)) {}
		TXScopeNRPStringSection(const base_class& src) : base_class(src) {}
		template<class _Ty2 = _TRAIterator, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _TRAIterator>::value) && (mse::impl::is_potentially_not_xscope<_TRAIterator>::value)> MSE_IMPL_EIS >
		TXScopeNRPStringSection(const TNRPStringSection<_TRAIterator, _Traits>& src) : base_class(static_cast<const base_class&>(src)) {}
		TXScopeNRPStringSection(const _TRAIterator& start_iter, size_type count) : base_class(start_iter, count) {}
		template <typename _TRALoneParam>
		TXScopeNRPStringSection(const _TRALoneParam& param) : base_class(param) {}

		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TXScopeNRPStringSection() {
			/* Note: Use TXScopeNRPStringConstSection instead if referencing a string literal. */
			valid_if_TRAIterator_is_not_a_native_pointer();
			mse::impl::T_valid_if_not_an_nrp_unsupported_iterator_msemsestring<_TRAIterator>();
		}

		TXScopeNRPStringSection xscope_subsection(size_type pos = 0, size_type n = npos) const {
			return base_class::subsection(pos, n);
		}
		TXScopeNRPStringSection xscope_substr(size_type pos = 0, size_type n = npos) const {
			return xscope_subsection(pos, n);
		}
		TNRPStringSection<_TRAIterator, _Traits> subsection(size_type pos = 0, size_type n = npos) const {
			return base_class::subsection(pos, n);
		}
		TNRPStringSection<_TRAIterator, _Traits> substr(size_type pos = 0, size_type n = npos) const {
			return subsection(pos, n);
		}

		typedef typename base_class::xscope_iterator xscope_iterator;
		typedef typename base_class::xscope_const_iterator xscope_const_iterator;

	private:
#if !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)
		template<class _Ty2 = _TRAIterator, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _TRAIterator>::value) && (!std::is_pointer<_Ty2>::value)> MSE_IMPL_EIS >
#endif /*!defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)*/
		void valid_if_TRAIterator_is_not_a_native_pointer() const {}

		//TXScopeNRPStringSection<_TRAIterator, _Traits>& operator=(const TXScopeNRPStringSection<_TRAIterator, _Traits>& _Right_cref) = delete;
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		friend std::basic_ostream<nonconst_value_type, std::char_traits<nonconst_value_type> >& operator<<(std::basic_ostream<nonconst_value_type, std::char_traits<nonconst_value_type> >& _Ostr, const TXScopeNRPStringSection& _Str) {
			return _Ostr << static_cast<const base_class&>(_Str);
		}
	};

	template <typename _TRAIterator>
	auto make_xscope_nrp_string_section(const _TRAIterator& start_iter, typename TXScopeNRPStringSection<_TRAIterator>::size_type count) {
		auto xscope_string_section = make_xscope_string_section(start_iter, count);
		return TXScopeNRPStringSection<typename decltype(xscope_string_section)::iterator_type>(xscope_string_section);
	}
	template <typename _TRALoneParam>
	auto make_xscope_nrp_string_section(const _TRALoneParam& param) {
		auto xscope_string_section = make_xscope_string_section(param);
		return TXScopeNRPStringSection<typename decltype(xscope_string_section)::iterator_type>(xscope_string_section);
	}

	/* Overloads for rsv::TReturnableFParam<>. */
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_nrp_string_section)

	template <typename _TRAIterator, class _Traits>
	class TNRPStringSection : public TStringSection<_TRAIterator, _Traits> {
	public:
		typedef TStringSection<_TRAIterator, _Traits> base_class;
		MSE_INHERITED_STRING_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);

		TNRPStringSection(const TNRPStringSection& src) : base_class(static_cast<const base_class&>(src)) {}
		TNRPStringSection(const base_class& src) : base_class(src) {}
		TNRPStringSection(const _TRAIterator& start_iter, size_type count) : base_class(start_iter, count) {}
		template <typename _TRALoneParam>
		TNRPStringSection(const _TRALoneParam& param) : base_class(param) {}
		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TNRPStringSection() {
			mse::impl::T_valid_if_not_an_xscope_type<_TRAIterator>();
			/* Note: Use TNRPStringConstSection instead if referencing a string literal. */
			valid_if_TRAIterator_is_not_a_native_pointer();
			mse::impl::T_valid_if_not_an_nrp_unsupported_iterator_msemsestring<_TRAIterator>();
		}

		TNRPStringSection subsection(size_type pos = 0, size_type n = npos) const {
			return base_class::subsection(pos, n);
		}
		TNRPStringSection substr(size_type pos = 0, size_type n = npos) const {
			return subsection(pos, n);
		}

		typedef typename base_class::iterator iterator;
		typedef typename base_class::const_iterator const_iterator;
		typedef typename base_class::reverse_iterator reverse_iterator;
		typedef typename base_class::const_reverse_iterator const_reverse_iterator;

	private:
		template<class _Ty2 = _TRAIterator, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _TRAIterator>::value) && (!std::is_pointer<_Ty2>::value)> MSE_IMPL_EIS >
		void valid_if_TRAIterator_is_not_a_native_pointer() const {}

		friend class TXScopeNRPStringSection<_TRAIterator, _Traits>;
		friend class TXScopeNRPStringConstSection<_TRAIterator, _Traits>;
		friend class TNRPStringConstSection<_TRAIterator, _Traits>;
		friend std::basic_ostream<nonconst_value_type, std::char_traits<nonconst_value_type> >& operator<<(std::basic_ostream<nonconst_value_type, std::char_traits<nonconst_value_type> >& _Ostr, const TNRPStringSection& _Str) {
			return _Ostr << static_cast<const base_class&>(_Str);
		}
	};

	template <typename _TRAIterator>
	auto make_nrp_string_section(const _TRAIterator& start_iter, typename TNRPStringSection<_TRAIterator>::size_type count) {
		auto string_section = make_string_section(start_iter, count);
		return TNRPStringSection<typename decltype(string_section)::iterator_type>(string_section);
	}
	template <typename _TRALoneParam>
	auto make_nrp_string_section(const _TRALoneParam& param) {
		auto string_section = make_string_section(param);
		return TNRPStringSection<typename decltype(string_section)::iterator_type>(string_section);
	}

	template <typename _TRAIterator, class _Traits>
	class TXScopeNRPStringConstSection : public TXScopeStringConstSection<_TRAIterator, _Traits> {
	public:
		typedef TXScopeStringConstSection<_TRAIterator, _Traits> base_class;
		MSE_INHERITED_STRING_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);

		//MSE_USING(TXScopeNRPStringConstSection, base_class);

		TXScopeNRPStringConstSection(const TXScopeNRPStringConstSection& src) : base_class(static_cast<const base_class&>(src)) {}
		TXScopeNRPStringConstSection(const base_class& src) : base_class(src) { valid_if_TRAIterator_is_not_a_native_pointer(); }
		template<class _Ty2 = _TRAIterator, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _TRAIterator>::value) && (mse::impl::is_potentially_not_xscope<_TRAIterator>::value)> MSE_IMPL_EIS >
		TXScopeNRPStringConstSection(const TNRPStringConstSection<_TRAIterator, _Traits>& src) : base_class(static_cast<const base_class&>(src)) {}
		template<class _Ty2 = _TRAIterator, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _TRAIterator>::value) && (!std::is_pointer<_Ty2>::value)> MSE_IMPL_EIS >
		TXScopeNRPStringConstSection(const TXScopeNRPStringSection<_TRAIterator, _Traits>& src) : base_class(static_cast<const typename TXScopeNRPStringSection<_TRAIterator, _Traits>::base_class&>(src)) {}
		template<class _Ty2 = _TRAIterator, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _TRAIterator>::value) && (!std::is_pointer<_Ty2>::value)
			&& (mse::impl::is_potentially_not_xscope<_TRAIterator>::value)> MSE_IMPL_EIS >
		TXScopeNRPStringConstSection(const TNRPStringSection<_TRAIterator, _Traits>& src) : base_class(static_cast<const typename TNRPStringSection<_TRAIterator, _Traits>::base_class&>(src)) {}
		template<class _Ty2 = _TRAIterator, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _TRAIterator>::value) && (!std::is_pointer<_Ty2>::value)> MSE_IMPL_EIS >
		TXScopeNRPStringConstSection(const _TRAIterator& start_iter, size_type count) : base_class(start_iter, count) {
			valid_if_TRAIterator_is_not_a_native_pointer();
		}
		template <typename _TRALoneParam>
		TXScopeNRPStringConstSection(const _TRALoneParam& param) : base_class(param) {
			valid_if_TRAIterator_is_not_a_native_pointer();
		}

		template<size_t Tn, typename = typename std::enable_if<1 <= Tn>::type>
		explicit TXScopeNRPStringConstSection(const value_type(&presumed_string_literal)[Tn]) : base_class(presumed_string_literal, Tn) {
			if ((1 <= (*this).size()) && (0 == (*this).back())) {
				/* We presume that the argument is a string literal, and remove the terminating null. */
				(*this).remove_suffix(1);
			}
			else {
				/* The last character does not seem to a null terminator, so presumably the argument is not a string
				literal. */
				MSE_THROW(std::invalid_argument("attempt to construct from a 'non-string literal' native array - mse::TXScopeNRPStringConstSection<>"));
			}
		}

		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TXScopeNRPStringConstSection() {
			mse::impl::T_valid_if_not_an_nrp_unsupported_iterator_msemsestring<_TRAIterator>();
		}

		TXScopeNRPStringConstSection xscope_subsection(size_type pos = 0, size_type n = npos) const {
			return base_class::subsection(pos, n);
		}
		TXScopeNRPStringConstSection xscope_substr(size_type pos = 0, size_type n = npos) const {
			return xscope_subsection(pos, n);
		}
		TNRPStringConstSection<_TRAIterator, _Traits> subsection(size_type pos = 0, size_type n = npos) const {
			return base_class::subsection(pos, n);
		}
		TNRPStringConstSection<_TRAIterator, _Traits> substr(size_type pos = 0, size_type n = npos) const {
			return subsection(pos, n);
		}

		//typedef typename base_class::xscope_iterator xscope_iterator;
		typedef typename base_class::xscope_const_iterator xscope_const_iterator;

	private:
		/* Construction from a const native array is publicly supported (only) because string literals are const
		native arrays. We do not want construction from a non-const native array to be publicly supported. */
		template<size_t Tn>
		explicit TXScopeNRPStringConstSection(mse::impl::remove_const_t<value_type>(&native_array)[Tn]) : base_class(native_array, Tn) {}

#if !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)
		template<class _Ty2 = _TRAIterator, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _TRAIterator>::value) && (!std::is_pointer<_Ty2>::value)> MSE_IMPL_EIS >
#endif /*!defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)*/
		void valid_if_TRAIterator_is_not_a_native_pointer() const {}

		//TXScopeNRPStringConstSection<_TRAIterator, _Traits>& operator=(const TXScopeNRPStringConstSection<_TRAIterator, _Traits>& _Right_cref) = delete;
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		friend std::basic_ostream<nonconst_value_type, std::char_traits<nonconst_value_type> >& operator<<(std::basic_ostream<nonconst_value_type, std::char_traits<nonconst_value_type> >& _Ostr, const TXScopeNRPStringConstSection& _Str) {
			return _Ostr << static_cast<const base_class&>(_Str);
		}
	};

	template <typename _TRAIterator>
	auto make_xscope_nrp_string_const_section(const _TRAIterator& start_iter, typename TXScopeNRPStringConstSection<_TRAIterator>::size_type count)
		//-> decltype(TXScopeNRPStringConstSection<typename decltype(make_xscope_string_const_section(start_iter, count))::iterator_type>(make_xscope_string_const_section(start_iter, count))) {
		-> decltype(TXScopeNRPStringConstSection<typename make_xscope_nrp_string_const_section_helper_type1<_TRAIterator>::iterator_type>(make_xscope_string_const_section(start_iter, count))) {
		return TXScopeNRPStringConstSection<typename decltype(make_xscope_string_const_section(start_iter, count))::iterator_type>(make_xscope_string_const_section(start_iter, count));
	}
	template<typename _Ty, size_t Tn, typename/* = typename std::enable_if<1 <= Tn>::type*/>
	auto make_xscope_nrp_string_const_section(const _Ty(&presumed_string_literal)[Tn]) -> decltype(TXScopeNRPStringConstSection<const _Ty*>(presumed_string_literal)) {
		return TXScopeNRPStringConstSection<const _Ty*>(presumed_string_literal);
	}
	namespace impl {
		namespace ra_section {
			template <typename _Ty>
			auto make_xscope_nrp_string_const_section_helper1(std::true_type, const _Ty& param) -> decltype(make_xscope_string_const_section(param)) {
				return make_xscope_string_const_section(param);
			}
			template <typename _Ty>
			auto make_xscope_nrp_string_const_section_helper1(std::true_type, _Ty&& param) -> decltype(make_xscope_string_const_section(MSE_FWD(param))) {
				return make_xscope_string_const_section(MSE_FWD(param));
			}
			template <typename _TRALoneParam>
			auto make_xscope_nrp_string_const_section_helper1(std::false_type, const _TRALoneParam& param)
				//-> decltype(TXScopeNRPStringConstSection<typename decltype(make_xscope_string_const_section(param))::iterator_type>(make_xscope_string_const_section(param))) {
				-> decltype(TXScopeNRPStringConstSection<typename make_xscope_nrp_string_const_section_helper_type2<_TRALoneParam>::iterator_type>(make_xscope_string_const_section(param))) {

				return TXScopeNRPStringConstSection<typename decltype(make_xscope_string_const_section(param))::iterator_type>(make_xscope_string_const_section(param));
			}
		}
	}
	template <typename _TRALoneParam>
	auto make_xscope_nrp_string_const_section(const _TRALoneParam& param) -> decltype(mse::impl::ra_section::make_xscope_nrp_string_const_section_helper1(
		typename std::is_base_of<mse::us::impl::CagedStringSectionTagBase, decltype(make_xscope_string_const_section(std::declval<_TRALoneParam>()))>::type(), param)) {

		return mse::impl::ra_section::make_xscope_nrp_string_const_section_helper1(typename std::is_base_of<mse::us::impl::CagedStringSectionTagBase,
			decltype(make_xscope_string_const_section(std::declval<_TRALoneParam>()))>::type()
			, param);
	}
	template <typename _TRALoneParam>
	auto make_xscope_nrp_string_const_section(_TRALoneParam&& param) -> decltype(mse::impl::ra_section::make_xscope_nrp_string_const_section_helper1(
		typename std::is_base_of<mse::us::impl::CagedStringSectionTagBase, decltype(make_xscope_string_const_section(std::declval<_TRALoneParam>()))>::type(), MSE_FWD(param))) {

		return mse::impl::ra_section::make_xscope_nrp_string_const_section_helper1(typename std::is_base_of<mse::us::impl::CagedStringSectionTagBase,
			decltype(make_xscope_string_const_section(std::declval<_TRALoneParam>()))>::type()
			, MSE_FWD(param));
	}

	/* Overloads for rsv::TReturnableFParam<>. */
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_nrp_string_const_section)

	template <typename _TRAIterator, class _Traits>
	class TNRPStringConstSection : public TStringConstSection<_TRAIterator, _Traits> {
	public:
		typedef TStringConstSection<_TRAIterator, _Traits> base_class;
		MSE_INHERITED_STRING_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);

		TNRPStringConstSection(const TNRPStringConstSection& src) : base_class(static_cast<const base_class&>(src)) {}
		TNRPStringConstSection(const base_class& src) : base_class(src) { valid_if_TRAIterator_is_not_a_native_pointer(); }
		template<class _Ty2 = _TRAIterator, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _TRAIterator>::value) && (!std::is_pointer<_Ty2>::value)> MSE_IMPL_EIS >
		TNRPStringConstSection(const TNRPStringSection<_TRAIterator, _Traits>& src) : base_class(static_cast<const typename TNRPStringSection<_TRAIterator, _Traits>::base_class&>(src)) {}
		template<class _Ty2 = _TRAIterator, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _TRAIterator>::value) && (!std::is_pointer<_Ty2>::value)> MSE_IMPL_EIS >
		TNRPStringConstSection(const _TRAIterator& start_iter, size_type count) : base_class(start_iter, count) {
			valid_if_TRAIterator_is_not_a_native_pointer();
		}
		template <typename _TRALoneParam>
		TNRPStringConstSection(const _TRALoneParam& param) : base_class(param) { valid_if_TRAIterator_is_not_a_native_pointer(); }

		template<size_t Tn, typename = typename std::enable_if<1 <= Tn>::type>
		explicit TNRPStringConstSection(const value_type(&presumed_string_literal)[Tn]) : base_class(presumed_string_literal, Tn) {
			if ((1 <= (*this).size()) && (0 == (*this).back())) {
				/* We presume that the argument is a string literal, and remove the terminating null. */
				(*this).remove_suffix(1);
			}
			else {
				/* The last character does not seem to a null terminator, so presumably the argument is not a string
				literal. */
				MSE_THROW(std::invalid_argument("attempt to construct from a 'non-string literal' native array - mse::TNRPStringConstSection<>"));
			}
		}

		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TNRPStringConstSection() {
			mse::impl::T_valid_if_not_an_xscope_type<_TRAIterator>();
			mse::impl::T_valid_if_not_an_nrp_unsupported_iterator_msemsestring<_TRAIterator>();
		}

		TNRPStringConstSection subsection(size_type pos = 0, size_type n = npos) const {
			return base_class::subsection(pos, n);
		}
		TNRPStringConstSection substr(size_type pos = 0, size_type n = npos) const {
			return subsection(pos, n);
		}

		//typedef typename base_class::iterator iterator;
		typedef typename base_class::const_iterator const_iterator;
		//typedef typename base_class::reverse_iterator reverse_iterator;
		typedef typename base_class::const_reverse_iterator const_reverse_iterator;

	private:
		/* Construction from a const native array is publicly supported (only) because string literals are const
		native arrays. We do not want construction from a non-const native array to be publicly supported. */
		template<size_t Tn>
		explicit TNRPStringConstSection(mse::impl::remove_const_t<value_type>(&native_array)[Tn]) : base_class(native_array, Tn) {}

		template<class _Ty2 = _TRAIterator, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _TRAIterator>::value) && (!std::is_pointer<_Ty2>::value)> MSE_IMPL_EIS >
		void valid_if_TRAIterator_is_not_a_native_pointer() const {}

		friend class TXScopeNRPStringConstSection<_TRAIterator, _Traits>;
		friend std::basic_ostream<nonconst_value_type, std::char_traits<nonconst_value_type> >& operator<<(std::basic_ostream<nonconst_value_type, std::char_traits<nonconst_value_type> >& _Ostr, const TNRPStringConstSection& _Str) {
			return _Ostr << static_cast<const base_class&>(_Str);
		}
	};

	template <typename _TRAIterator>
	auto make_nrp_string_const_section(const _TRAIterator& start_iter, typename TNRPStringConstSection<_TRAIterator>::size_type count) {
		auto string_const_section = make_string_const_section(start_iter, count);
		return TNRPStringConstSection<typename decltype(string_const_section)::iterator_type>(string_const_section);
	}
	template<typename _Ty, size_t Tn, typename = typename std::enable_if<1 <= Tn>::type>
	auto make_nrp_string_const_section(const _Ty(&presumed_string_literal)[Tn]) {
		return TNRPStringConstSection<const _Ty*>(presumed_string_literal);
	}
	template <typename _TRALoneParam>
	auto make_nrp_string_const_section(const _TRALoneParam& param) {
		auto string_const_section = make_string_const_section(param);
		return TNRPStringConstSection<typename decltype(string_const_section)::iterator_type>(string_const_section);
	}

#ifdef MSE_HAS_CXX17
	/* deduction guides */
	template<class _TRAIterator>
	TNRPStringSection(_TRAIterator, typename TNRPStringSection<_TRAIterator>::size_type)
		->TNRPStringSection<_TRAIterator>;
	template<class _TRALoneParam>
	TNRPStringSection(_TRALoneParam)
		->TNRPStringSection<typename decltype(make_nrp_string_section(std::declval<_TRALoneParam>()))::iterator_type>;

	template<class _TRAIterator>
	TNRPStringConstSection(_TRAIterator, typename TNRPStringConstSection<_TRAIterator>::size_type)
		->TNRPStringConstSection<_TRAIterator>;
	template<class _TRALoneParam>
	TNRPStringConstSection(_TRALoneParam)
		->TNRPStringConstSection<typename decltype(make_nrp_string_const_section(std::declval<_TRALoneParam>()))::iterator_type>;

	template<class _TRAIterator>
	TXScopeNRPStringSection(_TRAIterator, typename TXScopeNRPStringSection<_TRAIterator>::size_type)
		->TXScopeNRPStringSection<_TRAIterator>;
	template<class _TRALoneParam>
	TXScopeNRPStringSection(_TRALoneParam)
		->TXScopeNRPStringSection<typename decltype(make_xscope_nrp_string_section(std::declval<_TRALoneParam>()))::iterator_type>;

	template<class _TRAIterator>
	TXScopeNRPStringConstSection(_TRAIterator, typename TXScopeNRPStringConstSection<_TRAIterator>::size_type)
		->TXScopeNRPStringConstSection<_TRAIterator>;
	template<class _TRALoneParam>
	TXScopeNRPStringConstSection(_TRALoneParam)
		->TXScopeNRPStringConstSection<typename decltype(make_xscope_nrp_string_const_section(std::declval<_TRALoneParam>()))::iterator_type>;
#endif /* MSE_HAS_CXX17 */

	namespace rsv {

		template <typename _TRAIterator, class _Traits>
		class TXScopeNRPStringConstSectionFParam : public TXScopeNRPStringConstSection<_TRAIterator> {
		public:
			typedef TXScopeNRPStringConstSection<_TRAIterator> base_class;
			typedef _TRAIterator iterator_type;
			MSE_INHERITED_STRING_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);

			//MSE_USING(TXScopeNRPStringConstSectionFParam, base_class);
			TXScopeNRPStringConstSectionFParam(const TXScopeNRPStringConstSectionFParam& src) = default;
			TXScopeNRPStringConstSectionFParam(const _TRAIterator& start_iter, size_type count) : base_class(start_iter, count) {}
			template <typename _TRALoneParam>
			TXScopeNRPStringConstSectionFParam(const _TRALoneParam& param) : base_class(construction_helper1(typename
				std::conditional<mse::impl::is_instantiation_of<_TRALoneParam, mse::TXScopeCagedItemFixedConstPointerToRValue>::value
				// || mse::impl::is_instantiation_of<_TRALoneParam, mse::TXScopeCagedStringConstSectionToRValue>::value
				|| std::is_base_of<mse::us::impl::CagedStringSectionTagBase, _TRALoneParam>::value
				, std::true_type, std::false_type>::type(), param)) {
			}

			TXScopeNRPStringConstSectionFParam xscope_subsection(size_type pos = 0, size_type n = npos) const {
				return base_class::xscope_subsection(pos, n);
			}
			typedef mse::impl::conditional_t<mse::impl::is_potentially_xscope<_TRAIterator>::value, TXScopeNRPStringConstSectionFParam, TNRPStringConstSection<_TRAIterator> > subsection_t;
			subsection_t subsection(size_type pos = 0, size_type n = npos) const {
				return base_class::subsection(pos, n);
			}

			//typedef typename base_class::xscope_iterator xscope_iterator;
			typedef typename base_class::xscope_const_iterator xscope_const_iterator;

			void xscope_not_returnable_tag() const {}
			void xscope_tag() const {}

		private:
			template <typename _TRAContainer>
			static mse::TXScopeFixedConstPointer<_TRAContainer> construction_helper1(std::true_type, const mse::TXScopeCagedItemFixedConstPointerToRValue<_TRAContainer>& caged_xscpptr) {
				return mse::rsv::TXScopeFixedConstPointerFParam<_TRAContainer>(caged_xscpptr);
			}
			static mse::TXScopeNRPStringConstSection<_TRAIterator> construction_helper1(std::true_type, const mse::TXScopeCagedStringConstSectionToRValue<_TRAIterator>& caged_xscpsection) {
				return caged_xscpsection.uncaged_string_section();
			}
			template <typename _TRALoneParam>
			static auto construction_helper1(std::false_type, const _TRALoneParam& param) {
				return param;
			}

			MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
		};

		/* Template specializations of TFParam<>. */

		template<typename _Ty>
		class TFParam<mse::TXScopeNRPStringConstSection<_Ty> > : public TXScopeNRPStringConstSectionFParam<_Ty> {
		public:
			typedef TXScopeNRPStringConstSectionFParam<_Ty> base_class;
			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TFParam, base_class);
			void xscope_not_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};

		template<typename _Ty>
		class TFParam<const mse::TXScopeNRPStringConstSection<_Ty> > : public TXScopeNRPStringConstSectionFParam<_Ty> {
		public:
			typedef TXScopeNRPStringConstSectionFParam<_Ty> base_class;
			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TFParam, base_class);
			void xscope_not_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};

#if 0
		template<typename _Ty>
		class TFParam<mse::TXScopeCagedNRPStringConstSectionToRValue<_Ty> > : public TXScopeNRPStringConstSectionFParam<_Ty> {
		public:
			typedef TXScopeNRPStringConstSectionFParam<_Ty> base_class;
			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TFParam, base_class);
			void xscope_not_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};
#endif //0

		/* Template specializations of TReturnableFParam<>. */

		template<typename _Ty>
		class TReturnableFParam<mse::TXScopeNRPStringConstSection<_Ty> > : public TXScopeNRPStringConstSection<_Ty> {
		public:
			typedef TXScopeNRPStringConstSection<_Ty> base_class;
			MSE_INHERITED_STRING_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);
			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TReturnableFParam, base_class);

			/* Subsections of TReturnableFParam<mse::TXScopeNRPStringConstSection<_Ty> > can inherit the "returnability"
			of the original section. */
			auto xscope_subsection(size_type pos = 0, size_type n = npos) const {
				typedef decltype(base_class::xscope_subsection(pos, n)) base_return_type;
				return TReturnableFParam<base_return_type>(base_class::xscope_subsection(pos, n));
			}
			auto xscope_substr(size_type pos = 0, size_type n = npos) const {
				return xscope_subsection(pos, n);
			}
			auto subsection(size_type pos = 0, size_type n = npos) const {
				typedef decltype(base_class::subsection(pos, n)) base_return_type;
				return TReturnableFParam<base_return_type>(base_class::subsection(pos, n));
			}
			auto substr(size_type pos = 0, size_type n = npos) const {
				return subsection(pos, n);
			}

			void returnable_once_tag() const {}
			void xscope_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};

		template<typename _Ty>
		class TReturnableFParam<const mse::TXScopeNRPStringConstSection<_Ty> > : public TXScopeNRPStringConstSection<_Ty> {
		public:
			typedef TXScopeNRPStringConstSection<_Ty> base_class;
			MSE_INHERITED_STRING_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);
			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TReturnableFParam, base_class);

			/* Subsections of TReturnableFParam<const mse::TXScopeNRPStringConstSection<_Ty> > can inherit the "returnability"
			of the original section. */
			auto xscope_subsection(size_type pos = 0, size_type n = npos) const {
				typedef decltype(base_class::xscope_subsection(pos, n)) base_return_type;
				return TReturnableFParam<base_return_type>(base_class::xscope_subsection(pos, n));
			}
			auto xscope_substr(size_type pos = 0, size_type n = npos) const {
				return xscope_subsection(pos, n);
			}
			auto subsection(size_type pos = 0, size_type n = npos) const {
				typedef decltype(base_class::subsection(pos, n)) base_return_type;
				return TReturnableFParam<base_return_type>(base_class::subsection(pos, n));
			}
			auto substr(size_type pos = 0, size_type n = npos) const {
				return subsection(pos, n);
			}

			void returnable_once_tag() const {}
			void xscope_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};

		template<typename _Ty>
		class TReturnableFParam<mse::TXScopeNRPStringSection<_Ty> > : public TXScopeNRPStringSection<_Ty> {
		public:
			typedef TXScopeNRPStringSection<_Ty> base_class;
			MSE_INHERITED_STRING_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);
			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TReturnableFParam, base_class);

			/* Subsections of TReturnableFParam<mse::TXScopeNRPStringSection<_Ty> > can inherit the "returnability"
			of the original section. */
			auto xscope_subsection(size_type pos = 0, size_type n = npos) const {
				typedef decltype(base_class::xscope_subsection(pos, n)) base_return_type;
				return TReturnableFParam<base_return_type>(base_class::xscope_subsection(pos, n));
			}
			auto xscope_substr(size_type pos = 0, size_type n = npos) const {
				return xscope_subsection(pos, n);
			}
			auto subsection(size_type pos = 0, size_type n = npos) const {
				typedef decltype(base_class::subsection(pos, n)) base_return_type;
				return TReturnableFParam<base_return_type>(base_class::subsection(pos, n));
			}
			auto substr(size_type pos = 0, size_type n = npos) const {
				return subsection(pos, n);
			}

			void returnable_once_tag() const {}
			void xscope_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};

		template<typename _Ty>
		class TReturnableFParam<const mse::TXScopeNRPStringSection<_Ty> > : public TXScopeNRPStringSection<_Ty> {
		public:
			typedef TXScopeNRPStringSection<_Ty> base_class;
			MSE_INHERITED_STRING_SECTION_MEMBER_TYPE_AND_NPOS_DECLARATIONS(base_class);
			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TReturnableFParam, base_class);

			/* Subsections of TReturnableFParam<const mse::TXScopeNRPStringSection<_Ty> > can inherit the "returnability"
			of the original section. */
			auto xscope_subsection(size_type pos = 0, size_type n = npos) const {
				typedef decltype(base_class::xscope_subsection(pos, n)) base_return_type;
				return TReturnableFParam<base_return_type>(base_class::xscope_subsection(pos, n));
			}
			auto xscope_substr(size_type pos = 0, size_type n = npos) const {
				return xscope_subsection(pos, n);
			}
			auto subsection(size_type pos = 0, size_type n = npos) const {
				typedef decltype(base_class::subsection(pos, n)) base_return_type;
				return TReturnableFParam<base_return_type>(base_class::subsection(pos, n));
			}
			auto substr(size_type pos = 0, size_type n = npos) const {
				return subsection(pos, n);
			}

			void returnable_once_tag() const {}
			void xscope_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};
	}

	template <typename _TElement, class _Traits = std::char_traits<_TElement> >
	using TXScopeCSSSXSTENRPStringConstSection = TXScopeNRPStringConstSection<TXScopeCSSSXSTERAConstIterator<_TElement>, _Traits>;
	template <typename _TElement, class _Traits = std::char_traits<_TElement> >
	using TXScopeCSSSXSTENRPStringSection = TXScopeNRPStringSection<TXScopeCSSSXSTERAIterator<_TElement>, _Traits>;
}

namespace std {

	template <typename _TRAIterator, class _Traits>
	struct hash<mse::TXScopeNRPStringSection<_TRAIterator, _Traits> > : public hash<mse::TXScopeStringSection<_TRAIterator, _Traits> > {
		typedef hash<mse::TXScopeStringSection<_TRAIterator, _Traits> > base_class;
		typedef typename base_class::argument_type argument_type;
		typedef typename base_class::result_type result_type;
	};

	template <typename _TRAIterator, class _Traits>
	struct hash<mse::TNRPStringSection<_TRAIterator, _Traits> > : public hash<mse::TStringSection<_TRAIterator, _Traits> > {
		typedef hash<mse::TNRPStringSection<_TRAIterator, _Traits> > base_class;
		typedef typename base_class::argument_type argument_type;
		typedef typename base_class::result_type result_type;
	};

	template <typename _TRAIterator, class _Traits>
	struct hash<mse::TXScopeNRPStringConstSection<_TRAIterator, _Traits> > : public hash<mse::TXScopeStringConstSection<_TRAIterator, _Traits> > {
		typedef hash<mse::TXScopeStringConstSection<_TRAIterator, _Traits> > base_class;
		typedef typename base_class::argument_type argument_type;
		typedef typename base_class::result_type result_type;
	};

	template <typename _TRAIterator, class _Traits>
	struct hash<mse::TNRPStringConstSection<_TRAIterator, _Traits> > : public hash<mse::TStringConstSection<_TRAIterator, _Traits> > {
		typedef hash<mse::TStringConstSection<_TRAIterator, _Traits> > base_class;
		typedef typename base_class::argument_type argument_type;
		typedef typename base_class::result_type result_type;
	};
}

namespace mse {

	/* shorter aliases */
	template <typename... Args>
	auto mknss(Args&&... args) -> decltype(make_nrp_string_section(std::forward<Args>(args)...)) {
		return make_nrp_string_section(std::forward<Args>(args)...);
	}
	template <typename... Args>
	auto mknscs(Args&&... args) -> decltype(make_nrp_string_const_section(std::forward<Args>(args)...)) {
		return make_nrp_string_const_section(std::forward<Args>(args)...);
	}
}

namespace mse {

	namespace impl {
		namespace ns_gnii_basic_string {
			template<class _TContainer>
			class xscope_structure_lock_guard;
			template<class _TContainer, class _TAccessMutex/* = mse::non_thread_safe_shared_mutex*/>
			class xscope_ewconst_structure_lock_guard;
		}
	}
	namespace us {
		namespace impl {
			namespace ns_gnii_basic_string {
				template<class _TContainer>
				class xscope_const_structure_lock_guard;
			}
		}
	}

	//template<class _Ty> auto make_xscope_structure_lock_guard(const _Ty& owner_ptr);
	template<class _Ty, class _Traits, class _A, class _TStateMutex, template<typename> class _TTXScopeConstIterator>
	auto make_xscope_structure_lock_guard(const mse::TXScopeObjFixedPointer<mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator> >& owner_ptr)->mse::impl::ns_gnii_basic_string::xscope_structure_lock_guard<mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator> >;
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, class _Traits, class _A, class _TStateMutex, template<typename> class _TTXScopeConstIterator>
	auto make_xscope_structure_lock_guard(const mse::TXScopeFixedPointer<mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator> >& owner_ptr)->mse::impl::ns_gnii_basic_string::xscope_structure_lock_guard<mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator> >;
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, class _Traits, class _A, class _TStateMutex, template<typename> class _TTXScopeConstIterator, class _TAccessMutex/* = mse::non_thread_safe_shared_mutex*/>
	auto make_xscope_structure_lock_guard(const mse::TAccessControlledConstPointer<mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>, _TAccessMutex>& owner_ptr)->mse::impl::ns_gnii_basic_string::xscope_ewconst_structure_lock_guard<mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>, _TAccessMutex>;

	namespace impl {
		namespace ns_gnii_basic_string {
			/* Following are a bunch of template (iterator) classes that, organizationally, should be members of gnii_basic_string<>. (And they
			used to be.) However, being a member of gnii_basic_string<> makes them "dependent types", and dependent types do not participate
			in automatic template parameter type deduction. So we had to haul them here outside of gnii_basic_string<>. */

			/* The reason we specify the default parameter in the definition instead of this forward declaration is that there seems to be a
			bug in clang (3.8.0) such that if we don't specify the default parameter in the definition it seems to subsequently behave as if
			one were never specified. g++ and msvc don't seem to have the same issue. */
			template<typename _TBasicStringPointer>
			class Tgnii_basic_string_ss_iterator_type;

			/* Tgnii_basic_string_ss_const_iterator_type is a bounds checked const_iterator. */
			template<typename _TBasicStringConstPointer>
			class Tgnii_basic_string_ss_const_iterator_type : public mse::TFriendlyAugmentedRAConstIterator<mse::TRAConstIterator<_TBasicStringConstPointer> > {
			public:
				typedef mse::TFriendlyAugmentedRAConstIterator<mse::TRAConstIterator<_TBasicStringConstPointer> > base_class;
				MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

				typedef mse::impl::remove_reference_t<mse::impl::remove_const_t<decltype(*std::declval<_TBasicStringConstPointer>())> > _TBasicString;

				template<class _TBasicStringConstPointer2 = _TBasicStringConstPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_TBasicStringConstPointer2, _TBasicStringConstPointer>::value) && (std::is_default_constructible<_TBasicStringConstPointer>::value)> MSE_IMPL_EIS >
				Tgnii_basic_string_ss_const_iterator_type() {}

				Tgnii_basic_string_ss_const_iterator_type(const _TBasicStringConstPointer& owner_cptr) : base_class(owner_cptr) {}
				Tgnii_basic_string_ss_const_iterator_type(_TBasicStringConstPointer&& owner_cptr) : base_class(MSE_FWD(owner_cptr)) {}

				Tgnii_basic_string_ss_const_iterator_type(Tgnii_basic_string_ss_const_iterator_type&& src) = default;
				Tgnii_basic_string_ss_const_iterator_type(const Tgnii_basic_string_ss_const_iterator_type& src) = default;
				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2, _TBasicStringConstPointer>::value> MSE_IMPL_EIS >
				Tgnii_basic_string_ss_const_iterator_type(const Tgnii_basic_string_ss_const_iterator_type<_Ty2>& src) : base_class(src.target_container_ptr(), src.position()) {}
				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2, _TBasicStringConstPointer>::value> MSE_IMPL_EIS >
				Tgnii_basic_string_ss_const_iterator_type(const Tgnii_basic_string_ss_iterator_type<_Ty2>& src) : base_class(src.target_container_ptr(), src.position()) {}

				MSE_USING_ASSIGNMENT_OPERATOR(base_class);
				auto& operator=(Tgnii_basic_string_ss_const_iterator_type&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); }
				auto& operator=(const Tgnii_basic_string_ss_const_iterator_type& _X) { base_class::operator=(_X); return (*this); }

				Tgnii_basic_string_ss_const_iterator_type& operator ++() { (*this).set_to_next(); return (*this); }
				Tgnii_basic_string_ss_const_iterator_type operator++(int) { Tgnii_basic_string_ss_const_iterator_type _Tmp = *this; (*this).set_to_next(); return (_Tmp); }
				Tgnii_basic_string_ss_const_iterator_type& operator --() { (*this).set_to_previous(); return (*this); }
				Tgnii_basic_string_ss_const_iterator_type operator--(int) { Tgnii_basic_string_ss_const_iterator_type _Tmp = *this; (*this).set_to_previous(); return (_Tmp); }

				Tgnii_basic_string_ss_const_iterator_type& operator +=(difference_type n) { (*this).advance(n); return (*this); }
				Tgnii_basic_string_ss_const_iterator_type& operator -=(difference_type n) { (*this).regress(n); return (*this); }
				Tgnii_basic_string_ss_const_iterator_type operator+(difference_type n) const {
					Tgnii_basic_string_ss_const_iterator_type retval(*this);
					retval.advance(n);
					return retval;
				}
				Tgnii_basic_string_ss_const_iterator_type operator-(difference_type n) const { return ((*this) + (-n)); }
				difference_type operator-(const base_class& _Right_cref) const {
					return base_class::operator-(_Right_cref);
				}

				void set_to_const_item_pointer(const Tgnii_basic_string_ss_const_iterator_type& _Right_cref) {
					(*this) = _Right_cref;
				}

				MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(_TBasicStringConstPointer);

			private:

				friend _TBasicString;
			};
			/* Tgnii_basic_string_ss_iterator_type is a bounds checked iterator. */
			template<typename _TBasicStringPointer>
			class Tgnii_basic_string_ss_iterator_type : public mse::TFriendlyAugmentedRAIterator<mse::TRAIterator<_TBasicStringPointer> > {
			public:
				typedef mse::TFriendlyAugmentedRAIterator<mse::TRAIterator<_TBasicStringPointer> > base_class;
				MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

				typedef mse::impl::remove_reference_t<mse::impl::remove_const_t<decltype(*std::declval<_TBasicStringPointer>())> > _TBasicString;

				template<class _TBasicStringPointer2 = _TBasicStringPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_TBasicStringPointer2, _TBasicStringPointer>::value) && (std::is_default_constructible<_TBasicStringPointer>::value)> MSE_IMPL_EIS >
				Tgnii_basic_string_ss_iterator_type() {}

				Tgnii_basic_string_ss_iterator_type(const _TBasicStringPointer& owner_ptr) : base_class(owner_ptr) {}
				Tgnii_basic_string_ss_iterator_type(_TBasicStringPointer&& owner_ptr) : base_class(MSE_FWD(owner_ptr)) {}

				Tgnii_basic_string_ss_iterator_type(Tgnii_basic_string_ss_iterator_type&& src) = default;
				Tgnii_basic_string_ss_iterator_type(const Tgnii_basic_string_ss_iterator_type& src) = default;
				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2, _TBasicStringPointer>::value> MSE_IMPL_EIS >
				Tgnii_basic_string_ss_iterator_type(const Tgnii_basic_string_ss_iterator_type<_Ty2>& src) : base_class(src.target_container_ptr(), src.position()) {}


				MSE_USING_ASSIGNMENT_OPERATOR(base_class);
				auto& operator=(Tgnii_basic_string_ss_iterator_type&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); }
				auto& operator=(const Tgnii_basic_string_ss_iterator_type& _X) { base_class::operator=(_X); return (*this); }

				Tgnii_basic_string_ss_iterator_type& operator ++() { (*this).set_to_next(); return (*this); }
				Tgnii_basic_string_ss_iterator_type operator++(int) { Tgnii_basic_string_ss_iterator_type _Tmp = *this; (*this).set_to_next(); return (_Tmp); }
				Tgnii_basic_string_ss_iterator_type& operator --() { (*this).set_to_previous(); return (*this); }
				Tgnii_basic_string_ss_iterator_type operator--(int) { Tgnii_basic_string_ss_iterator_type _Tmp = *this; (*this).set_to_previous(); return (_Tmp); }

				Tgnii_basic_string_ss_iterator_type& operator +=(difference_type n) { (*this).advance(n); return (*this); }
				Tgnii_basic_string_ss_iterator_type& operator -=(difference_type n) { (*this).regress(n); return (*this); }
				Tgnii_basic_string_ss_iterator_type operator+(difference_type n) const {
					Tgnii_basic_string_ss_iterator_type retval(*this);
					retval.advance(n);
					return retval;
				}
				Tgnii_basic_string_ss_iterator_type operator-(difference_type n) const { return ((*this) + (-n)); }
				difference_type operator-(const base_class& _Right_cref) const {
					return base_class::operator-(_Right_cref);
				}

				void set_to_item_pointer(const Tgnii_basic_string_ss_iterator_type& _Right_cref) {
					(*this) = _Right_cref;
				}

				MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(_TBasicStringPointer);

			private:

				friend _TBasicString;
				template<typename _TBasicStringConstPointer2>
				friend class Tgnii_basic_string_ss_const_iterator_type;
			};

			template<typename _TBasicStringPointer>
			using Tgnii_basic_string_ss_reverse_iterator_type = std::reverse_iterator<Tgnii_basic_string_ss_iterator_type<_TBasicStringPointer> >;
			template<typename _TBasicStringConstPointer>
			using Tgnii_basic_string_ss_const_reverse_iterator_type = std::reverse_iterator<Tgnii_basic_string_ss_const_iterator_type<_TBasicStringConstPointer> >;

			template<typename _TBasicString>
			using Tgnii_basic_string_rp_ss_iterator_type = Tgnii_basic_string_ss_iterator_type<msev_pointer<_TBasicString> >;
			template<typename _TBasicString>
			using Tgnii_basic_string_rp_ss_const_iterator_type = Tgnii_basic_string_ss_const_iterator_type<msev_pointer<const _TBasicString> >;
			template<typename _TBasicString>
			using Tgnii_basic_string_rp_ss_reverse_iterator_type = Tgnii_basic_string_ss_iterator_type<msev_pointer<_TBasicString> >;
			template<typename _TBasicString>
			using Tgnii_basic_string_rp_ss_const_reverse_iterator_type = Tgnii_basic_string_ss_const_reverse_iterator_type<msev_pointer<const _TBasicString> >;


			template<typename _TBasicString>
			using TXScopeBasicStringPointer = mse::TXScopeFixedPointer<_TBasicString>;
			template<typename _TBasicString>
			using TXScopeBasicStringConstPointer = mse::TXScopeFixedConstPointer<_TBasicString>;

			template<typename _TBasicString>
			class Tgnii_basic_string_xscope_cslsstrong_iterator_type;

			template<typename _TBasicString>
			class Tgnii_basic_string_xscope_cslsstrong_const_iterator_type : public mse::TFriendlyAugmentedRAConstIterator<mse::us::impl::TXScopeCSLSStrongRAConstIterator<TXScopeBasicStringConstPointer<_TBasicString>, mse::us::impl::ns_gnii_basic_string::xscope_const_structure_lock_guard<_TBasicString> > >
				/*, public mse::us::impl::XScopeContainsNonOwningScopeReferenceTagBase, public mse::us::impl::AsyncNotShareableAndNotPassableTagBase*/ {
			public:
				typedef mse::TFriendlyAugmentedRAConstIterator<mse::us::impl::TXScopeCSLSStrongRAConstIterator<TXScopeBasicStringConstPointer<_TBasicString>, mse::us::impl::ns_gnii_basic_string::xscope_const_structure_lock_guard<_TBasicString> > > base_class;
				MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

				MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(Tgnii_basic_string_xscope_cslsstrong_const_iterator_type, base_class);

				MSE_USING_ASSIGNMENT_OPERATOR(base_class);
				auto& operator=(Tgnii_basic_string_xscope_cslsstrong_const_iterator_type&& _X) & { base_class::operator=(MSE_FWD(_X)); return (*this); }
				auto& operator=(const Tgnii_basic_string_xscope_cslsstrong_const_iterator_type& _X) & { base_class::operator=(_X); return (*this); }

				Tgnii_basic_string_xscope_cslsstrong_const_iterator_type& operator ++() & { base_class::operator ++(); return (*this); }
				Tgnii_basic_string_xscope_cslsstrong_const_iterator_type operator++(int) { Tgnii_basic_string_xscope_cslsstrong_const_iterator_type _Tmp = *this; base_class::operator++(); return (_Tmp); }
				Tgnii_basic_string_xscope_cslsstrong_const_iterator_type& operator --() & { base_class::operator --(); return (*this); }
				Tgnii_basic_string_xscope_cslsstrong_const_iterator_type operator--(int) { Tgnii_basic_string_xscope_cslsstrong_const_iterator_type _Tmp = *this; base_class::operator--(); return (_Tmp); }

				Tgnii_basic_string_xscope_cslsstrong_const_iterator_type& operator +=(difference_type n) & { base_class::operator +=(n); return (*this); }
				Tgnii_basic_string_xscope_cslsstrong_const_iterator_type& operator -=(difference_type n) & { base_class::operator -=(n); return (*this); }
				Tgnii_basic_string_xscope_cslsstrong_const_iterator_type operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
				Tgnii_basic_string_xscope_cslsstrong_const_iterator_type operator-(difference_type n) const { return ((*this) + (-n)); }
				difference_type operator-(const Tgnii_basic_string_xscope_cslsstrong_const_iterator_type& _Right_cref) const { return base_class::operator-(_Right_cref); }
				const_reference operator*() const { return base_class::operator*(); }

				Tgnii_basic_string_xscope_cslsstrong_const_iterator_type operator=(Tgnii_basic_string_xscope_cslsstrong_const_iterator_type&& _X) && { base_class::operator=(MSE_FWD(_X)); return std::move(*this); }
				Tgnii_basic_string_xscope_cslsstrong_const_iterator_type operator=(const Tgnii_basic_string_xscope_cslsstrong_const_iterator_type& _X) && { base_class::operator=(_X); return std::move(*this); }
				Tgnii_basic_string_xscope_cslsstrong_const_iterator_type operator ++() && { base_class::operator ++(); return std::move(*this); }
				Tgnii_basic_string_xscope_cslsstrong_const_iterator_type operator --() && { base_class::operator --(); return std::move(*this); }
				Tgnii_basic_string_xscope_cslsstrong_const_iterator_type operator +=(difference_type n) && { base_class::operator +=(n); return std::move(*this); }
				Tgnii_basic_string_xscope_cslsstrong_const_iterator_type operator -=(difference_type n) && { base_class::operator -=(n); return std::move(*this); }

				void set_to_const_item_pointer(const Tgnii_basic_string_xscope_cslsstrong_const_iterator_type& _Right_cref) { base_class::set_to_item_pointer(_Right_cref); }

				MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(base_class);
				void xscope_cslsstrong_iterator_type_tag() const {}

			private:
				MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

				friend _TBasicString;
				template<class _TBasicString2>
				friend class Tgnii_basic_string_xscope_cslsstrong_iterator_type;
			};

			template<typename _TBasicString>
			class Tgnii_basic_string_xscope_cslsstrong_iterator_type : public mse::TFriendlyAugmentedRAIterator<mse::us::impl::TXScopeCSLSStrongRAIterator<TXScopeBasicStringPointer<_TBasicString>, mse::impl::ns_gnii_basic_string::xscope_structure_lock_guard<_TBasicString> > >
				/*, public mse::us::impl::XScopeContainsNonOwningScopeReferenceTagBase, public mse::us::impl::AsyncNotShareableAndNotPassableTagBase*/ {
			public:
				typedef mse::TFriendlyAugmentedRAIterator<mse::us::impl::TXScopeCSLSStrongRAIterator<TXScopeBasicStringPointer<_TBasicString>, mse::impl::ns_gnii_basic_string::xscope_structure_lock_guard<_TBasicString> > > base_class;
				MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

				MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(Tgnii_basic_string_xscope_cslsstrong_iterator_type, base_class);

				MSE_USING_ASSIGNMENT_OPERATOR(base_class);
				auto& operator=(Tgnii_basic_string_xscope_cslsstrong_iterator_type&& _X) & { base_class::operator=(MSE_FWD(_X)); return (*this); }
				auto& operator=(const Tgnii_basic_string_xscope_cslsstrong_iterator_type& _X) & { base_class::operator=(_X); return (*this); }

				Tgnii_basic_string_xscope_cslsstrong_iterator_type& operator ++() & { base_class::operator ++(); return (*this); }
				Tgnii_basic_string_xscope_cslsstrong_iterator_type operator++(int) { Tgnii_basic_string_xscope_cslsstrong_iterator_type _Tmp = *this; base_class::operator++(); return (_Tmp); }
				Tgnii_basic_string_xscope_cslsstrong_iterator_type& operator --() & { base_class::operator --(); return (*this); }
				Tgnii_basic_string_xscope_cslsstrong_iterator_type operator--(int) { Tgnii_basic_string_xscope_cslsstrong_iterator_type _Tmp = *this; base_class::operator--(); return (_Tmp); }

				Tgnii_basic_string_xscope_cslsstrong_iterator_type& operator +=(difference_type n) & { base_class::operator +=(n); return (*this); }
				Tgnii_basic_string_xscope_cslsstrong_iterator_type& operator -=(difference_type n) & { base_class::operator -=(n); return (*this); }
				Tgnii_basic_string_xscope_cslsstrong_iterator_type operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
				Tgnii_basic_string_xscope_cslsstrong_iterator_type operator-(difference_type n) const { return ((*this) + (-n)); }
				difference_type operator-(const Tgnii_basic_string_xscope_cslsstrong_iterator_type& _Right_cref) const { return base_class::operator-(_Right_cref); }
				const_reference operator*() const { return base_class::operator*(); }

				Tgnii_basic_string_xscope_cslsstrong_iterator_type operator=(Tgnii_basic_string_xscope_cslsstrong_iterator_type&& _X) && { base_class::operator=(MSE_FWD(_X)); return std::move(*this); }
				Tgnii_basic_string_xscope_cslsstrong_iterator_type operator=(const Tgnii_basic_string_xscope_cslsstrong_iterator_type& _X) && { base_class::operator=(_X); return std::move(*this); }
				Tgnii_basic_string_xscope_cslsstrong_iterator_type operator ++() && { base_class::operator ++(); return std::move(*this); }
				Tgnii_basic_string_xscope_cslsstrong_iterator_type operator --() && { base_class::operator --(); return std::move(*this); }
				Tgnii_basic_string_xscope_cslsstrong_iterator_type operator +=(difference_type n) && { base_class::operator +=(n); return std::move(*this); }
				Tgnii_basic_string_xscope_cslsstrong_iterator_type operator -=(difference_type n) && { base_class::operator -=(n); return std::move(*this); }

				void set_to_item_pointer(const Tgnii_basic_string_xscope_cslsstrong_iterator_type& _Right_cref) { base_class::set_to_item_pointer(_Right_cref); }

				MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(base_class);
				void xscope_cslsstrong_iterator_type_tag() const {}

			private:
				MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

				friend _TBasicString;
			};

			template<typename _TBasicString>
			class Tgnii_basic_string_xscope_ss_iterator_type;

			template<typename _TBasicString>
			class Tgnii_basic_string_xscope_ss_const_iterator_type : public mse::TFriendlyAugmentedRAConstIterator<mse::TXScopeRAConstIterator<mse::TXScopeFixedConstPointer<_TBasicString> > >
				/*, public mse::us::impl::XScopeContainsNonOwningScopeReferenceTagBase, public mse::us::impl::AsyncNotShareableAndNotPassableTagBase*/ {
			public:
				typedef mse::TFriendlyAugmentedRAConstIterator<mse::TXScopeRAConstIterator<mse::TXScopeFixedConstPointer<_TBasicString> > > base_class;
				MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

				MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(Tgnii_basic_string_xscope_ss_const_iterator_type, base_class);

				/* gnii_basic_string<> uses this iterator class as its "xscope const" iterator, but uses the Tgnii_basic_string_xscope_cslsstrong_iterator_type<>
				iterator class for its "xscope non-const" iterator. So this class needs to support construction from that class.
				This class requires a TXScopeFixedConstPointer<> to the container, but the other class instead holds an
				xscope_structure_lock_guard<>, which can be converted to the needed TXScopeFixedConstPointer<> with an
				explicit intermediary conversion (to a TXScopeFixedPointer<>). */
				Tgnii_basic_string_xscope_ss_const_iterator_type(const Tgnii_basic_string_xscope_cslsstrong_iterator_type<_TBasicString>& src)
					: base_class(mse::TXScopeFixedPointer<_TBasicString>(src.target_container_ptr()), src.position()) {}

				//MSE_USING_ASSIGNMENT_OPERATOR(base_class);
				/* Normally we would just use the macro for inheriting assignment operators, but here we need to exclude any
				that handle Tgnii_basic_string_xscope_cslsstrong_iterator_type<>s, because that class needs to be handled a bit
				differently. */
				template<class _Ty2mse_uao, class _Tbase_class2 = base_class, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::HasOrInheritsAssignmentOperator_msepointerbasics<_Tbase_class2>::value \
					&& (!std::is_same<Tgnii_basic_string_xscope_cslsstrong_iterator_type<_TBasicString>, mse::impl::remove_reference_t<_Ty2mse_uao> >::value) \
					&& ((!mse::impl::is_a_pair_with_the_first_a_base_of_the_second<_Tbase_class2, _Ty2mse_uao>::value) || std::is_same<_Tbase_class2, mse::impl::remove_reference_t<_Ty2mse_uao> >::value)> MSE_IMPL_EIS > \
				auto& operator=(_Ty2mse_uao&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); } \
				template<class _Ty2mse_uao, class _Tbase_class2 = base_class, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::HasOrInheritsAssignmentOperator_msepointerbasics<_Tbase_class2>::value \
					&& (!std::is_same<Tgnii_basic_string_xscope_cslsstrong_iterator_type<_TBasicString>, mse::impl::remove_reference_t<_Ty2mse_uao> >::value) \
					&& ((!mse::impl::is_a_pair_with_the_first_a_base_of_the_second<_Tbase_class2, _Ty2mse_uao>::value) || std::is_same<_Tbase_class2, mse::impl::remove_reference_t<_Ty2mse_uao> >::value)> MSE_IMPL_EIS > \
				auto& operator=(const _Ty2mse_uao& _X) { base_class::operator=(_X); return (*this); }

				auto& operator=(Tgnii_basic_string_xscope_ss_const_iterator_type&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); }
				auto& operator=(const Tgnii_basic_string_xscope_ss_const_iterator_type& _X) { base_class::operator=(_X); return (*this); }

				Tgnii_basic_string_xscope_ss_const_iterator_type& operator ++() { base_class::operator ++(); return (*this); }
				Tgnii_basic_string_xscope_ss_const_iterator_type operator++(int) { Tgnii_basic_string_xscope_ss_const_iterator_type _Tmp = *this; base_class::operator++(); return (_Tmp); }
				Tgnii_basic_string_xscope_ss_const_iterator_type& operator --() { base_class::operator --(); return (*this); }
				Tgnii_basic_string_xscope_ss_const_iterator_type operator--(int) { Tgnii_basic_string_xscope_ss_const_iterator_type _Tmp = *this; base_class::operator--(); return (_Tmp); }

				Tgnii_basic_string_xscope_ss_const_iterator_type& operator +=(difference_type n) { base_class::operator +=(n); return (*this); }
				Tgnii_basic_string_xscope_ss_const_iterator_type& operator -=(difference_type n) { base_class::operator -=(n); return (*this); }
				Tgnii_basic_string_xscope_ss_const_iterator_type operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
				Tgnii_basic_string_xscope_ss_const_iterator_type operator-(difference_type n) const { return ((*this) + (-n)); }
				difference_type operator-(const Tgnii_basic_string_xscope_ss_const_iterator_type& _Right_cref) const { return base_class::operator-(_Right_cref); }
				const_reference operator*() const { return base_class::operator*(); }

				void set_to_const_item_pointer(const Tgnii_basic_string_xscope_ss_const_iterator_type& _Right_cref) { base_class::set_to_item_pointer(_Right_cref); }

				MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(base_class);
				void xscope_ss_iterator_type_tag() const {}

			private:
				MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

				friend _TBasicString;
				template<class _TBasicString2>
				friend class Tgnii_basic_string_xscope_ss_iterator_type;
			};
			template<typename _TBasicString>
			class Tgnii_basic_string_xscope_ss_iterator_type : public mse::TFriendlyAugmentedRAIterator<mse::TXScopeRAIterator<mse::TXScopeFixedPointer<_TBasicString> > >
				/*, public mse::us::impl::XScopeContainsNonOwningScopeReferenceTagBase, public mse::us::impl::AsyncNotShareableAndNotPassableTagBase*/ {
			public:
				typedef mse::TFriendlyAugmentedRAIterator<mse::TXScopeRAIterator<mse::TXScopeFixedPointer<_TBasicString> > > base_class;
				MSE_INHERITED_RANDOM_ACCESS_ITERATOR_MEMBER_TYPE_DECLARATIONS(base_class);

				MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(Tgnii_basic_string_xscope_ss_iterator_type, base_class);

				MSE_USING_ASSIGNMENT_OPERATOR(base_class);
				auto& operator=(Tgnii_basic_string_xscope_ss_iterator_type&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); }
				auto& operator=(const Tgnii_basic_string_xscope_ss_iterator_type& _X) { base_class::operator=(_X); return (*this); }

				Tgnii_basic_string_xscope_ss_iterator_type& operator ++() { base_class::operator ++(); return (*this); }
				Tgnii_basic_string_xscope_ss_iterator_type operator++(int) { Tgnii_basic_string_xscope_ss_iterator_type _Tmp = *this; base_class::operator++(); return (_Tmp); }
				Tgnii_basic_string_xscope_ss_iterator_type& operator --() { base_class::operator --(); return (*this); }
				Tgnii_basic_string_xscope_ss_iterator_type operator--(int) { Tgnii_basic_string_xscope_ss_iterator_type _Tmp = *this; base_class::operator--(); return (_Tmp); }

				Tgnii_basic_string_xscope_ss_iterator_type& operator +=(difference_type n) { base_class::operator +=(n); return (*this); }
				Tgnii_basic_string_xscope_ss_iterator_type& operator -=(difference_type n) { base_class::operator -=(n); return (*this); }
				Tgnii_basic_string_xscope_ss_iterator_type operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
				Tgnii_basic_string_xscope_ss_iterator_type operator-(difference_type n) const { return ((*this) + (-n)); }
				difference_type operator-(const Tgnii_basic_string_xscope_ss_iterator_type& _Right_cref) const { return base_class::operator-(_Right_cref); }

				void set_to_item_pointer(const Tgnii_basic_string_xscope_ss_iterator_type& _Right_cref) { base_class::set_to_item_pointer(_Right_cref); }

				MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(base_class);
				void xscope_ss_iterator_type_tag() const {}

			private:
				MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

				friend _TBasicString;
			};
		}
	}

	namespace mstd {
		template <typename _Ty, class _Traits> class basic_string_view;
	}

	namespace us {
		namespace impl {

			namespace impl {
				namespace ns_gnii_basic_string {
					template<class _Ty, class _Traits/* = std::char_traits<_Ty>*/, class _A/* = std::allocator<_Ty>*/, class _TStateMutex/* = mse::non_thread_safe_shared_mutex*/, template<typename> class _TTXScopeConstIterator/* = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_ss_const_iterator_type*/>
					std::basic_istream<_Ty, _Traits>& in_from_stream(std::basic_istream<_Ty, _Traits>&& _Istr, gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>& _Str);
					template<class _Ty, class _Traits/* = std::char_traits<_Ty>*/, class _A/* = std::allocator<_Ty>*/, class _TStateMutex/* = mse::non_thread_safe_shared_mutex*/, template<typename> class _TTXScopeConstIterator/* = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_ss_const_iterator_type*/>
					std::basic_istream<_Ty, _Traits>& in_from_stream(std::basic_istream<_Ty, _Traits>& _Istr, gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>& _Str);
					template<class _Ty, class _Traits/* = std::char_traits<_Ty>*/, class _A/* = std::allocator<_Ty>*/, class _TStateMutex/* = mse::non_thread_safe_shared_mutex*/, template<typename> class _TTXScopeConstIterator/* = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_ss_const_iterator_type*/>
					std::basic_ostream<_Ty, _Traits>& out_to_stream(std::basic_ostream<_Ty, _Traits>& _Ostr, const gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>& _Str);
				}
			}

			/* gnii_basic_string<> is essentially a memory-safe basic_string that does not expose (unprotected) non-static member functions
			like begin() or end() which return (memory) unsafe iterators. It does provide static member function templates
			like ss_begin<>(...) and ss_end<>(...) which take a pointer parameter and return a (bounds-checked) iterator that
			inherits the safety of the given pointer. gnii_basic_string<> also supports "scope" iterators which are safe without any
			run-time overhead. gnii_basic_string<> is a data type that is eligible to be shared between asynchronous threads. */
			/* Default template parameter values are specified in the forward declaration. */
			template<class _Ty, class _Traits/* = std::char_traits<_Ty>*/, class _A/* = std::allocator<_Ty>*/, class _TStateMutex/* = mse::non_thread_safe_shared_mutex*/, template<typename> class _TTXScopeConstIterator/* = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_ss_const_iterator_type*/>
			class gnii_basic_string : private mse::impl::TOpaqueWrapper<std::basic_string<_Ty, _Traits, _A> >, public us::impl::ContiguousSequenceContainerTagBase, public us::impl::LockableStructureContainerTagBase {
			public:
#ifdef MSE_HAS_CXX17
				/* Helper classes for converting from string_views. */
				template<class _StringViewIsh>
				using _is_string_view_ish = std::conjunction<
					std::is_convertible<const _StringViewIsh&, std::basic_string_view<_Ty, _Traits>>,
					std::negation<std::is_convertible<const _StringViewIsh&, const _Ty *>>
				>;
				template<class _StringViewIsh>
				using _Is_string_view_ish = std::enable_if_t<_is_string_view_ish<_StringViewIsh>::value>;

			private:
				/* We support conversion from both stringviews and string sections. For now we're just using a rough approximation
				for what should qualify as a "string section". */
				template<class T, class EqualTo>
				struct IsStringViewOrSectionish1_impl
				{
					static void foo1(const _Ty*) {}
					template<class U, class V>
					//static auto test(U*) -> decltype(_Ty(*mse::make_begin_const_iterator(std::declval<U>())), _Ty(*mse::make_begin_const_iterator(std::declval<V>())), bool(true));
					static auto test(U*) -> decltype(foo1(std::addressof(*std::cbegin(std::declval<U>()))), foo1(std::addressof(*std::cbegin(std::declval<V>()))), bool(true));
					template<typename, typename>
					static auto test(...)->std::false_type;

					static const bool value = std::is_same<bool, decltype(test<T, EqualTo>(0))>::value;
					using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
				};
				template<class T, class EqualTo = T>
				struct IsStringViewOrSectionish1 : IsStringViewOrSectionish1_impl<
					mse::impl::remove_reference_t<T>, mse::impl::remove_reference_t<EqualTo> >::type {};

				template<class T, class EqualTo>
				struct IsStringViewOrSectionish2_impl
				{
					static void foo1(const _Ty*) {}
					template<class U, class V>
					static auto test(U*) -> decltype(foo1(std::addressof(*std::cbegin(*std::declval<U>()))), foo1(std::addressof(*std::cbegin(*std::declval<V>()))), bool(true));
					template<typename, typename>
					static auto test(...)->std::false_type;

					static const bool value = std::is_same<bool, decltype(test<T, EqualTo>(0))>::value;
					using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
				};
				template<class T, class EqualTo = T>
				struct IsStringViewOrSectionish2 : IsStringViewOrSectionish2_impl<
					mse::impl::remove_reference_t<T>, mse::impl::remove_reference_t<EqualTo> >::type {};

				template<class T, class EqualTo>
				struct IsStringViewOrSectionish3_impl
				{
					static void foo1(const _Ty*) {}
					template<class U, class V>
					static auto test(U*) -> decltype(foo1(std::addressof((*std::declval<U>())[0])), foo1(std::addressof((*std::declval<U>())[0])), bool(true));
					template<typename, typename>
					static auto test(...)->std::false_type;

					static const bool value = std::is_same<bool, decltype(test<T, EqualTo>(0))>::value;
					using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
				};
				template<class T, class EqualTo = T>
				struct IsStringViewOrSectionish3 : IsStringViewOrSectionish3_impl<
					mse::impl::remove_reference_t<T>, mse::impl::remove_reference_t<EqualTo> >::type {};

			public:
				template<class _StringViewIsh>
				using _is_string_view_or_section_ish = std::conjunction<
					std::disjunction<_is_string_view_ish<_StringViewIsh>, IsStringViewOrSectionish1<_StringViewIsh>, IsStringViewOrSectionish2<_StringViewIsh>, IsStringViewOrSectionish3<_StringViewIsh> >,
					std::negation<std::is_convertible<const _StringViewIsh&, const _Ty*>>
				>;
				template<class _StringViewIsh>
				using _Is_string_view_or_section_ish = std::enable_if_t<_is_string_view_or_section_ish<_StringViewIsh>::value>;
#endif /* MSE_HAS_CXX17 */

			public:
				typedef _TStateMutex state_mutex_type;

				/* We (privately) inherit the underlying data type rather than make it a data member to ensure it's the "first" component in the structure.*/
				typedef mse::impl::TOpaqueWrapper<std::basic_string<_Ty, _Traits, _A> > base_class;
				typedef std::basic_string<_Ty, _Traits, _A> std_basic_string;
				typedef std_basic_string _MBS;
				typedef gnii_basic_string _Myt;

				typedef typename std_basic_string::allocator_type allocator_type;
				typedef typename std_basic_string::value_type value_type;
				//typedef typename std_basic_string::size_type size_type;
				typedef msev_size_t size_type;
				//typedef typename std_basic_string::difference_type difference_type;
				typedef msev_int difference_type;
				typedef typename std_basic_string::pointer pointer;
				typedef typename std_basic_string::const_pointer const_pointer;
				typedef typename std_basic_string::reference reference;
				typedef typename std_basic_string::const_reference const_reference;
				static const size_t npos = size_t(-1);

				typedef typename std_basic_string::iterator iterator;
				typedef typename std_basic_string::const_iterator const_iterator;

				typedef typename std_basic_string::reverse_iterator reverse_iterator;
				typedef typename std_basic_string::const_reverse_iterator const_reverse_iterator;

			private:
				const _MBS& contained_basic_string() const& { return (*this).value(); }
				const _MBS& contained_basic_string() const&& { return (*this).value(); }
				_MBS& contained_basic_string() & { return (*this).value(); }
				_MBS&& contained_basic_string() && {
					/* We're making sure that the basic_string is not "structure locked", because in that case it might not be
					safe to to allow the contained basic_string to be moved from (when made movable with std::move()). */
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					return std::move(*this).value();
				}

			public:
				explicit gnii_basic_string(const _A& _Al = _A()) : base_class(_Al) { /*m_debug_size = size();*/ }
				//explicit gnii_basic_string(size_type _N) : base_class(msev_as_a_size_t(_N)) { /*m_debug_size = size();*/ }
		#ifdef MSE_HAS_CXX17
				template<class _Alloc2 = _A, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc2>::value, int> = 0>
		#endif /* MSE_HAS_CXX17 */
				explicit gnii_basic_string(size_type _N, const _Ty& _V, const _A& _Al = _A()) : base_class(msev_as_a_size_t(_N), _V, _Al) { /*m_debug_size = size();*/ }
				gnii_basic_string(std_basic_string&& _X) : base_class(MSE_FWD(_X)) { /*m_debug_size = size();*/ }
				gnii_basic_string(const std_basic_string& _X) : base_class(_X) { /*m_debug_size = size();*/ }
				gnii_basic_string(_Myt&& _X) : base_class(MSE_FWD(_X).contained_basic_string()) { /*m_debug_size = size();*/ }
				gnii_basic_string(const _Myt& _X) : base_class(_X.contained_basic_string()) { /*m_debug_size = size();*/ }
				typedef typename std_basic_string::const_iterator _It;
				/* Note that safety cannot be guaranteed when using these constructors that take unsafe typename base_class::iterator and/or pointer parameters. */
				gnii_basic_string(_It _F, _It _L, const _A& _Al = _A()) : base_class(_F, _L, _Al) { /*m_debug_size = size();*/ }
				gnii_basic_string(const _Ty*  _F, const _Ty*  _L, const _A& _Al = _A()) : base_class(_F, _L, _Al) { /*m_debug_size = size();*/ }
				template<class _Iter
					//, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value> MSE_IMPL_EIS
					, class = mse::impl::_mse_RequireInputIter<_Iter> >
					gnii_basic_string(const _Iter& _First, const _Iter& _Last) : base_class(_First, _Last) { /*m_debug_size = size();*/ }
				template<class _Iter
					//, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value> MSE_IMPL_EIS
					, class = mse::impl::_mse_RequireInputIter<_Iter> >
					//gnii_basic_string(const _Iter& _First, const _Iter& _Last, const typename std_basic_string::_Alloc& _Al) : base_class(_First, _Last, _Al) { /*m_debug_size = size();*/ }
					gnii_basic_string(const _Iter& _First, const _Iter& _Last, const _A& _Al) : base_class(_First, _Last, _Al) { /*m_debug_size = size();*/ }
				gnii_basic_string(const _Ty* const _Ptr) : base_class(_Ptr) { /*m_debug_size = size();*/ }
				gnii_basic_string(const _Ty* const _Ptr, const size_t _Count) : base_class(_Ptr, mse::msev_as_a_size_t(_Count)) { /*m_debug_size = size();*/ }
				gnii_basic_string(const _Myt& _X, const size_type _Roff, const _A& _Al = _A()) : base_class(_X.contained_basic_string(), _Roff, npos, _Al) { /*m_debug_size = size();*/ }
				gnii_basic_string(const _Myt& _X, const size_type _Roff, const size_type _Count, const _A& _Al = _A()) : base_class(_X.contained_basic_string(), _Roff, _Count, _Al) { /*m_debug_size = size();*/ }

				gnii_basic_string(const mse::TXScopeFixedConstPointer<_Myt>& xs_ptr) : base_class(xs_ptr->contained_basic_string()) { /*m_debug_size = size();*/ }
				gnii_basic_string(const mse::TXScopeFixedConstPointer<_Myt>& xs_ptr, const size_type _Roff, const _A& _Al = _A()) : base_class(xs_ptr->contained_basic_string(), _Roff, npos, _Al) { /*m_debug_size = size();*/ }
				gnii_basic_string(const mse::TXScopeFixedConstPointer<_Myt>& xs_ptr, const size_type _Roff, const size_type _Count, const _A& _Al = _A()) : base_class(xs_ptr->contained_basic_string(), _Roff, _Count, _Al) { /*m_debug_size = size();*/ }

		#ifdef MSE_HAS_CXX17
				template<class _TParam1, class = _Is_string_view_or_section_ish<_TParam1> >
				gnii_basic_string(const _TParam1& _Right) { assign(_Right); }

				template<class _TParam1, class = _Is_string_view_or_section_ish<_TParam1> >
				gnii_basic_string(const _TParam1& _Right, const size_type _Roff, const size_type _Count, const _A& _Al = _A())
				: base_class(_Al) {
					assign(_Right, _Roff, _Count);
				}
		#else /* MSE_HAS_CXX17 */
				template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
				explicit gnii_basic_string(const _TStringSection& _X) : base_class(_X.cbegin(), _X.cend()) { /*m_debug_size = size();*/ }
		#endif /* MSE_HAS_CXX17 */

				/*
				_Myt& operator=(const std_basic_string& _X) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().operator =(_X);
					//m_debug_size = size();
					return (*this);
				}
				*/
				_Myt& operator=(_Myt&& _X) {
					if (std::addressof(_X) == this) { return (*this); }
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().operator=(MSE_FWD(_X).contained_basic_string());
					return (*this);
				}
				_Myt& operator=(const _Myt& _X) {
					if (std::addressof(_X) == this) { return (*this); }
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().operator=(_X.contained_basic_string());
					return (*this);
				}

				~gnii_basic_string() {
					mse::impl::destructor_lock_guard1<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);

					/* This is just a no-op function that will cause a compile error when _Ty is not an eligible type. */
					valid_if_Ty_is_not_an_xscope_type();
				}

				operator _MBS() const & { return contained_basic_string(); }
				operator _MBS() && { return std::forward<_MBS>(contained_basic_string()); }

				void reserve(size_type _Count)
				{	// determine new minimum length of allocated storage
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().reserve(msev_as_a_size_t(_Count));
				}
				size_type capacity() const _NOEXCEPT
				{	// return current length of allocated storage
					return contained_basic_string().capacity();
				}
				void shrink_to_fit() {	// reduce capacity
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().shrink_to_fit();
				}
				void resize(size_type _N, const _Ty& _X = _Ty()) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().resize(msev_as_a_size_t(_N), _X);
				}
				typename std_basic_string::const_reference operator[](msev_size_t _P) const {
					return (*this).at(msev_as_a_size_t(_P));
				}
				typename std_basic_string::reference operator[](msev_size_t _P) {
					return (*this).at(msev_as_a_size_t(_P));
				}
				typename std_basic_string::reference front() {	// return first element of mutable sequence
					if (0 == (*this).size()) { MSE_THROW(gnii_basic_string_range_error("front() on empty - typename std_basic_string::reference front() - gnii_basic_string")); }
					return contained_basic_string().front();
				}
				typename std_basic_string::const_reference front() const {	// return first element of nonmutable sequence
					if (0 == (*this).size()) { MSE_THROW(gnii_basic_string_range_error("front() on empty - typename std_basic_string::const_reference front() - gnii_basic_string")); }
					return contained_basic_string().front();
				}
				typename std_basic_string::reference back() {	// return last element of mutable sequence
					if (0 == (*this).size()) { MSE_THROW(gnii_basic_string_range_error("back() on empty - typename std_basic_string::reference back() - gnii_basic_string")); }
					return contained_basic_string().back();
				}
				typename std_basic_string::const_reference back() const {	// return last element of nonmutable sequence
					if (0 == (*this).size()) { MSE_THROW(gnii_basic_string_range_error("back() on empty - typename std_basic_string::const_reference back() - gnii_basic_string")); }
					return contained_basic_string().back();
				}
				void push_back(_Ty&& _X) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().push_back(MSE_FWD(_X));
				}
				void push_back(const _Ty& _X) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().push_back(_X);
				}
				void pop_back() {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().pop_back();
				}


				gnii_basic_string& assign(mse::TXScopeFixedConstPointer<gnii_basic_string> xs_ptr) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().assign(xs_ptr->contained_basic_string());
					/*m_debug_size = size();*/
					return (*this);
				}
				gnii_basic_string& assign(const gnii_basic_string& _Right) {
					auto xs_ptr = mse::us::unsafe_make_xscope_const_pointer_to(_Right);
					return assign(xs_ptr);
				}
				gnii_basic_string& assign(mse::TXScopeFixedConstPointer<gnii_basic_string> xs_ptr, const size_type _Roff, size_type _Count = npos) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().assign(xs_ptr->contained_basic_string(), mse::as_a_size_t(_Roff), mse::as_a_size_t(_Count));
					/*m_debug_size = size();*/
					return (*this);
				}
				gnii_basic_string& assign(const gnii_basic_string& _Right, const size_type _Roff, size_type _Count = npos) {
					auto xs_ptr = mse::us::unsafe_make_xscope_const_pointer_to(_Right);
					return assign(xs_ptr, _Roff, _Count);
				}

				gnii_basic_string& assign(const _Ty * const _Ptr, const size_type _Count) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().assign(_Ptr, mse::as_a_size_t(_Count));
					//m_debug_size = size();
					return (*this);
				}
				gnii_basic_string& assign(const _Ty * const _Ptr) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().assign(_Ptr);
					//m_debug_size = size();
					return (*this);
				}
				gnii_basic_string& assign(const size_type _Count, const _Ty& _Ch) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().assign(mse::as_a_size_t(_Count), _Ch);
					//m_debug_size = size();
					return (*this);
				}
				template<class _Iter, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::_mse_Is_iterator_v<_Iter> > MSE_IMPL_EIS >
				gnii_basic_string& assign(const _Iter _First, const _Iter _Last) {
					smoke_check_source_iterators(_First, _Last);
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().assign(_First, _Last);
					//m_debug_size = size();
					return (*this);
				}

		#ifdef MSE_HAS_CXX17
			private:
				template<class _StringViewIsh>
				gnii_basic_string& assign_helper2(std::true_type, const _StringViewIsh& _Right) {
					std::basic_string_view<_Ty, _Traits> _As_view = _Right;
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().assign(_As_view);
					/*m_debug_size = size();*/
					return (*this);
				}
				template<class _TParam1>
				gnii_basic_string& assign_helper2(std::false_type, const _TParam1& _Right) {
					const auto xs_iters = mse::impl::make_xscope_range_iter_provider(_Right);
					return assign(xs_iters.begin(), xs_iters.end());
				}
				gnii_basic_string& assign_helper1(std::true_type, const gnii_basic_string& _Right) {
					assign(_Right);
					return (*this);
				}
				template<class _TParam1>
				gnii_basic_string& assign_helper1(std::false_type, const _TParam1& _Right) {
					return assign_helper2(typename _is_string_view_ish<_TParam1>::type(), _Right);
				}
			public:
				template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
				gnii_basic_string& assign(const _TParam1& _Right) {
					return assign_helper1(typename std::is_base_of<gnii_basic_string, _TParam1>::type(), _Right);
				}

			private:
				template<class _StringViewIsh>
				gnii_basic_string& assign_helper2(std::true_type, const _StringViewIsh& _Right, const size_type _Roff, const size_type _Count) {
					std::basic_string_view<_Ty, _Traits> _As_view = _Right;
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().assign(_As_view, mse::as_a_size_t(_Roff), mse::as_a_size_t(_Count));
					/*m_debug_size = size();*/
					return (*this);
				}
				template<class _TParam1>
				gnii_basic_string& assign_helper2(std::false_type, const _TParam1& _Right, const size_type _Roff, const size_type _Count) {
					auto adjusted_count = (npos == _Count) ? (difference_type(mse::container_size(_Right)) - difference_type(_Roff))
						: difference_type(_Count);
					if (0 > adjusted_count) { MSE_THROW(std::out_of_range(" gnii_basic_string::assign() ")); }
					const auto xs_iters = mse::impl::make_xscope_range_iter_provider(_Right);
					return assign(xs_iters.begin() + difference_type(_Roff), xs_iters.begin() + difference_type(_Roff + adjusted_count));
				}
				gnii_basic_string& assign_helper1(std::true_type, const gnii_basic_string& _Right, const size_type _Roff, const size_type _Count) {
					assign(_Right, _Roff, _Count);
					return (*this);
				}
				template<class _TParam1>
				gnii_basic_string& assign_helper1(std::false_type, const _TParam1& _Right, const size_type _Roff, const size_type _Count) {
					return assign_helper2(typename _is_string_view_ish<_TParam1>::type(), _Right, _Roff, _Count);
				}
			public:
				template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
				gnii_basic_string& assign(const _TParam1& _Right, const size_type _Roff, const size_type _Count = npos) {
					return assign_helper1(typename std::is_base_of<gnii_basic_string, _TParam1>::type(), _Right, _Roff, _Count);
				}
		#else /* MSE_HAS_CXX17 */
				template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
				gnii_basic_string& assign(const _TStringSection& _X) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().assign(_X.cbegin(), _X.cend());
					/*m_debug_size = size();*/
					return (*this);
				}
		#endif /* MSE_HAS_CXX17 */


				template<class _Iter>
				static void smoke_check_source_iterators_helper(std::true_type, const _Iter& _First, const _Iter& _Last) {
					if (_Last < _First)/*comparison operations should also verify that safe iterators point to the same container*/ {
						MSE_THROW(gnii_basic_string_range_error("invalid arguments - void smoke_check_source_iterators() const - gnii_basic_string"));
					}
				}
				template<class _Iter>
				static void smoke_check_source_iterators_helper(std::false_type, const _Iter&, const _Iter&) {}

				template<class _Iter>
				static void smoke_check_source_iterators(const gnii_basic_string& target_cref, const _Iter& _First, const _Iter& _Last) {
		#ifndef MSE_NII_BASIC_STRING_SUPRESS_SOURCE_ITER_ALIAS_CHECK
					if (!(target_cref.empty())) {
						/* check if the source sequence is part of target (target) container */
						auto start_of_target_ptr = std::addressof(*(target_cref.cbegin()));
						auto end_of_target_ptr = std::addressof(*(target_cref.cend() - 1)) + 1;
						auto _First_ptr = std::addressof(*_First);
						if ((end_of_target_ptr > _First_ptr) && (start_of_target_ptr <= _First_ptr)) {
							MSE_THROW(gnii_basic_string_range_error("invalid arguments - void smoke_check_source_iterators() const - gnii_basic_string"));
						}
					}
		#endif // !MSE_NII_BASIC_STRING_SUPRESS_SOURCE_ITER_ALIAS_CHECK

		#ifdef MSE_NII_BASIC_STRING_ENABLE_SOURCE_ITER_ORDER_CHECK
					smoke_check_source_iterators_helper(typename mse::impl::HasOrInheritsLessThanOperator_msemsebasic_string<_Iter>::type(), _First, _Last);
		#endif // MSE_NII_BASIC_STRING_ENABLE_SOURCE_ITER_ORDER_CHECK
				}
				template<class _Iter>
				void smoke_check_source_iterators(const _Iter& _First, const _Iter& _Last) {
					smoke_check_source_iterators(*this, _First, _Last);
				}


				template<class ..._Valty>
				void emplace_back(_Valty&& ..._Val)
				{	// insert by moving into element at end
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().emplace_back(std::forward<_Valty>(_Val)...);
					/*m_debug_size = size();*/
				}
				void clear() {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().clear();
					/*m_debug_size = size();*/
				}

				void swap(_Myt& _Other) {	// swap contents with _Other
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().swap(_Other.contained_basic_string());
				}
				void swap(_MBS& _Other) {	// swap contents with _Other
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().swap(_Other);
				}
				template<typename _TStateMutex2, template<typename> class _TTXScopeConstIterator2>
				void swap(mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex2, _TTXScopeConstIterator2>& _Other) {	// swap contents with _Other
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().swap(_Other.contained_basic_string());
				}

				size_type length() const _NOEXCEPT
				{	// return length of sequence
					return contained_basic_string().length();
				}

				size_type size() const _NOEXCEPT
				{	// return length of sequence
					return contained_basic_string().size();
				}

				size_type max_size() const _NOEXCEPT
				{	// return maximum possible length of sequence
					return contained_basic_string().max_size();
				}

				bool empty() const _NOEXCEPT
				{	// test if sequence is empty
					return contained_basic_string().empty();
				}
				_A get_allocator() const _NOEXCEPT
				{	// return allocator object for values
					return contained_basic_string().get_allocator();
				}

				reference at(msev_size_t _Pos)
				{	// subscript mutable sequence with checking
					return contained_basic_string().at(msev_as_a_size_t(_Pos));
				}

				const_reference at(msev_size_t _Pos) const
				{	// subscript nonmutable sequence with checking
					return contained_basic_string().at(msev_as_a_size_t(_Pos));
				}

				gnii_basic_string(_XSTD initializer_list<typename std_basic_string::value_type> _Ilist, const _A& _Al = _A())
					: base_class(_Ilist, _Al) {	// construct from initializer_list
												/*m_debug_size = size();*/
				}
				_Myt& operator=(_XSTD initializer_list<typename std_basic_string::value_type> _Ilist) {	// assign initializer_list
					contained_basic_string().operator=(static_cast<std_basic_string>(_Ilist));
					return (*this);
				}
				void assign(_XSTD initializer_list<typename std_basic_string::value_type> _Ilist) {	// assign initializer_list
					contained_basic_string().assign(_Ilist);
					/*m_debug_size = size();*/
				}

				value_type *data() _NOEXCEPT
				{	// return pointer to mutable data basic_string
					return contained_basic_string().data();
				}

				const value_type *data() const _NOEXCEPT
				{	// return pointer to nonmutable data basic_string
					return contained_basic_string().data();
				}

				//class nbs_const_iterator_base : public std::iterator<std::random_access_iterator_tag, value_type, difference_type, const_pointer, const_reference> {};
				//class nbs_iterator_base : public std::iterator<std::random_access_iterator_tag, value_type, difference_type, pointer, reference> {};

				typedef mse::impl::random_access_const_iterator_base<_Ty> nbs_const_iterator_base;
				typedef mse::impl::random_access_iterator_base<_Ty> nbs_iterator_base;

				template<typename _TBasicStringConstPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_potentially_not_xscope<_TBasicStringConstPointer>::value)> MSE_IMPL_EIS >
				using Tss_const_iterator_type = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_ss_const_iterator_type<_TBasicStringConstPointer>;
				template<typename _TBasicStringPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_potentially_not_xscope<_TBasicStringPointer>::value)> MSE_IMPL_EIS >
				using Tss_iterator_type = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_ss_iterator_type<_TBasicStringPointer>;

				template<typename _TBasicStringPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_potentially_not_xscope<_TBasicStringPointer>::value)> MSE_IMPL_EIS >
				using Tss_reverse_iterator_type = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_ss_reverse_iterator_type<_TBasicStringPointer>;
				template<typename _TBasicStringConstPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_potentially_not_xscope<_TBasicStringConstPointer>::value)> MSE_IMPL_EIS >
				using Tss_const_reverse_iterator_type = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_ss_const_reverse_iterator_type<_TBasicStringConstPointer>;

				typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_rp_ss_iterator_type<_Myt> ss_iterator_type;
				typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_rp_ss_const_iterator_type<_Myt> ss_const_iterator_type;
				typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_rp_ss_reverse_iterator_type<_Myt> ss_reverse_iterator_type;
				typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_rp_ss_const_reverse_iterator_type<_Myt> ss_const_reverse_iterator_type;

			private:
				/* ss_iterator_type is bounds checked, but not safe against "use-after-free", so the member functions that
				involve ss_iterator_type are made publicly inaccessible. They are used by friend type us::mse_basic_string<>,
				which is in turn used by mstd::basic_string<>. */

				ss_iterator_type ss_begin() {	// return std_basic_string::iterator for beginning of mutable sequence
					ss_iterator_type retval(this);
					retval.set_to_beginning();
					return retval;
				}
				ss_const_iterator_type ss_begin() const {	// return std_basic_string::iterator for beginning of nonmutable sequence
					ss_const_iterator_type retval(this);
					retval.set_to_beginning();
					return retval;
				}
				ss_iterator_type ss_end() {	// return std_basic_string::iterator for end of mutable sequence
					ss_iterator_type retval(this);
					retval.set_to_end_marker();
					return retval;
				}
				ss_const_iterator_type ss_end() const {	// return std_basic_string::iterator for end of nonmutable sequence
					ss_const_iterator_type retval(this);
					retval.set_to_end_marker();
					return retval;
				}
				ss_const_iterator_type ss_cbegin() const {	// return std_basic_string::iterator for beginning of nonmutable sequence
					ss_const_iterator_type retval(this);
					retval.set_to_beginning();
					return retval;
				}
				ss_const_iterator_type ss_cend() const {	// return std_basic_string::iterator for end of nonmutable sequence
					ss_const_iterator_type retval(this);
					retval.set_to_end_marker();
					return retval;
				}

				ss_const_reverse_iterator_type ss_crbegin() const {	// return std_basic_string::iterator for beginning of reversed nonmutable sequence
					return (ss_rbegin());
				}
				ss_const_reverse_iterator_type ss_crend() const {	// return std_basic_string::iterator for end of reversed nonmutable sequence
					return (ss_rend());
				}
				ss_reverse_iterator_type ss_rbegin() {	// return std_basic_string::iterator for beginning of reversed mutable sequence
					return (reverse_iterator(ss_end()));
				}
				ss_const_reverse_iterator_type ss_rbegin() const {	// return std_basic_string::iterator for beginning of reversed nonmutable sequence
					return (const_reverse_iterator(ss_end()));
				}
				ss_reverse_iterator_type ss_rend() {	// return std_basic_string::iterator for end of reversed mutable sequence
					return (reverse_iterator(ss_begin()));
				}
				ss_const_reverse_iterator_type ss_rend() const {	// return std_basic_string::iterator for end of reversed nonmutable sequence
					return (const_reverse_iterator(ss_begin()));
				}

				gnii_basic_string(const ss_const_iterator_type &start, const ss_const_iterator_type &end, const _A& _Al = _A())
					: base_class(_Al) {
					/*m_debug_size = size();*/
					assign(start, end);
				}
				void assign(const ss_const_iterator_type &start, const ss_const_iterator_type &end) {
					if (start.m_owner_cptr != end.m_owner_cptr) { MSE_THROW(gnii_basic_string_range_error("invalid arguments - void assign(const ss_const_iterator_type &start, const ss_const_iterator_type &end) - gnii_basic_string")); }
					if (start > end) { MSE_THROW(gnii_basic_string_range_error("invalid arguments - void assign(const ss_const_iterator_type &start, const ss_const_iterator_type &end) - gnii_basic_string")); }
					typename std_basic_string::const_iterator _F = start;
					typename std_basic_string::const_iterator _L = end;
					(*this).assign(_F, _L);
				}
				void assign_inclusive(const ss_const_iterator_type &first, const ss_const_iterator_type &last) {
					auto end = last;
					end++; // this should include some checks
					(*this).assign(first, end);
				}
				ss_iterator_type insert_before(const ss_const_iterator_type &pos, size_type _M, _Ty _X) {
					if (pos.m_owner_cptr != this) { MSE_THROW(gnii_basic_string_range_error("invalid argument - void insert_before() - gnii_basic_string")); }
					pos.assert_valid_index();
					msev_size_t original_pos = pos.position();
					typename std_basic_string::const_iterator _P = pos.target_container_ptr()->contained_basic_string().cbegin() + pos.position();
					(*this).insert(_P, _M, _X);
					ss_iterator_type retval = ss_begin();
					retval.advance(msev_int(original_pos));
					return retval;
				}
				ss_iterator_type insert_before(const ss_const_iterator_type &pos, _Ty _X) { return (*this).insert(pos, 1, _X); }
				template<class _Iter
					//>mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value, typename std_basic_string::iterator>
					, class = mse::impl::_mse_RequireInputIter<_Iter> >
				ss_iterator_type insert_before(const ss_const_iterator_type &pos, const _Iter &start, const _Iter &end) {
					if (pos.m_owner_cptr != this) { MSE_THROW(gnii_basic_string_range_error("invalid argument - ss_iterator_type insert_before() - gnii_basic_string")); }
					//if (start.m_owner_cptr != end.m_owner_cptr) { MSE_THROW(gnii_basic_string_range_error("invalid arguments - void insert_before(const ss_const_iterator_type &pos, const ss_const_iterator_type &start, const ss_const_iterator_type &end) - gnii_basic_string")); }
					pos.assert_valid_index();
					msev_size_t original_pos = pos.position();
					typename std_basic_string::const_iterator _P = pos.target_container_ptr()->contained_basic_string().cbegin() + pos.position();
					(*this).insert(_P, start, end);
					ss_iterator_type retval = ss_begin();
					retval.advance(msev_int(original_pos));
					return retval;
				}
				ss_iterator_type insert_before(const ss_const_iterator_type &pos, const ss_const_iterator_type& start, const ss_const_iterator_type &end) {
					if (start.m_owner_cptr != end.m_owner_cptr) { MSE_THROW(gnii_basic_string_range_error("invalid arguments - void insert_before(const ss_const_iterator_type &pos, const ss_const_iterator_type &start, const ss_const_iterator_type &end) - gnii_basic_string")); }
					end.assert_valid_index();
					if (start > end) { MSE_THROW(gnii_basic_string_range_error("invalid arguments - void insert_before(const ss_const_iterator_type &pos, const ss_const_iterator_type &start, const ss_const_iterator_type &end) - gnii_basic_string")); }
					typename std_basic_string::const_iterator _S = start;
					typename std_basic_string::const_iterator _E = end;
					return (*this).insert_before(pos, _S, _E);
				}
				ss_iterator_type insert_before(const ss_const_iterator_type &pos, const _Ty* start, const _Ty* end) {
					if (pos.m_owner_cptr != this) { MSE_THROW(gnii_basic_string_range_error("invalid arguments - ss_iterator_type insert_before() - gnii_basic_string")); }
					//if (start.m_owner_cptr != end.m_owner_cptr) { MSE_THROW(gnii_basic_string_range_error("invalid arguments - void insert_before(const ss_const_iterator_type &pos, const ss_const_iterator_type &start, const ss_const_iterator_type &end) - gnii_basic_string")); }
					if (start > end) { MSE_THROW(gnii_basic_string_range_error("invalid arguments - ss_iterator_type insert_before() - gnii_basic_string")); }
					pos.assert_valid_index();
					msev_size_t original_pos = pos.position();
					typename std_basic_string::const_iterator _P = pos.target_container_ptr()->contained_basic_string().cbegin() + pos.position();
					(*this).insert(_P, start, end);
					ss_iterator_type retval = ss_begin();
					retval.advance(msev_int(original_pos));
					return retval;
				}
				template<class _Iter
					//>mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value, typename std_basic_string::iterator>
					, class = mse::impl::_mse_RequireInputIter<_Iter> >
					ss_iterator_type insert_before_inclusive(const ss_iterator_type &pos, const _Iter &first, const _Iter &last) {
					auto end = last;
					end++; // this may include some checks
					return (*this).insert_before(pos, first, end);
				}
				ss_iterator_type insert_before(const ss_const_iterator_type &pos, _XSTD initializer_list<typename std_basic_string::value_type> _Ilist) {	// insert initializer_list
					if (pos.m_owner_ptr != this) { MSE_THROW(gnii_basic_string_range_error("invalid arguments - void insert_before() - gnii_basic_string")); }
					pos.assert_valid_index();
					msev_size_t original_pos = pos.position();
					typename std_basic_string::const_iterator _P = pos.target_container_ptr()->contained_basic_string().cbegin() + pos.position();
					(*this).insert(_P, _Ilist);
					ss_iterator_type retval = ss_begin();
					retval.advance(msev_int(original_pos));
					return retval;
				}
				template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
				ss_iterator_type insert_before(const ss_const_iterator_type &pos, const _TStringSection& _X) {
					if (pos.m_owner_ptr != this) { MSE_THROW(gnii_basic_string_range_error("invalid arguments - void insert_before() - gnii_basic_string")); }
					pos.assert_valid_index();
					msev_size_t original_pos = pos.position();
					typename std_basic_string::const_iterator _P = pos.target_container_ptr()->contained_basic_string().cbegin() + pos.position();
					(*this).insert(_P, _X);
					ss_iterator_type retval = ss_begin();
					retval.advance(msev_int(original_pos));
					return retval;
				}

				/* These insert() functions are just aliases for their corresponding insert_before() functions. */
				/*
				ss_iterator_type insert(const ss_const_iterator_type &pos, size_type _M, const _Ty& _X) { return insert_before(pos, _M, _X); }
				ss_iterator_type insert(const ss_const_iterator_type &pos, _Ty&& _X) { return insert_before(pos, MSE_FWD(_X)); }
				ss_iterator_type insert(const ss_const_iterator_type &pos, const _Ty& _X = _Ty()) { return insert_before(pos, _X); }
				template<class _Iter
				//>mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value, typename std_basic_string::iterator>
				, class = mse::impl::_mse_RequireInputIter<_Iter> >
				ss_iterator_type insert(const ss_const_iterator_type &pos, const _Iter &start, const _Iter &end) { return insert_before(pos, start, end); }
				ss_iterator_type insert(const ss_const_iterator_type &pos, const _Ty* start, const _Ty* &end) { return insert_before(pos, start, end); }
				ss_iterator_type insert(const ss_const_iterator_type &pos, _XSTD initializer_list<typename std_basic_string::value_type> _Ilist) { return insert_before(pos, _Ilist); }
				*/
				/*
				template<class ..._Valty>
				ss_iterator_type emplace(const ss_const_iterator_type &pos, _Valty&& ..._Val)
				{	// insert by moving _Val at pos
				if (pos.m_owner_cptr != this) { MSE_THROW(gnii_basic_string_range_error("invalid arguments - void emplace() - gnii_basic_string")); }
				pos.assert_valid_index();
				msev_size_t original_pos = pos.position();
				typename std_basic_string::const_iterator _P = pos.target_container_ptr()->contained_basic_string().cbegin() + pos.position();
				(*this).emplace(_P, std::forward<_Valty>(_Val)...);
				ss_iterator_type retval = ss_begin();
				retval.advance(msev_int(original_pos));
				return retval;
				}
				ss_iterator_type erase(const ss_const_iterator_type &pos) {
				if (pos.m_owner_cptr != this) { MSE_THROW(gnii_basic_string_range_error("invalid arguments - void erase() - gnii_basic_string")); }
				if (!pos.points_to_an_item()) { MSE_THROW(gnii_basic_string_range_error("invalid arguments - void erase() - gnii_basic_string")); }
				auto pos_index = pos.position();

				typename std_basic_string::const_iterator _P = pos.target_container_ptr()->contained_basic_string().cbegin() + pos.position();
				(*this).erase(_P);

				ss_iterator_type retval = (*this).ss_begin();
				retval.advance(typename ss_const_iterator_type::difference_type(pos_index));
				return retval;
				}
				ss_iterator_type erase(const ss_const_iterator_type &start, const ss_const_iterator_type &end) {
				if (start.m_owner_cptr != this) { MSE_THROW(gnii_basic_string_range_error("invalid arguments - void erase() - gnii_basic_string")); }
				if (end.m_owner_cptr != this) { MSE_THROW(gnii_basic_string_range_error("invalid arguments - void erase() - gnii_basic_string")); }
				if (start.position() > end.position()) { MSE_THROW(gnii_basic_string_range_error("invalid arguments - void erase() - gnii_basic_string")); }
				auto pos_index = start.position();

				typename std_basic_string::const_iterator _F = start;
				typename std_basic_string::const_iterator _L = end;
				(*this).erase(_F, _L);

				ss_iterator_type retval = (*this).ss_begin();
				retval.advance(typename ss_const_iterator_type::difference_type(pos_index));
				return retval;
				}
				*/
				ss_iterator_type erase_inclusive(const ss_const_iterator_type &first, const ss_const_iterator_type &last) {
					auto end = last; end.set_to_next();
					return erase(first, end);
				}
				void erase_previous_item(const ss_const_iterator_type &pos) {
					if (pos.m_owner_cptr != this) { MSE_THROW(gnii_basic_string_range_error("invalid arguments - void erase_previous_item() - gnii_basic_string")); }
					if (!(pos.has_previous())) { MSE_THROW(gnii_basic_string_range_error("invalid arguments - void erase_previous_item() - gnii_basic_string")); }
					typename std_basic_string::const_iterator _P = pos.target_container_ptr()->contained_basic_string().cbegin() + pos.position();
					_P--;
					(*this).erase(_P);
				}

			public:
				/* Here we provide static versions of the member functions that return iterators. As static member functions do
				not have access to a "this" pointer, these functions require a pointer to the container to be passed as the first
				argument. Any returned iterator will contain a copy of the supplied pointer and inherit its safety properties. */

				template<typename _TBasicStringPointer>
				static auto ss_begin(const _TBasicStringPointer& owner_ptr) {
					mse::impl::T_valid_if_not_an_xscope_type<_TBasicStringPointer>();
					typedef mse::impl::conditional_t<std::is_const<mse::impl::remove_reference_t<decltype(*owner_ptr)> >::value
						, Tss_const_iterator_type<_TBasicStringPointer>, Tss_iterator_type<_TBasicStringPointer> > return_type;
					return_type retval(owner_ptr);
					retval.set_to_beginning();
					return retval;
				}

				template<typename _TBasicStringPointer>
				static auto ss_end(const _TBasicStringPointer& owner_ptr) {
					mse::impl::T_valid_if_not_an_xscope_type<_TBasicStringPointer>();
					typedef mse::impl::conditional_t<std::is_const<mse::impl::remove_reference_t<decltype(*owner_ptr)> >::value
						, Tss_const_iterator_type<_TBasicStringPointer>, Tss_iterator_type<_TBasicStringPointer> > return_type;
					return_type retval(owner_ptr);
					retval.set_to_end_marker();
					return retval;
				}

				template<typename _TBasicStringPointer>
				static auto ss_cbegin(const _TBasicStringPointer& owner_ptr) {
					mse::impl::T_valid_if_not_an_xscope_type<_TBasicStringPointer>();
					Tss_const_iterator_type<_TBasicStringPointer> retval(owner_ptr);
					retval.set_to_beginning();
					return retval;
				}

				template<typename _TBasicStringPointer>
				static auto ss_cend(const _TBasicStringPointer& owner_ptr) {
					mse::impl::T_valid_if_not_an_xscope_type<_TBasicStringPointer>();
					Tss_const_iterator_type<_TBasicStringPointer> retval(owner_ptr);
					retval.set_to_end_marker();
					return retval;
				}

				template<typename _TBasicStringPointer>
				static auto ss_rbegin(const _TBasicStringPointer& owner_ptr) {
					mse::impl::T_valid_if_not_an_xscope_type<_TBasicStringPointer>();
					typedef mse::impl::conditional_t<std::is_const<mse::impl::remove_reference_t<decltype(*owner_ptr)> >::value
						, Tss_const_reverse_iterator_type<_TBasicStringPointer>, Tss_reverse_iterator_type<_TBasicStringPointer> > return_type;
					return return_type(ss_end<_TBasicStringPointer>(owner_ptr));
				}

				template<typename _TBasicStringPointer>
				static auto ss_rend(const _TBasicStringPointer& owner_ptr) {
					mse::impl::T_valid_if_not_an_xscope_type<_TBasicStringPointer>();
					typedef mse::impl::conditional_t<std::is_const<mse::impl::remove_reference_t<decltype(*owner_ptr)> >::value
						, Tss_const_reverse_iterator_type<_TBasicStringPointer>, Tss_reverse_iterator_type<_TBasicStringPointer> > return_type;
					return return_type(ss_begin<_TBasicStringPointer>(owner_ptr));
				}

				template<typename _TBasicStringPointer>
				static auto ss_crbegin(const _TBasicStringPointer& owner_ptr) {
					mse::impl::T_valid_if_not_an_xscope_type<_TBasicStringPointer>();
					return (Tss_const_reverse_iterator_type<_TBasicStringPointer>(ss_cend<_TBasicStringPointer>(owner_ptr)));
				}

				template<typename _TBasicStringPointer>
				static auto ss_crend(const _TBasicStringPointer& owner_ptr) {
					mse::impl::T_valid_if_not_an_xscope_type<_TBasicStringPointer>();
					return (Tss_const_reverse_iterator_type<_TBasicStringPointer>(ss_crbegin<_TBasicStringPointer>(owner_ptr)));
				}

				template<typename _TBasicStringPointer1>
				static void s_assert_valid_index(const _TBasicStringPointer1& this_ptr, size_type index) {
					if ((*this_ptr).size() < index) { MSE_THROW(gnii_basic_string_range_error("invalid index - void assert_valid_index() const - gnii_basic_string")); }
				}

				template<typename _TBasicStringPointer1>
				static auto insert(_TBasicStringPointer1 this_ptr, size_type pos, _Ty _X) {
					s_assert_valid_index(this_ptr, pos);
					msev_size_t original_pos = pos;
					typename std_basic_string::const_iterator _P = (*this_ptr).contained_basic_string().cbegin() + difference_type(pos);
					(*this_ptr).insert(_P, _X);
					auto retval = mse::make_begin_iterator(this_ptr);
					retval.advance(msev_int(original_pos));
					return retval;
				}
				template<typename _TBasicStringPointer1>
				static auto insert(_TBasicStringPointer1 this_ptr, size_type pos, size_type _M, _Ty _X) {
					s_assert_valid_index(this_ptr, pos);
					msev_size_t original_pos = pos;
					typename std_basic_string::const_iterator _P = (*this_ptr).contained_basic_string().cbegin() + difference_type(pos);
					(*this_ptr).insert(_P, _M, _X);
					auto retval = mse::make_begin_iterator(this_ptr);
					retval.advance(msev_int(original_pos));
					return retval;
				}
				template<typename _TBasicStringPointer1, class _Iter, class = mse::impl::_mse_RequireInputIter<_Iter> >
				static auto insert(_TBasicStringPointer1 this_ptr, size_type pos, const _Iter& _First, const _Iter& _Last) {
					s_assert_valid_index(this_ptr, pos);
					msev_size_t original_pos = pos;
					typename std_basic_string::const_iterator _P = (*this_ptr).contained_basic_string().cbegin() + difference_type(pos);
					(*this_ptr).insert(_P, _First, _Last);
					auto retval = mse::make_begin_iterator(this_ptr);
					retval.advance(msev_int(original_pos));
					return retval;
				}
				template<typename _TBasicStringPointer1>
				static auto& insert(_TBasicStringPointer1 this_ptr, size_type pos, const _Ty* const s, size_type count) {
					s_assert_valid_index(this_ptr, pos);
					msev_size_t original_pos = pos;
					typename std_basic_string::const_iterator _P = (*this_ptr).contained_basic_string().cbegin() + difference_type(pos);
					(*this_ptr).insert(_P, s, count);
					auto retval = mse::make_begin_iterator(this_ptr);
					retval.advance(msev_int(original_pos));
					return retval;
				}
				template<typename _TBasicStringPointer1>
				static auto insert(_TBasicStringPointer1 this_ptr, size_type pos, _XSTD initializer_list<typename std_basic_string::value_type> _Ilist) {
					s_assert_valid_index(this_ptr, pos);
					msev_size_t original_pos = pos;
					typename std_basic_string::const_iterator _P = (*this_ptr).contained_basic_string().cbegin() + difference_type(pos);
					(*this_ptr).insert(_P, _Ilist);
					auto retval = mse::make_begin_iterator(this_ptr);
					retval.advance(msev_int(original_pos));
					return retval;
				}
				template<typename _TBasicStringPointer1, typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
				static auto& insert(_TBasicStringPointer1 this_ptr, size_type pos, const _TStringSection& _X) {
					s_assert_valid_index(this_ptr, pos);
					msev_size_t original_pos = pos;
					typename std_basic_string::const_iterator _P = (*this_ptr).contained_basic_string().cbegin() + difference_type(pos);
					(*this_ptr).insert(_P, _X);
					auto retval = mse::make_begin_iterator(this_ptr);
					retval.advance(msev_int(original_pos));
					return retval;
				}
				template<typename _TBasicStringPointer1, class ..._Valty>
				static auto emplace(_TBasicStringPointer1 this_ptr, size_type pos, _Valty&& ..._Val)
				{	// insert by moving _Val at _Where
					s_assert_valid_index(this_ptr, pos);
					msev_size_t original_pos = pos;
					typename std_basic_string::const_iterator _P = (*this_ptr).contained_basic_string().cbegin() + difference_type(pos);
					(*this_ptr).emplace(_P, std::forward<_Valty>(_Val)...);
					auto retval = mse::make_begin_iterator(this_ptr);
					retval.advance(msev_int(original_pos));
					return retval;
				}
				template<typename _TBasicStringPointer1>
				static auto erase(_TBasicStringPointer1 this_ptr, size_type pos) {
					s_assert_valid_index(this_ptr, pos);
					auto pos_index = pos;

					typename std_basic_string::const_iterator _P = (*this_ptr).contained_basic_string().cbegin() + difference_type(pos);
					(*this_ptr).erase(_P);

					auto retval = mse::make_begin_iterator(this_ptr);
					retval.advance(msev_int(pos_index));
					return retval;
				}
				template<typename _TBasicStringPointer1>
				static auto erase(_TBasicStringPointer1 this_ptr, size_type start, size_type end) {
					if (start > end) { MSE_THROW(gnii_basic_string_range_error("invalid arguments - void erase() - gnii_basic_string")); }
					auto pos_index = start;

					typename std_basic_string::const_iterator _F = (*this_ptr).contained_basic_string().cbegin() + difference_type(start);
					typename std_basic_string::const_iterator _L = (*this_ptr).contained_basic_string().cbegin() + difference_type(end);
					(*this_ptr).erase(_F, _L);

					auto retval = mse::make_begin_iterator(this_ptr);
					retval.advance(msev_int(pos_index));
					return retval;
				}

				template<typename _TBasicStringPointer1, typename _TBasicStringPointer2>
				static auto insert(_TBasicStringPointer1 this_ptr, const Tss_const_iterator_type<_TBasicStringPointer2>& pos, _Ty _X) {
					auto original_pos = pos;
					insert(this_ptr, pos.position(), _X);
					auto retval = mse::make_begin_iterator(this_ptr);
					retval.advance(msev_int(original_pos));
					return retval;
				}
				template<typename _TBasicStringPointer1, typename _TBasicStringPointer2>
				static auto insert(_TBasicStringPointer1 this_ptr, const Tss_const_iterator_type<_TBasicStringPointer2>& pos, size_type _M, _Ty _X) {
					return insert(this_ptr, pos.position(), _M, _X);
				}
				template<typename _TBasicStringPointer1, typename _TBasicStringPointer2, class _Iter, class = mse::impl::_mse_RequireInputIter<_Iter> >
				static auto insert(_TBasicStringPointer1 this_ptr, const Tss_const_iterator_type<_TBasicStringPointer2>& pos, const _Iter& _First, const _Iter& _Last) {
					return insert(this_ptr, pos.position(), _First, _Last);
				}
				template<typename _TBasicStringPointer1, typename _TBasicStringPointer2>
				static auto insert(_TBasicStringPointer1 this_ptr, const Tss_const_iterator_type<_TBasicStringPointer2>& pos, _XSTD initializer_list<typename std_basic_string::value_type> _Ilist) {
					return insert(this_ptr, pos.position(), _Ilist);
				}
				template<typename _TBasicStringPointer1, typename _TBasicStringPointer2, typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
				static auto insert(_TBasicStringPointer1 this_ptr, const Tss_const_iterator_type<_TBasicStringPointer2>& pos, const _TStringSection& _X) {
					return insert(this_ptr, pos.position(), _X);
				}

				template<typename _TBasicStringPointer1, typename _TBasicStringPointer2, class ..._Valty>
				static auto emplace(_TBasicStringPointer1 this_ptr, const Tss_const_iterator_type<_TBasicStringPointer2>& pos, _Valty&& ..._Val) {
					return emplace(this_ptr, pos.position(), std::forward<_Valty>(_Val)...);
				}
				template<typename _TBasicStringPointer1, typename _TBasicStringPointer2>
				static auto erase(_TBasicStringPointer1 this_ptr, const Tss_const_iterator_type<_TBasicStringPointer2>& pos) {
					return erase(this_ptr, pos.position());
				}
				template<typename _TBasicStringPointer1, typename _TBasicStringPointer2>
				static auto erase(_TBasicStringPointer1 this_ptr, const Tss_const_iterator_type<_TBasicStringPointer2>& start, const Tss_const_iterator_type<_TBasicStringPointer2>& end) {
					return erase(this_ptr, start.position(), end.position());
				}

				template<typename _TBasicStringPointer1, typename _TBasicStringPointer2>
				static auto insert(_TBasicStringPointer1 this_ptr, const Tss_iterator_type<_TBasicStringPointer2>& pos, _Ty&& _X) {
					return insert(this_ptr, Tss_const_iterator_type<_TBasicStringPointer2>(pos), MSE_FWD(_X));
				}
				template<typename _TBasicStringPointer1, typename _TBasicStringPointer2>
				static auto insert(_TBasicStringPointer1 this_ptr, const Tss_iterator_type<_TBasicStringPointer2>& pos, const _Ty& _X = _Ty()) {
					return insert(this_ptr, Tss_const_iterator_type<_TBasicStringPointer2>(pos), _X);
				}
				template<typename _TBasicStringPointer1, typename _TBasicStringPointer2>
				static auto insert(_TBasicStringPointer1 this_ptr, const Tss_iterator_type<_TBasicStringPointer2>& pos, size_type _M, const _Ty& _X) {
					return insert(this_ptr, Tss_const_iterator_type<_TBasicStringPointer2>(pos), _M, _X);
				}
				template<typename _TBasicStringPointer1, typename _TBasicStringPointer2, class _Iter, class = mse::impl::_mse_RequireInputIter<_Iter> >
				static auto insert(_TBasicStringPointer1 this_ptr, const Tss_iterator_type<_TBasicStringPointer2>& pos, const _Iter& _First, const _Iter& _Last) {
					return insert(this_ptr, Tss_const_iterator_type<_TBasicStringPointer2>(pos), _First, _Last);
				}
				template<typename _TBasicStringPointer1, typename _TBasicStringPointer2>
				static auto insert(_TBasicStringPointer1 this_ptr, const Tss_iterator_type<_TBasicStringPointer2>& pos, _XSTD initializer_list<typename std_basic_string::value_type> _Ilist) {
					return insert(this_ptr, Tss_const_iterator_type<_TBasicStringPointer2>(pos), _Ilist);
				}
				template<typename _TBasicStringPointer1, typename _TBasicStringPointer2, class ..._Valty>
				static auto emplace(_TBasicStringPointer1 this_ptr, const Tss_iterator_type<_TBasicStringPointer2>& pos, _Valty&& ..._Val) {
					return emplace(this_ptr, Tss_const_iterator_type<_TBasicStringPointer2>(pos), std::forward<_Valty>(_Val)...);
				}
				template<typename _TBasicStringPointer1, typename _TBasicStringPointer2>
				static auto erase(_TBasicStringPointer1 this_ptr, const Tss_iterator_type<_TBasicStringPointer2>& pos) {
					return erase(this_ptr, Tss_const_iterator_type<_TBasicStringPointer2>(pos));
				}
				template<typename _TBasicStringPointer1, typename _TBasicStringPointer2>
				static auto erase(_TBasicStringPointer1 this_ptr, const Tss_iterator_type<_TBasicStringPointer2>& start, const Tss_iterator_type<_TBasicStringPointer2>& end) {
					return erase(this_ptr, Tss_const_iterator_type<_TBasicStringPointer2>(start), Tss_const_iterator_type<_TBasicStringPointer2>(end));
				}


				typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_ss_const_iterator_type<_Myt> xscope_ss_const_iterator_type;
				typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_ss_iterator_type<_Myt> xscope_ss_iterator_type;

				//typedef xscope_ss_const_iterator_type xscope_const_iterator;
				typedef _TTXScopeConstIterator<_Myt> xscope_const_iterator;
				//typedef xscope_ss_iterator_type xscope_iterator;
				typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_cslsstrong_iterator_type<_Myt> xscope_iterator;


				template<typename _TBasicStringPointer1>
				static auto insert(_TBasicStringPointer1 this_ptr, const xscope_const_iterator& pos, _Ty _X) {
					return insert(this_ptr, pos.position(), _X);
				}
				template<typename _TBasicStringPointer1>
				static auto insert(_TBasicStringPointer1 this_ptr, const xscope_const_iterator& pos, size_type _M, _Ty _X) {
					return insert(this_ptr, pos.position(), _M, _X);
				}
				template<typename _TBasicStringPointer1, class _Iter, class = mse::impl::_mse_RequireInputIter<_Iter> >
				static auto insert(_TBasicStringPointer1 this_ptr, const xscope_const_iterator& pos, const _Iter& _First, const _Iter& _Last) {
					return insert(this_ptr, pos.position(), _First, _Last);
				}
				template<typename _TBasicStringPointer1>
				static auto insert(_TBasicStringPointer1 this_ptr, const xscope_const_iterator& pos, _XSTD initializer_list<typename std_basic_string::value_type> _Ilist) {
					return insert(this_ptr, pos.position(), _Ilist);
				}
				template<typename _TBasicStringPointer1, typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
				static auto insert(_TBasicStringPointer1 this_ptr, const xscope_const_iterator& pos, const _TStringSection& _X) {
					return insert(this_ptr, pos.position(), _X);
				}

				template<typename _TBasicStringPointer1, class ..._Valty>
				static auto emplace(_TBasicStringPointer1 this_ptr, const xscope_const_iterator& pos, _Valty&& ..._Val) {
					return emplace(this_ptr, pos.position(), std::forward<_Valty>(_Val)...);
				}
				template<typename _TBasicStringPointer1>
				static auto erase(_TBasicStringPointer1 this_ptr, const xscope_const_iterator& pos) {
					return erase(this_ptr, pos.position());
				}
				template<typename _TBasicStringPointer1>
				static auto erase(_TBasicStringPointer1 this_ptr, const xscope_const_iterator& start, const xscope_const_iterator& end) {
					return erase(this_ptr, start.position(), end.position());
				}

				template<typename _TBasicStringPointer1>
				static auto insert(_TBasicStringPointer1 this_ptr, const xscope_iterator& pos, _Ty&& _X) {
					return insert(this_ptr, xscope_const_iterator(pos), MSE_FWD(_X));
				}
				template<typename _TBasicStringPointer1>
				static auto insert(_TBasicStringPointer1 this_ptr, const xscope_iterator& pos, const _Ty& _X = _Ty()) {
					return insert(this_ptr, xscope_const_iterator(pos), _X);
				}
				template<typename _TBasicStringPointer1>
				static auto insert(_TBasicStringPointer1 this_ptr, const xscope_iterator& pos, size_type _M, const _Ty& _X) {
					return insert(this_ptr, xscope_const_iterator(pos), _M, _X);
				}
				template<typename _TBasicStringPointer1, class _Iter, class = mse::impl::_mse_RequireInputIter<_Iter> >
					static auto insert(_TBasicStringPointer1 this_ptr, const xscope_iterator& pos, const _Iter& _First, const _Iter& _Last) {
					return insert(this_ptr, xscope_const_iterator(pos), _First, _Last);
				}
				template<typename _TBasicStringPointer1>
				static auto insert(_TBasicStringPointer1 this_ptr, const xscope_iterator& pos, _XSTD initializer_list<typename std_basic_string::value_type> _Ilist) {
					return insert(this_ptr, xscope_const_iterator(pos), _Ilist);
				}
				template<typename _TBasicStringPointer1, class ..._Valty>
				static auto emplace(_TBasicStringPointer1 this_ptr, const xscope_iterator& pos, _Valty&& ..._Val) {
					return emplace(this_ptr, xscope_const_iterator(pos), std::forward<_Valty>(_Val)...);
				}
				template<typename _TBasicStringPointer1>
				static auto erase(_TBasicStringPointer1 this_ptr, const xscope_iterator& pos) {
					return erase(this_ptr, xscope_const_iterator(pos));
				}
				template<typename _TBasicStringPointer1>
				static auto erase(_TBasicStringPointer1 this_ptr, const xscope_iterator& start, const xscope_iterator& end) {
					return erase(this_ptr, xscope_const_iterator(start), xscope_const_iterator(end));
				}


				bool operator==(const _Myt& _Right) const {	// test for basic_string equality
					return (_Right.contained_basic_string() == contained_basic_string());
				}
				bool operator<(const _Myt& _Right) const {	// test if _Left < _Right for basic_strings
					return (contained_basic_string() < _Right.contained_basic_string());
				}


				gnii_basic_string& append(mse::TXScopeFixedConstPointer<gnii_basic_string> xs_ptr) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().append(xs_ptr->contained_basic_string());
					/*m_debug_size = size();*/
					return (*this);
				}
				gnii_basic_string& append(const gnii_basic_string& _Right) {	// append _Right
					auto xs_ptr = mse::us::unsafe_make_xscope_const_pointer_to(_Right);
					return append(xs_ptr);
				}
				gnii_basic_string& append(mse::TXScopeFixedConstPointer<gnii_basic_string> xs_ptr, const size_type _Roff, size_type _Count = npos) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().append(xs_ptr->contained_basic_string(), mse::as_a_size_t(_Roff), mse::as_a_size_t(_Count));
					/*m_debug_size = size();*/
					return (*this);
				}
				gnii_basic_string& append(const gnii_basic_string& _Right, const size_type _Roff, size_type _Count = npos) {
					auto xs_ptr = mse::us::unsafe_make_xscope_const_pointer_to(_Right);
					return append(xs_ptr, _Roff, _Count);
				}

				gnii_basic_string& append(const _Ty * const _Ptr, const size_type _Count) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().append(_Ptr, mse::as_a_size_t(_Count));
					//m_debug_size = size();
					return (*this);
				}
				gnii_basic_string& append(const _Ty * const _Ptr) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().append(_Ptr);
					//m_debug_size = size();
					return (*this);
				}
				gnii_basic_string& append(const size_type _Count, const _Ty& _Ch) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().append(mse::as_a_size_t(_Count), _Ch);
					//m_debug_size = size();
					return (*this);
				}
				template<class _Iter, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::_mse_Is_iterator_v<_Iter> > MSE_IMPL_EIS >
				gnii_basic_string& append(const _Iter _First, const _Iter _Last) {
					smoke_check_source_iterators(_First, _Last);
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().append(_First, _Last);
					//m_debug_size = size();
					return (*this);
				}
				gnii_basic_string& append(std::initializer_list<_Ty> _Ilist) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().append(_Ilist);
					return (*this);
				}

		#ifdef MSE_HAS_CXX17
			private:
				template<class _StringViewIsh>
				gnii_basic_string& append_helper2(std::true_type, const _StringViewIsh& _Right) {
					std::basic_string_view<_Ty, _Traits> _As_view = _Right;
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().append(_As_view);
					/*m_debug_size = size();*/
					return (*this);
				}
				template<class _TParam1>
				gnii_basic_string& append_helper2(std::false_type, const _TParam1& _Right) {
					const auto xs_iters = mse::impl::make_xscope_range_iter_provider(_Right);
					return (append(xs_iters.begin(), xs_iters.end()));
				}
				gnii_basic_string& append_helper1(std::true_type, const gnii_basic_string& _Right) {
					append(_Right);
					return (*this);
				}
				template<class _TParam1>
				gnii_basic_string& append_helper1(std::false_type, const _TParam1& _Right) {
					return append_helper2(typename _is_string_view_ish<_TParam1>::type(), _Right);
				}
			public:
				template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
				gnii_basic_string& append(const _TParam1& _Right) {
					return append_helper1(typename std::is_base_of<gnii_basic_string, _TParam1>::type(), _Right);
				}

			private:
				template<class _StringViewIsh>
				gnii_basic_string& append_helper2(std::true_type, const _StringViewIsh& _Right, const size_type _Roff, const size_type _Count) {
					std::basic_string_view<_Ty, _Traits> _As_view = _Right;
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().append(_As_view, mse::as_a_size_t(_Roff), mse::as_a_size_t(_Count));
					/*m_debug_size = size();*/
					return (*this);
				}
				template<class _TParam1>
				gnii_basic_string& append_helper2(std::false_type, const _TParam1& _Right, const size_type _Roff, const size_type _Count) {
					auto adjusted_count = (npos == _Count) ? (difference_type(mse::container_size(_Right)) - difference_type(_Roff))
						: difference_type(_Count);
					if (0 > adjusted_count) { MSE_THROW(std::out_of_range(" gnii_basic_string::append() ")); }
					const auto xs_iters = mse::impl::make_xscope_range_iter_provider(_Right);
					return append(xs_iters.begin() + difference_type(_Roff), xs_iters.begin() + difference_type(_Roff + adjusted_count));
				}
				gnii_basic_string& append_helper1(std::true_type, const gnii_basic_string& _Right, const size_type _Roff, const size_type _Count) {
					append(_Right, _Roff, _Count);
					return (*this);
				}
				template<class _TParam1>
				gnii_basic_string& append_helper1(std::false_type, const _TParam1& _Right, const size_type _Roff, const size_type _Count) {
					return append_helper2(typename _is_string_view_ish<_TParam1>::type(), _Right, _Roff, _Count);
				}
			public:
				template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
				gnii_basic_string& append(const _TParam1& _Right, const size_type _Roff, const size_type _Count = npos) {
					return append_helper1(typename std::is_base_of<gnii_basic_string, _TParam1>::type(), _Right, _Roff, _Count);
				}
		#else /* MSE_HAS_CXX17 */
				template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
				gnii_basic_string& append(const _TStringSection& _X) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().append(_X.cbegin(), _X.cend());
					/*m_debug_size = size();*/
					return (*this);
				}
		#endif /* MSE_HAS_CXX17 */

				gnii_basic_string& operator+=(mse::TXScopeFixedConstPointer<gnii_basic_string> xs_ptr) {
					return (append(xs_ptr));
				}
				gnii_basic_string& operator+=(const gnii_basic_string& _Right) {
					return (append(_Right));
				}
				gnii_basic_string& operator+=(const _Ty * const _Ptr) {
					return (append(_Ptr));
				}
				gnii_basic_string& operator+=(_Ty _Ch) {
					push_back(_Ch);
					return (*this);
				}
				gnii_basic_string& operator+=(std::initializer_list<_Ty> _Ilist) {
					return (append(_Ilist));
				}

		#ifdef MSE_HAS_CXX17
				template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1>*/>
				gnii_basic_string & operator+=(const _TParam1& _Right) {
					return (append(_Right));
				}
		#else /* MSE_HAS_CXX17 */
				template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
				gnii_basic_string& operator+=(const _TStringSection& _X) {
					return (append(_X));
				}
		#endif /* MSE_HAS_CXX17 */

				gnii_basic_string& replace(const size_type _Off, const size_type _N0, mse::TXScopeFixedConstPointer<gnii_basic_string> xs_ptr) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().replace(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), (*xs_ptr).contained_basic_string());
					return (*this);
				}
				gnii_basic_string& replace(const size_type _Off, const size_type _N0, const gnii_basic_string& _Right) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().replace(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _Right.contained_basic_string());
					return (*this);
				}

				gnii_basic_string& replace(const size_type _Off, size_type _N0,
					mse::TXScopeFixedConstPointer<gnii_basic_string> xs_ptr, const size_type _Roff, size_type _Count = npos) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().replace(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), (*xs_ptr).contained_basic_string(), mse::msev_as_a_size_t(_Roff), mse::msev_as_a_size_t(_Count));
					return (*this);
				}
				gnii_basic_string& replace(const size_type _Off, size_type _N0,
					const gnii_basic_string& _Right, const size_type _Roff, size_type _Count = npos) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().replace(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _Right.contained_basic_string(), mse::msev_as_a_size_t(_Roff), mse::msev_as_a_size_t(_Count));
					return (*this);
				}

		#ifdef MSE_HAS_CXX17
			private:
				template<class _StringViewIsh>
				gnii_basic_string& replace_helper2(std::true_type, const size_type _Off, const size_type _N0, const _StringViewIsh& _Right) {
					std::basic_string_view<_Ty, _Traits> _As_view = _Right;
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().replace(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _As_view);
					/*m_debug_size = size();*/
					return (*this);
				}
				template<class _TParam1>
				gnii_basic_string& replace_helper2(std::false_type, const size_type _Off, const size_type _N0, const _TParam1& _Right) {
					const auto xs_iters = mse::impl::make_xscope_range_iter_provider(_Right);
					return replace(_Off, _N0, xs_iters.begin(), xs_iters.end());
				}
				gnii_basic_string& replace_helper1(std::true_type, const size_type _Off, const size_type _N0, const gnii_basic_string& _Right) {
					return (replace(_Off, _N0, _Right));
				}
				template<class _TParam1>
				gnii_basic_string& replace_helper1(std::false_type, const size_type _Off, const size_type _N0, const _TParam1& _Right) {
					return replace_helper2(typename _is_string_view_ish<_TParam1>::type(), _Off, _N0, _Right);
				}
			public:
				template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
				gnii_basic_string& replace(const size_type _Off, const size_type _N0, const _TParam1& _Right) {
					return replace_helper1(typename std::is_base_of<gnii_basic_string, _TParam1>::type(), _Off, _N0, _Right);
				}

			private:
				template<class _StringViewIsh>
				gnii_basic_string& replace_helper2(std::true_type, const size_type _Off, const size_type _N0, const _StringViewIsh& _Right, const size_type _Roff, const size_type _Count) {
					std::basic_string_view<_Ty, _Traits> _As_view = _Right;
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().replace(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _As_view, mse::as_a_size_t(_Roff), mse::as_a_size_t(_Count));
					/*m_debug_size = size();*/
					return (*this);
				}
				template<class _TParam1>
				gnii_basic_string& replace_helper2(std::false_type, const size_type _Off, const size_type _N0, const _TParam1& _Right, const size_type _Roff, const size_type _Count) {
					auto adjusted_count = (npos == _Count) ? (difference_type(mse::container_size(_Right)) - difference_type(_Roff))
						: difference_type(_Count);
					if (0 > adjusted_count) { MSE_THROW(std::out_of_range(" gnii_basic_string::replace() ")); }
					const auto xs_iters = mse::impl::make_xscope_range_iter_provider(_Right);
					return replace(_Off, _N0, xs_iters.begin() + difference_type(_Roff), xs_iters.begin() + difference_type(_Roff + adjusted_count));
				}
				gnii_basic_string& replace_helper1(std::true_type, const size_type _Off, const size_type _N0, const gnii_basic_string& _Right, const size_type _Roff, const size_type _Count) {
					return (replace(_Off, _N0, _Right, _Roff, _Count));
				}
				template<class _TParam1>
				gnii_basic_string& replace_helper1(std::false_type, const size_type _Off, const size_type _N0, const _TParam1& _Right, const size_type _Roff, const size_type _Count) {
					return replace_helper2(typename _is_string_view_ish<_TParam1>::type(), _Off, _N0, _Right, _Roff, _Count);
				}
			public:
				template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
				gnii_basic_string& replace(const size_type _Off, const size_type _N0, const _TParam1& _Right, const size_type _Roff, const size_type _Count = npos) {
					return replace_helper1(typename std::is_base_of<gnii_basic_string, _TParam1>::type(), _Off, _N0, _Right, _Roff, _Count);
				}
		#else /* MSE_HAS_CXX17 */
				template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
				gnii_basic_string& replace(const size_type _Off, const size_type _N0, const _TStringSection& _X) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().replace(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _X.cbegin(), _X.cend());
					/*m_debug_size = size();*/
					return (*this);
				}
		#endif /* MSE_HAS_CXX17 */

				gnii_basic_string& replace(const size_type _Off, size_type _N0, const _Ty * const _Ptr, const size_type _Count) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().replace(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _Ptr, mse::msev_as_a_size_t(_Count));
					return (*this);
				}

				gnii_basic_string& replace(const size_type _Off, const size_type _N0, const _Ty * const _Ptr) {
					return (replace(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _Ptr, size_type(_Traits::length(_Ptr))));
				}

				gnii_basic_string& replace(const size_type _Off, size_type _N0, const size_type _Count, const _Ty _Ch) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().replace(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), mse::msev_as_a_size_t(_Count), _Ch);
					return (*this);
				}

				gnii_basic_string& replace(const const_iterator _First, const const_iterator _Last, mse::TXScopeFixedConstPointer<gnii_basic_string> xs_ptr) {
					return (replace(_First - begin(), _Last - _First, xs_ptr));
				}
				gnii_basic_string& replace(const const_iterator _First, const const_iterator _Last, const gnii_basic_string& _Right) {
					return (replace(_First - begin(), _Last - _First, _Right));
				}

		#if 0//_HAS_CXX17
				gnii_basic_string& replace(const const_iterator _First, const const_iterator _Last,
					const mstd::basic_string_view<_Ty, _Traits> _Right) {
					return (replace(_First - begin(), _Last - _First, _Right));
				}
		#endif /* _HAS_CXX17 */

				gnii_basic_string& replace(const const_iterator _First, const const_iterator _Last,
					const _Ty * const _Ptr, const size_type _Count) {
					return (replace(_First - begin(), _Last - _First, _Ptr, _Count));
				}

				gnii_basic_string& replace(const const_iterator _First, const const_iterator _Last, const _Ty * const _Ptr) {
					return (replace(_First - begin(), _Last - _First, _Ptr));
				}

				gnii_basic_string& replace(const const_iterator _First, const const_iterator _Last, const size_type _Count, const _Ty _Ch) {
					return (replace(_First - begin(), _Last - _First, _Count, _Ch));
				}

				template<class _Iter, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value> MSE_IMPL_EIS >
				gnii_basic_string& replace(const const_iterator _First, const const_iterator _Last,
					const _Iter _First2, const _Iter _Last2) {
					const gnii_basic_string _Right(_First2, _Last2, get_allocator());
					replace(_First, _Last, _Right);
					return (*this);
				}

				template<class _Iter, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value> MSE_IMPL_EIS >
				gnii_basic_string& replace(const size_type _Off, const size_type _N0, const _Iter _First2, const _Iter _Last2) {
					smoke_check_source_iterators(_First2, _Last2);
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					if (contained_basic_string().size() <= (mse::msev_as_a_size_t(_Off) + mse::msev_as_a_size_t(_N0))) { MSE_THROW(std::out_of_range(" gnii_basic_string::replace() ")); }
					auto iter1 = contained_basic_string().begin() + mse::msev_as_a_size_t(_Off);
					contained_basic_string().replace(iter1, iter1 + mse::msev_as_a_size_t(_N0), _First2, _Last2);
					return (*this);
				}

				int compare(mse::TXScopeFixedConstPointer<gnii_basic_string> xs_ptr) const _NOEXCEPT {
					return contained_basic_string().compare((*xs_ptr).contained_basic_string());
				}
				int compare(const gnii_basic_string& _Right) const _NOEXCEPT {
					return contained_basic_string().compare(_Right.contained_basic_string());
				}
				int compare(size_type _Off, size_type _N0, mse::TXScopeFixedConstPointer<gnii_basic_string> xs_ptr) const {
					return contained_basic_string().compare(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), (*xs_ptr).contained_basic_string());
				}
				int compare(size_type _Off, size_type _N0, const gnii_basic_string& _Right) const {
					return contained_basic_string().compare(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _Right.contained_basic_string());
				}
				int compare(const size_type _Off, const size_type _N0, mse::TXScopeFixedConstPointer<gnii_basic_string> xs_ptr,
					const size_type _Roff, const size_type _Count = npos) const {
					return contained_basic_string().compare(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), (*xs_ptr).contained_basic_string(), _Roff, _Count);
				}
				int compare(const size_type _Off, const size_type _N0, const gnii_basic_string& _Right,
					const size_type _Roff, const size_type _Count = npos) const {
					return contained_basic_string().compare(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _Right.contained_basic_string(), _Roff, _Count);
				}

				int compare(const _Ty * const _Ptr) const _NOEXCEPT {
					return contained_basic_string().compare(_Ptr);
				}

				int compare(const size_type _Off, const size_type _N0, const _Ty * const _Ptr) const {
					return contained_basic_string().compare(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _Ptr);
				}

				int compare(const size_type _Off, const size_type _N0, const _Ty * const _Ptr,
					const size_type _Count) const {
					return contained_basic_string().compare(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _Ptr, mse::msev_as_a_size_t(_Count));
				}

		#ifdef MSE_HAS_CXX17
			private:
				template<class _StringViewIsh>
				int compare_helper2(std::true_type, const _StringViewIsh& _Right) const {
					std::basic_string_view<_Ty, _Traits> _As_view = _Right;
					return contained_basic_string().compare(_As_view);
				}
				template<class _TParam1>
				int compare_helper2(std::false_type, const _TParam1& _Right) const {
					const auto xs_iters = mse::impl::make_xscope_range_iter_provider(_Right);
					return std::lexicographical_compare(contained_basic_string().cbegin(), contained_basic_string().cend(), xs_iters.begin(), xs_iters.end());
					//return compare(gnii_basic_string(xs_iters.begin(), xs_iters.end()));
				}
				int compare_helper1(std::true_type, const gnii_basic_string& _Right) const {
					return compare(_Right);
				}
				template<class _TParam1>
				int compare_helper1(std::false_type, const _TParam1& _Right) const {
					return compare_helper2(typename _is_string_view_ish<_TParam1>::type(), _Right);
				}
			public:
				template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
				int compare(const _TParam1& _Right) const {
					return compare_helper1(typename std::is_base_of<gnii_basic_string, _TParam1>::type(), _Right);
				}

			private:
				template<class _StringViewIsh>
				int compare_helper2(std::true_type, const size_type _Off, const size_type _N0, const _StringViewIsh& _Right) const {
					std::basic_string_view<_Ty, _Traits> _As_view = _Right;
					return contained_basic_string().compare(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _As_view);
				}
				template<class _TParam1>
				int compare_helper2(std::false_type, const size_type _Off, const size_type _N0, const _TParam1& _Right) const {
					const auto xs_iters = mse::impl::make_xscope_range_iter_provider(_Right);
					return compare(_Off, _N0, gnii_basic_string(xs_iters.begin(), xs_iters.end()));
				}
				int compare_helper1(std::true_type, const size_type _Off, const size_type _N0, const gnii_basic_string& _Right) const {
					return compare(_Off, _N0, _Right);
				}
				template<class _TParam1>
				int compare_helper1(std::false_type, const size_type _Off, const size_type _N0, const _TParam1& _Right) const {
					return compare_helper2(typename _is_string_view_ish<_TParam1>::type(), _Off, _N0, _Right);
				}
			public:
				template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
				int compare(const size_type _Off, const size_type _N0, const _TParam1& _Right) const {
					return compare_helper1(typename std::is_base_of<gnii_basic_string, _TParam1>::type(), _Off, _N0, _Right);
				}

			private:
				template<class _StringViewIsh>
				int compare_helper2(std::true_type, const size_type _Off, const size_type _N0, const _StringViewIsh& _Right, const size_type _Roff, const size_type _Count) const {
					std::basic_string_view<_Ty, _Traits> _As_view = _Right;
					return contained_basic_string().compare(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _As_view, mse::as_a_size_t(_Roff), mse::as_a_size_t(_Count));
				}
				template<class _TParam1>
				int compare_helper2(std::false_type, const size_type _Off, const size_type _N0, const _TParam1& _Right, const size_type _Roff, const size_type _Count) const {
					auto adjusted_count = (npos == _Count) ? (difference_type(mse::container_size(_Right)) - difference_type(_Roff))
						: difference_type(_Count);
					if (0 > adjusted_count) { MSE_THROW(std::out_of_range(" gnii_basic_string::compare() ")); }
					const auto xs_iters = mse::impl::make_xscope_range_iter_provider(_Right);
					return compare(_Off, _N0, gnii_basic_string(xs_iters.begin() + difference_type(_Roff), xs_iters.begin() + difference_type(_Roff + adjusted_count)));
				}
				int compare_helper1(std::true_type, const size_type _Off, const size_type _N0, const gnii_basic_string& _Right, const size_type _Roff, const size_type _Count) const {
					return compare(_Off, _N0, _Right, _Roff, _Count);
				}
				template<class _TParam1>
				int compare_helper1(std::false_type, const size_type _Off, const size_type _N0, const _TParam1& _Right, const size_type _Roff, const size_type _Count) const {
					return compare_helper2(typename _is_string_view_ish<_TParam1>::type(), _Off, _N0, _Right, _Roff, _Count);
				}
			public:
				template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
				int compare(const size_type _Off, const size_type _N0, const _TParam1& _Right, const size_type _Roff, const size_type _Count = npos) const {
					return compare_helper1(typename std::is_base_of<gnii_basic_string, _TParam1>::type(), _Off, _N0, _Right, _Roff, _Count);
				}
		#else /* MSE_HAS_CXX17 */
				template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
				int compare(const size_type _Off, const size_type _N0, const _TStringSection& _X) const {
					return contained_basic_string().compare(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), gnii_basic_string(_X.cbegin(), _X.cend()));
				}
		#endif /* MSE_HAS_CXX17 */

		#ifdef MSE_HAS_CXX17
			private:
				template<class _StringViewIsh>
				size_type find_helper2(std::true_type, const _StringViewIsh& _Right, const size_type _Off = npos) const {
					std::basic_string_view<_Ty, _Traits> _As_view = _Right;
					return contained_basic_string().find(_As_view, mse::as_a_size_t(_Off));
				}
				template<class _TParam1>
				size_type find_helper2(std::false_type, const _TParam1& _Right, const size_type _Off = npos) const {
					const auto xs_iters = mse::impl::make_xscope_range_iter_provider(_Right);
					return find(gnii_basic_string(xs_iters.begin(), xs_iters.end()), _Off);
				}
				size_type find_helper1(std::true_type, const gnii_basic_string& _Right, const size_type _Off = npos) const {
					return find(_Right, _Off);
				}
				template<class _TParam1>
				size_type find_helper1(std::false_type, const _TParam1& _Right, const size_type _Off = npos) const {
					return find_helper2(typename _is_string_view_ish<_TParam1>::type(), _Right, _Off);
				}
			public:
				template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
				size_type find(const _TParam1& _Right, const size_type _Off = npos) const {
					return find_helper1(typename std::is_base_of<gnii_basic_string, _TParam1>::type(), _Right, _Off);
				}
		#else /* MSE_HAS_CXX17 */
				template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
				size_type find(const _TStringSection& _X, const size_type _Off = npos) const {
					return contained_basic_string().find(gnii_basic_string(_X.cbegin(), _X.cend()), mse::as_a_size_t(_Off));
				}
		#endif /* MSE_HAS_CXX17 */

				size_type find(const gnii_basic_string& _Right, const size_type _Off = 0) const _NOEXCEPT {
					return contained_basic_string().find(_Right.contained_basic_string(), _Off);
				}

				size_type find(const _Ty * const _Ptr, const size_type _Off, const size_type _Count) const _NOEXCEPT {
					return contained_basic_string().find(_Ptr, _Off, _Count);
				}

				size_type find(const _Ty * const _Ptr, const size_type _Off = 0) const _NOEXCEPT {
					return contained_basic_string().find(_Ptr, _Off);
				}

				size_type find(const _Ty _Ch, const size_type _Off = 0) const _NOEXCEPT {
					return contained_basic_string().find(_Ch, _Off);
				}

		#ifdef MSE_HAS_CXX17
			private:
				template<class _StringViewIsh>
				size_type rfind_helper2(std::true_type, const _StringViewIsh& _Right, const size_type _Off = npos) const {
					std::basic_string_view<_Ty, _Traits> _As_view = _Right;
					return contained_basic_string().rfind(_As_view, mse::as_a_size_t(_Off));
				}
				template<class _TParam1>
				size_type rfind_helper2(std::false_type, const _TParam1& _Right, const size_type _Off = npos) const {
					const auto xs_iters = mse::impl::make_xscope_range_iter_provider(_Right);
					return rfind(gnii_basic_string(xs_iters.begin(), xs_iters.end()), _Off);
				}
				size_type rfind_helper1(std::true_type, const gnii_basic_string& _Right, const size_type _Off = npos) const {
					return rfind(_Right, _Off);
				}
				template<class _TParam1>
				size_type rfind_helper1(std::false_type, const _TParam1& _Right, const size_type _Off = npos) const {
					return rfind_helper2(typename _is_string_view_ish<_TParam1>::type(), _Right, _Off);
				}
			public:
				template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
				size_type rfind(const _TParam1& _Right, const size_type _Off = npos) const {
					return rfind_helper1(typename std::is_base_of<gnii_basic_string, _TParam1>::type(), _Right, _Off);
				}
		#else /* MSE_HAS_CXX17 */
				template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
				size_type rfind(const _TStringSection& _X, const size_type _Off = npos) const {
					return contained_basic_string().rfind(gnii_basic_string(_X.cbegin(), _X.cend()), mse::as_a_size_t(_Off));
				}
		#endif /* MSE_HAS_CXX17 */

				size_type rfind(const gnii_basic_string& _Right, const size_type _Off = npos) const _NOEXCEPT {
					return contained_basic_string().rfind(_Right.contained_basic_string(), _Off);
				}

				size_type rfind(const _Ty * const _Ptr, const size_type _Off, const size_type _Count) const _NOEXCEPT {
					return contained_basic_string().rfind(_Ptr, _Off, _Count);
				}

				size_type rfind(const _Ty * const _Ptr, const size_type _Off = npos) const _NOEXCEPT {
					return contained_basic_string().rfind(_Ptr, _Off);
				}

				size_type rfind(const _Ty _Ch, const size_type _Off = npos) const _NOEXCEPT {
					return contained_basic_string().rfind(_Ch, _Off);
				}

		#if 0//_HAS_CXX17
				size_type find_first_of(const mstd::basic_string_view<_Ty, _Traits> _Right, const size_type _Off = 0) const _NOEXCEPT
				{	// look for one of _Right at or after _Off
					auto& _My_data = this->_Get_data();
					return (static_cast<size_type>(_Traits_find_first_of<_Traits>(_My_data._Myptr(), _My_data._Mysize, _Off,
						_Right.data(), _Right.size())));
				}
		#endif /* _HAS_CXX17 */

				size_type find_first_of(const gnii_basic_string& _Right, const size_type _Off = 0) const _NOEXCEPT {
					return contained_basic_string().find_first_of(_Right.contained_basic_string(), _Off);
				}

				size_type find_first_of(const _Ty * const _Ptr, const size_type _Off,
					const size_type _Count) const _NOEXCEPT {
					return contained_basic_string().find_first_of(_Ptr, _Off, _Count);
				}

				size_type find_first_of(const _Ty * const _Ptr, const size_type _Off = 0) const _NOEXCEPT {
					return contained_basic_string().find_first_of(_Ptr, _Off);
				}

				size_type find_first_of(const _Ty _Ch, const size_type _Off = 0) const _NOEXCEPT {
					return contained_basic_string().find_first_of(_Ch, _Off);
				}

		#if 0//_HAS_CXX17
				size_type find_last_of(const mstd::basic_string_view<_Ty, _Traits> _Right, const size_type _Off = npos) const _NOEXCEPT
				{	// look for one of _Right before _Off
					auto& _My_data = this->_Get_data();
					return (static_cast<size_type>(_Traits_find_last_of<_Traits>(_My_data._Myptr(), _My_data._Mysize, _Off,
						_Right.data(), _Right.size())));
				}
		#endif /* _HAS_CXX17 */

				size_type find_last_of(const gnii_basic_string& _Right, size_type _Off = npos) const _NOEXCEPT {
					return contained_basic_string().find_last_of(_Right.contained_basic_string(), _Off);
				}

				size_type find_last_of(const _Ty * const _Ptr, const size_type _Off,
					const size_type _Count) const _NOEXCEPT {
					return contained_basic_string().find_last_of(_Ptr, _Off, _Count);
				}

				size_type find_last_of(const _Ty * const _Ptr, const size_type _Off = npos) const _NOEXCEPT {
					return contained_basic_string().find_last_of(_Ptr, _Off);
				}

				size_type find_last_of(const _Ty _Ch, const size_type _Off = npos) const _NOEXCEPT {
					return contained_basic_string().find_last_of(_Ch, _Off);
				}

		#if 0//_HAS_CXX17
				size_type find_first_not_of(const mstd::basic_string_view<_Ty, _Traits> _Right, const size_type _Off = 0) const
					_NOEXCEPT
				{	// look for none of _Right at or after _Off
					auto& _My_data = this->_Get_data();
					return (static_cast<size_type>(_Traits_find_first_not_of<_Traits>(_My_data._Myptr(), _My_data._Mysize, _Off,
						_Right.data(), _Right.size())));
				}
		#endif /* _HAS_CXX17 */

				size_type find_first_not_of(const gnii_basic_string& _Right, const size_type _Off = 0) const _NOEXCEPT {
					return contained_basic_string().find_first_not_of(_Right.contained_basic_string(), _Off);
				}

				size_type find_first_not_of(const _Ty * const _Ptr, const size_type _Off,
					const size_type _Count) const _NOEXCEPT {
					return contained_basic_string().find_first_not_of(_Ptr, _Off, _Count);
				}

				size_type find_first_not_of(const _Ty * const _Ptr, size_type _Off = 0) const _NOEXCEPT {
					return contained_basic_string().find_first_not_of(_Ptr, _Off);
				}

				size_type find_first_not_of(const _Ty _Ch, const size_type _Off = 0) const _NOEXCEPT {
					return contained_basic_string().find_first_not_of(_Ch, _Off);
				}

		#if 0//_HAS_CXX17
				size_type find_last_not_of(const mstd::basic_string_view<_Ty, _Traits> _Right, const size_type _Off = npos) const
					_NOEXCEPT
				{	// look for none of _Right before _Off
					auto& _My_data = this->_Get_data();
					return (static_cast<size_type>(_Traits_find_last_not_of<_Traits>(_My_data._Myptr(), _My_data._Mysize, _Off,
						_Right.data(), _Right.size())));
				}
		#endif /* _HAS_CXX17 */

				size_type find_last_not_of(const gnii_basic_string& _Right, const size_type _Off = npos) const _NOEXCEPT {
					return contained_basic_string().find_last_not_of(_Right.contained_basic_string(), _Off);
				}

				size_type find_last_not_of(const _Ty * const _Ptr, const size_type _Off,
					const size_type _Count) const _NOEXCEPT {
					return contained_basic_string().find_last_not_of(_Ptr, _Off, _Count);
				}

				size_type find_last_not_of(const _Ty * const _Ptr, const size_type _Off = npos) const _NOEXCEPT {
					return contained_basic_string().find_last_not_of(_Ptr, _Off);
				}

				size_type find_last_not_of(const _Ty _Ch, const size_type _Off = npos) const _NOEXCEPT {
					return contained_basic_string().find_last_not_of(_Ch, _Off);
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
						(*target_iter) = contained_basic_string()[mse::msev_as_a_size_t(i)];
						++target_iter;
					}
					return n;
				}

				gnii_basic_string substr(const size_type _Off = 0, const size_type _Count = npos) const {
					//return (gnii_basic_string(*this, _Off, _Count, get_allocator()));
					return (gnii_basic_string(contained_basic_string().substr(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_Count))));
				}

				static gnii_basic_string& _Myt_ref(gnii_basic_string& nbs_ref) {
					return nbs_ref;
				}

				template<typename _TThisPointer>
				static std::basic_istream<_Ty, _Traits>& getline(std::basic_istream<_Ty, _Traits>&& _Istr, _TThisPointer this_ptr, const _Ty _Delim) {
					return std::getline(MSE_FWD(_Istr), _Myt_ref(*this_ptr).contained_basic_string(), _Delim);
				}
				template<typename _TThisPointer>
				static std::basic_istream<_Ty, _Traits>& getline(std::basic_istream<_Ty, _Traits>&& _Istr, _TThisPointer this_ptr) {
					return std::getline(MSE_FWD(_Istr), _Myt_ref(*this_ptr).contained_basic_string());
				}
				template<typename _TThisPointer>
				static std::basic_istream<_Ty, _Traits>& getline(std::basic_istream<_Ty, _Traits>& _Istr, _TThisPointer this_ptr, const _Ty _Delim) {
					return std::getline(_Istr, _Myt_ref(*this_ptr).contained_basic_string(), _Delim);
				}
				template<typename _TThisPointer>
				static std::basic_istream<_Ty, _Traits>& getline(std::basic_istream<_Ty, _Traits>& _Istr, _TThisPointer this_ptr) {
					return std::getline(_Istr, _Myt_ref(*this_ptr).contained_basic_string());
				}

				MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(_Ty);

			private:
				/* If _Ty is an xscope type, then the following member function will not instantiate, causing an
				(intended) compile error. */
				template<class _Ty2 = _Ty, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _Ty>::value) && (mse::impl::is_potentially_not_xscope<_Ty2>::value)> MSE_IMPL_EIS >
				void valid_if_Ty_is_not_an_xscope_type() const {}

				auto begin() { return contained_basic_string().begin(); }
				auto end() { return contained_basic_string().end(); }
				auto begin() const { return contained_basic_string().begin(); }
				auto end() const { return contained_basic_string().end(); }
				auto cbegin() const { return contained_basic_string().cbegin(); }
				auto cend() const { return contained_basic_string().cend(); }

				auto rbegin() { return contained_basic_string().rbegin(); }
				auto rend() { return contained_basic_string().rend(); }
				auto rbegin() const { return contained_basic_string().rbegin(); }
				auto rend() const { return contained_basic_string().rend(); }
				auto crbegin() const { return contained_basic_string().crbegin(); }
				auto crend() const { return contained_basic_string().crend(); }


				typename std_basic_string::iterator insert(typename std_basic_string::const_iterator _P, _Ty _X) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					typename std_basic_string::iterator retval = contained_basic_string().insert(_P, _X);
					/*m_debug_size = size();*/
					return retval;
				}
				typename std_basic_string::iterator insert(typename std_basic_string::const_iterator _P, size_type _M, _Ty _X) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					typename std_basic_string::iterator retval = contained_basic_string().insert(_P, msev_as_a_size_t(_M), _X);
					/*m_debug_size = size();*/
					return retval;
				}
				template<class _Iter
					//>mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value, typename std_basic_string::iterator>
					, class = mse::impl::_mse_RequireInputIter<_Iter> >
				typename std_basic_string::iterator insert(typename std_basic_string::const_iterator _Where, const _Iter& _First, const _Iter& _Last) {	// insert [_First, _Last) at _Where
					smoke_check_source_iterators(_First, _Last);
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					auto retval = contained_basic_string().insert(_Where, _First, _Last);
					/*m_debug_size = size();*/
					return retval;
				}
				template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
				typename std_basic_string::iterator insert(typename std_basic_string::const_iterator _P, const _TStringSection& _X) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					/* todo: optimize? In most cases a temporary copy shouldn't be necessary, but what about when the string section
					refers to a section of the target string? */
					const _Myt temp_nii_str(_X);
					typename std_basic_string::iterator retval = contained_basic_string().insert(_P, temp_nii_str.contained_basic_string().cbegin(), temp_nii_str.contained_basic_string().cend());
					/*m_debug_size = size();*/
					return retval;
				}
				template<class ..._Valty>
				typename std_basic_string::iterator emplace(typename std_basic_string::const_iterator _Where, _Valty&& ..._Val)
				{	// insert by moving _Val at _Where
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					auto retval = contained_basic_string().insert(_Where, std::forward<_Valty>(_Val)...);
					/*m_debug_size = size();*/
					return retval;
				}
				typename std_basic_string::iterator erase(typename std_basic_string::const_iterator _P) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					typename std_basic_string::iterator retval = contained_basic_string().erase(_P);
					/*m_debug_size = size();*/
					return retval;
				}
				typename std_basic_string::iterator erase(typename std_basic_string::const_iterator _F, typename std_basic_string::const_iterator _L) {
					structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					typename std_basic_string::iterator retval = contained_basic_string().erase(_F, _L);
					/*m_debug_size = size();*/
					return retval;
				}

				typename std_basic_string::iterator insert(typename std_basic_string::const_iterator _Where, _XSTD initializer_list<typename std_basic_string::value_type> _Ilist) {	// insert initializer_list
					auto retval = contained_basic_string().insert(_Where, _Ilist);
					/*m_debug_size = size();*/
					return retval;
				}

				static std::basic_istream<_Ty, _Traits>& in_from_stream(std::basic_istream<_Ty, _Traits>&& _Istr, gnii_basic_string& _Str) {
					return MSE_FWD(_Istr) >> _Str.contained_basic_string();
				}
				static std::basic_istream<_Ty, _Traits>& in_from_stream(std::basic_istream<_Ty, _Traits>& _Istr, gnii_basic_string& _Str) {
					return _Istr >> _Str.contained_basic_string();
				}
				static std::basic_ostream<_Ty, _Traits>& out_to_stream(std::basic_ostream<_Ty, _Traits>& _Ostr, const gnii_basic_string& _Str) {
					return _Ostr << _Str.contained_basic_string();
				}

				template<class _Mutex>
				class structure_change_guard {
				public:
					structure_change_guard(_Mutex& _Mtx) MSE_FUNCTION_TRY : m_lock_guard(_Mtx) {}
					MSE_FUNCTION_CATCH_ANY {
						MSE_THROW(mse::structure_lock_violation_error("structure lock violation - Attempting to modify \
							the structure (size/capacity) of a container while a reference (iterator) to one of its elements \
							still exists?"));
					}
				private:
					std::lock_guard<_Mutex> m_lock_guard;
				};

				//std_basic_string m_basic_string;
				//_TStateMutex m_mutex1;

				/* The "mutability" of m_structure_change_mutex is not actually required or utilized by this class, and thus
				doesn't compromise the safety of sharing this class among asynchronous thread. The mutability is utilized by
				derived (friend) classes (that will identify themselves as not safely shareable). */
				mutable _TStateMutex m_structure_change_mutex;

				friend /*class */xscope_ss_const_iterator_type;
				friend /*class */xscope_ss_iterator_type;
				template<class _Ty2, class _Traits2, class _A2, class _TStateMutex2, template<typename> class _TTXScopeConstIterator2> friend class gnii_basic_string;
				//friend class us::msebasic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>;
				template<class _Ty2, class _Traits2, class _A2, class _TStateMutex2> friend class us::msebasic_string;
				friend class mse::impl::ns_gnii_basic_string::xscope_structure_lock_guard<_Myt>;
				friend class mse::us::impl::ns_gnii_basic_string::xscope_const_structure_lock_guard<_Myt>;
				friend class mse::us::impl::Txscope_structure_lock_guard<_Myt>;
				friend class mse::us::impl::Txscope_const_structure_lock_guard<_Myt>;

				friend struct std::hash<gnii_basic_string>;
				template<class _Ty2, class _Traits2/* = std::char_traits<_Ty2>*/, class _A2/* = std::allocator<_Ty2>*/, class _TStateMutex2/* = mse::non_thread_safe_shared_mutex*/, template<typename> class _TTXScopeConstIterator2/* = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_ss_const_iterator_type*/>
				friend std::basic_istream<_Ty2, _Traits2>& mse::us::impl::impl::ns_gnii_basic_string::in_from_stream(std::basic_istream<_Ty2, _Traits2>&& _Istr, gnii_basic_string<_Ty2, _Traits2, _A2, _TStateMutex2, _TTXScopeConstIterator2>& _Str);
				template<class _Ty2, class _Traits2/* = std::char_traits<_Ty2>*/, class _A2/* = std::allocator<_Ty2>*/, class _TStateMutex2/* = mse::non_thread_safe_shared_mutex*/, template<typename> class _TTXScopeConstIterator2/* = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_ss_const_iterator_type*/>
				friend std::basic_istream<_Ty2, _Traits2>& mse::us::impl::impl::ns_gnii_basic_string::in_from_stream(std::basic_istream<_Ty2, _Traits2>& _Istr, gnii_basic_string<_Ty2, _Traits2, _A2, _TStateMutex2, _TTXScopeConstIterator2>& _Str);
				template<class _Ty2, class _Traits2/* = std::char_traits<_Ty2>*/, class _A2/* = std::allocator<_Ty2>*/, class _TStateMutex2/* = mse::non_thread_safe_shared_mutex*/, template<typename> class _TTXScopeConstIterator2/* = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_ss_const_iterator_type*/>
				friend std::basic_ostream<_Ty2, _Traits2>& mse::us::impl::impl::ns_gnii_basic_string::out_to_stream(std::basic_ostream<_Ty2, _Traits2>& _Ostr, const gnii_basic_string<_Ty2, _Traits2, _A2, _TStateMutex2, _TTXScopeConstIterator2>& _Str);

				friend void swap(_Myt& a, _Myt& b) _NOEXCEPT_OP(_NOEXCEPT_OP(a.swap(b))) { a.swap(b); }
				friend void swap(_Myt& a, _MBS& b) _NOEXCEPT_OP(_NOEXCEPT_OP(a.swap(b))) { a.swap(b); }
				friend void swap(_MBS& a, _Myt& b) _NOEXCEPT_OP(_NOEXCEPT_OP(b.swap(a))) { b.swap(a); }
			};

		#ifdef MSE_HAS_CXX17
			/* deduction guides */
			template<class _Iter, class _Alloc = std::allocator<typename std::iterator_traits<_Iter>::value_type>
				, std::enable_if_t<std::conjunction_v< mse::impl::_mse_Is_iterator<_Iter>, mse::impl::_mse_Is_allocator<_Alloc> >, int> = 0>
			gnii_basic_string(_Iter, _Iter, _Alloc = _Alloc())
				->gnii_basic_string<typename std::iterator_traits<_Iter>::value_type, std::char_traits<typename std::iterator_traits<_Iter>::value_type>, _Alloc>;

			template<class _Elem, class _Traits, class _Alloc = std::allocator<_Elem>
				, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
			explicit gnii_basic_string(std::basic_string_view<_Elem, _Traits>, const _Alloc& = _Alloc())
				->gnii_basic_string<_Elem, _Traits, _Alloc>;

			template<class _Elem, class _Traits, class _Alloc = std::allocator<_Elem>
				, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
			gnii_basic_string(std::basic_string_view<_Elem, _Traits>, mse::impl::_mse_Guide_size_type_t<_Alloc>
				, mse::impl::_mse_Guide_size_type_t<_Alloc>, const _Alloc& = _Alloc())
				->gnii_basic_string<_Elem, _Traits, _Alloc>;

			template<class _TRAIterator, class _Traits, class _Alloc = std::allocator<typename std::iterator_traits<_TRAIterator>::value_type>
				, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
			explicit gnii_basic_string(mse::TStringConstSection<_TRAIterator, _Traits>, const _Alloc& = _Alloc())
				->gnii_basic_string<typename std::iterator_traits<_TRAIterator>::value_type, _Traits, _Alloc>;
			template<class _TRAIterator, class _Traits, class _Alloc = std::allocator<typename std::iterator_traits<_TRAIterator>::value_type>
				, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
			explicit gnii_basic_string(mse::TStringSection<_TRAIterator, _Traits>, const _Alloc& = _Alloc())
				->gnii_basic_string<typename std::iterator_traits<_TRAIterator>::value_type, _Traits, _Alloc>;
			template<class _TRAIterator, class _Traits, class _Alloc = std::allocator<typename std::iterator_traits<_TRAIterator>::value_type>
				, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
			explicit gnii_basic_string(mse::TXScopeStringConstSection<_TRAIterator, _Traits>, const _Alloc& = _Alloc())
				->gnii_basic_string<typename std::iterator_traits<_TRAIterator>::value_type, _Traits, _Alloc>;
			template<class _TRAIterator, class _Traits, class _Alloc = std::allocator<typename std::iterator_traits<_TRAIterator>::value_type>
				, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
			explicit gnii_basic_string(mse::TXScopeStringSection<_TRAIterator, _Traits>, const _Alloc& = _Alloc())
				->gnii_basic_string<typename std::iterator_traits<_TRAIterator>::value_type, _Traits, _Alloc>;
		#endif /* MSE_HAS_CXX17 */

			namespace impl {
				namespace ns_gnii_basic_string {
					template<class _Ty, class _Traits/* = std::char_traits<_Ty>*/, class _A/* = std::allocator<_Ty>*/, class _TStateMutex/* = mse::non_thread_safe_shared_mutex*/, template<typename> class _TTXScopeConstIterator/* = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_ss_const_iterator_type*/>
					std::basic_istream<_Ty, _Traits>& in_from_stream(std::basic_istream<_Ty, _Traits>&& _Istr, gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>& _Str) {
						return _Str.in_from_stream(MSE_FWD(_Istr), _Str);
					}
					template<class _Ty, class _Traits/* = std::char_traits<_Ty>*/, class _A/* = std::allocator<_Ty>*/, class _TStateMutex/* = mse::non_thread_safe_shared_mutex*/, template<typename> class _TTXScopeConstIterator/* = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_ss_const_iterator_type*/>
					std::basic_istream<_Ty, _Traits>& in_from_stream(std::basic_istream<_Ty, _Traits>& _Istr, gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>& _Str) {
						return _Str.in_from_stream(_Istr, _Str);
					}
					template<class _Ty, class _Traits/* = std::char_traits<_Ty>*/, class _A/* = std::allocator<_Ty>*/, class _TStateMutex/* = mse::non_thread_safe_shared_mutex*/, template<typename> class _TTXScopeConstIterator/* = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_ss_const_iterator_type*/>
					std::basic_ostream<_Ty, _Traits>& out_to_stream(std::basic_ostream<_Ty, _Traits>& _Ostr, const gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>& _Str) {
						return _Str.out_to_stream(_Ostr, _Str);
					}
				}
			}

			template<class _Ty, class _Traits, class _A, class _TStateMutex, template<typename> class _TTXScopeConstIterator>
			std::basic_istream<_Ty, _Traits>& operator>>(std::basic_istream<_Ty, _Traits>&& _Istr, gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>& _Str) {
				return mse::us::impl::impl::ns_gnii_basic_string::in_from_stream(MSE_FWD(_Istr), _Str);
			}
			template<class _Ty, class _Traits, class _A, class _TStateMutex, template<typename> class _TTXScopeConstIterator>
			std::basic_istream<_Ty, _Traits>& operator>>(std::basic_istream<_Ty, _Traits>& _Istr, gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>& _Str) {
				return mse::us::impl::impl::ns_gnii_basic_string::in_from_stream(_Istr, _Str);
			}
			template<class _Ty, class _Traits, class _A, class _TStateMutex, template<typename> class _TTXScopeConstIterator>
			std::basic_ostream<_Ty, _Traits>& operator<<(std::basic_ostream<_Ty, _Traits>& _Ostr, const gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>& _Str) {
				return mse::us::impl::impl::ns_gnii_basic_string::out_to_stream(_Ostr, _Str);
			}

			template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty>, class _TStateMutex = mse::non_thread_safe_shared_mutex, template<typename> class _TTXScopeConstIterator = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_ss_const_iterator_type>
			inline bool operator!=(const gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>& _Left,
				const gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>& _Right) {	// test for basic_string inequality
				return (!(_Left == _Right));
			}

			template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty>, class _TStateMutex = mse::non_thread_safe_shared_mutex, template<typename> class _TTXScopeConstIterator = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_ss_const_iterator_type>
			inline bool operator>(const gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>& _Left,
				const gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>& _Right) {	// test if _Left > _Right for basic_strings
				return (_Right < _Left);
			}

			template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty>, class _TStateMutex = mse::non_thread_safe_shared_mutex, template<typename> class _TTXScopeConstIterator = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_ss_const_iterator_type>
			inline bool operator<=(const gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>& _Left,
				const gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>& _Right) {	// test if _Left <= _Right for basic_strings
				return (!(_Right < _Left));
			}

			template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty>, class _TStateMutex = mse::non_thread_safe_shared_mutex, template<typename> class _TTXScopeConstIterator = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_ss_const_iterator_type>
			inline bool operator>=(const gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>& _Left,
				const gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>& _Right) {	// test if _Left >= _Right for basic_strings
				return (!(_Left < _Right));
			}


			template<class _Elem, class _Traits, class _Alloc, class _TStateMutex, template<typename> class _TTXScopeConstIterator>
			inline gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator> operator+(const gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator>& _Left,
				const gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator>& _Right) {	// return string + string
				gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator> _Ans;
				_Ans.reserve(_Left.size() + _Right.size());
				_Ans += _Left;
				_Ans += _Right;
				return (_Ans);
			}

			template<class _Elem, class _Traits, class _Alloc, class _TStateMutex, template<typename> class _TTXScopeConstIterator>
			inline gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator> operator+(const _Elem * const _Left,
				const gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator>& _Right) {	// return NTCTS + string
				using _String_type = gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator>;
				using _Size_type = typename _String_type::size_type;
				_String_type _Ans;
				_Ans.reserve(_Size_type(_Traits::length(_Left) + _Right.size()));
				_Ans += _Left;
				_Ans += _Right;
				return (_Ans);
			}

			template<class _Elem, class _Traits, class _Alloc, class _TStateMutex, template<typename> class _TTXScopeConstIterator>
			inline gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator> operator+(const _Elem _Left,
				const gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator>& _Right) {	// return character + string
				gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator> _Ans;
				_Ans.reserve(1 + _Right.size());
				_Ans += _Left;
				_Ans += _Right;
				return (_Ans);
			}

			template<class _Elem, class _Traits, class _Alloc, class _TStateMutex, template<typename> class _TTXScopeConstIterator>
			inline gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator> operator+(const gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator>& _Left,
				const _Elem * const _Right) {	// return string + NTCTS
				using _String_type = gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator>;
				using _Size_type = typename _String_type::size_type;
				_String_type _Ans;
				_Ans.reserve(_Size_type(_Left.size() + _Traits::length(_Right)));
				_Ans += _Left;
				_Ans += _Right;
				return (_Ans);
			}

			template<class _Elem, class _Traits, class _Alloc, class _TStateMutex, template<typename> class _TTXScopeConstIterator>
			inline gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator> operator+(const gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator>& _Left,
				const _Elem _Right) {	// return string + character
				gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator> _Ans;
				_Ans.reserve(_Left.size() + 1);
				_Ans += _Left;
				_Ans += _Right;
				return (_Ans);
			}

			template<class _Elem, class _Traits, class _Alloc, class _TStateMutex, template<typename> class _TTXScopeConstIterator>
			inline gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator> operator+(const gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator>& _Left,
				gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator>&& _Right) {	// return string + string
				return (_STD move(_Right.insert(0, _Left)));
			}

			template<class _Elem, class _Traits, class _Alloc, class _TStateMutex, template<typename> class _TTXScopeConstIterator>
			inline gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator> operator+(gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator>&& _Left,
				const gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator>& _Right) {	// return string + string
				return (_STD move(_Left.append(_Right)));
			}

			template<class _Elem, class _Traits, class _Alloc, class _TStateMutex, template<typename> class _TTXScopeConstIterator>
			inline gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator> operator+(gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator>&& _Left,
				gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator>&& _Right) {	// return string + string
				if (_Right.size() <= _Left.capacity() - _Left.size()
					|| _Right.capacity() - _Right.size() < _Left.size())
					return (_STD move(_Left.append(_Right)));
				else
					return (_STD move(_Right.insert(0, _Left)));
			}

			template<class _Elem, class _Traits, class _Alloc, class _TStateMutex, template<typename> class _TTXScopeConstIterator>
			inline gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator> operator+(const _Elem * const _Left,
				gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator>&& _Right) {
				return (_STD move(_Right.insert(0, _Left)));
			}

			template<class _Elem, class _Traits, class _Alloc, class _TStateMutex, template<typename> class _TTXScopeConstIterator>
			inline gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator> operator+(const _Elem _Left,
				gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator>&& _Right) {
				using size_type = typename std::allocator_traits<_Alloc>::size_type;
				return (_STD move(_Right.insert(static_cast<size_type>(0), static_cast<size_type>(1), _Left)));
			}

			template<class _Elem, class _Traits, class _Alloc, class _TStateMutex, template<typename> class _TTXScopeConstIterator>
			inline gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator> operator+(gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator>&& _Left,
				const _Elem * const _Right) {
				return (_STD move(_Left.append(_Right)));
			}

			template<class _Elem, class _Traits, class _Alloc, class _TStateMutex, template<typename> class _TTXScopeConstIterator>
			inline gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator> operator+(gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator>&& _Left,
				const _Elem _Right) {	// return string + character
				_Left.push_back(_Right);
				return (_STD move(_Left));
			}
	
			namespace ns_gnii_basic_string {
				/* While an instance of xscope_structure_lock_guard exists it ensures that direct (scope) pointers to
				individual elements in the basic_string do not become invalid by preventing any operation that might resize the basic_string
				or increase its capacity. Any attempt to execute such an operation would result in an exception. */
				/* Constructing an xscope_structure_lock_guard involves a (shared) lock operation on the gnii_basic_string<>'s
				mutable m_structure_change_mutex. So in cases where m_structure_change_mutex is not thread safe, neither is
				the construction of xscope_structure_lock_guard from a const reference. So while it's safe to expose
				generally xscope_structure_lock_guards constructed from non-const references, it's up to the specific
				specializations and derived classes of gnii_basic_string<> to (optionally) expose xscope_structure_lock_guards
				constructed from const references and ensure their safety by either indicating that they are not eligible to
				be shared between threads (like stnii_basic_string<> does), or ensuring that m_structure_change_mutex is thread safe
				(like mtnii_basic_string<> does). */
				template<class _TContainer>
				class xscope_const_structure_lock_guard : public mse::us::impl::Txscope_const_structure_lock_guard<_TContainer> {
				public:
					typedef mse::us::impl::Txscope_const_structure_lock_guard<_TContainer> base_class;
					using base_class::base_class;

					operator mse::TXScopeFixedConstPointer<_TContainer>() const {
						return static_cast<const base_class&>(*this);
					}
				private:
					MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
				};
			}
		}
	}

	namespace impl {
		namespace ns_gnii_basic_string {
			/* While an instance of xscope_structure_lock_guard exists it ensures that direct (scope) pointers to
			individual elements in the basic_string do not become invalid by preventing any operation that might resize the basic_string
			or increase its capacity. Any attempt to execute such an operation would result in an exception. */
			template<class _TContainer>
			class xscope_structure_lock_guard : public mse::us::impl::Txscope_structure_lock_guard<_TContainer> {
			public:
				typedef mse::us::impl::Txscope_structure_lock_guard<_TContainer> base_class;
				using base_class::base_class;
				typedef _TContainer TDynamicContainer;

				operator mse::TXScopeFixedPointer<TDynamicContainer>() const {
					return static_cast<const base_class&>(*this);
				}
				/*
				template<class TDynamicContainer2 = TDynamicContainer, MSE_IMPL_EIP mse::impl::enable_if_t<!std::is_same<mse::TXScopeFixedConstPointer<TDynamicContainer2>, mse::TXScopeFixedPointer<TDynamicContainer> >::value> MSE_IMPL_EIS >
				explicit operator mse::TXScopeFixedConstPointer<TDynamicContainer2>() const {
					return static_cast<const base_class&>(*this);
				}
				*/
			private:
				MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
			};

			/* For objects that are access controlled under an "exclusive writer" access policy, the object is immutable
			while a const pointer to the object exists. So given an "exclusive writer" const pointer to a basic_string, it is
			safe to store the pointer provide a direct scope const pointer to any of its elements. */
			template<class _TContainer, class _TAccessMutex = mse::non_thread_safe_shared_mutex>
			class xscope_ewconst_structure_lock_guard : public mse::us::impl::Txscope_ewconst_structure_lock_guard<_TContainer, _TAccessMutex> {
			public:
				typedef mse::us::impl::Txscope_ewconst_structure_lock_guard<_TContainer, _TAccessMutex> base_class;
				using base_class::base_class;

				typedef mse::TAccessControlledConstPointer<_TContainer, _TAccessMutex> exclusive_writer_const_pointer_t;

				operator exclusive_writer_const_pointer_t() const {
					return static_cast<const base_class&>(*this);
				}
			private:
				MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
			};
		}
	}

	/* While an instance of xscope_structure_lock_guard exists it ensures that direct (scope) pointers to
	individual elements in the basic_string do not become invalid by preventing any operation that might resize the basic_string
	or increase its capacity. Any attempt to execute such an operation would result in an exception. */
	template<class _Ty, class _Traits, class _A = std::allocator<_Ty>, class _TStateMutex = mse::non_thread_safe_shared_mutex, template<typename> class _TTXScopeConstIterator = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_ss_const_iterator_type>
	auto make_xscope_structure_lock_guard(const mse::TXScopeObjFixedPointer<mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator> >& owner_ptr) -> mse::impl::ns_gnii_basic_string::xscope_structure_lock_guard<mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator> > {
		return mse::impl::ns_gnii_basic_string::xscope_structure_lock_guard<mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator> >(owner_ptr);
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, class _Traits, class _A = std::allocator<_Ty>, class _TStateMutex = mse::non_thread_safe_shared_mutex, template<typename> class _TTXScopeConstIterator = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_ss_const_iterator_type>
	auto make_xscope_structure_lock_guard(const mse::TXScopeFixedPointer<mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator> >& owner_ptr) -> mse::impl::ns_gnii_basic_string::xscope_structure_lock_guard<mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator> > {
		return mse::impl::ns_gnii_basic_string::xscope_structure_lock_guard<mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator> >(owner_ptr);
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	/* For objects that are access controlled under an "exclusive writer" access policy, the object is immutable
	while a const pointer to the object exists. So given an "exclusive writer" const pointer to a basic_string, it is
	safe to store the pointer provide a direct scope const pointer to any of its elements. */
	template<class _Ty, class _Traits, class _A, class _TStateMutex, template<typename> class _TTXScopeConstIterator, class _TAccessMutex = mse::non_thread_safe_shared_mutex>
	auto make_xscope_structure_lock_guard(const mse::TAccessControlledConstPointer<mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>, _TAccessMutex>& owner_ptr) -> mse::impl::ns_gnii_basic_string::xscope_ewconst_structure_lock_guard<mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>, _TAccessMutex> {
		return mse::impl::ns_gnii_basic_string::xscope_ewconst_structure_lock_guard<mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>, _TAccessMutex>(owner_ptr);
	}

	template<class _TDynamicContainerPointer>
	auto make_xscope_basic_string_size_change_lock_guard(const _TDynamicContainerPointer& owner_ptr) -> decltype(make_xscope_structure_lock_guard(owner_ptr)) {
		return make_xscope_structure_lock_guard(owner_ptr);
	}
	/* Overloads for rsv::TReturnableFParam<>. */
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_basic_string_size_change_lock_guard)
	namespace mstd {
		template<class _TDynamicContainerPointer>
		auto make_xscope_basic_string_size_change_lock_guard(const _TDynamicContainerPointer& owner_ptr) -> decltype(make_xscope_structure_lock_guard(owner_ptr)) {
			return make_xscope_structure_lock_guard(owner_ptr);
		}
		/* Overloads for rsv::TReturnableFParam<>. */
		MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_basic_string_size_change_lock_guard)
	}
	namespace us {
		template<class _TDynamicContainerPointer>
		auto make_xscope_basic_string_size_change_lock_guard(const _TDynamicContainerPointer& owner_ptr) -> decltype(make_xscope_structure_lock_guard(owner_ptr)) {
			return make_xscope_structure_lock_guard(owner_ptr);
		}
		/* Overloads for rsv::TReturnableFParam<>. */
		MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_basic_string_size_change_lock_guard)
	}
}

namespace std {

	template<class _Elem, class _Traits, class _Alloc, class _TStateMutex, template<typename> class _TTXScopeConstIterator>
	struct hash<mse::us::impl::gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator> > {	// hash functor for mse::us::impl::gnii_basic_string
		typedef typename mse::us::impl::gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator>::base_class basic_string_t;
		using argument_type = mse::us::impl::gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator>;
		using result_type = size_t;

		size_t operator()(const mse::us::impl::gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator>& _Keyval) const _NOEXCEPT {
			auto retval = m_bs_hash(_Keyval.contained_basic_string());
			return retval;
		}

		hash<basic_string_t> m_bs_hash;
	};

	template<class _Elem, class _Traits, class _Alloc, class _TStateMutex, template<typename> class _TTXScopeConstIterator>
	inline basic_istream<_Elem, _Traits>& getline(basic_istream<_Elem, _Traits>&& _Istr,
		mse::us::impl::gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator>& _Str, const _Elem _Delim) {	// get characters into string, discard delimiter
		return _Str.getline(MSE_FWD(_Istr), &_Str, _Delim);
	}
	template<class _Elem, class _Traits, class _Alloc, class _TStateMutex, template<typename> class _TTXScopeConstIterator>
	inline basic_istream<_Elem, _Traits>& getline(basic_istream<_Elem, _Traits>&& _Istr,
		mse::us::impl::gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator>& _Str) {	// get characters into string, discard newline
		return _Str.getline(MSE_FWD(_Istr), &_Str);
	}
	template<class _Elem, class _Traits, class _Alloc, class _TStateMutex, template<typename> class _TTXScopeConstIterator>
	inline basic_istream<_Elem, _Traits>& getline(basic_istream<_Elem, _Traits>& _Istr,
		mse::us::impl::gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator>& _Str, const _Elem _Delim) {	// get characters into string, discard delimiter
		return _Str.getline(_Istr, &_Str, _Delim);
	}
	template<class _Elem, class _Traits, class _Alloc, class _TStateMutex, template<typename> class _TTXScopeConstIterator>
	inline basic_istream<_Elem, _Traits>& getline(basic_istream<_Elem, _Traits>& _Istr,
		mse::us::impl::gnii_basic_string<_Elem, _Traits, _Alloc, _TStateMutex, _TTXScopeConstIterator>& _Str) {	// get characters into string, discard newline
		return _Str.getline(_Istr, &_Str);
	}

	template<class _Ty, class _Traits, class _A, class _TStateMutex, template<typename> class _TTXScopeConstIterator>
	void swap(mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>& _Left, mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>& _Right) _NOEXCEPT_OP(_NOEXCEPT_OP(_Left.swap(_Right)))
	{	// swap basic_strings
		return (_Left.swap(_Right));
	}
	template<class _Ty, class _Traits, class _A, class _TStateMutex, template<typename> class _TTXScopeConstIterator
		, class _TStateMutex2, template<typename> class _TTXScopeConstIterator2, MSE_IMPL_EIP mse::impl::enable_if_t<!std::is_same<_TStateMutex, _TStateMutex2>::value> MSE_IMPL_EIS >
	void swap(mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>& _Left, mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex2, _TTXScopeConstIterator2>& _Right) _NOEXCEPT_OP(_NOEXCEPT_OP(_Left.swap(_Right)))
	{	// swap basic_strings
		return (_Left.swap(_Right));
	}
	template<class _Ty, class _Traits, class _A, class _TStateMutex, template<typename> class _TTXScopeConstIterator>
	void swap(basic_string<_Ty, _Traits, _A>& _Left, mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>& _Right) _NOEXCEPT_OP(_NOEXCEPT_OP(_Right.swap(_Left)))
	{	// swap basic_strings
		return (_Right.swap(_Left));
	}
	template<class _Ty, class _Traits, class _A, class _TStateMutex, template<typename> class _TTXScopeConstIterator>
	void swap(mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex, _TTXScopeConstIterator>& _Left, basic_string<_Ty, _Traits, _A>& _Right) _NOEXCEPT_OP(_NOEXCEPT_OP(_Left.swap(_Right)))
	{
		return (_Left.swap(_Right));
	}
}

namespace mse {

	/* These are some defines that automate the boilerplate of inheriting free operators of a string base class. */

#define MSE_STRING_INHERIT_FREE_STREAM_OPERATORS(template_param_decl, derived_class, base_class, element_type, trait_type) \
	template_param_decl \
	std::basic_istream<element_type, trait_type>& operator>>(std::basic_istream<element_type, trait_type>&& _Istr, derived_class& _Str) { \
		return MSE_FWD(_Istr) >> mse::us::impl::as_ref<base_class>(_Str); \
	} \
	template_param_decl \
	std::basic_istream<element_type, trait_type>& operator>>(std::basic_istream<element_type, trait_type>& _Istr, derived_class& _Str) { \
		return _Istr >> mse::us::impl::as_ref<base_class>(_Str); \
	} \
	template_param_decl \
	std::basic_ostream<element_type, trait_type>& operator<<(std::basic_ostream<element_type, trait_type>& _Ostr, const derived_class& _Str) { \
		return _Ostr << mse::us::impl::as_ref<base_class>(_Str); \
	}

#define MSE_STRING_DEFINE_FREE_REDUNDANT_COMPARISON_OPERATORS(template_param_decl, class1) \
	template_param_decl \
	inline bool operator!=(const class1& _Left, const class1& _Right) { return (!(_Left == _Right)); } \
	template_param_decl \
	inline bool operator>(const class1& _Left, const class1& _Right) { return (_Right < _Left); } \
	template_param_decl \
	inline bool operator<=(const class1& _Left, const class1& _Right) { return (!(_Right < _Left)); } \
	template_param_decl \
	inline bool operator>=(const class1& _Left, const class1& _Right) { return (!(_Left < _Right)); }

#define MSE_STRING_INHERIT_FREE_OPERATOR_PLUS1(template_param_decl, derived_class, base_class) \
	template_param_decl \
	inline derived_class operator+(const derived_class& _Left, const derived_class& _Right) { \
		return mse::us::impl::as_ref<base_class>(_Left) + mse::us::impl::as_ref<base_class>(_Right); \
	} \
	template_param_decl \
	inline derived_class operator+(const derived_class& _Left, derived_class&& _Right) { \
		return mse::us::impl::as_ref<base_class>(_Left) + mse::us::impl::as_ref<base_class>(MSE_FWD(_Right)); \
	} \
	template_param_decl \
	inline derived_class operator+(derived_class&& _Left, const derived_class& _Right) { \
		return mse::us::impl::as_ref<base_class>(MSE_FWD(_Left)) + mse::us::impl::as_ref<base_class>(_Right); \
	} \
	template_param_decl \
	inline derived_class operator+(derived_class&& _Left, derived_class&& _Right) { \
		return mse::us::impl::as_ref<base_class>(MSE_FWD(_Left)) + mse::us::impl::as_ref<base_class>(MSE_FWD(_Right)); \
	}

#define MSE_STRING_INHERIT_FREE_OPERATOR_PLUS2(template_param_decl, derived_class, base_class, element_type) \
	template_param_decl \
	inline derived_class operator+(const element_type * const _Left, const derived_class& _Right) { \
		return _Left + mse::us::impl::as_ref<base_class>(_Right); \
	} \
	template_param_decl \
	inline derived_class operator+(const element_type _Left, const derived_class& _Right) { \
		return _Left + mse::us::impl::as_ref<base_class>(_Right); \
	} \
	template_param_decl \
	inline derived_class operator+(const derived_class& _Left, const element_type * const _Right) { \
		return mse::us::impl::as_ref<base_class>(_Left) + _Right; \
	} \
	template_param_decl \
	inline derived_class operator+(const derived_class& _Left, const element_type _Right) { \
		return mse::us::impl::as_ref<base_class>(_Left) + _Right; \
	} \
	template_param_decl \
	inline derived_class operator+(const element_type * const _Left, derived_class&& _Right) { \
		return _Left + mse::us::impl::as_ref<base_class>(MSE_FWD(_Right)); \
	} \
	template_param_decl \
	inline derived_class operator+(const element_type _Left, derived_class&& _Right) { \
		return _Left + mse::us::impl::as_ref<base_class>(MSE_FWD(_Right)); \
	} \
	template_param_decl \
	inline derived_class operator+(derived_class&& _Left, const element_type * const _Right) { \
		return mse::us::impl::as_ref<base_class>(MSE_FWD(_Left)) + _Right; \
	} \
	template_param_decl \
	inline derived_class operator+(derived_class&& _Left, const element_type _Right) { \
		return mse::us::impl::as_ref<base_class>(MSE_FWD(_Left)) + _Right; \
	}

#define MSE_STRING_TEMPLATE_PARAMS_DECL1 template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty> >
#define MSE_STRING_BASE_CLASS_FROM_TEMPLATE1(x, base_state_mutex_type, base_xs_const_iterator_template) x<_Ty, _Traits, _A, base_state_mutex_type, base_xs_const_iterator_template>
#define MSE_STRING_DERIVED_CLASS_FROM_TEMPLATE1(x) x<_Ty, _Traits, _A>

#define MSE_STRING_INHERIT_FREE_OPERATORS(derived_template, base_template, base_state_mutex_type, base_xs_const_iterator_template) \
	MSE_STRING_INHERIT_FREE_STREAM_OPERATORS(MSE_STRING_TEMPLATE_PARAMS_DECL1, MSE_STRING_DERIVED_CLASS_FROM_TEMPLATE1(derived_template), MSE_STRING_BASE_CLASS_FROM_TEMPLATE1(base_template, base_state_mutex_type, base_xs_const_iterator_template), _Ty, _Traits) \
	MSE_STRING_DEFINE_FREE_REDUNDANT_COMPARISON_OPERATORS(MSE_STRING_TEMPLATE_PARAMS_DECL1, MSE_STRING_DERIVED_CLASS_FROM_TEMPLATE1(derived_template)) \
	MSE_STRING_INHERIT_FREE_OPERATOR_PLUS1(MSE_STRING_TEMPLATE_PARAMS_DECL1, MSE_STRING_DERIVED_CLASS_FROM_TEMPLATE1(derived_template), MSE_STRING_BASE_CLASS_FROM_TEMPLATE1(base_template, base_state_mutex_type, base_xs_const_iterator_template)) \
	MSE_STRING_INHERIT_FREE_OPERATOR_PLUS2(MSE_STRING_TEMPLATE_PARAMS_DECL1, MSE_STRING_DERIVED_CLASS_FROM_TEMPLATE1(derived_template), MSE_STRING_BASE_CLASS_FROM_TEMPLATE1(base_template, base_state_mutex_type, base_xs_const_iterator_template), _Ty)


	/* mtnii_basic_string<> is a string that is eligible to be shared among threads and does not support implicit
	iterators. */
	template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty> >
	using mtnii_basic_string = mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, mse::shareable_dynamic_container_mutex, mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_cslsstrong_const_iterator_type>;

	namespace impl {
		namespace ns_mtnii_basic_string {
			/* While an instance of xscope_structure_lock_guard exists it ensures that direct (scope) pointers to
			individual elements in the basic_string do not become invalid by preventing any operation that might resize the basic_string
			or increase its capacity. Any attempt to execute such an operation would result in an exception. */
			/* The following xscope_const_structure_lock_guard constructed from a const reference is thread safe because
			mtnii_basic_string<> is uses an atomic "state mutex". */
			template<class _TContainer>
			using xscope_const_structure_lock_guard = mse::us::impl::ns_gnii_basic_string::xscope_const_structure_lock_guard<typename _TContainer::base_class>;

			template<class _TContainer>
			using xscope_structure_lock_guard = mse::impl::ns_gnii_basic_string::xscope_structure_lock_guard<typename _TContainer::base_class>;
		}
	}

	/* While an instance of xscope_structure_lock_guard exists it ensures that direct (scope) pointers to
	individual elements in the basic_string do not become invalid by preventing any operation that might resize the basic_string
	or increase its capacity. Any attempt to execute such an operation would result in an exception. */
	/* The returned xscope_structure_lock_guard constructed from a const reference is only safe because
	mtnii_basic_string<> is not eligible to be shared between threads. */
	template<class _Ty, class _Traits, class _A>
	auto make_xscope_structure_lock_guard(const mse::TXScopeObjFixedConstPointer<mtnii_basic_string<_Ty, _Traits, _A> >& owner_ptr) {
		return mse::impl::ns_mtnii_basic_string::xscope_const_structure_lock_guard<mtnii_basic_string<_Ty, _Traits, _A> >(owner_ptr);
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, class _Traits, class _A>
	auto make_xscope_structure_lock_guard(const mse::TXScopeFixedConstPointer<mtnii_basic_string<_Ty, _Traits, _A> >& owner_ptr) {
		return mse::impl::ns_mtnii_basic_string::xscope_const_structure_lock_guard<mtnii_basic_string<_Ty, _Traits, _A> >(owner_ptr);
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	template<class _Ty, class _Traits, class _A>
	auto make_xscope_structure_lock_guard(const mse::TXScopeObjFixedPointer<mtnii_basic_string<_Ty, _Traits, _A> >& owner_ptr) {
		return mse::impl::ns_mtnii_basic_string::xscope_structure_lock_guard<mtnii_basic_string<_Ty, _Traits, _A> >(owner_ptr);
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, class _Traits, class _A>
	auto make_xscope_structure_lock_guard(const mse::TXScopeFixedPointer<mtnii_basic_string<_Ty, _Traits, _A> >& owner_ptr) {
		return mse::impl::ns_mtnii_basic_string::xscope_structure_lock_guard<mtnii_basic_string<_Ty, _Traits, _A> >(owner_ptr);
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	MSE_STRING_INHERIT_FREE_OPERATORS(mtnii_basic_string, mse::us::impl::gnii_basic_string, mse::shareable_dynamic_container_mutex, mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_cslsstrong_const_iterator_type)

	using mtnii_string = mtnii_basic_string<char>;
	using mtnii_wstring = mtnii_basic_string<wchar_t>;
	using mtnii_u16string = mtnii_basic_string<char16_t>;
	using mtnii_u32string = mtnii_basic_string<char32_t>;


	/* stnii_basic_string<> is a "low-overhead" string that is not eligible to be shared among threads and does not
	support implicit iterators. */
	template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty> >
	class stnii_basic_string : public mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, mse::non_thread_safe_shared_mutex, mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_cslsstrong_const_iterator_type>, public us::impl::AsyncNotShareableTagBase {
	public:
		typedef mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, mse::non_thread_safe_shared_mutex, mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_cslsstrong_const_iterator_type> base_class;
		typedef mse::non_thread_safe_shared_mutex _TStateMutex;

		typedef typename base_class::allocator_type allocator_type;
		MSE_INHERITED_RANDOM_ACCESS_MEMBER_TYPE_DECLARATIONS(base_class);

		typedef typename base_class::iterator iterator;
		typedef typename base_class::const_iterator const_iterator;
		typedef typename base_class::reverse_iterator reverse_iterator;
		typedef typename base_class::const_reverse_iterator const_reverse_iterator;

		template<typename _TBasicStringConstPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_potentially_not_xscope<_TBasicStringConstPointer>::value)> MSE_IMPL_EIS >
		using Tss_const_iterator_type = typename base_class::template Tss_const_iterator_type<_TBasicStringConstPointer>;
		template<typename _TBasicStringPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_potentially_not_xscope<_TBasicStringPointer>::value)> MSE_IMPL_EIS >
		using Tss_iterator_type = typename base_class::template Tss_iterator_type<_TBasicStringPointer>;
		template<typename _TBasicStringPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_potentially_not_xscope<_TBasicStringPointer>::value)> MSE_IMPL_EIS >
		using Tss_reverse_iterator_type = typename base_class::template Tss_reverse_iterator_type<_TBasicStringPointer>;
		template<typename _TBasicStringConstPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_potentially_not_xscope<_TBasicStringConstPointer>::value)> MSE_IMPL_EIS >
		using Tss_const_reverse_iterator_type = typename base_class::template Tss_const_reverse_iterator_type<_TBasicStringConstPointer>;
		typedef typename base_class::ss_iterator_type ss_iterator_type;
		typedef typename base_class::ss_const_iterator_type ss_const_iterator_type;
		typedef typename base_class::ss_reverse_iterator_type ss_reverse_iterator_type;
		typedef typename base_class::ss_const_reverse_iterator_type ss_const_reverse_iterator_type;

		typedef typename base_class::xscope_ss_const_iterator_type xscope_ss_const_iterator_type;
		typedef typename base_class::xscope_ss_iterator_type xscope_ss_iterator_type;

		//typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_cslsstrong_const_iterator_type<_Myt> xscope_const_iterator;
		typedef typename base_class::xscope_const_iterator xscope_const_iterator;
		typedef typename base_class::xscope_iterator xscope_iterator;

		MSE_USING(stnii_basic_string, base_class);

		stnii_basic_string(_XSTD initializer_list<value_type> _Ilist, const _A& _Al = _A()) : base_class(_Ilist, _Al) {}
	};

	namespace impl {
		namespace ns_stnii_basic_string {
			/* While an instance of xscope_structure_lock_guard exists it ensures that direct (scope) pointers to
			individual elements in the basic_string do not become invalid by preventing any operation that might resize the basic_string
			or increase its capacity. Any attempt to execute such an operation would result in an exception. */
			/* The following xscope_const_structure_lock_guard constructed from a const reference is only safe because
			stnii_basic_string<> is not eligible to be shared between threads. */
			template<class _TContainer>
			using xscope_const_structure_lock_guard = mse::us::impl::ns_gnii_basic_string::xscope_const_structure_lock_guard<typename _TContainer::base_class>;

			template<class _TContainer>
			using xscope_structure_lock_guard = mse::impl::ns_gnii_basic_string::xscope_structure_lock_guard<typename _TContainer::base_class>;
		}
	}

	/* While an instance of xscope_structure_lock_guard exists it ensures that direct (scope) pointers to
	individual elements in the basic_string do not become invalid by preventing any operation that might resize the basic_string
	or increase its capacity. Any attempt to execute such an operation would result in an exception. */
	/* The returned xscope_structure_lock_guard constructed from a const reference is only safe because
	stnii_basic_string<> is not eligible to be shared between threads. */
	template<class _Ty, class _Traits, class _A>
	auto make_xscope_structure_lock_guard(const mse::TXScopeObjFixedConstPointer<stnii_basic_string<_Ty, _Traits, _A> >& owner_ptr) {
		return mse::impl::ns_stnii_basic_string::xscope_const_structure_lock_guard<stnii_basic_string<_Ty, _Traits, _A> >(owner_ptr);
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, class _Traits, class _A>
	auto make_xscope_structure_lock_guard(const mse::TXScopeFixedConstPointer<stnii_basic_string<_Ty, _Traits, _A> >& owner_ptr) {
		return mse::impl::ns_stnii_basic_string::xscope_const_structure_lock_guard<stnii_basic_string<_Ty, _Traits, _A> >(owner_ptr);
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	template<class _Ty, class _Traits, class _A>
	auto make_xscope_structure_lock_guard(const mse::TXScopeObjFixedPointer<stnii_basic_string<_Ty, _Traits, _A> >& owner_ptr) {
		return mse::impl::ns_stnii_basic_string::xscope_structure_lock_guard<stnii_basic_string<_Ty, _Traits, _A> >(owner_ptr);
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, class _Traits, class _A>
	auto make_xscope_structure_lock_guard(const mse::TXScopeFixedPointer<stnii_basic_string<_Ty, _Traits, _A> >& owner_ptr) {
		return mse::impl::ns_stnii_basic_string::xscope_structure_lock_guard<stnii_basic_string<_Ty, _Traits, _A> >(owner_ptr);
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	MSE_STRING_INHERIT_FREE_OPERATORS(stnii_basic_string, mse::us::impl::gnii_basic_string, mse::non_thread_safe_shared_mutex, mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_cslsstrong_const_iterator_type)

	using stnii_string = stnii_basic_string<char>;
	using stnii_wstring = stnii_basic_string<wchar_t>;
	using stnii_u16string = stnii_basic_string<char16_t>;
	using stnii_u32string = stnii_basic_string<char32_t>;



	template<class _Ty, class _Traits, class _A>
	class fixed_nii_basic_string;
	template<class _Ty, class _Traits, class _A>
	class xscope_fixed_nii_basic_string;

	namespace us {
		namespace impl {
			template<class _Ty, class _Traits, class _A>
			class fixed_nii_basic_string_base;

			namespace impl {
				namespace ns_fixed_nii_basic_string_base {
					template<class _Ty, class _Traits/* = std::char_traits<_Ty>*/, class _A/* = std::allocator<_Ty>*/>
					std::basic_ostream<_Ty, _Traits>& out_to_stream(std::basic_ostream<_Ty, _Traits>& _Ostr, const fixed_nii_basic_string_base<_Ty, _Traits, _A>& _Str);
				}
			}

			template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty> >
			class fixed_nii_basic_string_base : private mse::impl::TOpaqueWrapper<std::basic_string<_Ty, _Traits, _A> >, private container_adjusted_default_state_mutex<_Ty>, public us::impl::ContiguousSequenceStaticStructureContainerTagBase {
			public:
#ifdef MSE_HAS_CXX17
				template<class _StringViewIsh>
				using _is_string_view_ish = typename mse::nii_basic_string<_Ty>::template _is_string_view_ish<_StringViewIsh>;
				template<class _StringViewIsh>
				using _Is_string_view_ish = typename mse::nii_basic_string<_Ty>::template _Is_string_view_ish<_StringViewIsh>;

				template<class _StringViewIsh>
				using _is_string_view_or_section_ish = typename mse::nii_basic_string<_Ty>::template _is_string_view_or_section_ish<_StringViewIsh>;
				template<class _StringViewIsh>
				using _Is_string_view_or_section_ish = typename mse::nii_basic_string<_Ty>::template _Is_string_view_or_section_ish<_StringViewIsh>;
#endif /* MSE_HAS_CXX17 */

				typedef container_adjusted_default_state_mutex<_Ty> state_mutex_t;
				typedef state_mutex_t _TStateMutex;

				/* We (privately) inherit the underlying data type rather than make it a data member to ensure it's the "first" component in the structure.*/
				typedef mse::impl::TOpaqueWrapper<std::basic_string<_Ty, _Traits, _A> > base_class;
				typedef std::basic_string<_Ty, _Traits, _A> std_basic_string;
				typedef std_basic_string _MBS;
				typedef fixed_nii_basic_string_base _Myt;

				typedef typename std_basic_string::allocator_type allocator_type;
				typedef typename std_basic_string::value_type value_type;
				//typedef typename std_basic_string::size_type size_type;
				typedef msev_size_t size_type;
				//typedef typename std_basic_string::difference_type difference_type;
				typedef msev_int difference_type;
				typedef typename std_basic_string::pointer pointer;
				typedef typename std_basic_string::const_pointer const_pointer;
				typedef typename std_basic_string::reference reference;
				typedef typename std_basic_string::const_reference const_reference;
				static const size_t npos = size_t(-1);

				typedef typename std_basic_string::iterator iterator;
				typedef typename std_basic_string::const_iterator const_iterator;

				typedef typename std_basic_string::reverse_iterator reverse_iterator;
				typedef typename std_basic_string::const_reverse_iterator const_reverse_iterator;

			private:
				const _MBS& contained_basic_string() const& { return (*this).value(); }
				const _MBS& contained_basic_string() const&& { return (*this).value(); }
				_MBS& contained_basic_string()& { return (*this).value(); }
				_MBS&& contained_basic_string()&& { return std::move(*this).value(); }

				state_mutex_t& state_mutex1()& { return (*this); }

			public:
				explicit fixed_nii_basic_string_base(const _A& _Al = _A()) : base_class(_Al) { /*m_debug_size = size();*/ }
				//explicit fixed_nii_basic_string_base(size_type _N) : base_class(msev_as_a_size_t(_N)) { /*m_debug_size = size();*/ }
#ifdef MSE_HAS_CXX17
				template<class _Alloc2 = _A, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc2>::value, int> = 0>
#endif /* MSE_HAS_CXX17 */
				explicit fixed_nii_basic_string_base(size_type _N, const _Ty& _V, const _A& _Al = _A()) : base_class(msev_as_a_size_t(_N), _V, _Al) { /*m_debug_size = size();*/ }
				fixed_nii_basic_string_base(std_basic_string&& _X) : base_class(MSE_FWD(_X)) { /*m_debug_size = size();*/ }
				fixed_nii_basic_string_base(const std_basic_string& _X) : base_class(_X) { /*m_debug_size = size();*/ }
				/* This move constructor has to copy since the source is not allowed to be resized. */
				fixed_nii_basic_string_base(_Myt&& _X) : base_class(_X.contained_basic_string()) { /*m_debug_size = size();*/ }
				fixed_nii_basic_string_base(const _Myt& _X) : base_class(_X.contained_basic_string()) { /*m_debug_size = size();*/ }
				typedef typename std_basic_string::const_iterator _It;
				/* Note that safety cannot be guaranteed when using these constructors that take unsafe typename base_class::iterator and/or pointer parameters. */
				fixed_nii_basic_string_base(_It _F, _It _L, const _A& _Al = _A()) : base_class(_F, _L, _Al) { /*m_debug_size = size();*/ }
				fixed_nii_basic_string_base(const _Ty* _F, const _Ty* _L, const _A& _Al = _A()) : base_class(_F, _L, _Al) { /*m_debug_size = size();*/ }
				template<class _Iter
					//, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value> MSE_IMPL_EIS
					, class = mse::impl::_mse_RequireInputIter<_Iter> >
					fixed_nii_basic_string_base(const _Iter& _First, const _Iter& _Last) : base_class(_First, _Last) { /*m_debug_size = size();*/ }
				template<class _Iter
					//, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value> MSE_IMPL_EIS
					, class = mse::impl::_mse_RequireInputIter<_Iter> >
					//fixed_nii_basic_string_base(const _Iter& _First, const _Iter& _Last, const typename std_basic_string::_Alloc& _Al) : base_class(_First, _Last, _Al) { /*m_debug_size = size();*/ }
					fixed_nii_basic_string_base(const _Iter& _First, const _Iter& _Last, const _A& _Al) : base_class(_First, _Last, _Al) { /*m_debug_size = size();*/ }
				fixed_nii_basic_string_base(const _Ty* const _Ptr) : base_class(_Ptr) { /*m_debug_size = size();*/ }
				fixed_nii_basic_string_base(const _Ty* const _Ptr, const size_t _Count) : base_class(_Ptr, mse::msev_as_a_size_t(_Count)) { /*m_debug_size = size();*/ }
				fixed_nii_basic_string_base(const _Myt& _X, const size_type _Roff, const _A& _Al = _A()) : base_class(_X.contained_basic_string(), _Roff, npos, _Al) { /*m_debug_size = size();*/ }
				fixed_nii_basic_string_base(const _Myt& _X, const size_type _Roff, const size_type _Count, const _A& _Al = _A()) : base_class(_X.contained_basic_string(), _Roff, _Count, _Al) { /*m_debug_size = size();*/ }

				fixed_nii_basic_string_base(const mse::TXScopeFixedConstPointer<_Myt>& xs_ptr) : base_class(xs_ptr->contained_basic_string()) { /*m_debug_size = size();*/ }
				fixed_nii_basic_string_base(const mse::TXScopeFixedConstPointer<_Myt>& xs_ptr, const size_type _Roff, const _A& _Al = _A()) : base_class(xs_ptr->contained_basic_string(), _Roff, npos, _Al) { /*m_debug_size = size();*/ }
				fixed_nii_basic_string_base(const mse::TXScopeFixedConstPointer<_Myt>& xs_ptr, const size_type _Roff, const size_type _Count, const _A& _Al = _A()) : base_class(xs_ptr->contained_basic_string(), _Roff, _Count, _Al) { /*m_debug_size = size();*/ }

#ifdef MSE_HAS_CXX17
				template<class _TParam1, class = _Is_string_view_or_section_ish<_TParam1> >
				fixed_nii_basic_string_base(const _TParam1& _Right) { assign(_Right); }

				template<class _TParam1, class = _Is_string_view_or_section_ish<_TParam1> >
				fixed_nii_basic_string_base(const _TParam1& _Right, const size_type _Roff, const size_type _Count, const _A& _Al = _A())
					: base_class(_Al) {
					assign(_Right, _Roff, _Count);
				}
#else /* MSE_HAS_CXX17 */
				template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
				explicit fixed_nii_basic_string_base(const _TStringSection& _X) : base_class(_X.cbegin(), _X.cend()) { /*m_debug_size = size();*/ }
#endif /* MSE_HAS_CXX17 */

				/* The returned basic_string should be a copy (rather than a move) even when this is an rvalue, as this basic_string is not allowed to be resized. */
				operator _MBS() const { return contained_basic_string(); }

				typename std_basic_string::const_reference operator[](msev_size_t _P) const {
					return (*this).at(msev_as_a_size_t(_P));
				}
				typename std_basic_string::reference operator[](msev_size_t _P) {
					return (*this).at(msev_as_a_size_t(_P));
				}
				typename std_basic_string::reference front() {	// return first element of mutable sequence
					if (0 == (*this).size()) { MSE_THROW(gnii_basic_string_range_error("front() on empty - typename std_basic_string::reference front() - fixed_nii_basic_string_base")); }
					return contained_basic_string().front();
				}
				typename std_basic_string::const_reference front() const {	// return first element of nonmutable sequence
					if (0 == (*this).size()) { MSE_THROW(gnii_basic_string_range_error("front() on empty - typename std_basic_string::const_reference front() - fixed_nii_basic_string_base")); }
					return contained_basic_string().front();
				}
				typename std_basic_string::reference back() {	// return last element of mutable sequence
					if (0 == (*this).size()) { MSE_THROW(gnii_basic_string_range_error("back() on empty - typename std_basic_string::reference back() - fixed_nii_basic_string_base")); }
					return contained_basic_string().back();
				}
				typename std_basic_string::const_reference back() const {	// return last element of nonmutable sequence
					if (0 == (*this).size()) { MSE_THROW(gnii_basic_string_range_error("back() on empty - typename std_basic_string::const_reference back() - fixed_nii_basic_string_base")); }
					return contained_basic_string().back();
				}

				size_type length() const _NOEXCEPT
				{	// return length of sequence
					return contained_basic_string().length();
				}

				size_type size() const _NOEXCEPT
				{	// return length of sequence
					return contained_basic_string().size();
				}

				bool empty() const _NOEXCEPT
				{	// test if sequence is empty
					return contained_basic_string().empty();
				}
				_A get_allocator() const _NOEXCEPT
				{	// return allocator object for values
					return contained_basic_string().get_allocator();
				}

				reference at(msev_size_t _Pos)
				{	// subscript mutable sequence with checking
					return contained_basic_string().at(msev_as_a_size_t(_Pos));
				}

				const_reference at(msev_size_t _Pos) const
				{	// subscript nonmutable sequence with checking
					return contained_basic_string().at(msev_as_a_size_t(_Pos));
				}

				fixed_nii_basic_string_base(_XSTD initializer_list<typename std_basic_string::value_type> _Ilist, const _A& _Al = _A())
					: base_class(_Ilist, _Al) {	// construct from initializer_list
					/*m_debug_size = size();*/
				}

				value_type* data() _NOEXCEPT
				{	// return pointer to mutable data basic_string
					return contained_basic_string().data();
				}

				const value_type* data() const _NOEXCEPT
				{	// return pointer to nonmutable data basic_string
					return contained_basic_string().data();
				}

				size_type max_size() const _NOEXCEPT
				{	// return maximum possible length of sequence
					return contained_basic_string().max_size();
				}

			private:
				_Myt& operator=(const std_basic_string& _X) {
					//structure_change_guard<decltype(state_mutex1())> lock1(state_mutex1());
					contained_basic_string().operator =(_X);
					/*m_debug_size = size();*/
					return (*this);
				}
				_Myt& operator=(_Myt&& _X) {
					if (std::addressof(_X) == this) { return (*this); }
					//structure_change_guard<decltype(state_mutex1())> lock1(state_mutex1());
					contained_basic_string().operator=(MSE_FWD(_X).contained_basic_string());
					return (*this);
				}
				_Myt& operator=(const _Myt& _X) {
					if (std::addressof(_X) == this) { return (*this); }
					//structure_change_guard<decltype(state_mutex1())> lock1(state_mutex1());
					contained_basic_string().operator=(_X.contained_basic_string());
					return (*this);
				}

				void reserve(size_type _Count)
				{	// determine new minimum length of allocated storage
					//structure_change_guard<decltype(state_mutex1())> lock1(state_mutex1());
					contained_basic_string().reserve(msev_as_a_size_t(_Count));
				}
				size_type capacity() const _NOEXCEPT
				{	// return current length of allocated storage
					return contained_basic_string().capacity();
				}
				void shrink_to_fit() {	// reduce capacity
					//structure_change_guard<decltype(state_mutex1())> lock1(state_mutex1());
					contained_basic_string().shrink_to_fit();
				}
				void resize(size_type _N, const _Ty& _X = _Ty()) {
					//structure_change_guard<decltype(state_mutex1())> lock1(state_mutex1());
					contained_basic_string().resize(msev_as_a_size_t(_N), _X);
				}

				void push_back(_Ty&& _X) {
					//structure_change_guard<decltype(state_mutex1())> lock1(state_mutex1());
					contained_basic_string().push_back(MSE_FWD(_X));
				}
				void push_back(const _Ty& _X) {
					//structure_change_guard<decltype(state_mutex1())> lock1(state_mutex1());
					contained_basic_string().push_back(_X);
				}
				void pop_back() {
					//structure_change_guard<decltype(state_mutex1())> lock1(state_mutex1());
					contained_basic_string().pop_back();
				}


				_Myt& assign(mse::TXScopeFixedConstPointer<_Myt> xs_ptr) {
					//structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().assign(xs_ptr->contained_basic_string());
					/*m_debug_size = size();*/
					return (*this);
				}
				_Myt& assign(const _Myt& _Right) {
					auto xs_ptr = mse::us::unsafe_make_xscope_const_pointer_to(_Right);
					return assign(xs_ptr);
				}
				_Myt& assign(mse::TXScopeFixedConstPointer<_Myt> xs_ptr, const size_type _Roff, size_type _Count = npos) {
					//structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().assign(xs_ptr->contained_basic_string(), mse::as_a_size_t(_Roff), mse::as_a_size_t(_Count));
					/*m_debug_size = size();*/
					return (*this);
				}
				_Myt& assign(const _Myt& _Right, const size_type _Roff, size_type _Count = npos) {
					auto xs_ptr = mse::us::unsafe_make_xscope_const_pointer_to(_Right);
					return assign(xs_ptr, _Roff, _Count);
				}

				_Myt& assign(const _Ty* const _Ptr, const size_type _Count) {
					//structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().assign(_Ptr, mse::as_a_size_t(_Count));
					//m_debug_size = size();
					return (*this);
				}
				_Myt& assign(const _Ty* const _Ptr) {
					//structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().assign(_Ptr);
					//m_debug_size = size();
					return (*this);
				}
				_Myt& assign(const size_type _Count, const _Ty& _Ch) {
					//structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().assign(mse::as_a_size_t(_Count), _Ch);
					//m_debug_size = size();
					return (*this);
				}
				template<class _Iter, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::_mse_Is_iterator_v<_Iter> > MSE_IMPL_EIS >
				_Myt& assign(const _Iter _First, const _Iter _Last) {
					smoke_check_source_iterators(_First, _Last);
					//structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().assign(_First, _Last);
					//m_debug_size = size();
					return (*this);
				}

#ifdef MSE_HAS_CXX17
			private:
				template<class _StringViewIsh>
				_Myt& assign_helper2(std::true_type, const _StringViewIsh& _Right) {
					std::basic_string_view<_Ty, _Traits> _As_view = _Right;
					//structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().assign(_As_view);
					/*m_debug_size = size();*/
					return (*this);
				}
				template<class _TParam1>
				_Myt& assign_helper2(std::false_type, const _TParam1& _Right) {
					const auto xs_iters = mse::impl::make_xscope_range_iter_provider(_Right);
					return assign(xs_iters.begin(), xs_iters.end());
				}
				_Myt& assign_helper1(std::true_type, const _Myt& _Right) {
					assign(_Right);
					return (*this);
				}
				template<class _TParam1>
				_Myt& assign_helper1(std::false_type, const _TParam1& _Right) {
					return assign_helper2(typename _is_string_view_ish<_TParam1>::type(), _Right);
				}
			//public:
				template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
				_Myt& assign(const _TParam1& _Right) {
					return assign_helper1(typename std::is_base_of<_Myt, _TParam1>::type(), _Right);
				}

			private:
				template<class _StringViewIsh>
				_Myt& assign_helper2(std::true_type, const _StringViewIsh& _Right, const size_type _Roff, const size_type _Count) {
					std::basic_string_view<_Ty, _Traits> _As_view = _Right;
					//structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().assign(_As_view, mse::as_a_size_t(_Roff), mse::as_a_size_t(_Count));
					/*m_debug_size = size();*/
					return (*this);
				}
				template<class _TParam1>
				_Myt& assign_helper2(std::false_type, const _TParam1& _Right, const size_type _Roff, const size_type _Count) {
					auto adjusted_count = (npos == _Count) ? (difference_type(mse::container_size(_Right)) - difference_type(_Roff))
						: difference_type(_Count);
					if (0 > adjusted_count) { MSE_THROW(std::out_of_range(" _Myt::assign() ")); }
					const auto xs_iters = mse::impl::make_xscope_range_iter_provider(_Right);
					return assign(xs_iters.begin() + difference_type(_Roff), xs_iters.begin() + difference_type(_Roff + adjusted_count));
				}
				_Myt& assign_helper1(std::true_type, const _Myt& _Right, const size_type _Roff, const size_type _Count) {
					assign(_Right, _Roff, _Count);
					return (*this);
				}
				template<class _TParam1>
				_Myt& assign_helper1(std::false_type, const _TParam1& _Right, const size_type _Roff, const size_type _Count) {
					return assign_helper2(typename _is_string_view_ish<_TParam1>::type(), _Right, _Roff, _Count);
				}
			//public:
				template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
				_Myt& assign(const _TParam1& _Right, const size_type _Roff, const size_type _Count = npos) {
					return assign_helper1(typename std::is_base_of<_Myt, _TParam1>::type(), _Right, _Roff, _Count);
				}
#else /* MSE_HAS_CXX17 */
				template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
				_Myt& assign(const _TStringSection& _X) {
					//structure_change_guard<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					contained_basic_string().assign(_X.cbegin(), _X.cend());
					/*m_debug_size = size();*/
					return (*this);
				}
#endif /* MSE_HAS_CXX17 */


				template<class _Iter>
				static void smoke_check_source_iterators_helper(std::true_type, const _Iter& _First, const _Iter& _Last) {
					if (_Last < _First)/*comparison operations should also verify that safe iterators point to the same container*/ {
						MSE_THROW(gnii_basic_string_range_error("invalid arguments - void smoke_check_source_iterators() const - fixed_nii_basic_string_base"));
					}
				}
				template<class _Iter>
				static void smoke_check_source_iterators_helper(std::false_type, const _Iter&, const _Iter&) {}

				template<class _Iter>
				static void smoke_check_source_iterators(const fixed_nii_basic_string_base& target_cref, const _Iter& _First, const _Iter& _Last) {
#ifndef MSE_NII_VECTOR_SUPRESS_SOURCE_ITER_ALIAS_CHECK
					if (!(target_cref.empty())) {
						/* check if the source sequence is part of target (target) container */
						auto start_of_target_ptr = std::addressof(*(target_cref.cbegin()));
						auto end_of_target_ptr = std::addressof(*(target_cref.cend() - 1)) + 1;
						auto _First_ptr = std::addressof(*_First);
						if ((end_of_target_ptr > _First_ptr) && (start_of_target_ptr <= _First_ptr)) {
							MSE_THROW(gnii_basic_string_range_error("invalid arguments - void smoke_check_source_iterators() const - fixed_nii_basic_string_base"));
						}
					}
#endif // !MSE_NII_VECTOR_SUPRESS_SOURCE_ITER_ALIAS_CHECK

#ifdef MSE_NII_VECTOR_ENABLE_SOURCE_ITER_ORDER_CHECK
					smoke_check_source_iterators_helper(typename mse::impl::HasOrInheritsLessThanOperator_msemsebasic_string<_Iter>::type(), _First, _Last);
#endif // MSE_NII_VECTOR_ENABLE_SOURCE_ITER_ORDER_CHECK
				}
				template<class _Iter>
				void smoke_check_source_iterators(const _Iter& _First, const _Iter& _Last) {
					smoke_check_source_iterators(*this, _First, _Last);
				}

				template<class ..._Valty>
				void emplace_back(_Valty&& ..._Val)
				{	// insert by moving into element at end
					//structure_change_guard<decltype(state_mutex1())> lock1(state_mutex1());
					contained_basic_string().emplace_back(std::forward<_Valty>(_Val)...);
					/*m_debug_size = size();*/
				}
				void clear() {
					//structure_change_guard<decltype(state_mutex1())> lock1(state_mutex1());
					contained_basic_string().clear();
					/*m_debug_size = size();*/
				}

				void swap(_Myt& _Other) {	// swap contents with _Other
					//structure_change_guard<decltype(state_mutex1())> lock1(state_mutex1());
					contained_basic_string().swap(_Other.contained_basic_string());
				}
				void swap(_MBS& _Other) {	// swap contents with _Other
					//structure_change_guard<decltype(state_mutex1())> lock1(state_mutex1());
					contained_basic_string().swap(_Other);
				}
				/*
				template<typename _TStateMutex2, template<typename> class _TTXScopeConstIterator2>
				void swap(mse::us::impl::fixed_nii_basic_string_base<_Ty, _Traits, _A, _TStateMutex2, _TTXScopeConstIterator2>& _Other) {	// swap contents with _Other
					//structure_change_guard<decltype(state_mutex1())> lock1(state_mutex1());
					contained_basic_string().swap(_Other.contained_basic_string());
				}
				*/

				_Myt& operator=(_XSTD initializer_list<typename std_basic_string::value_type> _Ilist) {	// assign initializer_list
					contained_basic_string().operator=(static_cast<std_basic_string>(_Ilist));
					return (*this);
				}
				void assign(_XSTD initializer_list<typename std_basic_string::value_type> _Ilist) {	// assign initializer_list
					contained_basic_string().assign(_Ilist);
					/*m_debug_size = size();*/
				}
			public:

				//class nbs_const_iterator_base : public std::iterator<std::random_access_iterator_tag, value_type, difference_type, const_pointer, const_reference> {};
				//class nbs_iterator_base : public std::iterator<std::random_access_iterator_tag, value_type, difference_type, pointer, reference> {};

				typedef mse::impl::random_access_const_iterator_base<_Ty> nbs_const_iterator_base;
				typedef mse::impl::random_access_iterator_base<_Ty> nbs_iterator_base;

				template<typename _TVectorConstPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_potentially_not_xscope<_TVectorConstPointer>::value)> MSE_IMPL_EIS >
				using Tss_const_iterator_type = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_ss_const_iterator_type<_TVectorConstPointer>;
				template<typename _TVectorPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_potentially_not_xscope<_TVectorPointer>::value)> MSE_IMPL_EIS >
				using Tss_iterator_type = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_ss_iterator_type<_TVectorPointer>;

				template<typename _TVectorPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_potentially_not_xscope<_TVectorPointer>::value)> MSE_IMPL_EIS >
				using Tss_reverse_iterator_type = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_ss_reverse_iterator_type<_TVectorPointer>;
				template<typename _TVectorConstPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_potentially_not_xscope<_TVectorConstPointer>::value)> MSE_IMPL_EIS >
				using Tss_const_reverse_iterator_type = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_ss_const_reverse_iterator_type<_TVectorConstPointer>;

				typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_rp_ss_iterator_type<_Myt> ss_iterator_type;
				typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_rp_ss_const_iterator_type<_Myt> ss_const_iterator_type;
				typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_rp_ss_reverse_iterator_type<_Myt> ss_reverse_iterator_type;
				typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_rp_ss_const_reverse_iterator_type<_Myt> ss_const_reverse_iterator_type;

				/* Here we provide static versions of the member functions that return iterators. As static member functions do
				not have access to a "this" pointer, these functions require a pointer to the container to be passed as the first
				argument. Any returned iterator will contain a copy of the supplied pointer and inherit its safety properties. */

				template<typename _TVectorPointer>
				static auto ss_begin(const _TVectorPointer& owner_ptr) {
					mse::impl::T_valid_if_not_an_xscope_type<_TVectorPointer>();
					typedef mse::impl::conditional_t<std::is_const<mse::impl::remove_reference_t<decltype(*owner_ptr)> >::value
						, Tss_const_iterator_type<_TVectorPointer>, Tss_iterator_type<_TVectorPointer> > return_type;
					return_type retval(owner_ptr);
					retval.set_to_beginning();
					return retval;
				}

				template<typename _TVectorPointer>
				static auto ss_end(const _TVectorPointer& owner_ptr) {
					mse::impl::T_valid_if_not_an_xscope_type<_TVectorPointer>();
					typedef mse::impl::conditional_t<std::is_const<mse::impl::remove_reference_t<decltype(*owner_ptr)> >::value
						, Tss_const_iterator_type<_TVectorPointer>, Tss_iterator_type<_TVectorPointer> > return_type;
					return_type retval(owner_ptr);
					retval.set_to_end_marker();
					return retval;
				}

				template<typename _TVectorPointer>
				static auto ss_cbegin(const _TVectorPointer& owner_ptr) {
					mse::impl::T_valid_if_not_an_xscope_type<_TVectorPointer>();
					Tss_const_iterator_type<_TVectorPointer> retval(owner_ptr);
					retval.set_to_beginning();
					return retval;
				}

				template<typename _TVectorPointer>
				static auto ss_cend(const _TVectorPointer& owner_ptr) {
					mse::impl::T_valid_if_not_an_xscope_type<_TVectorPointer>();
					Tss_const_iterator_type<_TVectorPointer> retval(owner_ptr);
					retval.set_to_end_marker();
					return retval;
				}

				template<typename _TVectorPointer>
				static auto ss_rbegin(const _TVectorPointer& owner_ptr) {
					mse::impl::T_valid_if_not_an_xscope_type<_TVectorPointer>();
					typedef mse::impl::conditional_t<std::is_const<mse::impl::remove_reference_t<decltype(*owner_ptr)> >::value
						, Tss_const_reverse_iterator_type<_TVectorPointer>, Tss_reverse_iterator_type<_TVectorPointer> > return_type;
					return return_type(ss_end<_TVectorPointer>(owner_ptr));
				}

				template<typename _TVectorPointer>
				static auto ss_rend(const _TVectorPointer& owner_ptr) {
					mse::impl::T_valid_if_not_an_xscope_type<_TVectorPointer>();
					typedef mse::impl::conditional_t<std::is_const<mse::impl::remove_reference_t<decltype(*owner_ptr)> >::value
						, Tss_const_reverse_iterator_type<_TVectorPointer>, Tss_reverse_iterator_type<_TVectorPointer> > return_type;
					return return_type(ss_begin<_TVectorPointer>(owner_ptr));
				}

				template<typename _TVectorPointer>
				static auto ss_crbegin(const _TVectorPointer& owner_ptr) {
					mse::impl::T_valid_if_not_an_xscope_type<_TVectorPointer>();
					return (Tss_const_reverse_iterator_type<_TVectorPointer>(ss_cend<_TVectorPointer>(owner_ptr)));
				}

				template<typename _TVectorPointer>
				static auto ss_crend(const _TVectorPointer& owner_ptr) {
					mse::impl::T_valid_if_not_an_xscope_type<_TVectorPointer>();
					return (Tss_const_reverse_iterator_type<_TVectorPointer>(ss_crbegin<_TVectorPointer>(owner_ptr)));
				}

				template<typename _TVectorPointer1>
				static void s_assert_valid_index(const _TVectorPointer1& this_ptr, size_type index) {
					if ((*this_ptr).size() < index) { MSE_THROW(gnii_basic_string_range_error("invalid index - void assert_valid_index() const - fixed_nii_basic_string_base")); }
				}

				typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_ss_const_iterator_type<_Myt> xscope_ss_const_iterator_type;
				typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_ss_iterator_type<_Myt> xscope_ss_iterator_type;

				//typedef xscope_ss_const_iterator_type xscope_const_iterator;
				//typedef _TTXScopeConstIterator<_Myt> xscope_const_iterator;
				//typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_cslsstrong_const_iterator_type<_Myt> xscope_const_iterator;
				//typedef xscope_ss_iterator_type xscope_iterator;
				//typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_cslsstrong_iterator_type<_Myt> xscope_iterator;
				typedef TXScopeCSSSXSRAConstIterator<_Myt> xscope_const_iterator;
				typedef TXScopeCSSSXSRAIterator<_Myt> xscope_iterator;

				bool operator==(const _Myt& _Right) const {	// test for basic_string equality
					return (_Right.contained_basic_string() == contained_basic_string());
				}
				bool operator<(const _Myt& _Right) const {	// test if _Left < _Right for basic_strings
					return (contained_basic_string() < _Right.contained_basic_string());
				}


				int compare(mse::TXScopeFixedConstPointer<fixed_nii_basic_string_base> xs_ptr) const _NOEXCEPT {
					return contained_basic_string().compare((*xs_ptr).contained_basic_string());
				}
				int compare(const fixed_nii_basic_string_base& _Right) const _NOEXCEPT {
					return contained_basic_string().compare(_Right.contained_basic_string());
				}
				int compare(size_type _Off, size_type _N0, mse::TXScopeFixedConstPointer<fixed_nii_basic_string_base> xs_ptr) const {
					return contained_basic_string().compare(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), (*xs_ptr).contained_basic_string());
				}
				int compare(size_type _Off, size_type _N0, const fixed_nii_basic_string_base& _Right) const {
					return contained_basic_string().compare(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _Right.contained_basic_string());
				}
				int compare(const size_type _Off, const size_type _N0, mse::TXScopeFixedConstPointer<fixed_nii_basic_string_base> xs_ptr,
					const size_type _Roff, const size_type _Count = npos) const {
					return contained_basic_string().compare(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), (*xs_ptr).contained_basic_string(), _Roff, _Count);
				}
				int compare(const size_type _Off, const size_type _N0, const fixed_nii_basic_string_base& _Right,
					const size_type _Roff, const size_type _Count = npos) const {
					return contained_basic_string().compare(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _Right.contained_basic_string(), _Roff, _Count);
				}

				int compare(const _Ty* const _Ptr) const _NOEXCEPT {
					return contained_basic_string().compare(_Ptr);
				}

				int compare(const size_type _Off, const size_type _N0, const _Ty* const _Ptr) const {
					return contained_basic_string().compare(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _Ptr);
				}

				int compare(const size_type _Off, const size_type _N0, const _Ty* const _Ptr,
					const size_type _Count) const {
					return contained_basic_string().compare(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _Ptr, mse::msev_as_a_size_t(_Count));
				}

#ifdef MSE_HAS_CXX17
			private:
				template<class _StringViewIsh>
				int compare_helper2(std::true_type, const _StringViewIsh& _Right) const {
					std::basic_string_view<_Ty, _Traits> _As_view = _Right;
					return contained_basic_string().compare(_As_view);
				}
				template<class _TParam1>
				int compare_helper2(std::false_type, const _TParam1& _Right) const {
					const auto xs_iters = mse::impl::make_xscope_range_iter_provider(_Right);
					return std::lexicographical_compare(contained_basic_string().cbegin(), contained_basic_string().cend(), xs_iters.begin(), xs_iters.end());
					//return compare(fixed_nii_basic_string_base(xs_iters.begin(), xs_iters.end()));
				}
				int compare_helper1(std::true_type, const fixed_nii_basic_string_base& _Right) const {
					return compare(_Right);
				}
				template<class _TParam1>
				int compare_helper1(std::false_type, const _TParam1& _Right) const {
					return compare_helper2(typename _is_string_view_ish<_TParam1>::type(), _Right);
				}
			public:
				template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
				int compare(const _TParam1& _Right) const {
					return compare_helper1(typename std::is_base_of<fixed_nii_basic_string_base, _TParam1>::type(), _Right);
				}

			private:
				template<class _StringViewIsh>
				int compare_helper2(std::true_type, const size_type _Off, const size_type _N0, const _StringViewIsh& _Right) const {
					std::basic_string_view<_Ty, _Traits> _As_view = _Right;
					return contained_basic_string().compare(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _As_view);
				}
				template<class _TParam1>
				int compare_helper2(std::false_type, const size_type _Off, const size_type _N0, const _TParam1& _Right) const {
					const auto xs_iters = mse::impl::make_xscope_range_iter_provider(_Right);
					return compare(_Off, _N0, fixed_nii_basic_string_base(xs_iters.begin(), xs_iters.end()));
				}
				int compare_helper1(std::true_type, const size_type _Off, const size_type _N0, const fixed_nii_basic_string_base& _Right) const {
					return compare(_Off, _N0, _Right);
				}
				template<class _TParam1>
				int compare_helper1(std::false_type, const size_type _Off, const size_type _N0, const _TParam1& _Right) const {
					return compare_helper2(typename _is_string_view_ish<_TParam1>::type(), _Off, _N0, _Right);
				}
			public:
				template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
				int compare(const size_type _Off, const size_type _N0, const _TParam1& _Right) const {
					return compare_helper1(typename std::is_base_of<fixed_nii_basic_string_base, _TParam1>::type(), _Off, _N0, _Right);
				}

			private:
				template<class _StringViewIsh>
				int compare_helper2(std::true_type, const size_type _Off, const size_type _N0, const _StringViewIsh& _Right, const size_type _Roff, const size_type _Count) const {
					std::basic_string_view<_Ty, _Traits> _As_view = _Right;
					return contained_basic_string().compare(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), _As_view, mse::as_a_size_t(_Roff), mse::as_a_size_t(_Count));
				}
				template<class _TParam1>
				int compare_helper2(std::false_type, const size_type _Off, const size_type _N0, const _TParam1& _Right, const size_type _Roff, const size_type _Count) const {
					auto adjusted_count = (npos == _Count) ? (difference_type(mse::container_size(_Right)) - difference_type(_Roff))
						: difference_type(_Count);
					if (0 > adjusted_count) { MSE_THROW(std::out_of_range(" fixed_nii_basic_string_base::compare() ")); }
					const auto xs_iters = mse::impl::make_xscope_range_iter_provider(_Right);
					return compare(_Off, _N0, fixed_nii_basic_string_base(xs_iters.begin() + difference_type(_Roff), xs_iters.begin() + difference_type(_Roff + adjusted_count)));
				}
				int compare_helper1(std::true_type, const size_type _Off, const size_type _N0, const fixed_nii_basic_string_base& _Right, const size_type _Roff, const size_type _Count) const {
					return compare(_Off, _N0, _Right, _Roff, _Count);
				}
				template<class _TParam1>
				int compare_helper1(std::false_type, const size_type _Off, const size_type _N0, const _TParam1& _Right, const size_type _Roff, const size_type _Count) const {
					return compare_helper2(typename _is_string_view_ish<_TParam1>::type(), _Off, _N0, _Right, _Roff, _Count);
				}
			public:
				template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
				int compare(const size_type _Off, const size_type _N0, const _TParam1& _Right, const size_type _Roff, const size_type _Count = npos) const {
					return compare_helper1(typename std::is_base_of<fixed_nii_basic_string_base, _TParam1>::type(), _Off, _N0, _Right, _Roff, _Count);
				}
#else /* MSE_HAS_CXX17 */
				template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
				int compare(const size_type _Off, const size_type _N0, const _TStringSection& _X) const {
					return contained_basic_string().compare(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_N0), fixed_nii_basic_string_base(_X.cbegin(), _X.cend()));
				}
#endif /* MSE_HAS_CXX17 */

#ifdef MSE_HAS_CXX17
			private:
				template<class _StringViewIsh>
				size_type find_helper2(std::true_type, const _StringViewIsh& _Right, const size_type _Off = npos) const {
					std::basic_string_view<_Ty, _Traits> _As_view = _Right;
					return contained_basic_string().find(_As_view, mse::as_a_size_t(_Off));
				}
				template<class _TParam1>
				size_type find_helper2(std::false_type, const _TParam1& _Right, const size_type _Off = npos) const {
					const auto xs_iters = mse::impl::make_xscope_range_iter_provider(_Right);
					return find(fixed_nii_basic_string_base(xs_iters.begin(), xs_iters.end()), _Off);
				}
				size_type find_helper1(std::true_type, const fixed_nii_basic_string_base& _Right, const size_type _Off = npos) const {
					return find(_Right, _Off);
				}
				template<class _TParam1>
				size_type find_helper1(std::false_type, const _TParam1& _Right, const size_type _Off = npos) const {
					return find_helper2(typename _is_string_view_ish<_TParam1>::type(), _Right, _Off);
				}
			public:
				template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
				size_type find(const _TParam1& _Right, const size_type _Off = npos) const {
					return find_helper1(typename std::is_base_of<fixed_nii_basic_string_base, _TParam1>::type(), _Right, _Off);
				}
#else /* MSE_HAS_CXX17 */
				template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
				size_type find(const _TStringSection& _X, const size_type _Off = npos) const {
					return contained_basic_string().find(fixed_nii_basic_string_base(_X.cbegin(), _X.cend()), mse::as_a_size_t(_Off));
				}
#endif /* MSE_HAS_CXX17 */

				size_type find(const fixed_nii_basic_string_base& _Right, const size_type _Off = 0) const _NOEXCEPT {
					return contained_basic_string().find(_Right.contained_basic_string(), _Off);
				}

				size_type find(const _Ty* const _Ptr, const size_type _Off, const size_type _Count) const _NOEXCEPT {
					return contained_basic_string().find(_Ptr, _Off, _Count);
				}

				size_type find(const _Ty* const _Ptr, const size_type _Off = 0) const _NOEXCEPT {
					return contained_basic_string().find(_Ptr, _Off);
				}

				size_type find(const _Ty _Ch, const size_type _Off = 0) const _NOEXCEPT {
					return contained_basic_string().find(_Ch, _Off);
				}

#ifdef MSE_HAS_CXX17
			private:
				template<class _StringViewIsh>
				size_type rfind_helper2(std::true_type, const _StringViewIsh& _Right, const size_type _Off = npos) const {
					std::basic_string_view<_Ty, _Traits> _As_view = _Right;
					return contained_basic_string().rfind(_As_view, mse::as_a_size_t(_Off));
				}
				template<class _TParam1>
				size_type rfind_helper2(std::false_type, const _TParam1& _Right, const size_type _Off = npos) const {
					const auto xs_iters = mse::impl::make_xscope_range_iter_provider(_Right);
					return rfind(fixed_nii_basic_string_base(xs_iters.begin(), xs_iters.end()), _Off);
				}
				size_type rfind_helper1(std::true_type, const fixed_nii_basic_string_base& _Right, const size_type _Off = npos) const {
					return rfind(_Right, _Off);
				}
				template<class _TParam1>
				size_type rfind_helper1(std::false_type, const _TParam1& _Right, const size_type _Off = npos) const {
					return rfind_helper2(typename _is_string_view_ish<_TParam1>::type(), _Right, _Off);
				}
			public:
				template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
				size_type rfind(const _TParam1& _Right, const size_type _Off = npos) const {
					return rfind_helper1(typename std::is_base_of<fixed_nii_basic_string_base, _TParam1>::type(), _Right, _Off);
				}
#else /* MSE_HAS_CXX17 */
				template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
				size_type rfind(const _TStringSection& _X, const size_type _Off = npos) const {
					return contained_basic_string().rfind(fixed_nii_basic_string_base(_X.cbegin(), _X.cend()), mse::as_a_size_t(_Off));
				}
#endif /* MSE_HAS_CXX17 */

				size_type rfind(const fixed_nii_basic_string_base& _Right, const size_type _Off = npos) const _NOEXCEPT {
					return contained_basic_string().rfind(_Right.contained_basic_string(), _Off);
				}

				size_type rfind(const _Ty* const _Ptr, const size_type _Off, const size_type _Count) const _NOEXCEPT {
					return contained_basic_string().rfind(_Ptr, _Off, _Count);
				}

				size_type rfind(const _Ty* const _Ptr, const size_type _Off = npos) const _NOEXCEPT {
					return contained_basic_string().rfind(_Ptr, _Off);
				}

				size_type rfind(const _Ty _Ch, const size_type _Off = npos) const _NOEXCEPT {
					return contained_basic_string().rfind(_Ch, _Off);
				}

#if 0//_HAS_CXX17
				size_type find_first_of(const mstd::basic_string_view<_Ty, _Traits> _Right, const size_type _Off = 0) const _NOEXCEPT
				{	// look for one of _Right at or after _Off
					auto& _My_data = this->_Get_data();
					return (static_cast<size_type>(_Traits_find_first_of<_Traits>(_My_data._Myptr(), _My_data._Mysize, _Off,
						_Right.data(), _Right.size())));
				}
#endif /* _HAS_CXX17 */

				size_type find_first_of(const fixed_nii_basic_string_base& _Right, const size_type _Off = 0) const _NOEXCEPT {
					return contained_basic_string().find_first_of(_Right.contained_basic_string(), _Off);
				}

				size_type find_first_of(const _Ty* const _Ptr, const size_type _Off,
					const size_type _Count) const _NOEXCEPT {
					return contained_basic_string().find_first_of(_Ptr, _Off, _Count);
				}

				size_type find_first_of(const _Ty* const _Ptr, const size_type _Off = 0) const _NOEXCEPT {
					return contained_basic_string().find_first_of(_Ptr, _Off);
				}

				size_type find_first_of(const _Ty _Ch, const size_type _Off = 0) const _NOEXCEPT {
					return contained_basic_string().find_first_of(_Ch, _Off);
				}

#if 0//_HAS_CXX17
				size_type find_last_of(const mstd::basic_string_view<_Ty, _Traits> _Right, const size_type _Off = npos) const _NOEXCEPT
				{	// look for one of _Right before _Off
					auto& _My_data = this->_Get_data();
					return (static_cast<size_type>(_Traits_find_last_of<_Traits>(_My_data._Myptr(), _My_data._Mysize, _Off,
						_Right.data(), _Right.size())));
				}
#endif /* _HAS_CXX17 */

				size_type find_last_of(const fixed_nii_basic_string_base& _Right, size_type _Off = npos) const _NOEXCEPT {
					return contained_basic_string().find_last_of(_Right.contained_basic_string(), _Off);
				}

				size_type find_last_of(const _Ty* const _Ptr, const size_type _Off,
					const size_type _Count) const _NOEXCEPT {
					return contained_basic_string().find_last_of(_Ptr, _Off, _Count);
				}

				size_type find_last_of(const _Ty* const _Ptr, const size_type _Off = npos) const _NOEXCEPT {
					return contained_basic_string().find_last_of(_Ptr, _Off);
				}

				size_type find_last_of(const _Ty _Ch, const size_type _Off = npos) const _NOEXCEPT {
					return contained_basic_string().find_last_of(_Ch, _Off);
				}

#if 0//_HAS_CXX17
				size_type find_first_not_of(const mstd::basic_string_view<_Ty, _Traits> _Right, const size_type _Off = 0) const
					_NOEXCEPT
				{	// look for none of _Right at or after _Off
					auto& _My_data = this->_Get_data();
					return (static_cast<size_type>(_Traits_find_first_not_of<_Traits>(_My_data._Myptr(), _My_data._Mysize, _Off,
						_Right.data(), _Right.size())));
				}
#endif /* _HAS_CXX17 */

				size_type find_first_not_of(const fixed_nii_basic_string_base& _Right, const size_type _Off = 0) const _NOEXCEPT {
					return contained_basic_string().find_first_not_of(_Right.contained_basic_string(), _Off);
				}

				size_type find_first_not_of(const _Ty* const _Ptr, const size_type _Off,
					const size_type _Count) const _NOEXCEPT {
					return contained_basic_string().find_first_not_of(_Ptr, _Off, _Count);
				}

				size_type find_first_not_of(const _Ty* const _Ptr, size_type _Off = 0) const _NOEXCEPT {
					return contained_basic_string().find_first_not_of(_Ptr, _Off);
				}

				size_type find_first_not_of(const _Ty _Ch, const size_type _Off = 0) const _NOEXCEPT {
					return contained_basic_string().find_first_not_of(_Ch, _Off);
				}

#if 0//_HAS_CXX17
				size_type find_last_not_of(const mstd::basic_string_view<_Ty, _Traits> _Right, const size_type _Off = npos) const
					_NOEXCEPT
				{	// look for none of _Right before _Off
					auto& _My_data = this->_Get_data();
					return (static_cast<size_type>(_Traits_find_last_not_of<_Traits>(_My_data._Myptr(), _My_data._Mysize, _Off,
						_Right.data(), _Right.size())));
				}
#endif /* _HAS_CXX17 */

				size_type find_last_not_of(const fixed_nii_basic_string_base& _Right, const size_type _Off = npos) const _NOEXCEPT {
					return contained_basic_string().find_last_not_of(_Right.contained_basic_string(), _Off);
				}

				size_type find_last_not_of(const _Ty* const _Ptr, const size_type _Off,
					const size_type _Count) const _NOEXCEPT {
					return contained_basic_string().find_last_not_of(_Ptr, _Off, _Count);
				}

				size_type find_last_not_of(const _Ty* const _Ptr, const size_type _Off = npos) const _NOEXCEPT {
					return contained_basic_string().find_last_not_of(_Ptr, _Off);
				}

				size_type find_last_not_of(const _Ty _Ch, const size_type _Off = npos) const _NOEXCEPT {
					return contained_basic_string().find_last_not_of(_Ch, _Off);
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
						(*target_iter) = contained_basic_string()[mse::msev_as_a_size_t(i)];
						++target_iter;
					}
					return n;
				}

				nii_basic_string<_Ty, _Traits, _A> substr(const size_type _Off = 0, const size_type _Count = npos) const {
					//return (gnii_basic_string(*this, _Off, _Count, get_allocator()));
					return (nii_basic_string<_Ty, _Traits, _A>(contained_basic_string().substr(mse::msev_as_a_size_t(_Off), mse::msev_as_a_size_t(_Count))));
				}

				static std::basic_ostream<_Ty, _Traits>& out_to_stream(std::basic_ostream<_Ty, _Traits>& _Ostr, const _Myt& _Str) {
					return _Ostr << _Str.contained_basic_string();
				}

				template<class _Ty2, class _Traits2>
				std::basic_ostream<_Ty2, _Traits2>& write_bytes(std::basic_ostream<_Ty2, _Traits2>& _Ostr, size_type byte_count, const size_type byte_start_offset = 0) const {
					const auto array_size_in_bytes = mse::msev_as_a_size_t(sizeof(_Ty) * (*this).size());
					auto byte_ptr = reinterpret_cast<const char*>((*this).contained_basic_string().data());
					if ((array_size_in_bytes <= byte_start_offset) || (0 >= byte_count)) {
						return _Ostr;
					}
					else {
						byte_ptr += mse::msev_as_a_size_t(byte_start_offset);
						return _Ostr.write(byte_ptr, std::min(mse::msev_as_a_size_t(array_size_in_bytes - byte_start_offset), mse::msev_as_a_size_t(byte_count)));
					}
				}
				template<class _Ty2, class _Traits2>
				std::basic_ostream<_Ty2, _Traits2>& write_bytes(std::basic_ostream<_Ty2, _Traits2>& _Ostr) const {
					return write_bytes(_Ostr, mse::msev_as_a_size_t(sizeof(_Ty) * (*this).size()));
				}

				MSE_INHERIT_XSCOPE_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(_Ty);

			private:

				/* If _Ty is an xscope type, then the following member function will not instantiate, causing an
				(intended) compile error. */
				template<class _Ty2 = _Ty, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _Ty>::value) && (mse::impl::is_potentially_not_xscope<_Ty2>::value)> MSE_IMPL_EIS >
				void valid_if_Ty_is_not_an_xscope_type() const {}

				template<class _Ty2 = _Ty, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _Ty>::value) && (!std::is_same<bool, _Ty2>::value)> MSE_IMPL_EIS >
				void valid_if_Ty_is_not_bool() const {}


				friend class mse::fixed_nii_basic_string<_Ty, _Traits, _A>;
				friend class mse::xscope_fixed_nii_basic_string<_Ty, _Traits, _A>;

				friend /*class */xscope_ss_const_iterator_type;
				friend /*class */xscope_ss_iterator_type;

				template<class _Ty2, class _Traits2/* = std::char_traits<_Ty2>*/, class _A2/* = std::allocator<_Ty2>*/>
				friend std::basic_ostream<_Ty2, _Traits2>& mse::us::impl::impl::ns_fixed_nii_basic_string_base::out_to_stream(std::basic_ostream<_Ty2, _Traits2>& _Ostr, const fixed_nii_basic_string_base<_Ty2, _Traits2, _A2>& _Str);
			};

			namespace impl {
				namespace ns_fixed_nii_basic_string_base {
					template<class _Ty, class _Traits/* = std::char_traits<_Ty>*/, class _A/* = std::allocator<_Ty>*/>
					std::basic_ostream<_Ty, _Traits>& out_to_stream(std::basic_ostream<_Ty, _Traits>& _Ostr, const fixed_nii_basic_string_base<_Ty, _Traits, _A>& _Str) {
						return _Str.out_to_stream(_Ostr, _Str);
					}
				}
			}

			template<class _Ty, class _Traits, class _A>
			std::basic_ostream<_Ty, _Traits>& operator<<(std::basic_ostream<_Ty, _Traits>& _Ostr, const fixed_nii_basic_string_base<_Ty, _Traits, _A>& _Str) {
				return mse::us::impl::impl::ns_fixed_nii_basic_string_base::out_to_stream(_Ostr, _Str);
			}

			template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty>>
			inline bool operator!=(const fixed_nii_basic_string_base<_Ty, _Traits, _A>& _Left,
				const fixed_nii_basic_string_base<_Ty, _Traits, _A>& _Right) {	// test for basic_string inequality
				return (!(_Left == _Right));
			}

			template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty>>
			inline bool operator>(const fixed_nii_basic_string_base<_Ty, _Traits, _A>& _Left,
				const fixed_nii_basic_string_base<_Ty, _Traits, _A>& _Right) {	// test if _Left > _Right for basic_strings
				return (_Right < _Left);
			}

			template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty>>
			inline bool operator<=(const fixed_nii_basic_string_base<_Ty, _Traits, _A>& _Left,
				const fixed_nii_basic_string_base<_Ty, _Traits, _A>& _Right) {	// test if _Left <= _Right for basic_strings
				return (!(_Right < _Left));
			}

			template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty>>
			inline bool operator>=(const fixed_nii_basic_string_base<_Ty, _Traits, _A>& _Left,
				const fixed_nii_basic_string_base<_Ty, _Traits, _A>& _Right) {	// test if _Left >= _Right for basic_strings
				return (!(_Left < _Right));
			}

		}
	}

	template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty> >
	class fixed_nii_basic_string : public mse::us::impl::fixed_nii_basic_string_base<_Ty, _Traits, _A> {
	public:
		typedef mse::us::impl::fixed_nii_basic_string_base<_Ty, _Traits, _A> base_class;
		typedef std::basic_string<_Ty, _Traits, _A> std_basic_string;
		typedef std_basic_string _MBS;
		typedef fixed_nii_basic_string _Myt;

		typedef typename std_basic_string::allocator_type allocator_type;
		typedef typename std_basic_string::value_type value_type;
		//typedef typename std_basic_string::size_type size_type;
		typedef msev_size_t size_type;
		//typedef typename std_basic_string::difference_type difference_type;
		typedef msev_int difference_type;
		typedef typename std_basic_string::pointer pointer;
		typedef typename std_basic_string::const_pointer const_pointer;
		typedef typename std_basic_string::reference reference;
		typedef typename std_basic_string::const_reference const_reference;

		typedef typename std_basic_string::iterator iterator;
		typedef typename std_basic_string::const_iterator const_iterator;

		typedef typename std_basic_string::reverse_iterator reverse_iterator;
		typedef typename std_basic_string::const_reverse_iterator const_reverse_iterator;

	private:
		const _MBS& contained_basic_string() const& { return base_class::contained_basic_string(); }
		const _MBS& contained_basic_string() const&& { return base_class::contained_basic_string(); }
		_MBS& contained_basic_string()& { return base_class::contained_basic_string(); }
		_MBS&& contained_basic_string()&& { return std::forward<_MBS>(base_class::contained_basic_string()); }

	public:
		explicit fixed_nii_basic_string(const _A& _Al = _A()) : base_class(_Al) {}
		explicit fixed_nii_basic_string(size_type _N) : base_class(_N) {}
		explicit fixed_nii_basic_string(size_type _N, const _Ty& _V, const _A& _Al = _A()) : base_class(_N, _V, _Al) {}
		fixed_nii_basic_string(std_basic_string&& _X) : base_class(MSE_FWD(_X)) { /*m_debug_size = size();*/ }
		fixed_nii_basic_string(const std_basic_string& _X) : base_class(_X) { /*m_debug_size = size();*/ }
		fixed_nii_basic_string(_Myt&& _X) : base_class(MSE_FWD(_X)) { /*m_debug_size = size();*/ }
		fixed_nii_basic_string(const _Myt& _X) : base_class(_X) { /*m_debug_size = size();*/ }
		typedef typename std_basic_string::const_iterator _It;
		/* Note that safety cannot be guaranteed when using these constructors that take unsafe typename base_class::iterator and/or pointer parameters. */
		fixed_nii_basic_string(_It _F, _It _L, const _A& _Al = _A()) : base_class(_F, _L, _Al) { /*m_debug_size = size();*/ }
		fixed_nii_basic_string(const _Ty* _F, const _Ty* _L, const _A& _Al = _A()) : base_class(_F, _L, _Al) { /*m_debug_size = size();*/ }
		template<class _Iter
			//, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value> MSE_IMPL_EIS
			, class = mse::impl::_mse_RequireInputIter<_Iter> >
			fixed_nii_basic_string(const _Iter& _First, const _Iter& _Last) : base_class(_First, _Last) { /*m_debug_size = size();*/ }
		template<class _Iter
			//, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value> MSE_IMPL_EIS
			, class = mse::impl::_mse_RequireInputIter<_Iter> >
			//fixed_nii_basic_string(const _Iter& _First, const _Iter& _Last, const typename std_basic_string::_Alloc& _Al) : base_class(_First, _Last, _Al) { /*m_debug_size = size();*/ }
			fixed_nii_basic_string(const _Iter& _First, const _Iter& _Last, const _A& _Al) : base_class(_First, _Last, _Al) { /*m_debug_size = size();*/ }

		~fixed_nii_basic_string() {
			/* This is just a no-op function that will cause a compile error when _Ty is not an eligible type. */
			valid_if_Ty_is_not_an_xscope_type();
		}

		fixed_nii_basic_string(_XSTD initializer_list<typename std_basic_string::value_type> _Ilist, const _A& _Al = _A())
			: base_class(_Ilist, _Al) {	// construct from initializer_list
			/*m_debug_size = size();*/
		}

		//class nv_const_iterator_base : public std::iterator<std::random_access_iterator_tag, value_type, difference_type, const_pointer, const_reference> {};
		//class nv_iterator_base : public std::iterator<std::random_access_iterator_tag, value_type, difference_type, pointer, reference> {};

		typedef mse::impl::random_access_const_iterator_base<_Ty> nv_const_iterator_base;
		typedef mse::impl::random_access_iterator_base<_Ty> nv_iterator_base;

		template<typename _TVectorConstPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_potentially_not_xscope<_TVectorConstPointer>::value)> MSE_IMPL_EIS >
		using Tss_const_iterator_type = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_ss_const_iterator_type<_TVectorConstPointer>;
		template<typename _TVectorPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_potentially_not_xscope<_TVectorPointer>::value)> MSE_IMPL_EIS >
		using Tss_iterator_type = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_ss_iterator_type<_TVectorPointer>;

		template<typename _TVectorPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_potentially_not_xscope<_TVectorPointer>::value)> MSE_IMPL_EIS >
		using Tss_reverse_iterator_type = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_ss_reverse_iterator_type<_TVectorPointer>;
		template<typename _TVectorConstPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_potentially_not_xscope<_TVectorConstPointer>::value)> MSE_IMPL_EIS >
		using Tss_const_reverse_iterator_type = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_ss_const_reverse_iterator_type<_TVectorConstPointer>;

		typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_rp_ss_iterator_type<_Myt> ss_iterator_type;
		typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_rp_ss_const_iterator_type<_Myt> ss_const_iterator_type;
		typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_rp_ss_reverse_iterator_type<_Myt> ss_reverse_iterator_type;
		typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_rp_ss_const_reverse_iterator_type<_Myt> ss_const_reverse_iterator_type;

		/* Here we provide static versions of the member functions that return iterators. As static member functions do
		not have access to a "this" pointer, these functions require a pointer to the container to be passed as the first
		argument. Any returned iterator will contain a copy of the supplied pointer and inherit its safety properties. */


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

		template<typename _TVectorPointer1>
		static void s_assert_valid_index(const _TVectorPointer1& this_ptr, size_type index) {
			if ((*this_ptr).size() < index) { MSE_THROW(gnii_basic_string_range_error("invalid index - void assert_valid_index() const - fixed_nii_basic_string")); }
		}

		typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_ss_const_iterator_type<_Myt> xscope_ss_const_iterator_type;
		typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_ss_iterator_type<_Myt> xscope_ss_iterator_type;

		//typedef xscope_ss_const_iterator_type xscope_const_iterator;
		//typedef _TTXScopeConstIterator<_Myt> xscope_const_iterator;
		//typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_cslsstrong_const_iterator_type<_Myt> xscope_const_iterator;
		//typedef xscope_ss_iterator_type xscope_iterator;
		//typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_cslsstrong_iterator_type<_Myt> xscope_iterator;
		typedef TXScopeCSSSXSRAConstIterator<_Myt> xscope_const_iterator;
		typedef TXScopeCSSSXSRAIterator<_Myt> xscope_iterator;

		bool operator==(const _Myt& _Right) const {	// test for basic_string equality
			return base_class::operator==(_Right);
		}
		bool operator<(const _Myt& _Right) const {	// test if _Left < _Right for basic_strings
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
	};

#ifdef MSE_HAS_CXX17
	/* deduction guides */
	template<class _Iter, class _Alloc = std::allocator<typename std::iterator_traits<_Iter>::value_type>
		, std::enable_if_t<std::conjunction_v< mse::impl::_mse_Is_iterator<_Iter>, mse::impl::_mse_Is_allocator<_Alloc> >, int> = 0>
	fixed_nii_basic_string(_Iter, _Iter, _Alloc = _Alloc())
		->fixed_nii_basic_string<typename std::iterator_traits<_Iter>::value_type, std::char_traits<typename std::iterator_traits<_Iter>::value_type>, _Alloc>;

	template<class _Elem, class _Traits, class _Alloc = std::allocator<_Elem>
		, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
	explicit fixed_nii_basic_string(std::basic_string_view<_Elem, _Traits>, const _Alloc & = _Alloc())
		->fixed_nii_basic_string<_Elem, _Traits, _Alloc>;

	template<class _Elem, class _Traits, class _Alloc = std::allocator<_Elem>
		, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
	fixed_nii_basic_string(std::basic_string_view<_Elem, _Traits>, mse::impl::_mse_Guide_size_type_t<_Alloc>
		, mse::impl::_mse_Guide_size_type_t<_Alloc>, const _Alloc & = _Alloc())
		->fixed_nii_basic_string<_Elem, _Traits, _Alloc>;

	template<class _TRAIterator, class _Traits, class _Alloc = std::allocator<typename std::iterator_traits<_TRAIterator>::value_type>
		, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
	explicit fixed_nii_basic_string(mse::TStringConstSection<_TRAIterator, _Traits>, const _Alloc & = _Alloc())
		->fixed_nii_basic_string<typename std::iterator_traits<_TRAIterator>::value_type, _Traits, _Alloc>;
	template<class _TRAIterator, class _Traits, class _Alloc = std::allocator<typename std::iterator_traits<_TRAIterator>::value_type>
		, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
	explicit fixed_nii_basic_string(mse::TStringSection<_TRAIterator, _Traits>, const _Alloc & = _Alloc())
		->fixed_nii_basic_string<typename std::iterator_traits<_TRAIterator>::value_type, _Traits, _Alloc>;
	template<class _TRAIterator, class _Traits, class _Alloc = std::allocator<typename std::iterator_traits<_TRAIterator>::value_type>
		, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
	explicit fixed_nii_basic_string(mse::TXScopeStringConstSection<_TRAIterator, _Traits>, const _Alloc & = _Alloc())
		->fixed_nii_basic_string<typename std::iterator_traits<_TRAIterator>::value_type, _Traits, _Alloc>;
	template<class _TRAIterator, class _Traits, class _Alloc = std::allocator<typename std::iterator_traits<_TRAIterator>::value_type>
		, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
	explicit fixed_nii_basic_string(mse::TXScopeStringSection<_TRAIterator, _Traits>, const _Alloc & = _Alloc())
		->fixed_nii_basic_string<typename std::iterator_traits<_TRAIterator>::value_type, _Traits, _Alloc>;
#endif /* MSE_HAS_CXX17 */

	using fixed_nii_string = fixed_nii_basic_string<char>;
	using fixed_nii_wstring = fixed_nii_basic_string<wchar_t>;
	using fixed_nii_u16string = fixed_nii_basic_string<char16_t>;
	using fixed_nii_u32string = fixed_nii_basic_string<char32_t>;


	template<class _TLender, class _Ty, class _Traits, class _A>
	class xscope_borrowing_fixed_nii_basic_string;

	template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty> >
	class xscope_fixed_nii_basic_string : public mse::us::impl::fixed_nii_basic_string_base<_Ty, _Traits, _A>, public mse::us::impl::XScopeTagBase
		, public mse::impl::first_or_placeholder_if_not_base_of_second<mse::us::impl::ReferenceableByScopePointerTagBase, _Ty, xscope_fixed_nii_basic_string<_Ty, _Traits, _A> >
		, public mse::impl::first_or_placeholder_if_not_base_of_second<mse::us::impl::ContainsNonOwningScopeReferenceTagBase, _Ty, xscope_fixed_nii_basic_string<_Ty, _Traits, _A> >
	{
	public:
		typedef mse::us::impl::fixed_nii_basic_string_base<_Ty, _Traits, _A> base_class;
		typedef std::basic_string<_Ty, _Traits, _A> std_basic_string;
		typedef std_basic_string _MBS;
		typedef xscope_fixed_nii_basic_string _Myt;

		typedef typename std_basic_string::allocator_type allocator_type;
		typedef typename std_basic_string::value_type value_type;
		//typedef typename std_basic_string::size_type size_type;
		typedef msev_size_t size_type;
		//typedef typename std_basic_string::difference_type difference_type;
		typedef msev_int difference_type;
		typedef typename std_basic_string::pointer pointer;
		typedef typename std_basic_string::const_pointer const_pointer;
		typedef typename std_basic_string::reference reference;
		typedef typename std_basic_string::const_reference const_reference;

		typedef typename std_basic_string::iterator iterator;
		typedef typename std_basic_string::const_iterator const_iterator;

		typedef typename std_basic_string::reverse_iterator reverse_iterator;
		typedef typename std_basic_string::const_reverse_iterator const_reverse_iterator;

	private:
		const _MBS& contained_basic_string() const& { return base_class::contained_basic_string(); }
		const _MBS& contained_basic_string() const&& { return base_class::contained_basic_string(); }
		_MBS& contained_basic_string()& { return base_class::contained_basic_string(); }
		_MBS&& contained_basic_string()&& { return std::forward<_MBS>(base_class::contained_basic_string()); }

	public:
		explicit xscope_fixed_nii_basic_string(const _A& _Al = _A()) : base_class(_Al) {}
		explicit xscope_fixed_nii_basic_string(size_type _N) : base_class(_N) {}
		explicit xscope_fixed_nii_basic_string(size_type _N, const _Ty& _V, const _A& _Al = _A()) : base_class(_N, _V, _Al) {}
		xscope_fixed_nii_basic_string(std_basic_string&& _X) : base_class(MSE_FWD(_X)) { /*m_debug_size = size();*/ }
		xscope_fixed_nii_basic_string(const std_basic_string& _X) : base_class(_X) { /*m_debug_size = size();*/ }
		xscope_fixed_nii_basic_string(_Myt&& _X) : base_class(MSE_FWD(_X)) { /*m_debug_size = size();*/ }
		xscope_fixed_nii_basic_string(const _Myt& _X) : base_class(_X) { /*m_debug_size = size();*/ }
		typedef typename std_basic_string::const_iterator _It;
		/* Note that safety cannot be guaranteed when using these constructors that take unsafe typename base_class::iterator and/or pointer parameters. */
		xscope_fixed_nii_basic_string(_It _F, _It _L, const _A& _Al = _A()) : base_class(_F, _L, _Al) { /*m_debug_size = size();*/ }
		xscope_fixed_nii_basic_string(const _Ty* _F, const _Ty* _L, const _A& _Al = _A()) : base_class(_F, _L, _Al) { /*m_debug_size = size();*/ }
		template<class _Iter
			//, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value> MSE_IMPL_EIS
			, class = mse::impl::_mse_RequireInputIter<_Iter> >
			xscope_fixed_nii_basic_string(const _Iter& _First, const _Iter& _Last) : base_class(_First, _Last) { /*m_debug_size = size();*/ }
		template<class _Iter
			//, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value> MSE_IMPL_EIS
			, class = mse::impl::_mse_RequireInputIter<_Iter> >
			//xscope_fixed_nii_basic_string(const _Iter& _First, const _Iter& _Last, const typename std_basic_string::_Alloc& _Al) : base_class(_First, _Last, _Al) { /*m_debug_size = size();*/ }
			xscope_fixed_nii_basic_string(const _Iter& _First, const _Iter& _Last, const _A& _Al) : base_class(_First, _Last, _Al) { /*m_debug_size = size();*/ }

		xscope_fixed_nii_basic_string(_XSTD initializer_list<typename std_basic_string::value_type> _Ilist, const _A& _Al = _A())
			: base_class(_Ilist, _Al) {	// construct from initializer_list
			/*m_debug_size = size();*/
		}

		//class nv_const_iterator_base : public std::iterator<std::random_access_iterator_tag, value_type, difference_type, const_pointer, const_reference> {};
		//class nv_iterator_base : public std::iterator<std::random_access_iterator_tag, value_type, difference_type, pointer, reference> {};

		typedef mse::impl::random_access_const_iterator_base<_Ty> nv_const_iterator_base;
		typedef mse::impl::random_access_iterator_base<_Ty> nv_iterator_base;

		template<typename _TVectorConstPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_potentially_not_xscope<_TVectorConstPointer>::value)> MSE_IMPL_EIS >
		using Tss_const_iterator_type = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_ss_const_iterator_type<_TVectorConstPointer>;
		template<typename _TVectorPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_potentially_not_xscope<_TVectorPointer>::value)> MSE_IMPL_EIS >
		using Tss_iterator_type = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_ss_iterator_type<_TVectorPointer>;

		template<typename _TVectorPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_potentially_not_xscope<_TVectorPointer>::value)> MSE_IMPL_EIS >
		using Tss_reverse_iterator_type = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_ss_reverse_iterator_type<_TVectorPointer>;
		template<typename _TVectorConstPointer, MSE_IMPL_EIP mse::impl::enable_if_t<(mse::impl::is_potentially_not_xscope<_TVectorConstPointer>::value)> MSE_IMPL_EIS >
		using Tss_const_reverse_iterator_type = mse::impl::ns_gnii_basic_string::Tgnii_basic_string_ss_const_reverse_iterator_type<_TVectorConstPointer>;

		typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_rp_ss_iterator_type<_Myt> ss_iterator_type;
		typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_rp_ss_const_iterator_type<_Myt> ss_const_iterator_type;
		typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_rp_ss_reverse_iterator_type<_Myt> ss_reverse_iterator_type;
		typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_rp_ss_const_reverse_iterator_type<_Myt> ss_const_reverse_iterator_type;

		/* Here we provide static versions of the member functions that return iterators. As static member functions do
		not have access to a "this" pointer, these functions require a pointer to the container to be passed as the first
		argument. Any returned iterator will contain a copy of the supplied pointer and inherit its safety properties. */

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

		template<typename _TVectorPointer1>
		static void s_assert_valid_index(const _TVectorPointer1& this_ptr, size_type index) {
			if ((*this_ptr).size() < index) { MSE_THROW(gnii_basic_string_range_error("invalid index - void assert_valid_index() const - xscope_fixed_nii_basic_string")); }
		}

		typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_ss_const_iterator_type<_Myt> xscope_ss_const_iterator_type;
		typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_ss_iterator_type<_Myt> xscope_ss_iterator_type;

		//typedef xscope_ss_const_iterator_type xscope_const_iterator;
		//typedef _TTXScopeConstIterator<_Myt> xscope_const_iterator;
		//typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_cslsstrong_const_iterator_type<_Myt> xscope_const_iterator;
		//typedef xscope_ss_iterator_type xscope_iterator;
		//typedef mse::impl::ns_gnii_basic_string::Tgnii_basic_string_xscope_cslsstrong_iterator_type<_Myt> xscope_iterator;
		typedef TXScopeCSSSXSRAConstIterator<_Myt> xscope_const_iterator;
		typedef TXScopeCSSSXSRAIterator<_Myt> xscope_iterator;

		bool operator==(const _Myt& _Right) const {	// test for basic_string equality
			return base_class::operator==(_Right);
		}
		bool operator<(const _Myt& _Right) const {	// test if _Left < _Right for basic_strings
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

		friend /*class */xscope_ss_const_iterator_type;
		friend /*class */xscope_ss_iterator_type;
		template<class _TLender2, class _Ty2, class _Traits2, class _A2> friend class xscope_borrowing_fixed_nii_basic_string;
	};

#ifdef MSE_HAS_CXX17
	/* deduction guides */
	template<class _Iter, class _Alloc = std::allocator<typename std::iterator_traits<_Iter>::value_type>
		, std::enable_if_t<std::conjunction_v< mse::impl::_mse_Is_iterator<_Iter>, mse::impl::_mse_Is_allocator<_Alloc> >, int> = 0>
	xscope_fixed_nii_basic_string(_Iter, _Iter, _Alloc = _Alloc())
		->xscope_fixed_nii_basic_string<typename std::iterator_traits<_Iter>::value_type, std::char_traits<typename std::iterator_traits<_Iter>::value_type>, _Alloc>;

	template<class _Elem, class _Traits, class _Alloc = std::allocator<_Elem>
		, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
	explicit xscope_fixed_nii_basic_string(std::basic_string_view<_Elem, _Traits>, const _Alloc & = _Alloc())
		->xscope_fixed_nii_basic_string<_Elem, _Traits, _Alloc>;

	template<class _Elem, class _Traits, class _Alloc = std::allocator<_Elem>
		, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
	xscope_fixed_nii_basic_string(std::basic_string_view<_Elem, _Traits>, mse::impl::_mse_Guide_size_type_t<_Alloc>
		, mse::impl::_mse_Guide_size_type_t<_Alloc>, const _Alloc & = _Alloc())
		->xscope_fixed_nii_basic_string<_Elem, _Traits, _Alloc>;

	template<class _TRAIterator, class _Traits, class _Alloc = std::allocator<typename std::iterator_traits<_TRAIterator>::value_type>
		, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
	explicit xscope_fixed_nii_basic_string(mse::TStringConstSection<_TRAIterator, _Traits>, const _Alloc & = _Alloc())
		->xscope_fixed_nii_basic_string<typename std::iterator_traits<_TRAIterator>::value_type, _Traits, _Alloc>;
	template<class _TRAIterator, class _Traits, class _Alloc = std::allocator<typename std::iterator_traits<_TRAIterator>::value_type>
		, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
	explicit xscope_fixed_nii_basic_string(mse::TStringSection<_TRAIterator, _Traits>, const _Alloc & = _Alloc())
		->xscope_fixed_nii_basic_string<typename std::iterator_traits<_TRAIterator>::value_type, _Traits, _Alloc>;
	template<class _TRAIterator, class _Traits, class _Alloc = std::allocator<typename std::iterator_traits<_TRAIterator>::value_type>
		, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
	explicit xscope_fixed_nii_basic_string(mse::TXScopeStringConstSection<_TRAIterator, _Traits>, const _Alloc & = _Alloc())
		->xscope_fixed_nii_basic_string<typename std::iterator_traits<_TRAIterator>::value_type, _Traits, _Alloc>;
	template<class _TRAIterator, class _Traits, class _Alloc = std::allocator<typename std::iterator_traits<_TRAIterator>::value_type>
		, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
	explicit xscope_fixed_nii_basic_string(mse::TXScopeStringSection<_TRAIterator, _Traits>, const _Alloc & = _Alloc())
		->xscope_fixed_nii_basic_string<typename std::iterator_traits<_TRAIterator>::value_type, _Traits, _Alloc>;
#endif /* MSE_HAS_CXX17 */

	using xscope_fixed_nii_string = xscope_fixed_nii_basic_string<char>;
	using xscope_fixed_nii_wstring = xscope_fixed_nii_basic_string<wchar_t>;
	using xscope_fixed_nii_u16string = xscope_fixed_nii_basic_string<char16_t>;
	using xscope_fixed_nii_u32string = xscope_fixed_nii_basic_string<char32_t>;


	template<class _TLender, class _Ty = mse::impl::container_element_type<_TLender>, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty> >
	class xscope_borrowing_fixed_nii_basic_string : public xscope_fixed_nii_basic_string<_Ty, _Traits, _A>
		, public mse::impl::first_or_placeholder_if_base_of_second<mse::us::impl::ContainsNonOwningScopeReferenceTagBase, xscope_fixed_nii_basic_string<_Ty, _Traits, _A>, xscope_borrowing_fixed_nii_basic_string<_TLender, _Ty, _Traits, _A> >
	{
	public:
		typedef xscope_fixed_nii_basic_string<_Ty, _Traits, _A> base_class;

		typedef typename base_class::std_basic_string std_basic_string;
		typedef typename base_class::_MBS _MBS;
		typedef xscope_borrowing_fixed_nii_basic_string _Myt;

		typedef typename base_class::allocator_type allocator_type;
		typedef typename base_class::value_type value_type;
		typedef typename base_class::size_type size_type;
		typedef typename base_class::difference_type difference_type;
		typedef typename base_class::pointer pointer;
		typedef typename base_class::const_pointer const_pointer;
		typedef typename base_class::reference reference;
		typedef typename base_class::const_reference const_reference;

		typedef typename base_class::iterator iterator;
		typedef typename base_class::const_iterator const_iterator;

		typedef typename base_class::reverse_iterator reverse_iterator;
		typedef typename base_class::const_reverse_iterator const_reverse_iterator;

		xscope_borrowing_fixed_nii_basic_string(xscope_borrowing_fixed_nii_basic_string&&) = default;

		xscope_borrowing_fixed_nii_basic_string(const mse::TXScopeFixedPointer<_TLender>& src_xs_ptr) : m_src_ref(*src_xs_ptr) {
			(*this).contained_basic_string() = std::move(m_src_ref);
		}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		xscope_borrowing_fixed_nii_basic_string(_TLender* src_xs_ptr) : m_src_ref(*src_xs_ptr) {
			(*this).contained_basic_string() = std::move(m_src_ref);
		}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
		~xscope_borrowing_fixed_nii_basic_string() {
			m_src_ref = std::move((*this).contained_basic_string());
		}

		MSE_INHERIT_XSCOPE_ASYNC_SHAREABILITY_OF(base_class);

	private:
#ifdef MSE_HAS_CXX17
		xscope_borrowing_fixed_nii_basic_string(const xscope_borrowing_fixed_nii_basic_string&) = delete;
#endif // MSE_HAS_CXX17

		_TLender& m_src_ref;
	};

	template<class _TLender>
	auto make_xscope_borrowing_fixed_nii_basic_string(const mse::TXScopeFixedPointer<_TLender>& src_xs_ptr) -> xscope_borrowing_fixed_nii_basic_string<_TLender> {
		return xscope_borrowing_fixed_nii_basic_string<_TLender>(src_xs_ptr);
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _TLender>
	auto make_xscope_borrowing_fixed_nii_basic_string(_TLender* src_xs_ptr) -> xscope_borrowing_fixed_nii_basic_string<_TLender> {
		return xscope_borrowing_fixed_nii_basic_string<_TLender>(src_xs_ptr);
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	template<class _TLender>
	auto make_xscope_borrowing_fixed_nii_string(const mse::TXScopeFixedPointer<_TLender>& src_xs_ptr) {
		return make_xscope_borrowing_fixed_nii_basic_string(src_xs_ptr);
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _TLender>
	auto make_xscope_borrowing_fixed_nii_string(_TLender* src_xs_ptr) {
		return make_xscope_borrowing_fixed_nii_basic_string(src_xs_ptr);
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)


	using nii_string = nii_basic_string<char>;
	using nii_wstring = nii_basic_string<wchar_t>;
	using nii_u16string = nii_basic_string<char16_t>;
	using nii_u32string = nii_basic_string<char32_t>;

	inline namespace literals {
		inline namespace string_literals {
			inline nii_basic_string<char> operator "" _niis(const char *z_str, size_t z_len) _NOEXCEPT {
				return nii_basic_string<char>(z_str, z_len);
			}
			inline nii_basic_string<wchar_t> operator "" _niis(const wchar_t *z_str, size_t z_len) _NOEXCEPT {
				return nii_basic_string<wchar_t>(z_str, z_len);
			}
			inline nii_basic_string<char16_t> operator "" _niis(const char16_t *z_str, size_t z_len) _NOEXCEPT {
				return nii_basic_string<char16_t>(z_str, z_len);
			}
			inline nii_basic_string<char32_t> operator "" _niis(const char32_t *z_str, size_t z_len) _NOEXCEPT {
				return nii_basic_string<char32_t>(z_str, z_len);
			}
		}
	}


	namespace mstd {
#ifndef MSE_MSTDSTRING_DISABLED
		/* forward declaration because mstd::basic_string<> needs to be declared a friend of us::msebasic_string<> (in its current
		implementation) */
		template<class _Ty, class _Traits/* = std::char_traits<_Ty>*/, class _A/* = std::allocator<_Ty> */>
		class basic_string;
#endif /*!MSE_MSTDSTRING_DISABLED*/
	}

	namespace us {

		namespace ns_msebasic_string {
			template<class _TContainer> class xscope_structure_lock_guard;
			template<class _TContainer> class xscope_const_structure_lock_guard;
		}

		/* msebasic_string<> is an unsafe extension of mse::us::impl::gnii_basic_string<> that provides the traditional begin() and end() (non-static)
		member functions that return unsafe iterators. It also provides ss_begin() and ss_end() (non-static) member
		functions which return bounds-checked, but still technically unsafe iterators. */
		template<class _Ty, class _Traits, class _A, class _TStateMutex>
		class msebasic_string : public mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex>, public mse::us::impl::AsyncNotShareableTagBase {
		public:
			typedef mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex> base_class;
			typedef std::basic_string<_Ty, _Traits, _A> std_basic_string;
			typedef msebasic_string _Myt;

			typedef typename base_class::allocator_type allocator_type;
			MSE_INHERITED_RANDOM_ACCESS_MEMBER_TYPE_DECLARATIONS(base_class);
			static const size_t npos = size_t(-1);

			typedef typename base_class::_MBS::iterator iterator;
			typedef typename base_class::_MBS::const_iterator const_iterator;
			typedef typename base_class::_MBS::reverse_iterator reverse_iterator;
			typedef typename base_class::_MBS::const_reverse_iterator const_reverse_iterator;

			explicit msebasic_string(const _A& _Al = _A())
				: base_class(_Al), m_mmitset(*this) {
				/*m_debug_size = size();*/
			}
			explicit msebasic_string(size_type _N)
				: base_class(msev_as_a_size_t(_N)), m_mmitset(*this) {
				/*m_debug_size = size();*/
			}
#ifdef MSE_HAS_CXX17
			template<class _Alloc2 = _A, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc2>::value, int> = 0>
#endif /* MSE_HAS_CXX17 */
			explicit msebasic_string(size_type _N, const _Ty& _V, const _A& _Al = _A())
				: base_class(msev_as_a_size_t(_N), _V, _Al), m_mmitset(*this) {
				/*m_debug_size = size();*/
			}
			msebasic_string(std_basic_string&& _X) : base_class(MSE_FWD(_X)), m_mmitset(*this) { /*m_debug_size = size();*/ }
			msebasic_string(const std_basic_string& _X) : base_class(_X), m_mmitset(*this) { /*m_debug_size = size();*/ }
			msebasic_string(base_class&& _X) : base_class(MSE_FWD(_X)), m_mmitset(*this) { /*m_debug_size = size();*/ }
			msebasic_string(const base_class& _X) : base_class(_X), m_mmitset(*this) { /*m_debug_size = size();*/ }
			msebasic_string(_Myt&& _X) : base_class(MSE_FWD(_X)), m_mmitset(*this) { /*m_debug_size = size();*/ }
			msebasic_string(const _Myt& _X) : base_class(_X), m_mmitset(*this) { /*m_debug_size = size();*/ }
			typedef typename base_class::const_iterator _It;
			/* Note that safety cannot be guaranteed when using these constructors that take unsafe typename base_class::iterator and/or pointer parameters. */
			msebasic_string(_It _F, _It _L, const _A& _Al = _A()) : base_class(_F, _L, _Al), m_mmitset(*this) { /*m_debug_size = size();*/ }
			msebasic_string(const _Ty*  _F, const _Ty*  _L, const _A& _Al = _A()) : base_class(_F, _L, _Al), m_mmitset(*this) { /*m_debug_size = size();*/ }
			template<class _Iter
				//, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value> MSE_IMPL_EIS
				, class = mse::impl::_mse_RequireInputIter<_Iter> >
				msebasic_string(const _Iter& _First, const _Iter& _Last) : base_class(_First, _Last), m_mmitset(*this) { /*m_debug_size = size();*/ }
			template<class _Iter
				//, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value> MSE_IMPL_EIS
				, class = mse::impl::_mse_RequireInputIter<_Iter> >
				//msebasic_string(const _Iter& _First, const _Iter& _Last, const typename base_class::_Alloc& _Al) : base_class(_First, _Last, _Al), m_mmitset(*this) { /*m_debug_size = size();*/ }
				msebasic_string(const _Iter& _First, const _Iter& _Last, const _A& _Al) : base_class(_First, _Last, _Al), m_mmitset(*this) { /*m_debug_size = size();*/ }
			msebasic_string(const _Ty* const _Ptr) : base_class(_Ptr), m_mmitset(*this) { /*m_debug_size = size();*/ }
			msebasic_string(const _Ty* const _Ptr, const size_t _Count) : base_class(_Ptr, _Count), m_mmitset(*this) { /*m_debug_size = size();*/ }
			msebasic_string(const _Myt& _X, const size_type _Roff, const _A& _Al = _A()) : base_class(_X, _Roff, npos, _Al), m_mmitset(*this) { /*m_debug_size = size();*/ }
			msebasic_string(const _Myt& _X, const size_type _Roff, const size_type _Count, const _A& _Al = _A()) : base_class(_X, _Roff, _Count, _Al), m_mmitset(*this) { /*m_debug_size = size();*/ }


			msebasic_string(const mse::TXScopeFixedConstPointer<_Myt>& xs_ptr) : base_class(*xs_ptr), m_mmitset(*this) { /*m_debug_size = size();*/ }
			msebasic_string(const mse::TXScopeFixedConstPointer<mse::us::impl::gnii_basic_string<_Ty, _Traits> >& xs_ptr) : base_class(*xs_ptr), m_mmitset(*this) { /*m_debug_size = size();*/ }
			msebasic_string(const mse::TXScopeFixedConstPointer<std::basic_string<_Ty, _Traits> >& xs_ptr) : base_class(*xs_ptr), m_mmitset(*this) { /*m_debug_size = size();*/ }
			msebasic_string(const mse::TXScopeFixedConstPointer<_Myt>& xs_ptr, const size_type _Roff, const _A& _Al = _A()) : base_class(*xs_ptr, _Roff, npos, _Al), m_mmitset(*this) { /*m_debug_size = size();*/ }
			msebasic_string(const mse::TXScopeFixedConstPointer<_Myt>& xs_ptr, const size_type _Roff, const size_type _Count, const _A& _Al = _A()) : base_class(*xs_ptr, _Roff, _Count, _Al), m_mmitset(*this) { /*m_debug_size = size();*/ }

#ifdef MSE_HAS_CXX17
			template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1>*/>
			msebasic_string(const _TParam1& _Right) : base_class(), m_mmitset(*this) { assign(_Right); }

			template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1>*/>
			msebasic_string(const _TParam1& _Right, const size_type _Roff, const size_type _Count, const _A& _Al = _A())
				: base_class(_Al), m_mmitset(*this) {
				assign(_Right, _Roff, _Count);
			}
#else /* MSE_HAS_CXX17 */
			/* construct from mse::string_view and "string sections". */
			template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
			explicit msebasic_string(const _TStringSection& _X) : base_class(_X), m_mmitset(*this) { /*m_debug_size = size();*/ }
#endif /* MSE_HAS_CXX17 */

			_Myt& operator=(const base_class& _X) {
				structure_change_guard<decltype(m_structure_change_mutex)> lock2(m_structure_change_mutex);
				base_class::operator =(_X);
				/*m_debug_size = size();*/
				m_mmitset.reset();
				return (*this);
			}
			_Myt& operator=(_Myt&& _X) {
				structure_change_guard<decltype(m_structure_change_mutex)> lock2(m_structure_change_mutex);
				base_class::operator=(MSE_FWD(_X));
				m_mmitset.reset();
				return (*this);
			}
			_Myt& operator=(const _Myt& _X) {
				structure_change_guard<decltype(m_structure_change_mutex)> lock2(m_structure_change_mutex);
				base_class::operator=(_X);
				m_mmitset.reset();
				return (*this);
			}
			void reserve(size_type _Count)
			{	// determine new minimum length of allocated storage
				structure_change_guard<decltype(m_structure_change_mutex)> lock2(m_structure_change_mutex);
				auto original_capacity = msev_size_t((*this).capacity());

				base_class::reserve(msev_as_a_size_t(_Count));

				auto new_capacity = msev_size_t((*this).capacity());
				bool realloc_occured = (new_capacity != original_capacity);
				if (realloc_occured) {
					m_mmitset.sync_iterators_to_index();
				}
			}
			void shrink_to_fit() {	// reduce capacity
				auto original_capacity = msev_size_t((*this).capacity());

				base_class::shrink_to_fit();

				auto new_capacity = msev_size_t((*this).capacity());
				bool realloc_occured = (new_capacity != original_capacity);
				if (realloc_occured) {
					m_mmitset.sync_iterators_to_index();
				}
			}
			void resize(size_type _N, const _Ty& _X = _Ty()) {
				structure_change_guard<decltype(m_structure_change_mutex)> lock2(m_structure_change_mutex);
				auto original_size = msev_size_t((*this).size());
				auto original_capacity = msev_size_t((*this).capacity());
				bool shrinking = (_N < original_size);

				base_class::resize(msev_as_a_size_t(_N), _X);
				/*m_debug_size = size();*/

				if (shrinking) {
					m_mmitset.invalidate_inclusive_range(_N, msev_size_t(original_size - 1));
				}
				m_mmitset.shift_inclusive_range(original_size, original_size, msev_size_t(_N) - original_size); /*shift the end markers*/
				auto new_capacity = msev_size_t((*this).capacity());
				bool realloc_occured = (new_capacity != original_capacity);
				if (realloc_occured) {
					m_mmitset.sync_iterators_to_index();
				}
			}
			typename base_class::const_reference operator[](size_type _P) const {
				return (*this).at(msev_as_a_size_t(_P));
			}
			typename base_class::reference operator[](size_type _P) {
				return (*this).at(msev_as_a_size_t(_P));
			}
			typename base_class::reference front() {	// return first element of mutable sequence
				if (0 == (*this).size()) { MSE_THROW(msebasic_string_range_error("front() on empty - typename base_class::reference front() - msebasic_string")); }
				return base_class::front();
			}
			typename base_class::const_reference front() const {	// return first element of nonmutable sequence
				if (0 == (*this).size()) { MSE_THROW(msebasic_string_range_error("front() on empty - typename base_class::const_reference front() - msebasic_string")); }
				return base_class::front();
			}
			typename base_class::reference back() {	// return last element of mutable sequence
				if (0 == (*this).size()) { MSE_THROW(msebasic_string_range_error("back() on empty - typename base_class::reference back() - msebasic_string")); }
				return base_class::back();
			}
			typename base_class::const_reference back() const {	// return last element of nonmutable sequence
				if (0 == (*this).size()) { MSE_THROW(msebasic_string_range_error("back() on empty - typename base_class::const_reference back() - msebasic_string")); }
				return base_class::back();
			}
			void push_back(_Ty&& _X) {
				structure_change_guard<decltype(m_structure_change_mutex)> lock2(m_structure_change_mutex);
				if (m_mmitset.is_empty()) {
					base_class::push_back(MSE_FWD(_X));
				}
				else {
					auto original_size = msev_size_t((*this).size());
					auto original_capacity = msev_size_t((*this).capacity());

					base_class::push_back(MSE_FWD(_X));
					/*m_debug_size = size();*/

					assert(msev_size_t(original_size + 1) == msev_size_t((*this).size()));
					m_mmitset.shift_inclusive_range(original_size, original_size, 1); /*shift the end markers*/
					auto new_capacity = msev_size_t((*this).capacity());
					bool realloc_occured = (new_capacity != original_capacity);
					if (realloc_occured) {
						m_mmitset.sync_iterators_to_index();
					}
				}
			}
			void push_back(const _Ty& _X) {
				structure_change_guard<decltype(m_structure_change_mutex)> lock2(m_structure_change_mutex);
				if (m_mmitset.is_empty()) {
					base_class::push_back(_X);
				}
				else {
					auto original_size = msev_size_t((*this).size());
					auto original_capacity = msev_size_t((*this).capacity());

					base_class::push_back(_X);
					/*m_debug_size = size();*/

					assert(msev_size_t(original_size + 1) == msev_size_t((*this).size()));
					m_mmitset.shift_inclusive_range(original_size, original_size, 1); /*shift the end markers*/
					auto new_capacity = msev_size_t((*this).capacity());
					bool realloc_occured = (new_capacity != original_capacity);
					if (realloc_occured) {
						m_mmitset.sync_iterators_to_index();
					}
				}
			}
			void pop_back() {
				structure_change_guard<decltype(m_structure_change_mutex)> lock2(m_structure_change_mutex);
				if (m_mmitset.is_empty()) {
					base_class::pop_back();
				}
				else {
					auto original_size = msev_size_t((*this).size());
					auto original_capacity = msev_size_t((*this).capacity());

					if (0 == original_size) { MSE_THROW(msebasic_string_range_error("pop_back() on empty - void pop_back() - msebasic_string")); }
					base_class::pop_back();
					/*m_debug_size = size();*/

					assert(msev_size_t(original_size - 1) == msev_size_t((*this).size()));
					m_mmitset.invalidate_inclusive_range(msev_size_t(original_size - 1), msev_size_t(original_size - 1));
					m_mmitset.shift_inclusive_range(original_size, original_size, -1); /*shift the end markers*/
					auto new_capacity = msev_size_t((*this).capacity());
					bool realloc_occured = (new_capacity != original_capacity);
					if (realloc_occured) {
						m_mmitset.sync_iterators_to_index();
					}
				}
			}

			msebasic_string& assign(mse::TXScopeFixedConstPointer<base_class> xs_ptr) {
				base_class::assign(*xs_ptr);
				m_mmitset.reset();
				return (*this);
			}
			msebasic_string& assign(const base_class& _Right) {
				auto xs_ptr = mse::us::unsafe_make_xscope_const_pointer_to(_Right);
				return assign(xs_ptr);
			}
			msebasic_string& assign(mse::TXScopeFixedConstPointer<base_class> xs_ptr, const size_type _Roff, size_type _Count = npos) {
				base_class::assign(*xs_ptr, _Roff, _Count);
				m_mmitset.reset();
				return (*this);
			}
			msebasic_string& assign(const base_class& _Right, const size_type _Roff, size_type _Count = npos) {
				auto xs_ptr = mse::us::unsafe_make_xscope_const_pointer_to(_Right);
				return assign(xs_ptr, _Roff, _Count);
			}

			msebasic_string& assign(const _Ty * const _Ptr, const size_type _Count) {
				base_class::assign(_Ptr, _Count);
				m_mmitset.reset();
				return (*this);
			}
			msebasic_string& assign(const _Ty * const _Ptr) {
				base_class::assign(_Ptr);
				m_mmitset.reset();
				return (*this);
			}
			msebasic_string& assign(const size_type _Count, const _Ty& _Ch) {
				base_class::assign(_Count, _Ch);
				m_mmitset.reset();
				return (*this);
			}
			template<class _Iter, MSE_IMPL_EIP mse::impl::enable_if_t<mse::impl::_mse_Is_iterator_v<_Iter> > MSE_IMPL_EIS >
			msebasic_string& assign(const _Iter _First, const _Iter _Last) {
				base_class::assign(_First, _Last);
				m_mmitset.reset();
				return (*this);
			}

#ifdef MSE_HAS_CXX17
		private:
			template<class _TParam1>
			msebasic_string& assign_helper1(std::true_type, const _TParam1& _Right) {
				return (assign(static_cast<const base_class&>(_Right)));
			}
			template<class _TParam1>
			msebasic_string& assign_helper1(std::false_type, const _TParam1& _Right) {
				base_class::assign(_Right);
				m_mmitset.reset();
				return (*this);
			}
		public:
			template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
			msebasic_string& assign(const _TParam1& _Right) {
				return assign_helper1(typename std::is_base_of<base_class, _TParam1>::type(), _Right);
			}

		private:
			template<class _TParam1>
			msebasic_string& assign_helper1(std::true_type, const _TParam1& _Right, const size_type _Roff, const size_type _Count) {
				return (assign(static_cast<const base_class&>(_Right, _Roff, _Count)));
			}
			template<class _TParam1>
			msebasic_string& assign_helper1(std::false_type, const _TParam1& _Right, const size_type _Roff, const size_type _Count) {
				base_class::assign(_Right, _Roff, _Count);
				m_mmitset.reset();
				return (*this);
			}
		public:
			template<class _TParam1/*, class = _Is_string_view_or_section_ish<_TParam1> */>
			msebasic_string& assign(const _TParam1& _Right, const size_type _Roff, const size_type _Count = npos) {
				return assign_helper1(typename std::is_base_of<base_class, _TParam1>::type(), _Right, _Roff, _Count);
			}
#else /* MSE_HAS_CXX17 */
			template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
			msebasic_string& assign(const _TStringSection& _X) {
				base_class::assign(_X);
				m_mmitset.reset();
				return (*this);
			}
#endif /* MSE_HAS_CXX17 */

			typename base_class::iterator insert(typename base_class::const_iterator _P, _Ty&& _X) {
				return (emplace(_P, MSE_FWD(_X)));
			}
			typename base_class::iterator insert(typename base_class::const_iterator _P, const _Ty& _X = _Ty()) {
				structure_change_guard<decltype(m_structure_change_mutex)> lock2(m_structure_change_mutex);
				if (m_mmitset.is_empty()) {
					typename base_class::iterator retval = base_class::insert(_P, _X);
					/*m_debug_size = size();*/
					return retval;
				}
				else {
					msev_int di = std::distance(base_class::cbegin(), _P);
					msev_size_t d = msev_size_t(di);
					if ((0 > di) || (msev_size_t((*this).size()) < di)) { MSE_THROW(msebasic_string_range_error("index out of range - typename base_class::iterator insert() - msebasic_string")); }

					auto original_size = msev_size_t((*this).size());
					auto original_capacity = msev_size_t((*this).capacity());

					typename base_class::iterator retval = base_class::insert(_P, _X);
					/*m_debug_size = size();*/

					assert(msev_size_t(original_size + 1) == msev_size_t((*this).size()));
					assert(di == std::distance(base_class::begin(), retval));
					m_mmitset.shift_inclusive_range(d, original_size, 1);
					auto new_capacity = msev_size_t((*this).capacity());
					bool realloc_occured = (new_capacity != original_capacity);
					if (realloc_occured) {
						m_mmitset.sync_iterators_to_index();
					}
					return retval;
				}
			}

#if !(defined(GPP4P8_COMPATIBLE))
			typename base_class::iterator insert(typename base_class::const_iterator _P, size_type _M, const _Ty& _X) {
				structure_change_guard<decltype(m_structure_change_mutex)> lock2(m_structure_change_mutex);
				if (m_mmitset.is_empty()) {
					typename base_class::iterator retval = base_class::insert(_P, msev_as_a_size_t(_M), _X);
					/*m_debug_size = size();*/
					return retval;
				}
				else {
					msev_int di = std::distance(base_class::cbegin(), _P);
					msev_size_t d = msev_size_t(di);
					if ((0 > di) || ((*this).size() < msev_size_t(di))) { MSE_THROW(msebasic_string_range_error("index out of range - typename base_class::iterator insert() - msebasic_string")); }

					auto original_size = msev_size_t((*this).size());
					auto original_capacity = msev_size_t((*this).capacity());

					typename base_class::iterator retval = base_class::insert(_P, msev_as_a_size_t(_M), _X);
					/*m_debug_size = size();*/

					assert(msev_size_t(original_size + _M) == msev_size_t((*this).size()));
					assert(di == std::distance(base_class::begin(), retval));
					m_mmitset.shift_inclusive_range(d, original_size, msev_int(_M));
					auto new_capacity = msev_size_t((*this).capacity());
					bool realloc_occured = (new_capacity != original_capacity);
					if (realloc_occured) {
						m_mmitset.sync_iterators_to_index();
					}
					return retval;
				}
			}
			template<class _Iter
				//>mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value, typename base_class::iterator>
				, class = mse::impl::_mse_RequireInputIter<_Iter> >
				typename base_class::iterator insert(typename base_class::const_iterator _Where, const _Iter& _First, const _Iter& _Last) {	// insert [_First, _Last) at _Where
				structure_change_guard<decltype(m_structure_change_mutex)> lock2(m_structure_change_mutex);
				if (m_mmitset.is_empty()) {
					auto retval = base_class::insert(_Where, _First, _Last);
					/*m_debug_size = size();*/
					return retval;
				}
				else {
					msev_int di = std::distance(base_class::cbegin(), _Where);
					msev_size_t d = msev_size_t(di);
					if ((0 > di) || ((*this).size() < msev_size_t(di))) { MSE_THROW(msebasic_string_range_error("index out of range - typename base_class::iterator insert() - msebasic_string")); }

					auto _M = msev_int(std::distance(_First, _Last));
					auto original_size = msev_size_t((*this).size());
					auto original_capacity = msev_size_t((*this).capacity());

					//if (0 > _M) { MSE_THROW(msebasic_string_range_error("invalid argument - typename base_class::iterator insert() - msebasic_string")); }
					auto retval = base_class::insert(_Where, _First, _Last);
					/*m_debug_size = size();*/

					assert(msev_size_t(original_size + _M) == msev_size_t((*this).size()));
					assert(di == std::distance(base_class::begin(), retval));
					m_mmitset.shift_inclusive_range(d, original_size, _M);
					auto new_capacity = msev_size_t((*this).capacity());
					bool realloc_occured = (new_capacity != original_capacity);
					if (realloc_occured) {
						m_mmitset.sync_iterators_to_index();
					}
					return retval;
				}
			}

#else /*!(defined(GPP4P8_COMPATIBLE))*/

			/*typename base_class::iterator*/
			void
				/* g++4.8 seems to be using the c++98 version of this insert function instead of the c++11 version. */
				insert(typename base_class::/*const_*/iterator _P, size_t _M, const _Ty& _X) {
				structure_change_guard<decltype(m_structure_change_mutex)> lock2(m_structure_change_mutex);
				msev_int di = std::distance(base_class::/*c*/begin(), _P);
				msev_size_t d = msev_size_t(di);
				if ((0 > di) || (msev_size_t((*this).size()) < di)) { MSE_THROW(msebasic_string_range_error("index out of range - typename base_class::iterator insert() - msebasic_string")); }

				auto original_size = msev_size_t((*this).size());
				auto original_capacity = msev_size_t((*this).capacity());

				/*typename base_class::iterator retval =*/
				base_class::insert(_P, _M, _X);
				/*m_debug_size = size();*/

				assert(msev_size_t(original_size + _M) == msev_size_t((*this).size()));
				/*assert(di == std::distance(base_class::begin(), retval));*/
				m_mmitset.shift_inclusive_range(d, original_size, _M);
				auto new_capacity = msev_size_t((*this).capacity());
				bool realloc_occured = (new_capacity != original_capacity);
				if (realloc_occured) {
					m_mmitset.sync_iterators_to_index();
				}
				/*return retval;*/
			}
			template<class _Iter
				//>mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value>
				, class = mse::impl::_mse_RequireInputIter<_Iter> > void
				insert(typename base_class::/*const_*/iterator _Where, const _Iter& _First, const _Iter& _Last) {	// insert [_First, _Last) at _Where
				structure_change_guard<decltype(m_structure_change_mutex)> lock2(m_structure_change_mutex);
				msev_int di = std::distance(base_class::/*c*/begin(), _Where);
				msev_size_t d = msev_size_t(di);
				if ((0 > di) || (msev_size_t((*this).size()) < di)) { MSE_THROW(msebasic_string_range_error("index out of range - typename base_class::iterator insert() - msebasic_string")); }

				auto _M = msev_int(std::distance(_First, _Last));
				auto original_size = msev_size_t((*this).size());
				auto original_capacity = msev_size_t((*this).capacity());

				//if (0 > _M) { MSE_THROW(msebasic_string_range_error("invalid argument - typename base_class::iterator insert() - msebasic_string")); }
				/*auto retval =*/
				base_class::insert(_Where, _First, _Last);
				/*m_debug_size = size();*/

				assert(msev_size_t(original_size + _M) == msev_size_t((*this).size()));
				/*assert(di == std::distance(base_class::begin(), retval));*/
				m_mmitset.shift_inclusive_range(d, original_size, _M);
				auto new_capacity = msev_size_t((*this).capacity());
				bool realloc_occured = (new_capacity != original_capacity);
				if (realloc_occured) {
					m_mmitset.sync_iterators_to_index();
				}
				/*return retval;*/
			}
#endif /*!(defined(GPP4P8_COMPATIBLE))*/
			template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
			typename base_class::iterator insert(typename base_class::const_iterator _P, const _TStringSection& _X) {
				structure_change_guard<decltype(m_structure_change_mutex)> lock2(m_structure_change_mutex);
				if (m_mmitset.is_empty()) {
					typename base_class::iterator retval = base_class::insert(_P, _X);
					/*m_debug_size = size();*/
					return retval;
				}
				else {
					msev_int di = std::distance(base_class::cbegin(), _P);
					msev_size_t d = msev_size_t(di);
					if ((0 > di) || ((*this).size() < msev_size_t(di))) { MSE_THROW(msebasic_string_range_error("index out of range - typename base_class::iterator insert() - msebasic_string")); }

					auto original_size = msev_size_t((*this).size());
					auto original_capacity = msev_size_t((*this).capacity());

					typename base_class::iterator retval = base_class::insert(_P, _X);
					/*m_debug_size = size();*/

					assert(msev_size_t(original_size + _X.length()) == msev_size_t((*this).size()));
					assert(di == std::distance(base_class::begin(), retval));
					m_mmitset.shift_inclusive_range(d, original_size, msev_int(_X.length()));
					auto new_capacity = msev_size_t((*this).capacity());
					bool realloc_occured = (new_capacity != original_capacity);
					if (realloc_occured) {
						m_mmitset.sync_iterators_to_index();
					}
					return retval;
				}
			}

			template<class ..._Valty>
			void emplace_back(_Valty&& ..._Val)
			{	// insert by moving into element at end
				structure_change_guard<decltype(m_structure_change_mutex)> lock2(m_structure_change_mutex);
				if (m_mmitset.is_empty()) {
					base_class::emplace_back(std::forward<_Valty>(_Val)...);
					/*m_debug_size = size();*/
				}
				else {
					auto original_size = msev_size_t((*this).size());
					auto original_capacity = msev_size_t((*this).capacity());

					base_class::emplace_back(std::forward<_Valty>(_Val)...);
					/*m_debug_size = size();*/

					assert(msev_size_t(original_size + 1) == msev_size_t((*this).size()));
					m_mmitset.shift_inclusive_range(original_size, original_size, 1); /*shift the end markers*/
					auto new_capacity = msev_size_t((*this).capacity());
					bool realloc_occured = (new_capacity != original_capacity);
					if (realloc_occured) {
						m_mmitset.sync_iterators_to_index();
					}
				}
			}
			template<class ..._Valty>
#if !(defined(GPP4P8_COMPATIBLE))
			typename base_class::iterator emplace(typename base_class::const_iterator _Where, _Valty&& ..._Val)
			{	// insert by moving _Val at _Where
#else /*!(defined(GPP4P8_COMPATIBLE))*/
			typename base_class::iterator emplace(typename base_class::/*const_*/iterator _Where, _Valty&& ..._Val)
			{	// insert by moving _Val at _Where
#endif /*!(defined(GPP4P8_COMPATIBLE))*/

				structure_change_guard<decltype(m_structure_change_mutex)> lock2(m_structure_change_mutex);
				if (m_mmitset.is_empty()) {
					auto retval = base_class::emplace(_Where, std::forward<_Valty>(_Val)...);
					/*m_debug_size = size();*/
					return retval;
				}
				else {

#if !(defined(GPP4P8_COMPATIBLE))
					msev_int di = std::distance(base_class::cbegin(), _Where);
#else /*!(defined(GPP4P8_COMPATIBLE))*/
					msev_int di = std::distance(base_class::/*c*/begin(), _Where);
#endif /*!(defined(GPP4P8_COMPATIBLE))*/

					msev_size_t d = msev_size_t(di);
					if ((0 > di) || ((*this).size() < msev_size_t(di))) { MSE_THROW(msebasic_string_range_error("index out of range - typename base_class::iterator emplace() - msebasic_string")); }

					auto original_size = msev_size_t((*this).size());
					auto original_capacity = msev_size_t((*this).capacity());

					auto retval = base_class::emplace(_Where, std::forward<_Valty>(_Val)...);
					/*m_debug_size = size();*/

					assert(msev_size_t(original_size + 1) == msev_size_t((*this).size()));
					assert(di == std::distance(base_class::begin(), retval));
					m_mmitset.shift_inclusive_range(d, original_size, 1);
					auto new_capacity = msev_size_t((*this).capacity());
					bool realloc_occured = (new_capacity != original_capacity);
					if (realloc_occured) {
						m_mmitset.sync_iterators_to_index();
					}
					return retval;
				}
			}
			typename base_class::iterator erase(typename base_class::const_iterator _P) {
				structure_change_guard<decltype(m_structure_change_mutex)> lock2(m_structure_change_mutex);
				if (m_mmitset.is_empty()) {
					typename base_class::iterator retval = base_class::erase(_P);
					/*m_debug_size = size();*/
					return retval;
				}
				else {
					msev_int di = std::distance(base_class::cbegin(), _P);
					msev_size_t d = msev_size_t(di);
					if ((0 > di) || ((*this).size() < msev_size_t(di))) { MSE_THROW(msebasic_string_range_error("index out of range - typename base_class::iterator erase() - msebasic_string")); }

					auto original_size = msev_size_t((*this).size());
					auto original_capacity = msev_size_t((*this).capacity());

					if (base_class::end() == _P) { MSE_THROW(msebasic_string_range_error("invalid argument - typename base_class::iterator erase(typename base_class::const_iterator _P) - msebasic_string")); }
					typename base_class::iterator retval = base_class::erase(_P);
					/*m_debug_size = size();*/

					assert(msev_size_t(original_size - 1) == msev_size_t((*this).size()));
					assert(di == std::distance(base_class::begin(), retval));
					{
						m_mmitset.invalidate_inclusive_range(d, d);
						m_mmitset.shift_inclusive_range(msev_size_t(d + 1), original_size, -1);
					}
					auto new_capacity = msev_size_t((*this).capacity());
					bool realloc_occured = (new_capacity != original_capacity);
					if (realloc_occured) {
						m_mmitset.sync_iterators_to_index();
					}
					return retval;
				}
			}
			typename base_class::iterator erase(typename base_class::const_iterator _F, typename base_class::const_iterator _L) {
				structure_change_guard<decltype(m_structure_change_mutex)> lock2(m_structure_change_mutex);
				if (m_mmitset.is_empty()) {
					typename base_class::iterator retval = base_class::erase(_F, _L);
					/*m_debug_size = size();*/
					return retval;
				}
				else {
					msev_int di = std::distance(base_class::cbegin(), _F);
					msev_size_t d = msev_size_t(di);
					if ((0 > di) || ((*this).size() < msev_size_t(di))) { MSE_THROW(msebasic_string_range_error("index out of range - typename base_class::iterator erase() - msebasic_string")); }
					msev_int di2 = std::distance(base_class::cbegin(), _L);
					if ((0 > di2) || ((*this).size() < msev_size_t(di2))) { MSE_THROW(msebasic_string_range_error("index out of range - typename base_class::iterator erase() - msebasic_string")); }

					auto _M = msev_int(std::distance(_F, _L));
					auto original_size = msev_size_t((*this).size());
					auto original_capacity = msev_size_t((*this).capacity());

					if ((base_class::end() == _F)/* || (0 > _M)*/) { MSE_THROW(msebasic_string_range_error("invalid argument - typename base_class::iterator erase(typename base_class::iterator _F, typename base_class::iterator _L) - msebasic_string")); }
					typename base_class::iterator retval = base_class::erase(_F, _L);
					/*m_debug_size = size();*/

					assert(msev_size_t(original_size - _M) == msev_size_t((*this).size()));
					assert(di == std::distance(base_class::begin(), retval));
					{
						if (1 <= _M) {
							m_mmitset.invalidate_inclusive_range(d, msev_size_t(d + _M - 1));
						}
						m_mmitset.shift_inclusive_range(msev_size_t(d + _M), original_size, -_M);
					}
					auto new_capacity = msev_size_t((*this).capacity());
					bool realloc_occured = (new_capacity != original_capacity);
					if (realloc_occured) {
						m_mmitset.sync_iterators_to_index();
					}
					return retval;
				}
			}
			void clear() {
				structure_change_guard<decltype(m_structure_change_mutex)> lock2(m_structure_change_mutex);
				base_class::clear();
				/*m_debug_size = size();*/
				m_mmitset.reset();
			}
			void swap(std::basic_string<_Ty, _Traits, _A>& _X) {
				structure_change_guard<decltype(m_structure_change_mutex)> lock2(m_structure_change_mutex);
				base_class::swap(_X);
				/*m_debug_size = size();*/
				m_mmitset.reset();
			}
			void swap(base_class& _X) {
				structure_change_guard<decltype(m_structure_change_mutex)> lock2(m_structure_change_mutex);
				base_class::swap(_X);
				/*m_debug_size = size();*/
				m_mmitset.reset();
			}
			void swap(_Myt& _X) {
				(*this).swap(static_cast<base_class&>(_X));
				m_mmitset.reset();
			}
			template<typename _TStateMutex2, template<typename> class _TTXScopeConstIterator2>
			void swap(mse::us::impl::gnii_basic_string<_Ty, _Traits, _A, _TStateMutex2, _TTXScopeConstIterator2>& _X) {
				structure_change_guard<decltype(m_structure_change_mutex)> lock2(m_structure_change_mutex);
				base_class::swap(_X);
				/*m_debug_size = size();*/
				m_mmitset.reset();
			}

			msebasic_string(_XSTD initializer_list<typename base_class::value_type> _Ilist,
				const _A& _Al = _A())
				: base_class(_Ilist, _Al), m_mmitset(*this) {	// construct from initializer_list
																/*m_debug_size = size();*/
			}
			_Myt& operator=(_XSTD initializer_list<typename base_class::value_type> _Ilist) {	// assign initializer_list
				structure_change_guard<decltype(m_structure_change_mutex)> lock2(m_structure_change_mutex);
				base_class::operator=(_Ilist);
				m_mmitset.reset();
				return (*this);
			}
			void assign(_XSTD initializer_list<typename base_class::value_type> _Ilist) {	// assign initializer_list
				structure_change_guard<decltype(m_structure_change_mutex)> lock2(m_structure_change_mutex);
				base_class::assign(_Ilist);
				/*m_debug_size = size();*/
				m_mmitset.reset();
			}
#if defined(GPP4P8_COMPATIBLE)
			/* g++4.8 seems to be (incorrectly) using the c++98 version of this insert function instead of the c++11 version. */
			/*typename base_class::iterator*/void insert(typename base_class::/*const_*/iterator _Where, _XSTD initializer_list<typename base_class::value_type> _Ilist) {	// insert initializer_list
				structure_change_guard<decltype(m_structure_change_mutex)> lock2(m_structure_change_mutex);
				msev_int di = std::distance(base_class::/*c*/begin(), _Where);
				msev_size_t d = msev_size_t(di);
				if ((0 > di) || (msev_size_t((*this).size()) < di)) { MSE_THROW(msebasic_string_range_error("index out of range - typename base_class::iterator insert() - msebasic_string")); }

				auto _M = _Ilist.size();
				auto original_size = msev_size_t((*this).size());
				auto original_capacity = msev_size_t((*this).capacity());

				/*auto retval = */base_class::insert(_Where, _Ilist);
				/*m_debug_size = size();*/

				assert(msev_size_t(original_size + _M) == msev_size_t((*this).size()));
				/*assert(di == std::distance(base_class::begin(), retval));*/
				m_mmitset.shift_inclusive_range(d, original_size, _M);
				auto new_capacity = msev_size_t((*this).capacity());
				bool realloc_occured = (new_capacity != original_capacity);
				if (realloc_occured) {
					m_mmitset.sync_iterators_to_index();
				}
				/*return retval;*/
			}
#else /*defined(GPP4P8_COMPATIBLE)*/
			typename base_class::iterator insert(typename base_class::const_iterator _Where, _XSTD initializer_list<typename base_class::value_type> _Ilist) {	// insert initializer_list
				structure_change_guard<decltype(m_structure_change_mutex)> lock2(m_structure_change_mutex);
				if (m_mmitset.is_empty()) {
					auto retval = base_class::insert(_Where, _Ilist);
					/*m_debug_size = size();*/
					return retval;
				}
				else {
					msev_int di = std::distance(base_class::cbegin(), _Where);
					msev_size_t d = msev_size_t(di);
					if ((0 > di) || ((*this).size() < msev_size_t(di))) { MSE_THROW(msebasic_string_range_error("index out of range - typename base_class::iterator insert() - msebasic_string")); }

					auto _M = _Ilist.size();
					auto original_size = msev_size_t((*this).size());
					auto original_capacity = msev_size_t((*this).capacity());

					auto retval = base_class::insert(_Where, _Ilist);
					/*m_debug_size = size();*/

					assert(msev_size_t(original_size + _M) == msev_size_t((*this).size()));
					assert(di == std::distance(base_class::begin(), retval));
					m_mmitset.shift_inclusive_range(d, original_size, msev_int(_M));
					auto new_capacity = msev_size_t((*this).capacity());
					bool realloc_occured = (new_capacity != original_capacity);
					if (realloc_occured) {
						m_mmitset.sync_iterators_to_index();
					}
					return retval;
				}
			}
#endif /*defined(GPP4P8_COMPATIBLE)*/

			//size_t m_debug_size;

			iterator begin() _NOEXCEPT {	// return iterator for beginning of mutable sequence
				return base_class::contained_basic_string().begin();
			}
			const_iterator begin() const _NOEXCEPT {	// return iterator for beginning of nonmutable sequence
				return base_class::contained_basic_string().begin();
			}
			iterator end() _NOEXCEPT {	// return iterator for end of mutable sequence
				return base_class::contained_basic_string().end();
			}
			const_iterator end() const _NOEXCEPT {	// return iterator for beginning of nonmutable sequence
				return base_class::contained_basic_string().end();
			}
			reverse_iterator rbegin() _NOEXCEPT {	// return iterator for beginning of reversed mutable sequence
				return base_class::contained_basic_string().rbegin();
			}
			const_reverse_iterator rbegin() const _NOEXCEPT {	// return iterator for beginning of reversed nonmutable sequence
				return base_class::contained_basic_string().rbegin();
			}
			reverse_iterator rend() _NOEXCEPT {	// return iterator for end of reversed mutable sequence
				return base_class::contained_basic_string().rend();
			}
			const_reverse_iterator rend() const _NOEXCEPT {	// return iterator for end of reversed nonmutable sequence
				return base_class::contained_basic_string().rend();
			}
			const_iterator cbegin() const _NOEXCEPT {	// return iterator for beginning of nonmutable sequence
				return base_class::contained_basic_string().cbegin();
			}
			const_iterator cend() const _NOEXCEPT {	// return iterator for end of nonmutable sequence
				return base_class::contained_basic_string().cend();
			}
			const_reverse_iterator crbegin() const _NOEXCEPT {	// return iterator for beginning of reversed nonmutable sequence
				return base_class::contained_basic_string().crbegin();
			}
			const_reverse_iterator crend() const _NOEXCEPT {	// return iterator for end of reversed nonmutable sequence
				return base_class::contained_basic_string().crend();
			}

			//class random_access_const_iterator_base : public std::iterator<std::random_access_iterator_tag, value_type, difference_type, const_pointer, const_reference> {};
			//class random_access_iterator_base : public std::iterator<std::random_access_iterator_tag, value_type, difference_type, pointer, reference> {};
			class random_access_const_iterator_base {
			public:
				using iterator_category = std::random_access_iterator_tag;
				MSE_INHERITED_RANDOM_ACCESS_MEMBER_TYPE_DECLARATIONS(_Myt);
			};
			class random_access_iterator_base {
			public:
				using iterator_category = std::random_access_iterator_tag;
				MSE_INHERITED_RANDOM_ACCESS_MEMBER_TYPE_DECLARATIONS(_Myt);
			};

			/* mm_const_iterator_type acts much like a list iterator. */
			class mm_const_iterator_type : public random_access_const_iterator_base {
			public:
				typedef typename base_class::const_iterator::iterator_category iterator_category;
				typedef typename base_class::const_iterator::value_type value_type;
				//typedef typename base_class::const_iterator::difference_type difference_type;
				typedef msev_int difference_type;
				typedef difference_type distance_type;	// retained
				typedef typename base_class::const_iterator::pointer pointer;
				typedef typename base_class::const_iterator::pointer const_pointer;
				typedef typename base_class::const_iterator::reference reference;
				typedef typename base_class::const_reference const_reference;

				void reset() { set_to_end_marker(); }
				bool points_to_an_item() const {
					if (m_points_to_an_item) { assert((1 <= m_owner_cptr->size()) && (m_index < m_owner_cptr->size())); return true; }
					else { assert(!((1 <= m_owner_cptr->size()) && (m_index < m_owner_cptr->size()))); return false; }
				}
				bool points_to_end_marker() const {
					if (false == points_to_an_item()) { assert(m_index == m_owner_cptr->size()); return true; }
					else { return false; }
				}
				bool points_to_beginning() const {
					if (0 == m_index) { return true; }
					else { return false; }
				}
				/* has_next_item_or_end_marker() is just an alias for points_to_an_item(). */
				bool has_next_item_or_end_marker() const { return points_to_an_item(); } //his is
																						 /* has_next() is just an alias for points_to_an_item() that's familiar to java programmers. */
				bool has_next() const { return has_next_item_or_end_marker(); }
				bool has_previous() const { return (!points_to_beginning()); }
				void set_to_beginning() {
					m_index = 0;
					if (1 <= m_owner_cptr->size()) {
						m_points_to_an_item = true;
					}
					else { assert(false == m_points_to_an_item); }
				}
				void set_to_end_marker() {
					m_index = m_owner_cptr->size();
					m_points_to_an_item = false;
				}
				void set_to_next() {
					if (points_to_an_item()) {
						m_index += 1;
						if (m_owner_cptr->size() <= m_index) {
							(*this).m_points_to_an_item = false;
							if (m_owner_cptr->size() < m_index) { assert(false); reset(); }
						}
					}
					else {
						MSE_THROW(msebasic_string_range_error("attempt to use invalid const_item_pointer - void set_to_next() - mm_const_iterator_type - msebasic_string"));
					}
				}
				void set_to_previous() {
					if (has_previous()) {
						m_index -= 1;
						(*this).m_points_to_an_item = true;
					}
					else {
						MSE_THROW(msebasic_string_range_error("attempt to use invalid const_item_pointer - void set_to_previous() - mm_const_iterator_type - msebasic_string"));
					}
				}
				mm_const_iterator_type& operator ++() { (*this).set_to_next(); return (*this); }
				mm_const_iterator_type operator++(int) { mm_const_iterator_type _Tmp = *this; ++*this; return (_Tmp); }
				mm_const_iterator_type& operator --() { (*this).set_to_previous(); return (*this); }
				mm_const_iterator_type operator--(int) { mm_const_iterator_type _Tmp = *this; --*this; return (_Tmp); }
				void advance(difference_type n) {
					auto new_index = msev_int(m_index) + n;
					if ((0 > new_index) || (m_owner_cptr->size() < msev_size_t(new_index))) {
						MSE_THROW(msebasic_string_range_error("index out of range - void advance(difference_type n) - mm_const_iterator_type - msebasic_string"));
					}
					else {
						m_index = msev_size_t(new_index);
						if (m_owner_cptr->size() <= m_index) {
							(*this).m_points_to_an_item = false;
						}
						else {
							(*this).m_points_to_an_item = true;
						}
					}
				}
				void regress(difference_type n) { advance(-n); }
				mm_const_iterator_type& operator +=(difference_type n) { (*this).advance(n); return (*this); }
				mm_const_iterator_type& operator -=(difference_type n) { (*this).regress(n); return (*this); }
				mm_const_iterator_type operator+(difference_type n) const {
					mm_const_iterator_type retval(*this);
					retval = (*this);
					retval.advance(n);
					return retval;
				}
				mm_const_iterator_type operator-(difference_type n) const { return ((*this) + (-n)); }
				difference_type operator-(const mm_const_iterator_type &rhs) const {
					if ((rhs.m_owner_cptr) != ((*this).m_owner_cptr)) { MSE_THROW(msebasic_string_range_error("invalid argument - difference_type operator-(const mm_const_iterator_type &rhs) const - msebasic_string::mm_const_iterator_type")); }
					auto retval = difference_type((*this).m_index) - difference_type(rhs.m_index);
					assert(difference_type(m_owner_cptr->size()) >= retval);
					return retval;
				}
				const_reference operator*() const {
					return m_owner_cptr->at(msev_as_a_size_t(m_index));
				}
				const_reference item() const { return operator*(); }
				const_reference previous_item() const {
					return m_owner_cptr->at(msev_as_a_size_t(m_index - 1));
				}
				const_pointer operator->() const {
					return std::addressof(m_owner_cptr->at(msev_as_a_size_t(m_index)));
				}
				const_reference operator[](difference_type _Off) const { return (*m_owner_cptr).at(msev_as_a_size_t(difference_type(m_index) + _Off)); }
				/*
				mm_const_iterator_type& operator=(const typename base_class::const_iterator& _Right_cref)
				{
				msev_int d = std::distance<typename base_class::iterator>(m_owner_cptr->cbegin(), _Right_cref);
				if ((0 <= d) && (m_owner_cptr->size() >= d)) {
				if (m_owner_cptr->size() == d) {
				assert(m_owner_cptr->cend() == _Right_cref);
				m_points_to_an_item = false;
				} else {
				m_points_to_an_item = true;
				}
				m_index = msev_size_t(d);
				base_class::const_iterator::operator=(_Right_cref);
				}
				else {
				MSE_THROW(msebasic_string_range_error("doesn't seem to be a valid assignment value - mm_const_iterator_type& operator=(const typename base_class::const_iterator& _Right_cref) - mm_const_iterator_type - msebasic_string"));
				}
				return (*this);
				}
				*/
				mm_const_iterator_type& operator=(const mm_const_iterator_type& _Right_cref)
				{
					if (((*this).m_owner_cptr) == (_Right_cref.m_owner_cptr)) {
						assert((*this).m_owner_cptr->size() >= _Right_cref.m_index);
						(*this).m_points_to_an_item = _Right_cref.m_points_to_an_item;
						(*this).m_index = _Right_cref.m_index;
					}
					else {
						MSE_THROW(msebasic_string_range_error("doesn't seem to be a valid assignment value - mm_const_iterator_type& operator=(const typename base_class::iterator& _Right_cref) - mm_const_iterator_type - msebasic_string"));
					}
					return (*this);
				}
				bool operator==(const mm_const_iterator_type& _Right_cref) const {
					if (((*this).m_owner_cptr) != (_Right_cref.m_owner_cptr)) { MSE_THROW(msebasic_string_range_error("invalid argument - mm_const_iterator_type& operator==(const mm_const_iterator_type& _Right) - mm_const_iterator_type - msebasic_string")); }
					return (_Right_cref.m_index == m_index);
				}
				bool operator!=(const mm_const_iterator_type& _Right_cref) const { return (!(_Right_cref == (*this))); }
				bool operator<(const mm_const_iterator_type& _Right) const {
					if (((*this).m_owner_cptr) != (_Right.m_owner_cptr)) { MSE_THROW(msebasic_string_range_error("invalid argument - mm_const_iterator_type& operator<(const mm_const_iterator_type& _Right) - mm_const_iterator_type - msebasic_string")); }
					return (m_index < _Right.m_index);
				}
				bool operator<=(const mm_const_iterator_type& _Right) const { return (((*this) < _Right) || (_Right == (*this))); }
				bool operator>(const mm_const_iterator_type& _Right) const { return (!((*this) <= _Right)); }
				bool operator>=(const mm_const_iterator_type& _Right) const { return (!((*this) < _Right)); }
				void set_to_const_item_pointer(const mm_const_iterator_type& _Right_cref) {
					(*this) = _Right_cref;
				}
				void invalidate_inclusive_range(msev_size_t index_of_first, msev_size_t index_of_last) {
					if ((index_of_first <= (*this).m_index) && (index_of_last >= (*this).m_index)) {
						(*this).reset();
					}
				}
				void shift_inclusive_range(msev_size_t index_of_first, msev_size_t index_of_last, msev_int shift) {
					if ((index_of_first <= (*this).m_index) && (index_of_last >= (*this).m_index)) {
						auto new_index = (*this).m_index + shift;
						if ((0 > new_index) || (m_owner_cptr->size() < new_index)) {
							MSE_THROW(msebasic_string_range_error("void shift_inclusive_range() - mm_const_iterator_type - msebasic_string"));
						}
						else {
							(*this).m_index = msev_size_t(new_index);
							(*this).sync_const_iterator_to_index();
						}
					}
				}
				msev_size_t position() const {
					return m_index;
				}
				operator typename base_class::const_iterator() const {
					typename base_class::const_iterator retval = (*m_owner_cptr).cbegin();
					retval += msev_as_a_size_t(m_index);
					return retval;
				}
				void async_not_shareable_and_not_passable_tag() const {}

														/* We actually want to make this constructor private, but doing so seems to break std::make_shared<mm_const_iterator_type>.  */
				mm_const_iterator_type(const _Myt& owner_cref) : m_owner_cptr(&owner_cref) { set_to_beginning(); }
			private:
				mm_const_iterator_type(const mm_const_iterator_type& src_cref) : m_owner_cptr(src_cref.m_owner_cptr) { (*this) = src_cref; }
				void sync_const_iterator_to_index() {
					assert(m_owner_cptr->size() >= (*this).m_index);
				}
				msev_bool m_points_to_an_item = false;
				msev_size_t m_index = 0;
				const _Myt* m_owner_cptr = nullptr;
				friend class mm_iterator_set_type;
				friend class /*_Myt*/msebasic_string<_Ty, _Traits, _A, _TStateMutex>;
				friend class mm_iterator_type;
			};
			/* mm_iterator_type acts much like a list iterator. */
			class mm_iterator_type : random_access_iterator_base {
			public:
				typedef typename base_class::iterator::iterator_category iterator_category;
				typedef typename base_class::iterator::value_type value_type;
				//typedef typename base_class::iterator::difference_type difference_type;
				typedef msev_int difference_type;
				typedef difference_type distance_type;	// retained
				typedef typename base_class::iterator::pointer pointer;
				typedef typename base_class::iterator::reference reference;

				void reset() { set_to_end_marker(); }
				bool points_to_an_item() const {
					if (m_points_to_an_item) { assert((1 <= m_owner_ptr->size()) && (m_index < m_owner_ptr->size())); return true; }
					else { assert(!((1 <= m_owner_ptr->size()) && (m_index < m_owner_ptr->size())));  return false; }
				}
				bool points_to_end_marker() const {
					if (false == points_to_an_item()) { assert(m_index == m_owner_ptr->size()); return true; }
					else { return false; }
				}
				bool points_to_beginning() const {
					if (0 == m_index) { return true; }
					else { return false; }
				}
				/* has_next_item_or_end_marker() is just an alias for points_to_an_item(). */
				bool has_next_item_or_end_marker() const { return points_to_an_item(); }
				/* has_next() is just an alias for points_to_an_item() that's familiar to java programmers. */
				bool has_next() const { return has_next_item_or_end_marker(); }
				bool has_previous() const { return (!points_to_beginning()); }
				void set_to_beginning() {
					m_index = 0;
					if (1 <= m_owner_ptr->size()) {
						m_points_to_an_item = true;
					}
					else { assert(false == m_points_to_an_item); }
				}
				void set_to_end_marker() {
					m_index = msev_size_t(m_owner_ptr->size());
					m_points_to_an_item = false;
				}
				void set_to_next() {
					if (points_to_an_item()) {
						m_index += 1;
						if (m_owner_ptr->size() <= m_index) {
							(*this).m_points_to_an_item = false;
							if (m_owner_ptr->size() < m_index) { assert(false); reset(); }
						}
					}
					else {
						MSE_THROW(msebasic_string_range_error("attempt to use invalid item_pointer - void set_to_next() - mm_const_iterator_type - msebasic_string"));
					}
				}
				void set_to_previous() {
					if (has_previous()) {
						m_index -= 1;
						(*this).m_points_to_an_item = true;
					}
					else {
						MSE_THROW(msebasic_string_range_error("attempt to use invalid item_pointer - void set_to_previous() - mm_iterator_type - msebasic_string"));
					}
				}
				mm_iterator_type& operator ++() { (*this).set_to_next(); return (*this); }
				mm_iterator_type operator++(int) { mm_iterator_type _Tmp = *this; ++*this; return (_Tmp); }
				mm_iterator_type& operator --() { (*this).set_to_previous(); return (*this); }
				mm_iterator_type operator--(int) { mm_iterator_type _Tmp = *this; --*this; return (_Tmp); }
				void advance(difference_type n) {
					auto new_index = msev_int(m_index) + n;
					if ((0 > new_index) || (m_owner_ptr->size() < msev_size_t(new_index))) {
						MSE_THROW(msebasic_string_range_error("index out of range - void advance(difference_type n) - mm_iterator_type - msebasic_string"));
					}
					else {
						m_index = msev_size_t(new_index);
						if (m_owner_ptr->size() <= m_index) {
							(*this).m_points_to_an_item = false;
						}
						else {
							(*this).m_points_to_an_item = true;
						}
					}
				}
				void regress(int n) { advance(-n); }
				mm_iterator_type& operator +=(difference_type n) { (*this).advance(n); return (*this); }
				mm_iterator_type& operator -=(difference_type n) { (*this).regress(n); return (*this); }
				mm_iterator_type operator+(difference_type n) const {
					mm_iterator_type retval(*this);
					retval = (*this);
					retval.advance(n);
					return retval;
				}
				mm_iterator_type operator-(difference_type n) const { return ((*this) + (-n)); }
				difference_type operator-(const mm_iterator_type& rhs) const {
					if ((rhs.m_owner_ptr) != ((*this).m_owner_ptr)) { MSE_THROW(msebasic_string_range_error("invalid argument - difference_type operator-(const mm_iterator_type& rhs) const - msebasic_string::mm_iterator_type")); }
					auto retval = difference_type((*this).m_index) - difference_type(rhs.m_index);
					assert(difference_type(m_owner_ptr->size()) >= retval);
					return retval;
				}
				reference operator*() const {
					return m_owner_ptr->at(msev_as_a_size_t(m_index));
				}
				reference item() const { return operator*(); }
				reference previous_item() const {
					return m_owner_ptr->at(msev_as_a_size_t(m_index - 1));
				}
				pointer operator->() const {
					return std::addressof(m_owner_ptr->at(msev_as_a_size_t(m_index)));
				}
				reference operator[](difference_type _Off) const { return (*m_owner_ptr).at(msev_as_a_size_t(difference_type(m_index) + _Off)); }
				/*
				mm_iterator_type& operator=(const typename base_class::iterator& _Right_cref)
				{
				msev_int d = std::distance<typename base_class::iterator>(m_owner_ptr->begin(), _Right_cref);
				if ((0 <= d) && (m_owner_ptr->size() >= d)) {
				if (m_owner_ptr->size() == d) {
				assert(m_owner_ptr->end() == _Right_cref);
				m_points_to_an_item = false;
				} else {
				m_points_to_an_item = true;
				}
				m_index = msev_size_t(d);
				base_class::iterator::operator=(_Right_cref);
				}
				else {
				MSE_THROW(msebasic_string_range_error("doesn't seem to be a valid assignment value - mm_iterator_type& operator=(const typename base_class::iterator& _Right_cref) - mm_const_iterator_type - msebasic_string"));
				}
				return (*this);
				}
				*/
				mm_iterator_type& operator=(const mm_iterator_type& _Right_cref)
				{
					if (((*this).m_owner_ptr) == (_Right_cref.m_owner_ptr)) {
						assert((*this).m_owner_ptr->size() >= _Right_cref.m_index);
						(*this).m_points_to_an_item = _Right_cref.m_points_to_an_item;
						(*this).m_index = _Right_cref.m_index;
					}
					else {
						MSE_THROW(msebasic_string_range_error("doesn't seem to be a valid assignment value - mm_iterator_type& operator=(const typename base_class::iterator& _Right_cref) - mm_const_iterator_type - msebasic_string"));
					}
					return (*this);
				}
				bool operator==(const mm_iterator_type& _Right_cref) const {
					if (((*this).m_owner_ptr) != (_Right_cref.m_owner_ptr)) { MSE_THROW(msebasic_string_range_error("invalid argument - mm_iterator_type& operator==(const typename base_class::iterator& _Right) - mm_iterator_type - msebasic_string")); }
					return (_Right_cref.m_index == m_index);
				}
				bool operator!=(const mm_iterator_type& _Right_cref) const { return (!(_Right_cref == (*this))); }
				bool operator<(const mm_iterator_type& _Right) const {
					if (((*this).m_owner_ptr) != (_Right.m_owner_ptr)) { MSE_THROW(msebasic_string_range_error("invalid argument - mm_iterator_type& operator<(const typename base_class::iterator& _Right) - mm_iterator_type - msebasic_string")); }
					return (m_index < _Right.m_index);
				}
				bool operator<=(const mm_iterator_type& _Right) const { return (((*this) < _Right) || (_Right == (*this))); }
				bool operator>(const mm_iterator_type& _Right) const { return (!((*this) <= _Right)); }
				bool operator>=(const mm_iterator_type& _Right) const { return (!((*this) < _Right)); }
				void set_to_item_pointer(const mm_iterator_type& _Right_cref) {
					(*this) = _Right_cref;
				}
				void invalidate_inclusive_range(msev_size_t index_of_first, msev_size_t index_of_last) {
					if ((index_of_first <= (*this).m_index) && (index_of_last >= (*this).m_index)) {
						(*this).reset();
					}
				}
				void shift_inclusive_range(msev_size_t index_of_first, msev_size_t index_of_last, msev_int shift) {
					if ((index_of_first <= (*this).m_index) && (index_of_last >= (*this).m_index)) {
						auto new_index = (*this).m_index + shift;
						if ((0 > new_index) || (m_owner_ptr->size() < new_index)) {
							MSE_THROW(msebasic_string_range_error("void shift_inclusive_range() - mm_iterator_type - msebasic_string"));
						}
						else {
							(*this).m_index = msev_size_t(new_index);
							(*this).sync_iterator_to_index();
						}
					}
				}
				msev_size_t position() const {
					return m_index;
				}
				operator mm_const_iterator_type() const {
					mm_const_iterator_type retval(*m_owner_ptr);
					retval.set_to_beginning();
					retval.advance(msev_int(m_index));
					return retval;
				}
				void async_not_shareable_and_not_passable_tag() const {}

														/* We actually want to make this constructor private, but doing so seems to break std::make_shared<mm_iterator_type>.  */
				mm_iterator_type(_Myt& owner_ref) : m_owner_ptr(&owner_ref) { set_to_beginning(); }
			private:
				mm_iterator_type(const mm_iterator_type& src_cref) : m_owner_ptr(src_cref.m_owner_ptr) { (*this) = src_cref; }
				void sync_iterator_to_index() {
					assert(m_owner_ptr->size() >= (*this).m_index);
				}
				msev_bool m_points_to_an_item = false;
				msev_size_t m_index = 0;
				_Myt* m_owner_ptr = nullptr;
				friend class mm_iterator_set_type;
				friend class /*_Myt*/msebasic_string<_Ty, _Traits, _A, _TStateMutex>;
			};

		private:
			typedef std::size_t CHashKey1;
			class mm_const_iterator_handle_type {
			public:
				mm_const_iterator_handle_type(const CHashKey1& key_cref, const std::shared_ptr<mm_const_iterator_type>& shptr_cref) : m_shptr(shptr_cref), m_key(key_cref) {}
			private:
				std::shared_ptr<mm_const_iterator_type> m_shptr;
				CHashKey1 m_key;
				friend class /*_Myt*/msebasic_string<_Ty, _Traits, _A, _TStateMutex>;
				friend class mm_iterator_set_type;
			};
			class mm_iterator_handle_type {
			public:
				mm_iterator_handle_type(const CHashKey1& key_cref, const std::shared_ptr<mm_iterator_type>& shptr_ref) : m_shptr(shptr_ref), m_key(key_cref) {}
			private:
				std::shared_ptr<mm_iterator_type> m_shptr;
				CHashKey1 m_key;
				friend class /*_Myt*/msebasic_string<_Ty, _Traits, _A, _TStateMutex>;
				friend class mm_iterator_set_type;
			};

			class mm_iterator_set_type {
			public:
				class CMMConstIterators : public std::unordered_map<CHashKey1, std::shared_ptr<mm_const_iterator_type>> {};
				class CMMIterators : public std::unordered_map<CHashKey1, std::shared_ptr<mm_iterator_type>> {};

				class assignable_CMMConstIterators_value_type : public std::pair<CHashKey1, std::shared_ptr<mm_const_iterator_type>> {
				public:
					assignable_CMMConstIterators_value_type() {}
					assignable_CMMConstIterators_value_type(const typename CMMConstIterators::value_type& src) : std::pair<CHashKey1, std::shared_ptr<mm_iterator_type>>(src.first, src.second) {}
					assignable_CMMConstIterators_value_type& operator=(const typename CMMConstIterators::value_type& rhs) { (*this).first = rhs.first; (*this).second = rhs.second; return (*this); }
					operator typename CMMConstIterators::value_type() const { return typename CMMConstIterators::value_type((*this).first, (*this).second); }
				};
				class assignable_CMMIterators_value_type : public std::pair<CHashKey1, std::shared_ptr<mm_iterator_type>> {
				public:
					assignable_CMMIterators_value_type() {}
					assignable_CMMIterators_value_type(const typename CMMIterators::value_type& src) : std::pair<CHashKey1, std::shared_ptr<mm_iterator_type>>(src.first, src.second) {}
					assignable_CMMIterators_value_type& operator=(const typename CMMIterators::value_type& rhs) { (*this).first = rhs.first; (*this).second = rhs.second; return (*this); }
					operator typename CMMIterators::value_type() const { return typename CMMIterators::value_type((*this).first, (*this).second); }
				};

				~mm_iterator_set_type() {
					if (!mm_const_fast_mode1()) {
						delete m_aux_mm_const_iterator_shptrs_ptr;
					}
					if (!mm_fast_mode1()) {
						delete m_aux_mm_iterator_shptrs_ptr;
					}
				}

				void apply_to_all_mm_const_iterator_shptrs(const std::function<void(std::shared_ptr<mm_const_iterator_type>&)>& func_obj_ref) {
					if (!mm_const_fast_mode1()) {
						for (auto it = (*m_aux_mm_const_iterator_shptrs_ptr).begin(); (*m_aux_mm_const_iterator_shptrs_ptr).end() != it; it++) {
							func_obj_ref((*it).second);
						}
					}
					else {
						for (int i = 0; i < m_fm1_num_mm_const_iterators; i += 1) {
							func_obj_ref(m_fm1_key_mm_const_it_array[i].second);
						}
					}
				}
				void apply_to_all_mm_iterator_shptrs(const std::function<void(std::shared_ptr<mm_iterator_type>&)>& func_obj_ref) {
					if (!mm_fast_mode1()) {
						for (auto it = (*m_aux_mm_iterator_shptrs_ptr).begin(); (*m_aux_mm_iterator_shptrs_ptr).end() != it; it++) {
							func_obj_ref((*it).second);
						}
					}
					else {
						for (int i = 0; i < m_fm1_num_mm_iterators; i += 1) {
							func_obj_ref(m_fm1_key_mm_it_array[i].second);
						}
					}
				}
				mm_iterator_set_type(_Myt& owner_ref) : m_next_available_key(0), m_owner_ptr(&owner_ref) {}
				void reset() {
					/* We can use "static" here because the lambda function does not capture any parameters. */
					static const std::function<void(std::shared_ptr<mm_const_iterator_type>&)> cit_func_obj = [](std::shared_ptr<mm_const_iterator_type>& a) { a->reset(); };
					apply_to_all_mm_const_iterator_shptrs(cit_func_obj);
					static const std::function<void(std::shared_ptr<mm_iterator_type>&)> it_func_obj = [](std::shared_ptr<mm_iterator_type>& a) { a->reset(); };
					apply_to_all_mm_iterator_shptrs(it_func_obj);
				}
				void sync_iterators_to_index() {
					/* No longer used. Relic from when mm_iterator_type contained a "native" iterator. */
					/* We can use "static" here because the lambda function does not capture any parameters. */
					/*
					static const std::function<void(std::shared_ptr<mm_const_iterator_type>&)> cit_func_obj = [](std::shared_ptr<mm_const_iterator_type>& a) { a->sync_const_iterator_to_index(); };
					apply_to_all_mm_const_iterator_shptrs(cit_func_obj);
					static const std::function<void(std::shared_ptr<mm_iterator_type>&)> it_func_obj = [](std::shared_ptr<mm_iterator_type>& a) { a->sync_iterator_to_index(); };
					apply_to_all_mm_iterator_shptrs(it_func_obj);
					*/
				}
				void invalidate_inclusive_range(msev_size_t start_index, msev_size_t end_index) {
					const std::function<void(std::shared_ptr<mm_const_iterator_type>&)> cit_func_obj = [start_index, end_index](std::shared_ptr<mm_const_iterator_type>& a) { a->invalidate_inclusive_range(start_index, end_index); };
					apply_to_all_mm_const_iterator_shptrs(cit_func_obj);
					const std::function<void(std::shared_ptr<mm_iterator_type>&)> it_func_obj = [start_index, end_index](std::shared_ptr<mm_iterator_type>& a) { a->invalidate_inclusive_range(start_index, end_index); };
					apply_to_all_mm_iterator_shptrs(it_func_obj);
				}
				void shift_inclusive_range(msev_size_t start_index, msev_size_t end_index, msev_int shift) {
					const std::function<void(std::shared_ptr<mm_const_iterator_type>&)> cit_func_obj = [start_index, end_index, shift](std::shared_ptr<mm_const_iterator_type>& a) { a->shift_inclusive_range(start_index, end_index, shift); };
					apply_to_all_mm_const_iterator_shptrs(cit_func_obj);
					const std::function<void(std::shared_ptr<mm_iterator_type>&)> it_func_obj = [start_index, end_index, shift](std::shared_ptr<mm_iterator_type>& a) { a->shift_inclusive_range(start_index, end_index, shift); };
					apply_to_all_mm_iterator_shptrs(it_func_obj);
				}
				bool is_empty() const {
					if (mm_const_fast_mode1()) {
						if (1 <= m_fm1_num_mm_const_iterators) {
							return false;
						}
					}
					else {
						if (1 <= m_aux_mm_const_iterator_shptrs_ptr->size()) {
							return false;
						}
					}
					if (mm_fast_mode1()) {
						if (1 <= m_fm1_num_mm_iterators) {
							return false;
						}
					}
					else {
						if (1 <= m_aux_mm_iterator_shptrs_ptr->size()) {
							return false;
						}
					}
					return true;
				}

				mm_const_iterator_handle_type allocate_new_const_item_pointer() {
					//auto shptr = std::shared_ptr<mm_const_iterator_type>(new mm_const_iterator_type(*m_owner_ptr));
					auto shptr = std::make_shared<mm_const_iterator_type>(*m_owner_ptr);
					auto key = m_next_available_key; m_next_available_key++;
					mm_const_iterator_handle_type retval(key, shptr);
					typename CMMConstIterators::value_type new_item(key, shptr);
					if (!mm_const_fast_mode1()) {
						(*m_aux_mm_const_iterator_shptrs_ptr).insert(new_item);
					}
					else {
						if (sc_fm1_max_mm_iterators == m_fm1_num_mm_const_iterators) {
							/* Too many items. Initiate and switch to slow mode. */
							/* Initialize slow storage. */
							m_aux_mm_const_iterator_shptrs_ptr = new CMMConstIterators();
							/* First copy the items from fast storage to slow storage. */
							for (int i = 0; i < sc_fm1_max_mm_iterators; i += 1) {
								(*m_aux_mm_const_iterator_shptrs_ptr).insert(m_fm1_key_mm_const_it_array[i]);
							}
							/* Add the new items to slow storage. */
							(*m_aux_mm_const_iterator_shptrs_ptr).insert(new_item);
						}
						else {
							m_fm1_key_mm_const_it_array[m_fm1_num_mm_const_iterators] = new_item;
							m_fm1_num_mm_const_iterators += 1;
						}
					}
					return retval;
				}
				void release_const_item_pointer(mm_const_iterator_handle_type handle) {
					if (!mm_const_fast_mode1()) {
						auto it = (*m_aux_mm_const_iterator_shptrs_ptr).find(handle.m_key);
						if ((*m_aux_mm_const_iterator_shptrs_ptr).end() != it) {
							(*m_aux_mm_const_iterator_shptrs_ptr).erase(it);
						}
						else {
							/* Do we need to throw here? */
							MSE_THROW(msebasic_string_range_error("invalid handle - void release_aux_mm_const_iterator(mm_const_iterator_handle_type handle) - msebasic_string::mm_iterator_set_type"));
						}
					}
					else {
						int found_index = -1;
						for (int i = 0; i < m_fm1_num_mm_const_iterators; i += 1) {
							if (handle.m_key == m_fm1_key_mm_const_it_array[i].first) {
								found_index = i;
								break;
							}
						}
						if (0 <= found_index) {
							m_fm1_num_mm_const_iterators -= 1;
							assert(0 <= m_fm1_num_mm_const_iterators);
							for (int j = found_index; j < m_fm1_num_mm_const_iterators; j += 1) {
								m_fm1_key_mm_const_it_array[j] = m_fm1_key_mm_const_it_array[j + 1];
							}
						}
						else {
							/* Do we need to throw here? */
							MSE_THROW(msebasic_string_range_error("invalid handle - void release_aux_mm_const_iterator(mm_const_iterator_handle_type handle) - msebasic_string::mm_iterator_set_type"));
						}
					}
				}

				mm_iterator_handle_type allocate_new_item_pointer() {
					//auto shptr = std::shared_ptr<mm_iterator_type>(new mm_iterator_type(*m_owner_ptr));
					auto shptr = std::make_shared<mm_iterator_type>(*m_owner_ptr);
					auto key = m_next_available_key; m_next_available_key++;
					mm_iterator_handle_type retval(key, shptr);
					typename CMMIterators::value_type new_item(key, shptr);
					if (!mm_fast_mode1()) {
						(*m_aux_mm_iterator_shptrs_ptr).insert(new_item);
					}
					else {
						if (sc_fm1_max_mm_iterators == m_fm1_num_mm_iterators) {
							/* Too many items. Initiate and switch to slow mode. */
							/* Initialize slow storage. */
							m_aux_mm_iterator_shptrs_ptr = new CMMIterators();
							/* First copy the items from fast storage to slow storage. */
							for (int i = 0; i < sc_fm1_max_mm_iterators; i += 1) {
								(*m_aux_mm_iterator_shptrs_ptr).insert(m_fm1_key_mm_it_array[i]);
							}
							/* Add the new items to slow storage. */
							(*m_aux_mm_iterator_shptrs_ptr).insert(new_item);
						}
						else {
							m_fm1_key_mm_it_array[m_fm1_num_mm_iterators] = new_item;
							m_fm1_num_mm_iterators += 1;
						}
					}
					return retval;
				}
				void release_item_pointer(mm_iterator_handle_type handle) {
					if (!mm_fast_mode1()) {
						auto it = (*m_aux_mm_iterator_shptrs_ptr).find(handle.m_key);
						if ((*m_aux_mm_iterator_shptrs_ptr).end() != it) {
							(*m_aux_mm_iterator_shptrs_ptr).erase(it);
						}
						else {
							/* Do we need to throw here? */
							MSE_THROW(msebasic_string_range_error("invalid handle - void release_aux_mm_iterator(mm_iterator_handle_type handle) - msebasic_string::mm_iterator_set_type"));
						}
					}
					else {
						int found_index = -1;
						for (int i = 0; i < m_fm1_num_mm_iterators; i += 1) {
							if (handle.m_key == m_fm1_key_mm_it_array[i].first) {
								found_index = i;
								break;
							}
						}
						if (0 <= found_index) {
							m_fm1_num_mm_iterators -= 1;
							assert(0 <= m_fm1_num_mm_iterators);
							for (int j = found_index; j < m_fm1_num_mm_iterators; j += 1) {
								m_fm1_key_mm_it_array[j] = m_fm1_key_mm_it_array[j + 1];
							}
						}
						else {
							/* Do we need to throw here? */
							MSE_THROW(msebasic_string_range_error("invalid handle - void release_aux_mm_iterator(mm_iterator_handle_type handle) - msebasic_string::mm_iterator_set_type"));
						}
					}
				}
				void release_all_item_pointers() {
					if (!mm_fast_mode1()) {
						(*m_aux_mm_iterator_shptrs_ptr).clear();
					}
					else {
						for (int i = 0; i < m_fm1_num_mm_iterators; i += 1) {
							m_fm1_key_mm_it_array[i] = assignable_CMMIterators_value_type();
						}
						m_fm1_num_mm_iterators = 0;
					}
				}
				mm_const_iterator_type &const_item_pointer(mm_const_iterator_handle_type handle) const {
					return (*(handle.m_shptr));
				}
				mm_iterator_type &item_pointer(mm_iterator_handle_type handle) {
					return (*(handle.m_shptr));
				}

			private:
				void release_all_const_item_pointers() {
					if (!mm_const_fast_mode1()) {
						(*m_aux_mm_const_iterator_shptrs_ptr).clear();
					}
					else {
						for (int i = 0; i < m_fm1_num_mm_const_iterators; i += 1) {
							m_fm1_key_mm_const_it_array[i] = assignable_CMMConstIterators_value_type();
						}
						m_fm1_num_mm_const_iterators = 0;
					}
				}

				mm_iterator_set_type& operator=(const mm_iterator_set_type& src_cref) {
					/* This is a special type of class. The state (i.e. member values) of an object of this class is specific to (and only
					valid for) the particular instance of the object (or the object of which it is a member). So the correct state of a new
					copy of this type of object is not a copy of the state, but rather the state of a new object (which is just the default
					initialization state). */
					(*this).reset();
					return (*this);
				}
				mm_iterator_set_type& operator=(mm_iterator_set_type&& src) { /* see above */ (*this).reset(); return (*this); }
				mm_iterator_set_type(const mm_iterator_set_type& src) { /* see above */ }
				mm_iterator_set_type(const mm_iterator_set_type&& src) { /* see above */ }

				CHashKey1 m_next_available_key = 0;

				static const int sc_fm1_max_mm_iterators = 6/*arbitrary*/;

				bool mm_const_fast_mode1() const { return (nullptr == m_aux_mm_const_iterator_shptrs_ptr); }
				int m_fm1_num_mm_const_iterators = 0;
				assignable_CMMConstIterators_value_type m_fm1_key_mm_const_it_array[sc_fm1_max_mm_iterators];
				CMMConstIterators* m_aux_mm_const_iterator_shptrs_ptr = nullptr;

				bool mm_fast_mode1() const { return (nullptr == m_aux_mm_iterator_shptrs_ptr); }
				int m_fm1_num_mm_iterators = 0;
				assignable_CMMIterators_value_type m_fm1_key_mm_it_array[sc_fm1_max_mm_iterators];
				CMMIterators* m_aux_mm_iterator_shptrs_ptr = nullptr;

				_Myt* m_owner_ptr = nullptr;

				friend class /*_Myt*/msebasic_string<_Ty, _Traits, _A, _TStateMutex>;
			};
			mutable mm_iterator_set_type m_mmitset;

		public:
			mm_const_iterator_type &const_item_pointer(mm_const_iterator_handle_type handle) const {
				return m_mmitset.const_item_pointer(handle);
			}
			mm_iterator_type &item_pointer(mm_iterator_handle_type handle) {
				return m_mmitset.item_pointer(handle);
			}

		private:
			mm_const_iterator_handle_type allocate_new_const_item_pointer() const { return m_mmitset.allocate_new_const_item_pointer(); }
			void release_const_item_pointer(mm_const_iterator_handle_type handle) const { m_mmitset.release_const_item_pointer(handle); }
			void release_all_const_item_pointers() const { m_mmitset.release_all_const_item_pointers(); }
			mm_iterator_handle_type allocate_new_item_pointer() const { return m_mmitset.allocate_new_item_pointer(); }
			void release_item_pointer(mm_iterator_handle_type handle) const { m_mmitset.release_item_pointer(handle); }
			void release_all_item_pointers() const { m_mmitset.release_all_item_pointers(); }

		public:
			class cipointer : public random_access_const_iterator_base {
			public:
				typedef typename mm_const_iterator_type::iterator_category iterator_category;
				typedef typename mm_const_iterator_type::value_type value_type;
				typedef typename mm_const_iterator_type::difference_type difference_type;
				typedef difference_type distance_type;	// retained
				typedef typename mm_const_iterator_type::pointer pointer;
				typedef typename mm_const_iterator_type::const_pointer const_pointer;
				typedef typename mm_const_iterator_type::reference reference;
				typedef typename mm_const_iterator_type::const_reference const_reference;

				cipointer(const _Myt& owner_cref) : m_owner_cptr(&owner_cref) {
					mm_const_iterator_handle_type handle = m_owner_cptr->allocate_new_const_item_pointer();
					m_handle_shptr = std::make_shared<mm_const_iterator_handle_type>(handle);
				}
				cipointer(const cipointer& src_cref) : m_owner_cptr(src_cref.m_owner_cptr) {
					mm_const_iterator_handle_type handle = m_owner_cptr->allocate_new_const_item_pointer();
					m_handle_shptr = std::make_shared<mm_const_iterator_handle_type>(handle);
					const_item_pointer() = src_cref.const_item_pointer();
				}
				~cipointer() {
					m_owner_cptr->release_const_item_pointer(*m_handle_shptr);
				}
				mm_const_iterator_type& const_item_pointer() const { return m_owner_cptr->const_item_pointer(*m_handle_shptr); }
				mm_const_iterator_type& cip() const { return const_item_pointer(); }
				//const mm_const_iterator_handle_type& handle() const { return (*m_handle_shptr); }

				void reset() { const_item_pointer().reset(); }
				bool points_to_an_item() const { return const_item_pointer().points_to_an_item(); }
				bool points_to_end_marker() const { return const_item_pointer().points_to_end_marker(); }
				bool points_to_beginning() const { return const_item_pointer().points_to_beginning(); }
				/* has_next_item_or_end_marker() is just an alias for points_to_an_item(). */
				bool has_next_item_or_end_marker() const { return const_item_pointer().has_next_item_or_end_marker(); }
				/* has_next() is just an alias for points_to_an_item() that's familiar to java programmers. */
				bool has_next() const { return const_item_pointer().has_next(); }
				bool has_previous() const { return const_item_pointer().has_previous(); }
				void set_to_beginning() { const_item_pointer().set_to_beginning(); }
				void set_to_end_marker() { const_item_pointer().set_to_end_marker(); }
				void set_to_next() { const_item_pointer().set_to_next(); }
				void set_to_previous() { const_item_pointer().set_to_previous(); }
				cipointer& operator ++() { const_item_pointer().operator ++(); return (*this); }
				cipointer operator++(int) { cipointer _Tmp = *this; ++*this; return (_Tmp); }
				cipointer& operator --() { const_item_pointer().operator --(); return (*this); }
				cipointer operator--(int) { cipointer _Tmp = *this; --*this; return (_Tmp); }
				void advance(difference_type n) { const_item_pointer().advance(n); }
				void regress(difference_type n) { const_item_pointer().regress(n); }
				cipointer& operator +=(difference_type n) { const_item_pointer().operator +=(n); return (*this); }
				cipointer& operator -=(difference_type n) { const_item_pointer().operator -=(n); return (*this); }
				cipointer operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
				cipointer operator-(difference_type n) const { return ((*this) + (-n)); }
				difference_type operator-(const cipointer& _Right_cref) const { return const_item_pointer() - (_Right_cref.const_item_pointer()); }
				const_reference operator*() const { return const_item_pointer().operator*(); }
				const_reference item() const { return operator*(); }
				const_reference previous_item() const { return const_item_pointer().previous_item(); }
				const_pointer operator->() const { return const_item_pointer().operator->(); }
				const_reference operator[](difference_type _Off) const { return const_item_pointer()[_Off]; }
				cipointer& operator=(const cipointer& _Right_cref) { const_item_pointer().operator=(_Right_cref.const_item_pointer()); return (*this); }
				bool operator==(const cipointer& _Right_cref) const { return const_item_pointer().operator==(_Right_cref.const_item_pointer()); }
				bool operator!=(const cipointer& _Right_cref) const { return (!(_Right_cref == (*this))); }
				bool operator<(const cipointer& _Right) const { return (const_item_pointer() < _Right.const_item_pointer()); }
				bool operator<=(const cipointer& _Right) const { return (const_item_pointer() <= _Right.const_item_pointer()); }
				bool operator>(const cipointer& _Right) const { return (const_item_pointer() > _Right.const_item_pointer()); }
				bool operator>=(const cipointer& _Right) const { return (const_item_pointer() >= _Right.const_item_pointer()); }
				void set_to_const_item_pointer(const cipointer& _Right_cref) { const_item_pointer().set_to_const_item_pointer(_Right_cref.const_item_pointer()); }
				msev_size_t position() const { return const_item_pointer().position(); }
				auto target_container_ptr() const {
					return m_owner_cptr;
				}
				void async_not_shareable_and_not_passable_tag() const {}
			private:
				const _Myt* m_owner_cptr = nullptr;
				std::shared_ptr<mm_const_iterator_handle_type> m_handle_shptr;
				friend class /*_Myt*/msebasic_string<_Ty, _Traits, _A, _TStateMutex>;
			};
			class ipointer : public random_access_iterator_base {
			public:
				typedef typename mm_iterator_type::iterator_category iterator_category;
				typedef typename mm_iterator_type::value_type value_type;
				typedef typename mm_iterator_type::difference_type difference_type;
				typedef difference_type distance_type;	// retained
				typedef typename mm_iterator_type::pointer pointer;
				typedef typename mm_iterator_type::reference reference;

				ipointer(_Myt& owner_ref) : m_owner_ptr(&owner_ref) {
					mm_iterator_handle_type handle = m_owner_ptr->allocate_new_item_pointer();
					m_handle_shptr = std::make_shared<mm_iterator_handle_type>(handle);
				}
				ipointer(const ipointer& src_cref) : m_owner_ptr(src_cref.m_owner_ptr) {
					mm_iterator_handle_type handle = m_owner_ptr->allocate_new_item_pointer();
					m_handle_shptr = std::make_shared<mm_iterator_handle_type>(handle);
					item_pointer() = src_cref.item_pointer();
				}
				~ipointer() {
					m_owner_ptr->release_item_pointer(*m_handle_shptr);
				}
				mm_iterator_type& item_pointer() const { return m_owner_ptr->item_pointer(*m_handle_shptr); }
				mm_iterator_type& ip() const { return item_pointer(); }
				//const mm_iterator_handle_type& handle() const { return (*m_handle_shptr); }
				operator cipointer() const {
					cipointer retval(*m_owner_ptr);
					retval.const_item_pointer().set_to_beginning();
					retval.const_item_pointer().advance(msev_int(item_pointer().position()));
					return retval;
				}

				void reset() { item_pointer().reset(); }
				bool points_to_an_item() const { return item_pointer().points_to_an_item(); }
				bool points_to_end_marker() const { return item_pointer().points_to_end_marker(); }
				bool points_to_beginning() const { return item_pointer().points_to_beginning(); }
				/* has_next_item_or_end_marker() is just an alias for points_to_an_item(). */
				bool has_next_item_or_end_marker() const { return item_pointer().has_next_item_or_end_marker(); }
				/* has_next() is just an alias for points_to_an_item() that's familiar to java programmers. */
				bool has_next() const { return item_pointer().has_next(); }
				bool has_previous() const { return item_pointer().has_previous(); }
				void set_to_beginning() { item_pointer().set_to_beginning(); }
				void set_to_end_marker() { item_pointer().set_to_end_marker(); }
				void set_to_next() { item_pointer().set_to_next(); }
				void set_to_previous() { item_pointer().set_to_previous(); }
				ipointer& operator ++() { item_pointer().operator ++(); return (*this); }
				ipointer operator++(int) { ipointer _Tmp = *this; ++*this; return (_Tmp); }
				ipointer& operator --() { item_pointer().operator --(); return (*this); }
				ipointer operator--(int) { ipointer _Tmp = *this; --*this; return (_Tmp); }
				void advance(difference_type n) { item_pointer().advance(n); }
				void regress(difference_type n) { item_pointer().regress(n); }
				ipointer& operator +=(difference_type n) { item_pointer().operator +=(n); return (*this); }
				ipointer& operator -=(difference_type n) { item_pointer().operator -=(n); return (*this); }
				ipointer operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
				ipointer operator-(difference_type n) const { return ((*this) + (-n)); }
				difference_type operator-(const ipointer& _Right_cref) const { return item_pointer() - (_Right_cref.item_pointer()); }
				reference operator*() const { return item_pointer().operator*(); }
				reference item() const { return operator*(); }
				reference previous_item() const { return item_pointer().previous_item(); }
				pointer operator->() const { return item_pointer().operator->(); }
				reference operator[](difference_type _Off) const { return item_pointer()[_Off]; }
				ipointer& operator=(const ipointer& _Right_cref) { item_pointer().operator=(_Right_cref.item_pointer()); return (*this); }
				bool operator==(const ipointer& _Right_cref) const { return item_pointer().operator==(_Right_cref.item_pointer()); }
				bool operator!=(const ipointer& _Right_cref) const { return (!(_Right_cref == (*this))); }
				bool operator<(const ipointer& _Right) const { return (item_pointer() < _Right.item_pointer()); }
				bool operator<=(const ipointer& _Right) const { return (item_pointer() <= _Right.item_pointer()); }
				bool operator>(const ipointer& _Right) const { return (item_pointer() > _Right.item_pointer()); }
				bool operator>=(const ipointer& _Right) const { return (item_pointer() >= _Right.item_pointer()); }
				void set_to_item_pointer(const ipointer& _Right_cref) { item_pointer().set_to_item_pointer(_Right_cref.item_pointer()); }
				msev_size_t position() const { return item_pointer().position(); }
				auto target_container_ptr() const {
					return m_owner_ptr;
				}
				void async_not_shareable_and_not_passable_tag() const {}
			private:
				_Myt* m_owner_ptr = nullptr;
				std::shared_ptr<mm_iterator_handle_type> m_handle_shptr;
				friend class /*_Myt*/msebasic_string<_Ty, _Traits, _A, _TStateMutex>;
			};

			ipointer ibegin() {	// return ipointer for beginning of mutable sequence
				ipointer retval(*this);
				retval.set_to_beginning();
				return retval;
			}
			cipointer ibegin() const {	// return ipointer for beginning of nonmutable sequence
				cipointer retval(*this);
				retval.set_to_beginning();
				return retval;
			}
			ipointer iend() {	// return ipointer for end of mutable sequence
				ipointer retval(*this);
				retval.set_to_end_marker();
				return retval;
			}
			cipointer iend() const {	// return ipointer for end of nonmutable sequence
				cipointer retval(*this);
				retval.set_to_end_marker();
				return retval;
			}
			cipointer cibegin() const {	// return ipointer for beginning of nonmutable sequence
				cipointer retval(*this);
				retval.set_to_beginning();
				return retval;
			}
			cipointer ciend() const {	// return ipointer for end of nonmutable sequence
				cipointer retval(*this);
				retval.set_to_end_marker();
				return retval;
			}

			msebasic_string(const cipointer &start, const cipointer &end, const _A& _Al = _A())
				: base_class(_Al), m_mmitset(*this) {
				/*m_debug_size = size();*/
				assign(start, end);
			}
			void assign(const mm_const_iterator_type &start, const mm_const_iterator_type &end) {
				if (start.m_owner_cptr != end.m_owner_cptr) { MSE_THROW(msebasic_string_range_error("invalid arguments - void assign(const mm_const_iterator_type &start, const mm_const_iterator_type &end) - msebasic_string")); }
				if (start > end) { MSE_THROW(msebasic_string_range_error("invalid arguments - void assign(const mm_const_iterator_type &start, const mm_const_iterator_type &end) - msebasic_string")); }
				typename base_class::const_iterator _F = start;
				typename base_class::const_iterator _L = end;
				(*this).assign(_F, _L);
			}
			void assign_inclusive(const mm_const_iterator_type &first, const mm_const_iterator_type &last) {
				auto end = last;
				end++; // this should include some checks
				(*this).assign(first, end);
			}
			void assign(const cipointer &start, const cipointer &end) {
				assign(start.const_item_pointer(), end.const_item_pointer());
			}
			void assign_inclusive(const cipointer &first, const cipointer &last) {
				assign_inclusive(first.const_item_pointer(), last.const_item_pointer());
			}
			void insert_before(const mm_const_iterator_type &pos, size_type _M, const _Ty& _X) {
				if (pos.m_owner_cptr != this) { MSE_THROW(msebasic_string_range_error("invalid arguments - void insert_before() - msebasic_string")); }
				typename base_class::const_iterator _P = pos;
				(*this).insert(_P, _M, _X);
			}
			void insert_before(const mm_const_iterator_type &pos, _Ty&& _X) {
				if (pos.m_owner_cptr != this) { MSE_THROW(msebasic_string_range_error("invalid arguments - void insert_before() - msebasic_string")); }
				typename base_class::const_iterator _P = pos;
				(*this).insert(_P, 1, MSE_FWD(_X));
			}
			void insert_before(const mm_const_iterator_type &pos, const _Ty& _X = _Ty()) { (*this).insert(pos, 1, _X); }
			template<class _Iter
				//>mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value, typename base_class::iterator>
				, class = mse::impl::_mse_RequireInputIter<_Iter> >
				void insert_before(const mm_const_iterator_type &pos, const _Iter &start, const _Iter &end) {
				if (pos.m_owner_cptr != this) { MSE_THROW(msebasic_string_range_error("invalid arguments - void insert_before() - msebasic_string")); }
				//if (start.m_owner_cptr != end.m_owner_cptr) { MSE_THROW(msebasic_string_range_error("invalid arguments - void insert_before(const mm_const_iterator_type &pos, const mm_const_iterator_type &start, const mm_const_iterator_type &end) - msebasic_string")); }
				typename base_class::const_iterator _P = pos;
				(*this).insert(_P, start, end);
			}
			template<class _Iter
				//>mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value, typename base_class::iterator>
				, class = mse::impl::_mse_RequireInputIter<_Iter> >
				void insert_before_inclusive(const mm_const_iterator_type &pos, const _Iter &first, const _Iter &last) {
				if (pos.m_owner_cptr != this) { MSE_THROW(msebasic_string_range_error("invalid arguments - void insert_before() - msebasic_string")); }
				if (first.m_owner_cptr != last.m_owner_cptr) { MSE_THROW(msebasic_string_range_error("invalid arguments - void insert_before_inclusive(const mm_const_iterator_type &pos, const mm_const_iterator_type &first, const mm_const_iterator_type &last) - msebasic_string")); }
				if (!(last.points_to_item())) { MSE_THROW(msebasic_string_range_error("invalid argument - void insert_before_inclusive(const mm_const_iterator_type &pos, const mm_const_iterator_type &first, const mm_const_iterator_type &last) - msebasic_string")); }
				typename base_class::const_iterator _P = pos;
				auto _L = last;
				_L++;
				(*this).insert(_P, first, _L);
			}
			void insert_before(const mm_const_iterator_type &pos, _XSTD initializer_list<typename base_class::value_type> _Ilist) {	// insert initializer_list
				if (pos.m_owner_cptr != this) { MSE_THROW(msebasic_string_range_error("invalid arguments - void insert_before() - msebasic_string")); }
				typename base_class::const_iterator _P = pos;
				(*this).insert(_P, _Ilist);
			}
			template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
			void insert_before(const mm_const_iterator_type &pos, const _TStringSection& _X) {
				if (pos.m_owner_cptr != this) { MSE_THROW(msebasic_string_range_error("invalid arguments - void insert_before() - msebasic_string")); }
				typename base_class::const_iterator _P = pos;
				(*this).insert(_P, _X);
			}
			ipointer insert_before(const cipointer &pos, size_type _M, const _Ty& _X) {
				msev_size_t original_pos = pos.position();
				insert_before(pos.const_item_pointer(), _M, _X);
				ipointer retval(*this); retval.advance(msev_int(original_pos));
				return retval;
			}
			ipointer insert_before(const cipointer &pos, _Ty&& _X) {
				msev_size_t original_pos = pos.position();
				insert_before(pos.const_item_pointer(), MSE_FWD(_X));
				ipointer retval(*this); retval.advance(msev_int(original_pos));
				return retval;
			}
			ipointer insert_before(const cipointer &pos, const _Ty& _X = _Ty()) { return insert_before(pos, 1, _X); }
			template<class _Iter
				//>mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value, typename base_class::iterator>
				, class = mse::impl::_mse_RequireInputIter<_Iter> >
				ipointer insert_before(const cipointer &pos, const _Iter &start, const _Iter &end) {
				msev_size_t original_pos = pos.position();
				insert_before(pos.const_item_pointer(), start, end);
				ipointer retval(*this); retval.advance(msev_int(original_pos));
				return retval;
			}
			template<class _Iter
				//>mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value, typename base_class::iterator>
				, class = mse::impl::_mse_RequireInputIter<_Iter> >
				ipointer insert_before_inclusive(const cipointer &pos, const _Iter &first, const _Iter &last) {
				auto end = last; end++;
				return insert_before(pos, first, end);
			}
			ipointer insert_before(const cipointer &pos, _XSTD initializer_list<typename base_class::value_type> _Ilist) {	// insert initializer_list
				msev_size_t original_pos = pos.position();
				(*this).insert_before(pos.const_item_pointer(), _Ilist);
				ipointer retval(*this); retval.advance(msev_int(original_pos));
				return retval;
			}
			template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
			ipointer insert_before(const cipointer &pos, const _TStringSection& _X) {
				msev_size_t original_pos = pos.position();
				(*this).insert_before(pos.const_item_pointer(), _X);
				ipointer retval(*this); retval.advance(msev_int(original_pos));
				return retval;
			}
			void insert_before(msev_size_t pos, _Ty&& _X) {
				typename base_class::const_iterator _P = (*this).begin() + msev_as_a_size_t(pos);
				(*this).insert(_P, MSE_FWD(_X));
			}
			void insert_before(msev_size_t pos, const _Ty& _X = _Ty()) {
				typename base_class::const_iterator _P = (*this).begin() + msev_as_a_size_t(pos);
				(*this).insert(_P, _X);
			}
			void insert_before(msev_size_t pos, size_t _M, const _Ty& _X) {
				typename base_class::const_iterator _P = (*this).begin() + msev_as_a_size_t(pos);
				(*this).insert(_P, _M, _X);
			}
			void insert_before(msev_size_t pos, _XSTD initializer_list<typename base_class::value_type> _Ilist) {	// insert initializer_list
				typename base_class::const_iterator _P = (*this).begin() + msev_as_a_size_t(pos);
				(*this).insert(_P, _Ilist);
			}
			template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
			void insert_before(msev_size_t pos, const _TStringSection& _X) {
				typename base_class::const_iterator _P = (*this).begin() + msev_as_a_size_t(pos);
				(*this).insert(_P, _X);
			}
			/* These insert() functions are just aliases for their corresponding insert_before() functions. */
			ipointer insert(const cipointer &pos, size_type _M, const _Ty& _X) { return insert_before(pos, _M, _X); }
			ipointer insert(const cipointer &pos, _Ty&& _X) { return insert_before(pos, MSE_FWD(_X)); }
			ipointer insert(const cipointer &pos, const _Ty& _X = _Ty()) { return insert_before(pos, _X); }
			template<class _Iter
				//>mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value, typename base_class::iterator>
				, class = mse::impl::_mse_RequireInputIter<_Iter> >
				ipointer insert(const cipointer &pos, const _Iter &start, const _Iter &end) { return insert_before(pos, start, end); }
			ipointer insert(const cipointer &pos, _XSTD initializer_list<typename base_class::value_type> _Ilist) { return insert_before(pos, _Ilist); }
			template<typename _TStringSection, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_base_of<mse::us::impl::StringSectionTagBase, _TStringSection>::value)> MSE_IMPL_EIS >
			ipointer insert(const cipointer &pos, const _TStringSection& _X) { return insert_before(pos, _X); }
			template<class ..._Valty>
#if !(defined(GPP4P8_COMPATIBLE))
			void emplace(const mm_const_iterator_type &pos, _Valty&& ..._Val)
			{	// insert by moving _Val at pos
#else /*!(defined(GPP4P8_COMPATIBLE))*/
			void emplace(const mm_iterator_type &pos, _Valty&& ..._Val)
			{	// insert by moving _Val at pos
#endif /*!(defined(GPP4P8_COMPATIBLE))*/
				if (pos.m_owner_cptr != this) { MSE_THROW(msebasic_string_range_error("invalid arguments - void emplace() - msebasic_string")); }
				typename base_class::const_iterator _P = pos;
				auto retval = (*this).emplace(_P, std::forward<_Valty>(_Val)...);
			}
			template<class ..._Valty>
#if !(defined(GPP4P8_COMPATIBLE))
			ipointer emplace(const cipointer &pos, _Valty&& ..._Val)
			{	// insert by moving _Val at pos
#else /*!(defined(GPP4P8_COMPATIBLE))*/
			ipointer emplace(const ipointer &pos, _Valty&& ..._Val)
			{	// insert by moving _Val at pos
#endif /*!(defined(GPP4P8_COMPATIBLE))*/
				msev_size_t original_pos = pos.position();
				(*this).emplace(pos.const_item_pointer(), std::forward<_Valty>(_Val)...);
				ipointer retval(*this); retval.advance(msev_int(original_pos));
				return retval;
			}
			void erase(const mm_const_iterator_type &pos) {
				if (pos.m_owner_cptr != this) { MSE_THROW(msebasic_string_range_error("invalid arguments - void erase() - msebasic_string")); }
				typename base_class::const_iterator _P = pos;
				(*this).erase(_P);
			}
			void erase(const mm_const_iterator_type &start, const mm_const_iterator_type &end) {
				if (start.m_owner_cptr != this) { MSE_THROW(msebasic_string_range_error("invalid arguments - void erase() - msebasic_string")); }
				if (end.m_owner_cptr != this) { MSE_THROW(msebasic_string_range_error("invalid arguments - void erase() - msebasic_string")); }
				typename base_class::const_iterator _F = start;
				typename base_class::const_iterator _L = end;
				(*this).erase(_F, _L);
			}
			void erase_inclusive(const mm_const_iterator_type &first, const mm_const_iterator_type &last) {
				if (first.m_owner_cptr != this) { MSE_THROW(msebasic_string_range_error("invalid arguments - void erase_inclusive() - msebasic_string")); }
				if (last.m_owner_cptr != this) { MSE_THROW(msebasic_string_range_error("invalid arguments - void erase_inclusive() - msebasic_string")); }
				if (!(last.points_to_item())) { MSE_THROW(msebasic_string_range_error("invalid argument - void erase_inclusive() - msebasic_string")); }
				typename base_class::const_iterator _F = first;
				typename base_class::const_iterator _L = last;
				_L++;
				(*this).erase(_F, _L);
			}
			ipointer erase(const cipointer &pos) {
				auto retval_pos = pos;
				retval_pos.set_to_next();
				(*this).erase(pos.const_item_pointer());
				ipointer retval = (*this).ibegin();
				retval.advance(msev_int(retval_pos.position()));
				return retval;
			}
			ipointer erase(const cipointer &start, const cipointer &end) {
				auto retval_pos = end;
				retval_pos.set_to_next();
				(*this).erase(start.const_item_pointer(), end.const_item_pointer());
				ipointer retval = (*this).ibegin();
				retval.advance(msev_int(retval_pos.position()));
				return retval;
			}
			ipointer erase_inclusive(const cipointer &first, const cipointer &last) {
				auto end = last; end.set_to_next();
				return (*this).erase(first, end);
			}
			void erase_previous_item(const mm_const_iterator_type &pos) {
				if (pos.m_owner_cptr != this) { MSE_THROW(msebasic_string_range_error("invalid arguments - void erase_previous_item() - msebasic_string")); }
				if (!(pos.has_previous())) { MSE_THROW(msebasic_string_range_error("invalid arguments - void erase_previous_item() - msebasic_string")); }
				typename base_class::const_iterator _P = pos;
				_P--;
				(*this).erase(_P);
			}
			ipointer erase_previous_item(const cipointer &pos) {
				erase_previous_item(pos.const_item_pointer());
				ipointer retval = (*this).ibegin();
				retval.advance(pos.position());
				return retval;
			}


			typedef typename base_class::ss_iterator_type ss_iterator_type;
			typedef typename base_class::ss_const_iterator_type ss_const_iterator_type;
			typedef typename base_class::ss_reverse_iterator_type ss_reverse_iterator_type;
			typedef typename base_class::ss_const_reverse_iterator_type ss_const_reverse_iterator_type;

			ss_iterator_type ss_begin() {	// return std_basic_string::iterator for beginning of mutable sequence
				return base_class::ss_begin();
			}
			ss_const_iterator_type ss_begin() const {	// return std_basic_string::iterator for beginning of nonmutable sequence
				return base_class::ss_begin();
			}
			ss_iterator_type ss_end() {	// return std_basic_string::iterator for end of mutable sequence
				return base_class::ss_end();
			}
			ss_const_iterator_type ss_end() const {	// return std_basic_string::iterator for end of nonmutable sequence
				return base_class::ss_end();
			}
			ss_const_iterator_type ss_cbegin() const {	// return std_basic_string::iterator for beginning of nonmutable sequence
				return base_class::ss_cbegin();
			}
			ss_const_iterator_type ss_cend() const {	// return std_basic_string::iterator for end of nonmutable sequence
				return base_class::ss_cend();
			}

			ss_const_reverse_iterator_type ss_crbegin() const {	// return std_basic_string::iterator for beginning of reversed nonmutable sequence
				return base_class::ss_crbegin();
			}
			ss_const_reverse_iterator_type ss_crend() const {	// return std_basic_string::iterator for end of reversed nonmutable sequence
				return base_class::ss_crend();
			}
			ss_reverse_iterator_type ss_rbegin() {	// return std_basic_string::iterator for beginning of reversed mutable sequence
				return base_class::ss_rbegin();
			}
			ss_const_reverse_iterator_type ss_rbegin() const {	// return std_basic_string::iterator for beginning of reversed nonmutable sequence
				return base_class::ss_rbegin();
			}
			ss_reverse_iterator_type ss_rend() {	// return std_basic_string::iterator for end of reversed mutable sequence
				return base_class::ss_rend();
			}
			ss_const_reverse_iterator_type ss_rend() const {	// return std_basic_string::iterator for end of reversed nonmutable sequence
				return base_class::ss_rend();
			}

			typename base_class::const_iterator const_iterator_from_ss_const_iterator_type(const ss_const_iterator_type& ss_citer) const {
				assert(ss_citer.target_container_ptr() == this);
				typename base_class::const_iterator retval = (*this).cbegin();
				retval += ss_citer.position();
				return retval;
			}

			msebasic_string(const ss_const_iterator_type &start, const ss_const_iterator_type &end, const _A& _Al = _A())
				: base_class(_Al), m_mmitset(*this) {
				/*m_debug_size = size();*/
				assign(start, end);
			}
			void assign(const ss_const_iterator_type &start, const ss_const_iterator_type &end) {
				if (start.target_container_ptr() != end.target_container_ptr()) { MSE_THROW(msebasic_string_range_error("invalid arguments - void assign(const ss_const_iterator_type &start, const ss_const_iterator_type &end) - msebasic_string")); }
				if (start > end) { MSE_THROW(msebasic_string_range_error("invalid arguments - void assign(const ss_const_iterator_type &start, const ss_const_iterator_type &end) - msebasic_string")); }
				typename base_class::const_iterator _F = const_iterator_from_ss_const_iterator_type(start);
				typename base_class::const_iterator _L = const_iterator_from_ss_const_iterator_type(end);
				(*this).assign(_F, _L);
			}
			void assign_inclusive(const ss_const_iterator_type &first, const ss_const_iterator_type &last) {
				auto end = last;
				end++; // this should include some checks
				(*this).assign(first, end);
			}
			ss_iterator_type insert_before(const ss_const_iterator_type &pos, size_type _M, const _Ty& _X) {
				if (pos.target_container_ptr() != this) { MSE_THROW(msebasic_string_range_error("invalid argument - void insert_before() - msebasic_string")); }
				pos.assert_valid_index();
				msev_size_t original_pos = pos.position();
				typename base_class::const_iterator _P = const_iterator_from_ss_const_iterator_type(pos);
				(*this).insert(_P, _M, _X);
				ss_iterator_type retval = ss_begin();
				retval.advance(msev_int(original_pos));
				return retval;
			}
			ss_iterator_type insert_before(const ss_const_iterator_type &pos, _Ty&& _X) {
				if (pos.target_container_ptr() != this) { MSE_THROW(msebasic_string_range_error("invalid argument - void insert_before() - msebasic_string")); }
				pos.assert_valid_index();
				msev_size_t original_pos = pos.position();
				typename base_class::const_iterator _P = const_iterator_from_ss_const_iterator_type(pos);
				(*this).insert(_P, MSE_FWD(_X));
				ss_iterator_type retval = ss_begin();
				retval.advance(msev_int(original_pos));
				return retval;
			}
			ss_iterator_type insert_before(const ss_const_iterator_type &pos, const _Ty& _X = _Ty()) { return (*this).insert(pos, 1, _X); }
			template<class _Iter
				//>mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value, typename base_class::iterator>
				, class = mse::impl::_mse_RequireInputIter<_Iter> >
				ss_iterator_type insert_before(const ss_const_iterator_type &pos, const _Iter &start, const _Iter &end) {
				if (pos.target_container_ptr() != this) { MSE_THROW(msebasic_string_range_error("invalid argument - ss_iterator_type insert_before() - msebasic_string")); }
				//if (start.m_owner_cptr != end.m_owner_cptr) { MSE_THROW(msebasic_string_range_error("invalid arguments - void insert_before(const ss_const_iterator_type &pos, const ss_const_iterator_type &start, const ss_const_iterator_type &end) - msebasic_string")); }
				pos.assert_valid_index();
				msev_size_t original_pos = pos.position();
				typename base_class::const_iterator _P = const_iterator_from_ss_const_iterator_type(pos);
				(*this).insert(_P, start, end);
				ss_iterator_type retval = ss_begin();
				retval.advance(msev_int(original_pos));
				return retval;
			}
			ss_iterator_type insert_before(const ss_const_iterator_type &pos, const ss_const_iterator_type& start, const ss_const_iterator_type &end) {
				if (start.target_container_ptr() != end.target_container_ptr()) { MSE_THROW(msebasic_string_range_error("invalid arguments - void insert_before(const ss_const_iterator_type &pos, const ss_const_iterator_type &start, const ss_const_iterator_type &end) - msebasic_string")); }
				end.assert_valid_index();
				if (start > end) { MSE_THROW(msebasic_string_range_error("invalid arguments - void insert_before(const ss_const_iterator_type &pos, const ss_const_iterator_type &start, const ss_const_iterator_type &end) - msebasic_string")); }
				typename base_class::const_iterator _S = start;
				typename base_class::const_iterator _E = end;
				return (*this).insert_before(pos, _S, _E);
			}
			ss_iterator_type insert_before(const ss_const_iterator_type &pos, const _Ty* start, const _Ty* end) {
				if (pos.target_container_ptr() != this) { MSE_THROW(msebasic_string_range_error("invalid arguments - ss_iterator_type insert_before() - msebasic_string")); }
				//if (start.target_container_ptr() != end.target_container_ptr()) { MSE_THROW(msebasic_string_range_error("invalid arguments - void insert_before(const ss_const_iterator_type &pos, const ss_const_iterator_type &start, const ss_const_iterator_type &end) - msebasic_string")); }
				if (start > end) { MSE_THROW(msebasic_string_range_error("invalid arguments - ss_iterator_type insert_before() - msebasic_string")); }
				pos.assert_valid_index();
				msev_size_t original_pos = pos.position();
				typename base_class::const_iterator _P = const_iterator_from_ss_const_iterator_type(pos);
				(*this).insert(_P, start, end);
				ss_iterator_type retval = ss_begin();
				retval.advance(msev_int(original_pos));
				return retval;
			}
			template<class _Iter
				//>mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value, typename base_class::iterator>
				, class = mse::impl::_mse_RequireInputIter<_Iter> >
				ss_iterator_type insert_before_inclusive(const ss_iterator_type &pos, const _Iter &first, const _Iter &last) {
				auto end = last;
				end++; // this may include some checks
				return (*this).insert_before(pos, first, end);
			}
			ss_iterator_type insert_before(const ss_const_iterator_type &pos, _XSTD initializer_list<typename base_class::value_type> _Ilist) {	// insert initializer_list
				if (pos.target_container_ptr() != this) { MSE_THROW(msebasic_string_range_error("invalid arguments - void insert_before() - msebasic_string")); }
				pos.assert_valid_index();
				msev_size_t original_pos = pos.position();
				typename base_class::const_iterator _P = const_iterator_from_ss_const_iterator_type(pos);
				(*this).insert(_P, _Ilist);
				ss_iterator_type retval = ss_begin();
				retval.advance(msev_int(original_pos));
				return retval;
			}
			/* These insert() functions are just aliases for their corresponding insert_before() functions. */
			ss_iterator_type insert(const ss_const_iterator_type &pos, size_type _M, const _Ty& _X) { return insert_before(pos, _M, _X); }
			ss_iterator_type insert(const ss_const_iterator_type &pos, _Ty&& _X) { return insert_before(pos, MSE_FWD(_X)); }
			ss_iterator_type insert(const ss_const_iterator_type &pos, const _Ty& _X = _Ty()) { return insert_before(pos, _X); }
			ss_iterator_type insert(const ss_const_iterator_type &pos, const _Ty* start, const _Ty* &end) { return insert_before(pos, start, end); }
			template<class _Iter
				//>mse::impl::enable_if_t<mse::impl::_mse_Is_iterator<_Iter>::value, typename base_class::iterator>
				, class = mse::impl::_mse_RequireInputIter<_Iter> >
				ss_iterator_type insert(const ss_const_iterator_type &pos, const _Iter &start, const _Iter &end) { return insert_before(pos, start, end); }
			ss_iterator_type insert(const ss_const_iterator_type &pos, _XSTD initializer_list<typename base_class::value_type> _Ilist) { return insert_before(pos, _Ilist); }
			template<class ..._Valty>
#if !(defined(GPP4P8_COMPATIBLE))
			ss_iterator_type emplace(const ss_const_iterator_type &pos, _Valty&& ..._Val)
			{	// insert by moving _Val at pos
				if (pos.target_container_ptr() != this) { MSE_THROW(msebasic_string_range_error("invalid arguments - void emplace() - msebasic_string")); }
#else /*!(defined(GPP4P8_COMPATIBLE))*/
			ipointer emplace(const ipointer &pos, _Valty&& ..._Val)
			{	// insert by moving _Val at pos
				if (pos.m_owner_ptr != this) { MSE_THROW(msebasic_string_range_error("invalid arguments - void emplace() - msebasic_string")); }
#endif /*!(defined(GPP4P8_COMPATIBLE))*/
				pos.assert_valid_index();
				msev_size_t original_pos = pos.position();
				typename base_class::const_iterator _P = const_iterator_from_ss_const_iterator_type(pos);
				(*this).emplace(_P, std::forward<_Valty>(_Val)...);
				ss_iterator_type retval = ss_begin();
				retval.advance(msev_int(original_pos));
				return retval;
			}
			ss_iterator_type erase(const ss_const_iterator_type &pos) {
				if (pos.target_container_ptr() != this) { MSE_THROW(msebasic_string_range_error("invalid arguments - void erase() - msebasic_string")); }
				if (!pos.points_to_an_item()) { MSE_THROW(msebasic_string_range_error("invalid arguments - void erase() - msebasic_string")); }
				auto pos_index = pos.position();

				typename base_class::const_iterator _P = const_iterator_from_ss_const_iterator_type(pos);
				(*this).erase(_P);

				ss_iterator_type retval = (*this).ss_begin();
				retval.advance(typename ss_const_iterator_type::difference_type(pos_index));
				return retval;
			}
			ss_iterator_type erase(const ss_const_iterator_type &start, const ss_const_iterator_type &end) {
				if (start.target_container_ptr() != this) { MSE_THROW(msebasic_string_range_error("invalid arguments - void erase() - msebasic_string")); }
				if (end.target_container_ptr() != this) { MSE_THROW(msebasic_string_range_error("invalid arguments - void erase() - msebasic_string")); }
				if (start.position() > end.position()) { MSE_THROW(msebasic_string_range_error("invalid arguments - void erase() - msebasic_string")); }
				auto pos_index = start.position();

				typename base_class::const_iterator _F = const_iterator_from_ss_const_iterator_type(start);
				typename base_class::const_iterator _L = const_iterator_from_ss_const_iterator_type(end);
				(*this).erase(_F, _L);

				ss_iterator_type retval = (*this).ss_begin();
				retval.advance(typename ss_const_iterator_type::difference_type(pos_index));
				return retval;
			}
			ss_iterator_type erase_inclusive(const ss_const_iterator_type &first, const ss_const_iterator_type &last) {
				auto end = last; end.set_to_next();
				return erase(first, end);
			}
			void erase_previous_item(const ss_const_iterator_type &pos) {
				if (pos.target_container_ptr() != this) { MSE_THROW(msebasic_string_range_error("invalid arguments - void erase_previous_item() - msebasic_string")); }
				if (!(pos.has_previous())) { MSE_THROW(msebasic_string_range_error("invalid arguments - void erase_previous_item() - msebasic_string")); }
				typename base_class::const_iterator _P = const_iterator_from_ss_const_iterator_type(pos);
				_P--;
				(*this).erase(_P);
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

			typedef typename base_class::xscope_ss_const_iterator_type xscope_ss_const_iterator_type;
			typedef typename base_class::xscope_ss_iterator_type xscope_ss_iterator_type;

			typedef typename base_class::xscope_const_iterator xscope_const_iterator;
			typedef typename base_class::xscope_iterator xscope_iterator;


			class xscope_ipointer;

			class xscope_cipointer : public cipointer, public mse::us::impl::XScopeTagBase {
			public:
				xscope_cipointer(const mse::TXScopeObjFixedConstPointer<msebasic_string>& owner_ptr) : cipointer((*owner_ptr).cibegin()) {}
				xscope_cipointer(const mse::TXScopeObjFixedPointer<msebasic_string>& owner_ptr) : cipointer((*owner_ptr).cibegin()) {}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
				xscope_cipointer(const mse::TXScopeFixedConstPointer<msebasic_string>& owner_ptr) : cipointer((*owner_ptr).cibegin()) {}
				xscope_cipointer(const mse::TXScopeFixedPointer<msebasic_string>& owner_ptr) : cipointer((*owner_ptr).cibegin()) {}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

				xscope_cipointer(const xscope_cipointer& src_cref) : cipointer(src_cref) {}
				xscope_cipointer(const xscope_ipointer& src_cref) : cipointer(src_cref) {}
				~xscope_cipointer() {}
				const cipointer& msebasic_string_cipointer() const {
					return (*this);
				}
				cipointer& msebasic_string_cipointer() {
					return (*this);
				}
				const cipointer& mvssci() const { return msebasic_string_cipointer(); }
				cipointer& mvssci() { return msebasic_string_cipointer(); }

				void reset() { cipointer::reset(); }
				bool points_to_an_item() const { return cipointer::points_to_an_item(); }
				bool points_to_end_marker() const { return cipointer::points_to_end_marker(); }
				bool points_to_beginning() const { return cipointer::points_to_beginning(); }
				/* has_next_item_or_end_marker() is just an alias for points_to_an_item(). */
				bool has_next_item_or_end_marker() const { return cipointer::has_next_item_or_end_marker(); }
				/* has_next() is just an alias for points_to_an_item() that's familiar to java programmers. */
				bool has_next() const { return cipointer::has_next(); }
				bool has_previous() const { return cipointer::has_previous(); }
				void set_to_beginning() { cipointer::set_to_beginning(); }
				void set_to_end_marker() { cipointer::set_to_end_marker(); }
				void set_to_next() { cipointer::set_to_next(); }
				void set_to_previous() { cipointer::set_to_previous(); }
				xscope_cipointer& operator ++() { cipointer::operator ++(); return (*this); }
				xscope_cipointer operator++(int) { xscope_cipointer _Tmp = *this; cipointer::operator++(); return (_Tmp); }
				xscope_cipointer& operator --() { cipointer::operator --(); return (*this); }
				xscope_cipointer operator--(int) { xscope_cipointer _Tmp = *this; cipointer::operator--(); return (_Tmp); }
				void advance(difference_type n) { cipointer::advance(n); }
				void regress(difference_type n) { cipointer::regress(n); }
				xscope_cipointer& operator +=(difference_type n) { cipointer::operator +=(n); return (*this); }
				xscope_cipointer& operator -=(difference_type n) { cipointer::operator -=(n); return (*this); }
				xscope_cipointer operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
				xscope_cipointer operator-(difference_type n) const { return ((*this) + (-n)); }
				difference_type operator-(const xscope_cipointer& _Right_cref) const { return cipointer::operator-(_Right_cref); }
				const_reference operator*() const { return cipointer::operator*(); }
				const_reference item() const { return operator*(); }
				const_reference previous_item() const { return cipointer::previous_item(); }
				const_pointer operator->() const { return cipointer::operator->(); }
				const_reference operator[](difference_type _Off) const { return cipointer::operator[](_Off); }
				xscope_cipointer& operator=(const cipointer& _Right_cref) {
					if ((&(*_Right_cref.target_container_ptr())) != (&(*(*this).target_container_ptr()))) { MSE_THROW(msebasic_string_range_error("invalid argument - xscope_cipointer& operator=(const xscope_cipointer& _Right_cref) - msebasic_string::xscope_cipointer")); }
					cipointer::operator=(_Right_cref);
					return (*this);
				}
				xscope_cipointer& operator=(const ipointer& _Right_cref) {
					if ((&(*_Right_cref.target_container_ptr())) != (&(*(*this).target_container_ptr()))) { MSE_THROW(msebasic_string_range_error("invalid argument - xscope_cipointer& operator=(const ipointer& _Right_cref) - msebasic_string::xscope_cipointer")); }
					return operator=(cipointer(_Right_cref));
				}
				bool operator==(const xscope_cipointer& _Right_cref) const { return cipointer::operator==(_Right_cref); }
				bool operator!=(const xscope_cipointer& _Right_cref) const { return (!(_Right_cref == (*this))); }
				bool operator<(const xscope_cipointer& _Right) const { return cipointer::operator<(_Right); }
				bool operator<=(const xscope_cipointer& _Right) const { return cipointer::operator<=(_Right); }
				bool operator>(const xscope_cipointer& _Right) const { return cipointer::operator>(_Right); }
				bool operator>=(const xscope_cipointer& _Right) const { return cipointer::operator>=(_Right); }
				void set_to_const_item_pointer(const xscope_cipointer& _Right_cref) { cipointer::set_to_item_pointer(_Right_cref); }
				msev_size_t position() const { return cipointer::position(); }
				auto target_container_ptr() const {
					return mse::us::unsafe_make_xscope_const_pointer_to(*(cipointer::target_container_ptr()));
				}
				void xscope_ipointer_tag() const {}
				void async_not_shareable_and_not_passable_tag() const {}
			private:
				void* operator new(size_t size) { return ::operator new(size); }

				//typename cipointer (*this);
				friend class /*_Myt*/msebasic_string<_Ty, _Traits, _A, _TStateMutex>;
				friend class xscope_ipointer;
			};
			class xscope_ipointer : public ipointer, public mse::us::impl::XScopeTagBase {
			public:
				xscope_ipointer(const mse::TXScopeObjFixedPointer<msebasic_string>& owner_ptr) : ipointer((*owner_ptr).ibegin()) {}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
				xscope_ipointer(const mse::TXScopeFixedPointer<msebasic_string>& owner_ptr) : ipointer((*owner_ptr).ibegin()) {}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

				xscope_ipointer(const xscope_ipointer& src_cref) : ipointer(src_cref) {}
				~xscope_ipointer() {}
				const ipointer& msebasic_string_ipointer() const {
					return (*this);
				}
				ipointer& msebasic_string_ipointer() {
					return (*this);
				}
				const ipointer& mvssi() const { return msebasic_string_ipointer(); }
				ipointer& mvssi() { return msebasic_string_ipointer(); }

				void reset() { ipointer::reset(); }
				bool points_to_an_item() const { return ipointer::points_to_an_item(); }
				bool points_to_end_marker() const { return ipointer::points_to_end_marker(); }
				bool points_to_beginning() const { return ipointer::points_to_beginning(); }
				/* has_next_item_or_end_marker() is just an alias for points_to_an_item(). */
				bool has_next_item_or_end_marker() const { return ipointer::has_next_item_or_end_marker(); }
				/* has_next() is just an alias for points_to_an_item() that's familiar to java programmers. */
				bool has_next() const { return ipointer::has_next(); }
				bool has_previous() const { return ipointer::has_previous(); }
				void set_to_beginning() { ipointer::set_to_beginning(); }
				void set_to_end_marker() { ipointer::set_to_end_marker(); }
				void set_to_next() { ipointer::set_to_next(); }
				void set_to_previous() { ipointer::set_to_previous(); }
				xscope_ipointer& operator ++() { ipointer::operator ++(); return (*this); }
				xscope_ipointer operator++(int) { xscope_ipointer _Tmp = *this; ipointer::operator++(); return (_Tmp); }
				xscope_ipointer& operator --() { ipointer::operator --(); return (*this); }
				xscope_ipointer operator--(int) { xscope_ipointer _Tmp = *this; ipointer::operator--(); return (_Tmp); }
				void advance(difference_type n) { ipointer::advance(n); }
				void regress(difference_type n) { ipointer::regress(n); }
				xscope_ipointer& operator +=(difference_type n) { ipointer::operator +=(n); return (*this); }
				xscope_ipointer& operator -=(difference_type n) { ipointer::operator -=(n); return (*this); }
				xscope_ipointer operator+(difference_type n) const { auto retval = (*this); retval += n; return retval; }
				xscope_ipointer operator-(difference_type n) const { return ((*this) + (-n)); }
				difference_type operator-(const xscope_ipointer& _Right_cref) const { return ipointer::operator-(_Right_cref); }
				reference operator*() const { return ipointer::operator*(); }
				reference item() const { return operator*(); }
				reference previous_item() const { return ipointer::previous_item(); }
				pointer operator->() const { return ipointer::operator->(); }
				reference operator[](difference_type _Off) const { return ipointer::operator[](_Off); }
				xscope_ipointer& operator=(const ipointer& _Right_cref) {
					if ((&(*_Right_cref.target_container_ptr())) != (&(*(*this).target_container_ptr()))) { MSE_THROW(msebasic_string_range_error("invalid argument - xscope_ipointer& operator=(const xscope_ipointer& _Right_cref) - msebasic_string::xscope_ipointer")); }
					ipointer::operator=(_Right_cref);
					return (*this);
				}
				bool operator==(const xscope_ipointer& _Right_cref) const { return ipointer::operator==(_Right_cref); }
				bool operator!=(const xscope_ipointer& _Right_cref) const { return (!(_Right_cref == (*this))); }
				bool operator<(const xscope_ipointer& _Right) const { return ipointer::operator<(_Right); }
				bool operator<=(const xscope_ipointer& _Right) const { return ipointer::operator<=(_Right); }
				bool operator>(const xscope_ipointer& _Right) const { return ipointer::operator>(_Right); }
				bool operator>=(const xscope_ipointer& _Right) const { return ipointer::operator>=(_Right); }
				void set_to_item_pointer(const xscope_ipointer& _Right_cref) { ipointer::set_to_item_pointer(_Right_cref); }
				msev_size_t position() const { return ipointer::position(); }
				auto target_container_ptr() const {
					return mse::us::unsafe_make_xscope_pointer_to(*(ipointer::target_container_ptr()));
				}
				void xscope_ipointer_tag() const {}
				void async_not_shareable_and_not_passable_tag() const {}
			private:
				void* operator new(size_t size) { return ::operator new(size); }

				//typename ipointer (*this);
				friend class /*_Myt*/msebasic_string<_Ty, _Traits, _A, _TStateMutex>;
			};

			void async_not_shareable_and_not_passable_tag() const {}

		private:

			template<class _Mutex>
			class structure_change_guard {
			public:
				structure_change_guard(_Mutex& _Mtx) MSE_FUNCTION_TRY : m_lock_guard(_Mtx) {}
				MSE_FUNCTION_CATCH_ANY {
					MSE_THROW(mse::structure_lock_violation_error("structure lock violation - Attempting to modify \
							the structure (size/capacity) of a container while a reference (iterator) to one of its elements \
							still exists?"));
				}
			private:
				std::lock_guard<_Mutex> m_lock_guard;
			};

			/* These are a couple of functions that are basically just here for the convenience of the mstd::basic_string<>
			implementation, which uses this class. */
#ifdef MSE_MSTD_STRING_CHECK_USE_AFTER_FREE
			void note_parent_destruction() { m_parent_destroyed = true; }
			void assert_parent_not_destroyed() const {
				/* This assert can fail if, for example, you dereference an mstd::basic_string<> iterator after the basic_string has
				been destroyed. You can supress this assert by defining MSE_SUPPRESS_MSTD_STRING_CHECK_USE_AFTER_FREE. */
				assert(!m_parent_destroyed);
			}
			bool m_parent_destroyed = false;
#else // MSE_MSTD_STRING_CHECK_USE_AFTER_FREE
			void note_parent_destruction() {}
			void assert_parent_not_destroyed() const {}
#endif // MSE_MSTD_STRING_CHECK_USE_AFTER_FREE

			mutable mse::non_thread_safe_shared_mutex m_structure_change_mutex;

			auto contained_basic_string() const -> decltype(base_class::contained_basic_string()) { return base_class::contained_basic_string(); }
			auto contained_basic_string() -> decltype(base_class::contained_basic_string()) { return base_class::contained_basic_string(); }

			friend class mse::us::ns_msebasic_string::xscope_structure_lock_guard<_Myt>;
			friend class mse::us::ns_msebasic_string::xscope_const_structure_lock_guard<_Myt>;
			friend class mse::us::impl::Txscope_structure_lock_guard<_Myt>;
			friend class mse::us::impl::Txscope_const_structure_lock_guard<_Myt>;
#ifndef MSE_MSTDSTRING_DISABLED
			template<class _Ty2, class _Traits2/* = std::char_traits<_Ty>*/, class _A2/* = std::allocator<_Ty> */>
			friend class mse::mstd::basic_string;
#endif /*!MSE_MSTDSTRING_DISABLED*/
		};

#ifdef MSE_HAS_CXX17
		/* deduction guides */
		template<class _Iter, class _Alloc = std::allocator<typename std::iterator_traits<_Iter>::value_type>
			, std::enable_if_t<std::conjunction_v< mse::impl::_mse_Is_iterator<_Iter>, mse::impl::_mse_Is_allocator<_Alloc> >, int> = 0>
			msebasic_string(_Iter, _Iter, _Alloc = _Alloc())
			->msebasic_string<typename std::iterator_traits<_Iter>::value_type, std::char_traits<typename std::iterator_traits<_Iter>::value_type>, _Alloc>;

		template<class _Elem, class _Traits, class _Alloc = std::allocator<_Elem>
			, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
			explicit msebasic_string(std::basic_string_view<_Elem, _Traits>, const _Alloc& = _Alloc())
			->msebasic_string<_Elem, _Traits, _Alloc>;

		template<class _Elem, class _Traits, class _Alloc = std::allocator<_Elem>
			, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
			msebasic_string(std::basic_string_view<_Elem, _Traits>, mse::impl::_mse_Guide_size_type_t<_Alloc>
				, mse::impl::_mse_Guide_size_type_t<_Alloc>, const _Alloc& = _Alloc())
			->msebasic_string<_Elem, _Traits, _Alloc>;

		template<class _TRAIterator, class _Traits, class _Alloc = std::allocator<typename std::iterator_traits<_TRAIterator>::value_type>
			, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
			explicit msebasic_string(mse::TStringConstSection<_TRAIterator, _Traits>, const _Alloc& = _Alloc())
			->msebasic_string<typename std::iterator_traits<_TRAIterator>::value_type, _Traits, _Alloc>;
		template<class _TRAIterator, class _Traits, class _Alloc = std::allocator<typename std::iterator_traits<_TRAIterator>::value_type>
			, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
			explicit msebasic_string(mse::TStringSection<_TRAIterator, _Traits>, const _Alloc& = _Alloc())
			->msebasic_string<typename std::iterator_traits<_TRAIterator>::value_type, _Traits, _Alloc>;
		template<class _TRAIterator, class _Traits, class _Alloc = std::allocator<typename std::iterator_traits<_TRAIterator>::value_type>
			, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
			explicit msebasic_string(mse::TXScopeStringConstSection<_TRAIterator, _Traits>, const _Alloc& = _Alloc())
			->msebasic_string<typename std::iterator_traits<_TRAIterator>::value_type, _Traits, _Alloc>;
		template<class _TRAIterator, class _Traits, class _Alloc = std::allocator<typename std::iterator_traits<_TRAIterator>::value_type>
			, std::enable_if_t<mse::impl::_mse_Is_allocator<_Alloc>::value, int> = 0>
			explicit msebasic_string(mse::TXScopeStringSection<_TRAIterator, _Traits>, const _Alloc& = _Alloc())
			->msebasic_string<typename std::iterator_traits<_TRAIterator>::value_type, _Traits, _Alloc>;
#endif /* MSE_HAS_CXX17 */

		template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty>, class _TStateMutex = mse::non_thread_safe_shared_mutex> inline bool operator!=(const msebasic_string<_Ty, _Traits, _A, _TStateMutex>& _Left,
			const msebasic_string<_Ty, _Traits, _A, _TStateMutex>& _Right) {	// test for basic_string inequality
			return (!(_Left == _Right));
		}

		template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty>, class _TStateMutex = mse::non_thread_safe_shared_mutex> inline bool operator>(const msebasic_string<_Ty, _Traits, _A, _TStateMutex>& _Left,
			const msebasic_string<_Ty, _Traits, _A, _TStateMutex>& _Right) {	// test if _Left > _Right for basic_strings
			return (_Right < _Left);
		}

		template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty>, class _TStateMutex = mse::non_thread_safe_shared_mutex> inline bool operator<=(const msebasic_string<_Ty, _Traits, _A, _TStateMutex>& _Left,
			const msebasic_string<_Ty, _Traits, _A, _TStateMutex>& _Right) {	// test if _Left <= _Right for basic_strings
			return (!(_Right < _Left));
		}

		template<class _Ty, class _Traits = std::char_traits<_Ty>, class _A = std::allocator<_Ty>, class _TStateMutex = mse::non_thread_safe_shared_mutex> inline bool operator>=(const msebasic_string<_Ty, _Traits, _A, _TStateMutex>& _Left,
			const msebasic_string<_Ty, _Traits, _A, _TStateMutex>& _Right) {	// test if _Left >= _Right for basic_strings
			return (!(_Left < _Right));
		}

		namespace ns_msebasic_string {

			/* While an instance of xscope_structure_lock_guard exists it ensures that direct (scope) pointers to
			individual elements in the basic_string do not become invalid by preventing any operation that might resize the basic_string
			or increase its capacity. Any attempt to execute such an operation would result in an exception. */
			template<class _TContainer>
			class xscope_structure_lock_guard : public mse::us::impl::Txscope_structure_lock_guard<_TContainer> {
			public:
				typedef mse::us::impl::Txscope_structure_lock_guard<_TContainer> base_class;
				typedef _TContainer MBS;

				xscope_structure_lock_guard(const xscope_structure_lock_guard&) = default;
				xscope_structure_lock_guard(xscope_structure_lock_guard&&) = default;

				xscope_structure_lock_guard(const mse::TXScopeObjFixedPointer<MBS>& owner_ptr) : base_class(owner_ptr)
					, m_base_xscope_structure_lock_guard(owner_ptr) {}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
				xscope_structure_lock_guard(const mse::TXScopeFixedPointer<MBS>& owner_ptr) : base_class(owner_ptr)
					, m_base_xscope_structure_lock_guard(owner_ptr) {}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

				auto xscope_ptr_to_element(typename MBS::size_type _P) const {
					return base_class::xscope_ptr_to_element(_P);
				}
				auto xscope_ptr_to_element(const typename MBS::xscope_const_iterator& ss_iter) const {
					return base_class::xscope_ptr_to_element(ss_iter);
				}
				auto xscope_ptr_to_element(const typename MBS::xscope_cipointer& ciptr) const {
					assert(std::addressof(*(ciptr.target_container_ptr())) == std::addressof(*(*this)));
					return xscope_ptr_to_element(ciptr.position());
				}

				operator mse::TXScopeFixedPointer<MBS>() const {
					return static_cast<const base_class&>(*this);
				}
				/*
				template<class MBS2 = MBS, MSE_IMPL_EIP mse::impl::enable_if_t<!std::is_same<mse::TXScopeFixedConstPointer<MBS2>, mse::TXScopeFixedPointer<MBS> >::value> MSE_IMPL_EIS >
				explicit operator mse::TXScopeFixedConstPointer<MBS2>() const {
					return static_cast<const base_class&>(*this);
				}
				*/

			private:
				mse::impl::ns_gnii_basic_string::xscope_structure_lock_guard<typename _TContainer::base_class> m_base_xscope_structure_lock_guard;

				friend class xscope_const_structure_lock_guard<_TContainer>;
			};
			template<class _TContainer>
			class xscope_const_structure_lock_guard : public mse::us::impl::Txscope_const_structure_lock_guard<_TContainer> {
			public:
				typedef mse::us::impl::Txscope_const_structure_lock_guard<_TContainer> base_class;
				typedef _TContainer MBS;

				xscope_const_structure_lock_guard(const xscope_const_structure_lock_guard&) = default;
				xscope_const_structure_lock_guard(xscope_const_structure_lock_guard&&) = default;

				xscope_const_structure_lock_guard(const xscope_structure_lock_guard<_TContainer>& src) : base_class(src), m_base_xscope_structure_lock_guard(src.m_base_xscope_structure_lock_guard) {}
				xscope_const_structure_lock_guard(xscope_structure_lock_guard<_TContainer>&& src) : base_class(MSE_FWD(src)), m_base_xscope_structure_lock_guard(MSE_FWD(src).m_base_xscope_structure_lock_guard) {}

				xscope_const_structure_lock_guard(const mse::TXScopeObjFixedConstPointer<MBS>& owner_ptr) : base_class(owner_ptr)
					, m_base_xscope_structure_lock_guard(owner_ptr) {}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
				xscope_const_structure_lock_guard(const mse::TXScopeFixedConstPointer<MBS>& owner_ptr) : base_class(owner_ptr)
					, m_base_xscope_structure_lock_guard(owner_ptr) {}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

				auto xscope_ptr_to_element(typename MBS::size_type _P) const {
					return base_class::xscope_ptr_to_element(_P);
				}
				auto xscope_ptr_to_element(const typename MBS::xscope_const_iterator& ss_iter) const {
					return base_class::xscope_ptr_to_element(ss_iter);
				}
				auto xscope_ptr_to_element(const typename MBS::xscope_cipointer& ciptr) const {
					assert(std::addressof(*(ciptr.target_container_ptr())) == std::addressof(*(*this)));
					return xscope_ptr_to_element(ciptr.position());
				}

				operator mse::TXScopeFixedConstPointer<MBS>() const {
					return static_cast<const base_class&>(*this);
				}

			private:
				mse::us::impl::ns_gnii_basic_string::xscope_const_structure_lock_guard<typename _TContainer::base_class> m_base_xscope_structure_lock_guard;
			};
		}

		/* While an instance of xscope_structure_lock_guard exists it ensures that direct (scope) pointers to
		individual elements in the basic_string do not become invalid by preventing any operation that might resize the basic_string
		or increase its capacity. Any attempt to execute such an operation would result in an exception. */
		template<class _Ty, class _Traits, class _A = std::allocator<_Ty>, class _TStateMutex = mse::non_thread_safe_shared_mutex>
		auto make_xscope_structure_lock_guard(const mse::TXScopeObjFixedPointer<msebasic_string<_Ty, _Traits, _A, _TStateMutex> >& owner_ptr) {
			return ns_msebasic_string::xscope_structure_lock_guard<msebasic_string<_Ty, _Traits, _A, _TStateMutex> >(owner_ptr);
		}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _Ty, class _Traits, class _A = std::allocator<_Ty>, class _TStateMutex = mse::non_thread_safe_shared_mutex>
		auto make_xscope_structure_lock_guard(const mse::TXScopeFixedPointer<msebasic_string<_Ty, _Traits, _A, _TStateMutex> >& owner_ptr) {
			return ns_msebasic_string::xscope_structure_lock_guard<msebasic_string<_Ty, _Traits, _A, _TStateMutex> >(owner_ptr);
		}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _Ty, class _Traits, class _A = std::allocator<_Ty>, class _TStateMutex = mse::non_thread_safe_shared_mutex>
		auto make_xscope_structure_lock_guard(const mse::TXScopeObjFixedConstPointer<msebasic_string<_Ty, _Traits, _A, _TStateMutex> >& owner_ptr) {
			return ns_msebasic_string::xscope_const_structure_lock_guard<msebasic_string<_Ty, _Traits, _A, _TStateMutex> >(owner_ptr);
		}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class _Ty, class _Traits, class _A = std::allocator<_Ty>, class _TStateMutex = mse::non_thread_safe_shared_mutex>
		auto make_xscope_structure_lock_guard(const mse::TXScopeFixedConstPointer<msebasic_string<_Ty, _Traits, _A, _TStateMutex> >& owner_ptr) {
			return ns_msebasic_string::xscope_const_structure_lock_guard<msebasic_string<_Ty, _Traits, _A, _TStateMutex> >(owner_ptr);
		}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

		using msestring = msebasic_string<char>;
		using msewstring = msebasic_string<wchar_t>;
		using mseu16string = msebasic_string<char16_t>;
		using mseu32string = msebasic_string<char32_t>;
	}
}

namespace std {

	template<class _Elem, class _Traits, class _Alloc>
	struct hash<mse::us::msebasic_string<_Elem, _Traits, _Alloc> > {	// hash functor for mse::us::msebasic_string
		typedef typename mse::us::msebasic_string<_Elem, _Traits, _Alloc>::base_class base_string_t;
		using argument_type = mse::us::msebasic_string<_Elem, _Traits, _Alloc>;
		using result_type = size_t;

		size_t operator()(const mse::us::msebasic_string<_Elem, _Traits, _Alloc>& _Keyval) const _NOEXCEPT {
			auto retval = m_bs_hash(_Keyval);
			return retval;
		}

		hash<base_string_t> m_bs_hash;
	};

	template<class _Elem, class _Traits, class _Alloc>
	inline basic_istream<_Elem, _Traits>& getline(basic_istream<_Elem, _Traits>&& _Istr,
		mse::us::msebasic_string<_Elem, _Traits, _Alloc>& _Str, const _Elem _Delim) {	// get characters into string, discard delimiter
		return _Str.getline(MSE_FWD(_Istr), &_Str, _Delim);
	}
	template<class _Elem, class _Traits, class _Alloc>
	inline basic_istream<_Elem, _Traits>& getline(basic_istream<_Elem, _Traits>&& _Istr,
		mse::us::msebasic_string<_Elem, _Traits, _Alloc>& _Str) {	// get characters into string, discard newline
		return _Str.getline(MSE_FWD(_Istr), &_Str);
	}
	template<class _Elem, class _Traits, class _Alloc>
	inline basic_istream<_Elem, _Traits>& getline(basic_istream<_Elem, _Traits>& _Istr,
		mse::us::msebasic_string<_Elem, _Traits, _Alloc>& _Str, const _Elem _Delim) {	// get characters into string, discard delimiter
		return _Str.getline(_Istr, &_Str, _Delim);
	}
	template<class _Elem, class _Traits, class _Alloc>
	inline basic_istream<_Elem, _Traits>& getline(basic_istream<_Elem, _Traits>& _Istr,
		mse::us::msebasic_string<_Elem, _Traits, _Alloc>& _Str) {	// get characters into string, discard newline
		return _Str.getline(_Istr, &_Str);
	}
}


namespace mse {
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

#endif /*ndef MSEMSESTRING_H*/
