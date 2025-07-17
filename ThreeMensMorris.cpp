/*
Three Men's Morris - A Classic Board Game
Developed using C++ and SFML

Author : Sasivarnasarma
Created : July 2025
License : MIT License
GitHub : https://github.com/Sasivarnasarma/ThreeMensMorris

Description :
A two-player strategy game implemented using SFML. Includes placement and
movement phases, with win detection logic and a clean UI.

Dependencies:
- SFML 2.6

Notes:
- See README.md for build and gameplay instructions

© 2025 Sasivarnasarma. Licensed under MIT
Version 1.0.0
*/

#include <array>
#include <cmath>
#include <ctime>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <unordered_map>
#include <SFML/Graphics.hpp>

// Slot structure to represent each position on the grid
// Each slot has a position and a unique prime number associated with it.
// We use primes to detect winning combinations by multiplying the primes of selected slots.
// | --------- |
// | 0 | 1 | 2 |
// | --------- |
// | 3 | 4 | 5 |
// | --------- |
// | 6 | 7 | 8 |
// | --------- |

std::ofstream logFile("game.log");


// Slot structure to represent each position on the grid
struct Slot {
    sf::Vector2f position; // Position of the slot in the window
    int prime; // Unique prime number for this slot, used for win detection
};

// Each slot has a unique prime number used for efficient win detection by multiplying selected slot primes.
std::array<Slot, 9> slots = {{
    {{50,  50}, 13}, {{300,  50},  3}, {{550,  50}, 23},
    {{50, 300}, 17}, {{300, 300}, 11}, {{550, 300},  5},
    {{50, 550}, 29}, {{300, 550},  7}, {{550, 550}, 19}
}};

// Winning combinations are defined by indices of slots
std::array<std::array<int, 3>, 8> winCombos = {{
    {{0,1,2}}, {{3,4,5}}, {{6,7,8}}, // Horizontal
    {{0,3,6}}, {{1,4,7}}, {{2,5,8}}, // Vertical
    {{0,4,8}}, {{2,4,6}} // Diagonal
}};

enum class Player { A, B }; // Enum to represent players
enum class GamePhase { Start, About, Instructions, Placement, Movement, Win }; // Enum to represent game phases

// Token structure to represent each player's token
struct Token {
    sf::Sprite sprite;       // Visual representation of the token (from SFML)
    int slotIndex = -1;      // Current slot index where this token is placed (-1 if not placed)
    int nextSlotIndex = -1;  // The next slot index where the token will move (-1 if no move scheduled)
    Player owner;            // Which player owns this token (Player::A or Player::B)
    bool selected = false;   // Is this token currently selected by the player?
    bool moving = false;     // Is the token currently in motion (moving to targetPos)?
    sf::Vector2f targetPos;  // The position the token is moving towards
};

std::unordered_map<std::string, sf::Sprite> spritesMap; // Map to hold sprites by name
sf::FloatRect startButtonBounds(165, 440, 275, 100); // Bounds for the start button
sf::FloatRect instructionsButtonBounds(165, 560, 275, 100); // Bounds for the instructions button
sf::FloatRect aboutButtonBounds(165, 680, 275, 100); // Bounds for the about button
sf::FloatRect exitButtonBounds(165, 650, 275, 100); // Bounds for the exit button

// Function to log messages to a file with timestamps
void log(const std::string& message) { 
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    logFile << "[" << std::put_time(localTime, "%a %b %d %H:%M:%S %Y") << "] - " << message << std::endl;
}

// Function to load assets (textures, images)
bool loadAssets(
    sf::Image& icon,
    sf::Texture& start,
    sf::Texture& instructions,
    sf::Texture& about,
    sf::Texture& board,
    sf::Texture& a,
    sf::Texture& b,
    sf::Texture& active,
    sf::Texture& papt,
    sf::Texture& pbpt,
    sf::Texture& pamt,
    sf::Texture& pbmt,
    sf::Texture& winA,
    sf::Texture& winB)
{
    if (!icon.loadFromFile("assets/icon.png") ||
        !start.loadFromFile("assets/start.png") ||
        !instructions.loadFromFile("assets/instructions.png") ||
        !about.loadFromFile("assets/about.png") ||
        !board.loadFromFile("assets/board.png") ||
        !a.loadFromFile("assets/token_a.png") ||
        !b.loadFromFile("assets/token_b.png") ||
        !active.loadFromFile("assets/active.png") ||
        !papt.loadFromFile("assets/papt.png") ||
        !pbpt.loadFromFile("assets/pbpt.png") ||
        !pamt.loadFromFile("assets/pamt.png") ||
        !pbmt.loadFromFile("assets/pbmt.png") ||
        !winA.loadFromFile("assets/win_a.png") ||
        !winB.loadFromFile("assets/win_b.png"))
    {
        log("Failed to load assets.\n");
        return false;
    }
    return true;
}

