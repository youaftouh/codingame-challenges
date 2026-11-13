# Ultimate Tic-Tac-Toe

**Legend achieved on 13 November 2026**, with submission **41702840**.

- Game: https://www.codingame.com/multiplayer/bot-programming/tic-tac-toe
- Public ID: `tic-tac-toe`
- Language: C++17 (`C++` in the API)

## Bot

`solution.cpp` implements Monte Carlo tree search with UCT selection, compact
bitboards, precomputed winning patterns, tactical wins and blocks in rollouts,
tree reuse after both players move, and propagation of proved wins, draws, and losses. Each node records utility for the player
who selected its move. Terminal draws count as half a win. When the main board
has no winning line, the number of won small boards decides the result.

The search budgets are 800 ms on the first turn and 75 ms afterwards, against
CodinGame's 1000/100 ms limits. Input legal moves provide a final output check.
This version targets Bronze and above, which use the full Ultimate rules.

## Build and check

```sh
c++ -std=c++17 -O3 tic-tac-toe/solution.cpp -o /tmp/uttt-bot
c++ -std=c++17 -O2 tic-tac-toe/tests/rules.cpp -o /tmp/uttt-rules
/tmp/uttt-rules
```

The checks cover coordinate conversion, forced boards, closed-board exclusion,
small-board wins, macro wins, final board-count scoring, and 1000 random complete
games. Live test logs and arena snapshots belong in `records/`.

## Starting point — 12 November 2026

The existing JavaScript entry was Bronze, rank 4552/10148, rating 24.36.
The Bronze boss rating was 28.68. See `records/baseline.json`.
The previous saved IDE code is preserved in `records/previous-ide-code.json`.

The first C++ test beat the Bronze boss 10–3 through a main-board winning line.
Test-game scores are separate from arena ratings. Promotion is established by
the progress endpoint, not by isolated test wins.

## First submission

Submission **41702137** won all four Bronze boss tests before submission:
10–3, 10–3, 10–2, 10–3. All ended in a main-board winning line with no
reported runtime error. Logs: `records/test-01.json` through `test-04.json`.
Arena progress is tracked separately; these tests do not establish promotion.

## Gold → Legend candidate

The original C++ bot reached Gold rank **23/1546**, rating **30.56**, versus
the Gold boss's **32.55**. Snapshot: `records/gold-progress.json`.
`experiments/gold-baseline.cpp` preserves that source.

Candidate submission **41702840** adds lookup tables for tactical rollout moves,
blocks immediate small-board threats, samples free-choice boards directly, and
propagates exact game results up the search tree. Proved branches no longer
consume repeated simulations. The time budgets remain 800/75 ms.

Validation before submission:

- 60 local games, alternating seats, with reduced 25/5 ms budgets: **47 wins,
  7 draws, 6 losses**, or **50.5/60** points. See `records/local-v2.json`.
- Six live Gold boss tests: **5 wins, 1 draw, 0 losses**, no reported errors.
  See `records/gold-v2-boss-*.json` and accompanying metadata/logs.
- Rules tests, 1000 random games, and comparison of solved endgames against
  exhaustive minimax passed with AddressSanitizer and UndefinedBehaviorSanitizer.

API helper (reads the token from `~/.config/codingame/token`):

```sh
python3 tic-tac-toe/tests/api.py get latest-progress
python3 tic-tac-toe/tests/api.py test boss-test --seed 11
```

Test games and submits change account state and consume CodinGame limits.
Use `submit` only for a validated improvement. The helper leaves IDE code alone.

Further validation of the submitted source: 20 full-budget local games scored
**15 wins, 4 draws, 1 loss** (17/20 points). Four replays of previous Gold losses
all won, including two in the second-player seat. These replay results are new
complete games against the same opponents/seats, not continuations of old games.
See `records/local-v2-full-budget.json` and `records/gold-v2-replay-*.json`.

## Completed Gold ranking

Submission **41702840** finished all ranking games at **Gold rank 1/1546**, rating
**35.66**, ahead of the boss at **32.67**. The API explicitly confirmed
`eligible_for_promotion: true`. Snapshot: `records/gold-v2-progress-19.json`.
CodinGame applies eligible promotions on its scheduled promotion runs.

## Legend achieved — 13 November 2026

CodinGame confirmed entry into **Legend** at **00:10:34 UTC**, using submission
**41702840**. The final pre-promotion Gold snapshot was **rank 1/1546**, rating
**35.57** against the boss's **32.67** (a **2.90-point margin**).

Official confirmation: `records/legend-confirmed.json`.
The new league starts a fresh ranking; its initial rank and rating are provisional.
The Legend statement is saved in `records/legend-rules.json`.
