
// Copyright (c) 2015 Noah Lopez
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/* TNoradPointers are basically just like TRegisteredPointers except that unlike TRegisteredPointers, they
are able to reference target object types before those target types are (fully) defined. This is required to support
"mutually referencing" objects or "cyclic references".

The implementation difference is that with TRegisteredPointers, the "pointer tracking registry" is located in the target
object, whereas TNoradPointers use (thread local) global registries that track all the pointers targeting
objects of a given type. */

#pragma once
#ifndef MSENORAD_H_
#define MSENORAD_H_

#include "msepointerbasics.h"
#include "mseprimitives.h"
#include <utility>
#include <unordered_map>
#include <mutex>
#include <cassert>
//include <typeinfo>      // std::bad_cast
#include <stdexcept>

#if defined(MSE_SAFER_SUBSTITUTES_DISABLED) || defined(MSE_SAFERPTR_DISABLED)
#define MSE_NORADPOINTER_DISABLED
#endif /*defined(MSE_SAFER_SUBSTITUTES_DISABLED) || defined(MSE_SAFERPTR_DISABLED)*/

#ifndef MSE_PUSH_MACRO_NOT_SUPPORTED
#pragma push_macro("MSE_THROW")
#pragma push_macro("_NOEXCEPT")
#endif // !MSE_PUSH_MACRO_NOT_SUPPORTED

#ifdef MSE_CUSTOM_THROW_DEFINITION
#include <iostream>
#include <stdlib.h> // we include this after including iostream as a workaround for an apparent bug in libtooling8
#define MSE_THROW(x) MSE_CUSTOM_THROW_DEFINITION(x)
#else // MSE_CUSTOM_THROW_DEFINITION
#define MSE_THROW(x) throw(x)
#endif // MSE_CUSTOM_THROW_DEFINITION

#ifndef _NOEXCEPT
#define _NOEXCEPT
#endif /*_NOEXCEPT*/

namespace mse {
	namespace us {
		namespace impl {
			template<typename _Ty, typename _TRefCounter = int> class TGNoradObj;
			template<typename _Ty, typename _TRefCounter = int> class TGNoradPointer;
			template<typename _Ty, typename _TRefCounter = int> class TGNoradConstPointer;
			template<typename _Ty, typename _TRefCounter = int> class TGNoradNotNullPointer;
			template<typename _Ty, typename _TRefCounter = int> class TGNoradNotNullConstPointer;
			template<typename _Ty, typename _TRefCounter = int> class TGNoradFixedPointer;
			template<typename _Ty, typename _TRefCounter = int> class TGNoradFixedConstPointer;
		}
	}
	
#ifndef MSE_DO_NOT_DEFINE_NDNORAD_AS_ALIAS

	template<typename _Ty> using TNDNoradObj = us::impl::TGNoradObj<_Ty>;
	template<typename _Ty> using TNDNoradPointer = us::impl::TGNoradPointer<_Ty>;
	template<typename _Ty> using TNDNoradConstPointer = us::impl::TGNoradConstPointer<_Ty>;
	template<typename _Ty> using TNDNoradNotNullPointer = us::impl::TGNoradNotNullPointer<_Ty>;
	template<typename _Ty> using TNDNoradNotNullConstPointer = us::impl::TGNoradNotNullConstPointer<_Ty>;
	template<typename _Ty> using TNDNoradFixedPointer = us::impl::TGNoradFixedPointer<_Ty>;
	template<typename _Ty> using TNDNoradFixedConstPointer = us::impl::TGNoradFixedConstPointer<_Ty>;

#else // !MSE_DO_NOT_DEFINE_NDNORAD_AS_ALIAS

	template<typename _Ty> class TNDNoradObj;
	template<typename _Ty> class TNDNoradPointer;
	template<typename _Ty> class TNDNoradConstPointer;
	template<typename _Ty> class TNDNoradNotNullPointer;
	template<typename _Ty> class TNDNoradNotNullConstPointer;
	template<typename _Ty> class TNDNoradFixedPointer;
	template<typename _Ty> class TNDNoradFixedConstPointer;

#ifdef MSE_HAS_CXX17
	/* deduction guide */
	template<class _TROy> TNDNoradObj(_TROy)->TNDNoradObj<_TROy>;
#endif /* MSE_HAS_CXX17 */

#endif // !MSE_DO_NOT_DEFINE_NDNORAD_AS_ALIAS

	template<typename _Ty>
	auto ndnorad_fptr_to(_Ty&& _X) {
		return _X.mse_norad_fptr();
	}
	template<typename _Ty>
	auto ndnorad_fptr_to(const _Ty& _X) {
		return _X.mse_norad_fptr();
	}

	template <class _Ty, class... Args> TNDNoradPointer<_Ty> ndnorad_new(Args&&... args);
	template <class _Ty> void ndnorad_delete(TNDNoradPointer<_Ty>& ndnoradPtrRef);
	template <class _Ty> void ndnorad_delete(TNDNoradConstPointer<_Ty>& ndnoradPtrRef);
	namespace us {
		template <class _Ty> void ndnorad_delete(TNDNoradPointer<_Ty>& ndnoradPtrRef);
		template <class _Ty> void ndnorad_delete(TNDNoradConstPointer<_Ty>& ndnoradPtrRef);
	}

	namespace impl {
		template<typename _Ty, class... Args>
		auto make_ndnorad_helper(std::true_type, Args&&... args) {
			return _Ty(std::forward<Args>(args)...);
		}
		template<typename _Ty, class... Args>
		auto make_ndnorad_helper(std::false_type, Args&&... args) {
			return TNDNoradObj<_Ty>(std::forward<Args>(args)...);
		}
	}
	template <class X, class... Args>
	auto make_ndnorad(Args&&... args) {
		typedef mse::impl::remove_reference_t<X> nrX;
		return impl::make_ndnorad_helper<nrX>(typename mse::impl::is_instantiation_of<nrX, TNDNoradObj>::type(), std::forward<Args>(args)...);
	}
	template <class X>
	auto make_ndnorad(const X& arg) {
		typedef mse::impl::remove_reference_t<X> nrX;
		return impl::make_ndnorad_helper<nrX>(typename mse::impl::is_instantiation_of<nrX, TNDNoradObj>::type(), arg);
	}
	template <class X>
	auto make_ndnorad(X&& arg) {
		typedef mse::impl::remove_reference_t<X> nrX;
		return impl::make_ndnorad_helper<nrX>(typename mse::impl::is_instantiation_of<nrX, TNDNoradObj>::type(), MSE_FWD(arg));
	}

#ifdef MSE_NORADPOINTER_DISABLED
	template<typename _Ty> using TNoradPointer = _Ty * ;
	template<typename _Ty> using TNoradConstPointer = const _Ty*;
	template<typename _Ty> using TNoradNotNullPointer = _Ty * ;
	template<typename _Ty> using TNoradNotNullConstPointer = const _Ty*;
	template<typename _Ty> using TNoradFixedPointer = _Ty * /*const*/; /* Can't be const qualified because standard
																			 library containers don't support const elements. */
	template<typename _Ty> using TNoradFixedConstPointer = const _Ty* /*const*/;
	template<typename _TROFLy> using TNoradObj = _TROFLy;

	template<typename _Ty> auto norad_fptr_to(_Ty&& _X) { return &_X; }
	template<typename _Ty> auto norad_fptr_to(const _Ty& _X) { return &_X; }

	template <class _Ty, class... Args>
	TNoradPointer<_Ty> norad_new(Args&&... args) {
		return new TNoradObj<_Ty>(std::forward<Args>(args)...);
	}
	template <class _Ty>
	void norad_delete(TNoradPointer<_Ty>& regPtrRef) {
		auto a = static_cast<TNoradObj<_Ty>*>(regPtrRef);
		regPtrRef = nullptr;
		delete a;
	}
	template <class _Ty>
	void norad_delete(TNoradConstPointer<_Ty>& regPtrRef) {
		auto a = static_cast<const TNoradObj<_Ty>*>(regPtrRef);
		regPtrRef = nullptr;
		delete a;
	}
	namespace us {
		template <class _Ty>
		void norad_delete(TNoradPointer<_Ty>& regPtrRef) {
			mse::norad_delete(regPtrRef);
		}
		template <class _Ty>
		void norad_delete(TNoradConstPointer<_Ty>& regPtrRef) {
			mse::norad_delete(regPtrRef);
		}
	}

	template <class X, class... Args>
	auto make_norad(Args&&... args) {
		typedef mse::impl::remove_reference_t<X> nrX;
		return nrX(std::forward<Args>(args)...);
	}
	template <class X>
	auto make_norad(const X& arg) {
		typedef mse::impl::remove_reference_t<X> nrX;
		return nrX(arg);
	}
	template <class X>
	auto make_norad(X&& arg) {
		typedef mse::impl::remove_reference_t<X> nrX;
		return nrX(MSE_FWD(arg));
	}

#else /*MSE_NORADPOINTER_DISABLED*/

	class norad_cannot_verify_cast_error : public std::logic_error {
	public:
		using std::logic_error::logic_error;
	};

	template<typename _Ty> using TNoradPointer = TNDNoradPointer<_Ty>;
	template<typename _Ty> using TNoradConstPointer = TNDNoradConstPointer<_Ty>;
	template<typename _Ty> using TNoradNotNullPointer = TNDNoradNotNullPointer<_Ty>;
	template<typename _Ty> using TNoradNotNullConstPointer = TNDNoradNotNullConstPointer<_Ty>;
	template<typename _Ty> using TNoradFixedPointer = TNDNoradFixedPointer<_Ty>;
	template<typename _Ty> using TNoradFixedConstPointer = TNDNoradFixedConstPointer<_Ty>;
	template<typename _TROFLy> using TNoradObj = TNDNoradObj<_TROFLy>;

	template<typename _Ty> auto norad_fptr_to(_Ty&& _X) { return ndnorad_fptr_to(MSE_FWD(_X)); }
	template<typename _Ty> auto norad_fptr_to(const _Ty& _X) { return ndnorad_fptr_to(_X); }

	template <class _Ty, class... Args> TNDNoradPointer<_Ty> norad_new(Args&&... args) { return ndnorad_new<_Ty>(std::forward<Args>(args)...); }
	template <class _Ty> void norad_delete(TNDNoradPointer<_Ty>& ndnoradPtrRef) { return mse::ndnorad_delete<_Ty>(ndnoradPtrRef); }
	template <class _Ty> void norad_delete(TNDNoradConstPointer<_Ty>& ndnoradPtrRef) { return mse::ndnorad_delete<_Ty>(ndnoradPtrRef); }
	namespace us {
		template <class _Ty> void norad_delete(TNDNoradPointer<_Ty>& ndnoradPtrRef) { return mse::us::ndnorad_delete<_Ty>(ndnoradPtrRef); }
		template <class _Ty> void norad_delete(TNDNoradConstPointer<_Ty>& ndnoradPtrRef) { return mse::us::ndnorad_delete<_Ty>(ndnoradPtrRef); }
	}

	template <class X, class... Args>
	auto make_norad(Args&&... args) {
		return make_ndnorad<X>(std::forward<Args>(args)...);
	}
	template <class X>
	auto make_norad(const X& arg) {
		return make_ndnorad(arg);
	}
	template <class X>
	auto make_norad(X&& arg) {
		return make_ndnorad(MSE_FWD(arg));
	}

#endif /*MSE_NORADPOINTER_DISABLED*/

	namespace us {
		namespace impl {

