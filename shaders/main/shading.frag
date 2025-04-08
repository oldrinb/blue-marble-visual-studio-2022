/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 30, 2025
 **/

#ifdef NO_SHADING_MODE
flat in vec4 diffColor;
#elif defined (FLAT_SHADING_MODE)
flat in vec4 diffColor;
flat in vec3 specColor, emissColor;
#elif defined (GOURAUD_SHADING_MODE)
smooth in vec4 diffColor;
smooth in vec3 specColor, emissColor;
#endif

uniform bool hasDiffuseTexture, hasSpecularTexture, hasEmissiveTexture;
uniform bool diffuseCompressed, specularCompressed, emissiveCompressed;
uniform sampler2D diffuseTexSampler, specularTexSampler, emissiveTexSampler;

#ifdef NORMAL_MAPPING_MODE
const float EPSILON = 0.01f;

uniform bool hasNormalMapTexture, normalMapCompressed;
uniform sampler2D normalMapTexSampler;
#endif

void computeShading(vec3 position, vec3 normal, vec3 tangent, vec3 bitangent, vec3 tNormal,
	                out vec4 color, out vec3 specularColor, out vec3 emissiveColor);



// position -> fragment position in eye space
// normal -> normalized value in eye space
void computeShadingNormal(vec3 position, vec3 normal, out vec4 color, out vec3 specularColor, out vec3 emissiveColor) {
	#ifdef NO_SHADING_MODE
	color = diffColor;
	
	#elif defined(FLAT_SHADING_MODE) || defined(GOURAUD_SHADING_MODE)
	color = diffColor;
	specularColor = specColor;
	emissiveColor = emissColor;
	
	#elif defined (PHONG_SHADING_MODE)
	computeShading(position, normal, vec3(0.0f), vec3(0.0f), vec3(0.0f), color, specularColor, emissiveColor);
	#endif
}



// position -> fragment position in eye space
// normal -> normalized value in eye space
// tangent, bitangent -> normalized values in eye space, if compute light parameters in tangent space
//					  -> vec3(0.0f), if compute light parameters in eye space
// tNormal -> normalized normal in tangent space, if compute light parameters in tangent space
//		   -> vec3(0.0f), if compute light parameters in eye space
void computeShadingTangent(vec3 position, vec3 normal, vec3 tangent, vec3 bitangent, vec3 tNormal,
	                out vec4 color, out vec3 specularColor, out vec3 emissiveColor) {
	#ifdef NORMAL_MAPPING_MODE
	computeShading(position, normal, tangent, bitangent, tNormal, color, specularColor, emissiveColor);
	#endif
}



#ifdef NORMAL_MAPPING_MODE
vec3 getNormalInTangentSpace(vec2 texCoord, vec2 invTexCoord, float gamma) {
	vec3 normal;
	if(normalMapCompressed)
		normal = texture(normalMapTexSampler, invTexCoord).rgb;
	else {
		normal = texture(normalMapTexSampler, texCoord).rgb;
		normal = pow(normal, vec3(gamma));
	}

	if (length (normal) > EPSILON)
		return normalize (2.0f * normal - 1.0f);
	else return vec3(0.0f, 0.0f, 0.0f);
}
#endif



void textureMapping (vec2 texCoord, vec2 invTexCoord, float gamma, 
	                 inout vec4 color, inout vec3 specularColor, inout vec3 emissiveColor) {
	if (hasDiffuseTexture) {
		vec4 tex;
		if(diffuseCompressed)
			tex = texture(diffuseTexSampler, invTexCoord);
		else {
			tex = texture(diffuseTexSampler, texCoord);
			tex.rgb = pow(tex.rgb, vec3(gamma));
		}
		color *= tex;
	}

	if (hasSpecularTexture) {
		vec3 tex;
		if(specularCompressed)
			tex = texture(specularTexSampler, invTexCoord).rgb;
		else {
			tex = texture(specularTexSampler, texCoord).rgb;
			tex = pow(tex, vec3(gamma));
		}
		specularColor *= tex;
	}

	if (hasEmissiveTexture) {
		vec3 tex;
		if (emissiveCompressed) 
			tex = texture(emissiveTexSampler, invTexCoord).rgb;
		else {
			tex = texture(emissiveTexSampler, texCoord).rgb;
			tex = pow(tex, vec3(gamma));
		}

		emissiveColor *= tex;
	}	
}
