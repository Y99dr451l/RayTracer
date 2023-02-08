#ifndef UTILS_H
#define UTILS_H
#include <random>

const double Pi = 3.1415926535897932385;
const double infinity = std::numeric_limits<double>::infinity();

inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}
inline double random_double(double min, double max) {return min + (max-min)*random_double();}
inline double clamp(double x, double min, double max) {if (x < min) return min; if (x > max) return max; return x;}

#endif