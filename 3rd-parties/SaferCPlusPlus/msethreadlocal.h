
// Copyright (c) 2015 Noah Lopez
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef MSETHREADLOCAL_H_
#define MSETHREADLOCAL_H_

#include "msepointerbasics.h"
#include "mseprimitives.h"
#ifndef MSE_THREADLOCAL_NO_XSCOPE_DEPENDENCE
#include "msescope.h"
#endif // !MSE_THREADLOCAL_NO_XSCOPE_DEPENDENCE
#include <utility>
#include <unordered_set>
#include <functional>
#include <cassert>

/* moved to msepointerbasics.h */
//#if defined(MSE_SAFER_SUBSTITUTES_DISABLED) || defined(MSE_SAFERPTR_DISABLED)
//#define MSE_THREADLOCALPOINTER_DISABLED
//#endif /*defined(MSE_SAFER_SUBSTITUTES_DISABLED) || defined(MSE_SAFERPTR_DISABLED)*/

#include "msenorad.h"
#include "mseany.h"

#ifdef _MSC_VER
#pragma warning( push )  
#pragma warning( disable : 4100 4456 4189 )
#endif /*_MSC_VER*/

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wunused-value"
#else /*__clang__*/
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wunused-value"
#endif /*__GNUC__*/
#endif /*__clang__*/

/* Note that by default, MSE_THREADLOCALPOINTER_DISABLED is defined in non-debug builds. This is enacted in "msepointerbasics.h". */

#ifndef MSE_PUSH_MACRO_NOT_SUPPORTED
#pragma push_macro("_NOEXCEPT")
#endif // !MSE_PUSH_MACRO_NOT_SUPPORTED

#ifndef _NOEXCEPT
#define _NOEXCEPT
#endif /*_NOEXCEPT*/


namespace mse {
	namespace rsv {

		/* This macro roughly simulates constructor inheritance. */
#define MSE_THREADLOCAL_USING(Derived, Base) MSE_USING(Derived, Base)

		template<typename _Ty> class TThreadLocalID {};

#ifdef MSE_THREADLOCALPOINTER_DISABLED
		template<typename _Ty> using TThreadLocalPointer = _Ty * ;
		template<typename _Ty> using TThreadLocalConstPointer = const _Ty*;
		template<typename _Ty> using TThreadLocalNotNullPointer = _Ty * ;
		template<typename _Ty> using TThreadLocalNotNullConstPointer = const _Ty*;
		template<typename _Ty> using TThreadLocalFixedPointer = _Ty * /*const*/; /* Can't be const qualified because standard
																		   library containers don't support const elements. */
		template<typename _Ty> using TThreadLocalFixedConstPointer = const _Ty* /*const*/;
		template<typename _TROy> using TThreadLocalObj = _TROy;

		template<typename _Ty> auto thread_local_fptr_to(_Ty&& _X) { return std::addressof(_X); }
		template<typename _Ty> auto thread_local_fptr_to(const _Ty& _X) { return std::addressof(_X); }

#else /*MSE_THREADLOCALPOINTER_DISABLED*/

	}
	namespace us {
		namespace impl {
			namespace ns_thread_local {

				template<typename _Ty> class TThreadLocalID {};

				/* Objects of thread_local duration are still prone to being accessed after destruction, and therefore still
				need appropriate safety mechanisms. However, in the case where the object has a trivial destructor,
				we're dispensing with the safety mechanisms. For now, anyway. */

				template<typename _TROz>
				using use_unchecked_base_type = typename mse::impl::conjunction<std::is_trivially_destructible<_TROz>
#if defined(MSE_THREADLOCALPOINTER_RUNTIME_CHECKS_ENABLED)
					, std::false_type
#endif // MSE_THREADLOCALPOINTER_RUNTIME_CHECKS_ENABLED
				>::type;

				template<typename _TROz>
				using TThreadLocalObjBaseBase = mse::impl::conditional_t<use_unchecked_base_type<_TROz>::value
					, /*mse::impl::remove_const_t<_TROz>*/_TROz, mse::TNDNoradObj<_TROz>>;

				template<typename _Ty>
				using TThreadLocalConstPointerBaseBase = mse::impl::conditional_t<use_unchecked_base_type<_Ty>::value
					, mse::us::impl::TPointerForLegacy<const _Ty, TThreadLocalID<const _Ty>>, mse::TNDNoradConstPointer<_Ty>>;

				template<typename _Ty>
				using TThreadLocalPointerBaseBase = mse::impl::conditional_t<use_unchecked_base_type<_Ty>::value
					, mse::us::impl::TPointerForLegacy<_Ty, TThreadLocalID<_Ty>>, mse::TNDNoradPointer<_Ty>>;

				template<typename _TROz>
				class TThreadLocalObjBase : public TThreadLocalObjBaseBase<_TROz> {
				public:
					typedef TThreadLocalObjBaseBase<_TROz> base_class;
					MSE_THREADLOCAL_USING(TThreadLocalObjBase, base_class);
					TThreadLocalObjBase(const TThreadLocalObjBase& _X) : base_class(_X) {}
					TThreadLocalObjBase(TThreadLocalObjBase&& _X) : base_class(MSE_FWD(_X)) {}

					TThreadLocalObjBase& operator=(TThreadLocalObjBase&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); }
					TThreadLocalObjBase& operator=(const TThreadLocalObjBase& _X) { base_class::operator=(_X); return (*this); }
					template<class _Ty2>
					TThreadLocalObjBase& operator=(_Ty2&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); }
					template<class _Ty2>
					TThreadLocalObjBase& operator=(_Ty2& _X) { base_class::operator=(_X); return (*this); }

					auto operator&() { return operator_ampersand(use_unchecked_base_type<_TROz>()); }
					auto operator&() const { return operator_ampersand(use_unchecked_base_type<_TROz>()); }

				private:
					auto operator_ampersand(std::false_type) const { return base_class::operator&(); }
					auto operator_ampersand(std::true_type) const { return static_cast<const base_class*>(this); }
					auto operator_ampersand(std::false_type) { return base_class::operator&(); }
					auto operator_ampersand(std::true_type) { return static_cast<base_class*>(this); }
				};

