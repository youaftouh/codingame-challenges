read a b c f p g h n
t[f]=$p
for((;n--;)){ read f p;t[f]=$p;}
while read f p d;do
((f>=0&&(t[f]-p)*(9-2*${#d})>0))&&echo BLOCK||echo WAIT
done
