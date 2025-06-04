#include "SnakeGame.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <string>
#include <fstream>
#include <stdexcept>

SnakeGame::SnakeGame() : window(sf::VideoMode(width, height), "Snake"), direction(1), gameOver(false) {
    
      try {
        snake.push_back(SnakeSegment(10, 10));
        spawnFood();
        
        if (!font.loadFromFile("arial.ttf")) {
            throw std::runtime_error("Nie można załadować czcionki arial.ttf");
        }
        
        loadTextures();
        setupSprites();
        setupButtons();
        
        // Dźwięki
        if (!eatBuffer.loadFromFile("sounds/eat.wav"))
            throw std::runtime_error("Nie można załadować eat.wav");
        if (!deathBuffer.loadFromFile("sounds/death.wav"))
            throw std::runtime_error("Nie można załadować death.wav");
        if (!clickBuffer.loadFromFile("sounds/click.wav"))
            throw std::runtime_error("Nie można załadować click.wav");
        
    } catch (const std::exception& e) {
        std::cerr << "Błąd inicjalizacji gry: " << e.what() << std::endl;
    }

    
    //score
    score = 0;
   
    //skin loading
    skinId = 0;
    currentSkinId = 0;
    loadSkinsModule();

    //zmienne potrzebne do plansz
    maxRows = (height - offsetY) / size;
    cols = width / size;
    rows = maxRows;

    //highscore
    highScore = 0;
    loadHighScore();

    //usprawnienia
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    //dzwieki 
    eatSound.setBuffer(eatBuffer);
    deathSound.setBuffer(deathBuffer);
    clickSound.setBuffer(clickBuffer);

    // Muzyka w tle
    musicLoaded = false;
    if (menuMusic.openFromFile("sounds/menu_music.ogg") && 
        gameMusic.openFromFile("sounds/game_music.ogg")) {
        musicLoaded = true;
        menuMusic.setLoop(true);
        gameMusic.setLoop(true);
        menuMusic.setVolume(30); // Cichsza muzyka żeby nie zagłuszała efektów
        gameMusic.setVolume(30);
        menuMusic.play(); // Zacznij od muzyki menu
    }

    // teksty od high score -> exit i restart
    highScoreText.setFont(font);
    highScoreText.setCharacterSize(24);
    highScoreText.setFillColor(sf::Color::Yellow);
    highScoreText.setPosition(200, 10);
    updateHighScoreText();

    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);
    updateScoreText();

    state = GameState::Menu;

    // Settings initialization
    isMuted = false;
    eatSound.setVolume(100); // 100% głośności domyślnie
    deathSound.setVolume(100);
    clickSound.setVolume(100);

    //Załaduj tekstury i ustaw sprite'y
    loadTextures();
    setupSprites();
    setupButtons();
}


//Konfiguracja textur
void SnakeGame::loadTextures() {
    // Reset flags
    hasGridCellTexture = false;
    hasTopBarTexture = false;
    hasButtonTexture = false;
    hasSnakeHeadTextures = false;
    hasSnakeBodyTextures = false;
    hasSnakeCornerTextures = false;
    hasFoodTexture = false;

    // Zaladuj basicowe tekstury
    hasGridCellTexture = gridCellTexture.loadFromFile("textures/grid_cell.png");
    hasTopBarTexture = topBarTexture.loadFromFile("textures/topbar.png");
    hasButtonTexture = buttonTexture.loadFromFile("textures/button.png");
    hasFoodTexture = foodTexture.loadFromFile("textures/food.png");

    // Zaladuje tekstury wensza
    hasSnakeHeadTextures = 
        snakeHeadUpTexture.loadFromFile("textures/skins/skin0/snake_head_up.png") &&
        snakeHeadDownTexture.loadFromFile("textures/skins/skin0/snake_head_down.png") &&
        snakeHeadLeftTexture.loadFromFile("textures/skins/skin0/snake_head_left.png") &&
        snakeHeadRightTexture.loadFromFile("textures/skins/skin0/snake_head_right.png");

    hasSnakeBodyTextures =
        snakeBodyHorizontalTexture.loadFromFile("textures/skins/skin0/snake_body_horizontal.png") &&
        snakeBodyVerticalTexture.loadFromFile("textures/skins/skin0/snake_body_vertical.png");

    hasSnakeCornerTextures =
        snakeCornerUpLeftTexture.loadFromFile("textures/skins/skin0/snake_corner_up_left.png") &&
        snakeCornerUpRightTexture.loadFromFile("textures/skins/skin0/snake_corner_up_right.png") &&
        snakeCornerDownLeftTexture.loadFromFile("textures/skins/skin0/snake_corner_down_left.png") &&
        snakeCornerDownRightTexture.loadFromFile("textures/skins/skin0/snake_corner_down_right.png");
}

