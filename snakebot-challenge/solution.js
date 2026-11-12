/**
 * CodinGame Challenge: Snakebot
 * Team search with simultaneous movement, collisions, and gravity.
 */

const myId = parseInt(readline());
const width = parseInt(readline());
const height = parseInt(readline());

// Store the grid map
const gridMap = [];
for (let i = 0; i < height; i++) {
    const row = readline();
    gridMap.push(row);
}

const snakebotsPerPlayer = parseInt(readline());
const mySnakebotIds = [];
for (let i = 0; i < snakebotsPerPlayer; i++) {
    const mySnakebotId = parseInt(readline());
    mySnakebotIds.push(mySnakebotId);
}

const oppSnakebotIds = [];
for (let i = 0; i < snakebotsPerPlayer; i++) {
    const oppSnakebotId = parseInt(readline());
    oppSnakebotIds.push(oppSnakebotId);
}

// Parse body coordinates
function parseBody(bodyString) {
    const bodyParts = bodyString.split(':'); // Split by : to get each body part
    const coordinates = [];
    for (const part of bodyParts) {
        const [x, y] = part.split(','); // Each part is "x,y"
        coordinates.push({
            x: parseInt(x),
            y: parseInt(y)
        });
    }
    return coordinates;
}

// Check if position is a platform
function isPlatform(x, y) {
    if (y < 0 || y >= height || x < 0 || x >= width) return false;
    return gridMap[y][x] === '#';
}

// Check if position contains a power source
function isPowerSource(x, y, powerSources) {
    for (const ps of powerSources) {
        if (ps.x === x && ps.y === y) return true;
    }
    return false;
}

// Get current direction from body
function getCurrentDirection(body) {
    if (body.length < 2) return null;
    const head = body[0];
    const neck = body[1];
    const dx = head.x - neck.x;
    const dy = head.y - neck.y;
    if (dx > 0) return 'RIGHT';
    if (dx < 0) return 'LEFT';
    if (dy > 0) return 'DOWN';
    if (dy < 0) return 'UP';
    return null;
}

// Predict settling after movement. Other snakes are held at their current
// positions; their simultaneous moves remain uncertain.
function predictLanding(myBot, direction, allBodies, powerSources) {
    const delta = {UP: [0, -1], DOWN: [0, 1], LEFT: [-1, 0], RIGHT: [1, 0]}[direction];
    const head = {x: myBot.body[0].x + delta[0], y: myBot.body[0].y + delta[1]};
    const ate = isPowerSource(head.x, head.y, powerSources);
    const body = [head, ...myBot.body.slice(0, ate ? myBot.body.length : -1).map(part => ({...part}))];
    const solids = new Set();
    for (const food of powerSources) {
        if (food.x !== head.x || food.y !== head.y) solids.add(`${food.x},${food.y}`);
    }
    for (const other of allBodies) {
        if (other === myBot.body) continue;
        // The tail normally vacates during movement; do not rely on it.
        for (const part of other.slice(0, -1)) solids.add(`${part.x},${part.y}`);
    }
    for (let fall = 0; fall <= height * 2; fall++) {
        if (body.every(part => part.y >= height)) return null;
        if (body.some(part => isPlatform(part.x, part.y + 1) || solids.has(`${part.x},${part.y + 1}`))) {
            return body;
        }
        for (const part of body) part.y++;
    }
    return null;
}

