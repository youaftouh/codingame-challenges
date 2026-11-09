@groovy.transform.CompileStatic
class Answer {
 static void main(String[] args) {
  Scanner s=new Scanner(System.in)
  int[] a=new int[8]
  int[] target=new int[15]
  for(int j=0;j<8;j++) a[j]=s.nextInt()
  for(int j=0;j<a[7];j++) {
   int floor=s.nextInt()
   target[floor]=s.nextInt()
  }
  target[a[3]]=a[4]
  while(s.hasNextInt()) {
   int floor=s.nextInt(),position=s.nextInt()
   String direction=s.next()
   boolean block=floor>=0 && ((direction.equals('LEFT') && position<target[floor]) || (direction.equals('RIGHT') && position>target[floor]))
   System.out.println(block?'BLOCK':'WAIT')
   System.out.flush()
  }
 }
}
