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

    void addItem(Actor* add);
    Penelope* getPenelope();
    int getNumCitizens(); 
private:
    Penelope* penelope;
    std::list<Actor> actors;
    
};

#endif // STUDENTWORLD_H_
