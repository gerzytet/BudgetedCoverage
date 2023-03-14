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
    int x, y, cost, coverage; //coverage: the number of other sensors within the current circle
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

double randnormal(double mean, double deviation) {
    normal_distribution<double> distribution(mean, deviation);
    return distribution(randomGenerator);
}

vector<Sensor> sensors;
const int NUM_POINTS = 40;
const int R = 20;
const int budget = 10000;

set<int> chooseSensorsRandomly() 
{    
    int totalCost = 0;
    set<int> chosen;
    int brokeCount = 0;

    while(chosen.size() < sensors.size() && brokeCount < 40)
    {
        int index = randint(0, NUM_POINTS - 1);
        while (chosen.count(index) != 0) 
        {
            index = randint(0, NUM_POINTS - 1);
        }

        if((totalCost + sensors[index].cost) > budget)
        {
            brokeCount++;
            continue;
        }        
        
        chosen.insert(index);
        totalCost += sensors[index].cost;
    }
    return chosen;
}

vector<Sensor> greedyAlgorithm()
{
    vector<Sensor> greedySort(sensors);
    vector<Sensor> chosen;
    int totalCost = 0;
    //Check greedySort.size first to prevent out of bounds error
    for (int i = 0; i < greedySort.size() && (totalCost + greedySort[i].cost) <= budget; ++i)
    {
        totalCost += sensors[i].cost;
        chosen.push_back(greedySort[i]);
    }
    return chosen;
}

double calculateDistance(Sensor s1, Sensor s2)
{
    return sqrt( pow(s2.x - s1.x, 2) + pow(s2.y - s1.y, 2) );
}

