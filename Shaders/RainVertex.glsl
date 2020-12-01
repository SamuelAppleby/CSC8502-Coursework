#version 330 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
in vec3 position;
in vec2 texCoord;

out Vertex {
	vec2 texCoord;
} OUT;

void main (void) {
	mat4 inverseRotation = inverse(viewMatrix);		// Camera facing quad only
		inverseRotation[3][0] = 0.0f;
		inverseRotation[3][1] = 0.0f;
		inverseRotation[3][2] = 0.0f;
	gl_Position = projMatrix * viewMatrix * modelMatrix * inverseRotation * vec4(position, 1.0);
	OUT.texCoord = (vec4(texCoord, 0.0, 1.0)).xy;
}
