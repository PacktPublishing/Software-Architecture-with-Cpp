
// Copyright (c) 2015 Noah Lopez
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef MSESCOPEATOMIC_H_
#define MSESCOPEATOMIC_H_

#include "msepointerbasics.h"
#include "mseprimitives.h"
#include "msemsearray.h" /* for T_valid_if_is_marked_as_xscope_shareable_msemsearray<>() */
#include "msestaticimmutable.h" /* for mse::rsv::impl::cts::TCheckedThreadSafeObj<> */
#include <atomic>
#include <utility>
#include <unordered_set>
#include <functional>
#include <cassert>

#ifdef MSE_SCOPEPOINTER_RUNTIME_CHECKS_ENABLED
#include "msenorad.h"
#include "mseany.h"
#endif // MSE_SCOPEPOINTER_RUNTIME_CHECKS_ENABLED

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

/* Note that by default, MSE_SCOPEPOINTER_DISABLED is defined in non-debug builds. This is enacted in "msepointerbasics.h". */

#ifndef MSE_PUSH_MACRO_NOT_SUPPORTED
#pragma push_macro("_NOEXCEPT")
#endif // !MSE_PUSH_MACRO_NOT_SUPPORTED

#ifndef _NOEXCEPT
#define _NOEXCEPT
#endif /*_NOEXCEPT*/


namespace mse {

	/* This macro roughly simulates constructor inheritance. */
#define MSE_SCOPE_ATOMIC_USING(Derived, Base) MSE_USING(Derived, Base)

	namespace impl {
		template<typename _Ty> class TScopeAtomicID {};
	}

#ifdef MSE_SCOPEPOINTER_DISABLED
	template<typename _Ty> using TXScopeAtomicPointer = std::atomic<_Ty> * ;
	template<typename _Ty> using TXScopeAtomicConstPointer = const std::atomic<_Ty>*;
	template<typename _Ty> using TXScopeAtomicNotNullPointer = std::atomic<_Ty> * ;
	template<typename _Ty> using TXScopeAtomicNotNullConstPointer = const std::atomic<_Ty>*;
	template<typename _Ty> using TXScopeAtomicFixedPointer = std::atomic<_Ty> * /*const*/; /* Can't be const qualified because standard
																	   library containers don't support const elements. */
	template<typename _Ty> using TXScopeAtomicFixedConstPointer = const std::atomic<_Ty>* /*const*/;

	namespace us {
		namespace impl {
			template<typename _TROy> using TXScopeAtomicObjBase = std::atomic<_TROy>;
		}
	}
	template<typename _TROy> using TXScopeAtomicObj = std::atomic<_TROy>;
	template<typename _Ty> using TXScopeAtomicItemFixedPointer = std::atomic<_Ty> * /*const*/;
	template<typename _Ty> using TXScopeAtomicItemFixedConstPointer = const std::atomic<_Ty>* /*const*/;
	template<typename _Ty> using TXScopeAtomicCagedItemFixedPointerToRValue = std::atomic<_Ty> * /*const*/;
	template<typename _Ty> using TXScopeAtomicCagedItemFixedConstPointerToRValue = const std::atomic<_Ty>* /*const*/;

	template<typename _TROy> class TXScopeAtomicOwnerPointer;

	template<typename _Ty> auto xscope_aifptr_to(_Ty&& _X) { return std::addressof(_X); }
	template<typename _Ty> auto xscope_aifptr_to(const _Ty& _X) { return std::addressof(_X); }

#else /*MSE_SCOPEPOINTER_DISABLED*/

	template<typename _Ty> class TXScopeAtomicObj;

	namespace us {
		namespace impl {

#ifdef MSE_SCOPEPOINTER_RUNTIME_CHECKS_ENABLED

			template<typename _TROz> using TXScopeAtomicObjBase = mse::rsv::impl::cts::TCheckedThreadSafeObj<std::atomic<_TROz> >;
			template<typename _Ty> using TXScopeAtomicItemPointerBase = mse::us::impl::TAnyPointerBase<std::atomic<_Ty> >;
			template<typename _Ty> using TXScopeAtomicItemConstPointerBase = mse::us::impl::TAnyConstPointerBase<std::atomic<_Ty> >;

			template<typename _Ty>
			class TXScopeAtomicPointerBase : public mse::rsv::impl::cts::TCheckedThreadSafePointer<std::atomic<_Ty> > {
			public:
				typedef mse::rsv::impl::cts::TCheckedThreadSafePointer<std::atomic<_Ty> > base_class;
				TXScopeAtomicPointerBase(const TXScopeAtomicPointerBase&) = default;
				TXScopeAtomicPointerBase(TXScopeAtomicPointerBase&&) = default;
				TXScopeAtomicPointerBase(TXScopeAtomicObj<_Ty>& scpobj_ref) : base_class(&(static_cast<TXScopeAtomicObjBase<_Ty>&>(scpobj_ref))) {}

				TXScopeAtomicObj<_Ty>& operator*() const {
					return static_cast<TXScopeAtomicObj<_Ty>& >(*(static_cast<const base_class&>(*this)));
				}
				TXScopeAtomicObj<_Ty>* operator->() const {
					return std::addressof(static_cast<TXScopeAtomicObj<_Ty>&>(*(static_cast<const base_class&>(*this))));
				}
			};

			template<typename _Ty>
			class TXScopeAtomicConstPointerBase : public mse::rsv::impl::cts::TCheckedThreadSafeConstPointer<std::atomic<_Ty> > {
			public:
				typedef mse::rsv::impl::cts::TCheckedThreadSafeConstPointer<std::atomic<_Ty> > base_class;
				TXScopeAtomicConstPointerBase(const TXScopeAtomicConstPointerBase&) = default;
				TXScopeAtomicConstPointerBase(TXScopeAtomicConstPointerBase&&) = default;
				TXScopeAtomicConstPointerBase(const TXScopeAtomicPointerBase<_Ty>& src_cref) : base_class(src_cref) {}
				TXScopeAtomicConstPointerBase(const TXScopeAtomicObj<_Ty>& scpobj_cref) : base_class(&(static_cast<const TXScopeAtomicObjBase<_Ty>&>(scpobj_cref))) {}

				const TXScopeAtomicObj<_Ty>& operator*() const {
					return static_cast<const TXScopeAtomicObj<_Ty>&>(*(static_cast<const base_class&>(*this)));
				}
				const TXScopeAtomicObj<_Ty>* operator->() const {
					return std::addressof(static_cast<const TXScopeAtomicObj<_Ty>&>(*(static_cast<const base_class&>(*this))));
				}
			};

#else // MSE_SCOPEPOINTER_RUNTIME_CHECKS_ENABLED

			template<typename _TROz>
			class TXScopeAtomicObjBase : public std::atomic<_TROz> {
			public:
				typedef std::atomic<_TROz> base_class;
				MSE_SCOPE_ATOMIC_USING(TXScopeAtomicObjBase, base_class);
				TXScopeAtomicObjBase(const TXScopeAtomicObjBase& _X) : base_class(_X) {}
				TXScopeAtomicObjBase(TXScopeAtomicObjBase&& _X) : base_class(MSE_FWD(_X)) {}

				TXScopeAtomicObjBase& operator=(TXScopeAtomicObjBase&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); }
				TXScopeAtomicObjBase& operator=(const TXScopeAtomicObjBase& _X) { base_class::operator=(_X); return (*this); }
				template<class _Ty2>
				TXScopeAtomicObjBase& operator=(_Ty2&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); }
				template<class _Ty2>
				TXScopeAtomicObjBase& operator=(const _Ty2& _X) { base_class::operator=(_X); return (*this); }

				auto operator&() {
					return this;
				}
				auto operator&() const {
					return this;
				}
			};

			template<typename _Ty>
			class TXScopeAtomicPointerBase : public mse::us::impl::TPointerForLegacy<std::atomic<_Ty>, mse::impl::TScopeAtomicID<const std::atomic<_Ty> >> {
			public:
				typedef mse::us::impl::TPointerForLegacy<std::atomic<_Ty>, mse::impl::TScopeAtomicID<const std::atomic<_Ty> >> base_class;
				TXScopeAtomicPointerBase(const TXScopeAtomicPointerBase&) = default;
				TXScopeAtomicPointerBase(TXScopeAtomicPointerBase&&) = default;
				TXScopeAtomicPointerBase(TXScopeAtomicObj<_Ty>& scpobj_ref) : base_class(&(static_cast<TXScopeAtomicObjBase<_Ty>&>(scpobj_ref))) {}

