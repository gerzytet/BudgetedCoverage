data = open("rounds_output.txt").read().strip()

participants, chosen, coverage, cost_per_coverage, average_price = [], [], [], [], []
lists = [participants, chosen, coverage, cost_per_coverage, average_price]
for row in map(lambda x: list(map(float, x.split())), data.split('\n')):
    for i, data in enumerate(row):
        lists[i].append(data)

import matplotlib.pyplot as plt

def display(data, ylabel):
    plt.plot(range(1, len(lists[0])+1), data)
    plt.xlabel("round")
    plt.ylabel(ylabel)
    plt.ylim(bottom=0)
    plt.show()

display(participants, "number of participants")
display(coverage, "coverage")
display(average_price, "average price")
