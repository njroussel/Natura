#ifndef NATURA_TERRAIN_H
#define NATURA_TERRAIN_H

#include <cstdint>
#include "../grid/grid.h"


class Terrain {
public:
    Terrain(uint32_t size_x, uint32_t size_y) : mGrid() {
        this->size_x_ = size_x;
        this->size_y_ = size_y;
    }

    void Init(){
        // TODO : Init gid
        mGrid.Init();
    }

    void Draw(float time, const glm::mat4 &model = IDENTITY_MATRIX,
              const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX) {
        mGrid.Draw(time, model, view, projection);
    }

private:
    uint32_t size_x_;
    uint32_t size_y_;
    Grid mGrid;
};


#endif //NATURA_TERRAIN_H
