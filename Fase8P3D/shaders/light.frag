#version 440 core

uniform mat4 Model;
uniform mat4 View;
uniform mat4 ModelView;		// View * Model

uniform sampler2D TexSampler;

// Estrutura da fonte de luz ambiente global
struct AmbientLight {
	vec3 ambient;	// Componente de luz ambiente global
	bool enabled;	// Indica se a fonte de luz ambiente global está ativa ou não
};

uniform AmbientLight ambientLight; // Fonte de luz ambiente global

// Estrutura de uma fonte de luz direcional
struct DirectionalLight	{
	vec3 direction;		// Direção da luz, espaço do mundo
	
	vec3 ambient;		// Componente de luz ambiente
	vec3 diffuse;		// Componente de luz difusa
	vec3 specular;		// Componente de luz especular
	bool enabled;		// Indica se a fonte de luz direcional está ativa ou não
};

uniform DirectionalLight directionalLight; // Fonte de luz direcional

// Estrutura de uma fonte de luz pontual
struct PointLight	{
	vec3 position;		// Posição do ponto de luz, espaço do mundo
	
	vec3 ambient;		// Componente de luz ambiente
	vec3 diffuse;		// Componente de luz difusa
	vec3 specular;		// Componente de luz especular
	
	float constant;		// Coeficiente de atenuação constante
	float linear;		// Coeficiente de atenuação linear
	float quadratic;	// Coeficiente de atenuação quadrática

	bool enabled;		// Indica se a fonte de luz pontual está ativa ou não
};

uniform PointLight pointLight[2]; // Duas fontes de luz pontual

// Estrutura de uma fonte de luz cónica
struct SpotLight {
	vec3 position;		// Posição do foco de luz, espaço do mundo
	
	vec3 ambient;		// Componente de luz ambiente
	vec3 diffuse;		// Componente de luz difusa
	vec3 specular;		// Componente de luz especular
	
	float constant;		// Coeficiente de atenuação constante
	float linear;		// Coeficiente de atenuação linear
	float quadratic;	// Coeficiente de atenuação quadrática

	float spotCutoff, spotExponent;
	vec3 spotDirection;

	bool enabled;		// Indica se a fonte de luz cónica está ativa ou não
};

uniform SpotLight spotLight; // Fonte de luz cónica

struct Material{
	vec3 emissive;
	vec3 ambient;		// Ka
	vec3 diffuse;		// kd
	vec3 specular;		// ke
	float shininess;
};

uniform Material material;
vec3 diffuseColor;

in vec3 vPositionEyeSpace;
in vec3 vNormalEyeSpace;
in vec2 textureCoord;

layout (location = 0) out vec4 fColor; // Cor final do fragmento

vec4 calcAmbientLight(AmbientLight light);
vec4 calcDirectionalLight(DirectionalLight light, out vec4 ambient);
vec4 calcPointLight(PointLight light, out vec4 ambient);
vec4 calcSpotLight(SpotLight light, out vec4 ambient);

void main()
{
	// Cor do Material
	// Se exsitir uma textura, então a cor do material é a cor da textura.
	diffuseColor = texture(TexSampler, textureCoord).rgb;
	// Senão
	//diffuseColor = material.diffuse;

	// Cálculo da componente emissiva do material.
	vec4 emissive = vec4(material.emissive, 1.0);

	// Luz Ambiente Global
	vec4 ambient;

	// Cálculo do efeito da iluminação no fragmento.
	vec4 light[4];
	vec4 ambientTmp;
	// Contribuição da fonte de luz ambiente
	ambient = calcAmbientLight(ambientLight);
	// Contribuição da fonte de luz direcional
	light[0] = calcDirectionalLight(directionalLight, ambientTmp);
	ambient += ambientTmp;
	// Contribuição de cada fonte de luz Pontual
	for(int i=0; i<2; i++) {
		light[i+1] = calcPointLight(pointLight[i], ambientTmp);
		ambient += ambientTmp;
	}
	// Contribuição da fonte de luz cónica
	light[3] = calcSpotLight(spotLight, ambientTmp);
	ambient += ambientTmp;

	// Afetar a componente ambiente pela cor do material
	ambient *= vec4(diffuseColor,1.0);

	// Cálculo da cor final do fragmento.
	// ----------------------------------------------------------------
	// Combinar luzes de forma mais realista
	vec4 directLighting = vec4(0.0);

	for(int i = 0; i < 4; i++) {
		directLighting += light[i];
	}

	// Combinar com ambiente de forma mais suave
	vec4 finalAmbient = ambient; // Reduzir ainda mais a contribuição ambiente
	fColor = emissive + finalAmbient + directLighting;
	fColor = clamp(fColor, 0.0, 1.0);
}

