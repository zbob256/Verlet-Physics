#pragma once
#include <SFML/Graphics.hpp>
#include "engine/directions.hpp"


// Window side lengths
unsigned int WIDTH = 800;
unsigned int HEIGHT = 600;


// FPS: 0 for uncapped
const unsigned int MAX_FPS = 240; 
const unsigned int TEXT_FPS = 5;


// Colors
const sf::Color BACKGROUND_COLOR = sf::Color(0, 0, 0);
const sf::Color FONT_COLOR = sf::Color(255, 255, 255);
const sf::Color TEXT_BG_COLOR = sf::Color(40, 40, 40);


// Ball and rope radius
const float RADIUS = 8.f;


// Rectangle length and width (for both vertical or sideways)
const float RECTANGLE_LONG = 128.f;
const float RECTANGLE_SHORT = 16.f;


// Side lengths for all 3 squares
const float SQUARE_BIG = 128.f;
const float SQUARE_SMALL = 32.f;
const float SQAURE_MED = 64.f;


// Pick between Manual, Sin, and Random
const DirectionMode DIRECTION_MODE = DirectionMode::Sin;


// X and Y magnitudes
const int MANUAL_DIRECTION_X = 50;
const int MANUAL_DIRECTION_Y = 50;


// How far sin oscilates back and forth, the angle it is pointing at, how fast the angle changes, and the magnitude
const int SIN_DIRECTION_RANGE = 20;
const int SIN_DIRECTION_TARGET = 90;
const float SIN_DIRECTION_DTHETA = 4.f;
const float SIN_DIRECTION_SPEED = 100.f;


// Magnitude of velocity for random direction
const float RANDOM_DIRECTION_SPEED = 50.f;


// Slower number = faster spawn rates of balls
const float SPAWN_INTERVAL = 0.02f;


// Slack factor (effects number of balls per rope), number of pixels in between links, added to diameter for final gap distance
const float SLACK = 0.9f;
const float SLACK_GAP_EXTRA_PX = 1.f;
const float SLACK_GAP = RADIUS * 2.f + SLACK_GAP_EXTRA_PX;


// Physics constants
const float SCALE = 100.f;
const float GRAVITY = 9.8f * SCALE;
const float DAMPING = 0.7;


// How many times we check collisions and update (good so balls don't overlap in large amounts)
const int SUBSTEPS = 4;