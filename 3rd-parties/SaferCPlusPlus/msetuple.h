
#pragma once
# ifndef MSETUPLE_H_
# define MSETUPLE_H_

#ifndef MSE_TUPLE_NO_XSCOPE_DEPENDENCE
#include "msescope.h"
#endif // !MSE_TUPLE_NO_XSCOPE_DEPENDENCE
#include "msepointerbasics.h"

#include<tuple>
#include<type_traits>
#include<functional>

#ifdef MSE_SELF_TESTS
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <memory>
#endif // MSE_SELF_TESTS


#ifndef MSE_PUSH_MACRO_NOT_SUPPORTED
#pragma push_macro("MSE_THROW")
#pragma push_macro("_NODISCARD")
#endif // !MSE_PUSH_MACRO_NOT_SUPPORTED

#ifdef MSE_CUSTOM_THROW_DEFINITION
#define MSE_THROW(x) MSE_CUSTOM_THROW_DEFINITION(x)
#else // MSE_CUSTOM_THROW_DEFINITION
#define MSE_THROW(x) throw(x)
#endif // MSE_CUSTOM_THROW_DEFINITION

#ifndef _NODISCARD
#ifdef MSE_HAS_CXX17
#define _NODISCARD [[nodiscard]]
#else // MSE_HAS_CXX17
#define _NODISCARD
#endif // MSE_HAS_CXX17
#endif // !_NODISCARD

#ifdef _MSC_VER
#pragma warning( push )  
#pragma warning( disable : 4702 4189 )
#endif /*_MSC_VER*/

#if (defined(__GNUC__) || defined(__GNUG__))
#define GPP_COMPATIBLE 1
#if ((7 > __GNUC__) && (!defined(__clang__)))
#define GPP6_COMPATIBLE 1
#endif /*((7 > __GNUC__) && (!defined(__clang__)))*/
#endif /*(defined(__GNUC__) || defined(__GNUG__))*/

#ifdef GPP6_COMPATIBLE
/* Inheriting constructors seems to not work quite right in g++ 5.5. It does seem to work fine in g++ 7. So for versions
prior to 7 we'll just alias the tuples to std::tuple<>. This means that the tuples won't (automatically) be recognized as
async shareable or passable. */
namespace mse {
	namespace mstd {
		template<class... _Types> using tuple = std::tuple<_Types...>;

		template<class... _Types>
		constexpr auto make_tuple(_Types&&... _Args) {
			return std::make_tuple(std::forward<_Types>(_Args)...);
		}

		template<class... _Tuples>
		_NODISCARD constexpr auto tuple_cat(_Tuples&&... _Tpls) {
			return std::tuple_cat(std::forward<_Tuples>(_Tpls)...);
		}
	}
	template<class... _Types> using tuple = mstd::tuple<_Types...>;

	template<class... _Types> using xscope_tuple = std::tuple<_Types...>;

	template<class... _Types>
	constexpr auto make_xscope_tuple(_Types&&... _Args) {
		return std::make_tuple(std::forward<_Types>(_Args)...);
	}

	template<class... _Tuples>
	_NODISCARD constexpr auto xscope_tuple_cat(_Tuples&&... _Tpls) {
		return std::tuple_cat(std::forward<_Tuples>(_Tpls)...);
	}
}
#else // GPP6_COMPATIBLE

namespace mse {
	namespace mstd {
		template<class... _Types> class tuple;
	}
	template<class... _Types> using tuple = mstd::tuple<_Types...>;
	template<class... _Types> class xscope_tuple;

	namespace impl {
		template<class>
		// false value attached to a dependent name (for static_assert)
		MSE_INLINE_VAR constexpr bool _Always_false = false;

		template<class _Ty>
		struct _Unrefwrap_helper
		{	// leave unchanged if not a reference_wrapper
			using type = _Ty;
		};
		template<class _Ty>
		struct _Unrefwrap_helper<std::reference_wrapper<_Ty>>
		{	// make a reference from a reference_wrapper
			using type = _Ty & ;
		};
		// decay, then unwrap a reference_wrapper
		template<class _Ty>
		using _Unrefwrap_t = typename _Unrefwrap_helper<std::decay_t<_Ty>>::type;
	}

	namespace impl {
		namespace tuple {

			template<class _Ty, bool>
			struct adjusted_tuple_cat_argument_type_helper1 {
				using type = _Ty;
			};
			template<class _Ty>
			struct adjusted_tuple_cat_argument_type_helper1<_Ty, true> {
				//using type = const typename _Ty::base_class &;
				using type = typename _Ty::base_class;
			};
			template<class _Ty>
			struct adjusted_tuple_cat_argument_type_helper2 {
				typedef mse::impl::remove_reference_t<_Ty> NoRefTy;
				using type = typename adjusted_tuple_cat_argument_type_helper1<NoRefTy,
						mse::impl::is_instantiation_of<NoRefTy, mse::mstd::tuple>::value
						|| mse::impl::is_instantiation_of<NoRefTy, mse::xscope_tuple>::value
					>::type;
			};

			template<class _Ty>
			static auto adjusted_tuple_cat_argument(_Ty&& x)
				-> decltype(mse::us::impl::as_ref<typename adjusted_tuple_cat_argument_type_helper2<_Ty>::type>(MSE_FWD(x)))
			{
				return mse::us::impl::as_ref<typename adjusted_tuple_cat_argument_type_helper2<_Ty>::type>(MSE_FWD(x));
			}
		}
	}

	namespace mstd {

		namespace impl {
			namespace tuple {
				template<class _Ty>
				void s_invoke_T_valid_if_not_an_xscope_type_on_each_type() {
					mse::impl::T_valid_if_not_an_xscope_type<_Ty>();
				}
				template<class _Ty, class _Ty2, class... _Args>
				void s_invoke_T_valid_if_not_an_xscope_type_on_each_type() {
					mse::impl::T_valid_if_not_an_xscope_type<_Ty>();
					s_invoke_T_valid_if_not_an_xscope_type_on_each_type<_Ty2, _Args...>();
				}
			}
		}


		template<class... _Types>
		class tuple;

		template<>
		class tuple<> : public std::tuple<> {
		public:
			typedef std::tuple<> base_class;

			//MSE_USING(tuple, base_class);
			using base_class::base_class;
			tuple(const base_class& src) : base_class(src) {}
			tuple(base_class&& src) : base_class(MSE_FWD(src)) {}

			//using base_class::operator=;

			void async_shareable_tag() const {}
			void async_passable_tag() const {}

		private:
			MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;
		};

		template<class _This, class... _Rest>
		class tuple<_This, _Rest...> : public std::tuple<_This, _Rest...> {
		public:
			typedef std::tuple<_This, _Rest...> base_class;

			//MSE_USING(tuple, base_class);
			using base_class::base_class;
			tuple(const base_class& src) : base_class(src) {}
			tuple(base_class&& src) : base_class(MSE_FWD(src)) {}

			MSE_IMPL_DESTRUCTOR_PREFIX1 ~tuple() {
				mse::mstd::impl::tuple::s_invoke_T_valid_if_not_an_xscope_type_on_each_type<_This, _Rest...>();
			}

			//using base_class::operator=;