//dodaj sprawdzanie flag przed ustawianiem tekstur
void SnakeGame::setupSprites() {
    if (hasGridCellTexture) {
        gridCellSprite.setTexture(gridCellTexture);
        gridCellSprite.setScale(
            static_cast<float>(size) / gridCellTexture.getSize().x,
            static_cast<float>(size) / gridCellTexture.getSize().y
        );
    }

    if (hasTopBarTexture) {
        topBarSprite.setTexture(topBarTexture);
        sf::Vector2u textureSize = topBarTexture.getSize();
        float scaleX = static_cast<float>(width) / textureSize.x;
        float scaleY = static_cast<float>(offsetY) / textureSize.y;
        topBarSprite.setScale(scaleX, scaleY);
        topBarSprite.setPosition(0, 0);
    }

    if (hasFoodTexture) {
        foodSprite.setTexture(foodTexture);
        foodSprite.setScale(
            static_cast<float>(size - 2) / foodTexture.getSize().x,
            static_cast<float>(size - 2) / foodTexture.getSize().y
        );
    }

    // Snake sprites
    if (hasSnakeHeadTextures) {
        snakeHeadUpSprite.setTexture(snakeHeadUpTexture);
        snakeHeadDownSprite.setTexture(snakeHeadDownTexture);
        snakeHeadLeftSprite.setTexture(snakeHeadLeftTexture);
        snakeHeadRightSprite.setTexture(snakeHeadRightTexture);
        
        float scale = static_cast<float>(size - 2) / snakeHeadUpTexture.getSize().x;
        snakeHeadUpSprite.setScale(scale, scale);
        snakeHeadDownSprite.setScale(scale, scale);
        snakeHeadLeftSprite.setScale(scale, scale);
        snakeHeadRightSprite.setScale(scale, scale);
    }

    if (hasSnakeBodyTextures) {
        snakeBodyHorizontalSprite.setTexture(snakeBodyHorizontalTexture);
        snakeBodyVerticalSprite.setTexture(snakeBodyVerticalTexture);
        
        float scale = static_cast<float>(size - 2) / snakeBodyHorizontalTexture.getSize().x;
        snakeBodyHorizontalSprite.setScale(scale, scale);
        snakeBodyVerticalSprite.setScale(scale, scale);
    }

    if (hasSnakeCornerTextures) {
        snakeCornerUpLeftSprite.setTexture(snakeCornerUpLeftTexture);
        snakeCornerUpRightSprite.setTexture(snakeCornerUpRightTexture);
        snakeCornerDownLeftSprite.setTexture(snakeCornerDownLeftTexture);
        snakeCornerDownRightSprite.setTexture(snakeCornerDownRightTexture);
        
        float scale = static_cast<float>(size - 2) / snakeCornerUpLeftTexture.getSize().x;
        snakeCornerUpLeftSprite.setScale(scale, scale);
        snakeCornerUpRightSprite.setScale(scale, scale);
        snakeCornerDownLeftSprite.setScale(scale, scale);
        snakeCornerDownRightSprite.setScale(scale, scale);
    }
}

