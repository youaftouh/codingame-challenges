input = new Scanner(System.in);
N = input.nextInt() // the number of points used to draw the surface of Mars.
for (i = 0; i < N; ++i) {
    input.nextInt()
    input.nextInt()
}
while (true) {
    fuel = 0
    X = input.nextInt()
    Y = input.nextInt()
    HS = input.nextInt()
    VS = input.nextInt()
    F = input.nextInt()
    R = input.nextInt()
    P = input.nextInt()
    if(VS.abs()>=40) println "0 4"
    else println "0 0"
}