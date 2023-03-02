#include <iostream>
#include <random>
#include <iomanip>
#include <fstream>
#include <set>
#include <cmath>

using namespace std;

//To run this file in VS Code
//Open Developer console
//Type code -> enter

//Struct variables are public by default instead of private
struct Sensor 
{
    int x, y, cost, coverage; //the number of other sensors within the current circle
    double weight;

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



set<int> chooseCheapestSensors()
{
    set<int> chosen;
    int totalCost = 0;
    for (int i = 0; budget >= sensors[i].cost; ++i)
    {
        totalCost += sensors[i].cost;
        chosen.insert(i);
    }
    return chosen;
}

double calculateDistance(Sensor s1, Sensor s2)
{
    return sqrt( pow(s2.x - s1.x, 2) + pow(s2.y - s1.y, 2) );
}

void calculateCoverage()
{
    for(int i = 0; i < sensors.size(); i++)
    {
        for(int j = 0; j < sensors.size(); j++)
        {
            //Don't count coverage for the sensor itself            
            if(i!=j && calculateDistance(sensors[i], sensors[j]) < R) 
            {                            
                sensors[i].coverage++;                
            }
        }
    }
}

/*
Formula: 
give weight to each sensor (coverage/cost placeholder)
sort sensors based on weight
function picks highest weight first, then goes down
skip if under budget

what if sensors already covered? create bool covered[]
*/

void assignWeight(vector<Sensor> sensors)
{
    for(int i = 0; i < sensors.size(); i++)
    {
        sensors[i].weight = (double)sensors[i].coverage/sensors[i].cost;
    }
}

void sortSensors(vector<Sensor> sensors)
{
    //copy sensors array to new array for sorting
    vector<Sensor> sortedSensors(sensors); 

    //selection sort
    int i, j, min_idx; 
    for(int i = 0; i < sortedSensors.size()-1; i++)
    {
        min_idx = i; 
        for (j = i+1; j < sortedSensors.size(); j++)
        {
          if (sortedSensors[j].weight < sortedSensors[min_idx].weight) 
              min_idx = j;
        }
        // Swap the found minimum element 
        // with the first element 
        if (min_idx!=i)
        {        
            double temp = sortedSensors[min_idx].weight;
            sortedSensors[min_idx].weight = sortedSensors[i].weight;
            sortedSensors[i].weight = temp;                        
        }
    }
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