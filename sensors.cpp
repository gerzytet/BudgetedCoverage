#include "common.hpp"
#include "sensor_generators.hpp"
#include "random_utils.hpp"
#include "sensor_algorithms.hpp"
#include <stdlib.h>

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
    int ms;

    double coveragePercent(int total_sensors) {
        return ((double) coverage / total_sensors);
    }

    double areaCoveragePercent() {
        return ((double) areaCoverage / (101 * 101));
    }

    TrialResult (int coverage, int areaCoverage, int totalCost, int ms) : coverage(coverage), areaCoverage(areaCoverage), totalCost(totalCost), ms(ms) {}
};

enum AlgorithmType {
    GREEDY_ALG_TYPE = 1, RANDOM_ALG_TYPE = 2, BETTER_GREEDY_ALG_TYPE = 3, DYNAMIC_ALG_TYPE = 4, BRUTE_FORCE_ALG_TYPE = 5, K_GREEDY_ALG_TYPE = 6
};

struct AlgorithmInfo {
    AlgorithmType algType;
    int k; //only used for type 6

    AlgorithmInfo(AlgorithmType algType, int k) : algType(algType), k(k) {}
};

AlgorithmInfo GREEDY_ALG(GREEDY_ALG_TYPE, 0);
AlgorithmInfo RANDOM_ALG(RANDOM_ALG_TYPE, 0);
AlgorithmInfo BETTER_GREEDY_ALG(BETTER_GREEDY_ALG_TYPE, 0);
AlgorithmInfo DYNAMIC_ALG(DYNAMIC_ALG_TYPE, 0);
AlgorithmInfo BRUTE_FORCE_ALG(BRUTE_FORCE_ALG_TYPE, 0);

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
    } else if (distributionChoice == EXPONENTIAL) {
        sensors = generateSensorsClustered(num_sensors, true);
    }

    return sensors;
}

