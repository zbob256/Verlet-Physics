#pragma once
#include "object.hpp"
#include "vec2.hpp"
#include "grid.hpp"
#include "engine/rectangle.hpp"
#include <vector>

struct Solver {
    float gravity;
    float width;
    float height;
    float damping;
    Vec2 acceleration;
    Grid grid;

    Solver(float gravity, float width, float height, float radius, float damping);

    void update(Object& ball, float deltaTime);
    void applyConstraint(Object& ball);
    void applyConstraintRope(Object& ball, Object& nextBall, float slack);
    void resolveCollision(std::vector<Object>& balls, float radius);
    void resolveRectCollision(Object& ball, const Rectangle& rectangle);
};