				template<typename _Ty> class TThreadLocalConstPointerBase : public TThreadLocalConstPointerBaseBase<_Ty> {
				public:
					typedef TThreadLocalConstPointerBaseBase<_Ty> base_class;
					MSE_USING(TThreadLocalConstPointerBase, base_class);
					//TThreadLocalConstPointerBase(const TThreadLocalObjBase<_Ty>* src) : base_class(&(*src)) {}
				};

				template<typename _Ty> class TThreadLocalPointerBase : public TThreadLocalPointerBaseBase<_Ty> {
				public:
					typedef TThreadLocalPointerBaseBase<_Ty> base_class;
					MSE_USING(TThreadLocalPointerBase, base_class);
					//TThreadLocalPointerBase(const TThreadLocalObjBase<_Ty>* src) : base_class(&(*src)) {}
				};
			}
		}
	}
	namespace rsv {

		template <class _Ty, class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<
			(!std::is_same<_Ty&&, _Ty2>::value) || (!std::is_rvalue_reference<_Ty2>::value)
			> MSE_IMPL_EIS >
			static void valid_if_not_rvalue_reference_of_given_type_msethreadlocal(_Ty2 src) {}

		template<typename _Ty> class TThreadLocalObj;
		template<typename _Ty> class TThreadLocalNotNullPointer;
		template<typename _Ty> class TThreadLocalNotNullConstPointer;
		template<typename _Ty> class TThreadLocalFixedPointer;
		template<typename _Ty> class TThreadLocalFixedConstPointer;
	}

	namespace us {
		namespace impl {
			template <typename _Ty, typename _TConstPointer1> class TCommonizedPointer;
			template <typename _Ty, typename _TConstPointer1> class TCommonizedConstPointer;
		}
	}

	namespace rsv {

		/* Use TThreadLocalFixedPointer instead. */
		template<typename _Ty>
		class TThreadLocalPointer : public mse::us::impl::ns_thread_local::TThreadLocalPointerBase<_Ty>, public mse::us::impl::StrongPointerAsyncNotShareableAndNotPassableTagBase {
		public:
			typedef mse::us::impl::ns_thread_local::TThreadLocalPointerBase<_Ty> base_class;
			MSE_IMPL_DESTRUCTOR_PREFIX1 ~TThreadLocalPointer() {}
		private:
			TThreadLocalPointer() : base_class() {}
			TThreadLocalPointer(const base_class& ptr) : base_class(ptr) {}
			TThreadLocalPointer(const TThreadLocalPointer& src_cref) : base_class(
				static_cast<const base_class&>(src_cref)) {}
			template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
			TThreadLocalPointer(const TThreadLocalPointer<_Ty2>& src_cref) : base_class(mse::us::impl::ns_thread_local::TThreadLocalPointerBase<_Ty2>(src_cref)) {}
			/*
			TThreadLocalPointer<_Ty>& operator=(TThreadLocalObj<_Ty>* ptr) {
				base_class::operator=(ptr);
				return *this;
			}
			*/
			TThreadLocalPointer<_Ty>& operator=(const TThreadLocalPointer<_Ty>& _Right_cref) {
				base_class::operator=(_Right_cref);
				return *this;
			}
			operator bool() const {
				bool retval = (bool(*static_cast<const base_class*>(this)));
				return retval;
			}
			/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
			MSE_DEPRECATED explicit operator _Ty*() const {
				_Ty* retval = std::addressof(*(*this))/*(*static_cast<const base_class*>(this))*/;
				return retval;
			}
			MSE_DEPRECATED explicit operator TThreadLocalObj<_Ty>*() const {
				TThreadLocalObj<_Ty>* retval = (*static_cast<const base_class*>(this));
				return retval;
			}

			MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

			friend class TThreadLocalNotNullPointer<_Ty>;
			friend class mse::us::impl::TCommonizedPointer<_Ty, TThreadLocalPointer<_Ty> >;
			friend class mse::us::impl::TCommonizedConstPointer<const _Ty, TThreadLocalPointer<_Ty> >;
		};

		template<typename _Ty>
		bool operator==(const TThreadLocalPointer<_Ty>& lhs, const TThreadLocalPointer<_Ty>& rhs) {
			return std::addressof(*lhs) == std::addressof(*rhs);
		}
		template<typename _Ty>
		bool operator!=(const TThreadLocalPointer<_Ty>& lhs, const TThreadLocalPointer<_Ty>& rhs) {
			return !(lhs == rhs);
		}

		/* Use TThreadLocalFixedConstPointer instead. */
		template<typename _Ty>
		class TThreadLocalConstPointer : public mse::us::impl::ns_thread_local::TThreadLocalConstPointerBase<_Ty>, public mse::us::impl::StrongPointerAsyncNotShareableAndNotPassableTagBase {
		public:
			typedef mse::us::impl::ns_thread_local::TThreadLocalConstPointerBase<_Ty> base_class;
			MSE_IMPL_DESTRUCTOR_PREFIX1 ~TThreadLocalConstPointer() {}
		private:
			TThreadLocalConstPointer() : base_class() {}
			TThreadLocalConstPointer(const base_class& ptr) : base_class(ptr) {}
			TThreadLocalConstPointer(const TThreadLocalConstPointer& src_cref) : base_class(static_cast<const base_class&>(src_cref)) {}
			template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
			TThreadLocalConstPointer(const TThreadLocalConstPointer<_Ty2>& src_cref) : base_class(src_cref) {}
			TThreadLocalConstPointer(const TThreadLocalPointer<_Ty>& src_cref) : base_class(static_cast<const typename TThreadLocalPointer<_Ty>::base_class&>(src_cref)) {}
			template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
			TThreadLocalConstPointer(const TThreadLocalPointer<_Ty2>& src_cref) : base_class(mse::us::impl::ns_thread_local::TThreadLocalConstPointerBase<_Ty2>(src_cref)) {}
			/*
			TThreadLocalConstPointer<_Ty>& operator=(const TThreadLocalObj<_Ty>* ptr) {
				base_class::operator=(ptr);
				return *this;
			}
			*/
			TThreadLocalConstPointer<_Ty>& operator=(const TThreadLocalConstPointer<_Ty>& _Right_cref) {
				base_class::operator=(_Right_cref);
				return *this;
			}
			operator bool() const {
				bool retval = (bool(*static_cast<const base_class*>(this)));
				return retval;
			}
			/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
			MSE_DEPRECATED explicit operator const _Ty*() const {
				const _Ty* retval = (*static_cast<const base_class*>(this));
				return retval;
			}
			MSE_DEPRECATED explicit operator const TThreadLocalObj<_Ty>*() const {
				const TThreadLocalObj<_Ty>* retval = (*static_cast<const base_class*>(this));
				return retval;
			}

			MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

			friend class TThreadLocalNotNullConstPointer<_Ty>;
			friend class mse::us::impl::TCommonizedConstPointer<const _Ty, TThreadLocalConstPointer<_Ty> >;
		};

		template<typename _Ty>
		bool operator==(const TThreadLocalConstPointer<_Ty>& lhs, const TThreadLocalConstPointer<_Ty>& rhs) {
			return std::addressof(*lhs) == std::addressof(*rhs);
		}
		template<typename _Ty>
		bool operator!=(const TThreadLocalConstPointer<_Ty>& lhs, const TThreadLocalConstPointer<_Ty>& rhs) {
			return !(lhs == rhs);
		}
		template<typename _Ty>
		bool operator==(const TThreadLocalConstPointer<_Ty>& lhs, const TThreadLocalPointer<_Ty>& rhs) {
			return std::addressof(*lhs) == std::addressof(*rhs);
		}
		template<typename _Ty>
		bool operator!=(const TThreadLocalConstPointer<_Ty>& lhs, const TThreadLocalPointer<_Ty>& rhs) {
			return !(lhs == rhs);
		}
		template<typename _Ty>
		bool operator==(const TThreadLocalPointer<_Ty>& lhs, const TThreadLocalConstPointer<_Ty>& rhs) {
			return std::addressof(*lhs) == std::addressof(*rhs);
		}
		template<typename _Ty>
		bool operator!=(const TThreadLocalPointer<_Ty>& lhs, const TThreadLocalConstPointer<_Ty>& rhs) {
			return !(lhs == rhs);
		}

		/* Use TThreadLocalFixedPointer instead. */
		template<typename _Ty>
		class TThreadLocalNotNullPointer : public TThreadLocalPointer<_Ty>, public mse::us::impl::NeverNullTagBase {
		public:
			typedef TThreadLocalPointer<_Ty> base_class;
			MSE_IMPL_DESTRUCTOR_PREFIX1 ~TThreadLocalNotNullPointer() {}
			operator bool() const { return (*static_cast<const base_class*>(this)); }
#ifndef MSE_THREADLOCAL_NO_XSCOPE_DEPENDENCE
			operator mse::TXScopeFixedPointer<_Ty>() const {
				return mse::us::unsafe_make_xscope_pointer_to(*(*this));
			}
			/*
			explicit operator mse::TXScopeFixedConstPointer<_Ty>() const {
				return mse::us::unsafe_make_xscope_const_pointer_to(*(*this));
			}
			*/
#endif // !MSE_THREADLOCAL_NO_XSCOPE_DEPENDENCE
			void thread_local_tag() const {}

		private:
			//TThreadLocalNotNullPointer(typename base_class::scope_obj_base_ptr_t src_cref) : base_class(src_cref) {}
			TThreadLocalNotNullPointer(const typename base_class::base_class& ptr) : base_class(ptr) {}
			TThreadLocalNotNullPointer(TThreadLocalObj<_Ty>* ptr) : base_class(ptr) {}
			template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2*, _Ty*>::value> MSE_IMPL_EIS >
			TThreadLocalNotNullPointer(const TThreadLocalNotNullPointer<_Ty2>& src_cref) : base_class(src_cref) {}

			TThreadLocalNotNullPointer<_Ty>& operator=(const TThreadLocalNotNullPointer<_Ty>& _Right_cref) {
				base_class::operator=(_Right_cref);
				return (*this);
			}

			/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
#ifndef MSE_SCOPEPOINTER_DISABLED
			MSE_DEPRECATED explicit operator _Ty* () const { return base_class::operator _Ty * (); }
#endif // !MSE_SCOPEPOINTER_DISABLED
			MSE_DEPRECATED explicit operator TThreadLocalObj<_Ty>*() const { return base_class::operator TThreadLocalObj<_Ty>*(); }

			MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

			friend class TThreadLocalFixedPointer<_Ty>;
		};

		/* Use TThreadLocalFixedConstPointer instead. */
		template<typename _Ty>
		class TThreadLocalNotNullConstPointer : public TThreadLocalConstPointer<_Ty>, public mse::us::impl::NeverNullTagBase {
		public:
			typedef TThreadLocalConstPointer<_Ty> base_class;
			MSE_IMPL_DESTRUCTOR_PREFIX1 ~TThreadLocalNotNullConstPointer() {}
			operator bool() const { return (*static_cast<const base_class*>(this)); }
#ifndef MSE_THREADLOCAL_NO_XSCOPE_DEPENDENCE
			operator mse::TXScopeFixedConstPointer<_Ty>() const {
				return mse::us::unsafe_make_xscope_const_pointer_to(*(*this));
			}
#endif // !MSE_THREADLOCAL_NO_XSCOPE_DEPENDENCE
			void thread_local_tag() const {}

		private:
			TThreadLocalNotNullConstPointer(const TThreadLocalNotNullConstPointer<_Ty>& src_cref) : base_class(src_cref) {}
			template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2*, _Ty*>::value> MSE_IMPL_EIS >
			TThreadLocalNotNullConstPointer(const TThreadLocalNotNullConstPointer<_Ty2>& src_cref) : base_class(src_cref) {}
			TThreadLocalNotNullConstPointer(const TThreadLocalNotNullPointer<_Ty>& src_cref) : base_class(src_cref) {}
			template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2*, _Ty*>::value> MSE_IMPL_EIS >
			TThreadLocalNotNullConstPointer(const TThreadLocalNotNullPointer<_Ty2>& src_cref) : base_class(src_cref) {}

			TThreadLocalNotNullConstPointer<_Ty>& operator=(const TThreadLocalNotNullConstPointer<_Ty>& _Right_cref) {
				base_class::operator=(_Right_cref);
				return *this;
			}

			/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
#ifndef MSE_SCOPEPOINTER_DISABLED
			MSE_DEPRECATED explicit operator const _Ty* () const { return base_class::operator const _Ty * (); }
#endif // !MSE_SCOPEPOINTER_DISABLED
			MSE_DEPRECATED explicit operator const TThreadLocalObj<_Ty>* () const { return base_class::operator const TThreadLocalObj<_Ty> * (); }
			//TThreadLocalNotNullConstPointer(typename base_class::scope_obj_base_const_ptr_t ptr) : base_class(ptr) {}
			TThreadLocalNotNullConstPointer(const typename base_class::base_class& ptr) : base_class(ptr) {}

			MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

			friend class TThreadLocalFixedConstPointer<_Ty>;
		};

		template<typename _Ty>
		class TThreadLocalFixedPointer : public TThreadLocalNotNullPointer<_Ty> {
		public:
			typedef TThreadLocalNotNullPointer<_Ty> base_class;
			TThreadLocalFixedPointer(const TThreadLocalFixedPointer& src_cref) : base_class(src_cref) {}
			template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
			TThreadLocalFixedPointer(const TThreadLocalFixedPointer<_Ty2>& src_cref) : base_class(src_cref) {}
			MSE_IMPL_DESTRUCTOR_PREFIX1 ~TThreadLocalFixedPointer() {}
			operator bool() const { return (*static_cast<const base_class*>(this)); }
			/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
#ifndef MSE_SCOPEPOINTER_DISABLED
			MSE_DEPRECATED explicit operator _Ty* () const { return base_class::operator _Ty * (); }
#endif // !MSE_SCOPEPOINTER_DISABLED
			MSE_DEPRECATED explicit operator TThreadLocalObj<_Ty>*() const { return base_class::operator TThreadLocalObj<_Ty>*(); }
			void thread_local_tag() const {}

		private:
			//TThreadLocalFixedPointer(typename TThreadLocalPointer<_Ty>::scope_obj_base_ptr_t ptr) : base_class(ptr) {}
			TThreadLocalFixedPointer(const typename TThreadLocalPointer<_Ty>::base_class& ptr) : base_class(ptr) {}
#ifdef MSE_THREADLOCAL_DISABLE_MOVE_RESTRICTIONS
			TThreadLocalFixedPointer(TThreadLocalFixedPointer&& src_ref) : base_class(src_ref) {
				int q = 5;
		}
#endif // !MSE_THREADLOCAL_DISABLE_MOVE_RESTRICTIONS
			TThreadLocalFixedPointer<_Ty>& operator=(const TThreadLocalFixedPointer<_Ty>& _Right_cref) = delete;
			MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

			friend class TThreadLocalObj<_Ty>;
		};

		template<typename _Ty>
		class TThreadLocalFixedConstPointer : public TThreadLocalNotNullConstPointer<_Ty> {
		public:
			typedef TThreadLocalNotNullConstPointer<_Ty> base_class;
			TThreadLocalFixedConstPointer(const TThreadLocalFixedConstPointer<_Ty>& src_cref) : base_class(src_cref) {}
			template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
			TThreadLocalFixedConstPointer(const TThreadLocalFixedConstPointer<_Ty2>& src_cref) : base_class(src_cref) {}
			TThreadLocalFixedConstPointer(const TThreadLocalFixedPointer<_Ty>& src_cref) : base_class(src_cref) {}
			template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
			TThreadLocalFixedConstPointer(const TThreadLocalFixedPointer<_Ty2>& src_cref) : base_class(src_cref) {}
			MSE_IMPL_DESTRUCTOR_PREFIX1 ~TThreadLocalFixedConstPointer() {}
			operator bool() const { return (*static_cast<const base_class*>(this)); }
			/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
#ifndef MSE_SCOPEPOINTER_DISABLED
			MSE_DEPRECATED explicit operator const _Ty* () const { return base_class::operator const _Ty * (); }
#endif // !MSE_SCOPEPOINTER_DISABLED
			MSE_DEPRECATED explicit operator const TThreadLocalObj<_Ty>*() const { return base_class::operator const TThreadLocalObj<_Ty>*(); }
			void thread_local_tag() const {}

		private:
			//TThreadLocalFixedConstPointer(typename TThreadLocalConstPointer<_Ty>::scope_obj_base_const_ptr_t ptr) : base_class(ptr) {}
			TThreadLocalFixedConstPointer(const typename TThreadLocalConstPointer<_Ty>::base_class& ptr) : base_class(ptr) {}
			TThreadLocalFixedConstPointer<_Ty>& operator=(const TThreadLocalFixedConstPointer<_Ty>& _Right_cref) = delete;
			MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

			friend class TThreadLocalObj<_Ty>;
		};

		template<typename _TROy>
		class TThreadLocalObj : public mse::us::impl::ns_thread_local::TThreadLocalObjBase<_TROy> {
		public:
			typedef mse::us::impl::ns_thread_local::TThreadLocalObjBase<_TROy> base_class;
			TThreadLocalObj(const TThreadLocalObj& _X) : base_class(_X) {}

#ifdef MSE_THREADLOCAL_DISABLE_MOVE_RESTRICTIONS
			explicit TThreadLocalObj(TThreadLocalObj&& _X) : base_class(MSE_FWD(_X)) {}
#endif // !MSE_THREADLOCAL_DISABLE_MOVE_RESTRICTIONS

			MSE_THREADLOCAL_USING(TThreadLocalObj, base_class);
			MSE_IMPL_DESTRUCTOR_PREFIX1 ~TThreadLocalObj() {
				valid_if_TROy_is_not_const_qualified<_TROy>();
			}

			TThreadLocalObj& operator=(TThreadLocalObj&& _X) {
				valid_if_not_rvalue_reference_of_given_type_msethreadlocal<TThreadLocalObj, decltype(_X)>(_X);
				base_class::operator=(MSE_FWD(_X));
				return (*this);
			}
			TThreadLocalObj& operator=(const TThreadLocalObj& _X) { base_class::operator=(_X); return (*this); }
			template<class _Ty2>
			TThreadLocalObj& operator=(_Ty2&& _X) {
				base_class::operator=(MSE_FWD(_X));
				return (*this);
			}
			template<class _Ty2>
			TThreadLocalObj& operator=(const _Ty2& _X) { base_class::operator=(_X); return (*this); }

			TThreadLocalFixedPointer<_TROy> operator&() & {
				return typename TThreadLocalPointer<_TROy>::base_class(&(*static_cast<base_class*>(this)));
			}
			TThreadLocalFixedConstPointer<_TROy> operator&() const & {
				return typename TThreadLocalConstPointer<_TROy>::base_class(&(*static_cast<const base_class*>(this)));
			}
			TThreadLocalFixedPointer<_TROy> mse_thread_local_fptr() & { return &(*this); }
			TThreadLocalFixedConstPointer<_TROy> mse_thread_local_fptr() const & { return &(*this); }

			void operator&() && = delete;
			void operator&() const && = delete;
			void mse_thread_local_fptr() && = delete;
			void mse_thread_local_fptr() const && = delete;

			void thread_local_tag() const {}

		private:
			void* operator new(size_t size) { return ::operator new(size); }

			template<class _Ty2 = _TROy, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _TROy>::value)
				&& ((!std::is_const<_TROy>::value) || (!std::is_trivially_destructible<_TROy>::value))> MSE_IMPL_EIS >
			void valid_if_TROy_is_not_const_qualified() const {}
		};

		template<typename _Ty>
		auto thread_local_fptr_to(_Ty&& _X) {
			return _X.mse_thread_local_fptr();
		}
		template<typename _Ty>
		auto thread_local_fptr_to(const _Ty& _X) {
			return _X.mse_thread_local_fptr();
		}
	}
}