//guziki
void SnakeGame::setupButtons() {
    // Oblicz środek okna
    float centerX = width / 2.0f;
    float centerY = height / 2.0f;
    
    // Menu główne - 4 przyciski (Start, Settings, Skins, Exit)
    float buttonSpacing = 65.0f; // Odstęp między przyciskami
    float totalHeight = 3 * buttonSpacing; // Wysokość dla 4 przycisków
    float startY = centerY - (totalHeight / 2.0f); 
    
    //startbutton
    startButton.setSize({200, 50});
    if (hasButtonTexture) {
        startButton.setTexture(&buttonTexture);
    } else {
        startButton.setFillColor(sf::Color::Blue);
    }
    startButton.setPosition(centerX - 100, startY);
    startText.setFont(font);
    startText.setCharacterSize(24);
    startText.setString("Zacznij gre");
    startText.setFillColor(sf::Color::White);
    centerTextInButton(startText, startButton);

    //settingsbutton
    settingsButton.setSize({200, 50});
    if (hasButtonTexture) {
        settingsButton.setTexture(&buttonTexture);
    } else {
        settingsButton.setFillColor(sf::Color::Blue);
    }
    settingsButton.setPosition(centerX - 100, startY + buttonSpacing);
    settingsText.setFont(font);
    settingsText.setCharacterSize(24);
    settingsText.setString("Ustawienia");
    settingsText.setFillColor(sf::Color::White);
    centerTextInButton(settingsText, settingsButton);

    //skinsbutton
    skinsButton.setSize({200, 50});
    if (hasButtonTexture) {
        skinsButton.setTexture(&buttonTexture);
    } else {
        skinsButton.setFillColor(sf::Color::Blue);
    }
    skinsButton.setPosition(centerX - 100, startY + 2 * buttonSpacing);
    skinsText.setFont(font);
    skinsText.setCharacterSize(24);
    skinsText.setString("Skorki");
    skinsText.setFillColor(sf::Color::White);
    centerTextInButton(skinsText, skinsButton);

    //exitbutton
    exitButton.setSize({200, 50});
    if (hasButtonTexture) {
        exitButton.setTexture(&buttonTexture);
    } else {
        exitButton.setFillColor(sf::Color::Blue);
    }
    exitButton.setPosition(centerX - 100, startY + 3 * buttonSpacing);
    exitText.setFont(font);
    exitText.setCharacterSize(24);
    exitText.setString("Wyjdz");
    exitText.setFillColor(sf::Color::White);
    centerTextInButton(exitText, exitButton);

    // Menu ustawień - 3 przyciski (Mute, Skins, Back)
    float settingsStartY = centerY - buttonSpacing;
    
    //mutebutton
    muteButton.setSize({200, 50});
    if (hasButtonTexture) {
        muteButton.setTexture(&buttonTexture);
    } else {
        muteButton.setFillColor(sf::Color::Blue);
    }
    muteButton.setPosition(centerX - 100, settingsStartY);
    muteText.setFont(font);
    muteText.setCharacterSize(24);
    muteText.setFillColor(sf::Color::White);
    muteText.setString(isMuted ? "Dzwiek: OFF" : "Dzwiek: ON");
    centerTextInButton(muteText, muteButton);

    //back from settings button
    backFromSettingsButton.setSize({200, 50});
    if (hasButtonTexture) {
        backFromSettingsButton.setTexture(&buttonTexture);
    } else {
        backFromSettingsButton.setFillColor(sf::Color::Blue);
    }
    backFromSettingsButton.setPosition(centerX - 100, settingsStartY + buttonSpacing);
    backFromSettingsText.setFont(font);
    backFromSettingsText.setCharacterSize(24);
    backFromSettingsText.setString("Powrot");
    backFromSettingsText.setFillColor(sf::Color::White);
    centerTextInButton(backFromSettingsText, backFromSettingsButton);

    //restartbutton
    restartButton.setSize({200, 50});
    if (hasButtonTexture) {
        restartButton.setTexture(&buttonTexture);
    } else {
        restartButton.setFillColor(sf::Color::Blue);
    }
    restartButton.setPosition(centerX - 100, centerY - 50);
    restartText.setFont(font);
    restartText.setCharacterSize(24);
    restartText.setString("Restart");
    restartText.setFillColor(sf::Color::White);
    centerTextInButton(restartText, restartButton);

    //backtomenubutton
    backToMenuButton.setSize({200, 50});
    if (hasButtonTexture) {
        backToMenuButton.setTexture(&buttonTexture);
    } else {
        backToMenuButton.setFillColor(sf::Color::Blue);
    }
    backToMenuButton.setPosition(centerX - 100, centerY + 20);
    backToMenuText.setFont(font);
    backToMenuText.setCharacterSize(24);
    backToMenuText.setString("Menu");
    backToMenuText.setFillColor(sf::Color::White);
    centerTextInButton(backToMenuText, backToMenuButton);

    //smallboardbutton
    smallBoardButton.setSize({200, 50});
    if (hasButtonTexture) {
        smallBoardButton.setTexture(&buttonTexture);
    } else {
        smallBoardButton.setFillColor(sf::Color::Blue);
    }
    smallBoardButton.setPosition(centerX - 100, centerY - 100);
    smallBoardText.setFont(font);
    smallBoardText.setCharacterSize(24);
    smallBoardText.setString("Small");
    smallBoardText.setFillColor(sf::Color::White);
    centerTextInButton(smallBoardText, smallBoardButton);

    //mediumboardbutton
    mediumBoardButton.setSize({200, 50});
    if (hasButtonTexture) {
        mediumBoardButton.setTexture(&buttonTexture);
    } else {
        mediumBoardButton.setFillColor(sf::Color::Blue);
    }
    mediumBoardButton.setPosition(centerX - 100, centerY - 25);
    mediumBoardText.setFont(font);
    mediumBoardText.setCharacterSize(24);
    mediumBoardText.setString("Medium");
    mediumBoardText.setFillColor(sf::Color::White);
    centerTextInButton(mediumBoardText, mediumBoardButton);

    //largeboardbutton
    largeBoardButton.setSize({200, 50});
    if (hasButtonTexture) {
        largeBoardButton.setTexture(&buttonTexture);
    } else {
        largeBoardButton.setFillColor(sf::Color::Blue);
    }
    largeBoardButton.setPosition(centerX - 100, centerY + 50);
    largeBoardText.setFont(font);
    largeBoardText.setCharacterSize(24);
    largeBoardText.setString("Large");
    largeBoardText.setFillColor(sf::Color::White);
    centerTextInButton(largeBoardText, largeBoardButton);

    //volume text
    volumeText.setFont(font);
    volumeText.setCharacterSize(20);
    volumeText.setFillColor(sf::Color::White);
    volumeText.setPosition(centerX - 50, centerY - 50);

    //skin 1 button
    skin1Button.setSize({180, 50});
    if (hasButtonTexture) {
        skin1Button.setTexture(&buttonTexture);
    } else {
        skin1Button.setFillColor(sf::Color::Blue);
    }
    skin1Button.setPosition(centerX - 90, centerY - 100);
    skin1Text.setFont(font);
    skin1Text.setCharacterSize(20);
    skin1Text.setString("Classic");
    skin1Text.setFillColor(sf::Color::White);
    centerTextInButton(skin1Text, skin1Button);

    //skin 2 button
    skin2Button.setSize({180, 50});
    if (hasButtonTexture) {
        skin2Button.setTexture(&buttonTexture);
    } else {
        skin2Button.setFillColor(sf::Color::Blue);
    }
    skin2Button.setPosition(centerX - 90, centerY - 40);
    skin2Text.setFont(font);
    skin2Text.setCharacterSize(20);
    skin2Text.setString("Golden (50)");
    skin2Text.setFillColor(sf::Color::White);
    centerTextInButton(skin2Text, skin2Button);

    //skin 3 button
    skin3Button.setSize({180, 50});
    if (hasButtonTexture) {
        skin3Button.setTexture(&buttonTexture);
    } else {
        skin3Button.setFillColor(sf::Color::Blue);
    }
    skin3Button.setPosition(centerX - 90, centerY + 20);
    skin3Text.setFont(font);
    skin3Text.setCharacterSize(20);
    skin3Text.setString("Rainbow (100)");
    skin3Text.setFillColor(sf::Color::White);
    centerTextInButton(skin3Text, skin3Button);

    //skin 4 button
    skin4Button.setSize({180, 50});
    if (hasButtonTexture) {
        skin4Button.setTexture(&buttonTexture);
    } else {
        skin4Button.setFillColor(sf::Color::Blue);
    }
    skin4Button.setPosition(centerX - 90, centerY + 80);
    skin4Text.setFont(font);
    skin4Text.setCharacterSize(20);
    skin4Text.setString("Legendary (200)");
    skin4Text.setFillColor(sf::Color::White);
    centerTextInButton(skin4Text, skin4Button);

    //back from skins to menu button
    backFromSkinsButton.setSize({200, 50});
    if (hasButtonTexture) {
        backFromSkinsButton.setTexture(&buttonTexture);
    } else {
        backFromSkinsButton.setFillColor(sf::Color::Blue);
    }
    backFromSkinsButton.setPosition(centerX - 100, centerY + 140);
    backFromSkinsText.setFont(font);
    backFromSkinsText.setCharacterSize(24);
    backFromSkinsText.setString("Powrot");
    backFromSkinsText.setFillColor(sf::Color::White);
    centerTextInButton(backFromSkinsText, backFromSkinsButton);
}

