#include "Camera.h"

#include "Core/Application.h"
#include "Core/Input.h"
#include "Renderer/GraphicsContext.h"

Camera::Camera(Vec3 pos, float aspect_ratio, float fov, float near_clip, float far_clip)
    : pos_(pos), aspect_ratio_(aspect_ratio), fov_(fov), near_clip_(near_clip), far_clip_(far_clip)
{
    Vec3 target = { 0.0f, 0.0f, 1.0f };
    LookAt(target);
    UpdateMatrices();
}

void Camera::LookAt(Vec3 target)
{
    forward_ = Vec3::Normalize(target - pos_);
    right_ = Vec3::Cross(Vec3::UP, forward_);
    is_view_dirty_ = true;
}

void Camera::SetNearClip(float z)
{
    near_clip_ = z;
    is_projection_dirty_ = true;
}

void Camera::SetFarClip(float z)
{
    far_clip_ = z;
    is_projection_dirty_ = true;
}

void Camera::SetFov(float fov_in_rad)
{
    fov_ = fov_in_rad;
    is_projection_dirty_ = true;
}

void Camera::SetAspectRatio(float aspect_ratio)
{
    if (aspect_ratio != aspect_ratio_)
    {
        aspect_ratio_ = aspect_ratio;
        is_projection_dirty_ = true;
        LOG("Aspect ratio set to {}", aspect_ratio_);
    }
}

const Mat4& Camera::GetProjection()
{
    if(is_projection_dirty_)
    {
        RecalculateProjection();
    }

    return projection_;
}

void Camera::SetProjection(const Mat4& projection)
{
    projection_ = projection;
}

const Mat4& Camera::GetView()
{
    if (is_view_dirty_)
    {
        RecalculateView();
    }

    return view_;
}

void Camera::SetView(const Mat4& view)
{
    view_ = view;
    is_view_dirty_ = true;
}

const Mat4& Camera::GetViewProjection()
{
    if (is_view_projection_dirty_)
    {
        RecalculateViewProjection();
    }

    return view_projection_;
}

void Camera::SetViewProjection(const Mat4& vp)
{
    view_projection_ = vp;
    is_view_projection_dirty_ = false;
}

const Mat4& Camera::GetInvViewProjection()
{
    if (is_view_projection_dirty_)
    {
        RecalculateViewProjection();
    }

    return inv_view_projection_;
}

void Camera::UpdateMatrices()
{
    if(is_view_dirty_)
    {
        RecalculateView();
    }

    if(is_projection_dirty_)
    {
        RecalculateProjection();
    }

    if(is_view_projection_dirty_)
    {
        RecalculateViewProjection();
    }
}

void Camera::Update()
{
    // Update viewport dependent data
    const D3D12_VIEWPORT& viewport = gfx::GetViewport();
    SetAspectRatio(viewport.Width / viewport.Height);

    if(input::IsButtonDown(input::Button::MOUSE_RIGHT) == false)
    {
        input::SetCursorVisibility(true);
        UpdateMatrices();
        return;
    }

    input::SetCursorVisibility(false);

    // Update rotation
    constexpr float rotation_velocity = 0.005f;
    Vec2 mouse_delta = input::GetMousePosDelta();
    if(mouse_delta.x != 0.0f || mouse_delta.y != 0.0f)
    {
        float pitch_delta = mouse_delta.y * rotation_velocity;
        float yaw_delta = mouse_delta.x * rotation_velocity;

        Quat rot_pitch = Quat::FromAxisAngle(right_, pitch_delta);
        Quat rot_yaw = Quat::FromAxisAngle(Vec3::UP, yaw_delta);
        Quat rot_combined = Quat::Normalize(rot_pitch * rot_yaw);

        forward_ = Vec3::Normalize(Vec3::Transform(forward_, rot_combined));
        right_ = Vec3::Cross(Vec3::UP, forward_);
        is_view_dirty_ = true;
    }

    // Update translation
    constexpr float max_translation_velocity = 2.0f;
    Vec3 movement_dir = Vec3::ZERO;
    if (input::IsKeyDown(SDL_KeyCode::SDLK_w))
    {
        movement_dir.z = 1.0f;
    }
    else if (input::IsKeyDown(SDL_KeyCode::SDLK_s))
    {
        movement_dir.z = -1.0f;
    }

    if (input::IsKeyDown(SDL_KeyCode::SDLK_d))
    {
        movement_dir.x = 1.0f;
    }
    else if (input::IsKeyDown(SDL_KeyCode::SDLK_a))
    {
        movement_dir.x = -1.0f;
    }

    if (input::IsKeyDown(SDL_KeyCode::SDLK_SPACE))
    {
        movement_dir.y = 1.0f;
    }
    else if (input::IsKeyDown(SDL_KeyCode::SDLK_LCTRL))
    {
        movement_dir.y = -1.0f;
    }

    if (movement_dir != Vec3::ZERO)
    {
        float delta_time = BaseApplication::Get()->GetTimestep();
        pos_ = pos_ + delta_time * max_translation_velocity * movement_dir.x * right_;
        pos_ = pos_ + delta_time * max_translation_velocity * movement_dir.y * up_;
        pos_ = pos_ + delta_time * max_translation_velocity * movement_dir.z * forward_;
        SetPosition(pos_);
    }

    UpdateMatrices();
}

void Camera::RecalculateProjection()
{
    projection_ = Mat4::PerspectiveFovLH(fov_, aspect_ratio_, near_clip_, far_clip_);
    is_projection_dirty_ = false;
    is_view_projection_dirty_ = true;
}

void Camera::RecalculateView()
{
    SetView(Mat4::LookAt(pos_, pos_ + forward_, up_));
    is_view_dirty_ = false;
    is_view_projection_dirty_ = true;
}

void Camera::RecalculateViewProjection()
{
    view_projection_ = view_ * projection_;
    inv_view_projection_ = view_projection_.Invert();
    is_view_projection_dirty_ = false;
}
