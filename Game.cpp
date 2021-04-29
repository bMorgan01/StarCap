//
// Created by benmo on 2/14/2020.
//

#include <iostream>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <chrono>
#include <SFML/Audio.hpp>
#include <queue>
#include <random>
#include "Game.h"
#include "System.h"
#include "COMShip.h"
#include "Explore.h"
#include "ProjectileWeapon.h"
#include "Collision.h"
#include "BeamWeapon.h"

std::vector<std::string> open(const std::string &path) {
    std::vector<std::string> files;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            files.emplace_back(ent->d_name);
        }
        closedir(dir);
    } else {
        /* could not open directory */
        perror("");
    }

    return files;
}

std::string wordWrap(const std::string &text, float length, const sf::Font &font, unsigned int charSize) {
    std::istringstream iss(text);
    std::vector<std::string> results((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());

    sf::Text temp;
    temp.setFont(font);
    temp.setCharacterSize(charSize);

    std::string tempStr;
    std::string returnStr;
    for (const std::string &s : results) {
        tempStr += s + " ";
        temp.setString(tempStr);
        if (temp.getGlobalBounds().width < length) returnStr += s + " ";
        else {
            returnStr += "\n" + s + " ";
            tempStr = s;
        }
    }

    return returnStr;
}

std::vector<std::string> readFileLines(const std::string &path) {
    std::vector<std::string> lines;
    std::ifstream inFile(path);

    if (!inFile.is_open()) {
        std::cerr << "Cannot open file: " + path << std::endl;
        return lines;
    }

    while (!inFile.eof()) {
        std::string input;

        std::getline(inFile, input);
        lines.push_back(input);
    }

    inFile.close();
    return lines;
}

template<class T, class RNG>
T pickRandomItem(std::vector<T> items, RNG &gen) {
    int index = std::uniform_int_distribution<int>(0, items.size() - 1)(gen);

    return items[index];
}

void Game::init() {
    const int DESC = 0, YARD = 1, TASKS = 2;
    int planetScreen = DESC;

    std::vector<GameSprite *> objects;
    std::vector<Shootable *> projectiles;
    std::vector<COMShip *> ships;
    std::vector<System *> systems;
    std::vector<GameSprite *> mapSprites;
    std::vector<sf::Vertex *> mapLines;

    std::vector<sf::Texture *> planets;
    std::vector<sf::Texture *> stars;
    std::vector<sf::Texture *> images;

    std::vector<sf::Texture *> shipTextures;
    std::vector<int> shipTextureScales;

    std::deque<sf::Text *> messageLog;

    std::vector<GameSprite *> shipyardMenu;
    std::vector<sf::Text *> shipyardMenuText;

    std::vector<sf::Text *> missions;
    std::vector<sf::Text *> missionSizes;
    std::vector<sf::Text *> missionAccepts;
    std::vector<GameSprite *> missionButtons;
    std::vector<sf::RectangleShape *> missionButtonFilters;

    std::vector<Task *> activeMissions;

    std::vector<Explore *> explores;

    sf::RenderWindow window(sf::VideoMode(1240, 640), "Star Captain");
    sf::View mainView;
    mainView.setCenter(window.getSize().x / (float) 2.0, window.getSize().y / (float) 2.0);
    mainView.setSize(window.getSize().x, window.getSize().y);
    window.setView(mainView);
    window.setFramerateLimit(60);
    window.setPosition(sf::Vector2i(window.getPosition().x, window.getPosition().y - 20));

    /*************************************************
     * Set Up Loading Screen
     *************************************************/
    oxan.loadFromFile("./data/Fonts/Oxanium-Light.ttf");
    updateLoader(window, "Preparing load screen...");
    auto start = std::chrono::high_resolution_clock::now();
    loadingBarEmpty.loadFromFile("./data/Gui/LoadingBarEmpty.png");
    updateLoader(window, "Preparing load screen...");
    loadingBarFull.loadFromFile("./data/Gui/LoadingBarFull.png");
    updateLoader(window, "Preparing load screen...");
    loadingBarFull.setRepeated(false);

    /*************************************************
     * File Input && Processing
     *************************************************/

    readNameComponents();

    sf::Texture ship;
    ship.loadFromFile("./data/Ships/39.png");
    updateLoader(window, "Loading player texture...");

    sf::Texture back;
    back.loadFromFile("./data/Gui/space.png");
    back.setRepeated(true);
    updateLoader(window, "Loading background image...");

    sf::Texture pointer;
    pointer.loadFromFile("./data/Gui/arrow.png");
    updateLoader(window, "");

    //Load star textures
    for (const std::string &s : open((std::string) "./data/Stars")) {
        if (s == ".." || s == ".") continue;
        stars.push_back(new sf::Texture());
        stars[stars.size() - 1]->loadFromFile("./data/Stars/" + s);
        updateLoader(window, "Loading star textures...");
    }

    //Load planet textures
    for (const std::string &s : open((std::string) "./data/Planets")) {
        if (s == ".." || s == ".") continue;
        planets.push_back(new sf::Texture());
        planets[planets.size() - 1]->loadFromFile("./data/Planets/" + s);
        updateLoader(window, "Loading planet textures...");
    }

    //Load landscape textures
    for (const std::string &s : open((std::string) "./data/Images")) {
        if (s == ".." || s == ".") continue;
        images.push_back(new sf::Texture());
        images[images.size() - 1]->loadFromFile("./data/Images/" + s);
        updateLoader(window, "Loading planet landscapes...");
    }

    //Load ship textures
    for (const std::string &s : open((std::string) "./data/Ships")) {
        if (s == ".." || s == ".") continue;
        shipTextures.push_back(new sf::Texture());
        Collision::CreateTextureAndBitmask(*shipTextures[shipTextures.size() - 1], "./data/Ships/" + s);
        shipTextureScales.push_back(50);
        updateLoader(window, "Loading ship textures...");
    }

    //Load projectile textures and sounds
    sf::Texture laser;
    Collision::CreateTextureAndBitmask(laser, "./data/Projectiles/laser.png");

    sf::Sound laserNoise;
    sf::SoundBuffer laserBuffer;
    laserBuffer.loadFromFile("./data/Sounds/Projectiles/laser.wav");

    sf::Texture beam;
    Collision::CreateTextureAndBitmask(beam, "./data/Projectiles/beam.png");
    beam.setRepeated(true);

    sf::Sound beamNoise;
    sf::SoundBuffer beamBuffer;
    beamBuffer.loadFromFile("./data/Sounds/Projectiles/beam.wav");

    //load GUI textures
    sf::Texture mapWin;
    mapWin.loadFromFile("./data/Gui/mapBox.png");
    updateLoader(window, "Loading GUI elements...");

    sf::Texture ball;
    ball.loadFromFile("./data/Gui/ball.png");
    updateLoader(window, "Loading GUI elements...");

    sf::Texture ring;
    ring.loadFromFile("./data/Gui/unexplored.png");
    updateLoader(window, "Loading GUI elements...");

    sf::Texture box;
    box.loadFromFile("./data/Gui/window.png");
    updateLoader(window, "Loading GUI elements...");

    sf::Texture win;
    win.loadFromFile("./data/Gui/windowSm.png");
    updateLoader(window, "Loading GUI elements...");

    sf::Texture dialog;
    dialog.loadFromFile("./data/Gui/planetDialog.png");
    updateLoader(window, "Loading GUI elements...");

    sf::Texture button;
    button.loadFromFile("./data/Gui/button.png");
    updateLoader(window, "Loading GUI elements...");

    sf::Texture buttonSm;
    buttonSm.loadFromFile("./data/Gui/smallButton.png");
    updateLoader(window, "Loading GUI elements...");

    sf::Texture gaugeBar;
    gaugeBar.loadFromFile("./data/Gui/FuelBar.png");
    updateLoader(window, "Loading GUI elements...");

    sf::Texture shield;
    shield.loadFromFile("./data/Gui/Armor_Icon.png");
    updateLoader(window, "Loading GUI elements...");

    sf::Texture hull;
    hull.loadFromFile("./data/Gui/HP_Icon.png");
    updateLoader(window, "Loading GUI elements...");

    sf::Texture fuel;
    fuel.loadFromFile("./data/Gui/Speed_Icon.png");
    updateLoader(window, "Loading GUI elements...");

    sf::Texture fleetTargetIndicator;
    fleetTargetIndicator.loadFromFile("./data/Gui/fleet_target.png");
    updateLoader(window, "Loading GUI elements...");

    sf::Texture friendlyTargetIndicator;
    friendlyTargetIndicator.loadFromFile("./data/Gui/friendly_target.png");
    updateLoader(window, "Loading GUI elements...");

    sf::Texture nuetralTargetIndicator;
    nuetralTargetIndicator.loadFromFile("./data/Gui/neutral_target.png");
    updateLoader(window, "Loading GUI elements...");

    sf::Texture hostileTargetIndicator;
    hostileTargetIndicator.loadFromFile("./data/Gui/hostile_target.png");
    updateLoader(window, "Loading GUI elements...");

    sf::Texture nuetralTargetTrackArrow;
    nuetralTargetTrackArrow.loadFromFile("./data/Gui/neutral_target_tracker.png");

    sf::Texture friendlyTargetTrackArrow;
    friendlyTargetTrackArrow.loadFromFile("./data/Gui/friendly_target_tracker.png");

    sf::Texture hostileTargetTrackArrow;
    hostileTargetTrackArrow.loadFromFile("./data/Gui/hostile_target_tracker.png");

    sf::Font xolo;
    xolo.loadFromFile("./data/Fonts/Xolonium-Bold.ttf");
    updateLoader(window, "Loading GUI elements...");

    sf::Font monkirta;
    monkirta.loadFromFile("./data/Fonts/Monkirta Pursuit NC.ttf");
    updateLoader(window, "Loading GUI elements...");

    sf::Font sk;
    sk.loadFromFile("./data/Fonts/Sk.ttf");
    updateLoader(window, "Loading GUI elements...");

    sf::Texture soundBTN;
    soundBTN.loadFromFile("./data/Gui/Sound.png");
    updateLoader(window, "Loading GUI elements...");

    sf::Texture musicBTN;
    musicBTN.loadFromFile("./data/Gui/Music.png");
    updateLoader(window, "Loading GUI elements...");

    sf::Texture moneyBar;
    moneyBar.loadFromFile("./data/Gui/money_bar.png");
    updateLoader(window, "Loading GUI elements...");

    sf::Texture statsBar;
    statsBar.loadFromFile("./data/Gui/Stats_Bar.png");
    updateLoader(window, "Loading GUI elements...");

    sf::Texture leftArrow;
    leftArrow.loadFromFile("./data/Gui/Backward_BTN.png");
    updateLoader(window, "Loading GUI elements...");

    sf::SoundBuffer warp;
    warp.loadFromFile("./data/Sounds/takeoff.wav");
    updateLoader(window, "Loading sounds...");

    sf::Sound sound;

    int neutralVol = 40;

    sf::Music neutralLoop;
    neutralLoop.openFromFile("./data/Sounds/Unknown Theme.wav");
    updateLoader(window, "Loading sounds...");
    neutralLoop.setLoop(true);
    neutralLoop.setVolume((float) neutralVol);
    if (musicOn) neutralLoop.play();

    bip.loadFromFile("./data/Sounds/rollover.wav");
    updateLoader(window, "Loading sounds...");

    err.loadFromFile("./data/Sounds/error.wav");
    updateLoader(window, "Loading sounds...");

    //Load systems and add planets
    std::ifstream inFile("./data/Systems.txt");
    std::ifstream descFile;
    std::string input;

    while (std::getline(inFile, input)) {
        systems.push_back(new System(input));
        std::getline(inFile, input);
        systems[systems.size() - 1]->setGovName(input);
        std::getline(inFile, input);
        std::istringstream iss(input);
        std::vector<std::string> results((std::istream_iterator<std::string>(iss)),
                                         std::istream_iterator<std::string>());
        systems[systems.size() - 1]->setPop(stoi(results[0]));
        systems[systems.size() - 1]->setStren(stoi(results[1]));
        std::getline(inFile, input);
        iss = std::istringstream(input);
        results = std::vector<std::string>((std::istream_iterator<std::string>(iss)),
                                           std::istream_iterator<std::string>());
        systems[systems.size() - 1]->addPlanet(
                new Planet(*stars[stoi(results[0])], stof(results[1]), window.getSize().x * stof(results[2]),
                           window.getSize().y * stof(results[3]), stof(results[4])));
        while (std::getline(inFile, input)) {
            if (input[0] == 'E') break;
            iss = std::istringstream(input);
            results = std::vector<std::string>((std::istream_iterator<std::string>(iss)),
                                               std::istream_iterator<std::string>());
            if (results.size() <= 5)
                systems[systems.size() - 1]->addPlanet(
                        new Planet(*planets[stoi(results[0])], stof(results[1]), window.getSize().x * stof(results[2]),
                                   window.getSize().y * stof(results[3]), stof(results[4])));
            else {
                descFile = std::ifstream("./data/sysDescs/" + results[5] + ".txt");
                std::string temp;
                std::getline(descFile, temp);
                systems[systems.size() - 1]->addPlanet(
                        new Planet(results[5], wordWrap(temp, 305, oxan, 15), stoi(results[6]),
                                   *planets[stoi(results[0])], stof(results[1]), window.getSize().x * stof(results[2]),
                                   window.getSize().y * stof(results[3]), stof(results[4])));
            }
        }

        iss = std::istringstream(input);
        results = std::vector<std::string>((std::istream_iterator<std::string>(iss)),
                                           std::istream_iterator<std::string>());

        for (int i = 1; i < results.size(); i++) systems[systems.size() - 1]->addExit(stoi(results[i]));
        std::getline(inFile, input);

        iss = std::istringstream(input);
        results = std::vector<std::string>((std::istream_iterator<std::string>(iss)),
                                           std::istream_iterator<std::string>());

        systems[systems.size() - 1]->setRelativeMapPos(sf::Vector2f(stof(results[0]), stof(results[1])));
        std::getline(inFile, input);

        updateLoader(window, "Preparing systems...");
    }

    inFile.close();

    inFile.open("./data/Explores.txt");
    while (std::getline(inFile, input)) {
        std::istringstream iss(input);
        std::vector<std::string> results;
        iss = std::istringstream(input);
        results = std::vector<std::string>((std::istream_iterator<std::string>(iss)),
                                           std::istream_iterator<std::string>());
        std::getline(inFile, input);
        std::string type = input;
        std::getline(inFile, input);
        explores.push_back(
                new Explore(type, input, stoi(results[0]), stoi(results[1]), stoi(results[2]), stoi(results[3])));
        std::getline(inFile, input);

        updateLoader(window, "Loading Explores...");
    }

    inFile.close();

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << duration.count() << " milliseconds" << std::endl;

    std::cout << loadedTextures << std::endl;
    if (loadedTextures != totalTextures) std::cout << "UPDATE!!!" << std::endl;

    /*************************************************
     * Object Initialization
     *************************************************/

    GameSprite background(back, 100);

    sf::FloatRect fBounds(mainView.getCenter().x, mainView.getCenter().y, background.getTexture()->getSize().x * 3,
                          background.getTexture()->getSize().y * 3);
    sf::IntRect iBounds(fBounds);

    background.setTextureRect(iBounds);
    background.setPosition(mainView.getCenter());
    background.setOrigin(iBounds.width / (float) 2.0, iBounds.height / (float) 2.0);

    sf::RectangleShape jumpFilter;
    jumpFilter.setFillColor(sf::Color(255, 255, 255, 0));
    jumpFilter.setSize(sf::Vector2f(iBounds.width, iBounds.height));
    jumpFilter.setOrigin(iBounds.width / (float) 2.0, iBounds.height / (float) 2.0);

    bool map = false;
    int landing = -9999;
    float landingdx = 0, landingdy = 0;
    Planet *landingPlanet = nullptr;

    int jumping = -9999;
    System *destSystem = nullptr;

    //workaround for SFML bug, fix if patched
    sf::Vector2f oldPos;
    sf::Vector2f currentPos;

    Ship *player = new Ship(ship, 50, window.getSize().x / (float) 2.0, window.getSize().y / (float) 2.0, 0, 10, -45, 2,3, 500, 100, 5);
    player->addWeapon(new BeamWeapon(Beam(beam, sf::IntRect(0, 207, 535, 91), 10, 12, 1, 0, 513, 3, 1, 1500), beamBuffer, 100, 360 , 25, 40));
    int playerMoney = 100000;

    System *currentSystem = systems[0];
    currentSystem->makeVisited();

    std::mt19937 gen(time(0));
    std::uniform_int_distribution<int> roll(1, (currentSystem->getPop() <= 6) ? currentSystem->getPop() : 6);

    //create tasks
    int numTasks = roll(gen);
    if (currentSystem->isLandable()) {
        for (auto t : currentSystem->getTasks()) delete t;
        currentSystem->getTasks().clear();

        for (int i = 0; i < numTasks; i++) {
            int sysNum = -1, planetNum = -1;
            while (sysNum == -1 || !systems[sysNum]->isLandable() || systems[sysNum] == currentSystem) {
                roll = std::uniform_int_distribution<int>(0, systems.size() - 1);
                sysNum = roll(gen);
            }

            while (planetNum == -1 || !systems[sysNum]->getPlanets()[planetNum]->isLandable()) {
                roll = std::uniform_int_distribution<int>(0, systems[sysNum]->getPlanets().size() - 1);
                planetNum = roll(gen);
            }

            std::string taskName;
            roll = std::uniform_int_distribution<int>(0, 1);
            int type = roll(gen);
            switch (type) {
                case 0: //delivery
                    taskName = "Delivery: " + systems[sysNum]->getPlanets()[planetNum]->getName() + " - " +
                               systems[sysNum]->getName();
                    break;
                case 1: //taxi
                    taskName = "Passengers: " + systems[sysNum]->getPlanets()[planetNum]->getName() + " - " +
                               systems[sysNum]->getName();
                    break;
            }

            int size = 0;
            int reward = 0;
            switch (type) {
                case 0: //delivery
                    roll = std::uniform_int_distribution<int>(25, player->getCargoSpace());
                    size = roll(gen);

                    roll = std::uniform_int_distribution<int>(950, 1050);
                    reward = roll(gen) * size;

                    break;
                case 1:
                    roll = std::uniform_int_distribution<int>(1, player->getPassengerSpace());
                    size = roll(gen);

                    roll = std::uniform_int_distribution<int>(20000, 30000);
                    reward = roll(gen) * size;

                    break;
            }


            currentSystem->addTask(
                    new Task(type, taskName, taskName, systems[sysNum], systems[sysNum]->getPlanets()[planetNum],
                             reward,
                             size));

            missions.push_back(new sf::Text(taskName, oxan, 15));
            missions.push_back(new sf::Text(
                    "$" + std::to_string(currentSystem->getTasks()[currentSystem->getTasks().size() - 1]->getReward()),
                    oxan, 12));
            missionSizes.push_back(new sf::Text(
                    std::to_string(currentSystem->getTasks()[currentSystem->getTasks().size() - 1]->getSize()) +
                    " units", oxan, 12));

            missionButtons.push_back(new GameSprite(buttonSm, 65));
            missionButtons[missionButtons.size() - 1]->setColor(sf::Color::Green);

            missionAccepts.push_back(new sf::Text("Accept" + std::to_string(missionAccepts.size()), monkirta, 18));
            missionAccepts[missionAccepts.size() - 1]->setString("Accept");
            missionAccepts[missionAccepts.size() - 1]->setOrigin(
                    missionAccepts[missionAccepts.size() - 1]->getGlobalBounds().width / 2,
                    missionAccepts[missionAccepts.size() - 1]->getGlobalBounds().height / 2);

            missionButtonFilters.push_back(new sf::RectangleShape(
                    sf::Vector2f(missionButtons[0]->getGlobalBounds().width,
                                 missionButtons[0]->getGlobalBounds().height)));
            missionButtonFilters[missionButtonFilters.size() - 1]->setFillColor(sf::Color(0, 0, 0, 0));
        }
    }

    //target HUD
    GameSprite targetCircle(nuetralTargetIndicator);

    GameSprite targetWindow(win, 20);
    targetWindow.setOrigin(0, 0);

    sf::Text targetScanTitle("Target Scan", xolo, 20);
    targetScanTitle.setOrigin(targetScanTitle.getGlobalBounds().width / 2, 0);

    sf::Text targetShipName("U.S.S. Test Name", monkirta, 15);

    GameSprite targetShipTracker = GameSprite(*player);

    GameSprite targetShipArrow(nuetralTargetTrackArrow, 45);
    targetShipArrow.setOrigin(0, targetShipArrow.getGlobalBounds().height / 2);

    //generate COM ships TODO: Move this to more sensible place
    for (auto s : ships) delete s;
    ships.clear();
    roll = std::uniform_int_distribution<int>(ceil(currentSystem->getPop() / 3), ceil(currentSystem->getPop() * 1.5));
    int numShips = 2;//roll(gen);

    for (int i = 0; i < numShips; i++) {
        roll = std::uniform_int_distribution<int>(0, shipTextures.size() - 1);
        int n = roll(gen);
        roll = std::uniform_int_distribution<int>(-1500, 1500);
        int randXpos = roll(gen);
        int randYpos = roll(gen);

        ships.push_back(new COMShip(*shipTextures[n], shipTextureScales[n],currentSystem->getPlanets()[1]->getXPos() + randXpos,currentSystem->getPlanets()[1]->getYPos() + randYpos, 0, 0.1, 10, 0, 2, 0, 500, 0,0, wordWrap(generateName(gen), targetWindow.getGlobalBounds().width, monkirta, 15),currentSystem->getSysRep()));
        ships[ships.size() - 1]->addWeapon(new BeamWeapon(Beam(beam, sf::IntRect(0, 207, 535, 91), 10, 12, 1, 0, 513, 3, 1, 1500), beamBuffer, 50, 360 , 25, 40));
    }

    //planet pointer
    GameSprite planetTracker(pointer, 25, window.getSize().x / (float) 2.0, window.getSize().y / (float) 2.0, 0, 0);

    //starmap
    GameSprite mapBox(mapWin, 50, window.getSize().x / (float) 2.0, window.getSize().y / (float) 2.0, 0, 0);
    mapBox.setScale(1.325, 1.02);

    for (System *s : systems) {
        mapSprites.push_back(new GameSprite(ring, 5, s->getRelativeMapPos().x * window.getSize().x,
                                            s->getRelativeMapPos().y * window.getSize().y, 0, 0));
        for (int i : s->getExits()) {
            mapLines.push_back(new sf::Vertex[2]);
            mapLines[mapLines.size() - 1][0].position = sf::Vector2f(mapSprites[mapSprites.size() - 1]->getPosition());
            mapLines[mapLines.size() - 1][0].color = sf::Color(220, 220, 220);
            mapLines[mapLines.size() - 1][1].position = sf::Vector2f(
                    systems[i]->getRelativeMapPos().x * window.getSize().x,
                    systems[i]->getRelativeMapPos().y * window.getSize().y);
            mapLines[mapLines.size() - 1][1].color = sf::Color(220, 220, 220);
        }
    }

    //sys info box
    bool showSysInfo = false;
    GameSprite sysInfoBox(box, 30, 0, 0, 0, 0);

    sf::Text sysInfoTitle("System Name", xolo, 25);
    sysInfoTitle.setFillColor(sf::Color::White);

    sf::Text sysStatus("Status - ", monkirta, 20);
    sysStatus.setFillColor(sf::Color::White);

    sf::Text sysStatusText("System Status", oxan, 15);
    sysStatusText.setFillColor(sf::Color::White);

    sf::Text sysGov("Authority - ", monkirta, 20);
    sysGov.setFillColor(sf::Color::White);

    sf::Text sysGovText("System Gov", oxan, 15);
    sysGovText.setFillColor(sf::Color::White);

    sf::Text sysRep("Prestige - ", monkirta, 20);
    sysGov.setFillColor(sf::Color::White);

    sf::Text sysRepText("System Prestige", oxan, 15);
    sysGovText.setFillColor(sf::Color::White);

    sf::Text sysPop("Population - ", monkirta, 20);
    sysGov.setFillColor(sf::Color::White);

    sf::Text sysPopText("System Population", oxan, 15);
    sysGovText.setFillColor(sf::Color::White);

    sf::Text sysStren("Strength - ", monkirta, 20);
    sysGov.setFillColor(sf::Color::White);

    sf::Text sysStrenText("System Strength", oxan, 15);
    sysGovText.setFillColor(sf::Color::White);

    //planet dialog
    bool showPlanetDialog = false;
    GameSprite planetDialogBox(dialog, 75, mainView.getSize().x * (float) 0.5, mainView.getSize().y * (float) 0.275, 0,
                               0);
    GameSprite planetDialogPic(*images[0], 75, planetDialogBox.getXPos(), planetDialogBox.getYPos(), 0, 0);
    GameSprite planetDialogDesc(win, 35, mainView.getSize().x * (float) 0.5, mainView.getSize().y * (float) 0.75, 0, 0);

    sf::Text planetInfoTitle("Planet Name", xolo, 28);
    planetInfoTitle.setFillColor(sf::Color::White);

    sf::Text planetInfoText("Desc", oxan, 15);
    planetInfoText.setFillColor(sf::Color::White);

    GameSprite departButton(button, 45);

    sf::Text departText("Depart", sk, 22);
    departText.setFillColor(sf::Color::White);
    departText.setLetterSpacing(departText.getLetterSpacing() + (float) 0.5);
    departText.setOrigin(departText.getLocalBounds().width / 2, departText.getLocalBounds().height / 2);

    GameSprite missionsButton(button, 45);

    sf::Text missionsText("Tasks", sk, 22);
    missionsText.setFillColor(sf::Color::White);
    missionsText.setLetterSpacing(missionsText.getLetterSpacing() + (float) 0.5);
    missionsText.setOrigin(missionsText.getLocalBounds().width / 2, missionsText.getLocalBounds().height / 2);

    GameSprite tradeButton(button, 45);

    sf::Text tradeText("Market", sk, 22);
    tradeText.setFillColor(sf::Color::White);
    tradeText.setLetterSpacing(tradeText.getLetterSpacing() + (float) 0.5);
    tradeText.setOrigin(tradeText.getLocalBounds().width / 2, tradeText.getLocalBounds().height / 2);

    GameSprite exploreButton(button, 45);

    sf::Text exploreText("Explore", sk, 22);
    exploreText.setFillColor(sf::Color::White);
    exploreText.setLetterSpacing(exploreText.getLetterSpacing() + (float) 0.5);
    exploreText.setOrigin(exploreText.getLocalBounds().width / 2, exploreText.getLocalBounds().height / 2);

    GameSprite yardButton(button, 45);

    sf::Text yardText("Shipyard", sk, 22);
    yardText.setFillColor(sf::Color::White);
    yardText.setLetterSpacing(yardText.getLetterSpacing() + (float) 0.5);
    yardText.setOrigin(yardText.getLocalBounds().width / 2, yardText.getLocalBounds().height / 2);

    GameSprite lodgeButton(button, 45);

    sf::Text lodgeText("Lodge", sk, 22);
    lodgeText.setFillColor(sf::Color::White);
    lodgeText.setLetterSpacing(lodgeText.getLetterSpacing() + (float) 0.5);
    lodgeText.setOrigin(lodgeText.getLocalBounds().width / 2, lodgeText.getLocalBounds().height / 2);

    GameSprite treasuryButton(button, 45);

    sf::Text treasuryText("Bank", sk, 22);
    treasuryText.setFillColor(sf::Color::White);
    treasuryText.setLetterSpacing(treasuryText.getLetterSpacing() + (float) 0.5);
    treasuryText.setOrigin(treasuryText.getLocalBounds().width / 2, treasuryText.getLocalBounds().height / 2);

    GameSprite capitalButton(button, 45);

    sf::Text capitalText("Capital", sk, 22);
    capitalText.setFillColor(sf::Color::White);
    capitalText.setLetterSpacing(capitalText.getLetterSpacing() + (float) 0.5);
    capitalText.setOrigin(capitalText.getLocalBounds().width / 2, capitalText.getLocalBounds().height / 2);

    //TODO: Academy
    /*GameSprite uniButton(button, 45);
    uniButton.setPosition(capitalButton.getXPos(), capitalButton.getYPos() - capitalButton.getGlobalBounds().height - 13);

    sf::Text uniText("Academy", sk, 22);
    uniText.setPosition(uniButton.getPosition().x, uniButton.getPosition().y - 8);
    uniText.setFillColor(sf::Color::White);
    uniText.setLetterSpacing(uniText.getLetterSpacing() + (float)0.5);
    uniText.setOrigin(uniText.getLocalBounds().width/2, uniText.getLocalBounds().height/2);*/

    float xDiff = mainView.getCenter().x - mainView.getSize().x / (float) 2;
    float yDiff = mainView.getCenter().y - mainView.getSize().y / (float) 2;

    planetDialogBox.setPosition(mainView.getSize().x * (float) 0.5 + xDiff,
                                mainView.getSize().y * (float) 0.275 + yDiff);
    planetDialogDesc.setPosition(mainView.getSize().x * (float) 0.5 + xDiff,
                                 mainView.getSize().y * (float) 0.75 + yDiff);
    planetDialogPic.setPosition(planetDialogBox.getXPos() - (float) 0.25, planetDialogBox.getYPos() - (float) 1.75);
    planetInfoTitle.setPosition(mainView.getSize().x / (float) 2 + xDiff, mainView.getSize().y * (float) 0.565 + yDiff);
    planetInfoText.setPosition(mainView.getSize().x / (float) 2.65 + xDiff,
                               mainView.getSize().y * (float) 0.628 + yDiff);
    departButton.setPosition(mainView.getSize().x * (float) 0.7125 + xDiff,
                             mainView.getSize().y * (float) 0.935 + yDiff);
    departText.setPosition(departButton.getPosition().x, departButton.getPosition().y - 7);
    missionsButton.setPosition(departButton.getXPos(),
                               departButton.getYPos() - departButton.getGlobalBounds().height - 13);
    missionsText.setPosition(missionsButton.getPosition().x, missionsButton.getPosition().y - 8);
    tradeButton.setPosition(missionsButton.getXPos(),
                            missionsButton.getYPos() - missionsButton.getGlobalBounds().height - 13);
    tradeText.setPosition(tradeButton.getPosition().x, tradeButton.getPosition().y - 8);
    exploreButton.setPosition(tradeButton.getXPos(), tradeButton.getYPos() - tradeButton.getGlobalBounds().height - 13);
    exploreText.setPosition(exploreButton.getPosition().x, exploreButton.getPosition().y - 8);
    yardButton.setPosition(mainView.getSize().x * (float) 0.2875 + xDiff, mainView.getSize().y * (float) 0.935 + yDiff);
    yardText.setPosition(yardButton.getPosition().x, yardButton.getPosition().y - 7);
    lodgeButton.setPosition(yardButton.getXPos(), yardButton.getYPos() - yardButton.getGlobalBounds().height - 13);
    lodgeText.setPosition(lodgeButton.getPosition().x, lodgeButton.getPosition().y - 8);
    treasuryButton.setPosition(lodgeButton.getXPos(),
                               lodgeButton.getYPos() - lodgeButton.getGlobalBounds().height - 13);
    treasuryText.setPosition(treasuryButton.getPosition().x, treasuryButton.getPosition().y - 8);
    capitalButton.setPosition(treasuryButton.getXPos(),
                              treasuryButton.getYPos() - treasuryButton.getGlobalBounds().height - 13);
    capitalText.setPosition(capitalButton.getPosition().x, capitalButton.getPosition().y - 8);

    //Fuel Gauge
    GameSprite fuelGauge(loadingBarEmpty, 25);
    fuelGauge.turn(90);

    GameSprite fuelLevel(gaugeBar, 23);
    fuelLevel.turn(270);
    fuelLevel.setScale(27.9 / 100.0, 23 / 100.0);
    fuelLevel.setColor(sf::Color::Yellow);
    sf::FloatRect origGaugeRect = fuelLevel.getLocalBounds();

    GameSprite hullGauge(loadingBarEmpty, 25);
    hullGauge.turn(90);

    GameSprite hullLevel(gaugeBar, 23);
    hullLevel.turn(270);
    hullLevel.setScale(27.9 / 100.0, 23 / 100.0);
    hullLevel.setColor(sf::Color::Red);

    GameSprite shieldGauge(loadingBarEmpty, 25);
    shieldGauge.turn(90);

    GameSprite shieldLevel(gaugeBar, 23);
    shieldLevel.turn(270);
    shieldLevel.setScale(27.9 / 100.0, 23 / 100.0);
    shieldLevel.setColor(sf::Color::Cyan);

    //Sound settings
    GameSprite soundButton(soundBTN, 25, 35, 37, 0, 0);
    GameSprite musicButton(musicBTN, 25, soundButton.getXPos() + soundButton.getGlobalBounds().width,
                           soundButton.getYPos(), 0, 0);

    GameSprite shieldIcon(shield, 50);
    GameSprite hullIcon(hull, 50);
    GameSprite fuelIcon(fuel, 50);

    GameSprite moneyGauge(moneyBar, 50);
    sf::Text moneyText("$" + std::to_string(playerMoney), oxan, 18);
    moneyText.setOrigin(0, moneyText.getGlobalBounds().height / 2);

    GameSprite statsMenuBar(statsBar, 30);
    sf::Text repText("Action Menu : Mouseover to Open", monkirta, 16);
    repText.setOrigin(repText.getGlobalBounds().width / 2, repText.getGlobalBounds().height / 2);

    //Shipyard planet menu
    sf::RenderTexture shipyardBoxTex;
    shipyardBoxTex.create(planetDialogDesc.getGlobalBounds().width,
                          (unsigned int) planetDialogDesc.getGlobalBounds().height - 53);

    GameSprite shipyardBox(shipyardBoxTex.getTexture(), 100);
    shipyardBox.setPosition(planetDialogDesc.getPosition().x, planetDialogDesc.getPosition().y + (float) 26.5);

    GameSprite refuelFullButton(buttonSm, 100);
    refuelFullButton.setPosition(shipyardBox.getGlobalBounds().width / 4, 25);

    sf::Text refuelFullText("Full Refuel", monkirta, 20);
    refuelFullText.setOrigin(refuelFullText.getGlobalBounds().width / 2, refuelFullText.getGlobalBounds().height / 2);
    refuelFullText.setPosition(refuelFullButton.getPosition().x, refuelFullButton.getPosition().y - 7);

    GameSprite repairFullButton(buttonSm, 100);
    repairFullButton.setPosition(shipyardBox.getGlobalBounds().width * 3 / 4, 25);

    sf::Text repairFullText("Full Repair", monkirta, 20);
    repairFullText.setOrigin(repairFullText.getGlobalBounds().width / 2, repairFullText.getGlobalBounds().height / 2);
    repairFullText.setPosition(repairFullButton.getPosition().x, repairFullButton.getPosition().y - 5);

    sf::Text repairFullCost("Cost", oxan, 18);
    sf::Text refuelFullCost("Cost", oxan, 18);

    refuelFullCost.setOutlineThickness(0.5);
    repairFullCost.setOutlineThickness(0.5);
    refuelFullCost.setOutlineColor(sf::Color::White);
    repairFullCost.setOutlineColor(sf::Color::White);

    shipyardMenu.push_back(&refuelFullButton);
    shipyardMenu.push_back(&repairFullButton);

    shipyardMenuText.push_back(&refuelFullText);
    shipyardMenuText.push_back(&repairFullText);
    shipyardMenuText.push_back(&refuelFullCost);
    shipyardMenuText.push_back(&repairFullCost);

    sf::RectangleShape refuelRect(
            sf::Vector2f(refuelFullButton.getGlobalBounds().width, refuelFullButton.getGlobalBounds().height));
    refuelRect.setPosition(shipyardBox.getGlobalBounds().left + refuelFullButton.getGlobalBounds().left,
                           shipyardBox.getGlobalBounds().top + refuelFullButton.getGlobalBounds().top);
    refuelRect.setFillColor(sf::Color(0, 0, 0, 0));

    sf::RectangleShape repairRect(
            sf::Vector2f(repairFullButton.getGlobalBounds().width, repairFullButton.getGlobalBounds().height));
    repairRect.setPosition(shipyardBox.getGlobalBounds().left + repairFullButton.getGlobalBounds().left,
                           shipyardBox.getGlobalBounds().top + repairFullButton.getGlobalBounds().top);
    repairRect.setFillColor(sf::Color(0, 0, 0, 0));

    // while our window is open, keep it open
    // this is our draw loop
    while (window.isOpen()) {
        /*********************************************
         * Handle keypresses here.
         *********************************************/

        if (jumping == -9999 && landing == -9999 && !showPlanetDialog && window.hasFocus()) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                player->accelerate(0.1);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                player->accelerate(-0.1);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                player->turn(-player->getTurnRate());
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                player->turn(player->getTurnRate());
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                player->shoot(projectiles);
            }
        }

        /*********************************************
         * Pre-draw ops here.
         *********************************************/

        if (!(map || showPlanetDialog)) {
            for (GameSprite *s : objects) {
                s->update();
            }

            player->update();

            for (COMShip *s : ships) {
                COMShip::Status res = s->pathfind(window, gen, currentSystem, player, projectiles);
                s->update();

                if (res == COMShip::WARPING) {
                    sound.setBuffer(warp);
                    sound.setVolume((GameSprite::distance(player->getPosition(), s->getPosition()) > 2000) ? 0 : 100 *(2000 -GameSprite::distance(player->getPosition(),s->getPosition())) /2000);
                    sound.play();

                    if (player->getTarget() == s) {
                        player->setTarget(nullptr);
                    }

                    delete s;
                    ships.erase(std::find(ships.begin(), ships.end(), s));

                    messageLog.push_front(new sf::Text("A ship left the system.", oxan, 18));
                }
            }

            for (Shootable *p : projectiles) {
                bool deleted = false;
                p->update();

                for (Ship *s : ships) {
                    if (p->getShooter()->getTarget() == s && ((p->getLifetime() == 1 && p->getGlobalBounds().intersects(s->getGlobalBounds())) || Collision::PixelPerfectTest(*p, *s))) {
                        s->setHull(s->getHullRemaining() - p->getDamage());

                        for (COMShip *s : ships) {
                            s->setPlayerRep(s->isFriendly() || s->isNeutral() ? -1 : s->getPlayerRep());
                        }

                        if (p->getLifetime() != 1) {
                            delete p;
                            projectiles.erase(std::find(projectiles.begin(), projectiles.end(), p));
                            deleted = true;
                        }

                        if (s->getHullRemaining() <= 0) {
                            delete s;
                            ships.erase(std::find(ships.begin(), ships.end(), s));

                            if (player->getTarget() == p->getShooter()->getTarget()) player->setTarget(nullptr);
                            for (COMShip *s : ships) {
                                if (s->getTarget() == p->getShooter()->getTarget()) s->setTarget(nullptr);
                            }

                            currentSystem->setSysRep(currentSystem->getSysRep() - 10);
                        }

                        break;
                    }
                }

                if (deleted) break;

                if (p->getShooter()->getTarget() == player && ((p->getLifetime() == 1 && p->getGlobalBounds().intersects(player->getGlobalBounds())) || Collision::PixelPerfectTest(*p, *player))) {
                    player->setHull(player->getHullRemaining() - p->getDamage());
                    hullLevel.setTextureRect(sf::IntRect(origGaugeRect.left, origGaugeRect.top,
                                                         origGaugeRect.width / (float) player->getHullCap() *
                                                         (float) player->getHullRemaining(), origGaugeRect.height));

                    if (p->getLifetime() != 1) {
                        delete p;
                        projectiles.erase(std::find(projectiles.begin(), projectiles.end(), p));
                    }

                    if (player->getHullRemaining() <= 0) {
                        delete player;

                        return;
                    }

                    break;
                }
            }

            roll = std::uniform_int_distribution<int>(1, 10000);
            if (roll(gen) < ceil(currentSystem->getPop() / 4) && ships.size() < currentSystem->getPop() * 1.5) {
                roll = std::uniform_int_distribution<int>(0, shipTextures.size() - 1);
                int n = roll(gen);
                roll = std::uniform_int_distribution<int>(-1500, 1500);
                int randXpos = roll(gen);
                int randYpos = roll(gen);

                ships.push_back(new COMShip(*shipTextures[n], shipTextureScales[n],currentSystem->getPlanets()[1]->getXPos() + randXpos,currentSystem->getPlanets()[1]->getYPos() + randYpos, 0, 0.1, 10, 0, 2, 0,500, 0, 0,wordWrap(generateName(gen), targetWindow.getGlobalBounds().width, monkirta,15),currentSystem->getSysRep()));
                ships[ships.size() - 1]->addWeapon(new ProjectileWeapon(Projectile(laser, sf::IntRect(29, 207, 651, 91), 10, 4, 3, 676, 513, 3, 15, 25, 1500), laserBuffer, 50, 10, 40));

                sound.setBuffer(warp);
                sound.setVolume((GameSprite::distance(player->getPosition(), ships[ships.size() - 1]->getPosition()) > 2000) ? 0 : 100 * (2000 - GameSprite::distance(player->getPosition(), ships[ships.size() - 1]->getPosition())) / 2000);
                sound.play();

                messageLog.push_front(new sf::Text("A ship entered the system.", oxan, 18));
            }
        }

        mainView.setCenter(player->getXPos(), player->getYPos());
        window.setView(mainView);

        background.setTextureRect(sf::IntRect(player->getXPos(), player->getYPos(), background.getTextureRect().width,
                                              background.getTextureRect().height));
        background.setPosition(mainView.getCenter().x, mainView.getCenter().y);

        jumpFilter.setTextureRect(sf::IntRect(player->getXPos(), player->getYPos(), background.getTextureRect().width,
                                              background.getTextureRect().height));
        jumpFilter.setPosition(mainView.getCenter().x, mainView.getCenter().y);

        if (jumping > 0) {
            jumping -= 5;
            jumpFilter.setFillColor(sf::Color(255, 255, 255, 255 - jumping));

            if (jumping == 100) {
                sound.setBuffer(warp);
                sound.setVolume(100);
                sound.play();
            }

            if (jumping <= 0) {
                currentSystem = destSystem;
                currentSystem->makeVisited();

                jumping = -255;

                player->setTarget(nullptr);

                for (auto p : projectiles) delete p;
                projectiles.clear();

                player->useFuel();
                fuelLevel.setTextureRect(sf::IntRect(origGaugeRect.left, origGaugeRect.top,
                                                     origGaugeRect.width / (float) player->getFuelCap() *
                                                     (float) player->getFuelRemaining(), origGaugeRect.height));

                messageLog.push_front(new sf::Text("Entered the " + currentSystem->getName() + " system.", oxan, 18));

                for (auto t: activeMissions) {
                    if (t->getSystem() == currentSystem) {
                        messageLog.push_front(new sf::Text("You have a task in this system.", oxan, 18));
                        break;
                    }
                }

                player->setPosition(currentSystem->getSystemCenter()->getPosition().x -
                                    (float) (3000 * cos(player->getDirection() * GameSprite::PI / 180)),
                                    currentSystem->getSystemCenter()->getPosition().y +
                                    (float) (3000 * sin(player->getDirection() * GameSprite::PI / 180)));
                player->setVelocity(25);

                //create tasks
                if (currentSystem->isLandable()) {
                    for (auto i : currentSystem->getTasks()) delete i;
                    currentSystem->getTasks().clear();
                    for (auto i : missions) delete i;
                    missions.clear();
                    for (auto i : missionAccepts) delete i;
                    missionAccepts.clear();
                    for (auto i : missionButtonFilters) delete i;
                    missionButtonFilters.clear();
                    for (auto i : missionButtons) delete i;
                    missionButtons.clear();
                    for (auto i : missionSizes) delete i;
                    missionSizes.clear();

                    roll = std::uniform_int_distribution<int>(1,
                                                              (currentSystem->getPop() <= 6) ? currentSystem->getPop()
                                                                                             : 6);

                    numTasks = roll(gen);
                    for (int i = 0; i < numTasks; i++) {
                        int sysNum = -1, planetNum = -1;
                        while (sysNum == -1 || !systems[sysNum]->isLandable() || systems[sysNum] == currentSystem) {
                            roll = std::uniform_int_distribution<int>(0, systems.size() - 1);
                            sysNum = roll(gen);
                        }

                        while (planetNum == -1 || !systems[sysNum]->getPlanets()[planetNum]->isLandable()) {
                            roll = std::uniform_int_distribution<int>(0, systems[sysNum]->getPlanets().size() - 1);
                            planetNum = roll(gen);
                        }

                        std::string taskName;
                        roll = std::uniform_int_distribution<int>(0, 1);
                        int type = roll(gen);
                        switch (type) {
                            case 0: //delivery
                                taskName = "Delivery: " + systems[sysNum]->getPlanets()[planetNum]->getName() + " - " +
                                           systems[sysNum]->getName();
                                break;
                            case 1: //taxi
                                taskName =
                                        "Passengers: " + systems[sysNum]->getPlanets()[planetNum]->getName() + " - " +
                                        systems[sysNum]->getName();
                                break;
                        }

                        int size = 0;
                        int reward = 0;
                        switch (type) {
                            case 0: //delivery
                                roll = std::uniform_int_distribution<int>(25, player->getCargoSpace());
                                size = roll(gen);

                                roll = std::uniform_int_distribution<int>(950, 1050);
                                reward = roll(gen) * size;

                                break;
                            case 1:
                                roll = std::uniform_int_distribution<int>(1, player->getPassengerSpace());
                                size = roll(gen);

                                roll = std::uniform_int_distribution<int>(20000, 30000);
                                reward = roll(gen) * size;

                                break;
                        }


                        currentSystem->addTask(
                                new Task(type, taskName, taskName, systems[sysNum],
                                         systems[sysNum]->getPlanets()[planetNum], reward,
                                         size));

                        missions.push_back(new sf::Text(taskName, oxan, 15));
                        missions.push_back(new sf::Text("$" + std::to_string(
                                currentSystem->getTasks()[currentSystem->getTasks().size() - 1]->getReward()), oxan,
                                                        12));
                        missionSizes.push_back(new sf::Text(std::to_string(
                                currentSystem->getTasks()[currentSystem->getTasks().size() - 1]->getSize()) + " units",
                                                            oxan, 12));

                        missionButtons.push_back(new GameSprite(buttonSm, 65));
                        missionButtons[missionButtons.size() - 1]->setColor(sf::Color::Green);

                        missionAccepts.push_back(
                                new sf::Text("Accept" + std::to_string(missionAccepts.size()), monkirta, 18));
                        missionAccepts[missionAccepts.size() - 1]->setString("Accept");
                        missionAccepts[missionAccepts.size() - 1]->setOrigin(
                                missionAccepts[missionAccepts.size() - 1]->getGlobalBounds().width / 2,
                                missionAccepts[missionAccepts.size() - 1]->getGlobalBounds().height / 2);

                        missionButtonFilters.push_back(new sf::RectangleShape(
                                sf::Vector2f(missionButtons[0]->getGlobalBounds().width,
                                             missionButtons[0]->getGlobalBounds().height)));
                        missionButtonFilters[missionButtonFilters.size() - 1]->setFillColor(sf::Color(0, 0, 0, 0));
                    }
                }

                for (auto i : ships) delete i;
                ships.clear();
                roll = std::uniform_int_distribution<int>(ceil(currentSystem->getPop() / 3),
                                                          ceil(currentSystem->getPop() * 1.5));
                int numShips = roll(gen);

                for (int i = 0; i < numShips; i++) {
                    roll = std::uniform_int_distribution<int>(0, shipTextures.size() - 1);
                    int n = roll(gen);
                    roll = std::uniform_int_distribution<int>(-1500, 1500);
                    int randXpos = roll(gen);
                    int randYpos = roll(gen);

                    ships.push_back(new COMShip(*shipTextures[n], shipTextureScales[n],currentSystem->getPlanets()[1]->getXPos() + randXpos,currentSystem->getPlanets()[1]->getYPos() + randYpos, 0, 0.1, 10, 0, 2,0, 500, 0, 0,wordWrap(generateName(gen), targetWindow.getGlobalBounds().width,monkirta, 15),currentSystem->getSysRep()));
                    ships[ships.size() - 1]->addWeapon(new ProjectileWeapon(Projectile(laser, sf::IntRect(29, 207, 651, 91), 10, 4, 3, 676, 513, 3, 15, 25, 1500), laserBuffer, 10, 50, 40));
                }

                mainView.setCenter(player->getXPos(), player->getYPos());
                window.setView(mainView);

                background.setTextureRect(
                        sf::IntRect(player->getXPos(), player->getYPos(), background.getTextureRect().width,
                                    background.getTextureRect().height));
                background.setPosition(mainView.getCenter().x, mainView.getCenter().y);

                jumpFilter.setTextureRect(
                        sf::IntRect(player->getXPos(), player->getYPos(), background.getTextureRect().width,
                                    background.getTextureRect().height));
                jumpFilter.setPosition(mainView.getCenter().x, mainView.getCenter().y);
            }
        } else if (jumping != -9999 && jumping < 0) {
            if (currentSystem->getSysRep() > NUETRAL_LOW && currentSystem->getSysRep() < NUETRAL_HIGH &&
                !neutralLoop.Playing && musicOn) {
                neutralLoop.play();
            }

            jumping += 5;
            jumpFilter.setFillColor(sf::Color(255, 255, 255, -jumping));

            if (jumping >= 0) {
                jumping = -9999;
            }
        } else if (landing > 0) {
            landing -= 2;

            player->setScale(player->getFullScale() / (float) 100 * (float) landing / 100,
                             player->getFullScale() / (float) 100 * (float) landing / 100);

            player->setPosition(window.getSize().x / (float) 2 + ((landingdx / 100) * (float) landing),
                                window.getSize().y / (float) 2 + ((landingdy / 100) * (float) landing));
            player->turn(2);
            if (landing <= 0) {
                landing = -9999;
                showPlanetDialog = true;
                planetScreen = DESC;

                player->setTarget(nullptr);

                messageLog.push_front(new sf::Text("Landed on " + landingPlanet->getName() + ".", oxan, 18));

                for (int i = 0; i < missions.size(); i += 2) {
                    float offsetX = mainView.getCenter().x - mainView.getSize().x / (float) 2;
                    float offsetY = mainView.getCenter().y - mainView.getSize().y / (float) 2;

                    missions[i]->setPosition(mainView.getSize().x / (float) 2.65 + offsetX,
                                             (mainView.getSize().y * (float) 0.628 + offsetY) + (i * 15));
                    missions[i + 1]->setPosition(mainView.getSize().x / (float) 2.5 + offsetX,
                                                 (mainView.getSize().y * (float) 0.628 + offsetY) + ((i / 2) * 30) +
                                                 15);
                }

                for (auto t: activeMissions) {
                    if (t->getPlanet() == landingPlanet) {
                        playerMoney += t->getReward();
                        moneyText.setString("$" + std::to_string(playerMoney));

                        if (t->getType() == Task::DELIVERY)
                            player->setUsedCargoSpace(player->getUsedCargoSpace() - t->getSize());
                        else if (t->getType() == Task::TAXI)
                            player->setPassengersAboard(player->getPassengersAboard() - t->getSize());

                        delete t;
                        activeMissions.erase(std::find(activeMissions.begin(), activeMissions.end(), t));
                    }
                }
            }
        } else if (landing != -9999 && landing < 0) {
            landing += 2;

            player->setScale(player->getFullScale() / (float) 100 * (float) (100 + landing) / 100,
                             player->getFullScale() / (float) 100 * (float) (100 + landing) / 100);
            player->turn(-2);
            if (landing >= 0) {
                landing = -9999;
                landingPlanet = nullptr;
            }
        }

        planetTracker.setRotation(GameSprite::getAimAngle(*(currentSystem->getSystemCenter()), *player));

        sf::IntRect viewRect(mainView.getCenter().x - mainView.getSize().x / 2,
                             mainView.getCenter().y - mainView.getSize().y / 2, mainView.getSize().x,
                             mainView.getSize().y);
        planetTracker.setPosition(viewRect.left + (float) 50, viewRect.top + (float) 50);

        xDiff = mainView.getCenter().x - mainView.getSize().x / (float) 2;
        yDiff = mainView.getCenter().y - mainView.getSize().y / (float) 2;

        fuelGauge.setPosition((mainView.getSize().x * (float) 0.98) + xDiff, mainView.getSize().y / 2 + yDiff);
        fuelLevel.setPosition((mainView.getSize().x * (float) 0.98) + xDiff, mainView.getSize().y / 2 + yDiff);
        hullGauge.setPosition(fuelGauge.getXPos() - 32, fuelGauge.getYPos());
        hullLevel.setPosition(fuelGauge.getXPos() - 32, fuelGauge.getYPos());
        shieldGauge.setPosition(hullGauge.getXPos() - 32, hullGauge.getYPos());
        shieldLevel.setPosition(hullGauge.getXPos() - 32, hullGauge.getYPos());

        fuelIcon.setPosition(fuelGauge.getXPos(), fuelGauge.getYPos() - fuelGauge.getGlobalBounds().height / 2 - 25);
        hullIcon.setPosition(hullGauge.getXPos(), hullGauge.getYPos() - hullGauge.getGlobalBounds().height / 2 - 25);
        shieldIcon.setPosition(shieldGauge.getXPos(),
                               shieldGauge.getYPos() - shieldGauge.getGlobalBounds().height / 2 - 25);

        moneyGauge.setPosition(hullGauge.getXPos() - 33, moneyGauge.getGlobalBounds().height + yDiff);
        moneyText.setPosition(moneyGauge.getPosition().x - 65, moneyGauge.getPosition().y - 4);

        statsMenuBar.setPosition(mainView.getCenter().x, mainView.getCenter().y + mainView.getSize().y / 2 -
                                                         statsMenuBar.getGlobalBounds().height / 2);
        repText.setPosition(statsMenuBar.getXPos(), statsMenuBar.getYPos() - 4);

        if (player->getTarget() != nullptr) {
            if (((COMShip *) player->getTarget())->isFriendly()) targetCircle.setTexture(friendlyTargetIndicator);
            else if (((COMShip *) player->getTarget())->isNeutral()) targetCircle.setTexture(nuetralTargetIndicator);
            else if (((COMShip *) player->getTarget())->isHostile()) targetCircle.setTexture(hostileTargetIndicator);

            double scaleFactor =
                    fmax(player->getTarget()->getGlobalBounds().width, player->getTarget()->getGlobalBounds().height) /
                    targetCircle.getLocalBounds().width;

            targetCircle.setScale(scaleFactor, scaleFactor);
            targetCircle.setOrigin(targetCircle.getLocalBounds().width / 2, targetCircle.getLocalBounds().height / 2);
            targetCircle.setPosition(player->getTarget()->getPosition());

            targetCircle.rotate(0.5);

            targetWindow.setPosition(mainView.getCenter().x - mainView.getSize().x / 2 + 10,
                                     mainView.getCenter().y + mainView.getSize().y / 2 -
                                     targetWindow.getGlobalBounds().height - 10);
            targetScanTitle.setPosition(targetWindow.getPosition().x + targetWindow.getGlobalBounds().width / 2,
                                        targetWindow.getPosition().y);
            targetShipName.setPosition(targetWindow.getPosition().x + 10, targetWindow.getPosition().y + 30);
            targetShipTracker.setPosition(targetWindow.getPosition().x + targetWindow.getGlobalBounds().width / 2,
                                          targetWindow.getPosition().y +
                                          22 * targetWindow.getGlobalBounds().height / 35);

            if (((COMShip *) player->getTarget())->isFriendly()) targetShipArrow.setTexture(friendlyTargetTrackArrow);
            else if (((COMShip *) player->getTarget())->isNeutral())
                targetShipArrow.setTexture(nuetralTargetTrackArrow);
            else if (((COMShip *) player->getTarget())->isHostile())
                targetShipArrow.setTexture(hostileTargetTrackArrow);

            targetShipArrow.setOrigin(0, targetShipArrow.getGlobalBounds().height / 2);

            targetShipArrow.setPosition(targetShipTracker.getPosition());
            targetShipArrow.setRotation(GameSprite::getAimAngle(*player->getTarget(), *player));
        }

        mapBox.setPosition(mainView.getCenter());

        if (messageLog.size() > 5) messageLog.pop_back();
        for (int i = 0; i < messageLog.size(); i++) {
            messageLog[i]->setPosition(mainView.getCenter().x - mainView.getSize().x / 2 + 100,
                                       (mainView.getCenter().y - mainView.getSize().y / 2 - 20) +
                                       (((float) 17) * (float) (i + 2)));
            messageLog[i]->setFillColor(sf::Color(255, 255, 255, 255 - (51 * i)));
        }

        refuelFullCost.setPosition(refuelFullButton.getXPos(), refuelFullButton.getYPos() + 30);
        repairFullCost.setPosition(repairFullButton.getXPos(), repairFullButton.getYPos() + 30);

        /*********************************************
         * Drawing goes here.
         *********************************************/

        window.clear(sf::Color::Black);           // clear the contents of the old frame

        if (!map) {
            window.draw(background);

            for (sf::Sprite *s : currentSystem->getPlanets()) {
                window.draw(*s);
            }

            for (GameSprite *s : objects) {
                window.draw(*s);
            }

            for (Shootable *p : projectiles) {
                window.draw(*p);
            }

            for (Ship *s : ships) {
                window.draw(*s);
            }

            window.draw(*player);

            if (player->getTarget() != nullptr) {
                window.draw(targetCircle);
                window.draw(targetWindow);
                window.draw(targetScanTitle);
                window.draw(targetShipName);
                window.draw(targetShipTracker);
                window.draw(targetShipArrow);
            }

            sf::IntRect planetRect(currentSystem->getSystemCenter()->getGlobalBounds().left,
                                   currentSystem->getSystemCenter()->getGlobalBounds().top,
                                   currentSystem->getSystemCenter()->getGlobalBounds().width,
                                   currentSystem->getSystemCenter()->getGlobalBounds().height);
            if (!viewRect.intersects(planetRect)) {
                window.draw(planetTracker);
            }

            window.draw(fuelGauge);
            window.draw(fuelLevel);
            window.draw(hullGauge);
            window.draw(hullLevel);
            window.draw(shieldGauge);
            window.draw(shieldLevel);

            window.draw(fuelIcon);
            window.draw(hullIcon);
            window.draw(shieldIcon);

            window.draw(moneyGauge);
            window.draw(moneyText);

            if (!showPlanetDialog) {
                window.draw(statsMenuBar);
                window.draw(repText);
            }

            for (auto t : messageLog) {
                window.draw(*t);
            }

            if (jumping != -9999) window.draw(jumpFilter);

            if (showPlanetDialog) {
                window.draw(planetDialogBox);
                window.draw(planetDialogDesc);
                window.draw(planetDialogPic);
                window.draw(planetInfoTitle);

                switch (planetScreen) {
                    case DESC:
                        window.draw(planetInfoText);
                        break;
                    case YARD:
                        shipyardBoxTex.clear(sf::Color(0, 0, 0, 0));

                        for (GameSprite *s : shipyardMenu) {
                            shipyardBoxTex.draw(*s);
                        }

                        for (sf::Text *t : shipyardMenuText) {
                            shipyardBoxTex.draw(*t);
                        }

                        shipyardBoxTex.display();

                        window.draw(shipyardBox);

                        window.draw(refuelRect);
                        window.draw(repairRect);
                        break;
                    case TASKS:
                        for (auto s : missions) {
                            window.draw(*s);
                        }

                        for (auto s : missionSizes) {
                            window.draw(*s);
                        }

                        for (auto s : missionButtons) {
                            window.draw(*s);
                        }

                        for (auto s : missionAccepts) {
                            window.draw(*s);
                        }

                        for (auto s : missionButtonFilters) {
                            window.draw(*s);
                        }
                        break;
                }
                window.draw(departButton);
                window.draw(departText);
                window.draw(missionsButton);
                window.draw(missionsText);
                window.draw(tradeButton);
                window.draw(tradeText);
                window.draw(exploreButton);
                window.draw(exploreText);
                window.draw(yardButton);
                window.draw(yardText);
                window.draw(lodgeButton);
                window.draw(lodgeText);
                window.draw(treasuryButton);
                window.draw(treasuryText);
                window.draw(capitalButton);
                window.draw(capitalText);
                //window.draw(uniButton);
                //window.draw(uniText);

                //Sound buttons
                window.draw(soundButton);
                window.draw(musicButton);
            }
        } else {
            window.draw(mapBox);

            for (sf::Vertex *v : mapLines) {
                window.draw(v, 2, sf::Lines);
            }

            for (int i = 0; i < systems.size(); i++) {
                if (systems[i]->isVisited()) {
                    mapSprites[i]->setColor(sf::Color::White);
                    mapSprites[i]->setTexture(ball);
                    if (systems[i]->getSysRep() <= HOSTILE_HIGH) mapSprites[i]->setColor(sf::Color::Red);
                    else if (systems[i]->getSysRep() >= FRIENDLY_LOW) mapSprites[i]->setColor(sf::Color::Cyan);
                    else if (systems[i]->getSysRep() >= NUETRAL_LOW &&
                             systems[i]->getSysRep() <= NUETRAL_HIGH)
                        mapSprites[i]->setColor(sf::Color::White);
                }
                if (systems[i] == currentSystem) mapSprites[i]->setColor(sf::Color::Yellow);

                window.draw(*mapSprites[i]);
            }

            if (showSysInfo) {
                window.draw(sysInfoBox);
                window.draw(sysInfoTitle);
                window.draw(sysStatus);
                window.draw(sysStatusText);
                window.draw(sysGov);
                window.draw(sysGovText);
                window.draw(sysRep);
                window.draw(sysRepText);
                window.draw(sysPop);
                window.draw(sysPopText);
                window.draw(sysStren);
                window.draw(sysStrenText);
            }
        }

        window.display();                       // display the window

        //delete old sprites
        std::vector<Shootable*> forDelete;
        for (Shootable *p : projectiles) {
            if ((p->getLifetime() == 1) || p->isPastLifetime()) {
                forDelete.push_back(p);
            }
        }

        for (Shootable *p: forDelete) {
            projectiles.erase(std::find(projectiles.begin(), projectiles.end(), p));
            delete p;
        }
        forDelete.clear();

        sf::Event event{};
        while (window.pollEvent(event)) {      // ask the window if any events occurred

            /*********************************************
            * Event handling here.
            *********************************************/
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2f mousePosF(static_cast<float>( mousePos.x ), static_cast<float>( mousePos.y ));
            sf::Vector2f mousePosWorldF = window.mapPixelToCoords(mousePos);

            if (event.type == sf::Event::Closed) { // if event type is a closed event
                // i.e. they clicked the X on the window
                window.close();                 // then close our window
            } else if (event.type == sf::Event::KeyReleased && landing == -9999 && jumping == -9999 && window.hasFocus()) {
                sf::Vector2f tempPos;
                switch (event.key.code) {
                    case sf::Keyboard::Key::M:
                        if (!showPlanetDialog) {
                            map = !map;
                            tempPos = oldPos;
                            if (map) oldPos = player->getPosition();
                            else oldPos = currentPos;
                            if (map)
                                currentPos = sf::Vector2f(mainView.getSize().x / (float) 2, window.getSize().y / (float) 2);
                            else currentPos = tempPos;
                            player->setPosition(currentPos);
                        }
                        break;
                    case sf::Keyboard::Key::L:
                        if (!showPlanetDialog && !map) {
                            for (auto p : currentSystem->getPlanets()) {
                                if ((GameSprite::distance(player->getPosition(), p->getPosition()) < p->getGlobalBounds().width / 2) && ((Planet *) p)->isLandable() && player->getVelocity() < 2) {
                                    landing = 100;
                                    landingPlanet = p;
                                    landingdx = player->getXPos() - p->getXPos();
                                    landingdy = player->getYPos() - p->getYPos();
                                    planetInfoTitle.setString(landingPlanet->getName());
                                    planetInfoTitle.setOrigin(planetInfoTitle.getGlobalBounds().width / 2, planetInfoTitle.getGlobalBounds().height / 2);
                                    planetInfoText.setString(landingPlanet->getDesc());
                                    planetDialogPic.setTexture(*images[landingPlanet->getImageNum()]);
                                    player->setVelocity(0);
                                }
                            }
                        }
                        break;
                    case sf::Keyboard::Key::Escape:
                        if (map) map = false;

                        if (showPlanetDialog) {
                            showPlanetDialog = false;
                            landing = -100;
                        }
                }
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (map) {
                    for (int i = 0; i < mapSprites.size(); i++) {
                        if (mapSprites[i]->getGlobalBounds().contains(mousePosF)) {
                            if (currentSystem != systems[i]) {
                                if (std::find(currentSystem->getExits().begin(), currentSystem->getExits().end(), i) !=
                                    currentSystem->getExits().end()) {
                                    for (int s : currentSystem->getExits()) {
                                        if (systems[i] == systems[s]) {
                                            if (player->getFuelRemaining() > 0) {
                                                playBip();
                                                jumping = 255;
                                                destSystem = systems[i];
                                                map = false;
                                                player->setPosition(oldPos);
                                                break;
                                            } else {
                                                playErr();
                                                messageLog.push_front(new sf::Text("No fuel to jump.", oxan, 18));
                                                map = false;
                                            }
                                        }
                                    }
                                } else playErr();
                            } else playErr();
                        }
                    }
                } else {
                    for (COMShip *s : ships) {
                        if (s->getGlobalBounds().contains(mousePosWorldF)) {
                            player->setTarget(s);

                            targetShipName.setString(s->getName());

                            targetShipTracker = GameSprite(*s);

                            double scaleFactor;
                            if (fmax(player->getTarget()->getGlobalBounds().width, player->getTarget()->getGlobalBounds().height) == player->getTarget()->getGlobalBounds().width) {
                                scaleFactor = (player->getLocalBounds().width * player->getScale().x * 0.8) / (player->getTarget()->getLocalBounds().width * player->getScale().x);
                            } else {
                                scaleFactor = (player->getLocalBounds().height * player->getScale().y * 0.8) / (player->getTarget()->getLocalBounds().height * player->getScale().y);
                            }

                            targetShipTracker.setScale(scaleFactor, scaleFactor);
                            targetShipTracker.setRotation(0);
                        }
                    }
                }

                if (departButton.getGlobalBounds().contains(mousePosF) && showPlanetDialog) {
                    playBip();
                    if (showPlanetDialog) {
                        showPlanetDialog = false;
                        landing = -100;
                    }
                } else if (soundButton.getGlobalBounds().contains(mousePosF) && showPlanetDialog) {
                    soundOn = !soundOn;
                    playBip();
                } else if (musicButton.getGlobalBounds().contains(mousePosF) && showPlanetDialog) {
                    playBip();
                    musicOn = !musicOn;
                    if (!musicOn) neutralLoop.setVolume(0);
                    else neutralLoop.setVolume((float) neutralVol);
                } else if (yardButton.getGlobalBounds().contains(mousePosF) && showPlanetDialog) {
                    if (planetScreen != YARD) playBip();
                    else playErr();

                    planetScreen = YARD;

                    int fuelCost = (player->getFuelCap() - player->getFuelRemaining()) * 1250;
                    int hullCost = (player->getHullCap() - player->getHullRemaining()) * 15;

                    refuelFullCost.setString("Cost: $" + std::to_string(fuelCost));
                    refuelFullCost.setOrigin(refuelFullCost.getGlobalBounds().width / 2, refuelFullCost.getGlobalBounds().height / 2);

                    repairFullCost.setString("Cost: $" + std::to_string(hullCost));
                    repairFullCost.setOrigin(repairFullCost.getGlobalBounds().width / 2, repairFullCost.getGlobalBounds().height / 2);

                    if (playerMoney > fuelCost && player->getFuelRemaining() < player->getFuelCap()) refuelFullButton.setColor(sf::Color::Green);
                    else refuelFullButton.setColor(sf::Color::White);

                    if (playerMoney > hullCost && player->getHullRemaining() < player->getHullCap()) repairFullButton.setColor(sf::Color::Green);
                    else repairFullButton.setColor(sf::Color::White);
                } else if (refuelRect.getGlobalBounds().contains(mousePosF) && showPlanetDialog && planetScreen == YARD) {
                    int fuelCost = (player->getFuelCap() - player->getFuelRemaining()) * 1250;

                    if (playerMoney >= fuelCost && player->getFuelRemaining() < player->getFuelCap()) {
                        playBip();
                        player->setFuel(player->getFuelCap());
                        fuelLevel.setTextureRect(sf::IntRect(origGaugeRect.left, origGaugeRect.top, origGaugeRect.width / (float) player->getFuelCap() * (float) player->getFuelRemaining(), origGaugeRect.height));

                        playerMoney -= fuelCost;
                        moneyText.setString("$" + std::to_string(playerMoney));

                        if (playerMoney > fuelCost && player->getFuelRemaining() < player->getFuelCap()) refuelFullButton.setColor(sf::Color::Green);
                        else refuelFullButton.setColor(sf::Color::White);

                        refuelFullCost.setString("Cost: $0");
                        refuelFullCost.setOrigin(refuelFullCost.getGlobalBounds().width / 2, refuelFullCost.getGlobalBounds().height / 2);
                    } else playErr();
                } else if (repairRect.getGlobalBounds().contains(mousePosF) && showPlanetDialog && planetScreen == YARD) {
                    int hullCost = (player->getHullCap() - player->getHullRemaining()) * 15;

                    if (playerMoney >= hullCost && player->getHullRemaining() < player->getHullCap()) {
                        playBip();
                        player->setHull(player->getHullCap());
                        playerMoney -= hullCost;

                        if (playerMoney > hullCost &&
                            player->getHullRemaining() < player->getHullCap())
                            repairFullButton.setColor(sf::Color::Green);
                        else repairFullButton.setColor(sf::Color::White);
                    } else playErr();
                } else if (exploreButton.getGlobalBounds().contains(mousePosF) && showPlanetDialog) {
                    playBip();

                    if (!(currentSystem->getStren() == 0 && currentSystem->getPop() == 0)) {
                        roll = std::uniform_int_distribution<int>(ceil(currentSystem->getPop() / 3), ceil(currentSystem->getPop() * 1.5));
                        int message = roll(gen);

                        planetInfoText.setString(wordWrap(explores[message]->getMessage(), 305, oxan, 15));
                    }
                } else if (missionsButton.getGlobalBounds().contains(mousePosF) && showPlanetDialog) {
                    if (planetScreen != TASKS) playBip();
                    else playErr();

                    planetScreen = TASKS;

                    for (int i = 0; i < missionAccepts.size(); i++) {
                        missionButtons[i]->setPosition(missions[2 * i]->getPosition().x + mainView.getSize().x / (float) 4.65,
                                                       missions[2 * i]->getPosition().y + missionButtons[i]->getGlobalBounds().height / 1.75);

                        missionSizes[i]->setPosition(missions[2 * i + 1]->getPosition().x + 75, missions[2 * i + 1]->getPosition().y);

                        missionAccepts[i]->setPosition(missionButtons[i]->getPosition().x,
                                                       missionButtons[i]->getPosition().y -
                                                       missionAccepts[0]->getGlobalBounds().height / 4);
                        missionButtonFilters[i]->setPosition(missionButtons[0]->getGlobalBounds().left,
                                                             missionButtons[i]->getGlobalBounds().top);

                        int delivTotal = player->getUsedCargoSpace() + currentSystem->getTasks()[i]->getSize();
                        int passTotal = player->getPassengersAboard() + currentSystem->getTasks()[i]->getSize();
                        if ((currentSystem->getTasks()[i]->getType() == Task::DELIVERY && delivTotal > player->getCargoSpace()) || (currentSystem->getTasks()[i]->getType() == Task::TAXI && passTotal > player->getPassengerSpace())) {
                            if (missionAccepts[i]->getString() == "Accept") {
                                missionButtons[i]->setColor(sf::Color::White);
                                missionAccepts[i]->setString("Too Full");
                                missionAccepts[i]->setOrigin(missionAccepts[i]->getGlobalBounds().width / 2,
                                                             missionAccepts[i]->getGlobalBounds().height / 2 + 2);
                            }
                        }
                    }
                }

                if (planetScreen == TASKS) {
                    for (int i = 0; i < missionButtons.size(); i++) {
                        if (missionButtonFilters[i]->getGlobalBounds().contains(mousePosF)) {
                            if (missionAccepts[i]->getString() == "Accept") {
                                playBip();
                                if (currentSystem->getTasks()[i]->getType() == Task::DELIVERY) player->setUsedCargoSpace(player->getUsedCargoSpace() + currentSystem->getTasks()[i]->getSize());
                                else if (currentSystem->getTasks()[i]->getType() == Task::TAXI) player->setPassengersAboard(player->getPassengersAboard() + currentSystem->getTasks()[i]->getSize());

                                for (int j = 0; j < missionAccepts.size(); j++) {
                                    if (missionAccepts[j]->getString() == "Accept") {
                                        int delivTotal = player->getUsedCargoSpace() + currentSystem->getTasks()[j]->getSize();
                                        int passTotal = player->getPassengersAboard() + currentSystem->getTasks()[j]->getSize();
                                        if ((currentSystem->getTasks()[j]->getType() == Task::DELIVERY && delivTotal > player->getCargoSpace()) || (currentSystem->getTasks()[j]->getType() == Task::TAXI && passTotal > player->getPassengerSpace())) {
                                            missionButtons[j]->setColor(sf::Color::White);
                                            missionAccepts[j]->setString("Too Full");
                                            missionAccepts[j]->setOrigin(missionAccepts[j]->getGlobalBounds().width / 2, missionAccepts[j]->getGlobalBounds().height / 2 + 2);
                                        }
                                    }
                                }

                                missionAccepts[i]->setString("Cancel");
                                missionAccepts[i]->setOrigin(missionAccepts[i]->getGlobalBounds().width / 2,
                                                             missionAccepts[i]->getGlobalBounds().height / 2 + 2);
                                missionButtons[i]->setColor(sf::Color::Red);

                                activeMissions.push_back(currentSystem->getTasks()[i]);
                            } else if (missionAccepts[i]->getString() == "Cancel") {
                                playBip();
                                if (currentSystem->getTasks()[i]->getType() == Task::DELIVERY) player->setUsedCargoSpace(player->getUsedCargoSpace() - currentSystem->getTasks()[i]->getSize());
                                else if (currentSystem->getTasks()[i]->getType() == Task::TAXI) player->setPassengersAboard(player->getPassengersAboard() - currentSystem->getTasks()[i]->getSize());

                                for (int j = 0; j < missionAccepts.size(); j++) {
                                    if (missionAccepts[j]->getString() == "Too Full") {
                                        int delivTotal = player->getUsedCargoSpace() + currentSystem->getTasks()[j]->getSize();
                                        int passTotal = player->getPassengersAboard() + currentSystem->getTasks()[j]->getSize();
                                        if ((currentSystem->getTasks()[j]->getType() == Task::DELIVERY && delivTotal <= player->getCargoSpace()) || (currentSystem->getTasks()[j]->getType() == Task::TAXI && passTotal <= player->getPassengerSpace())) {
                                            missionButtons[j]->setColor(sf::Color::Green);
                                            missionAccepts[j]->setString("Accept");
                                            missionAccepts[j]->setOrigin(missionAccepts[j]->getGlobalBounds().width / 2, missionAccepts[j]->getGlobalBounds().height / 2);
                                        }
                                    }
                                }
                                missionAccepts[i]->setString("Accept");
                                missionAccepts[i]->setOrigin(missionAccepts[i]->getGlobalBounds().width / 2,
                                                             missionAccepts[i]->getGlobalBounds().height / 2);
                                missionButtons[i]->setColor(sf::Color::Green);

                                delete currentSystem->getTasks()[i];
                                activeMissions.erase(std::find(activeMissions.begin(), activeMissions.end(), currentSystem->getTasks()[i]));
                            } else playErr();
                        }
                    }
                }

                playedBip = false;
                playedErr = false;
            } else if (event.type == sf::Event::MouseMoved) {
                for (int i = 0; i < missionButtons.size(); i++) {
                    if (missionButtonFilters[i]->getGlobalBounds().contains(mousePosF)) missionButtonFilters[i]->setFillColor(sf::Color(0, 0, 0, 100));
                }

                if (departButton.getGlobalBounds().contains(mousePosF) && showPlanetDialog) departButton.setColor(sf::Color::Red);
                else if (missionsButton.getGlobalBounds().contains(mousePosF) && showPlanetDialog) missionsButton.setColor(sf::Color::Red);
                else if (tradeButton.getGlobalBounds().contains(mousePosF) && showPlanetDialog) tradeButton.setColor(sf::Color::Red);
                else if (exploreButton.getGlobalBounds().contains(mousePosF) && showPlanetDialog) exploreButton.setColor(sf::Color::Red);
                else if (yardButton.getGlobalBounds().contains(mousePosF) && showPlanetDialog) yardButton.setColor(sf::Color::Red);
                else if (treasuryButton.getGlobalBounds().contains(mousePosF) && showPlanetDialog) treasuryButton.setColor(sf::Color::Red);
                else if (capitalButton.getGlobalBounds().contains(mousePosF) && showPlanetDialog) capitalButton.setColor(sf::Color::Red);
                else if (lodgeButton.getGlobalBounds().contains(mousePosF) && showPlanetDialog) lodgeButton.setColor(sf::Color::Red);
                else if (refuelRect.getGlobalBounds().contains(mousePosF) && showPlanetDialog && planetScreen == YARD) refuelRect.setFillColor(sf::Color(0, 0, 0, 100));
                else if (repairRect.getGlobalBounds().contains(mousePosF) && showPlanetDialog && planetScreen == YARD) repairRect.setFillColor(sf::Color(0, 0, 0, 100));
                else if (soundButton.getGlobalBounds().contains(mousePosF) && soundOn && showPlanetDialog) soundButton.setColor(sf::Color::Red);
                else if (musicButton.getGlobalBounds().contains(mousePosF) && musicOn && showPlanetDialog) musicButton.setColor(sf::Color::Red);
                else if (soundButton.getGlobalBounds().contains(mousePosF) && !soundOn && showPlanetDialog) soundButton.setColor(sf::Color::White);
                else if (musicButton.getGlobalBounds().contains(mousePosF) && !musicOn && showPlanetDialog) musicButton.setColor(sf::Color::White);

                for (int i = 0; i < mapSprites.size(); i++) {
                    if (mapSprites[i]->getGlobalBounds().contains(mousePosF)) {
                        if (systems[i]->isVisited()) {
                            sysInfoTitle.setString(systems[i]->getName());
                            sysGovText.setString(systems[i]->getGovName());
                            sysRepText.setString(std::to_string(systems[i]->getSysRep()) + "%");
                            if (systems[i] == currentSystem) {
                                sysStatusText.setString("Current System");
                                sysStatusText.setFillColor(sf::Color::Yellow);
                            } else if (systems[i]->getSysRep() <= HOSTILE_HIGH) {
                                sysStatusText.setString("Hostile");
                                sysStatusText.setFillColor(sf::Color::Red);
                            } else if (systems[i]->getSysRep() >= FRIENDLY_LOW) {
                                sysStatusText.setString("Friendly");
                                sysStatusText.setFillColor(sf::Color::Cyan);
                            } else if (systems[i]->getSysRep() >= NUETRAL_LOW && systems[i]->getSysRep() <= NUETRAL_HIGH) {
                                sysStatusText.setString("Neutral");
                                sysStatusText.setFillColor(sf::Color::White);
                            }
                            sysPopText.setString(std::to_string(systems[i]->getPop()) + " billion");
                            sysStrenText.setString(std::to_string(systems[i]->getStren()));
                        } else {
                            sysInfoTitle.setString("???");
                            sysStatusText.setString("Unexplored");
                            sysStatusText.setFillColor(sf::Color::White);
                            sysGovText.setString("???");
                            sysRepText.setString("???");
                            sysPopText.setString("???");
                            sysStrenText.setString("???");
                        }

                        sysInfoTitle.setOrigin(sysInfoTitle.getGlobalBounds().width / 2, sysInfoTitle.getGlobalBounds().height / 2);

                        if (systems[i]->getRelativeMapPos().x >= 0.5) {
                            sysInfoBox.setOrigin(0, 0);
                            sysInfoBox.setPosition(window.getSize().x / (float) 10, window.getSize().y / (float) 5);
                            sysInfoTitle.setPosition(sysInfoBox.getXPos() + sysInfoBox.getGlobalBounds().width / 2, sysInfoBox.getYPos() + sysInfoTitle.getGlobalBounds().height / (float) 1.3);
                            sysStatus.setPosition(sysInfoBox.getXPos() + 15, sysInfoTitle.getPosition().y + 37);
                            sysGov.setPosition(sysInfoBox.getXPos() + 15, sysStatus.getPosition().y + sysGov.getGlobalBounds().height + 6);
                            sysRep.setPosition(sysInfoBox.getXPos() + 15, sysGov.getPosition().y + sysRep.getGlobalBounds().height + 4);
                            sysPop.setPosition(sysInfoBox.getXPos() + 15, sysRep.getPosition().y + sysPop.getGlobalBounds().height + 3);
                            sysStren.setPosition(sysInfoBox.getXPos() + 15, sysPop.getPosition().y + sysStren.getGlobalBounds().height + 3);
                        } else {
                            sysInfoBox.setOrigin(sysInfoBox.getLocalBounds().width, 0);
                            sysInfoBox.setPosition(window.getSize().x - window.getSize().x / (float) 10, window.getSize().y / (float) 5);
                            sysInfoTitle.setPosition(sysInfoBox.getXPos() - sysInfoBox.getGlobalBounds().width / 2, sysInfoBox.getYPos() + sysInfoTitle.getGlobalBounds().height / (float) 1.3);
                            sysStatus.setPosition(sysInfoBox.getXPos() - sysInfoBox.getGlobalBounds().width + 15, sysInfoTitle.getPosition().y + 37);
                            sysGov.setPosition(sysInfoBox.getXPos() - sysInfoBox.getGlobalBounds().width + 15, sysStatus.getPosition().y + sysGov.getGlobalBounds().height + 6);
                            sysRep.setPosition(sysInfoBox.getXPos() - sysInfoBox.getGlobalBounds().width + 15, sysGov.getPosition().y + sysRep.getGlobalBounds().height + 4);
                            sysPop.setPosition(sysInfoBox.getXPos() - sysInfoBox.getGlobalBounds().width + 15, sysRep.getPosition().y + sysPop.getGlobalBounds().height + 3);
                            sysStren.setPosition(sysInfoBox.getXPos() - sysInfoBox.getGlobalBounds().width + 15, sysPop.getPosition().y + sysStren.getGlobalBounds().height + 3);
                        }

                        sysStatusText.setPosition(sysStatus.getPosition().x + sysStatus.getGlobalBounds().width, sysStatus.getPosition().y + sysStatusText.getGlobalBounds().height / (float) 2.75);
                        sysGovText.setPosition(sysGov.getPosition().x + sysGov.getGlobalBounds().width, sysGov.getPosition().y + sysGovText.getGlobalBounds().height / (float) 2.75);
                        sysRepText.setPosition(sysRep.getPosition().x + sysRep.getGlobalBounds().width, sysRep.getPosition().y + sysRepText.getGlobalBounds().height / (float) 2.75);
                        sysPopText.setPosition(sysPop.getPosition().x + sysPop.getGlobalBounds().width, sysPop.getPosition().y + sysPopText.getGlobalBounds().height / (float) 2.75);
                        sysStrenText.setPosition(sysStren.getPosition().x + sysStren.getGlobalBounds().width, sysStren.getPosition().y + sysStrenText.getGlobalBounds().height / (float) 2.75);
                    }
                }
            }

            showSysInfo = false;
            for (auto a : mapSprites) {
                if (a->getGlobalBounds().contains(mousePosF)) showSysInfo = true;
            }

            if (!departButton.getGlobalBounds().contains(mousePosF)) departButton.setColor(sf::Color::White);
            if (!missionsButton.getGlobalBounds().contains(mousePosF)) missionsButton.setColor(sf::Color::White);
            if (!tradeButton.getGlobalBounds().contains(mousePosF)) tradeButton.setColor(sf::Color::White);
            if (!exploreButton.getGlobalBounds().contains(mousePosF)) exploreButton.setColor(sf::Color::White);
            if (!yardButton.getGlobalBounds().contains(mousePosF)) yardButton.setColor(sf::Color::White);
            if (!treasuryButton.getGlobalBounds().contains(mousePosF)) treasuryButton.setColor(sf::Color::White);
            if (!capitalButton.getGlobalBounds().contains(mousePosF)) capitalButton.setColor(sf::Color::White);
            if (!lodgeButton.getGlobalBounds().contains(mousePosF)) lodgeButton.setColor(sf::Color::White);
            if (!soundButton.getGlobalBounds().contains(mousePosF) && soundOn) soundButton.setColor(sf::Color::White);
            if (!musicButton.getGlobalBounds().contains(mousePosF) && musicOn) musicButton.setColor(sf::Color::White);
            if (!soundButton.getGlobalBounds().contains(mousePosF) && !soundOn) soundButton.setColor(sf::Color::Red);
            if (!musicButton.getGlobalBounds().contains(mousePosF) && !musicOn) musicButton.setColor(sf::Color::Red);
            if (!refuelRect.getGlobalBounds().contains(mousePosF)) refuelRect.setFillColor(sf::Color(0, 0, 0, 0));
            if (!repairRect.getGlobalBounds().contains(mousePosF)) repairRect.setFillColor(sf::Color(0, 0, 0, 0));

            for (int i = 0; i < missionButtons.size(); i++) {
                if (!missionButtonFilters[i]->getGlobalBounds().contains(mousePosF)) {
                    missionButtonFilters[i]->setFillColor(sf::Color(0, 0, 0, 0));
                }
            }
        }
    }
}

