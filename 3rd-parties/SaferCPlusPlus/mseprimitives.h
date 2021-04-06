
// Copyright (c) 2015 Noah Lopez
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef MSEPRIMITIVES_H
#define MSEPRIMITIVES_H

#include <cassert>
#include <climits>       // ULONG_MAX
#include <limits>       // std::numeric_limits
#include <stdexcept>      // primitives_range_error

#ifndef MSEPOINTERBASICS_H

#define MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION

#if __cplusplus >= 201703L
#define MSE_HAS_CXX17
#endif // __cplusplus >= 201703L
/*compiler specific defines*/
#ifdef _MSC_VER
#if _MSVC_LANG >= 201703L || (defined(_HAS_CXX17) && (_HAS_CXX17 >= 1))
#define MSE_HAS_CXX17
#endif // _MSVC_LANG >= 201703L || (defined(_HAS_CXX17) && (_HAS_CXX17 >= 1))
#if (1700 > _MSC_VER)
#define MSVC2010_COMPATIBLE 1
#endif /*(1700 > _MSC_VER)*/
#if (1900 > _MSC_VER)
#define MSVC2013_COMPATIBLE 1
#endif /*(1900 > _MSC_VER)*/
#if (1910 > _MSC_VER)
#define MSVC2015_COMPATIBLE 1
#endif /*(1910 > _MSC_VER)*/
#else /*_MSC_VER*/
#if (defined(__GNUC__) || defined(__GNUG__))
#define GPP_COMPATIBLE 1
#if ((5 > __GNUC__) && (!defined(__clang__)))
#define GPP4P8_COMPATIBLE 1
#endif /*((5 > __GNUC__) && (!defined(__clang__)))*/
#endif /*(defined(__GNUC__) || defined(__GNUG__))*/
#endif /*_MSC_VER*/

#if __cpp_exceptions >= 199711
#define MSE_TRY try
#define MSE_CATCH(x) catch(x)
#define MSE_CATCH_ANY catch(...)
#define MSE_FUNCTION_TRY try
#define MSE_FUNCTION_CATCH(x) catch(x)
#define MSE_FUNCTION_CATCH_ANY catch(...)
#else // __cpp_exceptions >= 199711
#define MSE_TRY if (true)
#define MSE_CATCH(x) if (false)
#define MSE_CATCH_ANY if (false)
#define MSE_FUNCTION_TRY
#define MSE_FUNCTION_CATCH(x) void mse_placeholder_function_catch(x)
#define MSE_FUNCTION_CATCH_ANY void mse_placeholder_function_catch_any()
#define MSE_CUSTOM_THROW_DEFINITION(x) exit(-11)
#endif // __cpp_exceptions >= 199711

#endif /*ndef MSEPOINTERBASICS_H*/

#ifdef MSE_SAFER_SUBSTITUTES_DISABLED
#define MSE_PRIMITIVES_DISABLED
#endif /*MSE_SAFER_SUBSTITUTES_DISABLED*/

#if defined(MSVC2013_COMPATIBLE) || defined(MSVC2010_COMPATIBLE)
#define MSE_CONSTEXPR
#else // defined(MSVC2013_COMPATIBLE) || defined(MSVC2010_COMPATIBLE)
#define MSE_CONSTEXPR constexpr
#endif // defined(MSVC2013_COMPATIBLE) || defined(MSVC2010_COMPATIBLE)

#ifdef MSVC2015_COMPATIBLE
#ifndef MSE_FORCE_PRIMITIVE_ASSIGN_RANGE_CHECK_ENABLED
/* msvc2015's incomplete support for "constexpr" means that range checks that should be done at compile time would
be done at run time, at significant cost. So by default we disable range checks upon assignment. */
#define MSE_PRIMITIVE_ASSIGN_RANGE_CHECK_DISABLED 1
#endif // !MSE_FORCE_PRIMITIVE_ASSIGN_RANGE_CHECK_ENABLED
#endif // MSVC2015_COMPATIBLE


#ifndef MSE_PUSH_MACRO_NOT_SUPPORTED
#pragma push_macro("MSE_THROW")
#pragma push_macro("_NOEXCEPT")
#endif // !MSE_PUSH_MACRO_NOT_SUPPORTED

#ifdef MSE_CUSTOM_THROW_DEFINITION
#define MSE_THROW(x) MSE_CUSTOM_THROW_DEFINITION(x)
#else // MSE_CUSTOM_THROW_DEFINITION
#define MSE_THROW(x) throw(x)
#endif // MSE_CUSTOM_THROW_DEFINITION

#ifndef _NOEXCEPT
#define _NOEXCEPT
#endif /*_NOEXCEPT*/

/* This automatic (potential) definition of MSE_CHECK_USE_BEFORE_SET is also done in msepointerbasics.h */
#ifndef MSE_SUPPRESS_CHECK_USE_BEFORE_SET
#ifndef MSE_CHECK_USE_BEFORE_SET
#ifndef NDEBUG
#define MSE_CHECK_USE_BEFORE_SET
#endif // !NDEBUG

#if defined(MSE_NON_THREADSAFE_CHECK_USE_BEFORE_SET) && !defined(MSE_CHECK_USE_BEFORE_SET)
#define MSE_CHECK_USE_BEFORE_SET
#endif // defined(MSE_NON_THREADSAFE_CHECK_USE_BEFORE_SET) && !defined(MSE_CHECK_USE_BEFORE_SET)
#endif // !MSE_CHECK_USE_BEFORE_SET
#endif // !MSE_SUPPRESS_CHECK_USE_BEFORE_SET

#ifdef MSE_NON_THREADSAFE_CHECK_USE_BEFORE_SET
#define MSE_PRIMITIVES_IMPL_INITIALIZED_FLAG_TYPE bool
#else // MSE_NON_THREADSAFE_CHECK_USE_BEFORE_SET
#ifdef MSE_CHECK_USE_BEFORE_SET
#include <atomic>
#ifdef MSE_HAS_CXX17
#define MSE_PRIMITIVES_IMPL_INITIALIZED_FLAG_TYPE std::atomic_bool
#else // MSE_HAS_CXX17
namespace mse {
	namespace impl {
		namespace primitives {
			template<typename _ITYPE>
			class TAtomic : public std::atomic<_ITYPE> {
			public:
				typedef std::atomic<_ITYPE> _ATOMIC_ITYPE;
				using value_type = _ITYPE;
				using difference_type = _ITYPE;

				TAtomic() noexcept = default;

				constexpr TAtomic(_ITYPE _Val) noexcept
					: _ATOMIC_ITYPE{ /*(_ATOMIC_UINT)*/_Val }
				{	// construct from _Val, initialization is not TAtomic
				}
				constexpr TAtomic(const TAtomic& src) noexcept : _ATOMIC_ITYPE{ src.load() } {}

				_ITYPE operator=(_ITYPE _Val) volatile noexcept
				{	// assign from _Val
					return (_ATOMIC_ITYPE::operator=(_Val));
				}

				_ITYPE operator=(_ITYPE _Val) noexcept
				{	// assign from _Val
					return (_ATOMIC_ITYPE::operator=(_Val));
				}
			};
		}
	}
}
#define MSE_PRIMITIVES_IMPL_INITIALIZED_FLAG_TYPE mse::impl::primitives::TAtomic<bool>
#endif // MSE_HAS_CXX17
#endif // MSE_CHECK_USE_BEFORE_SET
#endif // MSE_NON_THREADSAFE_CHECK_USE_BEFORE_SET


#ifndef MSE_CINT_BASE_INTEGER_TYPE
#if (SIZE_MAX <= UINT_MAX) ||(!defined(MSE_MATCH_CINT_SIZE_TO_CSIZE_T))
#define MSE_CINT_BASE_INTEGER_TYPE int
#else // (SIZE_MAX <= UINT_MAX) ||(!defined(MSE_MATCH_CINT_SIZE_TO_CSIZE_T))
#if SIZE_MAX <= ULONG_MAX
#define MSE_CINT_BASE_INTEGER_TYPE long int
#else // SIZE_MAX <= ULONG_MAX
#define MSE_CINT_BASE_INTEGER_TYPE long long int
#endif // SIZE_MAX <= ULONG_MAX
#endif // (SIZE_MAX <= UINT_MAX) ||(!defined(MSE_MATCH_CINT_SIZE_TO_CSIZE_T))
#endif // !MSE_CINT_BASE_INTEGER_TYPE

#ifndef MSE_DEFAULT_INT_VALUE
#define MSE_DEFAULT_INT_VALUE 0
#endif // !MSE_DEFAULT_INT_VALUE

#ifdef _MSC_VER
#pragma warning( push )  
#pragma warning( disable : 4100 4456 4189 4505 )
#endif /*_MSC_VER*/

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-function"
//pragma clang diagnostic ignored "-Wunused-but-set-variable"
#else /*__clang__*/
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif /*__GNUC__*/
#endif /*__clang__*/


namespace mse {

	class primitives_range_error : public std::range_error {
	public:
		using std::range_error::range_error;
	};

	/* This class is just meant to act like the "bool" type, except that it has a default intialization value (false). */
	class CNDBool {
	public:
		// Constructs zero.
		CNDBool() : m_val(false) {}

		// Copy constructor
		CNDBool(const CNDBool &x) : m_val(x.m_val) { note_value_assignment(); };

		// Assignment operator
		CNDBool& operator=(const CNDBool &x) { note_value_assignment(); m_val = x.m_val; return (*this); }

		// Constructors from primitive boolean types
		CNDBool(bool   x) { note_value_assignment(); m_val = x; }

		// Casts to primitive boolean types
		operator bool() const { assert_initialized(); return m_val; }
		/* provisional */
		const bool& mse_base_type_ref() const & { (*this).assert_initialized(); return (*this).m_val; }
		const bool& mse_base_type_ref() const&& = delete;
		bool& mse_base_type_ref() & { (*this).assert_initialized(); return (*this).m_val; }
		bool& mse_base_type_ref() && = delete;

