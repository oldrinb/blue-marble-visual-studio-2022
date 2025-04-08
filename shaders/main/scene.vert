/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 28, 2025
 **/
 
in vec3 mPosition, mNormal;
in vec2 vTexCoord;

smooth out vec3 ePosition, eNormal;
smooth out vec2 fTexCoord, fInvTexCoord;

#ifdef NORMAL_MAPPING_MODE
in vec3 mTangent, mBitangent;
smooth out vec3 eTangent, eBitangent;
#endif

uniform mat4 modelViewMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 normalMatrix;



void main() {
	gl_Position = modelViewProjectionMatrix * vec4(mPosition, 1.0f);	
	ePosition = (modelViewMatrix * vec4(mPosition, 1.0f)).rgb;
	eNormal = (normalMatrix * vec4(mNormal, 1.0f)).rgb;

	fTexCoord = vTexCoord;
	fInvTexCoord = vec2(vTexCoord.x, 1.0f - vTexCoord.y);

	#ifdef PHONG_SHADING_MODE
	
	#elif defined (NORMAL_MAPPING_MODE)
	eTangent = (normalMatrix * vec4(mTangent, 1.0f)).rgb;
	eBitangent = (normalMatrix * vec4(mBitangent, 1.0f)).rgb;
	#else
	computeShading(ePosition, eNormal);
	#endif
}
