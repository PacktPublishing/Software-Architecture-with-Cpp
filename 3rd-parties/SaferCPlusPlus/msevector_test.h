
// Copyright (c) 2015 Noah Lopez
// Use, modification, and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef MSEVECTOR_TEST_H
#define MSEVECTOR_TEST_H

//include "msetl.h"
#include "msemsevector.h"
#include "msemstdvector.h"
#include "mseivector.h"
#include <algorithm>    // std::sort

#ifdef _MSC_VER
#pragma warning( push )  
#pragma warning( disable : 4100 4456 4189 )
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

namespace mse {
	class msevector_test {
	public:
		/* This is a unit test for mse::msevector. It was originally swiped from stlport (http://www.stlport.org/). */
		msev_bool m_result1_is_valid = false;
		msev_int m_result1;
		void m_initialize_m_result1_if_needed() {
			if (!m_result1_is_valid) { m_result1 = 0; m_result1_is_valid = true; }
		}
		msev_int m_terminate_m_result1() {
			if (!m_result1_is_valid) {
				m_initialize_m_result1_if_needed();
			}
			assert(m_result1_is_valid); m_result1_is_valid = false; return m_result1;
		}
#define EXAM_IMPL(x) x()
#define EXAM_CHECK(x) m_initialize_m_result1_if_needed(); m_result1 += (x) ? 0 : 1;
#define EXAM_RESULT m_terminate_m_result1()
#define EXAM_REQUIRE(x) assert(x)
		void run_all()
		{
			int q2 = 11;
			mse::msevector_test msevector_test1;
			mse::msev_int res1 = msevector_test1.msevec_test_1();
			mse::msev_int res2 = msevector_test1.msevec_test_2();
			mse::msev_int res3 = msevector_test1.msevec_test_3();
			mse::msev_int res4 = msevector_test1.msevec_test_4();
			mse::msev_int res5 = msevector_test1.msevec_test_5();
			mse::msev_int res6 = msevector_test1.msevec_test_6();
			mse::msev_int res7 = msevector_test1.msevec_test_7();
			mse::msev_int res_cap = msevector_test1.capacity();
			mse::msev_int res_at = msevector_test1.at();
			mse::msev_int res_ptr = msevector_test1.pointer();
			mse::msev_int res_aref = msevector_test1.auto_ref();
			mse::msev_int res_oc = msevector_test1.optimizations_check();
			mse::msev_int res_asschk = msevector_test1.assign_check();
			mse::msev_int res_its = msevector_test1.iterators();
			mse::msev_int res_ebo = msevector_test1.ebo();
			mse::msev_int res_bvec1 = msevector_test1.bvec1();
			mse::msev_int resm1 = msevector_test1.msevec_test_m1();
			mse::msev_int resm2 = msevector_test1.msevec_test_m2();
			mse::msev_int resm3 = msevector_test1.msevec_test_m3();
			mse::msev_int resm4 = msevector_test1.msevec_test_m4();
			mse::msev_int resm5 = msevector_test1.msevec_test_m5();
			mse::msev_int resm6 = msevector_test1.msevec_test_m6();
			mse::msev_int resm7 = msevector_test1.msevec_test_m7();
			mse::msev_int resi1 = msevector_test1.msevec_test_i1();
			mse::msev_int resi2 = msevector_test1.msevec_test_i2();
			mse::msev_int resi3 = msevector_test1.msevec_test_i3();
			mse::msev_int resi4 = msevector_test1.msevec_test_i4();
			mse::msev_int resi5 = msevector_test1.msevec_test_i5();
			mse::msev_int resi6 = msevector_test1.msevec_test_i6();
			mse::msev_int resi7 = msevector_test1.msevec_test_i7();
			mse::msev_int ress1 = msevector_test1.msevec_test_s1();
			mse::msev_int ress2 = msevector_test1.msevec_test_s2();
			mse::msev_int ress3 = msevector_test1.msevec_test_s3();
			mse::msev_int ress4 = msevector_test1.msevec_test_s4();
			mse::msev_int ress5 = msevector_test1.msevec_test_s5();
			mse::msev_int ress6 = msevector_test1.msevec_test_s6();
			mse::msev_int ress7 = msevector_test1.msevec_test_s7();
			mse::msev_int resss1 = msevector_test1.msevec_test_ss1();
			mse::msev_int resss2 = msevector_test1.msevec_test_ss2();
			mse::msev_int resss3 = msevector_test1.msevec_test_ss3();
			mse::msev_int resss4 = msevector_test1.msevec_test_ss4();
			mse::msev_int resss5 = msevector_test1.msevec_test_ss5();
			mse::msev_int resss6 = msevector_test1.msevec_test_ss6();
			mse::msev_int resss7 = msevector_test1.msevec_test_ss7();
			mse::msev_int resmt1 = msevector_test1.msevec_test_mt1();
			mse::msev_int resmt2 = msevector_test1.msevec_test_mt2();
			mse::msev_int resmt3 = msevector_test1.msevec_test_mt3();
			mse::msev_int resmt4 = msevector_test1.msevec_test_mt4();
			mse::msev_int resmt5 = msevector_test1.msevec_test_mt5();
			mse::msev_int resmt6 = msevector_test1.msevec_test_mt6();
			mse::msev_int resmt7 = msevector_test1.msevec_test_mt7();
			mse::msev_int res_mvec_as_stdvec = res1 + res2 + res3 + res4 + res5 + res6 + res7 + res_cap + res_at + res_ptr + res_aref
				/*+ res_oc */ + res_asschk + res_its + res_ebo + res_bvec1;
			mse::msev_int res_mvec = resm1 + resm2 + resm3 + resm4 + resm5 + resm6 + resm7;
			mse::msev_int res_ivec = resi1 + resi2 + resi3 + resi4 + resi5 + resi6 + resi7;
			mse::msev_int res_svec = ress1 + ress2 + ress3 + ress4 + ress5 + ress6 + ress7;
			mse::msev_int res_ssvec = resss1 + resss2 + resss3 + resss4 + resss5 + resss6 + resss7;
			mse::msev_int res_mtvec = resmt1 + resmt2 + resmt3 + resmt4 + resmt5 + resmt6 + resmt7;
			mse::msev_int resall = res_mvec_as_stdvec + res_mvec + res_ivec + res_svec + res_ssvec + res_mtvec;
			int q = 7;
		}

