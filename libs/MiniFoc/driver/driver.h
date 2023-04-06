//
// Created by 邓志君 on 2023/3/2.
//

#ifndef MINIFOC_F4_LIBS_SIMPLEFOC_OUTPUT_OUTPUT_H_
#define MINIFOC_F4_LIBS_SIMPLEFOC_OUTPUT_OUTPUT_H_

typedef struct {
  char *name ;
  void (*init)(void);
  void (*enable)(void);
  void (*disable)(void);
  void (*setPhaseState)(int sa, int sb, int sc);
  void (*setPwm)(float Ta, float Tb , float Tc);
}BldcDriver_t;

typedef struct {
  char *name ;
  void (*init)(void);
  void (*enable)(void);
  void (*disable)(void);
  void (*setPwm)(float Ua, float Ub);
}StepperDriver_t;

void MiniBldcDriverManager_init(void);
int MiniBldcDriverManager_register(BldcDriver_t *driver);
int MiniBldcDriverManager_unregister(const char *name);
BldcDriver_t* MiniBldcDriverManager_findByName(const char *name);

void MiniStepperDriverManager_init(void);
int MiniStepperDriverManager_register(StepperDriver_t *driver);
int MiniStepperDriverManager_unregister(const char *name);
BldcDriver_t* MiniStepperDriverManager_findByName(const char *name);


#endif //MINIFOC_F4_LIBS_SIMPLEFOC_OUTPUT_OUTPUT_H_
