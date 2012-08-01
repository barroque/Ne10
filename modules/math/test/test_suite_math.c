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
 * NE10 Library : test_suite_math.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "NE10_math.h"
#include "seatest.h"

//function table
ne10_func_2args_t ftbl_2args[MAX_FUNC_COUNT];
ne10_func_3args_t ftbl_3args[MAX_FUNC_COUNT];
ne10_func_4args_t ftbl_4args[MAX_FUNC_COUNT];
ne10_func_5args_t ftbl_5args[MAX_FUNC_COUNT];


//input and output
ne10_float32_t * guarded_acc = NULL;
ne10_float32_t * guarded_src1 = NULL;
ne10_float32_t * guarded_src2 = NULL;
ne10_float32_t * guarded_cst = NULL;
ne10_float32_t * theacc = NULL;
ne10_float32_t * thesrc1 = NULL;
ne10_float32_t * thesrc2 = NULL;
ne10_float32_t * thecst = NULL;

ne10_float32_t * guarded_dst_c = NULL;
ne10_float32_t * guarded_dst_neon = NULL;
ne10_float32_t * thedst_c = NULL;
ne10_float32_t * thedst_neon = NULL;

#ifdef PERFORMANCE_TEST
ne10_float32_t * perftest_guarded_acc = NULL;
ne10_float32_t * perftest_guarded_src1 = NULL;
ne10_float32_t * perftest_guarded_src2 = NULL;
ne10_float32_t * perftest_guarded_cst = NULL;
ne10_float32_t * perftest_theacc = NULL;
ne10_float32_t * perftest_thesrc1 = NULL;
ne10_float32_t * perftest_thesrc2 = NULL;
ne10_float32_t * perftest_thecst = NULL;

ne10_float32_t * perftest_thedst_c = NULL;
ne10_float32_t * perftest_guarded_dst_c = NULL;
ne10_float32_t * perftest_guarded_dst_neon = NULL;
ne10_float32_t * perftest_thedst_neon = NULL;
ne10_uint32_t perftest_length = 0;

ne10_float64_t time_c = 0;
ne10_float64_t time_neon = 0;
ne10_float64_t time_overhead = 0;
ne10_float32_t time_speedup = 0.0f;
#endif

void test_abs()
{
#define MAX_VEC_COMPONENTS 4
    ne10_int32_t loop;
    ne10_int32_t i;
    ne10_int32_t func_loop;
    ne10_int32_t vec_size;
    ne10_int32_t pos;

    /* init function table */
    memset (ftbl_3args, 0, sizeof (ftbl_3args));
    ftbl_3args[ 0] = (ne10_func_3args_t) abs_float_c;
    ftbl_3args[ 1] = (ne10_func_3args_t) abs_float_neon;
    ftbl_3args[ 2] = (ne10_func_3args_t) abs_vec2f_c;
    ftbl_3args[ 3] = (ne10_func_3args_t) abs_vec2f_neon;
    ftbl_3args[ 4] = (ne10_func_3args_t) abs_vec3f_c;
    ftbl_3args[ 5] = (ne10_func_3args_t) abs_vec3f_neon;
    ftbl_3args[ 6] = (ne10_func_3args_t) abs_vec4f_c;
    ftbl_3args[ 7] = (ne10_func_3args_t) abs_vec4f_neon;

    fprintf (stdout, "----------%30s start\n", __FUNCTION__);

#if defined (SMOKE_TEST)||(REGRESSION_TEST)
    const ne10_uint32_t fixed_length = TEST_ITERATION * MAX_VEC_COMPONENTS;

    /* init src memory */
    NE10_SRC_ALLOC (thesrc1, guarded_src1, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (thedst_c, guarded_dst_c, fixed_length);
    NE10_DST_ALLOC (thedst_neon, guarded_dst_neon, fixed_length);

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        for (loop = 0; loop < TEST_ITERATION; loop++)
        {
            vec_size = func_loop + 1;

            GUARD_ARRAY (thedst_c, loop * vec_size);
            GUARD_ARRAY (thedst_neon, loop * vec_size);

            ftbl_3args[2 * func_loop] (thedst_c, thesrc1, loop);
            ftbl_3args[2 * func_loop + 1] (thedst_neon, thesrc1, loop);

            CHECK_ARRAY_GUARD (thedst_c, loop * vec_size);
            CHECK_ARRAY_GUARD (thedst_neon, loop * vec_size);

            for (pos = 0; pos < loop; pos++)
            {
#ifdef DEBUG_TRACE
                fprintf (stdout, "func: %d loop count: %d position: %d \n", func_loop, loop, pos);
                for (i = 0; i < vec_size; i++)
                {
                    fprintf (stdout, "thesrc->%d: %e [0x%04X] \n", i, thesrc1[pos * vec_size + i], * (ne10_uint32_t*) &thesrc1[pos * vec_size + i]);
                }
#endif
                assert_float_vec_equal (&thedst_c[pos * vec_size], &thedst_neon[pos * vec_size], ERROR_MARGIN_SMALL, vec_size);
            }
        }
    }
    free (guarded_src1);
    free (guarded_dst_c);
    free (guarded_dst_neon);
#endif

#ifdef PERFORMANCE_TEST
    perftest_length = PERF_TEST_ITERATION * MAX_VEC_COMPONENTS;
    /* init src memory */
    NE10_SRC_ALLOC (perftest_thesrc1, perftest_guarded_src1, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (perftest_thedst_c, perftest_guarded_dst_c, perftest_length);
    NE10_DST_ALLOC (perftest_thedst_neon, perftest_guarded_dst_neon, perftest_length);

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        GET_TIME (time_overhead,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++);
                 );
        GET_TIME (time_c,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_3args[2 * func_loop] (perftest_thedst_c, perftest_thesrc1, loop);
                 );
        GET_TIME (time_neon,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_3args[2 * func_loop + 1] (perftest_thedst_neon, perftest_thesrc1, loop);
                 );
        time_speedup = (time_neon - time_overhead) / (time_c - time_overhead);
        fprintf (stdout, "vector %d\n", func_loop + 1);
        fprintf (stdout, "average time speedup  %8f \n", time_speedup);
        //fprintf(stdout, "average time overhead %8d micro seconds\n", time_overhead);
        //fprintf(stdout, "average time C        %8d micro seconds\n", time_c);
        //fprintf(stdout, "average time NEON     %8d micro seconds\n", time_neon);
    }

    free (perftest_guarded_src1);
    free (perftest_guarded_dst_c);
    free (perftest_guarded_dst_neon);
#endif

    fprintf (stdout, "----------%30s end\n", __FUNCTION__);
#undef MAX_VEC_COMPONENTS
}

void test_addc()
{
#define MAX_VEC_COMPONENTS 4
    ne10_int32_t loop;
    ne10_int32_t i;
    ne10_int32_t func_loop;
    ne10_int32_t vec_size;
    ne10_int32_t pos;

    fprintf (stdout, "----------%30s start\n", __FUNCTION__);

    /* init function table */
    memset (ftbl_4args, 0, sizeof (ftbl_4args));
    ftbl_4args[ 0] = (ne10_func_4args_t) addc_float_c;
    ftbl_4args[ 1] = (ne10_func_4args_t) addc_float_neon;
    ftbl_4args[ 2] = (ne10_func_4args_t) addc_vec2f_c;
    ftbl_4args[ 3] = (ne10_func_4args_t) addc_vec2f_neon;
    ftbl_4args[ 4] = (ne10_func_4args_t) addc_vec3f_c;
    ftbl_4args[ 5] = (ne10_func_4args_t) addc_vec3f_neon;
    ftbl_4args[ 6] = (ne10_func_4args_t) addc_vec4f_c;
    ftbl_4args[ 7] = (ne10_func_4args_t) addc_vec4f_neon;

#if defined (SMOKE_TEST)||(REGRESSION_TEST)
    const ne10_uint32_t fixed_length = TEST_ITERATION * MAX_VEC_COMPONENTS;

    /* init src memory */
    NE10_SRC_ALLOC (thesrc1, guarded_src1, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC (thecst, guarded_cst, MAX_VEC_COMPONENTS); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (thedst_c, guarded_dst_c, fixed_length);
    NE10_DST_ALLOC (thedst_neon, guarded_dst_neon, fixed_length);

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        for (loop = 0; loop < TEST_ITERATION; loop++)
        {
            vec_size = func_loop + 1;

            GUARD_ARRAY (thedst_c, loop * vec_size);
            GUARD_ARRAY (thedst_neon, loop * vec_size);

            ftbl_4args[2 * func_loop] (thedst_c, thesrc1, thecst, loop);
            ftbl_4args[2 * func_loop + 1] (thedst_neon, thesrc1, thecst, loop);

            CHECK_ARRAY_GUARD (thedst_c, loop * vec_size);
            CHECK_ARRAY_GUARD (thedst_neon, loop * vec_size);

            for (pos = 0; pos < loop; pos++)
            {
#ifdef DEBUG_TRACE
                fprintf (stdout, "func: %d loop count: %d position: %d \n", func_loop, loop, pos);
                for (i = 0; i < vec_size; i++)
                {
                    fprintf (stdout, "thesrc->%d: %e [0x%04X] \n", i, thesrc1[pos * vec_size + i], * (ne10_uint32_t*) &thesrc1[pos * vec_size + i]);
                    fprintf (stdout, "thecst->%d: %e [0x%04X] \n", i, thecst[i], * (ne10_uint32_t*) &thecst[i]);
                }
#endif
                assert_float_vec_equal (&thedst_c[pos * vec_size], &thedst_neon[pos * vec_size], ERROR_MARGIN_SMALL, vec_size);
            }
        }
    }
    free (guarded_src1);
    free (guarded_cst);
    free (guarded_dst_c);
    free (guarded_dst_neon);
#endif

#ifdef PERFORMANCE_TEST
    perftest_length = PERF_TEST_ITERATION * MAX_VEC_COMPONENTS;
    /* init src memory */
    NE10_SRC_ALLOC (perftest_thesrc1, perftest_guarded_src1, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC (perftest_thecst, perftest_guarded_cst, MAX_VEC_COMPONENTS); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (perftest_thedst_c, perftest_guarded_dst_c, perftest_length);
    NE10_DST_ALLOC (perftest_thedst_neon, perftest_guarded_dst_neon, perftest_length);

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        GET_TIME (time_overhead,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++);
                 );
        GET_TIME (time_c,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop] (perftest_thedst_c, perftest_thesrc1, perftest_thecst, loop);
                 );
        GET_TIME (time_neon,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop + 1] (perftest_thedst_neon, perftest_thesrc1, perftest_thecst, loop);
                 );
        time_speedup = (time_neon - time_overhead) / (time_c - time_overhead);
        fprintf (stdout, "vector %d\n", func_loop + 1);
        fprintf (stdout, "average time speedup  %8f \n", time_speedup);
        //fprintf(stdout, "average time overhead %8d micro seconds\n", time_overhead);
        //fprintf(stdout, "average time C        %8d micro seconds\n", time_c);
        //fprintf(stdout, "average time NEON     %8d micro seconds\n", time_neon);
    }

    free (perftest_guarded_src1);
    free (perftest_guarded_cst);
    free (perftest_guarded_dst_c);
    free (perftest_guarded_dst_neon);
#endif

    fprintf (stdout, "----------%30s end\n", __FUNCTION__);
#undef MAX_VEC_COMPONENTS
}

void test_add()
{
#define MAX_VEC_COMPONENTS 4
    ne10_int32_t loop;
    ne10_int32_t i;
    ne10_int32_t func_loop;
    ne10_int32_t vec_size;
    ne10_int32_t pos;

    fprintf (stdout, "----------%30s start\n", __FUNCTION__);

    /* init function table */
    memset (ftbl_4args, 0, sizeof (ftbl_4args));
    ftbl_4args[ 0] = (ne10_func_4args_t) add_float_c;
    ftbl_4args[ 1] = (ne10_func_4args_t) add_float_neon;
    ftbl_4args[ 2] = (ne10_func_4args_t) add_vec2f_c;
    ftbl_4args[ 3] = (ne10_func_4args_t) add_vec2f_neon;
    ftbl_4args[ 4] = (ne10_func_4args_t) add_vec3f_c;
    ftbl_4args[ 5] = (ne10_func_4args_t) add_vec3f_neon;
    ftbl_4args[ 6] = (ne10_func_4args_t) add_vec4f_c;
    ftbl_4args[ 7] = (ne10_func_4args_t) add_vec4f_neon;

#if defined (SMOKE_TEST)||(REGRESSION_TEST)
    const ne10_uint32_t fixed_length = TEST_ITERATION * MAX_VEC_COMPONENTS;

    /* init src memory */
    NE10_SRC_ALLOC (thesrc1, guarded_src1, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC (thesrc2, guarded_src2, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (thedst_c, guarded_dst_c, fixed_length);
    NE10_DST_ALLOC (thedst_neon, guarded_dst_neon, fixed_length);

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        for (loop = 0; loop < TEST_ITERATION; loop++)
        {
            vec_size = func_loop + 1;

            GUARD_ARRAY (thedst_c, loop * vec_size);
            GUARD_ARRAY (thedst_neon, loop * vec_size);

            ftbl_4args[2 * func_loop] (thedst_c, thesrc1, thesrc2, loop);
            ftbl_4args[2 * func_loop + 1] (thedst_neon, thesrc1, thesrc2, loop);

            CHECK_ARRAY_GUARD (thedst_c, loop * vec_size);
            CHECK_ARRAY_GUARD (thedst_neon, loop * vec_size);

            for (pos = 0; pos < loop; pos++)
            {
#ifdef DEBUG_TRACE
                fprintf (stdout, "func: %d loop count: %d position: %d \n", func_loop, loop, pos);
                for (i = 0; i < vec_size; i++)
                {
                    fprintf (stdout, "thesrc1->%d: %e [0x%04X] \n", i, thesrc1[pos * vec_size + i], * (ne10_uint32_t*) &thesrc1[pos * vec_size + i]);
                    fprintf (stdout, "thesrc2->%d: %e [0x%04X] \n", i, thesrc2[pos * vec_size + i], * (ne10_uint32_t*) &thesrc2[pos * vec_size + i]);
                }
#endif
                assert_float_vec_equal (&thedst_c[pos * vec_size], &thedst_neon[pos * vec_size], ERROR_MARGIN_SMALL, vec_size);
            }
        }
    }
    free (guarded_src1);
    free (guarded_src2);
    free (guarded_dst_c);
    free (guarded_dst_neon);
#endif

#ifdef PERFORMANCE_TEST
    perftest_length = PERF_TEST_ITERATION * MAX_VEC_COMPONENTS;
    /* init src memory */
    NE10_SRC_ALLOC (perftest_thesrc1, perftest_guarded_src1, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC (perftest_thesrc2, perftest_guarded_src2, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (perftest_thedst_c, perftest_guarded_dst_c, perftest_length);
    NE10_DST_ALLOC (perftest_thedst_neon, perftest_guarded_dst_neon, perftest_length);

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        GET_TIME (time_overhead,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++);
                 );
        GET_TIME (time_c,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop] (perftest_thedst_c, perftest_thesrc1, perftest_thesrc2, loop);
                 );
        GET_TIME (time_neon,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop + 1] (perftest_thedst_neon, perftest_thesrc1, perftest_thesrc2, loop);
                 );
        time_speedup = (time_neon - time_overhead) / (time_c - time_overhead);
        fprintf (stdout, "vector %d\n", func_loop + 1);
        fprintf (stdout, "average time speedup  %8f \n", time_speedup);
        //fprintf(stdout, "average time overhead %8d micro seconds\n", time_overhead);
        //fprintf(stdout, "average time C        %8d micro seconds\n", time_c);
        //fprintf(stdout, "average time NEON     %8d micro seconds\n", time_neon);
    }

    free (perftest_guarded_src1);
    free (perftest_guarded_src2);
    free (perftest_guarded_dst_c);
    free (perftest_guarded_dst_neon);
