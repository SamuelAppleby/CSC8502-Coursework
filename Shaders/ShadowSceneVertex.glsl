#version 330 core
#define lightSize 12
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

uniform mat4 shadowMatrices[lightSize]; //a new uniform !
uniform vec3 lightPos[lightSize];
uniform mat4 joints [128];
uniform bool animated;
uniform bool reflected;
uniform bool signMove;

in vec3 position;
in vec3 colour;
in vec3 normal;
in vec4 tangent;
in vec2 texCoord;
in vec4 jointWeights;
in ivec4 jointIndices;

out Vertex {
	vec3 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	vec4 shadowProj[lightSize]; //a new value !
} OUT;

void main (void) {
	OUT.colour = colour;
	if(reflected || signMove) {
		OUT.texCoord = (textureMatrix * vec4 (texCoord, 0.0, 1.0)).xy;
	}
	else {
		OUT.texCoord = texCoord;
	}
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	vec3 wNormal = normalize(normalMatrix * normalize(normal));
	vec3 wTangent = normalize(normalMatrix * normalize(tangent.xyz));

	OUT.normal = wNormal;
	OUT.tangent = wTangent;
	OUT.binormal = cross(wNormal, wTangent) * tangent.w;

	vec4 worldPos = (modelMatrix * vec4(position, 1));
	OUT.worldPos = worldPos.xyz;	
	if(animated) {
		vec4 localPos = vec4 (position, 1.0f);
		vec4 skelPos = vec4 (0, 0, 0, 0);
		for (int i = 0; i < 4; ++ i ) {
			int jointIndex = jointIndices [i];
			float jointWeight = jointWeights [i];
			skelPos += joints [jointIndex] * localPos * jointWeight;
		}
		gl_Position = (projMatrix * viewMatrix * modelMatrix) * vec4 (skelPos.xyz , 1.0);
	}
	else {
		gl_Position = (projMatrix * viewMatrix) * worldPos;
	}
	for(int i = 0; i < lightSize; ++i) { 
		vec3 viewDir = normalize(lightPos[i] - worldPos.xyz);
		vec4 pushVal = vec4(OUT.normal, 0) * dot(viewDir, OUT.normal);
		OUT.shadowProj[i] = shadowMatrices[i] * (worldPos + pushVal);
	}
}
