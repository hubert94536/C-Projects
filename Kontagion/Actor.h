#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <string>
#include <map>

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject {
public:
	Actor(int imageID, double startX, double startY, Direction startDirection, int depth, int hp, StudentWorld* world, int damage, bool beDamage);
	virtual void doSomething();
	virtual bool damaged(int amt);
	virtual void setDead();
	void affectHealth(int amt);	
	//getter methods
	int getDamage();
	StudentWorld* getStudentWorld();
	int getHp();
	bool isAlive();
	//class identifiers
	virtual bool isEnemy();
	virtual bool isAffectHealth();
	virtual bool isBlockable();
	virtual bool isEdible();
	virtual bool isSpawner();
	
private:
	int m_hp;
	bool m_alive;
	StudentWorld* m_studentWorld;
	int m_damage;
	bool m_beDamage;
};

class DirtPile: public Actor {
public:
	DirtPile(double startX, double startY, StudentWorld* world);
	virtual bool isBlockable();
private:
};

class Socrates : public Actor {
public: 
	Socrates(StudentWorld* world);
	virtual void doSomething();
	virtual void  moveAngle();
	void addFlamethrower(int amt);
	int getSprays();
	int getFlames();

private:
	int m_spray;
	int m_flamethrower;
	int m_angle;
};

class Food : public Actor {
public:
	Food(double startX, double startY, StudentWorld* world);
	virtual bool isEdible();

private:
};

//Projectile base class
class Projectile : public Actor {
public:
	Projectile(int imageID, double startX, double StartY, Direction startDirection, StudentWorld* world, int travelDistance, int damage);
	virtual void doSomething();
private:
	int m_travelDistance;
};

class Flame : public Projectile {
public:
	Flame(double startX, double startY, Direction startDirection, StudentWorld* world);
private:
	
};

class Spray : public Projectile {
public:
	Spray(double startX, double startY, Direction startDirection, StudentWorld* world);
private:

};

//Goodie base class
class Goodie : public Actor {
public:
	Goodie(int imageID, double startX, double startY, StudentWorld* world, int lifeTime, int points, int affectHealth);
	virtual void doSomething();
	virtual void activate();
	
private:
	int m_lifeTime;
	int m_points;
	int m_affectHealth;
};

class HealthGoodie : public Goodie {
public:
	HealthGoodie(double startX, double startY, StudentWorld* world, int level);
	virtual bool isAffectHealth();
private:
};


class FlamethrowerGoodie : public Goodie {
public:
	FlamethrowerGoodie(double startX, double startY, StudentWorld* world, int level);
	virtual void activate();
	
private:
	
};

class ExtraLifeGoodie : public Goodie {
public:
	ExtraLifeGoodie(double startX, double startY, StudentWorld* world, int level);
	virtual bool isAffectHealth();
	virtual void activate();
	
private:
	
};

class Fungus : public Goodie {
public:
	Fungus(double startX, double startY, StudentWorld* world, int level);
	virtual bool isAffectHealth();
	virtual void activate();
	
private:
	
};

class Pit : public Actor {
public:
	Pit(double startX, double startY, StudentWorld* world);
	virtual void doSomething();
	virtual bool isSpawner();
	
private:
	std::map<std::string, int> bacteria;
};

class Bacteria : public Actor {
public:
	Bacteria(int imageID, double startX, double startY, StudentWorld* world, int damage, int hp);
	bool virtual damaged(int amt);
	void virtual doSomething();	
	virtual void createActor(double x, double y);
	void increaseFoodCount();
	void findEdible();
	bool checkMovementDistance();
	virtual void moveForward(double x, double y, Direction direction);
	void duplicate();
	int getFoodCount();
	virtual bool bacteriaCondition();
	virtual void playSound(bool isDead);
	virtual bool isEnemy();
private:
	int m_planDistance;
	int m_foodCount;	
};

class RegularSalmonella : public Bacteria {
public:
	RegularSalmonella(double startX, double startY, StudentWorld* world);
	virtual void createActor(double x, double y);
	
private:

};

class AggressiveSalmonella : public Bacteria {
public:
	AggressiveSalmonella(double startX, double startY, StudentWorld* world);
	virtual void createActor(double x, double y);
	virtual void moveForward(double x, double y, Direction direction);
	virtual bool bacteriaCondition();
private:
};

class EColi : public Bacteria {
public:
	EColi(double startX, double startY, StudentWorld* world);
	virtual void doSomething();	
	virtual void playSound(bool isDead);
	virtual void moveForward(double x, double y, Direction direction);
private:
	
};
#endif // ACTOR_H_
