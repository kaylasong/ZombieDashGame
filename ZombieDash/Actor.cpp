#include "Actor.h"
#include "StudentWorld.h"
#include <list>

//implement: Actor::haveCollided, all the doSomethings, all the deploys


// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
//class StudentWorld;
//// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
Actor::Actor(bool inf, bool des, StudentWorld* sw, int id, double x, double y, int dir, int dep)
:GraphObject(id, x, y, dir, dep){
    canBeInfected=inf;
    canBeDestroyed=des;
    world=sw;
    m_isDead=false;
    boundingBox=100;
}
bool Actor::haveCollided(Actor* a, Actor* b){
    if((a->getX()-b->getX())*(a->getX()-b->getX())+(a->getY()-b->getY())*(a->getY()-b->getY())<=
       (a->boundingBox))
        return(true);
    return(false);
}
Actor* Actor::nearSomething(){
    std::list<Actor*>::iterator it=world->getActors();
    while(it!=world->getEnd()){
        if((this->getX()!=(*it)->getX() && this->getY()!=(*it)->getY()))
           if(haveCollided(this,(*it)))
               return(*it);
        it++;
    }
    return(nullptr);
}
//implement haveCollided

Wall::Wall(StudentWorld* sw, int x, int y)
:Actor(false,false,sw,11,x,y,0,0){
    setBoundingBox(256);
}
void Wall::doSomething(){
    Actor* other=this->nearSomething();
    if(other==nullptr)
        return;
    
}

Exit::Exit(StudentWorld* sw, int x, int y)
:Actor(false,false,sw,10,x,y,0,1){}
void Exit::doSomething(){
    Actor* other=this->nearSomething();
    if(other==nullptr)
        return;
    if(other->canBeInf()){
        if(!getWorld()->isPenelope(other)){
            //let them in
        }
        else if(getWorld()->getNumCitizens()==0){
            //let penelope in
        }
    }
}



Damageable::Damageable(bool inf, StudentWorld* sw, int id, double x, double y, int dir, int dep)
:Actor(inf, true, sw, id, x, y, dir, dep)
{}

Infectable::Infectable(StudentWorld* sw, int id, int x, int y, int dir, int dep)
:Damageable(true, sw, id, x, y, dir, dep){
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
:Infectable(sw,0,x,y,0,0){
    numVaccines,numFlames,numLandmines=0;
}
void Penelope::doSomething(){
    if(this->isDead())
        return;
}
//deploy functions
Citizen::Citizen(StudentWorld* sw, int x, int y)
:Infectable(sw,2,x,y,0,0){}
void Citizen::doSomething(){}

///////////////
Goodie::Goodie(StudentWorld* sw, int id, int x, int y)
:Damageable(false,sw,id,x,y,0,1){}
VaccineGoodie::VaccineGoodie(StudentWorld* sw, int x, int y)
:Goodie(sw,7,x,y){}
void VaccineGoodie::doSomething(){}
GasCanGoodie::GasCanGoodie(StudentWorld* sw, int x, int y)
:Goodie(sw,8,x,y){}
void GasCanGoodie::doSomething(){}
LandmineGoodie::LandmineGoodie(StudentWorld* sw, int x, int y)
:Goodie(sw,9,x,y){}
void LandmineGoodie::doSomething(){}

/////////////////
//Damageable::Damageable(bool inf, StudentWorld* sw, int id, double x, double y, int dir, int dep)
Zombie::Zombie(StudentWorld* sw, int x, int y)
:Damageable(false,sw,1,x,y,0,0){}
void Zombie::vomit(Infectable* target){
    target->getInfected();
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
:Actor(inf,des,sw,id,x,y,dir,dep){}
void Damaging::damage(Damageable* target){
    target->kill();
}

Pit::Pit(StudentWorld* sw, int x, int y)
:Damaging(false,false,sw,5,x,y,0,0){}
void Pit::doSomething(){}

Landmine::Landmine(StudentWorld* sw, int x, int y)
:Damaging(false,false,sw,6,x,y,0,0){}
void Landmine::doSomething(){}

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
:Projectile(sw,3,x,y,dir){}
void Flame::doSomething(){
    this->decST();
}

Vomit::Vomit(StudentWorld* sw, int x, int y, int dir)
:Projectile(sw,4,x,y,dir){}
void Vomit::doSomething(){
    this->decST();
}
