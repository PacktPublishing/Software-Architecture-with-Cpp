
// Copyright (c) 2015 Noah Lopez
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef MSEFUNCTIONAL_H
#define MSEFUNCTIONAL_H

#ifndef MSE_FUNCTIONAL_NO_XSCOPE_DEPENDENCE
#include "msescope.h"
#endif // !MSE_FUNCTIONAL_NO_XSCOPE_DEPENDENCE
#include "msepointerbasics.h"

#include<functional>

#ifdef MSE_SELF_TESTS
#include <iostream>
#endif // MSE_SELF_TESTS



#ifdef _MSC_VER
#pragma warning( push )  
#pragma warning( disable : 4189 )
#endif /*_MSC_VER*/

namespace mse {
	namespace impl {
		template <class _Fx>
		struct _std_function_template_param {
			typedef void type;
		};
		template <class _Fx>
		struct _std_function_template_param<std::function<_Fx> > {
			typedef _Fx type;
		};
	}

	namespace mstd {
		template<class _Fty> class function;
	}
	template<class _Fty> using function = mstd::function<_Fty>;
	template<class _Fty> class xscope_function;

	namespace mstd {

		template<class _Fty>
		class function : public std::function<_Fty> {
		public:
			typedef std::function<_Fty> base_class;
			function(const base_class& src) : base_class(src) {}
			function(base_class&& src) : base_class(MSE_FWD(src)) {}

			function() noexcept : base_class() {}
			function(std::nullptr_t) noexcept : base_class(nullptr) {}

			template <typename _Fty2, MSE_IMPL_EIP mse::impl::enable_if_t<(!std::is_convertible<const _Fty2*, const function*>::value)
				&& (!std::is_convertible<_Fty2, std::nullptr_t>::value) && (!std::is_same<_Fty2, int>::value)> MSE_IMPL_EIS >
			function(const _Fty2& func) : base_class(func) {
				mse::impl::T_valid_if_not_an_xscope_type<_Fty2>();
			}
			template <typename _Fty2, MSE_IMPL_EIP mse::impl::enable_if_t<(!std::is_convertible<const _Fty2*, const function*>::value)
				&& (!std::is_convertible<_Fty2, std::nullptr_t>::value) && (!std::is_same<_Fty2, int>::value)> MSE_IMPL_EIS >
			function(_Fty2&& func) : base_class(MSE_FWD(func)) {
				mse::impl::T_valid_if_not_an_xscope_type<_Fty2>();
			}

			function& operator=(const function& _Right_cref) {
				base_class::operator=(static_cast<const base_class&>(_Right_cref));
				return (*this);
			}

			void async_not_shareable_and_not_passable_tag() const {}

		private:
			MSE_DEFAULT_OPERATOR_AMPERSAND_DECLARATION;
		};

#ifdef MSE_HAS_CXX17
		/* deduction guides */
		/* just use the same template parameter that std::function would deduce */
		template <class _Fx>
		function(_Fx)->function<typename mse::impl::_std_function_template_param<decltype(std::function(std::declval<_Fx>()))>::type>;
#endif /* MSE_HAS_CXX17 */
	}

#ifndef MSE_FUNCTIONAL_NO_XSCOPE_DEPENDENCE

	template <class _Fty>
	class xscope_function : public std::function<_Fty>, public mse::us::impl::XScopeTagBase
		, public mse::us::impl::ContainsNonOwningScopeReferenceTagBase
	{
	public:
		typedef std::function<_Fty> base_class;

		xscope_function(const xscope_function& src) = default;
		xscope_function(xscope_function&& src) = default;

		xscope_function(const base_class& src) : base_class(src) {}
		xscope_function(base_class&& src) : base_class(MSE_FWD(src)) {}

		xscope_function() noexcept : base_class() {}
		xscope_function(std::nullptr_t) noexcept : base_class(nullptr) {}
		template <typename _Fty2, MSE_IMPL_EIP mse::impl::enable_if_t<(!std::is_convertible<const _Fty2*, const xscope_function*>::value)
			&& (!std::is_convertible<_Fty2, std::nullptr_t>::value) && (!std::is_same<_Fty2, int>::value)> MSE_IMPL_EIS >
		xscope_function(const _Fty2& func) : base_class(mse::us::impl::make_newable_xscope(func)) {}
		template <typename _Fty2, MSE_IMPL_EIP mse::impl::enable_if_t<(!std::is_convertible<const _Fty2*, const xscope_function*>::value)
			&& (!std::is_convertible<_Fty2, std::nullptr_t>::value) && (!std::is_same<_Fty2, int>::value)> MSE_IMPL_EIS >
		xscope_function(_Fty2&& func) : base_class(mse::us::impl::make_newable_xscope(MSE_FWD(func))) {}

		void async_not_shareable_and_not_passable_tag() const {}

	private:

		xscope_function& operator=(const xscope_function& _Right_cref) {
			base_class::operator=(static_cast<const base_class&>(_Right_cref));
			return (*this);
		}
		xscope_function& operator=(xscope_function&& _Right_cref) {
			base_class::operator=(mse::us::impl::as_ref<base_class>(MSE_FWD(_Right_cref)));
			return (*this);
		}
		template <class _Fx, MSE_IMPL_EIP mse::impl::enable_if_t<!std::is_base_of<xscope_function, _Fx>::value> MSE_IMPL_EIS >
		xscope_function& operator=(_Fx&& _Func) {
			base_class::operator=(mse::us::impl::as_ref<base_class>(MSE_FWD(_Func)));
			return (*this);
		}

		MSE_DEFAULT_OPERATOR_NEW_AND_AMPERSAND_DECLARATION;
	};

#ifdef MSE_HAS_CXX17
	/* deduction guides */
	/* just use the same template parameter that std::function would deduce */
	template <class _Fx>
	xscope_function(_Fx)->xscope_function<typename mse::impl::_std_function_template_param<decltype(std::function(std::declval<_Fx>()))>::type>;
#endif /* MSE_HAS_CXX17 */
#endif // !MSE_FUNCTIONAL_NO_XSCOPE_DEPENDENCE
}

