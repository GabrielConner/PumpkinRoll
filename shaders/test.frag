#version 460 core

in vec4 vertColor;

out vec4 fragColor;

uniform vec4 color;

void main() {
  fragColor = color;
}