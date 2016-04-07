#pragma once

#include <cstdint>
#include "../grid/grid.h"


class Terrain {
public:
    Terrain(uint32_t sideSize) : mGrid(sideSize) {
    }

    void Init(int tex){
        mGrid.Init(tex);
    }

    void Draw(float time, const glm::mat4 &model = IDENTITY_MATRIX,
              const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX) {
        mGrid.Draw(time, model, view, projection);
    }

    void Cleanup(){
        mGrid.Cleanup();
    }

private:
    Grid mGrid;
};