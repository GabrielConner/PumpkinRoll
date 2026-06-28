#version 460 core
layout(location=0) in vec2 pos;

uniform mat4 proj;
uniform mat4 view;

out vec4 fragPos;
void main() {
  vec4 vertPos = vec4(pos.x, 0, pos.y, 1);
  fragPos = vertPos;

  gl_Position = proj * view * vertPos;
}