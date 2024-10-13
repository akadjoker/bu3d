#pragma once
#include "Config.hpp"

#include <cmath>

const unsigned int MaxUInt32 = 0xFFFFFFFF;
const int MinInt32 = 0x80000000;
const int MaxInt32 = 0x7FFFFFFF;
const float Maxloat = 3.402823466e+38F;
const float MinPosFloat = 1.175494351e-38F;
const float MATH_FLOAT_SMALL = 1.0e-37f;
const float MATH_TOLERANCE = 2e-37f;
const float Pi = 3.141592654f;
const float TwoPi = 6.283185307f;
const float PiHalf = 1.570796327f;

const float dtor = 0.0174532925199432957692369076848861f;
const float rtod = 1 / dtor;

const float Epsilon = 0.000001f;
const float ZeroEpsilon = 32.0f * MinPosFloat; // Very small epsilon for checking against 0.0f

const float M_INFINITY = 1.0e30f;

template <typename T>
T Min(const T &a, const T &b) { return (a < b) ? a : b; }

template <typename T>
T Max(const T &a, const T &b) { return (a > b) ? a : b; }

#define powi(base, exp) (int)powf((float)(base), (float)(exp))

#define ToRadians(x) (float)(((x) * Pi / 180.0f))
#define ToDegrees(x) (float)(((x) * 180.0f / Pi))

inline float Sin(float a) { return sin(a * Pi / 180); }
inline float Cos(float a) { return cos(a * Pi / 180); }
inline float Tan(float a) { return tan(a * Pi / 180); }
inline float SinRad(float a) { return sin(a); }
inline float CosRad(float a) { return cos(a); }
inline float TanRad(float a) { return tan(a); }
inline float ASin(float a) { return asin(a) * 180 / Pi; }
inline float ACos(float a) { return acos(a) * 180 / Pi; }
inline float ATan(float a) { return atan(a) * 180 / Pi; }
inline float ATan2(float y, float x) { return atan2(y, x) * 180 / Pi; }
inline float ASinRad(float a) { return asin(a); }
inline float ACosRad(float a) { return acos(a); }
inline float ATanRad(float a) { return atan(a); }
inline float ATan2Rad(float y, float x) { return atan2(y, x); }
inline int Floor(float a) { return (int)(floor(a)); }
inline int Ceil(float a) { return (int)(ceil(a)); }
inline int Trunc(float a)
{
	if (a > 0)
		return Floor(a);
	else
		return Ceil(a);
}
inline int Round(float a)
{
	if (a < 0)
		return (int)(ceil(a - 0.5f));
	else
		return (int)(floor(a + 0.5f));
}
inline float Sqrt(float a)
{
	if (a > 0)
		return sqrt(a);
	else
		return 0;
}
inline float Abs(float a)
{
	if (a < 0)
		a = -a;
	return a;
}
inline int Mod(int a, int b)
{
	if (b == 0)
		return 0;
	return a % b;
}
inline float FMod(float a, float b)
{
	if (b == 0)
		return 0;
	return fmod(a, b);
}
inline float Pow(float a, float b) { return pow(a, b); }
inline int Sign(float a)
{
	if (a < 0)
		return -1;
	else if (a > 0)
		return 1;
	else
		return 0;
}
inline float Min(float a, float b) { return a < b ? a : b; }
inline float Max(float a, float b) { return a > b ? a : b; }
inline float Clamp(float a, float min, float max)
{
	if (a < min)
		a = min;
	else if (a > max)
		a = max;
	return a;
}
inline int Clamp(int a, int min, int max)
{
	if (a < min)
		a = min;
	else if (a > max)
		a = max;
	return a;
}

inline bool isnotzero(const float a, const float tolerance=Epsilon)
{
	return fabsf(a) > tolerance;
}

inline bool isequal(const float a, const float b,const float tolerance=Epsilon)
{
	if (fabs(b-a) <= tolerance)
            return true;
        else
            return false;
}

enum NoInitHint
{
	NO_INIT
};

inline float degToRad(float f)
{
	return f * 0.017453293f;
}

inline float radToDeg(float f)
{
	return f * 57.29577951f;
}

class Frustum;
class Ray;
class BoundingSphere;
class BoundingBox;
class Mat4;
class Mat3;

// -------------------------------------------------------------------------------------------------
// Vector
// -------------------------------------------------------------------------------------------------

class Vec2
{
public:
	float x, y;

	// ------------
	// Constructors
	// ------------
	Vec2() : x(0.0f), y(0.0f)
	{
	}
	Vec2(const float v) : x(v), y(v)
	{
	}

	explicit Vec2(NoInitHint)
	{
		// Constructor without default initialization
	}

	Vec2(const float x, const float y) : x(x), y(y)
	{
	}

	// ------
	// Access
	// ------
	float operator[](unsigned int index) const
	{
		return *(&x + index);
	}

	float &operator[](unsigned int index)
	{
		return *(&x + index);
	}

	// -----------
	// Comparisons
	// -----------
	bool operator==(const Vec2 &v) const
	{
		return (x > v.x - Epsilon && x < v.x + Epsilon &&
				y > v.y - Epsilon && y < v.y + Epsilon);
	}

	bool operator!=(const Vec2 &v) const
	{
		return (x < v.x - Epsilon || x > v.x + Epsilon ||
				y < v.y - Epsilon || y > v.y + Epsilon);
	}

	// ---------------------
	// Arithmetic operations
	// ---------------------
	Vec2 operator-() const
	{
		return Vec2(-x, -y);
	}

	Vec2 operator+(const Vec2 &v) const
	{
		return Vec2(x + v.x, y + v.y);
	}

	Vec2 &operator+=(const Vec2 &v)
	{
		return *this = *this + v;
	}

	Vec2 operator-(const Vec2 &v) const
	{
		return Vec2(x - v.x, y - v.y);
	}

	Vec2 &operator-=(const Vec2 &v)
	{
		return *this = *this - v;
	}

	Vec2 operator*(const float f) const
	{
		return Vec2(x * f, y * f);
	}

	Vec2 &operator*=(const float f)
	{
		return *this = *this * f;
	}

	Vec2 operator/(const float f) const
	{
		return Vec2(x / f, y / f);
	}

	Vec2 &operator/=(const float f)
	{
		return *this = *this / f;
	}

	// ----------------
	// Special products
	// ----------------
	float dot(const Vec2 &v) const
	{
		return x * v.x + y * v.y;
	}

	// ----------------
	// Other operations
	// ----------------
	float length() const
	{
		return sqrtf(x * x + y * y);
	}

	Vec2 normalized() const
	{
		float invLen = 1.0f / length();
		return Vec2(x * invLen, y * invLen);
	}

	void normalize()
	{
		float invLen = 1.0f / length();
		x *= invLen;
		y *= invLen;
	}

	Vec2 lerp(const Vec2 &v, float f) const
	{
		return Vec2(x + (v.x - x) * f, y + (v.y - y) * f);
	}
};

class Vec3
{
public:
	float x, y, z;

	Vec3() : x(0.0f), y(0.0f), z(0.0f)
	{
	}

	explicit Vec3(NoInitHint)
	{
		// Constructor without default initialization
	}

	Vec3(const float v) : x(v), y(v), z(v)
	{
	}

	Vec3(const float x, const float y, const float z) : x(x), y(y), z(z)
	{
	}

	void set(float _x, float _y, float _z)
	{
		this->x = _x;
		this->y = _y;
		this->z = _z;
	}
	void set(const Vec3 &v)
	{
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
	}
	void set(const Vec3& p1, const Vec3& p2)
	{
		x = p2.x - p1.x;
		y = p2.y - p1.y;
		z = p2.z - p1.z;
	}

	// ------
	// Access
	// ------
	float operator[](unsigned int index) const
	{
		return *(&x + index);
	}

	float &operator[](unsigned int index)
	{
		return *(&x + index);
	}

	static const Vec3 &one()
	{
		static Vec3 value(1.0f, 1.0f, 1.0f);
		return value;
	}

	static const Vec3 &unitX()
	{
		static Vec3 value(1.0f, 0.0f, 0.0f);
		return value;
	}

	static const Vec3 &unitY()
	{
		static Vec3 value(0.0f, 1.0f, 0.0f);
		return value;
	}

	static const Vec3 &unitZ()
	{
		static Vec3 value(0.0f, 0.0f, 1.0f);
		return value;
	}

	bool isZero() const
	{
		return x == 0.0f && y == 0.0f && z == 0.0f;
	}

	bool isOne() const
	{
		return x == 1.0f && y == 1.0f && z == 1.0f;
	}

	// -----------
	// Comparisons
	// -----------
	bool operator==(const Vec3 &v) const
	{
		return (x > v.x - Epsilon && x < v.x + Epsilon &&
				y > v.y - Epsilon && y < v.y + Epsilon &&
				z > v.z - Epsilon && z < v.z + Epsilon);
	}

	bool operator!=(const Vec3 &v) const
	{
		return (x < v.x - Epsilon || x > v.x + Epsilon ||
				y < v.y - Epsilon || y > v.y + Epsilon ||
				z < v.z - Epsilon || z > v.z + Epsilon);
	}

	// ---------------------
	// Arithmetic operations
	// ---------------------
	Vec3 operator-() const
	{
		return Vec3(-x, -y, -z);
	}

	Vec3 operator+(const Vec3 &v) const
	{
		return Vec3(x + v.x, y + v.y, z + v.z);
	}

	Vec3 &operator+=(const Vec3 &v)
	{
		return *this = *this + v;
	}

	Vec3 operator-(const Vec3 &v) const
	{
		return Vec3(x - v.x, y - v.y, z - v.z);
	}

	Vec3 &operator-=(const Vec3 &v)
	{
		return *this = *this - v;
	}

	Vec3 operator*(const float f) const
	{
		return Vec3(x * f, y * f, z * f);
	}

	Vec3 operator*(const Vec3 &v) const
	{
		return Vec3(x * v.x, y * v.y, z * v.z);
	}

	Vec3 &operator*=(const float f)
	{
		return *this = *this * f;
	}

	Vec3 operator/(const float f) const
	{
		return Vec3(x / f, y / f, z / f);
	}
	Vec3 operator/(const Vec3 &v) const
	{
		return Vec3(x / v.x, y / v.y, z / v.z);
	}

