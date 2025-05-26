#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <queue>
using namespace std;
class Battlefield; // Forward declaration of Battlefield class
class Robot
{
protected:
    int robotPositionX = -1;
    int robotPositionY = -1;
    string id_ = ""; // eg GRO1, to display robot id on the Battlefield
    string robotType_ = "";
    string robotName_ = ""; // Robot id underscore robot name, eg GROS5_Star
    int numOfLives_ = 3;
    int numOfKills_ = 0;

public:
    // PC + DC
    Robot(string id = "", int x = -1, int y = -1) : id_(id), robotPositionX(x), robotPositionY(y) {}
    virtual ~Robot() {}
    // Getter and Setter functions
    int x() const
    {
        return robotPositionX;
    }
    void setX(int x)
    {
        robotPositionX = x;
    }
    int y() const
    {
        return robotPositionY;
    }
    void setY(int y)
    {
        robotPositionY = y;
    }
    string id() const
    {
        return id_;
    }
    void setId(string id)
    {
        id_ = id;
    }
    string robotType() const
    {
        return robotType_;
    }
    void setRobotType(string robotType)
    {
        robotType_ = robotType;
    }
    string robotName() const
    {
        return robotName_;
    }
    void setRobotName(string robotName)
    {
        robotName_ = robotName;
    }
    int numOfLives() const
    {
        return numOfLives_;
    }
    void setNumOfLives(int numOfLives)
    {
        numOfLives_ = numOfLives;
    }
    int numOfKills() const
    {
        return numOfKills_;
    }
    void setNumOfKills(int numOfKills)
    {
        numOfKills_ = numOfKills;
    }
    void reduceLife()
    {
        if (numOfLives_ > 0)
            numOfLives_--;
    }
    void incrementKills()
    {
        numOfKills_++;
    }
    bool isAlive() const
    {
        return numOfLives_ > 0;
    }
    // Overloading the << operator for Robot class
    friend ostream &operator<<(ostream &out, const Robot &r)
    {
        out << r.id_ << " at (" << r.robotPositionX << ", " << r.robotPositionY << ")";
        return out;
    }
    // Pure virtual functions
    virtual void setLocation(int x, int y) = 0;
    virtual void actions(Battlefield *battlefield) = 0;
};

class ThinkingRobot : virtual public Robot
{
protected:
    // data member
public:
    virtual ~ThinkingRobot() {}
    virtual void actionThink(Battlefield *battlefield) = 0; // Pure virtual function for stepping
};

class SeeingRobot : virtual public Robot
{
protected:
    // data member
public:
    virtual ~SeeingRobot()
    {
    }
    virtual void actionLook(Battlefield *battlefield) = 0; // Pure virtual function for looking
};

class ShootingRobot : virtual public Robot
{
protected:
    // data member
public:
    virtual ~ShootingRobot() {}
    virtual void actionFire(Battlefield *battlefield) = 0; // Pure virtual function for moving
};

class MovingRobot : virtual public Robot
{
protected:
    // data member
public:
    virtual ~MovingRobot() {}
    virtual void actionMove(Battlefield *battlefield) = 0; // Pure virtual function for moving
};

class GenericRobot : public ThinkingRobot, public SeeingRobot, public ShootingRobot, public MovingRobot
{
private:
    static int robotAutoIncrementInt_; // Static member for auto-incrementing ID
    // data member
public:
    GenericRobot(string id = "", int x = -1, int y = -1)
    {
        id_ = id;
        robotPositionX = x;
        robotPositionY = y;
        robotAutoIncrementInt_++;
    }
    static int robotAutoIncrementInt() { return robotAutoIncrementInt_; }
    virtual ~GenericRobot() {}
    virtual void setLocation(int x, int y)
    {
        robotPositionX = x;
        robotPositionY = y;
    }
    virtual void actionThink(Battlefield *battlefield);
    virtual void actionLook(Battlefield *battlefield);
    virtual void actionFire(Battlefield *battlefield);
    virtual void actionMove(Battlefield *battlefield);
    virtual void actions(Battlefield *battlefield)
    {
        int randomActionThink = 0;
        if (randomActionThink % 2 == 0)
        {
            actionThink(battlefield);
            actionLook(battlefield);
            actionFire(battlefield);
            actionMove(battlefield);
        }
        else if (randomActionThink % 2 == 1)
        {
            actionThink(battlefield);
            actionLook(battlefield);
            actionMove(battlefield);
            actionFire(battlefield);
        }
    }
};
int GenericRobot::robotAutoIncrementInt_ = 0;

