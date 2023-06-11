//
// Created by 邓志君 on 2023/4/16.
//

#ifndef MINIFOC_F4_FRAMEWORK_PLATORM_INIT_H_
#define MINIFOC_F4_FRAMEWORK_PLATORM_INIT_H_

#include "version.h"
#include "compiler.h"
#include "apps/BldcController/pri_conf_app.h"
#include "common/board/miniFoc_v2/BoardConfig.h"
#include "common/board/Board.h"

#ifdef __cplusplus
extern "C"{
#endif

void platform_init(void);

#ifdef __cplusplus
}
#endif


#endif //MINIFOC_F4_FRAMEWORK_PLATORM_INIT_H_
