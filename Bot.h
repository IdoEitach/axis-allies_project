#pragma once

#include "Axis&allies.h"
#include "Axis&alliesBoard.h"
#include "Territory.h"
#include "StateMachine.h"
#include <vector>

enum class BotState {
    Fight ,
	giveUp
};

class Bot {

public:
	Bot(AxisBoard* board);
	Territory* chosenTerritoryToInit();
	Territory* territoryToReinforce(int forcesToReinforce);

private:

	void clearGrades();

	BotState botState = BotState::Fight;
    void evaluateTerritoriesInit();
    void chooseBestTerritoryToInit();
	void chooseBestTerritoryToReinforce();
    void handleContinentThreatInit();
	void handleEmptyMap();


	void reinforceTerritory();
	void handleContinentThreatReinforce();
	void handleTerritoryUndErencircleReinforce();
	void evaluateTerritoriesReinforce();
    
    bool isContinentUnderThreatInit();
	bool isMapIsEmpety();
	

	bool isContinentUnderThreatReinforce();
	bool isTerritoryUnderThreatReinforce();
	bool isTerritoryUndErencircle();


	bool needToAttack();


	int calculateForcesToAdd(Territory* territory, int availableForces);

    AxisBoard* board;
    StateMachine stateMachine;
    Territory* chosenTerritory;
	std::string threatingContinent;
	std::string threatingTerritory;
	std::vector <std::string> givingUpTerritories;
};