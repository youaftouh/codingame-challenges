#include <bits/stdc++.h>
using namespace std;

// One-step stochastic placement search, with coordinated triangle approach plans.
// Every move goes to an explicit adjacent house, avoiding routing ambiguity.
struct Triangle { int v[3], owner, capture, enemyCapture; uint64_t mask; };
int n, turnNumber=0, dista[50][50], enemy[50], mine[50];
vector<int> adj[50];
vector<Triangle> triangles;
bool attackThreat[50];
mt19937 rng(2026);
uint64_t neighbors[50], within[50][51];
int forecast[50];
const int* evalOpponent=enemy;
bool useForecast=false;
uint64_t lastOwned,lastEnemyOwned,lastAlive;
int lastOther[50];

double evaluateBase(const vector<int>& targets) {
    int count[50]={}, other[50], near[50];
    for(int x:targets) ++count[x];
    copy(evalOpponent,evalOpponent+n,other);
    uint64_t majority=0,opponentMajority=0;
    for(int i=0;i<n;i++) {
        if(count[i]>other[i]) majority|=1ULL<<i;
        if(other[i]>count[i]) opponentMajority|=1ULL<<i;
    }
    double value=0;
    // The referee kills surrounded units simultaneously, before ownership.
    for(int i=0;i<n;i++) {
        if(neighbors[i] && !(neighbors[i]&~opponentMajority)) {value-=18*count[i];count[i]=0;}
        if(neighbors[i] && !(neighbors[i]&~majority)) {value+=8*other[i];other[i]=0;}
    }
    majority=opponentMajority=0;
    uint64_t occupied=0,owned=0,enemyOwned=0;
    for(int i=0;i<n;i++) {
        if(count[i]>other[i]) majority|=1ULL<<i;
        if(other[i]>count[i]) opponentMajority|=1ULL<<i;
        if(count[i]) {occupied|=1ULL<<i;value+=0.12;if(other[i]) value+=min(count[i],other[i]+1)*0.15;}
        if(attackThreat[i]) value-=4.0*max(0,count[i]-2);
    }
    for(int i=0;i<n;i++) {
        uint64_t bit=1ULL<<i;
        bool ours=neighbors[i] && !(neighbors[i]&~majority);
        bool theirs=neighbors[i] && !(neighbors[i]&~opponentMajority);
        if(ours || (!theirs && (majority&bit))) owned|=bit;
        else if(theirs || (opponentMajority&bit)) enemyOwned|=bit;
        near[i]=50;
        if(occupied) {
            int d=0;while(d<50 && !(within[i][d]&occupied)) ++d;
            near[i]=d;
        }
    }
    for(const auto& t:triangles) {
        int own=__builtin_popcountll(t.mask&owned);
        int opp=__builtin_popcountll(t.mask&enemyOwned);
        int empty=3-__builtin_popcountll(t.mask&occupied);
        if(t.owner==0) {
            value+=opp==3?-32:opp==2?-3:0;
        } else if(t.capture || empty==3) {
            if(!t.capture && empty==3) value+=12;
            double weight=t.owner==1?1.6:1.0;
            value+=weight*(own==3?32:own==2?5.5:own==1?1.2:0);
            int travel=0;
            for(int x:t.v) if(!(owned&(1ULL<<x))) travel+=near[x]+1+min(other[x],3);
            value+=weight*8.0/(1.0+travel);
        } else {
            value+=empty==2?1:0;
        }
    }
    lastOwned=owned;lastEnemyOwned=enemyOwned;lastAlive=occupied;
    copy(other,other+n,lastOther);
    return value;
}

double evaluate(const vector<int>& targets) {
    evalOpponent=enemy;double now=evaluateBase(targets);
    if(!useForecast) return now;
    evalOpponent=forecast;double next=evaluateBase(targets);
    evalOpponent=enemy;
    return .2*now+.8*next;
}