			template<class _This2 = _This, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_This2, _This>::value)
				&& (mse::impl::is_marked_as_shareable_msemsearray<_This2>::value)
				&& (mse::impl::conjunction<mse::impl::is_marked_as_shareable_msemsearray<_Rest>...>::value)> MSE_IMPL_EIS >
			void async_shareable_tag() const {}
			template<class _This2 = _This, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_This2, _This>::value)
				&& (mse::impl::is_marked_as_passable_msemsearray<_This2>::value)
				&& (mse::impl::conjunction<mse::impl::is_marked_as_passable_msemsearray<_Rest>...>::value)> MSE_IMPL_EIS >
			void async_passable_tag() const {}

		private:
			MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;
		};

#ifdef MSE_HAS_CXX17
		template<class... _Types> tuple(_Types...)->tuple<_Types...>;
		template<class _Ty1, class _Ty2> tuple(std::pair<_Ty1, _Ty2>)->tuple<_Ty1, _Ty2>;
		template<class _Alloc, class... _Types> tuple(std::allocator_arg_t, _Alloc, _Types...)->tuple<_Types...>;
		template<class _Alloc, class _Ty1, class _Ty2> tuple(std::allocator_arg_t, _Alloc, std::pair<_Ty1, _Ty2>)->tuple<_Ty1, _Ty2>;
		template<class _Alloc, class... _Types> tuple(std::allocator_arg_t, _Alloc, tuple<_Types...>)->tuple<_Types...>;
#endif /* MSE_HAS_CXX17 */

		template<class... _Types>
		constexpr tuple<typename std::decay<_Types>::type...> make_tuple(_Types&&... _Args) {
			typedef tuple<mse::impl::_Unrefwrap_t<_Types>...> _Ttype;
			return (_Ttype(std::forward<_Types>(_Args)...));
		}

		namespace impl {
			namespace tuple {
				template<class... _Types>
				constexpr auto make_mstdtuple_from_stdtuple(std::tuple<_Types...>&& stdtuple) -> mse::mstd::tuple<_Types...> {
					return MSE_FWD(stdtuple);
				}
			}
		}

		template<class... _Tuples>
		_NODISCARD constexpr auto tuple_cat(_Tuples&&... _Tpls) {
			return impl::tuple::make_mstdtuple_from_stdtuple(std::tuple_cat(mse::impl::tuple::adjusted_tuple_cat_argument(std::forward<_Tuples>(_Tpls))...));
		}
	}

	template<class... _Tuples>
	_NODISCARD constexpr auto tuple_cat(_Tuples&&... _Tpls) {
		return mse::mstd::tuple_cat(std::forward<_Tuples>(_Tpls)...);
	}

#ifndef MSE_TUPLE_NO_XSCOPE_DEPENDENCE

	template<class... _Types>
	class xscope_tuple;

	template<>
	class xscope_tuple<> : public std::tuple<>, public mse::us::impl::XScopeTagBase {
	public:
		typedef std::tuple<> base_class;

		//MSE_USING(xscope_tuple, base_class);
		using base_class::base_class;
		xscope_tuple(const base_class& src) : base_class(src) {}
		xscope_tuple(base_class&& src) : base_class(MSE_FWD(src)) {}

		//using base_class::operator=;

		void async_xscope_shareable_tag() const {}
		void async_xscope_passable_tag() const {}

	private:
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
	};

	template <class _This, class... _Rest>
	class xscope_tuple<_This, _Rest...> : public std::tuple<_This, _Rest...>, public mse::us::impl::XScopeTagBase
		, public std::conditional<
				(mse::impl::is_referenceable_by_scope_pointer<_This>::value)
				|| (mse::impl::disjunction<mse::impl::is_referenceable_by_scope_pointer<_Rest>...>::value)
			, mse::us::impl::ReferenceableByScopePointerTagBase, mse::impl::TPlaceHolder<mse::us::impl::ReferenceableByScopePointerTagBase, xscope_tuple<_This, _Rest...> > >::type
		, public std::conditional<
				(mse::impl::contains_non_owning_scope_reference<_This>::value)
				|| (mse::impl::disjunction<mse::impl::contains_non_owning_scope_reference<_Rest>...>::value)
			, mse::us::impl::ContainsNonOwningScopeReferenceTagBase, mse::impl::TPlaceHolder<mse::us::impl::ContainsNonOwningScopeReferenceTagBase, xscope_tuple<_This, _Rest...> > >::type
	{
	public:
		typedef std::tuple<_This, _Rest...> base_class;

		//MSE_USING(xscope_tuple, base_class);
		using base_class::base_class;
		xscope_tuple(const base_class& src) : base_class(src) {}
		xscope_tuple(base_class&& src) : base_class(MSE_FWD(src)) {}

		template<class _This2 = _This, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_This2, _This>::value)
			&& (mse::impl::is_marked_as_xscope_shareable_msemsearray<_This2>::value)
			&& (mse::impl::conjunction<mse::impl::is_marked_as_xscope_shareable_msemsearray<_Rest>...>::value)> MSE_IMPL_EIS >
		void async_xscope_shareable_tag() const {}
		template<class _This2 = _This, MSE_IMPL_EIP mse::impl::enable_if_t<(std::is_same<_This2, _This>::value)
			&& (mse::impl::is_marked_as_xscope_passable_msemsearray<_This2>::value)
			&& (mse::impl::conjunction<mse::impl::is_marked_as_xscope_passable_msemsearray<_Rest>...>::value)> MSE_IMPL_EIS >
		void async_xscope_passable_tag() const {}

	private:
		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
	};

#ifdef MSE_HAS_CXX17
	/* deduction guides */
	template<class... _Types> xscope_tuple(_Types...)->xscope_tuple<_Types...>;
	template<class _Ty1, class _Ty2> xscope_tuple(std::pair<_Ty1, _Ty2>)->xscope_tuple<_Ty1, _Ty2>;
	template<class _Alloc, class... _Types> xscope_tuple(std::allocator_arg_t, _Alloc, _Types...)->xscope_tuple<_Types...>;
	template<class _Alloc, class _Ty1, class _Ty2> xscope_tuple(std::allocator_arg_t, _Alloc, std::pair<_Ty1, _Ty2>)->xscope_tuple<_Ty1, _Ty2>;
	template<class _Alloc, class... _Types> xscope_tuple(std::allocator_arg_t, _Alloc, xscope_tuple<_Types...>)->xscope_tuple<_Types...>;
#endif /* MSE_HAS_CXX17 */

	template<class... _Types>
	constexpr xscope_tuple<typename std::decay<_Types>::type...> make_xscope_tuple(_Types&&... _Args) {
		typedef xscope_tuple<mse::impl::_Unrefwrap_t<_Types>...> _Ttype;
		return (_Ttype(std::forward<_Types>(_Args)...));
	}

	namespace impl {
		namespace tuple {
			template<class... _Types>
			constexpr auto make_xscopetuple_from_stdtuple(std::tuple<_Types...>&& stdtuple) -> mse::xscope_tuple<_Types...> {
				return MSE_FWD(stdtuple);
			}
		}
	}

	template<class... _Tuples>
	_NODISCARD constexpr auto xscope_tuple_cat(_Tuples&&... _Tpls) {
		return impl::tuple::make_xscopetuple_from_stdtuple(std::tuple_cat(mse::impl::tuple::adjusted_tuple_cat_argument(std::forward<_Tuples>(_Tpls))...));
	}
