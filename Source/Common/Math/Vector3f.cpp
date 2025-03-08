#include "Vector3f.h"
#include <cmath>

namespace MagicRed {
Vector3f::Vector3f() : x(0.0f), y(0.0f), z(0.0f) {}
Vector3f::Vector3f(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
Vector3f::Vector3f(float _v) : x(_v), y(_v), z(_v) {}
Vector3f& Vector3f::operator*=(float c) {
    x *= c;
    y *= c;
    z *= c;
    return *this; 
}
Vector3f& Vector3f::operator/=(float c) {
    x /= c;
    y /= c;
    z /= c;
    return *this; 
}
Vector3f& Vector3f::operator+=(float c) {
    x += c;
    y += c;
    z += c;
    return *this; 
}
Vector3f& Vector3f::operator-=(float c) {
    x -= c;
    y -= c;
    z -= c;
    return *this; 
}
Vector3f& Vector3f::operator+=(const Vector3f& right) {
    x += right.x;
    y += right.y;
    z += right.z;
    return *this; 
}
Vector3f& Vector3f::operator-=(const Vector3f& right) {
    x -= right.x;
    y -= right.y;
    z -= right.z;
    return *this; 
}
float Vector3f::Length() {
    return sqrt(x * x + y * y + z * z);
}
Vector3f Vector3f::AsNormalized() {
    float mag = Length();
    Vector3f ret = *this / mag;
    return ret;
}

std::vector<Vector3f> Vector3f::GramSchmidt(const std::vector<Vector3f>& vlist, bool normalize) {
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

    if (normalize) {
        for (Vector3f& u : ulist) {
            u = u.AsNormalized();
        }
    }

    return ulist;
}

}