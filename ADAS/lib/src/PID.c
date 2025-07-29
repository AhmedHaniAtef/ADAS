#include "../inc/PID.h"

void PID_Init(PID_Controller *pid, float_t Kp, float_t Ki, float_t Kd, float_t N, float_t dt, float_t outputMin, float_t outputMax) {
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->N = N;
    pid->prevError = 0.0f;
    pid->integral = 0.0f;
    pid->prevD = 0.0f;
    pid->dt = dt;
	pid->outputMin = outputMin;
    pid->outputMax = outputMax;
}

float_t PID_Compute(PID_Controller *pid, float_t setpoint, float_t measuredValue) {
    // Calculate error
    float_t error = setpoint - measuredValue;

    // Proportional term
    float_t Pout = pid->Kp * error;

    // Integral term
    pid->integral += error * pid->dt;
    float_t Iout = pid->Ki * pid->integral;

    // Derivative term with filtering
    float_t derivative = (error - pid->prevError) / pid->dt;
    float_t Dout = pid->Kd * ((pid->N * derivative - pid->prevD) / (1.0f + pid->N * pid->dt));

    // Update previous values
    pid->prevError = error;
    pid->prevD = Dout;

    // Total output
    float_t output = Pout + Iout + Dout;

    // Apply output limits
    if (output > pid->outputMax) {
        output = pid->outputMax;
    } else if (output < pid->outputMin) {
        output = pid->outputMin;
    }

    // Anti-windup: Adjust integral term if output is saturated
    if (output == pid->outputMax && error > 0) {
        pid->integral -= error * pid->dt;
    } else if (output == pid->outputMin && error < 0) {
        pid->integral -= error * pid->dt;
    }

    return output;
}