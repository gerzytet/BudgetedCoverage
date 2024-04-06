#pragma once

#include <iostream>
#include <random>
#include <iomanip>
#include <fstream>
#include <set>
#include <cmath>
#include <chrono>
#include <map>
#include <unordered_map>
#include <bitset>
#include <algorithm>
#include <memory>

using std::vector;
using std::bitset;
using std::minstd_rand;
using std::uniform_int_distribution;
using std::uniform_real_distribution;
using std::normal_distribution;
using std::set;
using std::cout;
using std::cin;
using std::pair;
using std::unordered_map;
using std::tuple;
using std::ofstream;
using std::to_string;
using std::endl;
using std::swap;
using std::make_pair;
using std::max;
using std::min;
using std::string;
using std::sort;
using std::ifstream;
using std::unique_ptr;

//Struct variables are public by default instead of private
struct Sensor
{
    int x, y, cost, coverage; //coverage: the number of other sensors within the current circle
    double weight;
    int i;

    Sensor(int x, int y, int cost, int i): x(x), y(y), cost(cost), coverage(0), i(i) {} // Short for this.x = x this.y = y this.cost=cost
    double getROI() const;
    double getExpectedROI(double maxWinningBid);
    void markRoundResult(int reward);

    //PARTICIANT PARAMS
    float beta = 0; //more = slower to change ROI
    int t = 0; //true valuation
    int total_reward = 0;
    int times_participated = 0;
    bool is_participating = true;
};
using bs = bitset<128>;
const int MAX_COORDINATE = 100;

double calculateDistance(Sensor s1, Sensor s2);
double calculateDistance(pair<int, int> p1, pair<int, int> p2);
vector<int> returnCoveredSensors(const vector<Sensor> &sensors, int index, int R);
void calculateCoverage(vector<Sensor> &sensors, set<int> covered, int R);
void calculateCoverage(vector<Sensor> &sensors, int R);
bool inRange(int x, int y);
