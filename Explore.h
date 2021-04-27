//
// Created by benmo on 5/7/2020.
//

#ifndef SFML_TEMPLATE_EXPLORE_H
#define SFML_TEMPLATE_EXPLORE_H

#include <string>
#include <utility>

class Explore {
private:
    std::string type, message;
    int moneyHigh, moneyLow, prestigeHigh, prestigeLow;
public:
    Explore(std::string _type, std::string msg, int _moneyLow = 0, int _moneyHigh = 0, int _prestigeLow = 0, int _prestigeHigh = 0) {
        message = std::move(msg);

        type = _type;
        moneyHigh = _moneyHigh;
        moneyLow = _moneyLow;
        prestigeHigh = _prestigeHigh;
        prestigeLow = _prestigeLow;
    }

    std::string getMessage() {
        return message;
    }
};


#endif //SFML_TEMPLATE_EXPLORE_H
