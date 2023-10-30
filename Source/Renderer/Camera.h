#pragma once

class Camera
{
public:
    Camera() = default;
    Camera(Vec3 pos, float aspect_ratio, float fov, float near_clip, float far_clip);

    void LookAt(Vec3 target);

    void SetNearClip(float z);
    float GetNearClip() const
    {
        return near_clip_;
    }

    void SetFarClip(float z);
    float GetFarClip() const
    {
        return far_clip_;
    }

    void SetFov(float fov_in_rad);
    float GetFov() const 
    {
        return fov_;
    }

    void SetAspectRatio(float aspect_ratio);
    float GetAspectRatio() const
    {
        return aspect_ratio_;
    }

    const Mat4& GetProjection();
    void SetProjection(const Mat4& projection);

    const Mat4& GetView();
    void SetView(const Mat4& view);

    const Mat4& GetViewProjection();
    void SetViewProjection(const Mat4& vp);

    const Mat4& GetInvViewProjection();

    void SetPosition(const Vec3& pos)
    {
        pos_ = pos;
        is_view_dirty_ = true;
    }

    const Vec3& GetPosition() const
    {
        return pos_;
    }

    void UpdateMatrices();

    void Update();

    inline static const Vec3 DEFAULT_POS = Vec3::ZERO;
    inline static const float DEFAULT_ASPECT_RATIO = 16.0f / 9.0f;
    inline static const float DEFAULT_FOV = MathUtils::DegToRad(90.0f);
    inline static const float DEFAULT_NEAR_CLIP = 0.1f;
    inline static const float DEFAULT_FAR_CLIP = 100.0f;

private:
    void RecalculateProjection();
    void RecalculateView();
    void RecalculateViewProjection();

    Vec3 pos_ = Camera::DEFAULT_POS;
    Vec3 forward_ = Vec3::FORWARD;
    Vec3 up_ = Vec3::UP;
    Vec3 right_ = Vec3::RIGHT;

    float near_clip_ = Camera::DEFAULT_NEAR_CLIP;
    float far_clip_ = Camera::DEFAULT_FAR_CLIP;
    float fov_ = Camera::DEFAULT_FOV;
    float aspect_ratio_ = Camera::DEFAULT_ASPECT_RATIO;

    Mat4 view_;
    bool is_view_dirty_ = true;

    Mat4 projection_;
    bool is_projection_dirty_ = true;

    Mat4 view_projection_;
    Mat4 inv_view_projection_;
    bool is_view_projection_dirty_ = true;
};
