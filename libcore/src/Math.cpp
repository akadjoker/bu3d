
#include "Math.hpp"
#include "Core.hpp"

Vec3 operator*(float scalar, const Vec3 &vec)
{
    return Vec3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
}

void Transform(const Mat4 &mat, const Vec3 &vec, Vec3 &out)
{
    Vec4 temp(vec.x, vec.y, vec.z, 1.0f); // Adiciona w = 1.0 para calcular translação
    Vec4 result = mat * temp;             // Mat4 * Vec4
    out.x = result.x;
    out.y = result.y;
    out.z = result.z;
}

void Transform(const Mat4 &mat, const Vec4 &vec, Vec4 &out)
{
    out = mat * vec;
}

void Transform(const Mat3 &mat, const Vec3 &vec, Vec3 &out)
{
    out.x = vec.x * mat.at(0, 0) + vec.y * mat.at(1, 0) + vec.z * mat.at(2, 0) + mat.at(3, 0);
    out.y = vec.y * mat.at(0, 1) + vec.y * mat.at(1, 1) + vec.z * mat.at(2, 1) + mat.at(3, 1);
    out.z = vec.z * mat.at(0, 2) + vec.z * mat.at(1, 2) + vec.z * mat.at(2, 2) + mat.at(3, 2);
}

void TransformNormal(const Mat4 &mat, const Vec3 &vec, Vec3 &out)
{
    out.x = vec.x * mat.at(0, 0) + vec.y * mat.at(1, 0) + vec.z * mat.at(2, 0);
    out.y = vec.x * mat.at(0, 1) + vec.y * mat.at(1, 1) + vec.z * mat.at(2, 1);
    out.z = vec.x * mat.at(0, 2) + vec.y * mat.at(1, 2) + vec.z * mat.at(2, 2);
}

//***************************************************************************************************************
// VECTOR 2D
//***************************************************************************************************************

Vec2::Vec2() : x(0.0f), y(0.0f) {}

Vec2::Vec2(float x, float y) : x(x), y(y) {}

Vec2 Vec2::operator+(const Vec2 &other) const
{
    return Vec2(x + other.x, y + other.y);
}

Vec2 Vec2::operator-(const Vec2 &other) const
{
    return Vec2(x - other.x, y - other.y);
}

Vec2 Vec2::operator*(float scalar) const
{
    return Vec2(x * scalar, y * scalar);
}

Vec2 Vec2::operator/(float scalar) const
{
    return Vec2(x / scalar, y / scalar);
}

Vec2 &Vec2::operator+=(const Vec2 &other)
{
    x += other.x;
    y += other.y;
    return *this;
}

Vec2 &Vec2::operator-=(const Vec2 &other)
{
    x -= other.x;
    y -= other.y;
    return *this;
}

Vec2 &Vec2::operator*=(float scalar)
{
    x *= scalar;
    y *= scalar;
    return *this;
}

Vec2 &Vec2::operator/=(float scalar)
{
    x /= scalar;
    y /= scalar;
    return *this;
}

Vec2 Vec2::operator-() const
{
    return Vec2(-x, -y);
}

float Vec2::length() const
{
    return Sqrt(x * x + y * y);
}

float Vec2::lengthSquared() const
{
    return x * x + y * y;
}

Vec2 &Vec2::normalize()
{
    float len = length();
    if (len > 0.0f)
    {
        x /= len;
        y /= len;
    }

    return *this;
}

Vec2 Vec2::normal() const
{
    float len = length();
    return (len > 0.0f) ? *this / len : *this;
}

float Vec2::dot(const Vec2 &other) const
{
    return x * other.x + y * other.y;
}

float Vec2::angle(const Vec2 &other) const
{
    float dotProduct = this->dot(other);
    float lengths = this->length() * other.length();
    return ACosRad(dotProduct / lengths);
}

Vec2 Vec2::rotate(float radians) const
{
    float cosTheta = CosRad(radians);
    float sinTheta = SinRad(radians);
    return Vec2(
        x * cosTheta - y * sinTheta,
        x * sinTheta + y * cosTheta);
}

Vec2 Vec2::rotate(float radians, const Vec2 &center) const
{
    Vec2 translated = *this - center;
    Vec2 rotated = translated.rotate(radians);
    return rotated + center;
}

Vec2 Vec2::clamp(const Vec2 &min, const Vec2 &max) const
{
    return Vec2(
        Clamp(x, min.x, max.x),
        Clamp(y, min.y, max.y));
}

bool Vec2::operator==(const Vec2 &other) const
{
    return IsEqual(x, other.x) && IsEqual(y, other.y);
}

bool Vec2::operator!=(const Vec2 &other) const
{
    return !(*this == other);
}

float &Vec2::operator[](int index)
{
    DEBUG_BREAK_IF(index < 0 || index > 1);
    return index == 0 ? x : y;
}

const float &Vec2::operator[](int index) const
{
    DEBUG_BREAK_IF(index < 0 || index > 1);
    return index == 0 ? x : y;
}

float Vec2::Distance(const Vec2 &a, const Vec2 &b)
{
    return (a - b).length();
}

float Vec2::DistanceSquared(const Vec2 &a, const Vec2 &b)
{
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return dx * dx + dy * dy;
}

Vec2 Vec2::Lerp(const Vec2 &a, const Vec2 &b, float t)
{
    return Vec2(
        a.x + t * (b.x - a.x),
        a.y + t * (b.y - a.y));
}

Vec2 Vec2::Normal(const Vec2 &a, const Vec2 &b)
{
    Vec2 diff = b - a;
    return diff.normalize();
}

float Vec2::Angle(const Vec2 &a, const Vec2 &b)
{
    float dotProduct = a.dot(b);
    float lengths = a.length() * b.length();
    return ACosRad(dotProduct / lengths);
}

//***************************************************************************************************************
// VECTOR 3D
//***************************************************************************************************************
Vec3::Vec3() : x(0.0f), y(0.0f), z(0.0f) {}

Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

Vec3 Vec3::operator+(const Vec3 &other) const
{
    return Vec3(x + other.x, y + other.y, z + other.z);
}

Vec3 Vec3::operator-(const Vec3 &other) const
{
    return Vec3(x - other.x, y - other.y, z - other.z);
}

Vec3 Vec3::operator*(float scalar) const
{
    return Vec3(x * scalar, y * scalar, z * scalar);
}

Vec3 Vec3::operator/(float scalar) const
{
    return Vec3(x / scalar, y / scalar, z / scalar);
}