// Returns true if slot 'b' is directly adjacent to slot 'a' on the grid.
bool isAdjacent(int a, int b) {
    static const std::array<std::vector<int>, 9> adjacent = {{
        {1, 3, 4},       // 0 connects to right, down, diagonal (center)
        {0, 2, 4},       // 1 connects left, right, center
        {1, 5, 4},       // 2 connects left, down, diagonal
        {0, 4, 6},       // 3 connects up, right, down
        {0, 1, 2, 3, 5, 6, 7, 8}, // 4 (center) connects to all adjacent tiles
        {2, 4, 8},       // 5 connects left, center, down
        {3, 4, 7},       // 6 connects up, center, right
        {6, 4, 8},       // 7 connects left, center, right
        {4, 5, 7}        // 8 connects up, left, center
    }};
    for (int n : adjacent[a]) {
        if (n == b) return true;
    }
    return false;
}

// Computes the product of primes for a given winning combination
std::vector<int> computeWinProducts(
    const std::array<std::array<int, 3>, 8>& combos,
    const std::array<Slot, 9>& slots)
{
    std::vector<int> products;
    for (const auto& combo : combos) {
        int p = slots[combo[0]].prime * slots[combo[1]].prime * slots[combo[2]].prime;
        products.push_back(p);
    }
    return products;
}

// Checks if the current tokens of a player form a winning combination
bool checkWin(
    const std::vector<Token>& tokens,
    Player player,
    const std::vector<int>& winProducts)
{
    int product = 1;
    for (const auto& t : tokens)
        if (t.owner == player && t.slotIndex != -1)
            product *= slots[t.slotIndex].prime;

    for (int p : winProducts) {
        if (product % p == 0)
            return true;
    }
    return false;
}

// Returns the index of the unoccupied slot under the mouse, or -1 if none.
int getFreeSlotUnderMouse(const sf::Vector2f& pos, const std::vector<Token>& tokens) {
    for (int i = 0; i < slots.size(); ++i) {
        sf::FloatRect area(slots[i].position - sf::Vector2f(25,25), {50,50});
        bool occupied = false;
        for (const auto& t : tokens)
            if (t.slotIndex == i || (t.moving && t.nextSlotIndex == i))
                occupied = true;
        if (!occupied && area.contains(pos))
            return i;
    }
    return -1;
}

// Resets the game state
void resetGame(
    std::vector<Token>& tokens,
    int& placedA, int& placedB,
    Player& turn,
    GamePhase& phase,
    Token*& selected)
{
    if (phase == GamePhase::Win) {
        phase = GamePhase::Start;
        startButtonBounds = sf::FloatRect(165, 440, 275, 100);
    } else {
        phase = GamePhase::Placement;
        startButtonBounds = sf::FloatRect(340, 620, 240, 80);
        log("Game reset.");
    }
    tokens.clear();
    placedA = placedB = 0;
    turn = Player::A;
    selected = nullptr;
    spritesMap["currentPTI"] = spritesMap["papt"];
}