//funkcja centrujaca teksty do guzikow
void SnakeGame::centerTextInButton(sf::Text& text, const sf::RectangleShape& button) {
    sf::FloatRect textBounds = text.getLocalBounds();
    sf::Vector2f buttonCenter = button.getPosition() + button.getSize() / 2.f;
    text.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
    text.setPosition(buttonCenter);
}

//glowna petla - odpowiada za uruchomienie gry
void SnakeGame::run() {
    sf::Clock clock;
    const float frameTime = 1.0f / 60.0f; // 60 aktualizacji na sekundę
    float accumulatedTime = 0.0f;

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        accumulatedTime += deltaTime;

        processEvents();

        // Aktualizuj grę w stałych odstępach czasu
        while (accumulatedTime >= frameTime) {
            accumulatedTime -= frameTime;
            if (state == GameState::Playing) {
                update();
            }
        }

        draw(); // Renderuj tak często, jak to możliwe
    }
}

//wczytaj highscore z pliku
void SnakeGame::loadHighScore() {
    try {
        std::ifstream file("highscore.txt");
        if (file.is_open()) {
            file >> highScore;
            if (file.fail()) {
                throw std::runtime_error("Błędny format pliku highscore.txt");
            }
            file.close();
        } else {
            highScore = 0;
        }
    } catch (const std::exception& e) {
        std::cerr << "Błąd wczytywania high score: " << e.what() << std::endl;
        highScore = 0;
    }
}

//zapisz highscore do pliku
void SnakeGame::saveHighScore() {
    std::ofstream file("highscore.txt");
    if (file.is_open()) {
        file << highScore;
        file.close();
    }
}

//zaktualizuj highscore
void SnakeGame::updateHighScoreText() {
    highScoreText.setString("High Score: " + std::to_string(highScore));
}

