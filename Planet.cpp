#include "Planet.h"

Planet::Planet(const sf::Texture &texture, float scale, float xPos, float yPos, float direction) : GameSprite(texture, scale, xPos, yPos, 0, direction) {
    landable = false;
}

Planet::Planet(const std::string& _name, const std::string& _desc, int landscape, const sf::Texture &texture, float scale, float xPos, float yPos, float direction) : GameSprite(texture, scale, xPos, yPos, 0, direction) {
    landable = true;
    name = _name;
    desc = _desc;
    image = landscape;
}

bool Planet::isLandable() {
    return landable;
}

std::string Planet::getName() {
    return name;
}

std::string Planet::getDesc() {
    return desc;
}

int Planet::getImageNum() {
    return image;
}