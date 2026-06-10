#version 460 core

layout(location=0) in ivec2 pos;
layout(location=1) in vec2 uv;


out vec2 fragUV;

out vec2 fragPos;

uniform mat4 transform;


void main() {
		gl_Position = transform * vec4(pos, 0, 1);

		fragUV = uv;
		fragPos = vec2(gl_Position.xy);
}