	Vec3 &operator/=(const float f)
	{
		return *this = *this / f;
	}

	// ----------------
	// Special products
	// ----------------
	float dot(const Vec3 &v) const
	{
		return x * v.x + y * v.y + z * v.z;
	}

	Vec3 cross(const Vec3 &v) const
	{
		return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}

	// ----------------
	// Other operations
	// ----------------

	float length() const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	float length_squared() const
	{
		return x * x + y * y + z * z;
	}
	float distance(const Vec3 &v) const
	{
		return sqrtf((x - v.x) * (x - v.x) + (y - v.y) * (y - v.y) + (z - v.z) * (z - v.z));
	}

	float distance_squared(const Vec3 &v) const
	{
		return (x - v.x) * (x - v.x) + (y - v.y) * (y - v.y) + (z - v.z) * (z - v.z);
	}

	Vec3 normalized() const
	{
		float invLen = 1.0f / length();
		return Vec3(x * invLen, y * invLen, z * invLen);
	}

	void normalize()
	{
		float invLen = 1.0f / length();
		x *= invLen;
		y *= invLen;
		z *= invLen;
	}

	Vec3 lerp(const Vec3 &v, float f) const
	{
		return Vec3(x + (v.x - x) * f, y + (v.y - y) * f, z + (v.z - z) * f);
	}

	Vec3 toRotation() const
	{
		// Assumes that the unrotated view vector is (0, 0, -1)
		Vec3 v;

		if (y != 0)
			v.x = atan2f(y, sqrtf(x * x + z * z));
		if (x != 0 || z != 0)
			v.y = atan2f(-x, -z);

		return v;
	}

	Vec3 Min(const Vec3 &v) const
	{
		return Vec3(x < v.x ? x : v.x, y < v.y ? y : v.y, z < v.z ? z : v.z);
	}
	Vec3 Max(const Vec3 &v) const
	{
		return Vec3(x > v.x ? x : v.x, y > v.y ? y : v.y, z > v.z ? z : v.z);
	}
	void scale(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
	}


	void add(const Vec3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
	}

	void add(const Vec3& v1, const Vec3& v2, Vec3* dst)
	{
	

		dst->x = v1.x + v2.x;
		dst->y = v1.y + v2.y;
		dst->z = v1.z + v2.z;
	}




	static float Dot(const Vec3 &v1, const Vec3 &v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	static Vec3 Cross(const Vec3 &v1, const Vec3 &v2)
	{
		return Vec3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
	}
	static Vec3 Sub(const Vec3 &v1, const Vec3 &v2)
	{
		return Vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
	}
	static Vec3 Add(const Vec3 &v1, const Vec3 &v2)
	{
		return Vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
	}
	static Vec3 Normalize(const Vec3 &v)
	{
		float invLen = 1.0f / v.length();
		return Vec3(v.x * invLen, v.y * invLen, v.z * invLen);
	}
	static float DistanceFromSq(const Vec3 &v1, const Vec3 &v2)
	{
		Vec3 v = v1 - v2;
		return v.x * v.x + v.y * v.y + v.z * v.z;
	}
	static Vec3 GetAngleWeights(const Vec3 &v, const Vec3 &v1, const Vec3 &v2)
	{
		float a = DistanceFromSq(v1, v2);
		float asqrt = sqrtf(a);

		float b = DistanceFromSq(v, v1);
		float bsqrt = sqrtf(b);

		float c = DistanceFromSq(v, v2);
		float csqrt = sqrtf(c);

		return Vec3(
			acosf((b + c - a) / (2.f * bsqrt * csqrt)),
			acosf((-b + c + a) / (2.f * asqrt * csqrt)),
			acosf((b - c + a) / (2.f * bsqrt * asqrt)));
	}
		static Vec3 Lerp(const Vec3 &v1, const Vec3 &v2, float t)
		{
			return Vec3(v1.x + (v2.x - v1.x) * t, v1.y + (v2.y - v1.y) * t, v1.z + (v2.z - v1.z) * t);
		}
};

class Vec4
{
public:
	float x, y, z, w;

	Vec4() : x(0), y(0), z(0), w(0)
	{
	}

	explicit Vec4(const float x, const float y, const float z, const float w) : x(x), y(y), z(z), w(w)
	{
	}

	explicit Vec4(Vec3 v) : x(v.x), y(v.y), z(v.z), w(1.0f)
	{
	}

	// ------
	// Access
	// ------
	float operator[](unsigned int index) const
	{
		return *(&x + index);
	}

	float &operator[](unsigned int index)
	{
		return *(&x + index);
	}

	// ---------------------
	// Arithmetic operations
	// ---------------------
	Vec4 operator+(const Vec4 &v) const
	{
		return Vec4(x + v.x, y + v.y, z + v.z, w + v.w);
	}

	Vec4 operator-() const
	{
		return Vec4(-x, -y, -z, -w);
	}

	Vec4 operator*(const float f) const
	{
		return Vec4(x * f, y * f, z * f, w * f);
	}
	Vec4 operator/(const float f) const
	{
		return Vec4(x / f, y / f, z / f, w / f);
	}
	Vec4 operator+(const float f) const
	{
		return Vec4(x + f, y + f, z + f, w + f);
	}
	Vec4 operator-(const float f) const
	{
		return Vec4(x - f, y - f, z - f, w - f);
	}
	Vec4 operator*(const Vec4 &v) const
	{
		return Vec4(x * v.x, y * v.y, z * v.z, w * v.w);
	}
	Vec4 operator/(const Vec4 &v) const
	{
		return Vec4(x / v.x, y / v.y, z / v.z, w / v.w);
	}

	Vec4 operator-(const Vec4 &v) const
	{
		return Vec4(x - v.x, y - v.y, z - v.z, w - v.w);
	}
	Vec4 operator-=(const Vec4 &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}
	Vec4 operator*(const Mat4 &m);
};

// -------------------------------------------------------------------------------------------------
// Quaternion
// -------------------------------------------------------------------------------------------------

class Quaternion
{
public:
	float x, y, z, w;

	// ------------
	// Constructors
	// ------------
	Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
	{
	}

	explicit Quaternion(const float x, const float y, const float z, const float w) : x(x), y(y), z(z), w(w)
	{
	}

	Quaternion(const Quaternion &other) : x(other.x), y(other.y), z(other.z), w(other.w) {}

	Quaternion(const float eulerX, const float eulerY, const float eulerZ)
	{
		// Quaternion roll(0, 0, sinf(eulerZ * 0.5f), cosf(eulerZ * 0.5f));  // Z é o roll
		// Quaternion pitch(sinf(eulerX * 0.5f), 0, 0, cosf(eulerX * 0.5f));  // X é o pitch
		// Quaternion yaw(0, sinf(eulerY * 0.5f), 0, cosf(eulerY * 0.5f));  // Y é o yaw
		//*this = yaw * pitch *roll;

		Quaternion roll(sinf(eulerX * 0.5f), 0, 0, cosf(eulerX * 0.5f));
		Quaternion pitch(0, sinf(eulerY * 0.5f), 0, cosf(eulerY * 0.5f));
		Quaternion yaw(0, 0, sinf(eulerZ * 0.5f), cosf(eulerZ * 0.5f));
		// Order: y * x * z
		*this = pitch * roll * yaw;
	}
	const Quaternion &identity()
	{
		static Quaternion value(0.0f, 0.0f, 0.0f, 1.0f);
		return value;
	}

	const Quaternion &zero()
	{
		static Quaternion value(0.0f, 0.0f, 0.0f, 0.0f);
		return value;
	}

	bool isIdentity() const
	{
		return x == 0.0f && y == 0.0f && z == 0.0f && w == 1.0f;
	}

	bool isZero() const
	{
		return x == 0.0f && y == 0.0f && z == 0.0f && w == 0.0f;
	}
	void set(const Quaternion &q)
	{
		this->x = q.x;
		this->y = q.y;
		this->z = q.z;
		this->w = q.w;
	}
	void set(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}
	void conjugate()
	{
		conjugate(this);
	}

	void conjugate(Quaternion *dst) const
	{

		dst->x = -x;
		dst->y = -y;
		dst->z = -z;
		dst->w = w;
	}
	bool inverse()
	{
		return inverse(this);
	}

	bool inverse(Quaternion *dst) const
	{

		float n = x * x + y * y + z * z + w * w;
		if (n == 1.0f)
		{
			dst->x = -x;
			dst->y = -y;
			dst->z = -z;
			dst->w = w;

			return true;
		}

		// Too close to zero.
		if (n < 0.000001f)
			return false;

		n = 1.0f / n;
		dst->x = -x * n;
		dst->y = -y * n;
		dst->z = -z * n;
		dst->w = w * n;

		return true;
	}
	void normalize()
	{
		normalize(this);
	}

	void normalize(Quaternion *dst) const
	{

		if (this != dst)
		{
			dst->x = x;
			dst->y = y;
			dst->z = z;
			dst->w = w;
		}

		float n = x * x + y * y + z * z + w * w;

		// Already normalized.
		if (n == 1.0f)
			return;

		n = sqrt(n);
		// Too close to zero.
		if (n < 0.000001f)
			return;

		n = 1.0f / n;
		dst->x *= n;
		dst->y *= n;
		dst->z *= n;
		dst->w *= n;
	}
	float toAxisAngle(Vec3 *axis) const
	{
		Quaternion q(x, y, z, w);
		q.normalize();
		if (axis)
		{
			axis->x = q.x;
			axis->y = q.y;
			axis->z = q.z;
			axis->normalize();
		}
		return (2.0f * acos(q.w));
	}

