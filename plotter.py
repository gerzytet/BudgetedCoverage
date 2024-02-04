import random
import matplotlib.pyplot as plt
import math
import sys

if (len(sys.argv) > 1):
    name = sys.argv[1]
else:
    name = "output.txt"

print(name)
with open(name) as data:
    lines = list(map(str.strip, data.read().split('\n')))

points = []
index = 1
for line in lines[index:]:
    index += 1
    if not line.startswith('('):
        break
    points.append(tuple(map(int, line.strip('()').split(', '))))

x_data = []
y_data = []
color_data = []
for x, y, cost in points:
    x_data.append(x)
    y_data.append(y)
    color_data.append(((cost - 250) / 250) / 2 + 0.5)

while not lines[index].isnumeric():
    index += 1

chosen = []
for line in lines[index:]:
    if line.isnumeric():
        chosen.append(int(line))
    else:
        break

chosen_points = []
chosen_colors = []
for i in chosen:
    chosen_colors.append(color_data[i])
    chosen_points.append(points[i])

chosen_x = []
chosen_y = []

for x, y, _ in chosen_points:
    chosen_x.append(x)
    chosen_y.append(y)
    index += 1

while not lines[index].isnumeric():
    index += 1
R = int(lines[index])

colors = [(random.random() / 5) + 0.4 for i in range(len(chosen))]

fig = plt.figure()
fig.set_figwidth(5)
fig.set_figheight(5)
inch_per_unit = (math.pi) / 100
dots_per_inch = fig.dpi
dots_per_unit = inch_per_unit * dots_per_inch
fudge_factor = 1.1
area = math.pi * (R * dots_per_unit * fudge_factor) ** 2
print(area)

plt.ylim(0, 100)
plt.xlim(0, 100)
plt.scatter(chosen_x + [1000], chosen_y + [1000], marker='o', c=chosen_colors + [((cost - 250) / 250) / 2 + 0.5], s=[area]*(len(chosen) + 1), alpha = 0.4)
#plt.scatter([20], [20], marker='o', s=[area], alpha = 0.5)
plt.scatter(x_data, y_data, marker='X', c=color_data)
plt.show()