Vec3 &Vec3::operator+=(const Vec3 &other)
{
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Vec3 &Vec3::operator-=(const Vec3 &other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

Vec3 &Vec3::operator*=(float scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vec3 &Vec3::operator/=(float scalar)
{
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
}

Vec3 Vec3::operator-() const
{
    return Vec3(-x, -y, -z);
}

void Vec3::set(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

float &Vec3::operator[](int index)
{
    DEBUG_BREAK_IF(index < 0 || index > 2);
    return index == 0 ? x : (index == 1 ? y : z);
}

const float &Vec3::operator[](int index) const
{
    DEBUG_BREAK_IF(index < 0 || index > 2);
    return index == 0 ? x : (index == 1 ? y : z);
}

float Vec3::length() const
{
    return Sqrt(x * x + y * y + z * z);
}

float Vec3::lengthSquared() const
{
    return x * x + y * y + z * z;
}

Vec3 &Vec3::normalize()
{
    float len = length();
    if (len > 0.0f)
    {
        x /= len;
        y /= len;
        z /= len;
    }
    return *this;
}

Vec3 Vec3::normal() const
{
    float len = length();
    return (len > 0.0f) ? *this / len : *this;
}

float Vec3::dot(const Vec3 &other) const
{
    return x * other.x + y * other.y + z * other.z;
}

Vec3 Vec3::cross(const Vec3 &other) const
{
    return Vec3(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x);
}

Vec3 Vec3::rotate(float angle, const Vec3 &axis) const
{
    float cosTheta = CosRad(angle);
    float sinTheta = SinRad(angle);
    Vec3 u = axis.normal();

    return (*this * cosTheta) +
           (u.cross(*this) * sinTheta) +
           (u * (u.dot(*this) * (1 - cosTheta)));
}

Vec3 Vec3::project(const Vec3 &other) const
{
    float dotProduct = this->dot(other);
    float lengthSquared = other.lengthSquared();
    return other * (dotProduct / lengthSquared);
}

Vec3 Vec3::reflect(const Vec3 &normal) const
{
    return *this - normal * (2.0f * this->dot(normal));
}

bool Vec3::operator==(const Vec3 &other) const
{
    return IsEqual(x, other.x) && IsEqual(y, other.y) && IsEqual(z, other.z);
}

bool Vec3::operator!=(const Vec3 &other) const
{
    return !(*this == other);
}

float Vec3::Distance(const Vec3 &a, const Vec3 &b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return Sqrt(dx * dx + dy * dy + dz * dz);
}

float Vec3::DistanceSquared(const Vec3 &a, const Vec3 &b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return dx * dx + dy * dy + dz * dz;
}

Vec3 Vec3::Lerp(const Vec3 &a, const Vec3 &b, float t)
{
    return Vec3(
        a.x + t * (b.x - a.x),
        a.y + t * (b.y - a.y),
        a.z + t * (b.z - a.z));
}

Vec3 Vec3::Normal(const Vec3 &a, const Vec3 &b)
{
    Vec3 diff = b - a;
    return diff.normalize();
}

Vec3 Vec3::Normalize(const Vec3 &a)
{
    float aLen = a.length();
    return Vec3(a.x / aLen, a.y / aLen, a.z / aLen);
}

Vec3 Vec3::Cross(const Vec3 &a, const Vec3 &b)
{
    return a.cross(b);
}

float Vec3::Dot(const Vec3 &a, const Vec3 &b)
{
    return a.dot(b);
}

//***************************************************************************************************************
// VECTOR 4D
//***************************************************************************************************************

Vec4::Vec4() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}

Vec4::Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

Vec4 Vec4::operator+(const Vec4 &other) const
{
    return Vec4(x + other.x, y + other.y, z + other.z, w + other.w);
}

Vec4 Vec4::operator-(const Vec4 &other) const
{
    return Vec4(x - other.x, y - other.y, z - other.z, w - other.w);
}

Vec4 Vec4::operator*(float scalar) const
{
    return Vec4(x * scalar, y * scalar, z * scalar, w * scalar);
}

Vec4 Vec4::operator/(float scalar) const
{
    return Vec4(x / scalar, y / scalar, z / scalar, w / scalar);
}

Vec4 &Vec4::operator+=(const Vec4 &other)
{
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;
    return *this;
}

Vec4 &Vec4::operator-=(const Vec4 &other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
    w -= other.w;
    return *this;
}

Vec4 &Vec4::operator*=(float scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    w *= scalar;
    return *this;
}

Vec4 &Vec4::operator/=(float scalar)
{
    x /= scalar;
    y /= scalar;
    z /= scalar;
    w /= scalar;
    return *this;
}

float &Vec4::operator[](int index)
{
    switch (index)
    {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    case 3:
        return w;
    }
    return x;
}

const float &Vec4::operator[](int index) const
{
    switch (index)
    {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    case 3:
        return w;
    }
    return x;
}

Vec3 Vec4::toVec3() const
{
    return Vec3(x, y, z);
}

float Vec4::length() const
{
    return Sqrt(x * x + y * y + z * z + w * w);
}

float Vec4::lengthSquared() const
{
    return x * x + y * y + z * z + w * w;
}

Vec4 &Vec4::normalize()
{
    float len = length();
    if (len > 0.0f)
    {
        x /= len;
        y /= len;
        z /= len;
        w /= len;
    }
    return *this;
}

Vec4 Vec4::normal() const
{
    float len = length();
    return (len > 0.0f) ? *this / len : *this;
}

Vec4 Vec4::Normal(const Vec4 &a, const Vec4 &b)
{
    Vec4 diff = b - a;
    return diff.normalize();
}

bool Vec4::operator==(const Vec4 &other) const
{
    return IsEqual(x, other.x) && IsEqual(y, other.y) && IsEqual(z, other.z) && IsEqual(w, other.w);
}

bool Vec4::operator!=(const Vec4 &other) const
{
    return !(*this == other);
}

//***************************************************************************************************************
// MATRIX 3X3
//***************************************************************************************************************

Mat3::Mat3()
{
    for (int i = 0; i < 9; ++i)
        m[i] = 0.0f;
    m[0] = 1.0f;
    m[4] = 1.0f;
    m[8] = 1.0f;
}

Mat3::Mat3(float diagonal)
{
    for (int i = 0; i < 9; ++i)
        m[i] = 0.0f;
    m[0] = diagonal;
    m[4] = diagonal;
    m[8] = diagonal;
}

Mat3::Mat3(const float *elements)
{
    for (int i = 0; i < 9; ++i)
        m[i] = elements[i];
}

float &Mat3::at(int row, int col)
{
    DEBUG_BREAK_IF(row < 0 || row > 2 || col < 0 || col > 2);
    return m[row * 3 + col];
}

const float &Mat3::at(int row, int col) const
{
    DEBUG_BREAK_IF(row < 0 || row > 2 || col < 0 || col > 2);
    return m[row * 3 + col];
}

Mat3 Mat3::operator+(const Mat3 &other) const
{
    Mat3 result;
    for (int i = 0; i < 9; ++i)
        result.m[i] = m[i] + other.m[i];
    return result;
}

Mat3 Mat3::operator-(const Mat3 &other) const
{
    Mat3 result;
    for (int i = 0; i < 9; ++i)
        result.m[i] = m[i] - other.m[i];
    return result;
}

Mat3 Mat3::operator*(const Mat3 &other) const
{
    Mat3 result(0.0f);
    for (int row = 0; row < 3; ++row)
    {
        for (int col = 0; col < 3; ++col)
        {
            result.at(row, col) =
                at(row, 0) * other.at(0, col) +
                at(row, 1) * other.at(1, col) +
                at(row, 2) * other.at(2, col);
        }
    }
    return result;
}

Vec3 Mat3::operator*(const Vec3 &vec) const
{
    return Vec3(
        at(0, 0) * vec.x + at(0, 1) * vec.y + at(0, 2) * vec.z,
        at(1, 0) * vec.x + at(1, 1) * vec.y + at(1, 2) * vec.z,
        at(2, 0) * vec.x + at(2, 1) * vec.y + at(2, 2) * vec.z);
}

Mat3 Mat3::operator*(float scalar) const
{
    Mat3 result;
    for (int i = 0; i < 9; ++i)
        result.m[i] = m[i] * scalar;
    return result;
}

Mat3 &Mat3::operator*=(const Mat3 &other)
{
    *this = *this * other;
    return *this;
}

Mat3 &Mat3::operator*=(float scalar)
{
    for (int i = 0; i < 9; ++i)
        m[i] *= scalar;
    return *this;
}

Mat3 Mat3::transpose() const
{
    Mat3 result;
    for (int row = 0; row < 3; ++row)
    {
        for (int col = 0; col < 3; ++col)
        {
            result.at(row, col) = at(col, row);
        }
    }
    return result;
}

float Mat3::determinant() const
{
    return at(0, 0) * (at(1, 1) * at(2, 2) - at(1, 2) * at(2, 1)) -
           at(0, 1) * (at(1, 0) * at(2, 2) - at(1, 2) * at(2, 0)) +
           at(0, 2) * (at(1, 0) * at(2, 1) - at(1, 1) * at(2, 0));
}

Mat3 Mat3::inverse() const
{
    float det = determinant();
    Mat3 result;
    result.at(0, 0) = (at(1, 1) * at(2, 2) - at(1, 2) * at(2, 1)) / det;
    result.at(0, 1) = -(at(0, 1) * at(2, 2) - at(0, 2) * at(2, 1)) / det;
    result.at(0, 2) = (at(0, 1) * at(1, 2) - at(0, 2) * at(1, 1)) / det;
    result.at(1, 0) = -(at(1, 0) * at(2, 2) - at(1, 2) * at(2, 0)) / det;
    result.at(1, 1) = (at(0, 0) * at(2, 2) - at(0, 2) * at(2, 0)) / det;
    result.at(1, 2) = -(at(0, 0) * at(1, 2) - at(0, 2) * at(1, 0)) / det;
    result.at(2, 0) = (at(1, 0) * at(2, 1) - at(1, 1) * at(2, 0)) / det;
    result.at(2, 1) = -(at(0, 0) * at(2, 1) - at(0, 1) * at(2, 0)) / det;
    result.at(2, 2) = (at(0, 0) * at(1, 1) - at(0, 1) * at(1, 0)) / det;
    return result;
}

const float &Mat3::operator[](int row) const
{
    DEBUG_BREAK_IF(row < 0 || row > 2);
    return m[row * 3];
}

float &Mat3::operator[](int row)
{
    DEBUG_BREAK_IF(row < 0 || row > 2);
    return m[row * 3];
}

Mat3 Mat3::Identity()
{
    return Mat3(1.0f);
}

Mat3 Mat3::Scale(float x, float y)
{
    Mat3 result(1.0f);
    result.at(0, 0) = x;
    result.at(1, 1) = y;
    return result;
}

Mat3 Mat3::Rotation(float radians)
{
    Mat3 result(1.0f);
    float cosTheta = CosRad(radians);
    float sinTheta = SinRad(radians);
    result.at(0, 0) = cosTheta;
    result.at(0, 1) = -sinTheta;
    result.at(1, 0) = sinTheta;
    result.at(1, 1) = cosTheta;
    return result;
}

Mat3 Mat3::RotateX(float radians)
{
    Mat3 result(1.0f);
    float cosTheta = CosRad(radians);
    float sinTheta = SinRad(radians);
    result.at(1, 1) = cosTheta;
    result.at(1, 2) = -sinTheta;
    result.at(2, 1) = sinTheta;
    result.at(2, 2) = cosTheta;
    return result;
}

Mat3 Mat3::RotateY(float radians)
{
    Mat3 result(1.0f);
    float cosTheta = CosRad(radians);
    float sinTheta = SinRad(radians);
    result.at(0, 0) = cosTheta;
    result.at(0, 2) = sinTheta;
    result.at(2, 0) = -sinTheta;
    result.at(2, 2) = cosTheta;
    return result;
}

Mat3 Mat3::RotateZ(float radians)
{
    Mat3 result(1.0f);
    float cosTheta = CosRad(radians);
    float sinTheta = SinRad(radians);
    result.at(0, 0) = cosTheta;
    result.at(0, 1) = -sinTheta;
    result.at(1, 0) = sinTheta;
    result.at(1, 1) = cosTheta;
    return result;
}

Mat3 Mat3::Rotate(const Mat3 &mat, float radians, const Vec3 &axis)
{
    Mat3 rotation(1.0f);
    float cosTheta = CosRad(radians);
    float sinTheta = SinRad(radians);
    Vec3 normalizedAxis = axis.normal();

    float x = normalizedAxis.x;
    float y = normalizedAxis.y;
    float z = normalizedAxis.z;

    rotation.at(0, 0) = cosTheta + (1 - cosTheta) * x * x;
    rotation.at(0, 1) = (1 - cosTheta) * x * y - sinTheta * z;
    rotation.at(0, 2) = (1 - cosTheta) * x * z + sinTheta * y;

    rotation.at(1, 0) = (1 - cosTheta) * y * x + sinTheta * z;
    rotation.at(1, 1) = cosTheta + (1 - cosTheta) * y * y;
    rotation.at(1, 2) = (1 - cosTheta) * y * z - sinTheta * x;

    rotation.at(2, 0) = (1 - cosTheta) * z * x - sinTheta * y;
    rotation.at(2, 1) = (1 - cosTheta) * z * y + sinTheta * x;
    rotation.at(2, 2) = cosTheta + (1 - cosTheta) * z * z;

    return mat * rotation;
}

//***************************************************************************************************************
// MATRIX 4X4
//***************************************************************************************************************

Mat4::Mat4()
{
    m[0] = 1.0f;
    m[1] = m[2] = m[3] = m[4] = 0.0f;

    m[5] = 1.0f;
    m[6] = m[7] = m[8] = m[9] = 0.0f;

    m[10] = 1.0f;
    m[11] = m[12] = m[13] = m[14] = 0.0f;

    m[15] = 1.0f;
}

Mat4::Mat4(float diagonal)
{
    m[0] = diagonal;
    m[1] = m[2] = m[3] = m[4] = 0.0f;

    m[5] = diagonal;
    m[6] = m[7] = m[8] = m[9] = 0.0f;

    m[10] = diagonal;
    m[11] = m[12] = m[13] = m[14] = 0.0f;

    m[15] = diagonal;
}

Mat4::Mat4(const float *elements)
{
    for (int i = 0; i < 16; ++i)
        m[i] = elements[i];
}

float &Mat4::at(int row, int col)
{
    DEBUG_BREAK_IF(row < 0 || row > 3 || col < 0 || col > 3);
    return m[row * 4 + col];
}

const float &Mat4::at(int row, int col) const
{
    DEBUG_BREAK_IF(row < 0 || row > 3 || col < 0 || col > 3);
    return m[row * 4 + col];
}

const float &Mat4::operator[](int index) const
{
    DEBUG_BREAK_IF(index < 0 || index > 15);
    return m[index];
}

float &Mat4::operator[](int index)
{
    DEBUG_BREAK_IF(index < 0 || index > 15);
    return m[index];
}

void Mat4::set(float _11, float _12, float _13, float _14, float _21, float _22, float _23, float _24, float _31, float _32, float _33, float _34, float _41, float _42, float _43, float _44)
{
    m[0] = _11;
    m[1] = _12;
    m[2] = _13;
    m[3] = _14;
    m[4] = _21;
    m[5] = _22;
    m[6] = _23;
    m[7] = _24;
    m[8] = _31;
    m[9] = _32;
    m[10] = _33;
    m[11] = _34;
    m[12] = _41;
    m[13] = _42;
    m[14] = _43;
    m[15] = _44;
}

void Mat4::identity()
{
    m[0] = 1.0f;
    m[1] = m[2] = m[3] = m[4] = 0.0f;

    m[5] = 1.0f;
    m[6] = m[7] = m[8] = m[9] = 0.0f;

    m[10] = 1.0f;
    m[11] = m[12] = m[13] = m[14] = 0.0f;

    m[15] = 1.0f;
}

Mat4 Mat4::operator+(const Mat4 &other) const
{
    Mat4 result;
    for (int i = 0; i < 16; ++i)
        result.m[i] = m[i] + other.m[i];
    return result;
}

Mat4 Mat4::operator-(const Mat4 &other) const
{
    Mat4 result;
    for (int i = 0; i < 16; ++i)
        result.m[i] = m[i] - other.m[i];
    return result;
}

Mat4 Mat4::operator*(const Mat4 &other) const
{
    return Mat4::Multiply(*this, other);
}

Mat4 Mat4::operator*(float scalar) const
{
    Mat4 result;
    for (int i = 0; i < 16; ++i)
        result.m[i] = m[i] * scalar;
    return result;
}

Vec4 Mat4::operator*(const Vec4 &vec) const
{
    return Vec4(
        at(0, 0) * vec.x + at(0, 1) * vec.y + at(0, 2) * vec.z + at(0, 3) * vec.w,
        at(1, 0) * vec.x + at(1, 1) * vec.y + at(1, 2) * vec.z + at(1, 3) * vec.w,
        at(2, 0) * vec.x + at(2, 1) * vec.y + at(2, 2) * vec.z + at(2, 3) * vec.w,
        at(3, 0) * vec.x + at(3, 1) * vec.y + at(3, 2) * vec.z + at(3, 3) * vec.w);
}

Mat4 &Mat4::operator*=(const Mat4 &other)
{
    *this = Mat4::Multiply(*this, other);
    return *this;
}

Mat4 &Mat4::operator/=(const Vec3 &scale)
{
    for (int col = 0; col < 3; ++col)
    {
        at(0, col) /= scale[col];
        at(1, col) /= scale[col];
        at(2, col) /= scale[col];
    }
    return *this;
}
Mat4 &Mat4::operator/=(float scalar)
{
    for (int i = 0; i < 16; ++i)
    {
        m[i] /= scalar;
    }
    return *this;
}
Mat4 Mat4::operator/(const Vec3 &scale) const
{
    Mat4 result = *this;
    result /= scale;
    return result;
}
Mat4 Mat4::operator/(float scalar) const
{
    Mat4 result = *this;
    result /= scalar;
    return result;
}

void Mat4::translate(float x, float y, float z)
{
    m[12] += x;
    m[13] += y;
    m[14] += z;
}

void Mat4::scale(float x, float y, float z)
{
    m[0] *= x;
    m[5] *= y;
    m[10] *= z;
}

void Mat4::setRotationDegrees(const Vec3 &rotation)
{
    setRotationRadians(rotation * ToRad);
}

void Mat4::setRotationRadians(const Vec3 &rotation)
{
    const float cr = cos(rotation.x);
    const float sr = sin(rotation.x);
    const float cp = cos(rotation.y);
    const float sp = sin(rotation.y);
    const float cy = cos(rotation.z);
    const float sy = sin(rotation.z);

    m[0] = (float)(cp * cy);
    m[1] = (float)(cp * sy);
    m[2] = (float)(-sp);

    const float srsp = sr * sp;
    const float crsp = cr * sp;

    m[4] = (float)(srsp * cy - cr * sy);
    m[5] = (float)(srsp * sy + cr * cy);
    m[6] = (float)(sr * cp);

    m[8] = (float)(crsp * cy + sr * sy);
    m[9] = (float)(crsp * sy - sr * cy);
    m[10] = (float)(cr * cp);
}

void Mat4::setTranslation(const Vec3 &translation)
{
    m[12] = translation.x;
    m[13] = translation.y;
    m[14] = translation.z;
}

void Mat4::setScale(const Vec3 &scale)
{
    m[0] = scale.x;
    m[5] = scale.y;
    m[10] = scale.z;
}

Vec3 Mat4::getTranslation() const
{
    return Vec3(m[12], m[13], m[14]);
}

Vec3 Mat4::getPosition() const
{
    return Vec3(m[12], m[13], m[14]);
}

Mat3 Mat4::ToMat3(const Mat4 &mat)
{
    Mat3 result;

    result.at(0, 0) = mat.at(0, 0);
    result.at(0, 1) = mat.at(0, 1);
    result.at(0, 2) = mat.at(0, 2);
    result.at(1, 0) = mat.at(1, 0);
    result.at(1, 1) = mat.at(1, 1);
    result.at(1, 2) = mat.at(1, 2);
    result.at(2, 0) = mat.at(2, 0);
    result.at(2, 1) = mat.at(2, 1);
    result.at(2, 2) = mat.at(2, 2);

    return result;
}

Mat4 Mat4::inverse() const
{

    float a00 = m[0], a01 = m[1], a02 = m[2], a03 = m[3];
    float a10 = m[4], a11 = m[5], a12 = m[6], a13 = m[7];
    float a20 = m[8], a21 = m[9], a22 = m[10], a23 = m[11];
    float a30 = m[12], a31 = m[13], a32 = m[14], a33 = m[15];

    float b00 = a00 * a11 - a01 * a10;
    float b01 = a00 * a12 - a02 * a10;
    float b02 = a00 * a13 - a03 * a10;
    float b03 = a01 * a12 - a02 * a11;
    float b04 = a01 * a13 - a03 * a11;
    float b05 = a02 * a13 - a03 * a12;
    float b06 = a20 * a31 - a21 * a30;
    float b07 = a20 * a32 - a22 * a30;
    float b08 = a20 * a33 - a23 * a30;
    float b09 = a21 * a32 - a22 * a31;
    float b10 = a21 * a33 - a23 * a31;
    float b11 = a22 * a33 - a23 * a32;

    // Calculate the invert determinant (inlined to avoid double-caching)
    float invDet = 1.0f / (b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06);
    Mat4 result;
    result.m[0] = (a11 * b11 - a12 * b10 + a13 * b09) * invDet;
    result.m[1] = (-a01 * b11 + a02 * b10 - a03 * b09) * invDet;
    result.m[2] = (a31 * b05 - a32 * b04 + a33 * b03) * invDet;
    result.m[3] = (-a21 * b05 + a22 * b04 - a23 * b03) * invDet;
    result.m[4] = (-a10 * b11 + a12 * b08 - a13 * b07) * invDet;
    result.m[5] = (a00 * b11 - a02 * b08 + a03 * b07) * invDet;
    result.m[6] = (-a30 * b05 + a32 * b02 - a33 * b01) * invDet;
    result.m[7] = (a20 * b05 - a22 * b02 + a23 * b01) * invDet;
    result.m[8] = (a10 * b10 - a11 * b08 + a13 * b06) * invDet;
    result.m[9] = (-a00 * b10 + a01 * b08 - a03 * b06) * invDet;
    result.m[10] = (a30 * b04 - a31 * b02 + a33 * b00) * invDet;
    result.m[11] = (-a20 * b04 + a21 * b02 - a23 * b00) * invDet;
    result.m[12] = (-a10 * b09 + a11 * b07 - a12 * b06) * invDet;
    result.m[13] = (a00 * b09 - a01 * b07 + a02 * b06) * invDet;
    result.m[14] = (-a30 * b03 + a31 * b01 - a32 * b00) * invDet;
    result.m[15] = (a20 * b03 - a21 * b01 + a22 * b00) * invDet;

    return result;
}

Vec3 Mat4::getRotationInDegrees() const
{
    Vec3 rotation;

    float sy = sqrt(m[0] * m[0] + m[4] * m[4]);

    bool singular = sy < 1e-6;

    if (!singular)
    {
        rotation.x = atan2(m[6], m[10]); // Pitch
        rotation.y = atan2(-m[2], sy);   // Yaw
        rotation.z = atan2(m[1], m[0]);  // Roll
    }
    else
    {
        rotation.x = atan2(-m[9], m[5]);
        rotation.y = atan2(-m[2], sy);
        rotation.z = 0;
    }

    rotation.x = Degrees(rotation.x);
    rotation.y = Degrees(rotation.y);
    rotation.z = Degrees(rotation.z);

    return rotation;
}

Vec3 Mat4::getRotation() const
{
    Vec3 rotation;

    float sy = sqrt(m[0] * m[0] + m[4] * m[4]);

    bool singular = sy < 1e-6;

    if (!singular)
    {
        rotation.x = atan2(m[6], m[10]); // Pitch
        rotation.y = atan2(-m[2], sy);   // Yaw
        rotation.z = atan2(m[1], m[0]);  // Roll
    }
    else
    {
        rotation.x = atan2(-m[9], m[5]);
        rotation.y = atan2(-m[2], sy);
        rotation.z = 0;
    }

    return rotation;
}

Mat4 Mat4::Identity()
{
    return Mat4(1.0f);
}

Mat4 Mat4::Translate(const Vec3 &translation)
{
    Mat4 result;
    result.translate(translation.x, translation.y, translation.z);
    return result;
}
Mat4 Mat4::Scale(const Vec3 &scale)
{
    Mat4 result;
    result.scale(scale.x, scale.y, scale.z);
    return result;
}
Mat4 Mat4::Rotate(float radians, const Vec3 &axis)
{
    Mat4 result;

    float x = axis.x, y = axis.y, z = axis.z;

    float lengthSquared = x * x + y * y + z * z;

    if ((lengthSquared != 1.0f) && (lengthSquared != 0.0f))
    {
        float ilength = 1.0f / sqrtf(lengthSquared);
        x *= ilength;
        y *= ilength;
        z *= ilength;
    }

    float sinres = sinf(radians);
    float cosres = cosf(radians);
    float t = 1.0f - cosres;

    result.m[0] = x * x * t + cosres;
    result.m[1] = y * x * t + z * sinres;
    result.m[2] = z * x * t - y * sinres;
    result.m[3] = 0.0f;

    result.m[4] = x * y * t - z * sinres;
    result.m[5] = y * y * t + cosres;
    result.m[6] = z * y * t + x * sinres;
    result.m[7] = 0.0f;

    result.m[8] = x * z * t + y * sinres;
    result.m[9] = y * z * t - x * sinres;
    result.m[10] = z * z * t + cosres;
    result.m[11] = 0.0f;

    result.m[12] = 0.0f;
    result.m[13] = 0.0f;
    result.m[14] = 0.0f;
    result.m[15] = 1.0f;

    return result;
}

Mat4 Mat4::Perspective(float fovY, float aspect, float nearPlane, float farPlane)
{
    float top = nearPlane * tan(fovY * 0.5);
    float bottom = -top;
    float right = top * aspect;
    float left = -right;

    Mat4 result;
    float rl = (float)(right - left);
    float tb = (float)(top - bottom);
    float fn = (float)(farPlane - nearPlane);

    result.m[0] = (nearPlane * 2.0f) / rl;
    result.m[5] = (nearPlane * 2.0f) / tb;
    result.m[8] = (right + left) / rl;
    result.m[9] = (top + bottom) / tb;
    result.m[10] = -(farPlane + nearPlane) / fn;
    result.m[11] = -1.0f;
    result.m[14] = -(farPlane * nearPlane * 2.0f) / fn;

    return result;
}

Mat4 Mat4::Orthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane)
{
    float rl = (float)(right - left);
    float tb = (float)(top - bottom);
    float fn = (float)(farPlane - nearPlane);

    Mat4 result;
    result.m[0] = 2.0f / rl;
    result.m[1] = 0.0f;
    result.m[2] = 0.0f;
    result.m[3] = 0.0f;
    result.m[4] = 0.0f;
    result.m[5] = 2.0f / tb;
    result.m[6] = 0.0f;
    result.m[7] = 0.0f;
    result.m[8] = 0.0f;
    result.m[9] = 0.0f;
    result.m[10] = -2.0f / fn;
    result.m[11] = 0.0f;
    result.m[12] = -((float)left + (float)right) / rl;
    result.m[13] = -((float)top + (float)bottom) / tb;
    result.m[14] = -((float)farPlane + (float)nearPlane) / fn;
    result.m[15] = 1.0f;

    return result;
}

Mat4 Mat4::LookAt(const Vec3 &eye, const Vec3 &target, const Vec3 &up)
{
    Mat4 result;

    float length = 0.0f;
    float ilength = 0.0f;

    Vec3 vz = {eye.x - target.x, eye.y - target.y, eye.z - target.z};

    Vec3 v = vz;
    length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length == 0.0f)
        length = 1.0f;
    ilength = 1.0f / length;
    vz.x *= ilength;
    vz.y *= ilength;
    vz.z *= ilength;

    Vec3 vx = {up.y * vz.z - up.z * vz.y, up.z * vz.x - up.x * vz.z, up.x * vz.y - up.y * vz.x};

    v = vx;
    length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length == 0.0f)
        length = 1.0f;
    ilength = 1.0f / length;
    vx.x *= ilength;
    vx.y *= ilength;
    vx.z *= ilength;

    Vec3 vy = {vz.y * vx.z - vz.z * vx.y, vz.z * vx.x - vz.x * vx.z, vz.x * vx.y - vz.y * vx.x};

    result.m[0] = vx.x;
    result.m[1] = vy.x;
    result.m[2] = vz.x;
    result.m[3] = 0.0f;
    result.m[4] = vx.y;
    result.m[5] = vy.y;
    result.m[6] = vz.y;
    result.m[7] = 0.0f;
    result.m[8] = vx.z;
    result.m[9] = vy.z;
    result.m[10] = vz.z;
    result.m[11] = 0.0f;
    result.m[12] = -(vx.x * eye.x + vx.y * eye.y + vx.z * eye.z);
    result.m[13] = -(vy.x * eye.x + vy.y * eye.y + vy.z * eye.z);
    result.m[14] = -(vz.x * eye.x + vz.y * eye.y + vz.z * eye.z);
    result.m[15] = 1.0f;

    return result;
}

