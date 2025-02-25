#include "Player.h"

Player::Player(int id) : id(id), totalForces(0), territoryCount(0), amountOfForcesToAdd(3) {}

int Player::getId() const { return id; }
int Player::getTotalForces() const { return totalForces; }
int Player::getTerritoryCount() const { return territoryCount; }
const std::vector<std::string>& Player::getTerritoriesOwned() const { return territoriesOwned; }

void Player::addTerritory(const std::string& territory) {
    territoriesOwned.push_back(territory);
    territoryCount++;
}

void Player::removeTerritory(const std::string& territory) {
    territoriesOwned.erase(std::remove(territoriesOwned.begin(), territoriesOwned.end(), territory), territoriesOwned.end());
    territoryCount = std::max(0, territoryCount - 1);
}

void Player::addForces(int amount) {
    totalForces += amount;
}

void Player::deductForces(int amount) {
    totalForces -= amount;
    if (totalForces < 0) totalForces = 0;
}

int Player::getAmountOfForcesToAdd() {
    return amountOfForcesToAdd;
}

void Player::setAmountOfForcesToAdd() {
    amountOfForcesToAdd = howMuchForcesToAdd();
}

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

int Player::howMuchForcesToAdd() {
    if (territoryCount < 9) {
        return 3;
    }
    return territoryCount / 3;
}