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
using namespace std;

long double next_occurrence(double rate_param, uniform_real_distribution<long double> &random,
        default_random_engine &random_generator);
double exponential_probability(float lambda, int t);
double poisson_probability(float mu, int X);
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
    Logger<EVENT_TYPE> logger = Logger<EVENT_TYPE>("Traffic Engine", WARNING, "test.txt", true);
    // log for the number of Days specified at the initial running of Traffic
    stringstream msg;
    msg << "Started Traffic Engine for number of days " << simulate_days;
    logger.info(sim_time, UNKNOWN, msg.str());

    // TODO: time should be stepped in 1 minute blocks
    // TODO: program should give some indication as to what is happening, without being verbose
    default_random_engine random_generator(static_cast<unsigned long>(LONG_MAX));
    // generate_distribution_csv(random_generator);

    /* ARRIVAL EVENTS */
    const float minutes = 1379;  // 23 hours x 60 mins
    map<string, VehicleType>::iterator iter = vehicles.get_vehicles_dict()->begin();
    for (; iter != vehicles.get_vehicles_dict()->end(); ++iter) {

        // TODO: make a function to random create registration numbers

        cout << "Vehicle type: " << (*iter).first << endl;

        // @param mean and std. dev
        normal_distribution<double> normal_distribution((*iter).second.num_mean, (*iter).second.num_stddev);
        // @param min and max
        uniform_real_distribution<long double> uniform_distribution(0, 1);
        // Random number of X occurrences of vehicle over a day
        uint X = static_cast<uint>(lround(normal_distribution(random_generator)));
        // rate of occurrence
        u_long rate_param = static_cast<u_long>((*iter).second.num_mean / minutes);

        // For the number of occurrences for a day, generate the arrival events
        for (int j = 0; j <= X; j++) {
            cout << next_occurrence(rate_param, uniform_distribution, random_generator) << endl;
        }

    }

    cout << "Activity Engine finished: " << real_formatted_time_now() << "\n" << flush;
}

long double next_occurrence(double rate_param,
                            uniform_real_distribution<long double> &random,
                            default_random_engine &random_generator)
{
    return -logl(1.0L - (random(random_generator) / (random.max() + 1))) / rate_param;
}

/*
 * @param lamnda: the rate of occurrence
 * @param t: time interval to check probability
 * */
double exponential_probability(float lambda, int t)
{
    return lambda * pow(exp(1), (-lambda * t));
    // return 1.0f - pow(exp(1), (-lambda * t));
}

/*
 * @param mu: the expected occurrence over a time interval
 * @param X: the number of expected occurrences.
 * */
double poisson_probability(float mu, int X)
{
    return 1.0f - (pow(mu, X) / fact(X)) * pow(exp(1), (-mu));
}

void generate_distribution_csv(default_random_engine randomEngine)
{
    /* Bus generator test (num_mean = 3, num_std_dev = 1) */
    fstream file;
    file.open("data/normal.csv", ios::out | ios::trunc);
    if (!file.good())
        cout << "[!!] Open file error for csv." << endl;

    /* REF: http://www.cplusplus.com/reference/random/ */
    const char delim = ',';
    /* Log Normal distribution */
    lognormal_distribution<float> normal(3, 1);  // @param mean, std. dev.
    float normal_val[1379] = {};

    file << "raw" << delim << "lround" << "\n";
    for (float &i : normal_val) {
        float val = normal(randomEngine);
        i = val;
        file << val << delim << lround(val) << "\n";
    }
    file.close();

    /* Poisson distribution */
    const int n_times = 1379;
    poisson_distribution<int> poisson(3);  // @param mean
    int poisson_val[n_times] = {};

    file.open("data/poisson.csv", ios::out | ios::trunc);

    file << "raw" << "\n";
    for (int &i : poisson_val) {
        int val = poisson(randomEngine);
        i = val;
        file << val << delim << lround(val) << "\n";
    }
    file.close();
}
