#pragma once
#include <cmath>
#include <vector>

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
    Vector3f& operator+=(const Vector3f& right) {
        x += right.x;
        y += right.y;
        z += right.z;
        return *this; 
    }
    Vector3f& operator-=(const Vector3f& right) {
        x -= right.x;
        y -= right.y;
        z -= right.z;
        return *this; 
    }
    float Length() {
        return sqrt(x * x + y * y + z * z);
    }
    Vector3f AsNormalized();
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

inline Vector3f operator/ (const Vector3f& left, float div) {
    Vector3f result;
    result.x = left.x / div;
    result.y = left.y / div;
    result.z = left.z / div;
    return result;
}

inline Vector3f Vector3f::AsNormalized() {
    float mag = Length();
    Vector3f ret = *this / mag;
    return ret;
}

inline float Dot(const Vector3f& left, const Vector3f& right) {
    return left.x * right.x + left.y * right.y + left.z * right.z;
}

inline Vector3f Cross(const Vector3f& left, const Vector3f& right) {
    return Vector3f(
          left.y * right.z - left.z * right.y
        , left.z * right.x - left.x * right.z
        , left.x * right.y - left.y * right.x
    );
}

inline Vector3f Project(const Vector3f& a, const Vector3f& b) {
    // a onto b
    return (Dot(a, b) / Dot(b, b)) * b;
}

inline Vector3f Reject(const Vector3f& a, const Vector3f& b) {
    return a - Project(a, b);
}

inline Vector3f ProjectNorm(const Vector3f& a, const Vector3f& b) {
    // Assumes that b is normalized
    return Dot(a, b) * b;
}

inline Vector3f RejectNorm(const Vector3f& a, const Vector3f& b) {
    return a - ProjectNorm(a, b);
}

inline std::vector<Vector3f> GramSchmidt(const std::vector<Vector3f>& vlist, bool normalize) {
    // Takes in a list of vectors (assumed to be linearly independent) and orthogonalizes them
    std::vector<Vector3f> ulist;

    for (size_t i = 0; i < vlist.size(); i++) {
        Vector3f vsum = Vector3f(0.0f);
        for (size_t k = 0; k < i; k++) {
            vsum += Project(vlist[i], ulist[k]);
        }
        Vector3f u_i = vlist[i] - vsum;

        ulist.push_back(u_i);
    }

    for (Vector3f& u : ulist) {
        u = u.AsNormalized();
    }

    return ulist;
}

}
