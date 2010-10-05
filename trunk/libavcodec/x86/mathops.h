/*
 * simple math operations
 * Copyright (c) 2006 Michael Niedermayer <michaelni@gmx.at> et al
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef AVCODEC_X86_MATHOPS_H
#define AVCODEC_X86_MATHOPS_H

#include "config.h"
#include "libavutil/common.h"

#if ARCH_X86_32
#define MULL(ra, rb, shift) \
        ({ int rt, dummy; __asm__ (\
            "imull %3               \n\t"\
            "shrdl %4, %%edx, %%eax \n\t"\
            : "=a"(rt), "=d"(dummy)\
            : "a" ((int)(ra)), "rm" ((int)(rb)), "i"(shift));\
         rt; })

#define MULH(ra, rb) \
    ({ int rt, dummy;\
     __asm__ ("imull %3\n\t" : "=d"(rt), "=a"(dummy): "a" ((int)(ra)), "rm" ((int)(rb)));\
     rt; })

#define MUL64(ra, rb) \
    ({ int64_t rt;\
     __asm__ ("imull %2\n\t" : "=A"(rt) : "a" ((int)(ra)), "g" ((int)(rb)));\
     rt; })
#endif

#if HAVE_CMOV
/* median of 3 */
#define mid_pred mid_pred
static inline av_const int mid_pred(int a, int b, int c)
{
#ifndef _MSC_VER
	    int i=b;
	    __asm__ volatile(
	        "cmp    %2, %1 \n\t"
	        "cmovg  %1, %0 \n\t"
	        "cmovg  %2, %1 \n\t"
	        "cmp    %3, %1 \n\t"
	        "cmovl  %3, %1 \n\t"
	        "cmp    %1, %0 \n\t"
	        "cmovg  %1, %0 \n\t"
	        :"+&r"(i), "+&r"(a)
	        :"r"(b), "r"(c)
	    );
	    return i;
#else
	//JRS: convert
	if(a>b)
	{
		if(c>b)
		{
			if(c>a) 
				b=a;
			else
				b=c;
		}
	}
	else
	{
		if(b>c)
		{
			if(c>a)
				b=c;
			else
				b=a;
		}
	}

	return b;
#endif
}
#endif

#if HAVE_CMOV
//JRS: convert
#ifndef _MSC_VER
	#define COPY3_IF_LT(x, y, a, b, c, d)\
	__asm__ volatile(\
	    "cmpl  %0, %3       \n\t"\
	    "cmovl %3, %0       \n\t"\
	    "cmovl %4, %1       \n\t"\
	    "cmovl %5, %2       \n\t"\
	    : "+&r" (x), "+&r" (a), "+r" (c)\
	    : "r" (y), "r" (b), "r" (d)\
	);
#else
#define COPY3_IF_LT(x, y, a, b, c, d) if ((y) < (x)) { (x) = (y); (a) = (b); (c) = (d); }
#endif
#endif

// avoid +32 for shift optimization (gcc should do that ...)
#define NEG_SSR32 NEG_SSR32
//JRS: convert
#ifndef _MSC_VER
	static inline  int32_t NEG_SSR32( int32_t a, int8_t s){
	    __asm__ ("sarl %1, %0\n\t"
	         : "+r" (a)
	         : "ic" ((uint8_t)(-s))
	    );
	    return a;
	}
#else
#define NEG_SSR32(a,s) ((( int32_t)(a))>>(32-(s)))
#endif

#define NEG_USR32 NEG_USR32
//JRS: convert
#ifndef _MSC_VER
	static inline uint32_t NEG_USR32(uint32_t a, int8_t s){
	    __asm__ ("shrl %1, %0\n\t"
	         : "+r" (a)
	         : "ic" ((uint8_t)(-s))
	    );
	    return a;
	}
#else
#define NEG_USR32(a,s) (((uint32_t)(a))>>(32-(s)))
#endif

#endif /* AVCODEC_X86_MATHOPS_H */
