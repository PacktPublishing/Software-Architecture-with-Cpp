#pragma once
#ifndef MSEANY_H_
#define MSEANY_H_

/* The implementation of "any" is based on the open source one from https://github.com/thelink2012/any.

Note that this (pre-C++17) implementation doesn't really support over-aligned types.
*/

//
// Implementation of N4562 std::experimental::any (merged into C++17) for C++11 compilers.
//
// See also:
//   + http://en.cppreference.com/w/cpp/any
//   + http://en.cppreference.com/w/cpp/experimental/any
//   + http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4562.html#any
//   + https://cplusplus.github.io/LWG/lwg-active.html#2509
//
//
// Copyright (c) 2016 Denilson das Merc?s Amorim
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <typeinfo>
#include <type_traits>
#include <stdexcept>

#ifndef MSE_PUSH_MACRO_NOT_SUPPORTED
#pragma push_macro("MSE_THROW")
#endif // !MSE_PUSH_MACRO_NOT_SUPPORTED

#ifdef MSE_CUSTOM_THROW_DEFINITION
#define MSE_THROW(x) MSE_CUSTOM_THROW_DEFINITION(x)
#else // MSE_CUSTOM_THROW_DEFINITION
#define MSE_THROW(x) throw(x)
#endif // MSE_CUSTOM_THROW_DEFINITION

namespace mse
{

	class bad_any_cast : public std::bad_cast
	{
	public:
		const char* what() const noexcept override
		{
			return "bad any cast";
		}
	};

	namespace us {
		namespace impl {
			template <typename _Ty> class TAnyPointerBaseV1;
			template <typename _Ty> class TAnyConstPointerBaseV1;
			template <typename _Ty> class TAnyRandomAccessIteratorBase;
			template <typename _Ty> class TAnyRandomAccessConstIteratorBase;
		}
	}

	class any final
	{
	public:
		/// Constructs an object of type any with an empty state.
		any() :
			vtable(nullptr)
		{
		}

		/// Constructs an object of type any with an equivalent state as other.
		any(const any& rhs) :
			vtable(rhs.vtable)
		{
			if (!rhs.empty())
			{
				rhs.vtable->copy(rhs.storage, this->storage);
			}
		}

		/// Constructs an object of type any with a state equivalent to the original state of other.
		/// rhs is left in a valid but otherwise unspecified state.
		any(any&& rhs) noexcept :
		vtable(rhs.vtable)
		{
			if (!rhs.empty())
			{
				rhs.vtable->move(rhs.storage, this->storage);
				rhs.vtable = nullptr;
			}
		}

		/// Same effect as this->clear().
		~any()
		{
			this->clear();
		}

		/// Constructs an object of type any that contains an object of type T direct-initialized with std::forward<ValueType>(value).
		///
		/// T shall satisfy the CopyConstructible requirements, otherwise the program is ill-formed.
		/// This is because an `any` may be copy constructed into another `any` at any time, so a copy should always be allowed.
		template<typename ValueType, MSE_IMPL_EIP mse::impl::enable_if_t<!std::is_same<typename std::decay<ValueType>::type, any>::value> MSE_IMPL_EIS >
		any(ValueType&& value)
		{
			static_assert(std::is_copy_constructible<typename std::decay<ValueType>::type>::value,
				"T shall satisfy the CopyConstructible requirements.");
			this->construct(std::forward<ValueType>(value));
		}

		/// Has the same effect as any(rhs).swap(*this). No effects if an exception is thrown.
		any& operator=(const any& rhs)
		{
			any(rhs).swap(*this);
			return *this;
		}

		/// Has the same effect as any(std::move(rhs)).swap(*this).
		///
		/// The state of *this is equivalent to the original state of rhs and rhs is left in a valid
		/// but otherwise unspecified state.
		any& operator=(any&& rhs) noexcept
		{
			any(std::move(rhs)).swap(*this);
			return *this;
		}

