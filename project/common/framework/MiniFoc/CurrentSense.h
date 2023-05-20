#ifndef CURRENTSENSE_H
#define CURRENTSENSE_H


/******************************************************************************/
#include "foc_utils.h" 
/******************************************************************************/
float getDCCurrent(FocCurrent_t *sense,float motor_electrical_angle);
DQCurrent_s getFOCCurrents(FocCurrent_t *sense,float angle_el);
/******************************************************************************/

#endif

