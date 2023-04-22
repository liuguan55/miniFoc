//
// Created by 邓志君 on 2023/4/16.
//

#ifndef MINIFOC_F4_INCLUDE_SYS_INTERRUPT_H_
#define MINIFOC_F4_INCLUDE_SYS_INTERRUPT_H_

#include "compiler.h"

#if defined(__CC_ARM)
/* ARM Compiler */

/*
 * CPU interrupt mask handling.
 */
#define IRQMASK_REG_NAME_R primask
#define IRQMASK_REG_NAME_W primask

/*
 * Save the current interrupt enable state & disable IRQs
 */
static __always_inline unsigned long arch_irq_save(void)
{
    unsigned long flags;

    __asm {
        mrs	flags, IRQMASK_REG_NAME_R
        cpsid	i
    }
    return flags;
}

/*
 * restore saved IRQ state
 */
static __always_inline void arch_irq_restore(unsigned long flags)
{
    __asm { msr	IRQMASK_REG_NAME_W, flags }
}

/*
 * Enable IRQs
 */
#define arch_irq_enable()	__enable_irq()

/*
 * Disable IRQs
 */
#define arch_irq_disable()	__disable_irq()

/*
 * Enable FIQs
 */
#define arch_fiq_enable()	__enable_fiq()

/*
 * Disable FIQs
 */
#define arch_fiq_disable()	__disable_fiq()

#elif defined(__GNUC__)
/* GNU Compiler */

/*
 * CPU interrupt mask handling.
 */
#define IRQMASK_REG_NAME_R "primask"
#define IRQMASK_REG_NAME_W "primask"

/*
 * Save the current interrupt enable state & disable IRQs
 */
static __always_inline unsigned long arch_irq_save(void) {
  unsigned long flags;

  __asm volatile(
  "mrs	%0, " IRQMASK_REG_NAME_R "\n"
  "cpsid	i"
  : "=r" (flags) : : "memory", "cc");
  return flags;
}

/*
 * restore saved IRQ state
 */
static __always_inline void arch_irq_restore(unsigned long flags) {
  __asm volatile(
  "msr	" IRQMASK_REG_NAME_W ", %0"
  :
  : "r" (flags)
  : "memory", "cc");
}

/*
 * Save the current interrupt enable state.
 */
static __always_inline unsigned long arch_irq_get_flags(void) {
  unsigned long flags;

  __asm volatile(
  "mrs %0, " IRQMASK_REG_NAME_R "\n"
  : "=r" (flags) : : "memory", "cc");
  return flags;

}

/*
 * Enable IRQs
 */
#define arch_irq_enable()    __asm volatile("cpsie i" : : : "memory", "cc")

/*
 * Disable IRQs
 */
#define arch_irq_disable()    __asm volatile("cpsid i" : : : "memory", "cc")

/*
 * Enable FIQs
 */
#define arch_fiq_enable()    __asm volatile("cpsie f" : : : "memory", "cc")

/*
 * Disable FIQs
 */
#define arch_fiq_disable()    __asm volatile("cpsid f" : : : "memory", "cc")

#else
#error "Compiler not supported."
#endif

#endif //MINIFOC_F4_INCLUDE_SYS_INTERRUPT_H_
