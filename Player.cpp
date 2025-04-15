#include "Player.h"

Player::Player(int id) : id(id), totalForces(0), territoryCount(0), amountOfForcesToAdd(3) {}

int Player::getId() const { return id; }
int Player::getTotalForces() const { return totalForces; }
int Player::getTerritoryCount() const { return territoryCount; }


/// <summary>
/// This function is returning the territories owned by the players
/// </summary>
/// <returns></returns>
const std::vector<std::string>& Player::getTerritoriesOwned() const { return territoriesOwned; }


/// <summary>
/// This function is adding a territory to the player
/// </summary>
/// <param name="territory"></param>
void Player::addTerritory(const std::string& territory) {
	territoriesOwned.push_back(territory);
	territoryCount++;
}


/// <summary>
/// This function is removing a territory from the player
/// </summary>
/// <param name="territory"></param>
void Player::removeTerritory(const std::string& territory) {
	territoriesOwned.erase(std::remove(territoriesOwned.begin(), territoriesOwned.end(), territory), territoriesOwned.end());
	territoryCount = std::max(0, territoryCount - 1);
}


/// <summary>
/// This function is adding forces to the player
/// </summary>
/// <param name="amount"></param>
void Player::addForces(int amount) {
	totalForces += amount;
}


/// <summary>
/// This function is deducting forces from the player
/// </summary>
/// <param name="amount"></param>
void Player::deductForces(int amount) {
	totalForces -= amount;
	if (totalForces < 0) totalForces = 0;
}


/// <summary>
/// This function is getting the amount of forces to add
/// </summary>
/// <returns></returns>
int Player::getAmountOfForcesToAdd() {
	return amountOfForcesToAdd;
}


/// <summary>
/// This function is setting the amount of forces to add
/// </summary>
void Player::setAmountOfForcesToAdd(const AxisBoard& board) {
	amountOfForcesToAdd = howMuchForcesToAdd(board);
}


/// <summary>
/// This function is adding forces to the player
/// </summary>
/// <param name="amount"></param>
/// <param name="territoryPtr"></param>
void Player::reinForcement(int amount, Territory* territoryPtr) {
	if (amount > amountOfForcesToAdd) {
		std::cout << "you tried to add more than the amount you have so I added the max " << std::endl;
		territoryPtr->AddForces(amountOfForcesToAdd, id);
		amountOfForcesToAdd = 0;
		return;
	}
	amountOfForcesToAdd -= amount;
	totalForces += amount;
	territoryPtr->AddForces(amount, id);
}


/// <summary>
/// This function is calculating how much forces to add
/// </summary>
/// <returns></returns>
int Player::howMuchForcesToAdd(const AxisBoard& board) {
	int howMuchToadd = 0;
	howMuchToadd = std::max(3, territoryCount / 3);
	std::cout << "doing the calculation of how much to add " << std::endl;
	std::vector<std::string> ownedContinents = getOwnedContinents(board);
	// checks if the player owns the north america continent
	if (std::find(ownedContinents.begin(), ownedContinents.end(), "NORTAMERICA") != ownedContinents.end()) {
		howMuchToadd += 5; // Assuming owning North America gives 5 additional forces
	}
	if (std::find(ownedContinents.begin(), ownedContinents.end(), "SOUTHAMERICA") != ownedContinents.end()) {
		howMuchToadd += 2; // Assuming owning South America gives 2 additional forces
	}
	std::cout << "the amount of forces to add is: " << howMuchToadd << std::endl;
	return howMuchToadd;
}


std::vector<std::string> Player::getOwnedContinents(const AxisBoard& board) {
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
			if (board.territories.at(territoryName).getOwner() != this->getId()) {
				isOwned = false;
				break;
			}
		}

		if (isOwned) {
			ownedContinents.push_back(continentName);
		}
	}
	std::cout << "the owned continents are: " << std::endl;
	for (const auto& continent : ownedContinents) {
		std::cout << continent << std::endl;
	}

	return ownedContinents;
}


/// <summary>
/// This function is calculating how much forces to add
/// </summary>
/// <param name="board"></param>
/// <param name="continent"></param>
/// <returns></returns>
int Player::territoriesNeededForContinent(const AxisBoard& board, const std::string& continent) const {
	std::vector<Territory*> territoriesInContinent = board.getTerritoriesInContinent(continent);
	int neededTerritories = 0;

	for (Territory* territory : territoriesInContinent) {
		if (std::find(territoriesOwned.begin(), territoriesOwned.end(), territory->getName()) == territoriesOwned.end()) {
			neededTerritories++;
		}
	}
	return neededTerritories;
}


void Player::setAttackedWithPlane(bool attackedWithPlane) {
	this->attackedWithPlane = attackedWithPlane;
}

bool Player::getAttackedWithPlane() const {
	return attackedWithPlane;
}

/// <summary>
/// 
/// </summary>
/// <param name="movedWithPlane"></param>
void Player::setMovedWithPlane(bool movedWithPlane) {
	this->movedWithPlane = movedWithPlane;
}


bool Player::getMovedWithPlane() const {
	return movedWithPlane;
}