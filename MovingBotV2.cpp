class HideBot : public GenericRobot {
private:
    int remainingHides = 3;
    bool hidden = false;

public:
    HideBot(string id, int x, int y, Battlefield* bf) : GenericRobot(id, x, y, bf) {
        setRobotType("HideBot");
        setRobotName(id + "_HideBot");
    }

    bool isHidden() const { return hidden; }
    bool canHide() const { return remainingHides > 0; }

    void takeDamage() override {
        if (canHide()) {
            hidden = true; 
            remainingHides--;
            battlefield->getLogger()->log(robotName() + " is hiding and avoids the hit (invulnerable). Hides left: " + std::to_string(remainingHides) + "\n");
        } else {
            reduceLife();
            battlefield->getLogger()->log(robotName() + " tried to hide but has no hides left! TAKING DAMAGE!\n");
        }
    }

    void endTurn() {
        hidden = false; 
    }
};

class JumpBot : public GenericRobot {
private:
    int remainingJumps = 3;

public:
    JumpBot(string id, int x, int y, Battlefield* bf) : GenericRobot(id, x, y, bf) {
        setRobotType("JumpBot");
        setRobotName(id + "_JumpBot");
    }

    bool canJump() const { return remainingJumps > 0; }

    void move(int dx, int dy) override {
        if (canJump()) {
            int newX = rand() % battlefield->getCols();
            int newY = rand() % battlefield->getRows();

            if (battlefield->isInside(newX, newY) && !battlefield->isOccupied(newX, newY)) {
                setLocation(newX, newY);
                remainingJumps--;
                battlefield->getLogger()->log(robotName() + " jumped to (" +
                    std::to_string(newX) + ", " + std::to_string(newY) +
                    "). Jumps left: " + std::to_string(remainingJumps) + "\n");
            } else {
                battlefield->getLogger()->log(robotName() + " tried to jump to (" +
                    std::to_string(newX) + "," + std::to_string(newY) +
                    "). Invalid Position. No Jumps consumed.\n");
            }
        } else {
            int newX = x() + dx;
            int newY = y() + dy;
            battlefield->getLogger()->log(robotName() + " tried to jump but has no jumps left! Proceeding with normal movement.\n");

            if (battlefield->isInside(newX, newY) && !battlefield->isOccupied(newX, newY)) {
                setLocation(newX, newY);
                battlefield->getLogger()->log(robotName() + " moves to (" +
                    std::to_string(x()) + ", " + std::to_string(y()) + ")\n");
            } else {
                battlefield->getLogger()->log("Cannot move to (" +
                    std::to_string(newX) + "," + std::to_string(newY) + "): Invalid Position\n");
            }
        }
    }
};

class JuggernautBot : public GenericRobot {
private:
    int moveDistance = 3;

public:
    JuggernautBot(string id, int x, int y, Battlefield* bf) : GenericRobot(id, x, y, bf) {
        setRobotType("JuggernautBot");
        setRobotName(id + "_JuggernautBot");
    }

    void move(int dx, int dy) override {
        int direction = (rand() % 2 == 0) ? 1 : -1;
        int startY = y();
        int endY = startY + direction * moveDistance;

        endY = std::max(0, std::min(endY, battlefield->getRows() - 1));

        battlefield->getLogger()->log(robotName() + " charges vertically from y = " +
            std::to_string(startY) + " to y = " + std::to_string(endY) + "\n");

        for (Robot* other : battlefield->getRobots()) {
            if (other == this || !other->isAlive()) continue;

            // Check for HideBot invulnerability
            auto* hideBot = dynamic_cast<HideBot*>(other);
            if (hideBot && hideBot->isHidden()) {
                battlefield->getLogger()->log(hideBot->robotName() + " avoids Juggernaut charge (hidden)!\n");
                continue;
            }

            if (other->x() == x()) {
                int otherY = other->y();
                bool inPath = (startY < endY && otherY > startY && otherY <= endY) ||
                              (startY > endY && otherY < startY && otherY >= endY);

                if (inPath) {
                    battlefield->getLogger()->log(robotName() + " hits " + other->robotName() +
                        " at (" + std::to_string(x()) + ", " + std::to_string(otherY) + ")!\n");
                    other->takeDamage();
                }
            }
        }

        setY(endY);
    }
};

