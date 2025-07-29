#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

typedef struct {

	float Kp;        // Proportional gain
    float Ki;        // Integral gain
    float Kd;        // Derivative gain
    float N;         // Filter coefficient
    float prevError; // Previous error
    float integral;  // Integral sum
    float prevD;     // Previous derivative term
    float dt;        // Time step
	float outputMin; // Minimum output limit
    float outputMax; // Maximum output limit

} PID_Controller;

void PID_Init(PID_Controller *pid, float Kp, float Ki, float Kd, float N, float dt, float outputMin, float outputMax);
float PID_Compute(PID_Controller *pid, float setpoint, float measuredValue) ;

#endif
