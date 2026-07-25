#pragma once
#include <SFML/Graphics.hpp>
#include "engine/object.hpp"
#include "engine/rectangle.hpp"

class Renderer {
    public:
        Renderer(sf::RenderTarget& target);
        void draw(Object& ball);
        void drawRopeElement(Object& ball);
        void drawRectangle(Rectangle& rectangle);
    
    private:
        sf::RenderTarget& target;
        sf::Color color(int value);
};