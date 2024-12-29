
#pragma once
#include <cmath>
#include <cstring>

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
const float ToRad = Pi / 180.0f;
const float ToDeg = 180.0f / Pi;
const float dtor = 0.0174532925199432957692369076848861f;
const float rtod = 1 / dtor;

const float Epsilon = 0.000001f;
const float ZeroEpsilon = 32.0f * MinPosFloat;

const float M_INFINITY = 1.0e30f;

template <typename T>
T Min(const T &a, const T &b) { return (a < b) ? a : b; }

template <typename T>
T Max(const T &a, const T &b) { return (a > b) ? a : b; }

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

inline float RangeRandom(float min, float max) { return min + (max - min) * ((float)rand() / (float)RAND_MAX); }
inline int RangeRandom(int min, int max) { return min + (max - min) * ((float)rand() / (float)RAND_MAX); }

inline bool IsNotZero(const float a, const float tolerance = Epsilon)
{
	return fabsf(a) > tolerance;
}

inline bool IsEqual(const float a, const float b, const float tolerance = Epsilon)
{
	if (fabs(b - a) <= tolerance)
		return true;
	else
		return false;
}

inline float Radians(float degrees) { return degrees * Pi / 180.0f; }
inline float Degrees(float radians) { return radians * 180.0f / Pi; }

inline float DegToRad(float f)
{
	return f * 0.017453293f;
}

inline float RadToDeg(float f)
{
	return f * 57.29577951f;
}

struct Vec2;
struct Vec3;
struct Vec4;
struct Mat3;
struct Mat4;
struct Quat;
struct Triangle;
struct Ray;
struct BoundingBox;
struct Plane;


void Transform(const Mat4& mat, const Vec3& vec, Vec3& out);
void Transform(const Mat4& mat, const Vec4& vec, Vec4& out);
void Transform(const Mat3& mat, const Vec3& vec, Vec3& out);
void TransformNormal(const Mat4& mat, const Vec3& vec, Vec3& out);



//***************************************************************************************************************
// VECTOR 2D
//***************************************************************************************************************

struct Vec2
{
	float x, y;

	Vec2();
	Vec2(float x, float y);

	Vec2 operator+(const Vec2 &other) const;
	Vec2 operator-(const Vec2 &other) const;
	Vec2 operator*(float scalar) const;
	Vec2 operator/(float scalar) const;

	Vec2 &operator+=(const Vec2 &other);
	Vec2 &operator-=(const Vec2 &other);
	Vec2 &operator*=(float scalar);
	Vec2 &operator/=(float scalar);

	Vec2 operator-() const;

	float length() const;
	float lengthSquared() const;
	Vec2 &normalize();
	Vec2 normal() const;

	float dot(const Vec2 &other) const;
	float angle(const Vec2 &other) const;
	Vec2 rotate(float radians) const;
	Vec2 rotate(float radians, const Vec2 &center) const;

	Vec2 clamp(const Vec2 &min, const Vec2 &max) const;

	bool operator==(const Vec2 &other) const;
	bool operator!=(const Vec2 &other) const;

	float &operator[](int index);
	const float &operator[](int index) const;

	static float Distance(const Vec2 &a, const Vec2 &b);
	static float DistanceSquared(const Vec2 &a, const Vec2 &b);
	static Vec2 Lerp(const Vec2 &a, const Vec2 &b, float t);
	static Vec2 Normal(const Vec2 &a, const Vec2 &b);
	static float Angle(const Vec2 &a, const Vec2 &b);
};

//***************************************************************************************************************
// VECTOR 3D
//***************************************************************************************************************

struct Vec3
{
	float x, y, z;

	Vec3();
	Vec3(float x, float y, float z);

	Vec3 operator+(const Vec3 &other) const;
	Vec3 operator-(const Vec3 &other) const;
	Vec3 operator*(float scalar) const;
	Vec3 operator/(float scalar) const;

