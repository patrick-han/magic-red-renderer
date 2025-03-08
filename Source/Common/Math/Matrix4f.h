#pragma once
#include "Vector4f.h"

namespace MagicRed {

struct Vector3f;

class Matrix4f {
public:
    // Row major storage
    float m00, m01, m02, m03
        , m10, m11, m12, m13
        , m20, m21, m22, m23
        , m30, m31, m32, m33;
    void Identity();
    Matrix4f();
    Matrix4f(
          float _m00, float _m01, float _m02, float _m03
        , float _m10, float _m11, float _m12, float _m13
        , float _m20, float _m21, float _m22, float _m23
        , float _m30, float _m31, float _m32, float _m33);

    static Matrix4f FromColumns(const Vector4f& c1, const Vector4f& c2, const Vector4f& c3, const Vector4f& c4);

    static Matrix4f FromRows(const Vector4f& r1, const Vector4f& r2, const Vector4f& r3, const Vector4f& r4);

    static Matrix4f MakeScale(float s);
    static Matrix4f MakeScale(float sx, float sy, float sz);
    static Matrix4f MakeScale(const Vector3f& scale);

    static Matrix4f MakeTranslate(float tx, float ty, float tz);
    static Matrix4f MakeTranslate(const Vector3f& translate);

    // Right handed, CCW is a positive rotation
    static Matrix4f MakeRotateX(float rads);
    static Matrix4f MakeRotateY(float rads);
    static Matrix4f MakeRotateZ(float rads);

    Matrix4f Transposed();

    // Inverse of a transform matrix assuming no deformation
    // Mainly this can be used to compute the View matrix (world-to-camera) from the camera's world matrix (camera-to-world)
    Matrix4f InvertedRigid();

    float Trace();
};

inline Vector4f operator* (const Matrix4f& mat, const Vector4f& v) {
    Vector4f res;
    res.x = v.x * mat.m00 + v.y * mat.m01 + v.z * mat.m02 + v.w * mat.m03;
    res.y = v.x * mat.m10 + v.y * mat.m11 + v.z * mat.m12 + v.w * mat.m13;
    res.z = v.x * mat.m20 + v.y * mat.m21 + v.z * mat.m22 + v.w * mat.m23;
    res.w = v.x * mat.m30 + v.y * mat.m31 + v.z * mat.m32 + v.w * mat.m33;
    return res;
}

inline Matrix4f operator* (const Matrix4f& mat1, const Matrix4f& mat2) {
    return Matrix4f(
        // Row 0
        mat1.m00 * mat2.m00 + mat1.m01 * mat2.m10 + mat1.m02 * mat2.m20 + mat1.m03 * mat2.m30,
        mat1.m00 * mat2.m01 + mat1.m01 * mat2.m11 + mat1.m02 * mat2.m21 + mat1.m03 * mat2.m31,
        mat1.m00 * mat2.m02 + mat1.m01 * mat2.m12 + mat1.m02 * mat2.m22 + mat1.m03 * mat2.m32,
        mat1.m00 * mat2.m03 + mat1.m01 * mat2.m13 + mat1.m02 * mat2.m23 + mat1.m03 * mat2.m33,
        // Row 1
        mat1.m10 * mat2.m00 + mat1.m11 * mat2.m10 + mat1.m12 * mat2.m20 + mat1.m13 * mat2.m30,
        mat1.m10 * mat2.m01 + mat1.m11 * mat2.m11 + mat1.m12 * mat2.m21 + mat1.m13 * mat2.m31,
        mat1.m10 * mat2.m02 + mat1.m11 * mat2.m12 + mat1.m12 * mat2.m22 + mat1.m13 * mat2.m32,
        mat1.m10 * mat2.m03 + mat1.m11 * mat2.m13 + mat1.m12 * mat2.m23 + mat1.m13 * mat2.m33,
        // Row 2
        mat1.m20 * mat2.m00 + mat1.m21 * mat2.m10 + mat1.m22 * mat2.m20 + mat1.m23 * mat2.m30,
        mat1.m20 * mat2.m01 + mat1.m21 * mat2.m11 + mat1.m22 * mat2.m21 + mat1.m23 * mat2.m31,
        mat1.m20 * mat2.m02 + mat1.m21 * mat2.m12 + mat1.m22 * mat2.m22 + mat1.m23 * mat2.m32,
        mat1.m20 * mat2.m03 + mat1.m21 * mat2.m13 + mat1.m22 * mat2.m23 + mat1.m23 * mat2.m33,
        // Row 3
        mat1.m30 * mat2.m00 + mat1.m31 * mat2.m10 + mat1.m32 * mat2.m20 + mat1.m33 * mat2.m30,
        mat1.m30 * mat2.m01 + mat1.m31 * mat2.m11 + mat1.m32 * mat2.m21 + mat1.m33 * mat2.m31,
        mat1.m30 * mat2.m02 + mat1.m31 * mat2.m12 + mat1.m32 * mat2.m22 + mat1.m33 * mat2.m32,
        mat1.m30 * mat2.m03 + mat1.m31 * mat2.m13 + mat1.m32 * mat2.m23 + mat1.m33 * mat2.m33
    );
}

}