// Cálculo da contribuição da fonte de luz ambiente global para a cor final do fragmento.
vec4 calcAmbientLight(AmbientLight light) {

	if(!light.enabled)
		return vec4(0.0);

	vec4 ambient = vec4(material.ambient * light.ambient, 1.0);
	return ambient;
}

// Cálculo da contribuição da fonte de luz direcional para a cor final do fragmento.
vec4 calcDirectionalLight(DirectionalLight light, out vec4 ambient) {

	if(!light.enabled) {
		ambient = vec4(0.0);
		return vec4(0.0);
	}

	ambient = vec4(material.ambient * light.ambient, 1.0);

	// Cálculo da reflexão da componente da luz difusa.
	vec3 lightDirectionEyeSpace = (View * vec4(light.direction, 0.0)).xyz;
	vec3 L = normalize(-lightDirectionEyeSpace); // Direção inversa à da direção luz. Isto é, um vetor que aponta da superfície do objeto em direção à fonte de luz.
	vec3 N = normalize(vNormalEyeSpace);
	float NdotL = max(dot(N, L), 0.0);
	vec4 diffuse = vec4(diffuseColor * light.diffuse, 1.0) * NdotL;
	
	// Cálculo da reflexão da componente da luz especular.
	// Como os cálculos estão a ser realizados nas coordenadas do olho, então a câmara está na posição (0,0,0).
	// Resulta então um vetor V entre os pontos (0,0,0) e vPositionEyeSpace:
	//		V = (0,0,0) - vPositionEyeSpace = (0-vPositionEyeSpace.x, 0-vPositionEyeSpace.y, 0-vPositionEyeSpace.z)
	// Nota que multiplicar o vetor de posição da câmara pela matriz View resulta no vetor (0,0,0).
	// Que pode ser simplificado como:
	//		- vPositionEyeSpace
	vec4 specular = vec4(0.0);

	if(NdotL > 0.0)
	{
		vec3 V = normalize(-vPositionEyeSpace);

		vec3 R = reflect(-L, N);

		float RdotV = max(dot(R, V), 0.0);

		specular = pow(RdotV, material.shininess) * vec4(light.specular * material.specular, 1.0);
	}
		// Cálculo da contribuição da fonte de luz direcional para a cor final do fragmento.
	return (diffuse + specular);
}

// Cálculo da contribuição da fonte de luz direcional para a cor final do fragmento.
vec4 calcPointLight(PointLight light, out vec4 ambient) {

	if(!light.enabled) {
		ambient = vec4(0.0);
		return vec4(0.0);
	}

	ambient = vec4(material.ambient * light.ambient, 1.0);

	// Cálculo da reflexão da componente da luz difusa.
	//************************************************************************
	// Atenção: vec3 lightPositionEyeSpace = mat3(View) * light.position; é diferente de vec3 lightPositionEyeSpace = (View * vec4(light.position, 1.0)).xyz;
	//************************************************************************
	vec3 lightPositionEyeSpace = (View * vec4(light.position, 1.0)).xyz;
	vec3 L = normalize(lightPositionEyeSpace - vPositionEyeSpace);
	vec3 N = normalize(vNormalEyeSpace);
	float NdotL = max(dot(N, L), 0.0);
	vec4 diffuse = vec4(diffuseColor * light.diffuse, 1.0) * NdotL;

	// Cálculo da reflexão da componente da luz especular.
	// Como os cálculos estão a ser realizados nas coordenadas do olho, então a câmara está na posição (0,0,0).
	// Resulta então um vetor V entre os pontos (0,0,0) e vPositionEyeSpace:
	//		V = (0,0,0) - vPositionEyeSpace = (0-vPositionEyeSpace.x, 0-vPositionEyeSpace.y, 0-vPositionEyeSpace.z)
	// Nota que multiplicar o vetor de posição da câmara pela matriz View resulta no vetor (0,0,0).
	// Que pode ser simplificado como:
	//		- vPositionEyeSpace
	vec3 V = normalize(-vPositionEyeSpace);
	//vec3 H = normalize(L + V);	// Modelo Blinn-Phong
	vec3 R = reflect(-L, N);
	float RdotV = max(dot(R, V), 0.0);
	//float NdotH = max(dot(N, H), 0.0);	// Modelo Blinn-Phong
	vec4 specular = pow(RdotV, material.shininess) * vec4(light.specular * material.specular, 1.0);
	
	// attenuation
	float dist = length(lightPositionEyeSpace - vPositionEyeSpace);	// Cálculo da distância entre o ponto de luz e o vértice
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

	// Cálculo da contribuição da fonte de luz pontual para a cor final do fragmento.
	return (attenuation * (diffuse + specular));
}

