const assert = require('node:assert/strict');
const fs = require('node:fs');
const vm = require('node:vm');
const source = fs.readFileSync(`${__dirname}/../solution.js`, 'utf8');
function run(state, moves, platforms = []) {
    const ctx = vm.createContext({width: 15, height: 10,
        gridMap: Array.from({length: 10}, (_, y) => Array.from({length: 15}, (_, x) => platforms.some(p => p[0] === x && p[1] === y) ? '#' : '.').join(''))});
    vm.runInContext(source.slice(source.indexOf('// Check if position is a platform'), source.indexOf('while (true)')), ctx);
    return ctx.simulate(state, moves);
}
const snake = (id, points) => ({id, body: points.map(([x,y]) => ({x,y}))});
const floor = Array.from({length: 15}, (_, x) => [x, 9]);
const state = {bots: [snake(0, [[5,6],[5,7],[5,8]])], food: [{x: 6,y: 6}]};
const before = JSON.stringify(state);
let result = run(state, {0:'RIGHT'}, floor);
assert.equal(result.bots[0].body.length, 4, 'food grows the snake');
assert.equal(result.food.length, 0, 'food is removed');
assert.equal(JSON.stringify(state), before, 'simulation preserves its input');
result = run({bots: [snake(0,[[4,8],[3,8],[2,8]]),snake(1,[[6,8],[7,8],[8,8]])],food:[]},{0:'RIGHT',1:'LEFT'},floor);
assert.equal(result.bots.length, 0, 'simultaneous head collision removes short snakes');
result = run({bots:[snake(0,[[4,8],[3,8],[2,8],[1,8]])],food:[]},{0:'DOWN'},floor);
assert.equal(result.bots[0].body.length, 3, 'wall collision removes one head from a longer snake');
result = run(state,{0:'RIGHT'});
assert.equal(result.bots.length, 0, 'unsupported snakes fall out');
result = run({bots:[snake(0,[[4,7],[3,7],[2,7]]),snake(1,[[4,8],[3,8],[2,8]])],food:[]},{0:'RIGHT',1:'RIGHT'},floor);
assert.equal(result.bots[0].body[0].y, 7, 'a grounded snake supports another snake');
console.log('7 joint simulation checks passed');