			/* TGNoradPointer<>, like TNDCRegisteredPointer<>, behaves similar to native pointers. But where registered pointers are
			automatically set to nullptr when their target is destroyed, the destruction of an object while a "norad" pointer is targeting
			it results in program termination. This drastic consequence allows norad pointers' run-time safety mechanism to be very
			lightweight (compared to that of registered pointers). */
			template<typename _Ty, typename _TRefCounter>
			class TGNoradPointer : public mse::us::impl::StrongPointerTagBase
				, public std::conditional<(!std::is_convertible<_Ty*, mse::us::impl::AsyncNotShareableTagBase*>::value)
					&& (std::is_arithmetic/*as opposed to say, atomic*/<_TRefCounter>::value)
					, mse::us::impl::AsyncNotShareableTagBase, mse::impl::TPlaceHolder<mse::us::impl::AsyncNotShareableTagBase, TGNoradPointer< _Ty, _TRefCounter> > >::type
				, public std::conditional<(!std::is_convertible<_Ty*, mse::us::impl::AsyncNotPassableTagBase*>::value)
					&& (std::is_arithmetic/*as opposed to say, atomic*/<_TRefCounter>::value)
					, mse::us::impl::AsyncNotPassableTagBase, mse::impl::TPlaceHolder<mse::us::impl::AsyncNotPassableTagBase, TGNoradPointer< _Ty, _TRefCounter> > >::type
			{
			public:
				TGNoradPointer() : m_ptr() {}
				TGNoradPointer(const TGNoradPointer& src_cref) : m_ptr(src_cref.m_ptr) {
					if (m_ptr) { (*m_ptr).increment_refcount(); }
				}
				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
				TGNoradPointer(const TGNoradPointer<_Ty2, _TRefCounter>& src_cref) : m_ptr(src_cref.m_ptr) {
					if (m_ptr) { (*m_ptr).increment_refcount(); }
				}
				TGNoradPointer(TGNoradPointer&& src_ref) : m_ptr(MSE_FWD(src_ref).m_ptr) {
					src_ref.m_ptr = nullptr;
				}
				TGNoradPointer(std::nullptr_t) : m_ptr(nullptr) {}
				MSE_IMPL_DESTRUCTOR_PREFIX1 ~TGNoradPointer() {
					if (m_ptr) { (*m_ptr).decrement_refcount(); }
				}
				TGNoradPointer<_Ty, _TRefCounter>& operator=(const TGNoradPointer<_Ty, _TRefCounter>& _Right_cref) {
					if (m_ptr) { (*m_ptr).decrement_refcount(); }
					m_ptr = _Right_cref.m_ptr;
					if (m_ptr) { (*m_ptr).increment_refcount(); }
					return (*this);
				}
				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
				TGNoradPointer<_Ty, _TRefCounter>& operator=(const TGNoradPointer<_Ty2, _TRefCounter>& _Right_cref) {
					return (*this).operator=(TGNoradPointer(_Right_cref));
				}

				TGNoradObj<_Ty, _TRefCounter>& operator*() const {
					if (!m_ptr) { MSE_THROW(primitives_null_dereference_error("attempt to dereference null pointer - mse::TNoradPointer")); }
					return *m_ptr;
				}
				TGNoradObj<_Ty, _TRefCounter>* operator->() const {
					if (!m_ptr) { MSE_THROW(primitives_null_dereference_error("attempt to dereference null pointer - mse::TNoradPointer")); }
					return (TGNoradObj<_Ty, _TRefCounter>*)(m_ptr);
				}

				operator bool() const { return !(!m_ptr); }
				/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
				MSE_DEPRECATED explicit operator _Ty*() {
#ifdef NATIVE_PTR_DEBUG_HELPER1
					if (nullptr == m_ptr) {
						int q = 5; /* just a line of code for putting a debugger break point */
					}
#endif /*NATIVE_PTR_DEBUG_HELPER1*/
					return std::addressof(*m_ptr);
				}
				MSE_DEPRECATED explicit operator const _Ty*() const {
#ifdef NATIVE_PTR_DEBUG_HELPER1
					if (nullptr == m_ptr) {
						int q = 5; /* just a line of code for putting a debugger break point */
					}
#endif /*NATIVE_PTR_DEBUG_HELPER1*/
					return std::addressof(*m_ptr);
				}

				/* In C++, if an object is deleted via a pointer to its base class and the base class' destructor is not virtual,
				then the (derived) object's destructor won't be called possibly resulting in resource leaks. With registered
				objects, the destructor not being called also circumvents their memory safety mechanism. */
				void norad_delete() {
					auto a = asANativePointerToTGNoradObj();
					(*this) = nullptr;
					delete a;
				}

			private:
				TGNoradPointer(TGNoradObj<_Ty, _TRefCounter>* ptr) : m_ptr(ptr) {
					assert(m_ptr);
					(*m_ptr).increment_refcount();
				}

				/* This function, if possible, should not be used. It is meant to be used exclusively by norad_delete<>(). */
				TGNoradObj<_Ty, _TRefCounter>* asANativePointerToTGNoradObj() {
#ifdef NATIVE_PTR_DEBUG_HELPER1
					if (nullptr == m_ptr) {
						int q = 5; /* just a line of code for putting a debugger break point */
					}
#endif /*NATIVE_PTR_DEBUG_HELPER1*/
					return static_cast<TGNoradObj<_Ty, _TRefCounter>*>(m_ptr);
				}
				const TGNoradObj<_Ty, _TRefCounter>* asANativePointerToTGNoradObj() const {
#ifdef NATIVE_PTR_DEBUG_HELPER1
					if (nullptr == m_ptr) {
						int q = 5; /* just a line of code for putting a debugger break point */
					}
#endif /*NATIVE_PTR_DEBUG_HELPER1*/
					return static_cast<const TGNoradObj<_Ty, _TRefCounter>*>(m_ptr);
				}

				MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

				mse::us::impl::TPointer<TGNoradObj<_Ty, _TRefCounter> > m_ptr;

				template <class Y, typename _TRefCounter2> friend class TGNoradPointer;
				template <class Y, typename _TRefCounter2> friend class TGNoradConstPointer;
				friend class TGNoradNotNullPointer<_Ty, _TRefCounter>;
				template <class _Ty2> friend void mse::ndnorad_delete(TNDNoradPointer<_Ty2>& ndnoradPtrRef);
			};

			template<typename _Ty, typename _TRefCounter>
			class TGNoradConstPointer : public mse::us::impl::StrongPointerTagBase
				, public std::conditional<(!std::is_convertible<_Ty*, mse::us::impl::AsyncNotShareableTagBase*>::value)
					&& (std::is_arithmetic/*as opposed to say, atomic*/<_TRefCounter>::value)
					, mse::us::impl::AsyncNotShareableTagBase, mse::impl::TPlaceHolder<mse::us::impl::AsyncNotShareableTagBase, TGNoradConstPointer<_Ty, _TRefCounter> > >::type
				, public std::conditional<(!std::is_convertible<_Ty*, mse::us::impl::AsyncNotPassableTagBase*>::value)
					&& (std::is_arithmetic/*as opposed to say, atomic*/<_TRefCounter>::value)
					, mse::us::impl::AsyncNotPassableTagBase, mse::impl::TPlaceHolder<mse::us::impl::AsyncNotPassableTagBase, TGNoradConstPointer<_Ty, _TRefCounter> > >::type
			{
			public:
				TGNoradConstPointer() : m_ptr() {}
				TGNoradConstPointer(const TGNoradConstPointer& src_cref) : m_ptr(src_cref.m_ptr) {
					if (m_ptr) { (*m_ptr).increment_refcount(); }
				}
				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
				TGNoradConstPointer(const TGNoradConstPointer<_Ty2, _TRefCounter>& src_cref) : m_ptr(src_cref.m_ptr) {
					if (m_ptr) { (*m_ptr).increment_refcount(); }
				}
				TGNoradConstPointer(const TGNoradPointer<_Ty, _TRefCounter>& src_cref) : m_ptr(src_cref.m_ptr) {
					if (m_ptr) { (*m_ptr).increment_refcount(); }
				}
				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
				TGNoradConstPointer(const TGNoradPointer<_Ty2, _TRefCounter>& src_cref) : m_ptr(src_cref.m_ptr) {
					if (m_ptr) { (*m_ptr).increment_refcount(); }
				}

				TGNoradConstPointer(TGNoradConstPointer&& src_ref) : m_ptr(MSE_FWD(src_ref).m_ptr) {
					src_ref.m_ptr = nullptr;
				}
				TGNoradConstPointer(TGNoradPointer<_Ty, _TRefCounter>&& src_ref) : m_ptr(MSE_FWD(src_ref).m_ptr) {
					src_ref.m_ptr = nullptr;
				}

				TGNoradConstPointer(std::nullptr_t) : m_ptr(nullptr) {}
				MSE_IMPL_DESTRUCTOR_PREFIX1 ~TGNoradConstPointer() {
					if (m_ptr) { (*m_ptr).decrement_refcount(); }
				}
				TGNoradConstPointer<_Ty, _TRefCounter>& operator=(const TGNoradConstPointer<_Ty, _TRefCounter>& _Right_cref) {
					if (m_ptr) { (*m_ptr).decrement_refcount(); }
					m_ptr = _Right_cref.m_ptr;
					if (m_ptr) { (*m_ptr).increment_refcount(); }
					return (*this);
				}
				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
				TGNoradConstPointer<_Ty, _TRefCounter>& operator=(const TGNoradConstPointer<_Ty2, _TRefCounter>& _Right_cref) {
					return (*this).operator=(TGNoradConstPointer(_Right_cref));
				}

				const TGNoradObj<_Ty, _TRefCounter>& operator*() const {
					if (!m_ptr) { MSE_THROW(primitives_null_dereference_error("attempt to dereference null pointer - mse::TNoradPointer")); }
					return *m_ptr;
				}
				const TGNoradObj<_Ty, _TRefCounter>* operator->() const {
					if (!m_ptr) { MSE_THROW(primitives_null_dereference_error("attempt to dereference null pointer - mse::TNoradPointer")); }
					return (const TGNoradObj<_Ty, _TRefCounter>*)(m_ptr);
				}

				operator bool() const { return !(!m_ptr); }
				/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
				MSE_DEPRECATED explicit operator const _Ty*() const {
#ifdef NATIVE_PTR_DEBUG_HELPER1
					if (nullptr == m_ptr) {
						int q = 5; /* just a line of code for putting a debugger break point */
					}
#endif /*NATIVE_PTR_DEBUG_HELPER1*/
					return m_ptr;
				}

				/* In C++, if an object is deleted via a pointer to its base class and the base class' destructor is not virtual,
				then the (derived) object's destructor won't be called possibly resulting in resource leaks. With registered
				objects, the destructor not being called also circumvents their memory safety mechanism. */
				void norad_delete() {
					auto a = asANativePointerToTGNoradObj();
					(*this) = nullptr;
					delete a;
				}

			private:
				TGNoradConstPointer(const TGNoradObj<_Ty, _TRefCounter>* ptr) : m_ptr(ptr) {
					assert(m_ptr);
					(*m_ptr).increment_refcount();
				}

				/* This function, if possible, should not be used. It is meant to be used exclusively by norad_delete<>(). */
				const TGNoradObj<_Ty, _TRefCounter>* asANativePointerToTGNoradObj() const {
#ifdef NATIVE_PTR_DEBUG_HELPER1
					if (nullptr == m_ptr) {
						int q = 5; /* just a line of code for putting a debugger break point */
					}
#endif /*NATIVE_PTR_DEBUG_HELPER1*/
					return static_cast<const TGNoradObj<_Ty, _TRefCounter>*>(m_ptr);
				}

				MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

				mse::us::impl::TPointer<const TGNoradObj<_Ty, _TRefCounter> > m_ptr;

				template <class Y, typename _TRefCounter2> friend class TGNoradConstPointer;
				friend class TGNoradNotNullConstPointer<_Ty, _TRefCounter>;
				template <class _Ty2> friend void mse::ndnorad_delete(TNDNoradConstPointer<_Ty2>& ndnoradPtrRef);
			};

