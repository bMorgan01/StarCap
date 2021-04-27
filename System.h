//
// Created by benmo on 2/24/2020.
//

#ifndef SFML_TEMPLATE_SYSTEM_H
#define SFML_TEMPLATE_SYSTEM_H

#include <utility>
#include <vector>
#include <string>
#include "Planet.h"
#include "Task.h"

class System {
private:
    std::string name;
    std::string govName;

    sf::Vector2f mapPos;
    bool visited = false;
    bool landable = false;

    int rep = 5;
    int pop = 0;
    int stren = 0;

    std::vector<Planet*> planets;
    std::vector<Task*> tasks;
    std::vector<int> exits;
public:
    explicit System(std::string _name);

    sf::Sprite* getSystemCenter();

    void addPlanet(Planet *p);

    void setRelativeMapPos(const sf::Vector2f &pos);
    sf::Vector2f getRelativeMapPos();

    void makeVisited();
    bool isVisited() const;

    std::string getName();

    void setGovName(std::string gov);
    std::string getGovName();

    void setSysRep(int _rep);
    int getSysRep() const;

    void setPop(int _pop);
    int getPop() const;

    void setStren(int _stren);
    int getStren() const;

    bool isLandable() const;

    const std::vector<Planet *> &getPlanets() const;
    void setPlanets(const std::vector<Planet *> &planetList);

    std::vector<Task *> &getTasks();
    void addTask(Task* task);

    const std::vector<int> &getExits() const;
    void addExit(int exit);
};


#endif //SFML_TEMPLATE_SYSTEM_H
