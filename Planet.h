//
// Created by benmo on 3/3/2020.
//

#ifndef SFML_TEMPLATE_PLANET_H
#define SFML_TEMPLATE_PLANET_H

#include "GameSprite.h"

class Planet : public GameSprite {
private:
    bool landable = true;
    std::string name;
    std::string desc;
    int image;
public:
    Planet(const sf::Texture &texture, float scale, float xPos, float yPos, float direction);
    Planet(const std::string& _name, const std::string& _desc, int landscape, const sf::Texture &texture, float scale, float xPos, float yPos, float direction);

    bool isLandable();

    std::string getName();
    std::string getDesc();

    int getImageNum();
};


#endif //SFML_TEMPLATE_PLANET_H