#endif

    fprintf (stdout, "----------%30s end\n", __FUNCTION__);
#undef MAX_VEC_COMPONENTS
}

void test_cross()
{
#define MAX_VEC_COMPONENTS 3
    ne10_int32_t loop;
    ne10_int32_t i;
    ne10_int32_t func_loop;
    ne10_int32_t vec_size;
    ne10_int32_t pos;

    fprintf (stdout, "----------%30s start\n", __FUNCTION__);

    /* init function table */
    memset (ftbl_4args, 0, sizeof (ftbl_4args));
    ftbl_4args[ 4] = (ne10_func_4args_t) cross_vec3f_c;
    ftbl_4args[ 5] = (ne10_func_4args_t) cross_vec3f_neon;

#if defined (SMOKE_TEST)||(REGRESSION_TEST)
    const ne10_uint32_t fixed_length = TEST_ITERATION * MAX_VEC_COMPONENTS;

    /* init src memory */
    NE10_SRC_ALLOC_LIMIT (thesrc1, guarded_src1, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC_LIMIT (thesrc2, guarded_src2, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (thedst_c, guarded_dst_c, fixed_length);
    NE10_DST_ALLOC (thedst_neon, guarded_dst_neon, fixed_length);

    for (func_loop = 2; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        for (loop = 0; loop < TEST_ITERATION; loop++)
        {
            vec_size = func_loop + 1;

            GUARD_ARRAY (thedst_c, loop * vec_size);
            GUARD_ARRAY (thedst_neon, loop * vec_size);

            ftbl_4args[2 * func_loop] (thedst_c, thesrc1, thesrc2, loop);
            ftbl_4args[2 * func_loop + 1] (thedst_neon, thesrc1, thesrc2, loop);

            CHECK_ARRAY_GUARD (thedst_c, loop * vec_size);
            CHECK_ARRAY_GUARD (thedst_neon, loop * vec_size);

            for (pos = 0; pos < loop; pos++)
            {
#ifdef DEBUG_TRACE
                fprintf (stdout, "func: %d loop count: %d position: %d \n", func_loop, loop, pos);
                for (i = 0; i < vec_size; i++)
                {
                    fprintf (stdout, "thesrc1->%d: %e [0x%04X] \n", i, thesrc1[pos * vec_size + i], * (ne10_uint32_t*) &thesrc1[pos * vec_size + i]);
                    fprintf (stdout, "thesrc2->%d: %e [0x%04X] \n", i, thesrc2[pos * vec_size + i], * (ne10_uint32_t*) &thesrc2[pos * vec_size + i]);
                }
#endif
                assert_float_vec_equal (&thedst_c[pos * vec_size], &thedst_neon[pos * vec_size], ERROR_MARGIN_SMALL, vec_size);
            }
        }
    }
    free (guarded_src1);
    free (guarded_src2);
    free (guarded_dst_c);
    free (guarded_dst_neon);
#endif

#ifdef PERFORMANCE_TEST
    perftest_length = PERF_TEST_ITERATION * MAX_VEC_COMPONENTS;
    /* init src memory */
    NE10_SRC_ALLOC_LIMIT (perftest_thesrc1, perftest_guarded_src1, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC_LIMIT (perftest_thesrc2, perftest_guarded_src2, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (perftest_thedst_c, perftest_guarded_dst_c, perftest_length);
    NE10_DST_ALLOC (perftest_thedst_neon, perftest_guarded_dst_neon, perftest_length);

    for (func_loop = 2; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        GET_TIME (time_overhead,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++);
                 );
        GET_TIME (time_c,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop] (perftest_thedst_c, perftest_thesrc1, perftest_thesrc2, loop);
                 );
        GET_TIME (time_neon,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop + 1] (perftest_thedst_neon, perftest_thesrc1, perftest_thesrc2, loop);
                 );
        time_speedup = (time_neon - time_overhead) / (time_c - time_overhead);
        fprintf (stdout, "vector %d\n", func_loop + 1);
        fprintf (stdout, "average time speedup  %8f \n", time_speedup);
        //fprintf(stdout, "average time overhead %8d micro seconds\n", time_overhead);
        //fprintf(stdout, "average time C        %8d micro seconds\n", time_c);
        //fprintf(stdout, "average time NEON     %8d micro seconds\n", time_neon);
    }

    free (perftest_guarded_src1);
    free (perftest_guarded_src2);
    free (perftest_guarded_dst_c);
    free (perftest_guarded_dst_neon);
#endif

    fprintf (stdout, "----------%30s end\n", __FUNCTION__);
#undef MAX_VEC_COMPONENTS
}

void test_divc()
{
#define MAX_VEC_COMPONENTS 4
    ne10_int32_t loop;
    ne10_int32_t i;
    ne10_int32_t func_loop;
    ne10_int32_t vec_size;
    ne10_int32_t pos;

    fprintf (stdout, "----------%30s start\n", __FUNCTION__);

    /* init function table */
    memset (ftbl_4args, 0, sizeof (ftbl_4args));
    ftbl_4args[ 0] = (ne10_func_4args_t) divc_float_c;
    ftbl_4args[ 1] = (ne10_func_4args_t) divc_float_neon;
    ftbl_4args[ 2] = (ne10_func_4args_t) divc_vec2f_c;
    ftbl_4args[ 3] = (ne10_func_4args_t) divc_vec2f_neon;
    ftbl_4args[ 4] = (ne10_func_4args_t) divc_vec3f_c;
    ftbl_4args[ 5] = (ne10_func_4args_t) divc_vec3f_neon;
    ftbl_4args[ 6] = (ne10_func_4args_t) divc_vec4f_c;
    ftbl_4args[ 7] = (ne10_func_4args_t) divc_vec4f_neon;

#if defined (SMOKE_TEST)||(REGRESSION_TEST)
    const ne10_uint32_t fixed_length = TEST_ITERATION * MAX_VEC_COMPONENTS;

    /* init src memory */
    NE10_SRC_ALLOC (thesrc1, guarded_src1, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC (thecst, guarded_cst, MAX_VEC_COMPONENTS); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (thedst_c, guarded_dst_c, fixed_length);
    NE10_DST_ALLOC (thedst_neon, guarded_dst_neon, fixed_length);

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        for (loop = 0; loop < TEST_ITERATION; loop++)
        {
            vec_size = func_loop + 1;

            GUARD_ARRAY (thedst_c, loop * vec_size);
            GUARD_ARRAY (thedst_neon, loop * vec_size);

            ftbl_4args[2 * func_loop] (thedst_c, thesrc1, thecst, loop);
            ftbl_4args[2 * func_loop + 1] (thedst_neon, thesrc1, thecst, loop);

            CHECK_ARRAY_GUARD (thedst_c, loop * vec_size);
            CHECK_ARRAY_GUARD (thedst_neon, loop * vec_size);

            for (pos = 0; pos < loop; pos++)
            {
#ifdef DEBUG_TRACE
                fprintf (stdout, "func: %d loop count: %d position: %d \n", func_loop, loop, pos);
                for (i = 0; i < vec_size; i++)
                {
                    fprintf (stdout, "thesrc->%d: %e [0x%04X] \n", i, thesrc1[pos * vec_size + i], * (ne10_uint32_t*) &thesrc1[pos * vec_size + i]);
                    fprintf (stdout, "thecst->%d: %e [0x%04X] \n", i, thecst[i], * (ne10_uint32_t*) &thecst[i]);
                }
#endif
                assert_float_vec_equal (&thedst_c[pos * vec_size], &thedst_neon[pos * vec_size], ERROR_MARGIN_SMALL, vec_size);
            }
        }
    }
    free (guarded_src1);
    free (guarded_cst);
    free (guarded_dst_c);
    free (guarded_dst_neon);
#endif

#ifdef PERFORMANCE_TEST
    perftest_length = PERF_TEST_ITERATION * MAX_VEC_COMPONENTS;
    /* init src memory */
    NE10_SRC_ALLOC (perftest_thesrc1, perftest_guarded_src1, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC (perftest_thecst, perftest_guarded_cst, MAX_VEC_COMPONENTS); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (perftest_thedst_c, perftest_guarded_dst_c, perftest_length);
    NE10_DST_ALLOC (perftest_thedst_neon, perftest_guarded_dst_neon, perftest_length);

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        GET_TIME (time_overhead,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++);
                 );
        GET_TIME (time_c,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop] (perftest_thedst_c, perftest_thesrc1, perftest_thecst, loop);
                 );
        GET_TIME (time_neon,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop + 1] (perftest_thedst_neon, perftest_thesrc1, perftest_thecst, loop);
                 );
        time_speedup = (time_neon - time_overhead) / (time_c - time_overhead);
        fprintf (stdout, "vector %d\n", func_loop + 1);
        fprintf (stdout, "average time speedup  %8f \n", time_speedup);
        //fprintf(stdout, "average time overhead %8d micro seconds\n", time_overhead);
        //fprintf(stdout, "average time C        %8d micro seconds\n", time_c);
        //fprintf(stdout, "average time NEON     %8d micro seconds\n", time_neon);
    }

    free (perftest_guarded_src1);
    free (perftest_guarded_cst);
    free (perftest_guarded_dst_c);
    free (perftest_guarded_dst_neon);
#endif

    fprintf (stdout, "----------%30s end\n", __FUNCTION__);
#undef MAX_VEC_COMPONENTS
}

void test_div()
{
#define MAX_VEC_COMPONENTS 4
    ne10_int32_t loop;
    ne10_int32_t i;
    ne10_int32_t func_loop;
    ne10_int32_t vec_size;
    ne10_int32_t pos;

    fprintf (stdout, "----------%30s start\n", __FUNCTION__);

    /* init function table */
    memset (ftbl_4args, 0, sizeof (ftbl_4args));
    ftbl_4args[ 0] = (ne10_func_4args_t) div_float_c;
    ftbl_4args[ 1] = (ne10_func_4args_t) div_float_neon;
    ftbl_4args[ 2] = (ne10_func_4args_t) vdiv_vec2f_c;
    ftbl_4args[ 3] = (ne10_func_4args_t) vdiv_vec2f_neon;
    ftbl_4args[ 4] = (ne10_func_4args_t) vdiv_vec3f_c;
    ftbl_4args[ 5] = (ne10_func_4args_t) vdiv_vec3f_neon;
    ftbl_4args[ 6] = (ne10_func_4args_t) vdiv_vec4f_c;
    ftbl_4args[ 7] = (ne10_func_4args_t) vdiv_vec4f_neon;

#if defined (SMOKE_TEST)||(REGRESSION_TEST)
    const ne10_uint32_t fixed_length = TEST_ITERATION * MAX_VEC_COMPONENTS;

    /* init src memory */
    NE10_SRC_ALLOC_LIMIT (thesrc1, guarded_src1, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC_LIMIT (thesrc2, guarded_src2, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (thedst_c, guarded_dst_c, fixed_length);
    NE10_DST_ALLOC (thedst_neon, guarded_dst_neon, fixed_length);

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        for (loop = 0; loop < TEST_ITERATION; loop++)
        {
            vec_size = func_loop + 1;

            GUARD_ARRAY (thedst_c, loop * vec_size);
            GUARD_ARRAY (thedst_neon, loop * vec_size);

            ftbl_4args[2 * func_loop] (thedst_c, thesrc1, thesrc2, loop);
            ftbl_4args[2 * func_loop + 1] (thedst_neon, thesrc1, thesrc2, loop);

            CHECK_ARRAY_GUARD (thedst_c, loop * vec_size);
            CHECK_ARRAY_GUARD (thedst_neon, loop * vec_size);

            for (pos = 0; pos < loop; pos++)
            {
#ifdef DEBUG_TRACE
                fprintf (stdout, "func: %d loop count: %d position: %d \n", func_loop, loop, pos);
                for (i = 0; i < vec_size; i++)
                {
                    fprintf (stdout, "thesrc1->%d: %e [0x%04X] \n", i, thesrc1[pos * vec_size + i], * (ne10_uint32_t*) &thesrc1[pos * vec_size + i]);
                    fprintf (stdout, "thesrc2->%d: %e [0x%04X] \n", i, thesrc2[pos * vec_size + i], * (ne10_uint32_t*) &thesrc2[pos * vec_size + i]);
                }
#endif
                assert_float_vec_equal (&thedst_c[pos * vec_size], &thedst_neon[pos * vec_size], ERROR_MARGIN_LARGE, vec_size);
            }
        }
    }
    free (guarded_src1);
    free (guarded_src2);
    free (guarded_dst_c);
    free (guarded_dst_neon);
#endif

#ifdef PERFORMANCE_TEST
    perftest_length = PERF_TEST_ITERATION * MAX_VEC_COMPONENTS;
    /* init src memory */
    NE10_SRC_ALLOC (perftest_thesrc1, perftest_guarded_src1, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC (perftest_thesrc2, perftest_guarded_src2, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (perftest_thedst_c, perftest_guarded_dst_c, perftest_length);
    NE10_DST_ALLOC (perftest_thedst_neon, perftest_guarded_dst_neon, perftest_length);

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        GET_TIME (time_overhead,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++);
                 );
        GET_TIME (time_c,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop] (perftest_thedst_c, perftest_thesrc1, perftest_thesrc2, loop);
                 );
        GET_TIME (time_neon,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop + 1] (perftest_thedst_neon, perftest_thesrc1, perftest_thesrc2, loop);
                 );
        time_speedup = (time_neon - time_overhead) / (time_c - time_overhead);
        fprintf (stdout, "vector %d\n", func_loop + 1);
        fprintf (stdout, "average time speedup  %8f \n", time_speedup);
        //fprintf(stdout, "average time overhead %8d micro seconds\n", time_overhead);
        //fprintf(stdout, "average time C        %8d micro seconds\n", time_c);
        //fprintf(stdout, "average time NEON     %8d micro seconds\n", time_neon);
    }

    free (perftest_guarded_src1);
    free (perftest_guarded_src2);
    free (perftest_guarded_dst_c);
    free (perftest_guarded_dst_neon);
#endif

    fprintf (stdout, "----------%30s end\n", __FUNCTION__);
#undef MAX_VEC_COMPONENTS
}

