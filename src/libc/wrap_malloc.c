#include <stdio.h>
#include <string.h>
#include <sys/reent.h>
#include <stdint.h>
#include <stddef.h>
#include "sys/sys_heap.h"

extern uint8_t __heap_start__[];
extern uint8_t __heap_end__[];

static __always_inline int is_rangeof_sramheap(void *ptr) {
  return RANGEOF((uint32_t)ptr, (uint32_t)__heap_start__, (uint32_t)__heap_end__);
}

#if (__CONFIG_MALLOC_MODE == 0x00)

#ifdef __CONFIG_MALLOC_TRACE
#include "debug/heap_trace.h"
#endif

void *__real__malloc_r(struct _reent *reent, size_t size);
void *__real__realloc_r(struct _reent *reent, void *ptr, size_t size);
void __real__free_r(struct _reent *reent, void *ptr);

#if defined(__CONFIG_MALLOC_TRACE)
static uint8_t g_do_reallocing = 0; /* flag for realloc() */
#endif
void *__wrap__malloc_r(struct _reent *reent, size_t size) {
  void *ptr;
  size_t real_size = size;

  malloc_mutex_lock();

#ifdef __CONFIG_MALLOC_TRACE
  if (!g_do_reallocing) {
    real_size += HEAP_MAGIC_LEN;
  }
#endif

  ptr = __real__malloc_r(reent, real_size);

#ifdef __CONFIG_MALLOC_TRACE
  if (!g_do_reallocing) {
    heap_trace_malloc(ptr, real_size - HEAP_MAGIC_LEN);
  }
#endif

  malloc_mutex_unlock();

  return ptr;
}

void *__wrap__realloc_r(struct _reent *reent, void *ptr, size_t size) {
  void *new_ptr = NULL;
  size_t real_size = size;

  malloc_mutex_lock();

#if defined(__CONFIG_MALLOC_TRACE)
  g_do_reallocing = 1;
#endif
#ifdef __CONFIG_MALLOC_TRACE
  if (size != 0) {
    real_size += HEAP_MAGIC_LEN;
  }
#endif
  new_ptr = __real__realloc_r(reent, ptr, real_size);

#ifdef __CONFIG_MALLOC_TRACE
  if (size == 0) {
    heap_trace_realloc(ptr, new_ptr, size);
  } else {
    heap_trace_realloc(ptr, new_ptr, real_size - HEAP_MAGIC_LEN);
  }
#endif

#if defined(__CONFIG_MALLOC_TRACE)
  g_do_reallocing = 0;
#endif

  malloc_mutex_unlock();

  return new_ptr;
}

void __wrap__free_r(struct _reent *reent, void *ptr) {
  malloc_mutex_lock();

#ifdef __CONFIG_MALLOC_TRACE
  if (!g_do_reallocing) {
    heap_trace_free(ptr);
  }
#endif
  __real__free_r(reent, ptr);
#ifdef __CONFIG_HEAP_FREE_CHECK
  HEAP_ASSERT(is_rangeof_sramheap(ptr));
#endif
  malloc_mutex_unlock();
}

#else

#include "sys/sram_heap.h"

void *__wrap__malloc_r(struct _reent *reent, size_t size)
{
    void *ptr;

    ptr = sram_malloc(size);

    return ptr;
}

void *__wrap__realloc_r(struct _reent *reent, void *ptr, size_t size)
{
    void *new_ptr = NULL;


    new_ptr = sram_realloc(ptr, size);

    return new_ptr;
}

void __wrap__free_r(struct _reent *reent, void *ptr)
{

  sram_free(ptr);

}

#endif
