
// Copyright (c) 2015 Noah Lopez
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef MSEREGISTERED_H_
#define MSEREGISTERED_H_

#include "msepointerbasics.h"
#include "mseprimitives.h"
#include <utility>
#include <unordered_set>
#include <functional>
#include <cassert>
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


#if defined(MSE_SAFER_SUBSTITUTES_DISABLED) || defined(MSE_SAFERPTR_DISABLED)
#define MSE_REGISTEREDPOINTER_DISABLED
#endif /*defined(MSE_SAFER_SUBSTITUTES_DISABLED) || defined(MSE_SAFERPTR_DISABLED)*/

namespace mse {

	template<typename _Ty> class TNDRegisteredObj;
	template<typename _Ty> class TNDRegisteredPointer;
	template<typename _Ty> class TNDRegisteredConstPointer;
	template<typename _Ty> class TNDRegisteredNotNullPointer;
	template<typename _Ty> class TNDRegisteredNotNullConstPointer;
	template<typename _Ty> class TNDRegisteredFixedPointer;
	template<typename _Ty> class TNDRegisteredFixedConstPointer;

	template<typename _Ty>
	auto ndregistered_fptr_to(_Ty&& _X) {
		return _X.mse_registered_fptr();
	}
	template<typename _Ty>
	auto ndregistered_fptr_to(const _Ty& _X) {
		return _X.mse_registered_fptr();
	}

	template <class _Ty, class... Args> TNDRegisteredPointer<_Ty> ndregistered_new(Args&&... args);
	template <class _Ty> void ndregistered_delete(const TNDRegisteredPointer<_Ty>& ndregisteredPtrRef);
	template <class _Ty> void ndregistered_delete(const TNDRegisteredConstPointer<_Ty>& ndregisteredPtrRef);
	namespace us {
		template <class _Ty> void ndregistered_delete(const TNDRegisteredPointer<_Ty>& ndregisteredPtrRef);
		template <class _Ty> void ndregistered_delete(const TNDRegisteredConstPointer<_Ty>& ndregisteredPtrRef);
	}

	namespace impl {
		template<typename _Ty, class... Args>
		auto make_ndregistered_helper(std::true_type, Args&&... args) {
			return _Ty(std::forward<Args>(args)...);
		}
		template<typename _Ty, class... Args>
		auto make_ndregistered_helper(std::false_type, Args&&... args) {
			return TNDRegisteredObj<_Ty>(std::forward<Args>(args)...);
		}
	}
	template <class X, class... Args>
	auto make_ndregistered(Args&&... args) {
		typedef mse::impl::remove_reference_t<X> nrX;
		return impl::make_ndregistered_helper<nrX>(typename mse::impl::is_instantiation_of<nrX, TNDRegisteredObj>::type(), std::forward<Args>(args)...);
	}
	template <class X>
	auto make_ndregistered(const X& arg) {
		typedef mse::impl::remove_reference_t<X> nrX;
		return impl::make_ndregistered_helper<nrX>(typename mse::impl::is_instantiation_of<nrX, TNDRegisteredObj>::type(), arg);
	}
	template <class X>
	auto make_ndregistered(X&& arg) {
		typedef mse::impl::remove_reference_t<X> nrX;
		return impl::make_ndregistered_helper<nrX>(typename mse::impl::is_instantiation_of<nrX, TNDRegisteredObj>::type(), MSE_FWD(arg));
	}

#ifdef MSE_HAS_CXX17
	/* deduction guide */
	template<class _TROy> TNDRegisteredObj(_TROy)->TNDRegisteredObj<_TROy>;
#endif /* MSE_HAS_CXX17 */

#ifdef MSE_REGISTEREDPOINTER_DISABLED
	template<typename _Ty> using TRegisteredPointer = _Ty*;
	template<typename _Ty> using TRegisteredConstPointer = const _Ty*;
	template<typename _Ty> using TRegisteredNotNullPointer = _Ty*;
	template<typename _Ty> using TRegisteredNotNullConstPointer = const _Ty*;
	template<typename _Ty> using TRegisteredFixedPointer = _Ty* /*const*/; /* Can't be const qualified because standard
																											library containers don't support const elements. */
	template<typename _Ty> using TRegisteredFixedConstPointer = const _Ty* /*const*/;
	template<typename _TROy> using TRegisteredObj = _TROy;

	template<typename _Ty> auto registered_fptr_to(_Ty&& _X) { return std::addressof(_X); }
	template<typename _Ty> auto registered_fptr_to(const _Ty& _X) { return std::addressof(_X); }

	template <class _TRRWy> using TRegisteredRefWrapper = std::reference_wrapper<_TRRWy>;
	template <class _Ty, class... Args>
	TRegisteredPointer<_Ty> registered_new(Args&&... args) {
		return new TRegisteredObj<_Ty>(std::forward<Args>(args)...);
	}
	template <class _Ty>
	void registered_delete(const TRegisteredPointer<_Ty>& regPtrRef) {
		auto a = static_cast<TRegisteredObj<_Ty>*>(regPtrRef);
		delete a;
	}
	template <class _Ty>
	void registered_delete(const TRegisteredConstPointer<_Ty>& regPtrRef) {
		auto a = static_cast<const TRegisteredObj<_Ty>*>(regPtrRef);
		delete a;
	}
	namespace us {
		template <class _Ty>
		void registered_delete(const TRegisteredPointer<_Ty>& regPtrRef) {
			mse::registered_delete(regPtrRef);
		}
		template <class _Ty>
		void registered_delete(const TRegisteredConstPointer<_Ty>& regPtrRef) {
			mse::registered_delete(regPtrRef);
		}
	}

	template <class X, class... Args>
	auto make_registered(Args&&... args) {
		typedef mse::impl::remove_reference_t<X> nrX;
		return nrX(std::forward<Args>(args)...);
	}
	template <class X>
	auto make_registered(const X& arg) {
		typedef mse::impl::remove_reference_t<X> nrX;
		return nrX(arg);
	}
	template <class X>
	auto make_registered(X&& arg) {
		typedef mse::impl::remove_reference_t<X> nrX;
		return nrX(MSE_FWD(arg));
	}

#else /*MSE_REGISTEREDPOINTER_DISABLED*/

	template<typename _Ty> using TRegisteredPointer = TNDRegisteredPointer<_Ty>;
	template<typename _Ty> using TRegisteredConstPointer = TNDRegisteredConstPointer<_Ty>;
	template<typename _Ty> using TRegisteredNotNullPointer = TNDRegisteredNotNullPointer<_Ty>;
	template<typename _Ty> using TRegisteredNotNullConstPointer = TNDRegisteredNotNullConstPointer<_Ty>;
	template<typename _Ty> using TRegisteredFixedPointer = TNDRegisteredFixedPointer<_Ty>;
	template<typename _Ty> using TRegisteredFixedConstPointer = TNDRegisteredFixedConstPointer<_Ty>;
	template<typename _TROy> using TRegisteredObj = TNDRegisteredObj<_TROy>;

