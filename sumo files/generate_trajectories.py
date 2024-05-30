from collections import defaultdict
import math
from random import choice
import time
import traci
import sys
sumoBinary = "sumo"
sumoCmd = [sumoBinary, "-c", "london-seg4.100.sumocfg"]
sumoCmd += ["--no-warnings", "true"]



def getRandomRoute():
    while True:
        f, t = choice(edges), choice(edges)
        route = traci.simulation.findRoute(f, t)
        if route.edges == ():
            continue
        #route2 = traci.simulation.findRoute(t, f)
        #if route.edges == ():
        #    continue
        break
    return [f, t]

if len(sys.argv) != 2:
    print("No number of participants specified")
    sys.exit(1)
num_vehicles = int(sys.argv[1])
vehicle_route_endpoints = {}

traci.start(sumoCmd)

edges = traci.edge.getIDList()

for i in range(int(num_vehicles * 1.5)):
    vid = "v" + str(i)
    rid = "r" + str(i)
    route = getRandomRoute()
    vehicle_route_endpoints[vid] = route
    traci.route.add(rid, route)
    traci.vehicle.add(vid, rid)

data = defaultdict(list)

for step in range(1000):
    traci.simulationStep()
    vehicles = set(traci.vehicle.getIDList())
    for vid in vehicles:
        data[vid].append(traci.vehicle.getPosition(vid))

    #time.sleep(0.25)

traci.close()
#print(list(sorted(map(len, data.values()), reverse=True))[:num_vehicles])

scale_factor = 8

#get top num_vehicles trajectories based on length of path
chosen_data = list(sorted(data.items(), key=lambda x: len(x[1]), reverse=True))[:num_vehicles]
with open("trajectories.txt", "w") as output:
    xmin = math.inf
    ymin = math.inf
    xmax = -math.inf
    ymax = -math.inf
    for vehicle in chosen_data:
        for point in vehicle[1]:
            xmin = min(xmin, point[0])
            ymin = min(ymin, point[1])
            xmax = max(xmax, point[0])
            ymax = max(ymax, point[1])
    xdiff = xmax - xmin
    ydiff = ymax - ymin
    for vehicle in chosen_data:
        for point in vehicle[1][::scale_factor]:
            #round each point to 2 digits
            norm_point = (
                (point[0] - xmin) * (100 / xdiff),
                (point[1] - ymin) * (100 / ydiff)
            )
            print(' '.join(map(lambda x: str(round(x, 2)), norm_point)), file=output)
        print(file=output)
    output.close()
