#include "Math.hpp"

// -------------------------------------------------------------------------------------------------
//                                      PLANE
// -------------------------------------------------------------------------------------------------
void Plane::intersection(const Plane& p1, const Plane& p2, const Plane& p3, Vec3* point)
{
    

    // The planes' normals must be all normalized (which we guarantee in the Plane class).
    // Calculate the determinant of the matrix (i.e | n1 n2 n3 |).
    float det = p1.m_normal.x * (p2.m_normal.y * p3.m_normal.z -
                p2.m_normal.z * p3.m_normal.y) - p2.m_normal.x *(p1.m_normal.y * p3.m_normal.z -
                p1.m_normal.z * p3.m_normal.y) + p3.m_normal.x * (p1.m_normal.y * p2.m_normal.z - p1.m_normal.z * p2.m_normal.y);

    // If the determinant is zero, then the planes do not all intersect.
    if (fabs(det) <= Epsilon)
        return;

    // Create 3 points, one on each plane.
    // (We just pick the point on the plane directly along its normal from the origin).
    float p1x = -p1.m_normal.x * p1.m_distance;
    float p1y = -p1.m_normal.y * p1.m_distance;
    float p1z = -p1.m_normal.z * p1.m_distance;
    float p2x = -p2.m_normal.x * p2.m_distance;
    float p2y = -p2.m_normal.y * p2.m_distance;
    float p2z = -p2.m_normal.z * p2.m_distance;
    float p3x = -p3.m_normal.x * p3.m_distance;
    float p3y = -p3.m_normal.y * p3.m_distance;
    float p3z = -p3.m_normal.z * p3.m_distance;

    // Calculate the cross products of the normals.
    float c1x = (p2.m_normal.y * p3.m_normal.z) - (p2.m_normal.z * p3.m_normal.y);
    float c1y = (p2.m_normal.z * p3.m_normal.x) - (p2.m_normal.x * p3.m_normal.z);
    float c1z = (p2.m_normal.x * p3.m_normal.y) - (p2.m_normal.y * p3.m_normal.x);
    float c2x = (p3.m_normal.y * p1.m_normal.z) - (p3.m_normal.z * p1.m_normal.y);
    float c2y = (p3.m_normal.z * p1.m_normal.x) - (p3.m_normal.x * p1.m_normal.z);
    float c2z = (p3.m_normal.x * p1.m_normal.y) - (p3.m_normal.y * p1.m_normal.x);
    float c3x = (p1.m_normal.y * p2.m_normal.z) - (p1.m_normal.z * p2.m_normal.y);
    float c3y = (p1.m_normal.z * p2.m_normal.x) - (p1.m_normal.x * p2.m_normal.z);
    float c3z = (p1.m_normal.x * p2.m_normal.y) - (p1.m_normal.y * p2.m_normal.x);

    // Calculate the point of intersection using the formula:
    // x = (| n1 n2 n3 |)^-1 * [(x1 * n1)(n2 x n3) + (x2 * n2)(n3 x n1) + (x3 * n3)(n1 x n2)]
    float s1 = p1x * p1.m_normal.x + p1y * p1.m_normal.y + p1z * p1.m_normal.z;
    float s2 = p2x * p2.m_normal.x + p2y * p2.m_normal.y + p2z * p2.m_normal.z;
    float s3 = p3x * p3.m_normal.x + p3y * p3.m_normal.y + p3z * p3.m_normal.z;
    float detI = 1.0f / det;
    point->x = (s1 * c1x + s2 * c2x + s3 * c3x) * detI;
    point->y = (s1 * c1y + s2 * c2y + s3 * c3y) * detI;
    point->z = (s1 * c1z + s2 * c2z + s3 * c3z) * detI;
}

float Plane::intersects(const Frustum& frustum) const
{
    // Get the corners of the frustum.
    Vec3 corners[8];
    frustum.getCorners(corners);

    // Calculate the distances from all of the corners to the plane.
    // If all of the distances are positive, then the frustum is in the
    // positive half-space of this plane; if all the distances are negative,
    // then the frustum is in the negative half-space of this plane; if some of
    // the distances are positive and some are negative, the frustum intersects.
    float d = distance(corners[0]);
    if (d > 0.0f)
    {
        if (distance(corners[1]) <= 0.0f ||
            distance(corners[2]) <= 0.0f ||
            distance(corners[3]) <= 0.0f ||
            distance(corners[4]) <= 0.0f ||
            distance(corners[5]) <= 0.0f ||
            distance(corners[6]) <= 0.0f ||
            distance(corners[7]) <= 0.0f)
        {
            return Plane::INTERSECTS_INTERSECTING;
        }

        return Plane::INTERSECTS_FRONT;
    }
    else if (d < 0.0f)
    {
        if (distance(corners[1]) >= 0.0f ||
            distance(corners[2]) >= 0.0f ||
            distance(corners[3]) >= 0.0f ||
            distance(corners[4]) >= 0.0f ||
            distance(corners[5]) >= 0.0f ||
            distance(corners[6]) >= 0.0f ||
            distance(corners[7]) >= 0.0f)
        {
            return Plane::INTERSECTS_INTERSECTING;
        }

        return Plane::INTERSECTS_BACK;
    }
    else
    {
        return Plane::INTERSECTS_INTERSECTING;
    }
}

