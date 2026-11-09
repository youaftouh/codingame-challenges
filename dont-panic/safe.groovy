r=System.in.newReader();t=[:];while(l=r.readLine()){a=l.tokenize();f=a[0];p=a[1]as int;a[7]?(t[a[3]]=a[4]as int):a[2]?println(f!='-1'&&(t[f]-p)*(a[2]<'R'?1:-1)>0?'BLOCK':'WAIT'):t.putIfAbsent(f,p)}
