#version 150 core
uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main (void) {
	fragColour = texture(diffuseTex, IN.texCoord);
	if(fragColour.r  < 0.1 && fragColour.g < 0.1 && fragColour.b < 0.1) {
		discard;
	}
}