namespace std {
	template<class _Ty>
	struct hash<mse::rsv::TThreadLocalFixedPointer<_Ty> > {	// hash functor
		typedef mse::rsv::TThreadLocalFixedPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::rsv::TThreadLocalFixedPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};

	template<class _Ty>
	struct hash<mse::rsv::TThreadLocalFixedConstPointer<_Ty> > {	// hash functor
		typedef mse::rsv::TThreadLocalFixedConstPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::rsv::TThreadLocalFixedConstPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
}

namespace mse {
	namespace rsv {

		/* template specializations */

#ifndef MSE_THREADLOCAL_NO_XSCOPE_DEPENDENCE
#ifdef MSE_SCOPEPOINTER_DISABLED
		/* The safety of these reinterpret_cast<>s rely on the pointer target (i.e. the 'mapped_type') being safely
		"reinterpretable" as a 'specified_type'. */
#define MSE_THREADLOCAL_IMPL_PTR_OPERATOR_TXSCOPEFIXEDPOINTER_RETVAL(specified_type, mapped_type) \
			reinterpret_cast<specified_type*>(std::addressof(*(*this)))
#define MSE_THREADLOCAL_IMPL_PTR_OPERATOR_TXSCOPEFIXEDCONSTPOINTER_RETVAL(specified_type, mapped_type) \
			reinterpret_cast<const specified_type*>(std::addressof(*(*this)))
#else // MSE_SCOPEPOINTER_DISABLED
#define MSE_THREADLOCAL_IMPL_PTR_OPERATOR_TXSCOPEFIXEDPOINTER_RETVAL(specified_type, mapped_type) \
			mse::TXScopeFixedPointer<mapped_type>(*static_cast<const base_class *>(this))
#define MSE_THREADLOCAL_IMPL_PTR_OPERATOR_TXSCOPEFIXEDCONSTPOINTER_RETVAL(specified_type, mapped_type) \
			mse::TXScopeFixedConstPointer<mapped_type>(*static_cast<const base_class *>(this))
#endif // MSE_SCOPEPOINTER_DISABLED

#define MSE_THREADLOCAL_IMPL_PTR_OPERATOR_TXSCOPEFIXEDPOINTER(specified_type, mapped_type) \
		operator mse::TXScopeFixedPointer<specified_type>() const { \
			return MSE_THREADLOCAL_IMPL_PTR_OPERATOR_TXSCOPEFIXEDPOINTER_RETVAL(specified_type, mapped_type); \
		}
#define MSE_THREADLOCAL_IMPL_PTR_OPERATOR_TXSCOPEFIXEDCONSTPOINTER(specified_type, mapped_type) \
		operator mse::TXScopeFixedConstPointer<specified_type>() const { \
			return MSE_THREADLOCAL_IMPL_PTR_OPERATOR_TXSCOPEFIXEDCONSTPOINTER_RETVAL(specified_type, mapped_type); \
		}
#else // !MSE_THREADLOCAL_NO_XSCOPE_DEPENDENCE
#define MSE_THREADLOCAL_IMPL_PTR_OPERATOR_TXSCOPEFIXEDPOINTER(specified_type, mapped_type)
#define MSE_THREADLOCAL_IMPL_PTR_OPERATOR_TXSCOPEFIXEDCONSTPOINTER(specified_type, mapped_type) 
#endif // !MSE_THREADLOCAL_NO_XSCOPE_DEPENDENCE

#define MSE_THREADLOCAL_IMPL_OBJ_INHERIT_OPERATOR_AMPERSAND \
		auto operator&() & { return base_class::operator&(); } \
		auto operator&() const & { return base_class::operator&(); } \
		void operator&() && = delete; \
		void operator&() const&& = delete;

#define MSE_THREADLOCAL_IMPL_OBJ_INHERIT_ASSIGNMENT_OPERATOR(class_name) \
		/*auto& operator=(class_name&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); }*/ \
		/*auto& operator=(const class_name& _X) { base_class::operator=(_X); return (*this); }*/ \
		template<class _Ty2> auto& operator=(_Ty2&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); } \
		template<class _Ty2> auto& operator=(const _Ty2& _X) { base_class::operator=(_X); return (*this); }

#define MSE_THREADLOCAL_IMPL_OBJ_SPECIALIZATION_DEFINITIONS1(class_name) \
		class_name(const class_name&) = default; \
		class_name(class_name&&) = default; \
		MSE_THREADLOCAL_IMPL_OBJ_INHERIT_ASSIGNMENT_OPERATOR(class_name); \
		MSE_THREADLOCAL_IMPL_OBJ_INHERIT_OPERATOR_AMPERSAND;

#if !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)
#define MSE_THREADLOCAL_IMPL_OBJ_NATIVE_POINTER_PRIVATE_CONSTRUCTORS1(class_name) \
			class_name(std::nullptr_t) {} \
			class_name() {}
#else // !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)
#define MSE_THREADLOCAL_IMPL_OBJ_NATIVE_POINTER_PRIVATE_CONSTRUCTORS1(class_name)
#endif // !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)

