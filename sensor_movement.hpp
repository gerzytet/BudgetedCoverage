#pragma once

#include "common.hpp"

enum MovementType {
    RANDOM_MOVEMENT
};

void moveParticicpants(MovementType movementType, vector<Sensor>& sensors);