// Check if position is near edge: superseded by the joint turn simulator.
const steps = {UP: [0, -1], DOWN: [0, 1], LEFT: [-1, 0], RIGHT: [1, 0]};
const opposite = {UP: 'DOWN', DOWN: 'UP', LEFT: 'RIGHT', RIGHT: 'LEFT'};
const cell = p => `${p.x},${p.y}`;
const visits = new Map();
let foodDistance = [];
function updateNavigation(state) {
    foodDistance = Array(width * height).fill(Infinity);
    const queue = [];
    for (const p of state.food) {const k = p.y * width + p.x; foodDistance[k] = 0; queue.push(k);}
    for (let i = 0; i < queue.length; i++) {
        const k = queue[i], x = k % width, y = Math.floor(k / width);
        for (const [dx, dy] of Object.values(steps)) {
            const nx = x + dx, ny = y + dy, nk = ny * width + nx;
            if (nx < 0 || nx >= width || ny < 0 || ny >= height || isPlatform(nx, ny) || foodDistance[nk] !== Infinity) continue;
            foodDistance[nk] = foodDistance[k] + 1; queue.push(nk);
        }
    }
    for (const bot of state.bots) {
        if (!mySnakebotIds.includes(bot.id)) continue;
        const history = visits.get(bot.id) || [];
        history.push(cell(bot.body[0]));
        if (history.length > 24) history.shift();
        visits.set(bot.id, history);
    }
}
function routeDistance(head, food) {
    const index = head.y * width + head.x;
    const route = head.x >= 0 && head.x < width && head.y >= 0 && head.y < height ? foodDistance[index] : Infinity;
    return Number.isFinite(route) ? route : food.length ? Math.min(...food.map(p => Math.abs(head.x - p.x) + Math.abs(head.y - p.y))) + 10 : 0;
}
function choices(bot) {
    return Object.keys(steps).filter(d => d !== opposite[getCurrentDirection(bot.body)]);
}

// Resolve movement and head losses together, then freeze supported snakes
// transitively while all unsupported snakes fall together.
function simulate(state, moves) {
    const eaten = new Set();
    let bots = state.bots.map(bot => {
        const dir = moves[bot.id] || getCurrentDirection(bot.body) || 'UP';
        const [dx, dy] = steps[dir];
        const head = {x: bot.body[0].x + dx, y: bot.body[0].y + dy};
        const grows = state.food.some(p => p.x === head.x && p.y === head.y);
        if (grows) eaten.add(cell(head));
        return {...bot, body: [head, ...bot.body.slice(0, grows ? bot.body.length : -1).map(p => ({...p}))]};
    });
    const occupancy = new Map();
    for (const bot of bots) for (const part of bot.body) {
        const k = cell(part); occupancy.set(k, (occupancy.get(k) || 0) + 1);
    }
    for (const bot of bots) {
        if (isPlatform(bot.body[0].x, bot.body[0].y) || occupancy.get(cell(bot.body[0])) > 1) {
            bot.body = bot.body.length <= 3 ? [] : bot.body.slice(1);
        }
    }
    bots = bots.filter(bot => bot.body.length);
    const food = state.food.filter(p => !eaten.has(cell(p)));
    const foodCells = new Set(food.map(cell));
    const grounded = new Set();
    const support = new Set();
    for (let fall = 0; fall < height * 3 + 100; fall++) {
        let changed = true;
        while (changed) {
            changed = false;
            for (const bot of bots) {
                if (grounded.has(bot.id)) continue;
                if (bot.body.some(p => isPlatform(p.x, p.y + 1) || foodCells.has(`${p.x},${p.y + 1}`) || support.has(`${p.x},${p.y + 1}`))) {
                    grounded.add(bot.id);
                    for (const p of bot.body) support.add(cell(p));
                    changed = true;
                }
            }
        }
        const falling = bots.filter(bot => !grounded.has(bot.id));
        if (!falling.length) break;
        for (const bot of falling) for (const p of bot.body) p.y++;
        bots = bots.filter(bot => !bot.body.every(p => p.y >= height + 1));
    }
    return {bots, food};
}

