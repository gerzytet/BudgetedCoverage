from collections import defaultdict

data = open("coverage_vs_budget.csv", "r").readlines()
header = data[0]
data = data[1:]
split_data = []
for line in data:
    parts = line.split(',')
    parts = [parts[0]] + list(map(float, parts[1:]))
    split_data.append(parts)

d = defaultdict(list)
for line in split_data:
    d[tuple(line[:2])].append(line[2:])

d_averages = {}
for key, value in d.items():
    total = [0] * len(value[0])
    for item in value:
        for i in range(len(item)):
            total[i] += item[i]
    for i in range(len(total)):
        total[i] /= len(value)
    d_averages[key] = total

with open("coverage_vs_budget_processed.csv", "w") as file:
    file.write(header + '\n')
    for key, value in d_averages.items():
        row = list(key) + value
        file.write(','.join(map(str, row)) + '\n')
