//
// Created by benmo on 2/14/2020.
//

#ifndef SFML_TEMPLATE_GAME_H
#define SFML_TEMPLATE_GAME_H


#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "GameSprite.h"
#include "Ship.h"

class Game {
private:
    void init();

    bool playedBip = false, playedErr = false, soundOn = true, musicOn = true;
    sf::SoundBuffer bip;
    sf::Sound bipSound;
    sf::SoundBuffer err;
    sf::Sound errSound;

    sf::Texture loadingBarEmpty;
    sf::Texture loadingBarFull;
    sf::Font oxan;

    std::vector<std::string> abstractNounNameComponents;
    std::vector<std::string> adjectiveNameComponents;
    std::vector<std::string> animalNameComponents;
    std::vector<std::string> standaloneNameNameComponents;
    std::vector<std::string> femaleNameNameComponents;
    std::vector<std::string> femaleTitleNameComponents;
    std::vector<std::string> neutralTitleNameComponents;
    std::vector<std::string> maleNameNameComponents;
    std::vector<std::string> maleTitleNameComponents;
    std::vector<std::string> nounNameComponents;
    std::vector<std::string> numberNameComponents;
    std::vector<std::string> craftNameNameComponents;

    //update total
    int totalTextures = 195;
    int loadedTextures = 0;

    void playBip();
    void playErr();

    void readNameComponents();
    template<class RNG >
    std::string generateName(RNG &gen);

    void updateLoader(sf::RenderWindow &window, const std::string& msg);
public:
    const static int FRIENDLY_LOW = 100, NUETRAL_HIGH = 99, NUETRAL_LOW = 0, HOSTILE_HIGH = -1;

    Game(bool _soundOn, bool _musicOn) {
        soundOn = _soundOn;
        musicOn = _musicOn;

        init();
    }
};


#endif //SFML_TEMPLATE_GAME_H