	/* Note that because we explicitly define some (private) constructors, default copy and move constructors
	and assignment operators won't be generated, so we have to define those as well. */
	/* Note that the 'mapped_type' must be safely "reinterpret_cast<>able" as a 'specified_type'. */
#define MSE_THREADLOCAL_IMPL_OBJ_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type) \
		template<typename _Ty> \
		class TThreadLocalObj<specified_type> : public TThreadLocalObj<mapped_type> { \
		public: \
			typedef TThreadLocalObj<mapped_type> base_class; \
			MSE_USING(TThreadLocalObj, base_class); \
			MSE_THREADLOCAL_IMPL_OBJ_SPECIALIZATION_DEFINITIONS1(TThreadLocalObj); \
		private: \
			MSE_THREADLOCAL_IMPL_OBJ_NATIVE_POINTER_PRIVATE_CONSTRUCTORS1(TThreadLocalObj); \
		};

#define MSE_THREADLOCAL_IMPL_PTR_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type) \
		template<typename _Ty> \
		class TThreadLocalFixedPointer<specified_type> : public TThreadLocalFixedPointer<mapped_type> { \
		public: \
			typedef TThreadLocalFixedPointer<mapped_type> base_class; \
			MSE_USING(TThreadLocalFixedPointer, base_class); \
			MSE_THREADLOCAL_IMPL_PTR_OPERATOR_TXSCOPEFIXEDPOINTER(specified_type, mapped_type); \
			/*explicit MSE_THREADLOCAL_IMPL_PTR_OPERATOR_TXSCOPEFIXEDCONSTPOINTER(specified_type, mapped_type);*/ \
		}; \
		template<typename _Ty> \
		class TThreadLocalFixedConstPointer<specified_type> : public TThreadLocalFixedConstPointer<mapped_type> { \
		public: \
			typedef TThreadLocalFixedConstPointer<mapped_type> base_class; \
			MSE_USING(TThreadLocalFixedConstPointer, base_class); \
			MSE_THREADLOCAL_IMPL_PTR_OPERATOR_TXSCOPEFIXEDCONSTPOINTER(specified_type, mapped_type); \
		};

#define MSE_THREADLOCAL_IMPL_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type) \
		MSE_THREADLOCAL_IMPL_PTR_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type); \
		MSE_THREADLOCAL_IMPL_OBJ_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type);

		MSE_THREADLOCAL_IMPL_NATIVE_POINTER_SPECIALIZATION(_Ty*, mse::us::impl::TPointerForLegacy<_Ty>);
		MSE_THREADLOCAL_IMPL_NATIVE_POINTER_SPECIALIZATION(_Ty* const, const mse::us::impl::TPointerForLegacy<_Ty>);

