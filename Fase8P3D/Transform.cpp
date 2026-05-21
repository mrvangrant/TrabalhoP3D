#include "Common.h" // Inclui definições comuns e macros para o motor de jogo
#include "Transform.h"
#include <glm/gtc/constants.hpp>


namespace game_engine_p3d {

	static float DegToRad(float deg) { return deg * glm::pi<float>() / 180.0f; }

	Transform::Transform(float x, float y, float z) {
		position_ = glm::vec3(x, y, z);
		orientation_ = glm::vec3(0.0f);
		orientation_quat_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		scale_ = glm::vec3(1.0f);
		// Rebuild matrix using current components
		matrix_ = glm::translate(glm::mat4(1.0f), position_) * glm::mat4_cast(orientation_quat_) * glm::scale(glm::mat4(1.0f), scale_);
	}

	Transform::Transform(float x, float y, float z, float pitch, float yaw, float roll,
		float scale_x, float scale_y, float scale_z)
	{
		position_ = glm::vec3(x, y, z);
		orientation_ = glm::vec3(pitch, yaw, roll);
		// create quaternion from Euler angles (convert degrees -> radians)
		glm::vec3 eulerRad = glm::radians(orientation_);
		// glm::quat accepts radians as Euler angles (x = pitch, y = yaw, z = roll) depending on GLM convention
		orientation_quat_ = glm::quat(eulerRad);
		scale_ = glm::vec3(scale_x, scale_y, scale_z);
		matrix_ = glm::translate(glm::mat4(1.0f), position_) * glm::mat4_cast(orientation_quat_) * glm::scale(glm::mat4(1.0f), scale_);
	}

	void Transform::Translate(float delta_x, float delta_y, float delta_z) {
		position_ += glm::vec3(delta_x, delta_y, delta_z);
		// rebuild matrix
		matrix_ = glm::translate(glm::mat4(1.0f), position_) * glm::mat4_cast(orientation_quat_) * glm::scale(glm::mat4(1.0f), scale_);
	}

	void Transform::Rotate(float delta_pitch, float delta_yaw, float delta_roll) {
		// accumulate Euler for compatibility
		orientation_ += glm::vec3(delta_pitch, delta_yaw, delta_roll);
		// convert delta to radians and build incremental quaternion
		glm::vec3 deltaRad = glm::radians(glm::vec3(delta_pitch, delta_yaw, delta_roll));
		glm::quat dq = glm::quat(deltaRad);
		orientation_quat_ = glm::normalize(dq * orientation_quat_);
		// rebuild matrix
		matrix_ = glm::translate(glm::mat4(1.0f), position_) * glm::mat4_cast(orientation_quat_) * glm::scale(glm::mat4(1.0f), scale_);
	}

	void Transform::Scale(float scale_x, float scale_y, float scale_z) {
		scale_ = glm::vec3(scale_x, scale_y, scale_z);
		matrix_ = glm::translate(glm::mat4(1.0f), position_) * glm::mat4_cast(orientation_quat_) * glm::scale(glm::mat4(1.0f), scale_);
	}

	void Transform::ApplyRolling(const glm::vec3& displacement, float radius, const glm::vec3& up) {
		// ignore vertical displacement for rolling on a plane defined by 'up'
		glm::vec3 disp = displacement;
		// remove the component along up (we only want tangential displacement)
		disp -= glm::dot(disp, up) * up;

		float dist = glm::length(disp);
		if (dist <= glm::epsilon<float>()) return;

		// axis of rotation is perpendicular to displacement and up
		glm::vec3 axis = glm::normalize(glm::cross(up, disp)); // cross(up, disp) produces right axis for rolling
		// angle (radians) = distance / radius (rolling without slipping)
		float angle = dist / radius;

		// create quaternion for rolling rotation
		glm::quat roll_q = glm::angleAxis(angle, axis);

		// apply rolling rotation: new orientation = roll_q * current
		orientation_quat_ = glm::normalize(roll_q * orientation_quat_);

		// update matrix
		matrix_ = glm::translate(glm::mat4(1.0f), position_) * glm::mat4_cast(orientation_quat_) * glm::scale(glm::mat4(1.0f), scale_);
	}

}