				TXScopeAtomicObj<_Ty>& operator*() const {
					return static_cast<TXScopeAtomicObj<_Ty>&>(*(static_cast<const base_class&>(*this)));
				}
				TXScopeAtomicObj<_Ty>* operator->() const {
					return std::addressof(static_cast<TXScopeAtomicObj<_Ty>&>(*(static_cast<const base_class&>(*this))));
				}
			};

			template<typename _Ty>
			class TXScopeAtomicConstPointerBase : public mse::us::impl::TPointerForLegacy<const std::atomic<_Ty>, mse::impl::TScopeAtomicID<const std::atomic<_Ty> >> {
			public:
				typedef mse::us::impl::TPointerForLegacy<const std::atomic<_Ty>, mse::impl::TScopeAtomicID<const std::atomic<_Ty> >> base_class;
				TXScopeAtomicConstPointerBase(const TXScopeAtomicConstPointerBase&) = default;
				TXScopeAtomicConstPointerBase(TXScopeAtomicConstPointerBase&&) = default;
				TXScopeAtomicConstPointerBase(const TXScopeAtomicPointerBase<_Ty>& src_cref) : base_class(src_cref) {}
				TXScopeAtomicConstPointerBase(const TXScopeAtomicObj<_Ty>& scpobj_cref) : base_class(&(static_cast<const TXScopeAtomicObjBase<_Ty>&>(scpobj_cref))) {}

				const TXScopeAtomicObj<_Ty>& operator*() const {
					return static_cast<const TXScopeAtomicObj<_Ty>&>(*(static_cast<const base_class&>(*this)));
				}
				const TXScopeAtomicObj<_Ty>* operator->() const {
					return std::addressof(static_cast<const TXScopeAtomicObj<_Ty>&>(*(static_cast<const base_class&>(*this))));
				}
			};

