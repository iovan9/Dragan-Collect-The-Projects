#include <iostream>
#include <algorithm>
#include <cstring>
#include <string>
#include <random>

// Declarare anticipata
class Dragan;
class GameManager;

class Player {
private:
    float hp = 3.0f;
    int acad = 0;
    float speed = 1.2f;
    int CoinsCollected = 0;
    Dragan* dragan = nullptr;

public:
    Player() = default;

    Player(const Player &other) :
        hp(other.hp), acad(other.acad), speed(other.speed),CoinsCollected(other.CoinsCollected), dragan(other.dragan)
    { }

    Player& operator=(const Player& other) {
        if (this != &other) {
            hp = other.hp;
            CoinsCollected = other.CoinsCollected;
            acad = other.acad;
            speed = other.speed;
            dragan = other.dragan;
        }
        return *this;
    }

    ~Player() = default;

    void setDraganRef(Dragan* d) { this->dragan = d; }

    void coinCollected() {
        CoinsCollected++;
    }

    void pReady(char input) {
        if ( input == 'Q' && CoinsCollected >= 3) {
            stunDragan();
            setCoinsCollected(-3);
        }
    }

    void acadCollected() {
        acad++;
        isWin();
    }

    void reducePlayerHp(float dmg) {
        hp -= dmg;
        isDead();
    }

    void stunDragan();

    void isDead() const {
        if (hp <= 0) {
            std::cout << "dead :(";
        }
    }

    void isWin() const {
        if (acad >= 8)
            std::cout << "YOU WIN" << std::endl;
    }

    void setHp(float x) {
        this->hp = std::min(3.0f, this->hp + x);
    }

    void setCoinsCollected(int val) {
        this->CoinsCollected += val;
    }

    [[nodiscard]] int getCoinsCollected() const {
        return CoinsCollected;
    }

    friend std::ostream& operator<<(std::ostream& os, const Player& p) {
        os << "Player [HP: " << p.hp << ", Acad Points: " << p.acad << ", Coins: " << p.CoinsCollected << "]";
        return os;
    }
};

class Dragan {

private:
    float speed = 1.0f;
    float baseDmg = 1.0f;
    float dmg = 1.0f;
    float multi = 1.0f;
    Player* player = nullptr;

    void updateDmg() {
        dmg = baseDmg * multi;
    }

public:
    Dragan() = default;

    Dragan(const Dragan &other) :
        speed(other.speed), baseDmg(other.baseDmg), dmg(other.dmg),
        multi(other.multi), player(other.player)
    { }

    Dragan& operator=(const Dragan& other) {
        if (this != &other) {
            speed = other.speed;
            baseDmg = other.baseDmg;
            dmg = other.dmg;
            multi = other.multi;
            player = other.player;
        }
        return *this;
    }

    ~Dragan() = default;

    void setPlayerRef(Player* p) { this->player = p; }

    void setDraganSpeed(float newSpeed) {
        this->speed = newSpeed;
    }

    void setMulti(const char* dif) {
        if (std::strcmp(dif, "EASY") == 0) {
            multi = 1.0f;
        } else if (std::strcmp(dif, "MEDIUM") == 0) {
            multi = 1.2f;
        } else if (std::strcmp(dif, "HARD") == 0) {
            multi = 1.5f;
        } else if (std::strcmp(dif, "THE DRAGAN DREAM") == 0) {
            multi = 3.0f;
        } else {
            multi = 1.0f;
        }
        updateDmg();
        std::cout << "Dragan setat pe " << dif << ". Dauna: " << dmg << std::endl;
    }

    void hitPlayer() const {
        if (player != nullptr) {
            player->reducePlayerHp(dmg);
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const Dragan& d) {
        os << "[Dragan] Speed: " << d.speed << ", Dmg: " << d.dmg;
        return os;
    }
};

void Player::stunDragan() {
    if (dragan != nullptr) {
        dragan->setDraganSpeed(0.0f);
        std::cout << "Dragan la pamant pentru 10 secunde (SIMULARE)!" << std::endl;
        dragan->setDraganSpeed(1.0f);
    }
}

class Bucataru
{
    float heal = 1.0f;
    Player playerCopy;

public:
    Bucataru() { setHeal(); }

    Bucataru(float initialHeal, const Player &player)
    : heal(initialHeal), playerCopy(player) {}

    Bucataru (const Bucataru& other) = default;

    Bucataru& operator=(const Bucataru& other) {
        if (this != &other) {
            this->heal = other.heal;
            this->playerCopy = other.playerCopy;
        }
        return *this;
    }

    ~Bucataru() = default;

    [[nodiscard]] float getHeal() const {
        return this->heal;
    }

    void setHeal() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<> distrib(0.0f, 1.0f);

        this->heal = static_cast<float>(distrib(gen));
        this->heal = (this->heal <= 0.75f) ? 0.5f : 1.0f;
    }

