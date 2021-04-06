
// Copyright (c) 2015 Noah Lopez
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef EXCLUDE_MSETL_EXAMPLE3

#include "msetl_example_defs.h"

#include "msetl_example3.h"

#include "mseasyncshared.h"
#include "msepoly.h"
#include "msemsearray.h"
#include "msemsevector.h"
#include "msemsestring.h"
#include "msestaticimmutable.h"
#include "msescopeatomic.h"

#include <list>
#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>
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

class K {
public:
	template<class _TAsyncSharedReadWriteAccessRequester>
	static double foo7(_TAsyncSharedReadWriteAccessRequester A_ashar) {
		auto t1 = std::chrono::high_resolution_clock::now();
		/* A_ashar.readlock_ptr() will block until it can obtain a read lock. */
		auto ptr1 = A_ashar.readlock_ptr(); // while ptr1 exists it holds a (read) lock on the shared object
		auto t2 = std::chrono::high_resolution_clock::now();
		std::this_thread::sleep_for(std::chrono::seconds(1));
		auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
		auto timespan_in_seconds = time_span.count();
		auto thread_id = std::this_thread::get_id();
		//std::cout << "thread_id: " << thread_id << ", time to acquire read pointer: " << timespan_in_seconds << " seconds.";
		//std::cout << std::endl;
		return timespan_in_seconds;
	}
	/* This function takes a "random access section" (which is like an "array_view" or gsl::span) as its parameter. */
	template<class _TStringRASection>
	static void foo8(_TStringRASection ra_section) {
		size_t delay_in_milliseconds = 3000/*arbitrary*/;
		if (1 <= ra_section.size()) {
			delay_in_milliseconds /= mse::as_a_size_t(ra_section.size());
		}
		for (size_t i = 0; i < ra_section.size(); i += 1) {
			auto now1 = std::chrono::system_clock::now();
			auto tt = std::chrono::system_clock::to_time_t(now1);

			/* Just trying to obtain a string with the current time and date. The standard library doesn't yet
			seem to provide a safe, portable way to do this. */
#ifdef _MSC_VER
			static const size_t buffer_size = 64;
			char buffer[buffer_size];
			buffer[0] = '\0';
			ctime_s(buffer, buffer_size, &tt);
#else /*_MSC_VER*/
			const auto buffer = ctime(&tt);
#endif /*_MSC_VER*/

			std::string now_str(buffer);
			ra_section[i] = now_str;

			std::this_thread::sleep_for(std::chrono::milliseconds(delay_in_milliseconds));
		}
	}
	/* This function just obtains a writelock_ra_section from the given "splitter access requester" and calls the given
	function with the writelock_ra_section as the first argument. */
	template<class TAsyncSplitterRASectionReadWriteAccessRequester, class TFunction, class... Args>
	static void invoke_with_writelock_ra_section1(TAsyncSplitterRASectionReadWriteAccessRequester ar, const TFunction function1, Args&&... args) {
		function1(ar.writelock_ra_section(), args...);
	}
	/* This function just obtains an xscope_random_access_section from the given access controlled pointer and calls the given
	function with the xscope_random_access_section as the first argument. */
	template<class Ty, class TFunction, class... Args>
	static void invoke_with_ra_section(mse::TXScopeExclusiveStrongPointerStoreForAccessControl<Ty> xs_ac_store, const TFunction function1, Args&&... args) {
		auto xscope_ra_section = mse::make_xscope_random_access_section(xs_ac_store.xscope_pointer());
		function1(xscope_ra_section, args...);
	}

