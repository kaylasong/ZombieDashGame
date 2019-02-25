#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <sstream>
#include <iomanip>
#include "Level.h"
using namespace std;

GameWorld* createStudentWorld(string assetPath){
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::~StudentWorld(){
    cleanUp();
}

void StudentWorld::getRidOfDead(){
    list<Actor*>::iterator it=actors.begin();
    while(it!=actors.end()){
        if((*it)!=nullptr && ((*it)->isDead() || (*it)->isSafe())){
            if((*it)->canBeInf())
                numCitizens--;
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

Actor* StudentWorld::objectOverlap(Actor* curr,double x, double y){
    std::list<Actor*>::iterator it=actors.begin();
    while(it!=actors.end()){
        if(curr!=(*it)){
            if((x-(*it)->getX())*(x-(*it)->getX())+
               (y-(*it)->getY())*(y-(*it)->getY())<=100)
                return(*it);
        }
        it++;
    }
    return(nullptr);
}
Actor* StudentWorld::willHitObstacle(Actor* curr, double x, double y){
    std::list<Actor*>::iterator it=actors.begin();
    while(it!=actors.end()){
        if(curr!=(*it)){
            if((*it)->isObs()){
                if(x<=(*it)->getX()+15 && x+15>=(*it)->getX()&&
                   y<=(*it)->getY()+15 && y+15>=(*it)->getY())
                    return(*it);
            }
        }
        it++;
    }
    return(nullptr);
}
bool StudentWorld::willHitWall(Actor* curr, double x, double y){
    Actor* other=willHitObstacle(curr,x,y);
    return(other!=nullptr&&!other->canBeDes());
}
bool StudentWorld::willOverlapWithInfectable(Actor* curr, double x, double y){
    Actor* other=objectOverlap(curr,x,y);
    return(other!=nullptr&&other->canBeInf());
}
bool StudentWorld::willOverlapWithPenelope(Actor* curr, double x, double y){
    std::list<Actor*>::iterator it=actors.begin();
    while(it!=actors.end()){
        if((*it)==penelope){
            if((x-(*it)->getX())*(x-(*it)->getX())+
               (y-(*it)->getY())*(y-(*it)->getY())<=100)
                return(true);
        }
        it++;
    }
    return(false);
}
Actor* StudentWorld::willOverlapWithDestructable(Actor* curr, double x, double y){
    Actor* other=objectOverlap(curr,x,y);
    if(other!=nullptr && other->canBeDes())
        return(other);
    return(nullptr);
}
//obstacle bounding box
bool StudentWorld::obstacleThere(Actor* curr, double x, double y){
    Actor* other=willHitObstacle(curr,x,y);
    return(other!=nullptr);
}
//checks to see if bounding box will overlap at all
bool StudentWorld::willHitAnything(Actor* curr, double x, double y){
    std::list<Actor*>::iterator it=actors.begin();
    while(it!=actors.end()){
        if(curr!=(*it)){
            if(x<=(*it)->getX()+15 && x+15>=(*it)->getX()&&
               y<=(*it)->getY()+15 && y+15>=(*it)->getY()){
                return(true);
            }
        }
        it++;
    }
    return(false);
}
bool StudentWorld::killOverlapable(Actor* curr, double x, double y){
    Actor* other=willOverlapWithDestructable(curr, x, y);
    if(other!=nullptr){
        other->kill();
        return(true);
    }
    return(false);
}
void StudentWorld::infectOverlapper(Actor* curr, double x, double y){
    Actor* other=objectOverlap(curr, x, y);
    if(other!=nullptr && other->canBeInf())
        other->getInfected();
}
double StudentWorld::distanceFromPenelope(Actor* curr, double & dist){
    return(distanceBetween(curr,penelope,dist));
}
double StudentWorld::distanceBetween(Actor* curr, Actor* other, double& dist){
    double x=other->getX()-curr->getX();
    double y=other->getY()-curr->getY();
    dist=sqrt(pow(x,2)+pow(y,2));
    double angle=atan2(y,x);
    angle*=180;
    angle/=3.14159265;
    if(angle<0){
        angle+=(2*180);
    }
    return(angle);
}
double StudentWorld::getMinDistance(Actor* curr,double & minDist){
    double temp=minDist;
    //watch out for this one
    double tempAngle=0;
    double ansAngle=0;
    list<Actor*>::iterator it=actors.begin();
    while(it!=actors.end()){
        if((*it)->isObs()&&(*it)->canBeDes()&&!((*it)->canBeInf())){
            tempAngle=distanceBetween(curr,*it,temp);
            if(temp<minDist){
                minDist=temp;
                ansAngle=tempAngle;
            }
        }
        it++;
    }
    return(ansAngle);
}
void StudentWorld::manageExit(Actor* curr){
    Actor* other=objectOverlap(curr,curr->getX(),curr->getY());
    if(other==nullptr)
        return;
    if(other->canBeInf()){
        if(penelope!=other){
            increaseScore(500);
            playSound(SOUND_CITIZEN_SAVED);
            other->save();
        }
        else if(numCitizens==0){
            setGameStatus(GWSTATUS_FINISHED_LEVEL);
            playSound(SOUND_LEVEL_FINISHED);
            nextLevel();
        }
    }
}
double StudentWorld::closestTarget(Actor* curr,double & minDist){
    double temp=minDist;
    //watch out for this one
    double tempAngle=0;
    double ansAngle=0;
    list<Actor*>::iterator it=actors.begin();
    while(it!=actors.end()){
        if((*it)->canBeInf()){
            tempAngle=distanceBetween(curr,*it,temp);
            if(temp<minDist){
                minDist=temp;
                ansAngle=tempAngle;
            }
        }
        it++;
    }
    return(ansAngle);
}
////////////////////////////////////////////////////////////////////////////

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{}

int StudentWorld::init()
{
    numFlames=0;
    numLandmines=0;
    numVacc=0;
    if(whichLevel==6)
        return(GWSTATUS_PLAYER_WON);
    gameStatus=GWSTATUS_CONTINUE_GAME;
    Level l(assetPath());
    string level=levels[whichLevel]; 
    Level::LoadResult result = l.loadLevel(level);
    if (result == Level::load_fail_file_not_found)
        cout << "Cannot find level01.txt data file" << endl;
    else if (result == Level::load_fail_bad_format)
        cout << "Your level was improperly formatted" << endl;
    else if(result==Level::load_success){
        numCitizens=0;
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
                        if(randInt(1,10)==3)
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
    if(whichLevel==6)
        return(GWSTATUS_PLAYER_WON);
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    penelope->doSomething();
    list<Actor*>::iterator it=actors.begin();
    while(it!=actors.end()){
        (*it)->doSomething();
        it++;
    }
    getRidOfDead();
    ostringstream topThing;
    topThing.fill('0');
    topThing<<"Score: ";
    if(getScore()<0)
        topThing<<"-"<<setw(5)<<-1*getScore();
    else
        topThing<<setw(6)<<getScore();
    topThing<<"  Level: "<<getLevel()<<"  Lives: "<<getLives();
    topThing<<"  Vacc: "<<numVacc<<"  Flames: "<<numFlames<<
    "  Mines: "<<numLandmines<<"  Infected: ";
    if(penelope->getIC()==-1)
        topThing<<0;
    else
        topThing<<penelope->getIC();
    setGameStatText(topThing.str());
    return gameStatus;
}

void StudentWorld::cleanUp()
{
    list<Actor*>::iterator it=actors.begin();
    while(it!=actors.end()){
        if((*it)!=nullptr){
            delete(*it);
            it=actors.erase(it);
       }
       else
            it++;
    }
}
