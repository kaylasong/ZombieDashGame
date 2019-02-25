#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <iostream>


class StudentWorld;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor: public GraphObject{
public:
    Actor(bool inf, bool des, bool obs, StudentWorld* sw, int id, double x, double y, int dir, int dep);
    virtual void doSomething()=0;
    int nearObstacle(Actor* other);
    double distanceBetween(Actor* other, double& dist);
    double minDistance(double & minDist);
    
    //accessors
    void kill(){m_isDead=true;}
    void save(){m_safe=true;}
    bool canBeInf(){return(canBeInfected);}
    bool canBeDes(){return(canBeDestroyed);}
    bool isDead(){return(m_isDead);}
    bool isSafe(){return(m_safe);}
    bool isObs(){return(isObstacle);}
    StudentWorld* getWorld(){return(world);}
    
    virtual void getInfected(){}
private:
    bool canBeInfected;
    bool canBeDestroyed;
    bool m_isDead;
    bool isObstacle;
    bool m_safe;
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
    Damageable(bool inf, bool obs, StudentWorld* sw, int id, double x, double y, int dir, int dep);
    bool move(int dir, int dist);
};

//////////////
class Infectable: public Damageable{
public:
    Infectable(StudentWorld* sw, int id, int x, int y, int dir, int dep);
    int getIC(){return(infectedCount);}
    void incrementIC();
    void getInfected();
    void getHealed(){infectedCount=-1;}
private:
    int infectedCount;
};

class Penelope: public Infectable{
public:
    ~Penelope();
    Penelope(StudentWorld* sw, int x, int y);
    virtual void doSomething();
    //all the goodie thingies
    void deployVaccine();
    void deployFlame();
    void deployLandmine();
private:
    bool checkMovement(int dir); 
};

class Citizen: public Infectable{
public:
    ~Citizen();
    Citizen(StudentWorld* sw, int x, int y);
    virtual void doSomething();
    bool getCM(){return(canMove);};
    void toggle(){canMove=!canMove;};
    double minDistance(double & minDist);
private:
    bool canMove;
};
///////////////
class Goodie: public Damageable{
public:
    Goodie(StudentWorld* sw, int id, int x, int y);
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
    ~Zombie();
    Zombie(StudentWorld* sw, int x, int y);
    void doVomit(double x, double y, int dir);
    void pickNewMovementPlan();
    int getMD(){return(moveDis);}
    void setMD(int x){moveDis=x;}
    bool canMove(){return(m_canMove);}
    void toggleCM(){m_canMove=!m_canMove;}
    bool vomit();
private:
    int moveDis;
    bool m_canMove;
};

class DumbZombie: public Zombie{
public:
    ~DumbZombie();
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
};

class Pit: public Damaging{
public:
    Pit(StudentWorld* sw, int x, int y);
    virtual void doSomething();
};

class Landmine: public Damaging{
public:
    ~Landmine();
    Landmine(StudentWorld* sw, int x, int y);
    virtual void doSomething();
    void incrementActivationTime();
    int getActivationTime(){return(activationTime);}
private:
    int activationTime;
};

class Projectile: public Damaging{
public:
    Projectile(StudentWorld* sw, int id, int x, int y, int dir);
    void decST();
    int getScreenTime(){return(screenTime);}
private:
    int screenTime;
};

class Flame: public Projectile{
public:
    ~Flame();
    Flame(StudentWorld* sw, int x, int y, int dir, bool ilf);
    virtual void doSomething();
    bool getIsLandmineFlame(){return(isLandmineFlame);}
private:
    bool isLandmineFlame;
};

class Vomit: public Projectile{
public:
    Vomit(StudentWorld* sw, int x, int y, int dir);
    virtual void doSomething();
    void infect(Infectable* target);
};


#endif // ACTOR_H_
