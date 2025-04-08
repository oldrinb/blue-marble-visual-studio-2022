/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 4, 2021
 **/

const float EPSILON = 0.002f;

smooth in vec2 fTexCoord;
smooth in vec3 fColor;

out vec4 outputColor;

uniform sampler2D texSampler;
uniform bool inverseColor;


			 
void main() {
	bool isText = (texture(texSampler, fTexCoord).a >= 1.0f - EPSILON);
	if (inverseColor) isText = !isText;

	gl_FragDepth = 0.0f;
	outputColor = isText ? vec4(fColor, 0.0f) : vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
