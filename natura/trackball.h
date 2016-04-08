#pragma once

#include <complex>
#include "icg_helper.h"


using namespace glm;

class Trackball {
public:
    Trackball() : radius_(1.0f) { }

    // this function is called when the user presses the left mouse button down.
    // x, and y are in [-1, 1]. (-1, -1) is the bottom left corner while (1, 1)
    // is the top right corner.
    void BeginDrag(float x, float y) {
        anchor_pos_ = vec3(x, y, 0.0f);
        ProjectOntoSurface(anchor_pos_);
        old_matrix_ = matrix_;
    }

    // this function is called while the user moves the curser around while the
    // left mouse button is still pressed.
    // x, and y are in [-1, 1]. (-1, -1) is the bottom left corner while (1, 1)
    // is the top right corner.
    // returns the rotation of the trackball in matrix form.
    mat4 Drag(float x, float y) {
        vec3 current_pos = vec3(x, y, 0.0f);
        ProjectOntoSurface(current_pos);

        mat4 rotation = IDENTITY_MATRIX;
        vec3 v1 = normalize(anchor_pos_);
        vec3 v2 = normalize(current_pos);
        vec3 axis = cross(v1, v2);
        float factor = 1.0f;
        float theta = acos(dot(v1, v2));
        rotation = glm::rotate(rotation, factor * theta, axis);
        return rotation;
    }

    mat4 recomputeMatrixAfterDrag(float x, float y) {
        matrix_ = Drag(x, y) * old_matrix_;
    }

    mat4 matrix() {
        return matrix_;
    }

private:
    // projects the point p (whose z coordiante is still empty/zero) onto the
    // trackball surface. If the position at the mouse cursor is outside the
    // trackball, use a hyberbolic sheet as explained in:
    // https://www.opengl.org/wiki/Object_Mouse_Trackball.
    // The trackball radius is given by 'radius_'.
    void ProjectOntoSurface(vec3 &p) const {
        float norm = sqrt(p[0] * p[0] + p[1] * p[1]);
        if (norm <= radius_ * radius_ / 2.0f) {
            p[2] = sqrt(radius_ * radius_ - (p[0] * p[0] + p[1] * p[1]));
        } else {
            p[2] = ((radius_ * radius_) / 2.0f) / sqrt(p[0] * p[0] + p[1] * p[1]);
        }
    }

    float radius_;
    vec3 anchor_pos_;
    mat4 matrix_ = IDENTITY_MATRIX;
    mat4 old_matrix_ = IDENTITY_MATRIX;
};
