#pragma once

#include "Axis&allies.h"
#include "Axis&alliesBoard.h"
#include "Territory.h"
#include "StateMachine.h"

class Bot {

public:
	Bot(RiskBoard* board);
	Territory* chosenTerritoryToInit();

private:
    void init();
    void evaluateTerritories();
    void chooseBestTerritory();
    void handleContinentThreat();
	void handleEmptyMap();
    void done();

    bool isContinentUnderThreat();
	bool isMapIsEmpety();

    RiskBoard* board;
    StateMachine stateMachine;
    Territory* chosenTerritory;
	std::string threatingContinent;
};