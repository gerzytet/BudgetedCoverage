#include <iostream>
#include <random>
#include <iomanip>
#include <fstream>
#include <set>

using namespace std;

//To run this file in VS Code
//Open Developer console
//Type code -> enter

//Struct variables are public by default instead of private
struct Sensor 
{
    int x, y;
    int cost;
    int coverage; //the number of other sensors within the current circle
    
    Sensor(int x, int y, int cost): x(x), y(y), cost(cost), coverage(0) {} // Short for this.x = x this.y = y this.cost=cost   
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
const int budget = 10000;

set<int> chooseSensorsRandomly(int amount) 
{
    set<int> chosen;
    for (int i = 0; i < amount; i++) 
    {
        int index = randint(0, NUM_POINTS - 1);
        while (chosen.count(index) != 0) 
        {
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
        sensors.push_back(Sensor(randint(0, 100), randint(0, 100), randint(0, 100)));
    }
    
    for (Sensor &s : sensors)
    {
        output << '(' << s.x << ", " << s.y << ")\n";
    }

    output << endl;

    output << "Chosen:\n";
    for (int index : chooseSensorsRandomly(10)) 
    {
        output << index << '\n';
    }

    output << endl;

    output << "R:\n";
    output << R << '\n';
}