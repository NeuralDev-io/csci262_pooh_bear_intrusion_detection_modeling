# Pooh Bear Intrusion Detection System

CSCI262 System Security (Spring 2018) assignment 3 - intrusion detection system statistical modeling and simulation.

| Authors                         | Contact                                                 |
| ------------------------------- | ------------------------------------------------------- |
| Andrew (Dinh) Che (codeninja55) | [dbac496@uowmail.edu.au](mailto:dbac496@uowmail.edu.au) |
| Duong Le (daltonle)             | [ndl991@uowmail.edu.au](mailto:ndl991@uowmail.edu.au)   |

## Preface

### Overview

Pooh Bear Intrusion Detection System (IDS) is a honeypot event modeller and IDS/auditor system written in C++11. The system reads in vehicles information and traffic statistics and produce events consistent with this data. The collection of generated events forms the baseline data for the system, which are analysed to generate the baseline statistics. In the last phase, "live data" is compared to the baseline statistics to identify anomalies in the system.

### Logging system

A logging module is used consistently across the whole system. Following are 5 logging levels in the order of increasing severity.

| Level    | Description                                                  |
| -------- | ------------------------------------------------------------ |
| NOTSET   | No level has been set.                                       |
| INFO     | Confirmation that things are working as expected.            |
| DEBUG    | Detailed information, typically of interest only when diagnosing problems. |
| WARNING  | An indication that something unexpected happened, or indicative of some problem in the near future (e.g. ‘disk space low’). The software is still working as expected. |
| ERROR    | Due to a more serious problem, the software has not been able to perform some function. |
| CRITICAL | A serious error, indicating that the program itself may be unable to continue running. |

### Files

#### Directory

Log files are stored in `dir/logs/` folder and data generated from the Analysis Engine is stored in `dir/data/`.

#### Suffices

Log files and data files generated from statistics of initial input are succeeded with a suffix `_baseline`.

Each time live data is inputed by a user, the log and data files generated from this process are followed by a positive integer incrementing from 1, for example, `data_1, logs_1`, `data_2, logs_2`,... 

## Initial input

### Internal data storage

### Data inconsistencies

## Activity Engine

### Event generation

### Errors

## Analysis Engine

## Alert Engine



