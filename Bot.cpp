#include "Bot.h"

Bot::Bot(AxisBoard* board) : board(board), chosenTerritory(nullptr), threatingContinent("") {

}


void Bot::clearGrades() {
	for (auto& pair : board->territories) {
		Territory& territory = pair.second;
		territory.setGrade(0.0);
	}
}


int Bot::howMuchForcesToDefendWith(Territory* attackedTerritory) {
	return attackedTerritory->getForces() > 2 ? 2 : attackedTerritory->getForces();
}

/// <summary>
/// This function is choosing the territory to init
/// it is using a state machine to choose the territory
/// </summary>
/// <returns></returns>
Territory* Bot::chosenTerritoryToInit() {
	clearGrades();
	stateMachine.addTransition([this]() { return this->isMapIsEmpety(); }, [this]() { this->handleEmptyMap(); });
	stateMachine.addTransition([this]() { return this->isContinentUnderThreatInit(); }, [this]() { this->handleContinentThreatInit(); });
	stateMachine.addTransition([this]() { return true; }, [this]() { this->evaluateTerritoriesInit(); });
	stateMachine.run();
	stateMachine.clearTransitions();
	std::cout << "Chosen territory: " << chosenTerritory->getName() << std::endl;
	return chosenTerritory;
}


/// <summary>
/// Check if the continent is under threat
/// threat is if the enemy has more than half of the territories in the continent
/// </summary>
/// <returns></returns>
bool Bot::isContinentUnderThreatInit() {
	std::cout << "is the continent under threat? :" << std::endl;
	std::unordered_map<std::string, int> continentTerritoryCount;
	std::unordered_map<std::string, int> continentOwnedCount;
	std::unordered_map<std::string, int> continentEnemyCount;

	for (const auto& pair : board->territories) {
		const Territory& territory = pair.second;
		continentTerritoryCount[territory.getContinent()]++;
		if (territory.getOwner() == 0) {
			continentOwnedCount[territory.getContinent()]++;
		}
		else if (territory.getOwner() == 1) {
			continentEnemyCount[territory.getContinent()]++;
		}
	}

	for (const auto& pair : continentTerritoryCount) {
		const std::string& continent = pair.first;
		int totalTerritories = pair.second;
		std::cout << "Continent: " << continent << " the amount of territories the continent has: " << totalTerritories << std::endl;
		int enemyTerritories = continentEnemyCount[continent];

		if (enemyTerritories >= totalTerritories / 2) {
			threatingContinent = continent;
			return true;
		}
	}

	return false;
}


/// <summary>
/// Check if the map is empty
/// </summary>
/// <returns></returns>
bool Bot::isMapIsEmpety() {
	bool isMapEmpty = true;
	std::cout << "is the map is empty? :" << std::endl;
	for (const auto& pair : board->territories) {
		const Territory& territory = pair.second;
		if (territory.getOwner() != -1) {
			isMapEmpty = false;
		}
	}
	isMapEmpty ? std::cout << "The map is empty" : std::cout << "The map is not empty" << std::endl;
	return isMapEmpty;
}


/// <summary>
///  If the map is empty the bot will choose the territory "ONTARIO"
/// </summary>
void Bot::handleEmptyMap() {
	std::cout << "Handling empty map..." << std::endl;
	chosenTerritory = board->getTerritoryByName("ONTARIO");
	std::cout << "Chosen territory: " << chosenTerritory->getName() << std::endl;

}


