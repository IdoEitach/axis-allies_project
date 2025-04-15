#include "Bot.h"

Bot::Bot(AxisBoard* board) : board(board), chosenTerritory(nullptr), threatingContinent("") {

}


void Bot::clearGrades() {
	for (auto& pair : board->territories) {
		Territory& territory = pair.second;
		territory.setGrade(0.0);
	}
}


int Bot::howMuchForcesToDefendWith(Territory * attackedTerritory){
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
	stateMachine.removeTransition([this]() { return this->isMapIsEmpety(); });
	stateMachine.removeTransition([this]() { return this->isContinentUnderThreatInit(); });
	stateMachine.removeTransition([this]() { return true; });
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
	std::cout << "is the map is empty? :" << std::endl;
	for (const auto& pair : board->territories) {
		const Territory& territory = pair.second;
		if (territory.getOwner() != -1) {
			std::cout << "no" << std::endl;
			return false;
		}
	}
	std::cout << "yes" << std::endl;
	return true;
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
			break;
		}
	}

	if (allOccupied) {
		std::cout << "All territories in the continent are already occupied." << std::endl;
		evaluateTerritoriesInit();
		return;
	}

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
	std::cout << "Reinforceing" << std::endl;
	while (forcesToReinforce > 0) {
		clearGrades();
		givingUpTerritories.clear();
		stateMachine.addTransition([this]() { return this->isTerritoryUnderThreatReinforce(); },
			[this]() { this->handleContinentThreatReinforce(); });
		stateMachine.addTransition([this]() { return this->isContinentUnderThreatReinforce(); },
			[this]() { this->handleContinentThreatReinforce(); });
		stateMachine.addTransition([this]() { return this->isTerritoryUndErencircle(); },
			[this]() { this->handleTerritoryUndErencircleReinforce(); });
		stateMachine.addTransition([this]() { return true; },
			[this]() { this->evaluateTerritoriesReinforce(); });
		stateMachine.run();
		chooseBestTerritoryToReinforce();
		std::cout << "Chosen territory before adding: " << chosenTerritory->getName()
			<< " amount of Forces: " << chosenTerritory->getForces() << std::endl;
		// Determine how many forces to add based on the threat level and strategic importance
		int howMuchToAdd = calculateForcesToAdd(chosenTerritory, forcesToReinforce);
		chosenTerritory->AddForces(howMuchToAdd, 0); // Assuming 0 is the bot's owner ID
		forcesToReinforce -= howMuchToAdd;

		std::cout << "Added " << howMuchToAdd << " forces to " << chosenTerritory->getName() << std::endl;

		stateMachine.removeTransition([this]() { return this->isTerritoryUnderThreatReinforce(); });
		stateMachine.removeTransition([this]() { return this->isContinentUnderThreatReinforce(); });
		stateMachine.removeTransition([this]() { return this->isTerritoryUndErencircle(); });
		stateMachine.removeTransition([this]() { return true; });
	}
	return chosenTerritory;
}


/// <summary>
/// 
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
/// </summary>
void Bot::handleContinentThreatReinforce() {
	std::vector<Territory*> continentTerritories = board->getTerritoriesInContinent(threatingContinent);

	std::cout << "Handling continent threat..." << std::endl;
	std::cout << "The continent under threat is: " << threatingContinent << std::endl;


}


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
			territory.setGrade(grade);
		}
	}

}


/// <summary>
/// This function is checking if the territory is under encircle 
/// the idea is to check the amount of adjacent territories the enemy has 
/// if he has more than 2 adjacent territories the territory is under encircle
/// but if the enemy has more than half of the forces in the adjacent territories
/// than the bot state is to give up
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
/// </summary>
/// <returns></returns>
bool Bot::isContinentUnderThreatReinforce() {
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
		int enemyTerritories = continentEnemyCount[continent];
		if (enemyTerritories >= totalTerritories / 2) {
			threatingContinent = continent;
			return true;
		}
	}
	return false;
}


