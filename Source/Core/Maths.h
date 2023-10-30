#pragma once
#include <DirectXMath.h>
#include <DirectXMathMatrix.inl>

struct Vec2;
struct Vec3;
struct Vec4;
struct Mat4;
struct Quat;

static inline constexpr float PI = 3.141592653589793238463f;
static inline constexpr float PI_HALF = PI * 0.5f;

struct MathUtils
{
    static float DegToRad(float value)
    {
        return DirectX::XMConvertToRadians(value);
    }

    static float RadToDeg(float value)
    {
        return value * (180.0f / PI);
    }

    template <typename T>
    static inline T AlignToBytes(T value, size_t bytes)
    {
        const size_t byte_mask = bytes - 1;
        return static_cast<T>((static_cast<size_t>(value) + byte_mask) & ~byte_mask);
    }

    template <typename T>
    static inline bool IsAligned(T value, size_t bytes)
    {
        return 0 == (static_cast<size_t>(value) & (bytes - 1));
    }
};

//////////////////////////////////////////////////////////////////////////

struct Vec2 : public DirectX::XMFLOAT2
{
    Vec2() : DirectX::XMFLOAT2(0.0f, 0.0f) {}
    Vec2(float x, float y) : DirectX::XMFLOAT2(x, y) {}
    Vec2(DirectX::XMVECTOR v) { DirectX::XMStoreFloat2(this, v); }

    operator DirectX::XMVECTOR() const noexcept { return DirectX::XMLoadFloat2(this); }

    inline bool operator==(const Vec2& v) const
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(this);
        const XMVECTOR v2 = XMLoadFloat2(&v);
        return XMVector2Equal(v1, v2);
    }

    inline bool operator!=(const Vec2& v) const
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(this);
        const XMVECTOR v2 = XMLoadFloat2(&v);
        return XMVector2NotEqual(v1, v2);
    }

    static const Vec2 ZERO;
    static const Vec2 ONE;
};
MAKE_HASHABLE(Vec2, t.x, t.y);

//////////////////////////////////////////////////////////////////////////

struct Vec3 : public DirectX::XMFLOAT3
{
    Vec3() : DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f) {}
    Vec3(float f) : DirectX::XMFLOAT3(f, f, f) {}
    Vec3(float x, float y, float z) : DirectX::XMFLOAT3(x, y, z) {}
    Vec3(DirectX::XMVECTOR v) { DirectX::XMStoreFloat3(this, v); }

    operator DirectX::XMVECTOR() const { return DirectX::XMLoadFloat3(this); }

    inline bool operator==(const Vec3& v) const
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(this);
        const XMVECTOR v2 = XMLoadFloat3(&v);
        return XMVector3Equal(v1, v2);
    }

    inline bool operator!=(const Vec3& v) const
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(this);
        const XMVECTOR v2 = XMLoadFloat3(&v);
        return XMVector3NotEqual(v1, v2);
    }

    inline Vec3& operator+=(const Vec3& v)
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(this);
        const XMVECTOR v2 = XMLoadFloat3(&v);
        const XMVECTOR result = XMVectorAdd(v1, v2);
        XMStoreFloat3(this, result);
        return *this;
    }

    inline Vec3& operator-=(const Vec3& v)
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(this);
        const XMVECTOR v2 = XMLoadFloat3(&v);
        const XMVECTOR result = XMVectorSubtract(v1, v2);
        XMStoreFloat3(this, result);
        return *this;
    }

    inline Vec3& operator*=(const Vec3& v) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(this);
        const XMVECTOR v2 = XMLoadFloat3(&v);
        const XMVECTOR result = XMVectorMultiply(v1, v2);
        XMStoreFloat3(this, result);
        return *this;
    }

    inline Vec3& operator*=(float f) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(this);
        const XMVECTOR result = XMVectorScale(v1, f);
        XMStoreFloat3(this, result);
        return *this;
    }

    inline Vec3& operator/=(float f) noexcept
    {
        using namespace DirectX;
        CHECK(f != 0.0f);
        const XMVECTOR v1 = XMLoadFloat3(this);
        const XMVECTOR result = XMVectorScale(v1, 1.0f / f);
        XMStoreFloat3(this, result);
        return *this;
    }

    Vec3 Cross(const Vec3& v) const;
    float Dot(const Vec3& v) const;
    float Length() const;
    float LengthSquared() const;
    Vec3 Normalize();

    static Vec3 Normalize(const Vec3& v);

    static float Distance(const Vec3& v1, const Vec3& v2);
    static float DistanceSquared(const Vec3& v1, const Vec3& v2);

    static Vec3 Cross(const Vec3& v1, const Vec3& v2);
    static float Dot(const Vec3& v1, const Vec3& v2);
    static Vec3 Transform(const Vec3& v, const Quat& q);

    static const Vec3 ZERO;
    static const Vec3 ONE;
    static const Vec3 UP;
    static const Vec3 DOWN;
    static const Vec3 FORWARD;
    static const Vec3 BACKWARD;
    static const Vec3 RIGHT;
    static const Vec3 LEFT;
};
MAKE_HASHABLE(Vec3, t.x, t.y, t.z);

