#include "common.hpp"
#include "sensor_generators.hpp"
#include "random_utils.hpp"
#include "sensor_algorithms.hpp"

bool weightComparator(Sensor a, Sensor b) {
    return a.weight < b.weight;
}

vector<Sensor> sortSensorsByWeight(const vector<Sensor> &s)
{
    vector<Sensor> sortedSensors(s);
    sort(sortedSensors.begin(), sortedSensors.end(), weightComparator);
    return sortedSensors;
}

vector<Sensor> sortSensors(const vector<Sensor> &s)
{
    if (s.size() == 0) {
        return s;
    }
    //copy sensors array to new array for sorting
    vector<Sensor> sortedSensors(s);

    //selection sort
    int i, j, min_idx;
    for(int i = 0; i < sortedSensors.size()-1; i++)
    {
        min_idx = i;
        for (j = i+1; j < sortedSensors.size(); j++)
        {
          if (sortedSensors[j].cost < sortedSensors[min_idx].cost)
              min_idx = j;
        }
        // Swap the found minimum element
        // with the first element
        if (min_idx!=i)
        {
            swap(sortedSensors[min_idx], sortedSensors[i]);
        }
    }
    return sortedSensors;
}

int calculateTotalCoverage(const vector<Sensor> &sensors, vector<int> chosen, int R) {
    set<int> covered;
    for (int i : chosen) {
        covered.insert(i);
        for (int touching : returnCoveredSensors(sensors, i, R)) {
            covered.insert(touching);
        }
    }

    return covered.size();
}

int calculateAreaCoverage(const vector<int> sensors_indicies, vector<Sensor> &sensors, int R) {
    int coverage = 0;
    for (int x = 0; x <= 100; x++) {
        for (int y = 0; y <= 100; y++) {
            pair<int, int> p1 = make_pair(x, y);
            for (int index : sensors_indicies) {
                Sensor sensor = sensors[index];
                pair<int, int> p2 = make_pair(sensor.x, sensor.y);
                if (calculateDistance(p1, p2) < R) {
                    coverage++;
                    break;
                }
            }
        }
    }

    return coverage;
}

struct TrialResult {
    int coverage;
    int areaCoverage;
    int totalCost;
    int budgetSpent;

    double coveragePercent(int total_sensors) {
        return ((double) coverage / total_sensors);
    }

    double areaCoveragePercent() {
        return ((double) areaCoverage / (101 * 101));
    }

    TrialResult (int coverage, int areaCoverage, int totalCost, int budgetSpent) : coverage(coverage), areaCoverage(areaCoverage), totalCost(totalCost), budgetSpent(budgetSpent) {}
};

enum AlgorithmType {
    GREEDY_ALG = 1, RANDOM_ALG = 2, BETTER_GREEDY_ALG = 3, DYNAMIC_ALG = 4, BRUTE_FORCE_ALG = 5
};

vector<Sensor> generateSensors(DistributionType distributionChoice, int num_sensors, int seed) {
    vector<Sensor> sensors;
    seedRandom(seed);
    if(distributionChoice == UNIFORM)
    {
        sensors = generateSensorsUniformly(num_sensors);
    }
    else if(distributionChoice == CLUSTERED)
    {
        sensors = generateSensorsClustered(num_sensors);
    }
    else if(distributionChoice == RANDOM)
    {
        sensors = generateSensorsRandomly(num_sensors);
    }

    return sensors;
}

vector<Sensor> lastSensorsUsed;
TrialResult runTrial(AlgorithmType algorithmChoice, vector<Sensor> sensors, int R, int budget, string logname = "") {
    bool do_output = logname != "";
    ofstream output;

    if (do_output) {
        output.open ("outputs/" + logname, ofstream::out | ofstream::trunc); //truncate (erase) previous contents of the output file
        output << "Sensors:\n";
    }

    int totalCost = 0;
    int totalCoverage = 0;

    if (do_output) {
        for (Sensor &s : sensors)
        {
            output << '(' << s.x << ", " << s.y << ", " << s.cost << ")\n";
        }
    }

    vector<int> chosen;
    if(algorithmChoice == GREEDY_ALG)
    {
        sensors = sortSensors(sensors);
        calculateCoverage(sensors, R);
        chosen = greedyAlgorithm(sensors, budget, R);
    }
    else if(algorithmChoice == RANDOM_ALG)
    {
        chosen = chooseSensorsRandomly(sensors, budget, R);
    }
    else if(algorithmChoice == BETTER_GREEDY_ALG)
    {
        sensors = sortSensors(sensors);
        calculateCoverage(sensors, R);
        chosen = weightedAlgorithm(sensors, budget, R);

    } else if (algorithmChoice == DYNAMIC_ALG) {
        //time it
        auto start = std::chrono::steady_clock::now();

        chosen = dynamicAlgorithm(sensors, budget, R);

        auto end = std::chrono::steady_clock::now();
        auto diff = end - start;
        //cout << chrono::duration <double, milli> (diff).count() << " ms" << endl;
    } else if (algorithmChoice == BRUTE_FORCE_ALG) {
        chosen = bruteForceAlgorithm(sensors, budget, R);
    }

    if (do_output) {
        output << endl;
        output << "Chosen:\n";

        for (int index: chosen)
        {
            output << index << '\n';
            totalCost += sensors[index].cost;
            totalCoverage += sensors[index].coverage;
        }


        output << endl;

        output << "R:\n";
        output << R << '\n';
    }

    int area = calculateAreaCoverage(chosen, sensors, R);
    int totalSpent = 0;
    for (int i : chosen) {
        totalSpent += sensors[i].cost;
    }

    return TrialResult(calculateTotalCoverage(sensors, chosen, R), calculateAreaCoverage(chosen, sensors, R), totalCost, totalSpent);
}

