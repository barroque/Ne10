/*
 *  Copyright 2011-12 ARM Limited
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 * NE10 Library : headers/unit_test_common.h
 */

#ifndef __UNIT_TEST_COMMON__
#define __UNIT_TEST_COMMON__

// Make sure the following values are defined before including this header file:
// 1- length of the data arrays
//     #define ARRLEN
// 2- number of the operations in a given unit
//     #define OP_COUNT
// 3- number of the different implementations of each of the functions (C, ASM, NEON, ...)
//     #define IMPL_COUNT

#include <stdio.h>
#include <assert.h>
#include <math.h>

#include <stdlib.h>
#include <string.h>

#include "NE10.h"
#include "NE10_random.h"

//detect that it is regression test or smoke test
#if defined REGRESSION_TEST
#  define ITERATION 10000
#else
#  ifdef SMOKE_TEST
#    define ITERATION 10
#  endif
#endif

// length of the test data arrays
// A number that is not divisible by 2 3 and 4 so that all the
//  execution paths are tested; The larger the number the more
//  number of random values are stored in the array and passed
//  into the array as the input stream.
// 2^11 + 3 = 2051, it is not divisible by 2, 3, or 4
#define TEST_ARRLEN          2051
#define TEST_ARRLEN_MATRICES 1051

// The sign bit mask
#define SIGNBIT_MASK  0x7FFFFFFF

// What's the acceptable error between the integer representations of two float values
#define ERROR_MARGIN_SMALL 0x02
#define ERROR_MARGIN_LARGE 0xFF

// What's the acceptable number of warnings in a test
#define ACCEPTABLE_WARNS 12
#define ACCEPTABLE_WARNS_MATRICES 48

extern void FILL_FLOAT_ARRAY( arm_float_t *arr, unsigned int count );

extern void FILL_FLOAT_ARRAY_LIMIT( arm_float_t *arr, unsigned int count );

extern void FILL_FLOAT_ARRAY_LIMIT_GT1( arm_float_t *arr, unsigned int count );

// this function checks whether the difference between two float values is within the acceptable error range
extern int EQUALS_FLOAT( float fa, float fb , unsigned int err );

#endif // __UNIT_TEST_COMMON