	static Quaternion FromEuler(const Vec3 &rot)
	{
		// Metade dos ângulos de rotação
		float cx = cosf(rot.x * 0.5f);
		float sx = sinf(rot.x * 0.5f);
		float cy = cosf(rot.y * 0.5f);
		float sy = sinf(rot.y * 0.5f);
		float cz = cosf(rot.z * 0.5f);
		float sz = sinf(rot.z * 0.5f);

		// Quaternions individuais para cada eixo
		Quaternion qx(cx, sx, 0.0f, 0.0f); // Rotação no eixo X
		Quaternion qy(cy, 0.0f, sy, 0.0f); // Rotação no eixo Y
		Quaternion qz(cz, 0.0f, 0.0f, sz); // Rotação no eixo Z

		// Multiplicar os quaternions na ordem YXZ (ou a ordem que você preferir)
		return qy * qx * qz;
	}
	static Quaternion FromAxisAngle(const Vec3 &axis, float angle)
	{
		Quaternion dst;

		float halfAngle = angle * 0.5f;
		float sinHalfAngle = sinf(halfAngle);

		Vec3 normal(axis);
		normal.normalize();
		dst.x = normal.x * sinHalfAngle;
		dst.y = normal.y * sinHalfAngle;
		dst.z = normal.z * sinHalfAngle;
		dst.w = cosf(halfAngle);
		return dst;
	}

	void rotatePoint(const Vec3 &point, Vec3 *dst) const
	{
		Quaternion vecQuaternion;
		Quaternion resQuaternion;
		vecQuaternion.x = point.x;
		vecQuaternion.y = point.y;
		vecQuaternion.z = point.z;
		vecQuaternion.w = 0.0f;

		Quaternion conQuaternion;
		this->conjugate(&conQuaternion);

		resQuaternion = vecQuaternion * conQuaternion;
		resQuaternion = (*this) * resQuaternion;

		dst->set(resQuaternion.x, resQuaternion.y, resQuaternion.z);
	}
	// ---------------------
	// Arithmetic operations
	// ---------------------
	Quaternion operator*(const Quaternion &q) const
	{
		return Quaternion(
			y * q.z - z * q.y + q.x * w + x * q.w,
			z * q.x - x * q.z + q.y * w + y * q.w,
			x * q.y - y * q.x + q.z * w + z * q.w,
			w * q.w - (x * q.x + y * q.y + z * q.z));
	}

	Quaternion &operator*=(const Quaternion &q)
	{
		return *this = *this * q;
	}
	Vec3 operator*(const Vec3 &v) const
	{
		Vec3 u(x, y, z);
		float s = w;

		// Fórmula para rotação de vetor por quaternion
		return u * (2.0f * Vec3::Dot(u, v)) +
			   v * (s * s - Vec3::Dot(u, u)) +
			   Vec3::Cross(u, v) * (2.0f * s);
	}

	// ----------------
	// Other operations
	// ----------------

	Quaternion slerp(const Quaternion &q, const float t) const
	{
		// Spherical linear interpolation between two quaternions
		// Note: SLERP is not commutative

		Quaternion q1(q);

		// Calculate cosine
		float cosTheta = x * q.x + y * q.y + z * q.z + w * q.w;

		// Use the shortest path
		if (cosTheta < 0)
		{
			cosTheta = -cosTheta;
			q1.x = -q.x;
			q1.y = -q.y;
			q1.z = -q.z;
			q1.w = -q.w;
		}

		// Initialize with linear interpolation
		float scale0 = 1 - t, scale1 = t;

		// Use spherical interpolation only if the quaternions are not very close
		if ((1 - cosTheta) > 0.001f)
		{
			// SLERP
			float theta = acosf(cosTheta);
			float sinTheta = sinf(theta);
			scale0 = sinf((1 - t) * theta) / sinTheta;
			scale1 = sinf(t * theta) / sinTheta;
		}

		// Calculate final quaternion
		return Quaternion(x * scale0 + q1.x * scale1, y * scale0 + q1.y * scale1,
						  z * scale0 + q1.z * scale1, w * scale0 + q1.w * scale1);
	}

	Quaternion nlerp(const Quaternion &q, const float t) const
	{
		// Normalized linear quaternion interpolation
		// Note: NLERP is faster than SLERP and commutative but does not yield constant velocity

		Quaternion qt;
		float cosTheta = x * q.x + y * q.y + z * q.z + w * q.w;

		// Use the shortest path and interpolate linearly
		if (cosTheta < 0)
			qt = Quaternion(x + (-q.x - x) * t, y + (-q.y - y) * t,
							z + (-q.z - z) * t, w + (-q.w - w) * t);
		else
			qt = Quaternion(x + (q.x - x) * t, y + (q.y - y) * t,
							z + (q.z - z) * t, w + (q.w - w) * t);

		// Return normalized quaternion
		float invLen = 1.0f / sqrtf(qt.x * qt.x + qt.y * qt.y + qt.z * qt.z + qt.w * qt.w);
		return Quaternion(qt.x * invLen, qt.y * invLen, qt.z * invLen, qt.w * invLen);
	}

	Quaternion inverted() const
	{
		float len = x * x + y * y + z * z + w * w;
		if (len > 0)
		{
			float invLen = 1.0f / len;
			return Quaternion(-x * invLen, -y * invLen, -z * invLen, w * invLen);
		}
		else
			return Quaternion();
	}
	void setIdentity()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 1.0f;
	}
	static Quaternion Slerp(const Quaternion &a, const Quaternion &b, const float t ) 
	{
		return a.slerp(b, t);
	}

	static Quaternion Normalize(const Quaternion &q)
	{
		Quaternion out( q );
		out.normalize();
		return out;
	}
};

// -------------------------------------------------------------------------------------------------
// Matrix
// -------------------------------------------------------------------------------------------------

class Mat3
{
public:
	float m[3][3];

	explicit Mat3(NoInitHint)
	{
	}

	Mat3()
	{
		m[0][0] = 1.0f;
		m[0][1] = 0.0f;
		m[0][2] = 0.0f;
		m[1][0] = 0.0f;
		m[1][1] = 1.0f;
		m[1][2] = 0.0f;
		m[2][0] = 0.0f;
		m[2][1] = 0.0f;
		m[2][2] = 1.0f;
	}
	Mat3(const float arr[3][3])
	{
		memcpy(m, arr, 9 * sizeof(float));
	}
	Mat3(float V00, float V01, float V02,
		 float V10, float V11, float V12,
		 float V20, float V21, float V22)
	{
		m[0][0] = V00;
		m[0][1] = V01;
		m[0][2] = V02;

		m[1][0] = V10;
		m[1][1] = V11;
		m[1][2] = V12;

		m[2][0] = V20;
		m[2][1] = V21;
		m[2][2] = V22;
	}

	void identity()
	{

		m[0][0] = 1.0f;
		m[0][1] = 0.0f;
		m[0][2] = 0.0f;
		m[1][0] = 0.0f;
		m[1][1] = 1.0f;
		m[1][2] = 0.0f;
		m[2][0] = 0.0f;
		m[2][1] = 0.0f;
		m[2][2] = 1.0f;
	}
	Mat3 transpose() const
	{
		Mat3 kTranspose;
		for (size_t iRow = 0; iRow < 3; iRow++)
		{
			for (size_t iCol = 0; iCol < 3; iCol++)
				kTranspose[iRow][iCol] = m[iCol][iRow];
		}
		return kTranspose;
	}
	Mat3 operator*(const Mat3 &rkMatrix) const
	{
		Mat3 kProd;
		for (size_t iRow = 0; iRow < 3; iRow++)
		{
			for (size_t iCol = 0; iCol < 3; iCol++)
			{
				kProd.m[iRow][iCol] =
					m[iRow][0] * rkMatrix.m[0][iCol] +
					m[iRow][1] * rkMatrix.m[1][iCol] +
					m[iRow][2] * rkMatrix.m[2][iCol];
			}
		}
		return kProd;
	}

	Mat3 &operator=(const Mat3 &rkMatrix)
	{
		memcpy(m, rkMatrix.m, 9 * sizeof(float));
		return *this;
	}
	bool operator!=(const Mat3 &rkMatrix) const
	{
		return !operator==(rkMatrix);
	}
	bool operator==(const Mat3 &rkMatrix) const
	{
		for (size_t iRow = 0; iRow < 3; iRow++)
		{
			for (size_t iCol = 0; iCol < 3; iCol++)
			{
				if (m[iRow][iCol] != rkMatrix.m[iRow][iCol])
					return false;
			}
		}

		return true;
	}
	Vec3 operator*(const Vec3 &rkPoint) const
	{
		Vec3 kProd;
		for (unsigned int iRow = 0; iRow < 3; iRow++)
		{
			kProd[iRow] =
				m[iRow][0] * rkPoint.x +
				m[iRow][1] * rkPoint.y +
				m[iRow][2] * rkPoint.z;
		}
		return kProd;
	}
	Mat3 operator*(float fScalar) const
	{
		Mat3 kProd;
		for (size_t iRow = 0; iRow < 3; iRow++)
		{
			for (size_t iCol = 0; iCol < 3; iCol++)
				kProd[iRow][iCol] = fScalar * m[iRow][iCol];
		}
		return kProd;
	}

	inline const float *operator[](size_t iRow) const
	{
		return m[iRow];
	}

	inline float *operator[](size_t iRow)
	{
		return m[iRow];
	}
	static Mat3 createRotationX(float angle)
	{
		float cosTheta = cos(angle);
		float sinTheta = sin(angle);
		return Mat3(
			1.0f, 0.0f, 0.0f,
			0.0f, cosTheta, sinTheta,
			0.0f, -sinTheta, cosTheta);
	}

	static Mat3 createRotationY(float angle)
	{
		float cosTheta = cos(angle);
		float sinTheta = sin(angle);
		return Mat3(
			cosTheta, 0.0f, -sinTheta,
			0.0f, 1.0f, 0.0f,
			sinTheta, 0.0f, cosTheta);
	}

	static Mat3 createRotationZ(float angle)
	{
		float cosTheta = cos(angle);
		float sinTheta = sin(angle);
		return Mat3(
			cosTheta, sinTheta, 0.0f,
			-sinTheta, cosTheta, 0.0f,
			0.0f, 0.0f, 1.0f);
	}
	bool hasScale() const
	{
		// check magnitude of column vectors (==local axes)
		float t = m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0];
		if (!isequal(t, 1.0, (float)1e-04))
			return true;
		t = m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1];
		if (!isequal(t, 1.0, (float)1e-04))
			return true;
		t = m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2];
		if (!isequal(t, 1.0, (float)1e-04))
			return true;

		return false;
	}

	static const Mat3 ZERO;
	static const Mat3 IDENTITY;

