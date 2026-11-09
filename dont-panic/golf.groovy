i=new Scanner(System.in)
r={i.nextInt()}
q=[];8.times{q<<r()};t=[(q[3]):q[4]]
q[7].times{t<<[(r()):r()]}
for(;;){f=r();p=r();d=i.next();println f>=0&&(t[f]-p)*(d<'R'?1:-1)>0?'BLOCK':'WAIT'}
