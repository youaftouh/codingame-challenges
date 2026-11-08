# Code vs Zombies

Game: https://www.codingame.com/multiplayer/optimization/code-vs-zombies

The original Groovy solution is preserved in `baseline.groovy` and `code.json`.
`bot.py` is the Python 3 implementation. `bot.cpp` uses C++ to search substantially
more plans within the 100 ms turn limit.

Both versions simulate zombie targeting, integer movement, shooting before
humans are eaten, and Fibonacci combo scoring. They evaluate complete routes,
including waiting and retreating to gather zombies, then execute only the first
move and search again using the next authoritative input. The C++ version keeps
eight populations of competing plans and shifts their routes forward each turn.
It searches for 85 ms, leaving time for input/output and the final simulation.
All routes are generated from the current state; no test layouts are hardcoded.

## Confirmed leaderboard results

8 November 2026 (Dublin time):

| Version | Points | Rank |
| --- | ---: | ---: |
| Original Groovy | 60,860 | 3,322 / 25,812 |
| First Python submission, 41671696 | 108,310 | 1,176 / 25,812 |
| Tuned Python submission, 41671756 | 149,890 | 977 / 25,812 |
| C++ submission, 41671783 | 263,870 | 293 / 25,812 |
| Final C++ submission, 41671808 | **332,700** | **207 / 25,812** |

The revised target of 300,000–400,000 points was achieved. The final C++ result is
5.47 times the original score. `bot.cpp` matches the final submission snapshot
`results/submission-41671808-bot.cpp`. Search throughput and results vary with runner
speed, even with a fixed random seed. Offline plans found with much longer
budgets are experiments, not leaderboard results.

## Run

From the repository root, using the existing token at
`~/.config/codingame/token`:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File code-vs-zombies/cg.ps1 test -Case 17
powershell -NoProfile -ExecutionPolicy Bypass -File code-vs-zombies/cg.ps1 test -Case 17 -Language Python3
powershell -NoProfile -ExecutionPolicy Bypass -File code-vs-zombies/cg.ps1 submit
powershell -NoProfile -ExecutionPolicy Bypass -File code-vs-zombies/cg.ps1 progress
```

The default language is C++. Test responses, logs, submission snapshots, and
progress records are saved under `results/`. Submissions enter the leaderboard;
the helper does not overwrite the code in the website IDE. The API's progress
endpoint reports the best score, and can briefly show the preceding best while
the newest submission is queued.

For local simulator checks:

```sh
python -m unittest discover -s code-vs-zombies -p test_sim.py
python code-vs-zombies/bench.py 17 18
python code-vs-zombies/experiment.py 17 30
```

`bench.py` loads initial states captured from official test logs. `experiment.py`
explores longer offline search budgets. Its saved plans are never loaded by
either submitted bot. Four simulator tests cover movement rounding, turn order,
combo scoring, and targeting Ash before Ash moves. The C++ implementation is
compiled and exercised through official API test games.

The final C++ source passed all 21 official cases without reported errors or
timeouts, totaling 267,460 visible-test points. Per-case records are in
`results/final-validation.json`. The 332,700-point leaderboard result comes from
the separate hidden validators; these two totals are not directly comparable.