	template<typename _Ty> auto registered_fptr_to(_Ty&& _X) { return ndregistered_fptr_to(MSE_FWD(_X)); }
	template<typename _Ty> auto registered_fptr_to(const _Ty& _X) { return ndregistered_fptr_to(_X); }

	template <class _Ty, class... Args> TNDRegisteredPointer<_Ty> registered_new(Args&&... args) { return ndregistered_new<_Ty>(std::forward<Args>(args)...); }
	template <class _Ty> void registered_delete(TNDRegisteredPointer<_Ty>& ndregisteredPtrRef) { return mse::ndregistered_delete<_Ty>(ndregisteredPtrRef); }
	template <class _Ty> void registered_delete(TNDRegisteredConstPointer<_Ty>& ndregisteredPtrRef) { return mse::ndregistered_delete<_Ty>(ndregisteredPtrRef); }
	namespace us {
		template <class _Ty> void registered_delete(TNDRegisteredPointer<_Ty>& ndregisteredPtrRef) { return mse::us::ndregistered_delete<_Ty>(ndregisteredPtrRef); }
		template <class _Ty> void registered_delete(TNDRegisteredConstPointer<_Ty>& ndregisteredPtrRef) { return mse::us::ndregistered_delete<_Ty>(ndregisteredPtrRef); }
	}

	template <class X, class... Args>
	auto make_registered(Args&&... args) {
		return make_ndregistered<X>(std::forward<Args>(args)...);
	}
	template <class X>
	auto make_registered(const X& arg) {
		return make_ndregistered(arg);
	}
	template <class X>
	auto make_registered(X&& arg) {
		return make_ndregistered(MSE_FWD(arg));
	}

#endif /*MSE_REGISTEREDPOINTER_DISABLED*/

	namespace us {
		namespace impl {
			/* node of a (singly-linked) list of pointers */
			class CRegisteredNode {
			public:
				virtual void rn_set_pointer_to_null() const = 0;
				void set_next_ptr(const CRegisteredNode* next_ptr) const {
					m_next_ptr = next_ptr;
				}
				const CRegisteredNode* get_next_ptr() const {
					return m_next_ptr;
				}

			private:
				mutable const CRegisteredNode * m_next_ptr = nullptr;
			};
		}
	}