     void static HealPlayer(Player& player, Bucataru& bucataru) {
        if(player.getCoinsCollected() >= 1){
            player.setHp(bucataru.getHeal());
            player.setCoinsCollected(-1);
            std::cout << "Bucataru ti-a dat heal de " << bucataru.getHeal() << "." << std::endl;
        } else {
            std::cout << "Nu ai banuti pentru a primi heal de la Bucataru." << std::endl;
        }
        bucataru.setHeal();
    }

    friend std::ostream& operator<<(std::ostream& os, const Bucataru& bucataru) {
        os << "Bucataru [Next Heal Value: "<< bucataru.heal << "]";
        return os;
    }
};

class Flashlight {
private:
    const float MAX_BATTERY_LEVEL = 100.0f;

    float m_batteryLevel;
    float m_maxIntensity;

public:
    explicit Flashlight(float initialBattery = 100.0f, float maxIntensity = 1.0f)
    : m_batteryLevel(std::min(initialBattery, MAX_BATTERY_LEVEL)), m_maxIntensity(maxIntensity) {}

    Flashlight(const Flashlight& other)
    : m_batteryLevel(other.m_batteryLevel), m_maxIntensity(other.m_maxIntensity) {}

    Flashlight& operator=(const Flashlight& other) {
        if (this != &other) {
            m_batteryLevel = other.m_batteryLevel;
            m_maxIntensity = other.m_maxIntensity;
        }
        return *this;
    }

    ~Flashlight() = default;

    void drainBattery(float deltaTime) {
        if (m_batteryLevel > 0) {
             m_batteryLevel -= m_maxIntensity * 0.5f * deltaTime;
             m_batteryLevel = std::max(0.0f, m_batteryLevel);
        }
    }

    [[nodiscard]] float getBatteryLevel() const { return m_batteryLevel; }

    friend std::ostream& operator<<(std::ostream& os, const Flashlight& fl) {
        os << "[Baterie: " << fl.m_batteryLevel << "/" << fl.MAX_BATTERY_LEVEL;
        os << ", Intensitate: " << fl.m_maxIntensity << "]";
        return os;
    }
};

class GameManager {
private:
    std::string difficulty = "EASY";

public:
    GameManager() = default;
    GameManager(const GameManager& other) = default;
    GameManager& operator=(const GameManager& other) {
        if (this != &other) { difficulty = other.difficulty; }
        return *this;
    }
    ~GameManager() = default;

    void setDifficulty(const char* input) {
        if (std::strcmp(input, "EASY") == 0 ||
            std::strcmp(input, "MEDIUM") == 0 ||
            std::strcmp(input, "HARD") == 0 ||
            std::strcmp(input, "THE DRAGAN DREAM") == 0) {
            difficulty = input;
            } else {
                std::cout << "Dificultate invalida. Setat pe EASY." << std::endl;
                difficulty = "EASY";
            }
    }

    [[nodiscard]] std::string getDifficulty() const {
        return this->difficulty;
    }

    friend std::ostream& operator<<(std::ostream& os, const GameManager& gm) {
        os << "Game Difficulty: " << gm.difficulty;
        return os;
    }
};

int main() {

    Player player;
    Dragan dragan;

    dragan.setPlayerRef(&player);
    player.setDraganRef(&dragan);

    Flashlight flashlight(100.0f, 1.0f);
    GameManager gm;

    std::cout << "Salutare jucatorule alegeti dificultatea (EASY/MEDIUM/HARD/THE DRAGAN DREAM):\n";

    std::string difficulty_input;
    std::cin >> difficulty_input;

    const char* dif_c_str = difficulty_input.c_str();

    gm.setDifficulty(dif_c_str);
    dragan.setMulti(gm.getDifficulty().c_str());

    std::cout << gm << "\n" << dragan << "\n" << player << "\n" << flashlight << std::endl;

    player.acadCollected();
    std::cout << "\nAi colectat un proiect, grija la dragan!"<<std::endl;

    dragan.hitPlayer();
    std::cout<< "Dragan te a lovit. Starea noua a jucatorului: " << player << "\n";

    flashlight.drainBattery(15.0f);
    player.coinCollected();
    player.coinCollected();
    player.coinCollected();

    std::cout << "\nMai ai: "<< flashlight.getBatteryLevel()<<"% din baterie\n" <<
        "Ai colectat 3 banuti, power up-ul este pregatit, apasa Q pentru a-l folosi\n";

    player.pReady('Q');

    player.coinCollected();
    std::cout<<"Ai colectat 1 banut. Stare: " << player << std::endl;

    std::cout<<"\nTe intalnesti cu Bucataru, vrei sa dai un banut ca sa ti dea heal? (Y/N)\n";

    char input;
    std::cin >> input;

    if (input == 'Y' || input == 'y') {
        Bucataru bucataru;
        Bucataru::HealPlayer(player, bucataru);
        std::cout << "Bravo, ai primit heal! Stare: "<< player << std::endl << std::endl;
    }

    std::cout<<"To be continued....\n";

    return 0;
}