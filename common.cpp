#include "common.hpp"
#include "random_utils.hpp"

double calculateDistance(Sensor s1, Sensor s2)
{
    return sqrt( pow(s2.x - s1.x, 2) + pow(s2.y - s1.y, 2) );
}

double calculateDistance(pair<int, int> p1, pair<int, int> p2)
{
    return sqrt( pow(p1.first - p2.first, 2) + pow(p1.second - p2.second, 2) );
}

//return all sensors that sensors[index] covers, NOT including itself
vector<int> returnCoveredSensors(const vector<Sensor> &sensors, int index, int R)
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

void calculateCoverage(vector<Sensor> &sensors, set<int> covered, int R)
{
    for (Sensor &s : sensors) {
        s.coverage = 0;
    }
    for(int i = 0; i < sensors.size(); i++)
    {
        if (covered.count(i) == 0) {
            sensors[i].coverage++;
        }
        for(int j = 0; j < sensors.size(); j++)
        {
            if (covered.count(j) == 1) {
                continue;
            }
            //Don't count coverage for the sensor itself
            if(i != j && calculateDistance(sensors[i], sensors[j]) < R)
            {
                sensors[i].coverage++;
            }
        }
    }
}

double Sensor::getROI() const {
    return double(total_reward + beta) / (t * times_participated + beta);
}

double Sensor::getExpectedROI(double maxWinningBid) {
    return double(total_reward + beta + maxWinningBid) / (t * (times_participated + 1) + beta);
}

void Sensor::markRoundResult(int reward) {
    this->total_reward += reward;
    this->times_participated++;
}

void calculateCoverage(vector<Sensor> &sensors, int R) {
    set<int> covered;
    calculateCoverage(sensors, covered, R);
}

bool inRange(int x, int y) {
    return x >= 0 && x <= MAX_COORDINATE && y >= 0 && y <= MAX_COORDINATE;
}
