#ifndef KALMANFILTER_H_
#define KALMANFILTER_H_
#include <math.h>
typedef struct {
    float_t angle;     // Estimated yaw angle
    float_t bias;      // Gyro bias
    float_t P[2][2];   // Error covariance matrix
    float_t Q_angle;   // Process noise variance for angle
    float_t Q_bias;    // Process noise variance for bias
    float_t R_measure; // Measurement noise variance
} KalmanFilter;

// Function prototypes
void Kalman_Init(KalmanFilter *kf);
float_t Kalman_Update(KalmanFilter *kf, float_t new_angle, float_t new_rate, float_t dt);

#endif // KALMANFILTER_H_