	Vec3 &operator+=(const Vec3 &other);
	Vec3 &operator-=(const Vec3 &other);
	Vec3 &operator*=(float scalar);
	Vec3 &operator/=(float scalar);

	Vec3 operator-() const;


	void set(float x, float y, float z);

	float &operator[](int index);
	const float &operator[](int index) const;

	float length() const;
	float lengthSquared() const;

	Vec3 &normalize();
	Vec3 normal() const;

	float dot(const Vec3 &other) const;
	Vec3 cross(const Vec3 &other) const;

	Vec3 rotate(float angle, const Vec3 &axis) const;
	Vec3 project(const Vec3 &other) const;
	Vec3 reflect(const Vec3 &normal) const;

	bool operator==(const Vec3 &other) const;
	bool operator!=(const Vec3 &other) const;

	static float Distance(const Vec3 &a, const Vec3 &b);
	static float DistanceSquared(const Vec3 &a, const Vec3 &b);
	static Vec3 Lerp(const Vec3 &a, const Vec3 &b, float t);
	static Vec3 Normal(const Vec3 &a, const Vec3 &b);
	static Vec3 Normalize(const Vec3 &a);
	static Vec3 Cross(const Vec3 &a, const Vec3 &b);
	static float Dot(const Vec3 &a, const Vec3 &b);
	
};

Vec3 operator*(float scalar, const Vec3& vec);

//***************************************************************************************************************
// VECTOR 4D
//***************************************************************************************************************

struct Vec4
{
	float x, y, z, w;

	Vec4();
	Vec4(float x, float y, float z, float w = 1.0f);

	Vec4 operator+(const Vec4 &other) const;
	Vec4 operator-(const Vec4 &other) const;
	Vec4 operator*(float scalar) const;
	Vec4 operator/(float scalar) const;

	Vec4 &operator+=(const Vec4 &other);
	Vec4 &operator-=(const Vec4 &other);
	Vec4 &operator*=(float scalar);
	Vec4 &operator/=(float scalar);

	float &operator[](int index);
	const float &operator[](int index) const;



	Vec3 toVec3() const;

	float length() const;
	float lengthSquared() const;
	Vec4 &normalize();
	Vec4 normal() const;

	static Vec4 Normal(const Vec4 &a, const Vec4 &b);

	bool operator==(const Vec4 &other) const;
	bool operator!=(const Vec4 &other) const;
};

//***************************************************************************************************************
// MATRIX 3X3
//***************************************************************************************************************

struct Mat3
{
	float m[9];

	Mat3();
	Mat3(float diagonal);
	Mat3(const float *elements);

	float &at(int row, int col);
	const float &at(int row, int col) const;

	Mat3 operator+(const Mat3 &other) const;
	Mat3 operator-(const Mat3 &other) const;
	Mat3 operator*(const Mat3 &other) const;
	Vec3 operator*(const Vec3 &vec) const;
	Mat3 operator*(float scalar) const;
	Mat3 &operator*=(const Mat3 &other);
	Mat3 &operator*=(float scalar);

	Mat3 transpose() const;
	float determinant() const;
	Mat3 inverse() const;

	const float &operator[](int row) const;
	float &operator[](int row);

	static Mat3 Identity();
	static Mat3 Scale(float x, float y);
	static Mat3 Rotation(float radians);
	static Mat3 RotateX(float radians);
	static Mat3 RotateY(float radians);
	static Mat3 RotateZ(float radians);
	static Mat3 Rotate(const Mat3 &mat, float radians, const Vec3 &axis);
};

//***************************************************************************************************************
// MATRIX 4X4
//***************************************************************************************************************

struct Mat4
{
	float m[16];

	Mat4();
	Mat4(float diagonal);
	Mat4(const float *elements);

	float &at(int row, int col);
	const float &at(int row, int col) const;

	const float &operator[](int row) const;
	float &operator[](int row);

	void set(float _11, float _12, float _13, float _14, float _21, float _22, float _23, float _24, float _31, float _32, float _33, float _34, float _41, float _42, float _43, float _44);