		/// Has the same effect as any(std::forward<ValueType>(value)).swap(*this). No effect if a exception is thrown.
		///
		/// T shall satisfy the CopyConstructible requirements, otherwise the program is ill-formed.
		/// This is because an `any` may be copy constructed into another `any` at any time, so a copy should always be allowed.
		template<typename ValueType, MSE_IMPL_EIP mse::impl::enable_if_t<!std::is_same<typename std::decay<ValueType>::type, any>::value> MSE_IMPL_EIS >
		any& operator=(ValueType&& value)
		{
			static_assert(std::is_copy_constructible<typename std::decay<ValueType>::type>::value,
				"T shall satisfy the CopyConstructible requirements.");
			any(std::forward<ValueType>(value)).swap(*this);
			return *this;
		}

		/// If not empty, destroys the contained object.
		void clear() noexcept
		{
			if (!empty())
			{
				this->vtable->destroy(storage);
				this->vtable = nullptr;
			}
		}

		/// Returns true if *this has no contained object, otherwise false.
		bool empty() const noexcept
		{
			return this->vtable == nullptr;
		}

		/// If *this has a contained object of type T, typeid(T); otherwise typeid(void).
		const std::type_info& type() const noexcept
		{
			return empty() ? typeid(void) : this->vtable->type();
		}

		/// Exchange the states of *this and rhs.
		void swap(any& rhs) noexcept
		{
			if (this->vtable != rhs.vtable)
			{
				any tmp(std::move(rhs));

				// move from *this to rhs.
				rhs.vtable = this->vtable;
				if (this->vtable != nullptr)
				{
					this->vtable->move(this->storage, rhs.storage);
					//this->vtable = nullptr; -- uneeded, see below
				}

				// move from tmp (previously rhs) to *this.
				this->vtable = tmp.vtable;
				if (tmp.vtable != nullptr)
				{
					tmp.vtable->move(tmp.storage, this->storage);
					tmp.vtable = nullptr;
				}
			}
			else // same types
			{
				if (this->vtable != nullptr)
					this->vtable->swap(this->storage, rhs.storage);
			}
		}

	private: // Storage and Virtual Method Table

		void* storage_address() noexcept
		{
			return empty() ? nullptr : this->vtable->storage_address(storage);
		}
		const void* storage_address() const noexcept
		{
			return empty() ? nullptr : this->vtable->const_storage_address(storage);
		}

		union storage_union
		{
			using stack_storage_t = typename std::aligned_storage<2 * sizeof(void*), std::alignment_of<void*>::value>::type;

			void*               dynamic;
			stack_storage_t     stack;      // 2 words for e.g. shared_ptr
		};

		/// Base VTable specification.
		struct vtable_type
		{
			// Note: The caller is responssible for doing .vtable = nullptr after destructful operations
			// such as destroy() and/or move().

			/// The type of the object this vtable is for.
			const std::type_info& (*type)() noexcept;

			/// Destroys the object in the union.
			/// The state of the union after this call is unspecified, caller must ensure not to use src anymore.
			void(*destroy)(storage_union&) noexcept;

			/// Copies the **inner** content of the src union into the yet unitialized dest union.
			/// As such, both inner objects will have the same state, but on separate memory locations.
			void(*copy)(const storage_union& src, storage_union& dest);

			/// Moves the storage from src to the yet unitialized dest union.
			/// The state of src after this call is unspecified, caller must ensure not to use src anymore.
			void(*move)(storage_union& src, storage_union& dest) noexcept;

			/// Exchanges the storage between lhs and rhs.
			void(*swap)(storage_union& lhs, storage_union& rhs) noexcept;

			void* (*storage_address)(storage_union&) noexcept;
			const void* (*const_storage_address)(const storage_union&) noexcept;
		};

		/// VTable for dynamically allocated storage.
		template<typename T>
		struct vtable_dynamic
		{
			static const std::type_info& type() noexcept
			{
				return typeid(T);
			}

			static void destroy(storage_union& storage) noexcept
			{
				//assert(reinterpret_cast<T*>(storage.dynamic));
				delete reinterpret_cast<T*>(storage.dynamic);
			}

			static void copy(const storage_union& src, storage_union& dest)
			{
				dest.dynamic = new T(*reinterpret_cast<const T*>(src.dynamic));
			}

			static void move(storage_union& src, storage_union& dest) noexcept
			{
				dest.dynamic = src.dynamic;
				src.dynamic = nullptr;
			}

