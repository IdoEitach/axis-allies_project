#pragma once

#include "Axis&allies.h"
#include "Axis&alliesBoard.h"
#include "Territory.h"
#include "StateMachine.h"

class Bot {

public:
	Bot(AxisBoard* board);
	Territory* chosenTerritoryToInit();
	Territory* chosenTerritoryToReinforce();


private:
    void evaluateTerritoriesInit();
    void chooseBestTerritory();
    void handleContinentThreatInit();
	void handleEmptyMap();
	void reinforceTerritory();
	void handleContinentThreatReinforce();
    


    bool isContinentUnderThreatInit();
	bool isMapIsEmpety();

	bool isContinentUnderThreatReinforce();
	bool isTerritoryUnderThreatReinforce();

    AxisBoard* board;
    StateMachine stateMachine;
    Territory* chosenTerritory;
	std::string threatingContinent;
	std::string threatingTerritory;
};