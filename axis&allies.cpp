#include "Axis&allies.h"

RiskGame::RiskGame() {
	buildMap();
}

/// <summary>
/// runnig the game based on the current phase
/// </summary>
void RiskGame::RunGame() {
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
static int compare(const void* a, const void* b) {
	int int_a = *((int*)a);
	int int_b = *((int*)b);
	return (int_b - int_a);
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
		std::cout << "Do you want to attack? (y/n): ";
		std::getline(std::cin, input);
		keepAttacking = (input == "y");
		std::cout << "keep attacking is: " << keepAttacking << std::endl;

		while (keepAttacking) {
			board.setPhase(Phase::ChooseTerritoryToAttackFrom);
			int forcesToAttackWith , forcesToDefenceWith;
			std::cout << "keep attacking is: " << keepAttacking << std::endl;
			Territory* chosenToAttackFrom = ChoosingTeritorryToAttackFrom(&forcesToAttackWith);
			std::cout << "the forces to attack with are: " << forcesToAttackWith << std::endl;
			board.setPhase(Phase::ChooseTerritoryToAttack);
			Territory* chosenToAttack = ChoosingTeritorryToAttack(chosenToAttackFrom , forcesToDefenceWith);

			int valueAttackers[3] = { 0 };
			int valueDefenders[2] = { 0 };
			board.RollCubes();


			for (int i = 0; i < forcesToAttackWith; i++) {
				valueAttackers[i] = GetRandomValue(1, 6);
			}

			qsort(valueAttackers, forcesToAttackWith, sizeof(int), compare);
			for (int i = 0; i < forcesToAttackWith; i++) {
				std::cout << "the value of the attacker dice number " << i << " is: " << valueAttackers[i] << std::endl;
			}

			for (int i = 0; i < forcesToDefenceWith; i++) {
				valueDefenders[i] = GetRandomValue(1, 6);
			}

			qsort(valueDefenders, forcesToDefenceWith, sizeof(int), compare);
			for (int i = 0; i < forcesToDefenceWith; i++) {
				std::cout << "the value of the defender dice number " << i << " is: " << valueDefenders[i] << std::endl;
			}

			std::string input;
			std::cout << "Do you want to keep attacking? (y/n): ";
			std::getline(std::cin, input);
			keepAttacking = (input == "y");
			std::cout << "keep attacking is: " << keepAttacking << std::endl;
		}
	}
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

		messageManeger.drawMessages();
		board.drawChoosingTerritoryToAttackFrom();
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
				board.drawBoard();
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
Territory* RiskGame::ChoosingTeritorryToAttack(Territory* chosenTeritorryToAtackFrom ,int& forcesToDefenceWith) {
	float deltaTime = 0;
	bool clickedOnTerritory = false;
	Territory* clickTerritoryPtr = board.checkClick();
	while (clickTerritoryPtr == nullptr && !clickedOnTerritory) {
		deltaTime = GetFrameTime();
		messageManeger.updateMessages(deltaTime);
		messageManeger.drawMessages();
		BeginDrawing();
		ClearBackground(RAYWHITE);
		board.drawChoosingTerritoryToAttackFrom();
		clickTerritoryPtr = board.checkClick();
		if (clickTerritoryPtr != nullptr && clickTerritoryPtr->getOwner() == currentPlayer) {
			clickTerritoryPtr = nullptr;
			messageManeger.addMessage("You can only attack enemies territories", 2.0f);
		}
		else if (clickTerritoryPtr != nullptr && clickTerritoryPtr->getOwner() != currentPlayer) {
			for (auto& neighbor : board.adjacencyList[chosenTeritorryToAtackFrom->getName()]) {
				if (neighbor == clickTerritoryPtr->getName()) {
					forcesToDefenceWith = board.getInput(clickTerritoryPtr);
					if(forcesToDefenceWith <= clickTerritoryPtr->getForces() )
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
	this->board.addTerritory("NORTH_WEST_TERRITORY", -1, 0, { 200, 120 }, YELLOW);
	this->board.addTerritory("GREENLAND", -1, 0, { 500, 80 }, YELLOW);
	this->board.addTerritory("ONTARIO", -1, 0, { 300, 200 }, YELLOW);
	this->board.addTerritory("QUEBEC", -1, 0, { 400, 200 }, YELLOW);

	// Define South America (Orange)
	this->board.addTerritory("VENEZUELA", -1, 0, { 300, 400 }, ORANGE);
	this->board.addTerritory("BRAZIL", -1, 0, { 350, 500 }, ORANGE);
	this->board.addTerritory("PERU", -1, 0, { 300, 550 }, ORANGE);
	this->board.addTerritory("ARGENTINA", -1, 0, { 350, 600 }, ORANGE);

	// Define borders (example connections)
	this->board.addBorder("ALASKA", "NORTH_WEST_TERRITORY");
	this->board.addBorder("NORTH_WEST_TERRITORY", "ONTARIO");
	this->board.addBorder("ONTARIO", "QUEBEC");
	this->board.addBorder("ONTARIO", "GREENLAND");
	this->board.addBorder("QUEBEC", "GREENLAND");
	this->board.addBorder("VENEZUELA", "BRAZIL");
	this->board.addBorder("BRAZIL", "PERU");
	this->board.addBorder("PERU", "ARGENTINA");
#pragma endregion
}

int main() {
	float screen_width = 1088 /*GetScreenHeight() */;
	float  screen_height = 779.875/*GetScreenWidth()*/;
	InitWindow(screen_width, screen_height, "Risk Game Map");
	SetTargetFPS(140);

	//ToggleFullscreen();


	RiskGame game;

	game.board.displayLoadingScreen();

	game.RunGame();

	return 0;
}

