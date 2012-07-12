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

#include "unit_test_common.h"

void FILL_FLOAT_ARRAY( arm_float_t *arr, unsigned int count )
{
    unsigned int i = 0;

    sleep ( 1 );

    NE10_float_rng_init( time(NULL) );

    for ( i = 0; i < count; i++ )
    {
      arr[i] = NE10_float_rng_next();
    }
}

void FILL_FLOAT_ARRAY_LIMIT( arm_float_t *arr, unsigned int count )
{
    unsigned int i = 0;

    sleep ( 1 );

    NE10_float_rng_limit_init( time(NULL) );

    for ( i = 0; i < count; i++ )
    {
        arr[ i ] = NE10_float_rng_limit_next();
    }
}

void FILL_FLOAT_ARRAY_LIMIT_GT1( arm_float_t *arr, unsigned int count )
{
    unsigned int i = 0;

    sleep ( 1 );

    NE10_float_rng_limit_gt1_init( time(NULL) );

    for ( i = 0; i < count; i++ )
    {
        arr[ i ] = NE10_float_rng_limit_gt1_next();
    }
}

// this function checks whether the difference between two float values is within the acceptable error range
int EQUALS_FLOAT( float fa, float fb , unsigned int err )
{
  union
  {
    int          vi;
    float        vf;
  } conv1, conv2;

  unsigned int ui1, ui2;

  if ( fa == fb ) return 1; // if identical, then return TRUE

  conv1.vf = fa;
  conv2.vf = fb;

  if ( (conv1.vi & NAN_OR_INF) == NAN_OR_INF )
  {
     //fprintf( stderr, "HINT: The 1st floating-point value is either \'Not a number\' or \'Infinity\'. " );
     return 0; // INF or NAN, unacceptable return FALSE
  }

  if ( (conv2.vi & NAN_OR_INF) == NAN_OR_INF )
  {
     //fprintf( stderr, "HINT: The 1st floating-point value is either \'Not a number\' or \'Infinity\'. " );
     return 0; // INF or NAN, unacceptable return FALSE
  }

  int cut1 = conv1.vi & SIGNBIT_MASK; // drop the sign bit - i.e. the left most bit
  int cut2 = conv2.vi & SIGNBIT_MASK;

  if ( (cut1 & EXPONENT_MASK) == cut1 ) { cut1 = 0; } // zero out subnormal float values
  if ( (cut2 & EXPONENT_MASK) == cut2 ) { cut2 = 0; } // zero out subnormal float values

  memcpy( &ui1,  &fa, sizeof(arm_float_t) );
  memcpy( &ui2,  &fb, sizeof(arm_float_t) );

  if ( abs( cut1 - cut2 ) > err ) // this is the log() of the actual error
  {  // then we have an unacceptable error

     // report an unacceptable error
     //fprintf( stderr, "HINT: %e (0x%04X) != %e (0x%04X) ", fa, ui1, fb, ui2 );

     return 0;
  }

  if ( fb*fa < 0.0f )
  {

     //fprintf( stderr, "HINT: %e (0x%04X) is the opposite of %e (0x%04X) ", fa, ui1, fb, ui2 );

     return 0;
  }

  return 1; // acceptable, return TRUE
}
