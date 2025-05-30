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

SnakeGame::SnakeGame() : window(sf::VideoMode(width, height), "Snake"), direction(1), gameOver(false) {
    window.setFramerateLimit(10);
    snake.push_back(SnakeSegment(10, 10));
    spawnFood();

    //score i highscore
    score = 0;
    directionChanged = false;
    if (!font.loadFromFile("arial.ttf")) {
    }

    maxRows = (height - offsetY) / size;
    cols = width / size;
    rows = maxRows;

    highScore = 0;
    loadHighScore();

    //dzwieki 
    if (!eatBuffer.loadFromFile("sounds/eat.wav"))
        std::cerr << "Nie mogę załadować eat.wav\n";
    if (!deathBuffer.loadFromFile("sounds/death.wav"))
        std::cerr << "Nie mogę załadować death.wav\n";
    if (!clickBuffer.loadFromFile("sounds/click.wav"))
        std::cerr << "Nie mogę załadować click.wav\n";

    eatSound.setBuffer(eatBuffer);
    deathSound.setBuffer(deathBuffer);
    clickSound.setBuffer(clickBuffer);

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
    volume = 50.0f; // Default volume 50%
    isDraggingSlider = false;

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
        snakeHeadUpTexture.loadFromFile("textures/snake_head_up.png") &&
        snakeHeadDownTexture.loadFromFile("textures/snake_head_down.png") &&
        snakeHeadLeftTexture.loadFromFile("textures/snake_head_left.png") &&
        snakeHeadRightTexture.loadFromFile("textures/snake_head_right.png");

    hasSnakeBodyTextures =
        snakeBodyHorizontalTexture.loadFromFile("textures/snake_body_horizontal.png") &&
        snakeBodyVerticalTexture.loadFromFile("textures/snake_body_vertical.png");

    hasSnakeCornerTextures =
        snakeCornerUpLeftTexture.loadFromFile("textures/snake_corner_up_left.png") &&
        snakeCornerUpRightTexture.loadFromFile("textures/snake_corner_up_right.png") &&
        snakeCornerDownLeftTexture.loadFromFile("textures/snake_corner_down_left.png") &&
        snakeCornerDownRightTexture.loadFromFile("textures/snake_corner_down_right.png");
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

void SnakeGame::setupButtons() {
    // Oblicz środek okna
    float centerX = width / 2.0f;
    float centerY = height / 2.0f;
    
    //startbutton
    startButton.setSize({200, 50});
    if (hasButtonTexture) {
        startButton.setTexture(&buttonTexture);
    } else {
        startButton.setFillColor(sf::Color::Blue);
    }
    startButton.setPosition(centerX - 100, centerY - 75);
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
    settingsButton.setPosition(centerX - 100, centerY - 10);
    settingsText.setFont(font);
    settingsText.setCharacterSize(24);
    settingsText.setString("Ustawienia");
    settingsText.setFillColor(sf::Color::White);
    centerTextInButton(settingsText, settingsButton);

    //exitbutton
    exitButton.setSize({200, 50});
    if (hasButtonTexture) {
        exitButton.setTexture(&buttonTexture);
    } else {
        exitButton.setFillColor(sf::Color::Blue);
    }
    exitButton.setPosition(centerX - 100, centerY + 55);
    exitText.setFont(font);
    exitText.setCharacterSize(24);
    exitText.setString("Wyjdz");
    exitText.setFillColor(sf::Color::White);
    centerTextInButton(exitText, exitButton);

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

    // Settings buttons
    //mutebutton
    muteButton.setSize({200, 50});
    if (hasButtonTexture) {
        muteButton.setTexture(&buttonTexture);
    } else {
        muteButton.setFillColor(sf::Color::Blue);
    }
    muteButton.setPosition(centerX - 100, centerY - 100);
    muteText.setFont(font);
    muteText.setCharacterSize(24);
    muteText.setFillColor(sf::Color::White);
    centerTextInButton(muteText, muteButton);

    //volume slider background
    volumeSliderBg.setSize({300, 20});
    volumeSliderBg.setFillColor(sf::Color(100, 100, 100));
    volumeSliderBg.setPosition(centerX - 150, centerY - 10);

    //volume slider handle
    volumeSliderHandle.setSize({20, 30});
    volumeSliderHandle.setFillColor(sf::Color::White);
    
    //volume text
    volumeText.setFont(font);
    volumeText.setCharacterSize(20);
    volumeText.setFillColor(sf::Color::White);
    volumeText.setPosition(centerX - 50, centerY - 50);

    //back from settings button
    backFromSettingsButton.setSize({200, 50});
    if (hasButtonTexture) {
        backFromSettingsButton.setTexture(&buttonTexture);
    } else {
        backFromSettingsButton.setFillColor(sf::Color::Blue);
    }
    backFromSettingsButton.setPosition(centerX - 100, centerY + 50);
    backFromSettingsText.setFont(font);
    backFromSettingsText.setCharacterSize(24);
    backFromSettingsText.setString("Powrot");
    backFromSettingsText.setFillColor(sf::Color::White);
    centerTextInButton(backFromSettingsText, backFromSettingsButton);

    updateVolumeText();
    updateSoundVolume();
}

void SnakeGame::centerTextInButton(sf::Text& text, const sf::RectangleShape& button) {
    sf::FloatRect textBounds = text.getLocalBounds();
    sf::Vector2f buttonCenter = button.getPosition() + button.getSize() / 2.f;
    text.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
    text.setPosition(buttonCenter);
}

//glowna petla - odpowiada za uruchomienie gry
void SnakeGame::run() {
    while (window.isOpen()) {
        processEvents();
        if (state == GameState::Playing) {
            update();
        }
        draw();
    }
}

//wczytaj highscore z pliku
void SnakeGame::loadHighScore() {
    std::ifstream file("highscore.txt");
    if (file.is_open()) {
        file >> highScore;
        file.close();
    } else {
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

            if (state == GameState::Menu) {
                if (startButton.getGlobalBounds().contains(mousePos)) {
                    clickSound.play();
                    state = GameState::BoardSizeSelection;
                }
                else if (settingsButton.getGlobalBounds().contains(mousePos)) {
                    clickSound.play();
                    state = GameState::Settings;
                }
                else if (exitButton.getGlobalBounds().contains(mousePos)) {
                    window.close();
                }
            }
            else if (state == GameState::GameOver) {
                if (restartButton.getGlobalBounds().contains(mousePos)) {
                    clickSound.play();
                    resetGame();
                    state = GameState::Playing;
                }
                else if (backToMenuButton.getGlobalBounds().contains(mousePos)) {
                    clickSound.play();
                    state = GameState::Menu;
                }
            }
            
            else if(state == GameState::BoardSizeSelection){
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
            else if (state == GameState::Settings) {
                if (muteButton.getGlobalBounds().contains(mousePos)) {
                    clickSound.play();
                    isMuted = !isMuted;
                    updateVolumeText();
                    updateSoundVolume();
                }
                else if (backFromSettingsButton.getGlobalBounds().contains(mousePos)) {
                    clickSound.play();
                    state = GameState::Menu;
                }
                else if (volumeSliderBg.getGlobalBounds().contains(mousePos) || 
                         volumeSliderHandle.getGlobalBounds().contains(mousePos)) {
                    isDraggingSlider = true;
                    handleSliderDrag(mousePos.x);
                }
            }
        }

        // Handle mouse button release
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            isDraggingSlider = false;
        }

        // Handle mouse movement for slider dragging
        if (event.type == sf::Event::MouseMoved && isDraggingSlider && state == GameState::Settings) {
            handleSliderDrag(static_cast<float>(event.mouseMove.x));
        }
    }
}

