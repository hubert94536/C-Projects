#include "Actor.h"
#include "StudentWorld.h"
#include "GraphObject.h"
#include<map>
// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
using namespace std;
//Actor implementation
Actor::Actor(int imageID, double startX, double startY, Direction startDirection, int depth, int hp, StudentWorld* world, int damage, bool beDamage)
	:GraphObject(imageID, startX, startY, startDirection, depth)
 {
	if (hp > 0) {
		m_hp = hp;
	}
	m_alive = true;
	m_studentWorld = world;
	m_damage = damage;
	m_beDamage = beDamage;
}

int Actor::getDamage() {
	return m_damage;
}


void Actor::doSomething() {
	return;
}

void Actor::setDead() {
	m_alive = false;
}

bool Actor::isAlive() {
	return m_alive;
}

int Actor::getHp() {
	return m_hp;
}

bool Actor::damaged(int amt) {
	if (!m_beDamage || !isAlive()) { //check if actor can be damaged and is alive
		return false;
	}

	m_hp -= amt;
	if (m_hp <= 0) {
		setDead();
	}
	return true;
}

void Actor::affectHealth(int amt) {
	if (amt + m_hp > 100) { //if amount would make hp go over 100
		m_hp = 100;
	}

	else {
		m_hp += amt;
	}

}

StudentWorld* Actor::getStudentWorld() {
	return m_studentWorld;
}
bool Actor::isAffectHealth() { return false; }
bool Actor::isBlockable() { return false; }
bool Actor::isEdible() { return false; }
bool Actor::isEnemy() { return false; }
bool Actor::isSpawner() { return false; }
//DirtPile implementation
DirtPile::DirtPile(double startX, double startY, StudentWorld* world)
	:Actor(IID_DIRT, startX, startY, 0, 1, 0, world, 0, true) {
}

bool DirtPile::isBlockable() {
	return true;
}


//Socrates implementation
Socrates::Socrates(StudentWorld* world)
	:Actor(IID_PLAYER, 0, VIEW_HEIGHT /2 , 0, 0, 100, world, 0, true) {
	m_spray = 20;
	m_flamethrower = 5;
	m_angle = 180;
}

int Socrates::getFlames() {
	return m_flamethrower;
}

int Socrates::getSprays() {
	return m_spray;
}

void Socrates::doSomething() {
	if (!isAlive()) {
		return;
	}
	int ch;
	double x, y;
	if (getStudentWorld()->getKey(ch)) {
		switch (ch) {
		case KEY_PRESS_LEFT: //move counterclockwise 5 degress
			m_angle += 5;
			moveAngle();
			break;
		case KEY_PRESS_RIGHT: //move clockwise 5 degrees
			m_angle -= 5;
			moveAngle();
			break;
		case KEY_PRESS_SPACE:
			if (m_spray > 0) {
				//create spray
				getPositionInThisDirection(getDirection(), 2 * SPRITE_RADIUS, x, y);
				getStudentWorld()->createSpray(x, y, getDirection());
				m_spray--;
				getStudentWorld()->playSound(SOUND_PLAYER_SPRAY);
			}
			break;
		case KEY_PRESS_ENTER:
			if (m_flamethrower > 0) {
				//create ring of flame
				for (int i = 0; i < 16; i++) {
					getPositionInThisDirection(i * 22, 2 * SPRITE_RADIUS, x, y);
					getStudentWorld()->createFlame(x, y, i * 22);
				}
				m_flamethrower--;
				getStudentWorld()->playSound(SOUND_PLAYER_FIRE);
			}
			break;
		}
	}
	else {
		if (m_spray < 20) {
			m_spray++;
		}
	}
}

void Socrates::addFlamethrower(int amt) {
	m_flamethrower += amt;
}

void Socrates::moveAngle() {
	const double PI = 4 * atan(1);
	//move in a circle
	moveTo(VIEW_WIDTH / 2 + VIEW_RADIUS * cos(m_angle * PI / 180), VIEW_WIDTH / 2 + VIEW_RADIUS * sin(m_angle * PI / 180));
	setDirection(m_angle + 180);
}

//Food implemtation
Food::Food(double startX, double startY, StudentWorld* world)
	:Actor(IID_FOOD, startX, startY, 90, 1, 0, world, 0, false) {
}

bool Food::isEdible() {
	return true;
}

