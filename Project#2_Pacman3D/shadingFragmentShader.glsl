#version 330 core

in vec3 colorFrag;

out vec3 color;

void main() {
	vec3 materialDiffuseColor = colorFrag;
	vec3 materialAmbientColor = vec3(0.3, 0.3, 0.3) * colorFrag;
	vec3 materialSpecularColor = vec3(0.3, 0.3, 0.3);

	color = materialAmbientColor + materialDiffuseColor + materialSpecularColor;
}