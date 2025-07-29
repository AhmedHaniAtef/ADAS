#include "../inc/KalmanFilter.h"


// Kalman Filter Initialization
void Kalman_Init(KalmanFilter *kf) {
    kf->Q_angle = 0.8f;
    kf->Q_bias = 0.01f;
    kf->R_measure = 0.5f;
    
    kf->angle = 0.0f;
    kf->bias = 0.0f;
    
    kf->P[0][0] = 0.0f;
    kf->P[0][1] = 0.0f;
    kf->P[1][0] = 0.0f;
    kf->P[1][1] = 0.0f;
}

// Kalman Filter Update
float_t Kalman_Update(KalmanFilter *kf, float_t new_angle, float_t new_rate, float_t dt) {
    // Prediction step
    float_t rate = new_rate - kf->bias;
    kf->angle += dt * rate;
    
    // Update error covariance
    kf->P[0][0] += dt * (dt*kf->P[1][1] - kf->P[0][1] - kf->P[1][0] + kf->Q_angle);
    kf->P[0][1] -= dt * kf->P[1][1];
    kf->P[1][0] -= dt * kf->P[1][1];
    kf->P[1][1] += kf->Q_bias * dt;
    
    // Calculate innovation
    float_t y = new_angle - kf->angle;
    // Normalize angle difference
    if(y > 180.0f) y -= 360.0f;
    else if(y < -180.0f) y += 360.0f;
    
    // Calculate Kalman gain
    float_t S = kf->P[0][0] + kf->R_measure;
    float_t K[2];
    K[0] = kf->P[0][0] / S;
    K[1] = kf->P[1][0] / S;
    
    // Update state and covariance
    kf->angle += K[0] * y;
    kf->bias += K[1] * y;
    
    float_t P00_temp = kf->P[0][0];
    float_t P01_temp = kf->P[0][1];
    
    kf->P[0][0] -= K[0] * P00_temp;
    kf->P[0][1] -= K[0] * P01_temp;
    kf->P[1][0] -= K[1] * P00_temp;
    kf->P[1][1] -= K[1] * P01_temp;
    
    return kf->angle;
}