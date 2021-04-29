//
// Created by benmo on 3/26/2020.
//

#ifndef SFML_TEMPLATE_COMSHIP_H
#define SFML_TEMPLATE_COMSHIP_H

#include <random>
#include <iostream>
#include "System.h"
#include "Ship.h"
#include <float.h>

class COMShip : public Ship {
public:
    enum Status {
        ROLL, MOVING, WARPING, ATTACKING
    };
private:
    sf::Vector2f destination;
    int ticksSinceLast = 0, landing = -9999;
    float targetVelo;

    std::string name;
    int playerReputation;

    Status status = ROLL;
public:
    COMShip(const sf::Texture &texture, float scale, float xPos, float yPos, float velocity, float accelRate, float maxVelocity, float direction, float turnRate, int maxFuel, int maxHull, int cargo, int passengers, std::string _name, int playerRep) : Ship(texture, scale, xPos, yPos, velocity, maxVelocity, direction, turnRate, maxFuel, maxHull, cargo, passengers) {
        destination = sf::Vector2f(xPos + 1, yPos + 1);
        spritePhysics.acceleration = accelRate;

        playerReputation = playerRep;
        name = std::move(_name);
    };

    void shoot(std::vector<Shootable*> &shots) override {
        for (Weapon *w : weapons) {
            double angle = -getAimAngle(target->getPosition(), getPosition());
            double leftEffectiveBorder = spritePhysics.direction + w->getEffectiveAngle()/2;
            double rightEffectiveBorder = spritePhysics.direction - w->getEffectiveAngle()/2;

            if (w->canShoot() && ( angle < leftEffectiveBorder && angle > rightEffectiveBorder) && distance(target->getPosition(), getPosition()) < w->getProjectile().getRange()) {
                shots.push_back(w->shoot(this));
            }
        }
    }

    template< class RNG >
    Status pathfind(const sf::RenderWindow &window, RNG &gen, System *loc, Ship* player, std::vector<Shootable*> &projectiles) {
        if (status != ATTACKING && isHostile()) {
            setTarget(player);
            status = ATTACKING;
        }

        if (status == ROLL) {
            rollPosition(window, gen);
        }

        if (status == ATTACKING) {
            if (target != nullptr) {
                dogFight(getShortestWeaponRange(), target, projectiles);
            } else {
                status = MOVING;
            }
        }

        if (status == MOVING) {
            approachTargetVelocity();

            turnTowardsTarget();

            ticksSinceLast++;

            if (ticksSinceLast > 2000) std::cout << "likely loop" << std::endl;
            if (distance(destination, getPosition()) > 4000) std::cout << "Out of bounds" << std::endl;
            else if (distance(destination, getPosition()) <(180 * targetVelo) / (GameSprite::PI * getTurnRate()) * 1.1)
                status = ROLL;
        }

        return status;
    }

    void dogFight(double dist, Ship *target, std::vector<Shootable*> &projectiles) {
        destination = target->getPosition();

        if (distance(getPosition(), target->getPosition()) > dist) targetVelo = spritePhysics.maxVelocity;
        else targetVelo = target->getVelocity();

        turnTowardsTarget();
        approachTargetVelocity();

        shoot(projectiles);
    }

    double getShortestWeaponRange() {
        double smallest = DBL_MAX;
        for (Weapon *w : weapons) {
            if (w->getProjectile().getRange() < smallest) {
                smallest = w->getProjectile().getRange() == 0 ? smallest : w->getProjectile().getRange();
            }
        }

        return smallest == DBL_MAX ? -1 : smallest;
    }

    void turnTowardsTarget() {
        double targetAngle = -getAimAngle(destination, getPosition());

        double changeAngle = abs(spritePhysics.direction - targetAngle);
        if (changeAngle > 180) changeAngle = abs(changeAngle - 360);
        if (changeAngle > getTurnRate()) changeAngle = getTurnRate();

        if (abs(spritePhysics.direction - targetAngle) <= 180) turn(spritePhysics.direction - targetAngle > 0 ? changeAngle : -changeAngle);
        else turn(spritePhysics.direction - targetAngle > 0 ? -changeAngle : changeAngle);
    }

    void approachTargetVelocity() {
        if (getVelocity() > targetVelo) accelerate(
                    abs(spritePhysics.velocity - targetVelo) > spritePhysics.acceleration ? -spritePhysics.acceleration : -abs(
                            spritePhysics.velocity - targetVelo));
        else if (getVelocity() < targetVelo) accelerate(
                    abs(spritePhysics.velocity - targetVelo) > spritePhysics.acceleration ? spritePhysics.acceleration : abs(
                            spritePhysics.velocity - targetVelo));
    }

    template<class RNG>
    void rollPosition(const sf::RenderWindow &window, RNG &gen) {
        std::uniform_int_distribution<int> roll;

        roll = std::uniform_int_distribution<int>(0, 100);
        if (roll(gen) == 50) {
            status = WARPING;
            return;
        }
        else status = MOVING;

        roll = std::uniform_int_distribution<int>(-1500, 1500);

        do {
            int randXPos = roll(gen);
            int randYPos = roll(gen);
            destination = sf::Vector2f((int) window.getSize().x / 2.0 + randXPos,
                                       (int) window.getSize().y / 2.0 + randYPos);

            roll = std::uniform_int_distribution<int>(4, 6);
            targetVelo = spritePhysics.maxVelocity / roll(gen);
        } while (distance(destination, getPosition()) < (180 * targetVelo) / (PI * getTurnRate()) * 1.1);

        ticksSinceLast = 0;
    }

    int getPlayerRep() const {
        return playerReputation;
    }

    void setPlayerRep(int playerRep) {
        playerReputation = playerRep;
    }

    bool isFriendly() const {
        return playerReputation >= Game::FRIENDLY_LOW;
    }

    bool isNeutral() const {
        return playerReputation >= Game::NUETRAL_LOW && playerReputation <= Game::NUETRAL_HIGH;
    }

    bool isHostile() const {
        return playerReputation <= Game::HOSTILE_HIGH;
    }

    std::string getName() {
        return name;
    }
};


#endif //SFML_TEMPLATE_COMSHIP_H
