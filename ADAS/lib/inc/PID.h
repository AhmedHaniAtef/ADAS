#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

typedef struct {

	float_t Kp;        // Proportional gain
    float_t Ki;        // Integral gain
    float_t Kd;        // Derivative gain
    float_t N;         // Filter coefficient
    float_t prevError; // Previous error
    float_t integral;  // Integral sum
    float_t prevD;     // Previous derivative term
    float_t dt;        // Time step
	float_t outputMin; // Minimum output limit
    float_t outputMax; // Maximum output limit

} PID_Controller;

void PID_Init(PID_Controller *pid, float_t Kp, float_t Ki, float_t Kd, float_t N, float_t dt, float_t outputMin, float_t outputMax);
float_t PID_Compute(PID_Controller *pid, float_t setpoint, float_t measuredValue) ;

#endif
