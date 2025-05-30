class HideBot : public GenericRobot {
private:
    int remainingHides = 3;
    bool hidden = false;

public:
    HideBot(string id, int x, int y, Battlefield* bf)
        : GenericRobot(id, x, y, bf) {
        setRobotType("HideBot");
        setRobotName(id + "_HideBot");
    }

    bool isHidden() const { return hidden; }
    bool canHide() const { return remainingHides > 0; }

    void takeDamage() override {
        if (canHide()) {
            hidden = true; // Set hidden only when damage avoided
            remainingHides--;
            battlefield->getLogger()->log(robotName() + " is hiding and avoids the hit (invulnerable). Hides left: " + std::to_string(remainingHides) + "\n");
        } else {
            reduceLife();
            battlefield->getLogger()->log(robotName() + " tried to hide but has no hides left! TAKING DAMAGE!\n");
        }
    }

    void endTurn() {
        hidden = false; // reveal after hiding
    }
};
