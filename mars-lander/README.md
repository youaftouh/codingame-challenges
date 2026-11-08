# Mars Lander

Game: https://www.codingame.com/multiplayer/optimization/mars-lander

Public ID: `mars-lander` · Category: optimization · Language: **Go**

Land vertically on the unique flat area with horizontal speed at most 20 m/s
and vertical speed at most 40 m/s. Among successful landings, minimize fuel used.
Gravity is 3.711 m/s². Rotation changes by at most 15 degrees per second and
thrust by at most one unit. The response limit is 100 ms per turn.

[bot.go](bot.go) simulates trajectories and searches for safe landings with more
fuel remaining. A feedback controller generates initial trajectories, then a
rolling search mutates angle and thrust sequences for up to 78 ms per turn.
It retains the previous plan and sub-meter predicted state when observations
agree with the simulator.

Rules: [statement.md](statement.md). Input format: [stub-generator.json](stub-generator.json).
Original IDE response: [code.json](code.json). Credentials stay outside the repository.

## Validation

All five visible scenarios landed safely on 8 November 2026. The maximum
recorded search time was 78 ms; no errors or timeouts appeared.

| Scenario | Fuel remaining |
| --- | ---: |
| Easy on the right | 290 |
| Initial speed, correct side | 297 |
| Initial speed, wrong side | 464 |
| Deep canyon | 511 |
| High ground | 689 |

Full logs and [validation.json](results/validation.json) are in `results`.
The initial Go source is preserved in [baselines/initial.go](baselines/initial.go).
Submitted as `41671239`; the exact submitted source is saved in `results`.

Completed validator result: **100%**, **2,265 fuel left**, rank **390 / 8,789**,
checked on 8 November 2026 at 21:53 Dublin time. See [progress.json](progress.json).
The completion percentage is separate from the fuel optimization ranking.

The search uses a time budget, so trajectories and fuel consumption can vary
with execution speed. These tests establish a working baseline; they do not
establish minimum possible fuel use.

## API helper

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File ./mars-lander/cg.ps1 -Action test -Testcase 1
powershell -NoProfile -ExecutionPolicy Bypass -File ./mars-lander/cg.ps1 -Action progress
```

Other actions: `poll -GameId <id>` and `submit`. Use `-Source <file.go>` to select
a source file. The helper saves test logs and submission snapshots and leaves
IDE code unchanged. Local Go tooling was unavailable during setup; compilation
and execution were verified through the CodinGame Go runner.
