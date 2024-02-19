#pragma once
#include "common.hpp"

vector<int> chooseSensorsRandomly(const vector<Sensor> &sensors, int budget, int R);
vector<int> greedyAlgorithm(const vector<Sensor> &sensors, int budget, int R);
vector<int> weightedAlgorithm(vector<Sensor> &sensors, int budget, int R);
vector<int> bruteForceAlgorithm(const vector<Sensor> &sensors, int budget, int R);
vector<int> dynamicAlgorithm(const vector<Sensor> &sensors, int budget, int R);
vector<int> kGreedyAlgorithm(vector<Sensor> &sensors, int budget, int R, int k);
