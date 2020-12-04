#version 330 core
uniform sampler2D sceneTex;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColor;

void main() {
	fragColor = texture2D(sceneTex, IN.texCoord);
	float greyScale = fragColor.r * 0.3 + fragColor.g * 0.59 + fragColor.b * 0.11;
	fragColor = vec4(greyScale, greyScale, greyScale, 1.0);
}