		bool operator ==(const CNDBool &x) const { assert_initialized(); return (m_val == x.m_val); }
		bool operator !=(const CNDBool &x) const { assert_initialized(); return (m_val != x.m_val); }
		bool operator ==(bool x) const { assert_initialized(); return (m_val == x); }
		bool operator !=(bool x) const { assert_initialized(); return (m_val != x); }

		CNDBool& operator |=(const CNDBool &x) { assert_initialized(); m_val |= x.m_val; return (*this); }
		CNDBool& operator &=(const CNDBool &x) { assert_initialized(); m_val &= x.m_val; return (*this); }
		CNDBool& operator ^=(const CNDBool &x) { assert_initialized(); m_val ^= x.m_val; return (*this); }

#ifdef MSE_CHECK_USE_BEFORE_SET
		void assert_initialized() const { assert(m_initialized); }
#else // MSE_CHECK_USE_BEFORE_SET
		void assert_initialized() const {}
#endif // MSE_CHECK_USE_BEFORE_SET

		void async_shareable_and_passable_tag() const {}

	private:
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		bool m_val;

#ifdef MSE_CHECK_USE_BEFORE_SET
		void note_value_assignment() { m_initialized = true; }
		MSE_PRIMITIVES_IMPL_INITIALIZED_FLAG_TYPE m_initialized = false;
#else // MSE_CHECK_USE_BEFORE_SET
		void note_value_assignment() {}
#endif // MSE_CHECK_USE_BEFORE_SET
	};
}

namespace std {
	template<>
	struct hash<mse::CNDBool> {	// hash functor
		typedef mse::CNDBool argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::CNDBool& _Keyval) const _NOEXCEPT {
			return (hash<bool>()(_Keyval));
		}
	};
}

namespace mse {

	template<typename _TBaseInt = MSE_CINT_BASE_INTEGER_TYPE> class TInt;

	typedef TInt<MSE_CINT_BASE_INTEGER_TYPE> CNDInt;

	class CNDSize_t;
	static size_t as_a_size_t(CNDSize_t n);

	namespace impl {
		template<typename _TDestination, typename _TSource>
		MSE_CONSTEXPR static bool sg_can_exceed_upper_bound() {

			return (
				((std::numeric_limits<_TSource>::is_signed == std::numeric_limits<_TDestination>::is_signed)
					&& (std::numeric_limits<_TSource>::digits > std::numeric_limits<_TDestination>::digits))
				|| ((std::numeric_limits<_TSource>::is_signed != std::numeric_limits<_TDestination>::is_signed)
					&& ((std::numeric_limits<_TSource>::is_signed && (std::numeric_limits<_TSource>::digits > (1 + std::numeric_limits<_TDestination>::digits)))
						|| ((!std::numeric_limits<_TSource>::is_signed) && ((1 + std::numeric_limits<_TSource>::digits) > std::numeric_limits<_TDestination>::digits))
						)
					)
				);
		}
		template<typename _TDestination, typename _TSource>
		MSE_CONSTEXPR static bool sg_can_exceed_lower_bound() {
			return (
				(std::numeric_limits<_TSource>::is_signed && (!std::numeric_limits<_TDestination>::is_signed))
				|| (std::numeric_limits<_TSource>::is_signed && (std::numeric_limits<_TSource>::digits > std::numeric_limits<_TDestination>::digits))
				);
		}

		template<typename _TDestination, typename _TSource>
		void g_assign_check_range(const _TSource &x) {
#ifndef MSE_PRIMITIVE_ASSIGN_RANGE_CHECK_DISABLED
			/* This probably needs to be cleaned up. But at the moment this should be mostly compile time complexity. And
			as is it avoids "signed/unsigned" mismatch warnings. */
			MSE_CONSTEXPR const bool rhs_can_exceed_upper_bound = impl::sg_can_exceed_upper_bound<_TDestination, _TSource>();
			MSE_CONSTEXPR const bool rhs_can_exceed_lower_bound = impl::sg_can_exceed_lower_bound<_TDestination, _TSource>();
			MSE_CONSTEXPR const bool can_exceed_bounds = rhs_can_exceed_upper_bound || rhs_can_exceed_lower_bound;
			if (can_exceed_bounds) {
				if (rhs_can_exceed_upper_bound) {

#ifdef MSE_SUPPRESS_CSIZE_T_TO_CINT_CONVERSION_RANGE_CHECK
					MSE_CONSTEXPR const bool this_is_a_CSize_t_to_CInt_conversion = ((std::is_same<_TDestination, MSE_CINT_BASE_INTEGER_TYPE>::value || std::is_same<_TDestination, CNDInt>::value)
						&& (std::is_same<_TSource, CNDSize_t>::value || std::is_same<_TSource, size_t>::value));
					if (!this_is_a_CSize_t_to_CInt_conversion) {
#endif // MSE_SUPPRESS_CSIZE_T_TO_CINT_CONVERSION_RANGE_CHECK

					if (x > _TSource(std::numeric_limits<_TDestination>::max())) {
						MSE_THROW(primitives_range_error("range error - value to be assigned is out of range of the target (integer) type"));
					}

#ifdef MSE_SUPPRESS_CSIZE_T_TO_CINT_CONVERSION_RANGE_CHECK
					}
#endif // MSE_SUPPRESS_CSIZE_T_TO_CINT_CONVERSION_RANGE_CHECK
				}
				if (rhs_can_exceed_lower_bound) {
					/* We're assuming that std::numeric_limits<>::lowest() will never be greater than zero. */
					if (_TSource(0) > x) {
						if (0 == std::numeric_limits<_TDestination>::lowest()) {
							MSE_THROW(primitives_range_error("range error - value to be assigned is out of range of the target (integer) type"));
						}
						else if (x < _TSource(std::numeric_limits<_TDestination>::lowest())) {
							MSE_THROW(primitives_range_error("range error - value to be assigned is out of range of the target (integer) type"));
						}
					}
				}
			}
#endif // !MSE_PRIMITIVE_ASSIGN_RANGE_CHECK_DISABLED
		}
	}

//define MSE_TINT_TYPE_WITH_THE_LOWER_FLOOR(_Ty, _Tz) typename std::conditional<impl::sg_can_exceed_lower_bound<_Tz, _Ty>(), _Ty, _Tz>::type

	namespace impl {
		/* We don't assume that char, short, long and long long all have distinct sizes. But in the common case where
		each is double the size of the preceding type, efficient (run-time) arithmetic overflow detection for types
		smaller other than the largest ones (long long and unsigned long long) can be enabled (by defining
		MSE_RETURN_RANGE_EXTENDED_TYPE_FOR_INTEGER_ARITHMETIC). */
		template<typename _Ty> struct next_bigger_native_int_type { typedef _Ty type; };
		template<> struct next_bigger_native_int_type<char> { typedef short int type; };
		template<> struct next_bigger_native_int_type<signed char> { typedef short int type; };
		template<> struct next_bigger_native_int_type<short int> { typedef long int type; };
		template<> struct next_bigger_native_int_type<long int> { typedef long long int type; };
		template<> struct next_bigger_native_int_type<unsigned char> { typedef unsigned short int type; };
		template<> struct next_bigger_native_int_type<unsigned short int> { typedef unsigned long int type; };
		template<> struct next_bigger_native_int_type<unsigned long int> { typedef unsigned long long int type; };
		template<> struct next_bigger_native_int_type<int> {
			typedef typename std::conditional<(std::numeric_limits<long int>::digits > std::numeric_limits<int>::digits), long int, long long int>::type type;
		};
		template<> struct next_bigger_native_int_type<unsigned int> {
			typedef typename std::conditional<(std::numeric_limits<unsigned long int>::digits > std::numeric_limits<unsigned int>::digits), unsigned long int, unsigned long long int>::type type;
		};

		namespace range_encompassing {
			template<typename _Ty, typename _Tz>
			struct next_bigger_candidate {
				typedef typename std::conditional<std::is_signed<_Ty>::value || std::is_signed<_Tz>::value
					, typename std::make_signed<_Ty>::type, _Ty>::type candidate_type;

				typedef typename next_bigger_native_int_type<candidate_type>::type type;
			};
			template<typename _Ty, typename _Tz, typename candidate_type>
			struct last_encompasses_first_two {
				static const bool value = !((sg_can_exceed_upper_bound<candidate_type, _Ty>()) || (sg_can_exceed_upper_bound<candidate_type, _Tz>())
					|| (sg_can_exceed_lower_bound<candidate_type, _Ty>()) || (sg_can_exceed_lower_bound<candidate_type, _Tz>()));
			};
			template<typename _Ty>
			struct is_biggest_available_type {
				static const bool value = std::is_same<long long int, _Ty>::value || std::is_same<unsigned long long int, _Ty>::value;
			};