vector<Sensor> lastSensorsUsed;
TrialResult runTrial(AlgorithmInfo algorithmInfo, vector<Sensor> sensors, int R, int budget, string logname = "") {
    bool do_output = logname != "";
    ofstream output;

    if (do_output) {
        output.open ("outputs/" + logname, ofstream::out | ofstream::trunc); //truncate (erase) previous contents of the output file
        output << "Sensors:\n";
    }

    int totalCoverage = 0;

    if (do_output) {
        for (Sensor &s : sensors)
        {
            output << '(' << s.x << ", " << s.y << ", " << s.cost << ")\n";
        }
    }

    auto start = std::chrono::steady_clock::now();
    vector<int> chosen;
    AlgorithmType algorithmChoice = algorithmInfo.algType;
    if(algorithmChoice == GREEDY_ALG_TYPE)
    {
        sensors = sortSensors(sensors);
        calculateCoverage(sensors, R);
        chosen = greedyAlgorithm(sensors, budget, R);
    }
    else if(algorithmChoice == RANDOM_ALG_TYPE)
    {
        chosen = chooseSensorsRandomly(sensors, budget, R);
    }
    else if(algorithmChoice == BETTER_GREEDY_ALG_TYPE)
    {
        sensors = sortSensors(sensors);
        calculateCoverage(sensors, R);
        chosen = weightedAlgorithm(sensors, budget, R);

    } else if (algorithmChoice == DYNAMIC_ALG_TYPE) {
        chosen = dynamicAlgorithm(sensors, budget, R);
    } else if (algorithmChoice == BRUTE_FORCE_ALG_TYPE) {
        chosen = bruteForceAlgorithm(sensors, budget, R);
    } else if (algorithmChoice == K_GREEDY_ALG_TYPE) {
        chosen = kGreedyAlgorithm(sensors, budget, R, algorithmInfo.k);
    }
    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;
    int ms = std::chrono::duration<double, std::milli>(diff).count();

    if (do_output) {
        output << endl;
        output << "Chosen:\n";

        for (int index: chosen)
        {
            output << index << '\n';
            totalCoverage += sensors[index].coverage;
        }


        output << endl;

        output << "R:\n";
        output << R << '\n';

        output.close();
    }

    int area = calculateAreaCoverage(chosen, sensors, R);
    int totalCost = 0;
    for (int i : chosen) {
        totalCost += sensors[i].cost;
    }

    return TrialResult(calculateTotalCoverage(sensors, chosen, R), calculateAreaCoverage(chosen, sensors, R), totalCost, ms);
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

void compareDynamicBruteforce() {
    int outperform = 0;
    for (int i = 0; i < 100; i++) {
        vector<Sensor> sensors = generateSensors(CLUSTERED, 30, i);
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
        cout << "Dynamic spent: " << dynamicTrial.totalCost << " Brute force spent: " << bruteTrial.totalCost << '\n';

        if (dynamicTrial.coverage != bruteTrial.coverage || dynamicTrial.totalCost < bruteTrial.totalCost) {
            cout << "MISMATCH\n";
        }
        if (dynamicTrial.totalCost > bruteTrial.totalCost) {
            outperform++;
        }
    }
    cout << "outperformed in budget on " << outperform << " trials.\n";
}

void measureCoverageVsBudget() {
    ofstream output;
    output.open("coverage_vs_budget.csv");
    output << "algorithm,budget,coverage,cost per coverage unit\n";
    const int R = 15;

    auto printTrial = [&](string name, TrialResult result, int budget) {
        output << name << ',';
        output << budget << ',';
        output << result.coverage << ',';
        double coveragePerDollar = ((double)result.totalCost / result.coverage);
        output << coveragePerDollar << '\n';
    };
    for (int budget = 20; budget < 600; budget += 20) {
        for (int i = 0; i < 200; i++) {
            vector<Sensor> sensors = generateSensors(RANDOM, 30, i);
            printTrial("dynamic", runTrial(
                DYNAMIC_ALG,
                sensors,
                R,
                budget,
                "dynamic_" + to_string(i+1)
            ), budget);
            printTrial("brute_force", runTrial(
                BRUTE_FORCE_ALG,
                sensors,
                R,
                budget,
                "brute_force_" + to_string(i+1)
            ), budget);
            printTrial("greedy", runTrial(
                BETTER_GREEDY_ALG,
                sensors,
                R,
                budget,
                "greedy_" + to_string(i+1)
            ), budget);
            cout << "Budget " << budget << " Trial " << i+1 << "\n";

        }
    }
    output.close();
}

void compateAlgorithms() {
    ofstream output;
    output.open("algorithm_comparison.csv");
    output << "algorithm,coverage,time,cost per coverage unit\n";
    const int R = 15;
    const int budget = 450;

    auto printTrial = [&](string name, TrialResult result) {
        output << name << ',';
        output << result.coverage << ',';
        output << result.ms << ',';
        double coveragePerDollar = ((double)result.totalCost / result.coverage);
        output << coveragePerDollar << '\n';
    };
    for (int i = 0; i < 200; i++) {
        vector<Sensor> sensors = generateSensors(RANDOM, 30, i);
        printTrial("dynamic", runTrial(
            DYNAMIC_ALG,
            sensors,
            R,
            budget,
            "dynamic_" + to_string(i+1)
        ));
        printTrial("brute_force", runTrial(
            BRUTE_FORCE_ALG,
            sensors,
            R,
            budget,
            "brute_force_" + to_string(i+1)
        ));
        printTrial("greedy", runTrial(
            BETTER_GREEDY_ALG,
            sensors,
            R,
            budget,
            "greedy_" + to_string(i+1)
        ));
        cout << "Trial " << i+1 << "\n";

    }
    output.close();
}

void measureCoverageVsBudget2(int budget, int starttrial, int endtrial) {
    ofstream output;
    string name = to_string(budget) + "_" + to_string(starttrial) + "_" + to_string(endtrial);
    output.open(name + ".csv");
    //output << "algorithm,budget,coverage,cost per coverage unit,time\n";
    const int R = 11;

    auto printTrial = [&](string name, TrialResult result, int budget) {
        output << name << ',';
        output << budget << ',';
        output << result.coverage << ',';
        double coveragePerDollar = ((double)result.totalCost / result.coverage);
        output << coveragePerDollar << ',';
        output << result.ms << '\n';
    };
    for (int i = starttrial; i < endtrial; i++) {
        vector<Sensor> sensors = generateSensors(CLUSTERED, 60, i*budget);
        printTrial("dynamic", runTrial(
            DYNAMIC_ALG,
            sensors,
            R,
            budget,
            "dynamic_" + to_string(i+1)
        ), budget);
        cout << "1\n";
        printTrial("greedy", runTrial(
            BETTER_GREEDY_ALG,
            sensors,
            R,
            budget,
            "greedy_" + to_string(i+1)
        ), budget);
        cout << "2\n";
        printTrial("random", runTrial(
            RANDOM_ALG,
            sensors,
            R,
            budget,
            "random_" + to_string(i+1)
        ), budget);
        cout << "3\n";
        printTrial("bad_greedy", runTrial(
            GREEDY_ALG,
            sensors,
            R,
            budget,
            "bad_greedy_" + to_string(i+1)
        ), budget);
        cout << "4\n";
        printTrial("k=1", runTrial(
            AlgorithmInfo(K_GREEDY_ALG_TYPE, 1),
            sensors,
            R,
            budget,
            "k1_" + to_string(i+1)
        ), budget);
        cout << "5\n";
        printTrial("k=2", runTrial(
            AlgorithmInfo(K_GREEDY_ALG_TYPE, 2),
            sensors,
            R,
            budget,
            "k2_" + to_string(i+1)
        ), budget);
        cout << "Budget " << budget << " Trial " << i+1 << "\n";
    }
    output.close();
}

vector<int> getNextBatch() {
    system(".\\connect.bat");
    ifstream output;
    output.open("output.txt");
    vector<int> ans;
    for (int i = 0; i < 3; i++) {
        int x;
        output >> x;
        ans.push_back(x);
    }
    return ans;
}

void upload(string filename) {
    system((".\\upload_done.bat " + filename).c_str());
}

void measureCoverageVsBudget2RemoteLoop() {
    while (true) {
        cout << "Getting batch...\n";
        vector<int> params = getNextBatch();
        if (params[0] == 0 && params[1] == 0 && params[2] == 0) {
            break;
        }
        cout << "Got batch: " << params[0] << ' ' << params[1] << ' ' << params[2] << '\n';
        measureCoverageVsBudget2(params[0], params[1], params[2]);
        string name = to_string(params[0]) + "_" + to_string(params[1]) + "_" + to_string(params[2]) + ".csv";
        cout << "Uploading...\n";
        upload(name);
    }
}

void experiment4() {
    ofstream output;
    //string name = to_string(budget) + "_" + to_string(starttrial) + "_" + to_string(endtrial);
    output.open("experiment4.csv");
    for (int budget = 100; budget <= 3000; budget+=100) {
        for (int trial = 0; trial < 100; trial++) {
            for (DistributionType distribution : {CLUSTERED, RANDOM, EXPONENTIAL}) {
                //output << "algorithm,budget,coverage,cost per coverage unit,time\n";
                const int R = 11;

                auto printTrial = [&](string name, TrialResult result, int budget) {
                    output << name << ',';
                    output << budget << ',';
                    output << result.coverage << ',';
                    double coveragePerDollar = ((double)result.totalCost / result.coverage);
                    output << coveragePerDollar << ',';
                    output << distribution << ',';
                    output << result.ms << endl;
                };

                vector<Sensor> sensors = generateSensors(distribution, 100, budget + trial);
                printTrial("greedy", runTrial(
                    BETTER_GREEDY_ALG,
                    sensors,
                    R,
                    budget
                ), budget);
                printTrial("random", runTrial(
                    RANDOM_ALG,
                    sensors,
                    R,
                    budget
                ), budget);
                printTrial("bad_greedy", runTrial(
                    GREEDY_ALG,
                    sensors,
                    R,
                    budget
                ), budget);
                printTrial("k=1", runTrial(
                    AlgorithmInfo(K_GREEDY_ALG_TYPE, 1),
                    sensors,
                    R,
                    budget
                ), budget);
                printTrial("k=2", runTrial(
                    AlgorithmInfo(K_GREEDY_ALG_TYPE, 2),
                    sensors,
                    R,
                    budget
                ), budget);

                cout << "Budget " << budget << " Trial " << trial+1 << " dist " << (int)distribution << "\n";
            }
        }
    }
}

int main()
{
    experiment4();
    //for (int x : getNextBatch()) {
    //    cout << x << '\n';
    //}
    //experiment2();
    //measureCoverageVsBudget2RemoteLoop();
    //runTrial(DYNAMIC_ALG, generateSensors(CLUSTERED, 75, 0), 15, 500, "dynamic_test");
    //return 0;

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

    TrialResult result = runTrial(AlgorithmInfo((AlgorithmType)algorithmChoice, 0), generateSensors((DistributionType)distributionChoice, 100, randint(1, 100000)), 5, 400, "manual_run.txt");

    std::cout << "Total Cost: " << result.totalCost;
    std::cout << "\nTotal Coverage: " << result.coverage << ", " << result.coveragePercent(20) << '%' << endl;
    std::cout << "\nArea Coverage: " << result.areaCoverage << ", " << result.areaCoveragePercent() << '%' << endl;
}
