//
// Created by Benjamin on 4/20/2021.
//

#ifndef SFML_TEMPLATE_SHOOTABLE_H
#define SFML_TEMPLATE_SHOOTABLE_H

#include "GameSprite.h"
#include "Ship.h"

class Ship;

class Shootable : public GameSprite {
protected:
    double damage;
    double range = 0;

    Ship *shooter;
protected:

    Shootable(const sf::Texture& texture, const sf::IntRect &rect, double scale, int rows, int cols, int xOffset, int yOffset, int frameDelay, double _damage) : GameSprite(texture, rect, rows, cols, xOffset, yOffset, frameDelay) {
        damage = _damage;

        setScale(scale/100, scale/100);
    }

public:
    Shootable() = default;

    void setShooter(GameSprite* _shooter) {
        shooter = reinterpret_cast<Ship *>(_shooter);
    }

    Ship* getShooter() {
        return shooter;
    }

    double getDamage() const {
        return damage;
    }

    double getRange() const {
        return range;
    }
};

#endif //SFML_TEMPLATE_SHOOTABLE_H
