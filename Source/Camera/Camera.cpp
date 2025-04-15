#include "Camera.h"
#include <Common/Math/Math.h>
#include <Common/Log.h>

namespace MagicRed
{
    Camera::Camera(Vector3f _position, Vector3f _worldUp, Vector3f _forward, float _fovY) :
        m_position(_position),
        m_worldUp(_worldUp.AsNormalized()),
        m_forward(_forward.AsNormalized()),
        m_right(Cross(m_forward, m_worldUp).AsNormalized()),
        m_localUp(Cross(m_right, m_forward).AsNormalized()),
        m_yaw(90.0f),
        m_pitch(0.0f),
        m_fovY(_fovY),
        m_bAllowMovement(true)
    {
    }

    void Camera::Move(CameraMovementDirection direction, float cameraSpeed) 
    {
        if(m_bAllowMovement) {
            if (direction == CameraMovementDirection::FORWARD)
            {
                m_position += cameraSpeed * m_forward;
            }
            if (direction == CameraMovementDirection::BACKWARD)
            {
                m_position -= cameraSpeed * m_forward;
            }
            if (direction == CameraMovementDirection::LEFT)
            {
                m_position -= cameraSpeed * m_right;
            }
            if (direction == CameraMovementDirection::RIGHT)
            {
                m_position += cameraSpeed * m_right;
            }
            if (direction == CameraMovementDirection::UP)
            {
                m_position += cameraSpeed * m_worldUp;
            }
            if (direction == CameraMovementDirection::DOWN)
            {
                m_position -= cameraSpeed * m_worldUp;
            }
        }
    }

    void Camera::AdjustFov(float scrollOffset) 
    {
        m_fovY -= (float)scrollOffset;
        if (m_fovY < 1.0f)
        {
            m_fovY = 1.0f;
        }

        if (m_fovY > 120.0f)
        {
            m_fovY = 120.0f;
        }
    }

    void Camera::Rotate(float xoffset, float yoffset, bool constrainPitch)
    {
        if(m_bAllowMovement) 
        {
            // Moving the mouse left results in a negative offset. We want a left mouse movement to be a CCW rotation when viewed from above according to RHR.
            // Subtracting a negative results in a positive increase (positive angle is CCW)
            m_yaw -= xoffset;
            m_pitch += yoffset;

            if (constrainPitch) 
            {
                if (m_pitch > 89.0f) // +90 is looking straight up
                {
                    m_pitch = 89.0f;
                }
                if (m_pitch < -89.0f)
                {
                    m_pitch = -89.0f;
                }
            }
            // Update the direction the camera is looking at based on the camera yaw and pitch
            Vector3f direction;
            direction.x = std::cos(deg2rad(m_yaw)) * std::cos(deg2rad(m_pitch));
            direction.y = std::sin(deg2rad(m_yaw)) * std::cos(deg2rad(m_pitch));
            direction.z = std::sin(deg2rad(m_pitch));
            m_forward = direction.AsNormalized();
            // also re-calculate the right and up vector
            m_right = Cross(m_forward, m_worldUp).AsNormalized();  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
            m_localUp = Cross(m_right, m_forward).AsNormalized();
        }
    }

    Matrix4f Camera::GetView() 
    {

        Matrix4f view = Matrix4f(
            m_right.x, m_forward.x, m_localUp.x, m_position.x
          , m_right.y, m_forward.y, m_localUp.y, m_position.y
          , m_right.z, m_forward.z, m_localUp.z, m_position.z
          , 0.0f, 0.0f, 0.0f, 1.0f
      ).InvertedRigid();

        return view;
    }

    Matrix4f Camera::GetProjection(float width, float height, float near, float far) {
        float aspectRatio = width / height;
        float tanHalfFovy = std::tanf(deg2rad(m_fovY) / 2.0f);
        (void)(far);
        // Projection matrix for a view space already in the same orientation as Vulkan clip space (+Z away, +X right, +Y down)
        Matrix4f projection = Matrix4f(
              1.0f / (aspectRatio * tanHalfFovy), 0.0f, 0.0f, 0.0f
            , 0.0f, 1.0f / (tanHalfFovy), 0.0f, 0.0f
            , 0.0f, 0.0f, far / (far-near), -(near*far) / (far-near)
            // , 0.0f, 0.0f, 1.0f, -near // Infinite far plane
            , 0.0f, 0.0f, 1.0f, 0.0f
        );

        // Flips from view space +Z towards the viewer, +X right, +Y up to clip space +Z away, +X right, +Y down
        // Essentially a 180 degree CW rotation about +X
        // return projection * Matrix4f::MakeRotateX(deg2rad(-180.f));

        // Rotates view space +Y away from the viewer, +X right, +Z up to clip space +Z away, +X right, +Y down
        // Essentially a 90 degree CW rotation about +X
        return projection * Matrix4f::MakeRotateX(deg2rad(90.f)); // I would think this should be -90 but only +90 gives me the right result...
    }

    Vector3f Camera::GetWorldPosition()
    {
        return m_position;
    }

    void Camera::Freeze() 
    {
        m_bAllowMovement = false;
    }

    void Camera::Unfreeze() 
    {
        m_bAllowMovement = true;
    }
}