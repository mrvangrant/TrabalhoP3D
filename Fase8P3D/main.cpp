#include <iostream>
#include <array>

#include "Game.h"
#include "Camera.h"
#include "Light.h"
#include "Object.h"
#include "Shader.h"
#include "Renderer.h" // Inclui a classe 'Renderer', que define o renderizador a associar a cada objeto do jogo


// ------------------------------------------------------------
// É aqui que se incluem as suas classes de comportamento personalizados
// ------------------------------------------------------------
#include "Oscilator.h" // Inclui a classe 'Oscilator', que define o comportamento do objeto oscilador


// Usar um namespace mais curto para facilitar a escrita do código, e.g., 'gep3d::Game' em vez de 'game_engine_p3d::Game'
namespace gep3d = game_engine_p3d;


//Função de callback para eventos de teclado(1,2,3,4)
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action != GLFW_PRESS) return;// Processa apenas eventos de pressionamento de tecla (ignora os eventos de liberação e repetição)
		
	if (key == GLFW_KEY_1) {
		if (gep3d::Game::instance()){
			gep3d::Game::instance()->ToggleLight(0); // Alterna a luz de índice 0 (Ambiente)
			LOG("Toggled light 0 (Ambient)---------------------------------------------------------------------------------------------------");
		}
	}
	else if (key == GLFW_KEY_2) {
		if (gep3d::Game::instance()) {
			gep3d::Game::instance()->ToggleLight(1); // Alterna a luz de índice 1 (Direcional)
			LOG("Toggled light 1 (Direcional)-------------------------------------------------------------------------------------------------");
		}
	}
	else if (key == GLFW_KEY_3) {
		if (gep3d::Game::instance()) {
			gep3d::Game::instance()->ToggleLight(2); // Alterna a luz de índice 2 (Pontual)
			LOG("Toggled light 2 (Pontual)-------------------------------------------------------------------------------------------------------");
		}
	}
	else if (key == GLFW_KEY_4) {
		if (gep3d::Game::instance()) {
			gep3d::Game::instance()->ToggleLight(3); // Alterna a luz de índice 3 (Conica)
			LOG("Toggled light 3 (Conica)-----------------------------------------------------------------------------------------------------------");
		}
	}
}