			template<typename _Ty> using TXScopeAtomicItemPointerBase = mse::us::impl::TPointerForLegacy<std::atomic<_Ty>, mse::impl::TScopeAtomicID<const std::atomic<_Ty> >>;
			template<typename _Ty> using TXScopeAtomicItemConstPointerBase = mse::us::impl::TPointerForLegacy<const std::atomic<_Ty>, mse::impl::TScopeAtomicID<const std::atomic<_Ty> >>;

#endif // MSE_SCOPEPOINTER_RUNTIME_CHECKS_ENABLED

		}
	}

	template<typename _Ty> class TXScopeAtomicObj;
	template<typename _Ty> class TXScopeAtomicNotNullPointer;
	template<typename _Ty> class TXScopeAtomicNotNullConstPointer;
	template<typename _Ty> class TXScopeAtomicFixedPointer;
	template<typename _Ty> class TXScopeAtomicFixedConstPointer;
	template<typename _Ty> class TXScopeAtomicOwnerPointer;

	template<typename _Ty> class TXScopeAtomicItemFixedPointer;
	template<typename _Ty> class TXScopeAtomicItemFixedConstPointer;
	template<typename _Ty> class TXScopeAtomicCagedItemFixedPointerToRValue;
	template<typename _Ty> class TXScopeAtomicCagedItemFixedConstPointerToRValue;
	namespace rsv {
		template<typename _Ty> class TXScopeAtomicItemFixedPointerFParam;
		template<typename _Ty> class TXScopeAtomicItemFixedConstPointerFParam;
	}

	namespace us {
		namespace impl {
			template <typename _Ty, typename _TConstPointer1> class TCommonizedPointer;
			template <typename _Ty, typename _TConstPointer1> class TCommonizedConstPointer;
		}
	}

	/* Use TXScopeAtomicFixedPointer instead. */
	template<typename _Ty>
	class TXScopeAtomicPointer : public mse::us::impl::TXScopeAtomicPointerBase<_Ty>, public mse::us::impl::XScopeContainsNonOwningScopeReferenceTagBase
		, public mse::impl::first_or_placeholder_if_base_of_second<mse::us::impl::StrongPointerTagBase, mse::us::impl::TXScopeAtomicPointerBase<_Ty>, TXScopeAtomicPointer<_Ty> >
		{
	public:
		~TXScopeAtomicPointer() {}
	private:
		TXScopeAtomicPointer() : mse::us::impl::TXScopeAtomicPointerBase<_Ty>() {}
		TXScopeAtomicPointer(TXScopeAtomicObj<_Ty>& scpobj_ref) : mse::us::impl::TXScopeAtomicPointerBase<_Ty>(scpobj_ref) {}
		TXScopeAtomicPointer(const TXScopeAtomicPointer& src_cref) : mse::us::impl::TXScopeAtomicPointerBase<_Ty>(
			static_cast<const mse::us::impl::TXScopeAtomicPointerBase<_Ty>&>(src_cref)) {}
		//template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		//TXScopeAtomicPointer(const TXScopeAtomicPointer<_Ty2>& src_cref) : mse::us::impl::TXScopeAtomicPointerBase<_Ty>(mse::us::impl::TXScopeAtomicPointerBase<_Ty2>(src_cref)) {}
		TXScopeAtomicPointer<_Ty>& operator=(TXScopeAtomicObj<_Ty>* ptr) {
			return mse::us::impl::TXScopeAtomicPointerBase<_Ty>::operator=(ptr);
		}
		TXScopeAtomicPointer<_Ty>& operator=(const TXScopeAtomicPointer<_Ty>& _Right_cref) {
			return mse::us::impl::TXScopeAtomicPointerBase<_Ty>::operator=(_Right_cref);
		}
		operator bool() const {
			bool retval = (bool(*static_cast<const mse::us::impl::TXScopeAtomicPointerBase<_Ty>*>(this)));
			return retval;
		}
		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator _Ty*() const {
			_Ty* retval = std::addressof(*(*this))/*(*static_cast<const mse::us::impl::TXScopeAtomicPointerBase<_Ty>*>(this))*/;
			return retval;
		}
		MSE_DEPRECATED explicit operator TXScopeAtomicObj<_Ty>*() const {
			TXScopeAtomicObj<_Ty>* retval = (*static_cast<const mse::us::impl::TXScopeAtomicPointerBase<_Ty>*>(this));
			return retval;
		}

		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		friend class TXScopeAtomicNotNullPointer<_Ty>;
		friend class us::impl::TCommonizedPointer<_Ty, TXScopeAtomicPointer<_Ty> >;
		friend class us::impl::TCommonizedConstPointer<const _Ty, TXScopeAtomicPointer<_Ty> >;
	};

	/* Use TXScopeAtomicFixedConstPointer instead. */
	template<typename _Ty>
	class TXScopeAtomicConstPointer : public mse::us::impl::TXScopeAtomicConstPointerBase<_Ty>, public mse::us::impl::XScopeContainsNonOwningScopeReferenceTagBase
		, public mse::impl::first_or_placeholder_if_base_of_second<mse::us::impl::StrongPointerTagBase, mse::us::impl::TXScopeAtomicConstPointerBase<_Ty>, TXScopeAtomicConstPointer<_Ty> >
	{
	public:
		~TXScopeAtomicConstPointer() {}
	private:
		TXScopeAtomicConstPointer() : mse::us::impl::TXScopeAtomicConstPointerBase<_Ty>() {}
		TXScopeAtomicConstPointer(const TXScopeAtomicObj<_Ty>& scpobj_cref) : mse::us::impl::TXScopeAtomicConstPointerBase<_Ty>(scpobj_cref) {}
		TXScopeAtomicConstPointer(const TXScopeAtomicConstPointer& src_cref) : mse::us::impl::TXScopeAtomicConstPointerBase<_Ty>(
			static_cast<const mse::us::impl::TXScopeAtomicConstPointerBase<_Ty>&>(src_cref)) {}
		//template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		//TXScopeAtomicConstPointer(const TXScopeAtomicConstPointer<_Ty2>& src_cref) : mse::us::impl::TXScopeAtomicConstPointerBase<_Ty>(src_cref) {}
		TXScopeAtomicConstPointer(const TXScopeAtomicPointer<_Ty>& src_cref) : mse::us::impl::TXScopeAtomicConstPointerBase<_Ty>(src_cref) {}
		//template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		//TXScopeAtomicConstPointer(const TXScopeAtomicPointer<_Ty2>& src_cref) : mse::us::impl::TXScopeAtomicConstPointerBase<_Ty>(mse::us::impl::TXScopeAtomicItemConstPointerBase<_Ty2>(src_cref)) {}
		TXScopeAtomicConstPointer<_Ty>& operator=(const TXScopeAtomicObj<_Ty>* ptr) {
			return mse::us::impl::TXScopeAtomicItemConstPointerBase<_Ty>::operator=(ptr);
		}
		TXScopeAtomicConstPointer<_Ty>& operator=(const TXScopeAtomicConstPointer<_Ty>& _Right_cref) {
			return mse::us::impl::TXScopeAtomicItemConstPointerBase<_Ty>::operator=(_Right_cref);
		}
		operator bool() const {
			bool retval = (bool(*static_cast<const mse::us::impl::TXScopeAtomicConstPointerBase<_Ty>*>(this)));
			return retval;
		}
		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator const _Ty*() const {
			const _Ty* retval = (*static_cast<const mse::us::impl::TXScopeAtomicConstPointerBase<_Ty>*>(this));
			return retval;
		}
		MSE_DEPRECATED explicit operator const TXScopeAtomicObj<_Ty>*() const {
			const TXScopeAtomicObj<_Ty>* retval = (*static_cast<const mse::us::impl::TXScopeAtomicConstPointerBase<_Ty>*>(this));
			return retval;
		}

		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		friend class TXScopeAtomicNotNullConstPointer<_Ty>;
		friend class us::impl::TCommonizedConstPointer<const _Ty, TXScopeAtomicConstPointer<_Ty> >;
	};

	/* Use TXScopeAtomicFixedPointer instead. */
	template<typename _Ty>
	class TXScopeAtomicNotNullPointer : public TXScopeAtomicPointer<_Ty>, public mse::us::impl::NeverNullTagBase {
	public:
		~TXScopeAtomicNotNullPointer() {}
	private:
		TXScopeAtomicNotNullPointer(TXScopeAtomicObj<_Ty>& scpobj_ref) : TXScopeAtomicPointer<_Ty>(scpobj_ref) {}
		TXScopeAtomicNotNullPointer(TXScopeAtomicObj<_Ty>* ptr) : TXScopeAtomicPointer<_Ty>(ptr) {}
		//template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		//TXScopeAtomicNotNullPointer(const TXScopeAtomicNotNullPointer<_Ty2>& src_cref) : TXScopeAtomicPointer<_Ty>(src_cref) {}
		TXScopeAtomicNotNullPointer<_Ty>& operator=(const TXScopeAtomicPointer<_Ty>& _Right_cref) {
			TXScopeAtomicPointer<_Ty>::operator=(_Right_cref);
			return (*this);
		}
		operator bool() const { return (*static_cast<const TXScopeAtomicPointer<_Ty>*>(this)); }
		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator _Ty*() const { return TXScopeAtomicPointer<_Ty>::operator _Ty*(); }
		MSE_DEPRECATED explicit operator TXScopeAtomicObj<_Ty>*() const { return TXScopeAtomicPointer<_Ty>::operator TXScopeAtomicObj<_Ty>*(); }

		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		friend class TXScopeAtomicFixedPointer<_Ty>;
	};

	/* Use TXScopeAtomicFixedConstPointer instead. */
	template<typename _Ty>
	class TXScopeAtomicNotNullConstPointer : public TXScopeAtomicConstPointer<_Ty>, public mse::us::impl::NeverNullTagBase {
	public:
		~TXScopeAtomicNotNullConstPointer() {}
	private:
		TXScopeAtomicNotNullConstPointer(const TXScopeAtomicNotNullConstPointer<_Ty>& src_cref) : TXScopeAtomicConstPointer<_Ty>(src_cref) {}
		//template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		//TXScopeAtomicNotNullConstPointer(const TXScopeAtomicNotNullConstPointer<_Ty2>& src_cref) : TXScopeAtomicConstPointer<_Ty>(src_cref) {}
		TXScopeAtomicNotNullConstPointer(const TXScopeAtomicNotNullPointer<_Ty>& src_cref) : TXScopeAtomicConstPointer<_Ty>(src_cref) {}
		//template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		//TXScopeAtomicNotNullConstPointer(const TXScopeAtomicNotNullPointer<_Ty2>& src_cref) : TXScopeAtomicConstPointer<_Ty>(src_cref) {}
		TXScopeAtomicNotNullConstPointer(const TXScopeAtomicObj<_Ty>& scpobj_cref) : TXScopeAtomicConstPointer<_Ty>(scpobj_cref) {}
		operator bool() const { return (*static_cast<const TXScopeAtomicConstPointer<_Ty>*>(this)); }
		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator const _Ty*() const { return TXScopeAtomicConstPointer<_Ty>::operator const _Ty*(); }
		MSE_DEPRECATED explicit operator const TXScopeAtomicObj<_Ty>*() const { return TXScopeAtomicConstPointer<_Ty>::operator const TXScopeAtomicObj<_Ty>*(); }

		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		friend class TXScopeAtomicFixedConstPointer<_Ty>;
	};

	/* A TXScopeAtomicFixedPointer points to a TXScopeAtomicObj. Its intended for very limited use. Basically just to pass a TXScopeAtomicObj
	by reference as a function parameter. TXScopeAtomicFixedPointers can be obtained from TXScopeAtomicObj's "&" (address of) operator. */
	template<typename _Ty>
	class TXScopeAtomicFixedPointer : public TXScopeAtomicNotNullPointer<_Ty> {
	public:
		TXScopeAtomicFixedPointer(const TXScopeAtomicFixedPointer& src_cref) : TXScopeAtomicNotNullPointer<_Ty>(src_cref) {}
		//template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		//TXScopeAtomicFixedPointer(const TXScopeAtomicFixedPointer<_Ty2>& src_cref) : TXScopeAtomicNotNullPointer<_Ty>(src_cref) {}
		~TXScopeAtomicFixedPointer() {}
		operator bool() const { return (*static_cast<const TXScopeAtomicNotNullPointer<_Ty>*>(this)); }
		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator _Ty*() const { return TXScopeAtomicNotNullPointer<_Ty>::operator _Ty*(); }
		MSE_DEPRECATED explicit operator TXScopeAtomicObj<_Ty>*() const { return TXScopeAtomicNotNullPointer<_Ty>::operator TXScopeAtomicObj<_Ty>*(); }
		void xscope_tag() const {}
		void xscope_async_passable_tag() const {} /* Indication that this type is eligible to be passed between threads. */

	private:
		TXScopeAtomicFixedPointer(TXScopeAtomicObj<_Ty>& scpobj_ref) : TXScopeAtomicNotNullPointer<_Ty>(scpobj_ref) {}
#ifdef MSE_SCOPE_DISABLE_MOVE_RESTRICTIONS
		/* Disabling public move construction prevents some unsafe operations, like some, but not all,
		attempts to use a TXScopeAtomicFixedPointer<> as a return value. But it also prevents some safe
		operations too, like initialization via '=' (assignment operator). And the set of prevented
		operations might not be consistent across compilers. We're deciding here that the minor safety
		benefits outweigh the minor inconveniences. Note that we can't disable public move construction
		in the other scope pointers as it would interfere with implicit conversions. */
		TXScopeAtomicFixedPointer(TXScopeAtomicFixedPointer&& src_ref) : TXScopeAtomicNotNullPointer<_Ty>(src_ref) {
			int q = 5;
		}
#endif // !MSE_SCOPE_DISABLE_MOVE_RESTRICTIONS
		TXScopeAtomicFixedPointer<_Ty>& operator=(const TXScopeAtomicFixedPointer<_Ty>& _Right_cref) = delete;
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		friend class TXScopeAtomicObj<_Ty>;
	};

	template<typename _Ty>
	class TXScopeAtomicFixedConstPointer : public TXScopeAtomicNotNullConstPointer<_Ty> {
	public:
		TXScopeAtomicFixedConstPointer(const TXScopeAtomicFixedConstPointer<_Ty>& src_cref) : TXScopeAtomicNotNullConstPointer<_Ty>(src_cref) {}
		//template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		//TXScopeAtomicFixedConstPointer(const TXScopeAtomicFixedConstPointer<_Ty2>& src_cref) : TXScopeAtomicNotNullConstPointer<_Ty>(src_cref) {}
		TXScopeAtomicFixedConstPointer(const TXScopeAtomicFixedPointer<_Ty>& src_cref) : TXScopeAtomicNotNullConstPointer<_Ty>(src_cref) {}
		//template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		//TXScopeAtomicFixedConstPointer(const TXScopeAtomicFixedPointer<_Ty2>& src_cref) : TXScopeAtomicNotNullConstPointer<_Ty>(src_cref) {}
		~TXScopeAtomicFixedConstPointer() {}
		operator bool() const { return (*static_cast<const TXScopeAtomicNotNullConstPointer<_Ty>*>(this)); }
		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator const _Ty*() const { return TXScopeAtomicNotNullConstPointer<_Ty>::operator const _Ty*(); }
		MSE_DEPRECATED explicit operator const TXScopeAtomicObj<_Ty>*() const { return TXScopeAtomicNotNullConstPointer<_Ty>::operator const TXScopeAtomicObj<_Ty>*(); }
		void xscope_tag() const {}
		void xscope_async_passable_tag() const {} /* Indication that this type is eligible to be passed between threads. */

	private:
		TXScopeAtomicFixedConstPointer(const TXScopeAtomicObj<_Ty>& scpobj_cref) : TXScopeAtomicNotNullConstPointer<_Ty>(scpobj_cref) {}
		TXScopeAtomicFixedConstPointer<_Ty>& operator=(const TXScopeAtomicFixedConstPointer<_Ty>& _Right_cref) = delete;
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		friend class TXScopeAtomicObj<_Ty>;
	};

	/* TXScopeAtomicObj is intended as a transparent wrapper for other classes/objects with "scope lifespans". That is, objects
	that are either allocated on the stack, or whose "owning" pointer is allocated on the stack. Unfortunately it's not
	really possible to completely prevent misuse. For example, std::list<TXScopeAtomicObj<mse::CInt>> is an improper, and
	dangerous, use of TXScopeAtomicObj<>. So we provide the option of using an mse::us::TFLRegisteredObj as TXScopeAtomicObj's base
	class to enforce safety and to help catch misuse. Defining MSE_SCOPEPOINTER_RUNTIME_CHECKS_ENABLED will cause
	mse::us::TFLRegisteredObj to be used in non-debug modes as well. */
	template<typename _TROy>
	class TXScopeAtomicObj : public mse::us::impl::TXScopeAtomicObjBase<_TROy>
		, public mse::impl::first_or_placeholder_if_base_of_second<mse::us::impl::XScopeTagBase, mse::us::impl::TXScopeAtomicObjBase<_TROy>, TXScopeAtomicObj<_TROy> >
		, public mse::impl::first_or_placeholder_if_base_of_second<mse::us::impl::ReferenceableByScopePointerTagBase, mse::us::impl::TXScopeAtomicObjBase<_TROy>, TXScopeAtomicObj<_TROy> >
	{
	public:
		TXScopeAtomicObj(const TXScopeAtomicObj& _X) = default;

#ifdef MSE_SCOPE_DISABLE_MOVE_RESTRICTIONS
		explicit TXScopeAtomicObj(TXScopeAtomicObj&& _X) : mse::us::impl::TXScopeAtomicObjBase<_TROy>(MSE_FWD(_X)) {}
#endif // !MSE_SCOPE_DISABLE_MOVE_RESTRICTIONS

		MSE_SCOPE_ATOMIC_USING(TXScopeAtomicObj, mse::us::impl::TXScopeAtomicObjBase<_TROy>);
		~TXScopeAtomicObj() {
			mse::impl::T_valid_if_is_marked_as_xscope_shareable_msemsearray<_TROy>();
		}

		TXScopeAtomicObj& operator=(TXScopeAtomicObj&& _X) {
			mse::impl::valid_if_not_rvalue_reference_of_given_type<TXScopeAtomicObj, decltype(_X)>(_X);
			mse::us::impl::TXScopeAtomicObjBase<_TROy>::operator=(MSE_FWD(_X));
			return (*this);
		}
		TXScopeAtomicObj& operator=(const TXScopeAtomicObj& _X) { mse::us::impl::TXScopeAtomicObjBase<_TROy>::operator=(_X); return (*this); }
		template<class _Ty2>
		TXScopeAtomicObj& operator=(_Ty2&& _X) {
			mse::us::impl::TXScopeAtomicObjBase<_TROy>::operator=(MSE_FWD(_X));
			return (*this);
		}
		template<class _Ty2>
		TXScopeAtomicObj& operator=(const _Ty2& _X) { mse::us::impl::TXScopeAtomicObjBase<_TROy>::operator=(_X); return (*this); }

		const TXScopeAtomicFixedPointer<_TROy> operator&() & {
			return *this;
		}
		const TXScopeAtomicFixedConstPointer<_TROy> operator&() const & {
			return *this;
		}
		const TXScopeAtomicItemFixedPointer<_TROy> mse_xscope_ifptr() & { return &(*this); }
		const TXScopeAtomicItemFixedConstPointer<_TROy> mse_xscope_ifptr() const & { return &(*this); }

		TXScopeAtomicCagedItemFixedConstPointerToRValue<_TROy> operator&() && {
			//return TXScopeAtomicItemFixedConstPointer<_TROy>(TXScopeAtomicFixedPointer<_TROy>(&(*static_cast<mse::us::impl::TXScopeAtomicObjBase<_TROy>*>(this))));
			return TXScopeAtomicItemFixedConstPointer<_TROy>(TXScopeAtomicFixedPointer<_TROy>(*this));
		}
		TXScopeAtomicCagedItemFixedConstPointerToRValue<_TROy> operator&() const && {
			return TXScopeAtomicFixedConstPointer<_TROy>(TXScopeAtomicConstPointer<_TROy>(&(*static_cast<const mse::us::impl::TXScopeAtomicObjBase<_TROy>*>(this))));
		}
		const TXScopeAtomicCagedItemFixedConstPointerToRValue<_TROy> mse_xscope_ifptr() && { return &(*this); }
		const TXScopeAtomicCagedItemFixedConstPointerToRValue<_TROy> mse_xscope_ifptr() const && { return &(*this); }

		MSE_INHERIT_ASYNC_SHAREABILITY_AND_PASSABILITY_OF(_TROy);
		void xscope_tag() const {}
		//void xscope_contains_accessible_scope_address_of_operator_tag() const {}
		/* This type can be safely used as a function return value if _TROy is also safely returnable. */
		template<class _Ty2 = std::atomic<_TROy>, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _TROy>::value) && (
			(std::integral_constant<bool, mse::impl::HasXScopeReturnableTagMethod<_Ty2>::Has>()) || (mse::impl::is_potentially_not_xscope<_Ty2>::value)
			)> MSE_IMPL_EIS >
		void xscope_returnable_tag() const {} /* Indication that this type is can be used as a function return value. */

	private:
		void* operator new(size_t size) { return ::operator new(size); }

		friend class TXScopeAtomicOwnerPointer<_TROy>;
	};

	template<typename _Ty>
	auto xscope_aifptr_to(_Ty&& _X) {
		return _X.mse_xscope_ifptr();
	}
	template<typename _Ty>
	auto xscope_aifptr_to(const _Ty& _X) {
		return _X.mse_xscope_ifptr();
	}


	namespace us {
		/* A couple of unsafe functions for internal use. */
		template<typename _Ty>
		TXScopeAtomicItemFixedPointer<_Ty> unsafe_make_xscope_atomic_pointer_to(_Ty& ref);
		template<typename _Ty>
		TXScopeAtomicItemFixedConstPointer<_Ty> unsafe_make_xscope_atomic_const_pointer_to(const _Ty& cref);
	}

	namespace impl {
		/* This template type alias is only used because msvc2017(v15.9.0) crashes if the type expression is used directly. */
		template<class _Ty2, class _TMemberObjectPointer>
		using make_xscope_atomic_pointer_to_member_v2_return_type1 = TXScopeAtomicItemFixedPointer<mse::impl::remove_reference_t<decltype(std::declval<_Ty2>().*std::declval<_TMemberObjectPointer>())> >;
	}

	/* While TXScopeAtomicFixedPointer<> points to a TXScopeAtomicObj<>, TXScopeAtomicItemFixedPointer<> is intended to be able to point to a
	TXScopeAtomicObj<>, any member of a TXScopeAtomicObj<>, or various other items with scope lifetime that, for various reasons, aren't
	declared as TXScopeAtomicObj<>. */
	template<typename _Ty>
	class TXScopeAtomicItemFixedPointer : public mse::us::impl::TXScopeAtomicItemPointerBase<_Ty>, public mse::us::impl::XScopeContainsNonOwningScopeReferenceTagBase, public mse::us::impl::StrongPointerAsyncNotShareableAndNotPassableTagBase {
	public:
		TXScopeAtomicItemFixedPointer(const TXScopeAtomicItemFixedPointer& src_cref) = default;
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TXScopeAtomicItemFixedPointer(const TXScopeAtomicItemFixedPointer<_Ty2>& src_cref) : mse::us::impl::TXScopeAtomicItemPointerBase<_Ty>(static_cast<const mse::us::impl::TXScopeAtomicItemPointerBase<_Ty2>&>(src_cref)) {}

		TXScopeAtomicItemFixedPointer(const TXScopeAtomicFixedPointer<_Ty>& src_cref) : mse::us::impl::TXScopeAtomicItemPointerBase<_Ty>(static_cast<const mse::us::impl::TXScopeAtomicItemPointerBase<_Ty>&>(src_cref)) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TXScopeAtomicItemFixedPointer(const TXScopeAtomicFixedPointer<_Ty2>& src_cref) : mse::us::impl::TXScopeAtomicItemPointerBase<_Ty>(static_cast<const mse::us::impl::TXScopeAtomicItemPointerBase<_Ty2>&>(src_cref)) {}

		//TXScopeAtomicItemFixedPointer(const TXScopeAtomicOwnerPointer<_Ty>& src_cref) : TXScopeAtomicItemFixedPointer(&(*src_cref)) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TXScopeAtomicItemFixedPointer(const TXScopeAtomicOwnerPointer<_Ty2>& src_cref) : TXScopeAtomicItemFixedPointer(&(*src_cref)) {}

		~TXScopeAtomicItemFixedPointer() {}

		operator bool() const { return true; }
		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator std::atomic<_Ty>*() const { return std::addressof(*(*this))/*mse::us::impl::TXScopeAtomicItemPointerBase<_Ty>::operator _Ty*()*/; }
		void xscope_tag() const {}

	private:
		TXScopeAtomicItemFixedPointer(_Ty* ptr) : mse::us::impl::TXScopeAtomicItemPointerBase<_Ty>(ptr) {}
		TXScopeAtomicItemFixedPointer(const mse::us::impl::TXScopeAtomicItemPointerBase<_Ty>& ptr) : mse::us::impl::TXScopeAtomicItemPointerBase<_Ty>(ptr) {}
		TXScopeAtomicItemFixedPointer<_Ty>& operator=(const TXScopeAtomicItemFixedPointer<_Ty>& _Right_cref) = delete;
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		template<class _Ty2> friend TXScopeAtomicItemFixedPointer<_Ty2> us::unsafe_make_xscope_atomic_pointer_to(_Ty2& ref);
	};

	template<typename _Ty>
	class TXScopeAtomicItemFixedConstPointer : public mse::us::impl::TXScopeAtomicItemConstPointerBase<_Ty>, public mse::us::impl::XScopeContainsNonOwningScopeReferenceTagBase, public mse::us::impl::StrongPointerAsyncNotShareableAndNotPassableTagBase {
	public:
		TXScopeAtomicItemFixedConstPointer(const TXScopeAtomicItemFixedConstPointer<_Ty>& src_cref) = default;
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TXScopeAtomicItemFixedConstPointer(const TXScopeAtomicItemFixedConstPointer<_Ty2>& src_cref) : mse::us::impl::TXScopeAtomicItemConstPointerBase<_Ty>(static_cast<const mse::us::impl::TXScopeAtomicItemConstPointerBase<_Ty2>&>(src_cref)) {}

		TXScopeAtomicItemFixedConstPointer(const TXScopeAtomicItemFixedPointer<_Ty>& src_cref) : mse::us::impl::TXScopeAtomicItemConstPointerBase<_Ty>(static_cast<const mse::us::impl::TXScopeAtomicItemPointerBase<_Ty>&>(src_cref)) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TXScopeAtomicItemFixedConstPointer(const TXScopeAtomicItemFixedPointer<_Ty2>& src_cref) : mse::us::impl::TXScopeAtomicItemConstPointerBase<_Ty>(static_cast<const mse::us::impl::TXScopeAtomicItemPointerBase<_Ty2>&>(src_cref)) {}

		TXScopeAtomicItemFixedConstPointer(const TXScopeAtomicFixedConstPointer<_Ty>& src_cref) : mse::us::impl::TXScopeAtomicItemConstPointerBase<_Ty>(static_cast<const mse::us::impl::TXScopeAtomicItemConstPointerBase<_Ty>&>(src_cref)) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TXScopeAtomicItemFixedConstPointer(const TXScopeAtomicFixedConstPointer<_Ty2>& src_cref) : mse::us::impl::TXScopeAtomicItemConstPointerBase<_Ty>(static_cast<const mse::us::impl::TXScopeAtomicItemConstPointerBase<_Ty2>&>(src_cref)) {}

		TXScopeAtomicItemFixedConstPointer(const TXScopeAtomicFixedPointer<_Ty>& src_cref) : mse::us::impl::TXScopeAtomicItemConstPointerBase<_Ty>(static_cast<const mse::us::impl::TXScopeAtomicItemPointerBase<_Ty>&>(src_cref)) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TXScopeAtomicItemFixedConstPointer(const TXScopeAtomicFixedPointer<_Ty2>& src_cref) : mse::us::impl::TXScopeAtomicItemConstPointerBase<_Ty>(static_cast<const mse::us::impl::TXScopeAtomicItemPointerBase<_Ty2>&>(src_cref)) {}

		//TXScopeAtomicItemFixedConstPointer(const TXScopeAtomicOwnerPointer<_Ty>& src_cref) : TXScopeAtomicItemFixedConstPointer(&(*src_cref)) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TXScopeAtomicItemFixedConstPointer(const TXScopeAtomicOwnerPointer<_Ty2>& src_cref) : TXScopeAtomicItemFixedConstPointer(&(*src_cref)) {}

		~TXScopeAtomicItemFixedConstPointer() {}

		operator bool() const { return true; }
		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator const std::atomic<_Ty>*() const { return std::addressof(*(*this))/*mse::us::impl::TXScopeAtomicItemConstPointerBase<_Ty>::operator const _Ty*()*/; }
		void xscope_tag() const {}

	private:
		TXScopeAtomicItemFixedConstPointer(const _Ty* ptr) : mse::us::impl::TXScopeAtomicItemConstPointerBase<_Ty>(ptr) {}
		TXScopeAtomicItemFixedConstPointer(const mse::us::impl::TXScopeAtomicItemConstPointerBase<_Ty>& ptr) : mse::us::impl::TXScopeAtomicItemConstPointerBase<_Ty>(ptr) {}
		TXScopeAtomicItemFixedConstPointer<_Ty>& operator=(const TXScopeAtomicItemFixedConstPointer<_Ty>& _Right_cref) = delete;
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		template<class _Ty2> friend TXScopeAtomicItemFixedConstPointer<_Ty2> us::unsafe_make_xscope_atomic_const_pointer_to(const _Ty2& cref);
	};

	/* TXScopeAtomicCagedItemFixedPointerToRValue<> holds a TXScopeAtomicItemFixedPointer<> that points to an r-value which can only be
	accessed when converted to a rsv::TXScopeAtomicItemFixedPointerFParam<>. */
	template<typename _Ty>
	class TXScopeAtomicCagedItemFixedPointerToRValue : public mse::us::impl::XScopeContainsNonOwningScopeReferenceTagBase, public mse::us::impl::StrongPointerAsyncNotShareableAndNotPassableTagBase {
	public:
		void xscope_tag() const {}

	private:
		TXScopeAtomicCagedItemFixedPointerToRValue(const TXScopeAtomicCagedItemFixedPointerToRValue&) = delete;
		TXScopeAtomicCagedItemFixedPointerToRValue(TXScopeAtomicCagedItemFixedPointerToRValue&&) = default;
		TXScopeAtomicCagedItemFixedPointerToRValue(const TXScopeAtomicItemFixedPointer<_Ty>& ptr) : m_xscope_ptr(ptr) {}
#ifdef MSE_SCOPE_DISABLE_MOVE_RESTRICTIONS
		TXScopeAtomicCagedItemFixedPointerToRValue(TXScopeAtomicCagedItemFixedPointerToRValue&& src_ref) : m_xscope_ptr(src_ref) {}
#endif // !MSE_SCOPE_DISABLE_MOVE_RESTRICTIONS

		auto uncaged_pointer() const {
			return m_xscope_ptr;
		}

		TXScopeAtomicCagedItemFixedPointerToRValue<_Ty>& operator=(const TXScopeAtomicCagedItemFixedPointerToRValue<_Ty>& _Right_cref) = delete;
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		TXScopeAtomicItemFixedPointer<_Ty> m_xscope_ptr;

		friend class TXScopeAtomicObj<_Ty>;
		template<class _Ty2> friend class rsv::TXScopeAtomicItemFixedPointerFParam;
		template<class _Ty2> friend class rsv::TXScopeAtomicItemFixedConstPointerFParam;
	};

	template<typename _Ty>
	class TXScopeAtomicCagedItemFixedConstPointerToRValue : public mse::us::impl::XScopeContainsNonOwningScopeReferenceTagBase, public mse::us::impl::StrongPointerAsyncNotShareableAndNotPassableTagBase {
	public:
		void xscope_tag() const {}

	private:
		TXScopeAtomicCagedItemFixedConstPointerToRValue(const TXScopeAtomicCagedItemFixedConstPointerToRValue& src_cref) = delete;
		TXScopeAtomicCagedItemFixedConstPointerToRValue(TXScopeAtomicCagedItemFixedConstPointerToRValue&& src_cref) = default;
		TXScopeAtomicCagedItemFixedConstPointerToRValue(const TXScopeAtomicItemFixedConstPointer<_Ty>& ptr) : m_xscope_ptr(ptr) {}
#ifdef MSE_SCOPE_DISABLE_MOVE_RESTRICTIONS
		TXScopeAtomicCagedItemFixedConstPointerToRValue(TXScopeAtomicCagedItemFixedConstPointerToRValue&& src_ref) : m_xscope_ptr(src_ref) {}
#endif // !MSE_SCOPE_DISABLE_MOVE_RESTRICTIONS

		auto uncaged_pointer() const { return m_xscope_ptr; }

		TXScopeAtomicCagedItemFixedConstPointerToRValue<_Ty>& operator=(const TXScopeAtomicCagedItemFixedConstPointerToRValue<_Ty>& _Right_cref) = delete;
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		TXScopeAtomicItemFixedConstPointer<_Ty> m_xscope_ptr;

		friend class TXScopeAtomicObj<_Ty>;
		template<class _Ty2> friend class rsv::TXScopeAtomicItemFixedConstPointerFParam;
		template<typename _Ty2> friend auto pointer_to(_Ty2&& _X) -> decltype(&std::forward<_Ty2>(_X));
	};
}