	/* "Registered" pointers are intended to behave just like native C++ pointers, except that their value is (automatically)
	set to nullptr when the target object is destroyed. And by default they will throw an exception upon any attempt to
	dereference a nullptr. Because they don't take ownership like some other smart pointers, they can point to objects
	allocated on the stack as well as the heap. */
	template<typename _Ty>
	class TNDRegisteredPointer : public mse::us::TSaferPtr<TNDRegisteredObj<_Ty>>, public mse::us::impl::CRegisteredNode {
	public:
		TNDRegisteredPointer() : mse::us::TSaferPtr<TNDRegisteredObj<_Ty>>() {}
		TNDRegisteredPointer(const TNDRegisteredPointer& src_cref) : mse::us::TSaferPtr<TNDRegisteredObj<_Ty>>(src_cref.m_ptr) {
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).register_pointer(*this);
			}
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredPointer(const TNDRegisteredPointer<_Ty2>& src_cref) : mse::us::TSaferPtr<TNDRegisteredObj<_Ty>>(src_cref.m_ptr) {
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).register_pointer(*this);
			}
		}
		TNDRegisteredPointer(std::nullptr_t) : mse::us::TSaferPtr<TNDRegisteredObj<_Ty>>(nullptr) {}
		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TNDRegisteredPointer() {
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).unregister_pointer(*this);
			}
		}
		TNDRegisteredPointer<_Ty>& operator=(const TNDRegisteredPointer<_Ty>& _Right_cref) {
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).unregister_pointer(*this);
			}
			mse::us::TSaferPtr<TNDRegisteredObj<_Ty>>::operator=(_Right_cref);
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).register_pointer(*this);
			}
			return (*this);
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredPointer<_Ty>& operator=(const TNDRegisteredPointer<_Ty2>& _Right_cref) {
			return (*this).operator=(TNDRegisteredPointer(_Right_cref));
		}
		operator bool() const { return !(!((*this).m_ptr)); }
		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator _Ty*() const {
#ifdef NATIVE_PTR_DEBUG_HELPER1
			if (nullptr == (*this).m_ptr) {
				int q = 5; /* just a line of code for putting a debugger break point */
			}
#endif /*NATIVE_PTR_DEBUG_HELPER1*/
			return (*this).m_ptr;
		}

		/* In C++, if an object is deleted via a pointer to its base class and the base class' destructor is not virtual,
		then the (derived) object's destructor won't be called possibly resulting in resource leaks. With registered
		objects, the destructor not being called also circumvents their memory safety mechanism. */
		void registered_delete() const {
			auto a = asANativePointerToTNDRegisteredObj();
			delete a;
			assert(nullptr == (*this).m_ptr);
		}

		/* todo: make this private */
		void rn_set_pointer_to_null() const override { (*this).spb_set_to_null(); }

	private:
		TNDRegisteredPointer(TNDRegisteredObj<_Ty>* ptr) : mse::us::TSaferPtr<TNDRegisteredObj<_Ty>>(ptr) {
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).register_pointer(*this);
			}
		}

		/* This function, if possible, should not be used. It is meant to be used exclusively by registered_delete<>(). */
		TNDRegisteredObj<_Ty>* asANativePointerToTNDRegisteredObj() const {
#ifdef NATIVE_PTR_DEBUG_HELPER1
			if (nullptr == (*this).m_ptr) {
				int q = 5; /* just a line of code for putting a debugger break point */
			}
#endif /*NATIVE_PTR_DEBUG_HELPER1*/
			return static_cast<TNDRegisteredObj<_Ty>*>((*this).m_ptr);
		}

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		template <class Y> friend class TNDRegisteredPointer;
		template <class Y> friend class TNDRegisteredConstPointer;
		friend class TNDRegisteredNotNullPointer<_Ty>;
	};

	template<typename _Ty>
	class TNDRegisteredConstPointer : public mse::us::TSaferPtr<const TNDRegisteredObj<_Ty>>, public mse::us::impl::CRegisteredNode {
	public:
		TNDRegisteredConstPointer() : mse::us::TSaferPtr<const TNDRegisteredObj<_Ty>>() {}
		TNDRegisteredConstPointer(const TNDRegisteredConstPointer& src_cref) : mse::us::TSaferPtr<const TNDRegisteredObj<_Ty>>(src_cref.m_ptr) {
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).register_pointer(*this);
			}
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredConstPointer(const TNDRegisteredConstPointer<_Ty2>& src_cref) : mse::us::TSaferPtr<const TNDRegisteredObj<_Ty>>(src_cref.m_ptr) {
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).register_pointer(*this);
			}
		}
		TNDRegisteredConstPointer(const TNDRegisteredPointer<_Ty>& src_cref) : mse::us::TSaferPtr<const TNDRegisteredObj<_Ty>>(src_cref.m_ptr) {
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).register_pointer(*this);
			}
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredConstPointer(const TNDRegisteredPointer<_Ty2>& src_cref) : mse::us::TSaferPtr<const TNDRegisteredObj<_Ty>>(src_cref.m_ptr) {
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).register_pointer(*this);
			}
		}
		TNDRegisteredConstPointer(std::nullptr_t) : mse::us::TSaferPtr<const TNDRegisteredObj<_Ty>>(nullptr) {}
		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TNDRegisteredConstPointer() {
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).unregister_pointer(*this);
			}
		}
		TNDRegisteredConstPointer<_Ty>& operator=(const TNDRegisteredConstPointer<_Ty>& _Right_cref) {
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).unregister_pointer(*this);
			}
			mse::us::TSaferPtr<const TNDRegisteredObj<_Ty>>::operator=(_Right_cref);
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).register_pointer(*this);
			}
			return (*this);
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredConstPointer<_Ty>& operator=(const TNDRegisteredConstPointer<_Ty2>& _Right_cref) {
			return (*this).operator=(TNDRegisteredConstPointer(_Right_cref));
		}

		operator bool() const { return !(!((*this).m_ptr)); }
		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator const _Ty*() const {
#ifdef NATIVE_PTR_DEBUG_HELPER1
			if (nullptr == (*this).m_ptr) {
				int q = 5; /* just a line of code for putting a debugger break point */
			}
#endif /*NATIVE_PTR_DEBUG_HELPER1*/
			return (*this).m_ptr;
		}

		/* In C++, if an object is deleted via a pointer to its base class and the base class' destructor is not virtual,
		then the (derived) object's destructor won't be called possibly resulting in resource leaks. With registered
		objects, the destructor not being called also circumvents their memory safety mechanism. */
		void registered_delete() const {
			auto a = asANativePointerToTNDRegisteredObj();
			delete a;
			assert(nullptr == (*this).m_ptr);
		}

		/* todo: make this private */
		void rn_set_pointer_to_null() const override { (*this).spb_set_to_null(); }

	private:
		TNDRegisteredConstPointer(const TNDRegisteredObj<_Ty>* ptr) : mse::us::TSaferPtr<const TNDRegisteredObj<_Ty>>(ptr) {
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).register_pointer(*this);
			}
		}

		/* This function, if possible, should not be used. It is meant to be used exclusively by registered_delete<>(). */
		const TNDRegisteredObj<_Ty>* asANativePointerToTNDRegisteredObj() const {
#ifdef NATIVE_PTR_DEBUG_HELPER1
			if (nullptr == (*this).m_ptr) {
				int q = 5; /* just a line of code for putting a debugger break point */
			}
#endif /*NATIVE_PTR_DEBUG_HELPER1*/
			return static_cast<const TNDRegisteredObj<_Ty>*>((*this).m_ptr);
		}

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		template <class Y> friend class TNDRegisteredConstPointer;
		friend class TNDRegisteredNotNullConstPointer<_Ty>;
	};

	template<typename _Ty>
	class TNDRegisteredNotNullPointer : public TNDRegisteredPointer<_Ty> {
	public:
		TNDRegisteredNotNullPointer(const TNDRegisteredNotNullPointer& src_cref) : TNDRegisteredPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredNotNullPointer(const TNDRegisteredNotNullPointer<_Ty2>& src_cref) : TNDRegisteredPointer<_Ty>(src_cref) {}

		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TNDRegisteredNotNullPointer() {}
		/*
		TNDRegisteredNotNullPointer<_Ty>& operator=(const TNDRegisteredNotNullPointer<_Ty>& _Right_cref) {
		TNDRegisteredPointer<_Ty>::operator=(_Right_cref);
		return (*this);
		}
		*/
		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator _Ty*() const { return TNDRegisteredPointer<_Ty>::operator _Ty*(); }
		MSE_DEPRECATED explicit operator TNDRegisteredObj<_Ty>*() const { return TNDRegisteredPointer<_Ty>::operator TNDRegisteredObj<_Ty>*(); }

	private:
		TNDRegisteredNotNullPointer(TNDRegisteredObj<_Ty>* ptr) : TNDRegisteredPointer<_Ty>(ptr) {}

		/* If you want to use this constructor, use not_null_from_nullable() instead. */
		TNDRegisteredNotNullPointer(const  TNDRegisteredPointer<_Ty>& src_cref) : TNDRegisteredPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredNotNullPointer(const TNDRegisteredPointer<_Ty2>& src_cref) : TNDRegisteredPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}

		/* If you want a pointer to a TNDRegisteredNotNullPointer<_Ty>, declare the TNDRegisteredNotNullPointer<_Ty> as a
		TNDRegisteredObj<TNDRegisteredNotNullPointer<_Ty>> instead. So for example:
		auto reg_ptr = TNDRegisteredObj<TNDRegisteredNotNullPointer<_Ty>>(mse::registered_new<_Ty>());
		auto reg_ptr_to_reg_ptr = &reg_ptr;
		*/
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		friend class TNDRegisteredFixedPointer<_Ty>;
		template<typename _Ty2>
		friend TNDRegisteredNotNullPointer<_Ty2> not_null_from_nullable(const TNDRegisteredPointer<_Ty2>& src);
	};

	template<typename _Ty>
	class TNDRegisteredNotNullConstPointer : public TNDRegisteredConstPointer<_Ty> {
	public:
		TNDRegisteredNotNullConstPointer(const TNDRegisteredNotNullPointer<_Ty>& src_cref) : TNDRegisteredConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredNotNullConstPointer(const TNDRegisteredNotNullPointer<_Ty2>& src_cref) : TNDRegisteredConstPointer<_Ty>(src_cref) {}
		TNDRegisteredNotNullConstPointer(const TNDRegisteredNotNullConstPointer<_Ty>& src_cref) : TNDRegisteredConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredNotNullConstPointer(const TNDRegisteredNotNullConstPointer<_Ty2>& src_cref) : TNDRegisteredConstPointer<_Ty>(src_cref) {}

		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TNDRegisteredNotNullConstPointer() {}
		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator const _Ty*() const { return TNDRegisteredConstPointer<_Ty>::operator const _Ty*(); }
		MSE_DEPRECATED explicit operator const TNDRegisteredObj<_Ty>*() const { return TNDRegisteredConstPointer<_Ty>::operator const TNDRegisteredObj<_Ty>*(); }

	private:
		TNDRegisteredNotNullConstPointer(const TNDRegisteredObj<_Ty>* ptr) : TNDRegisteredConstPointer<_Ty>(ptr) {}

		/* If you want to use this constructor, use not_null_from_nullable() instead. */
		TNDRegisteredNotNullConstPointer(const TNDRegisteredPointer<_Ty>& src_cref) : TNDRegisteredConstPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredNotNullConstPointer(const TNDRegisteredPointer<_Ty2>& src_cref) : TNDRegisteredConstPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}
		TNDRegisteredNotNullConstPointer(const TNDRegisteredConstPointer<_Ty>& src_cref) : TNDRegisteredConstPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredNotNullConstPointer(const TNDRegisteredConstPointer<_Ty2>& src_cref) : TNDRegisteredConstPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		friend class TNDRegisteredFixedConstPointer<_Ty>;
		template<typename _Ty2>
		friend TNDRegisteredNotNullConstPointer<_Ty2> not_null_from_nullable(const TNDRegisteredConstPointer<_Ty2>& src);
	};

	template<typename _Ty>
	TNDRegisteredNotNullPointer<_Ty> not_null_from_nullable(const TNDRegisteredPointer<_Ty>& src) {
		return src;
	}
	template<typename _Ty>
	TNDRegisteredNotNullConstPointer<_Ty> not_null_from_nullable(const TNDRegisteredConstPointer<_Ty>& src) {
		return src;
	}

	/* TNDRegisteredFixedPointer cannot be retargeted or constructed without a target. This pointer is recommended for passing
	parameters by reference. */
	template<typename _Ty>
	class TNDRegisteredFixedPointer : public TNDRegisteredNotNullPointer<_Ty> {
	public:
		TNDRegisteredFixedPointer(const TNDRegisteredFixedPointer& src_cref) : TNDRegisteredNotNullPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredFixedPointer(const TNDRegisteredFixedPointer<_Ty2>& src_cref) : TNDRegisteredNotNullPointer<_Ty>(src_cref) {}

		TNDRegisteredFixedPointer(const TNDRegisteredNotNullPointer<_Ty>& src_cref) : TNDRegisteredNotNullPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredFixedPointer(const TNDRegisteredNotNullPointer<_Ty2>& src_cref) : TNDRegisteredNotNullPointer<_Ty>(src_cref) {}

		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TNDRegisteredFixedPointer() {}

		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator _Ty*() const { return TNDRegisteredNotNullPointer<_Ty>::operator _Ty*(); }
		MSE_DEPRECATED explicit operator TNDRegisteredObj<_Ty>*() const { return TNDRegisteredNotNullPointer<_Ty>::operator TNDRegisteredObj<_Ty>*(); }

	private:
		TNDRegisteredFixedPointer(TNDRegisteredObj<_Ty>* ptr) : TNDRegisteredNotNullPointer<_Ty>(ptr) {}
		TNDRegisteredFixedPointer<_Ty>& operator=(const TNDRegisteredFixedPointer<_Ty>& _Right_cref) = delete;

		/* If you want a pointer to a TNDRegisteredFixedPointer<_Ty>, declare the TNDRegisteredFixedPointer<_Ty> as a
		TNDRegisteredObj<TNDRegisteredFixedPointer<_Ty>> instead. So for example:
		auto reg_ptr = TNDRegisteredObj<TNDRegisteredFixedPointer<_Ty>>(mse::registered_new<_Ty>());
		auto reg_ptr_to_reg_ptr = &reg_ptr;
		*/
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		friend class TNDRegisteredObj<_Ty>;
	};

	template<typename _Ty>
	class TNDRegisteredFixedConstPointer : public TNDRegisteredNotNullConstPointer<_Ty> {
	public:
		TNDRegisteredFixedConstPointer(const TNDRegisteredFixedPointer<_Ty>& src_cref) : TNDRegisteredNotNullConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredFixedConstPointer(const TNDRegisteredFixedPointer<_Ty2>& src_cref) : TNDRegisteredNotNullConstPointer<_Ty>(src_cref) {}
		TNDRegisteredFixedConstPointer(const TNDRegisteredFixedConstPointer<_Ty>& src_cref) : TNDRegisteredNotNullConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredFixedConstPointer(const TNDRegisteredFixedConstPointer<_Ty2>& src_cref) : TNDRegisteredNotNullConstPointer<_Ty>(src_cref) {}

		TNDRegisteredFixedConstPointer(const TNDRegisteredNotNullPointer<_Ty>& src_cref) : TNDRegisteredNotNullConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredFixedConstPointer(const TNDRegisteredNotNullPointer<_Ty2>& src_cref) : TNDRegisteredNotNullConstPointer<_Ty>(src_cref) {}
		TNDRegisteredFixedConstPointer(const TNDRegisteredNotNullConstPointer<_Ty>& src_cref) : TNDRegisteredNotNullConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDRegisteredFixedConstPointer(const TNDRegisteredNotNullConstPointer<_Ty2>& src_cref) : TNDRegisteredNotNullConstPointer<_Ty>(src_cref) {}

		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TNDRegisteredFixedConstPointer() {}
		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator const _Ty*() const { return TNDRegisteredNotNullConstPointer<_Ty>::operator const _Ty*(); }
		MSE_DEPRECATED explicit operator const TNDRegisteredObj<_Ty>*() const { return TNDRegisteredNotNullConstPointer<_Ty>::operator const TNDRegisteredObj<_Ty>*(); }

	private:
		TNDRegisteredFixedConstPointer(const TNDRegisteredObj<_Ty>* ptr) : TNDRegisteredNotNullConstPointer<_Ty>(ptr) {}
		TNDRegisteredFixedConstPointer<_Ty>& operator=(const TNDRegisteredFixedConstPointer<_Ty>& _Right_cref) = delete;
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		friend class TNDRegisteredObj<_Ty>;
	};

	/* This macro roughly simulates constructor inheritance. */
