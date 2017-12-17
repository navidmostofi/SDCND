#include <math.h>
#include <numeric>
#include <vector>
#include <iostream>

#include "PID.h"

using namespace std;

/*
* TODO: Complete the PID class.
*/

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kp, double Ki, double Kd, double dKp, double dKi, double dKd) {
    // initializing params and d_params (P , dP liists in lectures)

    params.resize(3);
    params[0] = Kp;
    params[1] = Ki;
    params[2] = Kd;

    d_params.resize(3);
    d_params[0] = dKp;
    d_params[1] = dKi;
    d_params[2] = dKd;

    p_error = 0.0;
    d_error = 0.0;
    i_error = 0.0;

    // Accumulated mean squared error initialization
    iters = 0;
    sum_squared_error = 0;
    best_error = 50.0;
}

void PID::UpdateError(double cte) {
    // update p_error, d_error and i_error
    d_error = cte - p_error;
    p_error = cte;
    i_error += cte;

    // Accumulated sum squared error
    iters += 1;
    sum_squared_error += pow(cte, 2.0);
}

double PID::TotalError() {
    // return total error (steering angle) as multiplication of each coefficient and it's respective error

    double steer =  -params[0]*p_error -params[1]*i_error -params[2]*d_error;
    if (steer > 1){
        steer = 1;
    }else if (steer < -1){
        steer = -1;
    }

    return steer;

}

void PID::Twiddle(double tol, double best_err){

    double err = 0.0;

//    cout << "in twiddle!" << endl;
//    cout << AccErrorCalculation() << endl;

//    while (AccErrorCalculation() > tol){
        for (unsigned int i = 0; i < 3; ++i) {
            params[i] += d_params[i];
//            params[i] = params[i] + d_params[i];
            err = CalculateError();
            if (err < best_err){
//                cout << "if 1 !!" << endl;
                best_err = err;
                d_params [i] *= 1.1;
            }else{
//                cout << "else 1 !!" << endl;
                params[i] -= 2 * d_params[i];
                err = CalculateError();
                if (err < best_err){
//                    cout << "if 2 !!" << endl;
                    best_err = err;
                    d_params[i] *= 1.1;
                }else{
//                    cout << "else 2 !!" << endl;
                    params[i] += d_params[i];
                    d_params[i] *= 0.9;
                }
            }
        }
//    }

//    cout << "params in twiddle:" << params[0] << " " << params[1] << " " << params[2] << endl;

    iters = 0;
    sum_squared_error = 0.0;
}

double PID::CalculateError() {
    if(iters == 0) return 0;
    else return sum_squared_error / iters;
}

void PID::ResetError() {

    // PID errors
    p_error = 0.0;
    i_error = 0.0;
    d_error = 0.0;

    // Accumulated mean squared error
    iters = 0;
    sum_squared_error = 0.0;

}

double PID::AccErrorCalculation(){
    return accumulate(d_params.begin(), d_params.end(), 0.0);
}

double PID::CalculateBestError(double cte){
    if ( cte < best_error){
        best_error = cte;
    }
}