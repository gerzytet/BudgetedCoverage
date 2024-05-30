#1:
#algorithm,budget,coverage,cost_per_coverage,[distribution],time
#2:
#algorithm,coverage,cost_per_coverage,[distribution],radius,time

r_files = {}
b_files = {}
with open("experiment4_full.csv") as data:
    for line in data:
        if line.startswith("algorithm"):
            continue
        alg, budget, cov, cost, dist, radius, time = line.split(',')
        params = [alg, budget, cov, cost, dist, radius, time]

        r = False#radius != '11' or (radius == '11' and budget == '1500')
        b = True
        if r:
            if dist not in r_files:
                f = open(f"experiment3_radius_{dist}.csv","w")
                r_files[dist] = f
                f.write("algorithm,coverage,cost_per_coverage,radius,time\n")
            r_files[dist].write(','.join([alg, cov, cost, radius, time]))
        if b:
            if dist not in b_files:
                f = open(f"experiment3_budget_{dist}.csv","w")
                b_files[dist] = f
                f.write("algorithm,budget,coverage,cost_per_coverage,time\n")
            b_files[dist].write(','.join([alg, budget, cov, cost, time]))

for d in (r_files, b_files):
    for f in d.values():
        f.close()
