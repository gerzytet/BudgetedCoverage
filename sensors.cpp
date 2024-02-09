#include <iostream>
#include <random>
#include <iomanip>
#include <fstream>
#include <set>
#include <cmath>
#include <chrono>
#include <map>
#include <unordered_map>
#include <bitset>

using namespace std;

//To run this file in VS Code
//Open Developer console
//Type code -> enter

//Struct variables are public by default instead of private
struct Sensor
{
    int x, y, cost, coverage; //coverage: the number of other sensors within the current circle
    double weight;
    int i;

    Sensor(int x, int y, int cost, int i): x(x), y(y), cost(cost), coverage(0), i(i) {} // Short for this.x = x this.y = y this.cost=cost
};

long long nanos = chrono::steady_clock::now().time_since_epoch().count();
minstd_rand randomGenerator(nanos);
void seedRandom(int seed) {
    randomGenerator.seed(seed);
}

int randint(int a, int b)
{
    uniform_int_distribution<int> distribution(a, b);
    return distribution(randomGenerator);
}

double randfloat(double a, double b)
{
    uniform_real_distribution<double> distribution(a, b);
    return distribution(randomGenerator);
}

double randnormal(double mean, double deviation) {
    normal_distribution<double> distribution(mean, deviation);
    return distribution(randomGenerator);
}

const int MAX_COORDINATE = 100;

vector<int> chooseSensorsRandomly(const vector<Sensor> &sensors, int budget, int R)
{
    int totalCost = 0;
    set<int> chosen;
    int brokeCount = 0;

    while(chosen.size() < sensors.size() && brokeCount < 40)
    {
        int index = randint(0, sensors.size() - 1);
        while (chosen.count(index) != 0)
        {
            index = randint(0, sensors.size() - 1);
        }

        if((totalCost + sensors[index].cost) > budget)
        {
            brokeCount++;
            continue;
        }

        chosen.insert(index);
        totalCost += sensors[index].cost;
    }
    return vector<int>(chosen.begin(), chosen.end());
}

vector<int> greedyAlgorithm(const vector<Sensor> &sensors, int budget, int R)
{
    vector<Sensor> greedySort(sensors);
    vector<int32_t> chosen;
    int totalCost = 0;
    //Check greedySort.size first to prevent out of bounds error
    for (int i = 0; i < greedySort.size() && (totalCost + greedySort[i].cost) <= budget; ++i)
    {
        totalCost += sensors[i].cost;
        chosen.push_back(i);
    }
    return chosen;
}

double calculateDistance(Sensor s1, Sensor s2)
{
    return sqrt( pow(s2.x - s1.x, 2) + pow(s2.y - s1.y, 2) );
}

double calculateDistance(pair<int, int> p1, pair<int, int> p2)
{
    return sqrt( pow(p1.first - p2.first, 2) + pow(p1.second - p2.second, 2) );
}

vector<int> returnCoveredSensors(const vector<Sensor> &sensors, int index, int R)
{
    vector<int> covered;
    for(int j = 0; j < sensors.size(); j++)
    {
        //Don't count coverage for the sensor itself
        if(index != j && calculateDistance(sensors[index], sensors[j]) < R)
        {
            covered.push_back(j);
        }
    }
    return covered;
}

int countBits(int x) {
    /*int count = 0;
    while (x > 0) {
        count += x & 1;
        x >>= 1;
    }
    return count;*/
    return __builtin_popcount(x);
}

namespace std
{
    template<typename... TTypes>
    class hash<std::tuple<TTypes...>>
    {
    private:
        typedef std::tuple<TTypes...> Tuple;

        template<int N>
        size_t operator()(Tuple value) const { return 0; }

        template<int N, typename THead, typename... TTail>
        size_t operator()(Tuple value) const
        {
            constexpr int Index = N - sizeof...(TTail) - 1;
            return hash<THead>()(std::get<Index>(value)) ^ operator()<N, TTail...>(value);
        }

    public:
        size_t operator()(Tuple value) const
        {
            return operator()<sizeof...(TTypes), TTypes...>(value);
        }
    };
}

using bs = bitset<256>;

//{{budget, j, mask}: {covered, chosen}}
unordered_map<tuple<int, int, bs>, pair<bs, bs>> cache;