namespace std {
	template<class _Ty>
	struct hash<mse::TXScopeAtomicItemFixedPointer<_Ty> > {	// hash functor
		typedef mse::TXScopeAtomicItemFixedPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TXScopeAtomicItemFixedPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
	template<class _Ty>
	struct hash<mse::TXScopeAtomicFixedPointer<_Ty> > {	// hash functor
		typedef mse::TXScopeAtomicFixedPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TXScopeAtomicFixedPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};

	template<class _Ty>
	struct hash<mse::TXScopeAtomicItemFixedConstPointer<_Ty> > {	// hash functor
		typedef mse::TXScopeAtomicItemFixedConstPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TXScopeAtomicItemFixedConstPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
	template<class _Ty>
	struct hash<mse::TXScopeAtomicFixedConstPointer<_Ty> > {	// hash functor
		typedef mse::TXScopeAtomicFixedConstPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TXScopeAtomicFixedConstPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
}

namespace mse {

#endif /*MSE_SCOPEPOINTER_DISABLED*/

	namespace impl {
		template<typename _Ty, class... Args>
		auto make_xscope_atomic_helper(std::true_type, Args&&... args) {
			return _Ty(std::forward<Args>(args)...);
		}
		template<typename _Ty, class... Args>
		auto make_xscope_atomic_helper(std::false_type, Args&&... args) {
			return TXScopeAtomicObj<_Ty>(std::forward<Args>(args)...);
		}
	}
	template <class X, class... Args>
	auto make_xscope_atomic(Args&&... args) {
		typedef mse::impl::remove_reference_t<X> nrX;
		return impl::make_xscope_atomic_helper<nrX>(typename mse::impl::is_instantiation_of<nrX, TXScopeAtomicObj>::type(), std::forward<Args>(args)...);
	}
	template <class X>
	auto make_xscope_atomic(const X& arg) {
		typedef mse::impl::remove_reference_t<X> nrX;
		return impl::make_xscope_atomic_helper<nrX>(typename mse::impl::is_instantiation_of<nrX, TXScopeAtomicObj>::type(), arg);
	}
	template <class X, class = MSE_IMPL_ENABLE_IF_NOT_RETURNABLE_FPARAM(X)>
	auto make_xscope_atomic(X&& arg) {
		typedef mse::impl::remove_reference_t<X> nrX;
		return impl::make_xscope_atomic_helper<nrX>(typename mse::impl::is_instantiation_of<nrX, TXScopeAtomicObj>::type(), MSE_FWD(arg));
	}
	/* Overloads for rsv::TReturnableFParam<>. */
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_atomic)

