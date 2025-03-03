#pragma once
#include "raylib.h"
#include <string>
#include <vector>
#include <iostream>

class Territory {
private:
    std::string name;
    int owner; // 0 for PC, 1 for player
    int forces;
    Vector2 position;
    Color color;
	float grade;

public:
    Territory();
    Territory(const std::string& name, int owner, int forces, Vector2 position, Color color);

	float getGrade() const;
	void setGrade(float newGrade);
    std::string getName() const;
    int getOwner() const;
    int getForces() const;
    Vector2 getPosition() const;
    Color getColor() const;

    void setOwner(int newOwner);
    void setForces(int newForces);

    void AddForces(int amount, int currentPlayer);
    void changeOwner(int newOwner);
};


