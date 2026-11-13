#define UNIT_TEST
#include "../solution.cpp"
#include <cassert>
int exact(State s){
 int outcome=s.result();if(outcome>=0)return outcome;
 int moves[81],n=s.moves(moves);bool draw=false;
 for(int i=0;i<n;i++){State next=s;next.play(moves[i]);int r=exact(next);if(r==s.player)return r;draw|=r==2;}
 return draw?2:(s.player^1);
}
int main(){
 initFast();int moves[81];State s;assert(s.moves(moves)==81);
 for(int r=0;r<9;r++)for(int c=0;c<9;c++)assert(decode(encode(r,c))==make_pair(r,c));
 s.play(encode(4,4));assert(s.target==4);assert(s.moves(moves)==8);
 State t;t.board[0][0]=3;t.play(2);assert(t.won[0]==1&&t.closed==1);
 t.target=-1;int n=t.moves(moves);for(int i=0;i<n;i++)assert(moves[i]/9!=0);
 State u;u.closed=1<<4;u.play(4);assert(u.target==-1);
 State v;v.won[0]=7;assert(v.result()==0);
 State w;w.closed=511;w.won[0]=3;w.won[1]=16;assert(w.result()==0);
 w.won[1]=24;assert(w.result()==2);
 for(int game=0;game<1000;game++){
  State a;int turns=0;
  while(a.result()<0){int count=a.moves(moves);assert(count>0);a.play(moves[random32()%count]);assert(++turns<=81);}
 }
 // A forced immediate macro win must be chosen, and survive tree reuse.
 State tactical;tactical.won[0]=3;tactical.closed=3;
 tactical.board[0][2]=3;tactical.target=2;
 Search search;int chosen=search.choose(tactical,30);
 assert(chosen==2*9+2);assert(search.advance(chosen));
 tactical.play(chosen);assert(tactical.result()==0);
 // Compare proof propagation against exhaustive endgame minimax.
 for(int sample=0;sample<40;sample++){
  State late;
  while(late.result()<0){
   int remaining=0;
   for(int b=0;b<9;b++)if(!(late.closed>>b&1))remaining+=9-__builtin_popcount(late.board[0][b]|late.board[1][b]);
   if(remaining<=6)break;
   int n=late.moves(moves);late.play(moves[random32()%n]);
  }
  if(late.result()>=0)continue;
  Search proof;int m=proof.choose(late,100);
  assert(proof.tree[0].solved==exact(late));
  State next=late;next.play(m);assert(exact(next)==exact(late));
  assert(proof.advance(m));assert(proof.tree[0].solved==exact(next));
 }
 cerr<<"Rules checks and 1000 random games passed\n";
}
