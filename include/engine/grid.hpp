#pragma once
#include "vec2.hpp"
#include <vector>
#include <algorithm>

struct Grid {
    int columns;
    int rows;
    float cellSize;
    std::vector<std::vector<int>> cells;

    Grid(float width, float height, float radius) {
        cellSize = radius * 2.f;
        columns = static_cast<int>(width / cellSize) + 1;
        rows = static_cast<int>(height / cellSize) + 1;
        cells.resize(columns * rows);
    }

    int index(int cellX, int cellY) {
        return cellY * columns + cellX;
    }

    void build(std::vector<Object>& balls) {
        for (auto& cell : cells) {
            cell.clear();
        }

        for (int i = 0; i < static_cast<int>(balls.size()); i++) {
            int x = std::clamp(static_cast<int>(balls[i].position.x / cellSize), 0, columns - 1);
            int y = std::clamp(static_cast<int>(balls[i].position.y / cellSize), 0, rows - 1);

            cells[index(x, y)].push_back(i);
        }
    }
};