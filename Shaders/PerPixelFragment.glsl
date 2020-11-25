#version 330 core
uniform sampler2D diffuseTex;
uniform vec3 cameraPos;
uniform vec3 lightPos;
uniform vec4 lightColour;
uniform float lightRadius;
uniform vec3 lightDirection;
uniform float lightAngle;

in Vertex {
	vec3 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 fragColour;

void main (void) {
	fragColour = vec4(0, 0, 0, 0);
	vec4 diffuse = texture(diffuseTex, IN.texCoord);
	vec3 surface = (diffuse.rgb * lightColour.rgb);
	vec3 incident;
	float attenuation;
	float theta = 0.0;
	if(lightRadius == 0) {	// Directional Light
		incident = normalize(-lightDirection);
		attenuation = 1.0;
	}
	else {		// Point Light or Spot Light
		incident = normalize(lightPos - IN.worldPos);
		float distance = length(lightPos - IN.worldPos);
		attenuation = 1.0 - clamp(distance / lightRadius, 0.0, 1.0);
		if(lightAngle != 0) {		// Spotlight
			theta = acos(dot(incident, normalize(-lightDirection)));
			if(theta > lightAngle) {
				fragColour.rgb += surface * 0.1; // Ambient if outside spotlight
				return;
			}
		}
	}
	float lambert = max(dot(incident, IN.normal), 0.0f);
	vec3 viewDir = normalize(cameraPos - IN.worldPos);
	vec3 halfDir = normalize(incident + viewDir);

	float specFactor = clamp(dot(halfDir, IN.normal), 0.0, 1.0);
	specFactor = pow(specFactor, 60.0);

	fragColour.rgb = surface * lambert * attenuation;	// Diffuse
	fragColour.rgb += (lightColour.rgb * specFactor) * attenuation * 0.33;	// Specular
	fragColour.rgb = lightAngle == 0.0 ? fragColour.rgb + surface * 0.1 : fragColour.rgb + surface * attenuation;
	fragColour.a = diffuse.a;
}
