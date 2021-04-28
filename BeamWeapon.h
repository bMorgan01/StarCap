//
// Created by Benjamin on 4/23/2021.
//

#ifndef SFML_TEMPLATE_BEAMWEAPON_H
#define SFML_TEMPLATE_BEAMWEAPON_H

#include "Beam.h"

class BeamWeapon : public Weapon {
private:
    int duration;
    int framesShot = 0;
public:
    BeamWeapon(Beam _proj, const sf::SoundBuffer& buffer, float volume, double effectiveAngle, int _duration, int frameDelay) : Weapon(frameDelay, effectiveAngle, buffer, volume) {
        projectile = std::move(_proj);
        duration = _duration;
    }

    Shootable* shoot(const Ship* shooter) override {
        framesShot++;
        if (framesShot == duration) {
            framesShot = 0;
            currentFrame = 0;
        }

        noise.play();

        projectile.setShooter((GameSprite *) shooter);
        double newWidth = shooter->getTarget() == nullptr ? projectile.getRange() : GameSprite::distance(shooter->getPosition(), shooter->getTarget()->getPosition());
        projectile.setTextureRect(sf::IntRect(projectile.getTextureRect().left, projectile.getTextureRect().top, newWidth > projectile.getRange() ? projectile.getRange() * (1/projectile.getScale().x) : newWidth * (1/projectile.getScale().x), projectile.getTextureRect().height));
        projectile.setOrigin(0, projectile.getLocalBounds().height/2);
        projectile.setDirection(shooter->getTarget() == nullptr ? shooter->getDirection() : -GameSprite::getAimAngle(shooter->getTarget()->getPosition(), shooter->getPosition()));
        projectile.setPosition(shooter->getPosition());

        return new Shootable(projectile);
    }
};


#endif //SFML_TEMPLATE_BEAMWEAPON_H
