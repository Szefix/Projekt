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

    setupButtons();
}

void SnakeGame::setupButtons() {
    // Oblicz środek okna
    float centerX = width / 2.0f;
    float centerY = height / 2.0f;
    
    //startbutton
    startButton.setSize({200, 50});
    startButton.setFillColor(sf::Color::Blue);
    startButton.setPosition(centerX - 100, centerY - 50);
    startText.setFont(font);
    startText.setCharacterSize(24);
    startText.setString("Zacznij gre");
    startText.setFillColor(sf::Color::White);
    centerTextInButton(startText, startButton);

    //exitbutton
    exitButton.setSize({200, 50});
    exitButton.setFillColor(sf::Color::Blue);
    exitButton.setPosition(centerX - 100, centerY + 20);
    exitText.setFont(font);
    exitText.setCharacterSize(24);
    exitText.setString("Wyjdz");
    exitText.setFillColor(sf::Color::White);
    centerTextInButton(exitText, exitButton);

    //restartbutton
    restartButton.setSize({200, 50});
    restartButton.setFillColor(sf::Color::Blue);
    restartButton.setPosition(centerX - 100, centerY - 50);
    restartText.setFont(font);
    restartText.setCharacterSize(24);
    restartText.setString("Restart");
    restartText.setFillColor(sf::Color::White);
    centerTextInButton(restartText, restartButton);

    //backtomenubutton
    backToMenuButton.setSize({200, 50});
    backToMenuButton.setFillColor(sf::Color::Blue);
    backToMenuButton.setPosition(centerX - 100, centerY + 20);
    backToMenuText.setFont(font);
    backToMenuText.setCharacterSize(24);
    backToMenuText.setString("Menu");
    backToMenuText.setFillColor(sf::Color::White);
    centerTextInButton(backToMenuText, backToMenuButton);

    //smallboardbutton
    smallBoardButton.setSize({200, 50});
    smallBoardButton.setFillColor(sf::Color::Blue);
    smallBoardButton.setPosition(centerX - 100, centerY - 100);
    smallBoardText.setFont(font);
    smallBoardText.setCharacterSize(24);
    smallBoardText.setString("Small (20x15)");
    smallBoardText.setFillColor(sf::Color::White);
    centerTextInButton(smallBoardText, smallBoardButton);

    //mediumboardbutton
    mediumBoardButton.setSize({200, 50});
    mediumBoardButton.setFillColor(sf::Color::Blue);
    mediumBoardButton.setPosition(centerX - 100, centerY - 25);
    mediumBoardText.setFont(font);
    mediumBoardText.setCharacterSize(24);
    mediumBoardText.setString("Medium (30x20)");
    mediumBoardText.setFillColor(sf::Color::White);
    centerTextInButton(mediumBoardText, mediumBoardButton);

    //largeboardbutton
    largeBoardButton.setSize({200, 50});
    largeBoardButton.setFillColor(sf::Color::Blue);
    largeBoardButton.setPosition(centerX - 100, centerY + 50);
    largeBoardText.setFont(font);
    largeBoardText.setCharacterSize(24);
    largeBoardText.setString("Large (40x30)");
    largeBoardText.setFillColor(sf::Color::White);
    centerTextInButton(largeBoardText, largeBoardButton);
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
    //glowne okno
    window.clear(sf::Color::Black);

    //menu na gorze na score i highscore
    sf::RectangleShape topBar(sf::Vector2f(width, offsetY));
    topBar.setFillColor(sf::Color(50, 50, 50));
    window.draw(topBar);

    //czensci wenza
    if (state == GameState::Playing){
        sf::RectangleShape segmentShape(sf::Vector2f(size - 2, size - 2));
        segmentShape.setFillColor(sf::Color::Green);
        for (const auto& segment : snake) {
            segmentShape.setPosition(segment.x * size, segment.y * size + offsetY);
            window.draw(segmentShape);
        }
    }

    //jedzenie
    if (state == GameState::Playing) {
        sf::RectangleShape foodShape(sf::Vector2f(size - 2, size - 2));
        foodShape.setFillColor(sf::Color::Red);
        foodShape.setPosition(food.x * size, food.y * size + offsetY);
        window.draw(foodShape);
    }

    //menu i menu po smierci
    if (state == GameState::Menu) {
        window.draw(startButton);
        window.draw(startText);
        window.draw(exitButton);
        window.draw(exitText);
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
    
    // Aktualizuj pozycje guzików dla nowego rozmiaru
    setupButtons();
    
    // Zresetuj grę z nowymi wymiarami
    resetGame();
}