//zaktualizuj score - obecny
void SnakeGame::updateScoreText() {
    scoreText.setString("Score: " + std::to_string(score));
}

//aktualizowanie co robi waz
void SnakeGame::update() {
    
    // nie aktualizuj, jesli gra skonczona
    directionChanged = false;
    SnakeSegment head = snake.front();

    //poruszanie sie po planszy x.y
    switch (direction) {
        case 0: head.y--; break;
        case 1: head.x++; break;
        case 2: head.y++; break;
        case 3: head.x--; break;
    }

    //czy uderza glowa w krawedzie
    if (head.x < 0 || head.y < 0 || head.x >= cols || head.y >= rows)
        gameOver = true;

    //czy uderza glowa w siebie
    for (const auto& segment : snake) {
        if (segment.x == head.x && segment.y == head.y)
            gameOver = true;
    }

    snake.insert(snake.begin(), head);

    //czy wszedl na owoc i go zjadl
    if (head.x == food.x && head.y == food.y) {
        score++;
        eatSound.play();
        updateScoreText();
        spawnFood();
    }
    else {
        snake.pop_back();
    }

    //ponowne sprawdzanie kolizji
    if (checkCollision()) {
        deathSound.play();
        if (score > highScore) {
            highScore = score;
            updateHighScoreText();
            saveHighScore();
        }
        state = GameState::GameOver;
    }
}