protected:
	friend class Mat4;
};

class Mat4
{
public:
	union
	{
		float c[4][4]; // Column major order for OpenGL: c[column][row]
		float x[16];
	};

	// --------------
	// Static methods
	// --------------
	static Mat4 Translate(float x, float y, float z)
	{
		Mat4 m;

		m.c[3][0] = x;
		m.c[3][1] = y;
		m.c[3][2] = z;

		return m;
	}

	static Mat4 Scale(float x, float y, float z)
	{
		Mat4 m;

		m.c[0][0] = x;
		m.c[1][1] = y;
		m.c[2][2] = z;

		return m;
	}

	static Mat4 Rotate(float x, float y, float z)
	{
		// Rotation order: YXZ [* Vector]
		return Mat4(Quaternion(x, y, z));
	}

	static Mat4 Rotate(Vec3 axis, float angle)
	{
		axis = axis * sinf(angle * 0.5f);
		return Mat4(Quaternion(axis.x, axis.y, axis.z, cosf(angle * 0.5f)));
	}
	static Mat4 Perspective(float fovY, float aspect, float nearPlane, float farPlane)
	{
		float height = 1.0f / (float)tan(fovY * Pi / 360.0f);
		float width = height / aspect;
		float f = farPlane;
		float n = nearPlane;

		Mat4 m;

		m.x[0] = width;
		m.x[5] = height;
		m.x[10] = (f + n) / (n - f);
		m.x[11] = -1.0f;
		m.x[14] = 2.0f * f * n / (n - f);
		m.x[15] = 0.0f;

		return m;
	}
	static Mat4 Perspective(float l, float r, float b, float t, float n, float f)
	{
		Mat4 m;

		m.x[0] = 2 * n / (r - l);
		m.x[5] = 2 * n / (t - b);
		m.x[8] = (r + l) / (r - l);
		m.x[9] = (t + b) / (t - b);
		m.x[10] = -(f + n) / (f - n);
		m.x[11] = -1;
		m.x[14] = -2 * f * n / (f - n);
		m.x[15] = 0;

		return m;
	}

	static Mat4 Ortho(float l, float r, float b, float t, float n, float f)
	{
		Mat4 m;

		m.x[0] = 2 / (r - l);
		m.x[5] = 2 / (t - b);
		m.x[10] = -2 / (f - n);
		m.x[12] = -(r + l) / (r - l);
		m.x[13] = -(t + b) / (t - b);
		m.x[14] = -(f + n) / (f - n);

		return m;
	}

	static Mat4 LookAt(const Vec3 &eye, const Vec3 &center, const Vec3 &up)
	{
		Vec3 f = (center - eye).normalized();
		Vec3 u = up.normalized();
		Vec3 s = Vec3::Cross(f, u).normalized();
		u = Vec3::Cross(s, f);

		Mat4 m;

		m.c[0][0] = s.x;
		m.c[1][0] = s.y;
		m.c[2][0] = s.z;

		m.c[0][1] = u.x;
		m.c[1][1] = u.y;
		m.c[2][1] = u.z;

		m.c[0][2] = -f.x;
		m.c[1][2] = -f.y;
		m.c[2][2] = -f.z;

		m.c[3][0] = -s.dot(eye);
		m.c[3][1] = -u.dot(eye);
		m.c[3][2] = f.dot(eye);

		return m;
	}

	static void fastMult43(Mat4 &dst, const Mat4 &m1, const Mat4 &m2)
	{
		// Note: dst may not be the same as m1 or m2

		float *dstx = dst.x;
		const float *m1x = m1.x;
		const float *m2x = m2.x;

		dstx[0] = m1x[0] * m2x[0] + m1x[4] * m2x[1] + m1x[8] * m2x[2];
		dstx[1] = m1x[1] * m2x[0] + m1x[5] * m2x[1] + m1x[9] * m2x[2];
		dstx[2] = m1x[2] * m2x[0] + m1x[6] * m2x[1] + m1x[10] * m2x[2];
		dstx[3] = 0.0f;

		dstx[4] = m1x[0] * m2x[4] + m1x[4] * m2x[5] + m1x[8] * m2x[6];
		dstx[5] = m1x[1] * m2x[4] + m1x[5] * m2x[5] + m1x[9] * m2x[6];
		dstx[6] = m1x[2] * m2x[4] + m1x[6] * m2x[5] + m1x[10] * m2x[6];
		dstx[7] = 0.0f;

		dstx[8] = m1x[0] * m2x[8] + m1x[4] * m2x[9] + m1x[8] * m2x[10];
		dstx[9] = m1x[1] * m2x[8] + m1x[5] * m2x[9] + m1x[9] * m2x[10];
		dstx[10] = m1x[2] * m2x[8] + m1x[6] * m2x[9] + m1x[10] * m2x[10];
		dstx[11] = 0.0f;

		dstx[12] = m1x[0] * m2x[12] + m1x[4] * m2x[13] + m1x[8] * m2x[14] + m1x[12] * m2x[15];
		dstx[13] = m1x[1] * m2x[12] + m1x[5] * m2x[13] + m1x[9] * m2x[14] + m1x[13] * m2x[15];
		dstx[14] = m1x[2] * m2x[12] + m1x[6] * m2x[13] + m1x[10] * m2x[14] + m1x[14] * m2x[15];
		dstx[15] = 1.0f;
	}

	static Mat4 Identity()
	{
		Mat4 m(NO_INIT);
		m.x[0] = 1;
		m.x[1] = 0;
		m.x[2] = 0;
		m.x[3] = 0;
		m.x[4] = 0;
		m.x[5] = 1;
		m.x[6] = 0;
		m.x[7] = 0;
		m.x[8] = 0;
		m.x[9] = 0;
		m.x[10] = 1;
		m.x[11] = 0;
		m.x[12] = 0;
		m.x[13] = 0;
		m.x[14] = 0;
		m.x[15] = 1;
		return m;
	}

	// ------------
	// Constructors
	// ------------
	Mat4()
	{
		c[0][0] = 1;
		c[1][0] = 0;
		c[2][0] = 0;
		c[3][0] = 0;
		c[0][1] = 0;
		c[1][1] = 1;
		c[2][1] = 0;
		c[3][1] = 0;
		c[0][2] = 0;
		c[1][2] = 0;
		c[2][2] = 1;
		c[3][2] = 0;
		c[0][3] = 0;
		c[1][3] = 0;
		c[2][3] = 0;
		c[3][3] = 1;
	}

	explicit Mat4(NoInitHint)
	{
		// Constructor without default initialization
	}

	Mat4(const float *floatArray16)
	{
		for (unsigned int i = 0; i < 4; ++i)
		{
			for (unsigned int j = 0; j < 4; ++j)
			{
				c[i][j] = floatArray16[i * 4 + j];
			}
		}
	}

	Mat4(const Quaternion &q)
	{
		// Calculate coefficients
		float x2 = q.x + q.x, y2 = q.y + q.y, z2 = q.z + q.z;
		float xx = q.x * x2, xy = q.x * y2, xz = q.x * z2;
		float yy = q.y * y2, yz = q.y * z2, zz = q.z * z2;
		float wx = q.w * x2, wy = q.w * y2, wz = q.w * z2;

		c[0][0] = 1 - (yy + zz);
		c[1][0] = xy - wz;
		c[2][0] = xz + wy;
		c[3][0] = 0;
		c[0][1] = xy + wz;
		c[1][1] = 1 - (xx + zz);
		c[2][1] = yz - wx;
		c[3][1] = 0;
		c[0][2] = xz - wy;
		c[1][2] = yz + wx;
		c[2][2] = 1 - (xx + yy);
		c[3][2] = 0;
		c[0][3] = 0;
		c[1][3] = 0;
		c[2][3] = 0;
		c[3][3] = 1;
	}
	void set(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24,
                 float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44)
	{
		x[0]  = m11;
		x[1]  = m21;
		x[2]  = m31;
		x[3]  = m41;
		x[4]  = m12;
		x[5]  = m22;
		x[6]  = m32;
		x[7]  = m42;
		x[8]  = m13;
		x[9]  = m23;
		x[10] = m33;
		x[11] = m43;
		x[12] = m14;
		x[13] = m24;
		x[14] = m34;
		x[15] = m44;
	}
	void set(const Mat4& mat)
	{
			memcpy(this->x, mat.x, 16);
	}
	// ----------
	// Matrix sum
	// ----------
	Mat4 operator+(const Mat4 &m) const
	{
		Mat4 mf(NO_INIT);

		mf.x[0] = x[0] + m.x[0];
		mf.x[1] = x[1] + m.x[1];
		mf.x[2] = x[2] + m.x[2];
		mf.x[3] = x[3] + m.x[3];
		mf.x[4] = x[4] + m.x[4];
		mf.x[5] = x[5] + m.x[5];
		mf.x[6] = x[6] + m.x[6];
		mf.x[7] = x[7] + m.x[7];
		mf.x[8] = x[8] + m.x[8];
		mf.x[9] = x[9] + m.x[9];
		mf.x[10] = x[10] + m.x[10];
		mf.x[11] = x[11] + m.x[11];
		mf.x[12] = x[12] + m.x[12];
		mf.x[13] = x[13] + m.x[13];
		mf.x[14] = x[14] + m.x[14];
		mf.x[15] = x[15] + m.x[15];

		return mf;
	}

	Mat4 &operator+=(const Mat4 &m)
	{
		return *this = *this + m;
	}

