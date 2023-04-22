//
// Created by 邓志君 on 2023/4/16.
//

#ifndef MINIFOC_F4_SRC_DEBUG_HEAP_TRACE_H_
#define MINIFOC_F4_SRC_DEBUG_HEAP_TRACE_H_
#include <stdlib.h>
#ifdef __cplusplus
extern "C" {
#endif

#if defined(__CONFIG_MALLOC_TRACE)

#define HEAP_MAGIC_LEN	4

typedef struct {
    int entry_cnt;
    int entry_cnt_max;
    size_t sum;
    size_t sum_max;
} heap_info_t;

#ifdef __CONFIG_MALLOC_TRACE
void sram_heap_trace_info_show(int verbose);
void sram_heap_trace_info_get(heap_info_t *info);
#endif


void heap_trace_info_show(int verbose);
void heap_trace_info_get(heap_info_t *info);

int heap_trace_malloc(void *ptr, size_t size);
int heap_trace_free(void *ptr);
int heap_trace_realloc(void *old_ptr, void *new_ptr, size_t new_size);

#endif

#ifdef __cplusplus
}
#endif

#endif //MINIFOC_F4_SRC_DEBUG_HEAP_TRACE_H_
