/** Game START **/
//improvements
//$ zombie next position!
//$ use ranges (don't go until human's exact position)
//imports
import java.awt.geom.Point2D;
//Constants
WIDTH = 16000
HEIGHT = 9000
ZOMBIE_STEP = 400
ASH_STEP = 1000
ASH_RANGE = 2000
//Objects
class Human{
    int id;
    double distance = 20000;//max (corner to corner) is about 18357,55975068582
    double distanceNext = 20000;//max (corner to corner) is about 18357,55975068582
    Point2D.Double position;
    Point2D.Double nearestZombie;
    Human(int id){
        this.id = id;
        this.position = new Point2D.Double();
    }
}
class Zombie{
    int id;
    Point2D.Double position;
    Point2D.Double nextPosition;
    Zombie(int id){
        this.id = id;
        this.position = new Point2D.Double();
        this.nextPosition = new Point2D.Double();
    }
}
humans = []
zombies = []
me = new Point2D.Double()
input = new Scanner(System.in);
// game loop
while (true) {
    me.x = input.nextInt()
    me.y = input.nextInt()
    NbH = input.nextInt()
    remainingHumans = []
    remainingZombies = []
    //humansAbleToRescue =[]
    NbH.times{
        human = getHuman(input.nextInt())
        human.position.x = input.nextInt()
        human.position.y = input.nextInt()
        remainingHumans << human
    }
    NbZ = input.nextInt()
    NbZ.times{
        zombie = getZombie(input.nextInt())
        zombie.position.x = input.nextInt()
        zombie.position.y = input.nextInt()
        zombie.nextPosition.x = input.nextInt()
        zombie.nextPosition.y = input.nextInt()
        remainingZombies << zombie
        remainingHumans.each{
            //$ I recommand to check next position rather than actual position
            humanToZombieDist = it.position.distance(zombie.position)
            if(it.distance>humanToZombieDist){
                it.distance = humanToZombieDist
            }
            humanToZombieDistNext = it.position.distance(zombie.nextPosition)
            if(it.distanceNext>humanToZombieDistNext){
                it.distanceNext = humanToZombieDistNext
                it.nearestZombie = zombie.nextPosition
            }
        }
    }
    humansInDanger = remainingHumans.sort{it.distanceNext}

    //check if able to rescue
    target = humansInDanger[0]
    System.err.println "target "+target.id
    for(Human ningen : humansInDanger){
        if(canRescue(ningen)){
            //humansAbleToRescue << ningen
            target = ningen
            break
        }
    }
    //update Humans
    humans = remainingHumans
    zombies = remainingZombies
    humans.each{it.distance = it.distanceNext = 20000}

    //Calculate position
    //check if isRightAngle
    /*if(humansAbleToRescue.size()>1 && remainingZombies.size()>1){
        z1 = humansAbleToRescue[0].nearestZombie
        z2 = humansAbleToRescue[1].nearestZombie
        if(ableToTurn(z1, z2)){
            alpha = Math.atan((z2.y-z1.y)/(z2.x-z1.x))
            deltaX = z2.x>z1.x?ASH_STEP*Math.cos(alpha):(-ASH_STEP*Math.cos(alpha))
            deltaY = z2.y>z1.y?(-ASH_STEP*Math.sin(alpha)):ASH_STEP*Math.sin(alpha)
            xPos = deltaX + z1.x
            yPos = deltaY + z1.y
            System.err.println "xPos "+xPos +", yPos"+yPos
        }
    }*/

    x = (int) target.nearestZombie.x // - ASH_STEP
    y = (int) target.nearestZombie.y // - ASH_STEP
    println x+" "+y
}

//Function Helpers
Human getHuman(identifier){
    //System.err.println "finding...#"+identifier
    Human human = humans.find{it.id == identifier}
    //System.err.println "found "+human
    if(human == null){
        human = new Human(identifier)
        humans << human
    }
    return human
}
Zombie getZombie(identifier){
    Zombie zombie = zombies.find{it.id == identifier}
    if(zombie == null){
        zombie = new Zombie(identifier)
        zombies << zombie
    }
    return zombie
}
boolean canRescue(victim){
    mySteps = (me.distance(victim.position)-ASH_RANGE)/ASH_STEP
    zSteps = victim.distance/ZOMBIE_STEP
    System.err.println mySteps + " " + zSteps
    //System.err.println (mySteps <= zSteps)
    return mySteps <= zSteps
}
/*boolean ableToTurn(z1, z2){
    meToz1 = me.distance(z1)
    meToz1S = meToz1**2
    meToz2 = me.distance(z2)
    meToz2S = meToz2**2
    z1Toz2 = z1.distance(z2)
    z1Toz2S = z1Toz2**2
    System.err.println Math.acos((meToz1S+meToz2S-z1Toz2S)/(2*meToz1*meToz2))
    meToz1>0 && meToz2>0 && z1Toz2>0 && 
    Math.acos((meToz1S+meToz2S-z1Toz2S)/(2*meToz1*meToz2)) < Math.PI
}*/
/** Game END **/
