/*
 * particle_filter.cpp
 *
 *  Created on: Dec 12, 2016
 *      Author: Tiffany Huang
 */

#include <random>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <math.h> 
#include <iostream>
#include <sstream>
#include <string>
#include <iterator>

#include "particle_filter.h"

using namespace std;

static default_random_engine gen;

void ParticleFilter::init(double x, double y, double theta, double std[]) {
	// TODO: Set the number of particles. Initialize all particles to first position (based on estimates of 
	//   x, y, theta and their uncertainties from GPS) and all weights to 1. 
	// Add random Gaussian noise to each particle.
	// NOTE: Consult particle_filter.h for more information about this method (and others in this file).

    num_particles = 101;

    normal_distribution<double> normal_x(0, std[0]);
    normal_distribution<double> normal_y(0, std[1]);
    normal_distribution<double> normal_theta(0, std[2]);

    for (int i = 0; i < num_particles; ++i) {

        Particle p;
        p.id = i;
        p.x = x;
        p.y = y;
        p.theta = theta;
        p.weight = 1.0;

        // add random gaussian noise to particles
        p.x += normal_x(gen);
        p.y += normal_y(gen);
        p.theta += normal_theta(gen);

        // add created particle to particles vector
        particles.push_back(p);

    }

    // set is_initialized flag to true
    is_initialized = true;
}


void ParticleFilter::prediction(double delta_t, double std_pos[], double velocity, double yaw_rate) {
	// TODO: Add measurements to each particle and add random Gaussian noise.
	// NOTE: When adding noise you may find std::normal_distribution and std::default_random_engine useful.
	//  http://en.cppreference.com/w/cpp/numeric/random/normal_distribution
	//  http://www.cplusplus.com/reference/random/default_random_engine/

    normal_distribution<double> normal_x(0, std_pos[0]);
    normal_distribution<double> normal_y(0, std_pos[1]);
    normal_distribution<double> normal_theta(0, std_pos[2]);

    for (int i = 0; i < particles.size(); ++i) {
        if (fabs(yaw_rate) < 0.00001) {
            particles[i].x += velocity * delta_t * cos(particles[i].theta);
            particles[i].y += velocity * delta_t * sin(particles[i].theta);
        }else{
            particles[i].x +=
                    velocity * (sin(particles[i].theta + yaw_rate * delta_t) - sin(particles[i].theta)) / yaw_rate;
            particles[i].y +=
                    velocity * (cos(particles[i].theta) - cos(particles[i].theta + yaw_rate * delta_t)) / yaw_rate;
            particles[i].theta += yaw_rate * delta_t;
        }

        // add random gaussian noise to particle's x,y,theta
        particles[i].x += normal_x(gen);
        particles[i].y += normal_y(gen);
        particles[i].theta += normal_theta(gen);
    }
}

void ParticleFilter::dataAssociation(std::vector<LandmarkObs> predicted, std::vector<LandmarkObs>& observations) {
	// TODO: Find the predicted measurement that is closest to each observed measurement and assign the 
	//   observed measurement to this particular landmark.
	// NOTE: this method will NOT be called by the grading code. But you will probably find it useful to 
	//   implement this method and use it as a helper during the updateWeights phase.

    for (int i = 0; i < observations.size(); ++i) {

        int id = -1;
        double min_dist = 1e100;

        for (int j = 0; j < predicted.size(); ++j) {

            double diff_x = observations[i].x - predicted[j].x;
            double diff_y = observations[i].y - predicted[j].y;
            double cur_dist = sqrt(diff_x*diff_x + diff_y*diff_y);

            if (cur_dist < min_dist){
                min_dist = cur_dist;
                id = predicted[j].id;
            }
        }
        observations[i].id = id;
    }
}

