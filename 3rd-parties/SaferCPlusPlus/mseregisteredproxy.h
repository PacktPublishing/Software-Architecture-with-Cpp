
// Copyright (c) 2015 Noah Lopez
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef MSEREGISTEREDPROXY_H_
#define MSEREGISTEREDPROXY_H_

#include "msescope.h"
#include "mseregistered.h"
#ifdef MSE_SELF_TESTS
#include <string>
#endif // MSE_SELF_TESTS


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

#ifdef _MSC_VER
#pragma warning( push )  
#pragma warning( disable : 4100 4456 4189 )
#endif /*_MSC_VER*/

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#else /*__clang__*/
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif /*__GNUC__*/
#endif /*__clang__*/


namespace mse {

	template<typename _Ty> class TNDXScopeRegisteredProxyObj;
	template<typename _Ty> class TNDXScopeRegisteredConstProxyObj;
	template<typename _Ty> class TNDRegisteredProxyPointer;
	template<typename _Ty> class TNDRegisteredProxyConstPointer;
	template<typename _Ty> class TNDRegisteredProxyNotNullPointer;
	template<typename _Ty> class TNDRegisteredProxyNotNullConstPointer;
	template<typename _Ty> class TNDRegisteredProxyFixedPointer;
	template<typename _Ty> class TNDRegisteredProxyFixedConstPointer;

	template<typename _Ty>
	auto ndregistered_proxy_fptr(_Ty&& _X) {
		return _X.mse_registered_proxy_fptr();
	}
	template<typename _Ty>
	auto ndregistered_proxy_fptr(const _Ty& _X) {
		return _X.mse_registered_proxy_fptr();
	}

