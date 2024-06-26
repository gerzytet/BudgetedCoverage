#include "sensor_movement.hpp"
#include "random_utils.hpp"

#include <sstream>

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

Trajectories generateRandomTrajectories(const vector<Sensor> &starting_sensors, int rounds) {
    vector<pair<int, int>> coordinates;
    for (const Sensor &sensor : starting_sensors) {
        coordinates.push_back({sensor.x, sensor.y});
    }

    Trajectories t;
    for (int i = 0; i < rounds; i++) {
        for (pair<int, int> &coord : coordinates) {
            int offsetX, offsetY;
            do {
                float angle = randfloat(0, 2 * M_PI);
                int step = generateStepSize();
                offsetX = step * cos(angle);
                offsetY = step * sin(angle);
            } while (!inRange(coord.first + offsetX, coord.second + offsetY));
            coord.first += offsetX;
            coord.second += offsetY;
        }
        t.push_back(coordinates);
    }

    return t;
}

Trajectories generateSumoTrajectories(int num_participants, int rounds, int seed) {
    system((string("cd \"sumo files\"; ./run.sh ") + to_string(num_participants) + " " + to_string(seed)).c_str());

    ifstream input;
    Trajectories t;

    input.open("sumo files/trajectories.txt");
    for (int i = 0; i < num_participants; i++) {
        vector<pair<float, float>> raw_trajectory;
        while (true) {
            string inputLine;
            getline(input, inputLine);
            if (inputLine == "\n" || inputLine == "") { //either we hit a delimeter between trajectories or end of file
                break;
            }

            std::stringstream s(inputLine);
            float x, y;
            s >> x >> y;
            raw_trajectory.push_back({x, y});
        }
        if (raw_trajectory.size() == 0) {
            cout << t.size() << '\n';
            cout << "Error: trajectory file generated by sumo doesn't contain enough participants.  Retrying...\n";
            t.clear();
            return generateSumoTrajectories(num_participants, rounds, seed);
        }

        //sumo simulation steps are small, so take every 5 points
        vector<pair<int, int>> trajectory;
        for (int i = 0; i < raw_trajectory.size(); i += 5) {
            trajectory.push_back({raw_trajectory[i].first, raw_trajectory[i].second});
        }

        //mirror the data until the trajectories are long enough
        //points 123 become 1232123212 if 10 points are required
        if (trajectory.size() > rounds) {
            trajectory.erase(trajectory.begin() + rounds, trajectory.end());
        } else {
            //the likelier branch.
            int step = -1;
            int endpoint = trajectory.size() - 1;
            int index = endpoint;
            while (trajectory.size() < rounds) {
                index += step;
                trajectory.push_back(trajectory[index]);
                if (index == endpoint || index == 0) {
                    step = -step;
                }
            }
        }
        t.push_back(trajectory);
    }

    return t;
}


void moveParticicpants(MovementType movementType, vector<Sensor>& sensors) {
    switch (movementType) {
        case RANDOM_MOVEMENT:
            moveParticipantsRandomly(sensors);
            break;


    }
}

void moveAlongTrajectories(vector<Sensor> &sensors, Trajectories &t, int t_index) {
    for (int i = 0; i < sensors.size(); i++) {
        sensors[i].x = t[t_index][i].first;
        sensors[i].y = t[t_index][i].second;
    }
}
