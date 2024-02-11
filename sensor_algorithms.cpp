#include "sensor_algorithms.hpp"
#include "random_utils.hpp"
#include "sensor_algorithms.hpp"

vector<int> bsToVector(bs mask) {
    vector<int> chosen;
    for (int i = 0; i < mask.size(); i++) {
        if (mask.test(i) > 0) {
            chosen.push_back(i);
        }
    }

    return chosen;
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
};

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