Mat4 Mat4::Rotate(const Mat4 &mat, float radians, const Vec3 &axis)
{
    Mat4 rotation = Rotate(radians, axis);
    return Mat4::Multiply(mat, rotation);
}

Mat4 Mat4::RotateX(const Mat4 &mat, float radians)
{
    Mat4 rotation;
    float cosres = cos(radians);
    float sinres = sin(radians);

    rotation.m[5] = cosres;
    rotation.m[6] = sinres;
    rotation.m[9] = -sinres;
    rotation.m[10] = cosres;

    return Mat4::Multiply(mat, rotation);
}

Mat4 Mat4::RotateY(const Mat4 &mat, float radians)
{
    Mat4 result;
    float cosres = cosf(radians);
    float sinres = sinf(radians);

    result.m[0] = cosres;
    result.m[2] = -sinres;
    result.m[8] = sinres;
    result.m[10] = cosres;

    return Mat4::Multiply(mat, result);
}

Mat4 Mat4::RotateZ(const Mat4 &mat, float radians)
{
    Mat4 result;
    float cosres = cosf(radians);
    float sinres = sinf(radians);

    result.m[0] = cosres;
    result.m[1] = sinres;
    result.m[4] = -sinres;
    result.m[5] = cosres;

    return Mat4::Multiply(mat, result);
}

