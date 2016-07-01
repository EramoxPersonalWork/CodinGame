#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * CodinGame planet is being attacked by slimy insectoid aliens.
 * <---
 * Hint:To protect the planet, you can implement the pseudo-code provided in the statement, below the player.
 **/
 
typedef struct Enemy{
    int mDistance;
    string mName;
    Enemy() : Enemy(0, "undefined") {}
    Enemy(int dist, string name):  mDistance(dist), mName(name) {}
    int getDist() const {return mDistance;}
    string getName() const {return mName;}
};

typedef vector<Enemy> Threat;

int main()
{
    Threat threat = Threat(0);
    // game loop
    while (1) {
        string enemy1; // name of enemy 1
        cin >> enemy1; cin.ignore();
        int dist1; // distance to enemy 1
        cin >> dist1; cin.ignore();
        threat.push_back(Enemy(dist1, enemy1));

        string enemy2; // name of enemy 2
        cin >> enemy2; cin.ignore();
        int dist2; // distance to enemy 2
        cin >> dist2; cin.ignore();
        threat.push_back(Enemy(dist2, enemy2));

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        sort(threat.begin(), threat.end(), 
            [](const Enemy & a, const Enemy & b)
                { 
                    return a.getDist() > b.getDist(); 
                });
        Enemy threatToEliminate = threat.back();
        threat.pop_back();
        // You have to output a correct ship name to shoot ("Buzz", enemy1, enemy2, ...)
        cout << threatToEliminate.getName() << endl;
    }
}
