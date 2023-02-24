#include <iostream>
#include <random>
#include <iomanip>

using namespace std;

//Struct variables are public by default instead of private
struct Sensor 
{
    int x, y;
    
    Sensor(int x, int y): x(x), y(y) {} //Short for this.x = x this.y = y    
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
        sensors.push_back(Sensor(randint(0, 100), randint(0, 100)));
    }
    
    for (Sensor &s : sensors)
    {
        cout << setprecision(5) << '(' << s.x << ", " << s.y << ")\n";
    }
}