/// <summary>
/// If the continent is under threat the bot will choose the territory 
/// with the highest grade in the continent
/// if all the territories in the continent are already 
/// occupied the bot will evaluate the territories with the generic grade function
/// </summary>
void Bot::handleContinentThreatInit() {
	std::cout << "Handling continent threat..." << std::endl;
	std::cout << "The continent under threat is: " << threatingContinent << std::endl;
	std::vector<Territory*> continentTerritories = board->getTerritoriesInContinent(threatingContinent);

	// Check if all territories in the continent are already occupied
	bool allOccupied = true;
	for (Territory* territory : continentTerritories) {
		if (territory->getOwner() == -1) {
			allOccupied = false;
		}
	}

	if (allOccupied) {
		std::cout << "All territories in the continent are already occupied." << std::endl;
		evaluateTerritoriesInit();
	}
	else {

		for (Territory* territory : continentTerritories) {
			if (territory->getOwner() == -1) {
				double grade = 0.0;
				for (const std::string& adjName : board->adjacencyList[territory->getName()]) {
					Territory& adjTerritory = board->territories[adjName];
					if (adjTerritory.getOwner() == territory->getOwner()) {
						grade += adjTerritory.getForces() * 0.5;
						grade += 2.0;
					}
					else {
						grade -= 0.2;
					}
				}
				territory->setGrade(grade);
			}
		}
		Territory* bestTerritory = nullptr;
		double highestGrade = -std::numeric_limits<double>::infinity();
		for (Territory* territory : continentTerritories) {
			if (territory->getOwner() == -1 && territory->getGrade() > highestGrade) {
				highestGrade = territory->getGrade();
				bestTerritory = territory;
			}
		}
		chosenTerritory = bestTerritory;
		stateMachine.removeTransition([this]() { return this->isContinentUnderThreatInit(); });
	}
}


/// <summary>
/// This is choosing the best territory to 
/// </summary>
void Bot::chooseBestTerritoryToInit() {
	chosenTerritory = nullptr;
	double highestGrade = -std::numeric_limits<double>::infinity();

	for (auto& pair : board->territories) {
		Territory& territory = pair.second;
		if (territory.getOwner() == -1) {
			double grade = territory.getGrade();
			if (grade > highestGrade) {
				highestGrade = grade;
				chosenTerritory = &territory;
			}
		}
	}
	if (chosenTerritory != nullptr) {
		std::cout << "Chosen territory: " << chosenTerritory->getName() << std::endl;
	}
	else {
		std::cout << "No territory chosen." << std::endl;
	}
}


/// <summary>
/// This function is evaluating the grades the bot will give to the territories for the init phase 
/// </summary>
void Bot::evaluateTerritoriesInit() {
	Territory* bestTerritory = nullptr;
	double highestGrade = -std::numeric_limits<double>::infinity();
	std::unordered_map<std::string, int> continentTerritoryCount;
	std::unordered_map<std::string, int> continentOwnedCount;

	// Calculate the number of territories and owned territories in each continent
	for (const auto& pair : board->territories) {
		const Territory& territory = pair.second;
		continentTerritoryCount[territory.getContinent()]++;
		if (territory.getOwner() == 0) { // Assuming 0 is the bot's owner ID
			continentOwnedCount[territory.getContinent()]++;
		}
	}

	for (auto& pair : board->territories) {
		Territory& territory = pair.second;
		if (territory.getOwner() == -1) {
			double grade = 0.0;
			for (const std::string& adjName : board->adjacencyList[territory.getName()]) {
				Territory& adjTerritory = board->territories[adjName];
				if (adjTerritory.getOwner() == territory.getOwner()) {
					grade += adjTerritory.getForces() * 0.5;
					grade += 2.0;
				}
				else {
					grade -= 0.2;
				}
			}
			const std::string& continent = territory.getContinent();
			int totalTerritories = continentTerritoryCount[continent];
			int ownedTerritories = continentOwnedCount[continent];
			int remainingTerritories = totalTerritories - ownedTerritories;

			// Add a higher grade to territories in continents where the bot owns a significant portion
			if (ownedTerritories > totalTerritories / 2) {
				grade += (totalTerritories - remainingTerritories) * 1.0;
			}
			else {
				grade += (totalTerritories - remainingTerritories) * 0.7;
			}

			territory.setGrade(grade);
			if (grade > highestGrade) {
				highestGrade = grade;
				bestTerritory = &territory;
			}
		}
	}

	chosenTerritory = bestTerritory;
}


