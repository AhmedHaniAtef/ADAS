#include "../inc/PID.h"

void PID_Init(PID_Controller *pid, float Kp, float Ki, float Kd, float N, float dt, float outputMin, float outputMax) {
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

float PID_Compute(PID_Controller *pid, float setpoint, float measuredValue) {
    // Calculate error
    float error = setpoint - measuredValue;

    // Proportional term
    float Pout = pid->Kp * error;

    // Integral term
    pid->integral += error * pid->dt;
    float Iout = pid->Ki * pid->integral;

    // Derivative term with filtering
    float derivative = (error - pid->prevError) / pid->dt;
    float Dout = pid->Kd * ((pid->N * derivative - pid->prevD) / (1.0f + pid->N * pid->dt));

    // Update previous values
    pid->prevError = error;
    pid->prevD = Dout;

    // Total output
    float output = Pout + Iout + Dout;

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