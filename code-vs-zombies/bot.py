"""Receding horizon search; standalone Python 3 CodinGame entry."""
import sys
import math
import random
import time
import json

RNG = random.Random(731)
SEARCH_TIME = 0.082
HUMAN_CACHE = {}
FIB = [0, 1]
for _ in range(105):
    FIB.append(sum(FIB[-2:]))
COMBO = [0]
for i in range(1, 101):
    COMBO.append(COMBO[-1] + FIB[i + 1])


def move(x, y, tx, ty, speed):
    dx, dy = tx - x, ty - y
    d = math.hypot(dx, dy)
    if d <= speed:
        return tx, ty
    return int(x + dx * speed / d), int(y + dy * speed / d)


def advance(ash, humans, zombies, target, predicted=None):
    ax, ay = ash
    moved = []
    cache = HUMAN_CACHE.setdefault(tuple(humans), {})
    for i, (zx, zy) in enumerate(zombies):
        if predicted is not None:
            moved.append(predicted[i])
            continue
        cached = cache.get((zx,zy))
        if cached is None:
            best, tx, ty = 1e100, 0, 0
            for hx, hy in humans:
                d = (zx-hx)**2+(zy-hy)**2
                if d < best:
                    best, tx, ty = d, hx, hy
            cached = (best, move(zx,zy,tx,ty,400))
            cache[(zx,zy)] = cached
        if (zx-ax)**2+(zy-ay)**2 <= cached[0]:
            moved.append(move(zx,zy,ax,ay,400))
        else:
            moved.append(cached[1])
    ash = move(ax, ay, *target, 1000)
    ax, ay = ash
    alive = [z for z in moved if (z[0]-ax)**2 + (z[1]-ay)**2 > 4000000]
    points = 10 * len(humans)**2 * COMBO[len(zombies)-len(alive)]
    occupied = set(alive)
    humans = [h for h in humans if h not in occupied]
    return ash, humans, alive, points


def rescue_target(ash, humans, zombies, mode=0):
    ax, ay = ash
    best, target = -1e100, zombies[0]
    for h in humans:
        z = min(zombies, key=lambda z: (z[0]-h[0])**2+(z[1]-h[1])**2)
        danger = math.hypot(z[0]-h[0], z[1]-h[1]) / 400
        travel = max(0, (math.hypot(ax-h[0], ay-h[1])-2000)/1000)
        if travel <= danger + 0.5:
            value = -danger if mode == 0 else -travel
            if value > best:
                best, target = value, z
    if best == -1e100:
        target = min(zombies, key=lambda z: (z[0]-ax)**2+(z[1]-ay)**2)
    return target


def herd_target(ash, humans, zombies, margin):
    """Delay shooting while a human can still be defended; gather pursuers."""
    ax, ay = ash
    urgent, target = 1e100, zombies[0]
    for hx, hy in humans:
        z = min(zombies, key=lambda z: (z[0]-hx)**2+(z[1]-hy)**2)
        danger = math.hypot(z[0]-hx, z[1]-hy) / 400
        intercept = max(0, (math.hypot(z[0]-ax,z[1]-ay)-2000)/1400)
        slack = danger - intercept
        if slack >= -0.5 and slack < urgent:
            urgent, target = slack, z
    if urgent <= margin or urgent == 1e100:
        return target
    # Retreat only far enough to keep the leading zombie outside shooting range.
    z = min(zombies, key=lambda z: (z[0]-ax)**2+(z[1]-ay)**2)
    dx, dy = ax-z[0], ay-z[1]
    dist = math.hypot(dx,dy)
    if dist > 2900:
        return ash
    if dist == 0:
        dx, dy, dist = 1, 0, 1
    return (max(0,min(15999,int(z[0]+dx/dist*2900))),
            max(0,min(8999,int(z[1]+dy/dist*2900))))


