#include <bits/stdc++.h>
using namespace std;

// One-step stochastic placement search, with coordinated triangle approach plans.
// Every move goes to an explicit adjacent house, avoiding routing ambiguity.
struct Triangle { int v[3], owner, capture, enemyCapture; };
int n, turnNumber=0, dista[50][50], enemy[50], mine[50];
vector<int> adj[50];
vector<Triangle> triangles;
bool attackThreat[50];
int forecast[50];
bool useForecast=false;
mt19937 rng(2026);

double evaluateAgainst(const vector<int>& targets, const int* opposition) {
    int count[50]={}, owner[50], near[50];
    for(int x:targets) ++count[x];
    int queue[50], head=0, tail=0;
    for(int i=0;i<n;i++) {
        near[i]=count[i]>0?0:50;
        if(count[i]>0) queue[tail++]=i;
    }
    while(head<tail) {
        int x=queue[head++];
        for(int y:adj[x]) if(near[y]==50) {
            near[y]=near[x]+1;
            queue[tail++]=y;
        }
    }
    double value=0;
    for(int i=0;i<n;i++) {
        bool surroundMe=!adj[i].empty(), surroundEnemy=surroundMe;
        for(int j:adj[i]) {
            surroundMe &= count[j]>opposition[j];
            surroundEnemy &= opposition[j]>count[j];
        }
        owner[i]=surroundMe?0:surroundEnemy?1:count[i]>opposition[i]?0:count[i]<opposition[i]?1:-1;
        if(surroundEnemy) value-=18*count[i];
        if(surroundMe) value+=8*opposition[i];
        if(count[i]>0) {
            value+=0.12;
            if(opposition[i]) value+=min(count[i],opposition[i]+1)*0.15;
        }
        // An enemy triangle attacks before our next move: stacked units cannot flee.
        if(attackThreat[i]) value-=4.0*max(0,count[i]-2);
    }
    for(const auto& t:triangles) {
        int own=0, opp=0, empty=0, travel=0;
        for(int x:t.v) {
            own+=owner[x]==0; opp+=owner[x]==1;
            empty+=count[x]==0;
            travel+=owner[x]==0?0:near[x]+1+min(opposition[x],3);
        }
        if(t.owner==0) {
            // Ownership persists without garrisons; only a full enemy capture loses it.
            value+=opp==3?-32:opp==2?-3:0;
        } else if(t.capture || empty==3) {
            double weight=t.owner==1?1.6:1.0;
            value+=weight*(own==3?32:own==2?5.5:own==1?1.2:0);
            value+=weight*8.0/(1.0+travel);
        } else {
            // Clear all three corners to reset a spent triangle.
            value+=empty==3?9:empty==2?1:0;
        }
    }
    return value;
}

double evaluate(const vector<int>& targets) {
    double now=evaluateAgainst(targets,enemy);
    return useForecast ? 0.35*now+0.65*evaluateAgainst(targets,forecast) : now;
}

