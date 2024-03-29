#version 330 core

in vec2 textureFrag;

out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main() {    
  color = vec4(textColor, 1.0) * vec4(1.0, 1.0, 1.0, texture(text, textureFrag).r);
}