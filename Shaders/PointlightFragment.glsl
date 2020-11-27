#version 330 core
uniform sampler2D depthTex;
uniform sampler2D normTex;
uniform sampler2D shadowTex;

uniform vec2 pixelSize; // reciprocal of resolution
uniform vec3 cameraPos;

uniform float lightRadius;
uniform vec3 lightPos;
uniform vec4 lightColour;
uniform vec3 lightDirection;
uniform float lightAngle;
uniform mat4 inverseProjView;
uniform mat4 shadowMatrix;

out vec4 diffuseOutput;
out vec4 specularOutput;

void main (void) {
	vec2 texCoord = vec2(gl_FragCoord.xy * pixelSize);
	float depth = texture(depthTex, texCoord.xy).r;
	vec3 ndcPos = vec3(texCoord, depth) * 2.0 - 1.0;
	vec4 invClipPos = inverseProjView * vec4(ndcPos, 1.0);
	vec3 worldPos = invClipPos.xyz / invClipPos.w;

	vec3 incident;
	float attenuation;
	float theta = 0.0;
	if(lightRadius == 0) {	// Directional Light
		incident = normalize(-lightDirection);
		attenuation = 1.0;
	}
	else {		// Point Light or Spot Light
		incident = normalize(lightPos - worldPos);
		float distance = length(lightPos - worldPos);
		attenuation = 1.0 - clamp(distance / lightRadius, 0.0, 1.0);
		if(attenuation == 0.0) {
			discard;
		}
		if(lightAngle != 0) {		// Spotlight
			theta = acos(dot(incident, normalize(-lightDirection)));
			if(theta > lightAngle) {
				return;
			}
		}
	}
	vec3 normal = normalize(texture(normTex, texCoord.xy). xyz * 2.0 - 1.0);
	vec3 viewDir = normalize(cameraPos - worldPos);
	vec3 halfDir = normalize(incident + viewDir);

	float lambert = clamp(dot(incident, normal), 0.0, 1.0);
	float specFactor = clamp(dot(halfDir, normal), 0.0, 1.0);
	specFactor = pow(specFactor, 60.0);

	float shadow = 1.0;
	vec4 shadowNDC = shadowMatrix * (vec4(worldPos, 0.5) + vec4(normal, 0.5));
	shadowNDC /= shadowNDC.w;
	if(abs(shadowNDC.x) < 1.0f && abs(shadowNDC.y) < 1.0f && abs(shadowNDC.z) < 1.0f) {
		vec3 biasCoord = shadowNDC.xyz * 0.5f + 0.5f;
		float shadowZ = texture(shadowTex, biasCoord.xy).x;
		if(shadowZ < biasCoord.z) {
			shadow = 0.0f;
		}
	}
	vec3 attenuated = lightColour.xyz * attenuation;
	diffuseOutput = vec4(attenuated * lambert * shadow, 1.0);
	specularOutput = vec4(attenuated * specFactor * shadow * 0.33, 1.0);
}
