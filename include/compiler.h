//
// Created by 邓志君 on 2023/4/16.
//

#ifndef MINIFOC_F4_INCLUDE_COMPILER_H_
#define MINIFOC_F4_INCLUDE_COMPILER_H_

#if defined(__CC_ARM)
/* ARM Compiler */

#define inline        __inline
//#define __inline    __inline
#define __inline__    __inline

#ifndef __always_inline
#define __always_inline    __forceinline
#endif

#ifndef __noinline
#define __noinline
#endif

#ifdef __CHECKER__
# define __iomem    __attribute__((noderef, address_space(2)))
#else
# define __iomem
#endif

#if defined(__GNUC__)
/* ARM Compiler support GNU */
#define __packed    __attribute__((__packed__))
#else
//#define __packed    __packed
#endif
//#define __asm       __asm
//#define __weak      __weak

#elif defined(__GNUC__)
/* GNU Compiler */

#include <sys/cdefs.h>

//#define inline    inline
#define __inline    inline
#define __inline__  inline

#ifdef __always_inline
#undef __always_inline    /* already defined in <sys/cdefs.h> */
#define __always_inline    inline __attribute__((always_inline))
#endif



#ifndef __noinline
#define __noinline  __attribute__((__noinline__))
#endif

#define __packed    __attribute__((__packed__))
#define __asm       asm
#define __weak      __attribute__((weak))

#define __ccmram __attribute__((section (".ccmram")))

#else
#error "Compiler not supported."
#endif

#endif //MINIFOC_F4_INCLUDE_COMPILER_H_
