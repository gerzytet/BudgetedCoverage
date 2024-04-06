#include "sensor_generators.hpp"
#include "random_utils.hpp"
#define M_PI 3.14159265358979323846

int randomCost()
{
    //return project2Mode ? randint(10, 100) : randint(250, 500);
    return randnormal(50, 20);
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

template <typename T>
T logg(T t) {
    //cout << t << " ";
    return t;
}



pair<int, int> randomPoint(pair<int, int> center, int radius, bool exponental_mode = false) {

    int x = -1;
    int y = -1;
    while (!inRange(x, y)) {
        float angle = (rand() % 360) * M_PI / 180;
        x = center.first + logg(abs(exponental_mode ? randexponential(0, radius) : randnormal(0, radius)) * cos(angle));
        y = center.second + logg(abs(exponental_mode ? randexponential(0, radius) : randnormal(0, radius)) * sin(angle));
    }
    //cout << angle << " " << x << " " << y << endl;
    return make_pair(x, y);
}

const int NEIGHBORHOODS = 4;
vector<Sensor> generateSensorsClustered(int num_points, bool exponental_mode)
{
    vector<Sensor> sensors;
    /*vector<pair<int, int>> points;
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
    }*/
    int center_x[4] = {30, 80, 50, 90};
    int center_y[4] = {80, 80, 50, 30};
    vector<pair<int, int>> points;
    for (int i = 0; i < NEIGHBORHOODS; i++) {
        points.push_back(make_pair(center_x[i], center_y[i]));
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
                costMean = 50;
                costDeviation = 20;
                break;
            case 2:
                costMean = 100;
                costDeviation = 20;
                break;
            case 3:
                costMean = 150;
                costDeviation = 20;
                break;
            case 4:
                costMean = 200;
                costDeviation = 20;
                break;
        }
        for (int i = 0; i < points_per_neighborhood; i++)
        {
            //cout << points[i].first << " " << points[i].second << endl;
            pair<int, int> p = randomPoint(point, 30, exponental_mode);
            int x = p.first;
            int y = p.second;
            int cost = -1;
            while (cost < 0) {
                cost = exponental_mode ? randexponential(costMean, costDeviation) : randnormal(costMean, costDeviation);
            }

            sensors.push_back(Sensor(x, y, cost, i));
        }
    }

    return sensors;
}

void generateParticipantData(vector<Sensor> &sensors) {
    for (Sensor &s : sensors) {
        //the cost generated in the previous step is the true valuation
        //a new cost will be generated based off of that

        s.t = s.cost;
        s.cost = s.t * randfloat(1.0, 1.5);

        //generate beta
        s.beta = randfloat(3.0, 7.0);
    }
}
