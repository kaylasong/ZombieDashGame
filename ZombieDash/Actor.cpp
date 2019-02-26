#include "Actor.h"
#include "StudentWorld.h"
#include <list>
#include <cmath>
using namespace std;

//Flames and exits?

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
//class StudentWorld;
//// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
Actor::Actor(bool inf, bool des, bool deser, bool obs, StudentWorld* sw, int id, double x, double y, int dir, int dep)
:GraphObject(id, x, y, dir, dep){
    m_isDestroyer=deser;
    canBeInfected=inf;
    canBeDestroyed=des;
    world=sw;
    m_isDead=false;
    isObstacle=obs;
    m_safe=false;
}
//sets dist equal to  the "this" and other
//returns the angle between them in degrees


Wall::Wall(StudentWorld* sw, int x, int y)
:Actor(false,false,false,true,sw,IID_WALL,x,y,0,0){}
void Wall::doSomething(){}

Exit::Exit(StudentWorld* sw, int x, int y)
:Actor(false,false,false,false,sw,IID_EXIT,x,y,0,1){}
void Exit::doSomething(){
    getWorld()->manageExit(this);
}

Mover::Mover(bool inf, bool obs, StudentWorld* sw, int id, double x, double y, int dir, int dep)
:Actor(inf, true, false, obs, sw, id, x, y, dir, dep)
{}
bool Mover::move(int dir, int dist){
    switch(dir){
        case up:
            if(!getWorld()->obstacleThere(this,getX(),getY()+dist)){
                setDirection(up);
                moveTo(getX(),getY()+dist);
                return(true);
            }
            break;
        case right:
            if(!getWorld()->obstacleThere(this,getX()+dist,getY())){
                setDirection(right);
                moveTo(getX()+dist,getY());
                return(true);
            }
            break;
        case down:
            if(!getWorld()->obstacleThere(this,getX(),getY()-dist)){
                setDirection(down);
                moveTo(getX(),getY()-dist);
                return(true);
            }
            break;
        case left:
            if(!getWorld()->obstacleThere(this,getX()-dist,getY())){
                setDirection(left);
                moveTo(getX()-dist,getY());
                return(true);
            }
            break;
    }
    return(false);
}
bool Mover::smartMove(int dir, int dist){
    switch(dir){
        case 0:
        case 360:
            if(move(right,dist))
                return(true);
        case 90:
            if(move(up,dist))
                return(true);
        case 180:
            if(move(left,dist))
                return(true);
        case 270:
            if(move(down,dist))
                return(true);
    }
    int half=randInt(1,2);
    if(dir>0&&dir<90)
        return(chooseDir(half,right,up,dist));
    else if(dir>90 && dir<180)
        return(chooseDir(half,up,left,dist));
    else if(dir>180&&dir<270)
        return(chooseDir(half,left,down,dist));
    else if(dir>270)
        return(chooseDir(half,down,right,dist));
    return(false);
}
bool Mover::chooseDir(int half,int dir1, int dir2, int dist){
    if(half==1){
        if(!move(dir1,dist)){
            return(move(dir2,dist));
        }
        return(true);
    }
    else if(half==2){
        if(!move(dir2,dist)){
            return(move(dir1,dist));
        }
        return(true);
    }
    return(false);
}

Infectable::Infectable(StudentWorld* sw, int id, int x, int y, int dir, int dep)
:Mover(true, true, sw, id, x, y, dir, dep){
    infectedCount=-1;
}
void Infectable::incrementIC(){
    if(infectedCount==500){
        this->kill();
        getWorld()->playSound(SOUND_ZOMBIE_BORN);
        int chance=randInt(1,10);
        if(chance>3){
            int chance2=randInt(1,10);
            if(chance2==3)
                getWorld()->addItem(new DumbZombie(getWorld(),getX(),getY(),true));
            else
                getWorld()->addItem(new DumbZombie(getWorld(),getX(),getY(),false));
        }
        else{
            getWorld()->addItem(new SmartZombie(getWorld(),getX(),getY()));
        }
        
        return;
    }
    if(infectedCount!=-1)
        infectedCount++;
}
void Infectable::getInfected(){
    if(infectedCount==-1){
        infectedCount++;
    }
}