#endif // !MSE_TUPLE_NO_XSCOPE_DEPENDENCE
}

namespace std {
	template<size_t _Index, class... _Types>
	_NODISCARD constexpr tuple_element_t<_Index, tuple<_Types...>>& get(mse::mstd::tuple<_Types...>& _Tuple) noexcept {
		return get<_Index>(static_cast<typename mse::mstd::tuple<_Types...>::base_class&>(_Tuple));
	}

	template<size_t _Index, class... _Types>
	_NODISCARD constexpr const tuple_element_t<_Index, mse::mstd::tuple<_Types...>>& get(const mse::mstd::tuple<_Types...>& _Tuple) noexcept {
		return get<_Index>(static_cast<const typename mse::mstd::tuple<_Types...>::base_class&>(_Tuple));
	}

	template<size_t _Index, class... _Types>
	_NODISCARD constexpr tuple_element_t<_Index, mse::mstd::tuple<_Types...>>&& get(mse::mstd::tuple<_Types...>&& _Tuple) noexcept {
		return get<_Index>(std::forward<typename mse::mstd::tuple<_Types...>::base_class>(_Tuple));
	}

	template<size_t _Index, class... _Types>
	_NODISCARD constexpr const tuple_element_t<_Index, mse::mstd::tuple<_Types...>>&& get(const mse::mstd::tuple<_Types...>&& _Tuple) noexcept {
		return get<_Index>(std::forward<const typename mse::mstd::tuple<_Types...>::base_class>(_Tuple));
	}

	template<class _Ty, class... _Types>
	_NODISCARD constexpr _Ty& get(mse::mstd::tuple<_Types...>& _Tuple) noexcept {
		return get<_Ty>(static_cast<typename mse::mstd::tuple<_Types...>::base_class&>(_Tuple));
	}

	template<class _Ty, class... _Types>
	_NODISCARD constexpr const _Ty& get(const mse::mstd::tuple<_Types...>& _Tuple) noexcept {
		return get<_Ty>(static_cast<const typename mse::mstd::tuple<_Types...>::base_class&>(_Tuple));
	}

	template<class _Ty, class... _Types>
	_NODISCARD constexpr _Ty&& get(mse::mstd::tuple<_Types...>&& _Tuple) noexcept {
		return get<_Ty>(std::forward<typename mse::mstd::tuple<_Types...>::base_class>(_Tuple));
	}

	template<class _Ty, class... _Types>
	_NODISCARD constexpr const _Ty&& get(const mse::mstd::tuple<_Types...>&& _Tuple) noexcept {
		return get<_Ty>(std::forward<const typename mse::mstd::tuple<_Types...>::base_class>(_Tuple));
	}

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmismatched-tags"
#endif /*__clang__*/

	template<class... _Types>
	struct tuple_size<mse::mstd::tuple<_Types...>>
		: integral_constant<size_t, sizeof...(_Types)>
	{	// size of tuple
	};

	template<size_t _Index>
	struct tuple_element<_Index, mse::mstd::tuple<>>
	{	// enforce bounds checking
		static_assert(mse::impl::_Always_false<integral_constant<size_t, _Index>>,
			"tuple index out of bounds");
	};

	template<class _This,
		class... _Rest>
		struct tuple_element<0, mse::mstd::tuple<_This, _Rest...>>
	{	// select first element
		using type = _This;
		using _Ttype = mse::mstd::tuple<_This, _Rest...>;
	};

	template<size_t _Index,
		class _This,
		class... _Rest>
		struct tuple_element<_Index, mse::mstd::tuple<_This, _Rest...>>
		: public tuple_element<_Index - 1, mse::mstd::tuple<_Rest...>>
	{	// recursive tuple_element definition
	};

#ifdef __clang__
#pragma clang diagnostic pop
#endif /*__clang__*/


	template<size_t _Index, class... _Types>
	_NODISCARD constexpr tuple_element_t<_Index, tuple<_Types...>>& get(mse::xscope_tuple<_Types...>& _Tuple) noexcept {
		return get<_Index>(static_cast<typename mse::xscope_tuple<_Types...>::base_class&>(_Tuple));
	}

	template<size_t _Index, class... _Types>
	_NODISCARD constexpr const tuple_element_t<_Index, mse::xscope_tuple<_Types...>>& get(const mse::xscope_tuple<_Types...>& _Tuple) noexcept {
		return get<_Index>(static_cast<const typename mse::xscope_tuple<_Types...>::base_class&>(_Tuple));
	}

	template<size_t _Index, class... _Types>
	_NODISCARD constexpr tuple_element_t<_Index, mse::xscope_tuple<_Types...>>&& get(mse::xscope_tuple<_Types...>&& _Tuple) noexcept {
		return get<_Index>(std::forward<typename mse::xscope_tuple<_Types...>::base_class>(_Tuple));
	}

	template<size_t _Index, class... _Types>
	_NODISCARD constexpr const tuple_element_t<_Index, mse::xscope_tuple<_Types...>>&& get(const mse::xscope_tuple<_Types...>&& _Tuple) noexcept {
		return get<_Index>(std::forward<const typename mse::xscope_tuple<_Types...>::base_class>(_Tuple));
	}

	template<class _Ty, class... _Types>
	_NODISCARD constexpr _Ty& get(mse::xscope_tuple<_Types...>& _Tuple) noexcept {
		return get<_Ty>(static_cast<typename mse::xscope_tuple<_Types...>::base_class&>(_Tuple));
	}

	template<class _Ty, class... _Types>
	_NODISCARD constexpr const _Ty& get(const mse::xscope_tuple<_Types...>& _Tuple) noexcept {
		return get<_Ty>(static_cast<const typename mse::xscope_tuple<_Types...>::base_class&>(_Tuple));
	}

	template<class _Ty, class... _Types>
	_NODISCARD constexpr _Ty&& get(mse::xscope_tuple<_Types...>&& _Tuple) noexcept {
		return get<_Ty>(std::forward<typename mse::xscope_tuple<_Types...>::base_class>(_Tuple));
	}

	template<class _Ty, class... _Types>
	_NODISCARD constexpr const _Ty&& get(const mse::xscope_tuple<_Types...>&& _Tuple) noexcept {
		return get<_Ty>(std::forward<const typename mse::xscope_tuple<_Types...>::base_class>(_Tuple));
	}

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmismatched-tags"
#endif /*__clang__*/

	template<class... _Types>
	struct tuple_size<mse::xscope_tuple<_Types...>>
		: integral_constant<size_t, sizeof...(_Types)>
	{	// size of tuple
	};

	template<size_t _Index>
	struct tuple_element<_Index, mse::xscope_tuple<>>
	{	// enforce bounds checking
		static_assert(mse::impl::_Always_false<integral_constant<size_t, _Index>>,
			"tuple index out of bounds");
	};

	template<class _This,
		class... _Rest>
		struct tuple_element<0, mse::xscope_tuple<_This, _Rest...>>
	{	// select first element
		using type = _This;
		using _Ttype = mse::xscope_tuple<_This, _Rest...>;
	};

