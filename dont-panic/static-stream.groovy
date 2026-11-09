@groovy.transform.CompileStatic
class Answer{static void main(String[]z){Map<String,Integer>t=[:];System.in.eachLine{String l->def a=l.tokenize(),f=a[0];int p=a[1]as int;a[7]?(t[a[3]]=a[4]as int):a[2]?System.out.println(f!='-1'&&(t[f]-p)*(a[2]<'R'?1:-1)>0?'BLOCK':'WAIT'):t.putIfAbsent(f,p)}}}
