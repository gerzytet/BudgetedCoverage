#include <iostream>
#include <random>
#include <iomanip>

using namespace std;

struct Sensor 
{
    double x, y;
    
    Sensor(double x, double y): x(x), y(y) 
    {
        
    }
};

minstd_rand randomGenerator(1);

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

vector<Sensor> sensors;
const int NUM_POINTS = 40;

int main() 
{
    for (int i = 0; i < NUM_POINTS; i++) 
    {
        sensors.push_back(Point(randfloat(-5, 5), randfloat(-5, 5)));
    }
    
    for (Sensor &s : sensors)
    {
        cout << setprecision(5) << '(' << s.x << ", " << s.y << ")\n";
    }
}