//sprawdzanie co sie dzieje i reakcja, glownie ruch weza i guziki
void SnakeGame::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        //ruch weza w 4 strony i upewnianie sie ze nie zrobil przed chwila ruchu
        if (event.type == sf::Event::KeyPressed && state == GameState::Playing && !directionChanged) {
            if (event.key.code == sf::Keyboard::Up && direction != 2) {
                direction = 0;
                directionChanged = true;
            }
            else if (event.key.code == sf::Keyboard::Right && direction != 3) {
                direction = 1;
                directionChanged = true;
            }
            else if (event.key.code == sf::Keyboard::Down && direction != 0) {
                direction = 2;
                directionChanged = true;
            }
            else if (event.key.code == sf::Keyboard::Left && direction != 1) {
                direction = 3;
                directionChanged = true;
            }
        }

        //sprawdzanie czy kliknales w guzik
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));

            if (state == GameState::Menu) { //guziki w menu
                if (startButton.getGlobalBounds().contains(mousePos)) {
                    clickSound.play();
                    if (musicLoaded) {
                        menuMusic.stop();
                        gameMusic.play();
                    }
                    state = GameState::BoardSizeSelection;
                }
                else if (settingsButton.getGlobalBounds().contains(mousePos)) {
                    clickSound.play();
                    state = GameState::Settings;
                }
                else if (skinsButton.getGlobalBounds().contains(mousePos)) {
                    clickSound.play();
                    state = GameState::SkinsSelection;
                }
                else if (exitButton.getGlobalBounds().contains(mousePos)) {
                    window.close();
                }
            }
            else if (state == GameState::GameOver) { //guziki po przegranej
                if (restartButton.getGlobalBounds().contains(mousePos)) {
                    clickSound.play();
                    resetGame();
                    state = GameState::Playing;
                }
                else if (backToMenuButton.getGlobalBounds().contains(mousePos)) {
                    clickSound.play();
                    if (musicLoaded) {
                        gameMusic.stop();
                        menuMusic.play();
                    }
                    state = GameState::Menu;
                }
            }
            else if(state == GameState::BoardSizeSelection){ //guziki przy wyborze rozmiaru planszy
                if(smallBoardButton.getGlobalBounds().contains(mousePos)){
                    clickSound.play();
                    setBoardSize(20, 15);
                    state = GameState::Playing;
                }
                else if(mediumBoardButton.getGlobalBounds().contains(mousePos)){
                    clickSound.play();
                    setBoardSize(30, 20);
                    state = GameState::Playing;
                }
                else if(largeBoardButton.getGlobalBounds().contains(mousePos)){
                    clickSound.play();
                    setBoardSize(40, 30);
                    state = GameState::Playing;
                }
            }
            else if (state == GameState::Settings) { // ustawienia - guzik od wylaczania i wlaczania dzwieku
                if (muteButton.getGlobalBounds().contains(mousePos)) {
                    clickSound.play();
                    isMuted = !isMuted;
                    float volume = isMuted ? 0.0f : 100.0f;
                    float musicVolume = isMuted ? 0.0f : 30.0f; // Muzyka zawsze cichsza
                    eatSound.setVolume(volume);
                    deathSound.setVolume(volume); 
                    clickSound.setVolume(volume);
                    if (musicLoaded) {
                        menuMusic.setVolume(musicVolume);
                        gameMusic.setVolume(musicVolume);
                    }
                    muteText.setString(isMuted ? "Dzwiek: OFF" : "Dzwiek: ON");
                    centerTextInButton(muteText, muteButton); // odswiez guzik
                }
                else if (backFromSettingsButton.getGlobalBounds().contains(mousePos)) {
                    clickSound.play();
                    state = GameState::Menu;
                }
            }
            else if (state == GameState::SkinsSelection) { // guziki wyboru skorek
                if (skin1Button.getGlobalBounds().contains(mousePos) && isSkinUnlocked(0)) {
                    clickSound.play();
                    currentSkinId = 0;
                    loadSkinTextures(0);
                }
                else if (skin2Button.getGlobalBounds().contains(mousePos) && isSkinUnlocked(1)) {
                    clickSound.play();
                    currentSkinId = 1;
                    loadSkinTextures(1);
                }
                else if (skin3Button.getGlobalBounds().contains(mousePos) && isSkinUnlocked(2)) {
                    clickSound.play();
                    currentSkinId = 2;
                    loadSkinTextures(2);
                }
                else if (skin4Button.getGlobalBounds().contains(mousePos) && isSkinUnlocked(3)) {
                    clickSound.play();
                    currentSkinId = 3;
                    loadSkinTextures(3);
                }
                else if (backFromSkinsButton.getGlobalBounds().contains(mousePos)) {
                    clickSound.play();
                    state = GameState::Menu;
                }
            }
        }
    }
}

//zaktualizuj score - obecny
void SnakeGame::updateScoreText() {
    scoreText.setString("Score: " + std::to_string(score));
}

//aktualizowanie co robi waz
void SnakeGame::update() {
    static sf::Clock moveClock;
    const float moveInterval = 0.1f; // Czas między ruchami (sekundy)

    // Sprawdzaj kolizje CAŁY CZAS, nawet gdy wąż się nie porusza
    if (checkCollision()) {
        deathSound.play();
        if (score > highScore) {
            highScore = score;
            updateHighScoreText();
            saveHighScore();
        }
        state = GameState::GameOver;
        return; // Zakończ aktualizację, jeśli gra się skończyła
    }

    // Ruch węża tylko co określony czas
    if (moveClock.getElapsedTime().asSeconds() >= moveInterval) {
        moveClock.restart();

        directionChanged = false;
        SnakeSegment head = snake.front();

        switch (direction) {
            case 0: head.y--; break;
            case 1: head.x++; break;
            case 2: head.y++; break;
            case 3: head.x--; break;
        }

        snake.insert(snake.begin(), head);

        if (head.x == food.x && head.y == food.y) {
            score++;
            eatSound.play();
            updateScoreText();
            spawnFood();
        } else {
            snake.pop_back();
        }
    }
}

