#include <iostream>
#include <iomanip>
#include <stdexcept>

#include "asserts.h"

using namespace std;

void assertEquals(float one, int two) {
    if(one != two) {
        throw std::runtime_error("FAIL " + toString(one) + " != " + toString(two) );
    }
}

void assertEquals(int one, float two) {
    if(one != two) {
        throw std::runtime_error("FAIL " + toString(one) + " != " + toString(two) );
    }
}

