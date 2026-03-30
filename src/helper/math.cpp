#include "math.h"
// #include <algorithm>

template<class T>
const T& clamp(const T& value, const T& floor, const T& ceil){
    return std::min(ceil, std::max(value, floor));
}