Mat4 Mat4::Translate(const Mat4 &mat, const Vec3 &v)
{
    Mat4 translationMatrix;
    translationMatrix.translate(v.x, v.y, v.z);
    return Mat4::Multiply(mat, translationMatrix);
}

Mat4 Mat4::Translate(const Mat4 &mat, float x, float y, float z)
{
    return Translate(mat, Vec3(x, y, z));
}
Mat4 Mat4::Scale(const Mat4 &mat, const Vec3 &scale)
{
    Mat4 scaling;
    scaling.at(0, 0) = scale.x;
    scaling.at(1, 1) = scale.y;
    scaling.at(2, 2) = scale.z;

    return Mat4::Multiply(mat, scaling);
}

Mat4 Mat4::Scale(const Mat4 &mat, float x, float y, float z)
{
    return Scale(mat, Vec3(x, y, z));
}

void Mat4::transformPoint(Vec3 &v) const
{
    float x = v.x * m[0] + v.y * m[4] + v.z * m[8] + m[12];
    float y = v.x * m[1] + v.y * m[5] + v.z * m[9] + m[13];
    float z = v.x * m[2] + v.y * m[6] + v.z * m[10] + m[14];
    v.x = x;
    v.y = y;
    v.z = z;
}

void Mat4::transformNormal(Vec3 &v) const
{
    float x = v.x * m[0] + v.y * m[4] + v.z * m[8];
    float y = v.x * m[1] + v.y * m[5] + v.z * m[9];
    float z = v.x * m[2] + v.y * m[6] + v.z * m[10];
    v.x = x;
    v.y = y;
    v.z = z;
}
bool Mat4::Decompose(const Mat4 &mat, Vec3 &position, Vec3 &scale, Quat &rotation)
{
    position = Vec3(mat.at(0, 3), mat.at(1, 3), mat.at(2, 3));

    Vec3 col0(mat.at(0, 0), mat.at(1, 0), mat.at(2, 0));
    Vec3 col1(mat.at(0, 1), mat.at(1, 1), mat.at(2, 1));
    Vec3 col2(mat.at(0, 2), mat.at(1, 2), mat.at(2, 2));

    scale = Vec3(col0.length(), col1.length(), col2.length());

    if (scale.x < 0.000001f || scale.y < 0.000001f || scale.z < 0.000001f)
        return false;

    Mat3 rotMat;
    rotMat.at(0, 0) = col0.x / scale.x;
    rotMat.at(1, 0) = col0.y / scale.x;
    rotMat.at(2, 0) = col0.z / scale.x;
    rotMat.at(0, 1) = col1.x / scale.y;
    rotMat.at(1, 1) = col1.y / scale.y;
    rotMat.at(2, 1) = col1.z / scale.y;
    rotMat.at(0, 2) = col2.x / scale.z;
    rotMat.at(1, 2) = col2.y / scale.z;
    rotMat.at(2, 2) = col2.z / scale.z;

    rotation = Quat(rotMat);
    return true;
}

