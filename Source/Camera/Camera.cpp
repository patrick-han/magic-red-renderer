#include "Camera.h"
#include <Common/Math/Math.h>

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

    void Camera::process_keyboard_input(CameraMovementDirection direction, float cameraSpeed) 
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

    void Camera::process_mouse_movement(float xoffset, float yoffset, bool constrainPitch)
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
            update_camera_vectors();
        }
    }

    void Camera::adjust_fov(float scrollOffset) 
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

    Matrix4f Camera::get_view_matrix() 
    {
        // Return the view matrix which is just at lookAt matrix calculated from the cameras 3 main directional vectors
        Matrix4f view;
        glm::vec3 tempPos = glm::vec3(m_position.x, m_position.y, m_position.z);
        glm::vec3 tempFwd = glm::vec3(m_forward.x, m_forward.y, m_forward.z);
        glm::vec3 tempLocalUp = glm::vec3(m_localUp.x, m_localUp.y, m_localUp.z);
        view = glmToMat4(glm::lookAt(tempPos, tempPos + tempFwd, tempLocalUp));
        return view;
    }

    Vector3f Camera::get_world_position()
    {
        return m_position;
    }

    void Camera::freeze_camera() 
    {
        m_bAllowMovement = false;
    }

    void Camera::unfreeze_camera() 
    {
        m_bAllowMovement = true;
    }


    void Camera::update_camera_vectors()
    {
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