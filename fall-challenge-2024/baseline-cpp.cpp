#include <bits/stdc++.h>
using namespace std;
struct Building { int id,type,x,y; vector<int> people; };
struct Pod { int id; vector<int> path; };
vector<Building> b;
int cap[150][150], degree_[150], tele[150], dist_[21][150];
vector<Pod> pods;
int cross(int a,int c,int d) {return (b[c].x-b[a].x)*(b[d].y-b[a].y)-(b[c].y-b[a].y)*(b[d].x-b[a].x);}
bool legal(int a,int c) {
    if(cap[a][c] || degree_[a]>=5 || degree_[c]>=5) return false;
    for(int k=0;k<(int)b.size();k++) if(k!=a&&k!=c && cross(a,c,k)==0 &&
        (b[k].x-b[a].x)*(b[k].x-b[c].x)+(b[k].y-b[a].y)*(b[k].y-b[c].y)<=0) return false;
    for(int i=0;i<(int)b.size();i++) for(int j=0;j<i;j++) if(cap[i][j]>0 && i!=a&&i!=c&&j!=a&&j!=c) {
        if((long long)cross(a,c,i)*cross(a,c,j)<0 && (long long)cross(i,j,a)*cross(i,j,c)<0) return false;
    }
    return true;
}
void distances() {
    int n=b.size();
    for(int t=1;t<=20;t++) {
        fill(dist_[t],dist_[t]+n,999); deque<int> q;
        for(int i=0;i<n;i++) if(b[i].type==t) {dist_[t][i]=0;q.push_back(i);}
        while(!q.empty()) {int v=q.front();q.pop_front();
            for(int u=0;u<n;u++) {
                int w=tele[u]==v?0:(cap[u][v]>0?1:999);
                if(dist_[t][u]>dist_[t][v]+w) {dist_[t][u]=dist_[t][v]+w;if(w)q.push_back(u);else q.push_front(u);}
            }
        }
    }
}
// Simulate the month, including pod contention, passenger priorities and balancing.
double simulate() {
    distances();
    vector<int> loc,types; vector<int> order(b.size()); iota(order.begin(),order.end(),0);
    sort(order.begin(),order.end(),[](int a,int c){return b[a].id<b[c].id;});
    for(int i:order) for(int t:b[i].people) {loc.push_back(i);types.push_back(t);}
    vector<int> pos(pods.size(),0), arrivals(b.size(),0);
    double score=0;
    for(int day=0;day<20;day++) {
        for(int k=0;k<(int)loc.size();k++) if(loc[k]>=0) {
            int v=loc[k],w=tele[v];
            if(w>=0 && dist_[types[k]][w]<=dist_[types[k]][v]) loc[k]=w;
            v=loc[k]; if(b[v].type==types[k]) {score+=50-day+max(0,50-arrivals[v]++);loc[k]=-1;}
        }
        int used[150][150]={};
        vector<int> dest(pods.size(),-1), seats(pods.size(),10);
        vector<vector<int>> leaving(b.size());
        for(int p=0;p<(int)pods.size();p++) {
            auto &path=pods[p].path; int at=pos[p];
            if(at+1>=(int)path.size())continue;
            int a=path[at],c=path[at+1],lo=min(a,c),hi=max(a,c);
            if(used[lo][hi]>=cap[a][c])continue;
            used[lo][hi]++; dest[p]=c; leaving[a].push_back(p);
        }
        for(int k=0;k<(int)loc.size();k++) if(loc[k]>=0) {
            int v=loc[k],t=types[k];
            for(int p:leaving[v]) if(seats[p] && dist_[t][dest[p]]<dist_[t][v]) {
                seats[p]--;loc[k]=dest[p];break;
            }
            v=loc[k];if(b[v].type==t) {score+=49-day+max(0,50-arrivals[v]++);loc[k]=-1;}
        }
        for(int p=0;p<(int)pods.size();p++) if(dest[p]>=0) {
            pos[p]++;auto &path=pods[p].path;
            if(pos[p]+1==(int)path.size() && path.front()==path.back())pos[p]=0;
        }
    }
    return score;
}
int main() {
    ios::sync_with_stdio(false);cin.tie(nullptr);
    map<int,int> index; int money,month=0;
    while(cin>>money) {
        auto start=chrono::steady_clock::now(); month++;
        memset(cap,0,sizeof cap);fill(degree_,degree_+150,0);fill(tele,tele+150,-1);
        int n;cin>>n;
        for(int k=0;k<n;k++) {int a,c,v;cin>>a>>c>>v;a=index.at(a);c=index.at(c);if(v==0)tele[a]=c;else {cap[a][c]=cap[c][a]=v;degree_[a]++;degree_[c]++;}}
        cin>>n;pods.clear();bool taken[501]={};
        for(int k=0;k<n;k++){Pod p;int len;cin>>p.id>>len;taken[p.id]=true;while(len--){int v;cin>>v;p.path.push_back(index.at(v));}pods.push_back(p);}
        sort(pods.begin(),pods.end(),[](const Pod&a,const Pod&c){return a.id<c.id;});
        cin>>n;
        while(n--) {Building v;cin>>v.type>>v.id>>v.x>>v.y;if(v.type==0){int count;cin>>count;while(count--){int t;cin>>t;v.people.push_back(t);}}index[v.id]=b.size();b.push_back(v);}
        vector<string> actions;
        double current=simulate();
        for(int iter=0;iter<50;iter++) {
            if(chrono::duration<double,milli>(chrono::steady_clock::now()-start).count()>320)break;
            int pid=1;while(pid<=500&&taken[pid])pid++;if(pid>500)break;
            struct Candidate{int a,c,cost,kind;double priority;};vector<Candidate> candidates;
            bool occupied[150]={};for(int i=0;i<(int)b.size();i++)if(tele[i]>=0)occupied[i]=occupied[tele[i]]=true;
            // Favor links which shorten a landing pad's route to a required type.
            for(int a=0;a<(int)b.size();a++)for(int c=0;c<(int)b.size();c++)if(a!=c) {
                int cost=(int)(hypot(b[a].x-b[c].x,b[a].y-b[c].y)*10)+1000;
                double benefit=0;
                for(int t:b[a].people) if(dist_[t][c]<dist_[t][a])benefit+=dist_[t][a]>=999?90:8;
                if(benefit && cost<=money && (cap[a][c]>0 || legal(a,c))) {
                    if(cap[a][c]>0)cost=1000;
                    candidates.push_back({a,c,cost,0,benefit/pow(cost,0.75)});
                }
                if(!occupied[a]&&!occupied[c]&&money>=5000&&!b[a].people.empty()) {
                    double gain=0;for(int t:b[a].people)if(dist_[t][c]<dist_[t][a])gain+=dist_[t][a]>=999?90:10;
                    if(gain)candidates.push_back({a,c,5000,1,gain/pow(5000,0.75)});
                }
            }
            sort(candidates.begin(),candidates.end(),[](auto&a,auto&c){return a.priority>c.priority;});
            double best=0,bestScore=current;int choice=-1;
            for(int k=0;k<min(24,(int)candidates.size());k++) {
                if(chrono::duration<double,milli>(chrono::steady_clock::now()-start).count()>420)break;
                auto x=candidates[k];int old=cap[x.a][x.c];
                if(x.kind)tele[x.a]=x.c;
                else {cap[x.a][x.c]=cap[x.c][x.a]=max(1,old);pods.push_back({pid,{x.a,x.c,x.a}});}
                double score=simulate();
                double value=(score-current)/pow(x.cost,0.75);
                if(value>best+1e-9){best=value;choice=k;bestScore=score;}
                if(x.kind)tele[x.a]=-1;
                else {cap[x.a][x.c]=cap[x.c][x.a]=old;pods.pop_back();}
            }
            distances();
            if(choice<0)break;
            auto x=candidates[choice];money-=x.cost;current=bestScore;
            string ends=to_string(b[x.a].id)+" "+to_string(b[x.c].id);
            if(x.kind){tele[x.a]=x.c;actions.push_back("TELEPORT "+ends);}
            else {
                if(!cap[x.a][x.c]){cap[x.a][x.c]=cap[x.c][x.a]=1;degree_[x.a]++;degree_[x.c]++;actions.push_back("TUBE "+ends);}
                pods.push_back({pid,{x.a,x.c,x.a}});taken[pid]=true;
                actions.push_back("POD "+to_string(pid)+" "+ends+" "+to_string(b[x.a].id));
            }
            distances();
        }
        cerr<<"month="<<month<<" predicted="<<current<<" resources="<<money<<" ms="<<chrono::duration<double,milli>(chrono::steady_clock::now()-start).count()<<'\n';
        if(actions.empty())cout<<"WAIT";else for(int i=0;i<(int)actions.size();i++){if(i)cout<<';';cout<<actions[i];}cout<<endl;
    }
}
