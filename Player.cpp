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
void Player::setAmountOfForcesToAdd() {
    amountOfForcesToAdd = howMuchForcesToAdd();
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
int Player::howMuchForcesToAdd() {
	return std::max(3, territoryCount / 3);
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

void Player::setMovedWithPlane(bool movedWithPlane) {
	this->movedWithPlane = movedWithPlane;
}
bool Player::getMovedWithPlane() const {
	return movedWithPlane;
}