#version 150 core
uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 fragColour;

void main (void) {
	fragColour = texture(diffuseTex, IN.texCoord) * ((IN.colour * 0.5) + 0.5);
}

