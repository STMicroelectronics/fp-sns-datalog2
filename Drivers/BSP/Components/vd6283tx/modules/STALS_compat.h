/*
* Copyright (c) 2021, STMicroelectronics - All Rights Reserved
*
* This file is part "VD6283 API" and is licensed under the terms of 
* 'BSD 3-clause "New" or "Revised" License'.
*
********************************************************************************
*
* License terms BSD 3-clause "New" or "Revised" License.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met
*
* 1. Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its contributors
* may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*
********************************************************************************
*
*/
#ifndef __STALS_COMPAT__
#define __STALS_COMPAT__ 1

/* compiler is suppose to support c99 */
/*Kernel / user code compatibility macro */
#ifdef __KERNEL__
#define POPCOUNT(a)				hweight_long(a)
#define CTZ(a)					__builtin_ctz(a)
#define assert(c)				BUG_ON(!(c))
#define UNUSED_P(x)				x __attribute__((unused))
#else /* !__KERNEL__ */
#ifdef __GNUC__
#define POPCOUNT(a)				__builtin_popcount(a)
#define CTZ(a)					__builtin_ctz(a)
#define UNUSED_P(x)				x __attribute__((unused))
#else /* !__GNUC__ */
#define POPCOUNT(a)				popcount(a)
#define CTZ(a)					ctz(a)
#define UNUSED_P(x)				(x)
#endif /* __GNUC__ */
#define div64_u64(a, b)				((a) / (b))
#endif /* __KERNEL__ */

#ifndef __GNUC__
static inline int popcount(unsigned int x)
{
	int res = 0;

	while (x > 0) {
		res += x & 1;
		x >>= 1;
	}

	return res;
}

static inline int ctz(unsigned int x)
{
	int res = 0;

	if (!x)
		return 0;

	while (!(x & 1)) {
		res++;
		x >>= 1;
	}

	return res;
}
#endif

#endif