	// ---------------------
	// Matrix multiplication
	// ---------------------
	Mat4 operator*(const Mat4 &m) const
	{
		Mat4 mf(NO_INIT);

		mf.x[0] = x[0] * m.x[0] + x[4] * m.x[1] + x[8] * m.x[2] + x[12] * m.x[3];
		mf.x[1] = x[1] * m.x[0] + x[5] * m.x[1] + x[9] * m.x[2] + x[13] * m.x[3];
		mf.x[2] = x[2] * m.x[0] + x[6] * m.x[1] + x[10] * m.x[2] + x[14] * m.x[3];
		mf.x[3] = x[3] * m.x[0] + x[7] * m.x[1] + x[11] * m.x[2] + x[15] * m.x[3];

		mf.x[4] = x[0] * m.x[4] + x[4] * m.x[5] + x[8] * m.x[6] + x[12] * m.x[7];
		mf.x[5] = x[1] * m.x[4] + x[5] * m.x[5] + x[9] * m.x[6] + x[13] * m.x[7];
		mf.x[6] = x[2] * m.x[4] + x[6] * m.x[5] + x[10] * m.x[6] + x[14] * m.x[7];
		mf.x[7] = x[3] * m.x[4] + x[7] * m.x[5] + x[11] * m.x[6] + x[15] * m.x[7];

		mf.x[8] = x[0] * m.x[8] + x[4] * m.x[9] + x[8] * m.x[10] + x[12] * m.x[11];
		mf.x[9] = x[1] * m.x[8] + x[5] * m.x[9] + x[9] * m.x[10] + x[13] * m.x[11];
		mf.x[10] = x[2] * m.x[8] + x[6] * m.x[9] + x[10] * m.x[10] + x[14] * m.x[11];
		mf.x[11] = x[3] * m.x[8] + x[7] * m.x[9] + x[11] * m.x[10] + x[15] * m.x[11];

		mf.x[12] = x[0] * m.x[12] + x[4] * m.x[13] + x[8] * m.x[14] + x[12] * m.x[15];
		mf.x[13] = x[1] * m.x[12] + x[5] * m.x[13] + x[9] * m.x[14] + x[13] * m.x[15];
		mf.x[14] = x[2] * m.x[12] + x[6] * m.x[13] + x[10] * m.x[14] + x[14] * m.x[15];
		mf.x[15] = x[3] * m.x[12] + x[7] * m.x[13] + x[11] * m.x[14] + x[15] * m.x[15];

		return mf;
	}

	Mat4 operator*(const float f) const
	{
		Mat4 m(*this);

		m.x[0] *= f;
		m.x[1] *= f;
		m.x[2] *= f;
		m.x[3] *= f;
		m.x[4] *= f;
		m.x[5] *= f;
		m.x[6] *= f;
		m.x[7] *= f;
		m.x[8] *= f;
		m.x[9] *= f;
		m.x[10] *= f;
		m.x[11] *= f;
		m.x[12] *= f;
		m.x[13] *= f;
		m.x[14] *= f;
		m.x[15] *= f;

		return m;
	}

	// ----------------------------
	// Vector-Matrix multiplication
	// ----------------------------
	Vec3 operator*(const Vec3 &v) const
	{
		return Vec3(v.x * c[0][0] + v.y * c[1][0] + v.z * c[2][0] + c[3][0],
					v.x * c[0][1] + v.y * c[1][1] + v.z * c[2][1] + c[3][1],
					v.x * c[0][2] + v.y * c[1][2] + v.z * c[2][2] + c[3][2]);
	}

	Vec4 operator*(const Vec4 &v) const
	{
		return Vec4(v.x * c[0][0] + v.y * c[1][0] + v.z * c[2][0] + v.w * c[3][0],
					v.x * c[0][1] + v.y * c[1][1] + v.z * c[2][1] + v.w * c[3][1],
					v.x * c[0][2] + v.y * c[1][2] + v.z * c[2][2] + v.w * c[3][2],
					v.x * c[0][3] + v.y * c[1][3] + v.z * c[2][3] + v.w * c[3][3]);
	}

	Vec3 mult33Vec(const Vec3 &v) const
	{
		return Vec3(v.x * c[0][0] + v.y * c[1][0] + v.z * c[2][0],
					v.x * c[0][1] + v.y * c[1][1] + v.z * c[2][1],
					v.x * c[0][2] + v.y * c[1][2] + v.z * c[2][2]);
	}

	// ---------------
	// Transformations
	// ---------------
	void translate(const float tx, const float ty, const float tz)
	{
		*this = Translate(tx, ty, tz) * *this;
	}

	void scale(const float sx, const float sy, const float sz)
	{
		*this = Scale(sx, sy, sz) * *this;
	}

	void rotate(const float rx, const float ry, const float rz)
	{
		*this = Rotate(rx, ry, rz) * *this;
	}

	// ---------------
	// Other
	// ---------------
	

	void transformPoint(Vec3 *v) const
	{
		v->x = v->x * c[0][0] + v->y * c[1][0] + v->z * c[2][0] + c[3][0];
		v->y = v->x * c[0][1] + v->y * c[1][1] + v->z * c[2][1] + c[3][1];
		v->z = v->x * c[0][2] + v->y * c[1][2] + v->z * c[2][2] + c[3][2];
	}

	void transformNormal(Vec3 *v) const
	{
		v->x = v->x * c[0][0] + v->y * c[1][0] + v->z * c[2][0];
		v->x = v->x * c[0][1] + v->y * c[1][1] + v->z * c[2][1];
		v->x = v->x * c[0][2] + v->y * c[1][2] + v->z * c[2][2];
	}

	Quaternion toQuaternion() const
	{
		Quaternion q;
		float trace = c[0][0] + c[1][1] + c[2][2]; // Traço dacatriz
		if (trace > 0.0f)
		{
			float s = 0.5f / sqrtf(trace + 1.0f);
			q.w = 0.25f / s;
			q.x = (c[2][1] - c[1][2]) * s;
			q.y = (c[0][2] - c[2][0]) * s;
			q.z = (c[1][0] - c[0][1]) * s;
		}
		else
		{
			if (c[0][0] > c[1][1] && c[0][0] > c[2][2])
			{
				float s = 2.0f * sqrtf(1.0f + c[0][0] - c[1][1] - c[2][2]);
				q.w = (c[2][1] - c[1][2]) / s;
				q.x = 0.25f * s;
				q.y = (c[0][1] + c[1][0]) / s;
				q.z = (c[0][2] + c[2][0]) / s;
			}
			else if (c[1][1] > c[2][2])
			{
				float s = 2.0f * sqrtf(1.0f + c[1][1] - c[0][0] - c[2][2]);
				q.w = (c[0][2] - c[2][0]) / s;
				q.x = (c[0][1] + c[1][0]) / s;
				q.y = 0.25f * s;
				q.z = (c[1][2] + c[2][1]) / s;
			}
			else
			{
				float s = 2.0f * sqrtf(1.0f + c[2][2] - c[0][0] - c[1][1]);
				q.w = (c[1][0] - c[0][1]) / s;
				q.x = (c[0][2] + c[2][0]) / s;
				q.y = (c[1][2] + c[2][1]) / s;
				q.z = 0.25f * s;
			}
		}
		return q;
	}

	Mat4 transposed() const
	{
		Mat4 m(*this);

		for (unsigned int maty = 0; maty < 4; ++maty)
		{
			for (unsigned int matx = maty + 1; matx < 4; ++matx)
			{
				float tmp = m.c[matx][maty];
				m.c[matx][maty] = m.c[maty][matx];
				m.c[maty][matx] = tmp;
			}
		}

		return m;
	}

	float determinant() const
	{
		return c[0][3] * c[1][2] * c[2][1] * c[3][0] - c[0][2] * c[1][3] * c[2][1] * c[3][0] - c[0][3] * c[1][1] * c[2][2] * c[3][0] + c[0][1] * c[1][3] * c[2][2] * c[3][0] +
			   c[0][2] * c[1][1] * c[2][3] * c[3][0] - c[0][1] * c[1][2] * c[2][3] * c[3][0] - c[0][3] * c[1][2] * c[2][0] * c[3][1] + c[0][2] * c[1][3] * c[2][0] * c[3][1] +
			   c[0][3] * c[1][0] * c[2][2] * c[3][1] - c[0][0] * c[1][3] * c[2][2] * c[3][1] - c[0][2] * c[1][0] * c[2][3] * c[3][1] + c[0][0] * c[1][2] * c[2][3] * c[3][1] +
			   c[0][3] * c[1][1] * c[2][0] * c[3][2] - c[0][1] * c[1][3] * c[2][0] * c[3][2] - c[0][3] * c[1][0] * c[2][1] * c[3][2] + c[0][0] * c[1][3] * c[2][1] * c[3][2] +
			   c[0][1] * c[1][0] * c[2][3] * c[3][2] - c[0][0] * c[1][1] * c[2][3] * c[3][2] - c[0][2] * c[1][1] * c[2][0] * c[3][3] + c[0][1] * c[1][2] * c[2][0] * c[3][3] +
			   c[0][2] * c[1][0] * c[2][1] * c[3][3] - c[0][0] * c[1][2] * c[2][1] * c[3][3] - c[0][1] * c[1][0] * c[2][2] * c[3][3] + c[0][0] * c[1][1] * c[2][2] * c[3][3];
	}

