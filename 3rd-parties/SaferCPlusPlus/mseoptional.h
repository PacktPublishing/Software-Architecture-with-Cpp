// Copyright (C) 2011 - 2012 Andrzej Krzemienski.
//
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// The idea and interface is based on Boost.Optional library
// authored by Fernando Luis Cacciola Carballal

#pragma once
# ifndef MSEOPTIONAL_H_
# define MSEOPTIONAL_H_

#ifndef MSE_OPTIONAL_NO_XSCOPE_DEPENDENCE
#include "msescope.h"
#endif // !MSE_OPTIONAL_NO_XSCOPE_DEPENDENCE
#include "msepointerbasics.h"

#include <atomic>

#include<utility>
#include<type_traits>
#include<initializer_list>
#include<cassert>
#include<functional>
#include<string>
#include<stdexcept>
#include <shared_mutex>
#include <mutex>
#ifdef MSE_HAS_CXX17
#include<optional>
#include <variant>
#endif // MSE_HAS_CXX17

#ifdef MSE_SELF_TESTS
#include <iostream>
#include <stdlib.h> // we include this after including iostream as a workaround for an apparent bug in libtooling8
#endif // MSE_SELF_TESTS


#ifdef _MSC_VER
#define MSE_OPTIONAL_IMPLEMENTATION1
#else // _MSC_VER
/* msvc(2017) requires this define, but we'll use it for non-msvc compilers as well for consistency. This may result in some
reduction of functionality/compatibility. */
#define MSE_OPTIONAL_IMPLEMENTATION1
#endif // _MSC_VER


#ifndef MSE_PUSH_MACRO_NOT_SUPPORTED
#pragma push_macro("MSE_THROW")
#pragma push_macro("_STD")
#pragma push_macro("_NOEXCEPT")
#pragma push_macro("_THROW_NCEE")
#pragma push_macro("_NODISCARD")
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

#ifndef _THROW_NCEE
#define _THROW_NCEE(x, y)	MSE_THROW(x(y))
#endif /*_THROW_NCEE*/

#ifndef _NODISCARD
#define _NODISCARD
#endif /*_NODISCARD*/

#ifdef _MSC_VER
#pragma warning( push )  
#pragma warning( disable : 4702 4189 )
#endif /*_MSC_VER*/


namespace mse {

	class structure_lock_violation_error : public std::logic_error {
	public:
		using std::logic_error::logic_error;
	};

	namespace impl {
		template<class _StateMutex>
		class destructor_lock_guard1 {
		public:
			explicit destructor_lock_guard1(_StateMutex& _Mtx) : _MyStateMutex(_Mtx) {
				MSE_TRY {
					_Mtx.lock();
				}
				MSE_CATCH_ANY {
					/* It may not be safe to continue if the object is destroyed while the object state is locked (and presumably
					in use) by another part of the code. */
#ifdef MSE_CUSTOM_FATAL_ERROR_MESSAGE_HANDLER
				MSE_CUSTOM_FATAL_ERROR_MESSAGE_HANDLER("Fatal Error: mse::destructor_lock_guard1() failed \n");
#endif // MSE_CUSTOM_FATAL_ERROR_MESSAGE_HANDLER
					assert(false); std::terminate();
				}
			}
			~destructor_lock_guard1() _NOEXCEPT {
				_MyStateMutex.unlock();
			}
			destructor_lock_guard1(const destructor_lock_guard1&) = delete;
			destructor_lock_guard1& operator=(const destructor_lock_guard1&) = delete;
		private:
			_StateMutex & _MyStateMutex;
		};
	}

	template<typename _TExclusiveLockIndicator = bool, typename _TSharedLockCounter = size_t>
	class T_shared_mutex {
	public:
		T_shared_mutex() {}
		void lock() {	// lock exclusive
			if (m_is_exclusive_locked || (1 <= m_shared_lock_count)) {
				MSE_THROW(std::system_error(std::make_error_code(std::errc::resource_deadlock_would_occur)));
			}
			m_is_exclusive_locked = true;
		}
		bool try_lock() {	// try to lock exclusive
			bool retval = true;
			if (m_is_exclusive_locked || (1 <= m_shared_lock_count)) {
				retval = false;
			}
			else {
				m_is_exclusive_locked = true;
			}
			return retval;
		}
		template<class _Rep, class _Period>
		bool try_lock_for(const std::chrono::duration<_Rep, _Period>& _Rel_time) {	// try to lock for duration
			return (try_lock_until(std::chrono::steady_clock::now() + _Rel_time));
		}
		template<class _Clock, class _Duration>
		bool try_lock_until(const std::chrono::time_point<_Clock, _Duration>& _Abs_time) {	// try to lock until time point
			return try_lock();
		}
		void unlock() {	// unlock exclusive
			assert(m_is_exclusive_locked && (0 == m_shared_lock_count));
			m_is_exclusive_locked = false;
		}
		void lock_shared() {	// lock non-exclusive
			if (m_is_exclusive_locked) {
				MSE_THROW(std::system_error(std::make_error_code(std::errc::resource_deadlock_would_occur)));
			}
			m_shared_lock_count += 1;
		}
		bool try_lock_shared() {	// try to lock non-exclusive
			bool retval = true;
			if (m_is_exclusive_locked) {
				retval = false;
			}
			else {
				m_shared_lock_count += 1;
			}
			return retval;
		}
		template<class _Rep, class _Period>
		bool try_lock_shared_for(const std::chrono::duration<_Rep, _Period>& _Rel_time) {	// try to lock non-exclusive for relative time
			return (try_lock_shared_until(_Rel_time + std::chrono::steady_clock::now()));
		}
		template<class _Clock, class _Duration>
		bool try_lock_shared_until(const std::chrono::time_point<_Clock, _Duration>& _Abs_time) {	// try to lock non-exclusive until absolute time
			return try_lock_shared();
		}
		void unlock_shared() {	// unlock non-exclusive
			assert((1 <= m_shared_lock_count) && (!m_is_exclusive_locked));
			m_shared_lock_count -= 1;
		}

		T_shared_mutex(const T_shared_mutex&) = delete;
		T_shared_mutex& operator=(const T_shared_mutex&) = delete;

	private:
		_TExclusiveLockIndicator m_is_exclusive_locked = { false };
		_TSharedLockCounter m_shared_lock_count = { 0 };
	};

	class non_thread_safe_mutex {
	public:
		non_thread_safe_mutex() {}
		void lock() {	// lock exclusive
			if (m_is_locked) {
				MSE_THROW(std::system_error(std::make_error_code(std::errc::resource_deadlock_would_occur)));
			}
			m_is_locked = true;
		}
		bool try_lock() {	// try to lock exclusive
			if (m_is_locked) {
				return false;
			}
			else
			{
				m_is_locked = true;
				return true;
			}
		}
		template<class _Rep, class _Period>
		bool try_lock_for(const std::chrono::duration<_Rep, _Period>& _Rel_time) {	// try to lock for duration
			return (try_lock_until(std::chrono::steady_clock::now() + _Rel_time));
		}
		template<class _Clock, class _Duration>
		bool try_lock_until(const std::chrono::time_point<_Clock, _Duration>& _Abs_time) {	// try to lock until time point
			return try_lock();
		}
		void unlock() {	// unlock exclusive
			assert(m_is_locked);
			m_is_locked = false;
		}

		non_thread_safe_mutex(const non_thread_safe_mutex&) = delete;
		non_thread_safe_mutex& operator=(const non_thread_safe_mutex&) = delete;

	private:
		bool m_is_locked = false;
	};

	class non_thread_safe_shared_mutex : public T_shared_mutex<> {
	public:
		typedef T_shared_mutex<> base_class;
		MSE_USING(non_thread_safe_shared_mutex, base_class);
	};

	/* The library's dynamic containers have a mutex used to "lock their structure". Containers that might be shared
	among threads need the mutex to be (at least partially) thread-safe. Specifically, the "read lock" needs to be
	atomic. */
	typedef T_shared_mutex<bool, std::atomic<size_t> > shareable_dynamic_container_mutex;

	class non_thread_safe_recursive_shared_timed_mutex : public non_thread_safe_shared_mutex {
	public:
		typedef non_thread_safe_shared_mutex base_class;

		void lock() {
			base_class::lock_shared();
		}
		bool try_lock() {
			return base_class::try_lock_shared();
		}
		template<class _Rep, class _Period>
		bool try_lock_for(const std::chrono::duration<_Rep, _Period>& _Rel_time) {
			return (try_lock_until(std::chrono::steady_clock::now() + _Rel_time));
		}
		template<class _Clock, class _Duration>
		bool try_lock_until(const std::chrono::time_point<_Clock, _Duration>& _Abs_time) {
			return base_class::try_lock_shared_until(_Abs_time);
		}
		void unlock() {
			base_class::unlock_shared();
		}

		void nonrecursive_lock() {
			base_class::lock();
	}
		bool try_nonrecursive_lock() {
			return base_class::try_lock();
		}
		template<class _Rep, class _Period>
		bool try_nonrecursive_lock_for(const std::chrono::duration<_Rep, _Period>& _Rel_time) {
			return (try_nonrecursive_lock_until(std::chrono::steady_clock::now() + _Rel_time));
		}
		template<class _Clock, class _Duration>
		bool try_nonrecursive_lock_until(const std::chrono::time_point<_Clock, _Duration>& _Abs_time) {
			return base_class::try_lock_until(_Abs_time);
		}
		void nonrecursive_unlock() {
			base_class::unlock();
		}
	};

	class dummy_recursive_shared_timed_mutex {
	public:
		dummy_recursive_shared_timed_mutex() {}
		void lock() {
		}
		bool try_lock() {
			return true;
		}
		template<class _Rep, class _Period>
		bool try_lock_for(const std::chrono::duration<_Rep, _Period>& _Rel_time) {	// try to lock for duration
			return (try_lock_until(std::chrono::steady_clock::now() + _Rel_time));
		}
		template<class _Clock, class _Duration>
		bool try_lock_until(const std::chrono::time_point<_Clock, _Duration>& _Abs_time) {	// try to lock until time point
			return try_lock();
		}
		void unlock() {
		}
		void nonrecursive_lock() {
			lock();
		}
		bool try_nonrecursive_lock() {	// try to lock nonrecursive
			return try_lock();
		}
		template<class _Rep, class _Period>
		bool try_nonrecursive_lock_for(const std::chrono::duration<_Rep, _Period>& _Rel_time) {	// try to nonrecursive lock for duration
			return (try_nonrecursive_lock_until(std::chrono::steady_clock::now() + _Rel_time));
		}
		template<class _Clock, class _Duration>
		bool try_nonrecursive_lock_until(const std::chrono::time_point<_Clock, _Duration>& _Abs_time) {	// try to nonrecursive lock until time point
			return try_lock_until(_Abs_time);
		}
		void nonrecursive_unlock() {
			unlock();
		}
		void lock_shared() {	// lock non-exclusive
		}
		bool try_lock_shared() {	// try to lock non-exclusive
			return true;
		}
		template<class _Rep, class _Period>
		bool try_lock_shared_for(const std::chrono::duration<_Rep, _Period>& _Rel_time) {	// try to lock non-exclusive for relative time
			return (try_lock_shared_until(_Rel_time + std::chrono::steady_clock::now()));
		}
		template<class _Clock, class _Duration>
		bool try_lock_shared_until(const std::chrono::time_point<_Clock, _Duration>& _Abs_time) {	// try to lock non-exclusive until absolute time
			return try_lock_shared();
		}
		void unlock_shared() {	// unlock non-exclusive
		}

		dummy_recursive_shared_timed_mutex(const dummy_recursive_shared_timed_mutex&) = delete;
		dummy_recursive_shared_timed_mutex& operator=(const dummy_recursive_shared_timed_mutex&) = delete;
	};

	typedef
#if (!defined(NDEBUG)) || (!defined(MSE_DISABLE_REENTRANCY_CHECKS_BY_DEFAULT))
		non_thread_safe_mutex
#else // (!defined(NDEBUG)) || (!defined(MSE_DISABLE_REENTRANCY_CHECKS_BY_DEFAULT))
		dummy_recursive_shared_timed_mutex
#endif // (!defined(NDEBUG)) || (!defined(MSE_DISABLE_REENTRANCY_CHECKS_BY_DEFAULT))
		default_state_mutex;

	/* To protect against potentially misbehaving/malicious user defined element constructors/destructors, some of the containers'
	member functions/operations are protected by a (non-thread-safe) mutex. If the element type's constructors/destructors are
	"trivial", then such protection is unnecessary, and so a "dummy"/no-op mutex is be used. */
	template<class _Ty>
	using container_adjusted_default_state_mutex = mse::impl::conditional_t<
		std::is_trivially_constructible<_Ty>::value&& std::is_trivially_destructible<_Ty>::value
		&& ((!std::is_assignable<_Ty, _Ty>::value) || std::is_trivially_assignable<_Ty, _Ty>::value)
		, dummy_recursive_shared_timed_mutex, default_state_mutex>;

	namespace us {
		namespace impl {

			template<class _Mutex>
			class copyable_shared_lock : public std::shared_lock<_Mutex> {
			public:
				typedef std::shared_lock<_Mutex> base_class;

				using base_class::base_class;
				copyable_shared_lock(const copyable_shared_lock& _Other) {
					if (_Other.mutex()) {
						base_class::operator=(base_class(*(_Other.mutex())));
					}
				}
				copyable_shared_lock(copyable_shared_lock&& _Other) noexcept : base_class(MSE_FWD(_Other)) {}

				copyable_shared_lock& operator=(const copyable_shared_lock& _Right) {
					if (_Right.mutex()) {
						base_class::operator=(base_class(*(_Right.mutex())));
					}
					else {
						base_class::operator=(base_class());
					}
					return (*this);
				}
				copyable_shared_lock& operator=(copyable_shared_lock&& _Right) noexcept {
					base_class::operator=(MSE_FWD(_Right));
					return (*this);
				}
			};
		}
	}

	namespace impl {
		template<class T, class EqualTo>
		struct HasOrInheritsNonrecursiveUnlockMethod_msemsearray_impl
		{
			template<class U, class V>
			static auto test(U*) -> decltype(std::declval<U>().nonrecursive_unlock(), bool(true));
			template<typename, typename>
			static auto test(...)->std::false_type;

			using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
		};
		template<class T, class EqualTo = T>
		struct HasOrInheritsNonrecursiveUnlockMethod_msemsearray : HasOrInheritsNonrecursiveUnlockMethod_msemsearray_impl<
			mse::impl::remove_reference_t<T>, mse::impl::remove_reference_t<EqualTo> >::type {};

		template<class T, class EqualTo>
		struct HasOrInheritsUnlockSharedMethod_msemsearray_impl
		{
			template<class U, class V>
			static auto test(U*) -> decltype(std::declval<U>().unlock_shared(), bool(true));
			template<typename, typename>
			static auto test(...)->std::false_type;

			using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
		};
		template<class T, class EqualTo = T>
		struct HasOrInheritsUnlockSharedMethod_msemsearray : HasOrInheritsUnlockSharedMethod_msemsearray_impl<
			mse::impl::remove_reference_t<T>, mse::impl::remove_reference_t<EqualTo> >::type {};
	}

	template<class _Mutex>
	class recursive_shared_mutex_wrapped : public _Mutex {
	public:
		typedef _Mutex base_class;

		void nonrecursive_lock() {
			nonrecursive_lock_helper(typename impl::HasOrInheritsNonrecursiveUnlockMethod_msemsearray<_Mutex>::type());
		}
		bool try_nonrecursive_lock() {	// try to lock nonrecursive
			return nonrecursive_try_lock_helper(typename impl::HasOrInheritsNonrecursiveUnlockMethod_msemsearray<_Mutex>::type());
		}
		void nonrecursive_unlock() {
			nonrecursive_unlock_helper(typename impl::HasOrInheritsNonrecursiveUnlockMethod_msemsearray<_Mutex>::type());
		}

		void lock_shared() {	// lock non-exclusive
			lock_shared_helper(typename impl::HasOrInheritsUnlockSharedMethod_msemsearray<_Mutex>::type());
		}
		bool try_lock_shared() {	// try to lock non-exclusive
			return try_lock_shared_helper(typename impl::HasOrInheritsUnlockSharedMethod_msemsearray<_Mutex>::type());
		}
		void unlock_shared() {	// unlock non-exclusive
			unlock_shared_helper(typename impl::HasOrInheritsUnlockSharedMethod_msemsearray<_Mutex>::type());
		}
	private:
		void nonrecursive_lock_helper(std::true_type) {
			base_class::nonrecursive_lock();
		}
		void nonrecursive_lock_helper(std::false_type) {
			base_class::lock();
		}
		bool nonrecursive_try_lock_helper(std::true_type) {
			return base_class::nonrecursive_try_lock();
		}
		bool nonrecursive_try_lock_helper(std::false_type) {
			return base_class::try_lock();
		}
		void nonrecursive_unlock_helper(std::true_type) {
			base_class::nonrecursive_unlock();
		}
		void nonrecursive_unlock_helper(std::false_type) {
			base_class::unlock();
		}

		void lock_shared_helper(std::true_type) {
			base_class::lock_shared();
		}
		void lock_shared_helper(std::false_type) {
			base_class::lock();
		}
		bool try_lock_shared_helper(std::true_type) {
			return base_class::try_lock_shared();
		}
		bool try_lock_shared_helper(std::false_type) {
			return base_class::try_lock();
		}
		void unlock_shared_helper(std::true_type) {
			base_class::unlock_shared();
		}
		void unlock_shared_helper(std::false_type) {
			base_class::unlock();
		}

	};


	template<class _Mutex>
	class unique_nonrecursive_lock
	{	// a version of std::unique_lock that calls "nonrecursive_lock()" instead of "lock()"
	public:
		typedef unique_nonrecursive_lock<_Mutex> _Myt;
		typedef _Mutex mutex_type;

		// CONSTRUCT, ASSIGN, AND DESTROY
		unique_nonrecursive_lock() _NOEXCEPT
			: _Pmtx(0), _Owns(false)
		{	// default construct
		}

		explicit unique_nonrecursive_lock(_Mutex& _Mtx)
			: _Pmtx(&_Mtx), _Owns(false)
		{	// construct and lock
			_Pmtx->nonrecursive_lock();
			_Owns = true;
		}

		unique_nonrecursive_lock(_Mutex& _Mtx, std::adopt_lock_t)
			: _Pmtx(&_Mtx), _Owns(true)
		{	// construct and assume already locked
		}

		unique_nonrecursive_lock(_Mutex& _Mtx, std::defer_lock_t) _NOEXCEPT
			: _Pmtx(&_Mtx), _Owns(false)
		{	// construct but don't lock
		}

		unique_nonrecursive_lock(_Mutex& _Mtx, std::try_to_lock_t)
			: _Pmtx(&_Mtx), _Owns(_Pmtx->try_nonrecursive_lock())
		{	// construct and try to lock
		}

		template<class _Rep,
			class _Period>
			unique_nonrecursive_lock(_Mutex& _Mtx,
				const std::chrono::duration<_Rep, _Period>& _Rel_time)
			: _Pmtx(&_Mtx), _Owns(_Pmtx->try_nonrecursive_lock_for(_Rel_time))
		{	// construct and lock with timeout
		}

		template<class _Clock,
			class _Duration>
			unique_nonrecursive_lock(_Mutex& _Mtx,
				const std::chrono::time_point<_Clock, _Duration>& _Abs_time)
			: _Pmtx(&_Mtx), _Owns(_Pmtx->try_nonrecursive_lock_until(_Abs_time))
		{	// construct and lock with timeout
		}

		unique_nonrecursive_lock(unique_nonrecursive_lock&& _Other) _NOEXCEPT
			: _Pmtx(_Other._Pmtx), _Owns(_Other._Owns)
		{	// destructive copy
			_Other._Pmtx = 0;
			_Other._Owns = false;
		}

		unique_nonrecursive_lock& operator=(unique_nonrecursive_lock&& _Other)
		{	// destructive copy
			if (this != &_Other)
			{	// different, move contents
				if (_Owns)
					_Pmtx->nonrecursive_unlock();
				_Pmtx = _Other._Pmtx;
				_Owns = _Other._Owns;
				_Other._Pmtx = 0;
				_Other._Owns = false;
			}
			return (*this);
		}

		~unique_nonrecursive_lock() _NOEXCEPT
		{	// clean up
			if (_Owns)
				_Pmtx->nonrecursive_unlock();
		}

		unique_nonrecursive_lock(const unique_nonrecursive_lock&) = delete;
		unique_nonrecursive_lock& operator=(const unique_nonrecursive_lock&) = delete;

		// LOCK AND UNLOCK
		void lock()
		{	// lock the mutex
			_Validate();
			_Pmtx->nonrecursive_lock();
			_Owns = true;
		}

		bool try_lock()
		{	// try to lock the mutex
			_Validate();
			_Owns = _Pmtx->try_nonrecursive_lock();
			return (_Owns);
		}

		template<class _Rep,
			class _Period>
			bool try_lock_for(const std::chrono::duration<_Rep, _Period>& _Rel_time)
		{	// try to lock mutex for _Rel_time
			_Validate();
			_Owns = _Pmtx->try_nonrecursive_lock_for(_Rel_time);
			return (_Owns);
		}

		template<class _Clock,
			class _Duration>
			bool try_lock_until(
				const std::chrono::time_point<_Clock, _Duration>& _Abs_time)
		{	// try to lock mutex until _Abs_time
			_Validate();
			_Owns = _Pmtx->try_nonrecursive_lock_until(_Abs_time);
			return (_Owns);
		}

		void unlock()
		{	// try to unlock the mutex
			if (!_Pmtx || !_Owns)
				_THROW_NCEE(std::system_error,
					_STD make_error_code(std::errc::operation_not_permitted));

			_Pmtx->nonrecursive_unlock();
			_Owns = false;
		}

		// MUTATE
		void swap(unique_nonrecursive_lock& _Other) _NOEXCEPT
		{	// swap with _Other
			_STD swap(_Pmtx, _Other._Pmtx);
			_STD swap(_Owns, _Other._Owns);
		}

		_Mutex *release() _NOEXCEPT
		{	// disconnect
			_Mutex *_Res = _Pmtx;
			_Pmtx = 0;
			_Owns = false;
			return (_Res);
		}

		// OBSERVE
		bool owns_lock() const _NOEXCEPT
		{	// return true if this object owns the lock
			return (_Owns);
		}

		explicit operator bool() const _NOEXCEPT
		{	// return true if this object owns the lock
			return (_Owns);
		}

		_Mutex *mutex() const _NOEXCEPT
		{	// return pointer to managed mutex
			return (_Pmtx);
		}

	private:
		_Mutex * _Pmtx;
		bool _Owns;

		void _Validate() const
		{	// check if the mutex can be locked
			if (!_Pmtx)
				_THROW_NCEE(std::system_error,
					_STD make_error_code(std::errc::operation_not_permitted));

			if (_Owns)
				_THROW_NCEE(std::system_error,
					_STD make_error_code(std::errc::resource_deadlock_would_occur));
		}
	};

	namespace impl {
		template <typename _Ty>
		class TOpaqueWrapper {
		public:
			TOpaqueWrapper(const _Ty& value_param) : m_value(value_param) {}
			TOpaqueWrapper(_Ty&& value_param) : m_value(MSE_FWD(value_param)) {}

			template<typename ...Args, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_constructible<_Ty, Args...>::value
				&& !mse::impl::is_a_pair_with_the_first_a_base_of_the_second<TOpaqueWrapper, Args...>::value> MSE_IMPL_EIS >
				TOpaqueWrapper(Args&&...args) : m_value(std::forward<Args>(args)...) {}

			_Ty& value()& { return m_value; }
			_Ty&& value()&& { return MSE_FWD(m_value); }
			const _Ty& value() const& { return m_value; }
			//const _Ty& value() const && { return m_value; }

		private:
			_Ty m_value;
		};
	}
}

# define TR2_OPTIONAL_REQUIRES(...) mse::impl::enable_if_t<__VA_ARGS__::value, bool> = false

# if defined __GNUC__ // NOTE: GNUC is also defined for Clang
#   if (__GNUC__ == 4) && (__GNUC_MINOR__ >= 8)
#     define TR2_OPTIONAL_GCC_4_8_AND_HIGHER___
#   elif (__GNUC__ > 4)
#     define TR2_OPTIONAL_GCC_4_8_AND_HIGHER___
#   endif
#
#   if (__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)
#     define TR2_OPTIONAL_GCC_4_7_AND_HIGHER___
#   elif (__GNUC__ > 4)
#     define TR2_OPTIONAL_GCC_4_7_AND_HIGHER___
#   endif
#
#   if (__GNUC__ == 4) && (__GNUC_MINOR__ == 8) && (__GNUC_PATCHLEVEL__ >= 1)
#     define TR2_OPTIONAL_GCC_4_8_1_AND_HIGHER___
#   elif (__GNUC__ == 4) && (__GNUC_MINOR__ >= 9)
#     define TR2_OPTIONAL_GCC_4_8_1_AND_HIGHER___
#   elif (__GNUC__ > 4)
#     define TR2_OPTIONAL_GCC_4_8_1_AND_HIGHER___
#   endif
# endif
#
# if defined __clang_major__
#   if (__clang_major__ == 3 && __clang_minor__ >= 5)
#     define TR2_OPTIONAL_CLANG_3_5_AND_HIGHTER_
#   elif (__clang_major__ > 3)
#     define TR2_OPTIONAL_CLANG_3_5_AND_HIGHTER_
#   endif
#   if defined TR2_OPTIONAL_CLANG_3_5_AND_HIGHTER_
#     define TR2_OPTIONAL_CLANG_3_4_2_AND_HIGHER_
#   elif (__clang_major__ == 3 && __clang_minor__ == 4 && __clang_patchlevel__ >= 2)
#     define TR2_OPTIONAL_CLANG_3_4_2_AND_HIGHER_
#   endif
# endif
#
# if defined _MSC_VER
#   if (_MSC_VER >= 1900)
#     define TR2_OPTIONAL_MSVC_2015_AND_HIGHER___
#   endif
# endif

# if defined __clang__
#   if (__clang_major__ > 2) || (__clang_major__ == 2) && (__clang_minor__ >= 9)
#     define OPTIONAL_HAS_THIS_RVALUE_REFS 1
#   else
#     define OPTIONAL_HAS_THIS_RVALUE_REFS 0
#   endif
# elif defined TR2_OPTIONAL_GCC_4_8_1_AND_HIGHER___
#   define OPTIONAL_HAS_THIS_RVALUE_REFS 1
# elif defined TR2_OPTIONAL_MSVC_2015_AND_HIGHER___
#   define OPTIONAL_HAS_THIS_RVALUE_REFS 1
# else
#   define OPTIONAL_HAS_THIS_RVALUE_REFS 0
# endif


# if defined TR2_OPTIONAL_GCC_4_8_1_AND_HIGHER___
#   define OPTIONAL_HAS_CONSTEXPR_INIT_LIST 1
#   define OPTIONAL_CONSTEXPR_INIT_LIST constexpr
# else
#   define OPTIONAL_HAS_CONSTEXPR_INIT_LIST 0
#   define OPTIONAL_CONSTEXPR_INIT_LIST
# endif

# if defined TR2_OPTIONAL_CLANG_3_5_AND_HIGHTER_ && (defined __cplusplus) && (__cplusplus != 201103L)
#   define OPTIONAL_HAS_MOVE_ACCESSORS 1
# else
#   define OPTIONAL_HAS_MOVE_ACCESSORS 0
# endif

# // In C++11 constexpr implies const, so we need to make non-const members also non-constexpr
# if (defined __cplusplus) && (__cplusplus == 201103L)
#   define OPTIONAL_MUTABLE_CONSTEXPR
# else
#   define OPTIONAL_MUTABLE_CONSTEXPR constexpr
# endif

namespace mse {

	// workaround: std utility functions aren't constexpr yet
	template <class T> inline constexpr T&& constexpr_forward(mse::impl::remove_reference_t<T>& t) noexcept
	{
		return static_cast<T&&>(t);
	}

	template <class T> inline constexpr T&& constexpr_forward(mse::impl::remove_reference_t<T>&& t) noexcept
	{
		static_assert(!std::is_lvalue_reference<T>::value, "!!");
		return static_cast<T&&>(t);
	}

	template <class T> inline constexpr mse::impl::remove_reference_t<T>&& constexpr_move(T&& t) noexcept
	{
		return static_cast<mse::impl::remove_reference_t<T>&&>(t);
	}

#ifndef MSE_HAS_CXX17

	namespace us {
		namespace impl {

			// BEGIN workaround for missing std::is_trivially_destructible
# if defined TR2_OPTIONAL_GCC_4_8_AND_HIGHER___
	// leave it: it is already there
# elif defined TR2_OPTIONAL_CLANG_3_4_2_AND_HIGHER_
	// leave it: it is already there
# elif defined TR2_OPTIONAL_MSVC_2015_AND_HIGHER___
	// leave it: it is already there
# elif defined TR2_OPTIONAL_DISABLE_EMULATION_OF_TYPE_TRAITS
	// leave it: the user doesn't want it
# else
			template <typename T>
			using std::is_trivially_destructible = std::has_trivial_destructor<T>;
# endif
			// END workaround for missing std::is_trivially_destructible

# if (defined TR2_OPTIONAL_GCC_4_7_AND_HIGHER___)
	// leave it; our metafunctions are already defined.
# elif defined TR2_OPTIONAL_CLANG_3_4_2_AND_HIGHER_
	// leave it; our metafunctions are already defined.
# elif defined TR2_OPTIONAL_MSVC_2015_AND_HIGHER___
	// leave it: it is already there
# elif defined TR2_OPTIONAL_DISABLE_EMULATION_OF_TYPE_TRAITS
	// leave it: the user doesn't want it
# else


	// workaround for missing traits in GCC and CLANG
			template <class T>
			struct std::is_nothrow_move_constructible
			{
				constexpr static bool value = std::is_nothrow_constructible<T, T&&>::value;
			};


			template <class T, class U>
			struct is_assignable
			{
				template <class X, class Y>
				constexpr static bool has_assign(...) { return false; }

				template <class X, class Y, size_t S = sizeof((std::declval<X>() = std::declval<Y>(), true)) >
				// the comma operator is necessary for the cases where operator= returns void
				constexpr static bool has_assign(bool) { return true; }

				constexpr static bool value = has_assign<T, U>(true);
			};


			template <class T>
			struct std::is_nothrow_move_assignable
			{
				template <class X, bool has_any_move_assign>
				struct has_nothrow_move_assign {
					constexpr static bool value = false;
				};

				template <class X>
				struct has_nothrow_move_assign<X, true> {
					constexpr static bool value = noexcept(std::declval<X&>() = std::declval<X&&>());
				};

				constexpr static bool value = has_nothrow_move_assign<T, is_assignable<T&, T&&>::value>::value;
			};
			// end workaround


# endif



	// 20.5.4, optional for object types
			template <class T> class optional;

			// 20.5.5, optional for lvalue reference types
			template <class T> class optional<T&>;


			/*
			// workaround: std utility functions aren't constexpr yet
			template <class T> inline constexpr T&& constexpr_forward(mse::impl::remove_reference_t<T>& t) noexcept
			{
				return static_cast<T&&>(t);
			}

			template <class T> inline constexpr T&& constexpr_forward(mse::impl::remove_reference_t<T>&& t) noexcept
			{
				static_assert(!std::is_lvalue_reference<T>::value, "!!");
				return static_cast<T&&>(t);
			}

			template <class T> inline constexpr mse::impl::remove_reference_t<T>&& constexpr_move(T&& t) noexcept
			{
				return static_cast<mse::impl::remove_reference_t<T>&&>(t);
			}
			*/


#if defined NDEBUG
# define TR2_OPTIONAL_ASSERTED_EXPRESSION(CHECK, EXPR) (EXPR)
#else
# define TR2_OPTIONAL_ASSERTED_EXPRESSION(CHECK, EXPR) ((CHECK) ? (EXPR) : ([]{assert(!#CHECK);}(), (EXPR)))
#endif


			namespace detail_
			{

				// static_addressof: a constexpr version of addressof
				template <typename T>
				struct has_overloaded_addressof
				{
					template <class X>
					constexpr static bool has_overload(...) { return false; }

					template <class X, size_t S = sizeof(std::declval<X&>().operator&()) >
					constexpr static bool has_overload(bool) { return true; }

					constexpr static bool value = has_overload<T>(true);
				};

				template <typename T, TR2_OPTIONAL_REQUIRES(!has_overloaded_addressof<T>)>
				constexpr T* static_addressof(T& ref)
				{
					return &ref;
				}

				template <typename T, TR2_OPTIONAL_REQUIRES(has_overloaded_addressof<T>)>
				T* static_addressof(T& ref)
				{
					return std::addressof(ref);
				}


				// the call to convert<A>(b) has return type A and converts b to type A iff b decltype(b) is implicitly convertible to A  
				template <class U>
				constexpr U convert(U v) { return v; }

			} // namespace detail


			constexpr struct trivial_init_t {} trivial_init{};


			// 20.5.6, In-place construction
			constexpr struct in_place_t {} in_place{};


			// 20.5.7, Disengaged state indicator
			struct nullopt_t
			{
				struct init {};
				constexpr explicit nullopt_t(init) {}
			};
			constexpr nullopt_t nullopt{ nullopt_t::init() };


			// 20.5.8, class bad_optional_access
			class bad_optional_access : public std::logic_error {
			public:
				explicit bad_optional_access(const std::string& what_arg) : std::logic_error{ what_arg } {}
				explicit bad_optional_access(const char* what_arg) : std::logic_error{ what_arg } {}
			};


			template <class T>
			union storage_t
			{
				unsigned char dummy_;
				T value_;

				constexpr storage_t(trivial_init_t) noexcept : dummy_() {};

				template <class... Args>
				constexpr storage_t(Args&&... args) : value_(constexpr_forward<Args>(args)...) {}

				~storage_t() {}
			};


			template <class T>
			union constexpr_storage_t
			{
				unsigned char dummy_;
				T value_;

				constexpr constexpr_storage_t(trivial_init_t) noexcept : dummy_() {};

				template <class... Args>
				constexpr constexpr_storage_t(Args&&... args) : value_(constexpr_forward<Args>(args)...) {}

				~constexpr_storage_t() = default;
			};


			template <class T>
			struct optional_base
			{
				bool init_;
				storage_t<T> storage_;

				constexpr optional_base() noexcept : init_(false), storage_(trivial_init) {};

				explicit constexpr optional_base(const T& v) : init_(true), storage_(v) {}

				explicit constexpr optional_base(T&& v) : init_(true), storage_(constexpr_move(v)) {}

				template <class... Args> explicit optional_base(in_place_t, Args&&... args)
					: init_(true), storage_(constexpr_forward<Args>(args)...) {}

				template <class U, class... Args, TR2_OPTIONAL_REQUIRES(std::is_constructible<T, std::initializer_list<U>>)>
				explicit optional_base(in_place_t, std::initializer_list<U> il, Args&&... args)
					: init_(true), storage_(il, std::forward<Args>(args)...) {}

				~optional_base() { if (init_) storage_.value_.T::~T(); }
			};


			template <class T>
			struct constexpr_optional_base
			{
				bool init_;
				constexpr_storage_t<T> storage_;

				constexpr constexpr_optional_base() noexcept : init_(false), storage_(trivial_init) {};

				explicit constexpr constexpr_optional_base(const T& v) : init_(true), storage_(v) {}

				explicit constexpr constexpr_optional_base(T&& v) : init_(true), storage_(constexpr_move(v)) {}

				template <class... Args> explicit constexpr constexpr_optional_base(in_place_t, Args&&... args)
					: init_(true), storage_(constexpr_forward<Args>(args)...) {}

				template <class U, class... Args, TR2_OPTIONAL_REQUIRES(std::is_constructible<T, std::initializer_list<U>>)>
				OPTIONAL_CONSTEXPR_INIT_LIST explicit constexpr_optional_base(in_place_t, std::initializer_list<U> il, Args&&... args)
					: init_(true), storage_(il, std::forward<Args>(args)...) {}

				~constexpr_optional_base() = default;
			};

