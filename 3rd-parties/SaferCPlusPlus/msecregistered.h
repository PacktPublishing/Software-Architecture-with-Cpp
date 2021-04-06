
// Copyright (c) 2015 Noah Lopez
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/* TCRegisteredPointers are basically just like TRegisteredPointers except that unlike TRegisteredPointers, they
are able to reference target object types before those target types are (fully) defined. This is required to support
"mutually referencing" objects or "cyclic references".

The implementation difference is that with TRegisteredPointers, the "pointer tracking registry" is located in the target
object, whereas TCRegisteredPointers use (thread local) global registries that track all the pointers targeting
objects of a given type. */

#pragma once
#ifndef MSECREGISTERED_H_
#define MSECREGISTERED_H_

#include "msepointerbasics.h"
#include "mseprimitives.h"
#include <utility>
#include <unordered_map>
#include <mutex>
#include <cassert>
//include <typeinfo>      // std::bad_cast
#include <stdexcept>

#if defined(MSE_SAFER_SUBSTITUTES_DISABLED) || defined(MSE_SAFERPTR_DISABLED)
#define MSE_REGISTEREDPOINTER_DISABLED
#endif /*defined(MSE_SAFER_SUBSTITUTES_DISABLED) || defined(MSE_SAFERPTR_DISABLED)*/

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

namespace mse {

	template<typename _Ty> class TNDCRegisteredObj;
	template<typename _Ty> class TNDCRegisteredPointer;
	template<typename _Ty> class TNDCRegisteredConstPointer;
	template<typename _Ty> class TNDCRegisteredNotNullPointer;
	template<typename _Ty> class TNDCRegisteredNotNullConstPointer;
	template<typename _Ty> class TNDCRegisteredFixedPointer;
	template<typename _Ty> class TNDCRegisteredFixedConstPointer;

	template<typename _Ty>
	auto ndcregistered_fptr_to(_Ty&& _X) {
		return _X.mse_cregistered_fptr();
	}
	template<typename _Ty>
	auto ndcregistered_fptr_to(const _Ty& _X) {
		return _X.mse_cregistered_fptr();
	}

	template <class _Ty, class... Args> TNDCRegisteredPointer<_Ty> ndcregistered_new(Args&&... args);
	template <class _Ty> void ndcregistered_delete(const TNDCRegisteredPointer<_Ty>& ndcregisteredPtrRef);
	template <class _Ty> void ndcregistered_delete(const TNDCRegisteredConstPointer<_Ty>& ndcregisteredPtrRef);
	namespace us {
		template <class _Ty> void ndcregistered_delete(const TNDCRegisteredPointer<_Ty>& ndcregisteredPtrRef);
		template <class _Ty> void ndcregistered_delete(const TNDCRegisteredConstPointer<_Ty>& ndcregisteredPtrRef);
	}

	namespace impl {
		template<typename _Ty, class... Args>
		auto make_ndcregistered_helper(std::true_type, Args&&... args) {
			return _Ty(std::forward<Args>(args)...);
		}
		template<typename _Ty, class... Args>
		auto make_ndcregistered_helper(std::false_type, Args&&... args) {
			return TNDCRegisteredObj<_Ty>(std::forward<Args>(args)...);
		}
	}
	template <class X, class... Args>
	auto make_ndcregistered(Args&&... args) {
		typedef mse::impl::remove_reference_t<X> nrX;
		return impl::make_ndcregistered_helper<nrX>(typename mse::impl::is_instantiation_of<nrX, TNDCRegisteredObj>::type(), std::forward<Args>(args)...);
	}
	template <class X>
	auto make_ndcregistered(const X& arg) {
		typedef mse::impl::remove_reference_t<X> nrX;
		return impl::make_ndcregistered_helper<nrX>(typename mse::impl::is_instantiation_of<nrX, TNDCRegisteredObj>::type(), arg);
	}
	template <class X>
	auto make_ndcregistered(X&& arg) {
		typedef mse::impl::remove_reference_t<X> nrX;
		return impl::make_ndcregistered_helper<nrX>(typename mse::impl::is_instantiation_of<nrX, TNDCRegisteredObj>::type(), MSE_FWD(arg));
	}

#ifdef MSE_HAS_CXX17
	/* deduction guide */
	template<class _TROy> TNDCRegisteredObj(_TROy)->TNDCRegisteredObj<_TROy>;
#endif /* MSE_HAS_CXX17 */

#ifdef MSE_REGISTEREDPOINTER_DISABLED
	template<typename _Ty> using TCRegisteredPointer = _Ty*;
	template<typename _Ty> using TCRegisteredConstPointer = const _Ty*;
	template<typename _Ty> using TCRegisteredNotNullPointer = _Ty*;
	template<typename _Ty> using TCRegisteredNotNullConstPointer = const _Ty*;
	template<typename _Ty> using TCRegisteredFixedPointer = _Ty* /*const*/; /* Can't be const qualified because standard
																				  library containers don't support const elements. */
	template<typename _Ty> using TCRegisteredFixedConstPointer = const _Ty* /*const*/;
	template<typename _TROFLy> using TCRegisteredObj = _TROFLy;

	template<typename _Ty> auto cregistered_fptr_to(_Ty&& _X) { return std::addressof(_X); }
	template<typename _Ty> auto cregistered_fptr_to(const _Ty& _X) { return std::addressof(_X); }

	template <class _Ty, class... Args>
	TCRegisteredPointer<_Ty> cregistered_new(Args&&... args) {
		return new TCRegisteredObj<_Ty>(std::forward<Args>(args)...);
	}
	template <class _Ty>
	void cregistered_delete(const TCRegisteredPointer<_Ty>& regPtrRef) {
		auto a = static_cast<TCRegisteredObj<_Ty>*>(regPtrRef);
		delete a;
	}
	template <class _Ty>
	void cregistered_delete(const TCRegisteredConstPointer<_Ty>& regPtrRef) {
		auto a = static_cast<const TCRegisteredObj<_Ty>*>(regPtrRef);
		delete a;
	}
	namespace us {
		template <class _Ty>
		void cregistered_delete(const TCRegisteredPointer<_Ty>& regPtrRef) {
			mse::cregistered_delete(regPtrRef);
		}
		template <class _Ty>
		void cregistered_delete(const TCRegisteredConstPointer<_Ty>& regPtrRef) {
			mse::cregistered_delete(regPtrRef);
		}
	}