// rysowanie / gui gry menu i wszystkiego
void SnakeGame::draw() {
    //glowne okno - tło
    window.clear(sf::Color::Black);

    // tło
    if (hasGridCellTexture) {
        for (int x = 0; x < cols; x++) {
            for (int y = 0; y < rows; y++) {
                gridCellSprite.setPosition(x * size, y * size + offsetY);
                window.draw(gridCellSprite);
            }
        }
    } else {
        // Alternatywne tło (prostokąty z obramowaniem)
        for (int x = 0; x < cols; x++) {
            for (int y = 0; y < rows; y++) {
                sf::RectangleShape cell(sf::Vector2f(size, size));
                cell.setFillColor(sf::Color(30, 30, 30));
                cell.setOutlineColor(sf::Color(50, 50, 50));
                cell.setOutlineThickness(1);
                cell.setPosition(x * size, y * size + offsetY);
                window.draw(cell);
            }
        }
    }


    //menu na gorze na score i highscore
    if (hasTopBarTexture) {
        window.draw(topBarSprite);
    } else {
        sf::RectangleShape topBar(sf::Vector2f(width, offsetY));
        topBar.setFillColor(sf::Color(50, 50, 50));
        window.draw(topBar);
    }
    //przyciemnione prostokaty dla widocznosci
    if (state != GameState::Playing) {
        sf::RectangleShape overlay(sf::Vector2f(width, height));
        overlay.setFillColor(sf::Color(0, 0, 0, 150)); // Czarny z 60% przezroczystością
        window.draw(overlay);
    }

    if (state == GameState::Playing) {
        sf::RectangleShape textBg(sf::Vector2f(400, offsetY - 10));
        textBg.setFillColor(sf::Color(0, 0, 0, 120));
        textBg.setPosition(5, 5);
        window.draw(textBg);
    }


    

    //czensci wenza
    if (state == GameState::Playing){
    for (size_t i = 0; i < snake.size(); ++i) {
    const auto& segment = snake[i];
    sf::Vector2f position(segment.x * size, segment.y * size + offsetY);
    
    if (i == 0) { // Głowa
        if (hasSnakeHeadTextures) {
            sf::Sprite* headSprite = nullptr;
            switch (direction) {
                case 0: headSprite = &snakeHeadUpSprite; break;
                case 1: headSprite = &snakeHeadRightSprite; break;
                case 2: headSprite = &snakeHeadDownSprite; break;
                case 3: headSprite = &snakeHeadLeftSprite; break;
            }
            if (headSprite) {
                headSprite->setPosition(position);
                window.draw(*headSprite);
            }
        } else {
            sf::RectangleShape segmentShape(sf::Vector2f(size - 2, size - 2));
            segmentShape.setFillColor(sf::Color::Green);
            segmentShape.setPosition(position);
            window.draw(segmentShape);
        }
    } else { // Ciało
        // Sprawdź czy to skręt
        bool isCorner = false;
        sf::Sprite* cornerSprite = nullptr;
        
        if (i < snake.size() - 1) {
            int prevDir = getSegmentDirection(i - 1);
            int nextDir = getSegmentDirection(i + 1);
            
            if (prevDir != nextDir && prevDir != -1 && nextDir != -1) {
                isCorner = true;
                
                // Określ typ skrętu
                if ((prevDir == 0 && nextDir == 1) || (prevDir == 3 && nextDir == 2)) {
                    cornerSprite = &snakeCornerUpRightSprite;
                } else if ((prevDir == 0 && nextDir == 3) || (prevDir == 1 && nextDir == 2)) {
                    cornerSprite = &snakeCornerUpLeftSprite;
                } else if ((prevDir == 2 && nextDir == 1) || (prevDir == 3 && nextDir == 0)) {
                    cornerSprite = &snakeCornerDownRightSprite;
                } else if ((prevDir == 2 && nextDir == 3) || (prevDir == 1 && nextDir == 0)) {
                    cornerSprite = &snakeCornerDownLeftSprite;
                }
            }
        }
        
        if (isCorner && hasSnakeCornerTextures && cornerSprite) {
            cornerSprite->setPosition(position);
            window.draw(*cornerSprite);
        } else if (hasSnakeBodyTextures) {
            int segDir = getSegmentDirection(i);
            sf::Sprite* bodySprite = (segDir == 1 || segDir == 3) ? 
                                   &snakeBodyHorizontalSprite : &snakeBodyVerticalSprite;
            bodySprite->setPosition(position);
            window.draw(*bodySprite);
        } else {
            sf::RectangleShape segmentShape(sf::Vector2f(size - 2, size - 2));
            segmentShape.setFillColor(sf::Color::Green);
            segmentShape.setPosition(position);
            window.draw(segmentShape);
        }
    }
}
    }

    //jedzenie
    if (state == GameState::Playing) {
        if (hasFoodTexture) {
            foodSprite.setPosition(food.x * size, food.y * size + offsetY);
            window.draw(foodSprite);
        } else {
            sf::RectangleShape foodShape(sf::Vector2f(size - 2, size - 2));
            foodShape.setFillColor(sf::Color::Red);
            foodShape.setPosition(food.x * size, food.y * size + offsetY);
            window.draw(foodShape);
        }
    }

    if (state == GameState::Menu) { //menu glowne
        window.draw(startButton);
        window.draw(startText);
        window.draw(settingsButton);
        window.draw(skinsButton);
        window.draw(skinsText);
        window.draw(settingsText);
        window.draw(exitButton);
        window.draw(exitText);
    }
    else if (state == GameState::Settings) { //ustawienia
        window.draw(muteButton);
        window.draw(muteText);
        window.draw(backFromSettingsButton);
        window.draw(backFromSettingsText);
    }
    else if (state == GameState::GameOver) { //menu po smiertne
        window.draw(restartButton);
        window.draw(restartText);
        window.draw(backToMenuButton);
        window.draw(backToMenuText);
    }
    else if (state == GameState::BoardSizeSelection) { //menu wyboru planszy
        window.draw(smallBoardButton);
        window.draw(smallBoardText);
        window.draw(mediumBoardButton);
        window.draw(mediumBoardText);
        window.draw(largeBoardButton);
        window.draw(largeBoardText);
    }
    else if (state == GameState::SkinsSelection) { //menu wyboru skorek
        // Rysuj guziki z odpowiednimi kolorami (zablokowane/odblokowane)
        if (isSkinUnlocked(0)) {
            skin1Button.setFillColor(sf::Color::Green);
        } else {
            skin1Button.setFillColor(sf::Color::Red);
        }
        window.draw(skin1Button);
        window.draw(skin1Text);
        
        if (isSkinUnlocked(1)) {
            skin2Button.setFillColor(sf::Color::Green);
        } else {
            skin2Button.setFillColor(sf::Color::Red);
        }
        window.draw(skin2Button);
        window.draw(skin2Text);
        
        if (isSkinUnlocked(2)) {
            skin3Button.setFillColor(sf::Color::Green);
        } else {
            skin3Button.setFillColor(sf::Color::Red);
        }
        window.draw(skin3Button);
        window.draw(skin3Text);
        
        if (isSkinUnlocked(3)) {
            skin4Button.setFillColor(sf::Color::Green);
        } else {
            skin4Button.setFillColor(sf::Color::Red);
        }
        window.draw(skin4Button);
        window.draw(skin4Text);
        
        window.draw(backFromSkinsButton);
        window.draw(backFromSkinsText);
    }

    //napis score i highscore razem z wynikami
    window.draw(scoreText);
    window.draw(highScoreText);

    window.display();
}

