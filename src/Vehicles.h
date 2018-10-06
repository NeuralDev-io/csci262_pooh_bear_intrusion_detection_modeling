//
// Created by codeninja on 6/10/18.
//

#ifndef TRAFFIC_ACTIVITY_ENGINE_VEHICLES_H
#define TRAFFIC_ACTIVITY_ENGINE_VEHICLES_H

#include <string>
#include <map>
using namespace std;

typedef struct {
    int id;
    string name, reg_format;
    bool parking;
    unsigned int vol_weight, speed_weight;
} Vehicle;

class Vehicles {
public:
    Vehicles() : n_vehicles(0) {};
    void insert(Vehicle &v);
private:
    unsigned int n_vehicles;
    map<int, Vehicle, less<int> > vehicles_dict;
};

#endif //TRAFFIC_ACTIVITY_ENGINE_VEHICLES_H