/// <summary>
/// This function is choosing the territory to reinforce 
/// </summary>
/// <returns></returns>
Territory* Bot::territoryToReinforce(int forcesToReinforce) {
	std::cout << "Reinforceing  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
	while (forcesToReinforce > 0) {
		clearGrades();
		givingUpTerritories.clear();
		stateMachine.addTransition([this]() { return this->isContinentUnderThreatReinforce(); },
			[this]() { this->handleContinentThreatReinforce(); });
		stateMachine.addTransition([this]() { return this->isTerritoryUnderThreatReinforce(); },
			[this]() { this->handleTerritoryUnderTheatReinforce(); });
		stateMachine.addTransition([this]() { return this->isTerritoryUndErencircle(); },
			[this]() { this->handleTerritoryUndErencircleReinforce(); });
		stateMachine.addTransition([this]() { return true; },
			[this]() { this->evaluateTerritoriesReinforce(); });
		stateMachine.run();

		std::cout << "Chosen territory before adding: " << chosenTerritory->getName()
			<< " amount of Forces: " << chosenTerritory->getForces() << std::endl;
		// Determine how many forces to add based on the threat level and strategic importance
		int howMuchToAdd = calculateForcesToAdd(chosenTerritory, forcesToReinforce);
		chosenTerritory->AddForces(howMuchToAdd, 0); // Assuming 0 is the bot's owner ID
		forcesToReinforce -= howMuchToAdd;

		std::cout << "Added " << howMuchToAdd << " forces to " << chosenTerritory->getName() << std::endl;

		stateMachine.clearTransitions();
	}
	return chosenTerritory;
}


void Bot::handleTerritoryUnderTheatReinforce() {
	std::cout << "Handling territory under threat... reinforce " << std::endl;

	Territory* bestTerritory = nullptr;
	int highestEnemyForcesSum = -1; // Initialize with a value lower than any possible sum

	// Iterate through the territories under threat
	for (const std::string& territoryName : territoriesUnderThreat) {
		Territory& territory = board->territories[territoryName];
		int enemyForcesSum = 0;

		// Calculate the sum of forces of adjacent enemy territories
		for (const std::string& adjName : board->adjacencyList[territory.getName()]) {
			Territory& adjTerritory = board->territories[adjName];
			if (adjTerritory.getOwner() == 1) { // Check if the adjacent territory is owned by the enemy
				enemyForcesSum += adjTerritory.getForces();
			}
		}

		// Update the best territory if this one has a higher sum of enemy forces
		if (enemyForcesSum > highestEnemyForcesSum) {
			highestEnemyForcesSum = enemyForcesSum;
			bestTerritory = &territory;
		}
	}
	// Set the chosen territory to the one with the highest sum of adjacent enemy forces
	chosenTerritory = bestTerritory;

	if (chosenTerritory != nullptr) {
		std::cout << "Chosen territory: " << chosenTerritory->getName()
			<< " with highest adjacent enemy forces sum: " << highestEnemyForcesSum << std::endl;

		territoriesUnderThreat.clear(); // Clear the territories under threat after processing
	}
	else {
		std::cout << "No territory chosen for reinforcement." << std::endl;
	}
}



/// <summary>
/// This function is calculating the forces to add to the territory 
/// based on the threat level and strategic importance
/// It will add forces to the territory based on the amount of forces in the adjacent territories
/// and the amount of forces in the territory	
/// The function will return the amount of forces to add to the territory
/// </summary>
/// <param name="territory"></param>
/// <param name="availableForces"></param>
/// <returns></returns>
int Bot::calculateForcesToAdd(Territory* territory, int availableForces) {
	int threatLevel = 0;
	for (const std::string& adjName : board->adjacencyList[territory->getName()]) {
		Territory& adjTerritory = board->territories[adjName];
		if (adjTerritory.getOwner() == 1) {
			threatLevel += adjTerritory.getForces();
		}
	}

	int forcesToAdd = std::min(availableForces, threatLevel - territory->getForces() + 1);
	forcesToAdd = std::max(forcesToAdd, 1); // Ensure at least 1 force is added
	return forcesToAdd;
}


