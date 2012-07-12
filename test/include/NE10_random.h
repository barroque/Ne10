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
 * NE10 Library : headers/NE10_random.h
 */


#ifndef __NE10_RANDOM__
#define __NE10_RANDOM__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include <math.h>
#include <assert.h>


// used for creating different instances of random number generators with different seeds and states
typedef struct
{
  // these are used as internal values, please do not change them directly
  uint32_t _private_m_A              ;// = 1103515245L; // a, must be 0 < _A < _M
  uint32_t _private_m_C              ;// = 12345L; // c, must be 0 < _C < _M
  uint32_t _private_m_X_NM1          ;// = 0; // X(n-1), at first this value is the seed or the start value
} NE10_rng_t;

typedef struct
{
  // these are used as internal values, please do not change them directly
  // there are three separate seeds for 1) the sign, 2) the exponent, 3) and the fraction bits.
  NE10_rng_t _private_m_rngs[3];
} NE10_float_rng_t;

// a random number generator that generates IEEE 754 float numbers
// NAN_OR_INF is to check whether the value is a NAN or an INF
#define NAN_OR_INF (0xFF << 23)
#define IS_NAN_OR_INF(x) ( ((x & NAN_OR_INF) == NAN_OR_INF)?1:0 )

#define EXPONENT_MASK 0x807FFFFF
#define IS_SUBNORMAL(x) ( ((x & EXPONENT_MASK) == x)?1:0 )

// generic functions
extern void NE10_rng_init_g(NE10_rng_t *rng, uint32_t seed);

extern uint32_t NE10_rng_next_g(NE10_rng_t *rng);

extern const uint32_t NE10_rng_max_g(NE10_rng_t *rng);

extern void NE10_rng_init(uint32_t seed);

extern uint32_t NE10_rng_next();

extern const uint32_t NE10_rng_max();

extern void NE10_float_rng_init_g(NE10_float_rng_t* float_rng, uint32_t seed);

extern float NE10_float_rng_next_g(NE10_float_rng_t* float_rng);

extern float NE10_float_rng_max_g(NE10_float_rng_t* float_rng);

extern void NE10_float_rng_init(uint32_t seed);

extern float NE10_float_rng_next();

extern float NE10_float_rng_max();

extern void NE10_float_rng_limit_init(uint32_t seed);

extern float NE10_float_rng_limit_next();

extern float NE10_float_rng_limit_max();

extern void NE10_float_rng_limit_gt1_init(uint32_t seed);

extern float NE10_float_rng_limit_gt1_next();

extern float NE10_float_rng_limit_gt1_max();

#endif // NE10_RANDOM

