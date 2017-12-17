#ifndef PID_H
#define PID_H

class PID {
public:
  /*
  * Errors
  */
  double p_error;
  double i_error;
  double d_error;

  // Accumulated mean squared error variables
  double sum_squared_error;
  double best_error;
  int iters;

  /*
  * Coefficients
  */
  std::vector<double> d_params;
  std::vector<double> params;


  /*
  * Constructor
  */
  PID();

  /*
  * Destructor.
  */
  virtual ~PID();

  /*
  * Initialize PID.
  */
  void Init(double Kp, double Ki, double Kd, double dKp, double dKi, double dKd);

  /*
  * Update the PID error variables given cross track error.
  */
  void UpdateError(double cte);

  /*
  * Calculate the total PID error.
  */
  double TotalError();

  /*
  * Twiddle for tuning pid controller gains
  */
  void Twiddle(double tol, double cte);

  double CalculateError();

  void ResetError();

  double AccErrorCalculation();

  double CalculateBestError(double cte);
};


#endif /* PID_H */

