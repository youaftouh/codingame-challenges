// CodinGame A*Craft: standalone TypeScript, no dependencies.
declare function readline(): string;
const W = 19, H = 10, N = W * H;
const directions = 'URDL';
const board = new Int8Array(N).fill(-1);
const editable: number[] = [];
for (let y = 0; y < H; y++) {
    const row = readline();
    for (let x = 0; x < W; x++) {
        const p = y * W + x;
        if (row[x] === '#') board[p] = -2;
        else if (row[x] === '.') editable.push(p);
        else board[p] = directions.indexOf(row[x]);
    }
}
const robots: number[] = [];
const count = Number(readline());
for (let i = 0; i < count; i++) {
    const [x, y, d] = readline().split(' ');
    robots.push((Number(y) * W + Number(x)) * 4 + directions.indexOf(d));
}
const next = new Int16Array(N * 4);
const started = Date.now();
for (let p = 0; p < N; p++) {
    const x = p % W, y = Math.floor(p / W);
    next[p * 4] = ((y + H - 1) % H) * W + x;
    next[p * 4 + 1] = y * W + (x + 1) % W;
    next[p * 4 + 2] = ((y + 1) % H) * W + x;
    next[p * 4 + 3] = y * W + (x + W - 1) % W;
}
const fixed = board.slice();
const stamps = new Int32Array(N * 4);
const active = new Uint8Array(N);
let stamp = 0;
function evaluate(track: boolean = false): number {
    if (track) active.fill(0);
    let score = 0;
    for (const robot of robots) {
        ++stamp;
        let p = robot >> 2, d = robot & 3;
        while (board[p] !== -2) {
            if (board[p] >= 0) d = board[p];
            const state = p * 4 + d;
            if (stamps[state] === stamp) break;
            stamps[state] = stamp;
            if (track) active[p] = 1;
            ++score;
            p = next[state];
        }
    }
    return score;
}
let seed = 0x17a2b39;
function random(): number {
    seed ^= seed << 13; seed ^= seed >>> 17; seed ^= seed << 5;
    return (seed >>> 0) / 4294967296;
}
let score = evaluate(true), bestScore = score;
let best = board.slice();
const baseline = score;
let iterations = 0;
const changed: number[] = [], previous: number[] = [];
while (editable.length && Date.now() - started < 820) {
    const elapsed = (Date.now() - started) / 820;
    // Repeated cooling avoids spending the entire run in one local optimum.
    const phase = (elapsed * 6) % 1;
    const temperature = Math.max(0.15, (3 + bestScore * 0.025) * Math.pow(1 - phase, 2));
    changed.length = 0; previous.length = 0;
    const mutations = random() < 0.12 ? 2 + Math.floor(random() * 3) : 1;
    for (let j = 0; j < mutations; j++) {
        let p = editable[Math.floor(random() * editable.length)];
        for (let attempt = 0; attempt < 8 && !active[p]; attempt++)
            p = editable[Math.floor(random() * editable.length)];
        changed.push(p); previous.push(board[p]);
        board[p] = Math.floor(random() * 5) - 1;
    }
    const candidate = evaluate();
    if (candidate >= score || random() < Math.exp((candidate - score) / temperature)) {
        score = candidate;
        if (score > bestScore) { bestScore = score; best = board.slice(); }
        if ((iterations & 31) === 0) evaluate(true);
    } else {
        for (let j = changed.length - 1; j >= 0; j--) board[changed[j]] = previous[j];
    }
    if ((++iterations & 4095) === 0 && phase > 0.8) {
        board.set(best); score = bestScore; evaluate(true);
    }
}
const output: string[] = [];
for (const p of editable) if (best[p] >= 0 && best[p] !== fixed[p])
    output.push(`${p % W} ${Math.floor(p / W)} ${directions[best[p]]}`);
console.error(`baseline=${baseline} score=${bestScore} iterations=${iterations} ms=${Date.now() - started}`);
// Commands on nonempty cells are ignored by the referee.
if (!output.length) {
    const p = fixed.findIndex(value => value !== -1);
    if (p >= 0) output.push(`${p % W} ${Math.floor(p / W)} U`);
}
console.log(output.join(' '));
