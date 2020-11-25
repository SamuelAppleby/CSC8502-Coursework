#version 330 core
uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;
uniform sampler2D shadowTex; // NEW !
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
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	vec4 shadowProj; // New !
} IN;

out vec4 fragColour;void main (void) {
	fragColour = vec4(0, 0, 0, 0);
	vec4 diffuse = texture(diffuseTex, IN.texCoord);
	vec3 bumpNormal = texture (bumpTex, IN.texCoord).rgb;
	vec3 surface = (diffuse.rgb * lightColour.rgb);
	vec3 incident;
	float attenuation;
	float theta = 0.0;
	if(lightRadius == 0) {	// Directional Light
		incident = normalize(-lightDirection);
		attenuation = 10.0;
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
	mat3 TBN = mat3 (normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));
	bumpNormal = normalize(TBN * normalize(bumpNormal * 2.0 - 1.0));

	float lambert = max(dot(incident, bumpNormal), 0.0f);
	vec3 viewDir = normalize(cameraPos - IN.worldPos);
	vec3 halfDir = normalize(incident + viewDir);

	float specFactor = clamp(dot(halfDir, bumpNormal), 0.0, 1.0);
	specFactor = pow(specFactor, 60.0);
	float shadow = 1.0; // New !
	vec3 shadowNDC = IN.shadowProj.xyz / IN.shadowProj.w;
	if(abs(shadowNDC.x) < 1.0f && abs(shadowNDC.y) < 1.0f && abs(shadowNDC.z) < 1.0f) {
		vec3 biasCoord = shadowNDC * 0.5f + 0.5f;
		float shadowZ = texture(shadowTex, biasCoord.xy).x;
		if(shadowZ < biasCoord. z) {
			shadow = 0.0f;
		}
	}

	fragColour.rgb = surface * attenuation * lambert; // diffuse
	fragColour.rgb += (lightColour.rgb * attenuation * specFactor ) * 0.33;	// Specular
	fragColour.rgb *= shadow; // shadowing factor
	fragColour.rgb = lightAngle == 0.0 ? fragColour.rgb + surface * 0.1 : fragColour.rgb + surface * attenuation;
	fragColour.a = diffuse.a; // alpha
}