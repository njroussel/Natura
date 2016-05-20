#pragma once

#include <vector>
#include "../../../../../../../../../../usr/include/c++/4.9/bits/stl_bvector.h"
#include "../../../external/glm/detail/type_vec.hpp"
#include "../../../../../../../../../../usr/include/c++/4.9/bits/stl_vector.h"

class BezierCurve {
public:
    BezierCurve(std::vector control_points){
        m_control_points = control_points;
    }

    glm::vec3 getPosition(float time){
        return glm::vec3(0.f, 0.f, 0.f);
    }

private:
    std::vector<glm::vec3> m_control_points;
};