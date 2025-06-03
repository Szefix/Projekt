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
     //konstruktor
    ~SnakeGame();
     //dekonstruktor
    void run();
     //glowna petla - odpowiada za uruchomienie gry

private:
    void processEvents();
     //sprawdzanie co sie dzieje i reakcja, glownie ruch weza i guziki
    void update();
     //aktualizowanie co robi waz
    void draw();
     // rysowanie / gui gry menu i wszystkiego
    void spawnFood();
     //pojawianie sie jedzenia
    void updateScoreText();
     //zaktualizuj score - obecny
    void updateHighScoreText();
     //zaktualizuj highscore
    void loadHighScore();
     //wczytaj highscore z pliku
    void saveHighScore();
     //zapisz highscore do pliku
    void resetGame();
     //reset
    bool checkCollision();
     //sprawdzanie czy waz powinien umrzec, czy w krawedz lub w siebie
    void setBoardSize(int newCols, int newRows);
     //funkcja odpowiadajaca za wybor rozmiaru planszy
    void setupButtons();
     //guziki
    void centerTextInButton(sf::Text& text, const sf::RectangleShape& button);
     //funkcja centrujaca teksty do guzikow
    int getSegmentDirection(size_t index) const;
     //funkcja odpowiadajaca za dowiedzenie sie w jaka strone idzie waz
    void loadSkinsModule();
     //funkcja odpowiadajaca za ladowanie modulu skorek
    void unloadSkinsModule(); //odladowywanie skorek
    bool isSkinUnlocked(int skinId);
     //sprawdzanie czy skorka jest odblokowana
    void loadSkinTextures(int skinId);
     //ladownaie skorek w zaleznosci od wyboru
    void loadSkinSelection();
     //zaladowanie wybranej skorki
    void saveSkinSelection();
     //zapis wyboru skorki
    void loadTextures();
     //Konfiguracja textur
    void setupSprites();
     //dodaj sprawdzanie flag przed ustawianiem tekstur


    typedef bool (*IsClassicUnlockedFunc)(int); //bool czy odblokowane
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
    //okreslenie wysokosci i dlugosci podstawowych
    int width = 800;
    int height = 650;
    static constexpr int size = 20; // rozmiar jednej kratki pikselami
    static constexpr int offsetY = 50; // offset dla top bara
    int offsetRows = offsetY / size;
    int maxRows;

    int cols = 40; //startowe kolumny
    int rows = 30; //startowe rzedy
    //skorki
    HMODULE skinsModule;
    int currentSkinId;
    int skinId;
    //highscore i score
    int highScore;
    int score;
    sf::Text highScoreText;
    sf::Text scoreText;

    bool directionChanged;
    sf::RenderWindow window;
    std::vector<SnakeSegment> snake;
    int direction;
    sf::Vector2i food;
    bool gameOver;
    sf::Font font;
    sf::Text volumeText;

    enum class GameState { // mozliwe stany gry
        Menu,
        Settings,
        BoardSizeSelection,
        Playing,  
        SkinsSelection,
        GameOver
    };
    GameState state;

    //guziki w menu glownym
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

    // guziki w ustawieniach
    sf::RectangleShape muteButton;
    sf::Text muteText;
    sf::RectangleShape backFromSettingsButton;
    sf::Text backFromSettingsText;

    bool isMuted; //sprawdzanie czy muted i definiowanie dzwiekow

    sf::SoundBuffer eatBuffer;
    sf::SoundBuffer deathBuffer;
    sf::SoundBuffer clickBuffer;

    sf::Sound eatSound;
    sf::Sound deathSound;
    sf::Sound clickSound;

    // Tekstury i sprite'y i sprawdzanie czy tekstury wogule sa tam gdzie powinny
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


    // Muzyka w tle
    sf::Music menuMusic;
    sf::Music gameMusic;
    bool musicLoaded;
};

#endif