	template <class X, class... Args>
	auto make_cregistered(Args&&... args) {
		typedef mse::impl::remove_reference_t<X> nrX;
		return nrX(std::forward<Args>(args)...);
	}
	template <class X>
	auto make_cregistered(const X& arg) {
		typedef mse::impl::remove_reference_t<X> nrX;
		return nrX(arg);
	}
	template <class X>
	auto make_cregistered(X&& arg) {
		typedef mse::impl::remove_reference_t<X> nrX;
		return nrX(MSE_FWD(arg));
	}

#else /*MSE_REGISTEREDPOINTER_DISABLED*/

	class cregistered_cannot_verify_cast_error : public std::logic_error { public:
		using std::logic_error::logic_error;
	};

	template<typename _Ty> using TCRegisteredPointer = TNDCRegisteredPointer<_Ty>;
	template<typename _Ty> using TCRegisteredConstPointer = TNDCRegisteredConstPointer<_Ty>;
	template<typename _Ty> using TCRegisteredNotNullPointer = TNDCRegisteredNotNullPointer<_Ty>;
	template<typename _Ty> using TCRegisteredNotNullConstPointer = TNDCRegisteredNotNullConstPointer<_Ty>;
	template<typename _Ty> using TCRegisteredFixedPointer = TNDCRegisteredFixedPointer<_Ty>;
	template<typename _Ty> using TCRegisteredFixedConstPointer = TNDCRegisteredFixedConstPointer<_Ty>;
	template<typename _TROFLy> using TCRegisteredObj = TNDCRegisteredObj<_TROFLy>;

	template<typename _Ty> auto cregistered_fptr_to(_Ty&& _X) { return ndcregistered_fptr_to(MSE_FWD(_X)); }
	template<typename _Ty> auto cregistered_fptr_to(const _Ty& _X) { return ndcregistered_fptr_to(_X); }

	template <class _Ty, class... Args> TNDCRegisteredPointer<_Ty> cregistered_new(Args&&... args) { return ndcregistered_new<_Ty>(std::forward<Args>(args)...); }
	template <class _Ty> void cregistered_delete(TNDCRegisteredPointer<_Ty>& ndcregisteredPtrRef) { return mse::ndcregistered_delete<_Ty>(ndcregisteredPtrRef); }
	template <class _Ty> void cregistered_delete(TNDCRegisteredConstPointer<_Ty>& ndcregisteredPtrRef) { return mse::ndcregistered_delete<_Ty>(ndcregisteredPtrRef); }
	namespace us {
		template <class _Ty> void cregistered_delete(TNDCRegisteredPointer<_Ty>& ndcregisteredPtrRef) { return mse::us::ndcregistered_delete<_Ty>(ndcregisteredPtrRef); }
		template <class _Ty> void cregistered_delete(TNDCRegisteredConstPointer<_Ty>& ndcregisteredPtrRef) { return mse::us::ndcregistered_delete<_Ty>(ndcregisteredPtrRef); }
	}

	template <class X, class... Args>
	auto make_cregistered(Args&&... args) {
		return make_ndcregistered<X>(std::forward<Args>(args)...);
	}
	template <class X>
	auto make_cregistered(const X& arg) {
		return make_ndcregistered(arg);
	}
	template <class X>
	auto make_cregistered(X&& arg) {
		return make_ndcregistered(MSE_FWD(arg));
	}

#endif /*MSE_REGISTEREDPOINTER_DISABLED*/

	namespace us {
		namespace impl {
			class CCRegisteredNode;

			class CCRNMutablePointer {
			public:
				CCRNMutablePointer(const CCRegisteredNode* ptr) : m_ptr(ptr) {}
				/* explicit */CCRNMutablePointer(const CCRNMutablePointer& src_cref) : m_ptr(src_cref.m_ptr) {}

				auto& operator*() const {
					return (*m_ptr);
				}
				auto operator->() const {
					return m_ptr;
				}
				/* The point of this class is the const at the end. */
				auto& operator=(const CCRNMutablePointer& src_cref) const {
					m_ptr = src_cref.m_ptr;
					return (*this);
				}
				operator bool() const {
					return (m_ptr != nullptr);
				}

			private:
				mutable const CCRegisteredNode* m_ptr = nullptr;
			};

			/* node of a (doubly-linked) list of pointers */
			class CCRegisteredNode {
			public:
				virtual void rn_set_pointer_to_null() const = 0;
				void set_next_ptr(mse::us::impl::CCRNMutablePointer next_ptr) const {
					m_next_ptr = next_ptr;
				}
				mse::us::impl::CCRNMutablePointer get_next_ptr() const {
					return m_next_ptr;
				}
				void set_prev_next_ptr_ptr(const mse::us::impl::CCRNMutablePointer* prev_next_ptr_ptr) const {
					m_prev_next_ptr_ptr = prev_next_ptr_ptr;
				}
				const mse::us::impl::CCRNMutablePointer* get_prev_next_ptr_ptr() const {
					return m_prev_next_ptr_ptr;
				}
				const mse::us::impl::CCRNMutablePointer* get_address_of_my_next_ptr() const {
					return &m_next_ptr;
				}

			private:
				mutable mse::us::impl::CCRNMutablePointer m_next_ptr = nullptr;
				mutable const mse::us::impl::CCRNMutablePointer* m_prev_next_ptr_ptr = nullptr;
			};
		}
	}

