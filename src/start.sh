#!/usr/bin/env sh

CC -std=c++11 Utils.cpp Logger.cpp Vehicles.cpp ActivityEngine.cpp AnalysisEngine.cpp AlertEngine.cpp main.cpp -o Traffic
