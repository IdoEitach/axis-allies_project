#pragma once
#include <raylib.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include "Territory.h"
#include "Player.h"
#include "Axis&alliesBoard.h"
#include "MessageManeger.h"
#include "Dice.h"
#include <algorithm>
#include "Bot.h" 
#include <cstdlib> 
#include <ctime>   


class Bot;

#define NUMBER_OF_TERRITORRIES 9

class AxisGame{
public:
    AxisBoard board;

private:
    Player player1{ 1 };
    Player player0{ 0 };
    int currentPlayer;
    int counterOfSelectedTerritories;
    MessageManeger messageManeger{};
    Bot* bot; // Bot pointer declaration
    enum GamePhase { INITIALIZING, PLAYING, END_GAME };
    GamePhase currentPhase;

public:
    AxisGame();
    ~AxisGame();
    void RunGame();
    void handleInitializationPhase();
    void handlePlayingPhase();
    Territory* ChoosingTeritorryToAttackFrom(int* forcesToAttackWith);
    Territory* ChoosingTeritorryToAttack(Territory* chosenTeritorryToAtackFrom, int& forcesToDefenceWith);
    Territory* chossingTerritoryToMoveFrom();
    Territory* ChoosingTeritorryToMoveTo(Territory* chosenTeritorryToMoveFrom);
    void hanleReinforcement();
    void handleEndGamePhase();
    void changePlayerTurn();
    void buildMap();
    void attackWithPlane();
	bool checkVictory();

	void moveForcesWithPlane(Territory* territoryToMoveFrom ,int amountToMove);

	
};
