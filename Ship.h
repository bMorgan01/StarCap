//
// Created by benmo on 2/16/2020.
//

#ifndef SFML_TEMPLATE_SHIP_H
#define SFML_TEMPLATE_SHIP_H


#include <vector>
#include <SFML/Graphics/Texture.hpp>
#include "GameSprite.h"
#include "Weapon.h"

class Ship : public GameSprite {
protected:
    float fullScale, turnRate;
    int fuelCap, fuel, hullCap, hull, cargoSpace, cargoUsed = 0, passengerSpace, passengersOn = 0;

    std::vector<Weapon*> weapons;

    Ship *target;
public:
    Ship(const sf::Texture &texture, float scale, float xPos, float yPos, float velocity, float maxVelocity, float direction, float turnSpeed, int maxFuel, int maxHull, int cargo, int passengers);

    void update();

    virtual void shoot(std::vector<Shootable*> &projectiles);

    float getTurnRate() const;

    float getFullScale () const;

    int getFuelCap() const;
    int getFuelRemaining() const;
    void useFuel();
    void setFuel(int);

    int getHullCap() const;
    int getHullRemaining() const;
    void setHull(int _hull);

    int getCargoSpace() const;
    int getUsedCargoSpace() const;
    void setUsedCargoSpace(int _cargoUsed);

    int getPassengerSpace() const;
    int getPassengersAboard() const;
    void setPassengersAboard(int _passengersOn);

    Ship * getTarget() const;
    void setTarget(Ship *_target);

    void addWeapon(Weapon *w);
};


#endif //SFML_TEMPLATE_SHIP_H
