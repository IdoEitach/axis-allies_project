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
    Territory* clickedTerritory{ new Territory("", 0, 0, {0, 0}, {0, 0, 0, 0}) };
    Texture2D backgroundTexture;
    Texture2D territoryTexture;
public:
    std::unordered_map<std::string, Territory> territories;
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
    Territory* getTerritoryByName(const std::string& territoryName) const;
	void drawChoosingTerritoryToAttackFrom();
	void drawChoosingTerritoryToAttack();
	void drawChoosingTerritoryToMoveFrom();
	void drawChoosingTerritoryToMoveTo();
    void drawBoard();
    void drawForcesInfo();
    Territory* checkClick();
    void setPhase(Phase newPhase);
    Phase getPhase() const;
    void RollCubes();
    // Method to load textures
    void loadTextures();
    bool drawYesNoMessageBox(const std::string& question);
    // Method to unload textures
    void unloadTextures();
};