	Mat4 inverted() const
	{
		Mat4 m(NO_INIT);

		float d = determinant();
		if (d == 0)
			return m;
		d = 1.0f / d;

		m.c[0][0] = d * (c[1][2] * c[2][3] * c[3][1] - c[1][3] * c[2][2] * c[3][1] + c[1][3] * c[2][1] * c[3][2] - c[1][1] * c[2][3] * c[3][2] - c[1][2] * c[2][1] * c[3][3] + c[1][1] * c[2][2] * c[3][3]);
		m.c[0][1] = d * (c[0][3] * c[2][2] * c[3][1] - c[0][2] * c[2][3] * c[3][1] - c[0][3] * c[2][1] * c[3][2] + c[0][1] * c[2][3] * c[3][2] + c[0][2] * c[2][1] * c[3][3] - c[0][1] * c[2][2] * c[3][3]);
		m.c[0][2] = d * (c[0][2] * c[1][3] * c[3][1] - c[0][3] * c[1][2] * c[3][1] + c[0][3] * c[1][1] * c[3][2] - c[0][1] * c[1][3] * c[3][2] - c[0][2] * c[1][1] * c[3][3] + c[0][1] * c[1][2] * c[3][3]);
		m.c[0][3] = d * (c[0][3] * c[1][2] * c[2][1] - c[0][2] * c[1][3] * c[2][1] - c[0][3] * c[1][1] * c[2][2] + c[0][1] * c[1][3] * c[2][2] + c[0][2] * c[1][1] * c[2][3] - c[0][1] * c[1][2] * c[2][3]);
		m.c[1][0] = d * (c[1][3] * c[2][2] * c[3][0] - c[1][2] * c[2][3] * c[3][0] - c[1][3] * c[2][0] * c[3][2] + c[1][0] * c[2][3] * c[3][2] + c[1][2] * c[2][0] * c[3][3] - c[1][0] * c[2][2] * c[3][3]);
		m.c[1][1] = d * (c[0][2] * c[2][3] * c[3][0] - c[0][3] * c[2][2] * c[3][0] + c[0][3] * c[2][0] * c[3][2] - c[0][0] * c[2][3] * c[3][2] - c[0][2] * c[2][0] * c[3][3] + c[0][0] * c[2][2] * c[3][3]);
		m.c[1][2] = d * (c[0][3] * c[1][2] * c[3][0] - c[0][2] * c[1][3] * c[3][0] - c[0][3] * c[1][0] * c[3][2] + c[0][0] * c[1][3] * c[3][2] + c[0][2] * c[1][0] * c[3][3] - c[0][0] * c[1][2] * c[3][3]);
		m.c[1][3] = d * (c[0][2] * c[1][3] * c[2][0] - c[0][3] * c[1][2] * c[2][0] + c[0][3] * c[1][0] * c[2][2] - c[0][0] * c[1][3] * c[2][2] - c[0][2] * c[1][0] * c[2][3] + c[0][0] * c[1][2] * c[2][3]);
		m.c[2][0] = d * (c[1][1] * c[2][3] * c[3][0] - c[1][3] * c[2][1] * c[3][0] + c[1][3] * c[2][0] * c[3][1] - c[1][0] * c[2][3] * c[3][1] - c[1][1] * c[2][0] * c[3][3] + c[1][0] * c[2][1] * c[3][3]);
		m.c[2][1] = d * (c[0][3] * c[2][1] * c[3][0] - c[0][1] * c[2][3] * c[3][0] - c[0][3] * c[2][0] * c[3][1] + c[0][0] * c[2][3] * c[3][1] + c[0][1] * c[2][0] * c[3][3] - c[0][0] * c[2][1] * c[3][3]);
		m.c[2][2] = d * (c[0][1] * c[1][3] * c[3][0] - c[0][3] * c[1][1] * c[3][0] + c[0][3] * c[1][0] * c[3][1] - c[0][0] * c[1][3] * c[3][1] - c[0][1] * c[1][0] * c[3][3] + c[0][0] * c[1][1] * c[3][3]);
		m.c[2][3] = d * (c[0][3] * c[1][1] * c[2][0] - c[0][1] * c[1][3] * c[2][0] - c[0][3] * c[1][0] * c[2][1] + c[0][0] * c[1][3] * c[2][1] + c[0][1] * c[1][0] * c[2][3] - c[0][0] * c[1][1] * c[2][3]);
		m.c[3][0] = d * (c[1][2] * c[2][1] * c[3][0] - c[1][1] * c[2][2] * c[3][0] - c[1][2] * c[2][0] * c[3][1] + c[1][0] * c[2][2] * c[3][1] + c[1][1] * c[2][0] * c[3][2] - c[1][0] * c[2][1] * c[3][2]);
		m.c[3][1] = d * (c[0][1] * c[2][2] * c[3][0] - c[0][2] * c[2][1] * c[3][0] + c[0][2] * c[2][0] * c[3][1] - c[0][0] * c[2][2] * c[3][1] - c[0][1] * c[2][0] * c[3][2] + c[0][0] * c[2][1] * c[3][2]);
		m.c[3][2] = d * (c[0][2] * c[1][1] * c[3][0] - c[0][1] * c[1][2] * c[3][0] - c[0][2] * c[1][0] * c[3][1] + c[0][0] * c[1][2] * c[3][1] + c[0][1] * c[1][0] * c[3][2] - c[0][0] * c[1][1] * c[3][2]);
		m.c[3][3] = d * (c[0][1] * c[1][2] * c[2][0] - c[0][2] * c[1][1] * c[2][0] + c[0][2] * c[1][0] * c[2][1] - c[0][0] * c[1][2] * c[2][1] - c[0][1] * c[1][0] * c[2][2] + c[0][0] * c[1][1] * c[2][2]);

		return m;
	}

	bool decompose(Vec3 *scale, Quaternion *rotation, Vec3 *translation) const
	{
		if (translation)
		{
			// Extract the translation.
			translation->x = x[12];
			translation->y = x[13];
			translation->z = x[14];
		}

		// Nothing left to do.
		if (scale == NULL && rotation == NULL)
			return true;

		// Extract the scale.
		// This is simply the length of each axis (row/column) in the matrix.
		Vec3 xaxis(x[0], x[1], x[2]);
		float scaleX = xaxis.length();

		Vec3 yaxis(x[4], x[5], x[6]);
		float scaleY = yaxis.length();

		Vec3 zaxis(x[8], x[9], x[10]);
		float scaleZ = zaxis.length();

		// Determine if we have a negative scale (true if determinant is less than zero).
		// In this case, we simply negate a single axis of the scale.
		float det = determinant();
		if (det < 0)
			scaleZ = -scaleZ;

		if (scale)
		{
			scale->x = scaleX;
			scale->y = scaleY;
			scale->z = scaleZ;
		}

		// Nothing left to do.
		if (rotation == NULL)
			return true;

		// Scale too close to zero, can't decompose rotation.
		if (scaleX < MATH_TOLERANCE || scaleY < MATH_TOLERANCE || fabs(scaleZ) < MATH_TOLERANCE)
			return false;

		float rn;

		// Factor the scale out of the matrix axes.
		rn = 1.0f / scaleX;
		xaxis.x *= rn;
		xaxis.y *= rn;
		xaxis.z *= rn;

		rn = 1.0f / scaleY;
		yaxis.x *= rn;
		yaxis.y *= rn;
		yaxis.z *= rn;

		rn = 1.0f / scaleZ;
		zaxis.x *= rn;
		zaxis.y *= rn;
		zaxis.z *= rn;

		// Now calculate the rotation from the resulting matrix (axes).
		float trace = xaxis.x + yaxis.y + zaxis.z + 1.0f;

		if (trace > 1.0f)
		{
			float s = 0.5f / sqrt(trace);
			rotation->w = 0.25f / s;
			rotation->x = (yaxis.z - zaxis.y) * s;
			rotation->y = (zaxis.x - xaxis.z) * s;
			rotation->z = (xaxis.y - yaxis.x) * s;
		}
		else
		{
			// Note: since xaxis, yaxis, and zaxis are normalized,
			// we will never divide by zero in the code below.
			if (xaxis.x > yaxis.y && xaxis.x > zaxis.z)
			{
				float s = 0.5f / sqrt(1.0f + xaxis.x - yaxis.y - zaxis.z);
				rotation->w = (yaxis.z - zaxis.y) * s;
				rotation->x = 0.25f / s;
				rotation->y = (yaxis.x + xaxis.y) * s;
				rotation->z = (zaxis.x + xaxis.z) * s;
			}
			else if (yaxis.y > zaxis.z)
			{
				float s = 0.5f / sqrt(1.0f + yaxis.y - xaxis.x - zaxis.z);
				rotation->w = (zaxis.x - xaxis.z) * s;
				rotation->x = (yaxis.x + xaxis.y) * s;
				rotation->y = 0.25f / s;
				rotation->z = (zaxis.y + yaxis.z) * s;
			}
			else
			{
				float s = 0.5f / sqrt(1.0f + zaxis.z - xaxis.x - yaxis.y);
				rotation->w = (xaxis.y - yaxis.x) * s;
				rotation->x = (zaxis.x + xaxis.z) * s;
				rotation->y = (zaxis.y + yaxis.z) * s;
				rotation->z = 0.25f / s;
			}
		}

		return true;
	}
	void decompose(Vec3 &trans, Vec3 &rot, Vec3 &scale) const
	{
		// Getting translation is trivial
		trans = Vec3(c[3][0], c[3][1], c[3][2]);

		// Scale is length of columns
		scale.x = sqrtf(c[0][0] * c[0][0] + c[0][1] * c[0][1] + c[0][2] * c[0][2]);
		scale.y = sqrtf(c[1][0] * c[1][0] + c[1][1] * c[1][1] + c[1][2] * c[1][2]);
		scale.z = sqrtf(c[2][0] * c[2][0] + c[2][1] * c[2][1] + c[2][2] * c[2][2]);

		if (scale.x == 0 || scale.y == 0 || scale.z == 0)
			return;

		// Detect negative scale with determinant and flip one arbitrary axis
		if (determinant() < 0)
			scale.x = -scale.x;

		// Combined rotation matrix YXZ
		//
		// Cos[y]*Cos[z]+Sin[x]*Sin[y]*Sin[z]   Cos[z]*Sin[x]*Sin[y]-Cos[y]*Sin[z]  Cos[x]*Sin[y]
		// Cos[x]*Sin[z]                        Cos[x]*Cos[z]                       -Sin[x]
		// -Cos[z]*Sin[y]+Cos[y]*Sin[x]*Sin[z]  Cos[y]*Cos[z]*Sin[x]+Sin[y]*Sin[z]  Cos[x]*Cos[y]

		rot.x = asinf(-c[2][1] / scale.z);

		// Special case: Cos[x] == 0 (when Sin[x] is +/-1)
		float f = fabsf(c[2][1] / scale.z);
		if (f > 0.999f && f < 1.001f)
		{
			// Pin arbitrarily one of y or z to zero
			// Mathematical equivalent of gimbal lock
			rot.y = 0;

			// Now: Cos[x] = 0, Sin[x] = +/-1, Cos[y] = 1, Sin[y] = 0
			// => m[0][0] = Cos[z] and m[1][0] = Sin[z]
			rot.z = atan2f(-c[1][0] / scale.y, c[0][0] / scale.x);
		}
		// Standard case
		else
		{
			rot.y = atan2f(c[2][0] / scale.z, c[2][2] / scale.z);
			rot.z = atan2f(c[0][1] / scale.x, c[1][1] / scale.y);
		}
	}

	void setCol(unsigned int col, const Vec4 &v)
	{
		x[col * 4 + 0] = v.x;
		x[col * 4 + 1] = v.y;
		x[col * 4 + 2] = v.z;
		x[col * 4 + 3] = v.w;
	}