			template <class T>
			using OptionalBase = mse::impl::conditional_t<
				std::is_trivially_destructible<T>::value,                          // if possible
				constexpr_optional_base<mse::impl::remove_const_t<T> >, // use base with trivial destructor
				optional_base<mse::impl::remove_const_t<T> >
			>;



			template <class T>
			class optional : private OptionalBase<T>
			{
				static_assert(!std::is_same<typename std::decay<T>::type, nullopt_t>::value, "bad T");
				static_assert(!std::is_same<typename std::decay<T>::type, in_place_t>::value, "bad T");


				constexpr bool initialized() const noexcept { return OptionalBase<T>::init_; }
				mse::impl::remove_const_t<T>* dataptr() { return std::addressof(OptionalBase<T>::storage_.value_); }
				constexpr const T* dataptr() const { return detail_::static_addressof(OptionalBase<T>::storage_.value_); }

# if OPTIONAL_HAS_THIS_RVALUE_REFS == 1
				constexpr const T& contained_val() const& { return OptionalBase<T>::storage_.value_; }
#   if OPTIONAL_HAS_MOVE_ACCESSORS == 1
				OPTIONAL_MUTABLE_CONSTEXPR T&& contained_val() && { return std::move(OptionalBase<T>::storage_.value_); }
				OPTIONAL_MUTABLE_CONSTEXPR T& contained_val() & { return OptionalBase<T>::storage_.value_; }
#   else
				T& contained_val() & { return OptionalBase<T>::storage_.value_; }
				T&& contained_val() && { return std::move(OptionalBase<T>::storage_.value_); }
#   endif
# else
				constexpr const T& contained_val() const { return OptionalBase<T>::storage_.value_; }
				T& contained_val() { return OptionalBase<T>::storage_.value_; }
# endif

				void clear() noexcept {
					if (initialized()) dataptr()->T::~T();
					OptionalBase<T>::init_ = false;
				}

				template <class... Args>
				void initialize(Args&&... args) noexcept(noexcept(T(std::forward<Args>(args)...)))
				{
					assert(!OptionalBase<T>::init_);
					::new (static_cast<void*>(dataptr())) T(std::forward<Args>(args)...);
					OptionalBase<T>::init_ = true;
				}

				template <class U, class... Args>
				void initialize(std::initializer_list<U> il, Args&&... args) noexcept(noexcept(T(il, std::forward<Args>(args)...)))
				{
					assert(!OptionalBase<T>::init_);
					::new (static_cast<void*>(dataptr())) T(il, std::forward<Args>(args)...);
					OptionalBase<T>::init_ = true;
				}

			public:
				typedef T value_type;

				// 20.5.5.1, constructors
				constexpr optional() noexcept : OptionalBase<T>() {};
				constexpr optional(nullopt_t) noexcept : OptionalBase<T>() {};

				optional(const optional& rhs)
					: OptionalBase<T>()
				{
					if (rhs.initialized()) {
						::new (static_cast<void*>(dataptr())) T(*rhs);
						OptionalBase<T>::init_ = true;
					}
				}

				optional(optional&& rhs) noexcept(std::is_nothrow_move_constructible<T>::value)
					: OptionalBase<T>()
				{
					if (rhs.initialized()) {
						::new (static_cast<void*>(dataptr())) T(std::move(*rhs));
						OptionalBase<T>::init_ = true;
					}
				}

				constexpr optional(const T& v) : OptionalBase<T>(v) {}

				constexpr optional(T&& v) : OptionalBase<T>(constexpr_move(v)) {}

				template <class... Args>
				explicit constexpr optional(in_place_t, Args&&... args)
					: OptionalBase<T>(in_place_t{}, constexpr_forward<Args>(args)...) {}

				template <class U, class... Args, TR2_OPTIONAL_REQUIRES(std::is_constructible<T, std::initializer_list<U>>)>
				OPTIONAL_CONSTEXPR_INIT_LIST explicit optional(in_place_t, std::initializer_list<U> il, Args&&... args)
					: OptionalBase<T>(in_place_t{}, il, constexpr_forward<Args>(args)...) {}

				// 20.5.4.2, Destructor
				~optional() = default;

				// 20.5.4.3, assignment
				optional& operator=(nullopt_t) noexcept
				{
					clear();
					return *this;
				}

				optional& operator=(const optional& rhs)
				{
					if (initialized() == true && rhs.initialized() == false) clear();
					else if (initialized() == false && rhs.initialized() == true)  initialize(*rhs);
					else if (initialized() == true && rhs.initialized() == true)  contained_val() = *rhs;
					return *this;
				}

				optional& operator=(optional&& rhs)
					noexcept(std::is_nothrow_move_assignable<T>::value && std::is_nothrow_move_constructible<T>::value)
				{
					if (initialized() == true && rhs.initialized() == false) clear();
					else if (initialized() == false && rhs.initialized() == true)  initialize(std::move(*rhs));
					else if (initialized() == true && rhs.initialized() == true)  contained_val() = std::move(*rhs);
					return *this;
				}

				template <class U>
				auto operator=(U&& v)
					-> typename std::enable_if
					<
					std::is_same<typename std::decay<U>::type, T>::value,
					optional&
					>::type
				{
					if (initialized()) { contained_val() = std::forward<U>(v); }
					else { initialize(std::forward<U>(v)); }
					return *this;
				}


				template <class... Args>
				T& emplace(Args&&... args)
				{
					clear();
					initialize(std::forward<Args>(args)...);
					return *(*this);
				}

				template <class U, class... Args>
				T& emplace(std::initializer_list<U> il, Args&&... args)
				{
					clear();
					initialize<U, Args...>(il, std::forward<Args>(args)...);
					return *(*this);
				}

				// 20.5.4.4, Swap
				void swap(optional<T>& rhs) noexcept(std::is_nothrow_move_constructible<T>::value && noexcept(std::swap(std::declval<T&>(), std::declval<T&>())))
				{
					if (initialized() == true && rhs.initialized() == false) { rhs.initialize(std::move(**this)); clear(); }
					else if (initialized() == false && rhs.initialized() == true) { initialize(std::move(*rhs)); rhs.clear(); }
					else if (initialized() == true && rhs.initialized() == true) { using std::swap; swap(**this, *rhs); }
				}

				// 20.5.4.5, Observers

				explicit constexpr operator bool() const noexcept { return initialized(); }
				constexpr bool has_value() const noexcept { return initialized(); }

				constexpr T const* operator ->() const {
					if (!has_value()) { MSE_THROW(bad_optional_access("bad optional access")); }
					return TR2_OPTIONAL_ASSERTED_EXPRESSION(initialized(), dataptr());
				}

# if OPTIONAL_HAS_MOVE_ACCESSORS == 1

				OPTIONAL_MUTABLE_CONSTEXPR T* operator ->() {
					assert(initialized());
					if (!has_value()) { MSE_THROW(bad_optional_access("bad optional access")); }
					return dataptr();
				}

				constexpr T const& operator *() const& {
					if (!has_value()) { MSE_THROW(bad_optional_access("bad optional access")); }
					return TR2_OPTIONAL_ASSERTED_EXPRESSION(initialized(), contained_val());
				}

				OPTIONAL_MUTABLE_CONSTEXPR T& operator *() & {
					assert(initialized());
					if (!has_value()) { MSE_THROW(bad_optional_access("bad optional access")); }
					return contained_val();
				}

				OPTIONAL_MUTABLE_CONSTEXPR T&& operator *() && {
					assert(initialized());
					if (!has_value()) { MSE_THROW(bad_optional_access("bad optional access")); }
					return constexpr_move(contained_val());
				}

				constexpr T const& value() const& {
					return initialized() ? contained_val() : (MSE_THROW(bad_optional_access("bad optional access")), contained_val());
				}

				OPTIONAL_MUTABLE_CONSTEXPR T& value() & {
					return initialized() ? contained_val() : (MSE_THROW(bad_optional_access("bad optional access")), contained_val());
				}

				OPTIONAL_MUTABLE_CONSTEXPR T&& value() && {
					if (!initialized()) MSE_THROW(bad_optional_access("bad optional access"));
					return std::move(contained_val());
				}

# else

				T* operator ->() {
					assert(initialized());
					if (!has_value()) { MSE_THROW(bad_optional_access("bad optional access")); }
					return dataptr();
				}

				constexpr T const& operator *() const {
					if (!has_value()) { MSE_THROW(bad_optional_access("bad optional access")); }
					return TR2_OPTIONAL_ASSERTED_EXPRESSION(initialized(), contained_val());
				}

				T& operator *() {
					assert(initialized());
					if (!has_value()) { MSE_THROW(bad_optional_access("bad optional access")); }
					return contained_val();
				}

				constexpr T const& value() const {
					if (!initialized()) { MSE_THROW(bad_optional_access("bad optional access")); }
					return contained_val();
				}

				T& value() {
					if (!initialized()) { MSE_THROW(bad_optional_access("bad optional access")); }
					return contained_val();
				}

# endif

# if OPTIONAL_HAS_THIS_RVALUE_REFS == 1

				template <class V>
				constexpr T value_or(V&& v) const&
				{
					return *this ? **this : detail_::convert<T>(constexpr_forward<V>(v));
				}

#   if OPTIONAL_HAS_MOVE_ACCESSORS == 1

				template <class V>
				OPTIONAL_MUTABLE_CONSTEXPR T value_or(V&& v) &&
				{
					return *this ? constexpr_move(const_cast<optional<T>&>(*this).contained_val()) : detail_::convert<T>(constexpr_forward<V>(v));
				}

#   else

				template <class V>
				T value_or(V&& v) &&
				{
					return *this ? constexpr_move(const_cast<optional<T>&>(*this).contained_val()) : detail_::convert<T>(constexpr_forward<V>(v));
				}

#   endif

# else

				template <class V>
				constexpr T value_or(V&& v) const
				{
					return *this ? **this : detail_::convert<T>(constexpr_forward<V>(v));
				}

# endif

				// 20.6.3.6, modifiers
				void reset() noexcept { clear(); }
			};


			template <class T>
			class optional<T&>
			{
				static_assert(!std::is_same<T, nullopt_t>::value, "bad T");
				static_assert(!std::is_same<T, in_place_t>::value, "bad T");
				T* ref;

			public:

				// 20.5.5.1, construction/destruction
				constexpr optional() noexcept : ref(nullptr) {}

				constexpr optional(nullopt_t) noexcept : ref(nullptr) {}

				constexpr optional(T& v) noexcept : ref(detail_::static_addressof(v)) {}

				optional(T&&) = delete;

				constexpr optional(const optional& rhs) noexcept : ref(rhs.ref) {}

				explicit constexpr optional(in_place_t, T& v) noexcept : ref(detail_::static_addressof(v)) {}

				explicit optional(in_place_t, T&&) = delete;

				~optional() = default;

				// 20.5.5.2, mutation
				optional& operator=(nullopt_t) noexcept {
					ref = nullptr;
					return *this;
				}

				// optional& operator=(const optional& rhs) noexcept {
				// ref = rhs.ref;
				// return *this;
				// }

				// optional& operator=(optional&& rhs) noexcept {
				// ref = rhs.ref;
				// return *this;
				// }

				template <typename U>
				auto operator=(U&& rhs) noexcept
					-> typename std::enable_if
					<
					std::is_same<typename std::decay<U>::type, optional<T&>>::value,
					optional&
					>::type
				{
					ref = rhs.ref;
					return *this;
				}

				template <typename U>
				auto operator=(U&& rhs) noexcept
					-> typename std::enable_if
					<
					!std::is_same<typename std::decay<U>::type, optional<T&>>::value,
					optional&
					>::type
					= delete;

				void emplace(T& v) noexcept {
					ref = detail_::static_addressof(v);
				}

				void emplace(T&&) = delete;


				void swap(optional<T&>& rhs) noexcept
				{
					std::swap(ref, rhs.ref);
				}

				// 20.5.5.3, observers
				constexpr T* operator->() const {
					return TR2_OPTIONAL_ASSERTED_EXPRESSION(ref, ref);
				}

				constexpr T& operator*() const {
					return TR2_OPTIONAL_ASSERTED_EXPRESSION(ref, *ref);
				}

				constexpr T& value() const {
					return ref ? *ref : (MSE_THROW(bad_optional_access("bad optional access")), *ref);
				}

				explicit constexpr operator bool() const noexcept {
					return ref != nullptr;
				}

				constexpr bool has_value() const noexcept {
					return ref != nullptr;
				}

				template <class V>
				constexpr typename std::decay<T>::type value_or(V&& v) const
				{
					return *this ? **this : detail_::convert<typename std::decay<T>::type>(constexpr_forward<V>(v));
				}

				// x.x.x.x, modifiers
				void reset() noexcept { ref = nullptr; }
			};


			template <class T>
			class optional<T&&>
			{
				static_assert(sizeof(T) == 0, "optional rvalue references disallowed");
			};

#ifdef MSE_HAS_CXX17
			template<class _Ty>
			optional(_Ty)->optional<_Ty>;
#endif /* MSE_HAS_CXX17 */


			// 20.5.8, Relational operators
			template <class T> constexpr bool operator==(const optional<T>& x, const optional<T>& y)
			{
				return bool(x) != bool(y) ? false : bool(x) == false ? true : *x == *y;
			}

			template <class T> constexpr bool operator!=(const optional<T>& x, const optional<T>& y)
			{
				return !(x == y);
			}

			template <class T> constexpr bool operator<(const optional<T>& x, const optional<T>& y)
			{
				return (!y) ? false : (!x) ? true : *x < *y;
			}

			template <class T> constexpr bool operator>(const optional<T>& x, const optional<T>& y)
			{
				return (y < x);
			}

			template <class T> constexpr bool operator<=(const optional<T>& x, const optional<T>& y)
			{
				return !(y < x);
			}

			template <class T> constexpr bool operator>=(const optional<T>& x, const optional<T>& y)
			{
				return !(x < y);
			}


			// 20.5.9, Comparison with nullopt
			template <class T> constexpr bool operator==(const optional<T>& x, nullopt_t) noexcept
			{
				return (!x);
			}

			template <class T> constexpr bool operator==(nullopt_t, const optional<T>& x) noexcept
			{
				return (!x);
			}

			template <class T> constexpr bool operator!=(const optional<T>& x, nullopt_t) noexcept
			{
				return bool(x);
			}

			template <class T> constexpr bool operator!=(nullopt_t, const optional<T>& x) noexcept
			{
				return bool(x);
			}

			template <class T> constexpr bool operator<(const optional<T>&, nullopt_t) noexcept
			{
				return false;
			}

			template <class T> constexpr bool operator<(nullopt_t, const optional<T>& x) noexcept
			{
				return bool(x);
			}

			template <class T> constexpr bool operator<=(const optional<T>& x, nullopt_t) noexcept
			{
				return (!x);
			}

			template <class T> constexpr bool operator<=(nullopt_t, const optional<T>&) noexcept
			{
				return true;
			}

			template <class T> constexpr bool operator>(const optional<T>& x, nullopt_t) noexcept
			{
				return bool(x);
			}

			template <class T> constexpr bool operator>(nullopt_t, const optional<T>&) noexcept
			{
				return false;
			}

			template <class T> constexpr bool operator>=(const optional<T>&, nullopt_t) noexcept
			{
				return true;
			}

			template <class T> constexpr bool operator>=(nullopt_t, const optional<T>& x) noexcept
			{
				return (!x);
			}



			// 20.5.10, Comparison with T
			template <class T> constexpr bool operator==(const optional<T>& x, const T& v)
			{
				return bool(x) ? *x == v : false;
			}

			template <class T> constexpr bool operator==(const T& v, const optional<T>& x)
			{
				return bool(x) ? v == *x : false;
			}

			template <class T> constexpr bool operator!=(const optional<T>& x, const T& v)
			{
				return bool(x) ? *x != v : true;
			}

			template <class T> constexpr bool operator!=(const T& v, const optional<T>& x)
			{
				return bool(x) ? v != *x : true;
			}

			template <class T> constexpr bool operator<(const optional<T>& x, const T& v)
			{
				return bool(x) ? *x < v : true;
			}

			template <class T> constexpr bool operator>(const T& v, const optional<T>& x)
			{
				return bool(x) ? v > *x : true;
			}

			template <class T> constexpr bool operator>(const optional<T>& x, const T& v)
			{
				return bool(x) ? *x > v : false;
			}

			template <class T> constexpr bool operator<(const T& v, const optional<T>& x)
			{
				return bool(x) ? v < *x : false;
			}

			template <class T> constexpr bool operator>=(const optional<T>& x, const T& v)
			{
				return bool(x) ? *x >= v : false;
			}

			template <class T> constexpr bool operator<=(const T& v, const optional<T>& x)
			{
				return bool(x) ? v <= *x : false;
			}

			template <class T> constexpr bool operator<=(const optional<T>& x, const T& v)
			{
				return bool(x) ? *x <= v : true;
			}

			template <class T> constexpr bool operator>=(const T& v, const optional<T>& x)
			{
				return bool(x) ? v >= *x : true;
			}


			// Comparison of optional<T&> with T
			template <class T> constexpr bool operator==(const optional<T&>& x, const T& v)
			{
				return bool(x) ? *x == v : false;
			}

			template <class T> constexpr bool operator==(const T& v, const optional<T&>& x)
			{
				return bool(x) ? v == *x : false;
			}

			template <class T> constexpr bool operator!=(const optional<T&>& x, const T& v)
			{
				return bool(x) ? *x != v : true;
			}

			template <class T> constexpr bool operator!=(const T& v, const optional<T&>& x)
			{
				return bool(x) ? v != *x : true;
			}

			template <class T> constexpr bool operator<(const optional<T&>& x, const T& v)
			{
				return bool(x) ? *x < v : true;
			}

			template <class T> constexpr bool operator>(const T& v, const optional<T&>& x)
			{
				return bool(x) ? v > *x : true;
			}

			template <class T> constexpr bool operator>(const optional<T&>& x, const T& v)
			{
				return bool(x) ? *x > v : false;
			}

			template <class T> constexpr bool operator<(const T& v, const optional<T&>& x)
			{
				return bool(x) ? v < *x : false;
			}

			template <class T> constexpr bool operator>=(const optional<T&>& x, const T& v)
			{
				return bool(x) ? *x >= v : false;
			}

			template <class T> constexpr bool operator<=(const T& v, const optional<T&>& x)
			{
				return bool(x) ? v <= *x : false;
			}

			template <class T> constexpr bool operator<=(const optional<T&>& x, const T& v)
			{
				return bool(x) ? *x <= v : true;
			}

			template <class T> constexpr bool operator>=(const T& v, const optional<T&>& x)
			{
				return bool(x) ? v >= *x : true;
			}

			// Comparison of optional<T const&> with T
			template <class T> constexpr bool operator==(const optional<const T&>& x, const T& v)
			{
				return bool(x) ? *x == v : false;
			}

			template <class T> constexpr bool operator==(const T& v, const optional<const T&>& x)
			{
				return bool(x) ? v == *x : false;
			}

			template <class T> constexpr bool operator!=(const optional<const T&>& x, const T& v)
			{
				return bool(x) ? *x != v : true;
			}

			template <class T> constexpr bool operator!=(const T& v, const optional<const T&>& x)
			{
				return bool(x) ? v != *x : true;
			}

			template <class T> constexpr bool operator<(const optional<const T&>& x, const T& v)
			{
				return bool(x) ? *x < v : true;
			}

			template <class T> constexpr bool operator>(const T& v, const optional<const T&>& x)
			{
				return bool(x) ? v > *x : true;
			}

			template <class T> constexpr bool operator>(const optional<const T&>& x, const T& v)
			{
				return bool(x) ? *x > v : false;
			}

			template <class T> constexpr bool operator<(const T& v, const optional<const T&>& x)
			{
				return bool(x) ? v < *x : false;
			}

			template <class T> constexpr bool operator>=(const optional<const T&>& x, const T& v)
			{
				return bool(x) ? *x >= v : false;
			}

			template <class T> constexpr bool operator<=(const T& v, const optional<const T&>& x)
			{
				return bool(x) ? v <= *x : false;
			}

			template <class T> constexpr bool operator<=(const optional<const T&>& x, const T& v)
			{
				return bool(x) ? *x <= v : true;
			}

			template <class T> constexpr bool operator>=(const T& v, const optional<const T&>& x)
			{
				return bool(x) ? v >= *x : true;
			}


			// 20.5.12, Specialized algorithms
			template <class T>
			void swap(optional<T>& x, optional<T>& y) noexcept(noexcept(x.swap(y)))
			{
				x.swap(y);
			}


			template <class T>
			constexpr optional<typename std::decay<T>::type> make_optional(T&& v)
			{
				return optional<typename std::decay<T>::type>(constexpr_forward<T>(v));
			}

			template <class X>
			constexpr optional<X&> make_optional(std::reference_wrapper<X> v)
			{
				return optional<X&>(v.get());
			}

		}
	}
#endif // !MSE_HAS_CXX17


	/* This macro roughly simulates constructor inheritance. */
#define MSE_OPTIONAL_USING(Derived, Base) MSE_USING_SANS_INITIALIZER_LISTS(Derived, Base)

	template <class _TLender, class T/* = mse::impl::target_type<_TLender> */>
	class xscope_borrowing_fixed_optional;

	namespace us {
		namespace impl {
			template<class TDynamicContainer> class Txscope_const_optional_structure_lock_guard;
			template<class TDynamicContainer> class Txscope_optional_structure_lock_guard;

			namespace ns_optional {
#ifdef MSE_HAS_CXX17
				template <class T> using optional_base1 = std::optional<T>;
				using in_place_t_base = std::in_place_t;
				using nullopt_t_base = std::nullopt_t;
				using bad_optional_access_base = std::bad_optional_access;
#else // MSE_HAS_CXX17
				template <class T> using optional_base1 = mse::us::impl::optional<T>;
				using in_place_t_base = mse::us::impl::in_place_t;
				using nullopt_t_base = mse::us::impl::nullopt_t;
				using bad_optional_access_base = mse::us::impl::bad_optional_access;
#endif // MSE_HAS_CXX17

				// 20.5.6, In-place construction
				typedef mse::us::impl::ns_optional::in_place_t_base in_place_t;
				constexpr in_place_t in_place{};

				struct optional_base2_const_lockable_tag; struct optional_base2_not_const_lockable_tag;

