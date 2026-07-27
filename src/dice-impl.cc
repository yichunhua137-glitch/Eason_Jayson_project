module;

#include <cstdlib>
#include <iostream>

module dice;

using namespace std;

int LoadedDice::roll() {
    int value;

    while (true) {
        cout << "Input a roll between 2 and 12:"
             << endl;
        cout << "> ";

        if (cin >> value) {
            cin.ignore(10000, '\n');

            if (value >= 2 && value <= 12) {
                return value;
            }

            cout << "Invalid roll." << endl;
        } else {
            if (cin.eof()) {
                return -1;
            }

            cin.clear();
            cin.ignore(10000, '\n');

            cout << "Invalid roll." << endl;
        }
    }
}


int FairDice::roll() {
    int first = rand() % 6 + 1;
    int second = rand() % 6 + 1;

    return first + second;
}