			/* range_encompassing_native_int_type1<> determines the smallest (integer) type whose range encompasses the
			ranges of the two given (integer) types. Ideally it would use a recursive algorithm to accomplish this. But
			it was not readily apparent how to do the recursion in a supported way. So knowing that the maximum
			recursion depth needed is limited, we just manually emulate the recursion by creating a distinct struct for
			each level of recursion. */
			template<typename candidate_type, typename _Tz> struct range_encompassing_native_int_type_helper7 {
				typedef void type; /* Should induce a compile error if this type is actually ever used. */
			};
			template<typename candidate_type, typename _Tz> struct range_encompassing_native_int_type_helper6 {
				typedef typename std::conditional<!(is_biggest_available_type<candidate_type>::value || last_encompasses_first_two<candidate_type, _Tz, candidate_type>::value)
					, typename range_encompassing_native_int_type_helper7<typename next_bigger_candidate<candidate_type, _Tz>::type, _Tz>::type
					, candidate_type>::type type;
			};
			template<typename candidate_type, typename _Tz> struct range_encompassing_native_int_type_helper5 {
				typedef typename std::conditional<!(is_biggest_available_type<candidate_type>::value || last_encompasses_first_two<candidate_type, _Tz, candidate_type>::value)
					, typename range_encompassing_native_int_type_helper6<typename next_bigger_candidate<candidate_type, _Tz>::type, _Tz>::type
					, candidate_type>::type type;
			};
			template<typename candidate_type, typename _Tz> struct range_encompassing_native_int_type_helper4 {
				typedef typename std::conditional<!(is_biggest_available_type<candidate_type>::value || last_encompasses_first_two<candidate_type, _Tz, candidate_type>::value)
					, typename range_encompassing_native_int_type_helper5<typename next_bigger_candidate<candidate_type, _Tz>::type, _Tz>::type
					, candidate_type>::type type;
			};
			template<typename candidate_type, typename _Tz> struct range_encompassing_native_int_type_helper3 {
				typedef typename std::conditional<!(is_biggest_available_type<candidate_type>::value || last_encompasses_first_two<candidate_type, _Tz, candidate_type>::value)
					, typename range_encompassing_native_int_type_helper4<typename next_bigger_candidate<candidate_type, _Tz>::type, _Tz>::type
					, candidate_type>::type type;
			};
			template<typename candidate_type, typename _Tz> struct range_encompassing_native_int_type_helper2 {
				typedef typename std::conditional<!(is_biggest_available_type<candidate_type>::value || last_encompasses_first_two<candidate_type, _Tz, candidate_type>::value)
					, typename range_encompassing_native_int_type_helper3<typename next_bigger_candidate<candidate_type, _Tz>::type, _Tz>::type
					, candidate_type>::type type;
			};

			/* This template struct should deduce the smallest native integer type that can encompass the combined range
			of both template parameters (or the one that comes closest if there isn't one). */
			template<typename _Ty, typename _Tz> struct range_encompassing_native_int_type {
				typedef typename std::conditional<std::is_signed<_Ty>::value || std::is_signed<_Tz>::value, typename std::make_signed<_Ty>::type, _Ty>::type candidate_type;

				typedef typename std::conditional<!(is_biggest_available_type<candidate_type>::value || last_encompasses_first_two<_Ty, _Tz, candidate_type>::value)
					, typename range_encompassing_native_int_type_helper2<typename next_bigger_candidate<candidate_type, _Tz>::type, _Tz>::type
					, candidate_type>::type type;
			};
		}

		template<typename _Ty>
		struct corresponding_TInt { typedef typename std::conditional<std::is_arithmetic<_Ty>::value, TInt<_Ty>, _Ty>::type type; };
#define MSE_TINT_TYPE(_Ty) typename mse::impl::corresponding_TInt<_Ty>::type
#define MSE_NATIVE_INT_TYPE(_Ty) MSE_TINT_TYPE(_Ty)::base_int_type

#define MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(_Ty, _Tz) typename mse::impl::range_encompassing::range_encompassing_native_int_type<MSE_NATIVE_INT_TYPE(_Ty), MSE_NATIVE_INT_TYPE(_Tz)>::type

#ifdef MSE_RETURN_RANGE_EXTENDED_TYPE_FOR_INTEGER_ARITHMETIC
#define MSE_NATIVE_INT_RESULT_TYPE1(_Ty, _Tz) typename mse::impl::next_bigger_native_int_type<MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(_Ty, _Tz)>::type
#define MSE_NATIVE_INT_ADD_RESULT_TYPE1(_Ty, _Tz) typename mse::impl::next_bigger_native_int_type<MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(_Ty, _Tz)>::type
#define MSE_NATIVE_INT_SUBTRACT_RESULT_TYPE1(_Ty, _Tz) typename std::conditional<std::is_signed<_Ty>::value || std::is_signed<_Tz>::value \
, typename mse::impl::next_bigger_native_int_type<MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(_Ty, _Tz)>::type, typename std::make_signed<MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(_Ty, _Tz)>::type>::type
#define MSE_NATIVE_INT_MULTIPLY_RESULT_TYPE1(_Ty, _Tz) typename mse::impl::next_bigger_native_int_type<MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(_Ty, _Tz)>::type
#define MSE_NATIVE_INT_DIVIDE_RESULT_TYPE1(_Ty, _Tz) MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(_Ty, _Tz)
#else // MSE_RETURN_RANGE_EXTENDED_TYPE_FOR_INTEGER_ARITHMETIC
#define MSE_NATIVE_INT_RESULT_TYPE1(_Ty, _Tz) MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(_Ty, _Tz)
#define MSE_NATIVE_INT_ADD_RESULT_TYPE1(_Ty, _Tz) MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(_Ty, _Tz)
#define MSE_NATIVE_INT_SUBTRACT_RESULT_TYPE1(_Ty, _Tz) typename std::make_signed<MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(_Ty, _Tz)>::type
#define MSE_NATIVE_INT_MULTIPLY_RESULT_TYPE1(_Ty, _Tz) MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(_Ty, _Tz)
#define MSE_NATIVE_INT_DIVIDE_RESULT_TYPE1(_Ty, _Tz) MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(_Ty, _Tz)
#endif //MSE_RETURN_RANGE_EXTENDED_TYPE_FOR_INTEGER_ARITHMETIC

#define MSE_TINT_RESULT_TYPE1(_Ty, _Tz) TInt<MSE_NATIVE_INT_RESULT_TYPE1(MSE_NATIVE_INT_TYPE(_Ty), MSE_NATIVE_INT_TYPE(_Tz))>
#define MSE_TINT_ADD_RESULT_TYPE1(_Ty, _Tz) TInt<MSE_NATIVE_INT_ADD_RESULT_TYPE1(MSE_NATIVE_INT_TYPE(_Ty), MSE_NATIVE_INT_TYPE(_Tz))>
#define MSE_TINT_SUBTRACT_RESULT_TYPE1(_Ty, _Tz) TInt<MSE_NATIVE_INT_SUBTRACT_RESULT_TYPE1(MSE_NATIVE_INT_TYPE(_Ty), MSE_NATIVE_INT_TYPE(_Tz))>
#define MSE_TINT_MULTIPLY_RESULT_TYPE1(_Ty, _Tz) TInt<MSE_NATIVE_INT_MULTIPLY_RESULT_TYPE1(MSE_NATIVE_INT_TYPE(_Ty), MSE_NATIVE_INT_TYPE(_Tz))>
#define MSE_TINT_DIVIDE_RESULT_TYPE1(_Ty, _Tz) TInt<MSE_NATIVE_INT_DIVIDE_RESULT_TYPE1(MSE_NATIVE_INT_TYPE(_Ty), MSE_NATIVE_INT_TYPE(_Tz))>

		/* The CNDInt and CNDSize_t classes are meant to substitute for standard "int" and "size_t" types. The differences between
		the standard types and these classes are that the classes have a default intialization value (zero), and the
		classes, as much as possible, try to prevent the problematic behaviour of (possibly negative) signed integers
		being cast (inadvertently) to the unsigned size_t type. For example, the expression (0 > (int)5 - (size_t)7) evaluates
		(unintuitively) to false, whereas the expression (0 > (CNDInt)5 - (CNDSize_t)7) evaluates to true. Also, the classes do
		some range checking. For example, the code "CNDSize_t s = -2;" will throw an exception. */
		template<typename _Ty>
		class TIntBase1 {
		public:
			// Constructs zero.
			TIntBase1() : m_val(MSE_DEFAULT_INT_VALUE) {}

			// Copy constructor
			TIntBase1(const TIntBase1 &x) : m_val(x.m_val) { note_value_assignment(); };

			// Constructors from primitive integer types
			explicit TIntBase1(_Ty x) : m_val(x) { note_value_assignment(); }

			template<typename _Tz>
			void assign_check_range(const _Tz &x) {
				note_value_assignment();
				impl::g_assign_check_range<_Ty, _Tz>(x);
			}

			_Ty m_val;

#ifdef MSE_CHECK_USE_BEFORE_SET
			void note_value_assignment() { m_initialized = true; }
			void assert_initialized() const { assert(m_initialized); }
			MSE_PRIMITIVES_IMPL_INITIALIZED_FLAG_TYPE m_initialized = false;
#else // MSE_CHECK_USE_BEFORE_SET
			void note_value_assignment() {}
			void assert_initialized() const {}
#endif // MSE_CHECK_USE_BEFORE_SET
		};
	}
	

	template<typename _TBaseInt/* = MSE_CINT_BASE_INTEGER_TYPE*/>
	class TInt : private impl::TIntBase1<_TBaseInt> {
	public:
		typedef impl::TIntBase1<_TBaseInt> base_class;
		typedef _TBaseInt base_int_type;

		TInt() : base_class() {}
		TInt(const TInt &x) : base_class(x) {};
		//explicit TInt(const base_class &x) : base_class(x) {};
		//explicit TInt(const CNDSize_t &x) : base_class(CNDInt(x)) { (*this).template assign_check_range<CNDInt>(CNDInt(x)); };

		template<typename _Ty/*, class = typename std::enable_if<!std::is_same<typename std::remove_reference<
			typename std::remove_const<_Ty>::type>::type, CNDSize_t>::value, void>::type*/>
		TInt(const _Ty& x) : base_class(checked_and_adjusted_x(x)) {}

		TInt& operator=(const TInt &x) { (*this).note_value_assignment(); (*this).m_val = x.m_val; return (*this); }
		template<typename _Ty>
		TInt& operator=(const _Ty& x) { (*this).template assign_check_range<_Ty>(x); (*this).m_val = x/*static_cast<base_int_type>(x)*/; return (*this); }

		operator base_int_type() const { (*this).assert_initialized(); return (*this).m_val; }
		/* provisional */
		const base_int_type& mse_base_type_ref() const & { (*this).assert_initialized(); return (*this).m_val; }
		const base_int_type& mse_base_type_ref() const&& = delete;
		base_int_type& mse_base_type_ref() & { (*this).assert_initialized(); return (*this).m_val; }
		base_int_type& mse_base_type_ref() && = delete;

		TInt operator ~() const { (*this).assert_initialized(); return TInt(~(*this).m_val); }
		TInt& operator |=(const TInt &x) { (*this).assert_initialized(); (*this).m_val |= x.m_val; return (*this); }
		TInt& operator &=(const TInt &x) { (*this).assert_initialized(); (*this).m_val &= x.m_val; return (*this); }
		TInt& operator ^=(const TInt &x) { (*this).assert_initialized(); (*this).m_val ^= x.m_val; return (*this); }