	namespace us {
		namespace impl {
#ifdef MSE_SCOPEPOINTER_DISABLED
			template<typename _Ty> using TXScopeAtomicItemFixedPointerFParamBase = mse::us::impl::TPointer<_Ty>;
			template<typename _Ty> using TXScopeAtomicItemFixedConstPointerFParamBase = mse::us::impl::TPointer<const _Ty>;
#else /*MSE_SCOPEPOINTER_DISABLED*/
			template<typename _Ty> using TXScopeAtomicItemFixedPointerFParamBase = TXScopeAtomicItemFixedPointer<_Ty>;
			template<typename _Ty> using TXScopeAtomicItemFixedConstPointerFParamBase = TXScopeAtomicItemFixedConstPointer<_Ty>;
#endif /*MSE_SCOPEPOINTER_DISABLED*/
		}
	}

	namespace rsv {
		/* TXScopeAtomicItemFixedPointerFParam<> is just a version of TXScopeAtomicItemFixedPointer<> which may only be used for
		function parameter declations. It has the extra ability to accept (caged) scope pointers to r-value scope objects
		(i.e. supports temporaries by scope reference). */

		template<typename _Ty>
		class TXScopeAtomicItemFixedPointerFParam : public mse::us::impl::TXScopeAtomicItemFixedPointerFParamBase<_Ty> {
		public:
			typedef mse::us::impl::TXScopeAtomicItemFixedPointerFParamBase<_Ty> base_class;
			MSE_SCOPE_ATOMIC_USING(TXScopeAtomicItemFixedPointerFParam, base_class);

