//
// Created by Benjamin on 4/20/2021.
//

#ifndef SFML_TEMPLATE_PROJECTILE_H
#define SFML_TEMPLATE_PROJECTILE_H

#include "Shootable.h"

class Projectile : public Shootable {
public:
    Projectile() = default;

    Projectile(const sf::Texture& texture, const sf::IntRect &rect, double scale, int rows, int cols, int xOffset, int yOffset, int frameDelay, double velocity, double damage, double _range) : Shootable(texture, rect, scale, rows, cols, xOffset, yOffset, frameDelay, damage) {
        setVelocity(velocity);

        lifetime = _range/velocity;
        range = _range;
    }
};

#endif //SFML_TEMPLATE_PROJECTILE_H
