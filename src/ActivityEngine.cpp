/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System ActivityEngine.cpp
* Purpose: Creates a traffic simulation engine based on reading of input from
*          Vehicles.txt and Stats.txt, as well as an argument for how many days
*          to simulate traffic.
*
* @version 0.4-dev
* @date 2018.10.06
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails andrew at codeninja55.me & duong.daltonle at gmail.com
* Students Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/
#include "ActivityEngine.h"
#include <assert.h>

ActivityEngine::ActivityEngine() : n_vehicles_monitored(0), n_parking_spots(0), road_length(0),
                                   speed_limit(0), simulate_days(0), time_seed(0)
{
    // Set the last param to true if you want to output log to stdout
    logger = Logger<SimTime, ActivityLog>("Activity Engine", INFO, "logs_baseline", true);
    veh_logger = Logger<SimTime, VehicleLog>("Activity Engine", INFO, "logs_baseline", true);

    time_seed = static_cast<unsigned long>(chrono::system_clock::now().time_since_epoch().count());
    default_engine.seed(0);
    linear_congruential_engine.seed(0);
    mersenne_twister_engine.seed(0);
}

void ActivityEngine::run(Vehicles &vehicles)
{
    SimTime sim_time = time_now();

    cout << "[*****SYSTEM*****] Activity Engine started: " << real_formatted_time_now() << "\n" << flush;

    // log for the number of Days specified at the initial running of Traffic
    stringstream msg;
    msg << "Started Activity Engine for number of days:" << simulate_days;
    logger.info(sim_time, ActivityLog( "NOTICE", "Activity Log", msg.str() ));

    generate_arrivals(vehicles);
    // TODO: time should be stepped in 1 minute blocks
    simulate_events();

    // TODO: program should give some indication as to what is happening, without being verbose

    cout << "[*****SYSTEM*****] Activity Engine finished: " << real_formatted_time_now() << "\n" << flush;
}

void ActivityEngine::generate_arrivals(Vehicles &vehicles)
{
    auto iter = vehicles.get_vehicles_dict()->begin();
    auto iter_end = vehicles.get_vehicles_dict()->end();

    for (; iter != iter_end; ++iter) {

        // TODO: Ensure the returned value from a normal distribution is not negative because of a large std. dev.
        // @param mean and std. dev
        normal_distribution<double> num_normal_distrib((*iter).second.num_mean, (*iter).second.num_stddev);
        normal_distribution<double> speed_normal_distrib((*iter).second.speed_mean, (*iter).second.speed_stddev);

        // Random number of X occurrences of vehicle over a day
        auto N = static_cast<unsigned int>(lround(num_normal_distrib(default_engine)));
        // rate of occurrence
        const double arrival_time_interval = 23.0F;
        double limit = (arrival_time_interval * 60.0F * 60.0F) - 1.0F;
        double lambda = N / limit;  // converted to seconds i.e. 1 occurrence per X seconds

        // TODO: debug
        // cout << "\nVehicle type: " << (*iter).first << " (" << N << ") [" << setprecision(6) << lambda << "]" << endl;

        double timestamps;
        vector<double> timestamps_list;
        exponential_distribution<double> expovariate(lambda);

        // TODO: Add references (TAOCP 2 and https://preshing.com/20111007/how-to-generate-random-timings-for-a-poisson-process/)
        // TODO: Another reference (https://keystrokecountdown.com/articles/poisson/index.html)
        /*
         * Generate the next arrival delta interval using an exponential distribution and the poisson process.
         * The random deltas, being a continuous distribution, may exceed the total time limit of arrivals up to
         * 23:00. To overcome this, run a loop up to a max 1000 times and make a vector of timestamps.
         * If the timestamps do not exceed the limit, and it matches the N number of vehicles to arrive on road,
         * then return the values and continue.
         * */
        for (int j = 0; j < 1000; j++) {
            timestamps = 0;
            do {
                double delta = expovariate(mersenne_twister_engine);
                timestamps += delta;
                if (timestamps < limit)
                    timestamps_list.push_back(timestamps);
            } while (timestamps < limit);

            if (timestamps_list.size() == N)
                break;
            else
                timestamps_list.clear();
        }

        // With a list of timestamps for this vehicle type, create vehicle stats
        for (double &timestamp : timestamps_list) {

            VehicleStats veh_stats;
            veh_stats.veh_name = (*iter).first;
            veh_stats.registration_id = Vehicles::generate_registration((*iter).second.reg_format, default_engine);

            // make sure it is non-negative because not possible
            do {
                veh_stats.arrival_speed = speed_normal_distrib(default_engine);
            } while (veh_stats.arrival_speed < 0);

            // add event to queue
            SimTime event_time;
            event_time = time_now();

            event_time.tm_hour = static_cast<int>(lround(timestamp)) / 60 / 60;
            event_time.tm_min = static_cast<int>(lround(timestamp)) / 60 % 60;
            event_time.tm_sec = static_cast<int>(lround(timestamp)) % 60;
            veh_stats.arrival_time = event_time;

            // set probability for other 3 events
            veh_stats.prob_parking = (!(*iter).second.parking_flag) ? 0 : 0.02;
            veh_stats.prob_end_exit = 0.5;

            // random binomial distribution true or false based on the probability of the event occurring
            veh_stats.prob_side_exit = 0.2;
            bernoulli_distribution side_exit_random(veh_stats.prob_side_exit);
            veh_stats.side_exit_flag = side_exit_random(default_engine);  // bernoulli_distribution implem returns a bool

            /*
             * If the vehicle is due to exit via a side road, generate an exit timestamp.
             * To do this, make a biased uniform distribution that only gives small values.
             * To run this through the exponential distribution, it will always do a ln of a value extremely
             * close to 1 and then divide that by a very small lambda. This means the vehicle will be biased
             * to exit the road very soon.
             * */
            SimTime exit_time(event_time);
            if (veh_stats.side_exit_flag) {
                double exit_interval = (limit + (60*60)) - timestamp;  // vehicles can exit after 24:00 so add an hour
                double exit_timestamp = timestamp;  // can only exit after arrival time

                while (exit_timestamp < (limit + (60*60))) {
                    long double test = biased_expovariate(1/exit_interval, 0, 0.02F);
                    if (exit_timestamp + test < (limit + (60*60))) {
                        exit_timestamp += test;
                        break;
                    }
                }

                exit_time.tm_hour = static_cast<int>(lround(exit_timestamp)) / 60 / 60;
                exit_time.tm_min = static_cast<int>(lround(exit_timestamp)) / 60 % 60;
                exit_time.tm_sec = static_cast<int>(lround(exit_timestamp)) % 60;
                Event exit_event({DEPART_SIDE_ROAD, exit_time, veh_stats} );
                event_q.push(exit_event);
            }


            Event arrival_event = { ARRIVAL, event_time, veh_stats };
            event_q.push(arrival_event);


            // TODO: debug
            /*cout << "Arrival: " << event_time.formatted_time() << " ==> " << veh_stats.registration_id << " "
                 << "(" << veh_stats.arrival_speed << " kmh)" << endl;

            if (veh_stats.side_exit_flag)
                cout << "Exit: " << exit_time.formatted_time() << " ==> " << veh_stats.registration_id << endl;*/
        }
    }
}

