#pragma once

template<typename T>
static std::string toString(T val ) {
   std::ostringstream myostringstream;
   myostringstream << val;
   return myostringstream.str();
}