//rysowanie menu i gry
void SnakeGame::draw() {
    //glowne okno - tło
    window.clear(sf::Color::Black);


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

    if (state != GameState::Playing) {
        sf::RectangleShape overlay(sf::Vector2f(width, height));
        overlay.setFillColor(sf::Color(0, 0, 0, 150)); // Czarny z 60% przezroczystością
        window.draw(overlay);
    }

    //czensci wenza
    if (state == GameState::Playing){
        // W sekcji rysowania węża, zastąp całą pętlę for:
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

    //menu i menu po smierci
    if (state == GameState::Menu) {
        window.draw(startButton);
        window.draw(startText);
        window.draw(settingsButton);
        window.draw(settingsText);
        window.draw(exitButton);
        window.draw(exitText);
    }
    else if (state == GameState::Settings) {
        window.draw(muteButton);
        window.draw(muteText);
        window.draw(volumeSliderBg);
        window.draw(volumeSliderHandle);
        window.draw(volumeText);
        window.draw(backFromSettingsButton);
        window.draw(backFromSettingsText);
    }
    else if (state == GameState::GameOver) {
        window.draw(restartButton);
        window.draw(restartText);
        window.draw(backToMenuButton);
        window.draw(backToMenuText);
    }
    else if (state == GameState::BoardSizeSelection) {
        window.draw(smallBoardButton);
        window.draw(smallBoardText);
        window.draw(mediumBoardButton);
        window.draw(mediumBoardText);
        window.draw(largeBoardButton);
        window.draw(largeBoardText);
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

void SnakeGame::setBoardSize(int newCols, int newRows) {
    cols = newCols;
    rows = newRows;
    width = cols * size;
    height = rows * size + offsetY;
    
    // Aktualizuj okno bez zmiany jego rozmiaru
    // window.create(sf::VideoMode(width, height), "Snake");
    
    // Przeładuj sprite'y dla nowego rozmiaru
    setupSprites();
    
    // Aktualizuj pozycje guzików dla nowego rozmiaru
    setupButtons();
    
    // Zresetuj grę z nowymi wymiarami
    resetGame();
}       

void SnakeGame::updateVolumeText() {
    muteText.setString(isMuted ? "Dzwiek: OFF" : "Dzwiek: ON");
    centerTextInButton(muteText, muteButton);
    
    volumeText.setString("Glosnosc: " + std::to_string(static_cast<int>(volume)) + "%");
    
    // Update slider handle position
    float sliderProgress = volume / 100.0f;
    float handleX = volumeSliderBg.getPosition().x + (volumeSliderBg.getSize().x - volumeSliderHandle.getSize().x) * sliderProgress;
    volumeSliderHandle.setPosition(handleX, volumeSliderBg.getPosition().y - 5);
}

void SnakeGame::updateSoundVolume() {
    float actualVolume = isMuted ? 0.0f : volume;
    eatSound.setVolume(actualVolume);
    deathSound.setVolume(actualVolume);
}

void SnakeGame::handleSliderDrag(float mouseX) {
    float sliderLeft = volumeSliderBg.getPosition().x;
    float sliderRight = sliderLeft + volumeSliderBg.getSize().x;
    
    if (mouseX < sliderLeft) mouseX = sliderLeft;
    if (mouseX > sliderRight) mouseX = sliderRight;
    
    float progress = (mouseX - sliderLeft) / volumeSliderBg.getSize().x;
    volume = progress * 100.0f;
    
    updateVolumeText();
    updateSoundVolume();
}

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