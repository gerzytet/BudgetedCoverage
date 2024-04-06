#include "sensor_movement.hpp"
#include "random_utils.hpp"

int generateStepSize() {
    return randint(0,3) * randint(0, 11);
}

void moveParticipantsRandomly(vector<Sensor> &sensors) {
    for (Sensor& sensor : sensors) {
        int offsetX, offsetY;
        do {
            float angle = randfloat(0, 2 * M_PI);
            int step = generateStepSize();
            offsetX = step * cos(angle);
            offsetY = step * sin(angle);
        } while (!inRange(sensor.x + offsetX, sensor.y + offsetY));
        sensor.x += offsetX;
        sensor.y += offsetY;
    }
}

void moveParticicpants(MovementType movementType, vector<Sensor>& sensors) {
    switch (movementType) {
        case RANDOM_MOVEMENT:
            moveParticipantsRandomly(sensors);
            break;
    }
}
