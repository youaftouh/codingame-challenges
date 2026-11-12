const assert = require('node:assert/strict');
const fs = require('node:fs');
const vm = require('node:vm');
const source = fs.readFileSync(`${__dirname}/../solution.js`, 'utf8');
const functions = source.slice(source.indexOf('// Check if position is a platform'), source.indexOf('// Check if position is near edge'));
function landing(body, direction, platforms = [], food = []) {
    const context = vm.createContext({width: 15, height: 10, gridMap: Array.from({length: 10}, (_, y) => Array.from({length: 15}, (_, x) => platforms.some(p => p[0] === x && p[1] === y) ? '#' : '.').join(''))});
    vm.runInContext(functions, context);
    return context.predictLanding({body}, direction, [body], food);
}
const body = [{x: 5, y: 3}, {x: 5, y: 4}, {x: 5, y: 5}];
assert.equal(landing(body, 'RIGHT'), null, 'bottom boundary is not a floor');
assert.equal(landing(body, 'RIGHT', [[5, 8]])[2].y, 7, 'body can fall safely onto a lower platform');
assert.equal(landing(body, 'RIGHT', [], [{x: 5, y: 7}])[2].y, 6, 'uneaten food supports the body');
assert.equal(landing(body, 'RIGHT', [], [{x: 6, y: 3}]), null, 'eaten food no longer provides support');
assert.equal(landing(body, 'RIGHT', [[5, 6]])[2].y, 5, 'vacated tail cannot support its own snake');
assert.deepEqual(body, [{x: 5, y: 3}, {x: 5, y: 4}, {x: 5, y: 5}], 'simulation must preserve the observed body');
console.log('6 gravity checks passed');
