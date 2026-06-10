#version 460 core

out vec4 fragColor;

in vec2 fragUV;
in vec2 fragPos;

uniform usampler2D bitmapTexture;

uniform vec4 textColor;

uniform vec4 area;

uniform float cutoff;

bool clipPos() {
	return fragPos.x < area.x || fragPos.y < area.y || fragPos.x > area.z || fragPos.y > area.w;
}


void main () {
		if (!clipPos()) {
				float val = texture(bitmapTexture, fragUV).x / 255.f;
				if (val == 0) {
						fragColor = vec4(0);
				} else {
						fragColor = textColor;
				}


				if (val > cutoff) {
						fragColor = textColor * val;
				}
		} else {
				fragColor = vec4(0);
		}
}