			static void swap(storage_union& lhs, storage_union& rhs) noexcept
			{
				// just exchage the storage pointers.
				std::swap(lhs.dynamic, rhs.dynamic);
			}

			static void* storage_address(storage_union& storage) noexcept
			{
				return static_cast<void*>(storage.dynamic);
			}

			static const void* const_storage_address(const storage_union& storage) noexcept
			{
				return static_cast<const void*>(storage.dynamic);
			}
		};

		/// VTable for stack allocated storage.
		template<typename T>
		struct vtable_stack
		{
			static const std::type_info& type() noexcept
			{
				return typeid(T);
			}

			static void destroy(storage_union& storage) noexcept
			{
				reinterpret_cast<T*>(&storage.stack)->~T();
			}

			static void copy(const storage_union& src, storage_union& dest)
			{
				new (&dest.stack) T(reinterpret_cast<const T&>(src.stack));
			}

			static void move(storage_union& src, storage_union& dest) noexcept
			{
				// one of the conditions for using vtable_stack is a nothrow move constructor,
				// so this move constructor will never throw a exception.
				new (&dest.stack) T(std::move(reinterpret_cast<T&>(src.stack)));
				destroy(src);
			}

			static void swap(storage_union& lhs, storage_union& rhs) noexcept
			{
				std::swap(reinterpret_cast<T&>(lhs.stack), reinterpret_cast<T&>(rhs.stack));
			}

			static void* storage_address(storage_union& storage) noexcept
			{
				return static_cast<void*>(&storage.stack);
			}

			static const void* const_storage_address(const storage_union& storage) noexcept
			{
				return static_cast<const void*>(&storage.stack);
			}
		};

		/// Whether the type T must be dynamically allocated or can be stored on the stack.
		template<typename T>
		struct requires_allocation :
			std::integral_constant<bool,
			!(std::is_nothrow_move_constructible<T>::value      // N4562 ?6.3/3 [any.class]
				&& sizeof(T) <= sizeof(storage_union::stack)
				&& std::alignment_of<T>::value <= std::alignment_of<storage_union::stack_storage_t>::value)>
		{};

		/// Returns the pointer to the vtable of the type T.
		template<typename T>
		static vtable_type* vtable_for_type()
		{
			using VTableType = mse::impl::conditional_t<requires_allocation<T>::value, vtable_dynamic<T>, vtable_stack<T>>;
			static vtable_type table = {
				VTableType::type, VTableType::destroy,
				VTableType::copy, VTableType::move,
				VTableType::swap, VTableType::storage_address,
				VTableType::const_storage_address,
			};
			return &table;
		}

	protected:
		template<typename T>
		friend const T* any_cast(const any* operand) noexcept;
		template<typename T>
		friend T* any_cast(any* operand) noexcept;

		/// Same effect as is_same(this->type(), t);
		bool is_typed(const std::type_info& t) const
		{
			return is_same(this->type(), t);
		}

		/// Checks if two type infos are the same.
		///
		/// If ANY_IMPL_FAST_TYPE_INFO_COMPARE is defined, checks only the address of the
		/// type infos, otherwise does an actual comparision. Checking addresses is
		/// only a valid approach when there's no interaction with outside sources
		/// (other shared libraries and such).
		static bool is_same(const std::type_info& a, const std::type_info& b)
		{
#ifdef ANY_IMPL_FAST_TYPE_INFO_COMPARE
			return &a == &b;
#else
			return a == b;
#endif
		}

		/// Casts (with no type_info checks) the storage pointer as const T*.
		template<typename T>
		const T* cast() const noexcept
		{
			return requires_allocation<typename std::decay<T>::type>::value ?
				reinterpret_cast<const T*>(storage.dynamic) :
				reinterpret_cast<const T*>(&storage.stack);
		}

		/// Casts (with no type_info checks) the storage pointer as T*.
		template<typename T>
		T* cast() noexcept
		{
			return requires_allocation<typename std::decay<T>::type>::value ?
				reinterpret_cast<T*>(storage.dynamic) :
				reinterpret_cast<T*>(&storage.stack);
		}

	private:
		storage_union storage; // on offset(0) so no padding for align
		vtable_type*  vtable;

