#pragma once

#include "Axis&allies.h"
#include "Axis&alliesBoard.h"
#include "Territory.h"
#include "StateMachine.h"
#include <vector>

enum class BotState {
	Fight,
	giveUp
};

class Bot {

public:
	Bot(AxisBoard* board);
	
	Territory* chosenTerritoryToInit();
	Territory* territoryToReinforce(int forcesToReinforce);
	bool attackPhase(Territory*& attackingTerritory,
		Territory*& attackedTerritory);
	bool needToAttackWithPlane();
	int howMuchForcesToDefendWith(Territory * attackedTerritory );
	bool needToMoveForcesWithPlane();



private:

	void clearGrades();
	void evaluateTerritoriesInit();
	void chooseBestTerritoryToInit();
	void chooseBestTerritoryToReinforce();
	void handleContinentThreatInit();
	void handleEmptyMap();
	void handleContinentThreatReinforce();
	void handleTerritoryUnderTheatReinforce();
	void handleTerritoryUndErencircleReinforce();
	void evaluateTerritoriesReinforce();

	bool isContinentUnderThreatInit();
	bool isMapIsEmpety();


	bool isContinentUnderThreatReinforce();
	bool isTerritoryUnderThreatReinforce();
	bool isTerritoryUndErencircle();
	bool needToAttack();



private:
	
	bool hasSufficientForces(Territory* attackingTerritory, Territory* attackedTerritory);

	Territory* chooseTerritoryToAttackFrom();
	Territory* chooseTerritoryToAttack(Territory* AttackingTerritory);

	int calculateForcesToAdd(Territory* territory, int availableForces);

	AxisBoard* board;
	StateMachine stateMachine;
	Territory* chosenTerritory;
	std::vector<std::string> territoriesUnderThreat;
	std::string threatingContinent;
	std::string threatingTerritory;
	std::vector <std::string> givingUpTerritories;
};