				template <class T, class _TStateMutex/* = mse::non_thread_safe_shared_mutex*/, class TConstLockableIndicator/* = optional_base2_not_const_lockable_tag*/>
				class optional_base2
#ifdef MSE_HAS_CXX17
					: private mse::impl::TOpaqueWrapper<optional_base1<T> > {
				public:
					typedef mse::impl::TOpaqueWrapper<optional_base1<T> > base_class;
					typedef optional_base1<T> _MO;

				private:
					const _MO& contained_optional() const& { access_guard{ m_access_mutex }; return base_class::value(); }
					const _MO&& contained_optional() const&& { access_guard{ m_access_mutex }; return base_class::value(); }
					_MO& contained_optional()& { access_guard{ m_access_mutex }; return base_class::value(); }
					_MO&& contained_optional()&& {
						access_guard{ m_access_mutex };
						/* We're making sure that the optional is not "structure locked", because in that case it might not be
						safe to to allow the contained optional to be moved from (when made movable with std::move()). */
						structure_change_guard lock1(m_structure_change_mutex);
						return std::move(base_class::value());
					}

					const _MO& unchecked_contained_optional() const& { return base_class::value(); }
					const _MO&& unchecked_contained_optional() const&& { return base_class::value(); }
					_MO& unchecked_contained_optional()& { return base_class::value(); }
					_MO&& unchecked_contained_optional()&& { return std::move(base_class::value()); }

				public:
#ifdef MSE_OPTIONAL_IMPLEMENTATION1

					MSE_OPTIONAL_USING(optional_base2, base_class);

					template<class... _Types, class = std::enable_if_t<std::is_constructible_v<T, _Types...> > >
					constexpr explicit optional_base2(in_place_t, _Types&&... _Args) : base_class(in_place, std::forward<_Types>(_Args)...) {}
					template<class _Elem, class... _Types, class = std::enable_if_t<std::is_constructible_v<T, std::initializer_list<_Elem>&, _Types...> > >
					constexpr explicit optional_base2(in_place_t, std::initializer_list<_Elem> _Ilist, _Types&&... _Args)
						: base_class(in_place, _Ilist, std::forward<_Types>(_Args)...) {}

					template<class T2>
					using _AllowDirectConversion = std::integral_constant<bool, std::conjunction_v<
						//std::negation<std::is_same<std::remove_cv_t<std::remove_reference_t<T2>>, optional_base2>>,
						std::negation<std::is_base_of<base_class, std::remove_cv_t<std::remove_reference_t<T2>>>>,
						std::negation<std::is_same<std::remove_cv_t<std::remove_reference_t<T2>>, in_place_t>>,
						std::is_constructible<T, T2> > >;
					template<class T2 = T, std::enable_if_t<std::conjunction_v<_AllowDirectConversion<T2>, std::is_convertible<T2, T>>, int> = 0>
					constexpr optional_base2(T2&& _Right) : base_class(in_place, std::forward<T2>(_Right)) {}
					template<class T2 = T, std::enable_if_t<std::conjunction_v<_AllowDirectConversion<T2>, std::negation<std::is_convertible<T2, T> > >, int> = 0>
					constexpr explicit optional_base2(T2&& _Right) : base_class(in_place, std::forward<T2>(_Right)) {}

#else // MSE_OPTIONAL_IMPLEMENTATION1
					using base_class::base_class;
					template<class T2, class = mse::impl::disable_if_is_a_pair_with_the_first_a_base_of_the_second_t<optional_base2, T2> >
					explicit optional_base2(T2&& _X) : base_class(MSE_FWD(_X)) {}
#endif // MSE_OPTIONAL_IMPLEMENTATION1

					optional_base2(const _MO& src) : base_class(src) {}
					optional_base2(_MO&& src) : base_class(MSE_FWD(src)) {}

#else // MSE_HAS_CXX17
					: public optional_base1<T> {
				public:
					typedef optional_base1<T> base_class;
					typedef base_class _MO;

				private:
					const _MO& contained_optional() const& { access_guard{ m_access_mutex }; return (*this); }
					const _MO& contained_optional() const&& { access_guard{ m_access_mutex }; return (*this); }
					_MO& contained_optional()& { access_guard{ m_access_mutex }; return (*this); }
					auto contained_optional()&& {
						access_guard{ m_access_mutex };
						/* We're making sure that the optional is not "structure locked", because in that case it might not be
						safe to to allow the contained optional to be moved from (when made movable with std::move()). */
						structure_change_guard lock1(m_structure_change_mutex);
						return mse::us::impl::as_ref<base_class>(std::move(*this));
					}

					const _MO& unchecked_contained_optional() const& { return (*this); }
					const _MO& unchecked_contained_optional() const&& { return (*this); }
					_MO& unchecked_contained_optional()& { return (*this); }
					auto unchecked_contained_optional()&& { return mse::us::impl::as_ref<base_class>(std::move(*this)); }

				public:
					using base_class::base_class;
					MSE_OPTIONAL_USING(optional_base2, base_class);
					template<class T2, class = mse::impl::disable_if_is_a_pair_with_the_first_a_base_of_the_second_t<optional_base2, T2> >
					explicit optional_base2(T2&& _X) : base_class(MSE_FWD(_X)) {}
#endif // MSE_HAS_CXX17
					typedef optional_base2 _Myt;
					typedef typename _MO::value_type value_type;

					optional_base2(const optional_base2& src_ref) : base_class((src_ref).contained_optional()) {}
					optional_base2(optional_base2&& src_ref) : base_class(MSE_FWD(src_ref).contained_optional()) {}

					~optional_base2() {
						mse::impl::destructor_lock_guard1<decltype(m_structure_change_mutex)> lock1(m_structure_change_mutex);
					}

					constexpr explicit operator bool() const noexcept {
						return bool(contained_optional());
					}
					_NODISCARD constexpr bool has_value() const noexcept {
						return contained_optional().has_value();
					}

					_NODISCARD constexpr const T& value() const& {
						return contained_optional().value();
					}
					_NODISCARD constexpr T& value()& {
						return contained_optional().value();
					}
					_NODISCARD constexpr T&& value()&& {
						return std::move(MSE_FWD(contained_optional()).value());
					}
					_NODISCARD constexpr const T&& value() const&& {
						return std::move(MSE_FWD(contained_optional()).value());
					}

					template <class _Ty2>
					_NODISCARD constexpr T value_or(_Ty2&& _Right) const& {
						return contained_optional().value_or(std::forward<_Ty2>(_Right));
					}
					template <class _Ty2>
					_NODISCARD constexpr T value_or(_Ty2&& _Right)&& {
						return contained_optional().value_or(std::forward<_Ty2>(_Right));
					}

					_NODISCARD constexpr const T * operator->() const & {
						return std::addressof(contained_optional().value());
					}
					_NODISCARD constexpr const T* operator->() const && = delete;
					_NODISCARD constexpr T * operator->() & {
						return std::addressof((*this).value());
					}
					_NODISCARD constexpr const T* operator->() && = delete;
					_NODISCARD constexpr const T& operator*() const & {
						return (*this).value();
					}
					_NODISCARD constexpr T& operator*() & {
						return (*this).value();
					}
					_NODISCARD constexpr T&& operator*() && {
						return std::move((*this).value());
					}
					_NODISCARD constexpr const T&& operator*() const && {
						return std::move((*this).value());
					}

					optional_base2& operator=(const optional_base2& rhs) {
						if (std::addressof(rhs) == this) { return (*this); }
						structure_change_guard lock1(m_structure_change_mutex);
						contained_optional().operator=(rhs.contained_optional());
						return (*this);
					}
					optional_base2& operator=(optional_base2&& rhs) {
						if (std::addressof(rhs) == this) { return (*this); }
						structure_change_guard lock1(m_structure_change_mutex);
						contained_optional().operator=(MSE_FWD(rhs).contained_optional());
						return (*this);
					}
					template<typename ...Args>
					optional_base2& operator=(Args&&...args) {
						structure_change_guard lock1(m_structure_change_mutex);
						contained_optional().operator=(std::forward<Args>(args)...);
						return (*this);
					}
					template <class... Args>
					T& emplace(Args&&... args) {
						structure_change_guard lock1(m_structure_change_mutex);
						return contained_optional().emplace(std::forward<Args>(args)...);
					}
					template <class U, class... Args>
					T& emplace(std::initializer_list<U> il, Args&&... args) {
						structure_change_guard lock1(m_structure_change_mutex);
						return contained_optional().emplace(il, std::forward<Args>(args)...);
					}
					template<class T2>
					void swap(T2& rhs) {
						structure_change_guard lock1(m_structure_change_mutex);
						contained_optional().swap(rhs.contained_optional());
					}
					void reset() {
						structure_change_guard lock1(m_structure_change_mutex);
						contained_optional().reset();
					}

					MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(T);

				private:

					MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

					class structure_change_guard {
					public:
						structure_change_guard(_TStateMutex& _Mtx) MSE_FUNCTION_TRY : m_lock_guard(_Mtx) {}
						MSE_FUNCTION_CATCH_ANY {
							MSE_THROW(mse::structure_lock_violation_error("structure lock violation - Attempting to modify \
							the structure (size/capacity) of a container while a reference to one of its elements \
							still exists?"));
						}
					private:
						std::lock_guard<_TStateMutex> m_lock_guard;
					};
					auto structure_change_lock() const { m_structure_change_mutex.lock(); }
					auto structure_change_unlock() const { m_structure_change_mutex.unlock(); }

					class access_guard {
					public:
						access_guard(mse::non_thread_safe_mutex& _Mtx) MSE_FUNCTION_TRY : m_lock_guard(_Mtx) {}
						MSE_FUNCTION_CATCH_ANY{
							MSE_THROW(mse::structure_lock_violation_error("access lock violation - Attempting to access \
							the object while it is being borrowed?"));
						}
					private:
						std::lock_guard<mse::non_thread_safe_mutex> m_lock_guard;
					};
					auto access_lock() const { m_access_mutex.lock(); }
					auto access_unlock() const { m_access_mutex.unlock(); }

					mutable _TStateMutex m_structure_change_mutex;
					mutable mse::non_thread_safe_mutex m_access_mutex;

					friend class mse::us::impl::Txscope_optional_structure_lock_guard<_Myt>;
					friend class mse::us::impl::Txscope_const_optional_structure_lock_guard<_Myt>;
					template <class _TLender2, class T2>
					friend class mse::xscope_borrowing_fixed_optional;
				};

				// 20.5.8, Relational operators
				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator==(const optional_base2<T, _TStateMutex, TConstLockableIndicator>& x, const optional_base2<T, _TStateMutex, TConstLockableIndicator>& y)
				{
					return bool(x) != bool(y) ? false : bool(x) == false ? true : *x == *y;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator!=(const optional_base2<T, _TStateMutex, TConstLockableIndicator>& x, const optional_base2<T, _TStateMutex, TConstLockableIndicator>& y)
				{
					return !(x == y);
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator<(const optional_base2<T, _TStateMutex, TConstLockableIndicator>& x, const optional_base2<T, _TStateMutex, TConstLockableIndicator>& y)
				{
					return (!y) ? false : (!x) ? true : *x < *y;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator>(const optional_base2<T, _TStateMutex, TConstLockableIndicator>& x, const optional_base2<T, _TStateMutex, TConstLockableIndicator>& y)
				{
					return (y < x);
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator<=(const optional_base2<T, _TStateMutex, TConstLockableIndicator>& x, const optional_base2<T, _TStateMutex, TConstLockableIndicator>& y)
				{
					return !(y < x);
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator>=(const optional_base2<T, _TStateMutex, TConstLockableIndicator>& x, const optional_base2<T, _TStateMutex, TConstLockableIndicator>& y)
				{
					return !(x < y);
				}


				// 20.5.9, Comparison with nullopt
				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator==(const optional_base2<T, _TStateMutex, TConstLockableIndicator>& x, nullopt_t_base) noexcept
				{
					return (!x);
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator==(nullopt_t_base, const optional_base2<T, _TStateMutex, TConstLockableIndicator>& x) noexcept
				{
					return (!x);
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator!=(const optional_base2<T, _TStateMutex, TConstLockableIndicator>& x, nullopt_t_base) noexcept
				{
					return bool(x);
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator!=(nullopt_t_base, const optional_base2<T, _TStateMutex, TConstLockableIndicator>& x) noexcept
				{
					return bool(x);
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator<(const optional_base2<T, _TStateMutex, TConstLockableIndicator>&, nullopt_t_base) noexcept
				{
					return false;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator<(nullopt_t_base, const optional_base2<T, _TStateMutex, TConstLockableIndicator>& x) noexcept
				{
					return bool(x);
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator<=(const optional_base2<T, _TStateMutex, TConstLockableIndicator>& x, nullopt_t_base) noexcept
				{
					return (!x);
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator<=(nullopt_t_base, const optional_base2<T, _TStateMutex, TConstLockableIndicator>&) noexcept
				{
					return true;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator>(const optional_base2<T, _TStateMutex, TConstLockableIndicator>& x, nullopt_t_base) noexcept
				{
					return bool(x);
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator>(nullopt_t_base, const optional_base2<T, _TStateMutex, TConstLockableIndicator>&) noexcept
				{
					return false;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator>=(const optional_base2<T, _TStateMutex, TConstLockableIndicator>&, nullopt_t_base) noexcept
				{
					return true;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator>=(nullopt_t_base, const optional_base2<T, _TStateMutex, TConstLockableIndicator>& x) noexcept
				{
					return (!x);
				}


				// 20.5.10, Comparison with T
				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator==(const optional_base2<T, _TStateMutex, TConstLockableIndicator>& x, const T& v)
				{
					return bool(x) ? *x == v : false;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator==(const T& v, const optional_base2<T, _TStateMutex, TConstLockableIndicator>& x)
				{
					return bool(x) ? v == *x : false;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator!=(const optional_base2<T, _TStateMutex, TConstLockableIndicator>& x, const T& v)
				{
					return bool(x) ? *x != v : true;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator!=(const T& v, const optional_base2<T, _TStateMutex, TConstLockableIndicator>& x)
				{
					return bool(x) ? v != *x : true;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator<(const optional_base2<T, _TStateMutex, TConstLockableIndicator>& x, const T& v)
				{
					return bool(x) ? *x < v : true;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator>(const T& v, const optional_base2<T, _TStateMutex, TConstLockableIndicator>& x)
				{
					return bool(x) ? v > *x : true;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator>(const optional_base2<T, _TStateMutex, TConstLockableIndicator>& x, const T& v)
				{
					return bool(x) ? *x > v : false;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator<(const T& v, const optional_base2<T, _TStateMutex, TConstLockableIndicator>& x)
				{
					return bool(x) ? v < *x : false;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator>=(const optional_base2<T, _TStateMutex, TConstLockableIndicator>& x, const T& v)
				{
					return bool(x) ? *x >= v : false;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator<=(const T& v, const optional_base2<T, _TStateMutex, TConstLockableIndicator>& x)
				{
					return bool(x) ? v <= *x : false;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator<=(const optional_base2<T, _TStateMutex, TConstLockableIndicator>& x, const T& v)
				{
					return bool(x) ? *x <= v : true;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator>=(const T& v, const optional_base2<T, _TStateMutex, TConstLockableIndicator>& x)
				{
					return bool(x) ? v >= *x : true;
				}


				// Comparison of optional<T&> with T
				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator==(const optional_base2<T&, _TStateMutex, TConstLockableIndicator>& x, const T& v)
				{
					return bool(x) ? *x == v : false;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator==(const T& v, const optional_base2<T&, _TStateMutex, TConstLockableIndicator>& x)
				{
					return bool(x) ? v == *x : false;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator!=(const optional_base2<T&, _TStateMutex, TConstLockableIndicator>& x, const T& v)
				{
					return bool(x) ? *x != v : true;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator!=(const T& v, const optional_base2<T&, _TStateMutex, TConstLockableIndicator>& x)
				{
					return bool(x) ? v != *x : true;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator<(const optional_base2<T&, _TStateMutex, TConstLockableIndicator>& x, const T& v)
				{
					return bool(x) ? *x < v : true;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator>(const T& v, const optional_base2<T&, _TStateMutex, TConstLockableIndicator>& x)
				{
					return bool(x) ? v > *x : true;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator>(const optional_base2<T&, _TStateMutex, TConstLockableIndicator>& x, const T& v)
				{
					return bool(x) ? *x > v : false;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator<(const T& v, const optional_base2<T&, _TStateMutex, TConstLockableIndicator>& x)
				{
					return bool(x) ? v < *x : false;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator>=(const optional_base2<T&, _TStateMutex, TConstLockableIndicator>& x, const T& v)
				{
					return bool(x) ? *x >= v : false;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator<=(const T& v, const optional_base2<T&, _TStateMutex, TConstLockableIndicator>& x)
				{
					return bool(x) ? v <= *x : false;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator<=(const optional_base2<T&, _TStateMutex, TConstLockableIndicator>& x, const T& v)
				{
					return bool(x) ? *x <= v : true;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator>=(const T& v, const optional_base2<T&, _TStateMutex, TConstLockableIndicator>& x)
				{
					return bool(x) ? v >= *x : true;
				}

				// Comparison of optional<T const&> with T
				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator==(const optional_base2<const T&, _TStateMutex, TConstLockableIndicator>& x, const T& v)
				{
					return bool(x) ? *x == v : false;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator==(const T& v, const optional_base2<const T&, _TStateMutex, TConstLockableIndicator>& x)
				{
					return bool(x) ? v == *x : false;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator!=(const optional_base2<const T&, _TStateMutex, TConstLockableIndicator>& x, const T& v)
				{
					return bool(x) ? *x != v : true;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator!=(const T& v, const optional_base2<const T&, _TStateMutex, TConstLockableIndicator>& x)
				{
					return bool(x) ? v != *x : true;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator<(const optional_base2<const T&, _TStateMutex, TConstLockableIndicator>& x, const T& v)
				{
					return bool(x) ? *x < v : true;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator>(const T& v, const optional_base2<const T&, _TStateMutex, TConstLockableIndicator>& x)
				{
					return bool(x) ? v > *x : true;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator>(const optional_base2<const T&, _TStateMutex, TConstLockableIndicator>& x, const T& v)
				{
					return bool(x) ? *x > v : false;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator<(const T& v, const optional_base2<const T&, _TStateMutex, TConstLockableIndicator>& x)
				{
					return bool(x) ? v < *x : false;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator>=(const optional_base2<const T&, _TStateMutex, TConstLockableIndicator>& x, const T& v)
				{
					return bool(x) ? *x >= v : false;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator<=(const T& v, const optional_base2<const T&, _TStateMutex, TConstLockableIndicator>& x)
				{
					return bool(x) ? v <= *x : false;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator<=(const optional_base2<const T&, _TStateMutex, TConstLockableIndicator>& x, const T& v)
				{
					return bool(x) ? *x <= v : true;
				}

				template <class T, class _TStateMutex, class TConstLockableIndicator> constexpr bool operator>=(const T& v, const optional_base2<const T&, _TStateMutex, TConstLockableIndicator>& x)
				{
					return bool(x) ? v >= *x : true;
				}

				// 20.5.12, Specialized algorithms
				template <class T, class _TStateMutex, class TConstLockableIndicator>
				void swap(optional_base2<T, _TStateMutex, TConstLockableIndicator>& x, optional_base2<T, _TStateMutex, TConstLockableIndicator>& y) noexcept(noexcept(x.swap(y)))
				{
					x.swap(y);
				}
			}
		}
	}

	// 20.5.6, In-place construction
	typedef mse::us::impl::ns_optional::in_place_t_base in_place_t;
	constexpr in_place_t in_place{};
	// 20.5.7, Disengaged state indicator
	typedef typename mse::us::impl::ns_optional::nullopt_t_base nullopt_t;
#ifdef MSE_HAS_CXX17
	MSE_INLINE_VAR constexpr const auto& nullopt = std::nullopt;
#else // MSE_HAS_CXX17
	constexpr nullopt_t nullopt{ nullopt_t::init() };
#endif // MSE_HAS_CXX17
	// 20.5.8, class bad_optional_access
	typedef typename mse::us::impl::ns_optional::bad_optional_access_base bad_optional_access;


	template <class T>
	class optional : public mse::us::impl::ns_optional::optional_base2<T, mse::non_thread_safe_shared_mutex, mse::us::impl::ns_optional::optional_base2_not_const_lockable_tag> {
	public:
		typedef mse::us::impl::ns_optional::optional_base2<T, mse::non_thread_safe_shared_mutex, mse::us::impl::ns_optional::optional_base2_not_const_lockable_tag> base_class;
		typedef typename base_class::value_type value_type;

#ifdef MSE_HAS_CXX17
#ifdef MSE_OPTIONAL_IMPLEMENTATION1

		MSE_OPTIONAL_USING(optional, base_class);

		template<class... _Types, class = std::enable_if_t<std::is_constructible_v<T, _Types...> > >
		constexpr explicit optional(in_place_t, _Types&&... _Args) : base_class(in_place, std::forward<_Types>(_Args)...) {}
		template<class _Elem, class... _Types, class = std::enable_if_t<std::is_constructible_v<T, std::initializer_list<_Elem>&, _Types...> > >
		constexpr explicit optional(in_place_t, std::initializer_list<_Elem> _Ilist, _Types&&... _Args)
			: base_class(in_place, _Ilist, std::forward<_Types>(_Args)...) {}

		template<class T2>
		using _AllowDirectConversion = std::integral_constant<bool, std::conjunction_v<
			//std::negation<std::is_same<std::remove_cv_t<std::remove_reference_t<T2>>, optional>>,
			std::negation<std::is_base_of<base_class, std::remove_cv_t<std::remove_reference_t<T2>>>>,
			std::negation<std::is_same<std::remove_cv_t<std::remove_reference_t<T2>>, in_place_t>>,
			std::is_constructible<T, T2> > >;
		template<class T2 = T, std::enable_if_t<std::conjunction_v<_AllowDirectConversion<T2>, std::is_convertible<T2, T>>, int> = 0>
		constexpr optional(T2&& _Right) : base_class(in_place, std::forward<T2>(_Right)) {}
		template<class T2 = T, std::enable_if_t<std::conjunction_v<_AllowDirectConversion<T2>, std::negation<std::is_convertible<T2, T> > >, int> = 0>
		constexpr explicit optional(T2&& _Right) : base_class(in_place, std::forward<T2>(_Right)) {}

#else // MSE_OPTIONAL_IMPLEMENTATION1
		using base_class::base_class;
		template<class T2, class = mse::impl::disable_if_is_a_pair_with_the_first_a_base_of_the_second_t<optional, T2> >
		explicit optional(T2&& _X) : base_class(MSE_FWD(_X)) {}
#endif // MSE_OPTIONAL_IMPLEMENTATION1

		optional(const base_class& src) : base_class(src) {}
		optional(base_class&& src) : base_class(MSE_FWD(src)) {}

#else // MSE_HAS_CXX17
		using base_class::base_class;
		MSE_OPTIONAL_USING(optional, base_class);
		template<class T2, class = mse::impl::disable_if_is_a_pair_with_the_first_a_base_of_the_second_t<optional, T2> >
		explicit optional(T2&& _X) : base_class(MSE_FWD(_X)) {}
#endif // MSE_HAS_CXX17

		optional(const optional& src) : base_class(mse::us::impl::as_ref<base_class>(src)) {}
		//optional(optional&& src) : base_class(mse::us::impl::as_ref<base_class>(MSE_FWD(src))) {}

		~optional() {
#ifndef MSE_OPTIONAL_NO_XSCOPE_DEPENDENCE
			mse::impl::T_valid_if_not_an_xscope_type<T>();
#endif // !MSE_OPTIONAL_NO_XSCOPE_DEPENDENCE
		}

		//using base_class::operator=;

		MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(T);

	private:
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;
	};

#ifdef MSE_HAS_CXX17
	template<class _Ty>
	optional(_Ty)->optional<_Ty>;
#endif /* MSE_HAS_CXX17 */

	template <class T>
	constexpr optional<typename std::decay<T>::type> make_optional(T&& v) {
		return optional<typename std::decay<T>::type>(constexpr_forward<T>(v));
	}
	template <class X>
	constexpr optional<X&> make_optional(std::reference_wrapper<X> v) {
		return optional<X&>(v.get());
	}

	template <class T>
	class mt_optional : public mse::us::impl::ns_optional::optional_base2<T, mse::shareable_dynamic_container_mutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag> {
	public:
		typedef mse::us::impl::ns_optional::optional_base2<T, mse::shareable_dynamic_container_mutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag> base_class;
		typedef typename base_class::value_type value_type;

#ifdef MSE_HAS_CXX17
#ifdef MSE_OPTIONAL_IMPLEMENTATION1

		MSE_OPTIONAL_USING(mt_optional, base_class);

		template<class... _Types, class = std::enable_if_t<std::is_constructible_v<T, _Types...> > >
		constexpr explicit mt_optional(in_place_t, _Types&&... _Args) : base_class(in_place, std::forward<_Types>(_Args)...) {}
		template<class _Elem, class... _Types, class = std::enable_if_t<std::is_constructible_v<T, std::initializer_list<_Elem>&, _Types...> > >
		constexpr explicit mt_optional(in_place_t, std::initializer_list<_Elem> _Ilist, _Types&&... _Args)
			: base_class(in_place, _Ilist, std::forward<_Types>(_Args)...) {}

		template<class T2>
		using _AllowDirectConversion = std::integral_constant<bool, std::conjunction_v<
			//std::negation<std::is_same<std::remove_cv_t<std::remove_reference_t<T2>>, mt_optional>>,
			std::negation<std::is_base_of<base_class, std::remove_cv_t<std::remove_reference_t<T2>>>>,
			std::negation<std::is_same<std::remove_cv_t<std::remove_reference_t<T2>>, in_place_t>>,
			std::is_constructible<T, T2> > >;
		template<class T2 = T, std::enable_if_t<std::conjunction_v<_AllowDirectConversion<T2>, std::is_convertible<T2, T>>, int> = 0>
		constexpr mt_optional(T2&& _Right) : base_class(in_place, std::forward<T2>(_Right)) {}
		template<class T2 = T, std::enable_if_t<std::conjunction_v<_AllowDirectConversion<T2>, std::negation<std::is_convertible<T2, T> > >, int> = 0>
		constexpr explicit mt_optional(T2&& _Right) : base_class(in_place, std::forward<T2>(_Right)) {}

#else // MSE_OPTIONAL_IMPLEMENTATION1
		using base_class::base_class;
		template<class T2, class = mse::impl::disable_if_is_a_pair_with_the_first_a_base_of_the_second_t<mt_optional, T2> >
		explicit mt_optional(T2&& _X) : base_class(MSE_FWD(_X)) {}
#endif // MSE_OPTIONAL_IMPLEMENTATION1

		mt_optional(const base_class& src) : base_class(src) {}
		mt_optional(base_class&& src) : base_class(MSE_FWD(src)) {}

#else // MSE_HAS_CXX17
		using base_class::base_class;
		MSE_OPTIONAL_USING(mt_optional, base_class);
		template<class T2, class = mse::impl::disable_if_is_a_pair_with_the_first_a_base_of_the_second_t<mt_optional, T2> >
		explicit mt_optional(T2&& _X) : base_class(MSE_FWD(_X)) {}
#endif // MSE_HAS_CXX17

		mt_optional(const mt_optional& src_ref) : base_class(mse::us::impl::as_ref<base_class>(src_ref)) {}
		//mt_optional(mt_optional&& src_ref) : base_class(mse::us::impl::as_ref<base_class>(MSE_FWD(src_ref))) {}

		~mt_optional() {
#ifndef MSE_OPTIONAL_NO_XSCOPE_DEPENDENCE
			mse::impl::T_valid_if_not_an_xscope_type<T>();
#endif // !MSE_OPTIONAL_NO_XSCOPE_DEPENDENCE
		}

		//using base_class::operator=;

		MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(T);

	private:
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;
	};

#ifdef MSE_HAS_CXX17
	template<class _Ty>
	mt_optional(_Ty)->mt_optional<_Ty>;
#endif /* MSE_HAS_CXX17 */

	template <class T>
	constexpr mt_optional<typename std::decay<T>::type> make_mt_optional(T&& v) {
		return mt_optional<typename std::decay<T>::type>(constexpr_forward<T>(v));
	}
	template <class X>
	constexpr mt_optional<X&> make_mt_optional(std::reference_wrapper<X> v) {
		return mt_optional<X&>(v.get());
	}

	template <class T>
	class st_optional : public mse::us::impl::ns_optional::optional_base2<T, mse::non_thread_safe_shared_mutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag> {
	public:
		typedef mse::us::impl::ns_optional::optional_base2<T, mse::non_thread_safe_shared_mutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag> base_class;
		typedef typename base_class::value_type value_type;

#ifdef MSE_HAS_CXX17
#ifdef MSE_OPTIONAL_IMPLEMENTATION1

		MSE_OPTIONAL_USING(st_optional, base_class);

		template<class... _Types, class = std::enable_if_t<std::is_constructible_v<T, _Types...> > >
		constexpr explicit st_optional(in_place_t, _Types&&... _Args) : base_class(in_place, std::forward<_Types>(_Args)...) {}
		template<class _Elem, class... _Types, class = std::enable_if_t<std::is_constructible_v<T, std::initializer_list<_Elem>&, _Types...> > >
		constexpr explicit st_optional(in_place_t, std::initializer_list<_Elem> _Ilist, _Types&&... _Args)
			: base_class(in_place, _Ilist, std::forward<_Types>(_Args)...) {}

		template<class T2>
		using _AllowDirectConversion = std::integral_constant<bool, std::conjunction_v<
			//std::negation<std::is_same<std::remove_cv_t<std::remove_reference_t<T2>>, st_optional>>,
			std::negation<std::is_base_of<base_class, std::remove_cv_t<std::remove_reference_t<T2>>>>,
			std::negation<std::is_same<std::remove_cv_t<std::remove_reference_t<T2>>, in_place_t>>,
			std::is_constructible<T, T2> > >;
		template<class T2 = T, std::enable_if_t<std::conjunction_v<_AllowDirectConversion<T2>, std::is_convertible<T2, T>>, int> = 0>
		constexpr st_optional(T2&& _Right) : base_class(in_place, std::forward<T2>(_Right)) {}
		template<class T2 = T, std::enable_if_t<std::conjunction_v<_AllowDirectConversion<T2>, std::negation<std::is_convertible<T2, T> > >, int> = 0>
		constexpr explicit st_optional(T2&& _Right) : base_class(in_place, std::forward<T2>(_Right)) {}

#else // MSE_OPTIONAL_IMPLEMENTATION1
		using base_class::base_class;
		template<class T2, class = mse::impl::disable_if_is_a_pair_with_the_first_a_base_of_the_second_t<st_optional, T2> >
		explicit st_optional(T2&& _X) : base_class(MSE_FWD(_X)) {}
#endif // MSE_OPTIONAL_IMPLEMENTATION1

		st_optional(const base_class& src) : base_class(src) {}
		st_optional(base_class&& src) : base_class(MSE_FWD(src)) {}

#else // MSE_HAS_CXX17
		using base_class::base_class;
		MSE_OPTIONAL_USING(st_optional, base_class);
		template<class T2, class = mse::impl::disable_if_is_a_pair_with_the_first_a_base_of_the_second_t<st_optional, T2> >
		explicit st_optional(T2&& _X) : base_class(MSE_FWD(_X)) {}
#endif // MSE_HAS_CXX17

		st_optional(const st_optional& src_ref) : base_class(mse::us::impl::as_ref<base_class>(src_ref)) {}
		//st_optional(st_optional&& src_ref) : base_class(mse::us::impl::as_ref<base_class>(MSE_FWD(src_ref))) {}

		~st_optional() {
#ifndef MSE_OPTIONAL_NO_XSCOPE_DEPENDENCE
			mse::impl::T_valid_if_not_an_xscope_type<T>();
#endif // !MSE_OPTIONAL_NO_XSCOPE_DEPENDENCE
		}

		//using base_class::operator=;

	private:
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;
	};

#ifdef MSE_HAS_CXX17
	template<class _Ty>
	st_optional(_Ty)->st_optional<_Ty>;
#endif /* MSE_HAS_CXX17 */

	template <class T>
	constexpr st_optional<typename std::decay<T>::type> make_st_optional(T&& v) {
		return st_optional<typename std::decay<T>::type>(constexpr_forward<T>(v));
	}
	template <class X>
	constexpr st_optional<X&> make_st_optional(std::reference_wrapper<X> v) {
		return st_optional<X&>(v.get());
	}

	namespace mstd {
		// 20.5.6, In-place construction
		typedef mse::in_place_t in_place_t;
		constexpr in_place_t in_place{};
		// 20.5.7, Disengaged state indicator
		typedef mse::nullopt_t nullopt_t;
#ifdef MSE_HAS_CXX17
		MSE_INLINE_VAR constexpr const auto& nullopt = std::nullopt;
#else // MSE_HAS_CXX17
		constexpr nullopt_t nullopt{ nullopt_t::init() };
#endif // MSE_HAS_CXX17
		// 20.5.8, class bad_optional_access
		typedef mse::bad_optional_access bad_optional_access;

		template <class T>
		class optional : public mse::st_optional<T> {
		public:
			typedef mse::st_optional<T> base_class;
			typedef typename base_class::value_type value_type;

#ifdef MSE_HAS_CXX17
#ifdef MSE_OPTIONAL_IMPLEMENTATION1

			MSE_OPTIONAL_USING(optional, base_class);

			template<class... _Types, class = std::enable_if_t<std::is_constructible_v<T, _Types...> > >
			constexpr explicit optional(mse::mstd::in_place_t, _Types&&... _Args) : base_class(mse::mstd::in_place, std::forward<_Types>(_Args)...) {}
			template<class _Elem, class... _Types, class = std::enable_if_t<std::is_constructible_v<T, std::initializer_list<_Elem>&, _Types...> > >
			constexpr explicit optional(mse::mstd::in_place_t, std::initializer_list<_Elem> _Ilist, _Types&&... _Args)
				: base_class(mse::mstd::in_place, _Ilist, std::forward<_Types>(_Args)...) {}

			template<class T2>
			using _AllowDirectConversion = std::integral_constant<bool, std::conjunction_v<
				//std::negation<std::is_same<std::remove_cv_t<std::remove_reference_t<T2>>, optional>>,
				std::negation<std::is_base_of<base_class, std::remove_cv_t<std::remove_reference_t<T2>>>>,
				std::negation<std::is_same<std::remove_cv_t<std::remove_reference_t<T2>>, mse::mstd::in_place_t>>,
				std::is_constructible<T, T2> > >;
			template<class T2 = T, std::enable_if_t<std::conjunction_v<_AllowDirectConversion<T2>, std::is_convertible<T2, T>>, int> = 0>
			constexpr optional(T2&& _Right) : base_class(mse::mstd::in_place, std::forward<T2>(_Right)) {}
			template<class T2 = T, std::enable_if_t<std::conjunction_v<_AllowDirectConversion<T2>, std::negation<std::is_convertible<T2, T> > >, int> = 0>
			constexpr explicit optional(T2&& _Right) : base_class(mse::mstd::in_place, std::forward<T2>(_Right)) {}

#else // MSE_OPTIONAL_IMPLEMENTATION1
			using base_class::base_class;
			template<class T2, class = mse::impl::disable_if_is_a_pair_with_the_first_a_base_of_the_second_t<optional, T2> >
			explicit optional(T2&& _X) : base_class(MSE_FWD(_X)) {}
#endif // MSE_OPTIONAL_IMPLEMENTATION1

			optional(const base_class& src) : base_class(src) {}
			optional(base_class&& src) : base_class(MSE_FWD(src)) {}

#else // MSE_HAS_CXX17
			using base_class::base_class;
			MSE_OPTIONAL_USING(optional, base_class);
			template<class T2, class = mse::impl::disable_if_is_a_pair_with_the_first_a_base_of_the_second_t<optional, T2> >
			explicit optional(T2&& _X) : base_class(MSE_FWD(_X)) {}
#endif // MSE_HAS_CXX17

			optional(const optional& src_ref) : base_class(static_cast<const base_class&>(src_ref)) {}

			~optional() {
#ifndef MSE_OPTIONAL_NO_XSCOPE_DEPENDENCE
				mse::impl::T_valid_if_not_an_xscope_type<T>();
#endif // !MSE_OPTIONAL_NO_XSCOPE_DEPENDENCE
			}

			//using base_class::operator=;

		private:
			MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;
		};

#ifdef MSE_HAS_CXX17
		template<class _Ty>
		optional(_Ty)->optional<_Ty>;
#endif /* MSE_HAS_CXX17 */

		template <class T>
		constexpr optional<typename std::decay<T>::type> make_optional(T&& v) {
			return optional<typename std::decay<T>::type>(constexpr_forward<T>(v));
		}
		template <class X>
		constexpr optional<X&> make_optional(std::reference_wrapper<X> v) {
			return optional<X&>(v.get());
		}
	}

#ifndef MSE_OPTIONAL_NO_XSCOPE_DEPENDENCE

	template <class T>
	class xscope_optional : public mse::us::impl::ns_optional::optional_base2<T, mse::non_thread_safe_shared_mutex, mse::us::impl::ns_optional::optional_base2_not_const_lockable_tag>, public mse::us::impl::XScopeTagBase
		, MSE_INHERIT_XSCOPE_TAG_BASE_SET_FROM(T, xscope_optional<T>)
	{
	public:
		typedef mse::us::impl::ns_optional::optional_base2<T, mse::non_thread_safe_shared_mutex, mse::us::impl::ns_optional::optional_base2_not_const_lockable_tag> base_class;
		typedef typename base_class::value_type value_type;

#ifdef MSE_HAS_CXX17
#ifdef MSE_OPTIONAL_IMPLEMENTATION1

		MSE_OPTIONAL_USING(xscope_optional, base_class);

		template<class... Tpes, class = std::enable_if_t<std::is_constructible_v<T, Tpes...> > >
		constexpr explicit xscope_optional(mse::in_place_t, Tpes&&... _Args) : base_class(mse::in_place, std::forward<Tpes>(_Args)...) {}
		template<class _Elem, class... Tpes, class = std::enable_if_t<std::is_constructible_v<T, std::initializer_list<_Elem>&, Tpes...> > >
		constexpr explicit xscope_optional(mse::in_place_t, std::initializer_list<_Elem> _Ilist, Tpes&&... _Args)
			: base_class(mse::in_place, _Ilist, std::forward<Tpes>(_Args)...) {}

		template<class T2>
		using _AllowDirectConversion = std::integral_constant<bool, std::conjunction_v<
			//std::negation<std::is_same<std::remove_cv_t<std::remove_reference_t<T2>>, xscope_optional>>,
			std::negation<std::is_base_of<base_class, std::remove_cv_t<std::remove_reference_t<T2>>>>,
			std::negation<std::is_same<std::remove_cv_t<std::remove_reference_t<T2>>, mse::in_place_t>>,
			std::is_constructible<T, T2> > >;
		template<class T2 = T, std::enable_if_t<std::conjunction_v<_AllowDirectConversion<T2>, std::is_convertible<T2, T>>, int> = 0>
		constexpr xscope_optional(T2&& _Right) : base_class(mse::in_place, std::forward<T2>(_Right)) {}
		template<class T2 = T, std::enable_if_t<std::conjunction_v<_AllowDirectConversion<T2>, std::negation<std::is_convertible<T2, T> > >, int> = 0>
		constexpr explicit xscope_optional(T2&& _Right) : base_class(mse::in_place, std::forward<T2>(_Right)) {}

#else // MSE_OPTIONAL_IMPLEMENTATION1
		using base_class::base_class;
		template<class T2, class = mse::impl::disable_if_is_a_pair_with_the_first_a_base_of_the_second_t<xscope_optional, T2> >
		explicit xscope_optional(T2&& _X) : base_class(MSE_FWD(_X)) {}
#endif // MSE_OPTIONAL_IMPLEMENTATION1

		xscope_optional(const base_class& src) : base_class(src) {}
		xscope_optional(base_class&& src) : base_class(MSE_FWD(src)) {}

#else // MSE_HAS_CXX17
		using base_class::base_class;
		MSE_OPTIONAL_USING(xscope_optional, base_class);
		template<class T2, class = mse::impl::disable_if_is_a_pair_with_the_first_a_base_of_the_second_t<xscope_optional, T2> >
		explicit xscope_optional(T2&& _X) : base_class(MSE_FWD(_X)) {}
#endif // MSE_HAS_CXX17

		xscope_optional(const xscope_optional& src_ref) : base_class(mse::us::impl::as_ref<base_class>(src_ref)) {}
		xscope_optional(xscope_optional&& src_ref) : base_class(mse::us::impl::as_ref<base_class>(MSE_FWD(src_ref))) {}
		//xscope_optional(const mstd::optional<T>& src_ref) : base_class(mse::us::impl::as_ref<base_class>(src_ref)) {}

		xscope_optional& operator=(nullopt_t) noexcept {
			valid_if_T_is_not_marked_as_containing_an_accessible_scope_address_of_operator<T>();
			base_class::clear();
			return *this;
		}
		xscope_optional& operator=(const xscope_optional& rhs) {
			valid_if_T_is_not_marked_as_containing_a_scope_reference<T>();
			valid_if_T_is_not_marked_as_containing_an_accessible_scope_address_of_operator<T>();
			base_class::operator=(mse::us::impl::as_ref<base_class>(rhs));
			return *this;
		}
		xscope_optional& operator=(xscope_optional&& rhs) noexcept(std::is_nothrow_move_assignable<T>::value && std::is_nothrow_move_constructible<T>::value) {
			valid_if_T_is_not_marked_as_containing_a_scope_reference<T>();
			valid_if_T_is_not_marked_as_containing_an_accessible_scope_address_of_operator<T>();
			base_class::operator=(mse::us::impl::as_ref<base_class>(MSE_FWD(rhs)));
			return *this;
		}
		template <class U, class = mse::impl::disable_if_is_a_pair_with_the_first_a_base_of_the_second_t<xscope_optional, U> >
		auto operator=(U&& v) -> mse::impl::enable_if_t<std::is_same<typename std::decay<U>::type, T>::value, xscope_optional&> {
			valid_if_T_is_not_marked_as_containing_a_scope_reference<T>();
			valid_if_T_is_not_marked_as_containing_an_accessible_scope_address_of_operator<T>();
			base_class::operator=(std::forward<U>(v));
			return *this;
		}
		template <class... Args>
		void emplace(Args&&... args) {
			valid_if_T_is_not_marked_as_containing_a_scope_reference<T>();
			valid_if_T_is_not_marked_as_containing_an_accessible_scope_address_of_operator<T>();
			base_class::emplace(std::forward<Args>(args)...);
		}
		template <class U, class... Args>
		void emplace(std::initializer_list<U> il, Args&&... args) {
			valid_if_T_is_not_marked_as_containing_a_scope_reference<T>();
			valid_if_T_is_not_marked_as_containing_an_accessible_scope_address_of_operator<T>();
			base_class::emplace(il, std::forward<Args>(args)...);
		}
		template<class T2 = T, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<T2, T>::value) && (mse::impl::is_potentially_not_referenceable_by_scope_pointer<T2>::value)> MSE_IMPL_EIS >
		void reset() noexcept {
			valid_if_T_is_not_marked_as_containing_an_accessible_scope_address_of_operator<T>();
			base_class::reset();
		}
		template<class T2 = T, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<T2, T>::value)
			&& (mse::impl::potentially_does_not_contain_non_owning_scope_reference<T2>::value)
			&& (mse::impl::is_potentially_not_referenceable_by_scope_pointer<T2>::value)> MSE_IMPL_EIS >
		void swap(xscope_optional<T>& rhs) noexcept(std::is_nothrow_move_constructible<T>::value && noexcept(std::swap(std::declval<T&>(), std::declval<T&>()))) {
			valid_if_T_is_not_marked_as_containing_a_scope_reference<T>();
			valid_if_T_is_not_marked_as_containing_an_accessible_scope_address_of_operator<T>();
			base_class::swap(rhs);
		}

		/* This type can be safely used as a function return value if the element it contains is also safely returnable. */
		template<class T2 = T, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<T2, T>::value) && (
			(std::integral_constant<bool, mse::impl::HasXScopeReturnableTagMethod<T2>::Has>()) || (mse::impl::is_potentially_not_xscope<T2>::value)
			)> MSE_IMPL_EIS >
		void xscope_returnable_tag() const {} /* Indication that this type can be used as a function return value. */

		MSE_INHERIT_XSCOPE_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(T);

	private:
		/* If T is "marked" as containing a scope reference, then the following member function
		will not instantiate, causing an (intended) compile error. */
		template<class T2, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<T2, T>::value)
			&& (mse::impl::potentially_does_not_contain_non_owning_scope_reference<T2>::value)> MSE_IMPL_EIS >
		void valid_if_T_is_not_marked_as_containing_a_scope_reference() const {}

		/* If T is "marked" as containing an accessible "scope address of" operator, then the following member function
		will not instantiate, causing an (intended) compile error. */
		template<class T2, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<T2, T>::value)
			&& (mse::impl::is_potentially_not_referenceable_by_scope_pointer<T2>::value)
			> MSE_IMPL_EIS >
		void valid_if_T_is_not_marked_as_containing_an_accessible_scope_address_of_operator() const {}

		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
	};

#ifdef MSE_HAS_CXX17
	/* deduction guides */
	template<class T>
	xscope_optional(T)->xscope_optional<T>;
#endif /* MSE_HAS_CXX17 */

	template <class T>
	constexpr xscope_optional<typename std::decay<T>::type> make_xscope_optional(T&& v) {
		return xscope_optional<typename std::decay<T>::type>(constexpr_forward<T>(v));
	}
	template <class X>
	constexpr xscope_optional<X&> make_xscope_optional(std::reference_wrapper<X> v) {
		return xscope_optional<X&>(v.get());
	}


	template <class T>
	class xscope_mt_optional : public mse::us::impl::ns_optional::optional_base2<T, mse::shareable_dynamic_container_mutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag>, public mse::us::impl::XScopeTagBase
		, MSE_INHERIT_XSCOPE_TAG_BASE_SET_FROM(T, xscope_mt_optional<T>)
	{
	public:
		typedef mse::us::impl::ns_optional::optional_base2<T, mse::shareable_dynamic_container_mutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag> base_class;
		typedef typename base_class::value_type value_type;

#ifdef MSE_HAS_CXX17
#ifdef MSE_OPTIONAL_IMPLEMENTATION1

		MSE_OPTIONAL_USING(xscope_mt_optional, base_class);

		template<class... Tpes, class = std::enable_if_t<std::is_constructible_v<T, Tpes...> > >
		constexpr explicit xscope_mt_optional(mse::in_place_t, Tpes&&... _Args) : base_class(mse::in_place, std::forward<Tpes>(_Args)...) {}
		template<class _Elem, class... Tpes, class = std::enable_if_t<std::is_constructible_v<T, std::initializer_list<_Elem>&, Tpes...> > >
		constexpr explicit xscope_mt_optional(mse::in_place_t, std::initializer_list<_Elem> _Ilist, Tpes&&... _Args)
			: base_class(mse::in_place, _Ilist, std::forward<Tpes>(_Args)...) {}

		template<class T2>
		using _AllowDirectConversion = std::integral_constant<bool, std::conjunction_v<
			//std::negation<std::is_same<std::remove_cv_t<std::remove_reference_t<T2>>, xscope_mt_optional>>,
			std::negation<std::is_base_of<base_class, std::remove_cv_t<std::remove_reference_t<T2>>>>,
			std::negation<std::is_same<std::remove_cv_t<std::remove_reference_t<T2>>, mse::in_place_t>>,
			std::is_constructible<T, T2> > >;
		template<class T2 = T, std::enable_if_t<std::conjunction_v<_AllowDirectConversion<T2>, std::is_convertible<T2, T>>, int> = 0>
		constexpr xscope_mt_optional(T2&& _Right) : base_class(mse::in_place, std::forward<T2>(_Right)) {}
		template<class T2 = T, std::enable_if_t<std::conjunction_v<_AllowDirectConversion<T2>, std::negation<std::is_convertible<T2, T> > >, int> = 0>
		constexpr explicit xscope_mt_optional(T2&& _Right) : base_class(mse::in_place, std::forward<T2>(_Right)) {}

#else // MSE_OPTIONAL_IMPLEMENTATION1
		using base_class::base_class;
		template<class T2, class = mse::impl::disable_if_is_a_pair_with_the_first_a_base_of_the_second_t<xscope_mt_optional, T2> >
		explicit xscope_mt_optional(T2&& _X) : base_class(MSE_FWD(_X)) {}
#endif // MSE_OPTIONAL_IMPLEMENTATION1

		xscope_mt_optional(const base_class& src) : base_class(src) {}
		xscope_mt_optional(base_class&& src) : base_class(MSE_FWD(src)) {}

#else // MSE_HAS_CXX17
		using base_class::base_class;
		MSE_OPTIONAL_USING(xscope_mt_optional, base_class);
		template<class T2, class = mse::impl::disable_if_is_a_pair_with_the_first_a_base_of_the_second_t<xscope_mt_optional, T2> >
		explicit xscope_mt_optional(T2&& _X) : base_class(MSE_FWD(_X)) {}
#endif // MSE_HAS_CXX17

		xscope_mt_optional(const xscope_mt_optional& src_ref) : base_class(static_cast<const base_class&>(src_ref)) {}
		//xscope_mt_optional(const mstd::optional<T>& src_ref) : base_class(static_cast<const base_class&>(src_ref)) {}

		xscope_mt_optional& operator=(nullopt_t) noexcept {
			valid_if_T_is_not_marked_as_containing_an_accessible_scope_address_of_operator<T>();
			base_class::clear();
			return *this;
		}
		xscope_mt_optional& operator=(const xscope_mt_optional& rhs) {
			valid_if_T_is_not_marked_as_containing_a_scope_reference<T>();
			valid_if_T_is_not_marked_as_containing_an_accessible_scope_address_of_operator<T>();
			base_class::operator=(rhs);
			return *this;
		}
		xscope_mt_optional& operator=(xscope_mt_optional&& rhs) noexcept(std::is_nothrow_move_assignable<T>::value && std::is_nothrow_move_constructible<T>::value) {
			valid_if_T_is_not_marked_as_containing_a_scope_reference<T>();
			valid_if_T_is_not_marked_as_containing_an_accessible_scope_address_of_operator<T>();
			base_class::operator=(mse::us::impl::as_ref<base_class>(MSE_FWD(rhs)));
			return *this;
		}
		template <class U, class = mse::impl::disable_if_is_a_pair_with_the_first_a_base_of_the_second_t<xscope_mt_optional, U> >
		auto operator=(U&& v) -> mse::impl::enable_if_t<std::is_same<typename std::decay<U>::type, T>::value, xscope_mt_optional&> {
			valid_if_T_is_not_marked_as_containing_a_scope_reference<T>();
			valid_if_T_is_not_marked_as_containing_an_accessible_scope_address_of_operator<T>();
			base_class::operator=(std::forward<U>(v));
			return *this;
		}
		template <class... Args>
		void emplace(Args&&... args) {
			valid_if_T_is_not_marked_as_containing_a_scope_reference<T>();
			valid_if_T_is_not_marked_as_containing_an_accessible_scope_address_of_operator<T>();
			base_class::emplace(std::forward<Args>(args)...);
		}
		template <class U, class... Args>
		void emplace(std::initializer_list<U> il, Args&&... args) {
			valid_if_T_is_not_marked_as_containing_a_scope_reference<T>();
			valid_if_T_is_not_marked_as_containing_an_accessible_scope_address_of_operator<T>();
			base_class::emplace(il, std::forward<Args>(args)...);
		}
		void reset() noexcept {
			valid_if_T_is_not_marked_as_containing_an_accessible_scope_address_of_operator<T>();
			base_class::reset();
		}
		void swap(xscope_mt_optional<T>& rhs) noexcept(std::is_nothrow_move_constructible<T>::value && noexcept(std::swap(std::declval<T&>(), std::declval<T&>()))) {
			valid_if_T_is_not_marked_as_containing_a_scope_reference<T>();
			valid_if_T_is_not_marked_as_containing_an_accessible_scope_address_of_operator<T>();
			base_class::swap(rhs);
		}

		/* This type can be safely used as a function return value if the element it contains is also safely returnable. */
		template<class T2 = T, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<T2, T>::value) && (
			(std::integral_constant<bool, mse::impl::HasXScopeReturnableTagMethod<T2>::Has>()) || (mse::impl::is_potentially_not_xscope<T2>::value)
			)> MSE_IMPL_EIS >
			void xscope_returnable_tag() const {} /* Indication that this type is can be used as a function return value. */

		MSE_INHERIT_XSCOPE_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(T);

	private:
		/* If T is "marked" as containing a scope reference, then the following member function
		will not instantiate, causing an (intended) compile error. */
		template<class T2, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<T2, T>::value)
			&& (mse::impl::potentially_does_not_contain_non_owning_scope_reference<T2>::value)> MSE_IMPL_EIS >
			void valid_if_T_is_not_marked_as_containing_a_scope_reference() const {}

		/* If T is "marked" as containing an accessible "scope address of" operator, then the following member function
		will not instantiate, causing an (intended) compile error. */
		template<class T2, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<T2, T>::value)
			&& (mse::impl::is_potentially_not_referenceable_by_scope_pointer<T2>::value)
			> MSE_IMPL_EIS >
			void valid_if_T_is_not_marked_as_containing_an_accessible_scope_address_of_operator() const {}

		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
	};

#ifdef MSE_HAS_CXX17
	/* deduction guides */
	template<class T>
	xscope_mt_optional(T)->xscope_mt_optional<T>;
#endif /* MSE_HAS_CXX17 */

	template <class T>
	constexpr xscope_mt_optional<typename std::decay<T>::type> make_xscope_mt_optional(T&& v) {
		return xscope_mt_optional<typename std::decay<T>::type>(constexpr_forward<T>(v));
	}
	template <class X>
	constexpr xscope_mt_optional<X&> make_xscope_mt_optional(std::reference_wrapper<X> v) {
		return xscope_mt_optional<X&>(v.get());
	}


	template <class T>
	class xscope_st_optional : public mse::us::impl::ns_optional::optional_base2<T, mse::non_thread_safe_shared_mutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag>, public mse::us::impl::XScopeTagBase
		, MSE_INHERIT_XSCOPE_TAG_BASE_SET_FROM(T, xscope_st_optional<T>)
	{
	public:
		typedef mse::us::impl::ns_optional::optional_base2<T, mse::non_thread_safe_shared_mutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag> base_class;
		typedef typename base_class::value_type value_type;

#ifdef MSE_HAS_CXX17
#ifdef MSE_OPTIONAL_IMPLEMENTATION1

		MSE_OPTIONAL_USING(xscope_st_optional, base_class);

		template<class... Tpes, class = std::enable_if_t<std::is_constructible_v<T, Tpes...> > >
		constexpr explicit xscope_st_optional(mse::in_place_t, Tpes&&... _Args) : base_class(mse::in_place, std::forward<Tpes>(_Args)...) {}
		template<class _Elem, class... Tpes, class = std::enable_if_t<std::is_constructible_v<T, std::initializer_list<_Elem>&, Tpes...> > >
		constexpr explicit xscope_st_optional(mse::in_place_t, std::initializer_list<_Elem> _Ilist, Tpes&&... _Args)
			: base_class(mse::in_place, _Ilist, std::forward<Tpes>(_Args)...) {}

		template<class T2>
		using _AllowDirectConversion = std::integral_constant<bool, std::conjunction_v<
			//std::negation<std::is_same<std::remove_cv_t<std::remove_reference_t<T2>>, xscope_st_optional>>,
			std::negation<std::is_base_of<base_class, std::remove_cv_t<std::remove_reference_t<T2>>>>,
			std::negation<std::is_same<std::remove_cv_t<std::remove_reference_t<T2>>, mse::in_place_t>>,
			std::is_constructible<T, T2> > >;
		template<class T2 = T, std::enable_if_t<std::conjunction_v<_AllowDirectConversion<T2>, std::is_convertible<T2, T>>, int> = 0>
		constexpr xscope_st_optional(T2&& _Right) : base_class(mse::in_place, std::forward<T2>(_Right)) {}
		template<class T2 = T, std::enable_if_t<std::conjunction_v<_AllowDirectConversion<T2>, std::negation<std::is_convertible<T2, T> > >, int> = 0>
		constexpr explicit xscope_st_optional(T2&& _Right) : base_class(mse::in_place, std::forward<T2>(_Right)) {}

#else // MSE_OPTIONAL_IMPLEMENTATION1
		using base_class::base_class;
		template<class T2, class = mse::impl::disable_if_is_a_pair_with_the_first_a_base_of_the_second_t<xscope_st_optional, T2> >
		explicit xscope_st_optional(T2&& _X) : base_class(MSE_FWD(_X)) {}
#endif // MSE_OPTIONAL_IMPLEMENTATION1

		xscope_st_optional(const base_class& src) : base_class(src) {}
		xscope_st_optional(base_class&& src) : base_class(MSE_FWD(src)) {}

#else // MSE_HAS_CXX17
		using base_class::base_class;
		MSE_OPTIONAL_USING(xscope_st_optional, base_class);
		template<class T2, class = mse::impl::disable_if_is_a_pair_with_the_first_a_base_of_the_second_t<xscope_st_optional, T2> >
		explicit xscope_st_optional(T2&& _X) : base_class(MSE_FWD(_X)) {}
#endif // MSE_HAS_CXX17

		xscope_st_optional(const xscope_st_optional& src_ref) : base_class(static_cast<const base_class&>(src_ref)) {}
		//xscope_st_optional(const mstd::optional<T>& src_ref) : base_class(static_cast<const base_class&>(src_ref)) {}

		xscope_st_optional& operator=(nullopt_t) noexcept {
			valid_if_T_is_not_marked_as_containing_an_accessible_scope_address_of_operator<T>();
			base_class::clear();
			return *this;
		}
		xscope_st_optional& operator=(const xscope_st_optional& rhs) {
			valid_if_T_is_not_marked_as_containing_a_scope_reference<T>();
			valid_if_T_is_not_marked_as_containing_an_accessible_scope_address_of_operator<T>();
			base_class::operator=(rhs);
			return *this;
		}
		xscope_st_optional& operator=(xscope_st_optional&& rhs) noexcept(std::is_nothrow_move_assignable<T>::value && std::is_nothrow_move_constructible<T>::value) {
			valid_if_T_is_not_marked_as_containing_a_scope_reference<T>();
			valid_if_T_is_not_marked_as_containing_an_accessible_scope_address_of_operator<T>();
			base_class::operator=(mse::us::impl::as_ref<base_class>(MSE_FWD(rhs)));
			return *this;
		}
		template <class U, class = mse::impl::disable_if_is_a_pair_with_the_first_a_base_of_the_second_t<xscope_st_optional, U> >
		auto operator=(U&& v) -> mse::impl::enable_if_t<std::is_same<typename std::decay<U>::type, T>::value, xscope_st_optional&> {
			valid_if_T_is_not_marked_as_containing_a_scope_reference<T>();
			valid_if_T_is_not_marked_as_containing_an_accessible_scope_address_of_operator<T>();
			base_class::operator=(std::forward<U>(v));
			return *this;
		}
		template <class... Args>
		void emplace(Args&&... args) {
			valid_if_T_is_not_marked_as_containing_a_scope_reference<T>();
			valid_if_T_is_not_marked_as_containing_an_accessible_scope_address_of_operator<T>();
			base_class::emplace(std::forward<Args>(args)...);
		}
		template <class U, class... Args>
		void emplace(std::initializer_list<U> il, Args&&... args) {
			valid_if_T_is_not_marked_as_containing_a_scope_reference<T>();
			valid_if_T_is_not_marked_as_containing_an_accessible_scope_address_of_operator<T>();
			base_class::emplace(il, std::forward<Args>(args)...);
		}
		void reset() noexcept {
			valid_if_T_is_not_marked_as_containing_an_accessible_scope_address_of_operator<T>();
			base_class::reset();
		}
		void swap(xscope_st_optional<T>& rhs) noexcept(std::is_nothrow_move_constructible<T>::value && noexcept(std::swap(std::declval<T&>(), std::declval<T&>()))) {
			valid_if_T_is_not_marked_as_containing_a_scope_reference<T>();
			valid_if_T_is_not_marked_as_containing_an_accessible_scope_address_of_operator<T>();
			base_class::swap(rhs);
		}

		/* This type can be safely used as a function return value if the element it contains is also safely returnable. */
		template<class T2 = T, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<T2, T>::value) && (
			(std::integral_constant<bool, mse::impl::HasXScopeReturnableTagMethod<T2>::Has>()) || (mse::impl::is_potentially_not_xscope<T2>::value)
			)> MSE_IMPL_EIS >
			void xscope_returnable_tag() const {} /* Indication that this type is can be used as a function return value. */

	private:
		/* If T is "marked" as containing a scope reference, then the following member function
		will not instantiate, causing an (intended) compile error. */
		template<class T2, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<T2, T>::value)
			&& (mse::impl::potentially_does_not_contain_non_owning_scope_reference<T2>::value)> MSE_IMPL_EIS >
			void valid_if_T_is_not_marked_as_containing_a_scope_reference() const {}

		/* If T is "marked" as containing an accessible "scope address of" operator, then the following member function
		will not instantiate, causing an (intended) compile error. */
		template<class T2, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<T2, T>::value)
			&& (mse::impl::is_potentially_not_referenceable_by_scope_pointer<T2>::value)
			> MSE_IMPL_EIS >
			void valid_if_T_is_not_marked_as_containing_an_accessible_scope_address_of_operator() const {}

		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
	};

#ifdef MSE_HAS_CXX17
	/* deduction guides */
	template<class T>
	xscope_st_optional(T)->xscope_st_optional<T>;
#endif /* MSE_HAS_CXX17 */

	template <class T>
	constexpr xscope_st_optional<typename std::decay<T>::type> make_xscope_st_optional(T&& v) {
		return xscope_st_optional<typename std::decay<T>::type>(constexpr_forward<T>(v));
	}
	template <class X>
	constexpr xscope_st_optional<X&> make_xscope_st_optional(std::reference_wrapper<X> v) {
		return xscope_st_optional<X&>(v.get());
	}

#endif // !MSE_OPTIONAL_NO_XSCOPE_DEPENDENCE


	template<class T>
	class fixed_optional;
	template<class T>
	class xscope_fixed_optional;

	namespace us {
		namespace impl {
			namespace ns_optional {

				template <class T>
				class fixed_optional_base2
#ifdef MSE_HAS_CXX17
					: private mse::impl::TOpaqueWrapper<optional_base1<T> >, private container_adjusted_default_state_mutex<T>{
				public:
					typedef mse::impl::TOpaqueWrapper<optional_base1<T> > base_class;
					typedef optional_base1<T> _MO;

				private:
					const _MO& contained_optional() const& { return base_class::value(); }
					const _MO&& contained_optional() const&& { return base_class::value(); }
					_MO& contained_optional()& { return base_class::value(); }
					_MO&& contained_optional()&& {
						return std::move(base_class::value());
					}

				public:
#ifdef MSE_OPTIONAL_IMPLEMENTATION1

					MSE_OPTIONAL_USING(fixed_optional_base2, base_class);

					template<class... _Types, class = std::enable_if_t<std::is_constructible_v<T, _Types...> > >
					constexpr explicit fixed_optional_base2(in_place_t, _Types&&... _Args) : base_class(in_place, std::forward<_Types>(_Args)...) {}
					template<class _Elem, class... _Types, class = std::enable_if_t<std::is_constructible_v<T, std::initializer_list<_Elem>&, _Types...> > >
					constexpr explicit fixed_optional_base2(in_place_t, std::initializer_list<_Elem> _Ilist, _Types&&... _Args)
						: base_class(in_place, _Ilist, std::forward<_Types>(_Args)...) {}

					template<class T2>
					using _AllowDirectConversion = std::integral_constant<bool, std::conjunction_v<
						//std::negation<std::is_same<std::remove_cv_t<std::remove_reference_t<T2>>, fixed_optional_base2>>,
						std::negation<std::is_base_of<base_class, std::remove_cv_t<std::remove_reference_t<T2>>>>,
						std::negation<std::is_same<std::remove_cv_t<std::remove_reference_t<T2>>, in_place_t>>,
						std::is_constructible<T, T2> > >;
					template<class T2 = T, std::enable_if_t<std::conjunction_v<_AllowDirectConversion<T2>, std::is_convertible<T2, T>>, int> = 0>
					constexpr fixed_optional_base2(T2&& _Right) : base_class(in_place, std::forward<T2>(_Right)) {}
					template<class T2 = T, std::enable_if_t<std::conjunction_v<_AllowDirectConversion<T2>, std::negation<std::is_convertible<T2, T> > >, int> = 0>
					constexpr explicit fixed_optional_base2(T2&& _Right) : base_class(in_place, std::forward<T2>(_Right)) {}

#else // MSE_OPTIONAL_IMPLEMENTATION1
					using base_class::base_class;
					template<class T2, class = mse::impl::disable_if_is_a_pair_with_the_first_a_base_of_the_second_t<fixed_optional_base2, T2> >
					explicit fixed_optional_base2(T2&& _X) : base_class(MSE_FWD(_X)) {}
#endif // MSE_OPTIONAL_IMPLEMENTATION1

					fixed_optional_base2(const _MO& src) : base_class(src) {}
					//fixed_optional_base2(_MO&& src) : base_class(src) {}

#else // MSE_HAS_CXX17
					: public optional_base1<T>, private container_adjusted_default_state_mutex<T> {
				public:
					typedef optional_base1<T> base_class;
					typedef base_class _MO;

				private:
					const _MO& contained_optional() const& { return (*this); }
					const _MO& contained_optional() const&& { return (*this); }
					_MO& contained_optional()& { return (*this); }
					auto contained_optional()&& {
						return mse::us::impl::as_ref<base_class>(std::move(*this));
					}

				public:
					using base_class::base_class;
					MSE_OPTIONAL_USING(fixed_optional_base2, base_class);
					template<class T2, class = mse::impl::disable_if_is_a_pair_with_the_first_a_base_of_the_second_t<fixed_optional_base2, T2> >
					explicit fixed_optional_base2(T2&& _X) : base_class(_X) {}
#endif // MSE_HAS_CXX17

					typedef container_adjusted_default_state_mutex<T> state_mutex_t;
					typedef state_mutex_t _TStateMutex;

					typedef fixed_optional_base2 _Myt;
					typedef typename _MO::value_type value_type;

					fixed_optional_base2(const fixed_optional_base2& src_ref) : base_class((src_ref).contained_optional()) {}
					//fixed_optional_base2(fixed_optional_base2&& src_ref) : base_class((src_ref).contained_optional()) {}

					~fixed_optional_base2() {
						mse::impl::destructor_lock_guard1<state_mutex_t> lock1(state_mutex1());
					}

					constexpr explicit operator bool() const noexcept {
						return bool(contained_optional());
					}
					_NODISCARD constexpr bool has_value() const noexcept {
						return contained_optional().has_value();
					}

					_NODISCARD constexpr const T& value() const& {
						return contained_optional().value();
					}
					_NODISCARD constexpr T& value()& {
						return contained_optional().value();
					}
					_NODISCARD constexpr T&& value()&& {
						return std::move(MSE_FWD(contained_optional()).value());
					}
					_NODISCARD constexpr const T&& value() const&& {
						return std::move(MSE_FWD(contained_optional()).value());
					}

					template <class _Ty2>
					_NODISCARD constexpr T value_or(_Ty2&& _Right) const& {
						return contained_optional().value_or(std::forward<_Ty2>(_Right));
					}
					template <class _Ty2>
					_NODISCARD constexpr T value_or(_Ty2&& _Right)&& {
						return contained_optional().value_or(std::forward<_Ty2>(_Right));
					}

					_NODISCARD constexpr const T* operator->() const& {
						return std::addressof((*this).value());
					}
					_NODISCARD constexpr const T* operator->() const&& = delete;
					_NODISCARD constexpr T* operator->()& {
						return std::addressof((*this).value());
					}
					_NODISCARD constexpr const T* operator->() && = delete;
					_NODISCARD constexpr const T& operator*() const& {
						return (*this).value();
					}
					_NODISCARD constexpr T& operator*()& {
						return (*this).value();
					}
					_NODISCARD constexpr T&& operator*()&& {
						return std::move((*this).value());
					}
					_NODISCARD constexpr const T&& operator*() const&& {
						return std::move((*this).value());
					}

					MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(T);

				private:

					state_mutex_t& state_mutex1()& { return (*this); }

					MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

					friend class fixed_optional<T>;
					friend class xscope_fixed_optional<T>;
				};

				// 20.5.8, Relational operators
				template <class T> constexpr bool operator==(const fixed_optional_base2<T>& x, const fixed_optional_base2<T>& y)
				{
					return bool(x) != bool(y) ? false : bool(x) == false ? true : *x == *y;
				}

				template <class T> constexpr bool operator!=(const fixed_optional_base2<T>& x, const fixed_optional_base2<T>& y)
				{
					return !(x == y);
				}

				template <class T> constexpr bool operator<(const fixed_optional_base2<T>& x, const fixed_optional_base2<T>& y)
				{
					return (!y) ? false : (!x) ? true : *x < *y;
				}

				template <class T> constexpr bool operator>(const fixed_optional_base2<T>& x, const fixed_optional_base2<T>& y)
				{
					return (y < x);
				}

				template <class T> constexpr bool operator<=(const fixed_optional_base2<T>& x, const fixed_optional_base2<T>& y)
				{
					return !(y < x);
				}

				template <class T> constexpr bool operator>=(const fixed_optional_base2<T>& x, const fixed_optional_base2<T>& y)
				{
					return !(x < y);
				}


				// 20.5.9, Comparison with nullopt
				template <class T> constexpr bool operator==(const fixed_optional_base2<T>& x, nullopt_t_base) noexcept
				{
					return (!x);
				}

				template <class T> constexpr bool operator==(nullopt_t_base, const fixed_optional_base2<T>& x) noexcept
				{
					return (!x);
				}

				template <class T> constexpr bool operator!=(const fixed_optional_base2<T>& x, nullopt_t_base) noexcept
				{
					return bool(x);
				}

				template <class T> constexpr bool operator!=(nullopt_t_base, const fixed_optional_base2<T>& x) noexcept
				{
					return bool(x);
				}

				template <class T> constexpr bool operator<(const fixed_optional_base2<T>&, nullopt_t_base) noexcept
				{
					return false;
				}

				template <class T> constexpr bool operator<(nullopt_t_base, const fixed_optional_base2<T>& x) noexcept
				{
					return bool(x);
				}

				template <class T> constexpr bool operator<=(const fixed_optional_base2<T>& x, nullopt_t_base) noexcept
				{
					return (!x);
				}

				template <class T> constexpr bool operator<=(nullopt_t_base, const fixed_optional_base2<T>&) noexcept
				{
					return true;
				}

				template <class T> constexpr bool operator>(const fixed_optional_base2<T>& x, nullopt_t_base) noexcept
				{
					return bool(x);
				}

				template <class T> constexpr bool operator>(nullopt_t_base, const fixed_optional_base2<T>&) noexcept
				{
					return false;
				}

				template <class T> constexpr bool operator>=(const fixed_optional_base2<T>&, nullopt_t_base) noexcept
				{
					return true;
				}

				template <class T> constexpr bool operator>=(nullopt_t_base, const fixed_optional_base2<T>& x) noexcept
				{
					return (!x);
				}


				// 20.5.10, Comparison with T
				template <class T> constexpr bool operator==(const fixed_optional_base2<T>& x, const T& v)
				{
					return bool(x) ? *x == v : false;
				}

				template <class T> constexpr bool operator==(const T& v, const fixed_optional_base2<T>& x)
				{
					return bool(x) ? v == *x : false;
				}

				template <class T> constexpr bool operator!=(const fixed_optional_base2<T>& x, const T& v)
				{
					return bool(x) ? *x != v : true;
				}

				template <class T> constexpr bool operator!=(const T& v, const fixed_optional_base2<T>& x)
				{
					return bool(x) ? v != *x : true;
				}

				template <class T> constexpr bool operator<(const fixed_optional_base2<T>& x, const T& v)
				{
					return bool(x) ? *x < v : true;
				}

				template <class T> constexpr bool operator>(const T& v, const fixed_optional_base2<T>& x)
				{
					return bool(x) ? v > * x : true;
				}

				template <class T> constexpr bool operator>(const fixed_optional_base2<T>& x, const T& v)
				{
					return bool(x) ? *x > v : false;
				}

				template <class T> constexpr bool operator<(const T& v, const fixed_optional_base2<T>& x)
				{
					return bool(x) ? v < *x : false;
				}

				template <class T> constexpr bool operator>=(const fixed_optional_base2<T>& x, const T& v)
				{
					return bool(x) ? *x >= v : false;
				}

				template <class T> constexpr bool operator<=(const T& v, const fixed_optional_base2<T>& x)
				{
					return bool(x) ? v <= *x : false;
				}

				template <class T> constexpr bool operator<=(const fixed_optional_base2<T>& x, const T& v)
				{
					return bool(x) ? *x <= v : true;
				}

				template <class T> constexpr bool operator>=(const T& v, const fixed_optional_base2<T>& x)
				{
					return bool(x) ? v >= *x : true;
				}


				// Comparison of optional<T&> with T
				template <class T> constexpr bool operator==(const fixed_optional_base2<T&>& x, const T& v)
				{
					return bool(x) ? *x == v : false;
				}

				template <class T> constexpr bool operator==(const T& v, const fixed_optional_base2<T&>& x)
				{
					return bool(x) ? v == *x : false;
				}

				template <class T> constexpr bool operator!=(const fixed_optional_base2<T&>& x, const T& v)
				{
					return bool(x) ? *x != v : true;
				}

				template <class T> constexpr bool operator!=(const T& v, const fixed_optional_base2<T&>& x)
				{
					return bool(x) ? v != *x : true;
				}

				template <class T> constexpr bool operator<(const fixed_optional_base2<T&>& x, const T& v)
				{
					return bool(x) ? *x < v : true;
				}

				template <class T> constexpr bool operator>(const T& v, const fixed_optional_base2<T&>& x)
				{
					return bool(x) ? v > * x : true;
				}

				template <class T> constexpr bool operator>(const fixed_optional_base2<T&>& x, const T& v)
				{
					return bool(x) ? *x > v : false;
				}

				template <class T> constexpr bool operator<(const T& v, const fixed_optional_base2<T&>& x)
				{
					return bool(x) ? v < *x : false;
				}

				template <class T> constexpr bool operator>=(const fixed_optional_base2<T&>& x, const T& v)
				{
					return bool(x) ? *x >= v : false;
				}

				template <class T> constexpr bool operator<=(const T& v, const fixed_optional_base2<T&>& x)
				{
					return bool(x) ? v <= *x : false;
				}

				template <class T> constexpr bool operator<=(const fixed_optional_base2<T&>& x, const T& v)
				{
					return bool(x) ? *x <= v : true;
				}

				template <class T> constexpr bool operator>=(const T& v, const fixed_optional_base2<T&>& x)
				{
					return bool(x) ? v >= *x : true;
				}

				// Comparison of optional<T const&> with T
				template <class T> constexpr bool operator==(const fixed_optional_base2<const T&>& x, const T& v)
				{
					return bool(x) ? *x == v : false;
				}

				template <class T> constexpr bool operator==(const T& v, const fixed_optional_base2<const T&>& x)
				{
					return bool(x) ? v == *x : false;
				}

				template <class T> constexpr bool operator!=(const fixed_optional_base2<const T&>& x, const T& v)
				{
					return bool(x) ? *x != v : true;
				}

				template <class T> constexpr bool operator!=(const T& v, const fixed_optional_base2<const T&>& x)
				{
					return bool(x) ? v != *x : true;
				}

				template <class T> constexpr bool operator<(const fixed_optional_base2<const T&>& x, const T& v)
				{
					return bool(x) ? *x < v : true;
				}

				template <class T> constexpr bool operator>(const T& v, const fixed_optional_base2<const T&>& x)
				{
					return bool(x) ? v > * x : true;
				}

				template <class T> constexpr bool operator>(const fixed_optional_base2<const T&>& x, const T& v)
				{
					return bool(x) ? *x > v : false;
				}

				template <class T> constexpr bool operator<(const T& v, const fixed_optional_base2<const T&>& x)
				{
					return bool(x) ? v < *x : false;
				}

				template <class T> constexpr bool operator>=(const fixed_optional_base2<const T&>& x, const T& v)
				{
					return bool(x) ? *x >= v : false;
				}

				template <class T> constexpr bool operator<=(const T& v, const fixed_optional_base2<const T&>& x)
				{
					return bool(x) ? v <= *x : false;
				}

				template <class T> constexpr bool operator<=(const fixed_optional_base2<const T&>& x, const T& v)
				{
					return bool(x) ? *x <= v : true;
				}

				template <class T> constexpr bool operator>=(const T& v, const fixed_optional_base2<const T&>& x)
				{
					return bool(x) ? v >= *x : true;
				}
			}
		}
	}

	template<class T>
	class fixed_optional : public mse::us::impl::ns_optional::fixed_optional_base2<T> {
	public:
		typedef mse::us::impl::ns_optional::fixed_optional_base2<T> base_class;
		typedef mse::us::impl::ns_optional::optional_base1<T> std_optional;
		typedef std_optional _MO;
		typedef fixed_optional _Myt;
		typedef typename base_class::value_type value_type;

	private:
		const _MO& contained_optional() const& { return base_class::contained_optional(); }
		//const _MO& contained_optional() const&& { return base_class::contained_optional(); }
		_MO& contained_optional()& { return base_class::contained_optional(); }
		_MO&& contained_optional()&& { return std::forward<_MO>(base_class::contained_optional()); }

	public:
#ifdef MSE_HAS_CXX17
#ifdef MSE_OPTIONAL_IMPLEMENTATION1

		MSE_OPTIONAL_USING(fixed_optional, base_class);

		template<class... _Types, class = std::enable_if_t<std::is_constructible_v<T, _Types...> > >
		constexpr explicit fixed_optional(in_place_t, _Types&&... _Args) : base_class(in_place, std::forward<_Types>(_Args)...) {}
		template<class _Elem, class... _Types, class = std::enable_if_t<std::is_constructible_v<T, std::initializer_list<_Elem>&, _Types...> > >
		constexpr explicit fixed_optional(in_place_t, std::initializer_list<_Elem> _Ilist, _Types&&... _Args)
			: base_class(in_place, _Ilist, std::forward<_Types>(_Args)...) {}

		template<class T2>
		using _AllowDirectConversion = std::integral_constant<bool, std::conjunction_v<
			//std::negation<std::is_same<std::remove_cv_t<std::remove_reference_t<T2>>, fixed_optional>>,
			std::negation<std::is_base_of<base_class, std::remove_cv_t<std::remove_reference_t<T2>>>>,
			std::negation<std::is_same<std::remove_cv_t<std::remove_reference_t<T2>>, in_place_t>>,
			std::is_constructible<T, T2> > >;
		template<class T2 = T, std::enable_if_t<std::conjunction_v<_AllowDirectConversion<T2>, std::is_convertible<T2, T>>, int> = 0>
		constexpr fixed_optional(T2&& _Right) : base_class(in_place, std::forward<T2>(_Right)) {}
		template<class T2 = T, std::enable_if_t<std::conjunction_v<_AllowDirectConversion<T2>, std::negation<std::is_convertible<T2, T> > >, int> = 0>
		constexpr explicit fixed_optional(T2&& _Right) : base_class(in_place, std::forward<T2>(_Right)) {}

#else // MSE_OPTIONAL_IMPLEMENTATION1
		using base_class::base_class;
		template<class T2, class = mse::impl::disable_if_is_a_pair_with_the_first_a_base_of_the_second_t<fixed_optional, T2> >
		explicit fixed_optional(T2&& _X) : base_class(MSE_FWD(_X)) {}
#endif // MSE_OPTIONAL_IMPLEMENTATION1

		fixed_optional(const base_class& src) : base_class(src) {}
		fixed_optional(base_class&& src) : base_class(MSE_FWD(src)) {}

#else // MSE_HAS_CXX17
		using base_class::base_class;
		MSE_OPTIONAL_USING(fixed_optional, base_class);
		template<class T2, class = mse::impl::disable_if_is_a_pair_with_the_first_a_base_of_the_second_t<fixed_optional, T2> >
		explicit fixed_optional(T2&& _X) : base_class(MSE_FWD(_X)) {}
#endif // MSE_HAS_CXX17

		fixed_optional(const fixed_optional& src) : base_class(mse::us::impl::as_ref<base_class>(src)) {}
		//fixed_optional(fixed_optional&& src) : base_class(mse::us::impl::as_ref<base_class>(MSE_FWD(src))) {}

		~fixed_optional() {
#ifndef MSE_OPTIONAL_NO_XSCOPE_DEPENDENCE
			mse::impl::T_valid_if_not_an_xscope_type<T>();
#endif // !MSE_OPTIONAL_NO_XSCOPE_DEPENDENCE
		}

		MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(T);

	private:
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;
	};

#ifdef MSE_HAS_CXX17
	template<class _Ty>
	fixed_optional(_Ty)->fixed_optional<_Ty>;
#endif /* MSE_HAS_CXX17 */

	template <class T>
	constexpr fixed_optional<typename std::decay<T>::type> make_fixed_optional(T&& v) {
		return fixed_optional<typename std::decay<T>::type>(constexpr_forward<T>(v));
	}
	template <class X>
	constexpr fixed_optional<X&> make_fixed_optional(std::reference_wrapper<X> v) {
		return fixed_optional<X&>(v.get());
	}

	template<class T>
	class xscope_fixed_optional : public mse::us::impl::ns_optional::fixed_optional_base2<T>, public mse::us::impl::XScopeTagBase
		, MSE_INHERIT_XSCOPE_TAG_BASE_SET_FROM(T, xscope_fixed_optional<T>)
	{
	public:
		typedef mse::us::impl::ns_optional::fixed_optional_base2<T> base_class;
		typedef mse::us::impl::ns_optional::optional_base1<T> std_optional;
		typedef std_optional _MO;
		typedef xscope_fixed_optional _Myt;
		typedef typename base_class::value_type value_type;

	private:
		const _MO& contained_optional() const& { return base_class::contained_optional(); }
		//const _MO& contained_optional() const&& { return base_class::contained_optional(); }
		_MO& contained_optional()& { return base_class::contained_optional(); }
		_MO&& contained_optional()&& { return std::forward<_MO>(base_class::contained_optional()); }

	public:
#ifdef MSE_HAS_CXX17
#ifdef MSE_OPTIONAL_IMPLEMENTATION1

		MSE_OPTIONAL_USING(xscope_fixed_optional, base_class);

		template<class... _Types, class = std::enable_if_t<std::is_constructible_v<T, _Types...> > >
		constexpr explicit xscope_fixed_optional(in_place_t, _Types&&... _Args) : base_class(in_place, std::forward<_Types>(_Args)...) {}
		template<class _Elem, class... _Types, class = std::enable_if_t<std::is_constructible_v<T, std::initializer_list<_Elem>&, _Types...> > >
		constexpr explicit xscope_fixed_optional(in_place_t, std::initializer_list<_Elem> _Ilist, _Types&&... _Args)
			: base_class(in_place, _Ilist, std::forward<_Types>(_Args)...) {}

		template<class T2>
		using _AllowDirectConversion = std::integral_constant<bool, std::conjunction_v<
			//std::negation<std::is_same<std::remove_cv_t<std::remove_reference_t<T2>>, xscope_fixed_optional>>,
			std::negation<std::is_base_of<base_class, std::remove_cv_t<std::remove_reference_t<T2>>>>,
			std::negation<std::is_same<std::remove_cv_t<std::remove_reference_t<T2>>, in_place_t>>,
			std::is_constructible<T, T2> > >;
		template<class T2 = T, std::enable_if_t<std::conjunction_v<_AllowDirectConversion<T2>, std::is_convertible<T2, T>>, int> = 0>
		constexpr xscope_fixed_optional(T2&& _Right) : base_class(in_place, std::forward<T2>(_Right)) {}
		template<class T2 = T, std::enable_if_t<std::conjunction_v<_AllowDirectConversion<T2>, std::negation<std::is_convertible<T2, T> > >, int> = 0>
		constexpr explicit xscope_fixed_optional(T2&& _Right) : base_class(in_place, std::forward<T2>(_Right)) {}

#else // MSE_OPTIONAL_IMPLEMENTATION1
		using base_class::base_class;
		template<class T2, class = mse::impl::disable_if_is_a_pair_with_the_first_a_base_of_the_second_t<xscope_fixed_optional, T2> >
		explicit xscope_fixed_optional(T2&& _X) : base_class(MSE_FWD(_X)) {}
#endif // MSE_OPTIONAL_IMPLEMENTATION1

		xscope_fixed_optional(const base_class& src) : base_class(src) {}
		xscope_fixed_optional(base_class&& src) : base_class(MSE_FWD(src)) {}

#else // MSE_HAS_CXX17
		using base_class::base_class;
		MSE_OPTIONAL_USING(xscope_fixed_optional, base_class);
		template<class T2, class = mse::impl::disable_if_is_a_pair_with_the_first_a_base_of_the_second_t<xscope_fixed_optional, T2> >
		explicit xscope_fixed_optional(T2&& _X) : base_class(MSE_FWD(_X)) {}
#endif // MSE_HAS_CXX17

		xscope_fixed_optional(const xscope_fixed_optional& src) : base_class(mse::us::impl::as_ref<base_class>(src)) {}
		//xscope_fixed_optional(xscope_fixed_optional&& src) : base_class(mse::us::impl::as_ref<base_class>(MSE_FWD(src))) {}

		MSE_INHERIT_XSCOPE_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(T);

	private:
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;
	};

#ifdef MSE_HAS_CXX17
	template<class _Ty>
	xscope_fixed_optional(_Ty)->xscope_fixed_optional<_Ty>;
#endif /* MSE_HAS_CXX17 */

	template <class T>
	constexpr xscope_fixed_optional<typename std::decay<T>::type> make_xscope_fixed_optional(T&& v) {
		return xscope_fixed_optional<typename std::decay<T>::type>(constexpr_forward<T>(v));
	}
	template <class X>
	constexpr xscope_fixed_optional<X&> make_xscope_fixed_optional(std::reference_wrapper<X> v) {
		return xscope_fixed_optional<X&>(v.get());
	}

	template <class _TLender, class T = mse::impl::target_type<_TLender> >
	class xscope_borrowing_fixed_optional {
	public:
		xscope_borrowing_fixed_optional(xscope_borrowing_fixed_optional&&) = default;

		xscope_borrowing_fixed_optional(const mse::TXScopeFixedPointer<_TLender>& src_xs_ptr) : m_src_ref(*src_xs_ptr) {
			m_src_ref.structure_change_lock(); m_src_ref.access_lock();
		}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		xscope_borrowing_fixed_optional(_TLender* src_xs_ptr) : m_src_ref(*src_xs_ptr) {
			m_src_ref.structure_change_lock(); m_src_ref.access_lock();
		}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
		~xscope_borrowing_fixed_optional() {
			m_src_ref.access_unlock(); m_src_ref.structure_change_unlock();
		}

		constexpr explicit operator bool() const noexcept {
			return bool(m_src_ref.unchecked_contained_optional());
		}
		_NODISCARD constexpr bool has_value() const noexcept {
			return m_src_ref.unchecked_contained_optional().has_value();
		}

		_NODISCARD constexpr const T& value() const& {
			return m_src_ref.unchecked_contained_optional().value();
		}
		_NODISCARD constexpr T& value()& {
			return m_src_ref.unchecked_contained_optional().value();
		}
		_NODISCARD constexpr T&& value()&& {
			return std::move(MSE_FWD(m_src_ref.unchecked_contained_optional()).value());
		}
		_NODISCARD constexpr const T&& value() const&& {
			return std::move(MSE_FWD(m_src_ref.unchecked_contained_optional()).value());
		}

		template <class _Ty2>
		_NODISCARD constexpr T value_or(_Ty2&& _Right) const& {
			return m_src_ref.unchecked_contained_optional().value_or(std::forward<_Ty2>(_Right));
		}
		template <class _Ty2>
		_NODISCARD constexpr T value_or(_Ty2&& _Right)&& {
			return m_src_ref.unchecked_contained_optional().value_or(std::forward<_Ty2>(_Right));
		}

		_NODISCARD constexpr const T* operator->() const& {
			return std::addressof((*this).value());
		}
		_NODISCARD constexpr const T* operator->() const&& = delete;
		_NODISCARD constexpr T* operator->()& {
			return std::addressof((*this).value());
		}
		_NODISCARD constexpr const T* operator->() && = delete;
		_NODISCARD constexpr const T& operator*() const& {
			return (*this).value();
		}
		_NODISCARD constexpr T& operator*()& {
			return (*this).value();
		}
		_NODISCARD constexpr T&& operator*()&& {
			return std::move((*this).value());
		}
		_NODISCARD constexpr const T&& operator*() const&& {
			return std::move((*this).value());
		}

		MSE_INHERIT_XSCOPE_ASYNC_SHAREABILITY_OF(T);

	private:
#ifdef MSE_HAS_CXX17
		xscope_borrowing_fixed_optional(const xscope_borrowing_fixed_optional&) = delete;
#endif // MSE_HAS_CXX17

		_TLender& m_src_ref;
	};

	template<class _TLender>
	auto make_xscope_borrowing_fixed_optional(const mse::TXScopeFixedPointer<_TLender>& src_xs_ptr) -> xscope_borrowing_fixed_optional<_TLender> {
		return xscope_borrowing_fixed_optional<_TLender>(src_xs_ptr);
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _TLender>
	auto make_xscope_borrowing_fixed_optional(_TLender* src_xs_ptr) -> xscope_borrowing_fixed_optional<_TLender> {
		return xscope_borrowing_fixed_optional<_TLender>(src_xs_ptr);
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
}

namespace std
{
#ifndef MSE_HAS_CXX17
	template <class T, class _TStateMutex, class TConstLockableIndicator>
	struct hash<mse::us::impl::ns_optional::optional_base2<T, _TStateMutex, TConstLockableIndicator>>
	{
		typedef typename hash<T>::result_type result_type;
		typedef mse::us::impl::ns_optional::optional_base2<T, _TStateMutex, TConstLockableIndicator> argument_type;

		constexpr result_type operator()(argument_type const& arg) const {
			return arg ? std::hash<T>{}(*arg) : result_type{};
		}
	};

	template <class T, class _TStateMutex, class TConstLockableIndicator>
	struct hash<mse::us::impl::ns_optional::optional_base2<T&, _TStateMutex, TConstLockableIndicator>>
	{
		typedef typename hash<T>::result_type result_type;
		typedef mse::us::impl::ns_optional::optional_base2<T&, _TStateMutex, TConstLockableIndicator> argument_type;

		constexpr result_type operator()(argument_type const& arg) const {
			return arg ? std::hash<T>{}(*arg) : result_type{};
		}
	};
#endif // !MSE_HAS_CXX17

	template <typename T>
	struct hash<mse::mstd::optional<T>>
	{
		typedef typename hash<T>::result_type result_type;
		typedef mse::mstd::optional<T> argument_type;

		constexpr result_type operator()(argument_type const& arg) const {
			return arg ? std::hash<T>{}(*arg) : result_type{};
		}
	};

	template <typename T>
	struct hash<mse::mstd::optional<T&>>
	{
		typedef typename hash<T>::result_type result_type;
		typedef mse::mstd::optional<T&> argument_type;

		constexpr result_type operator()(argument_type const& arg) const {
			return arg ? std::hash<T>{}(*arg) : result_type{};
		}
	};

	template <typename T>
	struct hash<mse::xscope_optional<T>>
	{
		typedef typename hash<T>::result_type result_type;
		typedef mse::xscope_optional<T> argument_type;

		constexpr result_type operator()(argument_type const& arg) const {
			return arg ? std::hash<T>{}(*arg) : result_type{};
		}
	};

	template <typename T>
	struct hash<mse::xscope_optional<T&>>
	{
		typedef typename hash<T>::result_type result_type;
		typedef mse::xscope_optional<T&> argument_type;

		constexpr result_type operator()(argument_type const& arg) const {
			return arg ? std::hash<T>{}(*arg) : result_type{};
		}
	};
}

# undef TR2_OPTIONAL_REQUIRES
# undef TR2_OPTIONAL_ASSERTED_EXPRESSION

namespace mse {

	namespace impl {
		template <typename _Ty> struct is_thread_safety_enforcing_mutex_msemsearray : std::integral_constant<bool, (std::is_same<_Ty, std::mutex>::value)
#ifdef MSE_HAS_CXX17
			|| (std::is_same<_Ty, std::shared_mutex>::value)
#endif // MSE_HAS_CXX17
			|| (std::is_same<_Ty, std::timed_mutex>::value) || (std::is_same<_Ty, std::shared_timed_mutex>::value)> {};

		template <typename _Ty> struct is_exclusive_writer_enforcing_mutex_msemsearray : std::integral_constant<bool, (is_thread_safety_enforcing_mutex_msemsearray<_Ty>::value)
			|| (std::is_same<_Ty, mse::non_thread_safe_shared_mutex>::value) || (std::is_same<_Ty, mse::non_thread_safe_mutex>::value)> {};
		template<class _Ty, MSE_IMPL_EIP mse::impl::enable_if_t<(is_exclusive_writer_enforcing_mutex_msemsearray<_Ty>::value)> MSE_IMPL_EIS >
		void T_valid_if_is_exclusive_writer_enforcing_mutex_msemsearray() {}

		template <typename _Ty> struct is_supported_aco_mutex_msemsearray : std::integral_constant<bool, (is_exclusive_writer_enforcing_mutex_msemsearray<_Ty>::value)
			|| (std::is_same<_Ty, mse::non_thread_safe_recursive_shared_timed_mutex>::value)> {};
	}

	class recursive_shared_timed_mutex;

	namespace us {
		namespace impl {
			template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex> class TAccessControlledObjBase;
			template<typename _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex> class TAccessControlledPointerBase;
			template<typename _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex> class TAccessControlledConstPointerBase;
			template<typename _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex> class TAccessControlledExclusivePointerBase;
		}
	}

	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex> class TXScopeAccessControlledObj;
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex> class TAccessControlledObj;

	template<typename _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex> class TXScopeAccessControlledPointer;
	template<typename _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex> class TXScopeAccessControlledConstPointer;
	template<typename _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex> class TXScopeAccessControlledExclusivePointer;
	template<typename _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex> class TAccessControlledPointer;
	template<typename _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex> class TAccessControlledConstPointer;
	template<typename _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex> class TAccessControlledExclusivePointer;

	template<typename _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	TXScopeAccessControlledPointer<_Ty, _TAccessMutex> xscope_pointer_to_access_controlled_obj(const TXScopeFixedPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& aco_xscpptr);
	template<typename _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	TXScopeAccessControlledConstPointer<_Ty, _TAccessMutex> xscope_const_pointer_to_access_controlled_obj(const TXScopeFixedPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& aco_xscpptr);
	template<typename _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	TXScopeAccessControlledExclusivePointer<_Ty, _TAccessMutex> xscope_exclusive_pointer_to_access_controlled_obj(const TXScopeFixedPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& aco_xscpptr);
#ifndef MSE_SCOPEPOINTER_DISABLED
	template<typename _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	TXScopeAccessControlledPointer<_Ty, _TAccessMutex> xscope_pointer_to_access_controlled_obj(const TXScopeObjFixedPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& aco_xscpptr);
	template<typename _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	TXScopeAccessControlledConstPointer<_Ty, _TAccessMutex> xscope_const_pointer_to_access_controlled_obj(const TXScopeObjFixedPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& aco_xscpptr);
	template<typename _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	TXScopeAccessControlledExclusivePointer<_Ty, _TAccessMutex> xscope_exclusive_pointer_to_access_controlled_obj(const TXScopeObjFixedPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& aco_xscpptr);
#endif //!MSE_SCOPEPOINTER_DISABLED

	template<typename _TExclusiveStrongPointer, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex> class TXScopeExclusiveStrongPointerStoreForAccessControl;

	namespace us {
		namespace impl {
			template<typename _Ty, class _TAccessMutex/* = non_thread_safe_recursive_shared_timed_mutex*/>
			class TAccessControlledPointerBase : public mse::us::impl::StrongPointerTagBase {
			public:
				TAccessControlledPointerBase(const TAccessControlledPointerBase& src) : m_obj_ptr(src.m_obj_ptr), m_mutex_ptr(src.m_mutex_ptr), m_write_lock(*(src.m_mutex_ptr)) {}
				TAccessControlledPointerBase(TAccessControlledPointerBase&& src) = default; /* Note, the move constructor is only safe when std::move() is prohibited. */
				template<typename _Ty2>
				TAccessControlledPointerBase(const TAccessControlledPointerBase<_Ty2, _TAccessMutex>& src) : m_obj_ptr(src.m_obj_ptr), m_mutex_ptr(src.m_mutex_ptr), m_write_lock(*(src.m_mutex_ptr)) {}

				MSE_IMPL_DESTRUCTOR_PREFIX1 ~TAccessControlledPointerBase() {
					valid_if_TAccessMutex_is_supported<_TAccessMutex>();
				}

				operator bool() const {
					//assert(is_valid()); //{ MSE_THROW(asyncshared_use_of_invalid_pointer_error("attempt to use invalid pointer - mse::TAccessControlledPointerBase")); }
					return (nullptr != m_obj_ptr);
				}
				auto& operator*() const {
					assert(is_valid()); //{ MSE_THROW(asyncshared_use_of_invalid_pointer_error("attempt to use invalid pointer - mse::TAccessControlledPointerBase")); }
					return *m_obj_ptr;
				}
				auto operator->() const {
					assert(is_valid()); //{ MSE_THROW(asyncshared_use_of_invalid_pointer_error("attempt to use invalid pointer - mse::TAccessControlledPointerBase")); }
					return m_obj_ptr;
				}

			private:
				typedef recursive_shared_mutex_wrapped<_TAccessMutex> _TWrappedAccessMutex;
				TAccessControlledPointerBase(_Ty& obj_ref, _TWrappedAccessMutex& mutex_ref) : m_obj_ptr(std::addressof(obj_ref)), m_mutex_ptr(&mutex_ref), m_write_lock(mutex_ref) {}
				TAccessControlledPointerBase(_Ty& obj_ref, _TWrappedAccessMutex& mutex_ref, std::try_to_lock_t) : m_obj_ptr(std::addressof(obj_ref)), m_mutex_ptr(&mutex_ref), m_write_lock(mutex_ref, std::defer_lock) {
					if (!m_write_lock.try_lock()) {
						m_obj_ptr = nullptr;
					}
				}
				template<class _Rep, class _Period>
				TAccessControlledPointerBase(_Ty& obj_ref, _TWrappedAccessMutex& mutex_ref, std::try_to_lock_t, const std::chrono::duration<_Rep, _Period>& _Rel_time) : m_obj_ptr(std::addressof(obj_ref)), m_mutex_ptr(&mutex_ref), m_write_lock(mutex_ref, std::defer_lock) {
					if (!m_write_lock.try_lock_for(_Rel_time)) {
						m_obj_ptr = nullptr;
					}
				}
				template<class _Clock, class _Duration>
				TAccessControlledPointerBase(_Ty& obj_ref, _TWrappedAccessMutex& mutex_ref, std::try_to_lock_t, const std::chrono::time_point<_Clock, _Duration>& _Abs_time) : m_obj_ptr(std::addressof(obj_ref)), m_mutex_ptr(&mutex_ref), m_write_lock(mutex_ref, std::defer_lock) {
					if (!m_write_lock.try_lock_until(_Abs_time)) {
						m_obj_ptr = nullptr;
					}
				}

				template<class _TAccessMutex2 = _TAccessMutex, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_TAccessMutex2, _TAccessMutex>::value)
					&& (mse::impl::is_supported_aco_mutex_msemsearray<_TAccessMutex2>::value)> MSE_IMPL_EIS >
					void valid_if_TAccessMutex_is_supported() const {}

				MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

				bool is_valid() const {
					bool retval = (nullptr != m_obj_ptr);
					return retval;
				}

				_Ty* m_obj_ptr = nullptr;
				_TWrappedAccessMutex* m_mutex_ptr = nullptr;
				/* Note, the default mutex used here, non_thread_safe_recursive_shared_timed_mutex, like std::recursive_mutex, supports
				being locked multiple times simultaneously, even when using std::unique_lock<>. */
				std::unique_lock<_TWrappedAccessMutex> m_write_lock;

				friend class TAccessControlledObjBase<_Ty, _TAccessMutex>;
				friend class TXScopeAccessControlledPointer<_Ty, _TAccessMutex>;
				friend class TAccessControlledPointer<_Ty, _TAccessMutex>;
				friend class TAccessControlledConstPointerBase<_Ty, _TAccessMutex>;
			};
		}
	}

	template<typename _Ty, class _TAccessMutex/* = non_thread_safe_recursive_shared_timed_mutex*/>
	class TXScopeAccessControlledPointer : public mse::us::impl::TAccessControlledPointerBase<_Ty, _TAccessMutex>, public mse::us::impl::XScopeTagBase {
	public:
		typedef mse::us::impl::TAccessControlledPointerBase<_Ty, _TAccessMutex> base_class;

		MSE_USING(TXScopeAccessControlledPointer, base_class);
		TXScopeAccessControlledPointer(const TXScopeAccessControlledPointer& src) = default;
		TXScopeAccessControlledPointer(TXScopeAccessControlledPointer&& src) = default;
		template<typename _Ty2>
		TXScopeAccessControlledPointer(const TXScopeAccessControlledPointer<_Ty2, _TAccessMutex>& src) : base_class(src) {}

		template<class _TAccessMutex2 = _TAccessMutex, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_TAccessMutex2, _TAccessMutex>::value)
			&& (mse::impl::is_thread_safety_enforcing_mutex_msemsearray<_TAccessMutex2>::value)> MSE_IMPL_EIS >
			void xscope_async_passable_tag() const {} /* Indication that this type is eligible to be passed between threads. */

	private:
		typedef recursive_shared_mutex_wrapped<_TAccessMutex> _TWrappedAccessMutex;
		TXScopeAccessControlledPointer(_Ty& obj_ref, _TWrappedAccessMutex& mutex_ref) : base_class(obj_ref, mutex_ref) {}
		TXScopeAccessControlledPointer(_Ty& obj_ref, _TWrappedAccessMutex& mutex_ref, const std::try_to_lock_t& ttl) : base_class(obj_ref, mutex_ref, ttl) {}
		template<class _Rep, class _Period>
		TXScopeAccessControlledPointer(_Ty& obj_ref, _TWrappedAccessMutex& mutex_ref, const std::try_to_lock_t& ttl, const std::chrono::duration<_Rep, _Period>& _Rel_time)
			: base_class(obj_ref, mutex_ref, ttl, _Rel_time) {}
		template<class _Clock, class _Duration>
		TXScopeAccessControlledPointer(_Ty& obj_ref, _TWrappedAccessMutex& mutex_ref, const std::try_to_lock_t& ttl, const std::chrono::time_point<_Clock, _Duration>& _Abs_time)
			: base_class(obj_ref, mutex_ref, ttl, _Abs_time) {}
		//TXScopeAccessControlledPointer(base_class&& src) : base_class(MSE_FWD(src)) {}

		TXScopeAccessControlledPointer & operator=(const TXScopeAccessControlledPointer& _Right_cref) = delete;
		TXScopeAccessControlledPointer& operator=(TXScopeAccessControlledPointer&& _Right) = delete;

		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		friend class mse::us::impl::TAccessControlledObjBase<_Ty, _TAccessMutex>;
		friend class TXScopeAccessControlledConstPointer<_Ty, _TAccessMutex>;
		template<typename _Ty2, class _TAccessMutex2/* = non_thread_safe_recursive_shared_timed_mutex*/>
		friend TXScopeAccessControlledPointer<_Ty2, _TAccessMutex2> xscope_pointer_to_access_controlled_obj(const TXScopeFixedPointer<TAccessControlledObj<_Ty2, _TAccessMutex2> >& aco_xscpptr);
#ifndef MSE_SCOPEPOINTER_DISABLED
		template<typename _Ty2, class _TAccessMutex2/* = non_thread_safe_recursive_shared_timed_mutex*/>
		friend TXScopeAccessControlledPointer<_Ty2, _TAccessMutex2> xscope_pointer_to_access_controlled_obj(const TXScopeObjFixedPointer<TAccessControlledObj<_Ty2, _TAccessMutex2> >& aco_xscpptr);
#endif //!MSE_SCOPEPOINTER_DISABLED
		template<typename _TExclusiveStrongPointer2, class _TAccessMutex2/* = non_thread_safe_recursive_shared_timed_mutex*/>
		friend class TXScopeExclusiveStrongPointerStoreForAccessControl;
	};

	template<typename _Ty, class _TAccessMutex/* = non_thread_safe_recursive_shared_timed_mutex*/>
	class TAccessControlledPointer : public mse::us::impl::TAccessControlledPointerBase<_Ty, _TAccessMutex> {
	public:
		typedef mse::us::impl::TAccessControlledPointerBase<_Ty, _TAccessMutex> base_class;
		TAccessControlledPointer(const TAccessControlledPointer& src) = default;
		TAccessControlledPointer(TAccessControlledPointer&& src) = default; /* Note, the move constructor is only safe when std::move() is prohibited. */
		template<typename _Ty2>
		TAccessControlledPointer(const TAccessControlledPointer<_Ty2, _TAccessMutex>& src) : base_class(src) {}

		/* This element is safely "async passable" if the _TAccessMutex is a suitable thread safe mutex. */
		template<class _TAccessMutex2 = _TAccessMutex, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_TAccessMutex2, _TAccessMutex>::value)
			&& (mse::impl::is_thread_safety_enforcing_mutex_msemsearray<_TAccessMutex2>::value)> MSE_IMPL_EIS >
			void async_passable_tag() const {} /* Indication that this type is eligible to be passed between threads. */

	private:
		typedef recursive_shared_mutex_wrapped<_TAccessMutex> _TWrappedAccessMutex;
		TAccessControlledPointer(_Ty& obj_ref, _TWrappedAccessMutex& mutex_ref) : base_class(obj_ref, mutex_ref) {}
		TAccessControlledPointer(_Ty& obj_ref, _TWrappedAccessMutex& mutex_ref, const std::try_to_lock_t& ttl) : base_class(obj_ref, mutex_ref, ttl) {}
		template<class _Rep, class _Period>
		TAccessControlledPointer(_Ty& obj_ref, _TWrappedAccessMutex& mutex_ref, const std::try_to_lock_t& ttl, const std::chrono::duration<_Rep, _Period>& _Rel_time)
			: base_class(obj_ref, mutex_ref, ttl, _Rel_time) {}
		template<class _Clock, class _Duration>
		TAccessControlledPointer(_Ty& obj_ref, _TWrappedAccessMutex& mutex_ref, const std::try_to_lock_t& ttl, const std::chrono::time_point<_Clock, _Duration>& _Abs_time)
			: base_class(obj_ref, mutex_ref, ttl, _Abs_time) {}

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		friend class mse::us::impl::TAccessControlledObjBase<_Ty, _TAccessMutex>;
		friend class TAccessControlledConstPointer<_Ty, _TAccessMutex>;
	};

	namespace us {
		namespace impl {
			template<typename _Ty, class _TAccessMutex/* = non_thread_safe_recursive_shared_timed_mutex*/>
			class TAccessControlledConstPointerBase : public mse::us::impl::StrongPointerTagBase {
			public:
				TAccessControlledConstPointerBase(const TAccessControlledConstPointerBase& src) : m_obj_ptr(src.m_obj_ptr), m_mutex_ptr(src.m_mutex_ptr), m_read_lock(*(src.m_mutex_ptr)) {}
				TAccessControlledConstPointerBase(TAccessControlledConstPointerBase&& src) = default; /* Note, the move constructor is only safe when std::move() is prohibited. */
				template<typename _Ty2>
				TAccessControlledConstPointerBase(const TAccessControlledConstPointerBase<_Ty2, _TAccessMutex>& src) : m_obj_ptr(src.m_obj_ptr), m_mutex_ptr(src.m_mutex_ptr), m_read_lock(*(src.m_mutex_ptr)) {}

				MSE_IMPL_DESTRUCTOR_PREFIX1 ~TAccessControlledConstPointerBase() {
					valid_if_TAccessMutex_is_supported<_TAccessMutex>();
				}

				operator bool() const {
					//assert(is_valid()); //{ MSE_THROW(asyncshared_use_of_invalid_pointer_error("attempt to use invalid pointer - mse::TAccessControlledConstPointerBase")); }
					return (nullptr != m_obj_ptr);
				}
				const _Ty& operator*() const {
					assert(is_valid()); //{ MSE_THROW(asyncshared_use_of_invalid_pointer_error("attempt to use invalid pointer - mse::TAccessControlledConstPointerBase")); }
					return *m_obj_ptr;
				}
				const _Ty* operator->() const {
					assert(is_valid()); //{ MSE_THROW(asyncshared_use_of_invalid_pointer_error("attempt to use invalid pointer - mse::TAccessControlledConstPointerBase")); }
					return m_obj_ptr;
				}

			private:
				typedef recursive_shared_mutex_wrapped<_TAccessMutex> _TWrappedAccessMutex;
				TAccessControlledConstPointerBase(const _Ty& obj_ref, _TWrappedAccessMutex& mutex_ref) : m_obj_ptr(std::addressof(obj_ref)), m_mutex_ptr(&mutex_ref), m_read_lock(mutex_ref) {}
				TAccessControlledConstPointerBase(const _Ty& obj_ref, _TWrappedAccessMutex& mutex_ref, std::try_to_lock_t) : m_obj_ptr(std::addressof(obj_ref)), m_mutex_ptr(&mutex_ref), m_read_lock(mutex_ref, std::defer_lock) {
					if (!m_read_lock.try_lock()) {
						m_obj_ptr = nullptr;
					}
				}
				template<class _Rep, class _Period>
				TAccessControlledConstPointerBase(const _Ty& obj_ref, _TWrappedAccessMutex& mutex_ref, std::try_to_lock_t, const std::chrono::duration<_Rep, _Period>& _Rel_time) : m_obj_ptr(std::addressof(obj_ref)), m_mutex_ptr(&mutex_ref), m_read_lock(mutex_ref, std::defer_lock) {
					if (!m_read_lock.try_lock_for(_Rel_time)) {
						m_obj_ptr = nullptr;
					}
				}
				template<class _Clock, class _Duration>
				TAccessControlledConstPointerBase(const _Ty& obj_ref, _TWrappedAccessMutex& mutex_ref, std::try_to_lock_t, const std::chrono::time_point<_Clock, _Duration>& _Abs_time) : m_obj_ptr(std::addressof(obj_ref)), m_mutex_ptr(&mutex_ref), m_read_lock(mutex_ref, std::defer_lock) {
					if (!m_read_lock.try_lock_until(_Abs_time)) {
						m_obj_ptr = nullptr;
					}
				}

				template<class _TAccessMutex2 = _TAccessMutex, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_TAccessMutex2, _TAccessMutex>::value)
					&& (mse::impl::is_supported_aco_mutex_msemsearray<_TAccessMutex2>::value)> MSE_IMPL_EIS >
					void valid_if_TAccessMutex_is_supported() const {}

				MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

				bool is_valid() const {
					bool retval = (nullptr != m_obj_ptr);
					return retval;
				}

				const _Ty* m_obj_ptr = nullptr;
				_TWrappedAccessMutex* m_mutex_ptr = nullptr;
				std::shared_lock<_TWrappedAccessMutex> m_read_lock;

				friend class TXScopeAccessControlledConstPointer<_Ty, _TAccessMutex>;
				friend class TAccessControlledConstPointer<_Ty, _TAccessMutex>;
				template<typename _Ty2, class _TAccessMutex2> friend class TAccessControlledObjBase;
				template<typename _Ty2, class _TAccessMutex2> friend class TAccessControlledConstPointerBase;
			};
		}
	}

	template<typename _Ty, class _TAccessMutex/* = non_thread_safe_recursive_shared_timed_mutex*/>
	class TXScopeAccessControlledConstPointer : public mse::us::impl::TAccessControlledConstPointerBase<_Ty, _TAccessMutex>, public mse::us::impl::XScopeTagBase {
	public:
		typedef mse::us::impl::TAccessControlledConstPointerBase<_Ty, _TAccessMutex> base_class;

		MSE_USING(TXScopeAccessControlledConstPointer, base_class);
		TXScopeAccessControlledConstPointer(const TXScopeAccessControlledConstPointer& src) = default;
		TXScopeAccessControlledConstPointer(TXScopeAccessControlledConstPointer&& src) = default;
		template<typename _Ty2>
		TXScopeAccessControlledConstPointer(const TXScopeAccessControlledConstPointer<_Ty2, _TAccessMutex>& src) : base_class(src) {}

		template<class _TAccessMutex2 = _TAccessMutex, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_TAccessMutex2, _TAccessMutex>::value)
			&& (mse::impl::is_thread_safety_enforcing_mutex_msemsearray<_TAccessMutex2>::value)> MSE_IMPL_EIS >
			void xscope_async_passable_tag() const {} /* Indication that this type is eligible to be passed between threads. */

	private:
		typedef recursive_shared_mutex_wrapped<_TAccessMutex> _TWrappedAccessMutex;
		TXScopeAccessControlledConstPointer(const _Ty& obj_ref, _TWrappedAccessMutex& mutex_ref) : base_class(obj_ref, mutex_ref) {}
		TXScopeAccessControlledConstPointer(const _Ty& obj_ref, _TWrappedAccessMutex& mutex_ref, const std::try_to_lock_t& ttl) : base_class(obj_ref, mutex_ref, ttl) {}
		template<class _Rep, class _Period>
		TXScopeAccessControlledConstPointer(const _Ty& obj_ref, _TWrappedAccessMutex& mutex_ref, const std::try_to_lock_t& ttl, const std::chrono::duration<_Rep, _Period>& _Rel_time)
			: base_class(obj_ref, mutex_ref, ttl, _Rel_time) {}
		template<class _Clock, class _Duration>
		TXScopeAccessControlledConstPointer(const _Ty& obj_ref, _TWrappedAccessMutex& mutex_ref, const std::try_to_lock_t& ttl, const std::chrono::time_point<_Clock, _Duration>& _Abs_time)
			: base_class(obj_ref, mutex_ref, ttl, _Abs_time) {}
		//TXScopeAccessControlledConstPointer(base_class&& src) : base_class(MSE_FWD(src)) {}

		TXScopeAccessControlledConstPointer & operator=(const TXScopeAccessControlledConstPointer& _Right_cref) = delete;
		TXScopeAccessControlledConstPointer& operator=(TXScopeAccessControlledConstPointer&& _Right) = delete;

		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		friend class mse::us::impl::TAccessControlledObjBase<_Ty, _TAccessMutex>;
		template<typename _Ty2, class _TAccessMutex2/* = non_thread_safe_recursive_shared_timed_mutex*/>
		friend TXScopeAccessControlledConstPointer<_Ty2, _TAccessMutex2> xscope_pointer_to_access_controlled_obj(const TXScopeFixedPointer<TAccessControlledObj<_Ty2, _TAccessMutex2> >& aco_xscpptr);
#ifndef MSE_SCOPEPOINTER_DISABLED
		template<typename _Ty2, class _TAccessMutex2/* = non_thread_safe_recursive_shared_timed_mutex*/>
		friend TXScopeAccessControlledConstPointer<_Ty2, _TAccessMutex2> xscope_pointer_to_access_controlled_obj(const TXScopeObjFixedPointer<TAccessControlledObj<_Ty2, _TAccessMutex2> >& aco_xscpptr);
#endif //!MSE_SCOPEPOINTER_DISABLED
		template<typename _TExclusiveStrongPointer2, class _TAccessMutex2/* = non_thread_safe_recursive_shared_timed_mutex*/>
		friend class TXScopeExclusiveStrongPointerStoreForAccessControl;
	};

	template<typename _Ty, class _TAccessMutex/* = non_thread_safe_recursive_shared_timed_mutex*/>
	class TAccessControlledConstPointer : public mse::us::impl::TAccessControlledConstPointerBase<_Ty, _TAccessMutex> {
	public:
		typedef mse::us::impl::TAccessControlledConstPointerBase<_Ty, _TAccessMutex> base_class;
		TAccessControlledConstPointer(const TAccessControlledConstPointer& src) = default;
		TAccessControlledConstPointer(TAccessControlledConstPointer&& src) = default; /* Note, the move constructor is only safe when std::move() is prohibited. */
		template<typename _Ty2>
		TAccessControlledConstPointer(const TAccessControlledConstPointer<_Ty2, _TAccessMutex>& src) : base_class(src) {}

		/* This element is safely "async passable" if the _TAccessMutex is a suitable thread safe mutex. */
		template<class _TAccessMutex2 = _TAccessMutex, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_TAccessMutex2, _TAccessMutex>::value)
			&& (mse::impl::is_thread_safety_enforcing_mutex_msemsearray<_TAccessMutex2>::value)> MSE_IMPL_EIS >
		void async_passable_tag() const {} /* Indication that this type is eligible to be passed between threads. */

	private:
		typedef recursive_shared_mutex_wrapped<_TAccessMutex> _TWrappedAccessMutex;
		TAccessControlledConstPointer(const _Ty& obj_ref, _TWrappedAccessMutex& mutex_ref) : base_class(obj_ref, mutex_ref) {}
		TAccessControlledConstPointer(const _Ty& obj_ref, _TWrappedAccessMutex& mutex_ref, const std::try_to_lock_t& ttl) : base_class(obj_ref, mutex_ref, ttl) {}
		template<class _Rep, class _Period>
		TAccessControlledConstPointer(const _Ty& obj_ref, _TWrappedAccessMutex& mutex_ref, const std::try_to_lock_t& ttl, const std::chrono::duration<_Rep, _Period>& _Rel_time)
			: base_class(obj_ref, mutex_ref, ttl, _Rel_time) {}
		template<class _Clock, class _Duration>
		TAccessControlledConstPointer(const _Ty& obj_ref, _TWrappedAccessMutex& mutex_ref, const std::try_to_lock_t& ttl, const std::chrono::time_point<_Clock, _Duration>& _Abs_time)
			: base_class(obj_ref, mutex_ref, ttl, _Abs_time) {}

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		friend class mse::us::impl::TAccessControlledObjBase<_Ty, _TAccessMutex>;
	};

	namespace us {
		namespace impl {
			template<typename _Ty, class _TAccessMutex/* = non_thread_safe_recursive_shared_timed_mutex*/>
			class TAccessControlledExclusivePointerBase : public mse::us::impl::StrongExclusivePointerTagBase {
			public:
				TAccessControlledExclusivePointerBase(const TAccessControlledExclusivePointerBase& src) = delete;
				TAccessControlledExclusivePointerBase(TAccessControlledExclusivePointerBase&& src) = default; /* Note, the move constructor is only safe when std::move() is prohibited. */
				template<typename _Ty2>
				TAccessControlledExclusivePointerBase(TAccessControlledExclusivePointerBase<_Ty2, _TAccessMutex>&& src) : m_obj_ptr(src.m_obj_ptr), m_exclusive_write_lock(MSE_FWD(src).m_exclusive_write_lock) {}

				MSE_IMPL_DESTRUCTOR_PREFIX1 ~TAccessControlledExclusivePointerBase() {
					valid_if_TAccessMutex_is_supported<_TAccessMutex>();
				}

				operator bool() const {
					//assert(is_valid()); //{ MSE_THROW(asyncshared_use_of_invalid_pointer_error("attempt to use invalid pointer - mse::TAccessControlledExclusivePointerBase")); }
					return (nullptr != m_obj_ptr);
				}
				auto& operator*() const {
					assert(is_valid()); //{ MSE_THROW(asyncshared_use_of_invalid_pointer_error("attempt to use invalid pointer - mse::TAccessControlledExclusivePointerBase")); }
					return *m_obj_ptr;
				}
				auto operator->() const {
					assert(is_valid()); //{ MSE_THROW(asyncshared_use_of_invalid_pointer_error("attempt to use invalid pointer - mse::TAccessControlledExclusivePointerBase")); }
					return m_obj_ptr;
				}

			private:
				typedef recursive_shared_mutex_wrapped<_TAccessMutex> _TWrappedAccessMutex;
				TAccessControlledExclusivePointerBase(_Ty& obj_ref, _TWrappedAccessMutex& mutex_ref) : m_obj_ptr(std::addressof(obj_ref)), m_exclusive_write_lock(mutex_ref) {}
				TAccessControlledExclusivePointerBase(_Ty& obj_ref, _TWrappedAccessMutex& mutex_ref, std::try_to_lock_t) : m_obj_ptr(std::addressof(obj_ref)), m_exclusive_write_lock(mutex_ref, std::defer_lock) {
					if (!m_exclusive_write_lock.try_lock()) {
						m_obj_ptr = nullptr;
					}
				}
				template<class _Rep, class _Period>
				TAccessControlledExclusivePointerBase(_Ty& obj_ref, _TWrappedAccessMutex& mutex_ref, std::try_to_lock_t, const std::chrono::duration<_Rep, _Period>& _Rel_time) : m_obj_ptr(std::addressof(obj_ref)), m_exclusive_write_lock(mutex_ref, std::defer_lock) {
					if (!m_exclusive_write_lock.try_lock_for(_Rel_time)) {
						m_obj_ptr = nullptr;
					}
				}
				template<class _Clock, class _Duration>
				TAccessControlledExclusivePointerBase(_Ty& obj_ref, _TWrappedAccessMutex& mutex_ref, std::try_to_lock_t, const std::chrono::time_point<_Clock, _Duration>& _Abs_time) : m_obj_ptr(std::addressof(obj_ref)), m_exclusive_write_lock(mutex_ref, std::defer_lock) {
					if (!m_exclusive_write_lock.try_lock_until(_Abs_time)) {
						m_obj_ptr = nullptr;
					}
				}

				template<class _TAccessMutex2 = _TAccessMutex, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_TAccessMutex2, _TAccessMutex>::value)
					&& (mse::impl::is_supported_aco_mutex_msemsearray<_TAccessMutex2>::value)> MSE_IMPL_EIS >
					void valid_if_TAccessMutex_is_supported() const {}

				MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

				bool is_valid() const {
					bool retval = (nullptr != m_obj_ptr);
					return retval;
				}

				_Ty* m_obj_ptr = nullptr;
				unique_nonrecursive_lock<_TWrappedAccessMutex> m_exclusive_write_lock;

				friend class TAccessControlledObjBase<_Ty, _TAccessMutex>;
				friend class TXScopeAccessControlledExclusivePointer<_Ty, _TAccessMutex>;
				friend class TAccessControlledExclusivePointer<_Ty, _TAccessMutex>;
			};
		}
	}

	template<typename _Ty, class _TAccessMutex/* = non_thread_safe_recursive_shared_timed_mutex*/>
	class TXScopeAccessControlledExclusivePointer : public mse::us::impl::TAccessControlledExclusivePointerBase<_Ty, _TAccessMutex>, public mse::us::impl::XScopeTagBase {
	public:
		typedef mse::us::impl::TAccessControlledExclusivePointerBase<_Ty, _TAccessMutex> base_class;

		MSE_USING(TXScopeAccessControlledExclusivePointer, base_class);
		TXScopeAccessControlledExclusivePointer(const TXScopeAccessControlledExclusivePointer& src) = delete;
		TXScopeAccessControlledExclusivePointer(TXScopeAccessControlledExclusivePointer&& src) = default;
		template<typename _Ty2>
		TXScopeAccessControlledExclusivePointer(TXScopeAccessControlledExclusivePointer<_Ty2, _TAccessMutex>&& src) : base_class(MSE_FWD(src)) {}

		template<class _TAccessMutex2 = _TAccessMutex, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_TAccessMutex2, _TAccessMutex>::value)
			&& (mse::impl::is_thread_safety_enforcing_mutex_msemsearray<_TAccessMutex2>::value)> MSE_IMPL_EIS >
			void xscope_async_passable_tag() const {} /* Indication that this type is eligible to be passed between threads. */

	private:
		typedef recursive_shared_mutex_wrapped<_TAccessMutex> _TWrappedAccessMutex;
		TXScopeAccessControlledExclusivePointer(_Ty& obj_ref, _TWrappedAccessMutex& mutex_ref) : base_class(obj_ref, mutex_ref) {}
		TXScopeAccessControlledExclusivePointer(_Ty& obj_ref, _TWrappedAccessMutex& mutex_ref, const std::try_to_lock_t& ttl) : base_class(obj_ref, mutex_ref, ttl) {}
		template<class _Rep, class _Period>
		TXScopeAccessControlledExclusivePointer(_Ty& obj_ref, _TWrappedAccessMutex& mutex_ref, const std::try_to_lock_t& ttl, const std::chrono::duration<_Rep, _Period>& _Rel_time)
			: base_class(obj_ref, mutex_ref, ttl, _Rel_time) {}
		template<class _Clock, class _Duration>
		TXScopeAccessControlledExclusivePointer(_Ty& obj_ref, _TWrappedAccessMutex& mutex_ref, const std::try_to_lock_t& ttl, const std::chrono::time_point<_Clock, _Duration>& _Abs_time)
			: base_class(obj_ref, mutex_ref, ttl, _Abs_time) {}
		//TXScopeAccessControlledExclusivePointer(base_class&& src) : base_class(MSE_FWD(src)) {}

		TXScopeAccessControlledExclusivePointer & operator=(const TXScopeAccessControlledExclusivePointer& _Right_cref) = delete;
		TXScopeAccessControlledExclusivePointer& operator=(TXScopeAccessControlledExclusivePointer&& _Right) = delete;

		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		friend class mse::us::impl::TAccessControlledObjBase<_Ty, _TAccessMutex>;
		template<typename _Ty2, class _TAccessMutex2/* = non_thread_safe_recursive_shared_timed_mutex*/>
		friend TXScopeAccessControlledExclusivePointer<_Ty2, _TAccessMutex2> xscope_pointer_to_access_controlled_obj(const TXScopeFixedPointer<TAccessControlledObj<_Ty2, _TAccessMutex2> >& aco_xscpptr);
#ifndef MSE_SCOPEPOINTER_DISABLED
		template<typename _Ty2, class _TAccessMutex2/* = non_thread_safe_recursive_shared_timed_mutex*/>
		friend TXScopeAccessControlledExclusivePointer<_Ty2, _TAccessMutex2> xscope_pointer_to_access_controlled_obj(const TXScopeObjFixedPointer<TAccessControlledObj<_Ty2, _TAccessMutex2> >& aco_xscpptr);
#endif //!MSE_SCOPEPOINTER_DISABLED
		template<typename _TExclusiveStrongPointer2, class _TAccessMutex2/* = non_thread_safe_recursive_shared_timed_mutex*/>
		friend class TXScopeExclusiveStrongPointerStoreForAccessControl;
	};

	template<typename _Ty, class _TAccessMutex/* = non_thread_safe_recursive_shared_timed_mutex*/>
	class TAccessControlledExclusivePointer : public mse::us::impl::TAccessControlledExclusivePointerBase<_Ty, _TAccessMutex> {
	public:
		typedef mse::us::impl::TAccessControlledExclusivePointerBase<_Ty, _TAccessMutex> base_class;
		TAccessControlledExclusivePointer(const TAccessControlledExclusivePointer& src) = delete;
		TAccessControlledExclusivePointer(TAccessControlledExclusivePointer&& src) = default; /* Note, the move constructor is only safe when std::move() is prohibited. */
		template<typename _Ty2>
		TAccessControlledExclusivePointer(TAccessControlledExclusivePointer<_Ty2, _TAccessMutex>&& src) : base_class(MSE_FWD(src)) {}

		/* This element is safely "async passable" if the _TAccessMutex is a suitable thread safe mutex. */
		template<class _TAccessMutex2 = _TAccessMutex, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_TAccessMutex2, _TAccessMutex>::value)
			&& (mse::impl::is_thread_safety_enforcing_mutex_msemsearray<_TAccessMutex2>::value)> MSE_IMPL_EIS >
			void async_passable_tag() const {} /* Indication that this type is eligible to be passed between threads. */

	private:
		typedef recursive_shared_mutex_wrapped<_TAccessMutex> _TWrappedAccessMutex;
		TAccessControlledExclusivePointer(_Ty& obj_ref, _TWrappedAccessMutex& mutex_ref) : base_class(obj_ref, mutex_ref) {}
		TAccessControlledExclusivePointer(_Ty& obj_ref, _TWrappedAccessMutex& mutex_ref, const std::try_to_lock_t& ttl) : base_class(obj_ref, mutex_ref, ttl) {}
		template<class _Rep, class _Period>
		TAccessControlledExclusivePointer(_Ty& obj_ref, _TWrappedAccessMutex& mutex_ref, const std::try_to_lock_t& ttl, const std::chrono::duration<_Rep, _Period>& _Rel_time)
			: base_class(obj_ref, mutex_ref, ttl, _Rel_time) {}
		template<class _Clock, class _Duration>
		TAccessControlledExclusivePointer(_Ty& obj_ref, _TWrappedAccessMutex& mutex_ref, const std::try_to_lock_t& ttl, const std::chrono::time_point<_Clock, _Duration>& _Abs_time)
			: base_class(obj_ref, mutex_ref, ttl, _Abs_time) {}

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		friend class mse::us::impl::TAccessControlledObjBase<_Ty, _TAccessMutex>;
	};

	namespace us {
		namespace impl {
			namespace ns_aco {
				/* The purpose of the CReadLockedSrcRefHolder class is to obtain (and hold) read
				access to the source object. Used in the copy constructor. */
				template<class _Ty, class _TWrappedAccessMutex = decltype(std::declval<_Ty>().m_mutex1)>
				class CReadLockedSrcRefHolder : private std::shared_lock<_TWrappedAccessMutex> {
				public:
					CReadLockedSrcRefHolder(const _Ty& src) : std::shared_lock<_TWrappedAccessMutex>(src.m_mutex1), m_ptr(&src) {}
					const _Ty& ref() const { return *m_ptr; }
				private:
					const _Ty* m_ptr = nullptr;
				};
				/* The purpose of the CWriteLockedSrc class is to obtain (and hold) write
				access to a source object about to be moved from. Used in the move constructor. */
				template<class _Ty, class _TWrappedAccessMutex = decltype(std::declval<_Ty>().m_mutex1)>
				class CWriteLockedSrc : private std::unique_lock<_TWrappedAccessMutex> {
				public:
					CWriteLockedSrc(_Ty&& src) : std::unique_lock<_TWrappedAccessMutex>(src.m_mutex1), m_ref(MSE_FWD(src)) {}
					_Ty&& ref() const { return MSE_FWD(m_ref); }
				private:
					_Ty&& m_ref;
				};

				/* The ACOGuardedWrapper<>s just hold the object and the access mutex, and ensure that proper access is obtained
				(and maintained) while executing copy and/or move constructors. */
				template<class _Ty, class _TAccessMutex>
				class TACOGuardedWrapper {
				public:
					typedef recursive_shared_mutex_wrapped<_TAccessMutex> _TWrappedAccessMutex;
					TACOGuardedWrapper(_Ty&& obj) : m_obj(MSE_FWD(obj)) {}
					TACOGuardedWrapper(TACOGuardedWrapper&& src) : m_obj(CWriteLockedSrc<TACOGuardedWrapper>(MSE_FWD(src)).ref().m_obj) {}
					TACOGuardedWrapper(const TACOGuardedWrapper& src) : m_obj(CReadLockedSrcRefHolder<TACOGuardedWrapper>(src).ref().m_obj) {}

					_Ty m_obj;
					mutable _TWrappedAccessMutex m_mutex1;
				};
				template<class _Ty, class _TAccessMutex>
				class TUnMovableACOGuardedWrapper {
				public:
					typedef recursive_shared_mutex_wrapped<_TAccessMutex> _TWrappedAccessMutex;
					TUnMovableACOGuardedWrapper(_Ty&& obj) : m_obj(MSE_FWD(obj)) {}
					TUnMovableACOGuardedWrapper(const TUnMovableACOGuardedWrapper& src) : m_obj(CReadLockedSrcRefHolder<TUnMovableACOGuardedWrapper>(src).ref().m_obj) {}

					_Ty m_obj;
					mutable _TWrappedAccessMutex m_mutex1;
				};
				template<class _Ty, class _TAccessMutex>
				class TUnCopyableACOGuardedWrapper {
				public:
					typedef recursive_shared_mutex_wrapped<_TAccessMutex> _TWrappedAccessMutex;
					TUnCopyableACOGuardedWrapper(_Ty&& obj) : m_obj(MSE_FWD(obj)) {}
					TUnCopyableACOGuardedWrapper(TUnCopyableACOGuardedWrapper&& src) : m_obj(CWriteLockedSrc<TUnCopyableACOGuardedWrapper>(MSE_FWD(src)).ref().m_obj) {}

					_Ty m_obj;
					mutable _TWrappedAccessMutex m_mutex1;
				};
				template<class _Ty, class _TAccessMutex>
				class TUnCopyableAndUnMovableACOGuardedWrapper {
				public:
					typedef recursive_shared_mutex_wrapped<_TAccessMutex> _TWrappedAccessMutex;
					TUnCopyableAndUnMovableACOGuardedWrapper(_Ty&& obj) : m_obj(MSE_FWD(obj)) {}

					_Ty m_obj;
					mutable _TWrappedAccessMutex m_mutex1;
				};
				template<class _Ty, class _TAccessMutex>
				using unmovable_guarded_wrapper_t = mse::impl::conditional_t<std::is_copy_constructible<_Ty>::value, TUnMovableACOGuardedWrapper<_Ty, _TAccessMutex>, TUnCopyableAndUnMovableACOGuardedWrapper<_Ty, _TAccessMutex> >;
				template<class _Ty, class _TAccessMutex>
				using movable_guarded_wrapper_t = mse::impl::conditional_t<std::is_copy_constructible<_Ty>::value, TACOGuardedWrapper<_Ty, _TAccessMutex>, TUnCopyableACOGuardedWrapper<_Ty, _TAccessMutex> >;
				template<class _Ty, class _TAccessMutex>
				using guarded_wrapper_t = mse::impl::conditional_t<std::is_move_constructible<_Ty>::value, movable_guarded_wrapper_t<_Ty, _TAccessMutex>, unmovable_guarded_wrapper_t<_Ty, _TAccessMutex> >;
			}

			template<class _Ty, class _TAccessMutex/* = non_thread_safe_recursive_shared_timed_mutex*/>
			class TAccessControlledObjBase {
			public:
				typedef _Ty object_type;
				typedef _TAccessMutex access_mutex_type;

				TAccessControlledObjBase(TAccessControlledObjBase&& src) = default;
				TAccessControlledObjBase(const TAccessControlledObjBase& src) = default;

				template <class... Args>
				TAccessControlledObjBase(Args&&... args) : m_guarded_obj(constructor_helper1(std::forward<Args>(args)...)) {}

				MSE_IMPL_DESTRUCTOR_PREFIX1 ~TAccessControlledObjBase() {
					MSE_TRY{
						m_guarded_obj.m_mutex1.nonrecursive_lock();
						m_guarded_obj.m_mutex1.nonrecursive_unlock();
					}
						MSE_CATCH_ANY{
						/* It would be unsafe to allow this object to be destroyed as there are outstanding references to this object (in
						this thread). */
#ifdef MSE_CUSTOM_FATAL_ERROR_MESSAGE_HANDLER
						MSE_CUSTOM_FATAL_ERROR_MESSAGE_HANDLER("Fatal Error: mse::us::impl::TAccessControlledObjBase<> destructed with outstanding references in the same thread \n");
#endif // MSE_CUSTOM_FATAL_ERROR_MESSAGE_HANDLER
						assert(false); std::terminate();
					}

						/* This is just a no-op function that will cause a compile error when _Ty is not an eligible type. */
					mse::impl::T_valid_if_is_marked_as_xscope_shareable_msemsearray<_Ty>();

					/* todo: ensure that _TAccessMutex is a supported mutex type */
				}

				TXScopeAccessControlledPointer<_Ty, _TAccessMutex> xscope_pointer() {
					return TXScopeAccessControlledPointer<_Ty, _TAccessMutex>(m_guarded_obj.m_obj, m_guarded_obj.m_mutex1);
				}
				mse::xscope_optional<TXScopeAccessControlledPointer<_Ty, _TAccessMutex>> xscope_try_pointer() {
					mse::xscope_optional<TXScopeAccessControlledPointer<_Ty, _TAccessMutex>> retval(TXScopeAccessControlledPointer<_Ty, _TAccessMutex>(m_guarded_obj.m_obj, m_guarded_obj.m_mutex1, std::try_to_lock));
					if (!((*retval).is_valid())) {
						return{};
					}
					return retval;
				}
				template<class _Rep, class _Period>
				mse::xscope_optional<TXScopeAccessControlledPointer<_Ty, _TAccessMutex>> xscope_try_pointer_for(const std::chrono::duration<_Rep, _Period>& _Rel_time) {
					mse::xscope_optional<TXScopeAccessControlledPointer<_Ty, _TAccessMutex>> retval(TXScopeAccessControlledPointer<_Ty, _TAccessMutex>(m_guarded_obj.m_obj, m_guarded_obj.m_mutex1, std::try_to_lock, _Rel_time));
					if (!((*retval).is_valid())) {
						return{};
					}
					return retval;
				}
				template<class _Clock, class _Duration>
				mse::xscope_optional<TXScopeAccessControlledPointer<_Ty, _TAccessMutex>> xscope_try_pointer_until(const std::chrono::time_point<_Clock, _Duration>& _Abs_time) {
					mse::xscope_optional<TXScopeAccessControlledPointer<_Ty, _TAccessMutex>> retval(TXScopeAccessControlledPointer<_Ty, _TAccessMutex>(m_guarded_obj.m_obj, m_guarded_obj.m_mutex1, std::try_to_lock, _Abs_time));
					if (!((*retval).is_valid())) {
						return{};
					}
					return retval;
				}
				TXScopeAccessControlledConstPointer<_Ty, _TAccessMutex> xscope_const_pointer() const {
					return TXScopeAccessControlledConstPointer<_Ty, _TAccessMutex>(m_guarded_obj.m_obj, m_guarded_obj.m_mutex1);
				}
				mse::xscope_optional<TXScopeAccessControlledConstPointer<_Ty, _TAccessMutex>> xscope_try_const_pointer() const {
					mse::xscope_optional<TXScopeAccessControlledConstPointer<_Ty, _TAccessMutex>> retval(TXScopeAccessControlledConstPointer<_Ty, _TAccessMutex>(m_guarded_obj.m_obj, m_guarded_obj.m_mutex1, std::try_to_lock));
					if (!((*retval).is_valid())) {
						return{};
					}
					return retval;
				}
				template<class _Rep, class _Period>
				mse::xscope_optional<TXScopeAccessControlledConstPointer<_Ty, _TAccessMutex>> xscope_try_const_pointer_for(const std::chrono::duration<_Rep, _Period>& _Rel_time) const {
					mse::xscope_optional<TXScopeAccessControlledConstPointer<_Ty, _TAccessMutex>> retval(TXScopeAccessControlledConstPointer<_Ty, _TAccessMutex>(m_guarded_obj.m_obj, m_guarded_obj.m_mutex1, std::try_to_lock, _Rel_time));
					if (!((*retval).is_valid())) {
						return{};
					}
					return retval;
				}
				template<class _Clock, class _Duration>
				mse::xscope_optional<TXScopeAccessControlledConstPointer<_Ty, _TAccessMutex>> xscope_try_const_pointer_until(const std::chrono::time_point<_Clock, _Duration>& _Abs_time) const {
					mse::xscope_optional<TXScopeAccessControlledConstPointer<_Ty, _TAccessMutex>> retval(TXScopeAccessControlledConstPointer<_Ty, _TAccessMutex>(m_guarded_obj.m_obj, m_guarded_obj.m_mutex1, std::try_to_lock, _Abs_time));
					if (!((*retval).is_valid())) {
						return{};
					}
					return retval;
				}
				/* Note that an exclusive_pointer cannot coexist with any other lock_ptrs (targeting the same object), including ones in
				the same thread. Thus, using exclusive_pointers without sufficient care introduces the potential for exceptions (in a way
				that sticking to (regular) pointers doesn't). */
				TXScopeAccessControlledExclusivePointer<_Ty, _TAccessMutex> xscope_exclusive_pointer() {
					return TXScopeAccessControlledExclusivePointer<_Ty, _TAccessMutex>(m_guarded_obj.m_obj, m_guarded_obj.m_mutex1);
				}
				mse::xscope_optional<TXScopeAccessControlledExclusivePointer<_Ty, _TAccessMutex>> xscope_try_exclusive_pointer() {
					mse::xscope_optional<TXScopeAccessControlledExclusivePointer<_Ty, _TAccessMutex>> retval(TXScopeAccessControlledExclusivePointer<_Ty, _TAccessMutex>(m_guarded_obj.m_obj, m_guarded_obj.m_mutex1, std::try_to_lock));
					if (!((*retval).is_valid())) {
						return{};
					}
					return retval;
				}
				template<class _Rep, class _Period>
				mse::xscope_optional<TXScopeAccessControlledExclusivePointer<_Ty, _TAccessMutex>> xscope_try_exclusive_pointer_for(const std::chrono::duration<_Rep, _Period>& _Rel_time) {
					mse::xscope_optional<TXScopeAccessControlledExclusivePointer<_Ty, _TAccessMutex>> retval(TXScopeAccessControlledExclusivePointer<_Ty, _TAccessMutex>(m_guarded_obj.m_obj, m_guarded_obj.m_mutex1, std::try_to_lock, _Rel_time));
					if (!((*retval).is_valid())) {
						return{};
					}
					return retval;
				}
				template<class _Clock, class _Duration>
				mse::xscope_optional<TXScopeAccessControlledExclusivePointer<_Ty, _TAccessMutex>> xscope_try_exclusive_pointer_until(const std::chrono::time_point<_Clock, _Duration>& _Abs_time) {
					mse::xscope_optional<TXScopeAccessControlledExclusivePointer<_Ty, _TAccessMutex>> retval(TXScopeAccessControlledExclusivePointer<_Ty, _TAccessMutex>(m_guarded_obj.m_obj, m_guarded_obj.m_mutex1, std::try_to_lock, _Abs_time));
					if (!((*retval).is_valid())) {
						return{};
					}
					return retval;
				}

			protected:
				TAccessControlledPointer<_Ty, _TAccessMutex> pointer() {
					return TAccessControlledPointer<_Ty, _TAccessMutex>(m_guarded_obj.m_obj, m_guarded_obj.m_mutex1);
				}
				mse::mstd::optional<TAccessControlledPointer<_Ty, _TAccessMutex>> try_pointer() {
					mse::mstd::optional<TAccessControlledPointer<_Ty, _TAccessMutex>> retval(TAccessControlledPointer<_Ty, _TAccessMutex>(m_guarded_obj.m_obj, m_guarded_obj.m_mutex1, std::try_to_lock));
					if (!((*retval).is_valid())) {
						return{};
					}
					return retval;
				}
				template<class _Rep, class _Period>
				mse::mstd::optional<TAccessControlledPointer<_Ty, _TAccessMutex>> try_pointer_for(const std::chrono::duration<_Rep, _Period>& _Rel_time) {
					mse::mstd::optional<TAccessControlledPointer<_Ty, _TAccessMutex>> retval(TAccessControlledPointer<_Ty, _TAccessMutex>(m_guarded_obj.m_obj, m_guarded_obj.m_mutex1, std::try_to_lock, _Rel_time));
					if (!((*retval).is_valid())) {
						return{};
					}
					return retval;
				}
				template<class _Clock, class _Duration>
				mse::mstd::optional<TAccessControlledPointer<_Ty, _TAccessMutex>> try_pointer_until(const std::chrono::time_point<_Clock, _Duration>& _Abs_time) {
					mse::mstd::optional<TAccessControlledPointer<_Ty, _TAccessMutex>> retval(TAccessControlledPointer<_Ty, _TAccessMutex>(m_guarded_obj.m_obj, m_guarded_obj.m_mutex1, std::try_to_lock, _Abs_time));
					if (!((*retval).is_valid())) {
						return{};
					}
					return retval;
				}
				TAccessControlledConstPointer<_Ty, _TAccessMutex> const_pointer() const {
					return TAccessControlledConstPointer<_Ty, _TAccessMutex>(m_guarded_obj.m_obj, m_guarded_obj.m_mutex1);
				}
				mse::mstd::optional<TAccessControlledConstPointer<_Ty, _TAccessMutex>> try_const_pointer() const {
					mse::mstd::optional<TAccessControlledConstPointer<_Ty, _TAccessMutex>> retval(TAccessControlledConstPointer<_Ty, _TAccessMutex>(m_guarded_obj.m_obj, m_guarded_obj.m_mutex1, std::try_to_lock));
					if (!((*retval).is_valid())) {
						return{};
					}
					return retval;
				}
				template<class _Rep, class _Period>
				mse::mstd::optional<TAccessControlledConstPointer<_Ty, _TAccessMutex>> try_const_pointer_for(const std::chrono::duration<_Rep, _Period>& _Rel_time) const {
					mse::mstd::optional<TAccessControlledConstPointer<_Ty, _TAccessMutex>> retval(TAccessControlledConstPointer<_Ty, _TAccessMutex>(m_guarded_obj.m_obj, m_guarded_obj.m_mutex1, std::try_to_lock, _Rel_time));
					if (!((*retval).is_valid())) {
						return{};
					}
					return retval;
				}
				template<class _Clock, class _Duration>
				mse::mstd::optional<TAccessControlledConstPointer<_Ty, _TAccessMutex>> try_const_pointer_until(const std::chrono::time_point<_Clock, _Duration>& _Abs_time) const {
					mse::mstd::optional<TAccessControlledConstPointer<_Ty, _TAccessMutex>> retval(TAccessControlledConstPointer<_Ty, _TAccessMutex>(m_guarded_obj.m_obj, m_guarded_obj.m_mutex1, std::try_to_lock, _Abs_time));
					if (!((*retval).is_valid())) {
						return{};
					}
					return retval;
				}
				/* Note that an exclusive_pointer cannot coexist with any other lock_ptrs (targeting the same object), including ones in
				the same thread. Thus, using exclusive_pointers without sufficient care introduces the potential for exceptions (in a way
				that sticking to (regular) pointers doesn't). */
				TAccessControlledExclusivePointer<_Ty, _TAccessMutex> exclusive_pointer() {
					return TAccessControlledExclusivePointer<_Ty, _TAccessMutex>(m_guarded_obj.m_obj, m_guarded_obj.m_mutex1);
				}
				mse::mstd::optional<TAccessControlledExclusivePointer<_Ty, _TAccessMutex>> try_exclusive_pointer() {
					mse::mstd::optional<TAccessControlledExclusivePointer<_Ty, _TAccessMutex>> retval(TAccessControlledExclusivePointer<_Ty, _TAccessMutex>(m_guarded_obj.m_obj, m_guarded_obj.m_mutex1, std::try_to_lock));
					if (!((*retval).is_valid())) {
						return{};
					}
					return retval;
				}
				template<class _Rep, class _Period>
				mse::mstd::optional<TAccessControlledExclusivePointer<_Ty, _TAccessMutex>> try_exclusive_pointer_for(const std::chrono::duration<_Rep, _Period>& _Rel_time) {
					mse::mstd::optional<TAccessControlledExclusivePointer<_Ty, _TAccessMutex>> retval(TAccessControlledExclusivePointer<_Ty, _TAccessMutex>(m_guarded_obj.m_obj, m_guarded_obj.m_mutex1, std::try_to_lock, _Rel_time));
					if (!((*retval).is_valid())) {
						return{};
					}
					return retval;
				}
				template<class _Clock, class _Duration>
				mse::mstd::optional<TAccessControlledExclusivePointer<_Ty, _TAccessMutex>> try_exclusive_pointer_until(const std::chrono::time_point<_Clock, _Duration>& _Abs_time) {
					mse::mstd::optional<TAccessControlledExclusivePointer<_Ty, _TAccessMutex>> retval(TAccessControlledExclusivePointer<_Ty, _TAccessMutex>(m_guarded_obj.m_obj, m_guarded_obj.m_mutex1, std::try_to_lock, _Abs_time));
					if (!((*retval).is_valid())) {
						return{};
					}
					return retval;
				}

			private:
				/* construction helper functions */
				template <class... Args>
				_Ty initialize(Args&&... args) {
					return _Ty(std::forward<Args>(args)...);
				}
				template <class _TSoleArg>
				_Ty constructor_helper2(std::true_type, _TSoleArg&& sole_arg) {
					/* The sole parameter is derived from, or of this type, so we're going to consider the constructor
					a move constructor. */
					return MSE_FWD(sole_arg).m_guarded_obj.m_obj;
				}
				template <class _TSoleArg>
				_Ty constructor_helper2(std::false_type, _TSoleArg&& sole_arg) {
					/* The sole parameter is not derived from, or of this type, so the constructor is not a move
					constructor. */
					return initialize(MSE_FWD(sole_arg));
				}
				template <class... Args>
				_Ty constructor_helper1(Args&&... args) {
					return initialize(std::forward<Args>(args)...);
				}
				template <class _TSoleArg>
				_Ty constructor_helper1(_TSoleArg&& sole_arg) {
					/* The constructor was given exactly one parameter. If the parameter is derived from, or of this type,
					then we're going to consider the constructor a move constructor. */
					return constructor_helper2(typename std::is_base_of<TAccessControlledObjBase, _TSoleArg>::type(), MSE_FWD(sole_arg));
				}

				TAccessControlledObjBase& operator=(TAccessControlledObjBase&& _X) = delete;
				TAccessControlledObjBase& operator=(const TAccessControlledObjBase& _X) = delete;

				MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

				typedef recursive_shared_mutex_wrapped<_TAccessMutex> _TWrappedAccessMutex;

				ns_aco::guarded_wrapper_t<_Ty, _TWrappedAccessMutex> m_guarded_obj;

				//friend class TAccessControlledReadOnlyObj<_Ty, _TAccessMutex>;
				friend class TAccessControlledPointerBase<_Ty, _TAccessMutex>;
				friend class TAccessControlledConstPointerBase<_Ty, _TAccessMutex>;
				friend class TAccessControlledExclusivePointerBase<_Ty, _TAccessMutex>;
			};

		}
	}

	template<class _Ty, class _TAccessMutex/* = non_thread_safe_recursive_shared_timed_mutex*/>
	class TXScopeAccessControlledObj : public mse::us::impl::TAccessControlledObjBase<_Ty, _TAccessMutex>, public mse::us::impl::XScopeTagBase {
	public:
		typedef mse::us::impl::TAccessControlledObjBase<_Ty, _TAccessMutex> base_class;
		typedef _Ty object_type;
		typedef _TAccessMutex access_mutex_type;

		MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TXScopeAccessControlledObj, base_class);

		/* Prefer the "xscope_" prefixed versions to acknowledge that scope iterators are returned. */
		auto pointer() {
			return base_class::xscope_pointer();
		}
		auto try_pointer() {
			return base_class::xscope_try_pointer();
		}
		template<class _Rep, class _Period>
		auto try_pointer_for(const std::chrono::duration<_Rep, _Period>& _Rel_time) {
			return base_class::xscope_try_pointer_for(_Rel_time);
		}
		template<class _Clock, class _Duration>
		auto try_pointer_until(const std::chrono::time_point<_Clock, _Duration>& _Abs_time) {
			return base_class::xscope_try_pointer_until(_Abs_time);
		}
		auto const_pointer() const {
			return base_class::xscope_const_pointer();
		}
		auto try_const_pointer() const {
			return base_class::xscope_try_const_pointer();
		}
		template<class _Rep, class _Period>
		auto try_const_pointer_for(const std::chrono::duration<_Rep, _Period>& _Rel_time) const {
			return base_class::xscope_try_const_pointer_for(_Rel_time);
		}
		template<class _Clock, class _Duration>
		auto try_const_pointer_until(const std::chrono::time_point<_Clock, _Duration>& _Abs_time) const {
			return base_class::xscope_try_const_pointer_until(_Abs_time);
		}
		auto exclusive_pointer() {
			return base_class::xscope_exclusive_pointer();
		}
		auto try_exclusive_pointer() {
			return base_class::xscope_try_exclusive_pointer();
		}
		template<class _Rep, class _Period>
		auto try_exclusive_pointer_for(const std::chrono::duration<_Rep, _Period>& _Rel_time) {
			return base_class::xscope_try_exclusive_pointer_for(_Rel_time);
		}
		template<class _Clock, class _Duration>
		auto try_exclusive_pointer_until(const std::chrono::time_point<_Clock, _Duration>& _Abs_time) {
			return base_class::xscope_try_exclusive_pointer_until(_Abs_time);
		}

	private:
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		//friend class TAccessControlledReadOnlyObj<_Ty, _TAccessMutex>;
		friend class mse::us::impl::TAccessControlledPointerBase<_Ty, _TAccessMutex>;
		friend class mse::us::impl::TAccessControlledConstPointerBase<_Ty, _TAccessMutex>;
		friend class mse::us::impl::TAccessControlledExclusivePointerBase<_Ty, _TAccessMutex>;
	};

	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled(const _Ty& src) -> TXScopeAccessControlledObj<_Ty, _TAccessMutex> {
		return TXScopeAccessControlledObj<_Ty, _TAccessMutex>(src);
	}
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled(_Ty&& src) -> TXScopeAccessControlledObj<_Ty, _TAccessMutex> {
		return TXScopeAccessControlledObj<_Ty, _TAccessMutex>(MSE_FWD(src));
	}

	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_pointer(TXScopeAccessControlledObj<_Ty, _TAccessMutex>& src) {
		return src.xscope_pointer();
	}
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_pointer(const TXScopeAccessControlledObj<_Ty, _TAccessMutex>& src) = delete;
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_pointer(const mse::TXScopeFixedPointer<TXScopeAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_pointer(*xs_ptr); }
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_pointer(const mse::TXScopeFixedConstPointer<TXScopeAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) = delete;
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_pointer(const mse::TXScopeObjFixedPointer<TXScopeAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_pointer(*xs_ptr); }
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_pointer(const mse::TXScopeObjFixedConstPointer<TXScopeAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) = delete;
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_const_pointer(const TXScopeAccessControlledObj<_Ty, _TAccessMutex>& src) {
		return src.xscope_const_pointer();
	}
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_const_pointer(const mse::TXScopeFixedPointer<TXScopeAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_const_pointer(*xs_ptr); }
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_const_pointer(const mse::TXScopeFixedConstPointer<TXScopeAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_const_pointer(*xs_ptr); }
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_const_pointer(const mse::TXScopeObjFixedPointer<TXScopeAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_const_pointer(*xs_ptr); }
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_const_pointer(const mse::TXScopeObjFixedConstPointer<TXScopeAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_const_pointer(*xs_ptr); }
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_exclusive_pointer( TXScopeAccessControlledObj<_Ty, _TAccessMutex>& src) {
		return src.xscope_exclusive_pointer();
	}
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_exclusive_pointer(const TXScopeAccessControlledObj<_Ty, _TAccessMutex>& src) = delete;
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_exclusive_pointer(const mse::TXScopeFixedPointer<TXScopeAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_exclusive_pointer(*xs_ptr); }
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_exclusive_pointer(const mse::TXScopeFixedConstPointer<TXScopeAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) = delete;
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_exclusive_pointer(const mse::TXScopeObjFixedPointer<TXScopeAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_exclusive_pointer(*xs_ptr); }
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_exclusive_pointer(const mse::TXScopeObjFixedConstPointer<TXScopeAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) = delete;
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	template<class _Ty, class _TAccessMutex/* = non_thread_safe_recursive_shared_timed_mutex*/>
	class TAccessControlledObj : public mse::us::impl::TAccessControlledObjBase<_Ty, _TAccessMutex> {
	public:
		typedef mse::us::impl::TAccessControlledObjBase<_Ty, _TAccessMutex> base_class;
		typedef _Ty object_type;
		typedef _TAccessMutex access_mutex_type;

		MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TAccessControlledObj, base_class);

		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TAccessControlledObj() {
			mse::impl::T_valid_if_is_marked_as_shareable_msemsearray<_Ty>();
		}

		auto pointer() {
			return base_class::pointer();
		}
		auto try_pointer() {
			return base_class::try_pointer();
		}
		template<class _Rep, class _Period>
		auto try_pointer_for(const std::chrono::duration<_Rep, _Period>& _Rel_time) {
			return base_class::try_pointer_for(_Rel_time);
		}
		template<class _Clock, class _Duration>
		auto try_pointer_until(const std::chrono::time_point<_Clock, _Duration>& _Abs_time) {
			return base_class::try_pointer_until(_Abs_time);
		}
		auto const_pointer() const {
			return base_class::const_pointer();
		}
		auto try_const_pointer() const {
			return base_class::try_const_pointer();
		}
		template<class _Rep, class _Period>
		auto try_const_pointer_for(const std::chrono::duration<_Rep, _Period>& _Rel_time) const {
			return base_class::try_const_pointer_for(_Rel_time);
		}
		template<class _Clock, class _Duration>
		auto try_const_pointer_until(const std::chrono::time_point<_Clock, _Duration>& _Abs_time) const {
			return base_class::try_const_pointer_until(_Abs_time);
		}
		auto exclusive_pointer() {
			return base_class::exclusive_pointer();
		}
		auto try_exclusive_pointer() {
			return base_class::try_exclusive_pointer();
		}
		template<class _Rep, class _Period>
		auto try_exclusive_pointer_for(const std::chrono::duration<_Rep, _Period>& _Rel_time) {
			return base_class::try_exclusive_pointer_for(_Rel_time);
		}
		template<class _Clock, class _Duration>
		auto try_exclusive_pointer_until(const std::chrono::time_point<_Clock, _Duration>& _Abs_time) {
			return base_class::try_exclusive_pointer_until(_Abs_time);
		}

	private:
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		//friend class TAccessControlledReadOnlyObj<_Ty, _TAccessMutex>;
		friend class mse::us::impl::TAccessControlledPointerBase<_Ty, _TAccessMutex>;
		friend class mse::us::impl::TAccessControlledConstPointerBase<_Ty, _TAccessMutex>;
		friend class mse::us::impl::TAccessControlledExclusivePointerBase<_Ty, _TAccessMutex>;
	};

	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_access_controlled(const _Ty& src) {
		return TAccessControlledObj<_Ty, _TAccessMutex>(src);
	}
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_access_controlled(_Ty&& src) {
		return TAccessControlledObj<_Ty, _TAccessMutex>(MSE_FWD(src));
	}

	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_pointer(TAccessControlledObj<_Ty, _TAccessMutex>& src) {
		return src.xscope_pointer();
	}
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_pointer(const TAccessControlledObj<_Ty, _TAccessMutex>& src) = delete;
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_pointer(const mse::TXScopeFixedPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_pointer(*xs_ptr); }
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_pointer(const mse::TXScopeFixedConstPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) = delete;
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_pointer(const mse::TXScopeObjFixedPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_pointer(*xs_ptr); }
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_pointer(const mse::TXScopeObjFixedConstPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) = delete;
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_const_pointer(const TAccessControlledObj<_Ty, _TAccessMutex>& src) {
		return src.xscope_const_pointer();
	}
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_const_pointer(const mse::TXScopeFixedPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_const_pointer(*xs_ptr); }
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_const_pointer(const mse::TXScopeFixedConstPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_const_pointer(*xs_ptr); }
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_const_pointer(const mse::TXScopeObjFixedPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_const_pointer(*xs_ptr); }
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_const_pointer(const mse::TXScopeObjFixedConstPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_const_pointer(*xs_ptr); }
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_exclusive_pointer(TAccessControlledObj<_Ty, _TAccessMutex>& src) {
		return src.xscope_exclusive_pointer();
	}
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_exclusive_pointer(const TAccessControlledObj<_Ty, _TAccessMutex>& src) = delete;
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_exclusive_pointer(const mse::TXScopeFixedPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_exclusive_pointer(*xs_ptr); }
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_exclusive_pointer(const mse::TXScopeFixedConstPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) = delete;
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_exclusive_pointer(const mse::TXScopeObjFixedPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_xscope_access_controlled_exclusive_pointer(*xs_ptr); }
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_xscope_access_controlled_exclusive_pointer(const mse::TXScopeObjFixedConstPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) = delete;
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	/* Overloads for rsv::TReturnableFParam<>. */
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_access_controlled_pointer)
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_access_controlled_const_pointer)
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_access_controlled_exclusive_pointer)

	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_access_controlled_pointer(TAccessControlledObj<_Ty, _TAccessMutex>& src) {
		return src.pointer();
	}
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_access_controlled_pointer(const TAccessControlledObj<_Ty, _TAccessMutex>& src) = delete;
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_access_controlled_pointer(const mse::TXScopeFixedPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_access_controlled_pointer(*xs_ptr); }
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_access_controlled_pointer(const mse::TXScopeFixedConstPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) = delete;
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_access_controlled_pointer(const mse::TXScopeObjFixedPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_access_controlled_pointer(*xs_ptr); }
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_access_controlled_pointer(const mse::TXScopeObjFixedConstPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) = delete;
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_access_controlled_const_pointer(const TAccessControlledObj<_Ty, _TAccessMutex>& src) {
		return src.const_pointer();
	}
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_access_controlled_const_pointer(const mse::TXScopeFixedPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_access_controlled_const_pointer(*xs_ptr); }
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_access_controlled_const_pointer(const mse::TXScopeFixedConstPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_access_controlled_const_pointer(*xs_ptr); }
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_access_controlled_const_pointer(const mse::TXScopeObjFixedPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_access_controlled_const_pointer(*xs_ptr); }
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_access_controlled_const_pointer(const mse::TXScopeObjFixedConstPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_access_controlled_const_pointer(*xs_ptr); }
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_access_controlled_exclusive_pointer(TAccessControlledObj<_Ty, _TAccessMutex>& src) {
		return src.exclusive_pointer();
	}
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_access_controlled_exclusive_pointer(const TAccessControlledObj<_Ty, _TAccessMutex>& src) = delete;
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_access_controlled_exclusive_pointer(const mse::TXScopeFixedPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_access_controlled_exclusive_pointer(*xs_ptr); }
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_access_controlled_exclusive_pointer(const mse::TXScopeFixedConstPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) = delete;
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_access_controlled_exclusive_pointer(const mse::TXScopeObjFixedPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) { return make_access_controlled_exclusive_pointer(*xs_ptr); }
	template<class _Ty, class _TAccessMutex = non_thread_safe_recursive_shared_timed_mutex>
	auto make_access_controlled_exclusive_pointer(const mse::TXScopeObjFixedConstPointer<TAccessControlledObj<_Ty, _TAccessMutex> >& xs_ptr) = delete;
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)


	namespace us {
		namespace impl {

			/* While an instance of Txscope_optional_structure_lock_guard<> exists it ensures that direct (scope) pointers to
			the element in an "optional" container do not become invalid by preventing any operation (like reset(), for
			example) that might destroy the element. Any attempt to execute such an operation would result in an exception. */

			template<class TDynamicContainer> class Txscope_const_optional_structure_lock_guard;

			template<class TDynamicContainer>
			class Txscope_optional_structure_lock_guard : public mse::us::impl::XScopeStructureLockGuardTagBase {
			public:
				Txscope_optional_structure_lock_guard(Txscope_optional_structure_lock_guard&& src) : m_stored_ptr(MSE_FWD(src).m_stored_ptr) { lock_the_target(); }
				template<class TDynamicContainer2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<TDynamicContainer2 *, TDynamicContainer *>::value> MSE_IMPL_EIS >
				Txscope_optional_structure_lock_guard(Txscope_optional_structure_lock_guard<TDynamicContainer2>&& src) : m_stored_ptr(MSE_FWD(src).m_stored_ptr) { lock_the_target(); }

				Txscope_optional_structure_lock_guard(const Txscope_optional_structure_lock_guard& src) : m_stored_ptr(src.m_stored_ptr) { lock_the_target(); }
				template<class TDynamicContainer2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<TDynamicContainer2 *, TDynamicContainer *>::value> MSE_IMPL_EIS >
				Txscope_optional_structure_lock_guard(const Txscope_optional_structure_lock_guard<TDynamicContainer2>& src) : m_stored_ptr(src.m_stored_ptr) { lock_the_target(); }

				Txscope_optional_structure_lock_guard(const mse::TXScopeObjFixedPointer<TDynamicContainer>& owner_ptr) : m_stored_ptr(owner_ptr) {
					lock_the_target();
				}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
				Txscope_optional_structure_lock_guard(const mse::TXScopeFixedPointer<TDynamicContainer>& owner_ptr) : m_stored_ptr(owner_ptr) {
					lock_the_target();
				}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
				~Txscope_optional_structure_lock_guard() {
					unlock_the_target();
				}

				auto xscope_ptr_to_element() const & {
					return mse::us::unsafe_make_xscope_pointer_to((*m_stored_ptr).value());
				}
				auto target_container_ptr() const {
					return m_stored_ptr;
				}
				operator mse::TXScopeFixedPointer<TDynamicContainer>() const & {
					return m_stored_ptr;
				}
				/*
				template<class TDynamicContainer2 = TDynamicContainer, MSE_IMPL_EIP mse::impl::enable_if_t<!std::is_same<mse::TXScopeFixedConstPointer<TDynamicContainer2>, mse::TXScopeFixedPointer<TDynamicContainer> >::value> MSE_IMPL_EIS >
				explicit operator mse::TXScopeFixedConstPointer<TDynamicContainer2>() const & {
					return m_stored_ptr;
				}
				*/
				auto& operator*() const {
					return *m_stored_ptr;
				}
				auto* operator->() const {
					return std::addressof(*m_stored_ptr);
				}
				bool operator==(const Txscope_optional_structure_lock_guard& rhs) const {
					return (rhs.m_stored_ptr == m_stored_ptr);
				}

				void async_not_shareable_and_not_passable_tag() const {}

			private:
				void lock_the_target() const {
					(*m_stored_ptr).m_structure_change_mutex.lock_shared();
				}
				void unlock_the_target() const {
					(*m_stored_ptr).m_structure_change_mutex.unlock_shared();
				}

				MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

				mse::TXScopeFixedPointer<TDynamicContainer> m_stored_ptr;

				friend class Txscope_const_optional_structure_lock_guard<TDynamicContainer>;
			};
			template<class TDynamicContainer>
			class Txscope_const_optional_structure_lock_guard : public mse::us::impl::XScopeStructureLockGuardTagBase {
			public:
				Txscope_const_optional_structure_lock_guard(Txscope_const_optional_structure_lock_guard&& src) : m_stored_ptr(MSE_FWD(src).m_stored_ptr) { lock_the_target(); }
				template<class TDynamicContainer2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<TDynamicContainer2 *, TDynamicContainer *>::value> MSE_IMPL_EIS >
				Txscope_const_optional_structure_lock_guard(Txscope_const_optional_structure_lock_guard<TDynamicContainer2>&& src) : m_stored_ptr(MSE_FWD(src).m_stored_ptr) { lock_the_target(); }

				Txscope_const_optional_structure_lock_guard(const Txscope_const_optional_structure_lock_guard& src) : m_stored_ptr(src.m_stored_ptr) { lock_the_target(); }
				template<class TDynamicContainer2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<TDynamicContainer2 *, TDynamicContainer *>::value> MSE_IMPL_EIS >
				Txscope_const_optional_structure_lock_guard(const Txscope_const_optional_structure_lock_guard<TDynamicContainer2>& src) : m_stored_ptr(src.m_stored_ptr) { lock_the_target(); }

				Txscope_const_optional_structure_lock_guard(Txscope_optional_structure_lock_guard<TDynamicContainer>&& src) : m_stored_ptr(MSE_FWD(src).m_stored_ptr) { lock_the_target(); }
				template<class TDynamicContainer2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<TDynamicContainer2 *, TDynamicContainer *>::value> MSE_IMPL_EIS >
				Txscope_const_optional_structure_lock_guard(Txscope_optional_structure_lock_guard<TDynamicContainer2>&& src) : m_stored_ptr(MSE_FWD(src).m_stored_ptr) { lock_the_target(); }

				Txscope_const_optional_structure_lock_guard(const Txscope_optional_structure_lock_guard<TDynamicContainer>& src) : m_stored_ptr(src.m_stored_ptr) { lock_the_target(); }
				template<class TDynamicContainer2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<TDynamicContainer2 *, TDynamicContainer *>::value> MSE_IMPL_EIS >
				Txscope_const_optional_structure_lock_guard(const Txscope_optional_structure_lock_guard<TDynamicContainer2>& src) : m_stored_ptr(src.m_stored_ptr) { lock_the_target(); }

				Txscope_const_optional_structure_lock_guard(const mse::TXScopeObjFixedConstPointer<TDynamicContainer>& owner_ptr) : m_stored_ptr(owner_ptr) {
					lock_the_target();
				}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
				Txscope_const_optional_structure_lock_guard(const mse::TXScopeFixedConstPointer<TDynamicContainer>& owner_ptr) : m_stored_ptr(owner_ptr) {
					lock_the_target();
				}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
				~Txscope_const_optional_structure_lock_guard() {
					unlock_the_target();
				}

				auto xscope_ptr_to_element() const & {
					return mse::us::unsafe_make_xscope_const_pointer_to((*m_stored_ptr).value());
				}
				auto target_container_ptr() const {
					return m_stored_ptr;
				}
				operator mse::TXScopeFixedConstPointer<TDynamicContainer>() const & {
					return m_stored_ptr;
				}
				const auto& operator*() const {
					return *m_stored_ptr;
				}
				const auto* operator->() const {
					return std::addressof(*m_stored_ptr);
				}
				bool operator==(const Txscope_const_optional_structure_lock_guard& rhs) const {
					return (rhs.m_stored_ptr == m_stored_ptr);
				}

				void async_not_shareable_and_not_passable_tag() const {}

			private:
				void lock_the_target() {
					(*m_stored_ptr).m_structure_change_mutex.lock_shared();
				}
				void unlock_the_target() const {
					(*m_stored_ptr).m_structure_change_mutex.unlock_shared();
				}

				MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

				mse::TXScopeFixedConstPointer<TDynamicContainer> m_stored_ptr;
			};

			/* For objects that are access controlled under an "exclusive writer" access policy, the object is immutable
			while a const pointer to the object exists. So given an "exclusive writer" const pointer to an "optional"
			container, it is safe to store the pointer and provide a direct scope const pointer to its element. */
			template<class TDynamicContainer, class _TAccessMutex/* = mse::non_thread_safe_shared_mutex*/>
			class Txscope_ewconst_optional_structure_lock_guard : public mse::us::impl::XScopeStructureLockGuardTagBase {
			public:
				typedef mse::TXScopeAccessControlledConstPointer<TDynamicContainer, _TAccessMutex> xs_exclusive_writer_const_pointer_t;

				Txscope_ewconst_optional_structure_lock_guard(Txscope_ewconst_optional_structure_lock_guard&& src) : m_stored_ptr(MSE_FWD(src).m_stored_ptr) {}
				template<class TDynamicContainer2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<TDynamicContainer2 *, TDynamicContainer *>::value> MSE_IMPL_EIS >
				Txscope_ewconst_optional_structure_lock_guard(Txscope_ewconst_optional_structure_lock_guard<TDynamicContainer2, _TAccessMutex>&& src) : m_stored_ptr(MSE_FWD(src).m_stored_ptr) {}

				Txscope_ewconst_optional_structure_lock_guard(const Txscope_ewconst_optional_structure_lock_guard& src) : m_stored_ptr(src.m_stored_ptr) {}
				template<class TDynamicContainer2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<TDynamicContainer2 *, TDynamicContainer *>::value> MSE_IMPL_EIS >
				Txscope_ewconst_optional_structure_lock_guard(const Txscope_ewconst_optional_structure_lock_guard<TDynamicContainer2, _TAccessMutex>& src) : m_stored_ptr(src.m_stored_ptr) {}

				Txscope_ewconst_optional_structure_lock_guard(const xs_exclusive_writer_const_pointer_t& owner_ptr)
					: m_stored_ptr(owner_ptr) {}
				Txscope_ewconst_optional_structure_lock_guard(xs_exclusive_writer_const_pointer_t&& owner_ptr)
					: m_stored_ptr(MSE_FWD(owner_ptr)) {}

				~Txscope_ewconst_optional_structure_lock_guard() {
					mse::impl::T_valid_if_is_exclusive_writer_enforcing_mutex_msemsearray<_TAccessMutex>();
				}

				auto xscope_ptr_to_element() const & {
					return mse::us::unsafe_make_xscope_const_pointer_to((*m_stored_ptr).value());
				}
				auto target_container_ptr() const {
					return m_stored_ptr;
				}
				operator mse::TXScopeFixedConstPointer<TDynamicContainer>() const & {
					auto xs_sp_store = mse::make_xscope_strong_pointer_store(m_stored_ptr);
					return xs_sp_store.xscope_ptr();
				}
				operator xs_exclusive_writer_const_pointer_t() const {
					return m_stored_ptr;
				}
				const auto& operator*() const {
					return *m_stored_ptr;
				}
				const auto* operator->() const {
					return std::addressof(*m_stored_ptr);
				}

				void async_not_shareable_and_not_passable_tag() const {}

			private:
				MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

				xs_exclusive_writer_const_pointer_t m_stored_ptr;
			};

			template<class TDynamicContainer, class TBaseContainerStructureLockGuard> class Txscope_const_optional_structure_lock_guard_of_wrapper;

			template<class TDynamicContainer, class TBaseContainerStructureLockGuard>
			class Txscope_optional_structure_lock_guard_of_wrapper : public mse::us::impl::XScopeStructureLockGuardTagBase {
			public:
				Txscope_optional_structure_lock_guard_of_wrapper(const Txscope_optional_structure_lock_guard_of_wrapper&) = default;
				template<class TDynamicContainer2, class TBaseContainerStructureLockGuard2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<TDynamicContainer2 *, TDynamicContainer *>::value && std::is_convertible<TBaseContainerStructureLockGuard2, TBaseContainerStructureLockGuard>::value> MSE_IMPL_EIS >
				Txscope_optional_structure_lock_guard_of_wrapper(const Txscope_optional_structure_lock_guard_of_wrapper<TDynamicContainer2, TBaseContainerStructureLockGuard2>& src)
					: m_stored_ptr(src.m_stored_ptr), m_MV_xscope_optional_structure_lock_guard(src.m_MV_xscope_optional_structure_lock_guard) {}

				template<typename TBaseContainerStructureLockGuardInitParam>
				Txscope_optional_structure_lock_guard_of_wrapper(const mse::TXScopeObjFixedPointer<TDynamicContainer>& owner_ptr, const TBaseContainerStructureLockGuardInitParam& MV_xscope_optional_structure_lock_guard_init_param)
					: m_stored_ptr(owner_ptr), m_MV_xscope_optional_structure_lock_guard(MV_xscope_optional_structure_lock_guard_init_param) {}
				template<typename TBaseContainerStructureLockGuardInitParam>
				Txscope_optional_structure_lock_guard_of_wrapper(const mse::TXScopeObjFixedPointer<TDynamicContainer>& owner_ptr, TBaseContainerStructureLockGuardInitParam&& MV_xscope_optional_structure_lock_guard_init_param)
					: m_stored_ptr(owner_ptr), m_MV_xscope_optional_structure_lock_guard(MSE_FWD(MV_xscope_optional_structure_lock_guard_init_param)) {}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
				template<typename TBaseContainerStructureLockGuardInitParam>
				Txscope_optional_structure_lock_guard_of_wrapper(const mse::TXScopeFixedPointer<TDynamicContainer>& owner_ptr, const TBaseContainerStructureLockGuardInitParam& MV_xscope_optional_structure_lock_guard_init_param)
					: m_stored_ptr(owner_ptr), m_MV_xscope_optional_structure_lock_guard(MV_xscope_optional_structure_lock_guard_init_param) {}
				template<typename TBaseContainerStructureLockGuardInitParam>
				Txscope_optional_structure_lock_guard_of_wrapper(const mse::TXScopeFixedPointer<TDynamicContainer>& owner_ptr, TBaseContainerStructureLockGuardInitParam&& MV_xscope_optional_structure_lock_guard_init_param)
					: m_stored_ptr(owner_ptr), m_MV_xscope_optional_structure_lock_guard(MSE_FWD(MV_xscope_optional_structure_lock_guard_init_param)) {}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

				auto xscope_ptr_to_element() const & {
					return mse::us::unsafe_make_xscope_pointer_to((*m_stored_ptr).value());
				}
				auto target_container_ptr() const {
					return m_stored_ptr;
				}
				operator mse::TXScopeFixedPointer<TDynamicContainer>() const & {
					return m_stored_ptr;
				}
				/*
				template<class TDynamicContainer2 = TDynamicContainer, MSE_IMPL_EIP mse::impl::enable_if_t<!std::is_same<mse::TXScopeFixedConstPointer<TDynamicContainer2>, mse::TXScopeFixedPointer<TDynamicContainer> >::value> MSE_IMPL_EIS >
				explicit operator mse::TXScopeFixedConstPointer<TDynamicContainer2>() const & {
					return m_stored_ptr;
				}
				*/
				auto& operator*() const {
					return *m_stored_ptr;
				}
				auto* operator->() const {
					return std::addressof(*m_stored_ptr);
				}
				bool operator==(const Txscope_optional_structure_lock_guard_of_wrapper& rhs) const {
					return (rhs.m_stored_ptr == m_stored_ptr);
				}

				void async_not_shareable_and_not_passable_tag() const {}

			private:
				MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

				mse::TXScopeFixedPointer<TDynamicContainer> m_stored_ptr;
				TBaseContainerStructureLockGuard m_MV_xscope_optional_structure_lock_guard;

				template<class TDynamicContainer2, class TBaseContainerStructureLockGuard2>
				friend class Txscope_const_optional_structure_lock_guard_of_wrapper;
			};
			template<class TDynamicContainer, class TBaseContainerStructureLockGuard>
			class Txscope_const_optional_structure_lock_guard_of_wrapper : public mse::us::impl::XScopeStructureLockGuardTagBase {
			public:
				Txscope_const_optional_structure_lock_guard_of_wrapper(const Txscope_const_optional_structure_lock_guard_of_wrapper&) = default;
				template<class TDynamicContainer2, class TBaseContainerStructureLockGuard2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<TDynamicContainer2 *, TDynamicContainer *>::value && std::is_convertible<TBaseContainerStructureLockGuard2, TBaseContainerStructureLockGuard>::value> MSE_IMPL_EIS >
				Txscope_const_optional_structure_lock_guard_of_wrapper(const Txscope_const_optional_structure_lock_guard_of_wrapper<TDynamicContainer2, TBaseContainerStructureLockGuard2>& src)
					: m_stored_ptr(src.m_stored_ptr), m_MV_xscope_optional_structure_lock_guard(src.m_MV_xscope_optional_structure_lock_guard) {}

				Txscope_const_optional_structure_lock_guard_of_wrapper(const Txscope_optional_structure_lock_guard_of_wrapper<TDynamicContainer, TBaseContainerStructureLockGuard>& src)
					: m_stored_ptr(src.m_stored_ptr), m_MV_xscope_optional_structure_lock_guard(src.m_MV_xscope_optional_structure_lock_guard) {}
				template<class TDynamicContainer2, class TBaseContainerStructureLockGuard2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<TDynamicContainer2 *, TDynamicContainer *>::value && std::is_convertible<TBaseContainerStructureLockGuard2, TBaseContainerStructureLockGuard>::value> MSE_IMPL_EIS >
				Txscope_const_optional_structure_lock_guard_of_wrapper(const Txscope_optional_structure_lock_guard_of_wrapper<TDynamicContainer2, TBaseContainerStructureLockGuard2>& src)
					: m_stored_ptr(src.m_stored_ptr), m_MV_xscope_optional_structure_lock_guard(src.m_MV_xscope_optional_structure_lock_guard) {}

				template<typename TBaseContainerStructureLockGuardInitParam>
				Txscope_const_optional_structure_lock_guard_of_wrapper(const mse::TXScopeObjFixedConstPointer<TDynamicContainer>& owner_ptr, const TBaseContainerStructureLockGuardInitParam& MV_xscope_optional_structure_lock_guard_init_param)
					: m_stored_ptr(owner_ptr), m_MV_xscope_optional_structure_lock_guard(MV_xscope_optional_structure_lock_guard_init_param) {}
				template<typename TBaseContainerStructureLockGuardInitParam>
				Txscope_const_optional_structure_lock_guard_of_wrapper(const mse::TXScopeObjFixedConstPointer<TDynamicContainer>& owner_ptr, TBaseContainerStructureLockGuardInitParam&& MV_xscope_optional_structure_lock_guard_init_param)
					: m_stored_ptr(owner_ptr), m_MV_xscope_optional_structure_lock_guard(MSE_FWD(MV_xscope_optional_structure_lock_guard_init_param)) {}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
				template<typename TBaseContainerStructureLockGuardInitParam>
				Txscope_const_optional_structure_lock_guard_of_wrapper(const mse::TXScopeFixedConstPointer<TDynamicContainer>& owner_ptr, const TBaseContainerStructureLockGuardInitParam& MV_xscope_optional_structure_lock_guard_init_param)
					: m_stored_ptr(owner_ptr), m_MV_xscope_optional_structure_lock_guard(MV_xscope_optional_structure_lock_guard_init_param) {}
				template<typename TBaseContainerStructureLockGuardInitParam>
				Txscope_const_optional_structure_lock_guard_of_wrapper(const mse::TXScopeFixedConstPointer<TDynamicContainer>& owner_ptr, TBaseContainerStructureLockGuardInitParam&& MV_xscope_optional_structure_lock_guard_init_param)
					: m_stored_ptr(owner_ptr), m_MV_xscope_optional_structure_lock_guard(MSE_FWD(MV_xscope_optional_structure_lock_guard_init_param)) {}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

				auto xscope_ptr_to_element() const & {
					return mse::us::unsafe_make_xscope_const_pointer_to((*m_stored_ptr).value());
				}
				auto target_container_ptr() const {
					return m_stored_ptr;
				}
				operator mse::TXScopeFixedConstPointer<TDynamicContainer>() const & {
					return m_stored_ptr;
				}
				const auto& operator*() const {
					return *m_stored_ptr;
				}
				const auto* operator->() const {
					return std::addressof(*m_stored_ptr);
				}
				bool operator==(const Txscope_const_optional_structure_lock_guard_of_wrapper& rhs) const {
					return (rhs.m_stored_ptr == m_stored_ptr);
				}

				void async_not_shareable_and_not_passable_tag() const {}

			private:
				MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

				mse::TXScopeFixedConstPointer<TDynamicContainer> m_stored_ptr;
				TBaseContainerStructureLockGuard m_MV_xscope_optional_structure_lock_guard;
			};
		}
	}

	namespace us {
		namespace impl {
			namespace ns_optional {
				/* While an instance of xscope_optional_structure_lock_guard exists it ensures that direct (scope) pointers to
				the element in the "optional" does not become invalid by preventing any operation that might destroy the element. 
				Any attempt to execute such an operation would result in an exception. */
				/* Constructing an xscope_optional_structure_lock_guard involves a (shared) lock operation on the mse::us::impl::ns_optional::optional_base2<>'s
				mutable m_structure_change_mutex. So in cases where m_structure_change_mutex is not thread safe, neither is
				the construction of xscope_optional_structure_lock_guard from a const reference. So while it's safe to expose
				generally xscope_optional_structure_lock_guards constructed from non-const references, it's up to the specific
				specializations and derived classes of mse::us::impl::ns_optional::optional_base2<> to (optionally) expose xscope_optional_structure_lock_guards
				constructed from const references and ensure their safety by either indicating that they are not eligible to
				be shared between threads (like stoptional<> does), or ensuring that m_structure_change_mutex is thread safe
				(like mtoptional<> does). */
				template <class T, class _TStateMutex, class TConstLockableIndicator>
				class xscope_const_optional_structure_lock_guard : public mse::us::impl::Txscope_const_optional_structure_lock_guard<mse::us::impl::ns_optional::optional_base2<T, _TStateMutex, TConstLockableIndicator> > {
				public:
					typedef mse::us::impl::Txscope_const_optional_structure_lock_guard<mse::us::impl::ns_optional::optional_base2<T, _TStateMutex, TConstLockableIndicator> > base_class;
					using base_class::base_class;

					operator mse::TXScopeFixedConstPointer<mse::us::impl::ns_optional::optional_base2<T, _TStateMutex, TConstLockableIndicator> >() const {
						return static_cast<const base_class&>(*this);
					}
				private:
					MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
				};
			}
		}
	}

	namespace impl {
		namespace ns_optional {
			/* While an instance of xscope_optional_structure_lock_guard exists it ensures that direct (scope) pointers to
			the element in the "optional" does not become invalid by preventing any operation that might destroy the element.
			Any attempt to execute such an operation would result in an exception. */
			template <class T, class _TStateMutex, class TConstLockableIndicator>
			class xscope_optional_structure_lock_guard : public mse::us::impl::Txscope_optional_structure_lock_guard<mse::us::impl::ns_optional::optional_base2<T, _TStateMutex, TConstLockableIndicator> > {
			public:
				typedef mse::us::impl::Txscope_optional_structure_lock_guard<mse::us::impl::ns_optional::optional_base2<T, _TStateMutex, TConstLockableIndicator> > base_class;
				typedef mse::us::impl::ns_optional::optional_base2<T, _TStateMutex, TConstLockableIndicator> TDynamicContainer;
				using base_class::base_class;

				operator mse::TXScopeFixedPointer<TDynamicContainer>() const {
					return static_cast<const base_class&>(*this);
				}
				/*
				template<class TDynamicContainer2 = TDynamicContainer, MSE_IMPL_EIP mse::impl::enable_if_t<!std::is_same<mse::TXScopeFixedConstPointer<TDynamicContainer2>, mse::TXScopeFixedPointer<TDynamicContainer> >::value> MSE_IMPL_EIS >
				explicit operator mse::TXScopeFixedConstPointer<TDynamicContainer2>() const {
					return mse::TXScopeFixedConstPointer<TDynamicContainer2>(static_cast<const base_class&>(*this));
				}
				*/
			private:
				MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
			};

			/* For objects that are access controlled under an "exclusive writer" access policy, the object is immutable
			while a const pointer to the object exists. So given an "exclusive writer" const pointer to an optional, it is
			safe to provide a direct scope const pointer to its element. */
			template<class T, class _TStateMutex, class TConstLockableIndicator, class _TAccessMutex/* = mse::non_thread_safe_shared_mutex*/>
			class xscope_ewconst_optional_structure_lock_guard : public mse::us::impl::Txscope_ewconst_optional_structure_lock_guard<mse::us::impl::ns_optional::optional_base2<T, _TStateMutex, TConstLockableIndicator>, _TAccessMutex> {
			public:
				typedef mse::us::impl::Txscope_ewconst_optional_structure_lock_guard<mse::us::impl::ns_optional::optional_base2<T, _TStateMutex, TConstLockableIndicator>, _TAccessMutex> base_class;
				using base_class::base_class;

				typedef mse::TXScopeAccessControlledConstPointer<mse::us::impl::ns_optional::optional_base2<T, _TStateMutex, TConstLockableIndicator>, _TAccessMutex> exclusive_writer_const_pointer_t;

				operator exclusive_writer_const_pointer_t() const {
					return static_cast<const base_class&>(*this);
				}
			private:
				MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
			};
		}
	}

	/* While an instance of xscope_optional_structure_lock_guard exists it ensures that direct (scope) pointers to
	the element in the "optional" does not become invalid by preventing any operation that might destroy the element.
	Any attempt to execute such an operation would result in an exception. */
	template<class T, class _TStateMutex, class TConstLockableIndicator>
	auto make_xscope_optional_structure_lock_guard(const mse::TXScopeObjFixedPointer<mse::us::impl::ns_optional::optional_base2<T, _TStateMutex, TConstLockableIndicator> >& owner_ptr) -> mse::impl::ns_optional::xscope_optional_structure_lock_guard<T, _TStateMutex, TConstLockableIndicator> {
		return mse::impl::ns_optional::xscope_optional_structure_lock_guard<T, _TStateMutex, TConstLockableIndicator>(owner_ptr);
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class T, class _TStateMutex, class TConstLockableIndicator>
	auto make_xscope_optional_structure_lock_guard(const mse::TXScopeFixedPointer<mse::us::impl::ns_optional::optional_base2<T, _TStateMutex, TConstLockableIndicator> >& owner_ptr) -> mse::impl::ns_optional::xscope_optional_structure_lock_guard<T, _TStateMutex, TConstLockableIndicator> {
		return mse::impl::ns_optional::xscope_optional_structure_lock_guard<T, _TStateMutex, TConstLockableIndicator>(owner_ptr);
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	template<class T, class _TStateMutex>
	auto make_xscope_optional_structure_lock_guard(const mse::TXScopeObjFixedConstPointer<mse::us::impl::ns_optional::optional_base2<T, _TStateMutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag> >& owner_ptr) -> mse::impl::ns_optional::xscope_optional_structure_lock_guard<T, _TStateMutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag> {
		return mse::impl::ns_optional::xscope_optional_structure_lock_guard<T, _TStateMutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag>(owner_ptr);
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class T, class _TStateMutex>
	auto make_xscope_optional_structure_lock_guard(const mse::TXScopeFixedConstPointer<mse::us::impl::ns_optional::optional_base2<T, _TStateMutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag> >& owner_ptr) -> mse::impl::ns_optional::xscope_optional_structure_lock_guard<T, _TStateMutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag> {
		return mse::impl::ns_optional::xscope_optional_structure_lock_guard<T, _TStateMutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag>(owner_ptr);
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	/* For objects that are access controlled under an "exclusive writer" access policy, the object is immutable
	while a const pointer to the object exists. So given an "exclusive writer" const pointer to an optional, it is
	safe to provide a direct scope const pointer to its element. */
	template<class T, class _TStateMutex, class TConstLockableIndicator, class _TAccessMutex/* = mse::non_thread_safe_shared_mutex*/>
	auto make_xscope_optional_structure_lock_guard(const mse::TXScopeAccessControlledConstPointer<mse::us::impl::ns_optional::optional_base2<T, _TStateMutex, TConstLockableIndicator>, _TAccessMutex>& owner_ptr) -> mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, _TStateMutex, TConstLockableIndicator, _TAccessMutex> {
		return mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, _TStateMutex, TConstLockableIndicator, _TAccessMutex>(owner_ptr);
	}
	template<class T, class _TStateMutex, class TConstLockableIndicator, class _TAccessMutex/* = mse::non_thread_safe_shared_mutex*/>
	auto make_xscope_optional_structure_lock_guard(const mse::TAccessControlledConstPointer<mse::us::impl::ns_optional::optional_base2<T, _TStateMutex, TConstLockableIndicator>, _TAccessMutex>& owner_ptr) -> mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, _TStateMutex, TConstLockableIndicator, _TAccessMutex> {
		return mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, _TStateMutex, TConstLockableIndicator, _TAccessMutex>(owner_ptr);
	}

	template<class T>
	auto make_xscope_optional_structure_lock_guard(const mse::TXScopeObjFixedPointer<optional<T> >& owner_ptr) {
		return make_xscope_optional_structure_lock_guard(mse::TXScopeFixedPointer<typename optional<T>::base_class>(owner_ptr));
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class T>
	auto make_xscope_optional_structure_lock_guard(const mse::TXScopeFixedPointer<optional<T> >& owner_ptr) {
		return make_xscope_optional_structure_lock_guard(mse::TXScopeFixedPointer<typename optional<T>::base_class>(owner_ptr));
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class T, class _TAccessMutex>
	auto make_xscope_optional_structure_lock_guard(const mse::TXScopeAccessControlledConstPointer<optional<T>, _TAccessMutex>& owner_ptr) -> mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, mse::non_thread_safe_shared_mutex, mse::us::impl::ns_optional::optional_base2_not_const_lockable_tag, _TAccessMutex> {
		return mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, mse::non_thread_safe_shared_mutex, mse::us::impl::ns_optional::optional_base2_not_const_lockable_tag, _TAccessMutex>(owner_ptr);
	}
	template<class T, class _TAccessMutex>
	auto make_xscope_optional_structure_lock_guard(const mse::TAccessControlledConstPointer<optional<T>, _TAccessMutex>& owner_ptr) -> mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, mse::non_thread_safe_shared_mutex, mse::us::impl::ns_optional::optional_base2_not_const_lockable_tag, _TAccessMutex> {
		return mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, mse::non_thread_safe_shared_mutex, mse::us::impl::ns_optional::optional_base2_not_const_lockable_tag, _TAccessMutex>(owner_ptr);
	}

	template<class T>
	auto make_xscope_optional_structure_lock_guard(const mse::TXScopeObjFixedPointer<mt_optional<T> >& owner_ptr) {
		return make_xscope_optional_structure_lock_guard(mse::TXScopeFixedPointer<typename mt_optional<T>::base_class>(owner_ptr));
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class T>
	auto make_xscope_optional_structure_lock_guard(const mse::TXScopeFixedPointer<mt_optional<T> >& owner_ptr) {
		return make_xscope_optional_structure_lock_guard(mse::TXScopeFixedPointer<typename mt_optional<T>::base_class>(owner_ptr));
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class T, class _TAccessMutex>
	auto make_xscope_optional_structure_lock_guard(const mse::TXScopeAccessControlledConstPointer<mt_optional<T>, _TAccessMutex>& owner_ptr) -> mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, mse::shareable_dynamic_container_mutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag, _TAccessMutex> {
		return mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, mse::shareable_dynamic_container_mutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag, _TAccessMutex>(owner_ptr);
	}
	template<class T, class _TAccessMutex>
	auto make_xscope_optional_structure_lock_guard(const mse::TAccessControlledConstPointer<mt_optional<T>, _TAccessMutex>& owner_ptr) -> mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, mse::shareable_dynamic_container_mutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag, _TAccessMutex> {
		return mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, mse::shareable_dynamic_container_mutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag, _TAccessMutex>(owner_ptr);
	}

	template<class T>
	auto make_xscope_optional_structure_lock_guard(const mse::TXScopeObjFixedPointer<st_optional<T> >& owner_ptr) {
		return make_xscope_optional_structure_lock_guard(mse::TXScopeFixedPointer<typename st_optional<T>::base_class>(owner_ptr));
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class T>
	auto make_xscope_optional_structure_lock_guard(const mse::TXScopeFixedPointer<st_optional<T> >& owner_ptr) {
		return make_xscope_optional_structure_lock_guard(mse::TXScopeFixedPointer<typename st_optional<T>::base_class>(owner_ptr));
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class T, class _TAccessMutex>
	auto make_xscope_optional_structure_lock_guard(const mse::TXScopeAccessControlledConstPointer<st_optional<T>, _TAccessMutex>& owner_ptr) -> mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, mse::non_thread_safe_shared_mutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag, _TAccessMutex> {
		return mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, mse::non_thread_safe_shared_mutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag, _TAccessMutex>(owner_ptr);
	}
	template<class T, class _TAccessMutex>
	auto make_xscope_optional_structure_lock_guard(const mse::TAccessControlledConstPointer<st_optional<T>, _TAccessMutex>& owner_ptr) -> mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, mse::non_thread_safe_shared_mutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag, _TAccessMutex> {
		return mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, mse::non_thread_safe_shared_mutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag, _TAccessMutex>(owner_ptr);
	}

	template<class T>
	auto make_xscope_optional_structure_lock_guard(const mse::TXScopeObjFixedPointer<mse::mstd::optional<T> >& owner_ptr) {
		return make_xscope_optional_structure_lock_guard(mse::TXScopeFixedPointer<typename mse::mstd::optional<T>::base_class>(owner_ptr));
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class T>
	auto make_xscope_optional_structure_lock_guard(const mse::TXScopeFixedPointer<mse::mstd::optional<T> >& owner_ptr) {
		return make_xscope_optional_structure_lock_guard(mse::TXScopeFixedPointer<typename mse::mstd::optional<T>::base_class>(owner_ptr));
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
	namespace mstd {
		template<class T>
		auto make_xscope_optional_structure_lock_guard(const mse::TXScopeObjFixedPointer<mse::mstd::optional<T> >& owner_ptr) -> decltype(mse::make_xscope_optional_structure_lock_guard(owner_ptr)) {
			return mse::make_xscope_optional_structure_lock_guard(owner_ptr);
		}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		template<class T>
		auto make_xscope_optional_structure_lock_guard(const mse::TXScopeFixedPointer<mse::mstd::optional<T> >& owner_ptr) -> decltype(mse::make_xscope_optional_structure_lock_guard(owner_ptr)) {
			return mse::make_xscope_optional_structure_lock_guard(owner_ptr);
		}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
	}

	template<class T>
	auto make_xscope_optional_structure_lock_guard(const mse::TXScopeObjFixedPointer<xscope_optional<T> >& owner_ptr) {
		return make_xscope_optional_structure_lock_guard(mse::TXScopeFixedPointer<typename xscope_optional<T>::base_class>(owner_ptr));
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class T>
	auto make_xscope_optional_structure_lock_guard(const mse::TXScopeFixedPointer<xscope_optional<T> >& owner_ptr) {
		return make_xscope_optional_structure_lock_guard(mse::TXScopeFixedPointer<typename xscope_optional<T>::base_class>(owner_ptr));
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class T, class _TAccessMutex>
	auto make_xscope_optional_structure_lock_guard(const mse::TXScopeAccessControlledConstPointer<xscope_optional<T>, _TAccessMutex>& owner_ptr) -> mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, mse::non_thread_safe_shared_mutex, mse::us::impl::ns_optional::optional_base2_not_const_lockable_tag, _TAccessMutex> {
		return mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, mse::non_thread_safe_shared_mutex, mse::us::impl::ns_optional::optional_base2_not_const_lockable_tag, _TAccessMutex>(owner_ptr);
	}
	template<class T, class _TAccessMutex>
	auto make_xscope_optional_structure_lock_guard(const mse::TAccessControlledConstPointer<xscope_optional<T>, _TAccessMutex>& owner_ptr) -> mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, mse::non_thread_safe_shared_mutex, mse::us::impl::ns_optional::optional_base2_not_const_lockable_tag, _TAccessMutex> {
		return mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, mse::non_thread_safe_shared_mutex, mse::us::impl::ns_optional::optional_base2_not_const_lockable_tag, _TAccessMutex>(owner_ptr);
	}

	template<class T>
	auto make_xscope_optional_structure_lock_guard(const mse::TXScopeObjFixedPointer<xscope_mt_optional<T> >& owner_ptr) {
		return make_xscope_optional_structure_lock_guard(mse::TXScopeFixedPointer<typename xscope_mt_optional<T>::base_class>(owner_ptr));
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class T>
	auto make_xscope_optional_structure_lock_guard(const mse::TXScopeFixedPointer<xscope_mt_optional<T> >& owner_ptr) {
		return make_xscope_optional_structure_lock_guard(mse::TXScopeFixedPointer<typename xscope_mt_optional<T>::base_class>(owner_ptr));
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class T, class _TAccessMutex>
	auto make_xscope_optional_structure_lock_guard(const mse::TXScopeAccessControlledConstPointer<xscope_mt_optional<T>, _TAccessMutex>& owner_ptr) -> mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, mse::shareable_dynamic_container_mutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag, _TAccessMutex> {
		return mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, mse::shareable_dynamic_container_mutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag, _TAccessMutex>(owner_ptr);
	}
	template<class T, class _TAccessMutex>
	auto make_xscope_optional_structure_lock_guard(const mse::TAccessControlledConstPointer<xscope_mt_optional<T>, _TAccessMutex>& owner_ptr) -> mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, mse::shareable_dynamic_container_mutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag, _TAccessMutex> {
		return mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, mse::shareable_dynamic_container_mutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag, _TAccessMutex>(owner_ptr);
	}

	template<class T>
	auto make_xscope_optional_structure_lock_guard(const mse::TXScopeObjFixedPointer<xscope_st_optional<T> >& owner_ptr) {
		return make_xscope_optional_structure_lock_guard(mse::TXScopeFixedPointer<typename xscope_st_optional<T>::base_class>(owner_ptr));
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class T>
	auto make_xscope_optional_structure_lock_guard(const mse::TXScopeFixedPointer<xscope_st_optional<T> >& owner_ptr) {
		return make_xscope_optional_structure_lock_guard(mse::TXScopeFixedPointer<typename xscope_st_optional<T>::base_class>(owner_ptr));
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
	template<class T, class _TAccessMutex>
	auto make_xscope_optional_structure_lock_guard(const mse::TXScopeAccessControlledConstPointer<xscope_st_optional<T>, _TAccessMutex>& owner_ptr) -> mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, mse::non_thread_safe_shared_mutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag, _TAccessMutex> {
		return mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, mse::non_thread_safe_shared_mutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag, _TAccessMutex>(owner_ptr);
	}
	template<class T, class _TAccessMutex>
	auto make_xscope_optional_structure_lock_guard(const mse::TAccessControlledConstPointer<xscope_st_optional<T>, _TAccessMutex>& owner_ptr) -> mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, mse::non_thread_safe_shared_mutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag, _TAccessMutex> {
		return mse::impl::ns_optional::xscope_ewconst_optional_structure_lock_guard<T, mse::non_thread_safe_shared_mutex, mse::us::impl::ns_optional::optional_base2_const_lockable_tag, _TAccessMutex>(owner_ptr);
	}

	/* Overloads for rsv::TReturnableFParam<>. */
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_optional_structure_lock_guard)

	namespace impl {
		namespace ns_optional {
			template<typename TPointerToOptional>
			struct TypeInfoFromPointerToOptional1 {
				/* If you get a compile error here: You may be trying to obtain a TXScopeOptionalElementFixedConstPointer<> from a const
				reference to a type of optional<> that does not support it, such as mse::optional<> or mse::xscope_optional<>. You might
				consider using another type of optional<>, such as mse::xscope_st_optional<> or mse::xscope_mt_optional<>, that does
				support it. */
				typedef mse::impl::remove_reference_t<decltype(mse::make_xscope_optional_structure_lock_guard(std::declval<TPointerToOptional>()))> structure_lock_guard_t;
				typedef typename mse::impl::remove_reference_t<decltype(*std::declval<structure_lock_guard_t>())>::value_type value_t;
			};
		}
	}

	template<typename TXScopeOptionalPointer>
	class TXScopeOptionalElementFixedConstPointer;

	template<typename TXScopeOptionalPointer>
	class TXScopeOptionalElementFixedPointer : public mse::us::TXScopeStrongFixedPointer<
		typename mse::impl::ns_optional::TypeInfoFromPointerToOptional1<TXScopeOptionalPointer>::value_t
		, typename mse::impl::ns_optional::TypeInfoFromPointerToOptional1<TXScopeOptionalPointer>::structure_lock_guard_t>, public mse::us::impl::StructureLockingObjectTagBase {
	public:
		typedef mse::us::TXScopeStrongFixedPointer<
			typename mse::impl::ns_optional::TypeInfoFromPointerToOptional1<TXScopeOptionalPointer>::value_t
			, typename mse::impl::ns_optional::TypeInfoFromPointerToOptional1<TXScopeOptionalPointer>::structure_lock_guard_t> base_class;
		typedef typename mse::impl::ns_optional::TypeInfoFromPointerToOptional1<TXScopeOptionalPointer>::value_t value_t;
		typedef typename mse::impl::ns_optional::TypeInfoFromPointerToOptional1<TXScopeOptionalPointer>::structure_lock_guard_t structure_lock_guard_t;

		TXScopeOptionalElementFixedPointer(const TXScopeOptionalElementFixedPointer&) = default;
		TXScopeOptionalElementFixedPointer(TXScopeOptionalElementFixedPointer&&) = default;

		TXScopeOptionalElementFixedPointer(const TXScopeOptionalPointer& src) : base_class(construction_helper1(src)) {}

	private:
		static auto construction_helper1(const TXScopeOptionalPointer& src) {
			auto xs_structure_lock_guard1 = mse::make_xscope_optional_structure_lock_guard(src);
			return mse::us::make_xscope_strong((*xs_structure_lock_guard1).value(), std::move(xs_structure_lock_guard1));
		}

		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		template<typename TXScopeOptionalPointer2>
		friend class TXScopeOptionalElementFixedConstPointer;
	};

	template<typename TXScopeOptionalPointer>
	class TXScopeOptionalElementFixedConstPointer : public mse::us::TXScopeStrongFixedConstPointer<
		typename mse::impl::ns_optional::TypeInfoFromPointerToOptional1<TXScopeOptionalPointer>::value_t
		, typename mse::impl::ns_optional::TypeInfoFromPointerToOptional1<TXScopeOptionalPointer>::structure_lock_guard_t>, public mse::us::impl::StructureLockingObjectTagBase {
	public:
		typedef mse::us::TXScopeStrongFixedConstPointer<
			typename mse::impl::ns_optional::TypeInfoFromPointerToOptional1<TXScopeOptionalPointer>::value_t
			, typename mse::impl::ns_optional::TypeInfoFromPointerToOptional1<TXScopeOptionalPointer>::structure_lock_guard_t> base_class;
		typedef typename mse::impl::ns_optional::TypeInfoFromPointerToOptional1<TXScopeOptionalPointer>::value_t value_t;
		typedef typename mse::impl::ns_optional::TypeInfoFromPointerToOptional1<TXScopeOptionalPointer>::structure_lock_guard_t structure_lock_guard_t;

		TXScopeOptionalElementFixedConstPointer(const TXScopeOptionalElementFixedConstPointer&) = default;
		TXScopeOptionalElementFixedConstPointer(TXScopeOptionalElementFixedConstPointer&&) = default;

		template<typename TXScopeOptionalPointer2>
		TXScopeOptionalElementFixedConstPointer(const TXScopeOptionalElementFixedPointer<TXScopeOptionalPointer2>& src) : base_class(src) {}
		template<typename TXScopeOptionalPointer2>
		TXScopeOptionalElementFixedConstPointer(TXScopeOptionalElementFixedPointer<TXScopeOptionalPointer2>&& src) : base_class(MSE_FWD(src)) {}

		TXScopeOptionalElementFixedConstPointer(const TXScopeOptionalPointer& src) : base_class(construction_helper1(src)) {}
		TXScopeOptionalElementFixedConstPointer(TXScopeOptionalPointer&& src) : base_class(construction_helper1(MSE_FWD(src))) {}

	private:
		static auto construction_helper1(const TXScopeOptionalPointer& src) {
			auto xs_structure_lock_guard1 = mse::make_xscope_optional_structure_lock_guard(src);
			return mse::us::make_xscope_const_strong((*xs_structure_lock_guard1).value(), std::move(xs_structure_lock_guard1));
		}
		static auto construction_helper1(TXScopeOptionalPointer&& src) {
			auto xs_structure_lock_guard1 = mse::make_xscope_optional_structure_lock_guard(MSE_FWD(src));
			return mse::us::make_xscope_const_strong((*xs_structure_lock_guard1).value(), std::move(xs_structure_lock_guard1));
		}

		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
	};

	template<typename TXScopeOptionalPointer>
	auto xscope_pointer(const TXScopeOptionalElementFixedPointer<TXScopeOptionalPointer>& param) {
		return mse::us::unsafe_make_xscope_pointer_to(*param);
	}
	template<typename TXScopeOptionalPointer, class = MSE_IMPL_ENABLE_IF_NOT_RETURNABLE_FPARAM(TXScopeOptionalPointer)>
	auto xscope_pointer(const TXScopeOptionalElementFixedPointer<TXScopeOptionalPointer>&& param) = delete;
	template<typename TXScopeOptionalPointer>
	auto xscope_pointer(const TXScopeOptionalElementFixedConstPointer<TXScopeOptionalPointer>& param) {
		return mse::us::unsafe_make_xscope_const_pointer_to(*param);
	}
	template<typename TXScopeOptionalPointer, class = MSE_IMPL_ENABLE_IF_NOT_RETURNABLE_FPARAM(TXScopeOptionalPointer)>
	auto xscope_pointer(const TXScopeOptionalElementFixedConstPointer<TXScopeOptionalPointer>&& param) = delete;

	template<typename TXScopeOptionalPointer>
	auto make_xscope_optional_element_pointer(const TXScopeOptionalPointer& ptr) {
		return TXScopeOptionalElementFixedPointer<mse::impl::remove_reference_t<TXScopeOptionalPointer> >(ptr);
	}
	template<typename TXScopeOptionalPointer, class = MSE_IMPL_ENABLE_IF_NOT_RETURNABLE_FPARAM(TXScopeOptionalPointer)>
	auto make_xscope_optional_element_pointer(TXScopeOptionalPointer&& ptr) {
		return TXScopeOptionalElementFixedPointer<mse::impl::remove_reference_t<TXScopeOptionalPointer> >(MSE_FWD(ptr));
	}
	template<typename TXScopeOptionalPointer>
	auto make_xscope_optional_element_const_pointer(const TXScopeOptionalPointer& ptr) {
		return TXScopeOptionalElementFixedConstPointer<mse::impl::remove_reference_t<TXScopeOptionalPointer> >(ptr);
	}
	template<typename TXScopeOptionalPointer, class = MSE_IMPL_ENABLE_IF_NOT_RETURNABLE_FPARAM(TXScopeOptionalPointer)>
	auto make_xscope_optional_element_const_pointer(TXScopeOptionalPointer&& ptr) {
		return TXScopeOptionalElementFixedConstPointer<mse::impl::remove_reference_t<TXScopeOptionalPointer> >(MSE_FWD(ptr));
	}
	/* Overloads for rsv::TReturnableFParam<>. */
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_optional_element_pointer)
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_optional_element_const_pointer)

	template<typename TOptionalPointer>
	class TOptionalElementFixedConstPointer;

	template<typename TOptionalPointer>
	class TOptionalElementFixedPointer {
	public:
		typedef typename mse::impl::remove_reference_t<decltype(*std::declval<TOptionalPointer>())>::value_type value_t;

		TOptionalElementFixedPointer(const TOptionalElementFixedPointer&) = default;
		TOptionalElementFixedPointer(TOptionalElementFixedPointer&&) = default;

		TOptionalElementFixedPointer(const TOptionalPointer& src) : m_optional_ptr(src) {}
		TOptionalElementFixedPointer(TOptionalPointer&& src) : m_optional_ptr(MSE_FWD(src)) {}

		~TOptionalElementFixedPointer() {
#ifndef MSE_OPTIONAL_NO_XSCOPE_DEPENDENCE
			mse::impl::T_valid_if_not_an_xscope_type<TOptionalPointer>();
#endif // !MSE_OPTIONAL_NO_XSCOPE_DEPENDENCE
		}

		value_t& operator*() const {
			return (*m_optional_ptr).value();
		}
		value_t* operator->() const {
			return std::addressof((*m_optional_ptr).value());
		}

		bool operator==(const value_t* _Right_cref) const { return (_Right_cref == std::addressof(*(*this))); }
		bool operator!=(const value_t* _Right_cref) const { return (!((*this) == _Right_cref)); }
		bool operator==(const TOptionalElementFixedPointer &_Right_cref) const { return ((*this) == std::addressof(*_Right_cref)); }
		bool operator!=(const TOptionalElementFixedPointer &_Right_cref) const { return (!((*this) == _Right_cref)); }
		operator bool() const { return true; }

	private:
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		TOptionalPointer m_optional_ptr;

		template<typename TOptionalPointer2>
		friend class TOptionalElementFixedConstPointer;
	};

	template<typename TOptionalPointer>
	class TOptionalElementFixedConstPointer {
	public:
		typedef typename mse::impl::remove_reference_t<decltype(*std::declval<TOptionalPointer>())>::value_type value_t;

		TOptionalElementFixedConstPointer(const TOptionalElementFixedConstPointer&) = default;
		TOptionalElementFixedConstPointer(TOptionalElementFixedConstPointer&&) = default;

		template<typename TOptionalPointer2>
		TOptionalElementFixedConstPointer(const TOptionalElementFixedPointer<TOptionalPointer2>& src) : m_optional_ptr(src.m_optional_ptr) {}
		template<typename TOptionalPointer2>
		TOptionalElementFixedConstPointer(TOptionalElementFixedPointer<TOptionalPointer2>&& src) : m_optional_ptr(MSE_FWD(src).m_optional_ptr) {}

		TOptionalElementFixedConstPointer(const TOptionalPointer& src) : m_optional_ptr(src) {}
		TOptionalElementFixedConstPointer(TOptionalPointer&& src) : m_optional_ptr(std::forward<src>(src)) {}

		~TOptionalElementFixedConstPointer() {
#ifndef MSE_OPTIONAL_NO_XSCOPE_DEPENDENCE
			mse::impl::T_valid_if_not_an_xscope_type<TOptionalPointer>();
#endif // !MSE_OPTIONAL_NO_XSCOPE_DEPENDENCE
		}

		const value_t& operator*() const {
			return (*m_optional_ptr).value();
		}
		const value_t* operator->() const {
			return std::addressof((*m_optional_ptr).value());
		}

		bool operator==(const value_t* _Right_cref) const { return (_Right_cref == std::addressof(*(*this))); }
		bool operator!=(const value_t* _Right_cref) const { return (!((*this) == _Right_cref)); }
		bool operator==(const TOptionalElementFixedConstPointer &_Right_cref) const { return ((*this) == std::addressof(*_Right_cref)); }
		bool operator!=(const TOptionalElementFixedConstPointer &_Right_cref) const { return (!((*this) == _Right_cref)); }
		operator bool() const { return true; }

	private:
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		TOptionalPointer m_optional_ptr;
	};

	template<typename TOptionalPointer>
	auto make_optional_element_pointer(const TOptionalPointer& ptr) {
		return TOptionalElementFixedPointer<mse::impl::remove_reference_t<TOptionalPointer> >(ptr);
	}
	template<typename TOptionalPointer>
	auto make_optional_element_pointer(TOptionalPointer&& ptr) {
		return TOptionalElementFixedPointer<mse::impl::remove_reference_t<TOptionalPointer> >(MSE_FWD(ptr));
	}
	template<typename TOptionalPointer>
	auto make_optional_element_const_pointer(const TOptionalPointer& ptr) {
		return TOptionalElementFixedConstPointer<mse::impl::remove_reference_t<TOptionalPointer> >(ptr);
	}
	template<typename TOptionalPointer>
	auto make_optional_element_const_pointer(TOptionalPointer&& ptr) {
		return TOptionalElementFixedConstPointer<mse::impl::remove_reference_t<TOptionalPointer> >(MSE_FWD(ptr));
	}

	namespace impl {
		namespace ns_fixed_optional {
			template<typename T> using is_fixed_optional = mse::impl::disjunction<
				mse::impl::is_instantiation_of<mse::impl::remove_const_t<T>, mse::fixed_optional>
				, mse::impl::is_instantiation_of<mse::impl::remove_const_t<T>, mse::xscope_fixed_optional>
				, mse::impl::is_instantiation_of<mse::impl::remove_const_t<T>, mse::xscope_borrowing_fixed_optional> >;
		}
	}

	template<typename TXScopeFixedOptionalPointer>
	class TXScopeFixedOptionalElementFixedConstPointer;

	template<typename TXScopeFixedOptionalPointer>
	class TXScopeFixedOptionalElementFixedPointer : public mse::us::TXScopeStrongFixedPointer<
		mse::impl::target_type<mse::impl::target_type<TXScopeFixedOptionalPointer> >, TXScopeFixedOptionalPointer> {
	public:
		typedef mse::us::TXScopeStrongFixedPointer<
			mse::impl::target_type<mse::impl::target_type<TXScopeFixedOptionalPointer> >, TXScopeFixedOptionalPointer> base_class;
		typedef mse::impl::target_type<mse::impl::target_type<TXScopeFixedOptionalPointer> > value_t;

		TXScopeFixedOptionalElementFixedPointer(const TXScopeFixedOptionalElementFixedPointer&) = default;
		TXScopeFixedOptionalElementFixedPointer(TXScopeFixedOptionalElementFixedPointer&&) = default;

		TXScopeFixedOptionalElementFixedPointer(const TXScopeFixedOptionalPointer & src) : base_class((*src).value(), src) {}

		~TXScopeFixedOptionalElementFixedPointer() {
			static_assert(mse::impl::ns_fixed_optional::is_fixed_optional<mse::impl::target_type<TXScopeFixedOptionalPointer> >::value
				, "argument must be a pointer to a 'fixed optional' type - TXScopeFixedOptionalElementFixedPointer");
		}

	private:
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		template<typename TXScopeFixedOptionalPointer2>
		friend class TXScopeFixedOptionalElementFixedConstPointer;
	};

	template<typename TXScopeFixedOptionalPointer>
	class TXScopeFixedOptionalElementFixedConstPointer : public mse::us::TXScopeStrongFixedConstPointer<
		mse::impl::target_type<mse::impl::target_type<TXScopeFixedOptionalPointer> >, TXScopeFixedOptionalPointer> {
	public:
		typedef mse::us::TXScopeStrongFixedConstPointer<
			mse::impl::target_type<mse::impl::target_type<TXScopeFixedOptionalPointer> >, TXScopeFixedOptionalPointer> base_class;
		typedef mse::impl::target_type<mse::impl::target_type<TXScopeFixedOptionalPointer> > value_t;

		TXScopeFixedOptionalElementFixedConstPointer(const TXScopeFixedOptionalElementFixedConstPointer&) = default;
		TXScopeFixedOptionalElementFixedConstPointer(TXScopeFixedOptionalElementFixedConstPointer&&) = default;

		template<typename TXScopeFixedOptionalPointer2>
		TXScopeFixedOptionalElementFixedConstPointer(const TXScopeFixedOptionalElementFixedPointer<TXScopeFixedOptionalPointer2>& src) : base_class(src) {}
		template<typename TXScopeFixedOptionalPointer2>
		TXScopeFixedOptionalElementFixedConstPointer(TXScopeFixedOptionalElementFixedPointer<TXScopeFixedOptionalPointer2>&& src) : base_class(MSE_FWD(src)) {}

		TXScopeFixedOptionalElementFixedConstPointer(const TXScopeFixedOptionalPointer& src) : base_class((*src).value(), src) {}
		TXScopeFixedOptionalElementFixedConstPointer(TXScopeFixedOptionalPointer&& src) : base_class((*src).value(), MSE_FWD(src)) {}

		~TXScopeFixedOptionalElementFixedConstPointer() {
			static_assert(mse::impl::ns_fixed_optional::is_fixed_optional<mse::impl::target_type<TXScopeFixedOptionalPointer> >::value
				, "argument must be a pointer to a 'fixed optional' type - TXScopeFixedOptionalElementFixedConstPointer");
		}

	private:
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
	};

	template<typename TXScopeFixedOptionalPointer>
	auto xscope_pointer(const TXScopeFixedOptionalElementFixedPointer<TXScopeFixedOptionalPointer>& param) {
		return mse::us::unsafe_make_xscope_pointer_to(*param);
	}
	template<typename TXScopeFixedOptionalPointer, class = MSE_IMPL_ENABLE_IF_NOT_RETURNABLE_FPARAM(TXScopeFixedOptionalPointer)>
	auto xscope_pointer(const TXScopeFixedOptionalElementFixedPointer<TXScopeFixedOptionalPointer>&& param) = delete;
	template<typename TXScopeFixedOptionalPointer>
	auto xscope_pointer(const TXScopeFixedOptionalElementFixedConstPointer<TXScopeFixedOptionalPointer>& param) {
		return mse::us::unsafe_make_xscope_const_pointer_to(*param);
	}
	template<typename TXScopeFixedOptionalPointer, class = MSE_IMPL_ENABLE_IF_NOT_RETURNABLE_FPARAM(TXScopeFixedOptionalPointer)>
	auto xscope_pointer(const TXScopeFixedOptionalElementFixedConstPointer<TXScopeFixedOptionalPointer>&& param) = delete;

	template<typename TXScopeFixedOptionalPointer>
	auto make_xscope_fixed_optional_element_pointer(const TXScopeFixedOptionalPointer& ptr) {
		return TXScopeFixedOptionalElementFixedPointer<mse::impl::remove_reference_t<TXScopeFixedOptionalPointer> >(ptr);
	}
	template<typename TXScopeFixedOptionalPointer, class = MSE_IMPL_ENABLE_IF_NOT_RETURNABLE_FPARAM(TXScopeFixedOptionalPointer)>
	auto make_xscope_fixed_optional_element_pointer(TXScopeFixedOptionalPointer&& ptr) {
		return TXScopeFixedOptionalElementFixedPointer<mse::impl::remove_reference_t<TXScopeFixedOptionalPointer> >(MSE_FWD(ptr));
	}
	template<typename TXScopeFixedOptionalPointer>
	auto make_xscope_fixed_optional_element_const_pointer(const TXScopeFixedOptionalPointer& ptr) {
		return TXScopeFixedOptionalElementFixedConstPointer<mse::impl::remove_reference_t<TXScopeFixedOptionalPointer> >(ptr);
	}
	template<typename TXScopeFixedOptionalPointer, class = MSE_IMPL_ENABLE_IF_NOT_RETURNABLE_FPARAM(TXScopeFixedOptionalPointer)>
	auto make_xscope_fixed_optional_element_const_pointer(TXScopeFixedOptionalPointer&& ptr) {
		return TXScopeFixedOptionalElementFixedConstPointer<mse::impl::remove_reference_t<TXScopeFixedOptionalPointer> >(MSE_FWD(ptr));
	}
	/* Overloads for rsv::TReturnableFParam<>. */
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_fixed_optional_element_pointer)
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_fixed_optional_element_const_pointer)

	template<typename TFixedOptionalPointer>
	using TFixedOptionalElementFixedConstPointer = TOptionalElementFixedConstPointer<TFixedOptionalPointer>;

	template<typename TFixedOptionalPointer>
	using TFixedOptionalElementFixedPointer = TOptionalElementFixedPointer<TFixedOptionalPointer>;

	template<typename TFixedOptionalPointer>
	auto make_fixed_optional_element_pointer(const TFixedOptionalPointer& ptr) {
		return TFixedOptionalElementFixedPointer<mse::impl::remove_reference_t<TFixedOptionalPointer> >(ptr);
	}
	template<typename TFixedOptionalPointer>
	auto make_fixed_optional_element_pointer(TFixedOptionalPointer&& ptr) {
		return TFixedOptionalElementFixedPointer<mse::impl::remove_reference_t<TFixedOptionalPointer> >(MSE_FWD(ptr));
	}
	template<typename TFixedOptionalPointer>
	auto make_fixed_optional_element_const_pointer(const TFixedOptionalPointer& ptr) {
		return TFixedOptionalElementFixedConstPointer<mse::impl::remove_reference_t<TFixedOptionalPointer> >(ptr);
	}
	template<typename TFixedOptionalPointer>
	auto make_fixed_optional_element_const_pointer(TFixedOptionalPointer&& ptr) {
		return TFixedOptionalElementFixedConstPointer<mse::impl::remove_reference_t<TFixedOptionalPointer> >(MSE_FWD(ptr));
	}



#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-function"
#else /*__clang__*/
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#endif /*__GNUC__*/
#endif /*__clang__*/


	namespace self_test {
		class COptionalTest1 {
		public:
			static void s_test1() {
#ifdef MSE_SELF_TESTS
				{
					// from http://en.cppreference.com/w/cpp/utility/optional
					class CB {
					public:
						static mse::mstd::optional<std::string> create(bool b) {
							if (b)
								return std::string("Godzilla");
							return {};
						}

						// std::nullopt can be used to create any (empty) std::optional
						static auto create2(bool b) {
							return b ? mse::mstd::optional<std::string>{"Godzilla"} : mse::nullopt;
						}
					};

					std::cout << "create(false) returned "
						<< CB::create(false).value_or("empty") << '\n';

					// optional-returning factory functions are usable as conditions of while and if
					if (auto str = CB::create2(true)) {
						std::cout << "create2(true) returned " << *str << '\n';
					}

					auto o1 = CB::create2(true);
					auto o2 = o1;
					o1 = o2;
					o1.swap(o2);
					std::swap(o1, o2);

					auto ui_o1 = mse::optional<std::string>{ "Mothra" };
					auto ui_o2 = ui_o1;
					ui_o1 = ui_o2;
					ui_o1.swap(ui_o2);
					std::swap(ui_o1, ui_o2);
				}
				{
					mse::mstd::optional<int> o1, // empty
						o2 = 1, // init from rvalue
						o3 = o2; // copy-constructor

								 // calls std::string( initializer_list<CharT> ) constructor
					mse::mstd::optional<std::string> o4(mse::mstd::in_place, { 'a', 'b', 'c' });

					// calls std::string( size_type count, CharT ch ) constructor
					mse::mstd::optional<std::string> o5(mse::mstd::in_place, 3, 'A');

					// Move-constructed from std::string using deduction guide to pick the type

					mse::mstd::optional<std::string> o6(std::string{ "deduction" });

					std::cout << *o2 << ' ' << *o3 << ' ' << *o4 << ' ' << *o5 << ' ' << *o6 << '\n';
				}
				{
#ifdef MSE_DISABLE_RAW_POINTER_SCOPE_RESTRICTIONS
					mse::mstd::optional<const char*> s1 = "abc", s2; // constructor
					s2 = s1; // assignment
					s1 = "def"; // decaying assignment (U = char[4], T = const char*)
					std::cout << *s2 << ' ' << *s1 << '\n';
#endif // MSE_DISABLE_RAW_POINTER_SCOPE_RESTRICTIONS
				}
				{
					using namespace std::string_literals;

					mse::mstd::optional<int> opt1 = 1;
					std::cout << "opt1: " << *opt1 << '\n';

					*opt1 = 2;
					std::cout << "opt1: " << *opt1 << '\n';

					mse::mstd::optional<std::string> opt2 = "abc"s;
					std::cout << "opt2: " << *opt2 << " size: " << opt2->size() << '\n';

					// You can "take" the contained value by calling operator* on a rvalue to optional

					auto taken = *std::move(opt2);
					std::cout << "taken: " << taken << " opt2: " << *opt2 << "size: " << opt2->size() << '\n';
				}
				{
					mse::mstd::optional<int> opt = {};

					MSE_TRY {
						int n = opt.value();
					}
					MSE_CATCH (const std::exception& e) {
#if __cpp_exceptions >= 199711
						std::cout << e.what() << '\n';
#endif // __cpp_exceptions >= 199711
					}
				}
				{
#ifdef MSE_DISABLE_RAW_POINTER_SCOPE_RESTRICTIONS
					class CB {
					public:
						static mse::mstd::optional<const char*> maybe_getenv(const char* n)
						{
#ifdef _MSC_VER
							char* x;
							size_t len;
							errno_t err = _dupenv_s(&x, &len, n);
							if ((!err) && (1 <= len))
#else /*_MSC_VER*/
							if (const char* x = std::getenv(n))
#endif /*_MSC_VER*/
								return x;
							else
								return {};
						}
				};
					std::cout << CB::maybe_getenv("MYPWD").value_or("(none)") << '\n';
#endif // MSE_DISABLE_RAW_POINTER_SCOPE_RESTRICTIONS
				}

				{
					mse::xscope_optional<int> o1, // empty
						o2(1), // init from rvalue
						o3 = o2; // copy-constructor

								 // calls std::string( initializer_list<CharT> ) constructor
					mse::xscope_optional<std::string> o4(mse::in_place, { 'a', 'b', 'c' });

					// calls std::string( size_type count, CharT ch ) constructor
					mse::xscope_optional<std::string> o5(mse::in_place, 3, 'A');

					// Move-constructed from std::string using deduction guide to pick the type

					mse::xscope_optional<std::string> o6(std::string{ "deduction" });

					std::cout << *o2 << ' ' << *o3 << ' ' << *o4 << ' ' << *o5 << ' ' << *o6 << '\n';
				}
				{
#if (!defined(_MSC_VER)) || defined(MSE_HAS_CXX17) /* There seems to be a bug with pre-C++17 msvc that causes a link error. */
					mse::xscope_optional<std::string> s1("abc"), s2; // constructor
					s2 = s1; // assignment
					s1 = "def"; // decaying assignment (U = char[4], T = const char*)
					std::cout << *s2 << ' ' << *s1 << '\n';
#endif // (!defined(_MSC_VER)) || defined(MSE_HAS_CXX17)
				}
				{
					using namespace std::string_literals;

					mse::xscope_optional<int> opt1(1);
					std::cout << "opt1: " << *opt1 << '\n';

					*opt1 = 2;
					std::cout << "opt1: " << *opt1 << '\n';

					mse::xscope_optional<std::string> opt2("abc"s);
					std::cout << "opt2: " << *opt2 << " size: " << opt2->size() << '\n';

					// You can "take" the contained value by calling operator* on a rvalue to optional

					auto taken = *std::move(opt2);
					std::cout << "taken: " << taken << " opt2: " << *opt2 << "size: " << opt2->size() << '\n';
				}
				{
					mse::xscope_optional<int> opt = {};

					MSE_TRY {
						int n = opt.value();
					}
					MSE_CATCH (const std::exception& e) {
#if __cpp_exceptions >= 199711
						std::cout << e.what() << '\n';
#endif // __cpp_exceptions >= 199711
					}

					mse::xscope_optional<int> opt2 = opt;
					mse::xscope_optional<int> opt3 = std::move(opt);
				}

#endif // MSE_SELF_TESTS
			}

#ifdef __clang__
#pragma clang diagnostic pop
#else /*__clang__*/
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif /*__GNUC__*/
#endif /*__clang__*/
		};
	}
} // namespace mse

#ifdef _MSC_VER
#pragma warning( pop )  
#endif /*_MSC_VER*/

#ifndef MSE_PUSH_MACRO_NOT_SUPPORTED
#pragma pop_macro("MSE_THROW")
#pragma pop_macro("_STD")
#pragma pop_macro("_NOEXCEPT")
#pragma pop_macro("_THROW_NCEE")
#pragma pop_macro("_NODISCARD")
#endif // !MSE_PUSH_MACRO_NOT_SUPPORTED

# endif //MSEOPTIONAL_H_
