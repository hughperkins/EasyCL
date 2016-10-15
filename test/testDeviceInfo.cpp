#include "EasyCL.h"
#include <iostream>
using namespace std;

#include "gtest/gtest.h"
using namespace easycl;

TEST(testDeviceInfo, basic) {
    int numDevices = DevicesInfo::getNumDevices();
    cout << "numdevices " << numDevices << endl;
    for(int i = 0; i < numDevices; i++) {
        cout << DevicesInfo::getDeviceInfo(i).toString() << endl;
    }
}

TEST(testDeviceInfo, gpus) {
    int numGpus = DevicesInfo::getNumGpus();
    for(int i = 0; i < numGpus; i++) {
        cout << DevicesInfo::getGpuInfo(i).toString() << endl;
    }
}

