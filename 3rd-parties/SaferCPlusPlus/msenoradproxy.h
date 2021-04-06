
// Copyright (c) 2015 Noah Lopez
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef MSENORADPROXY_H_
#define MSENORADPROXY_H_

#include "msescope.h"
#include "msenorad.h"
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

	template<typename _Ty> class TNDXScopeNoradProxyObj;
	template<typename _Ty> class TNDXScopeNoradConstProxyObj;
	template<typename _Ty> class TNDNoradProxyPointer;
	template<typename _Ty> class TNDNoradProxyConstPointer;
	template<typename _Ty> class TNDNoradProxyNotNullPointer;
	template<typename _Ty> class TNDNoradProxyNotNullConstPointer;
	template<typename _Ty> class TNDNoradProxyFixedPointer;
	template<typename _Ty> class TNDNoradProxyFixedConstPointer;

	template<typename _Ty>
	auto ndnorad_proxy_fptr(_Ty&& _X) {
		return _X.mse_norad_proxy_fptr();
	}
	template<typename _Ty>
	auto ndnorad_proxy_fptr(const _Ty& _X) {
		return _X.mse_norad_proxy_fptr();
	}

	template <class X>
	auto make_xscope_ndnorad_proxy(const mse::TXScopeFixedPointer<X>& xs_ptr) {
		return TNDXScopeNoradProxyObj<X>(xs_ptr);
	}
	template <class X>
	auto make_xscope_ndnorad_proxy(const mse::TXScopeFixedConstPointer<X>& xs_ptr) {
		return TNDXScopeNoradConstProxyObj<X>(xs_ptr);
	}
	template <class X>
	auto make_xscope_ndnorad_const_proxy(const mse::TXScopeFixedConstPointer<X>& xs_ptr) {
		return TNDXScopeNoradConstProxyObj<X>(xs_ptr);
	}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
	template <class X>
	auto make_xscope_ndnorad_proxy(const mse::TXScopeObjFixedPointer<X>& xs_ptr) {
		return TNDXScopeNoradProxyObj<X>(xs_ptr);
	}
	template <class X>
	auto make_xscope_ndnorad_proxy(const mse::TXScopeObjFixedConstPointer<X>& xs_ptr) {
		return TNDXScopeNoradConstProxyObj<X>(xs_ptr);
	}
	template <class X>
	auto make_xscope_ndnorad_const_proxy(const mse::TXScopeObjFixedConstPointer<X>& xs_ptr) {
		return TNDXScopeNoradConstProxyObj<X>(xs_ptr);
	}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

#ifdef MSE_HAS_CXX17
	/* deduction guide */
	template<class _TROy> TNDXScopeNoradProxyObj(_TROy)->TNDXScopeNoradProxyObj<_TROy>;
	template<class _TROy> TNDXScopeNoradConstProxyObj(_TROy)->TNDXScopeNoradConstProxyObj<_TROy>;
