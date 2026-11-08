# Silver replay investigation, 8 November 2026

Original submission: `41669889`, source [baseline-silver.cpp](baseline-silver.cpp).
New submission: `41670167`, source [bot.cpp](bot.cpp).

## Diagnosis

Lost arena battle `918711249` against Balorman was downloaded and replayed with
the unchanged source. Original battle: loss after 104 turns; baseline replay:
loss after 98 turns. Timed stochastic search produces different decisions on
replay, so the outcomes are comparisons, not byte-for-byte reproductions.

The original game turned at turn 16: enemy attacks killed nine units, followed
by four more on turn 17. We had no ATTACK logic. Our army peaked at 36, below
the 38-unit cap, so raising the cap would not address this game's turning point.

## Change

- Spend owned triangles on attacks against stacks of four or more enemies.
  Account for the cost of one stationary unit at each corner before generating
  moves or spawns, and avoid reusing the consumed triangle.
- Penalize stacking more than two units within reach of armed enemy triangles.
- Replace the quadratic nearest-unit scan with equivalent multi-source BFS.
  Keep the 32 ms search budget and existing population/spawn policy.

## Evidence

Final candidate: `candidate-combined.cpp`, identical to the submitted `bot.cpp`.

| Test | Game ID | Outcome for candidate |
| --- | --- | --- |
| Baseline agent 6864924, seed 10 | 918714558 | Win, turn 37 |
| Baseline agent 6864924, seed 11 | 918714713 | Win, turn 54 |
| Baseline agent 6864924, seed 12 | 918714826 | Loss, turn 194 |
| Baseline agent 6864924, seed 13 | 918715106 | Win, turn 195 |
| Baseline agent 6864924, seed 14 | 918715296 | Win, turn 34 |
| Replay Balorman loss 918711249 | 918715486 | Loss, turn 90 |
| Silver boss, seed 1 | 918715703 | Win, turn 37 |
| Replay QProgS win 918710541 | 918715888 | Win, turn 30 |

No invalid-action or timeout errors were reported for our bot in these final
validation logs. The Balorman replay contains rejected moves from the opponent.
The 4/5 baseline wins are encouraging, but a small sample does not guarantee a
better arena rating or promotion. The Balorman matchup remains unsolved.

The earlier combat-only candidate won the Balorman replay once, while the
attack-only and search-only variants were also explored. Tests using opponent
`me` were discovered to be self-play and are excluded from baseline evidence.
The helper now rejects `me` and records source hashes/opponents for new tests.

Full logs and submission snapshots are in [results](results/).

## Completed arena ranking

At 19:40 UTC, submission `41670167` completed 100% of its ranking games with
rating **32.08**, up from the original submission's **29.41** before the change.
It was first among Silver players, but the boss's rating was **32.21**, leaving
a **0.13** gap. Promotion eligibility was false. The new source is retained;
Gold has not been confirmed. Latest state is in [progress.json](progress.json),
with checkpoints in [results/silver-improvement-rating-history.json](results/silver-improvement-rating-history.json).

## Rollback

Restore the old arena source with this explicit submission command:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File ./fall-challenge-2026/cg.ps1 -Action submit -Source baseline-silver.cpp
```

This starts fresh ranking games; it does not restore the old rating immediately.
To restore the local editor file too, copy `baseline-silver.cpp` over `bot.cpp`.
