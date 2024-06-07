from collections import defaultdict

data = open("multi_round_output.txt").read().strip()

algs, budgets, avg_participants, avg_coverage, avg_cost = [], [], [], [], []
lists = [algs, budgets, avg_participants, avg_coverage, avg_cost]
for row in map(lambda x: [x.split()[0]]+list(map(float, x.split()[1:])), data.split('\n')):
    for i, data in enumerate(row):
        lists[i].append(data)
algs_unique = set(algs)
print(algs, algs_unique)

d = defaultdict(list)
for i in range(len(algs)):
    d[(algs[i], budgets[i])].append((avg_participants[i], avg_coverage[i], avg_cost[i]))
#for each group, calculate average of each metric
#replace value of d with the average
for key in d:
    d[key] = tuple(map(lambda x: sum(x)/len(x), zip(*d[key])))
algs, budgets, avg_participants, avg_coverage, avg_cost = [], [], [], [], []
for key in d:
    algs.append(key[0])
    budgets.append(key[1])
    avg_participants.append(d[key][0])
    avg_coverage.append(d[key][1])
    avg_cost.append(d[key][2])

import matplotlib.pyplot as plt

def display(data, ylabel):
    alg_list = list(algs_unique)
    alg_list.sort()
    artists = []
    for alg in alg_list:
        new_data = []
        new_budgets = []
        for i in range(len(data)):
            if algs[i] == alg:
                new_data.append(data[i])
                new_budgets.append(budgets[i])
        #print(new_data)
        artists.append(plt.plot(new_budgets, new_data)[0])

    print(artists)
    plt.xlabel("budget")
    plt.ylabel(ylabel)
    plt.ylim(bottom=0)
    plt.legend(artists, alg_list)
    plt.show()

display(avg_participants, "average number of participants")
display(avg_coverage, "coverage")
display(avg_cost, "average price")
