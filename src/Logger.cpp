//
// Created by codeninja on 6/10/18.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include "Logger.h"

/*
 * TODO:
 * - Ensure level is_enabled_for(level)
 * - Threads
 * */

Logger::Logger()
{
    config.insert(pair<string, string>("LOGGER", "Default Logger"));
    config.insert(pair<string, string>("LEVEL", _level_to_name(WARNING)));
    config.insert(pair<string, string>("FILENAME", "false"));
    // TODO: consider what to do if you don't want to print to stdout
    config.insert(pair<string, string>("STDOUT", "true"));
}

Logger::Logger(string logger_name, string filename)
{
    stringstream ss;
    ss << "logs/" << filename;
    config.insert(pair<string, string>("LOGGER", logger_name));
    config.insert(pair<string, string>("LEVEL", _level_to_name(WARNING)));
    config.insert(pair<string, string>("FILENAME", ss.str()));
    config.insert(pair<string, string>("STDOUT", "false"));
}

Logger::Logger(string logger_name, LEVEL level, string filename)
{
    stringstream ss;
    ss << "logs/" << filename;
    config.insert(pair<string, string>("LOGGER", logger_name));
    config.insert(pair<string, string>("LEVEL", _level_to_name(level)));
    config.insert(pair<string, string>("FILENAME", ss.str()));
    config.insert(pair<string, string>("STDOUT", "false"));
}

Logger::Logger(string logger_name, LEVEL level, string filename, bool std_out)
{
    stringstream ss;
    ss << "logs/" << filename;
    config.insert(pair<string, string>("LOGGER", logger_name));
    config.insert(pair<string, string>("LEVEL", _level_to_name(level)));
    config.insert(pair<string, string>("FILENAME", ss.str()));
    config.insert(pair<string, string>("STDOUT", (std_out) ? "true" : "false"));
}

void Logger::info(SimTime &time, string msg)
{
    cout << "<" << formatted_date(time) << " " << formatted_time(time) << ">:"
         << get_logger_name() << ":" << _level_to_name(INFO) << ":" << msg << endl;

    ofstream fout(get_filename().c_str(), ofstream::out | ofstream::app);
    if (!fout.good())
        cout << "[!!] Failed to open log file " << get_filename();

    fout << "<" << formatted_date(time) << " " << formatted_time(time) << ">:"
         << get_logger_name() << ":" << _level_to_name(INFO) << ":" << msg << endl;

    fout.close();
}

void Logger::debug(string msg)
{

}

void Logger::warning(string msg)
{

}

void Logger::error(string msg)
{

}

void Logger::critical(string msg)
{

}

void Logger::set_level(LEVEL level)
{
    Config_Iter iter = config.find("LEVEL");
    if (iter != config.end())
        (*iter).second = _level_to_name(level);
}

const Config_Dict &Logger::get_config() const {
    return config;
}

string &Logger::get_logger_name()
{
    Config_Iter iter = config.find("LOGGER");
    if (iter != config.end())
        return (*iter).second;
}

string &Logger::get_filename() {
    Config_Iter iter = config.find("FILENAME");
    if (iter != config.end())
        return (*iter).second;
}

LEVEL Logger::_name_to_level(string &level_str) {
    if (level_str == "NOTSET")
        return NOTSET;
    else if (level_str == "INFO")
        return INFO;
    else if (level_str == "DEBUG")
        return DEBUG;
    else if (level_str == "ERROR")
        return ERROR;
    else if (level_str == "CRITICAL")
        return CRITICAL;
    else
        return WARNING;
}

string Logger::_level_to_name(LEVEL level) {
    switch (level) {
        case NOTSET:
            return "NOTSET";
        case INFO:
            return "INFO";
        case DEBUG:
            return "DEBUG";
        case WARNING:
            return "WARNING";
        case ERROR:
            return "ERROR";
        case CRITICAL:
            return "CRITICAL";
        default:
            return "WARNING";
    }
}
