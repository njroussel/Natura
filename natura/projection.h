#pragma once

class Projection {

public :

    Projection(float fovy, float aspect, float near, float far) {
        mFovy = fovy;
        mNear = near;
        mFar = far;
        mAspect = aspect;
        reGenerateMatrix(aspect);
    }

    mat4 reGenerateMatrix(float aspect) {
        mAspect = aspect;
        mat4 projection = IDENTITY_MATRIX;
        float width = tan(mFovy / 2.0f) * mNear * 2 * mAspect;
        float right = width / 2.0f;
        float height = width / mAspect;
        float top = height / 2.0f;
        projection[0][0] = mNear / right;
        projection[1][1] = mNear / top;
        projection[2][2] = -(mFar + mNear) / (mFar - mNear);
        projection[3][3] = 0;
        projection[3][2] = -2 * mFar * mNear / (mFar - mNear);
        projection[2][3] = -1;
        mProjection = projection;
    }

    mat4 perspective() {
        return mProjection;
    }


private:

    mat4 mProjection;
    float mFovy;
    float mAspect;
    float mNear;
    float mFar;
};


