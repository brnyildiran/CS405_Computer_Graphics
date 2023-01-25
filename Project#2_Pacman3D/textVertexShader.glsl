#version 330 core

layout (location = 0) in vec4 aVertexTexture;

out vec2 textureFrag;

uniform mat4 projectionMatrix;

void main() {
	gl_Position = projectionMatrix * vec4(aVertexTexture.xy, 0.0, 1.0);
	textureFrag = aVertexTexture.zw;
}