		//int EXAM_IMPL(vector_test::vec_test_1)
		msev_int msevec_test_1()
		{
			us::msevector<int> v1; // Empty vector of integers.

			EXAM_CHECK(v1.empty() == true);
			EXAM_CHECK(v1.size() == 0);

			// EXAM_CHECK( v1.max_size() == INT_MAX / sizeof(int) );
			// cout << "max_size = " << v1.max_size() << endl;
			v1.push_back(42); // Add an integer to the vector.

			EXAM_CHECK(v1.size() == 1);

			EXAM_CHECK(v1[0] == 42);

			{
				us::msevector<us::msevector<int> > vect(10);
				us::msevector<us::msevector<int> >::iterator it(vect.begin()), end(vect.end());
				for (; it != end; ++it) {
					EXAM_CHECK((*it).empty());
					EXAM_CHECK((*it).size() == 0);
					EXAM_CHECK((*it).capacity() == 0);
					EXAM_CHECK((*it).begin() == (*it).end());
				}
			}

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_2)
		msev_int msevec_test_2()
		{
			us::msevector<double> v1; // Empty vector of doubles.
			v1.push_back(32.1);
			v1.push_back(40.5);
			us::msevector<double> v2; // Another empty vector of doubles.
			v2.push_back(3.56);

			EXAM_CHECK(v1.size() == 2);
			EXAM_CHECK(v1[0] == 32.1);
			EXAM_CHECK(v1[1] == 40.5);

			EXAM_CHECK(v2.size() == 1);
			EXAM_CHECK(v2[0] == 3.56);
			msev_size_t v1Cap = v1.capacity();
			msev_size_t v2Cap = v2.capacity();

			v1.swap(v2); // Swap the vector's contents.

			EXAM_CHECK(v1.size() == 1);
			EXAM_CHECK(v1.capacity() == v2Cap);
			EXAM_CHECK(v1[0] == 3.56);

			EXAM_CHECK(v2.size() == 2);
			EXAM_CHECK(v2.capacity() == v1Cap);
			EXAM_CHECK(v2[0] == 32.1);
			EXAM_CHECK(v2[1] == 40.5);

			v2 = v1; // Assign one vector to another.

			EXAM_CHECK(v2.size() == 1);
			EXAM_CHECK(v2[0] == 3.56);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_3)
		msev_int msevec_test_3()
		{
			typedef us::msevector<char> vec_type;

			vec_type v1; // Empty vector of characters.
			v1.push_back('h');
			v1.push_back('i');

			EXAM_CHECK(v1.size() == 2);
			EXAM_CHECK(v1[0] == 'h');
			EXAM_CHECK(v1[1] == 'i');

			vec_type v2(v1.begin(), v1.end());
			v2[1] = 'o'; // Replace second character.

			EXAM_CHECK(v2.size() == 2);
			EXAM_CHECK(v2[0] == 'h');
			EXAM_CHECK(v2[1] == 'o');

			EXAM_CHECK((v1 == v2) == false);

			EXAM_CHECK((v1 < v2) == true);

			v1.swap(v2);

			EXAM_CHECK(v1[0] == 'h');
			EXAM_CHECK(v1[1] == 'o');
			EXAM_CHECK(v2[0] == 'h');
			EXAM_CHECK(v2[1] == 'i');

			std::swap(v1, v2);

			EXAM_CHECK(v1[0] == 'h');
			EXAM_CHECK(v1[1] == 'i');
			EXAM_CHECK(v2[0] == 'h');
			EXAM_CHECK(v2[1] == 'o');

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_4)
		msev_int msevec_test_4()
		{
			us::msevector<int> v(4);

			v[0] = 1;
			v[1] = 4;
			v[2] = 9;
			v[3] = 16;

			EXAM_CHECK(v.front() == 1);
			EXAM_CHECK(v.back() == 16);

			v.push_back(25);

			EXAM_CHECK(v.back() == 25);
			EXAM_CHECK(v.size() == 5);

			v.pop_back();

			EXAM_CHECK(v.back() == 16);
			EXAM_CHECK(v.size() == 4);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_5)
		msev_int msevec_test_5()
		{
			int array[] = { 1, 4, 9, 16 };

#ifdef MSVC2010_COMPATIBLE
			us::msevector<int> v(array, array + 4);
#else /*MSVC2010_COMPATIBLE*/
			us::msevector<int> v = { 1, 4, 9, 16 };
#endif /*MSVC2010_COMPATIBLE*/

			EXAM_CHECK(v.size() == 4);

			EXAM_CHECK(v[0] == 1);
			EXAM_CHECK(v[1] == 4);
			EXAM_CHECK(v[2] == 9);
			EXAM_CHECK(v[3] == 16);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_6)
		msev_int msevec_test_6()
		{
			int array[] = { 1, 4, 9, 16, 25, 36 };

#ifdef MSVC2010_COMPATIBLE
			us::msevector<int> v(array, array + 6);
#else /*MSVC2010_COMPATIBLE*/
			us::msevector<int> v = { 1, 4, 9, 16, 25, 36 };
#endif /*MSVC2010_COMPATIBLE*/
			us::msevector<int>::iterator vit;

			EXAM_CHECK(v.size() == 6);
			EXAM_CHECK(v[0] == 1);
			EXAM_CHECK(v[1] == 4);
			EXAM_CHECK(v[2] == 9);
			EXAM_CHECK(v[3] == 16);
			EXAM_CHECK(v[4] == 25);
			EXAM_CHECK(v[5] == 36);

			vit = v.erase(v.begin()); // Erase first element.
			EXAM_CHECK(*vit == 4);

			EXAM_CHECK(v.size() == 5);
			EXAM_CHECK(v[0] == 4);
			EXAM_CHECK(v[1] == 9);
			EXAM_CHECK(v[2] == 16);
			EXAM_CHECK(v[3] == 25);
			EXAM_CHECK(v[4] == 36);

			vit = v.erase(v.end() - 1); // Erase last element.
			EXAM_CHECK(vit == v.end());

			EXAM_CHECK(v.size() == 4);
			EXAM_CHECK(v[0] == 4);
			EXAM_CHECK(v[1] == 9);
			EXAM_CHECK(v[2] == 16);
			EXAM_CHECK(v[3] == 25);


			v.erase(v.begin() + 1, v.end() - 1); // Erase all but first and last.

			EXAM_CHECK(v.size() == 2);
			EXAM_CHECK(v[0] == 4);
			EXAM_CHECK(v[1] == 25);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_7)
		msev_int msevec_test_7()
		{
			int array1[] = { 1, 4, 25 };
			int array2[] = { 9, 16 };

#ifdef MSVC2010_COMPATIBLE
			us::msevector<int> v(array1, array1 + 3);
#else /*MSVC2010_COMPATIBLE*/
			us::msevector<int> v = { 1, 4, 25 };
#endif /*MSVC2010_COMPATIBLE*/
			us::msevector<int>::iterator vit;
			vit = v.insert(v.begin(), 0); // Insert before first element.
			EXAM_CHECK(*vit == 0);

			vit = v.insert(v.end(), 36);  // Insert after last element.
			EXAM_CHECK(*vit == 36);

			EXAM_CHECK(v.size() == 5);
			EXAM_CHECK(v[0] == 0);
			EXAM_CHECK(v[1] == 1);
			EXAM_CHECK(v[2] == 4);
			EXAM_CHECK(v[3] == 25);
			EXAM_CHECK(v[4] == 36);

			// Insert contents of array2 before fourth element.
#ifdef MSVC2010_COMPATIBLE
			v.insert(v.begin() + 3, array2, array2 + 2);
#else /*MSVC2010_COMPATIBLE*/
			v.insert(v.begin() + 3, { 9, 16 });
#endif /*MSVC2010_COMPATIBLE*/

			EXAM_CHECK(v.size() == 7);

			EXAM_CHECK(v[0] == 0);
			EXAM_CHECK(v[1] == 1);
			EXAM_CHECK(v[2] == 4);
			EXAM_CHECK(v[3] == 9);
			EXAM_CHECK(v[4] == 16);
			EXAM_CHECK(v[5] == 25);
			EXAM_CHECK(v[6] == 36);

			v.clear();
			EXAM_CHECK(v.empty());

			v.insert(v.begin(), 5, 10);
			EXAM_CHECK(v.size() == 5);
			EXAM_CHECK(v[0] == 10);
			EXAM_CHECK(v[1] == 10);
			EXAM_CHECK(v[2] == 10);
			EXAM_CHECK(v[3] == 10);
			EXAM_CHECK(v[4] == 10);

			/*
			{
			us::msevector<float> vf(2.0f, 3.0f);
			EXAM_CHECK( vf.size() == 2 );
			EXAM_CHECK( vf.front() == 3.0f );
			EXAM_CHECK( vf.back() == 3.0f );
			}
			*/

			return EXAM_RESULT;
		}

		struct TestStruct
		{
			unsigned int a[3];
		};

