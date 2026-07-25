#include "engine/solver.hpp"
#include "engine/grid.hpp"
#include <cmath>

Solver::Solver(float gravity, float width, float height, float radius, float damping)
    : gravity(gravity), width(width), height(height), damping(damping), grid(width, height, radius) {
        acceleration = {0.f, gravity};
}

void Solver::update(Object& ball, float deltaTime) {
    if (ball.isPinned) {
        ball.previousPosition = ball.position;
        
        return;
    }

    Vec2 velocity = ball.position - ball.previousPosition;
    Vec2 newPosition = ball.position + velocity + acceleration * (deltaTime * deltaTime);

    ball.previousPosition = ball.position;
    ball.position = newPosition;

    applyConstraint(ball);
}

void Solver::applyConstraint(Object& ball) {
    Vec2 velocity = ball.position - ball.previousPosition;

    if (ball.position.x - ball.radius < 0.f) {
        ball.position.x = ball.radius;
        ball.previousPosition.x = ball.position.x + velocity.x * damping;
    } else if (ball.position.x + ball.radius > width) {
        ball.position.x = width - ball.radius;
        ball.previousPosition.x = ball.position.x + velocity.x * damping;
    }
    
    if (ball.position.y - ball.radius < 0.f) {
        ball.position.y = ball.radius;
        ball.previousPosition.y = ball.position.y + velocity.y * damping;
    } else if (ball.position.y + ball.radius > height) {
        ball.position.y = height - ball.radius;
        ball.previousPosition.y = ball.position.y + velocity.y * damping;
    }
}

void Solver::applyConstraintRope(Object& ball, Object& nextBall, float slackPx) {
    Vec2 delta = nextBall.position - ball.position;
    float distance = hypot(delta.x, delta.y);

    Vec2 normal = delta / distance;
    float maxDistance = slackPx;
    float difference = distance - maxDistance;

    if (distance == 0.f) return;

    if (ball.isPinned) {
        nextBall.position = nextBall.position - normal * difference;
    } else if (nextBall.isPinned) {
        ball.position = ball.position + normal * difference;
    } else {
        ball.position = ball.position + normal * difference * 0.5f;
        nextBall.position = nextBall.position - normal * difference * 0.5f;
    }
}

static void resolvePair(Object& ball1, Object& ball2, float radius) {
    Vec2 collisionAxis = ball1.position - ball2.position;
    float distance = hypot(collisionAxis.x, collisionAxis.y);

    if (distance < radius * 2.f && distance != 0) {
        Vec2 normal = collisionAxis / distance;
        float delta = radius * 2.f - distance;

        if (ball1.isPinned) {
            ball2.position = ball2.position - normal * delta;
        } else if (ball2.isPinned) {
            ball1.position = ball1.position + normal * delta;
        } else {
            ball1.position = ball1.position + normal * delta * 0.5f;
            ball2.position = ball2.position - normal * delta * 0.5f;
        }
    }
}

void Solver::resolveCollision(std::vector<Object>& balls, float radius) {
    grid.build(balls);

    const int dx[] = {0, 1, 0, 1, -1};
    const int dy[] = {0, 0, 1, 1, 1};


    for (int y = 0; y < grid.rows; y++) {
        for (int x = 0; x < grid.columns; x++) {
            auto& cellA = grid.cells[grid.index(x, y)];

            for (int dir = 0; dir < 5; dir++) {
                int nx = x + dx[dir];
                int ny = y + dy[dir];

                if (nx < 0 || nx >= grid.columns || ny < 0 || ny >= grid.rows) continue;

                auto& cellB = grid.cells[grid.index(nx, ny)];

                if (dir == 0) {
                    for (int i = 0; i < cellA.size(); i++) {
                        for (int j = i + 1; j < cellA.size(); j++) {
                            resolvePair(balls[cellA[i]], balls[cellA[j]], radius);
                        }
                    }
                } else {
                    for (int i : cellA) {
                        for (int j : cellB) {
                            resolvePair(balls[i], balls[j], radius);
                        }
                    }
                }
            }
        }
    }
}

void Solver::resolveRectCollision(Object& ball, const Rectangle& rectangle) {
    float halfW = rectangle.width * 0.5f;
    float halfH = rectangle.height * 0.5f;

    float closestX = std::clamp(ball.position.x, rectangle.position.x - halfW, rectangle.position.x + halfW);
    float closestY = std::clamp(ball.position.y, rectangle.position.y - halfH, rectangle.position.y + halfH);

    Vec2 delta = ball.position - Vec2{closestX, closestY};
    float distance = hypot(delta.x, delta.y);

    if (distance < ball.radius && distance > 0.f) {
        Vec2 normal = delta / distance;
        float push = ball.radius - distance;
        ball.position = ball.position + normal * push;
    }
}