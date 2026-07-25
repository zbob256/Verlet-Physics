#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <deque>
#include <numeric>
#include <cmath>
#include <execution>

#include "render/render.hpp"
#include "engine/object.hpp"
#include "engine/solver.hpp"
#include "engine/vec2.hpp"
#include "engine/rectangle.hpp"
#include "settings.hpp"

int main() {
    sf::RenderWindow window(
        sf::VideoMode({WIDTH, HEIGHT}),
        "Verlet Physics",
        sf::Style::Titlebar | sf::Style::Close
    );
    window.setFramerateLimit(MAX_FPS);
    
    sf::Image icon;
    bool iconLoaded = icon.loadFromFile("assets/icon.png");
    if (iconLoaded) {
        window.setIcon(icon);
    } else {
        std::cout << "Window icon failed to load." << std::endl;
    }

    sf::Font font("assets/font.ttf");
    sf::Text fpsText(font);
    fpsText.setCharacterSize(24);
    fpsText.setFillColor(FONT_COLOR);
    fpsText.setPosition({10.f, 10.f});

    sf::RectangleShape textBackground;
    textBackground.setFillColor(TEXT_BG_COLOR);
    textBackground.setPosition({5.f, 5.f});
    textBackground.setSize({150.f, 50.f});

    sf::Clock clock;
    float elapsed = 0.f;
    unsigned int frames = 0;

    sf::Clock fpsUpdateClock;
    sf::Clock spawnClock;
    sf::Clock totalClock;

    std::deque<float> deltaTimeHistory;

    Renderer renderer(window);
    Solver solver(GRAVITY, static_cast<float>(WIDTH), static_cast<float>(HEIGHT), RADIUS, DAMPING);

    std::vector<Object> balls;
    std::vector<Rectangle> obstacles;

    bool rightClicked = false;
    int nextRopeId = 0;
    Vec2 firstPosition;
    Vec2 DIRECTION;

    while (window.isOpen()) {
        float t = totalClock.getElapsedTime().asSeconds();

        if (DIRECTION_MODE == DirectionMode::Manual) {
            DIRECTION = {MANUAL_DIRECTION_X, MANUAL_DIRECTION_Y};
        } else if (DIRECTION_MODE == DirectionMode::Sin) {
            float offset = std::sin(t * SIN_DIRECTION_DTHETA) * (SIN_DIRECTION_RANGE * 0.5f);
            float angleDegrees = SIN_DIRECTION_TARGET + offset;
            float angleRadians = angleDegrees * (3.14159f / 180.f);

            DIRECTION = Vec2{std::cos(angleRadians), std::sin(angleRadians)} * SIN_DIRECTION_SPEED;
        } else if (DIRECTION_MODE == DirectionMode::Random) {
            float angle = (static_cast<float>(rand()) / RAND_MAX) * 2.f * 3.14159f;
            DIRECTION = Vec2{static_cast<float>(cos(angle)), static_cast<float>(sin(angle))} * RANDOM_DIRECTION_SPEED;
        }

        DIRECTION = DIRECTION * SCALE * 0.001;

        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape) {
                    window.close();
                } else if (keyPressed->code == sf::Keyboard::Key::Backspace) {
                    balls.clear();
                } else if (keyPressed->code == sf::Keyboard::Key::Enter) {
                    obstacles.clear();
                } else if (keyPressed->code == sf::Keyboard::Key::Num4) {
                    sf::Vector2i mouse = sf::Mouse::getPosition(window);
                    Vec2 mousePosition = {static_cast<float>(mouse.x), static_cast<float>(mouse.y)};
                    obstacles.push_back(Rectangle{mousePosition, RECTANGLE_LONG, RECTANGLE_SHORT});
                }  else if (keyPressed->code == sf::Keyboard::Key::Num5) {
                    sf::Vector2i mouse = sf::Mouse::getPosition(window);
                    Vec2 mousePosition = {static_cast<float>(mouse.x), static_cast<float>(mouse.y)};
                    obstacles.push_back(Rectangle{mousePosition, RECTANGLE_SHORT, RECTANGLE_LONG});
                }  else if (keyPressed->code == sf::Keyboard::Key::Num1) {
                    sf::Vector2i mouse = sf::Mouse::getPosition(window);
                    Vec2 mousePosition = {static_cast<float>(mouse.x), static_cast<float>(mouse.y)};
                    obstacles.push_back(Rectangle{mousePosition, SQUARE_SMALL, SQUARE_SMALL});
                }  else if (keyPressed->code == sf::Keyboard::Key::Num2) {
                    sf::Vector2i mouse = sf::Mouse::getPosition(window);
                    Vec2 mousePosition = {static_cast<float>(mouse.x), static_cast<float>(mouse.y)};
                    obstacles.push_back(Rectangle{mousePosition, SQAURE_MED, SQAURE_MED});
                }  else if (keyPressed->code == sf::Keyboard::Key::Num3) {
                    sf::Vector2i mouse = sf::Mouse::getPosition(window);
                    Vec2 mousePosition = {static_cast<float>(mouse.x), static_cast<float>(mouse.y)};
                    obstacles.push_back(Rectangle{mousePosition, SQUARE_BIG, SQUARE_BIG});
                }
            } else if (const auto* mouseClicked = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseClicked->button == sf::Mouse::Button::Right) {
                    if (!rightClicked) {
                        firstPosition = {static_cast<float>(mouseClicked->position.x), static_cast<float>(mouseClicked->position.y)};

                        rightClicked = true;
                    } else {
                        Vec2 secondPosition = {static_cast<float>(mouseClicked->position.x), static_cast<float>(mouseClicked->position.y)};

                        float xDistance = secondPosition.x - firstPosition.x;
                        float yDistance = secondPosition.y - firstPosition.y;
                        float distance = hypot(xDistance, yDistance);

                        float spacing = RADIUS * 2.f;
                        int numBalls = static_cast<int>(distance / spacing) * SLACK + 1;

                        int currentRopeId = nextRopeId++;

                        for (int i = 0; i < numBalls; i++) {
                            Vec2 ropePosition = {
                                firstPosition.x + (xDistance / (numBalls - 1)) * i,
                                firstPosition.y + (yDistance / (numBalls - 1)) * i
                            };

                            Object newBall(ropePosition, ropePosition, RADIUS);
                            newBall.isRope = true;
                            newBall.ropeId = currentRopeId;

                            if (i == 0 || i == numBalls - 1) {
                                newBall.isPinned = true;
                            }

                            balls.push_back(newBall);
                        }

                        rightClicked = false;
                    }
                }
            }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            if (spawnClock.getElapsedTime().asSeconds() >= SPAWN_INTERVAL) {
                sf::Vector2i mouse = sf::Mouse::getPosition(window);
                Vec2 mousePosition = {static_cast<float>(mouse.x), static_cast<float>(mouse.y)};
                Vec2 startingPreviousPosition = mousePosition - DIRECTION;

                Object newBall(mousePosition, startingPreviousPosition, RADIUS);
                newBall.hue = balls.size() % 256;
                balls.push_back(newBall);

                spawnClock.restart();
            }
        }

        float deltaTime = clock.restart().asSeconds();
        
        if (deltaTimeHistory.size() == 10) {
            float latestTimes = std::accumulate(deltaTimeHistory.begin(), deltaTimeHistory.end(), 0.f);

            if (deltaTime > latestTimes) {
                deltaTime = latestTimes / 10;
            }
        }

        deltaTimeHistory.push_back(deltaTime);
        if (deltaTimeHistory.size() > 10) {
            deltaTimeHistory.pop_front();
        }

        float fps = 1.f / deltaTime;

        if (fpsUpdateClock.getElapsedTime().asSeconds() >= 1.f / TEXT_FPS) {
            fpsText.setString(
                "FPS: " + std::to_string(static_cast<int>(fps)) + "\n" +
                "Objects: " + std::to_string(balls.size()) + "\n" +
                "Obstacles: " + std::to_string(obstacles.size())
            );

            fpsUpdateClock.restart();
        }

        window.clear(BACKGROUND_COLOR);
        
        for (int i  = 0; i < balls.size(); i++) {
            Object &ball = balls[i];

            solver.update(ball, deltaTime);

            if (ball.isRope) {
                renderer.drawRopeElement(ball);

                if (i + 1 < balls.size() && balls[i + 1].isRope && balls[i + 1].ropeId == ball.ropeId) {
                    solver.applyConstraintRope(ball, balls[i + 1], SLACK_GAP);
                }
            } else {
                renderer.draw(ball);
            }
        }

        for (int steps = 0; steps < SUBSTEPS; steps++) {
            solver.resolveCollision(balls, RADIUS);
        }

        std::for_each(std::execution::par, balls.begin(), balls.end(), [&](Object& ball) {
            for (auto& obstacle : obstacles) {
                solver.resolveRectCollision(ball, obstacle);
            }
        });
        
        for (auto& obstacle : obstacles) {
            renderer.drawRectangle(obstacle);
        }
        
        sf::FloatRect bounds = fpsText.getLocalBounds();
        textBackground.setSize({bounds.size.x + 15.f, bounds.size.y + 20.f});
        textBackground.setPosition({fpsText.getPosition().x - 5.f, fpsText.getPosition().y - 5.f});

        window.draw(textBackground);
        window.draw(fpsText);
        window.display();
    }
}