void predictOpponent(chrono::steady_clock::time_point start) {
    // Predict a legal enemy placement using the same capture objective with
    // player identities reversed. Keep a stationary scenario as a hedge.
    auto savedTriangles=triangles;
    bool savedThreat[50];copy(attackThreat,attackThreat+n,savedThreat);
    for(int i=0;i<n;i++) swap(mine[i],enemy[i]);
    for(auto& t:triangles) {
        if(t.owner>=0) t.owner=1-t.owner;
        swap(t.capture,t.enemyCapture);
    }
    fill(attackThreat,attackThreat+n,false);
    vector<int> source;
    for(int i=0;i<n;i++) for(int u=0;u<mine[i];u++) source.push_back(i);
    vector<int> placement=source;
    double score=evaluate(placement);
    for(int iter=0;!source.empty() && iter<4000;iter++) {
        if((iter&63)==0 && chrono::duration<double,milli>(chrono::steady_clock::now()-start).count()>5) break;
        int a=rng()%source.size(),b=rng()%source.size();
        int oldA=placement[a],oldB=placement[b];
        auto next=[&](int u){int x=source[u],r=rng()%(adj[x].size()+1);return r==(int)adj[x].size()?x:adj[x][r];};
        placement[a]=next(a); if(iter%3==0) placement[b]=next(b);
        double value=evaluate(placement);
        if(value>=score) score=value;
        else {placement[a]=oldA;placement[b]=oldB;}
    }
    fill(forecast,forecast+n,0);
    for(int x:placement) forecast[x]++;
    // Opponent spawns happen after moves, using triangles already owned.
    for(const auto& t:triangles) if(t.owner==0 && source.size()<3*n) {
        int target=t.v[0];
        for(int x:t.v) if(forecast[x]<forecast[target]) target=x;
        forecast[target]++;
    }
    for(int i=0;i<n;i++) swap(mine[i],enemy[i]);
    triangles=savedTriangles;copy(savedThreat,savedThreat+n,attackThreat);
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
        }
        predictOpponent(start);
        vector<int> sources;
        for(int i=0;i<n;i++) for(int j=0;j<mine[i];j++) sources.push_back(i);
        vector<int> best=sources;
        double bestValue=evaluate(best);
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
                    for(int x:adj[from]) if(dista[x][target]<dista[next][target]) next=x;
                    plan[unit]=next;
                }
                double score=evaluate(plan);
                if(score>bestValue) {bestValue=score;best=plan;}
            } while(next_permutation(perm.begin(),perm.end()));
        }
        // Coordinate descent plus paired moves avoids many incomplete-capture traps.
        vector<int> current=best;
        double currentValue=bestValue;
        int iterations=0;
        while(!sources.empty() && iterations<22000) {
            if((iterations++&127)==0 && chrono::duration<double,milli>(chrono::steady_clock::now()-start).count()>32) break;
            int a=rng()%sources.size(), b=rng()%sources.size();
            int oldA=current[a],oldB=current[b];
            auto destination=[&](int u) {int from=sources[u];int r=rng()%(adj[from].size()+1);return r==(int)adj[from].size()?from:adj[from][r];};
            current[a]=destination(a);
            if(iterations%3==0) current[b]=destination(b);
            double score=evaluate(current);
            if(score+1e-8>=currentValue) {
                currentValue=score;
                if(score>bestValue+1e-8) {bestValue=score;best=current;}
            } else {current[a]=oldA;current[b]=oldB;}
            if(iterations%2500==0) {current=best;currentValue=bestValue;}
        }
        int moving[50][50]={};
        for(int u=0;u<(int)sources.size();u++) if(sources[u]!=best[u]) moving[sources[u]][best[u]]++;
        for(int a=0;a<n;a++) for(int b:adj[a]) if(moving[a][b]) commands.push_back("MOVE "+to_string(a)+" "+to_string(b)+" "+to_string(moving[a][b]));
        int population=sources.size();
        int enemyPopulation=accumulate(enemy,enemy+n,0);
        int armyTarget=min(3*n,max(n,enemyPopulation+20));
        for(const auto& t:triangles) if(t.owner==0 && population<armyTarget && turnNumber<175) {
            int spawn=t.v[0]; double top=-1e100;
            for(int x:t.v) {best.push_back(x);double score=evaluate(best);best.pop_back();if(score>top) {top=score;spawn=x;}}
            int other[2],j=0;for(int x:t.v) if(x!=spawn) other[j++]=x;
            commands.push_back("SPAWN "+to_string(spawn)+" "+to_string(other[0])+" "+to_string(other[1]));
            best.push_back(spawn); population++;
        }
        if(commands.empty()) cout<<"WAIT";
        else for(int i=0;i<(int)commands.size();i++) cout<<(i?";":"")<<commands[i];
        cout<<endl;
        cerr<<"turn="<<turnNumber<<" units="<<population<<" search="<<iterations<<" value="<<bestValue<<'\n';
    }
}
