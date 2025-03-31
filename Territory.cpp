#include "Territory.h"

Territory::Territory() : name(""), owner(0), forces(0), position({ 0, 0 }), color(BLANK) {} // Default constructor


Territory::Territory(const std::string& name,const std::string& continent, int owner, int forces, Vector2 position, Color color)
    : name(name), continent(continent), owner(owner), forces(forces), forcesToget(0), position(position), color(color), grade(0.0) {
} // constructor with parameters



float Territory::getGrade() const { return grade; }
void Territory::setGrade(float newGrade) { grade = newGrade; }



std::string Territory::getName() const { return name; }
std::string Territory::getContinent() const { return continent; }
int Territory::getOwner() const { return owner; }
int Territory::getForces() const { return this->forces; }
Vector2 Territory::getPosition() const { return position; }
Color Territory::getColor() const { return color; }

void Territory::setOwner(int newOwner) { owner = newOwner; }
void Territory::setForces(int newForces) { forces = newForces; }

void Territory::AddForces(int amount, int currentPlayer) {
    if (owner == -1) {
        this->forces = 1;
        this->owner = currentPlayer;
        return;
    }
    if (owner == currentPlayer)
    {
        this->forces += amount;
    }
}

void Territory::changeOwner(int newOwner) {
    if (owner != -1)
    {
        return;
    }
    owner = newOwner;
}