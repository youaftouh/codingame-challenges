package main

import (
 "bufio"
 "fmt"
 "math"
 "os"
 "time"
)

const horizon = 200
type Point struct { x,y float64 }
type State struct { x,y,vx,vy float64; fuel,angle,power int }
type Action struct { angle,power int }
type Plan [horizon]Action
var ground []Point
var left,right,height float64
var sine,cosine [181]float64
var rng uint64 = 82738129
func random(n int) int { rng ^= rng<<13; rng ^= rng>>7; rng ^= rng<<17; return int(rng%uint64(n)) }
func clamp(x,lo,hi float64) float64 { return math.Max(lo,math.Min(hi,x)) }
func abs(x int) int { if x<0 {return -x}; return x }
func advance(s State,a Action) State {
 a.angle=int(clamp(float64(a.angle),float64(s.angle-15),float64(s.angle+15)))
 a.power=int(clamp(float64(a.power),float64(s.power-1),float64(s.power+1)))
 if a.power>s.fuel {a.power=s.fuel}
 ax := -sine[a.angle+90]*float64(a.power)
 ay := cosine[a.angle+90]*float64(a.power)-3.711
 s.x+=s.vx+ax/2; s.y+=s.vy+ay/2
 s.vx+=ax; s.vy+=ay
 s.fuel-=a.power; s.angle=a.angle; s.power=a.power
 return s
}
func cross(a,b,c Point) float64 {return (b.x-a.x)*(c.y-a.y)-(b.y-a.y)*(c.x-a.x)}
func contact(a,b State) int {
 p,q:=Point{a.x,a.y},Point{b.x,b.y}
 for i:=0;i+1<len(ground);i++ {
  c,d:=ground[i],ground[i+1]
  if math.Max(p.x,q.x)<c.x || math.Min(p.x,q.x)>d.x {continue}
  if cross(p,q,c)*cross(p,q,d)<=0 && cross(c,d,p)*cross(c,d,q)<=0 {return i}
 }
 if b.x<0 || b.x>6999 || b.y<0 || b.y>3000 {return -2}
 return -1
}
func evaluate(s State,p *Plan) (float64,int) {
 for t,a:=range p {
  next:=advance(s,a); hit:=contact(s,next);s=next
  if hit != -1 {
   dx:=math.Max(left-s.x,math.Max(0,s.x-right))
   if hit>=0 && ground[hit].y==height && ground[hit+1].y==height && s.angle==0 && math.Abs(s.vx)<=20 && math.Abs(s.vy)<=40 {
    return 1e9+float64(s.fuel)*10000-float64(t),t+1
   }
   return 1e6-dx*100-math.Abs(s.y-height)*10-math.Max(0,math.Abs(s.vx)-18)*600-math.Max(0,-s.vy-38)*1500-float64(abs(s.angle))*250,t+1
  }
 }
 dx:=math.Max(left-s.x,math.Max(0,s.x-right))
 return -dx*100-math.Abs(s.y-height)*50-math.Abs(s.vx)*500-math.Abs(s.vy)*100,horizon
}
// Generate feedback trajectories to seed search with physically useful maneuvers.
func controller(s State,speed,margin,descent float64) Action {
 target:=(left+right)/2
 dx:=target-s.x
 desired:=clamp(dx*0.07,-speed,speed)
 if s.x>left+margin && s.x<right-margin {desired=clamp(dx*0.025,-12,12)}
 ax:=clamp((desired-s.vx)*0.16,-3.0,3.0)
 angle:=int(math.Round(-math.Asin(ax/4)*180/math.Pi))
 altitude:=s.y-height
 // Remaining vertical braking distance under full upright thrust.
 brake:=math.Max(0,(s.vy*s.vy-descent*descent)/(2*0.289))
 power:=4
 if altitude>brake+180 && math.Abs(float64(angle))<12 {power=0}
 if s.vy>descent && math.Abs(float64(angle))<15 {power=3}
 if altitude<250 && s.x>left+25 && s.x<right-25 {
  angle=int(clamp(s.vx*2,-30,30))
  if altitude<math.Max(70,-s.vy*3) && math.Abs(s.vx)<19 {angle=0}
 }
 if altitude<brake+50 && s.vy<descent {angle=int(clamp(float64(angle),-15,15));power=4}
 return Action{angle,power}
}
func seed(s State,speed,margin,descent float64) Plan {
 var p Plan
 for i:=range p {p[i]=controller(s,speed,margin,descent);s=advance(s,p[i])}
 return p
}
func main() {
 in:=bufio.NewReader(os.Stdin);out:=bufio.NewWriter(os.Stdout)
 var n int;if _,err:=fmt.Fscan(in,&n);err!=nil{return}
 ground=make([]Point,n)
 for i:=range ground {fmt.Fscan(in,&ground[i].x,&ground[i].y);if i>0 && ground[i].y==ground[i-1].y {left=ground[i-1].x;right=ground[i].x;height=ground[i].y}}
 for i:=0;i<=180;i++ {sine[i]=math.Sin(float64(i-90)*math.Pi/180);cosine[i]=math.Cos(float64(i-90)*math.Pi/180)}
 var best Plan;var predicted State;first:=true
 for {
  var input State
  if _,err:=fmt.Fscan(in,&input.x,&input.y,&input.vx,&input.vy,&input.fuel,&input.angle,&input.power);err!=nil{return}
  start:=time.Now();s:=input
  // Keep sub-meter physics precision when consistent with the rounded observation.
  if !first && math.Abs(predicted.x-input.x)<=0.55 && math.Abs(predicted.y-input.y)<=0.55 && math.Abs(predicted.vx-input.vx)<=0.55 && math.Abs(predicted.vy-input.vy)<=0.55 {s=predicted;s.fuel=input.fuel;s.angle=input.angle;s.power=input.power}
  if !first {copy(best[:],best[1:]);best[horizon-1]=Action{0,4}}
  score,length:=evaluate(s,&best)
  for _,speed:=range []float64{25,40,55,70,90} {for _,descent:=range []float64{-30,-36,-39} {
   p:=seed(s,speed,150,descent);value,l:=evaluate(s,&p)
   if first && speed==25 && descent == -30 || value>score {best=p;score=value;length=l}
  }}
  attempts:=0
  for time.Since(start)<78*time.Millisecond {
   p:=best
   for m:=0;m<1+random(3);m++ {
    at:=random(length);span:=1+random(15);if at+span>horizon {span=horizon-at}
    mode:=random(5);delta:=(random(7)-3)*5
    for i:=at;i<at+span;i++ {
     switch mode {
     case 0: p[i].power=random(5)
     case 1: p[i].angle=0
     default: p[i].angle=int(clamp(float64(p[i].angle+delta),-90,90))
     }
    }
   }
   value,l:=evaluate(s,&p);attempts++
   if value>score {best=p;score=value;length=l}
  }
  a:=best[0];predicted=advance(s,a);first=false
  fmt.Fprintf(os.Stderr,"search=%d fitness=%.0f horizon=%d ms=%d\n",attempts,score,length,time.Since(start).Milliseconds())
  fmt.Fprintln(out,a.angle,a.power);out.Flush()
 }
}