	/* TNDCRegisteredPointer is similar to TNDRegisteredPointer but uses a slightly different implementation with a little
	more overhead but better worse-case performance. */
	template<typename _Ty>
	class TNDCRegisteredPointer : public mse::us::TSaferPtr<TNDCRegisteredObj<_Ty>>, public mse::us::impl::CCRegisteredNode {
	public:
		TNDCRegisteredPointer() : mse::us::TSaferPtr<TNDCRegisteredObj<_Ty>>() {}
		TNDCRegisteredPointer(const TNDCRegisteredPointer& src_cref) : mse::us::TSaferPtr<TNDCRegisteredObj<_Ty>>(src_cref.m_ptr) {
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).register_pointer(*this);
			}
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDCRegisteredPointer(const TNDCRegisteredPointer<_Ty2>& src_cref) : mse::us::TSaferPtr<TNDCRegisteredObj<_Ty>>(src_cref.m_ptr) {
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).register_pointer(*this);
			}
		}
		TNDCRegisteredPointer(std::nullptr_t) : mse::us::TSaferPtr<TNDCRegisteredObj<_Ty>>(nullptr) {}
		virtual ~TNDCRegisteredPointer() {
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).unregister_pointer(*this);
			}
		}
		TNDCRegisteredPointer<_Ty>& operator=(const TNDCRegisteredPointer<_Ty>& _Right_cref) {
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).unregister_pointer(*this);
			}
			mse::us::TSaferPtr<TNDCRegisteredObj<_Ty>>::operator=(_Right_cref);
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).register_pointer(*this);
			}
			return (*this);
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDCRegisteredPointer<_Ty>& operator=(const TNDCRegisteredPointer<_Ty2>& _Right_cref) {
			return (*this).operator=(TNDCRegisteredPointer(_Right_cref));
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
		then the (derived) object's destructor won't be called possibly resulting in resource leaks. With cregistered
		objects, the destructor not being called also circumvents their memory safety mechanism. */
		void cregistered_delete() const {
			auto a = asANativePointerToTNDCRegisteredObj();
			delete a;
			assert(nullptr == (*this).m_ptr);
		}

		/* todo: make this private */
		void rn_set_pointer_to_null() const override { (*this).spb_set_to_null(); }

	private:
		TNDCRegisteredPointer(TNDCRegisteredObj<_Ty>* ptr) : mse::us::TSaferPtr<TNDCRegisteredObj<_Ty>>(ptr) {
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).register_pointer(*this);
			}
		}

		/* This function, if possible, should not be used. It is meant to be used exclusively by cregistered_delete<>(). */
		TNDCRegisteredObj<_Ty>* asANativePointerToTNDCRegisteredObj() const {
#ifdef NATIVE_PTR_DEBUG_HELPER1
			if (nullptr == (*this).m_ptr) {
				int q = 5; /* just a line of code for putting a debugger break point */
			}
#endif /*NATIVE_PTR_DEBUG_HELPER1*/
			return static_cast<TNDCRegisteredObj<_Ty>*>((*this).m_ptr);
		}

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		template <class Y> friend class TNDCRegisteredPointer;
		template <class Y> friend class TNDCRegisteredConstPointer;
		friend class TNDCRegisteredNotNullPointer<_Ty>;
	};

	template<typename _Ty>
	class TNDCRegisteredConstPointer : public mse::us::TSaferPtr<const TNDCRegisteredObj<_Ty>>, public mse::us::impl::CCRegisteredNode {
	public:
		TNDCRegisteredConstPointer() : mse::us::TSaferPtr<const TNDCRegisteredObj<_Ty>>() {}
		TNDCRegisteredConstPointer(const TNDCRegisteredConstPointer& src_cref) : mse::us::TSaferPtr<const TNDCRegisteredObj<_Ty>>(src_cref.m_ptr) {
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).register_pointer(*this);
			}
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDCRegisteredConstPointer(const TNDCRegisteredConstPointer<_Ty2>& src_cref) : mse::us::TSaferPtr<const TNDCRegisteredObj<_Ty>>(src_cref.m_ptr) {
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).register_pointer(*this);
			}
		}
		TNDCRegisteredConstPointer(const TNDCRegisteredPointer<_Ty>& src_cref) : mse::us::TSaferPtr<const TNDCRegisteredObj<_Ty>>(src_cref.m_ptr) {
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).register_pointer(*this);
			}
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDCRegisteredConstPointer(const TNDCRegisteredPointer<_Ty2>& src_cref) : mse::us::TSaferPtr<const TNDCRegisteredObj<_Ty>>(src_cref.m_ptr) {
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).register_pointer(*this);
			}
		}
		TNDCRegisteredConstPointer(std::nullptr_t) : mse::us::TSaferPtr<const TNDCRegisteredObj<_Ty>>(nullptr) {}
		virtual ~TNDCRegisteredConstPointer() {
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).unregister_pointer(*this);
			}
		}
		TNDCRegisteredConstPointer<_Ty>& operator=(const TNDCRegisteredConstPointer<_Ty>& _Right_cref) {
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).unregister_pointer(*this);
			}
			mse::us::TSaferPtr<const TNDCRegisteredObj<_Ty>>::operator=(_Right_cref);
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).register_pointer(*this);
			}
			return (*this);
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDCRegisteredConstPointer<_Ty>& operator=(const TNDCRegisteredConstPointer<_Ty2>& _Right_cref) {
			return (*this).operator=(TNDCRegisteredConstPointer(_Right_cref));
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
		then the (derived) object's destructor won't be called possibly resulting in resource leaks. With cregistered
		objects, the destructor not being called also circumvents their memory safety mechanism. */
		void cregistered_delete() const {
			auto a = asANativePointerToTNDCRegisteredObj();
			delete a;
			assert(nullptr == (*this).m_ptr);
		}

		/* todo: make this private */
		void rn_set_pointer_to_null() const override { (*this).spb_set_to_null(); }

	private:
		TNDCRegisteredConstPointer(const TNDCRegisteredObj<_Ty>* ptr) : mse::us::TSaferPtr<const TNDCRegisteredObj<_Ty>>(ptr) {
			if (nullptr != (*this).m_ptr) {
				(*((*this).m_ptr)).register_pointer(*this);
			}
		}

		/* This function, if possible, should not be used. It is meant to be used exclusively by cregistered_delete<>(). */
		const TNDCRegisteredObj<_Ty>* asANativePointerToTNDCRegisteredObj() const {
#ifdef NATIVE_PTR_DEBUG_HELPER1
			if (nullptr == (*this).m_ptr) {
				int q = 5; /* just a line of code for putting a debugger break point */
			}
#endif /*NATIVE_PTR_DEBUG_HELPER1*/
			return static_cast<const TNDCRegisteredObj<_Ty>*>((*this).m_ptr);
		}

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		template <class Y> friend class TNDCRegisteredConstPointer;
		friend class TNDCRegisteredNotNullConstPointer<_Ty>;
	};

	template<typename _Ty>
	class TNDCRegisteredNotNullPointer : public TNDCRegisteredPointer<_Ty> {
	public:
		TNDCRegisteredNotNullPointer(const TNDCRegisteredNotNullPointer& src_cref) : TNDCRegisteredPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDCRegisteredNotNullPointer(const TNDCRegisteredNotNullPointer<_Ty2>& src_cref) : TNDCRegisteredPointer<_Ty>(src_cref) {}

		virtual ~TNDCRegisteredNotNullPointer() {}
		/*
		TNDCRegisteredNotNullPointer<_Ty>& operator=(const TNDCRegisteredNotNullPointer<_Ty>& _Right_cref) {
			TNDCRegisteredPointer<_Ty>::operator=(_Right_cref);
			return (*this);
		}
		*/
		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator _Ty*() const { return TNDCRegisteredPointer<_Ty>::operator _Ty*(); }
		MSE_DEPRECATED explicit operator TNDCRegisteredObj<_Ty>*() const { return TNDCRegisteredPointer<_Ty>::operator TNDCRegisteredObj<_Ty>*(); }

	private:
		TNDCRegisteredNotNullPointer(TNDCRegisteredObj<_Ty>* ptr) : TNDCRegisteredPointer<_Ty>(ptr) {}

		/* If you want to use this constructor, use not_null_from_nullable() instead. */
		TNDCRegisteredNotNullPointer(const  TNDCRegisteredPointer<_Ty>& src_cref) : TNDCRegisteredPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDCRegisteredNotNullPointer(const TNDCRegisteredPointer<_Ty2>& src_cref) : TNDCRegisteredPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}

		/* If you want a pointer to a TNDCRegisteredNotNullPointer<_Ty>, declare the TNDCRegisteredNotNullPointer<_Ty> as a
		TNDCRegisteredObj<TNDCRegisteredNotNullPointer<_Ty>> instead. So for example:
		auto reg_ptr = TNDCRegisteredObj<TNDCRegisteredNotNullPointer<_Ty>>(mse::cregistered_new<_Ty>());
		auto reg_ptr_to_reg_ptr = &reg_ptr;
		*/
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		friend class TNDCRegisteredFixedPointer<_Ty>;
		template<typename _Ty2>
		friend TNDCRegisteredNotNullPointer<_Ty2> not_null_from_nullable(const TNDCRegisteredPointer<_Ty2>& src);
	};

	template<typename _Ty>
	class TNDCRegisteredNotNullConstPointer : public TNDCRegisteredConstPointer<_Ty> {
	public:
		TNDCRegisteredNotNullConstPointer(const TNDCRegisteredNotNullPointer<_Ty>& src_cref) : TNDCRegisteredConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDCRegisteredNotNullConstPointer(const TNDCRegisteredNotNullPointer<_Ty2>& src_cref) : TNDCRegisteredConstPointer<_Ty>(src_cref) {}
		TNDCRegisteredNotNullConstPointer(const TNDCRegisteredNotNullConstPointer<_Ty>& src_cref) : TNDCRegisteredConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDCRegisteredNotNullConstPointer(const TNDCRegisteredNotNullConstPointer<_Ty2>& src_cref) : TNDCRegisteredConstPointer<_Ty>(src_cref) {}

		virtual ~TNDCRegisteredNotNullConstPointer() {}
		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator const _Ty*() const { return TNDCRegisteredConstPointer<_Ty>::operator const _Ty*(); }
		MSE_DEPRECATED explicit operator const TNDCRegisteredObj<_Ty>*() const { return TNDCRegisteredConstPointer<_Ty>::operator const TNDCRegisteredObj<_Ty>*(); }

	private:
		TNDCRegisteredNotNullConstPointer(const TNDCRegisteredObj<_Ty>* ptr) : TNDCRegisteredConstPointer<_Ty>(ptr) {}

		/* If you want to use this constructor, use not_null_from_nullable() instead. */
		TNDCRegisteredNotNullConstPointer(const TNDCRegisteredPointer<_Ty>& src_cref) : TNDCRegisteredConstPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDCRegisteredNotNullConstPointer(const TNDCRegisteredPointer<_Ty2>& src_cref) : TNDCRegisteredConstPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}
		TNDCRegisteredNotNullConstPointer(const TNDCRegisteredConstPointer<_Ty>& src_cref) : TNDCRegisteredConstPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDCRegisteredNotNullConstPointer(const TNDCRegisteredConstPointer<_Ty2>& src_cref) : TNDCRegisteredConstPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		friend class TNDCRegisteredFixedConstPointer<_Ty>;
		template<typename _Ty2>
		friend TNDCRegisteredNotNullConstPointer<_Ty2> not_null_from_nullable(const TNDCRegisteredConstPointer<_Ty2>& src);
	};

	template<typename _Ty>
	TNDCRegisteredNotNullPointer<_Ty> not_null_from_nullable(const TNDCRegisteredPointer<_Ty>& src) {
		return src;
	}
	template<typename _Ty>
	TNDCRegisteredNotNullConstPointer<_Ty> not_null_from_nullable(const TNDCRegisteredConstPointer<_Ty>& src) {
		return src;
	}

	/* TNDCRegisteredFixedPointer cannot be retargeted or constructed without a target. This pointer is recommended for passing
	parameters by reference. */
	template<typename _Ty>
	class TNDCRegisteredFixedPointer : public TNDCRegisteredNotNullPointer<_Ty> {
	public:
		TNDCRegisteredFixedPointer(const TNDCRegisteredFixedPointer& src_cref) : TNDCRegisteredNotNullPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDCRegisteredFixedPointer(const TNDCRegisteredFixedPointer<_Ty2>& src_cref) : TNDCRegisteredNotNullPointer<_Ty>(src_cref) {}

		TNDCRegisteredFixedPointer(const TNDCRegisteredNotNullPointer<_Ty>& src_cref) : TNDCRegisteredNotNullPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDCRegisteredFixedPointer(const TNDCRegisteredNotNullPointer<_Ty2>& src_cref) : TNDCRegisteredNotNullPointer<_Ty>(src_cref) {}

		virtual ~TNDCRegisteredFixedPointer() {}

		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator _Ty*() const { return TNDCRegisteredNotNullPointer<_Ty>::operator _Ty*(); }
		MSE_DEPRECATED explicit operator TNDCRegisteredObj<_Ty>*() const { return TNDCRegisteredNotNullPointer<_Ty>::operator TNDCRegisteredObj<_Ty>*(); }

	private:
		TNDCRegisteredFixedPointer(TNDCRegisteredObj<_Ty>* ptr) : TNDCRegisteredNotNullPointer<_Ty>(ptr) {}
		TNDCRegisteredFixedPointer<_Ty>& operator=(const TNDCRegisteredFixedPointer<_Ty>& _Right_cref) = delete;

		/* If you want a pointer to a TNDCRegisteredFixedPointer<_Ty>, declare the TNDCRegisteredFixedPointer<_Ty> as a
		TNDCRegisteredObj<TNDCRegisteredFixedPointer<_Ty>> instead. So for example:
		auto reg_ptr = TNDCRegisteredObj<TNDCRegisteredFixedPointer<_Ty>>(mse::cregistered_new<_Ty>());
		auto reg_ptr_to_reg_ptr = &reg_ptr;
		*/
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		friend class TNDCRegisteredObj<_Ty>;
	};

	template<typename _Ty>
	class TNDCRegisteredFixedConstPointer : public TNDCRegisteredNotNullConstPointer<_Ty> {
	public:
		TNDCRegisteredFixedConstPointer(const TNDCRegisteredFixedPointer<_Ty>& src_cref) : TNDCRegisteredNotNullConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDCRegisteredFixedConstPointer(const TNDCRegisteredFixedPointer<_Ty2>& src_cref) : TNDCRegisteredNotNullConstPointer<_Ty>(src_cref) {}
		TNDCRegisteredFixedConstPointer(const TNDCRegisteredFixedConstPointer<_Ty>& src_cref) : TNDCRegisteredNotNullConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDCRegisteredFixedConstPointer(const TNDCRegisteredFixedConstPointer<_Ty2>& src_cref) : TNDCRegisteredNotNullConstPointer<_Ty>(src_cref) {}

		TNDCRegisteredFixedConstPointer(const TNDCRegisteredNotNullPointer<_Ty>& src_cref) : TNDCRegisteredNotNullConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDCRegisteredFixedConstPointer(const TNDCRegisteredNotNullPointer<_Ty2>& src_cref) : TNDCRegisteredNotNullConstPointer<_Ty>(src_cref) {}
		TNDCRegisteredFixedConstPointer(const TNDCRegisteredNotNullConstPointer<_Ty>& src_cref) : TNDCRegisteredNotNullConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDCRegisteredFixedConstPointer(const TNDCRegisteredNotNullConstPointer<_Ty2>& src_cref) : TNDCRegisteredNotNullConstPointer<_Ty>(src_cref) {}

		virtual ~TNDCRegisteredFixedConstPointer() {}
		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator const _Ty*() const { return TNDCRegisteredNotNullConstPointer<_Ty>::operator const _Ty*(); }
		MSE_DEPRECATED explicit operator const TNDCRegisteredObj<_Ty>*() const { return TNDCRegisteredNotNullConstPointer<_Ty>::operator const TNDCRegisteredObj<_Ty>*(); }

	private:
		TNDCRegisteredFixedConstPointer(const TNDCRegisteredObj<_Ty>* ptr) : TNDCRegisteredNotNullConstPointer<_Ty>(ptr) {}
		TNDCRegisteredFixedConstPointer<_Ty>& operator=(const TNDCRegisteredFixedConstPointer<_Ty>& _Right_cref) = delete;

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		friend class TNDCRegisteredObj<_Ty>;
	};

	/* This macro roughly simulates constructor inheritance. */
