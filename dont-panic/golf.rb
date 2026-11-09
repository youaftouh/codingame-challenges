$stdout.sync=true
h={}
while l=gets
a=l.split
f,p=a.map(&:to_i)
case a.size
when 8;h[a[3].to_i]=a[4].to_i
when 2;h[f]=p
else;puts f>=0&&(h[f]-p)*(a[2]=='LEFT'?1:-1)>0?'BLOCK':'WAIT'
end
end