		template<typename ValueType, typename T>
		mse::impl::enable_if_t<requires_allocation<T>::value>
			do_construct(ValueType&& value)
		{
			storage.dynamic = new T(std::forward<ValueType>(value));
		}

		template<typename ValueType, typename T>
		mse::impl::enable_if_t<!requires_allocation<T>::value>
			do_construct(ValueType&& value)
		{
			new (&storage.stack) T(std::forward<ValueType>(value));
		}

		/// Chooses between stack and dynamic allocation for the type decay_t<ValueType>,
		/// assigns the correct vtable, and constructs the object on our storage.
		template<typename ValueType>
		void construct(ValueType&& value)
		{
			using T = typename std::decay<ValueType>::type;

			this->vtable = vtable_for_type<T>();

			do_construct<ValueType, T>(std::forward<ValueType>(value));
		}

		template<typename _Ty2> friend class us::impl::TAnyPointerBaseV1;
		template<typename _Ty2> friend class us::impl::TAnyConstPointerBaseV1;
		template<typename _Ty2> friend class us::impl::TAnyRandomAccessIteratorBase;
		template<typename _Ty2> friend class us::impl::TAnyRandomAccessConstIteratorBase;
	};



	namespace detail
	{
		template<typename ValueType>
		inline ValueType any_cast_move_if_true(mse::impl::remove_reference_t<ValueType>* p, std::true_type)
		{
			return std::move(*p);
		}

		template<typename ValueType>
		inline ValueType any_cast_move_if_true(mse::impl::remove_reference_t<ValueType>* p, std::false_type)
		{
			return *p;
		}
	}

	/// Performs *any_cast<add_const_t<remove_reference_t<ValueType>>>(&operand), or throws bad_any_cast on failure.
	template<typename ValueType>
	inline ValueType any_cast(const any& operand)
	{
		auto p = any_cast<typename std::add_const<mse::impl::remove_reference_t<ValueType> >::type>(&operand);
		if (p == nullptr) MSE_THROW(bad_any_cast());
		return *p;
	}

	/// Performs *any_cast<remove_reference_t<ValueType>>(&operand), or throws bad_any_cast on failure.
	template<typename ValueType>
	inline ValueType any_cast(any& operand)
	{
		auto p = any_cast<mse::impl::remove_reference_t<ValueType> >(&operand);
		if (p == nullptr) MSE_THROW(bad_any_cast());
		return *p;
	}

	///
	/// If ANY_IMPL_ANYCAST_MOVEABLE is not defined, does as N4562 specifies:
	///     Performs *any_cast<remove_reference_t<ValueType>>(&operand), or throws bad_any_cast on failure.
	///
	/// If ANY_IMPL_ANYCAST_MOVEABLE is defined, does as LWG Defect 2509 specifies:
	///     If ValueType is MoveConstructible and isn't a lvalue reference, performs
	///     std::move(*any_cast<remove_reference_t<ValueType>>(&operand)), otherwise
	///     *any_cast<remove_reference_t<ValueType>>(&operand). Throws bad_any_cast on failure.
	///
	template<typename ValueType>
	inline ValueType any_cast(any&& operand)
	{
#ifdef ANY_IMPL_ANY_CAST_MOVEABLE
		// https://cplusplus.github.io/LWG/lwg-active.html#2509
		using can_move = std::integral_constant<bool,
			std::is_move_constructible<ValueType>::value
			&& !std::is_lvalue_reference<ValueType>::value>;
#else
		using can_move = std::false_type;
#endif

		auto p = any_cast<mse::impl::remove_reference_t<ValueType> >(&operand);
		if (p == nullptr) MSE_THROW(bad_any_cast());
		return detail::any_cast_move_if_true<ValueType>(p, can_move());
	}

	/// If operand != nullptr && operand->type() == typeid(ValueType), a pointer to the object
	/// contained by operand, otherwise nullptr.
	template<typename T>
	inline const T* any_cast(const any* operand) noexcept
	{
		if (operand == nullptr || !operand->is_typed(typeid(T)))
			return nullptr;
		else
			return operand->cast<T>();
	}

	/// If operand != nullptr && operand->type() == typeid(ValueType), a pointer to the object
	/// contained by operand, otherwise nullptr.
	template<typename T>
	inline T* any_cast(any* operand) noexcept
	{
		if (operand == nullptr || !operand->is_typed(typeid(T)))
			return nullptr;
		else
			return operand->cast<T>();
	}

}