#ifdef MSEPRIMITIVES_H

#define MSE_THREADLOCAL_IMPL_OBJ_INTEGRAL_SPECIALIZATION(integral_type) \
		template<> \
		class TThreadLocalObj<integral_type> : public TThreadLocalObj<mse::TInt<integral_type>> { \
		public: \
			typedef TThreadLocalObj<mse::TInt<integral_type>> base_class; \
			MSE_USING(TThreadLocalObj, base_class); \
			MSE_THREADLOCAL_IMPL_OBJ_SPECIALIZATION_DEFINITIONS1(TThreadLocalObj); \
		};

		/* Note that here we're relying on the fact that mse::TInt<integral_type> happens to be safely
		"reinterpret_cast<>able" as an 'integral_type'. */
#define MSE_THREADLOCAL_IMPL_PTR_INTEGRAL_SPECIALIZATION(integral_type) \
		template<> \
		class TThreadLocalFixedPointer<integral_type> : public TThreadLocalFixedPointer<mse::TInt<integral_type>> { \
		public: \
			typedef TThreadLocalFixedPointer<mse::TInt<integral_type>> base_class; \
			MSE_USING(TThreadLocalFixedPointer, base_class); \
			MSE_THREADLOCAL_IMPL_PTR_OPERATOR_TXSCOPEFIXEDPOINTER(integral_type, mse::TInt<integral_type>); \
		}; \
		template<> \
		class TThreadLocalFixedConstPointer<integral_type> : public TThreadLocalFixedConstPointer<mse::TInt<integral_type>> { \
		public: \
			typedef TThreadLocalFixedConstPointer<mse::TInt<integral_type>> base_class; \
			MSE_USING(TThreadLocalFixedConstPointer, base_class); \
			MSE_THREADLOCAL_IMPL_PTR_OPERATOR_TXSCOPEFIXEDCONSTPOINTER(integral_type, mse::TInt<integral_type>); \
		};

