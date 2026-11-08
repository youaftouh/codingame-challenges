"""Offline simulator/search benchmark using initial states captured in API logs."""
import json
import re
import pathlib
import sys
import bot


def load_cases():
    cases = {}
    for path in sorted(pathlib.Path(__file__).parent.joinpath('results').glob('*test-*.json')):
        data = json.loads(path.read_text(encoding='utf-8-sig'))
        match = re.search(r'INITIAL (.*)', data.get('log', ''))
        if match:
            cases[int(path.stem.split('-')[-1])] = json.loads(match[1])
    return cases


def play(state):
    ash, humans, zombies, predicted = state
    ash = tuple(ash)
    humans = list(map(tuple, humans))
    zombies = list(map(tuple, zombies))
    predicted = list(map(tuple, predicted))
    score, previous = 0, []
    for turn in range(150):
        if not humans or not zombies:
            break
        target, previous = bot.choose(ash, humans, zombies, predicted, previous)
        ash, humans, zombies, gain = bot.advance(ash, humans, zombies, target, predicted)
        score += gain
        # Reconstruct the next turn's authoritative zombie predictions.
        moved = []
        for z in zombies:
            target = min([ash]+humans, key=lambda h: (z[0]-h[0])**2+(z[1]-h[1])**2)
            moved.append(bot.move(*z, *target, 400))
        predicted = moved
    return score if humans and not zombies else 0


if __name__ == '__main__':
    cases = load_cases()
    selected = list(map(int, sys.argv[1:])) or sorted(cases)
    for case in selected:
        bot.RNG.seed(731)
        print(case, play(cases[case]), flush=True)
