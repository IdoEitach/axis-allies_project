#include "Axis&allies.h"


RiskGame::RiskGame()  { 
	buildMap();
}

/// <summary>
/// runnig the game based on the current phase
/// </summary>
void RiskGame::RunGame() {

	currentPlayer = 0;
	counterOfSelectedTerritories = 0;
	currentPhase = INITIALIZING;
	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);

		switch (currentPhase) {
		case INITIALIZING:
			handleInitializationPhase();
			break;

		case PLAYING:
			handlePlayingPhase();
			break;

		case END_GAME:
			handleEndGamePhase();
			break;
		}
		EndDrawing();
	}
}
void RiskGame::handleInitializationPhase() {

	board.drawBoard();
	board.drawInitPhase();
	// Check if all territories have been assigned  
	if (counterOfSelectedTerritories == NUMBER_OF_TERRITORRIES) {
		board.setPhase(Phase::ReinforcingChooseTerritory);
		currentPhase = PLAYING;
		return;
	}
	// Handle clicks to assign forces
	Territory* clickedTerritoryPtr = board.checkClick();
	while (clickedTerritoryPtr != nullptr && clickedTerritoryPtr->getOwner() == -1) {

		clickedTerritoryPtr->AddForces(0, currentPlayer);

		if (currentPlayer == 1) {

			player1.addForces(1);
			player1.addTerritory(clickedTerritoryPtr->getName());
		}
		else {
			/*clickedTerritoryPtr = bot.chosenTerritoryToInit();*/
			player0.addForces(1);
			player0.addTerritory(clickedTerritoryPtr->getName());
		}
		changePlayerTurn();
		counterOfSelectedTerritories++;

		if (counterOfSelectedTerritories == NUMBER_OF_TERRITORRIES) {
			std::cout << "  " << std::endl;
			board.setPhase(Phase::ReinforcingChooseTerritory);
			currentPhase = PLAYING;
			return;
		}
	}
}
void RiskGame::handlePlayingPhase() {
	if (currentPlayer == 0) {
		std::cout << "in player 0 game phase" << std::endl;
		player0.setAmountOfForcesToAdd();
	}
	else {
		bool keepAttacking = true;
		float deltaTime = 0;
		hanleReinforcement();
		std::string input;
		keepAttacking = board.drawYesNoMessageBox("Do you want to attack?");
		std::cout << "keep attacking is: " << keepAttacking << std::endl;

		while (keepAttacking) {
			board.setPhase(Phase::ChooseTerritoryToAttackFrom);
			int forcesToAttackWith, forcesToDefenceWith;
			std::cout << "keep attacking is: " << keepAttacking << std::endl;
			Territory* chosenToAttackFrom = ChoosingTeritorryToAttackFrom(&forcesToAttackWith);
			std::cout << "the forces to attack with are: " << forcesToAttackWith << std::endl;
			board.setPhase(Phase::ChooseTerritoryToAttack);
			Territory* chosenToAttack = ChoosingTeritorryToAttack(chosenToAttackFrom, forcesToDefenceWith);

			std::vector <int> valueAttackers;
			std::vector <int> valueDefenders;

			board.RollCubes();

			std::cout << "the forces to attack with are: " << forcesToAttackWith << std::endl;
			std::cout << "the forces to defence with are: " << forcesToDefenceWith << std::endl;

			for (int i = 0; i < forcesToAttackWith; i++) {
				valueAttackers.push_back(GetRandomValue(1, 6));
			}

			std::sort(valueAttackers.begin(), valueAttackers.end(), std::greater<int>());
			for (int i = 0; i < forcesToAttackWith; i++) {
				std::cout << "the value of the attacker dice number " << i << " is: " << valueAttackers[i] << std::endl;
			}

			for (int i = 0; i < forcesToDefenceWith; i++) {
				valueDefenders.push_back(GetRandomValue(1, 6));
			}
			std::sort(valueDefenders.begin(), valueDefenders.end(), std::greater<int>());
			for (int i = 0; i < forcesToDefenceWith; i++) {
				std::cout << "the value of the defender dice number " << i << " is: " << valueDefenders[i] << std::endl;
				if (valueAttackers[i] > valueDefenders[i]) {
					chosenToAttack->AddForces(-1, chosenToAttack->getOwner());
					currentPlayer == player0.getId() ? player1.addForces(-1) : player0.addForces(-1);
				}
				else {
					chosenToAttackFrom->AddForces(-1, chosenToAttackFrom->getOwner());
					currentPlayer == player0.getId() ? player0.addForces(-1) : player1.addForces(-1);
				}
			}
			if (chosenToAttack->getForces() == 0) {
				std::cout << "haha we occuipied ur territory " << std::endl;
				chosenToAttack->setOwner(currentPlayer);
				chosenToAttack->AddForces(forcesToAttackWith, currentPlayer);
				chosenToAttackFrom->AddForces(-forcesToAttackWith, currentPlayer);
				currentPlayer == player0.getId() ? player0.addTerritory(chosenToAttack->getName()) : player1.addTerritory(chosenToAttack->getName());
			}
			keepAttacking = board.drawYesNoMessageBox("Do you want to keep attacking?");
			std::cout << "keep attacking is: " << keepAttacking << std::endl;
		}
	}

	if (board.drawYesNoMessageBox("Do you want to move forces from ur territory ?")) {
		std::cout << "moving forces" << std::endl;
		board.setPhase(Phase::MovingForcesFrom);
		Territory* chosenToMoveFrom = chossingTerritoryToMoveFrom();
		std::cout << "the chosen territory to move from is: " << chosenToMoveFrom->getName() << std::endl;
		board.setPhase(Phase::MovingForcesTo);
	}
	changePlayerTurn();
}
Territory* RiskGame::chossingTerritoryToMoveFrom() {
	float deltaTime = 0;

	Territory* clickTerritoryPtr = board.checkClick();

	while (clickTerritoryPtr == nullptr) {
		deltaTime = GetFrameTime();
		messageManeger.updateMessages(deltaTime);
		BeginDrawing();
		board.drawChoosingTerritoryToMoveFrom();
		messageManeger.drawMessages();
		clickTerritoryPtr = board.checkClick();
		if (clickTerritoryPtr != nullptr) {
			if (clickTerritoryPtr->getOwner() != currentPlayer) {
				clickTerritoryPtr = nullptr;
				messageManeger.addMessage("You can only move from your own territories", 2.0f);
			}
			else if (clickTerritoryPtr->getForces() <= 1) {
				clickTerritoryPtr = nullptr;
				messageManeger.addMessage("You can only move from territories with more than one force ", 2.0f);
			}
			else {
				if (!board.hasAdjacentEnemies(clickTerritoryPtr->getName(), currentPlayer ^ 1)) {
					clickTerritoryPtr = nullptr;
					messageManeger.addMessage("You can only move from territories with adjacent ownedTerritories", 2.0f);
				}
			}
		}
		EndDrawing();
	}

	return clickTerritoryPtr;
}
/// <summary>
/// this function is to choose the territory to attack from and it choosing how many forces to attack with
/// </summary>
/// <param name="chosenTeritorryToAtackFrom"></param>
/// <returns></returns>
Territory* RiskGame::ChoosingTeritorryToAttackFrom(int* forcesToAttackWith) {
	float deltaTime = 0;
	bool clickedOnTerritory = false;

	bool flag = false;
	Territory* clickTerritoryPtr = board.checkClick();

	while (clickTerritoryPtr == nullptr && !clickedOnTerritory) {
		deltaTime = GetFrameTime();
		messageManeger.updateMessages(deltaTime);
		BeginDrawing();
		board.drawChoosingTerritoryToAttackFrom();
		messageManeger.drawMessages();
		clickTerritoryPtr = board.checkClick();
		if (clickTerritoryPtr != nullptr) {
			if (clickTerritoryPtr->getOwner() != currentPlayer) {
				clickTerritoryPtr = nullptr;
				messageManeger.addMessage("You can only attack from your own territories", 2.0f);
			}
			else if (clickTerritoryPtr->getForces() <= 1) {
				clickTerritoryPtr = nullptr;
				messageManeger.addMessage("You can only attack from territories with more than 1 force", 2.0f);
			}
			else {
				if (!board.hasAdjacentEnemies(clickTerritoryPtr->getName(), currentPlayer)) {
					clickTerritoryPtr = nullptr;
					messageManeger.addMessage("You can only attack from territories with adjacent enemies", 2.0f);
				}
			}
		}
		if (clickTerritoryPtr != nullptr) {
			while (true) {

				deltaTime = GetFrameTime();
				messageManeger.updateMessages(deltaTime);
				BeginDrawing();
				board.drawChoosingTerritoryToAttackFrom();
				messageManeger.drawMessages();
				*forcesToAttackWith = board.getInput(clickTerritoryPtr);
				if (clickTerritoryPtr != nullptr && clickTerritoryPtr->getForces() <= *forcesToAttackWith) {
					messageManeger.addMessage("You can only attack with less forces than the territory has", 2.0f);
					std::cout << "You can only attack with less forces than the territory has" << std::endl;
				}
				else if (clickTerritoryPtr != nullptr && (*forcesToAttackWith > 3 || *forcesToAttackWith <= 0)) {
					messageManeger.addMessage("You can only attack with up to 3 forces", 2.0f);
					std::cout << "You can only attack with up to 3 forces" << std::endl;
				}
				else {
					return clickTerritoryPtr;
				}
			}
		}
		EndDrawing();
	}
	return nullptr;
}
Territory* RiskGame::ChoosingTeritorryToAttack(Territory* chosenTeritorryToAtackFrom, int& forcesToDefenceWith) {
	float deltaTime = 0;
	bool clickedOnTerritory = false;
	Territory* clickTerritoryPtr = board.checkClick();
	while (clickTerritoryPtr == nullptr && !clickedOnTerritory) {
		deltaTime = GetFrameTime();
		messageManeger.updateMessages(deltaTime);
		BeginDrawing();
		board.drawChoosingTerritoryToAttack();
		messageManeger.drawMessages();
		clickTerritoryPtr = board.checkClick();
		if (clickTerritoryPtr != nullptr && clickTerritoryPtr->getOwner() == currentPlayer) {
			clickTerritoryPtr = nullptr;
			messageManeger.addMessage("You can only attack enemies territories", 2.0f);
			std::cout << "You can only attack enemies territories" << std::endl;
		}
		else if (clickTerritoryPtr != nullptr && clickTerritoryPtr->getOwner() != currentPlayer) {
			for (auto& neighbor : board.adjacencyList[chosenTeritorryToAtackFrom->getName()]) {
				if (neighbor == clickTerritoryPtr->getName()) {
					forcesToDefenceWith = board.getInput(clickTerritoryPtr);
					if (forcesToDefenceWith <= clickTerritoryPtr->getForces())
						return clickTerritoryPtr;
				}
			}
			clickTerritoryPtr = nullptr;
			messageManeger.addMessage("You can only attack territories that are adjacent to the one you chose", 2.0f);
		}
		EndDrawing();
	}

	return clickTerritoryPtr;
}
void RiskGame::hanleReinforcement() {
	float deltaTime = 0;
	std::cout << "in player 1 game phase" << std::endl;
	player1.setAmountOfForcesToAdd();

	while (player1.getAmountOfForcesToAdd() > 0) {
		deltaTime = GetFrameTime();

		BeginDrawing();
		ClearBackground(RAYWHITE);
		board.drawBoard();
		board.drawForcesInfo();
		board.setPhase(Phase::ReinforcingChooseTerritory);
		messageManeger.updateMessages(deltaTime);
		messageManeger.drawMessages();

		Territory* clickedTerritoryPtr = board.checkClick();

		if (clickedTerritoryPtr != nullptr) {
			// Redraw the board in each iteration
			board.drawBoard();
			if (clickedTerritoryPtr->getOwner() == 1) {
				board.setPhase(Phase::ReinforcingAddForces);
				int forces = board.getInput(clickedTerritoryPtr);
				std::cout << "the amount left to add is :" << player1.getAmountOfForcesToAdd() << std::endl;

				player1.reinForcement(forces, clickedTerritoryPtr); // reinforeme
				std::cout << "remaining" << player1.getAmountOfForcesToAdd() << std::endl;
				std::cout << "the amount left to add is :" << player1.getAmountOfForcesToAdd() << std::endl;
				std::cout << "the forces of the clicked territory are: " << clickedTerritoryPtr->getForces() << std::endl;
				EndDrawing();
			}
			else
			{
				messageManeger.addMessage("You can only add forces to your territories", 2.0f);
			}
		}
		EndDrawing();
	}
}
void RiskGame::handleEndGamePhase() {
	board.drawBoard();
	board.setPhase(Phase::EndGame);
}
/// <summary>
///  change the player turn
/// </summary>
void RiskGame::changePlayerTurn() {
	currentPlayer = currentPlayer ^ 1;
}
void RiskGame::buildMap() {
#pragma region InitMap
	// Define North America (Yellow)
	this->board.addTerritory("ALASKA", -1, 0, { 100, 100 }, YELLOW);
	this->board.addTerritory("NORTH_WEST", -1, 0, { 200, 120 }, YELLOW);
	this->board.addTerritory("GREENLAND", -1, 0, { 500, 80 }, YELLOW);
	this->board.addTerritory("ONTARIO", -1, 0, { 300, 250 }, YELLOW);
	this->board.addTerritory("QUEBEC", -1, 0, { 400, 200 }, YELLOW);

	// Define South America (Orange)
	this->board.addTerritory("VENEZUELA", -1, 0, { 300, 400 }, ORANGE);
	this->board.addTerritory("BRAZIL", -1, 0, { 350, 500 }, ORANGE);
	this->board.addTerritory("PERU", -1, 0, { 300, 550 }, ORANGE);
	this->board.addTerritory("ARGENTINA", -1, 0, { 350, 600 }, ORANGE);

	// Define borders (example connections)
	this->board.addBorder("ALASKA", "NORTH_WEST");
	this->board.addBorder("NORTH_WEST", "ONTARIO");
	this->board.addBorder("ONTARIO", "QUEBEC");
	this->board.addBorder("ONTARIO", "GREENLAND");
	this->board.addBorder("QUEBEC", "GREENLAND");
	this->board.addBorder("VENEZUELA", "BRAZIL");
	this->board.addBorder("BRAZIL", "PERU");
	this->board.addBorder("PERU", "ARGENTINA");
	this->board.addBorder("BRAZIL", "ARGENTINA");
	this->board.addBorder("VENEZUELA", "ONTARIO");
	this->board.addBorder("VENEZUELA", "QUEBEC");

#pragma endregion
}

int main() {
	float screen_width = 1088 /*GetScreenHeight()*/;
	float  screen_height = 779.875/*GetScreenWidth()*/;
	InitWindow(screen_width, screen_height, "Risk Game Map");
	SetTargetFPS(140);

	RiskGame game;
	game.board.loadTextures(); // Load textures

	game.board.displayLoadingScreen();
	game.RunGame();

	game.board.unloadTextures(); // Unload textures
	CloseWindow(); // Close window and OpenGL context

	return 0;
}