//pojawianie sie jedzenia
void SnakeGame::spawnFood() {
    while (true) {
        int x = rand() % cols;
        int y = rand() % rows; // Jedzenie tylko w obszarze gry (0 do rows-1)
        //upewnianie sie ze nie na wezu
        bool onSnake = false;
        for (const auto& segment : snake) {
            if (segment.x == x && segment.y == y) {
                onSnake = true;
                break;
            }
        }
        if (!onSnake) {
            food.x = x;
            food.y = y;
            break;
        }
    }
}

//reset
void SnakeGame::resetGame() {
    snake.clear();
    // Wąż zaczyna w środku planszy, ale w współrzędnych logicznych (bez offsetY)
    snake.push_back(SnakeSegment(cols/2, rows/2));
    direction = 1;
    directionChanged = false;
    gameOver = false;
    score = 0;
    updateScoreText();
    spawnFood();
}

//sprawdzanie czy waz powinien umrzec, czy w krawedz lub w siebie
bool SnakeGame::checkCollision() {
    const auto& head = snake.front();

    // Sprawdź kolizję z krawędziami (współrzędne logiczne od 0 do cols-1, 0 do rows-1)
    if (head.x < 0 || head.x >= cols || head.y < 0 || head.y >= rows) {
        return true;
    }

    //czy w siebie
    for (size_t i = 1; i < snake.size(); ++i) {
        if (snake[i].x == head.x && snake[i].y == head.y) {
            return true;
        }
    }
    return false;
}

//funkcja odpowiadajaca za wybor rozmiaru planszy
void SnakeGame::setBoardSize(int newCols, int newRows) {
    cols = newCols;
    rows = newRows;
    width = cols * size;
    height = rows * size + offsetY;
    
    // Przeładuj sprite'y dla nowego rozmiaru
    setupSprites();
    
    // Aktualizuj pozycje guzików dla nowego rozmiaru
    setupButtons();
    
    // Zresetuj grę z nowymi wymiarami
    resetGame();

    window.create(sf::VideoMode(width, height), "Snake");
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);
}       

