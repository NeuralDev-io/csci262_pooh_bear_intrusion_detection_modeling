/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System ActivityEngine.cpp
* Purpose: Creates a traffic simulation engine based on reading of input from
*          Vehicles.txt and Stats.txt, as well as an argument for how many days
 *         to simulate traffic.
*
* @version 0.1-dev
* @date 2018.10.06
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails andrew at codeninja55.me & duong.daltonle at gmail.com
* Students Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/
#include "ActivityEngine.h"
#include <Python.h>
#include <iostream>
#include <random>
#include <fstream>
#include <math.h>
#include <cmath>
#include <iomanip>
#include "Helper.h"
using namespace std;

long double next_occurrence(float);
double exponential_probability(float, int);
double poisson_probability(float, float, int);
void generate_distribution_csv(default_random_engine randomEngine);

/*
 * Constructor to the activity engine to store stats value to be used to
 * create a statistical simulation model
 *
 * @param days: number of days to run the simulation
 * @param vehicles_monitored: number of vehicle types to be monitored
 * @param road_len: length of the road in km
 * @param speed_lim: speed limit in km/h
 * @param parking_spots: number of parking spaces available
 * */
ActivityEngine::ActivityEngine(uint days, uint vehicles_monitored, float road_len, float speed_lim, uint parking_spots)
{
    simulate_days = days;
    n_vehicles_monitored = vehicles_monitored;
    n_parking_spots = parking_spots;
    speed_limit = speed_lim;
    road_length = road_len;
}


void ActivityEngine::run(Vehicles &vehicles)
{
    SimTime sim_time = time_now();

    cout << "Traffic Engine started: " << real_formatted_time_now() << "\n" << flush;
    Logger logger = Logger("Traffic Engine", WARNING, "test.txt", true);
    // log for the number of Days specified at the initial running of Traffic
    stringstream msg;
    msg << "Started Traffic Engine for number of days " << simulate_days;
    logger.info(sim_time, UNKNOWN, msg.str());

    default_random_engine random_engine(0);
    // generate_distribution_csv(random_engine);

    // TODO: time should be stepped in 1 minute blocks
    // TODO: program should give some indication as to what is happening, without being verbose

    // bool generate_flag = true;
    // while (generate_flag) {

    map<string, VehicleType>::iterator iter = vehicles.get_vehicles_dict()->find("Bus");

    float minutes = 1380;  // 23 hours x 60 mins

    poisson_distribution<int> poisson((*iter).second.num_mean);
    int X = poisson(random_engine);
    // Find probability of arrival
    cout << (*iter).first << ": \nRandom X: " << X << endl;
    double p_trials = poisson_probability((*iter).second.num_mean, minutes, X);
    cout << "Poisson Probability: " << p_trials << endl;

    float rate_param = (*iter).second.num_mean / minutes;

    for (int i = 0; i < minutes; i++) {
        double p_t = exponential_probability(rate_param, i);
        cout << "Exponential Probability at time (" << i << "): " << p_t << endl;
    }

    for (int j = 0; j <= X; j++) {
        cout<<next_occurrence(rate_param)<<endl;
    }

        // generate_flag = false;
    // }

    cout << "Activity Engine finished: " << real_formatted_time_now() << "\n" << flush;
}

long double next_occurrence(float rate_param)
{
    return (long double) -logf(1.0f - (float) random() / (RAND_MAX + 1)) / rate_param;
}

double exponential_probability(float lambda, int X)
{
    return lambda * pow(exp(1), (-lambda * X));
}

double poisson_probability(float mean, float t, int X)
{
    return (pow(mean, X) / fact(X)) * pow(exp(1), (-mean));
}

void generate_distribution_csv(default_random_engine randomEngine)
{
    /* Bus generator test (num_mean = 3, num_std_dev = 1) */
    fstream file;
    file.open("data/normal.csv", ios::out | ios::trunc);
    if (!file.good())
        cout << "[!!] Open file error for csv." << endl;

    /* REF: http://www.cplusplus.com/reference/random/ */
    char delim = ',';
    /* Normal distribution */
    normal_distribution<float> normal(3, 1);  // @param mean, std. dev.
    float normal_val[1439] = {};

    file << "raw" << delim << "lround" << "\n";
    for (float &i : normal_val) {
        float val = normal(randomEngine);
        i = val;
        file << val << delim << lround(val) << "\n";
    }
    file.close();

    /* Poisson distribution */
    int n_times = 1439;
    poisson_distribution<int> poisson(3);  // @param mean
    int poisson_val[1439] = {};

    file.open("data/poisson.csv", ios::out | ios::trunc);

    file << "raw" << "\n";
    for (int &i : poisson_val) {
        int val = poisson(randomEngine);
        i = val;
        file << val << delim << lround(val) << "\n";
    }
    file.close();
}