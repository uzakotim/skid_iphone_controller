#ifndef _KALMAN_
#define _KALMAN_
#include <Eigen/Eigen>

class KalmanFilter
{
private:
    int state_dim;
    int measurement_dim;

public:
    Eigen::VectorXd state;
    Eigen::MatrixXd covariance;
    KalmanFilter(int state_dim, int measurement_dim, Eigen::VectorXd init_state) : state_dim(state_dim), measurement_dim(measurement_dim), state(init_state), covariance(Eigen::MatrixXd::Identity(state_dim, state_dim)) {}

    void predict(double dt, double process_noise)
    {
        // State transition matrix
        Eigen::MatrixXd F = Eigen::MatrixXd::Identity(state_dim, state_dim);
        for (int i = 0; i < state_dim / 2; i++)
        {
            F(i, i + state_dim / 2) = dt;
        }

        // Process noise covariance matrix
        Eigen::MatrixXd Q = Eigen::MatrixXd::Identity(state_dim, state_dim) * (process_noise * process_noise);

        // Perform prediction
        state = F * state;
        covariance = F * covariance * F.transpose() + Q;
    }

    void correct(Eigen::VectorXd measurement, double measurement_noise)
    {
        // Measurement matrix
        Eigen::MatrixXd H = Eigen::MatrixXd::Identity(measurement_dim, state_dim);

        // Measurement noise covariance matrix
        Eigen::MatrixXd R = Eigen::MatrixXd::Identity(measurement_dim, measurement_dim) * (measurement_noise * measurement_noise);

        // Kalman gain
        Eigen::MatrixXd K = covariance * H.transpose() * (H * covariance * H.transpose() + R).inverse();

        // Update state and covariance
        state = state + K * (measurement - H * state);
        covariance = (Eigen::MatrixXd::Identity(state_dim, state_dim) - K * H) * covariance;
    }
};

#endif /* _KALMAN_ */