	Vec4 getCol(unsigned int col) const
	{
		return Vec4(x[col * 4 + 0], x[col * 4 + 1], x[col * 4 + 2], x[col * 4 + 3]);
	}

	Vec4 getRow(unsigned int row) const
	{
		return Vec4(x[row + 0], x[row + 4], x[row + 8], x[row + 12]);
	}

	Vec3 getTrans() const
	{
		return Vec3(c[3][0], c[3][1], c[3][2]);
	}

	Vec3 getScale() const
	{
		Vec3 scale;
		// Scale is length of columns
		scale.x = sqrtf(c[0][0] * c[0][0] + c[0][1] * c[0][1] + c[0][2] * c[0][2]);
		scale.y = sqrtf(c[1][0] * c[1][0] + c[1][1] * c[1][1] + c[1][2] * c[1][2]);
		scale.z = sqrtf(c[2][0] * c[2][0] + c[2][1] * c[2][1] + c[2][2] * c[2][2]);
		return scale;
	}
	void identity()
	{
		c[0][0] = 1;
		c[1][0] = 0;
		c[2][0] = 0;
		c[3][0] = 0;
		c[0][1] = 0;
		c[1][1] = 1;
		c[2][1] = 0;
		c[3][1] = 0;
		c[0][2] = 0;
		c[1][2] = 0;
		c[2][2] = 1;
		c[3][2] = 0;
		c[0][3] = 0;
		c[1][3] = 0;
		c[2][3] = 0;
		c[3][3] = 1;
	}
	static Vec3 Transform(const Mat4 &m, const Vec3 &v)
	{
		return Vec3(v.x * m.c[0][0] + v.y * m.c[1][0] + v.z * m.c[2][0] + m.c[3][0],
					v.x * m.c[0][1] + v.y * m.c[1][1] + v.z * m.c[2][1] + m.c[3][1],
					v.x * m.c[0][2] + v.y * m.c[1][2] + v.z * m.c[2][2] + m.c[3][2]);
	}

	static Vec4 Transform(const Mat4 &m, const Vec4 &v)
	{
		return Vec4(v.x * m.c[0][0] + v.y * m.c[1][0] + v.z * m.c[2][0] + v.w * m.c[3][0],
					v.x * m.c[0][1] + v.y * m.c[1][1] + v.z * m.c[2][1] + v.w * m.c[3][1],
					v.x * m.c[0][2] + v.y * m.c[1][2] + v.z * m.c[2][2] + v.w * m.c[3][2],
					v.x * m.c[0][3] + v.y * m.c[1][3] + v.z * m.c[2][3] + v.w * m.c[3][3]);
	}

	static Vec3 TranformNormal(const Mat4 &m, const Vec3 &v)
	{
		return Vec3(v.x * m.c[0][0] + v.y * m.c[1][0] + v.z * m.c[2][0],
					v.x * m.c[0][1] + v.y * m.c[1][1] + v.z * m.c[2][1],
					v.x * m.c[0][2] + v.y * m.c[1][2] + v.z * m.c[2][2]);
	}

	static Vec4 TranformNormal(const Mat4 &m, const Vec4 &v)
	{
		return Vec4(v.x * m.c[0][0] + v.y * m.c[1][0] + v.z * m.c[2][0],
					v.x * m.c[0][1] + v.y * m.c[1][1] + v.z * m.c[2][1],
					v.x * m.c[0][2] + v.y * m.c[1][2] + v.z * m.c[2][2],
					v.x * m.c[0][3] + v.y * m.c[1][3] + v.z * m.c[2][3]);
	}
};

// -------------------------------------------------------------------------------------------------
// Intersection
// -------------------------------------------------------------------------------------------------

inline bool rayTriangleIntersection(const Vec3 &rayOrig, const Vec3 &rayDir,
									const Vec3 &vert0, const Vec3 &vert1, const Vec3 &vert2,
									Vec3 &intsPoint)
{
	// Idea: Tomas Moeller and Ben Trumbore
	// in Fast, Minimum Storage Ray/Triangle Intersection

	// Find vectors for two edges sharing vert0
	Vec3 edge1 = vert1 - vert0;
	Vec3 edge2 = vert2 - vert0;

	// Begin calculating determinant - also used to calculate U parameter
	Vec3 pvec = rayDir.cross(edge2);

	// If determinant is near zero, ray lies in Plane3D of triangle
	float det = edge1.dot(pvec);

	// *** Culling branch ***
	/*if( det < Epsilon )return false;

	// Calculate distance from vert0 to ray origin
	Vec3 tvec = rayOrig - vert0;

	// Calculate U parameter and test bounds
	float u = tvec.dot( pvec );
	if (u < 0 || u > det ) return false;

	// Prepare to test V parameter
	Vec3 qvec = tvec.cross( edge1 );

	// Calculate V parameter and test bounds
	float v = rayDir.dot( qvec );
	if (v < 0 || u + v > det ) return false;

	// Calculate t, scale parameters, ray intersects triangle
	float t = edge2.dot( qvec ) / det;*/

	// *** Non-culling branch ***
	if (det > -Epsilon && det < Epsilon)
		return 0;
	float inv_det = 1.0f / det;

	// Calculate distance from vert0 to ray origin
	Vec3 tvec = rayOrig - vert0;

	// Calculate U parameter and test bounds
	float u = tvec.dot(pvec) * inv_det;
	if (u < 0.0f || u > 1.0f)
		return 0;

	// Prepare to test V parameter
	Vec3 qvec = tvec.cross(edge1);

	// Calculate V parameter and test bounds
	float v = rayDir.dot(qvec) * inv_det;
	if (v < 0.0f || u + v > 1.0f)
		return 0;

	// Calculate t, ray intersects triangle
	float t = edge2.dot(qvec) * inv_det;

	// Calculate intersection point and test ray length and direction
	intsPoint = rayOrig + rayDir * t;
	Vec3 vec = intsPoint - rayOrig;
	if (vec.dot(rayDir) < 0 || vec.length() > rayDir.length())
		return false;

	return true;
}

inline bool rayAABBIntersection(const Vec3 &rayOrig, const Vec3 &rayDir,
								const Vec3 &mins, const Vec3 &maxs)
{
	// SLAB based optimized ray/AABB intersection routine
	// Idea taken from http://ompf.org/ray/

	float l1 = (mins.x - rayOrig.x) / rayDir.x;
	float l2 = (maxs.x - rayOrig.x) / rayDir.x;
	float lmin = Min(l1, l2);
	float lmax = Max(l1, l2);

	l1 = (mins.y - rayOrig.y) / rayDir.y;
	l2 = (maxs.y - rayOrig.y) / rayDir.y;
	lmin = Max(Min(l1, l2), lmin);
	lmax = Min(Max(l1, l2), lmax);

	l1 = (mins.z - rayOrig.z) / rayDir.z;
	l2 = (maxs.z - rayOrig.z) / rayDir.z;
	lmin = Max(Min(l1, l2), lmin);
	lmax = Min(Max(l1, l2), lmax);

	if ((lmax >= 0.0f) & (lmax >= lmin))
	{
		// Consider length
		const Vec3 rayDest = rayOrig + rayDir;
		Vec3 rayMins(Min(rayDest.x, rayOrig.x), Min(rayDest.y, rayOrig.y), Min(rayDest.z, rayOrig.z));
		Vec3 rayMaxs(Max(rayDest.x, rayOrig.x), Max(rayDest.y, rayOrig.y), Max(rayDest.z, rayOrig.z));
		return (rayMins.x < maxs.x) && (rayMaxs.x > mins.x) &&
			   (rayMins.y < maxs.y) && (rayMaxs.y > mins.y) &&
			   (rayMins.z < maxs.z) && (rayMaxs.z > mins.z);
	}
	else
		return false;
}

inline float nearestDistToAABB(const Vec3 &pos, const Vec3 &mins, const Vec3 &maxs)
{
	const Vec3 center = (mins + maxs) * 0.5f;
	const Vec3 extent = (maxs - mins) * 0.5f;

	Vec3 nearestVec;
	nearestVec.x = Max(0.0f, fabsf(pos.x - center.x) - extent.x);
	nearestVec.y = Max(0.0f, fabsf(pos.y - center.y) - extent.y);
	nearestVec.z = Max(0.0f, fabsf(pos.z - center.z) - extent.z);

	return nearestVec.length();
}

// -------------------------------------------------------------------------------------------------
// RECT
// -------------------------------------------------------------------------------------------------
template <typename T>
struct Rect
{

	T x;
	T y;
	T width;
	T height;

	Rect() : x(0), y(0), width(0), height(0) {}
	Rect(T x, T y, T width, T height) : x(x), y(y), width(width), height(height) {}
	Rect(const Rect &rect) : x(rect.x), y(rect.y), width(rect.width), height(rect.height) {}

	void Set(T x, T y, T width, T height)
	{
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
	}

	void Merge(const Rect &rect)
	{
		T right = x + width;
		T bottom = y + height;
		T rectRight = rect.x + rect.width;
		T rectBottom = rect.y + rect.height;
		x = Min(x, rect.x);
		y = Min(y, rect.y);
		right = Max(right, rectRight);
		bottom = Max(bottom, rectBottom);
		width = right - x;
		height = bottom - y;
	}

	void Merge(const Vec2 &point)
	{
		T right = x + width;
		T bottom = y + height;
		x = Min(x, point.x);
		y = Min(y, point.y);
		right = Max(right, point.x);
		bottom = Max(bottom, point.y);
		width = right - x;
		height = bottom - y;
	}

	void Clear()
	{
		x = 0;
		y = 0;
		width = 0;
		height = 0;
	}

	Rect &operator=(const Rect &rect)
	{
		if (this == &rect)
			return *this;
		x = rect.x;
		y = rect.y;
		width = rect.width;
		height = rect.height;
		return *this;
	}
};

// -------------------------------------------------------------------------------------------------
// SIZE
// -------------------------------------------------------------------------------------------------
template <typename T>
struct Size
{
	T width;
	T height;

	Size() : width(0), height(0) {}
	Size(T w, T h) : width(w), height(h) {}
	Size(const Size &size) : width(size.width), height(size.height) {}

