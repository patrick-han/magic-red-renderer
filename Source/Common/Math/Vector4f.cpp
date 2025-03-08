#pragma once
#include <cmath>
#include "Vector4f.h"
#include "Vector3f.h"

namespace MagicRed {

Vector4f::Vector4f() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
Vector4f::Vector4f(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
Vector4f::Vector4f(float _v) : x(_v), y(_v), z(_v), w(_v) {}
Vector4f::Vector4f(const Vector3f& _vec, float _w) : x(_vec.x), y(_vec.y), z(_vec.z), w(_w) {}
Vector4f& Vector4f::operator*=(float c) {
    x *= c;
    y *= c;
    z *= c;
    w *= c;
    return *this; 
}
Vector4f& Vector4f::operator/=(float c) {
    x /= c;
    y /= c;
    z /= c;
    w /= c;
    return *this; 
}
Vector4f& Vector4f::operator+=(float c) {
    x += c;
    y += c;
    z += c;
    w += c;
    return *this; 
}
Vector4f& Vector4f::operator-=(float c) {
    x -= c;
    y -= c;
    z -= c;
    w -= c;
    return *this; 
}
float Vector4f::Length() {
    return sqrt(x * x + y * y + z * z + w * w);
}
Vector4f Vector4f::AsNormalized() {
    float mag = Length();
    Vector4f ret = *this / mag;
    return ret;
}

}
