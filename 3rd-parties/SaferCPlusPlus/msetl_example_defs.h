
// Copyright (c) 2015 Noah Lopez
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef MSETL_EXAMPLE_DEFS_H
#define MSETL_EXAMPLE_DEFS_H

/* Following are compile-time directives that control the behavior of the library. Generally, these symbols should be
consistently defined (or not) in all modules in the project. And generally, these symbols would be defined via build
options rather than "#define" directives in the code such as the ones here (for demonstration/documentation purposes). */

//#define MSE_SAFER_SUBSTITUTES_DISABLED /* This will replace all the classes with their native/standard counterparts. */

/* Each of the following will replace a subset of the classes with their native/standard counterparts. */
//#define MSE_MSTDVECTOR_DISABLED
//#define MSE_REGISTEREDPOINTER_DISABLED
//#define MSE_SAFERPTR_DISABLED /* MSE_SAFERPTR_DISABLED implies MSE_REGISTEREDPOINTER_DISABLED too. */
//#define MSE_PRIMITIVES_DISABLED
//#define MSE_REFCOUNTINGPOINTER_DISABLED
//#define MSE_SCOPEPOINTER_DISABLED
//#define MSE_NORADPOINTER_DISABLED

/* Part of the point of the library is to be able to avoid using raw pointers. However, the library does acknowledge
raw pointers, and by default, considers them (non-owning) scope pointers and imposes the corresponding restrictions on
their use. The following suppresses (a lot of) those restrictions allowing a little more flexibility in how raw
pointers can be used with some of the library elements. */
//#define MSE_DISABLE_RAW_POINTER_SCOPE_RESTRICTIONS

/* The following will result in program termination instead of exceptions being thrown when an invalid memory access is attempted. */
//#include <iostream>
//#define MSE_CUSTOM_THROW_DEFINITION(x) std::cerr << std::endl << x.what(); exit(-11)

/* The following adds run-time checks to scope pointers in non-debug mode. */
//#define MSE_SCOPEPOINTER_RUNTIME_CHECKS_ENABLED
/* The following removes run-time checks from scope pointers in debug mode. */
//#define MSE_SCOPEPOINTER_DEBUG_RUNTIME_CHECKS_DISABLED

/* The following directs the library's vectors and arrays to use CSize_t and CInt in place of size_t and int in their
interface and implementation. This adds a marginal increase in safety. (Mostly due to the interface.) */
//#define MSE_MSEVECTOR_USE_MSE_PRIMITIVES 1
//#define MSE_MSEARRAY_USE_MSE_PRIMITIVES 1

/* The following enables run-time checks for inappropriate reentrancy in the non-static member functions of the
library's container classes in release builds. (They are automatically enabled in debug builds.) (And only
necessary in the presence of potentially misbehaving user-defined constructors/destructors.) */
//#define MSE_ENABLE_REENTRANCY_CHECKS_BY_DEFAULT

/* The following causes arithmetic operations involving the library's integer substitutes to return, if available, a
type larger than the operand type(s). So for example, if the size of the native int (that CInt is based on) is 4
bytes and the native long long type is 8 bytes, then the result of multiplying two CInts would be a TInt<long long>.
Assigning that result back to a CInt would result in a run-time range check. (Effectively an overflow check for the
operation.) */
//#define MSE_RETURN_RANGE_EXTENDED_TYPE_FOR_INTEGER_ARITHMETIC

/* The conversion from CSize_t to CInt, like the conversion from size_t to int, is a common one that theoretically
requires a range check. The following will cause the range check for that particular conversion to be suppressed.
Empirically speaking, both the risks and rewards of doing so seem to be minor, but of course it's situation
dependent. */
//#define MSE_SUPPRESS_CSIZE_T_TO_CINT_CONVERSION_RANGE_CHECK

/* msvc2015's incomplete support for "constexpr" means that range checks that should be done at compile time would
be done at run time, at significant cost. So they are disabled by default for that compiler. The following "forces"
them to be enabled. */
#ifdef _MSC_VER
#if (1910 > _MSC_VER)
/* The compiler is msvc2015 or earlier. */
#define MSE_FORCE_PRIMITIVE_ASSIGN_RANGE_CHECK_ENABLED
#endif /*(1910 > _MSC_VER)*/
#endif /*_MSC_VER*/

#define MSE_SELF_TESTS

#endif /*ndef MSETL_EXAMPLE_DEFS_H*/