float Plane::intersects(const Plane& plane) const
{
    // Check if the planes intersect.
    if ((m_normal.x == plane.m_normal.x && m_normal.y == plane.m_normal.y && m_normal.z == plane.m_normal.z) || !isParallel(plane))
    {
        return Plane::INTERSECTS_INTERSECTING;
    }

    // Calculate the point where the given plane's normal vector intersects the given plane.
    Vec3 point(plane.m_normal.x * -plane.m_distance, plane.m_normal.y * -plane.m_distance, plane.m_normal.z * -plane.m_distance);

    // Calculate whether the given plane is in the positive or negative half-space of this plane
    // (corresponds directly to the sign of the distance from the point calculated above to this plane).
    if (distance(point) > 0.0f)
    {
        return Plane::INTERSECTS_FRONT;
    }
    else
    {
        return Plane::INTERSECTS_BACK;
    }
}

float Plane::intersects(const Ray& ray) const
{
    // Calculate the distance from the ray's origin to the plane.
    float d = distance(ray.getOrigin());

    // If the origin of the ray lies in the plane, then it intersects.
    if (d == 0.0f)
    {
        return Plane::INTERSECTS_INTERSECTING;
    }
    else
    {
        Vec3 rayDirection = ray.getDirection();
        // If the dot product of this plane's normal and the ray's direction is positive, and
        // if the distance from this plane to the ray's origin is negative -> intersection, OR
        // if the dot product of this plane's normal and the ray's direction is negative, and
        // if the distance from this plane to the ray's origin is positive -> intersection.
        if (m_normal.x * rayDirection.x + m_normal.y * rayDirection.y + m_normal.z * rayDirection.z > 0.0f)
        {
            if (d < 0.0f)
            {
                return Plane::INTERSECTS_INTERSECTING;
            }
            else
            {
                return Plane::INTERSECTS_FRONT;
            }
        }
        else
        {
            if (d > 0.0f)
            {
                return Plane::INTERSECTS_INTERSECTING;
            }
            else
            {
                return Plane::INTERSECTS_BACK;
            }
        }
    }
}

bool Plane::isParallel(const Plane& plane) const
{
    return (m_normal.y * plane.m_normal.z) - (m_normal.z * plane.m_normal.y) == 0.0f &&
           (m_normal.z * plane.m_normal.x) - (m_normal.x * plane.m_normal.z) == 0.0f &&
           (m_normal.x * plane.m_normal.y) - (m_normal.y * plane.m_normal.x) == 0.0f;
}

void Plane::set(const Vec3& normal, float distance)
{
    m_normal = normal;
    m_distance = distance;
    normalize();
}

void Plane::set(const Plane& plane)
{
    m_normal = plane.m_normal;
    m_distance = plane.m_distance;
}

void Plane::transform(const Mat4& matrix)
{
        Mat4 inverted = matrix.inverted();

        // Treat the plane as a four-tuple and multiply by the inverse transpose of the matrix to get the transformed plane.
        // Then we normalize the plane by dividing both the normal and the distance by the length of the normal.
        float nx = m_normal.x * inverted.x[0] + m_normal.y * inverted.x[1] + m_normal.z * inverted.x[2] + m_distance * inverted.x[3];
        float ny = m_normal.x * inverted.x[4] + m_normal.y * inverted.x[5] + m_normal.z * inverted.x[6] + m_distance * inverted.x[7];
        float nz = m_normal.x * inverted.x[8] + m_normal.y * inverted.x[9] + m_normal.z * inverted.x[10] + m_distance * inverted.x[11];
        float d = m_normal.x *  inverted.x[12]+ m_normal.y * inverted.x[13] + m_normal.z * inverted.x[14] + m_distance * inverted.x[15];
        float divisor = sqrt(nx * nx + ny * ny + nz * nz);
        SDL_assert(divisor);
        float factor = 1.0f / divisor;

        m_normal.x = nx * factor;
        m_normal.y = ny * factor;
        m_normal.z = nz * factor;
        m_distance = d * factor;
    
}
const Plane operator*(const Mat4& matrix, const Plane& plane)
{
    Plane p(plane);
    p.transform(matrix);
    return p;
}


void Plane::normalize()
{
    if (m_normal.isZero())
        return;

    // Normalize the plane's m_normal.
    float normalizeFactor = 1.0f / sqrt(m_normal.x * m_normal.x + m_normal.y * m_normal.y + m_normal.z * m_normal.z);

    if (normalizeFactor != 1.0f)
    {
        m_normal.x *= normalizeFactor;
        m_normal.y *= normalizeFactor;
        m_normal.z *= normalizeFactor;
        m_distance *= normalizeFactor;
    }
}