namespace std
{
	inline void swap(mse::any& lhs, mse::any& rhs) noexcept
	{
		lhs.swap(rhs);
	}
}



#include "msepointerbasics.h"

namespace mse {

	namespace impl {
		template <class _TPointer>
		bool operator_bool_helper1(std::true_type, const _TPointer& ptr_cref) {
			return !(!ptr_cref);
		}
		template <class _TPointer>
		bool operator_bool_helper1(std::false_type, const _TPointer&) {
			/* We need to return the result of conversion to bool, but in this case the "pointer" type, _TPointer, is not convertible
			to bool. Presumably because _TPointer is actually an iterator type. Unfortunately there isn't a good way, in general, to
			determine if an iterator points to a valid item. */
			assert(false);
			return false;
		}
	}

	namespace us {
		namespace impl {
			template <typename _Ty> class TAnyPointerBaseV1;
			template <typename _Ty> class TAnyConstPointerBaseV1;
			template <typename _Ty> using TAnyPointerBase = TAnyPointerBaseV1<_Ty>;
			template <typename _Ty> using TAnyConstPointerBase = TAnyConstPointerBaseV1<_Ty>;
		}
	}

	namespace us {
		namespace impl {

			template <typename _Ty>
			class TCommonPointerInterface {
			public:
				virtual ~TCommonPointerInterface() {}
				virtual _Ty& operator*() const = 0;
				virtual _Ty* operator->() const = 0;
				virtual operator bool() const = 0;
			};

			template <typename _Ty, typename _TPointer1>
			class TCommonizedPointer : public TCommonPointerInterface<_Ty> {
			public:
				TCommonizedPointer(const _TPointer1& pointer) : m_pointer(pointer) {}
				virtual ~TCommonizedPointer() {}

				_Ty& operator*() const {
					/* Using the mse::us::impl::raw_reference_to<>() function allows us to, for example, obtain an 'int&' to
					an mse::Tint<int>. This allows a pointer to an mse::TInt<int> to be used as a pointer to an int. */
					return mse::us::impl::raw_reference_to<_Ty>(*m_pointer);
				}
				_Ty* operator->() const {
					return std::addressof(mse::us::impl::raw_reference_to<_Ty>(*m_pointer));
				}
				operator bool() const {
					//return bool(m_pointer);
					return mse::impl::operator_bool_helper1<_TPointer1>(typename std::is_convertible<_TPointer1, bool>::type(), m_pointer);
				}

				_TPointer1 m_pointer;
			};

			template <typename _Ty>
			class TAnyPointerBaseV1 {
			public:
				TAnyPointerBaseV1(const TAnyPointerBaseV1& src) : m_any_pointer(src.m_any_pointer) {}

				template <typename _TPointer1, MSE_IMPL_EIP mse::impl::enable_if_t<
					(!std::is_convertible<_TPointer1, TAnyPointerBaseV1>::value)
					&& (!std::is_base_of<TAnyConstPointerBase<_Ty>, _TPointer1>::value)
					> MSE_IMPL_EIS >
					TAnyPointerBaseV1(const _TPointer1& pointer) : m_any_pointer(TCommonizedPointer<_Ty, _TPointer1>(pointer)) {}

				_Ty& operator*() const {
					return (*(*common_pointer_interface_ptr()));
				}
				_Ty* operator->() const {
					return std::addressof(*(*common_pointer_interface_ptr()));
				}
				template <typename _Ty2>
				bool operator ==(const _Ty2& _Right_cref) const {
					/* Note that both underlying stored pointers are dereferenced here and we may be relying on the intrinsic
					safety of those pointers to ensure the safety of the dereference operations. */
					return (std::addressof(*(*this)) == std::addressof(*_Right_cref));
				}
				template <typename _Ty2>
				bool operator !=(const _Ty2& _Right_cref) const { return !((*this) == _Right_cref); }

				operator bool() const {
					return bool(*common_pointer_interface_ptr());
				}

			protected:
				MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