function greedyMove(state, bot) {
    const occupied = new Set(state.bots.flatMap(b => b.body.slice(0, -1).map(cell)));
    let best = -Infinity, move = getCurrentDirection(bot.body) || 'UP';
    for (const dir of choices(bot)) {
        const [dx, dy] = steps[dir];
        const x = bot.body[0].x + dx, y = bot.body[0].y + dy;
        const eats = state.food.some(p => p.x === x && p.y === y);
        const landing = predictLanding(bot, dir, state.bots.map(b => b.body), state.food, false);
        let value = landing ? 0 : -10000;
        if (isPlatform(x, y) || occupied.has(`${x},${y}`)) value -= 1000;
        const head = landing ? landing[0] : {x, y};
        const distance = routeDistance(head, state.food);
        value -= (visits.get(bot.id) || []).filter(p => p === cell(head)).length * 5;
        value += (eats ? 100 : 0) - distance;
        if (value > best) {best = value; move = dir;}
    }
    return move;
}

function evaluate(state) {
    let value = 0, mine = 0, theirs = 0;
    for (const bot of state.bots) {
        const own = mySnakebotIds.includes(bot.id);
        if (own) mine += bot.body.length; else theirs += bot.body.length;
        value += own ? bot.body.length * 150 + 40 : -bot.body.length * 100 - 20;
        if (own && state.food.length) {
            const distance = routeDistance(bot.body[0], state.food);
            value -= (visits.get(bot.id) || []).filter(p => p === cell(bot.body[0])).length * 10;
            value -= distance * 2;
        }
    }
    if (!mine) return -100000;
    if (!theirs) return 100000 + mine;
    if (!state.food.length) value += Math.sign(mine - theirs) * 10000;
    return value;
}

function chooseTeam(state) {
    const deadline = Date.now() + 28;
    updateNavigation(state);
    const own = state.bots.filter(b => mySnakebotIds.includes(b.id));
    const initial = {};
    for (const bot of state.bots) initial[bot.id] = greedyMove(state, bot);
    let bestMoves = initial;
    let beam = [{state, first: null, value: -Infinity}];
    for (let depth = 0; depth < 5; depth++) {
        const next = [];
        for (const node of beam) {
            const base = {};
            for (const bot of node.state.bots) base[bot.id] = greedyMove(node.state, bot);
            const candidates = [base];
            // Change one teammate at a time; subsequent depths combine choices.
            for (const bot of node.state.bots.filter(b => mySnakebotIds.includes(b.id))) {
                for (const dir of choices(bot)) {
                    if (dir !== base[bot.id]) candidates.push({...base, [bot.id]: dir});
                }
            }
            for (const moves of candidates) {
                const future = simulate(node.state, moves);
                const first = node.first || moves;
                let value = evaluate(future);
                // Penalize root head-on threats even if the enemy model turns away.
                for (const bot of own) {
                    const [dx, dy] = steps[first[bot.id]];
                    const x = bot.body[0].x + dx, y = bot.body[0].y + dy;
                    if (state.bots.some(b => !mySnakebotIds.includes(b.id) && Math.abs(b.body[0].x - x) + Math.abs(b.body[0].y - y) === 1)) value -= 50;
                }
                next.push({state: future, first, value});
                if (Date.now() >= deadline) break;
            }
            if (Date.now() >= deadline) break;
        }
        next.sort((a, b) => b.value - a.value);
        if (next.length) {bestMoves = next[0].first;}
        beam = next.slice(0, 4);
        if (Date.now() >= deadline || !beam.length) break;
    }
    return own.map(bot => `${bot.id} ${bestMoves[bot.id] || getCurrentDirection(bot.body) || 'UP'}`).join(';') || 'WAIT';
}

while (true) {
    const food = [];
    const foodCount = Number(readline());
    for (let i = 0; i < foodCount; i++) {
        const [x, y] = readline().split(' ').map(Number); food.push({x, y});
    }
    const count = Number(readline()), bots = [];
    for (let i = 0; i < count; i++) {
        const [id, body] = readline().split(' '); bots.push({id: Number(id), body: parseBody(body)});
    }
    console.log(chooseTeam({bots, food}));
}
