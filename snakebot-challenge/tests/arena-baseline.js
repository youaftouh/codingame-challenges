/**
 * CodinGame Challenge: Snakebot
 * SURVIVAL FIRST Strategy - Protect the head at all costs
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

// Check if there's support (floor) below a position
function hasFloorBelow(x, y, allBodies) {
    // At bottom of map = has floor
    if (y >= height - 1) return true;
    
    const belowY = y + 1;
    
    // Platform below?
    if (isPlatform(x, belowY)) return true;
    
    // Snake body below?
    for (const body of allBodies) {
        for (let i = 0; i < body.length; i++) {
            if (body[i].x === x && body[i].y === belowY) {
                return true;
            }
        }
    }
    
    return false;
}

// Check if snake will have any support after moving
function willHaveSupport(myBot, direction, allBodies) {
    // Simulate the move
    const newBody = [];
    for (let i = 0; i < myBot.body.length; i++) {
        if (i === 0) {
            // Head moves in direction
            let newX = myBot.body[0].x;
            let newY = myBot.body[0].y;
            switch (direction) {
                case 'UP': newY--; break;
                case 'DOWN': newY++; break;
                case 'LEFT': newX--; break;
                case 'RIGHT': newX++; break;
            }
            newBody.push({x: newX, y: newY});
        } else {
            // Other parts follow
            newBody.push({x: myBot.body[i-1].x, y: myBot.body[i-1].y});
        }
    }
    
    // Check if ANY part of the new body has support
    for (const part of newBody) {
        if (hasFloorBelow(part.x, part.y, allBodies)) {
            return true; // At least one part has support
        }
    }
    
    return false; // No support = will fall
}

// Check if position is near edge (danger zone)
function isNearEdge(x, y, direction) {
    const margin = 2; // Consider edges within 2 cells dangerous
    
    if (direction === 'LEFT' && x <= margin) return true;
    if (direction === 'RIGHT' && x >= width - margin - 1) return true;
    if (direction === 'UP' && y <= margin) return true;
    if (direction === 'DOWN' && y >= height - margin - 1) return true;
    
    return false;
}

// Look ahead multiple moves to predict traps
function wouldBeCornered(head, direction, allBodies, powerSources, myBot) {
    // Simulate 1 move ahead
    let simX = head.x;
    let simY = head.y;
    
    switch (direction) {
        case 'UP': simY--; break;
        case 'DOWN': simY++; break;
        case 'LEFT': simX--; break;
        case 'RIGHT': simX++; break;
    }
    
    // Check if after this move, we'd have NO safe moves
    const nextMoves = ['UP', 'DOWN', 'LEFT', 'RIGHT'];
    let safeNextMoves = 0;
    
    for (const nextDir of nextMoves) {
        // Can't reverse
        if (direction === 'UP' && nextDir === 'DOWN') continue;
        if (direction === 'DOWN' && nextDir === 'UP') continue;
        if (direction === 'LEFT' && nextDir === 'RIGHT') continue;
        if (direction === 'RIGHT' && nextDir === 'LEFT') continue;
        
        let nextX = simX;
        let nextY = simY;
        
        switch (nextDir) {
            case 'UP': nextY--; break;
            case 'DOWN': nextY++; break;
            case 'LEFT': nextX--; break;
            case 'RIGHT': nextX++; break;
        }
        
        // Check if this next move would be safe
        if (nextX >= 0 && nextX < width && nextY >= 0 && nextY < height) {
            if (!isPlatform(nextX, nextY) || isPowerSource(nextX, nextY, powerSources)) {
                safeNextMoves++;
            }
        }
    }
    
    // If we'd have 0-1 safe moves after this move, we're getting cornered
    return safeNextMoves <= 1;
}

// Check if moving would cause falling off cliff (explicit edge check)
function wouldFallOffCliff(head, direction) {
    let newX = head.x;
    let newY = head.y;
    
    switch (direction) {
        case 'UP': newY--; break;
        case 'DOWN': newY++; break;
        case 'LEFT': newX--; break;
        case 'RIGHT': newX++; break;
    }
    
    // Already out of bounds
    if (newX < 0 || newX >= width || newY < 0 || newY >= height) {
        return true;
    }
    
    // Moving to bottom row is always falling off
    if (newY >= height - 1) {
        return false; // Bottom is floor, won't fall
    }
    
    // Check if next position would send us falling out
    // Horizontal movement to an edge without floor = fall off
    if (direction === 'LEFT' && newX === 0) {
        // Left edge - check if we have floor below
        if (!isPlatform(newX, newY + 1)) {
            // No floor at left edge = will fall off left side
            return true;
        }
    }
    
    if (direction === 'RIGHT' && newX === width - 1) {
        // Right edge - check if we have floor below
        if (!isPlatform(newX, newY + 1)) {
            // No floor at right edge = will fall off right side
            return true;
        }
    }
    
    // Check for cliff ahead - moving into empty space that would cause fall
    // Look down from new position to see if there's a drop to bottom
    let checkY = newY + 1;
    let hasFloor = false;
    
    while (checkY < height) {
        if (isPlatform(newX, checkY)) {
            hasFloor = true;
            break;
        }
        checkY++;
    }
    
    // If no floor all the way to bottom = cliff
    if (!hasFloor && newY < height - 1) {
        return true;
    }
    
    return false;
}

// Check if moving would be SAFE (survival first!)
function isSafeMove(head, direction, allBodies, powerSources, myBot, skipGravityCheck = false) {
    let newX = head.x;
    let newY = head.y;
    
    switch (direction) {
        case 'UP': newY--; break;
        case 'DOWN': newY++; break;
        case 'LEFT': newX--; break;
        case 'RIGHT': newX++; break;
    }
    
    // OUT OF BOUNDS = DEATH
    if (newX < 0 || newX >= width || newY < 0 || newY >= height) {
        return false;
    }
    
    // EXPLICIT CLIFF CHECK: Would we fall off the map edge?
    if (wouldFallOffCliff(head, direction)) {
        return false;
    }
    
    // PLATFORM = DEATH (unless it's a power source)
    if (isPlatform(newX, newY)) {
        return isPowerSource(newX, newY, powerSources);
    }
    
    // Check ALL snake bodies
    for (const body of allBodies) {
        const isSelf = (body === myBot.body);
        
        for (let i = 0; i < body.length; i++) {
            if (body[i].x === newX && body[i].y === newY) {
                // Power source = OK
                if (isPowerSource(newX, newY, powerSources)) {
                    return true;
                }
                
                // Our own tail will move
                if (isSelf && i === body.length - 1) {
                    continue;
                }
                
                // ANY other body part = DEATH
                return false;
            }
        }
    }
    
    // CRITICAL GRAVITY CHECK: Will the snake have ANY support after this move?
    // Skip this check if requested (for defensive UP movement when winning)
    if (!skipGravityCheck) {
        if (!willHaveSupport(myBot, direction, allBodies)) {
            // Snake would lose all support and fall = DEATH
            return false;
        }
        
        // EDGE WARNING: Near edge without escape = DANGER
        if (isNearEdge(newX, newY, direction)) {
            // Check if we'd be cornered
            if (wouldBeCornered(head, direction, allBodies, powerSources, myBot)) {
                return false; // Would trap us
            }
        }
    }
    
    return true;
}

// Count available safe moves from a position (space control metric)
function countSafeMoves(head, direction, allBodies, powerSources, myBot) {
    // Simulate moving to new position
    let newX = head.x;
    let newY = head.y;
    switch (direction) {
        case 'UP': newY--; break;
        case 'DOWN': newY++; break;
        case 'LEFT': newX--; break;
        case 'RIGHT': newX++; break;
    }
    
    // Out of bounds
    if (newX < 0 || newX >= width || newY < 0 || newY >= height) {
        return 0;
    }
    
    // Count how many directions are safe from this new position
    const testDirections = ['UP', 'DOWN', 'LEFT', 'RIGHT'];
    let safeCount = 0;
    
    // Determine current direction to avoid 180-turn
    const oppositeDir = {
        'UP': 'DOWN',
        'DOWN': 'UP',
        'LEFT': 'RIGHT',
        'RIGHT': 'LEFT'
    };
    
    for (const testDir of testDirections) {
        // Can't reverse from the direction we're testing
        if (testDir === oppositeDir[direction]) continue;
        
        let testX = newX;
        let testY = newY;
        switch (testDir) {
            case 'UP': testY--; break;
            case 'DOWN': testY++; break;
            case 'LEFT': testX--; break;
            case 'RIGHT': testX++; break;
        }
        
        // Check if this direction would be safe
        if (testX >= 0 && testX < width && testY >= 0 && testY < height) {
            if (!isPlatform(testX, testY)) {
                safeCount++;
            }
        }
    }
    
    return safeCount;
}

// Find closest power source
function findClosestPowerSource(head, powerSources) {
    if (powerSources.length === 0) return null;
    let closest = powerSources[0];
    let minDist = Math.abs(head.x - closest.x) + Math.abs(head.y - closest.y);
    for (let i = 1; i < powerSources.length; i++) {
        const dist = Math.abs(head.x - powerSources[i].x) + Math.abs(head.y - powerSources[i].y);
        if (dist < minDist) {
            minDist = dist;
            closest = powerSources[i];
        }
    }
    return closest;
}

// Calculate total body parts for all snakes
function calculateScore(mySnakebots, allSnakebots) {
    let myTotal = 0;
    let oppTotal = 0;
    
    for (const bot of allSnakebots) {
        const bodyCount = bot.body.length;
        if (mySnakebotIds.includes(bot.id)) {
            myTotal += bodyCount;
        } else {
            oppTotal += bodyCount;
        }
    }
    
    return { myTotal, oppTotal };
}

// Score a direction based on multiple factors
function scoreDirection(head, direction, target, allBodies, powerSources, myBot, isWinning) {
    // Check if move is safe first
    if (!isSafeMove(head, direction, allBodies, powerSources, myBot)) {
        return -1000; // Unsafe = very negative score
    }
    
    let score = 0;
    
    // Calculate new position
    let newX = head.x;
    let newY = head.y;
    switch (direction) {
        case 'UP': newY--; break;
        case 'DOWN': newY++; break;
        case 'LEFT': newX--; break;
        case 'RIGHT': newX++; break;
    }
    
    if (isWinning) {
        // WHEN WINNING OR TIE: Safety 100%
        // Base safety score
        score += 100;
        
        // Bonus for space control (more open space = safer)
        const spaceScore = countSafeMoves(head, direction, allBodies, powerSources, myBot);
        score += spaceScore * 20;
        
        // Penalty for going down (risky)
        if (direction === 'DOWN') {
            score -= 30;
        }
        
        // Bonus for going up (defensive dancing)
        if (direction === 'UP') {
            score += 40;
        }
        
        // Bonus for staying horizontal (safest)
        if (direction === 'LEFT' || direction === 'RIGHT') {
            score += 50;
        }
        
    } else {
        // WHEN LOSING: Safety (40%), Food proximity (50%), Space (10%)
        
        // SAFETY (40%)
        const spaceScore = countSafeMoves(head, direction, allBodies, powerSources, myBot);
        score += spaceScore * 10; // Space control for safety
        score += 20; // Base safety for being valid
        
        // FOOD PROXIMITY (50%)
        if (target) {
            const currentDist = Math.abs(head.x - target.x) + Math.abs(head.y - target.y);
            const newDist = Math.abs(newX - target.x) + Math.abs(newY - target.y);
            
            // Reward getting closer to food
            if (newDist < currentDist) {
                score += 50; // Moving closer
            } else if (newDist === currentDist) {
                score += 10; // Maintaining distance
            } else {
                score -= 20; // Moving away (penalty)
            }
            
            // Big bonus if we'll eat the food
            if (newX === target.x && newY === target.y) {
                score += 100;
            }
        }
        
        // SPACE (10%)
        score += spaceScore * 2;
    }
    
    return score;
}

// Choose best direction using scoring system
function chooseSafeDirection(head, target, allBodies, powerSources, myBot, allSnakebots, isWinning) {
    const currentDir = getCurrentDirection(myBot.body);
    
    // Score all possible directions
    const allDirections = ['UP', 'DOWN', 'LEFT', 'RIGHT'];
    const directionScores = [];
    
    for (const dir of allDirections) {
        // Can't reverse direction (180-degree turn)
        if (currentDir === 'UP' && dir === 'DOWN') continue;
        if (currentDir === 'DOWN' && dir === 'UP') continue;
        if (currentDir === 'LEFT' && dir === 'RIGHT') continue;
        if (currentDir === 'RIGHT' && dir === 'LEFT') continue;
        
        const score = scoreDirection(head, dir, target, allBodies, powerSources, myBot, isWinning);
        directionScores.push({dir, score});
    }
    
    // Sort by score (highest first)
    directionScores.sort((a, b) => b.score - a.score);
    
    // Return best direction, or fallback if none are safe
    if (directionScores.length > 0 && directionScores[0].score > -1000) {
        return directionScores[0].dir;
    }
    
    // No safe moves - try anything
    return 'RIGHT';
}

// Game loop
while (true) {
    const powerSourceCount = parseInt(readline());
    const powerSources = [];
    for (let i = 0; i < powerSourceCount; i++) {
        const inputs = readline().split(' ');
        powerSources.push({
            x: parseInt(inputs[0]),
            y: parseInt(inputs[1])
        });
    }
    
    const snakebotCount = parseInt(readline());
    const snakebots = [];
    const mySnakebots = [];
    const allBodies = [];
    
    for (let i = 0; i < snakebotCount; i++) {
        const inputs = readline().split(' ');
        const snakebotId = parseInt(inputs[0]);
        const body = parseBody(inputs[1]);
        
        const snakebot = {
            id: snakebotId,
            body: body,
            head: body[0]
        };
        
        snakebots.push(snakebot);
        allBodies.push(body);
        
        if (mySnakebotIds.includes(snakebotId)) {
            mySnakebots.push(snakebot);
        }
    }
    
    // Calculate if we're winning
    const score = calculateScore(mySnakebots, snakebots);
    const isWinning = score.myTotal > score.oppTotal;
    
    // Generate commands - SURVIVAL FIRST with strategy
    const commands = [];
    const plannedMoves = new Map(); // Track where each snake head will move
    
    // Helper to get next head position
    function getNextHeadPosition(head, direction) {
        let newX = head.x;
        let newY = head.y;
        switch (direction) {
            case 'UP': newY--; break;
            case 'DOWN': newY++; break;
            case 'LEFT': newX--; break;
            case 'RIGHT': newX++; break;
        }
        return `${newX},${newY}`;
    }
    
    // Check if a move conflicts with already planned moves
    function wouldCollideWithOwnSnakes(head, direction) {
        const nextPos = getNextHeadPosition(head, direction);
        return plannedMoves.has(nextPos);
    }
    
    // Smart coordination: Process snakes and track alternatives for conflict resolution
    const snakeMoves = []; // Store {bot, direction, alternatives} for each snake
    
    for (const bot of mySnakebots) {
        const currentDir = getCurrentDirection(bot.body);
        let chosenDir = null;
        const alternatives = [];
        
        // Check if power source is nearby (1-2 moves away)
        const target = findClosestPowerSource(bot.head, powerSources);
        const distToTarget = target ? (Math.abs(bot.head.x - target.x) + Math.abs(bot.head.y - target.y)) : Infinity;
        const targetIsNear = distToTarget <= 2;
        
        // PRIORITY: If power source is 1-2 moves away, GO FOR IT (even when winning)
        if (targetIsNear && target) {
            chosenDir = chooseSafeDirection(bot.head, target, allBodies, powerSources, bot, snakebots, isWinning);
        }
        // Determine best direction
        else if (score.myTotal >= score.oppTotal && powerSources.length > 0) {
            // Winning strategy - try UP first (only if no nearby food)
            if (currentDir !== 'DOWN' && isSafeMove(bot.head, 'UP', allBodies, powerSources, bot, true)) {
                chosenDir = 'UP';
            }
        }
        
        if (!chosenDir) {
            // Normal strategic movement
            chosenDir = chooseSafeDirection(bot.head, target, allBodies, powerSources, bot, snakebots, isWinning);
        }
        
        // Find all alternative safe directions and score them by space control
        const allDirs = ['UP', 'DOWN', 'LEFT', 'RIGHT'];
        for (const dir of allDirs) {
            if (dir === chosenDir) continue;
            
            // Check 180-degree turn
            if (currentDir === 'UP' && dir === 'DOWN') continue;
            if (currentDir === 'DOWN' && dir === 'UP') continue;
            if (currentDir === 'LEFT' && dir === 'RIGHT') continue;
            if (currentDir === 'RIGHT' && dir === 'LEFT') continue;
            
            if (isSafeMove(bot.head, dir, allBodies, powerSources, bot)) {
                const spaceScore = countSafeMoves(bot.head, dir, allBodies, powerSources, bot);
                alternatives.push({dir, spaceScore});
            }
        }
        
        // Sort alternatives by space control (more open space = better)
        alternatives.sort((a, b) => b.spaceScore - a.spaceScore);
        
        snakeMoves.push({bot, direction: chosenDir, alternatives});
    }
    
    // Resolve conflicts with smart coordination
    for (let i = 0; i < snakeMoves.length; i++) {
        const move = snakeMoves[i];
        const nextPos = getNextHeadPosition(move.bot.head, move.direction);
        
        // Check if another snake already claimed this position
        if (plannedMoves.has(nextPos)) {
            // Conflict! Find which snake has more alternatives
            const conflictSnakeId = plannedMoves.get(nextPos);
            const conflictMoveIndex = snakeMoves.findIndex(m => m.bot.id === conflictSnakeId);
            
            if (conflictMoveIndex !== -1) {
                const conflictMove = snakeMoves[conflictMoveIndex];
                
                // Snake with MORE alternatives should yield
                if (move.alternatives.length > conflictMove.alternatives.length) {
                    // This snake has more alternatives, so it yields
                    if (move.alternatives.length > 0) {
                        move.direction = move.alternatives[0].dir; // Pick best alternative
                    }
                } else if (move.alternatives.length < conflictMove.alternatives.length) {
                    // Other snake has more alternatives, make it yield
                    if (conflictMove.alternatives.length > 0) {
                        // Remove old position from plannedMoves
                        const oldPos = getNextHeadPosition(conflictMove.bot.head, conflictMove.direction);
                        plannedMoves.delete(oldPos);
                        
                        // Assign new direction to conflict snake
                        conflictMove.direction = conflictMove.alternatives[0].dir;
                        plannedMoves.set(getNextHeadPosition(conflictMove.bot.head, conflictMove.direction), conflictMove.bot.id);
                    }
                } else {
                    // Equal alternatives, first snake (lower index) gets priority
                    // This snake yields
                    if (move.alternatives.length > 0) {
                        move.direction = move.alternatives[0].dir;
                    }
                }
            }
        }
        
        // Record this snake's final position
        commands.push(`${move.bot.id} ${move.direction}`);
        plannedMoves.set(getNextHeadPosition(move.bot.head, move.direction), move.bot.id);
    }
    
    console.log(commands.length > 0 ? commands.join(';') : 'WAIT');
}