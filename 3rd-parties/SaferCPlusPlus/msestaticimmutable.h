
// Copyright (c) 2015 Noah Lopez
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef MSESTATIC_H_
#define MSESTATIC_H_

#include "msepointerbasics.h"
#include "mseprimitives.h"
#include "msemsearray.h"
#ifndef MSE_STATICIMMUTABLE_NO_XSCOPE_DEPENDENCE
#include "msescope.h"
#endif // !MSE_STATICIMMUTABLE_NO_XSCOPE_DEPENDENCE
#include <utility>
#include <unordered_set>
#include <functional>
#include <cassert>

/* moved to msepointerbasics.h */
//#if defined(MSE_SAFER_SUBSTITUTES_DISABLED) || defined(MSE_SAFERPTR_DISABLED)
//#define MSE_STATICPOINTER_DISABLED
//#endif /*defined(MSE_SAFER_SUBSTITUTES_DISABLED) || defined(MSE_SAFERPTR_DISABLED)*/

#ifndef MSE_CHECKED_THREAD_SAFE_DO_NOT_USE_GNORAD
#include "msenorad.h"
#endif // !MSE_CHECKED_THREAD_SAFE_DO_NOT_USE_GNORAD
#include <atomic>

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

#ifndef MSE_PUSH_MACRO_NOT_SUPPORTED
#pragma push_macro("_NOEXCEPT")
#pragma push_macro("MSE_THROW")
#endif // !MSE_PUSH_MACRO_NOT_SUPPORTED

#ifndef _NOEXCEPT
#define _NOEXCEPT
#endif /*_NOEXCEPT*/

#ifdef MSE_CUSTOM_THROW_DEFINITION
#define MSE_THROW(x) MSE_CUSTOM_THROW_DEFINITION(x)
#else // MSE_CUSTOM_THROW_DEFINITION
#define MSE_THROW(x) throw(x)
#endif // MSE_CUSTOM_THROW_DEFINITION


namespace mse {
	namespace rsv {

		/* This macro roughly simulates constructor inheritance. */
#define MSE_STATIC_USING(Derived, Base) MSE_USING(Derived, Base)

		namespace impl {
			namespace cts {
				class CNoOpCopyAtomicInt : public std::atomic<int> {
				public:
					typedef std::atomic<int> base_class;
					CNoOpCopyAtomicInt(int i) : base_class(i) {}
					CNoOpCopyAtomicInt(const CNoOpCopyAtomicInt&) : base_class(0) {}
				};
				typedef CNoOpCopyAtomicInt atomic_int_t;

#ifndef MSE_CHECKED_THREAD_SAFE_DO_NOT_USE_GNORAD
				template<typename _Ty> using TCheckedThreadSafeObj = mse::us::impl::TGNoradObj<_Ty, atomic_int_t>;
				template<typename _Ty> using TCheckedThreadSafePointer = mse::us::impl::TGNoradPointer<_Ty, atomic_int_t>;
				template<typename _Ty> using TCheckedThreadSafeConstPointer = mse::us::impl::TGNoradConstPointer<_Ty, atomic_int_t>;
				template<typename _Ty> using TCheckedThreadSafeFixedPointer = mse::us::impl::TGNoradFixedPointer<_Ty, atomic_int_t>;
				template<typename _Ty> using TCheckedThreadSafeFixedConstPointer = mse::us::impl::TGNoradFixedConstPointer<_Ty, atomic_int_t>;
#else // !MSE_CHECKED_THREAD_SAFE_DO_NOT_USE_GNORAD
				/* TCheckedThreadSafePointer<> is essentially just a simplified TNoradPointer<> with an atomic refcounter. */
				template<typename _Ty> class TCheckedThreadSafeObj;
				template<typename _Ty> class TCheckedThreadSafePointer;
				template<typename _Ty> class TCheckedThreadSafeConstPointer;
				template<typename _Ty> using TCheckedThreadSafeFixedPointer = TCheckedThreadSafePointer<_Ty>;
				template<typename _Ty> using TCheckedThreadSafeFixedConstPointer = TCheckedThreadSafeConstPointer<_Ty>;

				template<typename _Ty>
				class TCheckedThreadSafePointer : public mse::us::impl::TPointer<TCheckedThreadSafeObj<_Ty> >, public mse::us::impl::StrongPointerTagBase {
				public:
					typedef mse::us::impl::TPointer<TCheckedThreadSafeObj<_Ty> > base_class;
					TCheckedThreadSafePointer(const TCheckedThreadSafePointer& src_cref) : base_class(src_cref.m_ptr) {
						if (*this) { (*(*this)).increment_refcount(); }
					}
					template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2*, _Ty*>::value> MSE_IMPL_EIS >
					TCheckedThreadSafePointer(const TCheckedThreadSafePointer<_Ty2>& src_cref) : base_class(src_cref.m_ptr) {
						if (*this) { (*(*this)).increment_refcount(); }
					}
					TCheckedThreadSafePointer(TCheckedThreadSafePointer&& src_ref) : base_class(MSE_FWD(src_ref).m_ptr) {
						src_ref.m_ptr = nullptr;
					}
					MSE_IMPL_DESTRUCTOR_PREFIX1 ~TCheckedThreadSafePointer() {
						if (*this) { (*(*this)).decrement_refcount(); }
					}
					TCheckedThreadSafePointer<_Ty>& operator=(const TCheckedThreadSafePointer<_Ty>& _Right_cref) {
						if (*this) { (*(*this)).decrement_refcount(); }
						base_class::operator=(_Right_cref);
						if (*this) { (*(*this)).increment_refcount(); }
						return (*this);
					}
					template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2*, _Ty*>::value> MSE_IMPL_EIS >
					TCheckedThreadSafePointer<_Ty>& operator=(const TCheckedThreadSafePointer<_Ty2>& _Right_cref) {
						return (*this).operator=(TCheckedThreadSafePointer(_Right_cref));
					}

