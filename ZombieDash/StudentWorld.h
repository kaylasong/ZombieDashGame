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
    Actor* getPenelope(){return(penelope);};
    
    int getNumCitizens(){
        return(numCitizens);
    }
    std::list<Actor*>::iterator getActors(){
        return(actors.begin());
    }
    std::list<Actor*>::iterator getEnd(){
        return(actors.end());
    }
    void setGameStatus(int x){
        gameStatus=x;
    }
    int getGameStatus(){
        return(gameStatus);
    }
    void nextLevel(){
        whichLevel++; 
    }

private:
    Penelope* penelope;
    std::list<Actor*> actors;
    int numCitizens;
    int gameStatus;
    int whichLevel=0;
    std::string levels[6]={"level01.txt","level02.txt","level03.txt","level04.txt","level05.txt","level06.txt"};
};

#endif // STUDENTWORLD_H_
