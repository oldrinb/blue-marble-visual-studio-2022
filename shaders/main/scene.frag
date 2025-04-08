/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 30, 2025
 **/

float GAMMA = 1.0f / 2.2f;

smooth in vec3 ePosition, eNormal;
smooth in vec2 fTexCoord, fInvTexCoord;

#ifdef NORMAL_MAPPING_MODE
smooth in vec3 eTangent, eBitangent;
#endif

out vec4 outputColor;



void main() {
	vec4 color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	vec3 specularColor = vec3(0.0f), emissiveColor = vec3(0.0f);
	vec3 normal = normalize (eNormal);
	
	#ifdef NO_SHADING_MODE
	material.shadingModel;
	#endif
	
	#ifdef NORMAL_MAPPING_MODE
	vec3 tangent = normalize (eTangent);
	vec3 bitangent = normalize (eBitangent);

	if (hasNormalMapTexture) {
		vec3 tNormal = getNormalInTangentSpace(fTexCoord, fInvTexCoord, GAMMA);
		computeShadingTangent(ePosition, normal, tangent, bitangent, tNormal, color, specularColor, emissiveColor);
	}
	else computeShadingTangent(ePosition, normal, vec3(0.0f), vec3(0.0f), vec3(0.0f), color, specularColor, emissiveColor);
	#else
	computeShadingNormal(ePosition, normal, color, specularColor, emissiveColor);
	#endif

	textureMapping (fTexCoord, fInvTexCoord, GAMMA, color, specularColor, emissiveColor);

	float opacity = color.a;
	outputColor = vec4(color.rgb + specularColor + emissiveColor, opacity);	
}