#define MSE_THREADLOCAL_IMPL_INTEGRAL_SPECIALIZATION(integral_type) \
		MSE_THREADLOCAL_IMPL_PTR_INTEGRAL_SPECIALIZATION(integral_type); \
		MSE_THREADLOCAL_IMPL_OBJ_INTEGRAL_SPECIALIZATION(integral_type); \
		MSE_THREADLOCAL_IMPL_PTR_INTEGRAL_SPECIALIZATION(typename std::add_const<integral_type>::type); \
		MSE_THREADLOCAL_IMPL_OBJ_INTEGRAL_SPECIALIZATION(typename std::add_const<integral_type>::type);

		MSE_IMPL_APPLY_MACRO_FUNCTION_TO_EACH_OF_THE_INTEGER_TYPES(MSE_THREADLOCAL_IMPL_INTEGRAL_SPECIALIZATION)

#endif /*MSEPRIMITIVES_H*/

		/* end of template specializations */

#endif /*MSE_THREADLOCALPOINTER_DISABLED*/
	}

#define MSE_DECLARE_THREAD_LOCAL(type) thread_local mse::rsv::TThreadLocalObj<type> 
#define MSE_DECLARE_THREAD_LOCAL_CONST(type) thread_local const mse::rsv::TThreadLocalObj<type> 
#define MSE_DECLARE_THREAD_LOCAL_GLOBAL(type) MSE_DECLARE_THREAD_LOCAL(type) 
#define MSE_DECLARE_THREAD_LOCAL_GLOBAL_CONST(type) MSE_DECLARE_THREAD_LOCAL_CONST(type) 


