#include "Axis&allies.h"


// Update the constructor to initialize the bot
AxisGame::AxisGame() : board(), bot(new Bot(&board)) { // Initialize bot with pointer to board
	// Other initializations
	buildMap();
}

AxisGame::~AxisGame() {
	delete bot; // Clean up bot
}


/// <summary>
/// runnig the game based on the current phase
/// </summary>
void AxisGame::RunGame() {

	currentPlayer = 0;
	counterOfSelectedTerritories = 0;
	currentPhase = INITIALIZING;
	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);

		switch (currentPhase) {
		case INITIALIZING:
			std::cout << currentPlayer << std::endl;
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

/// <summary>
/// This function is handling the initialization phase
/// </summary>
void AxisGame::handleInitializationPhase() {

	board.drawBoard();
	board.drawInitPhase();
	// Check if all territories have been assigned  
	if (counterOfSelectedTerritories == NUMBER_OF_TERRITORRIES) {
		board.setPhase(Phase::ReinforcingChooseTerritory);
		currentPhase = PLAYING;
		currentPlayer = 0;
		EndDrawing();
		return;
	}
	// Handle clicks to assign forces
	Territory* clickedTerritoryPtr = nullptr;
	if (currentPlayer == 0)
	{
		clickedTerritoryPtr = bot->chosenTerritoryToInit();
		if (clickedTerritoryPtr != nullptr) {

			player0.addForces(1);
			player0.addTerritory(clickedTerritoryPtr->getName());
			clickedTerritoryPtr->setOwner(0);
			clickedTerritoryPtr->AddForces(1, 0);
			changePlayerTurn();
			std::cout << "in pc init phase" << std::endl;
			counterOfSelectedTerritories++;
		}
		return;
	}
	while (clickedTerritoryPtr == nullptr) {
		board.drawBoard();
		board.drawInitPhase();
		float deltaTime = GetFrameTime();
		messageManeger.updateMessages(deltaTime);
		BeginDrawing();
		board.drawInitPhase();
		messageManeger.drawMessages();
		clickedTerritoryPtr = board.checkClick();
		if (clickedTerritoryPtr != nullptr) {
			if (clickedTerritoryPtr->getOwner() == -1) {
				clickedTerritoryPtr->setOwner(currentPlayer);
				clickedTerritoryPtr->AddForces(1, currentPlayer);
				if (currentPlayer == 1) {
					player1.addForces(1);
				}
			}
			else {
				clickedTerritoryPtr = nullptr;
				messageManeger.addMessage("You can only select unowned territories", 2.0f);
			}
		}
		EndDrawing();
	}
	changePlayerTurn();
	counterOfSelectedTerritories++;

	if (counterOfSelectedTerritories == NUMBER_OF_TERRITORRIES) {
		std::cout << "  " << std::endl;
		board.setPhase(Phase::ReinforcingChooseTerritory);
		currentPlayer = 0; // The Pc is the first player
		currentPhase = PLAYING;
		return;
	}
}


/// <summary>
/// This function is handling the playing phase
/// </summary>
void AxisGame::handlePlayingPhase() {
	if (currentPlayer == 0) {

		// this is the bot playing turn 
		// first its reinforcement phase
		// then its attack phase
		std::cout << "in player 0 game phase" << std::endl;
		player0.setAmountOfForcesToAdd(board);
		int amountOfForcesToAdd = player0.getAmountOfForcesToAdd();
		// reinforcement phase
		bot->territoryToReinforce(amountOfForcesToAdd);
		std::cout << "Reinforced" << std::endl;


		bool keepAttacking = true;
		Territory* attackingTerritory = nullptr;
		Territory* attackedTerritory = nullptr;

		while ((bot->attackPhase(attackingTerritory, attackedTerritory))) {
			// Attack
			std::cout << "Attacking..." << std::endl;

			if (attackingTerritory != nullptr && attackedTerritory != nullptr) {

				board.setPhase(Phase::ChooseTerritoryToAttack);
				int forcesToDefenceWith = board.getInput(attackedTerritory);
				while (forcesToDefenceWith > 2 || forcesToDefenceWith > attackedTerritory->getForces()) {
					forcesToDefenceWith = board.getInput(attackedTerritory);
					if (forcesToDefenceWith > attackedTerritory->getForces())
					{
						std::cout << "u cant defend with more forces than u have " << std::endl;
					}

				}

				std::vector<int> valueAttackers;
				std::vector<int> valueDefenders;

				board.RollCubes();

				int amountToAttackWith = attackingTerritory->getForces() > 3 ? 3 : attackingTerritory->getForces() - 1;

				for (int i = 0; i < amountToAttackWith; i++) {
					valueAttackers.push_back(GetRandomValue(1, 6));
				}

				std::sort(valueAttackers.begin(), valueAttackers.end(), std::greater<int>());

				for (int i = 0; i < amountToAttackWith; i++) {
					std::cout << "the value of the attacker dice number " << i << " is: " << valueAttackers[i] << std::endl;
				}

				for (int i = 0; i < forcesToDefenceWith; i++) {
					valueDefenders.push_back(GetRandomValue(1, 6));
				}
				std::sort(valueDefenders.begin(), valueDefenders.end(), std::greater<int>());

				for (int i = 0; i < forcesToDefenceWith; i++) {
					std::cout << "the value of the defender dice number " << i << " is: " << valueDefenders[i] << std::endl;
					if (valueAttackers[i] > valueDefenders[i]) {
						attackedTerritory->AddForces(-1, attackedTerritory->getOwner());
						player1.addForces(-1);
					}
					else {
						attackingTerritory->AddForces(-1, attackingTerritory->getOwner());
						player0.addForces(-1);
					}
				}

				if (attackedTerritory->getForces() == 0) {
					attackedTerritory->setOwner(0);
					attackedTerritory->AddForces(amountToAttackWith, 0);
					attackingTerritory->AddForces(-amountToAttackWith, 0);
					player0.addTerritory(attackedTerritory->getName());
					std::cout << "haha we occuipied ur territory " << std::endl;
					if (checkVictory()) {
						std::cout << "Player 0 wins!" << std::endl;
						currentPhase = END_GAME;
						return;
					}
				}

			}
		}
		if (!player0.getAttackedWithPlane()) {
			if (bot->needToAttackWithPlane())
			{
				attackWithPlane();
				std::cout << "attacking with plane" << std::endl;
				player0.setAttackedWithPlane(true);
			}
		}
	}
	else {
		bool keepAttacking = true;
		float deltaTime = 0;
		//reinforcement phase
		hanleReinforcement();
		std::string msg;
		std::string input;
		keepAttacking = board.drawYesNoMessageBox("Do you want to attack?");
		std::cout << "keep attacking is: " << keepAttacking << std::endl;

		// attack phase
		while (keepAttacking) {
			// does the player want to attack with plane
			if (!player1.getAttackedWithPlane()) {
				if (board.drawYesNoMessageBox("Do u want to attack with plane ?")) {
					attackWithPlane();
					std::cout << "attacking with plane" << std::endl;
					player1.setAttackedWithPlane(true);
				}
			}
			//choose territory to attack from
			board.setPhase(Phase::ChooseTerritoryToAttackFrom);
			int forcesToAttackWith, forcesToDefenceWith;
			std::cout << "keep attacking is: " << keepAttacking << std::endl;
			Territory* chosenToAttackFrom = ChoosingTeritorryToAttackFrom(&forcesToAttackWith);
			std::cout << "the forces to attack with are: " << forcesToAttackWith << std::endl;
			board.setPhase(Phase::ChooseTerritoryToAttack);
			//choose territory to attack
			Territory* chosenToAttack = ChoosingTeritorryToAttack(chosenToAttackFrom, forcesToDefenceWith);

			std::vector <int> valueAttackers;
			std::vector <int> valueDefenders;
			//rolling the cubes and getting who is the winner of the battle
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
				msg = "You have conquered " + chosenToAttack->getName() + "!";
				chosenToAttack->setOwner(currentPlayer);
				chosenToAttack->AddForces(forcesToAttackWith, currentPlayer);
				chosenToAttackFrom->AddForces(-forcesToAttackWith, currentPlayer);
				player1.addTerritory(chosenToAttack->getName());
				if (checkVictory()) {
					std::cout << "Player 1 wins!" << std::endl;
					currentPhase = END_GAME;
					return;
				}
			}
			else {
				msg = "You have lost the battle for and lost forces " + chosenToAttack->getName() + "!";
			}
			std::cout << "sadadsadsadasdasdasd: " << keepAttacking << std::endl;

			keepAttacking = board.drawYesNoMessageBox("Do you want to keep attacking? \n "+msg);
			std::cout << "keep attacking is: " << keepAttacking << std::endl;
		}

		// ask the 
		if (!player1.getMovedWithPlane()) {

			if (board.drawYesNoMessageBox("Do you want to move forces from ur territory with plane ?")) {
				std::cout << "moving forces with plane" << std::endl;
				board.setPhase(Phase::MovingForcesFrom);
				Territory* chosenToMoveFrom = chossingTerritoryToMoveFrom();
				std::cout << "the chosen territory to move from is: " << chosenToMoveFrom->getName() << std::endl;
				int forcesToMove = board.getInput(chosenToMoveFrom);
				moveForcesWithPlane(chosenToMoveFrom, forcesToMove);
				std::cout << "the forces to move are: " << forcesToMove << std::endl;
				player1.setMovedWithPlane(true);
				std::cout << "moved forces with plane" << std::endl;

			}
		}
	}
	changePlayerTurn();
	std::cout << "changed the currnet player \n the current player is: " << currentPlayer << std::endl;

}