	template<size_t _Index,
		class _This,
		class... _Rest>
		struct tuple_element<_Index, mse::xscope_tuple<_This, _Rest...>>
		: public tuple_element<_Index - 1, mse::xscope_tuple<_Rest...>>
	{	// recursive tuple_element definition
	};

#ifdef __clang__
#pragma clang diagnostic pop
#endif /*__clang__*/
}
#endif // GPP6_COMPATIBLE

namespace mse {

	namespace impl {
		namespace ns_tuple {
			template<size_t Index, typename TPointerToTuple>
			struct ByIndexTypeInfoFromPointerToTuple1 {
				typedef mse::impl::remove_reference_t<decltype(std::get<Index>(*std::declval<TPointerToTuple>()))> value_t;
			};
		}
	}

	template<size_t Index, typename TXScopeTuplePointer>
	class TXScopeTupleElementByIndexFixedConstPointer;

	template<size_t Index, typename TXScopeTuplePointer>
	class TXScopeTupleElementByIndexFixedPointer : public mse::us::TXScopeStrongFixedPointer<typename impl::ns_tuple::ByIndexTypeInfoFromPointerToTuple1<Index, TXScopeTuplePointer>::value_t, TXScopeTuplePointer> {
	public:
		typedef mse::us::TXScopeStrongFixedPointer<typename impl::ns_tuple::ByIndexTypeInfoFromPointerToTuple1<Index, TXScopeTuplePointer>::value_t, TXScopeTuplePointer> base_class;
		typedef typename impl::ns_tuple::ByIndexTypeInfoFromPointerToTuple1<Index, TXScopeTuplePointer>::value_t value_t;

		TXScopeTupleElementByIndexFixedPointer(const TXScopeTupleElementByIndexFixedPointer&) = default;
		TXScopeTupleElementByIndexFixedPointer(TXScopeTupleElementByIndexFixedPointer&&) = default;

		TXScopeTupleElementByIndexFixedPointer(const TXScopeTuplePointer& lease)
			: base_class(std::get<Index>(*lease), lease) {}
		TXScopeTupleElementByIndexFixedPointer(TXScopeTuplePointer&& lease)
			: base_class(std::get<Index>(*lease), MSE_FWD(lease)) {}

	private:
		TXScopeTupleElementByIndexFixedPointer(const base_class& src_cref) : base_class(src_cref) {}

		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		template<size_t Index2, typename TXScopeTuplePointer2>
		friend class TXScopeTupleElementByIndexFixedConstPointer;
	};

	template<size_t Index, typename TXScopeTuplePointer>
	class TXScopeTupleElementByIndexFixedConstPointer : public mse::us::TXScopeStrongFixedConstPointer<typename impl::ns_tuple::ByIndexTypeInfoFromPointerToTuple1<Index, TXScopeTuplePointer>::value_t, TXScopeTuplePointer> {
	public:
		typedef mse::us::TXScopeStrongFixedConstPointer<typename impl::ns_tuple::ByIndexTypeInfoFromPointerToTuple1<Index, TXScopeTuplePointer>::value_t, TXScopeTuplePointer> base_class;
		typedef typename impl::ns_tuple::ByIndexTypeInfoFromPointerToTuple1<Index, TXScopeTuplePointer>::value_t value_t;

		TXScopeTupleElementByIndexFixedConstPointer(const TXScopeTupleElementByIndexFixedConstPointer&) = default;
		TXScopeTupleElementByIndexFixedConstPointer(TXScopeTupleElementByIndexFixedConstPointer&&) = default;

		template<size_t Index2, typename TXScopeTuplePointer2>
		TXScopeTupleElementByIndexFixedConstPointer(const TXScopeTupleElementByIndexFixedPointer<Index2, TXScopeTuplePointer2>& src) : base_class(src) {}
		template<size_t Index2, typename TXScopeTuplePointer2>
		TXScopeTupleElementByIndexFixedConstPointer(TXScopeTupleElementByIndexFixedPointer<Index2, TXScopeTuplePointer2>&& src) : base_class(MSE_FWD(src)) {}

		TXScopeTupleElementByIndexFixedConstPointer(const TXScopeTuplePointer& lease)
			: base_class(std::get<Index>(*lease), lease) {}
		TXScopeTupleElementByIndexFixedConstPointer(TXScopeTuplePointer&& lease)
			: base_class(std::get<Index>(*lease), MSE_FWD(lease)) {}

	private:
		TXScopeTupleElementByIndexFixedConstPointer(const base_class& src_cref) : base_class(src_cref) {}

		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
	};

	template<size_t Index, typename TXScopeTuplePointer>
	auto xscope_pointer(const TXScopeTupleElementByIndexFixedPointer<Index, TXScopeTuplePointer>& param) {
		return mse::us::unsafe_make_xscope_pointer_to(*param);
	}
	template<size_t Index, typename TXScopeTuplePointer>
	auto xscope_pointer(const TXScopeTupleElementByIndexFixedPointer<Index, TXScopeTuplePointer>&& param) = delete;
	template<size_t Index, typename TXScopeTuplePointer>
	auto xscope_pointer(const TXScopeTupleElementByIndexFixedConstPointer<Index, TXScopeTuplePointer>& param) {
		return mse::us::unsafe_make_xscope_const_pointer_to(*param);
	}
	template<size_t Index, typename TXScopeTuplePointer>
	auto xscope_pointer(const TXScopeTupleElementByIndexFixedConstPointer<Index, TXScopeTuplePointer>&& param) = delete;

	template<size_t Index, typename TXScopeTuplePointer>
	auto make_xscope_tuple_element_pointer(const TXScopeTuplePointer& ptr) -> TXScopeTupleElementByIndexFixedPointer<Index, TXScopeTuplePointer> {
		return TXScopeTupleElementByIndexFixedPointer<Index, TXScopeTuplePointer>(ptr);
	}
	template<size_t Index, typename TXScopeTuplePointer>
	auto make_xscope_tuple_element_pointer(TXScopeTuplePointer&& ptr) -> TXScopeTupleElementByIndexFixedPointer<Index, TXScopeTuplePointer> {
		return TXScopeTupleElementByIndexFixedPointer<Index, TXScopeTuplePointer>(MSE_FWD(ptr));
	}
	template<size_t Index, typename TXScopeTuplePointer>
	auto make_xscope_tuple_element_const_pointer(const TXScopeTuplePointer& ptr) -> TXScopeTupleElementByIndexFixedConstPointer<Index, TXScopeTuplePointer> {
		return TXScopeTupleElementByIndexFixedConstPointer<Index, TXScopeTuplePointer>(ptr);
	}
	template<size_t Index, typename TXScopeTuplePointer>
	auto make_xscope_tuple_element_const_pointer(TXScopeTuplePointer&& ptr) -> TXScopeTupleElementByIndexFixedConstPointer<Index, TXScopeTuplePointer> {
		return TXScopeTupleElementByIndexFixedConstPointer<Index, TXScopeTuplePointer>(MSE_FWD(ptr));
	}


	template<size_t Index, typename TTuplePointer>
	class TTupleElementByIndexFixedConstPointer;