					TCheckedThreadSafeObj<_Ty>& operator*() const {
						if (!((*this).m_ptr)) { MSE_THROW(primitives_null_dereference_error("attempt to dereference null pointer - mse::TTSNoradPointer")); }
						return *((*this).m_ptr);
					}
					TCheckedThreadSafeObj<_Ty>* operator->() const {
						if (!((*this).m_ptr)) { MSE_THROW(primitives_null_dereference_error("attempt to dereference null pointer - mse::TTSNoradPointer")); }
						return (*this).m_ptr;
					}

					operator bool() const { return !(!((*this).m_ptr)); }
					/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
					operator _Ty* () const {
						return std::addressof(*(*this));
					}

				private:
					TCheckedThreadSafePointer(TCheckedThreadSafeObj<_Ty>* ptr) : base_class(ptr) {
						assert(*this);
						(*(*this)).increment_refcount();
					}

					MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

					friend class TCheckedThreadSafeObj<_Ty>;
				};

				template<typename _Ty>
				class TCheckedThreadSafeConstPointer : public mse::us::impl::TPointer<const TCheckedThreadSafeObj<_Ty> >, public mse::us::impl::StrongPointerTagBase {
				public:
					typedef mse::us::impl::TPointer<const TCheckedThreadSafeObj<_Ty> > base_class;
					TCheckedThreadSafeConstPointer(const TCheckedThreadSafeConstPointer& src_cref) : base_class(src_cref.m_ptr) {
						if (*this) { (*(*this)).increment_refcount(); }
					}
					template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2*, _Ty*>::value> MSE_IMPL_EIS >
					TCheckedThreadSafeConstPointer(const TCheckedThreadSafeConstPointer<_Ty2>& src_cref) : base_class(src_cref.m_ptr) {
						if (*this) { (*(*this)).increment_refcount(); }
					}
					TCheckedThreadSafeConstPointer(TCheckedThreadSafeConstPointer&& src_ref) : base_class(MSE_FWD(src_ref).m_ptr) {
						src_ref.m_ptr = nullptr;
					}
					TCheckedThreadSafeConstPointer(const TCheckedThreadSafePointer<_Ty>& src_cref) : base_class(src_cref) {
						if (*this) { (*(*this)).increment_refcount(); }
					}
					MSE_IMPL_DESTRUCTOR_PREFIX1 ~TCheckedThreadSafeConstPointer() {
						if (*this) { (*(*this)).decrement_refcount(); }
					}
					TCheckedThreadSafeConstPointer<_Ty>& operator=(const TCheckedThreadSafeConstPointer<_Ty>& _Right_cref) {
						if (*this) { (*(*this)).decrement_refcount(); }
						base_class::operator=(_Right_cref);
						if (*this) { (*(*this)).increment_refcount(); }
						return (*this);
					}
					template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2*, _Ty*>::value> MSE_IMPL_EIS >
					TCheckedThreadSafeConstPointer<_Ty>& operator=(const TCheckedThreadSafeConstPointer<_Ty2>& _Right_cref) {
						return (*this).operator=(TCheckedThreadSafeConstPointer(_Right_cref));
					}

					const TCheckedThreadSafeObj<_Ty>& operator*() const {
						if (!((*this).m_ptr)) { MSE_THROW(primitives_null_dereference_error("attempt to dereference null pointer - mse::TTSNoradPointer")); }
						return *((*this).m_ptr);
					}
					const TCheckedThreadSafeObj<_Ty>* operator->() const {
						if (!((*this).m_ptr)) { MSE_THROW(primitives_null_dereference_error("attempt to dereference null pointer - mse::TTSNoradPointer")); }
						return (*this).m_ptr;
					}

					operator bool() const { return !(!((*this).m_ptr)); }
					/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
					MSE_DEPRECATED explicit operator const _Ty* () const {
						return std::addressof(*(*this));
					}

				private:
					TCheckedThreadSafeConstPointer(const TCheckedThreadSafeObj<_Ty>* ptr) : base_class(ptr) {
						assert(*this);
						(*(*this)).increment_refcount();
					}

					MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

					friend class TCheckedThreadSafeObj<_Ty>;
				};

				template<typename _TROFLy>
				class TCheckedThreadSafeObj : public _TROFLy
				{
				public:
					MSE_USING(TCheckedThreadSafeObj, _TROFLy);
					TCheckedThreadSafeObj(const TCheckedThreadSafeObj& _X) : _TROFLy(_X) {}
					TCheckedThreadSafeObj(TCheckedThreadSafeObj&& _X) : _TROFLy(MSE_FWD(_X)) {}
					MSE_IMPL_DESTRUCTOR_PREFIX1 ~TCheckedThreadSafeObj() {
						if (0 != m_atomic_counter.load()) {
							/* It would be unsafe to allow this object to be destroyed as there are outstanding references to this object. */
#ifdef MSE_CUSTOM_FATAL_ERROR_MESSAGE_HANDLER
							MSE_CUSTOM_FATAL_ERROR_MESSAGE_HANDLER("Fatal Error: mse::TCheckedThreadSafeObj<> destructed with outstanding references \n");
#endif // MSE_CUSTOM_FATAL_ERROR_MESSAGE_HANDLER
							assert(false); std::terminate();
						}
					}

					TCheckedThreadSafeObj& operator=(TCheckedThreadSafeObj&& _X) { _TROFLy::operator=(MSE_FWD(_X)); return (*this); }
					TCheckedThreadSafeObj& operator=(const TCheckedThreadSafeObj& _X) { _TROFLy::operator=(_X); return (*this); }
					template<class _Ty2>
					TCheckedThreadSafeObj& operator=(_Ty2&& _X) { _TROFLy::operator=(MSE_FWD(_X)); return (*this); }
					template<class _Ty2>
					TCheckedThreadSafeObj& operator=(const _Ty2& _X) { _TROFLy::operator=(_X); return (*this); }