Penelope::Penelope(StudentWorld* sw, int x, int y)
:Infectable(sw,IID_PLAYER,x,y,0,0){}
Penelope::~Penelope(){
    if(isDead()){
        getWorld()->playSound(SOUND_PLAYER_DIE);
        this->getWorld()->decLives();
        this->getWorld()->setGameStatus(GWSTATUS_PLAYER_DIED);
    }
}
void Penelope::doSomething(){
    incrementIC();
    if(this->isDead()){
        getWorld()->playSound(SOUND_PLAYER_DIE);
        this->getWorld()->setGameStatus(GWSTATUS_PLAYER_DIED);
        return;
    }
    int move;
    if (getWorld()->getKey(move)){
        switch (move)
        {
            case KEY_PRESS_LEFT:
                if(getDirection()!=left)
                    setDirection(left);
                else if(!getWorld()->obstacleThere(this,getX()-4,getY()))
                    moveTo(getX()-4,getY());
                break;
            case KEY_PRESS_RIGHT:
                if(getDirection()!=right)
                    setDirection(right);
                else if(!getWorld()->obstacleThere(this,getX()+4,getY()))
                    moveTo(getX()+4,getY());
                break;
            case KEY_PRESS_DOWN:
                if(getDirection()!=down)
                    setDirection(down);
                else if(!getWorld()->obstacleThere(this,getX(),getY()-4))
                    moveTo(getX(),getY()-4);
                break;
            case KEY_PRESS_UP:
                if(getDirection()!=up)
                    setDirection(up);
                else if(!getWorld()->obstacleThere(this,getX(),getY()+4))
                    moveTo(getX(),getY()+4);
                break;
            case KEY_PRESS_SPACE:
                deployFlame();
                break;
            case KEY_PRESS_TAB:
                deployLandmine();
                break;
            case KEY_PRESS_ENTER:
                deployVaccine();
                break;
        }
    }
    
}
void Penelope::deployFlame(){
    if(getWorld()->getFlames()==0)
        return;
    getWorld()->decFlames();
    getWorld()->playSound(SOUND_PLAYER_FIRE);
    switch(this->getDirection()){
        case up:
            for(int i=0;i<3;i++){
                if(this->getY()+(i+1)*SPRITE_HEIGHT>=VIEW_HEIGHT)
                    break;
                if(!getWorld()->flameAllowed(this->getX(),this->getY()+(i+1)*SPRITE_HEIGHT))
                    break;
                else
                    getWorld()->addItem(new Flame(getWorld(),this->getX(),this->getY()+(i+1)*SPRITE_HEIGHT, up,false));
            }
            break;
        case down:
            for(int i=0;i<3;i++){
                if(this->getY()-(i+1)*SPRITE_HEIGHT<0)
                    break;
                if(!getWorld()->flameAllowed(this->getX(),this->getY()-(i+1)*SPRITE_HEIGHT))
                    break;
                else
                    getWorld()->addItem(new Flame(getWorld(),this->getX(),this->getY()-(i+1)*SPRITE_HEIGHT, down,false));
            }
            break;
        case right:
            for(int i=0;i<3;i++){
                if(this->getX()+(i+1)*SPRITE_WIDTH>=VIEW_WIDTH)
                    break;
                if(!getWorld()->flameAllowed(this->getX()+(i+1)*SPRITE_WIDTH,this->getY()))
                    break;
                else
                    getWorld()->addItem(new Flame(getWorld(),this->getX()+(i+1)*SPRITE_WIDTH,this->getY(), right,false));
            }
            break;
        case left:
            for(int i=0;i<3;i++){
                if(this->getX()-(i+1)*SPRITE_WIDTH<=0)
                    break;
                if(!getWorld()->flameAllowed(this->getX()-(i+1)*SPRITE_WIDTH,this->getY()))
                    break;
                else
                    getWorld()->addItem(new Flame(getWorld(),this->getX()-(i+1)*SPRITE_WIDTH,this->getY(), left,false));
            }
                break;
    }
}
void Penelope::deployVaccine(){
    if(getWorld()->getVaccines()==0 || getIC()==-1)
        return;
    getHealed();
}
void Penelope::deployLandmine(){
    if(getWorld()->getLandmines()==0)
        return;
    getWorld()->decLandmines();
    getWorld()->addItem(new Landmine(getWorld(),getX(),getY()));
}

Citizen::~Citizen(){
    if(isDead())
        getWorld()->increaseScore(-1000);
}
Citizen::Citizen(StudentWorld* sw, int x, int y)
:Infectable(sw,IID_CITIZEN,x,y,0,0){
    canMove=false;
}
void Citizen::doSomething(){
    incrementIC();
    toggle();
    if(isDead() || !getCM())
        return;
    double dist_p;
    int p_dir=getWorld()->distanceFromPenelope(this,dist_p);
    double dist_z=9999999;
    double z_dir=getWorld()->getMinDistance(this,dist_z);
    if(dist_p<dist_z && dist_p<80){
        if(smartMove(p_dir,2))
            return;
    }
    if(dist_z<80){
        if(z_dir>=180)
            z_dir-=180;
        else
            z_dir+=180;
        smartMove(z_dir,2);
    }
}

