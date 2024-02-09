#include "random_utils.hpp"

long long nanos = std::chrono::steady_clock::now().time_since_epoch().count();
minstd_rand randomGenerator(nanos);
void seedRandom(int seed) {
    randomGenerator.seed(seed);
}

int randint(int a, int b)
{
    uniform_int_distribution<int> distribution(a, b);
    return distribution(randomGenerator);
}

double randfloat(double a, double b)
{
    uniform_real_distribution<double> distribution(a, b);
    return distribution(randomGenerator);
}

double randnormal(double mean, double deviation) {
    normal_distribution<double> distribution(mean, deviation);
    return distribution(randomGenerator);
}