double evaluatePlan(const vector<int>& first,const vector<int>& second) {
    double immediate=evaluate(first);
    uint64_t own=lastOwned,opp=lastEnemyOwned,alive=lastAlive;
    int futureEnemy[50];copy(lastOther,lastOther+n,futureEnemy);
    int firstCount[50]={};
    static vector<int> future;future.clear();
    for(int i=0;i<(int)first.size();i++) if(alive&(1ULL<<first[i])) {firstCount[first[i]]++;future.push_back(second[i]);}
    int owners[200],caps[200],enemyCaps[200];
    for(int ti=0;ti<(int)triangles.size();ti++) {
        auto& t=triangles[ti];owners[ti]=t.owner;caps[ti]=t.capture;enemyCaps[ti]=t.enemyCapture;
        bool clear=true;for(int x:t.v)clear &= firstCount[x]==0;
        if(clear)t.capture=1;
        if(t.capture && (own&t.mask)==t.mask)t.owner=0;
        if(t.enemyCapture && (opp&t.mask)==t.mask)t.owner=1;
        if(t.owner==0) {
            // Model the next turn's production from newly captured triangles.
            int corner=t.v[0];for(int x:t.v)if(adj[x].size()>adj[corner].size())corner=x;
            future.push_back(corner);t.owner=-1;t.capture=0;
        }
    }
    evalOpponent=futureEnemy;
    double later=evaluateBase(future);
    evalOpponent=enemy;
    for(int ti=0;ti<(int)triangles.size();ti++) {triangles[ti].owner=owners[ti];triangles[ti].capture=caps[ti];triangles[ti].enemyCapture=enemyCaps[ti];}
    return immediate+0.65*later;
}

void predictOpponent(chrono::steady_clock::time_point start) {
    auto saved=triangles;
    bool savedThreat[50];copy(attackThreat,attackThreat+n,savedThreat);
    for(int i=0;i<n;i++) swap(mine[i],enemy[i]);
    for(auto& t:triangles) {if(t.owner>=0)t.owner=1-t.owner;swap(t.capture,t.enemyCapture);}
    fill(attackThreat,attackThreat+n,false);
    vector<int> source;
    for(int i=0;i<n;i++) for(int u=0;u<mine[i];u++) source.push_back(i);
    vector<int> placement=source;
    double score=evaluate(placement);
    for(int iter=0;!source.empty() && iter<8000;iter++) {
        if((iter&63)==0 && chrono::duration<double,milli>(chrono::steady_clock::now()-start).count()>5) break;
        int a=rng()%source.size(),b=rng()%source.size();
        int oldA=placement[a],oldB=placement[b];
        auto next=[&](int u){int x=source[u],r=rng()%(adj[x].size()+1);return r==(int)adj[x].size()?x:adj[x][r];};
        placement[a]=next(a);if(iter%3==0)placement[b]=next(b);
        double value=evaluate(placement);
        if(value>=score)score=value;
        else {placement[a]=oldA;placement[b]=oldB;}
    }
    fill(forecast,forecast+n,0);for(int x:placement)forecast[x]++;
    for(const auto& t:triangles) if(t.owner==0) {
        int target=t.v[0];
        for(int x:t.v) if(adj[x].size()>adj[target].size())target=x;
        forecast[target]++;
    }
    for(int i=0;i<n;i++)swap(mine[i],enemy[i]);
    triangles=saved;copy(savedThreat,savedThreat+n,attackThreat);
    useForecast=true;
}

