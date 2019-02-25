#include "Actor.h"
#include "StudentWorld.h"
#include <list>
#include <cmath>
using namespace std;

//fix: zombie movement plans, pointer list erasal error


// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
//class StudentWorld;
//// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
Actor::Actor(bool inf, bool des, bool obs, StudentWorld* sw, int id, double x, double y, int dir, int dep)
:GraphObject(id, x, y, dir, dep){
    canBeInfected=inf;
    canBeDestroyed=des;
    world=sw;
    m_isDead=false;
    isObstacle=obs;
    m_safe=false;
}
Actor* Actor::objectOverlap(double x, double y){
    std::list<Actor*>::iterator it=world->getActors();    
    while(it!=world->getEnd()){
        if(this!=(*it)){
            if((x-(*it)->getX())*(x-(*it)->getX())+
               (y-(*it)->getY())*(y-(*it)->getY())<=100)
                return(*it);
        }
        it++;
    }
    return(nullptr);
}
bool Actor::willHitObstacle(double x, double y){
    std::list<Actor*>::iterator it=world->getActors();
    while(it!=world->getEnd()){
        if(this!=(*it)){
            if((*it)->isObs()){
                if(x<=(*it)->getX()+15 && x+15>=(*it)->getX()&&
                   y<=(*it)->getY()+15 && y+15>=(*it)->getY())
                    return(true);
            }
        }
        it++;
    }
    return(false);
}
//sets dist equal to  the "this" and other
//returns the angle between them in degrees
double Actor::distanceBetween(Actor* other, double& dist){
    double x=other->getX()-this->getX();
    double y=other->getY()-this->getY();
    dist=sqrt(pow(x,2)+pow(y,2));
    double angle=atan2(y,x);
    angle*=180;
    angle/=3.14159265;
    if(angle<0){
        angle+=(2*180);
    }
    return(angle);
}


Wall::Wall(StudentWorld* sw, int x, int y)
:Actor(false,false,true,sw,IID_WALL,x,y,0,0){}
void Wall::doSomething(){}

Exit::Exit(StudentWorld* sw, int x, int y)
:Actor(false,false,false,sw,IID_EXIT,x,y,0,1){}
void Exit::doSomething(){
    Actor* other=this->objectOverlap(getX(),getY());
    if(other==nullptr)
        return;
    if(other->canBeInf()){
        if(getWorld()->getPenelope()!=other){
            this->getWorld()->increaseScore(500);
            getWorld()->playSound(SOUND_CITIZEN_SAVED);
            other->save();
        }
        else if(getWorld()->getNumCitizens()==0){
            this->getWorld()->setGameStatus(GWSTATUS_FINISHED_LEVEL);
            this->getWorld()->playSound(SOUND_LEVEL_FINISHED);
            this->getWorld()->nextLevel();
        }
    }
}

Damageable::Damageable(bool inf, bool obs, StudentWorld* sw, int id, double x, double y, int dir, int dep)
:Actor(inf, true, obs, sw, id, x, y, dir, dep)
{}
bool Damageable::move(int dir, int dist){
    switch(dir){
        case up:
            if(!willHitObstacle(getX(),getY()+dist)){
                moveTo(getX(),getY()+dist);
                return(true);
            }
            break;
        case right:
            if(!willHitObstacle(getX()+dist,getY())){
                moveTo(getX()+dist,getY());
                return(true);
            }
            break;
        case down:
            if(!willHitObstacle(getX(),getY()-dist)){
                moveTo(getX(),getY()-dist);
                return(true);
            }
            break;
        case left:
            if(!willHitObstacle(getX()-dist,getY())){
                moveTo(getX()-dist,getY());
                return(true);
            }
            break;
    }
    return(false);
}