#define MSE_CREGISTERED_OBJ_USING(Derived, Base) MSE_USING(Derived, Base)

	/* TNDCRegisteredObj is intended as a transparent wrapper for other classes/objects. The purpose is to register the object's
	destruction so that TNDCRegisteredPointers will avoid referencing destroyed objects. Note that TNDCRegisteredObj can be used with
	objects allocated on the stack. */
	template<typename _TROFLy>
	class TNDCRegisteredObj : public _TROFLy, public mse::impl::first_or_placeholder_if_base_of_second<mse::us::impl::AsyncNotShareableTagBase, _TROFLy, TNDCRegisteredObj<_TROFLy> >
	{
	public:
		typedef _TROFLy base_class;

		MSE_CREGISTERED_OBJ_USING(TNDCRegisteredObj, _TROFLy);
		TNDCRegisteredObj(const TNDCRegisteredObj& _X) : _TROFLy(_X) {}
		TNDCRegisteredObj(TNDCRegisteredObj&& _X) : _TROFLy(MSE_FWD(_X)) {}
		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TNDCRegisteredObj() {
			unregister_and_set_outstanding_pointers_to_null();
		}

		TNDCRegisteredObj& operator=(TNDCRegisteredObj&& _X) { _TROFLy::operator=(MSE_FWD(_X)); return (*this); }
		TNDCRegisteredObj& operator=(const TNDCRegisteredObj& _X) { _TROFLy::operator=(_X); return (*this); }
		template<class _Ty2>
		TNDCRegisteredObj& operator=(_Ty2&& _X) { _TROFLy::operator=(MSE_FWD(_X)); return (*this); }
		template<class _Ty2>
		TNDCRegisteredObj& operator=(const _Ty2& _X) { _TROFLy::operator=(_X); return (*this); }

		TNDCRegisteredNotNullPointer<_TROFLy> operator&() {
			return TNDCRegisteredFixedPointer<_TROFLy>(this);
		}
		TNDCRegisteredNotNullConstPointer<_TROFLy> operator&() const {
			return TNDCRegisteredFixedConstPointer<_TROFLy>(this);
		}
		TNDCRegisteredNotNullPointer<_TROFLy> mse_cregistered_nnptr() { return TNDCRegisteredFixedPointer<_TROFLy>(this); }
		TNDCRegisteredNotNullConstPointer<_TROFLy> mse_cregistered_nnptr() const { return TNDCRegisteredFixedConstPointer<_TROFLy>(this); }
		TNDCRegisteredFixedPointer<_TROFLy> mse_cregistered_fptr() { return TNDCRegisteredFixedPointer<_TROFLy>(this); }
		TNDCRegisteredFixedConstPointer<_TROFLy> mse_cregistered_fptr() const { return TNDCRegisteredFixedConstPointer<_TROFLy>(this); }

	private:
		void register_pointer(const mse::us::impl::CCRegisteredNode& node_cref) const {
			if (m_head_ptr) {
				m_head_ptr->set_prev_next_ptr_ptr(node_cref.get_address_of_my_next_ptr());
			}
			node_cref.set_next_ptr(m_head_ptr);
			node_cref.set_prev_next_ptr_ptr(&m_head_ptr);
			m_head_ptr = &node_cref;
		}
		static void unregister_pointer(const mse::us::impl::CCRegisteredNode& node_cref) {
			assert(node_cref.get_prev_next_ptr_ptr());
			(*(node_cref.get_prev_next_ptr_ptr())) = node_cref.get_next_ptr();
			if (node_cref.get_next_ptr()) {
				node_cref.get_next_ptr()->set_prev_next_ptr_ptr(node_cref.get_prev_next_ptr_ptr());
			}
			node_cref.set_prev_next_ptr_ptr(nullptr);
			node_cref.set_next_ptr(nullptr);
		}

		void unregister_and_set_outstanding_pointers_to_null() const {
			auto current_node_ptr = m_head_ptr;
			while (current_node_ptr) {
				current_node_ptr->rn_set_pointer_to_null();
				current_node_ptr->set_prev_next_ptr_ptr(nullptr);
				auto next_ptr = current_node_ptr->get_next_ptr();
				current_node_ptr->set_next_ptr(nullptr);
				current_node_ptr = next_ptr;
			}
		}

		/* first node in a (doubly-linked) list of pointers targeting this object */
		mutable mse::us::impl::CCRNMutablePointer m_head_ptr = nullptr;

		template<typename _Ty2>
		friend class TNDCRegisteredPointer;
		template<typename _Ty2>
		friend class TNDCRegisteredConstPointer;
	};

	/* See ndregistered_new(). */
	template <class _Ty, class... Args>
	TNDCRegisteredPointer<_Ty> ndcregistered_new(Args&&... args) {
		auto a = new TNDCRegisteredObj<_Ty>(std::forward<Args>(args)...);
		mse::us::impl::tlSAllocRegistry_ref<TNDCRegisteredObj<_Ty> >().registerPointer(a);
		return &(*a);
	}
	template <class _Ty>
	void ndcregistered_delete(const TNDCRegisteredPointer<_Ty>& regPtrRef) {
		auto a = static_cast<TNDCRegisteredObj<_Ty>*>(regPtrRef);
		auto res = mse::us::impl::tlSAllocRegistry_ref<TNDCRegisteredObj<_Ty> >().unregisterPointer(a);
		if (!res) { assert(false); MSE_THROW(std::invalid_argument("invalid argument, no corresponding allocation found - mse::cregistered_delete() \n- tip: If deleting via base class pointer, use mse::us::cregistered_delete() instead. ")); }
		regPtrRef.cregistered_delete();
	}
	template <class _Ty>
	void ndcregistered_delete(const TNDCRegisteredConstPointer<_Ty>& regPtrRef) {
		auto a = static_cast<const TNDCRegisteredObj<_Ty>*>(regPtrRef);
		auto res = mse::us::impl::tlSAllocRegistry_ref<TNDCRegisteredObj<_Ty> >().unregisterPointer(a);
		if (!res) { assert(false); MSE_THROW(std::invalid_argument("invalid argument, no corresponding allocation found - mse::cregistered_delete() \n- tip: If deleting via base class pointer, use mse::us::cregistered_delete() instead. ")); }
		regPtrRef.cregistered_delete();
	}
	namespace us {
		template <class _Ty>
		void ndcregistered_delete(const TNDCRegisteredPointer<_Ty>& regPtrRef) {
			regPtrRef.cregistered_delete();
		}
		template <class _Ty>
		void ndcregistered_delete(const TNDCRegisteredConstPointer<_Ty>& regPtrRef) {
			regPtrRef.cregistered_delete();
		}
	}

}