#endif /* MSE_HAS_CXX17 */

	template<typename _Ty> using TNoradProxyPointer = TNDNoradProxyPointer<_Ty>;
	template<typename _Ty> using TNoradProxyConstPointer = TNDNoradProxyConstPointer<_Ty>;
	template<typename _Ty> using TNoradProxyNotNullPointer = TNDNoradProxyNotNullPointer<_Ty>;
	template<typename _Ty> using TNoradProxyNotNullConstPointer = TNDNoradProxyNotNullConstPointer<_Ty>;
	template<typename _Ty> using TNoradProxyFixedPointer = TNDNoradProxyFixedPointer<_Ty>;
	template<typename _Ty> using TNoradProxyFixedConstPointer = TNDNoradProxyFixedConstPointer<_Ty>;
	template<typename _TROy> using TXScopeNoradProxyObj = TNDXScopeNoradProxyObj<_TROy>;
	template<typename _TROy> using TXScopeNoradConstProxyObj = TNDXScopeNoradConstProxyObj<_TROy>;

	template<typename _Ty> auto norad_proxy_fptr(_Ty&& _X) { return ndnorad_proxy_fptr(MSE_FWD(_X)); }
	template<typename _Ty> auto norad_proxy_fptr(const _Ty& _X) { return ndnorad_proxy_fptr(_X); }

	template <class X>
	auto make_xscope_norad_proxy(const X& arg) {
		return make_xscope_ndnorad_proxy(arg);
	}
	template <class X>
	auto make_xscope_norad_const_proxy(const X& arg) {
		return make_xscope_ndnorad_const_proxy(arg);
	}

	/* TNDNoradProxyPointer<_Ty> is just an mse::TNDNoradPointer<mse::TXScopeFixedPointer<_Ty> > that
	dereferences to the target of type _Ty rather than the mse::TXScopeFixedPointer<_Ty>. */
	template<typename _Ty>
	class TNDNoradProxyPointer : public mse::TNDNoradConstPointer<mse::TXScopeFixedPointer<_Ty> > {
	public:
		typedef mse::TNDNoradConstPointer<mse::TXScopeFixedPointer<_Ty> > base_class;
		typedef base_class _MP;

		MSE_USING(TNDNoradProxyPointer, base_class);
		TNDNoradProxyPointer(const TNDNoradProxyPointer& src_cref) = default;
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradProxyPointer(const TNDNoradProxyPointer<_Ty2>& src_cref) : base_class(src_cref.contained_pointer()) {}
		TNDNoradProxyPointer<_Ty>& operator=(const TNDNoradProxyPointer<_Ty>& _Right_cref) {
			(*this).contained_pointer() = _Right_cref;
			return (*this);
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradProxyPointer<_Ty>& operator=(const TNDNoradProxyPointer<_Ty2>& _Right_cref) {
			(*this).contained_pointer() = (TNDNoradProxyPointer(_Right_cref));
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

		bool operator==(const TNDNoradProxyPointer& rhs) const {
			bool retval = false;
			if (contained_pointer()) {
				retval = (bool(rhs.contained_pointer()) && ((*contained_pointer()) == (*contained_pointer())));
			}
			else {
				retval = !bool(rhs.contained_pointer());
			}
			return retval;
		}
		bool operator!=(const TNDNoradProxyPointer& rhs) const {
			return !((*this) == rhs);
		}

	private:
		TNDNoradProxyPointer(const TNDXScopeNoradProxyObj<_Ty>* ptr) : base_class(ptr->mse_norad_fptr()) {}

		const _MP& contained_pointer() const& { return (*this); }
		const _MP& contained_pointer() const&& { return (*this); }
		_MP& contained_pointer()& { return (*this); }
		auto&& contained_pointer()&& { return static_cast<_MP&&>(MSE_FWD(*this)); }

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		template <class Y> friend class TNDNoradProxyPointer;
		template <class Y> friend class TNDNoradProxyConstPointer;
		friend class TNDNoradProxyNotNullPointer<_Ty>;
	};

	template<typename _Ty>
	class TNDNoradProxyConstPointer : public mse::TNDNoradConstPointer<mse::TXScopeFixedConstPointer<_Ty> > {
	public:
		typedef mse::TNDNoradConstPointer<mse::TXScopeFixedConstPointer<_Ty> > base_class;
		typedef base_class _MP;

		MSE_USING(TNDNoradProxyConstPointer, base_class);
		TNDNoradProxyConstPointer(const TNDNoradProxyConstPointer& src_cref) = default;
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradProxyConstPointer(const TNDNoradProxyConstPointer<_Ty2>& src_cref) : base_class(src_cref.contained_pointer()) {}
		TNDNoradProxyConstPointer(const TNDNoradProxyPointer<_Ty>& src_cref) : base_class(RegCPtrXSPtrToRegCPtrXSConstPtr(src_cref.contained_pointer())) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradProxyConstPointer(const TNDNoradProxyPointer<_Ty2>& src_cref) : base_class(src_cref.contained_pointer()) {}
		TNDNoradProxyConstPointer<_Ty>& operator=(const TNDNoradProxyConstPointer<_Ty>& _Right_cref) {
			(*this).contained_pointer() = _Right_cref;
			return (*this);
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradProxyConstPointer<_Ty>& operator=(const TNDNoradProxyConstPointer<_Ty2>& _Right_cref) {
			(*this).contained_pointer() = (TNDNoradProxyConstPointer(_Right_cref));
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

		bool operator==(const TNDNoradProxyConstPointer& rhs) const {
			bool retval = false;
			if (contained_pointer()) {
				retval = (bool(rhs.contained_pointer()) && ((*contained_pointer()) == (*contained_pointer())));
			}
			else {
				retval = !bool(rhs.contained_pointer());
			}
			return retval;
		}
		bool operator!=(const TNDNoradProxyConstPointer& rhs) const {
			return !((*this) == rhs);
		}

	private:
		TNDNoradProxyConstPointer(const TNDXScopeNoradProxyObj<const _Ty>* ptr) : base_class(ptr->mse_norad_fptr()) {}
		TNDNoradProxyConstPointer(const TNDXScopeNoradProxyObj<_Ty>* ptr) : base_class(ptr->mse_norad_fptr()) {}
		TNDNoradProxyConstPointer(const TNDXScopeNoradConstProxyObj<_Ty>* ptr) : base_class(ptr->mse_norad_fptr()) {}

		const _MP& contained_pointer() const& { return (*this); }
		const _MP& contained_pointer() const&& { return (*this); }
		_MP& contained_pointer()& { return (*this); }
		auto&& contained_pointer()&& { return static_cast<_MP&&>(MSE_FWD(*this)); }

		static auto ProxyObjPtrToProxyConstObjPtr(const TNDXScopeNoradProxyObj<_Ty>* ptr) {
			/* Reinterpreting a TNDXScopeNoradProxyObj<_Ty> as a TNDXScopeNoradProxyObj<const _Ty> should be fine. */
			return reinterpret_cast<const TNDXScopeNoradProxyObj<const _Ty>*>(ptr);
		}
		static const auto& RegCPtrXSPtrToRegCPtrXSConstPtr(const mse::TNDNoradConstPointer<mse::TXScopeFixedPointer<_Ty> >& src) {
			/* Reinterpreting a mse::TNDNoradConstPointer<mse::TXScopeFixedPointer<_Ty> > as an
			mse::TNDNoradConstPointer<mse::TXScopeFixedConstPointer<_Ty> > should be fine. */
			return reinterpret_cast<const mse::TNDNoradConstPointer<mse::TXScopeFixedConstPointer<_Ty> >&>(src);
		}

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		template <class Y> friend class TNDNoradProxyConstPointer;
		friend class TNDNoradProxyNotNullConstPointer<_Ty>;
	};

	template<typename _Ty>
	class TNDNoradProxyNotNullPointer : public TNDNoradProxyPointer<_Ty> {
	public:
		TNDNoradProxyNotNullPointer(const TNDNoradProxyNotNullPointer& src_cref) : TNDNoradProxyPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradProxyNotNullPointer(const TNDNoradProxyNotNullPointer<_Ty2>& src_cref) : TNDNoradProxyPointer<_Ty>(src_cref) {}

		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TNDNoradProxyNotNullPointer() {}

	private:
		TNDNoradProxyNotNullPointer(const TNDXScopeNoradProxyObj<_Ty>* ptr) : TNDNoradProxyPointer<_Ty>(ptr) {}

		/* If you want to use this constructor, use not_null_from_nullable() instead. */
		TNDNoradProxyNotNullPointer(const  TNDNoradProxyPointer<_Ty>& src_cref) : TNDNoradProxyPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradProxyNotNullPointer(const TNDNoradProxyPointer<_Ty2>& src_cref) : TNDNoradProxyPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		friend class TNDNoradProxyFixedPointer<_Ty>;
		template<typename _Ty2>
		friend TNDNoradProxyNotNullPointer<_Ty2> not_null_from_nullable(const TNDNoradProxyPointer<_Ty2>& src);
	};

	template<typename _Ty>
	class TNDNoradProxyNotNullConstPointer : public TNDNoradProxyConstPointer<_Ty> {
	public:
		TNDNoradProxyNotNullConstPointer(const TNDNoradProxyNotNullPointer<_Ty>& src_cref) : TNDNoradProxyConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradProxyNotNullConstPointer(const TNDNoradProxyNotNullPointer<_Ty2>& src_cref) : TNDNoradProxyConstPointer<_Ty>(src_cref) {}
		TNDNoradProxyNotNullConstPointer(const TNDNoradProxyNotNullConstPointer<_Ty>& src_cref) : TNDNoradProxyConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradProxyNotNullConstPointer(const TNDNoradProxyNotNullConstPointer<_Ty2>& src_cref) : TNDNoradProxyConstPointer<_Ty>(src_cref) {}

		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TNDNoradProxyNotNullConstPointer() {}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator const _Ty* () const { return TNDNoradProxyConstPointer<_Ty>::operator const _Ty * (); }
		MSE_DEPRECATED explicit operator const TNDXScopeNoradProxyObj<_Ty>* () const { return TNDNoradProxyConstPointer<_Ty>::operator const TNDXScopeNoradProxyObj<_Ty> * (); }
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	private:
		TNDNoradProxyNotNullConstPointer(const TNDXScopeNoradProxyObj<_Ty>* ptr) : TNDNoradProxyConstPointer<_Ty>(ptr) {}
		TNDNoradProxyNotNullConstPointer(const TNDXScopeNoradConstProxyObj<_Ty>* ptr) : TNDNoradProxyConstPointer<_Ty>(ptr) {}

		/* If you want to use this constructor, use not_null_from_nullable() instead. */
		TNDNoradProxyNotNullConstPointer(const TNDNoradProxyPointer<_Ty>& src_cref) : TNDNoradProxyConstPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradProxyNotNullConstPointer(const TNDNoradProxyPointer<_Ty2>& src_cref) : TNDNoradProxyConstPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}
		TNDNoradProxyNotNullConstPointer(const TNDNoradProxyConstPointer<_Ty>& src_cref) : TNDNoradProxyConstPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradProxyNotNullConstPointer(const TNDNoradProxyConstPointer<_Ty2>& src_cref) : TNDNoradProxyConstPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		friend class TNDNoradProxyFixedConstPointer<_Ty>;
		template<typename _Ty2>
		friend TNDNoradProxyNotNullConstPointer<_Ty2> not_null_from_nullable(const TNDNoradProxyConstPointer<_Ty2>& src);
	};

	template<typename _Ty>
	TNDNoradProxyNotNullPointer<_Ty> not_null_from_nullable(const TNDNoradProxyPointer<_Ty>& src) {
		return src;
	}
	template<typename _Ty>
	TNDNoradProxyNotNullConstPointer<_Ty> not_null_from_nullable(const TNDNoradProxyConstPointer<_Ty>& src) {
		return src;
	}

	/* TNDNoradProxyFixedPointer cannot be retargeted or constructed without a target. This pointer is recommended for passing
	parameters by reference. */
	template<typename _Ty>
	class TNDNoradProxyFixedPointer : public TNDNoradProxyNotNullPointer<_Ty> {
	public:
		TNDNoradProxyFixedPointer(const TNDNoradProxyFixedPointer& src_cref) : TNDNoradProxyNotNullPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradProxyFixedPointer(const TNDNoradProxyFixedPointer<_Ty2>& src_cref) : TNDNoradProxyNotNullPointer<_Ty>(src_cref) {}

		TNDNoradProxyFixedPointer(const TNDNoradProxyNotNullPointer<_Ty>& src_cref) : TNDNoradProxyNotNullPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradProxyFixedPointer(const TNDNoradProxyNotNullPointer<_Ty2>& src_cref) : TNDNoradProxyNotNullPointer<_Ty>(src_cref) {}

		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TNDNoradProxyFixedPointer() {}

#if !defined(MSE_SCOPEPOINTER_DISABLED)
		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator _Ty* () const { return TNDNoradProxyNotNullPointer<_Ty>::operator _Ty * (); }
		MSE_DEPRECATED explicit operator TNDXScopeNoradProxyObj<_Ty>* () const { return TNDNoradProxyNotNullPointer<_Ty>::operator TNDXScopeNoradProxyObj<_Ty> * (); }
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	private:
		TNDNoradProxyFixedPointer(const TNDXScopeNoradProxyObj<_Ty>* ptr) : TNDNoradProxyNotNullPointer<_Ty>(ptr) {}
		TNDNoradProxyFixedPointer<_Ty>& operator=(const TNDNoradProxyFixedPointer<_Ty>& _Right_cref) = delete;

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		friend class TNDXScopeNoradProxyObj<_Ty>;
	};

	template<typename _Ty>
	class TNDNoradProxyFixedConstPointer : public TNDNoradProxyNotNullConstPointer<_Ty> {
	public:
		TNDNoradProxyFixedConstPointer(const TNDNoradProxyFixedPointer<_Ty>& src_cref) : TNDNoradProxyNotNullConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradProxyFixedConstPointer(const TNDNoradProxyFixedPointer<_Ty2>& src_cref) : TNDNoradProxyNotNullConstPointer<_Ty>(src_cref) {}
		TNDNoradProxyFixedConstPointer(const TNDNoradProxyFixedConstPointer<_Ty>& src_cref) : TNDNoradProxyNotNullConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradProxyFixedConstPointer(const TNDNoradProxyFixedConstPointer<_Ty2>& src_cref) : TNDNoradProxyNotNullConstPointer<_Ty>(src_cref) {}

		TNDNoradProxyFixedConstPointer(const TNDNoradProxyNotNullPointer<_Ty>& src_cref) : TNDNoradProxyNotNullConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradProxyFixedConstPointer(const TNDNoradProxyNotNullPointer<_Ty2>& src_cref) : TNDNoradProxyNotNullConstPointer<_Ty>(src_cref) {}
		TNDNoradProxyFixedConstPointer(const TNDNoradProxyNotNullConstPointer<_Ty>& src_cref) : TNDNoradProxyNotNullConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradProxyFixedConstPointer(const TNDNoradProxyNotNullConstPointer<_Ty2>& src_cref) : TNDNoradProxyNotNullConstPointer<_Ty>(src_cref) {}

		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TNDNoradProxyFixedConstPointer() {}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator const _Ty* () const { return TNDNoradProxyNotNullConstPointer<_Ty>::operator const _Ty * (); }
		MSE_DEPRECATED explicit operator const TNDXScopeNoradProxyObj<_Ty>* () const { return TNDNoradProxyNotNullConstPointer<_Ty>::operator const TNDXScopeNoradProxyObj<_Ty> * (); }
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)

	private:
		TNDNoradProxyFixedConstPointer(const TNDXScopeNoradProxyObj<_Ty>* ptr) : TNDNoradProxyNotNullConstPointer<_Ty>(ptr) {}
		TNDNoradProxyFixedConstPointer(const TNDXScopeNoradConstProxyObj<_Ty>* ptr) : TNDNoradProxyNotNullConstPointer<_Ty>(ptr) {}
		TNDNoradProxyFixedConstPointer<_Ty>& operator=(const TNDNoradProxyFixedConstPointer<_Ty>& _Right_cref) = delete;
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		friend class TNDXScopeNoradProxyObj<_Ty>;
		friend class TNDXScopeNoradConstProxyObj<_Ty>;
	};

	template<typename _TROFLy> using TNDXScopeNoradProxyObjBase = mse::TNDNoradObj<mse::TXScopeFixedPointer<_TROFLy> >;

	/* TNDXScopeNoradProxyObj is intended as a transparent wrapper for scope pointers. The purpose is to check to ensure that
	the scope pointer isn't destroyed while any TNDNoradProxyPointers is targeting it. */
	template<typename _TROFLy>
	class TNDXScopeNoradProxyObj : public TNDXScopeNoradProxyObjBase<_TROFLy>, public mse::impl::first_or_placeholder_if_base_of_second<mse::us::impl::AsyncNotShareableTagBase, TNDXScopeNoradProxyObjBase<_TROFLy>, TNDXScopeNoradProxyObj<_TROFLy> >
		, public mse::impl::first_or_placeholder_if_base_of_second<mse::us::impl::XScopeTagBase, TNDXScopeNoradProxyObjBase<_TROFLy>, TNDXScopeNoradProxyObj<_TROFLy> >
	{
	public:
		typedef TNDXScopeNoradProxyObjBase<_TROFLy> base_class;

		TNDXScopeNoradProxyObj(const mse::TXScopeFixedPointer<_TROFLy> & xs_ptr) : base_class(xs_ptr) {}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		TNDXScopeNoradProxyObj(const mse::TXScopeObjFixedPointer<_TROFLy>& xs_ptr) : base_class(mse::TXScopeFixedPointer<_TROFLy>(xs_ptr)) {}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
		TNDXScopeNoradProxyObj(const TNDXScopeNoradProxyObj& _X) : base_class(_X) {}
		TNDXScopeNoradProxyObj(TNDXScopeNoradProxyObj&& _X) : base_class(MSE_FWD(_X)) {}

		TNDXScopeNoradProxyObj& operator=(TNDXScopeNoradProxyObj&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); }
		TNDXScopeNoradProxyObj& operator=(const TNDXScopeNoradProxyObj& _X) { base_class::operator=(_X); return (*this); }
		template<class _Ty2>
		TNDXScopeNoradProxyObj& operator=(_Ty2&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); }
		template<class _Ty2>
		TNDXScopeNoradProxyObj& operator=(const _Ty2& _X) { base_class::operator=(_X); return (*this); }

		TNDNoradProxyNotNullPointer<_TROFLy> mse_norad_proxy_nnptr() const { return TNDNoradProxyFixedPointer<_TROFLy>(this); }
		TNDNoradProxyFixedPointer<_TROFLy> mse_norad_proxy_fptr() const { return TNDNoradProxyFixedPointer<_TROFLy>(this); }

	private:

		/* The problenm with a public operator& returning a TNDNoradProxyFixedPointer<_TROFLy> is that its behavior
		wouldn't match it's native counterpart which would preclude a "disabled" mode where TNDXScopeNoradProxyObj<>
		was aliased to its native counterpart. */
		TNDNoradProxyFixedPointer<_TROFLy> operator&() const {
			return TNDNoradProxyFixedPointer<_TROFLy>(this);
		}

		template<typename _Ty2> friend class TNDNoradProxyPointer;
		template<typename _Ty2> friend class TNDNoradProxyConstPointer;
	};

	template<typename _TROFLy> using TNDXScopeNoradConstProxyObjBase = mse::TNDNoradObj<mse::TXScopeFixedConstPointer<_TROFLy> >;

	template<typename _TROFLy>
	class TNDXScopeNoradConstProxyObj : public TNDXScopeNoradConstProxyObjBase<_TROFLy>, public mse::impl::first_or_placeholder_if_base_of_second<mse::us::impl::AsyncNotShareableTagBase, TNDXScopeNoradConstProxyObjBase<_TROFLy>, TNDXScopeNoradConstProxyObj<_TROFLy> >
		, public mse::impl::first_or_placeholder_if_base_of_second<mse::us::impl::XScopeTagBase, TNDXScopeNoradConstProxyObjBase<_TROFLy>, TNDXScopeNoradConstProxyObj<_TROFLy> >
	{
	public:
		typedef TNDXScopeNoradConstProxyObjBase<_TROFLy> base_class;

		TNDXScopeNoradConstProxyObj(const mse::TXScopeFixedConstPointer<_TROFLy> & xs_ptr) : base_class(xs_ptr) {}
#if !defined(MSE_SCOPEPOINTER_DISABLED)
		TNDXScopeNoradConstProxyObj(const mse::TXScopeObjFixedConstPointer<_TROFLy>& xs_ptr) : base_class(mse::TXScopeFixedConstPointer<_TROFLy>(xs_ptr)) {}
#endif // !defined(MSE_SCOPEPOINTER_DISABLED)
		TNDXScopeNoradConstProxyObj(const TNDXScopeNoradConstProxyObj & _X) : base_class(_X) {}
		TNDXScopeNoradConstProxyObj(TNDXScopeNoradConstProxyObj && _X) : base_class(MSE_FWD(_X)) {}

		TNDXScopeNoradConstProxyObj& operator=(TNDXScopeNoradConstProxyObj&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); }
		TNDXScopeNoradConstProxyObj& operator=(const TNDXScopeNoradConstProxyObj& _X) { base_class::operator=(_X); return (*this); }
		template<class _Ty2>
		TNDXScopeNoradConstProxyObj& operator=(_Ty2 && _X) { base_class::operator=(MSE_FWD(_X)); return (*this); }
		template<class _Ty2>
		TNDXScopeNoradConstProxyObj& operator=(const _Ty2 & _X) { base_class::operator=(_X); return (*this); }

		TNDNoradProxyNotNullConstPointer<_TROFLy> mse_norad_proxy_nnptr() const { return TNDNoradProxyFixedConstPointer<_TROFLy>(this); }
		TNDNoradProxyFixedConstPointer<_TROFLy> mse_norad_proxy_fptr() const { return TNDNoradProxyFixedConstPointer<_TROFLy>(this); }

	private:

		/* The problenm with a public operator& returning a TNDNoradProxyFixedPointer<_TROFLy> is that its behavior
		wouldn't match it's native counterpart which would preclude a "disabled" mode where TNDXScopeNoradConstProxyObj<>
		was aliased to its native counterpart. */
		TNDNoradProxyFixedPointer<_TROFLy> operator&() const {
			return TNDNoradProxyFixedPointer<_TROFLy>(this);
		}

		template<typename _Ty2> friend class TNDNoradProxyPointer;
		template<typename _Ty2> friend class TNDNoradProxyConstPointer;
	};
}

