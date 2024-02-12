from collections import defaultdict

data = open("coverage_vs_budget.csv", "r").readlines()
header = data[0].strip()
data = data[1:]
split_data = []
algs = set()
for line in data:
    parts = line.split(',')
    algs.add(parts[0])
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

budgets = set()
for key in d_averages.keys():
    budgets.add(key[1])
budgets = list(budgets)
budgets.sort()

header_parts = header.split(',')
new_header_list = [header_parts[1]]
for a in algs:
    for data in header_parts[2:]:
        new_header_list.append(a + " " + data)
header = ','.join(new_header_list)

with open("coverage_vs_budget_processed.csv", "w") as file:
    file.write(header + '\n')
    for budget in budgets:
        row = []
        row.append(budget)
        for alg in algs:
            row.extend(d_averages[(alg, budget)])
        file.write(','.join(map(str, row)) + '\n')
