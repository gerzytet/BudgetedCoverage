#include <iostream>
#include <random>
#include <iomanip>
#include <fstream>
#include <set>

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
const int R = 20;

set<int> chooseSensorsRandomly(int amount) {
    set<int> chosen;
    for (int i = 0; i < amount; i++) {
        int index = randint(0, NUM_POINTS - 1);
        while (chosen.count(index) != 0) {
            index = randint(0, NUM_POINTS - 1);
        }
        chosen.insert(index);
    }
    return chosen;
}

int main() 
{
    //NO COUT
    ofstream output;
    output.open ("output.txt", ofstream::out | ofstream::trunc); //truncate (erase) previous contents of the output file
    output << "Sensors:\n";
    for (int i = 0; i < NUM_POINTS; i++)
    {
        sensors.push_back(Sensor(randint(0, 100), randint(0, 100)));
    }
    
    for (Sensor &s : sensors)
    {
        output << '(' << s.x << ", " << s.y << ")\n";
    }

    output << endl;

    output << "Chosen:\n";
    for (int index : chooseSensorsRandomly(10)) {
        output << index << '\n';
    }

    output << "R:\n";
    output << R << '\n';
}