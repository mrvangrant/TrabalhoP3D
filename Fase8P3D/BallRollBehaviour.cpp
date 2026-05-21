#include "BallRollBehaviour.h"
#include "Transform.h" // for access to Transform members
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/constants.hpp>
#include "Object.h"

namespace game_engine_p3d {

	void BallRollBehaviour::Start(Object& obj) {
		// Initialize last_position_ with current model position
		last_position_ = obj.model().position_;
	}

	void BallRollBehaviour::PhysicsUpdate(Object& obj) {
		// Compute displacement produced by physics/collisions
		glm::vec3 current = obj.model().position_;
		glm::vec3 displacement = current - last_position_;
		// If there was movement, apply rolling
		if (glm::length(displacement) > glm::epsilon<float>()) {
			obj.model().ApplyRolling(displacement, radius_, glm::vec3(0.0f, 1.0f, 0.0f));
			last_position_ = current;
		}
	}

	void BallRollBehaviour::Update(Object& obj) {
		// Also handle movement that may occur in Update()
		glm::vec3 current = obj.model().position_;
		glm::vec3 displacement = current - last_position_;
		if (glm::length(displacement) > glm::epsilon<float>()) {
			obj.model().ApplyRolling(displacement, radius_, glm::vec3(0.0f, 1.0f, 0.0f));
			last_position_ = current;
		}
	}

}