#define MSE_REGISTERED_OBJ_USING(Derived, Base) MSE_USING(Derived, Base)

	/* TNDRegisteredObj is intended as a transparent wrapper for other classes/objects. The purpose is to register the object's
	destruction so that TNDRegisteredPointers will avoid referencing destroyed objects. Note that TNDRegisteredObj can be used with
	objects allocated on the stack. */
	template<typename _TROFLy>
	class TNDRegisteredObj : public _TROFLy, public mse::impl::first_or_placeholder_if_base_of_second<mse::us::impl::AsyncNotShareableTagBase, _TROFLy, TNDRegisteredObj<_TROFLy> >
	{
	public:
		typedef _TROFLy base_class;

		MSE_REGISTERED_OBJ_USING(TNDRegisteredObj, _TROFLy);
		TNDRegisteredObj(const TNDRegisteredObj& _X) : _TROFLy(_X) {}
		TNDRegisteredObj(TNDRegisteredObj&& _X) : _TROFLy(MSE_FWD(_X)) {}
		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TNDRegisteredObj() {
			unregister_and_set_outstanding_pointers_to_null();
		}

		TNDRegisteredObj& operator=(TNDRegisteredObj&& _X) { _TROFLy::operator=(MSE_FWD(_X)); return (*this); }
		TNDRegisteredObj& operator=(const TNDRegisteredObj& _X) { _TROFLy::operator=(_X); return (*this); }
		template<class _Ty2>
		TNDRegisteredObj& operator=(_Ty2&& _X) { _TROFLy::operator=(MSE_FWD(_X)); return (*this); }
		template<class _Ty2>
		TNDRegisteredObj& operator=(const _Ty2& _X) { _TROFLy::operator=(_X); return (*this); }

		TNDRegisteredNotNullPointer<_TROFLy> operator&() {
			return TNDRegisteredFixedPointer<_TROFLy>(this);
		}
		TNDRegisteredNotNullConstPointer<_TROFLy> operator&() const {
			return TNDRegisteredFixedConstPointer<_TROFLy>(this);
		}
		TNDRegisteredNotNullPointer<_TROFLy> mse_registered_nnptr() { return TNDRegisteredFixedPointer<_TROFLy>(this); }
		TNDRegisteredNotNullConstPointer<_TROFLy> mse_registered_nnptr() const { return TNDRegisteredFixedConstPointer<_TROFLy>(this); }
		TNDRegisteredFixedPointer<_TROFLy> mse_registered_fptr() { return TNDRegisteredFixedPointer<_TROFLy>(this); }
		TNDRegisteredFixedConstPointer<_TROFLy> mse_registered_fptr() const { return TNDRegisteredFixedConstPointer<_TROFLy>(this); }

	private:
		void register_pointer(const mse::us::impl::CRegisteredNode& node_cref) const {
			node_cref.set_next_ptr(m_head_ptr);
			m_head_ptr = &node_cref;
		}
		void unregister_pointer(const mse::us::impl::CRegisteredNode& node_cref) const {
			const auto target_node_ptr = &node_cref;
			if (target_node_ptr == m_head_ptr) {
				m_head_ptr = target_node_ptr->get_next_ptr();
				node_cref.set_next_ptr(nullptr);
				return;
			}
			if (!m_head_ptr) {
				assert(false);
				return;
			}
			auto current_node_ptr = m_head_ptr;
			while (target_node_ptr != current_node_ptr->get_next_ptr()) {
				current_node_ptr = current_node_ptr->get_next_ptr();
				if (!current_node_ptr) {
					assert(false);
					return;
				}
			}
			current_node_ptr->set_next_ptr(target_node_ptr->get_next_ptr());
			node_cref.set_next_ptr(nullptr);
		}

		void unregister_and_set_outstanding_pointers_to_null() const {
			auto current_node_ptr = m_head_ptr;
			while (current_node_ptr) {
				current_node_ptr->rn_set_pointer_to_null();
				auto next_ptr = current_node_ptr->get_next_ptr();
				current_node_ptr->set_next_ptr(nullptr);
				current_node_ptr = next_ptr;
			}
		}

		/* first node in a (singly-linked) list of pointers targeting this object */
		mutable const mse::us::impl::CRegisteredNode * m_head_ptr = nullptr;

		template<typename _Ty2>
		friend class TNDRegisteredPointer;
		template<typename _Ty2>
		friend class TNDRegisteredConstPointer;
	};

	template <class _Ty, class... Args>
	TNDRegisteredPointer<_Ty> ndregistered_new(Args&&... args) {
		auto a = new TNDRegisteredObj<_Ty>(std::forward<Args>(args)...);
		mse::us::impl::tlSAllocRegistry_ref<TNDRegisteredObj<_Ty> >().registerPointer(a);
		return &(*a);
	}
	template <class _Ty>
	void ndregistered_delete(const TNDRegisteredPointer<_Ty>& regPtrRef) {
		auto a = static_cast<TNDRegisteredObj<_Ty>*>(regPtrRef);
		auto res = mse::us::impl::tlSAllocRegistry_ref<TNDRegisteredObj<_Ty> >().unregisterPointer(a);
		if (!res) { assert(false); MSE_THROW(std::invalid_argument("invalid argument, no corresponding allocation found - mse::registered_delete() \n- tip: If deleting via base class pointer, use mse::us::registered_delete() instead. ")); }
		regPtrRef.registered_delete();
	}
	template <class _Ty>
	void ndregistered_delete(const TNDRegisteredConstPointer<_Ty>& regPtrRef) {
		auto a = static_cast<const TNDRegisteredObj<_Ty>*>(regPtrRef);
		auto res = mse::us::impl::tlSAllocRegistry_ref<TNDRegisteredObj<_Ty> >().unregisterPointer(a);
		if (!res) { assert(false); MSE_THROW(std::invalid_argument("invalid argument, no corresponding allocation found - mse::registered_delete() \n- tip: If deleting via base class pointer, use mse::us::registered_delete() instead. ")); }
		regPtrRef.registered_delete();
	}
	namespace us {
		template <class _Ty>
		void ndregistered_delete(const TNDRegisteredPointer<_Ty>& regPtrRef) {
			regPtrRef.registered_delete();
		}
		template <class _Ty>
		void ndregistered_delete(const TNDRegisteredConstPointer<_Ty>& regPtrRef) {
			regPtrRef.registered_delete();
		}
	}
}

