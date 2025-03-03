#include "Bot.h"

Bot::Bot(RiskBoard& board) : board(board) {
}

Territory* Bot::chosenTerritoryToInit() {
    Territory* bestTerritory = nullptr;
    double highestGrade = -std::numeric_limits<double>::infinity();

    for (auto& pair : board.territories) {
        Territory& territory = pair.second;
        double grade = 0.0;

        // Calculate grade based on adjacent territories
        for (const std::string& adjName : board.adjacencyList[territory.getName()]) {
            Territory& adjTerritory = board.territories[adjName];
            if (adjTerritory.getOwner() == territory.getOwner()) { 
                grade += adjTerritory.getForces() * 0.5;
                grade += 1.0;
            } else {
                grade -= 0.2;
            }
        }
        std::cout << "the grade of the " << territory.getName() << std::endl;
        territory.setGrade(grade);
        if (grade > highestGrade) {
            highestGrade = grade;
            bestTerritory = &territory;
        }
    }

    return bestTerritory;
}