void Game::playBip() {
    if (!playedBip && soundOn) {
        playedBip = true;
        bipSound.setBuffer(bip);
        bipSound.setVolume(100);
        bipSound.play();
    }
}

void Game::playErr() {
    if (!playedErr && soundOn) {
        playedErr = true;
        errSound.setBuffer(err);
        errSound.setVolume(100);
        errSound.play();
    }
}

void Game::updateLoader(sf::RenderWindow &window, const std::string &msg = "") {
    loadedTextures++;
    window.clear(sf::Color::Black);

    //auto start = std::chrono::high_resolution_clock::now();
    GameSprite barEmpty(loadingBarEmpty, 100);
    GameSprite barFull(loadingBarFull, 100);
    barFull.setScale(1.1, 0.87);
    sf::FloatRect fullBarRect = barFull.getLocalBounds();
    barFull.setOrigin(fullBarRect.width / 2, fullBarRect.height / 2);

    barEmpty.setPosition(window.getSize().x / (float) 2, window.getSize().y / (float) 2);
    barFull.setPosition(barEmpty.getPosition().x, barEmpty.getPosition().y);
    barFull.setTextureRect(sf::IntRect(fullBarRect.left, fullBarRect.top, fullBarRect.width / (float) totalTextures * (float) loadedTextures, fullBarRect.height));

    sf::Text loadingMessage(msg, oxan, 15);
    loadingMessage.setOrigin(loadingMessage.getGlobalBounds().width / 2, loadingMessage.getGlobalBounds().height / 2);
    loadingMessage.setPosition(window.getSize().x / 2, window.getSize().y / 2 + 50);
    //auto stop = std::chrono::high_resolution_clock::now();
    //auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    //std::cout << duration.count() << " microseconds" << std::endl;

    window.draw(barEmpty);
    window.draw(barFull);
    window.draw(loadingMessage);

    window.display();
}

