$|=1;while(<>){@a=split;($f,$p,$d)=@a;if(@a==8){$h{$a[3]}=$a[4]}elsif(@a==2){$h{$f}=$p}else{print(($f>=0&&($h{$f}-$p)*($d eq'LEFT'?1:-1)>0?'BLOCK':'WAIT')."\n")}}
