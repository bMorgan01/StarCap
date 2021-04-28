#include <utility>

//
// Created by Benjamin on 4/20/2021.
//

#ifndef SFML_TEMPLATE_PROJECTILEWEAPON_H
#define SFML_TEMPLATE_PROJECTILEWEAPON_H


class ProjectileWeapon : public Weapon {
public:
    ProjectileWeapon(Projectile _proj, const sf::SoundBuffer& buffer, float volume, double effectiveAngle, int frameDelay) : Weapon(frameDelay, effectiveAngle, buffer, volume) {
        projectile = std::move(_proj);
    }

    Shootable* shoot(const Ship* shooter) override {
        currentFrame = 0;

        noise.play();

        projectile.setDirection(shooter->getDirection());
        projectile.setPosition(shooter->getXPos() + shooter->getLocalBounds().width/4 * shooter->getScale().x * cos(shooter->getDirection()*GameSprite::PI/180), shooter->getYPos() - shooter->getLocalBounds().width/4 * shooter->getScale().x * sin(shooter->getDirection()*GameSprite::PI/180));
        projectile.setShooter((GameSprite *) shooter);

        Shootable *copied = new Shootable(projectile);

        return copied;
    }
};


#endif //SFML_TEMPLATE_PROJECTILEWEAPON_H