		auto operator -() const ->MSE_TINT_SUBTRACT_RESULT_TYPE1(TInt, TInt) { (*this).assert_initialized(); return -MSE_NATIVE_INT_SUBTRACT_RESULT_TYPE1(base_int_type, base_int_type)((*this).m_val); }
		TInt& operator +=(const TInt &x) { (*this).assert_initialized(); (*this).m_val += x.m_val; return (*this); }
		TInt& operator -=(const TInt &x) { (*this).assert_initialized(); (*this).m_val -= x.m_val; return (*this); }
		TInt& operator *=(const TInt &x) { (*this).assert_initialized(); (*this).m_val *= x.m_val; return (*this); }
		TInt& operator /=(const TInt &x) { (*this).assert_initialized(); (*this).m_val /= x.m_val; return (*this); }
		TInt& operator %=(const TInt &x) { (*this).assert_initialized(); (*this).m_val %= x.m_val; return (*this); }
		TInt& operator >>=(const TInt &x) { (*this).assert_initialized(); (*this).m_val >>= x.m_val; return (*this); }
		TInt& operator <<=(const TInt &x) { (*this).assert_initialized(); (*this).m_val <<= x.m_val; return (*this); }

		auto operator +(const TInt &x) const -> MSE_TINT_ADD_RESULT_TYPE1(TInt, TInt) { (*this).assert_initialized(); return (MSE_NATIVE_INT_ADD_RESULT_TYPE1(base_int_type, base_int_type)((*this).m_val) + MSE_NATIVE_INT_ADD_RESULT_TYPE1(base_int_type, base_int_type)(x.m_val)); }
		template<typename _Ty2>
		auto operator +(const TInt<_Ty2> &x) const ->MSE_TINT_ADD_RESULT_TYPE1(TInt, TInt<_Ty2>) { (*this).assert_initialized(); return (MSE_TINT_ADD_RESULT_TYPE1(TInt, TInt<_Ty2>)((*this).m_val) + MSE_TINT_ADD_RESULT_TYPE1(TInt, TInt<_Ty2>)(x.m_val)); }
		template<typename _Ty2, class = typename std::enable_if<std::is_integral<_Ty2>::value, void>::type>
		auto operator +(_Ty2 x) const { (*this).assert_initialized(); return ((*this) + TInt<_Ty2>(x)); }

		auto operator -(const TInt &x) const ->MSE_TINT_SUBTRACT_RESULT_TYPE1(TInt, TInt) { (*this).assert_initialized(); return (MSE_NATIVE_INT_SUBTRACT_RESULT_TYPE1(base_int_type, base_int_type)((*this).m_val) - MSE_NATIVE_INT_SUBTRACT_RESULT_TYPE1(base_int_type, base_int_type)(x.m_val)); }
		template<typename _Ty2>
		auto operator -(const TInt<_Ty2> &x) const ->MSE_TINT_SUBTRACT_RESULT_TYPE1(TInt, TInt<_Ty2>) { (*this).assert_initialized(); return (MSE_TINT_SUBTRACT_RESULT_TYPE1(TInt, TInt<_Ty2>)((*this).m_val) - MSE_TINT_SUBTRACT_RESULT_TYPE1(TInt, TInt<_Ty2>)(x.m_val)); }
		template<typename _Ty2, class = typename std::enable_if<std::is_integral<_Ty2>::value, void>::type>
		auto operator -(_Ty2 x) const { (*this).assert_initialized(); return ((*this) - TInt<_Ty2>(x)); }

		auto operator *(const TInt &x) const ->MSE_TINT_MULTIPLY_RESULT_TYPE1(TInt, TInt) { (*this).assert_initialized(); return (MSE_NATIVE_INT_MULTIPLY_RESULT_TYPE1(base_int_type, base_int_type)((*this).m_val) * MSE_NATIVE_INT_MULTIPLY_RESULT_TYPE1(base_int_type, base_int_type)(x.m_val)); }
		template<typename _Ty2>
		auto operator *(const TInt<_Ty2> &x) const ->MSE_TINT_MULTIPLY_RESULT_TYPE1(TInt, TInt<_Ty2>) { (*this).assert_initialized(); return (MSE_TINT_MULTIPLY_RESULT_TYPE1(TInt, TInt<_Ty2>)((*this).m_val) * MSE_TINT_MULTIPLY_RESULT_TYPE1(TInt, TInt<_Ty2>)(x.m_val)); }
		template<typename _Ty2, class = typename std::enable_if<std::is_integral<_Ty2>::value, void>::type>
		auto operator *(_Ty2 x) const { (*this).assert_initialized(); return ((*this) * TInt<_Ty2>(x)); }

		auto operator /(const TInt &x) const->MSE_TINT_DIVIDE_RESULT_TYPE1(TInt, TInt) {
			if (x.m_val == 0) { MSE_THROW(std::domain_error("attempted division by zero - TInt")); }
			(*this).assert_initialized(); return (MSE_NATIVE_INT_DIVIDE_RESULT_TYPE1(base_int_type, base_int_type)((*this).m_val) / MSE_NATIVE_INT_DIVIDE_RESULT_TYPE1(base_int_type, base_int_type)(x.m_val));
		}
		template<typename _Ty2>
		auto operator /(const TInt<_Ty2> &x) const->MSE_TINT_DIVIDE_RESULT_TYPE1(TInt, TInt<_Ty2>) { (*this).assert_initialized(); return (MSE_TINT_DIVIDE_RESULT_TYPE1(TInt, TInt<_Ty2>)((*this).m_val) / MSE_TINT_DIVIDE_RESULT_TYPE1(TInt, TInt<_Ty2>)(x.m_val)); }
		template<typename _Ty2, class = typename std::enable_if<std::is_integral<_Ty2>::value, void>::type>
		auto operator /(_Ty2 x) const { (*this).assert_initialized(); return ((*this) / TInt<_Ty2>(x)); }

		bool operator <(const TInt &x) const { (*this).assert_initialized(); return (((*this).m_val) < (x.m_val)); }
		template<typename _Ty2>
		bool operator <(const TInt<_Ty2> &x) const { (*this).assert_initialized(); return (MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(TInt, TInt<_Ty2>)((*this).m_val) < MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(TInt, TInt<_Ty2>)(x.m_val)); }
		template<typename _Ty2, class = typename std::enable_if<std::is_integral<_Ty2>::value, void>::type>
		bool operator <(_Ty2 x) const { (*this).assert_initialized(); return ((*this) < TInt<_Ty2>(x)); }

		bool operator >(const TInt &x) const { (*this).assert_initialized(); return (((*this).m_val) > (x.m_val)); }
		template<typename _Ty2>
		bool operator >(const TInt<_Ty2> &x) const { (*this).assert_initialized(); return (MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(TInt, TInt<_Ty2>)((*this).m_val) > MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(TInt, TInt<_Ty2>)(x.m_val)); }
		template<typename _Ty2, class = typename std::enable_if<std::is_integral<_Ty2>::value, void>::type>
		bool operator >(_Ty2 x) const { (*this).assert_initialized(); return ((*this) > TInt<_Ty2>(x)); }

		bool operator <=(const TInt &x) const { (*this).assert_initialized(); return (((*this).m_val) <= (x.m_val)); }
		template<typename _Ty2>
		bool operator <=(const TInt<_Ty2> &x) const { (*this).assert_initialized(); return (MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(TInt, TInt<_Ty2>)((*this).m_val) <= MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(TInt, TInt<_Ty2>)(x.m_val)); }
		template<typename _Ty2, class = typename std::enable_if<std::is_integral<_Ty2>::value, void>::type>
		bool operator <=(_Ty2 x) const { (*this).assert_initialized(); return ((*this) <= TInt<_Ty2>(x)); }

		bool operator >=(const TInt &x) const { (*this).assert_initialized(); return (((*this).m_val) >= (x.m_val)); }
		template<typename _Ty2>
		bool operator >=(const TInt<_Ty2> &x) const { (*this).assert_initialized(); return (MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(TInt, TInt<_Ty2>)((*this).m_val) >= MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(TInt, TInt<_Ty2>)(x.m_val)); }
		template<typename _Ty2, class = typename std::enable_if<std::is_integral<_Ty2>::value, void>::type>
		bool operator >=(_Ty2 x) const { (*this).assert_initialized(); return ((*this) >= TInt<_Ty2>(x)); }

		bool operator ==(const TInt &x) const { (*this).assert_initialized(); return (((*this).m_val) == (x.m_val)); }
		template<typename _Ty2>
		bool operator ==(const TInt<_Ty2> &x) const { (*this).assert_initialized(); return (MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(TInt, TInt<_Ty2>)((*this).m_val) == MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(TInt, TInt<_Ty2>)(x.m_val)); }
		template<typename _Ty2, class = typename std::enable_if<std::is_integral<_Ty2>::value, void>::type>
		bool operator ==(_Ty2 x) const { (*this).assert_initialized(); return ((*this) == TInt<_Ty2>(x)); }

		bool operator !=(const TInt &x) const { (*this).assert_initialized(); return (((*this).m_val) != (x.m_val)); }
		template<typename _Ty2>
		bool operator !=(const TInt<_Ty2> &x) const { (*this).assert_initialized(); return (MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(TInt, TInt<_Ty2>)((*this).m_val) != MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(TInt, TInt<_Ty2>)(x.m_val)); }
		template<typename _Ty2, class = typename std::enable_if<std::is_integral<_Ty2>::value, void>::type>
		bool operator !=(_Ty2 x) const { (*this).assert_initialized(); return ((*this) != TInt<_Ty2>(x)); }