///////////////
Goodie::Goodie(StudentWorld* sw, int id, int x, int y)
:Actor(false,true,false,false,sw,id,x,y,0,1){}
VaccineGoodie::VaccineGoodie(StudentWorld* sw, int x, int y)
:Goodie(sw,IID_VACCINE_GOODIE,x,y){}
void VaccineGoodie::doSomething(){
    if(getWorld()->willOverlapWithPenelope(this,getX(), getY())){
        getWorld()->addVaccines(1);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(50);
        kill();
    }
}
GasCanGoodie::GasCanGoodie(StudentWorld* sw, int x, int y)
:Goodie(sw,IID_GAS_CAN_GOODIE,x,y){}
void GasCanGoodie::doSomething(){
    if(getWorld()->willOverlapWithPenelope(this,getX(), getY())){
        getWorld()->addFlames(5);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(50);
        kill();
    }
}
LandmineGoodie::LandmineGoodie(StudentWorld* sw, int x, int y)
:Goodie(sw,IID_LANDMINE_GOODIE,x,y){}
void LandmineGoodie::doSomething(){
    if(getWorld()->willOverlapWithPenelope(this,getX(), getY())){
        getWorld()->addLandmines(2);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(50);
        kill();
    }
}

/////////////////
//Damageable::Damageable(bool inf, StudentWorld* sw, int id, double x, double y, int dir, int dep)
Zombie::Zombie(StudentWorld* sw, int x, int y)
:Mover(false,true,sw,IID_ZOMBIE,x,y,0,0){
    moveDis=0;
    m_canMove=false;
}
void Zombie::doVomit(double x, double y, int dir){
    getWorld()->addItem(new Vomit(getWorld(),x, y,dir));
    getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
}
bool Zombie::vomit(){
    int dir=getDirection();
    int chance=randInt(1,3);
    switch(dir){
        case right:
            if(getWorld()->willOverlapWithInfectable(this,getX()+SPRITE_WIDTH,getY()) && chance==1){
                doVomit(getX()+SPRITE_WIDTH,getY(), right);
                return(true);
            }
            break;
        case left:
            if(getWorld()->willOverlapWithInfectable(this,getX()-SPRITE_WIDTH,getY()) && chance==1){
                doVomit(getX()-SPRITE_WIDTH,getY(), left);
                return(true);
            }
            break;
        case down:
            if(getWorld()->willOverlapWithInfectable(this,getX(),getY()-SPRITE_HEIGHT) && chance==1){
                doVomit(getX(),getY()-SPRITE_HEIGHT, down);
                return(true);
            }
            break;
        case up:
            if(getWorld()->willOverlapWithInfectable(this,getX(),getY()+SPRITE_HEIGHT) && chance==1){
                doVomit(getX(),getY()+SPRITE_HEIGHT, up);
                return(true);
            }
            break;
    }
    return(false);
}
void Zombie::pickNewMovementPlan(){
    moveDis=randInt(3,10);
    int rand=randInt(1,4)*90;
    setDirection(rand);
}

DumbZombie::DumbZombie(StudentWorld* sw, int x, int y, bool holdsVacc)
:Zombie(sw,x,y){
    holdsVaccine=holdsVacc;
}
DumbZombie::~DumbZombie(){
    if(isDead()){
        getWorld()->increaseScore(1000);
        getWorld()->playSound(SOUND_ZOMBIE_DIE);
        if(holdsVaccine){
            int dir=randInt(1,4);
            switch(dir){
                case 1://up
                    if(!getWorld()->willHitAnything(this,getX(),getY()+SPRITE_HEIGHT))
                        getWorld()->addItem(new VaccineGoodie(getWorld(),getX(),getY()+SPRITE_HEIGHT));
                    break;
                case 2://right
                    if(!getWorld()->willHitAnything(this,getX()+SPRITE_WIDTH,getY()))
                        getWorld()->addItem(new VaccineGoodie(getWorld(),getX()+SPRITE_WIDTH,getY()));
                    break;
                case 3://down
                    if(!getWorld()->willHitAnything(this,getX(),getY()-SPRITE_HEIGHT))
                        getWorld()->addItem(new VaccineGoodie(getWorld(),getX(),getY()-SPRITE_HEIGHT));
                    break;
                case 4://left
                    if(!getWorld()->willHitAnything(this, getX()-SPRITE_WIDTH,getY()))
                        getWorld()->addItem(new VaccineGoodie(getWorld(),getX()-SPRITE_WIDTH,getY()));
                    break;
            }
        }
    }
}
void DumbZombie::doSomething(){
    toggleCM();
    if(isDead() || !canMove() || vomit())
        return;
    if(getMD()==0){
        pickNewMovementPlan();
    }
    if(!move(getDirection(),1))
        setMD(0);
    else
        setMD(getMD()-1);
}

