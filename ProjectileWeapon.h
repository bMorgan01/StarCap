#include <utility>

//
// Created by Benjamin on 4/20/2021.
//

#ifndef SFML_TEMPLATE_PROJECTILEWEAPON_H
#define SFML_TEMPLATE_PROJECTILEWEAPON_H


class ProjectileWeapon : public Weapon {
public:
    ProjectileWeapon(Projectile _proj, int frameDelay) : Weapon(frameDelay) {
        projectile = std::move(_proj);
    }

    Shootable* shoot(const Ship* shooter) {
        currentFrame = 0;

        projectile.setDirection(shooter->getDirection());
        projectile.setPosition(shooter->getPosition());
        projectile.setShooter((GameSprite *) shooter);

        Shootable *copied = new Shootable(projectile);

        return copied;
    }
};


#endif //SFML_TEMPLATE_PROJECTILEWEAPON_H