			TXScopeAtomicItemFixedPointerFParam(const TXScopeAtomicItemFixedPointerFParam& src_cref) = default;

#ifndef MSE_SCOPEPOINTER_DISABLED
			template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
			TXScopeAtomicItemFixedPointerFParam(const TXScopeAtomicCagedItemFixedPointerToRValue<_Ty2>& src_cref) : base_class(src_cref.uncaged_pointer()) {}
#endif //!MSE_SCOPEPOINTER_DISABLED

			~TXScopeAtomicItemFixedPointerFParam() {}

			void xscope_not_returnable_tag() const {}
			void xscope_tag() const {}

		private:
			TXScopeAtomicItemFixedPointerFParam<_Ty>& operator=(const TXScopeAtomicItemFixedPointerFParam<_Ty>& _Right_cref) = delete;
			MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
		};

		template<typename _Ty>
		class TXScopeAtomicItemFixedConstPointerFParam : public mse::us::impl::TXScopeAtomicItemFixedConstPointerFParamBase<_Ty> {
		public:
			typedef mse::us::impl::TXScopeAtomicItemFixedConstPointerFParamBase<_Ty> base_class;
			MSE_SCOPE_ATOMIC_USING(TXScopeAtomicItemFixedConstPointerFParam, base_class);

			TXScopeAtomicItemFixedConstPointerFParam(const TXScopeAtomicItemFixedConstPointerFParam<_Ty>& src_cref) = default;
			TXScopeAtomicItemFixedConstPointerFParam(const TXScopeAtomicItemFixedPointerFParam<_Ty>& src_cref) : base_class(src_cref) {}

#ifndef MSE_SCOPEPOINTER_DISABLED
			template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
			TXScopeAtomicItemFixedConstPointerFParam(const TXScopeAtomicCagedItemFixedConstPointerToRValue<_Ty2>& src_cref) : base_class(src_cref.uncaged_pointer()) {}
			template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
			TXScopeAtomicItemFixedConstPointerFParam(const TXScopeAtomicCagedItemFixedPointerToRValue<_Ty2>& src_cref) : base_class(src_cref.uncaged_pointer()) {}
#endif //!MSE_SCOPEPOINTER_DISABLED

			~TXScopeAtomicItemFixedConstPointerFParam() {}

			void xscope_not_returnable_tag() const {}
			void xscope_tag() const {}

		private:
			TXScopeAtomicItemFixedConstPointerFParam<_Ty>& operator=(const TXScopeAtomicItemFixedConstPointerFParam<_Ty>& _Right_cref) = delete;
			MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
		};
	}

	namespace rsv {

		/* Template specializations of TFParam<>. There are a number of them. */

#ifndef MSE_SCOPEPOINTER_DISABLED
		template<typename _Ty>
		class TFParam<mse::TXScopeAtomicItemFixedConstPointer<_Ty> > : public TXScopeAtomicItemFixedConstPointerFParam<_Ty> {
		public:
			typedef TXScopeAtomicItemFixedConstPointerFParam<_Ty> base_class;
			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TFParam, base_class);
			void xscope_not_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};