class Battlefield
{
private:
    int BATTLEFIELD_NUM_OF_COLS_ = -1;
    int BATTLEFIELD_NUM_OF_ROWS_ = -1;
    int turns_ = -1; // Total number of turns
    int turn = 0;    // Current turn number

    int numOfRobots_ = -1; // Number of robots
    vector<Robot *> robots_;
    queue<Robot *> destroyedRobots_;
    queue<Robot *> waitingRobots_;

    vector<vector<string>> battlefield_; // 2D vector representing the battlefield
public:
    // Getter functions
    int
    BATTLEFIELD_NUM_OF_COLS()
    {
        return BATTLEFIELD_NUM_OF_COLS_;
    }
    int BATTLEFIELD_NUM_OF_ROWS() { return BATTLEFIELD_NUM_OF_ROWS_; }
    int turns() { return turns_; }
    int numOfRobots() { return numOfRobots_; }
    // Read input file to initialize battlefield and robots
    void readFile(string filename)
    {
    }
    // Place robots on the battlefield
    void placeRobots()
    {
        for (int i = 0; i < battlefield_.size(); ++i)
        {
            for (int j = 0; j < battlefield_[i].size(); ++j)
            {
                battlefield_[i][j] = "*";
            }
        }
        for (int i = 0; i < robots_.size(); ++i)
        {
            if (robots_[i]->y() < battlefield_.size() && robots_[i]->x() < battlefield_[0].size())
            {
                battlefield_[robots_[i]->y()][robots_[i]->x()] =
                    robots_[i]->id();
            }
            else
            {
                std::cout << "Error message: Invalid location for the robot " << robots_[i]->id() << endl;
                exit(1);
            }
        }
    }
    // Display the battlefield in the screen
    void displayBattlefield() const
    {
        std::cout << "Display Battlefield";
        std::cout << endl
                  << "    ";
        for (int j = 0; j < battlefield_[0].size(); ++j)
        {
            std::cout << "  " << right << setfill('0') << setw(2) << j << " ";
        }
        std::cout << endl;
        for (int i = 0; i < battlefield_.size(); ++i)
        {
            std::cout << "    ";
            for (int j = 0; j < battlefield_[i].size(); ++j)
            {
                std::cout << "+----";
            }
            std::cout << "+" << endl;
            std::cout << "  " << right << setfill('0') << setw(2) << i;
            for (int j = 0; j < battlefield_[0].size(); ++j)
            {
                if (battlefield_[i][j] == "")
                {
                    std::cout << "|" << "    ";
                }
                else
                {
                    std::cout << "|" << left << setfill(' ') << setw(4) << battlefield_[i][j];
                }
            }
            std::cout << "|" << endl;
        }
        std::cout << "    ";
        for (int j = 0; j < battlefield_[0].size(); ++j)
        {
            std::cout << "+----";
        }
        std::cout << "+" << endl;
    }
};

void GenericRobot::actionThink(Battlefield *battlefield)
{
    std::cout << "GenericRobot actionThink" << endl;
}

void GenericRobot::actionLook(Battlefield *battlefield)
{
    std::cout << "GenericRobot actionLook" << endl;
}
void GenericRobot::actionFire(Battlefield *battlefield)
{
    std::cout << "GenericRobot actionFire" << endl;
}
void GenericRobot::actionMove(Battlefield *battlefield)
{
    std::cout << "GenericRobot actionMove" << endl;
}
int main()
{
    srand(1211109038);

    Battlefield battlefield;
    Robot *robotGenericRobot = new GenericRobot("GRO1", 4, 4);
    std::cout << *robotGenericRobot << endl;
    robotGenericRobot->actions(&battlefield);
    delete robotGenericRobot;
    robotGenericRobot = nullptr;
    return 0;
}