namespace std {
	template<class _Ty>
	struct hash<mse::TNDRegisteredPointer<_Ty> > {	// hash functor
		typedef mse::TNDRegisteredPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDRegisteredPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
	template<class _Ty>
	struct hash<mse::TNDRegisteredNotNullPointer<_Ty> > {	// hash functor
		typedef mse::TNDRegisteredNotNullPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDRegisteredNotNullPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
	template<class _Ty>
	struct hash<mse::TNDRegisteredFixedPointer<_Ty> > {	// hash functor
		typedef mse::TNDRegisteredFixedPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDRegisteredFixedPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};

	template<class _Ty>
	struct hash<mse::TNDRegisteredConstPointer<_Ty> > {	// hash functor
		typedef mse::TNDRegisteredConstPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDRegisteredConstPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
	template<class _Ty>
	struct hash<mse::TNDRegisteredNotNullConstPointer<_Ty> > {	// hash functor
		typedef mse::TNDRegisteredNotNullConstPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDRegisteredNotNullConstPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
	template<class _Ty>
	struct hash<mse::TNDRegisteredFixedConstPointer<_Ty> > {	// hash functor
		typedef mse::TNDRegisteredFixedConstPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDRegisteredFixedConstPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
}

namespace mse {

	/* template specializations */

#define MSE_NDREGISTERED_IMPL_OBJ_INHERIT_ASSIGNMENT_OPERATOR(class_name) \
		auto& operator=(class_name&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); } \
		auto& operator=(const class_name& _X) { base_class::operator=(_X); return (*this); } \
		template<class _Ty2> auto& operator=(_Ty2&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); } \
		template<class _Ty2> auto& operator=(const _Ty2& _X) { base_class::operator=(_X); return (*this); }

#define MSE_NDREGISTERED_IMPL_OBJ_SPECIALIZATION_DEFINITIONS1(class_name) \
		class_name(const class_name&) = default; \
		class_name(class_name&&) = default; \
		MSE_NDREGISTERED_IMPL_OBJ_INHERIT_ASSIGNMENT_OPERATOR(class_name);

#if !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)
#define MSE_NDREGISTERED_IMPL_OBJ_NATIVE_POINTER_PRIVATE_CONSTRUCTORS1(class_name) \
			class_name(std::nullptr_t) {} \
			class_name() {}
#else // !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)
#define MSE_NDREGISTERED_IMPL_OBJ_NATIVE_POINTER_PRIVATE_CONSTRUCTORS1(class_name)
#endif // !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)