void experiment() {
    ofstream output;
    output.open ("experiment_output.txt", ofstream::out | ofstream::trunc); //truncate (erase) previous contents of the output file

    output << "budget coverage\n";
    output << "pure_greedy random greedy dynamic\n";
    int budgets[] = {100, 120, 200, 250, 400};
    int num_sensors = 170;

    for (int i = 0; i < 5; i++) {
        int budget  = budgets[i];
        vector<Sensor> sensors = generateSensors(CLUSTERED, num_sensors, i);
        auto trial  = runTrial(GREEDY_ALG, sensors, 5, budget);
        auto trial1 = runTrial(RANDOM_ALG, sensors, 5, budget);
        auto trial2 = runTrial(BETTER_GREEDY_ALG, sensors, 5, budget);
        auto trial3 = runTrial(DYNAMIC_ALG, sensors, 5, budget);
        output << budget << ' ' << trial.coveragePercent(20) << ' ' << trial1.coveragePercent(20) << ' ' << trial2.coveragePercent(20) << ' ' << trial3.coveragePercent(20) << endl;
    }
}

/*
goal of experiment: get high sample size data for the coverage % for a given number of sensors
also measure performance

control:
radius
budget
distribution strategy (clustered)
number of trials per sensors/algorithm choice

independent:
algorithm
number of sensors

dependent:
performance (total for all)
coverage (average)

*/
void experiment2() {
    ofstream output;
    output.open ("experiment2_output.txt", ofstream::out | ofstream::trunc); //truncate (erase) previous contents of the output file

    output << "example title\n";
    output << "pure_greedy random greedy dynamic\n";
    vector<int> sensorAmounts = {20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120};

    for (int i = 0; i < sensorAmounts.size(); i++) {
        int budget  = 10000;
        int points = sensorAmounts[i];
        vector<Sensor> sensors = generateSensors(CLUSTERED, points, i);
        auto trial  = runTrial(GREEDY_ALG, sensors, 5, budget);
        auto trial1 = runTrial(RANDOM_ALG, sensors, 5, budget);
        auto trial2 = runTrial(BETTER_GREEDY_ALG, sensors, 5, budget);
        auto trial3 = runTrial(DYNAMIC_ALG, sensors, 5, budget);

        output << budget
               << ' ' << trial.coveragePercent(points)
               << ' ' << trial1.coveragePercent(points)
               << ' ' << trial2.coveragePercent(points)
               << ' ' << trial3.coveragePercent(points) << endl;
    }
}

void proveDynamicOptimalExperiment() {
    int outperform = 0;
    for (int i = 0; i < 100; i++) {
        vector<Sensor> sensors = generateSensors(RANDOM, 30, i);
        auto dynamicTrial = runTrial(
            DYNAMIC_ALG,
            sensors,
            15,
            450,
            "dynamic_" + to_string(i+1)
        );
        auto bruteTrial = runTrial(
            BRUTE_FORCE_ALG,
            sensors,
            15,
            450,
            "brute_force_" + to_string(i+1)
        );
        cout << "Trial " << i+1 << ": ";
        cout << "Dynamic coverage: " << dynamicTrial.coverage << " Brute force coverage: " << bruteTrial.coverage << '\n';
        cout << "Dynamic spent: " << dynamicTrial.budgetSpent << " Brute force spent: " << bruteTrial.budgetSpent << '\n';

        if (dynamicTrial.coverage != bruteTrial.coverage || dynamicTrial.budgetSpent < bruteTrial.budgetSpent) {
            cout << "MISMATCH\n";
        }
        if (dynamicTrial.budgetSpent > bruteTrial.budgetSpent) {
            outperform++;
        }
    }
    cout << "outperformed in budget on " << outperform << " trials.\n";
}

int main()
{
    experiment();
    return 0;

    //This is a text based menu for running a custom trial
    //not in use for the experiment:
    int algorithmChoice;
    std::cout << "Which algorithm would you like to use?\n1. Greedy Algorithm\n2. Random Algorithm\n3. Budgeted Algorithm\n4. Dynamic Algorithm";
    std::cin >> algorithmChoice;

    cout << endl;

    int distributionChoice;
    cout << "Choose a distribution\n1. Uniform\n2. Clustered\n3. Random";
    cin >> distributionChoice;

    cout << endl;

    // for (int i = 0; i < NUM_POINTS; i++)
    // {
    //     sensors.push_back(Sensor(randint(0, 100), randint(0, 100), randint(250, 500)));
    // }

    TrialResult result = runTrial((AlgorithmType)algorithmChoice, generateSensors((DistributionType)distributionChoice, 20, randint(1, 100000)), 5, 400);

    std::cout << "Total Cost: " << result.totalCost;
    std::cout << "\nTotal Coverage: " << result.coverage << ", " << result.coveragePercent(20) << '%' << endl;
    std::cout << "\nArea Coverage: " << result.areaCoverage << ", " << result.areaCoveragePercent() << '%' << endl;
}
