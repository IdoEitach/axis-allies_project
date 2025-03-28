#include "Bot.h"

Bot::Bot(AxisBoard* board) : board(board), chosenTerritory(nullptr), threatingContinent("") {

}


/// <summary>
/// This function is choosing the territory to init
/// it is using a state machine to choose the territory
/// </summary>
/// <returns></returns>
Territory* Bot::chosenTerritoryToInit() {
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
void Bot::chooseBestTerritory() {
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
	std::cout << "Chosen territory: " << chosenTerritory->getName() << std::endl;
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
Territory* Bot::chosenTerritoryToReinforce() {
	stateMachine.addTransition([this]() { return this->isTerritoryUnderThreatReinforce(); },
		[this]() { this->handleContinentThreatReinforce(); });
}


/// <summary>
/// This function is handling the continent threat in the reinforce phase
/// </summary>
void Bot::handleContinentThreatReinforce() {


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