					TCheckedThreadSafeFixedPointer<_TROFLy> operator&() {
						return TCheckedThreadSafeFixedPointer<_TROFLy>(this);
					}
					TCheckedThreadSafeFixedConstPointer<_TROFLy> operator&() const {
						return TCheckedThreadSafeFixedConstPointer<_TROFLy>(this);
					}

					/* todo: make these private */
					void increment_refcount() const { m_atomic_counter += 1; }
					void decrement_refcount() const { m_atomic_counter -= 1; }

				private:
					mutable CNoOpCopyAtomicInt m_atomic_counter = 0;
				};

				/* template specializations */

				template<>
				class TCheckedThreadSafeObj<int> : public TCheckedThreadSafeObj<mse::TInt<int> > {
					typedef TCheckedThreadSafeObj<mse::TInt<int> > base_class;
					MSE_USING(TCheckedThreadSafeObj, base_class);
				};
				template<>
				class TCheckedThreadSafeObj<const int> : public TCheckedThreadSafeObj<const mse::TInt<int> > {
					typedef TCheckedThreadSafeObj<const mse::TInt<int> > base_class;
					MSE_USING(TCheckedThreadSafeObj, base_class);
				};
				template<>
				class TCheckedThreadSafePointer<int> : public TCheckedThreadSafePointer<mse::TInt<int> > {
					typedef TCheckedThreadSafePointer<mse::TInt<int> > base_class;
					MSE_USING(TCheckedThreadSafePointer, base_class);
				};
				template<>
				class TCheckedThreadSafePointer<const int> : public TCheckedThreadSafePointer<const mse::TInt<int> > {
					typedef TCheckedThreadSafePointer<const mse::TInt<int> > base_class;
					MSE_USING(TCheckedThreadSafePointer, base_class);
				};
				template<>
				class TCheckedThreadSafeConstPointer<int> : public TCheckedThreadSafeConstPointer<mse::TInt<int> > {
					typedef TCheckedThreadSafeConstPointer<mse::TInt<int> > base_class;
					MSE_USING(TCheckedThreadSafeConstPointer, base_class);
				};
				template<>
				class TCheckedThreadSafeConstPointer<const int> : public TCheckedThreadSafeConstPointer<const mse::TInt<int> > {
					typedef TCheckedThreadSafeConstPointer<const mse::TInt<int> > base_class;
					MSE_USING(TCheckedThreadSafeConstPointer, base_class);
				};

#endif // !MSE_CHECKED_THREAD_SAFE_DO_NOT_USE_GNORAD
			}
		}


#ifdef MSE_STATICPOINTER_DISABLED
		template<typename _Ty> using TStaticImmutableConstPointer = const _Ty*;
		template<typename _Ty> using TStaticImmutablePointer = TStaticImmutableConstPointer<_Ty>;
		template<typename _Ty> using TStaticImmutableNotNullConstPointer = const _Ty*;
		template<typename _Ty> using TStaticImmutableNotNullPointer = TStaticImmutableNotNullConstPointer<_Ty>;
		template<typename _Ty> using TStaticImmutableFixedConstPointer = const _Ty* /*const*/;
		template<typename _Ty> using TStaticImmutableFixedPointer = TStaticImmutableFixedConstPointer<_Ty>;
		template<typename _TROy> using TStaticImmutableObj = const _TROy;

		template<typename _Ty> auto static_fptr_to(_Ty&& _X) { return std::addressof(_X); }
		template<typename _Ty> auto static_fptr_to(const _Ty& _X) { return std::addressof(_X); }

#else /*MSE_STATICPOINTER_DISABLED*/

	}
	namespace us {
		namespace impl {
			namespace static_immutable {

				template<typename _Ty> class TStaticImmutableID {};

				/* Objects of static duration are still prone to being accessed after destruction, and therefore still
				need appropriate safety mechanisms. However, in the case where the object has a trivial destructor,
				we're dispensing with the safety mechanisms. For now, anyway. */

				template<typename _TROz>
				using use_unchecked_base_type = typename mse::impl::conjunction<std::is_trivially_destructible<_TROz>
#if defined(MSE_STATICIMMUTABLEPOINTER_RUNTIME_CHECKS_ENABLED)
					, std::false_type
#endif // MSE_STATICIMMUTABLEPOINTER_RUNTIME_CHECKS_ENABLED
				>::type;

				template<typename _TROz>
				using TStaticImmutableObjBaseBase = mse::impl::conditional_t<use_unchecked_base_type<_TROz>::value
					, mse::impl::remove_const_t<_TROz>, mse::rsv::impl::cts::TCheckedThreadSafeObj<const _TROz>>;

				template<typename _Ty>
				using TStaticImmutableConstPointerBaseBase = mse::impl::conditional_t<use_unchecked_base_type<_Ty>::value
					, mse::us::impl::TPointerForLegacy<const _Ty, TStaticImmutableID<const _Ty>>, mse::rsv::impl::cts::TCheckedThreadSafeConstPointer<const _Ty>>;

				template<typename _TROz>
				class TStaticImmutableObjBase : public TStaticImmutableObjBaseBase<_TROz> {
				public:
					typedef TStaticImmutableObjBaseBase<_TROz> base_class;
					MSE_STATIC_USING(TStaticImmutableObjBase, base_class);
					TStaticImmutableObjBase(const TStaticImmutableObjBase& _X) : base_class(_X) {}
					TStaticImmutableObjBase(TStaticImmutableObjBase&& _X) : base_class(MSE_FWD(_X)) {}