		// INCREMENT/DECREMENT OPERATORS
		TInt& operator ++() { (*this).assert_initialized(); (*this).m_val++; return (*this); }
		TInt operator ++(int) {
			(*this).assert_initialized();
			TInt tmp(*this); // copy
			operator++(); // pre-increment
			return tmp;   // return old value
		}
		TInt& operator --() {
			(*this).assert_initialized();
			if (0 <= std::numeric_limits<base_int_type>::lowest()) {
				(*this) = (*this) - 1; return (*this);
			}
			else {
				(*this).m_val--; return (*this);
			}
		}
		TInt operator --(int) {
			(*this).assert_initialized();
			TInt tmp(*this); // copy
			operator--(); // pre-decrement
			return tmp;   // return old value
		}

		void async_shareable_and_passable_tag() const {}

	private:
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		template<typename _Ty2>
		auto checked_and_adjusted_x(const _Ty2& x) {
			(*this).template assign_check_range<_Ty2>(x);
			return static_cast<base_int_type>(x);
		}

		template<typename _Ty2> friend class TInt;
		friend class CNDSize_t;
	};
}

namespace std {
#ifndef _THROW0
#define _THROW0()
#endif // !_THROW0
#ifndef _STCONS
#define _STCONS(ty, name, val)	static constexpr ty name = static_cast<ty>(val)
#endif // !_STCONS

	template<typename _TBaseInt>
	class numeric_limits<mse::TInt<_TBaseInt>> {	// limits for type int
	public:
		typedef _TBaseInt base_int_type;

		static constexpr base_int_type(min)() _THROW0()
		{	// return minimum value
			return numeric_limits<_TBaseInt>::min();
		}
		static constexpr base_int_type(max)() _THROW0()
		{	// return maximum value
			return numeric_limits<_TBaseInt>::max();
		}
		static constexpr base_int_type lowest() _THROW0()
		{	// return most negative value
			return numeric_limits<_TBaseInt>::lowest();
		}
		static constexpr base_int_type epsilon() _THROW0()
		{	// return smallest effective increment from 1.0
			return numeric_limits<_TBaseInt>::epsilon();
		}
		static constexpr base_int_type round_error() _THROW0()
		{	// return largest rounding error
			return numeric_limits<_TBaseInt>::round_error();
		}
		static constexpr base_int_type denorm_min() _THROW0()
		{	// return minimum denormalized value
			return numeric_limits<_TBaseInt>::denorm_min();
		}
		static constexpr base_int_type infinity() _THROW0()
		{	// return positive infinity
			return numeric_limits<_TBaseInt>::infinity();
		}
		static constexpr base_int_type quiet_NaN() _THROW0()
		{	// return non-signaling NaN
			return numeric_limits<_TBaseInt>::quiet_NaN();
		}
		static constexpr base_int_type signaling_NaN() _THROW0()
		{	// return signaling NaN
			return numeric_limits<_TBaseInt>::signaling_NaN();
		}
		_STCONS(float_denorm_style, has_denorm, numeric_limits<_TBaseInt>::has_denorm);
		_STCONS(bool, has_denorm_loss, numeric_limits<_TBaseInt>::has_denorm_loss);
		_STCONS(bool, has_infinity, numeric_limits<_TBaseInt>::has_infinity);
		_STCONS(bool, has_quiet_NaN, numeric_limits<_TBaseInt>::has_quiet_NaN);
		_STCONS(bool, has_signaling_NaN, numeric_limits<_TBaseInt>::has_signaling_NaN);
		_STCONS(bool, is_bounded, numeric_limits<_TBaseInt>::is_bounded);
		_STCONS(bool, is_exact, numeric_limits<_TBaseInt>::is_exact);
		_STCONS(bool, is_iec559, numeric_limits<_TBaseInt>::is_iec559);
		_STCONS(bool, is_integer, numeric_limits<_TBaseInt>::is_integer);
		_STCONS(bool, is_modulo, numeric_limits<_TBaseInt>::is_modulo);
		_STCONS(bool, is_signed, numeric_limits<_TBaseInt>::is_signed);
		_STCONS(bool, is_specialized, numeric_limits<_TBaseInt>::is_specialized);
		_STCONS(bool, tinyness_before, numeric_limits<_TBaseInt>::tinyness_before);
		_STCONS(bool, traps, numeric_limits<_TBaseInt>::traps);
		_STCONS(float_round_style, round_style, numeric_limits<_TBaseInt>::round_style);
		_STCONS(int, digits, numeric_limits<_TBaseInt>::digits);
		_STCONS(int, digits10, numeric_limits<_TBaseInt>::digits10);
		_STCONS(int, max_digits10, numeric_limits<_TBaseInt>::max_digits10);
		_STCONS(int, max_exponent, numeric_limits<_TBaseInt>::max_exponent);
		_STCONS(int, max_exponent10, numeric_limits<_TBaseInt>::max_exponent10);
		_STCONS(int, min_exponent, numeric_limits<_TBaseInt>::min_exponent);
		_STCONS(int, min_exponent10, numeric_limits<_TBaseInt>::min_exponent10);
		_STCONS(int, radix, numeric_limits<_TBaseInt>::radix);
	};

	template<typename _TBaseInt>
	struct hash<mse::TInt<_TBaseInt>> {	// hash functor
		typedef mse::TInt<_TBaseInt> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TInt<_TBaseInt>& _Keyval) const _NOEXCEPT {
			return (hash<typename mse::TInt<_TBaseInt>::base_int_type>()(_Keyval));
		}
	};
}

namespace mse {
	/* Note that CNDSize_t does not have a default conversion to size_t. This is by design. Use the as_a_size_t() member
	function to get a size_t when necessary. */
	class CNDSize_t : private impl::TIntBase1<size_t> {
	public:
		typedef impl::TIntBase1<size_t> base_class;
		typedef size_t base_int_type;
#ifdef MSE_RETURN_RANGE_EXTENDED_TYPE_FOR_INTEGER_ARITHMETIC
		typedef MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(base_int_type, signed char/*just to make sure the resulting type is signed*/) signed_size_t;
#else // MSE_RETURN_RANGE_EXTENDED_TYPE_FOR_INTEGER_ARITHMETIC
		typedef typename CNDInt::base_int_type signed_size_t;
#endif //MSE_RETURN_RANGE_EXTENDED_TYPE_FOR_INTEGER_ARITHMETIC
		typedef TInt<signed_size_t> CNDSignedSize_t;

		CNDSize_t() : base_class() {}
		CNDSize_t(const CNDSize_t &x) : base_class(x) {};
		//CNDSize_t(const base_class &x) : base_class(x) {};
		/*explicit */CNDSize_t(CNDInt x) : base_class(static_cast<base_int_type>(x)) { (*this).template assign_check_range<CNDInt>(x); }

		template<typename _Ty>
		CNDSize_t(const _Ty& x) : base_class(static_cast<base_int_type>(x)) { (*this).template assign_check_range<_Ty>(x); }

		CNDSize_t& operator=(const CNDSize_t &x) { (*this).note_value_assignment(); (*this).m_val = x.m_val; return (*this); }
		template<typename _Ty>
		CNDSize_t& operator=(const _Ty& x) { (*this).template assign_check_range<_Ty>(x); (*this).m_val = static_cast<base_int_type>(x); return (*this); }

		operator signed_size_t() const { (*this).assert_initialized(); impl::g_assign_check_range<signed_size_t, decltype(m_val)>(m_val); return signed_size_t(m_val); }
#ifndef MSVC2010_COMPATIBLE
		explicit operator CNDSignedSize_t() const { (*this).assert_initialized(); return CNDSignedSize_t(m_val); }
		//explicit operator CNDInt() const { (*this).assert_initialized(); return CNDInt(m_val); }
		explicit operator size_t() const { (*this).assert_initialized(); return (m_val); }
		/* provisional */
		const size_t& mse_base_type_ref() const & { (*this).assert_initialized(); return (*this).m_val; }
		const size_t& mse_base_type_ref() const&& = delete;
		size_t& mse_base_type_ref() & { (*this).assert_initialized(); return (*this).m_val; }
		size_t& mse_base_type_ref() && = delete;

		//explicit operator typename CNDInt::base_int_type() const { (*this).assert_initialized(); return CNDInt(m_val); }
#endif /*MSVC2010_COMPATIBLE*/
		//size_t as_a_size_t() const { (*this).assert_initialized(); return m_val; }

		CNDSize_t operator ~() const { (*this).assert_initialized(); return (~m_val); }
		CNDSize_t& operator |=(const CNDSize_t &x) { (*this).assert_initialized(); m_val |= x.m_val; return (*this); }
		CNDSize_t& operator &=(const CNDSize_t &x) { (*this).assert_initialized(); m_val &= x.m_val; return (*this); }
		CNDSize_t& operator ^=(const CNDSize_t &x) { (*this).assert_initialized(); m_val ^= x.m_val; return (*this); }

		CNDSignedSize_t operator -() const { (*this).assert_initialized(); /* Should unsigned types even support this operator? */
			return (-(CNDSignedSize_t(m_val)));
		}
		CNDSize_t& operator +=(const CNDSize_t &x) { (*this).assert_initialized(); m_val += x.m_val; return (*this); }
		CNDSize_t& operator -=(const CNDSize_t &x) {
			(*this).assert_initialized();
			//assert(0 <= std::numeric_limits<base_int_type>::lowest());
			if (x.m_val > m_val) {
				MSE_THROW(primitives_range_error("range error - value to be assigned is out of range of the target (integer) type"));
			}
			m_val -= x.m_val; return (*this);
		}
		CNDSize_t& operator *=(const CNDSize_t &x) { (*this).assert_initialized(); m_val *= x.m_val; return (*this); }
		CNDSize_t& operator /=(const CNDSize_t &x) { (*this).assert_initialized(); m_val /= x.m_val; return (*this); }
		CNDSize_t& operator %=(const CNDSize_t &x) { (*this).assert_initialized(); m_val %= x.m_val; return (*this); }
		CNDSize_t& operator >>=(const CNDSize_t &x) { (*this).assert_initialized(); m_val >>= x.m_val; return (*this); }
		CNDSize_t& operator <<=(const CNDSize_t &x) { (*this).assert_initialized(); m_val <<= x.m_val; return (*this); }