//////////////////////////////////////////////////////////////////////////

struct Vec4 : public DirectX::XMFLOAT4
{
    Vec4() : DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f) {}
    Vec4(float f) : DirectX::XMFLOAT4(f, f, f, f) {}
    Vec4(const Vec3& v, float w) : DirectX::XMFLOAT4(v.x, v.y, v.z, w) {}
    Vec4(float x, float y, float z, float w) : DirectX::XMFLOAT4(x, y, z, w) {}
    Vec4(DirectX::XMVECTOR v) { DirectX::XMStoreFloat4(this, v); }

    operator DirectX::XMVECTOR() const noexcept { return DirectX::XMLoadFloat4(this); }

    inline bool operator==(const Vec4& v) const
    {
        using namespace DirectX;
        XMVECTOR v1 = XMLoadFloat4(this);
        XMVECTOR v2 = XMLoadFloat4(&v);
        return XMVector4Equal(v1, v2);
    }

    inline bool operator!=(const Vec4& v) const
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(this);
        const XMVECTOR v2 = XMLoadFloat4(&v);
        return XMVector4NotEqual(v1, v2);
    }

    inline Vec4& operator+=(const Vec4& v)
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(this);
        const XMVECTOR v2 = XMLoadFloat4(&v);
        const XMVECTOR result = XMVectorAdd(v1, v2);
        XMStoreFloat4(this, result);
        return *this;
    }

    inline Vec4& operator-=(const Vec4& v)
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(this);
        const XMVECTOR v2 = XMLoadFloat4(&v);
        const XMVECTOR result = XMVectorSubtract(v1, v2);
        XMStoreFloat4(this, result);
        return *this;
    }

    inline Vec4& operator/=(float f) noexcept
    {
        using namespace DirectX;
        CHECK(f != 0.0f);
        const XMVECTOR v1 = XMLoadFloat4(this);
        const XMVECTOR result = XMVectorScale(v1, 1.0f / f);
        XMStoreFloat4(this, result);
        return *this;
    }

    static Vec4 Transform(const Vec4& v, const Quat& q);

    Vec4 Normalize();
    float Length() const;
    float LengthSquared() const;

    Vec3 xyz()
    {
        return { x, y, z };
    }

    static const Vec4 FORWARD;
};
MAKE_HASHABLE(Vec4, t.x, t.y, t.z, t.w);

//////////////////////////////////////////////////////////////////////////

struct Mat4 : public DirectX::XMFLOAT4X4
{
    Mat4() = default;
    Mat4(float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13, 
        float m20, float m21, float m22, float m23, 
        float m30, float m31, float m32, float m33)
        : DirectX::XMFLOAT4X4(m00, m01, m02, m03,
            m10, m11, m12, m13,
            m20, m21, m22, m23,
            m30, m31, m32, m33)
    {}

