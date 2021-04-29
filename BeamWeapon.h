//
// Created by Benjamin on 4/23/2021.
//

#ifndef SFML_TEMPLATE_BEAMWEAPON_H
#define SFML_TEMPLATE_BEAMWEAPON_H

#include <thread>
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

        if (noise.getStatus() != sf::Sound::Playing) noise.play();

        projectile.setShooter((GameSprite *) shooter);

        sf::Vector2f adjusted(shooter->getXPos() + shooter->getLocalBounds().width/4 * shooter->getScale().x * cos(shooter->getDirection()*GameSprite::PI/180), shooter->getYPos() - shooter->getLocalBounds().width/4 * shooter->getScale().x * sin(shooter->getDirection()*GameSprite::PI/180));
        double newWidth = shooter->getTarget() == nullptr ? projectile.getRange() : GameSprite::distance(adjusted, shooter->getTarget()->getPosition());
        projectile.setTextureRect(sf::IntRect(projectile.getTextureRect().left, projectile.getTextureRect().top, newWidth > projectile.getRange() ? projectile.getRange() * (1/projectile.getScale().x) : newWidth * (1/projectile.getScale().x), projectile.getTextureRect().height));
        projectile.setOrigin(0, projectile.getLocalBounds().height/2);
        projectile.setPosition(adjusted);
        projectile.setDirection(shooter->getTarget() == nullptr ? shooter->getDirection() : -GameSprite::getAimAngle(shooter->getTarget()->getPosition(), projectile.getPosition()));

        if (framesShot == duration) {
            framesShot = 0;
            currentFrame = 0;
            noise.stop();
        }

        return new Shootable(projectile);
    }
};


#endif //SFML_TEMPLATE_BEAMWEAPON_H
