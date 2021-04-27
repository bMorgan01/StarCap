#include <iostream>                             // for standard input/output
using namespace std;                            // using the standard namespace

#include "Game.h"
#include "Menu.h"

int main() {
    // display "Hello, World!" -- this still appears in our Run terminal as before
    cout << "Hello, World!" << endl;

    int result = EXIT_SUCCESS;
    while (result == EXIT_SUCCESS) {
        Menu m;
        result = m.result;
        if (result == EXIT_SUCCESS) Game g(m.soundOn, m.musicOn);
    }

    return EXIT_SUCCESS;                        // report our program exited successfully
}