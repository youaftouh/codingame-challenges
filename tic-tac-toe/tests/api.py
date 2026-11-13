"""Small recorder for this game's API; credentials stay outside the repository."""
import argparse, hashlib, json, pathlib, subprocess
ROOT = pathlib.Path(__file__).resolve().parents[1]
p = argparse.ArgumentParser()
p.add_argument('action', choices=['test', 'submit', 'get'])
p.add_argument('name')
p.add_argument('--source', default=str(ROOT / 'solution.cpp'))
p.add_argument('--opponent', default='boss')
p.add_argument('--endpoint', default='progress')
p.add_argument('--seed')
p.add_argument('--replay', type=int)
a = p.parse_args()
token = pathlib.Path.home().joinpath('.config/codingame/token').read_text().strip()
base = 'https://www.codingame.com/api/v1/'
url = base + 'puzzles/tic-tac-toe/'
args = ['curl', '-sS', '--max-time', '40', '-H', 'Authorization: Bearer ' + token]
body = None
if a.action == 'get':
    url += a.endpoint
else:
    code = pathlib.Path(a.source).read_text()
    body = {'language': 'C++', 'code': code}
    if a.action == 'test':
        if a.replay: body['replay_game_id'] = a.replay
        else:
            body['opponents'] = [a.opponent]
            if a.seed: body['game_options'] = 'seed=' + a.seed
        url += 'test-games?wait=30'
    else: url += 'submissions'
    args += ['-X', 'POST', '-H', 'Content-Type: application/json', '--data-binary', '@-']
raw = subprocess.run(args + [url], input=json.dumps(body) if body else None,
                     text=True, capture_output=True, check=True).stdout
try: data = json.loads(raw)
except ValueError: data = {'log': raw}
if a.action == 'test' and data.get('status') not in ('done', 'failed') and data.get('test_game_id'):
    poll = base + 'test-games/' + str(data['test_game_id']) + '?wait=30'
    while data.get('status') not in ('done', 'failed'):
        raw = subprocess.run(args[:6] + [poll], text=True, capture_output=True, check=True).stdout
        try: data = json.loads(raw)
        except ValueError: data = {'status': 'done', 'log': raw}
path = ROOT / 'records' / (a.name + '.json')
path.write_text(json.dumps(data, indent=2) + '\n')
if body:
    (ROOT / 'records' / (a.name + '.meta.json')).write_text(json.dumps({
        'source': str(pathlib.Path(a.source).relative_to(ROOT)),
        'sha256': hashlib.sha256(code.encode()).hexdigest(),
        'opponent': a.opponent if a.action == 'test' else None,
        'seed': a.seed, 'replay_game_id': a.replay}, indent=2) + '\n')
if 'log' in data:
    log = data['log']; (ROOT / 'records' / (a.name + '.log')).write_text(log)
    print(log[-1200:])
    print('errors:', 'error:' in log or 'timeout!' in log)
    print('limits:', data.get('limits'))
else: print(json.dumps(data))
