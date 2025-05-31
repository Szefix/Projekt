#ifndef SNAKEGAME_HPP
#define SNAKEGAME_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <iostream>
#include <windows.h>
#include "SnakeSegment.hpp"

class SnakeGame {
public:
    SnakeGame();
    ~SnakeGame();
    void run();

private:
    void processEvents();
    void update();
    void draw();
    void spawnFood();
    void updateScoreText();
    void updateHighScoreText();
    void loadHighScore();
    void saveHighScore();
    void resetGame();
    bool checkCollision();
    void setBoardSize(int newCols, int newRows);
    void setupButtons();
    void centerTextInButton(sf::Text& text, const sf::RectangleShape& button);
    int getSegmentDirection(size_t index) const;
    void loadSkinsModule();
    void unloadSkinsModule();
    bool isSkinUnlocked(int skinId);
    void loadSkinTextures(int skinId);
    
    // Nowe funkcje do obsługi tekstur
    void loadTextures();
    void setupSprites();

    void setupSkinsSelectionButtons();
    void drawSkinsSelection();


    typedef bool (*IsClassicUnlockedFunc)(int);
    typedef bool (*IsGoldenUnlockedFunc)(int);
    typedef bool (*IsRainbowUnlockedFunc)(int);
    typedef bool (*IsLegendaryUnlockedFunc)(int);
    typedef const char* (*GetSkinNameFunc)(int);
    typedef int (*GetRequiredScoreFunc)(int);

    IsClassicUnlockedFunc isClassicUnlocked;
    IsGoldenUnlockedFunc isGoldenUnlocked;
    IsRainbowUnlockedFunc isRainbowUnlocked;
    IsLegendaryUnlockedFunc isLegendaryUnlocked;
    GetSkinNameFunc getSkinName;
    GetRequiredScoreFunc getRequiredScore;

private:
    int width = 800;
    int height = 650;
    static constexpr int size = 20;
    static constexpr int offsetY = 50;
    int offsetRows = offsetY / size;
    int maxRows;

    int cols = 40;
    int rows = 30;

    HMODULE skinsModule;
    int currentSkinId;
    
    int highScore;
    sf::Text highScoreText;
    bool directionChanged;
    sf::RenderWindow window;
    std::vector<SnakeSegment> snake;
    int direction;
    sf::Vector2i food;
    bool gameOver;
    int score;
    sf::Font font;
    sf::Text scoreText;
    sf::Text volumeText;

    enum class GameState {
        Menu,
        Settings,
        BoardSizeSelection,
        Playing,  
        SkinsSelection,
        GameOver
    };
    GameState state;

    sf::RectangleShape startButton;
    sf::Text startText;
    sf::RectangleShape settingsButton;
    sf::Text settingsText;
    sf::RectangleShape exitButton;
    sf::Text exitText;
    sf::RectangleShape restartButton;
    sf::Text restartText;
    sf::RectangleShape backToMenuButton;
    sf::Text backToMenuText;
    sf::RectangleShape smallBoardButton, mediumBoardButton, largeBoardButton;
    sf::Text smallBoardText, mediumBoardText, largeBoardText;

    // Settings elements
    sf::RectangleShape muteButton;
    sf::Text muteText;
    sf::RectangleShape backFromSettingsButton;
    sf::Text backFromSettingsText;

    bool isMuted;

    sf::SoundBuffer eatBuffer;
    sf::SoundBuffer deathBuffer;
    sf::SoundBuffer clickBuffer;

    sf::Sound eatSound;
    sf::Sound deathSound;
    sf::Sound clickSound;

    // Tekstury i sprite'y
    sf::Texture topBarTexture;
    sf::Texture buttonTexture;
    sf::Texture foodTexture;
    
    sf::Sprite topBarSprite;
    sf::Sprite foodSprite;

    bool hasTopBarTexture;
    bool hasButtonTexture;
    bool hasFoodTexture;


    sf::Texture snakeHeadUpTexture, snakeHeadDownTexture, snakeHeadLeftTexture, snakeHeadRightTexture;
    sf::Texture snakeBodyHorizontalTexture, snakeBodyVerticalTexture;
    sf::Texture snakeCornerUpLeftTexture, snakeCornerUpRightTexture, snakeCornerDownLeftTexture, snakeCornerDownRightTexture;

    sf::Sprite snakeHeadUpSprite, snakeHeadDownSprite, snakeHeadLeftSprite, snakeHeadRightSprite;
    sf::Sprite snakeBodyHorizontalSprite, snakeBodyVerticalSprite;
    sf::Sprite snakeCornerUpLeftSprite, snakeCornerUpRightSprite, snakeCornerDownLeftSprite, snakeCornerDownRightSprite;

    bool hasSnakeHeadTextures, hasSnakeBodyTextures, hasSnakeCornerTextures;

    sf::Texture gridCellTexture;
    sf::Sprite gridCellSprite;
    bool hasGridCellTexture;

    sf::RectangleShape skinsButton;
    sf::Text skinsText;
    sf::RectangleShape skin1Button, skin2Button, skin3Button, skin4Button;
    sf::Text skin1Text, skin2Text, skin3Text, skin4Text;
    sf::RectangleShape backFromSkinsButton;
    sf::Text backFromSkinsText;

};

#endif