//Projectile implementation
Projectile::Projectile(int imageID, double startX, double startY, Direction startDirection, StudentWorld* world, int travelDistance, int damage) 
	:Actor(imageID, startX, startY, startDirection, 1, 0, world, damage, false)
{
	m_travelDistance = travelDistance;
}

void Projectile::doSomething() {
	if (!isAlive()) {
		return;
	}
	//check if damages an actor
	if (getStudentWorld()->doDamage(getX(), getY(), getDamage(), 2 * SPRITE_RADIUS)) {
		setDead();
		return;
	}
	moveAngle(getDirection(), SPRITE_RADIUS * 2); //move forward
	m_travelDistance -= SPRITE_RADIUS * 2;
	if (m_travelDistance <= 0) {
		setDead();
	}
}

//Flame implementation
Flame::Flame(double startX, double startY, Direction startDirection, StudentWorld* world)
	: Projectile(IID_FLAME, startX, startY, startDirection, world, 32, 5) {
}


//Spray implementation
Spray::Spray(double startX, double startY, Direction startDirection, StudentWorld* world)
	: Projectile(IID_SPRAY, startX, startY, startDirection,world, 112, 2) {
}


//Goodie implementation

Goodie::Goodie(int imageID, double startX, double startY, StudentWorld* world, int lifeTime, int points, int affectHealth)
	: Actor(imageID, startX, startY, 0, 1, 0, world, 0, true) {
	m_points = points;
	m_affectHealth = affectHealth;
	m_lifeTime = lifeTime;

}

void Goodie::doSomething() {
	if (!isAlive()) {
		return;
	}
	m_lifeTime--;
	//check overlap socrates
	if (getStudentWorld()->checkOverlapSocrates(getX(), getY())) {
		activate();
		return;
	}
	//check lifetime
	if (m_lifeTime == 0) {
		setDead();
	}
}

void Goodie::activate() {
	getStudentWorld()->increaseScore(m_points);
	getStudentWorld()->affectSoc(m_affectHealth, 0);
	setDead();
	getStudentWorld()->playSound(SOUND_GOT_GOODIE);
}

//Health Goodie implementation

HealthGoodie::HealthGoodie(double startX, double startY, StudentWorld* world, int level)
	:Goodie(IID_RESTORE_HEALTH_GOODIE, startX, startY, world, max(rand() % (300 - 10 * level), 50), 250, 100)
{
}

bool HealthGoodie::isAffectHealth() {
	return true;
}

//Flamethrower goodie implementation
FlamethrowerGoodie::FlamethrowerGoodie(double startX, double startY, StudentWorld* world, int level)
	:Goodie(IID_FLAME_THROWER_GOODIE, startX, startY, world, max(rand() % (300 - 10 * level), 50), 300, 0)
{
}

void FlamethrowerGoodie::activate() {
	getStudentWorld()->increaseScore(300);
	getStudentWorld()->affectSoc(0, 5);
	setDead();
	getStudentWorld()->playSound(SOUND_GOT_GOODIE);
}

//ExtraLife goodie implementation
ExtraLifeGoodie::ExtraLifeGoodie(double startX, double startY, StudentWorld* world, int level)
	:Goodie(IID_EXTRA_LIFE_GOODIE, startX, startY, world, max(rand() % (300 - 10 * level), 50), 500, 0)
{
}

void ExtraLifeGoodie::activate() {
	getStudentWorld()->increaseScore(500);
	getStudentWorld()->incLives();
	setDead();
	getStudentWorld()->playSound(SOUND_GOT_GOODIE);
}

bool ExtraLifeGoodie::isAffectHealth() {
	return true;
}

//Fungus implementation
Fungus::Fungus(double startX, double startY, StudentWorld* world, int level)
	:Goodie(IID_FUNGUS, startX, startY, world, max(rand() % (300 - 10 * level), 50), -50, -20)
{

}

bool Fungus::isAffectHealth() {
	return true;
}

void Fungus::activate() {
	getStudentWorld()->increaseScore(-50);
	getStudentWorld()->affectSoc(-20, 0);
	setDead();
}
//Pit implementation
Pit::Pit(double startX, double startY, StudentWorld* world) 
	:Actor(IID_PIT, startX, startY, 0, 1, 0, world, 0, false)
{
	bacteria.insert({ "regular salmonella", 5 });
	bacteria.insert({ "aggressive salmonella", 3 });
	bacteria.insert({ "E.coli", 3 });
}