/// <summary>
/// This function is handling the territory under encircle in the reinforce phase
/// /// it will grade the territories that are under encircle and give them a grade
/// the grade will be the amount of adjacent territories that are owned by the bot * 0.5
/// and the amount of forces in the adjacent territories * 1.2
/// the bot will not choose the territories that are giving up
/// the bot will choose the territory with the highest grade
/// </summary>
void Bot::handleTerritoryUndErencircleReinforce() {

	std::cout << "Handling territory under encircle..." << std::endl;
	Territory* bestTerritory = nullptr;
	double highestGrade = -std::numeric_limits<double>::infinity();

	for (auto& pair : board->territories) {
		Territory& territory = pair.second;
		if (territory.getOwner() == 0 && std::find(givingUpTerritories.begin(), givingUpTerritories.end(), territory.getName()) == givingUpTerritories.end()) {
			double grade = 0.0;
			for (const std::string& adjName : board->adjacencyList[territory.getName()]) {/// this for loop is to check if the territory is under encircle
				Territory& adjTerritory = board->territories[adjName];
				if (adjTerritory.getOwner() == territory.getOwner()) {
					grade += adjTerritory.getForces() * 0.5;
					grade += 2.0;
				}
				else {
					grade -= 0.2;
				}
			}
			territory.setGrade(grade);
			if (grade > highestGrade) {
				highestGrade = grade;
				bestTerritory = &territory;
			}
		}
	}
	chosenTerritory = bestTerritory;
	if (chosenTerritory != nullptr) {

		std::cout << "Chosen territory: " << chosenTerritory->getName() << std::endl;
	}
	else {
		std::cout << "No territory chosen." << std::endl;
	}
}



/// <summary>
/// This function is handling the continent threat in the reinforce phase
/// the bot will choose the territory with the highest grade in the continent
/// the grade will be the amount of adjacent territories that are owned by the bot * 0.5
/// and the amount of forces in the adjacent territories * 1.2
/// the bot will not choose the territories that are giving up
/// the bot will choose the territory with the highest grade
/// </summary>
void Bot::handleContinentThreatReinforce() {
	// Get all territories in the threatened continent
	std::vector<Territory*> continentTerritories = board->getTerritoriesInContinent(threatingContinent);

	std::cout << "Handling continent threat... in the reinforce" << std::endl;
	std::cout << "The continent under threat is: " << threatingContinent << std::endl;

	// Filter for bot-owned territories
	std::vector<Territory*> botOwnedTerritories;
	for (Territory* territory : continentTerritories) {
		if (territory->getOwner() == 0) { // Assuming 0 is the bot's ID
			botOwnedTerritories.push_back(territory);
		}
	}

	if (botOwnedTerritories.empty()) {
		std::cout << "No bot-owned territories in the threatened continent." << std::endl;
		chosenTerritory = nullptr; // No valid territory to reinforce
		return;
	}

	// Determine the best territory to reinforce
	Territory* bestTerritory = nullptr;
	int highestThreatScore = -1;

	for (Territory* territory : botOwnedTerritories) {
		int threatScore = 0;

		// Calculate threat score based on adjacent enemy-owned territories
		for (const std::string& adjName : board->adjacencyList[territory->getName()]) {
			Territory& adjacentTerritory = board->territories[adjName];
			if (adjacentTerritory.getOwner() != 0) { // Not owned by the bot
				threatScore++;
			}
		}

		// Keep track of the territory with the highest threat score
		if (threatScore > highestThreatScore) {
			highestThreatScore = threatScore;
			bestTerritory = territory;
		}
	}

	// Set the best territory to reinforce
	chosenTerritory = bestTerritory;

	if (chosenTerritory) {
		std::cout << "Chosen territory to reinforce: " << chosenTerritory->getName() << " with threat score: " << highestThreatScore << std::endl;
	}
	else {
		std::cout << "No suitable territory found to reinforce." << std::endl;
	}
}