		//int EXAM_IMPL(vector_test::capacity)
		msev_int capacity()
		{
			{
				us::msevector<int> v;

				EXAM_CHECK(v.capacity() == 0);
				v.push_back(42);
				EXAM_CHECK(v.capacity() >= 1);
				v.reserve(5000);
				EXAM_CHECK(v.capacity() >= 5000);
			}

			{
				//Test that used to generate an assertion when using __debug_alloc.
				us::msevector<TestStruct> va;
				va.reserve(1);
				va.reserve(2);
			}

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(msevector_test::at)
		msev_int at()
		{
			us::msevector<int> v;
			us::msevector<int> const& cv = v;

			v.push_back(10);
			EXAM_CHECK(v.at(0) == 10);
			v.at(0) = 20;
			EXAM_CHECK(cv.at(0) == 20);

#if !defined (STLPORT) || defined (_STLP_USE_EXCEPTIONS)
			MSE_TRY {
				v.at(1) = 20;
				//EXAM_ERROR("out_of_range exception expected");
				EXAM_CHECK(false);
			}
			MSE_CATCH (std::out_of_range const&) {
				//EXAM_MESSAGE("out_of_range exception, as expected");
			}
			MSE_CATCH_ANY {
				//EXAM_ERROR("out_of_range exception expected");
				EXAM_CHECK(false);
			}
#endif

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(msevector_test::pointer)
		msev_int pointer()
		{
#ifdef MSEREGISTERED_H_
			us::msevector<mse::TRegisteredPointer<int> > v1;
			us::msevector<mse::TRegisteredPointer<int> > v2 = v1;
			us::msevector<mse::TRegisteredPointer<int> > v3;

			v3.insert(v3.end(), v1.begin(), v1.end());
#endif // MSEREGISTERED_H_

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(msevector_test::auto_ref)
		msev_int auto_ref()
		{
			us::msevector<int> ref;
			for (int i = 0; i < 5; ++i) {
				ref.push_back(i);
			}

			us::msevector<us::msevector<int> > v_v_int(1, ref);
			v_v_int.push_back(v_v_int[0]);
			v_v_int.push_back(ref);
			v_v_int.push_back(v_v_int[0]);
			v_v_int.push_back(v_v_int[0]);
			v_v_int.push_back(ref);

			us::msevector<us::msevector<int> >::iterator vvit(v_v_int.begin()), vvitEnd(v_v_int.end());
			for (; vvit != vvitEnd; ++vvit) {
				EXAM_CHECK(*vvit == ref);
			}

			/*
			* Forbidden by the Standard:
			v_v_int.insert(v_v_int.end(), v_v_int.begin(), v_v_int.end());
			for (vvit = v_v_int.begin(), vvitEnd = v_v_int.end();
			vvit != vvitEnd; ++vvit) {
			EXAM_CHECK( *vvit == ref );
			}
			*/

			return EXAM_RESULT;
		}

#ifdef ALLOCATOR_WITH_STATE_TEST
		//int EXAM_IMPL(msevector_test::allocator_with_state)
		msev_int allocator_with_state()
		{
			char buf1[1024];
			StackAllocator<int> stack1(buf1, buf1 + sizeof(buf1));

			char buf2[1024];
			StackAllocator<int> stack2(buf2, buf2 + sizeof(buf2));

			{
				typedef us::msevector<int, StackAllocator<int> > VectorInt;
				VectorInt vint1(10, 0, stack1);
				VectorInt vint1Cpy(vint1);

				VectorInt vint2(10, 1, stack2);
				VectorInt vint2Cpy(vint2);

				vint1.swap(vint2);

				EXAM_CHECK(vint1.get_allocator().swaped());
				EXAM_CHECK(vint2.get_allocator().swaped());

				EXAM_CHECK(vint1 == vint2Cpy);
				EXAM_CHECK(vint2 == vint1Cpy);
				EXAM_CHECK(vint1.get_allocator() == stack2);
				EXAM_CHECK(vint2.get_allocator() == stack1);
			}
			EXAM_CHECK(stack1.ok());
			EXAM_CHECK(stack2.ok());

			return EXAM_RESULT;
		}
#endif /*ALLOCATOR_WITH_STATE_TEST*/

		struct Point
		{
			int x, y;
		};

		struct PointEx :
			public Point
		{
			PointEx() :
			builtFromBase(false)
			{ }
			PointEx(const Point& p) :
				builtFromBase(true)
			{
				x = p.x;
				y = p.y;
			}

			bool builtFromBase;
		};

		//This test check that msevector implementation do not over optimize
		//operation as PointEx copy constructor is trivial
		//int EXAM_IMPL(msevector_test::optimizations_check)
		msev_int optimizations_check()
		{
			us::msevector<Point> v1(1);
			EXAM_REQUIRE(v1.size() == 1);

			v1[0].x = 1;
			v1[0].y = 2;

#ifndef MSVC2010_COMPATIBLE
#if !(defined(GPP_COMPATIBLE))
			EXAM_CHECK(std::is_trivially_copyable<Point>::value == true);
			EXAM_CHECK(std::is_trivially_copyable<us::msevector<Point>::const_iterator>::value == true);
			EXAM_CHECK(std::is_trivially_copyable<PointEx>::value == true);
			EXAM_CHECK(std::is_trivially_copyable<us::msevector<PointEx>::const_iterator>::value == true);
#endif /*!(GPP_COMPATIBLE)*/
#endif /*MSVC2010_COMPATIBLE*/

			//us::msevector<PointEx> v2(v1.begin(), v1.end()); // non-trivial 
			std::vector<PointEx> v2a(v1.begin(), v1.end()); // non-trivial 
			us::msevector<PointEx> v2 = v2a;
			EXAM_REQUIRE(v2.size() == 1);
			EXAM_CHECK(v2[0].builtFromBase == true);
			EXAM_CHECK(v2[0].x == 1);
			EXAM_CHECK(v2[0].y == 2);

			v1[0].x = 3;
			v1[0].y = 4;

			v2.insert(v2.end(), v1.begin(), v1.end()); // non-trivial
			EXAM_REQUIRE(v2.size() == 2);
			EXAM_CHECK(v2[1].builtFromBase == true);
			EXAM_CHECK(v2[0].x == 1);
			EXAM_CHECK(v2[0].y == 2);
			EXAM_CHECK(v2[1].x == 3);
			EXAM_CHECK(v2[1].y == 4);

			v2[1].builtFromBase = false; // change it

			//us::msevector<PointEx> v3(v2.begin(), v2.end()); // should be trivial copy ctor
			std::vector<PointEx> v3a(v2.begin(), v2.end()); // should be trivial copy ctor
			us::msevector<PointEx> v3 = v3a;
			EXAM_REQUIRE(v3.size() == 2);
			EXAM_CHECK(v3[0].x == 1);
			EXAM_CHECK(v3[0].y == 2);
			EXAM_CHECK(v3[1].x == 3);
			EXAM_CHECK(v3[1].y == 4);
			EXAM_CHECK(v2[0].builtFromBase == true);
			EXAM_CHECK(v2[1].builtFromBase == false); // was changed above

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(msevector_test::assign_check)
		msev_int assign_check()
		{
			int array[] = { 1, 2, 3, 4, 5 };
			{
				us::msevector<int> v(3, 1);

#ifdef MSVC2010_COMPATIBLE
				v.assign(array, array + 5);
#else /*MSVC2010_COMPATIBLE*/
				v.assign({ 1, 2, 3, 4, 5 });
#endif /*MSVC2010_COMPATIBLE*/
				EXAM_CHECK(v[4] == 5);
				EXAM_CHECK(v[0] == 1);
				EXAM_CHECK(v[1] == 2);
				EXAM_CHECK(v.size() == 5);
			}
			{
				us::msevector<int> v(7, 1);
				int array[] = { 1, 2, 3, 4, 5 };

#ifdef MSVC2010_COMPATIBLE
				v.assign(array, array + 5);
#else /*MSVC2010_COMPATIBLE*/
				v.assign({ 1, 2, 3, 4, 5 });
#endif /*MSVC2010_COMPATIBLE*/
				EXAM_CHECK(v[4] == 5);
				EXAM_CHECK(v[0] == 1);
				EXAM_CHECK(v[1] == 2);
				EXAM_CHECK(v.size() == 5);
			}
			{
				us::msevector<int> v(3, 1);
				v.reserve(7);

#ifdef MSVC2010_COMPATIBLE
				v.assign(array, array + 5);
#else /*MSVC2010_COMPATIBLE*/
				v.assign({ 1, 2, 3, 4, 5 });
#endif /*MSVC2010_COMPATIBLE*/
				EXAM_CHECK(v[4] == 5);
				EXAM_CHECK(v[0] == 1);
				EXAM_CHECK(v[1] == 2);
				EXAM_CHECK(v.size() == 5);
			}

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(msevector_test::iterators)
		msev_int iterators()
		{
			us::msevector<int> vint(10, 0);
			us::msevector<int> const& crvint = vint;

			EXAM_CHECK(vint.begin() == vint.begin());
			EXAM_CHECK(crvint.begin() == vint.begin());
			EXAM_CHECK(vint.begin() == crvint.begin());
			EXAM_CHECK(crvint.begin() == crvint.begin());

			EXAM_CHECK(vint.begin() != vint.end());
			EXAM_CHECK(crvint.begin() != vint.end());
			EXAM_CHECK(vint.begin() != crvint.end());
			EXAM_CHECK(crvint.begin() != crvint.end());

			EXAM_CHECK(vint.rbegin() == vint.rbegin());
			// Not Standard:
			//EXAM_CHECK( vint.rbegin() == crvint.rbegin() );
			//EXAM_CHECK( crvint.rbegin() == vint.rbegin() );
			EXAM_CHECK(crvint.rbegin() == crvint.rbegin());

			EXAM_CHECK(vint.rbegin() != vint.rend());
			// Not Standard:
			//EXAM_CHECK( vint.rbegin() != crvint.rend() );
			//EXAM_CHECK( crvint.rbegin() != vint.rend() );
			EXAM_CHECK(crvint.rbegin() != crvint.rend());

			return EXAM_RESULT;
		}


		/* Simple compilation test: Check that nested types like iterator
		* can be access even if type used to instanciate container is not
		* yet completely defined.
		*/
		class IncompleteClass
		{
			us::msevector<IncompleteClass> instances;
			typedef us::msevector<IncompleteClass>::iterator it;
		};

#if defined (STLPORT)
#  define NOTHROW _STLP_NOTHROW
#else
#  define NOTHROW throw()
#endif

		/* This allocator implementation purpose is simply to break some
		* internal STLport mecanism specific to the STLport own allocator
		* implementation. */
		template <class _Tp>
		//struct NotSTLportAllocator : public allocator<_Tp> {
		class NotSTLportAllocator : public std::allocator<_Tp> {
		public:
			template <class _Tp1> struct rebind {
				typedef NotSTLportAllocator<_Tp1> other;
			};
			NotSTLportAllocator() NOTHROW{}
			template <class _Tp1> NotSTLportAllocator(const NotSTLportAllocator<_Tp1>&) NOTHROW{}
			NotSTLportAllocator(const NotSTLportAllocator<_Tp>&) NOTHROW{}
			~NotSTLportAllocator() NOTHROW{}
		};

		/* This test check a potential issue with empty base class
		* optimization. Some compilers (VC6) do not implement it
		* correctly resulting ina wrong behavior. */
		//int EXAM_IMPL(msevector_test::ebo)
		msev_int ebo()
		{
			// We use heap memory as test failure can corrupt msevector internal
			// representation making executable crash on msevector destructor invocation.
			// We prefer a simple memory leak, internal corruption should be reveal
			// by size or capacity checks.
			typedef us::msevector<int, NotSTLportAllocator<int> > V;
			V *pv1 = new V(1, 1);
			V *pv2 = new V(10, 2);

			msev_size_t v1Capacity = pv1->capacity();
			msev_size_t v2Capacity = pv2->capacity();

			pv1->swap(*pv2);

			EXAM_CHECK(pv1->size() == 10);
			EXAM_CHECK(pv1->capacity() == v2Capacity);
			EXAM_CHECK((*pv1)[5] == 2);

			EXAM_CHECK(pv2->size() == 1);
			EXAM_CHECK(pv2->capacity() == v1Capacity);
			EXAM_CHECK((*pv2)[0] == 1);

			delete pv2;
			delete pv1;

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(bmsevector_test::bvec1)
		msev_int bvec1()
		{
#if defined (STLPORT) && !defined (_STLP_NO_EXTENSIONS)
			bool ii[3] = { 1, 0, 1 };
			bit_msevector b(3);

			EXAM_CHECK(b[0] == 0);
			EXAM_CHECK(b[1] == 0);
			EXAM_CHECK(b[2] == 0);

			b[0] = b[2] = 1;

			EXAM_CHECK(b[0] == 1);
			EXAM_CHECK(b[1] == 0);
			EXAM_CHECK(b[2] == 1);

			b.insert(b.begin(), (bool*)ii, ii + 2);

			EXAM_CHECK(b[0] == 1);
			EXAM_CHECK(b[1] == 0);
			EXAM_CHECK(b[2] == 1);
			EXAM_CHECK(b[3] == 0);
			EXAM_CHECK(b[4] == 1);

			bit_msevector bb = b;
			if (bb != b)
				exit(1);

			b[0] |= 0;
			b[1] |= 0;
			b[2] |= 1;
			b[3] |= 1;
			EXAM_CHECK(!((b[0] != 1) || (b[1] != 0) || (b[2] != 1) || (b[3] != 1)));


			bb[0] &= 0;
			bb[1] &= 0;
			bb[2] &= 1;
			bb[3] &= 1;
			EXAM_CHECK(!((bb[0] != 0) || (bb[1] != 0) || (bb[2] != 1) || (bb[3] != 0)));
#else
			//throw exam::skip_exception();
#endif
			return EXAM_RESULT;
		}


		//int EXAM_IMPL(vector_test::vec_test_1)
		msev_int msevec_test_m1()
		{
			us::msevector<int> v1; // Empty vector of integers.

			EXAM_CHECK(v1.empty() == true);
			EXAM_CHECK(v1.size() == 0);

			// EXAM_CHECK( v1.max_size() == INT_MAX / sizeof(int) );
			// cout << "max_size = " << v1.max_size() << endl;
			v1.push_back(42); // Add an integer to the vector.

			EXAM_CHECK(v1.size() == 1);

			EXAM_CHECK(v1[0] == 42);

			{
				us::msevector<us::msevector<int> > vect(10);
				//us::msevector<us::msevector<int> >::iterator it(vect.begin()), end(vect.end());
				us::msevector<us::msevector<int> >::cipointer it(vect);
				for (; it.points_to_an_item(); it.set_to_next()) {
					EXAM_CHECK((*it).empty());
					EXAM_CHECK((*it).size() == 0);
					EXAM_CHECK((*it).capacity() == 0);
					EXAM_CHECK((*it).begin() == (*it).end());
				}
			}

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_2)
		msev_int msevec_test_m2()
		{
			us::msevector<double> v1; // Empty vector of doubles.
			v1.push_back(32.1);
			v1.push_back(40.5);
			us::msevector<double> v2; // Another empty vector of doubles.
			v2.push_back(3.56);

			EXAM_CHECK(v1.size() == 2);
			EXAM_CHECK(v1[0] == 32.1);
			EXAM_CHECK(v1[1] == 40.5);

			EXAM_CHECK(v2.size() == 1);
			EXAM_CHECK(v2[0] == 3.56);
			msev_size_t v1Cap = v1.capacity();
			msev_size_t v2Cap = v2.capacity();

			v1.swap(v2); // Swap the vector's contents.

			EXAM_CHECK(v1.size() == 1);
			EXAM_CHECK(v1.capacity() == v2Cap);
			EXAM_CHECK(v1[0] == 3.56);

			EXAM_CHECK(v2.size() == 2);
			EXAM_CHECK(v2.capacity() == v1Cap);
			EXAM_CHECK(v2[0] == 32.1);
			EXAM_CHECK(v2[1] == 40.5);

			v2 = v1; // Assign one vector to another.

			EXAM_CHECK(v2.size() == 1);
			EXAM_CHECK(v2[0] == 3.56);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_3)
		msev_int msevec_test_m3()
		{
			typedef us::msevector<char> vec_type;

			vec_type v1; // Empty vector of characters.
			v1.push_back('h');
			v1.push_back('i');

			EXAM_CHECK(v1.size() == 2);
			EXAM_CHECK(v1[0] == 'h');
			EXAM_CHECK(v1[1] == 'i');

			vec_type::cipointer it1(v1);
			vec_type::cipointer it2(v1);
			it2.set_to_end_marker();
			vec_type v2(it1, it2);
			v2[1] = 'o'; // Replace second character.

			EXAM_CHECK(v2.size() == 2);
			EXAM_CHECK(v2[0] == 'h');
			EXAM_CHECK(v2[1] == 'o');

			EXAM_CHECK((v1 == v2) == false);

			EXAM_CHECK((v1 < v2) == true);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_4)
		msev_int msevec_test_m4()
		{
			us::msevector<int> v(4);

			v[0] = 1;
			v[1] = 4;
			v[2] = 9;
			v[3] = 16;

			EXAM_CHECK(v.front() == 1);
			EXAM_CHECK(v.back() == 16);

			v.push_back(25);

			EXAM_CHECK(v.back() == 25);
			EXAM_CHECK(v.size() == 5);

			v.pop_back();

			EXAM_CHECK(v.back() == 16);
			EXAM_CHECK(v.size() == 4);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_5)
		msev_int msevec_test_m5()
		{
			int array[] = { 1, 4, 9, 16 };

#ifdef MSVC2010_COMPATIBLE
			us::msevector<int> v(array, array + 4);
#else /*MSVC2010_COMPATIBLE*/
			us::msevector<int> v = { 1, 4, 9, 16 };
#endif /*MSVC2010_COMPATIBLE*/

			EXAM_CHECK(v.size() == 4);

			EXAM_CHECK(v[0] == 1);
			EXAM_CHECK(v[1] == 4);
			EXAM_CHECK(v[2] == 9);
			EXAM_CHECK(v[3] == 16);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_6)
		msev_int msevec_test_m6()
		{
			int array[] = { 1, 4, 9, 16, 25, 36 };

#ifdef MSVC2010_COMPATIBLE
			us::msevector<int> v(array, array + 6);
#else /*MSVC2010_COMPATIBLE*/
			us::msevector<int> v = { 1, 4, 9, 16, 25, 36 };
#endif /*MSVC2010_COMPATIBLE*/
			us::msevector<int>::ipointer vit(v);
			//us::msevector<int>::iterator vit;

			EXAM_CHECK(v.size() == 6);
			EXAM_CHECK(v[0] == 1);
			EXAM_CHECK(v[1] == 4);
			EXAM_CHECK(v[2] == 9);
			EXAM_CHECK(v[3] == 16);
			EXAM_CHECK(v[4] == 25);
			EXAM_CHECK(v[5] == 36);

			//vit = v.erase(v.begin()); // Erase first element.
			vit.set_to_beginning();
			v.erase(vit); // Erase first element.
			vit.set_to_beginning();
			EXAM_CHECK(*vit == 4);

			EXAM_CHECK(v.size() == 5);
			EXAM_CHECK(v[0] == 4);
			EXAM_CHECK(v[1] == 9);
			EXAM_CHECK(v[2] == 16);
			EXAM_CHECK(v[3] == 25);
			EXAM_CHECK(v[4] == 36);

			//vit = v.erase(v.end() - 1); // Erase last element.
			vit.set_to_end_marker();
			vit.set_to_previous();
			v.erase(vit); // Erase last element.
			EXAM_CHECK(!(vit.points_to_an_item()));

			EXAM_CHECK(v.size() == 4);
			EXAM_CHECK(v[0] == 4);
			EXAM_CHECK(v[1] == 9);
			EXAM_CHECK(v[2] == 16);
			EXAM_CHECK(v[3] == 25);


			vit.set_to_beginning();
			vit.set_to_next();
			us::msevector<int>::ipointer vit2(v);
			vit2.set_to_end_marker();
			vit2.set_to_previous();
			v.erase(vit, vit2); // Erase all but first and last.
			//v.erase(v.begin() + 1, v.end() - 1); // Erase all but first and last.

			EXAM_CHECK(v.size() == 2);
			EXAM_CHECK(v[0] == 4);
			EXAM_CHECK(v[1] == 25);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_7)
		msev_int msevec_test_m7()
		{
			int array1[] = { 1, 4, 25 };
			int array2[] = { 9, 16 };

#ifdef MSVC2010_COMPATIBLE
			us::msevector<int> v(array1, array1 + 3);
#else /*MSVC2010_COMPATIBLE*/
			us::msevector<int> v = { 1, 4, 25 };
#endif /*MSVC2010_COMPATIBLE*/
			us::msevector<int>::ipointer vit(v);
			vit.set_to_beginning();
			v.insert_before(vit, 0); // Insert before first element.
			vit.set_to_beginning();
			//us::msevector<int>::iterator vit;
			//vit = v.insert(v.begin(), 0); // Insert before first element.
			EXAM_CHECK(*vit == 0);

			vit.set_to_end_marker();
			v.insert_before(vit, 36);  // Insert after last element.
			vit.set_to_previous();
			//vit = v.insert(v.end(), 36);  // Insert after last element.
			EXAM_CHECK(*vit == 36);

			EXAM_CHECK(v.size() == 5);
			EXAM_CHECK(v[0] == 0);
			EXAM_CHECK(v[1] == 1);
			EXAM_CHECK(v[2] == 4);
			EXAM_CHECK(v[3] == 25);
			EXAM_CHECK(v[4] == 36);

			// Insert contents of array2 before fourth element.
			//v.insert(v.begin() + 3, array2, array2 + 2);
			//v.insert(v.begin() + 3, { 9, 16 });
			vit.set_to_beginning();
			vit.advance(3);
			//for (int i = 0; i < 3; i += 1) { vit.set_to_next(); }
#ifdef MSVC2010_COMPATIBLE
			v.insert_before(vit, 9);
			v.insert_before(vit, 16);
#else /*MSVC2010_COMPATIBLE*/
			v.insert_before(vit, { 9, 16 });
#endif /*MSVC2010_COMPATIBLE*/

			EXAM_CHECK(v.size() == 7);

			EXAM_CHECK(v[0] == 0);
			EXAM_CHECK(v[1] == 1);
			EXAM_CHECK(v[2] == 4);
			EXAM_CHECK(v[3] == 9);
			EXAM_CHECK(v[4] == 16);
			EXAM_CHECK(v[5] == 25);
			EXAM_CHECK(v[6] == 36);

			v.clear();
			EXAM_CHECK(v.empty());

			vit.set_to_beginning();
			v.insert_before(vit, 5, 10);
			//v.insert(v.begin(), 5, 10);
			EXAM_CHECK(v.size() == 5);
			EXAM_CHECK(v[0] == 10);
			EXAM_CHECK(v[1] == 10);
			EXAM_CHECK(v[2] == 10);
			EXAM_CHECK(v[3] == 10);
			EXAM_CHECK(v[4] == 10);

			/*
			{
			us::msevector<float> vf(2.0f, 3.0f);
			EXAM_CHECK( vf.size() == 2 );
			EXAM_CHECK( vf.front() == 3.0f );
			EXAM_CHECK( vf.back() == 3.0f );
			}
			*/

			return EXAM_RESULT;
		}


		//int EXAM_IMPL(vector_test::vec_test_1)
		msev_int msevec_test_i1()
		{
			ivector<int> v1; // Empty vector of integers.

			EXAM_CHECK(v1.empty() == true);
			EXAM_CHECK(v1.size() == 0);

			// EXAM_CHECK( v1.max_size() == INT_MAX / sizeof(int) );
			// cout << "max_size = " << v1.max_size() << endl;
			v1.push_back(42); // Add an integer to the vector.

			EXAM_CHECK(v1.size() == 1);

			EXAM_CHECK(v1[0] == 42);

			{
				ivector<ivector<int> > vect(10);
				//ivector<ivector<int> >::iterator it(vect.begin()), end(vect.end());
				ivector<ivector<int> >::cipointer it(vect);
				for (; it.points_to_an_item(); it.set_to_next()) {
					EXAM_CHECK((*it).empty());
					EXAM_CHECK((*it).size() == 0);
					EXAM_CHECK((*it).capacity() == 0);
					//EXAM_CHECK((*it).begin() == (*it).end());
				}
			}

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_2)
		msev_int msevec_test_i2()
		{
			ivector<double> v1; // Empty vector of doubles.
			v1.push_back(32.1);
			v1.push_back(40.5);
			ivector<double> v2; // Another empty vector of doubles.
			v2.push_back(3.56);

			EXAM_CHECK(v1.size() == 2);
			EXAM_CHECK(v1[0] == 32.1);
			EXAM_CHECK(v1[1] == 40.5);

			EXAM_CHECK(v2.size() == 1);
			EXAM_CHECK(v2[0] == 3.56);
			msev_size_t v1Cap = v1.capacity();
			msev_size_t v2Cap = v2.capacity();

			v1.swap(v2); // Swap the vector's contents.

			EXAM_CHECK(v1.size() == 1);
			EXAM_CHECK(v1.capacity() == v2Cap);
			EXAM_CHECK(v1[0] == 3.56);

			EXAM_CHECK(v2.size() == 2);
			EXAM_CHECK(v2.capacity() == v1Cap);
			EXAM_CHECK(v2[0] == 32.1);
			EXAM_CHECK(v2[1] == 40.5);

			v2 = v1; // Assign one vector to another.

			EXAM_CHECK(v2.size() == 1);
			EXAM_CHECK(v2[0] == 3.56);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_3)
		msev_int msevec_test_i3()
		{
			typedef ivector<char> vec_type;

			vec_type v1; // Empty vector of characters.
			v1.push_back('h');
			v1.push_back('i');

			EXAM_CHECK(v1.size() == 2);
			EXAM_CHECK(v1[0] == 'h');
			EXAM_CHECK(v1[1] == 'i');

			vec_type::cipointer it1(v1);
			vec_type::cipointer it2(v1);
			it2.set_to_end_marker();
			vec_type v2(it1, it2);
			v2[1] = 'o'; // Replace second character.

			EXAM_CHECK(v2.size() == 2);
			EXAM_CHECK(v2[0] == 'h');
			EXAM_CHECK(v2[1] == 'o');

			EXAM_CHECK((v1 == v2) == false);

			EXAM_CHECK((v1 < v2) == true);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_4)
		msev_int msevec_test_i4()
		{
			ivector<int> v(4);

			v[0] = 1;
			v[1] = 4;
			v[2] = 9;
			v[3] = 16;

			EXAM_CHECK(v.front() == 1);
			EXAM_CHECK(v.back() == 16);

			v.push_back(25);

			EXAM_CHECK(v.back() == 25);
			EXAM_CHECK(v.size() == 5);

			v.pop_back();

			EXAM_CHECK(v.back() == 16);
			EXAM_CHECK(v.size() == 4);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_5)
		msev_int msevec_test_i5()
		{
			int array[] = { 1, 4, 9, 16 };

#ifdef MSVC2010_COMPATIBLE
			ivector<int> v(array, array + 4);
#else /*MSVC2010_COMPATIBLE*/
			ivector<int> v = { 1, 4, 9, 16 };
#endif /*MSVC2010_COMPATIBLE*/

			EXAM_CHECK(v.size() == 4);

			EXAM_CHECK(v[0] == 1);
			EXAM_CHECK(v[1] == 4);
			EXAM_CHECK(v[2] == 9);
			EXAM_CHECK(v[3] == 16);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_6)
		msev_int msevec_test_i6()
		{
			int array[] = { 1, 4, 9, 16, 25, 36 };

#ifdef MSVC2010_COMPATIBLE
			ivector<int> v(array, array + 6);
#else /*MSVC2010_COMPATIBLE*/
			ivector<int> v = { 1, 4, 9, 16, 25, 36 };
#endif /*MSVC2010_COMPATIBLE*/
			ivector<int>::ipointer vit(v);
			//ivector<int>::iterator vit;

			EXAM_CHECK(v.size() == 6);
			EXAM_CHECK(v[0] == 1);
			EXAM_CHECK(v[1] == 4);
			EXAM_CHECK(v[2] == 9);
			EXAM_CHECK(v[3] == 16);
			EXAM_CHECK(v[4] == 25);
			EXAM_CHECK(v[5] == 36);

			//vit = v.erase(v.begin()); // Erase first element.
			vit.set_to_beginning();
			v.erase(vit); // Erase first element.
			vit.set_to_beginning();
			EXAM_CHECK(*vit == 4);

			EXAM_CHECK(v.size() == 5);
			EXAM_CHECK(v[0] == 4);
			EXAM_CHECK(v[1] == 9);
			EXAM_CHECK(v[2] == 16);
			EXAM_CHECK(v[3] == 25);
			EXAM_CHECK(v[4] == 36);

			//vit = v.erase(v.end() - 1); // Erase last element.
			vit.set_to_end_marker();
			vit.set_to_previous();
			v.erase(vit); // Erase last element.
			EXAM_CHECK(!(vit.points_to_an_item()));

			EXAM_CHECK(v.size() == 4);
			EXAM_CHECK(v[0] == 4);
			EXAM_CHECK(v[1] == 9);
			EXAM_CHECK(v[2] == 16);
			EXAM_CHECK(v[3] == 25);


			vit.set_to_beginning();
			vit.set_to_next();
			ivector<int>::ipointer vit2(v);
			vit2.set_to_end_marker();
			vit2.set_to_previous();
			v.erase(vit, vit2); // Erase all but first and last.
			//v.erase(v.begin() + 1, v.end() - 1); // Erase all but first and last.

			EXAM_CHECK(v.size() == 2);
			EXAM_CHECK(v[0] == 4);
			EXAM_CHECK(v[1] == 25);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_7)
		msev_int msevec_test_i7()
		{
			int array1[] = { 1, 4, 25 };
			int array2[] = { 9, 16 };

#ifdef MSVC2010_COMPATIBLE
			ivector<int> v(array1, array1 + 3);
#else /*MSVC2010_COMPATIBLE*/
			ivector<int> v = { 1, 4, 25 };
#endif /*MSVC2010_COMPATIBLE*/
			ivector<int>::ipointer vit(v);
			vit.set_to_beginning();
			v.insert_before(vit, 0); // Insert before first element.
			vit.set_to_beginning();
			//ivector<int>::iterator vit;
			//vit = v.insert(v.begin(), 0); // Insert before first element.
			EXAM_CHECK(*vit == 0);

			vit.set_to_end_marker();
			v.insert_before(vit, 36);  // Insert after last element.
			vit.set_to_previous();
			//vit = v.insert(v.end(), 36);  // Insert after last element.
			EXAM_CHECK(*vit == 36);

			EXAM_CHECK(v.size() == 5);
			EXAM_CHECK(v[0] == 0);
			EXAM_CHECK(v[1] == 1);
			EXAM_CHECK(v[2] == 4);
			EXAM_CHECK(v[3] == 25);
			EXAM_CHECK(v[4] == 36);

			// Insert contents of array2 before fourth element.
			//v.insert(v.begin() + 3, array2, array2 + 2);
			//v.insert(v.begin() + 3, { 9, 16 });
			vit.set_to_beginning();
			vit.advance(3);
			//for (int i = 0; i < 3; i += 1) { vit.set_to_next(); }
#ifdef MSVC2010_COMPATIBLE
			v.insert_before(vit, 9);
			v.insert_before(vit, 16);
#else /*MSVC2010_COMPATIBLE*/
			v.insert_before(vit, { 9, 16 });
#endif /*MSVC2010_COMPATIBLE*/

			EXAM_CHECK(v.size() == 7);

			EXAM_CHECK(v[0] == 0);
			EXAM_CHECK(v[1] == 1);
			EXAM_CHECK(v[2] == 4);
			EXAM_CHECK(v[3] == 9);
			EXAM_CHECK(v[4] == 16);
			EXAM_CHECK(v[5] == 25);
			EXAM_CHECK(v[6] == 36);

			std::reverse(v.begin(), v.end());

			EXAM_CHECK(v.size() == 7);
			EXAM_CHECK(v[6] == 0);
			EXAM_CHECK(v[5] == 1);
			EXAM_CHECK(v[4] == 4);
			EXAM_CHECK(v[3] == 9);
			EXAM_CHECK(v[2] == 16);
			EXAM_CHECK(v[1] == 25);
			EXAM_CHECK(v[0] == 36);

			std::sort(v.begin(), v.end());

			EXAM_CHECK(v.size() == 7);
			EXAM_CHECK(v[0] == 0);
			EXAM_CHECK(v[1] == 1);
			EXAM_CHECK(v[2] == 4);
			EXAM_CHECK(v[3] == 9);
			EXAM_CHECK(v[4] == 16);
			EXAM_CHECK(v[5] == 25);
			EXAM_CHECK(v[6] == 36);

			v.clear();
			EXAM_CHECK(v.empty());

			vit.set_to_beginning();
			v.insert_before(vit, 5, 10);
			//v.insert(v.begin(), 5, 10);
			EXAM_CHECK(v.size() == 5);
			EXAM_CHECK(v[0] == 10);
			EXAM_CHECK(v[1] == 10);
			EXAM_CHECK(v[2] == 10);
			EXAM_CHECK(v[3] == 10);
			EXAM_CHECK(v[4] == 10);

			/*
			{
			ivector<float> vf(2.0f, 3.0f);
			EXAM_CHECK( vf.size() == 2 );
			EXAM_CHECK( vf.front() == 3.0f );
			EXAM_CHECK( vf.back() == 3.0f );
			}
			*/

			return EXAM_RESULT;
		}

#define STDVECTOR_IS_READY
#ifdef STDVECTOR_IS_READY
		//int EXAM_IMPL(vector_test::vec_test_1)
		msev_int msevec_test_s1()
		{
			mstd::vector<int> v1; // Empty vector of integers.

			EXAM_CHECK(v1.empty() == true);
			EXAM_CHECK(v1.size() == 0);

			// EXAM_CHECK( v1.max_size() == INT_MAX / sizeof(int) );
			// cout << "max_size = " << v1.max_size() << endl;
			v1.push_back(42); // Add an integer to the vector.

			EXAM_CHECK(v1.size() == 1);

			EXAM_CHECK(v1[0] == 42);

			{
				mstd::vector<mstd::vector<int> > vect(10);
				mstd::vector<mstd::vector<int> >::iterator it(vect.begin()), end(vect.end());
				//mstd::vector<mstd::vector<int> >::cipointer it(vect);
				for (; vect.end() != it; it++) {
					EXAM_CHECK((*it).empty());
					EXAM_CHECK((*it).size() == 0);
					EXAM_CHECK((*it).capacity() == 0);
					EXAM_CHECK((*it).begin() == (*it).end());
				}
			}

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_2)
		msev_int msevec_test_s2()
		{
			mstd::vector<double> v1; // Empty vector of doubles.
			v1.push_back(32.1);
			v1.push_back(40.5);
			mstd::vector<double> v2; // Another empty vector of doubles.
			v2.push_back(3.56);

			EXAM_CHECK(v1.size() == 2);
			EXAM_CHECK(v1[0] == 32.1);
			EXAM_CHECK(v1[1] == 40.5);

			EXAM_CHECK(v2.size() == 1);
			EXAM_CHECK(v2[0] == 3.56);
			msev_size_t v1Cap = v1.capacity();
			msev_size_t v2Cap = v2.capacity();

			v1.swap(v2); // Swap the vector's contents.

			EXAM_CHECK(v1.size() == 1);
			EXAM_CHECK(v1.capacity() == v2Cap);
			EXAM_CHECK(v1[0] == 3.56);

			EXAM_CHECK(v2.size() == 2);
			EXAM_CHECK(v2.capacity() == v1Cap);
			EXAM_CHECK(v2[0] == 32.1);
			EXAM_CHECK(v2[1] == 40.5);

			v2 = v1; // Assign one vector to another.

			EXAM_CHECK(v2.size() == 1);
			EXAM_CHECK(v2[0] == 3.56);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_3)
		msev_int msevec_test_s3()
		{
			typedef mstd::vector<char> vec_type;

			vec_type v1; // Empty vector of characters.
			v1.push_back('h');
			v1.push_back('i');

			EXAM_CHECK(v1.size() == 2);
			EXAM_CHECK(v1[0] == 'h');
			EXAM_CHECK(v1[1] == 'i');

			vec_type v2(v1.begin(), v1.end());
			v2[1] = 'o'; // Replace second character.

			EXAM_CHECK(v2.size() == 2);
			EXAM_CHECK(v2[0] == 'h');
			EXAM_CHECK(v2[1] == 'o');

			EXAM_CHECK((v1 == v2) == false);

			EXAM_CHECK((v1 < v2) == true);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_4)
		msev_int msevec_test_s4()
		{
			mstd::vector<int> v(4);

			v[0] = 1;
			v[1] = 4;
			v[2] = 9;
			v[3] = 16;

			EXAM_CHECK(v.front() == 1);
			EXAM_CHECK(v.back() == 16);

			v.push_back(25);

			EXAM_CHECK(v.back() == 25);
			EXAM_CHECK(v.size() == 5);

			v.pop_back();

			EXAM_CHECK(v.back() == 16);
			EXAM_CHECK(v.size() == 4);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_5)
		msev_int msevec_test_s5()
		{
			int array[] = { 1, 4, 9, 16 };

			mstd::vector<int> v(array, array + 4);
			//mstd::vector<int> v = { 1, 4, 9, 16 };

			EXAM_CHECK(v.size() == 4);

			EXAM_CHECK(v[0] == 1);
			EXAM_CHECK(v[1] == 4);
			EXAM_CHECK(v[2] == 9);
			EXAM_CHECK(v[3] == 16);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_6)
		msev_int msevec_test_s6()
		{
			int array[] = { 1, 4, 9, 16, 25, 36 };

			mstd::vector<int> v(array, array + 6);
			//mstd::vector<int> v = { 1, 4, 9, 16, 25, 36 };
			//mstd::vector<int>::ipointer vit(v);
			mstd::vector<int>::iterator vit;

			EXAM_CHECK(v.size() == 6);
			EXAM_CHECK(v[0] == 1);
			EXAM_CHECK(v[1] == 4);
			EXAM_CHECK(v[2] == 9);
			EXAM_CHECK(v[3] == 16);
			EXAM_CHECK(v[4] == 25);
			EXAM_CHECK(v[5] == 36);

			vit = v.erase(v.begin()); // Erase first element.
			//vit.set_to_beginning();
			//v.erase(vit); // Erase first element.
			//vit.set_to_beginning();
			EXAM_CHECK(*vit == 4);

			EXAM_CHECK(v.size() == 5);
			EXAM_CHECK(v[0] == 4);
			EXAM_CHECK(v[1] == 9);
			EXAM_CHECK(v[2] == 16);
			EXAM_CHECK(v[3] == 25);
			EXAM_CHECK(v[4] == 36);

			vit = v.erase(v.end() - 1); // Erase last element.
			//vit.set_to_end_marker();
			//vit.set_to_previous();
			//v.erase(vit); // Erase last element.
			EXAM_CHECK(vit == v.end());
			//EXAM_CHECK(!(vit.points_to_an_item()));

			EXAM_CHECK(v.size() == 4);
			EXAM_CHECK(v[0] == 4);
			EXAM_CHECK(v[1] == 9);
			EXAM_CHECK(v[2] == 16);
			EXAM_CHECK(v[3] == 25);


			//vit.set_to_beginning();
			//vit.set_to_next();
			//mstd::vector<int>::ipointer vit2(v);
			//vit2.set_to_end_marker();
			//vit2.set_to_previous();
			//v.erase(vit, vit2); // Erase all but first and last.
			v.erase(v.begin() + 1, v.end() - 1); // Erase all but first and last.

			EXAM_CHECK(v.size() == 2);
			EXAM_CHECK(v[0] == 4);
			EXAM_CHECK(v[1] == 25);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_7)
		msev_int msevec_test_s7()
		{
			int array1[] = { 1, 4, 25 };
			int array2[] = { 9, 16 };

			mstd::vector<int> v(array1, array1 + 3);
			//mstd::vector<int> v = { 1, 4, 25 };
			//mstd::vector<int>::ipointer vit(v);
			//vit.set_to_beginning();
			//v.insert_before(vit, 0); // Insert before first element.
			//vit.set_to_beginning();
			mstd::vector<int>::iterator vit;
			vit = v.insert(v.begin(), 0); // Insert before first element.
			EXAM_CHECK(*vit == 0);

			//vit.set_to_end_marker();
			//v.insert_before(vit, 36);  // Insert after last element.
			//vit.set_to_previous();
			vit = v.insert(v.end(), 36);  // Insert after last element.
			EXAM_CHECK(*vit == 36);

			EXAM_CHECK(v.size() == 5);
			EXAM_CHECK(v[0] == 0);
			EXAM_CHECK(v[1] == 1);
			EXAM_CHECK(v[2] == 4);
			EXAM_CHECK(v[3] == 25);
			EXAM_CHECK(v[4] == 36);

			// Insert contents of array2 before fourth element.
			v.insert(v.begin() + 3, array2, array2 + 2);
			////v.insert(v.begin() + 3, { 9, 16 });
			//vit.set_to_beginning();
			//vit.advance(3);
			////for (int i = 0; i < 3; i += 1) { vit.set_to_next(); }
			//v.insert_before(vit, { 9, 16 });

			EXAM_CHECK(v.size() == 7);

			EXAM_CHECK(v[0] == 0);
			EXAM_CHECK(v[1] == 1);
			EXAM_CHECK(v[2] == 4);
			EXAM_CHECK(v[3] == 9);
			EXAM_CHECK(v[4] == 16);
			EXAM_CHECK(v[5] == 25);
			EXAM_CHECK(v[6] == 36);

			std::reverse(v.begin(), v.end());

			EXAM_CHECK(v.size() == 7);
			EXAM_CHECK(v[6] == 0);
			EXAM_CHECK(v[5] == 1);
			EXAM_CHECK(v[4] == 4);
			EXAM_CHECK(v[3] == 9);
			EXAM_CHECK(v[2] == 16);
			EXAM_CHECK(v[1] == 25);
			EXAM_CHECK(v[0] == 36);

			std::sort(v.begin(), v.end());

			EXAM_CHECK(v.size() == 7);
			EXAM_CHECK(v[0] == 0);
			EXAM_CHECK(v[1] == 1);
			EXAM_CHECK(v[2] == 4);
			EXAM_CHECK(v[3] == 9);
			EXAM_CHECK(v[4] == 16);
			EXAM_CHECK(v[5] == 25);
			EXAM_CHECK(v[6] == 36);

			v.clear();
			EXAM_CHECK(v.empty());

			//vit.set_to_beginning();
			//v.insert_before(vit, 5, 10);
			v.insert(v.begin(), 5, 10);
			EXAM_CHECK(v.size() == 5);
			EXAM_CHECK(v[0] == 10);
			EXAM_CHECK(v[1] == 10);
			EXAM_CHECK(v[2] == 10);
			EXAM_CHECK(v[3] == 10);
			EXAM_CHECK(v[4] == 10);

			/*
			{
			mstd::vector<float> vf(2.0f, 3.0f);
			EXAM_CHECK( vf.size() == 2 );
			EXAM_CHECK( vf.front() == 3.0f );
			EXAM_CHECK( vf.back() == 3.0f );
			}
			*/

			return EXAM_RESULT;
		}
#endif /*STDVECTOR_IS_READY*/


		//int EXAM_IMPL(vector_test::vec_test_1)
		msev_int msevec_test_ss1()
		{
			mse::us::msevector<int> v1; // Empty vector of integers.

			EXAM_CHECK(v1.empty() == true);
			EXAM_CHECK(v1.size() == 0);

			// EXAM_CHECK( v1.max_size() == INT_MAX / sizeof(int) );
			// cout << "max_size = " << v1.max_size() << endl;
			v1.push_back(42); // Add an integer to the vector.

			EXAM_CHECK(v1.size() == 1);

			EXAM_CHECK(v1[0] == 42);

			{
				mse::us::msevector<mse::us::msevector<int> > vect(10);
				mse::us::msevector<mse::us::msevector<int> >::ss_iterator_type it(vect.ss_begin()), end(vect.ss_end());
				//mse::us::msevector<mse::us::msevector<int> >::cipointer it(vect);
				for (; vect.ss_end() != it; it++) {
					EXAM_CHECK((*it).empty());
					EXAM_CHECK((*it).size() == 0);
					EXAM_CHECK((*it).capacity() == 0);
					EXAM_CHECK((*it).ss_begin() == (*it).ss_end());
				}
			}

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_2)
		msev_int msevec_test_ss2()
		{
			mse::us::msevector<double> v1; // Empty vector of doubles.
			v1.push_back(32.1);
			v1.push_back(40.5);
			mse::us::msevector<double> v2; // Another empty vector of doubles.
			v2.push_back(3.56);

			EXAM_CHECK(v1.size() == 2);
			EXAM_CHECK(v1[0] == 32.1);
			EXAM_CHECK(v1[1] == 40.5);

			EXAM_CHECK(v2.size() == 1);
			EXAM_CHECK(v2[0] == 3.56);
			msev_size_t v1Cap = v1.capacity();
			msev_size_t v2Cap = v2.capacity();

			v1.swap(v2); // Swap the vector's contents.

			EXAM_CHECK(v1.size() == 1);
			EXAM_CHECK(v1.capacity() == v2Cap);
			EXAM_CHECK(v1[0] == 3.56);

			EXAM_CHECK(v2.size() == 2);
			EXAM_CHECK(v2.capacity() == v1Cap);
			EXAM_CHECK(v2[0] == 32.1);
			EXAM_CHECK(v2[1] == 40.5);

			v2 = v1; // Assign one vector to another.

			EXAM_CHECK(v2.size() == 1);
			EXAM_CHECK(v2[0] == 3.56);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_3)
		msev_int msevec_test_ss3()
		{
			typedef mse::us::msevector<char> vec_type;

			vec_type v1; // Empty vector of characters.
			v1.push_back('h');
			v1.push_back('i');

			EXAM_CHECK(v1.size() == 2);
			EXAM_CHECK(v1[0] == 'h');
			EXAM_CHECK(v1[1] == 'i');

			vec_type v2(v1.ss_begin(), v1.ss_end());
			v2[1] = 'o'; // Replace second character.

			EXAM_CHECK(v2.size() == 2);
			EXAM_CHECK(v2[0] == 'h');
			EXAM_CHECK(v2[1] == 'o');

			EXAM_CHECK((v1 == v2) == false);

			EXAM_CHECK((v1 < v2) == true);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_4)
		msev_int msevec_test_ss4()
		{
			mse::us::msevector<int> v(4);

			v[0] = 1;
			v[1] = 4;
			v[2] = 9;
			v[3] = 16;

			EXAM_CHECK(v.front() == 1);
			EXAM_CHECK(v.back() == 16);

			v.push_back(25);

			EXAM_CHECK(v.back() == 25);
			EXAM_CHECK(v.size() == 5);

			v.pop_back();

			EXAM_CHECK(v.back() == 16);
			EXAM_CHECK(v.size() == 4);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_5)
		msev_int msevec_test_ss5()
		{
			int array[] = { 1, 4, 9, 16 };

			mse::us::msevector<int> v(array, array + 4);
			//mse::us::msevector<int> v = { 1, 4, 9, 16 };

			EXAM_CHECK(v.size() == 4);

			EXAM_CHECK(v[0] == 1);
			EXAM_CHECK(v[1] == 4);
			EXAM_CHECK(v[2] == 9);
			EXAM_CHECK(v[3] == 16);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_6)
		msev_int msevec_test_ss6()
		{
			int array[] = { 1, 4, 9, 16, 25, 36 };

			mse::us::msevector<int> v(array, array + 6);
			//mse::us::msevector<int> v = { 1, 4, 9, 16, 25, 36 };
			//mse::us::msevector<int>::ipointer vit(v);
			mse::us::msevector<int>::ss_iterator_type vit;

			EXAM_CHECK(v.size() == 6);
			EXAM_CHECK(v[0] == 1);
			EXAM_CHECK(v[1] == 4);
			EXAM_CHECK(v[2] == 9);
			EXAM_CHECK(v[3] == 16);
			EXAM_CHECK(v[4] == 25);
			EXAM_CHECK(v[5] == 36);

			vit = v.erase(v.ss_begin()); // Erase first element.
									  //vit.set_to_beginning();
									  //v.erase(vit); // Erase first element.
									  //vit.set_to_beginning();
			EXAM_CHECK(*vit == 4);

			EXAM_CHECK(v.size() == 5);
			EXAM_CHECK(v[0] == 4);
			EXAM_CHECK(v[1] == 9);
			EXAM_CHECK(v[2] == 16);
			EXAM_CHECK(v[3] == 25);
			EXAM_CHECK(v[4] == 36);

			vit = v.erase(v.ss_end() - 1); // Erase last element.
										//vit.set_to_end_marker();
										//vit.set_to_previous();
										//v.erase(vit); // Erase last element.
			EXAM_CHECK(vit == v.ss_end());
			//EXAM_CHECK(!(vit.points_to_an_item()));

			EXAM_CHECK(v.size() == 4);
			EXAM_CHECK(v[0] == 4);
			EXAM_CHECK(v[1] == 9);
			EXAM_CHECK(v[2] == 16);
			EXAM_CHECK(v[3] == 25);


			//vit.set_to_beginning();
			//vit.set_to_next();
			//mse::us::msevector<int>::ipointer vit2(v);
			//vit2.set_to_end_marker();
			//vit2.set_to_previous();
			//v.erase(vit, vit2); // Erase all but first and last.
			v.erase(v.ss_begin() + 1, v.ss_end() - 1); // Erase all but first and last.

			EXAM_CHECK(v.size() == 2);
			EXAM_CHECK(v[0] == 4);
			EXAM_CHECK(v[1] == 25);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_7)
		msev_int msevec_test_ss7()
		{
			int array1[] = { 1, 4, 25 };
			int array2[] = { 9, 16 };

			mse::us::msevector<int> v(array1, array1 + 3);
			//mse::us::msevector<int> v = { 1, 4, 25 };
			//mse::us::msevector<int>::ipointer vit(v);
			//vit.set_to_beginning();
			//v.insert_before(vit, 0); // Insert before first element.
			//vit.set_to_beginning();
			mse::us::msevector<int>::ss_iterator_type vit;
			vit = v.insert(v.ss_begin(), 0); // Insert before first element.
			EXAM_CHECK(*vit == 0);

			//vit.set_to_end_marker();
			//v.insert_before(vit, 36);  // Insert after last element.
			//vit.set_to_previous();
			vit = v.insert(v.ss_end(), 36);  // Insert after last element.
			EXAM_CHECK(*vit == 36);

			EXAM_CHECK(v.size() == 5);
			EXAM_CHECK(v[0] == 0);
			EXAM_CHECK(v[1] == 1);
			EXAM_CHECK(v[2] == 4);
			EXAM_CHECK(v[3] == 25);
			EXAM_CHECK(v[4] == 36);

			// Insert contents of array2 before fourth element.
			v.insert(v.ss_begin() + 3, (int *)array2, (int *)(array2 + 2));
			////v.insert(v.ss_begin() + 3, { 9, 16 });
			//vit.set_to_beginning();
			//vit.advance(3);
			////for (int i = 0; i < 3; i += 1) { vit.set_to_next(); }
			//v.insert_before(vit, { 9, 16 });

			EXAM_CHECK(v.size() == 7);

			EXAM_CHECK(v[0] == 0);
			EXAM_CHECK(v[1] == 1);
			EXAM_CHECK(v[2] == 4);
			EXAM_CHECK(v[3] == 9);
			EXAM_CHECK(v[4] == 16);
			EXAM_CHECK(v[5] == 25);
			EXAM_CHECK(v[6] == 36);

			std::reverse(v.ss_begin(), v.ss_end());

			EXAM_CHECK(v.size() == 7);
			EXAM_CHECK(v[6] == 0);
			EXAM_CHECK(v[5] == 1);
			EXAM_CHECK(v[4] == 4);
			EXAM_CHECK(v[3] == 9);
			EXAM_CHECK(v[2] == 16);
			EXAM_CHECK(v[1] == 25);
			EXAM_CHECK(v[0] == 36);

			std::sort(v.ss_begin(), v.ss_end());

			EXAM_CHECK(v.size() == 7);
			EXAM_CHECK(v[0] == 0);
			EXAM_CHECK(v[1] == 1);
			EXAM_CHECK(v[2] == 4);
			EXAM_CHECK(v[3] == 9);
			EXAM_CHECK(v[4] == 16);
			EXAM_CHECK(v[5] == 25);
			EXAM_CHECK(v[6] == 36);

			v.clear();
			EXAM_CHECK(v.empty());

			//vit.set_to_beginning();
			//v.insert_before(vit, 5, 10);
			v.insert(v.ss_begin(), 5, 10);
			EXAM_CHECK(v.size() == 5);
			EXAM_CHECK(v[0] == 10);
			EXAM_CHECK(v[1] == 10);
			EXAM_CHECK(v[2] == 10);
			EXAM_CHECK(v[3] == 10);
			EXAM_CHECK(v[4] == 10);

			/*
			{
			mse::us::msevector<float> vf(2.0f, 3.0f);
			EXAM_CHECK( vf.size() == 2 );
			EXAM_CHECK( vf.front() == 3.0f );
			EXAM_CHECK( vf.back() == 3.0f );
			}
			*/

			{
				v.assign({3, 1, 4, 1, 5});
				mse::us::msevector<int> v2(v.begin(), v.end());
				std::sort(v2.begin(), v2.end());
				std::reverse(v2.begin(), v2.end());
				v2.assign(v.begin(), v.end());
				auto v2_iter1 = v2.insert(v2.cbegin() + 1, v.begin(), v.end());
				v2_iter1 = v2.insert(v2_iter1 + 1, {10, 11});
				v2_iter1 = v2.erase(v2_iter1 - 1);
				v2.erase(v2_iter1 + 1, v2_iter1 + 3);
				EXAM_CHECK(v2.size() == 9);
				EXAM_CHECK(v2[0] == 3);
				EXAM_CHECK(v2[1] == 10);
				EXAM_CHECK(v2[2] == 4);

				v.assign({ 3, 1, 4, 1, 5 });
				mse::us::msevector<int> v3(v.ss_begin(), v.ss_end());
				std::sort(v3.ss_begin(), v3.ss_end());
				std::reverse(v3.ss_begin(), v3.ss_end());
				v3.assign(v.ss_begin(), v.ss_end());
				auto v3_iter1 = v3.insert(v3.ss_cbegin() + 1, v.ss_begin(), v.ss_end());
				v3_iter1 = v3.insert(v3_iter1 + 1, { 10, 11 });
				v3_iter1 = v3.erase(v3_iter1 - 1);
				v3.erase(v3_iter1 + 1, v3_iter1 + 3);
				EXAM_CHECK(v3.size() == 9);
				EXAM_CHECK(v3[0] == 3);
				EXAM_CHECK(v3[1] == 10);
				EXAM_CHECK(v3[2] == 4);

#ifdef MSEREGISTERED_H_
				v.assign({ 3, 1, 4, 1, 5 });
				mse::TRegisteredObj<mse::nii_vector<int>> v4(v.ss_begin(), v.ss_end());
				mse::TRegisteredNotNullPointer<mse::nii_vector<int>> v4_nnregptr = &v4;
				std::sort(v4.ss_begin(v4_nnregptr), v4.ss_end(v4_nnregptr));
				std::reverse(v4.ss_begin(v4_nnregptr), v4.ss_end(v4_nnregptr));
				v4.assign(v.ss_begin(), v.ss_end());
				auto v4_iter1 = v4.insert(v4_nnregptr, v4.ss_cbegin(v4_nnregptr) + 1, v.ss_begin(), v.ss_end());
				typedef decltype(mse::make_begin_const_iterator(v4_nnregptr)) const_iter_type1;
				v4_iter1 = v4.insert(v4_nnregptr, const_iter_type1(v4_iter1) + 1, { 10, 11 });
				v4_iter1 = v4.insert(v4_nnregptr, v4_iter1 + 1, { 20, 21 });
				v4_iter1 = v4.erase(v4_nnregptr, const_iter_type1(v4_iter1) - 1);
				v4.erase(v4_nnregptr, const_iter_type1(v4_iter1) + 1, const_iter_type1(v4_iter1) + 3);
				EXAM_CHECK(v4.size() == 9);
				EXAM_CHECK(v4[0] == 3);
				EXAM_CHECK(v4[1] == 10);
				EXAM_CHECK(v4[2] == 4);
#endif // MSEREGISTERED_H_
			}

			return EXAM_RESULT;
		}


		//int EXAM_IMPL(vector_test::vec_test_1)
		msev_int msevec_test_mt1()
		{
			mse::TXScopeObj<mse::nii_vector<int> > v1; // Empty vector of integers.

			EXAM_CHECK(v1.empty() == true);
			EXAM_CHECK(v1.size() == 0);

			// EXAM_CHECK( v1.max_size() == INT_MAX / sizeof(int) );
			// cout << "max_size = " << v1.max_size() << endl;
			v1.push_back(42); // Add an integer to the vector.

			EXAM_CHECK(v1.size() == 1);

			EXAM_CHECK(v1[0] == 42);

			{
				mse::TXScopeObj<mse::nii_vector<mse::nii_vector<int> > > vect(10);
				//mse::TXScopeObj<mse::nii_vector<mse::nii_vector<int> > >::iterator it(vect.begin()), end(vect.end());
				auto it = mse::make_xscope_begin_iterator(&vect);
				auto end = mse::make_xscope_end_iterator(&vect);
				for (; end != it; it++) {
					EXAM_CHECK((*it).empty());
					EXAM_CHECK((*it).size() == 0);
					EXAM_CHECK((*it).capacity() == 0);
					//EXAM_CHECK((*it).begin() == (*it).end());
					EXAM_CHECK(mse::make_xscope_begin_iterator(mse::xscope_pointer(it)) == mse::make_xscope_end_iterator(mse::xscope_pointer(it)));
				}
			}

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_2)
		msev_int msevec_test_mt2()
		{
			mse::TXScopeObj<mse::nii_vector<double> > v1; // Empty vector of doubles.
			v1.push_back(32.1);
			v1.push_back(40.5);
			mse::TXScopeObj<mse::nii_vector<double> > v2; // Another empty vector of doubles.
			v2.push_back(3.56);

			EXAM_CHECK(v1.size() == 2);
			EXAM_CHECK(v1[0] == 32.1);
			EXAM_CHECK(v1[1] == 40.5);

			EXAM_CHECK(v2.size() == 1);
			EXAM_CHECK(v2[0] == 3.56);
			msev_size_t v1Cap = v1.capacity();
			msev_size_t v2Cap = v2.capacity();

			v1.swap(v2); // Swap the vector's contents.

			EXAM_CHECK(v1.size() == 1);
			EXAM_CHECK(v1.capacity() == v2Cap);
			EXAM_CHECK(v1[0] == 3.56);

			EXAM_CHECK(v2.size() == 2);
			EXAM_CHECK(v2.capacity() == v1Cap);
			EXAM_CHECK(v2[0] == 32.1);
			EXAM_CHECK(v2[1] == 40.5);

			v2 = v1; // Assign one vector to another.

			EXAM_CHECK(v2.size() == 1);
			EXAM_CHECK(v2[0] == 3.56);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_3)
		msev_int msevec_test_mt3()
		{
			typedef mse::nii_vector<char> vec_type;

			mse::TXScopeObj<vec_type> v1; // Empty vector of characters.
			v1.push_back('h');
			v1.push_back('i');

			EXAM_CHECK(v1.size() == 2);
			EXAM_CHECK(v1[0] == 'h');
			EXAM_CHECK(v1[1] == 'i');

			auto it1 = mse::make_xscope_begin_const_iterator(&v1);
			auto it2 = mse::make_xscope_end_const_iterator(&v1);
			mse::TXScopeObj<vec_type> v2(it1, it2);
			v2[1] = 'o'; // Replace second character.

			EXAM_CHECK(v2.size() == 2);
			EXAM_CHECK(v2[0] == 'h');
			EXAM_CHECK(v2[1] == 'o');

			EXAM_CHECK((v1 == v2) == false);

			EXAM_CHECK((v1 < v2) == true);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_4)
		msev_int msevec_test_mt4()
		{
			mse::TXScopeObj<mse::nii_vector<int> > v(4);

			v[0] = 1;
			v[1] = 4;
			v[2] = 9;
			v[3] = 16;

			EXAM_CHECK(v.front() == 1);
			EXAM_CHECK(v.back() == 16);

			v.push_back(25);

			EXAM_CHECK(v.back() == 25);
			EXAM_CHECK(v.size() == 5);

			v.pop_back();

			EXAM_CHECK(v.back() == 16);
			EXAM_CHECK(v.size() == 4);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_5)
		msev_int msevec_test_mt5()
		{
			int array[] = { 1, 4, 9, 16 };

#ifdef MSVC2010_COMPATIBLE
			mse::TXScopeObj<mse::nii_vector<int> > v(array, array + 4);
#else /*MSVC2010_COMPATIBLE*/
			auto v = mse::make_xscope(mse::nii_vector<int>{ 1, 4, 9, 16 });
#endif /*MSVC2010_COMPATIBLE*/

			EXAM_CHECK(v.size() == 4);

			EXAM_CHECK(v[0] == 1);
			EXAM_CHECK(v[1] == 4);
			EXAM_CHECK(v[2] == 9);
			EXAM_CHECK(v[3] == 16);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_6)
		msev_int msevec_test_mt6()
		{
			int array[] = { 1, 4, 9, 16, 25, 36 };

#ifdef MSVC2010_COMPATIBLE
			mse::TXScopeObj<mse::nii_vector<int> > v(array, array + 6);
#else /*MSVC2010_COMPATIBLE*/
			auto v = mse::make_xscope(mse::nii_vector<int>{ 1, 4, 9, 16, 25, 36 });
#endif /*MSVC2010_COMPATIBLE*/
			//auto xs_vit = mse::make_xscope_begin_iterator(&v);

			EXAM_CHECK(v.size() == 6);
			EXAM_CHECK(v[0] == 1);
			EXAM_CHECK(v[1] == 4);
			EXAM_CHECK(v[2] == 9);
			EXAM_CHECK(v[3] == 16);
			EXAM_CHECK(v[4] == 25);
			EXAM_CHECK(v[5] == 36);

			//vit = v.erase(v.begin()); // Erase first element.
			{
				auto xs_vit = v.erase(&v, 0); // Erase first element.
				EXAM_CHECK(*xs_vit == 4);
			}

			EXAM_CHECK(v.size() == 5);
			EXAM_CHECK(v[0] == 4);
			EXAM_CHECK(v[1] == 9);
			EXAM_CHECK(v[2] == 16);
			EXAM_CHECK(v[3] == 25);
			EXAM_CHECK(v[4] == 36);

			//vit = v.erase(v.end() - 1); // Erase last element.
			{
				auto xs_vit = v.erase(&v, v.size() - 1); // Erase first element.
				EXAM_CHECK(mse::make_xscope_end_iterator(&v) == xs_vit);
			}

			EXAM_CHECK(v.size() == 4);
			EXAM_CHECK(v[0] == 4);
			EXAM_CHECK(v[1] == 9);
			EXAM_CHECK(v[2] == 16);
			EXAM_CHECK(v[3] == 25);

			//v.erase(v.begin() + 1, v.end() - 1); // Erase all but first and last.
			v.erase(&v, 0 + 1, v.size() - 1); // Erase all but first and last.

			EXAM_CHECK(v.size() == 2);
			EXAM_CHECK(v[0] == 4);
			EXAM_CHECK(v[1] == 25);

			return EXAM_RESULT;
		}

		//int EXAM_IMPL(vector_test::vec_test_7)
		msev_int msevec_test_mt7()
		{
			int array1[] = { 1, 4, 25 };
			int array2[] = { 9, 16 };

#ifdef MSVC2010_COMPATIBLE
			mse::TXScopeObj<mse::nii_vector<int> > v(array1, array1 + 3);
#else /*MSVC2010_COMPATIBLE*/
			auto v = mse::make_xscope(mse::nii_vector<int>{ 1, 4, 25 });
#endif /*MSVC2010_COMPATIBLE*/
			//auto xs_vit = mse::make_xscope_begin_iterator(&v);

			//xs_vit = v.insert(v.begin(), 0); // Insert before first element.
			{
				auto xs_vit = v.insert(&v, 0, 0); // Insert before first element.
				EXAM_CHECK(*xs_vit == 0);
			}

			//xs_vit = v.insert(v.end(), 36);  // Insert after last element.
			{
				auto xs_vit = v.insert(&v, v.size(), 36); // Insert after last element.
				EXAM_CHECK(*xs_vit == 36);
			}

			EXAM_CHECK(v.size() == 5);
			EXAM_CHECK(v[0] == 0);
			EXAM_CHECK(v[1] == 1);
			EXAM_CHECK(v[2] == 4);
			EXAM_CHECK(v[3] == 25);
			EXAM_CHECK(v[4] == 36);

			// Insert contents of array2 before fourth element.
			//v.insert(v.begin() + 3, array2, array2 + 2);
			//v.insert(v.begin() + 3, { 9, 16 });
#ifdef MSVC2010_COMPATIBLE
			v.insert(&v, 0 + 3, 16);
			v.insert(&v, 0 + 3, 9);
#else /*MSVC2010_COMPATIBLE*/
			v.insert(&v, 0 + 3, { 9, 16 });
#endif /*MSVC2010_COMPATIBLE*/

			EXAM_CHECK(v.size() == 7);

			EXAM_CHECK(v[0] == 0);
			EXAM_CHECK(v[1] == 1);
			EXAM_CHECK(v[2] == 4);
			EXAM_CHECK(v[3] == 9);
			EXAM_CHECK(v[4] == 16);
			EXAM_CHECK(v[5] == 25);
			EXAM_CHECK(v[6] == 36);

			v.clear();
			EXAM_CHECK(v.empty());

			//v.insert(v.begin(), 5, 10);
			v.insert(&v, 0, 5, 10);
			EXAM_CHECK(v.size() == 5);
			EXAM_CHECK(v[0] == 10);
			EXAM_CHECK(v[1] == 10);
			EXAM_CHECK(v[2] == 10);
			EXAM_CHECK(v[3] == 10);
			EXAM_CHECK(v[4] == 10);

			{
				//mse::TXScopeObj<mse::nii_vector<float> > vf(2.0f, 3.0f);
				mse::TXScopeObj<mse::nii_vector<float> > vf(2, 3.0f);
				EXAM_CHECK( vf.size() == 2 );
				EXAM_CHECK( vf.front() == 3.0f );
				EXAM_CHECK( vf.back() == 3.0f );
			}

			return EXAM_RESULT;
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

#endif /*ndef MSEVECTOR_TEST_H*/
