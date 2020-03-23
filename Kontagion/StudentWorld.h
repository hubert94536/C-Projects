#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include <list>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    bool doDamage(double x2, double y2, int damage, int radius);
    bool ifOverlapBlockable(double x2, double y2, int radius);
    bool checkOverlapSocrates(double x2, double y2);
    int getDist(double x1, double y1, double x2, double y2);
    void affectSoc(int hp, int flamethrower);
    bool findEdible(double x2, double y2, Direction& direction); 
    bool findSocrates(double x2, double y2, int radius, Direction& direction);
    bool eaten(double x2, double y2);
    //create actors
    void createFood(double x, double y);
    void createRegularSalmonella(double x, double y);
    void createAggressiveSalmonella(double x, double y); 
    void createFlame(double x, double y, Direction direction);
    void createSpray(double x, double y, Direction direction);
    void createEColi(double x, double y);
    
private:
    Socrates* soc;
    std::list<Actor*> actorList;
    void createDirtPile(double x, double y);
    void createPit(double x, double y);
    void createHealthGoodie(double x, double y);
    void createFlamethrowerGoodie(double x, double y);
    void createFungus(double x, double y);
    void createExtraLifeGoodie(double x, double y);
    void getCoordinates(int& x, int& y);
    void getEdgeCoordinates(int& x, int& y);
    bool checkOverlap(double x2, double y2, Actor*& target, bool avoidDirt, int radius);
    void removeDeadGameObjects();
    void addNewActors();
    int findDirection(double x1, double y1, double x2, double y2);
   
};

#endif // STUDENTWORLD_H_