	template<class _TAPointer>
	static void foo17(_TAPointer a_ptr) {
		{
			auto now1 = std::chrono::system_clock::now();
			auto tt = std::chrono::system_clock::to_time_t(now1);

			/* Just trying to obtain a string with the current time and date. The standard library doesn't yet
			seem to provide a safe, portable way to do this. */
#ifdef _MSC_VER
			static const size_t buffer_size = 64;
			char buffer[buffer_size];
			buffer[0] = '\0';
			ctime_s(buffer, buffer_size, &tt);
#else /*_MSC_VER*/
			const auto buffer = ctime(&tt);
#endif /*_MSC_VER*/

			std::string now_str(buffer);
			a_ptr->s = now_str;

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
	template<class _TAPointer>
	static void foo17b(_TAPointer a_ptr) {
		static std::atomic<int> s_count(0);
		s_count += 1;
		a_ptr->s = std::to_string(s_count);

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	template<class _TConstPointer, class _TPointer>
	static void foo18(_TConstPointer src_ptr, _TPointer dst_ptr) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		dst_ptr->s = src_ptr->s;
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

};


void msetl_example3() {
	{
		/********************/
		/*  TAsyncSharedV2  */
		/********************/

		/* The TAsyncSharedV2 data types are used to safely share objects between asynchronous threads. */

		class A {
		public:
			A(int x) : b(x) {}
			virtual ~A() {}

			int b = 3;
			mse::nii_string s = "some text ";
		};
		/* User-defined classes need to be declared as (safely) shareable in order to be accepted by the access requesters. */
		typedef mse::rsv::TAsyncShareableAndPassableObj<A> ShareableA;

		/* trivially copyable class */
		class D {
		public:
			D(int x) : b(x) {}

			int b = 3;
		};
		/* User-defined classes need to be declared as (safely) shareable in order to be used with the atomic templates. */
		typedef mse::rsv::TAsyncShareableAndPassableObj<D> ShareableD;

		class B {
		public:
			static double foo1(mse::TAsyncSharedV2ReadWriteAccessRequester<ShareableA> A_ashar) {
				auto t1 = std::chrono::high_resolution_clock::now();
				/* mse::TAsyncSharedV2ReadWriteAccessRequester<ShareableA>::writelock_ptr() will block until it can obtain a write lock. */
				auto ptr1 = A_ashar.writelock_ptr(); // while ptr1 exists it holds a (write) lock on the shared object
				auto t2 = std::chrono::high_resolution_clock::now();
				std::this_thread::sleep_for(std::chrono::seconds(1));
				auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
				auto timespan_in_seconds = time_span.count();
				auto thread_id = std::this_thread::get_id();
				//std::cout << "thread_id: " << thread_id << ", time to acquire write pointer: " << timespan_in_seconds << " seconds.";
				//std::cout << std::endl;

				ptr1->s = std::to_string(timespan_in_seconds);
				return timespan_in_seconds;
			}
			static int foo2(mse::TAsyncSharedV2ImmutableFixedPointer<ShareableA> A_immptr) {
				return A_immptr->b;
			}
			static int foo3(mse::TAsyncSharedV2AtomicFixedPointer<ShareableD> D_atomic_ptr) {
				auto d = (*D_atomic_ptr).load();
				d.b += 1;
				(*D_atomic_ptr).store(d);
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				return (*D_atomic_ptr).load().b;
			}
			static int foo4(mse::TXScopeAtomicFixedPointer<ShareableD> xs_D_atomic_ptr) {
				auto d = (*xs_D_atomic_ptr).load();
				d.b += 1;
				(*xs_D_atomic_ptr).store(d);
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				return (*xs_D_atomic_ptr).load().b;
			}
		protected:
			~B() {}
		};

		std::cout << std::endl;
		std::cout << "AsyncSharedV2 test output:";
		std::cout << std::endl;

		{
			/* This block contains a simple example demonstrating the use of mse::TAsyncSharedV2ReadWriteAccessRequester
			to safely share an object between threads. */

			std::cout << "TAsyncSharedV2ReadWriteAccessRequester:";
			std::cout << std::endl;
			auto ash_access_requester = mse::make_asyncsharedv2readwrite<ShareableA>(7);
			ash_access_requester.writelock_ptr()->b = 11;
			int res1 = ash_access_requester.readlock_ptr()->b;

			{
				auto ptr3 = ash_access_requester.readlock_ptr();
				auto ptr1 = ash_access_requester.writelock_ptr();
				auto ptr2 = ash_access_requester.writelock_ptr();
			}

			std::list<mse::mstd::future<double>> futures;
			for (size_t i = 0; i < 3; i += 1) {
				futures.emplace_back(mse::mstd::async(B::foo1, ash_access_requester));
			}
			int count = 1;
			for (auto it = futures.begin(); futures.end() != it; it++, count++) {
				std::cout << "thread: " << count << ", time to acquire read pointer: " << (*it).get() << " seconds.";
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
		{
			std::cout << "TAsyncSharedV2ReadOnlyAccessRequester:";
			std::cout << std::endl;
			auto ash_access_requester = mse::make_asyncsharedv2readonly<ShareableA>(7);
			int res1 = ash_access_requester.readlock_ptr()->b;

			std::list<mse::mstd::future<double>> futures;
			for (size_t i = 0; i < 3; i += 1) {
				futures.emplace_back(mse::mstd::async(K::foo7<mse::TAsyncSharedV2ReadOnlyAccessRequester<ShareableA>>, ash_access_requester));
			}
			int count = 1;
			for (auto it = futures.begin(); futures.end() != it; it++, count++) {
				std::cout << "thread: " << count << ", time to acquire read pointer: " << (*it).get() << " seconds.";
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
#ifndef EXCLUDE_DUE_TO_MSVC2019_INTELLISENSE_BUGS1
		{
			/* Just demonstrating the existence of the "try" versions. */
			auto access_requester = mse::make_asyncsharedv2readwrite<mse::nii_string>("some text");
			auto writelock_ptr1 = access_requester.try_writelock_ptr();
			if (writelock_ptr1) {
				// lock request succeeded
				int q = 5;
			}
			auto readlock_ptr2 = access_requester.try_readlock_ptr_for(std::chrono::seconds(1));
			auto writelock_ptr3 = access_requester.try_writelock_ptr_until(std::chrono::steady_clock::now() + std::chrono::seconds(1));
		}
#endif // !EXCLUDE_DUE_TO_MSVC2019_INTELLISENSE_BUGS1
		{
			/* TAsyncSharedV2WeakReadWriteAccessRequester<> is the weak counterpart to TAsyncSharedV2ReadWriteAccessRequester<>
			analogous to how std::weak_ptr<> is the weak counterpart to std::shared_ptr<>. */

			typedef decltype(mse::make_asyncsharedv2readwrite<mse::nii_string>("abc")) access_requester_t;
			auto vec1 = mse::mstd::vector<access_requester_t>();
			vec1.push_back(mse::make_asyncsharedv2readwrite<mse::nii_string>("abc"));

			mse::TAsyncSharedV2WeakReadWriteAccessRequester<mse::nii_string> weak_ar1(vec1.at(0));

			/* Here we're obtaining a (strong) access requester from the weak access requester, then appending it the
			vector of access requesters. */
			vec1.push_back(weak_ar1.try_strong_access_requester().value());

			assert((*(vec1.at(1).readlock_ptr())) == "abc");

			vec1.clear();

			/* All the (strong) access requesters have just been destroyed so attempting to obtain a (strong) access requester
			from our weak one will result in an empty optional being returned. */
			assert(!(weak_ar1.try_strong_access_requester().has_value()));
		}
		{
			/* For scenarios where the shared object is immutable (i.e. is never modified), you can get away without using locks
			or access requesters. */
			auto A_immptr = mse::make_asyncsharedv2immutable<ShareableA>(5);
			int res1 = A_immptr->b;
			std::shared_ptr<const ShareableA> A_shptr(A_immptr);

			std::list<mse::mstd::future<int>> futures;
			for (size_t i = 0; i < 3; i += 1) {
				futures.emplace_back(mse::mstd::async(B::foo2, A_immptr));
			}
			int count = 1;
			for (auto it = futures.begin(); futures.end() != it; it++, count++) {
				int res2 = (*it).get();
			}

			auto A_b_safe_cptr = mse::make_const_pointer_to_member_v2(A_immptr, &A::b);
		}
		{
			/* For scenarios where the shared object is atomic, you can get away without using locks
			or access requesters. */
			auto D_atomic_ptr = mse::make_asyncsharedv2atomic<ShareableD>(5);
			int res1 = (*D_atomic_ptr).load().b;

			std::list<mse::mstd::future<int>> futures;
			for (size_t i = 0; i < 3; i += 1) {
				futures.emplace_back(mse::mstd::async(B::foo3, D_atomic_ptr));
			}
			int count = 1;
			for (auto it = futures.begin(); futures.end() != it; it++, count++) {
				int res2 = (*it).get();
			}
		}
		{
			/* mse::TAsyncSharedV2ReadWriteAccessRequester's flexibilty in allowing coexisting read and write lock
			pointers in the same thread introduces new ways to produce logical deadlocks. This block (likely)
			demonstrates the access requester's ability to detect these potential deadlocks (and throw an exception
			when they would occur). */

			std::cout << "TAsyncSharedV2ReadWriteAccessRequester deadlock detection:";
			std::cout << std::endl;

			class CC {
			public:
				static void foo1(mse::TAsyncSharedV2ReadWriteAccessRequester<ShareableA> A_ashar, int id) {
					auto readlock_ptr = A_ashar.readlock_ptr();
					std::this_thread::sleep_for(std::chrono::seconds(1));
					MSE_TRY {
						auto writelock_ptr = A_ashar.writelock_ptr();
						std::this_thread::sleep_for(std::chrono::seconds(1));
					}
					MSE_CATCH_ANY {
						// likely exception due to potential deadlock
						std::cout << "deadlock detected ";
						std::cout << std::endl;
					}
				}
			};

			auto ash_access_requester = mse::make_asyncsharedv2readwrite<ShareableA>(7);

			{
				auto thread1 = mse::mstd::thread(CC::foo1, ash_access_requester, 1);
				auto thread2 = mse::mstd::thread(CC::foo1, ash_access_requester, 2);
				thread1.join();
				thread2.join();
			}

			std::cout << std::endl;
		}
		{
			/* This block demonstrates safely allowing different threads to (simultaneously) modify different
			sections of a vector. (We use vectors in this example, but it works just as well with arrays.) */

			MSE_DECLARE_STATIC_IMMUTABLE(size_t) num_sections = 10;
			MSE_DECLARE_STATIC_IMMUTABLE(size_t) section_size = 5;
			const size_t num_elements = num_sections * section_size;

			typedef mse::nii_vector<mse::nii_string> async_shareable_vector1_t;
			typedef mse::mstd::vector<mse::nii_string> nonshareable_vector1_t;
			/* Let's say we have a vector. */
			nonshareable_vector1_t vector1;
			vector1.resize(num_elements);
			{
				size_t count = 0;
				for (auto iter = vector1.begin(); vector1.end() != iter; iter++) {
					count += 1;
					*iter = "text" + std::to_string(count);
				}
			}

			/* Only access controlled objects can be shared with other threads, so we'll make an access controlled vector and
			(temporarily) swap it with our original one. */
			auto ash_access_requester = mse::make_asyncsharedv2readwrite<async_shareable_vector1_t>();
			std::swap(vector1, (*(ash_access_requester.writelock_ptr())));

			std::cout << "mse::TAsyncRASectionSplitter<>, part 1: " << std::endl;

			{
				/* Now, we're going to use the access requester to obtain two new access requesters that provide access to
				(newly created) "random access section" objects which are used to access (disjoint) sections of the vector.
				We need to specify the position where we want to split the vector. Here we specify that it be split at index
				"num_elements / 2", right down the middle. */
				mse::TAsyncRASectionSplitter<decltype(ash_access_requester)> ra_section_split1(ash_access_requester, num_elements / 2);
				auto ar1 = ra_section_split1.first_ra_section_access_requester();
				auto ar2 = ra_section_split1.second_ra_section_access_requester();

				/* The K::foo8 template function is just an example function that operates on containers of strings. In our case the
				containers will be the random access sections we just created. We'll create an instance of the function here. */
				const auto my_foo8_function = K::foo8<decltype(ar1.writelock_ra_section())>;

				/* We want to execute the my_foo8 function in a separate thread. The function takes a "random access section"
				as an argument. But as we're not allowed to pass random access sections between threads, we must pass an
				access requester instead. The "K::invoke_with_writelock_ra_section1" template function is just a helper
				function that will obtain a (writelock) random access section from the access requester, then call the given
				function, in this case my_foo8, with that random access section. So here we'll use it to create a proxy
				function that we can execute directly in a separate thread and will accept an access requester as a
				parameter. */
				const auto my_foo8_proxy_function = K::invoke_with_writelock_ra_section1<decltype(ar1), decltype(my_foo8_function)>;

				std::list<mse::mstd::thread> threads;
				/* So this thread will modify the first section of the vector. */
				threads.emplace_back(mse::mstd::thread(my_foo8_proxy_function, ar1, my_foo8_function));
				/* While this thread modifies the other section. */
				threads.emplace_back(mse::mstd::thread(my_foo8_proxy_function, ar2, my_foo8_function));

				{
					int count = 1;
					for (auto it = threads.begin(); threads.end() != it; it++, count++) {
						(*it).join();
					}
				}
				int q = 5;
			}
			std::cout << "mse::TAsyncRASectionSplitter<>, part 2: " << std::endl;
			{
				/* Ok, now let's do it again, but instead of splitting the vector into two sections, let's split it into more sections: */
				/* First we create a list of a the sizes of each section. We'll use a vector here, but any iteratable container will work. */
				mse::mstd::vector<size_t> section_sizes;
				for (size_t i = 0; i < num_sections; i += 1) {
					section_sizes.push_back(section_size);
				}

				/* Just as before, TAsyncRASectionSplitter<> will generate a new access requester for each section. */
				mse::TAsyncRASectionSplitter<decltype(ash_access_requester)> ra_section_split1(ash_access_requester, section_sizes);
				auto ar0 = ra_section_split1.ra_section_access_requester(0);

				const auto my_foo8_function = K::foo8<decltype(ar0.writelock_ra_section())>;
				const auto my_foo8_proxy_function = K::invoke_with_writelock_ra_section1<decltype(ar0), decltype(my_foo8_function)>;

				{
					/* Here we demonstrate scope threads. Scope threads don't support being copied or moved. Unlike mstd::thread,
					scope threads can share objects declared on the stack (which is not utilized here), and in their destructor,
					scope thread objects will wait until their thread finishes its execution (i.e "join" the thread), blocking if
					necessary. Often rather than declaring scope thread objects directly, you'll create and manage multiple scope
					threads with an "xscope_thread_carrier". An xscope_thread_carrier is just a container that holds scope
					threads. */
					mse::xscope_thread_carrier xscope_threads;
					for (size_t i = 0; i < num_sections; i += 1) {
						auto ar = ra_section_split1.ra_section_access_requester(i);
						xscope_threads.new_thread(my_foo8_proxy_function, ar, my_foo8_function);
					}
					/* The scope will not end until all the scope threads have finished executing. */
				}
			}

			/* Now that we're done sharing the (controlled access) vector, we can swap it back to our original vector. */
			std::swap(vector1, (*(ash_access_requester.writelock_ptr())));
			auto first_element_value = vector1[0];
			auto last_element_value = vector1.back();

			int q = 5;
		}
		{
			/* Here we demonstrate safely sharing an existing stack allocated object among threads. */

			std::cout << "xscope_future_carrier<>: " << std::endl;

			/* (Mutable) objects can be shared between threads only if they are "access controlled". You can make an
			object "access controlled" by wrapping its type with the mse::TXScopeAccessControlledObj<> template wrapper. */
			mse::TXScopeObj<mse::TXScopeAccessControlledObj<ShareableA> > a_xscpacobj(7);

			/* Here we obtain a scope access requester for the access controlled object. */
			auto xscope_access_requester = mse::make_xscope_asyncsharedv2acoreadwrite(&a_xscpacobj);

			/* xscope_future_carrier<> is just a container that holds and manages scope futures. */
			mse::xscope_future_carrier<double> xscope_futures;

			std::list<mse::xscope_future_carrier<double>::handle_t> future_handles;
			for (size_t i = 0; i < 3; i += 1) {
				/* You add a future by specifying the async() function and parameters that will return the future value. */
				auto handle = xscope_futures.new_future(K::foo7<decltype(xscope_access_requester)>, xscope_access_requester);

				/* You need to store the handle of the added future in order to later retrieve its value. */
				future_handles.emplace_back(handle);
			}
			int count = 1;
			for (auto it = future_handles.begin(); future_handles.end() != it; it++, count++) {
				std::cout << "thread: " << count << ", time to acquire read pointer: " << xscope_futures.xscope_ptr_at(*it)->get() << " seconds.";
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}

		{
			mse::TXScopeObj<mse::TXScopeAccessControlledObj<ShareableA> > a_xscpacobj1(3);
			mse::TXScopeObj<mse::TXScopeAccessControlledObj<ShareableA> > a_xscpacobj2(5);
			mse::TXScopeObj<mse::TXScopeAccessControlledObj<ShareableA> > a_xscpacobj3(7);

			{
				std::cout << "mse::make_xscope_aco_locker_for_sharing(): " << std::endl;

				/* The mse::make_xscope_aco_locker_for_sharing() function takes a scope pointer to an "access controlled object"
				and returns a "locker" object which then holds an exclusive reference to the given access controlled
				object. From this locker object, you can obtain either one "scope passable" (non-const) pointer, or
				any number of "scope passable" const pointers. These scope passable pointers can then be safely
				passed directly as arguments to scope threads. This is a (little) more cumbersome, more restrictive
				way of sharing an object than, say, using the library's "access requesters". So generally using
				access requesters would be preferred. But you might choose to do it this way in certain cases where
				performance is critical. When using access requesters, each thread obtains the desired lock on a
				thread-safe mutex. Here we're obtaining the lock before launching the thread(s), so the mutex does
				not need to be thread-safe, thus saving a little overhead. */
				auto xscope_aco_locker1 = mse::make_xscope_aco_locker_for_sharing(&a_xscpacobj1);

				typedef decltype(xscope_aco_locker1.xscope_passable_pointer()) passable_exclusive_pointer_t;
				mse::xscope_thread xscp_thread1(K::foo17b<passable_exclusive_pointer_t>, xscope_aco_locker1.xscope_passable_pointer());
			}
			{
				auto xscope_aco_locker1 = mse::make_xscope_aco_locker_for_sharing(&a_xscpacobj1);
				auto xscope_aco_locker2 = mse::make_xscope_aco_locker_for_sharing(&a_xscpacobj2);
				auto xscope_aco_locker3 = mse::make_xscope_aco_locker_for_sharing(&a_xscpacobj3);

				typedef decltype(xscope_aco_locker1.xscope_passable_const_pointer()) passable_const_pointer_t;
				typedef decltype(xscope_aco_locker2.xscope_passable_pointer()) passable_exclusive_pointer_t;

				mse::xscope_thread xscp_thread1(K::foo18<passable_const_pointer_t, passable_exclusive_pointer_t>
					, xscope_aco_locker1.xscope_passable_const_pointer()
					, xscope_aco_locker2.xscope_passable_pointer());

				mse::xscope_thread xscp_thread2(K::foo18<passable_const_pointer_t, passable_exclusive_pointer_t>
					, xscope_aco_locker1.xscope_passable_const_pointer()
					, xscope_aco_locker3.xscope_passable_pointer());
			}
			{
				auto xscope_aco_locker1 = mse::make_xscope_aco_locker_for_sharing(&a_xscpacobj1);

				/* The mse::make_xscope_exclusive_strong_pointer_store_for_sharing() function returns the same kind of "locker" object that
				mse::make_xscope_aco_locker_for_sharing() does, but instead of taking a scope pointer to an "access controlled object", it
				accepts any recognized "exclusive" pointer. That is, a pointer that, while it exists, holds exclusive access to
				its target object. */
				auto xscope_strong_ptr_store1 = mse::make_xscope_exclusive_strong_pointer_store_for_sharing(xscope_aco_locker1.xscope_passable_pointer());

				auto xscope_aco_locker2 = mse::make_xscope_aco_locker_for_sharing(&a_xscpacobj2);
				auto xscope_aco_locker3 = mse::make_xscope_aco_locker_for_sharing(&a_xscpacobj3);

				typedef decltype(xscope_aco_locker1.xscope_passable_const_pointer()) passable_const_pointer_t;
				typedef decltype(xscope_aco_locker2.xscope_passable_pointer()) passable_exclusive_pointer_t;

				mse::xscope_thread xscp_thread1(K::foo18<passable_const_pointer_t, passable_exclusive_pointer_t>
					, xscope_strong_ptr_store1.xscope_passable_const_pointer()
					, xscope_aco_locker2.xscope_passable_pointer());

				mse::xscope_thread xscp_thread2(K::foo18<passable_const_pointer_t, passable_exclusive_pointer_t>
					, xscope_strong_ptr_store1.xscope_passable_const_pointer()
					, xscope_aco_locker3.xscope_passable_pointer());
			}
			{
				/* In this block we demonstrate obtaining various types of (const and non-const) pointers you might need from
				an exclusive pointer that might be passed to a thread. */

				std::cout << "mse::TXScopeExclusiveStrongPointerStoreForAccessControl<>: " << std::endl;

				a_xscpacobj1.pointer()->s = "";

				auto xscope_aco_locker1 = mse::make_xscope_aco_locker_for_sharing(&a_xscpacobj1);

				typedef decltype(xscope_aco_locker1.xscope_passable_pointer()) passable_exclusive_pointer_t;
				typedef decltype(xscope_aco_locker1.xscope_passable_const_pointer()) passable_const_pointer_t;

				class CD {
				public:
					/* mse::TXScopeExclusiveStrongPointerStoreForAccessControl<> is a data type that stores an exclusive strong
					pointer. From this data type you can obtain const, non-const and exclusive pointers. 
					So this function expects to be passed an (rvlaue) pointer of type passable_exclusive_pointer_t. */
					static void foo1(mse::TXScopeExclusiveStrongPointerStoreForAccessControl<passable_exclusive_pointer_t> xscope_store, int count) {
						{
							auto xsptr = xscope_store.xscope_pointer();
							xsptr->s.append(std::to_string(count));
						}
						{
							/* Here, from the exclusive (non-const) pointer passed to this function, we're going to obtain a couple
							of const pointers that we can pass to different (scope) threads. */
							auto xscope_strong_ptr_store1 = mse::make_xscope_exclusive_strong_pointer_store_for_sharing(xscope_store.xscope_exclusive_pointer());

							mse::xscope_thread xscp_thread1(CD::foo2, xscope_strong_ptr_store1.xscope_passable_const_pointer());
							mse::xscope_thread xscp_thread2(CD::foo2, xscope_strong_ptr_store1.xscope_passable_const_pointer());
						}
						if (1 <= count) {
							/* And here we're going to (re)obtain an exclusive strong pointer like the one that was passed to this
							function, then we're going to use it to recursively call this function again in another (scope) thread. */
							auto xscope_strong_ptr_store1 = mse::make_xscope_exclusive_strong_pointer_store_for_sharing(xscope_store.xscope_exclusive_pointer());
							mse::xscope_thread xscp_thread1(CD::foo1, xscope_strong_ptr_store1.xscope_passable_pointer(), count - 1);
						}
					}
					static void foo2(passable_const_pointer_t xscope_A_cptr) {
						std::cout << xscope_A_cptr->s << std::endl;
					}
				};

				mse::xscope_thread xscp_thread1(CD::foo1, xscope_aco_locker1.xscope_passable_pointer(), 3);

				std::cout << std::endl;
			}
		}

		{
			/* TExclusiveWriterObj<> is a specialization of TAccessControlledObj<> for which all non-const pointers are
			exclusive. That is, when a non-const pointer exists, no other pointer may exist. */
			mse::TXScopeObj<mse::TExclusiveWriterObj<ShareableA> > a_xscpxwobj1(3);
			mse::TXScopeObj<mse::TExclusiveWriterObj<ShareableA> > a_xscpxwobj2(5);
			mse::TXScopeObj<mse::TExclusiveWriterObj<ShareableA> > a_xscpxwobj3(7);

			{
				/* A (non-const) pointer of an "exclusive writer object" qualifies as an "exclusive strong" pointer, and
				thus you can obtain an xscope shareable pointer from it in the standard way. */
				auto xscope_xwo_pointer_store1 = mse::make_xscope_exclusive_strong_pointer_store_for_sharing(a_xscpxwobj1.pointer());

				typedef decltype(xscope_xwo_pointer_store1.xscope_passable_pointer()) passable_exclusive_pointer_t;
				mse::xscope_thread xscp_thread1(K::foo17b<passable_exclusive_pointer_t>, xscope_xwo_pointer_store1.xscope_passable_pointer());
			}
			{
				/* But uniquely, you can obtain an xscope shareable const pointer from a (non-exclusive) const pointer of an
				"exclusive writer object". There is a special function for this purpose: */
				auto xscope_xwo_const_pointer_store1 = mse::make_xscope_exclusive_write_obj_const_pointer_store_for_sharing(a_xscpxwobj1.const_pointer());

				auto xscope_xwo_pointer_store2 = mse::make_xscope_exclusive_strong_pointer_store_for_sharing(a_xscpxwobj2.pointer());
				auto xscope_xwo_pointer_store3 = mse::make_xscope_exclusive_strong_pointer_store_for_sharing(a_xscpxwobj3.pointer());

				typedef decltype(xscope_xwo_const_pointer_store1.xscope_passable_const_pointer()) passable_const_pointer_t;
				typedef decltype(xscope_xwo_pointer_store2.xscope_passable_pointer()) passable_exclusive_pointer_t;

				mse::xscope_thread xscp_thread1(K::foo18<passable_const_pointer_t, passable_exclusive_pointer_t>
					, xscope_xwo_const_pointer_store1.xscope_passable_const_pointer()
					, xscope_xwo_pointer_store2.xscope_passable_pointer());

				mse::xscope_thread xscp_thread2(K::foo18<passable_const_pointer_t, passable_exclusive_pointer_t>
					, xscope_xwo_const_pointer_store1.xscope_passable_const_pointer()
					, xscope_xwo_pointer_store3.xscope_passable_pointer());
			}
		}

		{
			/* For scenarios where the shared object is atomic, you can get away without using locks
			or access requesters. */
			mse::TXScopeAtomicObj<ShareableD> xscope_D_atomic_obj(7);
			int res1 = xscope_D_atomic_obj.load().b;

			auto xscope_D_atomic_ptr = &xscope_D_atomic_obj;

			mse::xscope_thread xscp_thread1(B::foo4, xscope_D_atomic_ptr);
			mse::xscope_thread xscp_thread2(B::foo4, xscope_D_atomic_ptr);

			int res2 = (*xscope_D_atomic_ptr).load().b;
		}

#ifndef EXCLUDE_DUE_TO_MSVC2019_INTELLISENSE_BUGS1
		{
			/* This block is similar to a previous one that demonstrates safely allowing different threads to (simultaneously)
			modify different sections of a vector. The difference is just that here the shared vector is a pre-existing one
			declared as a local variable. */

			MSE_DECLARE_STATIC_IMMUTABLE(size_t) num_sections = 10;
			MSE_DECLARE_STATIC_IMMUTABLE(size_t) section_size = 5;
			const size_t num_elements = num_sections * section_size;

			typedef mse::nii_vector<mse::nii_string> async_shareable_vector1_t;
			typedef mse::mstd::vector<mse::nii_string> nonshareable_vector1_t;
			/* Let's say we have a vector. */
			nonshareable_vector1_t vector1;
			vector1.resize(num_elements);
			{
				size_t count = 0;
				for (auto iter = vector1.begin(); vector1.end() != iter; iter++) {
					count += 1;
					*iter = "text" + std::to_string(count);
				}
			}

			/* Only access controlled objects can be shared with other threads, so we'll make an access controlled vector and
			(temporarily) swap it with our original one. */
			mse::TXScopeObj<mse::TXScopeAccessControlledObj<async_shareable_vector1_t> > xscope_acobj;
			std::swap(vector1, *(xscope_acobj.xscope_pointer()));

			std::cout << "mse::TXScopeACORASectionSplitter<>: " << std::endl;

			{
				/* From the access controlled vector we're going to obtain scope pointers to two access controlled
				"random access section" objects which are used to access (disjoint) sections of the vector. Then we're
				going to use "locker" objects to obtain pointers that are designated as safe to pass to other threads.
				We need to specify the position  where we want to split the vector. Here we specify that it be split
				at index "num_elements / 2", right down the middle. */
				mse::TXScopeACORASectionSplitter<async_shareable_vector1_t> xscope_ra_section_split2(&xscope_acobj, num_elements / 2);

				/* Here we obtain scope pointers to the access controlled sections. */
				auto first_ra_section_aco_xsptr = xscope_ra_section_split2.xscope_ptr_to_first_ra_section_aco();
				auto second_ra_section_aco_xsptr = xscope_ra_section_split2.xscope_ptr_to_second_ra_section_aco();

				/* Access controlled object pointers aren't themselves passable to other threads, but we can obtain
				corresponding pointers that are passable via a "locker" object that takes exclusive control over
				the access controlled object. */
#ifndef _MSC_VER
				auto xscope_section_access_owner1 = mse::make_xscope_aco_locker_for_sharing(first_ra_section_aco_xsptr);
				auto xscope_section_access_owner2 = mse::make_xscope_aco_locker_for_sharing(second_ra_section_aco_xsptr);
#else // !_MSC_VER
				/* An apparent bug in msvc2017 and msvc2019preview (March 2019) prevents the other branch from compiling.
				This presents an opportunity to demonstrate an alternative solution. Instead of a "locker" object that
				takes a scope pointer to the access controlled object, we can use an "exclusive strong pointer store"
				which takes any recognized exclusive pointer. */
				auto xscope_section_access_owner1 = mse::make_xscope_exclusive_strong_pointer_store_for_sharing(
					(*first_ra_section_aco_xsptr).xscope_exclusive_pointer());
				auto xscope_section_access_owner2 = mse::make_xscope_exclusive_strong_pointer_store_for_sharing(
					(*second_ra_section_aco_xsptr).xscope_exclusive_pointer());
#endif // !_MSC_VER

				/* The K::foo8 template function is just an example function that operates on containers of strings. In this case the
				containers will be the random access sections we just created. We'll create an instance of the function here. */
				const auto my_foo8_function = K::foo8<mse::TXScopeAnyRandomAccessSection<mse::nii_string> >;
				typedef decltype(my_foo8_function) my_foo8_function_type;

				mse::xscope_thread_carrier threads;
				/* So this thread will modify the first section of the vector. */
				threads.new_thread(K::invoke_with_ra_section<decltype(xscope_section_access_owner1.xscope_passable_pointer()), my_foo8_function_type>
					, xscope_section_access_owner1.xscope_passable_pointer(), my_foo8_function);
				/* While this thread modifies the other section. */
				threads.new_thread(K::invoke_with_ra_section<decltype(xscope_section_access_owner2.xscope_passable_pointer()), my_foo8_function_type>
					, xscope_section_access_owner2.xscope_passable_pointer(), my_foo8_function);

				/* Note that in this particular scenario we didn't need to use any access requesters (or (thread safe) locks). */
			}

			std::cout << "mse::TXScopeAsyncACORASectionSplitter<>: " << std::endl;

			{
				/* Ok, now let's do it again, but instead of splitting the vector into two sections, let's split it into more sections: */
				/* First we create a list of a the sizes of each section. We'll use a vector here, but any iteratable container will work. */
				mse::mstd::vector<size_t> section_sizes;
				for (size_t i = 0; i < num_sections; i += 1) {
					section_sizes.push_back(section_size);
				}

				/* This time (for demonstration purposes) we'll use TXScopeAsyncACORASectionSplitter<> to generate a new
				access requester for each section. */
				mse::TXScopeAsyncACORASectionSplitter<async_shareable_vector1_t> xscope_ra_section_split1(&xscope_acobj, section_sizes);
				auto ar0 = xscope_ra_section_split1.ra_section_access_requester(0);

				const auto my_foo8_function = K::foo8<decltype(ar0.writelock_ra_section())>;
				typedef decltype(my_foo8_function) my_foo8_function_type;
				const auto my_foo8_proxy_function = K::invoke_with_writelock_ra_section1<decltype(ar0), my_foo8_function_type>;

				{
					mse::xscope_thread_carrier xscope_threads;
					for (size_t i = 0; i < num_sections; i += 1) {
						auto ar = xscope_ra_section_split1.ra_section_access_requester(i);
						xscope_threads.new_thread(my_foo8_proxy_function, ar, my_foo8_function);
					}
					/* The scope will not end until all the scope threads have finished executing. */
				}
			}

			/* Now that we're done sharing the (controlled access) vector, we can swap it back to our original vector. */
			std::swap(vector1, *(xscope_acobj.xscope_pointer()));
			auto first_element_value = vector1[0];
			auto last_element_value = vector1.back();

			int q = 5;
		}
#endif // !EXCLUDE_DUE_TO_MSVC2019_INTELLISENSE_BUGS1
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

#else // !EXCLUDE_MSETL_EXAMPLE3
void msetl_example3() {}
#endif // !EXCLUDE_MSETL_EXAMPLE3