// -------------------------------------------------------------------------------------------------
//                                      Frustum
// -------------------------------------------------------------------------------------------------
Frustum::Frustum()
{
    set(Mat4::Identity());
}

Frustum::Frustum(const Mat4& matrix)
{
    set(matrix);
}

Frustum::Frustum(const Frustum& frustum)
{
    set(frustum);
}

Frustum::~Frustum()
{
}

const Plane& Frustum::getNear() const
{
    return m_near;
}

const Plane& Frustum::getFar() const
{
    return m_far;
}

const Plane& Frustum::getLeft() const
{
    return m_left;
}

const Plane& Frustum::getRight() const
{
    return m_right;
}

const Plane& Frustum::getBottom() const
{
    return m_bottom;
}

const Plane& Frustum::getTop() const
{
    return m_top;
}

void Frustum::getMat4(Mat4* dst) const
{
    dst->set(m_matrix);
}

void Frustum::getCorners(Vec3* corners) const
{
    getNearCorners(corners);
    getFarCorners(corners + 4);
}

void Frustum::getNearCorners(Vec3* corners) const
{
    SDL_assert(corners);

    Plane::intersection(m_near, m_left, m_top, &corners[0]);
    Plane::intersection(m_near, m_left, m_bottom, &corners[1]);
    Plane::intersection(m_near, m_right, m_bottom, &corners[2]);
    Plane::intersection(m_near, m_right, m_top, &corners[3]);
}

void Frustum::getFarCorners(Vec3* corners) const
{
    SDL_assert(corners);

    Plane::intersection(m_far, m_right, m_top, &corners[0]);
    Plane::intersection(m_far, m_right, m_bottom, &corners[1]);
    Plane::intersection(m_far, m_left, m_bottom, &corners[2]);
    Plane::intersection(m_far, m_left, m_top, &corners[3]);
}

bool Frustum::intersects(const Vec3& point) const
{
    if (m_near.distance(point) <= 0)
        return false;
    if (m_far.distance(point) <= 0)
        return false;
    if (m_left.distance(point) <= 0)
        return false;
    if (m_right.distance(point) <= 0)
        return false;
    if (m_top.distance(point) <= 0)
        return false;
    if (m_bottom.distance(point) <= 0)
        return false;

    return true;
}

bool Frustum::intersects(float x, float y, float z) const
{
    return intersects(Vec3(x, y, z));
}

bool Frustum::intersects(const BoundingSphere& sphere) const
{
    return sphere.intersects(*this);
}

bool Frustum::intersects(const BoundingBox& box) const
{
    return box.intersects(*this);
}

float Frustum::intersects(const Plane& plane) const
{
    return plane.intersects(*this);
}

float Frustum::intersects(const Ray& ray) const
{
    return ray.intersects(*this);
}

void Frustum::set(const Frustum& frustum)
{
    m_near = frustum.m_near;
    m_far = frustum.m_far;
    m_bottom = frustum.m_bottom;
    m_top = frustum.m_top;
    m_left = frustum.m_left;
    m_right = frustum.m_right;
    m_matrix.set(frustum.m_matrix);
}

void Frustum::updatePlanes()
{
    m_near.set(Vec3(m_matrix.x[3] + m_matrix.x[2], m_matrix.x[7] + m_matrix.x[6], m_matrix.x[11] + m_matrix.x[10]), m_matrix.x[15] + m_matrix.x[14]);
    m_far.set(Vec3(m_matrix.x[3] - m_matrix.x[2], m_matrix.x[7] - m_matrix.x[6], m_matrix.x[11] - m_matrix.x[10]), m_matrix.x[15] - m_matrix.x[14]);
    m_bottom.set(Vec3(m_matrix.x[3] + m_matrix.x[1], m_matrix.x[7] + m_matrix.x[5], m_matrix.x[11] + m_matrix.x[9]), m_matrix.x[15] + m_matrix.x[13]);
    m_top.set(Vec3(m_matrix.x[3] - m_matrix.x[1], m_matrix.x[7] - m_matrix.x[5], m_matrix.x[11] - m_matrix.x[9]), m_matrix.x[15] - m_matrix.x[13]);
    m_left.set(Vec3(m_matrix.x[3] + m_matrix.x[0], m_matrix.x[7] + m_matrix.x[4], m_matrix.x[11] + m_matrix.x[8]), m_matrix.x[15] + m_matrix.x[12]);
    m_right.set(Vec3(m_matrix.x[3] - m_matrix.x[0], m_matrix.x[7] - m_matrix.x[4], m_matrix.x[11] - m_matrix.x[8]), m_matrix.x[15] - m_matrix.x[12]);
}

