#include "EasyCL.h"
#include <iostream>
using namespace std;

#include "gtest/gtest.h"
using namespace easycl;

TEST( testDeviceInfo, basic ) {
    int numDevices = DevicesInfo::getNumDevices();
    for( int i = 0; i < numDevices; i++ ) {
        cout << DevicesInfo::getDeviceInfo(i).toString() << endl;
    }
}

