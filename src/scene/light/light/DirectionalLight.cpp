/**
 * Author: Oldrin Barbulescu
 * Last modified: Jan 29, 2025
 **/

#include "DirectionalLight.h"



DirectionalLight::DirectionalLight(unsigned int id, const vec3& kWorldDirection,
	                               const vec3& kAmbientColor, const vec3& kDiffuseColor, const vec3& kSpecularColor,
	                               bool on) :
	                               BaseLight(id, kAmbientColor, kDiffuseColor, kSpecularColor, on) {
	try {
		setWorldDirection(kWorldDirection);
	}
	catch (const exception& kException) {
		throw runtime_error("DirectionalLight > " + string(kException.what()));
	}

	//cout << "Directional light " << id << " created." << endl;
}



DirectionalLight::~DirectionalLight() {
	//cout << "Directional light " << kId_ << " deleted." << endl;
}



void DirectionalLight::setWorldDirection(const vec3& kDirection) {
	if (glm::length(kDirection) <= BaseLight::kLengthEpsilon_)
		throw runtime_error("DirectionalLight.setWorldDirection|Invalid light direction value.");
	
	worldDirection_ = glm::normalize(kDirection);
	worldPosition_ = -worldDirection_;
	worldLookAt_ = vec3(0.0f);

	updateLightInEyeSpace_();
}



const vec3* DirectionalLight::getWorldDirection() const {
	return &worldDirection_;
}



const vec3* DirectionalLight::getEyeDirection() const {
	return &eyeDirection_;
}



BaseLight::LightType DirectionalLight::getType() const {
	return BaseLight::LightType::DIRECTIONAL;
}