// Handles user input events
// Updates game state based on mouse clicks and key presses
void handleEvents(
    sf::RenderWindow& window,
    std::vector<Token>& tokens,
    Player& turn,
    GamePhase& phase,
    Token*& selected,
    int& placedA,
    int& placedB,
    const sf::Texture& tokenATex,
    const sf::Texture& tokenBTex,
    const std::vector<int>& winProducts)
{
    sf::Event ev;
    while (window.pollEvent(ev)) {
        if (ev.type == sf::Event::Closed) {
            log("Game closed by user.\n");
            window.close();
        }

        if (ev.type == sf::Event::MouseButtonPressed && ev.mouseButton.button == sf::Mouse::Left) {

            sf::Vector2f mousePos(ev.mouseButton.x, ev.mouseButton.y);

            if ((phase == GamePhase::Instructions || phase == GamePhase::About) && startButtonBounds.contains(mousePos)) {
                phase = GamePhase::Start;
                startButtonBounds = sf::FloatRect(165, 440, 275, 100);
                return;
            }

            if (phase == GamePhase::Start && startButtonBounds.contains(mousePos)) {
                phase = GamePhase::Placement;
                startButtonBounds = sf::FloatRect(340, 620, 240, 80);
                log("Game started.");
                return;
            }

            if (startButtonBounds.contains(mousePos)) {
                resetGame(tokens, placedA, placedB, turn, phase, selected);
                return;
            }

            if (phase == GamePhase::Start && instructionsButtonBounds.contains(mousePos)) {
                phase = GamePhase::Instructions;
                startButtonBounds = sf::FloatRect(165, 665, 275, 100);
                return;
            }

            if (phase == GamePhase::Start && aboutButtonBounds.contains(mousePos)) {
                phase = GamePhase::About;
                startButtonBounds = sf::FloatRect(165, 665, 275, 100);
                return;
            }

            if (phase == GamePhase::Win && exitButtonBounds.contains(mousePos)) {
                window.close();
                log("Game exited.\n");
                return;
            }

            if (phase == GamePhase::Placement) {
                int slot = getFreeSlotUnderMouse(mousePos, tokens);
                if (slot != -1) {
                    Token t;
                    t.owner = turn;
                    t.slotIndex = slot;
                    t.sprite.setTexture(turn == Player::A ? tokenATex : tokenBTex);
                    t.sprite.setPosition(slots[slot].position - sf::Vector2f(25, 25));
                    tokens.push_back(t);

                    spritesMap["currentPTI"] = spritesMap[turn == Player::A ? "pbpt" : "papt"];

                    if (checkWin(tokens, turn, winProducts)) {
                        log((turn == Player::A ? "Player A" : "Player B") + std::string(" wins!"));
                        spritesMap["winner"] = spritesMap[turn == Player::A ? "winA" : "winB"];
                        phase = GamePhase::Win;
                        startButtonBounds = sf::FloatRect(165, 510, 275, 100);
                        return;
                    }

                    (turn == Player::A ? placedA : placedB)++;
                    if (placedA == 3 && placedB == 3){
                        phase = GamePhase::Movement;
                        spritesMap["currentPTI"] = spritesMap["pamt"];
                    }

                    turn = (turn == Player::A ? Player::B : Player::A);
                }
            }
            else if (phase == GamePhase::Movement) {
                bool clicked = false;
                for (auto& t : tokens) {
                    if (t.owner == turn && t.sprite.getGlobalBounds().contains(mousePos)) {
                        if (selected) selected->selected = false;
                        t.selected = true;
                        selected = &t;
                        clicked = true;
                        break;
                    }
                }

                if (!clicked && selected && !selected->moving) {
                    int target = getFreeSlotUnderMouse(mousePos, tokens);
                    if (target != -1 && isAdjacent(selected->slotIndex, target)) {
                        spritesMap["currentPTI"] = spritesMap[turn == Player::A ? "pbmt" : "pamt"];
                        selected->moving = true;
                        selected->targetPos = slots[target].position - sf::Vector2f(25, 25);
                        selected->nextSlotIndex = target;
                        selected->selected = false;
                    }
                }
            }
        }
    }
}