int main() {
	//using namespace gep3d;

	// NOTA: Ao definir para PT as definições regionais, alteramos a forma como o programa interpreta os números decimais (e.g., o separador decimal passa a ser a vírgula ',' em vez do ponto '.').
	//       Tal pode causar problemas na leitura de ficheiros de texto que contenham números decimais, como os shaders ou os modelos 3D (ficheiros .obj e outros).
	//       Por exemplo, se um shader ou modelo 3D contiver um número decimal como '0.5', o programa pode interpretar isso como '0,5' e não conseguir ler corretamente o valor, levando a erros de compilação do shader ou de carregamento do modelo.
	//       Assim, nas funções que realizam a leitura de ficheiros de texto que contenham números decimais, é importante garantir que o programa esteja a utilizar a locale correta.
	//       Devemos guardar a locale que estamos a utilizar no programa, e definir explicitamente a locale para "C" ou "en_US.UTF-8" (que usam o ponto como separador decimal) antes de ler os ficheiros de texto, e depois restaurar a locale original do programa.
	// Definições regionais (locale)
	try {
		// locale para português de Portugal
#ifdef __linux__
		std::locale::global(std::locale("pt_PT"));
#else
		std::locale::global(std::locale("pt-PT"));
#endif
	}
	catch (const std::exception& e) {
		std::cerr << "Erro ao definir locale: " << e.what() << std::endl;
	}

	// --------------------------------------------------
	// Cria uma instância do jogo
	// --------------------------------------------------
	gep3d::Game game(1200, 800);

	gep3d::WindowSystem::SetKeyCallback(keyCallback);

	// --------------------------------------------------
	// Preparação da(s) câmara(s)
	// --------------------------------------------------
	// Instancia a câmara
	gep3d::Camera camera;
	// Define a cor de fundo da câmara
	camera.set_background_color(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
	// Define a posição da câmara e o ponto de vista
	camera.LookAt(glm::vec3(0.0f, 10.0f, 30.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	// Define a projeção perspetiva da câmara
	camera.Prespective(30.0f, static_cast<float>(game.width()) / game.height(), 0.1f, 100.0f);
	//camera.Orthographic(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f); // Define a projeção ortográfica
	// Define a viewport da câmara
	camera.Viewport(game.width(), game.height(), 0, 0);
	// Adiciona layers à máscara de culling da câmara
	std::array<std::string, 3> layers = { "Default", "Environment", "UI" };
	for (const auto& layer : layers) {
		camera.AddLayerToCullingMask(layer); // Adiciona a layer à máscara de culling da câmara
	}

	// Adiciona a câmara ao jogo (a primeira câmara adicionada é considerada a "Main Camera")
	game.AddCamera(&camera);

	// --------------------------------------------------
	// Preparação da(s) luzes(es)
	// --------------------------------------------------
	// Cria uma luz ambiente
	gep3d::Light* ambient_light = new gep3d::Light(glm::vec3(5.0f, 5.0f, 5.0f)); // Cor da luz ambiente
	// Cria uma luz direcional (ex: sol)
	gep3d::Light* directional_light = new gep3d::Light(
		glm::vec3(0.0f, 0.0f, -1.0f),	// Direção da luz
		glm::vec3(0.5f, 0.5f, 0.5f),    // Componente ambiente
		glm::vec3(1.0f, 1.0f, 1.0f),    // Componente difusa
		glm::vec3(0.3f, 0.3f, 0.3f)     // Componente especular
	);
	// Cria uma luz pontual (ex: lâmpada)
	gep3d::Light* point_light = new gep3d::Light(
		glm::vec3(0.0f, 0.0f, -1.0f),	// Posição da luz
		glm::vec3(0.5f, 0.5f, 0.5f),    // Componente ambiente
		glm::vec3(1.0f, 1.0f, 1.0f),    // Componente difusa
		glm::vec3(0.5f, 0.5f, 0.5f),    // Componente especular
		1.0f,                           // Constante de atenuação
		0.09f,                          // Linear de atenuação
		0.032f                          // Quadrática de atenuação
	);
	// Cria uma luz cónica (spotlight)
	gep3d::Light* spot_light = new gep3d::Light(
		glm::vec3(0.0f, 6.0f, 3.0f),  // Posição da luz
		glm::vec3(0.0f, -0.5f, -4.0f),   // Direção da luz
		glm::vec3(0.0f, 0.0f, 0.0f),    // Componente ambiente
		glm::vec3(1.0f, 1.0f, 1.0f),    // Componente difusa
		glm::vec3(1.0f, 1.0f, 1.0f),    // Componente especular
		1.0f,                           // Constante de atenuação
		0.09f,                          // Linear de atenuação
		0.032f,                         // Quadrática de atenuação
		10.0f,							// CutOff (ângulo interno, em graus)
		20.0f							// OuterCutOff (ângulo externo, em graus)
	);
	// Adiciona a luz ambiente ao jogo
	game.AddLight(ambient_light);
	// Adiciona a luz direcional ao jogo
	game.AddLight(directional_light);
	// Adiciona a luz pontual ao jogo
	game.AddLight(point_light);
	// Adiciona a luz cónica ao jogo
	game.AddLight(spot_light);


	// --------------------------------------------------
	// Preparação do(s) programa(s) shader
	// --------------------------------------------------
	// Para um programa shader, indica os tipos de shaders que serão usados e os respetivos caminhos para os ficheiros de código shader
	std::vector<ShaderSource> sources = {
		{GL_VERTEX_SHADER, "light.vert"},
		{GL_FRAGMENT_SHADER, "light.frag"}
	};
	
	// Cria o programa shader (lê e compila os shaders de um programa shader, a partir dos ficheiros especificados em 'sources')
	// O nome do shader é opcional, mas pode ser útil para identificação
	

	Shader* shader = new Shader(sources, "DefaultShader");




	// --------------------------------------------------
	// Preparação do(s) renderizador(es)
	// --------------------------------------------------
	// Cria o renderizador com o shader especificado e o caminho do modelo 3D (ficheiro OBJ)
	Renderer* renderer = new Renderer(shader, "model.obj");

	Renderer* tablerrenderer = new Renderer(shader, "mesa_bilhar_texturizada.obj");

	// Criar renderizadores para as bolas
	Renderer* ballrenderer1 = new Renderer(shader, "Ball1.obj");
	Renderer* ballrenderer2 = new Renderer(shader, "Ball2.obj");
	Renderer* ballrenderer3 = new Renderer(shader, "Ball3.obj");
	Renderer* ballrenderer4 = new Renderer(shader, "Ball4.obj");
	Renderer* ballrenderer5 = new Renderer(shader, "Ball5.obj");
	Renderer* ballrenderer6 = new Renderer(shader, "Ball6.obj");
	Renderer* ballrenderer7 = new Renderer(shader, "Ball7.obj");
	Renderer* ballrenderer8 = new Renderer(shader, "Ball8.obj");
	Renderer* ballrenderer9 = new Renderer(shader, "Ball9.obj");
	Renderer* ballrenderer10 = new Renderer(shader, "Ball10.obj");
	Renderer* ballrenderer11 = new Renderer(shader, "Ball11.obj");
	Renderer* ballrenderer12 = new Renderer(shader, "Ball12.obj");
	Renderer* ballrenderer13 = new Renderer(shader, "Ball13.obj");
	Renderer* ballrenderer14 = new Renderer(shader, "Ball14.obj");
	Renderer* ballrenderer15 = new Renderer(shader, "Ball15.obj");

	// --------------------------------------------------
	// Preparação do(s) comportamento(s) do(s) objeto(s)
	// --------------------------------------------------
	// Cria uma instância do comportamento Oscilator
	Oscilator* oscilator = new Oscilator();

	// --------------------------------------------------
	// Preparação do(s) objeto(s) do jogo
	// --------------------------------------------------
	// Instancia um objeto do jogo
	// Cria um objeto com nome "Objecto (1)" e layer padrão ("" = "Default")
	// Atribui um comportamento 'oscilator' ao objeto, que será executado no ciclo de atualização do jogo
	// Atribui um renderizador ao objeto, que será usado para renderizar o objeto no jogo
	gep3d::Object* table = new gep3d::Object("Mesa", "", nullptr, tablerrenderer, 0.0f, -0.75f, -15.0f, 0.0f,90.0f,0.0f,15.0f,15.0f,15.0f);
	

	gep3d::Object* ball1 = new gep3d::Object("Ball1", "", nullptr, ballrenderer1, -5.5f, 0.0f, -4.0f);
	gep3d::Object* ball2 = new gep3d::Object("Ball2", "", nullptr, ballrenderer2, -5.0f, 0.0f, -4.3f);
	gep3d::Object* ball3 = new gep3d::Object("Ball3", "", nullptr, ballrenderer3, -0.5f, 0.0f, -6.0f);
	gep3d::Object* ball4 = new gep3d::Object("Ball4", "", nullptr, ballrenderer4, 1.5f, 0.0f, -8.0f);
	gep3d::Object* ball5 = new gep3d::Object("Ball5", "", nullptr, ballrenderer5, -2.5f, 0.0f, -10.0f);
	gep3d::Object* ball6 = new gep3d::Object("Ball6", "", nullptr, ballrenderer6, 5.5f, 0.0f, -10.0f);
	gep3d::Object* ball7 = new gep3d::Object("Ball7", "", nullptr, ballrenderer7, -1.5f, 0.0f, -13.0f);
	gep3d::Object* ball8 = new gep3d::Object("Ball8", "", nullptr, ballrenderer8, 3.5f, 0.0f, -13.0f);
	gep3d::Object* ball9 = new gep3d::Object("Ball9", "", nullptr, ballrenderer9, 1.5f, 0.0f, -17.0f);
	gep3d::Object* ball10 = new gep3d::Object("Ball10", "", nullptr, ballrenderer10, -3.5f, 0.0f, -18.0f);
	gep3d::Object* ball11 = new gep3d::Object("Ball11", "", nullptr, ballrenderer11, -0.5f, 0.0f, -21.0f);
	gep3d::Object* ball12 = new gep3d::Object("Ball12", "", nullptr, ballrenderer12, 4.5f, 0.0f, -21.0f);
	gep3d::Object* ball13 = new gep3d::Object("Ball13", "", nullptr, ballrenderer13, -3.5f, 0.0f, -24.0f);
	gep3d::Object* ball14 = new gep3d::Object("Ball14", "", nullptr, ballrenderer14, 3.5f, 0.0f, -25.0f);
	gep3d::Object* ball15 = new gep3d::Object("Ball15", "", nullptr, ballrenderer15, -0.5f, 0.0f, -27.0f);

	// --------------------------------------------------
	// Adiciona o(s) objeto(s) ao jogo
	// --------------------------------------------------
	game.AddObject(table);
	game.AddObject(ball1);
	game.AddObject(ball2);
	game.AddObject(ball3);
	game.AddObject(ball4);
	game.AddObject(ball5);
	game.AddObject(ball6);
	game.AddObject(ball7);
	game.AddObject(ball8);
	game.AddObject(ball9);
	game.AddObject(ball10);
	game.AddObject(ball11);
	game.AddObject(ball12);
	game.AddObject(ball13);
	game.AddObject(ball14);
	game.AddObject(ball15);
	// --------------------------------------------------
	// Inicia o loop do jogo
	// --------------------------------------------------
	game.Run();

	// --------------------------------------------------
	// Liberta a memória alocada para os recursos do jogo
	// --------------------------------------------------
	delete shader;		// Liberta a memória alocada para o shader
	delete renderer;	// Liberta a memória alocada para o renderizador
	delete oscilator;	// Liberta a memória alocada para o comportamento
	delete table;
	delete tablerrenderer;
	delete ball1;
	delete ballrenderer1;
	delete ball2;
	delete ballrenderer2;
	delete ball3;
	delete ballrenderer3;
	delete ball4;
	delete ballrenderer4;
	delete ball5;
	delete ballrenderer5;
	delete ball6;
	delete ballrenderer6;
	delete ball7;
	delete ballrenderer7;
	delete ball8;
	delete ballrenderer8;
	delete ball9;
	delete ballrenderer9;
	delete ball10;
	delete ballrenderer10;
	delete ball11;
	delete ballrenderer11;
	delete ball12;
	delete ballrenderer12;
	delete ball13;
	delete ballrenderer13;
	delete ball14;
	delete ballrenderer14;
	delete ball15;
	delete ballrenderer15;
	delete ambient_light;
	delete directional_light;
	delete point_light;
	delete spot_light;

	LOG("Exit!");

	return 0;
}

