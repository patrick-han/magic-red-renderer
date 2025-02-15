#pragma once
#include <cmath>

namespace MagicRed {
// Column vector
struct Vector4f {
    float x;
    float y;
    float z;
    float w;
    Vector4f() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    Vector4f(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
    Vector4f(float _v) : x(_v), y(_v), z(_v), w(_v) {}
    Vector4f& operator*=(float c) {
        x *= c;
        y *= c;
        z *= c;
        w *= c;
        return *this; 
    }
    Vector4f& operator/=(float c) {
        x /= c;
        y /= c;
        z /= c;
        w /= c;
        return *this; 
    }
    Vector4f& operator+=(float c) {
        x += c;
        y += c;
        z += c;
        w += c;
        return *this; 
    }
    Vector4f& operator-=(float c) {
        x -= c;
        y -= c;
        z -= c;
        w -= c;
        return *this; 
    }
    float Length() {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }
};

inline Vector4f operator+ (const Vector4f& left, const Vector4f& right) {
    Vector4f result;
    result.x = left.x + right.x;
    result.y = left.y + right.y;
    result.z = left.z + right.z;
    result.w = left.w + right.w;
    return result;
}

inline Vector4f operator- (const Vector4f& left, const Vector4f& right) {
    Vector4f result;
    result.x = left.x - right.x;
    result.y = left.y - right.y;
    result.z = left.z - right.z;
    result.w = left.w - right.w;
    return result;
}

inline Vector4f operator* (float scale, const Vector4f& right) {
    Vector4f result;
    result.x = scale * right.x;
    result.y = scale * right.y;
    result.z = scale * right.z;
    result.w = scale * right.w;
    return result;
}

inline Vector4f operator* (const Vector4f& left, float scale) {
    Vector4f result;
    result.x = scale * left.x;
    result.y = scale * left.y;
    result.z = scale * left.z;
    result.w = scale * left.w;
    return result;
}

inline float Dot(const Vector4f& left, const Vector4f& right) {
    return left.x * right.x + left.y * right.y + left.z * right.z + left.w * right.w;
}

}