void Frustum::set(const Mat4& matrix)
{
    m_matrix.set(matrix);
    updatePlanes();
}
// -------------------------------------------------------------------------------------------------
//                                      RAY
// -------------------------------------------------------------------------------------------------
void Ray::normalize()
{
    if (m_direction.isZero())
    {
         return;
    }

    // Normalize the ray's direction vector.
    float normalizeFactor = 1.0f / sqrt(m_direction.x * m_direction.x + m_direction.y * m_direction.y + m_direction.z * m_direction.z);
    if (normalizeFactor != 1.0f)
    {
        m_direction.x *= normalizeFactor;
        m_direction.y *= normalizeFactor;
        m_direction.z *= normalizeFactor;
    }
}
float Ray::intersects(const BoundingSphere& sphere) const
{
    return sphere.intersects(*this);
}

float Ray::intersects(const BoundingBox& box) const
{
    return box.intersects(*this);
}

float Ray::intersects(const Frustum& frustum) const
{
    Plane n = frustum.getNear();
    float nD = intersects(n);
    float nOD = n.distance(m_origin);

    Plane f = frustum.getFar();
    float fD = intersects(f);
    float fOD = f.distance(m_origin);

    Plane l = frustum.getLeft();
    float lD = intersects(l);
    float lOD = l.distance(m_origin);

    Plane r = frustum.getRight();
    float rD = intersects(r);
    float rOD = r.distance(m_origin);

    Plane b = frustum.getBottom();
    float bD = intersects(b);
    float bOD = b.distance(m_origin);

    Plane t = frustum.getTop();
    float tD = intersects(t);
    float tOD = t.distance(m_origin);

    // If the ray's origin is in the negative half-space of one of the frustum's planes
    // and it does not intersect that same plane, then it does not intersect the frustum.
    if ((nOD < 0.0f && nD < 0.0f) || (fOD < 0.0f && fD < 0.0f) ||
        (lOD < 0.0f && lD < 0.0f)  || (rOD < 0.0f && rD < 0.0f) ||
        (bOD < 0.0f && bD < 0.0f)  || (tOD < 0.0f && tD < 0.0f))
    {
        return INTERSECTS_NONE;
    }

    // Otherwise, the intersection distance is the minimum positive intersection distance.
    float d = (nD > 0.0f) ? nD : 0.0f;
    d = (fD > 0.0f) ? ((d == 0.0f) ? fD : Min(fD, d)) : d;
    d = (lD > 0.0f) ? ((d == 0.0f) ? lD : Min(lD, d)) : d;
    d = (rD > 0.0f) ? ((d == 0.0f) ? rD : Min(rD, d)) : d;
    d = (tD > 0.0f) ? ((d == 0.0f) ? bD : Min(bD, d)) : d;
    d = (bD > 0.0f) ? ((d == 0.0f) ? tD : Min(tD, d)) : d;

    return d;
}

float  Ray::intersects(const Plane& plane) const
{
    const Vec3& normal = plane.getNormal();
    // If the origin of the ray is on the plane then the distance is zero.
    float alpha = (normal.dot(m_origin) + plane.getDistance());
    if (fabs(alpha) < Epsilon)
    {
        return 0.0f;
    }

    float dot = normal.dot(m_direction);
    
    // If the dot product of the plane's normal and this ray's direction is zero,
    // then the ray is parallel to the plane and does not intersect it.
    if (dot == 0.0f)
    {
        return INTERSECTS_NONE;
    }
    
    // Calculate the distance along the ray's direction vector to the point where
    // the ray intersects the plane (if it is negative the plane is behind the ray).
    float d = -alpha / dot;
    if (d < 0.0f)
    {
        return INTERSECTS_NONE;
    }
    return d;
}


const Ray operator*(const Mat4& matrix, const Ray& ray)
{
    Ray r(ray);
    r.transform(matrix);
    return r;
}
// -------------------------------------------------------------------------------------------------
//                                      BoundingSphere
// -------------------------------------------------------------------------------------------------

BoundingSphere::BoundingSphere()
    : radius(0)
{
}

BoundingSphere::BoundingSphere(const Vec3& center, float radius)
{
    set(center, radius);
}

BoundingSphere::BoundingSphere(const BoundingSphere& copy)
{
    set(copy);
}

BoundingSphere::~BoundingSphere()
{
}

const BoundingSphere& BoundingSphere::empty()
{
    static BoundingSphere s;
    return s;
}

bool BoundingSphere::intersects(const BoundingSphere& sphere) const
{
    // If the distance between the spheres' centers is less than or equal
    // to the sum of their radii, then the spheres intersect.
    float vx = sphere.center.x - center.x;
    float vy = sphere.center.y - center.y;
    float vz = sphere.center.z - center.z;

    return sqrt(vx * vx + vy * vy + vz * vz) <= (radius + sphere.radius);
}

