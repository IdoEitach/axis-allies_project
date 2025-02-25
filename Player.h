#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "Territory.h"

class Player {
private:
    int id; // 0 for PC, 1 for player
    int totalForces;
    int territoryCount;
    int amountOfForcesToAdd;
    std::vector<std::string> territoriesOwned;

public:
    Player(int id);

    int getId() const;
    int getTotalForces() const;
    int getTerritoryCount() const;
    const std::vector<std::string>& getTerritoriesOwned() const;

    void addTerritory(const std::string& territory);
    void removeTerritory(const std::string& territory);
    void addForces(int amount);
    void deductForces(int amount);
    int getAmountOfForcesToAdd();
    void setAmountOfForcesToAdd();
    void reinForcement(int amount, Territory* terrotiryPtr);

private:
    int howMuchForcesToAdd();
};
