//
// Created by Benjamin on 4/20/2021.
//

#ifndef SFML_TEMPLATE_WEAPON_H
#define SFML_TEMPLATE_WEAPON_H

#include "Projectile.h"
#include "GameSprite.h"
#include <SFML/Audio.hpp>
#include <iostream>

class Weapon {
protected:
    int frameDelay, currentFrame;
    Shootable projectile;
    sf::Sound noise;
    double effectiveAngle;

    Weapon() {
        frameDelay = 0;
        currentFrame = 0;

        projectile = Shootable();
    };

    explicit Weapon(int _frameDelay, double _effectiveAngle, const sf::SoundBuffer& _buffer, float volume) : Weapon() {
        effectiveAngle = _effectiveAngle;
        frameDelay = _frameDelay;

        noise.setBuffer(_buffer);
        noise.setVolume(volume);
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

    double getEffectiveAngle() const {
        return effectiveAngle;
    }
};

#endif //SFML_TEMPLATE_WEAPON_H
