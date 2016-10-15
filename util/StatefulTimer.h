// Copyright Hugh Perkins 2014 hughperkins at gmail
//
// This Source Code Form is subject to the terms of the Mozilla Public License, 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <iostream>

#include <vector>
#include <map>
#include <string>

#include "EasyCL_export.h"

#define VIRTUAL virtual
#define STATIC static

namespace easycl {

class EasyCL_EXPORT StatefulTimer {
public:
#ifdef _WIN32
#pragma warning(disable: 4251)
#endif
    std::string prefix;
#ifdef _WIN32
#pragma warning(default: 4251)
#endif
    static bool enabled;

private:
    double last;

#ifdef _WIN32
#pragma warning(disable: 4251)
#endif
    std::map< std::string, double > timeByState;
    std::map< std::string, int > countByState;
#ifdef _WIN32
#pragma warning(default: 4251)
#endif

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
    STATIC void timeCheck(std::string state);
    STATIC void timeCheck(const char *state);
    STATIC double getSystemMilliseconds();

    private:
    void _dump();
    void _dump(bool force);
    void _timeCheck(std::string _state);
    void _timeCheck(const char *_state);

	// [[[end]]]

};
}