void test_dot()
{
#define MAX_VEC_COMPONENTS 4
    ne10_int32_t loop;
    ne10_int32_t i;
    ne10_int32_t func_loop;
    ne10_int32_t vec_size;
    ne10_int32_t pos;

    fprintf (stdout, "----------%30s start\n", __FUNCTION__);

    /* init function table */
    memset (ftbl_4args, 0, sizeof (ftbl_4args));
    ftbl_4args[ 2] = (ne10_func_4args_t) dot_vec2f_c;
    ftbl_4args[ 3] = (ne10_func_4args_t) dot_vec2f_neon;
    ftbl_4args[ 4] = (ne10_func_4args_t) dot_vec3f_c;
    ftbl_4args[ 5] = (ne10_func_4args_t) dot_vec3f_neon;
    ftbl_4args[ 6] = (ne10_func_4args_t) dot_vec4f_c;
    ftbl_4args[ 7] = (ne10_func_4args_t) dot_vec4f_neon;

#if defined (SMOKE_TEST)||(REGRESSION_TEST)
    const ne10_uint32_t fixed_length = TEST_ITERATION * MAX_VEC_COMPONENTS;

    /* init src memory */
    NE10_SRC_ALLOC_LIMIT (thesrc1, guarded_src1, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC_LIMIT (thesrc2, guarded_src2, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (thedst_c, guarded_dst_c, fixed_length);
    NE10_DST_ALLOC (thedst_neon, guarded_dst_neon, fixed_length);

    for (func_loop = 1; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        for (loop = 0; loop < TEST_ITERATION; loop++)
        {
            vec_size = func_loop + 1;

            GUARD_ARRAY (thedst_c, loop);
            GUARD_ARRAY (thedst_neon, loop);

            ftbl_4args[2 * func_loop] (thedst_c, thesrc1, thesrc2, loop);
            ftbl_4args[2 * func_loop + 1] (thedst_neon, thesrc1, thesrc2, loop);

            CHECK_ARRAY_GUARD (thedst_c, loop);
            CHECK_ARRAY_GUARD (thedst_neon, loop);

            for (pos = 0; pos < loop; pos++)
            {
#ifdef DEBUG_TRACE
                fprintf (stdout, "func: %d loop count: %d position: %d \n", func_loop, loop, pos);
                for (i = 0; i < vec_size; i++)
                {
                    fprintf (stdout, "thesrc1->%d: %e [0x%04X] \n", i, thesrc1[pos * vec_size + i], * (ne10_uint32_t*) &thesrc1[pos * vec_size + i]);
                    fprintf (stdout, "thesrc2->%d: %e [0x%04X] \n", i, thesrc2[pos * vec_size + i], * (ne10_uint32_t*) &thesrc2[pos * vec_size + i]);
                }
#endif
                assert_float_vec_equal (&thedst_c[pos], &thedst_neon[pos], ERROR_MARGIN_SMALL, 1);
            }
        }
    }
    free (guarded_src1);
    free (guarded_src2);
    free (guarded_dst_c);
    free (guarded_dst_neon);
#endif

#ifdef PERFORMANCE_TEST
    perftest_length = PERF_TEST_ITERATION * MAX_VEC_COMPONENTS;
    /* init src memory */
    NE10_SRC_ALLOC_LIMIT (perftest_thesrc1, perftest_guarded_src1, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC_LIMIT (perftest_thesrc2, perftest_guarded_src2, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (perftest_thedst_c, perftest_guarded_dst_c, perftest_length);
    NE10_DST_ALLOC (perftest_thedst_neon, perftest_guarded_dst_neon, perftest_length);

    for (func_loop = 1; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        GET_TIME (time_overhead,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++);
                 );
        GET_TIME (time_c,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop] (perftest_thedst_c, perftest_thesrc1, perftest_thesrc2, loop);
                 );
        GET_TIME (time_neon,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop + 1] (perftest_thedst_neon, perftest_thesrc1, perftest_thesrc2, loop);
                 );
        time_speedup = (time_neon - time_overhead) / (time_c - time_overhead);
        fprintf (stdout, "vector %d\n", func_loop + 1);
        fprintf (stdout, "average time speedup  %8f \n", time_speedup);
        //fprintf(stdout, "average time overhead %8d micro seconds\n", time_overhead);
        //fprintf(stdout, "average time C        %8d micro seconds\n", time_c);
        //fprintf(stdout, "average time NEON     %8d micro seconds\n", time_neon);
    }

    free (perftest_guarded_src1);
    free (perftest_guarded_src2);
    free (perftest_guarded_dst_c);
    free (perftest_guarded_dst_neon);
#endif

    fprintf (stdout, "----------%30s end\n", __FUNCTION__);
#undef MAX_VEC_COMPONENTS
}

void test_len()
{
#define MAX_VEC_COMPONENTS 4
    ne10_int32_t loop;
    ne10_int32_t i;
    ne10_int32_t func_loop;
    ne10_int32_t vec_size;
    ne10_int32_t pos;

    /* init function table */
    memset (ftbl_3args, 0, sizeof (ftbl_3args));
    ftbl_3args[ 2] = (ne10_func_3args_t) len_vec2f_c;
    ftbl_3args[ 3] = (ne10_func_3args_t) len_vec2f_neon;
    ftbl_3args[ 4] = (ne10_func_3args_t) len_vec3f_c;
    ftbl_3args[ 5] = (ne10_func_3args_t) len_vec3f_neon;
    ftbl_3args[ 6] = (ne10_func_3args_t) len_vec4f_c;
    ftbl_3args[ 7] = (ne10_func_3args_t) len_vec4f_neon;

    fprintf (stdout, "----------%30s start\n", __FUNCTION__);

#if defined (SMOKE_TEST)||(REGRESSION_TEST)
    const ne10_uint32_t fixed_length = TEST_ITERATION * MAX_VEC_COMPONENTS;

    /* init src memory */
    NE10_SRC_ALLOC_LIMIT (thesrc1, guarded_src1, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (thedst_c, guarded_dst_c, fixed_length);
    NE10_DST_ALLOC (thedst_neon, guarded_dst_neon, fixed_length);

    for (func_loop = 1; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        for (loop = 0; loop < TEST_ITERATION; loop++)
        {
            vec_size = func_loop + 1;

            GUARD_ARRAY (thedst_c, loop);
            GUARD_ARRAY (thedst_neon, loop);

            ftbl_3args[2 * func_loop] (thedst_c, thesrc1, loop);
            ftbl_3args[2 * func_loop + 1] (thedst_neon, thesrc1, loop);

            CHECK_ARRAY_GUARD (thedst_c, loop);
            CHECK_ARRAY_GUARD (thedst_neon, loop);

            for (pos = 0; pos < loop; pos++)
            {
#ifdef DEBUG_TRACE
                fprintf (stdout, "func: %d loop count: %d position: %d \n", func_loop, loop, pos);
                for (i = 0; i < vec_size; i++)
                {
                    fprintf (stdout, "thesrc->%d: %e [0x%04X] \n", i, thesrc1[pos * vec_size + i], * (ne10_uint32_t*) &thesrc1[pos * vec_size + i]);
                }
#endif
                assert_float_vec_equal (&thedst_c[pos * vec_size], &thedst_neon[pos * vec_size], ERROR_MARGIN_LARGE, 1);
            }
        }
    }
    free (guarded_src1);
    free (guarded_dst_c);
    free (guarded_dst_neon);
#endif

#ifdef PERFORMANCE_TEST
    perftest_length = PERF_TEST_ITERATION * MAX_VEC_COMPONENTS;
    /* init src memory */
    NE10_SRC_ALLOC_LIMIT (perftest_thesrc1, perftest_guarded_src1, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (perftest_thedst_c, perftest_guarded_dst_c, perftest_length);
    NE10_DST_ALLOC (perftest_thedst_neon, perftest_guarded_dst_neon, perftest_length);

    for (func_loop = 1; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        GET_TIME (time_overhead,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++);
                 );
        GET_TIME (time_c,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_3args[2 * func_loop] (perftest_thedst_c, perftest_thesrc1, loop);
                 );
        GET_TIME (time_neon,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_3args[2 * func_loop + 1] (perftest_thedst_neon, perftest_thesrc1, loop);
                 );
        time_speedup = (time_neon - time_overhead) / (time_c - time_overhead);
        fprintf (stdout, "vector %d\n", func_loop + 1);
        fprintf (stdout, "average time speedup  %8f \n", time_speedup);
        //fprintf(stdout, "average time overhead %8d micro seconds\n", time_overhead);
        //fprintf(stdout, "average time C        %8d micro seconds\n", time_c);
        //fprintf(stdout, "average time NEON     %8d micro seconds\n", time_neon);
    }

    free (perftest_guarded_src1);
    free (perftest_guarded_dst_c);
    free (perftest_guarded_dst_neon);
#endif

    fprintf (stdout, "----------%30s end\n", __FUNCTION__);
#undef MAX_VEC_COMPONENTS
}

void test_mlac()
{
#define MAX_VEC_COMPONENTS 4
    ne10_int32_t loop;
    ne10_int32_t i;
    ne10_int32_t func_loop;
    ne10_int32_t vec_size;
    ne10_int32_t pos;

    fprintf (stdout, "----------%30s start\n", __FUNCTION__);

    /* init function table */
    memset (ftbl_5args, 0, sizeof (ftbl_5args));
    ftbl_5args[ 0] = (ne10_func_5args_t) mlac_float_c;
    ftbl_5args[ 1] = (ne10_func_5args_t) mlac_float_neon;
    ftbl_5args[ 2] = (ne10_func_5args_t) mlac_vec2f_c;
    ftbl_5args[ 3] = (ne10_func_5args_t) mlac_vec2f_neon;
    ftbl_5args[ 4] = (ne10_func_5args_t) mlac_vec3f_c;
    ftbl_5args[ 5] = (ne10_func_5args_t) mlac_vec3f_neon;
    ftbl_5args[ 6] = (ne10_func_5args_t) mlac_vec4f_c;
    ftbl_5args[ 7] = (ne10_func_5args_t) mlac_vec4f_neon;

#if defined (SMOKE_TEST)||(REGRESSION_TEST)
    const ne10_uint32_t fixed_length = TEST_ITERATION * MAX_VEC_COMPONENTS;

    /* init src memory */
    NE10_SRC_ALLOC_LIMIT (theacc, guarded_acc, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC_LIMIT (thesrc1, guarded_src1, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC_LIMIT (thecst, guarded_cst, MAX_VEC_COMPONENTS); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (thedst_c, guarded_dst_c, fixed_length);
    NE10_DST_ALLOC (thedst_neon, guarded_dst_neon, fixed_length);

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        for (loop = 0; loop < TEST_ITERATION; loop++)
        {
            vec_size = func_loop + 1;

            GUARD_ARRAY (thedst_c, loop * vec_size);
            GUARD_ARRAY (thedst_neon, loop * vec_size);

            ftbl_5args[2 * func_loop] (thedst_c, theacc, thesrc1, thecst, loop);
            ftbl_5args[2 * func_loop + 1] (thedst_neon, theacc, thesrc1, thecst, loop);

            CHECK_ARRAY_GUARD (thedst_c, loop * vec_size);
            CHECK_ARRAY_GUARD (thedst_neon, loop * vec_size);

            for (pos = 0; pos < loop; pos++)
            {
#ifdef DEBUG_TRACE
                fprintf (stdout, "func: %d loop count: %d position: %d \n", func_loop, loop, pos);
                for (i = 0; i < vec_size; i++)
                {
                    fprintf (stdout, "theacc->%d: %f [0x%04X] \n", i, theacc[pos * vec_size + i], * (ne10_uint32_t*) &theacc[pos * vec_size + i]);
                    fprintf (stdout, "thesrc->%d: %f [0x%04X] \n", i, thesrc1[pos * vec_size + i], * (ne10_uint32_t*) &thesrc1[pos * vec_size + i]);
                    fprintf (stdout, "thecst->%d: %f [0x%04X] \n", i, thecst[i], * (ne10_uint32_t*) &thecst[i]);
                }
#endif
                assert_float_vec_equal (&thedst_c[pos * vec_size], &thedst_neon[pos * vec_size], ERROR_MARGIN_SMALL, vec_size);
            }
        }
    }
    free (guarded_acc);
    free (guarded_src1);
    free (guarded_cst);
    free (guarded_dst_c);
    free (guarded_dst_neon);
#endif

#ifdef PERFORMANCE_TEST
    perftest_length = PERF_TEST_ITERATION * MAX_VEC_COMPONENTS;
    /* init src memory */
    NE10_SRC_ALLOC_LIMIT (perftest_theacc, perftest_guarded_acc, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC_LIMIT (perftest_thesrc1, perftest_guarded_src1, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC_LIMIT (perftest_thecst, perftest_guarded_cst, MAX_VEC_COMPONENTS); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (perftest_thedst_c, perftest_guarded_dst_c, perftest_length);
    NE10_DST_ALLOC (perftest_thedst_neon, perftest_guarded_dst_neon, perftest_length);

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        GET_TIME (time_overhead,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++);
                 );
        GET_TIME (time_c,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_5args[2 * func_loop] (perftest_thedst_c, perftest_theacc, perftest_thesrc1, perftest_thecst, loop);
                 );
        GET_TIME (time_neon,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_5args[2 * func_loop + 1] (perftest_thedst_neon, perftest_theacc, perftest_thesrc1, perftest_thecst, loop);
                 );
        time_speedup = (time_neon - time_overhead) / (time_c - time_overhead);
        fprintf (stdout, "vector %d\n", func_loop + 1);
        fprintf (stdout, "average time speedup  %8f \n", time_speedup);
        //fprintf(stdout, "average time overhead %8d micro seconds\n", time_overhead);
        //fprintf(stdout, "average time C        %8d micro seconds\n", time_c);
        //fprintf(stdout, "average time NEON     %8d micro seconds\n", time_neon);
    }

    free (perftest_guarded_acc);
    free (perftest_guarded_src1);
    free (perftest_guarded_cst);
    free (perftest_guarded_dst_c);
    free (perftest_guarded_dst_neon);
#endif

    fprintf (stdout, "----------%30s end\n", __FUNCTION__);
#undef MAX_VEC_COMPONENTS
}

void test_mla()
{
#define MAX_VEC_COMPONENTS 4
    ne10_int32_t loop;
    ne10_int32_t i;
    ne10_int32_t func_loop;
    ne10_int32_t vec_size;
    ne10_int32_t pos;

    fprintf (stdout, "----------%30s start\n", __FUNCTION__);

    /* init function table */
    memset (ftbl_5args, 0, sizeof (ftbl_5args));
    ftbl_5args[ 0] = (ne10_func_5args_t) mla_float_c;
    ftbl_5args[ 1] = (ne10_func_5args_t) mla_float_neon;
    ftbl_5args[ 2] = (ne10_func_5args_t) vmla_vec2f_c;
    ftbl_5args[ 3] = (ne10_func_5args_t) vmla_vec2f_neon;
    ftbl_5args[ 4] = (ne10_func_5args_t) vmla_vec3f_c;
    ftbl_5args[ 5] = (ne10_func_5args_t) vmla_vec3f_neon;
    ftbl_5args[ 6] = (ne10_func_5args_t) vmla_vec4f_c;
    ftbl_5args[ 7] = (ne10_func_5args_t) vmla_vec4f_neon;

#if defined (SMOKE_TEST)||(REGRESSION_TEST)
    const ne10_uint32_t fixed_length = TEST_ITERATION * MAX_VEC_COMPONENTS;

    /* init src memory */
    NE10_SRC_ALLOC_LIMIT (theacc, guarded_acc, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC_LIMIT (thesrc1, guarded_src1, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC_LIMIT (thesrc2, guarded_src2, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (thedst_c, guarded_dst_c, fixed_length);
    NE10_DST_ALLOC (thedst_neon, guarded_dst_neon, fixed_length);

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        for (loop = 0; loop < TEST_ITERATION; loop++)
        {
            vec_size = func_loop + 1;

            GUARD_ARRAY (thedst_c, loop * vec_size);
            GUARD_ARRAY (thedst_neon, loop * vec_size);

            ftbl_5args[2 * func_loop] (thedst_c, theacc, thesrc1, thesrc2, loop);
            ftbl_5args[2 * func_loop + 1] (thedst_neon, theacc, thesrc1, thesrc2, loop);

            CHECK_ARRAY_GUARD (thedst_c, loop * vec_size);
            CHECK_ARRAY_GUARD (thedst_neon, loop * vec_size);

            for (pos = 0; pos < loop; pos++)
            {
#ifdef DEBUG_TRACE
                fprintf (stdout, "func: %d loop count: %d position: %d \n", func_loop, loop, pos);
                for (i = 0; i < vec_size; i++)
                {
                    fprintf (stdout, "theacc->%d: %e [0x%04X] \n", i, theacc[pos * vec_size + i], * (ne10_uint32_t*) &theacc[pos * vec_size + i]);
                    fprintf (stdout, "thesrc1->%d: %e [0x%04X] \n", i, thesrc1[pos * vec_size + i], * (ne10_uint32_t*) &thesrc1[pos * vec_size + i]);
                    fprintf (stdout, "thesrc2->%d: %e [0x%04X] \n", i, thesrc2[pos * vec_size + i], * (ne10_uint32_t*) &thesrc2[pos * vec_size + i]);
                }
#endif
                assert_float_vec_equal (&thedst_c[pos * vec_size], &thedst_neon[pos * vec_size], ERROR_MARGIN_SMALL, vec_size);
            }
        }
    }
    free (guarded_acc);
    free (guarded_src1);
    free (guarded_src2);
    free (guarded_dst_c);
    free (guarded_dst_neon);
#endif

#ifdef PERFORMANCE_TEST
    perftest_length = PERF_TEST_ITERATION * MAX_VEC_COMPONENTS;
    /* init src memory */
    NE10_SRC_ALLOC_LIMIT (perftest_theacc, perftest_guarded_acc, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC_LIMIT (perftest_thesrc1, perftest_guarded_src1, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC_LIMIT (perftest_thesrc2, perftest_guarded_src2, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (perftest_thedst_c, perftest_guarded_dst_c, perftest_length);
    NE10_DST_ALLOC (perftest_thedst_neon, perftest_guarded_dst_neon, perftest_length);

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        GET_TIME (time_overhead,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++);
                 );
        GET_TIME (time_c,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_5args[2 * func_loop] (perftest_thedst_c, perftest_theacc, perftest_thesrc1, perftest_thesrc2, loop);
                 );
        GET_TIME (time_neon,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_5args[2 * func_loop + 1] (perftest_thedst_neon, perftest_theacc, perftest_thesrc1, perftest_thesrc2, loop);
                 );
        time_speedup = (time_neon - time_overhead) / (time_c - time_overhead);
        fprintf (stdout, "vector %d\n", func_loop + 1);
        fprintf (stdout, "average time speedup  %8f \n", time_speedup);
        //fprintf(stdout, "average time overhead %8d micro seconds\n", time_overhead);
        //fprintf(stdout, "average time C        %8d micro seconds\n", time_c);
        //fprintf(stdout, "average time NEON     %8d micro seconds\n", time_neon);
    }

    free (perftest_guarded_acc);
    free (perftest_guarded_src1);
    free (perftest_guarded_src2);
    free (perftest_guarded_dst_c);
    free (perftest_guarded_dst_neon);
#endif

    fprintf (stdout, "----------%30s end\n", __FUNCTION__);
#undef MAX_VEC_COMPONENTS
}

void test_mulc()
{
#define MAX_VEC_COMPONENTS 4
    ne10_int32_t loop;
    ne10_int32_t i;
    ne10_int32_t func_loop;
    ne10_int32_t vec_size;
    ne10_int32_t pos;

    fprintf (stdout, "----------%30s start\n", __FUNCTION__);

    /* init function table */
    memset (ftbl_4args, 0, sizeof (ftbl_4args));
    ftbl_4args[ 0] = (ne10_func_4args_t) mulc_float_c;
    ftbl_4args[ 1] = (ne10_func_4args_t) mulc_float_neon;
    ftbl_4args[ 2] = (ne10_func_4args_t) mulc_vec2f_c;
    ftbl_4args[ 3] = (ne10_func_4args_t) mulc_vec2f_neon;
    ftbl_4args[ 4] = (ne10_func_4args_t) mulc_vec3f_c;
    ftbl_4args[ 5] = (ne10_func_4args_t) mulc_vec3f_neon;
    ftbl_4args[ 6] = (ne10_func_4args_t) mulc_vec4f_c;
    ftbl_4args[ 7] = (ne10_func_4args_t) mulc_vec4f_neon;

#if defined (SMOKE_TEST)||(REGRESSION_TEST)
    const ne10_uint32_t fixed_length = TEST_ITERATION * MAX_VEC_COMPONENTS;

    /* init src memory */
    NE10_SRC_ALLOC_LIMIT (thesrc1, guarded_src1, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC_LIMIT (thecst, guarded_cst, MAX_VEC_COMPONENTS); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (thedst_c, guarded_dst_c, fixed_length);
    NE10_DST_ALLOC (thedst_neon, guarded_dst_neon, fixed_length);

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        for (loop = 0; loop < TEST_ITERATION; loop++)
        {
            vec_size = func_loop + 1;

            GUARD_ARRAY (thedst_c, loop * vec_size);
            GUARD_ARRAY (thedst_neon, loop * vec_size);

            ftbl_4args[2 * func_loop] (thedst_c, thesrc1, thecst, loop);
            ftbl_4args[2 * func_loop + 1] (thedst_neon, thesrc1, thecst, loop);

            CHECK_ARRAY_GUARD (thedst_c, loop * vec_size);
            CHECK_ARRAY_GUARD (thedst_neon, loop * vec_size);

            for (pos = 0; pos < loop; pos++)
            {
#ifdef DEBUG_TRACE
                fprintf (stdout, "func: %d loop count: %d position: %d \n", func_loop, loop, pos);
                for (i = 0; i < vec_size; i++)
                {
                    fprintf (stdout, "thesrc->%d: %e [0x%04X] \n", i, thesrc1[pos * vec_size + i], * (ne10_uint32_t*) &thesrc1[pos * vec_size + i]);
                    fprintf (stdout, "thecst->%d: %e [0x%04X] \n", i, thecst[i], * (ne10_uint32_t*) &thecst[i]);
                }
#endif
                assert_float_vec_equal (&thedst_c[pos * vec_size], &thedst_neon[pos * vec_size], ERROR_MARGIN_SMALL, vec_size);
            }
        }
    }
    free (guarded_src1);
    free (guarded_cst);
    free (guarded_dst_c);
    free (guarded_dst_neon);
#endif

#ifdef PERFORMANCE_TEST
    perftest_length = PERF_TEST_ITERATION * MAX_VEC_COMPONENTS;
    /* init src memory */
    NE10_SRC_ALLOC_LIMIT (perftest_thesrc1, perftest_guarded_src1, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC_LIMIT (perftest_thecst, perftest_guarded_cst, MAX_VEC_COMPONENTS); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (perftest_thedst_c, perftest_guarded_dst_c, perftest_length);
    NE10_DST_ALLOC (perftest_thedst_neon, perftest_guarded_dst_neon, perftest_length);

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        GET_TIME (time_overhead,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++);
                 );
        GET_TIME (time_c,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop] (perftest_thedst_c, perftest_thesrc1, perftest_thecst, loop);
                 );
        GET_TIME (time_neon,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop + 1] (perftest_thedst_neon, perftest_thesrc1, perftest_thecst, loop);
                 );
        time_speedup = (time_neon - time_overhead) / (time_c - time_overhead);
        fprintf (stdout, "vector %d\n", func_loop + 1);
        fprintf (stdout, "average time speedup  %8f \n", time_speedup);
        //fprintf(stdout, "average time overhead %8d micro seconds\n", time_overhead);
        //fprintf(stdout, "average time C        %8d micro seconds\n", time_c);
        //fprintf(stdout, "average time NEON     %8d micro seconds\n", time_neon);
    }

    free (perftest_guarded_src1);
    free (perftest_guarded_cst);
    free (perftest_guarded_dst_c);
    free (perftest_guarded_dst_neon);
#endif

    fprintf (stdout, "----------%30s end\n", __FUNCTION__);
#undef MAX_VEC_COMPONENTS
}

void test_mul()
{
#define MAX_VEC_COMPONENTS 4
    ne10_int32_t loop;
    ne10_int32_t i;
    ne10_int32_t func_loop;
    ne10_int32_t vec_size;
    ne10_int32_t pos;

    fprintf (stdout, "----------%30s start\n", __FUNCTION__);

    /* init function table */
    memset (ftbl_4args, 0, sizeof (ftbl_4args));
    ftbl_4args[ 0] = (ne10_func_4args_t) mul_float_c;
    ftbl_4args[ 1] = (ne10_func_4args_t) mul_float_neon;
    ftbl_4args[ 2] = (ne10_func_4args_t) vmul_vec2f_c;
    ftbl_4args[ 3] = (ne10_func_4args_t) vmul_vec2f_neon;
    ftbl_4args[ 4] = (ne10_func_4args_t) vmul_vec3f_c;
    ftbl_4args[ 5] = (ne10_func_4args_t) vmul_vec3f_neon;
    ftbl_4args[ 6] = (ne10_func_4args_t) vmul_vec4f_c;
    ftbl_4args[ 7] = (ne10_func_4args_t) vmul_vec4f_neon;

#if defined (SMOKE_TEST)||(REGRESSION_TEST)
    const ne10_uint32_t fixed_length = TEST_ITERATION * MAX_VEC_COMPONENTS;

    /* init src memory */
    NE10_SRC_ALLOC_LIMIT (thesrc1, guarded_src1, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC_LIMIT (thesrc2, guarded_src2, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (thedst_c, guarded_dst_c, fixed_length);
    NE10_DST_ALLOC (thedst_neon, guarded_dst_neon, fixed_length);

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        for (loop = 0; loop < TEST_ITERATION; loop++)
        {
            vec_size = func_loop + 1;

            GUARD_ARRAY (thedst_c, loop * vec_size);
            GUARD_ARRAY (thedst_neon, loop * vec_size);

            ftbl_4args[2 * func_loop] (thedst_c, thesrc1, thesrc2, loop);
            ftbl_4args[2 * func_loop + 1] (thedst_neon, thesrc1, thesrc2, loop);

            CHECK_ARRAY_GUARD (thedst_c, loop * vec_size);
            CHECK_ARRAY_GUARD (thedst_neon, loop * vec_size);

            for (pos = 0; pos < loop; pos++)
            {
#ifdef DEBUG_TRACE
                fprintf (stdout, "func: %d loop count: %d position: %d \n", func_loop, loop, pos);
                for (i = 0; i < vec_size; i++)
                {
                    fprintf (stdout, "thesrc1->%d: %e [0x%04X] \n", i, thesrc1[pos * vec_size + i], * (ne10_uint32_t*) &thesrc1[pos * vec_size + i]);
                    fprintf (stdout, "thesrc2->%d: %e [0x%04X] \n", i, thesrc2[pos * vec_size + i], * (ne10_uint32_t*) &thesrc2[pos * vec_size + i]);
                }
#endif
                assert_float_vec_equal (&thedst_c[pos * vec_size], &thedst_neon[pos * vec_size], ERROR_MARGIN_SMALL, vec_size);
            }
        }
    }
    free (guarded_src1);
    free (guarded_src2);
    free (guarded_dst_c);
    free (guarded_dst_neon);
#endif

#ifdef PERFORMANCE_TEST
    perftest_length = PERF_TEST_ITERATION * MAX_VEC_COMPONENTS;
    /* init src memory */
    NE10_SRC_ALLOC_LIMIT (perftest_thesrc1, perftest_guarded_src1, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC_LIMIT (perftest_thesrc2, perftest_guarded_src2, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (perftest_thedst_c, perftest_guarded_dst_c, perftest_length);
    NE10_DST_ALLOC (perftest_thedst_neon, perftest_guarded_dst_neon, perftest_length);

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        GET_TIME (time_overhead,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++);
                 );
        GET_TIME (time_c,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop] (perftest_thedst_c, perftest_thesrc1, perftest_thesrc2, loop);
                 );
        GET_TIME (time_neon,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop + 1] (perftest_thedst_neon, perftest_thesrc1, perftest_thesrc2, loop);
                 );
        time_speedup = (time_neon - time_overhead) / (time_c - time_overhead);
        fprintf (stdout, "vector %d\n", func_loop + 1);
        fprintf (stdout, "average time speedup  %8f \n", time_speedup);
        //fprintf(stdout, "average time overhead %8d micro seconds\n", time_overhead);
        //fprintf(stdout, "average time C        %8d micro seconds\n", time_c);
        //fprintf(stdout, "average time NEON     %8d micro seconds\n", time_neon);
    }

    free (perftest_guarded_src1);
    free (perftest_guarded_src2);
    free (perftest_guarded_dst_c);
    free (perftest_guarded_dst_neon);
#endif

    fprintf (stdout, "----------%30s end\n", __FUNCTION__);
#undef MAX_VEC_COMPONENTS
}

void test_normalize()
{
#define MAX_VEC_COMPONENTS 4
    ne10_int32_t loop;
    ne10_int32_t i;
    ne10_int32_t func_loop;
    ne10_int32_t vec_size;
    ne10_int32_t pos;

    /* init function table */
    memset (ftbl_3args, 0, sizeof (ftbl_3args));
    ftbl_3args[ 2] = (ne10_func_3args_t) normalize_vec2f_c;
    ftbl_3args[ 3] = (ne10_func_3args_t) normalize_vec2f_neon;
    ftbl_3args[ 4] = (ne10_func_3args_t) normalize_vec3f_c;
    ftbl_3args[ 5] = (ne10_func_3args_t) normalize_vec3f_neon;
    ftbl_3args[ 6] = (ne10_func_3args_t) normalize_vec4f_c;
    ftbl_3args[ 7] = (ne10_func_3args_t) normalize_vec4f_neon;

    fprintf (stdout, "----------%30s start\n", __FUNCTION__);

#if defined (SMOKE_TEST)||(REGRESSION_TEST)
    const ne10_uint32_t fixed_length = TEST_ITERATION * MAX_VEC_COMPONENTS;

    /* init src memory */
    NE10_SRC_ALLOC_LIMIT (thesrc1, guarded_src1, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (thedst_c, guarded_dst_c, fixed_length);
    NE10_DST_ALLOC (thedst_neon, guarded_dst_neon, fixed_length);

    for (func_loop = 1; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        for (loop = 0; loop < TEST_ITERATION; loop++)
        {
            vec_size = func_loop + 1;

            GUARD_ARRAY (thedst_c, loop * vec_size);
            GUARD_ARRAY (thedst_neon, loop * vec_size);

            ftbl_3args[2 * func_loop] (thedst_c, thesrc1, loop);
            ftbl_3args[2 * func_loop + 1] (thedst_neon, thesrc1, loop);

            CHECK_ARRAY_GUARD (thedst_c, loop * vec_size);
            CHECK_ARRAY_GUARD (thedst_neon, loop * vec_size);

            for (pos = 0; pos < loop; pos++)
            {
#ifdef DEBUG_TRACE
                fprintf (stdout, "func: %d loop count: %d position: %d \n", func_loop, loop, pos);
                for (i = 0; i < vec_size; i++)
                {
                    fprintf (stdout, "thesrc->%d: %e [0x%04X] \n", i, thesrc1[pos * vec_size + i], * (ne10_uint32_t*) &thesrc1[pos * vec_size + i]);
                }
#endif
                assert_float_vec_equal (&thedst_c[pos * vec_size], &thedst_neon[pos * vec_size], ERROR_MARGIN_LARGE, vec_size);
            }
        }
    }
    free (guarded_src1);
    free (guarded_dst_c);
    free (guarded_dst_neon);
#endif

#ifdef PERFORMANCE_TEST
    perftest_length = PERF_TEST_ITERATION * MAX_VEC_COMPONENTS;
    /* init src memory */
    NE10_SRC_ALLOC_LIMIT (perftest_thesrc1, perftest_guarded_src1, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (perftest_thedst_c, perftest_guarded_dst_c, perftest_length);
    NE10_DST_ALLOC (perftest_thedst_neon, perftest_guarded_dst_neon, perftest_length);

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        GET_TIME (time_overhead,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++);
                 );
        GET_TIME (time_c,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_3args[2 * func_loop] (perftest_thedst_c, perftest_thesrc1, loop);
                 );
        GET_TIME (time_neon,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_3args[2 * func_loop + 1] (perftest_thedst_neon, perftest_thesrc1, loop);
                 );
        time_speedup = (time_neon - time_overhead) / (time_c - time_overhead);
        fprintf (stdout, "vector %d\n", func_loop + 1);
        fprintf (stdout, "average time speedup  %8f \n", time_speedup);
        //fprintf(stdout, "average time overhead %8d micro seconds\n", time_overhead);
        //fprintf(stdout, "average time C        %8d micro seconds\n", time_c);
        //fprintf(stdout, "average time NEON     %8d micro seconds\n", time_neon);
    }

    free (perftest_guarded_src1);
    free (perftest_guarded_dst_c);
    free (perftest_guarded_dst_neon);
#endif

    fprintf (stdout, "----------%30s end\n", __FUNCTION__);
#undef MAX_VEC_COMPONENTS
}

void test_rsbc()
{
#define MAX_VEC_COMPONENTS 4
    ne10_int32_t loop;
    ne10_int32_t i;
    ne10_int32_t func_loop;
    ne10_int32_t vec_size;
    ne10_int32_t pos;

    fprintf (stdout, "----------%30s start\n", __FUNCTION__);

    /* init function table */
    memset (ftbl_4args, 0, sizeof (ftbl_4args));
    ftbl_4args[ 0] = (ne10_func_4args_t) rsbc_float_c;
    ftbl_4args[ 1] = (ne10_func_4args_t) rsbc_float_neon;
    ftbl_4args[ 2] = (ne10_func_4args_t) rsbc_vec2f_c;
    ftbl_4args[ 3] = (ne10_func_4args_t) rsbc_vec2f_neon;
    ftbl_4args[ 4] = (ne10_func_4args_t) rsbc_vec3f_c;
    ftbl_4args[ 5] = (ne10_func_4args_t) rsbc_vec3f_neon;
    ftbl_4args[ 6] = (ne10_func_4args_t) rsbc_vec4f_c;
    ftbl_4args[ 7] = (ne10_func_4args_t) rsbc_vec4f_neon;

#if defined (SMOKE_TEST)||(REGRESSION_TEST)
    const ne10_uint32_t fixed_length = TEST_ITERATION * MAX_VEC_COMPONENTS;

    /* init src memory */
    NE10_SRC_ALLOC (thesrc1, guarded_src1, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC (thecst, guarded_cst, MAX_VEC_COMPONENTS); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (thedst_c, guarded_dst_c, fixed_length);
    NE10_DST_ALLOC (thedst_neon, guarded_dst_neon, fixed_length);

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        for (loop = 0; loop < TEST_ITERATION; loop++)
        {
            vec_size = func_loop + 1;

            GUARD_ARRAY (thedst_c, loop * vec_size);
            GUARD_ARRAY (thedst_neon, loop * vec_size);

            ftbl_4args[2 * func_loop] (thedst_c, thesrc1, thecst, loop);
            ftbl_4args[2 * func_loop + 1] (thedst_neon, thesrc1, thecst, loop);

            CHECK_ARRAY_GUARD (thedst_c, loop * vec_size);
            CHECK_ARRAY_GUARD (thedst_neon, loop * vec_size);

            for (pos = 0; pos < loop; pos++)
            {
#ifdef DEBUG_TRACE
                fprintf (stdout, "func: %d loop count: %d position: %d \n", func_loop, loop, pos);
                for (i = 0; i < vec_size; i++)
                {
                    fprintf (stdout, "thesrc->%d: %e [0x%04X] \n", i, thesrc1[pos * vec_size + i], * (ne10_uint32_t*) &thesrc1[pos * vec_size + i]);
                    fprintf (stdout, "thecst->%d: %e [0x%04X] \n", i, thecst[i], * (ne10_uint32_t*) &thecst[i]);
                }
#endif
                assert_float_vec_equal (&thedst_c[pos * vec_size], &thedst_neon[pos * vec_size], ERROR_MARGIN_SMALL, vec_size);
            }
        }
    }
    free (guarded_src1);
    free (guarded_cst);
    free (guarded_dst_c);
    free (guarded_dst_neon);
#endif

#ifdef PERFORMANCE_TEST
    perftest_length = PERF_TEST_ITERATION * MAX_VEC_COMPONENTS;
    /* init src memory */
    NE10_SRC_ALLOC (perftest_thesrc1, perftest_guarded_src1, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC (perftest_thecst, perftest_guarded_cst, MAX_VEC_COMPONENTS); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (perftest_thedst_c, perftest_guarded_dst_c, perftest_length);
    NE10_DST_ALLOC (perftest_thedst_neon, perftest_guarded_dst_neon, perftest_length);

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        GET_TIME (time_overhead,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++);
                 );
        GET_TIME (time_c,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop] (perftest_thedst_c, perftest_thesrc1, perftest_thecst, loop);
                 );
        GET_TIME (time_neon,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop + 1] (perftest_thedst_neon, perftest_thesrc1, perftest_thecst, loop);
                 );
        time_speedup = (time_neon - time_overhead) / (time_c - time_overhead);
        fprintf (stdout, "vector %d\n", func_loop + 1);
        fprintf (stdout, "average time speedup  %8f \n", time_speedup);
        //fprintf(stdout, "average time overhead %8d micro seconds\n", time_overhead);
        //fprintf(stdout, "average time C        %8d micro seconds\n", time_c);
        //fprintf(stdout, "average time NEON     %8d micro seconds\n", time_neon);
    }

    free (perftest_guarded_src1);
    free (perftest_guarded_cst);
    free (perftest_guarded_dst_c);
    free (perftest_guarded_dst_neon);