namespace std {
	template<class _Ty>
	struct hash<mse::TNDNoradProxyPointer<_Ty> > {	// hash functor
		typedef mse::TNDNoradProxyPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDNoradProxyPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
	template<class _Ty>
	struct hash<mse::TNDNoradProxyNotNullPointer<_Ty> > {	// hash functor
		typedef mse::TNDNoradProxyNotNullPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDNoradProxyNotNullPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
	template<class _Ty>
	struct hash<mse::TNDNoradProxyFixedPointer<_Ty> > {	// hash functor
		typedef mse::TNDNoradProxyFixedPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDNoradProxyFixedPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};

	template<class _Ty>
	struct hash<mse::TNDNoradProxyConstPointer<_Ty> > {	// hash functor
		typedef mse::TNDNoradProxyConstPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDNoradProxyConstPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
	template<class _Ty>
	struct hash<mse::TNDNoradProxyNotNullConstPointer<_Ty> > {	// hash functor
		typedef mse::TNDNoradProxyNotNullConstPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDNoradProxyNotNullConstPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
	template<class _Ty>
	struct hash<mse::TNDNoradProxyFixedConstPointer<_Ty> > {	// hash functor
		typedef mse::TNDNoradProxyFixedConstPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDNoradProxyFixedConstPointer<_Ty>& _Keyval) const _NOEXCEPT {
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
	auto xscope_pointer(const TNDNoradProxyPointer<_Ty>& proxy_ptr) {
		return static_cast<mse::TXScopeFixedPointer<_Ty> >(proxy_ptr);
	}
	template <typename _Ty>
	auto xscope_pointer(const TNDNoradProxyConstPointer<_Ty>& proxy_ptr) {
		return static_cast<mse::TXScopeFixedConstPointer<_Ty> >(proxy_ptr);
	}
	template <typename _Ty>
	auto xscope_const_pointer(const TNDNoradProxyPointer<_Ty>& proxy_ptr) {
		return static_cast<mse::TXScopeFixedConstPointer<_Ty> >(TNDNoradProxyConstPointer<_Ty>(proxy_ptr));
	}
	template <typename _Ty>
	auto xscope_const_pointer(const TNDNoradProxyConstPointer<_Ty>& proxy_ptr) {
		return static_cast<mse::TXScopeFixedConstPointer<_Ty> >(proxy_ptr);
	}

	/* shorter aliases */
	/*
	template<typename _Ty> using npp = TNoradProxyPointer<_Ty>;
	template<typename _Ty> using npcp = TNoradProxyConstPointer<_Ty>;
	template<typename _Ty> using npnnp = TNoradProxyNotNullPointer<_Ty>;
	template<typename _Ty> using npnncp = TNoradProxyNotNullConstPointer<_Ty>;
	template<typename _Ty> using npfp = TNoradProxyFixedPointer<_Ty>;
	template<typename _Ty> using npfcp = TNoradProxyFixedConstPointer<_Ty>;
	template<typename _TROy> using npo = TXScopeNoradProxyObj<_TROy>;
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
		class CNoradProxyPtrTest1 {
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
					static int foo2(mse::TNoradProxyPointer<A> A_norad_proxy_ptr) { return A_norad_proxy_ptr->b; }
					static int foo3(mse::TXScopeFixedConstPointer<A> A_scope_ptr) { return A_scope_ptr->b; }
				protected:
					~B() {}
				};

				A* A_native_ptr = nullptr;
				/* mse::TNoradProxyPointer<>, like mse::TNoradPointer<>, is a flexible safe pointer. */
				mse::TNoradProxyPointer<A> A_norad_proxy_ptr1;
				mse::TNoradProxyPointer<A> A_norad_proxy_ptr2;

				{
					A a;
					mse::TXScopeObj<A> scope_a;
					/* Unlike mse::TNoradObj<> which wraps the target object type, mse::TXScopeNoradProxyObj<A> wraps
					a scope pointer to the target object. */
					mse::TXScopeNoradProxyObj<A> norad_proxy_a(&scope_a);

					A_native_ptr = &a;
					A_norad_proxy_ptr1 = mse::norad_proxy_fptr(norad_proxy_a);
					assert(A_native_ptr->b == A_norad_proxy_ptr1->b);

					mse::TNoradProxyPointer<A> A_norad_proxy_ptr2 = mse::norad_proxy_fptr(norad_proxy_a);
					A_norad_proxy_ptr2 = nullptr;
#ifndef MSE_NORADPOINTER_DISABLED
					bool expected_exception = false;
					MSE_TRY {
						int i = A_norad_proxy_ptr2->b; /* this is gonna throw an exception */
					}
					MSE_CATCH_ANY {
						//std::cerr << "expected exception" << std::endl;
						expected_exception = true;
						/* The exception is triggered by an attempt to dereference a null "norad_proxy pointer". */
					}
					assert(expected_exception);
#endif // !MSE_NORADPOINTER_DISABLED

					B::foo2(A_norad_proxy_ptr1);

					/* norad_proxy pointers can be explicitly converted to scope pointers. */
					B::foo3(mse::xscope_pointer(A_norad_proxy_ptr1));
					B::foo3(mse::xscope_const_pointer(A_norad_proxy_ptr1));
					mse::TXScopeFixedPointer<A> A_scope_ptr1 = mse::xscope_pointer(A_norad_proxy_ptr1);
					mse::TXScopeFixedConstPointer<A> A_scope_cptr1 = mse::xscope_const_pointer(A_norad_proxy_ptr1);

					/* norad_proxy pointers implicitly convert to scope pointers. */
					B::foo3(A_norad_proxy_ptr1);
					mse::TXScopeFixedPointer<A> A_scope_ptr2 = A_norad_proxy_ptr1;
					mse::TXScopeFixedConstPointer<A> A_scope_cptr2 = A_norad_proxy_ptr1;
					/* But doing the conversion explicitly may be necessary in some cases to avoid ambiguity. */

					if (A_norad_proxy_ptr2) {
						assert(false);
					}
					else if (A_norad_proxy_ptr2 != A_norad_proxy_ptr1) {
						A_norad_proxy_ptr2 = A_norad_proxy_ptr1;
						assert(A_norad_proxy_ptr2 == A_norad_proxy_ptr1);
					}
					else {
						assert(false);
					}

					A a2 = a;
					mse::TXScopeNoradProxyObj<A> norad_proxy_a2 = norad_proxy_a;

					mse::TNoradProxyConstPointer<A> rcp = A_norad_proxy_ptr1;
					mse::TNoradProxyConstPointer<A> rcp2 = rcp;
					const mse::TXScopeNoradProxyObj<A> cnorad_proxy_a = norad_proxy_a;
					rcp = mse::norad_proxy_fptr(cnorad_proxy_a);
					mse::TNoradProxyFixedConstPointer<A> rfcp = mse::norad_proxy_fptr(cnorad_proxy_a);

					/* Forgetting to detarget norad_proxy pointers before their target object is destroyed (in this case, by
					going out of scope) would result in program termination. */
					rcp = nullptr;
					A_norad_proxy_ptr1 = nullptr;
					A_norad_proxy_ptr2 = nullptr;
				}

				{
					/* Obtaining safe pointers to members of norad_proxy objects: */
					class E {
					public:
						std::string s2 = "some other text ";
					};

					mse::TXScopeObj<E> scope_e;
					mse::TXScopeNoradProxyObj<E> norad_proxy_e = &scope_e;
					mse::TNoradProxyPointer<E> E_norad_proxy_ptr1 = mse::norad_proxy_fptr(norad_proxy_e);

					/* To obtain a safe pointer to a member you can use the "mse::make_pointer_to_member_v2()" function. */
					auto s2_safe_ptr1 = mse::make_pointer_to_member_v2(E_norad_proxy_ptr1, &E::s2);
					(*s2_safe_ptr1) = "some new text";
					auto s2_safe_const_ptr1 = mse::make_const_pointer_to_member_v2(E_norad_proxy_ptr1, &E::s2);
				}
				{
					mse::TXScopeObj<A> scope_a;
					mse::TXScopeFixedConstPointer<A> a_xs_cptr1 = &scope_a;
					auto a_xs_nrd_cproxy_obj1 = mse::make_xscope_norad_proxy(a_xs_cptr1);
					auto a_xs_nrd_proxy_cptr1 = mse::norad_proxy_fptr(a_xs_nrd_cproxy_obj1);
				}
#ifdef MSE_HAS_CXX17
				{
					auto xscp_narr1 = mse::make_xscope(mse::nii_array<int, 3>({ 11, 12, 13 }));
					auto xscp_proxy_obj1 = mse::make_xscope_norad_proxy(&xscp_narr1);

					auto xscope_ptr1 = &xscp_narr1;
					mse::TXScopeFixedConstPointer<mse::nii_array<int, 3> > xscope_cptr1 = &xscp_narr1;
					auto proxy_ptr1 = mse::norad_proxy_fptr(xscp_proxy_obj1);

					auto xscope_random_access_section1 = mse::make_xscope_random_access_section(xscope_ptr1);
					auto xscope_random_access_const_section1 = mse::make_xscope_random_access_const_section(xscope_cptr1);
					auto proxy_random_access_section1 = mse::make_random_access_section(proxy_ptr1);

					/* Here a random access section based on a norad proxy pointer is implicitly converting to a
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

#endif // MSENORADPROXY_H_
