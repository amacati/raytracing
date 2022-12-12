#ifndef UTILS_H_
#define UTILS_H_

#include <cmath>
#include <limits>
#include <memory>
#include <random>

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.;
}

inline double random_double() {
    thread_local static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    thread_local static std::default_random_engine re;
    return distribution(re);
}

inline double randn_double() {
    thread_local static std::normal_distribution<double> distribution;
    thread_local static std::default_random_engine re;
    return distribution(re);
}

inline double random_double(double min, double max) {
    return min + (max - min) * random_double();
}

inline double clamp(double x, double min, double max) {
    return (x < min ? min : (x > max ? max : x));
}

#endif