pair<bs, bs> recursiveSolve(int budget, int j, bs mask, vector<bs> &sets, vector<int> &costs, vector<bs> &cumulativeSet) {
    if (j == 0 || budget <= 0) {
        return {0, 0};
    }

    mask &= cumulativeSet[j];
    if (cache.count({budget, j, mask}) > 0) {
        return cache[{budget, j, mask}];
    }

    pair<bs, bs> ans = recursiveSolve(budget, j - 1, mask, sets, costs, cumulativeSet);
    if ((mask | sets[j-1]) != mask) {
        if (budget >= costs[j-1]) {
            pair<bs, bs> ans3 = recursiveSolve(budget - costs[j-1], j - 1, mask | sets[j-1], sets, costs, cumulativeSet);
            ans3.first |= sets[j-1];
            ans3.second.set(j-1, true);
            if ((ans3.first | mask).count() > (ans.first | mask).count()) {
                ans = ans3;
            }
        }
    }
    cache[{budget, j, mask}] = ans;
    return ans;
}

vector<bs> calculateCoverageSets(const vector<Sensor> &sensors, int R) {
    vector<bs> sets(sensors.size());
    for (int i = 0; i < sensors.size(); i++) {
        bs s = 0;
        vector<int> touching = returnCoveredSensors(sensors, i, R);
        touching.push_back(i);
        for (int j : touching) {
            s.set(j, true);
        }

        sets[i] = s;
    }

    return sets;
}

vector<int> bsToVector(bs mask) {
    vector<int> chosen;
    for (int i = 0; i < mask.size(); i++) {
        if (mask.test(i) > 0) {
            chosen.push_back(i);
        }
    }

    return chosen;
}

vector<int> dynamicAlgorithm(const vector<Sensor> &sensors, int budget, int R) {
    cache.clear();
    vector<bs> sets = calculateCoverageSets(sensors, R);

    vector<bs> cumulativeSets(sensors.size() + 1);
    cumulativeSets[0] = 0;
    for (int i = 0; i < sensors.size(); i++) {
        cumulativeSets[i+1] = cumulativeSets[i] | sets[i];
    }

    vector<int> costs;
    for (const Sensor &s : sensors) {
        costs.push_back(s.cost);
    }

    auto [coveredMask, chosenMask] = recursiveSolve(budget, sensors.size(), 0, sets, costs, cumulativeSets);
    vector<int> chosen;
    chosen = bsToVector(chosenMask);

    int totalUsed = 0;
    for (int i = 0; i < chosen.size(); i++) {
        totalUsed += sensors[chosen[i]].cost;
    }
    //cout << "Total used: " << totalUsed << " out of " << budget << endl;
    return chosen;
}

void calculateCoverage(vector<Sensor> &sensors, set<int> covered, int R)
{
    for (Sensor &s : sensors) {
        s.coverage = 0;
    }
    for(int i = 0; i < sensors.size(); i++)
    {
        if (covered.count(i) == 0) {
            sensors[i].coverage++;
        }
        for(int j = 0; j < sensors.size(); j++)
        {
            if (covered.count(j) == 1) {
                continue;
            }
            //Don't count coverage for the sensor itself
            if(i != j && calculateDistance(sensors[i], sensors[j]) < R)
            {
                sensors[i].coverage++;
            }
        }
    }
}

void calculateCoverage(vector<Sensor> &sensors, int R) {
    set<int> covered;
    calculateCoverage(sensors, covered, R);
}

