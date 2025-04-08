/**
 * Author: Oldrin Barbulescu
 * Last modified: Feb 3, 2025
 **/

// constants defined also in Material.h
const uint NO_SHADING = 1u, GOURAUD = 3u, PHONG = 4u;

const float EPSILON = 0.01f;
const float TRANSITION_THRESHOLD = 0.2f;



// position -> vertex/fragment position in eye space
// normal -> normalized value in eye space
// tangent, bitangent -> normalized values in eye space, if compute light parameters in tangent space
//					  -> vec3(0.0f), if compute light parameters in eye space
// tNormal -> normalized normal in tangent space, if compute light parameters in tangent space
//		   -> vec3(0.0f), if compute light parameters in eye space
void computeLightParameters (vec3 position, vec3 normal, vec3 tangent, vec3 bitangent, vec3 tNormal,
							 out float NdotL, out float powNdotH) {
	vec3 L = -light.eDirection;	
	vec3 N = normal;
	
	#ifdef NORMAL_MAPPING_MODE
	bool tangentSpace = false;
	mat3 TBN = mat3(0.0f);

	if (length (tNormal) > 0.0f) {
		tangentSpace = true;
		TBN = mat3 (tangent.x, bitangent.x, normal.x,
					tangent.y, bitangent.y, normal.y,
					tangent.z, bitangent.z, normal.z); 
		N = tNormal;
	}
	
	NdotL = (tangentSpace) ?  max(0.0f, dot(N, TBN * L)) : max(0.0f, dot(N, L)); 	
	#else
	NdotL = max(0.0f, dot(N, L)); 
	#endif

	if (material.shadingModel == PHONG) {
		vec3 E = vec3(0.0f) - position;
		E = (length (E) > EPSILON) ? normalize(E) : vec3(0.0f); // surface -> eye

		vec3 H = L + E;
		H = (length(H) > EPSILON) ? normalize(H) : vec3(0.0f);
		
		#ifdef NORMAL_MAPPING_MODE
		float NdotH = (tangentSpace) ?  max(0.0f, dot(N, TBN * H)) : max(0.0f, dot(N, H));
		#else
		float NdotH = max(0.0f, dot(N, H));
		#endif
		
		powNdotH = (NdotL > 0.0f && material.shininess > 0.0f) ? pow(NdotH, material.shininess) : 0.0f;
	}
}



// position -> vertex/fragment position in eye space
// normal -> normalized value in eye space
// tangent, bitangent -> normalized values in eye space, if compute light in tangent space
//					  -> vec3(0.0f), if compute light in eye space
// tNormal -> normalized normal in tangent space, if compute light in tangent space
//		   -> vec3(0.0f), if compute light in eye space
void computeShading(vec3 position, vec3 normal, vec3 tangent, vec3 bitangent, vec3 tNormal,
	                out vec4 color, out vec3 specularColor, out vec3 emissiveColor) {
	specularColor = vec3(0.0f);
	emissiveColor = vec3(0.0f);

	if (material.shadingModel == NO_SHADING) {
		color = vec4(material.diffuseColor, material.opacity);
		return;
	}

	color = vec4(0.0f, 0.0f, 0.0f, material.opacity);

	float NdotL = 0.0f, powNdotH = 0.0f;
	computeLightParameters(position, normal, tangent, bitangent, tNormal, NdotL, powNdotH);	
	
	vec3 ambient = light.ambientColor;
	vec3 diffuse = light.diffuseColor * NdotL;
	vec3 specular = light.specularColor * powNdotH;	

	if (scene.ambientLightOn) color.rgb += ambient * material.ambientColor;
	if (scene.diffuseLightOn) color.rgb += diffuse * material.diffuseColor;
	color.rgb = min(color.rgb, 1.0f);

	if (material.shadingModel == PHONG && scene.specularLightOn)
		specularColor = specular * material.specularColor;

	if (scene.emissiveLightOn) emissiveColor = material.emissiveColor;	
	
	// transition between day and night
	NdotL = 0.0f, powNdotH = 0.0f;
	computeLightParameters(vec3(0.0f), normal, vec3(0.0f), vec3(0.0f), vec3(0.0f), NdotL, powNdotH);

	if (NdotL < TRANSITION_THRESHOLD) {
		float t = 1.0f / TRANSITION_THRESHOLD * NdotL;
		color.rgb = mix(vec3(0.0f), color.rgb, t);
		specularColor = mix(vec3(0.0f), specularColor, t);
		emissiveColor = mix(emissiveColor, vec3(0.0f), t);
	}
	else emissiveColor = vec3(0.0f);
}
