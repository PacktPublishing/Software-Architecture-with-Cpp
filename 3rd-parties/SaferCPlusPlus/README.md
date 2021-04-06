Mar 2021

### Overview

"SaferCPlusPlus" is essentially a collection of safe data types intended to facilitate memory and data race safe C++ programming. This library is intended to work with a safety assuring static analyzer like [scpptool](https://github.com/duneroadrunner/scpptool) and, optionally, the Core Guidelines lifetime checker, over their various stages of development and availability. (Including situations where they are not available at all.)

The library's elements are designed, as much as possible, to seamlessly integrate with all manner of existing and future C++ code. It includes things like:

- Drop-in replacements for [`std::vector<>`](#mstdvector), [`std::array<>`](#mstdarray) and [`std::string`](#mstdstring).

- Replacements for [`std::string_view`](#nrp_string_view) and [`std::span`](#txscopeanyrandomaccesssection-txscopeanyrandomaccessconstsection-tanyrandomaccesssection-tanyrandomaccessconstsection).

- Drop-in [replacements](#primitives) for `int`, `size_t` and `bool` that ensure against the use of uninitialized values and address the "signed-unsigned mismatch" issues.

- Data types for safe [sharing](#asynchronously-shared-objects) of objects among concurrently executing threads.

- Replacements for native pointers/references with various flexibility and performance trade-offs. 

While a "static safety analyzer/enforcer" like the scpptool would be required to ensure complete safety, the SaferCPlusPlus library elements have a lot of safety enforcement already built in. The library is extensive enough that most existing uses of unsafe C++ elements can be replaced. 

Besides zero-overhead pointers that enforce some of the necessary restrictions that would be imposed by a complete "static safety analyzer/enforcer", the library provides a reference counting pointer that's smaller and faster than `std::shared_ptr<>`, and an unrestricted pointer that ensures memory safety via run-time checks. The latter two being not (yet) provided by the Guidelines Support Library, but valuable in the context of having to work around the somewhat [draconian restrictions](https://github.com/duneroadrunner/misc/blob/master/201/8/Jul/implications%20of%20the%20lifetime%20checker%20restrictions.md) imposed by the (eventual completed) lifetime checker.

And the library also addresses the data race issue, where the Core Guidelines don't (yet) offer anything substantial.

To see the library in action, you can check out some [benchmark code](https://github.com/duneroadrunner/SaferCPlusPlus-BenchmarksGame). There you can compare traditional C++ and (high-performance) SaferCPlusPlus implementations of the same algorithms. Also, the [msetl_example.cpp](https://github.com/duneroadrunner/SaferCPlusPlus/blob/master/msetl_example.cpp) and [msetl_example2.cpp](https://github.com/duneroadrunner/SaferCPlusPlus/blob/master/msetl_example2.cpp) files contain usage examples of the library's elements. But at this point, there are a lot of them, so it might be more effective to peruse the documentation first, then search those files for the element(s) your interested in. 

Tested with msvc2019(v16.5.4), g++9.3.0 and clang++10.0.0. Versions of g++ prior to version 5 are not supported. Elements in this library are currently based on the C++17 version of their counterpart APIs. Note that parts of the library documentation were written before it was clear that a viable lifetime checker might be forthcoming and should be interpreted accordingly.


### Table of contents
1. [Overview](#overview)
2. [Use cases](#use-cases)
3. [Setup and dependencies](#setup-and-dependencies)
4. Comparisons
    1. [SaferCPlusPlus versus Clang/LLVM Sanitizers](#safercplusplus-versus-clangllvm-sanitizers)
    2. [SaferCPlusPlus versus Rust](#safercplusplus-versus-rust)
    3. [SaferCPlusPlus versus Checked C](#safercplusplus-versus-checked-c)
5. [Getting started on safening existing code](#getting-started-on-safening-existing-code)
6. [Registered pointers](#registered-pointers)
    1. [TRegisteredPointer](#tregisteredpointer)
        1. [TRegisteredNotNullPointer](#tregisterednotnullpointer)
        2. [TRegisteredFixedPointer](#tregisteredfixedpointer)
        3. [TRegisteredConstPointer](#tregisteredconstpointer-tregisterednotnullconstpointer-tregisteredfixedconstpointer)
        4. [TRegisteredRefWrapper](#tregisteredrefwrapper)
    2. [TCRegisteredPointer](#tcregisteredpointer)
    3. [TNDRegisteredPointer, TNDCRegisteredPointer](#tndregisteredpointer-tndcregisteredpointer)
7. [Norad pointers](#norad-pointers)
    1. [TNoradPointer](#tnoradpointer)
8. [Simple benchmarks](#simple-benchmarks)
9. [Reference counting pointers](#reference-counting-pointers)
    1. [TRefCountingPointer](#trefcountingpointer)
        1. [TRefCountingNotNullPointer](#trefcountingnotnullpointer)
        2. [TRefCountingFixedPointer](#trefcountingfixedpointer)
        3. [TRefCountingConstPointer](#trefcountingconstpointer-trefcountingnotnullconstpointer-trefcountingfixedconstpointer)
    2. [Using registered pointers as weak pointers](#using-registered-pointers-as-weak-pointers-with-reference-counting-pointers)
10. [Scope pointers](#scope-pointers)
    1. [TXScopeFixedPointer](#txscopefixedpointer)
    2. [TXScopeOwnerPointer](#txscopeownerpointer)
    3. [make_xscope_strong_pointer_store()](#make_xscope_strong_pointer_store)
    4. [TRegisteredProxyPointer](#tregisteredproxypointer)
    5. [TNoradProxyPointer](#tnoradproxypointer)
    6. [xscope_chosen()](#xscope_chosen)
    7. [as_a_returnable_fparam()](#as_a_returnable_fparam)
    8. [as_an_fparam()](#as_an_fparam)
    9. [Conformance helpers](#conformance-helpers)
        1. [return_value()](#return_value)
        2. [TMemberObj](#tmemberobj)
11. [make_pointer_to_member_v2()](#make_pointer_to_member_v2)
12. [Poly pointers](#poly-pointers)
    1. [TXScopePolyPointer](#txscopepolypointer-txscopepolyconstpointer)
    2. [TPolyPointer](#tpolypointer-tpolyconstpointer)
    3. [TAnyPointer](#txscopeanypointer-txscopeanyconstpointer-tanypointer-tanyconstpointer)
13. [pointer_to()](#pointer_to)
14. [Safely passing parameters by reference](#safely-passing-parameters-by-reference)
15. [Multithreading](#multithreading)
    1. [TAsyncPassableObj](#tasyncpassableobj)
    2. [thread](#thread)
    3. [async()](#async)
    4. [Asynchronously shared objects](#asynchronously-shared-objects)
        1. [TAsyncShareableObj](#tasyncshareableobj)
        2. [TAsyncShareableAndPassableObj](#tasyncshareableandpassableobj)
        3. [TAsyncSharedV2ReadWriteAccessRequester](#tasyncsharedv2readwriteaccessrequester)
        4. [TAsyncSharedV2ReadOnlyAccessRequester](#tasyncsharedv2readonlyaccessrequester)
        5. [TAsyncSharedV2ImmutableFixedPointer](#tasyncsharedv2immutablefixedpointer)
        6. [TAsyncSharedV2AtomicFixedPointer](#tasyncsharedv2atomicfixedpointer)
        7. [TAsyncRASectionSplitter](#tasyncrasectionsplitter)
    5. [Scope threads](#scope-threads)
        1. [access controlled objects](#access-controlled-objects)
        2. [xscope_thread_carrier](#xscope_thread_carrier)
        3. [xscope_future_carrier](#xscope_future_carrier)
        4. [make_xscope_asyncsharedv2acoreadwrite()](#make_xscope_asyncsharedv2acoreadwrite)
        5. [make_xscope_aco_locker_for_sharing()](#make_xscope_aco_locker_for_sharing)
        6. [make_xscope_exclusive_strong_pointer_store_for_sharing()](#make_xscope_exclusive_strong_pointer_store_for_sharing)
        7. [TXScopeExclusiveStrongPointerStoreForAccessControl](#txscopeexclusivestrongpointerstoreforaccesscontrol)
        8. [exclusive writer objects](#exclusive-writer-objects)
        9. [scope atomics](#scope-atomics)
        10. [TXScopeACORASectionSplitter and TXScopeAsyncACORASectionSplitter](#txscopeacorasectionsplitter-and-txscopeasyncacorasectionsplitter)
    6. [static and global variables](#static-and-global-variables)
        1. [static immutables](#static-immutables)
        2. [static atomics](#static-atomics)
        3. [static access controlled objects and access requesters](#static-access-controlled-objects-and-access-requesters)
16. [Primitives](#primitives)
    1. [CInt, CSize_t and CBool](#cint-csize_t-and-cbool)
    2. [CNDInt, CNDSize_t and CNDBool](#cndint-cndsize_t-and-cndbool)
    3. [Quarantined types](#quarantined-types)
17. [Arrays](#arrays)
    1. [mstd::array](#mstdarray)
    2. [nii_array](#nii_array)
    3. [xscope_nii_array](#xscope_nii_array)
    4. [xscope_iterator](#xscope_iterator)
18. [Vectors](#vectors)
    1. [mstd::vector](#mstdvector)
    2. [nii_vector](#nii_vector)
    3. [fixed_nii_vector](#fixed_nii_vector)
    4. [xscope_borrowing_fixed_nii_vector](#xscope_borrowing_fixed_nii_vector)
    5. [ivector](#ivector)
19. [TRandomAccessSection](#txscoperandomaccesssection-txscoperandomaccessconstsection-trandomaccesssection-trandomaccessconstsection)
20. [Strings](#strings)
    1. [mstd::string](#mstdstring)
    2. [nii_string](#nii_string)
    3. [xscope_borrowing_fixed_nii_basic_string](#xscope_borrowing_fixed_nii_basic_string)
    4. [TStringSection](#txscopestringsection-txscopestringconstsection-tstringsection-tstringconstsection)
    5. [TNRPStringSection](#txscopenrpstringsection-txscopenrpstringconstsection-tnrpstringsection-tnrpstringconstsection)
    6. [mstd::string_view](#string_view)
    7. [nrp_string_view](#nrp_string_view)
21. Poly Iterators and Sections
    1. [TAnyRandomAccessIterator](#txscopeanyrandomaccessiterator-txscopeanyrandomaccessconstiterator-tanyrandomaccessiterator-tanyrandomaccessconstiterator)
    2. [TAnyRandomAccessSection](#txscopeanyrandomaccesssection-txscopeanyrandomaccessconstsection-tanyrandomaccesssection-tanyrandomaccessconstsection)
    3. [TAnyStringSection](#txscopeanystringsection-txscopeanystringconstsection-tanystringsection-tanystringconstsection)
    4. [TAnyNRPStringSection](#txscopeanynrpstringsection-txscopeanynrpstringconstsection-tanynrpstringsection-tanynrpstringconstsection)
    5. [TXScopeCSSSXSTERandomAccessIterator and TXScopeCSSSXSTERandomAccessSection](#txscopecsssxsterandomaccessiterator-and-txscopecsssxsterandomaccesssection)
    6. [TXScopeCSSSXSTEStringSection](#txscopecsssxstestringsection-txscopecsssxstenrpstringsection)
22. [Optionals](#optionals)
    1. [mstd::optional](#mstdoptional)
    2. [optional](#optional)
    3. [fixed_optional](#fixed_optional)
    4. [xscope_borrowing_fixed_optional](#xscope_borrowing_fixed_optional)
23. [Tuples](#tuples)
    1. [mstd::tuple](#tuple)
    2. [xscope_tuple](#xscope_tuple)
24. [Algorithms](#algorithms)
    1. [for_each_ptr()](#for_each_ptr)
    2. [find_if_ptr()](#find_if_ptr)
25. [thread_local](#thread_local)
26. [(Type-erased) function objects](#type-erased-function-objects)
    1. [mstd::function](#function)
    2. [xscope_function](#xscope_function)
27. [Practical limitations](#practical-limitations)
28. [Questions and comments](#questions-and-comments)

### Use cases

The library was designed to help reduce or eliminate the potential for invalid memory accesses and data races in general C++ code. The general strategy is simply to substitute potentially unsafe C++ elements with compatible safe replacements from the library. The library does not impose any particular paradigm or code structure. (Though more modern coding styles that de-emphasize explicit use of iterators may result in better performance.)

When using a static safety enforcer (like the [scpptool](https://github.com/duneroadrunner/scpptool) and/or the Core Guidelines lifetime checker, when completed), some of the most used elements of the library (namely the "scope" pointer elements) may be rendered largely redundant. Code using the pointer/reference types in this library should, unlike "regular" C++ code, already be compliant with the restrictions that will be imposed by such a completed static safety enforcer. So you can think of the use of this library as a method of "future-proofing" your code for a time when it may become standard practice to automatically reject C++ code that isn't approved by a static safety enforcer.

While using the library can incur a modest performance penalty, because the library elements are largely compatible with their native counterparts, they can be easily "disabled" (automatically replaced with their native counterparts) with a compile-time directive, allowing them, for example, to be used to help catch bugs in debug/test/beta builds while incurring no overhead in release builds.

And note that the safe components of this library can be adopted completely incrementally. New code written with these safe elements will play nicely with existing code, and native C++ elements can be replaced selectively without breaking the existing code. So there is really no excuse for not using the library in pretty much any situation. Though for real time embedded applications, note the dependence on the standard library.

### Setup and dependencies

Using this (header-only) library generally involves copying the include files you want to use into your project, and that's it. Outside of the standard library, there are no other dependencies.  

Building the example: For those using msvc, project and solution files are included. Otherwise, just create a new project and add all the `.cpp` and `.h` files.

A couple of notes about compiling: With g++ and clang++, you'll need to enable thread support (-pthread). With 64-bit builds in msvc you may get a "[fatal error C1128: number of sections exceeded object file format limit: compile with /bigobj](https://msdn.microsoft.com/en-us/library/8578y171(v=vs.140).aspx)". Just [add](https://msdn.microsoft.com/en-us/library/ms173499.aspx) the "/bigobj" compile flag. For more help you can try the [questions and comments](#questions-and-comments) section. Also related: [Versioning and deployment philosophy](README2.md#versioning-and-deployment-philosophy).

### SaferCPlusPlus versus Clang/LLVM Sanitizers

The Clang/LLVM compiler provides a set of "sanitizers" (adopted by gcc) that address C/C++ "code safety" issues. While they address many of the same bugs, the solutions provided by the SaferCPlusPlus library and the Clang/LLVM sanitizers differ in significant ways. Namely (as of Sep 2016):

- The Clang/LLVM sanitizers require modifications to the build process, not the code, whereas with SaferCPlusPlus it's the other way around.
- SaferCPlusPlus can [more completely](https://en.wikipedia.org/wiki/AddressSanitizer#Limitations) solve the problem of invalid memory access, but in doing so does not support certain potentially dangerous language features (like pointer arithmetic).
- When encountering an invalid memory operation at run-time, the Clang/LLVM sanitizers terminate the executable, where SaferCPlusPlus, by default, throws a (catchable) exception, but supports any user-defined action, including program termination. 
- SaferCPlusPlus is portable C++ code that works on any platform, whereas Clang/LLVM sanitizers are available/maintained on a finite (but at the moment, ample) set of OS-architecture combinations.
- The Clang/LLVM sanitizers cost more in terms of run-time performance. [~2x slowdown](https://github.com/google/sanitizers/wiki/AddressSanitizerPerformanceNumbers) for the AddressSanitizer, for example. SaferCPlusPlus generally has substantially [lower performance costs](https://github.com/duneroadrunner/SaferCPlusPlus-BenchmarksGame), mainly because the Clang/LLVM sanitizers cannot assume any cooperation from the source code, so they have to instrument pretty much every allocated piece of memory and check pretty well every pointer dereference.
- SaferCPlusPlus supports the mixing of "safe" and (high-performance) "unsafe" code at a granular level, where Clang/LLVM Sanitizers apply to entire modules, or as in the case of the MemorySanitizer, all modules, requiring recompilation of any linked libraries.
- Clang's ThreadSanitizer tries to detect data race bugs, while SaferCPlusPlus provides [data types](#asynchronously-shared-objects) that eliminate the possibility of data race bugs (and a superset we call "object race" bugs).

Clang/LLVM Sanitizers are intended for debugging purposes, not to be used in deployed executables. As such, by design, some of their debugging convenience features themselves introduce [opportunities](http://seclists.org/oss-sec/2016/q1/363) for malicious exploitation. SaferCPlusPlus on the other hand, is designed to be used in deployed executables, as well as for debugging and testing. And that's reflected in its performance, security and "completeness of solution". But SaferCPlusPlus and the Clang/LLVM Sanitizers are not incompatible, and there's no reason you couldn't use both simultaneously, although there would be significant redundancies.

### SaferCPlusPlus versus Rust

C++ and Rust differ significantly in many ways, but SaferCPlusPlus is primarily concerned with addressing memory safety so here we'll consider only that aspect. Given that, what's most notable is the similarities between SaferCPlusPlus and Rust, considering they were developed independently. 

Probably the main issue when it comes to memory safety is the relationship between pointers/references and object deallocation. So it's notable that Rust and SaferCPlusPlus decided on roughly corresponding sets of (safe) pointer/reference types:

Rust | SaferCPlusPlus
---- | --------------
non-reassignable reference | scope pointer
reassignable (mut) reference | registered pointer
Box<> | scope owner pointer
Rc<> | reference counting pointer
Arc<> | shared immutable pointer
Arc< RwLock<> > | access requester

Indeed, if you are a Rust programmer you might be more comfortable using SaferCPlusPlus than traditional C++.

The most commonly used reference type, the non-reassignable (non-mut) reference in Rust and the scope pointer in SaferCPlusPlus, incurs no run-time overhead in both cases, which is a primary reason for the excellent performance of both solutions. 

Reassignable (mut) references occur much less frequently, but currently, safe, reassignable, zero-overhead pointers in C++ are even [more restricted](https://github.com/duneroadrunner/scpptool#restrictions-on-the-use-of-native-pointers-and-references) than their counterpart in Rust. However, the SaferCPlusPlus library also provides pointers with run-time safety mechanisms and greater flexibility, for which there are no Rust counterparts (due to Rust's lack of move constructors). So for example, "self-referencing" data types (that are movable and can be allocated on the stack) are straightforward to implement in C++'s memory safe subset in a way that's not really available in Rust.

Probably the most noticeable difference though, is that SaferCPlusPlus does not (universally) restrict the number and type of references to an object that can exist at one time (i.e. the "exclusivity of mutable references") the way Rust does. With respect to memory safety, the benefit of this restriction is that it ensures that objects with "arbitrary lifespan" (like an element in a (resizable) vector) are not decommissioned while other references to that object still exist. But most objects do not have "arbitrary lifespan" (both Rust and SaferCPlusPlus encourage most objects to have "scope lifespan"), so most of the time, from a memory safety perspective, this restriction is not necessary. So SaferCPlusPlus (and the lifetime checker too) are more selective about what restrictions are applied and when. 

There are situations where restrictions on object accessibility can be beneficial or essential for reasons other than (single-threaded) memory safety. (To help ensure data race safety of asynchronously shared objects, for example.) For those cases, the library does provide facilities for the enforcement of "access control" restrictions, including essentially the [equivalent](#exclusive-writer-objects) of Rust's `RefCell` wrapper.

Overall though, there's probably more commonality than difference between the Rust and the SaferCPlusPlus memory safety strategies. At least compared to other current languages. So, perhaps as expected, you could think of the comparison between SaferCPlusPlus and Rust as essentially the comparison between C++ and Rust, with diminished discrepancies in memory safety and performance.

### SaferCPlusPlus versus Checked C

"Checked C", like SaferCPlusPlus, takes the approach of extending the language with safer elements that can directly substitute for unsafe native elements. In chapter 9 of their [spec](https://github.com/Microsoft/checkedc/releases/download/v0.5-final/checkedc-v0.5.pdf), there is an extensive survey of existing (and historical) efforts to address C/C++ memory safety. There they make the argument for the (heretofore neglected) "language extension" approach (basically citing performance, compatibility and the support for granular mixing of safe and unsafe code), that applies to SaferCPlusPlus as well.

Checked C and SaferCPlusPlus are more complementary than competitive. Checked C targets low-level system C code and basically only addresses the array bounds checking issue, including pointer arithmetic, where SaferCPlusPlus skews more toward C++ code and legacy code that would benefit from being converted to modern C++. It seems that Checked C is not yet ready for deployment (as of Sep 2016), but one could imagine both solutions being used, with little contention, in projects that have both low-level system type code and higher-level application type code.

### Getting started on safening existing code

The elements in this library are straightforward enough that a separate tutorial, beyond the examples given in the documentation, is probably not necessary. But if you're wondering how best to start, probably the easiest and most effective thing to do is to replace the vectors and arrays in your code (that aren't being shared between threads) with [`mse::mstd::vector<>`](#mstdvector) and [`mse::mstd::array<>`](#mstdarray). You can substitute `std::string_view` with [`mse::nrp_string_view`](#nrp_string_view), and your `std::string`s with [`mse::mstd::string`](#mstdstring).

Statistically speaking, doing this should already catch a significant chunk of potential memory bugs. By default, an exception will be thrown upon any attempt to access invalid memory (or the program will terminate if compiled with support for exceptions disabled). This behavior can be customized (for example, to notify a custom logger of any invalid memory access attempts) by defining the `MSE_CUSTOM_THROW_DEFINITION()` preprocessor function macro.

Achieving a more comprehensive degree of safety (while maintaining efficiency) takes a bit more effort. First, you'll want to get acquainted with [scope pointers and objects](#scope-pointers). You'll want to, for example, understand how to [obtain a scope iterator](#xscope_iterator), and in turn, a scope pointer to a vector element (as demonstrated, for example, in the [`xscope_borrowing_fixed_nii_vector<>`](#xscope_borrowing_fixed_nii_vector) example). You'll want to understand how to obtain a scope pointer to [a class/struct member](#make_pointer_to_member_v2) from a scope pointer to the class/struct. You'll want to understand how to obtain a scope pointer to the [target of a strong/owning pointer](#make_xscope_strong_pointer_store).

Once you've embraced the scope pointer lifestyle, you can use a conformance helper tool like [scpptool](https://github.com/duneroadrunner/scpptool) to analyze your source files and flag parts of your code that it can't verify to be safe. Appeasing the tool largely involves simply obtaining scope pointers to the objects in question. 

### Registered pointers

"Registered" pointers behave much like native C++ pointers, except that their value is (automatically) set to nullptr when the target object is destroyed. And by default they will throw an exception upon any attempt to dereference a nullptr. Because they don't take ownership like some other smart pointers, they can point to objects allocated on the stack as well as the heap.  Safe, flexible pointers like these can be handy in situations that are not amenable to the confining restrictions of the lifetime checker. They may be particularly useful when updating legacy code (to be safer).

Two types of registered pointers are provided - [`TRegisteredPointer<>`](#tregisteredpointer) and [`TCRegisteredPointer<>`](#tcregisteredpointer). They are functionally equivalent, but `TRegisteredPointer<>` is optimized for better average performance, while `TCRegisteredPointer<>` is a little more optimized for better "worst-case" performance. (Specifically, the operation of retargeting (or "detargeting") a `TRegisteredPointer<>` in the worst case is *O(n)*, where *n* is the number of other pointers targeting the same original target object. With `TCRegisteredPointer<>` it's always *O(1)*.)

Note that these registered pointers cannot target some types that cannot act as base classes. The primitive types like int, bool, etc. cannot act as base classes. The library provides safer [substitutes](#cndint-cndsize_t-and-cndbool) for `int`, `bool` and `size_t` that can act as base classes. Also note that these registered pointers are not thread safe. When you need to share objects between asynchronous threads, you can use the [safe sharing data types](#asynchronously-shared-objects) in this library.

Although registered pointers are more general and flexible, it's expected that [scope pointers](#scope-pointers) will actually be more commonly used. At least in cases where performance is important. While more restricted than registered pointers, by default they have no run-time overhead. In fact, even when registered pointers are used, they would be expected to be commonly used in [conjuction with scope pointers](#tregisteredproxypointer). Though for the sake of simplicity, we don't use scope pointers in the registered pointer usage examples.  

### TRegisteredPointer

usage example:

```cpp
    #include "mseregistered.h"
    
    void main(int argc, char* argv[]) {
        class CA {
        public:
            CA(int x) : m_x(x) {}
            int m_x;
        };
    
        mse::TRegisteredPointer<CA> a_regptr;
        CA a2_obj(2);
        {
            // mse::TRegisteredObj<CA> is a class publicly derived from CA
    
            mse::TRegisteredObj<CA> a_regobj(1); // a_regobj is entirely on the stack
    
            a_regptr = &a_regobj;
            a2_obj = (*a_regptr);
        }
        if (a_regptr) {
            assert(false);
        } else {
            try {
                a2_obj = (*a_regptr);
            }
            catch (...) {
                // expected exception
            }
        }
    
        a_regptr = mse::registered_new<CA>(3); // heap allocation
        mse::registered_delete<CA>(a_regptr);
    
        /* You can also use the make_registered() function to obtain a registered object from a given value. */
        auto a3_regobj = mse::make_registered(CA(5));
    }
```

Note that using `mse::registered_delete()` to delete an object through a base class pointer will result in a failed assert / thrown exception. In such cases use (the not quite as safe) `mse::us::registered_delete()` instead.

### TRegisteredNotNullPointer
`TRegisteredNotNullPointer<>` is a version of `TRegisteredPointer<>` that cannot be constructed to a null value. Note that `TRegisteredPointer<>` does not implicitly convert to `TRegisteredNotNullPointer<>`. When needed, the conversion can be done with the `mse::not_null_from_nullable()` function.

### TRegisteredFixedPointer
`TRegisteredFixedPointer<>` is a version of `TRegisteredNotNullPointer<>` that cannot be retargeted after construction (basically a "`const TRegisteredNotNullPointer<>`"). It is essentially a functional equivalent of a C++ reference.  

usage example:

```cpp
    #include "mseregistered.h"
    
    void main(int argc, char* argv[]) {
        class CA {
        public:
            CA(std::string str) : m_str(str) {}
            std::string m_str;
        };
    
        class CB {
        public:
            static void foo(mse::TRegisteredFixedConstPointer<CA> input1_fc_ptr, mse::TRegisteredFixedConstPointer<CA> 
                input2_fc_ptr, mse::TRegisteredFixedPointer<CA> output_f_ptr) {
                output_f_ptr->m_str = "output from " + input1_fc_ptr->m_str + " and " + input2_fc_ptr->m_str;
                return;
            }
        };
    
        mse::TRegisteredObj<CA> in1_regobj("input1");
        mse::TRegisteredPointer<CA> in2_regptr = mse::registered_new<CA>("input2");
        auto out_regobj = mse::make_registered(CA("")); /* Alternative way to make a registered object. */
    
        CB::foo(&in1_regobj, &(*in2_regptr), &out_regobj);
    
        mse::registered_delete<CA>(in2_regptr);
    }
```

### TRegisteredConstPointer, TRegisteredNotNullConstPointer, TRegisteredFixedConstPointer
`TRegisteredPointer<X>` does implicitly convert to `TRegisteredPointer<const X>`. But some prefer to think of the pointer giving "const" access to the object rather than giving access to a "const object".

### TRegisteredRefWrapper
Just a registered version of [`std::reference_wrapper<>`](http://en.cppreference.com/w/cpp/utility/functional/reference_wrapper).  

usage example:

```cpp
    #include "mseprimitives.h"
    #include "mseregistered.h"

    void main(int argc, char* argv[]) {
        /* This example originally comes from http://www.cplusplus.com/reference/functional/reference_wrapper/. */
        mse::TRegisteredObj<mse::CInt> a(10), b(20), c(30);
        // an array of "references":
        mse::TRegisteredRefWrapper<mse::CInt> refs[] = { a,b,c };
        std::cout << "refs:";
        for (mse::CInt& x : refs) std::cout << ' ' << x;
        std::cout << '\n';
    
        mse::TRegisteredObj<mse::CInt> foo(10);
        auto bar = mse::registered_ref(foo);
        ++(mse::CInt&)bar;
        std::cout << foo << '\n';
    }
```

### TCRegisteredPointer

usage example:

```cpp
    #include "msecregistered.h"
    
    void main(int argc, char* argv[]) {
        
        class C;
    
        class D {
        public:
            virtual ~D() {}
            mse::TCRegisteredPointer<C> m_c_ptr;
        };
    
        class C {
        public:
            mse::TCRegisteredPointer<D> m_d_ptr;
        };
    
        mse::TCRegisteredObj<C> regobjfl_c;
        mse::TCRegisteredPointer<D> d_ptr = mse::cregistered_new<D>();
    
        regobjfl_c.m_d_ptr = d_ptr;
        d_ptr->m_c_ptr = &regobjfl_c;
    
        mse::cregistered_delete<D>(d_ptr);
    }
```

As with [`TRegisteredPointer<>`](#tregisteredpointer), if deleting a cregistered object via a pointer to its base class you'll need to use the `mse::us::cregistered_delete<>()` function instead.

#### TCRegisteredNotNullPointer

#### TCRegisteredFixedPointer

#### TCRegisteredConstPointer, TCRegisteredNotNullConstPointer, TCRegisteredFixedConstPointer

### TNDRegisteredPointer, TNDCRegisteredPointer

When pointing to a valid object, [`TRegisteredPointer<>` and `TCRegisteredPointer<>`](#registered-pointers) essentially behave like raw pointers. So when in "disabled" mode, they are just aliased to raw pointers. However, in cases when their target object becomes invalid (i.e. is destroyed), the behavior of registered pointers is not the same as raw pointers. Specifically, registered pointers are automatically set to null when their target object is destroyed. So any code that relies on this behavior might not work properly when the registered pointers are substituted with raw pointers.

So for those cases, `TNDRegisteredPointer<>` and `TNDCRegisteredPointer` are just versions of registered pointers that are not aliased to raw pointers in "disabled" mode. In fact, when not in "disabled" mode, `TRegisteredPointer<>` and `TCRegisteredPointer<>` are just aliases for `TNDRegisteredPointer<>` and `TNDCRegisteredPointer`.

### Norad pointers

"Norad" pointers, like [registered pointers](#registered-pointers), behave similar to native pointers. But where registered pointers are automatically set to `nullptr` when their target is destroyed, the destruction of an object while a "norad" pointer is still targeting it results in program termination. This drastic consequence allows norad pointers' run-time safety mechanism to be even more lightweight than registered pointers. And unlike registered pointers, you can obtain a [scope pointer](#scope-pointers) from any norad pointer (using the [`make_xscope_strong_pointer_store()`](#make_xscope_strong_pointer_store) function).

### TNoradPointer

usage example:

```cpp
    #include "msenorad.h"
    
    void main(int argc, char* argv[]) {
    
        class C;

        class D {
        public:
            virtual ~D() {}
            mse::TNoradPointer<C> m_c_ptr;
        };

        class C {
        public:
            mse::TNoradPointer<D> m_d_ptr;
        };

        mse::TNoradObj<C> noradobj_c;
        mse::TNoradPointer<D> d_ptr = mse::norad_new<D>();

        noradobj_c.m_d_ptr = d_ptr;
        d_ptr->m_c_ptr = &noradobj_c;

        /* We must make sure that there are no other references to the target of d_ptr before deleting it. Registered pointers don't
        have the same requirement. */
        noradobj_c.m_d_ptr = nullptr;

        mse::norad_delete<D>(d_ptr);

        /* You can also use the make_norad() function to obtain a norad object from a given value. */
        auto noradobj_c2 = mse::make_norad(C());
    }
```

### TNoradNotNullPointer

`TNoradNotNullPointer<>` is a version of `TNoradPointer<>` that cannot be constructed to or assigned a null value. Because a `TNoradNotNullPointer<>` cannot outlive its target, it should be always safe to assume that it points to a validly allocated object. Note that `TNoradPointer<>` does not implicitly convert to `TNoradNotNullPointer<>`. When needed, the conversion can be done with the `mse::not_null_from_nullable()` function. 

#### TNoradFixedPointer

#### TNoradConstPointer, TNoradNotNullConstPointer, TNoradFixedConstPointer, TNDNoradPointer


### Simple benchmarks

Just some simple microbenchmarks of the pointers. (Some less "micro" benchmarks of the library in general can be found [here](https://github.com/duneroadrunner/SaferCPlusPlus-BenchmarksGame).) The source code for these benchmarks can be found in the file [msetl_example.cpp](https://github.com/duneroadrunner/SaferCPlusPlus/blob/master/msetl_example.cpp). (Search for "benchmark" in the file.)

##### platform: msvc2017/default optimizations/x64/Windows7/Haswell (Sep 2018):

#### Target object allocation and deallocation:

Pointer Type | Time
------------ | ----
native pointer (stack) | 0.049 seconds
[mse::TCRegisteredPointer](#tcregisteredpointer) (stack) | 0.057 seconds
[mse::TRegisteredPointer](#tregisteredpointer) (stack) | 0.058 seconds
[mse::TNoradPointer](#tnoradpointer) (stack) | 0.059 seconds
native pointer (heap) | 0.384 seconds
mse::TNoradPointer (heap) | 0.394 seconds
[mse::TRefCountingPointer](#trefcountingpointer) (heap) | 0.402 seconds
mse::TCRegisteredPointer (heap) | 0.414 seconds
mse::TRegisteredPointer (heap) | 0.417 seconds
std::shared_ptr (heap) | 0.524 seconds

#### Pointer declaration, copy and assignment:

Pointer Type | Time
------------ | ----
native pointer | 0.046 seconds
mse::TRefCountingPointer | 0.090 seconds
mse::TNoradPointer | 0.119 seconds
mse::TRegisteredPointer | 0.145 seconds
mse::TCRegisteredPointer | 0.160 seconds
std::shared_ptr | 0.284 seconds

#### Dereferencing:

Pointer Type | Time
------------ | ----
native pointer | 0.106 seconds
native pointer + nullptr check | 0.107 seconds
mse::TNoradPointer | 0.160 seconds
mse::TCRegisteredPointer | 0.162 seconds
mse::TRefCountingPointer | 0.220 seconds
std::weak_ptr | 1.364 seconds

Take these results with a grain of salt. The benchmarks were run on a noisy machine, and anyway don't represent realistic usage scenarios. But they give you a rough idea of the relative performances.

You can see that the library's safe pointers are quite fast compared to, say, `std::shared_ptr`/`std::weak_ptr`. But in some sense, as long as it's not egregiously bad, their performance is kind of moot because performance sensitive programs would generally stick to using [scope pointers](#scope-pointers) in critical inner loops. And by default, scope pointers have identical performance to native pointers.

### Reference counting pointers

If you're going to use pointers, then to ensure they won't be used to access invalid memory you basically have two options - detect any attempt to do so and throw an exception, or, alternatively, ensure that the pointer targets a validly allocated object. Registered pointers rely on the former, and so-called "reference counting" pointers can be used to achieve the latter. The most famous reference counting pointer is `std::shared_ptr<>`, which is notable for its thread-safe reference counting that can be handy when you're sharing an object among asynchronous threads, but is unnecessarily costly when you aren't. So we provide fast reference counting pointers that forego any thread safety mechanisms. In addition to being substantially faster (and smaller) than `std::shared_ptr<>`, they are a bit more safety oriented in that they they don't support construction from raw pointers. (Use `mse::make_refcounting<>()` instead.) "Const", "not null" and "fixed" (non-retargetable) flavors are also provided with proper conversions between them.


### TRefCountingPointer

usage example:

```cpp
	#include "mserefcounting.h"
	
	void main(int argc, char* argv[]) {
		class A {
		public:
			A() {}
			A(const A& _X) : b(_X.b) {}
			virtual ~A() {
				int q = 3; /* just so you can place a breakpoint if you want */
			}
			A& operator=(const A& _X) { b = _X.b; return (*this); }

			int b = 3;
		};
		typedef std::vector<mse::TRefCountingFixedPointer<A>> CRCFPVector;
		class B {
		public:
			static int foo1(mse::TRefCountingPointer<A> A_refcounting_ptr, CRCFPVector& rcfpvector_ref) {
				rcfpvector_ref.clear();
				int retval = A_refcounting_ptr->b;
				A_refcounting_ptr = nullptr; /* Target object is destroyed here. */
				return retval;
			}
		protected:
			~B() {}
		};

		{
			CRCFPVector rcfpvector;
			{
				mse::TRefCountingFixedPointer<A> A_refcountingfixed_ptr1 = mse::make_refcounting<A>();
				rcfpvector.push_back(A_refcountingfixed_ptr1);

				/* Just to demonstrate conversion between refcounting pointer types. */
				mse::TRefCountingConstPointer<A> A_refcountingconst_ptr1 = A_refcountingfixed_ptr1;
			}
			B::foo1(rcfpvector.front(), rcfpvector);
		}
	}
```

### TRefCountingNotNullPointer

Same as `TRefCountingPointer<>`, but cannot be constructed to or assigned a null value. Because `TRefCountingNotNullPointer<>` controls the lifetime of its target, it should be always safe to assume that it points to a validly allocated object. Note that `TRefCountingPointer<>` does not implicitly convert to `TRefCountingNotNullPointer<>`. When needed, the conversion can be done with the `mse::not_null_from_nullable()` function. 

And also note that the `mse::make_refcounting<>()` function actually returns a `TRefCountingNotNullPointer<>`. When you want a regular `TRefCountingPointer<>`, you can use the `mse::make_nullable_refcounting<>()` function instead. 

### TRefCountingFixedPointer

Same as `TRefCountingNotNullPointer<>`, but cannot be retargeted after construction (basically a "`const TRefCountingNotNullPointer<>`").

### TRefCountingConstPointer, TRefCountingNotNullConstPointer, TRefCountingFixedConstPointer

`TRefCountingPointer<X>` actually does implicitly convert to `TRefCountingPointer<const X>`. But some prefer to think of the pointer giving "const" access to the object rather than giving access to a "const object".

### Using registered pointers as weak pointers with reference counting pointers

`TRefCountingPointer<>` does not have a specific associated weak pointer like `std::shared_ptr<>` does. But registered pointers can be thought of as sort of independent, universal weak pointers. Note that we're talking about targeting objects "in" the same thread here. Sharing objects between threads is done through the library's [data types for asynchronous sharing](#asynchronously-shared-objects). 

Generally you're going to want to obtain a "strong" pointer from the weak pointer, so rather than targeting the registered pointer directly at the object of interest, you'd target a/the strong owning pointer of the object.

```cpp
    #include "mserefcounting.h"
    #include "mseregistered.h"
    #include <iostream>
    
    void main(int argc, char* argv[]) {

        typedef mse::TRefCountingFixedPointer<std::string> str_rc_ptr_t; // owning pointer of a string
        typedef mse::TNDRegisteredObj<str_rc_ptr_t> str_rc_ptr_regobj_t; // registered version of above so that you can obtain a (weak)
                                                                       // registered pointer to it

         /* str_rc_rc_ptr1 is a "shared" owner of an owning pointer of a string  */
        auto str_rc_rc_ptr1 = mse::make_nullable_refcounting<str_rc_ptr_regobj_t>(str_rc_ptr_regobj_t(mse::make_refcounting<std::string>("some text")));
        /* You need to double dereference it to access the string value. */
        std::cout << **str_rc_rc_ptr1 << std::endl;

        /* Here we're obtaining a (weak) registered pointer to the owning pointer of the string. */
        auto str_rc_reg_ptr1 = &(*str_rc_rc_ptr1);
        /* Here you also need to double dereference it to access the string value. */
        std::cout << **str_rc_reg_ptr1 << std::endl;

        {
            /* We can obtain a (strong) owning pointer of the string from the (weak) registered pointer. */
            auto str_rc_ptr2 = *str_rc_reg_ptr1;

            std::cout << *str_rc_ptr2 << std::endl;
        }

        assert(str_rc_reg_ptr1); // just asserting the str_rc_reg_ptr1 is not null here

        /* Here we're releasing ownership of the string owning pointer. Since this was its only owner, the string owning
        pointer (and consequently the string) will be destroyed. */
        str_rc_rc_ptr1 = nullptr;

        assert(!str_rc_reg_ptr1); // here we're asserting that str_rc_reg_ptr1 has been (automatically) set to null
    }
```

This next example demonstrates using `TNDCRegisteredPointer<>` as a safe "weak_ptr" to prevent cyclic references from becoming memory leaks. This isn't much different from using `std::weak_ptr<>` in terms of functionality, but there can be performance and safety advantages.

```cpp
    #include "mserefcounting.h"
    #include "msecregistered.h"
    #include "mseregistered.h"
    
    void main(int argc, char* argv[]) {

        class CRCNode;

        typedef mse::TRefCountingFixedPointer<CRCNode> rcnode_strongptr_t;            // owning pointer of a CRCNode
        typedef mse::TNDRegisteredObj<rcnode_strongptr_t> rcnode_strongptr_regobj_t; // registered version of above so that you can obtain a (weak)
                                                                                    // registered pointer to it
        typedef mse::TNDRegisteredPointer<rcnode_strongptr_t> rcnode_strongptr_weakptr_t; // (weak) registered pointer to owning pointer of a CRCNode

        class CRCNode {
        public:
            CRCNode(mse::TRegisteredPointer<mse::CInt> node_count_ptr
                , rcnode_strongptr_weakptr_t root_ptr_ptr) : m_node_count_ptr(node_count_ptr), m_root_ptr_ptr(root_ptr_ptr) {
                (*node_count_ptr) += 1;
            }
            CRCNode(mse::TRegisteredPointer<mse::CInt> node_count_ptr) : m_node_count_ptr(node_count_ptr) {
                (*node_count_ptr) += 1;
            }
            virtual ~CRCNode() {
                (*m_node_count_ptr) -= 1;
            }
            static rcnode_strongptr_regobj_t MakeRoot(mse::TRegisteredPointer<mse::CInt> node_count_ptr) {
                auto retval = rcnode_strongptr_regobj_t{ mse::make_refcounting<CRCNode>(node_count_ptr) };
                (*retval).m_root_ptr_ptr = &retval;
                return retval;
            }
            static auto MaybeStrongChildPtr(const rcnode_strongptr_regobj_t this_ptr) { return this_ptr->m_maybe_child_ptr; }
            static rcnode_strongptr_regobj_t MakeChild(const rcnode_strongptr_regobj_t this_ptr) {
                this_ptr->m_maybe_child_ptr.emplace(rcnode_strongptr_regobj_t{ mse::make_refcounting<CRCNode>(this_ptr->m_node_count_ptr, this_ptr->m_root_ptr_ptr) });
                return this_ptr->m_maybe_child_ptr.value();
            }
            static void DisposeOfChild(const rcnode_strongptr_regobj_t this_ptr) {
                this_ptr->m_maybe_child_ptr.reset();
            }

        private:
            mse::TRegisteredPointer<mse::CInt> m_node_count_ptr;
            rcnode_strongptr_weakptr_t m_root_ptr_ptr;
            CRCNODE_STD_OPTIONAL<rcnode_strongptr_regobj_t> m_maybe_child_ptr;
        };

        mse::TRegisteredObj<mse::CInt> node_counter = 0;
        {
            auto root_owner_ptr = CRCNode::MakeRoot(&node_counter);
            auto kid1 = root_owner_ptr->MakeChild(root_owner_ptr);
            {
                auto kid2 = kid1->MakeChild(kid1);
                auto kid3 = kid2->MakeChild(kid2);
            }
            assert(4 == node_counter);
            kid1->DisposeOfChild(kid1);
            assert(2 == node_counter);
        }
        assert(0 == node_counter);
    }
```


### Scope pointers
Scope pointers are pointers that live (only) to the end of the scope in which they (or their owners) are declared, and that point (only) to objects that live at least that long. Because scope pointers are only allowed to target objects that are known, at compile-time, to outlive them, they should be memory safe without need of any run-time overhead.

Scope pointers generally satisfy the restrictions the lifetime checker would impose on raw pointers, and could be considered as basically a stand-in for raw pointers for situations where a complete lifetime checker is not available. When a lifetime checker is/becomes available, scope pointers can be "disabled", i.e. aliased to their corresponding raw pointers, by simply defining the `MSE_SCOPEPOINTER_DISABLED` preprocessor symbol. 

Unlike other elements of the library, proper use of scope types is not fully enforced in the type system. Full enforcement requires the use of a tool like [scpptool](https://github.com/duneroadrunner/scpptool) (or the eventually-completed lifetime checker). Run-time checking is (still) available to catch unsafe misuses of scope types by defining the `MSE_SCOPEPOINTER_RUNTIME_CHECKS_ENABLED` preprocessor symbol. (Though this is (now) kind of redundant if you're using an aforementioned enforcement tool.)

Scope pointers usually point to scope objects. Scope objects are objects that live to the end of the scope in which they are declared. You can designate pretty much any type to be a scope object type by wrapping it in the `mse::TXScopeObj<>` (transparent) wrapper template. As with registered objects, this wrapper does not support some types that cannot act as a base class. For `int`, `bool` and `size_t` use the safer [substitutes](#cndint-cndsize_t-and-cndbool) that can act as base classes. 

The rules for using scope pointers and objects are essentially as follows:

- Objects of scope type (types whose name starts with "TXScope" or "xscope") must be local ([non](#thread_local)-[static](#static-and-global-variables)) automatic variables.
	- Basically allocated on the stack.
- Note that scope pointers are themselves scope objects and must adhere to the same restrictions.
- Do not use scope types as members of classes or structs.
	- Note that you can use the [`mse::make_xscope_pointer_to_member_v2()`](#make_pointer_to_member_v2) function to obtain a scope pointer to a member of a scope object. So it's generally not necessary for any class/struct member to be declared as a scope object.
	- In the uncommon cases that you really want to use a scope type as a member of a class or struct, that class or struct must itself be a scope type. User defined scope types must adhere to the [rules](#defining-your-own-scope-types) of scope types.
- Do not use scope types as function return types.
	- In the uncommon cases that you really want to use a scope type as a function return type, it must be wrapped in the [`mse::TXScopeReturnValue<>`](#return_value) transparent template wrapper.
	- `mse::TXScopeReturnValue<>` will not accept non-owning scope pointer types. Pretty much the only time you would legitimately want to return a non-owning pointer to a scope object is when that pointer is one of the function's input parameters. In those cases you can use the [`xscope_chosen()`](#xscope_chosen) function.

Again, most inadvertent misuses of scope objects should result in compile errors. The remaining potential misuses would be caught by a companion tool like the aforementioned [scpptool](https://github.com/duneroadrunner/scpptool). But these rules should be intuitive enough that adherence should be fairly natural. Just remember that the safety of scope pointers is premised on the fact that scope objects are never deallocated before the end of the scope in which they are declared, and (non-owning) scope pointers (and any copies of them) never survive beyond the scope in which they are declared, so that a scope pointer cannot outlive its target scope object.

Generally, there are two types of scope pointers you might use, [`TXScopeOwnerPointer<>`](#txscopeownerpointer) and [`TXScopeFixedPointer<>`](#txscopefixedpointer). `TXScopeOwnerPointer<>` is kind of like `std::unique_ptr<>`, but restricted by the rules of scope objects. It creates an instance of a given class on the heap and destroys that instance in its destructor. 
`TXScopeFixedPointer<>` is a (zero-overhead) "non-owning" pointer to objects that are known (at compile-time) to outlive it. 

### TXScopeFixedPointer
`TXScopeFixedPointer<>`s are (zero-overhead) "non-owning", non-retargetable pointers. They may only be declared such that they do not outlive the scope in which they are declared (i.e. basically as non-static local variables, and may only point to objects known (at compile-time) to live at least that long. 

usage example:

```cpp
    #include "msescope.h"
    
    void main(int argc, char* argv[]) {
        class A {
        public:
            A(int x) : b(x) {}
            A(const A& _X) : b(_X.b) {}
            virtual ~A() {}
            A& operator=(const A& _X) { b = _X.b; return (*this); }

            int b = 3;
        };
        class B {
        public:
            static int foo2(mse::TXScopeFixedPointer<A> A_scpifptr) { return A_scpifptr->b; }
            static int foo3(mse::TXScopeFixedConstPointer<A> A_scpifcptr) { return A_scpifcptr->b; }
        protected:
            ~B() {}
        };
    
        mse::TXScopeObj<A> a_scpobj(5);
        /* note that the '&' ("ampersand") operator is overloaded to return a mse::TXScopeFixedPointer<>  */
        int res1 = (&a_scpobj)->b;
        int res2 = B::foo2(&a_scpobj);
        int res3 = B::foo3(&a_scpobj);

        /* You can also use the make_xscope() function to obtain a scope object from a given value. */
        auto a2_scpobj = mse::make_xscope(A(7));
    }
```

#### TXScopeFixedConstPointer

### TXScopeOwnerPointer
`TXScopeOwnerPointer<>` is kind of like an `std::unique_ptr<>` whose use is restricted by the rules of scope objects. So, it must live to the end of the scope in which it is declared (i.e. basically be declared as a non-static local (or `thread_local` variable)) and can only be used as a member of objects which are themselves scope objects. You can use it when you want to give scope lifetime to objects that are too large to be declared directly on the stack. Unlike `std::unique_ptr<>`s, you may not use `std::move()` with `TXScopeOwnerPointer<>`s. If you're not using a conformance helper tool like [scpptool](https://github.com/duneroadrunner/scpptool) to enforce this, you can disable `TXScopeOwnerPointer<>`'s move constructor by defining the `MSE_RESTRICT_TXSCOPEOWNERPOINTER_MOVABILITY` preprocessor symbol.

Instead of its constructor taking a native pointer pointing to an already allocated object, it allocates the object itself and forwards its construction arguments to the object's constructor. You may also use `mse::make_xscope_owner<>()` to create a `TXScopeOwnerPointer<>` in a manner akin to `std::make_unique<>()`.

usage example:

```cpp
    #include "msescope.h"
    
    void main(int argc, char* argv[]) {
        class A {
        public:
            A(int x) : b(x) {}
            A(const A& _X) : b(_X.b) {}
            virtual ~A() {}
            A& operator=(const A& _X) { b = _X.b; return (*this); }

            int b = 3;
        };
        class B {
        public:
            static int foo2(mse::TXScopeFixedPointer<A> A_scpfptr) { return A_scpfptr->b; }
            static int foo3(mse::TXScopeFixedConstPointer<A> A_scpfcptr) { return A_scpfcptr->b; }
        protected:
            ~B() {}
        };
    
        /* You can either pass the object's constructor arguments to mse::TXScopeOwnerPointer<>'s constructor, */
        mse::TXScopeOwnerPointer<A> xscp_a_ownerptr(7);
        /* or you can use mse::make_xscope_owner<>() in a manner akin to std::make_unique<>() */
        auto xscp_a_ownerptr2 = mse::make_xscope_owner<A>(7);
        int res4 = B::foo2(xscp_a_ownerptr);
        int res4b = B::foo2(&(*xscp_a_ownerptr));
    }
```

### make_xscope_strong_pointer_store()

`make_xscope_strong_pointer_store()` returns a scope object that holds a copy of the given strong pointer and allows you to obtain a corresponding scope pointer. Supported strong pointers include ones like [reference counting pointers](#reference-counting-pointers), [norad pointers](#norad-pointers) and pointers to [asynchronously shared objects](#asynchronously-shared-objects) (and scope pointers themselves for the sake of completeness).

usage example:

```cpp
    #include "msescope.h"
    #include "mserefcounting.h"
    
    void main(int argc, char* argv[]) {
        class A {
        public:
            A(int x) : b(x) {}
            A(const A& _X) : b(_X.b) {}
            virtual ~A() {}
            A& operator=(const A& _X) { b = _X.b; return (*this); }

            int b = 3;
        };
        class B {
        public:
            static int foo2(mse::TXScopeFixedPointer<A> A_scpfptr) { return A_scpfptr->b; }
            static int foo3(mse::TXScopeFixedConstPointer<A> A_scpfcptr) { return A_scpfcptr->b; }
        protected:
            ~B() {}
        };
    
        /* Using mse::make_xscope_strong_pointer_store(), you can obtain a scope pointer from a refcounting pointer. */
        /* Let's make it a const refcounting pointer, just for variety. */
        mse::TRefCountingFixedConstPointer<A> refc_cptr1 = mse::make_refcounting<A>(11);
        auto xscp_refc_cstore = mse::make_xscope_strong_pointer_store(refc_cptr1);
        auto xscp_cptr1 = xscp_refc_cstore.xscope_ptr();
        int res6 = B::foo3(xscp_cptr1);
        mse::TXScopeFixedConstPointer<A> xscp_cptr2 = xscp_cptr1;
        A res7 = *xscp_cptr2;
    }
```

### Retargetable references to scope objects

### TRegisteredProxyPointer

Scope pointers have limitations, (currently) for example, in terms of their ability to be retargeted, and their ability to be stored in dynamic containers. When necessary, you can circumvent these sorts of limitations by creating "registered proxy" pointers corresponding to given scope pointers. 

Registered proxy pointers are basically just [registered pointers](#registered-pointers) which target scope pointers, except that (more conveniently) they dereference to the scope pointer's target object rather than the scope pointer itself. That is, a `TRegisteredProxyPointer<T>` is similar to a `TRegisteredConstPointer<TXScopeFixedPointer<T> >`, except that it dereferences to the object of type `T` rather than the `TXScopeFixedPointer<T>`. They are also convertible back to scope pointers when needed. 

To be clear, a `TRegisteredProxyPointer<T>` doesn't have any functionality that a `TRegisteredConstPointer<TXScopeFixedPointer<T> >` does not already have, it's just more convenient in some situations.

usage example:

```cpp
    #include "msescope.h"
    #include "mseregisteredproxy.h"
    #include "msemsestring.h"
    
    void main(int argc, char* argv[]) {
        class CB {
        public:
            static void foo1(mse::TXScopeFixedPointer<mse::nii_string> xscope_ptr1) {
                std::cout << *xscope_ptr1;
            }
        };
        auto xscp_nstring1 = mse::make_xscope(mse::nii_string("some text"));

        CB::foo1(&xscp_nstring1);

        {
            auto xscp_proxy_obj1 = mse::make_xscope_registered_proxy(&xscp_nstring1);
            mse::TRegisteredProxyPointer<mse::nii_string> registered_proxy_ptr1 = mse::registered_proxy_fptr(xscp_proxy_obj1);

            /* Registered proxy pointers implicitly convert to scope pointers. */
            CB::foo1(registered_proxy_ptr1);

            auto xscp_nstring2 = mse::make_xscope(mse::nii_string("some other text"));
            auto xscp_proxy_obj2 = mse::make_xscope_registered_proxy(&xscp_nstring2);

            /* Registered proxy pointers are retargetable. */
            registered_proxy_ptr1 = mse::registered_proxy_fptr(xscp_proxy_obj2);

            CB::foo1(registered_proxy_ptr1);

            {
                auto xscp_nstring3 = mse::make_xscope(mse::nii_string("other text"));

                {
                    auto xscp_proxy_obj3 = mse::make_xscope_registered_proxy(&xscp_nstring3);

                    {
                        registered_proxy_ptr1 = mse::registered_proxy_fptr(xscp_proxy_obj3);
                    }

                    CB::foo1(registered_proxy_ptr1);
                }
                /* Attempting to dereference registered_proxy_ptr1 here would result in an exception. */
                //*registered_proxy_ptr1;
            }
        }
    }
```

#### TRegisteredProxyPointer, TRegisteredProxyNotNullPointer, TRegisteredProxyFixedPointer, TRegisteredProxyConstPointer, TRegisteredProxyNotNullConstPointer, TRegisteredProxyFixedConstPointer

### TNoradProxyPointer

"norad proxy" pointers are to ["registered proxy" pointers](#tregisteredproxypointer) as [norad pointers](#norad-pointers) are to [registered pointers](#registered-pointers). That is, the difference is that the destruction of a a norad proxy object while a norad proxy pointer still references it will result in program termination. So like their registered counterparts:

Norad proxy pointers are basically just norad pointers which target scope pointers, except that (more conveniently) they dereference to the scope pointer's target object rather than the scope pointer itself. That is, a `TNoradProxyPointer<T>` is similar to a `TNoradPointer<TXScopeFixedPointer<T> >`, except that it dereferences to the object of type `T` rather than the `TXScopeFixedPointer<T>`. They are also convertible back to scope pointers when needed.

To be clear, a `TNoradProxyPointer<T>` doesn't have any functionality that a `TNoradPointer<TXScopeFixedPointer<T> >` does not already have, it's just more convenient in some  situations.

usage example:

```cpp
    #include "msescope.h"
    #include "msenoradproxy.h"
    #include "msemsestring.h"
    
    void main(int argc, char* argv[]) {
        class CB {
        public:
            static void foo1(mse::TXScopeFixedPointer<mse::nii_string> xscope_ptr1) {
                std::cout << *xscope_ptr1;
            }
        };
        auto xscp_nstring1 = mse::make_xscope(mse::nii_string("some text"));

        CB::foo1(&xscp_nstring1);

        {
            auto xscp_proxy_obj1 = mse::make_xscope_norad_proxy(&xscp_nstring1);
            mse::TNoradProxyPointer<mse::nii_string> norad_proxy_ptr1 = mse::norad_proxy_fptr(xscp_proxy_obj1);

            /* Norad proxy pointers implicitly convert to scope pointers. */
            CB::foo1(norad_proxy_ptr1);

            auto xscp_nstring2 = mse::make_xscope(mse::nii_string("some other text"));
            auto xscp_proxy_obj2 = mse::make_xscope_norad_proxy(&xscp_nstring2);

            /* Norad proxy pointers are retargetable. */
            norad_proxy_ptr1 = mse::norad_proxy_fptr(xscp_proxy_obj2);

            CB::foo1(norad_proxy_ptr1);

            {
                auto xscp_nstring3 = mse::make_xscope(mse::nii_string("other text"));

                {
                    auto xscp_proxy_obj3 = mse::make_xscope_norad_proxy(&xscp_nstring3);

                    {
                        norad_proxy_ptr1 = mse::norad_proxy_fptr(xscp_proxy_obj3);
                    }

                    CB::foo1(norad_proxy_ptr1);

                    /* Forgetting to detarget norad proxy pointers before their target object is destroyed (in this case, by
                    going out of scope) would result in program termination. */
                    norad_proxy_ptr1 = nullptr;
                }
            }
        }
    }
```

#### TNoradProxyPointer, TNoradProxyNotNullPointer, TNoradProxyFixedPointer, TNoradProxyConstPointer, TNoradProxyNotNullConstPointer, TNoradProxyFixedConstPointer

### xscope_chosen()

[*provisional*]

For safety reasons, non-owning scope pointers (or any objects containing a scope reference) are not permitted to be used as function return values. (The [`return_value()`](#return_value) function wrapper enforces this.) Pretty much the only time you'd legitimately want to do this is when the returned pointer is one of the input parameters. An example might be a `min(a, b)` function which takes two objects by reference and returns the reference to the lesser of the two objects. For these cases you could use the `xscope_chosen()` function which takes two objects of the same type (in this case it will be two scope pointers) and returns (a copy of) one of the objects (scope pointers), which one depending on the value of a given "decider" function. You could use this function to implement the equivalent of a `min(a, b)` function like so:

```cpp
    #include "msescope.h"
    
    void main(int argc, char* argv[]) {
    
        class A {
        public:
            A(int x) : b(x) {}
            A(const A& _X) : b(_X.b) {}
            virtual ~A() {}
            bool operator<(const A& _X) const { return (b < _X.b); }

            int b = 3;
        };
    
        mse::TXScopeObj<A> a_scpobj(5);
        mse::TXScopeOwnerPointer<A> xscp_a_ownerptr(7);
    
        auto xscp_a_ptr5 = &a_scpobj;
        auto xscp_a_ptr6 = &(*xscp_a_ownerptr);
        const auto second_arg_is_smaller_fn = [](const auto xscp_a_ptr1, const auto xscp_a_ptr2) { return (*xscp_a_ptr2) < (*xscp_a_ptr1); };
        auto xscp_min_ptr1 = mse::xscope_chosen(second_arg_is_smaller_fn, xscp_a_ptr5, xscp_a_ptr6);
	/* xscp_min_ptr1 should be a copy of xscp_a_ptr5 */
        assert(5 == xscp_min_ptr1->b);
    }
```

### as_a_returnable_fparam()

[*provisional*]

Another alternative if you want to return a scope pointer (or any object containing a scope reference) function parameter is to (immediately) create a "returnable" version of it using the `rsv::as_a_returnable_fparam()` function.

Normally the [`return_value()`](#return_value) function wrapper will reject (with a compile error) scope pointers as unsafe return values. But the `rsv::as_a_returnable_fparam()` function can be used to (immediately) obtain a "returnable" version of a scope pointer function parameter. Because it's generally safe to return a reference to an object if that reference was passed as a parameter. Well, as long as the object is not a temporary object. So unlike [`rsv::as_an_fparam()`](#as_an_fparam), `rsv::as_a_returnable_fparam()` will not accept scope pointers to temporaries, as returning a (scope) reference to a temporary would be unsafe even if the reference was passed as a function parameter. So for scope reference parameters you have to choose between being able to use it as a return value, or supporting references to temporaries. (Or neither.)

Note that using the `rsv::as_a_returnable_fparam()` function on anything other than local function parameters would be unsafe, and wouldn't be prevented by the type system. Safety enforcement is reliant on a companion tool like [scpptool](https://github.com/duneroadrunner/scpptool).

usage example:

```cpp
#include "msescope.h"
#include "msemsestring.h"
    
class H {
public:
    /* This function will be used to demonstrate using rsv::as_a_returnable_fparam() to enable template functions to
    return one of their function parameters, potentially of the scope reference variety which would otherwise be
    rejected (with a compile error) as an unsafe return value. */
    template<class _TString1Pointer, class _TString2Pointer>
    static auto longest(const _TString1Pointer& string1_ptr, const _TString2Pointer& string2_ptr) {
        auto l_string1_ptr = mse::rsv::as_a_returnable_fparam(string1_ptr);
        auto l_string2_ptr = mse::rsv::as_a_returnable_fparam(string2_ptr);

        if (l_string1_ptr->length() > l_string2_ptr->length()) {
            /* If string1_ptr were a regular TXScopeFixedPointer<mse::nii_string> and we tried to return it
            directly instead of l_string1_ptr, it would have induced a compile error. */

            return mse::return_value(l_string1_ptr);
        }
        else {
            /* mse::return_value() usually just returns its input argument unmolested, but in this case, where the
            argument was obtained from the mse::rsv::as_a_returnable_fparam() it will convert it back the type of
            the original function parameter (thereby removing the "returnability" attribute that was added by
            mse::rsv::as_a_returnable_fparam()). */

            return mse::return_value(l_string2_ptr);
        }
    }
    /* This function will be used to demonstrate nested function calls (safely) returning scope pointer/references. */
    template<class _TString1Pointer, class _TString2Pointer>
    static auto nested_longest(const _TString1Pointer& string1_ptr, const _TString2Pointer& string2_ptr) {
        auto l_string1_ptr = mse::rsv::as_a_returnable_fparam(string1_ptr);
        auto l_string2_ptr = mse::rsv::as_a_returnable_fparam(string2_ptr);

        /* Note that with functions (potentially) returning a scope reference parameter (or an object derived
        from a scope reference parameter), you generally want the function to be a template function with the
        scope reference parameters types being (deduced) template parameters, as with this function, rather
        than more explicitly specified scope reference types or template types. The reason for this is that in
        the case of nested function calls, the number of nested function calls from which a scope reference
        object can be (safely) returned is embedded in the scope reference object's type. That is, the exact
        type of a returnable scope reference object depends on how many (levels of) nested function calls it
        has been passed through. And you generally want your functions that return scope reference objects to
        preserve the exact type of the scope reference passed, otherwise you may not be allowed (i.e. induced
        compile error) to return the scope reference all the way back to the scope it originated from. */

        return mse::return_value(longest(l_string1_ptr, l_string2_ptr));
    }

    struct CE {
        mse::nii_string m_string1 = "abcde";
    };

    /* This function demonstrates scope reference objects inheriting the "returnability" trait from the reference objects
    from which they were derived. */
    template<class _TPointer1>
    static auto xscope_string_const_section_to_member_of_CE(_TPointer1 CE_ptr) {
        auto returnable_CE_ptr = mse::rsv::as_a_returnable_fparam(CE_ptr);

        /* "Pointers to members" based on returnable pointers inherit the "returnability". */
        auto returnable_cpointer_to_member = mse::make_xscope_const_pointer_to_member_v2(returnable_CE_ptr, &CE::m_string1);

        /* "scope nrp string const sections" based on returnable pointers (or iterators) inherit the "returnability". */
        auto returnable_string_const_section = mse::make_xscope_string_const_section(returnable_cpointer_to_member);
        /* Subsections of returnable sections inherit the "returnability". */
        auto returnable_string_const_section2 = mse::make_xscope_subsection(returnable_string_const_section, 1, 3);
        return mse::return_value(returnable_string_const_section2);
    }
    template<class _TPointer1>
    static auto nested_xscope_string_const_section_to_member_of_CE(_TPointer1 CE_ptr) {
        auto returnable_CE_ptr = mse::rsv::as_a_returnable_fparam(CE_ptr);

        return mse::return_value(xscope_string_const_section_to_member_of_CE(returnable_CE_ptr));
    }
};
    
void main(int argc, char* argv[]) {
    mse::TXScopeObj<mse::nii_string> xscope_string1 = "abc";
    mse::TXScopeObj<mse::nii_string> xscope_string2 = "abcd";

    auto longer_string_xscpptr = H::longest(&xscope_string1, &xscope_string2);
    auto length1 = (*longer_string_xscpptr).length();

    auto longer_string_xscpptr2 = H::nested_longest(&xscope_string1, &xscope_string2);
    auto length2 = (*longer_string_xscpptr2).length();

    mse::TXScopeObj<H::CE> e_xscpobj;
    auto xscope_string_const_section1 = H::xscope_string_const_section_to_member_of_CE(&e_xscpobj);
    assert(xscope_string_const_section1 == "bcd");

    auto xscope_string_const_section2 = H::nested_xscope_string_const_section_to_member_of_CE(&e_xscpobj);
    assert(xscope_string_const_section2 == "bcd");
}
```

### as_an_fparam()

[*provisional*]

`rsv::TFParam<>` is just a transparent template wrapper for function parameter declarations. In most cases use of this wrapper is not necessary, but in some cases it enables functionality only available to variables that are function parameters. Specifically, it allows functions to support arguments that are scope pointer/references to temporary objects. For safety reasons, by default, scope pointer/references to temporaries are actually "functionally disabled" types distinct from regular scope pointer/reference types. Because it's safe to do so in the case of function parameters, the `rsv::TFParam<>` wrapper enables certain scope pointer/reference types (like `TXScopeFixedConstPointer<>`, and "[random access const sections](#txscoperandomaccesssection-txscoperandomaccessconstsection-trandomaccesssection-trandomaccessconstsection)" scope types) to be constructed from their "functionally disabled" counterparts.

In the case of function templates, sometimes you want the parameter types to be auto-deduced, and use of the `mse::rsv::TFParam<>` wrapper can interfere with that. In those cases you can instead convert parameters to their wrapped type after-the-fact using the `rsv::as_an_fparam()` function. Note that using this function (or the `rsv::TFParam<>` wrapper) on anything other than function parameters would be unsafe, and wouldn't be prevented by the type system. Safety enforcement is reliant on a companion tool like [scpptool](https://github.com/duneroadrunner/scpptool).

`rsv::TXScopeFParam<>` and `rsv::xscope_as_an_fparam()` can be used for situations when the types are necessarily scope types.

```cpp
#include "msescope.h"
#include "msemsestring.h"
#include "msepoly.h"
    
class H {
public:
    /* This function will be used to demonstrate using rsv::as_an_fparam() to enable template functions to accept scope 
    pointers to temporary objects. */
    template<class _TPointer1, class _TPointer2>
    static bool second_is_longer(_TPointer1&& string1_xscpptr, _TPointer2&& string2_xscpptr) {
        auto l_string1_xscpptr = mse::rsv::as_an_fparam(std::forward<decltype(string1_xscpptr)>(string1_xscpptr));
        auto l_string2_xscpptr = mse::rsv::as_an_fparam(std::forward<decltype(string2_xscpptr)>(string2_xscpptr));
        return (l_string1_xscpptr->length() > l_string2_xscpptr->length()) ? false : true;
    }
};
    
void main(int argc, char* argv[]) {
    class CD {
    public:
        static bool second_is_longer(mse::rsv::TXScopeFParam<mse::TXScopeFixedConstPointer<mse::nii_string> > string1_xscpptr
            , mse::rsv::TXScopeFParam<mse::TXScopeFixedConstPointer<mse::nii_string> > string2_xscpptr) {

            return (string1_xscpptr->length() > string2_xscpptr->length()) ? false : true;
        }

        static bool second_is_longer_any(mse::rsv::TXScopeFParam<mse::TXScopeAnyConstPointer<mse::nii_string> > string1_xscpptr
            , mse::rsv::TXScopeFParam<mse::TXScopeAnyConstPointer<mse::nii_string> > string2_xscpptr) {
            return (string1_xscpptr->length() > string2_xscpptr->length()) ? false : true;
        }

        static bool second_is_longer_poly(mse::rsv::TXScopeFParam<mse::TXScopePolyConstPointer<mse::nii_string> > string1_xscpptr
            , mse::rsv::TXScopeFParam<mse::TXScopePolyConstPointer<mse::nii_string> > string2_xscpptr) {
            return (string1_xscpptr->length() > string2_xscpptr->length()) ? false : true;
        }
    };

    mse::TXScopeObj<mse::nii_string> xscope_string1 = "abc";
    /* Here we're using the pointer_to() function to obtain a ("caged") pointer to the temporary scope object. The '&'
    (ampersand) operator would also work, but would not correspond to valid native C++, as C++ does not support taking
    the address of an r-value. */
    auto res1 = CD::second_is_longer(&xscope_string1, mse::pointer_to(mse::TXScopeObj<mse::nii_string>(xscope_string1 + "de")));
    auto res2 = H::second_is_longer(&xscope_string1, mse::pointer_to(mse::TXScopeObj<mse::nii_string>(xscope_string1 + "de")));
    auto res3 = CD::second_is_longer_any(&xscope_string1, mse::pointer_to(mse::TXScopeObj<mse::nii_string>(xscope_string1 + "de")));
    auto res4 = CD::second_is_longer_poly(&xscope_string1, mse::pointer_to(mse::TXScopeObj<mse::nii_string>(xscope_string1 + "de")));
}
```

### Conformance helpers

### return_value()

The safety of non-owning scope pointers is premised on the fact that they will not outlive the scope in which they are declared. So returning a non-owning scope pointer, or any object that contains or owns a non-owning scope pointer, from a function would be potentially unsafe. However, it could be safe to return a scope object if that object does not contain or own any non-owning scope pointers.

The `return_value()` function just returns its argument and verifies that it is of a type that is safe to return from a function (basically, doesn't contain any scope pointers). If not it will induce a compile error. Functions that do or could return scope types should wrap their return value with this function. Note that the [scpptool](https://github.com/duneroadrunner/scpptool) actually enforces this.

usage example:

```cpp
    #include "msescope.h"
    #include "mseregistered.h"
    #include "msemstdstring.h"
    #include "mseoptional.h"
    
    class J {
    public:
        template<typename _TParam>
        static auto foo10(_TParam param) {
            _TParam l_obj = param;

            auto ret_ptr = &l_obj;
            /* ret_ptr may be a smart pointer if the 'operator&' is overloaded.*/
            /* If ret_ptr is a scope pointer (or native pointer) it would be unsafe to return it. In those cases the
             'mse::return_value(ret_ptr)' expression will induce a compile error. */
            /* Functions that could return a scope type need to wrap their return value with the return_value() function. */
            return mse::return_value(ret_ptr);
        }
    };
    
    void main() {
        class CB {
        public:
            /* It's generally not necessary for a function return type to be a scope type. Even if the return value
            is of a scope type, you can usually just use the underlying (non-scope) type of the scope object as the
            return type. */
            static mse::mstd::string foo1() {
                mse::TXScopeObj<mse::mstd::string> xscp_string1("some text");
                return mse::return_value(xscp_string1);
            }

            /* In the less common case where the scope type doesn't have an underlying non-scope type, it may be safe
            to return the scope object.  */
            static mse::xscope_optional<mse::mstd::string> foo2() {
                mse::xscope_optional<mse::mstd::string> xscp_returnable_obj1(mse::mstd::string("some text"));
                return mse::return_value(xscp_returnable_obj1);
            }

            static auto foo3() {
                mse::TXScopeObj<mse::mstd::string> xscp_string1("some text");
                auto xscp_ptr = &xscp_string1;

                /* Returning a scope pointer (to a local variable for example) could be unsafe. So mse::return_value()
                induces a compile error if the return value contains a scope pointer/reference. */
                //return mse::return_value(xscp_ptr);

                return mse::return_value(xscp_string1);
            }
        };

        mse::TXScopeObj<mse::mstd::string> xscp_res1 = CB::foo1();
        mse::xscope_optional<mse::mstd::string> xscp_res2 = CB::foo2();

        typedef mse::TXScopeObj<mse::mstd::string> xscope_string_t;
        xscope_string_t xscp_str1 = "some text";

        mse::TNDRegisteredObj<mse::mstd::string> reg_str1 = "some text";
        auto reg_ptr_res1 = J::foo10(reg_str1);
        //auto xscp_ptr_res1 = J::foo10(xscp_str1); // <-- would induce a compile error inside J::foo10() 
    }
```

### TMemberObj

Scope types have built in protection that prevents them from being allocated dynamically. But those protections are circumvented if a scope type is used as a member of a class or struct. So `TMemberObj<>` is a transparent wrapper that can be used to wrap class/struct member types to ensure that they are not scope types. This is particularly relevant in cases when the member type is, or is derived from, a template parameter. Note though that this wrapper is no longer really necessary if you're using a conformance-enforcing tool like [scpptool](https://github.com/duneroadrunner/scpptool) that will detect such potential misuses.

### Defining your own scope types

example:
```cpp
#include "msescope.h"
#include "mseoptional.h"

void main(int argc, char* argv[]) {

    /* Defining your own scope types. */

    /* It is (intended to be) uncommon to need to define your own scope types. In general, if you want to use a
    type as a scope type, you can just wrap it with the mse::TXScopeObj<> template. */

    /* But in cases where you're going to use a scope type as a member of a class or struct, that class or struct
    must itself be a scope type. Improperly defining a scope type could result in unsafe code. Use of a safety
    enforcement helper tool, like scpptool, is required to ensure that your scope type is defined properly. 
    And note that mse::xscope_tuple<> also supports scope type elements/fields. */

    /* Scope types need to publicly inherit from mse::rsv::XScopeTagBase. And by convention, be named with a
    prefix indicating that it's a scope type. */
    class xscope_my_type1 : public mse::rsv::XScopeTagBase {
    public:
        xscope_my_type1(const mse::xscope_optional<mse::mstd::string>& xscp_maybe_string)
            : m_xscp_maybe_string1(xscp_maybe_string) {}

        mse::xscope_optional<mse::mstd::string> m_xscp_maybe_string1;
    };

    /* If your type contains or owns any non-owning scope pointers, then it must also publicly inherit
    from mse::rsv::ContainsNonOwningScopeReferenceTagBase. If your type contains or owns any item that can be
    independently targeted by scope pointers (i.e. basically has a '&' ("address of" operator) that yeilds
    a scope pointer), then it must also publicly inherit from mse::rsv::ReferenceableByScopePointerTagBase.
    Failure to do so could result in unsafe code. The scpptool, for example, will check for any such failure. */
    class xscope_my_type2 : public mse::rsv::XScopeTagBase, public mse::rsv::ContainsNonOwningScopeReferenceTagBase
        , public mse::rsv::ReferenceableByScopePointerTagBase
    {
    public:
        typedef mse::TXScopeFixedConstPointer<mse::mstd::string> xscope_string_ptr_t;

        xscope_my_type2(const mse::xscope_optional<xscope_string_ptr_t>& xscp_maybe_string_ptr) : m_xscp_maybe_string_ptr(xscp_maybe_string_ptr) {}

        /* This item (potentially) contains a non-owning scope pointer. */
        mse::xscope_optional<xscope_string_ptr_t> m_xscp_maybe_string_ptr;

        /* This item owns an object that can be independently targeted by scope pointers. That is,
        &(*m_xscp_string_owner_ptr) yields a scope pointer. */
        mse::TXScopeOwnerPointer<mse::mstd::string> m_xscp_string_owner_ptr;
    };
}
```

### make_pointer_to_member_v2()
If you need a safe pointer to a member of a class/struct, you could declare the member itself to be a registered object (or a reference counting pointer). But often a preferable option is to use `make_pointer_to_member_v2()`. This function takes a safe pointer to the containing class/struct and a "[pointer-to-member](http://en.cppreference.com/w/cpp/language/pointer#Pointers_to_members)" indicating the member you want to target, and combines them to create a safe pointer to the member. The actual type of the returned pointer varies depending on the types of the parameters passed.

usage example:

```cpp
    /* Including "msepoly.h" is not required to use mse::make_pointer_to_member_v2(). It just happens to include all the
    other include files used by this example. */
    #include "msepoly.h"
    
    class H {
    public:
        /* A member function that provides a safe pointer/reference to a class/struct member is going to need to
        take a safe version of the "this" pointer as a parameter. */
        template<class this_type>
        static auto safe_pointer_to_member_string1(this_type safe_this) {
            return mse::make_pointer_to_member_v2(safe_this, &H::m_string1);
        }
    
        mse::nii_string m_string1 = "initial text";
    };
    
    void main() {
        /* To demonstrate, first we'll declare some objects such that we can obtain safe pointers to those
        objects. For better or worse, this library provides a bunch of different safe pointers types. */
        mse::TXScopeObj<H> h_scpobj;
        auto h_refcptr = mse::make_refcounting<H>();
        mse::TRegisteredObj<H> h_regobj;
        mse::TCRegisteredObj<H> h_rlxregobj;
    
        /* Safe iterators are a type of safe pointer too. */
        mse::mstd::vector<H> h_mstdvec;
        h_mstdvec.resize(1);
        auto h_mstdvec_iter = h_mstdvec.begin();
        mse::us::msevector<H> h_msevec;
        h_msevec.resize(1);
        auto h_msevec_ipointer = h_msevec.ibegin();
        auto h_msevec_ssiter = h_msevec.ss_begin();
    
        /* And don't forget the safe async sharing pointers. */
        auto h_access_requester = mse::make_asyncsharedv2readwrite<ShareableH>();
        auto h_writelock_ptr = h_access_requester.writelock_ptr();
        auto h_stdshared_const_ptr = mse::make_stdsharedimmutable<H>();
    
        {
            /* So here's how you get a safe pointer to a member of the object using mse::make_pointer_to_member_v2(). */
            auto h_string1_scpptr = mse::make_xscope_pointer_to_member_v2(&h_scpobj, &H::m_string1);
            (*h_string1_scpptr) = "some new text";
            auto h_string1_scp_const_ptr = mse::make_xscope_const_pointer_to_member_v2(&h_scpobj, &H::m_string1);
    
            auto h_string1_refcptr = mse::make_pointer_to_member_v2(h_refcptr, &H::m_string1);
            (*h_string1_refcptr) = "some new text";
    
            auto h_string1_regptr = mse::make_pointer_to_member_v2(&h_regobj, &H::m_string1);
            (*h_string1_regptr) = "some new text";
    
            auto h_string1_rlxregptr = mse::make_pointer_to_member_v2(&h_rlxregobj, &H::m_string1);
            (*h_string1_rlxregptr) = "some new text";
    
            auto h_string1_mstdvec_iter = mse::make_pointer_to_member_v2(h_mstdvec_iter, &H::m_string1);
            (*h_string1_mstdvec_iter) = "some new text";
    
            auto h_string1_msevec_ipointer = mse::make_pointer_to_member_v2(h_msevec_ipointer, &H::m_string1);
            (*h_string1_msevec_ipointer) = "some new text";
    
            auto h_string1_msevec_ssiter = mse::make_pointer_to_member_v2(h_msevec_ssiter, &H::m_string1);
            (*h_string1_msevec_ssiter) = "some new text";
    
            auto h_string1_writelock_ptr = mse::make_pointer_to_member_v2(h_writelock_ptr, &H::m_string1);
            (*h_string1_writelock_ptr) = "some new text";
    
            auto h_string1_stdshared_const_ptr = mse::make_pointer_to_member_v2(h_stdshared_const_ptr, &H::m_string1);
            //(*h_string1_stdshared_const_ptr) = "some new text";
        }
    
        {
            /* Though the type of the safe pointer to the object member varies depending on how the object was
            declared, you can make a (templated) accessor function that will return a safe pointer of the
            appropriate type. */
            auto h_string1_scpptr = H::safe_pointer_to_member_string1(&h_scpobj);
            (*h_string1_scpptr) = "some new text";
    
            auto h_string1_refcptr = H::safe_pointer_to_member_string1(h_refcptr);
            (*h_string1_refcptr) = "some new text";
    
            auto h_string1_regptr = H::safe_pointer_to_member_string1(&h_regobj);
            (*h_string1_regptr) = "some new text";
    
            auto h_string1_rlxregptr = H::safe_pointer_to_member_string1(&h_rlxregobj);
            (*h_string1_rlxregptr) = "some new text";
    
            auto h_string1_mstdvec_iter = H::safe_pointer_to_member_string1(h_mstdvec_iter);
            (*h_string1_mstdvec_iter) = "some new text";
    
            auto h_string1_msevec_ipointer = H::safe_pointer_to_member_string1(h_msevec_ipointer);
            (*h_string1_msevec_ipointer) = "some new text";
    
            auto h_string1_msevec_ssiter = H::safe_pointer_to_member_string1(h_msevec_ssiter);
            (*h_string1_msevec_ssiter) = "some new text";
    
            auto h_string1_writelock_ptr = H::safe_pointer_to_member_string1(h_writelock_ptr);
            (*h_string1_writelock_ptr) = "some new text";
    
            auto h_string1_stdshared_const_ptr = H::safe_pointer_to_member_string1(h_stdshared_const_ptr);
            //(*h_string1_stdshared_const_ptr) = "some new text";
        }
    }
```

### Poly pointers
Poly pointers are "chameleon" (type-erased) pointers that can be constructed from, and retain the safety features of many of the pointer types in this library. If you're writing a function and you'd like it to be able to accept different types of safe pointer parameters, you can "templatize" your function. Alternatively, you can declare your pointer parameters as poly pointers.  

Note that poly pointers support only basic facilities common to all the covered pointer and iterator types, providing essentially the functionality of a C++ reference. For example, this means no assignment operator, and no `operator bool()`. Where null pointer values are desired you might consider using [`mse::mstd::optional<>`](#optional-xscope_optional) or `std::optional<>` instead.  

### TXScopePolyPointer, TXScopePolyConstPointer
Scope poly pointers are primarily intended to be used in function parameter declarations. In particular, as they can be constructed from a scope pointer (`TXScopeFixedPointer<>` or `TXScopeFixedConstPointer<>`), they must observe the same usage restrictions.

usage example:

```cpp
    #include "msepoly.h"
    
    void main() {
        class A {
        public:
            A() {}
            A(std::string x) : b(x) {}
            virtual ~A() {}

            std::string b = "some text ";
        };
        class D : public A {
        public:
            D(std::string x) : A(x) {}
        };
        class B {
        public:
            static std::string foo1(mse::TXScopePolyPointer<A> ptr) {
                std::string retval = ptr->b;
                return retval;
            }
            static std::string foo2(mse::TXScopePolyConstPointer<A> ptr) {
                std::string retval = ptr->b;
                return retval;
            }
            static std::string foo3(mse::TXScopePolyPointer<std::string> ptr) {
                std::string retval = (*ptr) + (*ptr);
                return retval;
            }
            static std::string foo4(mse::TXScopePolyConstPointer<std::string> ptr) {
                std::string retval = (*ptr) + (*ptr);
                return retval;
            }
        protected:
            ~B() {}
        };
    
        /* To demonstrate, first we'll declare some objects such that we can obtain safe pointers to those
        objects. For better or worse, this library provides a bunch of different safe pointers types. */
        mse::TXScopeObj<A> a_scpobj;
        auto a_refcptr = mse::make_refcounting<A>();
        mse::TRegisteredObj<A> a_regobj;
        mse::TCRegisteredObj<A> a_rlxregobj;
    
        /* Safe iterators are a type of safe pointer too. */
        mse::mstd::vector<A> a_mstdvec;
        a_mstdvec.resize(1);
        auto a_mstdvec_iter = a_mstdvec.begin();
        mse::us::msevector<A> a_msevec;
        a_msevec.resize(1);
        auto a_msevec_ipointer = a_msevec.ibegin();
        auto a_msevec_ssiter = a_msevec.ss_begin();
    
        /* And don't forget the safe async sharing pointers. */
        auto a_access_requester = mse::make_asyncsharedreadwrite<A>();
        auto a_writelock_ptr = a_access_requester.writelock_ptr();
        auto a_stdshared_const_ptr = mse::make_stdsharedimmutable<A>();
    
        /* And note that safe pointers to member elements need to be wrapped in an mse::TXScopeAnyPointer<> for
        mse::TXScopePolyPointer<> to accept them. */
        auto b_member_a_refc_anyptr = mse::TXScopeAnyPointer<std::string>(mse::make_pointer_to_member_v2(a_refcptr, &A::b));
        auto b_member_a_reg_anyptr = mse::TXScopeAnyPointer<std::string>(mse::make_pointer_to_member_v2(&a_regobj, &A::b));
        auto b_member_a_mstdvec_iter_anyptr = mse::TXScopeAnyPointer<std::string>(mse::make_pointer_to_member_v2(a_mstdvec_iter, &A::b));
    
        {
            /* All of these safe pointer types happily convert to an mse::TXScopePolyPointer<>. */
            auto res_using_scpptr = B::foo1(&a_scpobj);
            auto res_using_refcptr = B::foo1(a_refcptr);
            auto res_using_regptr = B::foo1(&a_regobj);
            auto res_using_rlxregptr = B::foo1(&a_rlxregobj);
            auto res_using_mstdvec_iter = B::foo1(a_mstdvec_iter);
            auto res_using_msevec_ipointer = B::foo1(a_msevec_ipointer);
            auto res_using_msevec_ssiter = B::foo1(a_msevec_ssiter);
            auto res_using_writelock_ptr = B::foo1(a_writelock_ptr);
            auto res_using_member_refc_anyptr = B::foo3(b_member_a_refc_anyptr);
            auto res_using_member_reg_anyptr = B::foo3(b_member_a_reg_anyptr);
            auto res_using_member_mstdvec_iter_anyptr = B::foo3(b_member_a_mstdvec_iter_anyptr);
    
            /* Or an mse::TXScopePolyConstPointer<>. */
            auto res_using_scpptr_via_const_poly = B::foo2(&a_scpobj);
            auto res_using_refcptr_via_const_poly = B::foo2(a_refcptr);
            auto res_using_regptr_via_const_poly = B::foo2(&a_regobj);
            auto res_using_rlxregptr_via_const_poly = B::foo2(&a_rlxregobj);
            auto res_using_mstdvec_iter_via_const_poly = B::foo2(a_mstdvec_iter);
            auto res_using_msevec_ipointer_via_const_poly = B::foo2(a_msevec_ipointer);
            auto res_using_msevec_ssiter_via_const_poly = B::foo2(a_msevec_ssiter);
            auto res_using_writelock_ptr_via_const_poly = B::foo2(a_writelock_ptr);
            auto res_using_stdshared_const_ptr_via_const_poly = B::foo2(a_stdshared_const_ptr);
            auto res_using_member_refc_anyptr_via_const_poly = B::foo4(b_member_a_refc_anyptr);
            auto res_using_member_reg_anyptr_via_const_poly = B::foo4(b_member_a_reg_anyptr);
            auto res_using_member_mstdvec_iter_anyptr_via_const_poly = B::foo4(b_member_a_mstdvec_iter_anyptr);
        }
    }
```

### TPolyPointer, TPolyConstPointer
These poly pointers do not support construction from scope pointers, and thus are not bound by the same usage restrictions. For example, these poly pointers may be used as a member of a class or struct.

### TXScopeAnyPointer, TXScopeAnyConstPointer, TAnyPointer, TAnyConstPointer
"Any" pointers are also “chameleon” (type-erased) pointers that behave similarly to poly pointers. One difference is that unlike poly pointers which can only be directly constructed from a finite set of pointer types, "any" pointers can be constructed from almost any kind of pointer. But poly pointers can be constructed from "any" pointers, so indirectly, via "any" pointers, pretty much any type of pointer converts to a poly pointer too. In particular, if you wanted to pass a pointer generated by [`make_pointer_to_member_v2()`](#make_pointer_to_member_v2) to a function that takes a poly pointer, you would first need to wrap it an "any" pointer. This is demonstrated in the [scope poly pointer](#txscopepolypointer-txscopepolyconstpointer) usage example.  

"Any" pointers can also be used as function arguments. The choice between using poly pointers versus "any" pointers is similar to the choice between [`std::variant` and `std::any`](http://www.boost.org/doc/libs/1_63_0/doc/html/variant/misc.html#variant.versus-any). 

### pointer_to()

`pointer_to(X)` simply returns `&X`, unless the type of `&X` is a native pointer (and the library's safe pointers have not been disabled). In that case a compiler error will be induced. It can be used in place of the `&` operator to help avoid inadvertent use of native pointers.

### Safely passing parameters by reference
A conformance-enforcment helper tool like scpptool [restricts](https://github.com/duneroadrunner/scpptool#restrictions-on-the-use-of-native-pointers-and-references) native reference parameters to (the limited) uses that it can verify are safe. You can ensure that an argument to a native reference parameter will be accepted as a safe by first obtaining a scope pointer to the argument and expressing the argument as a (direct) dereference of the scope pointer. Note that scope pointers can be obtained from [reference counting pointers](#reference-counting-pointers), [norad pointers](#norad-pointers), and pointers to [shared objects](#asynchronously-shared-objects) using [`make_xscope_strong_pointer_store()`](#make_xscope_strong_pointer_store), and [registered pointers](#registered-pointers) when they are pointing to scope objects (or scope pointers). And, if necessary, the `mse::us::unsafe_make_xscope_pointer_to()` function can be used to obtain a (potentially unsafe) scope pointer to any object.

Alternatively, instead of native references you can use scope pointers directly as reference parameters. 

In cases where the function takes (some degree of) "ownership" of the referenced parameter, and you want some flexibility in the (owning) pointer/reference parameter type, you probably want to make your function a function template. Another option would be to make your pointer parameters [poly pointers](#poly-pointers). 

### Safely returning references
While non-scope reference types can be safely returned just like value types, functions that return (or potentially return) scope pointer/reference types are a special case that must be handled differently. The challenge is to ensure that any returned scope pointer/reference doesn't outlive its original source, and therefore its target. (Btw, a "scope [section](#txscoperandomaccesssection-txscoperandomaccessconstsection-trandomaccesssection-trandomaccessconstsection)" would be an example of a scope reference type.) The general way to implement this case is demonstrated in the [`as_a_returnable_fparam()`](#as_a_returnable_fparam) section. It involves a significant amount of "ceremony", but perhaps that's not necessarily inappropriate, as it's kind of an insidously dangerous case in traditional C++. (Conformance-enforcment helper tools like [scpptool](https://github.com/duneroadrunner/scpptool) will catch attempts to return scope references unsafely.)

### Multithreading

The library requires and enforces that objects shared or passed between threads may only be of types identified as safe for such operations. 

### TAsyncPassableObj

When passing an argument to a function that will be executed in another thread using the library, the argument must be of a type identified as being safe to do so. If not, a compiler error will be induced. The library knows which of its own types and the standard types are and aren't safely passable to another thread, but can't automatically deduce whether or not a user-defined type is safe to pass. So in order to pass a user-defined type, you need to "declare" that it is safely passable by wrapping it with the transparent `rsv::TAsyncPassableObj<>` template. Otherwise you'll get a compile error. A type that is safe to pass should contain no indirect objects (i.e. pointers, references, etc.) that are not known to be safe to pass between threads. Using `rsv::TAsyncPassableObj<>` to indicate that a user-defined type is safely passable when that type does not meet these criteria could result in unsafe code, and wouldn't be prevented by the type system. Safety enforcement is reliant on a companion tool like [scpptool](https://github.com/duneroadrunner/scpptool).

### thread

`mstd::thread` is just an implementation of `std::thread` that verifies that the arguments passed are of a type that is designated (i.e. recognized or declared) as safe to pass between threads. Note that this includes the passed function object. "Regular, concrete" functions and non-capture lambdas are automatically recognized as safely passable, but functors, capture-lambdas and generic lambdas would need to be explicitly declared as "passable".

usage example: (used in the example for [TAsyncRASectionSplitter](#tasyncrasectionsplitter))

### async()

`mstd::async()` is just an implementation of `std::async()` that verifies that the arguments and return value passed are of a type that is designated as safe to pass between threads. 

usage example: ([see below](#async-aggregate-usage-example))

### Asynchronously shared objects
One situation where safety mechanisms are particularly important is when sharing objects between asynchronous threads. In particular, while one thread is modifying an object, you want to ensure that no other thread accesses it. But you also want to do it in a way that allows for maximum utilization of the shared object. To this end the library provides "access requesters". Access requesters provide "lock pointers" on demand that are used to safely access the shared object.

In cases where the object you want to share is "immutable" (i.e. not modifiable), no access control is necessary. For these cases the library provides "immutable fixed pointers", which can be thought of as sort of a safer version of `std::shared_ptr<>`.

In order to ensure safety, shared objects can only be accessed through lock pointers or immutable fixed pointers. If you have an existing object that you only want to share part of the time, you can swap (using `std::swap()` for example) the object with a shared object when it's time to share it, and swap it back when you're done sharing.

Note that not all types are safe to share between threads. For example, because of its iterators, `mstd::vector<int>` is not safe to share between threads. (And neither is `std::vector<int>`.) [`nii_vector<int>`](#nii_vector) on the other hand is. Trying to share the former using access requesters or immutable fixed pointers would result in a compile error.

### TAsyncShareableObj

As with [passing](#tasyncpassableobj) objects between threads, when using the library to share an object among threads, the object must be of a type identified as being safe to do so. If not, a compiler error will be induced. The library knows which of its own types and the standard types are and aren't safely shareable, but can't automatically deduce whether or not a user-defined type is safe to share. So in order to share a user-defined type, you need to "declare" that it is safely shareable by wrapping it with the transparent `rsv::TAsyncShareableObj<>` template.

A type that is safe to share should contain no indirect objects (i.e. pointers, references, etc.) that are not known to be safe to share among threads, and should not provide any facilities for obtaining such indirect objects. Note that this would disqualify, for example, container types that have a (non-static) `begin()` member function that returns an iterator (which is an indirect/reference object). (The library provides containers such as [`nii_array<>`](#nii_array) that are more appropriate for sharing among threads.)

Technically it would also disqualify pretty much any object with a functioning (non-static) `&` operator, but the (ubiquitous) default `&` operator returns a raw pointer, and for pragmatic reasons, we more precisely identify the *declaration* of a raw pointer object (that receives the returned raw pointer value) to be unsafe, rather than the mere presence of the (hopefully unused) `&` operator that potentially generates it. Objects with custom (non-static) `&` operators that return something other than a raw pointer (such as registered objects whose `&` operator yields a [registered pointer](#registered-pointers)), however, would be disqualified.

Types declared as shareable should also not have any `mutable` qualified members that are not protected by a thread-safety mechanism.

Using `rsv::TAsyncShareableObj<>` to indicate that a user-defined type is safely shareable when that type does not meet these criteria could result in unsafe code, and wouldn't be prevented by the type system. Safety enforcement is reliant on a companion tool like [scpptool](https://github.com/duneroadrunner/scpptool).

### TAsyncShareableAndPassableObj

Many objects that qualify as safely [shareable](#tasyncshareableobj) or [passable](#tasyncpassableobj) between threads qualify as both.

usage example: ([see below](#async-aggregate-usage-example))

### TAsyncSharedV2ReadWriteAccessRequester

Use the `mse::make_asyncsharedv2readwrite<>(...)` function (in a manner analogous to `std::make_shared<>(...)`) to create an object to be shared and obtain an associated "access requester", which is just an object that holds shared ownership of the shared object. Unlike `std::shared_ptr<>`, you cannot dereference an access requester directly. Instead you use the `writelock_ptr()` and `readlock_ptr()` member functions to obtain pointers to the shared object. Those functions will block until they can obtain the needed lock on the shared object. The obtained pointers will hold on to their lock for as long as they exist. Their locks are released when the pointers are destroyed. (Generally when they go out of scope).  

Acccess requesters can be copied and passed-by-value as a parameter (to another thread, generally).

Non-blocking `try_writelock_ptr()` and `try_readlock_ptr()` member functions are also available. As are the limited-blocking `try_writelock_ptr_for()`, `try_readlock_ptr_for()`, `try_writelock_ptr_until()` and `try_readlock_ptr_until()`.

Note that while a "write-lock" pointer will not simultaneously co-exist with any lock pointer to the same shared object in any other thread, it can co-exist with (read- and/or write-) lock pointers in the same thread. This means that lock pointers have ["upgrade lock"](http://www.boost.org/doc/libs/1_65_1/doc/html/thread/synchronization.html#thread.synchronization.mutex_concepts.upgrade_lockable) functionality. That is, for example, a thread that holds a read lock on a shared object (via read-lock pointer) can, at some later point, additionally obtain a write lock (via write-lock pointer) without surrendering the original read lock. It can then release the write lock (by allowing the write-lock pointer to go out of scope), again without surrendering the original read lock. Systems based on traditional "readers-writer" locks would require you to surrender the read lock before attempting to obtain a write lock, allowing another thread to potentially (and undesirably) obtain a write lock in between.

One caveat is that this introduces a new possible deadlock scenario where two threads hold read locks and both are blocked indefinitely waiting for write locks. The access requesters detect these situations, and will throw an exception (or whatever user-specified behavior) when they occur.

Just as `std::weak_ptr<>` is the "weak" counterpart of `std::shared_ptr<>`, `TAsyncSharedV2WeakReadWriteAccessRequester<>` is the weak counter part of `TAsyncSharedV2ReadWriteAccessRequester<>`. Its constructor takes a regular (strong) access requester (`TAsyncSharedV2ReadWriteAccessRequester<>`), and its `try_strong_access_requester()` member function returns an `optional` value containing the associated strong access requester, if available.

usage example: ([see below](#async-aggregate-usage-example))

### TAsyncSharedV2ReadOnlyAccessRequester
You can use the `mse::make_asyncsharedv2readonly<>()` function to create a shared object and obtain a "read only" access requester that has a `readlock_ptr()` member function, but not a `writelock_ptr()` member function. "Read only" access requesters can also be copy constructed from a regular [(read-write) access requester (`TAsyncSharedV2ReadWriteAccessRequester<>`)](#tasyncsharedv2readwriteaccessrequester).

usage example: ([see below](#async-aggregate-usage-example))

### TAsyncSharedV2ImmutableFixedPointer
In cases where the object you want to share is "immutable" (i.e. not modifiable), no access control is necessary. For these cases you can use `TAsyncSharedV2ImmutableFixedPointer<>`, which can be thought of as sort of a safer version of `std::shared_ptr<>`. Use the `mse::make_asyncsharedv2immutable<>()` function to obtain a `TAsyncSharedV2ImmutableFixedPointer<>`.

usage example: ([see below](#async-aggregate-usage-example))

### TAsyncSharedV2AtomicFixedPointer
Atomic objects also don't require access control. Use the `make_asyncsharedv2atomic<>()` function to obtain a `TAsyncSharedV2AtomicFixedPointer<>`.

usage example: ([see below](#async-aggregate-usage-example))

#### async aggregate usage example:

```cpp
#include "mseasyncshared.h"
#include <ctime>
#include <ratio>
#include <chrono>
#include <future>

class H {
public:
	template<class _TAsyncSharedReadWriteAccessRequester>
	static double foo7(_TAsyncSharedReadWriteAccessRequester A_ashar) {
		auto t1 = std::chrono::high_resolution_clock::now();
		/* A_ashar.readlock_ptr() will block until it can obtain a read lock. */
		auto ptr1 = A_ashar.readlock_ptr(); // while ptr1 exists it holds a (read) lock on the shared object
		auto t2 = std::chrono::high_resolution_clock::now();
		std::this_thread::sleep_for(std::chrono::seconds(1));
		auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
		auto timespan_in_seconds = time_span.count();
		auto thread_id = std::this_thread::get_id();
		return timespan_in_seconds;
	}
protected:
	~H() {}
};

void main(int argc, char* argv[]) {
	/* The TAsyncShared data types are used to safely share objects between asynchronous threads. */

	class A {
	public:
		A(int x) : b(x) {}
		virtual ~A() {}

		int b = 3;
		mse::nii_string s = "some text ";
	};
	/* User-defined classes need to be declared as (safely) shareable in order to be accepted by the access requesters. */
	typedef mse::rsv::TAsyncShareableAndPassableObj<A> ShareableA;

	class B {
	public:
		static double foo1(mse::TAsyncSharedV2ReadWriteAccessRequester<ShareableA> A_ashar) {
			auto t1 = std::chrono::high_resolution_clock::now();
			/* mse::TAsyncSharedV2ReadWriteAccessRequester<ShareableA>::writelock_ptr() will block until it can obtain a write lock. */
			auto ptr1 = A_ashar.writelock_ptr(); // while ptr1 exists it holds a (write) lock on the shared object
			auto t2 = std::chrono::high_resolution_clock::now();
			std::this_thread::sleep_for(std::chrono::seconds(1));
			auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
			auto timespan_in_seconds = time_span.count();
			auto thread_id = std::this_thread::get_id();
			//std::cout << "thread_id: " << thread_id << ", time to acquire write pointer: " << timespan_in_seconds << " seconds.";
			//std::cout << std::endl;

			ptr1->s = std::to_string(timespan_in_seconds);
			return timespan_in_seconds;
		}
		static int foo2(mse::TAsyncSharedV2ImmutableFixedPointer<ShareableA> A_immptr) {
			return A_immptr->b;
		}
		static int foo3(mse::TAsyncSharedV2AtomicFixedPointer<int> int_atomic_ptr) {
			(*int_atomic_ptr) += 1;
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			return *int_atomic_ptr;
		}
	protected:
		~B() {}
	};

	std::cout << std::endl;
	std::cout << "AsyncSharedV2 test output:";
	std::cout << std::endl;

	{
		/* This block contains a simple example demonstrating the use of mse::TAsyncSharedV2ReadWriteAccessRequester
		to safely share an object between threads. */

		std::cout << "TAsyncSharedV2ReadWriteAccessRequester:";
		std::cout << std::endl;
		auto ash_access_requester = mse::make_asyncsharedv2readwrite<ShareableA>(7);
		ash_access_requester.writelock_ptr()->b = 11;
		int res1 = ash_access_requester.readlock_ptr()->b;

		{
			auto ptr3 = ash_access_requester.readlock_ptr();
			auto ptr1 = ash_access_requester.writelock_ptr();
			auto ptr2 = ash_access_requester.writelock_ptr();
		}

		std::list<mse::mstd::future<double>> futures;
		for (size_t i = 0; i < 3; i += 1) {
			futures.emplace_back(mse::mstd::async(B::foo1, ash_access_requester));
		}
		int count = 1;
		for (auto it = futures.begin(); futures.end() != it; it++, count++) {
			std::cout << "thread: " << count << ", time to acquire read pointer: " << (*it).get() << " seconds.";
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
	{
		std::cout << "TAsyncSharedV2ReadOnlyAccessRequester:";
		std::cout << std::endl;
		auto ash_access_requester = mse::make_asyncsharedv2readonly<ShareableA>(7);
		int res1 = ash_access_requester.readlock_ptr()->b;

		std::list<mse::mstd::future<double>> futures;
		for (size_t i = 0; i < 3; i += 1) {
			futures.emplace_back(mse::mstd::async(J::foo7<mse::TAsyncSharedV2ReadOnlyAccessRequester<ShareableA>>, ash_access_requester));
		}
		int count = 1;
		for (auto it = futures.begin(); futures.end() != it; it++, count++) {
			std::cout << "thread: " << count << ", time to acquire read pointer: " << (*it).get() << " seconds.";
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
	{
		/* Just demonstrating the existence of the "try" versions. */
		auto access_requester = mse::make_asyncsharedv2readwrite<mse::nii_string>("some text");
		auto writelock_ptr1 = access_requester.try_writelock_ptr();
		if (writelock_ptr1) {
			// lock request succeeded
			int q = 5;
		}
		auto readlock_ptr2 = access_requester.try_readlock_ptr_for(std::chrono::seconds(1));
		auto writelock_ptr3 = access_requester.try_writelock_ptr_until(std::chrono::steady_clock::now() + std::chrono::seconds(1));
	}
	{
		/* TAsyncSharedV2WeakReadWriteAccessRequester<> is the weak counterpart to TAsyncSharedV2ReadWriteAccessRequester<>
		analogous to how std::weak_ptr<> is the weak counterpart to std::shared_ptr<>. */

		typedef decltype(mse::make_asyncsharedv2readwrite<mse::nii_string>("abc")) access_requester_t;
		auto vec1 = mse::mstd::vector<access_requester_t>();
		vec1.push_back(mse::make_asyncsharedv2readwrite<mse::nii_string>("abc"));

		mse::TAsyncSharedV2WeakReadWriteAccessRequester<mse::nii_string> weak_ar1(vec1.at(0));

		/* Here we're obtaining a (strong) access requester from the weak access requester, then appending it the
		vector of access requesters. */
		vec1.push_back(weak_ar1.try_strong_access_requester().value());

		assert((*(vec1.at(1).readlock_ptr())) == "abc");

		vec1.clear();

		/* All the (strong) access requesters have just been destroyed so attempting to obtain a (strong) access requester
		from our weak one will result in an empty optional being returned. */
		assert(!(weak_ar1.try_strong_access_requester().has_value()));
	}
	{
		/* For scenarios where the shared object is immutable (i.e. is never modified), you can get away without using locks
		or access requesters. */
		auto A_immptr = mse::make_asyncsharedv2immutable<ShareableA>(5);
		int res1 = A_immptr->b;
		std::shared_ptr<const ShareableA> A_shptr(A_immptr);

		std::list<mse::mstd::future<int>> futures;
		for (size_t i = 0; i < 3; i += 1) {
			futures.emplace_back(mse::mstd::async(B::foo2, A_immptr));
		}
		int count = 1;
		for (auto it = futures.begin(); futures.end() != it; it++, count++) {
			int res2 = (*it).get();
		}

		auto A_b_safe_cptr = mse::make_const_pointer_to_member(A_immptr->b, A_immptr);
	}
	{
		/* For scenarios where the shared object is atomic, you can get away without using locks
		or access requesters. */
		auto int_atomic_ptr = mse::make_asyncsharedv2atomic<int>(5);
		int res1 = (*int_atomic_ptr);

		std::list<mse::mstd::future<int>> futures;
		for (size_t i = 0; i < 3; i += 1) {
			futures.emplace_back(mse::mstd::async(B::foo3, int_atomic_ptr));
		}
		int count = 1;
		for (auto it = futures.begin(); futures.end() != it; it++, count++) {
			int res2 = (*it).get();
		}
	}
	{
		/* mse::TAsyncSharedV2ReadWriteAccessRequester's flexibilty in allowing coexisting read and write lock
		pointers in the same thread introduces new ways to produce logical deadlocks. This block (likely)
		demonstrates the access requester's ability to detect these potential deadlocks (and throw an exception
		when they would occur). */

		std::cout << "TAsyncSharedV2ReadWriteAccessRequester deadlock detection:";
		std::cout << std::endl;

		class CC {
		public:
			static void foo1(mse::TAsyncSharedV2ReadWriteAccessRequester<ShareableA> A_ashar, int id) {
				auto readlock_ptr = A_ashar.readlock_ptr();
				std::this_thread::sleep_for(std::chrono::seconds(1));
				try {
					auto writelock_ptr = A_ashar.writelock_ptr();
					std::this_thread::sleep_for(std::chrono::seconds(1));
				}
				catch (...) {
					// likely exception due to potential deadlock
					std::cout << "deadlock detected ";
					std::cout << std::endl;
				}
			}
		};

		auto ash_access_requester = mse::make_asyncsharedv2readwrite<ShareableA>(7);

		{
			auto thread1 = mse::mstd::thread(CC::foo1, ash_access_requester, 1);
			auto thread2 = mse::mstd::thread(CC::foo1, ash_access_requester, 2);
			thread1.join();
			thread2.join();
		}

		std::cout << std::endl;
	}
}
```

### TAsyncRASectionSplitter

`TAsyncRASectionSplitter<>` is used for situations where you want to allow multiple threads to concurrently access and/or modify different parts of an array or vector. You specify how you want the array/vector partitioned, and the `TAsyncRASectionSplitter<>` will provide a set of access requesters used to obtain access to each partition. Instead of the usual "lock pointers", these access requesters return "lock [random access section](#txscoperandomaccesssection-txscoperandomaccessconstsection-trandomaccesssection-trandomaccessconstsection)s".

usage example:
```cpp
#include "msemstdvector.h"
#include "mseasyncshared.h"

class J {
public:
	/* This function takes a "random access section" (which is like an "array_view" or gsl::span) as its parameter. */
	template<class _TStringRASection>
	static void foo8(_TStringRASection ra_section) {
		size_t delay_in_milliseconds = 3000/*arbitrary*/;
		if (1 <= ra_section.size()) {
			delay_in_milliseconds /= ra_section.size();
		}
		for (size_t i = 0; i < ra_section.size(); i += 1) {
			auto now1 = std::chrono::system_clock::now();
			auto tt = std::chrono::system_clock::to_time_t(now1);

			/* Just trying to obtain a string with the current time and date. The standard library doesn't yet
			seem to provide a safe, portable way to do this. */
#ifdef _MSC_VER
			static const size_t buffer_size = 64;
			char buffer[buffer_size];
			buffer[0] = '\0';
			ctime_s(buffer, buffer_size, &tt);
#else /*_MSC_VER*/
			auto buffer = ctime(&tt);
#endif /*_MSC_VER*/

			std::string now_str(buffer);
			ra_section[i] = now_str;

			std::this_thread::sleep_for(std::chrono::milliseconds(delay_in_milliseconds));
		}
	}

	/* This function just obtains a writelock_ra_section from the given "splitter access requester" and calls the given
	function with the writelock_ra_section as the first argument. */
	template<class _TAsyncSplitterRASectionReadWriteAccessRequester, class _TFunction, class... Args>
	static void invoke_with_writelock_ra_section1(_TAsyncSplitterRASectionReadWriteAccessRequester ar, _TFunction function1, Args&&... args) {
		function1(ar.writelock_ra_section(), args...);
	}
};

void main(int argc, char* argv[]) {
	/* This block demonstrates safely allowing different threads to (simultaneously) modify different
	sections of a vector. (We use vectors in this example, but it works just as well with arrays.) */

	static const size_t num_sections = 10;
	static const size_t section_size = 5;
	const size_t num_elements = num_sections * section_size;

	typedef mse::nii_vector<mse::nii_string> async_shareable_vector1_t;
	typedef mse::mstd::vector<mse::nii_string> nonshareable_vector1_t;
	/* Let's say we have a vector. */
	nonshareable_vector1_t vector1;
	vector1.resize(num_elements);
	{
		size_t count = 0;
		for (auto& item_ref : vector1) {
			count += 1;
			item_ref = "text" + std::to_string(count);
		}
	}

	/* Only access controlled objects can be shared with other threads, so we'll make an access controlled vector and
	(temporarily) swap it with our original one. */
	auto ash_access_requester = mse::make_asyncsharedv2readwrite<async_shareable_vector1_t>();
	std::swap(vector1, (*(ash_access_requester.writelock_ptr())));

	{
		/* Now, we're going to use the access requester to obtain two new access requesters that provide access to
		(newly created) "random access section" objects which are used to access (disjoint) sections of the vector.
		We need to specify the position where we want to split the vector. Here we specify that it be split at index
		"num_elements / 2", right down the middle. */
		mse::TAsyncRASectionSplitter<decltype(ash_access_requester)> ra_section_split1(ash_access_requester, num_elements / 2);
		auto ar1 = ra_rection_split1.first_ra_section_access_requester();
		auto ar2 = ra_rection_split1.second_ra_section_access_requester();

		/* The J::foo8 template function is just an example function that operates on containers of strings. In our case the
		containers will be the random access sections we just created. We'll create an instance of the function here. */
		auto my_foo8_function = J::foo8<decltype(ar1.writelock_ra_section())>;

		/* We want to execute the my_foo8 function in a separate thread. The function takes a "random access section"
		as an argument. But as we're not allowed to pass random access sections between threads, we must pass an
		access requester instead. The "J::invoke_with_writelock_ra_section1" template function is just a helper
		function that will obtain a (writelock) random access section from the access requester, then call the given
		function, in this case my_foo8, with that random access section. So here we'll use it to create a proxy
		function that we can execute directly in a separate thread and will accept an access requester as a
		parameter. */
		auto my_foo8_proxy_function = J::invoke_with_writelock_ra_section1<decltype(ar1), decltype(my_foo8_function)>;

		std::list<mse::mstd::thread> threads;
		/* So this thread will modify the first section of the vector. */
		threads.emplace_back(mse::mstd::thread(my_foo8_proxy_function, ar1, my_foo8_proxy_function));
		/* While this thread modifies the other section. */
		threads.emplace_back(mse::mstd::thread(my_foo8_proxy_function, ar2, my_foo8_proxy_function));

		{
			int count = 1;
			for (auto it = threads.begin(); threads.end() != it; it++, count++) {
				(*it).join();
			}
		}
	}
	{
		/* Ok, now let's do it again, but instead of splitting the vector into two sections, let's split it into more sections: */
		/* First we create a list of a the sizes of each section. We'll use a vector here, but any iteratable container will work. */
		mse::mstd::vector<size_t> section_sizes;
		for (size_t i = 0; i < num_sections; i += 1) {
			section_sizes.push_back(section_size);
		}

		/* Just as before, TAsyncRASectionSplitter<> will generate a new access requester for each section. */
		mse::TAsyncRASectionSplitter<decltype(ash_access_requester)> ra_section_split1(ash_access_requester, section_sizes);
		auto ar0 = ra_rection_split1.ra_section_access_requester(0);

		auto my_foo8_function = J::foo8<decltype(ar1.writelock_ra_section())>;
		auto my_foo8_proxy_function = J::invoke_with_writelock_ra_section1<decltype(ar1), decltype(my_foo8_function)>;

		std::list<mse::mstd::thread> threads;
		for (size_t i = 0; i < num_sections; i += 1) {
			auto ar = ra_rection_split1.ra_section_access_requester(i);
			threads.emplace_back(mse::mstd::thread(my_foo8_proxy_function, ar, my_foo8_proxy_function));
		}

		{
			int count = 1;
			for (auto it = threads.begin(); threads.end() != it; it++, count++) {
				(*it).join();
			}
		}
	}

	/* Now that we're done sharing the (controlled access) vector, we can swap it back to our original vector. */
	std::swap(vector1, (*(ash_access_requester.writelock_ptr())));
	auto first_element_value = vector1[0];
	auto last_element_value = vector1.back();
}
```

### Scope threads

`xscope_thread` is the scope counterpart to [`mstd::thread`](#thread). `xscope_thread` ensures that the actual associated thread doesn't outlive it (and therefore doesn't outlive the scope), blocking in its destructor if necessary. Note that objects shared with an `mstd::thread` generally have dynamic allocation (i.e. are allocated on the heap), whereas objects shared with a scope thread can themselves be scope objects (i.e. allocated on the stack). Which would generally be the primary reason for using scope threads over non-scope threads. 

Any data type that qualifies as "[shareable](#tasyncshareableobj)" (or "[passable](#tasyncpassableobj)") with non-scope threads also qualifies as shareable (or passable) with scope threads. (But not necessarily the other way around.) 

#### access controlled objects

But in order to share an existing scope object, that object also has to be an "access controlled" object. You make a type "access controlled" by wrapping it with the `mse::TXScopeAccessControlledObj<>` template wrapper. 

`mse::TXScopeAccessControlledObj<>` provides `xscope_pointer()`, `xscope_const_pointer()` and `xscope_exclusive_pointer()` member functions which you use to obtain (scope) pointers to the contained object. Note that a pointer obtained via `xscope_exclusive_pointer()` may not coexist with any other pointer to the same object. Attempting to violate this rule will result in an exception. 

#### xscope_thread_carrier

Ok, so getting back to scope threads, it would generally not be very common that you would use `xscope_thread`s directly. More often you would use them indirectly via an `xscope_thread_carrier`, which is just a simple container for creating and managing a set of `xscope_thread`s.

#### xscope_future_carrier

Like `xscope_thread`, `xscope_future` and `xscope_async()` are the scope versions of their non-scope counterparts. And similarly, rather than using them directly you would more often use them via an `xscope_future_carrier`, which is just a simple container for creating and managing a set of `xscope_future`s and their associated `xscope_async()` functions.

#### make_xscope_asyncsharedv2acoreadwrite()

And finally, the function used to obtain a (scope) [access requester](#tasyncsharedv2readwriteaccessrequester) to an access controlled scope object is `make_xscope_asyncsharedv2acoreadwrite()`. Note that it takes as its argument a scope pointer to the access controlled object, not a scope pointer to the contained object. Btw, scope access requesters are an example of an object type that can be passed to other scope threads, but does not qualify (i.e. would induce a compile error) to be passed to non-scope threads. 

usage example:
```cpp
#include "mseasyncshared.h"
#include "msescope.h"
#include "msemsestring.h"
#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>
#include <list>

class J {
public:
	template<class _TAsyncSharedReadWriteAccessRequester>
	static double foo7(_TAsyncSharedReadWriteAccessRequester A_ashar) {
		auto t1 = std::chrono::high_resolution_clock::now();
		/* A_ashar.readlock_ptr() will block until it can obtain a read lock. */
		auto ptr1 = A_ashar.readlock_ptr(); // while ptr1 exists it holds a (read) lock on the shared object
		auto t2 = std::chrono::high_resolution_clock::now();
		std::this_thread::sleep_for(std::chrono::seconds(1));
		auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
		auto timespan_in_seconds = time_span.count();
		auto thread_id = std::this_thread::get_id();
		//std::cout << "thread_id: " << thread_id << ", time to acquire read pointer: " << timespan_in_seconds << " seconds.";
		//std::cout << std::endl;
		return timespan_in_seconds;
	}
};

void main(int argc, char* argv[]) {

	/* Here we demonstrate safely sharing an existing stack allocated object among threads. */

	class A {
	public:
		A(int x) : b(x) {}
		virtual ~A() {}

		int b = 3;
		mse::nii_string s = "some text ";
	};
	/* User-defined classes need to be declared as (safely) shareable in order to be accepted by the access requesters. */
	typedef mse::rsv::TAsyncShareableAndPassableObj<A> ShareableA;

	std::cout << ": xscope_future_carrier<>";
	std::cout << std::endl;

	/* (Mutable) objects can be shared between threads only if they are "access controlled". You can make an
	object "access controlled" by wrapping its type with the mse::TXScopeAccessControlledObj<> template wrapper. */
	mse::TXScopeObj<mse::TXScopeAccessControlledObj<ShareableA> > a_xscpacobj(7);

	/* Here we obtain a scope access requester for the access controlled object. */
	auto xscope_access_requester = mse::make_xscope_asyncsharedv2acoreadwrite(&a_xscpacobj);

	/* xscope_future_carrier<> is just a container that holds and manages scope futures. */
	mse::xscope_future_carrier<double> xscope_futures;

	std::list<mse::xscope_future_carrier<double>::handle_t> future_handles;
	for (size_t i = 0; i < 3; i += 1) {
		/* You add a future by specifying the async() function and parameters that will return the future value. */
		auto handle = xscope_futures.new_future(J::foo7<decltype(xscope_access_requester)>, xscope_access_requester);

		/* You need to store the handle of the added future in order to later retrieve its value. */
		future_handles.emplace_back(handle);
	}
	int count = 1;
	for (auto it = future_handles.begin(); future_handles.end() != it; it++, count++) {
		std::cout << "thread: " << count << ", time to acquire read pointer: " << xscope_futures.xscope_ptr_at(*it)->get() << " seconds.";
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
```

#### make_xscope_aco_locker_for_sharing()

The `mse::make_xscope_aco_locker_for_sharing()` function takes a scope pointer to an "[access controlled object](#access-controlled-objects)" and returns a "locker" object which holds an exclusive reference to the given access controlled object. From this locker object, you can obtain either one "scope passable" (non-const) pointer, or any number of "scope passable" const pointers. These scope passable pointers can then be safely passed directly as arguments to scope threads. This is a (little) more cumbersome, more restrictive way of sharing an object than, say, using the library's "[access requesters](#make_xscope_asyncsharedv2acoreadwrite)". But you might choose to do it this way in certain cases where performance is critical. When using access requesters, each thread obtains the desired lock on a thread-safe mutex. When using `mse::make_xscope_aco_locker_for_sharing()`, the lock is obtained before launching the thread(s), so the mutex does not need to be thread-safe, thus saving a little overhead.

#### make_xscope_exclusive_strong_pointer_store_for_sharing()

The `mse::make_xscope_exclusive_strong_pointer_store_for_sharing()` function returns the same kind of "locker" object that `mse::make_xscope_aco_locker_for_sharing()` does, but instead of taking a scope pointer to an "access controlled object", it accepts any recognized "exclusive" pointer. That is, a pointer that, while it exists, holds exclusive access to its target object.

usage example:
```cpp
#include "mseasyncshared.h"
#include "msescope.h"
#include "msemsestring.h"
#include <iostream>
#include <ratio>
#include <chrono>

class J {
public:
    template<class _TAPointer>
    static void foo17b(_TAPointer a_ptr) {
        static int s_count = 0;
        s_count += 1;
        a_ptr->s = std::to_string(s_count);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    template<class _TConstPointer, class _TPointer>
    static void foo18(_TConstPointer src_ptr, _TPointer dst_ptr) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        dst_ptr->s = src_ptr->s;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
};

void main(int argc, char* argv[]) {

    class A {
    public:
        A(int x) : b(x) {}
        virtual ~A() {}

        int b = 3;
        mse::nii_string s = "some text ";
    };
    typedef mse::rsv::TAsyncShareableAndPassableObj<A> ShareableA;

    mse::TXScopeObj<mse::TXScopeAccessControlledObj<ShareableA> > a_xscpacobj1(3);
    mse::TXScopeObj<mse::TXScopeAccessControlledObj<ShareableA> > a_xscpacobj2(5);
    mse::TXScopeObj<mse::TXScopeAccessControlledObj<ShareableA> > a_xscpacobj3(7);

    {
        auto xscope_aco_locker1 = mse::make_xscope_aco_locker_for_sharing(&a_xscpacobj1);

        typedef decltype(xscope_aco_locker1.xscope_passable_pointer()) passable_exclusive_pointer_t;
        mse::xscope_thread xscp_thread1(J::foo17b<passable_exclusive_pointer_t>, xscope_aco_locker1.xscope_passable_pointer());
    }
    {
        auto xscope_aco_locker1 = mse::make_xscope_aco_locker_for_sharing(&a_xscpacobj1);
        auto xscope_aco_locker2 = mse::make_xscope_aco_locker_for_sharing(&a_xscpacobj2);
        auto xscope_aco_locker3 = mse::make_xscope_aco_locker_for_sharing(&a_xscpacobj3);

        typedef decltype(xscope_aco_locker1.xscope_passable_const_pointer()) passable_const_pointer_t;
        typedef decltype(xscope_aco_locker2.xscope_passable_pointer()) passable_exclusive_pointer_t;

        mse::xscope_thread xscp_thread1(J::foo18<passable_const_pointer_t, passable_exclusive_pointer_t>
            , xscope_aco_locker1.xscope_passable_const_pointer()
            , xscope_aco_locker2.xscope_passable_pointer());

        mse::xscope_thread xscp_thread2(J::foo18<passable_const_pointer_t, passable_exclusive_pointer_t>
            , xscope_aco_locker1.xscope_passable_const_pointer()
            , xscope_aco_locker3.xscope_passable_pointer());
    }
    {
        auto xscope_aco_locker1 = mse::make_xscope_aco_locker_for_sharing(&a_xscpacobj1);

        /* Here we're using a (non-const) "xscope_passable_pointer" as the argument. The "const" version
        wouldn't be accepted because an "xscope_passable_const_pointer" is not an exclusive pointer. That is, 
        it doesn't hold exclusive access to its target object. We could, for example, have instead used an 
        exclusive pointer obtained directly from the "access controlled" object, a_xscpacobj1. */

        auto xscope_xstrong_ptr_store1 = mse::make_xscope_exclusive_strong_pointer_store_for_sharing(xscope_aco_locker1.xscope_passable_pointer());

        auto xscope_aco_locker2 = mse::make_xscope_aco_locker_for_sharing(&a_xscpacobj2);
        auto xscope_aco_locker3 = mse::make_xscope_aco_locker_for_sharing(&a_xscpacobj3);

        typedef decltype(xscope_aco_locker1.xscope_passable_const_pointer()) passable_const_pointer_t;
        typedef decltype(xscope_aco_locker2.xscope_passable_pointer()) passable_exclusive_pointer_t;

        mse::xscope_thread xscp_thread1(J::foo18<passable_const_pointer_t, passable_exclusive_pointer_t>
            , xscope_xstrong_ptr_store1.xscope_passable_const_pointer()
            , xscope_aco_locker2.xscope_passable_pointer());

        mse::xscope_thread xscp_thread2(J::foo18<passable_const_pointer_t, passable_exclusive_pointer_t>
            , xscope_xstrong_ptr_store1.xscope_passable_const_pointer()
            , xscope_aco_locker3.xscope_passable_pointer());
    }
}
```

#### TXScopeExclusiveStrongPointerStoreForAccessControl

You can use [`make_xscope_exclusive_strong_pointer_store_for_sharing()`](#make_xscope_exclusive_strong_pointer_store_for_sharing) to obtain, from an exclusive pointer of, for example, an [access controlled object](#access-controlled-objects), pointers that can be passed to other threads. Occassionally, you may want to do the reverse. That is, obtain access controlled object pointers from an exclusive pointer that was passed to a thread. You can do this by declaring the parameter that receives the passed pointer as a `TXScopeExclusiveStrongPointerStoreForAccessControl<passable_exclusive_pointer_t>`, replacing `passable_exclusive_pointer_t` with the type of the passed pointer. From this parameter object you can obtain pointers in the same manner as with regular access controlled objects.

usage example:
```cpp
#include "mseasyncshared.h"
#include "msescope.h"
#include "msemsestring.h"
#include <iostream>

void main(int argc, char* argv[]) {

    class A {
    public:
        A(int x) : b(x) {}
        virtual ~A() {}

        int b = 3;
        mse::nii_string s = "some text ";
    };
    typedef mse::rsv::TAsyncShareableAndPassableObj<A> ShareableA;

    mse::TXScopeObj<mse::TXScopeAccessControlledObj<ShareableA> > a_xscpacobj1(3);

    {
        /* In this block we demonstrate obtaining various types of (const and non-const) pointers you might need from
        an exclusive pointer that might be passed to a thread. */

        a_xscpacobj1.pointer()->s = "";

        auto xscope_aco_locker1 = mse::make_xscope_aco_locker_for_sharing(&a_xscpacobj1);

        typedef decltype(xscope_aco_locker1.xscope_passable_pointer()) passable_exclusive_pointer_t;
        typedef decltype(xscope_aco_locker1.xscope_passable_const_pointer()) passable_const_pointer_t;

        class CD {
        public:
            static void foo1(mse::TXScopeExclusiveStrongPointerStoreForAccessControl<passable_exclusive_pointer_t> xscope_store, int count) {
                {
                    auto xsptr = xscope_store.xscope_pointer();
                    xsptr->s.append(std::to_string(count));
                }
                {
                    /* Here, from the exclusive (non-const) pointer passed to this function, we're going to obtain a couple
                    of const pointers that we can pass to different (scope) threads. */
                    auto xscope_xstrong_ptr_store1 = mse::make_xscope_exclusive_strong_pointer_store_for_sharing(xscope_store.xscope_exclusive_pointer());

                    mse::xscope_thread xscp_thread1(CD::foo2, xscope_xstrong_ptr_store1.xscope_passable_const_pointer());
                    mse::xscope_thread xscp_thread2(CD::foo2, xscope_xstrong_ptr_store1.xscope_passable_const_pointer());
                }
                if (1 <= count) {
                    /* And here we're going to (re)obtain an exclusive strong pointer like the one that was passed to this
                    function, then we're going to use it to recursively call this function again in another (scope) thread. */
                    auto xscope_xstrong_ptr_store1 = mse::make_xscope_exclusive_strong_pointer_store_for_sharing(xscope_store.xscope_exclusive_pointer());
                    mse::xscope_thread xscp_thread1(CD::foo1, xscope_xstrong_ptr_store1.xscope_passable_pointer(), count - 1);
                }
            }
            static void foo2(passable_const_pointer_t xscope_A_cptr) {
                std::cout << xscope_A_cptr->s << std::endl;
            }
        };

        mse::xscope_thread xscp_thread1(CD::foo1, xscope_aco_locker1.xscope_passable_pointer(), 3);
    }
}
```

#### exclusive writer objects

"Exclusive writer objects" are a specialization of [access controlled objects](#access-controlled-objects) for which all non-const pointers are exclusive. That is, when a non-const pointer of an exclusive writer object exists, no other pointer of that object may exist.

A bit of extra functionality that exclusive writer objects have over access controlled objects is that, from a const pointer of an exclusive writer object, you can obtain a const pointer that can be passed to other threads (using the `make_xscope_exclusive_write_obj_const_pointer_store_for_sharing()` function).

`TXScopeExclusiveStrongPointerStoreForExclusiveWriterAccess<>` is the corresponding specialization of [`TXScopeExclusiveStrongPointerStoreForAccessControl<>`](#txscopeexclusivestrongpointerstoreforaccesscontrol).

usage example:
```cpp
#include "mseasyncshared.h"
#include "msescope.h"
#include "msemsestring.h"
#include <iostream>
#include <ratio>
#include <chrono>

class J {
public:
    template<class _TAPointer>
    static void foo17b(_TAPointer a_ptr) {
        static int s_count = 0;
        s_count += 1;
        a_ptr->s = std::to_string(s_count);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    template<class _TConstPointer, class _TPointer>
    static void foo18(_TConstPointer src_ptr, _TPointer dst_ptr) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        dst_ptr->s = src_ptr->s;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
};

void main(int argc, char* argv[]) {

    class A {
    public:
        A(int x) : b(x) {}
        virtual ~A() {}

        int b = 3;
        mse::nii_string s = "some text ";
    };
    typedef mse::rsv::TAsyncShareableAndPassableObj<A> ShareableA;

    mse::TXScopeObj<mse::TExclusiveWriterObj<ShareableA> > a_xscpxwobj1(3);
    mse::TXScopeObj<mse::TExclusiveWriterObj<ShareableA> > a_xscpxwobj2(5);
    mse::TXScopeObj<mse::TExclusiveWriterObj<ShareableA> > a_xscpxwobj3(7);

    {
        /* A (non-const) pointer of an "exclusive writer object" qualifies as an "exclusive strong" pointer, and
        thus you can obtain an xscope shareable pointer from it in the standard way. */
        auto xscope_xwo_pointer_store1 = mse::make_xscope_exclusive_strong_pointer_store_for_sharing(a_xscpxwobj1.pointer());

        typedef decltype(xscope_xwo_pointer_store1.xscope_passable_pointer()) passable_exclusive_pointer_t;
        mse::xscope_thread xscp_thread1(J::foo17b<passable_exclusive_pointer_t>, xscope_xwo_pointer_store1.xscope_passable_pointer());
    }
    {
        /* But uniquely, you can obtain an xscope shareable const pointer from a (non-exclusive) const pointer of an
        "exclusive writer object". There is a special function for this purpose: */
        auto xscope_xwo_const_pointer_store1 = mse::make_xscope_exclusive_write_obj_const_pointer_store_for_sharing(a_xscpxwobj1.const_pointer());

        auto xscope_xwo_pointer_store2 = mse::make_xscope_exclusive_strong_pointer_store_for_sharing(a_xscpxwobj2.pointer());
        auto xscope_xwo_pointer_store3 = mse::make_xscope_exclusive_strong_pointer_store_for_sharing(a_xscpxwobj3.pointer());

        typedef decltype(xscope_xwo_const_pointer_store1.xscope_passable_const_pointer()) passable_const_pointer_t;
        typedef decltype(xscope_xwo_pointer_store2.xscope_passable_pointer()) passable_exclusive_pointer_t;

        mse::xscope_thread xscp_thread1(J::foo18<passable_const_pointer_t, passable_exclusive_pointer_t>
            , xscope_xwo_const_pointer_store1.xscope_passable_const_pointer()
            , xscope_xwo_pointer_store2.xscope_passable_pointer());

        mse::xscope_thread xscp_thread2(J::foo18<passable_const_pointer_t, passable_exclusive_pointer_t>
            , xscope_xwo_const_pointer_store1.xscope_passable_const_pointer()
            , xscope_xwo_pointer_store3.xscope_passable_pointer());
    }
}
```

#### scope atomics

Atomic objects don't require access control.

usage example:

```cpp
#include "msescopeatomic.h"
#include "mseasyncshared.h"
#include <ratio>
#include <chrono>


void main(int argc, char* argv[]) {

    /* trivially copyable class */
    class D {
    public:
        D(int x) : b(x) {}

        int b = 3;
    };
    /* User-defined classes need to be declared as (safely) shareable in order to be used with the atomic templates. */
    typedef mse::rsv::TAsyncShareableAndPassableObj<D> ShareableD;

    class B {
    public:
        static int foo4(mse::TXScopeAtomicFixedPointer<ShareableD> xs_D_atomic_ptr) {
            auto d = (*xs_D_atomic_ptr).load();
            d.b += 1;
            (*xs_D_atomic_ptr).store(d);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            return (*xs_D_atomic_ptr).load().b;
        }
    };

    {
        /* For scenarios where the shared object is atomic, you can get away without using locks
        or access requesters. */

        mse::TXScopeAtomicObj<ShareableD> xscope_D_atomic_obj(7);
        int res1 = xscope_D_atomic_obj.load().b;

        auto xscope_D_atomic_ptr = &xscope_D_atomic_obj;

        mse::xscope_thread xscp_thread1(B::foo4, xscope_D_atomic_ptr);
        mse::xscope_thread xscp_thread2(B::foo4, xscope_D_atomic_ptr);

        int res2 = (*xscope_D_atomic_ptr).load().b;
    }
}
```

#### TXScopeACORASectionSplitter and TXScopeAsyncACORASectionSplitter

`TXScopeAsyncACORASectionSplitter<>` is the scope version of [`TAsyncRASectionSplitter<>`](#tasyncrasectionsplitter), which enables multiple threads to safely access disjoint sections of an array or vector simultaneously. Instead of passing an access requester to its constructor, it takes a scope pointer to an existing [access controlled](#access-controlled-objects) array or vector. 

Where `TXScopeAsyncACORASectionSplitter<>` provides an [access requester](#make_xscope_asyncsharedv2acoreadwrite) for each section, `TXScopeACORASectionSplitter<>` provides each section as an [access controlled object](#access-controlled-objects). From each of these access controlled sections, as with all access controlled objects, you can obtain pointers to them (using [`make_xscope_aco_locker_for_sharing()`](#make_xscope_aco_locker_for_sharing), for example) that can be safely passed to other threads.

One reason you might choose to use `TXScopeACORASectionSplitter<>` over `TXScopeAsyncACORASectionSplitter<>` is that it does not involve any (costly) thread safe locks. The trade-off being that it doesn't support dynamically locking and unlocking sections (to maximize the availability of each section).

usage example:

```cpp
#include "msemstdvector.h"
#include "mseasyncshared.h"
#include "msescope.h"
#include "msemsearray.h"

class J {
public:
	/* This function takes a "random access section" (which is like an "array_view" or gsl::span) as its parameter. */
	template<class _TStringRASection>
	static void foo8(_TStringRASection ra_section) {
		size_t delay_in_milliseconds = 3000/*arbitrary*/;
		if (1 <= ra_section.size()) {
			delay_in_milliseconds /= mse::as_a_size_t(ra_section.size());
		}
		for (size_t i = 0; i < ra_section.size(); i += 1) {
			auto now1 = std::chrono::system_clock::now();
			auto tt = std::chrono::system_clock::to_time_t(now1);

			/* Just trying to obtain a string with the current time and date. The standard library doesn't yet
			seem to provide a safe, portable way to do this. */
#ifdef _MSC_VER
			static const size_t buffer_size = 64;
			char buffer[buffer_size];
			buffer[0] = '\0';
			ctime_s(buffer, buffer_size, &tt);
#else /*_MSC_VER*/
			auto buffer = ctime(&tt);
#endif /*_MSC_VER*/

			std::string now_str(buffer);
			ra_section[i] = now_str;

			std::this_thread::sleep_for(std::chrono::milliseconds(delay_in_milliseconds));
		}
	}

	/* This function just obtains a writelock_ra_section from the given "splitter access requester" and calls the given
	function with the writelock_ra_section as the first argument. */
	template<class TAsyncSplitterRASectionReadWriteAccessRequester, class TFunction, class... Args>
	static void invoke_with_writelock_ra_section1(TAsyncSplitterRASectionReadWriteAccessRequester ar, TFunction function1, Args&&... args) {
		function1(ar.writelock_ra_section(), args...);
	}

	/* This function just obtains an xscope_random_access_section from the given access controlled pointer and calls the given
	function with the xscope_random_access_section as the first argument. */
	template<class Ty, class TFunction, class... Args>
	static void invoke_with_ra_section(mse::TXScopeExclusiveStrongPointerStoreForAccessControl<Ty> xs_ac_store, TFunction function1, Args&&... args) {
		auto xscope_ra_section = mse::make_xscope_random_access_section(xs_ac_store.xscope_pointer());
		function1(xscope_ra_section, args...);
	}
};

void main(int argc, char* argv[]) {

	/* This block demonstrates safely allowing different threads to (simultaneously)
	modify different sections of an existing vector declared as a local variable. */

	static const size_t num_sections = 10;
	static const size_t section_size = 5;
	const size_t num_elements = num_sections * section_size;

	typedef mse::nii_vector<mse::nii_string> async_shareable_vector1_t;
	typedef mse::mstd::vector<mse::nii_string> nonshareable_vector1_t;
	/* Let's say we have a vector. */
	nonshareable_vector1_t vector1;
	vector1.resize(num_elements);
	{
		size_t count = 0;
		for (auto& item_ref : vector1) {
			count += 1;
			item_ref = "text" + std::to_string(count);
		}
	}

	/* Only access controlled objects can be shared with other threads, so we'll make an access controlled vector and
	(temporarily) swap it with our original one. */
	mse::TXScopeObj<mse::TXScopeAccessControlledObj<async_shareable_vector1_t> > xscope_acobj;
	std::swap(vector1, *(xscope_acobj.xscope_pointer()));

	std::cout << "mse::TXScopeACORASectionSplitter<>: " << std::endl;

	{
		/* From the access controlled vector we're going to obtain scope pointers to two access controlled
		"random access section" objects which are used to access (disjoint) sections of the vector. Then we're
		going to use "locker" objects to obtain pointers that are designated as safe to pass to other threads.
		We need to specify the position  where we want to split the vector. Here we specify that it be split
		at index "num_elements / 2", right down the middle. */
		mse::TXScopeACORASectionSplitter<async_shareable_vector1_t> xscope_ra_section_split2(&xscope_acobj, num_elements / 2);

		/* Here we obtain scope pointers to the access controlled sections. */
		auto first_ra_section_aco_xsptr = xscope_ra_section_split2.xscope_ptr_to_first_ra_section_aco();
		auto second_ra_section_aco_xsptr = xscope_ra_section_split2.xscope_ptr_to_second_ra_section_aco();

		/* Access controlled object pointers aren't themselves passable to other threads, but we can obtain
		corresponding pointers that are passable via a "locker" object that takes exclusive control over
		the access controlled object. */
#ifndef _MSC_VER
		auto xscope_section_access_owner1 = mse::make_xscope_aco_locker_for_sharing(first_ra_section_aco_xsptr);
		auto xscope_section_access_owner2 = mse::make_xscope_aco_locker_for_sharing(second_ra_section_aco_xsptr);
#else // !_MSC_VER
		/* An apparent bug in msvc2017 and msvc2019preview (March 2019) prevents the other branch from compiling.
		This presents an opportunity to demonstrate an alternative solution. Instead of a "locker" object that
		takes a scope pointer to the access controlled object, we can use an "exclusive strong pointer store"
		which takes any recognized exclusive pointer. */
		auto xscope_section_access_owner1 = mse::make_xscope_exclusive_strong_pointer_store_for_sharing(
			(*first_ra_section_aco_xsptr).xscope_exclusive_pointer());
		auto xscope_section_access_owner2 = mse::make_xscope_exclusive_strong_pointer_store_for_sharing(
			(*second_ra_section_aco_xsptr).xscope_exclusive_pointer());
#endif // !_MSC_VER

		/* The J::foo8 template function is just an example function that operates on containers of strings. In this case the
		containers will be the random access sections we just created. We'll create an instance of the function here. */
		auto my_foo8_function = J::foo8<mse::TXScopeAnyRandomAccessSection<mse::nii_string> >;
		typedef decltype(my_foo8_function) my_foo8_function_type;

		mse::xscope_thread_carrier threads;
		/* So this thread will modify the first section of the vector. */
		threads.new_thread(J::invoke_with_ra_section<decltype(xscope_section_access_owner1.xscope_passable_pointer()), my_foo8_function_type>
			, xscope_section_access_owner1.xscope_passable_pointer(), my_foo8_function);
		/* While this thread modifies the other section. */
		threads.new_thread(J::invoke_with_ra_section<decltype(xscope_section_access_owner2.xscope_passable_pointer()), my_foo8_function_type>
			, xscope_section_access_owner2.xscope_passable_pointer(), my_foo8_function);

		/* Note that in this particular scenario we didn't need to use any access requesters (or (thread safe) locks). */
	}

	std::cout << "mse::TXScopeAsyncACORASectionSplitter<>: " << std::endl;

	{
		/* Ok, now let's do it again, but instead of splitting the vector into two sections, let's split it into more sections: */
		/* First we create a list of a the sizes of each section. We'll use a vector here, but any iteratable container will work. */
		mse::mstd::vector<size_t> section_sizes;
		for (size_t i = 0; i < num_sections; i += 1) {
			section_sizes.push_back(section_size);
		}

		/* This time (for demonstration purposes) we'll use TXScopeAsyncACORASectionSplitter<> to generate a new
		access requester for each section. */
		mse::TXScopeAsyncACORASectionSplitter<async_shareable_vector1_t> xscope_ra_section_split1(&xscope_acobj, section_sizes);
		auto ar0 = xscope_ra_section_split1.ra_section_access_requester(0);

		auto my_foo8_function = J::foo8<decltype(ar0.writelock_ra_section())>;
		typedef decltype(my_foo8_function) my_foo8_function_type;
		auto my_foo8_proxy_function = J::invoke_with_writelock_ra_section1<decltype(ar0), my_foo8_function_type>;

		{
			mse::xscope_thread_carrier xscope_threads;
			for (size_t i = 0; i < num_sections; i += 1) {
				auto ar = xscope_ra_section_split1.ra_section_access_requester(i);
				xscope_threads.new_thread(my_foo8_proxy_function, ar, my_foo8_function);
			}
			/* The scope will not end until all the scope threads have finished executing. */
		}
	}

	/* Now that we're done sharing the (controlled access) vector, we can swap it back to our original vector. */
	std::swap(vector1, *(xscope_acobj.xscope_pointer()));
	auto first_element_value = vector1[0];
	auto last_element_value = vector1.back();
}
```

### static and global variables

[*provisional*]

While not encouraging the use of `static` or (non-[`thread_local`](#thread_local)) global variables, the library does provide some facilities for their use. Note that because `static` and non-`thread_local` global variables can be accessible from multiple threads, their type must be one that is [recognized or declared](#tasyncshareableobj) as safely shareable.

#### static immutables

usage example:
```cpp
#include "msescope.h"
#include "msemsestring.h"
#include "msestaticimmutable.h"
#include <iostream>

MSE_RSV_DECLARE_GLOBAL_IMMUTABLE(mse::nii_string) gimm_string1 = "some text";

void main(int argc, char* argv[]) {
    {
        auto gimm_ptr1 = &gimm_string1;
        auto xs_gimm_store1 = mse::make_xscope_strong_pointer_store(gimm_ptr1);
        auto xs_ptr1 = xs_gimm_store1.xscope_ptr();
        std::cout << *xs_ptr1 << std::endl;

        MSE_DECLARE_STATIC_IMMUTABLE(mse::nii_string) simm_string2 = "abc";
        auto simm_ptr2 = &simm_string2;
        auto xs_simm_store2 = mse::make_xscope_strong_pointer_store(simm_ptr2);
        auto xs_ptr2 = xs_simm_store2.xscope_ptr();
        std::cout << *xs_ptr2 << std::endl;

        class CA {
        public:
            auto foo1() const {
                MSE_DECLARE_STATIC_IMMUTABLE(mse::nii_string) simm_string = "abc";
                /* mse::return_value() just returns its argument and ensures that it's of a (pointer) type that's safe to return. */
                return mse::return_value(&simm_string);
            }
        };
        auto simm_ptr3 = CA().foo1();
        auto xs_simm_store3 = mse::make_xscope_strong_pointer_store(simm_ptr3);
        auto xs_cptr3 = xs_simm_store3.xscope_ptr();
        std::cout << *xs_cptr3 << std::endl;
    }
}
```

Note that proper use of the `MSE_RSV_DECLARE_GLOBAL_IMMUTABLE()` macro is not fully enforced in the type system. Full enforcement requires the use of a tool like [scpptool](https://github.com/duneroadrunner/scpptool). In debug builds any unsafe use will be caught at run-time. (This is (now) kind of redundant if you're using an aforementioned enforcement tool.) You can enable the run-time checking in non-debug builds by defining the `MSE_STATICIMMUTABLEPOINTER_RUNTIME_CHECKS_ENABLED` preprocessor symbol.

#### static atomics

Not yet available. Atomics declared `static` (or global) in traditional C++ should be fairly safe. Just make sure your object doesn't contain any indirect elements (i.e. pointers/references).

#### static access controlled objects and access requesters

Not yet available. [Access requesters](#tasyncsharedv2readwriteaccessrequester) declared `static` (or global) would be safe.


### Primitives

### CInt, CSize_t and CBool
These classes are meant to behave like, and be compatible with their native counterparts. In debug mode, they check for "use before initialization", and in release mode, they use default initialization to help ensure deterministic behavior. Upon value assignment involving a narrowing conversion, `CInt` and `CSize_t` will check to ensure that the value fits within the type's range. They check for division by zero and `CSize_t`'s `-=` operator checks that the operation evaluates to a non-negative value. And unlike its native counterpart, arithmetic operations involving `CSize_t` that could evaluate to a negative number are returned as a (signed) `CInt`.

usage example:

```cpp
    #include "mseprimitives.h"
    
    void main(int argc, char* argv[]) {
    
        mse::CInt i = 5;
        i -= 17;
        mse::CSize_t szt = 5;
        szt += 3;
        auto i2 = szt + i;
        mse::CBool b = false;
        if (-4 == i2) {
            b = true;
        }
        if (b) {
            try {
                szt -= 20; // out of range result - this is going to throw an exception
            }
            catch (...) {
                // expected exception
            }
        }
    }
```

Note that while `CInt` and `CSize_t` have no problem interacting with native signed integers, they do not implicitly play well with `size_t` or native unsigned integers. We'd be generally wary of using native unsigned integer types due to the (unintuitive) implicit conversion/promotion rules between signed and unsigned native integers. But if you need to obtain a `size_t` from a `CSize_t`, you can do so explicitly using the `mse::as_a_size_t()` function.   

Btw, `CInt` is actually just an alias for a specific instantiation of the `TInt<>` template, which can be used to make a safe version of any given integer type. (Eg. `typedef mse::TInt<signed char> my_safe_small_int;`)

### CNDInt, CNDSize_t and CNDBool

[`CInt`, `CSize_t` and `CBool`](#cint-csize_t-and-cbool) are intended to be compatible replacements for their native counterparts, and in "disabled" mode they are just aliased to their corresponding native counterparts. There are however, some functional differences between these elements and their native counterparts. For example, they can be used as base classes where their (scalar) native counterparts cannot. So any code that relies on such additional properties might not work properly when the elements are substituted with their native counterparts.

So for those cases `CNDInt`, `CNDSize_t` and `CNDBool` are just versions that are not aliased to their native counterparts in "disabled" mode. In fact, when not in "disabled" mode, `CInt`, `CSize_t` and `CBool` are just aliases for `CNDInt`, `CNDSize_t` and `CNDBool`.

### Quarantined types

Quarantined types are meant to hold values that are obtained from user input or some other untrusted source (like a media file for example). These are not yet available in the library, but are an important concept with respect to safe programming. Values obtained from untrusted sources are the main attack vector of malicious actors and should be handled with special care. For example, the so-called "stagefright" vulnerability in the Android OS is the result of a specially crafted media file causing the sum of integers to overflow.  

It is intended that these types will appropriately handle "extreme" values (at some run-time cost if necessary), and ensure that their values are in an appropriate range when converted to their (high-performance) native counterparts.

### CQuarantinedInt, CQuarantinedSize_t, CQuarantinedVector, CQuarantinedString

Not yet available.

Integer types with more comprehensive range checking can be found here: https://github.com/robertramey/safe_numerics.

### Arrays

The library provides a few array types - [`mstd::array<>`](#mstdarray), [`nii_array<>`](#nii_array) and [`xscope_nii_array<>`](#xscope_nii_array). `mstd::array<>` is simply a memory-safe drop-in replacement for `std::array<>`. Due to their iterators, arrays are not, in general, safe to share among threads.  `nii_array<>` is designed to be safely shared between asynchronous threads. Note that these two arrays do not support using [scope](#scope-pointers) types as the element type, while `xscope_nii_array<>` does. 

And remember that you can use ["random access sections"](#txscoperandomaccesssection-txscoperandomaccessconstsection-trandomaccesssection-trandomaccessconstsection) to provide access to a subsection of any vector or array.

### mstd::array

`mstd::array<>` is a memory-safe drop-in replacement for `std::array<>`. Note that the current implementation requires "`mseregistered.h`".  

usage example:

```cpp
    #include "msemstdarray.h"
    #include "msemsearray.h"
    #include <array>
    
    void main(int argc, char* argv[]) {
    
        mse::mstd::array<int, 3> ma;
        std::array<int, 3> sa;
        /* These two arrays should be completely interchangeable. The difference being that ma should throw
        an exception on any attempt to access invalid memory. */
    
    
        /* mse::msearray is not quite as safe as mse::mstd::array in the following way: */
    
        std::array<int, 3>::iterator sa1_it;
        mse::us::msearray<int, 3>::ss_iterator_type msea1_it; // bounds checked iterator just like mse::mstd::array::iterator
        mse::mstd::array<int, 3>::iterator ma1_it;
        {
            std::array<int, 3> sa1 = { 1, 2, 3 };
            sa1_it = sa1.begin();
    
            mse::us::msearray<int, 3> msea1 = { 1, 2, 3 };
            msea1_it = msea1.ss_begin();
    
            mse::mstd::array<int, 3> ma1 = { 1, 2, 3 };
            ma1_it = ma1.begin();
        }
    
        // (*sa1_it) = 4; // not good - undefined behavior
        // (*msea1_it) = 4; // not good - undefined behavior
    
        try {
            (*ma1_it) = 4; // not undefined behavior - will throw an exception
        } catch(...) {
            // expected exception
        }
    }
```

### nii_array

Due to their iterators, `std::array<>`s (and `mstd::array<>`s) are not, in general, safe to share among threads. `nii_array<>` is a "stripped down" array that does not support "implicit" iterators, allowing it to be safely shareable between asynchronous threads. "Explicit" iterators are supported. That is, in order to obtain an iterator, you must explicitly provide a (safe) pointer to the `nii_array<>`. So for example, instead of a `begin()` member function (that takes no parameters), you can obtain an iterator using the (generic) `make_begin_iterator(...)` free function that takes as an argument a (safe) pointer to the array.  

`nii_array<>`s are swappable with `mstd::array<>`s (and `std::array<>`s).

Also note that an `nii_array<>` will be (automatically) marked as [safely shareable](#asynchronously-shared-objects) only if its element type is known or declared to be safely shareable.

You might choose to use `nii_array<>`s over `mstd::array<>`s even in cases where the array is not being shared among threads, as it actually has less overhead.

usage example: (see the similar [`fixed_nii_vector<>`](#fixed_nii_vector))

### xscope_nii_array<>

`xscope_nii_array<>` is just the [scope](#scope-pointers) version of [`nii_array<>`](#nii_array). So unlike `nii_array<>`, `xscope_nii_array<>` supports elements that are scope types. But as a scope type itself, `xscope_nii_array<>` is subject to the restrictions of scope objects. Additionally, in cases where the element type is or contains a scope pointer or reference object, operations that modify array elements (such as `swap()`, or the assignment operator or the `fill()` member function) are disabled. (Though the subscript operator and `at()` member function can still yield a non-`const` reference to any specified element.)

### xscope_iterator

The implementation of, for example, `mstd::array<>` iterators uses [registered pointers](#registered-pointers) to ensure that iterators are not used to access array elements after the array has been deallocated. This incurs a slight run-time cost. So just as the library provides [scope pointers](#scope-pointers) without run-time cost, scope iterators for arrays are also provided. Scope iterators have usage restrictions similar to scope pointers. For example, they can only target arrays declared as scope objects, and may not be used as a member of any class or struct that is not itself a scope object, and may not be used as a function return value. `mstd::array<>`, `nii_array<>` and `xscope_nii_array<>` all support scope iterators.

And from a scope iterator, you can obtain a scope pointer to its target element.

usage example:

```cpp
    #include "msemstdarray.h"
    
    void main(int argc, char* argv[]) {
        /* If the array is declared as a "scope" object (which basically indicates that it is declared
        on the stack), then you can use "scope" iterators. While there are limitations on when they can
        be used, scope iterators would be the preferred iterator type where performance is a priority
        as they don't require extra run time overhead to ensure that the array has not been deallocated. */
        
        /* Here we're declaring an array as a scope object. */
        mse::TXScopeObj<mse::mstd::array<int, 3>> array1_scpobj = mse::mstd::array<int, 3>{ 1, 2, 3 };
        
        /* Here we're obtaining a scope iterator to the array. */
        auto scp_array_iter1 = mse::mstd::make_xscope_begin_iterator(&array1_scpobj);
        auto scp_array_iter2 = mse::mstd::make_xscope_end_iterator(&array1_scpobj);
        
        std::sort(scp_array_iter1, scp_array_iter2);
        
        auto scp_array_citer3 = mse::mstd::make_xscope_begin_const_iterator(&array1_scpobj);
        scp_array_citer3 = scp_array_iter1;
        scp_array_citer3 = mse::mstd::make_xscope_begin_const_iterator(&array1_scpobj);
        scp_array_citer3 += 2;
        auto res1 = *scp_array_citer3;
        auto res2 = scp_array_citer3[0];
        
        /* Here we demonstrate the case where the array is a member of a class/struct declared as a
        scope object. */
        class CContainer1 {
        public:
            CContainer1() : m_vector({ 1, 2, 3 }) {}
            mse::mstd::array<int, 3> m_array;
        };
        mse::TXScopeObj<CContainer1> container1_scpobj;
        auto container1_m_array_scpptr = mse::make_xscope_pointer_to_member_v2(&container1_scpobj, &CContainer1::m_array);
        auto scp_iter4 = mse::mstd::make_xscope_begin_iterator(container1_m_array_scpptr);
        scp_iter4++;
        auto res3 = *scp_iter4;
        
        /* You can also obtain a corresponding scope pointer from a scope iterator. */
        auto scp_ptr1 = mse::xscope_pointer(scp_iter4);
        auto res4 = *scp_ptr1;
        auto scp_cptr1 = mse::xscope_const_pointer(scp_iter4);
        auto res5 = *scp_cptr1;
    }
```

### Vectors

The library provides a number of vector types. Like their [array](#arrays) counterparts, [`mstd::vector<>`](#mstdvector) is a memory-safe drop-in replacement for `std::vector<>` and [`nii_vector<>`](#nii_vector) is a vector that doesn't support "implicit" iterators and (so) is eligible to be safely shared among asynchronous threads.

But unlike (fixed-size) arrays, dynamic containers like (resizable) vectors present an extra memory safety challenge in that the container may be modified/resized so that an element in the container might be eliminated, or moved to a different location, while there are still references (iterators) targeting the element. This results in dynamic containers having additional overhead (for safety mechanisms) and/or usage restrictions. For this reason, directly referencing elements in these (resizable) vectors is discouraged. You may instead move the vector contents to a (more efficient) "non-resizable" vector and access the elements from that container. A [`fixed_nii_vector<>`](#fixed_nii_vector) is provided, but generally you'll want to use [`xscope_borrowing_fixed_nii_vector<>`](#xscope_borrowing_fixed_nii_vector). Upon construction it "borrows" the contents of a specified vector and returns the (possibly modified) contents upon destruction. The premise here is that the moving of a vector's contents is an inexpensive operation that might be expected to be completely optimized away in cases where the contents are returned to their original location. Like arrays, fixed-size vectors support (efficient) [scope iterators](#xscope_iterator).

So to restate, the idea is to avoid referencing elements in dynamic containers like resizable vectors. To access (or hold references to) the elements, move the contents (if only temporarily) to a non-dynamic container, like a fixed-size vector.

But if there are occasions when you want to perform a dynamic/resizing operation (like inserting or removing elements) while holding references to individual elements, traditional vectors aren't ideal for this as such operations can invalidate any outstanding iterators. Traditionally a list container might be considered more appropriate for these situations as its iterators tend to remain intact after insertion/removal operations. But lists are generally not as "cache friendly" as vectors, which can be an issue when performance is a priority. So the library also includes [`ivector<>`](#ivector), a vector whose iterators behave like list iterators. That is, they don't get invalidated by insert/delete/resize vector operations unless the element they were pointing to is deleted, and after any such operation, they will continue to point to the same item, which may then be in a different position in the vector.

And remember that you can use ["random access sections"](#txscoperandomaccesssection-txscoperandomaccessconstsection-trandomaccesssection-trandomaccessconstsection) to provide access to a subsection of any vector or array.

### mstd::vector

`mstd::vector<>` is a memory-safe drop-in replacement for `std::vector<>`.

usage example:

```cpp
    #include "msemstdvector.h"
    #include <vector>
    
    void main(int argc, char* argv[]) {
    
        mse::mstd::vector<int> mv;
        std::vector<int> sv;
        /* These two vectors should be completely interchangeable. The difference being that mv should throw
        an exception on any attempt to access invalid memory. */
        
        std::vector<int>::iterator sv1_it;
        mse::mstd::vector<int>::iterator mv1_it;
        {
            std::vector<int> sv1 = { 1, 2, 3 };
            sv1_it = sv1.begin();
            
            mse::mstd::vector<int> mv1 = { 1, 2, 3 };
            mv1_it = mv1.begin();
        }
        
        // (*sv1_it) = 4; // not good
        
        try {
            /* At this point, mv1's data has not actually been deallocated/destructed yet because it "knows" that there
        is an iterator, namely mv1_it, that is still referencing it. It will be deallocated when there are no more
        iterators referencing it. */
        
            (*mv1_it) = 4; // In debug mode this will fail an assert. In non-debug mode it'll just work (safely). 
        } catch(...) {
            /* At present, no exception will be thrown. With future library implementations, maybe. */
        }
    }
```

### nii_vector

Like its array counterpart, [`nii_array<>`](#nii_array), `nii_vector<>` is a vector that does not support "implicit" iterators, allowing it to be safely shareable between asynchronous threads. You can obtain "explicit" iterators using the (generic) `make_begin_iterator(...)` free function that takes as an argument a (safe) pointer to the vector, but directly accessing the elements of an `nii_vector<>` is discouraged. Prefer instead to access (or hold references to) vector elements by using an [`xscope_borrowing_fixed_nii_vector<>`](#xscope_borrowing_fixed_nii_vector). (Indeed, if the elements are going to be accessed or referenced directly, consider using [`stnii_vector<>`](#stnii_vector) instead.)

Note that the standard vector `insert()`, `erase()` and `emplace()` member functions return "implicit" iterators which, again, `nii_vector<>` does not support. Instead there are free function counterparts that take a (safe) pointer to the container as their first argument which is used to generate and return an "explicit" iterator (as demonstrated in the usage example).

`nii_vector<>`s are swappable with `mstd::vector<>`s (and `std::vector<>`s).

Also note that an `nii_vector<>` will be (automatically) marked as [safely shareable](#asynchronously-shared-objects) only if its element type is known or declared to be safely shareable.

You might choose to use `nii_vector<>`s over `mstd::vector<>`s even in cases where the vector is not being shared among threads, as it actually has less overhead.

usage example: (see the example for [`xscope_borrowing_fixed_nii_vector<>`](#xscope_borrowing_fixed_nii_vector))

### fixed_nii_vector

A `fixed_nii_vector<>` is basically like an [`nii_array<>`](#nii_array) (i.e. not resizable) whose size is determined at construction rather than at compile-time. (Except that unlike an array, a `fixed_nii_vector<>` can (cheaply) "take" the contents of a dynamic/resizable vector during move construction, leaving the source vector empty.)

usage example:
```cpp
    #include "msemsevector.h"
    #include "mseasyncshared.h"
    #include "mseregistered.h"
    
    void main(int argc, char* argv[]) {
    
        typedef mse::fixed_nii_vector<mse::nii_string> fixed_nii_vector1_t;

        mse::TRegisteredObj<fixed_nii_vector1_t> rg_vo1{ mse::nii_string{"abc"}, mse::nii_string{"def"} };
        mse::TRegisteredPointer<fixed_nii_vector1_t> vo1_regptr1 = &rg_vo1;

        /* fixed_nii_vector<> does not have a begin() member function that returns an "implicit" iterator. You can obtain an
        iterator using the make_begin_iterator() et al. functions, which take a (safe) pointer to the container. */
        auto iter1 = mse::make_begin_iterator(vo1_regptr1);
        auto citer1 = mse::make_end_const_iterator(vo1_regptr1);
        citer1 = iter1;

        /* fixed_nii_vector<>s can be constructed from other (resizeable) vector types */
        auto nvec1 = mse::nii_vector<int>{ 1, 2, 3 };
        mse::fixed_nii_vector<int> fixed_nvec1(nvec1);
        assert(fixed_nvec1.size() == nvec1.size());
        mse::fixed_nii_vector<int> fixed_nvec2(mse::mstd::vector<int>{ 1, 2 });

        class A {
        public:
            A() {}
            int m_i = 0;
        };
        /* Here we're declaring that A can be safely shared between asynchronous threads. */
        typedef mse::rsv::TAsyncShareableAndPassableObj<A> shareable_A_t;

        /* When the element type of an fixed_nii_vector<> is marked as "async shareable", the fixed_nii_vector<> itself is
        (automatically) marked as async shareable as well and can be safely shared between asynchronous threads
        using "access requesters". */
        auto access_requester1 = mse::make_asyncsharedv2readwrite<mse::fixed_nii_vector<shareable_A_t>>();
        auto access_requester2 = mse::make_asyncsharedv2readwrite<fixed_nii_vector1_t>();

        /* If the element type of an fixed_nii_vector<> is not marked as "async shareable", then neither is the
        fixed_nii_vector<> itself. So attempting to create an "access requester" using it would result in a compile
        error. */
        //auto access_requester3 = mse::make_asyncsharedv2readwrite<mse::fixed_nii_vector<A>>();
        //auto access_requester4 = mse::make_asyncsharedv2readwrite<mse::fixed_nii_vector<mse::mstd::string>>();

        {
            /* If the vector is declared as a "scope" object (which basically indicates that it is declared
            on the stack), then you can use "scope" iterators. While there are limitations on when they can
            be used, scope iterators would be the preferred iterator type where performance is a priority
            as they don't require extra run-time overhead to ensure that the vector has not been prematurely
            deallocated. */

            /* Here we're declaring a fixed vector as a scope object. */
            mse::TXScopeObj<mse::fixed_nii_vector<int> > vector1_xscpobj{ 1, 2, 3 };

            {
                /* Here we're obtaining a scope iterator to the vector. */
                auto xscp_iter1 = mse::make_xscope_begin_iterator(&vector1_xscpobj);
                auto xscp_iter2 = mse::make_xscope_end_iterator(&vector1_xscpobj);

                std::sort(xscp_iter1, xscp_iter2);

                /* Here we're obtaining a scope pointer from a scope iterator. */
                auto xscp_ptr1 = mse::xscope_pointer(xscp_iter1);
                auto res3 = *xscp_ptr1;

                auto xscp_citer3 = mse::make_xscope_begin_const_iterator(&vector1_xscpobj);
                xscp_citer3 = xscp_iter1;
                xscp_citer3 = mse::make_xscope_begin_const_iterator(&vector1_xscpobj);

                xscp_citer3 += 2;
                auto res1 = *xscp_citer3;
                auto res2 = xscp_citer3[0];
            }
        }
    }
```

### xscope_fixed_nii_vector
`xscope_fixed_nii_vector<>` is just the [scope](#scope-pointers) version of [`fixed_nii_vector<>`](#fixed_nii_vector). So unlike `fixed_nii_vector<>`, `xscope_fixed_nii_vector<>` supports elements of scope type. But as a scope type itself, `xscope_fixed_nii_vector<>` is subject to the restrictions of scope objects.

### xscope_borrowing_fixed_nii_vector

`xscope_borrowing_fixed_nii_vector<>` is a kind of [`xscope_fixed_nii_vector<>`](#xscope_fixed_nii_vector) that, at construction, "borrows" (or "takes" by moving) the contents of a specified (via [scope pointer](#scope-pointers)) existing vector, then, upon destruction "returns" the (possibly modified) contents back to the original owner.

usage example:
```cpp
    #include "msemsevector.h"
    #include "mseasyncshared.h"
    
    void main(int argc, char* argv[]) {
    
        auto xs_nii_vector1_xscpobj = mse::make_xscope(mse::nii_vector<int>{ 1, 2, 3 });
        /* first we demonstrate some resizing operations on the nii_vector<> */
        /* Note that the standard emplace(), insert() and erase() member functions return an iterator. Since nii_vector<> doesn't
        support "implicit" iterators (i.e. iterators generated from the native "this" pointer) those operations are provided by
        free functions that take an explicit (safe) "this" pointer used to generate and return an explicit iterator. */
        mse::push_back(&xs_nii_vector1_xscpobj, 4/*value*/);
        mse::erase(&xs_nii_vector1_xscpobj, 2/*position index*/);
        mse::insert(&xs_nii_vector1_xscpobj, 1/*position index*/, 5/*value*/);
        mse::insert(&xs_nii_vector1_xscpobj, 0/*position index*/, { 6, 7, 8}/*value*/);
        mse::emplace(&xs_nii_vector1_xscpobj, 2/*position index*/, 9/*value*/);

        const auto nii_vector1_expected = mse::nii_vector<int>{ 6, 7, 9, 8, 1, 5, 2, 4 };
        assert(nii_vector1_expected == xs_nii_vector1_xscpobj);

        /* Constructing a xscope_borrowing_fixed_nii_vector<> requires a (non-const) scope pointer to an eligible vector. */
        auto xs_bf_nii_vector1_xscpobj = mse::make_xscope(mse::make_xscope_borrowing_fixed_nii_vector(&xs_nii_vector1_xscpobj));

        {
            /* Here we're obtaining a scope iterator to the fixed vector. */
            auto xscp_iter1 = mse::make_xscope_begin_iterator(&xs_bf_nii_vector1_xscpobj);
            auto xscp_iter2 = mse::make_xscope_end_iterator(&xs_bf_nii_vector1_xscpobj);

            std::sort(xscp_iter1, xscp_iter2);

            /* Here we're obtaining a scope pointer from a scope iterator. */
            auto xscp_ptr1 = mse::xscope_pointer(xscp_iter1);
            auto res3 = *xscp_ptr1;

            auto xscp_citer3 = mse::make_xscope_begin_const_iterator(&xs_bf_nii_vector1_xscpobj);
            xscp_citer3 = xscp_iter1;
            xscp_citer3 = mse::make_xscope_begin_const_iterator(&xs_bf_nii_vector1_xscpobj);

            xscp_citer3 += 2;
            auto res1 = *xscp_citer3;
            auto res2 = xscp_citer3[0];
        }

        class A {
        public:
            A() {}
            int m_i = 0;
        };
        /* Here we're declaring that A can be safely shared between asynchronous threads. */
        typedef mse::rsv::TAsyncShareableAndPassableObj<A> shareable_A_t;

        /* When the element type of an xscope_borrowing_fixed_nii_vector<> is marked as "async shareable", the
        xscope_borrowing_fixed_nii_vector<> itself is (automatically) marked as async shareable as well and can be safely
        shared between asynchronous threads using "access requesters". */

        auto xs_shareable_nii_vector1 = mse::make_xscope(mse::nii_vector<shareable_A_t>{});
        auto xs_aco_bf_nii_vector1 = mse::make_xscope(mse::make_xscope_access_controlled(mse::make_xscope_borrowing_fixed_nii_vector(&xs_shareable_nii_vector1)));
        auto xs_access_requester1 = mse::make_xscope_asyncsharedv2acoreadwrite(&xs_aco_bf_nii_vector1);

        auto xs_shareable_nii_vector2 = mse::make_xscope(mse::nii_vector<mse::nii_string>{ "abc", "def" });
        auto xs_aco_bf_nii_vector2 = mse::make_xscope(mse::make_xscope_access_controlled(mse::make_xscope_borrowing_fixed_nii_vector(&xs_shareable_nii_vector2)));
        auto xs_access_requester2 = mse::make_xscope_asyncsharedv2acoreadwrite(&xs_aco_bf_nii_vector2);

        auto xs_shareable_nii_vector3 = mse::make_xscope(mse::nii_vector<A>{});
        /* If the element type of an xscope_borrowing_fixed_nii_vector<> is not marked as "async shareable", then neither is the
        xscope_borrowing_fixed_nii_vector<> itself. So attempting to create an "access controlled object" using it would result in
        a compile error. */
        //auto xs_aco_bf_nii_vector3 = mse::make_xscope(mse::make_xscope_access_controlled(mse::make_xscope_borrowing_fixed_nii_vector(&xs_shareable_nii_vector3)));
        //auto xs_access_requester3 = mse::make_xscope_asyncsharedv2acoreadwrite(&xs_aco_bf_nii_vector3);
    }
```

### ivector

`mse::ivector<>` is a safe vector like [`mse::mstd::vector<>`](#mstdvector), but its (implicit) iterators behave more like list iterators than standard vector iterators. That is, upon insert or delete, the iterators continue to point to the same item, not (necessarily) the same position. And they don't become "invalid" upon insert or delete, unless the item they point to is deleted.

usage example:
```cpp
    #include "mseivector.h"
    
    void main(int argc, char* argv[]) {
        mse::ivector<int> v = { 1, 2, 3, 4 };

        auto ip1 = v.begin();
        ip1 += 2;
        assert(3 == (*ip1));
        auto ip2 = v.begin();
        v.erase(ip2); /* remove the first item */
        assert(3 == (*ip1)); /* ip1 continues to point to the same item, not the same position */
        ip1--;
        assert(2 == (*ip1));
        for (auto cip = v.cbegin(); v.cend() != cip; cip++) {
            /* You might imagine what would happen if cip were a regular vector iterator. */
            v.insert(v.begin(), (*cip));
        }

        /* Btw, the iterators are compatible with stl algorithms, like any other stl iterators. */
        std::sort(v.begin(), v.end());
    }
```

### structure locking

While the preferred method for accessing elements of a resizable vector is via [`xscope_borrowing_fixed_nii_vector`](#xscope_borrowing_fixed_nii_vector), note that constructing an `xscope_borrowing_fixed_nii_vector` requires a non-const [scope pointer](#scope-pointers) to the resizable vector. For situations where such a non-const scope pointer is not available, the elements can be accessed directly from the resizable vector, potentially with a little more overhead, and some restrictions. 

Like the arrays and fixed-size vectors, the provided resizable vectors also support [scope iterators](#xscope_iterator). But note that the scope iterators of resizable vectors have some somewhat subtle usage constraints that scope iterators of fixed-size vectors don't have. For safety/performance reasons, scope iterators of "dynamic" containers, like resizable vectors, have the side effect, while they exist, of "structure locking" the associated container. That is, they put the container in a state such that it cannot be resized or relocated, while still allowing the existing contents of the container to be modified or replaced. The "structure lock" is released upon destruction of the scope iterator.

So when you obtain a [scope pointer](#scope-pointers) to a vector element from a scope iterator, the assurance that the scope pointer's target element remains valid relies on the fact that the resizable vector is structure locked for the duration of the scope pointer's existence. Since the structure lock is maintained by the scope iterator, the scope iterator must be assured to outlive the obtained scope pointer. This means that obtaining a scope pointer from a temporary/rvalue vector scope iterator is not supported. (Attempting to do so results in a compile error.) Again, this is not the case for array or fixed-size vector scope iterators. 

### mtnii_vector

[`nii_vector<>`](#nii_vector) has a subtle restriction. Unlike the other vectors, its [scope const iterator](#xscope_iterator) doesn't "[structure lock](#structure-locking)" the vector and therefore, you cannot obtain a [scope pointer](#txscopefixedpointer) from it. The reason it doesn't do structure locking is because const iterators to the same vector can be created in multiple different threads simultaneously and `nii_vector<>` does not have a thread-safe structure locking mechanism.

So the library provides `mtnii_vector<>`, essentially a version of `nii_vector<>` that does have a thread-safe (atomic) structure locking mechanism, and so doesn't suffer the same restriction. The thread-safe locking mechanism does incur some extra cost, so you'd generally only want to use it when you need to share a resizable vector among asynchronous threads. (So presumably rarely.)

usage example:

```cpp
    #include "msemsevector.h"
    #include "mseasyncshared.h"
    #include "mseregistered.h"
    
    void main(int argc, char* argv[]) {
    
        /* mtnii_vector<> is a safe vector that is eligible to be (safely) shared between asynchronous threads. */

        typedef mse::mtnii_vector<mse::mtnii_string> mtnii_vector1_t;

        mse::TRegisteredObj<mtnii_vector1_t> rg_vo1;
        for (size_t i = 0; i < 5; i += 1) {
            rg_vo1.push_back("some text");
        }
        mse::TRegisteredPointer<mtnii_vector1_t> vo1_regptr1 = &rg_vo1;

        /* mtnii_vector<> does not have a begin() member function that returns an "implicit" iterator. You can obtain an
        iterator using the make_begin_iterator() et al. functions, which take a (safe) pointer to the container. */
        auto iter1 = mse::make_begin_iterator(vo1_regptr1);
        auto citer1 = mse::make_end_const_iterator(vo1_regptr1);
        citer1 = iter1;
        rg_vo1.emplace(vo1_regptr1, citer1, "some other text");
        rg_vo1.insert(vo1_regptr1, citer1, "some other text");
        mse::mtnii_string str1 = "some other text";
        rg_vo1.insert(vo1_regptr1, citer1, str1);

        class A {
        public:
            A() {}
            int m_i = 0;
        };
        /* Here we're declaring that A can be safely shared between asynchronous threads. */
        typedef mse::rsv::TAsyncShareableAndPassableObj<A> shareable_A_t;

        /* When the element type of an mtnii_vector<> is marked as "async shareable", the mtnii_vector<> itself is
        (automatically) marked as async shareable as well and can be safely shared between asynchronous threads
        using "access requesters". */
        auto access_requester1 = mse::make_asyncsharedv2readwrite<mse::mtnii_vector<shareable_A_t>>();
        auto access_requester2 = mse::make_asyncsharedv2readwrite<mtnii_vector1_t>();

        /* If the element type of an mtnii_vector<> is not marked as "async shareable", then neither is the
        mtnii_vector<> itself. So attempting to create an "access requester" using it would result in a compile
        error. */
        //auto access_requester3 = mse::make_asyncsharedv2readwrite<mse::mtnii_vector<A>>();
        //auto access_requester4 = mse::make_asyncsharedv2readwrite<mse::mtnii_vector<mse::mstd::string>>();

        typedef mse::mstd::vector<mse::mtnii_string> vector1_t;
        vector1_t vo2 = { "a", "b", "c" };
        /* mstd::vector<>s, for example, are not safely shareable between threads. But if its element type is
        safely shareable, then the contents of the mse::mstd::vector<>, can be swapped with a corresponding
        shareable mtnii_vector<>. Note that vector swaps are intrinsically fast operations. */
        std::swap(vo2, *(access_requester2.writelock_ptr()));

        {
            /* If the vector is declared as a "scope" object (which basically indicates that it is declared
            on the stack), then you can use "scope" iterators. While there are limitations on when they can
            be used, scope iterators would be the preferred iterator type where performance is a priority
            as they don't require extra run-time overhead to ensure that the vector has not been prematurely
            deallocated. */

            /* Here we're declaring a vector as a scope object. */
            mse::TXScopeObj<mse::mtnii_vector<int> > vector1_xscpobj = mse::mtnii_vector<int>{ 1, 2, 3 };

            {
                /* Here we're obtaining a scope iterator to the vector. */
                auto xscp_iter1 = mse::make_xscope_begin_iterator(&vector1_xscpobj);
                auto xscp_iter2 = mse::make_xscope_end_iterator(&vector1_xscpobj);

                std::sort(xscp_iter1, xscp_iter2);

                /* Note that scope iterators to vectors (and other dynamic containers), "lock the structure" of the container
                so that, for example, it cannot be resized. This allows us to obtain a scope pointer to the iterator's
                target element. */
                auto xscp_ptr1 = mse::xscope_pointer(xscp_iter1);
                auto res3 = *xscp_ptr1;

                auto xscp_citer3 = mse::make_xscope_begin_const_iterator(&vector1_xscpobj);
                xscp_citer3 = xscp_iter1;
                xscp_citer3 = mse::make_xscope_begin_const_iterator(&vector1_xscpobj);
                xscp_citer3 += 2;
                auto res1 = *xscp_citer3;
                auto res2 = xscp_citer3[0];
            }
            /* After all the scope pointers have gone out of scope, you may again perform operations that affect the container's
            "structure" (i.e. size or capacity). */
            vector1_xscpobj.push_back(4);
        }
    }
```

### stnii_vector

`stnii_vector<>` is a "performance" version of [`mtnii_vector<>`](#mtnii_vector) that is not eligible to be shared among threads. Because scope iterators ["structure lock"](#structure-locking) the container when they are created, and because it's possible to simultaneously/concurrently create multiple (const) scope iterators in different threads, `mtnii_vector<>` employs a thread-safe (atomic) locking mechanism. This means that every operation that affects the size of an `mtnii_vector<>` makes a thread-safe (atomic) access operation. Since `stnii_vector<>` is not eligible to be shared among threads, it does not need to perform any costly thread-safe access operations. 

While you might prefer `stnii_vector<>` to [`nii_vector<>`](#nii_vector) in cases where the vector won't be shared between asynchronous threads, using [`xscope_borrowing_fixed_nii_vector<>`](#xscope_borrowing_fixed_nii_vector) would still be the preferred method of accessing (or holding references to) the elements.

usage example:
```cpp
    #include "msescope.h"
    #include "msemsevector.h"
    #include "msemstdvector.h"
    
    void main(int argc, char* argv[]) {
    
        /* stnii_vector<> is just a version of mtnii_vector<> that is not eligible to be shared between threads (and has
        a little less overhead as a result). */

        mse::TXScopeObj<mse::stnii_vector<int> > vector1_xscpobj = mse::stnii_vector<int>{ 1, 2, 3 };

        {
            mse::TXScopeFixedConstPointer<mse::stnii_vector<int> > xscptr = &vector1_xscpobj;
            auto xscp_citer1 = mse::make_xscope_begin_const_iterator(xscptr);
            xscp_citer1 += 2;
            auto xscp_cptr1 = mse::xscope_const_pointer(xscp_citer1);
            auto res1 = *xscp_cptr1;
        }
        vector1_xscpobj.push_back(4);

        /* stnii_vector<>s can be (efficiently) swapped with mtnii_vector<>s. */
        auto mtniiv1 = mse::mtnii_vector<int>();
        std::swap(vector1_xscpobj, mtniiv1);
        /* Or mstd::vector<>s. */
        auto mstdv1 = mse::mstd::vector<int>();
        std::swap(vector1_xscpobj, mstdv1);
    }
```

### exclusive writer protected dynamic containers

As [noted previously](#mtnii_vector), unlike the other vectors, (for thread safety reasons) [`nii_vector<>`](#nii_vector)'s [scope const iterator](#xscope_iterator) doesn't "[structure lock](#structure-locking)" the vector and therefore, you cannot obtain a [scope pointer](#txscopefixedpointer) from it. However, an object protected by an "[excluse writer](#exclusive-writer-objects)" access control wrapper is ensured to remain unmodified while a corresponding "access controlled const pointer" exists. So it is safe to obtain a (const) scope pointer from a scope const iterator constructed from an "access controlled const pointer" of an `nii_vector<>` protected by an "excluse writer" access control wrapper.

usage example:

```cpp
    #include "msemsevector.h"
    
    void main(int argc, char* argv[]) {
        /* Unfortunately, you cannot obtain a direct scope const pointer to an nii_vector<> element from a scope const
        pointer to the nii_vector<>. (nii_vector<> is the only one of the library's vectors that has this shortcoming.)
        However, for vectors that are access controlled with an "exclusive writer" access policy, you can use an
        "exclusive writer" const pointer to obtain a direct scope const pointer to a vector element. */
        auto xs_ew_niivec1 = mse::make_xscope(mse::make_exclusive_writer(mse::nii_vector<int>{ 1, 2, 3 }));
        // which can also be written as:
        // mse::TXScopeObj<mse::TExclusiveWriterObj<mse::nii_vector<int> > > xs_ew_niivec1 = mse::nii_vector<int>{ 1, 2, 3 };
        {
            auto xs_ew_cptr1 = mse::make_xscope_access_controlled_const_pointer(xs_ew_niivec1);
            /* The following scope iterator is of a type that reflects that it was constructed from an "exclusive writer"
            const pointer. */
            auto xs_citer1 = mse::make_xscope_begin_const_iterator(xs_ew_cptr1);
            xs_citer1 += 2;
            /* We can obtain a (zero-overhead) scope pointer from the (lvalue) iterator. */
            auto xs_cptr1 = mse::xscope_const_pointer(xs_citer1);
            auto val1 = *xs_cptr1;
        }
        /* Once the "access controlled const pointer" no longer exists, we can obtain an "access controlled 
        (non-const) pointer" and modify the vector again. */
        mse::push_back(mse::make_xscope_access_controlled_pointer(xs_ew_niivec1), 4);
    }
```

### TXScopeRandomAccessSection, TXScopeRandomAccessConstSection, TRandomAccessSection, TRandomAccessConstSection

A "random access section" is basically a convenient interface to access a (contiguous) subsection of an existing array or vector. (Essentially an "array view" or "span" if you're familiar with those.) You construct them, using the `make_random_access_section()` functions, by specifying an iterator to the start of the section, and the length of the section. Random access sections support most of the member functions and operators that [std::basic_string_view](http://en.cppreference.com/w/cpp/string/basic_string_view) does, except that the "[substr()](http://en.cppreference.com/w/cpp/string/basic_string_view/substr)" member function is named "subsection()".

Note that for convenience, random access sections can be constructed from just a (safe) pointer to a supported container object, but in some cases the exact type of the resulting random access section may not be obvious. Constructing instead from a specified iterator and length should avoid any ambiguity.

usage example:

```cpp
    #include "msemsearray.h" //random access sections are defined in this file
    #include "msemstdarray.h"
    #include "msemstdvector.h"
    
    class J {
    public:
        template<class _TRASection>
        static void foo13(_TRASection ra_section) {
            for (_TRASection::size_type i = 0; i < ra_section.size(); i += 1) {
                ra_section[i] = 0;
            }
        }
        template<class _TRAConstSection>
        static int foo14(_TRAConstSection const_ra_section) {
            int retval = 0;
            for (_TRAConstSection::size_type i = 0; i < const_ra_section.size(); i += 1) {
                retval += const_ra_section[i];
            }
            return retval;
        }
        template<class _TRAConstSection>
        static int foo15(_TRAConstSection const_ra_section) {
            int retval = 0;
            for (const auto& const_item : const_ra_section) {
                retval += const_item;
            }
            return retval;
        }
    };

    void main(int argc, char* argv[]) {
        mse::mstd::array<int, 4> mstd_array1{ 1, 2, 3, 4 };
        mse::mstd::vector<int> mstd_vec1{ 10, 11, 12, 13, 14 };

        auto xscp_ra_section1 = mse::make_xscope_random_access_section(mstd_array1.begin(), 2);
        J::foo13(xscp_ra_section1);

        auto ra_const_section2 = mse::make_random_access_const_section(++mstd_vec1.begin(), 3);
        auto res6 = J::foo15(ra_const_section2);
        auto res7 = J::foo14(ra_const_section2);

        auto xscp_ra_section1_xscp_iter1 = xscp_ra_section1.xscope_begin();
        auto xscp_ra_section1_xscp_iter2 = xscp_ra_section1.xscope_end();
        auto res8 = xscp_ra_section1_xscp_iter2 - xscp_ra_section1_xscp_iter1;
        bool res9 = (xscp_ra_section1_xscp_iter1 < xscp_ra_section1_xscp_iter2);
        
        /* Like non-owning scope pointers, scope sections may not be used as a function return value. (The return_value()
        function wrapper enforces this.) Pretty much the only time you'd legitimately want to do this is when the
        returned section is constructed from one of the input parameters. Let's consider a simple example of a
        "first_half()" function that takes a scope section and returns a scope section spanning the first half of the
        section. The library provides the random_access_subsection() function which takes a random access section and a
        tuple containing a start index and a length and returns a random access section spanning the indicated
        subsection. You could use this function to implement the equivalent of a "first_half()" function like so: */
        
        auto xscp_ra_section3 = mse::random_access_subsection(xscp_ra_section1, std::make_tuple(0, xscp_ra_section1.length()/2));
        assert(xscp_ra_section3.length() == 1);
    }
```

Note that scope sections, like (non-owning) scope pointers, are not retargetable (i.e. don't support the assignment operator). If you find yourself needing a retargetable scope section, you can use the technique of adding a level of indirection and using a (retargetable) registered (or norad) pointer to the scope sections, is [demonstrated](#retargetable-references-to-scope-objects) with scope pointers.

See also [TXScopeCSSSXSTERandomAccessSection](https://github.com/duneroadrunner/SaferCPlusPlus/blob/master/README.md#txscopecsssxsterandomaccessiterator-and-txscopecsssxsterandomaccesssection).

### Strings

From an interface perspective, you might think of strings roughly as glorified vectors of characters, and thus they are given similar treatment in the library. A few string types are provided that correspond to their [vector](#vectors) counterparts. [`mstd::string`](#mstdstring) is a memory-safe drop-in replacement for `std::string` and [`nii_string`](#nii_string) is a string that doesn't support "implicit" iterators and (so) is eligible to be safely shared among asynchronous threads. [`xscope_borrowing_fixed_nii_basic_string<>`](#xscope_borrowing_fixed_nii_basic_string) is a non-resizable string that facilitates more efficient and less restricted direct access to characters in the string.

### mstd::string

`mstd::string` is a memory-safe drop-in replacement for `std::string`. As with the standard library, `mstd::string` is defined as an alias for `mstd::basic_string<char>`. The `mstd::wstring`, `mstd::u16string` and `mstd::u32string` aliases are also present.

### nii_string

Like its vector counterpart, [`nii_vector<>`](#nii_vector), `nii_string` is a string that does not support "implicit" iterators, allowing it to be safely shareable between asynchronous threads. But directly accessing the characters of an `nii_string` is discouraged. Prefer instead to access (or hold references to) characters of the string by using an [`xscope_borrowing_fixed_nii_basic_string<>`](#xscope_borrowing_fixed_nii_basic_string). The `nii_wstring`, `nii_u16string` and `nii_u32string` aliases are also present.

### fixed_nii_basic_string

See the [`fixed_nii_vector<>`](#fixed_nii_vector) counterpart.

### xscope_fixed_nii_basic_string

See the [`xscope_fixed_nii_vector<>`](#xscope_fixed_nii_vector) counterpart.

### xscope_borrowing_fixed_nii_basic_string

See the [`xscope_borrowing_fixed_nii_vector<>`](#xscope_borrowing_fixed_nii_vector) counterpart.

### TXScopeStringSection, TXScopeStringConstSection, TStringSection, TStringConstSection

"String sections" are string specialized versions of "[random access sections](#txscoperandomaccesssection-txscoperandomaccessconstsection-trandomaccesssection-trandomaccessconstsection)". 

usage example:

```cpp
    #include "msemsestring.h" // make_string_section() is defined in this header
    #include "msemstdstring.h"
    
    void main(int argc, char* argv[]) {

        /* "String sections" are the string specialized versions of "random access sections", basically providing the
        functionality of std::string_view but supporting construction from any (safe) iterator type, not just raw
        pointer iterators. */
    
        mse::mstd::string mstring1("some text");
        auto string_section1 = mse::make_string_section(mstring1.begin() + 1, 7);
        auto string_section2 = string_section1.substr(4, 3);
        assert(string_section2.front() == 't');
        assert(string_section2.back() == 'x');
    
        /* Unlike std::string_view, string sections are available in "non-const" versions. */
        string_section2[0] = 'T';
        std::cout << string_section2;
        assert(mstring1 == "some Text");
    }
```

### TXScopeNRPStringSection, TXScopeNRPStringConstSection, TNRPStringSection, TNRPStringConstSection

`TNRPStringSection<>` is just a version of [`TStringSection<>`](#txscopestringsection-txscopestringconstsection-tstringsection-tstringconstsection) that, for enhanced safety, does not support construction from unsafe raw pointer iterators or (unsafe) `std::string` iterators. Use the `make_nrp_string_section()` functions to create them.

### string_view

`std::string_view` is, in a way, a problematic addition to the standard library in the sense that it has an intrinsically unsafe interface. That is, its constructors support only (unsafe) raw pointer iterator parameters. In contrast, the standard library generally uses iterator types which allow for the option of a memory safe implementation. So to enable memory safe use, this library's version, `mstd::string_view`, generalizes the interface to support construction from safe iterator types. So while technically `mstd::string_view` can act as a drop-in replacement for `std::string_view`, it is designed to be used with safe iterator types, not unsafe raw pointer iterators.

Like `std::string_view`, `mstd::string_view` is defined as an alias for `mstd::basic_string_view<char>`. The `mstd::wstring_view`, `mstd::u16string_view` and `mstd::u32string_view` aliases are also present. Note that `mstd::basic_string_view<>` is in fact just a slightly augmented version of [`TAnyStringConstSection<>`](#txscopeanystringsection-txscopeanystringconstsection-tanystringsection-tanystringconstsection).

usage example:

```cpp
    #include "msepoly.h" // mstd::string_view is defined in this header
    #include "msemstdstring.h"
    
    void main(int argc, char* argv[]) {
    
        /* std::string_view stores an (unsafe) pointer iterator into its target string. mse::mstd::string_view can
        instead store any type of string iterator, including memory safe iterators. So for example, when assigned
        from an mse::mstd::string, mse::mstd::string_view will hold one of mse::mstd::string's safe (strong) iterators
        (obtained with a call to the string's cbegin() member function). Consequently, the mse::mstd::string_view will
        be safe against "use-after-free" bugs to which std::string_view is so prone. */
    
        mse::mstd::string_view msv1;
        {
            mse::mstd::string mstring1("some text");
            msv1 = mstring1;
        }
        try {
            /* This is not undefined (or unsafe) behavior. */
            auto ch1 = msv1[3]; /* In debug mode this will fail an assert. In non-debug mode it'll just work (safely). */
            assert('e' == ch1);
        }
        catch (...) {
            /* At present, no exception will be thrown. Instead, the lifespan of the string data is extended to match
            that of the mstd::string_view. It's possible that in future library implementations, an exception may be thrown. */
        }
    
        mse::mstd::string mstring2("some other text");
        /* With std::string_view, you specify a string subrange with a raw pointer iterator and a length. With
        mse::mstd::string_view you are not restricted to (unsafe) raw pointer iterators. You can use memory safe
        iterators like those provided by mse::mstd::string. */
        auto msv2 = mse::mstd::string_view(mstring2.cbegin()+5, 7);
        assert(msv2 == "other t");
    }
```

See also [TXScopeCSSSXSTEStringSection](https://github.com/duneroadrunner/SaferCPlusPlus/blob/master/README.md#txscopecsssxstestringsection-txscopecsssxstenrpstringsection).

### nrp_string_view

`mse::nrp_string_view` is just a version of [`mse::mstd::string_view`](#string_view) that, for enhanced safety, does not support construction from unsafe raw pointer iterators or (unsafe) `std::string` iterators.

### TXScopeAnyRandomAccessIterator, TXScopeAnyRandomAccessConstIterator, TAnyRandomAccessIterator, TAnyRandomAccessConstIterator

In modern C++ (and SaferCPlusPlus), arrays of different sizes are actually different types, with incompatible iterators. So, for example, if you wanted to make a function that accepts the iterators of arrays of varying size, you would generally do that by "templatizing" the function. Alternatively, you could use an "any random access iterator" which is a "chameleon" (type-erased) iterator that can be constructed from basically any iterator that supports `operator[]` (the "square bracket" operator).

### TXScopeAnyRandomAccessSection, TXScopeAnyRandomAccessConstSection, TAnyRandomAccessSection, TAnyRandomAccessConstSection

`TAnyRandomAccessSection<_Ty>` is essentially just an alias for `TRandomAccessSection<TAnyRandomAccessIterator<_Ty> >`. Analogous to [`TAnyRandomAccessIterator<>`](#txscopeanyrandomaccessiterator-txscopeanyrandomaccessconstiterator-tanyrandomaccessiterator-tanyrandomaccessconstiterator), it can be used to enable a function to accept, as a parameter, any type of "[random access section](#txscoperandomaccesssection-txscoperandomaccessconstsection-trandomaccesssection-trandomaccessconstsection)".

usage example:

```cpp
    #include "msepoly.h"
    
    void main(int argc, char* argv[]) {
        mse::mstd::array<int, 4> array1 { 1, 2, 3, 4 };
        mse::mstd::array<int, 5> array2 { 5, 6, 7, 8, 9 };
        mse::mstd::vector<int> vec1 { 10, 11, 12, 13, 14 };
        class B {
        public:
            static void foo1(mse::TXScopeAnyRandomAccessIterator<int> ra_iter1) {
                ra_iter1[1] = 15;
            }
            static int foo2(mse::TXScopeAnyRandomAccessConstIterator<int> const_ra_iter1) {
                const_ra_iter1 += 2;
                --const_ra_iter1;
                const_ra_iter1--;
                return const_ra_iter1[2];
            }
            static void foo3(mse::TXScopeAnyRandomAccessSection<int> ra_section) {
                for (mse::TXScopeAnyRandomAccessSection<int>::size_type i = 0; i < ra_section.size(); i += 1) {
                    ra_section[i] = 0;
                }
            }
            static int foo4(mse::TXScopeAnyRandomAccessConstSection<int> const_ra_section) {
                int retval = 0;
                for (mse::TXScopeAnyRandomAccessSection<int>::size_type i = 0; i < const_ra_section.size(); i += 1) {
                    retval += const_ra_section[i];
                }
                return retval;
            }
            static int foo5(mse::TXScopeAnyRandomAccessConstSection<int> const_ra_section) {
                int retval = 0;
                for (const auto& const_item : const_ra_section) {
                    retval += const_item;
                }
                return retval;
            }
        };
    
        auto array_iter1 = array1.begin();
        array_iter1++;
        auto res1 = B::foo2(array_iter1);
        B::foo1(array_iter1);
    
        auto array_const_iter2 = array2.cbegin();
        array_const_iter2 += 2;
        auto res2 = B::foo2(array_const_iter2);
    
        auto res3 = B::foo2(vec1.cbegin());
        B::foo1(++vec1.begin());
        auto res4 = B::foo2(vec1.begin());
    
        mse::TXScopeAnyRandomAccessSection<int> ra_section1(array_iter1, 2);
        B::foo3(ra_section1);
    
        mse::TXScopeAnyRandomAccessSection<int> ra_section2(++vec1.begin(), 3);
        auto res5 = B::foo5(ra_section2);
        B::foo3(ra_section2);
        auto res6 = B::foo4(ra_section2);
    }
```

### TXScopeAnyStringSection, TXScopeAnyStringConstSection, TAnyStringSection, TAnyStringConstSection

`TAnyStringSection<_Ty>` is essentially just an alias for `TStringSection<TAnyRandomAccessIterator<_Ty> >`. Like [`TAnyRandomAccessSection<_Ty>`](#txscopeanyrandomaccesssection-txscopeanyrandomaccessconstsection-tanyrandomaccesssection-tanyrandomaccessconstsection), it can be used to enable a function to accept, as a parameter, any type of "[string section](#txscopestringsection-txscopestringconstsection-tstringsection-tstringconstsection)".

usage example:

```cpp
    #include "msepoly.h" // TAnyStringSection<> is defined in this header
    #include "msemstdstring.h"
    #include "msemsestring.h"
    #include "mseregistered.h"
    
    void main(int argc, char* argv[]) {
    
        /* "Any" string sections are basically polymorphic string sections that can hold the value of any string
        section type. They can be used as function parameter types to enable functions to accept any type of string
        section. */
    
        mse::mstd::string mstring1("some text");
        mse::TAnyStringSection<char> any_string_section1(mstring1.begin()+5, 3);
    
        auto string_literal = "some text";
        mse::TAnyStringConstSection<char> any_string_const_section2(string_literal+5, 3);
    
        typedef mse::TRegisteredObj<mse::nii_string> reg_nii_string_t;
        reg_nii_string_t reg_nii_string3("some other text");
        /* This is a different type of (safe) iterator to a different type of string. */
        auto iter = reg_nii_string_t::ss_begin(&reg_nii_string3);
    
        /* Resulting in a different type of string section. */
        auto string_section3 = mse::make_string_section(iter+11, 3);
    
        mse::TAnyStringSection<char> any_string_section3 = string_section3;
        assert(any_string_section1 == any_string_section3);
        assert(any_string_section1.front() == 't');
        assert(any_string_section1.back() == 'x');
        any_string_section1 = string_section3;
        any_string_section1[1] = 'E';
    }
```

### TXScopeAnyNRPStringSection, TXScopeAnyNRPStringConstSection, TAnyNRPStringSection, TAnyNRPStringConstSection

`TAnyNRPStringSection<>` is just a version of [`TAnyStringSection<>`](#txscopeanystringsection-txscopeanystringconstsection-tanystringsection-tanystringconstsection) that, for enhanced safety, doesn't support construction from unsafe raw pointer iterators or (unsafe) `std::string` iterators.

### TXScopeCSSSXSTERandomAccessIterator and TXScopeCSSSXSTERandomAccessSection

`TXScopeCSSSXSTERandomAccessIterator<>` and `TXScopeCSSSXSTERandomAccessSection<>` are "type-erased" template classes that function much like [`TXScopeAnyRandomAccessIterator<>`](#txscopeanyrandomaccessiterator-txscopeanyrandomaccessconstiterator-tanyrandomaccessiterator-tanyrandomaccessconstiterator) and [`TXScopeAnyRandomAccessSection<>`](#txscopeanyrandomaccesssection-txscopeanyrandomaccessconstsection-tanyrandomaccesssection-tanyrandomaccessconstsection) in that they can be used to enable functions to take as arguments iterators or sections of various container types (like an arrays or vectors) without making the functions in to template functions. But in this case there are limitations on what types can be converted. In exchange for these limitations, these types require less overhead. The "CSSSXSTE" part of the typenames stands for "Contiguous Sequence, Static Structure, XScope, Type-Erased". So the first restriction is that the target container must be recognized as a "contiguous sequence" (basically an array or vector). It also must be recognized as having a "static structure". This essentially means that the container cannot be resized. Note that while vectors support resizing, the ones in the library can be "structure locked" at run-time to ensure that they are not resized (i.e. have a "static structure") during certain periods. And only the "scope" versions of the iterators and sections are supported.

usage example:

```cpp
    #include "msemsearray.h" //TXScopeCSSSXSTERandomAccessIterator/Section are defined in this header
    #include "msemstdarray.h"
    #include "msemstdvector.h"
    
    void main(int argc, char* argv[]) {
        auto xs_mstd_array1 = mse::make_xscope(mse::mstd::array<int, 4>{ 1, 2, 3, 4 });
        auto xs_msearray2 = mse::make_xscope(mse::us::msearray<int, 5>{ 5, 6, 7, 8, 9 });
        auto xs_mstd_vec1 = mse::make_xscope(mse::mstd::vector<int>{ 10, 11, 12, 13, 14 });
        class B {
        public:
            static void foo1(mse::TXScopeCSSSXSTERandomAccessIterator<int> ra_iter1) {
                ra_iter1[1] = 15;
            }
            static int foo2(mse::TXScopeCSSSXSTERandomAccessConstIterator<int> const_ra_iter1) {
                const_ra_iter1 += 2;
                --const_ra_iter1;
                const_ra_iter1--;
                return const_ra_iter1[2];
            }
            static void foo3(mse::TXScopeCSSSXSTERandomAccessSection<int> ra_section) {
                for (mse::TXScopeCSSSXSTERandomAccessSection<int>::size_type i = 0; i < ra_section.size(); i += 1) {
                    ra_section[i] = 0;
                }
            }
            static int foo4(mse::TXScopeCSSSXSTERandomAccessConstSection<int> const_ra_section) {
                int retval = 0;
                for (mse::TXScopeCSSSXSTERandomAccessSection<int>::size_type i = 0; i < const_ra_section.size(); i += 1) {
                    retval += const_ra_section[i];
                }
                return retval;
            }
            static int foo5(mse::TXScopeCSSSXSTERandomAccessConstSection<int> const_ra_section) {
                int retval = 0;
                for (const auto& const_item : const_ra_section) {
                    retval += const_item;
                }
                return retval;
            }
        };

        auto xs_mstd_array_iter1 = mse::make_xscope_begin_iterator(&xs_mstd_array1);
        xs_mstd_array_iter1++;
        auto res1 = B::foo2(xs_mstd_array_iter1);
        B::foo1(xs_mstd_array_iter1);

        auto xs_msearray_const_iter2 = mse::make_xscope_begin_const_iterator(&xs_msearray2);
        xs_msearray_const_iter2 += 2;
        auto res2 = B::foo2(xs_msearray_const_iter2);

        auto xs_mstd_vec1_citer1 = mse::make_xscope_begin_const_iterator(&xs_mstd_vec1);
        auto res3 = B::foo2(xs_mstd_vec1_citer1);
        /*
        Note that attempting to combine the previous two lines into a single expression like so:
        auto res3b = B::foo2(mse::make_xscope_begin_const_iterator(&xs_mstd_vec1));
        is not supported and would result in a compile error.

        In the case of dynamic containers, like vectors and strings, the (scope) iterators can only be converted to
        TXScopeCSSSXSTERandomAccessIterator<>s if they are lvalues, not temporaries/rvalues. This is because scope iterators
        hold a "structure lock" that prevents the container from being resized. If the scope iterator you're attempting to
        convert from is a temporary (rvalue) one, then the structure lock would be just as temporary and it would not be safe
        to create a TXScopeCSSSXSTERandomAccessIterator<> that outlives the period when the container is "structure locked".
        "Non-resizable" containers, like arrays, do not have the same issue, so this restriction does not apply to their
        (scope) iterators.
        */

        auto xs_mstd_vec1_citer2 = ++mse::make_xscope_begin_iterator(&xs_mstd_vec1);
        B::foo1(xs_mstd_vec1_citer2);
        auto xs_mstd_vec1_iter1 = mse::make_xscope_begin_iterator(&xs_mstd_vec1);
        auto res4 = B::foo2(xs_mstd_vec1_iter1);

        auto xs_mstd_vec1_begin_citer1 = mse::make_xscope_begin_iterator(&xs_mstd_vec1);
        mse::TXScopeCSSSXSTERandomAccessIterator<int> xs_te_iter1 = xs_mstd_vec1_begin_citer1;
        auto xs_mstd_vec1_end_citer1 = mse::make_xscope_end_iterator(&xs_mstd_vec1);
        mse::TXScopeCSSSXSTERandomAccessIterator<int> xs_te_iter2 = xs_mstd_vec1_end_citer1;
        auto res5 = xs_te_iter2 - xs_te_iter1;
        xs_te_iter1 = xs_te_iter2;

        {
            auto std_array1 = mse::make_xscope(std::array<int, 4>{ 1, 2, 3, 4 });
            mse::TXScopeCSSSXSTERandomAccessIterator<int> xs_te_iter1(mse::make_xscope_begin_iterator(&std_array1));
            mse::TXScopeCSSSXSTERandomAccessIterator<int> xs_te_iter2 = mse::make_xscope_end_iterator(&std_array1);
            auto res5 = xs_te_iter2 - xs_te_iter1;
            xs_te_iter1 = xs_te_iter2;
        }

        mse::TXScopeObj<mse::mstd::array<int, 4> > mstd_array3_scbobj = mse::mstd::array<int, 4>({ 1, 2, 3, 4 });
        auto mstd_array_scpiter3 = mse::mstd::make_xscope_begin_iterator(&mstd_array3_scbobj);
        ++mstd_array_scpiter3;
        B::foo1(mstd_array_scpiter3);

        mse::TXScopeCSSSXSTERandomAccessSection<int> xscp_ra_section1(xs_mstd_array_iter1++, 2);
        B::foo3(xscp_ra_section1);

        auto xs_mstd_vec_iter1 = mse::make_xscope_begin_iterator(&xs_mstd_vec1);
        mse::TXScopeCSSSXSTERandomAccessSection<int> xscp_ra_section2(++xs_mstd_vec_iter1, 3);
        /*
        Note that had we instead used the postfix increment operator on the xs_mstd_vec_iter1 argument in the previous line
        like so:
        mse::TXScopeCSSSXSTERandomAccessSection<int> xscp_ra_section2b(xs_mstd_vec_iter1++, 3);
        we would have gotten a compile error. That's because unlike the prefix increment operator, the postfix increment
        operator returns a temporary (rvalue) copy of the iterator, and as noted earlier, in the case of dynamic containers,
        like vectors and strings, conversion/construction from temporary (rvalue) versions of their iterators could be unsafe
        and is not supported.
        */

        auto res6 = B::foo5(xscp_ra_section2);
        B::foo3(xscp_ra_section2);
        auto res7 = B::foo4(xscp_ra_section2);

        auto xscp_ra_section1_xscp_iter1 = xscp_ra_section1.xscope_begin();
        auto xscp_ra_section1_xscp_iter2 = xscp_ra_section1.xscope_end();
        auto res8 = xscp_ra_section1_xscp_iter2 - xscp_ra_section1_xscp_iter1;
        bool res9 = (xscp_ra_section1_xscp_iter1 < xscp_ra_section1_xscp_iter2);

        auto xs_ra_section1 = mse::make_xscope_random_access_section(xs_mstd_array_iter1, 2);
        B::foo3(xs_ra_section1);
        auto xs_ra_const_section2 = mse::make_xscope_random_access_const_section(mse::make_xscope_begin_const_iterator(&xs_mstd_vec1), 2);
        B::foo4(xs_ra_const_section2);
    }
```

### TXScopeCSSSXSTEStringSection, TXScopeCSSSXSTENRPStringSection

`TXScopeCSSSXSTEStringSection<>` is the string specialized version of [`TXScopeCSSSXSTERandomAccessSection<>`](#txscopecsssxsterandomaccessiterator-and-txscopecsssxsterandomaccesssection). Like [`TXScopeAnyStringSection<>`](#txscopeanystringsection-txscopeanystringconstsection-tanystringsection-tanystringconstsection), it can, with lower overhead but more restrictions, be used to enable functions to take as arguments sections of strings. While [`mstd::string_view`](#string_view) (/`TAnyStringConstSection<char>`) closely matches the interface and flexibilty of `std::string_view`, `TXScopeCSSSXSTEStringConstSection<char>` would more closely match its (low) overhead.

usage example:

```cpp
    #include "msemsestring.h" // TXScopeCSSSXSTEStringSection is defined in this header
    #include "msemstdstring.h"
    
    void main(int argc, char* argv[]) {

        auto xs_mstring1 = mse::make_xscope(mse::mstd::string("some text"));
        auto xs_mstring1_iter1 = mse::make_xscope_begin_iterator(&xs_mstring1) + 5;
        mse::TXScopeCSSSXSTEStringSection<char> xs_csssxste_string_section1(xs_mstring1_iter1, 3);
        /* Note when passing a (scope) string iterator to TXScopeCSSSXSTEStringSection<>'s constructor, as with
        TXScopeCSSSXSTERandomAccessSection<>, that iterator must be an lvalue, not a temporary (rvalue), otherwise
        you'll get a compile error.*/

        mse::TXScopeCSSSXSTEStringConstSection<char> xs_csssxste_string_const_section2("some text");

        auto xs_nii_string3 = mse::make_xscope(mse::nii_string("some other text"));
        auto xs_string_section3 = mse::make_xscope_string_section(mse::make_xscope_begin_iterator(&xs_nii_string3) + 11, 3);
        /* Note that xs_string_section3 is just a regular string section, not a TXScopeCSSSXSTEStringSection<>, so we
        can construct it from an rvalue iterator. */

        mse::TXScopeCSSSXSTEStringSection<char> xs_csssxste_string_section3 = xs_string_section3;
        /* As with iterators, only construction from lvalue string sections is supported, not temporary (rvalue) string sections. */

        assert(xs_csssxste_string_section1 == xs_csssxste_string_section3);
        assert(xs_csssxste_string_section1.front() == 't');
        assert(xs_csssxste_string_section1.back() == 'x');
        xs_csssxste_string_section1[1] = 'E';
    }
```

### Optionals

Conceptually, you might think of an `optional<>` as a dynamic container, like a `vector<>`, that supports a maximum of one element. So the library provides a few versions of `optional<>` that roughly correspond to their [`vector<>` counterparts](#vectors). The library introduces "optional element" pointers, which would loosely correspond to iterators.

### mstd::optional

`mstd::optional<>` is essentially just a safe implementation of std::optional<>. 

usage example:

```cpp
    #include "mseoptional.h"
    
    void main(int argc, char* argv[]) {
        auto opt1 = mse::mstd::optional<int>{ 7 };
        assert(opt1.has_value());
        auto val1 = opt1.value();
    }
```

### optional

Analogous to its vector counterpart, [`nii_vector<>`](#nii_vector), `optional<>` is eligible to be shared among asynchronous threads. And like `nii_vector<>`, the preferred way of accessing or referencing the contained element is via [`xscope_borrowing_fixed_optional<>`](#xscope_borrowing_fixed_optional).

usage example: (see the example for [`xscope_borrowing_fixed_optional<>`](#xscope_borrowing_fixed_optional))

### fixed_optional

Analogous to its vector counterpart, [`fixed_nii_vector<>`](#fixed_nii_vector), `fixed_optional<>` is just a version of `optional<>` whose number of contained elements (zero or one) is fixed at construction.

### xscope_fixed_optional

Analogous to its vector counterpart, [`xscope_fixed_nii_vector<>`](#xscope_fixed_nii_vector), `xscope_fixed_optional<>` is just a version of [`fixed_optional<>`](#fixed_optional) that supports scope types as its element type, and as a scope type itself, is subject to the restrictions of scope objects.

### xscope_borrowing_fixed_optional

Analogous to its vector counterpart, [`xscope_borrowing_fixed_nii_vector<>`](#xscope_borrowing_fixed_nii_vector), `xscope_borrowing_fixed_optional<>` is like [`xscope_fixed_optional<>`](#xscope_fixed_optional), except that, at construction, "borrows" the contents of a specified (via scope pointer) existing `optional<>`, then, upon destruction "releases" the (possibly modified) contents back to the original owner. Unlike `xscope_borrowing_fixed_nii_vector<>` though, the contained element, if any, is not (necessarily) moved when "borrowed", but equivalently, the "borrowing" object is granted "exclusive access" to the "lending" object for the duration of the borrow.

usage example:

```cpp
    #include "mseoptional.h"
    #include "msemstdstring.h"
    
    void main(int argc, char* argv[]) {
    
        auto xs_opt1 = mse::make_xscope(mse::make_optional(mse::mstd::string("abc")));
        // which can also be written as
        // auto xs_opt1 = mse::TXScopeObj<mse::optional<mse::mstd::string> >("abc");

        xs_opt1 = {};
        xs_opt1 = mse::mstd::string("def");

        {
            /* Here we obtain an xscope_borrowing_fixed_optional<> that "borrows" the contents of xs_opt1. */
            auto xs_bfopt1 = mse::make_xscope_borrowing_fixed_optional(&xs_opt1);

            /* Note that accessing xs_opt1 is prohibited while xs_bfopt1 exists. This restriction is enforced.*/

            /* Here we obtain a (safe) pointer to the contained element. */
            auto xs_elem_ptr1 = mse::make_xscope_fixed_optional_element_pointer(&xs_bfopt1);

            /* We can also then obtain a scope pointer to the contained element. */
            auto xs_ptr1 = mse::xscope_pointer(xs_elem_ptr1);
            auto val1 = *xs_ptr1;
            *xs_ptr1 = mse::mstd::string("ghi");
        }
        /* After the xscope_borrowing_fixed_optional<> is gone, we can again access our optional<>. */
        xs_opt1.reset();
    }
```

### mt_optional

Analogous to [`mtnii_vector<>`](#mtnii_vector), `mt_optional<>` supports direct access and references to its contained element and is [eligible to be shared](#asynchronously-shared-objects) among threads (when its contained element is eligible to be shared), at a cost of slightly higher run-time overhead. Like their iterator counterparts, scope optional element pointers, while they exist, hold a ["structure lock"](#structure-locking) on their target `optional<>` object which prevents the contained element from being destroyed. As with the vectors, [`xscope_borrowing_fixed_optional<>`](#xscope_borrowing_fixed_optional) (like all the "fixed-size" optionals) can also be shared among threads and are generally preferred when suitable.

usage example:

```cpp
    #include "mseoptional.h"
    #include "msemsestring.h"
    #include "mseasyncshared.h"
    
    void main(int argc, char* argv[]) {
        auto opt1_access_requester = mse::make_asyncsharedv2readwrite<mse::mt_optional<mse::nii_string> >("abc");
        auto elem_ptr1 = mse::make_optional_element_pointer(opt1_access_requester.writelock_ptr());
        auto val1 = *elem_ptr1;
    }
```

### xscope_mt_optional, xscope_st_optional

[`mstd::optional<>`](#mstdoptional) and [`mt_optional<>`](#mt_optional) can, like any other type, be declared as a [scope type](#scope-pointers) (using `mse::make_xscope()` / `mse::TXScopeObj<>`). But they do not support using scope types as their contained element type. It is (intended to be) uncommon to need such capability. But the library does provide a couple of versions that support it. `xscope_mt_optional<>` is eligible to be shared among (scope) threads, while `xscope_st_optional<>` is not. `xscope_mt_optional<>` and `xscope_st_optional<>` are of course themselves scope types and subject to the corresponding usage restrictions.

usage example:

```cpp
    #include "mseoptional.h"
    #include "msemstdstring.h"
    
    void main(int argc, char* argv[]) {
        /* Here we're creating a (string) object of scope type. */
        auto xs_str1 = mse::make_xscope(mse::mstd::string("abc"));

        /* Here we're creating an xscope_st_optional<> object that contains a scope pointer to the (scope) string object.
        mstd::optional<>, for example, would not support this. */
        auto xsopt1 = mse::make_xscope_st_optional(&xs_str1);
        // which can also be written as
        // auto xsopt1 = mse::xscope_st_optional<mse::TXScopeFixedPointer<mse::mstd::string> >(&xs_str1);

        auto val1 = *(xsopt1.value());
    }
```

### Tuples

Tuples present fewer safety issues than containers with iterators or dynamic structure. But the provided tuple types, aside from supporting the library's scope and data race safety mechanisms, also come with safe pointer types that directly target individual tuple elements.

### tuple

`mstd::tuple<>` is essentially a drop-in substitute for `std::tuple<>`. 

usage example:

```cpp
    #include "msetuple.h"
    #include "msemstdstring.h"
    #include "mserefcounting.h"
    
    void main(int argc, char* argv[]) {
        {
            /* You may, on occasion, need a (safe) pointer that directly targets a tuple element. You
            could make the element type a "registered" or "norad" object. Alternatively, you can obtain a safe
            pointer to a contained element from a pointer to the tuple<> object like so: */
            auto tup1_refcptr = mse::make_refcounting<mse::mstd::tuple<double, char, mse::mstd::string> >(mse::mstd::make_tuple(3.8, 'A', mse::mstd::string("Lisa Simpson")));
            auto elem_ptr1 = mse::make_tuple_element_pointer<double>(tup1_refcptr);
            auto val1 = *elem_ptr1;
        }
        
        {
            /* More commonly, the tuple<> object might be declared as a scope object. */
            auto xs_tup1 = mse::make_xscope(mse::mstd::make_tuple(3.8, 'A', mse::mstd::string("Lisa Simpson")));
            // which can also be written as
            // auto xs_tup1 = mse::TXScopeObj<mse::mstd::tuple<double, char, mse::mstd::string> >(3.8, 'A', mse::mstd::string("Lisa Simpson"));

            auto xs_elem_ptr1 = mse::make_xscope_tuple_element_pointer<2>(&xs_tup1);

            /* Here we obtain a "regular" scope pointer to the element from the scope "tuple element pointer". */
            auto xs_ptr1 = mse::xscope_pointer(xs_elem_ptr1);
            auto val1 = *xs_ptr1;
        }
    }
```

### xscope_tuple

[`mstd::tuple<>`](#tuple) can, like any other type, be declared as a [scope type](#scope-pointers) (using `mse::make_xscope()` / `mse::TXScopeObj<>`). But it does not support using scope types as a contained element type. It is (intended to be) uncommon to need such capability. But the library does provide a version, `xscope_tuple<>`, that supports it.

usage example:

```cpp
    #include "msetuple.h"
    #include "msemstdstring.h"
    
    void main(int argc, char* argv[]) {
            
            /* Here we're creating a (string) object of scope type. */
            auto xs_str1 = mse::make_xscope(mse::mstd::string("abc"));
            
            /* Here we're creating an xscope_tuple<> object that contains a scope pointer to the (scope) string object.
            mstd::tuple<>, for example, would not support this. */
            auto xstup1 = mse::make_xscope_tuple(&xs_str1);
            // which can also be written as
            // auto xstup1 = mse::xscope_tuple<mse::TXScopeFixedPointer<mse::mstd::string> >(&xs_str1);
            
            auto val1 = *(std::get<0>(xstup1));
    }
```

### Algorithms

The library's safe iterators work just fine with the standard library algorithms. But some of the algorithms, like `std::for_each()`, take a function object parameter and pass to the function object a (native) reference to an element. If you want to avoid using native references, the library provides versions of some of these algorithms that pass to the function object a (safe) pointer to the element instead of a native reference to the element. 

#### for_each_ptr()

usage example:

```cpp
#include "msescope.h"
#include "msealgorithm.h"
#include "msemstdarray.h"
#include "msemstdvector.h"
    
void main(int argc, char* argv[]) {

    mse::TXScopeObj<mse::nii_array<int, 3> > xscope_na1 = mse::nii_array<int, 3>{ 1, 2, 3 };
    auto xscope_na1_begin_citer = mse::make_xscope_begin_const_iterator(&xscope_na1);
    auto xscope_na1_end_citer = mse::make_xscope_end_const_iterator(&xscope_na1);

    mse::mstd::array<int, 3> ma1{ 1, 2, 3 };

    auto xscope_nv1 = mse::make_xscope(mse::nii_vector<int>{ 1, 2, 3 });

    /* Here we declare a vector protected by an "excluse writer" access control wrapper. */
    auto xscope_ewnv1 = mse::make_xscope(mse::make_xscope_exclusive_writer(mse::nii_vector<int>{ 1, 2, 3 }));

    {
        /*  mse::for_each_ptr() is like std::for_each() but instead of passing, to the given function, a reference
        to each item it passes a (safe) pointer to each item. The actual type of the pointer varies depending on the
        type of the given iterators. */
        typedef mse::for_each_ptr_type<decltype(ma1.begin())> item_ptr_t;
        mse::for_each_ptr(ma1.begin(), ma1.end(), [](item_ptr_t x_ptr) { std::cout << *x_ptr << std::endl; });

        mse::for_each_ptr(xscope_na1_begin_citer, xscope_na1_end_citer, [](const auto x_ptr) { std::cout << *x_ptr << std::endl; });

        /* A "scope range" version is also available that bypasses the use of iterators. As well as often being more
        convenient, it can theoretically be little more performance optimal. */
        typedef mse::xscope_range_for_each_ptr_type<decltype(&xscope_na1)> range_item_ptr_t;
        mse::xscope_range_for_each_ptr(&xscope_na1, [](range_item_ptr_t x_ptr) { std::cout << *x_ptr << std::endl; });

        {
            auto xscope_bfnv1 = mse::make_xscope(mse::make_xscope_borrowing_fixed_nii_vector(&xscope_nv1));
            mse::xscope_range_for_each_ptr(&xscope_bfnv1, [](const auto x_ptr) { std::cout << *x_ptr << std::endl; });
        }
        {
            /* Note that for performance (and safety) reasons, vectors may be "structure locked" for the duration of the loop.
            That is, any attempt to modify the size of the vector during the loop may result in an exception. */
            auto xscope_nv1_begin_iter = mse::make_xscope_begin_iterator(&xscope_nv1);
            auto xscope_nv1_end_iter = mse::make_xscope_end_iterator(&xscope_nv1);
            mse::for_each_ptr(xscope_nv1_begin_iter, xscope_nv1_end_iter, [](const auto x_ptr) { std::cout << *x_ptr << std::endl; });
            mse::xscope_range_for_each_ptr(&xscope_nv1, [](const auto x_ptr) { std::cout << *x_ptr << std::endl; });
        }
        {
            /* A vector protected by an "excluse writer" access control wrapper is ensured to remain unmodified while a
            corresponding "access controlled const pointer" exists. */
            auto xscope_ewnv1_cptr = mse::make_xscope_access_controlled_const_pointer(&xscope_ewnv1);
            /* So looping over its elements is safe and efficient. */
            mse::xscope_range_for_each_ptr(xscope_ewnv1_cptr, [](const auto x_ptr) { std::cout << *x_ptr << std::endl; });
        }
    }
}
```

#### find_if_ptr()

usage example:

```cpp
#include "msescope.h"
#include "msealgorithm.h"
#include "msemstdarray.h"
    
void main(int argc, char* argv[]) {

    mse::TXScopeObj<mse::nii_array<int, 3> > xscope_na1 = mse::nii_array<int, 3>{ 1, 2, 3 };
    auto xscope_na1_begin_citer = mse::make_xscope_begin_const_iterator(&xscope_na1);
    auto xscope_na1_end_citer = mse::make_xscope_end_const_iterator(&xscope_na1);

    mse::mstd::array<int, 3> ma1{ 1, 2, 3 };

    {
        typedef mse::find_if_ptr_type<decltype(xscope_na1_begin_citer)> item_ptr_t;
        auto found_citer1 = mse::find_if_ptr(xscope_na1_begin_citer, xscope_na1_end_citer, [](item_ptr_t x_ptr) { return 2 == *x_ptr; });
        auto res1 = *found_citer1;

        auto found_citer3 = mse::find_if_ptr(ma1.cbegin(), ma1.cend(), [](auto x_ptr) { return 2 == *x_ptr; });

        /* This version returns an optional scope pointer to the found item rather than an iterator. */
        typedef mse::xscope_range_get_ref_if_ptr_type<decltype(&xscope_na1)> range_item_ptr_t;
        auto xscope_optional_xscpptr4 = mse::xscope_range_get_ref_if_ptr(&xscope_na1, [](range_item_ptr_t x_ptr) { return 2 == *x_ptr; });
        auto res4 = xscope_optional_xscpptr4.value();

        /* This version returns a scope pointer to the found item or throws an exception if an appropriate item isn't
        found. */
        auto xscope_pointer5 = mse::xscope_range_get_ref_to_element_known_to_be_present_ptr(&xscope_na1, [](auto x_ptr) { return 2 == *x_ptr; });
        auto res5 = *xscope_pointer5;
    }
}
```

### thread_local

[*provisional*]

While not encouraging the use of `thread_local` variables, the library does provide facilities for their use.

usage example:
```cpp
#include "msescope.h"
#include "msemstdstring.h"
#include "msethreadlocal.h"
#include <iostream>

MSE_DECLARE_THREAD_LOCAL_GLOBAL(mse::mstd::string) tlg_string1 = "some text";

void main(int argc, char* argv[]) {
    {
        auto tlg_ptr1 = &tlg_string1;
        auto xs_tlg_store1 = mse::make_xscope_strong_pointer_store(tlg_ptr1);
        auto xs_ptr1 = xs_tlg_store1.xscope_ptr();
        *xs_ptr1 += "...";
        std::cout << *xs_ptr1 << std::endl;

        MSE_DECLARE_THREAD_LOCAL_CONST(mse::mstd::string) tlc_string2 = "abc";
        auto tlc_ptr2 = &tlc_string2;
        auto xs_tlc_store2 = mse::make_xscope_strong_pointer_store(tlc_ptr2);
        auto xs_cptr2 = xs_tlc_store2.xscope_ptr();
        std::cout << *xs_cptr2 << std::endl;

        class CA {
        public:
            auto foo1() const {
                MSE_DECLARE_THREAD_LOCAL(mse::mstd::string) tl_string = "abc";
                /* mse::return_value() just returns its argument and ensures that it's of a (pointer) type that's safe to return. */
                return mse::return_value(&tl_string);
            }
        };
        auto tl_ptr3 = CA().foo1();
        auto xs_tl_store3 = mse::make_xscope_strong_pointer_store(tl_ptr3);
        auto xs_cptr3 = xs_tl_store3.xscope_ptr();
        *xs_cptr3 += "def";
        std::cout << *xs_cptr3 << std::endl;
    }

}
```

### (Type-erased) function objects

### function

`mstd::function<>` is essentially a drop-in replacement for `std::function<>`. 

usage example:

```cpp
    #include "msefunction.h"
    
    void main(int argc, char* argv[]) {
        /* mstd::function<> is essentially just an implementation of std::function<> that supports the library's scope and
        data race safety mechanisms. */
        mse::mstd::function<int()> function1 = []() { return 3; };
        function1 = []() { return 5; };
        int res1 = function1();
    }
```

### xscope_function

Some function objects, like functors and capture lambdas, can be scope types and would not be supported by [`mstd::function<>`](#function).

usage example:

```cpp
    #include "msefunction.h"
    
    void main(int argc, char* argv[]) {
        /* xscope_function<>s support scope function objects as well. */
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
        xs_function1 = xscope_my_function_obj1;
        int res1 = xs_function1();

        /* Just as structs with scope pointer/reference members need to be declared as such, lambdas that
        capture scope pointer/references must be declared as such. */
        auto xs_lambda1 = mse::rsv::make_xscope_reference_or_pointer_capture_lambda([int1_xsptr]() { return *int1_xsptr; });
        xs_function1 = xs_lambda1;
    }
```

### Practical limitations

In situations where a lifetime checker, or equivalent static analyzer, is not available, the degree of memory safety that can be achieved is a function of the degree to which use of C++'s (memory) unsafe elements is avoided. 

Note that one of C++'s more subtle unsafe elements is the implicit `this` pointer when accessing member variables from member functions. Consider this example:

```cpp
    #include "msescope.h"
    #include "msemstdvector.h"
    
    class CI {
    public:
        template<class safe_vector_pointer_type>
        void foo1(safe_vector_pointer_type vec_ptr) {
            vec_ptr->clear();
    
            /* These next two lines are equivalent and technically unsafe. */
            m_i += 1;
            this->m_i += 1;
        }
    
        int m_i = 0;
    };
    
    void main() {
        mse::TXScopeObj<mse::mstd::vector<CI>> vec1;
        vec1.resize(1);
        auto iter = vec1.begin();
        iter->foo1(&vec1);
    }
```

The above example contains unchecked accesses to deallocated memory via an implicit and explicit `this` pointer. The `this` pointer (implicit or explicit) is a native pointer, and like any other native pointer, is unsafe and can/should be replaced with a safer substitute:

```cpp
    #include "msescope.h"
    #include "msemstdvector.h"
    
    class CI {
    public:
        template<class safe_this_type, class safe_vector_pointer_type>
        static void foo2(safe_this_type safe_this, safe_vector_pointer_type vec_ptr) {
            vec_ptr->clear();
    
            /* The safe_this pointer will catch the attempted invalid memory access. */
            safe_this->m_i += 2;
        }
    
        int m_i = 0;
    };
    
    void main() {
        mse::TXScopeObj<mse::mstd::vector<CI>> vec1;
        vec1.resize(1);
        auto iter = vec1.begin();
        iter->foo2(iter, &vec1);
    }
```

So technically, in situations where a complete lifetime checker is not available, achieving complete memory safety requires passing a safe `this` pointer parameter as an argument to every member function that accesses a member variable. (I.e. Make your member functions `static`. Or "[free](https://www.youtube.com/watch?v=nWJHhtmWYcY)".)

But certain member functions can't be made static. Namely constructors, destructors and member operators. If all the constructors and destructors in the program are compiler-generated defaults (or are otherwise known to be "well behaved") then they would all be perfectly safe. The (theoretical) problem is that user-defined constructors or destructors aren't guaranteed to be "well behaved". Specifically, in conventional C++ they could cause objects to be deleted in the middle of their constructor/destructor(/non-static member function) calls. And not just their own object, but, for example, a misbehaving constructor that is invoked by a parent constructor could cause that parent object to be deleted before its construction is completed.

Fortunately, with the SaferCPlusPlus subset it's not quite so bad. Let's consider the possibility of an object's `this` pointer being invalidated (i.e. the object being destroyed) while in the middle of executing its constructor. In the SaferCPlusPlus subset there are a limited number of circumstances when a constructor is invoked. One is when calling `make_refcounting<>()`. In this case, no direct or indirect reference to the object (other than the `this` pointer itself) is available until after the constructor has finished executing, so there's no opportunity for the object to be destroyed (and the `this` pointer invalidated) before then. Same goes for `registered_new()` and `norad_new()`. 

A more complicated case is when a container, like say, `mstd::vector<>` causes the invocation of child object constructors. Consider this example:

```cpp
#include "msescope.h"
#include "msemstdvector.h"
#include "msemstdstring.h"

class CMisbehaver1 {
public:
	template<typename TVectorPtr>
	CMisbehaver1(TVectorPtr vector_ptr) {
		vector_ptr->clear(); // potentially dangerous behavior in a constructor

		// is the "this" pointer still valid here?

		this->m_string1 = "some text";
	}
	mse::mstd::string m_string1;
};

void main() {
	typedef mse::mstd::vector<CMisbehaver1> misb1_vec_t;
	typedef mse::mstd::vector<misb1_vec_t> misb1_vec_vec_t;
	mse::TXScopeObj<misb1_vec_vec_t> xs_vv1;
	xs_vv1.resize(1);
	xs_vv1[0].emplace_back(&xs_vv1);
}
```

It's a little bit tricky, but the `emplace_back()` call on the last line of the `main()` function is going to cause the constructor of `CMisbehaver1` to be invoked. But in its first line, the constructor seemingly causes its parent container to be deleted, making its own `this` pointer invalid.

But because the parent container (i.e. the thing invoking the constructor) is an element of the library, it has run-time checks to ensure that its destructor is not executed while in the middle of one of its (non-static) member functions, like `emplace_back()` (or a constructor or the destructor). So in this case, rather than execute its destructor in the middle of the `emplace_back()` call, the program will terminate.

Because constructors and destructors of dynamically (i.e. heap) allocated objects can only be invoked via elements of the library, those elements can (and do) ensure that the `this` pointer remains pointing to validly allocated memory for the whole constructor/destructor call in all cases.

The remaining case is when a local variable is declared (on the stack). In this case the `this` pointer is intrinsically guaranteed to point to a validly allocated object, not just for the duration of the constructor, but indeed for the duration of the scope. (Same goes for variables with static or thread_local storage if you're using those.)

Unfortunately, C++ does permit you to reference the object before the completion of its construction. For example, in this declaration

```cpp
    {
        std::vector<size_t> v1 { v1.at(3) };
    }
```

the local variable `v1` is used after it has been allocated, but before its constructor has been executed. It's also an issue inside constructors as in this example:

```cpp
    {
        struct W {
            W(int i) : j(i) {}
            int j;
        };

        struct Y {
            W w1;
            W w2;
            Y() : w1(w2.j), w2(w1.j) {}
        };
    }
```

This kind of bug is probably quite rare and would probably need to be addressed by a static tool, like the lifetime checker, or frankly, the compiler itself.

While we can generally ensure that the `this` pointer remains pointing to a validly allocated object in constructors/destructors, we cannot do the same for native reference parameters. This means that technically, without a lifetime checker, the safety of (user-defined) constructors which take native reference parameters, like copy and move constructors, cannot be ensured.

Also note that explicitly calling `std::move()` (the one in the `<utility>` library, not the one in the `<algorithm>` library) is potentially unsafe, particularly if applied to certain scope objects. Basically, just let the compiler decide when a reference is an rvalue reference.

And also, SaferCPlusPlus does not yet provide safer substitutes for all of the standard library containers, just the ones responsible for the most problems (vector and array). So be careful with your maps, sets, etc. In many cases lists can be replaced with [`ivector<>`](#ivector)s that support list-style iterators, often with a performance benefit.

### Questions and comments
If you have questions or comments you can post in the [discussions section](https://github.com/duneroadrunner/SaferCPlusPlus/discussions).