bool Mat4::Decompose(const Mat4 &mat, Vec3 &position, Mat3 &out)
{
    position = Vec3(mat.at(0, 3), mat.at(1, 3), mat.at(2, 3));

    Vec3 col0(mat.at(0, 0), mat.at(1, 0), mat.at(2, 0));
    Vec3 col1(mat.at(0, 1), mat.at(1, 1), mat.at(2, 1));
    Vec3 col2(mat.at(0, 2), mat.at(1, 2), mat.at(2, 2));

    if (col0.length() == 0.0f || col1.length() == 0.0f || col2.length() == 0.0f)
    {
        return false; //  matriz degenerada
    }

    col0 = col0.normal();
    col1 = col1.normal();
    col2 = col2.normal();

    out = Mat3();
    out.at(0, 0) = col0.x;
    out.at(0, 1) = col1.x;
    out.at(0, 2) = col2.x;
    out.at(1, 0) = col0.y;
    out.at(1, 1) = col1.y;
    out.at(1, 2) = col2.y;
    out.at(2, 0) = col0.z;
    out.at(2, 1) = col1.z;
    out.at(2, 2) = col2.z;

    return true;
}

Mat4 Mat4::Inverse(const Mat4 &mat)
{
    Mat4 result;

    // Cache the matrix values (speed optimization)
    float a00 = mat.m[0], a01 = mat.m[1], a02 = mat.m[2], a03 = mat.m[3];
    float a10 = mat.m[4], a11 = mat.m[5], a12 = mat.m[6], a13 = mat.m[7];
    float a20 = mat.m[8], a21 = mat.m[9], a22 = mat.m[10], a23 = mat.m[11];
    float a30 = mat.m[12], a31 = mat.m[13], a32 = mat.m[14], a33 = mat.m[15];

    float b00 = a00 * a11 - a01 * a10;
    float b01 = a00 * a12 - a02 * a10;
    float b02 = a00 * a13 - a03 * a10;
    float b03 = a01 * a12 - a02 * a11;
    float b04 = a01 * a13 - a03 * a11;
    float b05 = a02 * a13 - a03 * a12;
    float b06 = a20 * a31 - a21 * a30;
    float b07 = a20 * a32 - a22 * a30;
    float b08 = a20 * a33 - a23 * a30;
    float b09 = a21 * a32 - a22 * a31;
    float b10 = a21 * a33 - a23 * a31;
    float b11 = a22 * a33 - a23 * a32;

    // Calculate the invert determinant (inlined to avoid double-caching)
    float invDet = 1.0f / (b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06);

    result.m[0] = (a11 * b11 - a12 * b10 + a13 * b09) * invDet;
    result.m[1] = (-a01 * b11 + a02 * b10 - a03 * b09) * invDet;
    result.m[2] = (a31 * b05 - a32 * b04 + a33 * b03) * invDet;
    result.m[3] = (-a21 * b05 + a22 * b04 - a23 * b03) * invDet;
    result.m[4] = (-a10 * b11 + a12 * b08 - a13 * b07) * invDet;
    result.m[5] = (a00 * b11 - a02 * b08 + a03 * b07) * invDet;
    result.m[6] = (-a30 * b05 + a32 * b02 - a33 * b01) * invDet;
    result.m[7] = (a20 * b05 - a22 * b02 + a23 * b01) * invDet;
    result.m[8] = (a10 * b10 - a11 * b08 + a13 * b06) * invDet;
    result.m[9] = (-a00 * b10 + a01 * b08 - a03 * b06) * invDet;
    result.m[10] = (a30 * b04 - a31 * b02 + a33 * b00) * invDet;
    result.m[11] = (-a20 * b04 + a21 * b02 - a23 * b00) * invDet;
    result.m[12] = (-a10 * b09 + a11 * b07 - a12 * b06) * invDet;
    result.m[13] = (a00 * b09 - a01 * b07 + a02 * b06) * invDet;
    result.m[14] = (-a30 * b03 + a31 * b01 - a32 * b00) * invDet;
    result.m[15] = (a20 * b03 - a21 * b01 + a22 * b00) * invDet;

    return result;
}

Mat4 Mat4::Add(const Mat4 &left, const Mat4 &right)
{
    Mat4 result;

    result.m[0] = left.m[0] + right.m[0];
    result.m[1] = left.m[1] + right.m[1];
    result.m[2] = left.m[2] + right.m[2];
    result.m[3] = left.m[3] + right.m[3];
    result.m[4] = left.m[4] + right.m[4];
    result.m[5] = left.m[5] + right.m[5];
    result.m[6] = left.m[6] + right.m[6];
    result.m[7] = left.m[7] + right.m[7];
    result.m[8] = left.m[8] + right.m[8];
    result.m[9] = left.m[9] + right.m[9];
    result.m[10] = left.m[10] + right.m[10];
    result.m[11] = left.m[11] + right.m[11];
    result.m[12] = left.m[12] + right.m[12];
    result.m[13] = left.m[13] + right.m[13];
    result.m[14] = left.m[14] + right.m[14];
    result.m[15] = left.m[15] + right.m[15];

    return result;
}

Mat4 Mat4::Subtract(const Mat4 &left, const Mat4 &right)
{
    Mat4 result;

    result.m[0] = left.m[0] - right.m[0];
    result.m[1] = left.m[1] - right.m[1];
    result.m[2] = left.m[2] - right.m[2];
    result.m[3] = left.m[3] - right.m[3];
    result.m[4] = left.m[4] - right.m[4];
    result.m[5] = left.m[5] - right.m[5];
    result.m[6] = left.m[6] - right.m[6];
    result.m[7] = left.m[7] - right.m[7];
    result.m[8] = left.m[8] - right.m[8];
    result.m[9] = left.m[9] - right.m[9];
    result.m[10] = left.m[10] - right.m[10];
    result.m[11] = left.m[11] - right.m[11];
    result.m[12] = left.m[12] - right.m[12];
    result.m[13] = left.m[13] - right.m[13];
    result.m[14] = left.m[14] - right.m[14];
    result.m[15] = left.m[15] - right.m[15];

    return result;
}

Mat4 Mat4::Divide(const Mat4 &left, const Mat4 &right)
{
    Mat4 result;

    result.m[0] = left.m[0] / right.m[0];
    result.m[1] = left.m[1] / right.m[1];
    result.m[2] = left.m[2] / right.m[2];
    result.m[3] = left.m[3] / right.m[3];
    result.m[4] = left.m[4] / right.m[4];
    result.m[5] = left.m[5] / right.m[5];
    result.m[6] = left.m[6] / right.m[6];
    result.m[7] = left.m[7] / right.m[7];
    result.m[8] = left.m[8] / right.m[8];
    result.m[9] = left.m[9] / right.m[9];
    result.m[10] = left.m[10] / right.m[10];
    result.m[11] = left.m[11] / right.m[11];
    result.m[12] = left.m[12] / right.m[12];
    result.m[13] = left.m[13] / right.m[13];
    result.m[14] = left.m[14] / right.m[14];
    result.m[15] = left.m[15] / right.m[15];

    return result;
}

