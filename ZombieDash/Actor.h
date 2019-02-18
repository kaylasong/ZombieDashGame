#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <iostream>


class StudentWorld;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor: public GraphObject{
public:
    Actor(bool inf, bool des, StudentWorld* sw,
          std:: string id, int x, int y, int dir, int dep);
    virtual void doSomething()=0;
    //accessors
    bool canBeInf();
    bool canBeDes();
    
private:
    int boundingBox;
    bool canBeInfected;
    bool canBeDestroyed;
    bool isDead;
    StudentWorld* world;
};

//////////////////////////////////

class Damageable: public Actor{
public:
    Damageable();
};

//////////////
class Infectable: public Damageable{
public:
    Infectable();
    void incrementIC();
private:
    int infectedCount;
};

class Penelope: public Infectable{
public:
    Penelope();
    virtual void doSomething();
    //all the goodie thingies
    void setVaccines(int x);
    void setFlames(int x);
    void setLandmines(int x);
    bool deployVaccine();
    bool deployFlame();
    bool deployLandmine();
private:
    int numVaccines;
    int numFlames;
    int numLandmines;
};

class Citizen: public Infectable{
public:
    Citizen();
    virtual void doSomething();
};
///////////////
class Goodie: public Damageable{
public:
    Goodie();
    virtual void doSomething();
private:
    Penelope* p;
};

class VaccineGoodie: public Goodie{
public:
    VaccineGoodie();
    virtual void doSomething();
};

class GasCanGoodie: public Goodie{
public:
    GasCanGoodie();
    virtual void doSomething();
};
class LandmineGoodie: public Goodie{
public:
    LandmineGoodie();
    virtual void doSomething();
};

///////////////
class Zombie: public Damageable{
public:
    Zombie();
    void vomit(Infectable* target);
};

class DumbZombie: public Zombie{
public:
    DumbZombie();
    virtual void doSomething();
private:
    VaccineGoodie* vacc;
};

class SmartZombie: public Zombie{
public:
    virtual void doSomething();
};
/////////////////////////////////

class Damaging: public Actor{
public:
    void damage();
};

class Pit: public Damaging{
public:
    virtual void doSomething();
};

class Landmine: public Damaging{
public:
    virtual void doSomething();
};

class Projectile: public Damaging{
public:
    void decST();
private:
    int screenTime;
};

class Flame: public Projectile{
public:
    virtual void doSomething();
};

class Vomit: public Projectile{
public:
    virtual void doSomething();
};


#endif // ACTOR_H_
