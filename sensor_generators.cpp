#include "sensor_generators.hpp"
#include "random_utils.hpp"

const int MAX_COORDINATE = 100;


int randomCost()
{
    //return project2Mode ? randint(10, 100) : randint(250, 500);
    return randint(10, 100);
}

vector<Sensor> generateSensorsRandomly(int num_points)
{
    vector<Sensor> sensors;
    for (int i = 0; i < num_points; i++)
    {
        sensors.push_back(Sensor(randint(0, MAX_COORDINATE), randint(0, MAX_COORDINATE), randomCost(), i));
    }
    return sensors;
}

vector<Sensor> generateSensorsUniformly(int num_points)
{
    vector<Sensor> sensors;
    int n = 0;
    while (n * n < num_points) {
        n++;
    }
    int distance = 100 / n;
    int count = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            sensors.push_back(Sensor(i * distance, j * distance, randomCost(), count));
            count++;
            if (count >= num_points) {
                goto end;
            }
        }
    }
    end:
    //TODO: add assertion to make sure the sensors array has the right number
    return sensors;
}

const int NEIGHBORHOODS = 4;
vector<Sensor> generateSensorsClustered(int num_points)
{
    vector<Sensor> sensors;
    vector<pair<int, int>> points;
    double threshold = 40;
    while (points.size() < NEIGHBORHOODS)
    {
        pair<int, int> point = make_pair(randint(0, MAX_COORDINATE), randint(0, MAX_COORDINATE));
        bool tooClose = false;
        for (auto otherPoint : points)
        {
            if (calculateDistance(point, otherPoint) < threshold)
            {
                threshold -= 0.01;
                tooClose = true;
                break;
            }
        }

        if (tooClose)
        {
            continue;
        }
        points.push_back(point);
    }

    int points_per_neighborhood = num_points / NEIGHBORHOODS;
    int counter = 0;
    for (auto point : points)
    {
        counter++;

        int mean;
        int deviation;
        switch (counter) {
            case 1:
                mean = 5;
                deviation = 4;
                break;
            case 2:
                mean = 30;
                deviation = 10;
                break;
            case 3:
                mean = 65;
                deviation = 15;
                break;
            case 4:
                mean = 18;
                deviation = 7;
                break;
        }
        for (int i = 0; i < points_per_neighborhood; i++)
        {
            int x = randint(max(point.first - 20, 0), min(point.first + 20, 100));
            int y = randint(max(point.second - 20, 0), min(point.second + 20, 100));
            sensors.push_back(Sensor(x, y, randnormal(mean, deviation), i));
        }
    }

    return sensors;
}
