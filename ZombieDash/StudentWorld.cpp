#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include "Level.h"
using namespace std;

GameWorld* createStudentWorld(string assetPath){
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::~StudentWorld(){
    delete penelope;
    list<Actor*>::iterator it=actors.begin();
    while(it!=actors.end()){
        delete(*it);
        it=actors.erase(it);
    }
}

void StudentWorld::getRidOfDead(){
    list<Actor*>::iterator it=actors.begin();
    while(it!=actors.end()){
        if((*it)->isDead()){
            delete(*it);
            it=actors.erase(it);
        }
        else
            it++;
    }
}

void StudentWorld::addItem(Actor*add){
    actors.push_front(add);
}

////////////////////////////////////////////////////////////////////////////

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{}

int StudentWorld::init()
{
    Level l(assetPath());
    string level="level01.txt";
    Level::LoadResult result = l.loadLevel(level);
    if (result == Level::load_fail_file_not_found)
        cout << "Cannot find level01.txt data file" << endl;
    else if (result == Level::load_fail_bad_format)
        cout << "Your level was improperly formatted" << endl;
    else if(result==Level::load_success){
        int citizens=0;
        for(int r=0;r<16;r++){
            for(int c=0;c<16;c++){
                Level::MazeEntry ge = l.getContentsOf(c,r);
                switch(ge){
                    case Level::empty:
                        break;
                    case Level::smart_zombie:
                        actors.push_back(new SmartZombie(this,c*16,r*16));
                        break;
                    case Level::dumb_zombie:
                        if(citizens%10==0)
                            actors.push_back(new DumbZombie(this,c*16,r*16,true));
                        else
                            actors.push_back(new DumbZombie(this,c*16,r*16,false));
                        break;
                    case Level::player:
                        penelope=new Penelope(this,c*16,r*16);
                        actors.push_back(penelope);
                        break;
                    case Level::exit:
                        actors.push_back(new Exit(this,c*16,r*16));
                        break;
                    case Level::wall:
                        actors.push_back(new Wall(this,c*16,r*16));
                        break;
                    case Level::pit:
                        actors.push_back(new Pit(this,c*16,r*16));
                        break;
                    case Level::citizen:
                        numCitizens++;
                        actors.push_back(new Citizen(this,c*16,r*16));
                        break;
                    case Level::vaccine_goodie:
                        actors.push_back(new VaccineGoodie(this,c*16,r*16));
                        break;
                    case Level::gas_can_goodie:
                        actors.push_back(new GasCanGoodie(this,c*16,r*16));
                        break;
                    case Level::landmine_goodie:
                        actors.push_back(new LandmineGoodie(this,c*16,r*16));
                        break;
                }
            }
        }
        
        
        
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    list<Actor*>::iterator it=actors.begin();
    while(it!=actors.end()){
        (*it)->doSomething();
        it++;
    }
    getRidOfDead();
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
}