	/* Note that because we explicitly define some (private) constructors, default copy and move constructors
	and assignment operators won't be generated, so we have to define those as well. */
#define MSE_NDREGISTERED_IMPL_OBJ_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type) \
		template<typename _Ty> \
		class TNDRegisteredObj<specified_type> : public TNDRegisteredObj<mapped_type> { \
		public: \
			typedef TNDRegisteredObj<mapped_type> base_class; \
			MSE_USING(TNDRegisteredObj, base_class); \
			MSE_NDREGISTERED_IMPL_OBJ_SPECIALIZATION_DEFINITIONS1(TNDRegisteredObj); \
		private: \
			MSE_NDREGISTERED_IMPL_OBJ_NATIVE_POINTER_PRIVATE_CONSTRUCTORS1(TNDRegisteredObj); \
		};

#define MSE_NDREGISTERED_IMPL_PTR_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type) \
		template<typename _Ty> \
		class TNDRegisteredPointer<specified_type> : public TNDRegisteredPointer<mapped_type> { \
		public: \
			typedef TNDRegisteredPointer<mapped_type> base_class; \
			MSE_USING(TNDRegisteredPointer, base_class); \
		}; \
		template<typename _Ty> \
		class TNDRegisteredConstPointer<specified_type> : public TNDRegisteredConstPointer<mapped_type> { \
		public: \
			typedef TNDRegisteredConstPointer<mapped_type> base_class; \
			MSE_USING(TNDRegisteredConstPointer, base_class); \
		}; \
		template<typename _Ty> \
		class TNDRegisteredNotNullPointer<specified_type> : public TNDRegisteredNotNullPointer<mapped_type> { \
		public: \
			typedef TNDRegisteredNotNullPointer<mapped_type> base_class; \
			MSE_USING(TNDRegisteredNotNullPointer, base_class); \
		}; \
		template<typename _Ty> \
		class TNDRegisteredNotNullConstPointer<specified_type> : public TNDRegisteredNotNullConstPointer<mapped_type> { \
		public: \
			typedef TNDRegisteredNotNullConstPointer<mapped_type> base_class; \
			MSE_USING(TNDRegisteredNotNullConstPointer, base_class); \
		}; \
		template<typename _Ty> \
		class TNDRegisteredFixedPointer<specified_type> : public TNDRegisteredFixedPointer<mapped_type> { \
		public: \
			typedef TNDRegisteredFixedPointer<mapped_type> base_class; \
			MSE_USING(TNDRegisteredFixedPointer, base_class); \
		}; \
		template<typename _Ty> \
		class TNDRegisteredFixedConstPointer<specified_type> : public TNDRegisteredFixedConstPointer<mapped_type> { \
		public: \
			typedef TNDRegisteredFixedConstPointer<mapped_type> base_class; \
			MSE_USING(TNDRegisteredFixedConstPointer, base_class); \
		};

#define MSE_NDREGISTERED_IMPL_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type) \
		MSE_NDREGISTERED_IMPL_PTR_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type); \
		MSE_NDREGISTERED_IMPL_OBJ_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type);

	MSE_NDREGISTERED_IMPL_NATIVE_POINTER_SPECIALIZATION(_Ty*, mse::us::impl::TPointerForLegacy<_Ty>);
	MSE_NDREGISTERED_IMPL_NATIVE_POINTER_SPECIALIZATION(_Ty* const, const mse::us::impl::TPointerForLegacy<_Ty>);

#ifdef MSEPRIMITIVES_H

#define MSE_NDREGISTERED_IMPL_OBJ_INTEGRAL_SPECIALIZATION(integral_type) \
		template<> \
		class TNDRegisteredObj<integral_type> : public TNDRegisteredObj<mse::TInt<integral_type>> { \
		public: \
			typedef TNDRegisteredObj<mse::TInt<integral_type>> base_class; \
			MSE_USING(TNDRegisteredObj, base_class); \
		};

#define MSE_NDREGISTERED_IMPL_PTR_INTEGRAL_SPECIALIZATION(integral_type) \
		template<> \
		class TNDRegisteredPointer<integral_type> : public TNDRegisteredPointer<mse::TInt<integral_type>> { \
		public: \
			typedef TNDRegisteredPointer<mse::TInt<integral_type>> base_class; \
			MSE_USING(TNDRegisteredPointer, base_class); \
		}; \
		template<> \
		class TNDRegisteredConstPointer<integral_type> : public TNDRegisteredConstPointer<mse::TInt<integral_type>> { \
		public: \
			typedef TNDRegisteredConstPointer<mse::TInt<integral_type>> base_class; \
			MSE_USING(TNDRegisteredConstPointer, base_class); \
		}; \
		template<> \
		class TNDRegisteredNotNullPointer<integral_type> : public TNDRegisteredNotNullPointer<mse::TInt<integral_type>> { \
		public: \
			typedef TNDRegisteredNotNullPointer<mse::TInt<integral_type>> base_class; \
			MSE_USING(TNDRegisteredNotNullPointer, base_class); \
		}; \
		template<> \
		class TNDRegisteredNotNullConstPointer<integral_type> : public TNDRegisteredNotNullConstPointer<mse::TInt<integral_type>> { \
		public: \
			typedef TNDRegisteredNotNullConstPointer<mse::TInt<integral_type>> base_class; \
			MSE_USING(TNDRegisteredNotNullConstPointer, base_class); \
		}; \
		template<> \
		class TNDRegisteredFixedPointer<integral_type> : public TNDRegisteredFixedPointer<mse::TInt<integral_type>> { \
		public: \
			typedef TNDRegisteredFixedPointer<mse::TInt<integral_type>> base_class; \
			MSE_USING(TNDRegisteredFixedPointer, base_class); \
		}; \
		template<> \
		class TNDRegisteredFixedConstPointer<integral_type> : public TNDRegisteredFixedConstPointer<mse::TInt<integral_type>> { \
		public: \
			typedef TNDRegisteredFixedConstPointer<mse::TInt<integral_type>> base_class; \
			MSE_USING(TNDRegisteredFixedConstPointer, base_class); \
		};

#define MSE_NDREGISTERED_IMPL_INTEGRAL_SPECIALIZATION(integral_type) \
		MSE_NDREGISTERED_IMPL_PTR_INTEGRAL_SPECIALIZATION(integral_type); \
		MSE_NDREGISTERED_IMPL_OBJ_INTEGRAL_SPECIALIZATION(integral_type); \
		MSE_NDREGISTERED_IMPL_PTR_INTEGRAL_SPECIALIZATION(typename std::add_const<integral_type>::type); \
		MSE_NDREGISTERED_IMPL_OBJ_INTEGRAL_SPECIALIZATION(typename std::add_const<integral_type>::type);

	MSE_IMPL_APPLY_MACRO_FUNCTION_TO_EACH_OF_THE_INTEGER_TYPES(MSE_NDREGISTERED_IMPL_INTEGRAL_SPECIALIZATION)

#endif /*MSEPRIMITIVES_H*/