// Cálculo da contribuição da fonte de luz cónica para a cor final do fragmento.
vec4 calcSpotLight(SpotLight light, out vec4 ambient) 
{
	if(!light.enabled) {
		ambient = vec4(0.0);
		return vec4(0.0);
	}

	ambient = vec4(material.ambient * light.ambient, 1.0);

	// Cálculo da reflexão da componente da luz difusa.
	vec3 lightPositionEyeSpace = (View * vec4(light.position, 1.0)).xyz;
	vec3 L = normalize(lightPositionEyeSpace - vPositionEyeSpace);
	vec3 N = normalize(vNormalEyeSpace);
	float NdotL = max(dot(N, L), 0.0);
	vec4 diffuse = vec4(diffuseColor * light.diffuse, 1.0) * NdotL;

	// Cálculo da reflexão da componente da luz especular.
	// Como os cálculos estão a ser realizados nas coordenadas do olho, então a câmara está na posição (0,0,0).
	// Resulta então um vetor V entre os pontos (0,0,0) e vPositionEyeSpace:
	//		V = (0,0,0) - vPositionEyeSpace = (0-vPositionEyeSpace.x, 0-vPositionEyeSpace.y, 0-vPositionEyeSpace.z)
	// Nota que multiplicar o vetor de posição da câmara pela matriz View resulta no vetor (0,0,0).
	// Que pode ser simplificado como:
	//		- vPositionEyeSpace
	vec3 V = normalize(-vPositionEyeSpace);
	//vec3 H = normalize(L + V);	// Modelo Blinn-Phong
	vec3 R = reflect(-L, N);
	float RdotV = max(dot(R, V), 0.0);
	//float NdotH = max(dot(N, H), 0.0);	// Modelo Blinn-Phong
	vec4 specular = pow(RdotV, material.shininess) * vec4(light.specular * material.specular, 1.0);

	// SPOT EXPONENT
	// Effective light intensity is attenuated by the cosine of the angle between the direction of the light and the direction from the light to the vertex being lighted, raised to the power of the spot exponent.
	// Thus, higher spot exponents result in a more focused light source, regardless of the spot cutoff angle.
	// SPOT CUTOFF
	// Is a single integer or floating-point value that specifies the maximum spread angle of a light source.
	// Only values in the range 0 90 are accepted.
	// If the angle between the direction of the light and the direction from the light to the vertex being lighted is greater than the spot cutoff angle, the light is completely masked.
	// Otherwise, its intensity is controlled by the spot exponent and the attenuation factors.
	float spotIntensity;
	float spotCos = dot(L, normalize((View * vec4(-light.spotDirection, 0.0)).xyz)); // Cosseno do ângulo definido entre o vetor L (da fonte de luz ao fragmento) e o vetor (invertido) da direção do foco de luz.
	if(acos(spotCos) > radians(light.spotCutoff)) {
		spotIntensity = 0.0f;
	}
	else {
		spotIntensity = pow(spotCos, light.spotExponent);
	}
	diffuse *= spotIntensity;
	specular *= spotIntensity;

	// attenuation
	float dist = length(lightPositionEyeSpace - vPositionEyeSpace);	// Cálculo da distância entre o ponto de luz e o vértice
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

	// Cálculo da contribuição da fonte de luz pontual para a cor final do fragmento.
	return (attenuation * (diffuse + specular));
}