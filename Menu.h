//
// Created by benmo on 2/20/2020.
//

#ifndef SFML_TEMPLATE_MENU_H
#define SFML_TEMPLATE_MENU_H

#include <SFML/Audio.hpp>

class Menu {
public:
    int result;
    bool soundOn = true, musicOn = true;

    Menu() {
        result = init();
    }
private:
    sf::SoundBuffer bip;
    sf::Sound bipSound;
    bool playedBip = false;

    int init();
    void playBip();
};


#endif //SFML_TEMPLATE_MENU_H
