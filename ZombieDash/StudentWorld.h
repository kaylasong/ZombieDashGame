#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include "GraphObject.h"
#include "Actor.h"
#include <list>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();

    ~StudentWorld();
    void addItem(Actor* add);
    void getRidOfDead();
    
    //penelope's shit
    void addVaccines(int x){numVacc+=x;}
    void addFlames(int x){numFlames+=x;}
    void addLandmines(int x){numLandmines+=x;}
    int getFlames(){return(numFlames);}
    int getLandmines(){return(numLandmines);}
    int getVaccines(){return(numVacc);}
    void decFlames(){numFlames--;}
    void decLandmines(){numLandmines--;}
    void decVaccines(){numVacc--;}
    
    //working with them collisions
    bool willOverlap(Actor* curr, double x, double y);
    bool willHitWall(Actor* curr, double x, double y);
    bool willOverlapWithInfectable(Actor* curr,double x, double y);
    bool willOverlapWithPenelope(Actor* curr, double x, double y);
    bool obstacleThere(Actor* curr, double x, double y);
    bool willHitAnything(Actor* curr, double x, double y);
    bool killOverlapable(Actor* curr, double x, double y);
    void infectOverlapper(Actor* curr, double x, double y);
    bool flameAllowed(double x, double y);
    
    //for the smart movement
    double getMinDistance(Actor* curr,double & minDist);
    double distanceBetween(Actor* cur,Actor* other, double& dist);
    double distanceFromPenelope(Actor* curr, double & dist);
    double closestTarget(Actor* curr, double & dist);
    
    void manageExit(Actor* curr);
    
    int getNumCitizens(){
        return(numCitizens);
    }
    void setGameStatus(int x){
        gameStatus=x;
    }
    int getGameStatus(){
        return(gameStatus);
    }

private:
    Penelope* penelope;
    int numLandmines;
    int numFlames;
    int numVacc;
    
    Actor* objectOverlap(Actor* curr, double x, double y);
    Actor* willHitObstacle(Actor* curr, double x, double y);
    Actor* willOverlapWithDestructable(Actor* curr, double x, double y);
    
    std::list<Actor*> actors;
    int numCitizens;
    int gameStatus;
    int whichLevel=0;
    std::string levels[6]={"level01.txt","level02.txt","level03.txt","level04.txt","level05.txt","level06.txt"};
};

#endif // STUDENTWORLD_H_
