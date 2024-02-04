#include <iostream>
#include <random>
#include <iomanip>
#include <fstream>
#include <set>
#include <cmath>
#include <chrono>
#include <map>
#include <unordered_map>

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

int R = 20;
int budget = 10000;
const int MAX_COORDINATE = 100;

vector<int> chooseSensorsRandomly(const vector<Sensor> &sensors)
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

vector<int> greedyAlgorithm(const vector<Sensor> &sensors)
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

vector<int> returnCoveredSensors(const vector<Sensor> &sensors, int index)
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

//{{budget, j}: {covered, chosen}}
unordered_map<tuple<int, int, int>, pair<int, int>> cache;

pair<int, int> recursiveSolve(int budget, int j, int mask, vector<int> &sets, vector<int> &costs, vector<int> &cumulativeSet) {
    if (j == 0 || budget <= 0) {
        return {0, 0};
    }

    mask &= cumulativeSet[j];
    if (cache.count({budget, j, mask}) > 0) {
        return cache[{budget, j, mask}];
    }

    pair<int, int> ans = recursiveSolve(budget, j - 1, mask, sets, costs, cumulativeSet);
    if (mask | sets[j-1] != mask) {
        if (budget >= costs[j-1]) {
            pair<int, int> ans3 = recursiveSolve(budget - costs[j-1], j - 1, mask | sets[j-1], sets, costs, cumulativeSet);
            ans3.first |= sets[j-1];
            ans3.second |= (1 << (j-1));
            if (countBits(ans3.first | mask) > countBits(ans.first | mask)) {
                ans = ans3;
            }
        }
    }
    cache[{budget, j, mask}] = ans;
    return ans;
}

vector<int> dynamicAlgorithm(const vector<Sensor> &sensors) {
    cache.clear();
    vector<int> sets(sensors.size());
    for (int i = 0; i < sensors.size(); i++) {
        int s = 0;
        vector<int> touching = returnCoveredSensors(sensors, i);
        touching.push_back(i);
        for (int j : touching) {
            s |= (1 << j);
        }

        sets[i] = s;
    }

    vector<int> cumulativeSets(sensors.size() + 1);
    cumulativeSets[0] = 0;
    for (int i = 0; i < sensors.size(); i++) {
        cumulativeSets[i+1] |= cumulativeSets[i] | sets[i];
    }

    vector<int> costs;
    for (const Sensor &s : sensors) {
        costs.push_back(s.cost);
    }

    auto [coveredMask, chosenMask] = recursiveSolve(budget, sensors.size(), 0, sets, costs, cumulativeSets);
    vector<int> chosen;
    for (int i = 0; i < sensors.size(); i++) {
        if ((chosenMask & (1 << i)) > 0) {
            chosen.push_back(i);
        }
    }

    int totalUsed = 0;
    for (int i = 0; i < chosen.size(); i++) {
        totalUsed += sensors[chosen[i]].cost;
    }
    cout << "Total used: " << totalUsed << " out of " << budget << endl;
    return chosen;
}

void calculateCoverage(vector<Sensor> &sensors, set<int> covered)
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

