#pragma once

#include "Axis&allies.h"
#include "Axis&alliesBoard.h"
#include "Territory.h"

class Bot {

public:
    Bot(RiskBoard* board);
    
private:
    RiskBoard* board;

public:
    Territory* chosenTerritoryToInit();
};