			template<typename _Ty, typename _TRefCounter>
			class TGNoradNotNullPointer : public TGNoradPointer<_Ty, _TRefCounter>, public mse::us::impl::NeverNullTagBase {
			public:
				TGNoradNotNullPointer(const TGNoradNotNullPointer& src_cref) : TGNoradPointer<_Ty, _TRefCounter>(src_cref) {}
				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
				TGNoradNotNullPointer(const TGNoradNotNullPointer<_Ty2, _TRefCounter>& src_cref) : TGNoradPointer<_Ty, _TRefCounter>(src_cref) {}
				TGNoradNotNullPointer(TGNoradNotNullPointer&& src_ref) : TGNoradPointer<_Ty, _TRefCounter>(MSE_FWD(src_ref)) {}

				MSE_IMPL_DESTRUCTOR_PREFIX1 ~TGNoradNotNullPointer() {}
				/*
				TGNoradNotNullPointer<_Ty, _TRefCounter>& operator=(const TGNoradNotNullPointer<_Ty, _TRefCounter>& _Right_cref) {
				TGNoradPointer<_Ty, _TRefCounter>::operator=(_Right_cref);
				return (*this);
				}
				*/

				TGNoradObj<_Ty, _TRefCounter>& operator*() const {
					//if (!((*this).m_ptr)) { MSE_THROW(primitives_null_dereference_error("attempt to dereference null pointer - mse::TNoradPointer")); }
					return *((*this).m_ptr);
				}
				TGNoradObj<_Ty, _TRefCounter>* operator->() const {
					//if (!((*this).m_ptr)) { MSE_THROW(primitives_null_dereference_error("attempt to dereference null pointer - mse::TNoradPointer")); }
					return (*this).m_ptr;
				}

				/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
				MSE_DEPRECATED explicit operator _Ty*() const { return TGNoradPointer<_Ty, _TRefCounter>::operator _Ty*(); }
				MSE_DEPRECATED explicit operator TGNoradObj<_Ty, _TRefCounter>*() const { return TGNoradPointer<_Ty, _TRefCounter>::operator TGNoradObj<_Ty, _TRefCounter>*(); }

			private:
				TGNoradNotNullPointer(TGNoradObj<_Ty, _TRefCounter>* ptr) : TGNoradPointer<_Ty, _TRefCounter>(ptr) {}

				/* If you want to use this constructor, use not_null_from_nullable() instead. */
				TGNoradNotNullPointer(const  TGNoradPointer<_Ty, _TRefCounter>& src_cref) : TGNoradPointer<_Ty, _TRefCounter>(src_cref) {
					*src_cref; // to ensure that src_cref points to a valid target
				}
				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
				TGNoradNotNullPointer(const TGNoradPointer<_Ty2, _TRefCounter>& src_cref) : TGNoradPointer<_Ty, _TRefCounter>(src_cref) {
					*src_cref; // to ensure that src_cref points to a valid target
				}

				/* If you want a pointer to a TGNoradNotNullPointer<_Ty, _TRefCounter>, declare the TGNoradNotNullPointer<_Ty, _TRefCounter> as a
				TGNoradObj<TGNoradNotNullPointer<_Ty, _TRefCounter>> instead. So for example:
				auto reg_ptr = TGNoradObj<TGNoradNotNullPointer<_Ty, _TRefCounter>>(mse::registered_new<_Ty, _TRefCounter>());
				auto reg_ptr_to_reg_ptr = &reg_ptr;
				*/
				MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

				friend class TGNoradFixedPointer<_Ty, _TRefCounter>;
				template<typename _Ty2>
				friend TGNoradNotNullPointer<_Ty2, _TRefCounter> not_null_from_nullable(const TGNoradPointer<_Ty2, _TRefCounter>& src);
			};

			template<typename _Ty, typename _TRefCounter>
			class TGNoradNotNullConstPointer : public TGNoradConstPointer<_Ty, _TRefCounter>, public mse::us::impl::NeverNullTagBase {
			public:
				TGNoradNotNullConstPointer(const TGNoradNotNullPointer<_Ty, _TRefCounter>& src_cref) : TGNoradConstPointer<_Ty, _TRefCounter>(src_cref) {}
				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
				TGNoradNotNullConstPointer(const TGNoradNotNullPointer<_Ty2, _TRefCounter>& src_cref) : TGNoradConstPointer<_Ty, _TRefCounter>(src_cref) {}
				TGNoradNotNullConstPointer(const TGNoradNotNullConstPointer<_Ty, _TRefCounter>& src_cref) : TGNoradConstPointer<_Ty, _TRefCounter>(src_cref) {}
				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
				TGNoradNotNullConstPointer(const TGNoradNotNullConstPointer<_Ty2, _TRefCounter>& src_cref) : TGNoradConstPointer<_Ty, _TRefCounter>(src_cref) {}

				TGNoradNotNullConstPointer(TGNoradNotNullPointer<_Ty, _TRefCounter>&& src_ref) : TGNoradConstPointer<_Ty, _TRefCounter>(MSE_FWD(src_ref)) {}
				TGNoradNotNullConstPointer(TGNoradNotNullConstPointer<_Ty, _TRefCounter>&& src_ref) : TGNoradConstPointer<_Ty, _TRefCounter>(MSE_FWD(src_ref)) {}

				MSE_IMPL_DESTRUCTOR_PREFIX1 ~TGNoradNotNullConstPointer() {}

				const TGNoradObj<_Ty, _TRefCounter>& operator*() const {
					//if (!((*this).m_ptr)) { MSE_THROW(primitives_null_dereference_error("attempt to dereference null pointer - mse::TNoradPointer")); }
					return *((*this).m_ptr);
				}
				const TGNoradObj<_Ty, _TRefCounter>* operator->() const {
					//if (!((*this).m_ptr)) { MSE_THROW(primitives_null_dereference_error("attempt to dereference null pointer - mse::TNoradPointer")); }
					return (*this).m_ptr;
				}

				/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
				MSE_DEPRECATED explicit operator const _Ty*() const { return TGNoradConstPointer<_Ty, _TRefCounter>::operator const _Ty*(); }
				MSE_DEPRECATED explicit operator const TGNoradObj<_Ty, _TRefCounter>*() const { return TGNoradConstPointer<_Ty, _TRefCounter>::operator const TGNoradObj<_Ty, _TRefCounter>*(); }

			private:
				TGNoradNotNullConstPointer(const TGNoradObj<_Ty, _TRefCounter>* ptr) : TGNoradConstPointer<_Ty, _TRefCounter>(ptr) {}

				/* If you want to use this constructor, use not_null_from_nullable() instead. */
				TGNoradNotNullConstPointer(const TGNoradPointer<_Ty, _TRefCounter>& src_cref) : TGNoradConstPointer<_Ty, _TRefCounter>(src_cref) {
					*src_cref; // to ensure that src_cref points to a valid target
				}
				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
				TGNoradNotNullConstPointer(const TGNoradPointer<_Ty2, _TRefCounter>& src_cref) : TGNoradConstPointer<_Ty, _TRefCounter>(src_cref) {
					*src_cref; // to ensure that src_cref points to a valid target
				}
				TGNoradNotNullConstPointer(const TGNoradConstPointer<_Ty, _TRefCounter>& src_cref) : TGNoradConstPointer<_Ty, _TRefCounter>(src_cref) {
					*src_cref; // to ensure that src_cref points to a valid target
				}
				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
				TGNoradNotNullConstPointer(const TGNoradConstPointer<_Ty2, _TRefCounter>& src_cref) : TGNoradConstPointer<_Ty, _TRefCounter>(src_cref) {
					*src_cref; // to ensure that src_cref points to a valid target
				}

				MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

				friend class TGNoradFixedConstPointer<_Ty, _TRefCounter>;
				template<typename _Ty2>
				friend TGNoradNotNullConstPointer<_Ty2, _TRefCounter> not_null_from_nullable(const TGNoradConstPointer<_Ty2, _TRefCounter>& src);
			};

			template<typename _Ty, typename _TRefCounter>
			TGNoradNotNullPointer<_Ty, _TRefCounter> not_null_from_nullable(const TGNoradPointer<_Ty, _TRefCounter>& src) {
				return src;
			}
			template<typename _Ty, typename _TRefCounter>
			TGNoradNotNullConstPointer<_Ty, _TRefCounter> not_null_from_nullable(const TGNoradConstPointer<_Ty, _TRefCounter>& src) {
				return src;
			}

			/* TGNoradFixedPointer cannot be retargeted or constructed without a target. This pointer is recommended for passing
			parameters by reference. */
			template<typename _Ty, typename _TRefCounter>
			class TGNoradFixedPointer : public TGNoradNotNullPointer<_Ty, _TRefCounter> {
			public:
				TGNoradFixedPointer(const TGNoradFixedPointer& src_cref) : TGNoradNotNullPointer<_Ty, _TRefCounter>(src_cref) {}
				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
				TGNoradFixedPointer(const TGNoradFixedPointer<_Ty2, _TRefCounter>& src_cref) : TGNoradNotNullPointer<_Ty, _TRefCounter>(src_cref) {}

				TGNoradFixedPointer(const TGNoradNotNullPointer<_Ty, _TRefCounter>& src_cref) : TGNoradNotNullPointer<_Ty, _TRefCounter>(src_cref) {}
				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
				TGNoradFixedPointer(const TGNoradNotNullPointer<_Ty2, _TRefCounter>& src_cref) : TGNoradNotNullPointer<_Ty, _TRefCounter>(src_cref) {}

				TGNoradFixedPointer(TGNoradFixedPointer&& src_ref) : TGNoradNotNullPointer<_Ty, _TRefCounter>(MSE_FWD(src_ref)) {}
				TGNoradFixedPointer(TGNoradNotNullPointer<_Ty, _TRefCounter>&& src_ref) : TGNoradNotNullPointer<_Ty, _TRefCounter>(MSE_FWD(src_ref)) {}

				MSE_IMPL_DESTRUCTOR_PREFIX1 ~TGNoradFixedPointer() {}

				/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
				MSE_DEPRECATED explicit operator _Ty*() const { return TGNoradNotNullPointer<_Ty, _TRefCounter>::operator _Ty*(); }
				MSE_DEPRECATED explicit operator TGNoradObj<_Ty, _TRefCounter>*() const { return TGNoradNotNullPointer<_Ty, _TRefCounter>::operator TGNoradObj<_Ty, _TRefCounter>*(); }

			private:
				TGNoradFixedPointer(TGNoradObj<_Ty, _TRefCounter>* ptr) : TGNoradNotNullPointer<_Ty, _TRefCounter>(ptr) {}
				TGNoradFixedPointer<_Ty, _TRefCounter>& operator=(const TGNoradFixedPointer<_Ty, _TRefCounter>& _Right_cref) = delete;

				/* If you want a pointer to a TGNoradFixedPointer<_Ty, _TRefCounter>, declare the TGNoradFixedPointer<_Ty, _TRefCounter> as a
				TGNoradObj<TGNoradFixedPointer<_Ty, _TRefCounter>> instead. So for example:
				auto reg_ptr = TGNoradObj<TGNoradFixedPointer<_Ty, _TRefCounter>>(mse::registered_new<_Ty, _TRefCounter>());
				auto reg_ptr_to_reg_ptr = &reg_ptr;
				*/
				MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

				friend class TGNoradObj<_Ty, _TRefCounter>;
			};