namespace std {
	template<class _Ty>
	struct hash<mse::TNDCRegisteredPointer<_Ty> > {	// hash functor
		typedef mse::TNDCRegisteredPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDCRegisteredPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
	template<class _Ty>
	struct hash<mse::TNDCRegisteredNotNullPointer<_Ty> > {	// hash functor
		typedef mse::TNDCRegisteredNotNullPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDCRegisteredNotNullPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
	template<class _Ty>
	struct hash<mse::TNDCRegisteredFixedPointer<_Ty> > {	// hash functor
		typedef mse::TNDCRegisteredFixedPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDCRegisteredFixedPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};

	template<class _Ty>
	struct hash<mse::TNDCRegisteredConstPointer<_Ty> > {	// hash functor
		typedef mse::TNDCRegisteredConstPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDCRegisteredConstPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
	template<class _Ty>
	struct hash<mse::TNDCRegisteredNotNullConstPointer<_Ty> > {	// hash functor
		typedef mse::TNDCRegisteredNotNullConstPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDCRegisteredNotNullConstPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
	template<class _Ty>
	struct hash<mse::TNDCRegisteredFixedConstPointer<_Ty> > {	// hash functor
		typedef mse::TNDCRegisteredFixedConstPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDCRegisteredFixedConstPointer<_Ty>& _Keyval) const _NOEXCEPT {
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

#define MSE_NDCREGISTERED_IMPL_OBJ_INHERIT_ASSIGNMENT_OPERATOR(class_name) \
		auto& operator=(class_name&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); } \
		auto& operator=(const class_name& _X) { base_class::operator=(_X); return (*this); } \
		template<class _Ty2> auto& operator=(_Ty2&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); } \
		template<class _Ty2> auto& operator=(const _Ty2& _X) { base_class::operator=(_X); return (*this); }

#define MSE_NDCREGISTERED_IMPL_OBJ_SPECIALIZATION_DEFINITIONS1(class_name) \
		class_name(const class_name&) = default; \
		class_name(class_name&&) = default; \
		MSE_NDCREGISTERED_IMPL_OBJ_INHERIT_ASSIGNMENT_OPERATOR(class_name);

#if !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)
#define MSE_NDCREGISTERED_IMPL_OBJ_NATIVE_POINTER_PRIVATE_CONSTRUCTORS1(class_name) \
			class_name(std::nullptr_t) {} \
			class_name() {}
#else // !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)
#define MSE_NDCREGISTERED_IMPL_OBJ_NATIVE_POINTER_PRIVATE_CONSTRUCTORS1(class_name)
#endif // !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)