//funkcja odpowiadajaca za dowiedzenie sie w jaka strone idzie waz
int SnakeGame::getSegmentDirection(size_t index) const {
    if (index >= snake.size()) return -1;
    
    if (index == 0) {
        // Głowa - zwróć aktualny kierunek
        return direction;
    }
    
    // Dla segmentów ciała - kierunek od poprzedniego do aktualnego
    const auto& current = snake[index];
    const auto& previous = snake[index - 1];
    
    if (previous.x < current.x) return 1; // w prawo
    if (previous.x > current.x) return 3; // w lewo
    if (previous.y < current.y) return 2; // w dół
    if (previous.y > current.y) return 0; // w górę
    
    return -1;
}

//funkcja odpowiadajaca za ladowanie modulu skorek
void SnakeGame::loadSkinsModule() {
    skinsModule = LoadLibrary("SnakeSkinsManager.dll");
    if (skinsModule) {
        isClassicUnlocked = (IsClassicUnlockedFunc)GetProcAddress(skinsModule, "isClassicSkinUnlocked");
        isGoldenUnlocked = (IsGoldenUnlockedFunc)GetProcAddress(skinsModule, "isGoldenSkinUnlocked");
        isRainbowUnlocked = (IsRainbowUnlockedFunc)GetProcAddress(skinsModule, "isRainbowSkinUnlocked");
        isLegendaryUnlocked = (IsLegendaryUnlockedFunc)GetProcAddress(skinsModule, "isLegendarySkinUnlocked");
        getSkinName = (GetSkinNameFunc)GetProcAddress(skinsModule, "getSkinName");
        getRequiredScore = (GetRequiredScoreFunc)GetProcAddress(skinsModule, "getRequiredScore");
    }
}

//odladowywanie skorek
void SnakeGame::unloadSkinsModule() {
    if (skinsModule) {
        FreeLibrary(skinsModule);
    }
}

//sprawdzanie czy skorka jest odblokowana
bool SnakeGame::isSkinUnlocked(int skinId) {
    if (!skinsModule) return skinId == 0;
    
    switch (skinId) {
        case 0: return isClassicUnlocked ? isClassicUnlocked(highScore) : true;
        case 1: return isGoldenUnlocked ? isGoldenUnlocked(highScore) : false;
        case 2: return isRainbowUnlocked ? isRainbowUnlocked(highScore) : false;
        case 3: return isLegendaryUnlocked ? isLegendaryUnlocked(highScore) : false;
        default: return false;
    }
}

//ladownaie skorek w zaleznosci od wyboru
void SnakeGame::loadSkinTextures(int skinId) {
    std::string skinFolder = "textures/skins/skin" + std::to_string(skinId) + "/";
    
    // Reset flag
    hasSnakeHeadTextures = false;
    hasSnakeBodyTextures = false;
    hasSnakeCornerTextures = false;
    
    // Ładuj wszystkie tekstury głowy
    hasSnakeHeadTextures = 
        snakeHeadUpTexture.loadFromFile(skinFolder + "snake_head_up.png") &&
        snakeHeadDownTexture.loadFromFile(skinFolder + "snake_head_down.png") &&
        snakeHeadLeftTexture.loadFromFile(skinFolder + "snake_head_left.png") &&
        snakeHeadRightTexture.loadFromFile(skinFolder + "snake_head_right.png");

    // Ładuj tekstury ciała
    hasSnakeBodyTextures =
        snakeBodyHorizontalTexture.loadFromFile(skinFolder + "snake_body_horizontal.png") &&
        snakeBodyVerticalTexture.loadFromFile(skinFolder + "snake_body_vertical.png");

    // Ładuj tekstury zakrętów
    hasSnakeCornerTextures =
        snakeCornerUpLeftTexture.loadFromFile(skinFolder + "snake_corner_up_left.png") &&
        snakeCornerUpRightTexture.loadFromFile(skinFolder + "snake_corner_up_right.png") &&
        snakeCornerDownLeftTexture.loadFromFile(skinFolder + "snake_corner_down_left.png") &&
        snakeCornerDownRightTexture.loadFromFile(skinFolder + "snake_corner_down_right.png");
    
    // Ponownie skonfiguruj sprite'y z nowymi teksturami
    setupSprites();
}


SnakeGame::~SnakeGame() {
    if (musicLoaded) {
        menuMusic.stop();
        gameMusic.stop();
    }
    unloadSkinsModule();

}

//zapis wyboru skorki
void SnakeGame::saveSkinSelection() {
    std::ofstream file("skin.txt");
    if (file.is_open()) {
        file << currentSkinId;
        file.close();
    }
}

//zaladowanie wybranej skorki
void SnakeGame::loadSkinSelection() {
    std::ifstream file("skin.txt");
    if (file.is_open()) {
        file >> currentSkinId;
        file.close();
        if (isSkinUnlocked(currentSkinId)) {
            loadSkinTextures(currentSkinId);
        } else {
            currentSkinId = 0;
            loadSkinTextures(0);
        }
    }
}


