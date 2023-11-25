#include "Maths.h"

using namespace DirectX;

const Vec2 Vec2::ZERO = { 0.0f, 0.0f };
const Vec2 Vec2::ONE = { 1.0f, 1.0f };

const Vec3 Vec3::ZERO = { 0.0f, 0.0f, 0.0f };
const Vec3 Vec3::ONE = { 1.0f, 1.0f, 1.0f };
const Vec3 Vec3::UP = { 0.0f, 1.0f, 0.0f };
const Vec3 Vec3::DOWN = { 0.0f, -1.0f, 0.0f };
const Vec3 Vec3::FORWARD = { 0.0f, 0.0f, 1.0f };
const Vec3 Vec3::BACKWARD = { 0.0f, 0.0f, -1.0f };
const Vec3 Vec3::RIGHT = { 1.0f, 0.0f, 0.0f };
const Vec3 Vec3::LEFT = { -1.0f, 0.0f, 0.0f };

const Vec4 Vec4::FORWARD = { Vec3::FORWARD, 0.0f };

const Quat Quat::IDENTITY = { 0.f, 0.f, 0.f, 1.f };

//////////////////////////////////////////////////////////////////////////

Vec3 operator*(const Vec3& v, const Mat4& m)
{
    const XMVECTOR vec = XMLoadFloat3(&v);
    const XMMATRIX mat = XMLoadFloat4x4(&m);
    const XMVECTOR result = XMVector3TransformCoord(vec, mat);

    Vec3 out;
    XMStoreFloat3(&out, result);
    return out;
}

Vec3 Vec3::Cross(const Vec3& v) const
{
    const XMVECTOR xm_v1 = XMLoadFloat3(this);
    const XMVECTOR xm_v2 = XMLoadFloat3(&v);
    const XMVECTOR xm_out = XMVector3Cross(xm_v1, xm_v2);

    Vec3 out;
    XMStoreFloat3(&out, xm_out);
    return out;
}

float Vec3::Dot(const Vec3& v) const
{
    const XMVECTOR v1 = XMLoadFloat3(this);
    const XMVECTOR v2 = XMLoadFloat3(&v);
    const XMVECTOR result = XMVector3Dot(v1, v2);
    return XMVectorGetX(result);
}

float Vec3::Length() const
{
    const XMVECTOR xm_v = XMLoadFloat3(this);
    const XMVECTOR xm_out = XMVector3Length(xm_v);
    return XMVectorGetX(xm_out);
}

float Vec3::LengthSquared() const
{
    const XMVECTOR xm_v = XMLoadFloat3(this);
    const XMVECTOR xm_out = XMVector3LengthSq(xm_v);
    return XMVectorGetX(xm_out);
}

Vec3 Vec3::Normalize()
{
    const XMVECTOR xm_v = XMLoadFloat3(this);
    const XMVECTOR xm_out = XMVector3Normalize(xm_v);
    XMStoreFloat3(this, xm_out);
    return *this;
}

Vec3 Vec3::Normalize(const Vec3& v)
{
    const XMVECTOR xm_v = XMLoadFloat3(&v);
    const XMVECTOR xm_out = XMVector3Normalize(xm_v);

    Vec3 out;
    XMStoreFloat3(&out, xm_out);
    return out;
}

float Vec3::Distance(const Vec3& v1, const Vec3& v2)
{
    const XMVECTOR a = XMLoadFloat3(&v1);
    const XMVECTOR b = XMLoadFloat3(&v2);
    const XMVECTOR b_minus_a = XMVectorSubtract(b, a);
    const XMVECTOR length = XMVector3Length(b_minus_a);
    return XMVectorGetX(length);
}

float Vec3::DistanceSquared(const Vec3& v1, const Vec3& v2)
{
    const XMVECTOR a = XMLoadFloat3(&v1);
    const XMVECTOR b = XMLoadFloat3(&v2);
    const XMVECTOR b_minus_a = XMVectorSubtract(b, a);
    const XMVECTOR length_squared = XMVector3LengthSq(b_minus_a);
    return XMVectorGetX(length_squared);
}

Vec3 Vec3::Cross(const Vec3& v1, const Vec3& v2)
{
    const XMVECTOR xm_v1 = XMLoadFloat3(&v1);
    const XMVECTOR xm_v2 = XMLoadFloat3(&v2);
    const XMVECTOR xm_out = XMVector3Cross(xm_v1, xm_v2);

    Vec3 out;
    XMStoreFloat3(&out, xm_out);
    return out;
}

float Vec3::Dot(const Vec3& v1, const Vec3& v2)
{
    const XMVECTOR xm_v1 = XMLoadFloat3(&v1);
    const XMVECTOR xm_v2 = XMLoadFloat3(&v2);
    const XMVECTOR result = XMVector3Dot(xm_v1, xm_v2);
    return XMVectorGetX(result);
}

Vec3 Vec3::Transform(const Vec3& v, const Quat& q)
{
    const XMVECTOR xm_v = XMLoadFloat3(&v);
    const XMVECTOR xm_q = XMLoadFloat4(&q);
    const XMVECTOR xm_out = XMVector3Rotate(xm_v, xm_q);

    Vec3 out;
    XMStoreFloat3(&out, xm_out);
    return out;
}

