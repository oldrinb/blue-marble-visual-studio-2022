/**
 * Author: Oldrin Barbulescu
 * Last modified: Feb 8, 2020
 **/

in vec2 vPosition; 
in vec2 vTexCoord;
in vec3 vColor;

smooth out vec2 fTexCoord;
smooth out vec3 fColor;

uniform uvec2 windowSize;



void main() { 
	vec2 position = vPosition - windowSize / 2u;
	position /= windowSize / 2u;
	gl_Position = vec4(position, 0.0f, 1.0f);

	fTexCoord = vTexCoord; 
	fColor = vColor;
}