		CNDSize_t operator +(const CNDSize_t &x) const { (*this).assert_initialized(); return (m_val + x.m_val); }
		CNDSize_t operator +(size_t x) const { (*this).assert_initialized(); return ((*this) + CNDSize_t(x)); }
		template<typename _Ty2>
		auto operator +(const TInt<_Ty2> &x) const->MSE_TINT_ADD_RESULT_TYPE1(CNDSize_t, TInt<_Ty2>) { (*this).assert_initialized(); return (MSE_TINT_ADD_RESULT_TYPE1(CNDSize_t, TInt<_Ty2>)((*this).m_val) + MSE_TINT_ADD_RESULT_TYPE1(CNDSize_t, TInt<_Ty2>)(x.m_val)); }
		template<typename _Ty2, class = typename std::enable_if<std::is_integral<_Ty2>::value, void>::type>
		auto operator +(_Ty2 x) const { (*this).assert_initialized(); return ((*this) + TInt<_Ty2>(x)); }

		CNDSignedSize_t operator -(const CNDSize_t &x) const { (*this).assert_initialized(); return (CNDSignedSize_t(m_val) - CNDSignedSize_t(x.m_val)); }
		CNDSignedSize_t operator -(const CNDInt &x) const { (*this).assert_initialized(); return (CNDSignedSize_t(m_val) - x); }
		CNDSignedSize_t operator -(size_t x) const { (*this).assert_initialized(); return ((*this) - CNDSize_t(x)); }
		template<typename _Ty2>
		auto operator -(const TInt<_Ty2> &x) const->MSE_TINT_SUBTRACT_RESULT_TYPE1(CNDSize_t, TInt<_Ty2>) { (*this).assert_initialized(); return (MSE_TINT_SUBTRACT_RESULT_TYPE1(CNDSize_t, TInt<_Ty2>)((*this).m_val) - MSE_TINT_SUBTRACT_RESULT_TYPE1(CNDSize_t, TInt<_Ty2>)(x.m_val)); }
		template<typename _Ty2, class = typename std::enable_if<std::is_integral<_Ty2>::value, void>::type>
		auto operator -(_Ty2 x) const { (*this).assert_initialized(); return ((*this) - TInt<_Ty2>(x)); }

		CNDSize_t operator *(const CNDSize_t &x) const { (*this).assert_initialized(); return (m_val * x.m_val); }
		CNDSignedSize_t operator *(const CNDInt &x) const { (*this).assert_initialized(); return (CNDSignedSize_t(m_val) * x); }
		CNDSize_t operator *(size_t x) const { (*this).assert_initialized(); return ((*this) * CNDSize_t(x)); }
		template<typename _Ty2>
		auto operator *(const TInt<_Ty2> &x) const->MSE_TINT_MULTIPLY_RESULT_TYPE1(CNDSize_t, TInt<_Ty2>) { (*this).assert_initialized(); return (MSE_TINT_MULTIPLY_RESULT_TYPE1(CNDSize_t, TInt<_Ty2>)((*this).m_val) * MSE_TINT_MULTIPLY_RESULT_TYPE1(CNDSize_t, TInt<_Ty2>)(x.m_val)); }
		template<typename _Ty2, class = typename std::enable_if<std::is_integral<_Ty2>::value, void>::type>
		auto operator *(_Ty2 x) const { (*this).assert_initialized(); return ((*this) * TInt<_Ty2>(x)); }

		CNDSize_t operator /(const CNDSize_t &x) const {
			if (x.m_val == 0) { MSE_THROW(std::domain_error("attempted division by zero - CNDSize_t")); }
			(*this).assert_initialized(); return (m_val / x.m_val);
		}
		CNDSignedSize_t operator /(const CNDInt &x) const { (*this).assert_initialized(); return (CNDSignedSize_t(m_val) / x); }
		CNDSize_t operator /(size_t x) const { (*this).assert_initialized(); return ((*this) / CNDSize_t(x)); }
		template<typename _Ty2>
		auto operator /(const TInt<_Ty2> &x) const->MSE_TINT_DIVIDE_RESULT_TYPE1(CNDSize_t, TInt<_Ty2>) { (*this).assert_initialized(); return (MSE_TINT_DIVIDE_RESULT_TYPE1(CNDSize_t, TInt<_Ty2>)((*this).m_val) / MSE_TINT_DIVIDE_RESULT_TYPE1(CNDSize_t, TInt<_Ty2>)(x.m_val)); }
		template<typename _Ty2, class = typename std::enable_if<std::is_integral<_Ty2>::value, void>::type>
		auto operator /(_Ty2 x) const { (*this).assert_initialized(); return ((*this) / TInt<_Ty2>(x)); }

		bool operator <(const CNDSize_t &x) const { (*this).assert_initialized(); return (m_val < x.m_val); }
		bool operator <(const CNDInt &x) const { (*this).assert_initialized(); return (CNDSignedSize_t(m_val) < x); }
		bool operator <(size_t x) const { (*this).assert_initialized(); return ((*this) < CNDSize_t(x)); }
		template<typename _Ty2>
		bool operator <(const TInt<_Ty2> &x) const { (*this).assert_initialized(); return (MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(CNDSize_t, TInt<_Ty2>)((*this).m_val) < MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(CNDSize_t, TInt<_Ty2>)(x.m_val)); }
		template<typename _Ty2, class = typename std::enable_if<std::is_integral<_Ty2>::value, void>::type>
		bool operator <(_Ty2 x) const { (*this).assert_initialized(); return ((*this) < TInt<_Ty2>(x)); }

		bool operator >(const CNDSize_t &x) const { (*this).assert_initialized(); return (m_val > x.m_val); }
		bool operator >(const CNDInt &x) const { (*this).assert_initialized(); return (CNDSignedSize_t(m_val) > x); }
		bool operator >(size_t x) const { (*this).assert_initialized(); return ((*this) > CNDSize_t(x)); }
		template<typename _Ty2>
		bool operator >(const TInt<_Ty2> &x) const { (*this).assert_initialized(); return (MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(CNDSize_t, TInt<_Ty2>)((*this).m_val) > MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(CNDSize_t, TInt<_Ty2>)(x.m_val)); }
		template<typename _Ty2, class = typename std::enable_if<std::is_integral<_Ty2>::value, void>::type>
		bool operator >(_Ty2 x) const { (*this).assert_initialized(); return ((*this) > TInt<_Ty2>(x)); }

		bool operator <=(const CNDSize_t &x) const { (*this).assert_initialized(); return (m_val <= x.m_val); }
		bool operator <=(const CNDInt &x) const { (*this).assert_initialized(); return (CNDSignedSize_t(m_val) <= x); }
		bool operator <=(size_t x) const { (*this).assert_initialized(); return ((*this) <= CNDSize_t(x)); }
		template<typename _Ty2>
		bool operator <=(const TInt<_Ty2> &x) const { (*this).assert_initialized(); return (MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(CNDSize_t, TInt<_Ty2>)((*this).m_val) <= MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(CNDSize_t, TInt<_Ty2>)(x.m_val)); }
		template<typename _Ty2, class = typename std::enable_if<std::is_integral<_Ty2>::value, void>::type>
		bool operator <=(_Ty2 x) const { (*this).assert_initialized(); return ((*this) <= TInt<_Ty2>(x)); }

		bool operator >=(const CNDSize_t &x) const { (*this).assert_initialized(); return (m_val >= x.m_val); }
		bool operator >=(const CNDInt &x) const { (*this).assert_initialized(); return (CNDSignedSize_t(m_val) >= x); }
		bool operator >=(size_t x) const { (*this).assert_initialized(); return ((*this) >= CNDSize_t(x)); }
		template<typename _Ty2>
		bool operator >=(const TInt<_Ty2> &x) const { (*this).assert_initialized(); return (MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(CNDSize_t, TInt<_Ty2>)((*this).m_val) >= MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(CNDSize_t, TInt<_Ty2>)(x.m_val)); }
		template<typename _Ty2, class = typename std::enable_if<std::is_integral<_Ty2>::value, void>::type>
		bool operator >=(_Ty2 x) const { (*this).assert_initialized(); return ((*this) >= TInt<_Ty2>(x)); }

		bool operator ==(const CNDSize_t &x) const { (*this).assert_initialized(); return (m_val == x.m_val); }
		bool operator ==(const CNDInt &x) const { (*this).assert_initialized(); return (CNDSignedSize_t(m_val) == x); }
		bool operator ==(size_t x) const { (*this).assert_initialized(); return ((*this) == CNDSize_t(x)); }
		template<typename _Ty2>
		bool operator ==(const TInt<_Ty2> &x) const { (*this).assert_initialized(); return (MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(CNDSize_t, TInt<_Ty2>)((*this).m_val) == MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(CNDSize_t, TInt<_Ty2>)(x.m_val)); }
		template<typename _Ty2, class = typename std::enable_if<std::is_integral<_Ty2>::value, void>::type>
		bool operator ==(_Ty2 x) const { (*this).assert_initialized(); return ((*this) == TInt<_Ty2>(x)); }

		bool operator !=(const CNDSize_t &x) const { (*this).assert_initialized(); return (m_val != x.m_val); }
		bool operator !=(const CNDInt &x) const { (*this).assert_initialized(); return (CNDSignedSize_t(m_val) != x); }
		bool operator !=(size_t x) const { (*this).assert_initialized(); return ((*this) != CNDSize_t(x)); }
		template<typename _Ty2>
		bool operator !=(const TInt<_Ty2> &x) const { (*this).assert_initialized(); return (MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(CNDSize_t, TInt<_Ty2>)((*this).m_val) != MSE_RANGE_ENCOMPASSING_NATIVE_INT_TYPE(CNDSize_t, TInt<_Ty2>)(x.m_val)); }
		template<typename _Ty2, class = typename std::enable_if<std::is_integral<_Ty2>::value, void>::type>
		bool operator !=(_Ty2 x) const { (*this).assert_initialized(); return ((*this) != TInt<_Ty2>(x)); }

