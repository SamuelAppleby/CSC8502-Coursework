#version 330 core
uniform sampler2D sceneTex;
uniform float offset;
in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColor;

void main(void) {
	vec2 texcoord = IN.texCoord;
	texcoord.x += sin(texcoord.y * 4 * 2 * 3.14159 + offset) / 100;
	fragColor = texture2D(sceneTex, texcoord);
}