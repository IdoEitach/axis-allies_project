#include "Bot.h"



Bot::Bot(RiskBoard* board) : board(board) {} // Change to pointer

/// <summary>
/// the bot chooses the territory to init 
/// he grades the territories based on the number of adjacent territories that he owns  -0.5
/// and the number of forces in the adjacent territories - 0.5
/// and the number of adjacent territories that he doesn't own - (-0.2)
/// and the number of forces in the adjacent territories - 0.2
/// // the bot chooses the territory with the highest grade
/// 
/// </summary>
/// <returns></returns>
Territory* Bot::chosenTerritoryToInit() {
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

                    // Adjust grade based on the number of territories needed to fully own the continent
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

	// Count territories in each continent and owned territories in each continent
	
	return bestTerritory;

}

