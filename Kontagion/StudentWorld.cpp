#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <cmath>
#include <list>
#include <tgmath.h> 
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
	: GameWorld(assetPath)
{
}

StudentWorld::~StudentWorld() {
	cleanUp();
}

int StudentWorld::init()
{
	int level = getLevel();
	Actor* a = nullptr;
	soc = new Socrates(this);
	//create pits
	for (int i = 0; i < level; i++) {
		int x, y;
		getCoordinates(x, y);
		while (checkOverlap(x, y, a, false, 2 * SPRITE_RADIUS)) {
			getCoordinates(x, y);
		}
		actorList.push_back(new Pit(x, y, this));
	}
	//create food

	for (int i = 0; i < min(5 * level, 25); i++) {
		int x, y;
		getCoordinates(x, y);
		while (checkOverlap(x, y, a, false, 2 * SPRITE_RADIUS)) {
			getCoordinates(x, y);
		}
		actorList.push_back(new Food(x, y, this));
	}
	//create dirtpiles
	for (int i = 0; i < max(180 - 20 * level, 20); i++) {
		int x, y;
		getCoordinates(x, y);
		while (checkOverlap(x, y, a, true, 2 * SPRITE_RADIUS)) {
			getCoordinates(x, y);
		}
		actorList.push_back(new DirtPile(x, y, this));
	}



	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::affectSoc(int hp, int flamethrower) {
	soc->addFlamethrower(flamethrower);
	soc->affectHealth(hp);
	if (hp < 0 && soc->getHp() > 0) { //socrates still alive
		playSound(SOUND_PLAYER_HURT);
	}

	else if (soc->getHp() <= 0) { //socrates dead
		soc->setDead();
		playSound(SOUND_PLAYER_DIE);
	}
}

bool StudentWorld::doDamage(double x2, double y2, int damage, int radius) {
	Actor* target = nullptr;
	if (checkOverlap(x2, y2, target, false, radius) && target->damaged(damage)) { //check if overlapped target can be damaged
		return true;
	}
	return false;
}

bool StudentWorld::eaten(double x2, double y2) {
	for (list<Actor*>::iterator it = actorList.begin(); it != actorList.end(); it++) {
		double x = (*it)->getX();
		double y = (*it)->getY();
		//set overlapped food item that is alive to dead
		if (getDist(x, y, x2, y2) <= 2 * SPRITE_RADIUS && (*it)->isEdible() && (*it)->isAlive()) {
			(*it)->setDead();
			return true;
		}
	}
	return false;

}

void StudentWorld::getCoordinates(int& x, int& y) {
	x = randInt(0, VIEW_WIDTH);
	y = randInt(0, VIEW_HEIGHT);
	//changes coordinates if distance from center is greater than 120 pixels
	while (getDist(x, y, VIEW_WIDTH / 2, VIEW_HEIGHT / 2) > 120) {
		x = randInt(0, VIEW_WIDTH);
		y = randInt(0, VIEW_HEIGHT);
	}
}

void StudentWorld::getEdgeCoordinates(int& x, int& y) {
	x = randInt(0, VIEW_WIDTH);
	y = randInt(0, VIEW_HEIGHT);
	//changes coordinates if not on the circumference of the world
	while (getDist(x, y, VIEW_WIDTH / 2, VIEW_HEIGHT / 2) != VIEW_RADIUS) {
		x = randInt(0, VIEW_WIDTH);
		y = randInt(0, VIEW_HEIGHT);
	}
}

bool StudentWorld::checkOverlap(double x2, double y2, Actor*& target, bool avoidDirt, int radius) {
	for (list<Actor*>::iterator it = actorList.begin(); it != actorList.end(); it++) {
		double x = (*it)->getX();
		double y = (*it)->getY();
		if (getDist(x, y, x2, y2) <= radius) {
			//continue looping if dirtpile
			if (avoidDirt && (*it)->isBlockable()) {
				continue;
			}
			target = *it; //set target to the overlapped actor
			return true;
		}
	}
	return false;
}

bool StudentWorld::checkOverlapSocrates(double x2, double y2) {
	if (getDist(soc->getX(), soc->getY(), x2, y2) <= (2 * SPRITE_RADIUS)) {
		return true;
	}

	return false;
}

bool StudentWorld::findEdible(double x2, double y2, Direction& direction) {
	int closestDist = 128;
	double closestX;
	double closestY;
	bool foundFood = false;
	for (list<Actor*>::iterator it = actorList.begin(); it != actorList.end(); it++) {
		double x = (*it)->getX();
		double y = (*it)->getY();
		int currentDist = getDist(x, y, x2, y2);
		//set closest coordinates 
		if (currentDist <= closestDist && (*it)->isEdible()) {
			closestX = x;
			closestY = y;
			closestDist = currentDist;
			foundFood = true;
		}
	}
	if (foundFood) {
		direction = findDirection(x2, y2, closestX, closestY);
	}

	return foundFood;
}

bool StudentWorld::findSocrates(double x2, double y2, int radius, Direction& direction) {
	double x = soc->getX();
	double y = soc->getY();
	int currentDist = getDist(x, y, x2, y2);
	if (currentDist <= radius) {
		//get the direction of socrates
		direction = findDirection(x2, y2, x, y);
		return true;
	}
	return false;
}

int StudentWorld::findDirection(double x1, double y1, double x2, double y2) {
	if ((x2 - x1) == 0) {
		return 0;
	}
	const double PI = 4 * atan(1);
	return (atan2((y2 - y1), (x2 - x1)) * 180 / PI); //calculate angle between 2 coordinates
}

bool StudentWorld::ifOverlapBlockable(double x2, double y2, int radius) {
	for (list<Actor*>::iterator it = actorList.begin(); it != actorList.end(); it++) {
		double x = (*it)->getX();
		double y = (*it)->getY();
		//check if overlap a dirtpile
		if (getDist(x, y, x2, y2) <= radius && (*it)->isBlockable()) {
			return true;
		}
	}
	return false;
}

int StudentWorld::getDist(double x1, double y1, double x2, double y2) {
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

int StudentWorld::move()
{
	// This code is here merely to allow the game to build, run, and terminate after you hit enter.
	// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
	soc->doSomething();
	//call do something for each alive player
	for (list<Actor*>::iterator it = actorList.begin(); it != actorList.end(); it++) {
		if ((*it)->isAlive()) {
			(*it)->doSomething();
			if (!soc->isAlive()) {
				decLives();
				return GWSTATUS_PLAYER_DIED;
			}
		}
	}

	for (list<Actor*>::iterator it = actorList.begin(); it != actorList.end(); it++) {
		//while a dirt or pit exists
		if ((*it)->isAlive() && ((*it)->isEnemy() || (*it)->isSpawner())) {
			removeDeadGameObjects();
			addNewActors();
			ostringstream oss;
			//set score
			if (getScore() >= 0) {
				oss << "Score: " << setw(6);
			}
			else {
				oss << "Score: -" << setw(5);
			}
			oss.fill('0');
			oss << abs(getScore()) << "  " << "Level: " << getLevel() << "  " << "Lives: "
				<< getLives() << "  " << "Health:  " << soc->getHp() << "  " << "Sprays:  " << soc->getSprays() << "  " << "Flames: " << soc->getFlames();
			
			string s = oss.str();
			setGameStatText(s);
			return GWSTATUS_CONTINUE_GAME;
		}
	}
	removeDeadGameObjects();
	return GWSTATUS_FINISHED_LEVEL;
}

void StudentWorld::removeDeadGameObjects() {
	for (list<Actor*>::iterator it = actorList.begin(); it != actorList.end(); ) {
		if (!(*it)->isAlive()) {
			delete (*it);
			it = actorList.erase(it);
		}
		else {
			it++;
		}
	}
}

void StudentWorld::addNewActors() {
	//add fungus
	int ChanceFungus = min(510 - getLevel() * 10, 200);
	int x, y;
	if (randInt(0, ChanceFungus - 1) == 0) {
		getEdgeCoordinates(x, y);
		actorList.push_back(new Fungus(x, y, this, getLevel()));
	}
	int ChanceGoodie = min(510 - getLevel() * 10, 250);
	//add goodies
	if (randInt(0, ChanceGoodie - 1) == 0) {
		getEdgeCoordinates(x, y);
		int chance = randInt(0, 9);
		if (chance < 6) {
			actorList.push_back(new HealthGoodie(x, y, this, getLevel()));
		}
		else if (chance >= 6 && chance < 9) {
			actorList.push_back(new FlamethrowerGoodie(x, y, this, getLevel()));
		}
		else {
			actorList.push_back(new ExtraLifeGoodie(x, y, this, getLevel()));
		}
	}

}

void StudentWorld::cleanUp()
{
	list<Actor*>::iterator it = actorList.begin();
	while(it != actorList.end() ) {	
		delete (*it);
		it = actorList.erase(it);
	}
	delete soc;
	soc = nullptr;
	
}

void StudentWorld::createFood(double x, double y) {
	actorList.push_back(new Food(x, y, this));
}
void StudentWorld::createRegularSalmonella(double x, double y) {
	actorList.push_back(new RegularSalmonella(x, y, this));
}

void StudentWorld::createAggressiveSalmonella(double x, double y) {
	actorList.push_back(new AggressiveSalmonella(x, y, this));
}

void StudentWorld::createDirtPile(double x, double y) {
	actorList.push_back(new DirtPile(x, y, this));
}
void StudentWorld::createPit(double x, double y) {
	actorList.push_back(new Pit(x, y, this));
}
void StudentWorld::createHealthGoodie(double x, double y) {
	actorList.push_back(new HealthGoodie(x, y, this, getLevel()));
}
void StudentWorld::createFlamethrowerGoodie(double x, double y) {
	actorList.push_back(new FlamethrowerGoodie(x, y, this, getLevel()));
}
void StudentWorld::createFungus(double x, double y) {
	actorList.push_back(new Fungus(x, y, this, getLevel()));
}
void StudentWorld::createExtraLifeGoodie(double x, double y) {
	actorList.push_back(new ExtraLifeGoodie(x, y, this, getLevel()));
}
void StudentWorld::createFlame(double x, double y, Direction direction) {
	actorList.push_back(new Flame(x, y, direction, this));
}
void StudentWorld::createSpray(double x, double y, Direction direction) {
	actorList.push_back(new Spray(x, y, direction, this));
}

void StudentWorld::createEColi(double x, double y) {
	actorList.push_back(new EColi(x, y, this));
}
