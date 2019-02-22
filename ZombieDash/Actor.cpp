#include "Actor.h"
#include "StudentWorld.h"
#include <list>
using namespace std;

//implement:all the doSomethings, all the deploys


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
    boundingBox=10;
}

Actor* Actor::objectOverlap(double x, double y){
    std::list<Actor*>::iterator it=world->getActors();    
    while(it!=world->getEnd()){
        if(this!=(*it)){
            if(this->boundingBox!=16 && (*it)->boundingBox==16){
                if(x<=(*it)->getX()+15 && x+15>=(*it)->getX()&&
                   y<=(*it)->getY()+15 && y+15>=(*it)->getY())
                    return(*it);
            }
            else if(this->boundingBox==10 && (*it)->boundingBox==10)
                if((x-(*it)->getX())*(x-(*it)->getX())+
                   (y-(*it)->getY())*(y-(*it)->getY())<=
                   (this->boundingBox)*(this->boundingBox))
                    return(*it);
        }
        it++;
    }
    return(nullptr);
}
bool Actor::willHitObstacle(double x, double y){
    Actor* temp=this->objectOverlap(x,y);
    return(temp!=nullptr && temp->isObs());
}

Wall::Wall(StudentWorld* sw, int x, int y)
:Actor(false,false,true,sw,IID_WALL,x,y,0,0){
    setBoundingBox(16);
}
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
            other->kill();
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
        this->kill();
        return;
    }
    if(infectedCount!=-1)
        infectedCount++;
}
void Infectable::getInfected(){
    if(infectedCount==-1)
        infectedCount++;
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
                if(other==nullptr || other->getBoundingBox()!=16)
                    getWorld()->addItem(new Flame(getWorld(),this->getX(),this->getY()+(i+1)*SPRITE_HEIGHT, up));
            }
            break;
        case down:
            for(int i=0;i<3;i++){
                Actor* other=objectOverlap(this->getX(),this->getY()-(i+1)*SPRITE_HEIGHT);
                if(other==nullptr || other->getBoundingBox()!=16)
                    getWorld()->addItem(new Flame(getWorld(),this->getX(),this->getY()-(i+1)*SPRITE_HEIGHT, down));
            }
            break;
        case right:
            for(int i=0;i<3;i++){
                Actor* other=objectOverlap(this->getX()+(i+1)*SPRITE_WIDTH,this->getY());
                if(other==nullptr || other->getBoundingBox()!=16)
                    getWorld()->addItem(new Flame(getWorld(),this->getX()+(i+1)*SPRITE_WIDTH,this->getY(), right));
            }
            break;
        case left:
            for(int i=0;i<3;i++){
                Actor* other=objectOverlap(this->getX()-(i+1)*SPRITE_WIDTH,this->getY());
                if(other==nullptr || other->getBoundingBox()!=16)
                    getWorld()->addItem(new Flame(getWorld(),this->getX()-(i+1)*SPRITE_WIDTH,this->getY(), left));
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
//deploy functions
Citizen::Citizen(StudentWorld* sw, int x, int y)
:Infectable(sw,IID_CITIZEN,x,y,0,0){}
void Citizen::doSomething(){}

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
:Damageable(false,true,sw,IID_ZOMBIE,x,y,0,0){}
void Zombie::vomit(Infectable* target){
    target->getInfected();
    getWorld()->playSound(SOUND_ZOMBIE_BORN);
}
Zombie::~Zombie(){
    getWorld()->playSound(SOUND_ZOMBIE_DIE);
}

DumbZombie::DumbZombie(StudentWorld* sw, int x, int y, bool holdsVacc)
:Zombie(sw,x,y){
    holdsVaccine=holdsVacc;
}
void DumbZombie::doSomething(){}

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
:Damaging(false,false,sw,IID_LANDMINE,x,y,0,0){}
Landmine::~Landmine(){
    getWorld()->playSound(SOUND_LANDMINE_EXPLODE);
}
void Landmine::doSomething(){
    Actor* other=objectOverlap(getX(),getY());
    if(other==nullptr)
        return;
    if(other->canBeDes()){
        other->kill();
        getWorld()->addItem(new Pit(getWorld(),getX(),getY()));
        getWorld()->addItem(new Flame(getWorld(),getX()-SPRITE_WIDTH,getY(),left));
        getWorld()->addItem(new Flame(getWorld(),getX()+2*SPRITE_WIDTH,getY(),right));
        getWorld()->addItem(new Flame(getWorld(),getX(),getY()-SPRITE_WIDTH,up));
        getWorld()->addItem(new Flame(getWorld(),getX(),getY()-2*SPRITE_WIDTH,down));
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

Flame::Flame(StudentWorld* sw, int x, int y, int dir)
:Projectile(sw,IID_FLAME,x,y,dir){}
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
    this->decST();
}