#endif

    fprintf (stdout, "----------%30s end\n", __FUNCTION__);
#undef MAX_VEC_COMPONENTS
}

void test_setc()
{
#define MAX_VEC_COMPONENTS 4
    ne10_int32_t loop;
    ne10_int32_t i;
    ne10_int32_t func_loop;
    ne10_int32_t vec_size;
    ne10_int32_t pos;

    fprintf (stdout, "----------%30s start\n", __FUNCTION__);

    /* init function table */
    memset (ftbl_3args, 0, sizeof (ftbl_3args));
    ftbl_3args[ 0] = (ne10_func_3args_t) setc_float_c;
    ftbl_3args[ 1] = (ne10_func_3args_t) setc_float_neon;
    ftbl_3args[ 2] = (ne10_func_3args_t) setc_vec2f_c;
    ftbl_3args[ 3] = (ne10_func_3args_t) setc_vec2f_neon;
    ftbl_3args[ 4] = (ne10_func_3args_t) setc_vec3f_c;
    ftbl_3args[ 5] = (ne10_func_3args_t) setc_vec3f_neon;
    ftbl_3args[ 6] = (ne10_func_3args_t) setc_vec4f_c;
    ftbl_3args[ 7] = (ne10_func_3args_t) setc_vec4f_neon;

#if defined (SMOKE_TEST)||(REGRESSION_TEST)
    const ne10_uint32_t fixed_length = TEST_ITERATION * MAX_VEC_COMPONENTS;

    /* init src memory */
    NE10_SRC_ALLOC (thecst, guarded_cst, MAX_VEC_COMPONENTS); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (thedst_c, guarded_dst_c, fixed_length);
    NE10_DST_ALLOC (thedst_neon, guarded_dst_neon, fixed_length);

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        for (loop = 0; loop < TEST_ITERATION; loop++)
        {
            vec_size = func_loop + 1;

            GUARD_ARRAY (thedst_c, loop * vec_size);
            GUARD_ARRAY (thedst_neon, loop * vec_size);

            ftbl_3args[2 * func_loop] (thedst_c, thecst, loop);
            ftbl_3args[2 * func_loop + 1] (thedst_neon, thecst, loop);

            CHECK_ARRAY_GUARD (thedst_c, loop * vec_size);
            CHECK_ARRAY_GUARD (thedst_neon, loop * vec_size);

            for (pos = 0; pos < loop; pos++)
            {
#ifdef DEBUG_TRACE
                fprintf (stdout, "func: %d loop count: %d position: %d \n", func_loop, loop, pos);
                for (i = 0; i < vec_size; i++)
                {
                    fprintf (stdout, "thesrc->%d: %e [0x%04X] \n", i, thesrc1[pos * vec_size + i], * (ne10_uint32_t*) &thesrc1[pos * vec_size + i]);
                    fprintf (stdout, "thecst->%d: %e [0x%04X] \n", i, thecst[i], * (ne10_uint32_t*) &thecst[i]);
                }
#endif
                assert_float_vec_equal (&thedst_c[pos * vec_size], &thedst_neon[pos * vec_size], ERROR_MARGIN_SMALL, vec_size);
            }
        }
    }
    free (guarded_cst);
    free (guarded_dst_c);
    free (guarded_dst_neon);
