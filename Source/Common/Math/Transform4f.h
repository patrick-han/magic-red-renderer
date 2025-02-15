#pragma once

#include "Vector3f.h"

namespace MagicRed {
class Transform4f {
public:
    // Right handed
    // +X is left
    // +Y is up
    // +Z is forward
    // Row major storage
    float m00, m01, m02, m03
        , m10, m11, m12, m13
        , m20, m21, m22, m23;
        // , m30, m31, m32, m33; // TODO: Technically this last row is always 0 0 0 1, can remove it later
    void Identity() {
        m00 = 1.0f; m01 = 0.0f; m02 = 0.0f; m03 = 0.0f;
        m10 = 0.0f; m11 = 1.0f; m12 = 0.0f; m13 = 0.0f;
        m20 = 0.0f; m21 = 0.0f; m22 = 1.0f; m23 = 0.0f;
        // m30 = 0.0f; m31 = 0.0f; m32 = 0.0f; m33 = 1.0f;
    }
    Transform4f() {
        Identity();
    }
    Transform4f(
          float _m00, float _m01, float _m02, float _m03
        , float _m10, float _m11, float _m12, float _m13
        , float _m20, float _m21, float _m22, float _m23
        // , float _m30, float _m31, float _m32, float _m33
    )
        : m00(_m00), m01(_m01), m02(_m02), m03(_m03)
        , m10(_m10), m11(_m11), m12(_m12), m13(_m13)
        , m20(_m20), m21(_m21), m22(_m22), m23(_m23)
        // , m30(_m30), m31(_m31), m32(_m32), m33(_m33)
    {}
    void SetLeft(const Vector3f& _left) {
        m00 = _left.x;
        m10 = _left.y;
        m20 = _left.z;
    }
    void SetUp(const Vector3f& _up) {
        m01 = _up.x;
        m11 = _up.y;
        m21 = _up.z;
    }
    void SetForward(const Vector3f& _forward) {
        m02 = _forward.x;
        m12 = _forward.y;
        m22 = _forward.z;
    }
    void SetTranslation(const Vector3f& _position) {
        m03 = _position.x;
        m13 = _position.y;
        m23 = _position.z;
    }
    Vector3f GetLeft() {
        Vector3f ret(m00, m10, m20);
        return ret;
    }
    Vector3f GetUp() {
        Vector3f ret(m01, m11, m21);
        return ret;
    }
    Vector3f GetForward() {
        Vector3f ret(m02, m12, m22);
        return ret;
    }
    Vector3f GetTranslation() {
        Vector3f ret(m03, m13, m23);
        return ret;
    }
};
}