#ifndef MSE_THREADLOCALPOINTER_DISABLED
#ifndef MSE_THREADLOCAL_NO_XSCOPE_DEPENDENCE
	namespace impl {
		namespace ns_thread_local {
			template <typename _Ty>
			struct UnderlyingTemplateParam {
				typedef void type;
			};
			template <typename _Ty>
			struct UnderlyingTemplateParam<mse::rsv::TThreadLocalFixedConstPointer<_Ty> > {
				typedef typename UnderlyingTemplateParam<typename mse::rsv::TThreadLocalFixedConstPointer<_Ty>::base_class>::type helper_type1;
				typedef mse::impl::conditional_t<std::is_same<void, helper_type1>::value, _Ty, helper_type1> type;
			};
			template <typename _Ty>
			struct UnderlyingTemplateParam<mse::rsv::TThreadLocalFixedPointer<_Ty> > {
				typedef typename UnderlyingTemplateParam<typename mse::rsv::TThreadLocalFixedPointer<_Ty>::base_class>::type helper_type1;
				typedef mse::impl::conditional_t<std::is_same<void, helper_type1>::value, _Ty, helper_type1> type;
			};
		}
	}
	template <typename _Ty>
	auto xscope_pointer(const rsv::TThreadLocalFixedPointer<_Ty>& ptr) {
		typedef typename impl::ns_thread_local::UnderlyingTemplateParam<rsv::TThreadLocalFixedPointer<_Ty> >::type type1;
		type1* raw_ptr2 = std::addressof(*ptr);
		return mse::us::unsafe_make_xscope_pointer_to(*raw_ptr2);
	}
	template <typename _Ty>
	auto xscope_pointer(const rsv::TThreadLocalFixedConstPointer<_Ty>& ptr) {
		typedef typename impl::ns_thread_local::UnderlyingTemplateParam<rsv::TThreadLocalFixedPointer<_Ty> >::type type1;
		const type1* raw_ptr2 = std::addressof(*ptr);
		return mse::us::unsafe_make_xscope_const_pointer_to(*raw_ptr2);
	}
	template <typename _Ty>
	auto xscope_const_pointer(const rsv::TThreadLocalFixedPointer<_Ty>& ptr) {
		typedef typename impl::ns_thread_local::UnderlyingTemplateParam<rsv::TThreadLocalFixedPointer<_Ty> >::type type1;
		const type1* raw_ptr2 = std::addressof(*ptr);
		return mse::us::unsafe_make_xscope_const_pointer_to(*raw_ptr2);
	}
	template <typename _Ty>
	auto xscope_const_pointer(const rsv::TThreadLocalFixedConstPointer<_Ty>& ptr) {
		return mse::us::unsafe_make_xscope_const_pointer_to(*ptr);
	}
