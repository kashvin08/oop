#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <memory>
#include <random>
#include <ctime>
#include <iomanip>
#include <queue>

using namespace std;

const char EMPTY = '.';
const char GENERIC_SYMBOL = 'G';
const int MAX_LIVES = 3;
const int MAX_SHELLS = 10;
const int RESPAWN_LIMIT = 3;

struct Position {
    int x, y;
};

class Robot;
using RobotPtr = shared_ptr<Robot>;

struct Cell {
    char symbol = EMPTY;
    RobotPtr robot = nullptr;
};

class MovingRobot { public: virtual void move(vector<vector<Cell>>& battlefield, int width, int height) = 0; };
class ShootingRobot { public: virtual void fire(vector<vector<Cell>>& battlefield, int dx, int dy) = 0; };
class SeeingRobot { public: virtual void look(vector<vector<Cell>>& battlefield, int dx, int dy) = 0; };
class ThinkingRobot { public: virtual void think() = 0; };

class Robot : public MovingRobot, public ShootingRobot, public SeeingRobot, public ThinkingRobot {
protected:
    string name, type;
    int x, y;
    int lives, shells, respawns;
    bool alive;

public:
    Robot(string name, int x, int y) : name(name), x(x), y(y), lives(MAX_LIVES), shells(MAX_SHELLS), respawns(0), alive(true) {}

    virtual void move(vector<vector<Cell>>& battlefield, int width, int height) override = 0;
    virtual void fire(vector<vector<Cell>>& battlefield, int dx, int dy) override = 0;
    virtual void look(vector<vector<Cell>>& battlefield, int dx, int dy) override = 0;
    virtual void think() override = 0;

    string getName() const { return name; }
    int getX() const { return x; }
    int getY() const { return y; }
    bool isAlive() const { return alive; }

    void setPosition(int newX, int newY) { x = newX; y = newY; }
    char getSymbol() const { return GENERIC_SYMBOL; }

    void markDead() { alive = false; }
    void reset() {
        shells = MAX_SHELLS;
        alive = true;
        respawns++;
    }

    int getRespawns() const { return respawns; }
};

class GenericRobot : public Robot {
public:
    GenericRobot(string name, int x, int y) : Robot(name, x, y) {}

    void think() override {
        cout << name << " is thinking...\n";
    }

    void look(vector<vector<Cell>>& battlefield, int dx, int dy) override {
        int cx = x + dx, cy = y + dy;
        cout << name << " looks at vicinity around (" << cx << "," << cy << ")\n";
        for (int i = -1; i <= 1; ++i)
            for (int j = -1; j <= 1; ++j)
                if (cy + i >= 0 && cy + i < battlefield.size() &&
                    cx + j >= 0 && cx + j < battlefield[0].size()) {
                    auto r = battlefield[cy + i][cx + j].robot;
                    if (r)
                        cout << "  Sees " << r->getName() << " at (" << cx + j << "," << cy + i << ")\n";
                }
    }

    void fire(vector<vector<Cell>>& battlefield, int dx, int dy) override {
        if (shells <= 0) {
            cout << name << " is out of shells and self-destructs!\n";
            markDead();
            return;
        }
        if (dx == 0 && dy == 0) return;

        int tx = x + dx, ty = y + dy;
        if (ty >= 0 && ty < battlefield.size() && tx >= 0 && tx < battlefield[0].size()) {
            shells--;
            cout << name << " fires at (" << tx << "," << ty << ")\n";
            if (battlefield[ty][tx].robot) {
                int hit = rand() % 100;
                if (hit < 70) {
                    cout << "  HIT! Robot " << battlefield[ty][tx].robot->getName() << " destroyed!\n";
                    battlefield[ty][tx].robot->markDead();
                    battlefield[ty][tx].robot = nullptr;
                    battlefield[ty][tx].symbol = EMPTY;
                } else {
                    cout << "  Missed!\n";
                }
            } else {
                cout << "  Nothing there.\n";
            }
        }
    }

    void move(vector<vector<Cell>>& battlefield, int width, int height) override {
        int dx[] = { -1, 0, 1, 0 };
        int dy[] = { 0, -1, 0, 1 };
        int dir = rand() % 4;
        int nx = x + dx[dir], ny = y + dy[dir];
        if (nx >= 0 && nx < width && ny >= 0 && ny < height && battlefield[ny][nx].robot == nullptr) {
            battlefield[y][x].symbol = EMPTY;
            battlefield[y][x].robot = nullptr;
            x = nx; y = ny;
            battlefield[y][x].symbol = getSymbol();
            battlefield[y][x].robot = shared_from_this();
            cout << name << " moves to (" << x << "," << y << ")\n";
        } else {
            cout << name << " stays in place.\n";
        }
    }
};

class Simulator {
    int width, height, steps;
    vector<vector<Cell>> battlefield;
    vector<shared_ptr<GenericRobot>> robots;
    queue<shared_ptr<GenericRobot>> respawnQueue;

public:
    void loadConfig(const string& filename) {
        ifstream file(filename);
        if (!file) {
            cerr << "Cannot open file!\n"; return;
        }

        string line;
        while (getline(file, line)) {
            istringstream iss(line);
            string type, name, xs, ys;
            if (line.find("M by N") != string::npos)
                iss >> type >> type >> width >> height;
            else if (line.find("steps:") != string::npos)
                iss >> type >> steps;
            else if (line.find("robots:") != string::npos)
                continue;
            else if (isalpha(line[0])) {
                iss >> type >> name >> xs >> ys;
                int x = xs == "random" ? rand() % width : stoi(xs);
                int y = ys == "random" ? rand() % height : stoi(ys);
                auto robot = make_shared<GenericRobot>(name, x, y);
                robots.push_back(robot);
            }
        }
        battlefield.assign(height, vector<Cell>(width));
        for (auto& r : robots) {
            if (r->isAlive()) {
                battlefield[r->getY()][r->getX()].robot = r;
                battlefield[r->getY()][r->getX()].symbol = r->getSymbol();
            }
        }
    }

    void displayBattlefield() {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                cout << battlefield[y][x].symbol << " ";
            }
            cout << "\n";
        }
    }

    void simulate() {
        for (int t = 0; t < steps; ++t) {
            cout << "\n=== Turn " << t + 1 << " ===\n";
            for (auto& r : robots) {
                if (!r->isAlive()) {
                    if (r->getRespawns() < RESPAWN_LIMIT)
                        respawnQueue.push(r);
                    continue;
                }
                r->think();
                r->look(battlefield, 0, 0);
                r->fire(battlefield, 1, 0);
                r->move(battlefield, width, height);
            }

            if (!respawnQueue.empty()) {
                auto r = respawnQueue.front(); respawnQueue.pop();
                int x = rand() % width, y = rand() % height;
                if (!battlefield[y][x].robot) {
                    r->setPosition(x, y);
                    r->reset();
                    battlefield[y][x].robot = r;
                    battlefield[y][x].symbol = r->getSymbol();
                    cout << r->getName() << " respawns at (" << x << "," << y << ")\n";
                } else {
                    respawnQueue.push(r);  // retry later
                }
            }

            displayBattlefield();
            if (countAlive() <= 1) break;
        }
    }

    int countAlive() {
        int count = 0;
        for (auto& r : robots)
            if (r->isAlive()) count++;
        return count;
    }
};

int main() {
    srand(time(0));
    Simulator sim;
    sim.loadConfig("config.txt");
    sim.simulate();
    return 0;
}
