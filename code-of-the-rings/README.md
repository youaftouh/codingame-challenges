# Code of the Rings (D)

[Play the optimization puzzle](https://www.codingame.com/multiplayer/optimization/code-of-the-rings).

`bot.d` reads one uppercase phrase (including spaces) and emits a program for
the puzzle's machine. The objective is to minimize emitted program length.

The machine has 30 circular cells, each holding a value from space through Z
(modulo 27). `<>` move, `+-` change values, `.` prints, and `[]` loop while the
current cell is nonzero. The phrase has at most 500 characters; the solver has
2 seconds and must output fewer than 4000 instructions.

The initial D solver chooses the cheapest cell for each character, considering
both travel and rune changes. It also recognizes repeated blocks up to 14
characters long. When cheaper than ordinary emission, it initializes adjacent
cells and uses a separate countdown cell to print up to 26 repetitions.
It tracks the resulting machine state after every operation.

CodinGame language identifier: `D`. No local D compiler was available during
setup; compilation and public-case validation use the official API runner.
`test-results.json` records the public test responses.

The pre-existing account submission was Groovy: 100% correctness, 11,489
instructions, rank 6,588 / 9,593 at setup on 2026-11-08. Public-case totals
and hidden-validator totals are different benchmarks.

Potential next improvements: lookahead or beam search for cell assignment,
loops for changing sequences, and better handling of long repeated patterns.

Initial validation: all 24 public cases passed, totaling 4,675 instructions
against 11,499 for the previous algorithm on the same phrases (59.3% fewer).
Submitted as D, submission `41671330`. Completed hidden validation: 100%
correctness, 4,679 instructions, rank 681 / 9,593 (previously 6,588).
`progress.json` records the result.