SmartZombie::SmartZombie(StudentWorld* sw, int x, int y)
:Zombie(sw,x,y){}
SmartZombie::~SmartZombie(){
    if(isDead()){
        getWorld()->increaseScore(2000);
        getWorld()->playSound(SOUND_ZOMBIE_DIE);
    }
}
void SmartZombie::doSomething(){
    toggleCM();
    if(isDead() || !canMove() || vomit())
        return;
    if(getMD()==0){
        pickNewMovementPlan();
        double dist=999999999;
        int dir=getWorld()->closestTarget(this,dist);
        if(dist>80){
            int randDir=randInt(1,4)*90;
            if(move(randDir,1)){
                setMD(getMD()-1);
                return;
            }
        }
        else{
            if(smartMove(dir,1)){
                setMD(getMD()-1);
                return;
            }
        }
    }
    else
        if(move(getDirection(),1)){
            setMD(getMD()-1);
            return;
        }
    setMD(0);
}

///////////////////////////////////
//Actor::Actor(bool inf, bool des, StudentWorld* sw, int id, double x, double y, int dir, int dep)
Damaging::Damaging(bool inf, bool des, StudentWorld* sw, int id, int x, int y,int dir, int dep)
:Actor(inf,des,true,false, sw,id,x,y,dir,dep){}

Pit::Pit(StudentWorld* sw, int x, int y)
:Damaging(false,false,sw,IID_PIT,x,y,0,0){}
void Pit::doSomething(){
    getWorld()->killOverlapable(this,getX(),getY());
}

Landmine::Landmine(StudentWorld* sw, int x, int y)
:Damaging(false,false,sw,IID_LANDMINE,x,y,0,0){
    activationTime=0;
}
void Landmine::incrementActivationTime(){
    activationTime++;
}
void Landmine::doSomething(){
    if(getActivationTime()<30){
        incrementActivationTime();
        return;
    }
    if(!getWorld()->willOverlap(this,getX(),getY()))
        return;
    else{
        getWorld()->killOverlapable(this,getX(),getY());
        getWorld()->playSound(SOUND_LANDMINE_EXPLODE);
        getWorld()->addItem(new Flame(getWorld(),getX()-SPRITE_WIDTH,getY(),up,false));
        getWorld()->addItem(new Flame(getWorld(),getX()+SPRITE_WIDTH,getY(),up,false));
        getWorld()->addItem(new Flame(getWorld(),getX(),getY()-SPRITE_WIDTH,up,false));
        getWorld()->addItem(new Flame(getWorld(),getX(),getY()+SPRITE_WIDTH,up,false));
        getWorld()->addItem(new Flame(getWorld(),getX()-SPRITE_WIDTH,getY()+SPRITE_WIDTH,up,false));
        getWorld()->addItem(new Flame(getWorld(),getX()-SPRITE_WIDTH,getY()-SPRITE_WIDTH,up,false));
        getWorld()->addItem(new Flame(getWorld(),getX()+SPRITE_WIDTH,getY()+SPRITE_WIDTH,up,false));
        getWorld()->addItem(new Flame(getWorld(),getX()+SPRITE_WIDTH,getY()-SPRITE_WIDTH,up,false));
        getWorld()->addItem(new Flame(getWorld(),getX(),getY(),up, true));
        kill();
    }
}

Projectile::Projectile(StudentWorld* sw, int id, int x, int y, int dir)
:Damaging(false,false,sw,id,x,y,dir,0){
    screenTime=3;
}
void Projectile::decST(){
    if(screenTime==0){
        this->kill();
        return;
    }
    screenTime--;
}

Flame::Flame(StudentWorld* sw, int x, int y, int dir, bool ilf)
:Projectile(sw,IID_FLAME,x,y,dir){
    isLandmineFlame=ilf;
}
Flame::~Flame(){
    if(isLandmineFlame)
        getWorld()->addItem(new Pit(getWorld(),getX(),getY()));
}
void Flame::doSomething(){
    if(isDead())
        return;
    getWorld()->killOverlapable(this,getX(),getY());
    this->decST();
}

Vomit::Vomit(StudentWorld* sw, int x, int y, int dir)
:Projectile(sw,IID_VOMIT,x,y,dir){}
void Vomit::infect(Infectable* target){
    target->getInfected();
}
void Vomit::doSomething(){
    if(isDead())
        return;
    getWorld()->infectOverlapper(this,getX(),getY());
    this->decST();
}
