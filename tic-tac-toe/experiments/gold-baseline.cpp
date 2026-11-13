#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>
using namespace std;
static bool wins[512];
static uint32_t rng=712367821;
uint32_t random32(){rng^=rng<<13;rng^=rng>>17;return rng^=rng<<5;}
void init(){for(int m=0;m<512;m++)for(int line:{7,56,448,73,146,292,273,84})if((m&line)==line)wins[m]=true;}
struct State {
    uint16_t board[2][9]{}; uint16_t won[2]{},closed=0;
    int target=-1,player=0;
    int moves(int* out) const {
        int n=0;
        for(int b=0;b<9;b++) if(!(closed>>b&1)&&(target<0||target==b)) {
            unsigned mask=511^(board[0][b]|board[1][b]);
            while(mask){int c=__builtin_ctz(mask);mask&=mask-1;out[n++]=b*9+c;}
        }
        return n;
    }
    void play(int move){
        int b=move/9,c=move%9;board[player][b]|=1<<c;
        if(wins[board[player][b]]){won[player]|=1<<b;closed|=1<<b;}
        else if((board[0][b]|board[1][b])==511)closed|=1<<b;
        target=(closed>>c&1)?-1:c;player^=1;
    }
    int result() const {
        if(wins[won[0]])return 0;
        if(wins[won[1]])return 1;
        if(closed!=511)return -1;
        int a=__builtin_popcount(won[0]),b=__builtin_popcount(won[1]);
        return a==b?2:(a>b?0:1);
    }
};
int encode(int r,int c){return (r/3*3+c/3)*9+r%3*3+c%3;}
pair<int,int> decode(int m){int b=m/9,c=m%9;return {b/3*3+c/3,b%3*3+c%3};}
struct Node {int child=0,count=0,visits=0,move=-1;float value=0;};
struct Search {
    vector<Node> tree;
    Search(){tree.reserve(600000);tree.push_back({});}
    void reset(){tree.clear();tree.push_back({});}
    bool advance(int move){
        for(int i=0;i<tree[0].count;i++){
            int k=tree[0].child+i;
            if(tree[k].move==move){
                // Compact the surviving subtree to reclaim siblings and old ancestors.
                vector<Node> old;old.swap(tree);tree.reserve(600000);tree.push_back(old[k]);
                vector<int> source{k};
                for(size_t j=0;j<source.size();j++){
                    Node n=old[source[j]];
                    if(n.count){tree[j].child=tree.size();for(int z=0;z<n.count;z++){source.push_back(n.child+z);tree.push_back(old[n.child+z]);}}
                }
                return true;
            }
        }
        reset();return false;
    }
    int rollout(State s){
        int list[81];
        while(s.result()<0){
            int n=s.moves(list),pick=-1,seen=0;
            // Prefer immediate small-board wins; uniformly sample other moves.
            for(int i=0;i<n;i++){
                int m=list[i];
                if(wins[s.board[s.player][m/9]|(1<<(m%9))]){
                    if(wins[s.won[s.player]|(1<<(m/9))]){pick=i;break;}
                    if(random32()%++seen==0)pick=i;
                }
            }
            if(pick<0)pick=random32()%n;
            s.play(list[pick]);
        }
        return s.result();
    }
    int choose(State root,int milliseconds){
        auto end=chrono::steady_clock::now()+chrono::milliseconds(milliseconds);
        int iterations=0;
        do {
            State s=root;int at=0,path[82],depth=0;path[depth++]=0;
            while(s.result()<0){
                if(!tree[at].count){
                    int list[81],n=s.moves(list);
                    if(tree.size()+n>580000)break;
                    tree[at].child=tree.size();tree[at].count=n;
                    for(int i=n-1;i>0;i--)swap(list[i],list[random32()%(i+1)]);
                    for(int i=0;i<n;i++){Node node;node.move=list[i];tree.push_back(node);}
                }
                int best=-1;double score=-1e30;
                double logN=log(double(tree[at].visits+1));
                for(int i=0;i<tree[at].count;i++){
                    int k=tree[at].child+i;const Node& c=tree[k];
                    double u=c.visits?c.value/c.visits+0.55*sqrt(logN/c.visits):1e20;
                    if(u>score){score=u;best=k;}
                }
                at=best;s.play(tree[at].move);path[depth++]=at;
                if(tree[at].visits==0)break;
            }
            int outcome=rollout(s);
            // Each child stores utility for the player who selected that move.
            for(int i=0;i<depth;i++){
                Node& node=tree[path[i]];node.visits++;
                int mover=root.player^((i+1)&1);
                node.value+=outcome==2?0.5f:(outcome==mover?1.f:0.f);
            }
            iterations++;
        }while((iterations&31)||chrono::steady_clock::now()<end);
        int best=-1,visits=-1;
        for(int i=0;i<tree[0].count;i++){int k=tree[0].child+i;if(tree[k].visits>visits){visits=tree[k].visits;best=tree[k].move;}}
        cerr<<"simulations="<<iterations<<" nodes="<<tree.size()<<'\n';
        return best;
    }
};
#ifndef UNIT_TEST
int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);init();State state;Search search;bool first=true;
    int r,c;
    while(cin>>r>>c){
        if(r>=0){int m=encode(r,c);state.play(m);search.advance(m);}
        int n;cin>>n;vector<int> legal;
        for(int i=0;i<n;i++){int a,b;cin>>a>>b;legal.push_back(encode(a,b));}
        int m=search.choose(state,first?800:75);first=false;
        if(find(legal.begin(),legal.end(),m)==legal.end())m=legal[0];
        auto rc=decode(m);cout<<rc.first<<' '<<rc.second<<endl;
        state.play(m);search.advance(m);
    }
}
#endif
