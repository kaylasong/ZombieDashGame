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
    bool isPenelope(Actor* other){
        return(other==penelope);
    }
    int getNumCitizens(){
        return(numCitizens);
    }
    std::list<Actor*>::iterator getActors(){
        return(actors.begin());
    }
    std::list<Actor*>::iterator getEnd(){
        return(actors.end());
    }

private:
    Penelope* penelope;
    std::list<Actor*> actors;
    int numCitizens;
};

#endif // STUDENTWORLD_H_
