i=new Scanner(System.in)
r={i.nextInt()}
q=[];f=[:]
8.times{q+=r()}
q[7].times{f<<[(r()):r()]}
for(;;){p=r();c=r();d=i.next()
t=p<0?c:p==q[3]?q[4]:f[p]
println d<'R'&c<t|d>'Q'&t<c?"BLOCK":"WAIT"}
