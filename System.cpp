#include "System.h"

System::System(std::string _name) {
    name = std::move(_name);
}

sf::Sprite* System::getSystemCenter() {
    return planets[1];
}

void System::addPlanet(Planet *p) {
    planets.push_back(p);
    if (p->isLandable()) landable = true;
}

void System::setRelativeMapPos(const sf::Vector2f &pos) {
    mapPos = pos;
}

sf::Vector2f System::getRelativeMapPos() {
    return mapPos;
}

void System::makeVisited() {
    visited = true;
}

bool System::isVisited() const {
    return visited;
}

std::string System::getName() {
    return name;
}

void System::setGovName(std::string gov) {
    govName = std::move(gov);
}

std::string System::getGovName() {
    return govName;
}

void System::setSysRep(int _rep) {
    rep = _rep;
}

int System::getSysRep() const {
    return rep;
}

void System::setPop(int _pop) {
    pop = _pop;
}

int System::getPop() const {
    return pop;
}

void System::setStren(int _stren) {
    stren = _stren;
}

int System::getStren() const {
    return stren;
}

bool System::isLandable() const {
    return landable;
}

const std::vector<Planet *> &System::getPlanets() const {
    return planets;
}

void System::setPlanets(const std::vector<Planet *> &planetList) {
    System::planets = planetList;
}

std::vector<Task *> &System::getTasks(){
    return tasks;
}

const std::vector<int> &System::getExits() const {
    return exits;
}

void System::addExit(int exit) {
    exits.push_back(exit);
}

void System::addTask(Task *task) {
    tasks.push_back(task);
}
