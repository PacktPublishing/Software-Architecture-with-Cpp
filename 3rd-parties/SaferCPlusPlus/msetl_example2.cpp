
// Copyright (c) 2015 Noah Lopez
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef EXCLUDE_MSETL_EXAMPLE2

#include "msetl_example_defs.h"

#include "msetl_example2.h"
#include "mseregistered.h"
#include "msecregistered.h"
#include "msescope.h"
#include "msepoly.h"
#include "msemsearray.h"
#include "msemstdarray.h"
#include "msemsevector.h"
#include "msemstdvector.h"
#include "msemsestring.h"
#include "msemstdstring.h"
#include "mselegacyhelpers.h"
#include "msealgorithm.h"
#include "msethreadlocal.h"
#include "msestaticimmutable.h"
#include "mseoptional.h"
#include "msetuple.h"
#include "msefunctional.h"

#include <iostream>
#include <sstream>


#ifdef _MSC_VER
#pragma warning( push )  
#pragma warning( disable : 4100 4456 4189 4702 )
#endif /*_MSC_VER*/

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#pragma clang diagnostic ignored "-Wtautological-compare"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wunused-local-typedefs"
//pragma clang diagnostic ignored "-Wunused-but-set-variable"
#else /*__clang__*/
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif /*__GNUC__*/
#endif /*__clang__*/

class J {
public:
	template<typename _TParam>
	static auto foo10(_TParam param) {
		_TParam l_obj = param;

		auto ret_ptr = &l_obj;
		/* ret_ptr may be a smart pointer if the 'operator&' is overloaded.*/
		/* If ret_ptr is a scope pointer (or native pointer) it would be unsafe to return it. In those cases the
		 'mse::return_value(ret_ptr)' expression will induce a compile error. */
		/* Functions that could return a scope type need to wrap their return value with the return_value() function. */
		return mse::return_value(ret_ptr);
	}

	template<class _TRASection>
	static void foo13(_TRASection ra_section) {
		for (typename _TRASection::size_type i = 0; i < ra_section.size(); i += 1) {
			ra_section[i] = 0;
		}
	}
	template<class _TRAConstSection>
	static int foo14(_TRAConstSection const_ra_section) {
		int retval = 0;
		for (typename _TRAConstSection::size_type i = 0; i < const_ra_section.size(); i += 1) {
			retval += const_ra_section[i];
		}
		return retval;
	}
	template<class _TRAConstSection>
	static int foo15(_TRAConstSection const_ra_section) {
		int retval = 0;
		for (const auto& const_item : const_ra_section) {
			retval += const_item;
		}
		return retval;
	}

	/* This function will be used to demonstrate using rsv::as_an_fparam() to enable template functions to accept scope
	random access sections that reference temporary objects. */
	template<class _TRASection1, class _TRASection2>
	static bool second_is_longer(const _TRASection1& xscope_ra_csection1, const _TRASection2& xscope_ra_csection2) {
		auto l_xscope_ra_csection1 = mse::rsv::as_an_fparam(xscope_ra_csection1);
		auto l_xscope_ra_csection2 = mse::rsv::as_an_fparam(xscope_ra_csection2);
		return (l_xscope_ra_csection1.size() > l_xscope_ra_csection2.size()) ? false : true;
	}

#ifdef MSE_HAS_CXX17
	/* While not encouraging their use, this is how you might declare an immutable static member (in C++17 and later). */
	inline MSE_DECLARE_STATIC_IMMUTABLE(mse::nii_string) sm_simm_string = "abc";
#endif // MSE_HAS_CXX17

};

MSE_DECLARE_THREAD_LOCAL_GLOBAL(mse::mstd::string) tlg_string1 = "some text";
MSE_RSV_DECLARE_GLOBAL_IMMUTABLE(mse::nii_string) gimm_string1 = "some text";


