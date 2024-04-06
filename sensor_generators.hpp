#pragma once

#include "common.hpp"

enum DistributionType {
    UNIFORM = 1, CLUSTERED = 2, RANDOM = 3, EXPONENTIAL = 4
};

vector<Sensor> generateSensorsRandomly(int num_points);
vector<Sensor> generateSensorsUniformly(int num_points);
vector<Sensor> generateSensorsClustered(int num_points, bool exponential_mode = false);
vector<Sensor> generateSensors(DistributionType distributionChoice, int num_sensors, int seed);

void generateParticipantData(vector<Sensor> &sensors);
