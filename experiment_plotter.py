import random
import matplotlib
import matplotlib.pyplot as plt

with open('experiment_output.txt') as file:
    lines = file.read().strip().split('\n')

xaxis, yaxis = lines[0].split()
plt.xlabel(xaxis)
plt.ylabel(yaxis)
algs = lines[1].split()
keys = []
valuess = [[], [], []]
for line in lines[2:]:
    key = float(line.split()[0])
    values = list(map(float, line.split()[1:]))
    keys.append(key)
    for i, value in enumerate(values):
        valuess[i].append(value)

for key, values, alg in zip(keys, valuess, algs):
    print(keys, values)
    line, = plt.plot(keys, values, label=alg)
    line.set_label(alg)
    plt.legend()
plt.show()