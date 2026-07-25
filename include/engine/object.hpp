#pragma once
#include "vec2.hpp"

struct Object {
    Vec2 position;
    Vec2 previousPosition;
    float radius;
    int hue;
    bool isRope;
    bool isPinned;
    int ropeId = -1;

    Object(Vec2 position, Vec2 previousPosition, float radius, bool isRope = false, bool isPinned = false) 
        : position(position), previousPosition(previousPosition), radius(radius), isRope(isRope), isPinned(isPinned) {}
};