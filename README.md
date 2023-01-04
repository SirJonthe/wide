# wide
## Copyright
Copyright Jonathan Karlsson, 2023

## About
`wide` is a small, standalone, header-only library used to help guide the compiler to emit data parallel processing instructions using SIMD in C++.

## Definitions
Normally, code written in essentially any programming language is written in 'scalar', or 'serial', mode. This means that each basic data type refers to only one piece of data (compound types can still have individual fields accessed individually, which in turn only refer to one piece of data). This offers the most flexibility for programming, but each operation comes at a performance cost. Consider the following pseudo-code:

```
A = 1
B = 2
C = 0
if A < B:
	C = A + B
else:
	C = A - B
```
In the case above, `C` is expected to have a value of `3` since `A` is less than `B`, and therefore `C` is set to `A+B`, i.e. `1+2 = 3`.

Many CPU architectures also provide a set of accelerated instructions that address types in a 'parallel', or 'wide', mode. Each operation using wide types as an operand generally come at the cost of only one corresponding serial operation. Imagine that a basic data type now instead may refer to a field of serial data types, much like an array. However, elements in this array can never be accessed directly. Each operation performed on the array itself propagates this operation element-wise. Consider the following pseudo-code:

```
A = [1, 2, 3, 4]
B = [4, 3, 2, 1]
C = [0, 0, 0, 0]
if A < B:
	C = A + B
else:
	C = A - B
```
In the case above, `C` is expected to have a value of `[5, 5, 1, 3]`. This result happens because conditions operate on operands as if they are a single value. However, operands clearly alias several values. This means that only part of the final value will be influenced by one branch while another part will be influenced by the other. For elements #1 and #2, `A` is less than `B`, and therefore elements #1 and #2 in `C` is set to `A[0]+B[0], A[1]+B[1]`, i.e. `1+4, 2+3 = 5, 5`. For elements #3 and #4, `A` is greater or equal to `B`, and therefore elements #3 and #4 in `C` is set to `A[2]+B[2], A[3]+B[3]`, i.e. `3-2, 4-1 = 1, 3`.

Wide instructions are generally referred to as SIMD - Single Input, Multiple Data. Each processor architecture comes with their own implementation of SIMD. x86 and x64 have several implementations with different features; MMX, SSE, and AVX are the most common. ARM uses NEON. PowerPC uses Altivec.

## Usage
Since SIMD instructions can operate on several values at the cost of a single operation, SIMD theoretically provides a multiple boost in performance. For instance, if the underlying hardware supports SIMD operations on 4 32-bit integers, then one can expect a 4x performance boost in 32-bit integer operations over scalar code (however, in practice, the performance benefits of SIMD is a bit more complex).

Since this performance boost requires relatively deep knowledge of the underlying hardware and code designed specifically to run on architecture supporting the target SIMD instructions, attaining the performance boost provided by SIMD has traditionally code at the cost of developer time and code portability. Over time compilers have become better at detecting instances where SIMD instructions can be safely emitted so some of these issues have lessened, but not mitigated.

While `wide` does not implement SIMD instructions directly in the form of assembly or intrinsics, the library is designed to help the compiler make the right choises regarding SIMD optimization. As such, the code remains much more readable than if the developer were to use the compiler-provided instrinsics or direct use of assembly language, making it much more portable across compilers and architectures. Even when the compiler fails to optimize the code the code is still prepped and ready for data parallel processing which comes with other benefits such as lending itself well for 'embarrassingly parallel' issues.

## Design
The library is designed to help utilize an array-of-structures-of-arrays (AoSoA) design by grouping a number of values behind a single alias. These grouped values, referred to as 'wide' types, support operations such as if they were a single value, similar to this pseudo-code:

```
A = [1, 2, 3, 4]
B = [4, 5, 6, 7]
C = A + B * B    # Result: [ 17, 27, 39, 53 ]
```

Each data type provided by this library overload basic operators so as to mimic the use of built-in types as closely as feasable.

The library does not attempt to know what underlying hardware capabilities the developer is targeting. Therefore, the developer needs to meet the library half-way by specifying the number of serial data types per wide type and a bit depth. For instance, SSE and NEON supports operating on four 32-bit values. Failure to align these requirements with the underlying hardware capabilities may result in the compiler failing to emit SIMD instructions.

## Classes
The main staples are `wide_int`, `wide_uint`, and `wide_float`, which represent different types of numeric types. `wide_bool` represents boolean values. Each type overload basic operators so as to mimic the use of built-in types as closely as feasable.

## Functions
`wide` mainly hides operations behind overloaded operators for basic wide data types. However, since wide data types do not directly support branching code paths in a way that modern programming languages support, some additional math functions are provided to help with common computing tasks, such as `sqrt`, `sin`, `floor`, etc.

## Macros
While wide data types do not directly support branching code paths in a way that modern programming langauges support, `wide` provides macros to make such statements easier to use, such as `WIDE_IF`, `WIDE_ELSE`, `WIDE_WHILE`, and `WIDE_DOWHILE`. In order to use these macros successfully, a `mask` boolean variable needs to be defined in the first scope of the function being run (see Examples > Conditionals).

## Building
No special adjustments need to be made to build `wide`. Simply include the relevant headers in your code (`code.cpp` in the example below) and make sure the headers are available in your compiler search paths. Using `g++` as an example, building is no harder than:

`g++ code.cpp`

Some care needs to be taken by the developer to understand the compiler and architecture they are using. With `g++` and x86/x64 as an example, the following settings are recommended when building:

First, it is a good idea to turn on aggressive optimization of the code using the folloing compiler setting:

`-O3`

Secondly, `g++` provides flags to hint the compiler at parallelizing tight loops:

`-ftree-loop-vectorize -ftree-slp-vectorize`

Note, however, that providing the `-O3` option should already provide these flags to the compiler. As such, this step may not be needed.

Third, it is a good idea to provide your comiler with information about the instruction support available for your target platform. Again, using `g++` as an example and a 

`-msse` or `-mavx` depending on capabilities.

Fourth, it is a good idea to hide type information in your code behind settings provided to your compiler. `wide` already gives the developer a conveinient optiopn (although the developer is free to implement their own solution):

`-DWIDE_DEPTH=32 -DWIDE_WIDTH=4` if using SSE, or `-DWIDE_DEPTH=32 -DWIDE_WIDTH=8`.

The last option provides predefined types called `wide::bool_t`, `wide::int_t`, `wide::uint_t`, and `wide::float_t` which aliases your settings.

Read the documentation for your architecture and set up build scripts to properly adjust for your compiler's and architecture's capabilities.

## Limitations
* **Predication**: Several nested code branches in wide mode may degrade performance significantly below a serial implementation. While performance theoretically should only degrade to the point that it equals serial performance, in practice there is a lot of overhead that goes into implementing branches for wide types. For several levels of nested branching paths, developers should consider just doing plain serial coding.

* **Short-circuit conditions**: Normally, serial conditions short-circuit evaluation of terms as soon as the final result of the condition is known. This saves performance, and shapes how code is written. Due to how C++ implements overloading of && (and) and || (or) both the left-hand side and right-hand side of the condition will be evaluated for wide computations. This means that code like `if A < B && func(C)` will execute `func(C)` even though all elements of `A < B` fail the test.

* **Horizontal operations**: `wide` does not support horizontal operations without first converting a wide type into serial types. Horizontal operations are operations that operate on elements within the same wide type, e.g. `A[0] = A[1]+A[2]`, where `A` is a wide type. Note also that horizontal operations may degrade performance significantly as most architectures need to switch instruction set from wide mode to serial mode to make such operations. Some architectures, however, support horizontal operations in wide mode such as summing, getting the maximum or minimum value, or shuffling elements between indices. There is no explicit support for such architectures and operations.

## Examples
### Assigning values to wide types:
```
using namespace wide;

// ...

// Example array.
int32_t array[] = { 1, 2, 3, 4, 5, 6, 7 }

// Using aggregate constructor.
wide_int<32,4> A = wide_int<32,4>::values({1, 2, 3, 4}); // Reads 1, 2, 3, 4 into slots.

// Using pointers.
wide_int<32,4> B = wide_int(array); // Reads 1, 2, 3, 4 into slots.

// Using single values.
wide_int<32,4> C = 12; // Reads 12, 12, 12, 12 into slots.

// Using unsafe memory cast.
wide_int<32,4> *D = wide_cast< wide_int<32,4> >(array); // Probably points to 1, 2, 3, 4 (see memory alignment).
```
Note: The above code uses four 32-bit values. This must be adjusted depending on the developer's needs and underlying hardware's capabilities.

Note: This applies to other wide types than `wide_int`.

Note: Only the `wide_cast` function operates directly on the source data. All other assignment will copy the data from the serial source to the wide destination.

### Conditionals
Simple branch:
```
using namespace wide;

// ...

wide_bool<32,4> mask = true; // 'mask' needs to be available for conditionals. Declare and define either at the function root scope or as an input parameter to the function.

wide_float<32,4> A = wide_float::values({1.0f, 2.0f, 3.0f, 4.0f});
wide_float<32,4> B = wide_float::values({4.0f, 3.0f, 2.0f, 1.0f});
wide_float<32,4> C;
WIDE_IF(A < B) // Holds true for first and second index.
	C = WIDE_SET(A + B); // First and second index will be set to 5.0f and 5.0f respectively.
WIDE_ELSE // Holds true for third and fourth index.
	C = WIDE_SET(A - B); // Third and fourth index will be set to 1.0f and 3.0f respectively.
END_WIDE_IF
```

`while` loop:
```
using namespace wide;

// ...

wide_bool<32,4> mask = true; // 'mask' needs to be available for conditionals. Declare and define either at the function root scope or as an input parameter to the function.

wide_int<32,4> i = 0;
wide_int<32,4> count = wide_int<32,4>::values({1, 2, 3, 4});
WIDE_WHILE(i < count)
	i = WIDE_SET(i + 2)
END_WIDE_WHILE
```
Note: The resulting `i` will be `[2, 2, 4, 4]`.

`do-while` loop:
```
using namespace wide;

// ...

wide_bool<32,4> mask = true; // 'mask' needs to be available for conditionals. Declare and define either at the function root scope or as an input parameter to the function.

wide_int<32,4> i = 0;
wide_int<32,4> count = wide_int<32,4>::values({0, 1, 2, 3});
WIDE_DOWHILE
	i = WIDE_SET(i + 1)
END_WIDE_DOWHILE(i < count)
```
Note: The resulting `i` will be `[1, 2, 3, 4]`.

### Creating a data structure
```
struct Point4
{
	wide_float<32,4> x, y, z:
};
```
Note: The above code represents four points, where all X values are stored in `x`, all Y values stored in `y`, and all Z values stored in `z`.
