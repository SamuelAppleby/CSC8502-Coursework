#version 330 core
#define lightSize 11
uniform sampler2D diffuseTex;
uniform sampler2D bumpTex; 
uniform sampler2D shadowTex[lightSize]; 
uniform vec3 cameraPos;
uniform float shine;
uniform float iridescence;
uniform vec4 lightColour[lightSize];
uniform vec3 lightPos[lightSize];
uniform float lightRadius[lightSize];
uniform vec3 lightDirections[lightSize];
uniform float lightAngles[lightSize];
in Vertex {
	vec3 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent; 
	vec3 binormal;
	vec3 worldPos;
	vec4 shadowProj[lightSize];
} IN;

out vec4 fragColour;

void main (void) {
	fragColour = vec4(0, 0, 0, 0);
	vec4 diffuse = texture(diffuseTex, IN.texCoord);

	for(int i = 0; i < lightSize; ++i) {
		vec3 surface = (diffuse.rgb * lightColour[i].rgb);
		vec3 incident;
		float attenuation;
		float theta = 0.0;
		if(lightRadius[i] == 0 && lightAngles[i] == 0.0) {	// Directional Light
			incident = normalize(-lightDirections[i]);
			attenuation = 1.0;
		}
		else {		// Point Light or Spot Light
			incident = normalize(lightPos[i] - IN.worldPos);
			float distance = length(lightPos[i] - IN.worldPos);
			attenuation = 1.0 - clamp(distance / lightRadius[i], 0.0, 1.0);
			if(lightAngles[i] != 0) {		// Spotlight
				theta = acos(dot(incident, normalize(-lightDirections[i])));
				if(theta > lightAngles[i]) {
					fragColour.rgb += surface * 0.01; // Ambient if outside spotlight
					continue;
				}
			}
		}
		vec3 viewDir = normalize(cameraPos - IN.worldPos);
		vec3 halfDir = normalize(incident + viewDir);
		
		mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));
		vec3 bumpNormal = texture(bumpTex, IN.texCoord).rgb;
		bumpNormal = normalize(TBN * normalize(bumpNormal * 2.0 - 1.0));

		float lambert = max(dot(incident, bumpNormal), 0.0f);
		float specFactor = clamp(dot(halfDir, bumpNormal), 0.0, 1.0);
		specFactor = pow(specFactor, shine);

		float shadow = 1.0; 
		vec3 shadowNDC = IN.shadowProj[i].xyz / IN.shadowProj[i].w;
		if(lightRadius[i] == 0 && lightAngles[i] == 0.0) {		// Directional Light
			shadow = 1.0f;
		}
		else if(abs(shadowNDC.x) < 1.0f && abs(shadowNDC.y) < 1.0f && abs(shadowNDC.z) < 1.0f) {
			vec3 biasCoord = shadowNDC * 0.5f + 0.5f;
			float shadowZ = texture(shadowTex[i], biasCoord.xy).x;
			if(shadowZ < biasCoord.z) {
				shadow = 0.1f;
			}
		}
		vec3 temp = surface * lambert * attenuation + (lightColour[i].rgb * specFactor) * attenuation * iridescence;
		temp *= shadow;
		fragColour.rgb += temp;
		fragColour.rgb = lightAngles[i] == 0.0 ? fragColour.rgb + surface * 0.01 : fragColour.rgb + surface * attenuation;
		fragColour.a += diffuse.a;
	}
}
