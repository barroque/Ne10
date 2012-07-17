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

#include "NE10_math.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "seatest.h"
//function table
arm_func_3args_t ftbl_3args[MAX_FUNC_COUNT];
arm_func_4args_t ftbl_4args[MAX_FUNC_COUNT];
arm_func_5args_t ftbl_5args[MAX_FUNC_COUNT];


//input and output
arm_float_t * guarded_src1 = NULL;
arm_float_t * guarded_src2 = NULL;
arm_float_t * thesrc1 = NULL;
arm_float_t * thesrc2 = NULL;

arm_float_t * guarded_dst_c = NULL;
arm_float_t * guarded_dst_neon = NULL;
arm_float_t * thedst_c = NULL;
arm_float_t * thedst_neon = NULL;


void test_abs()
{

#define MAX_VEC_COMPONENTS 4
    int loop;
    int i;
    int func_loop;
    int vec_size;
    int pos;
    const unsigned int fixed_length = TEST_ITERATION * MAX_VEC_COMPONENTS; 
    
    /* init function table */
    memset(ftbl_3args, 0, sizeof(ftbl_3args));
    ftbl_3args[ 0] = (arm_func_3args_t) abs_float_c;
    ftbl_3args[ 1] = (arm_func_3args_t) abs_float_neon;
    ftbl_3args[ 2] = (arm_func_3args_t) abs_vec2f_c;
    ftbl_3args[ 3] = (arm_func_3args_t) abs_vec2f_neon;
    ftbl_3args[ 4] = (arm_func_3args_t) abs_vec3f_c;
    ftbl_3args[ 5] = (arm_func_3args_t) abs_vec3f_neon;
    ftbl_3args[ 6] = (arm_func_3args_t) abs_vec4f_c;
    ftbl_3args[ 7] = (arm_func_3args_t) abs_vec4f_neon;

    /* init src memory */
    NE10_SRC_ALLOC(thesrc1, guarded_src1, fixed_length); // 16 extra bytes at the begining and 16 extra bytes at the end
    
    /* init dst memory */
    NE10_DST_ALLOC(thedst_c, guarded_dst_c, fixed_length); 
    NE10_DST_ALLOC(thedst_neon, guarded_dst_neon, fixed_length); 

    for (func_loop = 0; func_loop < MAX_VEC_COMPONENTS; func_loop++)
    {
        for (loop = 0; loop < TEST_ITERATION; loop++)
        {
            vec_size = func_loop+1;
            ftbl_3args[2*func_loop]     (thedst_c, thesrc1, loop);
            ftbl_3args[2*func_loop + 1] (thedst_neon, thesrc1, loop);
            for (pos = 0; pos < loop; pos++)
            {
                printf("func: %d loop count: %d position: %d \n", func_loop, loop, pos);
                for (i=0; i<vec_size; i++)
                {
                    printf("thesrc->%d: %e [0x%04X] \n", i, thesrc1[pos*vec_size + i], *(unsigned int*)&thesrc1[pos*vec_size + i]);
                }
                assert_float_vec_equal(&thedst_c[pos*vec_size], &thedst_neon[pos*vec_size], ERROR_MARGIN_SMALL, vec_size);
            }
        }
    }

    free(guarded_src1);
    free(guarded_dst_c);
    free(guarded_dst_neon);
#undef MAX_VEC_COMPONENTS
}

void my_test_setup( void )
{
    //printf("------%-30s start\r\n", __FUNCTION__);
}

void my_test_teardown( void )
{
    //printf("--------end\r\n");
}

void test_fixture_basic_math( void )
{
    test_fixture_start();               // starts a fixture

    fixture_setup(my_test_setup);
    fixture_teardown(my_test_teardown);

    run_test(test_abs);        // run tests
    //run_test(test_);
    //run_test(test_);

    test_fixture_end();                 // ends a fixture
}