					TStaticImmutableObjBase& operator=(TStaticImmutableObjBase&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); }
					TStaticImmutableObjBase& operator=(const TStaticImmutableObjBase& _X) { base_class::operator=(_X); return (*this); }
					template<class _Ty2>
					TStaticImmutableObjBase& operator=(_Ty2&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); }
					template<class _Ty2>
					TStaticImmutableObjBase& operator=(const _Ty2& _X) { base_class::operator=(_X); return (*this); }

					auto operator&() { return operator_ampersand(use_unchecked_base_type<_TROz>()); }
					auto operator&() const { return operator_ampersand(use_unchecked_base_type<_TROz>()); }

				private:
					auto operator_ampersand(std::false_type) const { return base_class::operator&(); }
					auto operator_ampersand(std::true_type) const { return static_cast<const base_class*>(this); }
					auto operator_ampersand(std::false_type) { return base_class::operator&(); }
					auto operator_ampersand(std::true_type) { return static_cast<base_class*>(this); }
				};

				template<typename _Ty> class TStaticImmutableConstPointerBase : public TStaticImmutableConstPointerBaseBase<_Ty> {
				public:
					typedef TStaticImmutableConstPointerBaseBase<_Ty> base_class;
					MSE_USING(TStaticImmutableConstPointerBase, base_class);
					TStaticImmutableConstPointerBase(const TStaticImmutableObjBase<_Ty>* src) : base_class(&(*src)) {}
				};
			}
		}
	}
	namespace rsv {

		template <class _Ty, class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<
			(!std::is_same<_Ty&&, _Ty2>::value) || (!std::is_rvalue_reference<_Ty2>::value)
			> MSE_IMPL_EIS >
			static void valid_if_not_rvalue_reference_of_given_type_msestatic(_Ty2 src) {}

		template<typename _Ty> class TStaticImmutableObj;
		template<typename _Ty> class TStaticImmutableNotNullPointer;
		template<typename _Ty> class TStaticImmutableNotNullConstPointer;
		template<typename _Ty> class TStaticImmutableFixedConstPointer;
		template<typename _Ty> using TStaticImmutableFixedPointer = TStaticImmutableFixedConstPointer<_Ty>;
	}

	namespace us {
		namespace impl {
			template <typename _Ty, typename _TConstPointer1> class TCommonizedPointer;
			template <typename _Ty, typename _TConstPointer1> class TCommonizedConstPointer;
		}
	}

	namespace rsv {

		/* Use TStaticImmutableFixedConstPointer instead. */
		template<typename _Ty>
		class TStaticImmutableConstPointer : public mse::us::impl::static_immutable::TStaticImmutableConstPointerBase<_Ty>
			, public mse::impl::first_or_placeholder_if_base_of_second<mse::us::impl::StrongPointerTagBase, mse::us::impl::static_immutable::TStaticImmutableConstPointerBase<_Ty>, TStaticImmutableConstPointer<_Ty> >
		{
		public:
			typedef mse::us::impl::static_immutable::TStaticImmutableConstPointerBase<_Ty> base_class;
			MSE_IMPL_DESTRUCTOR_PREFIX1 ~TStaticImmutableConstPointer() {}
		private:
			TStaticImmutableConstPointer() : base_class() {}
			TStaticImmutableConstPointer(const base_class& ptr) : base_class(ptr) {}
			TStaticImmutableConstPointer(const TStaticImmutableConstPointer& src_cref) : base_class(static_cast<const base_class&>(src_cref)) {}
			template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
			TStaticImmutableConstPointer(const TStaticImmutableConstPointer<_Ty2>& src_cref) : base_class(src_cref) {}
			//TStaticImmutableConstPointer(const TStaticImmutablePointer<_Ty>& src_cref) : base_class(src_cref) {}
			//template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
			//TStaticImmutableConstPointer(const TStaticImmutablePointer<_Ty2>& src_cref) : base_class(impl::TStaticImmutableConstPointerBase<_Ty2>(src_cref)) {}
			TStaticImmutableConstPointer<_Ty>& operator=(const TStaticImmutableObj<_Ty>* ptr) {
				base_class::operator=(ptr);
				return *this;
			}
			TStaticImmutableConstPointer<_Ty>& operator=(const TStaticImmutableConstPointer<_Ty>& _Right_cref) {
				base_class::operator=(_Right_cref);
				return *this;
			}
			operator bool() const {
				bool retval = (bool(*static_cast<const base_class*>(this)));
				return retval;
			}
			/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
			MSE_DEPRECATED explicit operator const _Ty*() const {
				const _Ty* retval = std::addressof(*(*static_cast<const base_class*>(this)));
				return retval;
			}
			MSE_DEPRECATED explicit operator const TStaticImmutableObj<_Ty>*() const {
				const TStaticImmutableObj<_Ty>* retval = std::addressof(*(*static_cast<const base_class*>(this)));
				return retval;
			}

			MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

			friend class TStaticImmutableNotNullConstPointer<_Ty>;
			friend class mse::us::impl::TCommonizedConstPointer<const _Ty, TStaticImmutableConstPointer<_Ty> >;
		};

		template<typename _Ty>
		bool operator==(const TStaticImmutableConstPointer<_Ty>& lhs, const TStaticImmutableConstPointer<_Ty>& rhs) {
			return std::addressof(*lhs) == std::addressof(*rhs);
		}
		template<typename _Ty>
		bool operator!=(const TStaticImmutableConstPointer<_Ty>& lhs, const TStaticImmutableConstPointer<_Ty>& rhs) {
			return !(lhs == rhs);
		}

		/* Use TStaticImmutableFixedConstPointer instead. */
		template<typename _Ty>
		class TStaticImmutableNotNullConstPointer : public TStaticImmutableConstPointer<_Ty>, public mse::us::impl::NeverNullTagBase {
		public:
			typedef TStaticImmutableConstPointer<_Ty> base_class;
			MSE_IMPL_DESTRUCTOR_PREFIX1 ~TStaticImmutableNotNullConstPointer() {}
			operator bool() const { return (*static_cast<const base_class*>(this)); }
#ifndef MSE_STATICIMMUTABLE_NO_XSCOPE_DEPENDENCE
			operator mse::TXScopeFixedConstPointer<_Ty>() const {
				return mse::us::unsafe_make_xscope_const_pointer_to(*(*this));
			}
#endif // !MSE_STATICIMMUTABLE_NO_XSCOPE_DEPENDENCE
			void static_tag() const {}
			void async_passable_tag() const {} /* Indication that this type is eligible to be passed between threads. */

		private:
			TStaticImmutableNotNullConstPointer(const TStaticImmutableNotNullConstPointer<_Ty>& src_cref) : base_class(src_cref) {}
			template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2*, _Ty*>::value> MSE_IMPL_EIS >
			TStaticImmutableNotNullConstPointer(const TStaticImmutableNotNullConstPointer<_Ty2>& src_cref) : base_class(src_cref) {}
			//TStaticImmutableNotNullConstPointer(const TStaticImmutableNotNullPointer<_Ty>& src_cref) : base_class(src_cref) {}
			//template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
			//TStaticImmutableNotNullConstPointer(const TStaticImmutableNotNullPointer<_Ty2>& src_cref) : base_class(src_cref) {}

			TStaticImmutableNotNullConstPointer<_Ty>& operator=(const TStaticImmutableNotNullConstPointer<_Ty>& _Right_cref) {
				base_class::operator=(_Right_cref);
				return *this;
			}

			/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
#ifndef MSE_SCOPEPOINTER_DISABLED
			MSE_DEPRECATED explicit operator const _Ty* () const { return base_class::operator const _Ty * (); }
#endif // !MSE_SCOPEPOINTER_DISABLED
			MSE_DEPRECATED explicit operator const TStaticImmutableObj<_Ty>*() const { return base_class::operator const TStaticImmutableObj<_Ty>*(); }
			TStaticImmutableNotNullConstPointer(const typename base_class::base_class& ptr) : base_class(ptr) {}

			MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

			friend class TStaticImmutableFixedConstPointer<_Ty>;
		};

		template<typename _Ty>
		class TStaticImmutableFixedConstPointer : public TStaticImmutableNotNullConstPointer<_Ty> {
		public:
			typedef TStaticImmutableNotNullConstPointer<_Ty> base_class;
			TStaticImmutableFixedConstPointer(const TStaticImmutableFixedConstPointer<_Ty>& src_cref) : base_class(src_cref) {}
			template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
			TStaticImmutableFixedConstPointer(const TStaticImmutableFixedConstPointer<_Ty2>& src_cref) : base_class(src_cref) {}
			//TStaticImmutableFixedConstPointer(const TStaticImmutableFixedPointer<_Ty>& src_cref) : base_class(src_cref) {}
			//template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
			//TStaticImmutableFixedConstPointer(const TStaticImmutableFixedPointer<_Ty2>& src_cref) : base_class(src_cref) {}
			MSE_IMPL_DESTRUCTOR_PREFIX1 ~TStaticImmutableFixedConstPointer() {}
			operator bool() const { return (*static_cast<const base_class*>(this)); }
			/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
#ifndef MSE_SCOPEPOINTER_DISABLED
			MSE_DEPRECATED explicit operator const _Ty* () const { return base_class::operator const _Ty * (); }
#endif // !MSE_SCOPEPOINTER_DISABLED
			MSE_DEPRECATED explicit operator const TStaticImmutableObj<_Ty>*() const { return base_class::operator const TStaticImmutableObj<_Ty>*(); }
			void static_tag() const {}
			void async_passable_tag() const {} /* Indication that this type is eligible to be passed between threads. */

		private:
			TStaticImmutableFixedConstPointer(const typename TStaticImmutableConstPointer<_Ty>::base_class& ptr) : base_class(ptr) {}
			TStaticImmutableFixedConstPointer<_Ty>& operator=(const TStaticImmutableFixedConstPointer<_Ty>& _Right_cref) = delete;
			MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

			friend class TStaticImmutableObj<_Ty>;
		};

		template<typename _TROy>
		class TStaticImmutableObj : public mse::us::impl::static_immutable::TStaticImmutableObjBase<_TROy> {
		public:
			typedef mse::us::impl::static_immutable::TStaticImmutableObjBase<_TROy> base_class;
			TStaticImmutableObj(const TStaticImmutableObj& _X) : base_class(_X) {}
			TStaticImmutableObj(TStaticImmutableObj&& _X) : base_class(MSE_FWD(_X)) {}

			MSE_STATIC_USING(TStaticImmutableObj, base_class);
			MSE_IMPL_DESTRUCTOR_PREFIX1 ~TStaticImmutableObj() {
				mse::impl::T_valid_if_is_marked_as_xscope_shareable_msemsearray<_TROy>();
			}

			TStaticImmutableFixedConstPointer<_TROy> operator&() const & {
				return typename TStaticImmutableConstPointer<_TROy>::base_class(&(*static_cast<const base_class*>(this)));
			}
			TStaticImmutableFixedConstPointer<_TROy> mse_static_fptr() const & {
				return typename TStaticImmutableConstPointer<_TROy>::base_class(&(*static_cast<const base_class*>(this)));
			}

			void operator&() && = delete;
			void operator&() const && = delete;
			void mse_static_fptr() && = delete;
			void mse_static_fptr() const && = delete;

			void static_tag() const {}

		private:

			TStaticImmutableObj& operator=(TStaticImmutableObj&& _X) {
				base_class::operator=(MSE_FWD(_X));
				return (*this);
			}
			TStaticImmutableObj& operator=(const TStaticImmutableObj& _X) { base_class::operator=(_X); return (*this); }
			template<class _Ty2>
			TStaticImmutableObj& operator=(_Ty2&& _X) {
				base_class::operator=(MSE_FWD(_X));
				return (*this);
			}
			template<class _Ty2>
			TStaticImmutableObj& operator=(const _Ty2& _X) { base_class::operator=(_X); return (*this); }

			void operator&() & {
				/* This object does not seem to be declared const, which is not valid. Objects of this type should only be
				declared via the provided macro that declares them (static and) const. */
			}
			void mse_static_fptr() & { &(*this); }

			void* operator new(size_t size) { return ::operator new(size); }
		};

		template<typename _Ty>
		auto static_fptr_to(_Ty&& _X) {
			return _X.mse_static_fptr();
		}
		template<typename _Ty>
		auto static_fptr_to(const _Ty& _X) {
			return _X.mse_static_fptr();
		}
	}
}