		template<typename _Ty>
		class TFParam<const mse::TXScopeAtomicItemFixedConstPointer<_Ty> > : public TXScopeAtomicItemFixedConstPointerFParam<_Ty> {
		public:
			typedef TXScopeAtomicItemFixedConstPointerFParam<_Ty> base_class;
			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TFParam, base_class);
			void xscope_not_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};

		template<typename _Ty>
		class TFParam<mse::TXScopeAtomicCagedItemFixedConstPointerToRValue<_Ty> > : public TXScopeAtomicItemFixedConstPointerFParam<_Ty> {
		public:
			typedef TXScopeAtomicItemFixedConstPointerFParam<_Ty> base_class;
			MSE_USING_AND_DEFAULT_COPY_AND_MOVE_CONSTRUCTOR_DECLARATIONS(TFParam, base_class);
			void xscope_not_returnable_tag() const {}
			void xscope_tag() const {}
		private:
			MSE_USING_ASSIGNMENT_OPERATOR_AND_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION(base_class);
		};
#endif //!MSE_SCOPEPOINTER_DISABLED
	}

	/* TXScopeAtomicOwnerPointer is meant to be much like boost::scoped_ptr<>. Instead of taking a native pointer,
	TXScopeAtomicOwnerPointer just forwards it's constructor arguments to the constructor of the TXScopeAtomicObj<_Ty>.
	TXScopeAtomicOwnerPointers are meant to be allocated on the stack only. Unfortunately there's really no way to
	enforce this, which makes this data type less intrinsically safe than say, "reference counting" pointers.
	*/
	template<typename _Ty>
	class TXScopeAtomicOwnerPointer : public mse::us::impl::XScopeTagBase, public mse::us::impl::StrongPointerAsyncNotShareableAndNotPassableTagBase
		, MSE_INHERIT_XSCOPE_TAG_BASE_SET_FROM(_Ty, TXScopeAtomicOwnerPointer<_Ty>)
	{
	public:
		TXScopeAtomicOwnerPointer(TXScopeAtomicOwnerPointer<_Ty>&& src_ref) = default;

		template <class... Args>
		TXScopeAtomicOwnerPointer(Args&&... args) {
			/* In the case where there is exactly one argument and its type is derived from this type, we want to
			act like a move constructor here. We use a helper function to check for this case and act accordingly. */
			constructor_helper1(std::forward<Args>(args)...);
		}

		TXScopeAtomicObj<_Ty>& operator*() const {
			return (*m_ptr);
		}
		TXScopeAtomicObj<_Ty>* operator->() const {
			return std::addressof(*m_ptr);
		}

#ifdef MSE_SCOPEPOINTER_DISABLED
		operator _Ty*() const {
			return std::addressof(*(*this));
		}
		explicit operator const _Ty*() const {
			return std::addressof(*(*this));
		}
#endif /*MSE_SCOPEPOINTER_DISABLED*/

		template <class... Args>
		static TXScopeAtomicOwnerPointer make(Args&&... args) {
			return TXScopeAtomicOwnerPointer(std::forward<Args>(args)...);
		}

		void xscope_tag() const {}
		/* This type can be safely used as a function return value if _TROy is also safely returnable. */
		template<class _Ty2 = _Ty, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_Ty2, _Ty>::value) && (
			(std::integral_constant<bool, mse::impl::HasXScopeReturnableTagMethod<_Ty2>::Has>()) || (mse::impl::is_potentially_not_xscope<_Ty2>::value)
			)> MSE_IMPL_EIS >
			void xscope_returnable_tag() const {} /* Indication that this type is can be used as a function return value. */

	private:
		/* construction helper functions */
		template <class... Args>
		void initialize(Args&&... args) {
			/* We can't use std::make_unique<> because TXScopeAtomicObj<>'s "operator new()" is private and inaccessible to
			std::make_unique<> (which is not a friend of TXScopeAtomicObj<> like we are). */
			auto new_ptr = new TXScopeAtomicObj<_Ty>(std::forward<Args>(args)...);
			m_ptr.reset(new_ptr);
		}
		template <class _TSoleArg>
		void constructor_helper2(std::true_type, _TSoleArg&& sole_arg) {
			/* The sole parameter is derived from, or of this type, so we're going to consider the constructor
			a move constructor. */
			m_ptr = MSE_FWD(sole_arg.m_ptr);
		}
		template <class _TSoleArg>
		void constructor_helper2(std::false_type, _TSoleArg&& sole_arg) {
			/* The sole parameter is not derived from, or of this type, so the constructor is not a move
			constructor. */
			initialize(MSE_FWD(sole_arg));
		}
		template <class... Args>
		void constructor_helper1(Args&&... args) {
			initialize(std::forward<Args>(args)...);
		}
		template <class _TSoleArg>
		void constructor_helper1(_TSoleArg&& sole_arg) {
			/* The constructor was given exactly one parameter. If the parameter is derived from, or of this type,
			then we're going to consider the constructor a move constructor. */
			constructor_helper2(typename std::is_base_of<TXScopeAtomicOwnerPointer, _TSoleArg>::type(), MSE_FWD(sole_arg));
		}

		TXScopeAtomicOwnerPointer(const TXScopeAtomicOwnerPointer<_Ty>& src_cref) = delete;
		TXScopeAtomicOwnerPointer<_Ty>& operator=(const TXScopeAtomicOwnerPointer<_Ty>& _Right_cref) = delete;
		void* operator new(size_t size) { return ::operator new(size); }

		std::unique_ptr<TXScopeAtomicObj<_Ty> > m_ptr = nullptr;
	};

	template <class X, class... Args>
	TXScopeAtomicOwnerPointer<X> make_xscope_atomic_owner(Args&&... args) {
		return TXScopeAtomicOwnerPointer<X>::make(std::forward<Args>(args)...);
	}
}

namespace std {
	template<class _Ty>
	struct hash<mse::TXScopeAtomicOwnerPointer<_Ty> > {	// hash functor
		typedef mse::TXScopeAtomicOwnerPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TXScopeAtomicOwnerPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
}

namespace mse {

	namespace us {
		/* (Unsafely) obtain a scope pointer to any object. */
		template<typename _Ty>
		TXScopeAtomicItemFixedPointer<_Ty> unsafe_make_xscope_atomic_pointer_to(_Ty& ref) {
			return TXScopeAtomicItemFixedPointer<_Ty>(std::addressof(ref));
		}
		template<typename _Ty>
		TXScopeAtomicItemFixedConstPointer<_Ty> unsafe_make_xscope_atomic_const_pointer_to(const _Ty& cref) {
			return TXScopeAtomicItemFixedConstPointer<_Ty>(std::addressof(cref));
		}
		template<typename _Ty>
		TXScopeAtomicItemFixedConstPointer<_Ty> unsafe_make_xscope_atomic_pointer_to(const _Ty& cref) {
			return unsafe_make_xscope_atomic_const_pointer_to(cref);
		}
	}

	template<typename _Ty> using TXScopeXScopeAtomicItemFixedStore = TXScopeStrongNotNullPointerStore<TXScopeAtomicItemFixedPointer<_Ty> >;
	template<typename _Ty> using TXScopeXScopeAtomicItemFixedConstStore = TXScopeStrongNotNullConstPointerStore<TXScopeAtomicItemFixedConstPointer<_Ty> >;
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<typename _Ty> using TXScopeXScopeAtomicFixedStore = TXScopeStrongNotNullPointerStore<TXScopeAtomicFixedPointer<_Ty> >;
	template<typename _Ty> using TXScopeXScopeAtomicFixedConstStore = TXScopeStrongNotNullConstPointerStore<TXScopeAtomicFixedConstPointer<_Ty> >;
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)