	/* end of template specializations */

#ifdef MSE_REGISTEREDPOINTER_DISABLED
#else /*MSE_REGISTEREDPOINTER_DISABLED*/

#ifdef _MSC_VER
#if (1900 <= _MSC_VER)
#define MSEREGISTEREDREFWRAPPER 1
#endif // (1900 <= _MSC_VER)
#else /*_MSC_VER*/
#define MSEREGISTEREDREFWRAPPER 1
#if (defined(__GNUC__) || defined(__GNUG__))
#define GPP_COMPATIBLE 1
#else /*(defined(__GNUC__) || defined(__GNUG__))*/
#ifdef __clang__
#define CLANG_COMPATIBLE 1
#endif // __clang__
#endif /*(defined(__GNUC__) || defined(__GNUG__))*/
#endif /*_MSC_VER*/

#ifdef MSEREGISTEREDREFWRAPPER
	template <class _TRRWy>
	class TRegisteredRefWrapper : public mse::us::impl::AsyncNotShareableAndNotPassableTagBase {
	public:
		// types
		typedef TRegisteredObj<_TRRWy> type;

		// construct/copy/destroy
		TRegisteredRefWrapper(TRegisteredObj<_TRRWy>& ref) : _ptr(&ref) {}
		TRegisteredRefWrapper(TRegisteredObj<_TRRWy>&&) = delete;
		TRegisteredRefWrapper(const TRegisteredRefWrapper&) = default;

		// assignment
		TRegisteredRefWrapper& operator=(const TRegisteredRefWrapper& x) = default;

		// access
		operator TRegisteredObj<_TRRWy>& () const { return *_ptr; }
		TRegisteredObj<_TRRWy>& get() const { return *_ptr; }

		template< class... ArgTypes >
		typename std::result_of<TRegisteredObj<_TRRWy>&(ArgTypes&&...)>::type
			operator() (ArgTypes&&... args) const {
#if defined(GPP_COMPATIBLE) || defined(CLANG_COMPATIBLE)
			return __invoke(get(), std::forward<ArgTypes>(args)...);
#else // defined(GPP_COMPATIBLE) || definded(CLANG_COMPATIBLE)
			return std::invoke(get(), std::forward<ArgTypes>(args)...);
#endif // defined(GPP_COMPATIBLE) || definded(CLANG_COMPATIBLE)
		}

	private:
		TRegisteredPointer<_TRRWy> _ptr;
	};
#endif // MSEREGISTEREDREFWRAPPER

#endif /*MSE_REGISTEREDPOINTER_DISABLED*/

	/* shorter aliases */
	template<typename _Ty> using rp = TRegisteredPointer<_Ty>;
	template<typename _Ty> using rcp = TRegisteredConstPointer<_Ty>;
	template<typename _Ty> using rnnp = TRegisteredNotNullPointer<_Ty>;
	template<typename _Ty> using rnncp = TRegisteredNotNullConstPointer<_Ty>;
	template<typename _Ty> using rfp = TRegisteredFixedPointer<_Ty>;
	template<typename _Ty> using rfcp = TRegisteredFixedConstPointer<_Ty>;
	template<typename _TROy> using ro = TRegisteredObj<_TROy>;

	template<typename _Ty> using reg_ptr = TRegisteredPointer<_Ty>;
	template<typename _Ty> using reg_cptr = TRegisteredConstPointer<_Ty>;
	template<typename _Ty> using reg_nnptr = TRegisteredNotNullPointer<_Ty>;
	template<typename _Ty> using reg_nncptr = TRegisteredNotNullConstPointer<_Ty>;
	template<typename _Ty> using reg_fptr = TRegisteredFixedPointer<_Ty>;
	template<typename _Ty> using reg_fcptr = TRegisteredFixedConstPointer<_Ty>;
	template<typename _TROy> using reg_obj = TRegisteredObj<_TROy>;

	template <class _Ty, class... Args>
	TRegisteredPointer<_Ty> rnew(Args&&... args) { return registered_new<_Ty>(std::forward<Args>(args)...); }
	template <class _Ty>
	void rdelete(const TRegisteredPointer<_Ty>& regPtrRef) { registered_delete<_Ty>(regPtrRef); }

	/* deprecated aliases */
	template<class _TTargetType, class _TLeasePointerType> using swkfp MSE_DEPRECATED = TSyncWeakFixedPointer<_TTargetType, _TLeasePointerType>;
	template<class _TTargetType, class _TLeasePointerType> using swkfcp MSE_DEPRECATED = TSyncWeakFixedConstPointer<_TTargetType, _TLeasePointerType>;
	template<typename _Ty> using TWRegisteredPointer MSE_DEPRECATED = TNDRegisteredPointer<_Ty>;
	template<typename _Ty> using TWRegisteredConstPointer MSE_DEPRECATED = TNDRegisteredConstPointer<_Ty>;
	template<typename _Ty> using TWRegisteredNotNullPointer MSE_DEPRECATED = TNDRegisteredNotNullPointer<_Ty>;
	template<typename _Ty> using TWRegisteredNotNullConstPointer MSE_DEPRECATED = TNDRegisteredNotNullConstPointer<_Ty>;
	template<typename _Ty> using TWRegisteredFixedPointer MSE_DEPRECATED = TNDRegisteredFixedPointer<_Ty>;
	template<typename _Ty> using TWRegisteredFixedConstPointer MSE_DEPRECATED = TNDRegisteredFixedConstPointer<_Ty>;
	template<typename _TROy> using TWRegisteredObj MSE_DEPRECATED = TNDRegisteredObj<_TROy>;
	template <typename _TLoneParam> MSE_DEPRECATED auto mkrolp(const _TLoneParam& lone_param) { return make_registered(lone_param); }
	template <typename _TLoneParam> MSE_DEPRECATED auto mkrolp(_TLoneParam&& lone_param) { return make_registered(MSE_FWD(lone_param)); }


#ifdef MSEREGISTEREDREFWRAPPER
	template <class _TRRWy> using rrw = TRegisteredRefWrapper<_TRRWy>;

	// TEMPLATE FUNCTIONS ref AND cref
	template<class _TRRy> inline
		TRegisteredRefWrapper<_TRRy>
		registered_ref(TRegisteredObj<_TRRy>& _Val)
	{	// create TRegisteredRefWrapper<_TRRy> object
		return (TRegisteredRefWrapper<_TRRy>(_Val));
	}

	template<class _TRRy>
	void registered_ref(const TRegisteredObj<_TRRy>&&) = delete;

	template<class _TRRy> inline
		TRegisteredRefWrapper<_TRRy>
		registered_ref(TRegisteredRefWrapper<_TRRy> _Val)
	{	// create TRegisteredRefWrapper<_TRRy> object
		return (registered_ref(_Val.get()));
	}

	template<class _TRCRy> inline
		TRegisteredRefWrapper<const _TRCRy>
		registered_cref(const TRegisteredObj<_TRCRy>& _Val)
	{	// create TRegisteredRefWrapper<const _TRCRy> object
		return (TRegisteredRefWrapper<const _TRCRy>(_Val));
	}

	template<class _TRCRy>
	void registered_cref(const TRegisteredObj<_TRCRy>&&) = delete;

