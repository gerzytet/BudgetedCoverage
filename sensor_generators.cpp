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

        int costMean;
        int costDeviation;
        switch (counter) {
            case 1:
                costMean = 5;
                costDeviation = 4;
                break;
            case 2:
                costMean = 30;
                costDeviation = 10;
                break;
            case 3:
                costMean = 65;
                costDeviation = 15;
                break;
            case 4:
                costMean = 18;
                costDeviation = 7;
                break;
        }
        for (int i = 0; i < points_per_neighborhood; i++)
        {
            int x = -1;
            int mean = point.first;
            int deviation = 15;
            while (x < 0 || x > 100) {
                x = randnormal(mean, deviation);
            }
            int y = -1;
            mean = point.second;
            while (y < 0 || y > 100) {
                y = randnormal(mean, deviation);
            }
            int cost = -1;
            while (cost < 0) {
                cost = randnormal(costMean, costDeviation);
            }

            sensors.push_back(Sensor(x, y, cost, i));
        }
    }

    return sensors;
}