		// INCREMENT/DECREMENT OPERATORS
		CNDSize_t& operator ++() { (*this).assert_initialized(); m_val++; return (*this); }
		CNDSize_t operator ++(int) { (*this).assert_initialized();
			CNDSize_t tmp(*this); // copy
			operator++(); // pre-increment
			return tmp;   // return old value
		}
		CNDSize_t& operator --() { (*this).assert_initialized();
		(*this).assert_initialized();
		if (0 <= std::numeric_limits<base_int_type>::lowest()) {
				(*this) = (*this) - 1; return (*this);
			}
			else {
				m_val--; return (*this);
			}
		}
		CNDSize_t operator --(int) { (*this).assert_initialized();
			CNDSize_t tmp(*this); // copy
			operator--(); // pre-decrement
			return tmp;   // return old value
		}

		void async_shareable_and_passable_tag() const {}

	private:
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		//base_int_type m_val;

		friend size_t as_a_size_t(CNDSize_t n);
	};
	size_t as_a_size_t(CNDSize_t n) { n.assert_initialized(); return n.m_val; }
}

namespace std {
#ifndef _THROW0
#define _THROW0()
#endif // !_THROW0
#ifndef _STCONS
#define _STCONS(ty, name, val)	static constexpr ty name = (ty)(val)
#endif // !_STCONS

	template<> class numeric_limits<mse::CNDSize_t> {	// limits for type int
	public:
		typedef size_t base_int_type;

		static constexpr base_int_type(min)() _THROW0()
		{	// return minimum value
			return numeric_limits<size_t>::min();
		}
		static constexpr base_int_type(max)() _THROW0()
		{	// return maximum value
			return numeric_limits<size_t>::max();
		}
		static constexpr base_int_type lowest() _THROW0()
		{	// return most negative value
			return numeric_limits<size_t>::lowest();
		}
		static constexpr base_int_type epsilon() _THROW0()
		{	// return smallest effective increment from 1.0
			return numeric_limits<size_t>::epsilon();
		}
		static constexpr base_int_type round_error() _THROW0()
		{	// return largest rounding error
			return numeric_limits<size_t>::round_error();
		}
		static constexpr base_int_type denorm_min() _THROW0()
		{	// return minimum denormalized value
			return numeric_limits<size_t>::denorm_min();
		}
		static constexpr base_int_type infinity() _THROW0()
		{	// return positive infinity
			return numeric_limits<size_t>::infinity();
		}
		static constexpr base_int_type quiet_NaN() _THROW0()
		{	// return non-signaling NaN
			return numeric_limits<size_t>::quiet_NaN();
		}
		static constexpr base_int_type signaling_NaN() _THROW0()
		{	// return signaling NaN
			return numeric_limits<size_t>::signaling_NaN();
		}
		_STCONS(float_denorm_style, has_denorm, numeric_limits<size_t>::has_denorm);
		_STCONS(bool, has_denorm_loss, numeric_limits<size_t>::has_denorm_loss);
		_STCONS(bool, has_infinity, numeric_limits<size_t>::has_infinity);
		_STCONS(bool, has_quiet_NaN, numeric_limits<size_t>::has_quiet_NaN);
		_STCONS(bool, has_signaling_NaN, numeric_limits<size_t>::has_signaling_NaN);
		_STCONS(bool, is_bounded, numeric_limits<size_t>::is_bounded);
		_STCONS(bool, is_exact, numeric_limits<size_t>::is_exact);
		_STCONS(bool, is_iec559, numeric_limits<size_t>::is_iec559);
		_STCONS(bool, is_integer, numeric_limits<size_t>::is_integer);
		_STCONS(bool, is_modulo, numeric_limits<size_t>::is_modulo);
		_STCONS(bool, is_signed, numeric_limits<size_t>::is_signed);
		_STCONS(bool, is_specialized, numeric_limits<size_t>::is_specialized);
		_STCONS(bool, tinyness_before, numeric_limits<size_t>::tinyness_before);
		_STCONS(bool, traps, numeric_limits<size_t>::traps);
		_STCONS(float_round_style, round_style, numeric_limits<size_t>::round_style);
		_STCONS(int, digits, numeric_limits<size_t>::digits);
		_STCONS(int, digits10, numeric_limits<size_t>::digits10);
		_STCONS(int, max_digits10, numeric_limits<size_t>::max_digits10);
		_STCONS(int, max_exponent, numeric_limits<size_t>::max_exponent);
		_STCONS(int, max_exponent10, numeric_limits<size_t>::max_exponent10);
		_STCONS(int, min_exponent, numeric_limits<size_t>::min_exponent);
		_STCONS(int, min_exponent10, numeric_limits<size_t>::min_exponent10);
		_STCONS(int, radix, numeric_limits<size_t>::radix);
	};

	template<>
	struct hash<mse::CNDSize_t> {	// hash functor
		typedef mse::CNDSize_t argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::CNDSize_t& _Keyval) const _NOEXCEPT {
			return (hash<mse::CNDSize_t::base_int_type>()(mse::as_a_size_t(_Keyval)));
		}
	};
}

namespace mse {

	inline bool operator==(bool lhs, CNDBool rhs) { rhs.assert_initialized(); return CNDBool(lhs) == rhs; }
	inline bool operator!=(bool lhs, CNDBool rhs) { rhs.assert_initialized(); return CNDBool(lhs) != rhs; }

	template<typename _Ty, typename _Tz, class = typename std::enable_if<(std::is_arithmetic<_Tz>::value), void>::type>
	inline auto operator+(_Tz lhs, const TInt<_Ty> &rhs) { return rhs + lhs; }
	template<typename _Tz, class = typename std::enable_if<(std::is_arithmetic<_Tz>::value), void>::type>
	inline auto operator+(_Tz lhs, const CNDSize_t &rhs) { return rhs + lhs; }

	template<typename _Ty, typename _Tz, class = typename std::enable_if<(std::is_arithmetic<_Tz>::value), void>::type>
	inline auto operator-(_Tz lhs, const TInt<_Ty> &rhs) { return -(rhs - lhs); }
	template<typename _Tz, class = typename std::enable_if<(std::is_arithmetic<_Tz>::value), void>::type>
	inline auto operator-(_Tz lhs, const CNDSize_t &rhs) { return lhs - CNDInt(rhs); }

	template<typename _Ty, typename _Tz, class = typename std::enable_if<(std::is_arithmetic<_Tz>::value), void>::type>
	inline auto operator*(_Tz lhs, const TInt<_Ty> &rhs) { return rhs * lhs; }
	template<typename _Tz, class = typename std::enable_if<(std::is_arithmetic<_Tz>::value), void>::type>
	inline auto operator*(_Tz lhs, const CNDSize_t &rhs) { return rhs * lhs; }

	template<typename _Ty, typename _Tz, class = typename std::enable_if<(std::is_arithmetic<_Tz>::value), void>::type>
	inline auto operator/(_Tz lhs, const TInt<_Ty> &rhs) { return CNDInt(lhs) / rhs; }
	template<typename _Tz, class = typename std::enable_if<(std::is_arithmetic<_Tz>::value), void>::type>
	inline auto operator/(_Tz lhs, const CNDSize_t &rhs) { return CNDInt(lhs) / rhs; }

	template<typename _Ty, typename _Tz, class = typename std::enable_if<(std::is_arithmetic<_Tz>::value), void>::type>
	inline auto operator<(_Tz lhs, const TInt<_Ty> &rhs) { return rhs > lhs; }
	template<typename _Tz, class = typename std::enable_if<(std::is_arithmetic<_Tz>::value), void>::type>
	inline auto operator<(_Tz lhs, const CNDSize_t &rhs) { return rhs > lhs; }

	template<typename _Ty, typename _Tz, class = typename std::enable_if<(std::is_arithmetic<_Tz>::value), void>::type>
	inline auto operator>(_Tz lhs, const TInt<_Ty> &rhs) { return rhs < lhs; }
	template<typename _Tz, class = typename std::enable_if<(std::is_arithmetic<_Tz>::value), void>::type>
	inline auto operator>(_Tz lhs, const CNDSize_t &rhs) { return rhs < lhs; }

	template<typename _Ty, typename _Tz, class = typename std::enable_if<(std::is_arithmetic<_Tz>::value), void>::type>
	inline auto operator<=(_Tz lhs, const TInt<_Ty> &rhs) { return rhs >= lhs; }
	template<typename _Tz, class = typename std::enable_if<(std::is_arithmetic<_Tz>::value), void>::type>
	inline auto operator<=(_Tz lhs, const CNDSize_t &rhs) { return rhs >= lhs; }

	template<typename _Ty, typename _Tz, class = typename std::enable_if<(std::is_arithmetic<_Tz>::value), void>::type>
	inline auto operator>=(_Tz lhs, const TInt<_Ty> &rhs) { return rhs <= lhs; }
	template<typename _Tz, class = typename std::enable_if<(std::is_arithmetic<_Tz>::value), void>::type>
	inline auto operator>=(_Tz lhs, const CNDSize_t &rhs) { return rhs <= lhs; }

	template<typename _Ty, typename _Tz, class = typename std::enable_if<(std::is_arithmetic<_Tz>::value), void>::type>
	inline auto operator==(_Tz lhs, const TInt<_Ty> &rhs) { return rhs == lhs; }
	template<typename _Tz, class = typename std::enable_if<(std::is_arithmetic<_Tz>::value), void>::type>
	inline auto operator==(_Tz lhs, const CNDSize_t &rhs) { return rhs == lhs; }

	template<typename _Ty, typename _Tz, class = typename std::enable_if<(std::is_arithmetic<_Tz>::value), void>::type>
	inline auto operator!=(_Tz lhs, const TInt<_Ty> &rhs) { return rhs != lhs; }
	template<typename _Tz, class = typename std::enable_if<(std::is_arithmetic<_Tz>::value), void>::type>
	inline auto operator!=(_Tz lhs, const CNDSize_t &rhs) { return rhs != lhs; }

#ifdef MSE_PRIMITIVES_DISABLED
	typedef bool CBool;
	typedef MSE_CINT_BASE_INTEGER_TYPE CInt;
	typedef size_t CSize_t;
	static size_t as_a_size_t(CSize_t n) { return (n); }
#else /*MSE_PRIMITIVES_DISABLED*/