//////////////////////////////////////////////////////////////////////////

Vec4 operator*(const Vec4& v, const Mat4& m)
{
    const XMVECTOR vec = XMLoadFloat4(&v);
    const XMMATRIX mat = XMLoadFloat4x4(&m);
    const XMVECTOR result = XMVector4Transform(vec, mat);

    Vec4 out;
    XMStoreFloat4(&out, result);
    return out;
}

Vec4 operator+(const Vec4& v1, const Vec4& v2)
{
    const XMVECTOR xm_v1 = XMLoadFloat4(&v1);
    const XMVECTOR xm_v2 = XMLoadFloat4(&v2);
    const XMVECTOR xm_out = XMVectorAdd(xm_v1, xm_v2);

    Vec4 out;
    XMStoreFloat4(&out, xm_out);
    return out;
}

Vec4 operator-(const Vec4& v1, const Vec4& v2)
{
    const XMVECTOR xm_v1 = XMLoadFloat4(&v1);
    const XMVECTOR xm_v2 = XMLoadFloat4(&v2);
    const XMVECTOR xm_out = XMVectorSubtract(xm_v1, xm_v2);

    Vec4 out;
    XMStoreFloat4(&out, xm_out);
    return out;
}

Vec4 Vec4::Transform(const Vec4& v, const Quat& q)
{
    const XMVECTOR xm_v = XMLoadFloat4(&v);
    const XMVECTOR xm_q = XMLoadFloat4(&q);
    XMVECTOR xm_out = XMVector3Rotate(xm_v, xm_q);
    xm_out = XMVectorSelect(v, xm_out, g_XMSelect1110); // result.w = v.w

    Vec4 out;
    XMStoreFloat4(&out, xm_out);
    return out;
}

Vec4 Vec4::Normalize()
{
    const XMVECTOR xm_v = XMLoadFloat4(this);
    const XMVECTOR xm_out = XMVector4Normalize(xm_v);
    XMStoreFloat4(this, xm_out);
    return *this;
}

float Vec4::Length() const
{
    const XMVECTOR xm_v = XMLoadFloat4(this);
    const XMVECTOR xm_out = XMVector4Length(xm_v);
    return XMVectorGetX(xm_out);
}

float Vec4::LengthSquared() const
{
    const XMVECTOR xm_v = XMLoadFloat4(this);
    const XMVECTOR xm_out = XMVector4LengthSq(xm_v);
    return XMVectorGetX(xm_out);
}

//////////////////////////////////////////////////////////////////////////

Mat4& Mat4::operator*=(const Mat4& other)
{
    const XMMATRIX m1 = XMLoadFloat4x4(this);
    const XMMATRIX m2 = XMLoadFloat4x4(&other);
    const XMMATRIX multiplied = XMMatrixMultiply(m1, m2);
    XMStoreFloat4x4(this, multiplied);
    return *this;
}

void Mat4::Decompose(Vec3& out_scaling, Quat& out_rotation, Vec3& out_translation) const
{
    XMVECTOR scaling;
    XMVECTOR rotation;
    XMVECTOR translation;
    bool success = XMMatrixDecompose(&scaling, &rotation, &translation, *this);
    CHECK(success);

    XMStoreFloat3(&out_scaling, scaling);
    XMStoreFloat4(&out_rotation, rotation);
    XMStoreFloat3(&out_translation, translation);
}

Mat4 Mat4::Transpose() const
{
    XMMATRIX mat = *this;
    return Mat4(DirectX::XMMatrixTranspose(mat));
}

Mat4 Mat4::Invert() const
{
    XMMATRIX mat = *this;
    XMVECTOR det;
    return XMMatrixInverse(&det, mat);
}

Mat4 Mat4::Translation(const Vec3& v)
{
    return XMMatrixTranslation(v.x, v.y, v.z);
}

Mat4 Mat4::Translation(float x, float y, float z)
{
    return XMMatrixTranslation(x, y, z);
}

Mat4 Mat4::RotationX(float rad)
{
    return XMMatrixRotationX(rad);
}

Mat4 Mat4::RotationY(float rad)
{
    return XMMatrixRotationY(rad);
}

Mat4 Mat4::RotationZ(float rad)
{
    return XMMatrixRotationZ(rad);
}

Mat4 Mat4::Scaling(float s)
{
    return XMMatrixScaling(s, s, s);
}

Mat4 Mat4::Scaling(const Vec3& v)
{
    return XMMatrixScaling(v.x, v.y, v.z);
}

Mat4 Mat4::Scaling(float x, float y, float z)
{
    return XMMatrixScaling(x, y, z);
}

Mat4 Mat4::SRT(const Vec3& scaling, const Quat& rotation, const Vec3& translation)
{
    return Mat4::Scaling(scaling) * rotation.ToMatrix() * Mat4::Translation(translation);
}

Mat4 Mat4::LookAt(const Vec3& origin, const Vec3& target, const Vec3& up)
{
    return XMMatrixLookAtLH(origin, target, up);
}