vector<Sensor> sortSensorsByWeight(const vector<Sensor> &s)
{
    //copy sensors array to new array for sorting
    vector<Sensor> sortedSensors(s);

    //selection sort
    int i, j, max_idx;
    for(int i = 0; i < sortedSensors.size()-1; i++)
    {
        max_idx = i;
        for (j = i+1; j < sortedSensors.size(); j++)
        {
          if (sortedSensors[j].weight > sortedSensors[max_idx].weight)
              max_idx = j;
        }
        // Swap the found maximum element
        // with the first element
        if (max_idx!=i)
        {
            swap(sortedSensors[max_idx], sortedSensors[i]);
        }
    }
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

bool contains(vector<int> v, int find)
{
    for(int i = 0; i < v.size(); i++)
    {
        if(v[i] == find)
        return true;
    }
    return false;
}

vector<int> budgetAlgorithm(const vector<Sensor> &sensors, int budget, int R)
{
    vector<Sensor> sensorsCopy(sensors);
    vector<int> chosen;
    int totalCost = 0;
    vector< vector<int> > universe;

    for (int i = 0; i < sensorsCopy.size() && (totalCost + sensorsCopy[i].cost) <= budget; ++i)
    {
        totalCost += sensors[i].cost;
        chosen.push_back(i);
    }
    return chosen;
}

vector<int> weightedAlgorithm(vector<Sensor> &sensors, int budget, int R)
{
    set<int> chosen;
    set<int> considered;
    set<int> covered;
    int totalCost = 0;

    //initialize with the collection of sets S
    //allSets = ..

    while (considered.size() < sensors.size())
    {
        //get element with max weight
        int index_maxweight = -1;
        double maxweight = 0;
        for (int i = 0; i < sensors.size(); ++i)
        {
            if (considered.count(i) != 0) {
                continue;
            }

            if ((((double)sensors[i].coverage) / ((double)sensors[i].cost)) > maxweight)
            {
                index_maxweight = i;
                maxweight = (((double)sensors[i].coverage) / ((double)sensors[i].cost));
            }
        }

        if (index_maxweight == -1) {
            break;
        }

        considered.insert(index_maxweight);
        if (totalCost + sensors[index_maxweight].cost < budget)
        {
            //cout << sensors[index_maxweight].cost << ' ' << maxweight << ' ' << sensors[index_maxweight].coverage << '\n';
            chosen.insert(index_maxweight);
            for (int i : returnCoveredSensors(sensors, index_maxweight, R)) {
                covered.insert(i);
            }
            calculateCoverage(sensors, covered, R);
            totalCost += sensors[index_maxweight].cost;
        }
    }
    return vector<int>(chosen.begin(), chosen.end());
    //select a set St from G that maximizes Wt over S;

    /*if (G weight >= weight)
        return G;
    else
        return ;*/

}

void recursiveBruteForceSearch(
    bs &chosenMask,
    int pos,
    int currBudget,
    bs currCoverage,
    int &bestCoverageCount,
    bs &bestCoverage,
    bs &bestChosenMask,
    int &bestRemainingBudget,
    const vector<Sensor> &sensors,
    const vector<bs> &coverageSets
) {
    if (pos == sensors.size()) {
        return;
    }
    chosenMask.set(pos, false);
    recursiveBruteForceSearch(
        chosenMask,
        pos + 1,
        currBudget,
        currCoverage,
        bestCoverageCount,
        bestCoverage,
        bestChosenMask,
        bestRemainingBudget,
        sensors,
        coverageSets
    );

    if (sensors[pos].cost <= currBudget) {
        chosenMask.set(pos, true);
        currBudget -= sensors[pos].cost;
        bs newCoverage = currCoverage | coverageSets[pos];
        if (newCoverage.count() > bestCoverageCount ||
            (newCoverage.count() == bestCoverageCount && currBudget > bestRemainingBudget)) {
            bestCoverage = newCoverage;
            bestCoverageCount = newCoverage.count();
            bestChosenMask = chosenMask;
            bestRemainingBudget = currBudget;
        }
        recursiveBruteForceSearch (
            chosenMask,
            pos + 1,
            currBudget,
            newCoverage,
            bestCoverageCount,
            bestCoverage,
            bestChosenMask,
            bestRemainingBudget,
            sensors,
            coverageSets
        );
        chosenMask.set(pos, false);
    }
}

vector<int> bruteForceAlgorithm(const vector<Sensor> &sensors, int budget, int R) {
    int bestCoverageCount = 0, bestRemainingBudget = 0;
    bs bestCoverage = 0;
    bs chosenMask = 0, bestChosenMask = 0;
    recursiveBruteForceSearch(
        chosenMask,
        0,
        budget,
        0,
        bestCoverageCount,
        bestCoverage,
        bestChosenMask,
        bestRemainingBudget,
        sensors,
        calculateCoverageSets(sensors, R)
    );

    return bsToVector(bestChosenMask);
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

enum DistributionType {
    UNIFORM = 1, CLUSTERED = 2, RANDOM = 3
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
        auto start = chrono::steady_clock::now();

        chosen = dynamicAlgorithm(sensors, budget, R);

        auto end = chrono::steady_clock::now();
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
    proveDynamicOptimalExperiment();
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