	template<class _TRCRy> inline
		TRegisteredRefWrapper<const _TRCRy>
		registered_cref(TRegisteredRefWrapper<_TRCRy> _Val)
	{	// create TRegisteredRefWrapper<const _TRCRy> object
		return (registered_cref(_Val.get()));
	}
#endif // MSEREGISTEREDREFWRAPPER


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
		class CRegPtrTest1 {
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
					static int foo2(mse::TRegisteredPointer<A> A_registered_ptr) { return A_registered_ptr->b; }
				protected:
					~B() {}
				};

				A* A_native_ptr = nullptr;
				/* mse::TRegisteredPointer<> is basically a "safe" version of the native pointer. */
				mse::TRegisteredPointer<A> A_registered_ptr1;

				{
					A a;
					mse::TRegisteredObj<A> registered_a;
					/* mse::TRegisteredObj<A> is a class that is publicly derived from A, and so should be a compatible substitute for A
				in almost all cases. */

					assert(a.b == registered_a.b);
					A_native_ptr = &a;
					A_registered_ptr1 = &registered_a;
					assert(A_native_ptr->b == A_registered_ptr1->b);

					mse::TRegisteredPointer<A> A_registered_ptr2 = &registered_a;
					A_registered_ptr2 = nullptr;
#ifndef MSE_REGISTEREDPOINTER_DISABLED
					bool expected_exception = false;
					MSE_TRY {
						int i = A_registered_ptr2->b; /* this is gonna throw an exception */
					}
					MSE_CATCH_ANY {
						//std::cerr << "expected exception" << std::endl;
						expected_exception = true;
						/* The exception is triggered by an attempt to dereference a null "registered pointer". */
					}
					assert(expected_exception);
#endif // !MSE_REGISTEREDPOINTER_DISABLED

					/* mse::TRegisteredPointers can be coerced into native pointers if you need to interact with legacy code or libraries. */
					//B::foo1(static_cast<A*>(A_registered_ptr1));

					if (A_registered_ptr2) {
						assert(false);
					}
					else if (A_registered_ptr2 != A_registered_ptr1) {
						A_registered_ptr2 = A_registered_ptr1;
						assert(A_registered_ptr2 == A_registered_ptr1);
					}
					else {
						assert(false);
					}

					A a2 = a;
					mse::TRegisteredObj<A> registered_a2 = registered_a;

					a2 = A();
					registered_a2 = mse::TRegisteredObj<A>();

					A a3((A()));
					mse::TRegisteredObj<A> registered_a3((A()));
					{
						mse::TRegisteredObj<A> registered_a4((mse::TRegisteredObj<A>()));
					}

					mse::TRegisteredConstPointer<A> rcp = A_registered_ptr1;
					mse::TRegisteredConstPointer<A> rcp2 = rcp;
					const mse::TRegisteredObj<A> cregistered_a;
					rcp = &cregistered_a;
					mse::TRegisteredFixedConstPointer<A> rfcp = &cregistered_a;
					rcp = mse::registered_new<A>();
					mse::registered_delete<A>(rcp);
				}

				bool expected_exception = false;
#ifndef MSE_REGISTEREDPOINTER_DISABLED
				MSE_TRY {
					/* A_registered_ptr1 "knows" that the (registered) object it was pointing to has now been deallocated. */
					int i = A_registered_ptr1->b; /* So this is gonna throw an exception */
				}
				MSE_CATCH_ANY {
					//std::cerr << "expected exception" << std::endl;
					expected_exception = true;
				}
				assert(expected_exception);
#endif // !MSE_REGISTEREDPOINTER_DISABLED

				{
					/* For heap allocations mse::registered_new is kind of analagous to std::make_shared, but again,
				mse::TRegisteredPointers don't take ownership so you are responsible for deallocation. */
					auto A_registered_ptr3 = mse::registered_new<A>();
					assert(3 == A_registered_ptr3->b);
					mse::registered_delete<A>(A_registered_ptr3);
					bool expected_exception = false;
#ifndef MSE_REGISTEREDPOINTER_DISABLED
					MSE_TRY {
						/* A_registered_ptr3 "knows" that the (registered) object it was pointing to has now been deallocated. */
						int i = A_registered_ptr3->b; /* So this is gonna throw an exception */
					}
					MSE_CATCH_ANY {
						//std::cerr << "expected exception" << std::endl;
						expected_exception = true;
					}
					assert(expected_exception);
#endif // !MSE_REGISTEREDPOINTER_DISABLED
				}

				{
					/* Remember that registered pointers can only point to registered objects. So, for example, if you want
				a registered pointer to an object's base class object, that base class object has to be a registered
				object. */
					class DA : public mse::TRegisteredObj<A> {};
					mse::TRegisteredObj<DA> registered_da;
					mse::TRegisteredPointer<DA> DA_registered_ptr1 = &registered_da;
					mse::TRegisteredPointer<A> A_registered_ptr4 = DA_registered_ptr1;
					A_registered_ptr4 = &registered_da;
					mse::TRegisteredFixedPointer<A> A_registered_fptr1 = &registered_da;
					mse::TRegisteredFixedConstPointer<A> A_registered_fcptr1 = &registered_da;
				}

				{
					/* Obtaining safe pointers to members of registered objects: */
					class E {
					public:
						virtual ~E() {}
						mse::TRegisteredObj<std::string> reg_s = "some text ";
						std::string s2 = "some other text ";
					};

					mse::TRegisteredObj<E> registered_e;
					mse::TRegisteredPointer<E> E_registered_ptr1 = &registered_e;

					/* To obtain a safe pointer to a member of a registered object you could just make the
				member itself a registered object. */
					mse::TRegisteredPointer<std::string> reg_s_registered_ptr1 = &(E_registered_ptr1->reg_s);

					/* Or you can use the "mse::make_pointer_to_member_v2()" function. */
					auto s2_safe_ptr1 = mse::make_pointer_to_member_v2(E_registered_ptr1, &E::s2);
					(*s2_safe_ptr1) = "some new text";
					auto s2_safe_const_ptr1 = mse::make_const_pointer_to_member_v2(E_registered_ptr1, &E::s2);

					/* Just testing the convertibility of mse::TSyncWeakFixedPointers. */
					auto E_registered_fixed_ptr1 = &registered_e;
					auto swfptr1 = mse::make_syncweak<std::string>(E_registered_fixed_ptr1->s2, E_registered_fixed_ptr1);
					mse::TSyncWeakFixedPointer<std::string, mse::TRegisteredPointer<E>> swfptr2 = swfptr1;
					mse::TSyncWeakFixedConstPointer<std::string, mse::TRegisteredFixedPointer<E>> swfcptr1 = swfptr1;
					mse::TSyncWeakFixedConstPointer<std::string, mse::TRegisteredPointer<E>> swfcptr2 = swfcptr1;
					if (swfcptr1 == swfptr1) {
						int q = 7;
					}
					if (swfptr1 == swfcptr1) {
						int q = 7;
					}
					if (swfptr1) {
						int q = 7;
					}
				}

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

#endif // MSEREGISTERED_H_