void calculateCoverage(vector<Sensor> &sensors) {
    set<int> covered;
    calculateCoverage(sensors, covered);
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

vector<int> budgetAlgorithm(const vector<Sensor> &sensors)
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

vector<int> weightedAlgorithm(vector<Sensor> &sensors)
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
            for (int i : returnCoveredSensors(sensors, index_maxweight)) {
                covered.insert(i);
            }
            calculateCoverage(sensors, covered);
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

int calculateTotalCoverage(const vector<Sensor> &sensors, vector<int> chosen) {
    set<int> covered;
    for (int i : chosen) {
        covered.insert(i);
        for (int touching : returnCoveredSensors(sensors, i)) {
            covered.insert(touching);
        }
    }

    return covered.size();
}

int calculateAreaCoverage(const vector<int> sensors_indicies, vector<Sensor> &sensors) {
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

    double coveragePercent(int total_sensors) {
        return ((double) coverage / total_sensors);
    }

    double areaCoveragePercent() {
        return ((double) areaCoverage / (101 * 101));
    }

    TrialResult (int coverage, int areaCoverage, int totalCost) : coverage(coverage), areaCoverage(areaCoverage), totalCost(totalCost) {}
};

vector<Sensor> lastSensorsUsed;
TrialResult runTrial(int algorithmChoice, int distributionChoice, int R_, int budget_, bool regenerateSensors = false) {

    R = R_;
    budget = budget_;
    ofstream output;
    output.open ("outputs/output.txt" + to_string(algorithmChoice) + "_" + to_string(distributionChoice) + "_" + to_string(budget), ofstream::out | ofstream::trunc); //truncate (erase) previous contents of the output file
    output << "Sensors:\n";

    int totalCost = 0;
    int totalCoverage = 0;

    vector<Sensor> sensors;
    if (regenerateSensors) {
        if(distributionChoice == 1)
        {
            sensors = generateSensorsUniformly(20);
        }
        else if(distributionChoice == 2)
        {
            sensors = generateSensorsClustered(20);
        }
        else if(distributionChoice == 3)
        {
            sensors = generateSensorsRandomly(20);
        }
        lastSensorsUsed = sensors;
    } else {
        sensors = lastSensorsUsed;
    }

    sensors = sortSensors(sensors);
    calculateCoverage(sensors);

    if (sensors.size() == 0) {
        cout << "MERP";
    }

    for (Sensor &s : sensors)
    {
        output << '(' << s.x << ", " << s.y << ", " << s.cost << ")\n";
    }

    vector<int> chosen;
    if(algorithmChoice == 1)
    {
        chosen = greedyAlgorithm(sensors);
    }
    else if(algorithmChoice == 2)
    {
        chosen = chooseSensorsRandomly(sensors);
    }
    else if(algorithmChoice == 3)
    {
        chosen = weightedAlgorithm(sensors);

    } else if (algorithmChoice == 4) {
        //time it
        auto start = chrono::steady_clock::now();

        chosen = dynamicAlgorithm(sensors);

        auto end = chrono::steady_clock::now();
        auto diff = end - start;
        cout << chrono::duration <double, milli> (diff).count() << " ms" << endl;
    }

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

    int area = calculateAreaCoverage(chosen, sensors);

    return TrialResult(calculateTotalCoverage(sensors, chosen), calculateAreaCoverage(chosen, sensors), totalCost);
}

void experiment() {
    ofstream output;
    output.open ("experiment_output.txt", ofstream::out | ofstream::trunc); //truncate (erase) previous contents of the output file

    output << "budget coverage\n";
    output << "pure_greedy random greedy dynamic\n";
    int budgets[] = {100, 120, 200, 250, 400};

    for (int i = 0; i < 5; i++) {
        int budget  = budgets[i];
        auto trial  = runTrial(1, 2, 5, budget, true);
        auto trial1 = runTrial(2, 2, 5, budget);
        auto trial2 = runTrial(3, 2, 5, budget);
        auto trial3 = runTrial(4, 2, 5, budget);
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
        //int budget  = budgets[i];
        int points = sensorAmounts[i];
        auto trial  = runTrial(1, 4, 5, budget, true);
        auto trial1 = runTrial(2, 4, 5, budget);
        auto trial2 = runTrial(3, 4, 5, budget);
        auto trial3 = runTrial(4, 4, 5, budget);

        output << budget
               << ' ' << trial.coveragePercent(points)
               << ' ' << trial1.coveragePercent(points)
               << ' ' << trial2.coveragePercent(points)
               << ' ' << trial3.coveragePercent(points) << endl;
    }
}

int main()
{
    experiment();
    return 0;
    //runTrial(3, 3, 15, 2500);
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

    TrialResult result = runTrial(algorithmChoice, distributionChoice, 5, 400);

    std::cout << "Total Cost: " << result.totalCost;
    std::cout << "\nTotal Coverage: " << result.coverage << ", " << result.coveragePercent(20) << '%' << endl;
    std::cout << "\nArea Coverage: " << result.areaCoverage << ", " << result.areaCoveragePercent() << '%' << endl;
}
