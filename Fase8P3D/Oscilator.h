#pragma once


// ==============================================================
// Oscilator.h
// Descrição:   Declaração da classe Oscilator
// Autor:       Duarte Duque
// Data:        21/07/2025
// Versão:      1.0.0
// Dependências: Behaviour.h, Input.h
// Compilação:	g++ Oscilator.h -o Oscilator.o -lglew32s -lglfw3 -lopengl32
// Observações:
// Esta classe representa um comportamento de oscilador que pode ser aplicado a um objeto no jogo.
// Ela herda da classe Behaviour e implementa os métodos Start e Update para controlar o comportamento de oscilação.
// ==============================================================


#include <iostream>		// Para std::cout

#include "Game.h"		// Necessário para acessar o método Pause() do jogo associado ao objeto
#include "Behaviour.h"	// Necessário para herdar da classe Behaviour
#include "WindowSystem.h"	// Necessário para aceder às funções de entrada do teclado e rato


using namespace game_engine_p3d; // Usar o namespace do motor de jogo para evitar prefixos longos


class Oscilator : public Behaviour {
private:


	bool moving_ = false;

	glm::vec3 velocity_ = glm::vec3(5.0f, 0.0f, 0.0f);

	float ball_radius_ = 0.5f;

public:
	void Start(Object& object) override {

		LOG("[Oscilator] Start()");
	}

	void Update(Object& object) override {
		
		//Object* obj = object.game()->FindObjectByName("ball1"); // Encontra o objeto "Objecto (2)" no jogo

		//// Ativar animação
		//if (WindowSystem::GetKey(GLFW_KEY_SPACE) == true) {
		//	moving_ = true;


		//	LOG("[Oscilator] Space key pressed. sending ball.");


		//	
		//}

		//// Movimento
		//if (moving_) {

		//	// DeltaTime temporário
		//	float dt = 0.016f;

		//	// deslocamento deste frame
		//	glm::vec3 displacement = velocity_ * dt;

		//	// mover bola
		//	obj->model().Translate(
		//		displacement.x,
		//		displacement.y,
		//		displacement.z
		//	);
		//}


		auto mouse_pos = WindowSystem::GetMousePosition(); // Obtém a posição do rato
		LOG("[Oscilator] Mouse position: (" << mouse_pos.first << ", " << mouse_pos.second << ")");
	}
};