			template<typename _Ty, typename _TRefCounter>
			class TGNoradFixedConstPointer : public TGNoradNotNullConstPointer<_Ty, _TRefCounter> {
			public:
				TGNoradFixedConstPointer(const TGNoradFixedPointer<_Ty, _TRefCounter>& src_cref) : TGNoradNotNullConstPointer<_Ty, _TRefCounter>(src_cref) {}
				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
				TGNoradFixedConstPointer(const TGNoradFixedPointer<_Ty2, _TRefCounter>& src_cref) : TGNoradNotNullConstPointer<_Ty, _TRefCounter>(src_cref) {}
				TGNoradFixedConstPointer(const TGNoradFixedConstPointer<_Ty, _TRefCounter>& src_cref) : TGNoradNotNullConstPointer<_Ty, _TRefCounter>(src_cref) {}
				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
				TGNoradFixedConstPointer(const TGNoradFixedConstPointer<_Ty2, _TRefCounter>& src_cref) : TGNoradNotNullConstPointer<_Ty, _TRefCounter>(src_cref) {}

				TGNoradFixedConstPointer(const TGNoradNotNullPointer<_Ty, _TRefCounter>& src_cref) : TGNoradNotNullConstPointer<_Ty, _TRefCounter>(src_cref) {}
				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
				TGNoradFixedConstPointer(const TGNoradNotNullPointer<_Ty2, _TRefCounter>& src_cref) : TGNoradNotNullConstPointer<_Ty, _TRefCounter>(src_cref) {}
				TGNoradFixedConstPointer(const TGNoradNotNullConstPointer<_Ty, _TRefCounter>& src_cref) : TGNoradNotNullConstPointer<_Ty, _TRefCounter>(src_cref) {}
				template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
				TGNoradFixedConstPointer(const TGNoradNotNullConstPointer<_Ty2, _TRefCounter>& src_cref) : TGNoradNotNullConstPointer<_Ty, _TRefCounter>(src_cref) {}

				TGNoradFixedConstPointer(TGNoradFixedPointer<_Ty, _TRefCounter>&& src_ref) : TGNoradNotNullConstPointer<_Ty, _TRefCounter>(MSE_FWD(src_ref)) {}
				TGNoradFixedConstPointer(TGNoradFixedConstPointer<_Ty, _TRefCounter>&& src_ref) : TGNoradNotNullConstPointer<_Ty, _TRefCounter>(MSE_FWD(src_ref)) {}

				TGNoradFixedConstPointer(TGNoradNotNullPointer<_Ty, _TRefCounter>&& src_ref) : TGNoradNotNullConstPointer<_Ty, _TRefCounter>(MSE_FWD(src_ref)) {}
				TGNoradFixedConstPointer(TGNoradNotNullConstPointer<_Ty, _TRefCounter>&& src_ref) : TGNoradNotNullConstPointer<_Ty, _TRefCounter>(MSE_FWD(src_ref)) {}

				MSE_IMPL_DESTRUCTOR_PREFIX1 ~TGNoradFixedConstPointer() {}
				/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
				MSE_DEPRECATED explicit operator const _Ty*() const { return TGNoradNotNullConstPointer<_Ty, _TRefCounter>::operator const _Ty*(); }
				MSE_DEPRECATED explicit operator const TGNoradObj<_Ty, _TRefCounter>*() const { return TGNoradNotNullConstPointer<_Ty, _TRefCounter>::operator const TGNoradObj<_Ty, _TRefCounter>*(); }

			private:
				TGNoradFixedConstPointer(const TGNoradObj<_Ty, _TRefCounter>* ptr) : TGNoradNotNullConstPointer<_Ty, _TRefCounter>(ptr) {}
				TGNoradFixedConstPointer<_Ty, _TRefCounter>& operator=(const TGNoradFixedConstPointer<_Ty, _TRefCounter>& _Right_cref) = delete;

				MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

				friend class TGNoradObj<_Ty, _TRefCounter>;
			};

			/* This macro roughly simulates constructor inheritance. */
#define MSE_NORAD_OBJ_USING(Derived, Base) MSE_USING(Derived, Base)

		/* TGNoradObj is intended as a transparent wrapper for other classes/objects. The purpose is to track the number of
		references targeting the object and verify that there are none outstanding when the object is destroyed. Note that
		TGNoradObj can be used with objects allocated on the stack. */
			template<typename _TROFLy, typename _TRefCounter>
			class TGNoradObj : public _TROFLy
				, public std::conditional<(!mse::impl::is_derived_from<_TROFLy, mse::us::impl::AsyncNotShareableTagBase>::value)
					&& (std::is_arithmetic/*as opposed to say, atomic*/<_TRefCounter>::value)
					, mse::us::impl::AsyncNotShareableTagBase, mse::impl::TPlaceHolder<mse::us::impl::AsyncNotShareableTagBase, TGNoradObj<_TROFLy, _TRefCounter> > >::type
				, public std::conditional<(!mse::impl::is_derived_from<_TROFLy, mse::us::impl::AsyncNotPassableTagBase>::value)
					&& (std::is_arithmetic/*as opposed to say, atomic*/<_TRefCounter>::value)
					, mse::us::impl::AsyncNotPassableTagBase, mse::impl::TPlaceHolder<mse::us::impl::AsyncNotPassableTagBase, TGNoradObj<_TROFLy, _TRefCounter> > >::type
			{
			public:
				typedef _TROFLy base_class;

				MSE_NORAD_OBJ_USING(TGNoradObj, _TROFLy);
				TGNoradObj(const TGNoradObj& _X) : _TROFLy(_X) {}
				TGNoradObj(TGNoradObj&& _X) : _TROFLy(MSE_FWD(_X)) {}
				MSE_IMPL_DESTRUCTOR_PREFIX1 ~TGNoradObj() {
					if (0 != m_counter) {
						/* It would be unsafe to allow this object to be destroyed as there are outstanding references to this object. */
#ifdef MSE_CUSTOM_FATAL_ERROR_MESSAGE_HANDLER
						MSE_CUSTOM_FATAL_ERROR_MESSAGE_HANDLER("Fatal Error: mse::TGNoradObj<> destructed with outstanding references \n");
#endif // MSE_CUSTOM_FATAL_ERROR_MESSAGE_HANDLER
						assert(false); std::terminate();
					}
				}

				TGNoradObj& operator=(TGNoradObj&& _X) { _TROFLy::operator=(MSE_FWD(_X)); return (*this); }
				TGNoradObj& operator=(const TGNoradObj& _X) { _TROFLy::operator=(_X); return (*this); }
				template<class _Ty2>
				TGNoradObj& operator=(_Ty2&& _X) { _TROFLy::operator=(MSE_FWD(_X)); return (*this); }
				template<class _Ty2>
				TGNoradObj& operator=(const _Ty2& _X) { _TROFLy::operator=(_X); return (*this); }

				TGNoradNotNullPointer<_TROFLy, _TRefCounter> operator&() {
					return TGNoradFixedPointer<_TROFLy, _TRefCounter>(this);
				}
				TGNoradNotNullConstPointer<_TROFLy, _TRefCounter> operator&() const {
					return TGNoradFixedConstPointer<_TROFLy, _TRefCounter>(this);
				}
				TGNoradNotNullPointer<_TROFLy, _TRefCounter> mse_norad_nnptr() { return TGNoradFixedPointer<_TROFLy, _TRefCounter>(this); }
				TGNoradNotNullConstPointer<_TROFLy, _TRefCounter> mse_norad_nnptr() const { return TGNoradFixedConstPointer<_TROFLy, _TRefCounter>(this); }
				TGNoradFixedPointer<_TROFLy, _TRefCounter> mse_norad_fptr() { return TGNoradFixedPointer<_TROFLy, _TRefCounter>(this); }
				TGNoradFixedConstPointer<_TROFLy, _TRefCounter> mse_norad_fptr() const { return TGNoradFixedConstPointer<_TROFLy, _TRefCounter>(this); }

			private:
				void increment_refcount() const { m_counter += 1; }
				void decrement_refcount() const { m_counter -= 1; }

				mutable _TRefCounter m_counter = 0;

				template<typename _Ty2, typename _TRefCounter2>
				friend class TGNoradPointer;
				template<typename _Ty2, typename _TRefCounter2>
				friend class TGNoradConstPointer;
			};
		}
	}

	/* See ndregistered_new(). */
	template <class _Ty, class... Args>
	TNDNoradPointer<_Ty> ndnorad_new(Args&&... args) {
		auto a = new TNDNoradObj<_Ty>(std::forward<Args>(args)...);
		mse::us::impl::tlSAllocRegistry_ref<TNDNoradObj<_Ty> >().registerPointer(a);
		return &(*a);
	}
	template <class _Ty>
	void ndnorad_delete(TNDNoradPointer<_Ty>& ndnoradPtrRef) {
		//auto a = static_cast<TNDNoradObj<_Ty>*>(ndnoradPtrRef);
		auto a = ndnoradPtrRef.asANativePointerToTGNoradObj();
		auto res = mse::us::impl::tlSAllocRegistry_ref<TNDNoradObj<_Ty> >().unregisterPointer(a);
		if (!res) { assert(false); MSE_THROW(std::invalid_argument("invalid argument, no corresponding allocation found - mse::ndnorad_delete() \n- tip: If deleting via base class pointer, use mse::us::ndnorad_delete() instead. ")); }
		ndnoradPtrRef.norad_delete();
	}
	template <class _Ty>
	void ndnorad_delete(TNDNoradConstPointer<_Ty>& ndnoradPtrRef) {
		auto a = ndnoradPtrRef.asANativePointerToTGNoradObj();
		auto res = mse::us::impl::tlSAllocRegistry_ref<TNDNoradObj<_Ty> >().unregisterPointer(a);
		if (!res) { assert(false); MSE_THROW(std::invalid_argument("invalid argument, no corresponding allocation found - mse::ndnorad_delete() \n- tip: If deleting via base class pointer, use mse::us::ndnorad_delete() instead. ")); }
		ndnoradPtrRef.norad_delete();
	}
	namespace us {
		template <class _Ty>
		void ndnorad_delete(TNDNoradPointer<_Ty>& ndnoradPtrRef) {
			ndnoradPtrRef.norad_delete();
		}
		template <class _Ty>
		void ndnorad_delete(TNDNoradConstPointer<_Ty>& ndnoradPtrRef) {
			ndnoradPtrRef.norad_delete();
		}
	}
}

namespace std {
	template<typename _Ty, typename _TRefCounter>
	struct hash<mse::us::impl::TGNoradPointer<_Ty, _TRefCounter> > {	// hash functor
		typedef mse::us::impl::TGNoradPointer<_Ty, _TRefCounter> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::us::impl::TGNoradPointer<_Ty, _TRefCounter>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
	template<typename _Ty, typename _TRefCounter>
	struct hash<mse::us::impl::TGNoradNotNullPointer<_Ty, _TRefCounter> > {	// hash functor
		typedef mse::us::impl::TGNoradNotNullPointer<_Ty, _TRefCounter> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::us::impl::TGNoradNotNullPointer<_Ty, _TRefCounter>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
	template<typename _Ty, typename _TRefCounter>
	struct hash<mse::us::impl::TGNoradFixedPointer<_Ty, _TRefCounter> > {	// hash functor
		typedef mse::us::impl::TGNoradFixedPointer<_Ty, _TRefCounter> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::us::impl::TGNoradFixedPointer<_Ty, _TRefCounter>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};