	template <class X>
	auto make_xscope_ndregistered_proxy(const mse::TXScopeFixedPointer<X>& xs_ptr) {
		return TNDXScopeRegisteredProxyObj<X>(xs_ptr);
	}
	template <class X>
	auto make_xscope_ndregistered_proxy(const mse::TXScopeFixedConstPointer<X>& xs_ptr) {
		return TNDXScopeRegisteredConstProxyObj<X>(xs_ptr);
	}
	template <class X>
	auto make_xscope_ndregistered_const_proxy(const mse::TXScopeFixedConstPointer<X>& xs_ptr) {
		return TNDXScopeRegisteredConstProxyObj<X>(xs_ptr);
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template <class X>
	auto make_xscope_ndregistered_proxy(const mse::TXScopeObjFixedPointer<X>& xs_ptr) {
		return TNDXScopeRegisteredProxyObj<X>(xs_ptr);
	}
	template <class X>
	auto make_xscope_ndregistered_proxy(const mse::TXScopeObjFixedConstPointer<X>& xs_ptr) {
		return TNDXScopeRegisteredConstProxyObj<X>(xs_ptr);
	}
	template <class X>
	auto make_xscope_ndregistered_const_proxy(const mse::TXScopeObjFixedConstPointer<X>& xs_ptr) {
		return TNDXScopeRegisteredConstProxyObj<X>(xs_ptr);
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

#ifdef MSE_HAS_CXX17
	/* deduction guide */
	template<class _TROy> TNDXScopeRegisteredProxyObj(_TROy)->TNDXScopeRegisteredProxyObj<_TROy>;
	template<class _TROy> TNDXScopeRegisteredConstProxyObj(_TROy)->TNDXScopeRegisteredConstProxyObj<_TROy>;
#endif /* MSE_HAS_CXX17 */

	template<typename _Ty> using TRegisteredProxyPointer = TNDRegisteredProxyPointer<_Ty>;
	template<typename _Ty> using TRegisteredProxyConstPointer = TNDRegisteredProxyConstPointer<_Ty>;
	template<typename _Ty> using TRegisteredProxyNotNullPointer = TNDRegisteredProxyNotNullPointer<_Ty>;
	template<typename _Ty> using TRegisteredProxyNotNullConstPointer = TNDRegisteredProxyNotNullConstPointer<_Ty>;
	template<typename _Ty> using TRegisteredProxyFixedPointer = TNDRegisteredProxyFixedPointer<_Ty>;
	template<typename _Ty> using TRegisteredProxyFixedConstPointer = TNDRegisteredProxyFixedConstPointer<_Ty>;
	template<typename _TROy> using TXScopeRegisteredProxyObj = TNDXScopeRegisteredProxyObj<_TROy>;
	template<typename _TROy> using TXScopeRegisteredConstProxyObj = TNDXScopeRegisteredConstProxyObj<_TROy>;

	template<typename _Ty> auto registered_proxy_fptr(_Ty&& _X) { return ndregistered_proxy_fptr(MSE_FWD(_X)); }
	template<typename _Ty> auto registered_proxy_fptr(const _Ty& _X) { return ndregistered_proxy_fptr(_X); }

	template <class X>
	auto make_xscope_registered_proxy(const X& arg) {
		return make_xscope_ndregistered_proxy(arg);
	}
	template <class X>
	auto make_xscope_registered_const_proxy(const X& arg) {
		return make_xscope_ndregistered_const_proxy(arg);
	}

	/* TNDRegisteredProxyPointer<_Ty> is just an mse::TNDRegisteredPointer<mse::TXScopeFixedPointer<_Ty> > that
	dereferences to the target of type _Ty rather than the mse::TXScopeFixedPointer<_Ty>. */
	template<typename _Ty>
	class TNDRegisteredProxyPointer : public mse::TNDRegisteredConstPointer<mse::TXScopeFixedPointer<_Ty> > {
	public:
		typedef mse::TNDRegisteredConstPointer<mse::TXScopeFixedPointer<_Ty> > base_class;
		typedef base_class _MP;

		MSE_USING(TNDRegisteredProxyPointer, base_class);
		TNDRegisteredProxyPointer(const TNDRegisteredProxyPointer& src_cref) = default;
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredProxyPointer(const TNDRegisteredProxyPointer<_Ty2>& src_cref) : base_class(src_cref.contained_pointer()) {}
		TNDRegisteredProxyPointer<_Ty>& operator=(const TNDRegisteredProxyPointer<_Ty>& _Right_cref) {
			(*this).contained_pointer() = _Right_cref;
			return (*this);
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredProxyPointer<_Ty>& operator=(const TNDRegisteredProxyPointer<_Ty2>& _Right_cref) {
			(*this).contained_pointer() = (TNDRegisteredProxyPointer(_Right_cref));
			return (*this);
		}

		typedef mse::impl::remove_reference_t<decltype(*(*std::declval<_MP>()))> target_obj_t;
		target_obj_t& operator*() const {
			return (*(*contained_pointer()));
		}
		target_obj_t* operator->() const {
			return std::addressof(*(*contained_pointer()));
		}

		operator bool() const { return bool(contained_pointer()) && bool(*contained_pointer()); }
		operator mse::TXScopeFixedPointer<_Ty>() const {
			return (*contained_pointer());
		}
		operator mse::TXScopeFixedConstPointer<_Ty>() const {
			return (*contained_pointer());
		}

	private:
		TNDRegisteredProxyPointer(const TNDXScopeRegisteredProxyObj<_Ty>* ptr) : base_class(ptr->mse_registered_fptr()) {}

		const _MP& contained_pointer() const& { return (*this); }
		const _MP& contained_pointer() const&& { return (*this); }
		_MP& contained_pointer()& { return (*this); }
		auto&& contained_pointer()&& { return static_cast<_MP&&>(MSE_FWD(*this)); }

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		template <class Y> friend class TNDRegisteredProxyPointer;
		template <class Y> friend class TNDRegisteredProxyConstPointer;
		friend class TNDRegisteredProxyNotNullPointer<_Ty>;
	};

	template<typename _Ty, typename _Tz>
	inline auto operator==(const TNDRegisteredProxyPointer<_Ty>& lhs, const _Tz& rhs) {
		bool retval = false;
		if (lhs) {
			retval = (std::addressof(*lhs) == std::addressof(*rhs));
		}
		else {
			retval = !bool(rhs);
		}
		return retval;
	}
	template<typename _Ty, typename _Tz>
	inline auto operator!=(const TNDRegisteredProxyPointer<_Ty>& lhs, const _Tz& rhs) {
		return !(lhs == rhs);
	}

	template<typename _Ty>
	class TNDRegisteredProxyConstPointer : public mse::TNDRegisteredConstPointer<mse::TXScopeFixedConstPointer<_Ty> > {
	public:
		typedef mse::TNDRegisteredConstPointer<mse::TXScopeFixedConstPointer<_Ty> > base_class;
		typedef base_class _MP;

		MSE_USING(TNDRegisteredProxyConstPointer, base_class);
		TNDRegisteredProxyConstPointer(const TNDRegisteredProxyConstPointer& src_cref) = default;
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredProxyConstPointer(const TNDRegisteredProxyConstPointer<_Ty2>& src_cref) : base_class(src_cref.contained_pointer()) {}
		TNDRegisteredProxyConstPointer(const TNDRegisteredProxyPointer<_Ty>& src_cref) : base_class(RegCPtrXSPtrToRegCPtrXSConstPtr(src_cref.contained_pointer())) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredProxyConstPointer(const TNDRegisteredProxyPointer<_Ty2>& src_cref) : base_class(src_cref.contained_pointer()) {}
		TNDRegisteredProxyConstPointer<_Ty>& operator=(const TNDRegisteredProxyConstPointer<_Ty>& _Right_cref) {
			(*this).contained_pointer() = _Right_cref;
			return (*this);
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredProxyConstPointer<_Ty>& operator=(const TNDRegisteredProxyConstPointer<_Ty2>& _Right_cref) {
			(*this).contained_pointer() = (TNDRegisteredProxyConstPointer(_Right_cref));
			return (*this);
		}

		typedef mse::impl::remove_reference_t<decltype(*(*std::declval<_MP>()))> target_obj_t;
		target_obj_t& operator*() const {
			return (*(*contained_pointer()));
		}
		target_obj_t* operator->() const {
			return std::addressof(*(*contained_pointer()));
		}

		operator bool() const { return bool(contained_pointer()) && bool(*contained_pointer()); }
		operator mse::TXScopeFixedConstPointer<_Ty>() const {
			return (*contained_pointer());
		}

	private:
		TNDRegisteredProxyConstPointer(const TNDXScopeRegisteredProxyObj<const _Ty>* ptr) : base_class(ptr->mse_registered_fptr()) {}
		TNDRegisteredProxyConstPointer(const TNDXScopeRegisteredProxyObj<_Ty>* ptr) : base_class(ptr->mse_registered_fptr()) {}
		TNDRegisteredProxyConstPointer(const TNDXScopeRegisteredConstProxyObj<_Ty>* ptr) : base_class(ptr->mse_registered_fptr()) {}

		const _MP& contained_pointer() const& { return (*this); }
		const _MP& contained_pointer() const&& { return (*this); }
		_MP& contained_pointer()& { return (*this); }
		auto&& contained_pointer()&& { return static_cast<_MP&&>(MSE_FWD(*this)); }

		static auto ProxyObjPtrToProxyConstObjPtr(const TNDXScopeRegisteredProxyObj<_Ty>* ptr) {
			/* Reinterpreting a TNDXScopeRegisteredProxyObj<_Ty> as a TNDXScopeRegisteredProxyObj<const _Ty> should be fine. */
			return reinterpret_cast<const TNDXScopeRegisteredProxyObj<const _Ty>*>(ptr);
		}
		static const auto& RegCPtrXSPtrToRegCPtrXSConstPtr(const mse::TNDRegisteredConstPointer<mse::TXScopeFixedPointer<_Ty> >& src) {
			/* Reinterpreting a mse::TNDRegisteredConstPointer<mse::TXScopeFixedPointer<_Ty> > as an
			mse::TNDRegisteredConstPointer<mse::TXScopeFixedConstPointer<_Ty> > should be fine. */
			return reinterpret_cast<const mse::TNDRegisteredConstPointer<mse::TXScopeFixedConstPointer<_Ty> >&>(src);
		}

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		template <class Y> friend class TNDRegisteredProxyConstPointer;
		friend class TNDRegisteredProxyNotNullConstPointer<_Ty>;
	};

	template<typename _Ty, typename _Tz>
	inline auto operator==(const TNDRegisteredProxyConstPointer<_Ty>& lhs, const _Tz& rhs) {
		bool retval = false;
		if (lhs) {
			retval = (std::addressof(*lhs) == std::addressof(*rhs));
		}
		else {
			retval = !bool(rhs);
		}
		return retval;
	}
	template<typename _Ty, typename _Tz>
	inline auto operator!=(const TNDRegisteredProxyConstPointer<_Ty>& lhs, const _Tz& rhs) {
		return !(lhs == rhs);
	}

	template<typename _Ty>
	class TNDRegisteredProxyNotNullPointer : public TNDRegisteredProxyPointer<_Ty> {
	public:
		TNDRegisteredProxyNotNullPointer(const TNDRegisteredProxyNotNullPointer& src_cref) : TNDRegisteredProxyPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredProxyNotNullPointer(const TNDRegisteredProxyNotNullPointer<_Ty2>& src_cref) : TNDRegisteredProxyPointer<_Ty>(src_cref) {}

		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TNDRegisteredProxyNotNullPointer() {}
		/*
		TNDRegisteredProxyNotNullPointer<_Ty>& operator=(const TNDRegisteredProxyNotNullPointer<_Ty>& _Right_cref) {
		TNDRegisteredProxyPointer<_Ty>::operator=(_Right_cref);
		return (*this);
		}
		*/

	private:
		TNDRegisteredProxyNotNullPointer(const TNDXScopeRegisteredProxyObj<_Ty>* ptr) : TNDRegisteredProxyPointer<_Ty>(ptr) {}

		/* If you want to use this constructor, use not_null_from_nullable() instead. */
		TNDRegisteredProxyNotNullPointer(const  TNDRegisteredProxyPointer<_Ty>& src_cref) : TNDRegisteredProxyPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredProxyNotNullPointer(const TNDRegisteredProxyPointer<_Ty2>& src_cref) : TNDRegisteredProxyPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		friend class TNDRegisteredProxyFixedPointer<_Ty>;
		template<typename _Ty2>
		friend TNDRegisteredProxyNotNullPointer<_Ty2> not_null_from_nullable(const TNDRegisteredProxyPointer<_Ty2>& src);
	};

	template<typename _Ty>
	class TNDRegisteredProxyNotNullConstPointer : public TNDRegisteredProxyConstPointer<_Ty> {
	public:
		TNDRegisteredProxyNotNullConstPointer(const TNDRegisteredProxyNotNullPointer<_Ty>& src_cref) : TNDRegisteredProxyConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredProxyNotNullConstPointer(const TNDRegisteredProxyNotNullPointer<_Ty2>& src_cref) : TNDRegisteredProxyConstPointer<_Ty>(src_cref) {}
		TNDRegisteredProxyNotNullConstPointer(const TNDRegisteredProxyNotNullConstPointer<_Ty>& src_cref) : TNDRegisteredProxyConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredProxyNotNullConstPointer(const TNDRegisteredProxyNotNullConstPointer<_Ty2>& src_cref) : TNDRegisteredProxyConstPointer<_Ty>(src_cref) {}

		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TNDRegisteredProxyNotNullConstPointer() {}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator const _Ty* () const { return TNDRegisteredProxyConstPointer<_Ty>::operator const _Ty * (); }
		MSE_DEPRECATED explicit operator const TNDXScopeRegisteredProxyObj<_Ty>* () const { return TNDRegisteredProxyConstPointer<_Ty>::operator const TNDXScopeRegisteredProxyObj<_Ty> * (); }
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	private:
		TNDRegisteredProxyNotNullConstPointer(const TNDXScopeRegisteredProxyObj<_Ty>* ptr) : TNDRegisteredProxyConstPointer<_Ty>(ptr) {}
		TNDRegisteredProxyNotNullConstPointer(const TNDXScopeRegisteredConstProxyObj<_Ty>* ptr) : TNDRegisteredProxyConstPointer<_Ty>(ptr) {}

		/* If you want to use this constructor, use not_null_from_nullable() instead. */
		TNDRegisteredProxyNotNullConstPointer(const TNDRegisteredProxyPointer<_Ty>& src_cref) : TNDRegisteredProxyConstPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredProxyNotNullConstPointer(const TNDRegisteredProxyPointer<_Ty2>& src_cref) : TNDRegisteredProxyConstPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}
		TNDRegisteredProxyNotNullConstPointer(const TNDRegisteredProxyConstPointer<_Ty>& src_cref) : TNDRegisteredProxyConstPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredProxyNotNullConstPointer(const TNDRegisteredProxyConstPointer<_Ty2>& src_cref) : TNDRegisteredProxyConstPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		friend class TNDRegisteredProxyFixedConstPointer<_Ty>;
		template<typename _Ty2>
		friend TNDRegisteredProxyNotNullConstPointer<_Ty2> not_null_from_nullable(const TNDRegisteredProxyConstPointer<_Ty2>& src);
	};

	template<typename _Ty>
	TNDRegisteredProxyNotNullPointer<_Ty> not_null_from_nullable(const TNDRegisteredProxyPointer<_Ty>& src) {
		return src;
	}
	template<typename _Ty>
	TNDRegisteredProxyNotNullConstPointer<_Ty> not_null_from_nullable(const TNDRegisteredProxyConstPointer<_Ty>& src) {
		return src;
	}

	/* TNDRegisteredProxyFixedPointer cannot be retargeted or constructed without a target. This pointer is recommended for passing
	parameters by reference. */
	template<typename _Ty>
	class TNDRegisteredProxyFixedPointer : public TNDRegisteredProxyNotNullPointer<_Ty> {
	public:
		TNDRegisteredProxyFixedPointer(const TNDRegisteredProxyFixedPointer& src_cref) : TNDRegisteredProxyNotNullPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredProxyFixedPointer(const TNDRegisteredProxyFixedPointer<_Ty2>& src_cref) : TNDRegisteredProxyNotNullPointer<_Ty>(src_cref) {}

		TNDRegisteredProxyFixedPointer(const TNDRegisteredProxyNotNullPointer<_Ty>& src_cref) : TNDRegisteredProxyNotNullPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredProxyFixedPointer(const TNDRegisteredProxyNotNullPointer<_Ty2>& src_cref) : TNDRegisteredProxyNotNullPointer<_Ty>(src_cref) {}

		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TNDRegisteredProxyFixedPointer() {}

#if !defined(MSE_SCOPEPOINTER_DISABLED)
		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator _Ty* () const { return TNDRegisteredProxyNotNullPointer<_Ty>::operator _Ty * (); }
		MSE_DEPRECATED explicit operator TNDXScopeRegisteredProxyObj<_Ty>* () const { return TNDRegisteredProxyNotNullPointer<_Ty>::operator TNDXScopeRegisteredProxyObj<_Ty> * (); }
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	private:
		TNDRegisteredProxyFixedPointer(const TNDXScopeRegisteredProxyObj<_Ty>* ptr) : TNDRegisteredProxyNotNullPointer<_Ty>(ptr) {}
		TNDRegisteredProxyFixedPointer<_Ty>& operator=(const TNDRegisteredProxyFixedPointer<_Ty>& _Right_cref) = delete;

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		friend class TNDXScopeRegisteredProxyObj<_Ty>;
	};

	template<typename _Ty>
	class TNDRegisteredProxyFixedConstPointer : public TNDRegisteredProxyNotNullConstPointer<_Ty> {
	public:
		TNDRegisteredProxyFixedConstPointer(const TNDRegisteredProxyFixedPointer<_Ty>& src_cref) : TNDRegisteredProxyNotNullConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredProxyFixedConstPointer(const TNDRegisteredProxyFixedPointer<_Ty2>& src_cref) : TNDRegisteredProxyNotNullConstPointer<_Ty>(src_cref) {}
		TNDRegisteredProxyFixedConstPointer(const TNDRegisteredProxyFixedConstPointer<_Ty>& src_cref) : TNDRegisteredProxyNotNullConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredProxyFixedConstPointer(const TNDRegisteredProxyFixedConstPointer<_Ty2>& src_cref) : TNDRegisteredProxyNotNullConstPointer<_Ty>(src_cref) {}

		TNDRegisteredProxyFixedConstPointer(const TNDRegisteredProxyNotNullPointer<_Ty>& src_cref) : TNDRegisteredProxyNotNullConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredProxyFixedConstPointer(const TNDRegisteredProxyNotNullPointer<_Ty2>& src_cref) : TNDRegisteredProxyNotNullConstPointer<_Ty>(src_cref) {}
		TNDRegisteredProxyFixedConstPointer(const TNDRegisteredProxyNotNullConstPointer<_Ty>& src_cref) : TNDRegisteredProxyNotNullConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredProxyFixedConstPointer(const TNDRegisteredProxyNotNullConstPointer<_Ty2>& src_cref) : TNDRegisteredProxyNotNullConstPointer<_Ty>(src_cref) {}

		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TNDRegisteredProxyFixedConstPointer() {}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator const _Ty* () const { return TNDRegisteredProxyNotNullConstPointer<_Ty>::operator const _Ty * (); }
		MSE_DEPRECATED explicit operator const TNDXScopeRegisteredProxyObj<_Ty>* () const { return TNDRegisteredProxyNotNullConstPointer<_Ty>::operator const TNDXScopeRegisteredProxyObj<_Ty> * (); }
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	private:
		TNDRegisteredProxyFixedConstPointer(const TNDXScopeRegisteredProxyObj<_Ty>* ptr) : TNDRegisteredProxyNotNullConstPointer<_Ty>(ptr) {}
		TNDRegisteredProxyFixedConstPointer(const TNDXScopeRegisteredConstProxyObj<_Ty>* ptr) : TNDRegisteredProxyNotNullConstPointer<_Ty>(ptr) {}
		TNDRegisteredProxyFixedConstPointer<_Ty>& operator=(const TNDRegisteredProxyFixedConstPointer<_Ty>& _Right_cref) = delete;
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		friend class TNDXScopeRegisteredProxyObj<_Ty>;
		friend class TNDXScopeRegisteredConstProxyObj<_Ty>;
	};

	template<typename _TROFLy> using TNDXScopeRegisteredProxyObjBase = mse::TNDRegisteredObj<mse::TXScopeFixedPointer<_TROFLy> >;

	/* TNDXScopeRegisteredProxyObj is intended as a transparent wrapper for scope pointers. The purpose is to register the scope pointer's
	destruction so that TNDRegisteredProxyPointers will avoid referencing destroyed scope pointers. */
	template<typename _TROFLy>
	class TNDXScopeRegisteredProxyObj : public TNDXScopeRegisteredProxyObjBase<_TROFLy>, public mse::impl::first_or_placeholder_if_base_of_second<mse::us::impl::AsyncNotShareableTagBase, TNDXScopeRegisteredProxyObjBase<_TROFLy>, TNDXScopeRegisteredProxyObj<_TROFLy> >
		, public mse::impl::first_or_placeholder_if_base_of_second<mse::us::impl::XScopeTagBase, TNDXScopeRegisteredProxyObjBase<_TROFLy>, TNDXScopeRegisteredProxyObj<_TROFLy> >
	{
	public:
		typedef TNDXScopeRegisteredProxyObjBase<_TROFLy> base_class;

		TNDXScopeRegisteredProxyObj(const mse::TXScopeFixedPointer<_TROFLy> & xs_ptr) : base_class(xs_ptr) {}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		TNDXScopeRegisteredProxyObj(const mse::TXScopeObjFixedPointer<_TROFLy>& xs_ptr) : base_class(mse::TXScopeFixedPointer<_TROFLy>(xs_ptr)) {}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
		TNDXScopeRegisteredProxyObj(const TNDXScopeRegisteredProxyObj& _X) : base_class(_X) {}
		TNDXScopeRegisteredProxyObj(TNDXScopeRegisteredProxyObj&& _X) : base_class(MSE_FWD(_X)) {}

		TNDXScopeRegisteredProxyObj& operator=(TNDXScopeRegisteredProxyObj&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); }
		TNDXScopeRegisteredProxyObj& operator=(const TNDXScopeRegisteredProxyObj& _X) { base_class::operator=(_X); return (*this); }
		template<class _Ty2>
		TNDXScopeRegisteredProxyObj& operator=(_Ty2&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); }
		template<class _Ty2>
		TNDXScopeRegisteredProxyObj& operator=(const _Ty2& _X) { base_class::operator=(_X); return (*this); }

		TNDRegisteredProxyNotNullPointer<_TROFLy> mse_registered_proxy_nnptr() const { return TNDRegisteredProxyFixedPointer<_TROFLy>(this); }
		TNDRegisteredProxyFixedPointer<_TROFLy> mse_registered_proxy_fptr() const { return TNDRegisteredProxyFixedPointer<_TROFLy>(this); }

	private:

		/* The problenm with a public operator& returning a TNDRegisteredProxyFixedPointer<_TROFLy> is that its behavior
		wouldn't match it's native counterpart which would preclude a "disabled" mode where TNDXScopeRegisteredProxyObj<>
		was aliased to its native counterpart. */
		TNDRegisteredProxyFixedPointer<_TROFLy> operator&() const {
			return TNDRegisteredProxyFixedPointer<_TROFLy>(this);
		}

		template<typename _Ty2> friend class TNDRegisteredProxyPointer;
		template<typename _Ty2> friend class TNDRegisteredProxyConstPointer;
	};

	template<typename _TROFLy> using TNDXScopeRegisteredConstProxyObjBase = mse::TNDRegisteredObj<mse::TXScopeFixedConstPointer<_TROFLy> >;

	template<typename _TROFLy>
	class TNDXScopeRegisteredConstProxyObj : public TNDXScopeRegisteredConstProxyObjBase<_TROFLy>, public mse::impl::first_or_placeholder_if_base_of_second<mse::us::impl::AsyncNotShareableTagBase, TNDXScopeRegisteredConstProxyObjBase<_TROFLy>, TNDXScopeRegisteredConstProxyObj<_TROFLy> >
		, public mse::impl::first_or_placeholder_if_base_of_second<mse::us::impl::XScopeTagBase, TNDXScopeRegisteredConstProxyObjBase<_TROFLy>, TNDXScopeRegisteredConstProxyObj<_TROFLy> >
	{
	public:
		typedef TNDXScopeRegisteredConstProxyObjBase<_TROFLy> base_class;

		TNDXScopeRegisteredConstProxyObj(const mse::TXScopeFixedConstPointer<_TROFLy> & xs_ptr) : base_class(xs_ptr) {}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		TNDXScopeRegisteredConstProxyObj(const mse::TXScopeObjFixedConstPointer<_TROFLy>& xs_ptr) : base_class(mse::TXScopeFixedConstPointer<_TROFLy>(xs_ptr)) {}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
		TNDXScopeRegisteredConstProxyObj(const TNDXScopeRegisteredConstProxyObj & _X) : base_class(_X) {}
		TNDXScopeRegisteredConstProxyObj(TNDXScopeRegisteredConstProxyObj && _X) : base_class(MSE_FWD(_X)) {}

		TNDXScopeRegisteredConstProxyObj& operator=(TNDXScopeRegisteredConstProxyObj&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); }
		TNDXScopeRegisteredConstProxyObj& operator=(const TNDXScopeRegisteredConstProxyObj& _X) { base_class::operator=(_X); return (*this); }
		template<class _Ty2>
		TNDXScopeRegisteredConstProxyObj& operator=(_Ty2 && _X) { base_class::operator=(MSE_FWD(_X)); return (*this); }
		template<class _Ty2>
		TNDXScopeRegisteredConstProxyObj& operator=(const _Ty2 & _X) { base_class::operator=(_X); return (*this); }

		TNDRegisteredProxyNotNullConstPointer<_TROFLy> mse_registered_proxy_nnptr() const { return TNDRegisteredProxyFixedConstPointer<_TROFLy>(this); }
		TNDRegisteredProxyFixedConstPointer<_TROFLy> mse_registered_proxy_fptr() const { return TNDRegisteredProxyFixedConstPointer<_TROFLy>(this); }

	private:

		/* The problenm with a public operator& returning a TNDRegisteredProxyFixedPointer<_TROFLy> is that its behavior
		wouldn't match it's native counterpart which would preclude a "disabled" mode where TNDXScopeRegisteredConstProxyObj<>
		was aliased to its native counterpart. */
		TNDRegisteredProxyFixedPointer<_TROFLy> operator&() const {
			return TNDRegisteredProxyFixedPointer<_TROFLy>(this);
		}

		template<typename _Ty2> friend class TNDRegisteredProxyPointer;
		template<typename _Ty2> friend class TNDRegisteredProxyConstPointer;
	};
}

