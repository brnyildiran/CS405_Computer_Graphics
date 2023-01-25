#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexture;
layout(location = 2) in vec3 vertexColor;
layout(location = 3) in vec3 vertexNormal;

out vec3 positionFragWorldSpace;
out vec2 textureFrag;
out vec3 colorFrag;
out vec3 normalFragCameraSpace;
out vec3 eyeDirectionCameraSpace;
out vec3 lightDirectionCameraSpace;
out vec4 shadowCoordinates;

uniform mat4 spaceMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform vec3 lightInvDirectionWorldSpace;
uniform mat4 depthBiasSpaceMatrix;

void main(){
	gl_Position = spaceMatrix * vec4(vertexPosition, 1);
	shadowCoordinates = depthBiasSpaceMatrix * vec4(vertexPosition, 1);
	positionFragWorldSpace = (modelMatrix * vec4(vertexPosition, 1)).xyz;
	eyeDirectionCameraSpace = vec3(0.0f, 0.0f, 0.0f) - (viewMatrix * modelMatrix * vec4(vertexPosition, 1)).xyz;
	lightDirectionCameraSpace = (viewMatrix * vec4(lightInvDirectionWorldSpace, 0)).xyz;
	normalFragCameraSpace = (viewMatrix * modelMatrix * vec4(vertexNormal, 0)).xyz;
	textureFrag = vertexTexture;
	colorFrag = vertexColor;
}