def orbit_target(ash, zombies, parameter):
    ax, ay = ash
    zx, zy = min(zombies,key=lambda z:(z[0]-ax)**2+(z[1]-ay)**2)
    dx, dy = ax-zx, ay-zy
    distance = math.hypot(dx,dy) or 1
    radius = abs(parameter)
    if distance > radius+600:
        return (int(sum(z[0] for z in zombies)/len(zombies)),
                int(sum(z[1] for z in zombies)/len(zombies)))
    direction = 1 if parameter > 0 else -1
    outward = max(-0.5,min(1.5,(radius-distance+400)/800))
    vx = (dx*outward-dy*direction)/distance
    vy = (dy*outward+dx*direction)/distance
    return (max(0,min(15999,int(ax+vx*1500))),
            max(0,min(8999,int(ay+vy*1500))))


def rollout(ash, humans, zombies, predicted, genes, deadline):
    points = 0
    first = ash
    plan = []
    for turn in range(55):
        if not humans:
            return -1, first, plan
        if not zombies:
            return points, first, plan
        if time.perf_counter() >= deadline:
            break
        if turn < len(genes):
            target = genes[turn]
            plan.append(target)
            if target[0] == -1:
                target = herd_target(ash, humans, zombies, target[1])
            elif target[0] == -2:
                target = orbit_target(ash, zombies, target[1])
            elif target[0] == -3:
                refuge = (target[1],target[2])
                target = herd_target(ash, [refuge] if refuge in humans else humans,
                                     zombies, 1)
        else:
            target = rescue_target(ash, humans, zombies)
            plan.append(target)
        if turn == 0:
            first = target
        ash, humans, zombies, gain = advance(ash, humans, zombies, target,
                                              predicted if turn == 0 else None)
        points += gain
    if not humans:
        return -1, first, plan
    # Unfinished trajectories retain a conservative estimate of future kills.
    return points + 10 * len(humans)**2 * len(zombies) * 0.5, first, plan