Mat4 Mat4::Multiply(const Mat4 &left, const Mat4 &right)
{
    Mat4 result;

    result.m[0] = left.m[0] * right.m[0] + left.m[1] * right.m[4] + left.m[2] * right.m[8] + left.m[3] * right.m[12];
    result.m[1] = left.m[0] * right.m[1] + left.m[1] * right.m[5] + left.m[2] * right.m[9] + left.m[3] * right.m[13];
    result.m[2] = left.m[0] * right.m[2] + left.m[1] * right.m[6] + left.m[2] * right.m[10] + left.m[3] * right.m[14];
    result.m[3] = left.m[0] * right.m[3] + left.m[1] * right.m[7] + left.m[2] * right.m[11] + left.m[3] * right.m[15];
    result.m[4] = left.m[4] * right.m[0] + left.m[5] * right.m[4] + left.m[6] * right.m[8] + left.m[7] * right.m[12];
    result.m[5] = left.m[4] * right.m[1] + left.m[5] * right.m[5] + left.m[6] * right.m[9] + left.m[7] * right.m[13];
    result.m[6] = left.m[4] * right.m[2] + left.m[5] * right.m[6] + left.m[6] * right.m[10] + left.m[7] * right.m[14];
    result.m[7] = left.m[4] * right.m[3] + left.m[5] * right.m[7] + left.m[6] * right.m[11] + left.m[7] * right.m[15];
    result.m[8] = left.m[8] * right.m[0] + left.m[9] * right.m[4] + left.m[10] * right.m[8] + left.m[11] * right.m[12];
    result.m[9] = left.m[8] * right.m[1] + left.m[9] * right.m[5] + left.m[10] * right.m[9] + left.m[11] * right.m[13];
    result.m[10] = left.m[8] * right.m[2] + left.m[9] * right.m[6] + left.m[10] * right.m[10] + left.m[11] * right.m[14];
    result.m[11] = left.m[8] * right.m[3] + left.m[9] * right.m[7] + left.m[10] * right.m[11] + left.m[11] * right.m[15];
    result.m[12] = left.m[12] * right.m[0] + left.m[13] * right.m[4] + left.m[14] * right.m[8] + left.m[15] * right.m[12];
    result.m[13] = left.m[12] * right.m[1] + left.m[13] * right.m[5] + left.m[14] * right.m[9] + left.m[15] * right.m[13];
    result.m[14] = left.m[12] * right.m[2] + left.m[13] * right.m[6] + left.m[14] * right.m[10] + left.m[15] * right.m[14];
    result.m[15] = left.m[12] * right.m[3] + left.m[13] * right.m[7] + left.m[14] * right.m[11] + left.m[15] * right.m[15];
    return result;
}

Mat4 Mat4::Mirror(const Vec3 &n, float d)
{
    Mat4 result;

    result.m[0] = 1.0f - 2.0f * n.x * n.x;
    result.m[1] = -2.0f * n.x * n.y;
    result.m[2] = -2.0f * n.x * n.z;
    result.m[3] = 0.0f;

    result.m[4] = -2.0f * n.x * n.y;
    result.m[5] = 1.0f - 2.0f * n.y * n.y;
    result.m[6] = -2.0f * n.y * n.z;
    result.m[7] = 0.0f;

    result.m[8] = -2.0f * n.x * n.z;
    result.m[9] = -2.0f * n.y * n.z;
    result.m[10] = 1.0f - 2.0f * n.z * n.z;
    result.m[11] = 0.0f;

    result.m[12] = 2.0f * d * n.x;
    result.m[13] = 2.0f * d * n.y;
    result.m[14] = 2.0f * d * n.z;
    result.m[15] = 1.0f;

    return result;
}

//***************************************************************************************************************
// QUATERNION
//***************************************************************************************************************

Quat::Quat() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}

