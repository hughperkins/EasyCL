#include "StatefulTimer.h"

//#if (_MSC_VER == 1500 || _MSC_VER == 1600) // visual studio 2008 or 2010
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

#include <iostream>
#include <string>

using namespace std;

#undef VIRTUAL
#define VIRTUAL
#undef STATIC
#define STATIC
#define PUBLIC

namespace easycl {

bool StatefulTimer::enabled = false;

PUBLIC STATIC StatefulTimer *StatefulTimer::instance() {
    static StatefulTimer *_instance = new StatefulTimer();
    return _instance;
}
PUBLIC StatefulTimer::StatefulTimer() : 
		prefix("") {
    cout<< "statefultimer v0.7" << endl;
    last = getSystemMilliseconds();
//        enabled = false;
}
PUBLIC StatefulTimer::~StatefulTimer() {
    std::cout << "~StatefulTimer() StatefulTimer readings:" << std::endl;
    for(std::map< std::string, double >::iterator it = timeByState.begin(); it != timeByState.end(); it++) {
        std::cout << "   " << it->first << ": " << it->second << std::endl;
    }
}
void StatefulTimer::_dump() {
	_dump(false);
}
void StatefulTimer::_dump(bool force) {
    cout << "dump enabled=" << enabled << endl;
    if(!enabled) {
        return;
    }
    double totalTimings = 0;
    for(std::map< std::string, double >::iterator it = timeByState.begin(); it != timeByState.end(); it++) {
//            std::cout << "   " << it->first << ": " << it->second << std::endl;
        totalTimings += it->second;
    }
    if(!force && totalTimings < 800) {
        return;
    }
    std::cout << "StatefulTimer readings:" << std::endl;
    for(std::map< std::string, double >::iterator it = timeByState.begin(); it != timeByState.end(); it++) {
        if(it->second > 0) {
            std::cout << "   " << it->first << ": " << it->second << "ms" << " count=" << countByState[it->first] << std::endl;
        }
    }
    timeByState.clear();
    countByState.clear();
}
PUBLIC STATIC void StatefulTimer::setPrefix(const char *_prefix) {
    if(instance()->enabled) {
        instance()->prefix = _prefix;
    }
}
PUBLIC STATIC void StatefulTimer::setPrefix(std::string _prefix) {
    if(instance()->enabled) {
        instance()->prefix = _prefix;
    }
}
PUBLIC STATIC void StatefulTimer::setEnabled(bool _enabled) {
    enabled = _enabled;
}
PUBLIC STATIC void StatefulTimer::dump() {
	dump(false);
}
PUBLIC STATIC void StatefulTimer::dump(bool force) {
    instance()->_dump(force);
}
PUBLIC STATIC void StatefulTimer::timeCheck(std::string state) {
    if(enabled) {
       instance()->_timeCheck(state);
    }
}
PUBLIC STATIC void StatefulTimer::timeCheck(const char *state) {
    if(enabled) {
       instance()->_timeCheck(state);
    }
}
PUBLIC STATIC double StatefulTimer::getSystemMilliseconds() {
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
void StatefulTimer::_timeCheck(std::string _state) {
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
void StatefulTimer::_timeCheck(const char *_state) {
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
}
