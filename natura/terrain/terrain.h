#ifndef NATURA_TERRAIN_H
#define NATURA_TERRAIN_H

#include <cstdint>
#include "../grid/grid.h"


class Terrain {
public:
    Terrain(uint32_t sideSize) : mGrid(sideSize) {
        this->mSideSize = sideSize;
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
    uint32_t mSideSize;
    Grid mGrid;
};


#endif //NATURA_TERRAIN_H
