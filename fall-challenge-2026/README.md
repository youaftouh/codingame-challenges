# Fall Challenge 2026

CodinGame contest: https://www.codingame.com/contests/fall-challenge-2026

Public ID: `fall-challenge-2026`

This directory holds our bot, game notes, and experiment records.

## Contest snapshot

- Current league: Gold (promoted through Silver on 8 November 2026).
- Ends 9 November 2026 at 09:00 UTC (Dublin time).
- Initial C++ bot submitted as `41669889`.
- Silver improvement submitted as `41670167`; old source saved in [baseline-silver.cpp](baseline-silver.cpp).
- Gold prediction/growth candidate submitted as `41670762`; promoted source saved in [baseline-gold.cpp](baseline-gold.cpp).
- Completed Gold result: rating **10.70** (previously **6.44**), league position **313** (previously **398**), global position **459**. Legend promotion has not been reached (8 November, 21:16 UTC).
- Historical Silver ranking: rating **32.08**, first among Silver players, versus boss **32.21** at 19:40 UTC. The bot was subsequently promoted to Gold.
- Rules: [statement.md](statement.md).
- Silver rules: [statement-silver.md](statement-silver.md).
- Gold rules: [statement-gold.md](statement-gold.md).
- Input format: [stub-generator.json](stub-generator.json).
- Latest ranking state: [progress.json](progress.json).

## Game overview

Capture triangles by owning all three corner houses. Owned triangles earn one
point per turn and remain owned until spent or captured by the opponent. Spend a
triangle to spawn a unit, but recapturing it requires vacating its corners first
or letting the opponent capture it. Move units along graph edges; numerical
superiority and surrounding houses determine ownership. Surrounded units die.

The Gold statement lists MOVE, SPAWN, ATTACK, ADD_PATH, and REMOVE_PATH commands,
a 200-turn game, and a 50 ms turn limit (1000 ms on the first turn). ATTACK spends
one stationary unit per triangle corner and kills every enemy unit on an adjacent
target house. Path actions also cost one stationary unit per triangle corner.

## Bot

[bot.cpp](bot.cpp) uses all-pairs graph distances and coordinated three-unit
approach plans, followed by a placement search with single and paired mutations.
It rewards immediate captures, progress towards other triangles, surrounding
enemy units, and clearing spent triangles for reuse. Early spawning grows the
army up to 38 units; after turn 154 it preserves income. Search has a 32 ms
budget, leaving room within the 50 ms turn limit for input and output.

The Silver version adds attacks against stacks of at least four enemy units,
accounts for their three-unit cost before planning moves, and avoids large stacks
in range of currently armed enemy triangles. Multi-source BFS computes nearest
unit distances more efficiently, allowing more search within the same budget.
The opponent model still uses current positions, so simultaneous moves can foil
a capture or invalidate a later attack.

See [silver-improvement.md](silver-improvement.md) for replay diagnosis,
validation, limitations, and rollback instructions.

The current Gold version also predicts enemy movement and grows its army in
response to the opponent's unit count. See [gold-improvement.md](gold-improvement.md)
for the selected changes, tests, boss limitation, and Gold rollback command.

## Validation and API helper

Won all four Bronze boss games on seeds 1–4, in 24, 18, 33, and 49 turns.
No timeouts or invalid-action errors appeared in their logs. Full responses,
logs, validation summary, and an exact submission source snapshot are in
[results](results/).

Submission `41669889` also beat the Bronze boss in arena battle `918700787`.
At the last check (8 November 2026, 18:52 UTC), ranking was 31% complete:
league rank 1, rating 21.28 versus boss rating 21.57. Promotion was still pending;
these are provisional results, not a final ranking.

Run the helper from the repository root (authentication is read from the private
user configuration directory):

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File ./fall-challenge-2026/cg.ps1 -Action test -Seed 5
powershell -NoProfile -ExecutionPolicy Bypass -File ./fall-challenge-2026/cg.ps1 -Action progress
```

Other actions: `poll -GameId <id>`, `game -GameId <id>`,
`replay -GameId <id>`, `battles`, `challenge`, and `submit`.
Use `-Source <file.cpp>` to select a bot and `-Opponent <agent_id>` to pin an
arena opponent. `me` is deliberately rejected because it creates self-play.
`submit` replaces the arena submission. Responses are stored locally. The helper
does not overwrite IDE code or play settings.

Authentication credentials are kept outside this repository in the user's
CodinGame configuration directory.