/// <summary>
/// This function is choosing the best territory to reinforce
/// the bot will choose the territory with the highest grade
/// the bot will not choose the territories that are giving up
/// </summary>
void Bot::chooseBestTerritoryToReinforce() {
	double highestGrade = -std::numeric_limits<double>::infinity();
	for (auto& pair : board->territories) {
		Territory& territory = pair.second;
		if (territory.getOwner() == 0 && territory.getGrade() > highestGrade) {
			highestGrade = territory.getGrade();
			chosenTerritory = &territory;
		}
	}
}


/// <summary>
/// This function is evaluating the territories for the reinforce phase
/// this is a generic function that will give the territories a grade
/// the grade will be the amount of adjacent territories that are owned by the bot * 0.5
/// and the amount of forces in the adjacent territories * 1.2
/// </summary>
void Bot::evaluateTerritoriesReinforce() {

	for (auto& pair : board->territories) {
		Territory& territory = pair.second;
		if (territory.getOwner() == 0) {
			double grade = 0.0;
			for (const std::string& adjName : board->adjacencyList[territory.getName()]) {
				Territory& adjTerritory = board->territories[adjName];
				if (adjTerritory.getOwner() == territory.getOwner()) {
					grade += adjTerritory.getForces() * 0.5;
					grade += 2.0;
				}
				else {
					grade -= 0.2;
				}
			}
			territory.setGrade(grade);
		}
	}
	chooseBestTerritoryToReinforce();

}


/// <summary>
/// This function is checking if the territory is under encircle
/// under encicle is if the territory has more than 2 adjacent territories 
/// that are owned by the enemy
/// and the territory has more than 2 forces
/// the function will return true if the territory is under encircle
/// and false if the territory is not under encircle
/// </summary>
/// <returns></returns>
bool Bot::isTerritoryUndErencircle() {
	bool anyTerritoryUnderEncircle = false;

	for (auto& pair : board->territories) {
		Territory& territory = pair.second;
		int enemyAdjacentCount = 0;
		int totalAdjacentCount = 0;
		int enemyForces = 0;
		int totalForces = 0;

		for (const std::string& adjName : board->adjacencyList[territory.getName()]) {
			Territory& adjTerritory = board->territories[adjName];
			totalAdjacentCount++;
			totalForces += adjTerritory.getForces();

			if (adjTerritory.getOwner() == 1) { // Assuming 1 is the enemy's owner ID
				enemyAdjacentCount++;
				enemyForces += adjTerritory.getForces();
			}
		}
		if (enemyAdjacentCount >= totalAdjacentCount - 1) {
			givingUpTerritories.push_back(territory.getName());
		}

		if (enemyAdjacentCount > 2 && enemyForces > totalForces / 2) {
			anyTerritoryUnderEncircle = true;
		}
	}

	return anyTerritoryUnderEncircle;
}

/// <summary>
/// the idea of this function is to check if the continent is under threat
/// it will check if the enemy has more than half of the forces in the continent
/// and if the bot has at least one territory in the continent
/// if the bot has at least one territory in the continent
/// </summary>
/// <returns></returns>
bool Bot::isContinentUnderThreatReinforce() {
	std::unordered_map<std::string, int> continentTerritoryCount;
	std::unordered_map<std::string, int> continentOwnedCount;
	std::unordered_map<std::string, int> continentEnemyCount;
	bool isContinentUnderThreat = false;

	// Calculate the number of territories, bot-owned territories, and enemy-owned territories in each continent
	for (const auto& pair : board->territories) {
		const Territory& territory = pair.second;
		continentTerritoryCount[territory.getContinent()]++;
		if (territory.getOwner() == 0) { // Bot-owned territory
			continentOwnedCount[territory.getContinent()]++;
		}
		else if (territory.getOwner() == 1) { // Enemy-owned territory
			continentEnemyCount[territory.getContinent()]++;
		}
	}

	// Determine if any continent is under threat
	for (const auto& pair : continentTerritoryCount) {
		if (!isContinentUnderThreat) {
			const std::string& continent = pair.first;
			int totalTerritories = pair.second;
			int enemyTerritories = continentEnemyCount[continent];

			// Check if the enemy owns more than half of the territories in the continent
			if (enemyTerritories >= totalTerritories / 2) {
				// Check if the bot owns at least one territory in this continent
				if (continentOwnedCount[continent] > 0) { 
					threatingContinent = continent; // Set the threatened continent
					isContinentUnderThreat = true;
				}
			}
		}
	}

	// If no continent is under threat or the bot does not own any territory in the threatened continent
	return isContinentUnderThreat;
}


