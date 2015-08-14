// Copyright Hugh Perkins 2014 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <iostream>

//#if (_MSC_VER == 1500 || _MSC_VER == 1600 ) // visual studio 2008 or 2010
#ifdef _MSC_VER // make consistent across all msvc versions, so dont have to retest on different msvc versions...
#define WINNOCHRONO
//#include <ctime>
#define NOMINMAX // prevents errors compiling std::max and std::min
#include <Windows.h>
#else
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
//#include <chrono>
#endif

#include <vector>
#include <map>
#include <string>

class StatefulTimer {
public:
    static StatefulTimer *instance() {
        static StatefulTimer *_instance = new StatefulTimer();
        return _instance;
    }

    static bool enabled;
    double last;

    std::map< std::string, double > timeByState;
    std::map< std::string, int > countByState;
    std::string prefix;

    StatefulTimer() : prefix("") {
//      std::cout<< "statefultimer v0.6" << std::endl;
        last = getSystemMilliseconds();
//        enabled = false;
    }
    ~StatefulTimer() {
        std::cout << "StatefulTimer readings:" << std::endl;
        for( std::map< std::string, double >::iterator it = timeByState.begin(); it != timeByState.end(); it++ ) {
            std::cout << "   " << it->first << ": " << it->second << std::endl;
        }
    }
    void _dump(bool force = false) {
        if(!enabled) {
            return;
        }
        double totalTimings = 0;
        for( std::map< std::string, double >::iterator it = timeByState.begin(); it != timeByState.end(); it++ ) {
//            std::cout << "   " << it->first << ": " << it->second << std::endl;
            totalTimings += it->second;
        }
        if( !force && totalTimings < 800 ) {
            return;
        }
        std::cout << "StatefulTimer readings:" << std::endl;
        for( std::map< std::string, double >::iterator it = timeByState.begin(); it != timeByState.end(); it++ ) {
            if( it->second > 0 ) {
                std::cout << "   " << it->first << ": " << it->second << "ms" << " count=" << countByState[it->first] << std::endl;
            }
        }
        timeByState.clear();
        countByState.clear();
    }
    static void setPrefix(const char *_prefix) {
        if(instance()->enabled) {
            instance()->prefix = _prefix;
        }
    }
    static void setPrefix(std::string _prefix) {
        if(instance()->enabled) {
            instance()->prefix = _prefix;
        }
    }
    static void setEnabled(bool _enabled) {
        enabled = _enabled;
    }
    static void dump(bool force = false) {
        instance()->_dump(force);
    }
    static void timeCheck(std::string state ) {
        if(enabled) {
           instance()->_timeCheck( state );
        }
    }
    static void timeCheck(const char *state ) {
        if(enabled) {
           instance()->_timeCheck( state );
        }
    }
    static double getSystemMilliseconds() {
        // ok I fought for ages with chrono, but vs2010 doesnt support chrono anyway
        // so lets use the simpler normal functions :-P
        #ifdef WINNOCHRONO
          DWORD thistime = timeGetTime();
          return thistime;
        #else // linux etc
          struct timeval now;
          gettimeofday(&now, NULL);
          double mtime = now.tv_sec * 1000.0 + now.tv_usec/1000.0;
          return mtime;
        #endif
    }
    void _timeCheck(std::string _state) {
        if(!enabled) {
            return;
        }
        std::string state = prefix + _state;
        double now = getSystemMilliseconds();
        //std::cout << "now " << now << std::endl;
        double change = now - last;
        //std::cout << "change " << change << std::endl;
        timeByState[state] += change;
        countByState[state]++;
        last = now;
	}
    void _timeCheck(const char *_state) {
        if(!enabled) {
            return;
        }
        std::string state = prefix + _state;
        double now = getSystemMilliseconds();
        //std::cout << "now " << now << std::endl;
        double change = now - last;
        //std::cout << "change " << change << std::endl;
        timeByState[state] += change;
        countByState[state]++;
        last = now;
    }
};

