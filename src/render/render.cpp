#include "render/render.hpp"
#include <SFML/Graphics.hpp>

Renderer::Renderer(sf::RenderTarget& target)
    : target(target) {}

void Renderer::draw(Object& ball) {
    sf::CircleShape circle(ball.radius);
    circle.setPosition({ball.position.x - ball.radius, ball.position.y - ball.radius});
    
    circle.setFillColor(color(ball.hue));

    target.draw(circle);
}

void Renderer::drawRopeElement(Object& ball) {
    sf::CircleShape circle(ball.radius);
    circle.setPosition({ball.position.x - ball.radius, ball.position.y - ball.radius});
    
    circle.setFillColor(sf::Color::White);

    target.draw(circle);
}

void Renderer::drawRectangle(Rectangle& rectangle) {
    sf::RectangleShape shape({rectangle.width, rectangle.height});
    shape.setOrigin({rectangle.width / 2.f, rectangle.height / 2.f});
    shape.setPosition({rectangle.position.x, rectangle.position.y});
    shape.setFillColor(sf::Color::White);

    target.draw(shape);
}

sf::Color Renderer::color(int value) {
    int segment = value / 43;
    int remainder = (value % 43) * 6;

    switch (segment) {
        case 0: return sf::Color(255, remainder, 0);
        case 1: return sf::Color(255 - remainder, 255, 0);
        case 2: return sf::Color(0, 255, remainder);
        case 3: return sf::Color(0, 255 - remainder, 255);
        case 4: return sf::Color(remainder, 0, 255);
        default: return sf::Color(255, 0, 255 - remainder);
    }
}