Mat4 Mat4::PerspectiveFovLH(float fov_y_rad, float aspect_ratio, float near_z, float far_z)
{
    return XMMatrixPerspectiveFovLH(fov_y_rad, aspect_ratio, near_z, far_z);
}

Mat4 Mat4::OrthographicLH(float view_width, float view_height, float near_z, float far_z)
{
    return XMMatrixOrthographicLH(view_height, view_height, near_z, far_z);
}

Mat4 Mat4::OrthographicLH(float view_left, float view_right, float view_bottom, float view_top, float near_z, float far_z)
{
    return XMMatrixOrthographicOffCenterLH(view_left, view_right, view_bottom, view_top, near_z, far_z);
}

const Mat4 Mat4::IDENTITY = { 1.0f, 0.0f, 0.0f, 0.0f,
                            0.0f, 1.0f, 0.0f, 0.0f,
                            0.0f, 0.0f, 1.0f, 0.0f,
                            0.0f, 0.0f, 0.0f, 1.0f };

Mat4 operator*(const Mat4& mat_a, const Mat4& mat_b)
{
    return XMMatrixMultiply(mat_a, mat_b);
}

//////////////////////////////////////////////////////////////////////////

void Quat::Normalize()
{
    const XMVECTOR q = XMLoadFloat4(this);
    XMStoreFloat4(this, XMQuaternionNormalize(q));
}

Quat Quat::Normalize(const Quat& q)
{
    const XMVECTOR xm_q = XMLoadFloat4(&q);

    Quat out;
    XMStoreFloat4(&out, XMQuaternionNormalize(xm_q));
    return out;
}

Quat Quat::Inverse()
{
    const XMVECTOR q = XMLoadFloat4(this);
    Quat out;
    XMStoreFloat4(&out, XMQuaternionInverse(q));
    return out;
}

Vec3 Quat::ToEuler() const
{
    const float xx = x * x;
    const float yy = y * y;
    const float zz = z * z;

    const float m31 = 2.f * x * z + 2.f * y * w;
    const float m32 = 2.f * y * z - 2.f * x * w;
    const float m33 = 1.f - 2.f * xx - 2.f * yy;

    const float cy = sqrtf(m33 * m33 + m31 * m31);
    const float cx = atan2f(-m32, cy);
    if (cy > 16.f * FLT_EPSILON)
    {
        const float m12 = 2.f * x * y + 2.f * z * w;
        const float m22 = 1.f - 2.f * xx - 2.f * zz;

        return Vec3(cx, atan2f(m31, m33), atan2f(m12, m22));
    }
    else
    {
        const float m11 = 1.f - 2.f * yy - 2.f * zz;
        const float m21 = 2.f * x * y - 2.f * z * w;

        return Vec3(cx, 0.f, atan2f(-m21, m11));
    }
}

Mat4 Quat::ToMatrix() const
{
    const XMVECTOR quat = XMLoadFloat4(this);
    Mat4 out;
    XMStoreFloat4x4(&out, XMMatrixRotationQuaternion(quat));
    return out;
}

Quat Quat::FromAxisAngle(const Vec3& axis, float radians)
{
    const XMVECTOR axis_vec = XMLoadFloat3(&axis);
    Quat out;
    XMStoreFloat4(&out, XMQuaternionRotationAxis(axis_vec, radians));
    return out;
}

Quat Quat::FromPitchYawRoll(float pitch, float yaw, float roll)
{
    Quat out;
    XMStoreFloat4(&out, XMQuaternionRotationRollPitchYaw(pitch, yaw, roll));
    return out;
}

Quat Quat::FromPitchYawRoll(const Vec3& v)
{
    Quat out;
    XMStoreFloat4(&out, XMQuaternionRotationRollPitchYawFromVector(v));
    return out;
}

Quat Quat::FromRotationMatrix(const Mat4& m)
{
    const XMMATRIX mat = XMLoadFloat4x4(&m);
    Quat out;
    XMStoreFloat4(&out, XMQuaternionRotationMatrix(mat));
    return out;
}

Quat Quat::Lerp(const Quat& a, const Quat& b, float t)
{
    Quat out;
    // TODO
    CHECK(false);
    return out;
}

//////////////////////////////////////////////////////////////////////////

Box::Box(float in_min_x, float in_max_x, float in_min_y, float in_max_y, float in_min_z, float in_max_z) :
    min_x(in_min_x), max_x(in_min_x),
    min_y(in_min_y), max_y(in_min_y),
    min_z(in_min_z), max_z(in_min_z)
{
    center = CalculateCenter();
}

Box::Box(const std::vector<Vec3>& points)
{
    for (const Vec3& p : points)
    {
        min_x = std::min(p.x, min_x);
        max_x = std::max(p.x, max_x);
        min_y = std::min(p.y, min_y);
        max_y = std::max(p.y, max_y);
        min_z = std::min(p.z, min_z);
        max_z = std::max(p.z, max_z);
    }
    center = CalculateCenter();
}

Vec3 Box::CalculateCenter()
{
    const Vec3 center = { (min_x + max_x) * 0.5f, (min_y + max_y) * 0.5f, (min_z + max_z) * 0.5f };
    return center;
}
