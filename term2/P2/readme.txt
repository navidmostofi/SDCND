
PROJECT DESCRIPTION

In this project I implemented an unscented Kalman filter using the CTRV motion model and C++. We will be using the same bicycle simulation data set from the extended Kalman filter project. That way We can compare our results with the EKF project.

Remember that all Kalman filters have the same three steps:

- Initialization
- Prediction
- Update

A standard Kalman filter can only handle linear equations. Both the extended Kalman filter and the unscented Kalman filter allow you to use non-linear equations; the difference between EKF and UKF is how they handle non-linear equations. But the basics are the same: initialize, predict, update.

The project "unscented Kalman filter" is based on the same structure as the extended Kalman filter.
RMSE for two datasets are as follow: ([X, Y, Vx, Vy])
- 1st dataset : [0.07, 0.08, 0.29, 0.27] 
- 2nd dataset : [0.07, 0.06, 0.51, 0.29]
The results meet the rubric points of the project. 
