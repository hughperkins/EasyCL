#pragma once

#include <iostream>
#include <iomanip>
#include <sstream>

#include "gtest/gtest.h"

template<typename T>
std::string toString(T val) { // not terribly efficient, but works...
   std::ostringstream myostringstream;
   myostringstream << std::fixed << std::setprecision(4) << val;
   return myostringstream.str();
}

template<typename T>
void assertEquals(T one, T two) {
    if(one != two) {
        throw std::runtime_error("FAIL " + toString(one) + " != " + toString(two) );
    }
}

void assertEquals(float one, int two);
void assertEquals(int one, float two);

inline ::testing::AssertionResult AssertFloatsNear(const char *expr_one, const char *expr_two,
    float one, float two) {
    float diff = one - two;
    float absdiff = diff > 0 ? diff : - diff;
    float absone = one > 0 ? one : -one;
    if(absdiff <= absone * 0.0001f) {
        return ::testing::AssertionSuccess();
    }
    std::string onestr = toString(one);
    std::string twostr = toString(two);
    return ::testing::AssertionFailure()
        << expr_one << " and " << expr_two << " differ: " << onestr << " vs " << twostr;
} 

#define EXPECT_FLOAT_NEAR(one, two) EXPECT_PRED_FORMAT2(AssertFloatsNear, one, two)
#define ASSERT_FLOAT_NEAR(one, two) ASSERT_PRED_FORMAT2(AssertFloatsNear, one, two)