bool BoundingSphere::intersects(const BoundingBox& box) const
{
    // Determine what point is closest; if the distance to that
    // point is less than the radius, then this sphere intersects.
    float cpX = center.x;
    float cpY = center.y;
    float cpZ = center.z;

    const Vec3& boxMin = box.min;
    const Vec3& boxMax = box.max;
    // Closest x value.
    if (center.x < boxMin.x)
    {
        cpX = boxMin.x;
    }
    else if (center.x > boxMax.x)
    {
        cpX = boxMax.x;
    }

    // Closest y value.
    if (center.y < boxMin.y)
    {
        cpY = boxMin.y;
    }
    else if (center.y > boxMax.y)
    {
        cpY = boxMax.y;
    }

    // Closest z value.
    if (center.z < boxMin.z)
    {
        cpZ = boxMin.z;
    }
    else if (center.z > boxMax.z)
    {
        cpZ = boxMax.z;
    }

    // Find the distance to the closest point and see if it is less than or equal to the radius.
    cpX -= center.x;
    cpY -= center.y;
    cpZ -= center.z;

    return sqrt(cpX * cpX + cpY * cpY + cpZ * cpZ) <= radius;
}

bool BoundingSphere::intersects(const Frustum& frustum) const
{
    // The sphere must either intersect or be in the positive half-space of all six planes of the frustum.
    return (intersects(frustum.getNear()) != Plane::INTERSECTS_BACK &&
            intersects(frustum.getFar()) != Plane::INTERSECTS_BACK &&
            intersects(frustum.getLeft()) != Plane::INTERSECTS_BACK &&
            intersects(frustum.getRight()) != Plane::INTERSECTS_BACK &&
            intersects(frustum.getBottom()) != Plane::INTERSECTS_BACK &&
            intersects(frustum.getTop()) != Plane::INTERSECTS_BACK );
}

float BoundingSphere::intersects(const Plane& plane) const
{
    float distance = plane.distance(center);

    if (fabsf(distance) <= radius)
    {
        return Plane::INTERSECTS_INTERSECTING;
    }
    else if (distance > 0.0f)
    {
        return Plane::INTERSECTS_FRONT;
    }
    else
    {
        return Plane::INTERSECTS_BACK;
    }
}

float BoundingSphere::intersects(const Ray& ray) const
{
    const Vec3& origin = ray.getOrigin();
    const Vec3& direction = ray.getDirection();

    // Calculate the vector and the square of the distance from the ray's origin to this sphere's center.
    float vx = origin.x - center.x;
    float vy = origin.y - center.y;
    float vz = origin.z - center.z;
    float d2 = vx * vx + vy * vy + vz * vz;

    // Solve the quadratic equation using the ray's and sphere's equations together.
    // Since the ray's direction is guaranteed to be 1 by the Ray, we don't need to
    // calculate and use A (A=ray.getDirection().lengthSquared()).
    float B = 2.0f * (vx * direction.x + vy * direction.y + vz * direction.z);
    float C = d2 - radius * radius;
    float discriminant = B * B - 4.0f * C;

    // If the discriminant is negative, then there is no intersection.
    if (discriminant < 0.0f)
    {
        return Ray::INTERSECTS_NONE;
    }
    else
    {
        // The intersection is at the smaller positive root.
        float sqrtDisc = sqrt(discriminant);
        float t0 = (-B - sqrtDisc) * 0.5f;
        float t1 = (-B + sqrtDisc) * 0.5f;
        return (t0 > 0.0f && t0 < t1) ? t0 : t1;
    }
}

bool BoundingSphere::isEmpty() const
{
    return radius == 0.0f;
}

void BoundingSphere::merge(const BoundingSphere& sphere)
{
    if (sphere.isEmpty())
        return;

    // Calculate the distance between the two centers.
    float vx = center.x - sphere.center.x;
    float vy = center.y - sphere.center.y;
    float vz = center.z - sphere.center.z;
    float d = sqrt(vx * vx + vy * vy + vz * vz);

    // If one sphere is contained inside the other, set to the larger sphere.
    if (d <= (sphere.radius - radius))
    {
        center = sphere.center;
        radius = sphere.radius;
        return;
    }
    else if (d <= (radius - sphere.radius))
    {
        return;
    }

    // Calculate the unit vector between the two centers.
    SDL_assert(d != 0.0f);
    float dI = 1.0f / d;
    vx *= dI;
    vy *= dI;
    vz *= dI;

    // Calculate the new radius.
    float r = (radius + sphere.radius + d) * 0.5f;

    // Calculate the new center.
    float scaleFactor = (r - sphere.radius);
    vx = vx * scaleFactor + sphere.center.x;
    vy = vy * scaleFactor + sphere.center.y;
    vz = vz * scaleFactor + sphere.center.z;

    // Set the new center and radius.
    center.x = vx;
    center.y = vy;
    center.z = vz;
    radius = r;
}