namespace std {

	template<class _Ty>
	struct hash<mse::rsv::TStaticImmutableFixedConstPointer<_Ty> > {	// hash functor
		typedef mse::rsv::TStaticImmutableFixedConstPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::rsv::TStaticImmutableFixedConstPointer<_Ty>& _Keyval) const _NOEXCEPT {
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

#ifndef MSE_STATICIMMUTABLE_NO_XSCOPE_DEPENDENCE
#ifdef MSE_SCOPEPOINTER_DISABLED
		/* The safety of these reinterpret_cast<>s rely on the pointer target (i.e. the 'mapped_type') being safely
		"reinterpretable" as a 'specified_type'. */
#define MSE_STATICIMMUTABLE_IMPL_PTR_OPERATOR_TXSCOPEFIXEDCONSTPOINTER_RETVAL(specified_type, mapped_type) \
			reinterpret_cast<const specified_type*>(std::addressof(*(*this)))
#else // MSE_SCOPEPOINTER_DISABLED
#define MSE_STATICIMMUTABLE_IMPL_PTR_OPERATOR_TXSCOPEFIXEDCONSTPOINTER_RETVAL(specified_type, mapped_type) \
			mse::TXScopeFixedConstPointer<mapped_type>(*static_cast<const base_class *>(this))
#endif // MSE_SCOPEPOINTER_DISABLED

#define MSE_STATICIMMUTABLE_IMPL_PTR_OPERATOR_TXSCOPEFIXEDCONSTPOINTER(specified_type, mapped_type) \
		operator mse::TXScopeFixedConstPointer<specified_type>() const { \
			return MSE_STATICIMMUTABLE_IMPL_PTR_OPERATOR_TXSCOPEFIXEDCONSTPOINTER_RETVAL(specified_type, mapped_type); \
		}
#else // !MSE_STATICIMMUTABLE_NO_XSCOPE_DEPENDENCE
#define MSE_STATICIMMUTABLE_IMPL_PTR_OPERATOR_TXSCOPEFIXEDCONSTPOINTER(specified_type, mapped_type) 
#endif // !MSE_STATICIMMUTABLE_NO_XSCOPE_DEPENDENCE

#define MSE_STATICIMMUTABLE_IMPL_OBJ_INHERIT_OPERATOR_AMPERSAND \
		auto operator&() const & { return base_class::operator&(); } \
		void operator&() && = delete; \
		void operator&() const&& = delete;

#define MSE_STATICIMMUTABLE_IMPL_OBJ_SPECIALIZATION_DEFINITIONS1(class_name) \
		class_name(const class_name&) = default; \
		class_name(class_name&&) = default; \
		MSE_STATICIMMUTABLE_IMPL_OBJ_INHERIT_OPERATOR_AMPERSAND;

#if !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)
#define MSE_STATICIMMUTABLE_IMPL_OBJ_NATIVE_POINTER_PRIVATE_CONSTRUCTORS1(class_name) \
			class_name(std::nullptr_t) {} \
			class_name() {}
#else // !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)
#define MSE_STATICIMMUTABLE_IMPL_OBJ_NATIVE_POINTER_PRIVATE_CONSTRUCTORS1(class_name)
#endif // !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)

	/* Note that because we explicitly define some (private) constructors, default copy and move constructors
	and assignment operators won't be generated, so we have to define those as well. */
#define MSE_STATICIMMUTABLE_IMPL_OBJ_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type) \
		template<typename _Ty> \
		class TStaticImmutableObj<specified_type> : public TStaticImmutableObj<mapped_type> { \
		public: \
			typedef TStaticImmutableObj<mapped_type> base_class; \
			MSE_USING(TStaticImmutableObj, base_class); \
			MSE_STATICIMMUTABLE_IMPL_OBJ_SPECIALIZATION_DEFINITIONS1(TStaticImmutableObj); \
		private: \
			MSE_STATICIMMUTABLE_IMPL_OBJ_NATIVE_POINTER_PRIVATE_CONSTRUCTORS1(TStaticImmutableObj); \
		};

		/* Note that the 'mapped_type' must be safely "reinterpret_cast<>able" as a 'specified_type'. */
#define MSE_STATICIMMUTABLE_IMPL_PTR_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type) \
		template<typename _Ty> \
		class TStaticImmutableFixedConstPointer<specified_type> : public TStaticImmutableFixedConstPointer<mapped_type> { \
		public: \
			typedef TStaticImmutableFixedConstPointer<mapped_type> base_class; \
			MSE_USING(TStaticImmutableFixedConstPointer, base_class); \
			MSE_STATICIMMUTABLE_IMPL_PTR_OPERATOR_TXSCOPEFIXEDCONSTPOINTER(specified_type, mapped_type); \
		};

#define MSE_STATICIMMUTABLE_IMPL_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type) \
		MSE_STATICIMMUTABLE_IMPL_PTR_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type); \
		MSE_STATICIMMUTABLE_IMPL_OBJ_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type);

		MSE_STATICIMMUTABLE_IMPL_NATIVE_POINTER_SPECIALIZATION(_Ty*, mse::us::impl::TPointerForLegacy<_Ty>);
		MSE_STATICIMMUTABLE_IMPL_NATIVE_POINTER_SPECIALIZATION(_Ty* const, const mse::us::impl::TPointerForLegacy<_Ty>);