	/* Note that because we explicitly define some (private) constructors, default copy and move constructors
	and assignment operators won't be generated, so we have to define those as well. */
#define MSE_NDCREGISTERED_IMPL_OBJ_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type) \
		template<typename _Ty> \
		class TNDCRegisteredObj<specified_type> : public TNDCRegisteredObj<mapped_type> { \
		public: \
			typedef TNDCRegisteredObj<mapped_type> base_class; \
			MSE_USING(TNDCRegisteredObj, base_class); \
			MSE_NDCREGISTERED_IMPL_OBJ_SPECIALIZATION_DEFINITIONS1(TNDCRegisteredObj); \
		private: \
			MSE_NDCREGISTERED_IMPL_OBJ_NATIVE_POINTER_PRIVATE_CONSTRUCTORS1(TNDCRegisteredObj); \
		};

#define MSE_NDCREGISTERED_IMPL_PTR_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type) \
		template<typename _Ty> \
		class TNDCRegisteredPointer<specified_type> : public TNDCRegisteredPointer<mapped_type> { \
		public: \
			typedef TNDCRegisteredPointer<mapped_type> base_class; \
			MSE_USING(TNDCRegisteredPointer, base_class); \
		}; \
		template<typename _Ty> \
		class TNDCRegisteredConstPointer<specified_type> : public TNDCRegisteredConstPointer<mapped_type> { \
		public: \
			typedef TNDCRegisteredConstPointer<mapped_type> base_class; \
			MSE_USING(TNDCRegisteredConstPointer, base_class); \
		}; \
		template<typename _Ty> \
		class TNDCRegisteredNotNullPointer<specified_type> : public TNDCRegisteredNotNullPointer<mapped_type> { \
		public: \
			typedef TNDCRegisteredNotNullPointer<mapped_type> base_class; \
			MSE_USING(TNDCRegisteredNotNullPointer, base_class); \
		}; \
		template<typename _Ty> \
		class TNDCRegisteredNotNullConstPointer<specified_type> : public TNDCRegisteredNotNullConstPointer<mapped_type> { \
		public: \
			typedef TNDCRegisteredNotNullConstPointer<mapped_type> base_class; \
			MSE_USING(TNDCRegisteredNotNullConstPointer, base_class); \
		}; \
		template<typename _Ty> \
		class TNDCRegisteredFixedPointer<specified_type> : public TNDCRegisteredFixedPointer<mapped_type> { \
		public: \
			typedef TNDCRegisteredFixedPointer<mapped_type> base_class; \
			MSE_USING(TNDCRegisteredFixedPointer, base_class); \
		}; \
		template<typename _Ty> \
		class TNDCRegisteredFixedConstPointer<specified_type> : public TNDCRegisteredFixedConstPointer<mapped_type> { \
		public: \
			typedef TNDCRegisteredFixedConstPointer<mapped_type> base_class; \
			MSE_USING(TNDCRegisteredFixedConstPointer, base_class); \
		};

#define MSE_NDCREGISTERED_IMPL_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type) \
		MSE_NDCREGISTERED_IMPL_PTR_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type); \
		MSE_NDCREGISTERED_IMPL_OBJ_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type);

