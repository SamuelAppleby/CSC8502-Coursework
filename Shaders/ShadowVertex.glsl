# version 330 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;
in vec4 jointWeights;
in ivec4 jointIndices;

uniform mat4 joints [128];
uniform float animated;
void main (void) {
	if(animated == 1.0) {
		vec4 localPos = vec4 (position, 1.0f);
		vec4 skelPos = vec4 (0, 0, 0, 0);
		for (int i = 0; i < 4; ++ i ) {
			int jointIndex = jointIndices[i];
			float jointWeight = jointWeights[i];
			skelPos += joints [jointIndex] * localPos * jointWeight;
		}
		gl_Position = (projMatrix * viewMatrix * modelMatrix) * vec4 (skelPos.xyz , 1.0);
	}
	else {
		gl_Position = (projMatrix * viewMatrix * modelMatrix) * vec4 (position , 1.0);
	}
}