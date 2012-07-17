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
 * NE10 Library : test_main.c
 */

#include "seatest.h"

void test_fixture_basic_math( void );

void all_tests( void )
{
    test_fixture_basic_math();
}


void my_suite_setup( void )
{
    //printf("I'm done before every single test in the suite\r\n");
}

void my_suite_teardown( void )
{
    //printf("I'm done after every single test in the suite\r\n");
}

int main( int argc, char** argv )
{
    suite_setup(my_suite_setup);
    suite_teardown(my_suite_teardown);
    return run_tests(all_tests);
}