int main() {
    ios::sync_with_stdio(false); cin.tie(nullptr);
    if(!(cin>>n)) return 0;
    for(int i=0,id,x,y;i<n;i++) cin>>id>>x>>y;
    int myScore, enemyScore;
    while(cin>>myScore>>enemyScore) {
        auto start=chrono::steady_clock::now();
        ++turnNumber;
        useForecast=false;
        for(int i=0,id,a,b;i<n;i++) {cin>>id>>a>>b; mine[id]=a; enemy[id]=b; adj[i].clear();}
        int m; cin>>m;
        for(int i=0,a,b;i<m;i++) {cin>>a>>b;adj[a].push_back(b);adj[b].push_back(a);}
        int k; cin>>k; triangles.resize(k);
        for(auto& t:triangles) cin>>t.v[0]>>t.v[1]>>t.v[2]>>t.owner>>t.capture>>t.enemyCapture;
        cin>>k; for(int i=0,a,b;i<k;i++) cin>>a>>b;
        vector<string> commands;
        // Resolve profitable attacks before planning movement. Each attack spends
        // one triangle and one stationary unit on each of its three corners.
        for(int attacks=0;attacks<3;attacks++) {
            int chosen=-1,target=-1; double top=0;
            for(int ti=0;ti<(int)triangles.size();ti++) {
                const auto& t=triangles[ti];
                if(t.owner!=0 || any_of(begin(t.v),end(t.v),[](int x){return mine[x]<1;})) continue;
                for(int corner:t.v) for(int x:adj[corner]) {
                    if(enemy[x]<4) continue;
                    double score=enemy[x]-3.5;
                    // Breaking an armed enemy triangle can also prevent retaliation.
                    for(const auto& e:triangles) if(e.owner==1 && all_of(begin(e.v),end(e.v),[](int v){return enemy[v]>0;}) && find(begin(e.v),end(e.v),x)!=end(e.v)) score+=1.0;
                    if(score>top) {top=score;chosen=ti;target=x;}
                }
            }
            if(chosen<0) break;
            auto& t=triangles[chosen];
            commands.push_back("ATTACK "+to_string(t.v[0])+" "+to_string(t.v[1])+" "+to_string(t.v[2])+" "+to_string(target));
            for(int x:t.v) --mine[x];
            enemy[target]=0;t.owner=-1;t.capture=0;
        }
        fill(attackThreat,attackThreat+n,false);
        for(const auto& t:triangles) if(t.owner==1 && all_of(begin(t.v),end(t.v),[](int x){return enemy[x]>0;}))
            for(int x:t.v) for(int y:adj[x]) attackThreat[y]=true;
        for(int i=0;i<n;i++) {
            fill(dista[i],dista[i]+n,50); dista[i][i]=0;
            queue<int> q; q.push(i);
            while(!q.empty()) {int x=q.front();q.pop();for(int y:adj[x]) if(dista[i][y]==50) {dista[i][y]=dista[i][x]+1;q.push(y);}}
            neighbors[i]=0;for(int j:adj[i]) neighbors[i]|=1ULL<<j;
            uint64_t layer=0;
            for(int d=0;d<=50;d++) {for(int j=0;j<n;j++) if(dista[i][j]==d) layer|=1ULL<<j;within[i][d]=layer;}
        }
        for(auto& t:triangles) t.mask=(1ULL<<t.v[0])|(1ULL<<t.v[1])|(1ULL<<t.v[2]);
        predictOpponent(start);
        vector<int> sources;
        for(int i=0;i<n;i++) for(int j=0;j<mine[i];j++) sources.push_back(i);
        int realUnits=sources.size();
        vector<vector<int>> choices;
        for(int x:sources) {choices.push_back(adj[x]);choices.back().push_back(x);}
        vector<pair<int,int>> spawnInfo;
        int enemyPopulation=accumulate(enemy,enemy+n,0);
        int armyTarget=min(3*n,max(n,enemyPopulation+20));
        if(turnNumber<175) for(int ti=0;ti<(int)triangles.size() && (int)sources.size()<armyTarget;ti++) {
            auto& t=triangles[ti];if(t.owner!=0)continue;
            int corner=t.v[0];for(int x:t.v)if(adj[x].size()>adj[corner].size())corner=x;
            spawnInfo.push_back({sources.size(),ti});sources.push_back(corner);
            choices.push_back(vector<int>(begin(t.v),end(t.v)));
            t.owner=-1;t.capture=0;
        }
        vector<int> best=sources;
        double bestValue=evaluatePlan(best,best);
        // Seed with plans assigning three distinct units to a triangle's corners.
        for(const auto& t:triangles) if(t.owner!=0 && (t.capture || all_of(begin(t.v),end(t.v),[](int x){return mine[x]==0;})) && sources.size()>=3) {
            array<int,3> perm={0,1,2};
            do {
                vector<int> plan=sources;
                vector<bool> used(sources.size(),false);
                for(int p:perm) {
                    int target=t.v[p], unit=-1;
                    for(int u=0;u<(int)sources.size();u++) if(!used[u] && (unit<0 || dista[sources[u]][target]<dista[sources[unit]][target])) unit=u;
                    used[unit]=true;
                    int from=sources[unit], next=from;
                    for(int x:choices[unit]) if(dista[x][target]<dista[next][target]) next=x;
                    plan[unit]=next;
                }
                double score=evaluatePlan(plan,plan);
                if(score>bestValue) {bestValue=score;best=plan;}
            } while(next_permutation(perm.begin(),perm.end()));
        }
        // Coordinate descent plus paired moves avoids many incomplete-capture traps.
        vector<int> current=best;
        vector<int> second=best,bestSecond=best;
        double currentValue=bestValue;
        int iterations=0;
        while(!sources.empty() && iterations<22000) {
            if((iterations++&127)==0 && chrono::duration<double,milli>(chrono::steady_clock::now()-start).count()>32) break;
            int a=rng()%sources.size(), b=rng()%sources.size();
            int oldA=current[a],oldB=current[b];
            int oldSecondA=second[a],oldSecondB=second[b];
            bool changeFirst=rng()%2;
            auto mutate=[&](int u) {
                if(changeFirst) {
                    current[u]=choices[u][rng()%choices[u].size()];
                    if(second[u]!=current[u] && find(adj[current[u]].begin(),adj[current[u]].end(),second[u])==adj[current[u]].end())second[u]=current[u];
                } else {
                    int x=current[u],r=rng()%(adj[x].size()+1);
                    second[u]=r==(int)adj[x].size()?x:adj[x][r];
                }
            };
            mutate(a);if(iterations%3==0)mutate(b);
            double score=evaluatePlan(current,second);
            if(score+1e-8>=currentValue) {
                currentValue=score;
                if(score>bestValue+1e-8) {bestValue=score;best=current;bestSecond=second;}
            } else {current[a]=oldA;current[b]=oldB;second[a]=oldSecondA;second[b]=oldSecondB;}
            if(iterations%2500==0) {current=best;second=bestSecond;currentValue=bestValue;}
        }
        int moving[50][50]={};
        for(int u=0;u<realUnits;u++) if(sources[u]!=best[u]) moving[sources[u]][best[u]]++;
        for(int a=0;a<n;a++) for(int b:adj[a]) if(moving[a][b]) commands.push_back("MOVE "+to_string(a)+" "+to_string(b)+" "+to_string(moving[a][b]));
        int population=sources.size();
        for(auto info:spawnInfo) {
            const auto& t=triangles[info.second];int spawn=best[info.first];
            int other[2],j=0;for(int x:t.v) if(x!=spawn) other[j++]=x;
            commands.push_back("SPAWN "+to_string(spawn)+" "+to_string(other[0])+" "+to_string(other[1]));
        }
        if(commands.empty()) cout<<"WAIT";
        else for(int i=0;i<(int)commands.size();i++) cout<<(i?";":"")<<commands[i];
        cout<<endl;
        cerr<<"turn="<<turnNumber<<" units="<<population<<" search="<<iterations<<" value="<<bestValue<<'\n';
    }
}