	/* overloads of xscope_chosen_pointer() */
	template<typename _TBoolFunction, typename _Ty, class... Args>
	const auto& xscope_chosen_pointer(_TBoolFunction function1, const TXScopeAtomicItemFixedConstPointer<_Ty>& a, const TXScopeAtomicItemFixedConstPointer<_Ty>& b, Args&&... args) {
		return function1(a, b, std::forward<Args>(args)...) ? b : a;
	}
	template<typename _TBoolFunction, typename _Ty, class... Args>
	const auto& xscope_chosen_pointer(_TBoolFunction function1, const TXScopeAtomicItemFixedPointer<_Ty>& a, const TXScopeAtomicItemFixedPointer<_Ty>& b, Args&&... args) {
		return function1(a, b, std::forward<Args>(args)...) ? b : a;
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<typename _TBoolFunction, typename _Ty, class... Args>
	const auto& xscope_chosen_pointer(_TBoolFunction function1, const TXScopeAtomicFixedConstPointer<_Ty>& a, const TXScopeAtomicFixedConstPointer<_Ty>& b, Args&&... args) {
		return function1(a, b, std::forward<Args>(args)...) ? b : a;
	}
	template<typename _TBoolFunction, typename _Ty, class... Args>
	const auto& xscope_chosen_pointer(_TBoolFunction function1, const TXScopeAtomicFixedPointer<_Ty>& a, const TXScopeAtomicFixedPointer<_Ty>& b, Args&&... args) {
		return function1(a, b, std::forward<Args>(args)...) ? b : a;
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	template<typename _Ty>
	const auto& xscope_chosen_pointer(bool choose_the_second, const TXScopeAtomicItemFixedConstPointer<_Ty>& a, const TXScopeAtomicItemFixedConstPointer<_Ty>& b) {
		return choose_the_second ? b : a;
	}
	template<typename _Ty>
	const auto& xscope_chosen_pointer(bool choose_the_second, const TXScopeAtomicItemFixedPointer<_Ty>& a, const TXScopeAtomicItemFixedPointer<_Ty>& b) {
		return choose_the_second ? b : a;
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template<typename _Ty>
	const auto& xscope_chosen_pointer(bool choose_the_second, const TXScopeAtomicFixedConstPointer<_Ty>& a, const TXScopeAtomicFixedConstPointer<_Ty>& b) {
		return choose_the_second ? b : a;
	}
	template<typename _Ty>
	const auto& xscope_chosen_pointer(bool choose_the_second, const TXScopeAtomicFixedPointer<_Ty>& a, const TXScopeAtomicFixedPointer<_Ty>& b) {
		return choose_the_second ? b : a;
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)


	/* shorter aliases */
	template<typename _Ty> using safp = TXScopeAtomicFixedPointer<_Ty>;
	template<typename _Ty> using safcp = TXScopeAtomicFixedConstPointer<_Ty>;
	template<typename _TROy> using sao = TXScopeAtomicObj<_TROy>;
	template<typename _Ty> using saifp = TXScopeAtomicItemFixedPointer<_Ty>;
	template<typename _Ty> using saifcp = TXScopeAtomicItemFixedConstPointer<_Ty>;


	namespace self_test {
		class CXScpAtmcPtrTest1 {
		public:
			static void s_test1() {
#ifdef MSE_SELF_TESTS
				class A {
				public:
					A(int x) : b(x) {}
					A(const A& _X) = default;

					int b = 3;
				};
				/* Here we're declaring that A can be safely shared between asynchronous threads. */
				typedef mse::rsv::TAsyncShareableAndPassableObj<A> shareable_A_t;

				class B {
				public:
					static int foo1(std::atomic<shareable_A_t>* a_native_ptr) { return a_native_ptr->load().b; }
					static int foo2(mse::TXScopeAtomicItemFixedPointer<shareable_A_t> A_scope_ptr) { return A_scope_ptr->load().b; }
				protected:
					~B() {}
				};

				std::atomic<shareable_A_t>* A_native_ptr = nullptr;

				{
					std::atomic<shareable_A_t> a(7);
					mse::TXScopeAtomicObj<shareable_A_t> scope_a(7);
					/* mse::TXScopeAtomicObj<shareable_A_t> is a class that is publicly derived from A, and so should be a compatible substitute for A
					in almost all cases. */

					assert(a.load().b == scope_a.load().b);
					A_native_ptr = &a;

					mse::TXScopeAtomicItemFixedPointer<shareable_A_t> A_scope_ptr1(&scope_a);
					assert(A_native_ptr->load().b == A_scope_ptr1->load().b);
					mse::TXScopeAtomicItemFixedPointer<shareable_A_t> A_scope_ptr2 = &scope_a;

					if (!A_scope_ptr2) {
						assert(false);
					}
					else if (!(A_scope_ptr2 != A_scope_ptr1)) {
						int q = B::foo2(A_scope_ptr2);
					}
					else {
						assert(false);
					}

					mse::us::impl::TPointerForLegacy<std::atomic<shareable_A_t> > pfl_ptr1 = &a;
					if (!(pfl_ptr1 != nullptr)) {
						assert(false);
					}
					mse::us::impl::TPointerForLegacy<std::atomic<shareable_A_t> > pfl_ptr2 = nullptr;
					if (!(pfl_ptr1 != pfl_ptr2)) {
						assert(false);
					}

					std::atomic<shareable_A_t> a2(a.load());
					mse::TXScopeAtomicObj<shareable_A_t> scope_a2(scope_a.load());
					scope_a2 = a.load();
					scope_a2 = scope_a.load();

					mse::TXScopeAtomicItemFixedConstPointer<shareable_A_t> rcp = A_scope_ptr1;
					mse::TXScopeAtomicItemFixedConstPointer<shareable_A_t> rcp2 = rcp;
					const mse::TXScopeAtomicObj<shareable_A_t> cscope_a(11);
					mse::TXScopeAtomicItemFixedConstPointer<shareable_A_t> rfcp = &cscope_a;

					mse::TXScopeAtomicOwnerPointer<shareable_A_t> A_scpoptr(11);
					//B::foo2(A_scpoptr);
					B::foo2(&*A_scpoptr);
					if (A_scpoptr->load().b == (&*A_scpoptr)->load().b) {
					}
				}

				{
					class A {
					public:
						A(int x) : b(x) {}

						int b = 3;
					};
					typedef mse::rsv::TAsyncShareableAndPassableObj<A> shareable_A_t;

					class B {
					public:
						static int foo1(std::atomic<shareable_A_t>* a_native_ptr) { return a_native_ptr->load().b; }
						static int foo2(mse::TXScopeAtomicItemFixedPointer<shareable_A_t> A_scpfptr) { return A_scpfptr->load().b; }
						static int foo3(mse::TXScopeAtomicItemFixedConstPointer<shareable_A_t> A_scpfcptr) { return A_scpfcptr->load().b; }
					protected:
						~B() {}
					};

					mse::TXScopeAtomicObj<shareable_A_t> a_scpobj(5);
					int res1 = (&a_scpobj)->load().b;
					int res2 = B::foo2(&a_scpobj);
					int res3 = B::foo3(&a_scpobj);
					mse::TXScopeAtomicOwnerPointer<shareable_A_t> a_scpoptr(7);
					//int res4 = B::foo2(a_scpoptr);
					int res4b = B::foo2(&(*a_scpoptr));
				}

				{
					std::atomic<shareable_A_t> a(7);
					mse::TXScopeAtomicObj<shareable_A_t> scope_a(7);
					/* mse::TXScopeAtomicObj<shareable_A_t> is a class that is publicly derived from A, and so should be a compatible substitute for A
					in almost all cases. */

					assert(a.load().b == scope_a.load().b);
					A_native_ptr = &a;

					mse::TXScopeAtomicFixedPointer<shareable_A_t> A_scope_ptr1 = &scope_a;
					assert(A_native_ptr->load().b == A_scope_ptr1->load().b);
					mse::TXScopeAtomicFixedPointer<shareable_A_t> A_scope_ptr2 = &scope_a;

					/* mse::TXScopeAtomicFixedPointers can be coerced into native pointers if you need to interact with legacy code or libraries. */
					//B::foo1(static_cast<std::atomic<shareable_A_t>*>(A_scope_ptr1));

					if (!A_scope_ptr2) {
						assert(false);
					}
					else if (!(A_scope_ptr2 != A_scope_ptr1)) {
						int q = B::foo2(A_scope_ptr2);
					}
					else {
						assert(false);
					}

					std::atomic<shareable_A_t> a2(a.load());
					mse::TXScopeAtomicObj<shareable_A_t> scope_a2(scope_a.load());
					scope_a2 = a.load();
					scope_a2 = scope_a.load();

					mse::TXScopeAtomicFixedConstPointer<shareable_A_t> rcp = A_scope_ptr1;
					mse::TXScopeAtomicFixedConstPointer<shareable_A_t> rcp2 = rcp;
					const mse::TXScopeAtomicObj<shareable_A_t> cscope_a(11);
					mse::TXScopeAtomicFixedConstPointer<shareable_A_t> rfcp = &cscope_a;

					mse::TXScopeAtomicOwnerPointer<shareable_A_t> A_scpoptr(11);
					//B::foo2(A_scpoptr);
					B::foo2(&*A_scpoptr);
					if (A_scpoptr->load().b == (&*A_scpoptr)->load().b) {
					}
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

#endif // MSESCOPEATOMIC_H_