void BoundingSphere::merge(const BoundingBox& box)
{
    if (box.isEmpty())
        return;

    const Vec3& min = box.min;
    const Vec3& max = box.max;

    // Find the corner of the bounding box that is farthest away from this sphere's center.
    float v1x = min.x - center.x;
    float v1y = min.y - center.y;
    float v1z = min.z - center.z;
    float v2x = max.x - center.x;
    float v2y = max.y - center.y;
    float v2z = max.z - center.z;
    float fx = min.x;
    float fy = min.y;
    float fz = min.z;

    if (v2x > v1x)
    {
        fx = max.x;
    }
    if (v2y > v1y)
    {
        fy = max.y;
    }
    if (v2z > v1z)
    {
        fz = max.z;
    }

    // Calculate the unit vector and the distance between the center and the farthest point.
    v1x = center.x - fx;
    v1y = center.y - fy;
    v1z = center.z - fz;
    float distance = sqrt(v1x * v1x + v1y * v1y + v1z * v1z);

    // If the box is inside the sphere, we are done.
    if (distance <= radius)
    {
        return;
    }

    // Calculate the unit vector between the center and the farthest point.
    SDL_assert(distance != 0.0f);
    float dI = 1.0f / distance;
    v1x *= dI;
    v1y *= dI;
    v1z *= dI;

    // Calculate the new radius.
    float r = (radius + distance) * 0.5f;

    // Calculate the new center.
    v1x = v1x * r + fx;
    v1y = v1y * r + fy;
    v1z = v1z * r + fz;

    // Set the new center and radius.
    center.x = v1x;
    center.y = v1y;
    center.z = v1z;
    radius = r;
}

void BoundingSphere::set(const Vec3& center, float radius)
{
    this->center = center;
    this->radius = radius;
}

void BoundingSphere::set(const BoundingSphere& sphere)
{
    center = sphere.center;
    radius = sphere.radius;
}

void BoundingSphere::set(const BoundingBox& box)
{
    center.x = (box.min.x + box.max.x) * 0.5f;
    center.y = (box.min.y + box.max.y) * 0.5f;
    center.z = (box.min.z + box.max.z) * 0.5f;
    radius = center.distance(box.max);
}

void BoundingSphere::transform(const Mat4& matrix)
{
    // Translate the center point.
    matrix.transformPoint(&center);

    // Scale the sphere's radius by the scale fo the matrix
    Vec3 scale;
    matrix.decompose(&scale, NULL, NULL);
    float r = radius * scale.x;
    r = Max(r, radius * scale.y);
    r = Max(r, radius * scale.z);
    radius = r;
}

float BoundingSphere::distance(const BoundingSphere& sphere, const Vec3& point)
{
    return sqrt((point.x - sphere.center.x) * (point.x - sphere.center.x) +
                 (point.y - sphere.center.y) * (point.y - sphere.center.x) +
                 (point.z - sphere.center.z) * (point.z - sphere.center.x));
}

bool BoundingSphere::contains(const BoundingSphere& sphere, Vec3* points, unsigned int count)
{
    for (unsigned int i = 0; i < count; i++)
    {
        if (distance(sphere, points[i]) > sphere.radius)
        {
            return false;
        }
    }
    return true;
}

const BoundingSphere operator*(const Mat4& matrix, const BoundingSphere& sphere)
{
    BoundingSphere s(sphere);
    s.transform(matrix);
    return s;
}
// -------------------------------------------------------------------------------------------------
//                                      BoundingBox
// -------------------------------------------------------------------------------------------------

BoundingBox::BoundingBox()
{
}

BoundingBox::BoundingBox(const Vec3& min, const Vec3& max)
{
    set(min, max);
}

