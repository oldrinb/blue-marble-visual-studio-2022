/**
 * Author: Oldrin Barbulescu
 * Last modified: Feb 3, 2025
 **/

struct LIGHT {
  vec3 eDirection;
  vec3 ambientColor, diffuseColor, specularColor;
};

struct MATERIAL {
  uint shadingModel;
  vec3 ambientColor, diffuseColor, specularColor, emissiveColor;
  float shininess;
  float opacity;
};

struct SCENE {
  bool ambientLightOn, diffuseLightOn, specularLightOn, emissiveLightOn;	
};

uniform LIGHT light;
uniform MATERIAL material;
uniform SCENE scene;
