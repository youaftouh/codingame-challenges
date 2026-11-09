@groovy.transform.CompileStatic
class Answer{static void main(String[]z){def s=new Scanner(System.in);int[]a=new int[8],t=new int[15];for(int i=0;i<8;)a[i++]=s.nextInt();for(int i=a[7];i-->0;){int f=s.nextInt();t[f]=s.nextInt()};t[a[3]]=a[4];while(true){int f=s.nextInt(),p=s.nextInt();def d=s.next();System.out.println(f>=0&&(t[f]-p)*(d<'R'?1:-1)>0?'BLOCK':'WAIT')}}}
