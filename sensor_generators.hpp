#pragma once

#include "common.hpp"

enum DistributionType {
    UNIFORM = 1, CLUSTERED = 2, RANDOM = 3
};

vector<Sensor> generateSensorsRandomly(int num_points);
vector<Sensor> generateSensorsUniformly(int num_points);
vector<Sensor> generateSensorsClustered(int num_points);
vector<Sensor> generateSensors(DistributionType distributionChoice, int num_sensors, int seed);
