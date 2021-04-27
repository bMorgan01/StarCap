//
// Created by benmo on 3/19/2020.
//

#ifndef SFML_TEMPLATE_TASK_H
#define SFML_TEMPLATE_TASK_H

#include <string>
#include "Planet.h"

class System;

class Task {
private:
    std::string name, desc;
    int type, size, reward;
    System *sysLoc;
    Planet *loc;

public:
    static const int DELIVERY = 0, TAXI = 1;

    Task(int _type, const std::string& _name, const std::string& _desc, System *_sysLoc, Planet *_loc, int _reward, int _size) {
        type = _type;
        name = _name;
        desc = _desc;
        reward = _reward;
        size = _size;
        sysLoc = _sysLoc;
        loc = _loc;
    }

    int getType() const {
        return type;
    }

    int getReward() const {
        return reward;
    }

    int getSize() const {
        return size;
    }

    System* getSystem() {
        return sysLoc;
    }

    Planet* getPlanet() {
        return loc;
    }
};


#endif //SFML_TEMPLATE_TASK_H