def choose(ash, humans, zombies, predicted, previous):
    if sum(map(len,HUMAN_CACHE.values())) > 60000:
        HUMAN_CACHE.clear()
    deadline = time.perf_counter() + SEARCH_TIME
    fallback = rescue_target(ash, humans, predicted)
    candidates = [[], [fallback], [ash], [rescue_target(ash, humans, predicted, 1)]]
    if previous:
        candidates[0:0] = [plan[1:] for plan in previous[:8]]
    candidates[1:1] = [[(-1, margin)]*55 for margin in (1,2,3,5,8)]
    candidates[1:1] = [[(-2,r)]*n for n in (6,12,20,32)
                       for r in (2400,-2400,3000,-3000)]
    refuges = humans[:]
    RNG.shuffle(refuges)
    candidates[1:1] = [[h]*n for n in (6,12,20) for h in refuges[:8]]
    # Defend an isolated refuge first, retreat to let the distant wave merge,
    # then intercept it. These are geometric seeds, independent of test cases.
    routes = []
    for h in refuges[:8]:
        edges = [(h[0],0),(h[0],8999),(0,h[1]),(15999,h[1])]
        edges.sort(key=lambda p:(p[0]-h[0])**2+(p[1]-h[1])**2)
        for e in edges[:2]:
            for duration in (6,10,14):
                routes.append([(-1,2)]*2+[h]*duration+[e]*7)
    RNG.shuffle(routes)
    candidates[1:1] = routes
    # Coordinates are world targets, so shifted plans survive across turns.
    pool = predicted + humans + [(-1,m) for m in (1,2,3,5,8)]
    # Escape routes are essential: moving only towards enemies destroys the
    # leading zombies before the trailing wave has time to gather.
    escapes = [(0,0),(15999,0),(15999,8999),(0,8999),
               (8000,0),(8000,8999),(0,4500),(15999,4500)]
    for k in range(8):
        angle = k*math.pi/4
        escapes.append((max(0,min(15999,int(ash[0]+5000*math.cos(angle)))),
                        max(0,min(8999,int(ash[1]+5000*math.sin(angle))))))
    pool += escapes
    pool += [(-2,r) for r in (2400,-2400,3000,-3000)]
    pool += [(-3,*h) for h in humans]
    candidates[6:6] = [[p]*n for n in (4,8,12) for p in escapes[8:]]
    near = sorted(predicted, key=lambda z: (z[0]-ash[0])**2+(z[1]-ash[1])**2)[:10]
    candidates += [[z] for z in near]
    candidates += [[ash]*n for n in (3, 6, 10, 16)]
    candidates += [[h]*20 for h in humans[:12]]
    for a, b in zip(near, near[1:]):
        candidates.append([((a[0]+b[0])//2, (a[1]+b[1])//2)])
    best_score, best_action, best_genes = -2, fallback, []
    population = []
    iterations = 0
    while time.perf_counter() < deadline:
        if candidates and (not population or RNG.random() < 0.35):
            genes = candidates.pop(0)
        else:
            parent = max(RNG.choices(population, k=2), key=lambda p:p[0])
            genes = parent[1][:]
            if RNG.random() < 0.4:
                genes = []
                if RNG.random()<0.5:
                    h = RNG.choice(humans)
                    e = RNG.choice([(h[0],0),(h[0],8999),(0,h[1]),(15999,h[1])])
                    genes = [(-1,2)]*RNG.randint(0,3)+[h]*RNG.randint(3,14)+[e]*RNG.randint(3,12)
                else:
                    for _ in range(RNG.randint(1,4)):
                        waypoint = RNG.choice(humans if RNG.random()<0.7 else pool)
                        genes.extend([waypoint]*RNG.randint(1,16))
            while len(genes) < 16:
                genes.append(RNG.choice(pool))
            j = RNG.randrange(min(len(genes),24))
            if RNG.random() < 0.5:
                genes[j] = RNG.choice(pool)
            elif genes[j][0] >= 0:
                x, y = genes[j]
                genes[j] = (max(0, min(15999, x+RNG.randint(-2500,2500))),
                            max(0, min(8999, y+RNG.randint(-2500,2500))))
            if RNG.random() < 0.65:
                genes[j:j+RNG.randint(2,12)] = [genes[j]] * RNG.randint(2,12)
            if RNG.random() < 0.15:
                del genes[j:j+RNG.randint(1,5)]
            if RNG.random() < 0.15 and len(population) > 1:
                other = RNG.choice(population)[1]
                genes[j:] = other[j:]
            genes = genes[:55]
        score, action, plan = rollout(ash, humans, zombies, predicted, genes, deadline)
        if time.perf_counter() >= deadline:
            break
        if score > best_score:
            best_score, best_action, best_genes = score, action, plan
        if plan:
            population.append((score,plan))
            population.sort(key=lambda p:p[0],reverse=True)
            if len(population) > 24:
                population.pop()
        iterations += 1
    print('search', iterations, 'value', int(best_score), file=sys.stderr)
    return best_action, [p[1] for p in population[:12]]


def main():
    previous = []
    first_turn = True
    while True:
        line = sys.stdin.readline()
        if not line:
            return
        ash = tuple(map(int, line.split()))
        humans = [tuple(map(int, sys.stdin.readline().split()))[1:]
                  for _ in range(int(sys.stdin.readline()))]
        raw = [list(map(int, sys.stdin.readline().split()))
               for _ in range(int(sys.stdin.readline()))]
        zombies = [(z[1], z[2]) for z in raw]
        predicted = [(z[3], z[4]) for z in raw]
        if first_turn:
            print('INITIAL ' + json.dumps([ash, humans, zombies, predicted]), file=sys.stderr)
            first_turn = False
        action, previous = choose(ash, humans, zombies, predicted, previous)
        print(*action, flush=True)


if __name__ == '__main__':
    main()
