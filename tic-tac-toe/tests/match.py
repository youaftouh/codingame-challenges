"""Play two persistent CodinGame processes under full Ultimate rules."""
import argparse, json, random, subprocess
p=argparse.ArgumentParser();p.add_argument('candidate');p.add_argument('baseline');p.add_argument('--games',type=int,default=40);p.add_argument('--output',default='/tmp/uttt-matches.json');a=p.parse_args()
lines=[7,56,448,73,146,292,273,84]
def win(mask):return any(mask&line==line for line in lines)
def rc(m):b,c=divmod(m,9);return (b//3*3+c//3,b%3*3+c%3)
results=[]
for game in range(a.games):
 candidate=game%2;procs=[None,None]
 for player in range(2):procs[player]=subprocess.Popen([a.candidate if player==candidate else a.baseline],stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.DEVNULL,text=True,bufsize=1)
 board=[[0]*9 for _ in range(2)];won=[0,0];closed=0;target=-1;last=(-1,-1);turn=0
 try:
  while True:
   player=turn%2
   legal=[b*9+c for b in range(9) if not(closed>>b&1) and (target<0 or target==b) for c in range(9) if not((board[0][b]|board[1][b])>>c&1)]
   random.Random(game*101+turn).shuffle(legal)
   msg=f'{last[0]} {last[1]}\n{len(legal)}\n'+''.join(f'{r} {c}\n' for r,c in map(rc,legal))
   bot=procs[player];bot.stdin.write(msg);bot.stdin.flush();answer=bot.stdout.readline().split();r,c=map(int,answer[:2]);m=(r//3*3+c//3)*9+r%3*3+c%3
   assert m in legal,(game,turn,r,c)
   b,c0=divmod(m,9);board[player][b]|=1<<c0
   if win(board[player][b]):won[player]|=1<<b;closed|=1<<b
   elif board[0][b]|board[1][b]==511:closed|=1<<b
   target=-1 if closed>>c0&1 else c0;last=(r,c);turn+=1
   if win(won[player]):winner=player;break
   if closed==511:
    count=[x.bit_count() for x in won];winner=2 if count[0]==count[1] else int(count[1]>count[0]);break
  result=.5 if winner==2 else float(winner==candidate);results.append(result)
  print(f'{game+1}/{a.games} candidate={candidate} winner={winner} score={sum(results):g}/{len(results)}',flush=True)
 finally:
  for bot in procs:bot.stdin.close();bot.terminate();bot.wait()
 open(a.output,'w').write(json.dumps({'candidate':a.candidate,'baseline':a.baseline,'games':len(results),'score':sum(results),'results':results},indent=2)+'\n')
