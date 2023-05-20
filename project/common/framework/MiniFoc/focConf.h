//
// Created by 邓志君 on 2023/1/15.
//
#include "common/framework/MiniFoc/platorm/stm32/hal_port.h"

#ifndef MINI_FOC_LIBS_SIMPLEFOC_FOCCONF_H_
#define MINI_FOC_LIBS_SIMPLEFOC_FOCCONF_H_

#define DEF_POWER_SUPPLY 12.0 //!< default power supply voltage

#define DEF_POLEPAIRS    11

#define DEF_VOLTAGE_LIMIT  6
// velocity PI controller params
#define DEF_PID_VEL_P 0.5 //!< default PID controller P value
#define DEF_PID_VEL_I 0 //!<  default PID controller I value
#define DEF_PID_VEL_D 0.0 //!<  default PID controller D value
#define DEF_PID_VEL_RAMP 50.0 //!< default PID controller voltage ramp value
#define DEF_PID_VEL_LIMIT (DEF_POWER_SUPPLY) //!< default PID controller voltage limit

// current sensing PID values

// for stm32, due, teensy, esp32 and similar
#define DEF_PID_CURR_Q_P 0.6 //!< default PID controller P value
#define DEF_PID_CURR_Q_I 0 //!<  default PID controller I value
#define DEF_PID_CURR_Q_D 0.0 //!<  default PID controller D value
#define DEF_PID_CURR_Q_RAMP 0  //!< default PID controller voltage ramp value

#define DEF_PID_CURR_D_P 0.6 //!< default PID controller P value
#define DEF_PID_CURR_D_I 0 //!<  default PID controller I value
#define DEF_PID_CURR_D_D 0.0 //!<  default PID controller D value
#define DEF_PID_CURR_D_RAMP 0  //!< default PID controller voltage ramp value

#define DEF_PID_CURR_LIMIT (DEF_POWER_SUPPLY) //!< default PID controller voltage limit
#define DEF_CURR_FILTER_Tf 0.005 //!< default currnet filter time constant

// default current limit values
#define DEF_CURRENT_LIM 50 //!< 2Amps current limit by default

// default monitor downsample
#define DEF_MON_DOWNSMAPLE 100 //!< default monitor downsample
#define DEF_MOTION_DOWNSMAPLE 0 //!< default motion downsample - disable

// angle P params
#define DEF_P_ANGLE_P 20.0 //!< default P controller P value
#define DEF_P_ANGLE_I 0 //!< default P controller P value
#define DEF_P_ANGLE_D 0 //!< default P controller P value
#define DEF_P_ANGLE_RAMP 0 //!< default P controller P value
#define DEF_VEL_LIM 20.0 //!< angle velocity limit default

#define DEF_P_SPRING_P 2.5 //!< default P controller P value
#define DEF_P_SPRING_I 0 //!< default P controller P value
#define DEF_P_SPRING_D -0.1 //!< default P controller P value
#define DEF_P_SPRING_RAMP 30 //!< default P controller P value
#define DEF_SPRING_VEL_LIM 2 //!< angle velocity limit default

#define DEF_P_SPRING_WITH_DUMP_P 2.5 //!< default P controller P value
#define DEF_P_SPRING_WITH_DUMP_I 0 //!< default P controller P value
#define DEF_P_SPRING_WITH_DUMP_D 0.4 //!< default P controller P value
#define DEF_P_SPRING_WITH_DUMP_RAMP 30 //!< default P controller P value
#define DEF_SPRING_WITH_DUMP_VEL_LIM 6 //!< angle velocity limit default

#define DEF_P_DUMP_P 0 //!< default P controller P value
#define DEF_P_DUMP_I 0 //!< default P controller P value
#define DEF_P_DUMP_D 0.6 //!< default P controller P value
#define DEF_P_DUMP_RAMP 10 //!< default P controller P value
#define DEF_DUMP_VEL_LIM 8 //!< angle velocity limit default

#define DEF_P_ZERO_RESISTANCE_P 0 //!< default P controller P value
#define DEF_P_ZERO_RESISTANCE_I 0 //!< default P controller P value
#define DEF_P_ZERO_RESISTANCE_D -0.1 //!< default P controller P value
#define DEF_P_ZERO_RESISTANCE_RAMP 30 //!< default P controller P value
#define DEF_ZERO_RESISTANCE_VEL_LIM 4 //!< angle velocity limit default

#define DEF_P_KNOD_P 5 //!< default P controller P value
#define DEF_P_KNOD_I 0 //!< default P controller P value
#define DEF_P_KNOD_D 0 //!< default P controller P value
#define DEF_P_KNOD_RAMP 50 //!< default P controller P value
#define DEF_KNOD_VEL_LIM 2 //!< angle velocity limit default
#define DEF_KNOD_SECTOR_NUM 6

// index search
#define DEF_INDEX_SEARCH_TARGET_VELOCITY 1.0 //!< default index search velocity
// align voltage
#define DEF_VOLTAGE_SENSOR_ALIGN  0.8//3.0 //!< default voltage for sensor and motor zero alignemt
// low pass filter velocity
#define DEF_CURRENT_Q_FILTER_Tf  0.005 //!< default velocity filter time constant
#define DEF_CURRENT_D_FILTER_Tf  0.005 //!< default velocity filter time constant
#define DEF_VEL_FILTER_Tf  0.0001 //!< default velocity filter time constant
#define DEF_SPRING_FILTER_Tf  0.005 //!< default velocity filter time constant
#define DEF_VBUS_FILTER_Tf  0.5 //!< default velocity filter time constant
#endif //MINI_FOC_LIBS_SIMPLEFOC_FOCCONF_H_
