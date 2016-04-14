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

    /**
     * Refresh the terrain grid, takes the new texture Id of the new perlin noise.
     */
    void Refresh(int tex_id){
        mGrid.setTextureId(tex_id);
    }

    void Draw(float amplitude, float time, const glm::mat4 &model = IDENTITY_MATRIX,
              const glm::mat4 &view = IDENTITY_MATRIX,
              const glm::mat4 &projection = IDENTITY_MATRIX) {
        mGrid.Draw(amplitude, time, model, view, projection);
    }

    void Cleanup(){
        mGrid.Cleanup();
    }

private:
    Grid mGrid;
};