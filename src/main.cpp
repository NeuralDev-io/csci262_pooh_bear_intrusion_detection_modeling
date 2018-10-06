#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err34-c"
/*********************************************************************
* CSCI262 - Assignment 03
* main.cpp
*    - C++ main() driver for implementation of specifications
*
* Last modified: 2018.10.06
*
* Authors (Github) | email:
*   - Dinh Che (codeninja55) | andrew at codeninja55.me
*   - Duong Le (daltonle) |  duong.daltonle at gmail.com
* UOW Details:
*   - Dinh Che (5721970) | dbac496 at uowmail.edu.au
*   - Duong Le (5560536) | ndl991 at uowmail.edu.au
*********************************************************************/

#include <iostream>
#include <cstdlib>
#include <limits.h>
#include <string.h>

using namespace std;

#define BUFFER_SZ 100

int main(int argc, char * argv[]) {

    char days_str[sizeof(int)];
    string vehicles_file, stats_file;
    int days = 0;
    char *end;

    // check the correct amount of args has been passed
    if (argc < 4 || argc > 4) {
        cout << "Usage: Traffic [VEHICLES FILE] [STATS FILE] [DAYS]\n" << flush;
        exit(0);
    } else if (argc == 4) {  // correct number of args passed
        vehicles_file = argv[1];
        stats_file = argv[2];
        // attempting to do int conversion from args safely
        strncpy(days_str, argv[3], sizeof(int));
        long tmp = strtol(days_str, &end, sizeof(long));
        if (tmp >= INT_MIN && tmp <= INT_MAX)
            days = static_cast<int>(tmp);
        else {
            cout << "[!!] Invalid number used for number of days. \nExiting..." << endl;
            exit(1);
        }
    }



    return 0;
}
