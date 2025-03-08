#include "Camera.h"
#include <Common/Math/Math.h>
#include <Common/Log.h>

namespace MagicRed
{
    Camera::Camera(Vector3f _position, Vector3f _worldUp, Vector3f _front, float _yaw, float _pitch, float _fov, bool _allowMovement) :
        m_position(_position),
        m_worldUp(_worldUp.AsNormalized()),
        m_forward(_front.AsNormalized()),
        m_right(Cross(m_forward, m_localUp).AsNormalized()),
        m_localUp(Cross(m_right, m_forward).AsNormalized()),
        m_yaw(_yaw),
        m_pitch(_pitch),
        m_fov(_fov),
        m_bAllowMovement(_allowMovement)
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
                m_position -= Cross(m_forward, m_localUp).AsNormalized() * cameraSpeed;
            }
            if (direction == CameraMovementDirection::RIGHT)
            {
                m_position += Cross(m_forward, m_localUp).AsNormalized() * cameraSpeed;
            }
            if (direction == CameraMovementDirection::UP)
            {
                m_position += cameraSpeed * m_worldUp;
            }
            if (direction == CameraMovementDirection::DOWN)
            {
                m_position += cameraSpeed * -1.0f * m_worldUp;
            }
        }
    }

    void Camera::AdjustFov(float scrollOffset) 
    {
        m_fov -= (float)scrollOffset;
        if (m_fov < 1.0f)
        {
            m_fov = 1.0f;
        }

        if (m_fov > 45.0f)
        {
            m_fov = 45.0f;
        }
    }

    void Camera::Rotate(float xoffset, float yoffset, bool constrainPitch)
    {
        if(m_bAllowMovement) 
        {
            // Update camera pitch and yaw based on mouse offsets calculated in the mouse callback
            m_yaw += xoffset;
            m_pitch += yoffset;

            if (constrainPitch) 
            {
                if (m_pitch > 89.0f) // Prevent weird flipping when looking at exactly 90 degrees (messes with lookat)
                {
                    m_pitch = 89.0f;
                }
                if (m_pitch < -89.0f)
                {
                    m_pitch = -89.0f;
                }
            }
            // Update the direction the camera is looking at based on the camera yaw and pitch
            Vector3f direction; // Vector actually points towards camera from the looking position
            direction.x = std::cos(deg2rad(m_yaw)) * std::cos(deg2rad(m_pitch));
            direction.y = std::sin(deg2rad(m_pitch));
            direction.z = std::sin(deg2rad(m_yaw)) * std::cos(deg2rad(m_pitch));
            m_forward = direction.AsNormalized();
            // also re-calculate the right and up vector
            m_right = Cross(m_forward, m_worldUp).AsNormalized();  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
            m_localUp = Cross(m_right, m_forward).AsNormalized();
        }
    }

    Matrix4f Camera::GetView() 
    {
        auto glm2Vec3 = [](const Vector3f& v) -> glm::vec3 { return glm::vec3(v.x, v.y, v.z); };
        // Return the view matrix which is just at lookAt matrix calculated from the cameras 3 main directional vectors
        // Matrix4f view = glmToMat4(glm::lookAt(
        //       glm2Vec3(m_position)
        //     , glm2Vec3(m_position + m_forward)
        //     , glm2Vec3(m_localUp)
        // ));


        Matrix4f view2 = Matrix4f(
              m_right.x, m_localUp.x, -m_forward.x, m_position.x
            , m_right.y, m_localUp.y, -m_forward.y, m_position.y
            , m_right.z, m_localUp.z, -m_forward.z, m_position.z
            , 0.0f, 0.0f, 0.0f, 1.0f
        ).InvertedRigid();

        return view2;
    }

    Matrix4f Camera::GetProjection(float fovY, float width, float height, float near, float far) {
        float aspectRatio = width / height;
        float tanHalfFovy = std::tanf(deg2rad(fovY) / 2.0f);
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
        return projection * Matrix4f::MakeRotateX(deg2rad(-180.f));
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