namespace std {
	template<class _Ty>
	struct hash<mse::TNDRegisteredProxyPointer<_Ty> > {	// hash functor
		typedef mse::TNDRegisteredProxyPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDRegisteredProxyPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
	template<class _Ty>
	struct hash<mse::TNDRegisteredProxyNotNullPointer<_Ty> > {	// hash functor
		typedef mse::TNDRegisteredProxyNotNullPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDRegisteredProxyNotNullPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
	template<class _Ty>
	struct hash<mse::TNDRegisteredProxyFixedPointer<_Ty> > {	// hash functor
		typedef mse::TNDRegisteredProxyFixedPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDRegisteredProxyFixedPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};

	template<class _Ty>
	struct hash<mse::TNDRegisteredProxyConstPointer<_Ty> > {	// hash functor
		typedef mse::TNDRegisteredProxyConstPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDRegisteredProxyConstPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
	template<class _Ty>
	struct hash<mse::TNDRegisteredProxyNotNullConstPointer<_Ty> > {	// hash functor
		typedef mse::TNDRegisteredProxyNotNullConstPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDRegisteredProxyNotNullConstPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
	template<class _Ty>
	struct hash<mse::TNDRegisteredProxyFixedConstPointer<_Ty> > {	// hash functor
		typedef mse::TNDRegisteredProxyFixedConstPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDRegisteredProxyFixedConstPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
}

namespace mse {

