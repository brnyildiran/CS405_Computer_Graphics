#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexture;
layout(location = 2) in vec3 vertexColor;
layout(location = 3) in vec3 vertexNormal;

out vec3 colorFrag;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

void main() {
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1);
	colorFrag = vertexColor;
}