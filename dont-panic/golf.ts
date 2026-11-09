let t=[],a,f,p,d;for(;;){a=readline().split(' ');[f,p,d]=a; a[7]?t[a[3]]=+a[4]:d?console.log(f>=0&&(t[f]-p)*(d<'R'?1:-1)>0?'BLOCK':'WAIT'):t[f]=+p}