#ifdef MSEPRIMITIVES_H

		/* Note that here we're relying on the fact that mse::TInt<integral_type> happens to be safely
		"reinterpret_cast<>able" as an 'integral_type'. */
#define MSE_STATICIMMUTABLE_IMPL_OBJ_INTEGRAL_SPECIALIZATION(integral_type) \
		template<> \
		class TStaticImmutableObj<integral_type> : public TStaticImmutableObj<mse::TInt<integral_type>> { \
		public: \
			typedef TStaticImmutableObj<mse::TInt<integral_type>> base_class; \
			MSE_USING(TStaticImmutableObj, base_class); \
			MSE_STATICIMMUTABLE_IMPL_OBJ_SPECIALIZATION_DEFINITIONS1(TStaticImmutableObj); \
		};

#define MSE_STATICIMMUTABLE_IMPL_PTR_INTEGRAL_SPECIALIZATION(integral_type) \
		template<> \
		class TStaticImmutableFixedConstPointer<integral_type> : public TStaticImmutableFixedConstPointer<mse::TInt<integral_type>> { \
		public: \
			typedef TStaticImmutableFixedConstPointer<mse::TInt<integral_type>> base_class; \
			MSE_USING(TStaticImmutableFixedConstPointer, base_class); \
			MSE_STATICIMMUTABLE_IMPL_PTR_OPERATOR_TXSCOPEFIXEDCONSTPOINTER(integral_type, mse::TInt<integral_type>); \
		};