	template<typename _Ty, typename _TRefCounter>
	struct hash<mse::us::impl::TGNoradConstPointer<_Ty, _TRefCounter> > {	// hash functor
		typedef mse::us::impl::TGNoradConstPointer<_Ty, _TRefCounter> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::us::impl::TGNoradConstPointer<_Ty, _TRefCounter>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
	template<typename _Ty, typename _TRefCounter>
	struct hash<mse::us::impl::TGNoradNotNullConstPointer<_Ty, _TRefCounter> > {	// hash functor
		typedef mse::us::impl::TGNoradNotNullConstPointer<_Ty, _TRefCounter> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::us::impl::TGNoradNotNullConstPointer<_Ty, _TRefCounter>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
	template<typename _Ty, typename _TRefCounter>
	struct hash<mse::us::impl::TGNoradFixedConstPointer<_Ty, _TRefCounter> > {	// hash functor
		typedef mse::us::impl::TGNoradFixedConstPointer<_Ty, _TRefCounter> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::us::impl::TGNoradFixedConstPointer<_Ty, _TRefCounter>& _Keyval) const _NOEXCEPT {
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
		namespace impl {

			/* template specializations */

#define MSE_GNORAD_IMPL_OBJ_INHERIT_ASSIGNMENT_OPERATOR(class_name) \
		auto& operator=(class_name&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); } \
		auto& operator=(const class_name& _X) { base_class::operator=(_X); return (*this); } \
		template<class _Ty2> auto& operator=(_Ty2&& _X) { base_class::operator=(MSE_FWD(_X)); return (*this); } \
		template<class _Ty2> auto& operator=(const _Ty2& _X) { base_class::operator=(_X); return (*this); }

#define MSE_GNORAD_IMPL_OBJ_SPECIALIZATION_DEFINITIONS1(class_name) \
		class_name(const class_name&) = default; \
		class_name(class_name&&) = default; \
		MSE_GNORAD_IMPL_OBJ_INHERIT_ASSIGNMENT_OPERATOR(class_name);

#if !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)
#define MSE_GNORAD_IMPL_OBJ_NATIVE_POINTER_PRIVATE_CONSTRUCTORS1(class_name) \
			class_name(std::nullptr_t) {} \
			class_name() {}
#else // !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)
#define MSE_GNORAD_IMPL_OBJ_NATIVE_POINTER_PRIVATE_CONSTRUCTORS1(class_name)
#endif // !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)

	/* Note that because we explicitly define some (private) constructors, default copy and move constructors
	and assignment operators won't be generated, so we have to define those as well. */
#define MSE_GNORAD_IMPL_OBJ_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type) \
		template<typename _Ty, typename _TRefCounter> \
		class TGNoradObj<specified_type, _TRefCounter> : public TGNoradObj<mapped_type, _TRefCounter> { \
		public: \
			typedef TGNoradObj<mapped_type, _TRefCounter> base_class; \
			MSE_USING(TGNoradObj, base_class); \
			MSE_GNORAD_IMPL_OBJ_SPECIALIZATION_DEFINITIONS1(TGNoradObj); \
		private: \
			MSE_GNORAD_IMPL_OBJ_NATIVE_POINTER_PRIVATE_CONSTRUCTORS1(TGNoradObj); \
		};

#define MSE_GNORAD_IMPL_PTR_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type) \
		template<typename _Ty, typename _TRefCounter> \
		class TGNoradPointer<specified_type, _TRefCounter> : public TGNoradPointer<mapped_type, _TRefCounter> { \
		public: \
			typedef TGNoradPointer<mapped_type, _TRefCounter> base_class; \
			MSE_USING(TGNoradPointer, base_class); \
		}; \
		template<typename _Ty, typename _TRefCounter> \
		class TGNoradConstPointer<specified_type, _TRefCounter> : public TGNoradConstPointer<mapped_type, _TRefCounter> { \
		public: \
			typedef TGNoradConstPointer<mapped_type, _TRefCounter> base_class; \
			MSE_USING(TGNoradConstPointer, base_class); \
		}; \
		template<typename _Ty, typename _TRefCounter> \
		class TGNoradNotNullPointer<specified_type, _TRefCounter> : public TGNoradNotNullPointer<mapped_type, _TRefCounter> { \
		public: \
			typedef TGNoradNotNullPointer<mapped_type, _TRefCounter> base_class; \
			MSE_USING(TGNoradNotNullPointer, base_class); \
		}; \
		template<typename _Ty, typename _TRefCounter> \
		class TGNoradNotNullConstPointer<specified_type, _TRefCounter> : public TGNoradNotNullConstPointer<mapped_type, _TRefCounter> { \
		public: \
			typedef TGNoradNotNullConstPointer<mapped_type, _TRefCounter> base_class; \
			MSE_USING(TGNoradNotNullConstPointer, base_class); \
		}; \
		template<typename _Ty, typename _TRefCounter> \
		class TGNoradFixedPointer<specified_type, _TRefCounter> : public TGNoradFixedPointer<mapped_type, _TRefCounter> { \
		public: \
			typedef TGNoradFixedPointer<mapped_type, _TRefCounter> base_class; \
			MSE_USING(TGNoradFixedPointer, base_class); \
		}; \
		template<typename _Ty, typename _TRefCounter> \
		class TGNoradFixedConstPointer<specified_type, _TRefCounter> : public TGNoradFixedConstPointer<mapped_type, _TRefCounter> { \
		public: \
			typedef TGNoradFixedConstPointer<mapped_type, _TRefCounter> base_class; \
			MSE_USING(TGNoradFixedConstPointer, base_class); \
		};

#define MSE_GNORAD_IMPL_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type) \
		MSE_GNORAD_IMPL_PTR_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type); \
		MSE_GNORAD_IMPL_OBJ_NATIVE_POINTER_SPECIALIZATION(specified_type, mapped_type);

			MSE_GNORAD_IMPL_NATIVE_POINTER_SPECIALIZATION(_Ty*, mse::us::impl::TPointerForLegacy<_Ty>);
			MSE_GNORAD_IMPL_NATIVE_POINTER_SPECIALIZATION(_Ty* const, const mse::us::impl::TPointerForLegacy<_Ty>);

#ifdef MSEPRIMITIVES_H

#define MSE_GNORAD_IMPL_OBJ_INTEGRAL_SPECIALIZATION(integral_type) \
		template<typename _TRefCounter> \
		class TGNoradObj<integral_type, _TRefCounter> : public TGNoradObj<mse::TInt<integral_type>, _TRefCounter> { \
		public: \
			typedef TGNoradObj<mse::TInt<integral_type>, _TRefCounter> base_class; \
			MSE_USING(TGNoradObj, base_class); \
		};

#define MSE_GNORAD_IMPL_PTR_INTEGRAL_SPECIALIZATION(integral_type) \
		template<typename _TRefCounter> \
		class TGNoradPointer<integral_type, _TRefCounter> : public TGNoradPointer<mse::TInt<integral_type>, _TRefCounter> { \
		public: \
			typedef TGNoradPointer<mse::TInt<integral_type>, _TRefCounter> base_class; \
			MSE_USING(TGNoradPointer, base_class); \
		}; \
		template<typename _TRefCounter> \
		class TGNoradConstPointer<integral_type, _TRefCounter> : public TGNoradConstPointer<mse::TInt<integral_type>, _TRefCounter> { \
		public: \
			typedef TGNoradConstPointer<mse::TInt<integral_type>, _TRefCounter> base_class; \
			MSE_USING(TGNoradConstPointer, base_class); \
		}; \
		template<typename _TRefCounter> \
		class TGNoradNotNullPointer<integral_type, _TRefCounter> : public TGNoradNotNullPointer<mse::TInt<integral_type>, _TRefCounter> { \
		public: \
			typedef TGNoradNotNullPointer<mse::TInt<integral_type>, _TRefCounter> base_class; \
			MSE_USING(TGNoradNotNullPointer, base_class); \
		}; \
		template<typename _TRefCounter> \
		class TGNoradNotNullConstPointer<integral_type, _TRefCounter> : public TGNoradNotNullConstPointer<mse::TInt<integral_type>, _TRefCounter> { \
		public: \
			typedef TGNoradNotNullConstPointer<mse::TInt<integral_type>, _TRefCounter> base_class; \
			MSE_USING(TGNoradNotNullConstPointer, base_class); \
		}; \
		template<typename _TRefCounter> \
		class TGNoradFixedPointer<integral_type, _TRefCounter> : public TGNoradFixedPointer<mse::TInt<integral_type>, _TRefCounter> { \
		public: \
			typedef TGNoradFixedPointer<mse::TInt<integral_type>, _TRefCounter> base_class; \
			MSE_USING(TGNoradFixedPointer, base_class); \
		}; \
		template<typename _TRefCounter> \
		class TGNoradFixedConstPointer<integral_type, _TRefCounter> : public TGNoradFixedConstPointer<mse::TInt<integral_type>, _TRefCounter> { \
		public: \
			typedef TGNoradFixedConstPointer<mse::TInt<integral_type>, _TRefCounter> base_class; \
			MSE_USING(TGNoradFixedConstPointer, base_class); \
		};

#define MSE_GNORAD_IMPL_INTEGRAL_SPECIALIZATION(integral_type) \
		MSE_GNORAD_IMPL_PTR_INTEGRAL_SPECIALIZATION(integral_type); \
		MSE_GNORAD_IMPL_OBJ_INTEGRAL_SPECIALIZATION(integral_type); \
		MSE_GNORAD_IMPL_PTR_INTEGRAL_SPECIALIZATION(typename std::add_const<integral_type>::type); \
		MSE_GNORAD_IMPL_OBJ_INTEGRAL_SPECIALIZATION(typename std::add_const<integral_type>::type);

			MSE_IMPL_APPLY_MACRO_FUNCTION_TO_EACH_OF_THE_INTEGER_TYPES(MSE_GNORAD_IMPL_INTEGRAL_SPECIALIZATION)

#endif /*MSEPRIMITIVES_H*/

