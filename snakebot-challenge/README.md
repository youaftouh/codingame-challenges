# Snakebot Challenge - CodinGame

## Challenge Description
Control snake-like robots to collect power sources and grow bigger than your opponent.

## Game Rules
- Snakebots move on a grid with platforms
- They are affected by gravity
- Moving into power sources makes them grow
- Colliding with platforms or other snakebots destroys the head
- The player with the most body parts at the end wins

## Strategy Implemented
1. **Target Selection**: Each snakebot targets the closest power source
2. **Pathfinding**: Calculates direction towards target (prioritizing horizontal movement)
3. **Safety Checks**: Avoids immediate collisions with platforms and own body
4. **Fallback**: If no safe move to target, tries alternative directions

## How to Run
Copy the contents of `solution.js` to the CodinGame IDE and run.

## Key Functions
- `parseBody()`: Parses snakebot body coordinates from input
- `findClosestPowerSource()`: Finds nearest power source using Manhattan distance
- `getDirectionToTarget()`: Calculates best direction to move towards target
- `isSafeMove()`: Checks if a movement would be safe (no immediate collision)

## Potential Improvements
- Implement more sophisticated pathfinding (A* algorithm)
- Predict opponent movements
- Strategic positioning to block opponents
- Better handling of gravity effects
- Multi-turn planning instead of greedy approach
## Current local strategy

The bot searches team actions with a simulator for simultaneous movement,
head losses, food growth, and gravity. Supported snakes can support teammates;
unsupported snakes fall together. Food distances account for platforms, and
recently repeated positions receive a penalty to encourage exploration.
The search has a 28 ms budget against the game's 50 ms turn limit.
Opponent moves are predictions, so collision outcomes remain uncertain.

Run checks from the repository root:

```sh
node --check snakebot-challenge/solution.js
node snakebot-challenge/tests/gravity.cjs
node snakebot-challenge/tests/simulation.cjs
```

`tests/arena-baseline.js` preserves the latest retrievable historical submission,
40338748. The progress endpoint reports 40178235 as current, but that ID cannot
be retrieved through the submission endpoint. For comparisons, use the fixed
arena agent 6356674 from the opponents list.

The API opponent `me` runs self-play with the test code. Earlier self-play
results do not establish improvement over the arena submission.

## Submitted improvement (12 November 2026)

Submission **41700222** won all four comparison games against fixed arena agent
6356674: **20–4, 39–21, 28–21, 29–6**. These are test-game scores, separate
from the arena skill rating. Thirteen regression checks pass, and the four
comparison logs contained no reported runtime errors. Exact game IDs and the
submitted source hash are recorded in `tests/search-results.json`.

The pre-submission arena baseline was Silver, overall rank 1646/2481,
league rank 891/1221, and score 11.45. Arena results are recorded separately.

First ranking snapshot (13% complete, provisional): score **16.21**,
overall rank **1147/2481**, Silver rank **392/1221**.
See `tests/ranking-after.json`; these values may change as ranking completes.