				const TCommonPointerInterface<_Ty>* common_pointer_interface_ptr() const {
					auto retval = static_cast<const TCommonPointerInterface<_Ty>*>(m_any_pointer.storage_address());
					assert(nullptr != retval);
					return retval;
				}

				mse::any m_any_pointer;

				friend class TAnyConstPointerBaseV1<_Ty>;
			};
		}
	}

	namespace us {
		namespace impl {
			template <typename _Ty>
			class TCommonConstPointerInterface {
			public:
				virtual ~TCommonConstPointerInterface() {}
				virtual const _Ty& operator*() const = 0;
				virtual const _Ty* operator->() const = 0;
				virtual operator bool() const = 0;
			};

			template <typename _Ty, typename _TConstPointer1>
			class TCommonizedConstPointer : public TCommonConstPointerInterface<_Ty> {
			public:
				TCommonizedConstPointer(const _TConstPointer1& const_pointer) : m_const_pointer(const_pointer) {}
				virtual ~TCommonizedConstPointer() {}

				const _Ty& operator*() const {
					/* Using the mse::us::impl::raw_reference_to<>() function allows us to, for example, obtain a 'const int&' to
					an mse::Tint<int>. This allows a pointer to an mse::TInt<int> to be used as a pointer to a const int. */
					return mse::us::impl::raw_reference_to<const _Ty>(*m_const_pointer);
				}
				const _Ty* operator->() const {
					return std::addressof(mse::us::impl::raw_reference_to<const _Ty>(*m_const_pointer));
				}
				operator bool() const {
					//return bool(m_const_pointer);
					return mse::impl::operator_bool_helper1<_TConstPointer1>(typename std::is_convertible<_TConstPointer1, bool>::type(), m_const_pointer);
				}

				_TConstPointer1 m_const_pointer;
			};

			template <typename _Ty>
			class TAnyConstPointerBaseV1 {
			public:
				TAnyConstPointerBaseV1(const TAnyConstPointerBaseV1& src) : m_any_const_pointer(src.m_any_const_pointer) {}
				TAnyConstPointerBaseV1(const TAnyPointerBaseV1<_Ty>& src) : m_any_const_pointer(src.m_any_pointer) {}

				template <typename _TPointer1, MSE_IMPL_EIP mse::impl::enable_if_t<
					(!std::is_convertible<_TPointer1, TAnyConstPointerBaseV1>::value)
					&& (!std::is_convertible<TAnyPointerBaseV1<_Ty>, _TPointer1>::value)
					> MSE_IMPL_EIS >
					TAnyConstPointerBaseV1(const _TPointer1& pointer) : m_any_const_pointer(TCommonizedConstPointer<_Ty, _TPointer1>(pointer)) {}

				const _Ty& operator*() const {
					return (*(*common_pointer_interface_const_ptr()));
				}
				const _Ty* operator->() const {
					return std::addressof(*(*common_pointer_interface_const_ptr()));
				}
				operator bool() const {
					return bool(*common_pointer_interface_const_ptr());
				}
				template <typename _Ty2>
				bool operator ==(const _Ty2& _Right_cref) const {
					/* Note that both underlying stored pointers are dereferenced here and we may be relying on the intrinsic
					safety of those pointers to ensure the safety of the dereference operations. */
					return (std::addressof(*(*this)) == std::addressof(*_Right_cref));
				}
				template <typename _Ty2>
				bool operator !=(const _Ty2& _Right_cref) const { return !((*this) == _Right_cref); }

			protected:
				MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

				const TCommonPointerInterface<_Ty>* common_pointer_interface_const_ptr() const {
					/* This use of mse::any::storage_address() brings to mind the fact that the (pre-C++17) implementation
					of mse::any that we're using does not support over-aligned types. (And therefore neither does this
					template.) Though it's hard to imagine a reason why a pointer would be declared an over-aligned type. */
					auto retval = static_cast<const TCommonPointerInterface<_Ty>*>(m_any_const_pointer.storage_address());
					assert(nullptr != retval);
					return retval;
				}

				mse::any m_any_const_pointer;
			};
		}
	}
}

#ifndef MSE_PUSH_MACRO_NOT_SUPPORTED
#pragma pop_macro("MSE_THROW")
#endif // !MSE_PUSH_MACRO_NOT_SUPPORTED

#endif // MSEANY_H_
