//
// Created by benmo on 2/16/2020.
//

#include "Ship.h"

Ship::Ship(const sf::Texture &texture, float scale, float xPos, float yPos, float velocity, float maxVelocity, float direction, float turnSpeed, int maxFuel, int maxHull, int cargo, int passengers) : GameSprite(texture, scale, round(xPos), round(yPos), velocity, maxVelocity, direction) {
    turnRate = turnSpeed;

    fullScale = scale;
    fuelCap = maxFuel;
    fuel = maxFuel;

    hullCap = maxHull;
    hull = hullCap;

    cargoSpace = cargo;
    passengerSpace = passengers;

    target = nullptr;
}

void Ship::update() {
    for (Weapon *w : weapons) {
        w->recharge();
    }

    GameSprite::update();
}

void Ship::shoot(std::vector<Shootable*> &shots) {
    for (Weapon *w : weapons) {
        if (w->canShoot()) {
            shots.push_back(w->shoot(this));
        }
    }
}

float Ship::getTurnRate() const {
    return turnRate;
}

float Ship::getFullScale () const {
    return fullScale;
}

int Ship::getFuelCap() const {
    return fuelCap;
}

int Ship::getFuelRemaining() const {
    return fuel;
}

void Ship::useFuel() {
    fuel--;
}

void Ship::setFuel(int _fuel) {
    fuel = _fuel;
}

int Ship::getHullCap() const {
    return hullCap;
}

int Ship::getHullRemaining() const {
    return hull;
}

void Ship::setHull(int _hull) {
    hull = _hull;
}

int Ship::getCargoSpace() const {
    return cargoSpace;
}

int Ship::getUsedCargoSpace() const {
    return cargoUsed;
}

void Ship::setUsedCargoSpace(int _cargoUsed) {
    cargoUsed = _cargoUsed;
}

int Ship::getPassengerSpace() const {
    return passengerSpace;
}

int Ship::getPassengersAboard() const {
    return passengersOn;
}

void Ship::setPassengersAboard(int _passengersOn) {
    passengersOn = _passengersOn;
}

Ship * Ship::getTarget() const {
    return target;
}

void Ship::setTarget(Ship *_target) {
    target = _target;
}

void Ship::addWeapon(Weapon *w) {
    weapons.push_back(w);
}
