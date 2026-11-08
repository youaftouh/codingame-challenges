# A*Craft — TypeScript

Game: https://www.codingame.com/multiplayer/optimization/a-star-craft

`bot.ts` is a standalone CodinGame solution. It simulates toroidal movement,
fixed arrows, arrows under starting robots, and each robot's independent history
of position/direction states. It maximizes the sum of robot lifetimes using
simulated annealing with repeated cooling, mutations biased toward visited cells,
and preservation of the best solution. Search runs for 820 ms after reading input,
leaving headroom under the one-second response limit.

## Run through the API

Uses the existing credential at `~/.config/codingame/token`, without storing it
in the repository. Run from the repository root:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File a-star-craft/cg.ps1 test -Case 1
powershell -NoProfile -ExecutionPolicy Bypass -File a-star-craft/cg.ps1 submit
powershell -NoProfile -ExecutionPolicy Bypass -File a-star-craft/cg.ps1 progress
```

The helper saves responses and text logs under `results/`. Submissions update the
arena solution; the helper does not overwrite code in the website IDE.

## Initial validation — 2026-11-08

| Official case | No-arrow points | Solver points | Search time |
| --- | ---: | ---: | ---: |
| 1: Simple | 12 | 23 | 820 ms |
| 10: Multiple 3x3 Platforms | 22 | 82 | 820 ms |
| 30: Split | 52 | 462 | 820 ms |

All three referee point totals matched the local simulator's predictions.
The TypeScript compiled and ran on CodinGame without timeouts on these cases.
Scores can vary with the number of search iterations available on the runner.
Initial submission: `41671533`. Completed hidden-validator evaluation: **6,009
points**, rank **461 / 2,386** (API completion score: 100%). These are a baseline, not an exhaustive test of
all 30 cases or a claim of optimality.

Potential next improvements: cache unchanged robot paths, avoid mutations that
point into void, and add larger path rerouting moves to escape local optima.
