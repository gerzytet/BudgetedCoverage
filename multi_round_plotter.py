data = open("multi_round_output.txt").read().strip()

algs, budgets, avg_participants, avg_coverage, avg_cost = [], [], [], [], []
lists = [algs, budgets, avg_participants, avg_coverage, avg_cost]
for row in map(lambda x: [x.split()[0]]+list(map(float, x.split()[1:])), data.split('\n')):
    for i, data in enumerate(row):
        lists[i].append(data)
algs_unique = set(algs)
print(algs, algs_unique)

import matplotlib.pyplot as plt

def display(data, ylabel):
    alg_list = list(algs_unique)
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
