# CodinGame Sponsored Contest — VB.NET

Game: https://www.codingame.com/multiplayer/optimization/codingame-sponsored-contest

`bot.vb` is the balanced VB.NET exploration bot, developed from the account's existing Groovy strategy. `previous-code.json` preserves that source before any new submission. `baseline.vb` preserves the original VB.NET port.

## Working interpretation

The statement deliberately hides the mechanics. The old Groovy solution was itself a guess, not a reference implementation or verified specification. The interpretations below describe the current bot's hypotheses; future work should validate them through controlled input/output experiments.

The bot interprets the initial integers as height, width, and entity count. Each turn is interpreted as the north, east, south, and west neighboring cells, then one coordinate pair per entity. It treats the last entity as the player, earlier entities as threats, `_` as open space, and `#` as a wall.

Assumed actions: `A` right, `B` wait, `C` up, `D` down, `E` left.

The baseline tries right, down, left, then up. It only enters open cells more than one Manhattan step from every threat, and waits if none qualifies. The exploration bot retains that immediate safety heuristic, remembers observed walls and visited cells, and searches known passages for nearby unvisited cells. It favors targets farther from enemies and discourages repeated visits. It models map-edge passages with wrapping; the successful boundary-case test supports this model but does not establish all boundary rules. The bot reads the entire turn and flushes its output. Enemy behavior and the exact scoring formula still require controlled experiments.

## Previous arena result

Groovy submission 18179869: 100% completion, 1,574 Points, rank 2,206 of 21,167 when inspected on 2026-11-08. Completion percentage is separate from the optimization points.

## VB.NET public baseline

All ten public cases completed without compilation, runtime, or timeout errors. Points by case: 100, 148, 124, 132, 80, 40, 48, 70, 90, 120 (total 952). Public points are not directly comparable to hidden validator points.

## Exploration results

| Case | Baseline | Exploration | Cautious variant |
| --- | ---: | ---: | ---: |
| 1 | 100 | 190 | 178 |
| 2 | 148 | 266 | 188 |
| 3 | 124 | 558 | 246 |
| 4 | 132 | 102 | 138 |
| 5 | 80 | 48 | 48 |
| 6 | 40 | 388 | 388 |
| 7 | 48 | 100 | 214 |
| 8 | 70 | 110 | 124 |
| 9 | 90 | 166 | 184 |
| 10 | 120 | 680 | 360 |
| Total | 952 | 2608 | 2068 |

Exploration logs are `explore-N.json`, with the corrected boundary-passage run for case 10 in `explore-fixed-10.json`. The initial case-10 run exposed an index error at a boundary passage; the corrected run completed successfully. Cautious logs are `cautious-N.json`. Both source variants are retained. `test.ps1` runs selected public cases and saves full responses, including rate limits.

Submitted exploration as VB.NET submission **41671453** on 2026-11-08. The CodinGame IDE source was not overwritten.

The cautious variant was submitted as **41671463**. A subsequent completed progress response reported a new best of **2,298 Points**, rank **1,455 / 21,167**, in VB.NET: +724 points (+46.0%) over the previous Groovy best. Progress reports the account's best score, not an individual submission's validator score.

A balanced variant reduces the immediate enemy-clearance penalty from 20 to 5. Its public scores are 178, 534, 246, 394, 48, 388, 214, 124, 212, 372, totaling **2,710**. It was submitted as **41671477**. `balanced.vb` and `balanced-N.json` preserve that experiment.

Final completed progress after submission 41671477: **2,442 Points**, rank **1,338 / 21,167**, VB.NET, 100% completion. This improves the original best by **868 Points (+55.1%)**. `bot.vb` matches `balanced.vb`; `progress-balanced.json` stores the final response. The account automatically retains its highest validator score.
