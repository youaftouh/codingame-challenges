#include <bits/stdc++.h>
using namespace std;
constexpr int H=3,N=50,T=150,SLOTS=512;
struct Tri {int v[3];uint64_t mask;};
struct State {int u[2][N]{};int owner[T]{};bool cap[2][T]{};};
struct Plan {unsigned char move[H][N][SLOTS]{};unsigned char spawn[H][T]{};};
struct Metrics {int spawn[2]{},loss[2]{},points[2]{},capture[2]{},reset[2]{};};
int n,nt,roundNo,distances[N][N],hop[N][N];
vector<int> adj[N];vector<Tri> triangles;
uint64_t neighborMask[N],within[N][N+1];
Plan minePlan,enemyPlan;
State root,currentTrace[H],trialTrace[H];
mt19937 randomGen(843364);
auto began=chrono::steady_clock::now();
double elapsed(){return chrono::duration<double,milli>(chrono::steady_clock::now()-began).count();}

void majorities(const State& s,uint64_t& a,uint64_t& b){
    a=b=0;for(int i=0;i<n;i++){if(s.u[0][i]>s.u[1][i])a|=1ULL<<i;if(s.u[1][i]>s.u[0][i])b|=1ULL<<i;}
}
void owners(const State& s,uint64_t& a,uint64_t& b){
    uint64_t ma,mb;majorities(s,ma,mb);a=b=0;
    for(int i=0;i<n;i++){
        bool sa=neighborMask[i] && !(neighborMask[i]&~ma),sb=neighborMask[i] && !(neighborMask[i]&~mb);
        if(sa || (!sb && (ma&(1ULL<<i))))a|=1ULL<<i;
        else if(sb || (mb&(1ULL<<i)))b|=1ULL<<i;
    }
}
void resolve(State& s,Metrics& m){
    uint64_t a,b;majorities(s,a,b);
    for(int i=0;i<n;i++)if(neighborMask[i]){
        if(!(neighborMask[i]&~b)){m.loss[0]+=s.u[0][i];s.u[0][i]=0;}
        if(!(neighborMask[i]&~a)){m.loss[1]+=s.u[1][i];s.u[1][i]=0;}
    }
    owners(s,a,b);
    for(int t=0;t<nt;t++){
        const auto& tr=triangles[t];int old=s.owner[t];
        for(int p=0;p<2;p++){
            if(old>=0)s.cap[p][t]=old!=p;
            else if(!s.cap[p][t]){
                bool clear=true;for(int x:tr.v)clear &= s.u[p][x]==0;
                if(clear){s.cap[p][t]=true;m.reset[p]++;}
            }
        }
        if(s.cap[0][t] && (a&tr.mask)==tr.mask)s.owner[t]=0;
        if(s.cap[1][t] && (b&tr.mask)==tr.mask)s.owner[t]=1;
        if(s.owner[t]>=0){int p=s.owner[t];m.points[p]++;m.capture[p]+=old!=p;s.cap[p][t]=false;s.cap[1-p][t]=true;}
    }
}

void attacks(State& s,int p,Metrics& m,vector<string>* output){
    for(int iteration=0;iteration<2;iteration++){
        int chosen=-1,target=-1,amount=5;
        for(int t=0;t<nt;t++)if(s.owner[t]==p){
            const auto& tr=triangles[t];bool armed=true;for(int x:tr.v)armed &= s.u[p][x]>0;
            if(!armed)continue;
            for(int x:tr.v)for(int y:adj[x])if(s.u[1-p][y]>amount){amount=s.u[1-p][y];chosen=t;target=y;}
        }
        if(chosen<0)break;
        const auto& tr=triangles[chosen];
        if(output)output->push_back("ATTACK "+to_string(tr.v[0])+" "+to_string(tr.v[1])+" "+to_string(tr.v[2])+" "+to_string(target));
        for(int x:tr.v){s.u[p][x]--;m.loss[p]++;}
        s.owner[chosen]=-1;s.cap[p][chosen]=false;
        m.loss[1-p]+=s.u[1-p][target];s.u[1-p][target]=0;
    }
}

