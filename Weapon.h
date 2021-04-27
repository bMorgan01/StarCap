//
// Created by Benjamin on 4/20/2021.
//

#ifndef SFML_TEMPLATE_WEAPON_H
#define SFML_TEMPLATE_WEAPON_H

#include "Projectile.h"
#include "GameSprite.h"
#include <iostream>

class Weapon {
protected:
    int frameDelay, currentFrame;
    Shootable projectile;

    Weapon() {
        frameDelay = 0;
        currentFrame = 0;

        projectile = Shootable();
    };

    explicit Weapon(int _frameDelay) : Weapon() {
        frameDelay = _frameDelay;
    }
public:
    virtual Shootable* shoot(const Ship* shooter) {
        return new Shootable(projectile);
    }

    void recharge() {
        if (currentFrame < frameDelay) {
            currentFrame++;
        }
    }

    bool canShoot() const {
        return currentFrame == frameDelay;
    }

    Shootable& getProjectile() {
        return projectile;
    }
};

#endif //SFML_TEMPLATE_WEAPON_H
