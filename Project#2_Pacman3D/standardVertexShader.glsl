#version 330 core

layout(location = 0) in vec3 vertexPosition;

uniform mat4 depthSpaceMatrix;

void main() {
	gl_Position = depthSpaceMatrix * vec4(vertexPosition, 1);
}