Metrics step(State& s,int depth,bool stationaryEnemy=false,vector<string>* output=nullptr){
    Metrics m;
    // Treat our attack first in the model; real simultaneous attacks can differ.
    attacks(s,0,m,output);attacks(s,1,m,nullptr);
    int next[2][N]{};
    for(int p=0;p<2;p++)for(int from=0;from<n;from++){
        int moved[N]{};const Plan& plan=p?enemyPlan:minePlan;
        for(int slot=0;slot<s.u[p][from];slot++){
            int target=p && stationaryEnemy?from:plan.move[depth][from][slot%SLOTS];
            int to=target<n?hop[from][target]:from;
            next[p][to]++;if(to!=from)moved[to]++;
        }
        if(output && p==0)for(int to:adj[from])if(moved[to])output->push_back("MOVE "+to_string(from)+" "+to_string(to)+" "+to_string(moved[to]));
    }
    memcpy(s.u,next,sizeof(next));
    int population[2]{};for(int p=0;p<2;p++)for(int i=0;i<n;i++)population[p]+=s.u[p][i];
    int ownTarget=min(3*n,max(n,population[1]+20));
    for(int t=0;t<nt;t++)if(s.owner[t]>=0){
        int p=s.owner[t];if(p==0 && (population[0]>=ownTarget || roundNo+depth>=175))continue;
        if(p==1 && population[1]>=6*n)continue;
        const auto& tr=triangles[t];const Plan& plan=p?enemyPlan:minePlan;
        int index=plan.spawn[depth][t]%3,corner=tr.v[index];
        if(output && p==0)output->push_back("SPAWN "+to_string(corner)+" "+to_string(tr.v[(index+1)%3])+" "+to_string(tr.v[(index+2)%3]));
        s.u[p][corner]++;population[p]++;m.spawn[p]++;
        s.owner[t]=-1;s.cap[p][t]=false;
    }
    resolve(s,m);return m;
}

double reward(const Metrics& m){
    return 4.0*(m.points[0]-m.points[1])+3.0*(m.capture[0]-m.capture[1])
        +6.0*(m.spawn[0]-m.spawn[1])-2.5*(m.loss[0]-m.loss[1])+1.5*(m.reset[0]-m.reset[1]);
}
double potential(const State& s){
    uint64_t owned[2],occupied[2]{};owners(s,owned[0],owned[1]);
    int near[2][N],population[2]{};
    for(int p=0;p<2;p++){
        for(int i=0;i<n;i++){population[p]+=s.u[p][i];if(s.u[p][i])occupied[p]|=1ULL<<i;}
        for(int i=0;i<n;i++){
            int d=0;while(d<n && !(within[i][d]&occupied[p]))d++;
            near[p][i]=d;
        }
    }
    double value=1.5*(population[0]-population[1]);
    for(int p=0;p<2;p++){
        double score=.1*__builtin_popcountll(occupied[p]);
        for(int t=0;t<nt;t++)if(s.owner[t]!=p && s.cap[p][t]){
            int corners=__builtin_popcountll(triangles[t].mask&owned[p]),travel=0;
            score+=corners==2?1.5:corners==1?.3:0;
            for(int x:triangles[t].v)travel+=near[p][x];
            score+=2.0/(1+travel);
        }
        value+=(p?-1:1)*score;
    }
    return value;
}

double rollout(State* trace=nullptr){
    State s=root;double score=0,discount=1;
    for(int d=0;d<H;d++){
        if(trace)trace[d]=s;
        Metrics m=step(s,d);score+=discount*reward(m);discount*=.8;
    }
    score+=potential(s);
    // Hedge the first-turn forecast with an enemy that stays at its houses.
    State staticState=root;Metrics stationary=step(staticState,0,true);
    return .8*score+.2*(2.4*reward(stationary)+potential(staticState));
}

unsigned char destination(int from){int r=randomGen()%(adj[from].size()+1);return r==(int)adj[from].size()?from:adj[from][r];}
void preparePlans(bool first){
    for(Plan* plan:{&minePlan,&enemyPlan}){
        if(!first){memmove(plan->move[0],plan->move[1],2*N*SLOTS);memmove(plan->spawn[0],plan->spawn[1],2*T);}
        for(int d=first?0:H-1;d<H;d++){
            for(int x=0;x<n;x++)for(int slot=0;slot<SLOTS;slot++)plan->move[d][x][slot]=destination(x);
            for(int t=0;t<nt;t++)plan->spawn[d][t]=randomGen()%3;
        }
    }
}
void forecastEnemy(){
    State s=root;
    for(int d=0;d<H;d++){
        State preview=s;double score;
        {Metrics m=step(preview,d);score=-reward(m)-potential(preview);}
        for(int iter=0;iter<1000;iter++){
            if((iter&31)==0 && elapsed()>(d+1)*2.0)break;
            int total=0;for(int x=0;x<n;x++)total+=s.u[1][x];
            if(total==0)break;
            unsigned char* variable;unsigned char replacement;
            if(randomGen()%5==0 && nt){int t=randomGen()%nt;variable=&enemyPlan.spawn[d][t];replacement=randomGen()%3;}
            else {
                int unit=randomGen()%total,x=0;while(unit>=s.u[1][x])unit-=s.u[1][x++];
                variable=&enemyPlan.move[d][x][unit%SLOTS];replacement=destination(x);
            }
            unsigned char backup=*variable;*variable=replacement;
            preview=s;Metrics m=step(preview,d);double next=-reward(m)-potential(preview);
            if(next>=score)score=next;else *variable=backup;
        }
        step(s,d);
    }
}