	Size &operator=(const Size &size)
	{
		if (this == &size)
			return *this;
		width = size.width;
		height = size.height;
		return *this;
	}
};

typedef Rect<int> IntRect;
typedef Rect<float> FloatRect;
typedef Size<int> IntSize;
typedef Size<float> FloatSize;

// -------------------------------------------------------------------------------------------------
// Plane
// -------------------------------------------------------------------------------------------------

class Plane
{
public:
	static const int INTERSECTS_INTERSECTING = 0;
	static const int INTERSECTS_FRONT = 1;
	static const int INTERSECTS_BACK = -1;

	Plane()
	{
		m_normal.x = 0;
		m_normal.y = 1;
		m_normal.z = 0;
		m_distance = 0;
	};

	Plane(const Vec3 &normal, float distance)
	{
		set(normal, distance);
	}
	Plane(float normalX, float normalY, float normalZ, float distance)
	{
		set(Vec3(normalX, normalY, normalZ), distance);
	}
	Plane(const Plane &copy)
	{
		set(copy);
	}

	Plane(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2)
	{
		m_normal = v1 - v0;
		m_normal = m_normal.cross(v2 - v0);
		m_normal.normalize();
		m_distance = -m_normal.dot(v0);
	}
	const Vec3 &getNormal() const
	{
		return m_normal;
	}

	void setNormal(const Vec3 &normal)
	{
		m_normal.set(normal);
		normalize();
	}

	void setNormal(float x, float y, float z)
	{
		m_normal.set(x, y, z);
		normalize();
	}

	float getDistance() const
	{
		return m_distance;
	}

	void setDistance(float distance)
	{
		m_distance = distance;
	}

	float distance(const Vec3 &point) const
	{
		return m_normal.x * point.x + m_normal.y * point.y + m_normal.z * point.z + m_distance;
	}

	static void intersection(const Plane &p1, const Plane &p2, const Plane &p3, Vec3 *point);

	float intersects(const BoundingSphere &sphere) const;
	float intersects(const BoundingBox &box) const;
	float intersects(const Frustum &frustum) const;
	float intersects(const Plane &plane) const;
	float intersects(const Ray &ray) const;

	bool isParallel(const Plane &plane) const;
	void set(const Vec3 &normal, float distance);
	void set(const Plane &plane);
	void transform(const Mat4 &matrix);

	Plane &operator*=(const Mat4 &matrix)
	{
		transform(matrix);
		return *this;
	}
	float distToPoint(const Vec3 &v) const
	{
		return m_normal.dot(v) + m_distance;
	}

private:
	void normalize();
	Vec3 m_normal;
	float m_distance;
};

const Plane operator*(const Mat4 &matrix, const Plane &plane);
// -------------------------------------------------------------------------------------------------
// BOUNDING BOX
// -------------------------------------------------------------------------------------------------
class BoundingBox
{
public:
	Vec3 min;
	Vec3 max;

	BoundingBox();
	BoundingBox(const Vec3 &min, const Vec3 &max);
	BoundingBox(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
	BoundingBox(const BoundingBox &copy);
	~BoundingBox();
	static const BoundingBox &empty();
	Vec3 getCenter() const;
	Vec3 getCorner(u32 index) const;
	void getCenter(Vec3 *dst) const;
	void getCorners(Vec3 *dst) const;
	void getEdges(Vec3 *dst) const;
	bool intersects(const BoundingBox &box) const;
	bool intersects(const BoundingSphere &sphere) const;
	bool intersects(const Frustum &frustum) const;
	float intersects(const Plane &plane) const;
	float intersects(const Ray &ray) const;
	bool isEmpty() const;
	void merge(const BoundingSphere &sphere);
	void merge(const BoundingBox &box);
	void set(const Vec3 &min, const Vec3 &max);
	void set(float minX, float minY, float minZ, float maxX, float maxY, float maxZ);
	void set(const BoundingBox &box);
	void set(const BoundingSphere &sphere);
	void transform(const Mat4 &matrix);
	void clear();
	void addPoint(const Vec3 &p);
	BoundingBox &operator*=(const Mat4 &matrix)
	{
		transform(matrix);
		return *this;
	}
};
const BoundingBox operator*(const Mat4 &matrix, const BoundingBox &box);

// -------------------------------------------------------------------------------------------------
// BOUNDING SPHERE
// -------------------------------------------------------------------------------------------------
class BoundingSphere
{
public:
	Vec3 center;
	float radius;
	BoundingSphere();
	BoundingSphere(const Vec3 &center, float radius);
	BoundingSphere(const BoundingSphere &copy);
	~BoundingSphere();
	static const BoundingSphere &empty();
	bool intersects(const BoundingSphere &sphere) const;
	bool intersects(const BoundingBox &box) const;
	bool intersects(const Frustum &frustum) const;
	float intersects(const Plane &plane) const;
	float intersects(const Ray &ray) const;
	bool isEmpty() const;
	void merge(const BoundingSphere &sphere);
	void merge(const BoundingBox &box);
	void set(const Vec3 &center, float radius);
	void set(const BoundingSphere &sphere);
	void set(const BoundingBox &box);
	void transform(const Mat4 &matrix);

	BoundingSphere &operator*=(const Mat4 &matrix)
	{
		transform(matrix);
		return *this;
	}

private:
	float distance(const BoundingSphere &sphere, const Vec3 &);

	bool contains(const BoundingSphere &sphere, Vec3 *points, unsigned int count);
};

// -------------------------------------------------------------------------------------------------
// RAY
// -------------------------------------------------------------------------------------------------
class Ray
{
public:
	static const int INTERSECTS_NONE = -1;
	Ray() : m_direction(0, 0, 1)
	{
	}

	Ray(const Vec3 &origin, const Vec3 &direction)
	{
		set(origin, direction);
	}

	Ray(float originX, float originY, float originZ, float dirX, float dirY, float dirZ)
	{
		set(Vec3(originX, originY, originZ), Vec3(dirX, dirY, dirZ));
	}

	Ray(const Ray &copy)
	{
		set(copy);
	}

	~Ray()
	{
	}

	const Vec3 &getOrigin() const
	{
		return m_origin;
	}

	void setOrigin(const Vec3 &origin)
	{
		m_origin = origin;
	}

	void setOrigin(float x, float y, float z)
	{
		m_origin.set(x, y, z);
	}

	const Vec3 &getDirection() const
	{
		return m_direction;
	}

	void setDirection(const Vec3 &direction)
	{
		m_direction = direction;
		normalize();
	}

	void setDirection(float x, float y, float z)
	{
		m_direction.set(x, y, z);
		normalize();
	}

	float intersects(const BoundingSphere &sphere) const;
	float intersects(const BoundingBox &box) const;
	float intersects(const Frustum &frustum) const;
	float intersects(const Plane &plane) const;

	void set(const Vec3 &origin, const Vec3 &direction)
	{
		m_origin = origin;
		m_direction = direction;
		normalize();
	}

	void set(const Ray &ray)
	{
		m_origin = ray.m_origin;
		m_direction = ray.m_direction;
		normalize();
	}

	void transform(const Mat4 &matrix)
	{

		m_origin = Mat4::Transform(matrix, m_origin);
		m_direction = Mat4::TranformNormal(matrix, m_direction);
		m_direction.normalize();
	}

	Ray &operator*=(const Mat4 &matrix)
	{
		transform(matrix);
		return *this;
	}

private:
	void normalize();

	Vec3 m_origin;	  // The ray origin position.
	Vec3 m_direction; // The ray direction vector.
};
const Ray operator*(const Mat4 &matrix, const Ray &ray);

// -------------------------------------------------------------------------------------------------
// FRUSTUM
// -------------------------------------------------------------------------------------------------
class Frustum
{
public:
	Frustum();
	Frustum(const Mat4 &matrix);
	Frustum(const Frustum &frustum);
	~Frustum();
	const Plane &getNear() const;
	const Plane &getFar() const;
	const Plane &getLeft() const;
	const Plane &getRight() const;
	const Plane &getBottom() const;
	const Plane &getTop() const;
	void getMat4(Mat4 *dst) const;
	void getCorners(Vec3 *corners) const;
	void getNearCorners(Vec3 *corners) const;
	void getFarCorners(Vec3 *corners) const;
	bool intersects(const Vec3 &point) const;
	bool intersects(float x, float y, float z) const;
	bool intersects(const BoundingSphere &sphere) const;
	bool intersects(const BoundingBox &box) const;
	float intersects(const Plane &plane) const;
	float intersects(const Ray &ray) const;
	void set(const Frustum &frustum);
	void set(const Mat4 &matrix);

private:
	void updatePlanes();

	Plane m_near;
	Plane m_far;
	Plane m_bottom;
	Plane m_top;
	Plane m_left;
	Plane m_right;
	Mat4 m_matrix;
};

// -------------------------------------------------------------------------------------------------
// TRIANGLE3D
// -------------------------------------------------------------------------------------------------
struct Triangle3D
{
	Triangle3D() {}

	Triangle3D(const Vec3 &v1, const Vec3 &v2, const Vec3 &v3) : pointA(v1), pointB(v2), pointC(v3) {}

	//! Equality operator
	bool operator==(const Triangle3D &other) const
	{
		return other.pointA == pointA && other.pointB == pointB && other.pointC == pointC;
	}

	//! Inequality operator
	bool operator!=(const Triangle3D &other) const
	{
		return !(*this == other);
	}

	Plane getPlane() const
	{
		return Plane(pointA, pointB, pointC);
	}

	//! Get the area of the triangle
	float getArea() const
	{
		return (pointB - pointA).cross(pointC - pointA).length_squared() * 0.5f;
	}
	bool isFrontFacing(const Vec3 &lookDirection) const
	{
		const Vec3 n = getNormal().normalized();
		const float d = (float)n.dot(lookDirection);
		return (d <= 0.0f);
	}
	Vec3 getNormal() const
	{
		return (pointB - pointA).cross(pointC - pointA);
	}

	//! sets the triangle's points
	void set(const Vec3 &a, const Vec3 &b, const Vec3 &c)
	{
		pointA = a;
		pointB = b;
		pointC = c;
	}

	Vec3 pointA;
	Vec3 pointB;
	Vec3 pointC;
};
