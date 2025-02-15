#pragma once
#include <cmath>

namespace MagicRed {
// Column vector
struct Vector3f {
    float x;
    float y;
    float z;
    Vector3f() : x(0.0f), y(0.0f), z(0.0f) {}
    Vector3f(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    Vector3f(float _v) : x(_v), y(_v), z(_v) {}
    Vector3f& operator*=(float c) {
        x *= c;
        y *= c;
        z *= c;
        return *this; 
    }
    Vector3f& operator/=(float c) {
        x /= c;
        y /= c;
        z /= c;
        return *this; 
    }
    Vector3f& operator+=(float c) {
        x += c;
        y += c;
        z += c;
        return *this; 
    }
    Vector3f& operator-=(float c) {
        x -= c;
        y -= c;
        z -= c;
        return *this; 
    }
    float Length() {
        return std::sqrt(x * x + y * y + z * z);
    }
};

inline Vector3f operator+ (const Vector3f& left, const Vector3f& right) {
    Vector3f result;
    result.x = left.x + right.x;
    result.y = left.y + right.y;
    result.z = left.z + right.z;
    return result;
}

inline Vector3f operator- (const Vector3f& left, const Vector3f& right) {
    Vector3f result;
    result.x = left.x - right.x;
    result.y = left.y - right.y;
    result.z = left.z - right.z;
    return result;
}

inline Vector3f operator* (float scale, const Vector3f& right) {
    Vector3f result;
    result.x = scale * right.x;
    result.y = scale * right.y;
    result.z = scale * right.z;
    return result;
}

inline Vector3f operator* (const Vector3f& left, float scale) {
    Vector3f result;
    result.x = scale * left.x;
    result.y = scale * left.y;
    result.z = scale * left.z;
    return result;
}

inline float Dot(const Vector3f& left, const Vector3f& right) {
    return left.x * right.x + left.y * right.y + left.z * right.z;
}

}
