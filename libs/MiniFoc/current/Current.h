//
// Created by 邓志君 on 2023/3/5.
//

#ifndef MINIFOC_F4_LIBS_SIMPLEFOC_CURRENT_CURRENT_H_
#define MINIFOC_F4_LIBS_SIMPLEFOC_CURRENT_CURRENT_H_
#include "../foc_utils.h"

typedef struct {
  char *name ;
  void *data;
  void (*init)(void);
  PhaseCurrent_s (*getPhaseCurrents)(void);
}FocCurrent_t;

void MiniCurrentManager_init(void);
int MiniCurrentManager_registerCurrent(FocCurrent_t *current);
int MiniCurrentManager_unregisterCurrent(const char *currentName);
FocCurrent_t* MiniCurrentManager_findCurrentByName(const char *currentName);

#endif //MINIFOC_F4_LIBS_SIMPLEFOC_CURRENT_CURRENT_H_