	void identity();
	void translate(float x, float y, float z);
	void scale(float x, float y, float z);
	void setRotationDegrees(const Vec3 &rotation);
	void setRotationRadians(const Vec3 &rotation);
	void setTranslation(const Vec3 &translation);
	void setScale(const Vec3 &scale);
	Vec3 getTranslation() const;

	Mat4 operator+(const Mat4 &other) const;
	Mat4 operator-(const Mat4 &other) const;
	Mat4 operator*(const Mat4 &other) const;
	
	Vec4 operator*(const Vec4 &vec) const;
	Mat4 operator*(float scalar) const;

	Mat4 &operator*=(const Mat4 &other);
	Mat4 &operator*=(float scalar);
	Mat4 &operator/=(const Vec3 &scale);
	Mat4 &operator/=(float scalar);

	Mat4 operator/(const Vec3 &scale) const;
	Mat4 operator/(float scalar) const;

    void transformPoint(Vec3& v) const;
    void transformNormal(Vec3& v) const;

	Mat4 transpose() const;
	Mat4 inverse() const;
	Vec3 getRotationInDegrees() const;
	Vec3 getRotation() const;
	Vec3 getPosition() const;

	static Mat3 ToMat3(const Mat4 &mat) ;

	static Mat4 Identity();
	static Mat4 Translate(const Vec3 &translation);
	static Mat4 Scale(const Vec3 &scale);
	static Mat4 Rotate(float radians, const Vec3 &axis);
	static Mat4 Perspective(float fov, float aspect, float near, float far);
	static Mat4 Orthographic(float left, float right, float bottom, float top, float near, float far);
	static Mat4 LookAt(const Vec3 &eye, const Vec3 &center, const Vec3 &up);

	static bool Decompose(const Mat4 &mat, Vec3 &position, Vec3 &scale, Quat &rotation);
	static bool Decompose(const Mat4 &mat, Vec3 &position, Mat3 &out);
	static Mat4 Inverse(const Mat4 &mat);
	static Mat4 Multiply(const Mat4 &left, const  Mat4 &right);
	static Mat4 Add(const Mat4& left, const Mat4& right) ;
	static Mat4 Subtract(const Mat4& left, const Mat4& right) ;
	static Mat4 Divide(const Mat4& left, const Mat4& right) ;
		

		static Mat4 Mirror(const Vec3 &normal, float d);

		static Mat4 Rotate(const Mat4 &mat, float radians, const Vec3 &axis);
		static Mat4 RotateX(const Mat4 &mat, float radians);
		static Mat4 RotateY(const Mat4 &mat, float radians);
		static Mat4 RotateZ(const Mat4 &mat, float radians);
		static Mat4 Scale(const Mat4 &mat, const Vec3 &scale);
		static Mat4 Scale(const Mat4 &mat, float x, float y, float z);
		static Mat4 Translate(const Mat4 &mat, const Vec3 &translation);
		static Mat4 Translate(const Mat4 &mat, float x, float y, float z);
};

#define MAX_STACK  26

class Stack
{
	private:
		int m_index;
		Mat4 m_stack[MAX_STACK];

	public:
		Stack();
		void push();
		void pop();
		const Mat4& top() const;

		void identity();
		void multiply(const Mat4 &m);

		void translate(float x, float y, float z);
		void scale(float x, float y, float z);
		void rotate(float radians, const Vec3 &axis);

};

//***************************************************************************************************************
// QUATERNION
//***************************************************************************************************************

class Quat
{
public:
	float x, y, z, w;

	// Construtores
	Quat();
	Quat(float x, float y, float z, float w);
	Quat(const Vec3 &axis, float angle);
	Quat(const Mat3 &rotationMatrix);

	Quat operator+(const Quat &other) const;
	Quat operator-(const Quat &other) const;
	Quat operator*(const Quat &other) const;
	Quat operator*(float scalar) const;
	Quat &operator*=(const Quat &other);
	Quat &operator*=(float scalar);

