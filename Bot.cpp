#include "Bot.h"

Bot::Bot(RiskBoard* board) : board(board), chosenTerritory(nullptr), threatingContinent("") {
	// הוספת מעברים רק עבור שלב האתחול  
}

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

bool Bot::isContinentUnderThreat() {
	std::cout << "is the continent under threat? :" << std::endl;
	std::unordered_map<std::string, int> continentTerritoryCount;
	std::unordered_map<std::string, int> continentOwnedCount;

	for (const auto& pair : board->territories) {
		const Territory& territory = pair.second;
		continentTerritoryCount[territory.getContinent()]++;
		if (territory.getOwner() == 0) {
			continentOwnedCount[territory.getContinent()]++;
		}
	}
	for (const auto& pair : continentTerritoryCount) {
		const std::string& continent = pair.first;
		int totalTerritories = pair.second;
		int ownedTerritories = continentOwnedCount[continent];
		if (ownedTerritories >= totalTerritories / 2) {
			threatingContinent = continent;
			return true;
		}
	}
	return false;
}

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

void Bot::handleEmptyMap() {
	std::cout << "Handling empty map..." << std::endl;
	chosenTerritory = board->getTerritoryByName("BRAZIL");
	std::cout << "Chosen territory: " << chosenTerritory->getName() << std::endl;

}

void Bot::handleContinentThreat() {
	std::cout << "Handling continent threat..." << std::endl;
	std::cout << "The continent under threat is: " << threatingContinent << std::endl;
	// לוגיקה לטיפול באיום על היבשת
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
	/*for (auto& pair : board->territories) {
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
			std::cout << "the grade of the " << territory.getName() << " is: " << territory.getGrade() << std::endl;
		}
	}

	stateMachine.addTransition([this]() { return true; }, [this]() { this->chooseBestTerritory(); });
	stateMachine.run();*/

	Territory* bestTerritory = nullptr;
	double highestGrade = -std::numeric_limits<double>::infinity();
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
			std::unordered_map<std::string, int> continentTerritoryCount;
			std::unordered_map<std::string, int> continentOwnedCount;
			for (const auto& pair : board->territories) {
				const Territory& territory = pair.second;
				continentTerritoryCount[territory.getContinent()]++;
				if (territory.getOwner() == 0) { // Assuming 0 is the bot's owner ID
					continentOwnedCount[territory.getContinent()]++;
				}
			}
			// Print the status of each continent
			for (const auto& pair : continentTerritoryCount) {
				const std::string& continent = pair.first;
				int totalTerritories = pair.second;
				int ownedTerritories = continentOwnedCount[continent];
				int remainingTerritories = totalTerritories - ownedTerritories;

				std::cout << "Continent: " << continent << std::endl;
				std::cout << "Owned Territories: " << ownedTerritories << "/" << totalTerritories << std::endl;
				std::cout << "Territories needed to fully own: " << remainingTerritories << std::endl;
			}
			// Calculate the grade for each territory based on continent ownership
			for (auto& pair : board->territories) {
				Territory& territory = pair.second;
				if (territory.getOwner() == -1) {
					double grade = territory.getGrade();
					int totalTerritories = continentTerritoryCount[territory.getContinent()];
					int ownedTerritories = continentOwnedCount[territory.getContinent()];
					int remainingTerritories = totalTerritories - ownedTerritories;
					// this is the grade given to complete the continent
					grade += (totalTerritories - remainingTerritories) * 0.7;
					territory.setGrade(grade);
					std::cout << "Adjusted grade of " << territory.getName() << " is: " << territory.getGrade() << std::endl;
					if (grade > highestGrade) {
						highestGrade = grade;
						bestTerritory = &territory;
					}
				}
			}
			territory.setGrade(grade);
			std::cout << "the grade of the " << territory.getName() << "the grade is given to the territory :" << territory.getGrade() << std::endl;
			if (grade > highestGrade) {
				highestGrade = grade;
				bestTerritory = &territory;
			}
		}
	}
	chosenTerritory=bestTerritory;

}



/// <summary>
/// the bot chooses the territory to init 
/// he grades the territories based on the number of adjacent territories that he owns  -0.5
/// and the number of forces in the adjacent territories - 0.5
/// and the number of adjacent territories that he doesn't own - (-0.2)
/// and the number of forces in the adjacent territories - 0.2
/// // the bot chooses the territory with the highest grade
/// </summary>
/// <returns></returns>
//Territory* Bot::chosenTerritoryToInit() {
//	Territory* bestTerritory = nullptr;
//	double highestGrade = -std::numeric_limits<double>::infinity();
//	for (auto& pair : board->territories) {
//		Territory& territory = pair.second;
//		if (territory.getOwner() == -1) {
//
//			double grade = 0.0;
//
//			for (const std::string& adjName : board->adjacencyList[territory.getName()]) {
//				Territory& adjTerritory = board->territories[adjName];
//				if (adjTerritory.getOwner() == territory.getOwner()) {
//					grade += adjTerritory.getForces() * 0.5;
//					grade += 2.0;
//				}
//				else {
//					grade -= 0.2;
//				}
//			}
//			std::unordered_map<std::string, int> continentTerritoryCount;
//			std::unordered_map<std::string, int> continentOwnedCount;
//			for (const auto& pair : board->territories) {
//				const Territory& territory = pair.second;
//				continentTerritoryCount[territory.getContinent()]++;
//				if (territory.getOwner() == 0) { // Assuming 0 is the bot's owner ID
//					continentOwnedCount[territory.getContinent()]++;
//				}
//			}
//			// Print the status of each continent
//			for (const auto& pair : continentTerritoryCount) {
//				const std::string& continent = pair.first;
//				int totalTerritories = pair.second;
//				int ownedTerritories = continentOwnedCount[continent];
//				int remainingTerritories = totalTerritories - ownedTerritories;
//
//				std::cout << "Continent: " << continent << std::endl;
//				std::cout << "Owned Territories: " << ownedTerritories << "/" << totalTerritories << std::endl;
//				std::cout << "Territories needed to fully own: " << remainingTerritories << std::endl;
//			}
//			// Calculate the grade for each territory based on continent ownership
//			for (auto& pair : board->territories) {
//				Territory& territory = pair.second;
//				if (territory.getOwner() == -1) {
//					double grade = territory.getGrade();
//					int totalTerritories = continentTerritoryCount[territory.getContinent()];
//					int ownedTerritories = continentOwnedCount[territory.getContinent()];
//					int remainingTerritories = totalTerritories - ownedTerritories;
//					// this is the grade given to complete the continent
//					grade += (totalTerritories - remainingTerritories) * 0.7;
//					territory.setGrade(grade);
//					std::cout << "Adjusted grade of " << territory.getName() << " is: " << territory.getGrade() << std::endl;
//					if (grade > highestGrade) {
//						highestGrade = grade;
//						bestTerritory = &territory;
//					}
//				}
//			}
//			territory.setGrade(grade);
//			std::cout << "the grade of the " << territory.getName() << "the grade is given to the territory :" << territory.getGrade() << std::endl;
//			if (grade > highestGrade) {
//				highestGrade = grade;
//				bestTerritory = &territory;
//			}
//		}
//	}
//	return bestTerritory;
//
//}