    Mat4(const DirectX::XMFLOAT4X4& other) { memcpy(this, &other, sizeof(DirectX::XMFLOAT4X4)); }
    Mat4(const DirectX::XMMATRIX& other) { DirectX::XMStoreFloat4x4(this, other); }

    Mat4(const Mat4&) = default;
    Mat4& operator=(const Mat4&) = default;

    Mat4(Mat4&&) = default;
    Mat4& operator=(Mat4&&) = default;

    Mat4& operator*=(const Mat4& other);

    operator DirectX::XMMATRIX() const { return DirectX::XMLoadFloat4x4(this); }

    void Decompose(Vec3& out_scaling, Quat& out_rotation, Vec3& out_translation) const;

    Mat4 Transpose() const;
    Mat4 Invert() const;

    static Mat4 Translation(const Vec3& v);
    static Mat4 Translation(float x, float y, float z);
    static Mat4 RotationX(float rad);
    static Mat4 RotationY(float rad);
    static Mat4 RotationZ(float rad);
    static Mat4 Scaling(float s);
    static Mat4 Scaling(const Vec3& v);
    static Mat4 Scaling(float x, float y, float z);
    static Mat4 SRT(const Vec3& scaling, const Quat& rotation, const Vec3& translation);

    static Mat4 LookAt(const Vec3& origin, const Vec3& target, const Vec3& up);
    static Mat4 PerspectiveFovLH(float fov_y_rad, float aspect_ratio, float near_z, float far_z);
    static Mat4 OrthographicLH(float view_width, float view_height, float near_z, float far_z);
    static Mat4 OrthographicLH(float view_left, float view_right, float view_bottom, float view_top, float near_z, float far_z);

    static const Mat4 IDENTITY;
};

//////////////////////////////////////////////////////////////////////////

struct Quat : public DirectX::XMFLOAT4
{
    Quat() : DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) {}
    Quat(float x, float y, float z, float w) : DirectX::XMFLOAT4(x, y, z, w) {}
    Quat(const Vec4& v) : DirectX::XMFLOAT4(v.x, v.y, v.z, v.w) {}
    explicit Quat(DirectX::XMVECTOR v) { DirectX::XMStoreFloat4(this, v); }

    inline bool operator==(const Quat& q) const
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(this);
        const XMVECTOR v2 = XMLoadFloat4(&q);
        return XMVector4Equal(v1, v2);
    }

    inline bool operator!=(const Quat& v) const
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(this);
        const XMVECTOR v2 = XMLoadFloat4(&v);
        return XMVector4NotEqual(v1, v2);
    }

    inline Quat& operator+= (const Quat& q)
    {
        using namespace DirectX;
        const XMVECTOR q1 = XMLoadFloat4(this);
        const XMVECTOR q2 = XMLoadFloat4(&q);
        XMStoreFloat4(this, XMVectorAdd(q1, q2));
        return *this;
    }

    inline Quat& operator-= (const Quat& q)
    {
        using namespace DirectX;
        const XMVECTOR q1 = XMLoadFloat4(this);
        const XMVECTOR q2 = XMLoadFloat4(&q);
        XMStoreFloat4(this, XMVectorSubtract(q1, q2));
        return *this;
    }

    inline Quat& operator*= (const Quat& q)
    {
        using namespace DirectX;
        const XMVECTOR q1 = XMLoadFloat4(this);
        const XMVECTOR q2 = XMLoadFloat4(&q);
        XMStoreFloat4(this, XMQuaternionMultiply(q1, q2));
        return *this;
    }

    inline Quat& operator*= (float f) noexcept
    {
        using namespace DirectX;
        const XMVECTOR q = XMLoadFloat4(this);
        XMStoreFloat4(this, XMVectorScale(q, f));
        return *this;
    }

    inline Quat& operator/= (const Quat& q)
    {
        using namespace DirectX;
        const XMVECTOR q1 = XMLoadFloat4(this);
        XMVECTOR q2 = XMLoadFloat4(&q);
        q2 = XMQuaternionInverse(q2);
        Quat out;
        XMStoreFloat4(&out, XMQuaternionMultiply(q1, q2));
        return out;
    }

    void Normalize();

    Quat Inverse();

    // Convert to euler angles (degrees, order: first around y-axis, then x-axis, then z-axis)
    Vec3 ToEuler() const;

    Mat4 ToMatrix() const;

    static Quat Normalize(const Quat& q);

    static Quat FromAxisAngle(const Vec3& axis, float radians);
    static Quat FromPitchYawRoll(float pitch, float yaw, float roll);
    static Quat FromPitchYawRoll(const Vec3& v);
    static Quat FromRotationMatrix(const Mat4& m);

    static Quat Lerp(const Quat& a, const Quat& b, float t);

    static const Quat IDENTITY;
};