bool Pit::isSpawner() {
	return true;
}

void Pit::doSomething() {
	if (bacteria.size() == 0) {
		setDead();
		return;
	}
	//1 in 50 chance of bacteria being created
	if (randInt(0, 49) == 0) {
		//return a random bacteria
		int bac = randInt(0, bacteria.size() - 1);
		map<string, int>::iterator it = bacteria.begin();
		for (int i = 0; i < bac; i++) {
			it++;
		}
		it->second--;
		if (it->first == "regular salmonella") {
			getStudentWorld()->createRegularSalmonella(getX(), getY());
		}
		else if (it->first == "aggressive salmonella") {
			getStudentWorld()->createAggressiveSalmonella(getX(), getY());
		}

		else {
			getStudentWorld()->createEColi(getX(), getY());
		}
		//delete bacteria from map if the pit no longer contains it
		if (it->second == 0) {
			bacteria.erase(it);
		}
		getStudentWorld()->playSound(SOUND_BACTERIUM_BORN);
	}
}

//Bactera implementation
Bacteria::Bacteria(int imageID, double startX, double startY, StudentWorld* world, int damage, int hp)
	: Actor(imageID, startX, startY, 90, 0, hp, world, damage, true)
{
	m_planDistance = 0;
	m_foodCount = 0;
}

void Bacteria::createActor(double x, double y) {
	return;
}

int Bacteria::getFoodCount() {
	return m_foodCount;
}

void Bacteria::increaseFoodCount() {
	m_foodCount++;
}


void Bacteria::doSomething() {
	if (!isAlive()) {
		return;
	}

	bool bCondition = bacteriaCondition(); 
	//check overlap socrates
	if (getStudentWorld()->checkOverlapSocrates(getX(), getY())) {
		getStudentWorld()->affectSoc(-getDamage(), 0);
		if (!bCondition) {
			checkMovementDistance();
		}
		return;
	}
	//check if eaten 3 food items
	if (m_foodCount == 3) {
		duplicate();
		if (!bCondition) {
			checkMovementDistance();
		}
		return;
	}
	//check if overlap with food item
	if (getStudentWorld()->eaten(getX(), getY())){
		m_foodCount++;
	}
	
	if (!bCondition) {
		if (checkMovementDistance()) {
			return;
		}
		else {
			findEdible();
		}
	}

}

void Bacteria::duplicate() {
	double x = getX();
	double y = getY();
	//create coordinates of new bacteria to the old bacteria
	if (x < VIEW_WIDTH / 2) {
		x += SPRITE_RADIUS;
	}

	else {
		x -= SPRITE_RADIUS;
	}

	if (y < VIEW_HEIGHT / 2) {
		y += SPRITE_RADIUS;
	}

	else {
		y -= SPRITE_RADIUS;
	}
	
	createActor(x, y);
	m_foodCount = 0;
}

bool Bacteria::checkMovementDistance() {
	double x, y;
	//move forward if movement plan distance is greater than 0
	if (m_planDistance > 0) {
		m_planDistance--;
		getPositionInThisDirection(getDirection(), 3, x, y);
		moveForward(x, y, getDirection());
		return true;
	}
	//set random direction
	setDirection(randInt(0, 359));
	m_planDistance = 10;
	return false;
}

void Bacteria::findEdible() {
	Direction direction;
	double x, y;
	//find nearest food item and move towards it
	if (getStudentWorld()->findEdible(getX(), getY(), direction)) {
		getPositionInThisDirection(direction, 3, x, y);
		moveForward(x, y, direction);
	}
	//set random direction
	else {
		setDirection(randInt(0, 359));
		m_planDistance = 10;
	}
}

void Bacteria::moveForward(double x, double y, Direction direction) {
	//check overlap with dirtpile or the edge of the world
	if (getStudentWorld()->ifOverlapBlockable(x, y, SPRITE_RADIUS) || getStudentWorld()->getDist(x, y, VIEW_WIDTH / 2, VIEW_WIDTH / 2) >= VIEW_RADIUS) {
		setDirection(randInt(0, 359));
		m_planDistance = 10;
	}

	else {
		setDirection(direction);
		moveTo(x, y);
	}
}