// Updates the positions of tokens that are currently moving
// If a token reaches its target position, it updates its slot index and checks for a win
void updateTokens(
    std::vector<Token>& tokens,
    float dt,
    float speed,
    Player& turn,
    Token*& selected,
    sf::RenderWindow& window,
    const std::vector<int>& winProducts,
    GamePhase& phase)
{
    for (auto& t : tokens) {
        if (t.moving) {
            sf::Vector2f dir = t.targetPos - t.sprite.getPosition();
            float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
            if (dist < speed * dt) {
                t.sprite.setPosition(t.targetPos);
                t.slotIndex = t.nextSlotIndex;
                t.moving = false;
                t.nextSlotIndex = -1;

                if (checkWin(tokens, t.owner, winProducts)) {
                    log((turn == Player::A ? "Player A" : "Player B") + std::string(" wins!"));
                    spritesMap["winner"] = spritesMap[turn == Player::A ? "winA" : "winB"];
                    phase = GamePhase::Win;
                    startButtonBounds = sf::FloatRect(165, 510, 275, 100);
                }

                selected = nullptr;
                turn = (turn == Player::A ? Player::B : Player::A);
            } else {
                sf::Vector2f step = dir / dist * speed * dt;
                t.sprite.move(step);
            }
        }
    }
}

// Draws the current game state to the window
void drawGame(
    sf::RenderWindow& window,
    const std::vector<Token>& tokens,
    const sf::Texture& activeTex,
    const GamePhase& phase)
{
    window.clear(sf::Color::White);
    if (phase == GamePhase::Start) {
        spritesMap["start"].setPosition(0, 0);
        window.draw(spritesMap["start"]);
    
    } else if (phase == GamePhase::Instructions) {
        spritesMap["instructions"].setPosition(0, 0);
        window.draw(spritesMap["instructions"]);
    
    } else if (phase == GamePhase::About) {
        spritesMap["about"].setPosition(0, 0);
        window.draw(spritesMap["about"]);

    } else if (phase == GamePhase::Win) {
        spritesMap["winner"].setPosition(0, 0);
        window.draw(spritesMap["winner"]);

    } else if (phase == GamePhase::Placement || phase == GamePhase::Movement) {

        spritesMap["board"].setPosition(0, 0);
        window.draw(spritesMap["board"]);
        spritesMap["currentPTI"].setPosition(0, 600);
        window.draw(spritesMap["currentPTI"]);

        for (const auto& t : tokens) {
            window.draw(t.sprite);
            if (t.selected) {
                sf::Sprite overlay(activeTex);
                overlay.setPosition(t.sprite.getPosition());
                window.draw(overlay);
            }
        }
    }
    window.display();
}


// Main function to initialize the game, load assets, and run the game loop
int main() {
    log("Game started.");
    sf::RenderWindow window(sf::VideoMode(600, 800), "Three Men's Morris", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    sf::Image icon;
    sf::Texture startTex, instructionsTex, aboutTex, boardTex, tokenATex, tokenBTex, activeTex, paptTex, pbptTex, pamtTex, pbmtTex, winATex, winBTex;
    if (!loadAssets(icon, startTex, instructionsTex, aboutTex, boardTex, tokenATex, tokenBTex, activeTex, paptTex, pbptTex, pamtTex, pbmtTex, winATex, winBTex)) {
        return 1;
    }
    
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    spritesMap["start"] = sf::Sprite(startTex);
    spritesMap["instructions"] = sf::Sprite(instructionsTex);
    spritesMap["about"] = sf::Sprite(aboutTex);
    spritesMap["board"] = sf::Sprite(boardTex);
    spritesMap["currentPTI"] = sf::Sprite(paptTex);
    spritesMap["papt"] = sf::Sprite(paptTex);
    spritesMap["pbpt"] = sf::Sprite(pbptTex);
    spritesMap["pamt"] = sf::Sprite(pamtTex);
    spritesMap["pbmt"] = sf::Sprite(pbmtTex);
    spritesMap["winA"] = sf::Sprite(winATex);
    spritesMap["winB"] = sf::Sprite(winBTex);
    spritesMap["winner"] = sf::Sprite();

    std::vector<Token> tokens;
    Token* selected = nullptr;
    Player turn = Player::A;
    GamePhase phase = GamePhase::Start;
    int placedA = 0, placedB = 0;

    std::vector<int> winProducts = computeWinProducts(winCombos, slots);

    const float speed = 400.f;
    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        handleEvents(window, tokens, turn, phase, selected, placedA, placedB, tokenATex, tokenBTex, winProducts);
        updateTokens(tokens, dt, speed, turn, selected, window, winProducts, phase);
        drawGame(window, tokens, activeTex, phase);
    }
    logFile.close();
    return 0;
}