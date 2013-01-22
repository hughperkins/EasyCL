#pragma once

template<typename T>
std::string toString(T val ) { // not terribly efficient, but works...
   std::ostringstream myostringstream;
   myostringstream << val;
   return myostringstream.str();
}

template<typename T>
void assertEquals( T one, T two ) {
    if( one != two ) {
        throw runtime_error("FAIL " + toString(one) + " != " + toString(two ) );
    }
}

void assertEquals( float one, int two ) {
    if( one != two ) {
        throw runtime_error("FAIL " + toString(one) + " != " + toString(two ) );
    }
}

void assertEquals( int one, float two ) {
    if( one != two ) {
        throw runtime_error("FAIL " + toString(one) + " != " + toString(two ) );
    }
}

