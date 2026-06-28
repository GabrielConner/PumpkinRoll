#version 460 core

out vec4 fragColor;

in vec4 fragPos;
uniform vec3 move;

const float gridCellSize = 0.05f;

void main() {
  vec2 p = fragPos.xz + move.xz;
  fragColor = (abs(p.x - floor(p.x)) < gridCellSize || abs(p.y - floor(p.y)) < gridCellSize) ? vec4(1,0,0,1) : vec4(0,0,0,0);
}