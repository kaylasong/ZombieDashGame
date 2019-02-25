#include "Actor.h"
#include "StudentWorld.h"
#include <list>
#include <cmath>
using namespace std;

//implement:all the doSomethings, all the deploys, fix landmine destruction, probability of dumb zombie vomit


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
    Actor* temp=nullptr;
    std::list<Actor*>::iterator it=world->getActors();
    while(it!=world->getEnd()){
        if(this!=(*it)){
            if((*it)->isObs()){
                if(x<=(*it)->getX()+15 && x+15>=(*it)->getX()&&
                   y<=(*it)->getY()+15 && y+15>=(*it)->getY()){
                    temp=(*it);
                    break;
                }
            }
        }
        it++;
    }
    return(temp!=nullptr && temp->isObs());
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
double Actor::minDistance(double & minDist){
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
Penelope::~Penelope(){
    getWorld()->playSound(SOUND_PLAYER_DIE);
}
void Penelope::doSomething(){
    incrementIC();
    if(this->isDead()){
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
bool Citizen::move(double x, double y, int currDir, int toDir){
    if(!willHitObstacle(x,y)){
        if(toDir!=currDir)
            setDirection(toDir);
        else
            moveTo(x,y);
        return(true);
    }
    return(false);
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
            if(move(getX()+2,getY(),getDirection(),right))
                return;
        }
        else if(p_dir<=90 || p_dir<=135){
            if(move(getX(),getY()+2,getDirection(),up))
                return;
        }
        else if(p_dir<=180 || p_dir<=225){
            if(move(getX()-2,getY(),getDirection(),left))
                return;
        }
        else if(p_dir<=270 || p_dir<=315){
            if(move(getX(),getY()-2,getDirection(),down))
                return;
        }
        else
            cerr<<"you calculated the angles wrong."<<endl;
    }
    if(dist_z<80){
        if(z_dir<=45 || 360-z_dir<=45){
            if(move(getX()-2,getY(),getDirection(),left))
                return;
        }
        else if(z_dir<=90 || z_dir<=135){
            if(move(getX(),getY()-2,getDirection(),down))
                return;
        }
        else if(z_dir<=180 || z_dir<=225){
            if(move(getX()+2,getY(),getDirection(),right))
                return;
        }
        else if(z_dir<=270 || z_dir<=315){
            if(move(getX(),getY()-2,getDirection(),up))
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
}
bool Zombie::vomit(double x, double y, int dir){
    getWorld()->addItem(new Vomit(getWorld(),x, y,dir));
    getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
    return(true);
}
Zombie::~Zombie(){
    getWorld()->playSound(SOUND_ZOMBIE_DIE);
}
void Zombie::pickNewMovementPlan(){
    moveDis=randInt(3,10);
    int rand=randInt(0,3);
    switch(rand){
        case 0:
            setDirection(up);
            break;
        case 1:
            setDirection(down);
            break;
        case 2:
            setDirection(left);
            break;
        case 3:
            setDirection(right);
            break;
    }
}

DumbZombie::DumbZombie(StudentWorld* sw, int x, int y, bool holdsVacc)
:Zombie(sw,x,y){
    holdsVaccine=holdsVacc;
    m_canMove=false;
}
DumbZombie::~DumbZombie(){}
void DumbZombie::doSomething(){
    toggleCM();
    if(isDead())
        getWorld()->increaseScore(1000);
    if(!m_canMove)
        return;

    int dir=getDirection();
    Actor* other=nullptr;
    switch(dir){
        case right:
            other=objectOverlap(getX()+SPRITE_WIDTH,getY());
            if(other!=nullptr && other->canBeInf()){
                vomit(getX()+SPRITE_WIDTH,getY(), right);
                return;
            }
            break;
        case left:
            other=objectOverlap(getX()-SPRITE_WIDTH,getY());
            if(other!=nullptr && other->canBeInf()){
                vomit(getX()-SPRITE_WIDTH,getY(), left);
                return;
            }
            break;
        case down:
            other=objectOverlap(getX(),getY()-SPRITE_HEIGHT);
            if(other!=nullptr && other->canBeInf()){
                vomit(getX(),getY()-SPRITE_HEIGHT, down);
                return;
            }
            break;
        case up:
            other=objectOverlap(getX(),getY()+SPRITE_HEIGHT);
            if(other!=nullptr && other->canBeInf()){
                vomit(getX(),getY()+SPRITE_HEIGHT, up);
                return;
            }
            break;
    }
    if(getMD()==0){
        pickNewMovementPlan();
        dir=getDirection();
    }
    switch(dir){
        case right:
            if(!willHitObstacle(getX()+1,getY())){
                moveTo(getX()+1, getY());
                setMD(getMD()-1);
                return;
            }
            break;
        case left:
            if(!willHitObstacle(getX()-1,getY())){
                moveTo(getX()-1, getY());
                setMD(getMD()-1);
                return;
            }
            break;
        case down:
            if(!willHitObstacle(getX(),getY()-1)){
                moveTo(getX(), getY()-1);
                setMD(getMD()-1);
                return;
            }
            break;
        case up:
            if(!willHitObstacle(getX(),getY()+1)){
                moveTo(getX(), getY()+1);
                setMD(getMD()-1);
                return;
            }
            break;
    }
    setMD(0);
}

SmartZombie::SmartZombie(StudentWorld* sw, int x, int y)
:Zombie(sw,x,y){}
void SmartZombie::doSomething(){}

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
    getWorld()->playSound(SOUND_LANDMINE_EXPLODE);
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
        other->kill();
        getWorld()->addItem(new Flame(getWorld(),getX()-SPRITE_WIDTH,getY(),up,false));
        getWorld()->addItem(new Flame(getWorld(),getX()+SPRITE_WIDTH,getY(),up,false));
        getWorld()->addItem(new Flame(getWorld(),getX(),getY()-SPRITE_WIDTH,up,false));
        getWorld()->addItem(new Flame(getWorld(),getX(),getY()-SPRITE_WIDTH,up,false));
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