/// <summary>
/// this function is checking if the territory is under threat
/// the idea is to check if the enemy has more than the forces in
/// that territory in one of the adjacent territories
bool Bot::isTerritoryUnderThreatReinforce() {
	bool anyTerritoryUnderThreat = false;

	// Iterate through all territories owned by the bot
	for (auto& pair : board->territories) {
		Territory& territory = pair.second;

		if (territory.getOwner() == 0) { // Check only bot-owned territories
			bool isUnderThreat = false;

			// Check all adjacent territories
			for (const std::string& adjName : board->adjacencyList[territory.getName()]) {
				Territory& adjTerritory = board->territories[adjName];

				// Check if the adjacent territory is owned by the enemy and has more forces
				if (adjTerritory.getOwner() == 1 && adjTerritory.getForces() > territory.getForces()) {
					isUnderThreat = true;
				}
			}

			// If the territory is under threat, add it to the list
			if (isUnderThreat) {
				anyTerritoryUnderThreat = true;
				territoriesUnderThreat.push_back(territory.getName());
				std::cout << "yes territory is under threat: " << territory.getName() << std::endl;
			}
		}
	}

	return anyTerritoryUnderThreat;
}

/// <summary>
/// The idea is to check if there is any territory the player ownes
/// and the territory has more than twice in each adjacsnt enemy territory
/// </summary>
/// <returns></returns>
bool Bot::needToAttack() {
	bool isTerritoryNeedToAttack = false;
	for (const auto& pair : board->territories) {
		const Territory& territory = pair.second;
		if (territory.getOwner() == 0 && !isTerritoryNeedToAttack) {
			for (const std::string& adjName : board->adjacencyList[territory.getName()]) {
				const Territory& adjTerritory = board->territories[adjName];
				if (adjTerritory.getOwner() == 1 && territory.getForces() >= 2 * adjTerritory.getForces()) { // אם הטריטוריה השכנה שייכת לאויב ויש לה פחות מחצי כוחות
					isTerritoryNeedToAttack = true;

				}
			}
		}
	}
	return isTerritoryNeedToAttack;
}

/// <summary>
/// This function is attacking the enemy territories
/// only if needed
/// </summary>
bool Bot::attackPhase(Territory*& attackingTerritory, Territory*& attackedTerritory) {
	std::cout << "Attacking..." << std::endl;
	clearGrades();

	if (needToAttack()) {
		if ((attackingTerritory = chooseTerritoryToAttackFrom()) != nullptr) {
			if ((attackedTerritory = chooseTerritoryToAttack(attackingTerritory)) != nullptr) {
				if (hasSufficientForces(attackingTerritory, attackedTerritory)) {
					return true;
				}
			}
		}
	}
	else {
		std::cout << "No need to attack." << std::endl;
	}
	return false;
}


/// <summary>
/// This function checks if the bot has sufficient forces to attack
/// if the bot territory has more than twice the forces in the enemy territory
/// the bot has sufficient forces to attack
/// </summary>
/// <param name="attackingTerritory"></param>
/// <param name="attackedTerritory"></param>
/// <returns></returns>
bool Bot::hasSufficientForces(Territory* attackingTerritory, Territory* attackedTerritory) {
	return (attackingTerritory->getForces() >= 2 * attackedTerritory->getForces());
}