struct Edit {unsigned char* at;unsigned char old;};
void change(vector<Edit>& edits,unsigned char* at,unsigned char value){edits.push_back({at,*at});*at=value;}

int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    if(!(cin>>n))return 0;
    for(int i=0,id,x,y;i<n;i++)cin>>id>>x>>y;
    int myScore,opponentScore;
    while(cin>>myScore>>opponentScore){
        began=chrono::steady_clock::now();roundNo++;
        for(int i=0;i<n;i++)adj[i].clear();
        for(int i=0,id,a,b;i<n;i++){cin>>id>>a>>b;root.u[0][id]=a;root.u[1][id]=b;}
        int k;cin>>k;for(int i=0,a,b;i<k;i++){cin>>a>>b;adj[a].push_back(b);adj[b].push_back(a);}
        cin>>nt;if(nt>T)return 0;triangles.resize(nt);
        for(int t=0;t<nt;t++){
            auto& tr=triangles[t];int a,b;cin>>tr.v[0]>>tr.v[1]>>tr.v[2]>>root.owner[t]>>a>>b;
            root.cap[0][t]=a;root.cap[1][t]=b;tr.mask=(1ULL<<tr.v[0])|(1ULL<<tr.v[1])|(1ULL<<tr.v[2]);
        }
        cin>>k;for(int i=0,a,b;i<k;i++)cin>>a>>b;
        for(int x=0;x<n;x++){
            sort(adj[x].begin(),adj[x].end());neighborMask[x]=0;for(int y:adj[x])neighborMask[x]|=1ULL<<y;
            fill(distances[x],distances[x]+n,n);distances[x][x]=0;queue<int> q;q.push(x);
            while(!q.empty()){int a=q.front();q.pop();for(int b:adj[a])if(distances[x][b]==n){distances[x][b]=distances[x][a]+1;q.push(b);}}
            uint64_t layer=0;for(int d=0;d<=n;d++){for(int y=0;y<n;y++)if(distances[x][y]==d)layer|=1ULL<<y;within[x][d]=layer;}
        }
        for(int a=0;a<n;a++)for(int b=0;b<n;b++){
            hop[a][b]=a;if(a!=b)for(int x:adj[a])if(distances[x][b]<distances[a][b]){hop[a][b]=x;break;}
        }
        preparePlans(roundNo==1);forecastEnemy();
        double score=rollout(currentTrace);int iterations=0;
        double budget=roundNo==1?750:38;
        while(iterations<150000){
            if((iterations++&15)==0 && elapsed()>budget)break;
            int d=randomGen()%H;vector<Edit> edits;edits.reserve(6);
            const State& s=currentTrace[d];
            if(randomGen()%7==0 && nt){
                int t=randomGen()%nt;array<int,3> order={0,1,2};shuffle(order.begin(),order.end(),randomGen);
                int used[N]{};
                for(int corner:order){
                    int target=triangles[t].v[corner],from=-1;
                    for(int x=0;x<n;x++)if(used[x]<s.u[0][x] && (from<0 || distances[x][target]<distances[from][target]))from=x;
                    if(from<0)break;int slot=used[from]++%SLOTS;
                    change(edits,&minePlan.move[d][from][slot],hop[from][target]);
                }
            }else if(randomGen()%5==0 && nt){int t=randomGen()%nt;change(edits,&minePlan.spawn[d][t],randomGen()%3);}
            else {
                int total=0;for(int x=0;x<n;x++)total+=s.u[0][x];
                if(!total)continue;
                int mutations=randomGen()%3==0?2:1;
                for(int m=0;m<mutations;m++){
                    int unit=randomGen()%total,x=0;while(unit>=s.u[0][x])unit-=s.u[0][x++];
                    change(edits,&minePlan.move[d][x][unit%SLOTS],destination(x));
                }
            }
            double candidate=rollout(trialTrace);
            if(candidate>=score){score=candidate;memcpy(currentTrace,trialTrace,sizeof(currentTrace));}
            else for(int i=(int)edits.size()-1;i>=0;i--)*edits[i].at=edits[i].old;
        }
        State played=root;vector<string> output;step(played,0,false,&output);
        if(output.empty())cout<<"WAIT";
        else for(int i=0;i<(int)output.size();i++)cout<<(i?";":"")<<output[i];
        cout<<endl;
        cerr<<"turn="<<roundNo<<" rollouts="<<iterations<<" value="<<score<<" ms="<<elapsed()<<'\n';
    }
}