BoundingBox::BoundingBox(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{
    set(minX, minY, minZ, maxX, maxY, maxZ);
}

BoundingBox::BoundingBox(const BoundingBox& copy)
{
    set(copy);
}

BoundingBox::~BoundingBox()
{
}

const BoundingBox& BoundingBox::empty()
{
    static BoundingBox b;
    return b;
}

void BoundingBox::getEdges(Vec3* edges) const
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
void BoundingBox::getCorners(Vec3* dst) const
{
    SDL_assert(dst);

    // Near face, specified counter-clockwise looking towards the origin from the positive z-axis.
    // Left-top-front.
    dst[0].set(min.x, max.y, max.z);
    // Left-bottom-front.
    dst[1].set(min.x, min.y, max.z);
    // Right-bottom-front.
    dst[2].set(max.x, min.y, max.z);
    // Right-top-front.
    dst[3].set(max.x, max.y, max.z);

    // Far face, specified counter-clockwise looking towards the origin from the negative z-axis.
    // Right-top-back.
    dst[4].set(max.x, max.y, min.z);
    // Right-bottom-back.
    dst[5].set(max.x, min.y, min.z);
    // Left-bottom-back.
    dst[6].set(min.x, min.y, min.z);
    // Left-top-back.
    dst[7].set(min.x, max.y, min.z);
}
Vec3 BoundingBox::getCorner( u32 index ) const
{
    switch( index )
		{
		case 0:
			return Vec3( min.x, min.y, max.z );
		case 1:
			return Vec3( max.x, min.y, max.z );
		case 2:
			return Vec3( max.x, max.y, max.z );
		case 3:
			return Vec3( min.x, max.y, max.z );
		case 4:
			return Vec3( min.x, min.y, min.z );
		case 5:
			return Vec3( max.x, min.y, min.z );
		case 6:
			return Vec3( max.x, max.y, min.z );
		case 7:
			return Vec3( min.x, max.y, min.z );
		default:
			return Vec3();
		}
}

Vec3 BoundingBox::getCenter() const
{
    Vec3 center;
    getCenter(&center);
    return center;
}

void BoundingBox::getCenter(Vec3* dst) const
{
    SDL_assert(dst);

    dst->set(min, max);
    dst->scale(0.5f);
    dst->add(min);
}

bool BoundingBox::intersects(const BoundingSphere& sphere) const
{
    return sphere.intersects(*this);
}

bool BoundingBox::intersects(const BoundingBox& box) const
{
    return ((min.x >= box.min.x && min.x <= box.max.x) || (box.min.x >= min.x && box.min.x <= max.x)) &&
            ((min.y >= box.min.y && min.y <= box.max.y) || (box.min.y >= min.y && box.min.y <= max.y)) &&
            ((min.z >= box.min.z && min.z <= box.max.z) || (box.min.z >= min.z && box.min.z <= max.z));
}

bool BoundingBox::intersects(const Frustum& frustum) const
{
    // The box must either intersect or be in the positive half-space of all six planes of the frustum.
    return (intersects(frustum.getNear()) != Plane::INTERSECTS_BACK &&
            intersects(frustum.getFar()) != Plane::INTERSECTS_BACK &&
            intersects(frustum.getLeft()) != Plane::INTERSECTS_BACK &&
            intersects(frustum.getRight()) != Plane::INTERSECTS_BACK &&
            intersects(frustum.getBottom()) != Plane::INTERSECTS_BACK &&
            intersects(frustum.getTop()) != Plane::INTERSECTS_BACK);
}

float BoundingBox::intersects(const Plane& plane) const
{
    // Calculate the distance from the center of the box to the plane.
    Vec3 center((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f, (min.z + max.z) * 0.5f);
    float distance = plane.distance(center);

    // Get the extents of the box from its center along each axis.
    float extentX = (max.x - min.x) * 0.5f;
    float extentY = (max.y - min.y) * 0.5f;
    float extentZ = (max.z - min.z) * 0.5f;

    const Vec3& planeNormal = plane.getNormal();
    if (fabsf(distance) <= (fabsf(extentX * planeNormal.x) + fabsf(extentY * planeNormal.y) + fabsf(
        extentZ * planeNormal.z)))
    {
        return Plane::INTERSECTS_INTERSECTING;
    }

    return (distance > 0.0f) ? (float)Plane::INTERSECTS_FRONT : (float)Plane::INTERSECTS_BACK;
}

float BoundingBox::intersects(const Ray& ray) const
{
    // Intermediate calculation variables.
    float dnear = 0.0f;
    float dfar = 0.0f;
    float tmin = 0.0f;
    float tmax = 0.0f;

    const Vec3& origin = ray.getOrigin();
    const Vec3& direction = ray.getDirection();

    // X direction.
    float div = 1.0f / direction.x;
    if (div >= 0.0f)
    {
        tmin = (min.x - origin.x) * div;
        tmax = (max.x - origin.x) * div;
    }
    else
    {
        tmin = (max.x - origin.x) * div;
        tmax = (min.x - origin.x) * div;
    }
    dnear = tmin;
    dfar = tmax;

    // Check if the ray misses the box.
    if (dnear > dfar || dfar < 0.0f)
    {
        return Ray::INTERSECTS_NONE;
    }

    // Y direction.
    div = 1.0f / direction.y;
    if (div >= 0.0f)
    {
        tmin = (min.y - origin.y) * div;
        tmax = (max.y - origin.y) * div;
    }
    else
    {
        tmin = (max.y - origin.y) * div;
        tmax = (min.y - origin.y) * div;
    }

    // Update the near and far intersection distances.
    if (tmin > dnear)
    {
        dnear = tmin;
    }
    if (tmax < dfar)
    {
        dfar = tmax;
    }
    // Check if the ray misses the box.
    if (dnear > dfar || dfar < 0.0f)
    {
        return Ray::INTERSECTS_NONE;
    }

    // Z direction.
    div = 1.0f / direction.z;
    if (div >= 0.0f)
    {
        tmin = (min.z - origin.z) * div;
        tmax = (max.z - origin.z) * div;
    }
    else
    {
        tmin = (max.z - origin.z) * div;
        tmax = (min.z - origin.z) * div;
    }

    // Update the near and far intersection distances.
    if (tmin > dnear)
    {
        dnear = tmin;
    }
    if (tmax < dfar)
    {
        dfar = tmax;
    }

    // Check if the ray misses the box.
    if (dnear > dfar || dfar < 0.0f)
    {
        return Ray::INTERSECTS_NONE;
    }
    // The ray intersects the box (and since the direction of a Ray is normalized, dnear is the distance to the ray).
    return dnear;
}

bool BoundingBox::isEmpty() const
{
    return min.x == max.x && min.y == max.y && min.z == max.z;
}

void BoundingBox::merge(const BoundingBox& box)
{
    // Calculate the new minimum point.
    min.x = Min(min.x, box.min.x);
    min.y = Min(min.y, box.min.y);
    min.z = Min(min.z, box.min.z);

    // Calculate the new maximum point.
    max.x = Max(max.x, box.max.x);
    max.y = Max(max.y, box.max.y);
    max.z = Max(max.z, box.max.z);
}

void BoundingBox::merge(const BoundingSphere& sphere)
{
    const Vec3& center = sphere.center;
    float radius = sphere.radius;

    // Calculate the new minimum point for the merged bounding box.
    min.x = Min(min.x, center.x - radius);
    min.y = Min(min.y, center.y - radius);
    min.z = Min(min.z, center.z - radius);

    // Calculate the new maximum point for the merged bounding box.
    max.x = Max(max.x, center.x + radius);
    max.y = Max(max.y, center.y + radius);
    max.z = Max(max.z, center.z + radius);
}

void BoundingBox::set(const Vec3& min, const Vec3& max)
{
    this->min = min;
    this->max = max;
}

void BoundingBox::set(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
{
    min.set(minX, minY, minZ);
    max.set(maxX, maxY, maxZ);
}

static void updateMinMax(Vec3* point, Vec3* min, Vec3* max)
{
    SDL_assert(point);
    SDL_assert(min);
    SDL_assert(max);

    // Leftmost point.
    if (point->x < min->x)
    {
        min->x = point->x;
    }

    // Rightmost point.
    if (point->x > max->x)
    {
        max->x = point->x;
    }

    // Lowest point.
    if (point->y < min->y)
    {
        min->y = point->y;
    }

    // Highest point.
    if (point->y > max->y)
    {
        max->y = point->y;
    }

    // Farthest point.
    if (point->z < min->z)
    {
        min->z = point->z;
    }

    // Nearest point.
    if (point->z > max->z)
    {
        max->z = point->z;
    }
}

void BoundingBox::set(const BoundingBox& box)
{
    min = box.min;
    max = box.max;
}

void BoundingBox::set(const BoundingSphere& sphere)
{
    const Vec3& center = sphere.center;
    float radius = sphere.radius;

    // Calculate the minimum point for the box.
    min.x = center.x - radius;
    min.y = center.y - radius;
    min.z = center.z - radius;

    // Calculate the maximum point for the box.
    max.x = center.x + radius;
    max.y = center.y + radius;
    max.z = center.z + radius;
}

void BoundingBox::transform(const Mat4& matrix)
{
    // Calculate the corners.
    Vec3 corners[8];
    getCorners(corners);

    // Transform the corners, recalculating the min and max points along the way.

    matrix.transformPoint(&corners[0]);
    Vec3 newMin = corners[0];
    Vec3 newMax = corners[0];
    for (int i = 1; i < 8; i++)
    {
        matrix.transformPoint(&corners[i]);
        updateMinMax(&corners[i], &newMin, &newMax);
    }
    this->min.x = newMin.x;
    this->min.y = newMin.y;
    this->min.z = newMin.z;
    this->max.x = newMax.x;
    this->max.y = newMax.y;
    this->max.z = newMax.z;
}
void BoundingBox::clear()
{
        min = Vec3(0, 0, 0);
		max = Vec3(0, 0, 0);
}
void BoundingBox::addPoint(const Vec3 &p)
{
    min = min.Min(p);
	max = max.Max(p);
}
const BoundingBox operator*(const Mat4 &matrix, const BoundingBox &box)
{
    BoundingBox b(box);
    b.transform(matrix);
    return b;
}

Vec4 Vec4::operator*(const Mat4 &m)
{
   		return Vec4(x * m.c[0][0] + y * m.c[1][0] + z * m.c[2][0] + w * m.c[3][0],
					x * m.c[0][1] + y * m.c[1][1] + z * m.c[2][1] + w * m.c[3][1],
					x * m.c[0][2] + y * m.c[1][2] + z * m.c[2][2] + w * m.c[3][2],
					x * m.c[0][3] + y * m.c[1][3] + z * m.c[2][3] + w * m.c[3][3]);
	
}
