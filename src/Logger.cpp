/*********************************************************************************
* CSCI262 (Spring 2018) - Assignment 03
* Pooh Bear Intrusion Detection System Logger.cpp
* Purpose: Implementation for Logger package class for a logging system.
*          Based heavily on:
 *         https://github.com/python/cpython/blob/3.7/Lib/logging/__init__.py
*
* @version 0.2-dev
* @date 2018.10.07
*
* @authors Dinh Che (codeninja55) & Duong Le (daltonle)
* Emails: andrew at codeninja55.me & duong.daltonle at gmail.com
* StudentsS Dinh Che (5721970 | dbac496) & Duong Le (5560536 | ndl991)
*********************************************************************************/

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

void Logger::_log(LEVEL level, SimTime &time, EVENT_TYPE &ev_type, string msg)
{
    if (get("STDOUT") == "true") {
        cout << "<" << formatted_date(time) << " " << formatted_time(time) << ">:"
             << get("LOGGER") << ":" << _level_to_name(level) << ":"
             << event_name(ev_type) << ":" << msg << endl;
    }

    if (get("FILENAME") != "false") {
        ofstream fout(get("FILENAME").c_str(), ofstream::out | ofstream::app);
        if (!fout.good())
            cout << "[!!] Failed to open log file " << get("FILENAME");

        fout << "<" << formatted_date(time) << " " << formatted_time(time) << ">:"
             << get("LOGGER") << ":" << _level_to_name(level) << ":"
             << event_name(ev_type) << ":" << msg << endl;

        fout.close();
    }
}

void Logger::info(SimTime &time, EVENT_TYPE ev_type, string msg)
{
    // Check if info is allowed with the current level
    _log(INFO, time, ev_type, msg);
}

void Logger::debug(SimTime &time, EVENT_TYPE ev_type, string msg)
{
    // Check if info is allowed with the current level
    _log(DEBUG, time, ev_type, msg);
}

void Logger::warning(SimTime &time, EVENT_TYPE ev_type, string msg)
{
    // Check if info is allowed with the current level
    _log(WARNING, time, ev_type, msg);
}

void Logger::error(SimTime &time, EVENT_TYPE ev_type, string msg)
{
    // Check if info is allowed with the current level
    _log(ERROR, time, ev_type, msg);
}

void Logger::critical(SimTime &time, EVENT_TYPE ev_type, string msg)
{
    // Check if info is allowed with the current level
    _log(CRITICAL, time, ev_type, msg);
}

void Logger::set_level(LEVEL level)
{
    Config_Iter iter = config.find("LEVEL");
    if (iter != config.end())
        (*iter).second = _level_to_name(level);
}

const Config_Dict &Logger::get_config() const { return config; }

string &Logger::get(string key)
{
    Config_Iter iter = config.find(key);
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
