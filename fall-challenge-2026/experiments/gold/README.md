# Unreleased Gold experiments

These files are research candidates, not the arena source. The submitted bot is
the repository's `../../bot.cpp`, submission `41670762`. Source hashes, options,
and complete game logs are kept under `../../results/`.

| Candidate | Experiment | Evidence / status |
| --- | --- | --- |
| growth | Adaptive army size only | Lost Gold boss seed 1, game 918745679 |
| tactics | Prediction plus resolving deaths before capture | Lost boss seed 1, 918746104 |
| mobility | Prefer spawning on well-connected houses | Lost boss seed 1, 918746515 |
| reset | Restore the bypassed empty-triangle reset bonus | Lost boss seed 1, 918746764 |
| economy | Reduce early enemy-territory priority | Lost boss seed 1, 918747311 |
| fast | Bitset ownership/distances and post-death evaluation | Faster search, lost boss seed 1, 918747953 |
| model | Faster opponent forecasting | Lost boss seed 2, 918748303 |
| defense | Preserve armed triangles near enemy stacks | Lost boss seed 1, 918748695 |
| path | Reserve units to add secure triangle-forming paths | Won against agent 6865386, seed 10, 918751584, but no path action occurred; unvalidated feature |
| horizon | Joint moves/spawns with a second-turn estimate | Lost boss seed 2, 918753145, and the released bot on seed 10, 918753435 |
| rollout | Three-turn forward simulation, warm starts, and joint spawn decisions | Lost boss seed 2, 918757628; no timeout |

The rollout prototype models attacks, movement, spawning, surrounded-unit deaths,
capture eligibility, and persistent triangle ownership. It deliberately omits
path actions and approximates enemy decisions. Its simulated attack ordering and
future moves can differ from the real opponent. It needs independent simulator
validation and stronger match results before release.

To test a file from the repository root:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File ./fall-challenge-2026/cg.ps1 -Action test -Source experiments/gold/candidate-gold-rollout.cpp -Opponent boss -Seed 3
```

Do not infer that more lookahead or more search improves playing strength from
these prototypes: their match results did not justify replacing the live bot.
