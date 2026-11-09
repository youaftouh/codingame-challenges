t={}
while 1:
 a=input().split();f,p=map(int,a[:2])
 if len(a)==8:t[int(a[3])]=int(a[4])
 elif len(a)==2:t[f]=p
 else:print('BLOCK'if f>=0 and(t[f]-p)*(1 if a[2]=='LEFT'else-1)>0 else'WAIT')