	template<size_t Index, typename TTuplePointer>
	class TTupleElementByIndexFixedPointer : public std::conditional<mse::impl::is_strong_ptr<mse::impl::remove_reference_t<TTuplePointer> >::value
		, mse::us::impl::StrongPointerTagBase, mse::impl::TPlaceHolder<TTupleElementByIndexFixedPointer<Index, TTuplePointer> > >::type {
	public:
		typedef mse::impl::remove_reference_t<TTuplePointer> TTuplePointerRR;
		typedef typename impl::ns_tuple::ByIndexTypeInfoFromPointerToTuple1<Index, TTuplePointerRR>::value_t value_t;

		TTupleElementByIndexFixedPointer(const TTupleElementByIndexFixedPointer&) = default;
		TTupleElementByIndexFixedPointer(TTupleElementByIndexFixedPointer&&) = default;

		TTupleElementByIndexFixedPointer(const TTuplePointerRR& src) : m_tuple_ptr(src) {}
		TTupleElementByIndexFixedPointer(TTuplePointerRR&& src) : m_tuple_ptr(MSE_FWD(src)) {}

		~TTupleElementByIndexFixedPointer() {
#ifndef MSE_OPTIONAL_NO_XSCOPE_DEPENDENCE
			mse::impl::T_valid_if_not_an_xscope_type<TTuplePointerRR>();
#endif // !MSE_OPTIONAL_NO_XSCOPE_DEPENDENCE
		}

		value_t& operator*() const {
			return std::get<Index>(*m_tuple_ptr);
		}
		value_t* operator->() const {
			return std::addressof(std::get<Index>(*m_tuple_ptr));
		}

		bool operator==(const value_t* _Right_cref) const { return (_Right_cref == std::addressof(*(*this))); }
		bool operator!=(const value_t* _Right_cref) const { return (!((*this) == _Right_cref)); }
		bool operator==(const TTupleElementByIndexFixedPointer &_Right_cref) const { return ((*this) == std::addressof(*_Right_cref)); }
		bool operator!=(const TTupleElementByIndexFixedPointer &_Right_cref) const { return (!((*this) == _Right_cref)); }
		operator bool() const { return true; }

	private:
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		TTuplePointerRR m_tuple_ptr;

		template<size_t Index2, typename TTuplePointer2>
		friend class TTupleElementByIndexFixedConstPointer;
	};

	template<size_t Index, typename TTuplePointer>
	class TTupleElementByIndexFixedConstPointer : public std::conditional<mse::impl::is_strong_ptr<mse::impl::remove_reference_t<TTuplePointer> >::value
		, mse::us::impl::StrongPointerTagBase, mse::impl::TPlaceHolder<TTupleElementByIndexFixedConstPointer<Index, TTuplePointer> > >::type {
	public:
		typedef mse::impl::remove_reference_t<TTuplePointer> TTuplePointerRR;
		typedef typename impl::ns_tuple::ByIndexTypeInfoFromPointerToTuple1<Index, TTuplePointerRR>::value_t value_t;

		TTupleElementByIndexFixedConstPointer(const TTupleElementByIndexFixedConstPointer&) = default;
		TTupleElementByIndexFixedConstPointer(TTupleElementByIndexFixedConstPointer&&) = default;

		template<size_t Index2, typename TTuplePointerRR2>
		TTupleElementByIndexFixedConstPointer(const TTupleElementByIndexFixedPointer<Index2, TTuplePointerRR2>& src) : m_tuple_ptr(src.m_tuple_ptr) {}
		template<size_t Index2, typename TTuplePointerRR2>
		TTupleElementByIndexFixedConstPointer(TTupleElementByIndexFixedPointer<Index2, TTuplePointerRR2>&& src) : m_tuple_ptr(MSE_FWD(src).m_tuple_ptr) {}

		TTupleElementByIndexFixedConstPointer(const TTuplePointerRR& src) : m_tuple_ptr(src) {}
		TTupleElementByIndexFixedConstPointer(TTuplePointerRR&& src) : m_tuple_ptr(std::forward<src>(src)) {}

		~TTupleElementByIndexFixedConstPointer() {
#ifndef MSE_OPTIONAL_NO_XSCOPE_DEPENDENCE
			mse::impl::T_valid_if_not_an_xscope_type<TTuplePointerRR>();
#endif // !MSE_OPTIONAL_NO_XSCOPE_DEPENDENCE
		}

		const value_t& operator*() const {
			return std::get<Index>(*m_tuple_ptr);
		}
		const value_t* operator->() const {
			return std::addressof(std::get<Index>(*m_tuple_ptr));
		}

		bool operator==(const value_t* _Right_cref) const { return (_Right_cref == std::addressof(*(*this))); }
		bool operator!=(const value_t* _Right_cref) const { return (!((*this) == _Right_cref)); }
		bool operator==(const TTupleElementByIndexFixedConstPointer &_Right_cref) const { return ((*this) == std::addressof(*_Right_cref)); }
		bool operator!=(const TTupleElementByIndexFixedConstPointer &_Right_cref) const { return (!((*this) == _Right_cref)); }
		operator bool() const { return true; }

	private:
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		TTuplePointerRR m_tuple_ptr;
	};

	template<size_t Index, typename TTuplePointer>
	auto make_tuple_element_pointer(const TTuplePointer& ptr) {
		return TTupleElementByIndexFixedPointer<Index, TTuplePointer>(ptr);
	}
	template<size_t Index, typename TTuplePointer>
	auto make_tuple_element_pointer(TTuplePointer&& ptr) {
		return TTupleElementByIndexFixedPointer<Index, TTuplePointer>(MSE_FWD(ptr));
	}
	template<size_t Index, typename TTuplePointer>
	auto make_tuple_element_const_pointer(const TTuplePointer& ptr) {
		return TTupleElementByIndexFixedConstPointer<Index, TTuplePointer>(ptr);
	}
	template<size_t Index, typename TTuplePointer>
	auto make_tuple_element_const_pointer(TTuplePointer&& ptr) {
		return TTupleElementByIndexFixedConstPointer<Index, TTuplePointer>(MSE_FWD(ptr));
	}


	namespace impl {
		namespace ns_tuple {
			template<class TIndex, typename TPointerToTuple>
			struct TypeInfoFromPointerToTuple1 {
				typedef mse::impl::remove_reference_t<decltype(std::get<TIndex>(*std::declval<TPointerToTuple>()))> value_t;
			};
		}
	}

	template<class TIndex, typename TXScopeTuplePointer>
	class TXScopeTupleElementFixedConstPointer;

	template<class TIndex, typename TXScopeTuplePointer>
	class TXScopeTupleElementFixedPointer : public mse::us::TXScopeStrongFixedPointer<typename impl::ns_tuple::TypeInfoFromPointerToTuple1<TIndex, TXScopeTuplePointer>::value_t, TXScopeTuplePointer> {
	public:
		typedef mse::us::TXScopeStrongFixedPointer<typename impl::ns_tuple::TypeInfoFromPointerToTuple1<TIndex, TXScopeTuplePointer>::value_t, TXScopeTuplePointer> base_class;
		typedef typename impl::ns_tuple::TypeInfoFromPointerToTuple1<TIndex, TXScopeTuplePointer>::value_t value_t;

		TXScopeTupleElementFixedPointer(const TXScopeTupleElementFixedPointer&) = default;
		TXScopeTupleElementFixedPointer(TXScopeTupleElementFixedPointer&&) = default;

