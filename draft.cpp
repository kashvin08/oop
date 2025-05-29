class MovingRobot : public GenericRobot {
public:
    MovingRobot(string id, int x, int y) : GenericRobot(id, x, y) {}

    virtual void move(int dx, int dy) {
        setLocation(x() + dx, y() + dy);
    }

    virtual void actionMove(Battlefield* battlefield) = 0;
    virtual ~MovingRobot() = default;
};

class HideBot : public MovingRobot {
private:
    int remainingHides = 3;
    bool hidden = false;

public:
    HideBot(string id, int x, int y) : MovingRobot(id, x, y) {
        setRobotType("HideBot");
        setRobotName(id + "_HideBot");
    }

    void hide() {
        if (remainingHides > 0) {
            hidden = true;
            remainingHides--;
            cout << robotName() << " is hiding (invulnerable). Hides left: " << remainingHides << endl;
        } else {
            cout << robotName() << " tried to hide but has no hides left!" << endl;
        }
    }

    bool isHidden() const {
        return hidden;
    }

    void reveal() {
        hidden = false;
    }

    void actionMove(Battlefield* battlefield) override {
        int dx = rand() % 3 - 1;
        int dy = rand() % 3 - 1;
        move(dx, dy);
        cout << robotName() << " moves to (" << x() << ", " << y() << ")" << endl;
    }

    void actions(Battlefield* battlefield) override {
        actionThink(battlefield);
        actionLook(battlefield);
        if (!isHidden()) actionFire(battlefield);
        actionMove(battlefield);
        hide();
        reveal();
    }

    void move(int dx, int dy) override {
        setLocation(x() + dx, y() + dy);
    }
};

class JumpBot : public MovingRobot {
private:
    int remainingJumps = 3;

public:
    JumpBot(string id, int x, int y) : MovingRobot(id, x, y) {
        setRobotType("JumpBot");
        setRobotName(id + "_JumpBot");
    }

    void jump(Battlefield* battlefield) {
        if (remainingJumps <= 0) {
            cout << robotName() << " tried to jump but has no jumps left!" << endl;
            return;
        }

        int newX = rand() % battlefield->BATTLEFIELD_NUM_OF_COLS();
        int newY = rand() % battlefield->BATTLEFIELD_NUM_OF_ROWS();
        setLocation(newX, newY);
        remainingJumps--;

        cout << robotName() << " jumped to (" << newX << ", " << newY << "). Jumps left: " << remainingJumps << endl;
    }

    void actionMove(Battlefield* battlefield) override {
        jump(battlefield);
    }

    void actions(Battlefield* battlefield) override {
        actionThink(battlefield);
        actionLook(battlefield);
        actionFire(battlefield);
        actionMove(battlefield);
    }

    void move(int dx, int dy) override {
        setLocation(x() + dx, y() + dy);
    }
};

class JuggernautBot : public MovingRobot {
private:
    int moveDistance = 3;

public:
    JuggernautBot(string id, int x, int y) : MovingRobot(id, x, y) {
        setRobotType("JuggernautBot");
        setRobotName(id + "_JuggernautBot");
    }

    void actionMove(Battlefield* battlefield) override {
        int direction = (rand() % 2 == 0) ? 1 : -1;
        int startY = y();
        int endY = startY + direction * moveDistance;

        endY = max(0, min(endY, battlefield->BATTLEFIELD_NUM_OF_ROWS() - 1));

        cout << robotName() << " charges vertically from y=" << startY << " to y=" << endY << endl;

        for (Robot* other : battlefield->getRobots()) {
            if (other == this || !other->isAlive()) continue;

            HideBot* hidingBot = dynamic_cast<HideBot*>(other);
            if (hidingBot && hidingBot->isHidden()) {
                cout << hidingBot->robotName() << " avoids Juggernaut charge (hidden)!" << endl;
                continue;
            }

            if (other->x() == x()) {
                int otherY = other->y();
                if ((startY < otherY && otherY <= endY) || (endY <= otherY && otherY < startY)) {
                    cout << robotName() << " hits " << other->robotName() << " at (" << x() << ", " << otherY << ")!" << endl;
                    other->reduceLife();
                }
            }
        }

        setY(endY);
    }

    void actions(Battlefield* battlefield) override {
        actionThink(battlefield);
        actionLook(battlefield);
        actionFire(battlefield);
        actionMove(battlefield);
    }

    void move(int dx, int dy) override {
        setLocation(x() + dx, y() + dy);
    }
};