#endif

#ifdef PERFORMANCE_TEST
    perftest_length = PERF_TEST_ITERATION * MAX_VEC_COMPONENTS;
    /* init src memory */
    NE10_SRC_ALLOC (perftest_thecst, perftest_guarded_cst, MAX_VEC_COMPONENTS); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (perftest_thedst_c, perftest_guarded_dst_c, perftest_length);
    NE10_DST_ALLOC (perftest_thedst_neon, perftest_guarded_dst_neon, perftest_length);

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        GET_TIME (time_overhead,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++);
                 );
        GET_TIME (time_c,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_3args[2 * func_loop] (perftest_thedst_c, perftest_thecst, loop);
                 );
        GET_TIME (time_neon,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_3args[2 * func_loop + 1] (perftest_thedst_neon, perftest_thecst, loop);
                 );
        time_speedup = (time_neon - time_overhead) / (time_c - time_overhead);
        fprintf (stdout, "vector %d\n", func_loop + 1);
        fprintf (stdout, "average time speedup  %8f \n", time_speedup);
        //fprintf(stdout, "average time overhead %8d micro seconds\n", time_overhead);
        //fprintf(stdout, "average time C        %8d micro seconds\n", time_c);
        //fprintf(stdout, "average time NEON     %8d micro seconds\n", time_neon);
    }

    free (perftest_guarded_cst);
    free (perftest_guarded_dst_c);
    free (perftest_guarded_dst_neon);
#endif

    fprintf (stdout, "----------%30s end\n", __FUNCTION__);
#undef MAX_VEC_COMPONENTS
}