	MSE_NDCREGISTERED_IMPL_NATIVE_POINTER_SPECIALIZATION(_Ty*, mse::us::impl::TPointerForLegacy<_Ty>);
	MSE_NDCREGISTERED_IMPL_NATIVE_POINTER_SPECIALIZATION(_Ty* const, const mse::us::impl::TPointerForLegacy<_Ty>);

#ifdef MSEPRIMITIVES_H

#define MSE_NDCREGISTERED_IMPL_OBJ_INTEGRAL_SPECIALIZATION(integral_type) \
		template<> \
		class TNDCRegisteredObj<integral_type> : public TNDCRegisteredObj<mse::TInt<integral_type>> { \
		public: \
			typedef TNDCRegisteredObj<mse::TInt<integral_type>> base_class; \
			MSE_USING(TNDCRegisteredObj, base_class); \
		};

#define MSE_NDCREGISTERED_IMPL_PTR_INTEGRAL_SPECIALIZATION(integral_type) \
		template<> \
		class TNDCRegisteredPointer<integral_type> : public TNDCRegisteredPointer<mse::TInt<integral_type>> { \
		public: \
			typedef TNDCRegisteredPointer<mse::TInt<integral_type>> base_class; \
			MSE_USING(TNDCRegisteredPointer, base_class); \
		}; \
		template<> \
		class TNDCRegisteredConstPointer<integral_type> : public TNDCRegisteredConstPointer<mse::TInt<integral_type>> { \
		public: \
			typedef TNDCRegisteredConstPointer<mse::TInt<integral_type>> base_class; \
			MSE_USING(TNDCRegisteredConstPointer, base_class); \
		}; \
		template<> \
		class TNDCRegisteredNotNullPointer<integral_type> : public TNDCRegisteredNotNullPointer<mse::TInt<integral_type>> { \
		public: \
			typedef TNDCRegisteredNotNullPointer<mse::TInt<integral_type>> base_class; \
			MSE_USING(TNDCRegisteredNotNullPointer, base_class); \
		}; \
		template<> \
		class TNDCRegisteredNotNullConstPointer<integral_type> : public TNDCRegisteredNotNullConstPointer<mse::TInt<integral_type>> { \
		public: \
			typedef TNDCRegisteredNotNullConstPointer<mse::TInt<integral_type>> base_class; \
			MSE_USING(TNDCRegisteredNotNullConstPointer, base_class); \
		}; \
		template<> \
		class TNDCRegisteredFixedPointer<integral_type> : public TNDCRegisteredFixedPointer<mse::TInt<integral_type>> { \
		public: \
			typedef TNDCRegisteredFixedPointer<mse::TInt<integral_type>> base_class; \
			MSE_USING(TNDCRegisteredFixedPointer, base_class); \
		}; \
		template<> \
		class TNDCRegisteredFixedConstPointer<integral_type> : public TNDCRegisteredFixedConstPointer<mse::TInt<integral_type>> { \
		public: \
			typedef TNDCRegisteredFixedConstPointer<mse::TInt<integral_type>> base_class; \
			MSE_USING(TNDCRegisteredFixedConstPointer, base_class); \
		};

#define MSE_NDCREGISTERED_IMPL_INTEGRAL_SPECIALIZATION(integral_type) \
		MSE_NDCREGISTERED_IMPL_PTR_INTEGRAL_SPECIALIZATION(integral_type); \
		MSE_NDCREGISTERED_IMPL_OBJ_INTEGRAL_SPECIALIZATION(integral_type); \
		MSE_NDCREGISTERED_IMPL_PTR_INTEGRAL_SPECIALIZATION(typename std::add_const<integral_type>::type); \
		MSE_NDCREGISTERED_IMPL_OBJ_INTEGRAL_SPECIALIZATION(typename std::add_const<integral_type>::type);

	MSE_IMPL_APPLY_MACRO_FUNCTION_TO_EACH_OF_THE_INTEGER_TYPES(MSE_NDCREGISTERED_IMPL_INTEGRAL_SPECIALIZATION)

#endif /*MSEPRIMITIVES_H*/