double calculateDistance(pair<int, int> p1, pair<int, int> p2)
{
    return sqrt( pow(p1.first - p2.first, 2) + pow(p2.second - p2.second, 2) );
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

vector<int> returnCoveredSensors(int index)
{
    vector<int> covered;    
    for(int j = 0; j < sensors.size(); j++)
    {
        //Don't count coverage for the sensor itself            
        if(index != j && calculateDistance(sensors[index], sensors[j]) < R) 
        {                                                            
            covered.push_back(j);                                                 
        }
    }        
    return covered;
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

vector<Sensor> sortSensors(vector<Sensor> s)
{
    //copy sensors array to new array for sorting
    vector<Sensor> sortedSensors(s); 

    //selection sort
    int i, j, min_idx; 
    for(int i = 0; i < sortedSensors.size()-1; i++)
    {
        min_idx = i; 
        for (j = i+1; j < sortedSensors.size(); j++)
        {
          if (sortedSensors[j].cost < sortedSensors[min_idx].cost) 
              min_idx = j;
        }
        // Swap the found minimum element 
        // with the first element 
        if (min_idx!=i)
        {        
            swap(sortedSensors[min_idx], sortedSensors[i]);                    
        }
    }
    return sortedSensors;
}

int randomCost() 
{
    return randint(250, 500);
}

void generateSensorsRandomly() 
{
    for (int i = 0; i < NUM_POINTS; i++)
    {
        sensors.push_back(Sensor(randint(0, 100), randint(0, 100), randomCost()));
    }
}

void generateSensorsUniformly() 
{
    for (int i = 0; i < 10; i++) 
    {
        for (int j = 0; j < 10; j++) 
        {
            if (i * 10 + j >= NUM_POINTS) 
            {
                return;
            }
            sensors.push_back(Sensor(1 * 20, j * 20, randomCost()));
        }
    }
}

const int NEIGHBORHOODS = 4;
void generateSensorsClustered() 
{
    vector<pair<int, int>> points;
    while (points.size() < NEIGHBORHOODS) 
    {
        pair<int, int> point = make_pair(randint(0, 100), randint(0, 100));
        bool tooClose = false;
        for (auto otherPoint : points) 
        {
            if (calculateDistance(point, otherPoint) < 40) 
            {
                tooClose = true;
                break;
            }
        }

        if (tooClose) 
        {
            continue;
        }
    }

    for (auto point : points) 
    {
        int mean = randint(200, 400);
        int deviation = 8;
        for (int i = 0; i < 25; i++) 
        {
            int x = randint(max(point.first - 20, 0), min(point.first + 20, 100));
            int y = randint(max(point.second - 20, 0), min(point.second + 20, 100));
            sensors.push_back(Sensor(x, y, randnormal(mean, deviation)));
        }
    }
}

bool contains(vector<int> v, int find)
{
    for(int i = 0; i < v.size(); i++)
    {
        if(v[i] == find)
        return true;
    }
    return false;
}

void removeMutualCoverage(int index)
{   
    vector<int> sourceCoveredSensors = returnCoveredSensors(index);   
    vector<int> childCoveredSensors;        
    int mutualSensors = 0;                

    for(int sr : sourceCoveredSensors)
    {    
        childCoveredSensors = returnCoveredSensors(sr);            
        mutualSensors = 0;            

        for(int index : sourceCoveredSensors)
        {
            if(contains(childCoveredSensors, index))        
                mutualSensors++;        
        }              
        sensors[sr].coverage -= (1 + mutualSensors);
    }
    sensors[index].coverage = 0;
}

void removeMutualSensors(int origin) 
{
    vector<int> touched = returnCoveredSensors(origin);
    touched.push_back(origin);

    for (int inner : touched) 
    {
        for (int outer : returnCoveredSensors(inner)) 
        {    
            if (calculateDistance(sensors[outer], sensors[inner]) < R) 
            {
                sensors[outer].coverage--;
            }
        }
    }
}

int main() 
{    
    //NO COUT        
    int algorithmChoice = -1;
    int distributionChoice = -1;

    std::cout << "Which algorithm would you like to use?\n1. Greedy Algorithm\n2. Random Algorithm\n3. Budgeted Algorithm\n";        
    std::cin >> algorithmChoice;

    cout << endl;

    cout << "Choose a distribution\n1. Uniform\n2. Clustered\n3. Random\n";
    cin >> distributionChoice;

    cout << endl;

    ofstream output;
    output.open ("output.txt", ofstream::out | ofstream::trunc); //truncate (erase) previous contents of the output file
    output << "Sensors:\n";

    if(distributionChoice == 1)
    {
        generateSensorsUniformly();
    }
    else if(distributionChoice == 2)
    {
        generateSensorsClustered();
    }
    else if(distributionChoice == 3)
    {
        generateSensorsRandomly();
    }

    // for (int i = 0; i < NUM_POINTS; i++)
    // {
    //     sensors.push_back(Sensor(randint(0, 100), randint(0, 100), randint(250, 500)));        
    // }        
    
    sensors = sortSensors(sensors);    
    calculateCoverage();

    for (Sensor &s : sensors)
    {
        output << '(' << s.x << ", " << s.y << ", " << s.cost << ")\n";
    }
    output << endl;    
    output << "Chosen:\n";

    int index = 0;
    int totalCost = 0;
    int totalCoverage = 0;
    if(algorithmChoice == 1)
    {
        for(Sensor s : greedyAlgorithm())
        {
            output << index << '\n';
            index++;
            totalCost += s.cost;
            totalCoverage += s.coverage;
        }
    }
    else if(algorithmChoice == 2)
    {
        for (int index : chooseSensorsRandomly()) 
        {
            output << index << '\n';
            totalCost += sensors[index].cost;
            totalCoverage += sensors[index].coverage;            
        }        
    }    
    else if(algorithmChoice == 3)
    {
        
    }             

    std::cout << "Total Cost: " << totalCost;
    std::cout << "\nTotal Coverage: " << totalCoverage << endl;

    output << endl;

    output << "R:\n";
    output << R << '\n';       
}

//for (Sensor s : sensors) 
    // {
    //     std::cout << s.cost << ' ';
    // }

// cout << "Covered Sensors for sensor at x=" << sensors[20].x << ", y=" << sensors[20].y;
    // cout << "\n";
    // for(int i : returnCoveredSensors(20))
    // {
    //     cout << "Sensor at x=" << sensors[i].x << ", y=" << sensors[i].y << "\n";
    // }        

    //sensors.push_back({1,10,0});
    // sensors.push_back({15,10,0});
    // sensors.push_back({30,10,0});
    // sensors.push_back({43,10,0});

// std::cout << endl;
//     int i = 0;
//     for(Sensor s : sensors)
//     {
//         if(sensors[i].y == 10)
//         std::cout << "10 y found at index: " << i << '\n';
//         i++;
//     }
//     std::cout << endl << endl;

//     for(int i = 0; i < 4; i++)    
//     {
//         std::cout << "\nCoverage of sensor (" << i << ") at x=" << sensors[i].x << ", y=" << sensors[i].y 
//         << " before method: " << sensors[i].coverage;
//     }
//     std::cout << endl;

//     processChosenSensor(1);
//     for(int i = 0; i < 4; i++)    
//     {
//         std::cout << "\nCoverage of sensor (" << i << ") at x=" << sensors[i].x << ", y=" << sensors[i].y 
//         << " after method: " << sensors[i].coverage;
//     }
//     std::cout << endl << endl;