bool Bacteria::damaged(int amt) {
	affectHealth(-amt);
	if (getHp() > 0) {
		playSound(false);
	}
	else {
		setDead();
		playSound(true);
		getStudentWorld()->increaseScore(100);
		//1 in 2 chance of creating food item
		int chance = randInt(0, 1);
		if (chance == 0) {
			getStudentWorld()->createFood(getX(), getY());
		}
	}
	return true;
}

bool Bacteria::bacteriaCondition() {
	return false;
}

void Bacteria::playSound(bool isDead) {
	if (isDead) {
		getStudentWorld()->playSound(SOUND_SALMONELLA_DIE);
	}
	else {
		getStudentWorld()->playSound(SOUND_SALMONELLA_HURT);
	}
}

bool Bacteria::isEnemy() {
	return true;
}

//Regular salmonella implementation
RegularSalmonella::RegularSalmonella(double startX, double startY, StudentWorld* world)
	:Bacteria(IID_SALMONELLA, startX, startY, world, 1, 4)
{
}

void RegularSalmonella::createActor(double x, double y) {
	getStudentWorld()->createRegularSalmonella(x, y);
}

//Aggressive salmonella implementation
AggressiveSalmonella::AggressiveSalmonella(double startX, double startY, StudentWorld* world)
	:Bacteria(IID_SALMONELLA, startX, startY, world, 2, 10)
{
}

void AggressiveSalmonella::createActor(double x, double y) {
	getStudentWorld()->createAggressiveSalmonella(x, y);
}

bool AggressiveSalmonella::bacteriaCondition() {
	Direction direction;
	bool foundSocrates = getStudentWorld()->findSocrates(getX(), getY(), 72, direction);
	//move towards socrates if found
	if (foundSocrates) {
		double x, y;
		getPositionInThisDirection(direction, 3, x, y);
		moveForward(x, y, direction);
		return true;
	}
	return false;
}


void AggressiveSalmonella::moveForward(double x, double y, Direction direction) {
	bool foundSocrates = getStudentWorld()->findSocrates(getX(), getY(), 72, direction);
	//check if overlap with dirtpile and socrates is found
	if (getStudentWorld()->ifOverlapBlockable(x, y, SPRITE_RADIUS) && foundSocrates) {
		setDirection(direction);
		return;
	}
	//check if overlap with edge of world and socrates is found
	else if (getStudentWorld()->getDist(x, y, VIEW_WIDTH / 2, VIEW_WIDTH / 2) >= VIEW_RADIUS && foundSocrates) {
		return;
	}

	else {
		Bacteria::moveForward(x, y, direction);
	}
	
}

//EColi implementation
EColi::EColi(double startX, double startY, StudentWorld* world)
	:Bacteria(IID_ECOLI, startX, startY, world, 4, 5)
{
}

void EColi::doSomething() {
	if (!isAlive()) {
		return;
	}
	//check overlap with socrates
	if (getStudentWorld()->checkOverlapSocrates(getX(), getY())) {
		getStudentWorld()->affectSoc(-getDamage(), 0);
	}
	//check if eaten 3 food items
	else if (getFoodCount() == 3) {
		duplicate();
	}
	//check if EColi eats a food item
	else if (getStudentWorld()->eaten(getX(), getY())) {
		increaseFoodCount();
	}
	Direction direction;
	bool foundSocrates = getStudentWorld()->findSocrates(getX(), getY(), 256, direction);
	//move towards socrates if found
	if (foundSocrates) {
		double x, y;
		getPositionInThisDirection(direction, 3, x, y);
		moveForward(x, y, direction);
	}
}

void EColi::moveForward(double x, double y, Direction direction) {
	int attempts = 10;
	setDirection(direction);
	//increment direction by 10 degrees if it hits a dirt pile or edge of world
	while (getStudentWorld()->ifOverlapBlockable(x, y, SPRITE_RADIUS) || getStudentWorld()->getDist(x, y, VIEW_WIDTH / 2, VIEW_WIDTH / 2) >= VIEW_RADIUS) {
		direction += 10;
		getPositionInThisDirection(direction, 2, x, y);
		attempts--;
		if (attempts == 0) {
			return;
		}
	}
	moveTo(x, y);

}

void EColi::playSound(bool isDead) {
	if (isDead) {
		getStudentWorld()->playSound(SOUND_ECOLI_DIE);
	}

	else {
		getStudentWorld()->playSound(SOUND_ECOLI_HURT);
	}
}