void Game::readNameComponents() {
    abstractNounNameComponents = readFileLines("./data/Ship Names/Abstract Nouns.txt");
    adjectiveNameComponents = readFileLines("./data/Ship Names/Adjectives.txt");
    animalNameComponents = readFileLines("./data/Ship Names/Animals.txt");
    standaloneNameNameComponents = readFileLines("./data/Ship Names/Endless Sky Names.txt");
    femaleNameNameComponents = readFileLines("./data/Ship Names/Female Names.txt");
    femaleTitleNameComponents = readFileLines("./data/Ship Names/Female Titles.txt");
    neutralTitleNameComponents = readFileLines("./data/Ship Names/Gender Neutral Titles.txt");
    maleNameNameComponents = readFileLines("./data/Ship Names/Male Names.txt");
    maleTitleNameComponents = readFileLines("./data/Ship Names/Male Titles.txt");
    nounNameComponents = readFileLines("./data/Ship Names/Nouns.txt");
    numberNameComponents = readFileLines("./data/Ship Names/Numbers.txt");
    craftNameNameComponents = readFileLines("./data/Ship Names/Spacecraft Names.txt");
}

template<class RNG>
std::string Game::generateName(RNG &gen) {
    //TODO: things you can be of
    std::string name;
    int addNumber = std::uniform_int_distribution<int>(1, 3)(gen);

    int namingScheme = std::uniform_int_distribution<int>(1, 12)(gen);
    switch (namingScheme) {
        case 1:     //standalone naming scheme
            name = pickRandomItem(standaloneNameNameComponents, gen);
            break;
        case 2:     //existing craft naming scheme
            name = pickRandomItem(craftNameNameComponents, gen);
            break;
        case 3: {   //adjective based naming scheme
            int nounType = std::uniform_int_distribution<int>(1, 5)(gen);
            switch (nounType) {
                case 1:     //animal
                    name = pickRandomItem(animalNameComponents, gen);
                    break;
                case 2:     //noun
                    name = pickRandomItem(nounNameComponents, gen);
                    break;
                case 3:     //abstract
                    name = pickRandomItem(abstractNounNameComponents, gen);
                    break;
                case 4: {    //name
                    int nameType = std::uniform_int_distribution<int>(1, 2)(gen);
                    if (nameType == 1) name = pickRandomItem(femaleNameNameComponents, gen);
                    else name = pickRandomItem(maleNameNameComponents, gen);
                    break;
                }
                case 5: {    //title
                    int titleType = std::uniform_int_distribution<int>(1, 3)(gen);
                    if (titleType == 1) name = pickRandomItem(femaleTitleNameComponents, gen);
                    else if (titleType == 2) name = pickRandomItem(maleTitleNameComponents, gen);
                    else name = pickRandomItem(neutralTitleNameComponents, gen);
                    break;
                }
            }

            name = pickRandomItem(adjectiveNameComponents, gen) + " " + name;
            break;
        }
        case 4: {   //noun based naming scheme
            std::string name1, name2;
            int firstType = std::uniform_int_distribution<int>(1, 3)(gen);
            switch (firstType) {
                case 1:     //adjective
                    name1 = pickRandomItem(animalNameComponents, gen);
                    break;
                case 2:     //noun
                    name1 = pickRandomItem(nounNameComponents, gen);
                    break;
                case 3:     //abstract
                    name1 = pickRandomItem(abstractNounNameComponents, gen);
                    break;
            }

            int secondType = std::uniform_int_distribution<int>(1, 2)(gen);
            switch (secondType) {
                case 1:     //noun
                    name2 = pickRandomItem(nounNameComponents, gen);
                    break;
                case 2:     //abstract
                    name2 = pickRandomItem(abstractNounNameComponents, gen);
                    break;
            }

            name = name1 + " " + name2;
            break;
        }
        case 5: {   //possesive naming scheme
            std::string name1, name2;
            int firstType = std::uniform_int_distribution<int>(1, 2)(gen);
            switch (firstType) {
                case 1: {    //name
                    int nameType = std::uniform_int_distribution<int>(1, 2)(gen);
                    if (nameType == 1) name1 = pickRandomItem(femaleNameNameComponents, gen);
                    else name1 = pickRandomItem(maleNameNameComponents, gen);
                    break;
                }
                case 2: {    //title
                    int titleType = std::uniform_int_distribution<int>(1, 3)(gen);
                    if (titleType == 1) name1 = pickRandomItem(femaleTitleNameComponents, gen);
                    else if (titleType == 2) name1 = pickRandomItem(maleTitleNameComponents, gen);
                    else name1 = pickRandomItem(neutralTitleNameComponents, gen);
                    break;
                }
            }

            int secondType = std::uniform_int_distribution<int>(1, 4)(gen);
            switch (secondType) {
                case 1:     //animal
                    name2 = pickRandomItem(animalNameComponents, gen);
                    break;
                case 2:     //noun
                    name2 = pickRandomItem(nounNameComponents, gen);
                    break;
                case 3:     //abstract
                    name2 = pickRandomItem(abstractNounNameComponents, gen);
                case 4: {    //title
                    int titleType = std::uniform_int_distribution<int>(1, 3)(gen);
                    if (titleType == 1) name2 = pickRandomItem(femaleTitleNameComponents, gen);
                    else if (titleType == 2) name2 = pickRandomItem(maleTitleNameComponents, gen);
                    else name2 = pickRandomItem(neutralTitleNameComponents, gen);
                    break;
                }
            }

            name = name1 + "'s " + name2;
            break;
        }
        case 6: {   //noun and noun based naming scheme
            std::string name1, name2;
            int firstType = std::uniform_int_distribution<int>(1, 2)(gen);
            switch (firstType) {
                case 1:     //noun
                    name1 = pickRandomItem(nounNameComponents, gen);
                    break;
                case 2:     //abstract
                    name1 = pickRandomItem(abstractNounNameComponents, gen);
                    break;
            }

            int secondType = std::uniform_int_distribution<int>(1, 2)(gen);
            switch (secondType) {
                case 1:     //noun
                    name2 = pickRandomItem(nounNameComponents, gen);
                    break;
                case 2:     //abstract
                    name2 = pickRandomItem(abstractNounNameComponents, gen);
                    break;
            }

            name = name1 + " and " + name2;
            break;
        }
        case 7: {   //male naming scheme
            std::string name1, name2;

            int nameType = std::uniform_int_distribution<int>(1, 2)(gen);
            switch (nameType) {
                case 1:     //name
                    name1 = pickRandomItem(maleNameNameComponents, gen);
                    break;
                case 2:     //title
                    name1 = pickRandomItem(maleTitleNameComponents, gen);
                    break;
            }

            int nounType = std::uniform_int_distribution<int>(1, 1)(gen);
            switch (nounType) {
                case 1:     //name
                    name2 = pickRandomItem(maleNameNameComponents, gen);
                    break;
            }

            name = name1 + " " + name2;
        }
        case 8: {   //female naming scheme
            std::string name1, name2;

            int nameType = std::uniform_int_distribution<int>(1, 2)(gen);
            switch (nameType) {
                case 1:     //name
                    name1 = pickRandomItem(femaleNameNameComponents, gen);
                    break;
                case 2:     //title
                    name1 = pickRandomItem(femaleTitleNameComponents, gen);
                    break;
            }

            int nounType = std::uniform_int_distribution<int>(1, 1)(gen);
            switch (nounType) {
                case 1:     //name
                    name2 = pickRandomItem(femaleNameNameComponents, gen);
                    break;
            }

            name = name1 + " " + name2;
        }
        case 9: {   //Alexander the great naming scheme
            int nameType = std::uniform_int_distribution<int>(1, 2)(gen);
            if (nameType == 1) name = pickRandomItem(femaleNameNameComponents, gen);
            else name = pickRandomItem(maleNameNameComponents, gen);
            break;

            name = name + " the " + pickRandomItem(adjectiveNameComponents, gen);
            break;
        }
        case 10: {   //title based naming scheme
            std::string name1, name2;
            int firstType = std::uniform_int_distribution<int>(1, 2)(gen);
            switch (firstType) {
                case 1:     //noun
                    name1 = pickRandomItem(nounNameComponents, gen);
                    break;
                case 2:     //adj
                    name1 = pickRandomItem(abstractNounNameComponents, gen);
                    break;
            }

            int titleType = std::uniform_int_distribution<int>(1, 3)(gen);
            if (titleType == 1) name2 = pickRandomItem(femaleTitleNameComponents, gen);
            else if (titleType == 2) name2 = pickRandomItem(maleTitleNameComponents, gen);
            else name2 = pickRandomItem(neutralTitleNameComponents, gen);

            name = name1 + " " + name2;
            break;
        }
        case 11: {   //possesive naming scheme
            int nounType = std::uniform_int_distribution<int>(1, 4)(gen);
            switch (nounType) {
                case 1:     //animal
                    name = pickRandomItem(animalNameComponents, gen);
                    break;
                case 2:     //noun
                    name = pickRandomItem(nounNameComponents, gen);
                    break;
                case 3:     //abstract
                    name = pickRandomItem(abstractNounNameComponents, gen);
                case 4: {    //title
                    int titleType = std::uniform_int_distribution<int>(1, 3)(gen);
                    if (titleType == 1) name = pickRandomItem(femaleTitleNameComponents, gen);
                    else if (titleType == 2) name = pickRandomItem(maleTitleNameComponents, gen);
                    else name = pickRandomItem(neutralTitleNameComponents, gen);
                    break;
                }
            }

            name = pickRandomItem(abstractNounNameComponents, gen) + " " + name;
            break;
        }
        case 12: {  //single word naming scheme
            int nounType = std::uniform_int_distribution<int>(1, 5)(gen);
            switch (nounType) {
                case 1:     //animal
                    name = pickRandomItem(animalNameComponents, gen);
                    break;
                case 2:     //noun
                    name = pickRandomItem(nounNameComponents, gen);
                    break;
                case 3:     //abstract
                    name = pickRandomItem(abstractNounNameComponents, gen);
                    break;
                case 4: {    //name
                    int nameType = std::uniform_int_distribution<int>(1, 2)(gen);
                    if (nameType == 1) name = pickRandomItem(femaleNameNameComponents, gen);
                    else name = pickRandomItem(maleNameNameComponents, gen);
                    break;
                }
                case 5: {    //title
                    int titleType = std::uniform_int_distribution<int>(1, 3)(gen);
                    if (titleType == 1) name = pickRandomItem(femaleTitleNameComponents, gen);
                    else if (titleType == 2) name = pickRandomItem(maleTitleNameComponents, gen);
                    else name = pickRandomItem(neutralTitleNameComponents, gen);
                    break;
                }
            }
        }
    }

    if (addNumber == 2) name += " " + pickRandomItem(numberNameComponents, gen);
    return name;
}