void ActivityEngine::simulate_events()
{
    /*int t;
    for (t = 0; t <= 1439; t++) {
        Event ev = event_q.top();  // returns a reference to the top element

        if ((ev.time.tm_hour * 60 + ev.time.tm_min) == t) {

        }

    }*/

    while (!event_q.empty()) {
        Event ev = event_q.top();  // returns a reference to the top event but does not remove it
        switch(ev.ev_type) {
            case ARRIVAL:
                veh_logger.info(ev.time, VehicleLog( ARRIVAL, "Vehicle Log", ev.stats.veh_name, ev.stats.registration_id,
                                       ev.stats.arrival_speed ));
                event_q.pop();  // returns void, but must remove from queue
                break;
            case DEPART_SIDE_ROAD:
                veh_logger.warning(ev.time, VehicleLog( DEPART_SIDE_ROAD, "Vehicle Log", ev.stats.veh_name,
                                              ev.stats.registration_id, -1 ));
                event_q.pop();
                break;
        }
    }

    // veh_logger.flush();
    // logger.flush();
}

/*
 * @brief: a biased function to return the variate from an exponential distribution with a lower and upper
 * bound that does not necessarily need to be [0, 1].
 *
 * @param rate_param: the rate of occurrence for the event.
 * @param lower_bound: the lower bound of the bias, must be >= 0.
 * @param upper_bound: the upper bound of the bias, must be <= 1.0.
 *
 * @return: the y value of the exponential distribution which is the likely next interval occurence
 *          of the event defined by the rate parameter or lambda.
 * */
long double ActivityEngine::biased_expovariate(double rate_param, double lower_bound, double upper_bound) {
    // TODO: consider logging critical event to file
    assert (lower_bound >= 0);
    assert (upper_bound <= 1.0F);
    uniform_real_distribution<double> biased_distribution(lower_bound, upper_bound);
    long double rand_val = biased_distribution(mersenne_twister_engine);
    while (rand_val == 1.0F)  // check if the random value is 1 to avoid passing 0 to logb()
        rand_val = biased_distribution(mersenne_twister_engine);
    return -logl(1.0F - rand_val) / rate_param;
}

/*
 * Function to the activity engine to set stats value to be used to
 * create a statistical simulation model
 *
 * @param days: number of days to run the simulation
 * @param vehicles_monitored: number of vehicle types to be monitored
 * @param road_len: length of the road in km
 * @param speed_lim: speed limit in km/h
 * @param parking_spots: number of parking spaces available
 * */
void ActivityEngine::set_statistics(unsigned days, unsigned vehicles_monitored, float road_len,
                                    float speed_lim, unsigned parking_spots)
{
    simulate_days = days;
    n_vehicles_monitored = vehicles_monitored;
    n_parking_spots = parking_spots;
    speed_limit = speed_lim;
    road_length = road_len;
}
