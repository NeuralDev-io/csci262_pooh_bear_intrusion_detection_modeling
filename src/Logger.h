//
// Created by codeninja on 6/10/18.
//

#ifndef POOH_BEAR_INTRUSION_DETECTION_SYSTEM_LOGGER_H
#define POOH_BEAR_INTRUSION_DETECTION_SYSTEM_LOGGER_H

#include <string>
#include <map>
#include <time.h>
#include "Helper.h"
using namespace std;

enum LEVEL { NOTSET = 0, INFO = 10, DEBUG = 20, WARNING = 30, ERROR = 40, CRITICAL = 50 };
typedef enum LEVEL LEVEL;

typedef map<string, string>::iterator Config_Iter;
typedef map<string, string> Config_Dict;

class Logger {
public:
    Logger();  // default constructor
    Logger(string, string);  // default constructor with just filename
    Logger(string, LEVEL, string);
    Logger(string, LEVEL, string, bool);

    void set_level(LEVEL);
    void info(SimTime&, string);

    void debug(string);
    void warning(string);
    void error(string);
    void critical(string);

    const Config_Dict &get_config() const;
    string &get_logger_name();
    string &get_filename();
private:
    string _level_to_name(LEVEL);
    LEVEL _name_to_level(string&);
    Config_Dict config;
};

#endif //POOH_BEAR_INTRUSION_DETECTION_SYSTEM_LOGGER_H