/// <summary>
/// this function is checking if the territory is under threat
/// the idea is to check if the enemy has more than the forces in
/// that territory in one of the adjacent territories
bool Bot::isTerritoryUnderThreatReinforce() {
	for (const std::string& adjName : board->adjacencyList[chosenTerritory->getName()]) {
		Territory& adjTerritory = board->territories[adjName];
		if (adjTerritory.getOwner() == 1) {
			if (adjTerritory.getForces() >= chosenTerritory->getForces()) {
				threatingTerritory = adjTerritory.getName();
				return true;
			}
		}
	}
	return false;
}


/// <summary>
/// The idea is to check if there is any territory the player ownes
/// and the territory has more than twice in each adjacsnt enemy territory
/// </summary>
/// <returns></returns>
bool Bot::needToAttack() {
	for (const auto& pair : board->territories) {
		const Territory& territory = pair.second;
		if (territory.getOwner() == 0) { // אם הטריטוריה שייכת לבוט
			for (const std::string& adjName : board->adjacencyList[territory.getName()]) {
				const Territory& adjTerritory = board->territories[adjName];
				if (adjTerritory.getOwner() == 1 && territory.getForces() >= 2 * adjTerritory.getForces()) { // אם הטריטוריה השכנה שייכת לאויב ויש לה פחות מחצי כוחות
					return true;
				}
			}
		}
	}
	return false;
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
/// This function chooses the best territory to attack from
/// </summary>


Territory* Bot::chooseTerritoryToAttackFrom() {
	Territory* bestTerritory = nullptr;
	double highestForces = -1; // Fixed initialization

	for (auto& pair : board->territories) {
		Territory& territory = pair.second;
		if (territory.getOwner() == 0) {
			// Check if the territory can attack any adjacent enemy territories
			for (const std::string& adjName : board->adjacencyList[territory.getName()]) {
				Territory& adjTerritory = board->territories[adjName];
				if (adjTerritory.getOwner() == 1) {
					// Found an enemy neighbor, check if we have enough forces
					if (territory.getForces() >= 2 * adjTerritory.getForces()) {
						// This territory can attack, check if it has more forces than the current best
						if (territory.getForces() > highestForces) {
							highestForces = territory.getForces();
							bestTerritory = &territory;
						}
						break; // No need to check other neighbors if we can already attack
					}
				}
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
/// This function chooses the best territory to attack 
/// the attacked territory will be the one of the adjacent territories 
/// to the attacking territory
/// the bot will choose the territory with the lowest forces from the possible targets
/// </summary>

Territory* Bot::chooseTerritoryToAttack(Territory* attackingTerritory) {
	std::vector<Territory*> possibleTargets;

	for (const std::string& adjName : board->adjacencyList[attackingTerritory->getName()]) {
		Territory& adjTerritory = board->territories[adjName];
		if (adjTerritory.getOwner() == 1) {
			possibleTargets.push_back(&adjTerritory);
		}
	}

	if (possibleTargets.empty()) {
		std::cout << "No territory to attack." << std::endl;
		return nullptr;
	}

	Territory* bestTarget = nullptr;
	double lowestForces = std::numeric_limits<double>::infinity();
	for (Territory* territory : possibleTargets) {
		double forces = territory->getForces();
		if (forces < lowestForces) {
			lowestForces = forces;
			bestTarget = territory;
		}
	}

	std::cout << "Chosen territory to attack: " << bestTarget->getName() << std::endl;
	return bestTarget;
}

/// <summary>
/// 
/// </summary>
bool Bot::needToAttackWithPlane() {
	// Iterate through all territories owned by the bot
	for (auto& pair : board->territories) {
		Territory& territory = pair.second;
		if (territory.getOwner() == 0) { // 0 is the bot's owner ID

			// Check adjacent territories for potential attacks
			for (const std::string& adjName : board->adjacencyList[territory.getName()]) {
				Territory& adjTerritory = board->territories[adjName];
				if (adjTerritory.getOwner() == 1) { // 1 is the enemy's owner ID
					// If the bot has more than twice the forces of the adjacent enemy territory,
					// it means the bot can attack without needing the plane
					if (territory.getForces() > 2 * adjTerritory.getForces()) {
						return false; // No need to attack with a plane
					}
				}
			}
		}
	}

	// If the bot doesn't have any adjacent enemy territories that it can easily attack,
	// it should attack with a plane
	return true; // Need to attack with a plane
}
