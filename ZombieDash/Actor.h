#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <iostream>


class StudentWorld;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor: public GraphObject{
public:
    Actor(bool inf, bool des, StudentWorld* sw, int id, double x, double y, int dir, int dep);
    virtual void doSomething()=0;
    bool haveCollided(Actor* a, Actor* b);
    Actor* nearSomething();
    //accessors
    void kill(){m_isDead=true;}
    bool canBeInf(){return(canBeInfected);}
    bool canBeDes(){return(canBeDestroyed);}
    bool isDead(){return(m_isDead);}
    void setBoundingBox(int x){boundingBox=x;}
    StudentWorld* getWorld(){return(world);}
private:
    int boundingBox;
    bool canBeInfected;
    bool canBeDestroyed;
    bool m_isDead;
    StudentWorld* world;
};

//////////////////////////////////
class Wall: public Actor{
public:
    Wall(StudentWorld* sw, int x, int y);
    void doSomething();
};

class Exit: public Actor{
public:
    Exit(StudentWorld* sw, int x, int y);
    void doSomething();
};

//////////////////////////////////

class Damageable: public Actor{
public:
    Damageable(bool inf, StudentWorld* sw, int id, double x, double y, int dir, int dep);
};

//////////////
class Infectable: public Damageable{
public:
    Infectable(StudentWorld* sw, int id, int x, int y, int dir, int dep);
    void incrementIC();
    void getInfected();
private:
    int infectedCount;
};

class Penelope: public Infectable{
public:
    Penelope(StudentWorld* sw, int x, int y);
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
    Citizen(StudentWorld* sw, int x, int y);
    virtual void doSomething();
};
///////////////
class Goodie: public Damageable{
public:
    Goodie(StudentWorld* sw, int id, int x, int y);
    void editPenelope();
};

class VaccineGoodie: public Goodie{
public:
    VaccineGoodie(StudentWorld* sw, int x, int y);
    virtual void doSomething();
};

class GasCanGoodie: public Goodie{
public:
    GasCanGoodie(StudentWorld* sw, int x, int y);
    virtual void doSomething();
};
class LandmineGoodie: public Goodie{
public:
    LandmineGoodie(StudentWorld* sw, int x, int y);
    virtual void doSomething();
};

///////////////
class Zombie: public Damageable{
public:
    Zombie(StudentWorld* sw, int x, int y);
    void vomit(Infectable* target);
};

class DumbZombie: public Zombie{
public:
    DumbZombie(StudentWorld* sw, int x, int y, bool holdsVacc);
    virtual void doSomething();
private:
    bool holdsVaccine;
};

class SmartZombie: public Zombie{
public:
    SmartZombie(StudentWorld* sw, int x, int y);
    virtual void doSomething();
};
/////////////////////////////////

class Damaging: public Actor{
public:
    Damaging(bool inf, bool des, StudentWorld* sw, int id, int x, int y, int dir, int dep);
    virtual void damage(Damageable* target);
};

class Pit: public Damaging{
public:
    Pit(StudentWorld* sw, int x, int y);
    virtual void doSomething();
};

class Landmine: public Damaging{
public:
    Landmine(StudentWorld* sw, int x, int y);
    virtual void doSomething();
};

class Projectile: public Damaging{
public:
    Projectile(StudentWorld* sw, int id, int x, int y, int dir);
    void decST();
private:
    int screenTime;
};

class Flame: public Projectile{
public:
    Flame(StudentWorld* sw, int x, int y, int dir);
    virtual void doSomething();
};

class Vomit: public Projectile{
public:
    Vomit(StudentWorld* sw, int x, int y, int dir);
    virtual void doSomething();
};


#endif // ACTOR_H_
