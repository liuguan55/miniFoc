#ifndef _SYS_HEAP_H_
#define _SYS_HEAP_H_

#include <stdio.h>
#include <stdint.h>
#include "kernel/FreeRTOS/FreeRTOS.h"
#include "kernel/FreeRTOS/task.h"
#include "sys/interrupt.h"


static __inline void malloc_mutex_lock(void)
{
#ifdef USE_RTOS_SYSTEM
  if (!arch_irq_get_flags()){
    vTaskSuspendAll();
  }
#endif
}

static __inline void malloc_mutex_unlock(void)
{
#ifdef USE_RTOS_SYSTEM
  if (!arch_irq_get_flags()){
    xTaskResumeAll();
  }
#endif
}

#define RANGEOF(num, start, end) (((num) <= (end)) && ((num) >= (start)))

#ifdef CONFIG_BACKTRACE
#define HEAP_ASSERT(x)     \
	do {                   \
		if (!x) {          \
			printf("[ERR]heap assertion!<%s, %d>\n", __func__, __LINE__);    \
			backtrace_show();                                                \
		}                  \
	} while (0)
#else
#define HEAP_ASSERT(x)     \
	do {                   \
		if (!x) {          \
			printf("[ERR]heap assertion!<%s, %d>\n", __func__, __LINE__);    \
		}                  \
	} while (0)
#endif

/* Define the linked list structure.  This is used to link free blocks in order
of their memory address. */
typedef struct A_BLOCK_LINK {
    struct A_BLOCK_LINK *pxNextFreeBlock;   /*<< The next free block in the list. */
    size_t xBlockSize;                      /*<< The size of the free block. */
} BlockLink_t;

typedef struct sys_heap {
    size_t heapBits_Per_Byte;
    uint8_t portByte_Alignment;
    uint8_t portByte_Alignment_Mask;
    size_t configTotal_Heap_Size;
    size_t heapMinmun_Block_Size;
    size_t xHeapStructSize;
    uint8_t *ucHeap;
    BlockLink_t xStart;
    BlockLink_t *pxEnd;
    size_t xFreeBytesRemaining;
    size_t xMinimumEverFreeBytesRemaining;
    size_t xBlockAllocatedBit;
} sys_heap_t;

void sys_heap_free(sys_heap_t *sysHeap, void *ptr);
void *sys_heap_malloc(sys_heap_t *sysHeap, size_t size);
void *sys_heap_realloc(sys_heap_t *sysHeap, uint8_t *ptr, size_t size);
void *sys_heap_calloc(sys_heap_t *sysHeap, size_t nmemb, size_t size);
size_t sys_heap_xPortGetFreeHeapSize(sys_heap_t *sysHeap);
size_t sys_heap_xPortGetMinimumEverFreeHeapSize(sys_heap_t *sysHeap);

int sys_heap_default_init(sys_heap_t *sysHeap, uint8_t *baseAddr, size_t totalSize);
int sys_heap_init(sys_heap_t *sysHeap);

typedef void *(*sys_malloc_fn)(size_t size);
typedef void *(*sys_realloc_fn)(void *ptr, size_t size);
typedef void *(*sys_calloc_fn)(size_t nmemb, size_t size);
typedef void (*sys_free_fn)(void *ptr);

#endif

