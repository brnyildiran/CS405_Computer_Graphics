#version 330 core

in vec3 positionFragWorldSpace;
in vec2 textureFrag;
in vec3 colorFrag;
in vec3 normalFragCameraSpace;
in vec3 eyeDirectionCameraSpace;
in vec3 lightDirectionCameraSpace;
in vec4 shadowCoordinates;

out vec3 color;

uniform sampler2D samplingTexture;
uniform sampler2DShadow shadowMap;

vec2 poissonDisk[16] = vec2[]( 
	vec2(-0.94201624, -0.39906216), 
	vec2(0.94558609, -0.76890725), 
	vec2(-0.094184101, -0.92938870), 
	vec2(0.34495938, 0.29387760), 
	vec2(-0.91588581, 0.45771432), 
	vec2(-0.81544232, -0.87912464), 
	vec2(-0.38277543, 0.27676845), 
	vec2(0.97484398, 0.75648379), 
	vec2(0.44323325, -0.97511554), 
	vec2(0.53742981, -0.47373420), 
	vec2(-0.26496911, -0.41893023), 
	vec2(0.79197514, 0.19090188), 
	vec2(-0.24188840, 0.99706507), 
	vec2(-0.81409955, 0.91437590), 
	vec2(0.19984126, 0.78641367), 
	vec2(0.14383161, -0.14100790) 
);

void main() {
	vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
	float lightPower = 1.0f;
	
	vec3 initColor;
	if (textureFrag.x == 0.0f && textureFrag.y == 0.0f) {
		initColor = colorFrag;
	} else {
		initColor = texture(samplingTexture, textureFrag).rgb;
	}

	vec3 materialDiffuseColor = initColor;
	vec3 materialAmbientColor = vec3(0.3, 0.3, 0.3) * materialDiffuseColor;
	vec3 materialSpecularColor = vec3(0.3, 0.3, 0.3);

	vec3 n = normalize(normalFragCameraSpace);
	vec3 l = normalize(lightDirectionCameraSpace);
	float cosTheta = clamp(dot(n, l), 0, 1);
	
	vec3 E = normalize(eyeDirectionCameraSpace);
	vec3 R = reflect(-l, n);
	float cosAlpha = clamp(dot(E, R), 0, 1);
	
	float visibility = 1.0;
	float bias = 0.005;
	for (int i = 0; i < 4; i++) {
		visibility -= 0.2 * (1.0 - texture(shadowMap, vec3(shadowCoordinates.xy + poissonDisk[i] / 700.0, (shadowCoordinates.z - bias) / shadowCoordinates.w)));
	}
	
	color = materialAmbientColor +
		visibility * materialDiffuseColor * lightColor * lightPower * cosTheta +
		visibility * materialSpecularColor * lightColor * lightPower * pow(cosAlpha, 5);
}