Infectable::Infectable(StudentWorld* sw, int id, int x, int y, int dir, int dep)
:Damageable(true, true, sw, id, x, y, dir, dep){
    infectedCount=-1;
}
void Infectable::incrementIC(){
    if(infectedCount==500){
        if(this==getWorld()->getPenelope()){
            this->kill();
            return;
        }
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
:Infectable(sw,IID_PLAYER,x,y,0,0){
    numVaccines=0;
    numFlames=0;
    numLandmines=0;
}
Penelope::~Penelope(){}
void Penelope::doSomething(){
    incrementIC();
    if(this->isDead()){
        getWorld()->playSound(SOUND_PLAYER_DIE);
        this->getWorld()->decLives();
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
                else if(!willHitObstacle(getX()-4,getY()))
                    moveTo(getX()-4,getY());
                break;
            case KEY_PRESS_RIGHT:
                if(getDirection()!=right)
                    setDirection(right);
                else if(!willHitObstacle(getX()+4,getY()))
                    moveTo(getX()+4,getY());
                break;
            case KEY_PRESS_DOWN:
                if(getDirection()!=down)
                    setDirection(down);
                else if(!willHitObstacle(getX(),getY()-4))
                    moveTo(getX(),getY()-4);
                break;
            case KEY_PRESS_UP:
                if(getDirection()!=up)
                    setDirection(up);
                else if(!willHitObstacle(getX(),getY()+4))
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
    if(numFlames==0)
        return;
    getWorld()->playSound(SOUND_PLAYER_FIRE);
    switch(this->getDirection()){
        case up:
            for(int i=0;i<3;i++){
                Actor* other=objectOverlap(this->getX(),this->getY()+(i+1)*SPRITE_HEIGHT);
                if(other==nullptr || (other->canBeDes() && other->isObs()))
                    getWorld()->addItem(new Flame(getWorld(),this->getX(),this->getY()+(i+1)*SPRITE_HEIGHT, up,false));
            }
            break;
        case down:
            for(int i=0;i<3;i++){
                Actor* other=objectOverlap(this->getX(),this->getY()-(i+1)*SPRITE_HEIGHT);
                if(other==nullptr || (other->canBeDes() && other->isObs()))
                    getWorld()->addItem(new Flame(getWorld(),this->getX(),this->getY()-(i+1)*SPRITE_HEIGHT, down,false));
            }
            break;
        case right:
            for(int i=0;i<3;i++){
                Actor* other=objectOverlap(this->getX()+(i+1)*SPRITE_WIDTH,this->getY());
                if(other==nullptr || (other->canBeDes() && other->isObs()))
                    getWorld()->addItem(new Flame(getWorld(),this->getX()+(i+1)*SPRITE_WIDTH,this->getY(), right,false));
            }
            break;
        case left:
            for(int i=0;i<3;i++){
                Actor* other=objectOverlap(this->getX()-(i+1)*SPRITE_WIDTH,this->getY());
                if(other==nullptr || (other->canBeDes() && other->isObs()))
                    getWorld()->addItem(new Flame(getWorld(),this->getX()-(i+1)*SPRITE_WIDTH,this->getY(), left,false));
            }
                break;
    }
    numFlames--;
}
void Penelope::deployVaccine(){
    if(numVaccines==0 || getIC()==-1)
        return;
    getHealed();
    numVaccines--;
}
void Penelope::deployLandmine(){
    if(numLandmines==0)
        return;
    numLandmines--;
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
double Citizen::minDistance(double & minDist){
    double temp=minDist;
    //watch out for this one
    double tempAngle=0;
    double ansAngle=0;
    list<Actor*>::iterator it=getWorld()->getActors();
    while(it!=getWorld()->getEnd()){
        if((*it)->isObs()&&(*it)->canBeDes()&&!((*it)->canBeInf())){
            tempAngle=distanceBetween(*it,temp);
            if(temp<minDist){
                minDist=temp;
                ansAngle=tempAngle;
            }
        }
        it++;
    }
    return(ansAngle);
}
void Citizen::doSomething(){
    incrementIC();
    if(isDead())
        return;
    toggle();
    if(!getCM())
        return;
    double dist_p;
    int p_dir=distanceBetween(getWorld()->getPenelope(),dist_p);
    double dist_z=dist_p+1;
    double z_dir=minDistance(dist_z);
    if(dist_p<dist_z && dist_p<80){
        if(p_dir<=45 || 360-p_dir<=45){
            if(move(right,2))
                return;
        }
        else if(p_dir<=90 || p_dir<=135){
            if(move(up,2))
                return;
        }
        else if(p_dir<=180 || p_dir<=225){
            if(move(left,2))
                return;
        }
        else if(p_dir<=270 || p_dir<=315){
            if(move(down,2))
                return;
        }
        else
            cerr<<"you calculated the angles wrong."<<endl;
    }
    if(dist_z<80){
        if(z_dir<=45 || 360-z_dir<=45){
            if(move(left,2))
                return;
        }
        else if(z_dir<=90 || z_dir<=135){
            if(move(down,2))
                return;
        }
        else if(z_dir<=180 || z_dir<=225){
            if(move(right,2))
                return;
        }
        else if(z_dir<=270 || z_dir<=315){
            if(move(up,2))
                return;
        }
    }
}

///////////////
Goodie::Goodie(StudentWorld* sw, int id, int x, int y)
:Damageable(false,false,sw,id,x,y,0,1){}
VaccineGoodie::VaccineGoodie(StudentWorld* sw, int x, int y)
:Goodie(sw,IID_VACCINE_GOODIE,x,y){}
void VaccineGoodie::doSomething(){
    Actor*other=objectOverlap(getX(),getY());
    if(other==nullptr)
        return;
    if(other==getWorld()->getPenelope()){
        getWorld()->getPenelope()->addVaccines(1);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(50);
        kill();
    }
}
GasCanGoodie::GasCanGoodie(StudentWorld* sw, int x, int y)
:Goodie(sw,IID_GAS_CAN_GOODIE,x,y){}
void GasCanGoodie::doSomething(){
    Actor* other=objectOverlap(getX(),getY());
    if(other==nullptr)
        return;
    if(other==getWorld()->getPenelope()){
        getWorld()->getPenelope()->addFlames(5);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(50);
        kill();
    }
}
LandmineGoodie::LandmineGoodie(StudentWorld* sw, int x, int y)
:Goodie(sw,IID_LANDMINE_GOODIE,x,y){}
void LandmineGoodie::doSomething(){
    Actor*other=objectOverlap(getX(),getY());
    if(other==nullptr)
        return;
    if(other==getWorld()->getPenelope()){
        getWorld()->getPenelope()->addLandmines(2);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(50);
        kill();
    }
}

/////////////////
//Damageable::Damageable(bool inf, StudentWorld* sw, int id, double x, double y, int dir, int dep)
Zombie::Zombie(StudentWorld* sw, int x, int y)
:Damageable(false,true,sw,IID_ZOMBIE,x,y,0,0){
    moveDis=0;
    m_canMove=false;
}
void Zombie::doVomit(double x, double y, int dir){
    getWorld()->addItem(new Vomit(getWorld(),x, y,dir));
    getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
}
bool Zombie::vomit(){
    int dir=getDirection();
    Actor* other=nullptr;
    int chance=randInt(1,3);
    switch(dir){
        case right:
            other=objectOverlap(getX()+SPRITE_WIDTH,getY());
            if(other!=nullptr && other->canBeInf() && chance==1){
                doVomit(getX()+SPRITE_WIDTH,getY(), right);
                return(true);
            }
            break;
        case left:
            other=objectOverlap(getX()-SPRITE_WIDTH,getY());
            if(other!=nullptr && other->canBeInf() && chance==1){
                doVomit(getX()-SPRITE_WIDTH,getY(), left);
                return(true);
            }
            break;
        case down:
            other=objectOverlap(getX(),getY()-SPRITE_HEIGHT);
            if(other!=nullptr && other->canBeInf() && chance==1){
                doVomit(getX(),getY()-SPRITE_HEIGHT, down);
                return(true);
            }
            break;
        case up:
            other=objectOverlap(getX(),getY()+SPRITE_HEIGHT);
            if(other!=nullptr && other->canBeInf() && chance==1){
                doVomit(getX(),getY()+SPRITE_HEIGHT, up);
                return(true);
            }
            break;
    }
    return(false);
}
Zombie::~Zombie(){
    getWorld()->playSound(SOUND_ZOMBIE_DIE);
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
bool DumbZombie::willHitAnything(double x, double y){
    std::list<Actor*>::iterator it=getWorld()->getActors();
    while(it!=getWorld()->getEnd()){
        if(this!=(*it)){
            if(x<=(*it)->getX()+15 && x+15>=(*it)->getX()&&
               y<=(*it)->getY()+15 && y+15>=(*it)->getY()){
                return(true);
            }
        }
        it++;
    }
    return(false);
}
DumbZombie::~DumbZombie(){}
void DumbZombie::doSomething(){
    toggleCM();
    if(isDead()){
        getWorld()->increaseScore(1000);
        if(holdsVaccine){
            int dir=randInt(1,4);
            switch(dir){
                case 1:
                    if(willHitAnything(getX(),getY()+SPRITE_HEIGHT))
                        getWorld()->addItem(new VaccineGoodie(getWorld(),getX(),getY()+SPRITE_HEIGHT));
                    break;
                case 2:
                    if(willHitAnything(getX()+SPRITE_WIDTH,getY()))
                        getWorld()->addItem(new VaccineGoodie(getWorld(),getX()+SPRITE_WIDTH,getY()));
                    break;
                case 3:
                    if(willHitAnything(getX(),getY()-SPRITE_HEIGHT))
                        getWorld()->addItem(new VaccineGoodie(getWorld(),getX(),getY()-SPRITE_HEIGHT));
                    break;
                case 4:
                    if(willHitAnything(getX()-SPRITE_WIDTH,getY()))
                        getWorld()->addItem(new VaccineGoodie(getWorld(),getX()-SPRITE_WIDTH,getY()));
                    break;
            }
        }
    }
    if(!canMove())
        return;
    if(vomit())
        return;
    if(getMD()==0){
        pickNewMovementPlan();
    }
    if(!move(getDirection(),1))
        setMD(0);
}

SmartZombie::SmartZombie(StudentWorld* sw, int x, int y)
:Zombie(sw,x,y){}
double SmartZombie::closestTarget(double minDist){
    double temp=minDist;
    //watch out for this one
    double tempAngle=0;
    double ansAngle=0;
    list<Actor*>::iterator it=getWorld()->getActors();
    while(it!=getWorld()->getEnd()){
        if((*it)->canBeInf()){
            tempAngle=distanceBetween(*it,temp);
            if(temp<minDist){
                minDist=temp;
                ansAngle=tempAngle;
            }
        }
        it++;
    }
    return(ansAngle);
}
void SmartZombie::doSomething(){
    toggleCM();
    if(isDead() || !canMove())
        return;
    if(vomit())
        return;
    if(getMD()==0)
        pickNewMovementPlan();
    int dist=999999999;
    int dir=closestTarget(dist);
    if(dist>80){
        int randDir=randInt(1,4)*90;
        move(randDir,1);
    }
    else{
        setDirection(dir);
        if(dir<=45 || 360-dir<=45){
            if(move(right,1))
                return;
        }
        else if(dir<=90 || dir<=135){
            if(move(up,1))
                return;
        }
        else if(dir<=180 || dir<=225){
            if(move(left,1))
                return;
        }
        else if(dir<=270 || dir<=315){
            if(move(down,1))
                return;
        }
    }
    setMD(0);
}

///////////////////////////////////
//Actor::Actor(bool inf, bool des, StudentWorld* sw, int id, double x, double y, int dir, int dep)
Damaging::Damaging(bool inf, bool des, StudentWorld* sw, int id, int x, int y,int dir, int dep)
:Actor(inf,des,false, sw,id,x,y,dir,dep){}

Pit::Pit(StudentWorld* sw, int x, int y)
:Damaging(false,false,sw,IID_PIT,x,y,0,0){}
void Pit::doSomething(){
    Actor* other=objectOverlap(getX(),getY());
    if(other==nullptr)
        return;
    if(other->canBeDes()){
        other->kill();
    }
}

Landmine::Landmine(StudentWorld* sw, int x, int y)
:Damaging(false,false,sw,IID_LANDMINE,x,y,0,0){
    activationTime=0;
}
Landmine::~Landmine(){
}
void Landmine::incrementActivationTime(){
    activationTime++;
}
void Landmine::doSomething(){
    if(getActivationTime()<30){
        incrementActivationTime();
        return;
    }
    Actor* other=objectOverlap(getX(),getY());
    if(other==nullptr)
        return;
    if(other->canBeDes()){
        getWorld()->playSound(SOUND_LANDMINE_EXPLODE);
        other->kill();
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
    screenTime=2;
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
    Actor* other=objectOverlap(getX(),getY());
    if(other!=nullptr && other->canBeDes()){
        other->kill();
    }
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
    Actor* other=objectOverlap(getX(),getY());
    if(other!=nullptr&&other->canBeInf()){
        other->getInfected();
    }
    this->decST();
}
