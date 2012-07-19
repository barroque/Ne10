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
#  define TEST_ITERATION 2048
#else
#  ifdef SMOKE_TEST
#    define TEST_ITERATION 11
#  endif
#endif

//detect that it is performance test
#if defined PERFORMANCE_TEST
#  define PERF_TEST_ITERATION 1024
#endif

// length of the test data arrays
// A number that is not divisible by 2 3 and 4 so that all the
//  execution paths are tested; The larger the number the more
//  number of random values are stored in the array and passed
//  into the array as the input stream.
// 2^11 + 3 = 2051, it is not divisible by 2, 3, or 4
//#define TEST_ARRLEN          2051
//#define TEST_ARRLEN_MATRICES 1051

#define ARRAY_GUARD_LEN      4
#define MAX_FUNC_COUNT       8 //C and NEON version with 4 different data type, 

// The sign bit mask
#define SIGNBIT_MASK  0x7FFFFFFF

// What's the acceptable error between the integer representations of two float values
#define ERROR_MARGIN_SMALL 0x0A
#define ERROR_MARGIN_LARGE 0xFF

// What's the acceptable number of warnings in a test
#define ACCEPTABLE_WARNS 12
#define ACCEPTABLE_WARNS_MATRICES 48

#define NE10_SRC_ALLOC(src, guarded_src, length) { \
        (guarded_src) = (arm_float_t*) calloc (2*ARRAY_GUARD_LEN + (length), sizeof(arm_float_t)); \
        if ((guarded_src) == NULL) \
            printf ("error: calloc src failed\n"); \
        (src) = (guarded_src) + ARRAY_GUARD_LEN; \
        FILL_FLOAT_ARRAY((src), (length)); \
    }

#define NE10_SRC_ALLOC_LIMIT(src, guarded_src, length) { \
        (guarded_src) = (arm_float_t*) calloc (2*ARRAY_GUARD_LEN + (length), sizeof(arm_float_t)); \
        if ((guarded_src) == NULL) \
            printf ("error: calloc src failed\n"); \
        (src) = (guarded_src) + ARRAY_GUARD_LEN; \
        FILL_FLOAT_ARRAY_LIMIT((src), (length)); \
    }

#define NE10_DST_ALLOC(dst, guarded_dst, length) { \
        (guarded_dst) = (arm_float_t*) calloc (2*ARRAY_GUARD_LEN + (length), sizeof(arm_float_t)); \
        if ((guarded_dst) == NULL) \
            printf ("error: calloc dst failed\n"); \
        (dst) = (guarded_dst) + ARRAY_GUARD_LEN; \
    }

#define GET_TIME(time, code) { \
        (time) = GetTickCount(); \
        code \
        time = GetTickCount() - time;\
    }
typedef arm_result_t (*arm_func_5args_t) (void * dst, void * acc, void * src1, void * src2, unsigned int count);
typedef arm_result_t (*arm_func_4args_t) (void * dst, void * src1, void * src2, unsigned int count);
typedef arm_result_t (*arm_func_3args_t) (void * dst, void * src, unsigned int count);


extern void FILL_FLOAT_ARRAY( arm_float_t *arr, unsigned int count );
extern void FILL_FLOAT_ARRAY_LIMIT( arm_float_t *arr, unsigned int count );
extern void FILL_FLOAT_ARRAY_LIMIT_GT1( arm_float_t *arr, unsigned int count );

// this function checks whether the difference between two float values is within the acceptable error range
extern int EQUALS_FLOAT( float fa, float fb , unsigned int err );
extern int GUARD_ARRAY( arm_float_t* array, unsigned int array_length );
extern int CHECK_ARRAY_GUARD( arm_float_t* array, unsigned int array_length );

#endif // __UNIT_TEST_COMMON

