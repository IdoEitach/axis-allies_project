#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>
#include "raylib.h"
#include "Territory.h"
#include "Dice.h"

enum Phase {
    InitializingTerritories,
    ReinforcingChooseTerritory,
	ReinforcingAddForces,
    ChooseTerritoryToAttackFrom,
    ChooseTerritoryToAttack,
    MovingForcesFrom,
	MovingForcesTo,
    EndGame
};
class RiskBoard {
private:
    std::unordered_map<std::string, Territory> territories;
    Territory* clickedTerritory{ new Territory("", 0, 0, {0, 0}, {0, 0, 0, 0}) };
public:
    std::unordered_map<std::string, std::vector<std::string>> adjacencyList;
private:
	Phase currentPhase = InitializingTerritories;

public:
    void addTerritory(const std::string& territoryName, int owner, int forces, Vector2 position, Color color);
    void addBorder(const std::string& territoryName1, const std::string& territoryName2);
    void displayLoadingScreen();

	void drawInitPhase();
	void drawChoosingTerritoryToReinforce();
	int getInput(Territory* clickedTerritoryPtr);
    bool hasAdjacentEnemies(const std::string& territoryName, int currentPlayer) const;
	void drawChoosingTerritoryToAttackFrom();
    Territory* getTerritoryByName(const std::string& territoryName) const;
	void drawChoosingTerritoryToAttack();
    void drawBoard();
    void drawForcesInfo();
    Territory* checkClick();
    void setPhase(Phase newPhase);
    Phase getPhase() const;
    void RollCubes();
};