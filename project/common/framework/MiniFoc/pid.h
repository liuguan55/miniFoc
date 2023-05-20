#ifndef PID_H
#define PID_H


/******************************************************************************/
typedef struct 
{
    float P; //!< Proportional gain 
    float I; //!< Integral gain 
    float D; //!< Derivative gain 
    float output_ramp; //!< Maximum speed of change of the driver value
    float limit; //!< Maximum driver value
    float error_prev; //!< last tracking error value
    float output_prev;  //!< last pid driver value
    float integral_prev; //!< last integral component value
    unsigned long timestamp_prev; //!< Last execution timestamp
} PIDController;

/******************************************************************************/
void PID_init(PIDController *pid, float p, float i, float d, float outputLimit, float outputRamp);
float PIDoperator(PIDController* PID,float error);
/******************************************************************************/

#endif