Quat::Quat(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

Quat::Quat(const Vec3 &axis, float angle)
{

    Vec3 normalizedAxis = axis.normal();
    float sinHalfAngle = sin(angle / 2.0f);
    w = cos(angle / 2.0f);
    x = normalizedAxis.x * sinHalfAngle;
    y = normalizedAxis.y * sinHalfAngle;
    z = normalizedAxis.z * sinHalfAngle;
}

Quat::Quat(const Mat3 &rotationMatrix)
{
    float trace = rotationMatrix.at(0, 0) + rotationMatrix.at(1, 1) + rotationMatrix.at(2, 2);
    if (trace > 0.0f)
    {
        float s = sqrt(trace + 1.0f) * 2.0f;
        w = 0.25f * s;
        x = (rotationMatrix.at(2, 1) - rotationMatrix.at(1, 2)) / s;
        y = (rotationMatrix.at(0, 2) - rotationMatrix.at(2, 0)) / s;
        z = (rotationMatrix.at(1, 0) - rotationMatrix.at(0, 1)) / s;
    }
    else if (rotationMatrix.at(0, 0) > rotationMatrix.at(1, 1) && rotationMatrix.at(0, 0) > rotationMatrix.at(2, 2))
    {
        float s = sqrt(1.0f + rotationMatrix.at(0, 0) - rotationMatrix.at(1, 1) - rotationMatrix.at(2, 2)) * 2.0f;
        w = (rotationMatrix.at(2, 1) - rotationMatrix.at(1, 2)) / s;
        x = 0.25f * s;
        y = (rotationMatrix.at(0, 1) + rotationMatrix.at(1, 0)) / s;
        z = (rotationMatrix.at(0, 2) + rotationMatrix.at(2, 0)) / s;
    }
    else if (rotationMatrix.at(1, 1) > rotationMatrix.at(2, 2))
    {
        float s = sqrt(1.0f + rotationMatrix.at(1, 1) - rotationMatrix.at(0, 0) - rotationMatrix.at(2, 2)) * 2.0f;
        w = (rotationMatrix.at(0, 2) - rotationMatrix.at(2, 0)) / s;
        x = (rotationMatrix.at(0, 1) + rotationMatrix.at(1, 0)) / s;
        y = 0.25f * s;
        z = (rotationMatrix.at(1, 2) + rotationMatrix.at(2, 1)) / s;
    }
    else
    {
        float s = sqrt(1.0f + rotationMatrix.at(2, 2) - rotationMatrix.at(0, 0) - rotationMatrix.at(1, 1)) * 2.0f;
        w = (rotationMatrix.at(1, 0) - rotationMatrix.at(0, 1)) / s;
        x = (rotationMatrix.at(0, 2) + rotationMatrix.at(2, 0)) / s;
        y = (rotationMatrix.at(1, 2) + rotationMatrix.at(2, 1)) / s;
        z = 0.25f * s;
    }
}

Quat Quat::operator+(const Quat &other) const
{
    return Quat(x + other.x, y + other.y, z + other.z, w + other.w);
}

Quat Quat::operator-(const Quat &other) const
{
    return Quat(x - other.x, y - other.y, z - other.z, w - other.w);
}

Quat Quat::operator*(const Quat &other) const
{
    return Quat(
        w * other.x + x * other.w + y * other.z - z * other.y,
        w * other.y - x * other.z + y * other.w + z * other.x,
        w * other.z + x * other.y - y * other.x + z * other.w,
        w * other.w - x * other.x - y * other.y - z * other.z);
}

Quat Quat::operator*(float scalar) const
{
    return Quat(x * scalar, y * scalar, z * scalar, w * scalar);
}
Quat &Quat::operator*=(const Quat &other)
{
    *this = *this * other;
    return *this;
}

Quat &Quat::operator*=(float scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    w *= scalar;
    return *this;
}

float Quat::length() const
{
    return sqrt(x * x + y * y + z * z + w * w);
}
Quat Quat::normal() const
{
    float len = length();
    return Quat(x / len, y / len, z / len, w / len);
}

Quat &Quat::normalize()
{
    float len = length();
    if (len > 0.000001f)
    {
        float invLen = 1.0f / len;
        x *= invLen;
        y *= invLen;
        z *= invLen;
        w *= invLen;
    }
    return *this;
}

Quat Quat::conjugate() const
{
    return Quat(-x, -y, -z, w);
}

Quat Quat::inverse() const
{
    float lenSq = length() * length();
    return conjugate() * (1.0f / lenSq);
}
Vec3 Quat::euler() const
{
    Vec3 euler;

    // Pitch (x)
    float sinPitch = 2.0f * (w * x + y * z);
    float cosPitch = 1.0f - 2.0f * (x * x + y * y);
    euler.x = atan2(sinPitch, cosPitch);

    // Yaw (y)
    float sinYaw = 2.0f * (w * y - z * x);
    euler.y = fabs(sinYaw) >= 1.0f ? copysign(M_PI / 2.0f, sinYaw) : asin(sinYaw);

    // Roll (z)
    float sinRoll = 2.0f * (w * z + x * y);
    float cosRoll = 1.0f - 2.0f * (y * y + z * z);
    euler.z = atan2(sinRoll, cosRoll);

    return euler;
}

Vec3 Quat::rotate(const Vec3 &v) const
{
    Quat qv(v.x, v.y, v.z, 0.0f);
    Quat result = (*this) * qv * inverse();
    return Vec3(result.x, result.y, result.z);
}

Mat3 Quat::toMat3() const
{
    Mat3 result;

    float xx = x * x, yy = y * y, zz = z * z;
    float xy = x * y, xz = x * z, yz = y * z;
    float wx = w * x, wy = w * y, wz = w * z;

    result.at(0, 0) = 1.0f - 2.0f * (yy + zz);
    result.at(0, 1) = 2.0f * (xy - wz);
    result.at(0, 2) = 2.0f * (xz + wy);

    result.at(1, 0) = 2.0f * (xy + wz);
    result.at(1, 1) = 1.0f - 2.0f * (xx + zz);
    result.at(1, 2) = 2.0f * (yz - wx);

    result.at(2, 0) = 2.0f * (xz - wy);
    result.at(2, 1) = 2.0f * (yz + wx);
    result.at(2, 2) = 1.0f - 2.0f * (xx + yy);

    return result;
}

Mat4 Quat::toMat4() const
{
    Mat4 result = Mat4::Identity();
    Mat3 rot = toMat3();
    for (int row = 0; row < 3; ++row)
    {
        for (int col = 0; col < 3; ++col)
        {
            result.at(row, col) = rot.at(row, col);
        }
    }
    return result;
}

float Quat::yaw() const
{
    // Yaw (Y-axis rotation)
    float sinYaw = 2.0f * (w * y + z * x);
    float cosYaw = 1.0f - 2.0f * (y * y + z * z);
    return atan2(sinYaw, cosYaw);
}

float Quat::pitch() const
{

    // Pitch (X-axis rotation)
    float sinPitch = 2.0f * (w * x - y * z);
    return fabs(sinPitch) >= 1.0f ? copysign(M_PI / 2.0f, sinPitch) : asin(sinPitch);
}

float Quat::roll() const
{
    // Roll (Z-axis rotation)
    float sinRoll = 2.0f * (w * z + x * y);
    float cosRoll = 1.0f - 2.0f * (z * z + x * x);
    return atan2(sinRoll, cosRoll);
}

Quat Quat::Yaw(float angle)
{
    float halfAngle = angle * 0.5f;
    return Quat(0.0f, sin(halfAngle), 0.0f, cos(halfAngle));
}

Quat Quat::Pitch(float angle)
{
    float halfAngle = angle * 0.5f;
    return Quat(sin(halfAngle), 0.0f, 0.0f, cos(halfAngle));
}

Quat Quat::Roll(float angle)
{
    float halfAngle = angle * 0.5f;
    return Quat(0.0f, 0.0f, sin(halfAngle), cos(halfAngle));
}

Quat Quat::FromYawPitchRoll(float yaw, float pitch, float roll)
{
    Quat qYaw = Yaw(yaw);
    Quat qPitch = Pitch(pitch);
    Quat qRoll = Roll(roll);
    return qYaw * qPitch * qRoll;
}

Quat Quat::Slerp(const Quat &start, const Quat &end, float t)
{
    float dot = start.x * end.x + start.y * end.y + start.z * end.z + start.w * end.w;

    Quat endCorrected = end;
    if (dot < 0.0f)
    {
        dot = -dot;
        endCorrected = Quat(-end.x, -end.y, -end.z, -end.w);
    }

    if (dot > 0.9995f)
    {
        return (start + (endCorrected - start) * t).normalize();
    }

    float theta = acos(dot);
    float sinTheta = sqrt(1.0f - dot * dot);

    float weightStart = sin((1.0f - t) * theta) / sinTheta;
    float weightEnd = sin(t * theta) / sinTheta;

    return start * weightStart + endCorrected * weightEnd;
}

Quat Quat::Lerp(const Quat &start, const Quat &end, float t)
{

    float dot = start.x * end.x + start.y * end.y + start.z * end.z + start.w * end.w;

    Quat adjustedEnd = end;
    if (dot < 0.0f)
    {
        adjustedEnd.x = -end.x;
        adjustedEnd.y = -end.y;
        adjustedEnd.z = -end.z;
        adjustedEnd.w = -end.w;
    }

    Quat result;
    result.x = (1.0f - t) * start.x + t * adjustedEnd.x;
    result.y = (1.0f - t) * start.y + t * adjustedEnd.y;
    result.z = (1.0f - t) * start.z + t * adjustedEnd.z;
    result.w = (1.0f - t) * start.w + t * adjustedEnd.w;

    return result.normalize();
}

//***************************************************************************************************************
//                                               PLANE
//***************************************************************************************************************

Plane::Plane() : normal(0, 1, 0), distance(0) {}

Plane::Plane(const Vec3 &n, float d) : normal(n), distance(d) {}

Plane::Plane(const Vec3 &n, const Vec3 &point)
{
    normal = n.normal();
    distance = -normal.dot(point);
}

Plane::Plane(const Vec3 &p1, const Vec3 &p2, const Vec3 &p3)
{
    normal = (p2 - p1).cross(p3 - p1).normalize();
    distance = -normal.dot(p1);
}

Plane::Plane(float px, float py, float pz, float nx, float ny, float nz)
{
    normal = Vec3(nx, ny, nz).normalize();
    distance = -normal.dot(Vec3(px, py, pz));
}

float Plane::distanceToPoint(const Vec3 &point) const
{
    return normal.dot(point) + distance;
}

Vec3 Plane::projectPoint(const Vec3 &point) const
{
    return point - normal * distanceToPoint(point);
}

bool Plane::isFrontFacing(const Vec3 &point) const
{
    return distanceToPoint(point) > Epsilon;
}

bool Plane::isBackFacing(const Vec3 &point) const
{
    return distanceToPoint(point) < -Epsilon;
}

Plane::Side Plane::classifyPoint(const Vec3 &point) const
{
    float dist = distanceToPoint(point);
    if (dist > Epsilon)
        return FRONT;
    if (dist < -Epsilon)
        return BACK;
    return COPLANAR;
}

Plane::Side Plane::classifyBox(const BoundingBox &box) const
{
    float minDist = M_INFINITY;
    float maxDist = -M_INFINITY;

    for (int i = 0; i < 8; i++)
    {
        Vec3 corner(
            (i & 1) ? box.max.x : box.min.x,
            (i & 2) ? box.max.y : box.min.y,
            (i & 4) ? box.max.z : box.min.z);
        float dist = distanceToPoint(corner);
        minDist = Min(minDist, dist);
        maxDist = Max(maxDist, dist);
    }

    if (minDist > Epsilon)
        return FRONT;
    if (maxDist < -Epsilon)
        return BACK;
    return COPLANAR;
}

void Plane::recalculateDistance(const Vec3 &point)
{
    distance = -normal.dot(point);
}

//***************************************************************************************************************
//                                               TRIANGLE
//***************************************************************************************************************

Triangle::Triangle() {}

Triangle::Triangle(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2)
    : v0(v0), v1(v1), v2(v2) {}

Vec3 Triangle::normal() const
{
    return (v1 - v0).cross(v2 - v0).normal();
}

float Triangle::area() const
{
    return (v1 - v0).cross(v2 - v0).length() * 0.5f;
}

Vec3 Triangle::center() const
{
    return (v0 + v1 + v2) * (1.0f / 3.0f);
}

bool Triangle::isFrontFacing(const Vec3 &viewPoint) const
{
    Vec3 n = normal();
    return n.dot(viewPoint - v0) > 0;
}

bool Triangle::isBackFacing(const Vec3 &viewPoint) const
{
    return !isFrontFacing(viewPoint);
}

bool Triangle::containsPoint(const Vec3 &point) const
{
    Vec3 bary = getBarycentric(point);
    return bary.x >= 0 && bary.y >= 0 && bary.z >= 0 &&
           (bary.x + bary.y + bary.z) <= 1.0f + Epsilon;
}

Vec3 Triangle::getBarycentric(const Vec3 &point) const
{
    Vec3 v0v1 = v1 - v0;
    Vec3 v0v2 = v2 - v0;
    Vec3 v0p = point - v0;

    float d00 = v0v1.dot(v0v1);
    float d01 = v0v1.dot(v0v2);
    float d11 = v0v2.dot(v0v2);
    float d20 = v0p.dot(v0v1);
    float d21 = v0p.dot(v0v2);

    float denom = d00 * d11 - d01 * d01;
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    return Vec3(u, v, w);
}

void Triangle::getEdges(Vec3 &e0, Vec3 &e1, Vec3 &e2) const
{
    e0 = v1 - v0;
    e1 = v2 - v1;
    e2 = v0 - v2;
}

//***************************************************************************************************************
//                                               BOUNDING BOX
//***************************************************************************************************************

BoundingBox::BoundingBox() : min(M_INFINITY, M_INFINITY, M_INFINITY), max(-M_INFINITY, -M_INFINITY, -M_INFINITY) {}

BoundingBox::BoundingBox(const Vec3 &min, const Vec3 &max) : min(min), max(max) {}

void BoundingBox::reset(const Vec3 &point)
{
    min = max = point;
}

void BoundingBox::expand(float x, float y, float z)
{
    if (x > max.x)
        max.x = x;
    if (y > max.y)
        max.y = y;
    if (z > max.z)
        max.z = z;

    if (x < min.x)
        min.x = x;
    if (y < min.y)
        min.y = y;
    if (z < min.z)
        min.z = z;
}

void BoundingBox::expand(const Vec3 &point)
{
    expand(point.x, point.y, point.z);
}

void BoundingBox::expand(const BoundingBox &other)
{
    expand(other.max);
    expand(other.min);
}

void BoundingBox::scale(float factor)
{
    Vec3 centerPoint = center();
    Vec3 halfSize = size() * 0.5f * factor;
    min = centerPoint - halfSize;
    max = centerPoint + halfSize;
}

Vec3 BoundingBox::center() const
{
    return (min + max) * 0.5f;
}

Vec3 BoundingBox::size() const
{
    return max - min;
}

bool BoundingBox::contains(const Vec3 &point) const
{
    return point.x >= min.x && point.x <= max.x &&
           point.y >= min.y && point.y <= max.y &&
           point.z >= min.z && point.z <= max.z;
}

bool BoundingBox::intersects(const BoundingBox &other) const
{
    return min.x <= other.max.x && max.x >= other.min.x &&
           min.y <= other.max.y && max.y >= other.min.y &&
           min.z <= other.max.z && max.z >= other.min.z;
}

void BoundingBox::getEdges(Vec3 *edges) const
{
    Vec3 middle = (max + min) * 0.5f;
    Vec3 diag = max - middle;

    edges[0] = Vec3(middle.x + diag.x, middle.y + diag.y, middle.z + diag.z);
    edges[1] = Vec3(middle.x + diag.x, middle.y - diag.y, middle.z + diag.z);
    edges[2] = Vec3(middle.x + diag.x, middle.y + diag.y, middle.z - diag.z);
    edges[3] = Vec3(middle.x + diag.x, middle.y - diag.y, middle.z - diag.z);
    edges[4] = Vec3(middle.x - diag.x, middle.y + diag.y, middle.z + diag.z);
    edges[5] = Vec3(middle.x - diag.x, middle.y - diag.y, middle.z + diag.z);
    edges[6] = Vec3(middle.x - diag.x, middle.y + diag.y, middle.z - diag.z);
    edges[7] = Vec3(middle.x - diag.x, middle.y - diag.y, middle.z - diag.z);
}
void BoundingBox::getCorners(Vec3 *dst) const
{
    DEBUG_BREAK_IF(dst == nullptr);
    dst[0].set(min.x, max.y, max.z);
    dst[1].set(min.x, min.y, max.z);
    dst[2].set(max.x, min.y, max.z);
    dst[3].set(max.x, max.y, max.z);

    dst[4].set(max.x, max.y, min.z);
    dst[5].set(max.x, min.y, min.z);
    dst[6].set(min.x, min.y, min.z);
    dst[7].set(min.x, max.y, min.z);
}
Vec3 BoundingBox::getCorner(unsigned char index) const
{
    DEBUG_BREAK_IF(index > 7);
    Vec3 v = center();
    switch (index)
    {
    case 0:
        return Vec3(min.x, min.y, max.z);
    case 1:
        return Vec3(max.x, min.y, max.z);
    case 2:
        return Vec3(max.x, max.y, max.z);
    case 3:
        return Vec3(min.x, max.y, max.z);
    case 4:
        return Vec3(min.x, min.y, min.z);
    case 5:
        return Vec3(max.x, min.y, min.z);
    case 6:
        return Vec3(max.x, max.y, min.z);
    case 7:
        return Vec3(min.x, max.y, min.z);
    }
    return v;
}

static void updateMinMax(Vec3 *point, Vec3 *min, Vec3 *max)
{
    if (point->x < min->x)
        min->x = point->x;
    if (point->x > max->x)
        max->x = point->x;
    if (point->y < min->y)
        min->y = point->y;
    if (point->y > max->y)
        max->y = point->y;
    if (point->z < min->z)
        min->z = point->z;
    if (point->z > max->z)
        max->z = point->z;
}
void BoundingBox::transform(const Mat4 &matrix)
{
    Vec3 corners[8];
    getCorners(corners);
    matrix.transformPoint(corners[0]);
    Vec3 newMin = corners[0];
    Vec3 newMax = corners[0];
    for (int i = 1; i < 8; i++)
    {
        matrix.transformPoint(corners[i]);
        updateMinMax(&corners[i], &newMin, &newMax);
    }
    this->min.x = newMin.x;
    this->min.y = newMin.y;
    this->min.z = newMin.z;
    this->max.x = newMax.x;
    this->max.y = newMax.y;
    this->max.z = newMax.z;
}
void BoundingBox::transform(BoundingBox &box, const Mat4 &matrix)
{
    Vec3 corners[8];
    getCorners(corners);
    matrix.transformPoint(corners[0]);
    Vec3 newMin = corners[0];
    Vec3 newMax = corners[0];
    for (int i = 1; i < 8; i++)
    {
        matrix.transformPoint(corners[i]);
        updateMinMax(&corners[i], &newMin, &newMax);
    }
    box.min.x = newMin.x;
    box.min.y = newMin.y;
    box.min.z = newMin.z;
    box.max.x = newMax.x;
    box.max.y = newMax.y;
    box.max.z = newMax.z;
}
//***************************************************************************************************************
//                                               RAY
//***************************************************************************************************************

Ray::Ray() : direction(0, 0, 1) {}

Ray::Ray(const Vec3 &origin, const Vec3 &direction)
    : origin(origin), direction(direction.normal()) {}

Vec3 Ray::pointAt(float t) const
{
    return origin + direction * t;
}

bool Ray::intersectPlane(const Plane &plane, float &t) const
{
    float denom = direction.dot(plane.normal);
    if (fabs(denom) < Epsilon)
        return false;

    t = -(plane.distance + origin.dot(plane.normal)) / denom;
    return t >= 0;
}

bool Ray::intersectTriangle(const Triangle &tri, float &t) const
{
    // Möller–Trumbore intersection algorithm
    Vec3 edge1 = tri.v1 - tri.v0;
    Vec3 edge2 = tri.v2 - tri.v0;
    Vec3 h = direction.cross(edge2);
    float a = edge1.dot(h);

    if (fabs(a) < Epsilon)
        return false;

    float f = 1.0f / a;
    Vec3 s = origin - tri.v0;
    float u = f * s.dot(h);

    if (u < 0.0f || u > 1.0f)
        return false;

    Vec3 q = s.cross(edge1);
    float v = f * direction.dot(q);

    if (v < 0.0f || u + v > 1.0f)
        return false;

    t = f * edge2.dot(q);
    return t >= 0;
}

bool Ray::intersectBox(const BoundingBox &box, float &t) const
{
    Vec3 invDir(1.0f / direction.x, 1.0f / direction.y, 1.0f / direction.z);

    float tx1 = (box.min.x - origin.x) * invDir.x;
    float tx2 = (box.max.x - origin.x) * invDir.x;
    float tmin = Min(tx1, tx2);
    float tmax = Max(tx1, tx2);

    float ty1 = (box.min.y - origin.y) * invDir.y;
    float ty2 = (box.max.y - origin.y) * invDir.y;
    tmin = Max(tmin, Min(ty1, ty2));
    tmax = Min(tmax, Max(ty1, ty2));

    float tz1 = (box.min.z - origin.z) * invDir.z;
    float tz2 = (box.max.z - origin.z) * invDir.z;
    tmin = Max(tmin, Min(tz1, tz2));
    tmax = Min(tmax, Max(tz1, tz2));

    if (tmax >= tmin && tmax >= 0)
    {
        t = tmin < 0 ? tmax : tmin;
        return true;
    }
    return false;
}

//***************************************************************************************************************
//                                              FRUSTUM
//***************************************************************************************************************
Frustum::Frustum() {}

void Frustum::update(const Mat4 &viewProj)
{
    // Left plane
    planes[0].normal.x = viewProj.m[3] + viewProj.m[0];
    planes[0].normal.y = viewProj.m[7] + viewProj.m[4];
    planes[0].normal.z = viewProj.m[11] + viewProj.m[8];
    planes[0].distance = viewProj.m[15] + viewProj.m[12];

    // Right plane
    planes[1].normal.x = viewProj.m[3] - viewProj.m[0];
    planes[1].normal.y = viewProj.m[7] - viewProj.m[4];
    planes[1].normal.z = viewProj.m[11] - viewProj.m[8];
    planes[1].distance = viewProj.m[15] - viewProj.m[12];

    // Top plane
    planes[2].normal.x = viewProj.m[3] - viewProj.m[1];
    planes[2].normal.y = viewProj.m[7] - viewProj.m[5];
    planes[2].normal.z = viewProj.m[11] - viewProj.m[9];
    planes[2].distance = viewProj.m[15] - viewProj.m[13];

    // Bottom plane
    planes[3].normal.x = viewProj.m[3] + viewProj.m[1];
    planes[3].normal.y = viewProj.m[7] + viewProj.m[5];
    planes[3].normal.z = viewProj.m[11] + viewProj.m[9];
    planes[3].distance = viewProj.m[15] + viewProj.m[13];

    // Near plane
    planes[4].normal.x = viewProj.m[3] + viewProj.m[2];
    planes[4].normal.y = viewProj.m[7] + viewProj.m[6];
    planes[4].normal.z = viewProj.m[11] + viewProj.m[10];
    planes[4].distance = viewProj.m[15] + viewProj.m[14];

    // Far plane
    planes[5].normal.x = viewProj.m[3] - viewProj.m[2];
    planes[5].normal.y = viewProj.m[7] - viewProj.m[6];
    planes[5].normal.z = viewProj.m[11] - viewProj.m[10];
    planes[5].distance = viewProj.m[15] - viewProj.m[14];

    // Normalize all planes
    for (int i = 0; i < 6; i++)
    {
        float length = planes[i].normal.length();
        planes[i].normal /= length;
        planes[i].distance /= length;
    }
}

bool Frustum::containsPoint(const Vec3 &point) const
{
    for (int i = 0; i < 6; i++)
    {
        if (planes[i].distanceToPoint(point) < 0)
            return false;
    }
    return true;
}

bool Frustum::intersectsBox(const Vec3 &min, const Vec3 &max) const
{
    for (int i = 0; i < 6; i++)
    {
        Vec3 p = min;
        if (planes[i].normal.x >= 0)
            p.x = max.x;
        if (planes[i].normal.y >= 0)
            p.y = max.y;
        if (planes[i].normal.z >= 0)
            p.z = max.z;

        if (planes[i].distanceToPoint(p) < 0)
            return false;
    }
    return true;
}

bool Frustum::intersectsBox(const BoundingBox &box) const
{
    for (int i = 0; i < 6; i++)
    {
        Vec3 p = box.min;
        if (planes[i].normal.x >= 0)
            p.x = box.max.x;
        if (planes[i].normal.y >= 0)
            p.y = box.max.y;
        if (planes[i].normal.z >= 0)
            p.z = box.max.z;

        if (planes[i].distanceToPoint(p) < 0)
            return false;
    }
    return true;
}

Stack::Stack()
{
    m_index = 0;
    identity();
}

void Stack::push()
{
    if (m_index + 1 == MAX_STACK)
        return;

    m_stack[m_index] = m_stack[m_index - 1];
    m_index++;
}

void Stack::pop()
{
    if (m_index == 0)
        return;

    m_index--;
}

const Mat4 &Stack::top() const
{
    return m_stack[m_index - 1];
}

void Stack::identity()
{
    for (int i = 0; i < m_index; i++)
        m_stack[i].identity();
    m_index = 0;
}

void Stack::multiply(const Mat4 &m)
{
    //  m_stack[m_index] *= m;
}

void Stack::translate(float x, float y, float z)
{
    multiply(Mat4::Translate(Vec3(x, y, z)));
}

void Stack::scale(float x, float y, float z)
{
    multiply(Mat4::Scale(Vec3(x, y, z)));
}

void Stack::rotate(float radians, const Vec3 &axis)
{
    multiply(Mat4::Rotate(radians, axis));
}
