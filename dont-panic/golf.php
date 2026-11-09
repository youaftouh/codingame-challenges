<?php
while($a=explode(' ',trim(fgets(STDIN)))){[$f,$p]=$a;count($a)>3?$t[$a[3]]=$a[4]:(count($a)<3?$t[$f]=$p:print(($f>=0&&($t[$f]-$p)*($a[2]<'R'?1:-1)>0?'BLOCK':'WAIT')."\n"));}
