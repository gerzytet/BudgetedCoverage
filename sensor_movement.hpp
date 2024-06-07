#pragma once

#include "common.hpp"

enum MovementType {
    RANDOM_MOVEMENT, SUMO_MOVEMENT
};

using Trajectories = vector<vector<pair<int, int>>>;

void moveAlongTrajectories(vector<Sensor> &sensors, Trajectories &t, int t_index);
Trajectories generateRandomTrajectories(const vector<Sensor> &starting_sensors, int rounds);
Trajectories generateSumoTrajectories(int num_participants, int rounds, int seed);
