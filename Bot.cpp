#include "Bot.h"

Bot::Bot(RiskBoard* board) : board(board), chosenTerritory(nullptr), threatingContinent("") {
	// הוספת מעברים רק עבור שלב האתחול  
}
/// <summary>
/// this function is choosing the territory to init
/// it is using a state machine to choose the territory
/// </summary>
/// <returns></returns>
Territory* Bot::chosenTerritoryToInit() {
	stateMachine.addTransition([this]() { return this->isMapIsEmpety(); }, [this]() { this->handleEmptyMap(); });
	stateMachine.addTransition([this]() { return this->isContinentUnderThreat(); }, [this]() { this->handleContinentThreat(); });
	stateMachine.addTransition([this]() { return true; }, [this]() { this->evaluateTerritories(); });
	stateMachine.run();
	stateMachine.removeTransition([this]() { return this->isMapIsEmpety(); });
	stateMachine.removeTransition([this]() { return this->isContinentUnderThreat(); });
	stateMachine.removeTransition([this]() { return true; });
	std::cout << "Chosen territory: " << chosenTerritory->getName() << std::endl;
	return chosenTerritory;
}
/// <summary>
/// check if the continent is under threat
/// threat is if the enemy has more than half of the territories in the continent
/// </summary>
/// <returns></returns>
bool Bot::isContinentUnderThreat() {
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

		if (enemyTerritories >= totalTerritories/2) {
			threatingContinent = continent;
			return true;
		}
	}

	return false;
}
/// <summary>
/// check if the map is empty
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
///  if the map is empty the bot will choose the territory "ONTARIO"
/// </summary>
void Bot::handleEmptyMap() {
	std::cout << "Handling empty map..." << std::endl;
	chosenTerritory = board->getTerritoryByName("ONTARIO");
	std::cout << "Chosen territory: " << chosenTerritory->getName() << std::endl;

}
/// <summary>
/// if the continent is under threat the bot will choose the territory with the highest grade in the continent
/// if all the territories in the continent are already occupied the bot will evaluate the territories with the regular grade
/// </summary>
void Bot::handleContinentThreat() {
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
		evaluateTerritories();
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
                } else {
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
	stateMachine.removeTransition([this]() { return this->isContinentUnderThreat(); });
}
void Bot::init() {
	std::cout << "Initiating..." << std::endl;
}
void Bot::done() {
	std::cout << "Done!" << std::endl;
}
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
	std::cout << "Chosen territory: " << chosenTerritory->getName() << " with grade: " << highestGrade << std::endl;
}
void Bot::evaluateTerritories() {
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