void test_subc()
{
#define MAX_VEC_COMPONENTS 4
    ne10_int32_t loop;
    ne10_int32_t i;
    ne10_int32_t func_loop;
    ne10_int32_t vec_size;
    ne10_int32_t pos;

    fprintf (stdout, "----------%30s start\n", __FUNCTION__);

    /* init function table */
    memset (ftbl_4args, 0, sizeof (ftbl_4args));
    ftbl_4args[ 0] = (ne10_func_4args_t) subc_float_c;
    ftbl_4args[ 1] = (ne10_func_4args_t) subc_float_neon;
    ftbl_4args[ 2] = (ne10_func_4args_t) subc_vec2f_c;
    ftbl_4args[ 3] = (ne10_func_4args_t) subc_vec2f_neon;
    ftbl_4args[ 4] = (ne10_func_4args_t) subc_vec3f_c;
    ftbl_4args[ 5] = (ne10_func_4args_t) subc_vec3f_neon;
    ftbl_4args[ 6] = (ne10_func_4args_t) subc_vec4f_c;
    ftbl_4args[ 7] = (ne10_func_4args_t) subc_vec4f_neon;

#if defined (SMOKE_TEST)||(REGRESSION_TEST)
    const ne10_uint32_t fixed_length = TEST_ITERATION * MAX_VEC_COMPONENTS;

    /* init src memory */
    NE10_SRC_ALLOC (thesrc1, guarded_src1, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC (thecst, guarded_cst, MAX_VEC_COMPONENTS); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (thedst_c, guarded_dst_c, fixed_length);
    NE10_DST_ALLOC (thedst_neon, guarded_dst_neon, fixed_length);

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        for (loop = 0; loop < TEST_ITERATION; loop++)
        {
            vec_size = func_loop + 1;

            GUARD_ARRAY (thedst_c, loop * vec_size);
            GUARD_ARRAY (thedst_neon, loop * vec_size);

            ftbl_4args[2 * func_loop] (thedst_c, thesrc1, thecst, loop);
            ftbl_4args[2 * func_loop + 1] (thedst_neon, thesrc1, thecst, loop);

            CHECK_ARRAY_GUARD (thedst_c, loop * vec_size);
            CHECK_ARRAY_GUARD (thedst_neon, loop * vec_size);

            for (pos = 0; pos < loop; pos++)
            {
#ifdef DEBUG_TRACE
                fprintf (stdout, "func: %d loop count: %d position: %d \n", func_loop, loop, pos);
                for (i = 0; i < vec_size; i++)
                {
                    fprintf (stdout, "thesrc->%d: %e [0x%04X] \n", i, thesrc1[pos * vec_size + i], * (ne10_uint32_t*) &thesrc1[pos * vec_size + i]);
                    fprintf (stdout, "thecst->%d: %e [0x%04X] \n", i, thecst[i], * (ne10_uint32_t*) &thecst[i]);
                }
#endif
                assert_float_vec_equal (&thedst_c[pos * vec_size], &thedst_neon[pos * vec_size], ERROR_MARGIN_SMALL, vec_size);
            }
        }
    }
    free (guarded_src1);
    free (guarded_cst);
    free (guarded_dst_c);
    free (guarded_dst_neon);
#endif

#ifdef PERFORMANCE_TEST
    perftest_length = PERF_TEST_ITERATION * MAX_VEC_COMPONENTS;
    /* init src memory */
    NE10_SRC_ALLOC (perftest_thesrc1, perftest_guarded_src1, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC (perftest_thecst, perftest_guarded_cst, MAX_VEC_COMPONENTS); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (perftest_thedst_c, perftest_guarded_dst_c, perftest_length);
    NE10_DST_ALLOC (perftest_thedst_neon, perftest_guarded_dst_neon, perftest_length);

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        GET_TIME (time_overhead,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++);
                 );
        GET_TIME (time_c,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop] (perftest_thedst_c, perftest_thesrc1, perftest_thecst, loop);
                 );
        GET_TIME (time_neon,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop + 1] (perftest_thedst_neon, perftest_thesrc1, perftest_thecst, loop);
                 );
        time_speedup = (time_neon - time_overhead) / (time_c - time_overhead);
        fprintf (stdout, "vector %d\n", func_loop + 1);
        fprintf (stdout, "average time speedup  %8f \n", time_speedup);
        //fprintf(stdout, "average time overhead %8d micro seconds\n", time_overhead);
        //fprintf(stdout, "average time C        %8d micro seconds\n", time_c);
        //fprintf(stdout, "average time NEON     %8d micro seconds\n", time_neon);
    }

    free (perftest_guarded_src1);
    free (perftest_guarded_cst);
    free (perftest_guarded_dst_c);
    free (perftest_guarded_dst_neon);
#endif

    fprintf (stdout, "----------%30s end\n", __FUNCTION__);
#undef MAX_VEC_COMPONENTS
}

void test_sub()
{
#define MAX_VEC_COMPONENTS 4
    ne10_int32_t loop;
    ne10_int32_t i;
    ne10_int32_t func_loop;
    ne10_int32_t vec_size;
    ne10_int32_t pos;

    fprintf (stdout, "----------%30s start\n", __FUNCTION__);

    /* init function table */
    memset (ftbl_4args, 0, sizeof (ftbl_4args));
    ftbl_4args[ 0] = (ne10_func_4args_t) sub_float_c;
    ftbl_4args[ 1] = (ne10_func_4args_t) sub_float_neon;
    ftbl_4args[ 2] = (ne10_func_4args_t) sub_vec2f_c;
    ftbl_4args[ 3] = (ne10_func_4args_t) sub_vec2f_neon;
    ftbl_4args[ 4] = (ne10_func_4args_t) sub_vec3f_c;
    ftbl_4args[ 5] = (ne10_func_4args_t) sub_vec3f_neon;
    ftbl_4args[ 6] = (ne10_func_4args_t) sub_vec4f_c;
    ftbl_4args[ 7] = (ne10_func_4args_t) sub_vec4f_neon;

#if defined (SMOKE_TEST)||(REGRESSION_TEST)
    const ne10_uint32_t fixed_length = TEST_ITERATION * MAX_VEC_COMPONENTS;

    /* init src memory */
    NE10_SRC_ALLOC (thesrc1, guarded_src1, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC (thesrc2, guarded_src2, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (thedst_c, guarded_dst_c, fixed_length);
    NE10_DST_ALLOC (thedst_neon, guarded_dst_neon, fixed_length);

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        for (loop = 0; loop < TEST_ITERATION; loop++)
        {
            vec_size = func_loop + 1;

            GUARD_ARRAY (thedst_c, loop * vec_size);
            GUARD_ARRAY (thedst_neon, loop * vec_size);

            ftbl_4args[2 * func_loop] (thedst_c, thesrc1, thesrc2, loop);
            ftbl_4args[2 * func_loop + 1] (thedst_neon, thesrc1, thesrc2, loop);

            CHECK_ARRAY_GUARD (thedst_c, loop * vec_size);
            CHECK_ARRAY_GUARD (thedst_neon, loop * vec_size);

            for (pos = 0; pos < loop; pos++)
            {
#ifdef DEBUG_TRACE
                fprintf (stdout, "func: %d loop count: %d position: %d \n", func_loop, loop, pos);
                for (i = 0; i < vec_size; i++)
                {
                    fprintf (stdout, "thesrc1->%d: %e [0x%04X] \n", i, thesrc1[pos * vec_size + i], * (ne10_uint32_t*) &thesrc1[pos * vec_size + i]);
                    fprintf (stdout, "thesrc2->%d: %e [0x%04X] \n", i, thesrc2[pos * vec_size + i], * (ne10_uint32_t*) &thesrc2[pos * vec_size + i]);
                }
#endif
                assert_float_vec_equal (&thedst_c[pos * vec_size], &thedst_neon[pos * vec_size], ERROR_MARGIN_SMALL, vec_size);
            }
        }
    }
    free (guarded_src1);
    free (guarded_src2);
    free (guarded_dst_c);
    free (guarded_dst_neon);
#endif

#ifdef PERFORMANCE_TEST
    perftest_length = PERF_TEST_ITERATION * MAX_VEC_COMPONENTS;
    /* init src memory */
    NE10_SRC_ALLOC (perftest_thesrc1, perftest_guarded_src1, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC (perftest_thesrc2, perftest_guarded_src2, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (perftest_thedst_c, perftest_guarded_dst_c, perftest_length);
    NE10_DST_ALLOC (perftest_thedst_neon, perftest_guarded_dst_neon, perftest_length);

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        GET_TIME (time_overhead,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++);
                 );
        GET_TIME (time_c,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop] (perftest_thedst_c, perftest_thesrc1, perftest_thesrc2, loop);
                 );
        GET_TIME (time_neon,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop + 1] (perftest_thedst_neon, perftest_thesrc1, perftest_thesrc2, loop);
                 );
        time_speedup = (time_neon - time_overhead) / (time_c - time_overhead);
        fprintf (stdout, "vector %d\n", func_loop + 1);
        fprintf (stdout, "average time speedup  %8f \n", time_speedup);
        //fprintf(stdout, "average time overhead %8d micro seconds\n", time_overhead);
        //fprintf(stdout, "average time C        %8d micro seconds\n", time_c);
        //fprintf(stdout, "average time NEON     %8d micro seconds\n", time_neon);
    }

    free (perftest_guarded_src1);
    free (perftest_guarded_src2);
    free (perftest_guarded_dst_c);
    free (perftest_guarded_dst_neon);
#endif

    fprintf (stdout, "----------%30s end\n", __FUNCTION__);
#undef MAX_VEC_COMPONENTS
}