		TXScopeTupleElementFixedPointer(const TXScopeTuplePointer& lease)
			: base_class(std::get<TIndex>(*lease), lease) {}
		TXScopeTupleElementFixedPointer(TXScopeTuplePointer&& lease)
			: base_class(std::get<TIndex>(*lease), MSE_FWD(lease)) {}

	private:
		TXScopeTupleElementFixedPointer(const base_class& src_cref) : base_class(src_cref) {}

		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;

		template<class TIndex2, typename TXScopeTuplePointer2>
		friend class TXScopeTupleElementFixedConstPointer;
	};

	template<class TIndex, typename TXScopeTuplePointer>
	class TXScopeTupleElementFixedConstPointer : public mse::us::TXScopeStrongFixedConstPointer<typename impl::ns_tuple::TypeInfoFromPointerToTuple1<TIndex, TXScopeTuplePointer>::value_t, TXScopeTuplePointer> {
	public:
		typedef mse::us::TXScopeStrongFixedConstPointer<typename impl::ns_tuple::TypeInfoFromPointerToTuple1<TIndex, TXScopeTuplePointer>::value_t, TXScopeTuplePointer> base_class;
		typedef typename impl::ns_tuple::TypeInfoFromPointerToTuple1<TIndex, TXScopeTuplePointer>::value_t value_t;

		TXScopeTupleElementFixedConstPointer(const TXScopeTupleElementFixedConstPointer&) = default;
		TXScopeTupleElementFixedConstPointer(TXScopeTupleElementFixedConstPointer&&) = default;

		template<class TIndex2, typename TXScopeTuplePointer2>
		TXScopeTupleElementFixedConstPointer(const TXScopeTupleElementFixedPointer<TIndex2, TXScopeTuplePointer2>& src) : base_class(src) {}
		template<class TIndex2, typename TXScopeTuplePointer2>
		TXScopeTupleElementFixedConstPointer(TXScopeTupleElementFixedPointer<TIndex2, TXScopeTuplePointer2>&& src) : base_class(MSE_FWD(src)) {}

		TXScopeTupleElementFixedConstPointer(const TXScopeTuplePointer& lease)
			: base_class(std::get<TIndex>(*lease), lease) {}
		TXScopeTupleElementFixedConstPointer(TXScopeTuplePointer&& lease)
			: base_class(std::get<TIndex>(*lease), MSE_FWD(lease)) {}

	private:
		TXScopeTupleElementFixedConstPointer(const base_class& src_cref) : base_class(src_cref) {}

		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
	};

	template<class TIndex, typename TXScopeTuplePointer>
	auto xscope_pointer(const TXScopeTupleElementFixedPointer<TIndex, TXScopeTuplePointer>& param) {
		return mse::us::unsafe_make_xscope_pointer_to(*param);
	}
	template<class TIndex, typename TXScopeTuplePointer>
	auto xscope_pointer(const TXScopeTupleElementFixedPointer<TIndex, TXScopeTuplePointer>&& param) = delete;
	template<class TIndex, typename TXScopeTuplePointer>
	auto xscope_pointer(const TXScopeTupleElementFixedConstPointer<TIndex, TXScopeTuplePointer>& param) {
		return mse::us::unsafe_make_xscope_const_pointer_to(*param);
	}
	template<class TIndex, typename TXScopeTuplePointer>
	auto xscope_pointer(const TXScopeTupleElementFixedConstPointer<TIndex, TXScopeTuplePointer>&& param) = delete;

