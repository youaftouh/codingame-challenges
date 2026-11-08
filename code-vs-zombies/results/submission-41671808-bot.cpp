#include <bits/stdc++.h>
using namespace std;
struct P { int x,y; bool operator==(const P& b)const{return x==b.x&&y==b.y;} };
struct State { P ash,h[100],z[100]; int nh=0,nz=0; double score=0; };
struct Plan { vector<P> g; double score=-2; P first; };
mt19937 rng(731);
int ri(int n){return int(rng()%n);}
double combo[101];
int dist2(P a,P b){int x=a.x-b.x,y=a.y-b.y;return x*x+y*y;}
double dist(P a,P b){return sqrt(double(dist2(a,b)));}
P bounded(P a){return {clamp(a.x,0,15999),clamp(a.y,0,8999)};}
P moveTo(P a,P b,int speed){double d=dist(a,b); if(d<=speed)return b;
 return {(int)(a.x+(b.x-a.x)*speed/d),(int)(a.y+(b.y-a.y)*speed/d)};}
P rescue(const State&s,int mode=0){
 double best=-1e99; P target=s.z[0];
 for(int i=0;i<s.nh;i++){
  int k=0;for(int j=1;j<s.nz;j++)if(dist2(s.h[i],s.z[j])<dist2(s.h[i],s.z[k]))k=j;
  double danger=dist(s.h[i],s.z[k])/400,travel=max(0.,(dist(s.ash,s.h[i])-2000)/1000);
  double v=mode?-travel:-danger;
  if(travel<=danger+0.5&&v>best)best=v,target=s.z[k];
 }
 if(best==-1e99)for(int j=0;j<s.nz;j++)if(dist2(s.ash,s.z[j])<dist2(s.ash,target))target=s.z[j];
 return target;
}
P policy(const State&s,P g){
 if(g.x>=0)return g;
 int nearest=0;for(int j=1;j<s.nz;j++)if(dist2(s.ash,s.z[j])<dist2(s.ash,s.z[nearest]))nearest=j;
 P z=s.z[nearest];double dx=s.ash.x-z.x,dy=s.ash.y-z.y,d=hypot(dx,dy);
 if(g.x==-2){
  if(d>abs(g.y)+600){P p={0,0};for(int j=0;j<s.nz;j++)p.x+=s.z[j].x,p.y+=s.z[j].y;return {p.x/s.nz,p.y/s.nz};}
  double out=clamp((abs(g.y)-d+400)/800.,-0.5,1.5),sign=g.y>0?1:-1;
  if(d<1)d=1;
  return bounded({int(s.ash.x+1500*(dx*out-dy*sign)/d),int(s.ash.y+1500*(dy*out+dx*sign)/d)});
 }
 double urgent=1e99;P target=z;
 for(int i=0;i<s.nh;i++){
  int k=0;for(int j=1;j<s.nz;j++)if(dist2(s.h[i],s.z[j])<dist2(s.h[i],s.z[k]))k=j;
  double slack=dist(s.h[i],s.z[k])/400-max(0.,(dist(s.ash,s.z[k])-2000)/1400);
  if(slack>=-0.5&&slack<urgent)urgent=slack,target=s.z[k];
 }
 if(urgent<=g.y||urgent==1e99)return target;
 if(d>2900)return s.ash;
 if(d<1)d=1,dx=1;
 return bounded({int(z.x+dx/d*2900),int(z.y+dy/d*2900)});
}
void step(State&s,P target,const vector<P>*pred=nullptr){
 for(int j=0;j<s.nz;j++){
  if(pred){s.z[j]=(*pred)[j];continue;}
  P to=s.ash;int best=dist2(s.z[j],to);
  for(int i=0;i<s.nh;i++){int d=dist2(s.z[j],s.h[i]);if(d<best)best=d,to=s.h[i];}
  s.z[j]=moveTo(s.z[j],to,400);
 }
 s.ash=moveTo(s.ash,target,1000);
 int n=0;for(int j=0;j<s.nz;j++)if(dist2(s.ash,s.z[j])>4000000)s.z[n++]=s.z[j];
 s.score+=10.*s.nh*s.nh*combo[s.nz-n];s.nz=n;
 n=0;for(int i=0;i<s.nh;i++){
  bool dead=false;for(int j=0;j<s.nz;j++)if(s.h[i]==s.z[j]){dead=true;break;}
  if(!dead)s.h[n++]=s.h[i];
 }s.nh=n;
}
void evaluate(Plan&p,const State&initial,const vector<P>&pred){
 State s=initial;
 for(int t=0;t<60&&s.nh&&s.nz;t++){
  P target;
  if(t<(int)p.g.size())target=policy(s,p.g[t]);
  else {target=rescue(s);p.g.push_back(target);}
  if(t==0)p.first=target;
  step(s,target,t==0?&pred:nullptr);
  if(!s.nz||!s.nh)p.g.resize(t+1);
 }
 p.score=s.nh?s.score+5.*s.nh*s.nh*s.nz:-1;
}
vector<Plan> population;
P choose(const State&s,const vector<P>&pred){
 auto start=chrono::steady_clock::now();
 vector<vector<P>> seeds;
 seeds.push_back({});
 for(int i=0;i<(int)population.size();i++){
  auto g=population[i].g;if(!g.empty())g.erase(g.begin());seeds.push_back(g);
 }
 vector<P> pool=pred;
 for(int i=0;i<s.nh;i++)pool.push_back(s.h[i]);
 for(int x:{0,4000,8000,12000,15999})for(int y:{0,4500,8999})pool.push_back({x,y});
 for(int m:{1,2,3,5,8}){pool.push_back({-1,m});seeds.push_back(vector<P>(60,{-1,m}));}
 for(int r:{2400,-2400,3000,-3000}){pool.push_back({-2,r});for(int n:{6,12,20,32})seeds.push_back(vector<P>(n,{-2,r}));}
 for(int i=0;i<s.nh;i++)for(int n:{6,12,20,40})seeds.push_back(vector<P>(n,s.h[i]));
 for(int i=0;i<s.nh;i++){
  P h=s.h[i];vector<P> edges={{h.x,0},{h.x,8999},{0,h.y},{15999,h.y}};
  sort(edges.begin(),edges.end(),[&](P a,P b){return dist2(a,h)<dist2(b,h);});
  for(int k=0;k<2;k++)for(int duration:{6,10,14}){
   vector<P> g(2,{-1,2});g.insert(g.end(),duration,h);g.insert(g.end(),7,edges[k]);seeds.push_back(g);
  }
 }
 for(P p:pred)seeds.push_back({p});
 for(int n:{3,6,10,16})seeds.push_back(vector<P>(n,s.ash));
 population.clear();
 int iterations=0,seed=0;double best=-2;P action=rescue(s);
 while(chrono::duration<double>(chrono::steady_clock::now()-start).count()<0.085){
  Plan p;int island=ri(8)*10;
  if(seed<(int)seeds.size()&&(population.empty()||ri(100)<25))p.g=seeds[seed++];
  else {
   int parent=ri(population.size()),base=(parent/10)*10;
   island=base;
   for(int k=0;k<2;k++){int a=base+ri(min(10,(int)population.size()-base));if(population[a].score>population[parent].score)parent=a;}
   p.g=population[parent].g;
   if(ri(100)<15){
    p.g.clear();for(int k=ri(4)+1;k;k--){P waypoint=ri(100)<60?s.h[ri(s.nh)]:pool[ri(pool.size())];p.g.insert(p.g.end(),ri(16)+1,waypoint);}
   }
   if(p.g.empty())p.g.push_back(pool[ri(pool.size())]);
   int j=ri(p.g.size()),op=ri(100);
   if(op<40){
    int end=min((int)p.g.size(),j+ri(15)+1);p.g.erase(p.g.begin()+j,p.g.begin()+end);
    p.g.insert(p.g.begin()+j,ri(15)+1,pool[ri(pool.size())]);
   }else if(op<60){int end=min((int)p.g.size(),j+ri(8)+1);p.g.erase(p.g.begin()+j,p.g.begin()+end);}
   else if(op<85){
    P v=p.g[j];if(v.x<0||ri(4)==0)v=pool[ri(pool.size())];else v=bounded({v.x+ri(4001)-2000,v.y+ri(4001)-2000});
    int end=min((int)p.g.size(),j+ri(6)+1);fill(p.g.begin()+j,p.g.begin()+end,v);
   }else{
    auto&other=population[ri(population.size())].g;
    if(j<(int)other.size()){p.g.resize(j);p.g.insert(p.g.end(),other.begin()+j,other.end());}
   }
   if(p.g.size()>60)p.g.resize(60);
  }
  evaluate(p,s,pred);iterations++;
  if(p.score>best)best=p.score,action=p.first;
  if(population.size()<80)population.push_back(move(p));
  else {
   int base=island,worst=base;
   for(int i=base+1;i<base+10;i++)if(population[i].score<population[worst].score)worst=i;
   if(p.score>=population[worst].score||(p.score>0&&ri(100)<4))population[worst]=move(p);
  }
 }
 cerr<<"search "<<iterations<<" value "<<fixed<<setprecision(0)<<best<<'\n';
 return action;
}
int main(){
 ios::sync_with_stdio(false);cin.tie(nullptr);
 double a=1,b=2;for(int i=1;i<=100;i++){combo[i]=combo[i-1]+a;double c=a+b;a=b;b=c;}
 while(true){
  State s;if(!(cin>>s.ash.x>>s.ash.y))break;cin>>s.nh;
  for(int i=0,id;i<s.nh;i++)cin>>id>>s.h[i].x>>s.h[i].y;
  cin>>s.nz;vector<P>pred(s.nz);for(int i=0,id;i<s.nz;i++)cin>>id>>s.z[i].x>>s.z[i].y>>pred[i].x>>pred[i].y;
  P action=choose(s,pred);cout<<action.x<<' '<<action.y<<endl;
 }
}