	template <typename _Ty>
	auto xscope_pointer(const TNDRegisteredProxyPointer<_Ty>& proxy_ptr) {
		return static_cast<mse::TXScopeFixedPointer<_Ty> >(proxy_ptr);
	}
	template <typename _Ty>
	auto xscope_pointer(const TNDRegisteredProxyConstPointer<_Ty>& proxy_ptr) {
		return static_cast<mse::TXScopeFixedConstPointer<_Ty> >(proxy_ptr);
	}
	template <typename _Ty>
	auto xscope_const_pointer(const TNDRegisteredProxyPointer<_Ty>& proxy_ptr) {
		return static_cast<mse::TXScopeFixedConstPointer<_Ty> >(TNDRegisteredProxyConstPointer<_Ty>(proxy_ptr));
	}
	template <typename _Ty>
	auto xscope_const_pointer(const TNDRegisteredProxyConstPointer<_Ty>& proxy_ptr) {
		return static_cast<mse::TXScopeFixedConstPointer<_Ty> >(proxy_ptr);
	}

	/* shorter aliases */
	/*
	template<typename _Ty> using rpp = TRegisteredProxyPointer<_Ty>;
	template<typename _Ty> using rpcp = TRegisteredProxyConstPointer<_Ty>;
	template<typename _Ty> using rpnnp = TRegisteredProxyNotNullPointer<_Ty>;
	template<typename _Ty> using rpnncp = TRegisteredProxyNotNullConstPointer<_Ty>;
	template<typename _Ty> using rpfp = TRegisteredProxyFixedPointer<_Ty>;
	template<typename _Ty> using rpfcp = TRegisteredProxyFixedConstPointer<_Ty>;
	template<typename _TROy> using rpo = TXScopeRegisteredProxyObj<_TROy>;
	*/


#ifdef __clang__
#pragma clang diagnostic pop
#else /*__clang__*/
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif /*__GNUC__*/
#endif /*__clang__*/

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-function"
#else /*__clang__*/
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif /*__GNUC__*/
#endif /*__clang__*/