/// <summary>
/// Chooses the best territory to attack from
/// Prioritizes territories that can attack multiple enemy territories.
/// </summary>
Territory* Bot::chooseTerritoryToAttackFrom() {
	Territory* bestTerritory = nullptr;
	int maxAttackableEnemies = -1;

	for (auto& pair : board->territories) {
		Territory& territory = pair.second;
		if (territory.getOwner() == 0 && territory.getForces() > 0) {
			int attackableEnemies = 0;
			for (const std::string& adjName : board->adjacencyList[territory.getName()]) {
				Territory& adjTerritory = board->territories[adjName];
				if (adjTerritory.getOwner() == 1 && hasSufficientForces(&territory, &adjTerritory)) {
					attackableEnemies++;
				}
			}

			if (attackableEnemies > maxAttackableEnemies) {
				maxAttackableEnemies = attackableEnemies;
				bestTerritory = &territory;
			}
		}
	}

	if (bestTerritory != nullptr) {
		std::cout << "Chosen territory to attack from: " << bestTerritory->getName() << std::endl;
	}
	else {
		std::cout << "No territory to attack from." << std::endl;
	}

	return bestTerritory;
}


/// <summary>
/// Chooses the best territory to attack
/// Targets the enemy territory with the lowest forces and maximizes attackable options.
/// </summary>
Territory* Bot::chooseTerritoryToAttack(Territory* attackingTerritory) {
	Territory* bestTarget = nullptr;
	int minEnemyForces = std::numeric_limits<int>::max();

	for (const std::string& adjName : board->adjacencyList[attackingTerritory->getName()]) {
		Territory& adjTerritory = board->territories[adjName];
		if (adjTerritory.getOwner() == 1 && hasSufficientForces(attackingTerritory, &adjTerritory)) {
			if (adjTerritory.getForces() < minEnemyForces) {
				minEnemyForces = adjTerritory.getForces();
				bestTarget = &adjTerritory;
			}
		}
	}

	if (bestTarget != nullptr) {
		std::cout << "Chosen territory to attack: " << bestTarget->getName() << std::endl;
	}
	else {
		std::cout << "No target territory to attack." << std::endl;
	}

	return bestTarget;
}


/// <summary>
/// Determines if the bot needs to attack with a plane
/// Based entirely on the game state, focusing on critical risks or opportunities.
/// </summary>
bool Bot::needToAttackWithPlane() {
	bool planeNeeded = false;
	Territory* criticalDefenseTerritory = nullptr; // Territory at risk of being lost

	for (auto& pair : board->territories) {
		Territory& botTerritory = pair.second;

		// Check only bot-owned territories
		if (botTerritory.getOwner() == 0) {
			for (const std::string& adjName : board->adjacencyList[botTerritory.getName()]) {
				Territory& enemyTerritory = board->territories[adjName];

				// Check only enemy-owned territories
				if (enemyTerritory.getOwner() == 1) {
					int botForces = botTerritory.getForces();
					int enemyForces = enemyTerritory.getForces();

					// Critical Defense: Prevent losing a bot territory in the next enemy turn
					if (enemyForces > botForces && enemyForces >= 2 * botForces) {
						criticalDefenseTerritory = &botTerritory;
						planeNeeded = true;
						std::cout << "Plane needed: Critical defense for " << botTerritory.getName() << std::endl;
					}
				}
			}
		}
	}

	// Final decision: Only use the plane if the situation is critical
	if (planeNeeded) {
		if (criticalDefenseTerritory != nullptr) {
			std::cout << "Plane will be used to defend " << criticalDefenseTerritory->getName() << std::endl;
		}

		return true;
	}

	std::cout << "No plane attack needed." << std::endl;
	return false;
}