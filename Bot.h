#pragma once

#include "Axis&allies.h"
#include "Axis&alliesBoard.h"
#include "Territory.h"

class Bot {

public:
    Bot(RiskBoard& board);
    Bot(const Bot& other) = delete; // Delete copy constructor
    Bot& operator=(const Bot& other) = delete; // Delete copy assignment operator
private:
    RiskBoard& board;

public:
    Territory* chosenTerritoryToInit();
};