#pragma once

#include <vector>
#include "../../../../../../../../../../usr/include/c++/4.9/bits/stl_bvector.h"
#include "../../../external/glm/detail/type_vec.hpp"
#include "../../../../../../../../../../usr/include/c++/4.9/bits/stl_vector.h"

class BezierCurve {
public:
    BezierCurve(std::vector<glm::vec3> control_points){
        m_control_points = control_points;
    }

    glm::vec3 getPosition(float time){
        size_t n = m_control_points.size()-1;
        glm::vec3 res = glm::vec3(0, 0, 0);
        for (size_t j = 0 ; j <= n ; j ++){
            res += m_control_points[j]*B(time, n, j);
        }
        return res;
    }

private:
    std::vector<glm::vec3> m_control_points;

    float B(float t, int n, int i){
        if (i == 0 && n == 0)
            return 1.f;
        else if (!(0 <= i && i <= n))
            return 0.f;
        else {
            return (1.f - t) * B(t, n-1, i) + t * B(t, n-1, i-1);
        }
    }
};