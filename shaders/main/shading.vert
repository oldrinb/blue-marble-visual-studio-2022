/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 30, 2025
 **/

#ifdef NO_SHADING_MODE
flat out vec4 diffColor;
#elif defined (FLAT_SHADING_MODE)
flat out vec4 diffColor;
flat out vec3 specColor, emissColor;
#elif defined (GOURAUD_SHADING_MODE)
smooth out vec4 diffColor;
smooth out vec3 specColor, emissColor;
#endif

void computeShading(vec3 position, vec3 normal, vec3 tangent, vec3 bitangent, vec3 tNormal,
	                out vec4 color, out vec3 specularColor, out vec3 emissiveColor);



// position -> vertex position in eye space
// normal -> not-normalized value in eye space
void computeShading(vec3 position, vec3 normal) {
	#ifdef NO_SHADING_MODE
	diffColor = vec4(material.diffuseColor, material.opacity);
	
	#elif defined(FLAT_SHADING_MODE) || defined(GOURAUD_SHADING_MODE)
	vec3 n = normalize (normal);
	computeShading(position, n, vec3(0.0f), vec3(0.0f), vec3(0.0f), diffColor, specColor, emissColor);
	#endif
}