void ParticleFilter::updateWeights(double sensor_range, double std_landmark[], 
		const std::vector<LandmarkObs> &observations, const Map &map_landmarks) {
	// TODO: Update the weights of each particle using a mult-variate Gaussian distribution. You can read
	//   more about this distribution here: https://en.wikipedia.org/wiki/Multivariate_normal_distribution
	// NOTE: The observations are given in the VEHICLE'S coordinate system. Your particles are located
	//   according to the MAP'S coordinate system. You will need to transform between the two systems.
	//   Keep in mind that this transformation requires both rotation AND translation (but no scaling).
	//   The following is a good resource for the theory:
	//   https://www.willamette.edu/~gorr/classes/GeneralGraphics/Transforms/transforms2d.htm
	//   and the following is a good resource for the actual equation to implement (look at equation 
	//   3.33
	//   http://planning.cs.uiuc.edu/node99.html

    // iterate through each particle
    for (int i = 0; i < particles.size(); ++i) {

        double x_particle = particles[i].x;
        double y_particle = particles[i].y;
        double theta_particle = particles[i].theta;

        //transformation from car's coordinate sytem to map's coordinate system
        vector<LandmarkObs> transformed_coords;
        for (int j = 0; j < observations.size(); ++j) {

            double x_observation = observations[j].x;
            double y_observation = observations[j].y;

            double x_map = x_particle + x_observation * cos(theta_particle) - y_observation * sin(theta_particle);
            double y_map = y_particle + x_observation * sin(theta_particle) + y_observation * cos(theta_particle);

            transformed_coords.push_back(LandmarkObs{observations[j].id, x_map, y_map});
        }

        // find landmarks within the sensor range
        vector<LandmarkObs> lm_in_rang;
        for (int k = 0; k < map_landmarks.landmark_list.size(); ++k) {
            double lm_x = map_landmarks.landmark_list[k].x_f;
            double lm_y = map_landmarks.landmark_list[k].y_f;
            int lm_id = map_landmarks.landmark_list[k].id_i;

            double diff_x = lm_x - x_particle;
            double diff_y = lm_y - y_particle;
            double dist = sqrt(diff_x * diff_x + diff_y * diff_y);

            if (dist < sensor_range){
                lm_in_rang.push_back(LandmarkObs{lm_id, lm_x, lm_y});
            }
        }

        // do the data association part
        dataAssociation(lm_in_rang, transformed_coords);

        // update particle's weight
        particles[i].weight = 1.0;

        double sig_x = std_landmark[0];
        double sig_y = std_landmark[1];
        double gauss_norm = 1 / (2 * M_PI * sig_x * sig_y);

        for (int l = 0; l < transformed_coords.size(); ++l) {

            double associated_lm_x;
            double associated_lm_y;

            double tr_x = transformed_coords[l].x;
            double tr_y = transformed_coords[l].y;

            int tr_id = transformed_coords[l].id;

            // find the landmark associated to transformed observation
            if (lm_in_rang.size()>0) {
                for (int q = 0; q < lm_in_rang.size(); ++q) {
                    if (tr_id == lm_in_rang[q].id) {
                        associated_lm_x = lm_in_rang[q].x;
                        associated_lm_y = lm_in_rang[q].y;
                    }
                }
            }else{

            }

            double exponent = pow((tr_x - associated_lm_x), 2) / (2 * pow(sig_x, 2)) + pow((tr_y - associated_lm_y), 2) / (2 * pow(sig_y, 2));
            double tmp_w = gauss_norm * exp(-exponent);

            particles[i].weight *= tmp_w;
        }
    }


}

void ParticleFilter::resample() {
	// TODO: Resample particles with replacement with probability proportional to their weight. 
	// NOTE: You may find std::discrete_distribution helpful here.
	//   http://en.cppreference.com/w/cpp/numeric/random/discrete_distribution
    
    vector<double> weights;
    for (int i = 0; i < num_particles; ++i) {
        weights.push_back(particles[i].weight);
    }

    // generate random
    vector<Particle> new_particles;
    uniform_int_distribution<int> U_dist(0, num_particles-1);
    int index = U_dist(gen);
    double beta = 0.0;

    // find max(weight)
    double mw = 0.0;
    for (int j = 0; j < weights.size(); ++j) {
        if (weights[j] > mw){
            mw = weights[j];
        }
    }
//    double mw = *max_element(weights.begin(), weights.end());

    uniform_real_distribution<double> U_dist_real(0, 2.0*mw);
    for (int k = 0; k < num_particles; ++k) {
        beta += U_dist_real(gen);
        while(beta > weights[index]){
            beta -= weights[index];
            index = (index + 1) % num_particles;
        }
        new_particles.push_back(particles[index]);
    }

    particles = new_particles;

}

Particle ParticleFilter::SetAssociations(Particle particle, std::vector<int> associations, std::vector<double> sense_x, std::vector<double> sense_y)
{
	//particle: the particle to assign each listed association, and association's (x,y) world coordinates mapping to
	// associations: The landmark id that goes along with each listed association
	// sense_x: the associations x mapping already converted to world coordinates
	// sense_y: the associations y mapping already converted to world coordinates

	//Clear the previous associations
	particle.associations.clear();
	particle.sense_x.clear();
	particle.sense_y.clear();

	particle.associations= associations;
 	particle.sense_x = sense_x;
 	particle.sense_y = sense_y;

 	return particle;
}

string ParticleFilter::getAssociations(Particle best)
{
	vector<int> v = best.associations;
	stringstream ss;
    copy( v.begin(), v.end(), ostream_iterator<int>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    return s;
}
string ParticleFilter::getSenseX(Particle best)
{
	vector<double> v = best.sense_x;
	stringstream ss;
    copy( v.begin(), v.end(), ostream_iterator<float>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    return s;
}
string ParticleFilter::getSenseY(Particle best)
{
	vector<double> v = best.sense_y;
	stringstream ss;
    copy( v.begin(), v.end(), ostream_iterator<float>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    return s;
}
