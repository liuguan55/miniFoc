#ifndef LOWPASS_FILTER_H
#define LOWPASS_FILTER_H
#include <stdint.h>
/******************************************************************************/
typedef struct 
{
	float Tf; //!< Low pass filter time constant
	float y_prev; //!< filtered value in previous execution step 
	uint32_t timestamp_prev;  //!< Last execution timestamp
} LowPassFilter;

/******************************************************************************/
void LPF_init(LowPassFilter* LPF,  float freq);
float LPFoperator(LowPassFilter* LPF,float x);
/******************************************************************************/

#endif