			/* end of template specializations */
		}
	}


#ifndef MSE_DO_NOT_DEFINE_NDNORAD_AS_ALIAS
#else // !MSE_DO_NOT_DEFINE_NDNORAD_AS_ALIAS
	/* TNDNoradPointer<>, like TNDCRegisteredPointer<>, behaves similar to native pointers. But where registered pointers are
	automatically set to nullptr when their target is destroyed, the destruction of an object while a "norad" pointer is targeting
	it results in program termination. This drastic consequence allows norad pointers' run-time safety mechanism to be very
	lightweight (compared to that of registered pointers). */
	template<typename _Ty>
	class TNDNoradPointer : public mse::us::impl::TPointer<TNDNoradObj<_Ty> >, public mse::us::impl::StrongPointerTagBase {
	public:
		TNDNoradPointer() : mse::us::impl::TPointer<TNDNoradObj<_Ty>>() {}
		TNDNoradPointer(const TNDNoradPointer& src_cref) : mse::us::impl::TPointer<TNDNoradObj<_Ty>>(src_cref.m_ptr) {
			if (*this) { (*(*this)).increment_refcount(); }
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradPointer(const TNDNoradPointer<_Ty2>& src_cref) : mse::us::impl::TPointer<TNDNoradObj<_Ty>>(src_cref.m_ptr) {
			if (*this) { (*(*this)).increment_refcount(); }
		}
		TNDNoradPointer(TNDNoradPointer&& src_ref) : mse::us::impl::TPointer<TNDNoradObj<_Ty>>(MSE_FWD(src_ref).m_ptr) {
			src_ref.m_ptr = nullptr;
		}
		TNDNoradPointer(std::nullptr_t) : mse::us::impl::TPointer<TNDNoradObj<_Ty>>(nullptr) {}
		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TNDNoradPointer() {
			if (*this) { (*(*this)).decrement_refcount(); }
		}
		TNDNoradPointer<_Ty>& operator=(const TNDNoradPointer<_Ty>& _Right_cref) {
			if (*this) { (*(*this)).decrement_refcount(); }
			mse::us::impl::TPointer<TNDNoradObj<_Ty>>::operator=(_Right_cref);
			if (*this) { (*(*this)).increment_refcount(); }
			return (*this);
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradPointer<_Ty>& operator=(const TNDNoradPointer<_Ty2>& _Right_cref) {
			return (*this).operator=(TNDNoradPointer(_Right_cref));
		}

		TNDNoradObj<_Ty>& operator*() const {
			if (!((*this).m_ptr)) { MSE_THROW(primitives_null_dereference_error("attempt to dereference null pointer - mse::TNoradPointer")); }
			return *((*this).m_ptr);
		}
		TNDNoradObj<_Ty>* operator->() const {
			if (!((*this).m_ptr)) { MSE_THROW(primitives_null_dereference_error("attempt to dereference null pointer - mse::TNoradPointer")); }
			return (*this).m_ptr;
		}

		operator bool() const { return !(!((*this).m_ptr)); }
		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		operator _Ty*() const {
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
		void norad_delete() {
			auto a = asANativePointerToTNDNoradObj();
			(*this) = nullptr;
			delete a;
		}

	private:
		TNDNoradPointer(TNDNoradObj<_Ty>* ptr) : mse::us::impl::TPointer<TNDNoradObj<_Ty>>(ptr) {
			assert(*this);
			(*(*this)).increment_refcount();
		}

		/* This function, if possible, should not be used. It is meant to be used exclusively by norad_delete<>(). */
		TNDNoradObj<_Ty>* asANativePointerToTNDNoradObj() const {
#ifdef NATIVE_PTR_DEBUG_HELPER1
			if (nullptr == (*this).m_ptr) {
				int q = 5; /* just a line of code for putting a debugger break point */
			}
#endif /*NATIVE_PTR_DEBUG_HELPER1*/
			return static_cast<TNDNoradObj<_Ty>*>((*this).m_ptr);
		}

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		template <class Y> friend class TNDNoradPointer;
		template <class Y> friend class TNDNoradConstPointer;
		friend class TNDNoradNotNullPointer<_Ty>;
	};

	template<typename _Ty>
	class TNDNoradConstPointer : public mse::us::impl::TPointer<const TNDNoradObj<_Ty> >, public mse::us::impl::StrongPointerTagBase {
	public:
		TNDNoradConstPointer() : mse::us::impl::TPointer<const TNDNoradObj<_Ty>>() {}
		TNDNoradConstPointer(const TNDNoradConstPointer& src_cref) : mse::us::impl::TPointer<const TNDNoradObj<_Ty>>(src_cref.m_ptr) {
			if (*this) { (*(*this)).increment_refcount(); }
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradConstPointer(const TNDNoradConstPointer<_Ty2>& src_cref) : mse::us::impl::TPointer<const TNDNoradObj<_Ty>>(src_cref.m_ptr) {
			if (*this) { (*(*this)).increment_refcount(); }
		}
		TNDNoradConstPointer(const TNDNoradPointer<_Ty>& src_cref) : mse::us::impl::TPointer<const TNDNoradObj<_Ty>>(src_cref.m_ptr) {
			if (*this) { (*(*this)).increment_refcount(); }
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradConstPointer(const TNDNoradPointer<_Ty2>& src_cref) : mse::us::impl::TPointer<const TNDNoradObj<_Ty>>(src_cref.m_ptr) {
			if (*this) { (*(*this)).increment_refcount(); }
		}

		TNDNoradConstPointer(TNDNoradConstPointer&& src_ref) : mse::us::impl::TPointer<const TNDNoradObj<_Ty>>(MSE_FWD(src_ref).m_ptr) {
			src_ref.m_ptr = nullptr;
		}
		TNDNoradConstPointer(TNDNoradPointer<_Ty>&& src_ref) : mse::us::impl::TPointer<const TNDNoradObj<_Ty>>(MSE_FWD(src_ref).m_ptr) {
			src_ref.m_ptr = nullptr;
		}

		TNDNoradConstPointer(std::nullptr_t) : mse::us::impl::TPointer<const TNDNoradObj<_Ty>>(nullptr) {}
		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TNDNoradConstPointer() {
			if (*this) { (*(*this)).decrement_refcount(); }
		}
		TNDNoradConstPointer<_Ty>& operator=(const TNDNoradConstPointer<_Ty>& _Right_cref) {
			if (*this) { (*(*this)).decrement_refcount(); }
			mse::us::impl::TPointer<const TNDNoradObj<_Ty>>::operator=(_Right_cref);
			if (*this) { (*(*this)).increment_refcount(); }
			return (*this);
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradConstPointer<_Ty>& operator=(const TNDNoradConstPointer<_Ty2>& _Right_cref) {
			return (*this).operator=(TNDNoradConstPointer(_Right_cref));
		}

		const TNDNoradObj<_Ty>& operator*() const {
			if (!((*this).m_ptr)) { MSE_THROW(primitives_null_dereference_error("attempt to dereference null pointer - mse::TNoradPointer")); }
			return *((*this).m_ptr);
		}
		const TNDNoradObj<_Ty>* operator->() const {
			if (!((*this).m_ptr)) { MSE_THROW(primitives_null_dereference_error("attempt to dereference null pointer - mse::TNoradPointer")); }
			return (*this).m_ptr;
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
		void norad_delete() {
			auto a = asANativePointerToTNDNoradObj();
			(*this) = nullptr;
			delete a;
		}

	private:
		TNDNoradConstPointer(const TNDNoradObj<_Ty>* ptr) : mse::us::impl::TPointer<const TNDNoradObj<_Ty>>(ptr) {
			assert(*this);
			(*(*this)).increment_refcount();
		}

		/* This function, if possible, should not be used. It is meant to be used exclusively by norad_delete<>(). */
		const TNDNoradObj<_Ty>* asANativePointerToTNDNoradObj() const {
#ifdef NATIVE_PTR_DEBUG_HELPER1
			if (nullptr == (*this).m_ptr) {
				int q = 5; /* just a line of code for putting a debugger break point */
			}
#endif /*NATIVE_PTR_DEBUG_HELPER1*/
			return static_cast<const TNDNoradObj<_Ty>*>((*this).m_ptr);
		}

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		template <class Y> friend class TNDNoradConstPointer;
		friend class TNDNoradNotNullConstPointer<_Ty>;
	};

	template<typename _Ty>
	class TNDNoradNotNullPointer : public TNDNoradPointer<_Ty>, public mse::us::impl::NeverNullTagBase {
	public:
		TNDNoradNotNullPointer(const TNDNoradNotNullPointer& src_cref) : TNDNoradPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradNotNullPointer(const TNDNoradNotNullPointer<_Ty2>& src_cref) : TNDNoradPointer<_Ty>(src_cref) {}
		TNDNoradNotNullPointer(TNDNoradNotNullPointer&& src_ref) : TNDNoradPointer<_Ty>(MSE_FWD(src_ref)) {}

		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TNDNoradNotNullPointer() {}
		/*
		TNDNoradNotNullPointer<_Ty>& operator=(const TNDNoradNotNullPointer<_Ty>& _Right_cref) {
		TNDNoradPointer<_Ty>::operator=(_Right_cref);
		return (*this);
		}
		*/

		TNDNoradObj<_Ty>& operator*() const {
			//if (!((*this).m_ptr)) { MSE_THROW(primitives_null_dereference_error("attempt to dereference null pointer - mse::TNoradPointer")); }
			return *((*this).m_ptr);
		}
		TNDNoradObj<_Ty>* operator->() const {
			//if (!((*this).m_ptr)) { MSE_THROW(primitives_null_dereference_error("attempt to dereference null pointer - mse::TNoradPointer")); }
			return (*this).m_ptr;
		}

		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator _Ty*() const { return TNDNoradPointer<_Ty>::operator _Ty*(); }
		MSE_DEPRECATED explicit operator TNDNoradObj<_Ty>*() const { return TNDNoradPointer<_Ty>::operator TNDNoradObj<_Ty>*(); }

	private:
		TNDNoradNotNullPointer(TNDNoradObj<_Ty>* ptr) : TNDNoradPointer<_Ty>(ptr) {}

		/* If you want to use this constructor, use not_null_from_nullable() instead. */
		TNDNoradNotNullPointer(const  TNDNoradPointer<_Ty>& src_cref) : TNDNoradPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradNotNullPointer(const TNDNoradPointer<_Ty2>& src_cref) : TNDNoradPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}

		/* If you want a pointer to a TNDNoradNotNullPointer<_Ty>, declare the TNDNoradNotNullPointer<_Ty> as a
		TNDNoradObj<TNDNoradNotNullPointer<_Ty>> instead. So for example:
		auto reg_ptr = TNDNoradObj<TNDNoradNotNullPointer<_Ty>>(mse::registered_new<_Ty>());
		auto reg_ptr_to_reg_ptr = &reg_ptr;
		*/
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		friend class TNDNoradFixedPointer<_Ty>;
		template<typename _Ty2>
		friend TNDNoradNotNullPointer<_Ty2> not_null_from_nullable(const TNDNoradPointer<_Ty2>& src);
	};

	template<typename _Ty>
	class TNDNoradNotNullConstPointer : public TNDNoradConstPointer<_Ty>, public mse::us::impl::NeverNullTagBase {
	public:
		TNDNoradNotNullConstPointer(const TNDNoradNotNullPointer<_Ty>& src_cref) : TNDNoradConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradNotNullConstPointer(const TNDNoradNotNullPointer<_Ty2>& src_cref) : TNDNoradConstPointer<_Ty>(src_cref) {}
		TNDNoradNotNullConstPointer(const TNDNoradNotNullConstPointer<_Ty>& src_cref) : TNDNoradConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradNotNullConstPointer(const TNDNoradNotNullConstPointer<_Ty2>& src_cref) : TNDNoradConstPointer<_Ty>(src_cref) {}

		TNDNoradNotNullConstPointer(TNDNoradNotNullPointer<_Ty>&& src_ref) : TNDNoradConstPointer<_Ty>(MSE_FWD(src_ref)) {}
		TNDNoradNotNullConstPointer(TNDNoradNotNullConstPointer<_Ty>&& src_ref) : TNDNoradConstPointer<_Ty>(MSE_FWD(src_ref)) {}

		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TNDNoradNotNullConstPointer() {}

		const TNDNoradObj<_Ty>& operator*() const {
			//if (!((*this).m_ptr)) { MSE_THROW(primitives_null_dereference_error("attempt to dereference null pointer - mse::TNoradPointer")); }
			return *((*this).m_ptr);
		}
		const TNDNoradObj<_Ty>* operator->() const {
			//if (!((*this).m_ptr)) { MSE_THROW(primitives_null_dereference_error("attempt to dereference null pointer - mse::TNoradPointer")); }
			return (*this).m_ptr;
		}

		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator const _Ty*() const { return TNDNoradConstPointer<_Ty>::operator const _Ty*(); }
		MSE_DEPRECATED explicit operator const TNDNoradObj<_Ty>*() const { return TNDNoradConstPointer<_Ty>::operator const TNDNoradObj<_Ty>*(); }

	private:
		TNDNoradNotNullConstPointer(const TNDNoradObj<_Ty>* ptr) : TNDNoradConstPointer<_Ty>(ptr) {}

		/* If you want to use this constructor, use not_null_from_nullable() instead. */
		TNDNoradNotNullConstPointer(const TNDNoradPointer<_Ty>& src_cref) : TNDNoradConstPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradNotNullConstPointer(const TNDNoradPointer<_Ty2>& src_cref) : TNDNoradConstPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}
		TNDNoradNotNullConstPointer(const TNDNoradConstPointer<_Ty>& src_cref) : TNDNoradConstPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradNotNullConstPointer(const TNDNoradConstPointer<_Ty2>& src_cref) : TNDNoradConstPointer<_Ty>(src_cref) {
			*src_cref; // to ensure that src_cref points to a valid target
		}

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		friend class TNDNoradFixedConstPointer<_Ty>;
		template<typename _Ty2>
		friend TNDNoradNotNullConstPointer<_Ty2> not_null_from_nullable(const TNDNoradConstPointer<_Ty2>& src);
	};

	template<typename _Ty>
	TNDNoradNotNullPointer<_Ty> not_null_from_nullable(const TNDNoradPointer<_Ty>& src) {
		return src;
	}
	template<typename _Ty>
	TNDNoradNotNullConstPointer<_Ty> not_null_from_nullable(const TNDNoradConstPointer<_Ty>& src) {
		return src;
	}

	/* TNDNoradFixedPointer cannot be retargeted or constructed without a target. This pointer is recommended for passing
	parameters by reference. */
	template<typename _Ty>
	class TNDNoradFixedPointer : public TNDNoradNotNullPointer<_Ty> {
	public:
		TNDNoradFixedPointer(const TNDNoradFixedPointer& src_cref) : TNDNoradNotNullPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradFixedPointer(const TNDNoradFixedPointer<_Ty2>& src_cref) : TNDNoradNotNullPointer<_Ty>(src_cref) {}

		TNDNoradFixedPointer(const TNDNoradNotNullPointer<_Ty>& src_cref) : TNDNoradNotNullPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradFixedPointer(const TNDNoradNotNullPointer<_Ty2>& src_cref) : TNDNoradNotNullPointer<_Ty>(src_cref) {}

		TNDNoradFixedPointer(TNDNoradFixedPointer&& src_ref) : TNDNoradNotNullPointer<_Ty>(MSE_FWD(src_ref)) {}
		TNDNoradFixedPointer(TNDNoradNotNullPointer<_Ty>&& src_ref) : TNDNoradNotNullPointer<_Ty>(MSE_FWD(src_ref)) {}

		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TNDNoradFixedPointer() {}

		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator _Ty*() const { return TNDNoradNotNullPointer<_Ty>::operator _Ty*(); }
		MSE_DEPRECATED explicit operator TNDNoradObj<_Ty>*() const { return TNDNoradNotNullPointer<_Ty>::operator TNDNoradObj<_Ty>*(); }

	private:
		TNDNoradFixedPointer(TNDNoradObj<_Ty>* ptr) : TNDNoradNotNullPointer<_Ty>(ptr) {}
		TNDNoradFixedPointer<_Ty>& operator=(const TNDNoradFixedPointer<_Ty>& _Right_cref) = delete;

		/* If you want a pointer to a TNDNoradFixedPointer<_Ty>, declare the TNDNoradFixedPointer<_Ty> as a
		TNDNoradObj<TNDNoradFixedPointer<_Ty>> instead. So for example:
		auto reg_ptr = TNDNoradObj<TNDNoradFixedPointer<_Ty>>(mse::registered_new<_Ty>());
		auto reg_ptr_to_reg_ptr = &reg_ptr;
		*/
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		friend class TNDNoradObj<_Ty>;
	};

	template<typename _Ty>
	class TNDNoradFixedConstPointer : public TNDNoradNotNullConstPointer<_Ty> {
	public:
		TNDNoradFixedConstPointer(const TNDNoradFixedPointer<_Ty>& src_cref) : TNDNoradNotNullConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradFixedConstPointer(const TNDNoradFixedPointer<_Ty2>& src_cref) : TNDNoradNotNullConstPointer<_Ty>(src_cref) {}
		TNDNoradFixedConstPointer(const TNDNoradFixedConstPointer<_Ty>& src_cref) : TNDNoradNotNullConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradFixedConstPointer(const TNDNoradFixedConstPointer<_Ty2>& src_cref) : TNDNoradNotNullConstPointer<_Ty>(src_cref) {}

		TNDNoradFixedConstPointer(const TNDNoradNotNullPointer<_Ty>& src_cref) : TNDNoradNotNullConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradFixedConstPointer(const TNDNoradNotNullPointer<_Ty2>& src_cref) : TNDNoradNotNullConstPointer<_Ty>(src_cref) {}
		TNDNoradFixedConstPointer(const TNDNoradNotNullConstPointer<_Ty>& src_cref) : TNDNoradNotNullConstPointer<_Ty>(src_cref) {}
		template<class _Ty2, MSE_IMPL_EIP mse::impl::enable_if_t<std::is_convertible<_Ty2 *, _Ty *>::value> MSE_IMPL_EIS >
		TNDNoradFixedConstPointer(const TNDNoradNotNullConstPointer<_Ty2>& src_cref) : TNDNoradNotNullConstPointer<_Ty>(src_cref) {}

		TNDNoradFixedConstPointer(TNDNoradFixedPointer<_Ty>&& src_ref) : TNDNoradNotNullConstPointer<_Ty>(MSE_FWD(src_ref)) {}
		TNDNoradFixedConstPointer(TNDNoradFixedConstPointer<_Ty>&& src_ref) : TNDNoradNotNullConstPointer<_Ty>(MSE_FWD(src_ref)) {}

		TNDNoradFixedConstPointer(TNDNoradNotNullPointer<_Ty>&& src_ref) : TNDNoradNotNullConstPointer<_Ty>(MSE_FWD(src_ref)) {}
		TNDNoradFixedConstPointer(TNDNoradNotNullConstPointer<_Ty>&& src_ref) : TNDNoradNotNullConstPointer<_Ty>(MSE_FWD(src_ref)) {}

		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TNDNoradFixedConstPointer() {}
		/* This native pointer cast operator is just for compatibility with existing/legacy code and ideally should never be used. */
		MSE_DEPRECATED explicit operator const _Ty*() const { return TNDNoradNotNullConstPointer<_Ty>::operator const _Ty*(); }
		MSE_DEPRECATED explicit operator const TNDNoradObj<_Ty>*() const { return TNDNoradNotNullConstPointer<_Ty>::operator const TNDNoradObj<_Ty>*(); }

	private:
		TNDNoradFixedConstPointer(const TNDNoradObj<_Ty>* ptr) : TNDNoradNotNullConstPointer<_Ty>(ptr) {}
		TNDNoradFixedConstPointer<_Ty>& operator=(const TNDNoradFixedConstPointer<_Ty>& _Right_cref) = delete;

		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		friend class TNDNoradObj<_Ty>;
	};

	/* This macro roughly simulates constructor inheritance. */
#define MSE_NORAD_OBJ_USING(Derived, Base) MSE_USING(Derived, Base)

	/* TNDNoradObj is intended as a transparent wrapper for other classes/objects. The purpose is to register the object's
	destruction so that TNDNoradPointers will avoid referencing destroyed objects. Note that TNDNoradObj can be used with
	objects allocated on the stack. */
	template<typename _TROFLy>
	class TNDNoradObj : public _TROFLy
		, public std::conditional<(!std::is_convertible<_TROFLy*, mse::us::impl::AsyncNotShareableTagBase*>::value)
			&& (!std::is_base_of<mse::us::impl::AsyncNotShareableTagBase, _TROFLy>::value)
			, mse::us::impl::AsyncNotShareableTagBase, mse::impl::TPlaceHolder<mse::us::impl::AsyncNotShareableTagBase, TNDNoradObj> >::type
		, public std::conditional<(!std::is_convertible<_TROFLy*, mse::us::impl::AsyncNotPassableTagBase*>::value)
			&& (!std::is_base_of<mse::us::impl::AsyncNotPassableTagBase, _TROFLy>::value)
			, mse::us::impl::AsyncNotPassableTagBase, mse::impl::TPlaceHolder<mse::us::impl::AsyncNotPassableTagBase, TNDNoradObj> >::type
	{
	public:
		typedef _TROFLy base_class;

		MSE_NORAD_OBJ_USING(TNDNoradObj, _TROFLy);
		TNDNoradObj(const TNDNoradObj& _X) : _TROFLy(_X) {}
		TNDNoradObj(TNDNoradObj&& _X) : _TROFLy(MSE_FWD(_X)) {}
		MSE_IMPL_DESTRUCTOR_PREFIX1 ~TNDNoradObj() {
			if (0 != m_counter) {
				/* It would be unsafe to allow this object to be destroyed as there are outstanding references to this object. */
#ifdef MSE_CUSTOM_FATAL_ERROR_MESSAGE_HANDLER
				MSE_CUSTOM_FATAL_ERROR_MESSAGE_HANDLER("Fatal Error: mse::TNDNoradObj<> destructed with outstanding references \n");
#endif // MSE_CUSTOM_FATAL_ERROR_MESSAGE_HANDLER
				assert(false); std::terminate();
			}
		}

		TNDNoradObj& operator=(TNDNoradObj&& _X) { _TROFLy::operator=(MSE_FWD(_X)); return (*this); }
		TNDNoradObj& operator=(const TNDNoradObj& _X) { _TROFLy::operator=(_X); return (*this); }
		template<class _Ty2>
		TNDNoradObj& operator=(_Ty2&& _X) { _TROFLy::operator=(MSE_FWD(_X)); return (*this); }
		template<class _Ty2>
		TNDNoradObj& operator=(const _Ty2& _X) { _TROFLy::operator=(_X); return (*this); }

		TNDNoradFixedPointer<_TROFLy> operator&() {
			return TNDNoradFixedPointer<_TROFLy>(this);
		}
		TNDNoradFixedConstPointer<_TROFLy> operator&() const {
			return TNDNoradFixedConstPointer<_TROFLy>(this);
		}
		TNDNoradFixedPointer<_TROFLy> mse_norad_fptr() { return TNDNoradFixedPointer<_TROFLy>(this); }
		TNDNoradFixedConstPointer<_TROFLy> mse_norad_fptr() const { return TNDNoradFixedConstPointer<_TROFLy>(this); }

		/* todo: make these private */
		void increment_refcount() const { m_counter += 1; }
		void decrement_refcount() const { m_counter -= 1; }

	private:
		mutable int m_counter = 0;
	};
}

namespace std {
	template<class _Ty>
	struct hash<mse::TNDNoradPointer<_Ty> > {	// hash functor
		typedef mse::TNDNoradPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDNoradPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
	template<class _Ty>
	struct hash<mse::TNDNoradNotNullPointer<_Ty> > {	// hash functor
		typedef mse::TNDNoradNotNullPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDNoradNotNullPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
	template<class _Ty>
	struct hash<mse::TNDNoradFixedPointer<_Ty> > {	// hash functor
		typedef mse::TNDNoradFixedPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDNoradFixedPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};

	template<class _Ty>
	struct hash<mse::TNDNoradConstPointer<_Ty> > {	// hash functor
		typedef mse::TNDNoradConstPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDNoradConstPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
	template<class _Ty>
	struct hash<mse::TNDNoradNotNullConstPointer<_Ty> > {	// hash functor
		typedef mse::TNDNoradNotNullConstPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDNoradNotNullConstPointer<_Ty>& _Keyval) const _NOEXCEPT {
			const _Ty* ptr1 = nullptr;
			if (_Keyval) {
				ptr1 = std::addressof(*_Keyval);
			}
			return (hash<const _Ty *>()(ptr1));
		}
	};
	template<class _Ty>
	struct hash<mse::TNDNoradFixedConstPointer<_Ty> > {	// hash functor
		typedef mse::TNDNoradFixedConstPointer<_Ty> argument_type;
		typedef size_t result_type;
		size_t operator()(const mse::TNDNoradFixedConstPointer<_Ty>& _Keyval) const _NOEXCEPT {
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

	template<typename _Ty>
	class TNDNoradObj<_Ty*> : public TNDNoradObj<mse::us::impl::TPointerForLegacy<_Ty>> {
	public:
		typedef TNDNoradObj<mse::us::impl::TPointerForLegacy<_Ty>> base_class;
		MSE_USING(TNDNoradObj, base_class);
#if !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)
	private:
		TNDNoradObj(std::nullptr_t) {}
		TNDNoradObj() {}
#endif // !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)
	};
	template<typename _Ty>
	class TNDNoradObj<const _Ty*> : public TNDNoradObj<mse::us::impl::TPointerForLegacy<const _Ty>> {
	public:
		typedef TNDNoradObj<mse::us::impl::TPointerForLegacy<const _Ty>> base_class;
		MSE_USING(TNDNoradObj, base_class);
#if !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)
	private:
		TNDNoradObj(std::nullptr_t) {}
		TNDNoradObj() {}
#endif // !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)
	};

	template<typename _Ty>
	class TNDNoradObj<_Ty* const> : public TNDNoradObj<const mse::us::impl::TPointerForLegacy<_Ty>> {
	public:
		typedef TNDNoradObj<const mse::us::impl::TPointerForLegacy<_Ty>> base_class;
		MSE_USING(TNDNoradObj, base_class);
#if !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)
	private:
		TNDNoradObj(std::nullptr_t) {}
		TNDNoradObj() {}
#endif // !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)
	};
	template<typename _Ty>
	class TNDNoradObj<const _Ty * const> : public TNDNoradObj<const mse::us::impl::TPointerForLegacy<const _Ty>> {
	public:
		typedef TNDNoradObj<const mse::us::impl::TPointerForLegacy<const _Ty>> base_class;
		MSE_USING(TNDNoradObj, base_class);
#if !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)
	private:
		TNDNoradObj(std::nullptr_t) {}
		TNDNoradObj() {}
#endif // !defined(MSE_SOME_POINTER_TYPE_IS_DISABLED)
	};

	template<typename _Ty>
	class TNDNoradPointer<_Ty*> : public TNDNoradPointer<mse::us::impl::TPointerForLegacy<_Ty>> {
	public:
		typedef TNDNoradPointer<mse::us::impl::TPointerForLegacy<_Ty>> base_class;
		MSE_USING(TNDNoradPointer, base_class);
	};
	template<typename _Ty>
	class TNDNoradPointer<_Ty* const> : public TNDNoradPointer<const mse::us::impl::TPointerForLegacy<_Ty>> {
	public:
		typedef TNDNoradPointer<const mse::us::impl::TPointerForLegacy<_Ty>> base_class;
		MSE_USING(TNDNoradPointer, base_class);
	};
	template<typename _Ty>
	class TNDNoradPointer<const _Ty *> : public TNDNoradPointer<mse::us::impl::TPointerForLegacy<const _Ty>> {
	public:
		typedef TNDNoradPointer<mse::us::impl::TPointerForLegacy<const _Ty>> base_class;
		MSE_USING(TNDNoradPointer, base_class);
	};
	template<typename _Ty>
	class TNDNoradPointer<const _Ty * const> : public TNDNoradPointer<const mse::us::impl::TPointerForLegacy<const _Ty>> {
	public:
		typedef TNDNoradPointer<const mse::us::impl::TPointerForLegacy<const _Ty>> base_class;
		MSE_USING(TNDNoradPointer, base_class);
	};

	template<typename _Ty>
	class TNDNoradConstPointer<_Ty*> : public TNDNoradConstPointer<mse::us::impl::TPointerForLegacy<_Ty>> {
	public:
		typedef TNDNoradConstPointer<mse::us::impl::TPointerForLegacy<_Ty>> base_class;
		MSE_USING(TNDNoradConstPointer, base_class);
	};
	template<typename _Ty>
	class TNDNoradConstPointer<_Ty* const> : public TNDNoradConstPointer<const mse::us::impl::TPointerForLegacy<_Ty>> {
	public:
		typedef TNDNoradConstPointer<const mse::us::impl::TPointerForLegacy<_Ty>> base_class;
		MSE_USING(TNDNoradConstPointer, base_class);
	};
	template<typename _Ty>
	class TNDNoradConstPointer<const _Ty *> : public TNDNoradConstPointer<mse::us::impl::TPointerForLegacy<const _Ty>> {
	public:
		typedef TNDNoradConstPointer<mse::us::impl::TPointerForLegacy<const _Ty>> base_class;
		MSE_USING(TNDNoradConstPointer, base_class);
	};
	template<typename _Ty>
	class TNDNoradConstPointer<const _Ty * const> : public TNDNoradConstPointer<const mse::us::impl::TPointerForLegacy<const _Ty>> {
	public:
		typedef TNDNoradConstPointer<const mse::us::impl::TPointerForLegacy<const _Ty>> base_class;
		MSE_USING(TNDNoradConstPointer, base_class);
	};

#ifdef MSEPRIMITIVES_H
	template<>
	class TNDNoradObj<int> : public TNDNoradObj<mse::TInt<int>> {
	public:
		typedef TNDNoradObj<mse::TInt<int>> base_class;
		MSE_USING(TNDNoradObj, base_class);
	};
	template<>
	class TNDNoradObj<const int> : public TNDNoradObj<const mse::TInt<int>> {
	public:
		typedef TNDNoradObj<const mse::TInt<int>> base_class;
		MSE_USING(TNDNoradObj, base_class);
	};
	template<>
	class TNDNoradPointer<int> : public TNDNoradPointer<mse::TInt<int>> {
	public:
		typedef TNDNoradPointer<mse::TInt<int>> base_class;
		MSE_USING(TNDNoradPointer, base_class);
	};
	template<>
	class TNDNoradPointer<const int> : public TNDNoradPointer<const mse::TInt<int>> {
	public:
		typedef TNDNoradPointer<const mse::TInt<int>> base_class;
		MSE_USING(TNDNoradPointer, base_class);
	};
	template<>
	class TNDNoradConstPointer<int> : public TNDNoradConstPointer<mse::TInt<int>> {
	public:
		typedef TNDNoradConstPointer<mse::TInt<int>> base_class;
		MSE_USING(TNDNoradConstPointer, base_class);
	};
	template<>
	class TNDNoradConstPointer<const int> : public TNDNoradConstPointer<const mse::TInt<int>> {
	public:
		typedef TNDNoradConstPointer<const mse::TInt<int>> base_class;
		MSE_USING(TNDNoradConstPointer, base_class);
	};

	template<>
	class TNDNoradObj<size_t> : public TNDNoradObj<mse::TInt<size_t>> {
	public:
		typedef TNDNoradObj<mse::TInt<size_t>> base_class;
		MSE_USING(TNDNoradObj, base_class);
	};
	template<>
	class TNDNoradObj<const size_t> : public TNDNoradObj<const mse::TInt<size_t>> {
	public:
		typedef TNDNoradObj<const mse::TInt<size_t>> base_class;
		MSE_USING(TNDNoradObj, base_class);
	};
	template<>
	class TNDNoradPointer<size_t> : public TNDNoradPointer<mse::TInt<size_t>> {
	public:
		typedef TNDNoradPointer<mse::TInt<size_t>> base_class;
		MSE_USING(TNDNoradPointer, base_class);
	};
	template<>
	class TNDNoradPointer<const size_t> : public TNDNoradPointer<const mse::TInt<size_t>> {
	public:
		typedef TNDNoradPointer<const mse::TInt<size_t>> base_class;
		MSE_USING(TNDNoradPointer, base_class);
	};
	template<>
	class TNDNoradConstPointer<size_t> : public TNDNoradConstPointer<mse::TInt<size_t>> {
	public:
		typedef TNDNoradConstPointer<mse::TInt<size_t>> base_class;
		MSE_USING(TNDNoradConstPointer, base_class);
	};
	template<>
	class TNDNoradConstPointer<const size_t> : public TNDNoradConstPointer<const mse::TInt<size_t>> {
	public:
		typedef TNDNoradConstPointer<const mse::TInt<size_t>> base_class;
		MSE_USING(TNDNoradConstPointer, base_class);
	};
#endif /*MSEPRIMITIVES_H*/

	/* end of template specializations */
#endif // !MSE_DO_NOT_DEFINE_NDNORAD_AS_ALIAS

	/* shorter aliases */
	template<typename _Ty> using np = TNoradPointer<_Ty>;
	template<typename _Ty> using ncp = TNoradConstPointer<_Ty>;
	template<typename _Ty> using nnnp = TNoradNotNullPointer<_Ty>;
	template<typename _Ty> using nnncp = TNoradNotNullConstPointer<_Ty>;
	template<typename _Ty> using nfp = TNoradFixedPointer<_Ty>;
	template<typename _Ty> using nfcp = TNoradFixedConstPointer<_Ty>;
	template<typename _TROFLy> using no = TNoradObj<_TROFLy>;

	template<typename _Ty> using nrd_ptr = TNoradPointer<_Ty>;
	template<typename _Ty> using nrd_cptr = TNoradConstPointer<_Ty>;
	template<typename _Ty> using nrd_nnptr = TNoradNotNullPointer<_Ty>;
	template<typename _Ty> using nrd_nncptr = TNoradNotNullConstPointer<_Ty>;
	template<typename _Ty> using nrd_fptr = TNoradFixedPointer<_Ty>;
	template<typename _Ty> using nrd_fcptr = TNoradFixedConstPointer<_Ty>;
	template<typename _TROFLy> using nrd_obj = TNoradObj<_TROFLy>;

	template <class _Ty, class... Args>
	TNoradPointer<_Ty> nnew(Args&&... args) { return norad_new<_Ty>(std::forward<Args>(args)...); }
	template <class _Ty>
	void ndelete(const TNoradPointer<_Ty>& regPtrRef) { norad_delete<_Ty>(regPtrRef); }

	/* deprecated aliases */
	template<typename _Ty> using TWNoradPointer MSE_DEPRECATED = TNDNoradPointer<_Ty>;
	template<typename _Ty> using TWNoradConstPointer MSE_DEPRECATED = TNDNoradConstPointer<_Ty>;
	template<typename _Ty> using TWNoradNotNullPointer MSE_DEPRECATED = TNDNoradNotNullPointer<_Ty>;
	template<typename _Ty> using TWNoradNotNullConstPointer MSE_DEPRECATED = TNDNoradNotNullConstPointer<_Ty>;
	template<typename _Ty> using TWNoradFixedPointer MSE_DEPRECATED = TNDNoradFixedPointer<_Ty>;
	template<typename _Ty> using TWNoradFixedConstPointer MSE_DEPRECATED = TNDNoradFixedConstPointer<_Ty>;
	template<typename _TROFLy> using TWNoradObj MSE_DEPRECATED = TNDNoradObj<_TROFLy>;

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
		class CNoradPtrTest1 {
		public:
			static void s_test1() {
#ifdef MSE_SELF_TESTS
				class C;

				class D {
				public:
					virtual ~D() {}
					mse::TNoradPointer<C> m_c_ptr = nullptr;
				};

				class C {
				public:
					C() {}
					mse::TNoradPointer<D> m_d_ptr = nullptr;
				};

				mse::TNoradObj<C> noradobj_c;
				mse::TNoradPointer<D> d_ptr = mse::norad_new<D>();

				noradobj_c.m_d_ptr = d_ptr;
				d_ptr->m_c_ptr = &noradobj_c;

				mse::TNoradConstPointer<C> rrcp = d_ptr->m_c_ptr;
				mse::TNoradConstPointer<C> rrcp2 = rrcp;
				const mse::TNoradObj<C> noradobj_e;
				rrcp = &noradobj_e;
				mse::TNoradFixedConstPointer<C> rrfcp = &noradobj_e;
				rrcp = mse::norad_new<C>();
				mse::norad_delete<C>(rrcp);

				/* We must make sure that there are no other references to the target of d_ptr before deleting it. Registered pointers don't
				have the same requirement. */
				noradobj_c.m_d_ptr = nullptr;

				mse::norad_delete<D>(d_ptr);

				{
					/* Polymorphic conversions. */
					class FD : public mse::TNoradObj<D> {};
					mse::TNoradObj<FD> norad_fd;
					mse::TNoradPointer<FD> FD_norad_ptr1 = &norad_fd;
					mse::TNoradPointer<D> D_norad_ptr4 = FD_norad_ptr1;
					D_norad_ptr4 = &norad_fd;
					mse::TNoradFixedPointer<D> D_norad_fptr1 = &norad_fd;
					mse::TNoradFixedConstPointer<D> D_norad_fcptr1 = &norad_fd;
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

#endif // MSENORAD_H_
