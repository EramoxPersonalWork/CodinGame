#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>

using namespace std;

struct Timeout{
    // Time at which the timeout is started
    clock_t mStart;
    // Value of the timeout in seconds
    int mTimeout;
    bool mStarted;

    static const int TIME_UNDEFINED = -1;

    Timeout(): Timeout(0) {}
    Timeout(int timeout): mStart(0), mTimeout(timeout), mStarted(false) {}

    bool start() {
        if (!mStarted) mStart = clock();
        mStarted = true;
        return mStart != TIME_UNDEFINED;
    }

    bool hasTimedOut() {
        if (mStarted) {
            clock_t temp = clock();
            int time_elapsed = (temp - mStart) / CLOCKS_PER_SEC;
            bool timeoutReached = time_elapsed > mTimeout;
            if(timeoutReached) {cerr << "Timeout reached" << endl;}
            return timeoutReached;
        } else {
            cerr << "Timeout has not started yet" << endl;
        }
        return true;
    }
};

struct Enemy{
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
    Timeout loopTimeout = Timeout(40);
    // game loop
    while (1) {
        loopTimeout.start();
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

        // We have acquired the list of enemy
        // We have to sort the list from the
        // closest to far away
        sort(threat.begin(), threat.end(),
             [](const Enemy & a, const Enemy & b)
             {
                 // true will make the sort to let
                 // the object in the same order
                 return a.getDist() < b.getDist();
             });

        // We acquire the enemy which is the closer,
        // ie at the end of the list of threat
        Enemy threatToEliminate = threat.front();
        threat.erase(threat.begin());

        // We output the enemy to kill
        cout << threatToEliminate.getName() << endl;

        if(loopTimeout.hasTimedOut()) break;
    }

    return EXIT_SUCCESS;
}