	namespace self_test {
		class CRegProxyPtrTest1 {
		public:
			static void s_test1() {
#ifdef MSE_SELF_TESTS
				class A {
				public:
					A() {}
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
					static int foo2(mse::TRegisteredProxyPointer<A> A_registered_proxy_ptr) { return A_registered_proxy_ptr->b; }
					static int foo3(mse::TXScopeFixedConstPointer<A> A_scope_ptr) { return A_scope_ptr->b; }
				protected:
					~B() {}
				};

				A* A_native_ptr = nullptr;
				/* mse::TRegisteredProxyPointer<>, like mse::TRegisteredPointer<>, is a flexible safe pointer. */
				mse::TRegisteredProxyPointer<A> A_registered_proxy_ptr1;
				mse::TRegisteredProxyPointer<A> A_registered_proxy_ptr2;

				{
					A a;
					mse::TXScopeObj<A> scope_a;
					/* Unlike mse::TRegisteredObj<> which wraps the target object type, mse::TXScopeRegisteredProxyObj<A> wraps
					a scope pointer to the target object. */
					mse::TXScopeRegisteredProxyObj<A> registered_proxy_a(&scope_a);

					A_native_ptr = &a;
					A_registered_proxy_ptr1 = mse::registered_proxy_fptr(registered_proxy_a);
					assert(A_native_ptr->b == A_registered_proxy_ptr1->b);

					mse::TRegisteredProxyPointer<A> A_registered_proxy_ptr2 = mse::registered_proxy_fptr(registered_proxy_a);
					A_registered_proxy_ptr2 = nullptr;
#ifndef MSE_REGISTEREDPOINTER_DISABLED
					bool expected_exception = false;
					MSE_TRY {
						int i = A_registered_proxy_ptr2->b; /* this is gonna throw an exception */
					}
					MSE_CATCH_ANY {
						//std::cerr << "expected exception" << std::endl;
						expected_exception = true;
						/* The exception is triggered by an attempt to dereference a null "registered_proxy pointer". */
					}
					assert(expected_exception);
#endif // !MSE_REGISTEREDPOINTER_DISABLED

					B::foo2(A_registered_proxy_ptr1);

					/* registered_proxy pointers can be explicitly converted to scope pointers. */
					B::foo3(mse::xscope_pointer(A_registered_proxy_ptr1));
					B::foo3(mse::xscope_const_pointer(A_registered_proxy_ptr1));
					mse::TXScopeFixedPointer<A> A_scope_ptr1 = mse::xscope_pointer(A_registered_proxy_ptr1);
					mse::TXScopeFixedConstPointer<A> A_scope_cptr1 = mse::xscope_const_pointer(A_registered_proxy_ptr1);

					/* registered_proxy pointers implicitly convert to scope pointers. */
					B::foo3(A_registered_proxy_ptr1);
					mse::TXScopeFixedPointer<A> A_scope_ptr2 = A_registered_proxy_ptr1;
					mse::TXScopeFixedConstPointer<A> A_scope_cptr2 = A_registered_proxy_ptr1;
					/* But doing the conversion explicitly may be necessary in some cases to avoid ambiguity. */

					if (A_registered_proxy_ptr2) {
						assert(false);
					}
					else if (A_registered_proxy_ptr2 != A_registered_proxy_ptr1) {
						A_registered_proxy_ptr2 = A_registered_proxy_ptr1;
						assert(A_registered_proxy_ptr2 == A_registered_proxy_ptr1);
					}
					else {
						assert(false);
					}

					A a2 = a;
					mse::TXScopeRegisteredProxyObj<A> registered_proxy_a2 = registered_proxy_a;

					mse::TRegisteredProxyConstPointer<A> rcp = A_registered_proxy_ptr1;
					mse::TRegisteredProxyConstPointer<A> rcp2 = rcp;
					const mse::TXScopeRegisteredProxyObj<A> cregistered_proxy_a = registered_proxy_a;
					rcp = mse::registered_proxy_fptr(cregistered_proxy_a);
					mse::TRegisteredProxyFixedConstPointer<A> rfcp = mse::registered_proxy_fptr(cregistered_proxy_a);
				}

				bool expected_exception = false;
#ifndef MSE_REGISTEREDPOINTER_DISABLED
				MSE_TRY {
					/* A_registered_proxy_ptr1 "knows" that the (registered_proxy) object it was pointing to has now been deallocated. */
					int i = A_registered_proxy_ptr1->b; /* So this is gonna throw an exception */
				}
				MSE_CATCH_ANY {
					//std::cerr << "expected exception" << std::endl;
					expected_exception = true;
				}
				assert(expected_exception);
#endif // !MSE_REGISTEREDPOINTER_DISABLED

				{
					/* Obtaining safe pointers to members of registered_proxy objects: */
					class E {
					public:
						std::string s2 = "some other text ";
					};

					mse::TXScopeObj<E> scope_e;
					mse::TXScopeRegisteredProxyObj<E> registered_proxy_e = &scope_e;
					mse::TRegisteredProxyPointer<E> E_registered_proxy_ptr1 = mse::registered_proxy_fptr(registered_proxy_e);

					/* To obtain a safe pointer to a member you can use the "mse::make_pointer_to_member_v2()" function. */
					auto s2_safe_ptr1 = mse::make_pointer_to_member_v2(E_registered_proxy_ptr1, &E::s2);
					(*s2_safe_ptr1) = "some new text";
					auto s2_safe_const_ptr1 = mse::make_const_pointer_to_member_v2(E_registered_proxy_ptr1, &E::s2);
				}
				{
					mse::TXScopeObj<A> scope_a;
					mse::TXScopeFixedConstPointer<A> a_xs_cptr1 = &scope_a;
					auto a_xs_reg_cproxy_obj1 = mse::make_xscope_registered_proxy(a_xs_cptr1);
					auto a_xs_reg_proxy_cptr1 = mse::registered_proxy_fptr(a_xs_reg_cproxy_obj1);
				}
#ifdef MSE_HAS_CXX17
				{
					auto xscp_narr1 = mse::make_xscope(mse::nii_array<int, 3>({ 11, 12, 13 }));
					auto xscp_proxy_obj1 = mse::make_xscope_registered_proxy(&xscp_narr1);

					auto xscope_ptr1 = &xscp_narr1;
					mse::TXScopeFixedConstPointer<mse::nii_array<int, 3> > xscope_cptr1 = &xscp_narr1;
					auto proxy_ptr1 = mse::registered_proxy_fptr(xscp_proxy_obj1);

					auto xscope_random_access_section1 = mse::make_xscope_random_access_section(xscope_ptr1);
					auto xscope_random_access_const_section1 = mse::make_xscope_random_access_const_section(xscope_cptr1);
					auto proxy_random_access_section1 = mse::make_random_access_section(proxy_ptr1);

					/* Here a random access section based on a registered proxy pointer is implicitly converting to a
					random access section based on a scope pointer. */
					decltype(xscope_random_access_section1) xscope_random_access_section2 = proxy_random_access_section1;
					decltype(xscope_random_access_const_section1) xscope_random_access_const_section2 = proxy_random_access_section1;

					assert(xscp_narr1[0] == xscope_random_access_section2[0]);
					assert(xscp_narr1[1] == xscope_random_access_const_section2[1]);
				}
#endif /* MSE_HAS_CXX17 */

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

#ifdef _MSC_VER
#pragma warning( pop )  
#endif /*_MSC_VER*/

#ifndef MSE_PUSH_MACRO_NOT_SUPPORTED
#pragma pop_macro("MSE_THROW")
#pragma pop_macro("_NOEXCEPT")
#endif // !MSE_PUSH_MACRO_NOT_SUPPORTED

#endif // MSEREGISTEREDPROXY_H_