	/* end of template specializations */

#if defined(MSE_REGISTEREDPOINTER_DISABLED)
	/* Omit definition of make_pointer_to_member() as it would clash with the one already defined in mseregistered.h. */
#define MSE_CREGISTERED_OMIT_MAKE_POINTER_TO_MEMBER
#endif // defined(MSE_REGISTEREDPOINTER_DISABLED)
#if !defined(MSE_CREGISTERED_OMIT_MAKE_POINTER_TO_MEMBER) && defined(MSEREGISTERED_H_)
	template<class _TTargetType, class _Ty>
	TSyncWeakFixedPointer<_TTargetType, TCRegisteredPointer<_Ty>> make_pointer_to_member(_TTargetType& target, const TCRegisteredPointer<_Ty> &lease_pointer) {
		return TSyncWeakFixedPointer<_TTargetType, TCRegisteredPointer<_Ty>>::make(target, lease_pointer);
	}
	template<class _TTargetType, class _Ty>
	TSyncWeakFixedPointer<_TTargetType, TCRegisteredConstPointer<_Ty>> make_pointer_to_member(_TTargetType& target, const TCRegisteredConstPointer<_Ty> &lease_pointer) {
		return TSyncWeakFixedPointer<_TTargetType, TCRegisteredConstPointer<_Ty>>::make(target, lease_pointer);
	}
#endif // !defined(MSE_CREGISTERED_OMIT_MAKE_POINTER_TO_MEMBER) && defined(MSEREGISTERED_H_)

	/* shorter aliases */
	template<typename _Ty> using crp = TCRegisteredPointer<_Ty>;
	template<typename _Ty> using crcp = TCRegisteredConstPointer<_Ty>;
	template<typename _Ty> using crnnp = TCRegisteredNotNullPointer<_Ty>;
	template<typename _Ty> using crnncp = TCRegisteredNotNullConstPointer<_Ty>;
	template<typename _Ty> using crfp = TCRegisteredFixedPointer<_Ty>;
	template<typename _Ty> using crfcp = TCRegisteredFixedConstPointer<_Ty>;
	template<typename _TROFLy> using cro = TCRegisteredObj<_TROFLy>;
	template <class _Ty, class... Args>
	TCRegisteredPointer<_Ty> crnew(Args&&... args) { return cregistered_new<_Ty>(std::forward<Args>(args)...); }
	template <class _Ty>
	void crdelete(const TCRegisteredPointer<_Ty>& regPtrRef) { cregistered_delete<_Ty>(regPtrRef); }

	/* deprecated aliases */
	template<typename _Ty> using rrp MSE_DEPRECATED = TCRegisteredPointer<_Ty>;
	template<typename _Ty> using rrcp MSE_DEPRECATED = TCRegisteredConstPointer<_Ty>;
	template<typename _Ty> using rrnnp MSE_DEPRECATED = TCRegisteredNotNullPointer<_Ty>;
	template<typename _Ty> using rrnncp MSE_DEPRECATED = TCRegisteredNotNullConstPointer<_Ty>;
	template<typename _Ty> using rrfp MSE_DEPRECATED = TCRegisteredFixedPointer<_Ty>;
	template<typename _Ty> using rrfcp MSE_DEPRECATED = TCRegisteredFixedConstPointer<_Ty>;
	template<typename _TROFLy> using rro MSE_DEPRECATED = TCRegisteredObj<_TROFLy>;
	template <class _Ty, class... Args>
	MSE_DEPRECATED TCRegisteredPointer<_Ty> rrnew(Args&&... args) { return cregistered_new<_Ty>(std::forward<Args>(args)...); }
	template <class _Ty>
	MSE_DEPRECATED void rrdelete(const TCRegisteredPointer<_Ty>& regPtrRef) { cregistered_delete<_Ty>(regPtrRef); }
	template<typename _Ty> using TWCRegisteredPointer MSE_DEPRECATED = TNDCRegisteredPointer<_Ty>;
	template<typename _Ty> using TWCRegisteredConstPointer MSE_DEPRECATED = TNDCRegisteredConstPointer<_Ty>;
	template<typename _Ty> using TWCRegisteredNotNullPointer MSE_DEPRECATED = TNDCRegisteredNotNullPointer<_Ty>;
	template<typename _Ty> using TWCRegisteredNotNullConstPointer MSE_DEPRECATED = TNDCRegisteredNotNullConstPointer<_Ty>;
	template<typename _Ty> using TWCRegisteredFixedPointer MSE_DEPRECATED = TNDCRegisteredFixedPointer<_Ty>;
	template<typename _Ty> using TWCRegisteredFixedConstPointer MSE_DEPRECATED = TNDCRegisteredFixedConstPointer<_Ty>;
	template<typename _TROFLy> using TWCRegisteredObj MSE_DEPRECATED = TNDCRegisteredObj<_TROFLy>;

#ifdef _MSC_VER
#pragma warning( push )  
#pragma warning( disable : 4189 )
#endif /*_MSC_VER*/

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
		class CCRegPtrTest1 {
		public:
			static void s_test1() {
#ifdef MSE_SELF_TESTS
				class C;

				class D {
				public:
					virtual ~D() {}
					mse::TCRegisteredPointer<C> m_c_ptr = nullptr;
				};

				class C {
				public:
					C() {}
					mse::TCRegisteredPointer<D> m_d_ptr = nullptr;
				};

				mse::TCRegisteredObj<C> regobjfl_c;
				mse::TCRegisteredPointer<D> d_ptr = mse::cregistered_new<D>();

				regobjfl_c.m_d_ptr = d_ptr;
				d_ptr->m_c_ptr = &regobjfl_c;

				mse::TCRegisteredConstPointer<C> rrcp = d_ptr->m_c_ptr;
				mse::TCRegisteredConstPointer<C> rrcp2 = rrcp;
				const mse::TCRegisteredObj<C> regobjfl_e;
				rrcp = &regobjfl_e;
				mse::TCRegisteredFixedConstPointer<C> rrfcp = &regobjfl_e;
				rrcp = mse::cregistered_new<C>();
				mse::cregistered_delete<C>(rrcp);

				mse::cregistered_delete<D>(d_ptr);

				{
					/* Polymorphic conversions. */
					class FD : public mse::TCRegisteredObj<D> {};
					mse::TCRegisteredObj<FD> cregistered_fd;
					mse::TCRegisteredPointer<FD> FD_cregistered_ptr1 = &cregistered_fd;
					mse::TCRegisteredPointer<D> D_cregistered_ptr4 = FD_cregistered_ptr1;
					D_cregistered_ptr4 = &cregistered_fd;
					mse::TCRegisteredFixedPointer<D> D_cregistered_fptr1 = &cregistered_fd;
					mse::TCRegisteredFixedConstPointer<D> D_cregistered_fcptr1 = &cregistered_fd;
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

#ifdef _MSC_VER
#pragma warning( pop )  
#endif /*_MSC_VER*/

}

#ifndef MSE_PUSH_MACRO_NOT_SUPPORTED
#pragma pop_macro("MSE_THROW")
#pragma pop_macro("_NOEXCEPT")
#endif // !MSE_PUSH_MACRO_NOT_SUPPORTED

#endif // MSECREGISTERED_H_
