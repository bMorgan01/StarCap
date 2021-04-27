//
// Created by Benjamin on 4/23/2021.
//

#ifndef SFML_TEMPLATE_BEAM_H
#define SFML_TEMPLATE_BEAM_H


class Beam : public Shootable {
public:
    Beam(const sf::Texture& texture, const sf::IntRect &rect, double scale, int rows, int cols, int xOffset, int yOffset, int frameDelay, double damage, double _range) : Shootable(texture, rect, scale, rows, cols, xOffset, yOffset, frameDelay, damage) {
        lifetime = 1;

        range = _range;
    }
};


#endif //SFML_TEMPLATE_BEAM_H