#endif // !MSE_THREADLOCAL_NO_XSCOPE_DEPENDENCE
#endif // !MSE_THREADLOCALPOINTER_DISABLED



	namespace self_test {
		class CThreadLocalPtrTest1 {
		public:
			static void s_test1() {
#ifdef MSE_SELF_TESTS
				class A {
				public:
					A(int x) : b(x) {}
					A(const A& _X) : b(_X.b) {}
					A(A&& _X) : b(MSE_FWD(_X).b) {}
					virtual ~A() {}
					A& operator=(A&& _X) { b = MSE_FWD(_X).b; return (*this); }
					A& operator=(const A& _X) { b = _X.b; return (*this); }

					int b = 3;
				};
				class B {
				public:
					static int foo1(A* a_native_ptr) { return a_native_ptr->b; }
					static int foo2(mse::rsv::TThreadLocalFixedPointer<A> A_thread_local_ptr) { return A_thread_local_ptr->b; }
				protected:
					~B() {}
				};

				A* A_native_ptr = nullptr;

				{
					A a(7);
					MSE_DECLARE_THREAD_LOCAL(A) thread_local_a(7);

					assert(a.b == thread_local_a.b);
					A_native_ptr = &a;

					mse::rsv::TThreadLocalFixedPointer<A> A_thread_local_ptr1(&thread_local_a);
					assert(A_native_ptr->b == A_thread_local_ptr1->b);
					mse::rsv::TThreadLocalFixedPointer<A> A_thread_local_ptr2 = &thread_local_a;

					if (!A_thread_local_ptr2) {
						assert(false);
					}
					else if (!(A_thread_local_ptr2 != A_thread_local_ptr1)) {
						int q = B::foo2(A_thread_local_ptr2);
					}
					else {
						assert(false);
					}

					A a2 = a;
					MSE_DECLARE_THREAD_LOCAL(A) thread_local_a2 = thread_local_a;
					thread_local_a2 = a;
					thread_local_a2 = thread_local_a;

					mse::rsv::TThreadLocalFixedConstPointer<A> rcp = A_thread_local_ptr1;
					mse::rsv::TThreadLocalFixedConstPointer<A> rcp2 = rcp;
					MSE_DECLARE_THREAD_LOCAL_CONST(A) cthread_local_a(11);
					mse::rsv::TThreadLocalFixedConstPointer<A> rfcp = &cthread_local_a;
				}

				{
					class A {
					public:
						A(int x) : b(x) {}
						virtual ~A() {}

						int b = 3;
						std::string s = "some text ";
					};
					class B {
					public:
						static int foo1(A* a_native_ptr) { return a_native_ptr->b; }
						static int foo2(mse::rsv::TThreadLocalFixedPointer<A> A_scpfptr) { return A_scpfptr->b; }
						static int foo3(mse::rsv::TThreadLocalFixedConstPointer<A> A_scpfcptr) { return A_scpfcptr->b; }
					protected:
						~B() {}
					};

					MSE_DECLARE_THREAD_LOCAL(A) a_scpobj(5);
					int res1 = (&a_scpobj)->b;
					int res2 = B::foo2(&a_scpobj);
					int res3 = B::foo3(&a_scpobj);

					/* You can use the "mse::make_pointer_to_member_v2()" function to obtain a safe pointer to a member of
					an thread_local object. */
					auto s_safe_ptr1 = mse::make_pointer_to_member_v2((&a_scpobj), &A::s);
					(*s_safe_ptr1) = "some new text";
					auto s_safe_const_ptr1 = mse::make_const_pointer_to_member_v2((&a_scpobj), &A::s);
				}

				{
					A a(7);
					MSE_DECLARE_THREAD_LOCAL(A) thread_local_a(7);

					assert(a.b == thread_local_a.b);
					A_native_ptr = &a;

					mse::rsv::TThreadLocalFixedPointer<A> A_thread_local_ptr1 = &thread_local_a;
					assert(A_native_ptr->b == A_thread_local_ptr1->b);
					mse::rsv::TThreadLocalFixedPointer<A> A_thread_local_ptr2 = &thread_local_a;

					if (!A_thread_local_ptr2) {
						assert(false);
					}
					else if (!(A_thread_local_ptr2 != A_thread_local_ptr1)) {
						int q = B::foo2(A_thread_local_ptr2);
					}
					else {
						assert(false);
					}

					A a2 = a;
					MSE_DECLARE_THREAD_LOCAL(A) thread_local_a2 = thread_local_a;
					thread_local_a2 = a;
					thread_local_a2 = thread_local_a;

					mse::rsv::TThreadLocalFixedConstPointer<A> rcp = A_thread_local_ptr1;
					mse::rsv::TThreadLocalFixedConstPointer<A> rcp2 = rcp;

					MSE_DECLARE_THREAD_LOCAL_CONST(A) cthread_local_a(11);
					mse::rsv::TThreadLocalFixedConstPointer<A> rfcp = &cthread_local_a;

#ifndef MSE_THREADLOCAL_NO_XSCOPE_DEPENDENCE
					mse::TXScopeFixedPointer<A> xsptr = A_thread_local_ptr1;
					mse::TXScopeFixedPointer<A> xsptr2 = mse::xscope_pointer(A_thread_local_ptr1);
					mse::TXScopeFixedConstPointer<A> xscptr2 = mse::xscope_const_pointer(A_thread_local_ptr1);
#endif // !MSE_THREADLOCAL_NO_XSCOPE_DEPENDENCE
				}

				{
					int a(7);
					MSE_DECLARE_THREAD_LOCAL(int) thread_local_a(7);

					auto int_native_ptr = &a;

					mse::rsv::TThreadLocalFixedPointer<int> int_thread_local_ptr1 = &thread_local_a;
					mse::rsv::TThreadLocalFixedPointer<int> int_thread_local_ptr2 = &thread_local_a;

					if (!int_thread_local_ptr2) {
						assert(false);
					}
					else if (!(int_thread_local_ptr2 != int_thread_local_ptr1)) {
						int q = 5;
					}
					else {
						assert(false);
					}

					int a2 = a;
					MSE_DECLARE_THREAD_LOCAL(int) thread_local_a2 = thread_local_a;
					thread_local_a2 = a;
					thread_local_a2 = thread_local_a;

					mse::rsv::TThreadLocalFixedConstPointer<int> rcp = int_thread_local_ptr1;
					mse::rsv::TThreadLocalFixedConstPointer<int> rcp2 = rcp;

#ifndef MSE_THREADLOCAL_NO_XSCOPE_DEPENDENCE
					mse::TXScopeFixedPointer<int> xsptr = int_thread_local_ptr1;
					/*mse::TXScopeFixedPointer<int>*/ auto xsptr2 = mse::xscope_pointer(int_thread_local_ptr1);
					/*mse::TXScopeFixedConstPointer<int>*/ auto xscptr2 = mse::xscope_const_pointer(int_thread_local_ptr1);
#endif // !MSE_THREADLOCAL_NO_XSCOPE_DEPENDENCE
				}
				{
					MSE_DECLARE_THREAD_LOCAL_CONST(int) cthread_local_a(11);
					mse::rsv::TThreadLocalFixedConstPointer<int> rfcp = &cthread_local_a;
				}
				{
					typedef /*const */std::array<int, 3> type1;
					//typedef const std::string type1;
					MSE_DECLARE_THREAD_LOCAL(type1) cthread_local_a;
					mse::rsv::TThreadLocalFixedConstPointer<type1> rfcp = &cthread_local_a;
				}

#endif // MSE_SELF_TESTS
			}
		};
	}
}

#ifndef MSE_PUSH_MACRO_NOT_SUPPORTED
#pragma pop_macro("_NOEXCEPT")
#endif // !MSE_PUSH_MACRO_NOT_SUPPORTED

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

#endif // MSETHREADLOCAL_H_