#define MSE_STATICIMMUTABLE_IMPL_INTEGRAL_SPECIALIZATION(integral_type) \
		MSE_STATICIMMUTABLE_IMPL_PTR_INTEGRAL_SPECIALIZATION(integral_type); \
		MSE_STATICIMMUTABLE_IMPL_OBJ_INTEGRAL_SPECIALIZATION(integral_type); \
		MSE_STATICIMMUTABLE_IMPL_PTR_INTEGRAL_SPECIALIZATION(typename std::add_const<integral_type>::type); \
		MSE_STATICIMMUTABLE_IMPL_OBJ_INTEGRAL_SPECIALIZATION(typename std::add_const<integral_type>::type);

		MSE_IMPL_APPLY_MACRO_FUNCTION_TO_EACH_OF_THE_INTEGER_TYPES(MSE_STATICIMMUTABLE_IMPL_INTEGRAL_SPECIALIZATION)

#endif /*MSEPRIMITIVES_H*/

		/* end of template specializations */

#endif /*MSE_STATICPOINTER_DISABLED*/
	}

#define MSE_DECLARE_STATIC_IMMUTABLE(type) static const mse::rsv::TStaticImmutableObj<type> 
#define MSE_RSV_DECLARE_GLOBAL_IMMUTABLE(type) const mse::rsv::TStaticImmutableObj<type> 


#ifndef MSE_STATICPOINTER_DISABLED
#ifndef MSE_STATICIMMUTABLE_NO_XSCOPE_DEPENDENCE
	namespace impl {
		namespace static_immutable {
			template <typename _Ty>
			struct UnderlyingTemplateParam {
				typedef void type;
			};
			template <typename _Ty>
			struct UnderlyingTemplateParam<mse::rsv::TStaticImmutableFixedConstPointer<_Ty> > {
				typedef typename UnderlyingTemplateParam<typename mse::rsv::TStaticImmutableFixedConstPointer<_Ty>::base_class>::type helper_type1;
				typedef mse::impl::conditional_t<std::is_same<void, helper_type1>::value, _Ty, helper_type1> type;
			};
		}
	}
	template <typename _Ty>
	auto xscope_pointer(const rsv::TStaticImmutableFixedConstPointer<_Ty>& ptr) {
		typedef typename impl::static_immutable::UnderlyingTemplateParam<rsv::TStaticImmutableFixedConstPointer<_Ty> >::type type1;
		const type1* raw_ptr2 = std::addressof(*ptr);
		return mse::us::unsafe_make_xscope_const_pointer_to(*raw_ptr2);
	}
	template <typename _Ty>
	auto xscope_const_pointer(const rsv::TStaticImmutableFixedConstPointer<_Ty>& ptr) {
		typedef typename impl::static_immutable::UnderlyingTemplateParam<rsv::TStaticImmutableFixedConstPointer<_Ty> >::type type1;
		const type1* raw_ptr2 = std::addressof(*ptr);
		return mse::us::unsafe_make_xscope_const_pointer_to(*raw_ptr2);
	}