void msetl_example2() {
	{
		/**********************/
		/*   mtnii_vector<>   */
		/**********************/

		/* mtnii_vector<> is a safe vector that is eligible to be (safely) shared between asynchronous threads. Note that
		fixed-size vectors (like xscope_borrowing_fixed_nii_vector<>) are also eligible to be shared between asynchronous
		threads and are generally preferred when suitable. */

		typedef mse::mtnii_vector<mse::nii_string> mtnii_vector1_t;

		mse::TRegisteredObj<mtnii_vector1_t> rg_vo1;
		for (size_t i = 0; i < 5; i += 1) {
			rg_vo1.push_back("some text");
		}
		mse::TRegisteredPointer<mtnii_vector1_t> vo1_regptr1 = &rg_vo1;

		/* mtnii_vector<> does not have a begin() member function that returns an "implicit" iterator. You can obtain an
		iterator using the make_begin_iterator() et al. functions, which take a (safe) pointer to the container. */
		auto iter1 = mse::make_begin_iterator(vo1_regptr1);
		auto citer1 = mse::make_end_const_iterator(vo1_regptr1);
		citer1 = iter1;
		rg_vo1.emplace(vo1_regptr1, citer1, "some other text");
		rg_vo1.insert(vo1_regptr1, citer1, "some other text");
		mse::nii_string str1 = "some other text";
		rg_vo1.insert(vo1_regptr1, citer1, str1);

		class A {
		public:
			A() {}
			int m_i = 0;
		};
		/* Here we're declaring that A can be safely shared between asynchronous threads. */
		typedef mse::rsv::TAsyncShareableAndPassableObj<A> shareable_A_t;

		/* When the element type of an mtnii_vector<> is marked as "async shareable", the mtnii_vector<> itself is
		(automatically) marked as async shareable as well and can be safely shared between asynchronous threads
		using "access requesters". */
		auto access_requester1 = mse::make_asyncsharedv2readwrite<mse::mtnii_vector<shareable_A_t>>();
		auto access_requester2 = mse::make_asyncsharedv2readwrite<mtnii_vector1_t>();

		/* If the element type of an mtnii_vector<> is not marked as "async shareable", then neither is the
		mtnii_vector<> itself. So attempting to create an "access requester" using it would result in a compile
		error. */
		//auto access_requester3 = mse::make_asyncsharedv2readwrite<mse::mtnii_vector<A>>();
		//auto access_requester4 = mse::make_asyncsharedv2readwrite<mse::mtnii_vector<mse::mstd::string>>();

		typedef mse::mstd::vector<mse::nii_string> vector1_t;
		vector1_t vo2 = { "a", "b", "c" };
		/* mstd::vector<>s, for example, are not safely shareable between threads. But if its element type is
		safely shareable, then the contents of the mse::mstd::vector<>, can be swapped with a corresponding
		shareable mtnii_vector<>. Note that vector swaps are intrinsically fast operations. */
		std::swap(vo2, *(access_requester2.writelock_ptr()));

		{
			/* If the vector is declared as a "scope" object (which basically indicates that it is declared
			on the stack), then you can use "scope" iterators. While there are limitations on when they can
			be used, scope iterators would be the preferred iterator type where performance is a priority
			as they don't require extra run-time overhead to ensure that the vector has not been prematurely
			deallocated. */

			/* Here we're declaring a vector as a scope object. */
			mse::TXScopeObj<mse::mtnii_vector<int> > vector1_xscpobj = mse::mtnii_vector<int>{ 1, 2, 3 };

			{
				/* Here we're obtaining scope iterators to the vector. */
				auto xscp_iter1 = mse::make_xscope_begin_iterator(&vector1_xscpobj);
				auto xscp_iter2 = mse::make_xscope_end_iterator(&vector1_xscpobj);

				std::sort(xscp_iter1, xscp_iter2);

				auto xscp_citer3 = mse::make_xscope_begin_const_iterator(&vector1_xscpobj);
				xscp_citer3 = xscp_iter1;
				xscp_citer3 = mse::make_xscope_begin_const_iterator(&vector1_xscpobj);
				xscp_citer3 += 2;
				auto res1 = *xscp_citer3;
				auto res2 = xscp_citer3[0];

				/* Note that scope iterators to vectors (and other dynamic containers), "lock the structure" of the container
				so that, for example, it cannot be resized. This allows us to obtain a scope pointer to the (lvalue) iterator's
				target element. */
				auto xscp_ptr1 = mse::xscope_pointer(xscp_citer3);
				auto res3 = *xscp_ptr1;
			}
			/* After all the scope pointers have gone out of scope, you may again perform operations that affect the container's
			"structure" (i.e. size or capacity). */
			vector1_xscpobj.push_back(4);
		}
	}

	{
		/**********************/
		/*   stnii_vector<>   */
		/**********************/

		/* stnii_vector<> is just a version of mtnii_vector<> that is not eligible to be shared between threads (and has a
		little less overhead as a result). Though when suitable, using nii_vector<> and xscope_borrowing_fixed_nii_vector<>
		is generally preferred. */

		mse::TXScopeObj<mse::stnii_vector<int> > vector1_xscpobj = mse::stnii_vector<int>{ 1, 2, 3 };

		{
			mse::TXScopeFixedConstPointer<mse::stnii_vector<int> > xscptr = &vector1_xscpobj;
			auto xscp_citer1 = mse::make_xscope_begin_const_iterator(xscptr);
			xscp_citer1 += 2;
			auto xscp_cptr1 = mse::xscope_const_pointer(xscp_citer1);
			auto res1 = *xscp_cptr1;
		}
		vector1_xscpobj.push_back(4);

		/* stnii_vector<>s can be (efficiently) swapped with mtnii_vector<>s. */
		auto mtniiv1 = mse::mtnii_vector<int>();
		std::swap(vector1_xscpobj, mtniiv1);
		/* Or mstd::vector<>s. */
		auto mstdv1 = mse::mstd::vector<int>();
		std::swap(vector1_xscpobj, mstdv1);
	}

	{
		/********************/
		/*   nii_vector<>   */
		/********************/

		/* nii_vector<> is another safe vector that is eligible to be (safely) shared between asynchronous threads. It
		doesn't use a (costly) thread-safe (atomic) "structure locking" mechanism like mse::mtnii_vector<> does, but
		therefore it doesn't supports "structure locking" (and therefore obtaining a scope pointer to an element) via
		its scope const iterators, only its scope non-const iterators. */

		typedef mse::nii_vector<mse::nii_string> nii_vector1_t;

		mse::TRegisteredObj<nii_vector1_t> rg_vo1;
		for (size_t i = 0; i < 5; i += 1) {
			rg_vo1.push_back("some text");
		}
		mse::TRegisteredPointer<nii_vector1_t> vo1_regptr1 = &rg_vo1;

		/* nii_vector<> does not have a begin() member function that returns an "implicit" iterator. You can obtain an
		iterator using the make_begin_iterator() et al. functions, which take a (safe) pointer to the container. */
		auto iter1 = mse::make_begin_iterator(vo1_regptr1);
		auto citer1 = mse::make_end_const_iterator(vo1_regptr1);
		citer1 = iter1;
		rg_vo1.emplace(vo1_regptr1, citer1, "some other text");
		rg_vo1.insert(vo1_regptr1, citer1, "some other text");
		mse::nii_string str1 = "some other text";
		rg_vo1.insert(vo1_regptr1, citer1, str1);

		class A {
		public:
			A() {}
			int m_i = 0;
		};
		/* Here we're declaring that A can be safely shared between asynchronous threads. */
		typedef mse::rsv::TAsyncShareableAndPassableObj<A> shareable_A_t;

		/* When the element type of an nii_vector<> is marked as "async shareable", the nii_vector<> itself is
		(automatically) marked as async shareable as well and can be safely shared between asynchronous threads
		using "access requesters". */
		auto access_requester1 = mse::make_asyncsharedv2readwrite<mse::nii_vector<shareable_A_t>>();
		auto access_requester2 = mse::make_asyncsharedv2readwrite<nii_vector1_t>();

		/* If the element type of an nii_vector<> is not marked as "async shareable", then neither is the
		nii_vector<> itself. So attempting to create an "access requester" using it would result in a compile
		error. */
		//auto access_requester3 = mse::make_asyncsharedv2readwrite<mse::nii_vector<A>>();
		//auto access_requester4 = mse::make_asyncsharedv2readwrite<mse::nii_vector<mse::mstd::string>>();

		typedef mse::mstd::vector<mse::nii_string> vector1_t;
		vector1_t vo2 = { "a", "b", "c" };
		/* mstd::vector<>s, for example, are not safely shareable between threads. But if its element type is
		safely shareable, then the contents of the mse::mstd::vector<>, can be swapped with a corresponding
		shareable nii_vector<>. Note that vector swaps are intrinsically fast operations. */
		std::swap(vo2, *(access_requester2.writelock_ptr()));

		{
			/* If the vector is declared as a "scope" object (which basically indicates that it is declared
			on the stack), then you can use "scope" iterators. While there are limitations on when they can
			be used, scope iterators would be the preferred iterator type where performance is a priority
			as they don't require extra run-time overhead to ensure that the vector has not been prematurely
			deallocated. */

			/* Here we're declaring a vector as a scope object. */
			mse::TXScopeObj<mse::nii_vector<int> > vector1_xscpobj = mse::nii_vector<int>{ 1, 2, 3 };

			{
				/* Here we're obtaining a scope iterator to the vector. */
				auto xscp_iter1 = mse::make_xscope_begin_iterator(&vector1_xscpobj);
				auto xscp_iter2 = mse::make_xscope_end_iterator(&vector1_xscpobj);

				std::sort(xscp_iter1, xscp_iter2);

				/* Note that nii_vector<>'s scope non-const iterators "lock the structure" of the container so that, for
				example, it cannot be resized. This allows us to obtain a scope pointer to the (lvalue) iterator's target
				element. */
				auto xscp_ptr1 = mse::xscope_pointer(xscp_iter1);
				auto res3 = *xscp_ptr1;

				auto xscp_citer3 = mse::make_xscope_begin_const_iterator(&vector1_xscpobj);
				xscp_citer3 = xscp_iter1;
				xscp_citer3 = mse::make_xscope_begin_const_iterator(&vector1_xscpobj);

				xscp_citer3 += 2;
				auto res1 = *xscp_citer3;
				auto res2 = xscp_citer3[0];
			}
			/* After all the scope pointers have gone out of scope, you may again perform operations that affect the container's
			"structure" (i.e. size or capacity). */
			vector1_xscpobj.push_back(4);
		}
		{
			/* Unfortunately, you cannot obtain a direct scope const pointer to an nii_vector<> element from a scope const
			pointer to the nii_vector<>. (nii_vector<> is the only one of the library's vectors that has this shortcoming.)
			However, for vectors that are access controlled with an "exclusive writer" access policy, you can use an
			"exclusive writer" const pointer to obtain a direct scope const pointer to a vector element. */
			auto xs_ew_niivec1 = mse::make_xscope(mse::make_exclusive_writer(mse::nii_vector<int>{ 1, 2, 3 }));
			// which can also be written as:
			// mse::TXScopeObj<mse::TExclusiveWriterObj<mse::nii_vector<int> > > xs_ew_niivec1 = mse::nii_vector<int>{ 1, 2, 3 };
			{
				auto xs_ew_cptr1 = mse::make_xscope_access_controlled_const_pointer(xs_ew_niivec1);
				/* The following scope iterator is of a type that reflects that it was constructed from an "exclusive writer"
				const pointer. */
				auto xs_citer1 = mse::make_xscope_begin_const_iterator(xs_ew_cptr1);
				xs_citer1 += 2;
				/* We can obtain a (zero-overhead) scope pointer from the (lvalue) iterator. */
				auto xs_cptr1 = mse::xscope_const_pointer(xs_citer1);
				auto val1 = *xs_cptr1;
			}
			/* Once the "access controlled const pointer" no longer exists, we can modify the vector again (via "access controlled 
			(non-const) pointer") */
			mse::push_back(mse::make_xscope_access_controlled_pointer(xs_ew_niivec1), 4);
		}
	}

	{
		/**************************/
		/*   fixed_nii_vector<>   */
		/**************************/

		/* A fixed_nii_vector<> is basically like an nii_array<> (i.e. not resizable) whose size is specified at
		construction (rather than at compile-time). */

		typedef mse::fixed_nii_vector<mse::nii_string> fixed_nii_vector1_t;

		mse::TRegisteredObj<fixed_nii_vector1_t> rg_vo1{ mse::nii_string{"abc"}, mse::nii_string{"def"} };
		mse::TRegisteredPointer<fixed_nii_vector1_t> vo1_regptr1 = &rg_vo1;

		/* fixed_nii_vector<> does not have a begin() member function that returns an "implicit" iterator. You can obtain an
		iterator using the make_begin_iterator() et al. functions, which take a (safe) pointer to the container. */
		auto iter1 = mse::make_begin_iterator(vo1_regptr1);
		auto citer1 = mse::make_end_const_iterator(vo1_regptr1);
		citer1 = iter1;

		/* fixed_nii_vector<>s can be constructed from other (resizeable) vector types */
		auto nvec1 = mse::nii_vector<int>{ 1, 2, 3 };
		mse::fixed_nii_vector<int> fixed_nvec1(nvec1);
		assert(fixed_nvec1.size() == nvec1.size());
		mse::fixed_nii_vector<int> fixed_nvec2(mse::mstd::vector<int>{ 1, 2 });

		class A {
		public:
			A() {}
			int m_i = 0;
		};
		/* Here we're declaring that A can be safely shared between asynchronous threads. */
		typedef mse::rsv::TAsyncShareableAndPassableObj<A> shareable_A_t;

		/* When the element type of an fixed_nii_vector<> is marked as "async shareable", the fixed_nii_vector<> itself is
		(automatically) marked as async shareable as well and can be safely shared between asynchronous threads
		using "access requesters". */
		auto access_requester1 = mse::make_asyncsharedv2readwrite<mse::fixed_nii_vector<shareable_A_t>>();
		auto access_requester2 = mse::make_asyncsharedv2readwrite<fixed_nii_vector1_t>();

		/* If the element type of an fixed_nii_vector<> is not marked as "async shareable", then neither is the
		fixed_nii_vector<> itself. So attempting to create an "access requester" using it would result in a compile
		error. */
		//auto access_requester3 = mse::make_asyncsharedv2readwrite<mse::fixed_nii_vector<A>>();
		//auto access_requester4 = mse::make_asyncsharedv2readwrite<mse::fixed_nii_vector<mse::mstd::string>>();

		{
			/* If the vector is declared as a "scope" object (which basically indicates that it is declared
			on the stack), then you can use "scope" iterators. While there are limitations on when they can
			be used, scope iterators would be the preferred iterator type where performance is a priority
			as they don't require extra run-time overhead to ensure that the vector has not been prematurely
			deallocated. */

			/* Here we're declaring a fixed vector as a scope object. */
			mse::TXScopeObj<mse::fixed_nii_vector<int> > vector1_xscpobj{ 1, 2, 3 };

			{
				/* Here we're obtaining a scope iterator to the vector. */
				auto xscp_iter1 = mse::make_xscope_begin_iterator(&vector1_xscpobj);
				auto xscp_iter2 = mse::make_xscope_end_iterator(&vector1_xscpobj);

				std::sort(xscp_iter1, xscp_iter2);

				/* Here we're obtaining a scope pointer from a scope iterator. */
				auto xscp_ptr1 = mse::xscope_pointer(xscp_iter1);
				auto res3 = *xscp_ptr1;

				auto xscp_citer3 = mse::make_xscope_begin_const_iterator(&vector1_xscpobj);
				xscp_citer3 = xscp_iter1;
				xscp_citer3 = mse::make_xscope_begin_const_iterator(&vector1_xscpobj);

				xscp_citer3 += 2;
				auto res1 = *xscp_citer3;
				auto res2 = xscp_citer3[0];
			}
		}
	}

	{
		/*******************************************/
		/*   xscope_borrowing_fixed_nii_vector<>   */
		/*******************************************/

		/* xscope_borrowing_fixed_nii_vector<> is a kind of xscope_fixed_nii_vector<> that, at construction, "borrows" (or "takes"
		by moving) the contents of a specified existing (scope object) vector, then, upon destruction "returns" the (possibly
		modified) contents back to the original owner. */

		auto xs_nii_vector1_xscpobj = mse::make_xscope(mse::nii_vector<int>{ 1, 2, 3 });

		/* First we demonstrate some resizing operations on the nii_vector<>. */
		/* Note that the standard emplace(), insert() and erase() member functions return an iterator. Since nii_vector<> doesn't
		support "implicit" iterators (i.e. iterators generated from the native "this" pointer) those operations are provided by
		free functions that take an explicit (safe) "this" pointer used to generate and return an explicit iterator. */
		mse::push_back(&xs_nii_vector1_xscpobj, 4/*value*/);
		mse::erase(&xs_nii_vector1_xscpobj, 2/*position index*/);
		mse::insert(&xs_nii_vector1_xscpobj, 1/*position index*/, 5/*value*/);
		mse::insert(&xs_nii_vector1_xscpobj, 0/*position index*/, { 6, 7, 8 }/*value*/);
		mse::emplace(&xs_nii_vector1_xscpobj, 2/*position index*/, 9/*value*/);

		const auto fnii_vector1_expected = mse::fixed_nii_vector<int>{ 6, 7, 9, 8, 1, 5, 2, 4 };
		assert(fnii_vector1_expected == mse::make_xscope_borrowing_fixed_nii_vector(&xs_nii_vector1_xscpobj));

		/* Constructing a xscope_borrowing_fixed_nii_vector<> requires a (non-const) scope pointer to an eligible vector. */
		auto xs_bf_nii_vector1_xscpobj = mse::make_xscope(mse::make_xscope_borrowing_fixed_nii_vector(&xs_nii_vector1_xscpobj));

		{
			/* Here we're obtaining a scope iterator to the fixed vector. */
			auto xscp_iter1 = mse::make_xscope_begin_iterator(&xs_bf_nii_vector1_xscpobj);
			auto xscp_iter2 = mse::make_xscope_end_iterator(&xs_bf_nii_vector1_xscpobj);

			std::sort(xscp_iter1, xscp_iter2);

			/* Here we're obtaining a scope pointer from a scope iterator. */
			auto xscp_ptr1 = mse::xscope_pointer(xscp_iter1);
			auto res3 = *xscp_ptr1;
			*xscp_ptr1 = 7;

			auto xscp_citer3 = mse::make_xscope_begin_const_iterator(&xs_bf_nii_vector1_xscpobj);
			xscp_citer3 = xscp_iter1;
			xscp_citer3 = mse::make_xscope_begin_const_iterator(&xs_bf_nii_vector1_xscpobj);

			xscp_citer3 += 2;
			auto res1 = *xscp_citer3;
			auto res2 = xscp_citer3[0];
		}

		class A {
		public:
			A() {}
			int m_i = 0;
		};
		/* Here we're declaring that A can be safely shared between asynchronous threads. */
		typedef mse::rsv::TAsyncShareableAndPassableObj<A> shareable_A_t;

		/* When the element type of an xscope_borrowing_fixed_nii_vector<> is marked as "async shareable", the
		xscope_borrowing_fixed_nii_vector<> itself is (automatically) marked as async shareable as well and can be safely
		shared between asynchronous threads using "access requesters". */

		auto xs_shareable_nii_vector1 = mse::make_xscope(mse::nii_vector<shareable_A_t>{});
		auto xs_aco_bf_nii_vector1 = mse::make_xscope(mse::make_xscope_access_controlled(mse::make_xscope_borrowing_fixed_nii_vector(&xs_shareable_nii_vector1)));
		auto xs_access_requester1 = mse::make_xscope_asyncsharedv2acoreadwrite(&xs_aco_bf_nii_vector1);

		auto xs_shareable_nii_vector2 = mse::make_xscope(mse::nii_vector<mse::nii_string>{ "abc", "def" });
		auto xs_aco_bf_nii_vector2 = mse::make_xscope(mse::make_xscope_access_controlled(mse::make_xscope_borrowing_fixed_nii_vector(&xs_shareable_nii_vector2)));
		auto xs_access_requester2 = mse::make_xscope_asyncsharedv2acoreadwrite(&xs_aco_bf_nii_vector2);

		auto xs_shareable_nii_vector3 = mse::make_xscope(mse::nii_vector<A>{});
		/* If the element type of an xscope_borrowing_fixed_nii_vector<> is not marked as "async shareable", then neither is the
		xscope_borrowing_fixed_nii_vector<> itself. So attempting to create an "access controlled object" using it would result in
		a compile error. */
		//auto xs_aco_bf_nii_vector3 = mse::make_xscope(mse::make_xscope_access_controlled(mse::make_xscope_borrowing_fixed_nii_vector(&xs_shareable_nii_vector3)));
		//auto xs_access_requester3 = mse::make_xscope_asyncsharedv2acoreadwrite(&xs_aco_bf_nii_vector3);
	}

	{
		/*******************/
		/*  Poly pointers  */
		/*******************/

		/* Poly pointers are "chameleon" (or "type-erasing") pointers that can be constructed from, and retain
		the safety features of multiple different pointer types in this library. If you'd like your function
		to be able to take different types of safe pointer parameters, you can "templatize" your function, or
		alternatively, you can declare your pointer parameters as poly pointers. */

		class A {
		public:
			A() {}
			A(std::string x) : b(x) {}
			virtual ~A() {}

			std::string b = "some text ";
		};
		class D : public A {
		public:
			D(std::string x) : A(x) {}
		};
		class B {
		public:
			static std::string foo1(mse::TXScopePolyPointer<A> ptr) {
				std::string retval = ptr->b;
				return retval;
			}
			static std::string foo2(mse::TXScopePolyConstPointer<A> ptr) {
				std::string retval = ptr->b;
				return retval;
			}
			static std::string foo3(mse::TXScopePolyPointer<std::string> ptr) {
				std::string retval = (*ptr) + (*ptr);
				return retval;
			}
			static std::string foo4(mse::TXScopePolyConstPointer<std::string> ptr) {
				std::string retval = (*ptr) + (*ptr);
				return retval;
			}
		protected:
			~B() {}
		};

		/* To demonstrate, first we'll declare some objects such that we can obtain safe pointers to those
		objects. For better or worse, this library provides a bunch of different safe pointers types. */
		mse::TXScopeObj<A> a_scpobj;
		auto a_refcptr = mse::make_refcounting<A>();
		mse::TRegisteredObj<A> a_regobj;
		mse::TCRegisteredObj<A> a_cregobj;

		/* Safe iterators are a type of safe pointer too. */
		mse::mstd::vector<A> a_mstdvec;
		a_mstdvec.resize(1);
		auto a_mstdvec_iter = a_mstdvec.begin();
		mse::us::msevector<A> a_msevec;
		a_msevec.resize(1);
		auto a_msevec_ipointer = a_msevec.ibegin();
		auto a_msevec_ssiter = a_msevec.ss_begin();

		/* And note that safe pointers to member elements need to be wrapped in an mse::TXScopeAnyPointer<> for
		mse::TXScopePolyPointer<> to accept them. */
		auto b_member_a_refc_anyptr = mse::TXScopeAnyPointer<std::string>(mse::make_pointer_to_member_v2(a_refcptr, &A::b));
		auto b_member_a_reg_anyptr = mse::TXScopeAnyPointer<std::string>(mse::make_pointer_to_member_v2(&a_regobj, &A::b));
		auto b_member_a_mstdvec_iter_anyptr = mse::TXScopeAnyPointer<std::string>(mse::make_pointer_to_member_v2(a_mstdvec_iter, &A::b));

		{
			/* All of these safe pointer types happily convert to an mse::TXScopePolyPointer<>. */
			auto res_using_scpptr = B::foo1(&a_scpobj);
			auto res_using_refcptr = B::foo1(a_refcptr);
			auto res_using_regptr = B::foo1(&a_regobj);
			auto res_using_cregptr = B::foo1(&a_cregobj);
			auto res_using_mstdvec_iter = B::foo1(a_mstdvec_iter);
			auto res_using_msevec_ipointer = B::foo1(a_msevec_ipointer);
			auto res_using_msevec_ssiter = B::foo1(a_msevec_ssiter);
			auto res_using_member_refc_anyptr = B::foo3(b_member_a_refc_anyptr);
			auto res_using_member_reg_anyptr = B::foo3(b_member_a_reg_anyptr);
			auto res_using_member_mstdvec_iter_anyptr = B::foo3(b_member_a_mstdvec_iter_anyptr);

			/* Or an mse::TXScopePolyConstPointer<>. */
			auto res_using_scpptr_via_const_poly = B::foo2(&a_scpobj);
			auto res_using_refcptr_via_const_poly = B::foo2(a_refcptr);
			auto res_using_regptr_via_const_poly = B::foo2(&a_regobj);
			auto res_using_cregptr_via_const_poly = B::foo2(&a_cregobj);
			auto res_using_mstdvec_iter_via_const_poly = B::foo2(a_mstdvec_iter);
			auto res_using_msevec_ipointer_via_const_poly = B::foo2(a_msevec_ipointer);
			auto res_using_msevec_ssiter_via_const_poly = B::foo2(a_msevec_ssiter);
			auto res_using_member_refc_anyptr_via_const_poly = B::foo4(b_member_a_refc_anyptr);
			auto res_using_member_reg_anyptr_via_const_poly = B::foo4(b_member_a_reg_anyptr);
			auto res_using_member_mstdvec_iter_anyptr_via_const_poly = B::foo4(b_member_a_mstdvec_iter_anyptr);
		}

		mse::TNullableAnyPointer<A> nanyptr1;
		mse::TNullableAnyPointer<A> nanyptr2(nullptr);
		mse::TNullableAnyPointer<A> nanyptr3(a_refcptr);
		mse::TAnyPointer<A> anyptr3(a_refcptr);
		nanyptr1 = nullptr;
		nanyptr1 = 0;
		nanyptr1 = NULL;
		nanyptr1 = nanyptr2;
		nanyptr1 = mse::TNullableAnyPointer<A>(&a_regobj);
		nanyptr1 = mse::TNullableAnyPointer<A>(a_refcptr);
		auto res_nap1 = *nanyptr1;

		mse::self_test::CPolyPtrTest1::s_test1();
		int q = 3;
	}

	{
		/*********************************/
		/*  TAnyRandomAccessIterator<>   */
		/*  & TAnyRandomAccessSection<>  */
		/*********************************/

		/* Like TAnyPointer<>, TAnyRandomAccessIterator<> and TAnyRandomAccessSection<> are polymorphic iterators and
		"sections" that can be used to enable functions to take as arguments any type of iterator or section of any
		random access container (like an array or vector). */

		mse::mstd::array<int, 4> mstd_array1{ 1, 2, 3, 4 };
		mse::us::msearray<int, 5> msearray2{ 5, 6, 7, 8, 9 };
		mse::mstd::vector<int> mstd_vec1{ 10, 11, 12, 13, 14 };
		class B {
		public:
			static void foo1(mse::TXScopeAnyRandomAccessIterator<int> ra_iter1) {
				ra_iter1[1] = 15;
			}
			static int foo2(mse::TXScopeAnyRandomAccessConstIterator<int> const_ra_iter1) {
				const_ra_iter1 += 2;
				--const_ra_iter1;
				const_ra_iter1--;
				return const_ra_iter1[2];
			}
			static void foo3(mse::TXScopeAnyRandomAccessSection<int> ra_section) {
				for (mse::TXScopeAnyRandomAccessSection<int>::size_type i = 0; i < ra_section.size(); i += 1) {
					ra_section[i] = 0;
				}
			}
			static int foo4(mse::TXScopeAnyRandomAccessConstSection<int> const_ra_section) {
				int retval = 0;
				for (mse::TXScopeAnyRandomAccessSection<int>::size_type i = 0; i < const_ra_section.size(); i += 1) {
					retval += const_ra_section[i];
				}
				return retval;
			}
			static int foo5(mse::TXScopeAnyRandomAccessConstSection<int> const_ra_section) {
				int retval = 0;
				for (const auto& const_item : const_ra_section) {
					retval += const_item;
				}
				return retval;
			}
		};

		auto mstd_array_iter1 = mstd_array1.begin();
		mstd_array_iter1++;
		auto res1 = B::foo2(mstd_array_iter1);
		B::foo1(mstd_array_iter1);

		auto msearray_const_iter2 = msearray2.ss_cbegin();
		msearray_const_iter2 += 2;
		auto res2 = B::foo2(msearray_const_iter2);

		auto res3 = B::foo2(mstd_vec1.cbegin());
		B::foo1(++mstd_vec1.begin());
		auto res4 = B::foo2(mstd_vec1.begin());

		mse::TXScopeAnyRandomAccessIterator<int> ra_iter1 = mstd_vec1.begin();
		mse::TXScopeAnyRandomAccessIterator<int> ra_iter2 = mstd_vec1.end();
		auto res5 = ra_iter2 - ra_iter1;
		ra_iter1 = ra_iter2;

		{
			std::array<int, 4> std_array1{ 1, 2, 3, 4 };
			mse::TXScopeAnyRandomAccessIterator<int> ra_iter1(std_array1.begin());
			mse::TXScopeAnyRandomAccessIterator<int> ra_iter2 = std_array1.end();
			auto res5 = ra_iter2 - ra_iter1;
			ra_iter1 = ra_iter2;
		}

		mse::TXScopeObj<mse::mstd::array<int, 4> > mstd_array3_scpobj = mse::mstd::array<int, 4>({ 1, 2, 3, 4 });
		auto mstd_array_scpiter3 = mse::mstd::make_xscope_begin_iterator(&mstd_array3_scpobj);
		//mstd_array_scpiter3 = mstd_array3_scpobj.begin();
		++mstd_array_scpiter3;
		B::foo1(mstd_array_scpiter3);

		mse::TXScopeAnyRandomAccessSection<int> xscp_ra_section1(mstd_array_iter1, 2);
		B::foo3(xscp_ra_section1);

		mse::TXScopeAnyRandomAccessSection<int> xscp_ra_section2(++mstd_vec1.begin(), 3);
		auto res6 = B::foo5(xscp_ra_section2);
		B::foo3(xscp_ra_section2);
		auto res7 = B::foo4(xscp_ra_section2);

		auto xscp_ra_section1_xscp_iter1 = xscp_ra_section1.xscope_begin();
		auto xscp_ra_section1_xscp_iter2 = xscp_ra_section1.xscope_end();
		auto res8 = xscp_ra_section1_xscp_iter2 - xscp_ra_section1_xscp_iter1;
		bool res9 = (xscp_ra_section1_xscp_iter1 < xscp_ra_section1_xscp_iter2);

		auto ra_section1 = mse::make_random_access_section(mstd_array_iter1, 2);
		B::foo3(ra_section1);
		auto ra_const_section2 = mse::make_random_access_const_section(mstd_vec1.cbegin(), 2);
		B::foo4(ra_const_section2);

		int q = 5;
	}

	{
		/********************************************/
		/*  TXScopeCSSSXSTERandomAccessIterator<>   */
		/*  & TXScopeCSSSXSTERandomAccessSection<>  */
		/********************************************/

		/* TXScopeCSSSXSTERandomAccessIterator<> and TXScopeCSSSXSTERandomAccessSection<> are "type-erased" template classes
		that function much like TXScopeAnyRandomAccessIterator<> and TXScopeAnyRandomAccessSection<> in that they can be
		used to enable functions to take as arguments iterators or sections of various container types (like an arrays or
		vectors) without making the functions in to template functions. But in this case there are limitations on what types
		can be converted. In exchange for these limitations, these types require less overhead. The "CSSSXSTE" part of the
		typenames stands for "Contiguous Sequence, Static Structure, XScope, Type-Erased". So the first restriction is that
		the target container must be recognized as a "contiguous sequence" (basically an array or vector). It also must be
		recognized as having a "static structure". This essentially means that the container cannot be resized. Note that
		while vectors support resizing, the ones in the library can be "structure locked" at run-time to ensure that they
		are not resized (i.e. have a "static structure") during certain periods. And only the "scope" versions of the
		iterators and sections are supported. */

		auto xs_mstd_array1 = mse::make_xscope(mse::mstd::array<int, 4>{ 1, 2, 3, 4 });
		auto xs_msearray2 = mse::make_xscope(mse::us::msearray<int, 5>{ 5, 6, 7, 8, 9 });
		auto xs_mstd_vec1 = mse::make_xscope(mse::mstd::vector<int>{ 10, 11, 12, 13, 14 });
		class B {
		public:
			static void foo1(mse::TXScopeCSSSXSTERandomAccessIterator<int> ra_iter1) {
				ra_iter1[1] = 15;
			}
			static int foo2(mse::TXScopeCSSSXSTERandomAccessConstIterator<int> const_ra_iter1) {
				const_ra_iter1 += 2;
				--const_ra_iter1;
				const_ra_iter1--;
				return const_ra_iter1[2];
			}
			static int foo2b(mse::rsv::TFParam<mse::TXScopeCSSSXSTERandomAccessConstIterator<int> > const_ra_iter1) {
				return foo2(const_ra_iter1);
			}
#ifndef EXCLUDE_DUE_TO_MSVC2019_INTELLISENSE_BUGS1
			static void foo3(mse::TXScopeCSSSXSTERandomAccessSection<int> ra_section) {
				for (mse::TXScopeCSSSXSTERandomAccessSection<int>::size_type i = 0; i < ra_section.size(); i += 1) {
					ra_section[i] = 0;
				}
			}
			static int foo4(mse::TXScopeCSSSXSTERandomAccessConstSection<int> const_ra_section) {
				int retval = 0;
				for (mse::TXScopeCSSSXSTERandomAccessSection<int>::size_type i = 0; i < const_ra_section.size(); i += 1) {
					retval += const_ra_section[i];
				}
				return retval;
			}
			static int foo5(mse::TXScopeCSSSXSTERandomAccessConstSection<int> const_ra_section) {
				int retval = 0;
				for (const auto& const_item : const_ra_section) {
					retval += const_item;
				}
				return retval;
			}
#endif // !EXCLUDE_DUE_TO_MSVC2019_INTELLISENSE_BUGS1
		};

		auto xs_mstd_array_iter1 = mse::make_xscope_begin_iterator(&xs_mstd_array1);
		xs_mstd_array_iter1++;
		auto res1 = B::foo2(xs_mstd_array_iter1);
		B::foo1(xs_mstd_array_iter1);

		auto xs_msearray_const_iter2 = mse::make_xscope_begin_const_iterator(&xs_msearray2);
		xs_msearray_const_iter2 += 2;
		auto res2 = B::foo2(xs_msearray_const_iter2);

		auto xs_mstd_vec1_citer1 = mse::make_xscope_begin_const_iterator(&xs_mstd_vec1);
		auto res3 = B::foo2(xs_mstd_vec1_citer1);
		/*
		Note that attempting to combine the previous two lines into a single expression like so:
		auto res3b = B::foo2(mse::make_xscope_begin_const_iterator(&xs_mstd_vec1));
		is not supported and would result in a compile error.

		In the case of dynamic containers, like vectors and strings, the (scope) iterators can only be converted to
		TXScopeCSSSXSTERandomAccessIterator<>s if they are lvalues, not temporaries/rvalues. This is because scope iterators
		hold a "structure lock" that prevents the container from being resized. If the scope iterator you're attempting to
		convert from is a temporary (rvalue) one, then the structure lock would be just as temporary and it would not be safe
		to create a TXScopeCSSSXSTERandomAccessIterator<> that outlives the period when the container is "structure locked".
		"Non-resizable" containers, like arrays, do not have the same issue, so this restriction does not apply to their
		(scope) iterators.
		*/

		auto res3c = B::foo2b(mse::make_xscope_begin_const_iterator(&xs_mstd_vec1));
		/*
		However, note that, unlike B::foo2(), B::foo2b() "annotates" its (non-returnable) parameter with the rsv::TFParam<>
		template wrapper. This allows it to accept temporary/rvalue (scope) iterators of dynamic containers. This is safe
		because we know that (even temporary) function arguments will outlive their corresponding (non-returnable) parameter
		values.
		*/

		auto xs_mstd_vec1_citer2 = ++mse::make_xscope_begin_iterator(&xs_mstd_vec1);
		B::foo1(xs_mstd_vec1_citer2);
		auto xs_mstd_vec1_iter1 = mse::make_xscope_begin_iterator(&xs_mstd_vec1);
		auto res4 = B::foo2(xs_mstd_vec1_iter1);

		auto xs_mstd_vec1_begin_citer1 = mse::make_xscope_begin_iterator(&xs_mstd_vec1);
		mse::TXScopeCSSSXSTERandomAccessIterator<int> xs_te_iter1 = xs_mstd_vec1_begin_citer1;
		auto xs_mstd_vec1_end_citer1 = mse::make_xscope_end_iterator(&xs_mstd_vec1);
		mse::TXScopeCSSSXSTERandomAccessIterator<int> xs_te_iter2 = xs_mstd_vec1_end_citer1;
		auto res5 = xs_te_iter2 - xs_te_iter1;
		xs_te_iter1 = xs_te_iter2;

		{
			auto std_array1 = mse::make_xscope(std::array<int, 4>{ 1, 2, 3, 4 });
			mse::TXScopeCSSSXSTERandomAccessIterator<int> xs_te_iter1(mse::make_xscope_begin_iterator(&std_array1));
			mse::TXScopeCSSSXSTERandomAccessIterator<int> xs_te_iter2 = mse::make_xscope_end_iterator(&std_array1);
			auto res5 = xs_te_iter2 - xs_te_iter1;
			xs_te_iter1 = xs_te_iter2;
		}

		mse::TXScopeObj<mse::mstd::array<int, 4> > mstd_array3_scpobj = mse::mstd::array<int, 4>({ 1, 2, 3, 4 });
		auto mstd_array_scpiter3 = mse::mstd::make_xscope_begin_iterator(&mstd_array3_scpobj);
		++mstd_array_scpiter3;
		B::foo1(mstd_array_scpiter3);

#ifndef EXCLUDE_DUE_TO_MSVC2019_INTELLISENSE_BUGS1
		mse::TXScopeCSSSXSTERandomAccessSection<int> xscp_ra_section1(xs_mstd_array_iter1++, 2);
		B::foo3(xscp_ra_section1);
		auto xs_mstd_vec_iter1 = mse::make_xscope_begin_iterator(&xs_mstd_vec1);
		mse::TXScopeCSSSXSTERandomAccessSection<int> xscp_ra_section2(++xs_mstd_vec_iter1, 3);
		/*
		Note that had we instead used the postfix increment operator on the xs_mstd_vec_iter1 argument in the previous line
		like so:
		mse::TXScopeCSSSXSTERandomAccessSection<int> xscp_ra_section2b(xs_mstd_vec_iter1++, 3);
		we would have gotten a compile error. That's because unlike the prefix increment operator, the postfix increment
		operator returns a temporary (rvalue) copy of the iterator, and as noted earlier, in the case of dynamic containers,
		like vectors and strings, conversion/construction from temporary (rvalue) versions of their iterators could be unsafe
		and is not supported.
		*/

		auto res6 = B::foo5(xscp_ra_section2);
		B::foo3(xscp_ra_section2);
		auto res7 = B::foo4(xscp_ra_section2);

		auto xs_ra_section1 = mse::make_xscope_random_access_section(xs_mstd_array_iter1, 2);
		B::foo3(xs_ra_section1);
		auto xs_ra_const_section2 = mse::make_xscope_random_access_const_section(mse::make_xscope_begin_const_iterator(&xs_mstd_vec1), 2);
		B::foo4(xs_ra_const_section2);

		auto nii_array4_scpobj = mse::make_xscope(mse::nii_array<int, 4>{ 1, 2, 3, 4 });
		auto xscp_ra_section3 = mse::make_xscope_csssxste_random_access_section(&nii_array4_scpobj);

		auto xscp_ra_section1_xscp_iter1 = mse::make_xscope_begin_iterator(xscp_ra_section1);
		auto xscp_ra_section1_xscp_iter2 = mse::make_xscope_end_iterator(xscp_ra_section1);
		auto res8 = xscp_ra_section1_xscp_iter2 - xscp_ra_section1_xscp_iter1;
		bool res9 = (xscp_ra_section1_xscp_iter1 < xscp_ra_section1_xscp_iter2);
#endif // !EXCLUDE_DUE_TO_MSVC2019_INTELLISENSE_BUGS1
	}

	{
		/****************/
		/*  optional<>  */
		/****************/

		{
			/* mstd::optional<> is essentially just a safe implementation of std::optional<>. */
			auto opt1 = mse::mstd::optional<int>{ 7 };
			assert(opt1.has_value());
			auto val1 = opt1.value();
		}
#ifndef EXCLUDE_DUE_TO_MSVC2019_INTELLISENSE_BUGS1
		{
			/* You might think of optional<> as a dynamic container like a vector<> that supports a maximum of one element.
			So like vectors, directly accessing or referencing the contents of an optional<> is discouraged. Instead
			prefer to access the contents via an xscope_borrowing_fixed_optional<>, which is a "non-dynamic" type (i.e. its
			element, if present, can be modified, but elements cannot be added or removed), that will "borrow" the contents
			of the "dynamic" optional<>. */

			auto xs_opt1 = mse::make_xscope(mse::make_optional(mse::mstd::string("abc")));
			// which can also be written as
			// auto xs_opt1 = mse::TXScopeObj<mse::optional<mse::mstd::string> >("abc");

			xs_opt1 = {};
			xs_opt1 = mse::mstd::string("def");

			{
				/* Here we obtain an xscope_borrowing_fixed_optional<> that "borrows" the contents of xs_opt1. */
				auto xs_bfopt1 = mse::make_xscope_borrowing_fixed_optional(&xs_opt1);

				/* Note that accessing xs_opt1 is prohibited while xs_bfopt1 exists. This restriction is enforced.*/

				/* Here we obtain a (safe) pointer to the contained element. */
				auto xs_elem_ptr1 = mse::make_xscope_fixed_optional_element_pointer(&xs_bfopt1);

				/* We can also then obtain a scope pointer to the contained element. */
				auto xs_ptr1 = mse::xscope_pointer(xs_elem_ptr1);
				auto val1 = *xs_ptr1;
				*xs_ptr1 = mse::mstd::string("ghi");
			}
			/* After the xscope_borrowing_fixed_optional<> is gone, we can again access our optional<>. */
			xs_opt1.reset();
		}
#endif // !EXCLUDE_DUE_TO_MSVC2019_INTELLISENSE_BUGS1
		{
			/* Analogous to mtnii_vector<>, mt_optional<> is a version that is eligible to be shared among threads, at a
			cost of slightly higher run-time overhead. When suitable, using optional<> and xscope_borrowing_optional<> is
			generally preferred. */
			auto opt1_access_requester = mse::make_asyncsharedv2readwrite<mse::mt_optional<mse::nii_string> >("abc");
			auto elem_ptr1 = mse::make_optional_element_pointer(opt1_access_requester.writelock_ptr());
			auto val1 = *elem_ptr1;
		}
#ifndef EXCLUDE_DUE_TO_MSVC2019_INTELLISENSE_BUGS1
		{
			/* optional<>s can, like any other type, be declared as a scope type (using mse::make_xscope() / mse::TXScopeObj<>).
			But they do not support using scope types as their contained element type. It is (intended to be) uncommon to need
			such capability. But the library does provide a couple of versions that support it. xscope_mt_optional<> is eligible
			to be shared among (scope) threads, while xscope_st_optional<> is not. */

			/* Here we're creating a (string) object of scope type. */
			auto xs_str1 = mse::make_xscope(mse::mstd::string("abc"));

			/* Here we're creating an xscope_st_optional<> object that contains a scope pointer to the (scope) string object.
			mstd::optional<>, for example, would not support this. */
			auto xsopt1 = mse::make_xscope_st_optional(&xs_str1);
			// which can also be written as
			// auto xsopt1 = mse::xscope_st_optional<mse::TXScopeFixedPointer<mse::mstd::string> >(&xs_str1);

			auto val1 = *(xsopt1.value());
		}

		{
			/* You might think of optional<> as a dynamic container like a vector<> that supports a maximum of one element.
			That would make mse::optional<> analogous to mse::nii_vector<>. That is, it is eligible to be shared among
			threads while having lower overhead than mt_optional<>, but the trade-off being that you cannot, in general,
			obtain a direct scope pointer to the contained element from a const reference to the mse::optional<> container.
			But, like nii_vector<>, you can if the mse::optional<> container is declared as an "exclusive writer" object: */
			auto xs_ac_opt1 = mse::make_xscope(mse::make_exclusive_writer(mse::optional<mse::nii_string>("abc")));
			auto xs_ac_cptr1 = mse::make_xscope_access_controlled_const_pointer(xs_ac_opt1);
			auto xs_elem_cptr1 = mse::make_xscope_optional_element_const_pointer(xs_ac_cptr1);
			auto xs_ptr1 = mse::xscope_pointer(xs_elem_cptr1);
			auto val1 = *xs_ptr1;
		}

		mse::self_test::COptionalTest1::s_test1();
#endif // EXCLUDE_DUE_TO_MSVC2019_INTELLISENSE_BUGS1
	}

	{
		/*************/
		/*  tuple<>  */
		/*************/

		{
			/* mstd::tuple<> is essentially just an implementation of std::tuple<> that supports the library's scope and
			data race safety mechanisms. But you may, on occasion, also need a (safe) pointer that directly targets a
			contained element. You could make the element type a "registered" or "norad" object. Alternatively, you can
			obtain a safe pointer to a contained element from a pointer to the tuple<> object like so: */
			auto tup1_refcptr = mse::make_refcounting<mse::mstd::tuple<double, char, mse::mstd::string> >(mse::mstd::make_tuple(3.8, 'A', mse::mstd::string("Lisa Simpson")));
			auto elem_ptr1 = mse::make_tuple_element_pointer<double>(tup1_refcptr);
			auto val1 = *elem_ptr1;
		}
		{
			/* More commonly, the tuple<> object might be declared as a scope object. */
			auto xs_tup1 = mse::make_xscope(mse::mstd::make_tuple(3.8, 'A', mse::mstd::string("Lisa Simpson")));
			// which can also be written as
			// auto xs_tup1 = mse::TXScopeObj<mse::mstd::tuple<double, char, mse::mstd::string> >(3.8, 'A', mse::mstd::string("Lisa Simpson"));

			auto xs_elem_ptr1 = mse::make_xscope_tuple_element_pointer<2>(&xs_tup1);

			/* Here we obtain a "regular" scope pointer to the element from the scope "tuple element pointer". */
			auto xs_ptr1 = mse::xscope_pointer(xs_elem_ptr1);
			auto val1 = *xs_ptr1;
		}
		{
			/* mstd::tuple<> can, like any other type, be declared as a scope type (using mse::make_xscope()
			/ mse::TXScopeObj<>). But it does not support using scope types as a contained element type. It
			is (intended to be) uncommon to need such capability. But the library does provide a version,
			xscope_tuple<>, that supports it. */

			/* Here we're creating a (string) object of scope type. */
			auto xs_str1 = mse::make_xscope(mse::mstd::string("abc"));

			/* Here we're creating an xscope_tuple<> object that contains a scope pointer to the (scope) string object.
			mstd::tuple<>, for example, would not support this. */
			auto xstup1 = mse::make_xscope_tuple(&xs_str1);
			// which can also be written as
			// auto xstup1 = mse::xscope_tuple<mse::TXScopeFixedPointer<mse::mstd::string> >(&xs_str1);

			auto val1 = *(std::get<0>(xstup1));
		}

		mse::self_test::CTupleTest1::s_test1();
	}

	{
		/***********************/
		/*  return_value()     */
		/***********************/

		/* The return_value() function (generally) just returns its argument and verifies that it is of a type that is safe
		to return from a function (basically, doesn't contain any scope pointers). If not it will induce a compile error.
		Functions that do or could return scope types should wrap their return value with this function. */

		class CB {
		public:
			/* It's generally not necessary for a function return type to be a scope type. Even if the return value
			is of a scope type, you can usually just use the underlying (non-scope) type of the scope object as the
			return type. */
			static mse::mstd::string foo1() {
				mse::TXScopeObj<mse::mstd::string> xscp_string1("some text");
				return mse::return_value(xscp_string1);
			}

			/* In the less common case where the scope type doesn't have an underlying non-scope type, it may be safe
			to return the scope object.  */
			static mse::xscope_optional<mse::mstd::string> foo2() {
				mse::xscope_optional<mse::mstd::string> xscp_returnable_obj1(mse::mstd::string("some text"));
				return mse::return_value(xscp_returnable_obj1);
			}

			static auto foo3() {
				mse::TXScopeObj<mse::mstd::string> xscp_string1("some text");
				auto xscp_ptr = &xscp_string1;

				/* Returning a scope pointer (to a local variable for example) could be unsafe. So mse::return_value()
				induces a compile error if the return value contains a scope pointer/reference. */
				//return mse::return_value(xscp_ptr);

				return mse::return_value(xscp_string1);
			}
		};

		mse::TXScopeObj<mse::mstd::string> xscp_res1 = CB::foo1();
		mse::xscope_optional<mse::mstd::string> xscp_res2 = CB::foo2();

		typedef mse::TXScopeObj<mse::mstd::string> xscope_string_t;
		xscope_string_t xscp_str1 = "some text";

		mse::TNDRegisteredObj<mse::mstd::string> reg_str1 = "some text";
		auto reg_ptr_res1 = J::foo10(reg_str1);
		//auto xscp_ptr_res1 = J::foo10(xscp_str1); // <-- would induce a compile error inside J::foo10() 
	}

#ifndef EXCLUDE_DUE_TO_MSVC2019_INTELLISENSE_BUGS1
	{
		/* Defining your own scope types. */

		/* It is (intended to be) uncommon to need to define your own scope types. In general, if you want to use a
		type as a scope type, you can just wrap it with the mse::TXScopeObj<> template. */

		/* But in cases where you're going to use a scope type as a member of a class or struct, that class or struct
		must itself be a scope type. Improperly defining a scope type could result in unsafe code. Use of a safety
		enforcement helper tool, like scpptool, is required to ensure that your scope type is defined properly. 
		And note that mse::xscope_tuple<> also supports scope type elements/fields. */

		/* Scope types need to publicly inherit from mse::rsv::XScopeTagBase. And by convention, be named with a
		prefix indicating that it's a scope type. */
		class xscope_my_type1 : public mse::rsv::XScopeTagBase {
		public:
			xscope_my_type1(const mse::xscope_optional<mse::mstd::string>& xscp_maybe_string)
				: m_xscp_maybe_string1(xscp_maybe_string) {}

			mse::xscope_optional<mse::mstd::string> m_xscp_maybe_string1;
		};

		/* If your type contains or owns any non-owning scope pointers, then it must also publicly inherit
		from mse::rsv::ContainsNonOwningScopeReferenceTagBase. If your type contains or owns any item that can be
		independently targeted by scope pointers (i.e. basically has a '&' ("address of" operator) that yeilds
		a scope pointer), then it must also publicly inherit from mse::rsv::ReferenceableByScopePointerTagBase.
		Failure to do so could result in unsafe code. The scpptool, for example, will check for any such failure. */
		class xscope_my_type2 : public mse::rsv::XScopeTagBase, public mse::rsv::ContainsNonOwningScopeReferenceTagBase
			, public mse::rsv::ReferenceableByScopePointerTagBase
		{
		public:
			typedef mse::TXScopeFixedConstPointer<mse::mstd::string> xscope_string_ptr_t;

			xscope_my_type2(const mse::xscope_optional<xscope_string_ptr_t>& xscp_maybe_string_ptr) : m_xscp_maybe_string_ptr(xscp_maybe_string_ptr) {}

			/* This item (potentially) contains a non-owning scope pointer. */
			mse::xscope_optional<xscope_string_ptr_t> m_xscp_maybe_string_ptr;

			/* This item owns an object that can be independently targeted by scope pointers. That is,
			&(*m_xscp_string_owner_ptr) yields a scope pointer. */
			mse::TXScopeOwnerPointer<mse::mstd::string> m_xscp_string_owner_ptr;
		};
	}
#endif // !EXCLUDE_DUE_TO_MSVC2019_INTELLISENSE_BUGS1

	{
		/****************************/
		/*  TRandomAccessSection<>  */
		/****************************/

		/* "Random access sections" are basically objects that provide access to a (contiguous) subsection of an
		existing array or vector. Their interface is largely the same as std::basic_string_view<>, but are generally
		constructed using "make_random_access_section()" functions. */

		mse::mstd::array<int, 4> mstd_array1{ 1, 2, 3, 4 };
		mse::mstd::vector<int> mstd_vec1{ 10, 11, 12, 13, 14 };

		auto xscp_ra_section1 = mse::make_xscope_random_access_section(mstd_array1.begin(), 2);
		J::foo13(xscp_ra_section1);

		auto ra_const_section2 = mse::make_random_access_const_section(++mstd_vec1.begin(), 3);
		auto res6 = J::foo15(ra_const_section2);
		auto res7 = J::foo14(ra_const_section2);

		auto xscp_ra_section1_xscp_iter1 = xscp_ra_section1.xscope_begin();
		auto xscp_ra_section1_xscp_iter2 = xscp_ra_section1.xscope_end();
		auto res8 = xscp_ra_section1_xscp_iter2 - xscp_ra_section1_xscp_iter1;
		bool res9 = (xscp_ra_section1_xscp_iter1 < xscp_ra_section1_xscp_iter2);

		/* Like non-owning scope pointers, scope sections may not be used as a function return value. (The return_value()
		function wrapper enforces this.) Pretty much the only time you'd legitimately want to do this is when the
		returned section is constructed from one of the input parameters. Let's consider a simple example of a
		"first_half()" function that takes a scope section and returns a scope section spanning the first half of the
		section. The library provides the random_access_subsection() function which takes a random access section and a
		tuple containing a start index and a length and returns a random access section spanning the indicated
		subsection. You could use this function to implement the equivalent of a "first_half()" function like so: */

		auto xscp_ra_section3 = mse::random_access_subsection(xscp_ra_section1, std::make_tuple(0, xscp_ra_section1.length()/2));
		assert(xscp_ra_section3.length() == 1);

#ifndef EXCLUDE_DUE_TO_MSVC2019_INTELLISENSE_BUGS1
		{
			/* In this block we demonstrate the rsv::TXScopeFParam<> specializations that enable passing temporary objects to
			functions expecting scope random access section arguments. */
			class CD {
			public:
				typedef decltype(mse::make_xscope_random_access_const_section(mse::pointer_to(mse::TXScopeObj<mse::nii_vector<int> >
					(mse::nii_vector<int>{ 1, 2, 3})))) xscope_ra_csection_t;
				static bool second_is_longer(mse::rsv::TXScopeFParam<xscope_ra_csection_t> xscope_ra_csection1
					, mse::rsv::TXScopeFParam<xscope_ra_csection_t> xscope_ra_csection2) {

					return (xscope_ra_csection1.size() > xscope_ra_csection2.size()) ? false : true;
				}

				static bool second_is_longer_any(mse::rsv::TXScopeFParam<mse::TXScopeAnyRandomAccessConstSection<int> > xscope_ra_csection1
					, mse::rsv::TXScopeFParam<mse::TXScopeAnyRandomAccessConstSection<int> > xscope_ra_csection2) {
					return (xscope_ra_csection1.size() > xscope_ra_csection2.size()) ? false : true;
				}
			};

			mse::TXScopeObj<mse::nii_vector<int> > vector1(mse::nii_vector<int>{ 1, 2, 3});
			auto xscope_ra_csection1 = mse::make_xscope_random_access_const_section(&vector1);
			auto res1 = CD::second_is_longer(xscope_ra_csection1, mse::make_xscope_random_access_const_section(
				mse::pointer_to(mse::TXScopeObj<mse::nii_vector<int> >(mse::nii_vector<int>{ 1, 2, 3, 4}))));
			auto res2 = J::second_is_longer(xscope_ra_csection1, mse::make_xscope_random_access_const_section(
				mse::pointer_to(mse::TXScopeObj<mse::nii_vector<int> >(mse::nii_vector<int>{ 1, 2, 3, 4}))));
			auto res3 = CD::second_is_longer_any(xscope_ra_csection1, mse::make_xscope_random_access_const_section(
				mse::pointer_to(mse::TXScopeObj<mse::nii_vector<int> >(mse::nii_vector<int>{ 1, 2, 3, 4}))));
		}
#endif // !EXCLUDE_DUE_TO_MSVC2019_INTELLISENSE_BUGS1
	}

	{
		/******************/
		/*  mstd::string  */
		/*  & nii_string  */
		/******************/

		std::string str1 = "some text";
		mse::nii_string nii_str1 = "some text";
		mse::us::msestring msestr1 = "some text";
		mse::mstd::string mstdstr1 = "some text";

		std::cout << str1;
		std::cout << nii_str1;
		std::cout << msestr1;
		std::cout << mstdstr1;

		{
			std::string str2 = "some text";
			mse::nii_string nii_str2 = "some text";
			mse::us::msestring msestr2 = "some text";
			mse::mstd::string mstdstr2 = "some text";

			std::string stringvalues = "125 320 512 750 333, 125 \n320, 512 \n750, 333 \n125, 320 \n512, 750 \n333, \n";
			std::istringstream iss(stringvalues);
			iss >> mstdstr2;
			iss >> msestr2;
			iss >> nii_str2;
			iss >> str2;
			std::getline(iss, mstdstr2, ',');
			std::getline(iss, mstdstr2);
			std::getline(iss, msestr2, ',');
			std::getline(iss, msestr2);
			std::getline(iss, nii_str2, ',');
			std::getline(iss, nii_str2);
			std::getline(iss, str2);
		}

		auto str2 = str1 + str1;
		str2.replace(1, 2, str1);
		auto comp_res1 = str2.compare(str1);
		auto nii_str2 = nii_str1 + nii_str1;
		nii_str2.replace(1, 2, nii_str1);
		auto comp_res2 = nii_str2.compare(nii_str1);
		auto msestr2 = msestr1 + msestr1;
		msestr2.replace(1, 2, msestr1);
		auto comp_res3 = msestr2.compare(msestr1);
		auto mstdstr2 = mstdstr1 + mstdstr1;
		mstdstr2.replace(1, 2, mstdstr1);
		auto comp_res4 = mstdstr2.compare(mstdstr1);

		std::string str3 = "some text";
		mse::TXScopeObj<mse::nii_string> xscp_nii_str3 = "some text";
		auto nii_str3_xscpiter1 = mse::make_xscope_begin_iterator(&xscp_nii_str3);
		nii_str2.copy(nii_str3_xscpiter1, 5);

		mse::TRegisteredObj<mse::nii_string> reg_nii_str3 = "some text";
		nii_str2.copy(mse::make_begin_iterator(&reg_nii_str3), 5);

		str2 = str2.substr(1);
		nii_str2 = nii_str2.substr(1);
		msestr2 = msestr2.substr(1);
		mstdstr2 = mstdstr2.substr(1);

		std::swap(nii_str1, nii_str2);
		std::swap(str1, nii_str1);

		std::swap(msestr1, msestr2);
		std::swap(str1, msestr1);
		std::swap(msestr1, nii_str1);
		std::swap(nii_str1, msestr1);

		std::swap(mstdstr1, mstdstr2);
		std::swap(str1, mstdstr1);
		std::swap(mstdstr1, nii_str1);
		std::swap(nii_str1, mstdstr1);

		{
			using namespace std::literals;
			auto stdstr5 = "some text"s;
		}
		{
			using namespace mse::mstd::literals;
			auto mstdstr5 = "some text"_mstds;
		}
		{
			using namespace mse::literals;
			auto niistr5 = "some text"_niis;
		}

		{
#ifdef MSE_HAS_CXX17
			/* deduction guide example */
			auto str1 = std::string("abcd");
			{
				auto nii_str1 = mse::nii_string{ str1.cbegin(), str1.cend() };
				assert('b' == nii_str1[1]);
			}
			{
				auto sv1 = std::string_view(str1);
				auto nii_str2 = mse::nii_string{ sv1 };
				assert('b' == nii_str2[1]);
			}
			{
				auto xs_str_csection1 = mse::make_string_const_section(str1);
				auto nii_str3 = mse::nii_string{ xs_str_csection1 };
				assert('b' == nii_str3[1]);
			}
#endif /* MSE_HAS_CXX17 */
		}
	}

	{
		/************************/
		/*   fixed_nii_string   */
		/************************/

		/* An fixed_nii_string is basically like an nii_array<> (i.e. not resizable) whose size is specified at
		construction (rather than at compile-time). */

		mse::TRegisteredObj<mse::fixed_nii_string> rg_so1{ "abc" };
		mse::TRegisteredPointer<mse::fixed_nii_string> so1_regptr1 = &rg_so1;

		/* fixed_nii_string does not have a begin() member function that returns an "implicit" iterator. You can obtain an
		iterator using the make_begin_iterator() et al. functions, which take a (safe) pointer to the container. */
		auto iter1 = mse::make_begin_iterator(so1_regptr1);
		auto citer1 = mse::make_end_const_iterator(so1_regptr1);
		citer1 = iter1;

		/* fixed_nii_strings can be constructed from other (resizeable) string types */
		auto nstr1 = mse::nii_string{ "def" };
		mse::fixed_nii_string fixed_nstr1(nstr1);
		assert(fixed_nstr1.size() == nstr1.size());
		mse::fixed_nii_string fixed_nstr2(mse::mstd::string{ "de" });

		std::cout << fixed_nstr1 << "\n";

		/* "fixed_nii_string"s are marked as "async shareable" and can be safely shared between asynchronous threads
		using "access requesters". */
		auto access_requester1 = mse::make_asyncsharedv2readwrite<mse::fixed_nii_string>();

		{
			/* If the string is declared as a "scope" object (which basically indicates that it is declared
			on the stack), then you can use "scope" iterators. While there are limitations on when they can
			be used, scope iterators would be the preferred iterator type where performance is a priority
			as they don't require extra run-time overhead to ensure that the string has not been prematurely
			deallocated. */

			/* Here we're declaring a fixed string as a scope object. */
			mse::TXScopeObj<mse::fixed_nii_string > string1_xscpobj{ "abc" };

			{
				/* Here we're obtaining a scope iterator to the string. */
				auto xscp_iter1 = mse::make_xscope_begin_iterator(&string1_xscpobj);
				auto xscp_iter2 = mse::make_xscope_end_iterator(&string1_xscpobj);

				std::sort(xscp_iter1, xscp_iter2);

				/* Here we're obtaining a scope pointer from a scope iterator. */
				auto xscp_ptr1 = mse::xscope_pointer(xscp_iter1);
				auto res3 = *xscp_ptr1;

				auto xscp_citer3 = mse::make_xscope_begin_const_iterator(&string1_xscpobj);
				xscp_citer3 = xscp_iter1;
				xscp_citer3 = mse::make_xscope_begin_const_iterator(&string1_xscpobj);

				xscp_citer3 += 2;
				auto res1 = *xscp_citer3;
				auto res2 = xscp_citer3[0];
			}
		}
	}

	{
		/*****************************************/
		/*   xscope_borrowing_fixed_nii_string   */
		/*****************************************/

		/* xscope_borrowing_fixed_nii_string is a kind of xscope_fixed_nii_string that, at construction, "borrows" (or "takes"
		by moving) the contents of a specified existing (scope object) string, then, upon destruction "returns" the (possibly
		modified) contents back to the original owner. */

		auto xs_nii_string1_xscpobj = mse::make_xscope(mse::nii_string{ "abc" });

		/* Constructing a xscope_borrowing_fixed_nii_string requires a (non-const) scope pointer to an eligible string. */
		auto xs_bf_nii_string1_xscpobj = mse::make_xscope(mse::make_xscope_borrowing_fixed_nii_basic_string(&xs_nii_string1_xscpobj));
		/* or alternatively: */
		//mse::TXScopeObj<mse::xscope_borrowing_fixed_nii_basic_string<mse::nii_string> > xs_bf_nii_string1_xscpobj{ &xs_nii_string1_xscpobj };

		std::cout << xs_bf_nii_string1_xscpobj << "\n";

		{
			/* Here we're obtaining a scope iterator to the fixed string. */
			auto xscp_iter1 = mse::make_xscope_begin_iterator(&xs_bf_nii_string1_xscpobj);
			auto xscp_iter2 = mse::make_xscope_end_iterator(&xs_bf_nii_string1_xscpobj);

			std::sort(xscp_iter1, xscp_iter2);

			/* Here we're obtaining a scope pointer from a scope iterator. */
			auto xscp_ptr1 = mse::xscope_pointer(xscp_iter1);
			auto res3 = *xscp_ptr1;

			auto xscp_citer3 = mse::make_xscope_begin_const_iterator(&xs_bf_nii_string1_xscpobj);
			xscp_citer3 = xscp_iter1;
			xscp_citer3 = mse::make_xscope_begin_const_iterator(&xs_bf_nii_string1_xscpobj);

			xscp_citer3 += 2;
			auto res1 = *xscp_citer3;
			auto res2 = xscp_citer3[0];
		}

		/* "xscope_borrowing_fixed_nii_string"s are marked as "async shareable" and can be safely
		shared between asynchronous threads using "access requesters". */

		auto xs_shareable_nii_string1 = mse::make_xscope(mse::nii_string{ "def" });
		auto xs_aco_bf_nii_string1 = mse::make_xscope(mse::make_xscope_access_controlled(mse::make_xscope_borrowing_fixed_nii_basic_string(&xs_shareable_nii_string1)));
		auto xs_access_requester1 = mse::make_xscope_asyncsharedv2acoreadwrite(&xs_aco_bf_nii_string1);
	}

	{
		/**********************/
		/*  TStringSection<>  */
		/**********************/

		/* "String sections" are the string specialized versions of "random access sections", basically providing the
		functionality of std::string_view but supporting construction from any (safe) iterator type, not just raw
		pointer iterators. */

		mse::mstd::string mstring1("some text");
		auto string_section1 = mse::make_string_section(mstring1.begin() + 1, 7);
		auto string_section2 = string_section1.substr(4, 3);
		assert(string_section2.front() == 't');
		assert(string_section2.back() == 'x');

		/* Unlike std::string_view, string sections are available in "non-const" versions. */
		string_section2[0] = 'T';
		std::cout << string_section2;
		assert(mstring1 == "some Text");

		/* The (run-time overhead free) scope (and const) versions. */
		typedef mse::TXScopeObj< mse::nii_string > xscope_nii_string_t;
		xscope_nii_string_t xscp_nstring1("some text");
		auto xscp_citer1 = mse::make_xscope_begin_const_iterator(&xscp_nstring1);
#ifndef EXCLUDE_DUE_TO_MSVC2019_INTELLISENSE_BUGS1
		auto xscp_string_section1 = mse::make_xscope_string_const_section(xscp_citer1 + 1, 7);
		auto xscp_string_section2 = mse::make_xscope_substr(xscp_string_section1, 4, 3);
		assert(xscp_string_section2.front() == 't');
		assert(xscp_string_section2.back() == 'x');

		/* Btw, the random_access_subsection() function works on string sections just like any other random access sections. */
		auto string_section3 = mse::random_access_subsection(string_section1, std::make_tuple(0, string_section1.length() / 2));
		assert(string_section3.length() == 3);
		auto xscp_string_section4 = mse::xscope_random_access_subsection(string_section1, std::make_tuple(0, string_section1.length() / 2));
		auto xscp_string_section5 = mse::xscope_random_access_subsection(xscp_string_section1, std::make_tuple(0, string_section1.length() / 2));

		{
			/* For safety reasons, by default, you can't create a scope string section that references a temporary string. (This
			is not an issue with non-scope string sections.) However, there is one scenario when that is supported. Namely, when
			the scope string section is a function parameter and is indicated as such with the rsv::TXScopeFParam<> transparent
			template wrapper. */

			class CD {
			public:
				/* For this example function, the parameter type we'll be using is a "const scope string section that references a
				scope nii_string". It's a rather verbose type to express, and here we use decltype() to express it. But this example
				function is mostly for demonstration purposes. Generally, as demonstrated in the other example functions, when
				taking string sections as function parameters, rather than specifying a particular string section type, you would
				instead either make the function a function template or use a polymorphic string section type which are more concise
				and give the caller flexibility in terms of the type of string section they can pass. */

				typedef decltype(mse::make_xscope_string_const_section(std::declval<mse::TXScopeFixedConstPointer<mse::nii_string> >())) xscope_string_csection_t;
				static bool second_is_longer(mse::rsv::TXScopeFParam<xscope_string_csection_t> xscope_string_csection1
					, mse::rsv::TXScopeFParam<xscope_string_csection_t> xscope_string_csection2) {

					return (xscope_string_csection1.size() > xscope_string_csection2.size()) ? false : true;
				}

				/* Using (the polymorphic) TXScopeAnyStringConstSection<> as the parameter type will allow the caller to pass
				any type of string section. */
				static bool second_is_longer_any(mse::rsv::TXScopeFParam<mse::TXScopeAnyStringConstSection<> > xscope_string_csection1
					, mse::rsv::TXScopeFParam<mse::TXScopeAnyStringConstSection<> > xscope_string_csection2) {
					return (xscope_string_csection1.size() > xscope_string_csection2.size()) ? false : true;
				}
			};

			mse::TXScopeObj<mse::nii_string > string1(mse::nii_string{"abc"});
			auto xscope_string_csection1 = mse::make_xscope_string_const_section(&string1);

			/* In these function calls, the second parameter is a string section that refers to a temporary string. */
			auto res1 = CD::second_is_longer(xscope_string_csection1, mse::make_xscope_string_const_section(
				mse::pointer_to(mse::TXScopeObj<mse::nii_string >(mse::nii_string{"abcd"}))));
			auto res2 = J::second_is_longer(xscope_string_csection1, mse::make_xscope_string_const_section(
				mse::pointer_to(mse::TXScopeObj<mse::nii_string >(mse::nii_string{"abcd"}))));
			auto res3 = CD::second_is_longer_any(xscope_string_csection1, mse::make_xscope_string_const_section(
				mse::pointer_to(mse::TXScopeObj<mse::nii_string >(mse::nii_string{"abcd"}))));
		}
#endif // !EXCLUDE_DUE_TO_MSVC2019_INTELLISENSE_BUGS1
	}

	{
		/*************************/
		/*  TAnyStringSection<>  */
		/*************************/

		/* "Any" string sections are basically polymorphic string sections that can hold the value of any string
		section type. They can be used as function parameter types to enable functions to accept any type of string
		section. */

		mse::mstd::string mstring1("some text");
		mse::TAnyStringSection<char> any_string_section1(mstring1.begin()+5, 3);

		const auto string_literal = "some text";
		mse::TAnyStringConstSection<char> any_string_const_section2(string_literal+5, 3);

		typedef mse::TRegisteredObj<mse::nii_string> reg_nii_string_t;
		reg_nii_string_t reg_nii_string3("some other text");
		/* This is a different type of (safe) iterator to a different type of string. */
		auto iter = reg_nii_string_t::ss_begin(&reg_nii_string3);

#ifndef EXCLUDE_DUE_TO_MSVC2019_INTELLISENSE_BUGS1
		/* Resulting in a different type of string section. */
		auto string_section3 = mse::make_string_section(iter+11, 3);

		mse::TAnyStringSection<char> any_string_section3 = string_section3;
		assert(any_string_section1 == any_string_section3);
		assert(any_string_section1.front() == 't');
		assert(any_string_section1.back() == 'x');
		any_string_section1 = string_section3;
		any_string_section1[1] = 'E';
#endif // !EXCLUDE_DUE_TO_MSVC2019_INTELLISENSE_BUGS1
	}

#ifndef EXCLUDE_DUE_TO_MSVC2019_INTELLISENSE_BUGS1
	{
		/************************************/
		/*  TXScopeCSSSXSTEStringSection<>  */
		/************************************/

		/* TXScopeCSSSXSTEStringSection<> is the string specialized version of TXScopeCSSSXSTERandomAccessSection<>. Like
		TXScopeAnyStringSection<>, it can, with lower overhead but more restrictions, be used to enable functions to take
		as arguments sections of strings. While mstd::string_view (/TAnyStringConstSection<char>) closely matches the
		interface and flexibilty of std::string_view, TXScopeCSSSXSTEStringConstSection<char> would more closely match
		its (low) overhead. */

		auto xs_mstring1 = mse::make_xscope(mse::mstd::string("some text"));
		auto xs_mstring1_iter1 = mse::make_xscope_begin_iterator(&xs_mstring1) + 5;
		mse::TXScopeCSSSXSTEStringSection<char> xs_csssxste_string_section1(xs_mstring1_iter1, 3);
		/* Note when passing a (scope) string iterator to TXScopeCSSSXSTEStringSection<>'s constructor, as with
		TXScopeCSSSXSTERandomAccessSection<>, that iterator must be an lvalue, not a temporary (rvalue), otherwise
		you'll get a compile error.*/

		mse::TXScopeCSSSXSTEStringConstSection<char> xs_csssxste_string_const_section2("some text");

		auto xs_nii_string3 = mse::make_xscope(mse::nii_string("some other text"));
		auto xs_string_section3 = mse::make_xscope_string_section(mse::make_xscope_begin_iterator(&xs_nii_string3) + 11, 3);
		/* Note that xs_string_section3 is just a regular string section, not a TXScopeCSSSXSTEStringSection<>, so we
		can construct it from an rvalue iterator. */

		mse::TXScopeCSSSXSTEStringSection<char> xs_csssxste_string_section3 = xs_string_section3;
		/* As with iterators, only construction from lvalue string sections is supported, not temporary (rvalue) string sections. */

		assert(xs_csssxste_string_section1 == xs_csssxste_string_section3);
		assert(xs_csssxste_string_section1.front() == 't');
		assert(xs_csssxste_string_section1.back() == 'x');
		xs_csssxste_string_section1[1] = 'E';
	}
#endif // !EXCLUDE_DUE_TO_MSVC2019_INTELLISENSE_BUGS1

	{
		/***********************/
		/*  mstd::string_view  */
		/***********************/

		/* std::string_view stores an (unsafe) pointer iterator into its target string. mse::mstd::string_view can
		instead store any type of string iterator, including memory safe iterators. So for example, when assigned
		from an mse::mstd::string, mse::mstd::string_view will hold one of mse::mstd::string's safe (strong) iterators
		(obtained with a call to the string's cbegin() member function). Consequently, the mse::mstd::string_view will
		be safe against "use-after-free" bugs to which std::string_view is so prone. */
		mse::mstd::string_view msv1;
		{
			mse::mstd::string mstring1("some text");
			msv1 = mse::mstd::string_view(mstring1);
		}
#if !defined(MSE_MSTDSTRING_DISABLED) && !defined(MSE_MSTD_STRING_CHECK_USE_AFTER_FREE)
		MSE_TRY {
			/* This is not undefined (or unsafe) behavior. */
			auto ch1 = msv1[3]; /* In debug mode this will fail an assert. In non-debug mode it'll just work (safely). */
			assert('e' == ch1);
		}
		MSE_CATCH_ANY {
			/* At present, no exception will be thrown. Instead, the lifespan of the string data is extended to match
			that of the mstd::string_view. It's possible that in future library implementations, an exception may be thrown. */
		}
#endif //!defined(MSE_MSTDSTRING_DISABLED) && !defined(MSE_MSTD_STRING_CHECK_USE_AFTER_FREE)

		mse::mstd::string mstring2("some other text");
		/* With std::string_view, you specify a string subrange with a raw pointer iterator and a length. With
		mse::mstd::string_view you are not restricted to (unsafe) raw pointer iterators. You can use memory safe
		iterators like those provided by mse::mstd::string. */
		auto msv2 = mse::mstd::string_view(mstring2.cbegin()+5, 7);
		assert(msv2 == "other t");

		/* mse::mstd::string_view is actually discourged in favor mse::nrp_string_view, which doesn't support
		construction from unsafe raw pointer iterators. */
	}

	{
		/* Common string_view bug. */
		{
			std::string s = "Hellooooooooooooooo ";
			//std::string_view sv = s + "World\n";
			//std::cout << sv;
		}
		{
#if !defined(MSE_MSTDSTRING_DISABLED) && !defined(MSE_MSTD_STRING_CHECK_USE_AFTER_FREE)
			/* Memory safe substitutes for std::string and std::string_view eliminate the danger. */

			mse::mstd::string s = "Hellooooooooooooooo ";
			mse::nrp_string_view sv = s + "World\n";
			MSE_TRY {
				/* This is not undefined (or unsafe) behavior. */
				std::cout << sv; /* In debug mode this will fail an assert. In non-debug mode it'll just work (safely). */
			}
			MSE_CATCH_ANY {
				/* At present, no exception will be thrown. Instead, the lifespan of the string data is extended to match
				that of the mse::nrp_string_view. It's possible that in future library implementations, an exception may be thrown. */
			}
#endif //!defined(MSE_MSTDSTRING_DISABLED) && !defined(MSE_MSTD_STRING_CHECK_USE_AFTER_FREE)
		}
		{
			/* Memory safety can also be achieved without extra run-time overhead. */

			/* nii_string is a safe string type (with no extra run-time overhead). */
			mse::nii_string s = "Hellooooooooooooooo ";

			/* TXScopeObj<> is a transparent "annotation" template wrapper indicating that the object has "scope lifetime"
			(i.e. is declared on the stack). The wrapper, to the extent possible, enforces the claim. */
			mse::TXScopeObj< mse::nii_string > xscope_s2 = s + "World\n";

			/* xscope_pointer is not a raw pointer. It is an "annotated" pointer indicating its target has scope lifetime.
			The '&' operator is overloaded. */
			auto xscope_pointer = &xscope_s2;

			/* Here we create a safe "string_view"-like object we call a "string section". This version has no extra
			run-time overhead. The "xscope_" prefix indicates that this version also has scope lifetime (enforced to the
			extent possible). */
			auto xscope_sv = mse::make_xscope_nrp_string_const_section(xscope_pointer);

			std::cout << xscope_sv;

			/* The preceding block, while a little more verbose, is memory safe without extra run-time overhead. */

			/* And just to be clear: */

			/* You can't construct a string section directly from a naked nii_string (temporary or otherwise). */
			//auto xscope_sv2 = mse::make_xscope_nrp_string_const_section(s + "World\n");	 // <-- compile error

			/* And you won't be able to store a "scope" pointer to a temporary. */
			//auto xscope_pointer2 = &(mse::TXScopeObj< mse::nii_string >(s + "World\n"));	 // <-- compile error

			/* Passing a temporary scope string section that references a temporary string as a function argument is
			supported. But only if the function parameter is declared to support it. */
		}
#ifndef EXCLUDE_DUE_TO_MSVC2019_INTELLISENSE_BUGS1
		{
			std::string s = "Hellooooooooooooooo ";
			auto sv = mse::make_nrp_string_const_section(mse::make_refcounting<std::string>(s + "World\n"));
			std::cout << sv;
		}
		{
			std::string s = "Hellooooooooooooooo ";
			auto sv = mse::mknscs(mse::mkrclp(s + "World\n"));
			std::cout << sv;
		}
		{
#ifndef MSE_REGISTEREDPOINTER_DISABLED
			std::string s = "Hellooooooooooooooo ";
			auto sv = mse::make_nrp_string_const_section(&mse::TRegisteredObj<std::string>(s + "World\n"));
			MSE_TRY {
				std::cout << sv;
			}
			MSE_CATCH_ANY {
				std::cout << "expected exception\n";
			}
#endif //!MSE_REGISTEREDPOINTER_DISABLED
		}
		{
#ifndef MSE_REGISTEREDPOINTER_DISABLED
			std::string s = "Hellooooooooooooooo ";
			auto sv = mse::mknscs(&mse::make_registered(s + "World\n"));
			MSE_TRY {
				std::cout << sv;
			}
			MSE_CATCH_ANY {
				std::cout << "expected exception\n";
			}
#endif //!MSE_REGISTEREDPOINTER_DISABLED
		}
#endif // !EXCLUDE_DUE_TO_MSVC2019_INTELLISENSE_BUGS1
	}

	{
		/* algorithms */

		mse::TXScopeObj<mse::nii_array<int, 3> > xscope_na1 = mse::nii_array<int, 3>{ 1, 2, 3 };
		auto xscope_na1_begin_citer = mse::make_xscope_begin_const_iterator(&xscope_na1);
		auto xscope_na1_end_citer = mse::make_xscope_end_const_iterator(&xscope_na1);

		mse::mstd::array<int, 3> ma1{ 1, 2, 3 };

		auto xscope_nv1 = mse::make_xscope(mse::nii_vector<int>{ 1, 2, 3 });

		/* Here we declare a vector protected by an "excluse writer" access control wrapper. */
		auto xscope_ewnv1 = mse::make_xscope(mse::make_xscope_exclusive_writer(mse::nii_vector<int>{ 1, 2, 3 }));

		{
			/* for_each_ptr() */

			/*  mse::for_each_ptr() is like std::for_each() but instead of passing, to the given function, a reference
			to each item it passes a (safe) pointer to each item. The actual type of the pointer varies depending on the
			type of the given iterators. */
			typedef mse::for_each_ptr_type<decltype(ma1.begin())> item_ptr_t;
			mse::for_each_ptr(ma1.begin(), ma1.end(), [](item_ptr_t x_ptr) { std::cout << *x_ptr << std::endl; });

			mse::for_each_ptr(xscope_na1_begin_citer, xscope_na1_end_citer, [](const auto x_ptr) { std::cout << *x_ptr << std::endl; });

			/* A "scope range" version is also available that bypasses the use of iterators. As well as often being more
			convenient, it can theoretically be little more performance optimal. */
			typedef mse::xscope_range_for_each_ptr_type<decltype(&xscope_na1)> range_item_ptr_t;
			mse::xscope_range_for_each_ptr(&xscope_na1, [](range_item_ptr_t x_ptr) { std::cout << *x_ptr << std::endl; });

			{
				auto xscope_bfnv1 = mse::make_xscope(mse::make_xscope_borrowing_fixed_nii_vector(&xscope_nv1));
				mse::xscope_range_for_each_ptr(&xscope_bfnv1, [](const auto x_ptr) { std::cout << *x_ptr << std::endl; });
			}
			{
				/* Note that for performance (and safety) reasons, vectors may be "structure locked" for the duration of the loop.
				That is, any attempt to modify the size of the vector during the loop may result in an exception. */
				auto xscope_nv1_begin_iter = mse::make_xscope_begin_iterator(&xscope_nv1);
				auto xscope_nv1_end_iter = mse::make_xscope_end_iterator(&xscope_nv1);
				mse::for_each_ptr(xscope_nv1_begin_iter, xscope_nv1_end_iter, [](const auto x_ptr) { std::cout << *x_ptr << std::endl; });
				mse::xscope_range_for_each_ptr(&xscope_nv1, [](const auto x_ptr) { std::cout << *x_ptr << std::endl; });
			}
			{
				/* A vector protected by an "excluse writer" access control wrapper is ensured to remain unmodified while a
				corresponding "access controlled const pointer" exists. */
				auto xscope_ewnv1_cptr = mse::make_xscope_access_controlled_const_pointer(&xscope_ewnv1);
				/* So looping over its elements is safe and efficient. */
				mse::xscope_range_for_each_ptr(xscope_ewnv1_cptr, [](const auto x_ptr) { std::cout << *x_ptr << std::endl; });
			}
		}
		{
			/* find_if_ptr() */

			typedef mse::find_if_ptr_type<decltype(xscope_na1_begin_citer)> item_ptr_t;
			auto found_citer1 = mse::find_if_ptr(xscope_na1_begin_citer, xscope_na1_end_citer, [](item_ptr_t x_ptr) { return 2 == *x_ptr; });
			auto res1 = *found_citer1;

			auto found_citer3 = mse::find_if_ptr(ma1.cbegin(), ma1.cend(), [](const auto x_ptr) { return 2 == *x_ptr; });

#ifndef EXCLUDE_DUE_TO_MSVC2019_INTELLISENSE_BUGS1
			/* This version returns an optional scope pointer to the found item rather than an iterator. */
			typedef mse::xscope_range_get_ref_if_ptr_type<decltype(&xscope_na1)> range_item_ptr_t;
			auto xscope_optional_xscpptr4 = mse::xscope_range_get_ref_if_ptr(&xscope_na1, [](range_item_ptr_t x_ptr) { return 2 == *x_ptr; });
			auto res4 = xscope_optional_xscpptr4.value();
#endif // !EXCLUDE_DUE_TO_MSVC2019_INTELLISENSE_BUGS1

			/* This version returns a scope pointer to the found item or throws an exception if an appropriate item isn't
			found. */
			auto xscope_pointer5 = mse::xscope_range_get_ref_to_element_known_to_be_present_ptr(&xscope_na1, [](const auto x_ptr) { return 2 == *x_ptr; });
			auto res5 = *xscope_pointer5;
		}
	}

	{
		/*****************************************/
		/*  MSE_DECLARE_THREAD_LOCAL()           */
		/*  & MSE_DECLARE_THREAD_LOCAL_GLOBAL()  */
		/*****************************************/

		auto tlg_ptr1 = &tlg_string1;
		auto xs_tlg_store1 = mse::make_xscope_strong_pointer_store(tlg_ptr1);
		auto xs_ptr1 = xs_tlg_store1.xscope_ptr();
		*xs_ptr1 += "...";
		std::cout << *xs_ptr1 << std::endl;

		MSE_DECLARE_THREAD_LOCAL_CONST(mse::mstd::string) tlc_string2 = "abc";
		auto tlc_ptr2 = &tlc_string2;
		auto xs_tlc_store2 = mse::make_xscope_strong_pointer_store(tlc_ptr2);
		auto xs_cptr2 = xs_tlc_store2.xscope_ptr();
		std::cout << *xs_cptr2 << std::endl;

		class CA {
		public:
			auto foo1() const {
				MSE_DECLARE_THREAD_LOCAL(mse::mstd::string) tl_string = "abc";
				/* mse::return_value() just returns its argument and ensures that it's of a (pointer) type that's safe to return. */
				return mse::return_value(&tl_string);
			}
		};
		auto tl_ptr3 = CA().foo1();
		auto xs_tl_store3 = mse::make_xscope_strong_pointer_store(tl_ptr3);
		auto xs_cptr3 = xs_tl_store3.xscope_ptr();
		*xs_cptr3 += "def";
		std::cout << *xs_cptr3 << std::endl;

		mse::self_test::CThreadLocalPtrTest1::s_test1();
	}

	{
		/******************************************/
		/*  MSE_DECLARE_STATIC_IMMUTABLE()        */
		/*  & MSE_RSV_DECLARE_GLOBAL_IMMUTABLE()  */
		/******************************************/

		auto gimm_ptr1 = &gimm_string1;
		auto xs_gimm_store1 = mse::make_xscope_strong_pointer_store(gimm_ptr1);
		auto xs_ptr1 = xs_gimm_store1.xscope_ptr();
		std::cout << *xs_ptr1 << std::endl;

		MSE_DECLARE_STATIC_IMMUTABLE(mse::nii_string) simm_string2 = "abc";
		auto simm_ptr2 = &simm_string2;
		auto xs_simm_store2 = mse::make_xscope_strong_pointer_store(simm_ptr2);
		auto xs_ptr2 = xs_simm_store2.xscope_ptr();
		std::cout << *xs_ptr2 << std::endl;

		class CA {
		public:
			auto foo1() const {
				MSE_DECLARE_STATIC_IMMUTABLE(mse::nii_string) simm_string = "abc";
				/* mse::return_value() just returns its argument and ensures that it's of a (pointer) type that's safe to return. */
				return mse::return_value(&simm_string);
			}
		};
		auto simm_ptr3 = CA().foo1();
		auto xs_simm_store3 = mse::make_xscope_strong_pointer_store(simm_ptr3);
		auto xs_cptr3 = xs_simm_store3.xscope_ptr();
		std::cout << *xs_cptr3 << std::endl;

		mse::self_test::CStaticImmutablePtrTest1::s_test1();
	}

	{
		/****************************/
		/*   mtnii_basic_string<>   */
		/****************************/

		/* mtnii_basic_string<> is the string counterpart to mtmtnii_vector<>. It is eligible to be (safely) shared
		between asynchronous threads. */

		typedef mse::mtnii_basic_string<char> mtnii_basic_string1_t;
		auto access_requester1 = mse::make_asyncsharedv2readwrite<mtnii_basic_string1_t>(mtnii_basic_string1_t{ "abc" });

		{
			/* Here we're obtaining a scope pointer to the basic_string from a writelock pointer to the basic_string. */
			auto xs_strong_pointer_store = mse::make_xscope_strong_pointer_store(access_requester1.writelock_ptr());
			auto basic_string_xscope_ptr = xs_strong_pointer_store.xscope_ptr();

			/* Here we're obtaining scope iterators to the string. */
			auto xscp_iter1 = mse::make_xscope_begin_iterator(basic_string_xscope_ptr);
			auto xscp_iter2 = mse::make_xscope_end_iterator(basic_string_xscope_ptr);

			std::sort(xscp_iter1, xscp_iter2);

			auto xscp_citer3 = mse::make_xscope_begin_const_iterator(basic_string_xscope_ptr);
			xscp_citer3 = xscp_iter1;
			xscp_citer3 = mse::make_xscope_begin_const_iterator(basic_string_xscope_ptr);
			xscp_citer3 += 1;
			auto res1 = *xscp_citer3;
			auto res2 = xscp_citer3[0];

			/* Note that scope iterators to strings (and other dynamic containers), "lock the structure" of the container
			so that, for example, it cannot be resized. This allows us to obtain a scope pointer to the (lvalue) iterator's
			target element. */
			auto xscp_ptr1 = mse::xscope_pointer(xscp_citer3);
			auto res3 = *xscp_ptr1;

			assert(res3 == 'b');
		}

		/* stnii_basic_string<>s can be (efficiently) swapped with stnii_basic_string<>s. */
		auto stniibs1 = mse::stnii_basic_string<char>();
		std::swap(*(access_requester1.writelock_ptr()), stniibs1);
		/* Or mstd::basic_string<>s. */
		auto mstdbs1 = mse::mstd::basic_string<char>();
		std::swap(*(access_requester1.writelock_ptr()), mstdbs1);
	}

	{
		/****************************/
		/*   stnii_basic_string<>   */
		/****************************/

		/* stnii_basic_string<> is the string counterpart to stmtnii_vector<>. It is not eligible to be shared between threads. */

		mse::TXScopeObj<mse::stnii_basic_string<char> > basic_string1_xscpobj = mse::stnii_basic_string<char>{ "abc" };

		{
			mse::TXScopeFixedConstPointer<mse::stnii_basic_string<char> > xscptr = &basic_string1_xscpobj;
			auto xscp_citer1 = mse::make_xscope_begin_const_iterator(xscptr);
			xscp_citer1 += 2;
			auto xscp_cptr1 = mse::xscope_const_pointer(xscp_citer1);
			auto res1 = *xscp_cptr1;
		}
		basic_string1_xscpobj.push_back(4);

		/* And of course stnii_basic_string<>s can be (efficiently) swapped with mtnii_basic_string<>s. */
		auto mtniibs1 = mse::mtnii_basic_string<char>();
		std::swap(basic_string1_xscpobj, mtniibs1);
		/* Or mstd::basic_string<>s. */
		auto mstdbs1 = mse::mstd::string();
		std::swap(basic_string1_xscpobj, mstdbs1);
	}

	{
		/****************/
		/*  function<>  */
		/****************/

		{
			/* mstd::function<> is essentially just an implementation of std::function<> that supports the library's scope and
			data race safety mechanisms. */
			mse::mstd::function<int()> function1 = []() { return 3; };
			function1 = []() { return 5; };
			int res1 = function1();
		}
		{
			/* xscope_function<>s support scope function objects as well. */
			mse::xscope_function<int()> xs_function1 = []() { return 5; };

			auto xs_int1 = mse::make_xscope(int(5));
			auto int1_xsptr = &xs_int1;

			struct xscope_my_function_obj_t : public mse::rsv::XScopeTagBase, public mse::rsv::ContainsNonOwningScopeReferenceTagBase {
				xscope_my_function_obj_t(decltype(int1_xsptr) int_xsptr) : m_int_xsptr(int_xsptr) {}
				decltype(int1_xsptr) m_int_xsptr; /* this is a scope pointer */
				int operator()() const { return *m_int_xsptr; }
			};
			/* xscope_my_function_obj_t is a scope type with a scope (pointer) member. */
			xscope_my_function_obj_t xscope_my_function_obj1(int1_xsptr);
			mse::xscope_function<int()> xs_function2 = xscope_my_function_obj1;
			int res1 = xs_function2();

			/*  Just as structs with scope pointer/reference members need to be declared as such, lambdas that
			capture scope pointer/references must be declared as such. */
			auto xs_lambda1 = mse::rsv::make_xscope_reference_or_pointer_capture_lambda([int1_xsptr]() { return *int1_xsptr; });
			mse::xscope_function<int()> xs_function3 = xs_lambda1;
		}

		mse::self_test::CFunctionTest1::s_test1();
	}

	{
		/********************/
		/*  legacy helpers  */
		/********************/

		{
			MSE_LH_DYNAMIC_ARRAY_ITERATOR_TYPE(int) iptrwbv1 = MSE_LH_ALLOC_DYN_ARRAY1(MSE_LH_DYNAMIC_ARRAY_ITERATOR_TYPE(int), 2 * sizeof(int));
			iptrwbv1[0] = 1;
			iptrwbv1[1] = 2;
			MSE_LH_DYNAMIC_ARRAY_ITERATOR_TYPE(int) iptrwbv2 = MSE_LH_REALLOC(int, iptrwbv1, 5 * sizeof(int));
			if (iptrwbv2) {
				auto res10 = iptrwbv2[0];
				auto res11 = iptrwbv2[1];
				auto res12 = iptrwbv2[2];
			}
		}

		{
			struct s1_type {
				MSE_LH_FIXED_ARRAY_DECLARATION(int, 3, nar11) = { 1, 2, 3 };
			} s1, s2;

			MSE_LH_FIXED_ARRAY_DECLARATION(int, 5, nar1) = { 1, 2, 3, 4, 5 };
			auto res14 = nar1[0];
			auto res15 = nar1[1];
			auto res16 = nar1[2];

			s2 = s1;

			s2.nar11[1] = 4;
			s1 = s2;
			auto res16b = s1.nar11[1];

			MSE_LH_ARRAY_ITERATOR_TYPE(int) naraiter1 = s1.nar11;
			auto res16c = naraiter1[1];
		}

		{
			MSE_LH_DYNAMIC_ARRAY_ITERATOR_TYPE(int) iptrwbv1 = MSE_LH_ALLOC_DYN_ARRAY1(MSE_LH_DYNAMIC_ARRAY_ITERATOR_TYPE(int), 2 * sizeof(int));
			iptrwbv1[0] = 1;
			iptrwbv1[1] = 2;

			MSE_LH_ARRAY_ITERATOR_TYPE(int) naraiter1;
			MSE_LH_ARRAY_ITERATOR_TYPE(int) naraiter2 = nullptr;
			MSE_LH_ARRAY_ITERATOR_TYPE(int) naraiter3 = iptrwbv1;
			naraiter1 = nullptr;
			naraiter1 = 0;
			naraiter1 = NULL;
			naraiter1 = naraiter2;
			naraiter1 = iptrwbv1;
			auto res17 = naraiter1[1];

			MSE_LH_FREE(iptrwbv1);
		}

		{
			typedef int dyn_arr2_element_type;
			MSE_LH_DYNAMIC_ARRAY_ITERATOR_TYPE(dyn_arr2_element_type) dyn_arr2;
			dyn_arr2 = MSE_LH_ALLOC_DYN_ARRAY1(MSE_LH_DYNAMIC_ARRAY_ITERATOR_TYPE(dyn_arr2_element_type), 64/*bytes*/);

			MSE_LH_MEMSET(dyn_arr2, 99, 64/*bytes*/);
			auto dyn_arr2b = dyn_arr2;

			MSE_LH_FREE(dyn_arr2);
		}

		{
			MSE_LH_POINTER_TYPE(int) ptr0 = MSE_LH_ALLOC_POINTER1(int);
			*ptr0 = 7;

			MSE_LH_POINTER_TYPE(int) ptr1;
			MSE_LH_POINTER_TYPE(int) ptr2 = nullptr;
			MSE_LH_POINTER_TYPE(int) ptr3 = ptr0;
			ptr1 = nullptr;
			ptr1 = 0;
			ptr1 = NULL;
			ptr1 = ptr2;
			ptr1 = ptr0;
			auto res17 = *ptr1;

			MSE_LH_FREE(ptr0);
		}

		{
			typedef int arr_element_type;
			MSE_LH_FIXED_ARRAY_DECLARATION(arr_element_type, 3/*elements*/, array1) = { 1, 2, 3 };
			MSE_LH_FIXED_ARRAY_DECLARATION(arr_element_type, 3/*elements*/, array2) = { 4, 5, 6 };

			MSE_LH_MEMSET(array1, 99, 3/*elements*/ * sizeof(arr_element_type));
			MSE_LH_MEMCPY(array2, array1, 3/*elements*/ * sizeof(arr_element_type));
			auto res18 = array2[1];
		}
	}
}


#ifdef __clang__
#pragma clang diagnostic pop
#else /*__clang__*/
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif /*__GNUC__*/
#endif /*__clang__*/

#ifdef _MSC_VER
#pragma warning( pop )  
#endif /*_MSC_VER*/

#else // !EXCLUDE_MSETL_EXAMPLE2
void msetl_example2() {}
#endif // !EXCLUDE_MSETL_EXAMPLE2
