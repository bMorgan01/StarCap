//
// Created by benmo on 2/20/2020.
//

#include <iostream>
#include <windows.h>
#include <shellapi.h>
#include <winuser.h>
#include "Menu.h"
#include "GameSprite.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>

int Menu::init() {
    const int MENU = 0, CREDITS = 1;
    int screen = MENU;

    std::vector<GameSprite*> buttons;
    std::vector<sf::Text*> text;

    std::vector<sf::Text*> credits;
    std::vector<sf::Sprite*> creditsGUI;

    sf::RenderWindow window( sf::VideoMode(1240, 640), "Star Captain" );
    sf::View mainView;
    mainView.setCenter(window.getSize().x/(float)2.0, window.getSize().y/(float)2.0);
    mainView.setSize(window.getSize().x, window.getSize().y);
    window.setView(mainView);
    window.setPosition(sf::Vector2i(window.getPosition().x, window.getPosition().y - 20));

    /*************************************************
     * File Input && Processing
     *************************************************/
    sf::Font sk;
    sk.loadFromFile("./data/Fonts/Sk.ttf");

    sf::Font skME;
    skME.loadFromFile("./data/Fonts/SKoME.ttf");

    sf::Font xolo;
    xolo.loadFromFile("./data/Fonts/Xolonium-Bold.ttf");

    sf::Font monkirta;
    monkirta.loadFromFile("./data/Fonts/Monkirta Pursuit NC.ttf");

    sf::Font oxan;
    oxan.loadFromFile("./data/Fonts/Oxanium-Light.ttf");

    sf::Texture back;
    back.loadFromFile("./data/Gui/space.png");
    back.setRepeated(true);

    sf::Texture button;
    button.loadFromFile("./data/Gui/button.png");

    sf::Texture box;
    box.loadFromFile("./data/Gui/window.png");

    sf::Texture boxSm;
    boxSm.loadFromFile("./data/Gui/windowSm.png");

    sf::Texture cloud;
    cloud.loadFromFile("./data/Gui/cloud.png");

    sf::Texture leftArrow;
    leftArrow.loadFromFile("./data/Gui/Backward_BTN.png");

    sf::Texture soundBTN;
    soundBTN.loadFromFile("./data/Gui/Sound.png");

    sf::Texture musicBTN;
    musicBTN.loadFromFile("./data/Gui/Music.png");

    sf::Music menuLoop;
    menuLoop.openFromFile("./data/Sounds/Menu Loop.wav");
    menuLoop.setLoop(true);
    menuLoop.play();

    bip.loadFromFile("./data/Sounds/rollover.wav");

    /*************************************************
     * Object Initialization
     *************************************************/

    //Background pan sprite
    sf::Sprite background(back);
    sf::FloatRect fBounds(mainView.getCenter().x, mainView.getCenter().y, background.getTexture()->getSize().x * 3, background.getTexture()->getSize().y * 3);
    sf::IntRect iBounds(fBounds);

    background.setTextureRect(iBounds);
    background.setPosition(mainView.getCenter());
    background.setOrigin(iBounds.width/(float)2.0,iBounds.height/(float)2.0);

    //Sound settings
    GameSprite soundButton(soundBTN, 25, 35, 37, 0, 0);
    GameSprite musicButton(musicBTN, 25, soundButton.getXPos() + soundButton.getGlobalBounds().width, soundButton.getYPos(), 0, 0);

    //Title text
    sf::Text title("Star Captain", skME, 90);
    title.setPosition(mainView.getCenter().x, mainView.getCenter().y - window.getSize().y / (float)3.0);
    title.setFillColor(sf::Color::White);
    title.setLetterSpacing(title.getLetterSpacing() + (float)0.5);
    title.setOrigin(title.getGlobalBounds().width/(float)2.0, title.getGlobalBounds().height/(float)2.0);

    //Start button & text
    GameSprite startButton(button, 55);
    startButton.setPosition(mainView.getCenter().x, mainView.getCenter().y - window.getSize().y / (float)12.0);

    //default button color
    sf::Color defButtonColor = startButton.getColor();

    sf::Text startText("Start", sk, 28);
    startText.setPosition(startButton.getPosition().x, startButton.getPosition().y - 7);
    startText.setFillColor(sf::Color(0,0,0,0));
    startText.setOutlineColor(sf::Color::White);
    startText.setOutlineThickness(1);
    startText.setLetterSpacing(startText.getLetterSpacing() + 1);
    startText.setOrigin(startText.getLocalBounds().width/2, startText.getLocalBounds().height/2);

    //Credits button & text
    GameSprite creditsButton(button, 55);
    creditsButton.setPosition(mainView.getCenter().x, mainView.getCenter().y);

    sf::Text creditsText("Credits", sk, 28);
    creditsText.setPosition(creditsButton.getPosition().x, creditsButton.getPosition().y - 6);
    creditsText.setFillColor(sf::Color(0,0,0,0));
    creditsText.setOutlineColor(sf::Color::White);
    creditsText.setOutlineThickness(1);
    creditsText.setLetterSpacing(creditsText.getLetterSpacing() + 1);
    creditsText.setOrigin(creditsText.getLocalBounds().width/2, creditsText.getLocalBounds().height/2);

    //Exit button & text
    GameSprite exitButton(button, 55);
    exitButton.setPosition(mainView.getCenter().x, mainView.getCenter().y + window.getSize().y / (float)12.0);

    sf::Text exitText("Exit", sk, 28);
    exitText.setPosition(mainView.getCenter().x, exitButton.getPosition().y - 6);
    exitText.setFillColor(sf::Color(0,0,0,0));
    exitText.setOutlineColor(sf::Color::White);
    exitText.setOutlineThickness(1);
    exitText.setLetterSpacing(exitText.getLetterSpacing() + 1);
    exitText.setOrigin(exitText.getLocalBounds().width/2, exitText.getLocalBounds().height/2);

    buttons.push_back(&startButton);
    buttons.push_back(&creditsButton);
    buttons.push_back(&exitButton);

    text.push_back(&startText);
    text.push_back(&creditsText);
    text.push_back(&exitText);

    //Credits
    sf::Text creditsTitle("Credits + Resources", sk, 70);
    creditsTitle.setPosition(mainView.getCenter().x, window.getSize().y / (float)14.0);
    creditsTitle.setFillColor(sf::Color::White);
    creditsTitle.setOrigin(creditsTitle.getGlobalBounds().width/2, creditsTitle.getGlobalBounds().height/2);

    //Credits box
    sf::Text credsTitle("Credits", xolo, 28);
    credsTitle.setOrigin(credsTitle.getGlobalBounds().width/2, credsTitle.getGlobalBounds().height/2);
    credsTitle.setPosition(mainView.getSize().x/(float)5.1, mainView.getSize().y/2 + mainView.getSize().y/22);
    credsTitle.setFillColor(sf::Color::White);

    sf::Text music("River Schreckengost - ", monkirta, 20);
    music.setOrigin(music.getGlobalBounds().width/2, music.getGlobalBounds().height/2);
    music.setPosition(mainView.getSize().x/(float)5.8, mainView.getSize().y/(float)2 + mainView.getSize().y/(float)8.5);
    music.setFillColor(sf::Color::White);

    sf::Text musicText("Music", oxan, 15);
    musicText.setPosition(mainView.getSize().x/(float)5.8 + music.getGlobalBounds().width/2, mainView.getSize().y/(float)2 + mainView.getSize().y/(float)8.925);
    musicText.setFillColor(sf::Color::White);

    sf::Text musicLabel("Instagram - ", monkirta, 15);
    musicLabel.setPosition(mainView.getSize().x/(float)11.5, mainView.getSize().y/(float)2 + mainView.getSize().y/(float)7.3);
    musicLabel.setFillColor(sf::Color::White);

    sf::Text musicText0("@river.schreck", oxan, 12);
    musicText0.setPosition(mainView.getSize().x/(float)11.5 + musicLabel.getGlobalBounds().width, mainView.getSize().y/(float)2 + mainView.getSize().y/(float)7.15);
    musicText0.setFillColor(sf::Color::White);
    musicText0.setStyle(sf::Text::Style::Underlined);

    sf::Text musicLabel0("SoundCloud - ", monkirta, 15);
    musicLabel0.setPosition(mainView.getSize().x/(float)11.5, mainView.getSize().y/(float)2 + mainView.getSize().y/(float)6.1);
    musicLabel0.setFillColor(sf::Color::White);

    sf::Text musicText1("River Ethans", oxan, 12);
    musicText1.setPosition(mainView.getSize().x/(float)11.5 + musicLabel0.getGlobalBounds().width, mainView.getSize().y/(float)2 + mainView.getSize().y/(float)5.95);
    musicText1.setFillColor(sf::Color::White);
    musicText1.setStyle(sf::Text::Style::Underlined);

    //Resources Box
    sf::Text resourcesTitle("Resources", xolo, 28);
    resourcesTitle.setOrigin(resourcesTitle.getGlobalBounds().width/2, resourcesTitle.getGlobalBounds().height/2);
    resourcesTitle.setPosition(mainView.getSize().x  - mainView.getSize().x/(float)4.95, mainView.getSize().y/(float)4.8);
    resourcesTitle.setFillColor(sf::Color::White);

    //Dev box
    sf::Text developerTitle("Developer: ", xolo, 25);
    developerTitle.setPosition(mainView.getSize().x/13, mainView.getSize().y/(float)5.2);
    developerTitle.setFillColor(sf::Color(0,0,0,0));
    developerTitle.setOutlineThickness(.8);
    developerTitle.setOutlineColor(sf::Color::White);

    sf::Text developer("Benjamin Morgan", monkirta, 20);
    developer.setOrigin(developer.getGlobalBounds().width/2, developer.getGlobalBounds().height/2);
    developer.setPosition(mainView.getSize().x/(float)6.725, mainView.getSize().y/(float)3.95);
    developer.setFillColor(sf::Color::White);

    sf::Text devLabel("Site - ", monkirta, 15);
    devLabel.setPosition(mainView.getSize().x/(float)11.5, mainView.getSize().y/(float)3.65);
    devLabel.setFillColor(sf::Color::White);

    sf::Text devText("https://bmorgan01.github.io/Portfolio-Blog/", oxan, 12);
    devText.setPosition(mainView.getSize().x/(float)11.5 + devLabel.getGlobalBounds().width, mainView.getSize().y/(float)3.6);
    devText.setFillColor(sf::Color::White);
    devText.setStyle(sf::Text::Style::Underlined);

    sf::Text devLabel0("Github - ", monkirta, 15);
    devLabel0.setPosition(mainView.getSize().x/(float)11.5, mainView.getSize().y/(float)3.35);
    devLabel0.setFillColor(sf::Color::White);

    sf::Text devText0("bMorgan01", oxan, 12);
    devText0.setPosition(mainView.getSize().x/(float)11.5 + devLabel0.getGlobalBounds().width, mainView.getSize().y/(float)3.3);
    devText0.setFillColor(sf::Color::White);
    devText0.setStyle(sf::Text::Style::Underlined);

    sf::Text devLabel1("Email - ", monkirta, 15);
    devLabel1.setPosition(mainView.getSize().x/(float)11.5, mainView.getSize().y/(float)3.1);
    devLabel1.setFillColor(sf::Color::White);

    sf::Text devText1("ben.morgan5000@gmail.com", oxan, 12);
    devText1.setPosition(mainView.getSize().x/(float)11.5 + devLabel1.getGlobalBounds().width, mainView.getSize().y/(float)3.05);
    devText1.setFillColor(sf::Color::White);

    sf::Text devLabel2("Repo - ", monkirta, 15);
    devLabel2.setPosition(mainView.getSize().x/(float)11.5, mainView.getSize().y/(float)2.87);
    devLabel2.setFillColor(sf::Color::White);

    sf::Text devText2("https://github.com/bMorgan01/StarCap", oxan, 12);
    devText2.setPosition(mainView.getSize().x/(float)11.5 + devLabel2.getGlobalBounds().width, mainView.getSize().y/(float)2.83);
    devText2.setFillColor(sf::Color::White);
    devText2.setStyle(sf::Text::Style::Underlined);

    //Credits GUI
    sf::Sprite backButton(leftArrow);
    backButton.setScale(.3, .3);
    backButton.setPosition(33, 100);

    sf::Sprite textBox(box);
    textBox.setScale(.35, .35);
    textBox.setPosition(mainView.getSize().x  - mainView.getSize().x/3, mainView.getSize().y/(float)5.5);

    sf::Sprite textBoxSm(boxSm);
    textBoxSm.setScale(.35, .35);
    textBoxSm.setPosition(mainView.getSize().x/15, mainView.getSize().y/2 + mainView.getSize().y/50);

    sf::Sprite devBox(cloud);
    devBox.setScale(.442, .442);
    devBox.setPosition(mainView.getSize().x/15 - 2, mainView.getSize().y/(float)5.5);

    sf::Sprite issueButton(button);
    issueButton.setColor(sf::Color::Red);
    issueButton.setScale(40.0/100.0, 40.0/100.0);
    issueButton.setOrigin(issueButton.getGlobalBounds().width/2, issueButton.getGlobalBounds().height/2);
    issueButton.setPosition(mainView.getSize().x/(float)6.2, mainView.getSize().y/(float)2.52);

    sf::Text issueText("Report Bug", sk, 16);
    issueText.setPosition(issueButton.getPosition().x - 21, issueButton.getPosition().y);
    issueText.setFillColor(sf::Color(0,0,0,0));
    issueText.setFillColor(sf::Color::White);

    credits.push_back(&creditsTitle);
    credits.push_back(&developerTitle);
    credits.push_back(&credsTitle);
    credits.push_back(&music);
    credits.push_back(&musicText);
    credits.push_back(&musicLabel);
    credits.push_back(&musicText0);
    credits.push_back(&musicLabel0);
    credits.push_back(&musicText1);
    credits.push_back(&resourcesTitle);
    credits.push_back(&developer);
    credits.push_back(&devLabel);
    credits.push_back(&devText);
    credits.push_back(&devLabel0);
    credits.push_back(&devText0);
    credits.push_back(&devLabel1);
    credits.push_back(&devText1);
    credits.push_back(&devLabel2);
    credits.push_back(&devText2);
    credits.push_back(&issueText);

    creditsGUI.push_back(&backButton);
    creditsGUI.push_back(&textBox);
    creditsGUI.push_back(&textBoxSm);
    creditsGUI.push_back(&devBox);
    creditsGUI.push_back(&issueButton);

    while( window.isOpen() ) {
        /*********************************************
         * Pre-draw ops here.
         *********************************************/


        /*********************************************
         * Drawing goes here.
         *********************************************/

        window.clear( sf::Color::Black );           // clear the contents of the old frame

        window.draw(background);

        switch(screen) {
        case MENU:
            /**************
             * Draw Menu
             **************/
            backButton.setPosition(33, 100);

            //Sound buttons
            window.draw(soundButton);
            window.draw(musicButton);

            //Title text
            window.draw(title);

            for (int i = 0; i < buttons.size(); i++) {
                window.draw(*buttons[i]);
                window.draw(*text[i]);
            }
            break;
        case CREDITS:
            /**************
             * Draw Credits
             **************/
            backButton.setPosition(33, 27);

            for (sf::Sprite *s : creditsGUI) {
                window.draw(*s);
            }

            for (sf::Text *t : credits) {
                window.draw(*t);
            }

            break;
        }

        window.display();                       // display the window

        sf::Event event{};
        while( window.pollEvent(event) ) {      // ask the window if any events occurred

            /*********************************************
            * Event handling here.
            *********************************************/
            sf::Vector2i mousePos = sf::Mouse::getPosition( window );
            sf::Vector2f mousePosF( static_cast<float>( mousePos.x ), static_cast<float>( mousePos.y ) );

            switch (event.type) {
            case sf::Event::Closed:  //user clicked X button
                window.close();
                break;
            case sf::Event::MouseButtonPressed:   //User clicked mouse
                if (exitButton.getGlobalBounds().contains(mousePosF) && screen == MENU) {
                    playBip();
                    return EXIT_FAILURE;
                } else if (startButton.getGlobalBounds().contains(mousePosF) && screen == MENU) {
                    playBip();
                    return EXIT_SUCCESS;
                } else if (creditsButton.getGlobalBounds().contains(mousePosF) && screen == MENU) {
                    playBip();
                    screen = CREDITS;
                } else if (issueButton.getGlobalBounds().contains(mousePosF) && screen == CREDITS) {
                    playBip();
                    ShellExecute(nullptr, "open", "https://github.com/bMorgan01/StarCap/issues", nullptr, nullptr, SW_SHOWNORMAL);
                } else if (devText.getGlobalBounds().contains(mousePosF) && screen == CREDITS) {
                    playBip();
                    ShellExecute(nullptr, "open", "https://bmorgan01.github.io/Portfolio-Blog/", nullptr, nullptr, SW_SHOWNORMAL);
                } else if (devText0.getGlobalBounds().contains(mousePosF) && screen == CREDITS) {
                    playBip();
                    ShellExecute(nullptr, "open", "https://github.com/bMorgan01", nullptr, nullptr, SW_SHOWNORMAL);
                } else if (devText2.getGlobalBounds().contains(mousePosF) && screen == CREDITS) {
                    playBip();
                    ShellExecute(nullptr, "open", "https://github.com/bMorgan01/StarCap", nullptr, nullptr, SW_SHOWNORMAL);
                } else if (musicText0.getGlobalBounds().contains(mousePosF) && screen == CREDITS) {
                    playBip();
                    ShellExecute(nullptr, "open", "https://www.instagram.com/river.schreck/", nullptr, nullptr, SW_SHOWNORMAL);
                } else if (musicText1.getGlobalBounds().contains(mousePosF) && screen == CREDITS) {
                    playBip();
                    ShellExecute(nullptr, "open", "https://soundcloud.com/riverethans", nullptr, nullptr, SW_SHOWNORMAL);
                } else if (backButton.getGlobalBounds().contains(mousePosF) && screen == CREDITS) {
                    playBip();
                    screen = MENU;
                } else if (soundButton.getGlobalBounds().contains(mousePosF) && screen == MENU) {
                    soundOn = !soundOn;
                    playBip();
                } else if (musicButton.getGlobalBounds().contains(mousePosF) && screen == MENU) {
                    playBip();
                    musicOn = !musicOn;
                    if (!musicOn) menuLoop.setVolume(0);
                    else menuLoop.setVolume(100);
                }
                break;
            case sf::Event::MouseMoved:
                if (exitButton.getGlobalBounds().contains(mousePosF) && screen == MENU) exitButton.setColor(sf::Color::Red);
                else if (startButton.getGlobalBounds().contains(mousePosF) && screen == MENU) startButton.setColor(sf::Color::Red);
                else if (creditsButton.getGlobalBounds().contains(mousePosF) && screen == MENU) creditsButton.setColor(sf::Color::Red);
                else if (issueButton.getGlobalBounds().contains(mousePosF) && screen == CREDITS) issueButton.setColor(sf::Color::Green);
                else if (devText.getGlobalBounds().contains(mousePosF) && screen == CREDITS) devText.setFillColor(sf::Color::Red);
                else if (devText0.getGlobalBounds().contains(mousePosF) && screen == CREDITS) devText0.setFillColor(sf::Color::Red);
                else if (devText2.getGlobalBounds().contains(mousePosF) && screen == CREDITS) devText2.setFillColor(sf::Color::Red);
                else if (musicText0.getGlobalBounds().contains(mousePosF) && screen == CREDITS) musicText0.setFillColor(sf::Color::Red);
                else if (musicText1.getGlobalBounds().contains(mousePosF) && screen == CREDITS) musicText1.setFillColor(sf::Color::Red);
                else if (backButton.getGlobalBounds().contains(mousePosF) && screen == CREDITS) backButton.setColor(sf::Color::Red);
                else if (soundButton.getGlobalBounds().contains(mousePosF) && soundOn && screen == MENU) soundButton.setColor(sf::Color::Red);
                else if (musicButton.getGlobalBounds().contains(mousePosF) && musicOn && screen == MENU) musicButton.setColor(sf::Color::Red);
                else if (soundButton.getGlobalBounds().contains(mousePosF) && !soundOn && screen == MENU) soundButton.setColor(sf::Color::White);
                else if (musicButton.getGlobalBounds().contains(mousePosF) && !musicOn && screen == MENU) musicButton.setColor(sf::Color::White);
                break;
            }

            if (!exitButton.getGlobalBounds().contains(mousePosF)) exitButton.setColor(defButtonColor);
            if (!startButton.getGlobalBounds().contains(mousePosF)) startButton.setColor(defButtonColor);
            if (!creditsButton.getGlobalBounds().contains(mousePosF)) creditsButton.setColor(defButtonColor);
            if (!issueButton.getGlobalBounds().contains(mousePosF)) issueButton.setColor(sf::Color::Red);
            if (!devText.getGlobalBounds().contains(mousePosF)) devText.setFillColor(sf::Color::White);
            if (!devText0.getGlobalBounds().contains(mousePosF)) devText0.setFillColor(sf::Color::White);
            if (!devText2.getGlobalBounds().contains(mousePosF)) devText2.setFillColor(sf::Color::White);
            if (!musicText0.getGlobalBounds().contains(mousePosF)) musicText0.setFillColor(sf::Color::White);
            if (!musicText1.getGlobalBounds().contains(mousePosF)) musicText1.setFillColor(sf::Color::White);
            if (!backButton.getGlobalBounds().contains(mousePosF)) backButton.setColor(defButtonColor);
            if (!soundButton.getGlobalBounds().contains(mousePosF) && soundOn) soundButton.setColor(sf::Color::White);
            if (!musicButton.getGlobalBounds().contains(mousePosF) && musicOn) musicButton.setColor(sf::Color::White);
            if (!soundButton.getGlobalBounds().contains(mousePosF) && !soundOn) soundButton.setColor(sf::Color::Red);
            if (!musicButton.getGlobalBounds().contains(mousePosF) && !musicOn) musicButton.setColor(sf::Color::Red);
        }
    }

    return EXIT_FAILURE;
}

void Menu::playBip() {
    if (soundOn) {
        bipSound.setBuffer(bip);
        bipSound.setVolume(100);
        bipSound.play();
    }
}