namespace mse {

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
		class CFunctionTest1 {
		public:
			static void s_test1() {
#ifdef MSE_SELF_TESTS
				{
					{
						// from https://en.cppreference.com/w/cpp/utility/functional/function
						struct Foo {
							Foo(int num) : num_(num) {}
							void print_add(int i) const { std::cout << num_ + i << '\n'; }
							int num_;
						};

						struct PrintNum {
							void operator()(int i) const
							{
								std::cout << i << '\n';
							}
						};

						class CB {
						public:
							static void print_num(int i)
							{
								std::cout << i << '\n';
							}
						};

						// store a free function
						mse::mstd::function<void(int)> f_display = CB::print_num;
						f_display(-9);

						// store a lambda
						mse::mstd::function<void()> f_display_42 = []() { CB::print_num(42); };
						f_display_42();

						// store the result of a call to std::bind
						mse::mstd::function<void()> f_display_31337 = std::bind(CB::print_num, 31337);
						f_display_31337();

						// store a call to a member function
						mse::mstd::function<void(const Foo&, int)> f_add_display = &Foo::print_add;
						const Foo foo(314159);
						f_add_display(foo, 1);
						f_add_display(314159, 1);

						// store a call to a data member accessor
						mse::mstd::function<int(Foo const&)> f_num = &Foo::num_;
						std::cout << "num_: " << f_num(foo) << '\n';

						// store a call to a member function and object
						using std::placeholders::_1;
						mse::mstd::function<void(int)> f_add_display2 = std::bind(&Foo::print_add, foo, _1);
						f_add_display2(2);

						// store a call to a member function and object ptr
						mse::mstd::function<void(int)> f_add_display3 = std::bind(&Foo::print_add, &foo, _1);
						f_add_display3(3);

						// store a call to a function object
						mse::mstd::function<void(int)> f_display_obj = PrintNum();
						f_display_obj(18);
					}
					{
#ifdef MSE_HAS_CXX17
						// from https://en.cppreference.com/w/cpp/utility/functional/function/deduction_guides
						class CB {
						public:
							static int func(double) { return 0; }
						};

						mse::mstd::function f{ CB::func }; // guide #1 deduces function<int(double)>
						int i = 5;
						mse::mstd::function g = [&](double) { return i; }; // guide #2 deduces function<int(double)>
#endif /* MSE_HAS_CXX17 */
					}
					{
						// from https://en.cppreference.com/w/cpp/utility/functional/function/operator()
						class CB {
						public:
							static void call(mse::mstd::function<int()> f)  // can be passed by value
							{
								std::cout << f() << '\n';
							}

							static int normal_function()
							{
								return 42;
							}
						};
						int n = 1;
						mse::mstd::function<int()> f = [&n]() { return n; };
						CB::call(f);

						n = 2;
						CB::call(f);

						f = CB::normal_function;
						CB::call(f);
					}
					{
						// from https://en.cppreference.com/w/cpp/utility/functional/function/operator_bool
						class CB {
						public:
							static void sampleFunction()
							{
								std::cout << "This is the sample function!\n";
							}

							static void checkFunc(std::function<void()>& func)
							{
								// Use operator bool to determine if callable target is available.
								if (func)
								{
									std::cout << "Function is not empty! Calling function.\n";
									func();
								}
								else
								{
									std::cout << "Function is empty. Nothing to do.\n";
								}
							}
						};
						mse::mstd::function<void()> f1;
						mse::mstd::function<void()> f2(CB::sampleFunction);

						std::cout << "f1: ";
						CB::checkFunc(f1);

						std::cout << "f2: ";
						CB::checkFunc(f2);
					}
					{
						mse::mstd::function<int()> function1 = []() { return 3; };
						function1 = []() { return 5; };
						int res1 = function1();
					}
				}

				{
					{
						// from https://en.cppreference.com/w/cpp/utility/functional/function
						struct Foo {
							Foo(int num) : num_(num) {}
							void print_add(int i) const { std::cout << num_ + i << '\n'; }
							int num_;
						};

						struct PrintNum {
							void operator()(int i) const
							{
								std::cout << i << '\n';
							}
						};

						class CB {
						public:
							static void print_num(int i)
							{
								std::cout << i << '\n';
							}
						};

						// store a free function
						mse::xscope_function<void(int)> f_display = CB::print_num;
						f_display(-9);

						// store a lambda
						mse::xscope_function<void()> f_display_42 = []() { CB::print_num(42); };
						f_display_42();

						// store the result of a call to std::bind
						mse::xscope_function<void()> f_display_31337 = std::bind(CB::print_num, 31337);
						f_display_31337();

						// store a call to a member function
						mse::xscope_function<void(const Foo&, int)> f_add_display = &Foo::print_add;
						const Foo foo(314159);
						f_add_display(foo, 1);
						f_add_display(314159, 1);

						// store a call to a data member accessor
						mse::xscope_function<int(Foo const&)> f_num = &Foo::num_;
						std::cout << "num_: " << f_num(foo) << '\n';

						// store a call to a member function and object
						using std::placeholders::_1;
						mse::xscope_function<void(int)> f_add_display2 = std::bind(&Foo::print_add, foo, _1);
						f_add_display2(2);

						// store a call to a member function and object ptr
						mse::xscope_function<void(int)> f_add_display3 = std::bind(&Foo::print_add, &foo, _1);
						f_add_display3(3);

						// store a call to a function object
						mse::xscope_function<void(int)> f_display_obj = PrintNum();
						f_display_obj(18);
					}
					{
#ifdef MSE_HAS_CXX17
					// from https://en.cppreference.com/w/cpp/utility/functional/function/deduction_guides
					class CB {
					public:
						static int func(double) { return 0; }
					};

					mse::xscope_function f{ CB::func }; // guide #1 deduces function<int(double)>
					int i = 5;
					mse::xscope_function g = [&](double) { return i; }; // guide #2 deduces function<int(double)>
#endif /* MSE_HAS_CXX17 */
					}
					{
						// from https://en.cppreference.com/w/cpp/utility/functional/function/operator()
						class CB {
						public:
							static void call(mse::xscope_function<int()> f)  // can be passed by value
							{
								std::cout << f() << '\n';
							}

							static int normal_function()
							{
								return 42;
							}
						};
						int n = 1;
						mse::xscope_function<int()> f = [&n]() { return n; };
						CB::call(f);

						n = 2;
						CB::call(f);

						mse::xscope_function<int()> f2 = CB::normal_function;
						CB::call(f2);
					}
					{
						// from https://en.cppreference.com/w/cpp/utility/functional/function/operator_bool
						class CB {
						public:
							static void sampleFunction()
							{
								std::cout << "This is the sample function!\n";
							}

							static void checkFunc(std::function<void()>& func)
							{
								// Use operator bool to determine if callable target is available.
								if (func)
								{
									std::cout << "Function is not empty! Calling function.\n";
									func();
								}
								else
								{
									std::cout << "Function is empty. Nothing to do.\n";
								}
							}
						};
						mse::xscope_function<void()> f1;
						mse::xscope_function<void()> f2(CB::sampleFunction);

						std::cout << "f1: ";
						CB::checkFunc(f1);

						std::cout << "f2: ";
						CB::checkFunc(f2);
					}
					{
						mse::xscope_function<int()> xs_function1 = []() { return 5; };

						auto xs_int1 = mse::make_xscope(int(5));
						auto int1_xsptr = &xs_int1;

						struct xscope_my_function_obj_t : public mse::rsv::XScopeTagBase, public mse::rsv::ContainsNonOwningScopeReferenceTagBase {
							xscope_my_function_obj_t(decltype(int1_xsptr) int_xsptr) : m_int_xsptr(int_xsptr) {}
							decltype(int1_xsptr) m_int_xsptr; /* this is a scope pointer */
							int operator()() const { return *m_int_xsptr; }
						};
						/* xscope_my_function_obj_t is a scope type with a scope (pointer) member. */
						xscope_my_function_obj_t xscope_my_function_obj1(int1_xsptr);
						mse::xscope_function<int()> xs_function2 = xscope_my_function_obj1;
						int res1 = xs_function2();

						/*  Just as structs with scope pointer/reference members need to be declared as such, lambdas that
						capture scope pointer/references must be declared as such. */
						auto xs_lambda1 = mse::rsv::make_xscope_reference_or_pointer_capture_lambda([int1_xsptr]() { return *int1_xsptr; });
						mse::xscope_function<int()> xs_function3 = xs_lambda1;
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


#endif /*ndef MSEFUNCTIONAL_H*/
