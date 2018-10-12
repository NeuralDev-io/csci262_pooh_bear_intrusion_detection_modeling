/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System ActivityEngine.cpp
* Purpose: Creates a traffic simulation engine based on reading of input from
*          Vehicles.txt and Stats.txt, as well as an argument for how many days
 *         to simulate traffic.
*
* @version 0.3-dev
* @date 2018.10.06
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails andrew at codeninja55.me & duong.daltonle at gmail.com
* Students Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/
#include "ActivityEngine.h"

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

    time_seed = static_cast<unsigned long>(chrono::system_clock::now().time_since_epoch().count());
    default_engine.seed(LONG_MAX);
    linear_congruential_engine.seed(LONG_MAX);
    mersenne_twister_engine.seed(LONG_MAX);
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

    generate_arrivals(vehicles);

    // TODO: time should be stepped in 1 minute blocks
    // TODO: program should give some indication as to what is happening, without being verbose


    cout << "Activity Engine finished: " << real_formatted_time_now() << "\n" << flush;
}

void ActivityEngine::generate_arrivals(Vehicles &vehicles)
{
    const float arrival_tot_min_interval = 1380;  // 23 hours x 60 mins - because no more arrivals after 23:00
    const float arrival_tot_hr_interval = 24;  // 24 hours x 1 day
    map<string, VehicleType>::iterator iter = vehicles.get_vehicles_dict()->begin();
    for (; iter != vehicles.get_vehicles_dict()->end(); ++iter) {

        // TODO: Ensure the returned value from a normal distribution is not negative because of a large std. dev.
        // @param mean and std. dev
        normal_distribution<double> num_normal_distrib((*iter).second.num_mean, (*iter).second.num_stddev);
        normal_distribution<double> speed_normal_distrib((*iter).second.speed_mean);
        // @param min and max
        uniform_real_distribution<double> uniform_distribution(0, 1);
        // Random number of X occurrences of vehicle over a day
        uint X = static_cast<uint>(lround(num_normal_distrib(linear_congruential_engine)));
        // rate of occurrence
        double rate_param = static_cast<double>(X / arrival_tot_min_interval);

        // TODO: debug
        cout << "Vehicle type: " << (*iter).first << " (" << X << ")" << endl;

        // For the number of occurrences for a day, generate the arrival events
        for (int j = 1; j <= X; j++) {
            VehicleStats veh_stats;
            veh_stats.registration_id = Vehicles::generate_registration((*iter).second.reg_format, default_engine);

            // number of minutes since time T = 0
            int arrival_minutes = static_cast<int>(lround(next_arrival(rate_param,
                    uniform_distribution,
                    mersenne_twister_engine)));

            // make sure it is non-negative because not possible
            do {
                veh_stats.arrival_speed = speed_normal_distrib(linear_congruential_engine);
            } while (veh_stats.arrival_speed < 0);

            // add event to queue
            SimTime event_time;
            event_time = time_now();
            event_time.tm_hour = arrival_minutes / 60;
            event_time.tm_min = arrival_minutes % 60;
            veh_stats.arrival_time = event_time;

            Event arrival_event = { ARRIVAL, event_time, veh_stats };
            event_q.push(arrival_event);

            // TODO: debug
            cout << "Arrival: " << arrival_minutes << " " << veh_stats.registration_id << " ==> ";
            cout << formatted_time(event_time)
                 << " (" << veh_stats.arrival_speed << " kmh)" << endl;
        }
    }
}

/*
 * @brief: Generate the next arrival using a exponential distribution with a uniform distribution between 0 and 1.
 *
 * @return: a value that corresponds to the 'y' axis value for the random 'x' axis value between 0 and 1 from an
 * exponential distribution.
 * */
long double ActivityEngine::next_arrival(double rate_param, uniform_real_distribution<double> &random,
                                         mt19937_64 &mt_engine)
{
    return -logl(1.0L - random(mt_engine) / (random.max() + 1)) / rate_param;
}
