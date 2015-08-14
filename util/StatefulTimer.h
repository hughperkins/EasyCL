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

#include "EasyCL_export.h"

#define VIRTUAL virtual
#define STATIC static

class EasyCL_EXPORT StatefulTimer {
private:
    static bool enabled;
    double last;

    std::map< std::string, double > timeByState;
    std::map< std::string, int > countByState;
    std::string prefix;

    // [[[cog
    // import cog_addheaders
    // cog_addheaders.addv2()
    // ]]]
    // generated, using cog:

    public:
    STATIC StatefulTimer *instance();
    StatefulTimer();
    ~StatefulTimer();
    STATIC void setPrefix(const char *_prefix);
    STATIC void setPrefix(std::string _prefix);
    STATIC void setEnabled(bool _enabled);
    STATIC void dump();
    STATIC void dump(bool force);
    STATIC void timeCheck(std::string state );
    STATIC void timeCheck(const char *state );
    STATIC double getSystemMilliseconds();

    private:
    void _dump();
    void _dump(bool force);
    void _timeCheck(std::string _state);
    void _timeCheck(const char *_state);

	// [[[end]]]

};