	float length() const;
	Quat normal() const;
	Quat &normalize();
	Quat conjugate() const;
	Quat inverse() const;
	Vec3 euler() const;
	Vec3 rotate(const Vec3 &v) const;

	Mat3 toMat3() const;
	Mat4 toMat4() const;

	float yaw() const;
	float pitch() const;
	float roll() const;

	static Quat Yaw(float angle);
	static Quat Pitch(float angle);
	static Quat Roll(float angle);
	static Quat FromYawPitchRoll(float yaw, float pitch, float roll);
	static Quat Slerp(const Quat &start, const Quat &end, float t);
	static Quat Lerp(const Quat &start, const Quat &end, float t);
};

//***************************************************************************************************************
//  PLANE
//***************************************************************************************************************

struct Plane
{
	enum Side
	{
		FRONT = 1,
		BACK = -1,
		COPLANAR = 0
	};

	Vec3 normal;
	float distance;

	Plane();
	Plane(const Vec3 &n, float d);
	Plane(const Vec3 &n, const Vec3 &point);
	Plane(const Vec3 &p1, const Vec3 &p2, const Vec3 &p3);
	Plane(float px, float py, float pz, float nx, float ny, float nz);

	float distanceToPoint(const Vec3 &point) const;
	Vec3 projectPoint(const Vec3 &point) const;
	bool isFrontFacing(const Vec3 &point) const;
	bool isBackFacing(const Vec3 &point) const;
	Side classifyPoint(const Vec3 &point) const;
	Side classifyBox(const BoundingBox &box) const;
	void recalculateDistance(const Vec3 &point);


};
//***************************************************************************************************************
//  TRIANGLE
//***************************************************************************************************************

struct Triangle
{
	Vec3 v0, v1, v2;

	Triangle();
	Triangle(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2);

	Vec3 normal() const;
	float area() const;
	Vec3 center() const;
	bool isFrontFacing(const Vec3 &viewPoint) const;
	bool isBackFacing(const Vec3 &viewPoint) const;
	bool containsPoint(const Vec3 &point) const;
	Vec3 getBarycentric(const Vec3 &point) const;
	void getEdges(Vec3 &e0, Vec3 &e1, Vec3 &e2) const;
	bool rayIntersection(const Ray &ray, float &t, Vec3 &barycentric) const;
	Plane getPlane() const;
};

//***************************************************************************************************************
//  BOUNDING BOX
//***************************************************************************************************************


struct BoundingBox
{
	Vec3 min, max;

	BoundingBox();
	BoundingBox(const Vec3 &min, const Vec3 &max);
	void reset(const Vec3 &point);
	void expand(float x, float y, float z);
	void expand(const Vec3 &point);
	void expand(const BoundingBox &other);
	void scale(float factor);
	Vec3 center() const;
	Vec3 size() const;
	bool contains(const Vec3 &point) const;
	bool intersects(const BoundingBox &other) const;
	
	Vec3 getCorner(unsigned char index) const;
	void getCorners(Vec3 *dst) const;

	void getEdges(Vec3 *dst) const;
	void transform(const Mat4 &matrix);
	void transform(BoundingBox &box,  const Mat4 &matrix);
};

//***************************************************************************************************************
//  RAY
//***************************************************************************************************************

struct Ray
{
	Vec3 origin;
	Vec3 direction;

	Ray();
	Ray(const Vec3 &origin, const Vec3 &direction);

	Vec3 pointAt(float t) const;
	bool intersectTriangle(const Triangle &tri, float &t) const;
	bool intersectPlane(const Plane &plane, float &t) const;
	bool intersectBox(const BoundingBox &box, float &t) const;
};

//***************************************************************************************************************
//  FRUSTUM
//***************************************************************************************************************

struct Frustum
{
	Plane planes[6];

	Frustum();
	
	void update(const Mat4 &viewProj);

	bool containsPoint(const Vec3 &point) const;
	bool intersectsBox(const Vec3 &min, const Vec3 &max) const;
	bool intersectsBox(const BoundingBox &box) const;
	
};