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

#define NUMBER_OF_TERRITORRIES 9


class RiskGame {
public:
    RiskBoard board;
private:
    Player player1{1};
    Player player0{0};
    int currentPlayer;
    int counterOfSelectedTerritories;
    MessageManeger messageManeger{};


    enum GamePhase { INITIALIZING, PLAYING, END_GAME };
    GamePhase currentPhase;

public:
    RiskGame();
    void RunGame();
    void handleInitializationPhase();
    void handlePlayingPhase();
    Territory* ChoosingTeritorryToAttackFrom(int * forcesToAttackWith);
    Territory* ChoosingTeritorryToAttack(Territory* chosenTeritorryToAtackFrom ,int & forcesToDefenceWith);
    void hanleReinforcement();
    void handleEndGamePhase();
    void changePlayerTurn();
    void buildMap();
};