void test_addmat()
{
#define MAX_VEC_COMPONENTS 4
    ne10_int32_t loop;
    ne10_int32_t i;
    ne10_int32_t func_loop;
    ne10_int32_t vec_size;
    ne10_int32_t pos;

    fprintf (stdout, "----------%30s start\n", __FUNCTION__);

    /* init function table */
    memset (ftbl_4args, 0, sizeof (ftbl_4args));
    ftbl_4args[ 2] = (ne10_func_4args_t) addmat_2x2f_c;
    ftbl_4args[ 3] = (ne10_func_4args_t) addmat_2x2f_neon;
    ftbl_4args[ 4] = (ne10_func_4args_t) addmat_3x3f_c;
    ftbl_4args[ 5] = (ne10_func_4args_t) addmat_3x3f_neon;
    ftbl_4args[ 6] = (ne10_func_4args_t) addmat_4x4f_c;
    ftbl_4args[ 7] = (ne10_func_4args_t) addmat_4x4f_neon;

#if defined (SMOKE_TEST)||(REGRESSION_TEST)
    const ne10_uint32_t fixed_length = TEST_ITERATION * MAX_VEC_COMPONENTS * MAX_VEC_COMPONENTS;

    /* init src memory */
    NE10_SRC_ALLOC (thesrc1, guarded_src1, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC (thesrc2, guarded_src2, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (thedst_c, guarded_dst_c, fixed_length);
    NE10_DST_ALLOC (thedst_neon, guarded_dst_neon, fixed_length);

    for (func_loop = 1; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        for (loop = 0; loop < TEST_ITERATION; loop++)
        {
            vec_size = (func_loop + 1) * (func_loop + 1);

            GUARD_ARRAY (thedst_c, loop * vec_size);
            GUARD_ARRAY (thedst_neon, loop * vec_size);

            ftbl_4args[2 * func_loop] (thedst_c, thesrc1, thesrc2, loop);
            ftbl_4args[2 * func_loop + 1] (thedst_neon, thesrc1, thesrc2, loop);

            CHECK_ARRAY_GUARD (thedst_c, loop * vec_size);
            CHECK_ARRAY_GUARD (thedst_neon, loop * vec_size);

            for (pos = 0; pos < loop; pos++)
            {
#ifdef DEBUG_TRACE
                fprintf (stdout, "func: %d loop count: %d position: %d \n", func_loop, loop, pos);
                for (i = 0; i < vec_size; i++)
                {
                    fprintf (stdout, "thesrc1->%d: %e [0x%04X] \n", i, thesrc1[pos * vec_size + i], * (ne10_uint32_t*) &thesrc1[pos * vec_size + i]);
                    fprintf (stdout, "thesrc2->%d: %e [0x%04X] \n", i, thesrc2[pos * vec_size + i], * (ne10_uint32_t*) &thesrc2[pos * vec_size + i]);
                }
#endif
                assert_float_vec_equal (&thedst_c[pos * vec_size], &thedst_neon[pos * vec_size], ERROR_MARGIN_SMALL, vec_size);
            }
        }
    }
    free (guarded_src1);
    free (guarded_src2);
    free (guarded_dst_c);
    free (guarded_dst_neon);
#endif

#ifdef PERFORMANCE_TEST
    perftest_length = PERF_TEST_ITERATION * MAX_VEC_COMPONENTS * MAX_VEC_COMPONENTS;
    /* init src memory */
    NE10_SRC_ALLOC (perftest_thesrc1, perftest_guarded_src1, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC (perftest_thesrc2, perftest_guarded_src2, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (perftest_thedst_c, perftest_guarded_dst_c, perftest_length);
    NE10_DST_ALLOC (perftest_thedst_neon, perftest_guarded_dst_neon, perftest_length);

    for (func_loop = 1; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        GET_TIME (time_overhead,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++)
                 );
        GET_TIME (time_c,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop] (perftest_thedst_c, perftest_thesrc1, perftest_thesrc2, loop);
                 );
        GET_TIME (time_neon,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop + 1] (perftest_thedst_neon, perftest_thesrc1, perftest_thesrc2, loop);
                 );
        time_speedup = (time_neon - time_overhead) / (time_c - time_overhead);
        fprintf (stdout, "vector %d\n", func_loop + 1);
        fprintf (stdout, "average time speedup  %8f \n", time_speedup);
        //fprintf(stdout, "average time overhead %8d micro seconds\n", time_overhead);
        //fprintf(stdout, "average time C        %8d micro seconds\n", time_c);
        //fprintf(stdout, "average time NEON     %8d micro seconds\n", time_neon);
    }

    free (perftest_guarded_src1);
    free (perftest_guarded_src2);
    free (perftest_guarded_dst_c);
    free (perftest_guarded_dst_neon);
#endif

    fprintf (stdout, "----------%30s end\n", __FUNCTION__);
#undef MAX_VEC_COMPONENTS
}

void test_detmat()
{
#define MAX_VEC_COMPONENTS 4
    ne10_int32_t loop;
    ne10_int32_t i;
    ne10_int32_t func_loop;
    ne10_int32_t vec_size;
    ne10_int32_t pos;

    fprintf (stdout, "----------%30s start\n", __FUNCTION__);

    /* init function table */
    memset (ftbl_3args, 0, sizeof (ftbl_3args));
    ftbl_3args[ 2] = (ne10_func_3args_t) detmat_2x2f_c;
    ftbl_3args[ 3] = (ne10_func_3args_t) detmat_2x2f_neon;
    ftbl_3args[ 4] = (ne10_func_3args_t) detmat_3x3f_c;
    ftbl_3args[ 5] = (ne10_func_3args_t) detmat_3x3f_neon;
    ftbl_3args[ 6] = (ne10_func_3args_t) detmat_4x4f_c;
    ftbl_3args[ 7] = (ne10_func_3args_t) detmat_4x4f_neon;

#if defined (SMOKE_TEST)||(REGRESSION_TEST)
    const ne10_uint32_t fixed_length = TEST_ITERATION * MAX_VEC_COMPONENTS * MAX_VEC_COMPONENTS;

    /* init src memory */
    NE10_SRC_ALLOC_LIMIT (thesrc1, guarded_src1, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (thedst_c, guarded_dst_c, fixed_length);
    NE10_DST_ALLOC (thedst_neon, guarded_dst_neon, fixed_length);

    for (func_loop = 1; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        for (loop = 0; loop < TEST_ITERATION; loop++)
        {
            vec_size = (func_loop + 1) * (func_loop + 1);

            GUARD_ARRAY (thedst_c, loop);
            GUARD_ARRAY (thedst_neon, loop);

            ftbl_3args[2 * func_loop] (thedst_c, thesrc1, loop);
            ftbl_3args[2 * func_loop + 1] (thedst_neon, thesrc1, loop);

            CHECK_ARRAY_GUARD (thedst_c, loop);
            CHECK_ARRAY_GUARD (thedst_neon, loop);

            for (pos = 0; pos < loop; pos++)
            {
#ifdef DEBUG_TRACE
                fprintf (stdout, "func: %d loop count: %d position: %d \n", func_loop, loop, pos);
                for (i = 0; i < vec_size; i++)
                {
                    fprintf (stdout, "thesrc1->%d: %e [0x%04X] \n", i, thesrc1[pos * vec_size + i], * (ne10_uint32_t*) &thesrc1[pos * vec_size + i]);
                }
#endif
                assert_float_vec_equal (&thedst_c[pos * vec_size], &thedst_neon[pos * vec_size], ERROR_MARGIN_SMALL, 1);
            }
        }
    }
    free (guarded_src1);
    free (guarded_dst_c);
    free (guarded_dst_neon);
#endif

#ifdef PERFORMANCE_TEST
    perftest_length = PERF_TEST_ITERATION * MAX_VEC_COMPONENTS * MAX_VEC_COMPONENTS;
    /* init src memory */
    NE10_SRC_ALLOC (perftest_thesrc1, perftest_guarded_src1, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (perftest_thedst_c, perftest_guarded_dst_c, perftest_length);
    NE10_DST_ALLOC (perftest_thedst_neon, perftest_guarded_dst_neon, perftest_length);

    for (func_loop = 1; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        GET_TIME (time_overhead,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++);
                 );
        GET_TIME (time_c,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_3args[2 * func_loop] (perftest_thedst_c, perftest_thesrc1, loop);
                 );
        GET_TIME (time_neon,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_3args[2 * func_loop + 1] (perftest_thedst_neon, perftest_thesrc1, loop);
                 );
        time_speedup = (time_neon - time_overhead) / (time_c - time_overhead);
        fprintf (stdout, "vector %d\n", func_loop + 1);
        fprintf (stdout, "average time speedup  %8f \n", time_speedup);
        //fprintf(stdout, "average time overhead %8d micro seconds\n", time_overhead);
        //fprintf(stdout, "average time C        %8d micro seconds\n", time_c);
        //fprintf(stdout, "average time NEON     %8d micro seconds\n", time_neon);
    }

    free (perftest_guarded_src1);
    free (perftest_guarded_dst_c);
    free (perftest_guarded_dst_neon);
#endif

    fprintf (stdout, "----------%30s end\n", __FUNCTION__);
#undef MAX_VEC_COMPONENTS
}

void test_identitymat()
{
#define MAX_VEC_COMPONENTS 4
    ne10_int32_t loop;
    ne10_int32_t i;
    ne10_int32_t func_loop;
    ne10_int32_t vec_size;
    ne10_int32_t pos;

    fprintf (stdout, "----------%30s start\n", __FUNCTION__);

    /* init function table */
    memset (ftbl_2args, 0, sizeof (ftbl_2args));
    ftbl_2args[ 2] = (ne10_func_2args_t) identitymat_2x2f_c;
    ftbl_2args[ 3] = (ne10_func_2args_t) identitymat_2x2f_neon;
    ftbl_2args[ 4] = (ne10_func_2args_t) identitymat_3x3f_c;
    ftbl_2args[ 5] = (ne10_func_2args_t) identitymat_3x3f_neon;
    ftbl_2args[ 6] = (ne10_func_2args_t) identitymat_4x4f_c;
    ftbl_2args[ 7] = (ne10_func_2args_t) identitymat_4x4f_neon;

#if defined (SMOKE_TEST)||(REGRESSION_TEST)
    const ne10_uint32_t fixed_length = TEST_ITERATION * MAX_VEC_COMPONENTS * MAX_VEC_COMPONENTS;

    /* init dst memory */
    NE10_DST_ALLOC (thedst_c, guarded_dst_c, fixed_length);
    NE10_DST_ALLOC (thedst_neon, guarded_dst_neon, fixed_length);

    for (func_loop = 1; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        for (loop = 0; loop < TEST_ITERATION; loop++)
        {
            vec_size = (func_loop + 1) * (func_loop + 1);

            GUARD_ARRAY (thedst_c, loop * vec_size);
            GUARD_ARRAY (thedst_neon, loop * vec_size);

            ftbl_2args[2 * func_loop] (thedst_c, loop);
            ftbl_2args[2 * func_loop + 1] (thedst_neon, loop);

            CHECK_ARRAY_GUARD (thedst_c, loop * vec_size);
            CHECK_ARRAY_GUARD (thedst_neon, loop * vec_size);

            for (pos = 0; pos < loop; pos++)
            {
#ifdef DEBUG_TRACE
                fprintf (stdout, "func: %d loop count: %d position: %d \n", func_loop, loop, pos);
#endif
                assert_float_vec_equal (&thedst_c[pos * vec_size], &thedst_neon[pos * vec_size], ERROR_MARGIN_SMALL, vec_size);
            }
        }
    }
    free (guarded_dst_c);
    free (guarded_dst_neon);
#endif

#ifdef PERFORMANCE_TEST
    perftest_length = PERF_TEST_ITERATION * MAX_VEC_COMPONENTS * MAX_VEC_COMPONENTS;
    /* init dst memory */
    NE10_DST_ALLOC (perftest_thedst_c, perftest_guarded_dst_c, perftest_length);
    NE10_DST_ALLOC (perftest_thedst_neon, perftest_guarded_dst_neon, perftest_length);

    for (func_loop = 1; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        GET_TIME (time_overhead,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++)
                 );
        GET_TIME (time_c,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_2args[2 * func_loop] (perftest_thedst_c, loop);
                 );
        GET_TIME (time_neon,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_2args[2 * func_loop + 1] (perftest_thedst_neon, loop);
                 );
        time_speedup = (time_neon - time_overhead) / (time_c - time_overhead);
        fprintf (stdout, "vector %d\n", func_loop + 1);
        fprintf (stdout, "average time speedup  %8f \n", time_speedup);
        //fprintf(stdout, "average time overhead %8d micro seconds\n", time_overhead);
        //fprintf(stdout, "average time C        %8d micro seconds\n", time_c);
        //fprintf(stdout, "average time NEON     %8d micro seconds\n", time_neon);
    }

    free (perftest_guarded_dst_c);
    free (perftest_guarded_dst_neon);
#endif

    fprintf (stdout, "----------%30s end\n", __FUNCTION__);
#undef MAX_VEC_COMPONENTS
}

void test_invmat()
{
#define MAX_VEC_COMPONENTS 4
    ne10_int32_t loop;
    ne10_int32_t i;
    ne10_int32_t func_loop;
    ne10_int32_t vec_size;
    ne10_int32_t pos;

    fprintf (stdout, "----------%30s start\n", __FUNCTION__);

    /* init function table */
    memset (ftbl_3args, 0, sizeof (ftbl_3args));
    ftbl_3args[ 2] = (ne10_func_3args_t) invmat_2x2f_c;
    ftbl_3args[ 3] = (ne10_func_3args_t) invmat_2x2f_neon;
    ftbl_3args[ 4] = (ne10_func_3args_t) invmat_3x3f_c;
    ftbl_3args[ 5] = (ne10_func_3args_t) invmat_3x3f_neon;
    ftbl_3args[ 6] = (ne10_func_3args_t) invmat_4x4f_c;
    ftbl_3args[ 7] = (ne10_func_3args_t) invmat_4x4f_neon;

#if defined (SMOKE_TEST)||(REGRESSION_TEST)
    const ne10_uint32_t fixed_length = TEST_ITERATION * MAX_VEC_COMPONENTS * MAX_VEC_COMPONENTS;

    /* init src memory */
    NE10_SRC_ALLOC_LIMIT (thesrc1, guarded_src1, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (thedst_c, guarded_dst_c, fixed_length);
    NE10_DST_ALLOC (thedst_neon, guarded_dst_neon, fixed_length);

    for (func_loop = 1; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        for (loop = 0; loop < TEST_ITERATION; loop++)
        {
            vec_size = (func_loop + 1) * (func_loop + 1);

            GUARD_ARRAY (thedst_c, loop * vec_size);
            GUARD_ARRAY (thedst_neon, loop * vec_size);

            ftbl_3args[2 * func_loop] (thedst_c, thesrc1, loop);
            ftbl_3args[2 * func_loop + 1] (thedst_neon, thesrc1, loop);

            CHECK_ARRAY_GUARD (thedst_c, loop * vec_size);
            CHECK_ARRAY_GUARD (thedst_neon, loop * vec_size);

            for (pos = 0; pos < loop; pos++)
            {
#ifdef DEBUG_TRACE
                fprintf (stdout, "func: %d loop count: %d position: %d \n", func_loop, loop, pos);
                for (i = 0; i < vec_size; i++)
                {
                    fprintf (stdout, "thesrc1->%d: %e [0x%04X] \n", i, thesrc1[pos * vec_size + i], * (ne10_uint32_t*) &thesrc1[pos * vec_size + i]);
                }
#endif
                assert_float_vec_equal (&thedst_c[pos * vec_size], &thedst_neon[pos * vec_size], ERROR_MARGIN_LARGE, vec_size);
            }
        }
    }
    free (guarded_src1);
    free (guarded_dst_c);
    free (guarded_dst_neon);
#endif

#ifdef PERFORMANCE_TEST
    perftest_length = PERF_TEST_ITERATION * MAX_VEC_COMPONENTS * MAX_VEC_COMPONENTS;
    /* init src memory */
    NE10_SRC_ALLOC (perftest_thesrc1, perftest_guarded_src1, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (perftest_thedst_c, perftest_guarded_dst_c, perftest_length);
    NE10_DST_ALLOC (perftest_thedst_neon, perftest_guarded_dst_neon, perftest_length);

    for (func_loop = 1; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        GET_TIME (time_overhead,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++)
                 );
        GET_TIME (time_c,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_3args[2 * func_loop] (perftest_thedst_c, perftest_thesrc1, loop);
                 );
        GET_TIME (time_neon,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_3args[2 * func_loop + 1] (perftest_thedst_neon, perftest_thesrc1, loop);
                 );
        time_speedup = (time_neon - time_overhead) / (time_c - time_overhead);
        fprintf (stdout, "vector %d\n", func_loop + 1);
        fprintf (stdout, "average time speedup  %8f \n", time_speedup);
        //fprintf(stdout, "average time overhead %8d micro seconds\n", time_overhead);
        //fprintf(stdout, "average time C        %8d micro seconds\n", time_c);
        //fprintf(stdout, "average time NEON     %8d micro seconds\n", time_neon);
    }

    free (perftest_guarded_src1);
    free (perftest_guarded_dst_c);
    free (perftest_guarded_dst_neon);
#endif

    fprintf (stdout, "----------%30s end\n", __FUNCTION__);
#undef MAX_VEC_COMPONENTS
}

void test_mulmat()
{
#define MAX_VEC_COMPONENTS 4
    ne10_int32_t loop;
    ne10_int32_t i;
    ne10_int32_t func_loop;
    ne10_int32_t vec_size;
    ne10_int32_t pos;

    fprintf (stdout, "----------%30s start\n", __FUNCTION__);

    /* init function table */
    memset (ftbl_4args, 0, sizeof (ftbl_4args));
    ftbl_4args[ 2] = (ne10_func_4args_t) mulmat_2x2f_c;
    ftbl_4args[ 3] = (ne10_func_4args_t) mulmat_2x2f_neon;
    ftbl_4args[ 4] = (ne10_func_4args_t) mulmat_3x3f_c;
    ftbl_4args[ 5] = (ne10_func_4args_t) mulmat_3x3f_neon;
    ftbl_4args[ 6] = (ne10_func_4args_t) mulmat_4x4f_c;
    ftbl_4args[ 7] = (ne10_func_4args_t) mulmat_4x4f_neon;

#if defined (SMOKE_TEST)||(REGRESSION_TEST)
    const ne10_uint32_t fixed_length = TEST_ITERATION * MAX_VEC_COMPONENTS * MAX_VEC_COMPONENTS;

    /* init src memory */
    NE10_SRC_ALLOC_LIMIT (thesrc1, guarded_src1, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC_LIMIT (thesrc2, guarded_src2, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (thedst_c, guarded_dst_c, fixed_length);
    NE10_DST_ALLOC (thedst_neon, guarded_dst_neon, fixed_length);

    for (func_loop = 1; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        for (loop = 0; loop < TEST_ITERATION; loop++)
        {
            vec_size = (func_loop + 1) * (func_loop + 1);

            GUARD_ARRAY (thedst_c, loop * vec_size);
            GUARD_ARRAY (thedst_neon, loop * vec_size);

            ftbl_4args[2 * func_loop] (thedst_c, thesrc1, thesrc2, loop);
            ftbl_4args[2 * func_loop + 1] (thedst_neon, thesrc1, thesrc2, loop);

            CHECK_ARRAY_GUARD (thedst_c, loop * vec_size);
            CHECK_ARRAY_GUARD (thedst_neon, loop * vec_size);

            for (pos = 0; pos < loop; pos++)
            {
#ifdef DEBUG_TRACE
                fprintf (stdout, "func: %d loop count: %d position: %d \n", func_loop, loop, pos);
                for (i = 0; i < vec_size; i++)
                {
                    fprintf (stdout, "thesrc1->%d: %e [0x%04X] \n", i, thesrc1[pos * vec_size + i], * (ne10_uint32_t*) &thesrc1[pos * vec_size + i]);
                    fprintf (stdout, "thesrc2->%d: %e [0x%04X] \n", i, thesrc2[pos * vec_size + i], * (ne10_uint32_t*) &thesrc2[pos * vec_size + i]);
                }
#endif
                assert_float_vec_equal (&thedst_c[pos * vec_size], &thedst_neon[pos * vec_size], ERROR_MARGIN_SMALL, vec_size);
            }
        }
    }
    free (guarded_src1);
    free (guarded_src2);
    free (guarded_dst_c);
    free (guarded_dst_neon);
#endif

#ifdef PERFORMANCE_TEST
    perftest_length = PERF_TEST_ITERATION * MAX_VEC_COMPONENTS * MAX_VEC_COMPONENTS;
    /* init src memory */
    NE10_SRC_ALLOC (perftest_thesrc1, perftest_guarded_src1, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC (perftest_thesrc2, perftest_guarded_src2, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (perftest_thedst_c, perftest_guarded_dst_c, perftest_length);
    NE10_DST_ALLOC (perftest_thedst_neon, perftest_guarded_dst_neon, perftest_length);

    for (func_loop = 1; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        GET_TIME (time_overhead,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++)
                 );
        GET_TIME (time_c,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop] (perftest_thedst_c, perftest_thesrc1, perftest_thesrc2, loop);
                 );
        GET_TIME (time_neon,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop + 1] (perftest_thedst_neon, perftest_thesrc1, perftest_thesrc2, loop);
                 );
        time_speedup = (time_neon - time_overhead) / (time_c - time_overhead);
        fprintf (stdout, "vector %d\n", func_loop + 1);
        fprintf (stdout, "average time speedup  %8f \n", time_speedup);
        //fprintf(stdout, "average time overhead %8d micro seconds\n", time_overhead);
        //fprintf(stdout, "average time C        %8d micro seconds\n", time_c);
        //fprintf(stdout, "average time NEON     %8d micro seconds\n", time_neon);
    }

    free (perftest_guarded_src1);
    free (perftest_guarded_src2);
    free (perftest_guarded_dst_c);
    free (perftest_guarded_dst_neon);
#endif

    fprintf (stdout, "----------%30s end\n", __FUNCTION__);
#undef MAX_VEC_COMPONENTS
}

void test_submat()
{
#define MAX_VEC_COMPONENTS 4
    ne10_int32_t loop;
    ne10_int32_t i;
    ne10_int32_t func_loop;
    ne10_int32_t vec_size;
    ne10_int32_t pos;

    fprintf (stdout, "----------%30s start\n", __FUNCTION__);

    /* init function table */
    memset (ftbl_4args, 0, sizeof (ftbl_4args));
    ftbl_4args[ 2] = (ne10_func_4args_t) submat_2x2f_c;
    ftbl_4args[ 3] = (ne10_func_4args_t) submat_2x2f_neon;
    ftbl_4args[ 4] = (ne10_func_4args_t) submat_3x3f_c;
    ftbl_4args[ 5] = (ne10_func_4args_t) submat_3x3f_neon;
    ftbl_4args[ 6] = (ne10_func_4args_t) submat_4x4f_c;
    ftbl_4args[ 7] = (ne10_func_4args_t) submat_4x4f_neon;

#if defined (SMOKE_TEST)||(REGRESSION_TEST)
    const ne10_uint32_t fixed_length = TEST_ITERATION * MAX_VEC_COMPONENTS * MAX_VEC_COMPONENTS;

    /* init src memory */
    NE10_SRC_ALLOC (thesrc1, guarded_src1, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC (thesrc2, guarded_src2, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (thedst_c, guarded_dst_c, fixed_length);
    NE10_DST_ALLOC (thedst_neon, guarded_dst_neon, fixed_length);

    for (func_loop = 1; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        for (loop = 0; loop < TEST_ITERATION; loop++)
        {
            vec_size = (func_loop + 1) * (func_loop + 1);

            GUARD_ARRAY (thedst_c, loop * vec_size);
            GUARD_ARRAY (thedst_neon, loop * vec_size);

            ftbl_4args[2 * func_loop] (thedst_c, thesrc1, thesrc2, loop);
            ftbl_4args[2 * func_loop + 1] (thedst_neon, thesrc1, thesrc2, loop);

            CHECK_ARRAY_GUARD (thedst_c, loop * vec_size);
            CHECK_ARRAY_GUARD (thedst_neon, loop * vec_size);

            for (pos = 0; pos < loop; pos++)
            {
#ifdef DEBUG_TRACE
                fprintf (stdout, "func: %d loop count: %d position: %d \n", func_loop, loop, pos);
                for (i = 0; i < vec_size; i++)
                {
                    fprintf (stdout, "thesrc1->%d: %e [0x%04X] \n", i, thesrc1[pos * vec_size + i], * (ne10_uint32_t*) &thesrc1[pos * vec_size + i]);
                    fprintf (stdout, "thesrc2->%d: %e [0x%04X] \n", i, thesrc2[pos * vec_size + i], * (ne10_uint32_t*) &thesrc2[pos * vec_size + i]);
                }
#endif
                assert_float_vec_equal (&thedst_c[pos * vec_size], &thedst_neon[pos * vec_size], ERROR_MARGIN_SMALL, vec_size);
            }
        }
    }
    free (guarded_src1);
    free (guarded_src2);
    free (guarded_dst_c);
    free (guarded_dst_neon);
#endif

#ifdef PERFORMANCE_TEST
    perftest_length = PERF_TEST_ITERATION * MAX_VEC_COMPONENTS * MAX_VEC_COMPONENTS;
    /* init src memory */
    NE10_SRC_ALLOC (perftest_thesrc1, perftest_guarded_src1, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC (perftest_thesrc2, perftest_guarded_src2, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (perftest_thedst_c, perftest_guarded_dst_c, perftest_length);
    NE10_DST_ALLOC (perftest_thedst_neon, perftest_guarded_dst_neon, perftest_length);

    for (func_loop = 1; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        GET_TIME (time_overhead,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++)
                 );
        GET_TIME (time_c,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop] (perftest_thedst_c, perftest_thesrc1, perftest_thesrc2, loop);
                 );
        GET_TIME (time_neon,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop + 1] (perftest_thedst_neon, perftest_thesrc1, perftest_thesrc2, loop);
                 );
        time_speedup = (time_neon - time_overhead) / (time_c - time_overhead);
        fprintf (stdout, "vector %d\n", func_loop + 1);
        fprintf (stdout, "average time speedup  %8f \n", time_speedup);
        //fprintf(stdout, "average time overhead %8d micro seconds\n", time_overhead);
        //fprintf(stdout, "average time C        %8d micro seconds\n", time_c);
        //fprintf(stdout, "average time NEON     %8d micro seconds\n", time_neon);
    }

    free (perftest_guarded_src1);
    free (perftest_guarded_src2);
    free (perftest_guarded_dst_c);
    free (perftest_guarded_dst_neon);
#endif

    fprintf (stdout, "----------%30s end\n", __FUNCTION__);
#undef MAX_VEC_COMPONENTS
}

void test_transmat()
{
#define MAX_VEC_COMPONENTS 4
    ne10_int32_t loop;
    ne10_int32_t i;
    ne10_int32_t func_loop;
    ne10_int32_t vec_size;
    ne10_int32_t pos;

    fprintf (stdout, "----------%30s start\n", __FUNCTION__);

    /* init function table */
    memset (ftbl_3args, 0, sizeof (ftbl_3args));
    ftbl_3args[ 2] = (ne10_func_3args_t) transmat_2x2f_c;
    ftbl_3args[ 3] = (ne10_func_3args_t) transmat_2x2f_neon;
    ftbl_3args[ 4] = (ne10_func_3args_t) transmat_3x3f_c;
    ftbl_3args[ 5] = (ne10_func_3args_t) transmat_3x3f_neon;
    ftbl_3args[ 6] = (ne10_func_3args_t) transmat_4x4f_c;
    ftbl_3args[ 7] = (ne10_func_3args_t) transmat_4x4f_neon;

#if defined (SMOKE_TEST)||(REGRESSION_TEST)
    const ne10_uint32_t fixed_length = TEST_ITERATION * MAX_VEC_COMPONENTS * MAX_VEC_COMPONENTS;

    /* init src memory */
    NE10_SRC_ALLOC (thesrc1, guarded_src1, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (thedst_c, guarded_dst_c, fixed_length);
    NE10_DST_ALLOC (thedst_neon, guarded_dst_neon, fixed_length);

    for (func_loop = 1; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        for (loop = 0; loop < TEST_ITERATION; loop++)
        {
            vec_size = (func_loop + 1) * (func_loop + 1);

            GUARD_ARRAY (thedst_c, loop * vec_size);
            GUARD_ARRAY (thedst_neon, loop * vec_size);

            ftbl_3args[2 * func_loop] (thedst_c, thesrc1, loop);
            ftbl_3args[2 * func_loop + 1] (thedst_neon, thesrc1, loop);

            CHECK_ARRAY_GUARD (thedst_c, loop * vec_size);
            CHECK_ARRAY_GUARD (thedst_neon, loop * vec_size);

            for (pos = 0; pos < loop; pos++)
            {
#ifdef DEBUG_TRACE
                fprintf (stdout, "func: %d loop count: %d position: %d \n", func_loop, loop, pos);
                for (i = 0; i < vec_size; i++)
                {
                    fprintf (stdout, "thesrc1->%d: %e [0x%04X] \n", i, thesrc1[pos * vec_size + i], * (ne10_uint32_t*) &thesrc1[pos * vec_size + i]);
                }
#endif
                assert_float_vec_equal (&thedst_c[pos * vec_size], &thedst_neon[pos * vec_size], ERROR_MARGIN_SMALL, vec_size);
            }
        }
    }
    free (guarded_src1);
    free (guarded_dst_c);
    free (guarded_dst_neon);
#endif

#ifdef PERFORMANCE_TEST
    perftest_length = PERF_TEST_ITERATION * MAX_VEC_COMPONENTS * MAX_VEC_COMPONENTS;
    /* init src memory */
    NE10_SRC_ALLOC (perftest_thesrc1, perftest_guarded_src1, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (perftest_thedst_c, perftest_guarded_dst_c, perftest_length);
    NE10_DST_ALLOC (perftest_thedst_neon, perftest_guarded_dst_neon, perftest_length);

    for (func_loop = 1; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        GET_TIME (time_overhead,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++)
                 );
        GET_TIME (time_c,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_3args[2 * func_loop] (perftest_thedst_c, perftest_thesrc1, loop);
                 );
        GET_TIME (time_neon,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_3args[2 * func_loop + 1] (perftest_thedst_neon, perftest_thesrc1, loop);
                 );
        time_speedup = (time_neon - time_overhead) / (time_c - time_overhead);
        fprintf (stdout, "vector %d\n", func_loop + 1);
        fprintf (stdout, "average time speedup  %8f \n", time_speedup);
        //fprintf(stdout, "average time overhead %8d micro seconds\n", time_overhead);
        //fprintf(stdout, "average time C        %8d micro seconds\n", time_c);
        //fprintf(stdout, "average time NEON     %8d micro seconds\n", time_neon);
    }

    free (perftest_guarded_src1);
    free (perftest_guarded_dst_c);
    free (perftest_guarded_dst_neon);
#endif

    fprintf (stdout, "----------%30s end\n", __FUNCTION__);
#undef MAX_VEC_COMPONENTS
}

void test_mulcmatvec()
{
#define MAX_VEC_COMPONENTS 4
    ne10_int32_t loop;
    ne10_int32_t i;
    ne10_int32_t func_loop;
    ne10_int32_t vec_size;
    ne10_int32_t pos;

    fprintf (stdout, "----------%30s start\n", __FUNCTION__);

    /* init function table */
    memset (ftbl_4args, 0, sizeof (ftbl_4args));
    ftbl_4args[ 2] = (ne10_func_4args_t) mulcmatvec_cm2x2f_v2f_c;
    ftbl_4args[ 3] = (ne10_func_4args_t) mulcmatvec_cm2x2f_v2f_neon;
    ftbl_4args[ 4] = (ne10_func_4args_t) mulcmatvec_cm3x3f_v3f_c;
    ftbl_4args[ 5] = (ne10_func_4args_t) mulcmatvec_cm3x3f_v3f_neon;
    ftbl_4args[ 6] = (ne10_func_4args_t) mulcmatvec_cm4x4f_v4f_c;
    ftbl_4args[ 7] = (ne10_func_4args_t) mulcmatvec_cm4x4f_v4f_neon;

#if defined (SMOKE_TEST)||(REGRESSION_TEST)
    const ne10_uint32_t fixed_length = TEST_ITERATION * MAX_VEC_COMPONENTS;

    /* init src memory */
    NE10_SRC_ALLOC_LIMIT (thesrc1, guarded_src1, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC_LIMIT (thecst, guarded_cst, MAX_VEC_COMPONENTS * MAX_VEC_COMPONENTS); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (thedst_c, guarded_dst_c, fixed_length);
    NE10_DST_ALLOC (thedst_neon, guarded_dst_neon, fixed_length);

    for (func_loop = 1; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        for (loop = 0; loop < TEST_ITERATION; loop++)
        {
            vec_size = func_loop + 1;

            GUARD_ARRAY (thedst_c, loop * vec_size);
            GUARD_ARRAY (thedst_neon, loop * vec_size);

            ftbl_4args[2 * func_loop] (thedst_c, thecst, thesrc1, loop);
            ftbl_4args[2 * func_loop + 1] (thedst_neon, thecst, thesrc1, loop);

            CHECK_ARRAY_GUARD (thedst_c, loop * vec_size);
            CHECK_ARRAY_GUARD (thedst_neon, loop * vec_size);

            for (pos = 0; pos < loop; pos++)
            {
#ifdef DEBUG_TRACE
                fprintf (stdout, "func: %d loop count: %d position: %d \n", func_loop, loop, pos);
                for (i = 0; i < vec_size * vec_size; i++)
                {
                    fprintf (stdout, "thecst->%d: %e [0x%04X] \n", i, thecst[i], * (ne10_uint32_t*) &thecst[i]);
                }
                for (i = 0; i < vec_size; i++)
                {
                    fprintf (stdout, "thesrc1->%d: %e [0x%04X] \n", i, thesrc1[pos * vec_size + i], * (ne10_uint32_t*) &thesrc1[pos * vec_size + i]);
                }
#endif
                assert_float_vec_equal (&thedst_c[pos * vec_size], &thedst_neon[pos * vec_size], ERROR_MARGIN_SMALL, vec_size);
            }
        }
    }
    free (guarded_cst);
    free (guarded_src1);
    free (guarded_dst_c);
    free (guarded_dst_neon);
#endif

#ifdef PERFORMANCE_TEST
    perftest_length = PERF_TEST_ITERATION * MAX_VEC_COMPONENTS;
    /* init src memory */
    NE10_SRC_ALLOC (perftest_thesrc1, perftest_guarded_src1, perftest_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    NE10_SRC_ALLOC (perftest_thecst, perftest_guarded_cst, MAX_VEC_COMPONENTS * MAX_VEC_COMPONENTS); // 16 extra bytes at the begining and 16 extra bytes at the end

    /* init dst memory */
    NE10_DST_ALLOC (perftest_thedst_c, perftest_guarded_dst_c, perftest_length);
    NE10_DST_ALLOC (perftest_thedst_neon, perftest_guarded_dst_neon, perftest_length);

    for (func_loop = 1; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        GET_TIME (time_overhead,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++)
                 );
        GET_TIME (time_c,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop] (perftest_thedst_c, perftest_thecst, perftest_thesrc1, loop);
                 );
        GET_TIME (time_neon,
                  for (loop = 0; loop < PERF_TEST_ITERATION; loop++) ftbl_4args[2 * func_loop + 1] (perftest_thedst_neon, perftest_thecst, perftest_thesrc1, loop);
                 );
        time_speedup = (time_neon - time_overhead) / (time_c - time_overhead);
        fprintf (stdout, "vector %d\n", func_loop + 1);
        fprintf (stdout, "average time speedup  %8f \n", time_speedup);
        //fprintf(stdout, "average time overhead %8d micro seconds\n", time_overhead);
        //fprintf(stdout, "average time C        %8d micro seconds\n", time_c);
        //fprintf(stdout, "average time NEON     %8d micro seconds\n", time_neon);
    }

    free (perftest_guarded_src1);
    free (perftest_guarded_src2);
    free (perftest_guarded_dst_c);
    free (perftest_guarded_dst_neon);
#endif

    fprintf (stdout, "----------%30s end\n", __FUNCTION__);
#undef MAX_VEC_COMPONENTS
}



void my_test_setup (void)
{
    //printf("------%-30s start\r\n", __FUNCTION__);
}

void my_test_teardown (void)
{
    //printf("--------end\r\n");
}

void test_fixture_math (void)
{
    test_fixture_start();               // starts a fixture

    fixture_setup (my_test_setup);
    fixture_teardown (my_test_teardown);

#if 1
    run_test (test_abs);       // run tests
    run_test (test_addc);
    run_test (test_add);
    run_test (test_cross);
    run_test (test_divc);
    run_test (test_div);
    run_test (test_dot);
    run_test (test_len);
    run_test (test_mlac);
    run_test (test_mla);
    run_test (test_mulc);
    run_test (test_mul);
    run_test (test_normalize);
    run_test (test_rsbc);
    run_test (test_setc);
    run_test (test_subc);
    run_test (test_sub);
    run_test (test_addmat);
    run_test (test_detmat);
    run_test (test_identitymat);
    run_test (test_invmat);
    run_test (test_mulmat);
    run_test (test_mulcmatvec);
    run_test (test_submat);
    run_test (test_transmat);
#endif

    test_fixture_end();                 // ends a fixture
}
