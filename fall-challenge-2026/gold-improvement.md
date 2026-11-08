# Gold investigation, 8 November 2026

Baseline: submission `41670167`, Gold agent `6865148`, source
[baseline-gold.cpp](baseline-gold.cpp). At the start of this investigation its
completed Gold rating was 6.44, league rank 398. Ratings reset across leagues;
the earlier Silver rating of 32.08 is not comparable to a Gold rating.

Candidate: [candidate-gold-predict.cpp](candidate-gold-predict.cpp), submitted
unchanged as `41670762` and copied to [bot.cpp](bot.cpp).

## Selected change

Predict an enemy move by reversing player identities and running a short legal
placement search. Score our placements against both the current enemy positions
(35%) and the predicted move/spawns (65%). Prediction uses the first 5 ms of
the existing 32 ms search budget; it does not add another 5 ms to the deadline.

Replace the fixed 38-unit cap with an adaptive target:
`min(3 * houseCount, max(houseCount, enemyUnits + 20))`. Allow spawning until
turn 174, rather than stopping after turn 154.

Gold unlocks ADD_PATH and REMOVE_PATH; the full rules and input format are saved
in [statement-gold.md](statement-gold.md) and
[stub-generator-gold.json](stub-generator-gold.json). The selected candidate
reads the whole input but does not yet use path actions.

## Validation

| Opponent / original battle | Test game | Result |
| --- | --- | --- |
| Baseline agent 6865148, seed 10 | 918748832 | Win, turn 41 |
| Baseline agent 6865148, seed 11 | 918749066 | Win, turn 46 |
| Baseline agent 6865148, seed 12 | 918749214 | Win, turn 26 |
| Replay loss to ovidiu.caba, 918734774 | 918745011 | Win, turn 44 |
| Replay loss to Pingwi85, 918734150 | 918749416 | Win, turn 58 |
| Replay win against nkokhre, 918734741 | 918749658 | Win, turn 20 |
| Gold boss, seed 1 | 918745324 | Loss, turn 24 |

The candidate is a tested improvement over the preserved bot, not a demonstrated
Legend bot. Replays have the same map, seats, and opponent code, but timed
stochastic search can produce different decisions each run.

Other experiments isolate adaptive growth, spawn mobility, opponent prediction,
surrounded-unit resolution, triangle reset scoring, a faster bitset evaluator,
and preserving armed triangles. None of those tested variants beat the Gold
boss. They remain separate files; their changes were not folded into the release
without positive validation. They are archived under
[experiments/gold](experiments/gold/README.md). The faster evaluator reached about 10,000 search
iterations in some turns, but additional search alone did not solve the matchup.

A conservative path prototype won a comparison without using ADD_PATH, so that
does not validate its path behavior. A joint two-turn planner lost to both the
boss and the released candidate. A three-turn forward simulation prototype also
lost its boss test. These experiments were withheld from the arena release.

One specific issue found during investigation: the original `empty == 3` reset
bonus is unreachable in its final `else` branch, because that condition enters
the preceding branch. A separate reset-bonus experiment did not beat the boss;
the chosen release retains the baseline scoring and changes prediction/growth.

## Completed arena result

At 21:16 UTC, submission `41670762` reached 100% completion:

- Gold rating **10.70**, versus the previous snapshot's **6.44**.
- Gold position **313**, versus **398** before the change.
- Global position **459**, versus **544** before the change.
- Boss rating **31.50**; promotion eligibility **false**.

The release is retained. Legend has not been reached. Exact final state is in
[progress.json](progress.json); checkpoints are in
[results/gold-improvement-rating-history.json](results/gold-improvement-rating-history.json).
The early production gap against the boss remains the main unsolved matchup.

## Rollback command

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File ./fall-challenge-2026/cg.ps1 -Action submit -Source baseline-gold.cpp
```

This restores the previous source in a new submission and starts new ranking
games; it does not immediately restore a historical rating.
