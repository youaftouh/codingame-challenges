# Don't Panic — code golf

Puzzle: https://www.codingame.com/codegolf/easy/don%27t-panic

Work performed on 8 November 2026. The supplied Groovy baseline is preserved in `baseline.groovy`.

All solutions listed below passed all seven public test cases on CodinGame. Counts exclude the final newline; `api.ps1` removes final newlines before submitting. All submitted characters are ASCII, so byte and character counts agree.

| Language | Characters | Source | Submission |
|---|---:|---|---:|
| JavaScript | 119 | [shortest.js](shortest.js) | 41671960 |
| Bash | 142 | [golf.sh](golf.sh) | 41671870 |
| TypeScript | 147 | [golf.ts](golf.ts) | 41671954 |
| Perl | 162 | [golf.pl](golf.pl) | 41671878 |
| C | 167 | [golf.c](golf.c) | 41671910 |
| PHP | 179 | [golf.php](golf.php) | 41671925 |
| Groovy (100% confirmed on website) | 293 | [static-stream.groovy](static-stream.groovy) | 41672112 |
| Ruby | 189 | [golf.rb](golf.rb) | 41671892 |
| Python 3 | 190 | [golf.py](golf.py) | 41671928 |
| C++ | 199 | [golf.cpp](golf.cpp) | 41671938 |
| Objective-C | 199 | [golf.m](golf.m) | 41671966 |

The final API progress check confirmed **119 characters, 100%, rank 578 out of 16,202**, in JavaScript. This is 86 characters (about 42%) below the stated 205-character baseline. The earlier 127-character JavaScript solution in `golf.js` was submitted as 41671902. The API previously confirmed that version and Bash's 142-character submission at 100% too.

The API's progress endpoint reports the last submission ID together with the account's best score, size and language. Those fields can refer to different submissions. It does not provide a separate validator score for every language; do not interpret a later submission ID beside a 100% best score as confirmation that the later submission passed every hidden validator. Raw test, submission and progress responses are in `results/`.

Follow-up: the user reports **85% for Groovy submission 41671946** on the website. That overrides any inference from the account-best API result. Subsequent testing reproduced first-turn timeouts in the buffered-reader version too. The map also needs to preserve the exit when an elevator is listed on the exit floor; `putIfAbsent` fixes that edge case, but it is not established as the cause of the reported validator failure. `safe.groovy` is a 197-character experiment; `eachline.groovy` is a 171-character replacement using `System.in.eachLine` and preserving existing targets.

The replacement passed all seven public tests and was submitted as **41671996**. Its per-language hidden-validator result still needs checking on the website; the account-best API result cannot confirm it.

Further website feedback: **41671996 scored 71%, failing "U Turn" and "U Turn, lower floor"**, with no failure details available. Passing public cases is not sufficient evidence of hidden-validator correctness. Testing a separate conservative variant reproduced a first-turn timeout on public case 3; the original baseline passed the same case immediately afterward. This suggests timing sensitivity but does not establish the hidden validators' failure cause.

`direct.groovy` is a 195-character minimal edit of the supplied baseline. It preserves the Scanner input code, elevator map, missing-clone handling and exit-floor precedence. Its only changes are `d[0]=='L'` to `d<'R'` and `d[0]=='R'` to `d>'Q'`. For LEFT and RIGHT these are equivalent; NONE always has `t=c`, so neither branch can block. Exhaustive comparison covered all 20,100 valid position/direction decisions in the bounded domain. This avoids the broader streaming-parser rewrites while still meeting the under-200 target.

The user reports **71% for the 195-character version, submission 41672051**, failing "One floor" and "U Turn, lower floor". Its public passes and decision-equivalence checks did not establish validator success. Further shortening is suspended until a fresh control result is available. The exact pasted original, with final newline omitted, was resubmitted as **41672057**. The helper counts this source as 203 characters; the user originally described it as 205. Its new website result is needed to determine whether the original still passes the current runtime and validators.

The user's screenshot subsequently confirmed a fresh submission of the original 205-character code scored **71%**, and showed the 203-character control at **57%**. The historical 100% result is marked outdated. This establishes that the original no longer reliably passes; it does not prove which platform upgrade caused it.

Reliability investigation: CodinGame launches Groovy's compiled `Answer` class directly (a class named `Player` failed with "Could not find or load main class Answer"). `static.groovy` uses an explicit `Answer.main`, `@groovy.transform.CompileStatic`, typed local variables and primitive arrays, and explicit output flushing. It passed all seven public cases and was submitted as **41672095**, 627 characters. **The user confirmed its website validator score is 100%.** `static-short.groovy`, 368 characters, also passed all seven public cases. Static compilation bypasses Groovy's meta-object protocol: https://docs.groovy-lang.org/latest/html/gapi/groovy/transform/CompileStatic.html

`static-stream.groovy` reduces this approach to **293 characters**, passed all seven public cases, and was submitted as **41672112**. **The user confirmed that it also scores 100% on the website.** This is the shortest currently confirmed Groovy reference. Both the 627-character and 293-character versions use a statically compiled `Answer.main`; the latter uses a streaming parser and preserves existing targets. This supports a runtime-overhead explanation for the older dynamic scripts but does not identify a particular compiler/JVM upgrade as the cause.

`static-compact.groovy` is a further **273-character** candidate that passed all seven public cases. On 9 November 2026, it was submitted at the user's request as **41672205**. Its hidden-validator score is not yet confirmed; the 293-character version remains the shortest website-confirmed 100% reference.

The main structural change treats the exit as another entry in the floor-to-target map. Elevator lines update that map, while clone lines produce an action. Most versions use a single streaming input loop, distinguishing initialization, elevator definitions and clone turns by the number of fields. This removes the separate elevator-reading loop and its counter.

For a valid clone, block exactly when `(target-position)*directionSign>0`. LEFT has sign +1 and RIGHT has sign -1. A clone standing on its target therefore waits. A floor of -1 also waits.

JavaScript uses destructuring to read the fields while retaining the full array. Subtraction converts stored numeric strings to numbers, avoiding explicit conversions. `~f` is zero exactly for floor -1 within the valid input domain. The last initialization field is a nonempty string even when its value is `"0"`, so `a[7]` identifies the eight-field initialization line.

Bash obtains the sign from the direction length: `9-2*${#d}` is +1 for LEFT and -1 for RIGHT. Its floor guard excludes NONE. Groovy's original buffered-reader version passed one public suite but was reported at 85% on the website. Both Scanner and buffered-reader variants have encountered first-turn timeouts.

The 167-character C entry relies on this platform accepting implicit declarations and linking `gets`. The C++ and Objective-C versions use `fgets` and an explicit standard header instead. The 99-byte line buffer covers this puzzle's input bounds.

To repeat a suite from PowerShell:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File ./dont-panic/suite.ps1 -Language Javascript -File shortest.js
```

To submit a tested file:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File ./dont-panic/api.ps1 -Action submit -Language Javascript -File shortest.js
```

The helper reads the existing token from the user's private CodinGame configuration and never prints or copies it. Respect the limits returned by the API before starting further tests or submissions. No IDE-code overwrite was requested.
