#version 330 core

layout (location = 0) out vec4 fColor; // Cor final do fragmento

void main()
{
	// Definindo a cor do fragmento como 'verde-escuro'
	fColor = vec4(0.0f, 0.58f, 0.02f, 1.0f);
}