#endif // !MSE_STATICIMMUTABLE_NO_XSCOPE_DEPENDENCE
#endif // !MSE_STATICPOINTER_DISABLED



	namespace self_test {
		class CStaticImmutablePtrTest1 {
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
				typedef mse::rsv::TAsyncShareableObj<A> shareable_A;
				class B {
				public:
					static int foo1(const shareable_A* a_native_ptr) { return a_native_ptr->b; }
					static int foo2(mse::rsv::TStaticImmutableFixedPointer<shareable_A> shareable_A_static_ptr) { return shareable_A_static_ptr->b; }
				protected:
					~B() {}
				};

				shareable_A* shareable_A_native_ptr = nullptr;

				{
					shareable_A a(7);
					MSE_DECLARE_STATIC_IMMUTABLE(shareable_A) static_a(7);

					assert(a.b == static_a.b);
					shareable_A_native_ptr = std::addressof(a);

					mse::rsv::TStaticImmutableFixedPointer<shareable_A> shareable_A_static_ptr1(&static_a);
					assert(shareable_A_native_ptr->b == shareable_A_static_ptr1->b);
					mse::rsv::TStaticImmutableFixedPointer<shareable_A> shareable_A_static_ptr2 = &static_a;

					if (!shareable_A_static_ptr2) {
						assert(false);
					}
					else if (!(shareable_A_static_ptr2 != shareable_A_static_ptr1)) {
						int q = B::foo2(shareable_A_static_ptr2);
					}
					else {
						assert(false);
					}

					shareable_A a2 = a;
					MSE_DECLARE_STATIC_IMMUTABLE(shareable_A) static_a2 = static_a;

					mse::rsv::TStaticImmutableFixedConstPointer<shareable_A> rcp = shareable_A_static_ptr1;
					mse::rsv::TStaticImmutableFixedConstPointer<shareable_A> rcp2 = rcp;
				}

				{
					class A {
					public:
						A(int x) : b(x) {}
						virtual ~A() {}

						int b = 3;
						std::string s = "some text ";
					};
					typedef mse::rsv::TAsyncShareableObj<A> shareable_A;
					class B {
					public:
						static int foo1(shareable_A* a_native_ptr) { return a_native_ptr->b; }
						static int foo2(mse::rsv::TStaticImmutableFixedPointer<shareable_A> shareable_A_scpfptr) { return shareable_A_scpfptr->b; }
						static int foo3(mse::rsv::TStaticImmutableFixedConstPointer<shareable_A> shareable_A_scpfcptr) { return shareable_A_scpfcptr->b; }
					protected:
						~B() {}
					};

					MSE_DECLARE_STATIC_IMMUTABLE(shareable_A) a_scpobj(5);
					int res1 = (&a_scpobj)->b;
					int res2 = B::foo2(&a_scpobj);
					int res3 = B::foo3(&a_scpobj);

					/* You can use the "mse::make_pointer_to_member_v2()" function to obtain a safe pointer to a member of
					an static object. */
					auto s_safe_ptr1 = mse::make_pointer_to_member_v2((&a_scpobj), &shareable_A::s);
					auto s_safe_const_ptr1 = mse::make_const_pointer_to_member_v2((&a_scpobj), &shareable_A::s);
				}

				{
					shareable_A a(7);
					MSE_DECLARE_STATIC_IMMUTABLE(shareable_A) static_a(7);

					assert(a.b == static_a.b);
					shareable_A_native_ptr = std::addressof(a);

					mse::rsv::TStaticImmutableFixedPointer<shareable_A> shareable_A_static_ptr1 = &static_a;
					assert(shareable_A_native_ptr->b == shareable_A_static_ptr1->b);
					mse::rsv::TStaticImmutableFixedPointer<shareable_A> shareable_A_static_ptr2 = &static_a;

					if (!shareable_A_static_ptr2) {
						assert(false);
					}
					else if (!(shareable_A_static_ptr2 != shareable_A_static_ptr1)) {
						int q = B::foo2(shareable_A_static_ptr2);
					}
					else {
						assert(false);
					}

					shareable_A a2 = a;
					MSE_DECLARE_STATIC_IMMUTABLE(shareable_A) static_a2 = static_a;

					mse::rsv::TStaticImmutableFixedConstPointer<shareable_A> rcp = shareable_A_static_ptr1;
					mse::rsv::TStaticImmutableFixedConstPointer<shareable_A> rcp2 = rcp;

#ifndef MSE_STATICIMMUTABLE_NO_XSCOPE_DEPENDENCE
					mse::TXScopeFixedConstPointer<shareable_A> xsptr = shareable_A_static_ptr1;
					mse::TXScopeFixedConstPointer<shareable_A> xscptr2 = mse::xscope_const_pointer(shareable_A_static_ptr1);
#endif // !MSE_STATICIMMUTABLE_NO_XSCOPE_DEPENDENCE
				}

				{
					int a(7);
					MSE_DECLARE_STATIC_IMMUTABLE(int) static_a(7);

					auto int_native_ptr = std::addressof(a);

					mse::rsv::TStaticImmutableFixedPointer<int> int_static_ptr1 = &static_a;
					mse::rsv::TStaticImmutableFixedPointer<int> int_static_ptr2 = &static_a;

					if (!int_static_ptr2) {
						assert(false);
					}
					else if (!(int_static_ptr2 != int_static_ptr1)) {
						int q = 5;
					}
					else {
						assert(false);
					}

					int a2 = a;
					MSE_DECLARE_STATIC_IMMUTABLE(int) static_a2 = static_a;

					mse::rsv::TStaticImmutableFixedConstPointer<int> rcp = int_static_ptr1;
					mse::rsv::TStaticImmutableFixedConstPointer<int> rcp2 = rcp;

#ifndef MSE_STATICIMMUTABLE_NO_XSCOPE_DEPENDENCE
					mse::TXScopeFixedConstPointer<int> xsptr = int_static_ptr1;
					/*mse::TXScopeFixedConstPointer<int>*/ auto xscptr2 = mse::xscope_const_pointer(int_static_ptr1);
#endif // !MSE_STATICIMMUTABLE_NO_XSCOPE_DEPENDENCE
				}
				{
					MSE_DECLARE_STATIC_IMMUTABLE(int) cthread_local_a(11);
					mse::rsv::TStaticImmutableFixedConstPointer<int> rfcp = &cthread_local_a;
				}

#endif // MSE_SELF_TESTS
			}
		};
	}
}

#ifndef MSE_PUSH_MACRO_NOT_SUPPORTED
#pragma pop_macro("MSE_THROW")
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

#endif // MSESTATIC_H_
