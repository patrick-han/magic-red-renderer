#pragma once
#include <Common/Math/Matrix4f.h>
#include <Common/Math/Vector3f.h>
#include <Common/Math/Matrix4f.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
namespace MagicRed
{
	enum class CameraMovementDirection 
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
		UP,
		DOWN
	};

	inline Matrix4f glmToMat4(const glm::mat4& in) {
        return Matrix4f(
              in[0][0], in[1][0], in[2][0], in[3][0]
            , in[0][1], in[1][1], in[2][1], in[3][1]
            , in[0][2], in[1][2], in[2][2], in[3][2]
            , in[0][3], in[1][3], in[2][3], in[3][3]
        );
    }

	class Camera {
	public:
		Vector3f m_position;
		Vector3f m_worldUp;
		Vector3f m_forward;
		Vector3f m_right;
		Vector3f m_localUp;
		float m_yaw;
		float m_pitch;
		float m_fovY;
		bool m_bAllowMovement;
	public:
		Camera(Vector3f _position, Vector3f _worldUp, Vector3f _forward, float _fovY);

		void Move(CameraMovementDirection direction, float cameraSpeed);

		void Rotate(float xoffset, float yoffset, bool constrainPitch);

		void AdjustFov(float scrollOffset);

		Matrix4f GetView();
		Matrix4f GetProjection(float width, float height, float near, float far);
		Vector3f GetWorldPosition();

		void Freeze();
		void Unfreeze();
	};
}