	typedef CNDBool CBool;
	typedef CNDInt CInt;
	typedef CNDSize_t CSize_t;

#endif /*MSE_PRIMITIVES_DISABLED*/


	namespace us {
		namespace impl {
			namespace rrt {

				template<typename _Ty>
				_Ty& force_lvalue_ref(_Ty& x) { return x; }
				template<typename _Ty>
				_Ty& force_lvalue_ref(_Ty&& x) { return force_lvalue_ref(x); }

				template<class T, class EqualTo>
				struct HasOrInheritsMseBaseTypeRefMethod_impl
				{
					template<class U, class V>
					static auto test(U*) -> decltype(force_lvalue_ref(std::declval<U>()).mse_base_type_ref(), force_lvalue_ref(std::declval<V>()).mse_base_type_ref(), bool(true));
					template<typename, typename>
					static auto test(...)->std::false_type;

					static const bool value = std::is_same<bool, decltype(test<T, EqualTo>(0))>::value;
					using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
				};
				template<class T, class EqualTo = T>
				struct HasOrInheritsMseBaseTypeRefMethod : HasOrInheritsMseBaseTypeRefMethod_impl<
					typename std::remove_reference<T>::type, typename std::remove_reference<EqualTo>::type>::type {};

				template<typename _Ty, typename _Tz>
				_Ty& raw_reference_to_helper3(std::true_type, _Tz& x) {
					return x.mse_base_type_ref();
				}
				template<typename _Ty, typename _Tz>
				_Ty& raw_reference_to_helper3(std::false_type, _Tz& x) {
					_Tz* ptr1 = std::addressof(x);
					/* A compile error here can occur if, for example, if you try to assign a pointer to a 'const int' to a
					'TXScopeAnyConstPointer<unsigned int>'. 'int' and 'unsigned int' are "incompatible" types in this situation. */
					_Ty* ptr2 = ptr1;
					return *ptr2;
				}

				template<typename _Ty, typename _Tz>
				_Ty& raw_reference_to_helper2(std::true_type, _Tz& x) {
					return raw_reference_to_helper3<_Ty>(typename std::is_convertible<typename std::remove_reference<decltype(force_lvalue_ref(std::declval<_Tz>()).mse_base_type_ref())>::type*, _Ty*>::type(), x);
				}
				template<typename _Ty, typename _Tz>
				_Ty& raw_reference_to_helper2(std::false_type, _Tz& x) { return raw_reference_to_helper3<_Ty>(std::false_type(), x); }

				template<typename _Ty, typename _Tz>
				_Ty& raw_reference_to_helper1(std::true_type, _Tz& x) { return raw_reference_to_helper3<_Ty>(std::false_type(), x); }
				template<typename _Ty, typename _Tz>
				_Ty& raw_reference_to_helper1(std::false_type, _Tz& x) {
					return raw_reference_to_helper2<_Ty>(typename HasOrInheritsMseBaseTypeRefMethod<_Tz>::type(), x);
				}
			}

			/* This function returns a raw reference of the specified type to the given object (which may be of a different type).
			In particular, it can be used to obtain a 'const int&' to an mse::TInt<int>. */
			template<typename _Ty, typename _Tz>
			_Ty& raw_reference_to(_Tz& x) {
				return rrt::raw_reference_to_helper1<_Ty>(typename std::is_convertible<_Tz*, _Ty*>::type(), x);
			}

			/* for now, we don't need this one */
			template<typename _Ty, typename _Tz>
			_Ty& raw_reference_to(_Tz&& x) = delete;
		}
	}


#if LLONG_MAX != LONG_MAX
#define MSE_IMPL_APPLY_MACRO_FUNCTION_TO_LONG_LONG_INTEGER_TYPES(MACRO_FUNCTION) \
	MACRO_FUNCTION(long long int) \
	MACRO_FUNCTION(unsigned long long int)
#else // LLONG_MAX != LONG_MAX
#define MSE_IMPL_APPLY_MACRO_FUNCTION_TO_LONG_LONG_INTEGER_TYPES(MACRO_FUNCTION)
#endif // LLONG_MAX != LONG_MAX

#if LONG_MAX != INT_MAX
#define MSE_IMPL_APPLY_MACRO_FUNCTION_TO_LONG_INTEGER_TYPES(MACRO_FUNCTION) \
	MACRO_FUNCTION(long int) \
	MACRO_FUNCTION(unsigned long int)
#else // LONG_MAX != INT_MAX
#define MSE_IMPL_APPLY_MACRO_FUNCTION_TO_LONG_INTEGER_TYPES(MACRO_FUNCTION)
#endif // LONG_MAX != INT_MAX

#if INT_MAX != SHRT_MAX
#define MSE_IMPL_APPLY_MACRO_FUNCTION_TO_INTEGER_TYPES(MACRO_FUNCTION) \
	MACRO_FUNCTION(int) \
	MACRO_FUNCTION(unsigned int)
#else // INT_MAX != SHRT_MAX
#define MSE_IMPL_APPLY_MACRO_FUNCTION_TO_INTEGER_TYPES(MACRO_FUNCTION)
#endif // INT_MAX != SHRT_MAX

#if SHRT_MAX != CHAR_MAX
#define MSE_IMPL_APPLY_MACRO_FUNCTION_TO_SHORT_INTEGER_TYPES(MACRO_FUNCTION) \
	MACRO_FUNCTION(short int) \
	MACRO_FUNCTION(unsigned short int)
#else // SHRT_MAX != CHAR_MAX
#define MSE_IMPL_APPLY_MACRO_FUNCTION_TO_SHORT_INTEGER_TYPES(MACRO_FUNCTION)
#endif // SHRT_MAX != CHAR_MAX

#define MSE_IMPL_APPLY_MACRO_FUNCTION_TO_CHAR_TYPES(MACRO_FUNCTION) \
	MACRO_FUNCTION(char) \
	MACRO_FUNCTION(unsigned char) \
	MACRO_FUNCTION(signed char)

#define MSE_IMPL_APPLY_MACRO_FUNCTION_TO_EACH_OF_THE_INTEGER_TYPES(MACRO_FUNCTION) \
	MSE_IMPL_APPLY_MACRO_FUNCTION_TO_LONG_LONG_INTEGER_TYPES(MACRO_FUNCTION) \
	MSE_IMPL_APPLY_MACRO_FUNCTION_TO_LONG_INTEGER_TYPES(MACRO_FUNCTION) \
	MSE_IMPL_APPLY_MACRO_FUNCTION_TO_INTEGER_TYPES(MACRO_FUNCTION) \
	MSE_IMPL_APPLY_MACRO_FUNCTION_TO_SHORT_INTEGER_TYPES(MACRO_FUNCTION) \
	MSE_IMPL_APPLY_MACRO_FUNCTION_TO_CHAR_TYPES(MACRO_FUNCTION)


	namespace self_test {
		class CPrimitivesTest1 {
		public:
			static void s_test1() {
#ifdef MSE_SELF_TESTS
				CInt i1(3);
				CInt i2 = 5;
				CInt i3;
				i3 = 7;
				CInt i4 = i1 + i2;
				i4 = i1 + 17;
				i4 = 19 + i1;
				i4 += i2;
				i4 -= 23;
				i4++;
				i4 = i3 - i1;
				i4 -= i1;
				i4 *= i1;
				i4 = i1 * i2;
				i4 /= i1;
				i4 = i1 * i2 / i3;
				CBool b1 = (i1 < i2);
				b1 = (i1 < 17);
				b1 = (19 < i1);
				b1 = (i1 == i2);
				b1 = (i1 == 17);
				b1 = (19 == i1);

				CSize_t szt1(3);
				CSize_t szt2 = 5U;
				CSize_t szt3;
				szt3 = 7;
				CSize_t szt4 = szt1 + szt2;
				szt4 = szt1 + 17;
				szt4 = 19 + szt1;
				szt4 += szt2;
				szt4 -= 23;
				szt4++;
#ifndef MSVC2010_COMPATIBLE
				size_t szt5 = size_t(szt4);
#endif /*MSVC2010_COMPATIBLE*/
				szt4 = szt3 - szt1;
				szt4 *= szt1;
				szt4 = szt1 * szt2;
				szt4 /= szt1;
				szt4 = szt1 * szt2 / szt3;
#ifndef MSEPRIMITIVES_H
				CInt i11 = 19 + szt1;
				CInt i12 = szt1 * i11;
				i12 = szt1;
				i12 = szt3 - szt1;
				i12 = szt3 - i11;
#else // !MSEPRIMITIVES_H
				CInt i11 = 19 + CInt(szt1);
				CInt i12 = CInt(szt1) * i11;
				i12 = CInt(szt1);
				i12 = CInt(szt3 - szt1);
				i12 = CInt(szt3) - i11;
#endif // !MSEPRIMITIVES_H
				bool b3 = (szt1 < szt2);
				b3 = (szt1 < 17);
				b3 = (19 < szt1);
				CBool b2 = (19 < szt1);
				b3 = (szt1 == szt2);
				b3 = (szt1 == 17);
				b3 = (19 == szt1);
				CBool b4 = (b1 == b2);
				b4 = (b3 == b1);
				b4 = (b1 && b2);
				b4 = (b1 || b3);
				b4 = (b3 && b1);
				b4 |= b1;
				b4 &= b3;
#endif // MSE_SELF_TESTS
			}
		};
	}

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
#pragma pop_macro("_NOEXCEPT")
#endif // !MSE_PUSH_MACRO_NOT_SUPPORTED

#ifdef _MSC_VER
#pragma warning( pop )  
#endif /*_MSC_VER*/

#ifndef MSEPOINTERBASICS_H
#undef MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION
#endif /*ndef MSEPOINTERBASICS_H*/

#endif /*ndef MSEPRIMITIVES_H*/
