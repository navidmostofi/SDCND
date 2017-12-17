# CarND-Controls-PID
## Self-Driving Car Engineer Nanodegree Program


In this project I used 2 PID (stands for Proportional, Integral, Differential) controllers to control steerning value and throttle of the car. I tuned Kp, Ki, Kd parameters of PID controller manually.

CTE = desired position – actual position

The P component causes the steering angle to be proportional to CTE. If car is in the right side of center of the road, it will turn to left and if the car is in the left side of center of the road, it will turn to right. But the car will osilate around desired trajectory.

The D component is used to reduce osilations caused by P component. Output of controller is proportional to changing rate of CTE. A properly tuned D parameter will cause the car to approach the center line smoothly without ringing.

The I component is used to reduce systematic bias. This bias can take several forms, such as a steering drift.

Tuning PID controller is not easy. I tuned the parameters manually and started from suggested values from lectures of Sebastian Thurn. Then tuned them to get good results. Finally PID controller parameters equal to [0.224, 0.00045, 4.5] worked well.

I used just P controller for throttle controller and set target speed to 55 MPH and it worked good. I started from little values and increased that to 0.2 and it works.

You can find a video of performance of pid controller in the following link:
https://youtu.be/aDVZdrf69QA

---

## Dependencies

* cmake >= 3.5
 * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1(mac, linux), 3.81(Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools]((https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)
* [uWebSockets](https://github.com/uWebSockets/uWebSockets)
  * Run either `./install-mac.sh` or `./install-ubuntu.sh`.
  * If you install from source, checkout to commit `e94b6e1`, i.e.
    ```
    git clone https://github.com/uWebSockets/uWebSockets 
    cd uWebSockets
    git checkout e94b6e1
    ```
    Some function signatures have changed in v0.14.x. See [this PR](https://github.com/udacity/CarND-MPC-Project/pull/3) for more details.
* Simulator. You can download these from the [project intro page](https://github.com/udacity/self-driving-car-sim/releases) in the classroom.

There's an experimental patch for windows in this [PR](https://github.com/udacity/CarND-PID-Control-Project/pull/3)

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./pid`. 

Tips for setting up your environment can be found [here](https://classroom.udacity.com/nanodegrees/nd013/parts/40f38239-66b6-46ec-ae68-03afd8a601c8/modules/0949fca6-b379-42af-a919-ee50aa304e6a/lessons/f758c44c-5e40-4e01-93b5-1a82aa4e044f/concepts/23d376c7-0195-4276-bdf0-e02f1f3c665d)