/// <summary>
/// This function is handling the reinforcement phase
/// </summary>
void AxisGame::hanleReinforcement() {
	float deltaTime = 0;
	std::cout << "in player 1 game phase" << std::endl;
	player1.setAmountOfForcesToAdd(board);

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


/// <summary>
/// this function is to choose the territory to attack from and it choosing how many forces to attack with
/// </summary>
/// <param name="chosenTeritorryToAtackFrom"></param>
/// <returns></returns>
Territory* AxisGame::ChoosingTeritorryToAttackFrom(int* forcesToAttackWith) {
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


/// <summary>
/// This function is to choose the territory to move forces from
/// </summary>
/// <returns></returns>
Territory* AxisGame::chossingTerritoryToMoveFrom() {
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
/// This function is to choose Territory to attack
/// </summary>
/// <param name="chosenTeritorryToAtackFrom"></param>
/// <param name="forcesToDefenceWith"></param>
/// <returns></returns>
Territory* AxisGame::ChoosingTeritorryToAttack(Territory* chosenTeritorryToAtackFrom, int& forcesToDefenceWith) {
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
					forcesToDefenceWith = bot->howMuchForcesToDefendWith(clickTerritoryPtr);
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


/// <summary>
/// This function is meant to attack with plane.
/// it choose randomly one of the enemy territories
/// and attack it with 3 forces
/// </summary>
void AxisGame::attackWithPlane() {
	std::cout << "Attacking with a plane..." << std::endl;

	// 1. Create a list of potential target territories (all enemy territories)
	std::vector<Territory*> potentialTargets;
	for (auto& pair : board.territories) {
		Territory& territory = pair.second;
		if (territory.getOwner() != currentPlayer) { // Enemy territory
			potentialTargets.push_back(&territory);
		}
	}

	// 2. If there are no potential targets, exit the function
	if (potentialTargets.empty()) {
		std::cout << "No valid targets for plane attack." << std::endl;
		return;
	}

	// 3. Choose a random target territory from the list of potential targets
	// Seed the random number generator (only do this once at the beginning of the game)
	static bool seeded = false;
	if (!seeded) {
		srand(time(0));
		seeded = true;
	}
	int randomIndex = rand() % potentialTargets.size();
	Territory* targetTerritory = potentialTargets[randomIndex];

	// 4. Implement the attack logic (replace with your actual attack implementation)
	std::cout << "Attacking " << targetTerritory->getName() << " with a plane." << std::endl;
	// Example attack logic: Reduce the forces in the target territory
	int damage = 3; // Plane attacks with 3 forces
	targetTerritory->AddForces(-damage, targetTerritory->getOwner());
	std::cout << "The plane inflicted " << damage << " damage to " << targetTerritory->getName() << std::endl;

	// 5. Check if the territory was conquered
	if (targetTerritory->getForces() <= 0) {
		std::cout << targetTerritory->getName() << " was conquered!" << std::endl;
		// Implement logic to transfer ownership to the current player
		targetTerritory->setOwner(currentPlayer);
		targetTerritory->setForces(3);

	}

	// 6. Print the attack result
	std::cout << "Attack completed." << std::endl;
}


/// <summary>
/// This function moves forces from one territory to another using the plane.
/// The player chooses the territory to move from and the amount to move.
/// The function will randomly choose one of the player's territories and move the forces there.
/// </summary>
/// <param name="territoryToMoveFrom"></param>
/// <param name="amountToMove"></param>
void AxisGame::moveForcesWithPlane(Territory* territoryToMoveFrom, int amountToMove) {
	// Check if the territory to move from is valid
	if (territoryToMoveFrom == nullptr) {
		std::cout << "The selected territory is invalid." << std::endl;
		return;
	}

	// Check if the player owns the territory
	if (territoryToMoveFrom->getOwner() != currentPlayer) {
		std::cout << "You do not own this territory." << std::endl;
		return;
	}

	// Check if there are enough forces to move
	if (territoryToMoveFrom->getForces() < amountToMove || amountToMove <= 0) {
		std::cout << "Not enough forces to move or invalid move amount." << std::endl;
		return;
	}

	// Get a list of all territories owned by the player
	std::vector<Territory*> ownedTerritories;
	for (auto& pair : board.territories) {
		Territory& territory = pair.second;
		if (territory.getOwner() == currentPlayer && &territory != territoryToMoveFrom) {
			ownedTerritories.push_back(&territory);
		}
	}

	// Check if there are any valid territories to move to
	if (ownedTerritories.empty()) {
		std::cout << "No valid territories to move forces to." << std::endl;
		return;
	}

	// Randomly select a target territory
	int randomIndex = GetRandomValue(0, ownedTerritories.size() - 1);
	Territory* targetTerritory = ownedTerritories[randomIndex];

	// Move the forces
	territoryToMoveFrom->AddForces(-amountToMove, currentPlayer); // Deduct forces
	targetTerritory->AddForces(amountToMove, currentPlayer);      // Add forces

	// Log the move
	std::cout << "Moved " << amountToMove << " forces from "
		<< territoryToMoveFrom->getName() << " to "
		<< targetTerritory->getName() << " using the plane." << std::endl;
}

/// <summary>
/// this function is handling the end game phase
/// </summary>
void AxisGame::handleEndGamePhase() {
	board.drawBoard();
	board.setPhase(Phase::EndGame);
}


/// <summary>
///  change the player turn
/// </summary>
void AxisGame::changePlayerTurn() {
	currentPlayer = currentPlayer ^ 1;
}


bool AxisGame::checkVictory() {
	for (const auto& pair : board.territories) {
		if (pair.second.getOwner() != currentPlayer) {
			return false;
		}
	}
	return true;
}


/// <summary>
/// This function checks which continents the player owns. 
/// It iterates through all territories and checks if the player owns all territories in each continent.
/// </summary>
/// <param name="player"></param>
/// <returns>the contintes owned by the player</returns>
std::vector<std::string> AxisGame::getOwnedContinents(int player) {
	std::unordered_map<std::string, std::vector<std::string>> continents; // Map of continents and their territories
	std::vector<std::string> ownedContinents;

	// Build the continents map from the board
	for (const auto& pair : board.territories) {
		const std::string& territoryName = pair.first;
		const std::string& continentName = pair.second.getContinent();

		continents[continentName].push_back(territoryName);
	}

	// Check ownership of each continent
	for (const auto& pair : continents) {
		const std::string& continentName = pair.first;
		const std::vector<std::string>& territories = pair.second;

		bool isOwned = true;
		for (const auto& territoryName : territories) {
			if (board.territories.at(territoryName).getOwner() != player) {
				isOwned = false;
				break;
			}
		}

		if (isOwned) {
			ownedContinents.push_back(continentName);
		}
	}

	return ownedContinents;
}


/// <summary>
/// This function is to build the map
/// the map is built by adding territories and borders
/// </summary>
void AxisGame::buildMap() {
#pragma region InitMap
	// Define North America (Yellow)
	this->board.addTerritory("ALASKA", "NORTAMERICA", -1, 0, { 100, 100 }, YELLOW);
	this->board.addTerritory("NORTH_WEST", "NORTAMERICA", -1, 0, { 200, 120 }, YELLOW);
	this->board.addTerritory("GREENLAND", "NORTAMERICA", -1, 0, { 500, 80 }, YELLOW);
	this->board.addTerritory("ONTARIO", "NORTAMERICA", -1, 0, { 300, 250 }, YELLOW);
	this->board.addTerritory("QUEBEC", "NORTAMERICA", -1, 0, { 400, 200 }, YELLOW);

	// Define South America (Orange)
	this->board.addTerritory("VENEZUELA", "SOUTHAMERICA", -1, 0, { 300, 400 }, ORANGE);
	this->board.addTerritory("BRAZIL", "SOUTHAMERICA", -1, 0, { 350, 500 }, ORANGE);
	this->board.addTerritory("PERU", "SOUTHAMERICA", -1, 0, { 300, 550 }, ORANGE);
	this->board.addTerritory("ARGENTINA", "SOUTHAMERICA", -1, 0, { 350, 600 }, ORANGE);


	// Define Europe (Blue)
	this->board.addTerritory("FRANCE", "EUROPE", -1, 0, { 900, 300 }, BLUE);
	this->board.addTerritory("GERMANY", "EUROPE", -1, 0, { 1000, 300 }, BLUE);
	this->board.addTerritory("ITALY", "EUROPE", -1, 0, { 1100, 300 }, BLUE);
	this->board.addTerritory("SPAIN", "EUROPE", -1, 0, { 900, 400 }, BLUE);
	this->board.addTerritory("UK", "EUROPE", -1, 0, { 800, 300 }, BLUE);
	this->board.addTerritory("SWEDEN", "EUROPE", -1, 0, { 700, 200 }, BLUE);
	this->board.addTerritory("NORWAY", "EUROPE", -1, 0, { 600, 150 }, BLUE);
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
	this->board.addBorder("BRAZIL", "FRANCE");
	this->board.addBorder("FRANCE", "GERMANY");
	this->board.addBorder("GERMANY", "ITALY");
	this->board.addBorder("GERMANY", "SWEDEN");
	this->board.addBorder("SWEDEN", "NORWAY");
	this->board.addBorder("NORWAY", "UK");
	this->board.addBorder("NORWAY", "SWEDEN");
	this->board.addBorder("SPAIN", "FRANCE");
	this->board.addBorder("SPAIN", "GERMANY");
	this->board.addBorder("SPAIN", "ITALY");
	this->board.addBorder("SPAIN", "UK");
	this->board.addBorder("SPAIN", "NORWAY");
	this->board.addBorder("SPAIN", "BRAZIL");
	this->board.addBorder("SPAIN", "PERU");
	this->board.addBorder("SPAIN", "ARGENTINA");
	this->board.addBorder("SPAIN", "GREENLAND");

	
	
#pragma endregion
}


int main() {
	float screen_width = /*1088*/ GetScreenHeight();
	float  screen_height = /*779.875*/GetScreenWidth();
	InitWindow(screen_width, screen_height, "Axis and Allies Game Map");
	SetTargetFPS(140);

	AxisGame game;
	game.board.loadTextures(); // Load textures

	game.board.displayLoadingScreen();
	game.RunGame();

	game.board.unloadTextures(); // Unload textures
	CloseWindow(); // Close window and OpenGL context

	return 0;
}