	template<class TIndex, typename TXScopeTuplePointer>
	auto make_xscope_tuple_element_pointer(const TXScopeTuplePointer& ptr) -> TXScopeTupleElementFixedPointer<TIndex, TXScopeTuplePointer> {
		return TXScopeTupleElementFixedPointer<TIndex, TXScopeTuplePointer>(ptr);
	}
	template<class TIndex, typename TXScopeTuplePointer, class = MSE_IMPL_ENABLE_IF_NOT_RETURNABLE_FPARAM(TXScopeTuplePointer)>
	auto make_xscope_tuple_element_pointer(TXScopeTuplePointer&& ptr) -> TXScopeTupleElementFixedPointer<TIndex, TXScopeTuplePointer> {
		return TXScopeTupleElementFixedPointer<TIndex, TXScopeTuplePointer>(MSE_FWD(ptr));
	}
	template<class TIndex, typename TXScopeTuplePointer>
	auto make_xscope_tuple_element_const_pointer(const TXScopeTuplePointer& ptr) -> TXScopeTupleElementFixedConstPointer<TIndex, TXScopeTuplePointer> {
		return TXScopeTupleElementFixedConstPointer<TIndex, TXScopeTuplePointer>(ptr);
	}
	template<class TIndex, typename TXScopeTuplePointer, class = MSE_IMPL_ENABLE_IF_NOT_RETURNABLE_FPARAM(TXScopeTuplePointer)>
	auto make_xscope_tuple_element_const_pointer(TXScopeTuplePointer&& ptr) -> TXScopeTupleElementFixedConstPointer<TIndex, TXScopeTuplePointer> {
		return TXScopeTupleElementFixedConstPointer<TIndex, TXScopeTuplePointer>(MSE_FWD(ptr));
	}
	/* Overloads for rsv::TReturnableFParam<>. */
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_tuple_element_pointer)
	MSE_OVERLOAD_FOR_RETURNABLE_FPARAM_DECLARATION(make_xscope_tuple_element_const_pointer)


	template<class TIndex, typename TTuplePointer>
	class TTupleElementFixedConstPointer;

	template<class TIndex, typename TTuplePointer>
	class TTupleElementFixedPointer : public std::conditional<mse::impl::is_strong_ptr<mse::impl::remove_reference_t<TTuplePointer> >::value
		, mse::us::impl::StrongPointerTagBase, mse::impl::TPlaceHolder<TTupleElementFixedPointer<TIndex, TTuplePointer> > >::type {
	public:
		typedef mse::impl::remove_reference_t<TTuplePointer> TTuplePointerRR;
		typedef typename impl::ns_tuple::TypeInfoFromPointerToTuple1<TIndex, TTuplePointerRR>::value_t value_t;

		TTupleElementFixedPointer(const TTupleElementFixedPointer&) = default;
		TTupleElementFixedPointer(TTupleElementFixedPointer&&) = default;

		TTupleElementFixedPointer(const TTuplePointerRR& src) : m_tuple_ptr(src) {}
		TTupleElementFixedPointer(TTuplePointerRR&& src) : m_tuple_ptr(MSE_FWD(src)) {}

		~TTupleElementFixedPointer() {
#ifndef MSE_OPTIONAL_NO_XSCOPE_DEPENDENCE
			mse::impl::T_valid_if_not_an_xscope_type<TTuplePointerRR>();
#endif // !MSE_OPTIONAL_NO_XSCOPE_DEPENDENCE
		}

		value_t& operator*() const {
			return std::get<TIndex>(*m_tuple_ptr);
		}
		value_t* operator->() const {
			return std::addressof(std::get<TIndex>(*m_tuple_ptr));
		}

		bool operator==(const value_t* _Right_cref) const { return (_Right_cref == std::addressof(*(*this))); }
		bool operator!=(const value_t* _Right_cref) const { return (!((*this) == _Right_cref)); }
		bool operator==(const TTupleElementFixedPointer &_Right_cref) const { return ((*this) == std::addressof(*_Right_cref)); }
		bool operator!=(const TTupleElementFixedPointer &_Right_cref) const { return (!((*this) == _Right_cref)); }
		operator bool() const { return true; }

	private:
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		TTuplePointerRR m_tuple_ptr;

		template<class TIndex2, typename TTuplePointer2>
		friend class TTupleElementFixedConstPointer;
	};

	template<class TIndex, typename TTuplePointer>
	class TTupleElementFixedConstPointer : public std::conditional<mse::impl::is_strong_ptr<mse::impl::remove_reference_t<TTuplePointer> >::value
		, mse::us::impl::StrongPointerTagBase, mse::impl::TPlaceHolder<TTupleElementFixedConstPointer<TIndex, TTuplePointer> > >::type {
	public:
		typedef mse::impl::remove_reference_t<TTuplePointer> TTuplePointerRR;
		typedef typename impl::ns_tuple::TypeInfoFromPointerToTuple1<TIndex, TTuplePointerRR>::value_t value_t;

		TTupleElementFixedConstPointer(const TTupleElementFixedConstPointer&) = default;
		TTupleElementFixedConstPointer(TTupleElementFixedConstPointer&&) = default;

		template<class TIndex2, typename TTuplePointer2>
		TTupleElementFixedConstPointer(const TTupleElementFixedPointer<TIndex2, TTuplePointer2>& src) : m_tuple_ptr(src.m_tuple_ptr) {}
		template<class TIndex2, typename TTuplePointer2>
		TTupleElementFixedConstPointer(TTupleElementFixedPointer<TIndex2, TTuplePointer2>&& src) : m_tuple_ptr(MSE_FWD(src).m_tuple_ptr) {}

		TTupleElementFixedConstPointer(const TTuplePointerRR& src) : m_tuple_ptr(src) {}
		TTupleElementFixedConstPointer(TTuplePointerRR&& src) : m_tuple_ptr(std::forward<src>(src)) {}

		~TTupleElementFixedConstPointer() {
#ifndef MSE_OPTIONAL_NO_XSCOPE_DEPENDENCE
			mse::impl::T_valid_if_not_an_xscope_type<TIndex, TTuplePointerRR>();
#endif // !MSE_OPTIONAL_NO_XSCOPE_DEPENDENCE
		}

		const value_t& operator*() const {
			return std::get<TIndex>(*m_tuple_ptr);
		}
		const value_t* operator->() const {
			return std::addressof(std::get<TIndex>(*m_tuple_ptr));
		}

		bool operator==(const value_t* _Right_cref) const { return (_Right_cref == std::addressof(*(*this))); }
		bool operator!=(const value_t* _Right_cref) const { return (!((*this) == _Right_cref)); }
		bool operator==(const TTupleElementFixedConstPointer &_Right_cref) const { return ((*this) == std::addressof(*_Right_cref)); }
		bool operator!=(const TTupleElementFixedConstPointer &_Right_cref) const { return (!((*this) == _Right_cref)); }
		operator bool() const { return true; }

	private:
		MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;

		TTuplePointerRR m_tuple_ptr;
	};

	template<class TIndex, typename TTuplePointer>
	auto make_tuple_element_pointer(const TTuplePointer& ptr) {
		return TTupleElementFixedPointer<TIndex, TTuplePointer>(ptr);
	}
	template<class TIndex, typename TTuplePointer>
	auto make_tuple_element_pointer(TTuplePointer&& ptr) {
		return TTupleElementFixedPointer<TIndex, TTuplePointer>(MSE_FWD(ptr));
	}
	template<class TIndex, typename TTuplePointer>
	auto make_tuple_element_const_pointer(const TTuplePointer& ptr) {
		return TTupleElementFixedConstPointer<TIndex, TTuplePointer>(ptr);
	}
	template<class TIndex, typename TTuplePointer>
	auto make_tuple_element_const_pointer(TTuplePointer&& ptr) {
		return TTupleElementFixedConstPointer<TIndex, TTuplePointer>(MSE_FWD(ptr));
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
		class CTupleTest1 {
		public:
			template<class Tuple, std::size_t N>
			struct TuplePrinter {
				static void print(const Tuple& t)
				{
					TuplePrinter<Tuple, N - 1>::print(t);
					std::cout << ", " << std::get<N - 1>(t);
				}
			};

			template<class Tuple>
			struct TuplePrinter<Tuple, 1> {
				static void print(const Tuple& t)
				{
					std::cout << std::get<0>(t);
				}
			};
			// helper function to print a tuple of any size
			template<class... Args>
			static void print(const std::tuple<Args...>& t)
			{
				std::cout << "(";
				TuplePrinter<decltype(t), sizeof...(Args)>::print(t);
				std::cout << ")\n";
			}
			// end helper function

			static void s_test1() {
#ifdef MSE_SELF_TESTS
				std::cout << "\nmstd::tuple<> tests: \n";
				{
					/* example from https://en.cppreference.com/w/cpp/utility/tuple */
					struct CB {
						static mse::mstd::tuple<double, char, std::string> get_student(int id)
						{
							if (id == 0) return mse::mstd::make_tuple(3.8, 'A', std::string("Lisa Simpson"));
							if (id == 1) return mse::mstd::make_tuple(2.9, 'C', std::string("Milhouse Van Houten"));
							if (id == 2) return mse::mstd::make_tuple(1.7, 'D', std::string("Ralph Wiggum"));
#if __cpp_exceptions >= 199711
							throw std::invalid_argument("id");
#else // __cpp_exceptions >= 199711
							return mse::mstd::make_tuple(0.0, 'Z', std::string("invalid"));
#endif // __cpp_exceptions >= 199711
						}
					};

					{
						auto student0 = CB::get_student(0);
						std::cout << "ID: 0, "
							<< "GPA: " << std::get<0>(student0) << ", "
							<< "grade: " << std::get<1>(student0) << ", "
							<< "name: " << std::get<2>(student0) << '\n';

						double gpa1;
						char grade1;
						std::string name1;
						std::tie(gpa1, grade1, name1) = CB::get_student(1);
						std::cout << "ID: 1, "
							<< "GPA: " << gpa1 << ", "
							<< "grade: " << grade1 << ", "
							<< "name: " << name1 << '\n';

#ifdef MSE_HAS_CXX17
						// C++17 structured binding:
						auto[gpa2, grade2, name2] = CB::get_student(2);
						std::cout << "ID: 2, "
							<< "GPA: " << gpa2 << ", "
							<< "grade: " << grade2 << ", "
							<< "name: " << name2 << '\n';
#endif // MSE_HAS_CXX17
					}
				}
				{
					/* example from https://en.cppreference.com/w/cpp/utility/tuple/tuple */
					{
						mse::mstd::tuple<int, std::string, double> t1;
						std::cout << "Value-initialized: "; print(t1);
						mse::mstd::tuple<int, std::string, double> t2(42, "Test", -3.14);
						std::cout << "Initialized with values: "; print(t2);
						//mse::mstd::tuple<char, std::string, int> t3(t2);
						//std::cout << "Implicitly converted: "; print(t3);
						mse::mstd::tuple<int, double> t4(std::make_pair(42, 3.14));
						std::cout << "Constructed from a pair"; print(t4);
					}
				}
				{
					/* example from https://en.cppreference.com/w/cpp/utility/tuple/swap */
					mse::mstd::tuple<int, std::string, double> p1, p2;
					p1 = mse::mstd::make_tuple(10, std::string("test"), 3.14);
					p2.swap(p1);
					std::cout << "(" << std::get<0>(p2)
						<< ", " << std::get<1>(p2)
						<< ", " << std::get<2>(p2) << ")\n";
				}
				{
					/* example from https://en.cppreference.com/w/cpp/utility/tuple/swap */
					struct CB {
						static mse::mstd::tuple<int, int> f() // this function returns multiple values
						{
							int x = 5;
							return mse::mstd::make_tuple(x, 7); // return {x,7}; in C++17
						}
					};

					{
						// heterogeneous tuple construction
						int n = 1;
						auto t = mse::mstd::make_tuple(10, std::string("Test"), 3.14, std::ref(n), n);
						n = 7;
						std::cout << "The value of t is " << "("
							<< std::get<0>(t) << ", " << std::get<1>(t) << ", "
							<< std::get<2>(t) << ", " << std::get<3>(t) << ", "
							<< std::get<4>(t) << ")\n";

						// function returning multiple values
						int a, b;
						std::tie(a, b) = CB::f();
						std::cout << a << " " << b << "\n";
					}
				}
				{
					/* example from https://en.cppreference.com/w/cpp/utility/tuple/tuple_cat */
					{
						mse::mstd::tuple<int, std::string, double> t1(10, "Test", 3.14);
						int n = 7;
						auto t2 = mse::mstd::tuple_cat(t1, std::make_pair(std::string("Foo"), std::string("bar")), t1, std::tie(n));
						n = 10;
						print(t2);
					}
				}
				{
					mse::mstd::tuple<int, std::string, double> t1(10, "Test", 3.14);
					mse::mstd::tuple<int, std::string, double> t2(11, "Test2", 4.14);
					mse::mstd::tuple<int, std::string, double> t3(10, "Test", 3.14);
					std::swap(t1, t2);
					bool b1 = (t1 == t2);
					bool b2 = (t2 == t3);
				}

				std::cout << "\nxscope_tuple<> tests: \n";
				{
					/* example from https://en.cppreference.com/w/cpp/utility/tuple */
					struct CB {
						static mse::xscope_tuple<double, char, std::string> get_student(int id)
						{
							if (id == 0) return mse::mstd::make_tuple(3.8, 'A', std::string("Lisa Simpson"));
							if (id == 1) return mse::mstd::make_tuple(2.9, 'C', std::string("Milhouse Van Houten"));
							if (id == 2) return mse::mstd::make_tuple(1.7, 'D', std::string("Ralph Wiggum"));
#if __cpp_exceptions >= 199711
							throw std::invalid_argument("id");
#else // __cpp_exceptions >= 199711
							return mse::mstd::make_tuple(0.0, 'Z', std::string("invalid"));
#endif // __cpp_exceptions >= 199711
						}
					};

					{
						auto student0 = CB::get_student(0);
						std::cout << "ID: 0, "
							<< "GPA: " << std::get<0>(student0) << ", "
							<< "grade: " << std::get<1>(student0) << ", "
							<< "name: " << std::get<2>(student0) << '\n';

						double gpa1;
						char grade1;
						std::string name1;
						std::tie(gpa1, grade1, name1) = CB::get_student(1);
						std::cout << "ID: 1, "
							<< "GPA: " << gpa1 << ", "
							<< "grade: " << grade1 << ", "
							<< "name: " << name1 << '\n';

#ifdef MSE_HAS_CXX17
						// C++17 structured binding:
						auto[gpa2, grade2, name2] = CB::get_student(2);
						std::cout << "ID: 2, "
							<< "GPA: " << gpa2 << ", "
							<< "grade: " << grade2 << ", "
							<< "name: " << name2 << '\n';
#endif // MSE_HAS_CXX17
					}
				}
				{
					/* example from https://en.cppreference.com/w/cpp/utility/tuple/tuple */
					{
						mse::xscope_tuple<int, std::string, double> t1;
						std::cout << "Value-initialized: "; print(t1);
						mse::xscope_tuple<int, std::string, double> t2(42, "Test", -3.14);
						std::cout << "Initialized with values: "; print(t2);
						//mse::xscope_tuple<char, std::string, int> t3(t2);
						//std::cout << "Implicitly converted: "; print(t3);
						mse::xscope_tuple<int, double> t4(std::make_pair(42, 3.14));
						std::cout << "Constructed from a pair"; print(t4);
					}
				}
				{
					/* example from https://en.cppreference.com/w/cpp/utility/tuple/swap */
					mse::xscope_tuple<int, std::string, double> p1, p2;
					p1 = mse::mstd::make_tuple(10, std::string("test"), 3.14);
					p2.swap(p1);
					std::cout << "(" << std::get<0>(p2)
						<< ", " << std::get<1>(p2)
						<< ", " << std::get<2>(p2) << ")\n";
				}
				{
					/* example from https://en.cppreference.com/w/cpp/utility/tuple/swap */
					struct CB {
						static mse::xscope_tuple<int, int> f() // this function returns multiple values
						{
							int x = 5;
							return mse::mstd::make_tuple(x, 7); // return {x,7}; in C++17
						}
					};

					{
						// heterogeneous tuple construction
						int n = 1;
						auto t = mse::mstd::make_tuple(10, std::string("Test"), 3.14, std::ref(n), n);
						n = 7;
						std::cout << "The value of t is " << "("
							<< std::get<0>(t) << ", " << std::get<1>(t) << ", "
							<< std::get<2>(t) << ", " << std::get<3>(t) << ", "
							<< std::get<4>(t) << ")\n";

						// function returning multiple values
						int a, b;
						std::tie(a, b) = CB::f();
						std::cout << a << " " << b << "\n";
					}
				}
				{
					/* example from https://en.cppreference.com/w/cpp/utility/tuple/tuple_cat */
					{
						mse::xscope_tuple<int, std::string, double> t1(10, "Test", 3.14);
						int n = 7;
						auto t2 = mse::xscope_tuple_cat(t1, std::make_pair(std::string("Foo"), std::string("bar")), t1, std::tie(n));
						n = 10;
						print(t2);
					}
					{
						mse::xscope_tuple<int, std::string, double> t1(10, "Test", 3.14);
						mse::xscope_tuple<int, std::string, double> t2(11, "Test2", 4.14);
						mse::xscope_tuple<int, std::string, double> t3(10, "Test", 3.14);
						std::swap(t1, t2);
						bool b1 = (t1 == t2);
						bool b2 = (t2 == t3);
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

} // namespace mse

#ifdef _MSC_VER
#pragma warning( pop )  
#endif /*_MSC_VER*/

#ifndef MSE_PUSH_MACRO_NOT_SUPPORTED
#pragma pop_macro("MSE_THROW")
#pragma pop_macro("_NODISCARD")
#endif // !MSE_PUSH_MACRO_NOT_SUPPORTED

# endif //MSETUPLE_H_
