"""Long offline trajectory search, to investigate search quality limits."""
import bot
import bench
import random
import time
import sys
import json

case = int(sys.argv[1])
seconds = float(sys.argv[2]) if len(sys.argv)>2 else 30
ash, humans, zombies, predicted = bench.load_cases()[case]
ash = tuple(ash)
humans, zombies, predicted = [list(map(tuple,x)) for x in (humans,zombies,predicted)]
pool = zombies+humans+[(x,y) for x in (0,4000,8000,12000,15999) for y in (0,4500,8999)]
pool += [(-1,m) for m in (1,2,3,5,8)] + [(-2,r) for r in (2400,-2400,3000,-3000)]
population = []
end = time.perf_counter()+seconds
best = -2
iterations = 0
while time.perf_counter()<end:
    if not population or random.random()<0.15:
        genes=[]
        for _ in range(random.randrange(1,6)):
            genes += [random.choice(pool)]*random.randint(1,18)
    else:
        genes = max(random.choices(population,k=3),key=lambda p:p[0])[1][:]
        j = random.randrange(len(genes))
        if random.random()<0.6:
            genes[j:j+random.randrange(1,16)] = [random.choice(pool)]*random.randrange(1,16)
        else:
            del genes[j:j+random.randrange(1,10)]
        if random.random()<0.25:
            genes[j:] = random.choice(population)[1][j:]
    score, action, plan = bot.rollout(ash,humans,zombies,predicted,genes,1e100)
    population.append((score,plan))
    population.sort(key=lambda p:p[0],reverse=True)
    population=population[:100]
    if score>best:
        best=score
        print(iterations,int(best),flush=True)
        with open('code-vs-zombies/results/experiment-%s.json'%case,'w') as f:
            json.dump({'score':score,'plan':plan},f)
    iterations+=1
