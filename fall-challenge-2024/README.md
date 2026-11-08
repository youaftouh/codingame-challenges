# Fall Challenge 2024 - Selenia City

Game: https://www.codingame.com/multiplayer/optimization/selenia-city

Public ID: `selenia-city` · Category: optimization

This directory holds our bot, game notes, and experiment records.
API endpoints use `/puzzles/selenia-city`.

## Starting snapshot

Fetched on 8 November 2026:

- Existing submission: `36115168` (Ruby).
- Score: **58.3333**; raw points: **304,150**.
- Rank: **2,464 / 4,287**.
- Existing IDE source saved in [baseline.rb](baseline.rb), with its API response in [code.json](code.json). The IDE code may differ from the ranked submission.
- Full rules: [statement.md](statement.md).
- Input format: [stub-generator.json](stub-generator.json).
- Game metadata and 12 visible test cases: [puzzle.json](puzzle.json).
- Ranking snapshot: [progress.json](progress.json).

## Game overview

Build a transport network over 20 months, each containing 20 days. Astronauts
arrive at landing pads each month and must reach modules matching their type.
Points reward fast arrivals and balancing arrivals between modules.

Tubes cost the distance multiplied by 10, rounded down, cannot cross or pass
through buildings, and allow at most five connections per building. Pods cost
1,000 resources and carry ten passengers. Directed teleporters cost 5,000
resources; each building can have one entrance or exit. Unspent resources earn
10% interest each month.

Persist building information across turns: input lists only newly constructed
buildings, alongside all current routes and pods. Astronaut routing uses tube
distance, with teleporters costing zero; pods must strictly reduce that distance.
The time limit is 500 ms per turn and 1,000 ms on the first turn.

## C++ bot

[bot.cpp](bot.cpp) simulates all 20 days of a month, including shortest-distance
passenger routing, pod capacity and priority, teleporters, and arrival balancing.
It greedily compares affordable tube-and-pod connections and teleporters by
simulated score improvement relative to cost. Geometry checks prevent crossing
tubes, passing through buildings, and exceeding the five-tube limit.

The search stops considering new purchases after 320 ms and evaluating candidates
after 420 ms, leaving time for input and output within the 500 ms limit. It
currently considers connections originating at landing pads; module-to-module
extensions, tube upgrades, and coordinated multi-purchase search are future
improvements. Its cost ranking optimizes the current month's gain rather than
explicitly planning future income and interest.

Run from the repository root:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File ./fall-challenge-2024/cg.ps1 -Action test -Testcase 1
powershell -NoProfile -ExecutionPolicy Bypass -File ./fall-challenge-2024/cg.ps1 -Action progress
```

Other actions: `poll -GameId <id>` and `submit`. Use `-Source <file.cpp>` to select
a source file. Responses and readable test logs are saved in `results`; submission
sources are saved by submission ID. The helper does not change IDE code.
Credentials stay outside this repository.

## Validation

The initial C++ bot completed all 12 visible test cases on 8 November 2026,
scoring **4,976,995 points** in total. No warnings or timeouts appeared. The
simulator's predicted monthly totals matched the actual final score on every
map. The slowest recorded turn took **409 ms** against the 500 ms limit.
See [results/validation.json](results/validation.json) for per-map scores and
timings, and the adjacent `.log` files for complete test output.

Submitted as `41671144`; the exact source is saved in
[results/submission-41671144.cpp](results/submission-41671144.cpp).
The first local C++ baseline is also preserved in [baseline-cpp.cpp](baseline-cpp.cpp).

The completed hidden-validator result was **5,323,131 points**, rank
**127 / 4,287**, with a displayed score of **100%**, checked on 8 November 2026
at 21:42 Dublin time. The previous submission had 304,150 points and ranked
2,464. A displayed score of 100% does not mean the transport plans are optimal;
raw points determine the optimization ranking. [progress.json](progress.json)
contains the latest result.
