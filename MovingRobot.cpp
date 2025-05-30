class HideBot : public GenericRobot {                //override takeDamage()
private:
    int remainingHides = 3;
    bool hidden = false;

public:
    HideBot(string id, int x, int y, Battlefield* bf) : GenericRobot(id, x, y, bf) {}
    
    bool canHide() const {
        return remainingHides > 0 ;
    }

    void takeDamage() override {
        if(canHide()) {
            remainingHides--;
            battlefield->getLogger->log(name + " is hiding and avoid the hit (invulnerable). Hides left: " + std::to_string(remainingHides) + "\n") ;
            return ;
        } 
        else {
            lives-- ;
            battlefield->getLogger->log(name + " tried to hide but has no hides left! TAKING DAMAGE!\n") ;
        }
    }
};

class JumpBot : public GenericRobot {       //override move()
private:
    int remainingJumps = 3;

public:
    JumpBot(string id, int x, int y, Battlefield* bf) : GenericRobot(id, x, y, bf) {}

    void move(int dx, int dy) override {                 //changed from jump() to just overriding move()
        if(canJump()) {
            int newX = rand() % battlefield->getCols();       //random position inside the boundaries
            int newY = rand() % battlefield->getRows();
    
            if(battlefield->isInside(newX, newY) && !battlefield->isOccuppied(newX, newY)) {
                setLocation(newX, newY);
                remainingJumps--;
                battlefield->getLogger()->log(name + " jumped to (" + std::to_string(newX) + ", " 
                                          + std::to_string(newY) + "). Jumps left: " + std::to_string(remainingJumps) + "\n") ;
            }
            else {
                battlefield->getLogger()->log(name + " tried to jump to (" + std::to_string(newX) + "," + std::to_string(newY) 
                                          + "). Invalid Position. No Jumps consume.\n") ;
            }
        }
        else {
            int newX = posX + dx;
            int newY = posY + dy;
            battlefield->getLogger()->log(name + " tried to jump but has no jumps left! Proceed with normal movement logic\n") ;
            battlefield->getLogger()->log(name + " want to move to (" + std::to_string(newX) + "," + std::to_string(newY) + ")\n") ;
            
            if(battlefield->isInside(newX, newY) && !battlefield->isOccupied(newX, newY)) {
                setLocation(newX , newY) ;
                battlefield->getLogger()->log(name + " moves to (" + std::to_string(posX) + ", " + std::to_string(posY) + ")\n") ;
            }
            else {
                battlefield->getLogger()->log("Cannot move to (" + std::to_string(newX) + "," + std::to_string(newY) + ") : Invalid Position\n") ;
            }
        }
    }

    bool canJump() { return remainingJumps > 0 ; }
};

/*class JuggernautBot : public GenericRobot {
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
*/