inline Quat operator+ (const Quat& a, const Quat& b)
{
    using namespace DirectX;
    const XMVECTOR q1 = XMLoadFloat4(&a);
    const XMVECTOR q2 = XMLoadFloat4(&b);
    Quat out;
    XMStoreFloat4(&out, XMVectorAdd(q1, q2));
    return out;
}

inline Quat operator- (const Quat& a, const Quat& b)
{
    using namespace DirectX;
    const XMVECTOR q1 = XMLoadFloat4(&a);
    const XMVECTOR q2 = XMLoadFloat4(&b);
    Quat out;
    XMStoreFloat4(&out, XMVectorSubtract(q1, q2));
    return out;
}

inline Quat operator* (const Quat& a, const Quat& b)
{
    using namespace DirectX;
    const XMVECTOR q1 = XMLoadFloat4(&a);
    const XMVECTOR q2 = XMLoadFloat4(&b);
    Quat out;
    XMStoreFloat4(&out, XMQuaternionMultiply(q1, q2));
    return out;
}

inline Quat operator/ (const Quat& a, const Quat& b)
{
    using namespace DirectX;
    const XMVECTOR q1 = XMLoadFloat4(&a);
    XMVECTOR q2 = XMLoadFloat4(&b);
    q2 = XMQuaternionInverse(q2);
    Quat out;
    XMStoreFloat4(&out, XMQuaternionMultiply(q1, q2));
    return out;
}

Vec3 operator* (const Vec3& v, const Mat4& m);
Vec4 operator+ (const Vec4& v1, const Vec4& v2);
Vec4 operator- (const Vec4& v1, const Vec4& v2);
Vec4 operator* (const Vec4& v, const Mat4& m);
Mat4 operator* (const Mat4& mat_a, const Mat4& mat_b);

//////////////////////////////////////////////////////////////////////////

struct Box
{
    Box();

    Box(float in_min_x, float in_max_x,
        float in_min_y, float in_max_y,
        float in_min_z, float in_max_z);

    Box(const std::vector<Vec3>& points);

    float getWidth() { return std::abs(max_x - min_x); }
    float getHeight() { return std::abs(max_y - min_y); }
    float getDepth() { return std::abs(max_z - min_y); }

    Vec3 center = Vec3::ZERO;
    float min_x = std::numeric_limits<float>::max();
    float max_x = std::numeric_limits<float>::lowest();
    float min_y = std::numeric_limits<float>::max();
    float max_y = std::numeric_limits<float>::lowest();
    float min_z = std::numeric_limits<float>::max();
    float max_z = std::numeric_limits<float>::lowest();

private:
    Vec3 CalculateCenter();
};

struct Sphere
{
    Sphere(const Vec3& in_center, float in_radius) : center(in_center), radius(in_radius) {}

    Vec3 center = Vec3::ZERO;
    float radius = 0.0f;
};
