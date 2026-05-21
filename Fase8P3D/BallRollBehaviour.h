#pragma once


// Behaviour that makes a spherical object visually roll when its position changes.
// It tracks the last world position and applies rolling based on the displacement.
#include "Behaviour.h"
#include <glm/glm.hpp>

namespace game_engine_p3d {

	class BallRollBehaviour : public Behaviour {
	public:
		// radius: sphere radius in world units (model radius * model scale)
		BallRollBehaviour(float radius) : radius_(radius) {}

		// Called when behaviour starts: capture initial position
		void Start(Object& obj) override;

		// Called during physics updates (preferred if collisions/physics run in PhysicsUpdate)
		void PhysicsUpdate(Object& obj) override;

		// Also handle regular Update in case movement happens there
		void Update(Object& obj) override;

	private:
		float radius_ = 0.5f;
		glm::vec3 last_position_ = glm::vec3(0.0f);
	};

}